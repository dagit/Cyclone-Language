#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ void* f1; void*
f2;}; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple5{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct
_tuple6{ void* f1; struct Cyc_List_List* f2;}; struct _tuple7{ struct Cyc_Dict_Dict*
f1; struct Cyc_List_List* f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct
Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int); extern int
Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
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
struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u]; struct
Cyc_List_List_mismatch_struct{ char* tag;}; extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; extern int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_is_empty(
struct Cyc_Set_Set* s); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{
char* tag;}; extern void* Cyc_Set_choose( struct Cyc_Set_Set* s); struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{
char* tag;}; extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; typedef
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
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_compress(
void* t); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
const int Cyc_CfFlowInfo_VarRoot_tag= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; const int Cyc_CfFlowInfo_MallocPt_tag=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
const int Cyc_CfFlowInfo_StructF_tag= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1;}; const int Cyc_CfFlowInfo_TupleF_tag= 1;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1;}; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields;}; void* Cyc_CfFlowInfo_Esc=( void*) 0;
void* Cyc_CfFlowInfo_Unesc=( void*) 1; void* Cyc_CfFlowInfo_NoneIL=( void*) 0;
void* Cyc_CfFlowInfo_ThisIL=( void*) 1; void* Cyc_CfFlowInfo_AllIL=( void*) 2;
const int Cyc_CfFlowInfo_UnknownIS_tag= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2;}; const int Cyc_CfFlowInfo_MustPointTo_tag= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1;}; const int Cyc_CfFlowInfo_LeafPI_tag= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1;}; const int Cyc_CfFlowInfo_TuplePI_tag= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1;}; const int Cyc_CfFlowInfo_StructPI_tag= 2;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1;}; void*
Cyc_CfFlowInfo_BottomFL=( void*) 0; const int Cyc_CfFlowInfo_InitsFL_tag= 0;
struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1;};
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int
Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*);
extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp1={ 0u,( void*) 1u,( void*) 0u}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp0={ 0u,( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp1}; void* Cyc_CfFlowInfo_leaf_unesc_none=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp0; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp3={ 0u,( void*) 1u,( void*) 1u}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp2={ 0u,( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp3}; void* Cyc_CfFlowInfo_leaf_unesc_this=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp2; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp5={ 0u,( void*) 1u,( void*) 2u}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp4={ 0u,( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp5}; void* Cyc_CfFlowInfo_leaf_unesc_all=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp4; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp7={ 0u,( void*) 0u,( void*) 0u}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp6={ 0u,( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp7}; void* Cyc_CfFlowInfo_leaf_esc_none=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp6; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp9={ 0u,( void*) 0u,( void*) 1u}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp8={ 0u,( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp9}; void* Cyc_CfFlowInfo_leaf_esc_this=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp8; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp11={ 0u,( void*) 0u,( void*) 2u}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp10={ 0u,( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp11}; void* Cyc_CfFlowInfo_leaf_esc_all=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp10; void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1=
esc; _temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31;
void* _temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 == Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{ goto _LL17;}
_LL28: _temp27= _temp13.f2; if( _temp27 == Cyc_CfFlowInfo_NoneIL){ goto _LL16;}
else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33 == Cyc_CfFlowInfo_Unesc){
goto _LL32;} else{ goto _LL19;} _LL32: _temp31= _temp13.f2; if( _temp31 == Cyc_CfFlowInfo_ThisIL){
goto _LL18;} else{ goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37
== Cyc_CfFlowInfo_Unesc){ goto _LL36;} else{ goto _LL21;} _LL36: _temp35=
_temp13.f2; if( _temp35 == Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;}
_LL21: _LL42: _temp41= _temp13.f1; if( _temp41 == Cyc_CfFlowInfo_Esc){ goto
_LL40;} else{ goto _LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 == Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
== Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43= _temp13.f2;
if( _temp43 == Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{ goto _LL25;} _LL25:
_LL50: _temp49= _temp13.f1; if( _temp49 == Cyc_CfFlowInfo_Esc){ goto _LL48;}
else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 == Cyc_CfFlowInfo_AllIL){
goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(((
struct _enum_struct*) _temp66)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL69:
_temp68=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(((
struct _enum_struct*) _temp62)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL65:
_temp64=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if((( struct _enum_struct*) _temp74)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL77:
_temp76=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(((
struct _enum_struct*) _temp70)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL73:
_temp72=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if((( struct _enum_struct*) _temp82)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL85:
_temp84=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(((
struct _enum_struct*) _temp78)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL81:
_temp80=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if((( struct _enum_struct*) _temp90)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL93:
_temp92=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(((
struct _enum_struct*) _temp86)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL89:
_temp88=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_string* _temp111; void* _temp113;
struct _tagged_string* _temp115; void* _temp117; int _temp119; void* _temp121;
struct _tagged_string* _temp123; void* _temp125; struct _tagged_string* _temp127;
void* _temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110:
_temp109= _temp95.f1; if((( struct _enum_struct*) _temp109)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL112: _temp111=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp109)->f1; goto _LL106;} else{ goto _LL99;} _LL106: _temp105= _temp95.f2;
if((( struct _enum_struct*) _temp105)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL108:
_temp107=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if((( struct
_enum_struct*) _temp117)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL120: _temp119=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp117)->f1; goto _LL114;} else{
goto _LL101;} _LL114: _temp113= _temp95.f2; if((( struct _enum_struct*) _temp113)->tag
== Cyc_CfFlowInfo_StructF_tag){ _LL116: _temp115=( struct _tagged_string*)((
struct Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1; goto _LL100;} else{ goto
_LL101;} _LL101: _LL126: _temp125= _temp95.f1; if((( struct _enum_struct*)
_temp125)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL128: _temp127=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp125)->f1; goto
_LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2; if((( struct
_enum_struct*) _temp121)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL124: _temp123=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp121)->f1;
goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133= _temp95.f1; if(((
struct _enum_struct*) _temp133)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL136:
_temp135=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1; goto
_LL130;} else{ goto _LL96;} _LL130: _temp129= _temp95.f2; if((( struct
_enum_struct*) _temp129)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL132: _temp131=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto _LL104;} else{
goto _LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return(( int(*)(
struct _tagged_string*, struct _tagged_string*)) Cyc_Core_ptrcmp)( _temp127,
_temp123); _LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void*
_temp137= Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp145; struct Cyc_Absyn_Structdecl**
_temp147; struct Cyc_Absyn_Structdecl* _temp149; struct Cyc_Absyn_Structdecl
_temp150; struct Cyc_Core_Opt* _temp151; struct Cyc_Core_Opt _temp153; struct
Cyc_List_List* _temp154; struct Cyc_List_List* _temp156; struct Cyc_Core_Opt*
_temp158; void* _temp160; struct Cyc_List_List* _temp162; struct _tuple0*
_temp164; _LL139: if(( unsigned int) _temp137 > 5u?(( struct _enum_struct*)
_temp137)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL146: _temp145=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp137)->f1; goto _LL140;} else{ goto
_LL141;} _LL141: if(( unsigned int) _temp137 > 5u?(( struct _enum_struct*)
_temp137)->tag == Cyc_Absyn_StructType_tag: 0){ _LL165: _temp164=( struct
_tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp137)->f1; goto _LL163;
_LL163: _temp162=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f2; goto _LL148; _LL148: _temp147=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp137)->f3; if( _temp147 == 0){ goto
_LL143;} else{ _temp149=* _temp147; _temp150=* _temp149; _LL161: _temp160=( void*)
_temp150.sc; goto _LL159; _LL159: _temp158=( struct Cyc_Core_Opt*) _temp150.name;
goto _LL157; _LL157: _temp156=( struct Cyc_List_List*) _temp150.tvs; goto _LL152;
_LL152: _temp151=( struct Cyc_Core_Opt*) _temp150.fields; if( _temp151 == 0){
goto _LL143;} else{ _temp153=* _temp151; _LL155: _temp154=( struct Cyc_List_List*)
_temp153.v; goto _LL142;}}} else{ goto _LL143;} _LL143: goto _LL144; _LL140: {
struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( int, int)))
Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0; _temp145 != 0; _temp145=
_temp145->tl, ++ i){ struct _tuple4 _temp168; void* _temp169; struct Cyc_Absyn_Tqual*
_temp171; struct _tuple4* _temp166=( struct _tuple4*) _temp145->hd; _temp168=*
_temp166; _LL172: _temp171= _temp168.f1; goto _LL170; _LL170: _temp169= _temp168.f2;
goto _LL167; _LL167: d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int
key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp169));}} return({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp173=( struct
Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp173->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp173->f1= d;( void*) _temp173;});}
_LL142: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp154 != 0; _temp154= _temp154->tl){ struct _tuple5 _temp176; void*
_temp177; struct Cyc_Absyn_Tqual* _temp179; struct _tagged_string* _temp181;
struct _tuple5* _temp174=( struct _tuple5*) _temp154->hd; _temp176=* _temp174;
_LL182: _temp181= _temp176.f1; goto _LL180; _LL180: _temp179= _temp176.f2; goto
_LL178; _LL178: _temp177= _temp176.f3; goto _LL175; _LL175: d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
d, _temp181, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp177));} return({
struct Cyc_CfFlowInfo_StructPI_struct* _temp183=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp183->tag= Cyc_CfFlowInfo_StructPI_tag;
_temp183->f1= d;( void*) _temp183;});} _LL144: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp186; struct Cyc_List_List* _temp187;
void* _temp189; struct Cyc_CfFlowInfo_Place* _temp184= place; _temp186=*
_temp184; _LL190: _temp189=( void*) _temp186.root; goto _LL188; _LL188: _temp187=(
struct Cyc_List_List*) _temp186.fields; goto _LL185; _LL185: { void* ans=(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp189); for( 0;
_temp187 != 0; _temp187= _temp187->tl){ struct _tuple3 _temp192=({ struct
_tuple3 _temp191; _temp191.f1= ans; _temp191.f2=( void*) _temp187->hd; _temp191;});
void* _temp200; int _temp202; void* _temp204; struct Cyc_Dict_Dict* _temp206;
void* _temp208; struct _tagged_string* _temp210; void* _temp212; struct Cyc_Dict_Dict*
_temp214; _LL194: _LL205: _temp204= _temp192.f1; if((( struct _enum_struct*)
_temp204)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL207: _temp206=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp204)->f1; goto _LL201;} else{ goto
_LL196;} _LL201: _temp200= _temp192.f2; if((( struct _enum_struct*) _temp200)->tag
== Cyc_CfFlowInfo_TupleF_tag){ _LL203: _temp202=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp200)->f1; goto _LL195;} else{ goto _LL196;} _LL196: _LL213: _temp212=
_temp192.f1; if((( struct _enum_struct*) _temp212)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL215: _temp214=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp212)->f1; goto _LL209;} else{ goto _LL198;} _LL209: _temp208= _temp192.f2;
if((( struct _enum_struct*) _temp208)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL211: _temp210=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp208)->f1; goto _LL197;} else{ goto _LL198;} _LL198: goto _LL199; _LL195:
ans=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp206,
_temp202); goto _LL193; _LL197: ans=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp214, _temp210); goto _LL193; _LL199:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp216=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp216=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp217=( char*)"bad lookup_place";
struct _tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 17; _temp218;})};( struct _xenum_struct*)
_temp216;})); _LL193:;} return ans;}} void* Cyc_CfFlowInfo_insert_place_rec(
void* insert_path, struct Cyc_List_List* fields, void* pinfo){ if( fields == 0){
return pinfo;}{ struct _tuple6 _temp220=({ struct _tuple6 _temp219; _temp219.f1=
insert_path; _temp219.f2= fields; _temp219;}); struct Cyc_List_List* _temp228;
struct Cyc_List_List _temp230; struct Cyc_List_List* _temp231; void* _temp233;
int _temp235; void* _temp237; struct Cyc_Dict_Dict* _temp239; struct Cyc_List_List*
_temp241; struct Cyc_List_List _temp243; struct Cyc_List_List* _temp244; void*
_temp246; struct _tagged_string* _temp248; void* _temp250; struct Cyc_Dict_Dict*
_temp252; _LL222: _LL238: _temp237= _temp220.f1; if((( struct _enum_struct*)
_temp237)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL240: _temp239=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp237)->f1; goto _LL229;} else{ goto
_LL224;} _LL229: _temp228= _temp220.f2; if( _temp228 == 0){ goto _LL224;} else{
_temp230=* _temp228; _LL234: _temp233=( void*) _temp230.hd; if((( struct
_enum_struct*) _temp233)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL236: _temp235=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp233)->f1; goto _LL232;} else{
goto _LL224;} _LL232: _temp231=( struct Cyc_List_List*) _temp230.tl; goto _LL223;}
_LL224: _LL251: _temp250= _temp220.f1; if((( struct _enum_struct*) _temp250)->tag
== Cyc_CfFlowInfo_StructPI_tag){ _LL253: _temp252=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp250)->f1; goto _LL242;} else{ goto
_LL226;} _LL242: _temp241= _temp220.f2; if( _temp241 == 0){ goto _LL226;} else{
_temp243=* _temp241; _LL247: _temp246=( void*) _temp243.hd; if((( struct
_enum_struct*) _temp246)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL249: _temp248=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp246)->f1;
goto _LL245;} else{ goto _LL226;} _LL245: _temp244=( struct Cyc_List_List*)
_temp243.tl; goto _LL225;} _LL226: goto _LL227; _LL223: return({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp254=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp254->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp254->f1=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( _temp239,
_temp235, Cyc_CfFlowInfo_insert_place_rec((( void*(*)( struct Cyc_Dict_Dict* d,
int key)) Cyc_Dict_lookup)( _temp239, _temp235), _temp231, pinfo));( void*)
_temp254;}); _LL225: return({ struct Cyc_CfFlowInfo_StructPI_struct* _temp255=(
struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp255->tag= Cyc_CfFlowInfo_StructPI_tag; _temp255->f1=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
_temp252, _temp248, Cyc_CfFlowInfo_insert_place_rec((( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp252, _temp248), _temp244,
pinfo));( void*) _temp255;}); _LL227:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp256=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp256=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp257=( char*)"bad insert_place"; struct
_tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 17; _temp258;})};( struct _xenum_struct*)
_temp256;})); _LL221:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){
struct Cyc_CfFlowInfo_Place _temp261; struct Cyc_List_List* _temp262; void*
_temp264; struct Cyc_CfFlowInfo_Place* _temp259= place; _temp261=* _temp259;
_LL265: _temp264=( void*) _temp261.root; goto _LL263; _LL263: _temp262=( struct
Cyc_List_List*) _temp261.fields; goto _LL260; _LL260: return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d, _temp264,
Cyc_CfFlowInfo_insert_place_rec((( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( d, _temp264), _temp262, pinfo));} int Cyc_CfFlowInfo_isAllInit_pinfo(
struct _tuple7* env, void* path); int Cyc_CfFlowInfo_isAllInit_is( struct
_tuple7* env, void* state){ void* _temp266= state; void* _temp274; void*
_temp276; void* _temp278; void* _temp280; struct Cyc_CfFlowInfo_Place* _temp282;
_LL268: if((( struct _enum_struct*) _temp266)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL277: _temp276=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp266)->f1;
goto _LL275; _LL275: _temp274=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp266)->f2; if( _temp274 == Cyc_CfFlowInfo_AllIL){ goto _LL269;} else{ goto
_LL270;}} else{ goto _LL270;} _LL270: if((( struct _enum_struct*) _temp266)->tag
== Cyc_CfFlowInfo_UnknownIS_tag){ _LL281: _temp280=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp266)->f1; goto _LL279; _LL279: _temp278=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp266)->f2; goto _LL271;} else{ goto _LL272;} _LL272: if((( struct
_enum_struct*) _temp266)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL283:
_temp282=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp266)->f1; goto _LL273;} else{ goto _LL267;} _LL269: return 1; _LL271:
return 0; _LL273: { struct _tuple7 _temp286; struct Cyc_List_List* _temp287;
struct Cyc_List_List** _temp289; struct Cyc_Dict_Dict* _temp290; struct _tuple7*
_temp284= env; _temp286=* _temp284; _LL291: _temp290= _temp286.f1; goto _LL288;
_LL288: _temp287= _temp286.f2; _temp289=&(* _temp284).f2; goto _LL285; _LL285:
if((( int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp289, _temp282)){ return 1;}* _temp289=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp292=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp292[ 0]=({ struct Cyc_List_List _temp293; _temp293.hd=( void*) _temp282;
_temp293.tl=* _temp289; _temp293;}); _temp292;}); return Cyc_CfFlowInfo_isAllInit_pinfo(
env, Cyc_CfFlowInfo_lookup_place( _temp290, _temp282));} _LL267:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple7* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple7* env, void* path){
void* _temp294= path; void* _temp302; struct Cyc_Dict_Dict* _temp304; struct Cyc_Dict_Dict*
_temp306; _LL296: if((( struct _enum_struct*) _temp294)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL303: _temp302=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp294)->f1;
goto _LL297;} else{ goto _LL298;} _LL298: if((( struct _enum_struct*) _temp294)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL305: _temp304=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp294)->f1; goto _LL299;} else{ goto
_LL300;} _LL300: if((( struct _enum_struct*) _temp294)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL307: _temp306=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp294)->f1; goto _LL301;} else{ goto _LL295;} _LL297: return Cyc_CfFlowInfo_isAllInit_is(
env, _temp302); _LL299: return(( int(*)( int(* f)( struct _tuple7*, int, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple7* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp304); _LL301: return(( int(*)( int(* f)( struct _tuple7*, struct
_tagged_string*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple7* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp306); _LL295:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple7 isInitEnv=({ struct _tuple7 _temp308;
_temp308.f1= pinfo_dict; _temp308.f2= 0; _temp308;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, pinfo);} void* Cyc_CfFlowInfo_pinfo_getplaces_esc( struct Cyc_Set_Set**
places_ptr, void* pinfo){ void* _temp309= pinfo; void* _temp321; struct Cyc_CfFlowInfo_Place*
_temp323; void* _temp325; void* _temp327; void* _temp329; struct Cyc_Dict_Dict*
_temp331; struct Cyc_Dict_Dict* _temp333; _LL311: if((( struct _enum_struct*)
_temp309)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL322: _temp321=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp309)->f1; if((( struct _enum_struct*)
_temp321)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL324: _temp323=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp321)->f1; goto _LL312;} else{
goto _LL313;}} else{ goto _LL313;} _LL313: if((( struct _enum_struct*) _temp309)->tag
== Cyc_CfFlowInfo_LeafPI_tag){ _LL326: _temp325=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp309)->f1; if((( struct _enum_struct*) _temp325)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL330: _temp329=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp325)->f1;
goto _LL328; _LL328: _temp327=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp325)->f2; if( _temp327 == Cyc_CfFlowInfo_AllIL){ goto _LL314;} else{ goto
_LL315;}} else{ goto _LL315;}} else{ goto _LL315;} _LL315: if((( struct
_enum_struct*) _temp309)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL332: _temp331=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp309)->f1;
goto _LL316;} else{ goto _LL317;} _LL317: if((( struct _enum_struct*) _temp309)->tag
== Cyc_CfFlowInfo_StructPI_tag){ _LL334: _temp333=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp309)->f1; goto _LL318;} else{ goto
_LL319;} _LL319: goto _LL320; _LL312:* places_ptr=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
places_ptr, _temp323); goto _LL314; _LL314: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,
Cyc_CfFlowInfo_AllIL); _LL316: return({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp335=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp335->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp335->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct Cyc_Set_Set**, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc, places_ptr, _temp331);(
void*) _temp335;}); _LL318: return({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp336=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp336->tag= Cyc_CfFlowInfo_StructPI_tag; _temp336->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct Cyc_Set_Set**, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc, places_ptr, _temp333);(
void*) _temp336;}); _LL320:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp337=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp337=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp338=( char*)"bad arg to escape"; struct
_tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 18; _temp339;})};( struct _xenum_struct*)
_temp337;})); _LL310:;} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set**
places_ptr, void* a, void* pinfo){ void* _temp340= pinfo; void* _temp350; void*
_temp352; void* _temp354; void* _temp356; struct Cyc_CfFlowInfo_Place* _temp358;
struct Cyc_Dict_Dict* _temp360; struct Cyc_Dict_Dict* _temp362; _LL342: if(((
struct _enum_struct*) _temp340)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL351:
_temp350=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp340)->f1; if(((
struct _enum_struct*) _temp350)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL355:
_temp354=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp350)->f1; goto
_LL353; _LL353: _temp352=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp350)->f2; goto _LL343;} else{ goto _LL344;}} else{ goto _LL344;} _LL344:
if((( struct _enum_struct*) _temp340)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL357:
_temp356=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp340)->f1; if(((
struct _enum_struct*) _temp356)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL359:
_temp358=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp356)->f1; goto _LL345;} else{ goto _LL346;}} else{ goto _LL346;} _LL346:
if((( struct _enum_struct*) _temp340)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL361: _temp360=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp340)->f1; goto _LL347;} else{ goto _LL348;} _LL348: if((( struct
_enum_struct*) _temp340)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL363: _temp362=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp340)->f1;
goto _LL349;} else{ goto _LL341;} _LL343: return; _LL345:* places_ptr=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
places_ptr, _temp358); return; _LL347:(( void(*)( void(* f)( struct Cyc_Set_Set**,
int, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp360); return; _LL349:(( void(*)( void(* f)( struct Cyc_Set_Set**,
struct _tagged_string*, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, struct
_tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr,
_temp362); return; _LL341:;} static struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr=
0; struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict*
d){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=(
struct Cyc_Set_Set**)({ struct Cyc_Set_Set** _temp364=( struct Cyc_Set_Set**)
GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp364[ 0]=(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)(
Cyc_CfFlowInfo_place_cmp); _temp364;});}{ struct Cyc_Set_Set* places=* Cyc_CfFlowInfo_empty_place_set_ptr;((
void(*)( struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
places, 0, pinfo); while( !(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_is_empty)(
places)) { struct Cyc_CfFlowInfo_Place* place=(( struct Cyc_CfFlowInfo_Place*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_choose)( places); places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_delete)(
places, place);{ void* place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, place);
void* place_new_pinfo= Cyc_CfFlowInfo_pinfo_getplaces_esc(& places,
place_old_pinfo); if( place_old_pinfo != place_new_pinfo){ d= Cyc_CfFlowInfo_insert_place(
d, place, place_new_pinfo);}}} return d;}} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp365= pinfo; void* _temp375; void* _temp377;
void* _temp379; void* _temp381; struct Cyc_Dict_Dict* _temp383; struct Cyc_Dict_Dict*
_temp385; _LL367: if((( struct _enum_struct*) _temp365)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL376: _temp375=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp365)->f1;
if((( struct _enum_struct*) _temp375)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL380: _temp379=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp375)->f1;
if( _temp379 == Cyc_CfFlowInfo_Esc){ goto _LL378;} else{ goto _LL369;} _LL378:
_temp377=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp375)->f2; goto
_LL368;} else{ goto _LL369;}} else{ goto _LL369;} _LL369: if((( struct
_enum_struct*) _temp365)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL382: _temp381=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp365)->f1; goto _LL370;}
else{ goto _LL371;} _LL371: if((( struct _enum_struct*) _temp365)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL384: _temp383=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp365)->f1; goto _LL372;} else{ goto _LL373;} _LL373: if((( struct
_enum_struct*) _temp365)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL386: _temp385=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp365)->f1;
goto _LL374;} else{ goto _LL366;} _LL368: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,
level); _LL370: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc, level);
_LL372: return({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp387=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp387->tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp387->f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp383);( void*) _temp387;}); _LL374: return({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp388=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp388->tag= Cyc_CfFlowInfo_StructPI_tag; _temp388->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_assign_unknown_dict, level, _temp385);( void*) _temp388;});
_LL366:;} struct Cyc_CfFlowInfo_JoinEnv{ struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict*
d2;}; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t; void*
Cyc_CfFlowInfo_join_escape( void* esc1, void* esc2){ struct _tuple3 _temp390=({
struct _tuple3 _temp389; _temp389.f1= esc1; _temp389.f2= esc2; _temp389;}); void*
_temp398; void* _temp400; void* _temp402; void* _temp404; _LL392: _LL401:
_temp400= _temp390.f1; if( _temp400 == Cyc_CfFlowInfo_Esc){ goto _LL399;} else{
goto _LL394;} _LL399: _temp398= _temp390.f2; goto _LL393; _LL394: _LL405:
_temp404= _temp390.f1; goto _LL403; _LL403: _temp402= _temp390.f2; if( _temp402
== Cyc_CfFlowInfo_Esc){ goto _LL395;} else{ goto _LL396;} _LL396: goto _LL397;
_LL393: goto _LL395; _LL395: return Cyc_CfFlowInfo_Esc; _LL397: return Cyc_CfFlowInfo_Unesc;
_LL391:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp407=({ struct _tuple3 _temp406; _temp406.f1= il1; _temp406.f2= il2;
_temp406;}); void* _temp419; void* _temp421; void* _temp423; void* _temp425;
void* _temp427; void* _temp429; void* _temp431; void* _temp433; _LL409: _LL422:
_temp421= _temp407.f1; if( _temp421 == Cyc_CfFlowInfo_NoneIL){ goto _LL420;}
else{ goto _LL411;} _LL420: _temp419= _temp407.f2; goto _LL410; _LL411: _LL426:
_temp425= _temp407.f1; goto _LL424; _LL424: _temp423= _temp407.f2; if( _temp423
== Cyc_CfFlowInfo_NoneIL){ goto _LL412;} else{ goto _LL413;} _LL413: _LL430:
_temp429= _temp407.f1; if( _temp429 == Cyc_CfFlowInfo_ThisIL){ goto _LL428;}
else{ goto _LL415;} _LL428: _temp427= _temp407.f2; goto _LL414; _LL415: _LL434:
_temp433= _temp407.f1; goto _LL432; _LL432: _temp431= _temp407.f2; if( _temp431
== Cyc_CfFlowInfo_ThisIL){ goto _LL416;} else{ goto _LL417;} _LL417: goto _LL418;
_LL410: goto _LL412; _LL412: return Cyc_CfFlowInfo_NoneIL; _LL414: goto _LL416;
_LL416: return Cyc_CfFlowInfo_ThisIL; _LL418: return Cyc_CfFlowInfo_AllIL;
_LL408:;} void* Cyc_CfFlowInfo_join_initstate( struct Cyc_CfFlowInfo_JoinEnv*
env, void* state1, void* state2){ struct _tuple3 _temp436=({ struct _tuple3
_temp435; _temp435.f1= state1; _temp435.f2= state2; _temp435;}); void* _temp446;
void* _temp448; void* _temp450; void* _temp452; void* _temp454; void* _temp456;
void* _temp458; struct Cyc_CfFlowInfo_Place* _temp460; void* _temp462; void*
_temp464; void* _temp466; void* _temp468; void* _temp470; void* _temp472; void*
_temp474; struct Cyc_CfFlowInfo_Place* _temp476; void* _temp478; struct Cyc_CfFlowInfo_Place*
_temp480; void* _temp482; struct Cyc_CfFlowInfo_Place* _temp484; _LL438: _LL453:
_temp452= _temp436.f1; if((( struct _enum_struct*) _temp452)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL457: _temp456=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp452)->f1;
goto _LL455; _LL455: _temp454=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp452)->f2; goto _LL447;} else{ goto _LL440;} _LL447: _temp446= _temp436.f2;
if((( struct _enum_struct*) _temp446)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL451: _temp450=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp446)->f1;
goto _LL449; _LL449: _temp448=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp446)->f2; goto _LL439;} else{ goto _LL440;} _LL440: _LL463: _temp462=
_temp436.f1; if((( struct _enum_struct*) _temp462)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL467: _temp466=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp462)->f1;
goto _LL465; _LL465: _temp464=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp462)->f2; goto _LL459;} else{ goto _LL442;} _LL459: _temp458= _temp436.f2;
if((( struct _enum_struct*) _temp458)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL461: _temp460=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp458)->f1; goto _LL441;} else{ goto _LL442;} _LL442: _LL475: _temp474=
_temp436.f1; if((( struct _enum_struct*) _temp474)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL477: _temp476=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp474)->f1; goto _LL469;} else{ goto _LL444;} _LL469: _temp468= _temp436.f2;
if((( struct _enum_struct*) _temp468)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL473: _temp472=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp468)->f1;
goto _LL471; _LL471: _temp470=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp468)->f2; goto _LL443;} else{ goto _LL444;} _LL444: _LL483: _temp482=
_temp436.f1; if((( struct _enum_struct*) _temp482)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL485: _temp484=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp482)->f1; goto _LL479;} else{ goto _LL437;} _LL479: _temp478= _temp436.f2;
if((( struct _enum_struct*) _temp478)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL481: _temp480=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp478)->f1; goto _LL445;} else{ goto _LL437;} _LL439: return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_join_escape( _temp456, _temp450), Cyc_CfFlowInfo_join_initlevel(
_temp454, _temp448)); _LL441: { struct _tuple7 isInitEnv=({ struct _tuple7
_temp486; _temp486.f1= env->d2; _temp486.f2= 0; _temp486;}); if( _temp464 == Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d2,
_temp460)): 0){ env->d2= Cyc_CfFlowInfo_escape_pointsto( Cyc_CfFlowInfo_lookup_place(
env->d2, _temp460), env->d2); return Cyc_CfFlowInfo_mkLeafPI( _temp466, _temp464);}
return Cyc_CfFlowInfo_mkLeafPI( _temp466, Cyc_CfFlowInfo_join_initlevel(
_temp464, Cyc_CfFlowInfo_ThisIL));} _LL443: { struct _tuple7 isInitEnv=({ struct
_tuple7 _temp487; _temp487.f1= env->d1; _temp487.f2= 0; _temp487;}); if(
_temp470 == Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(& isInitEnv,
Cyc_CfFlowInfo_lookup_place( env->d1, _temp476)): 0){ env->d1= Cyc_CfFlowInfo_escape_pointsto(
Cyc_CfFlowInfo_lookup_place( env->d1, _temp476), env->d1); return Cyc_CfFlowInfo_mkLeafPI(
_temp472, _temp470);} return Cyc_CfFlowInfo_mkLeafPI( _temp472, Cyc_CfFlowInfo_join_initlevel(
Cyc_CfFlowInfo_ThisIL, _temp470));} _LL445: if( Cyc_CfFlowInfo_place_cmp(
_temp484, _temp480) == 0){ return({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp488=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp488->tag= Cyc_CfFlowInfo_LeafPI_tag; _temp488->f1=( void*) state1;( void*)
_temp488;});}{ struct _tuple7 isInitEnv1=({ struct _tuple7 _temp489; _temp489.f1=
env->d1; _temp489.f2= 0; _temp489;}); struct _tuple7 isInitEnv2=({ struct
_tuple7 _temp490; _temp490.f1= env->d2; _temp490.f2= 0; _temp490;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv1, Cyc_CfFlowInfo_lookup_place( env->d1, _temp484))? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv2, Cyc_CfFlowInfo_lookup_place( env->d2, _temp480)): 0){ env->d1= Cyc_CfFlowInfo_escape_pointsto(
Cyc_CfFlowInfo_lookup_place( env->d1, _temp484), env->d1); env->d2= Cyc_CfFlowInfo_escape_pointsto(
Cyc_CfFlowInfo_lookup_place( env->d2, _temp480), env->d2); return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_Unesc, Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_Unesc, Cyc_CfFlowInfo_ThisIL);} _LL437:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp492=({ struct _tuple3 _temp491;
_temp491.f1= pinfo1; _temp491.f2= pinfo2; _temp491;}); void* _temp502; void*
_temp504; void* _temp506; void* _temp508; void* _temp510; struct Cyc_Dict_Dict*
_temp512; void* _temp514; struct Cyc_Dict_Dict* _temp516; void* _temp518; struct
Cyc_Dict_Dict* _temp520; void* _temp522; struct Cyc_Dict_Dict* _temp524; _LL494:
_LL507: _temp506= _temp492.f1; if((( struct _enum_struct*) _temp506)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL509: _temp508=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp506)->f1;
goto _LL503;} else{ goto _LL496;} _LL503: _temp502= _temp492.f2; if((( struct
_enum_struct*) _temp502)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL505: _temp504=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp502)->f1; goto _LL495;}
else{ goto _LL496;} _LL496: _LL515: _temp514= _temp492.f1; if((( struct
_enum_struct*) _temp514)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL517: _temp516=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp514)->f1;
goto _LL511;} else{ goto _LL498;} _LL511: _temp510= _temp492.f2; if((( struct
_enum_struct*) _temp510)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL513: _temp512=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp510)->f1;
goto _LL497;} else{ goto _LL498;} _LL498: _LL523: _temp522= _temp492.f1; if(((
struct _enum_struct*) _temp522)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL525:
_temp524=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp522)->f1; goto _LL519;} else{ goto _LL500;} _LL519: _temp518= _temp492.f2;
if((( struct _enum_struct*) _temp518)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL521: _temp520=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp518)->f1; goto _LL499;} else{ goto _LL500;} _LL500: goto _LL501; _LL495:
if( _temp508 == _temp504){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp508, _temp504); _LL497: if( _temp516 == _temp512){ return pinfo1;}{
struct Cyc_Dict_Dict* dict_ans=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp516, _temp512); if( _temp516 == dict_ans){ return pinfo1;} if(
_temp512 == dict_ans){ return pinfo2;} return({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp526=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp526->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp526->f1= dict_ans;( void*)
_temp526;});} _LL499: if( _temp524 == _temp520){ return pinfo1;}{ struct Cyc_Dict_Dict*
dict_ans=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp524, _temp520); if( _temp524 == dict_ans){ return pinfo1;} if(
_temp520 == dict_ans){ return pinfo2;} return({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp527=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp527->tag= Cyc_CfFlowInfo_StructPI_tag; _temp527->f1= dict_ans;( void*)
_temp527;});} _LL501:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp528=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp528=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp529=( char*)"join_pathinfo"; struct
_tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 14; _temp530;})};( struct _xenum_struct*)
_temp528;})); _LL493:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){
if( f1 == f2){ return f1;}{ struct _tuple3 _temp532=({ struct _tuple3 _temp531;
_temp531.f1= f1; _temp531.f2= f2; _temp531;}); void* _temp540; void* _temp542;
void* _temp544; void* _temp546; void* _temp548; struct Cyc_Dict_Dict* _temp550;
void* _temp552; struct Cyc_Dict_Dict* _temp554; _LL534: _LL543: _temp542=
_temp532.f1; if( _temp542 == Cyc_CfFlowInfo_BottomFL){ goto _LL541;} else{ goto
_LL536;} _LL541: _temp540= _temp532.f2; goto _LL535; _LL536: _LL547: _temp546=
_temp532.f1; goto _LL545; _LL545: _temp544= _temp532.f2; if( _temp544 == Cyc_CfFlowInfo_BottomFL){
goto _LL537;} else{ goto _LL538;} _LL538: _LL553: _temp552= _temp532.f1; if((
unsigned int) _temp552 > 1u?(( struct _enum_struct*) _temp552)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL555: _temp554=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp552)->f1; goto _LL549;} else{ goto _LL533;} _LL549: _temp548= _temp532.f2;
if(( unsigned int) _temp548 > 1u?(( struct _enum_struct*) _temp548)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL551: _temp550=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp548)->f1; goto _LL539;} else{ goto _LL533;} _LL535: return f2; _LL537:
return f1; _LL539: if( _temp554 == _temp550){ return f1;}{ struct Cyc_CfFlowInfo_JoinEnv
joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp556; _temp556.d1= _temp554;
_temp556.d2= _temp550; _temp556;}); return({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp557=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp557->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp557->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo,& joinenv, _temp554, _temp550);( void*) _temp557;});}
_LL533:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void* state1, void*
state2){ struct _tuple3 _temp559=({ struct _tuple3 _temp558; _temp558.f1= state1;
_temp558.f2= state2; _temp558;}); void* _temp567; void* _temp569; void* _temp571;
void* _temp573; void* _temp575; void* _temp577; void* _temp579; struct Cyc_CfFlowInfo_Place*
_temp581; void* _temp583; struct Cyc_CfFlowInfo_Place* _temp585; _LL561: _LL574:
_temp573= _temp559.f1; if((( struct _enum_struct*) _temp573)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL578: _temp577=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp573)->f1;
goto _LL576; _LL576: _temp575=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp573)->f2; goto _LL568;} else{ goto _LL563;} _LL568: _temp567= _temp559.f2;
if((( struct _enum_struct*) _temp567)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL572: _temp571=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp567)->f1;
goto _LL570; _LL570: _temp569=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp567)->f2; goto _LL562;} else{ goto _LL563;} _LL563: _LL584: _temp583=
_temp559.f1; if((( struct _enum_struct*) _temp583)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL586: _temp585=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp583)->f1; goto _LL580;} else{ goto _LL565;} _LL580: _temp579= _temp559.f2;
if((( struct _enum_struct*) _temp579)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL582: _temp581=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp579)->f1; goto _LL564;} else{ goto _LL565;} _LL565: goto _LL566; _LL562:
return Cyc_CfFlowInfo_join_escape( _temp577, _temp571) == _temp571? Cyc_CfFlowInfo_join_initlevel(
_temp575, _temp569) == _temp569: 0; _LL564: return Cyc_CfFlowInfo_place_cmp(
_temp585, _temp581) == 0; _LL566: return 0; _LL560:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp588=({ struct _tuple3 _temp587; _temp587.f1= pinfo1;
_temp587.f2= pinfo2; _temp587;}); void* _temp598; void* _temp600; void* _temp602;
void* _temp604; void* _temp606; struct Cyc_Dict_Dict* _temp608; void* _temp610;
struct Cyc_Dict_Dict* _temp612; void* _temp614; struct Cyc_Dict_Dict* _temp616;
void* _temp618; struct Cyc_Dict_Dict* _temp620; _LL590: _LL603: _temp602=
_temp588.f1; if((( struct _enum_struct*) _temp602)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL605: _temp604=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp602)->f1;
goto _LL599;} else{ goto _LL592;} _LL599: _temp598= _temp588.f2; if((( struct
_enum_struct*) _temp598)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL601: _temp600=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp598)->f1; goto _LL591;}
else{ goto _LL592;} _LL592: _LL611: _temp610= _temp588.f1; if((( struct
_enum_struct*) _temp610)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL613: _temp612=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp610)->f1;
goto _LL607;} else{ goto _LL594;} _LL607: _temp606= _temp588.f2; if((( struct
_enum_struct*) _temp606)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL609: _temp608=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp606)->f1;
goto _LL593;} else{ goto _LL594;} _LL594: _LL619: _temp618= _temp588.f1; if(((
struct _enum_struct*) _temp618)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL621:
_temp620=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp618)->f1; goto _LL615;} else{ goto _LL596;} _LL615: _temp614= _temp588.f2;
if((( struct _enum_struct*) _temp614)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL617: _temp616=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp614)->f1; goto _LL595;} else{ goto _LL596;} _LL596: goto _LL597; _LL591:
return _temp604 == _temp600? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp604, _temp600); _LL593: return _temp612 == _temp608? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp612, _temp608); _LL595: return _temp620 == _temp616? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp620, _temp616);
_LL597:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp622=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp622=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp623=( char*)"pinfo_lessthan"; struct
_tagged_string _temp624; _temp624.curr= _temp623; _temp624.base= _temp623;
_temp624.last_plus_one= _temp623 + 15; _temp624;})};( struct _xenum_struct*)
_temp622;})); _LL589:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2){ if( f1 == f2){ return 1;}{ struct _tuple3 _temp626=({ struct _tuple3
_temp625; _temp625.f1= f1; _temp625.f2= f2; _temp625;}); void* _temp634; void*
_temp636; void* _temp638; void* _temp640; void* _temp642; struct Cyc_Dict_Dict*
_temp644; void* _temp646; struct Cyc_Dict_Dict* _temp648; _LL628: _LL637:
_temp636= _temp626.f1; if( _temp636 == Cyc_CfFlowInfo_BottomFL){ goto _LL635;}
else{ goto _LL630;} _LL635: _temp634= _temp626.f2; goto _LL629; _LL630: _LL641:
_temp640= _temp626.f1; goto _LL639; _LL639: _temp638= _temp626.f2; if( _temp638
== Cyc_CfFlowInfo_BottomFL){ goto _LL631;} else{ goto _LL632;} _LL632: _LL647:
_temp646= _temp626.f1; if(( unsigned int) _temp646 > 1u?(( struct _enum_struct*)
_temp646)->tag == Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL649: _temp648=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp646)->f1; goto _LL643;} else{ goto
_LL627;} _LL643: _temp642= _temp626.f2; if(( unsigned int) _temp642 > 1u?((
struct _enum_struct*) _temp642)->tag == Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL645:
_temp644=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp642)->f1; goto _LL633;} else{ goto _LL627;} _LL629: return 1; _LL631:
return 0; _LL633: if( _temp648 == _temp644){ return 1;} return(( int(*)( int(* f)(
void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp644, _temp648); _LL627:;}}