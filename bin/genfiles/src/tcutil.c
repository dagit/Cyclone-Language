#include "cyc_include.h"

 struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;}; struct
_tuple1{ void* f1; struct _tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple4{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple5{ struct Cyc_Absyn_Tqual*
f1; void* f2;}; struct _tuple6{ struct Cyc_Absyn_Tvar* f1; void* f2;}; struct
_tuple7{ int f1; void* f2;}; struct _tuple8{ void* f1; void* f2;}; struct
_tuple9{ void* f1; struct Cyc_List_List* f2;}; struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2;}; struct _tuple11{ void* f1; void* f2; void* f3;};
struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2;}; struct
_tuple13{ struct _tuple12* f1; void* f2;}; struct _tuple14{ struct _tuple4* f1;
int f2;}; struct _tuple15{ struct Cyc_List_List* f1; void* f2;}; struct _tuple16{
struct _tuple4* f1; void* f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct
Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int); extern char
Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_revappend(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct _tuple0 Cyc_List_split(
struct Cyc_List_List* x); extern void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col;};
typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag;}; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct Cyc_List_List*
Cyc_Position_strings_of_segments( struct Cyc_List_List*); typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc;}; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag;}; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
typedef struct _tagged_string* Cyc_Absyn_field_name; typedef struct
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern int Cyc_Absyn_qvar_cmp(
struct _tuple1*, struct _tuple1*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern
void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_atb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct _tuple4* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct _tuple5*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct
Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern struct
Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag;}; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void
Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string); extern
void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_flush_warnings(); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_arithmetic_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_comparison_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern struct _tuple6* Cyc_Tcutil_make_inst_var(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp*); extern struct Cyc_List_List* Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*,
void* k, void*); extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment* loc, struct
Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t,
struct Cyc_Absyn_Exp* e); extern struct _tuple7 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); char Cyc_Tcutil_TypeErr_tag[ 8u]="TypeErr";
extern void Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr(
struct Cyc_Position_Segment* loc, struct _tagged_string s){ Cyc_Position_post_error(
Cyc_Position_mk_err_elab( loc, s));} void* Cyc_Tcutil_impos( struct
_tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Tcutil_TypeErr_struct*
_temp1=( struct Cyc_Tcutil_TypeErr_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_TypeErr_struct));*
_temp1=( struct Cyc_Tcutil_TypeErr_struct){.tag= Cyc_Tcutil_TypeErr_tag};(
struct _xenum_struct*) _temp1;}));} static struct _tagged_string Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs= tvs->tl){({ struct
_tagged_string _temp2= Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*) tvs->hd);
struct _tagged_string _temp3= Cyc_Absynpp_ckind2string((( struct Cyc_Absyn_Tvar*)
tvs->hd)->kind); fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ", _temp2.last_plus_one -
_temp2.curr, _temp2.curr, _temp3.last_plus_one - _temp3.curr, _temp3.curr);});}
fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp4=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp4->hd=( void*) sg; _temp4->tl= Cyc_Tcutil_warning_segs;
_temp4;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5->hd=( void*)(( struct
_tagged_string*)({ struct _tagged_string* _temp6=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6[ 0]= msg; _temp6;}));
_temp5->tl= Cyc_Tcutil_warning_msgs; _temp5;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;} fprintf( Cyc_Stdio_stderr,"***Warnings***\n");{
struct Cyc_List_List* seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp7=*(( struct
_tagged_string*) seg_strs->hd); struct _tagged_string _temp8=*(( struct
_tagged_string*) Cyc_Tcutil_warning_msgs->hd); fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp7.last_plus_one - _temp7.curr, _temp7.curr, _temp8.last_plus_one - _temp8.curr,
_temp8.curr);}); seg_strs= seg_strs->tl; Cyc_Tcutil_warning_msgs= Cyc_Tcutil_warning_msgs->tl;}
fprintf( Cyc_Stdio_stderr,"**************\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; void* Cyc_Tcutil_compress(
void* t){ void* _temp9= t; int _temp21; struct Cyc_Core_Opt* _temp23; void*
_temp25; int _temp27; struct Cyc_Core_Opt* _temp29; struct Cyc_Core_Opt**
_temp31; void* _temp32; struct Cyc_Core_Opt* _temp34; struct Cyc_List_List*
_temp36; struct _tuple1* _temp38; struct Cyc_Core_Opt* _temp40; struct Cyc_Core_Opt**
_temp42; struct Cyc_List_List* _temp43; struct _tuple1* _temp45; _LL11: if((
unsigned int) _temp9 > 5u?(( struct _enum_struct*) _temp9)->tag == Cyc_Absyn_Evar_tag:
0){ _LL26: _temp25=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp9)->f1; goto
_LL24; _LL24: _temp23=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; if( _temp23 == 0){ goto _LL22;} else{ goto _LL13;} _LL22: _temp21=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp9)->f3; goto _LL12;} else{ goto _LL13;}
_LL13: if(( unsigned int) _temp9 > 5u?(( struct _enum_struct*) _temp9)->tag ==
Cyc_Absyn_Evar_tag: 0){ _LL33: _temp32=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f1; goto _LL30; _LL30: _temp29=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; _temp31=&(( struct Cyc_Absyn_Evar_struct*) _temp9)->f2; goto _LL28;
_LL28: _temp27=( int)(( struct Cyc_Absyn_Evar_struct*) _temp9)->f3; goto _LL14;}
else{ goto _LL15;} _LL15: if(( unsigned int) _temp9 > 5u?(( struct _enum_struct*)
_temp9)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL39: _temp38=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp9)->f1; goto _LL37; _LL37: _temp36=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f2; goto
_LL35; _LL35: _temp34=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f3; if( _temp34 == 0){ goto _LL16;} else{ goto _LL17;}} else{ goto
_LL17;} _LL17: if(( unsigned int) _temp9 > 5u?(( struct _enum_struct*) _temp9)->tag
== Cyc_Absyn_TypedefType_tag: 0){ _LL46: _temp45=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f1; goto _LL44; _LL44: _temp43=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f2; goto _LL41; _LL41: _temp40=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f3; _temp42=&(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f3; goto
_LL18;} else{ goto _LL19;} _LL19: goto _LL20; _LL12: return t; _LL14: { void* t2=
Cyc_Tcutil_compress(( void*)(* _temp31)->v); if( t2 !=( void*)(* _temp31)->v){*
_temp31=({ struct Cyc_Core_Opt* _temp47=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp47->v=( void*) t2; _temp47;});} return t2;}
_LL16: return t; _LL18: { void* t2= Cyc_Tcutil_compress(( void*)(* _temp42)->v);
if( t2 !=( void*)(* _temp42)->v){* _temp42=({ struct Cyc_Core_Opt* _temp48=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp48->v=(
void*) t2; _temp48;});} return t2;} _LL20: return t; _LL10:;} static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp50=({ struct
_tuple8 _temp49; _temp49.f1= k1; _temp49.f2= k2; _temp49;}); void* _temp60; void*
_temp62; void* _temp64; void* _temp66; void* _temp68; void* _temp70; _LL52:
_LL63: _temp62= _temp50.f1; if( _temp62 == Cyc_Absyn_BoxKind){ goto _LL61;}
else{ goto _LL54;} _LL61: _temp60= _temp50.f2; if( _temp60 == Cyc_Absyn_MemKind){
goto _LL53;} else{ goto _LL54;} _LL54: _LL67: _temp66= _temp50.f1; if( _temp66
== Cyc_Absyn_BoxKind){ goto _LL65;} else{ goto _LL56;} _LL65: _temp64= _temp50.f2;
if( _temp64 == Cyc_Absyn_AnyKind){ goto _LL55;} else{ goto _LL56;} _LL56: _LL71:
_temp70= _temp50.f1; if( _temp70 == Cyc_Absyn_MemKind){ goto _LL69;} else{ goto
_LL58;} _LL69: _temp68= _temp50.f2; if( _temp68 == Cyc_Absyn_AnyKind){ goto
_LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL53: return 1; _LL55: return 1;
_LL57: return 1; _LL59: return 0; _LL51:;}} void* Cyc_Tcutil_typ_kind( void* t){
void* _temp72= Cyc_Tcutil_compress( t); int _temp114; struct Cyc_Core_Opt*
_temp116; void* _temp118; struct Cyc_Absyn_Tvar* _temp120; void* _temp122; void*
_temp124; struct Cyc_Absyn_FnInfo _temp126; void* _temp128; struct Cyc_Absyn_Enumdecl**
_temp130; struct Cyc_List_List* _temp132; struct _tuple1* _temp134; struct Cyc_Absyn_Xenumdecl**
_temp136; struct _tuple1* _temp138; struct Cyc_Absyn_Structdecl** _temp140;
struct Cyc_List_List* _temp142; struct _tuple1* _temp144; struct Cyc_Absyn_Structdecl**
_temp146; struct Cyc_List_List* _temp148; struct _tuple1* _temp150; struct Cyc_Absyn_PtrInfo
_temp152; struct Cyc_Absyn_Exp* _temp154; struct Cyc_Absyn_Tqual* _temp156; void*
_temp158; struct Cyc_List_List* _temp160; struct Cyc_Core_Opt* _temp162; struct
Cyc_List_List* _temp164; struct _tuple1* _temp166; void* _temp168; struct Cyc_List_List*
_temp170; _LL74: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_Evar_tag: 0){ _LL119: _temp118=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp72)->f1; goto _LL117; _LL117: _temp116=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp72)->f2; goto _LL115; _LL115: _temp114=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp72)->f3; goto _LL75;} else{ goto _LL76;} _LL76: if(( unsigned int) _temp72
> 5u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_VarType_tag: 0){ _LL121:
_temp120=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp72)->f1;
goto _LL77;} else{ goto _LL78;} _LL78: if( _temp72 == Cyc_Absyn_VoidType){ goto
_LL79;} else{ goto _LL80;} _LL80: if(( unsigned int) _temp72 > 5u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_IntType_tag: 0){ _LL125: _temp124=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp72)->f1; goto _LL123; _LL123:
_temp122=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp72)->f2; goto _LL81;}
else{ goto _LL82;} _LL82: if( _temp72 == Cyc_Absyn_FloatType){ goto _LL83;}
else{ goto _LL84;} _LL84: if( _temp72 == Cyc_Absyn_DoubleType){ goto _LL85;}
else{ goto _LL86;} _LL86: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_FnType_tag: 0){ _LL127: _temp126=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp72)->f1; goto _LL87;} else{ goto _LL88;}
_LL88: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL129: _temp128=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp72)->f1; goto _LL89;} else{ goto _LL90;} _LL90: if( _temp72 == Cyc_Absyn_HeapRgn){
goto _LL91;} else{ goto _LL92;} _LL92: if(( unsigned int) _temp72 > 5u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL135: _temp134=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp72)->f1; goto _LL133;
_LL133: _temp132=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp72)->f2; goto _LL131; _LL131: _temp130=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp72)->f3; goto _LL93;} else{ goto _LL94;}
_LL94: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_XenumType_tag: 0){ _LL139: _temp138=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp72)->f1; goto _LL137; _LL137: _temp136=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp72)->f2; goto _LL95;} else{ goto _LL96;}
_LL96: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL145: _temp144=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp72)->f1; goto _LL143; _LL143: _temp142=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp72)->f2; goto _LL141; _LL141: _temp140=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp72)->f3;
if( _temp140 == 0){ goto _LL97;} else{ goto _LL98;}} else{ goto _LL98;} _LL98:
if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_StructType_tag:
0){ _LL151: _temp150=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp72)->f1; goto _LL149; _LL149: _temp148=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp72)->f2; goto _LL147; _LL147: _temp146=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp72)->f3;
goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp72 > 5u?((
struct _enum_struct*) _temp72)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL153:
_temp152=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp72)->f1; goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int)
_temp72 > 5u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL159: _temp158=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp72)->f1;
goto _LL157; _LL157: _temp156=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp72)->f2; goto _LL155; _LL155: _temp154=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp72)->f3; goto _LL103;} else{ goto _LL104;}
_LL104: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*) _temp72)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL161: _temp160=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp72)->f1; goto _LL105;} else{ goto
_LL106;} _LL106: if(( unsigned int) _temp72 > 5u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL167: _temp166=( struct
_tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp72)->f1; goto _LL165;
_LL165: _temp164=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp72)->f2; goto _LL163; _LL163: _temp162=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp72)->f3; goto _LL107;} else{ goto _LL108;} _LL108: if( _temp72 == Cyc_Absyn_UnionType){
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp72 > 5u?((
struct _enum_struct*) _temp72)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL169:
_temp168=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp72)->f1; goto
_LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp72 > 5u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL171: _temp170=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp72)->f1; goto
_LL113;} else{ goto _LL73;} _LL75: return _temp118; _LL77: return(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp120->kind); _LL79:
return Cyc_Absyn_MemKind; _LL81: return _temp122 == Cyc_Absyn_B4? Cyc_Absyn_BoxKind:
Cyc_Absyn_MemKind; _LL83: return Cyc_Absyn_MemKind; _LL85: return Cyc_Absyn_MemKind;
_LL87: return Cyc_Absyn_MemKind; _LL89: return Cyc_Absyn_BoxKind; _LL91: return
Cyc_Absyn_RgnKind; _LL93: return Cyc_Absyn_BoxKind; _LL95: return Cyc_Absyn_BoxKind;
_LL97: return Cyc_Absyn_AnyKind; _LL99: return Cyc_Absyn_MemKind; _LL101: { void*
_temp172=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp152.bounds))->v; void* _temp182; void* _temp184;
struct Cyc_Absyn_Exp* _temp186; struct Cyc_Absyn_Conref* _temp188; _LL174: if((
unsigned int) _temp172 > 1u?(( struct _enum_struct*) _temp172)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL183: _temp182=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp172)->f1;
if( _temp182 == Cyc_Absyn_Unknown_b){ goto _LL175;} else{ goto _LL176;}} else{
goto _LL176;} _LL176: if(( unsigned int) _temp172 > 1u?(( struct _enum_struct*)
_temp172)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL185: _temp184=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp172)->f1; if(( unsigned int) _temp184 >
1u?(( struct _enum_struct*) _temp184)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL187:
_temp186=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp184)->f1;
goto _LL177;} else{ goto _LL178;}} else{ goto _LL178;} _LL178: if( _temp172 ==
Cyc_Absyn_No_constr){ goto _LL179;} else{ goto _LL180;} _LL180: if((
unsigned int) _temp172 > 1u?(( struct _enum_struct*) _temp172)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL189: _temp188=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp172)->f1; goto _LL181;} else{ goto _LL173;} _LL175: return Cyc_Absyn_MemKind;
_LL177: return Cyc_Absyn_BoxKind; _LL179: return Cyc_Absyn_MemKind; _LL181:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp190=( char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp191; _temp191.curr= _temp190; _temp191.base= _temp190;
_temp191.last_plus_one= _temp190 + 43; _temp191;})); _LL173:;} _LL103: return
Cyc_Absyn_MemKind; _LL105: return Cyc_Absyn_MemKind; _LL107: return(( void*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp192=
Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s", _temp192.last_plus_one
- _temp192.curr, _temp192.curr);})); _LL109: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp193=(
char*)"union type"; struct _tagged_string _temp194; _temp194.curr= _temp193;
_temp194.base= _temp193; _temp194.last_plus_one= _temp193 + 11; _temp194;}));
_LL111: return Cyc_Absyn_EffKind; _LL113: return Cyc_Absyn_EffKind; _LL73:;}
char Cyc_Tcutil_Unify_tag[ 6u]="Unify"; struct Cyc_Tcutil_Unify_struct{ char*
tag;}; int Cyc_Tcutil_unify( void* t1, void* t2){ struct _handler_cons _temp195;
_push_handler(& _temp195);{ struct _xenum_struct* _temp196=( struct
_xenum_struct*) setjmp( _temp195.handler); if( ! _temp196){ Cyc_Tcutil_unify_it(
t1, t2);{ int _temp197= 1; _npop_handler( 0u); return _temp197;}; _pop_handler();}
else{ struct _xenum_struct* _temp199= _temp196; _LL201: if( _temp199->tag == Cyc_Tcutil_Unify_tag){
goto _LL202;} else{ goto _LL203;} _LL203: goto _LL204; _LL202: return 0; _LL204:(
void) _throw( _temp199); _LL200:;}}} static void Cyc_Tcutil_occurslist( void*
evar, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar, void*
t){ void* _temp205= Cyc_Tcutil_compress( t); int _temp231; struct Cyc_Core_Opt*
_temp233; void* _temp235; struct Cyc_Absyn_PtrInfo _temp237; struct Cyc_Absyn_Exp*
_temp239; struct Cyc_Absyn_Tqual* _temp241; void* _temp243; struct Cyc_Absyn_FnInfo
_temp245; int _temp247; struct Cyc_List_List* _temp249; void* _temp251; struct
Cyc_Core_Opt* _temp253; struct Cyc_List_List* _temp255; struct Cyc_List_List*
_temp257; void* _temp259; struct Cyc_Core_Opt* _temp261; struct Cyc_List_List*
_temp263; struct _tuple1* _temp265; struct Cyc_Absyn_Enumdecl** _temp267; struct
Cyc_List_List* _temp269; struct _tuple1* _temp271; struct Cyc_Absyn_Structdecl**
_temp273; struct Cyc_List_List* _temp275; struct _tuple1* _temp277; void*
_temp279; struct Cyc_List_List* _temp281; _LL207: if(( unsigned int) _temp205 >
5u?(( struct _enum_struct*) _temp205)->tag == Cyc_Absyn_Evar_tag: 0){ _LL236:
_temp235=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp205)->f1; goto _LL234;
_LL234: _temp233=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp205)->f2; goto _LL232; _LL232: _temp231=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp205)->f3; goto _LL208;} else{ goto _LL209;} _LL209: if(( unsigned int)
_temp205 > 5u?(( struct _enum_struct*) _temp205)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL238: _temp237=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp205)->f1; goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp205 > 5u?(( struct _enum_struct*) _temp205)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL244: _temp243=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f1;
goto _LL242; _LL242: _temp241=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp205)->f2; goto _LL240; _LL240: _temp239=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp205)->f3; goto _LL212;} else{ goto _LL213;}
_LL213: if(( unsigned int) _temp205 > 5u?(( struct _enum_struct*) _temp205)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL246: _temp245=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp205)->f1; _LL256: _temp255=( struct Cyc_List_List*)
_temp245.tvars; goto _LL254; _LL254: _temp253=( struct Cyc_Core_Opt*) _temp245.effect;
goto _LL252; _LL252: _temp251=( void*) _temp245.ret_typ; goto _LL250; _LL250:
_temp249=( struct Cyc_List_List*) _temp245.args; goto _LL248; _LL248: _temp247=(
int) _temp245.varargs; goto _LL214;} else{ goto _LL215;} _LL215: if((
unsigned int) _temp205 > 5u?(( struct _enum_struct*) _temp205)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL258: _temp257=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp205)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp205 > 5u?(( struct _enum_struct*) _temp205)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL260: _temp259=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp205)->f1;
goto _LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp205 > 5u?((
struct _enum_struct*) _temp205)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL266:
_temp265=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp205)->f1;
goto _LL264; _LL264: _temp263=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp205)->f2; goto _LL262; _LL262: _temp261=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp205)->f3; goto _LL220;} else{ goto _LL221;}
_LL221: if(( unsigned int) _temp205 > 5u?(( struct _enum_struct*) _temp205)->tag
== Cyc_Absyn_EnumType_tag: 0){ _LL272: _temp271=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp205)->f1; goto _LL270; _LL270: _temp269=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp205)->f2; goto _LL268; _LL268: _temp267=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp205)->f3;
goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp205 > 5u?((
struct _enum_struct*) _temp205)->tag == Cyc_Absyn_StructType_tag: 0){ _LL278:
_temp277=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp205)->f1;
goto _LL276; _LL276: _temp275=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp205)->f2; goto _LL274; _LL274: _temp273=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp205)->f3; goto _LL224;} else{ goto
_LL225;} _LL225: if(( unsigned int) _temp205 > 5u?(( struct _enum_struct*)
_temp205)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL280: _temp279=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp205)->f1; goto _LL226;} else{ goto
_LL227;} _LL227: if(( unsigned int) _temp205 > 5u?(( struct _enum_struct*)
_temp205)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL282: _temp281=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp205)->f1; goto _LL228;} else{ goto _LL229;}
_LL229: goto _LL230; _LL208: if( t == evar){( void) _throw(({ struct Cyc_Tcutil_Unify_struct*
_temp283=( struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));*
_temp283=( struct Cyc_Tcutil_Unify_struct){.tag= Cyc_Tcutil_Unify_tag};( struct
_xenum_struct*) _temp283;}));} else{ if( _temp233 != 0){ Cyc_Tcutil_occurs( evar,(
void*) _temp233->v);}} goto _LL206; _LL210: Cyc_Tcutil_occurs( evar,( void*)
_temp237.elt_typ); Cyc_Tcutil_occurs( evar,( void*) _temp237.rgn_typ); goto
_LL206; _LL212: Cyc_Tcutil_occurs( evar, _temp243); goto _LL206; _LL214: if(
_temp253 != 0){ Cyc_Tcutil_occurs( evar,( void*) _temp253->v);} Cyc_Tcutil_occurs(
evar, _temp251); for( 0; _temp249 != 0; _temp249= _temp249->tl){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple2*) _temp249->hd)).f3);} goto _LL206; _LL216: for( 0;
_temp257 != 0; _temp257= _temp257->tl){ Cyc_Tcutil_occurs( evar,(*(( struct
_tuple5*) _temp257->hd)).f2);} goto _LL206; _LL218: Cyc_Tcutil_occurs( evar,
_temp259); goto _LL206; _LL220: _temp269= _temp263; goto _LL222; _LL222:
_temp275= _temp269; goto _LL224; _LL224: Cyc_Tcutil_occurslist( evar, _temp275);
goto _LL206; _LL226: Cyc_Tcutil_occurs( evar, _temp279); goto _LL206; _LL228:
Cyc_Tcutil_occurslist( evar, _temp281); goto _LL206; _LL230: goto _LL206; _LL206:;}
static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* ts){ for( 0;
ts != 0; ts= ts->tl){ Cyc_Tcutil_occurs( evar,( void*) ts->hd);}} static void
Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp284; _push_handler(& _temp284);{ struct _xenum_struct*
_temp285=( struct _xenum_struct*) setjmp( _temp284.handler); if( ! _temp285){((
void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();} else{
struct _xenum_struct* _temp287= _temp285; _LL289: if( _temp287->tag == Cyc_List_List_mismatch_tag){
goto _LL290;} else{ goto _LL291;} _LL291: goto _LL292; _LL290:( void) _throw(({
struct Cyc_Tcutil_Unify_struct* _temp293=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));* _temp293=( struct
Cyc_Tcutil_Unify_struct){.tag= Cyc_Tcutil_Unify_tag};( struct _xenum_struct*)
_temp293;})); _LL292:( void) _throw( _temp287); _LL288:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2){ if(( tq1->q_const !=
tq2->q_const? 1: tq1->q_volatile != tq2->q_volatile)? 1: tq1->q_restrict != tq2->q_restrict){(
void) _throw(({ struct Cyc_Tcutil_Unify_struct* _temp294=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));* _temp294=( struct
Cyc_Tcutil_Unify_struct){.tag= Cyc_Tcutil_Unify_tag};( struct _xenum_struct*)
_temp294;}));}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual* tq1, struct
Cyc_Absyn_Tqual* tq2){ return( tq1->q_const == tq2->q_const? tq1->q_volatile ==
tq2->q_volatile: 0)? tq1->q_restrict == tq2->q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp295=( void*) x->v; struct Cyc_Absyn_Conref*
_temp303; void* _temp305; _LL297: if( _temp295 == Cyc_Absyn_No_constr){ goto
_LL298;} else{ goto _LL299;} _LL299: if(( unsigned int) _temp295 > 1u?(( struct
_enum_struct*) _temp295)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL304:
_temp303=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp295)->f1; goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int)
_temp295 > 1u?(( struct _enum_struct*) _temp295)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL306: _temp305=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp295)->f1;
goto _LL302;} else{ goto _LL296;} _LL298:( void*)( x->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp307=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp307->tag= Cyc_Absyn_Forward_constr_tag;
_temp307->f1= y;( void*) _temp307;})); return; _LL300:( void) _throw((( struct
_xenum_struct*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp308=( char*)"unify_conref: forward after compress";
struct _tagged_string _temp309; _temp309.curr= _temp308; _temp309.base= _temp308;
_temp309.last_plus_one= _temp308 + 37; _temp309;}))); _LL302: { void* _temp310=(
void*) y->v; struct Cyc_Absyn_Conref* _temp318; void* _temp320; _LL312: if(
_temp310 == Cyc_Absyn_No_constr){ goto _LL313;} else{ goto _LL314;} _LL314: if((
unsigned int) _temp310 > 1u?(( struct _enum_struct*) _temp310)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL319: _temp318=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp310)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int)
_temp310 > 1u?(( struct _enum_struct*) _temp310)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL321: _temp320=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp310)->f1;
goto _LL317;} else{ goto _LL311;} _LL313:( void*)( y->v=( void*)(( void*) x->v));
return; _LL315:( void) _throw((( struct _xenum_struct*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp322=( char*)"unify_confref: forward after compress(2)";
struct _tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 41; _temp323;}))); _LL317: if( cmp( _temp305,
_temp320) != 0){( void) _throw(({ struct Cyc_Tcutil_Unify_struct* _temp324=(
struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));*
_temp324=( struct Cyc_Tcutil_Unify_struct){.tag= Cyc_Tcutil_Unify_tag};( struct
_xenum_struct*) _temp324;}));} return; _LL311:;} _LL296:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp325; _push_handler(& _temp325);{ struct
_xenum_struct* _temp326=( struct _xenum_struct*) setjmp( _temp325.handler); if(
! _temp326){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp327= 1; _npop_handler( 0u); return _temp327;}; _pop_handler();} else{
struct _xenum_struct* _temp329= _temp326; _LL331: if( _temp329->tag == Cyc_Tcutil_Unify_tag){
goto _LL332;} else{ goto _LL333;} _LL333: goto _LL334; _LL332: return 0; _LL334:(
void) _throw( _temp329); _LL330:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp336=({ struct _tuple8 _temp335; _temp335.f1= b1;
_temp335.f2= b2; _temp335;}); void* _temp346; void* _temp348; void* _temp350;
void* _temp352; void* _temp354; void* _temp356; void* _temp358; struct Cyc_Absyn_Exp*
_temp360; void* _temp362; struct Cyc_Absyn_Exp* _temp364; _LL338: _LL349:
_temp348= _temp336.f1; if( _temp348 == Cyc_Absyn_Unknown_b){ goto _LL347;} else{
goto _LL340;} _LL347: _temp346= _temp336.f2; if( _temp346 == Cyc_Absyn_Unknown_b){
goto _LL339;} else{ goto _LL340;} _LL340: _LL353: _temp352= _temp336.f1; if(
_temp352 == Cyc_Absyn_Unknown_b){ goto _LL351;} else{ goto _LL342;} _LL351:
_temp350= _temp336.f2; goto _LL341; _LL342: _LL357: _temp356= _temp336.f1; goto
_LL355; _LL355: _temp354= _temp336.f2; if( _temp354 == Cyc_Absyn_Unknown_b){
goto _LL343;} else{ goto _LL344;} _LL344: _LL363: _temp362= _temp336.f1; if((
unsigned int) _temp362 > 1u?(( struct _enum_struct*) _temp362)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL365: _temp364=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp362)->f1; goto _LL359;} else{ goto _LL337;} _LL359: _temp358= _temp336.f2;
if(( unsigned int) _temp358 > 1u?(( struct _enum_struct*) _temp358)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL361: _temp360=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp358)->f1; goto _LL345;} else{ goto _LL337;} _LL339: return 0; _LL341:
return - 1; _LL343: return 1; _LL345: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp364); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp360); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL337:;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp366= e; struct Cyc_List_List*
_temp372; struct Cyc_List_List** _temp374; _LL368: if(( unsigned int) _temp366 >
5u?(( struct _enum_struct*) _temp366)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL373:
_temp372=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp366)->f1;
_temp374=&(( struct Cyc_Absyn_JoinEff_struct*) _temp366)->f1; goto _LL369;}
else{ goto _LL370;} _LL370: goto _LL371; _LL369: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp374; for( 0; effs != 0; effs= effs->tl){ void* eff=(
void*) effs->hd; Cyc_Tcutil_normalize_effect( eff);{ void* _temp375= Cyc_Tcutil_compress(
eff); struct Cyc_List_List* _temp381; _LL377: if(( unsigned int) _temp375 > 5u?((
struct _enum_struct*) _temp375)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL382:
_temp381=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp375)->f1;
goto _LL378;} else{ goto _LL379;} _LL379: goto _LL380; _LL378: nested_join= 1;
goto _LL376; _LL380: goto _LL376; _LL376:;}}} if( ! nested_join){ return;}{
struct Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp374; for( 0;
effs != 0; effs= effs->tl){ void* _temp383= Cyc_Tcutil_compress(( void*) effs->hd);
struct Cyc_List_List* _temp389; _LL385: if(( unsigned int) _temp383 > 5u?((
struct _enum_struct*) _temp383)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL390:
_temp389=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp383)->f1;
goto _LL386;} else{ goto _LL387;} _LL387: goto _LL388; _LL386: effects=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(
_temp389, effects); goto _LL384; _LL388: effects=({ struct Cyc_List_List*
_temp391=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp391->hd=( void*) _temp383; _temp391->tl= effects; _temp391;}); goto _LL384;
_LL384:;}}* _temp374=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); goto _LL367;}} _LL371: goto _LL367; _LL367:;}} static struct _tuple9*
Cyc_Tcutil_get_effect_evar( void* t){ void* _temp392= Cyc_Tcutil_compress( t);
struct Cyc_List_List* _temp400; struct Cyc_List_List _temp402; struct Cyc_List_List*
_temp403; void* _temp405; int _temp407; struct Cyc_Core_Opt* _temp409; struct
Cyc_Core_Opt** _temp411; void* _temp412; void** _temp414; _LL394: if((
unsigned int) _temp392 > 5u?(( struct _enum_struct*) _temp392)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL401: _temp400=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp392)->f1; if( _temp400 == 0){ goto _LL396;} else{ _temp402=* _temp400;
_LL406: _temp405=( void*) _temp402.hd; goto _LL404; _LL404: _temp403=( struct
Cyc_List_List*) _temp402.tl; goto _LL395;}} else{ goto _LL396;} _LL396: if((
unsigned int) _temp392 > 5u?(( struct _enum_struct*) _temp392)->tag == Cyc_Absyn_Evar_tag:
0){ _LL413: _temp412=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp392)->f1;
_temp414=&(( struct Cyc_Absyn_Evar_struct*) _temp392)->f1; goto _LL410; _LL410:
_temp409=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp392)->f2;
_temp411=&(( struct Cyc_Absyn_Evar_struct*) _temp392)->f2; goto _LL408; _LL408:
_temp407=( int)(( struct Cyc_Absyn_Evar_struct*) _temp392)->f3; goto _LL397;}
else{ goto _LL398;} _LL398: goto _LL399; _LL395: { void* _temp415= Cyc_Tcutil_compress(
_temp405); int _temp421; struct Cyc_Core_Opt* _temp423; void* _temp425; _LL417:
if(( unsigned int) _temp415 > 5u?(( struct _enum_struct*) _temp415)->tag == Cyc_Absyn_Evar_tag:
0){ _LL426: _temp425=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp415)->f1;
goto _LL424; _LL424: _temp423=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp415)->f2; goto _LL422; _LL422: _temp421=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp415)->f3; goto _LL418;} else{ goto _LL419;} _LL419: goto _LL420; _LL418:
return({ struct _tuple9* _temp427=( struct _tuple9*) GC_malloc( sizeof( struct
_tuple9)); _temp427->f1= _temp405; _temp427->f2= _temp403; _temp427;}); _LL420:
return 0; _LL416:;} _LL397: if(* _temp414 != Cyc_Absyn_EffKind){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp428=( char*)"effect evar has wrong kind"; struct _tagged_string _temp429;
_temp429.curr= _temp428; _temp429.base= _temp428; _temp429.last_plus_one=
_temp428 + 27; _temp429;}));} return({ struct _tuple9* _temp430=( struct _tuple9*)
GC_malloc( sizeof( struct _tuple9)); _temp430->f1= t; _temp430->f2= 0; _temp430;});
_LL399: return 0; _LL393:;} int Cyc_Tcutil_region_in_effect( int constrain, void*
r, void* e){ r= Cyc_Tcutil_compress( r); if( r == Cyc_Absyn_HeapRgn){ return 1;}{
void* _temp431= Cyc_Tcutil_compress( e); void* _temp441; struct Cyc_List_List*
_temp443; int _temp445; struct Cyc_Core_Opt* _temp447; struct Cyc_Core_Opt**
_temp449; void* _temp450; void** _temp452; _LL433: if(( unsigned int) _temp431 >
5u?(( struct _enum_struct*) _temp431)->tag == Cyc_Absyn_AccessEff_tag: 0){
_LL442: _temp441=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp431)->f1;
goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int) _temp431 > 5u?((
struct _enum_struct*) _temp431)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL444:
_temp443=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp431)->f1;
goto _LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp431 > 5u?((
struct _enum_struct*) _temp431)->tag == Cyc_Absyn_Evar_tag: 0){ _LL451: _temp450=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp431)->f1; _temp452=&(( struct Cyc_Absyn_Evar_struct*)
_temp431)->f1; goto _LL448; _LL448: _temp447=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp431)->f2; _temp449=&(( struct Cyc_Absyn_Evar_struct*)
_temp431)->f2; goto _LL446; _LL446: _temp445=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp431)->f3; goto _LL438;} else{ goto _LL439;} _LL439: goto _LL440; _LL434:
if( constrain){ return Cyc_Tcutil_unify( r, _temp441);} Cyc_Tcutil_compress(
_temp441); if( r == _temp441){ return 1;}{ struct _tuple8 _temp454=({ struct
_tuple8 _temp453; _temp453.f1= r; _temp453.f2= _temp441; _temp453;}); void*
_temp460; struct Cyc_Absyn_Tvar* _temp462; void* _temp464; struct Cyc_Absyn_Tvar*
_temp466; _LL456: _LL465: _temp464= _temp454.f1; if(( unsigned int) _temp464 > 5u?((
struct _enum_struct*) _temp464)->tag == Cyc_Absyn_VarType_tag: 0){ _LL467:
_temp466=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp464)->f1;
goto _LL461;} else{ goto _LL458;} _LL461: _temp460= _temp454.f2; if((
unsigned int) _temp460 > 5u?(( struct _enum_struct*) _temp460)->tag == Cyc_Absyn_VarType_tag:
0){ _LL463: _temp462=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp460)->f1; goto _LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL457:
return Cyc_Absyn_tvar_cmp( _temp466, _temp462) == 0; _LL459: return 0; _LL455:;}
_LL436: for( 0; _temp443 != 0; _temp443= _temp443->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*) _temp443->hd)){ return 1;}} return 0; _LL438: if(*
_temp452 != Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp468=( char*)"effect evar has wrong kind";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 27; _temp469;}));} if( ! constrain){ return 0;}{
void* ev= Cyc_Absyn_new_evar( Cyc_Absyn_EffKind); void* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp470=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp470->tag= Cyc_Absyn_JoinEff_tag; _temp470->f1=({ struct Cyc_List_List*
_temp471=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp471->hd=( void*) ev; _temp471->tl=({ struct Cyc_List_List* _temp472=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp472->hd=(
void*)({ struct Cyc_Absyn_AccessEff_struct* _temp473=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp473->tag= Cyc_Absyn_AccessEff_tag;
_temp473->f1=( void*) r;( void*) _temp473;}); _temp472->tl= 0; _temp472;});
_temp471;});( void*) _temp470;});* _temp449=({ struct Cyc_Core_Opt* _temp474=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp474->v=(
void*) new_typ; _temp474;}); return 1;} _LL440: return 0; _LL432:;}} static int
Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar* v, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp475= e; struct Cyc_Absyn_Tvar* _temp485;
struct Cyc_List_List* _temp487; int _temp489; struct Cyc_Core_Opt* _temp491;
struct Cyc_Core_Opt** _temp493; void* _temp494; void** _temp496; _LL477: if((
unsigned int) _temp475 > 5u?(( struct _enum_struct*) _temp475)->tag == Cyc_Absyn_VarType_tag:
0){ _LL486: _temp485=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp475)->f1; goto _LL478;} else{ goto _LL479;} _LL479: if(( unsigned int)
_temp475 > 5u?(( struct _enum_struct*) _temp475)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL488: _temp487=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp475)->f1; goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int)
_temp475 > 5u?(( struct _enum_struct*) _temp475)->tag == Cyc_Absyn_Evar_tag: 0){
_LL495: _temp494=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp475)->f1;
_temp496=&(( struct Cyc_Absyn_Evar_struct*) _temp475)->f1; goto _LL492; _LL492:
_temp491=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp475)->f2;
_temp493=&(( struct Cyc_Absyn_Evar_struct*) _temp475)->f2; goto _LL490; _LL490:
_temp489=( int)(( struct Cyc_Absyn_Evar_struct*) _temp475)->f3; goto _LL482;}
else{ goto _LL483;} _LL483: goto _LL484; _LL478: return Cyc_Absyn_tvar_cmp( v,
_temp485) == 0; _LL480: for( 0; _temp487 != 0; _temp487= _temp487->tl){ if( Cyc_Tcutil_variable_in_effect(
constrain, v,( void*) _temp487->hd)){ return 1;}} return 0; _LL482: if(*
_temp496 != Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp497=( char*)"effect evar has wrong kind";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 27; _temp498;}));}{ void* ev= Cyc_Absyn_new_evar(
Cyc_Absyn_EffKind); void* new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp499=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp499->tag= Cyc_Absyn_JoinEff_tag; _temp499->f1=({ struct Cyc_List_List*
_temp500=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp500->hd=( void*) ev; _temp500->tl=({ struct Cyc_List_List* _temp501=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp501->hd=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp502=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp502->tag= Cyc_Absyn_VarType_tag;
_temp502->f1= v;( void*) _temp502;}); _temp501->tl= 0; _temp501;}); _temp500;});(
void*) _temp499;});* _temp493=({ struct Cyc_Core_Opt* _temp503=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp503->v=( void*) new_typ; _temp503;});
return 1;} _LL484: return 0; _LL476:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp504= e; struct Cyc_List_List*
_temp512; int _temp514; struct Cyc_Core_Opt* _temp516; void* _temp518; _LL506:
if(( unsigned int) _temp504 > 5u?(( struct _enum_struct*) _temp504)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL513: _temp512=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp504)->f1; goto _LL507;} else{ goto _LL508;} _LL508: if(( unsigned int)
_temp504 > 5u?(( struct _enum_struct*) _temp504)->tag == Cyc_Absyn_Evar_tag: 0){
_LL519: _temp518=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp504)->f1; goto
_LL517; _LL517: _temp516=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp504)->f2; goto _LL515; _LL515: _temp514=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp504)->f3; goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL507:
for( 0; _temp512 != 0; _temp512= _temp512->tl){ if( Cyc_Tcutil_evar_in_effect(
evar,( void*) _temp512->hd)){ return 1;}} return 0; _LL509: return evar == e;
_LL511: return 0; _LL505:;}} int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2){{ struct _tuple9* _temp520= Cyc_Tcutil_get_effect_evar( e2);
struct _tuple9 _temp526; struct Cyc_List_List* _temp527; void* _temp529; _LL522:
if( _temp520 == 0){ goto _LL524;} else{ _temp526=* _temp520; _LL530: _temp529=
_temp526.f1; goto _LL528; _LL528: _temp527= _temp526.f2; goto _LL523;} _LL524:
goto _LL525; _LL523: { void* ev= Cyc_Absyn_new_evar( Cyc_Absyn_EffKind); void*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp531=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp531->tag= Cyc_Absyn_JoinEff_tag;
_temp531->f1=({ struct Cyc_List_List* _temp532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp532->hd=( void*) ev; _temp532->tl=({
struct Cyc_List_List* _temp533=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp533->hd=( void*) e1; _temp533->tl= 0; _temp533;});
_temp532;});( void*) _temp531;}); return Cyc_Tcutil_unify( _temp529, new_typ);}
_LL525: goto _LL521; _LL521:;}{ void* _temp534= Cyc_Tcutil_compress( e1); struct
Cyc_List_List* _temp546; void* _temp548; struct Cyc_Absyn_Tvar* _temp550; int
_temp552; struct Cyc_Core_Opt* _temp554; struct Cyc_Core_Opt** _temp556; void*
_temp557; _LL536: if(( unsigned int) _temp534 > 5u?(( struct _enum_struct*)
_temp534)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL547: _temp546=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp534)->f1; goto _LL537;} else{ goto _LL538;}
_LL538: if(( unsigned int) _temp534 > 5u?(( struct _enum_struct*) _temp534)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL549: _temp548=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp534)->f1; goto _LL539;} else{ goto _LL540;} _LL540: if(( unsigned int)
_temp534 > 5u?(( struct _enum_struct*) _temp534)->tag == Cyc_Absyn_VarType_tag:
0){ _LL551: _temp550=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp534)->f1; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp534 > 5u?(( struct _enum_struct*) _temp534)->tag == Cyc_Absyn_Evar_tag: 0){
_LL558: _temp557=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp534)->f1; goto
_LL555; _LL555: _temp554=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp534)->f2; _temp556=&(( struct Cyc_Absyn_Evar_struct*) _temp534)->f2; goto
_LL553; _LL553: _temp552=( int)(( struct Cyc_Absyn_Evar_struct*) _temp534)->f3;
goto _LL543;} else{ goto _LL544;} _LL544: goto _LL545; _LL537: for( 0; _temp546
!= 0; _temp546= _temp546->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*) _temp546->hd, e2)){ return 0;}} return 1; _LL539: return( Cyc_Tcutil_region_in_effect(
0, _temp548, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp548, e2))? 1: Cyc_Tcutil_unify(
_temp548, Cyc_Absyn_HeapRgn); _LL541: return Cyc_Tcutil_variable_in_effect( 0,
_temp550, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp550, e2); _LL543: if( !
Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp556=({ struct Cyc_Core_Opt*
_temp559=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp559->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp560=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp560->tag= Cyc_Absyn_JoinEff_tag;
_temp560->f1= 0;( void*) _temp560;}); _temp559;});} else{* _temp556=({ struct
Cyc_Core_Opt* _temp561=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp561->v=( void*) e2; _temp561;});}} return 1; _LL545: return(( int(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp562=
Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s", _temp562.last_plus_one
- _temp562.curr, _temp562.curr);})); _LL535:;}} static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple10 _temp564=({ struct _tuple10 _temp563; _temp563.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp563.f2= Cyc_Tcutil_get_effect_evar( e2); _temp563;}); struct _tuple9*
_temp570; struct _tuple9 _temp572; struct Cyc_List_List* _temp573; void*
_temp575; struct _tuple9* _temp577; struct _tuple9 _temp579; struct Cyc_List_List*
_temp580; void* _temp582; _LL566: _LL578: _temp577= _temp564.f1; if( _temp577 ==
0){ goto _LL568;} else{ _temp579=* _temp577; _LL583: _temp582= _temp579.f1; goto
_LL581; _LL581: _temp580= _temp579.f2; goto _LL571;} _LL571: _temp570= _temp564.f2;
if( _temp570 == 0){ goto _LL568;} else{ _temp572=* _temp570; _LL576: _temp575=
_temp572.f1; goto _LL574; _LL574: _temp573= _temp572.f2; goto _LL567;} _LL568:
goto _LL569; _LL567: { void* ev= Cyc_Absyn_new_evar( Cyc_Absyn_EffKind); void*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp584=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp584->tag= Cyc_Absyn_JoinEff_tag;
_temp584->f1=({ struct Cyc_List_List* _temp585=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp585->hd=( void*) ev; _temp585->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp580, _temp573); _temp585;});( void*) _temp584;}); return Cyc_Tcutil_unify(
_temp582, new_typ)? Cyc_Tcutil_unify( _temp575, new_typ): 0;} _LL569: return Cyc_Tcutil_subset_effect(
0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1): 0; _LL565:;}} void Cyc_Tcutil_unify_it(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);
if( t1 == t2){ return;}{ void* _temp586= t1; int _temp592; struct Cyc_Core_Opt*
_temp594; struct Cyc_Core_Opt** _temp596; void* _temp597; _LL588: if((
unsigned int) _temp586 > 5u?(( struct _enum_struct*) _temp586)->tag == Cyc_Absyn_Evar_tag:
0){ _LL598: _temp597=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp586)->f1;
goto _LL595; _LL595: _temp594=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp586)->f2; _temp596=&(( struct Cyc_Absyn_Evar_struct*) _temp586)->f2; goto
_LL593; _LL593: _temp592=( int)(( struct Cyc_Absyn_Evar_struct*) _temp586)->f3;
goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL589: Cyc_Tcutil_occurs(
t1, t2);{ void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( kind2,
_temp597)){* _temp596=({ struct Cyc_Core_Opt* _temp599=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp599->v=( void*) t2; _temp599;});
return;} else{{ void* _temp600= t2; int _temp608; struct Cyc_Core_Opt* _temp610;
struct Cyc_Core_Opt** _temp612; void* _temp613; struct Cyc_Absyn_PtrInfo
_temp616; _LL602: if(( unsigned int) _temp600 > 5u?(( struct _enum_struct*)
_temp600)->tag == Cyc_Absyn_Evar_tag: 0){ _LL614: _temp613=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp600)->f1; goto _LL611; _LL611: _temp610=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp600)->f2; _temp612=&(( struct Cyc_Absyn_Evar_struct*)
_temp600)->f2; goto _LL609; _LL609: _temp608=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp600)->f3; goto _LL603;} else{ goto _LL604;} _LL604: if(( unsigned int)
_temp600 > 5u?(( struct _enum_struct*) _temp600)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL617: _temp616=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp600)->f1; goto _LL615;} else{ goto _LL606;} _LL615: if( _temp597 == Cyc_Absyn_BoxKind){
goto _LL605;} else{ goto _LL606;} _LL606: goto _LL607; _LL603: if( Cyc_Tcutil_kind_leq(
_temp597, kind2)){* _temp612=({ struct Cyc_Core_Opt* _temp618=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp618->v=( void*) t1; _temp618;});
return;} goto _LL601; _LL605: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp616.bounds);{ void*
_temp619=( void*) c->v; _LL621: if( _temp619 == Cyc_Absyn_No_constr){ goto
_LL622;} else{ goto _LL623;} _LL623: goto _LL624; _LL622:( void*)( c->v=( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp625=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp625->tag= Cyc_Absyn_Eq_constr_tag;
_temp625->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp626=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp626->tag= Cyc_Absyn_Upper_b_tag;
_temp626->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp626;});( void*)
_temp625;}));* _temp596=({ struct Cyc_Core_Opt* _temp627=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp627->v=( void*) t2; _temp627;});
return; _LL624: goto _LL620; _LL620:;} goto _LL601;} _LL607: goto _LL601; _LL601:;}(
void) _throw(({ struct Cyc_Tcutil_Unify_struct* _temp628=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));* _temp628=( struct
Cyc_Tcutil_Unify_struct){.tag= Cyc_Tcutil_Unify_tag};( struct _xenum_struct*)
_temp628;}));}} _LL591: goto _LL587; _LL587:;}{ struct _tuple8 _temp630=({
struct _tuple8 _temp629; _temp629.f1= t2; _temp629.f2= t1; _temp629;}); void*
_temp672; void* _temp674; int _temp676; struct Cyc_Core_Opt* _temp678; void*
_temp680; void* _temp682; void* _temp684; void* _temp686; struct Cyc_Absyn_Tvar*
_temp688; void* _temp690; struct Cyc_Absyn_Tvar* _temp692; void* _temp694;
struct Cyc_Absyn_Structdecl** _temp696; struct Cyc_List_List* _temp698; struct
_tuple1* _temp700; void* _temp702; struct Cyc_Absyn_Structdecl** _temp704;
struct Cyc_List_List* _temp706; struct _tuple1* _temp708; void* _temp710; struct
Cyc_Absyn_Enumdecl** _temp712; struct Cyc_List_List* _temp714; struct _tuple1*
_temp716; void* _temp718; struct Cyc_Absyn_Enumdecl** _temp720; struct Cyc_List_List*
_temp722; struct _tuple1* _temp724; void* _temp726; struct Cyc_Absyn_Xenumdecl**
_temp728; struct _tuple1* _temp730; void* _temp732; struct Cyc_Absyn_Xenumdecl**
_temp734; struct _tuple1* _temp736; void* _temp738; struct Cyc_Absyn_PtrInfo
_temp740; struct Cyc_Absyn_Conref* _temp742; struct Cyc_Absyn_Tqual* _temp744;
struct Cyc_Absyn_Conref* _temp746; void* _temp748; void* _temp750; void*
_temp752; struct Cyc_Absyn_PtrInfo _temp754; struct Cyc_Absyn_Conref* _temp756;
struct Cyc_Absyn_Tqual* _temp758; struct Cyc_Absyn_Conref* _temp760; void*
_temp762; void* _temp764; void* _temp766; void* _temp768; void* _temp770; void*
_temp772; void* _temp774; void* _temp776; void* _temp778; void* _temp780; void*
_temp782; void* _temp784; void* _temp786; struct Cyc_Absyn_Exp* _temp788; struct
Cyc_Absyn_Tqual* _temp790; void* _temp792; void* _temp794; struct Cyc_Absyn_Exp*
_temp796; struct Cyc_Absyn_Tqual* _temp798; void* _temp800; void* _temp802;
struct Cyc_Absyn_FnInfo _temp804; int _temp806; struct Cyc_List_List* _temp808;
void* _temp810; struct Cyc_Core_Opt* _temp812; struct Cyc_List_List* _temp814;
void* _temp816; struct Cyc_Absyn_FnInfo _temp818; int _temp820; struct Cyc_List_List*
_temp822; void* _temp824; struct Cyc_Core_Opt* _temp826; struct Cyc_List_List*
_temp828; void* _temp830; struct Cyc_List_List* _temp832; void* _temp834; struct
Cyc_List_List* _temp836; void* _temp838; void* _temp840; void* _temp842; void*
_temp844; void* _temp846; void* _temp848; void* _temp850; void* _temp852; struct
Cyc_List_List* _temp854; void* _temp856; struct Cyc_List_List* _temp858; void*
_temp860; void* _temp862; void* _temp864; void* _temp866; void* _temp868; void*
_temp870; void* _temp872; _LL632: _LL675: _temp674= _temp630.f1; if((
unsigned int) _temp674 > 5u?(( struct _enum_struct*) _temp674)->tag == Cyc_Absyn_Evar_tag:
0){ _LL681: _temp680=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp674)->f1;
goto _LL679; _LL679: _temp678=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp674)->f2; goto _LL677; _LL677: _temp676=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp674)->f3; goto _LL673;} else{ goto _LL634;} _LL673: _temp672= _temp630.f2;
goto _LL633; _LL634: _LL685: _temp684= _temp630.f1; if( _temp684 == Cyc_Absyn_VoidType){
goto _LL683;} else{ goto _LL636;} _LL683: _temp682= _temp630.f2; if( _temp682 ==
Cyc_Absyn_VoidType){ goto _LL635;} else{ goto _LL636;} _LL636: _LL691: _temp690=
_temp630.f1; if(( unsigned int) _temp690 > 5u?(( struct _enum_struct*) _temp690)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL693: _temp692=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp690)->f1; goto _LL687;} else{ goto _LL638;}
_LL687: _temp686= _temp630.f2; if(( unsigned int) _temp686 > 5u?(( struct
_enum_struct*) _temp686)->tag == Cyc_Absyn_VarType_tag: 0){ _LL689: _temp688=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp686)->f1; goto
_LL637;} else{ goto _LL638;} _LL638: _LL703: _temp702= _temp630.f1; if((
unsigned int) _temp702 > 5u?(( struct _enum_struct*) _temp702)->tag == Cyc_Absyn_StructType_tag:
0){ _LL709: _temp708=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp702)->f1; goto _LL707; _LL707: _temp706=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp702)->f2; goto _LL705; _LL705: _temp704=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp702)->f3;
goto _LL695;} else{ goto _LL640;} _LL695: _temp694= _temp630.f2; if((
unsigned int) _temp694 > 5u?(( struct _enum_struct*) _temp694)->tag == Cyc_Absyn_StructType_tag:
0){ _LL701: _temp700=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp694)->f1; goto _LL699; _LL699: _temp698=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp694)->f2; goto _LL697; _LL697: _temp696=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp694)->f3;
goto _LL639;} else{ goto _LL640;} _LL640: _LL719: _temp718= _temp630.f1; if((
unsigned int) _temp718 > 5u?(( struct _enum_struct*) _temp718)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL725: _temp724=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp718)->f1; goto _LL723; _LL723: _temp722=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp718)->f2; goto _LL721; _LL721: _temp720=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp718)->f3;
goto _LL711;} else{ goto _LL642;} _LL711: _temp710= _temp630.f2; if((
unsigned int) _temp710 > 5u?(( struct _enum_struct*) _temp710)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL717: _temp716=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp710)->f1; goto _LL715; _LL715: _temp714=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp710)->f2; goto _LL713; _LL713: _temp712=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp710)->f3;
goto _LL641;} else{ goto _LL642;} _LL642: _LL733: _temp732= _temp630.f1; if((
unsigned int) _temp732 > 5u?(( struct _enum_struct*) _temp732)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL737: _temp736=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp732)->f1; goto _LL735; _LL735: _temp734=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp732)->f2; goto _LL727;} else{ goto
_LL644;} _LL727: _temp726= _temp630.f2; if(( unsigned int) _temp726 > 5u?((
struct _enum_struct*) _temp726)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL731:
_temp730=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp726)->f1;
goto _LL729; _LL729: _temp728=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp726)->f2; goto _LL643;} else{ goto _LL644;} _LL644: _LL753: _temp752=
_temp630.f1; if(( unsigned int) _temp752 > 5u?(( struct _enum_struct*) _temp752)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL755: _temp754=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp752)->f1; _LL765: _temp764=( void*)
_temp754.elt_typ; goto _LL763; _LL763: _temp762=( void*) _temp754.rgn_typ; goto
_LL761; _LL761: _temp760=( struct Cyc_Absyn_Conref*) _temp754.nullable; goto
_LL759; _LL759: _temp758=( struct Cyc_Absyn_Tqual*) _temp754.tq; goto _LL757;
_LL757: _temp756=( struct Cyc_Absyn_Conref*) _temp754.bounds; goto _LL739;}
else{ goto _LL646;} _LL739: _temp738= _temp630.f2; if(( unsigned int) _temp738 >
5u?(( struct _enum_struct*) _temp738)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL741: _temp740=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp738)->f1; _LL751: _temp750=( void*) _temp740.elt_typ; goto _LL749; _LL749:
_temp748=( void*) _temp740.rgn_typ; goto _LL747; _LL747: _temp746=( struct Cyc_Absyn_Conref*)
_temp740.nullable; goto _LL745; _LL745: _temp744=( struct Cyc_Absyn_Tqual*)
_temp740.tq; goto _LL743; _LL743: _temp742=( struct Cyc_Absyn_Conref*) _temp740.bounds;
goto _LL645;} else{ goto _LL646;} _LL646: _LL773: _temp772= _temp630.f1; if((
unsigned int) _temp772 > 5u?(( struct _enum_struct*) _temp772)->tag == Cyc_Absyn_IntType_tag:
0){ _LL777: _temp776=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp772)->f1;
goto _LL775; _LL775: _temp774=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp772)->f2; goto _LL767;} else{ goto _LL648;} _LL767: _temp766= _temp630.f2;
if(( unsigned int) _temp766 > 5u?(( struct _enum_struct*) _temp766)->tag == Cyc_Absyn_IntType_tag:
0){ _LL771: _temp770=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp766)->f1;
goto _LL769; _LL769: _temp768=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp766)->f2; goto _LL647;} else{ goto _LL648;} _LL648: _LL781: _temp780=
_temp630.f1; if( _temp780 == Cyc_Absyn_FloatType){ goto _LL779;} else{ goto
_LL650;} _LL779: _temp778= _temp630.f2; if( _temp778 == Cyc_Absyn_FloatType){
goto _LL649;} else{ goto _LL650;} _LL650: _LL785: _temp784= _temp630.f1; if(
_temp784 == Cyc_Absyn_DoubleType){ goto _LL783;} else{ goto _LL652;} _LL783:
_temp782= _temp630.f2; if( _temp782 == Cyc_Absyn_DoubleType){ goto _LL651;}
else{ goto _LL652;} _LL652: _LL795: _temp794= _temp630.f1; if(( unsigned int)
_temp794 > 5u?(( struct _enum_struct*) _temp794)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL801: _temp800=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp794)->f1;
goto _LL799; _LL799: _temp798=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp794)->f2; goto _LL797; _LL797: _temp796=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp794)->f3; goto _LL787;} else{ goto _LL654;}
_LL787: _temp786= _temp630.f2; if(( unsigned int) _temp786 > 5u?(( struct
_enum_struct*) _temp786)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL793: _temp792=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp786)->f1; goto _LL791; _LL791:
_temp790=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp786)->f2; goto _LL789; _LL789: _temp788=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp786)->f3; goto _LL653;} else{ goto _LL654;}
_LL654: _LL817: _temp816= _temp630.f1; if(( unsigned int) _temp816 > 5u?((
struct _enum_struct*) _temp816)->tag == Cyc_Absyn_FnType_tag: 0){ _LL819:
_temp818=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp816)->f1;
_LL829: _temp828=( struct Cyc_List_List*) _temp818.tvars; goto _LL827; _LL827:
_temp826=( struct Cyc_Core_Opt*) _temp818.effect; goto _LL825; _LL825: _temp824=(
void*) _temp818.ret_typ; goto _LL823; _LL823: _temp822=( struct Cyc_List_List*)
_temp818.args; goto _LL821; _LL821: _temp820=( int) _temp818.varargs; goto
_LL803;} else{ goto _LL656;} _LL803: _temp802= _temp630.f2; if(( unsigned int)
_temp802 > 5u?(( struct _enum_struct*) _temp802)->tag == Cyc_Absyn_FnType_tag: 0){
_LL805: _temp804=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp802)->f1; _LL815: _temp814=( struct Cyc_List_List*) _temp804.tvars; goto
_LL813; _LL813: _temp812=( struct Cyc_Core_Opt*) _temp804.effect; goto _LL811;
_LL811: _temp810=( void*) _temp804.ret_typ; goto _LL809; _LL809: _temp808=(
struct Cyc_List_List*) _temp804.args; goto _LL807; _LL807: _temp806=( int)
_temp804.varargs; goto _LL655;} else{ goto _LL656;} _LL656: _LL835: _temp834=
_temp630.f1; if(( unsigned int) _temp834 > 5u?(( struct _enum_struct*) _temp834)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL837: _temp836=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp834)->f1; goto _LL831;} else{ goto
_LL658;} _LL831: _temp830= _temp630.f2; if(( unsigned int) _temp830 > 5u?((
struct _enum_struct*) _temp830)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL833:
_temp832=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp830)->f1;
goto _LL657;} else{ goto _LL658;} _LL658: _LL841: _temp840= _temp630.f1; if(
_temp840 == Cyc_Absyn_HeapRgn){ goto _LL839;} else{ goto _LL660;} _LL839:
_temp838= _temp630.f2; if( _temp838 == Cyc_Absyn_HeapRgn){ goto _LL659;} else{
goto _LL660;} _LL660: _LL847: _temp846= _temp630.f1; if(( unsigned int) _temp846
> 5u?(( struct _enum_struct*) _temp846)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL849: _temp848=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp846)->f1;
goto _LL843;} else{ goto _LL662;} _LL843: _temp842= _temp630.f2; if((
unsigned int) _temp842 > 5u?(( struct _enum_struct*) _temp842)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL845: _temp844=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp842)->f1;
goto _LL661;} else{ goto _LL662;} _LL662: _LL853: _temp852= _temp630.f1; if((
unsigned int) _temp852 > 5u?(( struct _enum_struct*) _temp852)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL855: _temp854=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp852)->f1; goto _LL851;} else{ goto _LL664;} _LL851: _temp850= _temp630.f2;
goto _LL663; _LL664: _LL861: _temp860= _temp630.f1; goto _LL857; _LL857:
_temp856= _temp630.f2; if(( unsigned int) _temp856 > 5u?(( struct _enum_struct*)
_temp856)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL859: _temp858=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp856)->f1; goto _LL665;} else{ goto _LL666;}
_LL666: _LL865: _temp864= _temp630.f1; if(( unsigned int) _temp864 > 5u?((
struct _enum_struct*) _temp864)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL867:
_temp866=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp864)->f1; goto
_LL863;} else{ goto _LL668;} _LL863: _temp862= _temp630.f2; goto _LL667; _LL668:
_LL873: _temp872= _temp630.f1; goto _LL869; _LL869: _temp868= _temp630.f2; if((
unsigned int) _temp868 > 5u?(( struct _enum_struct*) _temp868)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL871: _temp870=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp868)->f1;
goto _LL669;} else{ goto _LL670;} _LL670: goto _LL671; _LL633: Cyc_Tcutil_unify_it(
t2, t1); return; _LL635: return; _LL637: { struct _tagged_string* x2= _temp692->name;
struct _tagged_string* x1= _temp688->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp692->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp688->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp874=* x2; struct _tagged_string _temp875= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp876= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp874.last_plus_one - _temp874.curr, _temp874.curr, _temp875.last_plus_one -
_temp875.curr, _temp875.curr, _temp876.last_plus_one - _temp876.curr, _temp876.curr);}));}
return;} goto _LL631;} _LL639: if((( _temp700 != 0? _temp708 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*)({ struct _tuple1* _temp877= _temp700; if( _temp877 == 0){
_throw( Null_Exception);} _temp877;}),( struct _tuple1*)({ struct _tuple1*
_temp878= _temp708; if( _temp878 == 0){ _throw( Null_Exception);} _temp878;}))
== 0: 0)? 1:( _temp700 == 0? _temp708 == 0: 0)){ Cyc_Tcutil_unify_list( _temp698,
_temp706); return;} goto _LL631; _LL641: if((( _temp716 != 0? _temp724 != 0: 0)?
Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1* _temp879= _temp716; if(
_temp879 == 0){ _throw( Null_Exception);} _temp879;}),( struct _tuple1*)({
struct _tuple1* _temp880= _temp724; if( _temp880 == 0){ _throw( Null_Exception);}
_temp880;})) == 0: 0)? 1:( _temp716 == 0? _temp724 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp714, _temp722); return;} goto _LL631; _LL643: if(* _temp734 ==* _temp728){
return;} goto _LL631; _LL645: Cyc_Tcutil_unify_it( _temp750, _temp764); Cyc_Tcutil_unify_it(
_temp762, _temp748); Cyc_Tcutil_unify_tqual( _temp744, _temp758);(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp, _temp742, _temp756);{ void*
_temp881=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp742))->v; void* _temp887; _LL883: if((
unsigned int) _temp881 > 1u?(( struct _enum_struct*) _temp881)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL888: _temp887=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp881)->f1;
if( _temp887 == Cyc_Absyn_Unknown_b){ goto _LL884;} else{ goto _LL885;}} else{
goto _LL885;} _LL885: goto _LL886; _LL884: return; _LL886: goto _LL882; _LL882:;}((
void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp, _temp746, _temp760); return;
_LL647: if( _temp770 == _temp776? _temp768 == _temp774: 0){ return;} goto _LL631;
_LL649: return; _LL651: return; _LL653: Cyc_Tcutil_unify_it( _temp792, _temp800);
Cyc_Tcutil_unify_tqual( _temp790, _temp798); if( Cyc_Evexp_eval_const_uint_exp(
_temp796) == Cyc_Evexp_eval_const_uint_exp( _temp788)){ return;} goto _LL631;
_LL655: { struct Cyc_List_List* inst= 0; while( _temp814 != 0) { if( _temp828 ==
0){ break;} inst=({ struct Cyc_List_List* _temp889=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp889->hd=( void*)({ struct
_tuple6* _temp890=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp890->f1=( struct Cyc_Absyn_Tvar*) _temp828->hd; _temp890->f2=({ struct Cyc_Absyn_VarType_struct*
_temp891=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp891->tag= Cyc_Absyn_VarType_tag; _temp891->f1=( struct Cyc_Absyn_Tvar*)
_temp814->hd;( void*) _temp891;}); _temp890;}); _temp889->tl= inst; _temp889;});
_temp814= _temp814->tl; _temp828= _temp828->tl;} if( _temp828 != 0){ goto _LL631;}
if( inst != 0){ Cyc_Tcutil_unify_it(({ struct Cyc_Absyn_FnType_struct* _temp892=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp892->tag= Cyc_Absyn_FnType_tag; _temp892->f1=({ struct Cyc_Absyn_FnInfo
_temp893; _temp893.tvars= 0; _temp893.effect= _temp812; _temp893.ret_typ=( void*)
_temp810; _temp893.args= _temp808; _temp893.varargs= _temp806; _temp893;});(
void*) _temp892;}), Cyc_Tcutil_substitute( inst,({ struct Cyc_Absyn_FnType_struct*
_temp894=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp894->tag= Cyc_Absyn_FnType_tag; _temp894->f1=({ struct Cyc_Absyn_FnInfo
_temp895; _temp895.tvars= 0; _temp895.effect= _temp826; _temp895.ret_typ=( void*)
_temp824; _temp895.args= _temp822; _temp895.varargs= _temp820; _temp895;});(
void*) _temp894;}))); return;} Cyc_Tcutil_unify_it( _temp810, _temp824); for( 0;
_temp808 != 0? _temp822 != 0: 0; _temp808= _temp808->tl, _temp822= _temp822->tl){
Cyc_Tcutil_unify_tqual((*(( struct _tuple2*) _temp808->hd)).f2,(*(( struct
_tuple2*) _temp822->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*) _temp808->hd)).f3,(*((
struct _tuple2*) _temp822->hd)).f3);} if( _temp808 != 0? 1: _temp822 != 0){ goto
_LL631;} if( _temp806 != _temp820){ goto _LL631;} if(( _temp812 == 0? _temp826
!= 0: 0)? 1:( _temp812 != 0? _temp826 == 0: 0)){ goto _LL631;} if( ! Cyc_Tcutil_unify_effect((
void*) _temp812->v,( void*) _temp826->v)){ goto _LL631;} return;} _LL657: for( 0;
_temp832 != 0? _temp836 != 0: 0; _temp832= _temp832->tl, _temp836= _temp836->tl){
Cyc_Tcutil_unify_tqual((*(( struct _tuple5*) _temp832->hd)).f1,(*(( struct
_tuple5*) _temp836->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple5*) _temp832->hd)).f2,(*((
struct _tuple5*) _temp836->hd)).f2);} if( _temp832 == 0? _temp836 == 0: 0){
return;} goto _LL631; _LL659: return; _LL661: Cyc_Tcutil_unify_it( _temp848,
_temp844); return; _LL663: goto _LL665; _LL665: goto _LL667; _LL667: goto _LL669;
_LL669: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto _LL631;}
_LL671: goto _LL631; _LL631:;}( void) _throw(({ struct Cyc_Tcutil_Unify_struct*
_temp896=( struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));*
_temp896=( struct Cyc_Tcutil_Unify_struct){.tag= Cyc_Tcutil_Unify_tag};( struct
_xenum_struct*) _temp896;}));} int Cyc_Tcutil_is_arithmetic_type( void* t){ void*
_temp897= Cyc_Tcutil_compress( t); void* _temp907; void* _temp909; _LL899: if((
unsigned int) _temp897 > 5u?(( struct _enum_struct*) _temp897)->tag == Cyc_Absyn_IntType_tag:
0){ _LL910: _temp909=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp897)->f1;
goto _LL908; _LL908: _temp907=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp897)->f2; goto _LL900;} else{ goto _LL901;} _LL901: if( _temp897 == Cyc_Absyn_FloatType){
goto _LL902;} else{ goto _LL903;} _LL903: if( _temp897 == Cyc_Absyn_DoubleType){
goto _LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL900: return 1; _LL902:
return 1; _LL904: return 1; _LL906: return 0; _LL898:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp912=({ struct _tuple8 _temp911; _temp911.f1= t1; _temp911.f2=
t2; _temp911;}); void* _temp934; void* _temp936; void* _temp938; void* _temp940;
void* _temp942; void* _temp944; void* _temp946; void* _temp948; void* _temp950;
void* _temp952; void* _temp954; void* _temp956; void* _temp958; void* _temp960;
void* _temp962; void* _temp964; void* _temp966; void* _temp968; void* _temp970;
void* _temp972; void* _temp974; void* _temp976; void* _temp978; void* _temp980;
void* _temp982; void* _temp984; void* _temp986; void* _temp988; void* _temp990;
void* _temp992; void* _temp994; void* _temp996; void* _temp998; void* _temp1000;
void* _temp1002; void* _temp1004; void* _temp1006; void* _temp1008; void*
_temp1010; void* _temp1012; void* _temp1014; void* _temp1016; _LL914: _LL937:
_temp936= _temp912.f1; if( _temp936 == Cyc_Absyn_DoubleType){ goto _LL935;}
else{ goto _LL916;} _LL935: _temp934= _temp912.f2; if( _temp934 == Cyc_Absyn_FloatType){
goto _LL915;} else{ goto _LL916;} _LL916: _LL945: _temp944= _temp912.f1; if(
_temp944 == Cyc_Absyn_DoubleType){ goto _LL939;} else{ goto _LL918;} _LL939:
_temp938= _temp912.f2; if(( unsigned int) _temp938 > 5u?(( struct _enum_struct*)
_temp938)->tag == Cyc_Absyn_IntType_tag: 0){ _LL943: _temp942=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp938)->f1; goto _LL941; _LL941: _temp940=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp938)->f2; goto _LL917;} else{ goto _LL918;}
_LL918: _LL953: _temp952= _temp912.f1; if( _temp952 == Cyc_Absyn_FloatType){
goto _LL947;} else{ goto _LL920;} _LL947: _temp946= _temp912.f2; if((
unsigned int) _temp946 > 5u?(( struct _enum_struct*) _temp946)->tag == Cyc_Absyn_IntType_tag:
0){ _LL951: _temp950=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp946)->f1;
goto _LL949; _LL949: _temp948=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp946)->f2; goto _LL919;} else{ goto _LL920;} _LL920: _LL961: _temp960=
_temp912.f1; if(( unsigned int) _temp960 > 5u?(( struct _enum_struct*) _temp960)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL965: _temp964=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp960)->f1; goto _LL963; _LL963: _temp962=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp960)->f2; if( _temp962 == Cyc_Absyn_B8){ goto _LL955;} else{ goto _LL922;}}
else{ goto _LL922;} _LL955: _temp954= _temp912.f2; if(( unsigned int) _temp954 >
5u?(( struct _enum_struct*) _temp954)->tag == Cyc_Absyn_IntType_tag: 0){ _LL959:
_temp958=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp954)->f1; goto _LL957;
_LL957: _temp956=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp954)->f2; if(
_temp956 == Cyc_Absyn_B8){ goto _LL921;} else{ goto _LL922;}} else{ goto _LL922;}
_LL922: _LL969: _temp968= _temp912.f1; if(( unsigned int) _temp968 > 5u?((
struct _enum_struct*) _temp968)->tag == Cyc_Absyn_IntType_tag: 0){ _LL973:
_temp972=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp968)->f1; goto _LL971;
_LL971: _temp970=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp968)->f2; if(
_temp970 == Cyc_Absyn_B8){ goto _LL967;} else{ goto _LL924;}} else{ goto _LL924;}
_LL967: _temp966= _temp912.f2; goto _LL923; _LL924: _LL977: _temp976= _temp912.f1;
if(( unsigned int) _temp976 > 5u?(( struct _enum_struct*) _temp976)->tag == Cyc_Absyn_IntType_tag:
0){ _LL981: _temp980=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp976)->f1;
goto _LL979; _LL979: _temp978=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp976)->f2; if( _temp978 == Cyc_Absyn_B4){ goto _LL975;} else{ goto _LL926;}}
else{ goto _LL926;} _LL975: _temp974= _temp912.f2; if( _temp974 == Cyc_Absyn_FloatType){
goto _LL925;} else{ goto _LL926;} _LL926: _LL989: _temp988= _temp912.f1; if((
unsigned int) _temp988 > 5u?(( struct _enum_struct*) _temp988)->tag == Cyc_Absyn_IntType_tag:
0){ _LL993: _temp992=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp988)->f1;
goto _LL991; _LL991: _temp990=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp988)->f2; if( _temp990 == Cyc_Absyn_B4){ goto _LL983;} else{ goto _LL928;}}
else{ goto _LL928;} _LL983: _temp982= _temp912.f2; if(( unsigned int) _temp982 >
5u?(( struct _enum_struct*) _temp982)->tag == Cyc_Absyn_IntType_tag: 0){ _LL987:
_temp986=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp982)->f1; goto _LL985;
_LL985: _temp984=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp982)->f2; if(
_temp984 == Cyc_Absyn_B2){ goto _LL927;} else{ goto _LL928;}} else{ goto _LL928;}
_LL928: _LL1001: _temp1000= _temp912.f1; if(( unsigned int) _temp1000 > 5u?((
struct _enum_struct*) _temp1000)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1005:
_temp1004=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1000)->f1; goto
_LL1003; _LL1003: _temp1002=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1000)->f2; if( _temp1002 == Cyc_Absyn_B4){ goto _LL995;} else{ goto _LL930;}}
else{ goto _LL930;} _LL995: _temp994= _temp912.f2; if(( unsigned int) _temp994 >
5u?(( struct _enum_struct*) _temp994)->tag == Cyc_Absyn_IntType_tag: 0){ _LL999:
_temp998=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp994)->f1; goto _LL997;
_LL997: _temp996=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp994)->f2; if(
_temp996 == Cyc_Absyn_B1){ goto _LL929;} else{ goto _LL930;}} else{ goto _LL930;}
_LL930: _LL1013: _temp1012= _temp912.f1; if(( unsigned int) _temp1012 > 5u?((
struct _enum_struct*) _temp1012)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1017:
_temp1016=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1012)->f1; goto
_LL1015; _LL1015: _temp1014=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1012)->f2; if( _temp1014 == Cyc_Absyn_B2){ goto _LL1007;} else{ goto _LL932;}}
else{ goto _LL932;} _LL1007: _temp1006= _temp912.f2; if(( unsigned int)
_temp1006 > 5u?(( struct _enum_struct*) _temp1006)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1011: _temp1010=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1006)->f1;
goto _LL1009; _LL1009: _temp1008=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1006)->f2; if( _temp1008 == Cyc_Absyn_B1){ goto _LL931;} else{ goto _LL932;}}
else{ goto _LL932;} _LL932: goto _LL933; _LL915: return 1; _LL917: return 1;
_LL919: return 1; _LL921: return 0; _LL923: return 1; _LL925: return 1; _LL927:
return 1; _LL929: return 1; _LL931: return 1; _LL933: return 0; _LL913:;}} int
Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_List_List*
es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List* el= es; for(
0; el != 0; el= el->tl){ void* t1= Cyc_Tcutil_compress(( void*)((( struct Cyc_Absyn_Exp*)
el->hd)->topt)->v); if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type
== 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*) max_arith_type->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1018=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1018->v=( void*) t1; _temp1018;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*) max_arith_type->v)){
return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0; el= el->tl){ if( !
Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*) el->hd, t)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) el->hd)->loc,({ struct _tagged_string _temp1019= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1020= Cyc_Absynpp_typ2string(( void*)((( struct
Cyc_Absyn_Exp*) el->hd)->topt)->v); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1019.last_plus_one - _temp1019.curr, _temp1019.curr, _temp1020.last_plus_one
- _temp1020.curr, _temp1020.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1021= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1027; _LL1023: if(( unsigned int) _temp1021 > 5u?(( struct _enum_struct*)
_temp1021)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1028: _temp1027=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1021)->f1; goto
_LL1024;} else{ goto _LL1025;} _LL1025: goto _LL1026; _LL1024: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1022; _LL1026: return 0; _LL1022:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1029= Cyc_Tcutil_compress(
t); void* _temp1035; void* _temp1037; _LL1031: if(( unsigned int) _temp1029 > 5u?((
struct _enum_struct*) _temp1029)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1038:
_temp1037=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1029)->f1; goto
_LL1036; _LL1036: _temp1035=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1029)->f2; goto _LL1032;} else{ goto _LL1033;} _LL1033: goto _LL1034;
_LL1032: return 1; _LL1034: return 0; _LL1030:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1039=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1040; _temp1040.curr= _temp1039; _temp1040.base= _temp1039;
_temp1040.last_plus_one= _temp1039 + 44; _temp1040;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1041=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041;
_temp1042.last_plus_one= _temp1041 + 44; _temp1042;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1044=({ struct _tuple8 _temp1043; _temp1043.f1= t1; _temp1043.f2= t2;
_temp1043;}); void* _temp1052; struct Cyc_Absyn_PtrInfo _temp1054; void*
_temp1056; struct Cyc_Absyn_PtrInfo _temp1058; void* _temp1060; struct Cyc_Absyn_Exp*
_temp1062; struct Cyc_Absyn_Tqual* _temp1064; void* _temp1066; void* _temp1068;
struct Cyc_Absyn_Exp* _temp1070; struct Cyc_Absyn_Tqual* _temp1072; void*
_temp1074; _LL1046: _LL1057: _temp1056= _temp1044.f1; if(( unsigned int)
_temp1056 > 5u?(( struct _enum_struct*) _temp1056)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1059: _temp1058=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1056)->f1; goto _LL1053;} else{ goto _LL1048;} _LL1053: _temp1052=
_temp1044.f2; if(( unsigned int) _temp1052 > 5u?(( struct _enum_struct*)
_temp1052)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1055: _temp1054=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1052)->f1; goto
_LL1047;} else{ goto _LL1048;} _LL1048: _LL1069: _temp1068= _temp1044.f1; if((
unsigned int) _temp1068 > 5u?(( struct _enum_struct*) _temp1068)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1075: _temp1074=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1068)->f1;
goto _LL1073; _LL1073: _temp1072=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1068)->f2; goto _LL1071; _LL1071: _temp1070=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1068)->f3; goto _LL1061;} else{ goto
_LL1050;} _LL1061: _temp1060= _temp1044.f2; if(( unsigned int) _temp1060 > 5u?((
struct _enum_struct*) _temp1060)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1067:
_temp1066=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1060)->f1; goto
_LL1065; _LL1065: _temp1064=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1060)->f2; goto _LL1063; _LL1063: _temp1062=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1060)->f3; goto _LL1049;} else{ goto
_LL1050;} _LL1050: goto _LL1051; _LL1047: { int okay= 1; if( !(( int(*)( int(*
cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1058.nullable, _temp1054.nullable)){ void* _temp1076=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1058.nullable))->v; int _temp1082; _LL1078: if(( unsigned int) _temp1076 >
1u?(( struct _enum_struct*) _temp1076)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL1083: _temp1082=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1076)->f1;
goto _LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081; _LL1079: okay= !
_temp1082; goto _LL1077; _LL1081:( void) _throw((( struct _xenum_struct*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1084=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1085; _temp1085.curr= _temp1084; _temp1085.base= _temp1084; _temp1085.last_plus_one=
_temp1084 + 30; _temp1085;}))); _LL1077:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1058.bounds, _temp1054.bounds)){ struct _tuple8
_temp1087=({ struct _tuple8 _temp1086; _temp1086.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1058.bounds))->v;
_temp1086.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1054.bounds))->v; _temp1086;}); void* _temp1095;
void* _temp1097; void* _temp1099; void* _temp1101; struct Cyc_Absyn_Exp*
_temp1103; void* _temp1105; void* _temp1107; void* _temp1109; void* _temp1111;
_LL1089: _LL1100: _temp1099= _temp1087.f1; if(( unsigned int) _temp1099 > 1u?((
struct _enum_struct*) _temp1099)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1102:
_temp1101=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1099)->f1; if((
unsigned int) _temp1101 > 1u?(( struct _enum_struct*) _temp1101)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1104: _temp1103=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1101)->f1; goto _LL1096;} else{ goto _LL1091;}} else{ goto _LL1091;}
_LL1096: _temp1095= _temp1087.f2; if(( unsigned int) _temp1095 > 1u?(( struct
_enum_struct*) _temp1095)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1098:
_temp1097=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1095)->f1; if(
_temp1097 == Cyc_Absyn_Unknown_b){ goto _LL1090;} else{ goto _LL1091;}} else{
goto _LL1091;} _LL1091: _LL1110: _temp1109= _temp1087.f1; if(( unsigned int)
_temp1109 > 1u?(( struct _enum_struct*) _temp1109)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1112: _temp1111=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1109)->f1;
if( _temp1111 == Cyc_Absyn_Unknown_b){ goto _LL1106;} else{ goto _LL1093;}}
else{ goto _LL1093;} _LL1106: _temp1105= _temp1087.f2; if(( unsigned int)
_temp1105 > 1u?(( struct _enum_struct*) _temp1105)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1108: _temp1107=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1105)->f1;
if( _temp1107 == Cyc_Absyn_Unknown_b){ goto _LL1092;} else{ goto _LL1093;}}
else{ goto _LL1093;} _LL1093: goto _LL1094; _LL1090: okay= 1; goto _LL1088;
_LL1092: okay= 1; goto _LL1088; _LL1094: okay= 0; goto _LL1088; _LL1088:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1058.elt_typ,( void*) _temp1054.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1058.rgn_typ,( void*) _temp1054.rgn_typ): 0)?
!( _temp1058.tq)->q_const? 1:( _temp1054.tq)->q_const: 0;} _LL1049: { int okay;
okay= Cyc_Evexp_eval_const_uint_exp( _temp1070) >= Cyc_Evexp_eval_const_uint_exp(
_temp1062); return( okay? Cyc_Tcutil_unify( _temp1074, _temp1066): 0)? !
_temp1072->q_const? 1: _temp1064->q_const: 0;} _LL1051: return Cyc_Tcutil_unify(
t1, t2); _LL1045:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type( t1)?
Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision( t1,
t2)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char* _temp1113=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1114; _temp1114.curr= _temp1113; _temp1114.base=
_temp1113; _temp1114.last_plus_one= _temp1113 + 44; _temp1114;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1115= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1116= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1115.last_plus_one - _temp1115.curr, _temp1115.curr, _temp1116.last_plus_one
- _temp1116.curr, _temp1116.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v);
void* t;{ void* _temp1117= old_typ; void* _temp1127; void* _temp1129; _LL1119:
if(( unsigned int) _temp1117 > 5u?(( struct _enum_struct*) _temp1117)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL1130: _temp1129=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1117)->f1; goto _LL1128; _LL1128: _temp1127=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1117)->f2; goto _LL1120;} else{ goto _LL1121;} _LL1121: if( _temp1117 ==
Cyc_Absyn_FloatType){ goto _LL1122;} else{ goto _LL1123;} _LL1123: if( _temp1117
== Cyc_Absyn_DoubleType){ goto _LL1124;} else{ goto _LL1125;} _LL1125: goto
_LL1126; _LL1120:{ void* _temp1131= _temp1127; _LL1133: if( _temp1131 == Cyc_Absyn_B1){
goto _LL1134;} else{ goto _LL1135;} _LL1135: if( _temp1131 == Cyc_Absyn_B2){
goto _LL1136;} else{ goto _LL1137;} _LL1137: if( _temp1131 == Cyc_Absyn_B4){
goto _LL1138;} else{ goto _LL1139;} _LL1139: if( _temp1131 == Cyc_Absyn_B8){
goto _LL1140;} else{ goto _LL1132;} _LL1134: t= Cyc_Absyn_sint_t; goto _LL1132;
_LL1136: t= Cyc_Absyn_sint_t; goto _LL1132; _LL1138: t= _temp1129 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1132; _LL1140: t= _temp1129 == Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1132; _LL1132:;} goto _LL1118;
_LL1122: t= Cyc_Absyn_float_t; goto _LL1118; _LL1124: t= Cyc_Absyn_double_t;
goto _LL1118; _LL1126: return 0; _LL1118:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc,
t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1141= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1142= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1141.last_plus_one - _temp1141.curr, _temp1141.curr, _temp1142.last_plus_one
- _temp1142.curr, _temp1142.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1143= Cyc_Tcutil_compress( t); void* _temp1153; void*
_temp1155; _LL1145: if(( unsigned int) _temp1143 > 5u?(( struct _enum_struct*)
_temp1143)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1156: _temp1155=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1143)->f1; goto _LL1154; _LL1154:
_temp1153=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1143)->f2; goto
_LL1146;} else{ goto _LL1147;} _LL1147: if( _temp1143 == Cyc_Absyn_FloatType){
goto _LL1148;} else{ goto _LL1149;} _LL1149: if( _temp1143 == Cyc_Absyn_DoubleType){
goto _LL1150;} else{ goto _LL1151;} _LL1151: goto _LL1152; _LL1146: return 1;
_LL1148: return 1; _LL1150: return 1; _LL1152: return 0; _LL1144:;} static
struct _tuple5* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct
_tuple4* x){ return({ struct _tuple5* _temp1157=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp1157->f1=(* x).f2; _temp1157->f2= Cyc_Tcutil_substitute(
inst,(* x).f3); _temp1157;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1158= t1; struct Cyc_List_List* _temp1168; struct Cyc_Absyn_Structdecl**
_temp1170; struct Cyc_List_List* _temp1172; struct _tuple1* _temp1174; _LL1160:
if( _temp1158 == Cyc_Absyn_VoidType){ goto _LL1161;} else{ goto _LL1162;}
_LL1162: if(( unsigned int) _temp1158 > 5u?(( struct _enum_struct*) _temp1158)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1169: _temp1168=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1158)->f1; goto _LL1163;} else{ goto
_LL1164;} _LL1164: if(( unsigned int) _temp1158 > 5u?(( struct _enum_struct*)
_temp1158)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1175: _temp1174=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1158)->f1; goto _LL1173;
_LL1173: _temp1172=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1158)->f2; goto _LL1171; _LL1171: _temp1170=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1158)->f3; goto _LL1165;} else{ goto
_LL1166;} _LL1166: goto _LL1167; _LL1161: return 0; _LL1163: return _temp1168;
_LL1165: if( _temp1170 == 0? 1: _temp1174 == 0){ return({ struct Cyc_List_List*
_temp1176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1176->hd=( void*)({ struct _tuple5* _temp1177=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp1177->f1= Cyc_Absyn_empty_tqual(); _temp1177->f2=
t1; _temp1177;}); _temp1176->tl= 0; _temp1176;});}{ struct Cyc_Absyn_Structdecl*
sd=* _temp1170; struct _tuple1 n=* _temp1174; struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1172); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1178=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1178->hd=( void*)({ struct _tuple5* _temp1179=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp1179->f1= Cyc_Absyn_empty_tqual(); _temp1179->f2=
t1; _temp1179;}); _temp1178->tl= 0; _temp1178;});} return(( struct Cyc_List_List*(*)(
struct _tuple5*(* f)( struct Cyc_List_List*, struct _tuple4*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, inst,(
struct Cyc_List_List*)( sd->fields)->v);} _LL1167: return({ struct Cyc_List_List*
_temp1180=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1180->hd=( void*)({ struct _tuple5* _temp1181=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp1181->f1= Cyc_Absyn_empty_tqual(); _temp1181->f2=
t1; _temp1181;}); _temp1180->tl= 0; _temp1180;}); _LL1159:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2);
static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1, t2)){ return 1;}{ struct
Cyc_List_List* a= assume; for( 0; a != 0; a= a->tl){ if( Cyc_Tcutil_unify( t1,(*((
struct _tuple8*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*(( struct _tuple8*) a->hd)).f2):
0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1183=({ struct _tuple8 _temp1182; _temp1182.f1= t1;
_temp1182.f2= t2; _temp1182;}); void* _temp1189; struct Cyc_Absyn_PtrInfo
_temp1191; struct Cyc_Absyn_Conref* _temp1193; struct Cyc_Absyn_Tqual* _temp1195;
struct Cyc_Absyn_Conref* _temp1197; void* _temp1199; void* _temp1201; void*
_temp1203; struct Cyc_Absyn_PtrInfo _temp1205; struct Cyc_Absyn_Conref*
_temp1207; struct Cyc_Absyn_Tqual* _temp1209; struct Cyc_Absyn_Conref* _temp1211;
void* _temp1213; void* _temp1215; _LL1185: _LL1204: _temp1203= _temp1183.f1; if((
unsigned int) _temp1203 > 5u?(( struct _enum_struct*) _temp1203)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1206: _temp1205=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1203)->f1; _LL1216: _temp1215=( void*) _temp1205.elt_typ; goto _LL1214;
_LL1214: _temp1213=( void*) _temp1205.rgn_typ; goto _LL1212; _LL1212: _temp1211=(
struct Cyc_Absyn_Conref*) _temp1205.nullable; goto _LL1210; _LL1210: _temp1209=(
struct Cyc_Absyn_Tqual*) _temp1205.tq; goto _LL1208; _LL1208: _temp1207=( struct
Cyc_Absyn_Conref*) _temp1205.bounds; goto _LL1190;} else{ goto _LL1187;} _LL1190:
_temp1189= _temp1183.f2; if(( unsigned int) _temp1189 > 5u?(( struct
_enum_struct*) _temp1189)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1192:
_temp1191=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1189)->f1; _LL1202: _temp1201=( void*) _temp1191.elt_typ; goto _LL1200;
_LL1200: _temp1199=( void*) _temp1191.rgn_typ; goto _LL1198; _LL1198: _temp1197=(
struct Cyc_Absyn_Conref*) _temp1191.nullable; goto _LL1196; _LL1196: _temp1195=(
struct Cyc_Absyn_Tqual*) _temp1191.tq; goto _LL1194; _LL1194: _temp1193=( struct
Cyc_Absyn_Conref*) _temp1191.bounds; goto _LL1186;} else{ goto _LL1187;} _LL1187:
goto _LL1188; _LL1186: if( _temp1209->q_const? ! _temp1195->q_const: 0){ return
0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1211,
_temp1197)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1211): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1197): 0){ return 0;}{ void* _temp1217= _temp1213; _LL1219: if( _temp1217
== Cyc_Absyn_HeapRgn){ goto _LL1220;} else{ goto _LL1221;} _LL1221: goto _LL1222;
_LL1220: goto _LL1218; _LL1222: Cyc_Tcutil_unify( _temp1213, _temp1199); goto
_LL1218; _LL1218:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1207, _temp1193)){ struct _tuple8 _temp1224=({ struct _tuple8 _temp1223;
_temp1223.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1207); _temp1223.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1193); _temp1223;}); void* _temp1232; void* _temp1234; struct Cyc_Absyn_Exp*
_temp1236; void* _temp1238; struct Cyc_Absyn_Exp* _temp1240; void* _temp1242;
struct Cyc_Absyn_Exp* _temp1244; _LL1226: _LL1235: _temp1234= _temp1224.f1; if((
unsigned int) _temp1234 > 1u?(( struct _enum_struct*) _temp1234)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1237: _temp1236=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1234)->f1; goto _LL1233;} else{ goto _LL1228;} _LL1233: _temp1232=
_temp1224.f2; if( _temp1232 == Cyc_Absyn_Unknown_b){ goto _LL1227;} else{ goto
_LL1228;} _LL1228: _LL1243: _temp1242= _temp1224.f1; if(( unsigned int)
_temp1242 > 1u?(( struct _enum_struct*) _temp1242)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1245: _temp1244=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1242)->f1; goto _LL1239;} else{ goto _LL1230;} _LL1239: _temp1238=
_temp1224.f2; if(( unsigned int) _temp1238 > 1u?(( struct _enum_struct*)
_temp1238)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1241: _temp1240=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1238)->f1; goto _LL1229;} else{ goto
_LL1230;} _LL1230: goto _LL1231; _LL1227: goto _LL1225; _LL1229: if( Cyc_Evexp_eval_const_uint_exp(
_temp1244) < Cyc_Evexp_eval_const_uint_exp( _temp1240)){ return 0;} goto _LL1225;
_LL1231: return 0; _LL1225:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1246=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1246->hd=( void*)({ struct _tuple8* _temp1247=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1247->f1= t1; _temp1247->f2= t2; _temp1247;});
_temp1246->tl= assume; _temp1246;}), _temp1215, _temp1201); _LL1188: return 0;
_LL1184:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2= tqs2->tl, tqs1= tqs1->tl){ if( tqs1 == 0){ return 0;}{ struct
_tuple5 _temp1250; void* _temp1251; struct Cyc_Absyn_Tqual* _temp1253; struct
_tuple5* _temp1248=( struct _tuple5*) tqs1->hd; _temp1250=* _temp1248; _LL1254:
_temp1253= _temp1250.f1; goto _LL1252; _LL1252: _temp1251= _temp1250.f2; goto
_LL1249; _LL1249: { struct _tuple5 _temp1257; void* _temp1258; struct Cyc_Absyn_Tqual*
_temp1260; struct _tuple5* _temp1255=( struct _tuple5*) tqs2->hd; _temp1257=*
_temp1255; _LL1261: _temp1260= _temp1257.f1; goto _LL1259; _LL1259: _temp1258=
_temp1257.f2; goto _LL1256; _LL1256: if( _temp1260->q_const? Cyc_Tcutil_subtype(
te, assume, _temp1251, _temp1258): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1251, _temp1258)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;}{ void* _temp1262= t1; struct Cyc_Absyn_Enumdecl** _temp1278;
struct Cyc_List_List* _temp1280; struct _tuple1* _temp1282; struct Cyc_Absyn_PtrInfo
_temp1284; struct Cyc_Absyn_Conref* _temp1286; struct Cyc_Absyn_Tqual* _temp1288;
struct Cyc_Absyn_Conref* _temp1290; void* _temp1292; void* _temp1294; struct Cyc_Absyn_Exp*
_temp1296; struct Cyc_Absyn_Tqual* _temp1298; void* _temp1300; void* _temp1302;
void* _temp1304; _LL1264: if(( unsigned int) _temp1262 > 5u?(( struct
_enum_struct*) _temp1262)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL1283: _temp1282=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1262)->f1; goto
_LL1281; _LL1281: _temp1280=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1262)->f2; goto _LL1279; _LL1279: _temp1278=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp1262)->f3; goto _LL1265;} else{ goto
_LL1266;} _LL1266: if(( unsigned int) _temp1262 > 5u?(( struct _enum_struct*)
_temp1262)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1285: _temp1284=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1262)->f1;
_LL1295: _temp1294=( void*) _temp1284.elt_typ; goto _LL1293; _LL1293: _temp1292=(
void*) _temp1284.rgn_typ; goto _LL1291; _LL1291: _temp1290=( struct Cyc_Absyn_Conref*)
_temp1284.nullable; goto _LL1289; _LL1289: _temp1288=( struct Cyc_Absyn_Tqual*)
_temp1284.tq; goto _LL1287; _LL1287: _temp1286=( struct Cyc_Absyn_Conref*)
_temp1284.bounds; goto _LL1267;} else{ goto _LL1268;} _LL1268: if(( unsigned int)
_temp1262 > 5u?(( struct _enum_struct*) _temp1262)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1301: _temp1300=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1262)->f1;
goto _LL1299; _LL1299: _temp1298=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1262)->f2; goto _LL1297; _LL1297: _temp1296=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1262)->f3; goto _LL1269;} else{ goto
_LL1270;} _LL1270: if(( unsigned int) _temp1262 > 5u?(( struct _enum_struct*)
_temp1262)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1305: _temp1304=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1262)->f1; goto _LL1303; _LL1303:
_temp1302=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1262)->f2; goto
_LL1271;} else{ goto _LL1272;} _LL1272: if( _temp1262 == Cyc_Absyn_FloatType){
goto _LL1273;} else{ goto _LL1274;} _LL1274: if( _temp1262 == Cyc_Absyn_DoubleType){
goto _LL1275;} else{ goto _LL1276;} _LL1276: goto _LL1277; _LL1265: return Cyc_Tcutil_coerceable(
t2); _LL1267: { void* _temp1306= t2; struct Cyc_Absyn_PtrInfo _temp1312; struct
Cyc_Absyn_Conref* _temp1314; struct Cyc_Absyn_Tqual* _temp1316; struct Cyc_Absyn_Conref*
_temp1318; void* _temp1320; void* _temp1322; _LL1308: if(( unsigned int)
_temp1306 > 5u?(( struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1313: _temp1312=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1306)->f1; _LL1323: _temp1322=( void*) _temp1312.elt_typ; goto _LL1321;
_LL1321: _temp1320=( void*) _temp1312.rgn_typ; goto _LL1319; _LL1319: _temp1318=(
struct Cyc_Absyn_Conref*) _temp1312.nullable; goto _LL1317; _LL1317: _temp1316=(
struct Cyc_Absyn_Tqual*) _temp1312.tq; goto _LL1315; _LL1315: _temp1314=( struct
Cyc_Absyn_Conref*) _temp1312.bounds; goto _LL1309;} else{ goto _LL1310;} _LL1310:
goto _LL1311; _LL1309: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1324=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1324->hd=( void*)({ struct _tuple8* _temp1325=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1325->f1= t1; _temp1325->f2= t2; _temp1325;});
_temp1324->tl= 0; _temp1324;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te,( struct
Cyc_List_List*) assump, _temp1294, _temp1322)? ! _temp1288->q_const? 1:
_temp1316->q_const: 0; int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1286, _temp1314); if( ! bounds_ok){ struct _tuple8
_temp1327=({ struct _tuple8 _temp1326; _temp1326.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1286); _temp1326.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1314); _temp1326;}); void* _temp1333; struct Cyc_Absyn_Exp*
_temp1335; void* _temp1337; struct Cyc_Absyn_Exp* _temp1339; _LL1329: _LL1338:
_temp1337= _temp1327.f1; if(( unsigned int) _temp1337 > 1u?(( struct
_enum_struct*) _temp1337)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1340: _temp1339=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1337)->f1; goto
_LL1334;} else{ goto _LL1331;} _LL1334: _temp1333= _temp1327.f2; if((
unsigned int) _temp1333 > 1u?(( struct _enum_struct*) _temp1333)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1336: _temp1335=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1333)->f1; goto _LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332;
_LL1330: if( Cyc_Evexp_eval_const_uint_exp( _temp1339) >= Cyc_Evexp_eval_const_uint_exp(
_temp1335)){ bounds_ok= 1;} goto _LL1328; _LL1332: bounds_ok= 1; goto _LL1328;
_LL1328:;} return( bounds_ok? ptrsub: 0)? _temp1292 == Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1292, _temp1320): 0;} _LL1311: return 0; _LL1307:;} _LL1269: { void*
_temp1341= t2; struct Cyc_Absyn_Exp* _temp1347; struct Cyc_Absyn_Tqual*
_temp1349; void* _temp1351; _LL1343: if(( unsigned int) _temp1341 > 5u?(( struct
_enum_struct*) _temp1341)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1352:
_temp1351=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1341)->f1; goto
_LL1350; _LL1350: _temp1349=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1341)->f2; goto _LL1348; _LL1348: _temp1347=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1341)->f3; goto _LL1344;} else{ goto
_LL1345;} _LL1345: goto _LL1346; _LL1344: { int okay; okay= Cyc_Evexp_eval_const_uint_exp(
_temp1296) >= Cyc_Evexp_eval_const_uint_exp( _temp1347); return( okay? Cyc_Tcutil_unify(
_temp1300, _temp1351): 0)? ! _temp1298->q_const? 1: _temp1349->q_const: 0;}
_LL1346: return 0; _LL1342:;} _LL1271: goto _LL1273; _LL1273: goto _LL1275;
_LL1275: return Cyc_Tcutil_coerceable( t2); _LL1277: return 0; _LL1263:;}} void
Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e,
void* t){ if( ! Cyc_Tcutil_unify(( void*)( e->topt)->v, t)){ struct Cyc_Absyn_Exp*
inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp1353=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1353->tag= Cyc_Absyn_Cast_e_tag; _temp1353->f1=( void*) t; _temp1353->f2=
inner;( void*) _temp1353;})); e->topt=({ struct Cyc_Core_Opt* _temp1354=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1354->v=( void*) t;
_temp1354;});}} int Cyc_Tcutil_comparison_promote( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp1355= Cyc_Tcutil_compress(( void*)(
e->topt)->v); void* _temp1365; void* _temp1367; _LL1357: if(( unsigned int)
_temp1355 > 5u?(( struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1368: _temp1367=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1355)->f1;
goto _LL1366; _LL1366: _temp1365=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1355)->f2; goto _LL1358;} else{ goto _LL1359;} _LL1359: if( _temp1355 ==
Cyc_Absyn_FloatType){ goto _LL1360;} else{ goto _LL1361;} _LL1361: if( _temp1355
== Cyc_Absyn_DoubleType){ goto _LL1362;} else{ goto _LL1363;} _LL1363: goto
_LL1364; _LL1358:{ void* _temp1369= _temp1365; _LL1371: if( _temp1369 == Cyc_Absyn_B1){
goto _LL1372;} else{ goto _LL1373;} _LL1373: if( _temp1369 == Cyc_Absyn_B2){
goto _LL1374;} else{ goto _LL1375;} _LL1375: if( _temp1369 == Cyc_Absyn_B4){
goto _LL1376;} else{ goto _LL1377;} _LL1377: if( _temp1369 == Cyc_Absyn_B8){
goto _LL1378;} else{ goto _LL1370;} _LL1372: t= Cyc_Absyn_sint_t; goto _LL1370;
_LL1374: t= Cyc_Absyn_sint_t; goto _LL1370; _LL1376: t= _temp1367 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1370; _LL1378: return 1; _LL1370:;}
goto _LL1356; _LL1360: t= Cyc_Absyn_float_t; goto _LL1356; _LL1362: return 1;
_LL1364: return 0; _LL1356:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;}
int Cyc_Tcutil_arithmetic_promote( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* t;{ void*
_temp1379= old_typ; void* _temp1393; void* _temp1395; struct Cyc_Absyn_Enumdecl**
_temp1397; struct Cyc_List_List* _temp1399; struct _tuple1* _temp1401; int
_temp1403; struct Cyc_Core_Opt* _temp1405; void* _temp1407; _LL1381: if((
unsigned int) _temp1379 > 5u?(( struct _enum_struct*) _temp1379)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1396: _temp1395=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1379)->f1;
goto _LL1394; _LL1394: _temp1393=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1379)->f2; goto _LL1382;} else{ goto _LL1383;} _LL1383: if( _temp1379 ==
Cyc_Absyn_FloatType){ goto _LL1384;} else{ goto _LL1385;} _LL1385: if( _temp1379
== Cyc_Absyn_DoubleType){ goto _LL1386;} else{ goto _LL1387;} _LL1387: if((
unsigned int) _temp1379 > 5u?(( struct _enum_struct*) _temp1379)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1402: _temp1401=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1379)->f1; goto _LL1400; _LL1400: _temp1399=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1379)->f2; goto _LL1398; _LL1398:
_temp1397=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1379)->f3; goto _LL1388;} else{ goto _LL1389;} _LL1389: if(( unsigned int)
_temp1379 > 5u?(( struct _enum_struct*) _temp1379)->tag == Cyc_Absyn_Evar_tag: 0){
_LL1408: _temp1407=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1379)->f1;
goto _LL1406; _LL1406: _temp1405=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1379)->f2; goto _LL1404; _LL1404: _temp1403=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1379)->f3; goto _LL1390;} else{ goto _LL1391;} _LL1391: goto _LL1392;
_LL1382:{ void* _temp1409= _temp1393; _LL1411: if( _temp1409 == Cyc_Absyn_B1){
goto _LL1412;} else{ goto _LL1413;} _LL1413: if( _temp1409 == Cyc_Absyn_B2){
goto _LL1414;} else{ goto _LL1415;} _LL1415: if( _temp1409 == Cyc_Absyn_B4){
goto _LL1416;} else{ goto _LL1417;} _LL1417: if( _temp1409 == Cyc_Absyn_B8){
goto _LL1418;} else{ goto _LL1410;} _LL1412: t= Cyc_Absyn_sint_t; goto _LL1410;
_LL1414: t= Cyc_Absyn_sint_t; goto _LL1410; _LL1416: t= _temp1395 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1410; _LL1418: t= _temp1395 == Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1410; _LL1410:;} goto _LL1380;
_LL1384: t= Cyc_Absyn_float_t; goto _LL1380; _LL1386: t= Cyc_Absyn_double_t;
goto _LL1380; _LL1388: t= Cyc_Absyn_sint_t; goto _LL1380; _LL1390: t= Cyc_Absyn_sint_t;
if( ! Cyc_Tcutil_unify( old_typ, t)){ return 0;} goto _LL1380; _LL1392: return 0;
_LL1380:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;} int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* t;{ void* _temp1419= old_typ; void* _temp1429; void*
_temp1431; struct Cyc_Absyn_Enumdecl** _temp1433; struct Cyc_List_List*
_temp1435; struct _tuple1* _temp1437; int _temp1439; struct Cyc_Core_Opt*
_temp1441; void* _temp1443; _LL1421: if(( unsigned int) _temp1419 > 5u?(( struct
_enum_struct*) _temp1419)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1432: _temp1431=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1419)->f1; goto _LL1430; _LL1430:
_temp1429=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1419)->f2; goto
_LL1422;} else{ goto _LL1423;} _LL1423: if(( unsigned int) _temp1419 > 5u?((
struct _enum_struct*) _temp1419)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL1438:
_temp1437=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1419)->f1;
goto _LL1436; _LL1436: _temp1435=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1419)->f2; goto _LL1434; _LL1434: _temp1433=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp1419)->f3; goto _LL1424;} else{ goto
_LL1425;} _LL1425: if(( unsigned int) _temp1419 > 5u?(( struct _enum_struct*)
_temp1419)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1444: _temp1443=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp1419)->f1; goto _LL1442; _LL1442: _temp1441=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1419)->f2; goto
_LL1440; _LL1440: _temp1439=( int)(( struct Cyc_Absyn_Evar_struct*) _temp1419)->f3;
goto _LL1426;} else{ goto _LL1427;} _LL1427: goto _LL1428; _LL1422:{ void*
_temp1445= _temp1429; _LL1447: if( _temp1445 == Cyc_Absyn_B1){ goto _LL1448;}
else{ goto _LL1449;} _LL1449: if( _temp1445 == Cyc_Absyn_B2){ goto _LL1450;}
else{ goto _LL1451;} _LL1451: if( _temp1445 == Cyc_Absyn_B4){ goto _LL1452;}
else{ goto _LL1453;} _LL1453: if( _temp1445 == Cyc_Absyn_B8){ goto _LL1454;}
else{ goto _LL1446;} _LL1448: t= Cyc_Absyn_sint_t; goto _LL1446; _LL1450: t= Cyc_Absyn_sint_t;
goto _LL1446; _LL1452: t= _temp1431 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL1446; _LL1454: t= _temp1431 == Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL1446; _LL1446:;} goto _LL1420; _LL1424: t= Cyc_Absyn_sint_t;
goto _LL1420; _LL1426: t= Cyc_Absyn_sint_t; if( ! Cyc_Tcutil_unify( old_typ, t)){
return 0;} goto _LL1420; _LL1428: return 0; _LL1420:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp1456=({ struct _tuple8 _temp1455; _temp1455.f1= t1;
_temp1455.f2= t2; _temp1455;}); void* _temp1480; void* _temp1482; void*
_temp1484; void* _temp1486; void* _temp1488; void* _temp1490; void* _temp1492;
void* _temp1494; void* _temp1496; void* _temp1498; void* _temp1500; void*
_temp1502; void* _temp1504; void* _temp1506; void* _temp1508; void* _temp1510;
void* _temp1512; void* _temp1514; void* _temp1516; void* _temp1518; void*
_temp1520; void* _temp1522; void* _temp1524; void* _temp1526; void* _temp1528;
void* _temp1530; void* _temp1532; void* _temp1534; void* _temp1536; void*
_temp1538; void* _temp1540; void* _temp1542; _LL1458: _LL1483: _temp1482=
_temp1456.f1; if( _temp1482 == Cyc_Absyn_DoubleType){ goto _LL1481;} else{ goto
_LL1460;} _LL1481: _temp1480= _temp1456.f2; goto _LL1459; _LL1460: _LL1487:
_temp1486= _temp1456.f1; goto _LL1485; _LL1485: _temp1484= _temp1456.f2; if(
_temp1484 == Cyc_Absyn_DoubleType){ goto _LL1461;} else{ goto _LL1462;} _LL1462:
_LL1491: _temp1490= _temp1456.f1; if( _temp1490 == Cyc_Absyn_FloatType){ goto
_LL1489;} else{ goto _LL1464;} _LL1489: _temp1488= _temp1456.f2; goto _LL1463;
_LL1464: _LL1495: _temp1494= _temp1456.f1; goto _LL1493; _LL1493: _temp1492=
_temp1456.f2; if( _temp1492 == Cyc_Absyn_FloatType){ goto _LL1465;} else{ goto
_LL1466;} _LL1466: _LL1499: _temp1498= _temp1456.f1; if(( unsigned int)
_temp1498 > 5u?(( struct _enum_struct*) _temp1498)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1503: _temp1502=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1498)->f1;
if( _temp1502 == Cyc_Absyn_Unsigned){ goto _LL1501;} else{ goto _LL1468;}
_LL1501: _temp1500=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1498)->f2;
if( _temp1500 == Cyc_Absyn_B8){ goto _LL1497;} else{ goto _LL1468;}} else{ goto
_LL1468;} _LL1497: _temp1496= _temp1456.f2; goto _LL1467; _LL1468: _LL1511:
_temp1510= _temp1456.f1; goto _LL1505; _LL1505: _temp1504= _temp1456.f2; if((
unsigned int) _temp1504 > 5u?(( struct _enum_struct*) _temp1504)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1509: _temp1508=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1504)->f1;
if( _temp1508 == Cyc_Absyn_Unsigned){ goto _LL1507;} else{ goto _LL1470;}
_LL1507: _temp1506=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1504)->f2;
if( _temp1506 == Cyc_Absyn_B8){ goto _LL1469;} else{ goto _LL1470;}} else{ goto
_LL1470;} _LL1470: _LL1515: _temp1514= _temp1456.f1; if(( unsigned int)
_temp1514 > 5u?(( struct _enum_struct*) _temp1514)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1519: _temp1518=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1514)->f1;
if( _temp1518 == Cyc_Absyn_Signed){ goto _LL1517;} else{ goto _LL1472;} _LL1517:
_temp1516=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1514)->f2; if(
_temp1516 == Cyc_Absyn_B8){ goto _LL1513;} else{ goto _LL1472;}} else{ goto
_LL1472;} _LL1513: _temp1512= _temp1456.f2; goto _LL1471; _LL1472: _LL1527:
_temp1526= _temp1456.f1; goto _LL1521; _LL1521: _temp1520= _temp1456.f2; if((
unsigned int) _temp1520 > 5u?(( struct _enum_struct*) _temp1520)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1525: _temp1524=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1520)->f1;
if( _temp1524 == Cyc_Absyn_Signed){ goto _LL1523;} else{ goto _LL1474;} _LL1523:
_temp1522=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1520)->f2; if(
_temp1522 == Cyc_Absyn_B8){ goto _LL1473;} else{ goto _LL1474;}} else{ goto
_LL1474;} _LL1474: _LL1531: _temp1530= _temp1456.f1; if(( unsigned int)
_temp1530 > 5u?(( struct _enum_struct*) _temp1530)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1535: _temp1534=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1530)->f1;
if( _temp1534 == Cyc_Absyn_Unsigned){ goto _LL1533;} else{ goto _LL1476;}
_LL1533: _temp1532=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1530)->f2;
if( _temp1532 == Cyc_Absyn_B4){ goto _LL1529;} else{ goto _LL1476;}} else{ goto
_LL1476;} _LL1529: _temp1528= _temp1456.f2; goto _LL1475; _LL1476: _LL1543:
_temp1542= _temp1456.f1; goto _LL1537; _LL1537: _temp1536= _temp1456.f2; if((
unsigned int) _temp1536 > 5u?(( struct _enum_struct*) _temp1536)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1541: _temp1540=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1536)->f1;
if( _temp1540 == Cyc_Absyn_Unsigned){ goto _LL1539;} else{ goto _LL1478;}
_LL1539: _temp1538=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1536)->f2;
if( _temp1538 == Cyc_Absyn_B4){ goto _LL1477;} else{ goto _LL1478;}} else{ goto
_LL1478;} _LL1478: goto _LL1479; _LL1459: goto _LL1461; _LL1461: return Cyc_Absyn_DoubleType;
_LL1463: goto _LL1465; _LL1465: return Cyc_Absyn_FloatType; _LL1467: goto
_LL1469; _LL1469: return({ struct Cyc_Absyn_IntType_struct* _temp1544=( struct
Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1544->tag= Cyc_Absyn_IntType_tag; _temp1544->f1=( void*) Cyc_Absyn_Unsigned;
_temp1544->f2=( void*) Cyc_Absyn_B8;( void*) _temp1544;}); _LL1471: goto _LL1473;
_LL1473: return({ struct Cyc_Absyn_IntType_struct* _temp1545=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1545->tag= Cyc_Absyn_IntType_tag;
_temp1545->f1=( void*) Cyc_Absyn_Signed; _temp1545->f2=( void*) Cyc_Absyn_B8;(
void*) _temp1545;}); _LL1475: goto _LL1477; _LL1477: return({ struct Cyc_Absyn_IntType_struct*
_temp1546=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1546->tag= Cyc_Absyn_IntType_tag; _temp1546->f1=( void*) Cyc_Absyn_Unsigned;
_temp1546->f2=( void*) Cyc_Absyn_B4;( void*) _temp1546;}); _LL1479: return({
struct Cyc_Absyn_IntType_struct* _temp1547=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1547->tag= Cyc_Absyn_IntType_tag;
_temp1547->f1=( void*) Cyc_Absyn_Signed; _temp1547->f2=( void*) Cyc_Absyn_B4;(
void*) _temp1547;}); _LL1457:;} void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp* e){ void* _temp1548=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1554; struct Cyc_Core_Opt* _temp1556; struct Cyc_Absyn_Exp* _temp1558;
_LL1550: if((( struct _enum_struct*) _temp1548)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1559: _temp1558=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1548)->f1; goto _LL1557; _LL1557: _temp1556=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1548)->f2; if( _temp1556 == 0){ goto
_LL1555;} else{ goto _LL1552;} _LL1555: _temp1554=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1548)->f3; goto _LL1551;} else{ goto
_LL1552;} _LL1552: goto _LL1553; _LL1551: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1560=( char*)"assignment in test"; struct
_tagged_string _temp1561; _temp1561.curr= _temp1560; _temp1561.base= _temp1560;
_temp1561.last_plus_one= _temp1560 + 19; _temp1561;})); goto _LL1549; _LL1553:
goto _LL1549; _LL1549:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v == Cyc_Absyn_No_constr){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1562=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1562->tag= Cyc_Absyn_Forward_constr_tag;
_temp1562->f1= c2;( void*) _temp1562;})); return 1;} else{ if(( void*) c2->v ==
Cyc_Absyn_No_constr){( void*)( c2->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1563=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1563->tag= Cyc_Absyn_Forward_constr_tag;
_temp1563->f1= c1;( void*) _temp1563;})); return 1;} else{ void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq(
k1, k2)){( void*)( c2->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1564=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1564->tag= Cyc_Absyn_Forward_constr_tag;
_temp1564->f1= c1;( void*) _temp1564;})); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1565=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1565->tag= Cyc_Absyn_Forward_constr_tag;
_temp1565->f1= c2;( void*) _temp1565;})); return 1;} else{ return 0;}}}}} static
struct Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tvs == 0){ return({
struct Cyc_List_List* _temp1566=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1566->hd=( void*) tv; _temp1566->tl= 0; _temp1566;});}
else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ if(
Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) == 0){
struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct
Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1567=* tv->name; struct _tagged_string
_temp1568= Cyc_Absynpp_ckind2string( k1); struct _tagged_string _temp1569= Cyc_Absynpp_ckind2string(
k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1567.last_plus_one - _temp1567.curr, _temp1567.curr, _temp1568.last_plus_one
- _temp1568.curr, _temp1568.curr, _temp1569.last_plus_one - _temp1569.curr,
_temp1569.curr);}));} return tvs;} else{ if( tvs2->tl == 0){ tvs2->tl=({ struct
Cyc_List_List* _temp1570=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1570->hd=( void*) tv; _temp1570->tl= 0; _temp1570;}); return tvs;}}}}
return(( struct Cyc_List_List*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1571=( char*)"Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp1572; _temp1572.curr= _temp1571; _temp1572.base=
_temp1571; _temp1572.last_plus_one= _temp1571 + 50; _temp1572;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=
tvs->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b= b->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs->hd)->name,(( struct
Cyc_Absyn_Tvar*) b->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp1573=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1573->hd=( void*)(( struct Cyc_Absyn_Tvar*) tvs->hd);
_temp1573->tl= r; _temp1573;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} static int Cyc_Tcutil_tvar_counter= 0;
struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k){ struct _tagged_string s=
xprintf("#%d",( Cyc_Tcutil_tvar_counter ++)); return({ struct Cyc_Absyn_Tvar*
_temp1574=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp1574->name=({ struct _tagged_string* _temp1575=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1575[ 0]= s; _temp1575;});
_temp1574->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp1574;});} static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct
_tuple4* x){ return({ struct _tuple2* _temp1576=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp1576->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp1577=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1577->v=( void*)(* x).f1; _temp1577;}); _temp1576->f2=(* x).f2; _temp1576->f3=(*
x).f3; _temp1576;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){
if( fd->cached_typ == 0){ return({ struct Cyc_Absyn_FnType_struct* _temp1578=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1578->tag= Cyc_Absyn_FnType_tag; _temp1578->f1=({ struct Cyc_Absyn_FnInfo
_temp1579; _temp1579.tvars= fd->tvs; _temp1579.effect= fd->effect; _temp1579.ret_typ=(
void*)(( void*) fd->ret_type); _temp1579.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp1579.varargs= fd->varargs; _temp1579;});(
void*) _temp1578;});} return( void*)( fd->cached_typ)->v;} static void* Cyc_Tcutil_fst3(
struct _tuple11* t){ return(* t).f1;} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp1580= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1586; int _temp1588; struct Cyc_List_List* _temp1590; void* _temp1592;
struct Cyc_Core_Opt* _temp1594; struct Cyc_List_List* _temp1596; _LL1582: if((
unsigned int) _temp1580 > 5u?(( struct _enum_struct*) _temp1580)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1587: _temp1586=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1580)->f1; _LL1597: _temp1596=( struct Cyc_List_List*) _temp1586.tvars;
goto _LL1595; _LL1595: _temp1594=( struct Cyc_Core_Opt*) _temp1586.effect; goto
_LL1593; _LL1593: _temp1592=( void*) _temp1586.ret_typ; goto _LL1591; _LL1591:
_temp1590=( struct Cyc_List_List*) _temp1586.args; goto _LL1589; _LL1589:
_temp1588=( int) _temp1586.varargs; goto _LL1583;} else{ goto _LL1584;} _LL1584:
goto _LL1585; _LL1583: fd->tvs= _temp1596; fd->effect= _temp1594; goto _LL1581;
_LL1585:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp1598=( char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base=
_temp1598; _temp1599.last_plus_one= _temp1598 + 38; _temp1599;})); return;
_LL1581:;} Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
struct _tagged_string*(*)( struct _tuple4* t)) Cyc_Tcutil_fst3, fd->args), loc,(
struct _tagged_string)({ char* _temp1600=( char*)"function declaration has repeated parameter";
struct _tagged_string _temp1601; _temp1601.curr= _temp1600; _temp1601.base=
_temp1600; _temp1601.last_plus_one= _temp1600 + 44; _temp1601;})); fd->cached_typ=({
struct Cyc_Core_Opt* _temp1602=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1602->v=( void*) t; _temp1602;});} static struct _tuple13*
Cyc_Tcutil_substitute_f1( struct _tuple2* y){ return({ struct _tuple13*
_temp1603=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1603->f1=({
struct _tuple12* _temp1604=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp1604->f1=(* y).f1; _temp1604->f2=(* y).f2; _temp1604;});
_temp1603->f2=(* y).f3; _temp1603;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple13* w){ void* _temp1607; struct _tuple12* _temp1609; struct
_tuple13 _temp1605=* w; _LL1610: _temp1609= _temp1605.f1; goto _LL1608; _LL1608:
_temp1607= _temp1605.f2; goto _LL1606; _LL1606: { struct Cyc_Absyn_Tqual*
_temp1613; struct Cyc_Core_Opt* _temp1615; struct _tuple12 _temp1611=* _temp1609;
_LL1616: _temp1615= _temp1611.f1; goto _LL1614; _LL1614: _temp1613= _temp1611.f2;
goto _LL1612; _LL1612: return({ struct _tuple2* _temp1617=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp1617->f1= _temp1615; _temp1617->f2=
_temp1613; _temp1617->f3= _temp1607; _temp1617;});}} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts); void*
Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ void* _temp1618=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp1658; struct Cyc_Absyn_Structdecl**
_temp1660; struct Cyc_List_List* _temp1662; struct _tuple1* _temp1664; struct
Cyc_Absyn_Enumdecl** _temp1666; struct Cyc_List_List* _temp1668; struct _tuple1*
_temp1670; struct Cyc_Core_Opt* _temp1672; struct Cyc_List_List* _temp1674;
struct _tuple1* _temp1676; struct Cyc_Absyn_Exp* _temp1678; struct Cyc_Absyn_Tqual*
_temp1680; void* _temp1682; struct Cyc_Absyn_PtrInfo _temp1684; struct Cyc_Absyn_Conref*
_temp1686; struct Cyc_Absyn_Tqual* _temp1688; struct Cyc_Absyn_Conref* _temp1690;
void* _temp1692; void* _temp1694; struct Cyc_Absyn_FnInfo _temp1696; int
_temp1698; struct Cyc_List_List* _temp1700; void* _temp1702; struct Cyc_Core_Opt*
_temp1704; struct Cyc_List_List* _temp1706; struct Cyc_List_List* _temp1708; int
_temp1710; struct Cyc_Core_Opt* _temp1712; void* _temp1714; void* _temp1716;
struct Cyc_Absyn_Xenumdecl** _temp1718; struct _tuple1* _temp1720; void*
_temp1722; void* _temp1724; void* _temp1726; struct Cyc_List_List* _temp1728;
_LL1620: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*) _temp1618)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL1659: _temp1658=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1618)->f1; goto _LL1621;} else{ goto
_LL1622;} _LL1622: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*)
_temp1618)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1665: _temp1664=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1618)->f1; goto _LL1663;
_LL1663: _temp1662=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1618)->f2; goto _LL1661; _LL1661: _temp1660=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1618)->f3; goto _LL1623;} else{ goto
_LL1624;} _LL1624: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*)
_temp1618)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL1671: _temp1670=( struct
_tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1618)->f1; goto _LL1669;
_LL1669: _temp1668=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1618)->f2; goto _LL1667; _LL1667: _temp1666=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp1618)->f3; goto _LL1625;} else{ goto
_LL1626;} _LL1626: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*)
_temp1618)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL1677: _temp1676=( struct
_tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp1618)->f1; goto _LL1675;
_LL1675: _temp1674=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1618)->f2; goto _LL1673; _LL1673: _temp1672=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1618)->f3; goto _LL1627;} else{ goto
_LL1628;} _LL1628: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*)
_temp1618)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1683: _temp1682=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1618)->f1; goto _LL1681; _LL1681:
_temp1680=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1618)->f2; goto _LL1679; _LL1679: _temp1678=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1618)->f3; goto _LL1629;} else{ goto
_LL1630;} _LL1630: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*)
_temp1618)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1685: _temp1684=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1618)->f1;
_LL1695: _temp1694=( void*) _temp1684.elt_typ; goto _LL1693; _LL1693: _temp1692=(
void*) _temp1684.rgn_typ; goto _LL1691; _LL1691: _temp1690=( struct Cyc_Absyn_Conref*)
_temp1684.nullable; goto _LL1689; _LL1689: _temp1688=( struct Cyc_Absyn_Tqual*)
_temp1684.tq; goto _LL1687; _LL1687: _temp1686=( struct Cyc_Absyn_Conref*)
_temp1684.bounds; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(( unsigned int)
_temp1618 > 5u?(( struct _enum_struct*) _temp1618)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1697: _temp1696=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1618)->f1; _LL1707: _temp1706=( struct Cyc_List_List*) _temp1696.tvars;
goto _LL1705; _LL1705: _temp1704=( struct Cyc_Core_Opt*) _temp1696.effect; goto
_LL1703; _LL1703: _temp1702=( void*) _temp1696.ret_typ; goto _LL1701; _LL1701:
_temp1700=( struct Cyc_List_List*) _temp1696.args; goto _LL1699; _LL1699:
_temp1698=( int) _temp1696.varargs; goto _LL1633;} else{ goto _LL1634;} _LL1634:
if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*) _temp1618)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1709: _temp1708=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1618)->f1; goto _LL1635;} else{ goto
_LL1636;} _LL1636: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*)
_temp1618)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1715: _temp1714=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp1618)->f1; goto _LL1713; _LL1713: _temp1712=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1618)->f2; goto
_LL1711; _LL1711: _temp1710=( int)(( struct Cyc_Absyn_Evar_struct*) _temp1618)->f3;
goto _LL1637;} else{ goto _LL1638;} _LL1638: if(( unsigned int) _temp1618 > 5u?((
struct _enum_struct*) _temp1618)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL1717: _temp1716=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1618)->f1;
goto _LL1639;} else{ goto _LL1640;} _LL1640: if( _temp1618 == Cyc_Absyn_VoidType){
goto _LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int) _temp1618 > 5u?((
struct _enum_struct*) _temp1618)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL1721:
_temp1720=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp1618)->f1;
goto _LL1719; _LL1719: _temp1718=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp1618)->f2; goto _LL1643;} else{ goto _LL1644;} _LL1644: if(( unsigned int)
_temp1618 > 5u?(( struct _enum_struct*) _temp1618)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1725: _temp1724=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1618)->f1;
goto _LL1723; _LL1723: _temp1722=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1618)->f2; goto _LL1645;} else{ goto _LL1646;} _LL1646: if( _temp1618 ==
Cyc_Absyn_FloatType){ goto _LL1647;} else{ goto _LL1648;} _LL1648: if( _temp1618
== Cyc_Absyn_DoubleType){ goto _LL1649;} else{ goto _LL1650;} _LL1650: if(
_temp1618 == Cyc_Absyn_HeapRgn){ goto _LL1651;} else{ goto _LL1652;} _LL1652:
if( _temp1618 == Cyc_Absyn_UnionType){ goto _LL1653;} else{ goto _LL1654;}
_LL1654: if(( unsigned int) _temp1618 > 5u?(( struct _enum_struct*) _temp1618)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL1727: _temp1726=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1618)->f1; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(( unsigned int)
_temp1618 > 5u?(( struct _enum_struct*) _temp1618)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL1729: _temp1728=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1618)->f1; goto _LL1657;} else{ goto _LL1619;} _LL1621: { struct
_handler_cons _temp1730; _push_handler(& _temp1730);{ struct _xenum_struct*
_temp1731=( struct _xenum_struct*) setjmp( _temp1730.handler); if( ! _temp1731){{
void* _temp1732=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp1658); _npop_handler( 0u); return _temp1732;}; _pop_handler();} else{
struct _xenum_struct* _temp1734= _temp1731; _LL1736: if( _temp1734->tag == Cyc_Core_Not_found_tag){
goto _LL1737;} else{ goto _LL1738;} _LL1738: goto _LL1739; _LL1737: return t;
_LL1739:( void) _throw( _temp1734); _LL1735:;}}} _LL1623: return({ struct Cyc_Absyn_StructType_struct*
_temp1740=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1740->tag= Cyc_Absyn_StructType_tag; _temp1740->f1= _temp1664; _temp1740->f2=
Cyc_Tcutil_substs( inst, _temp1662); _temp1740->f3= _temp1660;( void*) _temp1740;});
_LL1625: return({ struct Cyc_Absyn_EnumType_struct* _temp1741=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1741->tag= Cyc_Absyn_EnumType_tag;
_temp1741->f1= _temp1670; _temp1741->f2= Cyc_Tcutil_substs( inst, _temp1668);
_temp1741->f3= _temp1666;( void*) _temp1741;}); _LL1627: return({ struct Cyc_Absyn_TypedefType_struct*
_temp1742=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp1742->tag= Cyc_Absyn_TypedefType_tag; _temp1742->f1= _temp1676; _temp1742->f2=
Cyc_Tcutil_substs( inst, _temp1674); _temp1742->f3= _temp1672;( void*) _temp1742;});
_LL1629: return({ struct Cyc_Absyn_ArrayType_struct* _temp1743=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1743->tag= Cyc_Absyn_ArrayType_tag;
_temp1743->f1=( void*) Cyc_Tcutil_substitute( inst, _temp1682); _temp1743->f2=
_temp1680; _temp1743->f3= _temp1678;( void*) _temp1743;}); _LL1631: return({
struct Cyc_Absyn_PointerType_struct* _temp1744=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1744->tag= Cyc_Absyn_PointerType_tag;
_temp1744->f1=({ struct Cyc_Absyn_PtrInfo _temp1745; _temp1745.elt_typ=( void*)
Cyc_Tcutil_substitute( inst, _temp1694); _temp1745.rgn_typ=( void*) Cyc_Tcutil_substitute(
inst, _temp1692); _temp1745.nullable= _temp1690; _temp1745.tq= _temp1688;
_temp1745.bounds= _temp1686; _temp1745;});( void*) _temp1744;}); _LL1633:{
struct Cyc_List_List* p= _temp1706; for( 0; p != 0; p= p->tl){ inst=({ struct
Cyc_List_List* _temp1746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1746->hd=( void*)({ struct _tuple6* _temp1747=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp1747->f1=( struct Cyc_Absyn_Tvar*) p->hd;
_temp1747->f2=({ struct Cyc_Absyn_VarType_struct* _temp1748=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1748->tag= Cyc_Absyn_VarType_tag;
_temp1748->f1=( struct Cyc_Absyn_Tvar*) p->hd;( void*) _temp1748;}); _temp1747;});
_temp1746->tl= inst; _temp1746;});}}{ struct Cyc_List_List* _temp1751; struct
Cyc_List_List* _temp1753; struct _tuple0 _temp1749=(( struct _tuple0(*)( struct
Cyc_List_List* x)) Cyc_List_split)((( struct Cyc_List_List*(*)( struct _tuple13*(*
f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f1,
_temp1700)); _LL1754: _temp1753= _temp1749.f1; goto _LL1752; _LL1752: _temp1751=
_temp1749.f2; goto _LL1750; _LL1750: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs(
inst, _temp1751); struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1753, ts2)); struct Cyc_Core_Opt*
eff2= _temp1704 == 0? 0:( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1755=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1755->v=(
void*) Cyc_Tcutil_substitute( inst,( void*) _temp1704->v); _temp1755;}); return({
struct Cyc_Absyn_FnType_struct* _temp1756=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1756->tag= Cyc_Absyn_FnType_tag;
_temp1756->f1=({ struct Cyc_Absyn_FnInfo _temp1757; _temp1757.tvars= _temp1706;
_temp1757.effect= eff2; _temp1757.ret_typ=( void*) Cyc_Tcutil_substitute( inst,
_temp1702); _temp1757.args= args2; _temp1757.varargs= _temp1698; _temp1757;});(
void*) _temp1756;});}} _LL1635: { struct Cyc_List_List* _temp1760; struct Cyc_List_List*
_temp1762; struct _tuple0 _temp1758=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( _temp1708); _LL1763: _temp1762= _temp1758.f1; goto _LL1761;
_LL1761: _temp1760= _temp1758.f2; goto _LL1759; _LL1759: { struct Cyc_List_List*
ts2= Cyc_Tcutil_substs( inst, _temp1760); struct Cyc_List_List* tqts2=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1762, ts2); return({ struct Cyc_Absyn_TupleType_struct* _temp1764=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1764->tag= Cyc_Absyn_TupleType_tag; _temp1764->f1= tqts2;( void*) _temp1764;});}}
_LL1637: if( _temp1712 != 0){ return Cyc_Tcutil_substitute( inst,( void*)
_temp1712->v);} else{ return t;} _LL1639: return({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1765=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1765->tag= Cyc_Absyn_RgnHandleType_tag;
_temp1765->f1=( void*) Cyc_Tcutil_substitute( inst, _temp1716);( void*)
_temp1765;}); _LL1641: return t; _LL1643: return t; _LL1645: return t; _LL1647:
return t; _LL1649: return t; _LL1651: return t; _LL1653: return t; _LL1655:
return({ struct Cyc_Absyn_AccessEff_struct* _temp1766=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp1766->tag= Cyc_Absyn_AccessEff_tag;
_temp1766->f1=( void*) Cyc_Tcutil_substitute( inst, _temp1726);( void*)
_temp1766;}); _LL1657: return({ struct Cyc_Absyn_JoinEff_struct* _temp1767=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1767->tag= Cyc_Absyn_JoinEff_tag; _temp1767->f1= Cyc_Tcutil_substs( inst,
_temp1728);( void*) _temp1767;}); _LL1619:;} static struct Cyc_List_List* Cyc_Tcutil_substs(
struct Cyc_List_List* inst, struct Cyc_List_List* ts){ return(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_List_List*, void*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_substitute, inst, ts);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp1768=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1768->tag= Cyc_Absyn_Const_e_tag; _temp1768->f1=( void*) Cyc_Absyn_Null_c;(
void*) _temp1768;}), loc);{ void* _temp1769= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp1783; void* _temp1785; void* _temp1787; void* _temp1789; void* _temp1791;
_LL1771: if(( unsigned int) _temp1769 > 5u?(( struct _enum_struct*) _temp1769)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1784: _temp1783=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1769)->f1; goto _LL1772;} else{ goto
_LL1773;} _LL1773: if(( unsigned int) _temp1769 > 5u?(( struct _enum_struct*)
_temp1769)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1788: _temp1787=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1769)->f1; goto _LL1786; _LL1786:
_temp1785=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1769)->f2; if(
_temp1785 == Cyc_Absyn_B1){ goto _LL1774;} else{ goto _LL1775;}} else{ goto
_LL1775;} _LL1775: if(( unsigned int) _temp1769 > 5u?(( struct _enum_struct*)
_temp1769)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1792: _temp1791=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1769)->f1; goto _LL1790; _LL1790:
_temp1789=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1769)->f2; goto
_LL1776;} else{ goto _LL1777;} _LL1777: if( _temp1769 == Cyc_Absyn_FloatType){
goto _LL1778;} else{ goto _LL1779;} _LL1779: if( _temp1769 == Cyc_Absyn_DoubleType){
goto _LL1780;} else{ goto _LL1781;} _LL1781: goto _LL1782; _LL1772: goto _LL1770;
_LL1774:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct* _temp1793=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1793->tag= Cyc_Absyn_Const_e_tag; _temp1793->f1=( void*)({ struct Cyc_Absyn_Char_c_struct*
_temp1794=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp1794->tag= Cyc_Absyn_Char_c_tag; _temp1794->f1=( void*) Cyc_Absyn_Signed;
_temp1794->f2='\000';( void*) _temp1794;});( void*) _temp1793;})); goto _LL1770;
_LL1776:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct* _temp1795=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1795->tag= Cyc_Absyn_Const_e_tag; _temp1795->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp1796=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1796->tag= Cyc_Absyn_Int_c_tag; _temp1796->f1=( void*) _temp1791; _temp1796->f2=
0;( void*) _temp1796;});( void*) _temp1795;})); if( _temp1789 != Cyc_Absyn_B4){
e= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Cast_e_struct* _temp1797=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1797->tag= Cyc_Absyn_Cast_e_tag;
_temp1797->f1=( void*) t; _temp1797->f2= e;( void*) _temp1797;}), loc);} goto
_LL1770; _LL1778:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp1798=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1798->tag= Cyc_Absyn_Const_e_tag; _temp1798->f1=( void*)({ struct Cyc_Absyn_Float_c_struct*
_temp1799=( struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp1799->tag= Cyc_Absyn_Float_c_tag; _temp1799->f1=( struct _tagged_string)({
char* _temp1800=( char*)"0.0"; struct _tagged_string _temp1801; _temp1801.curr=
_temp1800; _temp1801.base= _temp1800; _temp1801.last_plus_one= _temp1800 + 4;
_temp1801;});( void*) _temp1799;});( void*) _temp1798;})); goto _LL1770; _LL1780:(
void*)( e->r=( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1802=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1802->tag= Cyc_Absyn_Cast_e_tag;
_temp1802->f1=( void*) t; _temp1802->f2= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp1803=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1803->tag= Cyc_Absyn_Const_e_tag; _temp1803->f1=( void*)({ struct Cyc_Absyn_Float_c_struct*
_temp1804=( struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp1804->tag= Cyc_Absyn_Float_c_tag; _temp1804->f1=( struct _tagged_string)({
char* _temp1805=( char*)"0.0"; struct _tagged_string _temp1806; _temp1806.curr=
_temp1805; _temp1806.base= _temp1805; _temp1806.last_plus_one= _temp1805 + 4;
_temp1806;});( void*) _temp1804;});( void*) _temp1803;}), loc);( void*)
_temp1802;})); goto _LL1770; _LL1782: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1807= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp1807.last_plus_one - _temp1807.curr, _temp1807.curr);})); goto _LL1770;
_LL1770:;} return e;} struct _tuple6* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple6* _temp1808=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp1808->f1= tv; _temp1808->f2= Cyc_Absyn_new_evar( k); _temp1808;});}
struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp1809= Cyc_Tcutil_compress( t); int
_temp1849; struct Cyc_Core_Opt* _temp1851; void* _temp1853; struct Cyc_Absyn_Tvar*
_temp1855; struct Cyc_Absyn_Enumdecl** _temp1857; struct Cyc_Absyn_Enumdecl***
_temp1859; struct Cyc_List_List* _temp1860; struct _tuple1* _temp1862; struct
Cyc_Absyn_Xenumdecl** _temp1864; struct Cyc_Absyn_Xenumdecl*** _temp1866; struct
_tuple1* _temp1867; struct Cyc_Absyn_PtrInfo _temp1869; struct Cyc_Absyn_Conref*
_temp1871; struct Cyc_Absyn_Tqual* _temp1873; struct Cyc_Absyn_Conref* _temp1875;
void* _temp1877; void* _temp1879; void* _temp1881; void* _temp1883; struct Cyc_Absyn_Exp*
_temp1885; struct Cyc_Absyn_Tqual* _temp1887; void* _temp1889; struct Cyc_Absyn_FnInfo
_temp1891; int _temp1893; struct Cyc_List_List* _temp1895; void* _temp1897;
struct Cyc_Core_Opt* _temp1899; struct Cyc_Core_Opt** _temp1901; struct Cyc_List_List*
_temp1902; struct Cyc_List_List** _temp1904; struct Cyc_List_List* _temp1905;
struct Cyc_Absyn_Structdecl** _temp1907; struct Cyc_Absyn_Structdecl***
_temp1909; struct Cyc_List_List* _temp1910; struct _tuple1* _temp1912; struct
Cyc_Core_Opt* _temp1914; struct Cyc_Core_Opt** _temp1916; struct Cyc_List_List*
_temp1917; struct _tuple1* _temp1919; void* _temp1921; void* _temp1923; struct
Cyc_List_List* _temp1925; _LL1811: if( _temp1809 == Cyc_Absyn_VoidType){ goto
_LL1812;} else{ goto _LL1813;} _LL1813: if(( unsigned int) _temp1809 > 5u?((
struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1854:
_temp1853=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1809)->f1; goto _LL1852;
_LL1852: _temp1851=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1809)->f2; goto _LL1850; _LL1850: _temp1849=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1809)->f3; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(( unsigned int)
_temp1809 > 5u?(( struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_VarType_tag:
0){ _LL1856: _temp1855=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1809)->f1; goto _LL1816;} else{ goto _LL1817;} _LL1817: if(( unsigned int)
_temp1809 > 5u?(( struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1863: _temp1862=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1809)->f1; goto _LL1861; _LL1861: _temp1860=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1809)->f2; goto _LL1858; _LL1858:
_temp1857=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1809)->f3; _temp1859=&(( struct Cyc_Absyn_EnumType_struct*) _temp1809)->f3;
goto _LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int) _temp1809 > 5u?((
struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL1868:
_temp1867=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp1809)->f1;
goto _LL1865; _LL1865: _temp1864=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp1809)->f2; _temp1866=&(( struct Cyc_Absyn_XenumType_struct*) _temp1809)->f2;
goto _LL1820;} else{ goto _LL1821;} _LL1821: if(( unsigned int) _temp1809 > 5u?((
struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1870:
_temp1869=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1809)->f1; _LL1880: _temp1879=( void*) _temp1869.elt_typ; goto _LL1878;
_LL1878: _temp1877=( void*) _temp1869.rgn_typ; goto _LL1876; _LL1876: _temp1875=(
struct Cyc_Absyn_Conref*) _temp1869.nullable; goto _LL1874; _LL1874: _temp1873=(
struct Cyc_Absyn_Tqual*) _temp1869.tq; goto _LL1872; _LL1872: _temp1871=( struct
Cyc_Absyn_Conref*) _temp1869.bounds; goto _LL1822;} else{ goto _LL1823;} _LL1823:
if(( unsigned int) _temp1809 > 5u?(( struct _enum_struct*) _temp1809)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL1884: _temp1883=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1809)->f1; goto _LL1882; _LL1882: _temp1881=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1809)->f2; goto _LL1824;} else{ goto _LL1825;} _LL1825: if( _temp1809 ==
Cyc_Absyn_FloatType){ goto _LL1826;} else{ goto _LL1827;} _LL1827: if( _temp1809
== Cyc_Absyn_DoubleType){ goto _LL1828;} else{ goto _LL1829;} _LL1829: if((
unsigned int) _temp1809 > 5u?(( struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1890: _temp1889=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1809)->f1;
goto _LL1888; _LL1888: _temp1887=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1809)->f2; goto _LL1886; _LL1886: _temp1885=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1809)->f3; goto _LL1830;} else{ goto
_LL1831;} _LL1831: if(( unsigned int) _temp1809 > 5u?(( struct _enum_struct*)
_temp1809)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1892: _temp1891=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1809)->f1; _LL1903: _temp1902=( struct Cyc_List_List*)
_temp1891.tvars; _temp1904=&((( struct Cyc_Absyn_FnType_struct*) _temp1809)->f1).tvars;
goto _LL1900; _LL1900: _temp1899=( struct Cyc_Core_Opt*) _temp1891.effect;
_temp1901=&((( struct Cyc_Absyn_FnType_struct*) _temp1809)->f1).effect; goto
_LL1898; _LL1898: _temp1897=( void*) _temp1891.ret_typ; goto _LL1896; _LL1896:
_temp1895=( struct Cyc_List_List*) _temp1891.args; goto _LL1894; _LL1894:
_temp1893=( int) _temp1891.varargs; goto _LL1832;} else{ goto _LL1833;} _LL1833:
if(( unsigned int) _temp1809 > 5u?(( struct _enum_struct*) _temp1809)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1906: _temp1905=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1809)->f1; goto _LL1834;} else{ goto
_LL1835;} _LL1835: if(( unsigned int) _temp1809 > 5u?(( struct _enum_struct*)
_temp1809)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1913: _temp1912=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1809)->f1; goto _LL1911;
_LL1911: _temp1910=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1809)->f2; goto _LL1908; _LL1908: _temp1907=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1809)->f3; _temp1909=&(( struct Cyc_Absyn_StructType_struct*)
_temp1809)->f3; goto _LL1836;} else{ goto _LL1837;} _LL1837: if(( unsigned int)
_temp1809 > 5u?(( struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL1920: _temp1919=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1809)->f1; goto _LL1918; _LL1918: _temp1917=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1809)->f2; goto _LL1915; _LL1915:
_temp1914=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1809)->f3; _temp1916=&(( struct Cyc_Absyn_TypedefType_struct*) _temp1809)->f3;
goto _LL1838;} else{ goto _LL1839;} _LL1839: if( _temp1809 == Cyc_Absyn_UnionType){
goto _LL1840;} else{ goto _LL1841;} _LL1841: if( _temp1809 == Cyc_Absyn_HeapRgn){
goto _LL1842;} else{ goto _LL1843;} _LL1843: if(( unsigned int) _temp1809 > 5u?((
struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL1922: _temp1921=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1809)->f1;
goto _LL1844;} else{ goto _LL1845;} _LL1845: if(( unsigned int) _temp1809 > 5u?((
struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL1924:
_temp1923=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1809)->f1; goto
_LL1846;} else{ goto _LL1847;} _LL1847: if(( unsigned int) _temp1809 > 5u?((
struct _enum_struct*) _temp1809)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL1926:
_temp1925=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp1809)->f1;
goto _LL1848;} else{ goto _LL1810;} _LL1812: goto _LL1810; _LL1814: goto _LL1810;
_LL1816: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1855->kind); if(( void*) c->v == Cyc_Absyn_No_constr){(
void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1927=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1927->tag= Cyc_Absyn_Eq_constr_tag; _temp1927->f1=( void*) expected_kind;(
void*) _temp1927;}));} free_vars= Cyc_Tcutil_add_free_tvar( loc, free_vars,
_temp1855); goto _LL1810;} _LL1818: if( _temp1862 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1928=( char*)"missing enum name"; struct
_tagged_string _temp1929; _temp1929.curr= _temp1928; _temp1929.base= _temp1928;
_temp1929.last_plus_one= _temp1928 + 18; _temp1929;})); return free_vars;} if(*
_temp1859 == 0){ struct _handler_cons _temp1930; _push_handler(& _temp1930);{
struct _xenum_struct* _temp1931=( struct _xenum_struct*) setjmp( _temp1930.handler);
if( ! _temp1931){* _temp1859=( struct Cyc_Absyn_Enumdecl**) Cyc_Tcenv_lookup_enumdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp1932= _temp1862; if( _temp1932
== 0){ _throw( Null_Exception);} _temp1932;}));; _pop_handler();} else{ struct
_xenum_struct* _temp1934= _temp1931; _LL1936: if( _temp1934->tag == Cyc_Dict_Absent_tag){
goto _LL1937;} else{ goto _LL1938;} _LL1938: goto _LL1939; _LL1937: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1941= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp1940= _temp1862; if( _temp1940 == 0){ _throw(
Null_Exception);} _temp1940;})); xprintf("unbound type enum %.*s", _temp1941.last_plus_one
- _temp1941.curr, _temp1941.curr);})); return free_vars; _LL1939:( void) _throw(
_temp1934); _LL1935:;}}}{ struct Cyc_Absyn_Enumdecl* ed=*(* _temp1859); if( ed->name
!= 0){* _temp1862=({ struct _tuple1* _temp1942=( struct _tuple1*)( ed->name)->v;
unsigned int _temp1943= 0; if( _temp1943 >= 1u){ _throw( Null_Exception);}
_temp1942[ _temp1943];});}{ struct Cyc_List_List* tvs= ed->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1860); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1945= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp1944= _temp1862; if( _temp1944 == 0){ _throw(
Null_Exception);} _temp1944;})); int _temp1946= lvs; int _temp1947= largs;
xprintf("enum %.*s expects %d type arguments, not %d", _temp1945.last_plus_one -
_temp1945.curr, _temp1945.curr, _temp1946, _temp1947);}));} for( 0; _temp1860 !=
0; _temp1860= _temp1860->tl, tvs= tvs->tl){ void* t1=( void*) _temp1860->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1810;}} _LL1820: if(* _temp1866 == 0){
struct _handler_cons _temp1948; _push_handler(& _temp1948);{ struct
_xenum_struct* _temp1949=( struct _xenum_struct*) setjmp( _temp1948.handler);
if( ! _temp1949){{ struct Cyc_Core_Opt* xed= Cyc_Tcenv_lookup_xenumdecl( te, loc,
_temp1867); if( xed == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp1950= Cyc_Absynpp_qvar2string( _temp1867); xprintf("unbound xenum %.*s",
_temp1950.last_plus_one - _temp1950.curr, _temp1950.curr);}));{ struct Cyc_List_List*
_temp1951= free_vars; _npop_handler( 0u); return _temp1951;}}* _temp1866=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_Xenumdecl**) xed->v);};
_pop_handler();} else{ struct _xenum_struct* _temp1953= _temp1949; _LL1955: if(
_temp1953->tag == Cyc_Dict_Absent_tag){ goto _LL1956;} else{ goto _LL1957;}
_LL1957: goto _LL1958; _LL1956: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp1959= Cyc_Absynpp_qvar2string( _temp1867); xprintf("unbound xenum %.*s",
_temp1959.last_plus_one - _temp1959.curr, _temp1959.curr);})); return free_vars;
_LL1958:( void) _throw( _temp1953); _LL1954:;}}}{ struct Cyc_Absyn_Xenumdecl*
xed=*(* _temp1866);({ struct _tuple1* _temp1960= _temp1867; unsigned int
_temp1961= 0; if( _temp1961 >= 1u){ _throw( Null_Exception);} _temp1960[
_temp1961]=({ struct _tuple1* _temp1962= xed->name; unsigned int _temp1963= 0;
if( _temp1963 >= 1u){ _throw( Null_Exception);} _temp1962[ _temp1963];});});
goto _LL1810;} _LL1822: free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars, Cyc_Absyn_AnyKind, _temp1879); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, Cyc_Absyn_RgnKind, _temp1877);{ void* _temp1964=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1871))->v; void* _temp1970; struct Cyc_Absyn_Exp* _temp1972; _LL1966: if((
unsigned int) _temp1964 > 1u?(( struct _enum_struct*) _temp1964)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1971: _temp1970=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1964)->f1;
if(( unsigned int) _temp1970 > 1u?(( struct _enum_struct*) _temp1970)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1973: _temp1972=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1970)->f1; goto _LL1967;} else{ goto _LL1968;}}
else{ goto _LL1968;} _LL1968: goto _LL1969; _LL1967: Cyc_Evexp_eval_const_uint_exp(
_temp1972); goto _LL1965; _LL1969: goto _LL1965; _LL1965:;} goto _LL1810;
_LL1824: goto _LL1810; _LL1826: goto _LL1810; _LL1828: goto _LL1810; _LL1830:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_MemKind,
_temp1889); Cyc_Evexp_eval_const_uint_exp( _temp1885); goto _LL1810; _LL1832:
Cyc_Tcutil_check_unique_tvars( loc,* _temp1904);{ struct Cyc_List_List* b=*
_temp1904; for( 0; b != 0; b= b->tl){ void* _temp1974=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)
b->hd)->kind))->v; void* _temp1980; _LL1976: if(( unsigned int) _temp1974 > 1u?((
struct _enum_struct*) _temp1974)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1981:
_temp1980=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1974)->f1; if(
_temp1980 == Cyc_Absyn_MemKind){ goto _LL1977;} else{ goto _LL1978;}} else{ goto
_LL1978;} _LL1978: goto _LL1979; _LL1977: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1982=*(( struct Cyc_Absyn_Tvar*) b->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp1982.last_plus_one - _temp1982.curr, _temp1982.curr);})); goto _LL1975;
_LL1979: goto _LL1975; _LL1975:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars, Cyc_Absyn_MemKind,
_temp1897); for( 0; _temp1895 != 0; _temp1895= _temp1895->tl){ new_free_vars=
Cyc_Tcutil_check_valid_type( loc, te, new_free_vars, Cyc_Absyn_MemKind,(*((
struct _tuple2*) _temp1895->hd)).f3);} if(* _temp1901 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars, Cyc_Absyn_EffKind,( void*)(* _temp1901)->v);} else{
struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar( Cyc_Absyn_EffKind); new_free_vars=({
struct Cyc_List_List* _temp1983=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1983->hd=( void*) e; _temp1983->tl= new_free_vars;
_temp1983;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs=
new_free_vars; for( 0; tvs != 0; tvs= tvs->tl){ void* _temp1984=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp1992; void* _temp1994;
_LL1986: if(( unsigned int) _temp1984 > 1u?(( struct _enum_struct*) _temp1984)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL1993: _temp1992=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1984)->f1; if( _temp1992 == Cyc_Absyn_RgnKind){ goto _LL1987;} else{ goto
_LL1988;}} else{ goto _LL1988;} _LL1988: if(( unsigned int) _temp1984 > 1u?((
struct _enum_struct*) _temp1984)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1995:
_temp1994=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1984)->f1; if(
_temp1994 == Cyc_Absyn_EffKind){ goto _LL1989;} else{ goto _LL1990;}} else{ goto
_LL1990;} _LL1990: goto _LL1991; _LL1987: effect=({ struct Cyc_List_List*
_temp1996=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1996->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp1997=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp1997->tag= Cyc_Absyn_AccessEff_tag; _temp1997->f1=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1998=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1998->tag= Cyc_Absyn_VarType_tag; _temp1998->f1=( struct Cyc_Absyn_Tvar*)
tvs->hd;( void*) _temp1998;});( void*) _temp1997;}); _temp1996->tl= effect;
_temp1996;}); goto _LL1985; _LL1989: effect=({ struct Cyc_List_List* _temp1999=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1999->hd=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp2000=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2000->tag= Cyc_Absyn_VarType_tag;
_temp2000->f1=( struct Cyc_Absyn_Tvar*) tvs->hd;( void*) _temp2000;}); _temp1999->tl=
effect; _temp1999;}); goto _LL1985; _LL1991: goto _LL1985; _LL1985:;}}*
_temp1901=({ struct Cyc_Core_Opt* _temp2001=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2001->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2002=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2002->tag= Cyc_Absyn_JoinEff_tag; _temp2002->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);( void*) _temp2002;});
_temp2001;});}} if(* _temp1904 != 0){ struct Cyc_List_List* new_btvs= 0;{ struct
Cyc_List_List* bs=* _temp1904; for( 0; bs != 0; bs= bs->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*) bs->hd);{ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp2003=( void*) c->v; void*
_temp2011; _LL2005: if( _temp2003 == Cyc_Absyn_No_constr){ goto _LL2006;} else{
goto _LL2007;} _LL2007: if(( unsigned int) _temp2003 > 1u?(( struct _enum_struct*)
_temp2003)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2012: _temp2011=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2003)->f1; if( _temp2011 == Cyc_Absyn_MemKind){
goto _LL2008;} else{ goto _LL2009;}} else{ goto _LL2009;} _LL2009: goto _LL2010;
_LL2006: Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp2013=*(( struct Cyc_Absyn_Tvar*)
bs->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2013.last_plus_one - _temp2013.curr, _temp2013.curr);})); goto _LL2008;
_LL2008:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2014=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2014->tag= Cyc_Absyn_Eq_constr_tag; _temp2014->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp2014;})); goto _LL2004; _LL2010: goto _LL2004; _LL2004:;}}}
new_free_vars= Cyc_Tcutil_remove_bound_tvars( new_free_vars,* _temp1904);} for(
0; new_free_vars != 0; new_free_vars= new_free_vars->tl){ free_vars= Cyc_Tcutil_add_free_tvar(
loc, free_vars,( struct Cyc_Absyn_Tvar*) new_free_vars->hd);} goto _LL1810;}
_LL1834: for( 0; _temp1905 != 0; _temp1905= _temp1905->tl){ free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, Cyc_Absyn_MemKind,(*(( struct _tuple5*) _temp1905->hd)).f2);}
goto _LL1810; _LL1836: if( _temp1912 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2015=( char*)"missing struct name"; struct
_tagged_string _temp2016; _temp2016.curr= _temp2015; _temp2016.base= _temp2015;
_temp2016.last_plus_one= _temp2015 + 20; _temp2016;})); return free_vars;} if(*
_temp1909 == 0){ struct _handler_cons _temp2017; _push_handler(& _temp2017);{
struct _xenum_struct* _temp2018=( struct _xenum_struct*) setjmp( _temp2017.handler);
if( ! _temp2018){* _temp1909=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2019= _temp1912; if( _temp2019
== 0){ _throw( Null_Exception);} _temp2019;}));; _pop_handler();} else{ struct
_xenum_struct* _temp2021= _temp2018; _LL2023: if( _temp2021->tag == Cyc_Dict_Absent_tag){
goto _LL2024;} else{ goto _LL2025;} _LL2025: goto _LL2026; _LL2024: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2028= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2027= _temp1912; if( _temp2027 == 0){ _throw(
Null_Exception);} _temp2027;})); xprintf("unbound type struct %.*s", _temp2028.last_plus_one
- _temp2028.curr, _temp2028.curr);})); return free_vars; _LL2026:( void) _throw(
_temp2021); _LL2022:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(* _temp1909); if( sd->name
!= 0){* _temp1912=({ struct _tuple1* _temp2029=( struct _tuple1*)( sd->name)->v;
unsigned int _temp2030= 0; if( _temp2030 >= 1u){ _throw( Null_Exception);}
_temp2029[ _temp2030];});}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1910); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2032= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2031= _temp1912; if( _temp2031 == 0){ _throw(
Null_Exception);} _temp2031;})); int _temp2033= lvs; int _temp2034= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _temp2032.last_plus_one
- _temp2032.curr, _temp2032.curr, _temp2033, _temp2034);}));} for( 0; _temp1910
!= 0; _temp1910= _temp1910->tl, tvs= tvs->tl){ void* t1=( void*) _temp1910->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1810;}} _LL1838: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2035; _push_handler(& _temp2035);{ struct
_xenum_struct* _temp2036=( struct _xenum_struct*) setjmp( _temp2035.handler);
if( ! _temp2036){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc, _temp1919);;
_pop_handler();} else{ struct _xenum_struct* _temp2038= _temp2036; _LL2040: if(
_temp2038->tag == Cyc_Dict_Absent_tag){ goto _LL2041;} else{ goto _LL2042;}
_LL2042: goto _LL2043; _LL2041: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2044= Cyc_Absynpp_qvar2string( _temp1919); xprintf("unbound typedef name %.*s",
_temp2044.last_plus_one - _temp2044.curr, _temp2044.curr);})); return free_vars;
_LL2043:( void) _throw( _temp2038); _LL2039:;}}}({ struct _tuple1* _temp2045=
_temp1919; unsigned int _temp2046= 0; if( _temp2046 >= 1u){ _throw(
Null_Exception);} _temp2045[ _temp2046]=({ struct _tuple1* _temp2047= td->name;
unsigned int _temp2048= 0; if( _temp2048 >= 1u){ _throw( Null_Exception);}
_temp2047[ _temp2048];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp1917; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=
ts->tl, tvs= tvs->tl){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp2049=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2049->hd=(
void*)({ struct _tuple6* _temp2050=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp2050->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp2050->f2=( void*)
ts->hd; _temp2050;}); _temp2049->tl= inst; _temp2049;});} if( tvs != 0? 1: ts !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2051= Cyc_Absynpp_qvar2string(
_temp1919); xprintf("typedef %.*s expects a different number of arguments",
_temp2051.last_plus_one - _temp2051.curr, _temp2051.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp1916=({
struct Cyc_Core_Opt* _temp2052=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2052->v=( void*) new_typ; _temp2052;}); goto _LL1810;}}}
_LL1840: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2053=( char*)"union type unsupported";
struct _tagged_string _temp2054; _temp2054.curr= _temp2053; _temp2054.base=
_temp2053; _temp2054.last_plus_one= _temp2053 + 23; _temp2054;})); goto _LL1810;
_LL1842: goto _LL1810; _LL1844: _temp1923= _temp1921; goto _LL1846; _LL1846:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_RgnKind,
_temp1923); goto _LL1810; _LL1848: for( 0; _temp1925 != 0; _temp1925= _temp1925->tl){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_EffKind,(
void*) _temp1925->hd);} goto _LL1810; _LL1810:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind(
t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2055=
Cyc_Absynpp_typ2string( t); struct _tagged_string _temp2056= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_string _temp2057= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2055.last_plus_one - _temp2055.curr, _temp2055.curr, _temp2056.last_plus_one
- _temp2056.curr, _temp2056.curr, _temp2057.last_plus_one - _temp2057.curr,
_temp2057.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x= x->tl){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) x->hd)->kind); void* _temp2058=( void*) c->v; void*
_temp2066; _LL2060: if( _temp2058 == Cyc_Absyn_No_constr){ goto _LL2061;} else{
goto _LL2062;} _LL2062: if(( unsigned int) _temp2058 > 1u?(( struct _enum_struct*)
_temp2058)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2067: _temp2066=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2058)->f1; if( _temp2066 == Cyc_Absyn_MemKind){
goto _LL2063;} else{ goto _LL2064;}} else{ goto _LL2064;} _LL2064: goto _LL2065;
_LL2061: goto _LL2063; _LL2063:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2068=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2068->tag= Cyc_Absyn_Eq_constr_tag; _temp2068->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp2068;})); goto _LL2059; _LL2065: goto _LL2059; _LL2059:;}} if(
free_tvars != 0){ void* _temp2069= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2075; int _temp2077; struct Cyc_List_List* _temp2079; void* _temp2081;
struct Cyc_Core_Opt* _temp2083; struct Cyc_List_List* _temp2085; struct Cyc_List_List**
_temp2087; _LL2071: if(( unsigned int) _temp2069 > 5u?(( struct _enum_struct*)
_temp2069)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2076: _temp2075=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2069)->f1; _LL2086: _temp2085=( struct Cyc_List_List*)
_temp2075.tvars; _temp2087=&((( struct Cyc_Absyn_FnType_struct*) _temp2069)->f1).tvars;
goto _LL2084; _LL2084: _temp2083=( struct Cyc_Core_Opt*) _temp2075.effect; goto
_LL2082; _LL2082: _temp2081=( void*) _temp2075.ret_typ; goto _LL2080; _LL2080:
_temp2079=( struct Cyc_List_List*) _temp2075.args; goto _LL2078; _LL2078:
_temp2077=( int) _temp2075.varargs; goto _LL2072;} else{ goto _LL2073;} _LL2073:
goto _LL2074; _LL2072: if(* _temp2087 == 0){* _temp2087= free_tvars;} else{ if(
free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*) free_tvars->hd)->name;
if(( int)({ struct _tagged_string _temp2088= s; char* _temp2090= _temp2088.curr
+ 0; if( _temp2090 < _temp2088.base? 1: _temp2090 >= _temp2088.last_plus_one){
_throw( Null_Exception);}* _temp2090;}) ==( int)'?'){ Cyc_Tcutil_terr( loc,
xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2091= s; xprintf("unbound type variable %.*s",
_temp2091.last_plus_one - _temp2091.curr, _temp2091.curr);}));}}} goto _LL2070;
_LL2074: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2092=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; xprintf("unbound type variable %.*s", _temp2092.last_plus_one
- _temp2092.curr, _temp2092.curr);})); return; _LL2070:;}} void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List* free_tvars=
Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct Cyc_List_List*
fs= free_tvars; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Tvar* ftv=(
struct Cyc_Absyn_Tvar*) fs->hd; struct _tagged_string* ftvn= ftv->name; struct
Cyc_Absyn_Conref* ftvk= ftv->kind; int present= 0;{ struct Cyc_List_List* bs=
bound_tvars; for( 0; bs != 0; bs= bs->tl){ struct Cyc_Absyn_Tvar* btv=( struct
Cyc_Absyn_Tvar*) bs->hd; struct _tagged_string* btvn= btv->name; struct Cyc_Absyn_Conref*
btvk= btv->kind; if( Cyc_String_zstrptrcmp( ftvn, btvn) == 0){ present= 1; if( !
Cyc_Tcutil_constrain_kinds( btvk, ftvk)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2093=* ftvn; struct _tagged_string _temp2094= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2095= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2093.last_plus_one - _temp2093.curr, _temp2093.curr, _temp2094.last_plus_one
- _temp2094.curr, _temp2094.curr, _temp2095.last_plus_one - _temp2095.curr,
_temp2095.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2096=* ftvn; struct _tagged_string _temp2097= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2096.last_plus_one -
_temp2096.curr, _temp2096.curr, _temp2097.last_plus_one - _temp2097.curr,
_temp2097.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs= vs->tl){ struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 != 0; vs2=
vs2->tl){ if( cmp(( void*) vs->hd,( void*) vs2->hd) == 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2098= msg; struct _tagged_string _temp2099= a2string((
void*) vs->hd); xprintf("%.*s: %.*s", _temp2098.last_plus_one - _temp2098.curr,
_temp2098.curr, _temp2099.last_plus_one - _temp2099.curr, _temp2099.curr);}));}}}}
static struct _tagged_string Cyc_Tcutil_strptr2string( struct _tagged_string* s){
return* s;} void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct
Cyc_Position_Segment* loc, struct _tagged_string msg){(( void(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct
_tagged_string*), struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2100=(
char*)"duplicate type variable"; struct _tagged_string _temp2101; _temp2101.curr=
_temp2100; _temp2101.base= _temp2100; _temp2101.last_plus_one= _temp2100 + 24;
_temp2101;}));} struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd){ if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2102=( char*)"inappropriate use of abstract struct"; struct _tagged_string
_temp2103; _temp2103.curr= _temp2102; _temp2103.base= _temp2102; _temp2103.last_plus_one=
_temp2102 + 37; _temp2103;}));}{ struct Cyc_List_List* fields= 0;{ struct Cyc_List_List*
sd_fields=( struct Cyc_List_List*)( sd->fields)->v; for( 0; sd_fields != 0;
sd_fields= sd_fields->tl){ fields=({ struct Cyc_List_List* _temp2104=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2104->hd=( void*)({
struct _tuple14* _temp2105=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp2105->f1=( struct _tuple4*) sd_fields->hd; _temp2105->f2= 0;
_temp2105;}); _temp2104->tl= fields; _temp2104;});}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des= des->tl){ struct _tuple15 _temp2108; void* _temp2109;
struct Cyc_List_List* _temp2111; struct _tuple15* _temp2106=( struct _tuple15*)
des->hd; _temp2108=* _temp2106; _LL2112: _temp2111= _temp2108.f1; goto _LL2110;
_LL2110: _temp2109= _temp2108.f2; goto _LL2107; _LL2107: if( _temp2111 == 0){
struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2= fields2->tl){
if( !(*(( struct _tuple14*) fields2->hd)).f2){(*(( struct _tuple14*) fields2->hd)).f2=
1;(*(( struct _tuple15*) des->hd)).f1=({ struct Cyc_List_List* _temp2113=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2113->hd=(
void*)({ struct Cyc_Absyn_FieldName_struct* _temp2114=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp2114->tag= Cyc_Absyn_FieldName_tag;
_temp2114->f1=(*(*(( struct _tuple14*) fields2->hd)).f1).f1;( void*) _temp2114;});
_temp2113->tl= 0; _temp2113;}); ans=({ struct Cyc_List_List* _temp2115=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*)({
struct _tuple16* _temp2116=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2116->f1=(*(( struct _tuple14*) fields2->hd)).f1; _temp2116->f2=
_temp2109; _temp2116;}); _temp2115->tl= ans; _temp2115;}); break;}} if( fields2
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2117=( char*)"too many arguments to struct";
struct _tagged_string _temp2118; _temp2118.curr= _temp2117; _temp2118.base=
_temp2117; _temp2118.last_plus_one= _temp2117 + 29; _temp2118;}));}} else{ if(
_temp2111->tl != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2119=( char*)"multiple designators are not supported"; struct
_tagged_string _temp2120; _temp2120.curr= _temp2119; _temp2120.base= _temp2119;
_temp2120.last_plus_one= _temp2119 + 39; _temp2120;}));} else{ void* _temp2121=(
void*) _temp2111->hd; struct Cyc_Absyn_Exp* _temp2127; struct _tagged_string*
_temp2129; _LL2123: if((( struct _enum_struct*) _temp2121)->tag == Cyc_Absyn_ArrayElement_tag){
_LL2128: _temp2127=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp2121)->f1; goto _LL2124;} else{ goto _LL2125;} _LL2125: if((( struct
_enum_struct*) _temp2121)->tag == Cyc_Absyn_FieldName_tag){ _LL2130: _temp2129=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2121)->f1;
goto _LL2126;} else{ goto _LL2122;} _LL2124: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2131=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2132; _temp2132.curr= _temp2131; _temp2132.base=
_temp2131; _temp2132.last_plus_one= _temp2131 + 44; _temp2132;})); goto _LL2122;
_LL2126: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=
fields2->tl){ if( Cyc_String_zstrptrcmp( _temp2129,(*(*(( struct _tuple14*)
fields2->hd)).f1).f1) == 0){ if((*(( struct _tuple14*) fields2->hd)).f2){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2133=* _temp2129; xprintf("field %.*s has already been used as an argument",
_temp2133.last_plus_one - _temp2133.curr, _temp2133.curr);}));}(*(( struct
_tuple14*) fields2->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp2134=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2134->hd=( void*)({
struct _tuple16* _temp2135=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2135->f1=(*(( struct _tuple14*) fields2->hd)).f1; _temp2135->f2=
_temp2109; _temp2135;}); _temp2134->tl= ans; _temp2134;}); break;}} if( fields2
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2136=* _temp2129;
xprintf("bad field designator %.*s", _temp2136.last_plus_one - _temp2136.curr,
_temp2136.curr);}));} goto _LL2122;} _LL2122:;}}} for( 0; fields != 0; fields=
fields->tl){ if( !(*(( struct _tuple14*) fields->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2137=( char*)"too few arguments to struct";
struct _tagged_string _temp2138; _temp2138.curr= _temp2137; _temp2138.base=
_temp2137; _temp2138.last_plus_one= _temp2137 + 28; _temp2138;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp2139= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp2145; struct Cyc_Absyn_Conref* _temp2147;
struct Cyc_Absyn_Tqual* _temp2149; struct Cyc_Absyn_Conref* _temp2151; void*
_temp2153; void* _temp2155; _LL2141: if(( unsigned int) _temp2139 > 5u?(( struct
_enum_struct*) _temp2139)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2146:
_temp2145=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2139)->f1; _LL2156: _temp2155=( void*) _temp2145.elt_typ; goto _LL2154;
_LL2154: _temp2153=( void*) _temp2145.rgn_typ; goto _LL2152; _LL2152: _temp2151=(
struct Cyc_Absyn_Conref*) _temp2145.nullable; goto _LL2150; _LL2150: _temp2149=(
struct Cyc_Absyn_Tqual*) _temp2145.tq; goto _LL2148; _LL2148: _temp2147=( struct
Cyc_Absyn_Conref*) _temp2145.bounds; goto _LL2142;} else{ goto _LL2143;} _LL2143:
goto _LL2144; _LL2142: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2147); void*
_temp2157=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp2165; _LL2159: if(( unsigned int)
_temp2157 > 1u?(( struct _enum_struct*) _temp2157)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2166: _temp2165=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2157)->f1;
if( _temp2165 == Cyc_Absyn_Unknown_b){ goto _LL2160;} else{ goto _LL2161;}}
else{ goto _LL2161;} _LL2161: if( _temp2157 == Cyc_Absyn_No_constr){ goto
_LL2162;} else{ goto _LL2163;} _LL2163: goto _LL2164; _LL2160: return 1; _LL2162:(
void*)( b->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2167=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2167->tag= Cyc_Absyn_Eq_constr_tag; _temp2167->f1=( void*) Cyc_Absyn_Unknown_b;(
void*) _temp2167;})); return 1; _LL2164: return 0; _LL2158:;} _LL2144: return 0;
_LL2140:;} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp2168;
_temp2168.f1= 0; _temp2168.f2= Cyc_Absyn_HeapRgn; _temp2168;}); void* _temp2169=(
void*) e->r; void* _temp2183; struct _tuple1* _temp2185; struct _tagged_string*
_temp2187; struct Cyc_Absyn_Exp* _temp2189; struct _tagged_string* _temp2191;
struct Cyc_Absyn_Exp* _temp2193; struct Cyc_Absyn_Exp* _temp2195; struct Cyc_Absyn_Exp*
_temp2197; struct Cyc_Absyn_Exp* _temp2199; _LL2171: if((( struct _enum_struct*)
_temp2169)->tag == Cyc_Absyn_Var_e_tag){ _LL2186: _temp2185=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp2169)->f1; goto _LL2184; _LL2184: _temp2183=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp2169)->f2; goto _LL2172;} else{
goto _LL2173;} _LL2173: if((( struct _enum_struct*) _temp2169)->tag == Cyc_Absyn_StructMember_e_tag){
_LL2190: _temp2189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2169)->f1; goto _LL2188; _LL2188: _temp2187=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp2169)->f2; goto _LL2174;} else{
goto _LL2175;} _LL2175: if((( struct _enum_struct*) _temp2169)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL2194: _temp2193=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2169)->f1; goto _LL2192; _LL2192: _temp2191=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp2169)->f2; goto _LL2176;} else{
goto _LL2177;} _LL2177: if((( struct _enum_struct*) _temp2169)->tag == Cyc_Absyn_Deref_e_tag){
_LL2196: _temp2195=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp2169)->f1; goto _LL2178;} else{ goto _LL2179;} _LL2179: if((( struct
_enum_struct*) _temp2169)->tag == Cyc_Absyn_Subscript_e_tag){ _LL2200: _temp2199=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp2169)->f1;
goto _LL2198; _LL2198: _temp2197=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2169)->f2; goto _LL2180;} else{ goto _LL2181;} _LL2181: goto _LL2182;
_LL2172: { void* _temp2201= _temp2183; struct Cyc_Absyn_Fndecl* _temp2215;
struct Cyc_Absyn_Vardecl* _temp2217; struct Cyc_Absyn_Vardecl* _temp2219; struct
Cyc_Absyn_Vardecl* _temp2221; struct Cyc_Absyn_Vardecl* _temp2223; _LL2203: if(
_temp2201 == Cyc_Absyn_Unresolved_b){ goto _LL2204;} else{ goto _LL2205;}
_LL2205: if(( unsigned int) _temp2201 > 1u?(( struct _enum_struct*) _temp2201)->tag
== Cyc_Absyn_Funname_b_tag: 0){ _LL2216: _temp2215=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp2201)->f1; goto _LL2206;} else{ goto
_LL2207;} _LL2207: if(( unsigned int) _temp2201 > 1u?(( struct _enum_struct*)
_temp2201)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL2218: _temp2217=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp2201)->f1; goto _LL2208;} else{ goto
_LL2209;} _LL2209: if(( unsigned int) _temp2201 > 1u?(( struct _enum_struct*)
_temp2201)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL2220: _temp2219=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp2201)->f1; goto _LL2210;} else{ goto
_LL2211;} _LL2211: if(( unsigned int) _temp2201 > 1u?(( struct _enum_struct*)
_temp2201)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL2222: _temp2221=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp2201)->f1; goto _LL2212;} else{ goto
_LL2213;} _LL2213: if(( unsigned int) _temp2201 > 1u?(( struct _enum_struct*)
_temp2201)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL2224: _temp2223=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp2201)->f1; goto _LL2214;} else{ goto
_LL2202;} _LL2204: return bogus_ans; _LL2206: return({ struct _tuple7 _temp2225;
_temp2225.f1= 1; _temp2225.f2= Cyc_Absyn_HeapRgn; _temp2225;}); _LL2208: { void*
_temp2226= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_Exp*
_temp2232; struct Cyc_Absyn_Tqual* _temp2234; void* _temp2236; _LL2228: if((
unsigned int) _temp2226 > 5u?(( struct _enum_struct*) _temp2226)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2237: _temp2236=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2226)->f1;
goto _LL2235; _LL2235: _temp2234=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2226)->f2; goto _LL2233; _LL2233: _temp2232=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2226)->f3; goto _LL2229;} else{ goto
_LL2230;} _LL2230: goto _LL2231; _LL2229: return({ struct _tuple7 _temp2238;
_temp2238.f1= 1; _temp2238.f2= Cyc_Absyn_HeapRgn; _temp2238;}); _LL2231: return({
struct _tuple7 _temp2239; _temp2239.f1=( _temp2217->tq)->q_const; _temp2239.f2=
Cyc_Absyn_HeapRgn; _temp2239;}); _LL2227:;} _LL2210: { void* _temp2240= Cyc_Tcutil_compress((
void*)( e->topt)->v); struct Cyc_Absyn_Exp* _temp2246; struct Cyc_Absyn_Tqual*
_temp2248; void* _temp2250; _LL2242: if(( unsigned int) _temp2240 > 5u?(( struct
_enum_struct*) _temp2240)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2251:
_temp2250=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2240)->f1; goto
_LL2249; _LL2249: _temp2248=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2240)->f2; goto _LL2247; _LL2247: _temp2246=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2240)->f3; goto _LL2243;} else{ goto
_LL2244;} _LL2244: goto _LL2245; _LL2243: return({ struct _tuple7 _temp2252;
_temp2252.f1= 1; _temp2252.f2=( void*)( _temp2219->region)->v; _temp2252;});
_LL2245: return({ struct _tuple7 _temp2253; _temp2253.f1=( _temp2219->tq)->q_const;
_temp2253.f2=( void*)( _temp2219->region)->v; _temp2253;}); _LL2241:;} _LL2212:
_temp2223= _temp2221; goto _LL2214; _LL2214: return({ struct _tuple7 _temp2254;
_temp2254.f1=( _temp2223->tq)->q_const; _temp2254.f2=( void*)( _temp2223->region)->v;
_temp2254;}); _LL2202:;} _LL2174: { void* _temp2255= Cyc_Tcutil_compress(( void*)(
_temp2189->topt)->v); struct Cyc_Absyn_Structdecl** _temp2261; struct Cyc_Absyn_Structdecl*
_temp2263; struct Cyc_List_List* _temp2264; struct _tuple1* _temp2266; _LL2257:
if(( unsigned int) _temp2255 > 5u?(( struct _enum_struct*) _temp2255)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL2267: _temp2266=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2255)->f1; goto _LL2265; _LL2265: _temp2264=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2255)->f2; goto _LL2262; _LL2262:
_temp2261=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2255)->f3; if( _temp2261 == 0){ goto _LL2259;} else{ _temp2263=* _temp2261;
goto _LL2258;}} else{ goto _LL2259;} _LL2259: goto _LL2260; _LL2258: { struct
_tuple4* finfo= Cyc_Absyn_lookup_struct_field( _temp2263, _temp2187); if( finfo
!= 0){ return({ struct _tuple7 _temp2268; _temp2268.f1=((* finfo).f2)->q_const;
_temp2268.f2=( Cyc_Tcutil_addressof_props( te, _temp2189)).f2; _temp2268;});}
goto _LL2260;} _LL2260: return bogus_ans; _LL2256:;} _LL2176: { void* _temp2269=
Cyc_Tcutil_compress(( void*)( _temp2193->topt)->v); struct Cyc_Absyn_PtrInfo
_temp2275; struct Cyc_Absyn_Conref* _temp2277; struct Cyc_Absyn_Tqual* _temp2279;
struct Cyc_Absyn_Conref* _temp2281; void* _temp2283; void* _temp2285; _LL2271:
if(( unsigned int) _temp2269 > 5u?(( struct _enum_struct*) _temp2269)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL2276: _temp2275=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp2269)->f1; _LL2286: _temp2285=( void*)
_temp2275.elt_typ; goto _LL2284; _LL2284: _temp2283=( void*) _temp2275.rgn_typ;
goto _LL2282; _LL2282: _temp2281=( struct Cyc_Absyn_Conref*) _temp2275.nullable;
goto _LL2280; _LL2280: _temp2279=( struct Cyc_Absyn_Tqual*) _temp2275.tq; goto
_LL2278; _LL2278: _temp2277=( struct Cyc_Absyn_Conref*) _temp2275.bounds; goto
_LL2272;} else{ goto _LL2273;} _LL2273: goto _LL2274; _LL2272: { void* _temp2287=
Cyc_Tcutil_compress( _temp2285); struct Cyc_Absyn_Structdecl** _temp2293; struct
Cyc_Absyn_Structdecl* _temp2295; struct Cyc_List_List* _temp2296; struct _tuple1*
_temp2298; _LL2289: if(( unsigned int) _temp2287 > 5u?(( struct _enum_struct*)
_temp2287)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2299: _temp2298=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2287)->f1; goto _LL2297;
_LL2297: _temp2296=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2287)->f2; goto _LL2294; _LL2294: _temp2293=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2287)->f3; if( _temp2293 == 0){ goto
_LL2291;} else{ _temp2295=* _temp2293; goto _LL2290;}} else{ goto _LL2291;}
_LL2291: goto _LL2292; _LL2290: { struct _tuple4* finfo= Cyc_Absyn_lookup_struct_field(
_temp2295, _temp2191); if( finfo != 0){ return({ struct _tuple7 _temp2300;
_temp2300.f1=((* finfo).f2)->q_const; _temp2300.f2= _temp2283; _temp2300;});}
goto _LL2292;} _LL2292: return bogus_ans; _LL2288:;} _LL2274: return bogus_ans;
_LL2270:;} _LL2178: { void* _temp2301= Cyc_Tcutil_compress(( void*)( _temp2195->topt)->v);
struct Cyc_Absyn_PtrInfo _temp2307; struct Cyc_Absyn_Conref* _temp2309; struct
Cyc_Absyn_Tqual* _temp2311; struct Cyc_Absyn_Conref* _temp2313; void* _temp2315;
void* _temp2317; _LL2303: if(( unsigned int) _temp2301 > 5u?(( struct
_enum_struct*) _temp2301)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2308:
_temp2307=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2301)->f1; _LL2318: _temp2317=( void*) _temp2307.elt_typ; goto _LL2316;
_LL2316: _temp2315=( void*) _temp2307.rgn_typ; goto _LL2314; _LL2314: _temp2313=(
struct Cyc_Absyn_Conref*) _temp2307.nullable; goto _LL2312; _LL2312: _temp2311=(
struct Cyc_Absyn_Tqual*) _temp2307.tq; goto _LL2310; _LL2310: _temp2309=( struct
Cyc_Absyn_Conref*) _temp2307.bounds; goto _LL2304;} else{ goto _LL2305;} _LL2305:
goto _LL2306; _LL2304: return({ struct _tuple7 _temp2319; _temp2319.f1=
_temp2311->q_const; _temp2319.f2= _temp2315; _temp2319;}); _LL2306: return
bogus_ans; _LL2302:;} _LL2180: { void* t= Cyc_Tcutil_compress(( void*)(
_temp2199->topt)->v); void* _temp2320= t; struct Cyc_Absyn_Exp* _temp2330;
struct Cyc_Absyn_Tqual* _temp2332; void* _temp2334; struct Cyc_List_List*
_temp2336; struct Cyc_Absyn_PtrInfo _temp2338; struct Cyc_Absyn_Conref*
_temp2340; struct Cyc_Absyn_Tqual* _temp2342; struct Cyc_Absyn_Conref* _temp2344;
void* _temp2346; void* _temp2348; _LL2322: if(( unsigned int) _temp2320 > 5u?((
struct _enum_struct*) _temp2320)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2335:
_temp2334=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2320)->f1; goto
_LL2333; _LL2333: _temp2332=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2320)->f2; goto _LL2331; _LL2331: _temp2330=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2320)->f3; goto _LL2323;} else{ goto
_LL2324;} _LL2324: if(( unsigned int) _temp2320 > 5u?(( struct _enum_struct*)
_temp2320)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL2337: _temp2336=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2320)->f1; goto _LL2325;} else{ goto
_LL2326;} _LL2326: if(( unsigned int) _temp2320 > 5u?(( struct _enum_struct*)
_temp2320)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2339: _temp2338=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2320)->f1;
_LL2349: _temp2348=( void*) _temp2338.elt_typ; goto _LL2347; _LL2347: _temp2346=(
void*) _temp2338.rgn_typ; goto _LL2345; _LL2345: _temp2344=( struct Cyc_Absyn_Conref*)
_temp2338.nullable; goto _LL2343; _LL2343: _temp2342=( struct Cyc_Absyn_Tqual*)
_temp2338.tq; goto _LL2341; _LL2341: _temp2340=( struct Cyc_Absyn_Conref*)
_temp2338.bounds; goto _LL2327;} else{ goto _LL2328;} _LL2328: goto _LL2329;
_LL2323: return({ struct _tuple7 _temp2350; _temp2350.f1= _temp2332->q_const;
_temp2350.f2=( Cyc_Tcutil_addressof_props( te, _temp2199)).f2; _temp2350;});
_LL2325: { struct _tuple5* finfo= Cyc_Absyn_lookup_tuple_field( _temp2336,( int)
Cyc_Evexp_eval_const_uint_exp( _temp2197)); if( finfo != 0){ return({ struct
_tuple7 _temp2351; _temp2351.f1=((* finfo).f1)->q_const; _temp2351.f2=( Cyc_Tcutil_addressof_props(
te, _temp2199)).f2; _temp2351;});} return bogus_ans;} _LL2327: return({ struct
_tuple7 _temp2352; _temp2352.f1= _temp2342->q_const; _temp2352.f2= _temp2346;
_temp2352;}); _LL2329: return bogus_ans; _LL2321:;} _LL2182: Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ char* _temp2353=( char*)"unary & applied to non-lvalue";
struct _tagged_string _temp2354; _temp2354.curr= _temp2353; _temp2354.base=
_temp2353; _temp2354.last_plus_one= _temp2353 + 30; _temp2354;})); return
bogus_ans; _LL2170:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp2355= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp2361; struct Cyc_Absyn_Tqual* _temp2363; void*
_temp2365; _LL2357: if(( unsigned int) _temp2355 > 5u?(( struct _enum_struct*)
_temp2355)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2366: _temp2365=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2355)->f1; goto _LL2364; _LL2364:
_temp2363=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2355)->f2; goto _LL2362; _LL2362: _temp2361=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2355)->f3; goto _LL2358;} else{ goto
_LL2359;} _LL2359: goto _LL2360; _LL2358: { void* _temp2369; int _temp2371;
struct _tuple7 _temp2367= Cyc_Tcutil_addressof_props( te, e); _LL2372: _temp2371=
_temp2367.f1; goto _LL2370; _LL2370: _temp2369= _temp2367.f2; goto _LL2368;
_LL2368: return Cyc_Absyn_atb_typ( _temp2365, _temp2369, _temp2363,({ struct Cyc_Absyn_Upper_b_struct*
_temp2373=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp2373->tag= Cyc_Absyn_Upper_b_tag; _temp2373->f1= e;( void*) _temp2373;}));}
_LL2360: return e_typ; _LL2356:;} void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*
loc, unsigned int i, struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( b);{ void* _temp2374=(
void*) b->v; void* _temp2382; void* _temp2384; struct Cyc_Absyn_Exp* _temp2386;
_LL2376: if(( unsigned int) _temp2374 > 1u?(( struct _enum_struct*) _temp2374)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL2383: _temp2382=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2374)->f1; if( _temp2382 == Cyc_Absyn_Unknown_b){ goto _LL2377;} else{ goto
_LL2378;}} else{ goto _LL2378;} _LL2378: if(( unsigned int) _temp2374 > 1u?((
struct _enum_struct*) _temp2374)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2385:
_temp2384=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2374)->f1; if((
unsigned int) _temp2384 > 1u?(( struct _enum_struct*) _temp2384)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2387: _temp2386=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2384)->f1; goto _LL2379;} else{ goto _LL2380;}} else{ goto _LL2380;}
_LL2380: goto _LL2381; _LL2377: return; _LL2379: if( Cyc_Evexp_eval_const_uint_exp(
_temp2386) <= i){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2388=( char*)"dereference is out of bounds"; struct _tagged_string
_temp2389; _temp2389.curr= _temp2388; _temp2389.base= _temp2388; _temp2389.last_plus_one=
_temp2388 + 29; _temp2389;}));} return; _LL2381:( void*)( b->v=( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp2390=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2390->tag= Cyc_Absyn_Eq_constr_tag;
_temp2390->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp2391=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp2391->tag= Cyc_Absyn_Upper_b_tag;
_temp2391->f1= Cyc_Absyn_uint_exp( i +( unsigned int) 1, 0);( void*) _temp2391;});(
void*) _temp2390;})); return; _LL2375:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);}