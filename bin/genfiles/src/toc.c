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
struct _tagged_string){ _temp46, _temp46, _temp46 + 14u}; static struct
_tagged_string* Cyc_Toc__npop_handler_sp=& Cyc_Toc__npop_handler_string; static
char _temp49[ 16u]="Match_Exception"; static struct _tagged_string Cyc_Toc_Match_Exception_string=(
struct _tagged_string){ _temp49, _temp49, _temp49 + 16u}; static struct
_tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static char _temp52[ 15u]="Cyc_new_string"; static struct _tagged_string Cyc_Toc_Cyc_new_string_string=(
struct _tagged_string){ _temp52, _temp52, _temp52 + 15u}; static struct
_tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static int Cyc_Toc_is_nullable( void* t){ if( ! Cyc_Toc_check_for_null){ return
0;}{ void* _temp53= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp59;
struct Cyc_Absyn_Conref* _temp61; struct Cyc_Absyn_Tqual* _temp63; struct Cyc_Absyn_Conref*
_temp65; void* _temp67; void* _temp69; _LL55: if(( unsigned int) _temp53 > 5u?((
struct _enum_struct*) _temp53)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL60:
_temp59=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp53)->f1; _LL70: _temp69=( void*) _temp59.elt_typ; goto _LL68; _LL68:
_temp67=( void*) _temp59.rgn_typ; goto _LL66; _LL66: _temp65=( struct Cyc_Absyn_Conref*)
_temp59.nullable; goto _LL64; _LL64: _temp63=( struct Cyc_Absyn_Tqual*) _temp59.tq;
goto _LL62; _LL62: _temp61=( struct Cyc_Absyn_Conref*) _temp59.bounds; goto
_LL56;} else{ goto _LL57;} _LL57: goto _LL58; _LL56: { void* _temp71=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp65))->v; int _temp77; _LL73: if(( unsigned int) _temp71 > 1u?(( struct
_enum_struct*) _temp71)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL78: _temp77=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp71)->f1; goto _LL74;} else{ goto
_LL75;} _LL75: goto _LL76; _LL74: return _temp77; _LL76: return 0; _LL72:;}
_LL58:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp79=( char*)"is_nullable"; struct _tagged_string
_temp80; _temp80.curr= _temp79; _temp80.base= _temp79; _temp80.last_plus_one=
_temp79 + 12; _temp80;})); return 0; _LL54:;}} static int Cyc_Toc_really_is_nullable(
void* t){ int b= Cyc_Toc_check_for_null; Cyc_Toc_check_for_null= 1;{ int ans=
Cyc_Toc_is_nullable( t); Cyc_Toc_check_for_null= b; return ans;}} static struct
_tuple1* Cyc_Toc_collapse_qvar_tag( struct _tuple1* x, struct _tagged_string tag){
return({ struct _tuple1* _temp81=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp81->f1=(* x).f1; _temp81->f2=({ struct _tagged_string* _temp82=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp82[
0]= Cyc_String_strconcat(*(* x).f2, tag); _temp82;}); _temp81;});} static struct
Cyc_List_List* Cyc_Toc_added_decls= 0; static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts= ts->tl, i ++){ fs=({ struct Cyc_List_List*
_temp83=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp83->hd=( void*)({ struct _tuple4* _temp84=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp84->f1=({ struct _tagged_string* _temp85=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp85[ 0]=
xprintf("f%d", i); _temp85;}); _temp84->f2= Cyc_Toc_mt_tq; _temp84->f3=( void*)
ts->hd; _temp84;}); _temp83->tl= fs; _temp83;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl* _temp86=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp86->sc=( void*) Cyc_Absyn_Public;
_temp86->name=({ struct Cyc_Core_Opt* _temp88=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp88->v=( void*)({ struct _tuple1* _temp89=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp89->f1= Cyc_Toc_abs_ns;
_temp89->f2= x; _temp89;}); _temp88;}); _temp86->tvs= 0; _temp86->fields=({
struct Cyc_Core_Opt* _temp87=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp87->v=( void*) fs; _temp87;}); _temp86;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp90->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp91=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp91->tag= Cyc_Absyn_Struct_d_tag; _temp91->f1= sd;( void*) _temp91;}), 0);
_temp90->tl= Cyc_Toc_added_decls; _temp90;});}} static void Cyc_Toc_add_tagged_ptr_decl(
struct _tagged_string* x, void* t){ void* tptr= Cyc_Absyn_star_typ( t, Cyc_Absyn_HeapRgn,
Cyc_Toc_mt_tq); struct _tuple4* f1=({ struct _tuple4* _temp92=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp92->f1= Cyc_Toc_curr_sp; _temp92->f2=
Cyc_Toc_mt_tq; _temp92->f3= tptr; _temp92;}); struct _tuple4* f2=({ struct
_tuple4* _temp93=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp93->f1=
Cyc_Toc_base_sp; _temp93->f2= Cyc_Toc_mt_tq; _temp93->f3= tptr; _temp93;});
struct _tuple4* f3=({ struct _tuple4* _temp94=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp94->f1= Cyc_Toc_last_plus_one_sp; _temp94->f2=
Cyc_Toc_mt_tq; _temp94->f3= tptr; _temp94;}); struct Cyc_List_List* fs=({ struct
Cyc_List_List* _temp95=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp95->hd=( void*) f1; _temp95->tl=({ struct Cyc_List_List* _temp96=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp96->hd=( void*)
f2; _temp96->tl=({ struct Cyc_List_List* _temp97=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp97->hd=( void*) f3; _temp97->tl=
0; _temp97;}); _temp96;}); _temp95;}); struct Cyc_Absyn_Structdecl* sd=({ struct
Cyc_Absyn_Structdecl* _temp98=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Structdecl)); _temp98->sc=( void*) Cyc_Absyn_Public; _temp98->name=({
struct Cyc_Core_Opt* _temp100=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp100->v=( void*)({ struct _tuple1* _temp101=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp101->f1= Cyc_Toc_abs_ns; _temp101->f2=
x; _temp101;}); _temp100;}); _temp98->tvs= 0; _temp98->fields=({ struct Cyc_Core_Opt*
_temp99=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp99->v=( void*)(( struct Cyc_List_List*) fs); _temp99;}); _temp98;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp102=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp102->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp103=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp103->tag= Cyc_Absyn_Struct_d_tag; _temp103->f1= sd;( void*) _temp103;}), 0);
_temp102->tl= Cyc_Toc_added_decls; _temp102;});} static struct Cyc_List_List*
Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter= 0; static struct
_tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{ struct Cyc_List_List*
tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts= tts->tl){ struct _tuple5
_temp106; struct Cyc_List_List* _temp107; struct _tagged_string* _temp109;
struct _tuple5* _temp104=( struct _tuple5*) tts->hd; _temp106=* _temp104; _LL110:
_temp109= _temp106.f1; goto _LL108; _LL108: _temp107= _temp106.f2; goto _LL105;
_LL105: { int okay= 1; struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0?
_temp107 != 0: 0; tqs= tqs->tl, _temp107= _temp107->tl){ if( ! Cyc_Tcutil_unify((*((
struct _tuple6*) tqs->hd)).f2,( void*) _temp107->hd)){ okay= 0; break;}} if(( !
okay? 1: tqs != 0)? 1: _temp107 != 0){ continue;} return _temp109;}}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp111=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp111[ 0]= xprintf("_tuple%d",(
Cyc_Toc_tuple_type_counter ++)); _temp111;}); struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple6*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp112=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp112->hd=( void*)({ struct
_tuple5* _temp113=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp113->f1= x; _temp113->f2= ts; _temp113;}); _temp112->tl= Cyc_Toc_tuple_types;
_temp112;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual* tq){{ void* _temp114= t; void* _temp120; void*
_temp122; _LL116: if(( unsigned int) _temp114 > 5u?(( struct _enum_struct*)
_temp114)->tag == Cyc_Absyn_IntType_tag: 0){ _LL123: _temp122=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp114)->f1; if( _temp122 == Cyc_Absyn_Unsigned){
goto _LL121;} else{ goto _LL118;} _LL121: _temp120=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp114)->f2; if( _temp120 == Cyc_Absyn_B1){ goto _LL117;} else{ goto _LL118;}}
else{ goto _LL118;} _LL118: goto _LL119; _LL117: return Cyc_Toc__tagged_string_sp;
_LL119: goto _LL115; _LL115:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts= tts->tl){ struct _tuple7 _temp126; void* _temp127; struct
_tagged_string* _temp129; struct _tuple7* _temp124=( struct _tuple7*) tts->hd;
_temp126=* _temp124; _LL130: _temp129= _temp126.f1; goto _LL128; _LL128:
_temp127= _temp126.f2; goto _LL125; _LL125: if( ! Cyc_Tcutil_unify( t, _temp127)){
continue;} return _temp129;}}{ struct _tagged_string* x=({ struct _tagged_string*
_temp131=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp131[ 0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter ++));
_temp131;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp132=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp132->hd=( void*)({ struct _tuple7* _temp133=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp133->f1= x; _temp133->f2=
t; _temp133;}); _temp132->tl= Cyc_Toc_tagged_ptr_types; _temp132;}); return x;}}
static int Cyc_Toc_temp_var_counter= 0; static struct _tuple1* Cyc_Toc_temp_var(){
return({ struct _tuple1* _temp134=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp134->f1= Cyc_Toc_abs_ns; _temp134->f2=({ struct _tagged_string*
_temp135=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp135[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp135;});
_temp134;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp136=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp136[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp136;});} static struct
Cyc_List_List* Cyc_Toc_temp_topdecls= 0; static char _temp139[ 10u]="_LongLong";
static struct _tagged_string Cyc_Toc_long_long_string=( struct _tagged_string){
_temp139, _temp139, _temp139 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp140={ 1u, 0}; static struct _tuple1 Cyc_Toc_longlong_v=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp140,.f2=& Cyc_Toc_long_long_string};
static struct Cyc_Absyn_TypedefType_struct _temp141={ 11u,& Cyc_Toc_longlong_v,
0, 0}; static void* Cyc_Toc_longlong_typ=( struct Cyc_Absyn_TypedefType_struct*)&
_temp141; static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple2* Cyc_Toc_arg_to_c(
struct _tuple2* a){ void* _temp144; struct Cyc_Absyn_Tqual* _temp146; struct Cyc_Core_Opt*
_temp148; struct _tuple2 _temp142=* a; _LL149: _temp148= _temp142.f1; goto
_LL147; _LL147: _temp146= _temp142.f2; goto _LL145; _LL145: _temp144= _temp142.f3;
goto _LL143; _LL143: return({ struct _tuple2* _temp150=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp150->f1= _temp148; _temp150->f2=
_temp146; _temp150->f3= Cyc_Toc_typ_to_c( _temp144); _temp150;});} static struct
_tuple6* Cyc_Toc_typ_to_c_f( struct _tuple6* x){ void* _temp153; struct Cyc_Absyn_Tqual*
_temp155; struct _tuple6 _temp151=* x; _LL156: _temp155= _temp151.f1; goto
_LL154; _LL154: _temp153= _temp151.f2; goto _LL152; _LL152: return({ struct
_tuple6* _temp157=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp157->f1= _temp155; _temp157->f2= Cyc_Toc_typ_to_c( _temp153); _temp157;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp158= t; struct Cyc_Absyn_Exp*
_temp166; struct Cyc_Absyn_Tqual* _temp168; void* _temp170; int _temp172; struct
Cyc_Core_Opt* _temp174; struct Cyc_Core_Opt _temp176; void* _temp177; void*
_temp179; _LL160: if(( unsigned int) _temp158 > 5u?(( struct _enum_struct*)
_temp158)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL171: _temp170=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp158)->f1; goto _LL169; _LL169: _temp168=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp158)->f2;
goto _LL167; _LL167: _temp166=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp158)->f3; goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int)
_temp158 > 5u?(( struct _enum_struct*) _temp158)->tag == Cyc_Absyn_Evar_tag: 0){
_LL180: _temp179=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp158)->f1; goto
_LL175; _LL175: _temp174=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp158)->f2; if( _temp174 == 0){ goto _LL164;} else{ _temp176=* _temp174;
_LL178: _temp177=( void*) _temp176.v; goto _LL173;} _LL173: _temp172=( int)((
struct Cyc_Absyn_Evar_struct*) _temp158)->f3; goto _LL163;} else{ goto _LL164;}
_LL164: goto _LL165; _LL161: return({ struct Cyc_Absyn_ArrayType_struct*
_temp181=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp181->tag= Cyc_Absyn_ArrayType_tag; _temp181->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp170); _temp181->f2= _temp168; _temp181->f3= _temp166;( void*) _temp181;});
_LL163: return Cyc_Toc_typ_to_c_array( _temp177); _LL165: return Cyc_Toc_typ_to_c(
t); _LL159:;} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp182= t; int
_temp224; struct Cyc_Core_Opt* _temp226; void* _temp228; int _temp230; struct
Cyc_Core_Opt* _temp232; struct Cyc_Core_Opt _temp234; void* _temp235; void*
_temp237; struct Cyc_Absyn_Tvar* _temp239; struct Cyc_Absyn_Enumdecl** _temp241;
struct Cyc_List_List* _temp243; struct _tuple1* _temp245; struct Cyc_Absyn_Xenumdecl**
_temp247; struct _tuple1* _temp249; struct Cyc_Absyn_PtrInfo _temp251; struct
Cyc_Absyn_Conref* _temp253; struct Cyc_Absyn_Tqual* _temp255; struct Cyc_Absyn_Conref*
_temp257; void* _temp259; void* _temp261; void* _temp263; void* _temp265; struct
Cyc_Absyn_Exp* _temp267; struct Cyc_Absyn_Tqual* _temp269; void* _temp271;
struct Cyc_Absyn_FnInfo _temp273; int _temp275; struct Cyc_List_List* _temp277;
void* _temp279; struct Cyc_Core_Opt* _temp281; struct Cyc_List_List* _temp283;
struct Cyc_List_List* _temp285; struct Cyc_Absyn_Structdecl** _temp287; struct
Cyc_List_List* _temp289; struct _tuple1* _temp291; struct Cyc_Core_Opt* _temp293;
struct Cyc_List_List* _temp295; struct _tuple1* _temp297; void* _temp299; void*
_temp301; struct Cyc_List_List* _temp303; _LL184: if( _temp182 == Cyc_Absyn_VoidType){
goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int) _temp182 > 5u?((
struct _enum_struct*) _temp182)->tag == Cyc_Absyn_Evar_tag: 0){ _LL229: _temp228=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp182)->f1; goto _LL227; _LL227:
_temp226=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp182)->f2;
if( _temp226 == 0){ goto _LL225;} else{ goto _LL188;} _LL225: _temp224=( int)((
struct Cyc_Absyn_Evar_struct*) _temp182)->f3; goto _LL187;} else{ goto _LL188;}
_LL188: if(( unsigned int) _temp182 > 5u?(( struct _enum_struct*) _temp182)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL238: _temp237=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp182)->f1; goto _LL233; _LL233: _temp232=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp182)->f2; if( _temp232 == 0){ goto _LL190;} else{
_temp234=* _temp232; _LL236: _temp235=( void*) _temp234.v; goto _LL231;} _LL231:
_temp230=( int)(( struct Cyc_Absyn_Evar_struct*) _temp182)->f3; goto _LL189;}
else{ goto _LL190;} _LL190: if(( unsigned int) _temp182 > 5u?(( struct
_enum_struct*) _temp182)->tag == Cyc_Absyn_VarType_tag: 0){ _LL240: _temp239=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp182)->f1; goto
_LL191;} else{ goto _LL192;} _LL192: if(( unsigned int) _temp182 > 5u?(( struct
_enum_struct*) _temp182)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL246: _temp245=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp182)->f1; goto _LL244;
_LL244: _temp243=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp182)->f2; goto _LL242; _LL242: _temp241=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp182)->f3; goto _LL193;} else{ goto
_LL194;} _LL194: if(( unsigned int) _temp182 > 5u?(( struct _enum_struct*)
_temp182)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL250: _temp249=( struct _tuple1*)((
struct Cyc_Absyn_XenumType_struct*) _temp182)->f1; goto _LL248; _LL248: _temp247=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp182)->f2;
goto _LL195;} else{ goto _LL196;} _LL196: if(( unsigned int) _temp182 > 5u?((
struct _enum_struct*) _temp182)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL252:
_temp251=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp182)->f1; _LL262: _temp261=( void*) _temp251.elt_typ; goto _LL260; _LL260:
_temp259=( void*) _temp251.rgn_typ; goto _LL258; _LL258: _temp257=( struct Cyc_Absyn_Conref*)
_temp251.nullable; goto _LL256; _LL256: _temp255=( struct Cyc_Absyn_Tqual*)
_temp251.tq; goto _LL254; _LL254: _temp253=( struct Cyc_Absyn_Conref*) _temp251.bounds;
goto _LL197;} else{ goto _LL198;} _LL198: if(( unsigned int) _temp182 > 5u?((
struct _enum_struct*) _temp182)->tag == Cyc_Absyn_IntType_tag: 0){ _LL266:
_temp265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp182)->f1; goto _LL264;
_LL264: _temp263=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp182)->f2;
goto _LL199;} else{ goto _LL200;} _LL200: if( _temp182 == Cyc_Absyn_FloatType){
goto _LL201;} else{ goto _LL202;} _LL202: if( _temp182 == Cyc_Absyn_DoubleType){
goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int) _temp182 > 5u?((
struct _enum_struct*) _temp182)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL272:
_temp271=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp182)->f1; goto
_LL270; _LL270: _temp269=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp182)->f2; goto _LL268; _LL268: _temp267=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp182)->f3; goto _LL205;} else{ goto _LL206;}
_LL206: if(( unsigned int) _temp182 > 5u?(( struct _enum_struct*) _temp182)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL274: _temp273=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp182)->f1; _LL284: _temp283=( struct Cyc_List_List*)
_temp273.tvars; goto _LL282; _LL282: _temp281=( struct Cyc_Core_Opt*) _temp273.effect;
goto _LL280; _LL280: _temp279=( void*) _temp273.ret_typ; goto _LL278; _LL278:
_temp277=( struct Cyc_List_List*) _temp273.args; goto _LL276; _LL276: _temp275=(
int) _temp273.varargs; goto _LL207;} else{ goto _LL208;} _LL208: if((
unsigned int) _temp182 > 5u?(( struct _enum_struct*) _temp182)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL286: _temp285=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp182)->f1; goto _LL209;} else{ goto _LL210;} _LL210: if(( unsigned int)
_temp182 > 5u?(( struct _enum_struct*) _temp182)->tag == Cyc_Absyn_StructType_tag:
0){ _LL292: _temp291=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp182)->f1; goto _LL290; _LL290: _temp289=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp182)->f2; goto _LL288; _LL288: _temp287=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp182)->f3;
goto _LL211;} else{ goto _LL212;} _LL212: if(( unsigned int) _temp182 > 5u?((
struct _enum_struct*) _temp182)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL298:
_temp297=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp182)->f1;
goto _LL296; _LL296: _temp295=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp182)->f2; goto _LL294; _LL294: _temp293=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp182)->f3; goto _LL213;} else{ goto _LL214;}
_LL214: if(( unsigned int) _temp182 > 5u?(( struct _enum_struct*) _temp182)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL300: _temp299=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp182)->f1; goto _LL215;} else{ goto _LL216;} _LL216: if( _temp182 == Cyc_Absyn_HeapRgn){
goto _LL217;} else{ goto _LL218;} _LL218: if( _temp182 == Cyc_Absyn_UnionType){
goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp182 > 5u?((
struct _enum_struct*) _temp182)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL302:
_temp301=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp182)->f1; goto
_LL221;} else{ goto _LL222;} _LL222: if(( unsigned int) _temp182 > 5u?(( struct
_enum_struct*) _temp182)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL304: _temp303=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp182)->f1; goto
_LL223;} else{ goto _LL183;} _LL185: return t; _LL187: return Cyc_Absyn_VoidType;
_LL189: return Cyc_Toc_typ_to_c( _temp235); _LL191: return Cyc_Absyn_void_star_typ();
_LL193: if( _temp245 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp305=( char*)"anonymous enum"; struct
_tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 15; _temp306;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp307=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp307->tag= Cyc_Absyn_TypedefType_tag; _temp307->f1=( struct _tuple1*)({
struct _tuple1* _temp308= _temp245; if( _temp308 == 0){ _throw( Null_Exception);}
_temp308;}); _temp307->f2= 0; _temp307->f3=({ struct Cyc_Core_Opt* _temp309=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp309->v=(
void*) Cyc_Absyn_void_star_typ(); _temp309;});( void*) _temp307;}); _LL195:
return({ struct Cyc_Absyn_TypedefType_struct* _temp310=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp310->tag= Cyc_Absyn_TypedefType_tag;
_temp310->f1= _temp249; _temp310->f2= 0; _temp310->f3=({ struct Cyc_Core_Opt*
_temp311=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp311->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp),
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp311;});( void*) _temp310;}); _LL197:
_temp261= Cyc_Toc_typ_to_c_array( _temp261);{ void* _temp312=( void*) _temp253->v;
void* _temp318; _LL314: if(( unsigned int) _temp312 > 1u?(( struct _enum_struct*)
_temp312)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL319: _temp318=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp312)->f1; if( _temp318 == Cyc_Absyn_Unknown_b){
goto _LL315;} else{ goto _LL316;}} else{ goto _LL316;} _LL316: goto _LL317;
_LL315: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp261,
_temp255); return Cyc_Absyn_strct( n);} _LL317: return Cyc_Absyn_star_typ(
_temp261, Cyc_Absyn_HeapRgn, _temp255); _LL313:;} _LL199: return t; _LL201:
return t; _LL203: return t; _LL205: return({ struct Cyc_Absyn_ArrayType_struct*
_temp320=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp320->tag= Cyc_Absyn_ArrayType_tag; _temp320->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp271); _temp320->f2= _temp269; _temp320->f3= _temp267;( void*) _temp320;});
_LL207: return({ struct Cyc_Absyn_FnType_struct* _temp321=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp321->tag= Cyc_Absyn_FnType_tag;
_temp321->f1=({ struct Cyc_Absyn_FnInfo _temp322; _temp322.tvars= 0; _temp322.effect=
0; _temp322.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp279); _temp322.args=((
struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp277); _temp322.varargs= _temp275;
_temp322;});( void*) _temp321;}); _LL209: _temp285=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_typ_to_c_f, _temp285);{ struct _tagged_string* n= Cyc_Toc_add_tuple_type(
_temp285); return Cyc_Absyn_strct( n);} _LL211: if( _temp291 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp323=( char*)"anonymous struct"; struct _tagged_string _temp324; _temp324.curr=
_temp323; _temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 17;
_temp324;}));} return Cyc_Absyn_strctq(( struct _tuple1*)({ struct _tuple1*
_temp325= _temp291; if( _temp325 == 0){ _throw( Null_Exception);} _temp325;}));
_LL213: if( _temp293 == 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp326=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp327; _temp327.curr= _temp326; _temp327.base= _temp326;
_temp327.last_plus_one= _temp326 + 38; _temp327;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp328=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp328->tag= Cyc_Absyn_TypedefType_tag; _temp328->f1= _temp297; _temp328->f2=
0; _temp328->f3=({ struct Cyc_Core_Opt* _temp329=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp329->v=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp293->v); _temp329;});( void*) _temp328;}); _LL215: return Cyc_Absyn_void_star_typ();
_LL217: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp330=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 55; _temp331;})); _LL219: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp332=( char*)"uniontype"; struct _tagged_string _temp333; _temp333.curr=
_temp332; _temp333.base= _temp332; _temp333.last_plus_one= _temp332 + 10;
_temp333;})); _LL221: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp334=( char*)"Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 54; _temp335;})); _LL223: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp336=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 52; _temp337;})); _LL183:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp338= t; struct Cyc_Absyn_Exp* _temp344; struct Cyc_Absyn_Tqual*
_temp346; void* _temp348; _LL340: if(( unsigned int) _temp338 > 5u?(( struct
_enum_struct*) _temp338)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL349: _temp348=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp338)->f1; goto _LL347; _LL347:
_temp346=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp338)->f2; goto _LL345; _LL345: _temp344=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp338)->f3; goto _LL341;} else{ goto _LL342;}
_LL342: goto _LL343; _LL341: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp348, Cyc_Absyn_HeapRgn, _temp346), e, l); _LL343: return e; _LL339:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp350= t; struct Cyc_Absyn_Exp*
_temp356; struct Cyc_Absyn_Tqual* _temp358; void* _temp360; _LL352: if((
unsigned int) _temp350 > 5u?(( struct _enum_struct*) _temp350)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp350)->f1;
goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp350)->f2; goto _LL357; _LL357: _temp356=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp350)->f3; goto _LL353;} else{ goto _LL354;}
_LL354: goto _LL355; _LL353: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp360, Cyc_Absyn_HeapRgn, _temp358), e, l); _LL355: return Cyc_Absyn_cast_exp(
t, e, l); _LL351:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp362= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp394; void* _temp396; void* _temp398; struct Cyc_Absyn_Exp*
_temp400; struct Cyc_Absyn_Tqual* _temp402; void* _temp404; struct Cyc_Absyn_FnInfo
_temp406; struct Cyc_List_List* _temp408; struct Cyc_Absyn_Structdecl** _temp410;
struct Cyc_List_List* _temp412; struct _tuple1* _temp414; struct Cyc_Absyn_Enumdecl**
_temp416; struct Cyc_List_List* _temp418; struct _tuple1* _temp420; struct Cyc_Absyn_Xenumdecl**
_temp422; struct _tuple1* _temp424; struct Cyc_Absyn_PtrInfo _temp426; void*
_temp428; _LL364: if( _temp362 == Cyc_Absyn_VoidType){ goto _LL365;} else{ goto
_LL366;} _LL366: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*)
_temp362)->tag == Cyc_Absyn_VarType_tag: 0){ _LL395: _temp394=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp362)->f1; goto _LL367;} else{ goto _LL368;}
_LL368: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*) _temp362)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL399: _temp398=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp362)->f1; goto _LL397; _LL397: _temp396=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp362)->f2; goto _LL369;} else{ goto _LL370;} _LL370: if( _temp362 == Cyc_Absyn_FloatType){
goto _LL371;} else{ goto _LL372;} _LL372: if( _temp362 == Cyc_Absyn_DoubleType){
goto _LL373;} else{ goto _LL374;} _LL374: if(( unsigned int) _temp362 > 5u?((
struct _enum_struct*) _temp362)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL405:
_temp404=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp362)->f1; goto
_LL403; _LL403: _temp402=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp362)->f2; goto _LL401; _LL401: _temp400=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp362)->f3; goto _LL375;} else{ goto _LL376;}
_LL376: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*) _temp362)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL407: _temp406=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp362)->f1; goto _LL377;} else{ goto _LL378;}
_LL378: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*) _temp362)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL409: _temp408=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp362)->f1; goto _LL379;} else{ goto
_LL380;} _LL380: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*)
_temp362)->tag == Cyc_Absyn_StructType_tag: 0){ _LL415: _temp414=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp362)->f1; goto _LL413;
_LL413: _temp412=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp362)->f2; goto _LL411; _LL411: _temp410=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp362)->f3; goto _LL381;} else{ goto
_LL382;} _LL382: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*)
_temp362)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL421: _temp420=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp362)->f1; goto _LL419; _LL419: _temp418=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp362)->f2; goto
_LL417; _LL417: _temp416=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp362)->f3; goto _LL383;} else{ goto _LL384;} _LL384: if(( unsigned int)
_temp362 > 5u?(( struct _enum_struct*) _temp362)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL425: _temp424=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp362)->f1; goto _LL423; _LL423: _temp422=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp362)->f2; goto _LL385;} else{ goto
_LL386;} _LL386: if( _temp362 == Cyc_Absyn_UnionType){ goto _LL387;} else{ goto
_LL388;} _LL388: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*)
_temp362)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL427: _temp426=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp362)->f1; goto _LL389;} else{ goto
_LL390;} _LL390: if(( unsigned int) _temp362 > 5u?(( struct _enum_struct*)
_temp362)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL429: _temp428=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp362)->f1; goto _LL391;} else{ goto
_LL392;} _LL392: goto _LL393; _LL365: return 1; _LL367: return 0; _LL369: return
1; _LL371: return 1; _LL373: return 0; _LL375: return Cyc_Toc_atomic_typ(
_temp404); _LL377: return 1; _LL379: for( 0; _temp408 != 0; _temp408= _temp408->tl){
if( ! Cyc_Toc_atomic_typ((*(( struct _tuple6*) _temp408->hd)).f2)){ return 0;}}
return 1; _LL381: return 0; _LL383: return 0; _LL385: return 0; _LL387: return 0;
_LL389: return 0; _LL391: return 0; _LL393:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp430= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp430.last_plus_one - _temp430.curr,
_temp430.curr);})); return 0; _LL363:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp431= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp437;
struct Cyc_Absyn_Conref* _temp439; struct Cyc_Absyn_Tqual* _temp441; struct Cyc_Absyn_Conref*
_temp443; void* _temp445; void* _temp447; _LL433: if(( unsigned int) _temp431 >
5u?(( struct _enum_struct*) _temp431)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL438: _temp437=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp431)->f1; _LL448: _temp447=( void*) _temp437.elt_typ; goto _LL446; _LL446:
_temp445=( void*) _temp437.rgn_typ; goto _LL444; _LL444: _temp443=( struct Cyc_Absyn_Conref*)
_temp437.nullable; goto _LL442; _LL442: _temp441=( struct Cyc_Absyn_Tqual*)
_temp437.tq; goto _LL440; _LL440: _temp439=( struct Cyc_Absyn_Conref*) _temp437.bounds;
goto _LL434;} else{ goto _LL435;} _LL435: goto _LL436; _LL434: { void* _temp449=
Cyc_Tcutil_compress( _temp447); _LL451: if( _temp449 == Cyc_Absyn_VoidType){
goto _LL452;} else{ goto _LL453;} _LL453: goto _LL454; _LL452: return 1; _LL454:
return 0; _LL450:;} _LL436: return 0; _LL432:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp455= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp463; struct Cyc_Absyn_Structdecl* _temp465;
struct Cyc_List_List* _temp466; struct _tuple1* _temp468; struct Cyc_Absyn_Structdecl**
_temp470; struct Cyc_List_List* _temp472; struct _tuple1* _temp474; _LL457: if((
unsigned int) _temp455 > 5u?(( struct _enum_struct*) _temp455)->tag == Cyc_Absyn_StructType_tag:
0){ _LL469: _temp468=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp455)->f1; goto _LL467; _LL467: _temp466=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp455)->f2; goto _LL464; _LL464: _temp463=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp455)->f3;
if( _temp463 == 0){ goto _LL459;} else{ _temp465=* _temp463; goto _LL458;}}
else{ goto _LL459;} _LL459: if(( unsigned int) _temp455 > 5u?(( struct
_enum_struct*) _temp455)->tag == Cyc_Absyn_StructType_tag: 0){ _LL475: _temp474=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp455)->f1; goto
_LL473; _LL473: _temp472=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp455)->f2; goto _LL471; _LL471: _temp470=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp455)->f3; if( _temp470 == 0){ goto
_LL460;} else{ goto _LL461;}} else{ goto _LL461;} _LL461: goto _LL462; _LL458:
if( _temp468 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp476=( char*)"StructType name not properly set";
struct _tagged_string _temp477; _temp477.curr= _temp476; _temp477.base= _temp476;
_temp477.last_plus_one= _temp476 + 33; _temp477;}));} if( _temp465->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp479= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp478=
_temp468; if( _temp478 == 0){ _throw( Null_Exception);} _temp478;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp479.last_plus_one - _temp479.curr, _temp479.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp465->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(*(( struct _tuple4*) fields->hd)).f1)
== 0){ return Cyc_Toc_is_void_star((*(( struct _tuple4*) fields->hd)).f3);}}((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp481= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp480=
_temp468; if( _temp480 == 0){ _throw( Null_Exception);} _temp480;})); struct
_tagged_string _temp482=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp481.last_plus_one - _temp481.curr, _temp481.curr, _temp482.last_plus_one -
_temp482.curr, _temp482.curr);})); return 0;} _LL460: if( _temp474 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp483=( char*)"StructType name not properly set"; struct _tagged_string
_temp484; _temp484.curr= _temp483; _temp484.base= _temp483; _temp484.last_plus_one=
_temp483 + 33; _temp484;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp486= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp485= _temp474; if( _temp485 == 0){ _throw( Null_Exception);}
_temp485;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp486.last_plus_one
- _temp486.curr, _temp486.curr);})); return 0; _LL462:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp487= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp487.last_plus_one - _temp487.curr,
_temp487.curr);})); return 0; _LL456:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp488=( void*) e->r; struct _tagged_string*
_temp496; struct Cyc_Absyn_Exp* _temp498; struct _tagged_string* _temp500;
struct Cyc_Absyn_Exp* _temp502; _LL490: if((( struct _enum_struct*) _temp488)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL499: _temp498=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp488)->f1; goto _LL497; _LL497:
_temp496=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp488)->f2; goto _LL491;} else{ goto _LL492;} _LL492: if((( struct
_enum_struct*) _temp488)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL503: _temp502=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp488)->f1;
goto _LL501; _LL501: _temp500=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp488)->f2; goto _LL493;} else{ goto _LL494;} _LL494: goto _LL495; _LL491:
return Cyc_Toc_is_poly_field(( void*)( _temp498->topt)->v, _temp496); _LL493: {
void* _temp504= Cyc_Tcutil_compress(( void*)( _temp502->topt)->v); struct Cyc_Absyn_PtrInfo
_temp510; struct Cyc_Absyn_Conref* _temp512; struct Cyc_Absyn_Tqual* _temp514;
struct Cyc_Absyn_Conref* _temp516; void* _temp518; void* _temp520; _LL506: if((
unsigned int) _temp504 > 5u?(( struct _enum_struct*) _temp504)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL511: _temp510=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp504)->f1; _LL521: _temp520=( void*) _temp510.elt_typ; goto _LL519; _LL519:
_temp518=( void*) _temp510.rgn_typ; goto _LL517; _LL517: _temp516=( struct Cyc_Absyn_Conref*)
_temp510.nullable; goto _LL515; _LL515: _temp514=( struct Cyc_Absyn_Tqual*)
_temp510.tq; goto _LL513; _LL513: _temp512=( struct Cyc_Absyn_Conref*) _temp510.bounds;
goto _LL507;} else{ goto _LL508;} _LL508: goto _LL509; _LL507: return Cyc_Toc_is_poly_field(
_temp520, _temp500); _LL509:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp522= Cyc_Absynpp_typ2string(( void*)( _temp502->topt)->v);
xprintf("is_poly_project:  bad type %.*s", _temp522.last_plus_one - _temp522.curr,
_temp522.curr);})); return 0; _LL505:;} _LL495: return 0; _LL489:;} static char
_temp525[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp525, _temp525, _temp525 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp526={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp526,.f2=& Cyc_Toc_gc_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var, 0),({
struct Cyc_List_List* _temp527=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp527->hd=( void*) s; _temp527->tl= 0; _temp527;}), 0);}
static char _temp530[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp530, _temp530, _temp530 + 17u}; static struct Cyc_Absyn_Abs_n_struct
_temp531={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp531,.f2=& Cyc_Toc_gc_malloc_atomic_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var, 0),({
struct Cyc_List_List* _temp532=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp532->hd=( void*) s; _temp532->tl= 0; _temp532;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} else{ return
Cyc_Toc_malloc_ptr( s);}} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag=
0; struct Cyc_Toc_NullCheck_struct{ int tag; void* f1;}; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3;}; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2;}; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp534=({ struct _tuple0
_temp533; _temp533.f1= t1; _temp533.f2= t2; _temp533;}); void* _temp540; struct
Cyc_Absyn_PtrInfo _temp542; void* _temp544; struct Cyc_Absyn_PtrInfo _temp546;
_LL536: _LL545: _temp544= _temp534.f1; if(( unsigned int) _temp544 > 5u?((
struct _enum_struct*) _temp544)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL547:
_temp546=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp544)->f1; goto _LL541;} else{ goto _LL538;} _LL541: _temp540= _temp534.f2;
if(( unsigned int) _temp540 > 5u?(( struct _enum_struct*) _temp540)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL543: _temp542=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp540)->f1; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539; _LL537: {
int n1=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp546.nullable);
int n2=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp542.nullable);
void* b1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp546.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp542.bounds); struct Cyc_List_List* convs= 0;{ struct _tuple0 _temp549=({
struct _tuple0 _temp548; _temp548.f1= b1; _temp548.f2= b2; _temp548;}); void*
_temp559; struct Cyc_Absyn_Exp* _temp561; void* _temp563; struct Cyc_Absyn_Exp*
_temp565; void* _temp567; void* _temp569; void* _temp571; void* _temp573; struct
Cyc_Absyn_Exp* _temp575; void* _temp577; struct Cyc_Absyn_Exp* _temp579; void*
_temp581; _LL551: _LL564: _temp563= _temp549.f1; if(( unsigned int) _temp563 > 1u?((
struct _enum_struct*) _temp563)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL566:
_temp565=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp563)->f1;
goto _LL560;} else{ goto _LL553;} _LL560: _temp559= _temp549.f2; if((
unsigned int) _temp559 > 1u?(( struct _enum_struct*) _temp559)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL562: _temp561=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp559)->f1; goto _LL552;} else{ goto _LL553;} _LL553: _LL570: _temp569=
_temp549.f1; if( _temp569 == Cyc_Absyn_Unknown_b){ goto _LL568;} else{ goto
_LL555;} _LL568: _temp567= _temp549.f2; if( _temp567 == Cyc_Absyn_Unknown_b){
goto _LL554;} else{ goto _LL555;} _LL555: _LL574: _temp573= _temp549.f1; if((
unsigned int) _temp573 > 1u?(( struct _enum_struct*) _temp573)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL576: _temp575=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp573)->f1; goto _LL572;} else{ goto _LL557;} _LL572: _temp571= _temp549.f2;
if( _temp571 == Cyc_Absyn_Unknown_b){ goto _LL556;} else{ goto _LL557;} _LL557:
_LL582: _temp581= _temp549.f1; if( _temp581 == Cyc_Absyn_Unknown_b){ goto _LL578;}
else{ goto _LL550;} _LL578: _temp577= _temp549.f2; if(( unsigned int) _temp577 >
1u?(( struct _enum_struct*) _temp577)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL580:
_temp579=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp577)->f1;
goto _LL558;} else{ goto _LL550;} _LL552: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List*
_temp583=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp583->hd=( void*)({ struct Cyc_Toc_NullCheck_struct* _temp584=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp584->tag= Cyc_Toc_NullCheck_tag;
_temp584->f1=( void*) t2;( void*) _temp584;}); _temp583->tl= convs; _temp583;});}
goto _LL550; _LL554: goto _LL550; _LL556: convs=({ struct Cyc_List_List*
_temp585=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp585->hd=( void*)({ struct Cyc_Toc_TagPtr_struct* _temp586=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp586->tag= Cyc_Toc_TagPtr_tag;
_temp586->f1=( void*) Cyc_Toc_typ_to_c( t1); _temp586->f2= _temp575;( void*)
_temp586;}); _temp585->tl= convs; _temp585;}); goto _LL550; _LL558: convs=({
struct Cyc_List_List* _temp587=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp587->hd=( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp588=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp588->tag= Cyc_Toc_UntagPtr_tag; _temp588->f1=( void*) Cyc_Toc_typ_to_c( t1);
_temp588->f2= _temp579; _temp588->f3= n2;( void*) _temp588;}); _temp587->tl=
convs; _temp587;}); goto _LL550; _LL550:;} return convs;} _LL539: return 0;
_LL535:;}} static char _temp591[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp591, _temp591, _temp591 + 7u}; static struct Cyc_Absyn_Abs_n_struct
_temp592={ 1u, 0}; static struct _tuple1 Cyc_Toc__throw_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp592,.f2=& Cyc_Toc__throw_string}; static
struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({ struct Cyc_List_List* _temp593=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp593->hd=(
void*) e; _temp593->tl= 0; _temp593;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c(
t); if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp594=( char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp595; _temp595.curr= _temp594; _temp595.base= _temp594;
_temp595.last_plus_one= _temp594 + 44; _temp595;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp596= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp602; struct Cyc_Absyn_Conref* _temp604; struct Cyc_Absyn_Tqual*
_temp606; struct Cyc_Absyn_Conref* _temp608; void* _temp610; void* _temp612;
_LL598: if(( unsigned int) _temp596 > 5u?(( struct _enum_struct*) _temp596)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL603: _temp602=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp596)->f1; _LL613: _temp612=( void*)
_temp602.elt_typ; goto _LL611; _LL611: _temp610=( void*) _temp602.rgn_typ; goto
_LL609; _LL609: _temp608=( struct Cyc_Absyn_Conref*) _temp602.nullable; goto
_LL607; _LL607: _temp606=( struct Cyc_Absyn_Tqual*) _temp602.tq; goto _LL605;
_LL605: _temp604=( struct Cyc_Absyn_Conref*) _temp602.bounds; goto _LL599;}
else{ goto _LL600;} _LL600: goto _LL601; _LL599: elt_typ= _temp612;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp612, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL597;} _LL601: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp614=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp615;
_temp615.curr= _temp614; _temp615.base= _temp614; _temp615.last_plus_one=
_temp614 + 28; _temp615;})); _LL597:;} if( toplevel){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp616=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp616->tag= Cyc_Absyn_ArrayType_tag;
_temp616->f1=( void*) elt_typ; _temp616->f2= Cyc_Toc_mt_tq; _temp616->f3= sz;(
void*) _temp616;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp617=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp617->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp618=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp618->tag= Cyc_Absyn_Var_d_tag; _temp618->f1= vd;( void*) _temp618;}), 0);
_temp617->tl= Cyc_Toc_temp_topdecls; _temp617;});{ struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*)({ struct _tuple8* _temp624=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp624->f1= 0; _temp624->f2= xexp; _temp624;});
_temp619->tl=({ struct Cyc_List_List* _temp620=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp620->hd=( void*)({ struct
_tuple8* _temp623=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp623->f1= 0; _temp623->f2= xexp; _temp623;}); _temp620->tl=({ struct Cyc_List_List*
_temp621=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp621->hd=( void*)({ struct _tuple8* _temp622=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp622->f1= 0; _temp622->f2= xplussz; _temp622;});
_temp621->tl= 0; _temp621;}); _temp620;}); _temp619;}), 0); return urm_exp;}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp625=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp626; _temp626.curr= _temp625; _temp626.base= _temp625; _temp626.last_plus_one=
_temp625 + 45; _temp626;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
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
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs= cs->tl){ void* _temp627=(
void*) cs->hd; void* _temp635; struct Cyc_Absyn_Exp* _temp637; void* _temp639;
int _temp641; struct Cyc_Absyn_Exp* _temp643; void* _temp645; _LL629: if(((
struct _enum_struct*) _temp627)->tag == Cyc_Toc_NullCheck_tag){ _LL636: _temp635=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp627)->f1; goto _LL630;} else{
goto _LL631;} _LL631: if((( struct _enum_struct*) _temp627)->tag == Cyc_Toc_TagPtr_tag){
_LL640: _temp639=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp627)->f1; goto
_LL638; _LL638: _temp637=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp627)->f2; goto _LL632;} else{ goto _LL633;} _LL633: if((( struct
_enum_struct*) _temp627)->tag == Cyc_Toc_UntagPtr_tag){ _LL646: _temp645=( void*)((
struct Cyc_Toc_UntagPtr_struct*) _temp627)->f1; goto _LL644; _LL644: _temp643=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*) _temp627)->f2; goto
_LL642; _LL642: _temp641=( int)(( struct Cyc_Toc_UntagPtr_struct*) _temp627)->f3;
goto _LL634;} else{ goto _LL628;} _LL630: e= Cyc_Toc_null_check_conv( toplevel,
_temp635, e); goto _LL628; _LL632: e= Cyc_Toc_tag_ptr_conv( toplevel, _temp639,
_temp637, e); goto _LL628; _LL634: e= Cyc_Toc_untag_ptr_conv( toplevel, _temp645,
_temp643, _temp641, e); goto _LL628; _LL628:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int to_pop_break; int
to_pop_continue; int to_pop_return; int to_pop_fallthru; int toplevel;}; typedef
struct Cyc_Toc_Env* Cyc_Toc_env; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp647=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp647->break_lab= 0; _temp647->continue_lab= 0;
_temp647->fallthru_info= 0; _temp647->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp647->to_pop_break= 0; _temp647->to_pop_continue= 0; _temp647->to_pop_return=
0; _temp647->to_pop_fallthru= 0; _temp647->toplevel= 1; _temp647;});} static
struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){ return({ struct
Cyc_Toc_Env* _temp648=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct Cyc_Toc_Env));
_temp648->break_lab= e->break_lab; _temp648->continue_lab= e->continue_lab;
_temp648->fallthru_info= e->fallthru_info; _temp648->varmap= e->varmap; _temp648->to_pop_break=
e->to_pop_break; _temp648->to_pop_continue= e->to_pop_continue; _temp648->to_pop_return=
e->to_pop_return; _temp648->to_pop_fallthru= e->to_pop_fallthru; _temp648->toplevel=
e->toplevel; _temp648;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp649=(* x).f1; struct
Cyc_List_List* _temp655; _LL651: if(( unsigned int) _temp649 > 1u?(( struct
_enum_struct*) _temp649)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL656: _temp655=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp649)->f1; goto
_LL652;} else{ goto _LL653;} _LL653: goto _LL654; _LL652:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp657=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp657=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=({ struct _tagged_string _temp658= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp658.last_plus_one -
_temp658.curr, _temp658.curr);})};( struct _xenum_struct*) _temp657;})); goto
_LL650; _LL654: goto _LL650; _LL650:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
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
_temp659=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp659->hd=( void*)(( struct Cyc_Absyn_Vardecl*) fallthru_binders->hd)->name;
_temp659->tl= fallthru_vars; _temp659;});} fallthru_vars=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fallthru_vars);{ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp660=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp660->v=(
void*) break_l; _temp660;}); ans->fallthru_info=({ struct Cyc_Core_Opt* _temp661=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp661->v=(
void*)({ struct _tuple9* _temp662=( struct _tuple9*) GC_malloc( sizeof( struct
_tuple9)); _temp662->f1= fallthru_l; _temp662->f2= fallthru_vars; _temp662->f3=
next_case_env->varmap; _temp662;}); _temp661;}); ans->to_pop_break= 0; ans->to_pop_fallthru=
0; return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp663=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp663->v=( void*)
break_l; _temp663;}); ans->fallthru_info= 0; ans->to_pop_break= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e,
struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp664=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp664->v=( void*)({
struct _tuple9* _temp665=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp665->f1= next_l; _temp665->f2= 0; _temp665->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp665;}); _temp664;}); ans->to_pop_break= 0; ans->to_pop_fallthru= 0; return
ans;} static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp666=( char*)"expecting a literal format string";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 34; _temp667;}));}{ struct _tagged_string
fmt_str;{ void* _temp668=( void*)(( struct Cyc_Absyn_Exp*) args->hd)->r; void*
_temp674; struct _tagged_string _temp676; int _temp678; _LL670: if((( struct
_enum_struct*) _temp668)->tag == Cyc_Absyn_Const_e_tag){ _LL675: _temp674=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp668)->f1; if(( unsigned int) _temp674 > 1u?((
struct _enum_struct*) _temp674)->tag == Cyc_Absyn_String_c_tag: 0){ _LL679:
_temp678=( int)(( struct Cyc_Absyn_String_c_struct*) _temp674)->f1; goto _LL677;
_LL677: _temp676=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp674)->f2; goto _LL671;} else{ goto _LL672;}} else{ goto _LL672;} _LL672:
goto _LL673; _LL671: fmt_str= _temp676; goto _LL669; _LL673: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp680=( char*)"expecting a literal format string"; struct _tagged_string
_temp681; _temp681.curr= _temp680; _temp681.base= _temp680; _temp681.last_plus_one=
_temp680 + 34; _temp681;})); _LL669:;}{ int len=( int)({ struct _tagged_string
_temp682= fmt_str;( unsigned int)( _temp682.last_plus_one - _temp682.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(( int)({ struct _tagged_string _temp683=
fmt_str; char* _temp685= _temp683.curr + i; if( _temp685 < _temp683.base? 1:
_temp685 >= _temp683.last_plus_one){ _throw( Null_Exception);}* _temp685;}) ==(
int)'%'){ i ++; if( i < len?( int)({ struct _tagged_string _temp686= fmt_str;
char* _temp688= _temp686.curr + i; if( _temp688 < _temp686.base? 1: _temp688 >=
_temp686.last_plus_one){ _throw( Null_Exception);}* _temp688;}) ==( int)'s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp689=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp689->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp689->tl= args; _temp689;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args= args->tl; typs= typs->tl;{
struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct
Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){ char c=({ struct
_tagged_string _temp690= fmt_str; char* _temp692= _temp690.curr + i; if(
_temp692 < _temp690.base? 1: _temp692 >= _temp690.last_plus_one){ _throw(
Null_Exception);}* _temp692;}); rev_fmt=({ struct Cyc_List_List* _temp693=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp693->hd=(
void*)(( int) c); _temp693->tl= rev_fmt; _temp693;}); if(( int) c !=( int)'%'){
continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp694=( char*)"bad format string"; struct
_tagged_string _temp695; _temp695.curr= _temp694; _temp695.base= _temp694;
_temp695.last_plus_one= _temp694 + 18; _temp695;}));}{ struct _tuple10 _temp698;
int _temp699; char _temp701; struct Cyc_List_List* _temp703; struct Cyc_List_List*
_temp705; struct Cyc_List_List* _temp707; struct Cyc_List_List* _temp709; struct
_tuple10* _temp696=( struct _tuple10*) x->v; _temp698=* _temp696; _LL710:
_temp709= _temp698.f1; goto _LL708; _LL708: _temp707= _temp698.f2; goto _LL706;
_LL706: _temp705= _temp698.f3; goto _LL704; _LL704: _temp703= _temp698.f4; goto
_LL702; _LL702: _temp701= _temp698.f5; goto _LL700; _LL700: _temp699= _temp698.f6;
goto _LL697; _LL697: i= _temp699 - 1; if(( int) _temp701 !=( int)'s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp711=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp711->hd=( void*) _temp709; _temp711->tl=({ struct Cyc_List_List* _temp712=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp712->hd=(
void*) _temp707; _temp712->tl=({ struct Cyc_List_List* _temp713=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp713->hd=( void*) _temp705;
_temp713->tl=({ struct Cyc_List_List* _temp714=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp714->hd=( void*) _temp703;
_temp714->tl= 0; _temp714;}); _temp713;}); _temp712;}); _temp711;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp715=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp715->hd=( void*)(( int) _temp701); _temp715->tl=
rev_fmt; _temp715;});{ struct Cyc_List_List* _temp716= _temp707; struct Cyc_List_List
_temp723; struct Cyc_List_List* _temp724; int _temp726; _LL718: if( _temp716 ==
0){ goto _LL720;} else{ _temp723=* _temp716; _LL727: _temp726=( int) _temp723.hd;
goto _LL725; _LL725: _temp724=( struct Cyc_List_List*) _temp723.tl; if( _temp724
== 0){ goto _LL722;} else{ goto _LL720;}} _LL722: if( _temp726 ==( int)'*'){
goto _LL719;} else{ goto _LL720;} _LL720: goto _LL721; _LL719: { struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp728=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp728->hd=( void*)({
struct _tuple11* _temp729=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp729->f1= temp; _temp729->f2=( void*) typs->hd; _temp729->f3=( struct Cyc_Absyn_Exp*)
args->hd; _temp729;}); _temp728->tl= rev_temps; _temp728;}); rev_result=({
struct Cyc_List_List* _temp730=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp730->hd=( void*) Cyc_Absyn_var_exp( temp, 0);
_temp730->tl= rev_result; _temp730;}); args= args->tl; typs= typs->tl; goto
_LL717;} _LL721: goto _LL717; _LL717:;}{ struct Cyc_List_List* _temp731=
_temp705; struct Cyc_List_List _temp738; struct Cyc_List_List* _temp739; struct
Cyc_List_List _temp741; struct Cyc_List_List* _temp742; int _temp744; int
_temp746; _LL733: if( _temp731 == 0){ goto _LL735;} else{ _temp738=* _temp731;
_LL747: _temp746=( int) _temp738.hd; goto _LL740; _LL740: _temp739=( struct Cyc_List_List*)
_temp738.tl; if( _temp739 == 0){ goto _LL735;} else{ _temp741=* _temp739; _LL745:
_temp744=( int) _temp741.hd; goto _LL743; _LL743: _temp742=( struct Cyc_List_List*)
_temp741.tl; if( _temp742 == 0){ goto _LL737;} else{ goto _LL735;}}} _LL737: if(
_temp746 ==( int)'.'? _temp744 ==( int)'*': 0){ goto _LL734;} else{ goto _LL735;}
_LL735: goto _LL736; _LL734: { struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp748=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp748->hd=( void*)({ struct _tuple11*
_temp749=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp749->f1=
temp; _temp749->f2=( void*) typs->hd; _temp749->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp749;}); _temp748->tl= rev_temps; _temp748;}); rev_result=({ struct Cyc_List_List*
_temp750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp750->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp750->tl= rev_result;
_temp750;}); args= args->tl; typs= typs->tl; goto _LL732;} _LL736: goto _LL732;
_LL732:;} if(( int) _temp701 !=( int)'%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp751=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp751.last_plus_one
- _temp751.curr, _temp751.curr);}));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp752=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp752->hd=( void*)({ struct _tuple11*
_temp753=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp753->f1=
temp; _temp753->f2=( void*) typs->hd; _temp753->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp753;}); _temp752->tl= rev_temps; _temp752;}); rev_result=({ struct Cyc_List_List*
_temp754=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp754->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp754->tl= rev_result;
_temp754;}); args= args->tl; typs= typs->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp755->hd=( void*)(( int)'.'); _temp755->tl= rev_fmt; _temp755;}); rev_fmt=({
struct Cyc_List_List* _temp756=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp756->hd=( void*)(( int)'*'); _temp756->tl= rev_fmt;
_temp756;}); rev_fmt=({ struct Cyc_List_List* _temp757=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp757->hd=( void*)(( int)'s');
_temp757->tl= rev_fmt; _temp757;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp758->hd=( void*)({ struct _tuple11*
_temp759=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp759->f1=
temp; _temp759->f2=( void*) typs->hd; _temp759->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp759;}); _temp758->tl= rev_temps; _temp758;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp760=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp760->hd=( void*) size_exp; _temp760->tl= rev_result; _temp760;});
rev_result=({ struct Cyc_List_List* _temp761=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp761->hd=( void*) cstring_exp; _temp761->tl=
rev_result; _temp761;}); args= args->tl; typs= typs->tl;}}}}}}{ struct Cyc_List_List*
es=({ struct Cyc_List_List* _temp762=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp762->hd=( void*) Cyc_Absyn_string_exp( 0, Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp762->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp762;}); if( aopt != 0){ es=({ struct Cyc_List_List* _temp763=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp763->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp763->tl=( struct Cyc_List_List*)
es; _temp763;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p,( struct Cyc_List_List*)
es, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps
!= 0; rev_temps= rev_temps->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple11*)
rev_temps->hd)).f1,(*(( struct _tuple11*) rev_temps->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple11*) rev_temps->hd)).f3, s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}}
static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp764=( char*)"Missing type in primop "; struct _tagged_string _temp765;
_temp765.curr= _temp764; _temp765.base= _temp764; _temp765.last_plus_one=
_temp764 + 24; _temp765;}));} return Cyc_Toc_typ_to_c(( void*)( e->topt)->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp766=( char*)"Missing type in primop "; struct _tagged_string _temp767;
_temp767.curr= _temp766; _temp767.base= _temp766; _temp767.last_plus_one=
_temp766 + 24; _temp767;}));} return( void*)( e->topt)->v;} static struct
_tuple6* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple6*
_temp768=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp768->f1=
Cyc_Toc_mt_tq; _temp768->f2= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); _temp768;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple8* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple8* _temp769=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp769->f1= 0; _temp769->f2=
e; _temp769;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct
_tuple1* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, int
is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* se= Cyc_Absyn_sizeof_exp(
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
struct _tagged_string* _temp770=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp770[ 0]= xprintf("f%d", i); _temp770;}), 0),(
struct Cyc_Absyn_Exp*) es->hd, 0), 0), s, 0);}} return Cyc_Toc_make_struct( x,
Cyc_Absyn_strct( n), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_List_List*
dles, struct _tuple1* tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*, struct
_tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es= es->tl){ struct _tuple8 _temp773; struct Cyc_Absyn_Exp* _temp774;
struct Cyc_List_List* _temp776; struct _tuple8* _temp771=( struct _tuple8*) es->hd;
_temp773=* _temp771; _LL777: _temp776= _temp773.f1; goto _LL775; _LL775:
_temp774= _temp773.f2; goto _LL772; _LL772: is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)( _temp774->topt)->v): 0; if( _temp776 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp778=(
char*)"empty designator list"; struct _tagged_string _temp779; _temp779.curr=
_temp778; _temp779.base= _temp778; _temp779.last_plus_one= _temp778 + 22;
_temp779;}));} if( _temp776->tl != 0){ struct _tuple1* y= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp( y, 0); for( 0; _temp776 != 0;
_temp776= _temp776->tl){ void* _temp780=( void*) _temp776->hd; struct
_tagged_string* _temp786; _LL782: if((( struct _enum_struct*) _temp780)->tag ==
Cyc_Absyn_FieldName_tag){ _LL787: _temp786=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp780)->f1; goto _LL783;} else{ goto _LL784;}
_LL784: goto _LL785; _LL783: if( Cyc_Toc_is_poly_field( struct_type, _temp786)){
yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp, _temp786, 0), yexp, 0),
0), s, 0); goto _LL781; _LL785:( void) _throw((( struct _xenum_struct*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp788=( char*)"array designator in struct"; struct _tagged_string _temp789;
_temp789.curr= _temp788; _temp789.base= _temp788; _temp789.last_plus_one=
_temp788 + 27; _temp789;}))); _LL781:;} Cyc_Toc_exp_to_c( nv, _temp774); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( yexp, _temp774, 0), 0), s, 0);} else{
void* _temp790=( void*) _temp776->hd; struct _tagged_string* _temp796; _LL792:
if((( struct _enum_struct*) _temp790)->tag == Cyc_Absyn_FieldName_tag){ _LL797:
_temp796=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp790)->f1; goto _LL793;} else{ goto _LL794;} _LL794: goto _LL795; _LL793:
Cyc_Toc_exp_to_c( nv, _temp774); if( Cyc_Toc_is_poly_field( struct_type,
_temp796)){ _temp774= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp774, 0);}
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,
_temp796, 0), _temp774, 0), 0), s, 0); goto _LL791; _LL795:( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp798=( char*)"array designator in struct"; struct
_tagged_string _temp799; _temp799.curr= _temp798; _temp799.base= _temp798;
_temp799.last_plus_one= _temp798 + 27; _temp799;}))); _LL791:;}}} return Cyc_Toc_make_struct(
x, Cyc_Absyn_strctq( tdn), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple12* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp800=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp800->tag= Cyc_Absyn_Increment_e_tag; _temp800->f1= e1; _temp800->f2=( void*)
incr;( void*) _temp800;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp801=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp811; struct Cyc_Absyn_Exp* _temp813; void* _temp815; struct _tagged_string*
_temp817; struct Cyc_Absyn_Exp* _temp819; _LL803: if((( struct _enum_struct*)
_temp801)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL812: _temp811=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp801)->f1; goto _LL804;} else{ goto
_LL805;} _LL805: if((( struct _enum_struct*) _temp801)->tag == Cyc_Absyn_Cast_e_tag){
_LL816: _temp815=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp801)->f1; goto
_LL814; _LL814: _temp813=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp801)->f2; goto _LL806;} else{ goto _LL807;} _LL807: if((( struct
_enum_struct*) _temp801)->tag == Cyc_Absyn_StructMember_e_tag){ _LL820: _temp819=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp801)->f1;
goto _LL818; _LL818: _temp817=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp801)->f2; goto _LL808;} else{ goto _LL809;} _LL809: goto _LL810; _LL804:((
void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp811, fs, f, f_env); goto _LL802; _LL806:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp813, fs, f, f_env); goto
_LL802; _LL808:( void*)( e1->r=( void*)(( void*) _temp819->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp821=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp821->hd=( void*) _temp817; _temp821->tl= fs;
_temp821;}), f, f_env); goto _LL802; _LL810: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp822= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp822.last_plus_one - _temp822.curr, _temp822.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_string*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)((
void*)( f( e1_copy, f_env))->r)); goto _LL802;} _LL802:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp823=( void*) s->r;
struct Cyc_Absyn_Exp* _temp833; struct Cyc_Absyn_Stmt* _temp835; struct Cyc_Absyn_Decl*
_temp837; struct Cyc_Absyn_Stmt* _temp839; struct Cyc_Absyn_Stmt* _temp841;
_LL825: if(( unsigned int) _temp823 > 1u?(( struct _enum_struct*) _temp823)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL834: _temp833=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp823)->f1; goto _LL826;} else{ goto _LL827;} _LL827:
if(( unsigned int) _temp823 > 1u?(( struct _enum_struct*) _temp823)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL838: _temp837=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp823)->f1; goto _LL836; _LL836: _temp835=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp823)->f2; goto _LL828;} else{ goto _LL829;}
_LL829: if(( unsigned int) _temp823 > 1u?(( struct _enum_struct*) _temp823)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL842: _temp841=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp823)->f1; goto _LL840; _LL840: _temp839=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp823)->f2; goto _LL830;}
else{ goto _LL831;} _LL831: goto _LL832; _LL826:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp833, fs, f, f_env); goto
_LL824; _LL828:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp835, fs, f, f_env); goto _LL824; _LL830:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp839, fs, f, f_env); goto
_LL824; _LL832:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp843= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp843.last_plus_one - _temp843.curr, _temp843.curr);})); goto _LL824; _LL824:;}
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
_tagged_string _temp844= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp844.last_plus_one - _temp844.curr, _temp844.curr);})); return;}{ void*
old_typ=( void*)( e->topt)->v; void* _temp845= r; void* _temp913; void* _temp916;
struct _tagged_string _temp918; int _temp920; void* _temp922; void* _temp924;
struct _tuple1* _temp926; struct _tuple1* _temp928; struct Cyc_List_List*
_temp930; void* _temp932; void* _temp934; struct Cyc_Absyn_Exp* _temp936; struct
Cyc_Absyn_Exp* _temp938; struct Cyc_Core_Opt* _temp940; struct Cyc_Absyn_Exp*
_temp942; struct Cyc_Absyn_Exp* _temp944; struct Cyc_Absyn_Exp* _temp946; struct
Cyc_Absyn_Exp* _temp948; struct Cyc_Absyn_Exp* _temp950; struct Cyc_Absyn_Exp*
_temp952; struct Cyc_List_List* _temp954; struct Cyc_Absyn_Exp* _temp956; struct
Cyc_List_List* _temp958; struct Cyc_Absyn_Exp* _temp960; struct Cyc_Absyn_Exp*
_temp962; struct Cyc_Absyn_Exp* _temp964; struct Cyc_List_List* _temp966; struct
Cyc_Absyn_Exp* _temp968; struct Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Exp**
_temp972; void* _temp973; void** _temp975; struct Cyc_Absyn_Exp* _temp976; void*
_temp978; struct Cyc_Absyn_Exp* _temp980; struct _tagged_string* _temp982;
struct Cyc_Absyn_Exp* _temp984; struct _tagged_string* _temp986; struct Cyc_Absyn_Exp*
_temp988; struct Cyc_Absyn_Exp* _temp990; struct Cyc_Absyn_Exp* _temp992; struct
Cyc_List_List* _temp994; struct Cyc_List_List* _temp996; struct _tuple2*
_temp998; struct Cyc_List_List* _temp1000; int _temp1002; struct Cyc_Absyn_Exp*
_temp1004; struct Cyc_Absyn_Exp* _temp1006; struct Cyc_Absyn_Vardecl* _temp1008;
struct Cyc_Absyn_Structdecl* _temp1010; struct Cyc_List_List* _temp1012; struct
Cyc_Core_Opt* _temp1014; struct _tuple1* _temp1016; struct Cyc_Absyn_Enumfield*
_temp1018; struct Cyc_Absyn_Enumdecl* _temp1020; struct Cyc_List_List* _temp1022;
struct Cyc_Core_Opt* _temp1024; struct Cyc_Core_Opt* _temp1026; struct Cyc_Absyn_Enumfield*
_temp1028; struct Cyc_Absyn_Xenumdecl* _temp1030; struct Cyc_List_List*
_temp1032; struct Cyc_Core_Opt* _temp1034; struct Cyc_Absyn_Stmt* _temp1036;
struct Cyc_List_List* _temp1038; struct Cyc_Core_Opt* _temp1040; struct Cyc_Absyn_Fndecl*
_temp1042; struct Cyc_Absyn_Exp* _temp1044; _LL847: if((( struct _enum_struct*)
_temp845)->tag == Cyc_Absyn_Const_e_tag){ _LL914: _temp913=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp845)->f1; if( _temp913 == Cyc_Absyn_Null_c){ goto _LL848;} else{ goto
_LL849;}} else{ goto _LL849;} _LL849: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_Const_e_tag){ _LL917: _temp916=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp845)->f1; if(( unsigned int) _temp916 > 1u?(( struct _enum_struct*)
_temp916)->tag == Cyc_Absyn_String_c_tag: 0){ _LL921: _temp920=( int)(( struct
Cyc_Absyn_String_c_struct*) _temp916)->f1; goto _LL919; _LL919: _temp918=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp916)->f2; goto
_LL915;} else{ goto _LL851;}} else{ goto _LL851;} _LL915: if( _temp920){ goto
_LL850;} else{ goto _LL851;} _LL851: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_Const_e_tag){ _LL923: _temp922=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp845)->f1; goto _LL852;} else{ goto _LL853;} _LL853: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Var_e_tag){ _LL927: _temp926=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp845)->f1; goto _LL925; _LL925:
_temp924=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp845)->f2; goto _LL854;}
else{ goto _LL855;} _LL855: if((( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL929: _temp928=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp845)->f1; goto _LL856;} else{ goto _LL857;} _LL857: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Primop_e_tag){ _LL933: _temp932=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp845)->f1; goto _LL931; _LL931:
_temp930=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp845)->f2;
goto _LL858;} else{ goto _LL859;} _LL859: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_Increment_e_tag){ _LL937: _temp936=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp845)->f1; goto _LL935; _LL935:
_temp934=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp845)->f2; goto
_LL860;} else{ goto _LL861;} _LL861: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL943: _temp942=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp845)->f1; goto _LL941; _LL941: _temp940=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp845)->f2; goto
_LL939; _LL939: _temp938=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp845)->f3; goto _LL862;} else{ goto _LL863;} _LL863: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Conditional_e_tag){ _LL949: _temp948=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp845)->f1;
goto _LL947; _LL947: _temp946=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp845)->f2; goto _LL945; _LL945: _temp944=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp845)->f3; goto _LL864;} else{ goto _LL865;}
_LL865: if((( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL953: _temp952=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp845)->f1; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp845)->f2; goto _LL866;} else{ goto _LL867;}
_LL867: if((( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL957: _temp956=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp845)->f1; goto _LL955; _LL955: _temp954=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp845)->f2; goto _LL868;} else{ goto _LL869;}
_LL869: if((( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_FnCall_e_tag){
_LL961: _temp960=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp845)->f1; goto _LL959; _LL959: _temp958=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp845)->f2; goto _LL870;} else{ goto _LL871;}
_LL871: if((( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_Throw_e_tag){
_LL963: _temp962=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp845)->f1; goto _LL872;} else{ goto _LL873;} _LL873: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL965:
_temp964=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp845)->f1; goto _LL874;} else{ goto _LL875;} _LL875: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL969: _temp968=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp845)->f1;
goto _LL967; _LL967: _temp966=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp845)->f2; goto _LL876;} else{ goto _LL877;} _LL877: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Cast_e_tag){ _LL974: _temp973=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp845)->f1; _temp975=&(( struct Cyc_Absyn_Cast_e_struct*)
_temp845)->f1; goto _LL971; _LL971: _temp970=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp845)->f2; _temp972=&(( struct Cyc_Absyn_Cast_e_struct*)
_temp845)->f2; goto _LL878;} else{ goto _LL879;} _LL879: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Address_e_tag){ _LL977: _temp976=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp845)->f1; goto
_LL880;} else{ goto _LL881;} _LL881: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_Sizeof_e_tag){ _LL979: _temp978=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp845)->f1; goto _LL882;} else{ goto _LL883;} _LL883: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Deref_e_tag){ _LL981: _temp980=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp845)->f1; goto
_LL884;} else{ goto _LL885;} _LL885: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL985: _temp984=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp845)->f1; goto _LL983; _LL983:
_temp982=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp845)->f2; goto _LL886;} else{ goto _LL887;} _LL887: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL989: _temp988=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp845)->f1;
goto _LL987; _LL987: _temp986=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp845)->f2; goto _LL888;} else{ goto _LL889;} _LL889: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Subscript_e_tag){ _LL993: _temp992=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp845)->f1;
goto _LL991; _LL991: _temp990=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp845)->f2; goto _LL890;} else{ goto _LL891;} _LL891: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Tuple_e_tag){ _LL995: _temp994=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp845)->f1; goto
_LL892;} else{ goto _LL893;} _LL893: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL999: _temp998=( struct _tuple2*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp845)->f1; goto _LL997; _LL997: _temp996=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp845)->f2;
goto _LL894;} else{ goto _LL895;} _LL895: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_Array_e_tag){ _LL1003: _temp1002=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp845)->f1; goto _LL1001; _LL1001: _temp1000=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp845)->f2; goto _LL896;} else{ goto _LL897;}
_LL897: if((( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL1009: _temp1008=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp845)->f1; goto _LL1007; _LL1007: _temp1006=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp845)->f2; goto _LL1005; _LL1005:
_temp1004=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp845)->f3; goto _LL898;} else{ goto _LL899;} _LL899: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Struct_e_tag){ _LL1017: _temp1016=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp845)->f1; goto
_LL1015; _LL1015: _temp1014=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp845)->f2; goto _LL1013; _LL1013: _temp1012=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp845)->f3; goto _LL1011; _LL1011:
_temp1010=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp845)->f4; goto _LL900;} else{ goto _LL901;} _LL901: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Enum_e_tag){ _LL1027: _temp1026=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp845)->f1; goto
_LL1025; _LL1025: _temp1024=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp845)->f2; goto _LL1023; _LL1023: _temp1022=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp845)->f3; goto _LL1021; _LL1021: _temp1020=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp845)->f4;
goto _LL1019; _LL1019: _temp1018=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp845)->f5; goto _LL902;} else{ goto _LL903;} _LL903: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1035: _temp1034=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp845)->f1; goto
_LL1033; _LL1033: _temp1032=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp845)->f2; goto _LL1031; _LL1031: _temp1030=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp845)->f3; goto _LL1029; _LL1029:
_temp1028=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp845)->f4; goto _LL904;} else{ goto _LL905;} _LL905: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1037: _temp1036=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp845)->f1;
goto _LL906;} else{ goto _LL907;} _LL907: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_UnresolvedMem_e_tag){ _LL1041: _temp1040=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp845)->f1; goto _LL1039; _LL1039:
_temp1038=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp845)->f2; goto _LL908;} else{ goto _LL909;} _LL909: if((( struct
_enum_struct*) _temp845)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1043: _temp1042=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp845)->f1;
goto _LL910;} else{ goto _LL911;} _LL911: if((( struct _enum_struct*) _temp845)->tag
== Cyc_Absyn_Fill_e_tag){ _LL1045: _temp1044=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Fill_e_struct*) _temp845)->f1; goto _LL912;} else{ goto _LL846;}
_LL848:{ void* _temp1046= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1052; struct Cyc_Absyn_Conref* _temp1054; struct Cyc_Absyn_Tqual* _temp1056;
struct Cyc_Absyn_Conref* _temp1058; void* _temp1060; void* _temp1062; _LL1048:
if(( unsigned int) _temp1046 > 5u?(( struct _enum_struct*) _temp1046)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1053: _temp1052=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1046)->f1; _LL1063: _temp1062=( void*)
_temp1052.elt_typ; goto _LL1061; _LL1061: _temp1060=( void*) _temp1052.rgn_typ;
goto _LL1059; _LL1059: _temp1058=( struct Cyc_Absyn_Conref*) _temp1052.nullable;
goto _LL1057; _LL1057: _temp1056=( struct Cyc_Absyn_Tqual*) _temp1052.tq; goto
_LL1055; _LL1055: _temp1054=( struct Cyc_Absyn_Conref*) _temp1052.bounds; goto
_LL1049;} else{ goto _LL1050;} _LL1050: goto _LL1051; _LL1049:{ void* _temp1064=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1054); struct
Cyc_Absyn_Exp* _temp1070; _LL1066: if( _temp1064 == Cyc_Absyn_Unknown_b){ goto
_LL1067;} else{ goto _LL1068;} _LL1068: if(( unsigned int) _temp1064 > 1u?((
struct _enum_struct*) _temp1064)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1071:
_temp1070=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1064)->f1;
goto _LL1069;} else{ goto _LL1065;} _LL1067: { struct _tuple8* zero=({ struct
_tuple8* _temp1072=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1072->f1= 0; _temp1072->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1072;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1073=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1073->hd=( void*) zero; _temp1073->tl=({ struct Cyc_List_List* _temp1074=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1074->hd=(
void*) zero; _temp1074->tl=({ struct Cyc_List_List* _temp1075=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1075->hd=( void*) zero;
_temp1075->tl= 0; _temp1075;}); _temp1074;}); _temp1073;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1062), Cyc_Toc_mt_tq)); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1065;} _LL1069:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1065; _LL1065:;} goto _LL1047; _LL1051:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1076= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1076.last_plus_one -
_temp1076.curr, _temp1076.curr);})); goto _LL1047; _LL1047:;} goto _LL846;
_LL850:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1077=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1077->f1= Cyc_Toc_abs_ns; _temp1077->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1077;}), 0),({ struct Cyc_List_List* _temp1078=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1078->hd=( void*) Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp1079=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1079->tag= Cyc_Absyn_Const_e_tag;
_temp1079->f1=( void*)({ struct Cyc_Absyn_String_c_struct* _temp1080=( struct
Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1080->tag= Cyc_Absyn_String_c_tag; _temp1080->f1= 0; _temp1080->f2=
_temp918;( void*) _temp1080;});( void*) _temp1079;}), 0); _temp1078->tl= 0;
_temp1078;}), 0))->r)); goto _LL846; _LL852: goto _LL846; _LL854:{ struct
_handler_cons _temp1081; _push_handler(& _temp1081);{ struct _xenum_struct*
_temp1082=( struct _xenum_struct*) setjmp( _temp1081.handler); if( ! _temp1082){(
void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict*
d, struct _tuple1* key)) Cyc_Dict_lookup)( nv->varmap, _temp926))->r));
_pop_handler();} else{ struct _xenum_struct* _temp1084= _temp1082; _LL1086: if(
_temp1084->tag == Cyc_Dict_Absent_tag){ goto _LL1087;} else{ goto _LL1088;}
_LL1088: goto _LL1089; _LL1087:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1090= Cyc_Absynpp_qvar2string( _temp926); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1090.last_plus_one - _temp1090.curr, _temp1090.curr);})); return; _LL1089:(
void) _throw( _temp1084); _LL1085:;}}} goto _LL846; _LL856:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1091=(
char*)"unknownid"; struct _tagged_string _temp1092; _temp1092.curr= _temp1091;
_temp1092.base= _temp1091; _temp1092.last_plus_one= _temp1091 + 10; _temp1092;}));
goto _LL846; _LL858: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp930); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp930);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp930);{ void* _temp1093= _temp932; _LL1095: if( _temp1093 == Cyc_Absyn_Size){
goto _LL1096;} else{ goto _LL1097;} _LL1097: if( _temp1093 == Cyc_Absyn_Printf){
goto _LL1098;} else{ goto _LL1099;} _LL1099: if( _temp1093 == Cyc_Absyn_Xprintf){
goto _LL1100;} else{ goto _LL1101;} _LL1101: if( _temp1093 == Cyc_Absyn_Fprintf){
goto _LL1102;} else{ goto _LL1103;} _LL1103: if( _temp1093 == Cyc_Absyn_Sscanf){
goto _LL1104;} else{ goto _LL1105;} _LL1105: if( _temp1093 == Cyc_Absyn_Plus){
goto _LL1106;} else{ goto _LL1107;} _LL1107: if( _temp1093 == Cyc_Absyn_Minus){
goto _LL1108;} else{ goto _LL1109;} _LL1109: if( _temp1093 == Cyc_Absyn_Eq){
goto _LL1110;} else{ goto _LL1111;} _LL1111: if( _temp1093 == Cyc_Absyn_Neq){
goto _LL1112;} else{ goto _LL1113;} _LL1113: if( _temp1093 == Cyc_Absyn_Gt){
goto _LL1114;} else{ goto _LL1115;} _LL1115: if( _temp1093 == Cyc_Absyn_Gte){
goto _LL1116;} else{ goto _LL1117;} _LL1117: if( _temp1093 == Cyc_Absyn_Lt){
goto _LL1118;} else{ goto _LL1119;} _LL1119: if( _temp1093 == Cyc_Absyn_Lte){
goto _LL1120;} else{ goto _LL1121;} _LL1121: goto _LL1122; _LL1096: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*) _temp930->hd;{ void* _temp1123=( void*)( arg->topt)->v;
struct Cyc_Absyn_Exp* _temp1131; struct Cyc_Absyn_Tqual* _temp1133; void*
_temp1135; struct Cyc_Absyn_PtrInfo _temp1137; struct Cyc_Absyn_Conref*
_temp1139; struct Cyc_Absyn_Tqual* _temp1141; struct Cyc_Absyn_Conref* _temp1143;
void* _temp1145; void* _temp1147; _LL1125: if(( unsigned int) _temp1123 > 5u?((
struct _enum_struct*) _temp1123)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1136:
_temp1135=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1123)->f1; goto
_LL1134; _LL1134: _temp1133=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1123)->f2; goto _LL1132; _LL1132: _temp1131=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1123)->f3; goto _LL1126;} else{ goto
_LL1127;} _LL1127: if(( unsigned int) _temp1123 > 5u?(( struct _enum_struct*)
_temp1123)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1138: _temp1137=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1123)->f1;
_LL1148: _temp1147=( void*) _temp1137.elt_typ; goto _LL1146; _LL1146: _temp1145=(
void*) _temp1137.rgn_typ; goto _LL1144; _LL1144: _temp1143=( struct Cyc_Absyn_Conref*)
_temp1137.nullable; goto _LL1142; _LL1142: _temp1141=( struct Cyc_Absyn_Tqual*)
_temp1137.tq; goto _LL1140; _LL1140: _temp1139=( struct Cyc_Absyn_Conref*)
_temp1137.bounds; goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130;
_LL1126:( void*)( e->r=( void*)(( void*) _temp1131->r)); goto _LL1124; _LL1128: {
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)( arg->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1124;}
_LL1130:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1149= Cyc_Absynpp_typ2string(( void*) typs->hd); struct
_tagged_string _temp1150= Cyc_Absynpp_typ2string(( void*)( arg->topt)->v);
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1149.last_plus_one
- _temp1149.curr, _temp1149.curr, _temp1150.last_plus_one - _temp1150.curr,
_temp1150.curr);})); return; _LL1124:;} goto _LL1094;} _LL1098:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp932, 0, _temp930, typs))->r)); goto
_LL1094; _LL1100:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp932,
0, _temp930, typs))->r)); goto _LL1094; _LL1102: if( _temp930 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1151=( char*)"fprintf without arguments"; struct _tagged_string _temp1152;
_temp1152.curr= _temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one=
_temp1151 + 26; _temp1152;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp932,({ struct Cyc_Core_Opt* _temp1153=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1153->v=( void*)(( struct Cyc_Absyn_Exp*)
_temp930->hd); _temp1153;}), _temp930->tl, typs->tl))->r)); goto _LL1094;
_LL1104: if( _temp930 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1154=( char*)"sscanf without arguments";
struct _tagged_string _temp1155; _temp1155.curr= _temp1154; _temp1155.base=
_temp1154; _temp1155.last_plus_one= _temp1154 + 25; _temp1155;}));}( void*)(((
struct Cyc_Absyn_Exp*) _temp930->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*) _temp930->hd)->r,(( struct
Cyc_Absyn_Exp*) _temp930->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1094;
_LL1106: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp930->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp930->tl)->hd; struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp,
0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1156=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1156->v=( void*) Cyc_Absyn_Plus; _temp1156;}), e2, 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1094; _LL1108: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp930->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp930->tl)->hd; void*
old_typ_e2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1157=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1157->v=( void*) Cyc_Absyn_Minus; _temp1157;}), e2,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1094; _LL1110: goto _LL1112; _LL1112:
goto _LL1114; _LL1114: goto _LL1116; _LL1116: goto _LL1118; _LL1118: goto
_LL1120; _LL1120: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp930->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp930->tl)->hd; void* t1=(
void*) old_typs->hd; void* t2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1094;} _LL1122: goto
_LL1094; _LL1094:;} goto _LL846;} _LL860: { void* e2_cyc_typ=( void*)( _temp936->topt)->v;
Cyc_Toc_exp_to_c( nv, _temp936); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){( void*)( _temp936->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp936->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( !
Cyc_Absyn_is_lvalue( _temp936)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp936, 0, Cyc_Toc_incr_lvalue, _temp934);( void*)( e->r=( void*)(( void*)
_temp936->r));} goto _LL846;} _LL862: { int e1_poly= Cyc_Toc_is_poly_project(
_temp942); void* e1_old_typ=( void*)( _temp942->topt)->v; void* e2_old_typ=(
void*)( _temp938->topt)->v; Cyc_Toc_exp_to_c( nv, _temp942); Cyc_Toc_exp_to_c(
nv, _temp938); if( _temp940 != 0?( void*) _temp940->v == Cyc_Absyn_Plus? 1:(
void*) _temp940->v == Cyc_Absyn_Minus: 0){ if( Cyc_Tcutil_is_tagged_pointer_typ(
e1_old_typ)){( void*)( _temp942->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp942->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp938->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp938->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp938->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp938->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp942)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple12*), struct
_tuple12* f_env)) Cyc_Toc_lvalue_assign)( _temp942, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple12* _temp1158=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12) * 1); _temp1158[ 0]=({ struct _tuple12 _temp1159; _temp1159.f1=
_temp940; _temp1159.f2= _temp938; _temp1159;}); _temp1158;}));( void*)( e->r=(
void*)(( void*) _temp942->r));} goto _LL846;} _LL864: Cyc_Toc_exp_to_c( nv,
_temp948); Cyc_Toc_exp_to_c( nv, _temp946); Cyc_Toc_exp_to_c( nv, _temp944);
goto _LL846; _LL866: Cyc_Toc_exp_to_c( nv, _temp952); Cyc_Toc_exp_to_c( nv,
_temp950); goto _LL846; _LL868: Cyc_Toc_exp_to_c( nv, _temp956);(( void(*)( void(*
f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct
Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp954); goto
_LL846; _LL870: Cyc_Toc_exp_to_c( nv, _temp960);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp958); goto _LL846; _LL872: Cyc_Toc_exp_to_c(
nv, _temp962);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp962), 0))->r)); goto _LL846; _LL874: Cyc_Toc_exp_to_c(
nv, _temp964); goto _LL846; _LL876: Cyc_Toc_exp_to_c( nv, _temp968);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); for( 0; _temp966 != 0; _temp966=
_temp966->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp966->hd); if( k != Cyc_Absyn_EffKind?
k != Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp968, 0))->r)); break;}} goto _LL846;} _LL878: { void* old_t2=( void*)((*
_temp972)->topt)->v; void* new_typ=* _temp975;* _temp975= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv,* _temp972);* _temp972= Cyc_Toc_convert_exp( nv->toplevel,
old_t2, new_typ,* _temp972); goto _LL846;} _LL880:{ void* _temp1160=( void*)
_temp976->r; struct Cyc_Absyn_Structdecl* _temp1168; struct Cyc_List_List*
_temp1170; struct Cyc_Core_Opt* _temp1172; struct _tuple1* _temp1174; struct Cyc_List_List*
_temp1176; _LL1162: if((( struct _enum_struct*) _temp1160)->tag == Cyc_Absyn_Struct_e_tag){
_LL1175: _temp1174=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1160)->f1; goto _LL1173; _LL1173: _temp1172=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1160)->f2; goto _LL1171; _LL1171:
_temp1170=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1160)->f3; goto _LL1169; _LL1169: _temp1168=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1160)->f4; goto _LL1163;} else{ goto
_LL1164;} _LL1164: if((( struct _enum_struct*) _temp1160)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1177: _temp1176=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1160)->f1; goto _LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167;
_LL1163: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1178= Cyc_Position_string_of_segment( _temp976->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1178.last_plus_one -
_temp1178.curr, _temp1178.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)( _temp976->topt)->v, 1, _temp1170, _temp1174))->r)); goto _LL1161;
_LL1165: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1179= Cyc_Position_string_of_segment( _temp976->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1179.last_plus_one -
_temp1179.curr, _temp1179.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1176))->r)); goto _LL1161; _LL1167: Cyc_Toc_exp_to_c( nv, _temp976);
if( ! Cyc_Absyn_is_lvalue( _temp976)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int),
int f_env)) Cyc_Toc_lvalue_assign)( _temp976, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*) _temp976->r));} goto _LL1161; _LL1161:;} goto
_LL846; _LL882:( void*)( e->r=( void*)({ struct Cyc_Absyn_Sizeof_e_struct*
_temp1180=( struct Cyc_Absyn_Sizeof_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeof_e_struct));
_temp1180->tag= Cyc_Absyn_Sizeof_e_tag; _temp1180->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp978);( void*) _temp1180;})); goto _LL846; _LL884: Cyc_Toc_exp_to_c( nv,
_temp980);{ void* _temp1181= Cyc_Tcutil_compress(( void*)( _temp980->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1187; struct Cyc_Absyn_Conref* _temp1189; struct
Cyc_Absyn_Tqual* _temp1191; struct Cyc_Absyn_Conref* _temp1193; void* _temp1195;
void* _temp1197; _LL1183: if(( unsigned int) _temp1181 > 5u?(( struct
_enum_struct*) _temp1181)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1188:
_temp1187=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1181)->f1; _LL1198: _temp1197=( void*) _temp1187.elt_typ; goto _LL1196;
_LL1196: _temp1195=( void*) _temp1187.rgn_typ; goto _LL1194; _LL1194: _temp1193=(
struct Cyc_Absyn_Conref*) _temp1187.nullable; goto _LL1192; _LL1192: _temp1191=(
struct Cyc_Absyn_Tqual*) _temp1187.tq; goto _LL1190; _LL1190: _temp1189=( struct
Cyc_Absyn_Conref*) _temp1187.bounds; goto _LL1184;} else{ goto _LL1185;} _LL1185:
goto _LL1186; _LL1184:{ void* _temp1199=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1189); struct Cyc_Absyn_Exp* _temp1205; _LL1201: if(
_temp1199 == Cyc_Absyn_Unknown_b){ goto _LL1202;} else{ goto _LL1203;} _LL1203:
if(( unsigned int) _temp1199 > 1u?(( struct _enum_struct*) _temp1199)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1206: _temp1205=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1199)->f1; goto _LL1204;} else{ goto _LL1200;}
_LL1202: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
void* ta1= Cyc_Toc_typ_to_c_array( _temp1197); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1191); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1191); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp980, s1, 0), 0))->r));
goto _LL1200;} _LL1204: { int nullable= Cyc_Toc_is_nullable(( void*)( _temp980->topt)->v);
if( nullable){ struct _tuple1* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)( _temp980->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( _temp1205->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp980, s3, 0), 0))->r));} goto _LL1200;} _LL1200:;} goto _LL1182; _LL1186:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1207=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1208; _temp1208.curr= _temp1207; _temp1208.base= _temp1207; _temp1208.last_plus_one=
_temp1207 + 29; _temp1208;})); goto _LL1182; _LL1182:;} goto _LL846; _LL886: Cyc_Toc_exp_to_c(
nv, _temp984); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL846; _LL888: Cyc_Toc_exp_to_c( nv, _temp988);
if( Cyc_Toc_is_nullable(( void*)( _temp988->topt)->v)){ struct _tuple1* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)( _temp988->topt)->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp986, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp988, s3, 0), 0))->r));} else{ if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}} goto _LL846; _LL890: { void*
e1_typ= Cyc_Tcutil_compress(( void*)( _temp992->topt)->v); Cyc_Toc_exp_to_c( nv,
_temp992); Cyc_Toc_exp_to_c( nv, _temp990);( void*)( _temp992->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*)
_temp992->r, 0), 0))->r));{ void* _temp1209= e1_typ; struct Cyc_List_List*
_temp1219; struct Cyc_Absyn_Exp* _temp1221; struct Cyc_Absyn_Tqual* _temp1223;
void* _temp1225; struct Cyc_Absyn_PtrInfo _temp1227; struct Cyc_Absyn_Conref*
_temp1229; struct Cyc_Absyn_Tqual* _temp1231; struct Cyc_Absyn_Conref* _temp1233;
void* _temp1235; void* _temp1237; _LL1211: if(( unsigned int) _temp1209 > 5u?((
struct _enum_struct*) _temp1209)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1220:
_temp1219=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1209)->f1; goto _LL1212;} else{ goto _LL1213;} _LL1213: if(( unsigned int)
_temp1209 > 5u?(( struct _enum_struct*) _temp1209)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1226: _temp1225=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1209)->f1;
goto _LL1224; _LL1224: _temp1223=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1209)->f2; goto _LL1222; _LL1222: _temp1221=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1209)->f3; goto _LL1214;} else{ goto
_LL1215;} _LL1215: if(( unsigned int) _temp1209 > 5u?(( struct _enum_struct*)
_temp1209)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1228: _temp1227=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1209)->f1;
_LL1238: _temp1237=( void*) _temp1227.elt_typ; goto _LL1236; _LL1236: _temp1235=(
void*) _temp1227.rgn_typ; goto _LL1234; _LL1234: _temp1233=( struct Cyc_Absyn_Conref*)
_temp1227.nullable; goto _LL1232; _LL1232: _temp1231=( struct Cyc_Absyn_Tqual*)
_temp1227.tq; goto _LL1230; _LL1230: _temp1229=( struct Cyc_Absyn_Conref*)
_temp1227.bounds; goto _LL1216;} else{ goto _LL1217;} _LL1217: goto _LL1218;
_LL1212: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp990) +(
unsigned int) 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp992,({ struct _tagged_string* _temp1239=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1239[ 0]= xprintf("f%d",( int) i);
_temp1239;}), 0))->r)); goto _LL1210;} _LL1214:( void*)( e->r=( void*)(( void*)(
Cyc_Toc_known_size_subscript( _temp992, _temp990, _temp1221, _temp1225,
_temp1223, 0))->r)); goto _LL1210; _LL1216:{ void* _temp1240=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1229); struct Cyc_Absyn_Exp*
_temp1246; _LL1242: if(( unsigned int) _temp1240 > 1u?(( struct _enum_struct*)
_temp1240)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1247: _temp1246=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1240)->f1; goto _LL1243;} else{ goto
_LL1244;} _LL1244: if( _temp1240 == Cyc_Absyn_Unknown_b){ goto _LL1245;} else{
goto _LL1241;} _LL1243:( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp992, _temp990, _temp1246, _temp1237, _temp1231, 0))->r)); goto _LL1241;
_LL1245: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1237); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1231); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1231); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp990, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp992, s1, 0), 0))->r)); goto _LL1241;} _LL1241:;}
goto _LL1210; _LL1218:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1248=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1249; _temp1249.curr= _temp1248; _temp1249.base=
_temp1248; _temp1249.last_plus_one= _temp1248 + 49; _temp1249;})); goto _LL1210;
_LL1210:;} goto _LL846;} _LL892: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp994))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp994); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp994 != 0; _temp994= _temp994->tl, i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp994->hd);{ struct Cyc_List_List* des=({ struct
Cyc_List_List* _temp1250=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1250->hd=( void*)({ struct Cyc_Absyn_FieldName_struct* _temp1251=( struct
Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1251->tag= Cyc_Absyn_FieldName_tag; _temp1251->f1=({ struct _tagged_string*
_temp1252=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1252[ 0]= xprintf("f%d", i); _temp1252;});( void*) _temp1251;});
_temp1250->tl= 0; _temp1250;}); dles=({ struct Cyc_List_List* _temp1253=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1253->hd=( void*)({
struct _tuple8* _temp1254=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1254->f1=( struct Cyc_List_List*) des; _temp1254->f2=( struct Cyc_Absyn_Exp*)
_temp994->hd; _temp1254;}); _temp1253->tl= dles; _temp1253;});}}} dles=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp(
0, dles, 0), 0))->r));} goto _LL846; _LL894:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1255=( char*)"compoundlit";
struct _tagged_string _temp1256; _temp1256.curr= _temp1255; _temp1256.base=
_temp1255; _temp1256.last_plus_one= _temp1255 + 12; _temp1256;})); goto _LL846;
_LL896: if( nv->toplevel? 1: ! _temp1002){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1000, 0))->r));{ struct Cyc_List_List* dles= _temp1000; for( 0; dles !=
0; dles= dles->tl){ struct _tuple8 _temp1259; struct Cyc_Absyn_Exp* _temp1260;
struct Cyc_List_List* _temp1262; struct _tuple8* _temp1257=( struct _tuple8*)
dles->hd; _temp1259=* _temp1257; _LL1263: _temp1262= _temp1259.f1; goto _LL1261;
_LL1261: _temp1260= _temp1259.f2; goto _LL1258; _LL1258: Cyc_Toc_exp_to_c( nv,
_temp1260);}}} else{ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1000) - 1;{ struct Cyc_List_List* dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1000);
for( 0; dles != 0; dles= dles->tl){ struct _tuple8 _temp1266; struct Cyc_Absyn_Exp*
_temp1267; struct Cyc_List_List* _temp1269; struct _tuple8* _temp1264=( struct
_tuple8*) dles->hd; _temp1266=* _temp1264; _LL1270: _temp1269= _temp1266.f1;
goto _LL1268; _LL1268: _temp1267= _temp1266.f2; goto _LL1265; _LL1265: { struct
Cyc_Absyn_Exp* e_index; if( _temp1269 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if( _temp1269->tl != 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1271=( char*)"multiple designators in array";
struct _tagged_string _temp1272; _temp1272.curr= _temp1271; _temp1272.base=
_temp1271; _temp1272.last_plus_one= _temp1271 + 30; _temp1272;}));}{ void* d=(
void*) _temp1269->hd; void* _temp1273= d; struct Cyc_Absyn_Exp* _temp1279;
struct _tagged_string* _temp1281; _LL1275: if((( struct _enum_struct*) _temp1273)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1280: _temp1279=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1273)->f1; goto _LL1276;} else{ goto
_LL1277;} _LL1277: if((( struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_FieldName_tag){
_LL1282: _temp1281=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1273)->f1; goto _LL1278;} else{ goto _LL1274;} _LL1276: Cyc_Toc_exp_to_c(
nv, _temp1279); e_index= _temp1279; goto _LL1274; _LL1278: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1283=( char*)"field name designators in array"; struct _tagged_string
_temp1284; _temp1284.curr= _temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one=
_temp1283 + 32; _temp1284;})); goto _LL1274; _LL1274:;}} Cyc_Toc_exp_to_c( nv,
_temp1267); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1267, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1285= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1291; struct Cyc_Absyn_Conref* _temp1293; struct Cyc_Absyn_Tqual* _temp1295;
struct Cyc_Absyn_Conref* _temp1297; void* _temp1299; void* _temp1301; _LL1287:
if(( unsigned int) _temp1285 > 5u?(( struct _enum_struct*) _temp1285)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1292: _temp1291=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1285)->f1; _LL1302: _temp1301=( void*)
_temp1291.elt_typ; goto _LL1300; _LL1300: _temp1299=( void*) _temp1291.rgn_typ;
goto _LL1298; _LL1298: _temp1297=( struct Cyc_Absyn_Conref*) _temp1291.nullable;
goto _LL1296; _LL1296: _temp1295=( struct Cyc_Absyn_Tqual*) _temp1291.tq; goto
_LL1294; _LL1294: _temp1293=( struct Cyc_Absyn_Conref*) _temp1291.bounds; goto
_LL1288;} else{ goto _LL1289;} _LL1289: goto _LL1290; _LL1288: old_elt_typ=
_temp1301; goto _LL1286; _LL1290: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1303=( char*)"exp_to_c:array expression doesn't have array type";
struct _tagged_string _temp1304; _temp1304.curr= _temp1303; _temp1304.base=
_temp1303; _temp1304.last_plus_one= _temp1303 + 50; _temp1304;})); goto _LL1286;
_LL1286:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeof_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1000), 0), 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ,
e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r));}}} goto _LL846; _LL898: { struct _tuple1*
x1= _temp1008->name; int is_tagged_ptr= 0;{ void* _temp1305= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1311; struct Cyc_Absyn_Conref* _temp1313;
struct Cyc_Absyn_Tqual* _temp1315; struct Cyc_Absyn_Conref* _temp1317; void*
_temp1319; void* _temp1321; _LL1307: if(( unsigned int) _temp1305 > 5u?(( struct
_enum_struct*) _temp1305)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1312:
_temp1311=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1305)->f1; _LL1322: _temp1321=( void*) _temp1311.elt_typ; goto _LL1320;
_LL1320: _temp1319=( void*) _temp1311.rgn_typ; goto _LL1318; _LL1318: _temp1317=(
struct Cyc_Absyn_Conref*) _temp1311.nullable; goto _LL1316; _LL1316: _temp1315=(
struct Cyc_Absyn_Tqual*) _temp1311.tq; goto _LL1314; _LL1314: _temp1313=( struct
Cyc_Absyn_Conref*) _temp1311.bounds; goto _LL1308;} else{ goto _LL1309;} _LL1309:
goto _LL1310; _LL1308:{ void* _temp1323=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1313))->v; void*
_temp1329; _LL1325: if(( unsigned int) _temp1323 > 1u?(( struct _enum_struct*)
_temp1323)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1330: _temp1329=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1323)->f1; if( _temp1329 == Cyc_Absyn_Unknown_b){
goto _LL1326;} else{ goto _LL1327;}} else{ goto _LL1327;} _LL1327: goto _LL1328;
_LL1326: is_tagged_ptr= 1; goto _LL1324; _LL1328: goto _LL1324; _LL1324:;} goto
_LL1306; _LL1310:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1331=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1332; _temp1332.curr= _temp1331; _temp1332.base=
_temp1331; _temp1332.last_plus_one= _temp1331 + 42; _temp1332;})); goto _LL1306;
_LL1306:;}{ void* old_elt_typ=( void*)( _temp1004->topt)->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1006);{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp(
x1,({ struct Cyc_Absyn_Local_b_struct* _temp1333=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1333->tag= Cyc_Absyn_Local_b_tag;
_temp1333->f1= _temp1008;( void*) _temp1333;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1004);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1004, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1334=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1334->hd=( void*)({ struct _tuple11* _temp1335=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1335->f1= max;
_temp1335->f2= Cyc_Absyn_uint_t; _temp1335->f3=( struct Cyc_Absyn_Exp*)
_temp1006; _temp1335;}); _temp1334->tl= 0; _temp1334;}); struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeof_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0)), 0); decls=({
struct Cyc_List_List* _temp1336=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1336->hd=( void*)({ struct _tuple11* _temp1337=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1337->f1= a;
_temp1337->f2= ptr_typ; _temp1337->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1337;});
_temp1336->tl= decls; _temp1336;}); decls=({ struct Cyc_List_List* _temp1338=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1338->hd=(
void*)({ struct _tuple11* _temp1339=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp1339->f1= x1; _temp1339->f2= Cyc_Absyn_uint_t; _temp1339->f3=
0; _temp1339;}); _temp1338->tl= decls; _temp1338;}); if( is_tagged_ptr){ struct
_tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp*
binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1340=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1340->hd=(
void*)({ struct _tuple8* _temp1345=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1345->f1= 0; _temp1345->f2= Cyc_Absyn_var_exp( a, 0); _temp1345;});
_temp1340->tl=({ struct Cyc_List_List* _temp1341=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1341->hd=( void*)({ struct
_tuple8* _temp1344=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1344->f1= 0; _temp1344->f2= Cyc_Absyn_var_exp( a, 0); _temp1344;});
_temp1341->tl=({ struct Cyc_List_List* _temp1342=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1342->hd=( void*)({ struct
_tuple8* _temp1343=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1343->f1= 0; _temp1343->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1343;}); _temp1342->tl= 0; _temp1342;});
_temp1341;}); _temp1340;}), 0); decls=({ struct Cyc_List_List* _temp1346=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1346->hd=(
void*)({ struct _tuple11* _temp1347=( struct _tuple11*) GC_malloc( sizeof(
struct _tuple11)); _temp1347->f1= b; _temp1347->f2= t; _temp1347->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1347;}); _temp1346->tl= decls; _temp1346;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Exp* _temp1350; void*
_temp1352; struct _tuple1* _temp1354; struct _tuple11 _temp1348=*(( struct
_tuple11*) ds->hd); _LL1355: _temp1354= _temp1348.f1; goto _LL1353; _LL1353:
_temp1352= _temp1348.f2; goto _LL1351; _LL1351: _temp1350= _temp1348.f3; goto
_LL1349; _LL1349: s= Cyc_Absyn_declare_stmt( _temp1354, _temp1352, _temp1350, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r)); goto
_LL846;}}}} _LL900: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, _temp1012, _temp1016))->r));} else{ struct _tuple1* n= _temp1016;
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp1357= _temp1010 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1356= Cyc_Absynpp_qvar2string(
_temp1016); xprintf("no structdecl %.*s", _temp1356.last_plus_one - _temp1356.curr,
_temp1356.curr);})): _temp1010; if( _temp1357 == 0){ _throw( Null_Exception);}
_temp1357;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1012,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields= fields->tl){ struct _tuple13 _temp1360;
struct Cyc_Absyn_Exp* _temp1361; struct _tuple4* _temp1363; struct _tuple4
_temp1365; void* _temp1366; struct Cyc_Absyn_Tqual* _temp1368; struct
_tagged_string* _temp1370; struct _tuple13* _temp1358=( struct _tuple13*) fields->hd;
_temp1360=* _temp1358; _LL1364: _temp1363= _temp1360.f1; _temp1365=* _temp1363;
_LL1371: _temp1370= _temp1365.f1; goto _LL1369; _LL1369: _temp1368= _temp1365.f2;
goto _LL1367; _LL1367: _temp1366= _temp1365.f3; goto _LL1362; _LL1362: _temp1361=
_temp1360.f2; goto _LL1359; _LL1359: Cyc_Toc_exp_to_c( nv, _temp1361); if( Cyc_Toc_is_void_star(
_temp1366)){( void*)( _temp1361->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1361->r, 0), 0))->r));}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1012, 0), 0))->r));} goto _LL846; _LL902: { struct _tuple1* qv= _temp1018->name;
if( _temp1022 == 0){ if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp(
qv, 0))->r));} else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1020->fields
== 0? 0:( struct Cyc_List_List*)( _temp1020->fields)->v; while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs == 0){ tag_count ++;} fields= fields->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0), 0))->r));}} else{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1372=( char*)"_struct";
struct _tagged_string _temp1373; _temp1373.curr= _temp1372; _temp1373.base=
_temp1372; _temp1373.last_plus_one= _temp1372 + 8; _temp1373;}))); void*
pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ, Cyc_Toc_mt_tq); if( nv->toplevel){
int tag_count= 0; struct Cyc_List_List* fields= _temp1020->fields == 0? 0:(
struct Cyc_List_List*)( _temp1020->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,((
struct Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs != 0){ tag_count ++;} fields= fields->tl;}{ struct Cyc_Absyn_Exp*
tag_exp= Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); struct Cyc_List_List*
dles=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Toc_add_designator, nv, _temp1022); dles=({ struct Cyc_List_List* _temp1374=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1374->hd=(
void*)({ struct _tuple8* _temp1375=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1375->f1= 0; _temp1375->f2= tag_exp; _temp1375;}); _temp1374->tl=
dles; _temp1374;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1376=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1376->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1377=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1377->tag= Cyc_Absyn_Var_d_tag; _temp1377->f1= vd;( void*) _temp1377;}), 0);
_temp1376->tl= Cyc_Toc_temp_topdecls; _temp1376;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( pstrct_typ, Cyc_Absyn_address_exp( xexp, 0), 0))->r));}}}
else{ struct Cyc_List_List* typs= _temp1018->typs; struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeof_exp(
strct_typ, 0); int atomic= 1; struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1378=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1378->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1379=( char*)"_tag"; struct _tagged_string
_temp1380; _temp1380.curr= _temp1379; _temp1380.base= _temp1379; _temp1380.last_plus_one=
_temp1379 + 5; _temp1380;})), 0), 0); _temp1378->tl= 0; _temp1378;});{ int i= 1;
for( 0; _temp1022 != 0;( _temp1022= _temp1022->tl, i ++), typs= typs->tl){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1022->hd; void*
field_t= Cyc_Toc_typ_to_c((*(( struct _tuple6*) typs->hd)).f2); if( ! Cyc_Toc_atomic_typ((
void*)( cur_e->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_t)){ cur_e= Cyc_Absyn_cast_exp( field_t, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp( xexp,({ struct
_tagged_string* _temp1381=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1381[ 0]= xprintf("f%d", i); _temp1381;}), 0), cur_e,
0); rev_assign=({ struct Cyc_List_List* _temp1382=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1382->hd=( void*) a; _temp1382->tl=(
struct Cyc_List_List*) rev_assign; _temp1382;});}}}{ struct Cyc_Absyn_Exp* e1=
Cyc_Absyn_cast_exp( pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr(
e2), 0); struct Cyc_Absyn_Stmt* final_cast= Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c( old_typ), xexp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(({ struct Cyc_List_List*
_temp1383=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1383->hd=( void*) final_cast; _temp1383->tl=( struct Cyc_List_List*)
rev_assign; _temp1383;})), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}}}
goto _LL846;} _LL904: { struct _tuple1* qv= _temp1028->name; struct _tuple1* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1384=( char*)"_struct"; struct _tagged_string
_temp1385; _temp1385.curr= _temp1384; _temp1385.base= _temp1384; _temp1385.last_plus_one=
_temp1384 + 8; _temp1385;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1386=( char*)"_tag"; struct
_tagged_string _temp1387; _temp1387.curr= _temp1386; _temp1387.base= _temp1386;
_temp1387.last_plus_one= _temp1386 + 5; _temp1387;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple8*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1032); dles=({ struct Cyc_List_List*
_temp1388=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1388->hd=( void*)({ struct _tuple8* _temp1389=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1389->f1= 0; _temp1389->f2= tag_exp; _temp1389;});
_temp1388->tl= dles; _temp1388;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1390=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1390->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1391=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1391->tag= Cyc_Absyn_Var_d_tag; _temp1391->f1= vd;( void*) _temp1391;}), 0);
_temp1390->tl= Cyc_Toc_temp_topdecls; _temp1390;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), Cyc_Absyn_address_exp(
xexp, 0), 0))->r));}} else{ int atomic= 1; struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeof_exp(
strct_typ, 0); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1032 != 0;
_temp1032= _temp1032->tl, i ++){ if( ! Cyc_Toc_atomic_typ(( void*)((( struct Cyc_Absyn_Exp*)
_temp1032->hd)->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_temp1032->hd);{ struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1392=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1392->hd=(
void*)({ struct Cyc_Absyn_FieldName_struct* _temp1393=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1393->tag= Cyc_Absyn_FieldName_tag;
_temp1393->f1=({ struct _tagged_string* _temp1394=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1394[ 0]= xprintf("f%d", i);
_temp1394;});( void*) _temp1393;}); _temp1392->tl= 0; _temp1392;}); dles=({
struct Cyc_List_List* _temp1395=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1395->hd=( void*)({ struct _tuple8* _temp1396=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1396->f1=( struct Cyc_List_List*)
des; _temp1396->f2=( struct Cyc_Absyn_Exp*) _temp1032->hd; _temp1396;});
_temp1395->tl= dles; _temp1395;});}}} dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( dles);{ struct Cyc_Absyn_Exp* e1= Cyc_Absyn_cast_exp(
pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr( e2), 0);
struct Cyc_List_List* d=({ struct Cyc_List_List* _temp1397=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1397->hd=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp1398=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1398->tag= Cyc_Absyn_FieldName_tag; _temp1398->f1= Cyc_Toc_tag_sp;( void*)
_temp1398;}); _temp1397->tl= 0; _temp1397;}); struct Cyc_List_List* dles2=({
struct Cyc_List_List* _temp1399=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1399->hd=( void*)({ struct _tuple8* _temp1400=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1400->f1=( struct Cyc_List_List*)
d; _temp1400->f2= tag_exp; _temp1400;}); _temp1399->tl= dles; _temp1399;});
struct Cyc_Absyn_Exp* strct_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles2, 0);
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_deref_exp( xexp, 0),
Cyc_Absyn_cast_exp( strct_typ, strct_exp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s1, Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), xexp, 0),
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}} goto _LL846;}
_LL906: Cyc_Toc_stmt_to_c( nv, _temp1036); goto _LL846; _LL908:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1401=( char*)"UnresolvedMem"; struct _tagged_string _temp1402; _temp1402.curr=
_temp1401; _temp1402.base= _temp1401; _temp1402.last_plus_one= _temp1401 + 14;
_temp1402;})); goto _LL846; _LL910:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1403=( char*)"codegen"; struct
_tagged_string _temp1404; _temp1404.curr= _temp1403; _temp1404.base= _temp1403;
_temp1404.last_plus_one= _temp1403 + 8; _temp1404;})); goto _LL846; _LL912:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1405=( char*)"fill"; struct _tagged_string _temp1406; _temp1406.curr=
_temp1405; _temp1406.base= _temp1405; _temp1406.last_plus_one= _temp1405 + 5;
_temp1406;})); goto _LL846; _LL846:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple14 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1407=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1439; struct Cyc_Absyn_Vardecl _temp1441; struct Cyc_Core_Opt* _temp1442;
int _temp1444; struct Cyc_Absyn_Exp* _temp1446; void* _temp1448; struct Cyc_Absyn_Tqual*
_temp1450; struct _tuple1* _temp1452; void* _temp1454; struct Cyc_Absyn_Vardecl*
_temp1456; int _temp1458; void* _temp1460; char _temp1462; struct _tagged_string
_temp1464; struct Cyc_Absyn_Enumfield* _temp1466; struct Cyc_Absyn_Enumdecl*
_temp1468; struct Cyc_List_List* _temp1470; struct Cyc_List_List* _temp1472;
struct Cyc_Core_Opt* _temp1474; struct _tuple1* _temp1476; struct Cyc_Absyn_Enumfield*
_temp1478; struct Cyc_Absyn_Xenumdecl* _temp1480; struct Cyc_List_List*
_temp1482; struct Cyc_List_List* _temp1484; struct _tuple1* _temp1486; struct
Cyc_List_List* _temp1488; struct Cyc_List_List* _temp1490; struct Cyc_List_List*
_temp1492; struct Cyc_Core_Opt* _temp1494; struct Cyc_Absyn_Structdecl*
_temp1496; struct Cyc_Absyn_Pat* _temp1498; struct _tuple1* _temp1500; struct
Cyc_List_List* _temp1502; struct Cyc_List_List* _temp1504; struct _tuple1*
_temp1506; struct Cyc_List_List* _temp1508; struct Cyc_List_List* _temp1510;
struct _tuple1* _temp1512; _LL1409: if( _temp1407 == Cyc_Absyn_Wild_p){ goto
_LL1410;} else{ goto _LL1411;} _LL1411: if(( unsigned int) _temp1407 > 2u?((
struct _enum_struct*) _temp1407)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1440:
_temp1439=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1407)->f1; _temp1441=* _temp1439; _LL1455: _temp1454=( void*) _temp1441.sc;
goto _LL1453; _LL1453: _temp1452=( struct _tuple1*) _temp1441.name; goto _LL1451;
_LL1451: _temp1450=( struct Cyc_Absyn_Tqual*) _temp1441.tq; goto _LL1449;
_LL1449: _temp1448=( void*) _temp1441.type; goto _LL1447; _LL1447: _temp1446=(
struct Cyc_Absyn_Exp*) _temp1441.initializer; goto _LL1445; _LL1445: _temp1444=(
int) _temp1441.shadow; goto _LL1443; _LL1443: _temp1442=( struct Cyc_Core_Opt*)
_temp1441.region; goto _LL1412;} else{ goto _LL1413;} _LL1413: if(( unsigned int)
_temp1407 > 2u?(( struct _enum_struct*) _temp1407)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1457: _temp1456=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1407)->f1; goto _LL1414;} else{ goto _LL1415;} _LL1415: if( _temp1407 ==
Cyc_Absyn_Null_p){ goto _LL1416;} else{ goto _LL1417;} _LL1417: if((
unsigned int) _temp1407 > 2u?(( struct _enum_struct*) _temp1407)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1461: _temp1460=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1407)->f1;
goto _LL1459; _LL1459: _temp1458=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1407)->f2; goto _LL1418;} else{ goto _LL1419;} _LL1419: if(( unsigned int)
_temp1407 > 2u?(( struct _enum_struct*) _temp1407)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1463: _temp1462=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1407)->f1;
goto _LL1420;} else{ goto _LL1421;} _LL1421: if(( unsigned int) _temp1407 > 2u?((
struct _enum_struct*) _temp1407)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1465:
_temp1464=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1407)->f1;
goto _LL1422;} else{ goto _LL1423;} _LL1423: if(( unsigned int) _temp1407 > 2u?((
struct _enum_struct*) _temp1407)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1477:
_temp1476=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1407)->f1;
goto _LL1475; _LL1475: _temp1474=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1407)->f2; goto _LL1473; _LL1473: _temp1472=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1407)->f3; goto _LL1471; _LL1471:
_temp1470=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1407)->f4;
goto _LL1469; _LL1469: _temp1468=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1407)->f5; goto _LL1467; _LL1467: _temp1466=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1407)->f6; goto _LL1424;} else{ goto
_LL1425;} _LL1425: if(( unsigned int) _temp1407 > 2u?(( struct _enum_struct*)
_temp1407)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1487: _temp1486=( struct
_tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1407)->f1; goto _LL1485;
_LL1485: _temp1484=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1407)->f2; goto _LL1483; _LL1483: _temp1482=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1407)->f3; goto _LL1481; _LL1481:
_temp1480=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1407)->f4; goto _LL1479; _LL1479: _temp1478=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1407)->f5; goto _LL1426;} else{ goto
_LL1427;} _LL1427: if(( unsigned int) _temp1407 > 2u?(( struct _enum_struct*)
_temp1407)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1489: _temp1488=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp1407)->f1; goto _LL1428;} else{ goto
_LL1429;} _LL1429: if(( unsigned int) _temp1407 > 2u?(( struct _enum_struct*)
_temp1407)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL1497: _temp1496=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1407)->f1; goto _LL1495; _LL1495:
_temp1494=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1407)->f2;
goto _LL1493; _LL1493: _temp1492=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1407)->f3; goto _LL1491; _LL1491: _temp1490=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1407)->f4; goto _LL1430;} else{ goto
_LL1431;} _LL1431: if(( unsigned int) _temp1407 > 2u?(( struct _enum_struct*)
_temp1407)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1499: _temp1498=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1407)->f1; goto _LL1432;} else{ goto
_LL1433;} _LL1433: if(( unsigned int) _temp1407 > 2u?(( struct _enum_struct*)
_temp1407)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1501: _temp1500=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1407)->f1; goto _LL1434;}
else{ goto _LL1435;} _LL1435: if(( unsigned int) _temp1407 > 2u?(( struct
_enum_struct*) _temp1407)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1507:
_temp1506=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1407)->f1;
goto _LL1505; _LL1505: _temp1504=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1407)->f2; goto _LL1503; _LL1503: _temp1502=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1407)->f3; goto _LL1436;} else{
goto _LL1437;} _LL1437: if(( unsigned int) _temp1407 > 2u?(( struct _enum_struct*)
_temp1407)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1513: _temp1512=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1407)->f1;
goto _LL1511; _LL1511: _temp1510=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1407)->f2; goto _LL1509; _LL1509: _temp1508=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1407)->f3; goto _LL1438;} else{
goto _LL1408;} _LL1410: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1408;
_LL1412: nv= Cyc_Toc_add_varmap( nv, _temp1452, r); s= Cyc_Absyn_goto_stmt(
succ_lab, 0); goto _LL1408; _LL1414: { struct _tuple1* v= Cyc_Toc_temp_var();
decls=({ struct Cyc_List_List* _temp1514=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1514->hd=( void*)({ struct _tuple15*
_temp1515=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1515->f1=
v; _temp1515->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1515;}); _temp1514->tl= decls; _temp1514;}); nv= Cyc_Toc_add_varmap( nv,
_temp1456->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1408;} _LL1416: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1408; _LL1418: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1460, _temp1458, 0), succ_lab, fail_lab); goto _LL1408;
_LL1420: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1462, 0),
succ_lab, fail_lab); goto _LL1408; _LL1422: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1464, 0), succ_lab, fail_lab); goto _LL1408; _LL1424: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1470);
struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1466->name,( struct
_tagged_string)({ char* _temp1516=( char*)"_struct"; struct _tagged_string
_temp1517; _temp1517.curr= _temp1516; _temp1517.base= _temp1516; _temp1517.last_plus_one=
_temp1516 + 8; _temp1517;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1470); for( 0; rps != 0; rps= rps->tl, cnt --){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=(
void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string*
lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp1518=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1518->hd=( void*)({
struct _tuple15* _temp1519=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp1519->f1= v; _temp1519->f2= t2c; _temp1519;}); _temp1518->tl=
decls; _temp1518;});{ struct _tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1520=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1520[ 0]= xprintf("f%d", cnt); _temp1520;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp(
t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1521=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1521[ 0]= xprintf("f%d", cnt); _temp1521;}), 0), 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List* _temp1522=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1522->hd=(
void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1522->tl= ss; _temp1522;});
succ_lab= lab;}}}} if( _temp1470 == 0){ s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_var_exp(
_temp1476, 0), succ_lab, fail_lab);} else{ unsigned int max_tag= 0;{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)( _temp1468->fields)->v; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->typs
== 0){ if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1523=( char*)"can't deal with explicit tags yet";
struct _tagged_string _temp1524; _temp1524.curr= _temp1523; _temp1524.base=
_temp1523; _temp1524.last_plus_one= _temp1523 + 34; _temp1524;}));} max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__enum_struct_sp),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp( e3,
Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1476,( struct _tagged_string)({ char* _temp1525=(
char*)"_tag"; struct _tagged_string _temp1526; _temp1526.curr= _temp1525;
_temp1526.base= _temp1525; _temp1526.last_plus_one= _temp1525 + 5; _temp1526;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag >( unsigned int) 0? Cyc_Absyn_and_exp(
e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}}
goto _LL1408;} _LL1426: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1482); struct _tuple1* xfstrct= Cyc_Toc_collapse_qvar_tag(
_temp1478->name,( struct _tagged_string)({ char* _temp1527=( char*)"_struct";
struct _tagged_string _temp1528; _temp1528.curr= _temp1527; _temp1528.base=
_temp1527; _temp1528.last_plus_one= _temp1527 + 8; _temp1528;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1482); for( 0; rps != 0; rps= rps->tl, cnt --){ struct
Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1529=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1529->hd=( void*)({ struct _tuple15*
_temp1530=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1530->f1=
v; _temp1530->f2= Cyc_Toc_typ_to_c( t2); _temp1530;}); _temp1529->tl= decls;
_temp1529;});{ struct _tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1531=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1531[ 0]= xprintf("f%d", cnt); _temp1531;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1532=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1532[ 0]= xprintf("f%d", cnt);
_temp1532;}), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0);
ss=({ struct Cyc_List_List* _temp1533=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1533->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1533->tl= ss; _temp1533;}); succ_lab= lab;}}}} if( _temp1482 == 0){
s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0),
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1486,( struct _tagged_string)({
char* _temp1534=( char*)"_tag"; struct _tagged_string _temp1535; _temp1535.curr=
_temp1534; _temp1535.base= _temp1534; _temp1535.last_plus_one= _temp1534 + 5;
_temp1535;})), 0), succ_lab, fail_lab);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structmember_exp( e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e=
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1486,( struct _tagged_string)({
char* _temp1536=( char*)"_tag"; struct _tagged_string _temp1537; _temp1537.curr=
_temp1536; _temp1537.base= _temp1536; _temp1537.last_plus_one= _temp1536 + 5;
_temp1537;})), 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0),
Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} goto
_LL1408;} _LL1428: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1488);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1488); for( 0; rps != 0; rps= rps->tl,
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct
_tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1538=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1538->hd=( void*)({ struct _tuple15* _temp1539=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1539->f1= v; _temp1539->f2= Cyc_Toc_typ_to_c(
t2); _temp1539;}); _temp1538->tl= decls; _temp1538;});{ struct _tuple14 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1540=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1540[ 0]= xprintf("f%d", cnt);
_temp1540;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1541=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1541[ 0]= xprintf("f%d", cnt); _temp1541;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1542=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1542->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1542->tl= ss; _temp1542;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1408;} _LL1430: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1490); for( 0; dlps != 0; dlps= dlps->tl){ struct _tuple16* tup=( struct
_tuple16*) dlps->hd; struct Cyc_Absyn_Pat* p2=(* tup).f2; struct _tagged_string*
f;{ void* _temp1543=( void*)((* tup).f1)->hd; struct _tagged_string* _temp1549;
_LL1545: if((( struct _enum_struct*) _temp1543)->tag == Cyc_Absyn_FieldName_tag){
_LL1550: _temp1549=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1543)->f1; goto _LL1546;} else{ goto _LL1547;} _LL1547: goto _LL1548;
_LL1546: f= _temp1549; goto _LL1544; _LL1548:( void) _throw(({ struct Cyc_Toc_Match_error_struct*
_temp1551=( struct Cyc_Toc_Match_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Match_error_struct));* _temp1551=( struct Cyc_Toc_Match_error_struct){.tag=
Cyc_Toc_Match_error_tag};( struct _xenum_struct*) _temp1551;})); _LL1544:;}{
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; void*
t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1552=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1552->hd=( void*)({ struct _tuple15*
_temp1553=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1553->f1=
v; _temp1553->f2= t2c; _temp1553;}); _temp1552->tl= decls; _temp1552;});{ struct
_tuple14 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path, f, 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1554=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1554->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1554->tl= ss; _temp1554;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1408;} _LL1432: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( _temp1498->topt)->v; decls=({ struct Cyc_List_List* _temp1555=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1555->hd=(
void*)({ struct _tuple15* _temp1556=( struct _tuple15*) GC_malloc( sizeof(
struct _tuple15)); _temp1556->f1= v; _temp1556->f2= Cyc_Toc_typ_to_c( t2);
_temp1556;}); _temp1555->tl= decls; _temp1555;});{ struct _tuple14 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path, 0), _temp1498,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0), s2, 0); if( Cyc_Toc_really_is_nullable(
t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);} else{ s= s3;} goto
_LL1408;}}} _LL1434: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1557=( char*)"unknownid";
struct _tagged_string _temp1558; _temp1558.curr= _temp1557; _temp1558.base=
_temp1557; _temp1558.last_plus_one= _temp1557 + 10; _temp1558;})); goto _LL1408;
_LL1436: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1559=( char*)"unknowncall"; struct
_tagged_string _temp1560; _temp1560.curr= _temp1559; _temp1560.base= _temp1559;
_temp1560.last_plus_one= _temp1559 + 12; _temp1560;})); goto _LL1408; _LL1438: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1561=( char*)"unknownfields"; struct _tagged_string
_temp1562; _temp1562.curr= _temp1561; _temp1562.base= _temp1561; _temp1562.last_plus_one=
_temp1561 + 14; _temp1562;})); goto _LL1408; _LL1408:;} return({ struct _tuple14
_temp1563; _temp1563.f1= nv; _temp1563.f2= decls; _temp1563.f3= s; _temp1563;});}
static struct _tuple17* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){
return({ struct _tuple17* _temp1564=( struct _tuple17*) GC_malloc( sizeof(
struct _tuple17)); _temp1564->f1= Cyc_Toc_fresh_label(); _temp1564->f2= Cyc_Toc_fresh_label();
_temp1564->f3= sc; _temp1564;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)( e->topt)->v; int
leave_as_switch;{ void* _temp1565= Cyc_Tcutil_compress( t); void* _temp1571;
void* _temp1573; _LL1567: if(( unsigned int) _temp1565 > 5u?(( struct
_enum_struct*) _temp1565)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1574: _temp1573=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1565)->f1; goto _LL1572; _LL1572:
_temp1571=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1565)->f2; goto
_LL1568;} else{ goto _LL1569;} _LL1569: goto _LL1570; _LL1568: leave_as_switch=
1; goto _LL1566; _LL1570: leave_as_switch= 0; goto _LL1566; _LL1566:;}{ struct
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
succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1575->hd=( void*) res.f1;
_temp1575->tl= nvs; _temp1575;}); decls= res.f2; test_stmts=({ struct Cyc_List_List*
_temp1576=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1576->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple17*) lscs2->hd)).f1,
res.f3, 0); _temp1576->tl= test_stmts; _temp1576;});} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1577= sc->where_clause;
if( _temp1577 == 0){ _throw( Null_Exception);} _temp1577;}); struct
_tagged_string* intermed_lab= Cyc_Toc_fresh_label(); struct _tuple14 res= Cyc_Toc_xlate_pat(
nv, t, r, path, sc->pattern, intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c(
res.f1, e); nvs=({ struct Cyc_List_List* _temp1578=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1578->hd=( void*) res.f1;
_temp1578->tl= nvs; _temp1578;}); decls= res.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt(
e, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( intermed_lab, s2, 0);
test_stmts=({ struct Cyc_List_List* _temp1579=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1579->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple17*) lscs2->hd)).f1, Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0);
_temp1579->tl= test_stmts; _temp1579;});}}}}{ struct Cyc_Absyn_Stmt* test_stmt=
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
decls; decls != 0; decls= decls->tl){ struct _tuple15 _temp1582; void* _temp1583;
struct _tuple1* _temp1585; struct _tuple15* _temp1580=( struct _tuple15*) decls->hd;
_temp1582=* _temp1580; _LL1586: _temp1585= _temp1582.f1; goto _LL1584; _LL1584:
_temp1583= _temp1582.f2; goto _LL1581; _LL1581: res= Cyc_Absyn_declare_stmt(
_temp1585, _temp1583, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)( e->topt)->v),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple18 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1587=(
void*) s->r; struct Cyc_Absyn_Exp* _temp1625; struct Cyc_Absyn_Stmt* _temp1627;
struct Cyc_Absyn_Stmt* _temp1629; struct Cyc_Absyn_Exp* _temp1631; struct Cyc_Absyn_Stmt*
_temp1633; struct Cyc_Absyn_Stmt* _temp1635; struct Cyc_Absyn_Exp* _temp1637;
struct Cyc_Absyn_Stmt* _temp1639; struct _tuple3 _temp1641; struct Cyc_Absyn_Stmt*
_temp1643; struct Cyc_Absyn_Exp* _temp1645; struct Cyc_Absyn_Stmt* _temp1647;
struct Cyc_Absyn_Stmt* _temp1649; struct Cyc_Absyn_Stmt* _temp1651; struct
_tagged_string* _temp1653; struct Cyc_Absyn_Stmt* _temp1655; struct _tuple3
_temp1657; struct Cyc_Absyn_Stmt* _temp1659; struct Cyc_Absyn_Exp* _temp1661;
struct _tuple3 _temp1663; struct Cyc_Absyn_Stmt* _temp1665; struct Cyc_Absyn_Exp*
_temp1667; struct Cyc_Absyn_Exp* _temp1669; struct Cyc_List_List* _temp1671;
struct Cyc_Absyn_Exp* _temp1673; struct Cyc_Absyn_Switch_clause** _temp1675;
struct Cyc_List_List* _temp1677; struct Cyc_Absyn_Stmt* _temp1679; struct Cyc_Absyn_Decl*
_temp1681; struct Cyc_Absyn_Stmt* _temp1683; struct Cyc_Absyn_Stmt* _temp1685;
struct Cyc_Absyn_Stmt* _temp1687; struct _tagged_string* _temp1689; struct
_tuple3 _temp1691; struct Cyc_Absyn_Stmt* _temp1693; struct Cyc_Absyn_Exp*
_temp1695; struct Cyc_Absyn_Stmt* _temp1697; struct Cyc_List_List* _temp1699;
struct Cyc_Absyn_Stmt* _temp1701; _LL1589: if( _temp1587 == Cyc_Absyn_Skip_s){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if(( unsigned int) _temp1587 > 1u?((
struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1626:
_temp1625=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1587)->f1;
goto _LL1592;} else{ goto _LL1593;} _LL1593: if(( unsigned int) _temp1587 > 1u?((
struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1630:
_temp1629=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1587)->f1;
goto _LL1628; _LL1628: _temp1627=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1587)->f2; goto _LL1594;} else{ goto _LL1595;} _LL1595: if(( unsigned int)
_temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1632: _temp1631=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1587)->f1; goto _LL1596;} else{ goto _LL1597;} _LL1597: if(( unsigned int)
_temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1638: _temp1637=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1587)->f1; goto _LL1636; _LL1636: _temp1635=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1587)->f2; goto _LL1634; _LL1634:
_temp1633=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1587)->f3; goto _LL1598;} else{ goto _LL1599;} _LL1599: if(( unsigned int)
_temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1642: _temp1641=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1587)->f1; _LL1646: _temp1645= _temp1641.f1; goto _LL1644; _LL1644:
_temp1643= _temp1641.f2; goto _LL1640; _LL1640: _temp1639=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1587)->f2; goto _LL1600;} else{ goto
_LL1601;} _LL1601: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1648: _temp1647=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1587)->f1; goto _LL1602;} else{ goto
_LL1603;} _LL1603: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1650: _temp1649=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1587)->f1; goto _LL1604;} else{ goto
_LL1605;} _LL1605: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1654: _temp1653=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1587)->f1; goto _LL1652;
_LL1652: _temp1651=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1587)->f2; goto _LL1606;} else{ goto _LL1607;} _LL1607: if(( unsigned int)
_temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1670: _temp1669=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1587)->f1; goto _LL1664; _LL1664: _temp1663=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1587)->f2; _LL1668: _temp1667= _temp1663.f1; goto _LL1666; _LL1666:
_temp1665= _temp1663.f2; goto _LL1658; _LL1658: _temp1657=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1587)->f3; _LL1662: _temp1661= _temp1657.f1;
goto _LL1660; _LL1660: _temp1659= _temp1657.f2; goto _LL1656; _LL1656: _temp1655=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1587)->f4; goto
_LL1608;} else{ goto _LL1609;} _LL1609: if(( unsigned int) _temp1587 > 1u?((
struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1674:
_temp1673=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1587)->f1; goto _LL1672; _LL1672: _temp1671=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1587)->f2; goto _LL1610;} else{ goto
_LL1611;} _LL1611: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1678: _temp1677=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1587)->f1; goto _LL1676; _LL1676:
_temp1675=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1587)->f2; goto _LL1612;} else{ goto _LL1613;} _LL1613: if(( unsigned int)
_temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1682: _temp1681=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1587)->f1; goto _LL1680; _LL1680: _temp1679=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1587)->f2; goto _LL1614;} else{ goto
_LL1615;} _LL1615: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1684: _temp1683=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1587)->f1; goto _LL1616;} else{ goto
_LL1617;} _LL1617: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1686: _temp1685=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1587)->f1; goto _LL1618;} else{ goto
_LL1619;} _LL1619: if(( unsigned int) _temp1587 > 1u?(( struct _enum_struct*)
_temp1587)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1690: _temp1689=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1587)->f1; goto
_LL1688; _LL1688: _temp1687=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1587)->f2; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int)
_temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1698: _temp1697=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1587)->f1; goto _LL1692; _LL1692: _temp1691=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1587)->f2; _LL1696: _temp1695= _temp1691.f1; goto _LL1694; _LL1694:
_temp1693= _temp1691.f2; goto _LL1622;} else{ goto _LL1623;} _LL1623: if((
unsigned int) _temp1587 > 1u?(( struct _enum_struct*) _temp1587)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1702: _temp1701=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1587)->f1; goto _LL1700; _LL1700: _temp1699=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1587)->f2; goto _LL1624;} else{ goto
_LL1588;} _LL1590: return; _LL1592: Cyc_Toc_exp_to_c( nv, _temp1625); return;
_LL1594: Cyc_Toc_stmt_to_c( nv, _temp1629); s= _temp1627; continue; _LL1596: {
struct Cyc_Core_Opt* topt= 0; if( _temp1631 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1703=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1703->v=( void*) Cyc_Toc_typ_to_c(( void*)( _temp1631->topt)->v); _temp1703;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1704=
_temp1631; if( _temp1704 == 0){ _throw( Null_Exception);} _temp1704;}));} if( nv->to_pop_return
!= 0){ struct Cyc_Absyn_Exp* call_exp= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1705=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1705->f1= Cyc_Toc_abs_ns; _temp1705->f2= Cyc_Toc__npop_handler_sp;
_temp1705;}), 0),({ struct Cyc_List_List* _temp1706=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1706->hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( nv->to_pop_return - 1), 0); _temp1706->tl= 0; _temp1706;}), 0);
struct Cyc_Absyn_Stmt* call_stmt= Cyc_Absyn_exp_stmt( call_exp, 0); if( topt !=
0){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*) topt->v, _temp1631, Cyc_Absyn_seq_stmt(
call_stmt, retn_stmt, 0), 0))->r));} else{( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
call_stmt, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}} return;} _LL1598:
Cyc_Toc_exp_to_c( nv, _temp1637); Cyc_Toc_stmt_to_c( nv, _temp1635); s=
_temp1633; continue; _LL1600: Cyc_Toc_exp_to_c( nv, _temp1645); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1639); return; _LL1602: if( nv->break_lab != 0){(
void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)(
nv->break_lab)->v, 0))->r));} if( nv->to_pop_break != 0){ struct Cyc_Absyn_Exp*
call_exp= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct _tuple1* _temp1707=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1707->f1= Cyc_Toc_abs_ns;
_temp1707->f2= Cyc_Toc__npop_handler_sp; _temp1707;}), 0),({ struct Cyc_List_List*
_temp1708=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1708->hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( nv->to_pop_break - 1),
0); _temp1708->tl= 0; _temp1708;}), 0); struct Cyc_Absyn_Stmt* call_stmt= Cyc_Absyn_exp_stmt(
call_exp, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( call_stmt,
Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));} return; _LL1604: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)( nv->continue_lab)->v, 0))->r));} if( nv->to_pop_continue != 0){
struct Cyc_Absyn_Exp* call_exp= Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1709=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1709->f1= Cyc_Toc_abs_ns; _temp1709->f2= Cyc_Toc__npop_handler_sp;
_temp1709;}), 0),({ struct Cyc_List_List* _temp1710=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1710->hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( nv->to_pop_continue - 1), 0); _temp1710->tl= 0; _temp1710;}), 0);
struct Cyc_Absyn_Stmt* call_stmt= Cyc_Absyn_exp_stmt( call_exp, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_seq_stmt( call_stmt, Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));} return; _LL1606: return; _LL1608: Cyc_Toc_exp_to_c( nv, _temp1669);
Cyc_Toc_exp_to_c( nv, _temp1667); Cyc_Toc_exp_to_c( nv, _temp1661); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1655); return; _LL1610: Cyc_Toc_xlate_switch( nv, s,
_temp1673, _temp1671); return; _LL1612: if( nv->fallthru_info == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1711=( char*)"fallthru in unexpected place"; struct _tagged_string
_temp1712; _temp1712.curr= _temp1711; _temp1712.base= _temp1711; _temp1712.last_plus_one=
_temp1711 + 29; _temp1712;})); return;}{ struct _tuple9 _temp1715; struct Cyc_Dict_Dict*
_temp1716; struct Cyc_List_List* _temp1718; struct _tagged_string* _temp1720;
struct _tuple9* _temp1713=( struct _tuple9*)( nv->fallthru_info)->v; _temp1715=*
_temp1713; _LL1721: _temp1720= _temp1715.f1; goto _LL1719; _LL1719: _temp1718=
_temp1715.f2; goto _LL1717; _LL1717: _temp1716= _temp1715.f3; goto _LL1714;
_LL1714: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1720, 0); struct
Cyc_List_List* vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1718); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1677); for( 0; vs2 != 0; vs2= vs2->tl, es2= es2->tl){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) es2->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple1* key)) Cyc_Dict_lookup)( _temp1716,( struct _tuple1*) vs2->hd),(
struct Cyc_Absyn_Exp*) es2->hd, 0), s2, 0);}( void*)( s->r=( void*)(( void*) s2->r));
if( nv->to_pop_fallthru != 0){ struct Cyc_Absyn_Exp* call_exp= Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(({ struct _tuple1* _temp1722=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp1722->f1= Cyc_Toc_abs_ns; _temp1722->f2= Cyc_Toc__npop_handler_sp;
_temp1722;}), 0),({ struct Cyc_List_List* _temp1723=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1723->hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( nv->to_pop_fallthru - 1), 0); _temp1723->tl= 0; _temp1723;}), 0);
struct Cyc_Absyn_Stmt* call_stmt= Cyc_Absyn_exp_stmt( call_exp, 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_seq_stmt( call_stmt, Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));} return;}} _LL1614:{ void* _temp1724=( void*) _temp1681->r; int
_temp1730; struct Cyc_Absyn_Exp* _temp1732; struct Cyc_Core_Opt* _temp1734;
struct Cyc_Core_Opt* _temp1736; struct Cyc_Absyn_Pat* _temp1738; _LL1726: if((
unsigned int) _temp1724 > 1u?(( struct _enum_struct*) _temp1724)->tag == Cyc_Absyn_Let_d_tag:
0){ _LL1739: _temp1738=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1724)->f1; goto _LL1737; _LL1737: _temp1736=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1724)->f2; goto _LL1735; _LL1735: _temp1734=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1724)->f3; goto
_LL1733; _LL1733: _temp1732=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1724)->f4; goto _LL1731; _LL1731: _temp1730=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1724)->f5; goto _LL1727;} else{ goto _LL1728;} _LL1728: goto _LL1729;
_LL1727: { int var_case= 0;{ void* _temp1740=( void*) _temp1738->r; struct Cyc_Absyn_Vardecl*
_temp1746; _LL1742: if(( unsigned int) _temp1740 > 2u?(( struct _enum_struct*)
_temp1740)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1747: _temp1746=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1740)->f1; goto _LL1743;} else{ goto
_LL1744;} _LL1744: goto _LL1745; _LL1743: var_case= 1; _temp1746->initializer=(
struct Cyc_Absyn_Exp*) _temp1732;( void*)( _temp1681->r=( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1748=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1748->tag= Cyc_Absyn_Var_d_tag; _temp1748->f1= _temp1746;( void*) _temp1748;}));
goto _LL1741; _LL1745: goto _LL1741; _LL1741:;} if( var_case){ goto _LL1729;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp1738, _temp1734,
_temp1732, _temp1730, _temp1679))->r)); goto _LL1725;} _LL1729: { struct Cyc_List_List*
_temp1752; struct Cyc_Toc_Env* _temp1754; struct _tuple18 _temp1750= Cyc_Toc_decls_to_c(
nv,({ struct Cyc_List_List* _temp1749=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1749->hd=( void*) _temp1681; _temp1749->tl=
0; _temp1749;}), 0); _LL1755: _temp1754= _temp1750.f1; goto _LL1753; _LL1753:
_temp1752= _temp1750.f2; goto _LL1751; _LL1751: Cyc_Toc_stmt_to_c( _temp1754,
_temp1679); for( 0; _temp1752 != 0; _temp1752= _temp1752->tl){ _temp1679= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*) _temp1752->hd, _temp1679, 0);}( void*)( s->r=( void*)((
void*) _temp1679->r)); goto _LL1725;} _LL1725:;} return; _LL1616:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1756=( char*)"cut"; struct _tagged_string _temp1757; _temp1757.curr=
_temp1756; _temp1757.base= _temp1756; _temp1757.last_plus_one= _temp1756 + 4;
_temp1757;})); return; _LL1618:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1758=( char*)"splice"; struct _tagged_string
_temp1759; _temp1759.curr= _temp1758; _temp1759.base= _temp1758; _temp1759.last_plus_one=
_temp1758 + 7; _temp1759;})); return; _LL1620: s= _temp1687; continue; _LL1622:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1697); Cyc_Toc_exp_to_c( nv,
_temp1695); return; _LL1624: { struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp1760=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1760->f1= Cyc_Toc_abs_ns; _temp1760->f2= Cyc_Toc__push_handler_sp;
_temp1760;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1761=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1761->f1= Cyc_Toc_abs_ns; _temp1761->f2= Cyc_Toc_setjmp_sp; _temp1761;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1*
_temp1762=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1762->f1=
Cyc_Toc_abs_ns; _temp1762->f2= Cyc_Toc__pop_handler_sp; _temp1762;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp1763=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1763->v=( void*) e_typ; _temp1763;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( Cyc_Toc_try_env( nv), _temp1701);{ struct
Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp1701, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl*
x_vd= Cyc_Absyn_new_vardecl( x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt*
_temp1764=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1764->v=( void*) Cyc_Absyn_exn_typ; _temp1764;});{ struct Cyc_Absyn_Pat* p=({
struct Cyc_Absyn_Pat* _temp1765=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof(
struct Cyc_Absyn_Pat)); _temp1765->r=( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1767=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1767->tag= Cyc_Absyn_Var_p_tag; _temp1767->f1= x_vd;( void*) _temp1767;});
_temp1765->topt=({ struct Cyc_Core_Opt* _temp1766=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1766->v=( void*) Cyc_Absyn_exn_typ;
_temp1766;}); _temp1765->loc= 0; _temp1765;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp(
xexp, 0); te->topt=({ struct Cyc_Core_Opt* _temp1768=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1768->v=( void*) Cyc_Absyn_VoidType;
_temp1768;});{ struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp1769=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp1769->pattern= p;
_temp1769->pat_vars=({ struct Cyc_Core_Opt* _temp1770=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1770->v=( void*)({ struct Cyc_List_List*
_temp1771=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1771->hd=( void*) x_vd; _temp1771->tl= 0; _temp1771;}); _temp1770;});
_temp1769->where_clause= 0; _temp1769->body= rs; _temp1769->loc= 0; _temp1769;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1699,({
struct Cyc_List_List* _temp1772=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1772->hd=( void*) default_case; _temp1772->tl= 0;
_temp1772;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp1773=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1773->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1773->tl= 0; _temp1773;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp1774=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1774->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp1774->tl= 0;
_temp1774;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp( Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL1588:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args= args->tl){(*(( struct _tuple4*) args->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple4*) args->hd)).f3);{ struct _tuple1* x=({ struct _tuple1* _temp1775=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1775->f1= Cyc_Absyn_Loc_n;
_temp1775->f2=(*(( struct _tuple4*) args->hd)).f1; _temp1775;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1776= s;
_LL1778: if( _temp1776 == Cyc_Absyn_Abstract){ goto _LL1779;} else{ goto _LL1780;}
_LL1780: if( _temp1776 == Cyc_Absyn_ExternC){ goto _LL1781;} else{ goto _LL1782;}
_LL1782: goto _LL1783; _LL1779: return Cyc_Absyn_Public; _LL1781: return Cyc_Absyn_Extern;
_LL1783: return s; _LL1777:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0;
static void Cyc_Toc_structdecl_to_c( struct Cyc_Absyn_Structdecl* s){ struct
_tuple1* n=( struct _tuple1*)( s->name)->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1784=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1784->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp1784;});}{ int seen_defn_before; struct Cyc_Core_Opt*
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
struct Cyc_Core_Opt* _temp1785=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1785->v=( void*) n; _temp1785;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)( s->fields)->v; for( 0; fields != 0; fields= fields->tl){(*((
struct _tuple4*) fields->hd)).f3= Cyc_Toc_typ_to_c_array((*(( struct _tuple4*)
fields->hd)).f3);}}}}} static struct Cyc_Core_Opt* Cyc_Toc_enums_so_far= 0;
static struct Cyc_List_List* Cyc_Toc_enumdecl_to_c( struct Cyc_Absyn_Enumdecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)( ed->name)->v;
if( Cyc_Toc_enums_so_far == 0){ Cyc_Toc_enums_so_far=({ struct Cyc_Core_Opt*
_temp1786=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1786->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1786;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
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
_temp1787=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1787->name= n; _temp1787->tvs= 0; _temp1787->defn=( void*) Cyc_Absyn_void_star_typ();
_temp1787;}); res=({ struct Cyc_List_List* _temp1788=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1788->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1789=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1789->tag= Cyc_Absyn_Typedef_d_tag;
_temp1789->f1= tdef;( void*) _temp1789;}), 0); _temp1788->tl= res; _temp1788;});}{
void* t=({ struct Cyc_Absyn_TypedefType_struct* _temp1790=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1790->tag= Cyc_Absyn_TypedefType_tag;
_temp1790->f1= n; _temp1790->f2= 0; _temp1790->f3=({ struct Cyc_Core_Opt*
_temp1791=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1791->v=( void*) Cyc_Absyn_void_star_typ(); _temp1791;});( void*) _temp1790;});
int tag_count= 0; int box_count= 0;{ struct Cyc_List_List* fields= ed->fields ==
0? 0:( struct Cyc_List_List*)( ed->fields)->v; for( 0; fields != 0; fields=
fields->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fields->hd;
if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1792=( char*)"can't deal with enum tags yet";
struct _tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base=
_temp1792; _temp1793.last_plus_one= _temp1792 + 30; _temp1793;}));} if( f->typs
== 0){ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc == Cyc_Absyn_Extern? 0:(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Absyn_signed_int_exp((
tag_count ++), 0), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( f->name,
t, eopt);( void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({
struct Cyc_Absyn_Tqual* _temp1794=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1794->q_const= 1; _temp1794->q_volatile=
0; _temp1794->q_restrict= 0; _temp1794;}); res=({ struct Cyc_List_List*
_temp1795=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1795->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1796=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1796->tag= Cyc_Absyn_Var_d_tag; _temp1796->f1= vd;( void*) _temp1796;}), 0);
_temp1795->tl= res; _temp1795;});} else{ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc
== Cyc_Absyn_Extern? 0:( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1797=( char*)"_tag"; struct
_tagged_string _temp1798; _temp1798.curr= _temp1797; _temp1798.base= _temp1797;
_temp1798.last_plus_one= _temp1797 + 5; _temp1798;})), Cyc_Absyn_sint_t, eopt);(
void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({ struct
Cyc_Absyn_Tqual* _temp1799=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Tqual)); _temp1799->q_const= 1; _temp1799->q_volatile= 0;
_temp1799->q_restrict= 0; _temp1799;}); res=({ struct Cyc_List_List* _temp1800=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1800->hd=(
void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct* _temp1801=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1801->tag= Cyc_Absyn_Var_d_tag; _temp1801->f1= vd;( void*) _temp1801;}), 0);
_temp1800->tl= res; _temp1800;}); if( ! seen_defn_before){ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl,
i ++){ struct _tagged_string* fname=( struct _tagged_string*)({ struct
_tagged_string* _temp1802=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1802[ 0]= xprintf("f%d", i); _temp1802;}); fs=({
struct Cyc_List_List* _temp1803=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1803->hd=( void*)({ struct _tuple4* _temp1804=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1804->f1= fname;
_temp1804->f2=(*(( struct _tuple6*) ts->hd)).f1; _temp1804->f3= Cyc_Toc_typ_to_c_array((*((
struct _tuple6*) ts->hd)).f2); _temp1804;}); _temp1803->tl= fs; _temp1803;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);
fs=({ struct Cyc_List_List* _temp1805=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1805->hd=( void*)({ struct _tuple4*
_temp1806=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1806->f1=
Cyc_Toc_tag_sp; _temp1806->f2= Cyc_Toc_mt_tq; _temp1806->f3= Cyc_Absyn_sint_t;
_temp1806;}); _temp1805->tl= fs; _temp1805;});{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp1807=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp1807->sc=( void*) Cyc_Absyn_Public;
_temp1807->name=({ struct Cyc_Core_Opt* _temp1809=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1809->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1810=( char*)"_struct"; struct
_tagged_string _temp1811; _temp1811.curr= _temp1810; _temp1811.base= _temp1810;
_temp1811.last_plus_one= _temp1810 + 8; _temp1811;})); _temp1809;}); _temp1807->tvs=
0; _temp1807->fields=({ struct Cyc_Core_Opt* _temp1808=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1808->v=( void*) fs; _temp1808;});
_temp1807;}); res=({ struct Cyc_List_List* _temp1812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1812->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp1813=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1813->tag= Cyc_Absyn_Struct_d_tag;
_temp1813->f1= sd;( void*) _temp1813;}), 0); _temp1812->tl= res; _temp1812;});}}}}}
return res;}}} static struct Cyc_Core_Opt* Cyc_Toc_xenums_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xenumdecl_to_c( struct Cyc_Absyn_Xenumdecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp1814=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1814->name= n; _temp1814->tvs= 0; _temp1814->defn=( void*) t; _temp1814;});
res=({ struct Cyc_List_List* _temp1815=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1815->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1816=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1816->tag= Cyc_Absyn_Typedef_d_tag;
_temp1816->f1= td;( void*) _temp1816;}), 0); _temp1815->tl= res; _temp1815;});}
else{ if( Cyc_Toc_xenums_so_far == 0){ Cyc_Toc_xenums_so_far=({ struct Cyc_Core_Opt*
_temp1817=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1817->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1817;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->tag
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1818=( char*)"xenum:  can't deal with explicit tags";
struct _tagged_string _temp1819; _temp1819.curr= _temp1818; _temp1819.base=
_temp1818; _temp1819.last_plus_one= _temp1818 + 38; _temp1819;}));}{ struct
_tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp1820=* fn;( unsigned int)( _temp1820.last_plus_one -
_temp1820.curr);}), 0); void* tag_typ=({ struct Cyc_Absyn_ArrayType_struct*
_temp1821=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1821->tag= Cyc_Absyn_ArrayType_tag; _temp1821->f1=( void*) Cyc_Absyn_uchar_t;
_temp1821->f2= Cyc_Toc_mt_tq; _temp1821->f3= sz_exp;( void*) _temp1821;});
struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc != Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp( 0,* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp1822=( char*)"_tag"; struct _tagged_string
_temp1823; _temp1823.curr= _temp1822; _temp1823.base= _temp1822; _temp1823.last_plus_one=
_temp1822 + 5; _temp1823;})), tag_typ, initopt);( void*)( tag_decl->sc=( void*)((
void*) xd->sc)); res=({ struct Cyc_List_List* _temp1824=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1824->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1825=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1825->tag= Cyc_Absyn_Var_d_tag;
_temp1825->f1= tag_decl;( void*) _temp1825;}), 0); _temp1824->tl= res; _temp1824;});{
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
_temp1826=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1826[ 0]= xprintf("f%d", i); _temp1826;}); struct _tuple4* newf=({
struct _tuple4* _temp1827=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1827->f1= field_n; _temp1827->f2=(*(( struct _tuple6*) ts->hd)).f1;
_temp1827->f3= Cyc_Toc_typ_to_c_array((*(( struct _tuple6*) ts->hd)).f2);
_temp1827;}); fields=({ struct Cyc_List_List* _temp1828=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1828->hd=( void*) newf;
_temp1828->tl= fields; _temp1828;});}} fields=({ struct Cyc_List_List* _temp1829=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1829->hd=(
void*)({ struct _tuple4* _temp1830=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1830->f1= Cyc_Toc_tag_sp; _temp1830->f2= Cyc_Toc_mt_tq;
_temp1830->f3= Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp1830;});
_temp1829->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
fields); _temp1829;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp1831=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp1831->sc=( void*) Cyc_Absyn_Public; _temp1831->name=({ struct Cyc_Core_Opt*
_temp1833=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1833->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp1834=( char*)"_struct"; struct _tagged_string _temp1835; _temp1835.curr=
_temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one= _temp1834 + 8;
_temp1835;})); _temp1833;}); _temp1831->tvs= 0; _temp1831->fields=({ struct Cyc_Core_Opt*
_temp1832=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1832->v=( void*) fields; _temp1832;}); _temp1831;}); res=({ struct Cyc_List_List*
_temp1836=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1836->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp1837=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1837->tag= Cyc_Absyn_Struct_d_tag; _temp1837->f1= strct_decl;( void*)
_temp1837;}), 0); _temp1836->tl= res; _temp1836;});}}}}}}}} return res;} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s){ void* t=( void*) topt->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp1840; struct Cyc_List_List* _temp1842; struct Cyc_Toc_Env* _temp1844;
struct _tuple14 _temp1838= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL1845: _temp1844=
_temp1838.f1; goto _LL1843; _LL1843: _temp1842= _temp1838.f2; goto _LL1841;
_LL1841: _temp1840= _temp1838.f3; goto _LL1839; _LL1839: Cyc_Toc_stmt_to_c(
_temp1844, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1840, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1846=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1846->f1= Cyc_Toc_abs_ns; _temp1846->f2= Cyc_Toc_Match_Exception_sp;
_temp1846;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1840, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp1842 != 0; _temp1842= _temp1842->tl){
struct _tuple15 _temp1849; void* _temp1850; struct _tuple1* _temp1852; struct
_tuple15* _temp1847=( struct _tuple15*) _temp1842->hd; _temp1849=* _temp1847;
_LL1853: _temp1852= _temp1849.f1; goto _LL1851; _LL1851: _temp1850= _temp1849.f2;
goto _LL1848; _LL1848: s= Cyc_Absyn_declare_stmt( _temp1852, _temp1850, 0, s, 0);}
return s;}}} static struct _tuple18 Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top){ struct Cyc_List_List* res= 0; for( 0; ds !=
0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) ds->hd; void*
_temp1854=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp1878; struct Cyc_Absyn_Fndecl*
_temp1880; int _temp1882; struct Cyc_Absyn_Exp* _temp1884; struct Cyc_Core_Opt*
_temp1886; struct Cyc_Core_Opt* _temp1888; struct Cyc_Absyn_Pat* _temp1890;
struct Cyc_Absyn_Structdecl* _temp1892; struct Cyc_Absyn_Enumdecl* _temp1894;
struct Cyc_Absyn_Xenumdecl* _temp1896; struct Cyc_Absyn_Typedefdecl* _temp1898;
struct Cyc_List_List* _temp1900; struct _tagged_string* _temp1902; struct Cyc_List_List*
_temp1904; struct _tuple1* _temp1906; struct Cyc_List_List* _temp1908; _LL1856:
if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*) _temp1854)->tag ==
Cyc_Absyn_Var_d_tag: 0){ _LL1879: _temp1878=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1854)->f1; goto _LL1857;} else{ goto
_LL1858;} _LL1858: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL1881: _temp1880=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1854)->f1; goto _LL1859;} else{ goto _LL1860;}
_LL1860: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*) _temp1854)->tag
== Cyc_Absyn_Let_d_tag: 0){ _LL1891: _temp1890=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp1854)->f1; goto _LL1889; _LL1889: _temp1888=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1854)->f2; goto
_LL1887; _LL1887: _temp1886=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1854)->f3; goto _LL1885; _LL1885: _temp1884=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1854)->f4; goto _LL1883; _LL1883: _temp1882=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1854)->f5; goto _LL1861;} else{ goto
_LL1862;} _LL1862: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL1893: _temp1892=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1854)->f1; goto _LL1863;} else{ goto
_LL1864;} _LL1864: if( _temp1854 == Cyc_Absyn_Union_d){ goto _LL1865;} else{
goto _LL1866;} _LL1866: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1895: _temp1894=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1854)->f1; goto _LL1867;} else{ goto
_LL1868;} _LL1868: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1897: _temp1896=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1854)->f1; goto _LL1869;} else{ goto
_LL1870;} _LL1870: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL1899: _temp1898=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1854)->f1; goto _LL1871;} else{ goto
_LL1872;} _LL1872: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1903: _temp1902=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1854)->f1; goto
_LL1901; _LL1901: _temp1900=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1854)->f2; goto _LL1873;} else{ goto _LL1874;} _LL1874: if(( unsigned int)
_temp1854 > 1u?(( struct _enum_struct*) _temp1854)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1907: _temp1906=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1854)->f1; goto _LL1905; _LL1905: _temp1904=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1854)->f2; goto _LL1875;} else{ goto
_LL1876;} _LL1876: if(( unsigned int) _temp1854 > 1u?(( struct _enum_struct*)
_temp1854)->tag == Cyc_Absyn_ExternC_d_tag: 0){ _LL1909: _temp1908=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1854)->f1; goto _LL1877;} else{ goto
_LL1855;} _LL1857: { struct _tuple1* c_name= _temp1878->name; if(( void*)
_temp1878->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1* _temp1910=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1910->f1= Cyc_Toc_abs_ns;
_temp1910->f2=(* c_name).f2; _temp1910;});} if( _temp1878->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1911=
_temp1878->initializer; if( _temp1911 == 0){ _throw( Null_Exception);} _temp1911;}));}
if( nv->toplevel){ res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls, res); Cyc_Toc_temp_topdecls=
0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp1878->name, Cyc_Absyn_varb_exp(
c_name,({ struct Cyc_Absyn_Global_b_struct* _temp1912=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp1912->tag= Cyc_Absyn_Global_b_tag;
_temp1912->f1= _temp1878;( void*) _temp1912;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp1878->name, Cyc_Absyn_varb_exp( c_name,({ struct Cyc_Absyn_Local_b_struct*
_temp1913=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1913->tag= Cyc_Absyn_Local_b_tag; _temp1913->f1= _temp1878;( void*)
_temp1913;}), 0));} _temp1878->name= c_name;( void*)( _temp1878->sc=( void*) Cyc_Toc_scope_to_c((
void*) _temp1878->sc));( void*)( _temp1878->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1878->type)); res=({ struct Cyc_List_List* _temp1914=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1914->hd=( void*) d; _temp1914->tl=
res; _temp1914;}); goto _LL1855;} _LL1859: { struct _tuple1* c_name= _temp1880->name;
if(( void*) _temp1880->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1*
_temp1915=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1915->f1=
Cyc_Toc_abs_ns; _temp1915->f2=(* c_name).f2; _temp1915;});} nv= Cyc_Toc_add_varmap(
nv, _temp1880->name, Cyc_Absyn_var_exp( c_name, 0)); _temp1880->name= c_name;
Cyc_Toc_fndecl_to_c( nv, _temp1880); res=({ struct Cyc_List_List* _temp1916=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1916->hd=(
void*) d; _temp1916->tl= res; _temp1916;}); goto _LL1855;} _LL1861:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1917=( char*)"letdecl"; struct _tagged_string _temp1918; _temp1918.curr=
_temp1917; _temp1918.base= _temp1917; _temp1918.last_plus_one= _temp1917 + 8;
_temp1918;})); goto _LL1855; _LL1863: Cyc_Toc_structdecl_to_c( _temp1892); res=({
struct Cyc_List_List* _temp1919=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1919->hd=( void*) d; _temp1919->tl= res; _temp1919;});
goto _LL1855; _LL1865:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1920=( char*)"uniondecl"; struct
_tagged_string _temp1921; _temp1921.curr= _temp1920; _temp1921.base= _temp1920;
_temp1921.last_plus_one= _temp1920 + 10; _temp1921;})); goto _LL1855; _LL1867:
res= Cyc_Toc_enumdecl_to_c( _temp1894, res); goto _LL1855; _LL1869: res= Cyc_Toc_xenumdecl_to_c(
_temp1896, res); goto _LL1855; _LL1871: _temp1898->name= _temp1898->name;
_temp1898->tvs= 0;( void*)( _temp1898->defn=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1898->defn)); res=({ struct Cyc_List_List* _temp1922=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1922->hd=( void*) d; _temp1922->tl=
res; _temp1922;}); goto _LL1855; _LL1873: _temp1904= _temp1900; goto _LL1875;
_LL1875: _temp1908= _temp1904; goto _LL1877; _LL1877: { struct _tuple18 p= Cyc_Toc_decls_to_c(
nv, _temp1908, top); nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( p.f2), res); goto _LL1855;} _LL1855:;}
return({ struct _tuple18 _temp1923; _temp1923.f1= nv; _temp1923.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp1923;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_enums_so_far= 0; Cyc_Toc_xenums_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List* _temp1926;
struct Cyc_Toc_Env* _temp1928; struct _tuple18 _temp1924= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL1929: _temp1928= _temp1924.f1; goto _LL1927;
_LL1927: _temp1926= _temp1924.f2; goto _LL1925; _LL1925: while( Cyc_Toc_added_decls
!= 0) { struct Cyc_List_List* temp= Cyc_Toc_added_decls->tl; Cyc_Toc_added_decls->tl=
_temp1926; _temp1926= Cyc_Toc_added_decls; Cyc_Toc_added_decls= temp;} return
_temp1926;}}