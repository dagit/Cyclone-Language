#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2;}; struct _tuple1{ void* f1; struct
_tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2;}; struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3;}; struct _tuple5{ struct _tagged_string* f1; struct Cyc_List_List* f2;};
struct _tuple6{ struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple7{ struct
_tagged_string* f1; void* f2;}; struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2;}; struct _tuple9{ struct _tagged_string* f1; struct Cyc_List_List*
f2; struct Cyc_Dict_Dict* f3;}; struct _tuple10{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4;
char f5; int f6;}; struct _tuple11{ struct _tuple1* f1; void* f2; struct Cyc_Absyn_Exp*
f3;}; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2;};
struct _tuple13{ struct _tuple4* f1; struct Cyc_Absyn_Exp* f2;}; struct _tuple14{
struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Stmt* f3;};
struct _tuple15{ struct _tuple1* f1; void* f2;}; struct _tuple16{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2;}; struct _tuple17{ struct _tagged_string* f1;
struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause* f3;}; struct _tuple18{
struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v;}; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd(
struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{
char* tag;}; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
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
char* tag;}; extern void Cyc_List_iter_c( void(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_revappend(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_flatten( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strconcat( struct _tagged_string, struct _tagged_string); extern
struct _tagged_string Cyc_String_implode( struct Cyc_List_List* c); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col;};
typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag;}; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex; extern
void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc;}; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern int Cyc_Absyn_qvar_cmp(
struct _tuple1*, struct _tuple1*); extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_cstar_typ( void* t,
struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_void_star_typ(); extern void*
Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple1* name); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*,
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
char c, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp( int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct _tuple1*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple1*, void*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeof_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct
Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1,
struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct
Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt(
struct _tuple1*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string*
lab, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple1*); extern struct _tagged_string Cyc_Absynpp_prim2string( void* p);
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
char* tag;}; extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_unify( void*, void*); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment* loc, struct
Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); extern int Cyc_Toc_check_for_null; int Cyc_Toc_check_for_null=
0; char Cyc_Toc_Toc_Unimplemented_tag[ 18u]="Toc_Unimplemented"; struct Cyc_Toc_Toc_Unimplemented_struct{
char* tag;}; char Cyc_Toc_Toc_Impossible_tag[ 15u]="Toc_Impossible"; struct Cyc_Toc_Toc_Impossible_struct{
char* tag;}; static void* Cyc_Toc_unimp( struct _tagged_string s){({ struct
_tagged_string _temp0= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Toc_Toc_Unimplemented_struct*
_temp1=( struct Cyc_Toc_Toc_Unimplemented_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Toc_Toc_Unimplemented_struct));* _temp1=( struct Cyc_Toc_Toc_Unimplemented_struct){.tag=
Cyc_Toc_Toc_Unimplemented_tag};( struct _xenum_struct*) _temp1;}));} static void*
Cyc_Toc_toc_impos( struct _tagged_string s){({ struct _tagged_string _temp2= s;
fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp2.last_plus_one - _temp2.curr, _temp2.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({
struct Cyc_Toc_Toc_Impossible_struct* _temp3=( struct Cyc_Toc_Toc_Impossible_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Toc_Toc_Impossible_struct));* _temp3=(
struct Cyc_Toc_Toc_Impossible_struct){.tag= Cyc_Toc_Toc_Impossible_tag};( struct
_xenum_struct*) _temp3;}));} char Cyc_Toc_Match_error_tag[ 12u]="Match_error";
struct Cyc_Toc_Match_error_struct{ char* tag;}; static char _temp6[ 5u]="curr";
static struct _tagged_string Cyc_Toc_curr_string=( struct _tagged_string){
_temp6, _temp6, _temp6 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static char _temp9[ 14u]="last_plus_one"; static struct
_tagged_string Cyc_Toc_last_plus_one_string=( struct _tagged_string){ _temp9,
_temp9, _temp9 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp12[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string=( struct _tagged_string){ _temp12, _temp12,
_temp12 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string;
static char _temp15[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string=(
struct _tagged_string){ _temp15, _temp15, _temp15 + 15u}; static struct
_tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static char _temp18[ 13u]="_enum_struct"; static struct _tagged_string Cyc_Toc__enum_struct_string=(
struct _tagged_string){ _temp18, _temp18, _temp18 + 13u}; static struct
_tagged_string* Cyc_Toc__enum_struct_sp=& Cyc_Toc__enum_struct_string; static
char _temp21[ 14u]="_xenum_struct"; static struct _tagged_string Cyc_Toc__xenum_struct_string=(
struct _tagged_string){ _temp21, _temp21, _temp21 + 14u}; static struct
_tagged_string* Cyc_Toc__xenum_struct_sp=& Cyc_Toc__xenum_struct_string; static
char _temp24[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=( struct
_tagged_string){ _temp24, _temp24, _temp24 + 2u}; static struct _tagged_string*
Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp27[ 4u]="tag"; static struct
_tagged_string Cyc_Toc_tag_string=( struct _tagged_string){ _temp27, _temp27,
_temp27 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _temp30[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string=(
struct _tagged_string){ _temp30, _temp30, _temp30 + 14u}; static struct
_tagged_string* Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string; static
char _temp33[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string=(
struct _tagged_string){ _temp33, _temp33, _temp33 + 7u}; static struct
_tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string; static char _temp36[
14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp36, _temp36, _temp36 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp39[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp39, _temp39, _temp39 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp42[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp42, _temp42, _temp42 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp45[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp45, _temp45, _temp45 + 14u}; static struct
_tagged_string* Cyc_Toc__npop_handler_sp=& Cyc_Toc__npop_handler_string; static
char _temp48[ 16u]="Match_Exception"; static struct _tagged_string Cyc_Toc_Match_Exception_string=(
struct _tagged_string){ _temp48, _temp48, _temp48 + 16u}; static struct
_tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static char _temp51[ 15u]="Cyc_new_string"; static struct _tagged_string Cyc_Toc_Cyc_new_string_string=(
struct _tagged_string){ _temp51, _temp51, _temp51 + 15u}; static struct
_tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static int Cyc_Toc_is_nullable( void* t){ if( ! Cyc_Toc_check_for_null){ return
0;}{ void* _temp52= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp58;
struct Cyc_Absyn_Conref* _temp60; struct Cyc_Absyn_Tqual* _temp62; struct Cyc_Absyn_Conref*
_temp64; void* _temp66; void* _temp68; _LL54: if(( unsigned int) _temp52 > 5u?((
struct _enum_struct*) _temp52)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL59:
_temp58=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp52)->f1; _LL69: _temp68=( void*) _temp58.elt_typ; goto _LL67; _LL67:
_temp66=( void*) _temp58.rgn_typ; goto _LL65; _LL65: _temp64=( struct Cyc_Absyn_Conref*)
_temp58.nullable; goto _LL63; _LL63: _temp62=( struct Cyc_Absyn_Tqual*) _temp58.tq;
goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Conref*) _temp58.bounds; goto
_LL55;} else{ goto _LL56;} _LL56: goto _LL57; _LL55: { void* _temp70=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp64))->v; int _temp76; _LL72: if(( unsigned int) _temp70 > 1u?(( struct
_enum_struct*) _temp70)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL77: _temp76=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp70)->f1; goto _LL73;} else{ goto
_LL74;} _LL74: goto _LL75; _LL73: return _temp76; _LL75: return 0; _LL71:;}
_LL57:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp78=( char*)"is_nullable"; struct _tagged_string
_temp79; _temp79.curr= _temp78; _temp79.base= _temp78; _temp79.last_plus_one=
_temp78 + 12; _temp79;})); return 0; _LL53:;}} static int Cyc_Toc_really_is_nullable(
void* t){ int b= Cyc_Toc_check_for_null; Cyc_Toc_check_for_null= 1;{ int ans=
Cyc_Toc_is_nullable( t); Cyc_Toc_check_for_null= b; return ans;}} static struct
_tuple1* Cyc_Toc_collapse_qvar_tag( struct _tuple1* x, struct _tagged_string tag){
return({ struct _tuple1* _temp80=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp80->f1=(* x).f1; _temp80->f2=({ struct _tagged_string* _temp81=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp81[
0]= Cyc_String_strconcat(*(* x).f2, tag); _temp81;}); _temp80;});} static struct
Cyc_List_List* Cyc_Toc_added_decls= 0; static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts= ts->tl, i ++){ fs=({ struct Cyc_List_List*
_temp82=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp82->hd=( void*)({ struct _tuple4* _temp83=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp83->f1=({ struct _tagged_string* _temp84=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp84[ 0]=
xprintf("f%d", i); _temp84;}); _temp83->f2= Cyc_Absyn_empty_tqual(); _temp83->f3=(
void*) ts->hd; _temp83;}); _temp82->tl= fs; _temp82;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl* _temp85=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp85->sc=( void*) Cyc_Absyn_Public;
_temp85->name=({ struct Cyc_Core_Opt* _temp87=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp87->v=( void*)({ struct _tuple1* _temp88=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp88->f1=({ struct Cyc_Absyn_Abs_n_struct*
_temp89=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp89->tag= Cyc_Absyn_Abs_n_tag; _temp89->f1= 0;( void*) _temp89;}); _temp88->f2=
x; _temp88;}); _temp87;}); _temp85->tvs= 0; _temp85->fields=({ struct Cyc_Core_Opt*
_temp86=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp86->v=( void*) fs; _temp86;}); _temp85;}); Cyc_Toc_added_decls=({ struct
Cyc_List_List* _temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp91=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp91->tag= Cyc_Absyn_Struct_d_tag; _temp91->f1= sd;( void*) _temp91;}), 0);
_temp90->tl= Cyc_Toc_added_decls; _temp90;});}} static void Cyc_Toc_add_tagged_ptr_decl(
struct _tagged_string* x, void* t){ void* tptr= Cyc_Absyn_star_typ( t, Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); struct _tuple4* f1=({ struct _tuple4* _temp92=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp92->f1= Cyc_Toc_curr_sp;
_temp92->f2= Cyc_Absyn_empty_tqual(); _temp92->f3= tptr; _temp92;}); struct
_tuple4* f2=({ struct _tuple4* _temp93=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp93->f1= Cyc_Toc_base_sp; _temp93->f2= Cyc_Absyn_empty_tqual();
_temp93->f3= tptr; _temp93;}); struct _tuple4* f3=({ struct _tuple4* _temp94=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp94->f1= Cyc_Toc_last_plus_one_sp;
_temp94->f2= Cyc_Absyn_empty_tqual(); _temp94->f3= tptr; _temp94;}); struct Cyc_List_List*
fs=({ struct Cyc_List_List* _temp95=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp95->hd=( void*) f1; _temp95->tl=({ struct Cyc_List_List*
_temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*) f2; _temp96->tl=({ struct Cyc_List_List* _temp97=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp97->hd=( void*)
f3; _temp97->tl= 0; _temp97;}); _temp96;}); _temp95;}); struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl* _temp98=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp98->sc=( void*) Cyc_Absyn_Public;
_temp98->name=({ struct Cyc_Core_Opt* _temp100=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp100->v=( void*)({ struct _tuple1* _temp101=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp101->f1=({ struct Cyc_Absyn_Abs_n_struct*
_temp102=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp102->tag= Cyc_Absyn_Abs_n_tag; _temp102->f1= 0;( void*) _temp102;});
_temp101->f2= x; _temp101;}); _temp100;}); _temp98->tvs= 0; _temp98->fields=({
struct Cyc_Core_Opt* _temp99=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp99->v=( void*)(( struct Cyc_List_List*) fs); _temp99;});
_temp98;}); Cyc_Toc_added_decls=({ struct Cyc_List_List* _temp103=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp103->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp104=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp104->tag= Cyc_Absyn_Struct_d_tag;
_temp104->f1= sd;( void*) _temp104;}), 0); _temp103->tl= Cyc_Toc_added_decls;
_temp103;});} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int
Cyc_Toc_tuple_type_counter= 0; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* tts= Cyc_Toc_tuple_types;
for( 0; tts != 0; tts= tts->tl){ struct _tuple5 _temp107; struct Cyc_List_List*
_temp108; struct _tagged_string* _temp110; struct _tuple5* _temp105=( struct
_tuple5*) tts->hd; _temp107=* _temp105; _LL111: _temp110= _temp107.f1; goto
_LL109; _LL109: _temp108= _temp107.f2; goto _LL106; _LL106: { int okay= 1;
struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0? _temp108 != 0: 0; tqs= tqs->tl,
_temp108= _temp108->tl){ if( ! Cyc_Tcutil_unify((*(( struct _tuple6*) tqs->hd)).f2,(
void*) _temp108->hd)){ okay= 0; break;}} if(( ! okay? 1: tqs != 0)? 1: _temp108
!= 0){ continue;} return _temp110;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp112=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp112[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp112;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple6*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp113=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp113->hd=( void*)({ struct _tuple5* _temp114=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp114->f1= x; _temp114->f2=
ts; _temp114;}); _temp113->tl= Cyc_Toc_tuple_types; _temp113;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type( void* t, struct
Cyc_Absyn_Tqual* tq){{ void* _temp115= t; void* _temp121; void* _temp123; _LL117:
if(( unsigned int) _temp115 > 5u?(( struct _enum_struct*) _temp115)->tag == Cyc_Absyn_IntType_tag:
0){ _LL124: _temp123=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp115)->f1;
if( _temp123 == Cyc_Absyn_Unsigned){ goto _LL122;} else{ goto _LL119;} _LL122:
_temp121=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp115)->f2; if(
_temp121 == Cyc_Absyn_B1){ goto _LL118;} else{ goto _LL119;}} else{ goto _LL119;}
_LL119: goto _LL120; _LL118: return Cyc_Toc__tagged_string_sp; _LL120: goto
_LL116; _LL116:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types; for( 0;
tts != 0; tts= tts->tl){ struct _tuple7 _temp127; void* _temp128; struct
_tagged_string* _temp130; struct _tuple7* _temp125=( struct _tuple7*) tts->hd;
_temp127=* _temp125; _LL131: _temp130= _temp127.f1; goto _LL129; _LL129:
_temp128= _temp127.f2; goto _LL126; _LL126: if( ! Cyc_Tcutil_unify( t, _temp128)){
continue;} return _temp130;}}{ struct _tagged_string* x=({ struct _tagged_string*
_temp132=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp132[ 0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter ++));
_temp132;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp133=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp133->hd=( void*)({ struct _tuple7* _temp134=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp134->f1= x; _temp134->f2=
t; _temp134;}); _temp133->tl= Cyc_Toc_tagged_ptr_types; _temp133;}); return x;}}
static int Cyc_Toc_temp_var_counter= 0; static struct _tuple1* Cyc_Toc_temp_var(){
return({ struct _tuple1* _temp135=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp135->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp137=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp137->tag= Cyc_Absyn_Abs_n_tag;
_temp137->f1= 0;( void*) _temp137;}); _temp135->f2=({ struct _tagged_string*
_temp136=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp136[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp136;});
_temp135;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp138=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp138[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp138;});} static struct
Cyc_List_List* Cyc_Toc_temp_topdecls= 0; static char _temp141[ 10u]="_LongLong";
static struct _tagged_string Cyc_Toc_long_long_string=( struct _tagged_string){
_temp141, _temp141, _temp141 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp142={ 1u, 0}; static struct _tuple1 Cyc_Toc_longlong_v=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp142,.f2=& Cyc_Toc_long_long_string};
static struct Cyc_Absyn_TypedefType_struct _temp143={ 11u,& Cyc_Toc_longlong_v,
0, 0}; static void* Cyc_Toc_longlong_typ=( struct Cyc_Absyn_TypedefType_struct*)&
_temp143; static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple2* Cyc_Toc_arg_to_c(
struct _tuple2* a){ void* _temp146; struct Cyc_Absyn_Tqual* _temp148; struct Cyc_Core_Opt*
_temp150; struct _tuple2 _temp144=* a; _LL151: _temp150= _temp144.f1; goto
_LL149; _LL149: _temp148= _temp144.f2; goto _LL147; _LL147: _temp146= _temp144.f3;
goto _LL145; _LL145: return({ struct _tuple2* _temp152=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp152->f1= _temp150; _temp152->f2=
_temp148; _temp152->f3= Cyc_Toc_typ_to_c( _temp146); _temp152;});} static struct
_tuple6* Cyc_Toc_typ_to_c_f( struct _tuple6* x){ void* _temp155; struct Cyc_Absyn_Tqual*
_temp157; struct _tuple6 _temp153=* x; _LL158: _temp157= _temp153.f1; goto
_LL156; _LL156: _temp155= _temp153.f2; goto _LL154; _LL154: return({ struct
_tuple6* _temp159=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp159->f1= _temp157; _temp159->f2= Cyc_Toc_typ_to_c( _temp155); _temp159;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp160= t; struct Cyc_Absyn_Exp*
_temp168; struct Cyc_Absyn_Tqual* _temp170; void* _temp172; int _temp174; struct
Cyc_Core_Opt* _temp176; struct Cyc_Core_Opt _temp178; void* _temp179; void*
_temp181; _LL162: if(( unsigned int) _temp160 > 5u?(( struct _enum_struct*)
_temp160)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL173: _temp172=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp160)->f1; goto _LL171; _LL171: _temp170=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp160)->f2;
goto _LL169; _LL169: _temp168=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp160)->f3; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp160 > 5u?(( struct _enum_struct*) _temp160)->tag == Cyc_Absyn_Evar_tag: 0){
_LL182: _temp181=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp160)->f1; goto
_LL177; _LL177: _temp176=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp160)->f2; if( _temp176 == 0){ goto _LL166;} else{ _temp178=* _temp176;
_LL180: _temp179=( void*) _temp178.v; goto _LL175;} _LL175: _temp174=( int)((
struct Cyc_Absyn_Evar_struct*) _temp160)->f3; goto _LL165;} else{ goto _LL166;}
_LL166: goto _LL167; _LL163: return({ struct Cyc_Absyn_ArrayType_struct*
_temp183=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp183->tag= Cyc_Absyn_ArrayType_tag; _temp183->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp172); _temp183->f2= _temp170; _temp183->f3= _temp168;( void*) _temp183;});
_LL165: return Cyc_Toc_typ_to_c_array( _temp179); _LL167: return Cyc_Toc_typ_to_c(
t); _LL161:;} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp184= t; int
_temp226; struct Cyc_Core_Opt* _temp228; void* _temp230; int _temp232; struct
Cyc_Core_Opt* _temp234; struct Cyc_Core_Opt _temp236; void* _temp237; void*
_temp239; struct Cyc_Absyn_Tvar* _temp241; struct Cyc_Absyn_Enumdecl** _temp243;
struct Cyc_List_List* _temp245; struct _tuple1* _temp247; struct Cyc_Absyn_Xenumdecl**
_temp249; struct _tuple1* _temp251; struct Cyc_Absyn_PtrInfo _temp253; struct
Cyc_Absyn_Conref* _temp255; struct Cyc_Absyn_Tqual* _temp257; struct Cyc_Absyn_Conref*
_temp259; void* _temp261; void* _temp263; void* _temp265; void* _temp267; struct
Cyc_Absyn_Exp* _temp269; struct Cyc_Absyn_Tqual* _temp271; void* _temp273;
struct Cyc_Absyn_FnInfo _temp275; int _temp277; struct Cyc_List_List* _temp279;
void* _temp281; struct Cyc_Core_Opt* _temp283; struct Cyc_List_List* _temp285;
struct Cyc_List_List* _temp287; struct Cyc_Absyn_Structdecl** _temp289; struct
Cyc_List_List* _temp291; struct _tuple1* _temp293; struct Cyc_Core_Opt* _temp295;
struct Cyc_List_List* _temp297; struct _tuple1* _temp299; void* _temp301; void*
_temp303; struct Cyc_List_List* _temp305; _LL186: if( _temp184 == Cyc_Absyn_VoidType){
goto _LL187;} else{ goto _LL188;} _LL188: if(( unsigned int) _temp184 > 5u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_Evar_tag: 0){ _LL231: _temp230=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp184)->f1; goto _LL229; _LL229:
_temp228=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp184)->f2;
if( _temp228 == 0){ goto _LL227;} else{ goto _LL190;} _LL227: _temp226=( int)((
struct Cyc_Absyn_Evar_struct*) _temp184)->f3; goto _LL189;} else{ goto _LL190;}
_LL190: if(( unsigned int) _temp184 > 5u?(( struct _enum_struct*) _temp184)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL240: _temp239=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp184)->f1; goto _LL235; _LL235: _temp234=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp184)->f2; if( _temp234 == 0){ goto _LL192;} else{
_temp236=* _temp234; _LL238: _temp237=( void*) _temp236.v; goto _LL233;} _LL233:
_temp232=( int)(( struct Cyc_Absyn_Evar_struct*) _temp184)->f3; goto _LL191;}
else{ goto _LL192;} _LL192: if(( unsigned int) _temp184 > 5u?(( struct
_enum_struct*) _temp184)->tag == Cyc_Absyn_VarType_tag: 0){ _LL242: _temp241=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp184)->f1; goto
_LL193;} else{ goto _LL194;} _LL194: if(( unsigned int) _temp184 > 5u?(( struct
_enum_struct*) _temp184)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL248: _temp247=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp184)->f1; goto _LL246;
_LL246: _temp245=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp184)->f2; goto _LL244; _LL244: _temp243=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp184)->f3; goto _LL195;} else{ goto
_LL196;} _LL196: if(( unsigned int) _temp184 > 5u?(( struct _enum_struct*)
_temp184)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL252: _temp251=( struct _tuple1*)((
struct Cyc_Absyn_XenumType_struct*) _temp184)->f1; goto _LL250; _LL250: _temp249=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp184)->f2;
goto _LL197;} else{ goto _LL198;} _LL198: if(( unsigned int) _temp184 > 5u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL254:
_temp253=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp184)->f1; _LL264: _temp263=( void*) _temp253.elt_typ; goto _LL262; _LL262:
_temp261=( void*) _temp253.rgn_typ; goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Conref*)
_temp253.nullable; goto _LL258; _LL258: _temp257=( struct Cyc_Absyn_Tqual*)
_temp253.tq; goto _LL256; _LL256: _temp255=( struct Cyc_Absyn_Conref*) _temp253.bounds;
goto _LL199;} else{ goto _LL200;} _LL200: if(( unsigned int) _temp184 > 5u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_IntType_tag: 0){ _LL268:
_temp267=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp184)->f1; goto _LL266;
_LL266: _temp265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp184)->f2;
goto _LL201;} else{ goto _LL202;} _LL202: if( _temp184 == Cyc_Absyn_FloatType){
goto _LL203;} else{ goto _LL204;} _LL204: if( _temp184 == Cyc_Absyn_DoubleType){
goto _LL205;} else{ goto _LL206;} _LL206: if(( unsigned int) _temp184 > 5u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL274:
_temp273=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp184)->f1; goto
_LL272; _LL272: _temp271=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp184)->f2; goto _LL270; _LL270: _temp269=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp184)->f3; goto _LL207;} else{ goto _LL208;}
_LL208: if(( unsigned int) _temp184 > 5u?(( struct _enum_struct*) _temp184)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL276: _temp275=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp184)->f1; _LL286: _temp285=( struct Cyc_List_List*)
_temp275.tvars; goto _LL284; _LL284: _temp283=( struct Cyc_Core_Opt*) _temp275.effect;
goto _LL282; _LL282: _temp281=( void*) _temp275.ret_typ; goto _LL280; _LL280:
_temp279=( struct Cyc_List_List*) _temp275.args; goto _LL278; _LL278: _temp277=(
int) _temp275.varargs; goto _LL209;} else{ goto _LL210;} _LL210: if((
unsigned int) _temp184 > 5u?(( struct _enum_struct*) _temp184)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL288: _temp287=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp184)->f1; goto _LL211;} else{ goto _LL212;} _LL212: if(( unsigned int)
_temp184 > 5u?(( struct _enum_struct*) _temp184)->tag == Cyc_Absyn_StructType_tag:
0){ _LL294: _temp293=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp184)->f1; goto _LL292; _LL292: _temp291=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp184)->f2; goto _LL290; _LL290: _temp289=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp184)->f3;
goto _LL213;} else{ goto _LL214;} _LL214: if(( unsigned int) _temp184 > 5u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL300:
_temp299=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp184)->f1;
goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp184)->f2; goto _LL296; _LL296: _temp295=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp184)->f3; goto _LL215;} else{ goto _LL216;}
_LL216: if(( unsigned int) _temp184 > 5u?(( struct _enum_struct*) _temp184)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL302: _temp301=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp184)->f1; goto _LL217;} else{ goto _LL218;} _LL218: if( _temp184 == Cyc_Absyn_HeapRgn){
goto _LL219;} else{ goto _LL220;} _LL220: if( _temp184 == Cyc_Absyn_UnionType){
goto _LL221;} else{ goto _LL222;} _LL222: if(( unsigned int) _temp184 > 5u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL304:
_temp303=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp184)->f1; goto
_LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp184 > 5u?(( struct
_enum_struct*) _temp184)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL306: _temp305=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp184)->f1; goto
_LL225;} else{ goto _LL185;} _LL187: return t; _LL189: return Cyc_Absyn_VoidType;
_LL191: return Cyc_Toc_typ_to_c( _temp237); _LL193: return Cyc_Absyn_void_star_typ();
_LL195: if( _temp247 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp307=( char*)"anonymous enum"; struct
_tagged_string _temp308; _temp308.curr= _temp307; _temp308.base= _temp307;
_temp308.last_plus_one= _temp307 + 15; _temp308;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp309=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp309->tag= Cyc_Absyn_TypedefType_tag; _temp309->f1=( struct _tuple1*)({
struct _tuple1* _temp310= _temp247; if( _temp310 == 0){ _throw( Null_Exception);}
_temp310;}); _temp309->f2= 0; _temp309->f3=({ struct Cyc_Core_Opt* _temp311=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp311->v=(
void*) Cyc_Absyn_void_star_typ(); _temp311;});( void*) _temp309;}); _LL197:
return({ struct Cyc_Absyn_TypedefType_struct* _temp312=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp312->tag= Cyc_Absyn_TypedefType_tag;
_temp312->f1= _temp251; _temp312->f2= 0; _temp312->f3=({ struct Cyc_Core_Opt*
_temp313=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp313->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp),
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp313;});( void*) _temp312;});
_LL199: _temp263= Cyc_Toc_typ_to_c_array( _temp263);{ void* _temp314=( void*)
_temp255->v; void* _temp320; _LL316: if(( unsigned int) _temp314 > 1u?(( struct
_enum_struct*) _temp314)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL321: _temp320=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp314)->f1; if( _temp320 == Cyc_Absyn_Unknown_b){
goto _LL317;} else{ goto _LL318;}} else{ goto _LL318;} _LL318: goto _LL319;
_LL317: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp263,
_temp257); return Cyc_Absyn_strct( n);} _LL319: return Cyc_Absyn_star_typ(
_temp263, Cyc_Absyn_HeapRgn, _temp257); _LL315:;} _LL201: return t; _LL203:
return t; _LL205: return t; _LL207: return({ struct Cyc_Absyn_ArrayType_struct*
_temp322=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp322->tag= Cyc_Absyn_ArrayType_tag; _temp322->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp273); _temp322->f2= _temp271; _temp322->f3= _temp269;( void*) _temp322;});
_LL209: return({ struct Cyc_Absyn_FnType_struct* _temp323=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp323->tag= Cyc_Absyn_FnType_tag;
_temp323->f1=({ struct Cyc_Absyn_FnInfo _temp324; _temp324.tvars= 0; _temp324.effect=
0; _temp324.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp281); _temp324.args=((
struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp279); _temp324.varargs= _temp277;
_temp324;});( void*) _temp323;}); _LL211: _temp287=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_typ_to_c_f, _temp287);{ struct _tagged_string* n= Cyc_Toc_add_tuple_type(
_temp287); return Cyc_Absyn_strct( n);} _LL213: if( _temp293 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp325=( char*)"anonymous struct"; struct _tagged_string _temp326; _temp326.curr=
_temp325; _temp326.base= _temp325; _temp326.last_plus_one= _temp325 + 17;
_temp326;}));} return Cyc_Absyn_strctq(( struct _tuple1*)({ struct _tuple1*
_temp327= _temp293; if( _temp327 == 0){ _throw( Null_Exception);} _temp327;}));
_LL215: if( _temp295 == 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp328=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp329; _temp329.curr= _temp328; _temp329.base= _temp328;
_temp329.last_plus_one= _temp328 + 38; _temp329;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp330=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp330->tag= Cyc_Absyn_TypedefType_tag; _temp330->f1= _temp299; _temp330->f2=
0; _temp330->f3=({ struct Cyc_Core_Opt* _temp331=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp331->v=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp295->v); _temp331;});( void*) _temp330;}); _LL217: return Cyc_Absyn_void_star_typ();
_LL219: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp332=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 55; _temp333;})); _LL221: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp334=( char*)"uniontype"; struct _tagged_string _temp335; _temp335.curr=
_temp334; _temp335.base= _temp334; _temp335.last_plus_one= _temp334 + 10;
_temp335;})); _LL223: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp336=( char*)"Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 54; _temp337;})); _LL225: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp338=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 52; _temp339;})); _LL185:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp340= t; struct Cyc_Absyn_Exp* _temp346; struct Cyc_Absyn_Tqual*
_temp348; void* _temp350; _LL342: if(( unsigned int) _temp340 > 5u?(( struct
_enum_struct*) _temp340)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL351: _temp350=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp340)->f1; goto _LL349; _LL349:
_temp348=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp340)->f2; goto _LL347; _LL347: _temp346=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp340)->f3; goto _LL343;} else{ goto _LL344;}
_LL344: goto _LL345; _LL343: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp350, Cyc_Absyn_HeapRgn, _temp348), e, l); _LL345: return e; _LL341:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp352= t; struct Cyc_Absyn_Exp*
_temp358; struct Cyc_Absyn_Tqual* _temp360; void* _temp362; _LL354: if((
unsigned int) _temp352 > 5u?(( struct _enum_struct*) _temp352)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL363: _temp362=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp352)->f1;
goto _LL361; _LL361: _temp360=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp352)->f2; goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp352)->f3; goto _LL355;} else{ goto _LL356;}
_LL356: goto _LL357; _LL355: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp362, Cyc_Absyn_HeapRgn, _temp360), e, l); _LL357: return Cyc_Absyn_cast_exp(
t, e, l); _LL353:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp364= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp396; void* _temp398; void* _temp400; struct Cyc_Absyn_Exp*
_temp402; struct Cyc_Absyn_Tqual* _temp404; void* _temp406; struct Cyc_Absyn_FnInfo
_temp408; struct Cyc_List_List* _temp410; struct Cyc_Absyn_Structdecl** _temp412;
struct Cyc_List_List* _temp414; struct _tuple1* _temp416; struct Cyc_Absyn_Enumdecl**
_temp418; struct Cyc_List_List* _temp420; struct _tuple1* _temp422; struct Cyc_Absyn_Xenumdecl**
_temp424; struct _tuple1* _temp426; struct Cyc_Absyn_PtrInfo _temp428; void*
_temp430; _LL366: if( _temp364 == Cyc_Absyn_VoidType){ goto _LL367;} else{ goto
_LL368;} _LL368: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*)
_temp364)->tag == Cyc_Absyn_VarType_tag: 0){ _LL397: _temp396=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp364)->f1; goto _LL369;} else{ goto _LL370;}
_LL370: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*) _temp364)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL401: _temp400=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp364)->f1; goto _LL399; _LL399: _temp398=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp364)->f2; goto _LL371;} else{ goto _LL372;} _LL372: if( _temp364 == Cyc_Absyn_FloatType){
goto _LL373;} else{ goto _LL374;} _LL374: if( _temp364 == Cyc_Absyn_DoubleType){
goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int) _temp364 > 5u?((
struct _enum_struct*) _temp364)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL407:
_temp406=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp364)->f1; goto
_LL405; _LL405: _temp404=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp364)->f2; goto _LL403; _LL403: _temp402=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp364)->f3; goto _LL377;} else{ goto _LL378;}
_LL378: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*) _temp364)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL409: _temp408=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp364)->f1; goto _LL379;} else{ goto _LL380;}
_LL380: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*) _temp364)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL411: _temp410=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp364)->f1; goto _LL381;} else{ goto
_LL382;} _LL382: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*)
_temp364)->tag == Cyc_Absyn_StructType_tag: 0){ _LL417: _temp416=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp364)->f1; goto _LL415;
_LL415: _temp414=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp364)->f2; goto _LL413; _LL413: _temp412=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp364)->f3; goto _LL383;} else{ goto
_LL384;} _LL384: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*)
_temp364)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL423: _temp422=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp364)->f1; goto _LL421; _LL421: _temp420=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp364)->f2; goto
_LL419; _LL419: _temp418=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp364)->f3; goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int)
_temp364 > 5u?(( struct _enum_struct*) _temp364)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL427: _temp426=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp364)->f1; goto _LL425; _LL425: _temp424=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp364)->f2; goto _LL387;} else{ goto
_LL388;} _LL388: if( _temp364 == Cyc_Absyn_UnionType){ goto _LL389;} else{ goto
_LL390;} _LL390: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*)
_temp364)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL429: _temp428=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp364)->f1; goto _LL391;} else{ goto
_LL392;} _LL392: if(( unsigned int) _temp364 > 5u?(( struct _enum_struct*)
_temp364)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL431: _temp430=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp364)->f1; goto _LL393;} else{ goto
_LL394;} _LL394: goto _LL395; _LL367: return 1; _LL369: return 0; _LL371: return
1; _LL373: return 1; _LL375: return 0; _LL377: return Cyc_Toc_atomic_typ(
_temp406); _LL379: return 1; _LL381: for( 0; _temp410 != 0; _temp410= _temp410->tl){
if( ! Cyc_Toc_atomic_typ((*(( struct _tuple6*) _temp410->hd)).f2)){ return 0;}}
return 1; _LL383: return 0; _LL385: return 0; _LL387: return 0; _LL389: return 0;
_LL391: return 0; _LL393: return 0; _LL395:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp432= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp432.last_plus_one - _temp432.curr,
_temp432.curr);})); return 0; _LL365:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp433= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp439;
struct Cyc_Absyn_Conref* _temp441; struct Cyc_Absyn_Tqual* _temp443; struct Cyc_Absyn_Conref*
_temp445; void* _temp447; void* _temp449; _LL435: if(( unsigned int) _temp433 >
5u?(( struct _enum_struct*) _temp433)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL440: _temp439=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp433)->f1; _LL450: _temp449=( void*) _temp439.elt_typ; goto _LL448; _LL448:
_temp447=( void*) _temp439.rgn_typ; goto _LL446; _LL446: _temp445=( struct Cyc_Absyn_Conref*)
_temp439.nullable; goto _LL444; _LL444: _temp443=( struct Cyc_Absyn_Tqual*)
_temp439.tq; goto _LL442; _LL442: _temp441=( struct Cyc_Absyn_Conref*) _temp439.bounds;
goto _LL436;} else{ goto _LL437;} _LL437: goto _LL438; _LL436: { void* _temp451=
Cyc_Tcutil_compress( _temp449); _LL453: if( _temp451 == Cyc_Absyn_VoidType){
goto _LL454;} else{ goto _LL455;} _LL455: goto _LL456; _LL454: return 1; _LL456:
return 0; _LL452:;} _LL438: return 0; _LL434:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp457= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp465; struct Cyc_Absyn_Structdecl* _temp467;
struct Cyc_List_List* _temp468; struct _tuple1* _temp470; struct Cyc_Absyn_Structdecl**
_temp472; struct Cyc_List_List* _temp474; struct _tuple1* _temp476; _LL459: if((
unsigned int) _temp457 > 5u?(( struct _enum_struct*) _temp457)->tag == Cyc_Absyn_StructType_tag:
0){ _LL471: _temp470=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp457)->f1; goto _LL469; _LL469: _temp468=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp457)->f2; goto _LL466; _LL466: _temp465=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp457)->f3;
if( _temp465 == 0){ goto _LL461;} else{ _temp467=* _temp465; goto _LL460;}}
else{ goto _LL461;} _LL461: if(( unsigned int) _temp457 > 5u?(( struct
_enum_struct*) _temp457)->tag == Cyc_Absyn_StructType_tag: 0){ _LL477: _temp476=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp457)->f1; goto
_LL475; _LL475: _temp474=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp457)->f2; goto _LL473; _LL473: _temp472=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp457)->f3; if( _temp472 == 0){ goto
_LL462;} else{ goto _LL463;}} else{ goto _LL463;} _LL463: goto _LL464; _LL460:
if( _temp470 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp478=( char*)"StructType name not properly set";
struct _tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 33; _temp479;}));} if( _temp467->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp481= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp480=
_temp470; if( _temp480 == 0){ _throw( Null_Exception);} _temp480;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp481.last_plus_one - _temp481.curr, _temp481.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp467->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(*(( struct _tuple4*) fields->hd)).f1)
== 0){ return Cyc_Toc_is_void_star((*(( struct _tuple4*) fields->hd)).f3);}}((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp483= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp482=
_temp470; if( _temp482 == 0){ _throw( Null_Exception);} _temp482;})); struct
_tagged_string _temp484=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp483.last_plus_one - _temp483.curr, _temp483.curr, _temp484.last_plus_one -
_temp484.curr, _temp484.curr);})); return 0;} _LL462: if( _temp476 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp485=( char*)"StructType name not properly set"; struct _tagged_string
_temp486; _temp486.curr= _temp485; _temp486.base= _temp485; _temp486.last_plus_one=
_temp485 + 33; _temp486;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp488= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp487= _temp476; if( _temp487 == 0){ _throw( Null_Exception);}
_temp487;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp488.last_plus_one
- _temp488.curr, _temp488.curr);})); return 0; _LL464:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp489= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp489.last_plus_one - _temp489.curr,
_temp489.curr);})); return 0; _LL458:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp490=( void*) e->r; struct _tagged_string*
_temp498; struct Cyc_Absyn_Exp* _temp500; struct _tagged_string* _temp502;
struct Cyc_Absyn_Exp* _temp504; _LL492: if((( struct _enum_struct*) _temp490)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL501: _temp500=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp490)->f1; goto _LL499; _LL499:
_temp498=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp490)->f2; goto _LL493;} else{ goto _LL494;} _LL494: if((( struct
_enum_struct*) _temp490)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL505: _temp504=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp490)->f1;
goto _LL503; _LL503: _temp502=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp490)->f2; goto _LL495;} else{ goto _LL496;} _LL496: goto _LL497; _LL493:
return Cyc_Toc_is_poly_field(( void*)( _temp500->topt)->v, _temp498); _LL495: {
void* _temp506= Cyc_Tcutil_compress(( void*)( _temp504->topt)->v); struct Cyc_Absyn_PtrInfo
_temp512; struct Cyc_Absyn_Conref* _temp514; struct Cyc_Absyn_Tqual* _temp516;
struct Cyc_Absyn_Conref* _temp518; void* _temp520; void* _temp522; _LL508: if((
unsigned int) _temp506 > 5u?(( struct _enum_struct*) _temp506)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL513: _temp512=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp506)->f1; _LL523: _temp522=( void*) _temp512.elt_typ; goto _LL521; _LL521:
_temp520=( void*) _temp512.rgn_typ; goto _LL519; _LL519: _temp518=( struct Cyc_Absyn_Conref*)
_temp512.nullable; goto _LL517; _LL517: _temp516=( struct Cyc_Absyn_Tqual*)
_temp512.tq; goto _LL515; _LL515: _temp514=( struct Cyc_Absyn_Conref*) _temp512.bounds;
goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL509: return Cyc_Toc_is_poly_field(
_temp522, _temp502); _LL511:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp524= Cyc_Absynpp_typ2string(( void*)( _temp504->topt)->v);
xprintf("is_poly_project:  bad type %.*s", _temp524.last_plus_one - _temp524.curr,
_temp524.curr);})); return 0; _LL507:;} _LL497: return 0; _LL491:;} static char
_temp527[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp527, _temp527, _temp527 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp528={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp528,.f2=& Cyc_Toc_gc_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var, 0),({
struct Cyc_List_List* _temp529=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp529->hd=( void*) s; _temp529->tl= 0; _temp529;}), 0);}
static char _temp532[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp532, _temp532, _temp532 + 17u}; static struct Cyc_Absyn_Abs_n_struct
_temp533={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp533,.f2=& Cyc_Toc_gc_malloc_atomic_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var, 0),({
struct Cyc_List_List* _temp534=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp534->hd=( void*) s; _temp534->tl= 0; _temp534;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} else{ return
Cyc_Toc_malloc_ptr( s);}} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag=
0; struct Cyc_Toc_NullCheck_struct{ int tag; void* f1;}; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3;}; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2;}; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp536=({ struct _tuple0
_temp535; _temp535.f1= t1; _temp535.f2= t2; _temp535;}); void* _temp542; struct
Cyc_Absyn_PtrInfo _temp544; void* _temp546; struct Cyc_Absyn_PtrInfo _temp548;
_LL538: _LL547: _temp546= _temp536.f1; if(( unsigned int) _temp546 > 5u?((
struct _enum_struct*) _temp546)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL549:
_temp548=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp546)->f1; goto _LL543;} else{ goto _LL540;} _LL543: _temp542= _temp536.f2;
if(( unsigned int) _temp542 > 5u?(( struct _enum_struct*) _temp542)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL545: _temp544=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp542)->f1; goto _LL539;} else{ goto _LL540;} _LL540: goto _LL541; _LL539: {
int n1=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp548.nullable);
int n2=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp544.nullable);
void* b1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp548.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp544.bounds); struct Cyc_List_List* convs= 0;{ struct _tuple0 _temp551=({
struct _tuple0 _temp550; _temp550.f1= b1; _temp550.f2= b2; _temp550;}); void*
_temp561; struct Cyc_Absyn_Exp* _temp563; void* _temp565; struct Cyc_Absyn_Exp*
_temp567; void* _temp569; void* _temp571; void* _temp573; void* _temp575; struct
Cyc_Absyn_Exp* _temp577; void* _temp579; struct Cyc_Absyn_Exp* _temp581; void*
_temp583; _LL553: _LL566: _temp565= _temp551.f1; if(( unsigned int) _temp565 > 1u?((
struct _enum_struct*) _temp565)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL568:
_temp567=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp565)->f1;
goto _LL562;} else{ goto _LL555;} _LL562: _temp561= _temp551.f2; if((
unsigned int) _temp561 > 1u?(( struct _enum_struct*) _temp561)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL564: _temp563=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp561)->f1; goto _LL554;} else{ goto _LL555;} _LL555: _LL572: _temp571=
_temp551.f1; if( _temp571 == Cyc_Absyn_Unknown_b){ goto _LL570;} else{ goto
_LL557;} _LL570: _temp569= _temp551.f2; if( _temp569 == Cyc_Absyn_Unknown_b){
goto _LL556;} else{ goto _LL557;} _LL557: _LL576: _temp575= _temp551.f1; if((
unsigned int) _temp575 > 1u?(( struct _enum_struct*) _temp575)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL578: _temp577=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp575)->f1; goto _LL574;} else{ goto _LL559;} _LL574: _temp573= _temp551.f2;
if( _temp573 == Cyc_Absyn_Unknown_b){ goto _LL558;} else{ goto _LL559;} _LL559:
_LL584: _temp583= _temp551.f1; if( _temp583 == Cyc_Absyn_Unknown_b){ goto _LL580;}
else{ goto _LL552;} _LL580: _temp579= _temp551.f2; if(( unsigned int) _temp579 >
1u?(( struct _enum_struct*) _temp579)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL582:
_temp581=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp579)->f1;
goto _LL560;} else{ goto _LL552;} _LL554: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List*
_temp585=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp585->hd=( void*)({ struct Cyc_Toc_NullCheck_struct* _temp586=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp586->tag= Cyc_Toc_NullCheck_tag;
_temp586->f1=( void*) t2;( void*) _temp586;}); _temp585->tl= convs; _temp585;});}
goto _LL552; _LL556: goto _LL552; _LL558: convs=({ struct Cyc_List_List*
_temp587=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp587->hd=( void*)({ struct Cyc_Toc_TagPtr_struct* _temp588=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp588->tag= Cyc_Toc_TagPtr_tag;
_temp588->f1=( void*) Cyc_Toc_typ_to_c( t1); _temp588->f2= _temp577;( void*)
_temp588;}); _temp587->tl= convs; _temp587;}); goto _LL552; _LL560: convs=({
struct Cyc_List_List* _temp589=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp589->hd=( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp590=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp590->tag= Cyc_Toc_UntagPtr_tag; _temp590->f1=( void*) Cyc_Toc_typ_to_c( t1);
_temp590->f2= _temp581; _temp590->f3= n2;( void*) _temp590;}); _temp589->tl=
convs; _temp589;}); goto _LL552; _LL552:;} return convs;} _LL541: return 0;
_LL537:;}} static char _temp593[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp593, _temp593, _temp593 + 7u}; static struct Cyc_Absyn_Abs_n_struct
_temp594={ 1u, 0}; static struct _tuple1 Cyc_Toc__throw_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp594,.f2=& Cyc_Toc__throw_string}; static
struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({ struct Cyc_List_List* _temp595=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp595->hd=(
void*) e; _temp595->tl= 0; _temp595;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c(
t); if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp596=( char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 44; _temp597;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp598= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp604; struct Cyc_Absyn_Conref* _temp606; struct Cyc_Absyn_Tqual*
_temp608; struct Cyc_Absyn_Conref* _temp610; void* _temp612; void* _temp614;
_LL600: if(( unsigned int) _temp598 > 5u?(( struct _enum_struct*) _temp598)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL605: _temp604=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp598)->f1; _LL615: _temp614=( void*)
_temp604.elt_typ; goto _LL613; _LL613: _temp612=( void*) _temp604.rgn_typ; goto
_LL611; _LL611: _temp610=( struct Cyc_Absyn_Conref*) _temp604.nullable; goto
_LL609; _LL609: _temp608=( struct Cyc_Absyn_Tqual*) _temp604.tq; goto _LL607;
_LL607: _temp606=( struct Cyc_Absyn_Conref*) _temp604.bounds; goto _LL601;}
else{ goto _LL602;} _LL602: goto _LL603; _LL601: elt_typ= _temp614;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp614, Cyc_Absyn_empty_tqual());
strt_typ= Cyc_Absyn_strct( n); goto _LL599;} _LL603: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp616=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp617;
_temp617.curr= _temp616; _temp617.base= _temp616; _temp617.last_plus_one=
_temp616 + 28; _temp617;})); _LL599:;} if( toplevel){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp618=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp618->tag= Cyc_Absyn_ArrayType_tag;
_temp618->f1=( void*) elt_typ; _temp618->f2= Cyc_Absyn_empty_tqual(); _temp618->f3=
sz;( void*) _temp618;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp620=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp620->tag= Cyc_Absyn_Var_d_tag; _temp620->f1= vd;( void*) _temp620;}), 0);
_temp619->tl= Cyc_Toc_temp_topdecls; _temp619;});{ struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp621=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp621->hd=( void*)({ struct _tuple8* _temp626=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp626->f1= 0; _temp626->f2= xexp; _temp626;});
_temp621->tl=({ struct Cyc_List_List* _temp622=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp622->hd=( void*)({ struct
_tuple8* _temp625=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp625->f1= 0; _temp625->f2= xexp; _temp625;}); _temp622->tl=({ struct Cyc_List_List*
_temp623=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp623->hd=( void*)({ struct _tuple8* _temp624=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp624->f1= 0; _temp624->f2= xplussz; _temp624;});
_temp623->tl= 0; _temp623;}); _temp622;}); _temp621;}), 0); return urm_exp;}}
else{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
b, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_signed_int_exp( szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp(
a, 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Absyn_empty_tqual()),( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, e, 0), s, 0); return Cyc_Absyn_stmt_exp( s, 0);}} static
struct Cyc_Absyn_Exp* Cyc_Toc_untag_ptr_conv( int toplevel, void* t1, struct Cyc_Absyn_Exp*
sz, int nullable, struct Cyc_Absyn_Exp* e){ if( toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp627=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp628; _temp628.curr= _temp627; _temp628.base= _temp627; _temp628.last_plus_one=
_temp627 + 45; _temp628;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); struct Cyc_Absyn_Exp* xcurr=
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xcurr, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_add_exp( xcurr, Cyc_Absyn_signed_int_exp( szvalue, 0), 0); struct
Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_gt_exp( e1, e2, 0); struct Cyc_Absyn_Exp*
e4= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( xcurr, Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), e3, 0); struct Cyc_Absyn_Exp* e5= ! nullable? e4:
Cyc_Absyn_and_exp( Cyc_Absyn_neq_exp( xcurr, Cyc_Absyn_uint_exp( 0, 0), 0), e4,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( e5, Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt(
0), 0), s, 0); s= Cyc_Absyn_declare_stmt( x, t1,( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_convert_exp(
int toplevel, void* t1, void* t2, struct Cyc_Absyn_Exp* e){{ struct Cyc_List_List*
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs= cs->tl){ void* _temp629=(
void*) cs->hd; void* _temp637; struct Cyc_Absyn_Exp* _temp639; void* _temp641;
int _temp643; struct Cyc_Absyn_Exp* _temp645; void* _temp647; _LL631: if(((
struct _enum_struct*) _temp629)->tag == Cyc_Toc_NullCheck_tag){ _LL638: _temp637=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp629)->f1; goto _LL632;} else{
goto _LL633;} _LL633: if((( struct _enum_struct*) _temp629)->tag == Cyc_Toc_TagPtr_tag){
_LL642: _temp641=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp629)->f1; goto
_LL640; _LL640: _temp639=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp629)->f2; goto _LL634;} else{ goto _LL635;} _LL635: if((( struct
_enum_struct*) _temp629)->tag == Cyc_Toc_UntagPtr_tag){ _LL648: _temp647=( void*)((
struct Cyc_Toc_UntagPtr_struct*) _temp629)->f1; goto _LL646; _LL646: _temp645=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*) _temp629)->f2; goto
_LL644; _LL644: _temp643=( int)(( struct Cyc_Toc_UntagPtr_struct*) _temp629)->f3;
goto _LL636;} else{ goto _LL630;} _LL632: e= Cyc_Toc_null_check_conv( toplevel,
_temp637, e); goto _LL630; _LL634: e= Cyc_Toc_tag_ptr_conv( toplevel, _temp641,
_temp639, e); goto _LL630; _LL636: e= Cyc_Toc_untag_ptr_conv( toplevel, _temp647,
_temp645, _temp643, e); goto _LL630; _LL630:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int to_pop_break; int
to_pop_continue; int to_pop_return; int to_pop_fallthru; int toplevel;}; typedef
struct Cyc_Toc_Env* Cyc_Toc_env; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp649=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp649->break_lab= 0; _temp649->continue_lab= 0;
_temp649->fallthru_info= 0; _temp649->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp649->to_pop_break= 0; _temp649->to_pop_continue= 0; _temp649->to_pop_return=
0; _temp649->to_pop_fallthru= 0; _temp649->toplevel= 1; _temp649;});} static
struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){ return({ struct
Cyc_Toc_Env* _temp650=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp650->break_lab= e->break_lab; _temp650->continue_lab= e->continue_lab;
_temp650->fallthru_info= e->fallthru_info; _temp650->varmap= e->varmap; _temp650->to_pop_break=
e->to_pop_break; _temp650->to_pop_continue= e->to_pop_continue; _temp650->to_pop_return=
e->to_pop_return; _temp650->to_pop_fallthru= e->to_pop_fallthru; _temp650->toplevel=
e->toplevel; _temp650;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp651=(* x).f1; struct
Cyc_List_List* _temp657; _LL653: if(( unsigned int) _temp651 > 1u?(( struct
_enum_struct*) _temp651)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL658: _temp657=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp651)->f1; goto
_LL654;} else{ goto _LL655;} _LL655: goto _LL656; _LL654:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp659=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp659=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=({ struct _tagged_string _temp660= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp660.last_plus_one -
_temp660.curr, _temp660.curr);})};( struct _xenum_struct*) _temp659;})); goto
_LL652; _LL656: goto _LL652; _LL652:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple1* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y);
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; ans->to_pop_break= 0; ans->to_pop_continue= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_try_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->to_pop_break ++; ans->to_pop_continue ++; ans->to_pop_return ++; ans->to_pop_fallthru
++; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* fallthru_vars= 0; for( 0; fallthru_binders
!= 0; fallthru_binders= fallthru_binders->tl){ fallthru_vars=({ struct Cyc_List_List*
_temp661=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp661->hd=( void*)(( struct Cyc_Absyn_Vardecl*) fallthru_binders->hd)->name;
_temp661->tl= fallthru_vars; _temp661;});} fallthru_vars=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fallthru_vars);{ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp662=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp662->v=(
void*) break_l; _temp662;}); ans->fallthru_info=({ struct Cyc_Core_Opt* _temp663=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp663->v=(
void*)({ struct _tuple9* _temp664=( struct _tuple9*) GC_malloc( sizeof( struct
_tuple9)); _temp664->f1= fallthru_l; _temp664->f2= fallthru_vars; _temp664->f3=
next_case_env->varmap; _temp664;}); _temp663;}); ans->to_pop_break= 0; ans->to_pop_fallthru=
0; return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp665=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp665->v=( void*)
break_l; _temp665;}); ans->fallthru_info= 0; ans->to_pop_break= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e,
struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp666=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp666->v=( void*)({
struct _tuple9* _temp667=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp667->f1= next_l; _temp667->f2= 0; _temp667->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp667;}); _temp666;}); ans->to_pop_break= 0; ans->to_pop_fallthru= 0; return
ans;} static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp668=( char*)"expecting a literal format string";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 34; _temp669;}));}{ struct _tagged_string
fmt_str;{ void* _temp670=( void*)(( struct Cyc_Absyn_Exp*) args->hd)->r; void*
_temp676; struct _tagged_string _temp678; int _temp680; _LL672: if((( struct
_enum_struct*) _temp670)->tag == Cyc_Absyn_Const_e_tag){ _LL677: _temp676=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp670)->f1; if(( unsigned int) _temp676 > 1u?((
struct _enum_struct*) _temp676)->tag == Cyc_Absyn_String_c_tag: 0){ _LL681:
_temp680=( int)(( struct Cyc_Absyn_String_c_struct*) _temp676)->f1; goto _LL679;
_LL679: _temp678=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp676)->f2; goto _LL673;} else{ goto _LL674;}} else{ goto _LL674;} _LL674:
goto _LL675; _LL673: fmt_str= _temp678; goto _LL671; _LL675: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp682=( char*)"expecting a literal format string"; struct _tagged_string
_temp683; _temp683.curr= _temp682; _temp683.base= _temp682; _temp683.last_plus_one=
_temp682 + 34; _temp683;})); _LL671:;}{ int len=( int)({ struct _tagged_string
_temp684= fmt_str;( unsigned int)( _temp684.last_plus_one - _temp684.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(( int)({ struct _tagged_string _temp685=
fmt_str; char* _temp687= _temp685.curr + i; if( _temp687 < _temp685.base? 1:
_temp687 >= _temp685.last_plus_one){ _throw( Null_Exception);}* _temp687;}) ==(
int)'%'){ i ++; if( i < len?( int)({ struct _tagged_string _temp688= fmt_str;
char* _temp690= _temp688.curr + i; if( _temp690 < _temp688.base? 1: _temp690 >=
_temp688.last_plus_one){ _throw( Null_Exception);}* _temp690;}) ==( int)'s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp691=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp691->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp691->tl= args; _temp691;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args= args->tl; typs= typs->tl;{
struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct
Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){ char c=({ struct
_tagged_string _temp692= fmt_str; char* _temp694= _temp692.curr + i; if(
_temp694 < _temp692.base? 1: _temp694 >= _temp692.last_plus_one){ _throw(
Null_Exception);}* _temp694;}); rev_fmt=({ struct Cyc_List_List* _temp695=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp695->hd=(
void*)(( int) c); _temp695->tl= rev_fmt; _temp695;}); if(( int) c !=( int)'%'){
continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp696=( char*)"bad format string"; struct
_tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 18; _temp697;}));}{ struct _tuple10 _temp700;
int _temp701; char _temp703; struct Cyc_List_List* _temp705; struct Cyc_List_List*
_temp707; struct Cyc_List_List* _temp709; struct Cyc_List_List* _temp711; struct
_tuple10* _temp698=( struct _tuple10*) x->v; _temp700=* _temp698; _LL712:
_temp711= _temp700.f1; goto _LL710; _LL710: _temp709= _temp700.f2; goto _LL708;
_LL708: _temp707= _temp700.f3; goto _LL706; _LL706: _temp705= _temp700.f4; goto
_LL704; _LL704: _temp703= _temp700.f5; goto _LL702; _LL702: _temp701= _temp700.f6;
goto _LL699; _LL699: i= _temp701 - 1; if(( int) _temp703 !=( int)'s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp713=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp713->hd=( void*) _temp711; _temp713->tl=({ struct Cyc_List_List* _temp714=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp714->hd=(
void*) _temp709; _temp714->tl=({ struct Cyc_List_List* _temp715=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp715->hd=( void*) _temp707;
_temp715->tl=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp716->hd=( void*) _temp705;
_temp716->tl= 0; _temp716;}); _temp715;}); _temp714;}); _temp713;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp717=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp717->hd=( void*)(( int) _temp703); _temp717->tl=
rev_fmt; _temp717;});{ struct Cyc_List_List* _temp718= _temp709; struct Cyc_List_List
_temp725; struct Cyc_List_List* _temp726; int _temp728; _LL720: if( _temp718 ==
0){ goto _LL722;} else{ _temp725=* _temp718; _LL729: _temp728=( int) _temp725.hd;
goto _LL727; _LL727: _temp726=( struct Cyc_List_List*) _temp725.tl; if( _temp726
== 0){ goto _LL724;} else{ goto _LL722;}} _LL724: if( _temp728 ==( int)'*'){
goto _LL721;} else{ goto _LL722;} _LL722: goto _LL723; _LL721: { struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp730=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp730->hd=( void*)({
struct _tuple11* _temp731=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp731->f1= temp; _temp731->f2=( void*) typs->hd; _temp731->f3=( struct Cyc_Absyn_Exp*)
args->hd; _temp731;}); _temp730->tl= rev_temps; _temp730;}); rev_result=({
struct Cyc_List_List* _temp732=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp732->hd=( void*) Cyc_Absyn_var_exp( temp, 0);
_temp732->tl= rev_result; _temp732;}); args= args->tl; typs= typs->tl; goto
_LL719;} _LL723: goto _LL719; _LL719:;}{ struct Cyc_List_List* _temp733=
_temp707; struct Cyc_List_List _temp740; struct Cyc_List_List* _temp741; struct
Cyc_List_List _temp743; struct Cyc_List_List* _temp744; int _temp746; int
_temp748; _LL735: if( _temp733 == 0){ goto _LL737;} else{ _temp740=* _temp733;
_LL749: _temp748=( int) _temp740.hd; goto _LL742; _LL742: _temp741=( struct Cyc_List_List*)
_temp740.tl; if( _temp741 == 0){ goto _LL737;} else{ _temp743=* _temp741; _LL747:
_temp746=( int) _temp743.hd; goto _LL745; _LL745: _temp744=( struct Cyc_List_List*)
_temp743.tl; if( _temp744 == 0){ goto _LL739;} else{ goto _LL737;}}} _LL739: if(
_temp748 ==( int)'.'? _temp746 ==( int)'*': 0){ goto _LL736;} else{ goto _LL737;}
_LL737: goto _LL738; _LL736: { struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp750=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp750->hd=( void*)({ struct _tuple11*
_temp751=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp751->f1=
temp; _temp751->f2=( void*) typs->hd; _temp751->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp751;}); _temp750->tl= rev_temps; _temp750;}); rev_result=({ struct Cyc_List_List*
_temp752=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp752->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp752->tl= rev_result;
_temp752;}); args= args->tl; typs= typs->tl; goto _LL734;} _LL738: goto _LL734;
_LL734:;} if(( int) _temp703 !=( int)'%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp753=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp753.last_plus_one
- _temp753.curr, _temp753.curr);}));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp754=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp754->hd=( void*)({ struct _tuple11*
_temp755=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp755->f1=
temp; _temp755->f2=( void*) typs->hd; _temp755->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp755;}); _temp754->tl= rev_temps; _temp754;}); rev_result=({ struct Cyc_List_List*
_temp756=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp756->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp756->tl= rev_result;
_temp756;}); args= args->tl; typs= typs->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp757=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp757->hd=( void*)(( int)'.'); _temp757->tl= rev_fmt; _temp757;}); rev_fmt=({
struct Cyc_List_List* _temp758=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp758->hd=( void*)(( int)'*'); _temp758->tl= rev_fmt;
_temp758;}); rev_fmt=({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp759->hd=( void*)(( int)'s');
_temp759->tl= rev_fmt; _temp759;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp760=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp760->hd=( void*)({ struct _tuple11*
_temp761=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp761->f1=
temp; _temp761->f2=( void*) typs->hd; _temp761->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp761;}); _temp760->tl= rev_temps; _temp760;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp762=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp762->hd=( void*) size_exp; _temp762->tl= rev_result; _temp762;});
rev_result=({ struct Cyc_List_List* _temp763=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp763->hd=( void*) cstring_exp; _temp763->tl=
rev_result; _temp763;}); args= args->tl; typs= typs->tl;}}}}}}{ struct Cyc_List_List*
es=({ struct Cyc_List_List* _temp764=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp764->hd=( void*) Cyc_Absyn_string_exp( 0, Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp764->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp764;}); if( aopt != 0){ es=({ struct Cyc_List_List* _temp765=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp765->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp765->tl=( struct Cyc_List_List*)
es; _temp765;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p,( struct Cyc_List_List*)
es, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps
!= 0; rev_temps= rev_temps->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple11*)
rev_temps->hd)).f1,(*(( struct _tuple11*) rev_temps->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple11*) rev_temps->hd)).f3, s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}}
static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp766=( char*)"Missing type in primop "; struct _tagged_string _temp767;
_temp767.curr= _temp766; _temp767.base= _temp766; _temp767.last_plus_one=
_temp766 + 24; _temp767;}));} return Cyc_Toc_typ_to_c(( void*)( e->topt)->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp768=( char*)"Missing type in primop "; struct _tagged_string _temp769;
_temp769.curr= _temp768; _temp769.base= _temp768; _temp769.last_plus_one=
_temp768 + 24; _temp769;}));} return( void*)( e->topt)->v;} static struct
_tuple6* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple6*
_temp770=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp770->f1=
Cyc_Absyn_empty_tqual(); _temp770->f2= Cyc_Toc_typ_to_c(( void*)( e->topt)->v);
_temp770;});} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static struct _tuple8* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple8*
_temp771=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp771->f1= 0;
_temp771->f2= e; _temp771;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct _tuple1* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, int
is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Absyn_empty_tqual());{ struct Cyc_Absyn_Exp* se= Cyc_Absyn_sizeof_exp(
struct_typ, 0); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic?
Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr( se), 0);}} else{ t= struct_typ;
eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_List_List* es){ struct Cyc_List_List* tqs=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, es); struct _tagged_string* n= Cyc_Toc_add_tuple_type( tqs);
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( es);{
int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); for( 0; es !=
0; es= es->tl, -- i){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) es->hd);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v):
0; s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,({
struct _tagged_string* _temp772=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp772[ 0]= xprintf("f%d", i); _temp772;}), 0),(
struct Cyc_Absyn_Exp*) es->hd, 0), 0), s, 0);}} return Cyc_Toc_make_struct( x,
Cyc_Absyn_strct( n), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_List_List*
dles, struct _tuple1* tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*, struct
_tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es= es->tl){ struct _tuple8 _temp775; struct Cyc_Absyn_Exp* _temp776;
struct Cyc_List_List* _temp778; struct _tuple8* _temp773=( struct _tuple8*) es->hd;
_temp775=* _temp773; _LL779: _temp778= _temp775.f1; goto _LL777; _LL777:
_temp776= _temp775.f2; goto _LL774; _LL774: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)( _temp776->topt)->v): 0; if( _temp778 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp780=(
char*)"empty designator list"; struct _tagged_string _temp781; _temp781.curr=
_temp780; _temp781.base= _temp780; _temp781.last_plus_one= _temp780 + 22;
_temp781;}));} if( _temp778->tl != 0){ struct _tuple1* y= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp( y, 0); for( 0; _temp778 != 0;
_temp778= _temp778->tl){ void* _temp782=( void*) _temp778->hd; struct
_tagged_string* _temp788; _LL784: if((( struct _enum_struct*) _temp782)->tag ==
Cyc_Absyn_FieldName_tag){ _LL789: _temp788=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp782)->f1; goto _LL785;} else{ goto _LL786;}
_LL786: goto _LL787; _LL785: if( Cyc_Toc_is_poly_field( struct_type, _temp788)){
yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp, _temp788, 0), yexp, 0),
0), s, 0); goto _LL783; _LL787:( void) _throw((( struct _xenum_struct*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp790=( char*)"array designator in struct"; struct _tagged_string _temp791;
_temp791.curr= _temp790; _temp791.base= _temp790; _temp791.last_plus_one=
_temp790 + 27; _temp791;}))); _LL783:;} Cyc_Toc_exp_to_c( nv, _temp776); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( yexp, _temp776, 0), 0), s, 0);} else{
void* _temp792=( void*) _temp778->hd; struct _tagged_string* _temp798; _LL794:
if((( struct _enum_struct*) _temp792)->tag == Cyc_Absyn_FieldName_tag){ _LL799:
_temp798=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp792)->f1; goto _LL795;} else{ goto _LL796;} _LL796: goto _LL797; _LL795:
Cyc_Toc_exp_to_c( nv, _temp776); if( Cyc_Toc_is_poly_field( struct_type,
_temp798)){ _temp776= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp776, 0);}
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,
_temp798, 0), _temp776, 0), 0), s, 0); goto _LL793; _LL797:( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp800=( char*)"array designator in struct"; struct
_tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 27; _temp801;}))); _LL793:;}}} return Cyc_Toc_make_struct(
x, Cyc_Absyn_strctq( tdn), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple12* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp802=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp802->tag= Cyc_Absyn_Increment_e_tag; _temp802->f1= e1; _temp802->f2=( void*)
incr;( void*) _temp802;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp803=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp813; struct Cyc_Absyn_Exp* _temp815; void* _temp817; struct _tagged_string*
_temp819; struct Cyc_Absyn_Exp* _temp821; _LL805: if((( struct _enum_struct*)
_temp803)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL814: _temp813=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp803)->f1; goto _LL806;} else{ goto
_LL807;} _LL807: if((( struct _enum_struct*) _temp803)->tag == Cyc_Absyn_Cast_e_tag){
_LL818: _temp817=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp803)->f1; goto
_LL816; _LL816: _temp815=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp803)->f2; goto _LL808;} else{ goto _LL809;} _LL809: if((( struct
_enum_struct*) _temp803)->tag == Cyc_Absyn_StructMember_e_tag){ _LL822: _temp821=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp803)->f1;
goto _LL820; _LL820: _temp819=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp803)->f2; goto _LL810;} else{ goto _LL811;} _LL811: goto _LL812; _LL806:((
void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp813, fs, f, f_env); goto _LL804; _LL808:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp815, fs, f, f_env); goto
_LL804; _LL810:( void*)( e1->r=( void*)(( void*) _temp821->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp823=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp823->hd=( void*) _temp819; _temp823->tl= fs;
_temp823;}), f, f_env); goto _LL804; _LL812: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp824= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp824.last_plus_one - _temp824.curr, _temp824.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_string*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)((
void*)( f( e1_copy, f_env))->r)); goto _LL804;} _LL804:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp825=( void*) s->r;
struct Cyc_Absyn_Exp* _temp835; struct Cyc_Absyn_Stmt* _temp837; struct Cyc_Absyn_Decl*
_temp839; struct Cyc_Absyn_Stmt* _temp841; struct Cyc_Absyn_Stmt* _temp843;
_LL827: if(( unsigned int) _temp825 > 1u?(( struct _enum_struct*) _temp825)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL836: _temp835=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp825)->f1; goto _LL828;} else{ goto _LL829;} _LL829:
if(( unsigned int) _temp825 > 1u?(( struct _enum_struct*) _temp825)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL840: _temp839=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp825)->f1; goto _LL838; _LL838: _temp837=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp825)->f2; goto _LL830;} else{ goto _LL831;}
_LL831: if(( unsigned int) _temp825 > 1u?(( struct _enum_struct*) _temp825)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL844: _temp843=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp825)->f1; goto _LL842; _LL842: _temp841=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp825)->f2; goto _LL832;}
else{ goto _LL833;} _LL833: goto _LL834; _LL828:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp835, fs, f, f_env); goto
_LL826; _LL830:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp837, fs, f, f_env); goto _LL826; _LL832:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp841, fs, f, f_env); goto
_LL826; _LL834:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp845= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp845.last_plus_one - _temp845.curr, _temp845.curr);})); goto _LL826; _LL826:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript( struct Cyc_Absyn_Exp*
arr, struct Cyc_Absyn_Exp* ind, struct Cyc_Absyn_Exp* esz, void* elt_typ, struct
Cyc_Absyn_Tqual* tq, int take_address){ unsigned int sz= Cyc_Evexp_eval_const_uint_exp(
esz); struct _tuple1* a= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
void* ta= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( elt_typ), tq); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_prim2_exp( Cyc_Absyn_Gte, Cyc_Absyn_var_exp( i, 0), Cyc_Absyn_uint_exp(
sz, 0), 0); struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); if( take_address){ e5= Cyc_Absyn_address_exp( e5, 0);}{ struct Cyc_Absyn_Stmt*
s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt( i, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) ind, s3, 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
a, ta,( struct Cyc_Absyn_Exp*) arr, s1, 0), 0);}} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp846= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp846.last_plus_one - _temp846.curr, _temp846.curr);})); return;}{ void*
old_typ=( void*)( e->topt)->v; void* _temp847= r; void* _temp915; void* _temp918;
struct _tagged_string _temp920; int _temp922; void* _temp924; void* _temp926;
struct _tuple1* _temp928; struct _tuple1* _temp930; struct Cyc_List_List*
_temp932; void* _temp934; void* _temp936; struct Cyc_Absyn_Exp* _temp938; struct
Cyc_Absyn_Exp* _temp940; struct Cyc_Core_Opt* _temp942; struct Cyc_Absyn_Exp*
_temp944; struct Cyc_Absyn_Exp* _temp946; struct Cyc_Absyn_Exp* _temp948; struct
Cyc_Absyn_Exp* _temp950; struct Cyc_Absyn_Exp* _temp952; struct Cyc_Absyn_Exp*
_temp954; struct Cyc_List_List* _temp956; struct Cyc_Absyn_Exp* _temp958; struct
Cyc_List_List* _temp960; struct Cyc_Absyn_Exp* _temp962; struct Cyc_Absyn_Exp*
_temp964; struct Cyc_Absyn_Exp* _temp966; struct Cyc_List_List* _temp968; struct
Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Exp**
_temp974; void* _temp975; void** _temp977; struct Cyc_Absyn_Exp* _temp978; void*
_temp980; struct Cyc_Absyn_Exp* _temp982; struct _tagged_string* _temp984;
struct Cyc_Absyn_Exp* _temp986; struct _tagged_string* _temp988; struct Cyc_Absyn_Exp*
_temp990; struct Cyc_Absyn_Exp* _temp992; struct Cyc_Absyn_Exp* _temp994; struct
Cyc_List_List* _temp996; struct Cyc_List_List* _temp998; struct _tuple2*
_temp1000; struct Cyc_List_List* _temp1002; int _temp1004; struct Cyc_Absyn_Exp*
_temp1006; struct Cyc_Absyn_Exp* _temp1008; struct Cyc_Absyn_Vardecl* _temp1010;
struct Cyc_Absyn_Structdecl* _temp1012; struct Cyc_List_List* _temp1014; struct
Cyc_Core_Opt* _temp1016; struct _tuple1* _temp1018; struct Cyc_Absyn_Enumfield*
_temp1020; struct Cyc_Absyn_Enumdecl* _temp1022; struct Cyc_List_List* _temp1024;
struct Cyc_Core_Opt* _temp1026; struct Cyc_Core_Opt* _temp1028; struct Cyc_Absyn_Enumfield*
_temp1030; struct Cyc_Absyn_Xenumdecl* _temp1032; struct Cyc_List_List*
_temp1034; struct Cyc_Core_Opt* _temp1036; struct Cyc_Absyn_Stmt* _temp1038;
struct Cyc_List_List* _temp1040; struct Cyc_Core_Opt* _temp1042; struct Cyc_Absyn_Fndecl*
_temp1044; struct Cyc_Absyn_Exp* _temp1046; _LL849: if((( struct _enum_struct*)
_temp847)->tag == Cyc_Absyn_Const_e_tag){ _LL916: _temp915=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp847)->f1; if( _temp915 == Cyc_Absyn_Null_c){ goto _LL850;} else{ goto
_LL851;}} else{ goto _LL851;} _LL851: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Const_e_tag){ _LL919: _temp918=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp847)->f1; if(( unsigned int) _temp918 > 1u?(( struct _enum_struct*)
_temp918)->tag == Cyc_Absyn_String_c_tag: 0){ _LL923: _temp922=( int)(( struct
Cyc_Absyn_String_c_struct*) _temp918)->f1; goto _LL921; _LL921: _temp920=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp918)->f2; goto
_LL917;} else{ goto _LL853;}} else{ goto _LL853;} _LL917: if( _temp922){ goto
_LL852;} else{ goto _LL853;} _LL853: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Const_e_tag){ _LL925: _temp924=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp847)->f1; goto _LL854;} else{ goto _LL855;} _LL855: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Var_e_tag){ _LL929: _temp928=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp847)->f1; goto _LL927; _LL927:
_temp926=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp847)->f2; goto _LL856;}
else{ goto _LL857;} _LL857: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL931: _temp930=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp847)->f1; goto _LL858;} else{ goto _LL859;} _LL859: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Primop_e_tag){ _LL935: _temp934=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp847)->f1; goto _LL933; _LL933:
_temp932=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp847)->f2;
goto _LL860;} else{ goto _LL861;} _LL861: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Increment_e_tag){ _LL939: _temp938=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp847)->f1; goto _LL937; _LL937:
_temp936=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp847)->f2; goto
_LL862;} else{ goto _LL863;} _LL863: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL945: _temp944=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp847)->f1; goto _LL943; _LL943: _temp942=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp847)->f2; goto
_LL941; _LL941: _temp940=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp847)->f3; goto _LL864;} else{ goto _LL865;} _LL865: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Conditional_e_tag){ _LL951: _temp950=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp847)->f1;
goto _LL949; _LL949: _temp948=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp847)->f2; goto _LL947; _LL947: _temp946=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp847)->f3; goto _LL866;} else{ goto _LL867;}
_LL867: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL955: _temp954=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp847)->f1; goto _LL953; _LL953: _temp952=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp847)->f2; goto _LL868;} else{ goto _LL869;}
_LL869: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL959: _temp958=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp847)->f1; goto _LL957; _LL957: _temp956=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp847)->f2; goto _LL870;} else{ goto _LL871;}
_LL871: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_FnCall_e_tag){
_LL963: _temp962=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp847)->f1; goto _LL961; _LL961: _temp960=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp847)->f2; goto _LL872;} else{ goto _LL873;}
_LL873: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_Throw_e_tag){
_LL965: _temp964=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp847)->f1; goto _LL874;} else{ goto _LL875;} _LL875: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL967:
_temp966=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp847)->f1; goto _LL876;} else{ goto _LL877;} _LL877: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL971: _temp970=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp847)->f1;
goto _LL969; _LL969: _temp968=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp847)->f2; goto _LL878;} else{ goto _LL879;} _LL879: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Cast_e_tag){ _LL976: _temp975=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp847)->f1; _temp977=&(( struct Cyc_Absyn_Cast_e_struct*)
_temp847)->f1; goto _LL973; _LL973: _temp972=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp847)->f2; _temp974=&(( struct Cyc_Absyn_Cast_e_struct*)
_temp847)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Address_e_tag){ _LL979: _temp978=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp847)->f1; goto
_LL882;} else{ goto _LL883;} _LL883: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Sizeof_e_tag){ _LL981: _temp980=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp847)->f1; goto _LL884;} else{ goto _LL885;} _LL885: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Deref_e_tag){ _LL983: _temp982=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp847)->f1; goto
_LL886;} else{ goto _LL887;} _LL887: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL987: _temp986=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp847)->f1; goto _LL985; _LL985:
_temp984=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp847)->f2; goto _LL888;} else{ goto _LL889;} _LL889: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL991: _temp990=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp847)->f1;
goto _LL989; _LL989: _temp988=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp847)->f2; goto _LL890;} else{ goto _LL891;} _LL891: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Subscript_e_tag){ _LL995: _temp994=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp847)->f1;
goto _LL993; _LL993: _temp992=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp847)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Tuple_e_tag){ _LL997: _temp996=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp847)->f1; goto
_LL894;} else{ goto _LL895;} _LL895: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL1001: _temp1000=( struct _tuple2*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp847)->f1; goto _LL999; _LL999: _temp998=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp847)->f2;
goto _LL896;} else{ goto _LL897;} _LL897: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Array_e_tag){ _LL1005: _temp1004=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp847)->f1; goto _LL1003; _LL1003: _temp1002=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp847)->f2; goto _LL898;} else{ goto _LL899;}
_LL899: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL1011: _temp1010=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp847)->f1; goto _LL1009; _LL1009: _temp1008=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp847)->f2; goto _LL1007; _LL1007:
_temp1006=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp847)->f3; goto _LL900;} else{ goto _LL901;} _LL901: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Struct_e_tag){ _LL1019: _temp1018=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp847)->f1; goto
_LL1017; _LL1017: _temp1016=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp847)->f2; goto _LL1015; _LL1015: _temp1014=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp847)->f3; goto _LL1013; _LL1013:
_temp1012=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp847)->f4; goto _LL902;} else{ goto _LL903;} _LL903: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Enum_e_tag){ _LL1029: _temp1028=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp847)->f1; goto
_LL1027; _LL1027: _temp1026=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp847)->f2; goto _LL1025; _LL1025: _temp1024=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp847)->f3; goto _LL1023; _LL1023: _temp1022=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp847)->f4;
goto _LL1021; _LL1021: _temp1020=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp847)->f5; goto _LL904;} else{ goto _LL905;} _LL905: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1037: _temp1036=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp847)->f1; goto
_LL1035; _LL1035: _temp1034=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp847)->f2; goto _LL1033; _LL1033: _temp1032=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp847)->f3; goto _LL1031; _LL1031:
_temp1030=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp847)->f4; goto _LL906;} else{ goto _LL907;} _LL907: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1039: _temp1038=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp847)->f1;
goto _LL908;} else{ goto _LL909;} _LL909: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_UnresolvedMem_e_tag){ _LL1043: _temp1042=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp847)->f1; goto _LL1041; _LL1041:
_temp1040=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp847)->f2; goto _LL910;} else{ goto _LL911;} _LL911: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1045: _temp1044=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp847)->f1;
goto _LL912;} else{ goto _LL913;} _LL913: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Fill_e_tag){ _LL1047: _temp1046=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Fill_e_struct*) _temp847)->f1; goto _LL914;} else{ goto _LL848;}
_LL850:{ void* _temp1048= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1054; struct Cyc_Absyn_Conref* _temp1056; struct Cyc_Absyn_Tqual* _temp1058;
struct Cyc_Absyn_Conref* _temp1060; void* _temp1062; void* _temp1064; _LL1050:
if(( unsigned int) _temp1048 > 5u?(( struct _enum_struct*) _temp1048)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1055: _temp1054=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1048)->f1; _LL1065: _temp1064=( void*)
_temp1054.elt_typ; goto _LL1063; _LL1063: _temp1062=( void*) _temp1054.rgn_typ;
goto _LL1061; _LL1061: _temp1060=( struct Cyc_Absyn_Conref*) _temp1054.nullable;
goto _LL1059; _LL1059: _temp1058=( struct Cyc_Absyn_Tqual*) _temp1054.tq; goto
_LL1057; _LL1057: _temp1056=( struct Cyc_Absyn_Conref*) _temp1054.bounds; goto
_LL1051;} else{ goto _LL1052;} _LL1052: goto _LL1053; _LL1051:{ void* _temp1066=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1056); struct
Cyc_Absyn_Exp* _temp1072; _LL1068: if( _temp1066 == Cyc_Absyn_Unknown_b){ goto
_LL1069;} else{ goto _LL1070;} _LL1070: if(( unsigned int) _temp1066 > 1u?((
struct _enum_struct*) _temp1066)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1073:
_temp1072=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1066)->f1;
goto _LL1071;} else{ goto _LL1067;} _LL1069: { struct _tuple8* zero=({ struct
_tuple8* _temp1074=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1074->f1= 0; _temp1074->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1074;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1075=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1075->hd=( void*) zero; _temp1075->tl=({ struct Cyc_List_List* _temp1076=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1076->hd=(
void*) zero; _temp1076->tl=({ struct Cyc_List_List* _temp1077=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1077->hd=( void*) zero;
_temp1077->tl= 0; _temp1077;}); _temp1076;}); _temp1075;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1064), Cyc_Absyn_empty_tqual())); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1067;} _LL1071:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1067; _LL1067:;} goto _LL1049; _LL1053:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1078= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1078.last_plus_one -
_temp1078.curr, _temp1078.curr);})); goto _LL1049; _LL1049:;} goto _LL848;
_LL852:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1079=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1079->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp1080=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1080->tag= Cyc_Absyn_Abs_n_tag;
_temp1080->f1= 0;( void*) _temp1080;}); _temp1079->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1079;}), 0),({ struct Cyc_List_List* _temp1081=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1081->hd=( void*) Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp1082=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1082->tag= Cyc_Absyn_Const_e_tag;
_temp1082->f1=( void*)({ struct Cyc_Absyn_String_c_struct* _temp1083=( struct
Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1083->tag= Cyc_Absyn_String_c_tag; _temp1083->f1= 0; _temp1083->f2=
_temp920;( void*) _temp1083;});( void*) _temp1082;}), 0); _temp1081->tl= 0;
_temp1081;}), 0))->r)); goto _LL848; _LL854: goto _LL848; _LL856:{ struct
_handler_cons _temp1084; _push_handler(& _temp1084);{ struct _xenum_struct*
_temp1085=( struct _xenum_struct*) setjmp( _temp1084.handler); if( ! _temp1085){(
void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict*
d, struct _tuple1* key)) Cyc_Dict_lookup)( nv->varmap, _temp928))->r));
_pop_handler();} else{ struct _xenum_struct* _temp1087= _temp1085; _LL1089: if(
_temp1087->tag == Cyc_Dict_Absent_tag){ goto _LL1090;} else{ goto _LL1091;}
_LL1091: goto _LL1092; _LL1090:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1093= Cyc_Absynpp_qvar2string( _temp928); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1093.last_plus_one - _temp1093.curr, _temp1093.curr);})); return; _LL1092:(
void) _throw( _temp1087); _LL1088:;}}} goto _LL848; _LL858:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1094=(
char*)"unknownid"; struct _tagged_string _temp1095; _temp1095.curr= _temp1094;
_temp1095.base= _temp1094; _temp1095.last_plus_one= _temp1094 + 10; _temp1095;}));
goto _LL848; _LL860: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp932); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp932);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp932);{ void* _temp1096= _temp934; _LL1098: if( _temp1096 == Cyc_Absyn_Size){
goto _LL1099;} else{ goto _LL1100;} _LL1100: if( _temp1096 == Cyc_Absyn_Printf){
goto _LL1101;} else{ goto _LL1102;} _LL1102: if( _temp1096 == Cyc_Absyn_Xprintf){
goto _LL1103;} else{ goto _LL1104;} _LL1104: if( _temp1096 == Cyc_Absyn_Fprintf){
goto _LL1105;} else{ goto _LL1106;} _LL1106: if( _temp1096 == Cyc_Absyn_Sscanf){
goto _LL1107;} else{ goto _LL1108;} _LL1108: if( _temp1096 == Cyc_Absyn_Plus){
goto _LL1109;} else{ goto _LL1110;} _LL1110: if( _temp1096 == Cyc_Absyn_Minus){
goto _LL1111;} else{ goto _LL1112;} _LL1112: if( _temp1096 == Cyc_Absyn_Eq){
goto _LL1113;} else{ goto _LL1114;} _LL1114: if( _temp1096 == Cyc_Absyn_Neq){
goto _LL1115;} else{ goto _LL1116;} _LL1116: if( _temp1096 == Cyc_Absyn_Gt){
goto _LL1117;} else{ goto _LL1118;} _LL1118: if( _temp1096 == Cyc_Absyn_Gte){
goto _LL1119;} else{ goto _LL1120;} _LL1120: if( _temp1096 == Cyc_Absyn_Lt){
goto _LL1121;} else{ goto _LL1122;} _LL1122: if( _temp1096 == Cyc_Absyn_Lte){
goto _LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125; _LL1099: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*) _temp932->hd;{ void* _temp1126=( void*)( arg->topt)->v;
struct Cyc_Absyn_Exp* _temp1134; struct Cyc_Absyn_Tqual* _temp1136; void*
_temp1138; struct Cyc_Absyn_PtrInfo _temp1140; struct Cyc_Absyn_Conref*
_temp1142; struct Cyc_Absyn_Tqual* _temp1144; struct Cyc_Absyn_Conref* _temp1146;
void* _temp1148; void* _temp1150; _LL1128: if(( unsigned int) _temp1126 > 5u?((
struct _enum_struct*) _temp1126)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1139:
_temp1138=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1126)->f1; goto
_LL1137; _LL1137: _temp1136=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1126)->f2; goto _LL1135; _LL1135: _temp1134=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1126)->f3; goto _LL1129;} else{ goto
_LL1130;} _LL1130: if(( unsigned int) _temp1126 > 5u?(( struct _enum_struct*)
_temp1126)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1141: _temp1140=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1126)->f1;
_LL1151: _temp1150=( void*) _temp1140.elt_typ; goto _LL1149; _LL1149: _temp1148=(
void*) _temp1140.rgn_typ; goto _LL1147; _LL1147: _temp1146=( struct Cyc_Absyn_Conref*)
_temp1140.nullable; goto _LL1145; _LL1145: _temp1144=( struct Cyc_Absyn_Tqual*)
_temp1140.tq; goto _LL1143; _LL1143: _temp1142=( struct Cyc_Absyn_Conref*)
_temp1140.bounds; goto _LL1131;} else{ goto _LL1132;} _LL1132: goto _LL1133;
_LL1129:( void*)( e->r=( void*)(( void*) _temp1134->r)); goto _LL1127; _LL1131: {
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)( arg->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1127;}
_LL1133:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1152= Cyc_Absynpp_typ2string(( void*) typs->hd); struct
_tagged_string _temp1153= Cyc_Absynpp_typ2string(( void*)( arg->topt)->v);
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1152.last_plus_one
- _temp1152.curr, _temp1152.curr, _temp1153.last_plus_one - _temp1153.curr,
_temp1153.curr);})); return; _LL1127:;} goto _LL1097;} _LL1101:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp934, 0, _temp932, typs))->r)); goto
_LL1097; _LL1103:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp934,
0, _temp932, typs))->r)); goto _LL1097; _LL1105: if( _temp932 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1154=( char*)"fprintf without arguments"; struct _tagged_string _temp1155;
_temp1155.curr= _temp1154; _temp1155.base= _temp1154; _temp1155.last_plus_one=
_temp1154 + 26; _temp1155;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp934,({ struct Cyc_Core_Opt* _temp1156=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1156->v=( void*)(( struct Cyc_Absyn_Exp*)
_temp932->hd); _temp1156;}), _temp932->tl, typs->tl))->r)); goto _LL1097;
_LL1107: if( _temp932 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1157=( char*)"sscanf without arguments";
struct _tagged_string _temp1158; _temp1158.curr= _temp1157; _temp1158.base=
_temp1157; _temp1158.last_plus_one= _temp1157 + 25; _temp1158;}));}( void*)(((
struct Cyc_Absyn_Exp*) _temp932->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*) _temp932->hd)->r,(( struct
Cyc_Absyn_Exp*) _temp932->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1097;
_LL1109: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp932->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp932->tl)->hd; struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp,
0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1159=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1159->v=( void*) Cyc_Absyn_Plus; _temp1159;}), e2, 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1097; _LL1111: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp932->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp932->tl)->hd; void*
old_typ_e2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1160=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1160->v=( void*) Cyc_Absyn_Minus; _temp1160;}), e2,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1097; _LL1113: goto _LL1115; _LL1115:
goto _LL1117; _LL1117: goto _LL1119; _LL1119: goto _LL1121; _LL1121: goto
_LL1123; _LL1123: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp932->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp932->tl)->hd; void* t1=(
void*) old_typs->hd; void* t2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1097;} _LL1125: goto
_LL1097; _LL1097:;} goto _LL848;} _LL862: { void* e2_cyc_typ=( void*)( _temp938->topt)->v;
Cyc_Toc_exp_to_c( nv, _temp938); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){( void*)( _temp938->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp938->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( !
Cyc_Absyn_is_lvalue( _temp938)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp938, 0, Cyc_Toc_incr_lvalue, _temp936);( void*)( e->r=( void*)(( void*)
_temp938->r));} goto _LL848;} _LL864: { int e1_poly= Cyc_Toc_is_poly_project(
_temp944); void* e1_old_typ=( void*)( _temp944->topt)->v; void* e2_old_typ=(
void*)( _temp940->topt)->v; Cyc_Toc_exp_to_c( nv, _temp944); Cyc_Toc_exp_to_c(
nv, _temp940); if( _temp942 != 0?( void*) _temp942->v == Cyc_Absyn_Plus? 1:(
void*) _temp942->v == Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ(
e1_old_typ)){( void*)( _temp944->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp944->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp940->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp940->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp940->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp940->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp944)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple12*), struct
_tuple12* f_env)) Cyc_Toc_lvalue_assign)( _temp944, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple12* _temp1161=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12) * 1); _temp1161[ 0]=({ struct _tuple12 _temp1162; _temp1162.f1=
_temp942; _temp1162.f2= _temp940; _temp1162;}); _temp1161;}));( void*)( e->r=(
void*)(( void*) _temp944->r));} goto _LL848;} _LL866: Cyc_Toc_exp_to_c( nv,
_temp950); Cyc_Toc_exp_to_c( nv, _temp948); Cyc_Toc_exp_to_c( nv, _temp946);
goto _LL848; _LL868: Cyc_Toc_exp_to_c( nv, _temp954); Cyc_Toc_exp_to_c( nv,
_temp952); goto _LL848; _LL870: Cyc_Toc_exp_to_c( nv, _temp958);(( void(*)( void(*
f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct
Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp956); goto
_LL848; _LL872: Cyc_Toc_exp_to_c( nv, _temp962);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp960); goto _LL848; _LL874: Cyc_Toc_exp_to_c(
nv, _temp964);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp964), 0))->r)); goto _LL848; _LL876: Cyc_Toc_exp_to_c(
nv, _temp966); goto _LL848; _LL878: Cyc_Toc_exp_to_c( nv, _temp970);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); for( 0; _temp968 != 0; _temp968=
_temp968->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp968->hd); if( k != Cyc_Absyn_EffKind?
k != Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp970, 0))->r)); break;}} goto _LL848;} _LL880: { void* old_t2=( void*)((*
_temp974)->topt)->v; void* new_typ=* _temp977;* _temp977= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv,* _temp974);* _temp974= Cyc_Toc_convert_exp( nv->toplevel,
old_t2, new_typ,* _temp974); goto _LL848;} _LL882:{ void* _temp1163=( void*)
_temp978->r; struct Cyc_Absyn_Structdecl* _temp1171; struct Cyc_List_List*
_temp1173; struct Cyc_Core_Opt* _temp1175; struct _tuple1* _temp1177; struct Cyc_List_List*
_temp1179; _LL1165: if((( struct _enum_struct*) _temp1163)->tag == Cyc_Absyn_Struct_e_tag){
_LL1178: _temp1177=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1163)->f1; goto _LL1176; _LL1176: _temp1175=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1163)->f2; goto _LL1174; _LL1174:
_temp1173=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1163)->f3; goto _LL1172; _LL1172: _temp1171=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1163)->f4; goto _LL1166;} else{ goto
_LL1167;} _LL1167: if((( struct _enum_struct*) _temp1163)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1180: _temp1179=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1163)->f1; goto _LL1168;} else{ goto _LL1169;} _LL1169: goto _LL1170;
_LL1166: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1181= Cyc_Position_string_of_segment( _temp978->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1181.last_plus_one -
_temp1181.curr, _temp1181.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)( _temp978->topt)->v, 1, _temp1173, _temp1177))->r)); goto _LL1164;
_LL1168: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1182= Cyc_Position_string_of_segment( _temp978->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1182.last_plus_one -
_temp1182.curr, _temp1182.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1179))->r)); goto _LL1164; _LL1170: Cyc_Toc_exp_to_c( nv, _temp978);
if( ! Cyc_Absyn_is_lvalue( _temp978)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int),
int f_env)) Cyc_Toc_lvalue_assign)( _temp978, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*) _temp978->r));} goto _LL1164; _LL1164:;} goto
_LL848; _LL884:( void*)( e->r=( void*)({ struct Cyc_Absyn_Sizeof_e_struct*
_temp1183=( struct Cyc_Absyn_Sizeof_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeof_e_struct));
_temp1183->tag= Cyc_Absyn_Sizeof_e_tag; _temp1183->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp980);( void*) _temp1183;})); goto _LL848; _LL886: Cyc_Toc_exp_to_c( nv,
_temp982);{ void* _temp1184= Cyc_Tcutil_compress(( void*)( _temp982->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1190; struct Cyc_Absyn_Conref* _temp1192; struct
Cyc_Absyn_Tqual* _temp1194; struct Cyc_Absyn_Conref* _temp1196; void* _temp1198;
void* _temp1200; _LL1186: if(( unsigned int) _temp1184 > 5u?(( struct
_enum_struct*) _temp1184)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1191:
_temp1190=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1184)->f1; _LL1201: _temp1200=( void*) _temp1190.elt_typ; goto _LL1199;
_LL1199: _temp1198=( void*) _temp1190.rgn_typ; goto _LL1197; _LL1197: _temp1196=(
struct Cyc_Absyn_Conref*) _temp1190.nullable; goto _LL1195; _LL1195: _temp1194=(
struct Cyc_Absyn_Tqual*) _temp1190.tq; goto _LL1193; _LL1193: _temp1192=( struct
Cyc_Absyn_Conref*) _temp1190.bounds; goto _LL1187;} else{ goto _LL1188;} _LL1188:
goto _LL1189; _LL1187:{ void* _temp1202=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1192); struct Cyc_Absyn_Exp* _temp1208; _LL1204: if(
_temp1202 == Cyc_Absyn_Unknown_b){ goto _LL1205;} else{ goto _LL1206;} _LL1206:
if(( unsigned int) _temp1202 > 1u?(( struct _enum_struct*) _temp1202)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1209: _temp1208=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1202)->f1; goto _LL1207;} else{ goto _LL1203;}
_LL1205: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
void* ta1= Cyc_Toc_typ_to_c_array( _temp1200); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1194); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1194); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt(
test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s3, s5, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt( a, ta2,( struct
Cyc_Absyn_Exp*) a_init, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp982, s1, 0), 0))->r));
goto _LL1203;} _LL1207: { int nullable= Cyc_Toc_is_nullable(( void*)( _temp982->topt)->v);
if( nullable){ struct _tuple1* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)( _temp982->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( _temp1208->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp982, s3, 0), 0))->r));} goto _LL1203;} _LL1203:;} goto _LL1185; _LL1189:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1210=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1211; _temp1211.curr= _temp1210; _temp1211.base= _temp1210; _temp1211.last_plus_one=
_temp1210 + 29; _temp1211;})); goto _LL1185; _LL1185:;} goto _LL848; _LL888: Cyc_Toc_exp_to_c(
nv, _temp986); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL848; _LL890: Cyc_Toc_exp_to_c( nv, _temp990);
if( Cyc_Toc_is_nullable(( void*)( _temp990->topt)->v)){ struct _tuple1* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)( _temp990->topt)->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp988, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp990, s3, 0), 0))->r));} else{ if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}} goto _LL848; _LL892: { void*
e1_typ= Cyc_Tcutil_compress(( void*)( _temp994->topt)->v); Cyc_Toc_exp_to_c( nv,
_temp994); Cyc_Toc_exp_to_c( nv, _temp992);( void*)( _temp994->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*)
_temp994->r, 0), 0))->r));{ void* _temp1212= e1_typ; struct Cyc_List_List*
_temp1222; struct Cyc_Absyn_Exp* _temp1224; struct Cyc_Absyn_Tqual* _temp1226;
void* _temp1228; struct Cyc_Absyn_PtrInfo _temp1230; struct Cyc_Absyn_Conref*
_temp1232; struct Cyc_Absyn_Tqual* _temp1234; struct Cyc_Absyn_Conref* _temp1236;
void* _temp1238; void* _temp1240; _LL1214: if(( unsigned int) _temp1212 > 5u?((
struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1223:
_temp1222=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1212)->f1; goto _LL1215;} else{ goto _LL1216;} _LL1216: if(( unsigned int)
_temp1212 > 5u?(( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1229: _temp1228=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f1;
goto _LL1227; _LL1227: _temp1226=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1212)->f2; goto _LL1225; _LL1225: _temp1224=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1212)->f3; goto _LL1217;} else{ goto
_LL1218;} _LL1218: if(( unsigned int) _temp1212 > 5u?(( struct _enum_struct*)
_temp1212)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1231: _temp1230=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1212)->f1;
_LL1241: _temp1240=( void*) _temp1230.elt_typ; goto _LL1239; _LL1239: _temp1238=(
void*) _temp1230.rgn_typ; goto _LL1237; _LL1237: _temp1236=( struct Cyc_Absyn_Conref*)
_temp1230.nullable; goto _LL1235; _LL1235: _temp1234=( struct Cyc_Absyn_Tqual*)
_temp1230.tq; goto _LL1233; _LL1233: _temp1232=( struct Cyc_Absyn_Conref*)
_temp1230.bounds; goto _LL1219;} else{ goto _LL1220;} _LL1220: goto _LL1221;
_LL1215: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp992) +(
unsigned int) 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp994,({ struct _tagged_string* _temp1242=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1242[ 0]= xprintf("f%d",( int) i);
_temp1242;}), 0))->r)); goto _LL1213;} _LL1217:( void*)( e->r=( void*)(( void*)(
Cyc_Toc_known_size_subscript( _temp994, _temp992, _temp1224, _temp1228,
_temp1226, 0))->r)); goto _LL1213; _LL1219:{ void* _temp1243=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1232); struct Cyc_Absyn_Exp*
_temp1249; _LL1245: if(( unsigned int) _temp1243 > 1u?(( struct _enum_struct*)
_temp1243)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1250: _temp1249=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1243)->f1; goto _LL1246;} else{ goto
_LL1247;} _LL1247: if( _temp1243 == Cyc_Absyn_Unknown_b){ goto _LL1248;} else{
goto _LL1244;} _LL1246:( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp994, _temp992, _temp1249, _temp1240, _temp1234, 0))->r)); goto _LL1244;
_LL1248: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1240); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1234); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1234); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp992, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp994, s1, 0), 0))->r)); goto _LL1244;} _LL1244:;}
goto _LL1213; _LL1221:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1251=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1252; _temp1252.curr= _temp1251; _temp1252.base=
_temp1251; _temp1252.last_plus_one= _temp1251 + 49; _temp1252;})); goto _LL1213;
_LL1213:;} goto _LL848;} _LL894: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp996))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp996); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp996 != 0; _temp996= _temp996->tl, i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp996->hd);{ struct Cyc_List_List* des=({ struct
Cyc_List_List* _temp1253=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1253->hd=( void*)({ struct Cyc_Absyn_FieldName_struct* _temp1254=( struct
Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1254->tag= Cyc_Absyn_FieldName_tag; _temp1254->f1=({ struct _tagged_string*
_temp1255=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1255[ 0]= xprintf("f%d", i); _temp1255;});( void*) _temp1254;});
_temp1253->tl= 0; _temp1253;}); dles=({ struct Cyc_List_List* _temp1256=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1256->hd=( void*)({
struct _tuple8* _temp1257=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1257->f1=( struct Cyc_List_List*) des; _temp1257->f2=( struct Cyc_Absyn_Exp*)
_temp996->hd; _temp1257;}); _temp1256->tl= dles; _temp1256;});}}} dles=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp(
0, dles, 0), 0))->r));} goto _LL848; _LL896:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1258=( char*)"compoundlit";
struct _tagged_string _temp1259; _temp1259.curr= _temp1258; _temp1259.base=
_temp1258; _temp1259.last_plus_one= _temp1258 + 12; _temp1259;})); goto _LL848;
_LL898: if( nv->toplevel? 1: ! _temp1004){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1002, 0))->r));{ struct Cyc_List_List* dles= _temp1002; for( 0; dles !=
0; dles= dles->tl){ struct _tuple8 _temp1262; struct Cyc_Absyn_Exp* _temp1263;
struct Cyc_List_List* _temp1265; struct _tuple8* _temp1260=( struct _tuple8*)
dles->hd; _temp1262=* _temp1260; _LL1266: _temp1265= _temp1262.f1; goto _LL1264;
_LL1264: _temp1263= _temp1262.f2; goto _LL1261; _LL1261: Cyc_Toc_exp_to_c( nv,
_temp1263);}}} else{ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1002) - 1;{ struct Cyc_List_List* dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1002);
for( 0; dles != 0; dles= dles->tl){ struct _tuple8 _temp1269; struct Cyc_Absyn_Exp*
_temp1270; struct Cyc_List_List* _temp1272; struct _tuple8* _temp1267=( struct
_tuple8*) dles->hd; _temp1269=* _temp1267; _LL1273: _temp1272= _temp1269.f1;
goto _LL1271; _LL1271: _temp1270= _temp1269.f2; goto _LL1268; _LL1268: { struct
Cyc_Absyn_Exp* e_index; if( _temp1272 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if( _temp1272->tl != 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1274=( char*)"multiple designators in array";
struct _tagged_string _temp1275; _temp1275.curr= _temp1274; _temp1275.base=
_temp1274; _temp1275.last_plus_one= _temp1274 + 30; _temp1275;}));}{ void* d=(
void*) _temp1272->hd; void* _temp1276= d; struct Cyc_Absyn_Exp* _temp1282;
struct _tagged_string* _temp1284; _LL1278: if((( struct _enum_struct*) _temp1276)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1283: _temp1282=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1276)->f1; goto _LL1279;} else{ goto
_LL1280;} _LL1280: if((( struct _enum_struct*) _temp1276)->tag == Cyc_Absyn_FieldName_tag){
_LL1285: _temp1284=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1276)->f1; goto _LL1281;} else{ goto _LL1277;} _LL1279: Cyc_Toc_exp_to_c(
nv, _temp1282); e_index= _temp1282; goto _LL1277; _LL1281: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1286=( char*)"field name designators in array"; struct _tagged_string
_temp1287; _temp1287.curr= _temp1286; _temp1287.base= _temp1286; _temp1287.last_plus_one=
_temp1286 + 32; _temp1287;})); goto _LL1277; _LL1277:;}} Cyc_Toc_exp_to_c( nv,
_temp1270); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1270, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1288= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1294; struct Cyc_Absyn_Conref* _temp1296; struct Cyc_Absyn_Tqual* _temp1298;
struct Cyc_Absyn_Conref* _temp1300; void* _temp1302; void* _temp1304; _LL1290:
if(( unsigned int) _temp1288 > 5u?(( struct _enum_struct*) _temp1288)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1295: _temp1294=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1288)->f1; _LL1305: _temp1304=( void*)
_temp1294.elt_typ; goto _LL1303; _LL1303: _temp1302=( void*) _temp1294.rgn_typ;
goto _LL1301; _LL1301: _temp1300=( struct Cyc_Absyn_Conref*) _temp1294.nullable;
goto _LL1299; _LL1299: _temp1298=( struct Cyc_Absyn_Tqual*) _temp1294.tq; goto
_LL1297; _LL1297: _temp1296=( struct Cyc_Absyn_Conref*) _temp1294.bounds; goto
_LL1291;} else{ goto _LL1292;} _LL1292: goto _LL1293; _LL1291: old_elt_typ=
_temp1304; goto _LL1289; _LL1293: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1306=( char*)"exp_to_c:array expression doesn't have array type";
struct _tagged_string _temp1307; _temp1307.curr= _temp1306; _temp1307.base=
_temp1306; _temp1307.last_plus_one= _temp1306 + 50; _temp1307;})); goto _LL1289;
_LL1289:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp(
Cyc_Absyn_sizeof_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1002), 0), 0); struct Cyc_Absyn_Exp*
e1= Cyc_Toc_malloc_exp( old_elt_typ, e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r));}}} goto _LL848; _LL900: {
struct _tuple1* x1= _temp1010->name; int is_tagged_ptr= 0;{ void* _temp1308= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1314; struct Cyc_Absyn_Conref* _temp1316;
struct Cyc_Absyn_Tqual* _temp1318; struct Cyc_Absyn_Conref* _temp1320; void*
_temp1322; void* _temp1324; _LL1310: if(( unsigned int) _temp1308 > 5u?(( struct
_enum_struct*) _temp1308)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1315:
_temp1314=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1308)->f1; _LL1325: _temp1324=( void*) _temp1314.elt_typ; goto _LL1323;
_LL1323: _temp1322=( void*) _temp1314.rgn_typ; goto _LL1321; _LL1321: _temp1320=(
struct Cyc_Absyn_Conref*) _temp1314.nullable; goto _LL1319; _LL1319: _temp1318=(
struct Cyc_Absyn_Tqual*) _temp1314.tq; goto _LL1317; _LL1317: _temp1316=( struct
Cyc_Absyn_Conref*) _temp1314.bounds; goto _LL1311;} else{ goto _LL1312;} _LL1312:
goto _LL1313; _LL1311:{ void* _temp1326=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1316))->v; void*
_temp1332; _LL1328: if(( unsigned int) _temp1326 > 1u?(( struct _enum_struct*)
_temp1326)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1333: _temp1332=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1326)->f1; if( _temp1332 == Cyc_Absyn_Unknown_b){
goto _LL1329;} else{ goto _LL1330;}} else{ goto _LL1330;} _LL1330: goto _LL1331;
_LL1329: is_tagged_ptr= 1; goto _LL1327; _LL1331: goto _LL1327; _LL1327:;} goto
_LL1309; _LL1313:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1334=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1335; _temp1335.curr= _temp1334; _temp1335.base=
_temp1334; _temp1335.last_plus_one= _temp1334 + 42; _temp1335;})); goto _LL1309;
_LL1309:;}{ void* old_elt_typ=( void*)( _temp1006->topt)->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Absyn_empty_tqual());
Cyc_Toc_exp_to_c( nv, _temp1008);{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap(
nv, x1, Cyc_Absyn_varb_exp( x1,({ struct Cyc_Absyn_Local_b_struct* _temp1336=(
struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1336->tag= Cyc_Absyn_Local_b_tag; _temp1336->f1= _temp1010;( void*)
_temp1336;}), 0)); Cyc_Toc_exp_to_c( nv2, _temp1006);{ struct _tuple1* max= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea, eb, ec, Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( x1, 0), 0),
_temp1006, 0), 0); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1337=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1337->hd=( void*)({ struct _tuple11* _temp1338=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp1338->f1= max; _temp1338->f2= Cyc_Absyn_uint_t;
_temp1338->f3=( struct Cyc_Absyn_Exp*) _temp1008; _temp1338;}); _temp1337->tl= 0;
_temp1337;}); struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeof_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0)), 0); decls=({ struct Cyc_List_List* _temp1339=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1339->hd=( void*)({ struct
_tuple11* _temp1340=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp1340->f1= a; _temp1340->f2= ptr_typ; _temp1340->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1340;}); _temp1339->tl= decls; _temp1339;}); decls=({ struct Cyc_List_List*
_temp1341=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1341->hd=( void*)({ struct _tuple11* _temp1342=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp1342->f1= x1; _temp1342->f2= Cyc_Absyn_uint_t;
_temp1342->f3= 0; _temp1342;}); _temp1341->tl= decls; _temp1341;}); if(
is_tagged_ptr){ struct _tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp1343=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1343->hd=( void*)({ struct _tuple8* _temp1348=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1348->f1= 0; _temp1348->f2= Cyc_Absyn_var_exp( a,
0); _temp1348;}); _temp1343->tl=({ struct Cyc_List_List* _temp1344=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1344->hd=( void*)({ struct
_tuple8* _temp1347=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1347->f1= 0; _temp1347->f2= Cyc_Absyn_var_exp( a, 0); _temp1347;});
_temp1344->tl=({ struct Cyc_List_List* _temp1345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1345->hd=( void*)({ struct
_tuple8* _temp1346=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1346->f1= 0; _temp1346->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1346;}); _temp1345->tl= 0; _temp1345;});
_temp1344;}); _temp1343;}), 0); decls=({ struct Cyc_List_List* _temp1349=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1349->hd=(
void*)({ struct _tuple11* _temp1350=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp1350->f1= b; _temp1350->f2= t; _temp1350->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1350;}); _temp1349->tl= decls; _temp1349;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Exp* _temp1353; void*
_temp1355; struct _tuple1* _temp1357; struct _tuple11 _temp1351=*(( struct
_tuple11*) ds->hd); _LL1358: _temp1357= _temp1351.f1; goto _LL1356; _LL1356:
_temp1355= _temp1351.f2; goto _LL1354; _LL1354: _temp1353= _temp1351.f3; goto
_LL1352; _LL1352: s= Cyc_Absyn_declare_stmt( _temp1357, _temp1355, _temp1353, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r)); goto
_LL848;}}}} _LL902: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, _temp1014, _temp1018))->r));} else{ struct _tuple1* n= _temp1018;
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp1360= _temp1012 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1359= Cyc_Absynpp_qvar2string(
_temp1018); xprintf("no structdecl %.*s", _temp1359.last_plus_one - _temp1359.curr,
_temp1359.curr);})): _temp1012; if( _temp1360 == 0){ _throw( Null_Exception);}
_temp1360;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1014,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields= fields->tl){ struct _tuple13 _temp1363;
struct Cyc_Absyn_Exp* _temp1364; struct _tuple4* _temp1366; struct _tuple4
_temp1368; void* _temp1369; struct Cyc_Absyn_Tqual* _temp1371; struct
_tagged_string* _temp1373; struct _tuple13* _temp1361=( struct _tuple13*) fields->hd;
_temp1363=* _temp1361; _LL1367: _temp1366= _temp1363.f1; _temp1368=* _temp1366;
_LL1374: _temp1373= _temp1368.f1; goto _LL1372; _LL1372: _temp1371= _temp1368.f2;
goto _LL1370; _LL1370: _temp1369= _temp1368.f3; goto _LL1365; _LL1365: _temp1364=
_temp1363.f2; goto _LL1362; _LL1362: Cyc_Toc_exp_to_c( nv, _temp1364); if( Cyc_Toc_is_void_star(
_temp1369)){( void*)( _temp1364->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1364->r, 0), 0))->r));}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1014, 0), 0))->r));} goto _LL848; _LL904: { struct _tuple1* qv= _temp1020->name;
if( _temp1024 == 0){ if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp(
qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1022->fields
== 0? 0:( struct Cyc_List_List*)( _temp1022->fields)->v; while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs == 0){ tag_count ++;} fields= fields->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0), 0))->r));}} else{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1375=( char*)"_struct";
struct _tagged_string _temp1376; _temp1376.curr= _temp1375; _temp1376.base=
_temp1375; _temp1376.last_plus_one= _temp1375 + 8; _temp1376;}))); void*
pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ, Cyc_Absyn_empty_tqual()); if( nv->toplevel){
int tag_count= 0; struct Cyc_List_List* fields= _temp1022->fields == 0? 0:(
struct Cyc_List_List*)( _temp1022->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,((
struct Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs != 0){ tag_count ++;} fields= fields->tl;}{ struct Cyc_Absyn_Exp*
tag_exp= Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); struct Cyc_List_List*
dles=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Toc_add_designator, nv, _temp1024); dles=({ struct Cyc_List_List* _temp1377=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1377->hd=(
void*)({ struct _tuple8* _temp1378=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1378->f1= 0; _temp1378->f2= tag_exp; _temp1378;}); _temp1377->tl=
dles; _temp1377;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1379=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1379->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1380=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1380->tag= Cyc_Absyn_Var_d_tag; _temp1380->f1= vd;( void*) _temp1380;}), 0);
_temp1379->tl= Cyc_Toc_temp_topdecls; _temp1379;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( pstrct_typ, Cyc_Absyn_address_exp( xexp, 0), 0))->r));}}}
else{ struct Cyc_List_List* typs= _temp1020->typs; struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeof_exp(
strct_typ, 0); int atomic= 1; struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1381=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1381->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1382=( char*)"_tag"; struct _tagged_string
_temp1383; _temp1383.curr= _temp1382; _temp1383.base= _temp1382; _temp1383.last_plus_one=
_temp1382 + 5; _temp1383;})), 0), 0); _temp1381->tl= 0; _temp1381;});{ int i= 1;
for( 0; _temp1024 != 0;( _temp1024= _temp1024->tl, i ++), typs= typs->tl){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1024->hd; void*
field_t= Cyc_Toc_typ_to_c((*(( struct _tuple6*) typs->hd)).f2); if( ! Cyc_Toc_atomic_typ((
void*)( cur_e->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_t)){ cur_e= Cyc_Absyn_cast_exp( field_t, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp( xexp,({ struct
_tagged_string* _temp1384=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1384[ 0]= xprintf("f%d", i); _temp1384;}), 0), cur_e,
0); rev_assign=({ struct Cyc_List_List* _temp1385=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1385->hd=( void*) a; _temp1385->tl=(
struct Cyc_List_List*) rev_assign; _temp1385;});}}}{ struct Cyc_Absyn_Exp* e1=
Cyc_Absyn_cast_exp( pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr(
e2), 0); struct Cyc_Absyn_Stmt* final_cast= Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c( old_typ), xexp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(({ struct Cyc_List_List*
_temp1386=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1386->hd=( void*) final_cast; _temp1386->tl=( struct Cyc_List_List*)
rev_assign; _temp1386;})), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}}}
goto _LL848;} _LL906: { struct _tuple1* qv= _temp1030->name; struct _tuple1* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1387=( char*)"_struct"; struct _tagged_string
_temp1388; _temp1388.curr= _temp1387; _temp1388.base= _temp1387; _temp1388.last_plus_one=
_temp1387 + 8; _temp1388;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1389=( char*)"_tag"; struct
_tagged_string _temp1390; _temp1390.curr= _temp1389; _temp1390.base= _temp1389;
_temp1390.last_plus_one= _temp1389 + 5; _temp1390;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1034); dles=({ struct Cyc_List_List*
_temp1391=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1391->hd=( void*)({ struct _tuple8* _temp1392=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1392->f1= 0; _temp1392->f2= tag_exp; _temp1392;});
_temp1391->tl= dles; _temp1391;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1393=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1393->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1394=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1394->tag= Cyc_Absyn_Var_d_tag; _temp1394->f1= vd;( void*) _temp1394;}), 0);
_temp1393->tl= Cyc_Toc_temp_topdecls; _temp1393;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), Cyc_Absyn_address_exp(
xexp, 0), 0))->r));}} else{ int atomic= 1; struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeof_exp(
strct_typ, 0); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1034 != 0;
_temp1034= _temp1034->tl, i ++){ if( ! Cyc_Toc_atomic_typ(( void*)((( struct Cyc_Absyn_Exp*)
_temp1034->hd)->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_temp1034->hd);{ struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1395=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1395->hd=(
void*)({ struct Cyc_Absyn_FieldName_struct* _temp1396=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1396->tag= Cyc_Absyn_FieldName_tag;
_temp1396->f1=({ struct _tagged_string* _temp1397=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1397[ 0]= xprintf("f%d", i);
_temp1397;});( void*) _temp1396;}); _temp1395->tl= 0; _temp1395;}); dles=({
struct Cyc_List_List* _temp1398=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1398->hd=( void*)({ struct _tuple8* _temp1399=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1399->f1=( struct Cyc_List_List*)
des; _temp1399->f2=( struct Cyc_Absyn_Exp*) _temp1034->hd; _temp1399;});
_temp1398->tl= dles; _temp1398;});}}} dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( dles);{ struct Cyc_Absyn_Exp* e1= Cyc_Absyn_cast_exp(
pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr( e2), 0);
struct Cyc_List_List* d=({ struct Cyc_List_List* _temp1400=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1400->hd=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp1401=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1401->tag= Cyc_Absyn_FieldName_tag; _temp1401->f1= Cyc_Toc_tag_sp;( void*)
_temp1401;}); _temp1400->tl= 0; _temp1400;}); struct Cyc_List_List* dles2=({
struct Cyc_List_List* _temp1402=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1402->hd=( void*)({ struct _tuple8* _temp1403=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1403->f1=( struct Cyc_List_List*)
d; _temp1403->f2= tag_exp; _temp1403;}); _temp1402->tl= dles; _temp1402;});
struct Cyc_Absyn_Exp* strct_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles2, 0);
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_deref_exp( xexp, 0),
Cyc_Absyn_cast_exp( strct_typ, strct_exp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s1, Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), xexp, 0),
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}} goto _LL848;}
_LL908: Cyc_Toc_stmt_to_c( nv, _temp1038); goto _LL848; _LL910:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1404=( char*)"UnresolvedMem"; struct _tagged_string _temp1405; _temp1405.curr=
_temp1404; _temp1405.base= _temp1404; _temp1405.last_plus_one= _temp1404 + 14;
_temp1405;})); goto _LL848; _LL912:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1406=( char*)"codegen"; struct
_tagged_string _temp1407; _temp1407.curr= _temp1406; _temp1407.base= _temp1406;
_temp1407.last_plus_one= _temp1406 + 8; _temp1407;})); goto _LL848; _LL914:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1408=( char*)"fill"; struct _tagged_string _temp1409; _temp1409.curr=
_temp1408; _temp1409.base= _temp1408; _temp1409.last_plus_one= _temp1408 + 5;
_temp1409;})); goto _LL848; _LL848:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple14 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1410=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1442; struct Cyc_Absyn_Vardecl _temp1444; struct Cyc_Core_Opt* _temp1445;
int _temp1447; struct Cyc_Absyn_Exp* _temp1449; void* _temp1451; struct Cyc_Absyn_Tqual*
_temp1453; struct _tuple1* _temp1455; void* _temp1457; struct Cyc_Absyn_Vardecl*
_temp1459; int _temp1461; void* _temp1463; char _temp1465; struct _tagged_string
_temp1467; struct Cyc_Absyn_Enumfield* _temp1469; struct Cyc_Absyn_Enumdecl*
_temp1471; struct Cyc_List_List* _temp1473; struct Cyc_List_List* _temp1475;
struct Cyc_Core_Opt* _temp1477; struct _tuple1* _temp1479; struct Cyc_Absyn_Enumfield*
_temp1481; struct Cyc_Absyn_Xenumdecl* _temp1483; struct Cyc_List_List*
_temp1485; struct Cyc_List_List* _temp1487; struct _tuple1* _temp1489; struct
Cyc_List_List* _temp1491; struct Cyc_List_List* _temp1493; struct Cyc_List_List*
_temp1495; struct Cyc_Core_Opt* _temp1497; struct Cyc_Absyn_Structdecl*
_temp1499; struct Cyc_Absyn_Pat* _temp1501; struct _tuple1* _temp1503; struct
Cyc_List_List* _temp1505; struct Cyc_List_List* _temp1507; struct _tuple1*
_temp1509; struct Cyc_List_List* _temp1511; struct Cyc_List_List* _temp1513;
struct _tuple1* _temp1515; _LL1412: if( _temp1410 == Cyc_Absyn_Wild_p){ goto
_LL1413;} else{ goto _LL1414;} _LL1414: if(( unsigned int) _temp1410 > 2u?((
struct _enum_struct*) _temp1410)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1443:
_temp1442=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1410)->f1; _temp1444=* _temp1442; _LL1458: _temp1457=( void*) _temp1444.sc;
goto _LL1456; _LL1456: _temp1455=( struct _tuple1*) _temp1444.name; goto _LL1454;
_LL1454: _temp1453=( struct Cyc_Absyn_Tqual*) _temp1444.tq; goto _LL1452;
_LL1452: _temp1451=( void*) _temp1444.type; goto _LL1450; _LL1450: _temp1449=(
struct Cyc_Absyn_Exp*) _temp1444.initializer; goto _LL1448; _LL1448: _temp1447=(
int) _temp1444.shadow; goto _LL1446; _LL1446: _temp1445=( struct Cyc_Core_Opt*)
_temp1444.region; goto _LL1415;} else{ goto _LL1416;} _LL1416: if(( unsigned int)
_temp1410 > 2u?(( struct _enum_struct*) _temp1410)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1460: _temp1459=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1410)->f1; goto _LL1417;} else{ goto _LL1418;} _LL1418: if( _temp1410 ==
Cyc_Absyn_Null_p){ goto _LL1419;} else{ goto _LL1420;} _LL1420: if((
unsigned int) _temp1410 > 2u?(( struct _enum_struct*) _temp1410)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1464: _temp1463=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1410)->f1;
goto _LL1462; _LL1462: _temp1461=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1410)->f2; goto _LL1421;} else{ goto _LL1422;} _LL1422: if(( unsigned int)
_temp1410 > 2u?(( struct _enum_struct*) _temp1410)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1466: _temp1465=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1410)->f1;
goto _LL1423;} else{ goto _LL1424;} _LL1424: if(( unsigned int) _temp1410 > 2u?((
struct _enum_struct*) _temp1410)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1468:
_temp1467=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1410)->f1;
goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int) _temp1410 > 2u?((
struct _enum_struct*) _temp1410)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1480:
_temp1479=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1410)->f1;
goto _LL1478; _LL1478: _temp1477=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1410)->f2; goto _LL1476; _LL1476: _temp1475=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1410)->f3; goto _LL1474; _LL1474:
_temp1473=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1410)->f4;
goto _LL1472; _LL1472: _temp1471=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1410)->f5; goto _LL1470; _LL1470: _temp1469=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1410)->f6; goto _LL1427;} else{ goto
_LL1428;} _LL1428: if(( unsigned int) _temp1410 > 2u?(( struct _enum_struct*)
_temp1410)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1490: _temp1489=( struct
_tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1410)->f1; goto _LL1488;
_LL1488: _temp1487=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1410)->f2; goto _LL1486; _LL1486: _temp1485=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1410)->f3; goto _LL1484; _LL1484:
_temp1483=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1410)->f4; goto _LL1482; _LL1482: _temp1481=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1410)->f5; goto _LL1429;} else{ goto
_LL1430;} _LL1430: if(( unsigned int) _temp1410 > 2u?(( struct _enum_struct*)
_temp1410)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1492: _temp1491=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp1410)->f1; goto _LL1431;} else{ goto
_LL1432;} _LL1432: if(( unsigned int) _temp1410 > 2u?(( struct _enum_struct*)
_temp1410)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL1500: _temp1499=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1410)->f1; goto _LL1498; _LL1498:
_temp1497=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1410)->f2;
goto _LL1496; _LL1496: _temp1495=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1410)->f3; goto _LL1494; _LL1494: _temp1493=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1410)->f4; goto _LL1433;} else{ goto
_LL1434;} _LL1434: if(( unsigned int) _temp1410 > 2u?(( struct _enum_struct*)
_temp1410)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1502: _temp1501=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1410)->f1; goto _LL1435;} else{ goto
_LL1436;} _LL1436: if(( unsigned int) _temp1410 > 2u?(( struct _enum_struct*)
_temp1410)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1504: _temp1503=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1410)->f1; goto _LL1437;}
else{ goto _LL1438;} _LL1438: if(( unsigned int) _temp1410 > 2u?(( struct
_enum_struct*) _temp1410)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1510:
_temp1509=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1410)->f1;
goto _LL1508; _LL1508: _temp1507=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1410)->f2; goto _LL1506; _LL1506: _temp1505=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1410)->f3; goto _LL1439;} else{
goto _LL1440;} _LL1440: if(( unsigned int) _temp1410 > 2u?(( struct _enum_struct*)
_temp1410)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1516: _temp1515=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1410)->f1;
goto _LL1514; _LL1514: _temp1513=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1410)->f2; goto _LL1512; _LL1512: _temp1511=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1410)->f3; goto _LL1441;} else{
goto _LL1411;} _LL1413: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1411;
_LL1415: nv= Cyc_Toc_add_varmap( nv, _temp1455, r); s= Cyc_Absyn_goto_stmt(
succ_lab, 0); goto _LL1411; _LL1417: { struct _tuple1* v= Cyc_Toc_temp_var();
decls=({ struct Cyc_List_List* _temp1517=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1517->hd=( void*)({ struct _tuple15*
_temp1518=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1518->f1=
v; _temp1518->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Absyn_empty_tqual());
_temp1518;}); _temp1517->tl= decls; _temp1517;}); nv= Cyc_Toc_add_varmap( nv,
_temp1459->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1411;} _LL1419: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1411; _LL1421: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1463, _temp1461, 0), succ_lab, fail_lab); goto _LL1411;
_LL1423: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1465, 0),
succ_lab, fail_lab); goto _LL1411; _LL1425: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1467, 0), succ_lab, fail_lab); goto _LL1411; _LL1427: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1473);
struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1469->name,( struct
_tagged_string)({ char* _temp1519=( char*)"_struct"; struct _tagged_string
_temp1520; _temp1520.curr= _temp1519; _temp1520.base= _temp1519; _temp1520.last_plus_one=
_temp1519 + 8; _temp1520;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Absyn_empty_tqual()), r, 0);{
struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp1473); for( 0; rps != 0; rps= rps->tl, cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1521=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1521->hd=( void*)({ struct _tuple15* _temp1522=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1522->f1= v; _temp1522->f2= t2c;
_temp1522;}); _temp1521->tl= decls; _temp1521;});{ struct _tuple14 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp1523=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1523[ 0]= xprintf("f%d", cnt); _temp1523;}), 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp1524=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1524[ 0]= xprintf("f%d", cnt); _temp1524;}), 0), 0),
0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp1525=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1525->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1525->tl= ss;
_temp1525;}); succ_lab= lab;}}}} if( _temp1473 == 0){ s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_var_exp( _temp1479, 0), succ_lab, fail_lab);} else{ unsigned int
max_tag= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( _temp1471->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*)
fs->hd; if( f->typs == 0){ if( f->tag != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1526=( char*)"can't deal with explicit tags yet";
struct _tagged_string _temp1527; _temp1527.curr= _temp1526; _temp1527.base=
_temp1526; _temp1527.last_plus_one= _temp1526 + 34; _temp1527;}));} max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__enum_struct_sp),
Cyc_Absyn_empty_tqual()), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp(
e3, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1479,( struct _tagged_string)({ char* _temp1528=(
char*)"_tag"; struct _tagged_string _temp1529; _temp1529.curr= _temp1528;
_temp1529.base= _temp1528; _temp1529.last_plus_one= _temp1528 + 5; _temp1529;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag >( unsigned int) 0? Cyc_Absyn_and_exp(
e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}}
goto _LL1411;} _LL1429: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1485); struct _tuple1* xfstrct= Cyc_Toc_collapse_qvar_tag(
_temp1481->name,( struct _tagged_string)({ char* _temp1530=( char*)"_struct";
struct _tagged_string _temp1531; _temp1531.curr= _temp1530; _temp1531.base=
_temp1530; _temp1531.last_plus_one= _temp1530 + 8; _temp1531;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( xfstrct), Cyc_Absyn_empty_tqual()),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1485); for( 0; rps != 0; rps= rps->tl, cnt --){ struct
Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1532=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1532->hd=( void*)({ struct _tuple15*
_temp1533=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1533->f1=
v; _temp1533->f2= Cyc_Toc_typ_to_c( t2); _temp1533;}); _temp1532->tl= decls;
_temp1532;});{ struct _tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1534=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1534[ 0]= xprintf("f%d", cnt); _temp1534;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1535=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1535[ 0]= xprintf("f%d", cnt);
_temp1535;}), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0);
ss=({ struct Cyc_List_List* _temp1536=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1536->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1536->tl= ss; _temp1536;}); succ_lab= lab;}}}} if( _temp1485 == 0){
s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0),
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1489,( struct _tagged_string)({
char* _temp1537=( char*)"_tag"; struct _tagged_string _temp1538; _temp1538.curr=
_temp1537; _temp1538.base= _temp1537; _temp1538.last_plus_one= _temp1537 + 5;
_temp1538;})), 0), succ_lab, fail_lab);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Absyn_empty_tqual()),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structmember_exp( e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e=
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1489,( struct _tagged_string)({
char* _temp1539=( char*)"_tag"; struct _tagged_string _temp1540; _temp1540.curr=
_temp1539; _temp1540.base= _temp1539; _temp1540.last_plus_one= _temp1539 + 5;
_temp1540;})), 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0),
Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} goto
_LL1411;} _LL1431: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1491);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1491); for( 0; rps != 0; rps= rps->tl,
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct
_tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1541=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1541->hd=( void*)({ struct _tuple15* _temp1542=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1542->f1= v; _temp1542->f2= Cyc_Toc_typ_to_c(
t2); _temp1542;}); _temp1541->tl= decls; _temp1541;});{ struct _tuple14 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1543=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1543[ 0]= xprintf("f%d", cnt);
_temp1543;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1544=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1544[ 0]= xprintf("f%d", cnt); _temp1544;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1545=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1545->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1545->tl= ss; _temp1545;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1411;} _LL1433: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1493); for( 0; dlps != 0; dlps= dlps->tl){ struct _tuple16* tup=( struct
_tuple16*) dlps->hd; struct Cyc_Absyn_Pat* p2=(* tup).f2; struct _tagged_string*
f;{ void* _temp1546=( void*)((* tup).f1)->hd; struct _tagged_string* _temp1552;
_LL1548: if((( struct _enum_struct*) _temp1546)->tag == Cyc_Absyn_FieldName_tag){
_LL1553: _temp1552=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1546)->f1; goto _LL1549;} else{ goto _LL1550;} _LL1550: goto _LL1551;
_LL1549: f= _temp1552; goto _LL1547; _LL1551:( void) _throw(({ struct Cyc_Toc_Match_error_struct*
_temp1554=( struct Cyc_Toc_Match_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Match_error_struct));* _temp1554=( struct Cyc_Toc_Match_error_struct){.tag=
Cyc_Toc_Match_error_tag};( struct _xenum_struct*) _temp1554;})); _LL1547:;}{
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; void*
t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1555=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1555->hd=( void*)({ struct _tuple15*
_temp1556=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1556->f1=
v; _temp1556->f2= t2c; _temp1556;}); _temp1555->tl= decls; _temp1555;});{ struct
_tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path, f, 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1557=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1557->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1557->tl= ss; _temp1557;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1411;} _LL1435: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( _temp1501->topt)->v; decls=({ struct Cyc_List_List* _temp1558=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1558->hd=(
void*)({ struct _tuple15* _temp1559=( struct _tuple15*) GC_malloc( sizeof(
struct _tuple15)); _temp1559->f1= v; _temp1559->f2= Cyc_Toc_typ_to_c( t2);
_temp1559;}); _temp1558->tl= decls; _temp1558;});{ struct _tuple14 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path, 0), _temp1501,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0), s2, 0); if( Cyc_Toc_really_is_nullable(
t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);} else{ s= s3;} goto
_LL1411;}}} _LL1437: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1560=( char*)"unknownid";
struct _tagged_string _temp1561; _temp1561.curr= _temp1560; _temp1561.base=
_temp1560; _temp1561.last_plus_one= _temp1560 + 10; _temp1561;})); goto _LL1411;
_LL1439: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1562=( char*)"unknowncall"; struct
_tagged_string _temp1563; _temp1563.curr= _temp1562; _temp1563.base= _temp1562;
_temp1563.last_plus_one= _temp1562 + 12; _temp1563;})); goto _LL1411; _LL1441: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1564=( char*)"unknownfields"; struct _tagged_string
_temp1565; _temp1565.curr= _temp1564; _temp1565.base= _temp1564; _temp1565.last_plus_one=
_temp1564 + 14; _temp1565;})); goto _LL1411; _LL1411:;} return({ struct _tuple14
_temp1566; _temp1566.f1= nv; _temp1566.f2= decls; _temp1566.f3= s; _temp1566;});}
static struct _tuple17* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){
return({ struct _tuple17* _temp1567=( struct _tuple17*) GC_malloc( sizeof(
struct _tuple17)); _temp1567->f1= Cyc_Toc_fresh_label(); _temp1567->f2= Cyc_Toc_fresh_label();
_temp1567->f3= sc; _temp1567;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)( e->topt)->v; int
leave_as_switch;{ void* _temp1568= Cyc_Tcutil_compress( t); void* _temp1574;
void* _temp1576; _LL1570: if(( unsigned int) _temp1568 > 5u?(( struct
_enum_struct*) _temp1568)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1577: _temp1576=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1568)->f1; goto _LL1575; _LL1575:
_temp1574=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1568)->f2; goto
_LL1571;} else{ goto _LL1572;} _LL1572: goto _LL1573; _LL1571: leave_as_switch=
1; goto _LL1569; _LL1573: leave_as_switch= 0; goto _LL1569; _LL1569:;}{ struct
Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1= scs1->tl){ if(( struct Cyc_List_List*)(((
struct Cyc_Absyn_Switch_clause*) scs1->hd)->pat_vars)->v != 0){ leave_as_switch=
0; break;}}} if( leave_as_switch){ struct _tagged_string* next_l= Cyc_Toc_fresh_label();{
struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1= scs1->tl){ struct Cyc_Absyn_Stmt*
s=(( struct Cyc_Absyn_Switch_clause*) scs1->hd)->body;(( struct Cyc_Absyn_Switch_clause*)
scs1->hd)->body= Cyc_Absyn_label_stmt( next_l, s, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), s);}} return;}{
struct _tuple1* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple17*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2= lscs2->tl){ struct
Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple17*) lscs2->hd)).f3; struct
_tagged_string* fail_lab= lscs2->tl == 0? end_l:(*(( struct _tuple17*)( lscs2->tl)->hd)).f1;
struct _tagged_string* succ_lab=(*(( struct _tuple17*) lscs2->hd)).f2; if( sc->where_clause
== 0){ struct _tuple14 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1578=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1578->hd=( void*) res.f1;
_temp1578->tl= nvs; _temp1578;}); decls= res.f2; test_stmts=({ struct Cyc_List_List*
_temp1579=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1579->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple17*) lscs2->hd)).f1,
res.f3, 0); _temp1579->tl= test_stmts; _temp1579;});} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1580= sc->where_clause;
if( _temp1580 == 0){ _throw( Null_Exception);} _temp1580;}); struct
_tagged_string* intermed_lab= Cyc_Toc_fresh_label(); struct _tuple14 res= Cyc_Toc_xlate_pat(
nv, t, r, path, sc->pattern, intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c(
res.f1, e); nvs=({ struct Cyc_List_List* _temp1581=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1581->hd=( void*) res.f1;
_temp1581->tl= nvs; _temp1581;}); decls= res.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt(
e, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( intermed_lab, s2, 0);
test_stmts=({ struct Cyc_List_List* _temp1582=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1582->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple17*) lscs2->hd)).f1, Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0);
_temp1582->tl= test_stmts; _temp1582;});}}}}{ struct Cyc_Absyn_Stmt* test_stmt=
Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0; lscs->tl != 0; lscs= lscs->tl, nvs= nvs->tl){ struct Cyc_Toc_Env* e=(
struct Cyc_Toc_Env*) nvs->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple17*)
lscs->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( e,
end_l,(*(( struct _tuple17*)( lscs->tl)->hd)).f2,( struct Cyc_List_List*)(((*((
struct _tuple17*)( lscs->tl)->hd)).f3)->pat_vars)->v,( struct Cyc_Toc_Env*)( nvs->tl)->hd),
s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct
_tuple17*) lscs->hd)).f2, s, 0), 0);}{ struct Cyc_Toc_Env* e=( struct Cyc_Toc_Env*)
nvs->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple17*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( e, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple17*) lscs->hd)).f2, s, 0), 0);}}{
struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for(
decls; decls != 0; decls= decls->tl){ struct _tuple15 _temp1585; void* _temp1586;
struct _tuple1* _temp1588; struct _tuple15* _temp1583=( struct _tuple15*) decls->hd;
_temp1585=* _temp1583; _LL1589: _temp1588= _temp1585.f1; goto _LL1587; _LL1587:
_temp1586= _temp1585.f2; goto _LL1584; _LL1584: res= Cyc_Absyn_declare_stmt(
_temp1588, _temp1586, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)( e->topt)->v),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple18 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1590=(
void*) s->r; struct Cyc_Absyn_Exp* _temp1628; struct Cyc_Absyn_Stmt* _temp1630;
struct Cyc_Absyn_Stmt* _temp1632; struct Cyc_Absyn_Exp* _temp1634; struct Cyc_Absyn_Stmt*
_temp1636; struct Cyc_Absyn_Stmt* _temp1638; struct Cyc_Absyn_Exp* _temp1640;
struct Cyc_Absyn_Stmt* _temp1642; struct _tuple3 _temp1644; struct Cyc_Absyn_Stmt*
_temp1646; struct Cyc_Absyn_Exp* _temp1648; struct Cyc_Absyn_Stmt* _temp1650;
struct Cyc_Absyn_Stmt* _temp1652; struct Cyc_Absyn_Stmt* _temp1654; struct
_tagged_string* _temp1656; struct Cyc_Absyn_Stmt* _temp1658; struct _tuple3
_temp1660; struct Cyc_Absyn_Stmt* _temp1662; struct Cyc_Absyn_Exp* _temp1664;
struct _tuple3 _temp1666; struct Cyc_Absyn_Stmt* _temp1668; struct Cyc_Absyn_Exp*
_temp1670; struct Cyc_Absyn_Exp* _temp1672; struct Cyc_List_List* _temp1674;
struct Cyc_Absyn_Exp* _temp1676; struct Cyc_Absyn_Switch_clause** _temp1678;
struct Cyc_List_List* _temp1680; struct Cyc_Absyn_Stmt* _temp1682; struct Cyc_Absyn_Decl*
_temp1684; struct Cyc_Absyn_Stmt* _temp1686; struct Cyc_Absyn_Stmt* _temp1688;
struct Cyc_Absyn_Stmt* _temp1690; struct _tagged_string* _temp1692; struct
_tuple3 _temp1694; struct Cyc_Absyn_Stmt* _temp1696; struct Cyc_Absyn_Exp*
_temp1698; struct Cyc_Absyn_Stmt* _temp1700; struct Cyc_List_List* _temp1702;
struct Cyc_Absyn_Stmt* _temp1704; _LL1592: if( _temp1590 == Cyc_Absyn_Skip_s){
goto _LL1593;} else{ goto _LL1594;} _LL1594: if(( unsigned int) _temp1590 > 1u?((
struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1629:
_temp1628=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1590)->f1;
goto _LL1595;} else{ goto _LL1596;} _LL1596: if(( unsigned int) _temp1590 > 1u?((
struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1633:
_temp1632=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1590)->f1;
goto _LL1631; _LL1631: _temp1630=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1590)->f2; goto _LL1597;} else{ goto _LL1598;} _LL1598: if(( unsigned int)
_temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1635: _temp1634=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1590)->f1; goto _LL1599;} else{ goto _LL1600;} _LL1600: if(( unsigned int)
_temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1641: _temp1640=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1590)->f1; goto _LL1639; _LL1639: _temp1638=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1590)->f2; goto _LL1637; _LL1637:
_temp1636=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1590)->f3; goto _LL1601;} else{ goto _LL1602;} _LL1602: if(( unsigned int)
_temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1645: _temp1644=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1590)->f1; _LL1649: _temp1648= _temp1644.f1; goto _LL1647; _LL1647:
_temp1646= _temp1644.f2; goto _LL1643; _LL1643: _temp1642=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1590)->f2; goto _LL1603;} else{ goto
_LL1604;} _LL1604: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1651: _temp1650=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1590)->f1; goto _LL1605;} else{ goto
_LL1606;} _LL1606: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1653: _temp1652=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1590)->f1; goto _LL1607;} else{ goto
_LL1608;} _LL1608: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1657: _temp1656=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1590)->f1; goto _LL1655;
_LL1655: _temp1654=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1590)->f2; goto _LL1609;} else{ goto _LL1610;} _LL1610: if(( unsigned int)
_temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1673: _temp1672=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1590)->f1; goto _LL1667; _LL1667: _temp1666=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1590)->f2; _LL1671: _temp1670= _temp1666.f1; goto _LL1669; _LL1669:
_temp1668= _temp1666.f2; goto _LL1661; _LL1661: _temp1660=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1590)->f3; _LL1665: _temp1664= _temp1660.f1;
goto _LL1663; _LL1663: _temp1662= _temp1660.f2; goto _LL1659; _LL1659: _temp1658=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1590)->f4; goto
_LL1611;} else{ goto _LL1612;} _LL1612: if(( unsigned int) _temp1590 > 1u?((
struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1677:
_temp1676=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1590)->f1; goto _LL1675; _LL1675: _temp1674=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1590)->f2; goto _LL1613;} else{ goto
_LL1614;} _LL1614: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1681: _temp1680=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1590)->f1; goto _LL1679; _LL1679:
_temp1678=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1590)->f2; goto _LL1615;} else{ goto _LL1616;} _LL1616: if(( unsigned int)
_temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1685: _temp1684=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1590)->f1; goto _LL1683; _LL1683: _temp1682=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1590)->f2; goto _LL1617;} else{ goto
_LL1618;} _LL1618: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1687: _temp1686=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1590)->f1; goto _LL1619;} else{ goto
_LL1620;} _LL1620: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1689: _temp1688=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1590)->f1; goto _LL1621;} else{ goto
_LL1622;} _LL1622: if(( unsigned int) _temp1590 > 1u?(( struct _enum_struct*)
_temp1590)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1693: _temp1692=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1590)->f1; goto
_LL1691; _LL1691: _temp1690=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1590)->f2; goto _LL1623;} else{ goto _LL1624;} _LL1624: if(( unsigned int)
_temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1701: _temp1700=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1590)->f1; goto _LL1695; _LL1695: _temp1694=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1590)->f2; _LL1699: _temp1698= _temp1694.f1; goto _LL1697; _LL1697:
_temp1696= _temp1694.f2; goto _LL1625;} else{ goto _LL1626;} _LL1626: if((
unsigned int) _temp1590 > 1u?(( struct _enum_struct*) _temp1590)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1705: _temp1704=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1590)->f1; goto _LL1703; _LL1703: _temp1702=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1590)->f2; goto _LL1627;} else{ goto
_LL1591;} _LL1593: return; _LL1595: Cyc_Toc_exp_to_c( nv, _temp1628); return;
_LL1597: Cyc_Toc_stmt_to_c( nv, _temp1632); s= _temp1630; continue; _LL1599: {
struct Cyc_Core_Opt* topt= 0; if( _temp1634 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1706=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1706->v=( void*) Cyc_Toc_typ_to_c(( void*)( _temp1634->topt)->v); _temp1706;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1707=
_temp1634; if( _temp1707 == 0){ _throw( Null_Exception);} _temp1707;}));} if( nv->to_pop_return
!= 0){ struct Cyc_Absyn_Exp* call_exp= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1708=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1708->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp1709=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1709->tag= Cyc_Absyn_Abs_n_tag;
_temp1709->f1= 0;( void*) _temp1709;}); _temp1708->f2= Cyc_Toc__npop_handler_sp;
_temp1708;}), 0),({ struct Cyc_List_List* _temp1710=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1710->hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( nv->to_pop_return - 1), 0); _temp1710->tl= 0; _temp1710;}), 0);
struct Cyc_Absyn_Stmt* call_stmt= Cyc_Absyn_exp_stmt( call_exp, 0); if( topt !=
0){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*) topt->v, _temp1634, Cyc_Absyn_seq_stmt(
call_stmt, retn_stmt, 0), 0))->r));} else{( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
call_stmt, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}} return;} _LL1601:
Cyc_Toc_exp_to_c( nv, _temp1640); Cyc_Toc_stmt_to_c( nv, _temp1638); s=
_temp1636; continue; _LL1603: Cyc_Toc_exp_to_c( nv, _temp1648); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1642); return; _LL1605: if( nv->break_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(
nv->break_lab)->v, 0))->r));} if( nv->to_pop_break != 0){ struct Cyc_Absyn_Exp*
call_exp= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct _tuple1* _temp1711=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1711->f1=({ struct
Cyc_Absyn_Abs_n_struct* _temp1712=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1712->tag= Cyc_Absyn_Abs_n_tag;
_temp1712->f1= 0;( void*) _temp1712;}); _temp1711->f2= Cyc_Toc__npop_handler_sp;
_temp1711;}), 0),({ struct Cyc_List_List* _temp1713=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1713->hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( nv->to_pop_break - 1), 0); _temp1713->tl= 0; _temp1713;}), 0);
struct Cyc_Absyn_Stmt* call_stmt= Cyc_Absyn_exp_stmt( call_exp, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_seq_stmt( call_stmt, Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));} return; _LL1607: if( nv->continue_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)( nv->continue_lab)->v, 0))->r));}
if( nv->to_pop_continue != 0){ struct Cyc_Absyn_Exp* call_exp= Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(({ struct _tuple1* _temp1714=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp1714->f1=({ struct Cyc_Absyn_Abs_n_struct*
_temp1715=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp1715->tag= Cyc_Absyn_Abs_n_tag; _temp1715->f1= 0;( void*) _temp1715;});
_temp1714->f2= Cyc_Toc__npop_handler_sp; _temp1714;}), 0),({ struct Cyc_List_List*
_temp1716=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1716->hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( nv->to_pop_continue -
1), 0); _temp1716->tl= 0; _temp1716;}), 0); struct Cyc_Absyn_Stmt* call_stmt=
Cyc_Absyn_exp_stmt( call_exp, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
call_stmt, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} return; _LL1609:
return; _LL1611: Cyc_Toc_exp_to_c( nv, _temp1672); Cyc_Toc_exp_to_c( nv,
_temp1670); Cyc_Toc_exp_to_c( nv, _temp1664); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp1658); return; _LL1613: Cyc_Toc_xlate_switch( nv, s, _temp1676,
_temp1674); return; _LL1615: if( nv->fallthru_info == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1717=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp1718; _temp1718.curr=
_temp1717; _temp1718.base= _temp1717; _temp1718.last_plus_one= _temp1717 + 29;
_temp1718;})); return;}{ struct _tuple9 _temp1721; struct Cyc_Dict_Dict*
_temp1722; struct Cyc_List_List* _temp1724; struct _tagged_string* _temp1726;
struct _tuple9* _temp1719=( struct _tuple9*)( nv->fallthru_info)->v; _temp1721=*
_temp1719; _LL1727: _temp1726= _temp1721.f1; goto _LL1725; _LL1725: _temp1724=
_temp1721.f2; goto _LL1723; _LL1723: _temp1722= _temp1721.f3; goto _LL1720;
_LL1720: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1726, 0); struct
Cyc_List_List* vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1724); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1680); for( 0; vs2 != 0; vs2= vs2->tl, es2= es2->tl){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) es2->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple1* key)) Cyc_Dict_lookup)( _temp1722,( struct _tuple1*) vs2->hd),(
struct Cyc_Absyn_Exp*) es2->hd, 0), s2, 0);}( void*)( s->r=( void*)(( void*) s2->r));
if( nv->to_pop_fallthru != 0){ struct Cyc_Absyn_Exp* call_exp= Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(({ struct _tuple1* _temp1728=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp1728->f1=({ struct Cyc_Absyn_Abs_n_struct*
_temp1729=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp1729->tag= Cyc_Absyn_Abs_n_tag; _temp1729->f1= 0;( void*) _temp1729;});
_temp1728->f2= Cyc_Toc__npop_handler_sp; _temp1728;}), 0),({ struct Cyc_List_List*
_temp1730=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1730->hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( nv->to_pop_fallthru -
1), 0); _temp1730->tl= 0; _temp1730;}), 0); struct Cyc_Absyn_Stmt* call_stmt=
Cyc_Absyn_exp_stmt( call_exp, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
call_stmt, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} return;}} _LL1617:{
void* _temp1731=( void*) _temp1684->r; int _temp1737; struct Cyc_Absyn_Exp*
_temp1739; struct Cyc_Core_Opt* _temp1741; struct Cyc_Core_Opt* _temp1743;
struct Cyc_Absyn_Pat* _temp1745; _LL1733: if(( unsigned int) _temp1731 > 1u?((
struct _enum_struct*) _temp1731)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1746:
_temp1745=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1731)->f1;
goto _LL1744; _LL1744: _temp1743=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1731)->f2; goto _LL1742; _LL1742: _temp1741=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1731)->f3; goto _LL1740; _LL1740: _temp1739=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1731)->f4; goto
_LL1738; _LL1738: _temp1737=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1731)->f5;
goto _LL1734;} else{ goto _LL1735;} _LL1735: goto _LL1736; _LL1734: { int
var_case= 0;{ void* _temp1747=( void*) _temp1745->r; struct Cyc_Absyn_Vardecl*
_temp1753; _LL1749: if(( unsigned int) _temp1747 > 2u?(( struct _enum_struct*)
_temp1747)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1754: _temp1753=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1747)->f1; goto _LL1750;} else{ goto
_LL1751;} _LL1751: goto _LL1752; _LL1750: var_case= 1; _temp1753->initializer=(
struct Cyc_Absyn_Exp*) _temp1739;( void*)( _temp1684->r=( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1755=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1755->tag= Cyc_Absyn_Var_d_tag; _temp1755->f1= _temp1753;( void*) _temp1755;}));
goto _LL1748; _LL1752: goto _LL1748; _LL1748:;} if( var_case){ goto _LL1736;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp1745, _temp1741,
_temp1739, _temp1737, _temp1682))->r)); goto _LL1732;} _LL1736: { struct Cyc_List_List*
_temp1759; struct Cyc_Toc_Env* _temp1761; struct _tuple18 _temp1757= Cyc_Toc_decls_to_c(
nv,({ struct Cyc_List_List* _temp1756=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1756->hd=( void*) _temp1684; _temp1756->tl=
0; _temp1756;}), 0); _LL1762: _temp1761= _temp1757.f1; goto _LL1760; _LL1760:
_temp1759= _temp1757.f2; goto _LL1758; _LL1758: Cyc_Toc_stmt_to_c( _temp1761,
_temp1682); for( 0; _temp1759 != 0; _temp1759= _temp1759->tl){ _temp1682= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*) _temp1759->hd, _temp1682, 0);}( void*)( s->r=( void*)((
void*) _temp1682->r)); goto _LL1732;} _LL1732:;} return; _LL1619:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1763=( char*)"cut"; struct _tagged_string _temp1764; _temp1764.curr=
_temp1763; _temp1764.base= _temp1763; _temp1764.last_plus_one= _temp1763 + 4;
_temp1764;})); return; _LL1621:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1765=( char*)"splice"; struct _tagged_string
_temp1766; _temp1766.curr= _temp1765; _temp1766.base= _temp1765; _temp1766.last_plus_one=
_temp1765 + 7; _temp1766;})); return; _LL1623: s= _temp1690; continue; _LL1625:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1700); Cyc_Toc_exp_to_c( nv,
_temp1698); return; _LL1627: { struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp1767=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1767->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp1768=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1768->tag= Cyc_Absyn_Abs_n_tag;
_temp1768->f1= 0;( void*) _temp1768;}); _temp1767->f2= Cyc_Toc__push_handler_sp;
_temp1767;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1769=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1769->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp1770=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1770->tag= Cyc_Absyn_Abs_n_tag;
_temp1770->f1= 0;( void*) _temp1770;}); _temp1769->f2= Cyc_Toc_setjmp_sp;
_temp1769;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp1771=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1771->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp1772=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1772->tag= Cyc_Absyn_Abs_n_tag;
_temp1772->f1= 0;( void*) _temp1772;}); _temp1771->f2= Cyc_Toc__pop_handler_sp;
_temp1771;}), 0); e_exp->topt=({ struct Cyc_Core_Opt* _temp1773=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1773->v=( void*) e_typ; _temp1773;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( Cyc_Toc_try_env(
nv), _temp1704);{ struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt(
_temp1704, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0),
0); struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl( x, Cyc_Absyn_exn_typ,
0); xexp->topt=({ struct Cyc_Core_Opt* _temp1774=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1774->v=( void*) Cyc_Absyn_exn_typ;
_temp1774;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat* _temp1775=(
struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp1775->r=(
void*)({ struct Cyc_Absyn_Var_p_struct* _temp1777=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp1777->tag= Cyc_Absyn_Var_p_tag;
_temp1777->f1= x_vd;( void*) _temp1777;}); _temp1775->topt=({ struct Cyc_Core_Opt*
_temp1776=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1776->v=( void*) Cyc_Absyn_exn_typ; _temp1776;}); _temp1775->loc= 0;
_temp1775;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp1778=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1778->v=( void*) Cyc_Absyn_VoidType; _temp1778;});{ struct
Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp1779=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp1779->pattern= p;
_temp1779->pat_vars=({ struct Cyc_Core_Opt* _temp1780=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1780->v=( void*)({ struct Cyc_List_List*
_temp1781=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1781->hd=( void*) x_vd; _temp1781->tl= 0; _temp1781;}); _temp1780;});
_temp1779->where_clause= 0; _temp1779->body= rs; _temp1779->loc= 0; _temp1779;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1702,({
struct Cyc_List_List* _temp1782=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1782->hd=( void*) default_case; _temp1782->tl= 0;
_temp1782;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp1783=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1783->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1783->tl= 0; _temp1783;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp1784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1784->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp1784->tl= 0;
_temp1784;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp( Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL1591:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args= args->tl){(*(( struct _tuple4*) args->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple4*) args->hd)).f3);{ struct _tuple1* x=({ struct _tuple1* _temp1785=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1785->f1= Cyc_Absyn_Loc_n;
_temp1785->f2=(*(( struct _tuple4*) args->hd)).f1; _temp1785;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1786= s;
_LL1788: if( _temp1786 == Cyc_Absyn_Abstract){ goto _LL1789;} else{ goto _LL1790;}
_LL1790: if( _temp1786 == Cyc_Absyn_ExternC){ goto _LL1791;} else{ goto _LL1792;}
_LL1792: goto _LL1793; _LL1789: return Cyc_Absyn_Public; _LL1791: return Cyc_Absyn_Extern;
_LL1793: return s; _LL1787:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0;
static void Cyc_Toc_structdecl_to_c( struct Cyc_Absyn_Structdecl* s){ struct
_tuple1* n=( struct _tuple1*)( s->name)->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1794=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1794->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp1794;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(*
n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_structs_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(* n).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*) dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)(
Cyc_Toc_structs_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(* n).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc == Cyc_Absyn_Extern?
1:( void*) s->sc == Cyc_Absyn_Static)? 1:( void*) s->sc == Cyc_Absyn_ExternC)?
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp1795=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1795->v=( void*) n; _temp1795;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)( s->fields)->v; for( 0; fields != 0; fields= fields->tl){(*((
struct _tuple4*) fields->hd)).f3= Cyc_Toc_typ_to_c_array((*(( struct _tuple4*)
fields->hd)).f3);}}}}} static struct Cyc_Core_Opt* Cyc_Toc_enums_so_far= 0;
static struct Cyc_List_List* Cyc_Toc_enumdecl_to_c( struct Cyc_Absyn_Enumdecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)( ed->name)->v;
if( Cyc_Toc_enums_so_far == 0){ Cyc_Toc_enums_so_far=({ struct Cyc_Core_Opt*
_temp1796=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1796->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1796;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*) Cyc_Toc_enums_so_far->v,(* n).f2);
if( dopt == 0){ seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)(
Cyc_Toc_enums_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_enums_so_far->v,(* n).f2, ed));} else{
seen_before= 1; if((( struct Cyc_Absyn_Enumdecl*) dopt->v)->fields == 0){
seen_defn_before= 0;(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(( struct
Cyc_Dict_Dict*) Cyc_Toc_enums_so_far->v,(* n).f2, ed);} else{ seen_defn_before=
1;}} if( ! seen_before){ struct Cyc_Absyn_Typedefdecl* tdef=({ struct Cyc_Absyn_Typedefdecl*
_temp1797=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1797->name= n; _temp1797->tvs= 0; _temp1797->defn=( void*) Cyc_Absyn_void_star_typ();
_temp1797;}); res=({ struct Cyc_List_List* _temp1798=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1798->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1799=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1799->tag= Cyc_Absyn_Typedef_d_tag;
_temp1799->f1= tdef;( void*) _temp1799;}), 0); _temp1798->tl= res; _temp1798;});}{
void* t=({ struct Cyc_Absyn_TypedefType_struct* _temp1800=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1800->tag= Cyc_Absyn_TypedefType_tag;
_temp1800->f1= n; _temp1800->f2= 0; _temp1800->f3=({ struct Cyc_Core_Opt*
_temp1801=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1801->v=( void*) Cyc_Absyn_void_star_typ(); _temp1801;});( void*) _temp1800;});
int tag_count= 0; int box_count= 0;{ struct Cyc_List_List* fields= ed->fields ==
0? 0:( struct Cyc_List_List*)( ed->fields)->v; for( 0; fields != 0; fields=
fields->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fields->hd;
if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1802=( char*)"can't deal with enum tags yet";
struct _tagged_string _temp1803; _temp1803.curr= _temp1802; _temp1803.base=
_temp1802; _temp1803.last_plus_one= _temp1802 + 30; _temp1803;}));} if( f->typs
== 0){ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc == Cyc_Absyn_Extern? 0:(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Absyn_signed_int_exp((
tag_count ++), 0), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( f->name,
t, eopt);( void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({
struct Cyc_Absyn_Tqual* _temp1804=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1804->q_const= 1; _temp1804->q_volatile=
0; _temp1804->q_restrict= 0; _temp1804;}); res=({ struct Cyc_List_List*
_temp1805=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1805->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1806=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1806->tag= Cyc_Absyn_Var_d_tag; _temp1806->f1= vd;( void*) _temp1806;}), 0);
_temp1805->tl= res; _temp1805;});} else{ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc
== Cyc_Absyn_Extern? 0:( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1807=( char*)"_tag"; struct
_tagged_string _temp1808; _temp1808.curr= _temp1807; _temp1808.base= _temp1807;
_temp1808.last_plus_one= _temp1807 + 5; _temp1808;})), Cyc_Absyn_sint_t, eopt);(
void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({ struct
Cyc_Absyn_Tqual* _temp1809=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Tqual)); _temp1809->q_const= 1; _temp1809->q_volatile= 0;
_temp1809->q_restrict= 0; _temp1809;}); res=({ struct Cyc_List_List* _temp1810=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1810->hd=(
void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct* _temp1811=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1811->tag= Cyc_Absyn_Var_d_tag; _temp1811->f1= vd;( void*) _temp1811;}), 0);
_temp1810->tl= res; _temp1810;}); if( ! seen_defn_before){ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl,
i ++){ struct _tagged_string* fname=( struct _tagged_string*)({ struct
_tagged_string* _temp1812=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1812[ 0]= xprintf("f%d", i); _temp1812;}); fs=({
struct Cyc_List_List* _temp1813=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1813->hd=( void*)({ struct _tuple4* _temp1814=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1814->f1= fname;
_temp1814->f2=(*(( struct _tuple6*) ts->hd)).f1; _temp1814->f3= Cyc_Toc_typ_to_c_array((*((
struct _tuple6*) ts->hd)).f2); _temp1814;}); _temp1813->tl= fs; _temp1813;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);
fs=({ struct Cyc_List_List* _temp1815=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1815->hd=( void*)({ struct _tuple4*
_temp1816=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1816->f1=
Cyc_Toc_tag_sp; _temp1816->f2= Cyc_Absyn_empty_tqual(); _temp1816->f3= Cyc_Absyn_sint_t;
_temp1816;}); _temp1815->tl= fs; _temp1815;});{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp1817=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp1817->sc=( void*) Cyc_Absyn_Public;
_temp1817->name=({ struct Cyc_Core_Opt* _temp1819=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1819->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1820=( char*)"_struct"; struct
_tagged_string _temp1821; _temp1821.curr= _temp1820; _temp1821.base= _temp1820;
_temp1821.last_plus_one= _temp1820 + 8; _temp1821;})); _temp1819;}); _temp1817->tvs=
0; _temp1817->fields=({ struct Cyc_Core_Opt* _temp1818=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1818->v=( void*) fs; _temp1818;});
_temp1817;}); res=({ struct Cyc_List_List* _temp1822=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1822->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp1823=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1823->tag= Cyc_Absyn_Struct_d_tag;
_temp1823->f1= sd;( void*) _temp1823;}), 0); _temp1822->tl= res; _temp1822;});}}}}}
return res;}}} static struct Cyc_Core_Opt* Cyc_Toc_xenums_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xenumdecl_to_c( struct Cyc_Absyn_Xenumdecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp1824=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1824->name= n; _temp1824->tvs= 0; _temp1824->defn=( void*) t; _temp1824;});
res=({ struct Cyc_List_List* _temp1825=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1825->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1826=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1826->tag= Cyc_Absyn_Typedef_d_tag;
_temp1826->f1= td;( void*) _temp1826;}), 0); _temp1825->tl= res; _temp1825;});}
else{ if( Cyc_Toc_xenums_so_far == 0){ Cyc_Toc_xenums_so_far=({ struct Cyc_Core_Opt*
_temp1827=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1827->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1827;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->tag
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1828=( char*)"xenum:  can't deal with explicit tags";
struct _tagged_string _temp1829; _temp1829.curr= _temp1828; _temp1829.base=
_temp1828; _temp1829.last_plus_one= _temp1828 + 38; _temp1829;}));}{ struct
_tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp1830=* fn;( unsigned int)( _temp1830.last_plus_one -
_temp1830.curr);}), 0); void* tag_typ=({ struct Cyc_Absyn_ArrayType_struct*
_temp1831=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1831->tag= Cyc_Absyn_ArrayType_tag; _temp1831->f1=( void*) Cyc_Absyn_uchar_t;
_temp1831->f2= Cyc_Absyn_empty_tqual(); _temp1831->f3= sz_exp;( void*) _temp1831;});
struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc != Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp( 0,* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp1832=( char*)"_tag"; struct _tagged_string
_temp1833; _temp1833.curr= _temp1832; _temp1833.base= _temp1832; _temp1833.last_plus_one=
_temp1832 + 5; _temp1833;})), tag_typ, initopt);( void*)( tag_decl->sc=( void*)((
void*) xd->sc)); res=({ struct Cyc_List_List* _temp1834=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1834->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1835=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1835->tag= Cyc_Absyn_Var_d_tag;
_temp1835->f1= tag_decl;( void*) _temp1835;}), 0); _temp1834->tl= res; _temp1834;});{
int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*) Cyc_Toc_xenums_so_far->v, fn); if( dopt == 0){
seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_xenums_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Xenumdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_xenums_so_far->v, fn, xd));} else{ seen_before= 1;
if((( struct Cyc_Absyn_Xenumdecl*) dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)(
Cyc_Toc_xenums_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Xenumdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_xenums_so_far->v, fn, xd)); seen_defn_before= 0;}
else{ seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0;
int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl, i ++){
struct _tagged_string* field_n=( struct _tagged_string*)({ struct _tagged_string*
_temp1836=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1836[ 0]= xprintf("f%d", i); _temp1836;}); struct _tuple4* newf=({
struct _tuple4* _temp1837=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1837->f1= field_n; _temp1837->f2=(*(( struct _tuple6*) ts->hd)).f1;
_temp1837->f3= Cyc_Toc_typ_to_c_array((*(( struct _tuple6*) ts->hd)).f2);
_temp1837;}); fields=({ struct Cyc_List_List* _temp1838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1838->hd=( void*) newf;
_temp1838->tl= fields; _temp1838;});}} fields=({ struct Cyc_List_List* _temp1839=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1839->hd=(
void*)({ struct _tuple4* _temp1840=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1840->f1= Cyc_Toc_tag_sp; _temp1840->f2= Cyc_Absyn_empty_tqual();
_temp1840->f3= Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_empty_tqual());
_temp1840;}); _temp1839->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( fields); _temp1839;});{ struct Cyc_Absyn_Structdecl* strct_decl=({
struct Cyc_Absyn_Structdecl* _temp1841=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp1841->sc=( void*) Cyc_Absyn_Public;
_temp1841->name=({ struct Cyc_Core_Opt* _temp1843=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1843->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1844=( char*)"_struct"; struct
_tagged_string _temp1845; _temp1845.curr= _temp1844; _temp1845.base= _temp1844;
_temp1845.last_plus_one= _temp1844 + 8; _temp1845;})); _temp1843;}); _temp1841->tvs=
0; _temp1841->fields=({ struct Cyc_Core_Opt* _temp1842=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1842->v=( void*) fields;
_temp1842;}); _temp1841;}); res=({ struct Cyc_List_List* _temp1846=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1846->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp1847=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1847->tag= Cyc_Absyn_Struct_d_tag;
_temp1847->f1= strct_decl;( void*) _temp1847;}), 0); _temp1846->tl= res;
_temp1846;});}}}}}}}} return res;} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)
topt->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp1850; struct Cyc_List_List* _temp1852; struct Cyc_Toc_Env* _temp1854;
struct _tuple14 _temp1848= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL1855: _temp1854=
_temp1848.f1; goto _LL1853; _LL1853: _temp1852= _temp1848.f2; goto _LL1851;
_LL1851: _temp1850= _temp1848.f3; goto _LL1849; _LL1849: Cyc_Toc_stmt_to_c(
_temp1854, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1850, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1856=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1856->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp1857=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1857->tag= Cyc_Absyn_Abs_n_tag;
_temp1857->f1= 0;( void*) _temp1857;}); _temp1856->f2= Cyc_Toc_Match_Exception_sp;
_temp1856;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1850, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp1852 != 0; _temp1852= _temp1852->tl){
struct _tuple15 _temp1860; void* _temp1861; struct _tuple1* _temp1863; struct
_tuple15* _temp1858=( struct _tuple15*) _temp1852->hd; _temp1860=* _temp1858;
_LL1864: _temp1863= _temp1860.f1; goto _LL1862; _LL1862: _temp1861= _temp1860.f2;
goto _LL1859; _LL1859: s= Cyc_Absyn_declare_stmt( _temp1863, _temp1861, 0, s, 0);}
return s;}}} static struct _tuple18 Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top){ struct Cyc_List_List* res= 0; for( 0; ds !=
0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) ds->hd; void*
_temp1865=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp1889; struct Cyc_Absyn_Fndecl*
_temp1891; int _temp1893; struct Cyc_Absyn_Exp* _temp1895; struct Cyc_Core_Opt*
_temp1897; struct Cyc_Core_Opt* _temp1899; struct Cyc_Absyn_Pat* _temp1901;
struct Cyc_Absyn_Structdecl* _temp1903; struct Cyc_Absyn_Enumdecl* _temp1905;
struct Cyc_Absyn_Xenumdecl* _temp1907; struct Cyc_Absyn_Typedefdecl* _temp1909;
struct Cyc_List_List* _temp1911; struct _tagged_string* _temp1913; struct Cyc_List_List*
_temp1915; struct _tuple1* _temp1917; struct Cyc_List_List* _temp1919; _LL1867:
if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*) _temp1865)->tag ==
Cyc_Absyn_Var_d_tag: 0){ _LL1890: _temp1889=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1865)->f1; goto _LL1868;} else{ goto
_LL1869;} _LL1869: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL1892: _temp1891=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1865)->f1; goto _LL1870;} else{ goto _LL1871;}
_LL1871: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*) _temp1865)->tag
== Cyc_Absyn_Let_d_tag: 0){ _LL1902: _temp1901=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp1865)->f1; goto _LL1900; _LL1900: _temp1899=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1865)->f2; goto
_LL1898; _LL1898: _temp1897=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1865)->f3; goto _LL1896; _LL1896: _temp1895=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1865)->f4; goto _LL1894; _LL1894: _temp1893=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1865)->f5; goto _LL1872;} else{ goto
_LL1873;} _LL1873: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL1904: _temp1903=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1865)->f1; goto _LL1874;} else{ goto
_LL1875;} _LL1875: if( _temp1865 == Cyc_Absyn_Union_d){ goto _LL1876;} else{
goto _LL1877;} _LL1877: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1906: _temp1905=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1865)->f1; goto _LL1878;} else{ goto
_LL1879;} _LL1879: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1908: _temp1907=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1865)->f1; goto _LL1880;} else{ goto
_LL1881;} _LL1881: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL1910: _temp1909=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1865)->f1; goto _LL1882;} else{ goto
_LL1883;} _LL1883: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1914: _temp1913=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1865)->f1; goto
_LL1912; _LL1912: _temp1911=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1865)->f2; goto _LL1884;} else{ goto _LL1885;} _LL1885: if(( unsigned int)
_temp1865 > 1u?(( struct _enum_struct*) _temp1865)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1918: _temp1917=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1865)->f1; goto _LL1916; _LL1916: _temp1915=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1865)->f2; goto _LL1886;} else{ goto
_LL1887;} _LL1887: if(( unsigned int) _temp1865 > 1u?(( struct _enum_struct*)
_temp1865)->tag == Cyc_Absyn_ExternC_d_tag: 0){ _LL1920: _temp1919=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1865)->f1; goto _LL1888;} else{ goto
_LL1866;} _LL1868: { struct _tuple1* c_name= _temp1889->name; if(( void*)
_temp1889->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1* _temp1921=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1921->f1=({ struct
Cyc_Absyn_Abs_n_struct* _temp1922=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1922->tag= Cyc_Absyn_Abs_n_tag;
_temp1922->f1= 0;( void*) _temp1922;}); _temp1921->f2=(* c_name).f2; _temp1921;});}
if( _temp1889->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1923= _temp1889->initializer; if( _temp1923 == 0){
_throw( Null_Exception);} _temp1923;}));} if( nv->toplevel){ res=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls,
res); Cyc_Toc_temp_topdecls= 0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp1889->name,
Cyc_Absyn_varb_exp( c_name,({ struct Cyc_Absyn_Global_b_struct* _temp1924=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp1924->tag= Cyc_Absyn_Global_b_tag; _temp1924->f1= _temp1889;( void*)
_temp1924;}), 0));} else{ nv= Cyc_Toc_add_varmap( nv, _temp1889->name, Cyc_Absyn_varb_exp(
c_name,({ struct Cyc_Absyn_Local_b_struct* _temp1925=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1925->tag= Cyc_Absyn_Local_b_tag;
_temp1925->f1= _temp1889;( void*) _temp1925;}), 0));} _temp1889->name= c_name;(
void*)( _temp1889->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp1889->sc));(
void*)( _temp1889->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp1889->type));
res=({ struct Cyc_List_List* _temp1926=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1926->hd=( void*) d; _temp1926->tl= res;
_temp1926;}); goto _LL1866;} _LL1870: { struct _tuple1* c_name= _temp1891->name;
if(( void*) _temp1891->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1*
_temp1927=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1927->f1=({
struct Cyc_Absyn_Abs_n_struct* _temp1928=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1928->tag= Cyc_Absyn_Abs_n_tag;
_temp1928->f1= 0;( void*) _temp1928;}); _temp1927->f2=(* c_name).f2; _temp1927;});}
nv= Cyc_Toc_add_varmap( nv, _temp1891->name, Cyc_Absyn_var_exp( c_name, 0));
_temp1891->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp1891); res=({ struct Cyc_List_List*
_temp1929=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1929->hd=( void*) d; _temp1929->tl= res; _temp1929;}); goto _LL1866;}
_LL1872:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1930=( char*)"letdecl"; struct _tagged_string
_temp1931; _temp1931.curr= _temp1930; _temp1931.base= _temp1930; _temp1931.last_plus_one=
_temp1930 + 8; _temp1931;})); goto _LL1866; _LL1874: Cyc_Toc_structdecl_to_c(
_temp1903); res=({ struct Cyc_List_List* _temp1932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1932->hd=( void*) d; _temp1932->tl=
res; _temp1932;}); goto _LL1866; _LL1876:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1933=( char*)"uniondecl";
struct _tagged_string _temp1934; _temp1934.curr= _temp1933; _temp1934.base=
_temp1933; _temp1934.last_plus_one= _temp1933 + 10; _temp1934;})); goto _LL1866;
_LL1878: res= Cyc_Toc_enumdecl_to_c( _temp1905, res); goto _LL1866; _LL1880: res=
Cyc_Toc_xenumdecl_to_c( _temp1907, res); goto _LL1866; _LL1882: _temp1909->name=
_temp1909->name; _temp1909->tvs= 0;( void*)( _temp1909->defn=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1909->defn)); res=({ struct Cyc_List_List* _temp1935=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1935->hd=( void*) d; _temp1935->tl=
res; _temp1935;}); goto _LL1866; _LL1884: _temp1915= _temp1911; goto _LL1886;
_LL1886: _temp1919= _temp1915; goto _LL1888; _LL1888: { struct _tuple18 p= Cyc_Toc_decls_to_c(
nv, _temp1919, top); nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( p.f2), res); goto _LL1866;} _LL1866:;}
return({ struct _tuple18 _temp1936; _temp1936.f1= nv; _temp1936.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp1936;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_enums_so_far= 0; Cyc_Toc_xenums_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List* _temp1939;
struct Cyc_Toc_Env* _temp1941; struct _tuple18 _temp1937= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL1942: _temp1941= _temp1937.f1; goto _LL1940;
_LL1940: _temp1939= _temp1937.f2; goto _LL1938; _LL1938: while( Cyc_Toc_added_decls
!= 0) { struct Cyc_List_List* temp= Cyc_Toc_added_decls->tl; Cyc_Toc_added_decls->tl=
_temp1939; _temp1939= Cyc_Toc_added_decls; Cyc_Toc_added_decls= temp;} return
_temp1939;}}