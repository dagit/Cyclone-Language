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
struct _tuple1*, struct _tuple1*); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
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
struct Cyc_List_List* ds); extern int Cyc_Toc_check_for_null; static struct Cyc_Absyn_Abs_n_struct
_temp0={ 1u, 0}; void* Cyc_Toc_abs_ns=( struct Cyc_Absyn_Abs_n_struct*)& _temp0;
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq_v=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile=
0,.q_restrict= 0}; struct Cyc_Absyn_Tqual* Cyc_Toc_mt_tq=& Cyc_Toc_mt_tq_v; int
Cyc_Toc_check_for_null= 0; char Cyc_Toc_Toc_Unimplemented_tag[ 18u]="Toc_Unimplemented";
struct Cyc_Toc_Toc_Unimplemented_struct{ char* tag;}; char Cyc_Toc_Toc_Impossible_tag[
15u]="Toc_Impossible"; struct Cyc_Toc_Toc_Impossible_struct{ char* tag;}; static
void* Cyc_Toc_unimp( struct _tagged_string s){({ struct _tagged_string _temp1= s;
fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one - _temp1.curr, _temp1.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({
struct Cyc_Toc_Toc_Unimplemented_struct* _temp2=( struct Cyc_Toc_Toc_Unimplemented_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Toc_Toc_Unimplemented_struct));* _temp2=(
struct Cyc_Toc_Toc_Unimplemented_struct){.tag= Cyc_Toc_Toc_Unimplemented_tag};(
struct _xenum_struct*) _temp2;}));} static void* Cyc_Toc_toc_impos( struct
_tagged_string s){({ struct _tagged_string _temp3= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp3.last_plus_one - _temp3.curr, _temp3.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Toc_Toc_Impossible_struct*
_temp4=( struct Cyc_Toc_Toc_Impossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Toc_Impossible_struct));* _temp4=( struct Cyc_Toc_Toc_Impossible_struct){.tag=
Cyc_Toc_Toc_Impossible_tag};( struct _xenum_struct*) _temp4;}));} char Cyc_Toc_Match_error_tag[
12u]="Match_error"; struct Cyc_Toc_Match_error_struct{ char* tag;}; static char
_temp7[ 5u]="curr"; static struct _tagged_string Cyc_Toc_curr_string=( struct
_tagged_string){ _temp7, _temp7, _temp7 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static char _temp10[ 14u]="last_plus_one"; static struct
_tagged_string Cyc_Toc_last_plus_one_string=( struct _tagged_string){ _temp10,
_temp10, _temp10 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp13[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string=( struct _tagged_string){ _temp13, _temp13,
_temp13 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string;
static char _temp16[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string=(
struct _tagged_string){ _temp16, _temp16, _temp16 + 15u}; static struct
_tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static char _temp19[ 13u]="_enum_struct"; static struct _tagged_string Cyc_Toc__enum_struct_string=(
struct _tagged_string){ _temp19, _temp19, _temp19 + 13u}; static struct
_tagged_string* Cyc_Toc__enum_struct_sp=& Cyc_Toc__enum_struct_string; static
char _temp22[ 14u]="_xenum_struct"; static struct _tagged_string Cyc_Toc__xenum_struct_string=(
struct _tagged_string){ _temp22, _temp22, _temp22 + 14u}; static struct
_tagged_string* Cyc_Toc__xenum_struct_sp=& Cyc_Toc__xenum_struct_string; static
char _temp25[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=( struct
_tagged_string){ _temp25, _temp25, _temp25 + 2u}; static struct _tagged_string*
Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp28[ 4u]="tag"; static struct
_tagged_string Cyc_Toc_tag_string=( struct _tagged_string){ _temp28, _temp28,
_temp28 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _temp31[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string=(
struct _tagged_string){ _temp31, _temp31, _temp31 + 14u}; static struct
_tagged_string* Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string; static
char _temp34[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string=(
struct _tagged_string){ _temp34, _temp34, _temp34 + 7u}; static struct
_tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string; static char _temp37[
14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp37, _temp37, _temp37 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp40[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp40, _temp40, _temp40 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp43[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp43, _temp43, _temp43 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp46[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp46, _temp46, _temp46 + 14u}; static struct Cyc_Absyn_Abs_n_struct
_temp47={ 1u, 0}; static struct _tuple1 Cyc_Toc__npop_handler_pr=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp47,.f2=& Cyc_Toc__npop_handler_string};
static struct _tuple1* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static char _temp50[ 16u]="Match_Exception"; static struct _tagged_string Cyc_Toc_Match_Exception_string=(
struct _tagged_string){ _temp50, _temp50, _temp50 + 16u}; static struct
_tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static char _temp53[ 15u]="Cyc_new_string"; static struct _tagged_string Cyc_Toc_Cyc_new_string_string=(
struct _tagged_string){ _temp53, _temp53, _temp53 + 15u}; static struct
_tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static int Cyc_Toc_is_nullable( void* t){ if( ! Cyc_Toc_check_for_null){ return
0;}{ void* _temp54= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp60;
struct Cyc_Absyn_Conref* _temp62; struct Cyc_Absyn_Tqual* _temp64; struct Cyc_Absyn_Conref*
_temp66; void* _temp68; void* _temp70; _LL56: if(( unsigned int) _temp54 > 5u?((
struct _enum_struct*) _temp54)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL61:
_temp60=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp54)->f1; _LL71: _temp70=( void*) _temp60.elt_typ; goto _LL69; _LL69:
_temp68=( void*) _temp60.rgn_typ; goto _LL67; _LL67: _temp66=( struct Cyc_Absyn_Conref*)
_temp60.nullable; goto _LL65; _LL65: _temp64=( struct Cyc_Absyn_Tqual*) _temp60.tq;
goto _LL63; _LL63: _temp62=( struct Cyc_Absyn_Conref*) _temp60.bounds; goto
_LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL57: { void* _temp72=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp66))->v; int _temp78; _LL74: if(( unsigned int) _temp72 > 1u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL79: _temp78=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp72)->f1; goto _LL75;} else{ goto
_LL76;} _LL76: goto _LL77; _LL75: return _temp78; _LL77: return 0; _LL73:;}
_LL59:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp80=( char*)"is_nullable"; struct _tagged_string
_temp81; _temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one=
_temp80 + 12; _temp81;})); return 0; _LL55:;}} static int Cyc_Toc_really_is_nullable(
void* t){ int b= Cyc_Toc_check_for_null; Cyc_Toc_check_for_null= 1;{ int ans=
Cyc_Toc_is_nullable( t); Cyc_Toc_check_for_null= b; return ans;}} static struct
_tuple1* Cyc_Toc_collapse_qvar_tag( struct _tuple1* x, struct _tagged_string tag){
return({ struct _tuple1* _temp82=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp82->f1=(* x).f1; _temp82->f2=({ struct _tagged_string* _temp83=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp83[
0]= Cyc_String_strconcat(*(* x).f2, tag); _temp83;}); _temp82;});} static struct
Cyc_List_List* Cyc_Toc_added_decls= 0; static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts= ts->tl, i ++){ fs=({ struct Cyc_List_List*
_temp84=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp84->hd=( void*)({ struct _tuple4* _temp85=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp85->f1=({ struct _tagged_string* _temp86=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp86[ 0]=
xprintf("f%d", i); _temp86;}); _temp85->f2= Cyc_Toc_mt_tq; _temp85->f3=( void*)
ts->hd; _temp85;}); _temp84->tl= fs; _temp84;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl* _temp87=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp87->sc=( void*) Cyc_Absyn_Public;
_temp87->name=({ struct Cyc_Core_Opt* _temp89=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp89->v=( void*)({ struct _tuple1* _temp90=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp90->f1= Cyc_Toc_abs_ns;
_temp90->f2= x; _temp90;}); _temp89;}); _temp87->tvs= 0; _temp87->fields=({
struct Cyc_Core_Opt* _temp88=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp88->v=( void*) fs; _temp88;}); _temp87;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp91=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp91->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp92=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp92->tag= Cyc_Absyn_Struct_d_tag; _temp92->f1= sd;( void*) _temp92;}), 0);
_temp91->tl= Cyc_Toc_added_decls; _temp91;});}} static void Cyc_Toc_add_tagged_ptr_decl(
struct _tagged_string* x, void* t){ void* tptr= Cyc_Absyn_star_typ( t, Cyc_Absyn_HeapRgn,
Cyc_Toc_mt_tq); struct _tuple4* f1=({ struct _tuple4* _temp93=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp93->f1= Cyc_Toc_curr_sp; _temp93->f2=
Cyc_Toc_mt_tq; _temp93->f3= tptr; _temp93;}); struct _tuple4* f2=({ struct
_tuple4* _temp94=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp94->f1=
Cyc_Toc_base_sp; _temp94->f2= Cyc_Toc_mt_tq; _temp94->f3= tptr; _temp94;});
struct _tuple4* f3=({ struct _tuple4* _temp95=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp95->f1= Cyc_Toc_last_plus_one_sp; _temp95->f2=
Cyc_Toc_mt_tq; _temp95->f3= tptr; _temp95;}); struct Cyc_List_List* fs=({ struct
Cyc_List_List* _temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*) f1; _temp96->tl=({ struct Cyc_List_List* _temp97=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp97->hd=( void*)
f2; _temp97->tl=({ struct Cyc_List_List* _temp98=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp98->hd=( void*) f3; _temp98->tl=
0; _temp98;}); _temp97;}); _temp96;}); struct Cyc_Absyn_Structdecl* sd=({ struct
Cyc_Absyn_Structdecl* _temp99=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Structdecl)); _temp99->sc=( void*) Cyc_Absyn_Public; _temp99->name=({
struct Cyc_Core_Opt* _temp101=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp101->v=( void*)({ struct _tuple1* _temp102=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp102->f1= Cyc_Toc_abs_ns; _temp102->f2=
x; _temp102;}); _temp101;}); _temp99->tvs= 0; _temp99->fields=({ struct Cyc_Core_Opt*
_temp100=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp100->v=( void*)(( struct Cyc_List_List*) fs); _temp100;}); _temp99;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp103=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp103->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp104=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp104->tag= Cyc_Absyn_Struct_d_tag; _temp104->f1= sd;( void*) _temp104;}), 0);
_temp103->tl= Cyc_Toc_added_decls; _temp103;});} static struct Cyc_List_List*
Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter= 0; static struct
_tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{ struct Cyc_List_List*
tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts= tts->tl){ struct _tuple5
_temp107; struct Cyc_List_List* _temp108; struct _tagged_string* _temp110;
struct _tuple5* _temp105=( struct _tuple5*) tts->hd; _temp107=* _temp105; _LL111:
_temp110= _temp107.f1; goto _LL109; _LL109: _temp108= _temp107.f2; goto _LL106;
_LL106: { int okay= 1; struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0?
_temp108 != 0: 0; tqs= tqs->tl, _temp108= _temp108->tl){ if( ! Cyc_Tcutil_unify((*((
struct _tuple6*) tqs->hd)).f2,( void*) _temp108->hd)){ okay= 0; break;}} if(( !
okay? 1: tqs != 0)? 1: _temp108 != 0){ continue;} return _temp110;}}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp112=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp112[ 0]= xprintf("_tuple%d",(
Cyc_Toc_tuple_type_counter ++)); _temp112;}); struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple6*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp113->hd=( void*)({ struct
_tuple5* _temp114=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp114->f1= x; _temp114->f2= ts; _temp114;}); _temp113->tl= Cyc_Toc_tuple_types;
_temp113;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual* tq){{ void* _temp115= t; void* _temp121; void*
_temp123; _LL117: if(( unsigned int) _temp115 > 5u?(( struct _enum_struct*)
_temp115)->tag == Cyc_Absyn_IntType_tag: 0){ _LL124: _temp123=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp115)->f1; if( _temp123 == Cyc_Absyn_Unsigned){
goto _LL122;} else{ goto _LL119;} _LL122: _temp121=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp115)->f2; if( _temp121 == Cyc_Absyn_B1){ goto _LL118;} else{ goto _LL119;}}
else{ goto _LL119;} _LL119: goto _LL120; _LL118: return Cyc_Toc__tagged_string_sp;
_LL120: goto _LL116; _LL116:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts= tts->tl){ struct _tuple7 _temp127; void* _temp128; struct
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
_tuple1)); _temp135->f1= Cyc_Toc_abs_ns; _temp135->f2=({ struct _tagged_string*
_temp136=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp136[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp136;});
_temp135;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp137=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp137[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp137;});} static struct
Cyc_List_List* Cyc_Toc_temp_topdecls= 0; static char _temp140[ 10u]="_LongLong";
static struct _tagged_string Cyc_Toc_long_long_string=( struct _tagged_string){
_temp140, _temp140, _temp140 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp141={ 1u, 0}; static struct _tuple1 Cyc_Toc_longlong_v=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp141,.f2=& Cyc_Toc_long_long_string};
static struct Cyc_Absyn_TypedefType_struct _temp142={ 11u,& Cyc_Toc_longlong_v,
0, 0}; static void* Cyc_Toc_longlong_typ=( struct Cyc_Absyn_TypedefType_struct*)&
_temp142; static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple2* Cyc_Toc_arg_to_c(
struct _tuple2* a){ void* _temp145; struct Cyc_Absyn_Tqual* _temp147; struct Cyc_Core_Opt*
_temp149; struct _tuple2 _temp143=* a; _LL150: _temp149= _temp143.f1; goto
_LL148; _LL148: _temp147= _temp143.f2; goto _LL146; _LL146: _temp145= _temp143.f3;
goto _LL144; _LL144: return({ struct _tuple2* _temp151=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp151->f1= _temp149; _temp151->f2=
_temp147; _temp151->f3= Cyc_Toc_typ_to_c( _temp145); _temp151;});} static struct
_tuple6* Cyc_Toc_typ_to_c_f( struct _tuple6* x){ void* _temp154; struct Cyc_Absyn_Tqual*
_temp156; struct _tuple6 _temp152=* x; _LL157: _temp156= _temp152.f1; goto
_LL155; _LL155: _temp154= _temp152.f2; goto _LL153; _LL153: return({ struct
_tuple6* _temp158=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp158->f1= _temp156; _temp158->f2= Cyc_Toc_typ_to_c( _temp154); _temp158;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp159= t; struct Cyc_Absyn_Exp*
_temp167; struct Cyc_Absyn_Tqual* _temp169; void* _temp171; int _temp173; struct
Cyc_Core_Opt* _temp175; struct Cyc_Core_Opt _temp177; void* _temp178; void*
_temp180; _LL161: if(( unsigned int) _temp159 > 5u?(( struct _enum_struct*)
_temp159)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL172: _temp171=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp159)->f1; goto _LL170; _LL170: _temp169=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp159)->f2;
goto _LL168; _LL168: _temp167=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp159)->f3; goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int)
_temp159 > 5u?(( struct _enum_struct*) _temp159)->tag == Cyc_Absyn_Evar_tag: 0){
_LL181: _temp180=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp159)->f1; goto
_LL176; _LL176: _temp175=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp159)->f2; if( _temp175 == 0){ goto _LL165;} else{ _temp177=* _temp175;
_LL179: _temp178=( void*) _temp177.v; goto _LL174;} _LL174: _temp173=( int)((
struct Cyc_Absyn_Evar_struct*) _temp159)->f3; goto _LL164;} else{ goto _LL165;}
_LL165: goto _LL166; _LL162: return({ struct Cyc_Absyn_ArrayType_struct*
_temp182=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp182->tag= Cyc_Absyn_ArrayType_tag; _temp182->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp171); _temp182->f2= _temp169; _temp182->f3= _temp167;( void*) _temp182;});
_LL164: return Cyc_Toc_typ_to_c_array( _temp178); _LL166: return Cyc_Toc_typ_to_c(
t); _LL160:;} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp183= t; int
_temp225; struct Cyc_Core_Opt* _temp227; void* _temp229; int _temp231; struct
Cyc_Core_Opt* _temp233; struct Cyc_Core_Opt _temp235; void* _temp236; void*
_temp238; struct Cyc_Absyn_Tvar* _temp240; struct Cyc_Absyn_Enumdecl** _temp242;
struct Cyc_List_List* _temp244; struct _tuple1* _temp246; struct Cyc_Absyn_Xenumdecl**
_temp248; struct _tuple1* _temp250; struct Cyc_Absyn_PtrInfo _temp252; struct
Cyc_Absyn_Conref* _temp254; struct Cyc_Absyn_Tqual* _temp256; struct Cyc_Absyn_Conref*
_temp258; void* _temp260; void* _temp262; void* _temp264; void* _temp266; struct
Cyc_Absyn_Exp* _temp268; struct Cyc_Absyn_Tqual* _temp270; void* _temp272;
struct Cyc_Absyn_FnInfo _temp274; int _temp276; struct Cyc_List_List* _temp278;
void* _temp280; struct Cyc_Core_Opt* _temp282; struct Cyc_List_List* _temp284;
struct Cyc_List_List* _temp286; struct Cyc_Absyn_Structdecl** _temp288; struct
Cyc_List_List* _temp290; struct _tuple1* _temp292; struct Cyc_Core_Opt* _temp294;
struct Cyc_List_List* _temp296; struct _tuple1* _temp298; void* _temp300; void*
_temp302; struct Cyc_List_List* _temp304; _LL185: if( _temp183 == Cyc_Absyn_VoidType){
goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_Evar_tag: 0){ _LL230: _temp229=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f1; goto _LL228; _LL228:
_temp227=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f2;
if( _temp227 == 0){ goto _LL226;} else{ goto _LL189;} _LL226: _temp225=( int)((
struct Cyc_Absyn_Evar_struct*) _temp183)->f3; goto _LL188;} else{ goto _LL189;}
_LL189: if(( unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp183)->f1; goto _LL234; _LL234: _temp233=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp183)->f2; if( _temp233 == 0){ goto _LL191;} else{
_temp235=* _temp233; _LL237: _temp236=( void*) _temp235.v; goto _LL232;} _LL232:
_temp231=( int)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f3; goto _LL190;}
else{ goto _LL191;} _LL191: if(( unsigned int) _temp183 > 5u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_VarType_tag: 0){ _LL241: _temp240=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp183)->f1; goto
_LL192;} else{ goto _LL193;} _LL193: if(( unsigned int) _temp183 > 5u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL247: _temp246=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp183)->f1; goto _LL245;
_LL245: _temp244=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp183)->f2; goto _LL243; _LL243: _temp242=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp183)->f3; goto _LL194;} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp183 > 5u?(( struct _enum_struct*)
_temp183)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL251: _temp250=( struct _tuple1*)((
struct Cyc_Absyn_XenumType_struct*) _temp183)->f1; goto _LL249; _LL249: _temp248=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp183)->f2;
goto _LL196;} else{ goto _LL197;} _LL197: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL253:
_temp252=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp183)->f1; _LL263: _temp262=( void*) _temp252.elt_typ; goto _LL261; _LL261:
_temp260=( void*) _temp252.rgn_typ; goto _LL259; _LL259: _temp258=( struct Cyc_Absyn_Conref*)
_temp252.nullable; goto _LL257; _LL257: _temp256=( struct Cyc_Absyn_Tqual*)
_temp252.tq; goto _LL255; _LL255: _temp254=( struct Cyc_Absyn_Conref*) _temp252.bounds;
goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_IntType_tag: 0){ _LL267:
_temp266=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f1; goto _LL265;
_LL265: _temp264=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f2;
goto _LL200;} else{ goto _LL201;} _LL201: if( _temp183 == Cyc_Absyn_FloatType){
goto _LL202;} else{ goto _LL203;} _LL203: if( _temp183 == Cyc_Absyn_DoubleType){
goto _LL204;} else{ goto _LL205;} _LL205: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL273:
_temp272=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp183)->f1; goto
_LL271; _LL271: _temp270=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp183)->f2; goto _LL269; _LL269: _temp268=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp183)->f3; goto _LL206;} else{ goto _LL207;}
_LL207: if(( unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL275: _temp274=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp183)->f1; _LL285: _temp284=( struct Cyc_List_List*)
_temp274.tvars; goto _LL283; _LL283: _temp282=( struct Cyc_Core_Opt*) _temp274.effect;
goto _LL281; _LL281: _temp280=( void*) _temp274.ret_typ; goto _LL279; _LL279:
_temp278=( struct Cyc_List_List*) _temp274.args; goto _LL277; _LL277: _temp276=(
int) _temp274.varargs; goto _LL208;} else{ goto _LL209;} _LL209: if((
unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL287: _temp286=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp183)->f1; goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_StructType_tag:
0){ _LL293: _temp292=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp183)->f1; goto _LL291; _LL291: _temp290=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp183)->f2; goto _LL289; _LL289: _temp288=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp183)->f3;
goto _LL212;} else{ goto _LL213;} _LL213: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL299:
_temp298=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp183)->f1;
goto _LL297; _LL297: _temp296=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp183)->f2; goto _LL295; _LL295: _temp294=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp183)->f3; goto _LL214;} else{ goto _LL215;}
_LL215: if(( unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL301: _temp300=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp183)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if( _temp183 == Cyc_Absyn_HeapRgn){
goto _LL218;} else{ goto _LL219;} _LL219: if( _temp183 == Cyc_Absyn_UnionType){
goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL303:
_temp302=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp183)->f1; goto
_LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp183 > 5u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL305: _temp304=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp183)->f1; goto
_LL224;} else{ goto _LL184;} _LL186: return t; _LL188: return Cyc_Absyn_VoidType;
_LL190: return Cyc_Toc_typ_to_c( _temp236); _LL192: return Cyc_Absyn_void_star_typ();
_LL194: if( _temp246 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp306=( char*)"anonymous enum"; struct
_tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 15; _temp307;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp308=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp308->tag= Cyc_Absyn_TypedefType_tag; _temp308->f1=( struct _tuple1*)({
struct _tuple1* _temp309= _temp246; if( _temp309 == 0){ _throw( Null_Exception);}
_temp309;}); _temp308->f2= 0; _temp308->f3=({ struct Cyc_Core_Opt* _temp310=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp310->v=(
void*) Cyc_Absyn_void_star_typ(); _temp310;});( void*) _temp308;}); _LL196:
return({ struct Cyc_Absyn_TypedefType_struct* _temp311=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp311->tag= Cyc_Absyn_TypedefType_tag;
_temp311->f1= _temp250; _temp311->f2= 0; _temp311->f3=({ struct Cyc_Core_Opt*
_temp312=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp312->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp),
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp312;});( void*) _temp311;}); _LL198:
_temp262= Cyc_Toc_typ_to_c_array( _temp262);{ void* _temp313=( void*) _temp254->v;
void* _temp319; _LL315: if(( unsigned int) _temp313 > 1u?(( struct _enum_struct*)
_temp313)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL320: _temp319=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp313)->f1; if( _temp319 == Cyc_Absyn_Unknown_b){
goto _LL316;} else{ goto _LL317;}} else{ goto _LL317;} _LL317: goto _LL318;
_LL316: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp262,
_temp256); return Cyc_Absyn_strct( n);} _LL318: return Cyc_Absyn_star_typ(
_temp262, Cyc_Absyn_HeapRgn, _temp256); _LL314:;} _LL200: return t; _LL202:
return t; _LL204: return t; _LL206: return({ struct Cyc_Absyn_ArrayType_struct*
_temp321=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp321->tag= Cyc_Absyn_ArrayType_tag; _temp321->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp272); _temp321->f2= _temp270; _temp321->f3= _temp268;( void*) _temp321;});
_LL208: return({ struct Cyc_Absyn_FnType_struct* _temp322=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp322->tag= Cyc_Absyn_FnType_tag;
_temp322->f1=({ struct Cyc_Absyn_FnInfo _temp323; _temp323.tvars= 0; _temp323.effect=
0; _temp323.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp280); _temp323.args=((
struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp278); _temp323.varargs= _temp276;
_temp323;});( void*) _temp322;}); _LL210: _temp286=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_typ_to_c_f, _temp286);{ struct _tagged_string* n= Cyc_Toc_add_tuple_type(
_temp286); return Cyc_Absyn_strct( n);} _LL212: if( _temp292 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp324=( char*)"anonymous struct"; struct _tagged_string _temp325; _temp325.curr=
_temp324; _temp325.base= _temp324; _temp325.last_plus_one= _temp324 + 17;
_temp325;}));} return Cyc_Absyn_strctq(( struct _tuple1*)({ struct _tuple1*
_temp326= _temp292; if( _temp326 == 0){ _throw( Null_Exception);} _temp326;}));
_LL214: if( _temp294 == 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp327=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp328; _temp328.curr= _temp327; _temp328.base= _temp327;
_temp328.last_plus_one= _temp327 + 38; _temp328;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp329=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp329->tag= Cyc_Absyn_TypedefType_tag; _temp329->f1= _temp298; _temp329->f2=
0; _temp329->f3=({ struct Cyc_Core_Opt* _temp330=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp330->v=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp294->v); _temp330;});( void*) _temp329;}); _LL216: return Cyc_Absyn_void_star_typ();
_LL218: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp331=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 55; _temp332;})); _LL220: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp333=( char*)"uniontype"; struct _tagged_string _temp334; _temp334.curr=
_temp333; _temp334.base= _temp333; _temp334.last_plus_one= _temp333 + 10;
_temp334;})); _LL222: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp335=( char*)"Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 54; _temp336;})); _LL224: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp337=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 52; _temp338;})); _LL184:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp339= t; struct Cyc_Absyn_Exp* _temp345; struct Cyc_Absyn_Tqual*
_temp347; void* _temp349; _LL341: if(( unsigned int) _temp339 > 5u?(( struct
_enum_struct*) _temp339)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL350: _temp349=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp339)->f1; goto _LL348; _LL348:
_temp347=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp339)->f2; goto _LL346; _LL346: _temp345=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp339)->f3; goto _LL342;} else{ goto _LL343;}
_LL343: goto _LL344; _LL342: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp349, Cyc_Absyn_HeapRgn, _temp347), e, l); _LL344: return e; _LL340:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp351= t; struct Cyc_Absyn_Exp*
_temp357; struct Cyc_Absyn_Tqual* _temp359; void* _temp361; _LL353: if((
unsigned int) _temp351 > 5u?(( struct _enum_struct*) _temp351)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL362: _temp361=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f1;
goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp351)->f2; goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp351)->f3; goto _LL354;} else{ goto _LL355;}
_LL355: goto _LL356; _LL354: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp361, Cyc_Absyn_HeapRgn, _temp359), e, l); _LL356: return Cyc_Absyn_cast_exp(
t, e, l); _LL352:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp363= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp395; void* _temp397; void* _temp399; struct Cyc_Absyn_Exp*
_temp401; struct Cyc_Absyn_Tqual* _temp403; void* _temp405; struct Cyc_Absyn_FnInfo
_temp407; struct Cyc_List_List* _temp409; struct Cyc_Absyn_Structdecl** _temp411;
struct Cyc_List_List* _temp413; struct _tuple1* _temp415; struct Cyc_Absyn_Enumdecl**
_temp417; struct Cyc_List_List* _temp419; struct _tuple1* _temp421; struct Cyc_Absyn_Xenumdecl**
_temp423; struct _tuple1* _temp425; struct Cyc_Absyn_PtrInfo _temp427; void*
_temp429; _LL365: if( _temp363 == Cyc_Absyn_VoidType){ goto _LL366;} else{ goto
_LL367;} _LL367: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*)
_temp363)->tag == Cyc_Absyn_VarType_tag: 0){ _LL396: _temp395=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp363)->f1; goto _LL368;} else{ goto _LL369;}
_LL369: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*) _temp363)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp363)->f1; goto _LL398; _LL398: _temp397=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp363)->f2; goto _LL370;} else{ goto _LL371;} _LL371: if( _temp363 == Cyc_Absyn_FloatType){
goto _LL372;} else{ goto _LL373;} _LL373: if( _temp363 == Cyc_Absyn_DoubleType){
goto _LL374;} else{ goto _LL375;} _LL375: if(( unsigned int) _temp363 > 5u?((
struct _enum_struct*) _temp363)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL406:
_temp405=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp363)->f1; goto
_LL404; _LL404: _temp403=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp363)->f2; goto _LL402; _LL402: _temp401=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp363)->f3; goto _LL376;} else{ goto _LL377;}
_LL377: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*) _temp363)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL408: _temp407=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp363)->f1; goto _LL378;} else{ goto _LL379;}
_LL379: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*) _temp363)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL410: _temp409=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp363)->f1; goto _LL380;} else{ goto
_LL381;} _LL381: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*)
_temp363)->tag == Cyc_Absyn_StructType_tag: 0){ _LL416: _temp415=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp363)->f1; goto _LL414;
_LL414: _temp413=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp363)->f2; goto _LL412; _LL412: _temp411=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp363)->f3; goto _LL382;} else{ goto
_LL383;} _LL383: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*)
_temp363)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL422: _temp421=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp363)->f1; goto _LL420; _LL420: _temp419=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp363)->f2; goto
_LL418; _LL418: _temp417=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp363)->f3; goto _LL384;} else{ goto _LL385;} _LL385: if(( unsigned int)
_temp363 > 5u?(( struct _enum_struct*) _temp363)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL426: _temp425=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp363)->f1; goto _LL424; _LL424: _temp423=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp363)->f2; goto _LL386;} else{ goto
_LL387;} _LL387: if( _temp363 == Cyc_Absyn_UnionType){ goto _LL388;} else{ goto
_LL389;} _LL389: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*)
_temp363)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL428: _temp427=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp363)->f1; goto _LL390;} else{ goto
_LL391;} _LL391: if(( unsigned int) _temp363 > 5u?(( struct _enum_struct*)
_temp363)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL430: _temp429=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp363)->f1; goto _LL392;} else{ goto
_LL393;} _LL393: goto _LL394; _LL366: return 1; _LL368: return 0; _LL370: return
1; _LL372: return 1; _LL374: return 0; _LL376: return Cyc_Toc_atomic_typ(
_temp405); _LL378: return 1; _LL380: for( 0; _temp409 != 0; _temp409= _temp409->tl){
if( ! Cyc_Toc_atomic_typ((*(( struct _tuple6*) _temp409->hd)).f2)){ return 0;}}
return 1; _LL382: return 0; _LL384: return 0; _LL386: return 0; _LL388: return 0;
_LL390: return 0; _LL392: return 0; _LL394:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp431= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp431.last_plus_one - _temp431.curr,
_temp431.curr);})); return 0; _LL364:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp432= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp438;
struct Cyc_Absyn_Conref* _temp440; struct Cyc_Absyn_Tqual* _temp442; struct Cyc_Absyn_Conref*
_temp444; void* _temp446; void* _temp448; _LL434: if(( unsigned int) _temp432 >
5u?(( struct _enum_struct*) _temp432)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL439: _temp438=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp432)->f1; _LL449: _temp448=( void*) _temp438.elt_typ; goto _LL447; _LL447:
_temp446=( void*) _temp438.rgn_typ; goto _LL445; _LL445: _temp444=( struct Cyc_Absyn_Conref*)
_temp438.nullable; goto _LL443; _LL443: _temp442=( struct Cyc_Absyn_Tqual*)
_temp438.tq; goto _LL441; _LL441: _temp440=( struct Cyc_Absyn_Conref*) _temp438.bounds;
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL435: { void* _temp450=
Cyc_Tcutil_compress( _temp448); _LL452: if( _temp450 == Cyc_Absyn_VoidType){
goto _LL453;} else{ goto _LL454;} _LL454: goto _LL455; _LL453: return 1; _LL455:
return 0; _LL451:;} _LL437: return 0; _LL433:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp456= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp464; struct Cyc_Absyn_Structdecl* _temp466;
struct Cyc_List_List* _temp467; struct _tuple1* _temp469; struct Cyc_Absyn_Structdecl**
_temp471; struct Cyc_List_List* _temp473; struct _tuple1* _temp475; _LL458: if((
unsigned int) _temp456 > 5u?(( struct _enum_struct*) _temp456)->tag == Cyc_Absyn_StructType_tag:
0){ _LL470: _temp469=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp456)->f1; goto _LL468; _LL468: _temp467=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp456)->f2; goto _LL465; _LL465: _temp464=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp456)->f3;
if( _temp464 == 0){ goto _LL460;} else{ _temp466=* _temp464; goto _LL459;}}
else{ goto _LL460;} _LL460: if(( unsigned int) _temp456 > 5u?(( struct
_enum_struct*) _temp456)->tag == Cyc_Absyn_StructType_tag: 0){ _LL476: _temp475=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp456)->f1; goto
_LL474; _LL474: _temp473=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp456)->f2; goto _LL472; _LL472: _temp471=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp456)->f3; if( _temp471 == 0){ goto
_LL461;} else{ goto _LL462;}} else{ goto _LL462;} _LL462: goto _LL463; _LL459:
if( _temp469 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp477=( char*)"StructType name not properly set";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 33; _temp478;}));} if( _temp466->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp480= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp479=
_temp469; if( _temp479 == 0){ _throw( Null_Exception);} _temp479;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp480.last_plus_one - _temp480.curr, _temp480.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp466->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(*(( struct _tuple4*) fields->hd)).f1)
== 0){ return Cyc_Toc_is_void_star((*(( struct _tuple4*) fields->hd)).f3);}}((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp482= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp481=
_temp469; if( _temp481 == 0){ _throw( Null_Exception);} _temp481;})); struct
_tagged_string _temp483=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp482.last_plus_one - _temp482.curr, _temp482.curr, _temp483.last_plus_one -
_temp483.curr, _temp483.curr);})); return 0;} _LL461: if( _temp475 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp484=( char*)"StructType name not properly set"; struct _tagged_string
_temp485; _temp485.curr= _temp484; _temp485.base= _temp484; _temp485.last_plus_one=
_temp484 + 33; _temp485;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp487= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp486= _temp475; if( _temp486 == 0){ _throw( Null_Exception);}
_temp486;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp487.last_plus_one
- _temp487.curr, _temp487.curr);})); return 0; _LL463:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp488= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp488.last_plus_one - _temp488.curr,
_temp488.curr);})); return 0; _LL457:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp489=( void*) e->r; struct _tagged_string*
_temp497; struct Cyc_Absyn_Exp* _temp499; struct _tagged_string* _temp501;
struct Cyc_Absyn_Exp* _temp503; _LL491: if((( struct _enum_struct*) _temp489)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL500: _temp499=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp489)->f1; goto _LL498; _LL498:
_temp497=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp489)->f2; goto _LL492;} else{ goto _LL493;} _LL493: if((( struct
_enum_struct*) _temp489)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL504: _temp503=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp489)->f1;
goto _LL502; _LL502: _temp501=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp489)->f2; goto _LL494;} else{ goto _LL495;} _LL495: goto _LL496; _LL492:
return Cyc_Toc_is_poly_field(( void*)( _temp499->topt)->v, _temp497); _LL494: {
void* _temp505= Cyc_Tcutil_compress(( void*)( _temp503->topt)->v); struct Cyc_Absyn_PtrInfo
_temp511; struct Cyc_Absyn_Conref* _temp513; struct Cyc_Absyn_Tqual* _temp515;
struct Cyc_Absyn_Conref* _temp517; void* _temp519; void* _temp521; _LL507: if((
unsigned int) _temp505 > 5u?(( struct _enum_struct*) _temp505)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL512: _temp511=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp505)->f1; _LL522: _temp521=( void*) _temp511.elt_typ; goto _LL520; _LL520:
_temp519=( void*) _temp511.rgn_typ; goto _LL518; _LL518: _temp517=( struct Cyc_Absyn_Conref*)
_temp511.nullable; goto _LL516; _LL516: _temp515=( struct Cyc_Absyn_Tqual*)
_temp511.tq; goto _LL514; _LL514: _temp513=( struct Cyc_Absyn_Conref*) _temp511.bounds;
goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL508: return Cyc_Toc_is_poly_field(
_temp521, _temp501); _LL510:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp523= Cyc_Absynpp_typ2string(( void*)( _temp503->topt)->v);
xprintf("is_poly_project:  bad type %.*s", _temp523.last_plus_one - _temp523.curr,
_temp523.curr);})); return 0; _LL506:;} _LL496: return 0; _LL490:;} static char
_temp526[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp526, _temp526, _temp526 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp527={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp527,.f2=& Cyc_Toc_gc_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var, 0),({
struct Cyc_List_List* _temp528=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp528->hd=( void*) s; _temp528->tl= 0; _temp528;}), 0);}
static char _temp531[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp531, _temp531, _temp531 + 17u}; static struct Cyc_Absyn_Abs_n_struct
_temp532={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp532,.f2=& Cyc_Toc_gc_malloc_atomic_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var, 0),({
struct Cyc_List_List* _temp533=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp533->hd=( void*) s; _temp533->tl= 0; _temp533;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1;}; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3;}; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2;}; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp535=({ struct _tuple0
_temp534; _temp534.f1= t1; _temp534.f2= t2; _temp534;}); void* _temp541; struct
Cyc_Absyn_PtrInfo _temp543; void* _temp545; struct Cyc_Absyn_PtrInfo _temp547;
_LL537: _LL546: _temp545= _temp535.f1; if(( unsigned int) _temp545 > 5u?((
struct _enum_struct*) _temp545)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL548:
_temp547=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp545)->f1; goto _LL542;} else{ goto _LL539;} _LL542: _temp541= _temp535.f2;
if(( unsigned int) _temp541 > 5u?(( struct _enum_struct*) _temp541)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL544: _temp543=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp541)->f1; goto _LL538;} else{ goto _LL539;} _LL539: goto _LL540; _LL538: {
int n1=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp547.nullable);
int n2=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp543.nullable);
void* b1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp547.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp543.bounds); struct Cyc_List_List* convs= 0;{ struct _tuple0 _temp550=({
struct _tuple0 _temp549; _temp549.f1= b1; _temp549.f2= b2; _temp549;}); void*
_temp560; struct Cyc_Absyn_Exp* _temp562; void* _temp564; struct Cyc_Absyn_Exp*
_temp566; void* _temp568; void* _temp570; void* _temp572; void* _temp574; struct
Cyc_Absyn_Exp* _temp576; void* _temp578; struct Cyc_Absyn_Exp* _temp580; void*
_temp582; _LL552: _LL565: _temp564= _temp550.f1; if(( unsigned int) _temp564 > 1u?((
struct _enum_struct*) _temp564)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL567:
_temp566=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp564)->f1;
goto _LL561;} else{ goto _LL554;} _LL561: _temp560= _temp550.f2; if((
unsigned int) _temp560 > 1u?(( struct _enum_struct*) _temp560)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL563: _temp562=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp560)->f1; goto _LL553;} else{ goto _LL554;} _LL554: _LL571: _temp570=
_temp550.f1; if( _temp570 == Cyc_Absyn_Unknown_b){ goto _LL569;} else{ goto
_LL556;} _LL569: _temp568= _temp550.f2; if( _temp568 == Cyc_Absyn_Unknown_b){
goto _LL555;} else{ goto _LL556;} _LL556: _LL575: _temp574= _temp550.f1; if((
unsigned int) _temp574 > 1u?(( struct _enum_struct*) _temp574)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL577: _temp576=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp574)->f1; goto _LL573;} else{ goto _LL558;} _LL573: _temp572= _temp550.f2;
if( _temp572 == Cyc_Absyn_Unknown_b){ goto _LL557;} else{ goto _LL558;} _LL558:
_LL583: _temp582= _temp550.f1; if( _temp582 == Cyc_Absyn_Unknown_b){ goto _LL579;}
else{ goto _LL551;} _LL579: _temp578= _temp550.f2; if(( unsigned int) _temp578 >
1u?(( struct _enum_struct*) _temp578)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL581:
_temp580=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp578)->f1;
goto _LL559;} else{ goto _LL551;} _LL553: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List*
_temp584=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp584->hd=( void*)({ struct Cyc_Toc_NullCheck_struct* _temp585=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp585->tag= Cyc_Toc_NullCheck_tag;
_temp585->f1=( void*) t2;( void*) _temp585;}); _temp584->tl= convs; _temp584;});}
goto _LL551; _LL555: goto _LL551; _LL557: convs=({ struct Cyc_List_List*
_temp586=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp586->hd=( void*)({ struct Cyc_Toc_TagPtr_struct* _temp587=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp587->tag= Cyc_Toc_TagPtr_tag;
_temp587->f1=( void*) Cyc_Toc_typ_to_c( t1); _temp587->f2= _temp576;( void*)
_temp587;}); _temp586->tl= convs; _temp586;}); goto _LL551; _LL559: convs=({
struct Cyc_List_List* _temp588=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp588->hd=( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp589=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp589->tag= Cyc_Toc_UntagPtr_tag; _temp589->f1=( void*) Cyc_Toc_typ_to_c( t1);
_temp589->f2= _temp580; _temp589->f3= n2;( void*) _temp589;}); _temp588->tl=
convs; _temp588;}); goto _LL551; _LL551:;} return convs;} _LL540: return 0;
_LL536:;}} static char _temp592[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp592, _temp592, _temp592 + 7u}; static struct Cyc_Absyn_Abs_n_struct
_temp593={ 1u, 0}; static struct _tuple1 Cyc_Toc__throw_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp593,.f2=& Cyc_Toc__throw_string}; static
struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({ struct Cyc_List_List* _temp594=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp594->hd=(
void*) e; _temp594->tl= 0; _temp594;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c(
t); if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp595=( char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 44; _temp596;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp597= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp603; struct Cyc_Absyn_Conref* _temp605; struct Cyc_Absyn_Tqual*
_temp607; struct Cyc_Absyn_Conref* _temp609; void* _temp611; void* _temp613;
_LL599: if(( unsigned int) _temp597 > 5u?(( struct _enum_struct*) _temp597)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL604: _temp603=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp597)->f1; _LL614: _temp613=( void*)
_temp603.elt_typ; goto _LL612; _LL612: _temp611=( void*) _temp603.rgn_typ; goto
_LL610; _LL610: _temp609=( struct Cyc_Absyn_Conref*) _temp603.nullable; goto
_LL608; _LL608: _temp607=( struct Cyc_Absyn_Tqual*) _temp603.tq; goto _LL606;
_LL606: _temp605=( struct Cyc_Absyn_Conref*) _temp603.bounds; goto _LL600;}
else{ goto _LL601;} _LL601: goto _LL602; _LL600: elt_typ= _temp613;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp613, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL598;} _LL602: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp615=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp616;
_temp616.curr= _temp615; _temp616.base= _temp615; _temp616.last_plus_one=
_temp615 + 28; _temp616;})); _LL598:;} if( toplevel){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp617=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp617->tag= Cyc_Absyn_ArrayType_tag;
_temp617->f1=( void*) elt_typ; _temp617->f2= Cyc_Toc_mt_tq; _temp617->f3= sz;(
void*) _temp617;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp618=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp618->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp619=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp619->tag= Cyc_Absyn_Var_d_tag; _temp619->f1= vd;( void*) _temp619;}), 0);
_temp618->tl= Cyc_Toc_temp_topdecls; _temp618;});{ struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp620=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp620->hd=( void*)({ struct _tuple8* _temp625=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp625->f1= 0; _temp625->f2= xexp; _temp625;});
_temp620->tl=({ struct Cyc_List_List* _temp621=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp621->hd=( void*)({ struct
_tuple8* _temp624=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp624->f1= 0; _temp624->f2= xexp; _temp624;}); _temp621->tl=({ struct Cyc_List_List*
_temp622=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp622->hd=( void*)({ struct _tuple8* _temp623=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp623->f1= 0; _temp623->f2= xplussz; _temp623;});
_temp622->tl= 0; _temp622;}); _temp621;}); _temp620;}), 0); return urm_exp;}}
else{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
b, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_signed_int_exp( szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp(
a, 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, e, 0), s, 0); return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp*
Cyc_Toc_untag_ptr_conv( int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int
nullable, struct Cyc_Absyn_Exp* e){ if( toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp626=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp627; _temp627.curr= _temp626; _temp627.base= _temp626; _temp627.last_plus_one=
_temp626 + 45; _temp627;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
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
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs= cs->tl){ void* _temp628=(
void*) cs->hd; void* _temp636; struct Cyc_Absyn_Exp* _temp638; void* _temp640;
int _temp642; struct Cyc_Absyn_Exp* _temp644; void* _temp646; _LL630: if(((
struct _enum_struct*) _temp628)->tag == Cyc_Toc_NullCheck_tag){ _LL637: _temp636=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp628)->f1; goto _LL631;} else{
goto _LL632;} _LL632: if((( struct _enum_struct*) _temp628)->tag == Cyc_Toc_TagPtr_tag){
_LL641: _temp640=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp628)->f1; goto
_LL639; _LL639: _temp638=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp628)->f2; goto _LL633;} else{ goto _LL634;} _LL634: if((( struct
_enum_struct*) _temp628)->tag == Cyc_Toc_UntagPtr_tag){ _LL647: _temp646=( void*)((
struct Cyc_Toc_UntagPtr_struct*) _temp628)->f1; goto _LL645; _LL645: _temp644=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*) _temp628)->f2; goto
_LL643; _LL643: _temp642=( int)(( struct Cyc_Toc_UntagPtr_struct*) _temp628)->f3;
goto _LL635;} else{ goto _LL629;} _LL631: e= Cyc_Toc_null_check_conv( toplevel,
_temp636, e); goto _LL629; _LL633: e= Cyc_Toc_tag_ptr_conv( toplevel, _temp640,
_temp638, e); goto _LL629; _LL635: e= Cyc_Toc_untag_ptr_conv( toplevel, _temp646,
_temp644, _temp642, e); goto _LL629; _LL629:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel;}; typedef struct Cyc_Toc_Env*
Cyc_Toc_env; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({ struct Cyc_Toc_Env*
_temp648=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp648->break_lab= 0; _temp648->continue_lab= 0; _temp648->fallthru_info= 0;
_temp648->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple1*,
struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp648->toplevel= 1;
_temp648;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp649=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp649->break_lab= e->break_lab; _temp649->continue_lab=
e->continue_lab; _temp649->fallthru_info= e->fallthru_info; _temp649->varmap= e->varmap;
_temp649->toplevel= e->toplevel; _temp649;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp650=(* x).f1; struct
Cyc_List_List* _temp656; _LL652: if(( unsigned int) _temp650 > 1u?(( struct
_enum_struct*) _temp650)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL657: _temp656=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp650)->f1; goto
_LL653;} else{ goto _LL654;} _LL654: goto _LL655; _LL653:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp658=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp658=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=({ struct _tagged_string _temp659= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp659.last_plus_one -
_temp659.curr, _temp659.curr);})};( struct _xenum_struct*) _temp658;})); _LL655:
goto _LL651; _LL651:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key, struct
Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static
struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
fallthru_vars= 0; for( 0; fallthru_binders != 0; fallthru_binders=
fallthru_binders->tl){ fallthru_vars=({ struct Cyc_List_List* _temp660=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp660->hd=( void*)((
struct Cyc_Absyn_Vardecl*) fallthru_binders->hd)->name; _temp660->tl=
fallthru_vars; _temp660;});} fallthru_vars=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( fallthru_vars);{ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp661=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp661->v=( void*)
break_l; _temp661;}); ans->fallthru_info=({ struct Cyc_Core_Opt* _temp662=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp662->v=(
void*)({ struct _tuple9* _temp663=( struct _tuple9*) GC_malloc( sizeof( struct
_tuple9)); _temp663->f1= fallthru_l; _temp663->f2= fallthru_vars; _temp663->f3=
next_case_env->varmap; _temp663;}); _temp662;}); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp664=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp664->v=( void*) break_l; _temp664;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp665=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp665->v=( void*)({
struct _tuple9* _temp666=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp666->f1= next_l; _temp666->f2= 0; _temp666->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp666;}); _temp665;}); return ans;} static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp667=( char*)"expecting a literal format string";
struct _tagged_string _temp668; _temp668.curr= _temp667; _temp668.base= _temp667;
_temp668.last_plus_one= _temp667 + 34; _temp668;}));}{ struct _tagged_string
fmt_str;{ void* _temp669=( void*)(( struct Cyc_Absyn_Exp*) args->hd)->r; void*
_temp675; struct _tagged_string _temp677; int _temp679; _LL671: if((( struct
_enum_struct*) _temp669)->tag == Cyc_Absyn_Const_e_tag){ _LL676: _temp675=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp669)->f1; if(( unsigned int) _temp675 > 1u?((
struct _enum_struct*) _temp675)->tag == Cyc_Absyn_String_c_tag: 0){ _LL680:
_temp679=( int)(( struct Cyc_Absyn_String_c_struct*) _temp675)->f1; goto _LL678;
_LL678: _temp677=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp675)->f2; goto _LL672;} else{ goto _LL673;}} else{ goto _LL673;} _LL673:
goto _LL674; _LL672: fmt_str= _temp677; goto _LL670; _LL674: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp681=( char*)"expecting a literal format string"; struct _tagged_string
_temp682; _temp682.curr= _temp681; _temp682.base= _temp681; _temp682.last_plus_one=
_temp681 + 34; _temp682;})); _LL670:;}{ int len=( int)({ struct _tagged_string
_temp683= fmt_str;( unsigned int)( _temp683.last_plus_one - _temp683.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(( int)({ struct _tagged_string _temp684=
fmt_str; char* _temp686= _temp684.curr + i; if( _temp686 < _temp684.base? 1:
_temp686 >= _temp684.last_plus_one){ _throw( Null_Exception);}* _temp686;}) ==(
int)'%'){ i ++; if( i < len?( int)({ struct _tagged_string _temp687= fmt_str;
char* _temp689= _temp687.curr + i; if( _temp689 < _temp687.base? 1: _temp689 >=
_temp687.last_plus_one){ _throw( Null_Exception);}* _temp689;}) ==( int)'s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp690=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp690->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp690->tl= args; _temp690;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args= args->tl; typs= typs->tl;{
struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct
Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){ char c=({ struct
_tagged_string _temp691= fmt_str; char* _temp693= _temp691.curr + i; if(
_temp693 < _temp691.base? 1: _temp693 >= _temp691.last_plus_one){ _throw(
Null_Exception);}* _temp693;}); rev_fmt=({ struct Cyc_List_List* _temp694=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp694->hd=(
void*)(( int) c); _temp694->tl= rev_fmt; _temp694;}); if(( int) c !=( int)'%'){
continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp695=( char*)"bad format string"; struct
_tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 18; _temp696;}));}{ struct _tuple10 _temp699;
int _temp700; char _temp702; struct Cyc_List_List* _temp704; struct Cyc_List_List*
_temp706; struct Cyc_List_List* _temp708; struct Cyc_List_List* _temp710; struct
_tuple10* _temp697=( struct _tuple10*) x->v; _temp699=* _temp697; _LL711:
_temp710= _temp699.f1; goto _LL709; _LL709: _temp708= _temp699.f2; goto _LL707;
_LL707: _temp706= _temp699.f3; goto _LL705; _LL705: _temp704= _temp699.f4; goto
_LL703; _LL703: _temp702= _temp699.f5; goto _LL701; _LL701: _temp700= _temp699.f6;
goto _LL698; _LL698: i= _temp700 - 1; if(( int) _temp702 !=( int)'s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp712=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp712->hd=( void*) _temp710; _temp712->tl=({ struct Cyc_List_List* _temp713=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp713->hd=(
void*) _temp708; _temp713->tl=({ struct Cyc_List_List* _temp714=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp714->hd=( void*) _temp706;
_temp714->tl=({ struct Cyc_List_List* _temp715=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp715->hd=( void*) _temp704;
_temp715->tl= 0; _temp715;}); _temp714;}); _temp713;}); _temp712;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp716->hd=( void*)(( int) _temp702); _temp716->tl=
rev_fmt; _temp716;});{ struct Cyc_List_List* _temp717= _temp708; struct Cyc_List_List
_temp724; struct Cyc_List_List* _temp725; int _temp727; _LL719: if( _temp717 ==
0){ goto _LL721;} else{ _temp724=* _temp717; _LL728: _temp727=( int) _temp724.hd;
goto _LL726; _LL726: _temp725=( struct Cyc_List_List*) _temp724.tl; if( _temp725
== 0){ goto _LL723;} else{ goto _LL721;}} _LL723: if( _temp727 ==( int)'*'){
goto _LL720;} else{ goto _LL721;} _LL721: goto _LL722; _LL720: { struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp729=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp729->hd=( void*)({
struct _tuple11* _temp730=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp730->f1= temp; _temp730->f2=( void*) typs->hd; _temp730->f3=( struct Cyc_Absyn_Exp*)
args->hd; _temp730;}); _temp729->tl= rev_temps; _temp729;}); rev_result=({
struct Cyc_List_List* _temp731=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp731->hd=( void*) Cyc_Absyn_var_exp( temp, 0);
_temp731->tl= rev_result; _temp731;}); args= args->tl; typs= typs->tl; goto
_LL718;} _LL722: goto _LL718; _LL718:;}{ struct Cyc_List_List* _temp732=
_temp706; struct Cyc_List_List _temp739; struct Cyc_List_List* _temp740; struct
Cyc_List_List _temp742; struct Cyc_List_List* _temp743; int _temp745; int
_temp747; _LL734: if( _temp732 == 0){ goto _LL736;} else{ _temp739=* _temp732;
_LL748: _temp747=( int) _temp739.hd; goto _LL741; _LL741: _temp740=( struct Cyc_List_List*)
_temp739.tl; if( _temp740 == 0){ goto _LL736;} else{ _temp742=* _temp740; _LL746:
_temp745=( int) _temp742.hd; goto _LL744; _LL744: _temp743=( struct Cyc_List_List*)
_temp742.tl; if( _temp743 == 0){ goto _LL738;} else{ goto _LL736;}}} _LL738: if(
_temp747 ==( int)'.'? _temp745 ==( int)'*': 0){ goto _LL735;} else{ goto _LL736;}
_LL736: goto _LL737; _LL735: { struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp749=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp749->hd=( void*)({ struct _tuple11*
_temp750=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp750->f1=
temp; _temp750->f2=( void*) typs->hd; _temp750->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp750;}); _temp749->tl= rev_temps; _temp749;}); rev_result=({ struct Cyc_List_List*
_temp751=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp751->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp751->tl= rev_result;
_temp751;}); args= args->tl; typs= typs->tl; goto _LL733;} _LL737: goto _LL733;
_LL733:;} if(( int) _temp702 !=( int)'%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp752=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp752.last_plus_one
- _temp752.curr, _temp752.curr);}));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp753->hd=( void*)({ struct _tuple11*
_temp754=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp754->f1=
temp; _temp754->f2=( void*) typs->hd; _temp754->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp754;}); _temp753->tl= rev_temps; _temp753;}); rev_result=({ struct Cyc_List_List*
_temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp755->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp755->tl= rev_result;
_temp755;}); args= args->tl; typs= typs->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp756=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp756->hd=( void*)(( int)'.'); _temp756->tl= rev_fmt; _temp756;}); rev_fmt=({
struct Cyc_List_List* _temp757=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp757->hd=( void*)(( int)'*'); _temp757->tl= rev_fmt;
_temp757;}); rev_fmt=({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp758->hd=( void*)(( int)'s');
_temp758->tl= rev_fmt; _temp758;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp759->hd=( void*)({ struct _tuple11*
_temp760=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp760->f1=
temp; _temp760->f2=( void*) typs->hd; _temp760->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp760;}); _temp759->tl= rev_temps; _temp759;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp761=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp761->hd=( void*) size_exp; _temp761->tl= rev_result; _temp761;});
rev_result=({ struct Cyc_List_List* _temp762=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp762->hd=( void*) cstring_exp; _temp762->tl=
rev_result; _temp762;}); args= args->tl; typs= typs->tl;}}}}}}{ struct Cyc_List_List*
es=({ struct Cyc_List_List* _temp763=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp763->hd=( void*) Cyc_Absyn_string_exp( 0, Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp763->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp763;}); if( aopt != 0){ es=({ struct Cyc_List_List* _temp764=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp764->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp764->tl=( struct Cyc_List_List*)
es; _temp764;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p,( struct Cyc_List_List*)
es, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps
!= 0; rev_temps= rev_temps->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple11*)
rev_temps->hd)).f1,(*(( struct _tuple11*) rev_temps->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple11*) rev_temps->hd)).f3, s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}}
static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp765=( char*)"Missing type in primop "; struct _tagged_string _temp766;
_temp766.curr= _temp765; _temp766.base= _temp765; _temp766.last_plus_one=
_temp765 + 24; _temp766;}));} return Cyc_Toc_typ_to_c(( void*)( e->topt)->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp767=( char*)"Missing type in primop "; struct _tagged_string _temp768;
_temp768.curr= _temp767; _temp768.base= _temp767; _temp768.last_plus_one=
_temp767 + 24; _temp768;}));} return( void*)( e->topt)->v;} static struct
_tuple6* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple6*
_temp769=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp769->f1=
Cyc_Toc_mt_tq; _temp769->f2= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); _temp769;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple8* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple8* _temp770=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8) * 1); _temp770[ 0]=({ struct
_tuple8 _temp771; _temp771.f1= 0; _temp771.f2= e; _temp771;}); _temp770;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct _tuple1* x, void*
struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* se= Cyc_Absyn_sizeof_exp( struct_typ, 0); eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr(
se), 0);}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct
Cyc_Toc_Env* nv, int pointer, struct Cyc_List_List* es){ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, es); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; es=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es); for( 0; es != 0; es= es->tl, -- i){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) es->hd); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v): 0; s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string*
_temp772=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp772[ 0]= xprintf("f%d", i); _temp772;}), 0),( struct Cyc_Absyn_Exp*) es->hd,
0), 0), s, 0);}} return Cyc_Toc_make_struct( x, Cyc_Absyn_strct( n), s, pointer,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_List_List* dles, struct _tuple1*
tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es= es->tl){ struct _tuple8 _temp775;
struct Cyc_Absyn_Exp* _temp776; struct Cyc_List_List* _temp778; struct _tuple8*
_temp773=( struct _tuple8*) es->hd; _temp775=* _temp773; _LL779: _temp778=
_temp775.f1; goto _LL777; _LL777: _temp776= _temp775.f2; goto _LL774; _LL774:
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)( _temp776->topt)->v): 0; if(
_temp778 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp780=( char*)"empty designator list"; struct
_tagged_string _temp781; _temp781.curr= _temp780; _temp781.base= _temp780;
_temp781.last_plus_one= _temp780 + 22; _temp781;}));} if( _temp778->tl != 0){
struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp(
y, 0); for( 0; _temp778 != 0; _temp778= _temp778->tl){ void* _temp782=( void*)
_temp778->hd; struct _tagged_string* _temp788; _LL784: if((( struct _enum_struct*)
_temp782)->tag == Cyc_Absyn_FieldName_tag){ _LL789: _temp788=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp782)->f1; goto
_LL785;} else{ goto _LL786;} _LL786: goto _LL787; _LL785: if( Cyc_Toc_is_poly_field(
struct_type, _temp788)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp788, 0), yexp, 0), 0), s, 0); goto _LL783; _LL787:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp790=( char*)"array designator in struct";
struct _tagged_string _temp791; _temp791.curr= _temp790; _temp791.base= _temp790;
_temp791.last_plus_one= _temp790 + 27; _temp791;}))); _LL783:;} Cyc_Toc_exp_to_c(
nv, _temp776); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp776, 0), 0), s, 0);} else{ void* _temp792=( void*) _temp778->hd;
struct _tagged_string* _temp798; _LL794: if((( struct _enum_struct*) _temp792)->tag
== Cyc_Absyn_FieldName_tag){ _LL799: _temp798=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp792)->f1; goto _LL795;} else{ goto _LL796;}
_LL796: goto _LL797; _LL795: Cyc_Toc_exp_to_c( nv, _temp776); if( Cyc_Toc_is_poly_field(
struct_type, _temp798)){ _temp776= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp776, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp798, 0), _temp776, 0), 0), s, 0); goto _LL793; _LL797:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp800=( char*)"array designator in struct";
struct _tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
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
old_typ=( void*)( e->topt)->v; void* _temp847= r; void* _temp917; void* _temp920;
struct _tagged_string _temp922; int _temp924; void* _temp926; void* _temp928;
struct _tuple1* _temp930; struct _tuple1* _temp932; struct Cyc_List_List*
_temp934; void* _temp936; void* _temp938; struct Cyc_Absyn_Exp* _temp940; struct
Cyc_Absyn_Exp* _temp942; struct Cyc_Core_Opt* _temp944; struct Cyc_Absyn_Exp*
_temp946; struct Cyc_Absyn_Exp* _temp948; struct Cyc_Absyn_Exp* _temp950; struct
Cyc_Absyn_Exp* _temp952; struct Cyc_Absyn_Exp* _temp954; struct Cyc_Absyn_Exp*
_temp956; struct Cyc_List_List* _temp958; struct Cyc_Absyn_Exp* _temp960; struct
Cyc_List_List* _temp962; struct Cyc_Absyn_Exp* _temp964; struct Cyc_Absyn_Exp*
_temp966; struct Cyc_Absyn_Exp* _temp968; struct Cyc_List_List* _temp970; struct
Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Exp* _temp974; struct Cyc_Absyn_Exp**
_temp976; void* _temp977; void** _temp979; struct Cyc_Absyn_Exp* _temp980; void*
_temp982; struct Cyc_Absyn_Exp* _temp984; struct _tagged_string* _temp986;
struct Cyc_Absyn_Exp* _temp988; struct _tagged_string* _temp990; struct Cyc_Absyn_Exp*
_temp992; struct Cyc_Absyn_Exp* _temp994; struct Cyc_Absyn_Exp* _temp996; struct
Cyc_List_List* _temp998; struct Cyc_List_List* _temp1000; struct _tuple2*
_temp1002; struct Cyc_List_List* _temp1004; int _temp1006; struct Cyc_Absyn_Exp*
_temp1008; struct Cyc_Absyn_Exp* _temp1010; struct Cyc_Absyn_Vardecl* _temp1012;
struct Cyc_Absyn_Structdecl* _temp1014; struct Cyc_List_List* _temp1016; struct
Cyc_Core_Opt* _temp1018; struct _tuple1* _temp1020; struct Cyc_Absyn_Enumfield*
_temp1022; struct Cyc_Absyn_Enumdecl* _temp1024; struct Cyc_List_List* _temp1026;
struct Cyc_Core_Opt* _temp1028; struct Cyc_Core_Opt* _temp1030; struct Cyc_Absyn_Enumfield*
_temp1032; struct Cyc_Absyn_Enumdecl* _temp1034; struct Cyc_List_List* _temp1036;
struct Cyc_Core_Opt* _temp1038; struct Cyc_Core_Opt* _temp1040; struct Cyc_Absyn_Enumfield*
_temp1042; struct Cyc_Absyn_Xenumdecl* _temp1044; struct Cyc_List_List*
_temp1046; struct Cyc_Core_Opt* _temp1048; struct Cyc_Absyn_Stmt* _temp1050;
struct Cyc_List_List* _temp1052; struct Cyc_Core_Opt* _temp1054; struct Cyc_Absyn_Fndecl*
_temp1056; struct Cyc_Absyn_Exp* _temp1058; _LL849: if((( struct _enum_struct*)
_temp847)->tag == Cyc_Absyn_Const_e_tag){ _LL918: _temp917=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp847)->f1; if( _temp917 == Cyc_Absyn_Null_c){ goto _LL850;} else{ goto
_LL851;}} else{ goto _LL851;} _LL851: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Const_e_tag){ _LL921: _temp920=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp847)->f1; if(( unsigned int) _temp920 > 1u?(( struct _enum_struct*)
_temp920)->tag == Cyc_Absyn_String_c_tag: 0){ _LL925: _temp924=( int)(( struct
Cyc_Absyn_String_c_struct*) _temp920)->f1; goto _LL923; _LL923: _temp922=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp920)->f2; goto
_LL919;} else{ goto _LL853;}} else{ goto _LL853;} _LL919: if( _temp924){ goto
_LL852;} else{ goto _LL853;} _LL853: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Const_e_tag){ _LL927: _temp926=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp847)->f1; goto _LL854;} else{ goto _LL855;} _LL855: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Var_e_tag){ _LL931: _temp930=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp847)->f1; goto _LL929; _LL929:
_temp928=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp847)->f2; goto _LL856;}
else{ goto _LL857;} _LL857: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL933: _temp932=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp847)->f1; goto _LL858;} else{ goto _LL859;} _LL859: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Primop_e_tag){ _LL937: _temp936=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp847)->f1; goto _LL935; _LL935:
_temp934=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp847)->f2;
goto _LL860;} else{ goto _LL861;} _LL861: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Increment_e_tag){ _LL941: _temp940=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp847)->f1; goto _LL939; _LL939:
_temp938=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp847)->f2; goto
_LL862;} else{ goto _LL863;} _LL863: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL947: _temp946=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp847)->f1; goto _LL945; _LL945: _temp944=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp847)->f2; goto
_LL943; _LL943: _temp942=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp847)->f3; goto _LL864;} else{ goto _LL865;} _LL865: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Conditional_e_tag){ _LL953: _temp952=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp847)->f1;
goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp847)->f2; goto _LL949; _LL949: _temp948=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp847)->f3; goto _LL866;} else{ goto _LL867;}
_LL867: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL957: _temp956=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp847)->f1; goto _LL955; _LL955: _temp954=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp847)->f2; goto _LL868;} else{ goto _LL869;}
_LL869: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL961: _temp960=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp847)->f1; goto _LL959; _LL959: _temp958=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp847)->f2; goto _LL870;} else{ goto _LL871;}
_LL871: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_FnCall_e_tag){
_LL965: _temp964=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp847)->f1; goto _LL963; _LL963: _temp962=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp847)->f2; goto _LL872;} else{ goto _LL873;}
_LL873: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_Throw_e_tag){
_LL967: _temp966=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp847)->f1; goto _LL874;} else{ goto _LL875;} _LL875: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL969:
_temp968=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp847)->f1; goto _LL876;} else{ goto _LL877;} _LL877: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL973: _temp972=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp847)->f1;
goto _LL971; _LL971: _temp970=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp847)->f2; goto _LL878;} else{ goto _LL879;} _LL879: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Cast_e_tag){ _LL978: _temp977=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp847)->f1; _temp979=&(( struct Cyc_Absyn_Cast_e_struct*)
_temp847)->f1; goto _LL975; _LL975: _temp974=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp847)->f2; _temp976=&(( struct Cyc_Absyn_Cast_e_struct*)
_temp847)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Address_e_tag){ _LL981: _temp980=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp847)->f1; goto
_LL882;} else{ goto _LL883;} _LL883: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Sizeof_e_tag){ _LL983: _temp982=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp847)->f1; goto _LL884;} else{ goto _LL885;} _LL885: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Deref_e_tag){ _LL985: _temp984=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp847)->f1; goto
_LL886;} else{ goto _LL887;} _LL887: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL989: _temp988=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp847)->f1; goto _LL987; _LL987:
_temp986=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp847)->f2; goto _LL888;} else{ goto _LL889;} _LL889: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL993: _temp992=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp847)->f1;
goto _LL991; _LL991: _temp990=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp847)->f2; goto _LL890;} else{ goto _LL891;} _LL891: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Subscript_e_tag){ _LL997: _temp996=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp847)->f1;
goto _LL995; _LL995: _temp994=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp847)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Tuple_e_tag){ _LL999: _temp998=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp847)->f1; goto
_LL894;} else{ goto _LL895;} _LL895: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL1003: _temp1002=( struct _tuple2*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp847)->f1; goto _LL1001; _LL1001: _temp1000=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp847)->f2;
goto _LL896;} else{ goto _LL897;} _LL897: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Array_e_tag){ _LL1007: _temp1006=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp847)->f1; goto _LL1005; _LL1005: _temp1004=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp847)->f2; goto _LL898;} else{ goto _LL899;}
_LL899: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL1013: _temp1012=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp847)->f1; goto _LL1011; _LL1011: _temp1010=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp847)->f2; goto _LL1009; _LL1009:
_temp1008=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp847)->f3; goto _LL900;} else{ goto _LL901;} _LL901: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Struct_e_tag){ _LL1021: _temp1020=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp847)->f1; goto
_LL1019; _LL1019: _temp1018=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp847)->f2; goto _LL1017; _LL1017: _temp1016=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp847)->f3; goto _LL1015; _LL1015:
_temp1014=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp847)->f4; goto _LL902;} else{ goto _LL903;} _LL903: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Enum_e_tag){ _LL1031: _temp1030=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp847)->f1; goto
_LL1029; _LL1029: _temp1028=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp847)->f2; goto _LL1027; _LL1027: _temp1026=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp847)->f3; if( _temp1026 == 0){ goto
_LL1025;} else{ goto _LL905;} _LL1025: _temp1024=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp847)->f4; goto _LL1023; _LL1023: _temp1022=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp847)->f5;
goto _LL904;} else{ goto _LL905;} _LL905: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Enum_e_tag){ _LL1041: _temp1040=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp847)->f1; goto _LL1039; _LL1039: _temp1038=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp847)->f2; goto
_LL1037; _LL1037: _temp1036=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp847)->f3; goto _LL1035; _LL1035: _temp1034=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp847)->f4; goto _LL1033; _LL1033: _temp1032=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp847)->f5;
goto _LL906;} else{ goto _LL907;} _LL907: if((( struct _enum_struct*) _temp847)->tag
== Cyc_Absyn_Xenum_e_tag){ _LL1049: _temp1048=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp847)->f1; goto _LL1047; _LL1047: _temp1046=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp847)->f2; goto
_LL1045; _LL1045: _temp1044=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp847)->f3; goto _LL1043; _LL1043: _temp1042=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp847)->f4; goto _LL908;} else{ goto _LL909;}
_LL909: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1051: _temp1050=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp847)->f1; goto _LL910;} else{ goto _LL911;} _LL911: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1055:
_temp1054=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp847)->f1; goto _LL1053; _LL1053: _temp1052=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp847)->f2; goto _LL912;} else{
goto _LL913;} _LL913: if((( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1057: _temp1056=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp847)->f1; goto _LL914;} else{ goto _LL915;} _LL915: if((( struct
_enum_struct*) _temp847)->tag == Cyc_Absyn_Fill_e_tag){ _LL1059: _temp1058=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp847)->f1; goto
_LL916;} else{ goto _LL848;} _LL850:{ void* _temp1060= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1066; struct Cyc_Absyn_Conref* _temp1068;
struct Cyc_Absyn_Tqual* _temp1070; struct Cyc_Absyn_Conref* _temp1072; void*
_temp1074; void* _temp1076; _LL1062: if(( unsigned int) _temp1060 > 5u?(( struct
_enum_struct*) _temp1060)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1067:
_temp1066=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1060)->f1; _LL1077: _temp1076=( void*) _temp1066.elt_typ; goto _LL1075;
_LL1075: _temp1074=( void*) _temp1066.rgn_typ; goto _LL1073; _LL1073: _temp1072=(
struct Cyc_Absyn_Conref*) _temp1066.nullable; goto _LL1071; _LL1071: _temp1070=(
struct Cyc_Absyn_Tqual*) _temp1066.tq; goto _LL1069; _LL1069: _temp1068=( struct
Cyc_Absyn_Conref*) _temp1066.bounds; goto _LL1063;} else{ goto _LL1064;} _LL1064:
goto _LL1065; _LL1063:{ void* _temp1078=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1068); struct Cyc_Absyn_Exp* _temp1084; _LL1080: if(
_temp1078 == Cyc_Absyn_Unknown_b){ goto _LL1081;} else{ goto _LL1082;} _LL1082:
if(( unsigned int) _temp1078 > 1u?(( struct _enum_struct*) _temp1078)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1085: _temp1084=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1078)->f1; goto _LL1083;} else{ goto _LL1079;}
_LL1081: { struct _tuple8* zero=({ struct _tuple8* _temp1086=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1086->f1= 0; _temp1086->f2= Cyc_Absyn_signed_int_exp(
0, 0); _temp1086;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List* _temp1087=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1087->hd=( void*) zero; _temp1087->tl=({ struct Cyc_List_List*
_temp1088=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1088->hd=( void*) zero; _temp1088->tl=({ struct Cyc_List_List* _temp1089=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1089->hd=(
void*) zero; _temp1089->tl= 0; _temp1089;}); _temp1088;}); _temp1087;}), 0); if(
nv->toplevel){( void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct(
Cyc_Toc_add_tagged_ptr_type( Cyc_Toc_typ_to_c( _temp1076), Cyc_Toc_mt_tq));
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*)
um, Cyc_Absyn_exp_stmt( xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));} goto _LL1079;} _LL1083:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1079; _LL1079:;} goto _LL1061; _LL1065:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1090= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1090.last_plus_one -
_temp1090.curr, _temp1090.curr);})); goto _LL1061; _LL1061:;} goto _LL848;
_LL852:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1091=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1091->f1= Cyc_Toc_abs_ns; _temp1091->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1091;}), 0),({ struct Cyc_List_List* _temp1092=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1092->hd=( void*) Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp1093=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1093->tag= Cyc_Absyn_Const_e_tag;
_temp1093->f1=( void*)({ struct Cyc_Absyn_String_c_struct* _temp1094=( struct
Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1094->tag= Cyc_Absyn_String_c_tag; _temp1094->f1= 0; _temp1094->f2=
_temp922;( void*) _temp1094;});( void*) _temp1093;}), 0); _temp1092->tl= 0;
_temp1092;}), 0))->r)); goto _LL848; _LL854: goto _LL848; _LL856:{ struct
_handler_cons _temp1095; _push_handler(& _temp1095);{ struct _xenum_struct*
_temp1096=( struct _xenum_struct*) setjmp( _temp1095.handler); if( ! _temp1096){(
void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict*
d, struct _tuple1* key)) Cyc_Dict_lookup)( nv->varmap, _temp930))->r));;
_pop_handler();} else{ struct _xenum_struct* _temp1098= _temp1096; _LL1100: if(
_temp1098->tag == Cyc_Dict_Absent_tag){ goto _LL1101;} else{ goto _LL1102;}
_LL1102: goto _LL1103; _LL1101:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1104= Cyc_Absynpp_qvar2string( _temp930); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1104.last_plus_one - _temp1104.curr, _temp1104.curr);})); return; _LL1103:(
void) _throw( _temp1098); _LL1099:;}}} goto _LL848; _LL858:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1105=(
char*)"unknownid"; struct _tagged_string _temp1106; _temp1106.curr= _temp1105;
_temp1106.base= _temp1105; _temp1106.last_plus_one= _temp1105 + 10; _temp1106;}));
goto _LL848; _LL860: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp934); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp934);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp934);{ void* _temp1107= _temp936; _LL1109: if( _temp1107 == Cyc_Absyn_Size){
goto _LL1110;} else{ goto _LL1111;} _LL1111: if( _temp1107 == Cyc_Absyn_Printf){
goto _LL1112;} else{ goto _LL1113;} _LL1113: if( _temp1107 == Cyc_Absyn_Xprintf){
goto _LL1114;} else{ goto _LL1115;} _LL1115: if( _temp1107 == Cyc_Absyn_Fprintf){
goto _LL1116;} else{ goto _LL1117;} _LL1117: if( _temp1107 == Cyc_Absyn_Sscanf){
goto _LL1118;} else{ goto _LL1119;} _LL1119: if( _temp1107 == Cyc_Absyn_Plus){
goto _LL1120;} else{ goto _LL1121;} _LL1121: if( _temp1107 == Cyc_Absyn_Minus){
goto _LL1122;} else{ goto _LL1123;} _LL1123: if( _temp1107 == Cyc_Absyn_Eq){
goto _LL1124;} else{ goto _LL1125;} _LL1125: if( _temp1107 == Cyc_Absyn_Neq){
goto _LL1126;} else{ goto _LL1127;} _LL1127: if( _temp1107 == Cyc_Absyn_Gt){
goto _LL1128;} else{ goto _LL1129;} _LL1129: if( _temp1107 == Cyc_Absyn_Gte){
goto _LL1130;} else{ goto _LL1131;} _LL1131: if( _temp1107 == Cyc_Absyn_Lt){
goto _LL1132;} else{ goto _LL1133;} _LL1133: if( _temp1107 == Cyc_Absyn_Lte){
goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136; _LL1110: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*) _temp934->hd;{ void* _temp1137=( void*)( arg->topt)->v;
struct Cyc_Absyn_Exp* _temp1145; struct Cyc_Absyn_Tqual* _temp1147; void*
_temp1149; struct Cyc_Absyn_PtrInfo _temp1151; struct Cyc_Absyn_Conref*
_temp1153; struct Cyc_Absyn_Tqual* _temp1155; struct Cyc_Absyn_Conref* _temp1157;
void* _temp1159; void* _temp1161; _LL1139: if(( unsigned int) _temp1137 > 5u?((
struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1150:
_temp1149=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1137)->f1; goto
_LL1148; _LL1148: _temp1147=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1137)->f2; goto _LL1146; _LL1146: _temp1145=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1137)->f3; goto _LL1140;} else{ goto
_LL1141;} _LL1141: if(( unsigned int) _temp1137 > 5u?(( struct _enum_struct*)
_temp1137)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1152: _temp1151=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1137)->f1;
_LL1162: _temp1161=( void*) _temp1151.elt_typ; goto _LL1160; _LL1160: _temp1159=(
void*) _temp1151.rgn_typ; goto _LL1158; _LL1158: _temp1157=( struct Cyc_Absyn_Conref*)
_temp1151.nullable; goto _LL1156; _LL1156: _temp1155=( struct Cyc_Absyn_Tqual*)
_temp1151.tq; goto _LL1154; _LL1154: _temp1153=( struct Cyc_Absyn_Conref*)
_temp1151.bounds; goto _LL1142;} else{ goto _LL1143;} _LL1143: goto _LL1144;
_LL1140:( void*)( e->r=( void*)(( void*) _temp1145->r)); goto _LL1138; _LL1142: {
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)( arg->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1138;}
_LL1144:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1163= Cyc_Absynpp_typ2string(( void*) typs->hd); struct
_tagged_string _temp1164= Cyc_Absynpp_typ2string(( void*)( arg->topt)->v);
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1163.last_plus_one
- _temp1163.curr, _temp1163.curr, _temp1164.last_plus_one - _temp1164.curr,
_temp1164.curr);})); return; _LL1138:;} goto _LL1108;} _LL1112:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp936, 0, _temp934, typs))->r)); goto
_LL1108; _LL1114:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp936,
0, _temp934, typs))->r)); goto _LL1108; _LL1116: if( _temp934 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1165=( char*)"fprintf without arguments"; struct _tagged_string _temp1166;
_temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 26; _temp1166;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp936,({ struct Cyc_Core_Opt* _temp1167=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1167->v=( void*)(( struct Cyc_Absyn_Exp*)
_temp934->hd); _temp1167;}), _temp934->tl, typs->tl))->r)); goto _LL1108;
_LL1118: if( _temp934 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1168=( char*)"sscanf without arguments";
struct _tagged_string _temp1169; _temp1169.curr= _temp1168; _temp1169.base=
_temp1168; _temp1169.last_plus_one= _temp1168 + 25; _temp1169;}));}( void*)(((
struct Cyc_Absyn_Exp*) _temp934->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*) _temp934->hd)->r,(( struct
Cyc_Absyn_Exp*) _temp934->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1108;
_LL1120: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp934->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp934->tl)->hd; struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp,
0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1170=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1170->v=( void*) Cyc_Absyn_Plus; _temp1170;}), e2, 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1108; _LL1122: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp934->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp934->tl)->hd; void*
old_typ_e2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1171=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1171->v=( void*) Cyc_Absyn_Minus; _temp1171;}), e2,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1108; _LL1124: goto _LL1126; _LL1126:
goto _LL1128; _LL1128: goto _LL1130; _LL1130: goto _LL1132; _LL1132: goto
_LL1134; _LL1134: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp934->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp934->tl)->hd; void* t1=(
void*) old_typs->hd; void* t2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1108;} _LL1136: goto
_LL1108; _LL1108:;} goto _LL848;} _LL862: { void* e2_cyc_typ=( void*)( _temp940->topt)->v;
Cyc_Toc_exp_to_c( nv, _temp940); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){( void*)( _temp940->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp940->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( !
Cyc_Absyn_is_lvalue( _temp940)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp940, 0, Cyc_Toc_incr_lvalue, _temp938);( void*)( e->r=( void*)(( void*)
_temp940->r));} goto _LL848;} _LL864: { int e1_poly= Cyc_Toc_is_poly_project(
_temp946); void* e1_old_typ=( void*)( _temp946->topt)->v; void* e2_old_typ=(
void*)( _temp942->topt)->v; Cyc_Toc_exp_to_c( nv, _temp946); Cyc_Toc_exp_to_c(
nv, _temp942); if( _temp944 != 0?( void*) _temp944->v == Cyc_Absyn_Plus? 1:(
void*) _temp944->v == Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ(
e1_old_typ)){( void*)( _temp946->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp946->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp942->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp942->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp942->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp942->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp946)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple12*), struct
_tuple12* f_env)) Cyc_Toc_lvalue_assign)( _temp946, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple12* _temp1172=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12) * 1); _temp1172[ 0]=({ struct _tuple12 _temp1173; _temp1173.f1=
_temp944; _temp1173.f2= _temp942; _temp1173;}); _temp1172;}));( void*)( e->r=(
void*)(( void*) _temp946->r));} goto _LL848;} _LL866: Cyc_Toc_exp_to_c( nv,
_temp952); Cyc_Toc_exp_to_c( nv, _temp950); Cyc_Toc_exp_to_c( nv, _temp948);
goto _LL848; _LL868: Cyc_Toc_exp_to_c( nv, _temp956); Cyc_Toc_exp_to_c( nv,
_temp954); goto _LL848; _LL870: _temp964= _temp960; _temp962= _temp958; goto
_LL872; _LL872: Cyc_Toc_exp_to_c( nv, _temp964);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp962); goto _LL848; _LL874: Cyc_Toc_exp_to_c(
nv, _temp966);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp966), 0))->r)); goto _LL848; _LL876: Cyc_Toc_exp_to_c(
nv, _temp968); goto _LL848; _LL878: Cyc_Toc_exp_to_c( nv, _temp972);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); for( 0; _temp970 != 0; _temp970=
_temp970->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp970->hd); if( k != Cyc_Absyn_EffKind?
k != Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp972, 0))->r)); break;}} goto _LL848;} _LL880: { void* old_t2=( void*)((*
_temp976)->topt)->v; void* new_typ=* _temp979;* _temp979= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv,* _temp976);* _temp976= Cyc_Toc_convert_exp( nv->toplevel,
old_t2, new_typ,* _temp976); goto _LL848;} _LL882:{ void* _temp1174=( void*)
_temp980->r; struct Cyc_Absyn_Structdecl* _temp1182; struct Cyc_List_List*
_temp1184; struct Cyc_Core_Opt* _temp1186; struct _tuple1* _temp1188; struct Cyc_List_List*
_temp1190; _LL1176: if((( struct _enum_struct*) _temp1174)->tag == Cyc_Absyn_Struct_e_tag){
_LL1189: _temp1188=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1174)->f1; goto _LL1187; _LL1187: _temp1186=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1174)->f2; goto _LL1185; _LL1185:
_temp1184=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1174)->f3; goto _LL1183; _LL1183: _temp1182=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1174)->f4; goto _LL1177;} else{ goto
_LL1178;} _LL1178: if((( struct _enum_struct*) _temp1174)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1191: _temp1190=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1174)->f1; goto _LL1179;} else{ goto _LL1180;} _LL1180: goto _LL1181;
_LL1177: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1192= Cyc_Position_string_of_segment( _temp980->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1192.last_plus_one -
_temp1192.curr, _temp1192.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)( _temp980->topt)->v, 1, _temp1184, _temp1188))->r)); goto _LL1175;
_LL1179: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1193= Cyc_Position_string_of_segment( _temp980->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1193.last_plus_one -
_temp1193.curr, _temp1193.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1190))->r)); goto _LL1175; _LL1181: Cyc_Toc_exp_to_c( nv, _temp980);
if( ! Cyc_Absyn_is_lvalue( _temp980)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int),
int f_env)) Cyc_Toc_lvalue_assign)( _temp980, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*) _temp980->r));} goto _LL1175; _LL1175:;} goto
_LL848; _LL884:( void*)( e->r=( void*)({ struct Cyc_Absyn_Sizeof_e_struct*
_temp1194=( struct Cyc_Absyn_Sizeof_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeof_e_struct));
_temp1194->tag= Cyc_Absyn_Sizeof_e_tag; _temp1194->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp982);( void*) _temp1194;})); goto _LL848; _LL886: Cyc_Toc_exp_to_c( nv,
_temp984);{ void* _temp1195= Cyc_Tcutil_compress(( void*)( _temp984->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1201; struct Cyc_Absyn_Conref* _temp1203; struct
Cyc_Absyn_Tqual* _temp1205; struct Cyc_Absyn_Conref* _temp1207; void* _temp1209;
void* _temp1211; _LL1197: if(( unsigned int) _temp1195 > 5u?(( struct
_enum_struct*) _temp1195)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1202:
_temp1201=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1195)->f1; _LL1212: _temp1211=( void*) _temp1201.elt_typ; goto _LL1210;
_LL1210: _temp1209=( void*) _temp1201.rgn_typ; goto _LL1208; _LL1208: _temp1207=(
struct Cyc_Absyn_Conref*) _temp1201.nullable; goto _LL1206; _LL1206: _temp1205=(
struct Cyc_Absyn_Tqual*) _temp1201.tq; goto _LL1204; _LL1204: _temp1203=( struct
Cyc_Absyn_Conref*) _temp1201.bounds; goto _LL1198;} else{ goto _LL1199;} _LL1199:
goto _LL1200; _LL1198:{ void* _temp1213=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1203); struct Cyc_Absyn_Exp* _temp1219; _LL1215: if(
_temp1213 == Cyc_Absyn_Unknown_b){ goto _LL1216;} else{ goto _LL1217;} _LL1217:
if(( unsigned int) _temp1213 > 1u?(( struct _enum_struct*) _temp1213)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1220: _temp1219=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1213)->f1; goto _LL1218;} else{ goto _LL1214;}
_LL1216: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
void* ta1= Cyc_Toc_typ_to_c_array( _temp1211); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1205); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1205); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp984, s1, 0), 0))->r));
goto _LL1214;} _LL1218: { int nullable= Cyc_Toc_is_nullable(( void*)( _temp984->topt)->v);
if( nullable){ struct _tuple1* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)( _temp984->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( _temp1219->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp984, s3, 0), 0))->r));} goto _LL1214;} _LL1214:;} goto _LL1196; _LL1200:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1221=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1222; _temp1222.curr= _temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one=
_temp1221 + 29; _temp1222;})); goto _LL1196; _LL1196:;} goto _LL848; _LL888: Cyc_Toc_exp_to_c(
nv, _temp988); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL848; _LL890: Cyc_Toc_exp_to_c( nv, _temp992);
if( Cyc_Toc_is_nullable(( void*)( _temp992->topt)->v)){ struct _tuple1* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)( _temp992->topt)->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp990, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp992, s3, 0), 0))->r));} else{ if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}} goto _LL848; _LL892: { void*
e1_typ= Cyc_Tcutil_compress(( void*)( _temp996->topt)->v); Cyc_Toc_exp_to_c( nv,
_temp996); Cyc_Toc_exp_to_c( nv, _temp994);( void*)( _temp996->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*)
_temp996->r, 0), 0))->r));{ void* _temp1223= e1_typ; struct Cyc_List_List*
_temp1233; struct Cyc_Absyn_Exp* _temp1235; struct Cyc_Absyn_Tqual* _temp1237;
void* _temp1239; struct Cyc_Absyn_PtrInfo _temp1241; struct Cyc_Absyn_Conref*
_temp1243; struct Cyc_Absyn_Tqual* _temp1245; struct Cyc_Absyn_Conref* _temp1247;
void* _temp1249; void* _temp1251; _LL1225: if(( unsigned int) _temp1223 > 5u?((
struct _enum_struct*) _temp1223)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1234:
_temp1233=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1223)->f1; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(( unsigned int)
_temp1223 > 5u?(( struct _enum_struct*) _temp1223)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1240: _temp1239=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1223)->f1;
goto _LL1238; _LL1238: _temp1237=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1223)->f2; goto _LL1236; _LL1236: _temp1235=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1223)->f3; goto _LL1228;} else{ goto
_LL1229;} _LL1229: if(( unsigned int) _temp1223 > 5u?(( struct _enum_struct*)
_temp1223)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1242: _temp1241=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1223)->f1;
_LL1252: _temp1251=( void*) _temp1241.elt_typ; goto _LL1250; _LL1250: _temp1249=(
void*) _temp1241.rgn_typ; goto _LL1248; _LL1248: _temp1247=( struct Cyc_Absyn_Conref*)
_temp1241.nullable; goto _LL1246; _LL1246: _temp1245=( struct Cyc_Absyn_Tqual*)
_temp1241.tq; goto _LL1244; _LL1244: _temp1243=( struct Cyc_Absyn_Conref*)
_temp1241.bounds; goto _LL1230;} else{ goto _LL1231;} _LL1231: goto _LL1232;
_LL1226: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp994) +(
unsigned int) 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp996,({ struct _tagged_string* _temp1253=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1253[ 0]= xprintf("f%d",( int) i);
_temp1253;}), 0))->r)); goto _LL1224;} _LL1228:( void*)( e->r=( void*)(( void*)(
Cyc_Toc_known_size_subscript( _temp996, _temp994, _temp1235, _temp1239,
_temp1237, 0))->r)); goto _LL1224; _LL1230:{ void* _temp1254=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1243); struct Cyc_Absyn_Exp*
_temp1260; _LL1256: if(( unsigned int) _temp1254 > 1u?(( struct _enum_struct*)
_temp1254)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1261: _temp1260=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1254)->f1; goto _LL1257;} else{ goto
_LL1258;} _LL1258: if( _temp1254 == Cyc_Absyn_Unknown_b){ goto _LL1259;} else{
goto _LL1255;} _LL1257:( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp996, _temp994, _temp1260, _temp1251, _temp1245, 0))->r)); goto _LL1255;
_LL1259: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1251); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1245); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1245); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp994, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp996, s1, 0), 0))->r)); goto _LL1255;} _LL1255:;}
goto _LL1224; _LL1232:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1262=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1263; _temp1263.curr= _temp1262; _temp1263.base=
_temp1262; _temp1263.last_plus_one= _temp1262 + 49; _temp1263;})); goto _LL1224;
_LL1224:;} goto _LL848;} _LL894: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp998))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp998); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp998 != 0; _temp998= _temp998->tl, i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp998->hd);{ struct Cyc_List_List* des=({ struct
Cyc_List_List* _temp1264=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1264->hd=( void*)({ struct Cyc_Absyn_FieldName_struct* _temp1265=( struct
Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1265->tag= Cyc_Absyn_FieldName_tag; _temp1265->f1=({ struct _tagged_string*
_temp1266=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1266[ 0]= xprintf("f%d", i); _temp1266;});( void*) _temp1265;});
_temp1264->tl= 0; _temp1264;}); dles=({ struct Cyc_List_List* _temp1267=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1267->hd=( void*)({
struct _tuple8* _temp1268=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1268->f1=( struct Cyc_List_List*) des; _temp1268->f2=( struct Cyc_Absyn_Exp*)
_temp998->hd; _temp1268;}); _temp1267->tl= dles; _temp1267;});}}} dles=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp(
0, dles, 0), 0))->r));} goto _LL848; _LL896:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1269=( char*)"compoundlit";
struct _tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base=
_temp1269; _temp1270.last_plus_one= _temp1269 + 12; _temp1270;})); goto _LL848;
_LL898: if( nv->toplevel? 1: ! _temp1006){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1004, 0))->r));{ struct Cyc_List_List* dles= _temp1004; for( 0; dles !=
0; dles= dles->tl){ struct _tuple8 _temp1273; struct Cyc_Absyn_Exp* _temp1274;
struct Cyc_List_List* _temp1276; struct _tuple8* _temp1271=( struct _tuple8*)
dles->hd; _temp1273=* _temp1271; _LL1277: _temp1276= _temp1273.f1; goto _LL1275;
_LL1275: _temp1274= _temp1273.f2; goto _LL1272; _LL1272: Cyc_Toc_exp_to_c( nv,
_temp1274);}}} else{ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1004) - 1;{ struct Cyc_List_List* dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1004);
for( 0; dles != 0; dles= dles->tl){ struct _tuple8 _temp1280; struct Cyc_Absyn_Exp*
_temp1281; struct Cyc_List_List* _temp1283; struct _tuple8* _temp1278=( struct
_tuple8*) dles->hd; _temp1280=* _temp1278; _LL1284: _temp1283= _temp1280.f1;
goto _LL1282; _LL1282: _temp1281= _temp1280.f2; goto _LL1279; _LL1279: { struct
Cyc_Absyn_Exp* e_index; if( _temp1283 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if( _temp1283->tl != 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1285=( char*)"multiple designators in array";
struct _tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base=
_temp1285; _temp1286.last_plus_one= _temp1285 + 30; _temp1286;}));}{ void* d=(
void*) _temp1283->hd; void* _temp1287= d; struct Cyc_Absyn_Exp* _temp1293;
struct _tagged_string* _temp1295; _LL1289: if((( struct _enum_struct*) _temp1287)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1294: _temp1293=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1287)->f1; goto _LL1290;} else{ goto
_LL1291;} _LL1291: if((( struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_FieldName_tag){
_LL1296: _temp1295=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1287)->f1; goto _LL1292;} else{ goto _LL1288;} _LL1290: Cyc_Toc_exp_to_c(
nv, _temp1293); e_index= _temp1293; goto _LL1288; _LL1292: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1297=( char*)"field name designators in array"; struct _tagged_string
_temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297; _temp1298.last_plus_one=
_temp1297 + 32; _temp1298;})); goto _LL1288; _LL1288:;}} Cyc_Toc_exp_to_c( nv,
_temp1281); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1281, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1299= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1305; struct Cyc_Absyn_Conref* _temp1307; struct Cyc_Absyn_Tqual* _temp1309;
struct Cyc_Absyn_Conref* _temp1311; void* _temp1313; void* _temp1315; _LL1301:
if(( unsigned int) _temp1299 > 5u?(( struct _enum_struct*) _temp1299)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1306: _temp1305=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1299)->f1; _LL1316: _temp1315=( void*)
_temp1305.elt_typ; goto _LL1314; _LL1314: _temp1313=( void*) _temp1305.rgn_typ;
goto _LL1312; _LL1312: _temp1311=( struct Cyc_Absyn_Conref*) _temp1305.nullable;
goto _LL1310; _LL1310: _temp1309=( struct Cyc_Absyn_Tqual*) _temp1305.tq; goto
_LL1308; _LL1308: _temp1307=( struct Cyc_Absyn_Conref*) _temp1305.bounds; goto
_LL1302;} else{ goto _LL1303;} _LL1303: goto _LL1304; _LL1302: old_elt_typ=
_temp1315; goto _LL1300; _LL1304: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1317=( char*)"exp_to_c:array expression doesn't have array type";
struct _tagged_string _temp1318; _temp1318.curr= _temp1317; _temp1318.base=
_temp1317; _temp1318.last_plus_one= _temp1317 + 50; _temp1318;})); goto _LL1300;
_LL1300:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeof_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1004), 0), 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ,
e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r));}}} goto _LL848; _LL900: { struct _tuple1*
x1= _temp1012->name; int is_tagged_ptr= 0;{ void* _temp1319= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1325; struct Cyc_Absyn_Conref* _temp1327;
struct Cyc_Absyn_Tqual* _temp1329; struct Cyc_Absyn_Conref* _temp1331; void*
_temp1333; void* _temp1335; _LL1321: if(( unsigned int) _temp1319 > 5u?(( struct
_enum_struct*) _temp1319)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1326:
_temp1325=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1319)->f1; _LL1336: _temp1335=( void*) _temp1325.elt_typ; goto _LL1334;
_LL1334: _temp1333=( void*) _temp1325.rgn_typ; goto _LL1332; _LL1332: _temp1331=(
struct Cyc_Absyn_Conref*) _temp1325.nullable; goto _LL1330; _LL1330: _temp1329=(
struct Cyc_Absyn_Tqual*) _temp1325.tq; goto _LL1328; _LL1328: _temp1327=( struct
Cyc_Absyn_Conref*) _temp1325.bounds; goto _LL1322;} else{ goto _LL1323;} _LL1323:
goto _LL1324; _LL1322:{ void* _temp1337=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1327))->v; void*
_temp1343; _LL1339: if(( unsigned int) _temp1337 > 1u?(( struct _enum_struct*)
_temp1337)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1344: _temp1343=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1337)->f1; if( _temp1343 == Cyc_Absyn_Unknown_b){
goto _LL1340;} else{ goto _LL1341;}} else{ goto _LL1341;} _LL1341: goto _LL1342;
_LL1340: is_tagged_ptr= 1; goto _LL1338; _LL1342: goto _LL1338; _LL1338:;} goto
_LL1320; _LL1324:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1345=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1346; _temp1346.curr= _temp1345; _temp1346.base=
_temp1345; _temp1346.last_plus_one= _temp1345 + 42; _temp1346;})); goto _LL1320;
_LL1320:;}{ void* old_elt_typ=( void*)( _temp1008->topt)->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1010);{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp(
x1,({ struct Cyc_Absyn_Local_b_struct* _temp1347=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1347->tag= Cyc_Absyn_Local_b_tag;
_temp1347->f1= _temp1012;( void*) _temp1347;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1008);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1008, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1348=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1348->hd=( void*)({ struct _tuple11* _temp1349=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1349->f1= max;
_temp1349->f2= Cyc_Absyn_uint_t; _temp1349->f3=( struct Cyc_Absyn_Exp*)
_temp1010; _temp1349;}); _temp1348->tl= 0; _temp1348;}); struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeof_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0)), 0); decls=({
struct Cyc_List_List* _temp1350=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1350->hd=( void*)({ struct _tuple11* _temp1351=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1351->f1= a;
_temp1351->f2= ptr_typ; _temp1351->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1351;});
_temp1350->tl= decls; _temp1350;}); decls=({ struct Cyc_List_List* _temp1352=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1352->hd=(
void*)({ struct _tuple11* _temp1353=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp1353->f1= x1; _temp1353->f2= Cyc_Absyn_uint_t; _temp1353->f3=
0; _temp1353;}); _temp1352->tl= decls; _temp1352;}); if( is_tagged_ptr){ struct
_tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp*
binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1354=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1354->hd=(
void*)({ struct _tuple8* _temp1359=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1359->f1= 0; _temp1359->f2= Cyc_Absyn_var_exp( a, 0); _temp1359;});
_temp1354->tl=({ struct Cyc_List_List* _temp1355=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1355->hd=( void*)({ struct
_tuple8* _temp1358=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1358->f1= 0; _temp1358->f2= Cyc_Absyn_var_exp( a, 0); _temp1358;});
_temp1355->tl=({ struct Cyc_List_List* _temp1356=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1356->hd=( void*)({ struct
_tuple8* _temp1357=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1357->f1= 0; _temp1357->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1357;}); _temp1356->tl= 0; _temp1356;});
_temp1355;}); _temp1354;}), 0); decls=({ struct Cyc_List_List* _temp1360=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1360->hd=(
void*)({ struct _tuple11* _temp1361=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp1361->f1= b; _temp1361->f2= t; _temp1361->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1361;}); _temp1360->tl= decls; _temp1360;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Exp* _temp1364; void*
_temp1366; struct _tuple1* _temp1368; struct _tuple11 _temp1362=*(( struct
_tuple11*) ds->hd); _LL1369: _temp1368= _temp1362.f1; goto _LL1367; _LL1367:
_temp1366= _temp1362.f2; goto _LL1365; _LL1365: _temp1364= _temp1362.f3; goto
_LL1363; _LL1363: s= Cyc_Absyn_declare_stmt( _temp1368, _temp1366, _temp1364, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r)); goto
_LL848;}}}} _LL902: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, _temp1016, _temp1020))->r));} else{ struct _tuple1* n= _temp1020;
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp1371= _temp1014 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1370= Cyc_Absynpp_qvar2string(
_temp1020); xprintf("no structdecl %.*s", _temp1370.last_plus_one - _temp1370.curr,
_temp1370.curr);})): _temp1014; if( _temp1371 == 0){ _throw( Null_Exception);}
_temp1371;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1016,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields= fields->tl){ struct _tuple13 _temp1374;
struct Cyc_Absyn_Exp* _temp1375; struct _tuple4* _temp1377; struct _tuple4
_temp1379; void* _temp1380; struct Cyc_Absyn_Tqual* _temp1382; struct
_tagged_string* _temp1384; struct _tuple13* _temp1372=( struct _tuple13*) fields->hd;
_temp1374=* _temp1372; _LL1378: _temp1377= _temp1374.f1; _temp1379=* _temp1377;
_LL1385: _temp1384= _temp1379.f1; goto _LL1383; _LL1383: _temp1382= _temp1379.f2;
goto _LL1381; _LL1381: _temp1380= _temp1379.f3; goto _LL1376; _LL1376: _temp1375=
_temp1374.f2; goto _LL1373; _LL1373: Cyc_Toc_exp_to_c( nv, _temp1375); if( Cyc_Toc_is_void_star(
_temp1380)){( void*)( _temp1375->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1375->r, 0), 0))->r));}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1016, 0), 0))->r));} goto _LL848; _LL904: { struct _tuple1* qv= _temp1022->name;
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1024->fields == 0? 0:(
struct Cyc_List_List*)( _temp1024->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,((
struct Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs == 0){ tag_count ++;} fields= fields->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0), 0))->r));} goto _LL848;} _LL906: { struct _tuple1*
qv= _temp1032->name; struct Cyc_List_List* tqts= _temp1032->typs; struct _tuple1*
x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0);
void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1386=( char*)"_struct"; struct _tagged_string
_temp1387; _temp1387.curr= _temp1386; _temp1387.base= _temp1386; _temp1387.last_plus_one=
_temp1386 + 8; _temp1387;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Toc_mt_tq); if( nv->toplevel){ int tag_count= 0; struct Cyc_List_List*
fields= _temp1034->fields == 0? 0:( struct Cyc_List_List*)( _temp1034->fields)->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Enumfield*) fields->hd)->name)
!= 0) { if((( struct Cyc_Absyn_Enumfield*) fields->hd)->typs != 0){ tag_count ++;}
fields= fields->tl;}{ struct Cyc_List_List* dles= 0; for( 0; _temp1036 != 0;
_temp1036= _temp1036->tl, tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct
Cyc_Absyn_Exp*) _temp1036->hd; void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple6*) tqts->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1388=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1388[ 0]=({ struct Cyc_List_List _temp1389;
_temp1389.hd=( void*)({ struct _tuple8* _temp1390=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8) * 1); _temp1390[ 0]=({ struct _tuple8 _temp1391;
_temp1391.f1= 0; _temp1391.f2= cur_e; _temp1391;}); _temp1390;}); _temp1389.tl=
dles; _temp1389;}); _temp1388;});}{ struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0); dles=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1392=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1392[ 0]=({ struct Cyc_List_List _temp1393; _temp1393.hd=( void*)({ struct
_tuple8* _temp1394=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8) * 1);
_temp1394[ 0]=({ struct _tuple8 _temp1395; _temp1395.f1= 0; _temp1395.f2=
tag_exp; _temp1395;}); _temp1394;}); _temp1393.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1393;}); _temp1392;});{
struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles, 0); struct
Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x, strct_typ,( struct Cyc_Absyn_Exp*)
init_exp); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List* _temp1396=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1396->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1397=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1397->tag= Cyc_Absyn_Var_d_tag;
_temp1397->f1= vd;( void*) _temp1397;}), 0); _temp1396->tl= Cyc_Toc_temp_topdecls;
_temp1396;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( pstrct_typ,
Cyc_Absyn_address_exp( xexp, 0), 0))->r));}}}} else{ struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_sizeof_exp( strct_typ, 0); int atomic= 1; struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1398=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1398->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structarrow_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1399=( char*)"_tag"; struct
_tagged_string _temp1400; _temp1400.curr= _temp1399; _temp1400.base= _temp1399;
_temp1400.last_plus_one= _temp1399 + 5; _temp1400;})), 0), 0); _temp1398->tl= 0;
_temp1398;});{ int i= 1; for( 0; _temp1036 != 0;( _temp1036= _temp1036->tl, i ++),
tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1036->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple6*) tqts->hd)).f2); if( !
Cyc_Toc_atomic_typ(( void*)( cur_e->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,
cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp(
xexp,({ struct _tagged_string* _temp1401=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1401[ 0]= xprintf("f%d", i); _temp1401;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1402=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1402->hd=( void*) a; _temp1402->tl=(
struct Cyc_List_List*) rev_assign; _temp1402;});}}}{ struct Cyc_Absyn_Exp* e1=
Cyc_Absyn_cast_exp( pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr(
e2), 0); struct Cyc_Absyn_Stmt* final_cast= Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c( old_typ), xexp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1403=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1403->hd=( void*) final_cast; _temp1403->tl=( struct Cyc_List_List*)
rev_assign; _temp1403;})), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}}
goto _LL848;} _LL908: { struct _tuple1* qv= _temp1042->name; struct _tuple1* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1404=( char*)"_struct"; struct _tagged_string
_temp1405; _temp1405.curr= _temp1404; _temp1405.base= _temp1404; _temp1405.last_plus_one=
_temp1404 + 8; _temp1405;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1406=( char*)"_tag"; struct
_tagged_string _temp1407; _temp1407.curr= _temp1406; _temp1407.base= _temp1406;
_temp1407.last_plus_one= _temp1406 + 5; _temp1407;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1046); dles=({ struct Cyc_List_List*
_temp1408=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1408->hd=( void*)({ struct _tuple8* _temp1409=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1409->f1= 0; _temp1409->f2= tag_exp; _temp1409;});
_temp1408->tl= dles; _temp1408;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1410=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1410->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1411=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1411->tag= Cyc_Absyn_Var_d_tag; _temp1411->f1= vd;( void*) _temp1411;}), 0);
_temp1410->tl= Cyc_Toc_temp_topdecls; _temp1410;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), Cyc_Absyn_address_exp(
xexp, 0), 0))->r));}} else{ int atomic= 1; struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeof_exp(
strct_typ, 0); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1046 != 0;
_temp1046= _temp1046->tl, i ++){ if( ! Cyc_Toc_atomic_typ(( void*)((( struct Cyc_Absyn_Exp*)
_temp1046->hd)->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_temp1046->hd);{ struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1412=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1412->hd=(
void*)({ struct Cyc_Absyn_FieldName_struct* _temp1413=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1413->tag= Cyc_Absyn_FieldName_tag;
_temp1413->f1=({ struct _tagged_string* _temp1414=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1414[ 0]= xprintf("f%d", i);
_temp1414;});( void*) _temp1413;}); _temp1412->tl= 0; _temp1412;}); dles=({
struct Cyc_List_List* _temp1415=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1415->hd=( void*)({ struct _tuple8* _temp1416=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1416->f1=( struct Cyc_List_List*)
des; _temp1416->f2=( struct Cyc_Absyn_Exp*) _temp1046->hd; _temp1416;});
_temp1415->tl= dles; _temp1415;});}}} dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( dles);{ struct Cyc_Absyn_Exp* e1= Cyc_Absyn_cast_exp(
pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr( e2), 0);
struct Cyc_List_List* d=({ struct Cyc_List_List* _temp1417=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1417->hd=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp1418=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1418->tag= Cyc_Absyn_FieldName_tag; _temp1418->f1= Cyc_Toc_tag_sp;( void*)
_temp1418;}); _temp1417->tl= 0; _temp1417;}); struct Cyc_List_List* dles2=({
struct Cyc_List_List* _temp1419=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1419->hd=( void*)({ struct _tuple8* _temp1420=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1420->f1=( struct Cyc_List_List*)
d; _temp1420->f2= tag_exp; _temp1420;}); _temp1419->tl= dles; _temp1419;});
struct Cyc_Absyn_Exp* strct_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles2, 0);
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_deref_exp( xexp, 0),
Cyc_Absyn_cast_exp( strct_typ, strct_exp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s1, Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), xexp, 0),
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}} goto _LL848;}
_LL910: Cyc_Toc_stmt_to_c( nv, _temp1050); goto _LL848; _LL912:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1421=( char*)"UnresolvedMem"; struct _tagged_string _temp1422; _temp1422.curr=
_temp1421; _temp1422.base= _temp1421; _temp1422.last_plus_one= _temp1421 + 14;
_temp1422;})); goto _LL848; _LL914:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1423=( char*)"codegen"; struct
_tagged_string _temp1424; _temp1424.curr= _temp1423; _temp1424.base= _temp1423;
_temp1424.last_plus_one= _temp1423 + 8; _temp1424;})); goto _LL848; _LL916:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1425=( char*)"fill"; struct _tagged_string _temp1426; _temp1426.curr=
_temp1425; _temp1426.base= _temp1425; _temp1426.last_plus_one= _temp1425 + 5;
_temp1426;})); goto _LL848; _LL848:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple14 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1427=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1459; struct Cyc_Absyn_Vardecl _temp1461; struct Cyc_Core_Opt* _temp1462;
int _temp1464; struct Cyc_Absyn_Exp* _temp1466; void* _temp1468; struct Cyc_Absyn_Tqual*
_temp1470; struct _tuple1* _temp1472; void* _temp1474; struct Cyc_Absyn_Vardecl*
_temp1476; int _temp1478; void* _temp1480; char _temp1482; struct _tagged_string
_temp1484; struct Cyc_Absyn_Enumfield* _temp1486; struct Cyc_Absyn_Enumdecl*
_temp1488; struct Cyc_List_List* _temp1490; struct Cyc_List_List* _temp1492;
struct Cyc_Core_Opt* _temp1494; struct _tuple1* _temp1496; struct Cyc_Absyn_Enumfield*
_temp1498; struct Cyc_Absyn_Xenumdecl* _temp1500; struct Cyc_List_List*
_temp1502; struct Cyc_List_List* _temp1504; struct _tuple1* _temp1506; struct
Cyc_List_List* _temp1508; struct Cyc_List_List* _temp1510; struct Cyc_List_List*
_temp1512; struct Cyc_Core_Opt* _temp1514; struct Cyc_Absyn_Structdecl*
_temp1516; struct Cyc_Absyn_Pat* _temp1518; struct _tuple1* _temp1520; struct
Cyc_List_List* _temp1522; struct Cyc_List_List* _temp1524; struct _tuple1*
_temp1526; struct Cyc_List_List* _temp1528; struct Cyc_List_List* _temp1530;
struct _tuple1* _temp1532; _LL1429: if( _temp1427 == Cyc_Absyn_Wild_p){ goto
_LL1430;} else{ goto _LL1431;} _LL1431: if(( unsigned int) _temp1427 > 2u?((
struct _enum_struct*) _temp1427)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1460:
_temp1459=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1427)->f1; _temp1461=* _temp1459; _LL1475: _temp1474=( void*) _temp1461.sc;
goto _LL1473; _LL1473: _temp1472=( struct _tuple1*) _temp1461.name; goto _LL1471;
_LL1471: _temp1470=( struct Cyc_Absyn_Tqual*) _temp1461.tq; goto _LL1469;
_LL1469: _temp1468=( void*) _temp1461.type; goto _LL1467; _LL1467: _temp1466=(
struct Cyc_Absyn_Exp*) _temp1461.initializer; goto _LL1465; _LL1465: _temp1464=(
int) _temp1461.shadow; goto _LL1463; _LL1463: _temp1462=( struct Cyc_Core_Opt*)
_temp1461.region; goto _LL1432;} else{ goto _LL1433;} _LL1433: if(( unsigned int)
_temp1427 > 2u?(( struct _enum_struct*) _temp1427)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1477: _temp1476=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1427)->f1; goto _LL1434;} else{ goto _LL1435;} _LL1435: if( _temp1427 ==
Cyc_Absyn_Null_p){ goto _LL1436;} else{ goto _LL1437;} _LL1437: if((
unsigned int) _temp1427 > 2u?(( struct _enum_struct*) _temp1427)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1481: _temp1480=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1427)->f1;
goto _LL1479; _LL1479: _temp1478=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1427)->f2; goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( unsigned int)
_temp1427 > 2u?(( struct _enum_struct*) _temp1427)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1483: _temp1482=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1427)->f1;
goto _LL1440;} else{ goto _LL1441;} _LL1441: if(( unsigned int) _temp1427 > 2u?((
struct _enum_struct*) _temp1427)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1485:
_temp1484=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1427)->f1;
goto _LL1442;} else{ goto _LL1443;} _LL1443: if(( unsigned int) _temp1427 > 2u?((
struct _enum_struct*) _temp1427)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1497:
_temp1496=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1427)->f1;
goto _LL1495; _LL1495: _temp1494=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1427)->f2; goto _LL1493; _LL1493: _temp1492=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1427)->f3; goto _LL1491; _LL1491:
_temp1490=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1427)->f4;
goto _LL1489; _LL1489: _temp1488=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1427)->f5; goto _LL1487; _LL1487: _temp1486=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1427)->f6; goto _LL1444;} else{ goto
_LL1445;} _LL1445: if(( unsigned int) _temp1427 > 2u?(( struct _enum_struct*)
_temp1427)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1507: _temp1506=( struct
_tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1427)->f1; goto _LL1505;
_LL1505: _temp1504=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1427)->f2; goto _LL1503; _LL1503: _temp1502=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1427)->f3; goto _LL1501; _LL1501:
_temp1500=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1427)->f4; goto _LL1499; _LL1499: _temp1498=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1427)->f5; goto _LL1446;} else{ goto
_LL1447;} _LL1447: if(( unsigned int) _temp1427 > 2u?(( struct _enum_struct*)
_temp1427)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1509: _temp1508=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp1427)->f1; goto _LL1448;} else{ goto
_LL1449;} _LL1449: if(( unsigned int) _temp1427 > 2u?(( struct _enum_struct*)
_temp1427)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL1517: _temp1516=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1427)->f1; goto _LL1515; _LL1515:
_temp1514=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1427)->f2;
goto _LL1513; _LL1513: _temp1512=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1427)->f3; goto _LL1511; _LL1511: _temp1510=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1427)->f4; goto _LL1450;} else{ goto
_LL1451;} _LL1451: if(( unsigned int) _temp1427 > 2u?(( struct _enum_struct*)
_temp1427)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1519: _temp1518=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1427)->f1; goto _LL1452;} else{ goto
_LL1453;} _LL1453: if(( unsigned int) _temp1427 > 2u?(( struct _enum_struct*)
_temp1427)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1521: _temp1520=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1427)->f1; goto _LL1454;}
else{ goto _LL1455;} _LL1455: if(( unsigned int) _temp1427 > 2u?(( struct
_enum_struct*) _temp1427)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1527:
_temp1526=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1427)->f1;
goto _LL1525; _LL1525: _temp1524=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1427)->f2; goto _LL1523; _LL1523: _temp1522=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1427)->f3; goto _LL1456;} else{
goto _LL1457;} _LL1457: if(( unsigned int) _temp1427 > 2u?(( struct _enum_struct*)
_temp1427)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1533: _temp1532=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1427)->f1;
goto _LL1531; _LL1531: _temp1530=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1427)->f2; goto _LL1529; _LL1529: _temp1528=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1427)->f3; goto _LL1458;} else{
goto _LL1428;} _LL1430: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1428;
_LL1432: nv= Cyc_Toc_add_varmap( nv, _temp1472, r); s= Cyc_Absyn_goto_stmt(
succ_lab, 0); goto _LL1428; _LL1434: { struct _tuple1* v= Cyc_Toc_temp_var();
decls=({ struct Cyc_List_List* _temp1534=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1534->hd=( void*)({ struct _tuple15*
_temp1535=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1535->f1=
v; _temp1535->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1535;}); _temp1534->tl= decls; _temp1534;}); nv= Cyc_Toc_add_varmap( nv,
_temp1476->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1428;} _LL1436: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1428; _LL1438: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1480, _temp1478, 0), succ_lab, fail_lab); goto _LL1428;
_LL1440: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1482, 0),
succ_lab, fail_lab); goto _LL1428; _LL1442: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1484, 0), succ_lab, fail_lab); goto _LL1428; _LL1444: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1490);
struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1486->name,( struct
_tagged_string)({ char* _temp1536=( char*)"_struct"; struct _tagged_string
_temp1537; _temp1537.curr= _temp1536; _temp1537.base= _temp1536; _temp1537.last_plus_one=
_temp1536 + 8; _temp1537;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1490); for( 0; rps != 0; rps= rps->tl, cnt --){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=(
void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string*
lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp1538=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1538->hd=( void*)({
struct _tuple15* _temp1539=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp1539->f1= v; _temp1539->f2= t2c; _temp1539;}); _temp1538->tl=
decls; _temp1538;});{ struct _tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1540=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1540[ 0]= xprintf("f%d", cnt); _temp1540;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp(
t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1541=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1541[ 0]= xprintf("f%d", cnt); _temp1541;}), 0), 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List* _temp1542=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1542->hd=(
void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1542->tl= ss; _temp1542;});
succ_lab= lab;}}}} if( _temp1490 == 0){ s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_var_exp(
_temp1496, 0), succ_lab, fail_lab);} else{ unsigned int max_tag= 0;{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)( _temp1488->fields)->v; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->typs
== 0){ if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1543=( char*)"can't deal with explicit tags yet";
struct _tagged_string _temp1544; _temp1544.curr= _temp1543; _temp1544.base=
_temp1543; _temp1544.last_plus_one= _temp1543 + 34; _temp1544;}));} max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__enum_struct_sp),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp( e3,
Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1496,( struct _tagged_string)({ char* _temp1545=(
char*)"_tag"; struct _tagged_string _temp1546; _temp1546.curr= _temp1545;
_temp1546.base= _temp1545; _temp1546.last_plus_one= _temp1545 + 5; _temp1546;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag >( unsigned int) 0? Cyc_Absyn_and_exp(
e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}}
goto _LL1428;} _LL1446: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1502); struct _tuple1* xfstrct= Cyc_Toc_collapse_qvar_tag(
_temp1498->name,( struct _tagged_string)({ char* _temp1547=( char*)"_struct";
struct _tagged_string _temp1548; _temp1548.curr= _temp1547; _temp1548.base=
_temp1547; _temp1548.last_plus_one= _temp1547 + 8; _temp1548;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1502); for( 0; rps != 0; rps= rps->tl, cnt --){ struct
Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1549=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1549->hd=( void*)({ struct _tuple15*
_temp1550=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1550->f1=
v; _temp1550->f2= Cyc_Toc_typ_to_c( t2); _temp1550;}); _temp1549->tl= decls;
_temp1549;});{ struct _tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1551=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1551[ 0]= xprintf("f%d", cnt); _temp1551;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1552=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1552[ 0]= xprintf("f%d", cnt);
_temp1552;}), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0);
ss=({ struct Cyc_List_List* _temp1553=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1553->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1553->tl= ss; _temp1553;}); succ_lab= lab;}}}} if( _temp1502 == 0){
s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0),
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1506,( struct _tagged_string)({
char* _temp1554=( char*)"_tag"; struct _tagged_string _temp1555; _temp1555.curr=
_temp1554; _temp1555.base= _temp1554; _temp1555.last_plus_one= _temp1554 + 5;
_temp1555;})), 0), succ_lab, fail_lab);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structmember_exp( e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e=
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1506,( struct _tagged_string)({
char* _temp1556=( char*)"_tag"; struct _tagged_string _temp1557; _temp1557.curr=
_temp1556; _temp1557.base= _temp1556; _temp1557.last_plus_one= _temp1556 + 5;
_temp1557;})), 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0),
Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} goto
_LL1428;} _LL1448: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1508);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1508); for( 0; rps != 0; rps= rps->tl,
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct
_tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1558=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1558->hd=( void*)({ struct _tuple15* _temp1559=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1559->f1= v; _temp1559->f2= Cyc_Toc_typ_to_c(
t2); _temp1559;}); _temp1558->tl= decls; _temp1558;});{ struct _tuple14 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1560=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1560[ 0]= xprintf("f%d", cnt);
_temp1560;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1561=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1561[ 0]= xprintf("f%d", cnt); _temp1561;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1562=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1562->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1562->tl= ss; _temp1562;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1428;} _LL1450: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1510); for( 0; dlps != 0; dlps= dlps->tl){ struct _tuple16* tup=( struct
_tuple16*) dlps->hd; struct Cyc_Absyn_Pat* p2=(* tup).f2; struct _tagged_string*
f;{ void* _temp1563=( void*)((* tup).f1)->hd; struct _tagged_string* _temp1569;
_LL1565: if((( struct _enum_struct*) _temp1563)->tag == Cyc_Absyn_FieldName_tag){
_LL1570: _temp1569=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1563)->f1; goto _LL1566;} else{ goto _LL1567;} _LL1567: goto _LL1568;
_LL1566: f= _temp1569; goto _LL1564; _LL1568:( void) _throw(({ struct Cyc_Toc_Match_error_struct*
_temp1571=( struct Cyc_Toc_Match_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Match_error_struct));* _temp1571=( struct Cyc_Toc_Match_error_struct){.tag=
Cyc_Toc_Match_error_tag};( struct _xenum_struct*) _temp1571;})); _LL1564:;}{
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; void*
t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1572=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1572->hd=( void*)({ struct _tuple15*
_temp1573=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1573->f1=
v; _temp1573->f2= t2c; _temp1573;}); _temp1572->tl= decls; _temp1572;});{ struct
_tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path, f, 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1574->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1574->tl= ss; _temp1574;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1428;} _LL1452: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( _temp1518->topt)->v; decls=({ struct Cyc_List_List* _temp1575=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1575->hd=(
void*)({ struct _tuple15* _temp1576=( struct _tuple15*) GC_malloc( sizeof(
struct _tuple15)); _temp1576->f1= v; _temp1576->f2= Cyc_Toc_typ_to_c( t2);
_temp1576;}); _temp1575->tl= decls; _temp1575;});{ struct _tuple14 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path, 0), _temp1518,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0), s2, 0); if( Cyc_Toc_really_is_nullable(
t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);} else{ s= s3;} goto
_LL1428;}}} _LL1454: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1577=( char*)"unknownid";
struct _tagged_string _temp1578; _temp1578.curr= _temp1577; _temp1578.base=
_temp1577; _temp1578.last_plus_one= _temp1577 + 10; _temp1578;})); goto _LL1428;
_LL1456: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1579=( char*)"unknowncall"; struct
_tagged_string _temp1580; _temp1580.curr= _temp1579; _temp1580.base= _temp1579;
_temp1580.last_plus_one= _temp1579 + 12; _temp1580;})); goto _LL1428; _LL1458: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1581=( char*)"unknownfields"; struct _tagged_string
_temp1582; _temp1582.curr= _temp1581; _temp1582.base= _temp1581; _temp1582.last_plus_one=
_temp1581 + 14; _temp1582;})); goto _LL1428; _LL1428:;} return({ struct _tuple14
_temp1583; _temp1583.f1= nv; _temp1583.f2= decls; _temp1583.f3= s; _temp1583;});}
static struct _tuple17* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){
return({ struct _tuple17* _temp1584=( struct _tuple17*) GC_malloc( sizeof(
struct _tuple17)); _temp1584->f1= Cyc_Toc_fresh_label(); _temp1584->f2= Cyc_Toc_fresh_label();
_temp1584->f3= sc; _temp1584;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)( e->topt)->v; int
leave_as_switch;{ void* _temp1585= Cyc_Tcutil_compress( t); void* _temp1591;
void* _temp1593; _LL1587: if(( unsigned int) _temp1585 > 5u?(( struct
_enum_struct*) _temp1585)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1594: _temp1593=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1585)->f1; goto _LL1592; _LL1592:
_temp1591=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1585)->f2; goto
_LL1588;} else{ goto _LL1589;} _LL1589: goto _LL1590; _LL1588: leave_as_switch=
1; goto _LL1586; _LL1590: leave_as_switch= 0; goto _LL1586; _LL1586:;}{ struct
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
succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1595=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1595->hd=( void*) res.f1;
_temp1595->tl= nvs; _temp1595;}); decls= res.f2; test_stmts=({ struct Cyc_List_List*
_temp1596=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1596->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple17*) lscs2->hd)).f1,
res.f3, 0); _temp1596->tl= test_stmts; _temp1596;});} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1597= sc->where_clause;
if( _temp1597 == 0){ _throw( Null_Exception);} _temp1597;}); struct
_tagged_string* intermed_lab= Cyc_Toc_fresh_label(); struct _tuple14 res= Cyc_Toc_xlate_pat(
nv, t, r, path, sc->pattern, intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c(
res.f1, e); nvs=({ struct Cyc_List_List* _temp1598=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1598->hd=( void*) res.f1;
_temp1598->tl= nvs; _temp1598;}); decls= res.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt(
e, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( intermed_lab, s2, 0);
test_stmts=({ struct Cyc_List_List* _temp1599=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1599->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple17*) lscs2->hd)).f1, Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0);
_temp1599->tl= test_stmts; _temp1599;});}}}}{ struct Cyc_Absyn_Stmt* test_stmt=
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
decls; decls != 0; decls= decls->tl){ struct _tuple15 _temp1602; void* _temp1603;
struct _tuple1* _temp1605; struct _tuple15* _temp1600=( struct _tuple15*) decls->hd;
_temp1602=* _temp1600; _LL1606: _temp1605= _temp1602.f1; goto _LL1604; _LL1604:
_temp1603= _temp1602.f2; goto _LL1601; _LL1601: res= Cyc_Absyn_declare_stmt(
_temp1605, _temp1603, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)( e->topt)->v),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple18 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1607=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1607[ 0]=({ struct Cyc_List_List _temp1608;
_temp1608.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0); _temp1608.tl=
0; _temp1608;}); _temp1607;}), 0), 0);} void Cyc_Toc_do_npop_before( int n,
struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}}
static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s){
while( 1) { void* _temp1609=( void*) s->r; struct Cyc_Absyn_Exp* _temp1647;
struct Cyc_Absyn_Stmt* _temp1649; struct Cyc_Absyn_Stmt* _temp1651; struct Cyc_Absyn_Exp*
_temp1653; struct Cyc_Absyn_Stmt* _temp1655; struct Cyc_Absyn_Stmt* _temp1657;
struct Cyc_Absyn_Exp* _temp1659; struct Cyc_Absyn_Stmt* _temp1661; struct
_tuple3 _temp1663; struct Cyc_Absyn_Stmt* _temp1665; struct Cyc_Absyn_Exp*
_temp1667; struct Cyc_Absyn_Stmt* _temp1669; struct Cyc_Absyn_Stmt* _temp1671;
struct Cyc_Absyn_Stmt* _temp1673; struct _tagged_string* _temp1675; struct Cyc_Absyn_Stmt*
_temp1677; struct _tuple3 _temp1679; struct Cyc_Absyn_Stmt* _temp1681; struct
Cyc_Absyn_Exp* _temp1683; struct _tuple3 _temp1685; struct Cyc_Absyn_Stmt*
_temp1687; struct Cyc_Absyn_Exp* _temp1689; struct Cyc_Absyn_Exp* _temp1691;
struct Cyc_List_List* _temp1693; struct Cyc_Absyn_Exp* _temp1695; struct Cyc_Absyn_Switch_clause**
_temp1697; struct Cyc_List_List* _temp1699; struct Cyc_Absyn_Stmt* _temp1701;
struct Cyc_Absyn_Decl* _temp1703; struct Cyc_Absyn_Stmt* _temp1705; struct Cyc_Absyn_Stmt*
_temp1707; struct Cyc_Absyn_Stmt* _temp1709; struct _tagged_string* _temp1711;
struct _tuple3 _temp1713; struct Cyc_Absyn_Stmt* _temp1715; struct Cyc_Absyn_Exp*
_temp1717; struct Cyc_Absyn_Stmt* _temp1719; struct Cyc_List_List* _temp1721;
struct Cyc_Absyn_Stmt* _temp1723; _LL1611: if( _temp1609 == Cyc_Absyn_Skip_s){
goto _LL1612;} else{ goto _LL1613;} _LL1613: if(( unsigned int) _temp1609 > 1u?((
struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1648:
_temp1647=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1609)->f1;
goto _LL1614;} else{ goto _LL1615;} _LL1615: if(( unsigned int) _temp1609 > 1u?((
struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1652:
_temp1651=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1609)->f1;
goto _LL1650; _LL1650: _temp1649=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1609)->f2; goto _LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int)
_temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1654: _temp1653=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1609)->f1; goto _LL1618;} else{ goto _LL1619;} _LL1619: if(( unsigned int)
_temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1660: _temp1659=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1609)->f1; goto _LL1658; _LL1658: _temp1657=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1609)->f2; goto _LL1656; _LL1656:
_temp1655=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1609)->f3; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int)
_temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1664: _temp1663=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1609)->f1; _LL1668: _temp1667= _temp1663.f1; goto _LL1666; _LL1666:
_temp1665= _temp1663.f2; goto _LL1662; _LL1662: _temp1661=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1609)->f2; goto _LL1622;} else{ goto
_LL1623;} _LL1623: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1670: _temp1669=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1609)->f1; goto _LL1624;} else{ goto
_LL1625;} _LL1625: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1672: _temp1671=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1609)->f1; goto _LL1626;} else{ goto
_LL1627;} _LL1627: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1676: _temp1675=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1609)->f1; goto _LL1674;
_LL1674: _temp1673=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1609)->f2; goto _LL1628;} else{ goto _LL1629;} _LL1629: if(( unsigned int)
_temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1692: _temp1691=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1609)->f1; goto _LL1686; _LL1686: _temp1685=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1609)->f2; _LL1690: _temp1689= _temp1685.f1; goto _LL1688; _LL1688:
_temp1687= _temp1685.f2; goto _LL1680; _LL1680: _temp1679=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1609)->f3; _LL1684: _temp1683= _temp1679.f1;
goto _LL1682; _LL1682: _temp1681= _temp1679.f2; goto _LL1678; _LL1678: _temp1677=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1609)->f4; goto
_LL1630;} else{ goto _LL1631;} _LL1631: if(( unsigned int) _temp1609 > 1u?((
struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1696:
_temp1695=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1609)->f1; goto _LL1694; _LL1694: _temp1693=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1609)->f2; goto _LL1632;} else{ goto
_LL1633;} _LL1633: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1700: _temp1699=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1609)->f1; goto _LL1698; _LL1698:
_temp1697=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1609)->f2; goto _LL1634;} else{ goto _LL1635;} _LL1635: if(( unsigned int)
_temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1704: _temp1703=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1609)->f1; goto _LL1702; _LL1702: _temp1701=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1609)->f2; goto _LL1636;} else{ goto
_LL1637;} _LL1637: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1706: _temp1705=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1609)->f1; goto _LL1638;} else{ goto
_LL1639;} _LL1639: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1708: _temp1707=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1609)->f1; goto _LL1640;} else{ goto
_LL1641;} _LL1641: if(( unsigned int) _temp1609 > 1u?(( struct _enum_struct*)
_temp1609)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1712: _temp1711=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1609)->f1; goto
_LL1710; _LL1710: _temp1709=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1609)->f2; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(( unsigned int)
_temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1720: _temp1719=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1609)->f1; goto _LL1714; _LL1714: _temp1713=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1609)->f2; _LL1718: _temp1717= _temp1713.f1; goto _LL1716; _LL1716:
_temp1715= _temp1713.f2; goto _LL1644;} else{ goto _LL1645;} _LL1645: if((
unsigned int) _temp1609 > 1u?(( struct _enum_struct*) _temp1609)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1724: _temp1723=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1609)->f1; goto _LL1722; _LL1722: _temp1721=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1609)->f2; goto _LL1646;} else{ goto
_LL1610;} _LL1612: return; _LL1614: Cyc_Toc_exp_to_c( nv, _temp1647); return;
_LL1616: Cyc_Toc_stmt_to_c( nv, _temp1651); s= _temp1649; continue; _LL1618: {
struct Cyc_Core_Opt* topt= 0; if( _temp1653 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1725=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1725->v=( void*) Cyc_Toc_typ_to_c(( void*)( _temp1653->topt)->v); _temp1725;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1726=
_temp1653; if( _temp1726 == 0){ _throw( Null_Exception);} _temp1726;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*) topt->v,
_temp1653, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth),
retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth, s);}}
return;} _LL1620: Cyc_Toc_exp_to_c( nv, _temp1659); Cyc_Toc_stmt_to_c( nv,
_temp1657); s= _temp1655; continue; _LL1622: Cyc_Toc_exp_to_c( nv, _temp1667);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1661); return; _LL1624: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)( nv->break_lab)->v, 0))->r));}{ int dest_depth= _temp1669 == 0?
0: _temp1669->try_depth; Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s);
return;} _LL1626: if( nv->continue_lab != 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_string*)( nv->continue_lab)->v, 0))->r));}
Cyc_Toc_do_npop_before( s->try_depth - _temp1671->try_depth, s); return; _LL1628:
Cyc_Toc_do_npop_before( s->try_depth - _temp1673->try_depth, s); return; _LL1630:
Cyc_Toc_exp_to_c( nv, _temp1691); Cyc_Toc_exp_to_c( nv, _temp1689); Cyc_Toc_exp_to_c(
nv, _temp1683); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1677); return;
_LL1632: Cyc_Toc_xlate_switch( nv, s, _temp1695, _temp1693); return; _LL1634:
if( nv->fallthru_info == 0){( void) _throw((( struct _xenum_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1727=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp1728; _temp1728.curr=
_temp1727; _temp1728.base= _temp1727; _temp1728.last_plus_one= _temp1727 + 29;
_temp1728;})));}{ struct _tuple9 _temp1731; struct Cyc_Dict_Dict* _temp1732;
struct Cyc_List_List* _temp1734; struct _tagged_string* _temp1736; struct
_tuple9* _temp1729=( struct _tuple9*)( nv->fallthru_info)->v; _temp1731=*
_temp1729; _LL1737: _temp1736= _temp1731.f1; goto _LL1735; _LL1735: _temp1734=
_temp1731.f2; goto _LL1733; _LL1733: _temp1732= _temp1731.f3; goto _LL1730;
_LL1730: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1736, 0); Cyc_Toc_do_npop_before(
s->try_depth -((* _temp1697)->body)->try_depth, s2);{ struct Cyc_List_List* vs2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1734);
struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp1699); for( 0; vs2 != 0; vs2= vs2->tl, es2= es2->tl){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) es2->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
_temp1732,( struct _tuple1*) vs2->hd),( struct Cyc_Absyn_Exp*) es2->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1636:{ void*
_temp1738=( void*) _temp1703->r; int _temp1744; struct Cyc_Absyn_Exp* _temp1746;
struct Cyc_Core_Opt* _temp1748; struct Cyc_Core_Opt* _temp1750; struct Cyc_Absyn_Pat*
_temp1752; _LL1740: if(( unsigned int) _temp1738 > 1u?(( struct _enum_struct*)
_temp1738)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1753: _temp1752=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1738)->f1; goto _LL1751; _LL1751: _temp1750=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1738)->f2; goto
_LL1749; _LL1749: _temp1748=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1738)->f3; goto _LL1747; _LL1747: _temp1746=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1738)->f4; goto _LL1745; _LL1745: _temp1744=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1738)->f5; goto _LL1741;} else{ goto
_LL1742;} _LL1742: goto _LL1743; _LL1741: { int var_case= 0;{ void* _temp1754=(
void*) _temp1752->r; struct Cyc_Absyn_Vardecl* _temp1760; _LL1756: if((
unsigned int) _temp1754 > 2u?(( struct _enum_struct*) _temp1754)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL1761: _temp1760=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1754)->f1; goto _LL1757;} else{ goto _LL1758;} _LL1758: goto _LL1759;
_LL1757: var_case= 1; _temp1760->initializer=( struct Cyc_Absyn_Exp*) _temp1746;(
void*)( _temp1703->r=( void*)({ struct Cyc_Absyn_Var_d_struct* _temp1762=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1762->tag= Cyc_Absyn_Var_d_tag; _temp1762->f1= _temp1760;( void*) _temp1762;}));
goto _LL1755; _LL1759: goto _LL1755; _LL1755:;} if( var_case){ goto _LL1743;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp1752, _temp1748,
_temp1746, _temp1744, _temp1701))->r)); goto _LL1739;} _LL1743: { struct Cyc_List_List*
_temp1766; struct Cyc_Toc_Env* _temp1768; struct _tuple18 _temp1764= Cyc_Toc_decls_to_c(
nv,({ struct Cyc_List_List* _temp1763=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1763->hd=( void*) _temp1703; _temp1763->tl=
0; _temp1763;}), 0); _LL1769: _temp1768= _temp1764.f1; goto _LL1767; _LL1767:
_temp1766= _temp1764.f2; goto _LL1765; _LL1765: Cyc_Toc_stmt_to_c( _temp1768,
_temp1701); for( 0; _temp1766 != 0; _temp1766= _temp1766->tl){ _temp1701= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*) _temp1766->hd, _temp1701, 0);}( void*)( s->r=( void*)((
void*) _temp1701->r)); goto _LL1739;} _LL1739:;} return; _LL1638:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1770=( char*)"cut"; struct _tagged_string _temp1771; _temp1771.curr=
_temp1770; _temp1771.base= _temp1770; _temp1771.last_plus_one= _temp1770 + 4;
_temp1771;})); return; _LL1640:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1772=( char*)"splice"; struct _tagged_string
_temp1773; _temp1773.curr= _temp1772; _temp1773.base= _temp1772; _temp1773.last_plus_one=
_temp1772 + 7; _temp1773;})); return; _LL1642: s= _temp1709; continue; _LL1644:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1719); Cyc_Toc_exp_to_c( nv,
_temp1717); return; _LL1646: { struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp1774=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1774->f1= Cyc_Toc_abs_ns; _temp1774->f2= Cyc_Toc__push_handler_sp;
_temp1774;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1775=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1775->f1= Cyc_Toc_abs_ns; _temp1775->f2= Cyc_Toc_setjmp_sp; _temp1775;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1*
_temp1776=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1776->f1=
Cyc_Toc_abs_ns; _temp1776->f2= Cyc_Toc__pop_handler_sp; _temp1776;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp1777=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1777->v=( void*) e_typ; _temp1777;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1723);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp1723, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp1778=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1778->v=( void*)
Cyc_Absyn_exn_typ; _temp1778;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp1779=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp1779->r=( void*)({ struct Cyc_Absyn_Var_p_struct* _temp1781=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp1781->tag= Cyc_Absyn_Var_p_tag;
_temp1781->f1= x_vd;( void*) _temp1781;}); _temp1779->topt=({ struct Cyc_Core_Opt*
_temp1780=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1780->v=( void*) Cyc_Absyn_exn_typ; _temp1780;}); _temp1779->loc= 0;
_temp1779;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp1782=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1782->v=( void*) Cyc_Absyn_VoidType; _temp1782;});{ struct
Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp1783=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp1783->pattern= p;
_temp1783->pat_vars=({ struct Cyc_Core_Opt* _temp1784=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1784->v=( void*)({ struct Cyc_List_List*
_temp1785=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1785->hd=( void*) x_vd; _temp1785->tl= 0; _temp1785;}); _temp1784;});
_temp1783->where_clause= 0; _temp1783->body= rs; _temp1783->loc= 0; _temp1783;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1721,({
struct Cyc_List_List* _temp1786=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1786->hd=( void*) default_case; _temp1786->tl= 0;
_temp1786;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp1787=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1787->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1787->tl= 0; _temp1787;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp1788=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1788->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp1788->tl= 0;
_temp1788;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp( Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL1610:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args= args->tl){(*(( struct _tuple4*) args->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple4*) args->hd)).f3);{ struct _tuple1* x=({ struct _tuple1* _temp1789=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1789->f1= Cyc_Absyn_Loc_n;
_temp1789->f2=(*(( struct _tuple4*) args->hd)).f1; _temp1789;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1790= s;
_LL1792: if( _temp1790 == Cyc_Absyn_Abstract){ goto _LL1793;} else{ goto _LL1794;}
_LL1794: if( _temp1790 == Cyc_Absyn_ExternC){ goto _LL1795;} else{ goto _LL1796;}
_LL1796: goto _LL1797; _LL1793: return Cyc_Absyn_Public; _LL1795: return Cyc_Absyn_Extern;
_LL1797: return s; _LL1791:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0;
static void Cyc_Toc_structdecl_to_c( struct Cyc_Absyn_Structdecl* s){ struct
_tuple1* n=( struct _tuple1*)( s->name)->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1798=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1798->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp1798;});}{ int seen_defn_before; struct Cyc_Core_Opt*
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
struct Cyc_Core_Opt* _temp1799=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1799->v=( void*) n; _temp1799;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)( s->fields)->v; for( 0; fields != 0; fields= fields->tl){(*((
struct _tuple4*) fields->hd)).f3= Cyc_Toc_typ_to_c_array((*(( struct _tuple4*)
fields->hd)).f3);}}}}} static struct Cyc_Core_Opt* Cyc_Toc_enums_so_far= 0;
static struct Cyc_List_List* Cyc_Toc_enumdecl_to_c( struct Cyc_Absyn_Enumdecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)( ed->name)->v;
if( Cyc_Toc_enums_so_far == 0){ Cyc_Toc_enums_so_far=({ struct Cyc_Core_Opt*
_temp1800=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1800->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1800;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
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
_temp1801=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1801->name= n; _temp1801->tvs= 0; _temp1801->defn=( void*) Cyc_Absyn_void_star_typ();
_temp1801;}); res=({ struct Cyc_List_List* _temp1802=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1802->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1803=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1803->tag= Cyc_Absyn_Typedef_d_tag;
_temp1803->f1= tdef;( void*) _temp1803;}), 0); _temp1802->tl= res; _temp1802;});}{
void* t=({ struct Cyc_Absyn_TypedefType_struct* _temp1804=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1804->tag= Cyc_Absyn_TypedefType_tag;
_temp1804->f1= n; _temp1804->f2= 0; _temp1804->f3=({ struct Cyc_Core_Opt*
_temp1805=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1805->v=( void*) Cyc_Absyn_void_star_typ(); _temp1805;});( void*) _temp1804;});
int tag_count= 0; int box_count= 0;{ struct Cyc_List_List* fields= ed->fields ==
0? 0:( struct Cyc_List_List*)( ed->fields)->v; for( 0; fields != 0; fields=
fields->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fields->hd;
if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1806=( char*)"can't deal with enum tags yet";
struct _tagged_string _temp1807; _temp1807.curr= _temp1806; _temp1807.base=
_temp1806; _temp1807.last_plus_one= _temp1806 + 30; _temp1807;}));} if( f->typs
== 0){ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc == Cyc_Absyn_Extern? 0:(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Absyn_signed_int_exp((
tag_count ++), 0), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( f->name,
t, eopt);( void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({
struct Cyc_Absyn_Tqual* _temp1808=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1808->q_const= 1; _temp1808->q_volatile=
0; _temp1808->q_restrict= 0; _temp1808;}); res=({ struct Cyc_List_List*
_temp1809=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1809->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1810=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1810->tag= Cyc_Absyn_Var_d_tag; _temp1810->f1= vd;( void*) _temp1810;}), 0);
_temp1809->tl= res; _temp1809;});} else{ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc
== Cyc_Absyn_Extern? 0:( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1811=( char*)"_tag"; struct
_tagged_string _temp1812; _temp1812.curr= _temp1811; _temp1812.base= _temp1811;
_temp1812.last_plus_one= _temp1811 + 5; _temp1812;})), Cyc_Absyn_sint_t, eopt);(
void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({ struct
Cyc_Absyn_Tqual* _temp1813=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Tqual)); _temp1813->q_const= 1; _temp1813->q_volatile= 0;
_temp1813->q_restrict= 0; _temp1813;}); res=({ struct Cyc_List_List* _temp1814=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1814->hd=(
void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct* _temp1815=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1815->tag= Cyc_Absyn_Var_d_tag; _temp1815->f1= vd;( void*) _temp1815;}), 0);
_temp1814->tl= res; _temp1814;}); if( ! seen_defn_before){ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl,
i ++){ struct _tagged_string* fname=( struct _tagged_string*)({ struct
_tagged_string* _temp1816=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1816[ 0]= xprintf("f%d", i); _temp1816;}); fs=({
struct Cyc_List_List* _temp1817=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1817->hd=( void*)({ struct _tuple4* _temp1818=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1818->f1= fname;
_temp1818->f2=(*(( struct _tuple6*) ts->hd)).f1; _temp1818->f3= Cyc_Toc_typ_to_c_array((*((
struct _tuple6*) ts->hd)).f2); _temp1818;}); _temp1817->tl= fs; _temp1817;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);
fs=({ struct Cyc_List_List* _temp1819=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1819->hd=( void*)({ struct _tuple4*
_temp1820=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1820->f1=
Cyc_Toc_tag_sp; _temp1820->f2= Cyc_Toc_mt_tq; _temp1820->f3= Cyc_Absyn_sint_t;
_temp1820;}); _temp1819->tl= fs; _temp1819;});{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp1821=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp1821->sc=( void*) Cyc_Absyn_Public;
_temp1821->name=({ struct Cyc_Core_Opt* _temp1823=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1823->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1824=( char*)"_struct"; struct
_tagged_string _temp1825; _temp1825.curr= _temp1824; _temp1825.base= _temp1824;
_temp1825.last_plus_one= _temp1824 + 8; _temp1825;})); _temp1823;}); _temp1821->tvs=
0; _temp1821->fields=({ struct Cyc_Core_Opt* _temp1822=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1822->v=( void*) fs; _temp1822;});
_temp1821;}); res=({ struct Cyc_List_List* _temp1826=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1826->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp1827=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1827->tag= Cyc_Absyn_Struct_d_tag;
_temp1827->f1= sd;( void*) _temp1827;}), 0); _temp1826->tl= res; _temp1826;});}}}}}
return res;}}} static struct Cyc_Core_Opt* Cyc_Toc_xenums_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xenumdecl_to_c( struct Cyc_Absyn_Xenumdecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp1828=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1828->name= n; _temp1828->tvs= 0; _temp1828->defn=( void*) t; _temp1828;});
res=({ struct Cyc_List_List* _temp1829=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1829->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1830=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1830->tag= Cyc_Absyn_Typedef_d_tag;
_temp1830->f1= td;( void*) _temp1830;}), 0); _temp1829->tl= res; _temp1829;});}
else{ if( Cyc_Toc_xenums_so_far == 0){ Cyc_Toc_xenums_so_far=({ struct Cyc_Core_Opt*
_temp1831=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1831->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1831;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->tag
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1832=( char*)"xenum:  can't deal with explicit tags";
struct _tagged_string _temp1833; _temp1833.curr= _temp1832; _temp1833.base=
_temp1832; _temp1833.last_plus_one= _temp1832 + 38; _temp1833;}));}{ struct
_tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp1834=* fn;( unsigned int)( _temp1834.last_plus_one -
_temp1834.curr);}), 0); void* tag_typ=({ struct Cyc_Absyn_ArrayType_struct*
_temp1835=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1835->tag= Cyc_Absyn_ArrayType_tag; _temp1835->f1=( void*) Cyc_Absyn_uchar_t;
_temp1835->f2= Cyc_Toc_mt_tq; _temp1835->f3= sz_exp;( void*) _temp1835;});
struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc != Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp( 0,* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp1836=( char*)"_tag"; struct _tagged_string
_temp1837; _temp1837.curr= _temp1836; _temp1837.base= _temp1836; _temp1837.last_plus_one=
_temp1836 + 5; _temp1837;})), tag_typ, initopt);( void*)( tag_decl->sc=( void*)((
void*) xd->sc)); res=({ struct Cyc_List_List* _temp1838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1838->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1839=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1839->tag= Cyc_Absyn_Var_d_tag;
_temp1839->f1= tag_decl;( void*) _temp1839;}), 0); _temp1838->tl= res; _temp1838;});{
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
_temp1840=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1840[ 0]= xprintf("f%d", i); _temp1840;}); struct _tuple4* newf=({
struct _tuple4* _temp1841=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1841->f1= field_n; _temp1841->f2=(*(( struct _tuple6*) ts->hd)).f1;
_temp1841->f3= Cyc_Toc_typ_to_c_array((*(( struct _tuple6*) ts->hd)).f2);
_temp1841;}); fields=({ struct Cyc_List_List* _temp1842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1842->hd=( void*) newf;
_temp1842->tl= fields; _temp1842;});}} fields=({ struct Cyc_List_List* _temp1843=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1843->hd=(
void*)({ struct _tuple4* _temp1844=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1844->f1= Cyc_Toc_tag_sp; _temp1844->f2= Cyc_Toc_mt_tq;
_temp1844->f3= Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp1844;});
_temp1843->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
fields); _temp1843;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp1845=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp1845->sc=( void*) Cyc_Absyn_Public; _temp1845->name=({ struct Cyc_Core_Opt*
_temp1847=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1847->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp1848=( char*)"_struct"; struct _tagged_string _temp1849; _temp1849.curr=
_temp1848; _temp1849.base= _temp1848; _temp1849.last_plus_one= _temp1848 + 8;
_temp1849;})); _temp1847;}); _temp1845->tvs= 0; _temp1845->fields=({ struct Cyc_Core_Opt*
_temp1846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1846->v=( void*) fields; _temp1846;}); _temp1845;}); res=({ struct Cyc_List_List*
_temp1850=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1850->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp1851=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1851->tag= Cyc_Absyn_Struct_d_tag; _temp1851->f1= strct_decl;( void*)
_temp1851;}), 0); _temp1850->tl= res; _temp1850;});}}}}}}}} return res;} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s){ void* t=( void*) topt->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp1854; struct Cyc_List_List* _temp1856; struct Cyc_Toc_Env* _temp1858;
struct _tuple14 _temp1852= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL1859: _temp1858=
_temp1852.f1; goto _LL1857; _LL1857: _temp1856= _temp1852.f2; goto _LL1855;
_LL1855: _temp1854= _temp1852.f3; goto _LL1853; _LL1853: Cyc_Toc_stmt_to_c(
_temp1858, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1854, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1860=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1860->f1= Cyc_Toc_abs_ns; _temp1860->f2= Cyc_Toc_Match_Exception_sp;
_temp1860;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1854, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp1856 != 0; _temp1856= _temp1856->tl){
struct _tuple15 _temp1863; void* _temp1864; struct _tuple1* _temp1866; struct
_tuple15* _temp1861=( struct _tuple15*) _temp1856->hd; _temp1863=* _temp1861;
_LL1867: _temp1866= _temp1863.f1; goto _LL1865; _LL1865: _temp1864= _temp1863.f2;
goto _LL1862; _LL1862: s= Cyc_Absyn_declare_stmt( _temp1866, _temp1864, 0, s, 0);}
return s;}}} static struct _tuple18 Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top){ struct Cyc_List_List* res= 0; for( 0; ds !=
0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) ds->hd; void*
_temp1868=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp1892; struct Cyc_Absyn_Fndecl*
_temp1894; int _temp1896; struct Cyc_Absyn_Exp* _temp1898; struct Cyc_Core_Opt*
_temp1900; struct Cyc_Core_Opt* _temp1902; struct Cyc_Absyn_Pat* _temp1904;
struct Cyc_Absyn_Structdecl* _temp1906; struct Cyc_Absyn_Enumdecl* _temp1908;
struct Cyc_Absyn_Xenumdecl* _temp1910; struct Cyc_Absyn_Typedefdecl* _temp1912;
struct Cyc_List_List* _temp1914; struct _tagged_string* _temp1916; struct Cyc_List_List*
_temp1918; struct _tuple1* _temp1920; struct Cyc_List_List* _temp1922; _LL1870:
if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*) _temp1868)->tag ==
Cyc_Absyn_Var_d_tag: 0){ _LL1893: _temp1892=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1868)->f1; goto _LL1871;} else{ goto
_LL1872;} _LL1872: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL1895: _temp1894=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1868)->f1; goto _LL1873;} else{ goto _LL1874;}
_LL1874: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*) _temp1868)->tag
== Cyc_Absyn_Let_d_tag: 0){ _LL1905: _temp1904=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp1868)->f1; goto _LL1903; _LL1903: _temp1902=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1868)->f2; goto
_LL1901; _LL1901: _temp1900=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1868)->f3; goto _LL1899; _LL1899: _temp1898=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1868)->f4; goto _LL1897; _LL1897: _temp1896=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1868)->f5; goto _LL1875;} else{ goto
_LL1876;} _LL1876: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL1907: _temp1906=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1868)->f1; goto _LL1877;} else{ goto
_LL1878;} _LL1878: if( _temp1868 == Cyc_Absyn_Union_d){ goto _LL1879;} else{
goto _LL1880;} _LL1880: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1909: _temp1908=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1868)->f1; goto _LL1881;} else{ goto
_LL1882;} _LL1882: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1911: _temp1910=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1868)->f1; goto _LL1883;} else{ goto
_LL1884;} _LL1884: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL1913: _temp1912=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1868)->f1; goto _LL1885;} else{ goto
_LL1886;} _LL1886: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1917: _temp1916=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1868)->f1; goto
_LL1915; _LL1915: _temp1914=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1868)->f2; goto _LL1887;} else{ goto _LL1888;} _LL1888: if(( unsigned int)
_temp1868 > 1u?(( struct _enum_struct*) _temp1868)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1921: _temp1920=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1868)->f1; goto _LL1919; _LL1919: _temp1918=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1868)->f2; goto _LL1889;} else{ goto
_LL1890;} _LL1890: if(( unsigned int) _temp1868 > 1u?(( struct _enum_struct*)
_temp1868)->tag == Cyc_Absyn_ExternC_d_tag: 0){ _LL1923: _temp1922=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1868)->f1; goto _LL1891;} else{ goto
_LL1869;} _LL1871: { struct _tuple1* c_name= _temp1892->name; if(( void*)
_temp1892->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1* _temp1924=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1924->f1= Cyc_Toc_abs_ns;
_temp1924->f2=(* c_name).f2; _temp1924;});} if( _temp1892->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1925=
_temp1892->initializer; if( _temp1925 == 0){ _throw( Null_Exception);} _temp1925;}));}
if( nv->toplevel){ res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls, res); Cyc_Toc_temp_topdecls=
0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp1892->name, Cyc_Absyn_varb_exp(
c_name,({ struct Cyc_Absyn_Global_b_struct* _temp1926=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp1926->tag= Cyc_Absyn_Global_b_tag;
_temp1926->f1= _temp1892;( void*) _temp1926;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp1892->name, Cyc_Absyn_varb_exp( c_name,({ struct Cyc_Absyn_Local_b_struct*
_temp1927=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1927->tag= Cyc_Absyn_Local_b_tag; _temp1927->f1= _temp1892;( void*)
_temp1927;}), 0));} _temp1892->name= c_name;( void*)( _temp1892->sc=( void*) Cyc_Toc_scope_to_c((
void*) _temp1892->sc));( void*)( _temp1892->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1892->type)); res=({ struct Cyc_List_List* _temp1928=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1928->hd=( void*) d; _temp1928->tl=
res; _temp1928;}); goto _LL1869;} _LL1873: { struct _tuple1* c_name= _temp1894->name;
if(( void*) _temp1894->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1*
_temp1929=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1929->f1=
Cyc_Toc_abs_ns; _temp1929->f2=(* c_name).f2; _temp1929;});} nv= Cyc_Toc_add_varmap(
nv, _temp1894->name, Cyc_Absyn_var_exp( c_name, 0)); _temp1894->name= c_name;
Cyc_Toc_fndecl_to_c( nv, _temp1894); res=({ struct Cyc_List_List* _temp1930=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1930->hd=(
void*) d; _temp1930->tl= res; _temp1930;}); goto _LL1869;} _LL1875:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1931=( char*)"letdecl"; struct _tagged_string _temp1932; _temp1932.curr=
_temp1931; _temp1932.base= _temp1931; _temp1932.last_plus_one= _temp1931 + 8;
_temp1932;})); goto _LL1869; _LL1877: Cyc_Toc_structdecl_to_c( _temp1906); res=({
struct Cyc_List_List* _temp1933=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1933->hd=( void*) d; _temp1933->tl= res; _temp1933;});
goto _LL1869; _LL1879:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1934=( char*)"uniondecl"; struct
_tagged_string _temp1935; _temp1935.curr= _temp1934; _temp1935.base= _temp1934;
_temp1935.last_plus_one= _temp1934 + 10; _temp1935;})); goto _LL1869; _LL1881:
res= Cyc_Toc_enumdecl_to_c( _temp1908, res); goto _LL1869; _LL1883: res= Cyc_Toc_xenumdecl_to_c(
_temp1910, res); goto _LL1869; _LL1885: _temp1912->name= _temp1912->name;
_temp1912->tvs= 0;( void*)( _temp1912->defn=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1912->defn)); res=({ struct Cyc_List_List* _temp1936=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1936->hd=( void*) d; _temp1936->tl=
res; _temp1936;}); goto _LL1869; _LL1887: _temp1918= _temp1914; goto _LL1889;
_LL1889: _temp1922= _temp1918; goto _LL1891; _LL1891: { struct _tuple18 p= Cyc_Toc_decls_to_c(
nv, _temp1922, top); nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( p.f2), res); goto _LL1869;} _LL1869:;}
return({ struct _tuple18 _temp1937; _temp1937.f1= nv; _temp1937.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp1937;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_enums_so_far= 0; Cyc_Toc_xenums_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List* _temp1940;
struct Cyc_Toc_Env* _temp1942; struct _tuple18 _temp1938= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL1943: _temp1942= _temp1938.f1; goto _LL1941;
_LL1941: _temp1940= _temp1938.f2; goto _LL1939; _LL1939: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp1940);}}