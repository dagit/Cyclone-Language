#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple4{ void*
f1; void* f2;}; struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int); extern int Cyc_Core_ptrcmp(
void**, void**); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
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
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char*
tag;}; struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2); extern struct Cyc_Set_Set* Cyc_Set_intersect( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set*
s, void* elt); extern int Cyc_Set_is_empty( struct Cyc_Set_Set* s); extern int
Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_subset(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern char Cyc_Set_Absent_tag[
7u]; struct Cyc_Set_Absent_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_intersect( void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); struct Cyc_Stdio___sFILE; typedef struct
Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t; typedef
int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t;
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
Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag;}; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern void Cyc_ControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); extern void
Cyc_ControlFlow_cf_check( struct Cyc_List_List*); extern struct Cyc_Absyn_Stmt*
Cyc_ControlFlow_get_encloser( struct Cyc_Absyn_Stmt* enclosee); typedef struct
Cyc_Absyn_Vardecl* Cyc_ControlFlow_vardecl_t; typedef struct _tagged_string* Cyc_ControlFlow_field_name_t;
typedef struct Cyc_Set_Set* Cyc_ControlFlow_set_t; typedef struct Cyc_Dict_Dict*
Cyc_ControlFlow_dict_t; struct Cyc_ControlFlow_FlowInfo{ struct Cyc_Set_Set*
simples; struct Cyc_Dict_Dict* tuples; struct Cyc_Dict_Dict* structs;}; typedef
struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_flow_info_t; static struct Cyc_Absyn_Tqual
Cyc_ControlFlow_bogus_tqual=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict=
0}; static char _temp2[ 1u]=""; static struct _tagged_string Cyc_ControlFlow_empty_string=(
struct _tagged_string){ _temp2, _temp2, _temp2 + 1u}; static struct Cyc_Absyn_Abs_n_struct
_temp3={ 1u, 0}; static struct _tuple0 Cyc_ControlFlow_bogus_qvar=( struct
_tuple0){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp3,.f2=& Cyc_ControlFlow_empty_string};
static struct Cyc_Absyn_Vardecl Cyc_ControlFlow_bogus_vardecl_v=( struct Cyc_Absyn_Vardecl){.sc=(
void*)(( void*) 2u),.name=& Cyc_ControlFlow_bogus_qvar,.tq=& Cyc_ControlFlow_bogus_tqual,.type=(
void*)(( void*) 0u),.initializer= 0,.shadow= 0,.region= 0}; static struct Cyc_Absyn_Vardecl*
Cyc_ControlFlow_bogus_vardecl=& Cyc_ControlFlow_bogus_vardecl_v; static struct
Cyc_ControlFlow_FlowInfo* Cyc_ControlFlow_empty_flow_info= 0; static struct Cyc_ControlFlow_FlowInfo
Cyc_ControlFlow_empty_flow(){ if( Cyc_ControlFlow_empty_flow_info == 0){ Cyc_ControlFlow_empty_flow_info=({
struct Cyc_ControlFlow_FlowInfo* _temp4=( struct Cyc_ControlFlow_FlowInfo*)
GC_malloc( sizeof( struct Cyc_ControlFlow_FlowInfo)); _temp4->simples=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*)))
Cyc_Set_empty)(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp); _temp4->tuples=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))) Cyc_Dict_empty)(( int(*)(
struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*)) Cyc_Core_ptrcmp); _temp4->structs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*)))
Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp); _temp4;});} return* Cyc_ControlFlow_empty_flow_info;} static
struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_merge_flow( struct Cyc_ControlFlow_FlowInfo
f1, struct Cyc_ControlFlow_FlowInfo f2){ return({ struct Cyc_ControlFlow_FlowInfo
_temp5; _temp5.simples=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct
Cyc_Set_Set* s2)) Cyc_Set_union_two)( f1.simples, f2.simples); _temp5.tuples=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Set_Set*(* f)( struct Cyc_Set_Set*, struct
Cyc_Set_Set*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two)((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two,
f1.tuples, f2.tuples); _temp5.structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Set_Set*(*
f)( struct Cyc_Set_Set*, struct Cyc_Set_Set*), struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2)) Cyc_Dict_union_two)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two, f1.structs, f2.structs); _temp5;});}
static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_meet_flow( struct Cyc_ControlFlow_FlowInfo
f1, struct Cyc_ControlFlow_FlowInfo f2){ return({ struct Cyc_ControlFlow_FlowInfo
_temp6; _temp6.simples=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct
Cyc_Set_Set* s2)) Cyc_Set_intersect)( f1.simples, f2.simples); _temp6.tuples=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Set_Set*(* f)( struct Cyc_Set_Set*, struct
Cyc_Set_Set*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect)((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_intersect,
f1.tuples, f2.tuples); _temp6.structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Set_Set*(*
f)( struct Cyc_Set_Set*, struct Cyc_Set_Set*), struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2)) Cyc_Dict_intersect)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_intersect, f1.structs, f2.structs); _temp6;});}
static int Cyc_ControlFlow_dict_less_than_f( struct Cyc_Dict_Dict* d2, void* a,
struct Cyc_Set_Set* s){ return !(( int(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_member)( d2, a)? 1:(( int(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_subset)( s,(( struct Cyc_Set_Set*(*)( struct Cyc_Dict_Dict* d, void*
key)) Cyc_Dict_lookup)( d2, a));} static int Cyc_ControlFlow_less_than_flow(
struct Cyc_ControlFlow_FlowInfo f1, struct Cyc_ControlFlow_FlowInfo f2){ return(((
int(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_subset)( f1.simples,
f2.simples)?(( int(*)( int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Vardecl*,
struct Cyc_Set_Set*), struct Cyc_Dict_Dict* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct Cyc_Dict_Dict* d2, struct Cyc_Absyn_Vardecl* a, struct Cyc_Set_Set*
s)) Cyc_ControlFlow_dict_less_than_f, f1.tuples, f2.tuples): 0)?(( int(*)( int(*
f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Vardecl*, struct Cyc_Set_Set*),
struct Cyc_Dict_Dict* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct Cyc_Dict_Dict* d2, struct Cyc_Absyn_Vardecl* a, struct Cyc_Set_Set* s))
Cyc_ControlFlow_dict_less_than_f, f1.structs, f2.structs): 0;} static struct Cyc_ControlFlow_FlowInfo
Cyc_ControlFlow_add_unassigned( struct Cyc_ControlFlow_FlowInfo f, struct Cyc_Absyn_Vardecl*
vd){ void* _temp7= Cyc_Tcutil_compress(( void*) vd->type); struct Cyc_List_List*
_temp15; struct Cyc_Absyn_Structdecl** _temp17; struct Cyc_Absyn_Structdecl*
_temp19; struct Cyc_Absyn_Structdecl _temp20; struct Cyc_Core_Opt* _temp21;
struct Cyc_Core_Opt _temp23; struct Cyc_List_List* _temp24; struct Cyc_List_List*
_temp26; struct Cyc_Core_Opt* _temp28; void* _temp30; struct Cyc_List_List*
_temp32; struct _tuple0* _temp34; _LL9: if(( unsigned int) _temp7 > 5u?(( struct
_enum_struct*) _temp7)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL16: _temp15=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp7)->f1; goto
_LL10;} else{ goto _LL11;} _LL11: if(( unsigned int) _temp7 > 5u?(( struct
_enum_struct*) _temp7)->tag == Cyc_Absyn_StructType_tag: 0){ _LL35: _temp34=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp7)->f1; goto _LL33;
_LL33: _temp32=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp7)->f2; goto _LL18; _LL18: _temp17=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp7)->f3; if( _temp17 == 0){ goto _LL13;}
else{ _temp19=* _temp17; _temp20=* _temp19; _LL31: _temp30=( void*) _temp20.sc;
goto _LL29; _LL29: _temp28=( struct Cyc_Core_Opt*) _temp20.name; goto _LL27;
_LL27: _temp26=( struct Cyc_List_List*) _temp20.tvs; goto _LL22; _LL22: _temp21=(
struct Cyc_Core_Opt*) _temp20.fields; if( _temp21 == 0){ goto _LL13;} else{
_temp23=* _temp21; _LL25: _temp24=( struct Cyc_List_List*) _temp23.v; goto _LL12;}}}
else{ goto _LL13;} _LL13: goto _LL14; _LL10: { int n=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp15) - 1; struct Cyc_Set_Set* s=(( struct Cyc_Set_Set*(*)(
int(* comp)( int, int))) Cyc_Set_empty)( Cyc_Core_intcmp); for( 0; n >= 0; -- n){
s=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, int elt)) Cyc_Set_insert)( s,
n);} return({ struct Cyc_ControlFlow_FlowInfo _temp36; _temp36.simples= f.simples;
_temp36.tuples=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl*
key, struct Cyc_Set_Set* data)) Cyc_Dict_insert)( f.tuples, vd, s); _temp36.structs=
f.structs; _temp36;});} _LL12: { struct Cyc_Set_Set* s=(( struct Cyc_Set_Set*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)(
Cyc_String_zstrptrcmp); for( 0; _temp24 != 0; _temp24= _temp24->tl){ s=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
s,(*(( struct _tuple3*) _temp24->hd)).f1);} return({ struct Cyc_ControlFlow_FlowInfo
_temp37; _temp37.simples= f.simples; _temp37.tuples= f.tuples; _temp37.structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key,
struct Cyc_Set_Set* data)) Cyc_Dict_insert)( f.structs, vd, s); _temp37;});}
_LL14: return({ struct Cyc_ControlFlow_FlowInfo _temp38; _temp38.simples=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt))
Cyc_Set_insert)( f.simples, vd); _temp38.tuples= f.tuples; _temp38.structs= f.structs;
_temp38;}); _LL8:;} static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_remove_unassigned(
struct Cyc_ControlFlow_FlowInfo f, struct Cyc_Absyn_Vardecl* vd){ if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt)) Cyc_Set_member)( f.simples,
vd)){ return({ struct Cyc_ControlFlow_FlowInfo _temp39; _temp39.simples=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt))
Cyc_Set_delete)( f.simples, vd); _temp39.tuples= f.tuples; _temp39.structs= f.structs;
_temp39;});} if((( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl*
key)) Cyc_Dict_member)( f.tuples, vd)){ return({ struct Cyc_ControlFlow_FlowInfo
_temp40; _temp40.simples= f.simples; _temp40.tuples=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key, struct Cyc_Set_Set* data))
Cyc_Dict_insert)( f.tuples, vd,(( struct Cyc_Set_Set*(*)( int(* comp)( int, int)))
Cyc_Set_empty)( Cyc_Core_intcmp)); _temp40.structs= f.structs; _temp40;});} if(((
int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_member)(
f.structs, vd)){ return({ struct Cyc_ControlFlow_FlowInfo _temp41; _temp41.simples=
f.simples; _temp41.tuples= f.tuples; _temp41.structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key, struct Cyc_Set_Set* data))
Cyc_Dict_insert)( f.structs, vd,(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp));
_temp41;});} return f;} static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_remove_unassigned_tuplefield(
struct Cyc_ControlFlow_FlowInfo f, struct Cyc_Absyn_Vardecl* vd, int field){ if(((
int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_member)(
f.tuples, vd)){ struct Cyc_Set_Set* s=(( struct Cyc_Set_Set*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_lookup)( f.tuples, vd); if((( int(*)(
struct Cyc_Set_Set* s, int elt)) Cyc_Set_member)( s, field)){ return({ struct
Cyc_ControlFlow_FlowInfo _temp42; _temp42.simples= f.simples; _temp42.tuples=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key,
struct Cyc_Set_Set* data)) Cyc_Dict_insert)( f.tuples, vd,(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, int elt)) Cyc_Set_delete)( s, field)); _temp42.structs= f.structs;
_temp42;});}} return f;} static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_remove_unassigned_structfield(
struct Cyc_ControlFlow_FlowInfo f, struct Cyc_Absyn_Vardecl* vd, struct
_tagged_string* field){ if((( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl*
key)) Cyc_Dict_member)( f.structs, vd)){ struct Cyc_Set_Set* s=(( struct Cyc_Set_Set*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_lookup)( f.structs,
vd); if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
s, field)){ return({ struct Cyc_ControlFlow_FlowInfo _temp43; _temp43.simples= f.simples;
_temp43.tuples= f.tuples; _temp43.structs=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key, struct Cyc_Set_Set* data)) Cyc_Dict_insert)(
f.structs, vd,(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_delete)( s, field)); _temp43;});}} return f;}
static int Cyc_ControlFlow_is_unassigned( struct Cyc_ControlFlow_FlowInfo f,
struct Cyc_Absyn_Vardecl* vd){ return((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Absyn_Vardecl* elt)) Cyc_Set_member)( f.simples, vd)? 1:((( int(*)( struct
Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_member)( f.tuples, vd)?
!(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_is_empty)((( struct Cyc_Set_Set*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_lookup)( f.tuples,
vd)): 0))? 1:((( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key))
Cyc_Dict_member)( f.structs, vd)? !(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_is_empty)(((
struct Cyc_Set_Set*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key))
Cyc_Dict_lookup)( f.structs, vd)): 0);} static int Cyc_ControlFlow_is_unassigned_tuplefield(
struct Cyc_ControlFlow_FlowInfo f, struct Cyc_Absyn_Vardecl* vd, int field){
return(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_member)(
f.tuples, vd)?(( int(*)( struct Cyc_Set_Set* s, int elt)) Cyc_Set_member)(((
struct Cyc_Set_Set*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl* key))
Cyc_Dict_lookup)( f.tuples, vd), field): 0;} static int Cyc_ControlFlow_is_unassigned_structfield(
struct Cyc_ControlFlow_FlowInfo f, struct Cyc_Absyn_Vardecl* vd, struct
_tagged_string* field){ return(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Vardecl*
key)) Cyc_Dict_member)( f.structs, vd)?(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)((( struct Cyc_Set_Set*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_Absyn_Vardecl* key)) Cyc_Dict_lookup)( f.structs, vd), field): 0;}
struct Cyc_ControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt* encloser; int visited;
struct Cyc_ControlFlow_FlowInfo old_env;}; typedef struct Cyc_ControlFlow_CFStmtAnnot
Cyc_ControlFlow_cf_stmt_annot_t; char Cyc_ControlFlow_CFAnnot_tag[ 8u]="CFAnnot";
struct Cyc_ControlFlow_CFAnnot_struct{ char* tag; struct Cyc_ControlFlow_CFStmtAnnot
f1;}; void Cyc_ControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee, struct
Cyc_Absyn_Stmt* encloser){ enclosee->annot=({ struct Cyc_ControlFlow_CFAnnot_struct*
_temp44=( struct Cyc_ControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct Cyc_ControlFlow_CFAnnot_struct));*
_temp44=( struct Cyc_ControlFlow_CFAnnot_struct){.tag= Cyc_ControlFlow_CFAnnot_tag,.f1=({
struct Cyc_ControlFlow_CFStmtAnnot _temp45; _temp45.encloser= encloser; _temp45.visited=
0; _temp45.old_env= Cyc_ControlFlow_empty_flow(); _temp45;})};( struct
_xenum_struct*) _temp44;});} static struct Cyc_ControlFlow_CFStmtAnnot* Cyc_ControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt* s){ struct _xenum_struct* _temp46= s->annot; struct Cyc_ControlFlow_CFStmtAnnot
_temp52; struct Cyc_ControlFlow_CFStmtAnnot* _temp54; _LL48: if((*(( struct
_xenum_struct*) _temp46)).tag == Cyc_ControlFlow_CFAnnot_tag){ _LL53: _temp52=((
struct Cyc_ControlFlow_CFAnnot_struct*) _temp46)->f1; _temp54=&(( struct Cyc_ControlFlow_CFAnnot_struct*)
_temp46)->f1; goto _LL49;} else{ goto _LL50;} _LL50: goto _LL51; _LL49: return
_temp54; _LL51:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp55=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp55=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp56=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp57; _temp57.curr= _temp56; _temp57.base= _temp56;
_temp57.last_plus_one= _temp56 + 37; _temp57;})};( struct _xenum_struct*)
_temp55;})); _LL47:;} struct Cyc_Absyn_Stmt* Cyc_ControlFlow_get_encloser(
struct Cyc_Absyn_Stmt* enclosee){ return( Cyc_ControlFlow_get_stmt_annot(
enclosee))->encloser;} struct Cyc_ControlFlow_CFExpSyn{ struct Cyc_ControlFlow_FlowInfo
when_true; struct Cyc_ControlFlow_FlowInfo when_false;}; typedef struct Cyc_ControlFlow_CFExpSyn
Cyc_ControlFlow_cfexpsyn_t; static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_expsynth_to_flow(
struct Cyc_ControlFlow_CFExpSyn syn){ return Cyc_ControlFlow_merge_flow( syn.when_true,
syn.when_false);} static struct Cyc_ControlFlow_CFExpSyn Cyc_ControlFlow_meet_expsynth(
struct Cyc_ControlFlow_CFExpSyn syn1, struct Cyc_ControlFlow_CFExpSyn syn2){
struct Cyc_ControlFlow_FlowInfo f= Cyc_ControlFlow_meet_flow( Cyc_ControlFlow_expsynth_to_flow(
syn1), Cyc_ControlFlow_expsynth_to_flow( syn2)); return({ struct Cyc_ControlFlow_CFExpSyn
_temp58; _temp58.when_true= f; _temp58.when_false= f; _temp58;});} static int
Cyc_ControlFlow_iterate_cf_check= 0; static int Cyc_ControlFlow_iteration_num= 0;
static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_pre_stmt_check( struct
Cyc_ControlFlow_FlowInfo env, struct Cyc_Absyn_Stmt* s){ struct Cyc_ControlFlow_CFStmtAnnot*
sannot= Cyc_ControlFlow_get_stmt_annot( s); env= Cyc_ControlFlow_merge_flow( env,
sannot->old_env); sannot->old_env= env; ++ sannot->visited; if( sannot->visited
!= Cyc_ControlFlow_iteration_num){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp59=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp59=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp60=( char*)"ControlFlow::cf_check_stmt -- bad traversal";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 44; _temp61;})};( struct _xenum_struct*)
_temp59;}));} return env;} static void Cyc_ControlFlow_update_flow( struct Cyc_Absyn_Stmt*
s, struct Cyc_ControlFlow_FlowInfo new_flow){ struct Cyc_ControlFlow_CFStmtAnnot*
sannot= Cyc_ControlFlow_get_stmt_annot( s); if( ! Cyc_ControlFlow_less_than_flow(
new_flow, sannot->old_env)){ sannot->old_env= Cyc_ControlFlow_merge_flow( sannot->old_env,
new_flow); if( sannot->visited == Cyc_ControlFlow_iteration_num){ Cyc_ControlFlow_iterate_cf_check=
1;}}} static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_cf_check_stmt(
struct Cyc_ControlFlow_FlowInfo, struct Cyc_Absyn_Stmt*); static struct Cyc_ControlFlow_CFExpSyn
Cyc_ControlFlow_cf_check_exp( struct Cyc_ControlFlow_FlowInfo, struct Cyc_Absyn_Exp*);
static struct Cyc_ControlFlow_CFExpSyn Cyc_ControlFlow_cf_check_explist_unordered(
struct Cyc_ControlFlow_FlowInfo env, struct Cyc_List_List* es){ struct Cyc_ControlFlow_CFExpSyn
accum=({ struct Cyc_ControlFlow_CFExpSyn _temp62; _temp62.when_true= env;
_temp62.when_false= env; _temp62;}); for( 0; es != 0; es= es->tl){ accum= Cyc_ControlFlow_meet_expsynth(
Cyc_ControlFlow_cf_check_exp( env,( struct Cyc_Absyn_Exp*) es->hd), accum);}
return accum;} static struct Cyc_ControlFlow_CFExpSyn Cyc_ControlFlow_cf_check_exp(
struct Cyc_ControlFlow_FlowInfo env, struct Cyc_Absyn_Exp* e0){ void* _temp63=(
void*) e0->r; void* _temp129; int _temp131; void* _temp133; void* _temp135; int
_temp137; void* _temp139; void* _temp141; void* _temp143; struct Cyc_List_List*
_temp145; void* _temp147; void* _temp149; struct Cyc_Absyn_Vardecl* _temp151;
struct _tuple0* _temp153; struct _tuple0 _temp155; struct _tagged_string*
_temp156; void* _temp158; void* _temp160; struct _tuple0* _temp162; struct
_tagged_string* _temp164; struct Cyc_Absyn_Exp* _temp166; struct Cyc_Absyn_Exp
_temp168; struct Cyc_Position_Segment* _temp169; void* _temp171; void* _temp173;
struct Cyc_Absyn_Vardecl* _temp175; struct _tuple0* _temp177; struct _tuple0
_temp179; struct _tagged_string* _temp180; void* _temp182; struct Cyc_Core_Opt*
_temp184; struct _tagged_string* _temp186; struct Cyc_Absyn_Exp* _temp188;
struct Cyc_Absyn_Exp* _temp190; struct Cyc_Absyn_Exp* _temp192; struct Cyc_Absyn_Exp*
_temp194; struct Cyc_Core_Opt* _temp196; struct Cyc_Absyn_Exp* _temp198; struct
Cyc_Absyn_Exp* _temp200; struct Cyc_Core_Opt* _temp202; struct Cyc_Absyn_Exp*
_temp204; struct Cyc_Absyn_Exp* _temp206; struct Cyc_Absyn_Exp* _temp208; struct
Cyc_Absyn_Exp* _temp210; struct Cyc_List_List* _temp212; struct Cyc_Absyn_Exp*
_temp214; struct Cyc_Absyn_Exp* _temp216; void* _temp218; struct Cyc_Absyn_Exp*
_temp220; struct Cyc_Absyn_Exp* _temp222; struct Cyc_List_List* _temp224; struct
Cyc_Absyn_Exp* _temp226; struct Cyc_Absyn_Exp* _temp228; void* _temp230; struct
Cyc_Absyn_Exp* _temp232; struct Cyc_Absyn_Exp* _temp234; struct _tagged_string*
_temp236; struct Cyc_Absyn_Exp* _temp238; struct Cyc_List_List* _temp240; struct
Cyc_Absyn_Enumfield* _temp242; struct Cyc_Absyn_Enumdecl* _temp244; struct Cyc_List_List*
_temp246; struct Cyc_Core_Opt* _temp248; struct Cyc_Core_Opt* _temp250; struct
Cyc_Absyn_Enumfield* _temp252; struct Cyc_Absyn_Xenumdecl* _temp254; struct Cyc_List_List*
_temp256; struct Cyc_Core_Opt* _temp258; struct Cyc_List_List* _temp260; struct
_tuple1* _temp262; struct Cyc_List_List* _temp264; int _temp266; struct Cyc_Absyn_Structdecl*
_temp268; struct Cyc_List_List* _temp270; struct Cyc_Core_Opt* _temp272; struct
_tuple0* _temp274; struct Cyc_Absyn_Exp* _temp276; struct Cyc_Absyn_Exp*
_temp278; struct Cyc_Absyn_Vardecl* _temp280; struct Cyc_Absyn_Exp* _temp282;
struct Cyc_Absyn_Exp* _temp284; struct Cyc_Absyn_Stmt* _temp286; _LL65: if(((
struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Const_e_tag){ _LL130: _temp129=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if(( unsigned int)
_temp129 > 1u?(( struct _enum_struct*) _temp129)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL134: _temp133=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp129)->f1; if(
_temp133 == Cyc_Absyn_Signed){ goto _LL132;} else{ goto _LL67;} _LL132: _temp131=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp129)->f2; if( _temp131 == 0){ goto
_LL66;} else{ goto _LL67;}} else{ goto _LL67;}} else{ goto _LL67;} _LL67: if(((
struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Const_e_tag){ _LL136: _temp135=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if(( unsigned int)
_temp135 > 1u?(( struct _enum_struct*) _temp135)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL140: _temp139=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp135)->f1; if(
_temp139 == Cyc_Absyn_Signed){ goto _LL138;} else{ goto _LL69;} _LL138: _temp137=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp135)->f2; goto _LL68;} else{ goto
_LL69;}} else{ goto _LL69;} _LL69: if((( struct _enum_struct*) _temp63)->tag ==
Cyc_Absyn_Const_e_tag){ _LL142: _temp141=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp63)->f1; goto _LL70;} else{ goto _LL71;} _LL71: if((( struct _enum_struct*)
_temp63)->tag == Cyc_Absyn_Sizeof_e_tag){ _LL144: _temp143=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp63)->f1; goto _LL72;} else{ goto _LL73;} _LL73: if((( struct _enum_struct*)
_temp63)->tag == Cyc_Absyn_Primop_e_tag){ _LL148: _temp147=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp63)->f1; goto _LL146; _LL146: _temp145=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp63)->f2; goto _LL74;} else{ goto _LL75;} _LL75:
if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Var_e_tag){ _LL154:
_temp153=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp63)->f1;
_temp155=* _temp153; _LL159: _temp158= _temp155.f1; goto _LL157; _LL157:
_temp156= _temp155.f2; goto _LL150; _LL150: _temp149=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp63)->f2; if(( unsigned int) _temp149 > 1u?(( struct _enum_struct*) _temp149)->tag
== Cyc_Absyn_Local_b_tag: 0){ _LL152: _temp151=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp149)->f1; goto _LL76;} else{ goto _LL77;}}
else{ goto _LL77;} _LL77: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Var_e_tag){
_LL163: _temp162=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp63)->f1;
goto _LL161; _LL161: _temp160=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp63)->f2;
goto _LL78;} else{ goto _LL79;} _LL79: if((( struct _enum_struct*) _temp63)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL167: _temp166=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp63)->f1; _temp168=* _temp166;
_LL185: _temp184=( struct Cyc_Core_Opt*) _temp168.topt; goto _LL172; _LL172:
_temp171=( void*) _temp168.r; if((( struct _enum_struct*) _temp171)->tag == Cyc_Absyn_Var_e_tag){
_LL178: _temp177=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp171)->f1;
_temp179=* _temp177; _LL183: _temp182= _temp179.f1; goto _LL181; _LL181:
_temp180= _temp179.f2; goto _LL174; _LL174: _temp173=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp171)->f2; if(( unsigned int) _temp173 > 1u?(( struct _enum_struct*)
_temp173)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL176: _temp175=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp173)->f1; goto _LL170;} else{ goto _LL81;}}
else{ goto _LL81;} _LL170: _temp169=( struct Cyc_Position_Segment*) _temp168.loc;
goto _LL165; _LL165: _temp164=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp63)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _enum_struct*)
_temp63)->tag == Cyc_Absyn_StructMember_e_tag){ _LL189: _temp188=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp63)->f1; goto _LL187; _LL187:
_temp186=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp63)->f2; goto _LL82;} else{ goto _LL83;} _LL83: if((( struct _enum_struct*)
_temp63)->tag == Cyc_Absyn_Subscript_e_tag){ _LL193: _temp192=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp63)->f1; goto _LL191; _LL191:
_temp190=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp63)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if((( struct _enum_struct*)
_temp63)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL199: _temp198=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp63)->f1; goto _LL197; _LL197: _temp196=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp63)->f2; if(
_temp196 == 0){ goto _LL195;} else{ goto _LL87;} _LL195: _temp194=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp63)->f3; goto _LL86;} else{ goto _LL87;}
_LL87: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL205: _temp204=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp63)->f1; goto _LL203; _LL203: _temp202=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp63)->f2; goto _LL201; _LL201: _temp200=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp63)->f3; goto _LL88;} else{ goto _LL89;}
_LL89: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Conditional_e_tag){
_LL211: _temp210=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp63)->f1; goto _LL209; _LL209: _temp208=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp63)->f2; goto _LL207; _LL207: _temp206=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp63)->f3;
goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _enum_struct*) _temp63)->tag
== Cyc_Absyn_FnCall_e_tag){ _LL215: _temp214=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp63)->f1; goto _LL213; _LL213: _temp212=( struct
Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp63)->f2; goto _LL92;}
else{ goto _LL93;} _LL93: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Throw_e_tag){
_LL217: _temp216=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp63)->f1; goto _LL94;} else{ goto _LL95;} _LL95: if((( struct _enum_struct*)
_temp63)->tag == Cyc_Absyn_Increment_e_tag){ _LL221: _temp220=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp63)->f1; goto _LL219; _LL219:
_temp218=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp63)->f2; goto
_LL96;} else{ goto _LL97;} _LL97: if((( struct _enum_struct*) _temp63)->tag ==
Cyc_Absyn_NoInstantiate_e_tag){ _LL223: _temp222=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp63)->f1; goto _LL98;} else{ goto
_LL99;} _LL99: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL227: _temp226=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp63)->f1; goto _LL225; _LL225: _temp224=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp63)->f2; goto _LL100;} else{ goto _LL101;}
_LL101: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Cast_e_tag){
_LL231: _temp230=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp63)->f1; goto
_LL229; _LL229: _temp228=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp63)->f2; goto _LL102;} else{ goto _LL103;} _LL103: if((( struct
_enum_struct*) _temp63)->tag == Cyc_Absyn_Address_e_tag){ _LL233: _temp232=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp63)->f1; goto
_LL104;} else{ goto _LL105;} _LL105: if((( struct _enum_struct*) _temp63)->tag
== Cyc_Absyn_Deref_e_tag){ _LL235: _temp234=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp63)->f1; goto _LL106;} else{ goto _LL107;}
_LL107: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL239: _temp238=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp63)->f1; goto _LL237; _LL237: _temp236=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp63)->f2; goto _LL108;} else{ goto _LL109;}
_LL109: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Tuple_e_tag){
_LL241: _temp240=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp63)->f1; goto _LL110;} else{ goto _LL111;} _LL111: if((( struct
_enum_struct*) _temp63)->tag == Cyc_Absyn_Enum_e_tag){ _LL251: _temp250=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp63)->f1; goto _LL249;
_LL249: _temp248=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp63)->f2; goto _LL247; _LL247: _temp246=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp63)->f3; goto _LL245; _LL245: _temp244=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp63)->f4; goto
_LL243; _LL243: _temp242=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp63)->f5; goto _LL112;} else{ goto _LL113;} _LL113: if((( struct
_enum_struct*) _temp63)->tag == Cyc_Absyn_Xenum_e_tag){ _LL259: _temp258=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp63)->f1; goto
_LL257; _LL257: _temp256=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp63)->f2; goto _LL255; _LL255: _temp254=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp63)->f3; goto _LL253; _LL253: _temp252=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp63)->f4;
goto _LL114;} else{ goto _LL115;} _LL115: if((( struct _enum_struct*) _temp63)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL263: _temp262=( struct _tuple1*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp63)->f1; goto _LL261; _LL261: _temp260=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp63)->f2;
goto _LL116;} else{ goto _LL117;} _LL117: if((( struct _enum_struct*) _temp63)->tag
== Cyc_Absyn_Array_e_tag){ _LL267: _temp266=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp63)->f1; goto _LL265; _LL265: _temp264=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp63)->f2; goto _LL118;} else{ goto _LL119;}
_LL119: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_Struct_e_tag){
_LL275: _temp274=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp63)->f1;
goto _LL273; _LL273: _temp272=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp63)->f2; goto _LL271; _LL271: _temp270=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp63)->f3; goto _LL269; _LL269: _temp268=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp63)->f4; goto
_LL120;} else{ goto _LL121;} _LL121: if((( struct _enum_struct*) _temp63)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL281: _temp280=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp63)->f1; goto _LL279; _LL279:
_temp278=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp63)->f2; goto _LL277; _LL277: _temp276=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp63)->f3; goto _LL122;} else{ goto _LL123;}
_LL123: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL285: _temp284=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp63)->f1; goto _LL283; _LL283: _temp282=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp63)->f2; goto _LL124;} else{ goto _LL125;}
_LL125: if((( struct _enum_struct*) _temp63)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL287: _temp286=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp63)->f1; goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL66:
return({ struct Cyc_ControlFlow_CFExpSyn _temp288; _temp288.when_true= Cyc_ControlFlow_empty_flow();
_temp288.when_false= env; _temp288;}); _LL68: return({ struct Cyc_ControlFlow_CFExpSyn
_temp289; _temp289.when_true= env; _temp289.when_false= Cyc_ControlFlow_empty_flow();
_temp289;}); _LL70: goto _LL72; _LL72: return({ struct Cyc_ControlFlow_CFExpSyn
_temp290; _temp290.when_true= env; _temp290.when_false= env; _temp290;}); _LL74:
return Cyc_ControlFlow_cf_check_explist_unordered( env, _temp145); _LL76: if(
Cyc_ControlFlow_is_unassigned( env, _temp151)){ Cyc_Tcutil_terr( e0->loc,({
struct _tagged_string _temp291=* _temp156; xprintf("variable %.*s may be uninitialized",
_temp291.last_plus_one - _temp291.curr, _temp291.curr);}));} return({ struct Cyc_ControlFlow_CFExpSyn
_temp292; _temp292.when_true= env; _temp292.when_false= env; _temp292;}); _LL78:
return({ struct Cyc_ControlFlow_CFExpSyn _temp293; _temp293.when_true= env;
_temp293.when_false= env; _temp293;}); _LL80: if( Cyc_ControlFlow_is_unassigned_structfield(
env, _temp175, _temp164)){ Cyc_Tcutil_terr( e0->loc,({ struct _tagged_string
_temp294=* _temp164; struct _tagged_string _temp295=* _temp180; xprintf("field %.*s of struct %.*s may be uninitialized",
_temp294.last_plus_one - _temp294.curr, _temp294.curr, _temp295.last_plus_one -
_temp295.curr, _temp295.curr);}));} return({ struct Cyc_ControlFlow_CFExpSyn
_temp296; _temp296.when_true= env; _temp296.when_false= env; _temp296;}); _LL82:
return Cyc_ControlFlow_cf_check_exp( env, _temp188); _LL84: { struct _tuple4
_temp298=({ struct _tuple4 _temp297; _temp297.f1=( void*) _temp192->r; _temp297.f2=
Cyc_Tcutil_compress(( void*)( _temp192->topt)->v); _temp297;}); void* _temp304;
struct Cyc_List_List* _temp306; void* _temp308; void* _temp310; struct Cyc_Absyn_Vardecl*
_temp312; struct _tuple0* _temp314; struct _tuple0 _temp316; struct
_tagged_string* _temp317; void* _temp319; _LL300: _LL309: _temp308= _temp298.f1;
if((( struct _enum_struct*) _temp308)->tag == Cyc_Absyn_Var_e_tag){ _LL315:
_temp314=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp308)->f1;
_temp316=* _temp314; _LL320: _temp319= _temp316.f1; goto _LL318; _LL318:
_temp317= _temp316.f2; goto _LL311; _LL311: _temp310=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp308)->f2; if(( unsigned int) _temp310 > 1u?(( struct _enum_struct*)
_temp310)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL313: _temp312=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp310)->f1; goto _LL305;} else{ goto _LL302;}}
else{ goto _LL302;} _LL305: _temp304= _temp298.f2; if(( unsigned int) _temp304 >
5u?(( struct _enum_struct*) _temp304)->tag == Cyc_Absyn_TupleType_tag: 0){
_LL307: _temp306=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp304)->f1; goto _LL301;} else{ goto _LL302;} _LL302: goto _LL303; _LL301: {
unsigned int field= Cyc_Evexp_eval_const_uint_exp( _temp190); if( Cyc_ControlFlow_is_unassigned_tuplefield(
env, _temp312,( int) field)){ Cyc_Tcutil_terr( e0->loc,({ unsigned int _temp321=
field; struct _tagged_string _temp322=* _temp317; xprintf("field %u of tuple %.*s may be uninitialized",
_temp321, _temp322.last_plus_one - _temp322.curr, _temp322.curr);}));} return({
struct Cyc_ControlFlow_CFExpSyn _temp323; _temp323.when_true= env; _temp323.when_false=
env; _temp323;});} _LL303: return Cyc_ControlFlow_meet_expsynth( Cyc_ControlFlow_cf_check_exp(
env, _temp192), Cyc_ControlFlow_cf_check_exp( env, _temp190)); _LL299:;} _LL86: {
struct Cyc_ControlFlow_CFExpSyn syn2= Cyc_ControlFlow_cf_check_exp( env,
_temp194); struct Cyc_ControlFlow_FlowInfo flow; void* _temp324=( void*)
_temp198->r; void* _temp334; struct Cyc_Absyn_Vardecl* _temp336; struct _tuple0*
_temp338; struct _tagged_string* _temp340; struct Cyc_Absyn_Exp* _temp342;
struct Cyc_Absyn_Exp _temp344; struct Cyc_Position_Segment* _temp345; void*
_temp347; void* _temp349; struct Cyc_Absyn_Vardecl* _temp351; struct _tuple0*
_temp353; struct Cyc_Core_Opt* _temp355; struct Cyc_Absyn_Exp* _temp357; struct
Cyc_Absyn_Exp* _temp359; _LL326: if((( struct _enum_struct*) _temp324)->tag ==
Cyc_Absyn_Var_e_tag){ _LL339: _temp338=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp324)->f1; goto _LL335; _LL335: _temp334=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp324)->f2; if(( unsigned int) _temp334 > 1u?(( struct _enum_struct*)
_temp334)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL337: _temp336=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp334)->f1; goto _LL327;} else{ goto _LL328;}}
else{ goto _LL328;} _LL328: if((( struct _enum_struct*) _temp324)->tag == Cyc_Absyn_StructMember_e_tag){
_LL343: _temp342=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp324)->f1; _temp344=* _temp342; _LL356: _temp355=( struct Cyc_Core_Opt*)
_temp344.topt; goto _LL348; _LL348: _temp347=( void*) _temp344.r; if((( struct
_enum_struct*) _temp347)->tag == Cyc_Absyn_Var_e_tag){ _LL354: _temp353=( struct
_tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp347)->f1; goto _LL350; _LL350:
_temp349=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp347)->f2; if((
unsigned int) _temp349 > 1u?(( struct _enum_struct*) _temp349)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL352: _temp351=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp349)->f1; goto _LL346;} else{ goto _LL330;}} else{ goto _LL330;} _LL346:
_temp345=( struct Cyc_Position_Segment*) _temp344.loc; goto _LL341; _LL341:
_temp340=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp324)->f2; goto _LL329;} else{ goto _LL330;} _LL330: if((( struct
_enum_struct*) _temp324)->tag == Cyc_Absyn_Subscript_e_tag){ _LL360: _temp359=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp324)->f1;
goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp324)->f2; goto _LL331;} else{ goto _LL332;} _LL332: goto _LL333; _LL327:
flow= Cyc_ControlFlow_remove_unassigned( Cyc_ControlFlow_expsynth_to_flow( syn2),
_temp336); return({ struct Cyc_ControlFlow_CFExpSyn _temp361; _temp361.when_true=
flow; _temp361.when_false= flow; _temp361;}); _LL329: flow= Cyc_ControlFlow_remove_unassigned_structfield(
Cyc_ControlFlow_expsynth_to_flow( syn2), _temp351, _temp340); return({ struct
Cyc_ControlFlow_CFExpSyn _temp362; _temp362.when_true= flow; _temp362.when_false=
flow; _temp362;}); _LL331: { struct _tuple4 _temp364=({ struct _tuple4 _temp363;
_temp363.f1=( void*) _temp359->r; _temp363.f2= Cyc_Tcutil_compress(( void*)(
_temp359->topt)->v); _temp363;}); void* _temp370; struct Cyc_List_List* _temp372;
void* _temp374; void* _temp376; struct Cyc_Absyn_Vardecl* _temp378; struct
_tuple0* _temp380; _LL366: _LL375: _temp374= _temp364.f1; if((( struct
_enum_struct*) _temp374)->tag == Cyc_Absyn_Var_e_tag){ _LL381: _temp380=( struct
_tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp374)->f1; goto _LL377; _LL377:
_temp376=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp374)->f2; if((
unsigned int) _temp376 > 1u?(( struct _enum_struct*) _temp376)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL379: _temp378=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp376)->f1; goto _LL371;} else{ goto _LL368;}} else{ goto _LL368;} _LL371:
_temp370= _temp364.f2; if(( unsigned int) _temp370 > 5u?(( struct _enum_struct*)
_temp370)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL373: _temp372=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp370)->f1; goto _LL367;} else{ goto
_LL368;} _LL368: goto _LL369; _LL367: { unsigned int field= Cyc_Evexp_eval_const_uint_exp(
_temp357); flow= Cyc_ControlFlow_remove_unassigned_tuplefield( Cyc_ControlFlow_expsynth_to_flow(
syn2), _temp378,( int) field); return({ struct Cyc_ControlFlow_CFExpSyn _temp382;
_temp382.when_true= flow; _temp382.when_false= flow; _temp382;});} _LL369:
return Cyc_ControlFlow_meet_expsynth( Cyc_ControlFlow_cf_check_exp( env,
_temp198), syn2); _LL365:;} _LL333: return Cyc_ControlFlow_meet_expsynth( Cyc_ControlFlow_cf_check_exp(
env, _temp198), syn2); _LL325:;} _LL88: return Cyc_ControlFlow_meet_expsynth(
Cyc_ControlFlow_cf_check_exp( env, _temp204), Cyc_ControlFlow_cf_check_exp( env,
_temp200)); _LL90: { struct Cyc_ControlFlow_CFExpSyn syn1= Cyc_ControlFlow_cf_check_exp(
env, _temp210); struct Cyc_ControlFlow_CFExpSyn syn2= Cyc_ControlFlow_cf_check_exp(
syn1.when_true, _temp208); struct Cyc_ControlFlow_CFExpSyn syn3= Cyc_ControlFlow_cf_check_exp(
syn1.when_false, _temp206); return({ struct Cyc_ControlFlow_CFExpSyn _temp383;
_temp383.when_true= Cyc_ControlFlow_merge_flow( syn2.when_true, syn3.when_true);
_temp383.when_false= Cyc_ControlFlow_merge_flow( syn2.when_false, syn3.when_false);
_temp383;});} _LL92: return Cyc_ControlFlow_meet_expsynth( Cyc_ControlFlow_cf_check_exp(
env, _temp214), Cyc_ControlFlow_cf_check_explist_unordered( env, _temp212));
_LL94: Cyc_ControlFlow_cf_check_exp( env, _temp216); return({ struct Cyc_ControlFlow_CFExpSyn
_temp384; _temp384.when_true= Cyc_ControlFlow_empty_flow(); _temp384.when_false=
Cyc_ControlFlow_empty_flow(); _temp384;}); _LL96: _temp222= _temp220; goto _LL98;
_LL98: _temp226= _temp222; goto _LL100; _LL100: _temp228= _temp226; goto _LL102;
_LL102: _temp232= _temp228; goto _LL104; _LL104: _temp234= _temp232; goto _LL106;
_LL106: _temp238= _temp234; goto _LL108; _LL108: return Cyc_ControlFlow_cf_check_exp(
env, _temp238); _LL110: _temp246= _temp240; goto _LL112; _LL112: _temp256=
_temp246; goto _LL114; _LL114: return Cyc_ControlFlow_cf_check_explist_unordered(
env, _temp256); _LL116: _temp264= _temp260; goto _LL118; _LL118: _temp270=
_temp264; goto _LL120; _LL120: { struct Cyc_ControlFlow_CFExpSyn accum=({ struct
Cyc_ControlFlow_CFExpSyn _temp385; _temp385.when_true= env; _temp385.when_false=
env; _temp385;}); for( 0; _temp270 != 0; _temp270= _temp270->tl){ accum= Cyc_ControlFlow_meet_expsynth(
Cyc_ControlFlow_cf_check_exp( env,(*(( struct _tuple5*) _temp270->hd)).f2),
accum);} return accum;} _LL122: { struct Cyc_ControlFlow_CFExpSyn syn1= Cyc_ControlFlow_cf_check_exp(
env, _temp278); Cyc_ControlFlow_cf_check_exp( Cyc_ControlFlow_expsynth_to_flow(
syn1), _temp276); return syn1;} _LL124: return Cyc_ControlFlow_cf_check_exp( Cyc_ControlFlow_expsynth_to_flow(
Cyc_ControlFlow_cf_check_exp( env, _temp284)), _temp282); _LL126: { struct Cyc_ControlFlow_FlowInfo
syn= Cyc_ControlFlow_cf_check_stmt( env, _temp286); return({ struct Cyc_ControlFlow_CFExpSyn
_temp386; _temp386.when_true= syn; _temp386.when_false= syn; _temp386;});}
_LL128:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp387=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp387=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp388=( char*)"ControlFlow::cf_check_exp -- bad exp syntax or unimplemented exp form";
struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 70; _temp389;})};( struct _xenum_struct*)
_temp387;})); _LL64:;} static int Cyc_ControlFlow_cf_check_stmt_ft( struct Cyc_ControlFlow_FlowInfo
env, struct Cyc_Absyn_Stmt* s){ return ! Cyc_ControlFlow_less_than_flow( Cyc_ControlFlow_cf_check_stmt(
Cyc_ControlFlow_add_unassigned( env, Cyc_ControlFlow_bogus_vardecl), s), Cyc_ControlFlow_empty_flow());}
static void Cyc_ControlFlow_cf_check_switch_clauses( struct Cyc_ControlFlow_FlowInfo
env, struct Cyc_List_List* scs){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp392; struct Cyc_Position_Segment* _temp393; struct Cyc_Absyn_Stmt* _temp395;
struct Cyc_Absyn_Exp* _temp397; struct Cyc_Core_Opt* _temp399; struct Cyc_Absyn_Pat*
_temp401; struct Cyc_Absyn_Switch_clause* _temp390=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp392=* _temp390; _LL402: _temp401=( struct Cyc_Absyn_Pat*) _temp392.pattern;
goto _LL400; _LL400: _temp399=( struct Cyc_Core_Opt*) _temp392.pat_vars; goto
_LL398; _LL398: _temp397=( struct Cyc_Absyn_Exp*) _temp392.where_clause; goto
_LL396; _LL396: _temp395=( struct Cyc_Absyn_Stmt*) _temp392.body; goto _LL394;
_LL394: _temp393=( struct Cyc_Position_Segment*) _temp392.loc; goto _LL391;
_LL391: if( _temp397 != 0){ env= Cyc_ControlFlow_expsynth_to_flow( Cyc_ControlFlow_cf_check_exp(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp403= _temp397; if(
_temp403 == 0){ _throw( Null_Exception);} _temp403;})));} if( Cyc_ControlFlow_cf_check_stmt_ft(
env, _temp395)){ Cyc_Tcutil_terr( _temp395->loc,( struct _tagged_string)({ char*
_temp404=( char*)"switch clause may implicitly fallthru"; struct _tagged_string
_temp405; _temp405.curr= _temp404; _temp405.base= _temp404; _temp405.last_plus_one=
_temp404 + 38; _temp405;}));}}} static struct Cyc_ControlFlow_FlowInfo Cyc_ControlFlow_cf_check_stmt(
struct Cyc_ControlFlow_FlowInfo env, struct Cyc_Absyn_Stmt* s){ env= Cyc_ControlFlow_pre_stmt_check(
env, s);{ void* _temp406=( void*) s->r; struct Cyc_Absyn_Exp* _temp448; struct
Cyc_Absyn_Exp* _temp450; struct Cyc_Absyn_Exp* _temp452; struct Cyc_Absyn_Stmt*
_temp454; struct Cyc_Absyn_Stmt* _temp456; struct Cyc_Absyn_Stmt* _temp458;
struct Cyc_Absyn_Stmt* _temp460; struct Cyc_Absyn_Exp* _temp462; struct Cyc_Absyn_Stmt*
_temp464; struct _tuple2 _temp466; struct Cyc_Absyn_Stmt* _temp468; struct Cyc_Absyn_Exp*
_temp470; struct _tuple2 _temp472; struct Cyc_Absyn_Stmt* _temp474; struct Cyc_Absyn_Exp*
_temp476; struct Cyc_Absyn_Stmt* _temp478; struct Cyc_Absyn_Stmt* _temp480;
struct _tuple2 _temp482; struct Cyc_Absyn_Stmt* _temp484; struct Cyc_Absyn_Exp*
_temp486; struct _tuple2 _temp488; struct Cyc_Absyn_Stmt* _temp490; struct Cyc_Absyn_Exp*
_temp492; struct Cyc_Absyn_Exp* _temp494; struct Cyc_Absyn_Stmt* _temp496;
struct _tagged_string* _temp498; struct Cyc_Absyn_Stmt* _temp500; struct Cyc_Absyn_Stmt*
_temp502; struct Cyc_Absyn_Stmt* _temp504; struct Cyc_Absyn_Stmt* _temp506;
struct Cyc_List_List* _temp508; struct Cyc_List_List* _temp510; struct Cyc_Absyn_Exp*
_temp512; struct Cyc_List_List* _temp514; struct Cyc_Absyn_Stmt* _temp516;
struct Cyc_Absyn_Stmt* _temp518; struct Cyc_Absyn_Decl* _temp520; struct Cyc_Absyn_Decl
_temp522; struct Cyc_Position_Segment* _temp523; void* _temp525; struct Cyc_Absyn_Vardecl*
_temp527; struct Cyc_Absyn_Stmt* _temp529; struct Cyc_Absyn_Decl* _temp531;
struct Cyc_Absyn_Decl _temp533; struct Cyc_Position_Segment* _temp534; void*
_temp536; int _temp538; struct Cyc_Absyn_Exp* _temp540; struct Cyc_Core_Opt*
_temp542; struct Cyc_Core_Opt* _temp544; struct Cyc_Core_Opt _temp546; struct
Cyc_List_List* _temp547; struct Cyc_Absyn_Pat* _temp549; struct Cyc_Absyn_Stmt*
_temp551; struct _tagged_string* _temp553; _LL408: if( _temp406 == Cyc_Absyn_Skip_s){
goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp406 > 1u?((
struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL449:
_temp448=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp406)->f1;
goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int) _temp406 > 1u?((
struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL451:
_temp450=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp406)->f1;
if( _temp450 == 0){ goto _LL413;} else{ goto _LL414;}} else{ goto _LL414;}
_LL414: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*) _temp406)->tag
== Cyc_Absyn_Return_s_tag: 0){ _LL453: _temp452=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp406)->f1; goto _LL415;} else{ goto
_LL416;} _LL416: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*)
_temp406)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL457: _temp456=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp406)->f1; goto _LL455; _LL455: _temp454=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp406)->f2; goto
_LL417;} else{ goto _LL418;} _LL418: if(( unsigned int) _temp406 > 1u?(( struct
_enum_struct*) _temp406)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL463:
_temp462=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp406)->f1; goto _LL461; _LL461: _temp460=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp406)->f2; goto _LL459; _LL459: _temp458=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp406)->f3;
goto _LL419;} else{ goto _LL420;} _LL420: if(( unsigned int) _temp406 > 1u?((
struct _enum_struct*) _temp406)->tag == Cyc_Absyn_While_s_tag: 0){ _LL467:
_temp466=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp406)->f1;
_LL471: _temp470= _temp466.f1; goto _LL469; _LL469: _temp468= _temp466.f2; goto
_LL465; _LL465: _temp464=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp406)->f2; goto _LL421;} else{ goto _LL422;} _LL422: if(( unsigned int)
_temp406 > 1u?(( struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL479: _temp478=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp406)->f1; goto _LL473; _LL473: _temp472=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp406)->f2; _LL477: _temp476= _temp472.f1; goto _LL475; _LL475: _temp474=
_temp472.f2; goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int)
_temp406 > 1u?(( struct _enum_struct*) _temp406)->tag == Cyc_Absyn_For_s_tag: 0){
_LL495: _temp494=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f1; goto _LL489; _LL489: _temp488=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f2; _LL493: _temp492= _temp488.f1; goto _LL491; _LL491: _temp490=
_temp488.f2; goto _LL483; _LL483: _temp482=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f3; _LL487: _temp486= _temp482.f1; goto _LL485; _LL485: _temp484=
_temp482.f2; goto _LL481; _LL481: _temp480=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp406)->f4; goto _LL425;} else{ goto _LL426;} _LL426:
if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL499: _temp498=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp406)->f1; goto _LL497; _LL497: _temp496=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp406)->f2; goto _LL427;} else{ goto _LL428;}
_LL428: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*) _temp406)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL501: _temp500=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp406)->f1; if( _temp500 == 0){ goto _LL429;}
else{ goto _LL430;}} else{ goto _LL430;} _LL430: if(( unsigned int) _temp406 > 1u?((
struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL503:
_temp502=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp406)->f1;
goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp406 > 1u?((
struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL505:
_temp504=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp406)->f1; goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp406 > 1u?(( struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL509: _temp508=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp406)->f1; goto _LL507; _LL507: _temp506=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Fallthru_s_struct*) _temp406)->f2; goto _LL435;} else{ goto _LL436;}
_LL436: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*) _temp406)->tag
== Cyc_Absyn_Switch_s_tag: 0){ _LL513: _temp512=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp406)->f1; goto _LL511; _LL511: _temp510=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp406)->f2; goto
_LL437;} else{ goto _LL438;} _LL438: if(( unsigned int) _temp406 > 1u?(( struct
_enum_struct*) _temp406)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL517: _temp516=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*) _temp406)->f1;
goto _LL515; _LL515: _temp514=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp406)->f2; goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp406 > 1u?(( struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL521: _temp520=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp406)->f1; _temp522=* _temp520; _LL526: _temp525=( void*) _temp522.r; if((
unsigned int) _temp525 > 1u?(( struct _enum_struct*) _temp525)->tag == Cyc_Absyn_Var_d_tag:
0){ _LL528: _temp527=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp525)->f1; goto _LL524;} else{ goto _LL442;} _LL524: _temp523=( struct Cyc_Position_Segment*)
_temp522.loc; goto _LL519; _LL519: _temp518=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp406)->f2; goto _LL441;} else{ goto _LL442;}
_LL442: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*) _temp406)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL532: _temp531=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp406)->f1; _temp533=* _temp531; _LL537: _temp536=(
void*) _temp533.r; if(( unsigned int) _temp536 > 1u?(( struct _enum_struct*)
_temp536)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL550: _temp549=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp536)->f1; goto _LL545; _LL545: _temp544=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp536)->f2; if(
_temp544 == 0){ goto _LL444;} else{ _temp546=* _temp544; _LL548: _temp547=(
struct Cyc_List_List*) _temp546.v; goto _LL543;} _LL543: _temp542=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp536)->f3; goto _LL541; _LL541: _temp540=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp536)->f4; goto
_LL539; _LL539: _temp538=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp536)->f5;
goto _LL535;} else{ goto _LL444;} _LL535: _temp534=( struct Cyc_Position_Segment*)
_temp533.loc; goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp406)->f2; goto _LL443;} else{ goto _LL444;}
_LL444: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*) _temp406)->tag
== Cyc_Absyn_Label_s_tag: 0){ _LL554: _temp553=( struct _tagged_string*)((
struct Cyc_Absyn_Label_s_struct*) _temp406)->f1; goto _LL552; _LL552: _temp551=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*) _temp406)->f2; goto
_LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL409: return env; _LL411:
return Cyc_ControlFlow_expsynth_to_flow( Cyc_ControlFlow_cf_check_exp( env,
_temp448)); _LL413: return Cyc_ControlFlow_empty_flow(); _LL415: Cyc_ControlFlow_cf_check_exp(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp555= _temp452; if(
_temp555 == 0){ _throw( Null_Exception);} _temp555;})); return Cyc_ControlFlow_empty_flow();
_LL417: return Cyc_ControlFlow_cf_check_stmt( Cyc_ControlFlow_cf_check_stmt( env,
_temp456), _temp454); _LL419: { struct Cyc_ControlFlow_CFExpSyn syn_e= Cyc_ControlFlow_cf_check_exp(
env, _temp462); struct Cyc_ControlFlow_FlowInfo syn1= Cyc_ControlFlow_cf_check_stmt(
syn_e.when_true, _temp460); struct Cyc_ControlFlow_FlowInfo syn2= Cyc_ControlFlow_cf_check_stmt(
syn_e.when_false, _temp458); return Cyc_ControlFlow_merge_flow( syn1, syn2);}
_LL421: { struct Cyc_ControlFlow_CFExpSyn syn_e= Cyc_ControlFlow_cf_check_exp(
Cyc_ControlFlow_pre_stmt_check( env, _temp468), _temp470); struct Cyc_ControlFlow_FlowInfo
syn_b= Cyc_ControlFlow_cf_check_stmt( syn_e.when_true, _temp464); Cyc_ControlFlow_update_flow(
_temp468, syn_b); return syn_e.when_false;} _LL423: { struct Cyc_ControlFlow_FlowInfo
syn_b= Cyc_ControlFlow_cf_check_stmt( env, _temp478); struct Cyc_ControlFlow_CFExpSyn
syn_e= Cyc_ControlFlow_cf_check_exp( Cyc_ControlFlow_pre_stmt_check( syn_b,
_temp474), _temp476); Cyc_ControlFlow_update_flow( _temp478, syn_e.when_true);
return syn_e.when_false;} _LL425: { struct Cyc_ControlFlow_FlowInfo syn_e1= Cyc_ControlFlow_expsynth_to_flow(
Cyc_ControlFlow_cf_check_exp( env, _temp494)); struct Cyc_ControlFlow_CFExpSyn
syn_e2= Cyc_ControlFlow_cf_check_exp( Cyc_ControlFlow_pre_stmt_check( syn_e1,
_temp490), _temp492); struct Cyc_ControlFlow_FlowInfo syn_b= Cyc_ControlFlow_cf_check_stmt(
syn_e2.when_true, _temp480); struct Cyc_ControlFlow_FlowInfo syn_e3= Cyc_ControlFlow_expsynth_to_flow(
Cyc_ControlFlow_cf_check_exp( Cyc_ControlFlow_pre_stmt_check( syn_b, _temp484),
_temp486)); Cyc_ControlFlow_update_flow( _temp490, syn_e3); return syn_e2.when_false;}
_LL427: if( Cyc_ControlFlow_iteration_num == 1){ struct Cyc_Absyn_Stmt*
my_encloser=( Cyc_ControlFlow_get_stmt_annot( s))->encloser; struct Cyc_Absyn_Stmt*
dest_encloser=( Cyc_ControlFlow_get_stmt_annot(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Absyn_Stmt* _temp556= _temp496; if( _temp556 == 0){ _throw(
Null_Exception);} _temp556;})))->encloser; while( dest_encloser != my_encloser) {
struct Cyc_Absyn_Stmt* next_encloser=( Cyc_ControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp557=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 45; _temp558;})); break;} my_encloser=
next_encloser;}} Cyc_ControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct
Cyc_Absyn_Stmt* _temp559= _temp496; if( _temp559 == 0){ _throw( Null_Exception);}
_temp559;}), env); return Cyc_ControlFlow_empty_flow(); _LL429: return Cyc_ControlFlow_empty_flow();
_LL431: _temp504= _temp502; goto _LL433; _LL433: Cyc_ControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp560= _temp504; if(
_temp560 == 0){ _throw( Null_Exception);} _temp560;}), env); return Cyc_ControlFlow_empty_flow();
_LL435: Cyc_ControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt*
_temp561= _temp506; if( _temp561 == 0){ _throw( Null_Exception);} _temp561;}),
Cyc_ControlFlow_expsynth_to_flow( Cyc_ControlFlow_cf_check_explist_unordered(
env, _temp508))); return Cyc_ControlFlow_empty_flow(); _LL437: Cyc_ControlFlow_cf_check_switch_clauses(
Cyc_ControlFlow_expsynth_to_flow( Cyc_ControlFlow_cf_check_exp( env, _temp512)),
_temp510); return Cyc_ControlFlow_empty_flow(); _LL439: Cyc_ControlFlow_cf_check_switch_clauses(
env, _temp514); return Cyc_ControlFlow_cf_check_stmt( env, _temp516); _LL441: {
struct Cyc_Absyn_Exp* e= _temp527->initializer; if( e == 0){ return Cyc_ControlFlow_cf_check_stmt(
Cyc_ControlFlow_add_unassigned( env, _temp527), _temp518);}{ struct Cyc_ControlFlow_FlowInfo
e_env= Cyc_ControlFlow_add_unassigned( env, _temp527); struct Cyc_ControlFlow_FlowInfo
s_env= Cyc_ControlFlow_remove_unassigned( Cyc_ControlFlow_expsynth_to_flow( Cyc_ControlFlow_cf_check_exp(
e_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp562= e; if( _temp562
== 0){ _throw( Null_Exception);} _temp562;}))), _temp527); return Cyc_ControlFlow_cf_check_stmt(
s_env, _temp518);}} _LL443: { struct Cyc_ControlFlow_FlowInfo e_env= env;{
struct Cyc_List_List* vds2= _temp547; for( 0; vds2 != 0; vds2= vds2->tl){ e_env=
Cyc_ControlFlow_add_unassigned( env,( struct Cyc_Absyn_Vardecl*) vds2->hd);}}{
struct Cyc_ControlFlow_FlowInfo s_env= Cyc_ControlFlow_expsynth_to_flow( Cyc_ControlFlow_cf_check_exp(
e_env, _temp540));{ struct Cyc_List_List* vds2= _temp547; for( 0; vds2 != 0;
vds2= vds2->tl){ s_env= Cyc_ControlFlow_remove_unassigned( env,( struct Cyc_Absyn_Vardecl*)
vds2->hd);}} return Cyc_ControlFlow_cf_check_stmt( s_env, _temp529);}} _LL445:
return Cyc_ControlFlow_cf_check_stmt( env, _temp551); _LL447:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp563=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp563=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp564=( char*)"ControlFlow::cf_check_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 73; _temp565;})};( struct _xenum_struct*)
_temp563;})); _LL407:;}} static void Cyc_ControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ Cyc_ControlFlow_iterate_cf_check= 1; Cyc_ControlFlow_iteration_num= 0;
while( Cyc_ControlFlow_iterate_cf_check) { ++ Cyc_ControlFlow_iteration_num; Cyc_ControlFlow_iterate_cf_check=
0; if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ if( Cyc_ControlFlow_cf_check_stmt_ft(
Cyc_ControlFlow_empty_flow(), fd->body)){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp566=( char*)"function may complete without returning a value";
struct _tagged_string _temp567; _temp567.curr= _temp566; _temp567.base= _temp566;
_temp567.last_plus_one= _temp566 + 48; _temp567;}));}} else{ Cyc_ControlFlow_cf_check_stmt(
Cyc_ControlFlow_empty_flow(), fd->body);}}} void Cyc_ControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp568=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp578; struct
Cyc_List_List* _temp580; struct _tuple0* _temp582; struct Cyc_List_List*
_temp584; struct _tagged_string* _temp586; _LL570: if(( unsigned int) _temp568 >
1u?(( struct _enum_struct*) _temp568)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL579:
_temp578=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp568)->f1;
goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 > 1u?((
struct _enum_struct*) _temp568)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL583:
_temp582=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp568)->f1;
goto _LL581; _LL581: _temp580=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp568)->f2; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp568 > 1u?(( struct _enum_struct*) _temp568)->tag == Cyc_Absyn_Namespace_d_tag:
0){ _LL587: _temp586=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp568)->f1; goto _LL585; _LL585: _temp584=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp568)->f2; goto _LL575;} else{ goto _LL576;}
_LL576: goto _LL577; _LL571: Cyc_ControlFlow_cf_check_fun( _temp578); goto
_LL569; _LL573: _temp584= _temp580; goto _LL575; _LL575: Cyc_ControlFlow_cf_check(
_temp584); goto _LL569; _LL577: goto _LL569; _LL569:;}}