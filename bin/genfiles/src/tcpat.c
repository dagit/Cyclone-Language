#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2;}; struct _tuple1{ void* f1; struct
_tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2;}; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2;}; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_List_List* f2;}; struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2;}; struct _tuple7{ struct Cyc_Absyn_Tqual* f1; void*
f2;}; struct _tuple8{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3;}; struct _tuple9{ struct _tuple8* f1; struct Cyc_Absyn_Pat* f2;};
struct _tuple10{ struct Cyc_Absyn_Enumdecl* f1;}; struct _tuple11{ struct Cyc_Absyn_Xenumdecl*
f1;}; struct _tuple12{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;};
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3;}; struct _tuple14{ int f1; struct Cyc_Position_Segment* f2;}; struct
_tuple15{ void* f1; struct _tuple14* f2;}; struct _tuple16{ int f1;}; struct
_tuple17{ struct Cyc_Position_Segment* f1; int f2;}; struct _tuple18{ void* f1;
int f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef
struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct
Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*);
extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; extern struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env); struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char* tag;};
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality(
struct Cyc_Set_Set* s); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void*
elt); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Stdio___sFILE; typedef
struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void*
Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple1*); typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1;}; extern const int Cyc_Tcenv_StructRes_tag;
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
Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern char Cyc_Tcutil_TypeErr_tag[ 8u];
struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*,
void*); extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern struct _tuple5* Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Pat* p, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); static struct _tuple6* Cyc_Tcpat_null_designator(
struct Cyc_Absyn_Pat* p){ return({ struct _tuple6* _temp0=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp0->f1= 0; _temp0->f2= p; _temp0;});}
static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} struct _tuple5* Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** region_opt){ void* t; struct Cyc_List_List*
tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp1=( void*) p->r;
int _temp33; void* _temp35; char _temp37; struct _tagged_string _temp39; struct
Cyc_Absyn_Vardecl* _temp41; struct Cyc_Absyn_Vardecl* _temp43; struct Cyc_Absyn_Pat*
_temp45; struct Cyc_List_List* _temp47; struct Cyc_List_List* _temp49; struct
Cyc_List_List* _temp51; struct Cyc_Core_Opt* _temp53; struct Cyc_Core_Opt**
_temp55; struct Cyc_Absyn_Structdecl* _temp56; struct Cyc_Absyn_Enumfield*
_temp58; struct Cyc_Absyn_Enumdecl* _temp60; struct Cyc_List_List* _temp62;
struct Cyc_List_List* _temp64; struct Cyc_Core_Opt* _temp66; struct _tuple1*
_temp68; struct Cyc_Absyn_Enumfield* _temp70; struct Cyc_Absyn_Xenumdecl*
_temp72; struct Cyc_List_List* _temp74; struct Cyc_List_List* _temp76; struct
_tuple1* _temp78; struct _tuple1* _temp80; struct Cyc_List_List* _temp82; struct
Cyc_List_List* _temp84; struct _tuple1* _temp86; struct Cyc_List_List* _temp88;
struct Cyc_List_List* _temp90; struct _tuple1* _temp92; _LL3: if( _temp1 == Cyc_Absyn_Wild_p){
goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int) _temp1 > 2u?(( struct
_enum_struct*) _temp1)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL36: _temp35=( void*)((
struct Cyc_Absyn_Int_p_struct*) _temp1)->f1; goto _LL34; _LL34: _temp33=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1)->f2; goto _LL6;} else{ goto _LL7;} _LL7:
if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL38: _temp37=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1)->f1; goto
_LL8;} else{ goto _LL9;} _LL9: if(( unsigned int) _temp1 > 2u?(( struct
_enum_struct*) _temp1)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL40: _temp39=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1)->f1; goto
_LL10;} else{ goto _LL11;} _LL11: if( _temp1 == Cyc_Absyn_Null_p){ goto _LL12;}
else{ goto _LL13;} _LL13: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*)
_temp1)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL42: _temp41=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1)->f1; goto _LL14;} else{ goto _LL15;}
_LL15: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_Reference_p_tag: 0){ _LL44: _temp43=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp1)->f1; goto _LL16;} else{ goto _LL17;}
_LL17: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_Pointer_p_tag: 0){ _LL46: _temp45=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Pointer_p_struct*) _temp1)->f1; goto _LL18;} else{ goto _LL19;} _LL19:
if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL48: _temp47=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1)->f1; goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL57:
_temp56=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f1; goto _LL54; _LL54: _temp53=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f2; _temp55=&(( struct Cyc_Absyn_Struct_p_struct*) _temp1)->f2; goto
_LL52; _LL52: _temp51=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f3; goto _LL50; _LL50: _temp49=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f4; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL69:
_temp68=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1)->f1; goto
_LL67; _LL67: _temp66=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f2; goto _LL65; _LL65: _temp64=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f3; goto _LL63; _LL63: _temp62=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f4; goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp1)->f5; goto _LL59; _LL59: _temp58=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1)->f6; goto _LL24;} else{ goto _LL25;}
_LL25: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_Xenum_p_tag: 0){ _LL79: _temp78=( struct _tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1)->f1; goto _LL77; _LL77: _temp76=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1)->f2; goto _LL75; _LL75: _temp74=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1)->f3; goto _LL73; _LL73: _temp72=( struct Cyc_Absyn_Xenumdecl*)(( struct
Cyc_Absyn_Xenum_p_struct*) _temp1)->f4; goto _LL71; _LL71: _temp70=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1)->f5; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_UnknownId_p_tag: 0){ _LL81: _temp80=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1)->f1; goto _LL28;} else{ goto _LL29;} _LL29: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL87: _temp86=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f1; goto _LL85; _LL85: _temp84=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f2; goto _LL83; _LL83: _temp82=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f3; goto _LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL93: _temp92=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f1; goto _LL91; _LL91: _temp90=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f2; goto _LL89; _LL89: _temp88=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f3; goto _LL32;} else{ goto _LL2;} _LL4: t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
goto _LL2; _LL6: t= _temp35 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL2; _LL8: t= Cyc_Absyn_uchar_t; goto _LL2; _LL10: t= Cyc_Absyn_float_t;
goto _LL2; _LL12: t=({ struct Cyc_Absyn_PointerType_struct* _temp94=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp94->tag= Cyc_Absyn_PointerType_tag;
_temp94->f1=({ struct Cyc_Absyn_PtrInfo _temp95; _temp95.elt_typ=( void*) Cyc_Absyn_new_evar(
Cyc_Absyn_AnyKind); _temp95.rgn_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind);
_temp95.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp95.tq= Cyc_Absyn_empty_tqual(); _temp95.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp95;});( void*) _temp94;}); goto _LL2; _LL14: t=
Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);( void*)( _temp41->type=( void*) t);
_temp41->tq= Cyc_Absyn_empty_tqual(); _temp41->shadow= 0; v_result=({ struct Cyc_List_List*
_temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*) _temp41; _temp96->tl= v_result; _temp96;}); goto _LL2;
_LL16: t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind); if( region_opt == 0){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ char* _temp97=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 45; _temp98;})); goto _LL2;}( void*)( _temp43->type=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp99=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp99->tag= Cyc_Absyn_PointerType_tag;
_temp99->f1=({ struct Cyc_Absyn_PtrInfo _temp100; _temp100.elt_typ=( void*) t;
_temp100.rgn_typ=( void*)* region_opt; _temp100.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp100.tq= Cyc_Absyn_empty_tqual();
_temp100.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp100;});( void*) _temp99;})); _temp43->tq= Cyc_Absyn_empty_tqual(); _temp43->shadow=
0; v_result=({ struct Cyc_List_List* _temp101=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp101->hd=( void*) _temp43; _temp101->tl=
v_result; _temp101;}); goto _LL2; _LL18: { void* ptr_rgn= Cyc_Absyn_new_evar(
Cyc_Absyn_RgnKind); struct _tuple5 _temp104; struct Cyc_List_List* _temp105;
struct Cyc_List_List* _temp107; struct _tuple5* _temp102= Cyc_Tcpat_tcPat( te,
_temp45,( void**)& ptr_rgn); _temp104=* _temp102; _LL108: _temp107= _temp104.f1;
goto _LL106; _LL106: _temp105= _temp104.f2; goto _LL103; _LL103: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp107); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp105); t=({ struct
Cyc_Absyn_PointerType_struct* _temp109=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp109->tag= Cyc_Absyn_PointerType_tag;
_temp109->f1=({ struct Cyc_Absyn_PtrInfo _temp110; _temp110.elt_typ=( void*)((
void*)( _temp45->topt)->v); _temp110.rgn_typ=( void*) ptr_rgn; _temp110.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp110.tq= Cyc_Absyn_empty_tqual();
_temp110.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp110;});( void*) _temp109;}); goto _LL2;} _LL20: { struct Cyc_List_List* ts=
0; for( 0; _temp47 != 0; _temp47= _temp47->tl){ struct _tuple5 _temp113; struct
Cyc_List_List* _temp114; struct Cyc_List_List* _temp116; struct _tuple5*
_temp111= Cyc_Tcpat_tcPat( te,( struct Cyc_Absyn_Pat*) _temp47->hd, region_opt);
_temp113=* _temp111; _LL117: _temp116= _temp113.f1; goto _LL115; _LL115:
_temp114= _temp113.f2; goto _LL112; _LL112: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp116); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp114); ts=({ struct Cyc_List_List*
_temp118=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp118->hd=( void*)({ struct _tuple7* _temp119=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp119->f1= Cyc_Absyn_empty_tqual(); _temp119->f2=(
void*)((( struct Cyc_Absyn_Pat*) _temp47->hd)->topt)->v; _temp119;}); _temp118->tl=
ts; _temp118;});} t=({ struct Cyc_Absyn_TupleType_struct* _temp120=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp120->tag= Cyc_Absyn_TupleType_tag;
_temp120->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ts);( void*) _temp120;}); goto _LL2;} _LL22: if( _temp51 != 0){ return(( struct
_tuple5*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp121=( char*)"tcPat: struct<...> not implemented"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 35; _temp122;}));}{ struct Cyc_List_List*
dps0= _temp49; for( 0; dps0 != 0; dps0= dps0->tl){ struct _tuple5 _temp125;
struct Cyc_List_List* _temp126; struct Cyc_List_List* _temp128; struct _tuple5*
_temp123= Cyc_Tcpat_tcPat( te,(*(( struct _tuple6*) dps0->hd)).f2, region_opt);
_temp125=* _temp123; _LL129: _temp128= _temp125.f1; goto _LL127; _LL127:
_temp126= _temp125.f2; goto _LL124; _LL124: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp128); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp126);}}{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp56->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst);* _temp55=({ struct Cyc_Core_Opt* _temp130=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp130->v=( void*) all_typs;
_temp130;}); t=({ struct Cyc_Absyn_StructType_struct* _temp131=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp131->tag= Cyc_Absyn_StructType_tag;
_temp131->f1=( struct _tuple1*)(( struct _tuple1*)( _temp56->name)->v); _temp131->f2=
all_typs; _temp131->f3=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp132=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp132[ 0]= _temp56;
_temp132;}));( void*) _temp131;});{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc, _temp49, _temp56); for( 0;
fields != 0; fields= fields->tl){ struct _tuple9 _temp135; struct Cyc_Absyn_Pat*
_temp136; struct _tuple8* _temp138; struct _tuple8 _temp140; void* _temp141;
struct Cyc_Absyn_Tqual* _temp143; struct _tagged_string* _temp145; struct
_tuple9* _temp133=( struct _tuple9*) fields->hd; _temp135=* _temp133; _LL139:
_temp138= _temp135.f1; _temp140=* _temp138; _LL146: _temp145= _temp140.f1; goto
_LL144; _LL144: _temp143= _temp140.f2; goto _LL142; _LL142: _temp141= _temp140.f3;
goto _LL137; _LL137: _temp136= _temp135.f2; goto _LL134; _LL134: { void*
inst_fieldtyp= Cyc_Tcutil_substitute( all_inst, _temp141); if( ! Cyc_Tcutil_unify((
void*)( _temp136->topt)->v, inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp147=* _temp145; struct _tagged_string _temp148= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp149= Cyc_Absynpp_typ2string(( void*)(
_temp136->topt)->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp147.last_plus_one - _temp147.curr, _temp147.curr, _temp148.last_plus_one -
_temp148.curr, _temp148.curr, _temp149.last_plus_one - _temp149.curr, _temp149.curr);}));}}}
goto _LL2;}} _LL24: { void* heap= Cyc_Absyn_HeapRgn; struct Cyc_Tcenv_Tenv* te2=
Cyc_Tcenv_add_type_vars( p->loc, te, _temp64); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp64);{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp60->tvs); struct Cyc_List_List* all_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, all_inst); t=({ struct
Cyc_Absyn_EnumType_struct* _temp150=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp150->tag= Cyc_Absyn_EnumType_tag;
_temp150->f1=( struct _tuple1*)(( struct _tuple1*)( _temp60->name)->v); _temp150->f2=
all_typs; _temp150->f3=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_Enumdecl**)({
struct _tuple10* _temp151=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp151->f1= _temp60; _temp151;}));( void*) _temp150;});{ struct Cyc_List_List*
tqts= _temp58->typs; for( 0; _temp62 != 0? tqts != 0: 0; _temp62= _temp62->tl,
tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) _temp62->hd;
struct _tuple5 _temp154; struct Cyc_List_List* _temp155; struct Cyc_List_List*
_temp157; struct _tuple5* _temp152= Cyc_Tcpat_tcPat( te2, p2,( void**)& heap);
_temp154=* _temp152; _LL158: _temp157= _temp154.f1; goto _LL156; _LL156:
_temp155= _temp154.f2; goto _LL153; _LL153: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp157); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp155);{ void* field_typ= Cyc_Tcutil_substitute(
all_inst,(*(( struct _tuple7*) tqts->hd)).f2); if( ! Cyc_Tcutil_unify(( void*)(
p2->topt)->v, field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string
_temp159= Cyc_Absynpp_qvar2string( _temp68); struct _tagged_string _temp160= Cyc_Absynpp_typ2string(
field_typ); struct _tagged_string _temp161= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp159.last_plus_one -
_temp159.curr, _temp159.curr, _temp160.last_plus_one - _temp160.curr, _temp160.curr,
_temp161.last_plus_one - _temp161.curr, _temp161.curr);}));}}} if( _temp62 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp162= Cyc_Absynpp_qvar2string(
_temp68); xprintf("too many arguments for enum constructor %.*s", _temp162.last_plus_one
- _temp162.curr, _temp162.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp163= Cyc_Absynpp_qvar2string( _temp68); xprintf("too few arguments for enum constructor %.*s",
_temp163.last_plus_one - _temp163.curr, _temp163.curr);}));} goto _LL2;}}} _LL26: {
void* heap= Cyc_Absyn_HeapRgn; struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
p->loc, te, _temp76); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp76); t=({ struct
Cyc_Absyn_XenumType_struct* _temp164=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp164->tag= Cyc_Absyn_XenumType_tag;
_temp164->f1= _temp72->name; _temp164->f2=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_Xenumdecl**)({ struct _tuple11* _temp165=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp165->f1= _temp72; _temp165;}));( void*)
_temp164;});{ struct Cyc_List_List* tqts= _temp70->typs; for( 0; _temp74 != 0?
tqts != 0: 0; _temp74= _temp74->tl, tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) _temp74->hd; struct _tuple5 _temp168; struct Cyc_List_List*
_temp169; struct Cyc_List_List* _temp171; struct _tuple5* _temp166= Cyc_Tcpat_tcPat(
te2, p2,( void**)& heap); _temp168=* _temp166; _LL172: _temp171= _temp168.f1;
goto _LL170; _LL170: _temp169= _temp168.f2; goto _LL167; _LL167: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp171); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp169);{ void*
field_typ=(*(( struct _tuple7*) tqts->hd)).f2; if( ! Cyc_Tcutil_unify(( void*)(
p2->topt)->v, field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string
_temp173= Cyc_Absynpp_qvar2string( _temp78); struct _tagged_string _temp174= Cyc_Absynpp_typ2string(
field_typ); struct _tagged_string _temp175= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp173.last_plus_one -
_temp173.curr, _temp173.curr, _temp174.last_plus_one - _temp174.curr, _temp174.curr,
_temp175.last_plus_one - _temp175.curr, _temp175.curr);}));}}} if( _temp74 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp176= Cyc_Absynpp_qvar2string(
_temp78); xprintf("too many arguments for xenum constructor %.*s", _temp176.last_plus_one
- _temp176.curr, _temp176.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp177= Cyc_Absynpp_qvar2string( _temp78); xprintf("too few arguments for xenum constructor %.*s",
_temp177.last_plus_one - _temp177.curr, _temp177.curr);}));} goto _LL2;}} _LL28:{
struct _handler_cons _temp178; _push_handler(& _temp178);{ struct _xenum_struct*
_temp179=( struct _xenum_struct*) setjmp( _temp178.handler); if( ! _temp179){{
void* _temp180= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp80); struct Cyc_Absyn_Structdecl*
_temp190; struct Cyc_Absyn_Enumfield* _temp192; struct Cyc_Absyn_Enumdecl*
_temp194; struct Cyc_Absyn_Enumfield* _temp196; struct Cyc_Absyn_Xenumdecl*
_temp198; void* _temp200; _LL182: if((( struct _enum_struct*) _temp180)->tag ==
Cyc_Tcenv_StructRes_tag){ _LL191: _temp190=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp180)->f1; goto _LL183;} else{ goto
_LL184;} _LL184: if((( struct _enum_struct*) _temp180)->tag == Cyc_Tcenv_EnumRes_tag){
_LL195: _temp194=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp180)->f1; goto _LL193; _LL193: _temp192=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp180)->f2; goto _LL185;} else{ goto _LL186;}
_LL186: if((( struct _enum_struct*) _temp180)->tag == Cyc_Tcenv_XenumRes_tag){
_LL199: _temp198=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp180)->f1; goto _LL197; _LL197: _temp196=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp180)->f2; goto _LL187;} else{ goto
_LL188;} _LL188: if((( struct _enum_struct*) _temp180)->tag == Cyc_Tcenv_VarRes_tag){
_LL201: _temp200=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp180)->f1; goto
_LL189;} else{ goto _LL181;} _LL183: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp202=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp203; _temp203.curr= _temp202; _temp203.base= _temp202;
_temp203.last_plus_one= _temp202 + 45; _temp203;})); t= Cyc_Absyn_wildtyp();
goto _LL181; _LL185:( void*)( p->r=( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp204=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp204->tag= Cyc_Absyn_Enum_p_tag; _temp204->f1= _temp192->name; _temp204->f2=
0; _temp204->f3= 0; _temp204->f4= 0; _temp204->f5= _temp194; _temp204->f6=
_temp192;( void*) _temp204;}));{ struct _tuple5* _temp205= Cyc_Tcpat_tcPat( te,
p, region_opt); _npop_handler( 0u); return _temp205;} _LL187:( void*)( p->r=(
void*)({ struct Cyc_Absyn_Xenum_p_struct* _temp206=( struct Cyc_Absyn_Xenum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenum_p_struct)); _temp206->tag= Cyc_Absyn_Xenum_p_tag;
_temp206->f1= _temp196->name; _temp206->f2= 0; _temp206->f3= 0; _temp206->f4=
_temp198; _temp206->f5= _temp196;( void*) _temp206;}));{ struct _tuple5*
_temp207= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp207;} _LL189:{ void* _temp208=(* _temp80).f1; struct Cyc_List_List*
_temp216; _LL210: if( _temp208 == Cyc_Absyn_Loc_n){ goto _LL211;} else{ goto
_LL212;} _LL212: if(( unsigned int) _temp208 > 1u?(( struct _enum_struct*)
_temp208)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL217: _temp216=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp208)->f1; if( _temp216 == 0){ goto _LL213;}
else{ goto _LL214;}} else{ goto _LL214;} _LL214: goto _LL215; _LL211: goto
_LL213; _LL213:(* _temp80).f1= Cyc_Absyn_Loc_n;( void*)( p->r=( void*)({ struct
Cyc_Absyn_Var_p_struct* _temp218=( struct Cyc_Absyn_Var_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Var_p_struct)); _temp218->tag= Cyc_Absyn_Var_p_tag;
_temp218->f1= Cyc_Absyn_new_vardecl( _temp80, Cyc_Absyn_VoidType, 0);( void*)
_temp218;}));{ struct _tuple5* _temp219= Cyc_Tcpat_tcPat( te, p, region_opt);
_npop_handler( 0u); return _temp219;} _LL215: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp220=( char*)"qualified variable in pattern"; struct
_tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 30; _temp221;})); goto _LL209; _LL209:;} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp222= Cyc_Absynpp_qvar2string( _temp80);
xprintf("unresolved identifier %.*s in pattern", _temp222.last_plus_one -
_temp222.curr, _temp222.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL181; _LL181:;}
_pop_handler();} else{ struct _xenum_struct* _temp224= _temp179; _LL226: if(
_temp224->tag == Cyc_Dict_Absent_tag){ goto _LL227;} else{ goto _LL228;} _LL228:
goto _LL229; _LL227:{ void* _temp230=(* _temp80).f1; struct Cyc_List_List*
_temp238; _LL232: if( _temp230 == Cyc_Absyn_Loc_n){ goto _LL233;} else{ goto
_LL234;} _LL234: if(( unsigned int) _temp230 > 1u?(( struct _enum_struct*)
_temp230)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL239: _temp238=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp230)->f1; if( _temp238 == 0){ goto _LL235;}
else{ goto _LL236;}} else{ goto _LL236;} _LL236: goto _LL237; _LL233: goto
_LL235; _LL235:(* _temp80).f1= Cyc_Absyn_Loc_n;( void*)( p->r=( void*)({ struct
Cyc_Absyn_Var_p_struct* _temp240=( struct Cyc_Absyn_Var_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Var_p_struct)); _temp240->tag= Cyc_Absyn_Var_p_tag;
_temp240->f1= Cyc_Absyn_new_vardecl( _temp80, Cyc_Absyn_VoidType, 0);( void*)
_temp240;})); return Cyc_Tcpat_tcPat( te, p, region_opt); _LL237: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp241= Cyc_Absynpp_qvar2string( _temp80);
xprintf("unbound identifier %.*s in pattern", _temp241.last_plus_one - _temp241.curr,
_temp241.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL231; _LL231:;} goto _LL225;
_LL229:( void) _throw( _temp224); _LL225:;}}} goto _LL2; _LL30:{ struct
_handler_cons _temp242; _push_handler(& _temp242);{ struct _xenum_struct*
_temp243=( struct _xenum_struct*) setjmp( _temp242.handler); if( ! _temp243){{
void* _temp244= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp86); struct Cyc_Absyn_Structdecl*
_temp254; struct Cyc_Absyn_Enumfield* _temp256; struct Cyc_Absyn_Enumdecl*
_temp258; struct Cyc_Absyn_Enumfield* _temp260; struct Cyc_Absyn_Xenumdecl*
_temp262; void* _temp264; _LL246: if((( struct _enum_struct*) _temp244)->tag ==
Cyc_Tcenv_StructRes_tag){ _LL255: _temp254=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp244)->f1; goto _LL247;} else{ goto
_LL248;} _LL248: if((( struct _enum_struct*) _temp244)->tag == Cyc_Tcenv_EnumRes_tag){
_LL259: _temp258=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp244)->f1; goto _LL257; _LL257: _temp256=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp244)->f2; goto _LL249;} else{ goto _LL250;}
_LL250: if((( struct _enum_struct*) _temp244)->tag == Cyc_Tcenv_XenumRes_tag){
_LL263: _temp262=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp244)->f1; goto _LL261; _LL261: _temp260=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp244)->f2; goto _LL251;} else{ goto
_LL252;} _LL252: if((( struct _enum_struct*) _temp244)->tag == Cyc_Tcenv_VarRes_tag){
_LL265: _temp264=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp244)->f1; goto
_LL253;} else{ goto _LL245;} _LL247: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp82);( void*)( p->r=( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp266=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp266->tag= Cyc_Absyn_Struct_p_tag; _temp266->f1= _temp254; _temp266->f2= 0;
_temp266->f3= _temp84; _temp266->f4= x;( void*) _temp266;}));{ struct _tuple5*
_temp267= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp267;}} _LL249:( void*)( p->r=( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp268=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp268->tag= Cyc_Absyn_Enum_p_tag; _temp268->f1= _temp256->name; _temp268->f2=
0; _temp268->f3= _temp84; _temp268->f4= _temp82; _temp268->f5= _temp258;
_temp268->f6= _temp256;( void*) _temp268;}));{ struct _tuple5* _temp269= Cyc_Tcpat_tcPat(
te, p, region_opt); _npop_handler( 0u); return _temp269;} _LL251:( void*)( p->r=(
void*)({ struct Cyc_Absyn_Xenum_p_struct* _temp270=( struct Cyc_Absyn_Xenum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenum_p_struct)); _temp270->tag= Cyc_Absyn_Xenum_p_tag;
_temp270->f1= _temp260->name; _temp270->f2= _temp84; _temp270->f3= _temp82;
_temp270->f4= _temp262; _temp270->f5= _temp260;( void*) _temp270;}));{ struct
_tuple5* _temp271= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp271;} _LL253: goto _LL245; _LL245:;} _pop_handler();} else{ struct
_xenum_struct* _temp273= _temp243; _LL275: if( _temp273->tag == Cyc_Dict_Absent_tag){
goto _LL276;} else{ goto _LL277;} _LL277: goto _LL278; _LL276: goto _LL274;
_LL278:( void) _throw( _temp273); _LL274:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp279= Cyc_Absynpp_qvar2string( _temp86); xprintf("%.*s is not a constructor in pattern",
_temp279.last_plus_one - _temp279.curr, _temp279.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL2; _LL32:{ struct _handler_cons _temp280; _push_handler(& _temp280);{
struct _xenum_struct* _temp281=( struct _xenum_struct*) setjmp( _temp280.handler);
if( ! _temp281){{ void* _temp282= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp92);
struct Cyc_Absyn_Structdecl* _temp292; struct Cyc_Absyn_Enumfield* _temp294;
struct Cyc_Absyn_Enumdecl* _temp296; struct Cyc_Absyn_Enumfield* _temp298;
struct Cyc_Absyn_Xenumdecl* _temp300; void* _temp302; _LL284: if((( struct
_enum_struct*) _temp282)->tag == Cyc_Tcenv_StructRes_tag){ _LL293: _temp292=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp282)->f1;
goto _LL285;} else{ goto _LL286;} _LL286: if((( struct _enum_struct*) _temp282)->tag
== Cyc_Tcenv_EnumRes_tag){ _LL297: _temp296=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp282)->f1; goto _LL295; _LL295: _temp294=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp282)->f2;
goto _LL287;} else{ goto _LL288;} _LL288: if((( struct _enum_struct*) _temp282)->tag
== Cyc_Tcenv_XenumRes_tag){ _LL301: _temp300=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp282)->f1; goto _LL299; _LL299: _temp298=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_XenumRes_struct*) _temp282)->f2;
goto _LL289;} else{ goto _LL290;} _LL290: if((( struct _enum_struct*) _temp282)->tag
== Cyc_Tcenv_VarRes_tag){ _LL303: _temp302=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp282)->f1; goto _LL291;} else{ goto _LL283;} _LL285:( void*)( p->r=( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp304=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp304->tag= Cyc_Absyn_Struct_p_tag;
_temp304->f1= _temp292; _temp304->f2= 0; _temp304->f3= _temp90; _temp304->f4=
_temp88;( void*) _temp304;}));{ struct _tuple5* _temp305= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp305;} _LL287: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp306=( char*)"enum with designators not implemented";
struct _tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 38; _temp307;})); t= Cyc_Absyn_wildtyp();
goto _LL283; _LL289: Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp308=( char*)"xenum with designators not implemented"; struct _tagged_string
_temp309; _temp309.curr= _temp308; _temp309.base= _temp308; _temp309.last_plus_one=
_temp308 + 39; _temp309;})); t= Cyc_Absyn_wildtyp(); goto _LL283; _LL291: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp310= Cyc_Absynpp_qvar2string( _temp92);
xprintf("unbound struct %.*s in pattern", _temp310.last_plus_one - _temp310.curr,
_temp310.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL283; _LL283:;} _pop_handler();}
else{ struct _xenum_struct* _temp312= _temp281; _LL314: if( _temp312->tag == Cyc_Dict_Absent_tag){
goto _LL315;} else{ goto _LL316;} _LL316: goto _LL317; _LL315: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp318= Cyc_Absynpp_qvar2string( _temp92);
xprintf("unbound struct %.*s in pattern", _temp318.last_plus_one - _temp318.curr,
_temp318.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL313; _LL317:( void) _throw(
_temp312); _LL313:;}}} goto _LL2; _LL2:;} p->topt=({ struct Cyc_Core_Opt*
_temp319=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp319->v=( void*) t; _temp319;}); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Tcpat_get_name, v_result), p->loc,( struct _tagged_string)({
char* _temp320=( char*)"pattern contains a repeated variable"; struct
_tagged_string _temp321; _temp321.curr= _temp320; _temp321.base= _temp320;
_temp321.last_plus_one= _temp320 + 37; _temp321;})); return({ struct _tuple5*
_temp322=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp322->f1=
tv_result; _temp322->f2= v_result; _temp322;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp323=( void*) p->r;
struct Cyc_Absyn_Pat* _temp337; struct Cyc_List_List* _temp339; struct Cyc_List_List*
_temp341; struct Cyc_Core_Opt* _temp343; struct Cyc_Absyn_Structdecl* _temp345;
struct Cyc_Absyn_Enumfield* _temp347; struct Cyc_Absyn_Xenumdecl* _temp349;
struct Cyc_List_List* _temp351; struct Cyc_List_List* _temp353; struct _tuple1*
_temp355; struct Cyc_Absyn_Enumfield* _temp357; struct Cyc_Absyn_Enumdecl*
_temp359; struct Cyc_List_List* _temp361; struct Cyc_List_List* _temp363; struct
Cyc_Core_Opt* _temp365; struct _tuple1* _temp367; struct Cyc_List_List* _temp369;
_LL325: if(( unsigned int) _temp323 > 2u?(( struct _enum_struct*) _temp323)->tag
== Cyc_Absyn_Pointer_p_tag: 0){ _LL338: _temp337=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp323)->f1; goto _LL326;} else{ goto
_LL327;} _LL327: if(( unsigned int) _temp323 > 2u?(( struct _enum_struct*)
_temp323)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL346: _temp345=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp323)->f1; goto _LL344; _LL344: _temp343=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp323)->f2; goto
_LL342; _LL342: _temp341=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp323)->f3; goto _LL340; _LL340: _temp339=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp323)->f4; goto _LL328;} else{ goto _LL329;}
_LL329: if(( unsigned int) _temp323 > 2u?(( struct _enum_struct*) _temp323)->tag
== Cyc_Absyn_Xenum_p_tag: 0){ _LL356: _temp355=( struct _tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp323)->f1; goto _LL354; _LL354: _temp353=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_p_struct*) _temp323)->f2; goto _LL352; _LL352: _temp351=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp323)->f3; goto _LL350;
_LL350: _temp349=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp323)->f4; goto _LL348; _LL348: _temp347=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp323)->f5; goto _LL330;} else{ goto _LL331;}
_LL331: if(( unsigned int) _temp323 > 2u?(( struct _enum_struct*) _temp323)->tag
== Cyc_Absyn_Enum_p_tag: 0){ _LL368: _temp367=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp323)->f1; goto _LL366; _LL366: _temp365=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp323)->f2; goto _LL364; _LL364: _temp363=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp323)->f3; goto _LL362;
_LL362: _temp361=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp323)->f4; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp323)->f5; goto _LL358; _LL358: _temp357=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp323)->f6;
goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int) _temp323 > 2u?((
struct _enum_struct*) _temp323)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL370:
_temp369=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp323)->f1;
goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336; _LL326: Cyc_Tcpat_check_pat_regions(
te, _temp337);{ void* _temp371=( void*)( p->topt)->v; struct Cyc_Absyn_PtrInfo
_temp377; struct Cyc_Absyn_Conref* _temp379; struct Cyc_Absyn_Tqual* _temp381;
struct Cyc_Absyn_Conref* _temp383; void* _temp385; void* _temp387; _LL373: if((
unsigned int) _temp371 > 5u?(( struct _enum_struct*) _temp371)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL378: _temp377=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp371)->f1; _LL388: _temp387=( void*) _temp377.elt_typ; goto _LL386; _LL386:
_temp385=( void*) _temp377.rgn_typ; goto _LL384; _LL384: _temp383=( struct Cyc_Absyn_Conref*)
_temp377.nullable; goto _LL382; _LL382: _temp381=( struct Cyc_Absyn_Tqual*)
_temp377.tq; goto _LL380; _LL380: _temp379=( struct Cyc_Absyn_Conref*) _temp377.bounds;
goto _LL374;} else{ goto _LL375;} _LL375: goto _LL376; _LL374: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp385); return; _LL376:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp389=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp389=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp390=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 36; _temp391;})};( struct _xenum_struct*)
_temp389;})); _LL372:;} _LL328: _temp351=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp339); goto _LL330; _LL330: _temp361=
_temp351; goto _LL332; _LL332: _temp369= _temp361; goto _LL334; _LL334: for( 0;
_temp369 != 0; _temp369= _temp369->tl){ Cyc_Tcpat_check_pat_regions( te,( struct
Cyc_Absyn_Pat*) _temp369->hd);} return; _LL336: return; _LL324:;} typedef void*
Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_string f1;}; static const int Cyc_Tcpat_Int_v_tag= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1;}; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span;};
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; typedef void* Cyc_Tcpat_Simple_pat;
static void* Cyc_Tcpat_Any=( void*) 0; static const int Cyc_Tcpat_Con_tag= 0;
struct Cyc_Tcpat_Con_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2;}; typedef void* Cyc_Tcpat_simple_pat_t; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp392=( void*)
c1->name; struct _tagged_string _temp398; int _temp400; _LL394: if((( struct
_enum_struct*) _temp392)->tag == Cyc_Tcpat_Name_v_tag){ _LL399: _temp398=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp392)->f1; goto
_LL395;} else{ goto _LL396;} _LL396: if((( struct _enum_struct*) _temp392)->tag
== Cyc_Tcpat_Int_v_tag){ _LL401: _temp400=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp392)->f1; goto _LL397;} else{ goto _LL393;} _LL395: { void* _temp402=( void*)
c2->name; struct _tagged_string _temp408; int _temp410; _LL404: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Tcpat_Name_v_tag){ _LL409: _temp408=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp402)->f1; goto
_LL405;} else{ goto _LL406;} _LL406: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Tcpat_Int_v_tag){ _LL411: _temp410=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp402)->f1; goto _LL407;} else{ goto _LL403;} _LL405: return Cyc_String_zstrcmp(
_temp398, _temp408); _LL407: return - 1; _LL403:;} _LL397: { void* _temp412=(
void*) c2->name; struct _tagged_string _temp418; int _temp420; _LL414: if(((
struct _enum_struct*) _temp412)->tag == Cyc_Tcpat_Name_v_tag){ _LL419: _temp418=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp412)->f1; goto
_LL415;} else{ goto _LL416;} _LL416: if((( struct _enum_struct*) _temp412)->tag
== Cyc_Tcpat_Int_v_tag){ _LL421: _temp420=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp412)->f1; goto _LL417;} else{ goto _LL413;} _LL415: return 1; _LL417:
return _temp400 - _temp420; _LL413:;} _LL393:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){
return(( struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct
Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt
Cyc_Tcpat_one_opt=( struct Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt=( struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char
_temp425[ 6u]="tuple"; static struct Cyc_Tcpat_Name_v_struct _temp422={ 0u,(
struct _tagged_string){ _temp425, _temp425, _temp425 + 6u}}; static void* Cyc_Tcpat_tuple_name=(
struct Cyc_Tcpat_Name_v_struct*)& _temp422; static char _temp429[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct _temp426={ 0u,( struct _tagged_string){
_temp429, _temp429, _temp429 + 5u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0=(
struct Cyc_Tcpat_Con_s){.name=( void*)(( struct Cyc_Tcpat_Name_v_struct*)&
_temp426),.arity= 0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static
char _temp433[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct _temp430={ 0u,(
struct _tagged_string){ _temp433, _temp433, _temp433 + 4u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( struct Cyc_Tcpat_Name_v_struct*)&
_temp430),.arity= 1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static
char _temp437[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct _temp434={ 0u,(
struct _tagged_string){ _temp437, _temp437, _temp437 + 4u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( struct Cyc_Tcpat_Name_v_struct*)&
_temp434),.arity= 1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp438=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp438->name=(
void*)({ struct Cyc_Tcpat_Int_v_struct* _temp439=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp439->tag= Cyc_Tcpat_Int_v_tag;
_temp439->f1= i;( void*) _temp439;}); _temp438->arity= 0; _temp438->span= 0;
_temp438;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({
struct Cyc_Tcpat_Con_s* _temp440=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp440->name=( void*)({ struct Cyc_Tcpat_Int_v_struct*
_temp442=( struct Cyc_Tcpat_Int_v_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp442->tag= Cyc_Tcpat_Int_v_tag; _temp442->f1=( int) c;( void*) _temp442;});
_temp440->arity= 0; _temp440->span=({ struct Cyc_Core_Opt* _temp441=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp441->v=( void*) 256;
_temp441;}); _temp440;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con(
struct _tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp443=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp443->name=( void*)({ struct
Cyc_Tcpat_Name_v_struct* _temp444=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp444->tag= Cyc_Tcpat_Name_v_tag;
_temp444->f1= f;( void*) _temp444;}); _temp443->arity= 0; _temp443->span= 0;
_temp443;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_double_con( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_s* _temp445=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp445->name=( void*)({ struct
Cyc_Tcpat_Name_v_struct* _temp446=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp446->tag= Cyc_Tcpat_Name_v_tag;
_temp446->f1= d;( void*) _temp446;}); _temp445->arity= 0; _temp445->span= 0;
_temp445;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({
struct Cyc_Tcpat_Con_s* _temp447=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp447->name=( void*) Cyc_Tcpat_tuple_name; _temp447->arity=
i; _temp447->span= Cyc_Tcpat_one_opt_ptr; _temp447;});} static void* Cyc_Tcpat_null_pat(){
return({ struct Cyc_Tcpat_Con_struct* _temp448=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp448->tag= Cyc_Tcpat_Con_tag;
_temp448->f1= Cyc_Tcpat_null_con; _temp448->f2= 0;( void*) _temp448;});} static
void* Cyc_Tcpat_int_pat( int i){ return({ struct Cyc_Tcpat_Con_struct* _temp449=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp449->tag= Cyc_Tcpat_Con_tag; _temp449->f1= Cyc_Tcpat_int_con( i); _temp449->f2=
0;( void*) _temp449;});} static void* Cyc_Tcpat_char_pat( char c){ return({
struct Cyc_Tcpat_Con_struct* _temp450=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp450->tag= Cyc_Tcpat_Con_tag;
_temp450->f1= Cyc_Tcpat_char_con( c); _temp450->f2= 0;( void*) _temp450;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_string f){ return({ struct Cyc_Tcpat_Con_struct*
_temp451=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp451->tag= Cyc_Tcpat_Con_tag; _temp451->f1= Cyc_Tcpat_float_con( f);
_temp451->f2= 0;( void*) _temp451;});} static void* Cyc_Tcpat_double_pat( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_struct* _temp452=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp452->tag= Cyc_Tcpat_Con_tag;
_temp452->f1= Cyc_Tcpat_double_con( d); _temp452->f2= 0;( void*) _temp452;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return({ struct Cyc_Tcpat_Con_struct*
_temp453=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp453->tag= Cyc_Tcpat_Con_tag; _temp453->f1= Cyc_Tcpat_null_ptr_con; _temp453->f2=({
struct Cyc_List_List* _temp454=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp454->hd=( void*) p; _temp454->tl= 0; _temp454;});(
void*) _temp453;});} static void* Cyc_Tcpat_ptr_pat( void* p){ return({ struct
Cyc_Tcpat_Con_struct* _temp455=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp455->tag= Cyc_Tcpat_Con_tag;
_temp455->f1= Cyc_Tcpat_ptr_con; _temp455->f2=({ struct Cyc_List_List* _temp456=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp456->hd=(
void*) p; _temp456->tl= 0; _temp456;});( void*) _temp455;});} static void* Cyc_Tcpat_tuple_pat(
struct Cyc_List_List* ss){ return({ struct Cyc_Tcpat_Con_struct* _temp457=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp457->tag= Cyc_Tcpat_Con_tag; _temp457->f1= Cyc_Tcpat_tuple_con((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ss)); _temp457->f2= ss;( void*)
_temp457;});} static void* Cyc_Tcpat_con_pat( struct _tagged_string con_name,
struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){ struct Cyc_Tcpat_Con_s* c=({
struct Cyc_Tcpat_Con_s* _temp458=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp458->name=( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp459=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp459->tag= Cyc_Tcpat_Name_v_tag; _temp459->f1= con_name;( void*) _temp459;});
_temp458->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ps);
_temp458->span= span; _temp458;}); return({ struct Cyc_Tcpat_Con_struct*
_temp460=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp460->tag= Cyc_Tcpat_Con_tag; _temp460->f1= c; _temp460->f2= ps;( void*)
_temp460;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp461=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp493; int
_temp495; void* _temp497; char _temp499; struct _tagged_string _temp501; struct
Cyc_List_List* _temp503; struct Cyc_Absyn_Vardecl* _temp505; struct Cyc_Absyn_Pat*
_temp507; struct Cyc_List_List* _temp509; struct Cyc_List_List* _temp511; struct
Cyc_Core_Opt* _temp513; struct Cyc_Absyn_Structdecl* _temp515; struct Cyc_Absyn_Enumfield*
_temp517; struct Cyc_Absyn_Enumdecl* _temp519; struct Cyc_List_List* _temp521;
struct Cyc_List_List* _temp523; struct Cyc_Core_Opt* _temp525; struct _tuple1*
_temp527; struct Cyc_Absyn_Enumfield* _temp529; struct Cyc_Absyn_Xenumdecl*
_temp531; struct Cyc_List_List* _temp533; struct Cyc_List_List* _temp535; struct
_tuple1* _temp537; struct _tuple1* _temp539; struct Cyc_List_List* _temp541;
struct Cyc_List_List* _temp543; struct _tuple1* _temp545; struct Cyc_List_List*
_temp547; struct Cyc_List_List* _temp549; struct _tuple1* _temp551; _LL463: if(
_temp461 == Cyc_Absyn_Wild_p){ goto _LL464;} else{ goto _LL465;} _LL465: if((
unsigned int) _temp461 > 2u?(( struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL494: _temp493=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp461)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if( _temp461 == Cyc_Absyn_Null_p){
goto _LL468;} else{ goto _LL469;} _LL469: if(( unsigned int) _temp461 > 2u?((
struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL498:
_temp497=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp461)->f1; goto _LL496;
_LL496: _temp495=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp461)->f2; goto
_LL470;} else{ goto _LL471;} _LL471: if(( unsigned int) _temp461 > 2u?(( struct
_enum_struct*) _temp461)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL500: _temp499=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp461)->f1; goto _LL472;} else{ goto
_LL473;} _LL473: if(( unsigned int) _temp461 > 2u?(( struct _enum_struct*)
_temp461)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL502: _temp501=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp461)->f1; goto _LL474;}
else{ goto _LL475;} _LL475: if(( unsigned int) _temp461 > 2u?(( struct
_enum_struct*) _temp461)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL504: _temp503=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp461)->f1; goto
_LL476;} else{ goto _LL477;} _LL477: if(( unsigned int) _temp461 > 2u?(( struct
_enum_struct*) _temp461)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL506: _temp505=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp461)->f1;
goto _LL478;} else{ goto _LL479;} _LL479: if(( unsigned int) _temp461 > 2u?((
struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL508:
_temp507=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp461)->f1;
goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int) _temp461 > 2u?((
struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL516:
_temp515=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp461)->f1; goto _LL514; _LL514: _temp513=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp461)->f2; goto _LL512; _LL512: _temp511=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp461)->f3; goto
_LL510; _LL510: _temp509=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp461)->f4; goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int)
_temp461 > 2u?(( struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Enum_p_tag: 0){
_LL528: _temp527=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp461)->f1;
goto _LL526; _LL526: _temp525=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp461)->f2; goto _LL524; _LL524: _temp523=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp461)->f3; goto _LL522; _LL522: _temp521=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp461)->f4; goto _LL520;
_LL520: _temp519=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp461)->f5; goto _LL518; _LL518: _temp517=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp461)->f6; goto _LL484;} else{ goto _LL485;}
_LL485: if(( unsigned int) _temp461 > 2u?(( struct _enum_struct*) _temp461)->tag
== Cyc_Absyn_Xenum_p_tag: 0){ _LL538: _temp537=( struct _tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp461)->f1; goto _LL536; _LL536: _temp535=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_p_struct*) _temp461)->f2; goto _LL534; _LL534: _temp533=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp461)->f3; goto _LL532;
_LL532: _temp531=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp461)->f4; goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp461)->f5; goto _LL486;} else{ goto _LL487;}
_LL487: if(( unsigned int) _temp461 > 2u?(( struct _enum_struct*) _temp461)->tag
== Cyc_Absyn_UnknownId_p_tag: 0){ _LL540: _temp539=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownId_p_struct*) _temp461)->f1; goto _LL488;} else{ goto _LL489;}
_LL489: if(( unsigned int) _temp461 > 2u?(( struct _enum_struct*) _temp461)->tag
== Cyc_Absyn_UnknownCall_p_tag: 0){ _LL546: _temp545=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp461)->f1; goto _LL544; _LL544: _temp543=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp461)->f2;
goto _LL542; _LL542: _temp541=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp461)->f3; goto _LL490;} else{ goto _LL491;} _LL491: if(( unsigned int)
_temp461 > 2u?(( struct _enum_struct*) _temp461)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL552: _temp551=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp461)->f1; goto _LL550; _LL550: _temp549=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp461)->f2; goto _LL548; _LL548: _temp547=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp461)->f3;
goto _LL492;} else{ goto _LL462;} _LL464: s= Cyc_Tcpat_Any; goto _LL462; _LL466:
s= Cyc_Tcpat_Any; goto _LL462; _LL468: s= Cyc_Tcpat_null_pat(); goto _LL462;
_LL470: s= Cyc_Tcpat_int_pat( _temp495); goto _LL462; _LL472: s= Cyc_Tcpat_char_pat(
_temp499); goto _LL462; _LL474: s= Cyc_Tcpat_float_pat( _temp501); goto _LL462;
_LL476: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp503)); goto _LL462; _LL478: s= Cyc_Tcpat_Any; goto _LL462; _LL480: { void*
ss= Cyc_Tcpat_compile_pat( _temp507);{ void* _temp553= Cyc_Tcutil_compress((
void*)( p->topt)->v); struct Cyc_Absyn_PtrInfo _temp559; struct Cyc_Absyn_Conref*
_temp561; struct Cyc_Absyn_Tqual* _temp563; struct Cyc_Absyn_Conref* _temp565;
void* _temp567; void* _temp569; _LL555: if(( unsigned int) _temp553 > 5u?((
struct _enum_struct*) _temp553)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL560:
_temp559=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp553)->f1; _LL570: _temp569=( void*) _temp559.elt_typ; goto _LL568; _LL568:
_temp567=( void*) _temp559.rgn_typ; goto _LL566; _LL566: _temp565=( struct Cyc_Absyn_Conref*)
_temp559.nullable; goto _LL564; _LL564: _temp563=( struct Cyc_Absyn_Tqual*)
_temp559.tq; goto _LL562; _LL562: _temp561=( struct Cyc_Absyn_Conref*) _temp559.bounds;
goto _LL556;} else{ goto _LL557;} _LL557: goto _LL558; _LL556: { int is_nullable=
0; int still_working= 1; while( still_working) { void* _temp571=( void*)
_temp565->v; struct Cyc_Absyn_Conref* _temp579; int _temp581; _LL573: if((
unsigned int) _temp571 > 1u?(( struct _enum_struct*) _temp571)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL580: _temp579=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp571)->f1; goto _LL574;} else{ goto _LL575;} _LL575: if( _temp571 == Cyc_Absyn_No_constr){
goto _LL576;} else{ goto _LL577;} _LL577: if(( unsigned int) _temp571 > 1u?((
struct _enum_struct*) _temp571)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL582:
_temp581=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp571)->f1; goto _LL578;}
else{ goto _LL572;} _LL574:( void*)( _temp565->v=( void*)(( void*) _temp579->v));
continue; _LL576:( void*)( _temp565->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp583=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Eq_constr_struct)); _temp583->tag= Cyc_Absyn_Eq_constr_tag; _temp583->f1=(
void*) 0;( void*) _temp583;})); is_nullable= 0; still_working= 0; goto _LL572;
_LL578: is_nullable=( int) _temp581; still_working= 0; goto _LL572; _LL572:;}
if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat( ss);} else{ s= Cyc_Tcpat_ptr_pat(
ss);} goto _LL554;} _LL558: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp584=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp585; _temp585.curr= _temp584; _temp585.base= _temp584;
_temp585.last_plus_one= _temp584 + 44; _temp585;})); goto _LL554; _LL554:;} goto
_LL462;} _LL482: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)( _temp515->fields)->v; for( 0; fields != 0; fields=
fields->tl){ int found= 0;{ struct Cyc_List_List* dlps0= _temp509; for( 0; !
found? dlps0 != 0: 0; dlps0= dlps0->tl){ struct _tuple6 _temp588; struct Cyc_Absyn_Pat*
_temp589; struct Cyc_List_List* _temp591; struct _tuple6* _temp586=( struct
_tuple6*) dlps0->hd; _temp588=* _temp586; _LL592: _temp591= _temp588.f1; goto
_LL590; _LL590: _temp589= _temp588.f2; goto _LL587; _LL587: { struct Cyc_List_List*
_temp593= _temp591; struct Cyc_List_List _temp599; struct Cyc_List_List*
_temp600; void* _temp602; struct _tagged_string* _temp604; _LL595: if( _temp593
== 0){ goto _LL597;} else{ _temp599=* _temp593; _LL603: _temp602=( void*)
_temp599.hd; if((( struct _enum_struct*) _temp602)->tag == Cyc_Absyn_FieldName_tag){
_LL605: _temp604=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp602)->f1; goto _LL601;} else{ goto _LL597;} _LL601: _temp600=( struct Cyc_List_List*)
_temp599.tl; if( _temp600 == 0){ goto _LL596;} else{ goto _LL597;}} _LL597: goto
_LL598; _LL596: if( Cyc_String_zstrptrcmp( _temp604,(*(( struct _tuple8*) fields->hd)).f1)
== 0){ ps=({ struct Cyc_List_List* _temp606=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp606->hd=( void*) Cyc_Tcpat_compile_pat(
_temp589); _temp606->tl= ps; _temp606;}); found= 1;} goto _LL594; _LL598:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp607=( char*)"bad designator(s)"; struct _tagged_string _temp608; _temp608.curr=
_temp607; _temp608.base= _temp607; _temp608.last_plus_one= _temp607 + 18;
_temp608;})); goto _LL594; _LL594:;}}} if( ! found){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp609=(
char*)"bad designator"; struct _tagged_string _temp610; _temp610.curr= _temp609;
_temp610.base= _temp609; _temp610.last_plus_one= _temp609 + 15; _temp610;}));}}}
s= Cyc_Tcpat_tuple_pat( ps); goto _LL462;} _LL484: { int span=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)( _temp519->fields)->v);
s= Cyc_Tcpat_con_pat(*(* _temp517->name).f2,({ struct Cyc_Core_Opt* _temp611=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp611->v=(
void*) span; _temp611;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp521)); goto
_LL462;} _LL486: s= Cyc_Tcpat_con_pat(*(* _temp529->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp533)); goto _LL462; _LL488: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp612=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp613; _temp613.curr= _temp612; _temp613.base= _temp612;
_temp613.last_plus_one= _temp612 + 23; _temp613;})); goto _LL462; _LL490: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp614=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp615; _temp615.curr= _temp614; _temp615.base= _temp614; _temp615.last_plus_one=
_temp614 + 25; _temp615;})); goto _LL462; _LL492: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp616=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp617; _temp617.curr=
_temp616; _temp617.base= _temp616; _temp617.last_plus_one= _temp616 + 27;
_temp617;})); goto _LL462; _LL462:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
const int Cyc_Tcpat_Pos_tag= 0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2;}; const int Cyc_Tcpat_Neg_tag= 1; struct Cyc_Tcpat_Neg_struct{
int tag; struct Cyc_Set_Set* f1;}; typedef void* Cyc_Tcpat_term_desc_t; typedef
void* Cyc_Tcpat_Access; static void* Cyc_Tcpat_Obj=( void*) 0; static const int
Cyc_Tcpat_Sel_tag= 0; struct Cyc_Tcpat_Sel_struct{ int tag; int f1; void* f2;};
typedef void* Cyc_Tcpat_access_t; typedef void* Cyc_Tcpat_Decision; static void*
Cyc_Tcpat_Failure=( void*) 0; static const int Cyc_Tcpat_Success_tag= 0; struct
Cyc_Tcpat_Success_struct{ int tag; void* f1;}; static const int Cyc_Tcpat_IfEq_tag=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; void* f1; struct Cyc_Tcpat_Con_s* f2;
void* f3; void* f4;}; typedef void* Cyc_Tcpat_decision_t; typedef void* Cyc_Tcpat_Answer;
static void* Cyc_Tcpat_Yes=( void*) 0; static void* Cyc_Tcpat_No=( void*) 1;
static void* Cyc_Tcpat_Maybe=( void*) 2; typedef void* Cyc_Tcpat_answer_t;
static void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void*
_temp618= td; struct Cyc_Set_Set* _temp624; struct Cyc_List_List* _temp626;
struct Cyc_Tcpat_Con_s* _temp628; _LL620: if((( struct _enum_struct*) _temp618)->tag
== Cyc_Tcpat_Neg_tag){ _LL625: _temp624=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp618)->f1; goto _LL621;} else{ goto _LL622;} _LL622: if((( struct
_enum_struct*) _temp618)->tag == Cyc_Tcpat_Pos_tag){ _LL629: _temp628=( struct
Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp618)->f1; goto _LL627;
_LL627: _temp626=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp618)->f2; goto _LL623;} else{ goto _LL619;} _LL621: if((( int(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp624, c)){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp630=( char*)"add_neg called when constructor already in set"; struct
_tagged_string _temp631; _temp631.curr= _temp630; _temp631.base= _temp630;
_temp631.last_plus_one= _temp630 + 47; _temp631;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp624) + 1 >=( int)( c->span)->v:
0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp632=( char*)"add_neg called when |cs U {c}| >= span(c)"; struct
_tagged_string _temp633; _temp633.curr= _temp632; _temp633.base= _temp632;
_temp633.last_plus_one= _temp632 + 42; _temp633;}));} return({ struct Cyc_Tcpat_Neg_struct*
_temp634=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp634->tag= Cyc_Tcpat_Neg_tag; _temp634->f1=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp624, c);(
void*) _temp634;}); _LL623: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp635=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 35; _temp636;})); _LL619:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp637= td; struct Cyc_List_List*
_temp643; struct Cyc_Tcpat_Con_s* _temp645; struct Cyc_Set_Set* _temp647; _LL639:
if((( struct _enum_struct*) _temp637)->tag == Cyc_Tcpat_Pos_tag){ _LL646:
_temp645=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp637)->f1;
goto _LL644; _LL644: _temp643=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp637)->f2; goto _LL640;} else{ goto _LL641;} _LL641: if((( struct
_enum_struct*) _temp637)->tag == Cyc_Tcpat_Neg_tag){ _LL648: _temp647=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp637)->f1; goto _LL642;} else{
goto _LL638;} _LL640: if( Cyc_Tcpat_compare_con( c, _temp645) == 0){ return Cyc_Tcpat_Yes;}
else{ return Cyc_Tcpat_No;} _LL642: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp647, c)){ return Cyc_Tcpat_No;}
else{ if( c->span != 0?( int)( c->span)->v ==(( int(*)( struct Cyc_Set_Set* s))
Cyc_Set_cardinality)( _temp647) + 1: 0){ return Cyc_Tcpat_Yes;} else{ return Cyc_Tcpat_Maybe;}}
_LL638:;} static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List*
ctxt, void* dsc){ struct Cyc_List_List* _temp649= ctxt; struct Cyc_List_List
_temp655; struct Cyc_List_List* _temp656; struct _tuple12* _temp658; struct
_tuple12 _temp660; struct Cyc_List_List* _temp661; struct Cyc_Tcpat_Con_s*
_temp663; _LL651: if( _temp649 == 0){ goto _LL652;} else{ goto _LL653;} _LL653:
if( _temp649 == 0){ goto _LL650;} else{ _temp655=* _temp649; _LL659: _temp658=(
struct _tuple12*) _temp655.hd; _temp660=* _temp658; _LL664: _temp663= _temp660.f1;
goto _LL662; _LL662: _temp661= _temp660.f2; goto _LL657; _LL657: _temp656=(
struct Cyc_List_List*) _temp655.tl; goto _LL654;} _LL652: return 0; _LL654:
return({ struct Cyc_List_List* _temp665=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp665->hd=( void*)({ struct _tuple12*
_temp666=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp666->f1=
_temp663; _temp666->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp667=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp667->hd=(
void*) dsc; _temp667->tl= _temp661; _temp667;}); _temp666;}); _temp665->tl=
_temp656; _temp665;}); _LL650:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp668= ctxt; struct Cyc_List_List
_temp674; struct Cyc_List_List* _temp675; struct _tuple12* _temp677; struct
_tuple12 _temp679; struct Cyc_List_List* _temp680; struct Cyc_Tcpat_Con_s*
_temp682; _LL670: if( _temp668 == 0){ goto _LL671;} else{ goto _LL672;} _LL672:
if( _temp668 == 0){ goto _LL669;} else{ _temp674=* _temp668; _LL678: _temp677=(
struct _tuple12*) _temp674.hd; _temp679=* _temp677; _LL683: _temp682= _temp679.f1;
goto _LL681; _LL681: _temp680= _temp679.f2; goto _LL676; _LL676: _temp675=(
struct Cyc_List_List*) _temp674.tl; goto _LL673;} _LL671: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp684=( char*)"norm_context: empty context"; struct _tagged_string _temp685;
_temp685.curr= _temp684; _temp685.base= _temp684; _temp685.last_plus_one=
_temp684 + 28; _temp685;})); _LL673: return Cyc_Tcpat_augment( _temp675,({
struct Cyc_Tcpat_Pos_struct* _temp686=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp686->tag= Cyc_Tcpat_Pos_tag;
_temp686->f1= _temp682; _temp686->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp680);( void*) _temp686;})); _LL669:;} static void* Cyc_Tcpat_build_desc(
struct Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct
_tuple5 _temp688=({ struct _tuple5 _temp687; _temp687.f1= ctxt; _temp687.f2=
work; _temp687;}); struct Cyc_List_List* _temp698; struct Cyc_List_List*
_temp700; struct Cyc_List_List* _temp702; struct Cyc_List_List* _temp704; struct
Cyc_List_List* _temp706; struct Cyc_List_List* _temp708; struct Cyc_List_List*
_temp710; struct Cyc_List_List _temp712; struct Cyc_List_List* _temp713; struct
_tuple13* _temp715; struct _tuple13 _temp717; struct Cyc_List_List* _temp718;
struct Cyc_List_List* _temp720; struct Cyc_List_List* _temp722; struct Cyc_List_List*
_temp724; struct Cyc_List_List _temp726; struct Cyc_List_List* _temp727; struct
_tuple12* _temp729; struct _tuple12 _temp731; struct Cyc_List_List* _temp732;
struct Cyc_Tcpat_Con_s* _temp734; _LL690: _LL701: _temp700= _temp688.f1; if(
_temp700 == 0){ goto _LL699;} else{ goto _LL692;} _LL699: _temp698= _temp688.f2;
if( _temp698 == 0){ goto _LL691;} else{ goto _LL692;} _LL692: _LL705: _temp704=
_temp688.f1; if( _temp704 == 0){ goto _LL703;} else{ goto _LL694;} _LL703:
_temp702= _temp688.f2; goto _LL693; _LL694: _LL709: _temp708= _temp688.f1; goto
_LL707; _LL707: _temp706= _temp688.f2; if( _temp706 == 0){ goto _LL695;} else{
goto _LL696;} _LL696: _LL725: _temp724= _temp688.f1; if( _temp724 == 0){ goto
_LL689;} else{ _temp726=* _temp724; _LL730: _temp729=( struct _tuple12*)
_temp726.hd; _temp731=* _temp729; _LL735: _temp734= _temp731.f1; goto _LL733;
_LL733: _temp732= _temp731.f2; goto _LL728; _LL728: _temp727=( struct Cyc_List_List*)
_temp726.tl; goto _LL711;} _LL711: _temp710= _temp688.f2; if( _temp710 == 0){
goto _LL689;} else{ _temp712=* _temp710; _LL716: _temp715=( struct _tuple13*)
_temp712.hd; _temp717=* _temp715; _LL723: _temp722= _temp717.f1; goto _LL721;
_LL721: _temp720= _temp717.f2; goto _LL719; _LL719: _temp718= _temp717.f3; goto
_LL714; _LL714: _temp713=( struct Cyc_List_List*) _temp712.tl; goto _LL697;}
_LL691: return dsc; _LL693: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp736=( char*)"build_desc: ctxt and work don't match";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 38; _temp737;})); _LL695: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp738=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp739; _temp739.curr= _temp738; _temp739.base= _temp738; _temp739.last_plus_one=
_temp738 + 38; _temp739;})); _LL697: { void* td=({ struct Cyc_Tcpat_Pos_struct*
_temp740=( struct Cyc_Tcpat_Pos_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp740->tag= Cyc_Tcpat_Pos_tag; _temp740->f1= _temp734; _temp740->f2=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp732),({
struct Cyc_List_List* _temp741=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp741->hd=( void*) dsc; _temp741->tl= _temp718;
_temp741;}));( void*) _temp740;}); return Cyc_Tcpat_build_desc( _temp727, td,
_temp713);} _LL689:;} static void* Cyc_Tcpat_match( void* p, void* obj, void*
dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp742=
allmrules; struct Cyc_List_List _temp748; struct Cyc_List_List* _temp749; struct
_tuple0* _temp751; struct _tuple0 _temp753; void* _temp754; void* _temp756;
_LL744: if( _temp742 == 0){ goto _LL745;} else{ goto _LL746;} _LL746: if(
_temp742 == 0){ goto _LL743;} else{ _temp748=* _temp742; _LL752: _temp751=(
struct _tuple0*) _temp748.hd; _temp753=* _temp751; _LL757: _temp756= _temp753.f1;
goto _LL755; _LL755: _temp754= _temp753.f2; goto _LL750; _LL750: _temp749=(
struct Cyc_List_List*) _temp748.tl; goto _LL747;} _LL745: return Cyc_Tcpat_Failure;
_LL747: return(( void*(*)( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp756, Cyc_Tcpat_Obj, dsc, 0, 0, _temp754, _temp749);
_LL743:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){
return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(({
struct Cyc_Tcpat_Neg_struct* _temp758=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp758->tag= Cyc_Tcpat_Neg_tag;
_temp758->f1= Cyc_Tcpat_empty_con_set();( void*) _temp758;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp759= work; struct Cyc_List_List _temp767; struct Cyc_List_List* _temp768;
struct _tuple13* _temp770; struct _tuple13 _temp772; struct Cyc_List_List*
_temp773; struct Cyc_List_List* _temp775; struct Cyc_List_List* _temp777; struct
Cyc_List_List _temp779; struct Cyc_List_List* _temp780; struct _tuple13*
_temp782; struct _tuple13 _temp784; struct Cyc_List_List* _temp785; struct Cyc_List_List*
_temp787; struct Cyc_List_List* _temp789; _LL761: if( _temp759 == 0){ goto
_LL762;} else{ goto _LL763;} _LL763: if( _temp759 == 0){ goto _LL765;} else{
_temp767=* _temp759; _LL771: _temp770=( struct _tuple13*) _temp767.hd; _temp772=*
_temp770; _LL778: _temp777= _temp772.f1; if( _temp777 == 0){ goto _LL776;} else{
goto _LL765;} _LL776: _temp775= _temp772.f2; if( _temp775 == 0){ goto _LL774;}
else{ goto _LL765;} _LL774: _temp773= _temp772.f3; if( _temp773 == 0){ goto
_LL769;} else{ goto _LL765;} _LL769: _temp768=( struct Cyc_List_List*) _temp767.tl;
goto _LL764;} _LL765: if( _temp759 == 0){ goto _LL760;} else{ _temp779=*
_temp759; _LL783: _temp782=( struct _tuple13*) _temp779.hd; _temp784=* _temp782;
_LL790: _temp789= _temp784.f1; goto _LL788; _LL788: _temp787= _temp784.f2; goto
_LL786; _LL786: _temp785= _temp784.f3; goto _LL781; _LL781: _temp780=( struct
Cyc_List_List*) _temp779.tl; goto _LL766;} _LL762: return({ struct Cyc_Tcpat_Success_struct*
_temp791=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp791->tag= Cyc_Tcpat_Success_tag; _temp791->f1=( void*) right_hand_side;(
void*) _temp791;}); _LL764: return(( void*(*)( struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
Cyc_Tcpat_norm_context( ctx), _temp768, right_hand_side, rules); _LL766: if((
_temp789 == 0? 1: _temp787 == 0)? 1: _temp785 == 0){ return(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp792=(
char*)"tcpat:and_match: malformed work frame"; struct _tagged_string _temp793;
_temp793.curr= _temp792; _temp793.base= _temp792; _temp793.last_plus_one=
_temp792 + 38; _temp793;}));}{ struct Cyc_List_List _temp797; struct Cyc_List_List*
_temp798; void* _temp800; struct Cyc_List_List* _temp795=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp794= _temp789; if( _temp794 == 0){ _throw(
Null_Exception);} _temp794;}); _temp797=* _temp795; _LL801: _temp800=( void*)
_temp797.hd; goto _LL799; _LL799: _temp798=( struct Cyc_List_List*) _temp797.tl;
goto _LL796; _LL796: { struct Cyc_List_List _temp805; struct Cyc_List_List*
_temp806; void* _temp808; struct Cyc_List_List* _temp803=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp802= _temp787; if( _temp802 == 0){ _throw(
Null_Exception);} _temp802;}); _temp805=* _temp803; _LL809: _temp808=( void*)
_temp805.hd; goto _LL807; _LL807: _temp806=( struct Cyc_List_List*) _temp805.tl;
goto _LL804; _LL804: { struct Cyc_List_List _temp813; struct Cyc_List_List*
_temp814; void* _temp816; struct Cyc_List_List* _temp811=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp810= _temp785; if( _temp810 == 0){ _throw(
Null_Exception);} _temp810;}); _temp813=* _temp811; _LL817: _temp816=( void*)
_temp813.hd; goto _LL815; _LL815: _temp814=( struct Cyc_List_List*) _temp813.tl;
goto _LL812; _LL812: { struct _tuple13* wf=({ struct _tuple13* _temp818=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp818->f1= _temp798;
_temp818->f2= _temp806; _temp818->f3= _temp814; _temp818;}); return(( void*(*)(
void* p, void* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp800, _temp808, _temp816, ctx,({ struct Cyc_List_List* _temp819=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp819->hd=( void*) wf; _temp819->tl=
_temp780; _temp819;}), right_hand_side, rules);}}}} _LL760:;} static struct Cyc_List_List*
Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp820=
dsc; struct Cyc_Set_Set* _temp826; struct Cyc_List_List* _temp828; struct Cyc_Tcpat_Con_s*
_temp830; _LL822: if((( struct _enum_struct*) _temp820)->tag == Cyc_Tcpat_Neg_tag){
_LL827: _temp826=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp820)->f1;
goto _LL823;} else{ goto _LL824;} _LL824: if((( struct _enum_struct*) _temp820)->tag
== Cyc_Tcpat_Pos_tag){ _LL831: _temp830=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp820)->f1; goto _LL829; _LL829: _temp828=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp820)->f2; goto _LL825;} else{ goto _LL821;} _LL823: {
void* any=({ struct Cyc_Tcpat_Neg_struct* _temp832=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp832->tag= Cyc_Tcpat_Neg_tag;
_temp832->f1= Cyc_Tcpat_empty_con_set();( void*) _temp832;}); struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ res=({ struct Cyc_List_List*
_temp833=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp833->hd=( void*) any; _temp833->tl= res; _temp833;});}} return res;} _LL825:
return _temp828; _LL821:;} static void* Cyc_Tcpat_getoarg( void* obj, int i){
return({ struct Cyc_Tcpat_Sel_struct* _temp834=( struct Cyc_Tcpat_Sel_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct)); _temp834->tag= Cyc_Tcpat_Sel_tag;
_temp834->f1= i + 1; _temp834->f2=( void*) obj;( void*) _temp834;});} static
struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, void*
obj){ return(( struct Cyc_List_List*(*)( int n, void*(* f)( void*, int), void*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules){
void* _temp835= p; struct Cyc_List_List* _temp841; struct Cyc_Tcpat_Con_s*
_temp843; _LL837: if( _temp835 == Cyc_Tcpat_Any){ goto _LL838;} else{ goto
_LL839;} _LL839: if(( unsigned int) _temp835 > 1u?(( struct _enum_struct*)
_temp835)->tag == Cyc_Tcpat_Con_tag: 0){ _LL844: _temp843=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Con_struct*) _temp835)->f1; goto _LL842; _LL842: _temp841=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*) _temp835)->f2; goto
_LL840;} else{ goto _LL836;} _LL838: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc), work,
right_hand_side, rules); _LL840: { void* _temp845= Cyc_Tcpat_static_match(
_temp843, dsc); _LL847: if( _temp845 == Cyc_Tcpat_Yes){ goto _LL848;} else{ goto
_LL849;} _LL849: if( _temp845 == Cyc_Tcpat_No){ goto _LL850;} else{ goto _LL851;}
_LL851: if( _temp845 == Cyc_Tcpat_Maybe){ goto _LL852;} else{ goto _LL846;}
_LL848: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List* _temp853=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp853->hd=( void*)({
struct _tuple12* _temp854=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp854->f1= _temp843; _temp854->f2= 0; _temp854;}); _temp853->tl= ctx;
_temp853;}); struct _tuple13* work_frame=({ struct _tuple13* _temp855=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp855->f1= _temp841;
_temp855->f2= Cyc_Tcpat_getoargs( _temp843, obj); _temp855->f3= Cyc_Tcpat_getdargs(
_temp843, dsc); _temp855;}); struct Cyc_List_List* work2=({ struct Cyc_List_List*
_temp856=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp856->hd=( void*) work_frame; _temp856->tl= work; _temp856;}); return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(( struct Cyc_List_List*) ctx2,(
struct Cyc_List_List*) work2, right_hand_side, rules);} _LL850: return(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL852: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp857=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp857->hd=( void*)({ struct _tuple12* _temp858=( struct _tuple12*) GC_malloc(
sizeof( struct _tuple12)); _temp858->f1= _temp843; _temp858->f2= 0; _temp858;});
_temp857->tl= ctx; _temp857;}); struct _tuple13* work_frame=({ struct _tuple13*
_temp859=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp859->f1=
_temp841; _temp859->f2= Cyc_Tcpat_getoargs( _temp843, obj); _temp859->f3= Cyc_Tcpat_getdargs(
_temp843, dsc); _temp859;}); struct Cyc_List_List* work2=({ struct Cyc_List_List*
_temp860=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp860->hd=( void*) work_frame; _temp860->tl= work; _temp860;}); void* s=((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(( struct Cyc_List_List*)
ctx2,( struct Cyc_List_List*) work2, right_hand_side, rules); void* f=(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp843), work), rules); return({ struct Cyc_Tcpat_IfEq_struct*
_temp861=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp861->tag= Cyc_Tcpat_IfEq_tag; _temp861->f1=( void*) obj; _temp861->f2=
_temp843; _temp861->f3=( void*) s; _temp861->f4=( void*) f;( void*) _temp861;});}
_LL846:;} _LL836:;} static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(*
not_exhaust)( void*), void* env1, void(* rhs_appears)( void*, void*), void* env2){
void* _temp862= d; void* _temp870; void* _temp872; void* _temp874; struct Cyc_Tcpat_Con_s*
_temp876; void* _temp878; _LL864: if( _temp862 == Cyc_Tcpat_Failure){ goto
_LL865;} else{ goto _LL866;} _LL866: if(( unsigned int) _temp862 > 1u?(( struct
_enum_struct*) _temp862)->tag == Cyc_Tcpat_Success_tag: 0){ _LL871: _temp870=(
void*)(( struct Cyc_Tcpat_Success_struct*) _temp862)->f1; goto _LL867;} else{
goto _LL868;} _LL868: if(( unsigned int) _temp862 > 1u?(( struct _enum_struct*)
_temp862)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL879: _temp878=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp862)->f1; goto _LL877; _LL877: _temp876=( struct Cyc_Tcpat_Con_s*)(( struct
Cyc_Tcpat_IfEq_struct*) _temp862)->f2; goto _LL875; _LL875: _temp874=( void*)((
struct Cyc_Tcpat_IfEq_struct*) _temp862)->f3; goto _LL873; _LL873: _temp872=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp862)->f4; goto _LL869;} else{ goto
_LL863;} _LL865: not_exhaust( env1); goto _LL863; _LL867: rhs_appears( env2,
_temp870); goto _LL863; _LL869:(( void(*)( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp874, not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(*
not_exhaust)( void*), void* env1, void(* rhs_appears)( void*, void*), void* env2))
Cyc_Tcpat_check_exhaust_overlap)( _temp872, not_exhaust, env1, rhs_appears, env2);
goto _LL863; _LL863:;} static struct _tuple15* Cyc_Tcpat_get_match( int* r,
struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple14* rhs=({ struct _tuple14* _temp880=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp880->f1= 0; _temp880->f2=( swc->pattern)->loc;
_temp880;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp881=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp881->hd=( void*) sp0; _temp881->tl=({ struct Cyc_List_List*
_temp882=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp882->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp882->tl= 0; _temp882;});
_temp881;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp883=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp883->hd=( void*) sp0; _temp883->tl=({ struct Cyc_List_List* _temp884=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp884->hd=(
void*) Cyc_Tcpat_Any; _temp884->tl= 0; _temp884;}); _temp883;}));} return({
struct _tuple15* _temp885=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp885->f1= sp; _temp885->f2= rhs; _temp885;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp886=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp887; _temp887.curr= _temp886; _temp887.base= _temp886; _temp887.last_plus_one=
_temp886 + 31; _temp887;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple14* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple15*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,( int*)({
struct _tuple16* _temp888=( struct _tuple16*) GC_malloc_atomic( sizeof( struct
_tuple16)); _temp888->f1= 0; _temp888;}), swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple14*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules= match_rules->tl){ struct _tuple14 _temp891;
struct Cyc_Position_Segment* _temp892; int _temp894; struct _tuple14* _temp889=(*((
struct _tuple15*) match_rules->hd)).f2; _temp891=* _temp889; _LL895: _temp894=
_temp891.f1; goto _LL893; _LL893: _temp892= _temp891.f2; goto _LL890; _LL890:
if( ! _temp894){ Cyc_Tcutil_terr( _temp892,( struct _tagged_string)({ char*
_temp896=( char*)"redundant pattern"; struct _tagged_string _temp897; _temp897.curr=
_temp896; _temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 18;
_temp897;}));}}} static void Cyc_Tcpat_not_exhaust_warn( struct _tuple17* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({ char* _temp898=(
char*)"pattern not exhaustive"; struct _tagged_string _temp899; _temp899.curr=
_temp898; _temp899.base= _temp898; _temp899.last_plus_one= _temp898 + 23;
_temp899;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp900=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp900->hd=( void*)({ struct _tuple18*
_temp901=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp901->f1=
Cyc_Tcpat_compile_pat( p); _temp901->f2= 0; _temp901;}); _temp900->tl= 0;
_temp900;}); void* dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)((
struct Cyc_List_List*) match_rules); struct _tuple17* exhaust_env=({ struct
_tuple17* _temp902=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp902->f1= loc; _temp902->f2= 1; _temp902;});(( void(*)( void* d, void(*
not_exhaust)( struct _tuple17*), struct _tuple17* env1, void(* rhs_appears)( int,
int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_warn,
exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(* exhaust_env).f2;} static struct
_tuple15* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple14* rhs=({ struct _tuple14*
_temp903=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp903->f1=
0; _temp903->f2=( swc->pattern)->loc; _temp903;}); return({ struct _tuple15*
_temp904=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp904->f1=
sp0; _temp904->f2= rhs; _temp904;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple15*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple14*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=
match_rules->tl){ struct _tuple14* pair=(*(( struct _tuple15*) match_rules->hd)).f2;
if( !(* pair).f1){ Cyc_Tcutil_terr((* pair).f2,( struct _tagged_string)({ char*
_temp905=( char*)"redundant pattern"; struct _tagged_string _temp906; _temp906.curr=
_temp905; _temp906.base= _temp905; _temp906.last_plus_one= _temp905 + 18;
_temp906;}));}}}