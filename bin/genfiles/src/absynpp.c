#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple6{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple7{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{ void* f1; void* f2; } ;
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List*
x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc*
Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); extern struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql(
struct _tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name; typedef
struct _tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
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
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl;
struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator; typedef struct
_xenum_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute; struct
Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope;
typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Uniondecl* Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Enumfield*
Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl;
typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl;
typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl;
typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; typedef void* Cyc_Absyn_mallocarg_t;
extern void* Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Abs_n_tag;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; extern
void* Cyc_Absyn_Static; extern void* Cyc_Absyn_Abstract; extern void* Cyc_Absyn_Public;
extern void* Cyc_Absyn_Extern; extern void* Cyc_Absyn_ExternC; struct Cyc_Absyn_Tqual{
int q_const; int q_volatile; int q_restrict; } ; extern void* Cyc_Absyn_B1;
extern void* Cyc_Absyn_B2; extern void* Cyc_Absyn_B4; extern void* Cyc_Absyn_B8;
extern void* Cyc_Absyn_AnyKind; extern void* Cyc_Absyn_MemKind; extern void* Cyc_Absyn_BoxKind;
extern void* Cyc_Absyn_RgnKind; extern void* Cyc_Absyn_EffKind; extern void* Cyc_Absyn_Signed;
extern void* Cyc_Absyn_Unsigned; struct Cyc_Absyn_Conref{ void* v; } ; extern
const int Cyc_Absyn_Eq_constr_tag; struct Cyc_Absyn_Eq_constr_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Forward_constr_tag; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; extern void* Cyc_Absyn_No_constr;
struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; extern void* Cyc_Absyn_Unknown_b; extern const int Cyc_Absyn_Upper_b_tag;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; extern void* Cyc_Absyn_VoidType; extern const int Cyc_Absyn_Evar_tag;
struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int f3;
} ; extern const int Cyc_Absyn_VarType_tag; struct Cyc_Absyn_VarType_struct{ int
tag; struct Cyc_Absyn_Tvar* f1; } ; extern const int Cyc_Absyn_EnumType_tag;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Enumdecl** f3; } ; extern const int Cyc_Absyn_XenumType_tag;
struct Cyc_Absyn_XenumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl**
f2; } ; extern const int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void*
Cyc_Absyn_FloatType; extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern const int Cyc_Absyn_UnionType_tag; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; extern const int Cyc_Absyn_RgnHandleType_tag; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; extern void* Cyc_Absyn_HeapRgn; extern const int Cyc_Absyn_AccessEff_tag;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_JoinEff_tag;
struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_NoTypes_tag; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; extern const int
Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef void* Cyc_Absyn_Pointer_Sort;
extern const int Cyc_Absyn_NonNullable_ps_tag; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Nullable_ps_tag;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern void* Cyc_Absyn_TaggedArray_ps; extern const int Cyc_Absyn_Regparm_att_tag;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; extern void* Cyc_Absyn_Stdcall_att;
extern void* Cyc_Absyn_Cdecl_att; extern void* Cyc_Absyn_Noreturn_att; extern
void* Cyc_Absyn_Const_att; extern const int Cyc_Absyn_Aligned_att_tag; struct
Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; extern void* Cyc_Absyn_Packed_att;
extern const int Cyc_Absyn_Section_att_tag; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; extern void* Cyc_Absyn_Nocommon_att;
extern void* Cyc_Absyn_Shared_att; extern void* Cyc_Absyn_Unused_att; extern
void* Cyc_Absyn_Weak_att; extern void* Cyc_Absyn_Dllimport_att; extern void* Cyc_Absyn_Dllexport_att;
extern void* Cyc_Absyn_No_instrument_function_att; extern void* Cyc_Absyn_Constructor_att;
extern void* Cyc_Absyn_Destructor_att; extern void* Cyc_Absyn_No_check_memory_usage_att;
extern void* Cyc_Absyn_Carray_mod; extern const int Cyc_Absyn_ConstArray_mod_tag;
struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Pointer_mod_tag; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; extern const int
Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{ int tag; void*
f1; } ; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
extern const int Cyc_Absyn_Attributes_mod_tag; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; extern
const int Cyc_Absyn_Char_c_tag; struct Cyc_Absyn_Char_c_struct{ int tag; void*
f1; char f2; } ; extern const int Cyc_Absyn_Short_c_tag; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; extern const int Cyc_Absyn_Int_c_tag; struct
Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2; } ; extern const int Cyc_Absyn_LongLong_c_tag;
struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; extern
const int Cyc_Absyn_Float_c_tag; struct Cyc_Absyn_Float_c_struct{ int tag;
struct _tagged_string f1; } ; extern const int Cyc_Absyn_String_c_tag; struct
Cyc_Absyn_String_c_struct{ int tag; int f1; struct _tagged_string f2; } ; extern
void* Cyc_Absyn_Null_c; extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times;
extern void* Cyc_Absyn_Minus; extern void* Cyc_Absyn_Div; extern void* Cyc_Absyn_Mod;
extern void* Cyc_Absyn_Eq; extern void* Cyc_Absyn_Neq; extern void* Cyc_Absyn_Gt;
extern void* Cyc_Absyn_Lt; extern void* Cyc_Absyn_Gte; extern void* Cyc_Absyn_Lte;
extern void* Cyc_Absyn_Not; extern void* Cyc_Absyn_Bitnot; extern void* Cyc_Absyn_Bitand;
extern void* Cyc_Absyn_Bitor; extern void* Cyc_Absyn_Bitxor; extern void* Cyc_Absyn_Bitlshift;
extern void* Cyc_Absyn_Bitlrshift; extern void* Cyc_Absyn_Bitarshift; extern
void* Cyc_Absyn_Size; extern void* Cyc_Absyn_Printf; extern void* Cyc_Absyn_Fprintf;
extern void* Cyc_Absyn_Xprintf; extern void* Cyc_Absyn_Scanf; extern void* Cyc_Absyn_Fscanf;
extern void* Cyc_Absyn_Sscanf; extern void* Cyc_Absyn_PreInc; extern void* Cyc_Absyn_PostInc;
extern void* Cyc_Absyn_PreDec; extern void* Cyc_Absyn_PostDec; extern const int
Cyc_Absyn_Typ_m_tag; struct Cyc_Absyn_Typ_m_struct{ int tag; void* f1; } ;
extern const int Cyc_Absyn_Unresolved_m_tag; struct Cyc_Absyn_Unresolved_m_struct{
int tag; struct _tuple0* f1; } ; extern const int Cyc_Absyn_Enum_m_tag; struct
Cyc_Absyn_Enum_m_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield*
f2; } ; extern const int Cyc_Absyn_Xenum_m_tag; struct Cyc_Absyn_Xenum_m_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
extern const int Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; extern
const int Cyc_Absyn_Primop_e_tag; struct Cyc_Absyn_Primop_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_AssignOp_e_tag;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Increment_e_tag;
struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void* f2;
} ; extern const int Cyc_Absyn_Conditional_e_tag; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_SeqExp_e_tag; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; extern const
int Cyc_Absyn_UnknownCall_e_tag; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_FnCall_e_tag;
struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Throw_e_tag; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_NoInstantiate_e_tag;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Instantiate_e_tag; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Cast_e_tag; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Address_e_tag; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern const
int Cyc_Absyn_Sizeoftyp_e_tag; struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Sizeofexp_e_tag; struct Cyc_Absyn_Sizeofexp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Deref_e_tag;
struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_StructMember_e_tag; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const
int Cyc_Absyn_StructArrow_e_tag; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const int Cyc_Absyn_Subscript_e_tag;
struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_CompoundLit_e_tag;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; extern const
int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl*
f4; struct Cyc_Absyn_Enumfield* f5; } ; extern const int Cyc_Absyn_Xenum_e_tag;
struct Cyc_Absyn_Xenum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Xenumdecl* f3; struct Cyc_Absyn_Enumfield* f4; } ; extern
const int Cyc_Absyn_Malloc_e_tag; struct Cyc_Absyn_Malloc_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; extern const int
Cyc_Absyn_StmtExp_e_tag; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; extern const int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Fill_e_tag;
struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment*
loc; } ; extern void* Cyc_Absyn_Skip_s; extern const int Cyc_Absyn_Exp_s_tag;
struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_Seq_s_tag; struct Cyc_Absyn_Seq_s_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Return_s_tag;
struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_IfThenElse_s_tag; struct Cyc_Absyn_IfThenElse_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; extern const int Cyc_Absyn_While_s_tag; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Break_s_tag;
struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Continue_s_tag; struct Cyc_Absyn_Continue_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_For_s_tag; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; extern
const int Cyc_Absyn_Switch_s_tag; struct Cyc_Absyn_Switch_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Fallthru_s_tag;
struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1; struct
Cyc_Absyn_Switch_clause** f2; } ; extern const int Cyc_Absyn_Decl_s_tag; struct
Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; extern const int Cyc_Absyn_Cut_s_tag; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Splice_s_tag;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Do_s_tag;
struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2
f2; } ; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
int try_depth; struct _xenum_struct* annot; } ; extern void* Cyc_Absyn_Wild_p;
extern const int Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern void* Cyc_Absyn_Null_p; extern const
int Cyc_Absyn_Int_p_tag; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int
f2; } ; extern const int Cyc_Absyn_Char_p_tag; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; extern const int Cyc_Absyn_Float_p_tag; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; extern const int Cyc_Absyn_Tuple_p_tag;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; extern const int Cyc_Absyn_Reference_p_tag; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern
const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; extern const int Cyc_Absyn_Enum_p_tag; struct
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6; } ; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
extern void* Cyc_Absyn_Unresolved_b; extern const int Cyc_Absyn_Global_b_tag;
struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_Absyn_Funname_b_tag; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Param_b_tag;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_Absyn_Local_b_tag; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern const int Cyc_Absyn_Pat_b_tag; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq; void* type; struct
Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt*
cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; extern const
int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; extern const int Cyc_Absyn_Fn_d_tag; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Let_d_tag;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; extern const
int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Absyn_Union_d_tag; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; extern const int Cyc_Absyn_Enum_d_tag;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{ int tag;
struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; extern const int Cyc_Absyn_Namespace_d_tag; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_FieldName_tag; struct
Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ; extern char
Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; }
; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim(
void* p); extern struct _tagged_string Cyc_Absyn_attribute2string( void*);
struct Cyc_Buffer_t; typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern struct
_tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag;
struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
extern const int Cyc_Tcenv_EnumRes_tag; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; extern const
int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{ int tag; struct
Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* xenumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; typedef struct Cyc_Tcenv_Genv*
Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t;
typedef void* Cyc_Tcenv_Jumpee; extern void* Cyc_Tcenv_NotLoop_j; extern void*
Cyc_Tcenv_CaseEnd_j; extern void* Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; typedef void* Cyc_Tcenv_frames; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*); extern int Cyc_Absynpp_expand_typedefs;
extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_decls_first; extern
int Cyc_Absynpp_print_all_tvars; extern int Cyc_Absynpp_print_all_kinds; extern
int Cyc_Absynpp_print_using_stmts; extern int Cyc_Absynpp_print_externC_stmts;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d);
extern struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_kind2string( void*); extern struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl); extern struct _tagged_string Cyc_Absynpp_prim2string(
void* p); extern struct _tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat*
p); extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_string Cyc_Absynpp_char_escape( char); extern struct _tagged_string Cyc_Absynpp_string_escape(
struct _tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void*
p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual* tq, void* t); extern struct
Cyc_PP_Doc* Cyc_Absynpp_egroup( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat11(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat10( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat9( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat8(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat3(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat2( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual*
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt); extern
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fields);
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids= 0; int Cyc_Absynpp_decls_first=
0; int Cyc_Absynpp_dont_rewrite_temp_tvars= 0; int Cyc_Absynpp_print_all_tvars=
0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_print_using_stmts= 0; int
Cyc_Absynpp_print_externC_stmts= 0; static struct Cyc_List_List* Cyc_Absynpp_curr_namespace=
0; static void Cyc_Absynpp_curr_namespace_add( struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= Cyc_Absynpp_curr_namespace;
_temp0;});} static void Cyc_Absynpp_curr_namespace_drop(){ Cyc_Absynpp_curr_namespace=
Cyc_Absynpp_curr_namespace->tl;} struct _tagged_string Cyc_Absynpp_char_escape(
char c){ switch( c){ case '\a': _LL1: return( struct _tagged_string)({ char*
_temp3=( char*)"\\a"; struct _tagged_string _temp4; _temp4.curr= _temp3; _temp4.base=
_temp3; _temp4.last_plus_one= _temp3 + 3; _temp4;}); case '\b': _LL2: return(
struct _tagged_string)({ char* _temp6=( char*)"\\b"; struct _tagged_string
_temp7; _temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one= _temp6 +
3; _temp7;}); case '\f': _LL5: return( struct _tagged_string)({ char* _temp9=(
char*)"\\f"; struct _tagged_string _temp10; _temp10.curr= _temp9; _temp10.base=
_temp9; _temp10.last_plus_one= _temp9 + 3; _temp10;}); case '\n': _LL8: return(
struct _tagged_string)({ char* _temp12=( char*)"\\n"; struct _tagged_string
_temp13; _temp13.curr= _temp12; _temp13.base= _temp12; _temp13.last_plus_one=
_temp12 + 3; _temp13;}); case '\r': _LL11: return( struct _tagged_string)({ char*
_temp15=( char*)"\\r"; struct _tagged_string _temp16; _temp16.curr= _temp15;
_temp16.base= _temp15; _temp16.last_plus_one= _temp15 + 3; _temp16;}); case '\t':
_LL14: return( struct _tagged_string)({ char* _temp18=( char*)"\\t"; struct
_tagged_string _temp19; _temp19.curr= _temp18; _temp19.base= _temp18; _temp19.last_plus_one=
_temp18 + 3; _temp19;}); case '\v': _LL17: return( struct _tagged_string)({ char*
_temp21=( char*)"\\v"; struct _tagged_string _temp22; _temp22.curr= _temp21;
_temp22.base= _temp21; _temp22.last_plus_one= _temp21 + 3; _temp22;}); case '\\':
_LL20: return( struct _tagged_string)({ char* _temp24=( char*)"\\\\"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}); case '"': _LL23: return( struct _tagged_string)({ char*
_temp27=( char*)"\""; struct _tagged_string _temp28; _temp28.curr= _temp27;
_temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 2; _temp28;}); case '\'':
_LL26: return( struct _tagged_string)({ char* _temp30=( char*)"\\'"; struct
_tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 3; _temp31;}); default: _LL29: if(( int) c >=( int)' '?( int) c <=(
int)'~': 0){ struct _tagged_string t= Cyc_Core_new_string( 1);({ struct
_tagged_string _temp33= t; char* _temp35= _temp33.curr + 0; if( _temp35 <
_temp33.base? 1: _temp35 >= _temp33.last_plus_one){ _throw( Null_Exception);}*
_temp35= c;}); return t;} else{ struct _tagged_string t= Cyc_Core_new_string( 4);
int j= 0;({ struct _tagged_string _temp36= t; char* _temp38= _temp36.curr +( j
++); if( _temp38 < _temp36.base? 1: _temp38 >= _temp36.last_plus_one){ _throw(
Null_Exception);}* _temp38='\\';});({ struct _tagged_string _temp39= t; char*
_temp41= _temp39.curr +( j ++); if( _temp41 < _temp39.base? 1: _temp41 >=
_temp39.last_plus_one){ _throw( Null_Exception);}* _temp41=( char)(( int)'0' +((
int) c >> 6 & 7));});({ struct _tagged_string _temp42= t; char* _temp44= _temp42.curr
+( j ++); if( _temp44 < _temp42.base? 1: _temp44 >= _temp42.last_plus_one){
_throw( Null_Exception);}* _temp44=( char)(( int)'0' +(( int) c >> 3 & 7));});({
struct _tagged_string _temp45= t; char* _temp47= _temp45.curr +( j ++); if(
_temp47 < _temp45.base? 1: _temp47 >= _temp45.last_plus_one){ _throw(
Null_Exception);}* _temp47=( char)(( int)'0' +(( int) c & 7));}); return t;}}}
static int Cyc_Absynpp_special( struct _tagged_string s){ int sz=( int)(({
struct _tagged_string _temp48= s;( unsigned int)( _temp48.last_plus_one -
_temp48.curr);}) -( unsigned int) 1);{ int i= 0; for( 0; i < sz; i ++){ char c=({
struct _tagged_string _temp49= s; char* _temp51= _temp49.curr + i; if( _temp51 <
_temp49.base? 1: _temp51 >= _temp49.last_plus_one){ _throw( Null_Exception);}*
_temp51;}); if(((( int) c <=( int)' '? 1:( int) c >=( int)'~')? 1:( int) c ==(
int)'"')? 1:( int) c ==( int)'\\'){ return 1;}}} return 0;} struct
_tagged_string Cyc_Absynpp_string_escape( struct _tagged_string s){ if( ! Cyc_Absynpp_special(
s)){ return s;}{ int n=( int)(({ struct _tagged_string _temp52= s;( unsigned int)(
_temp52.last_plus_one - _temp52.curr);}) -( unsigned int) 1); if( n > 0?( int)({
struct _tagged_string _temp53= s; char* _temp55= _temp53.curr + n; if( _temp55 <
_temp53.base? 1: _temp55 >= _temp53.last_plus_one){ _throw( Null_Exception);}*
_temp55;}) ==( int)'\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i
++){ char _temp59=({ struct _tagged_string _temp56= s; char* _temp58= _temp56.curr
+ i; if( _temp58 < _temp56.base? 1: _temp58 >= _temp56.last_plus_one){ _throw(
Null_Exception);}* _temp58;}); _LL61: if( _temp59 =='\a'){ goto _LL62;} else{
goto _LL63;} _LL63: if( _temp59 =='\b'){ goto _LL64;} else{ goto _LL65;} _LL65:
if( _temp59 =='\f'){ goto _LL66;} else{ goto _LL67;} _LL67: if( _temp59 =='\n'){
goto _LL68;} else{ goto _LL69;} _LL69: if( _temp59 =='\r'){ goto _LL70;} else{
goto _LL71;} _LL71: if( _temp59 =='\t'){ goto _LL72;} else{ goto _LL73;} _LL73:
if( _temp59 =='\v'){ goto _LL74;} else{ goto _LL75;} _LL75: if( _temp59 =='\\'){
goto _LL76;} else{ goto _LL77;} _LL77: if( _temp59 =='"'){ goto _LL78;} else{
goto _LL79;} _LL79: goto _LL80; _LL62: goto _LL64; _LL64: goto _LL66; _LL66:
goto _LL68; _LL68: goto _LL70; _LL70: goto _LL72; _LL72: goto _LL74; _LL74: goto
_LL76; _LL76: goto _LL78; _LL78: len += 2; goto _LL60; _LL80: if(( int) _temp59
>=( int)' '?( int) _temp59 <=( int)'~': 0){ len ++;} else{ len += 4;} goto _LL60;
_LL60:;}}{ struct _tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i=
0; for( 0; i <= n; i ++){ char _temp84=({ struct _tagged_string _temp81= s; char*
_temp83= _temp81.curr + i; if( _temp83 < _temp81.base? 1: _temp83 >= _temp81.last_plus_one){
_throw( Null_Exception);}* _temp83;}); _LL86: if( _temp84 =='\a'){ goto _LL87;}
else{ goto _LL88;} _LL88: if( _temp84 =='\b'){ goto _LL89;} else{ goto _LL90;}
_LL90: if( _temp84 =='\f'){ goto _LL91;} else{ goto _LL92;} _LL92: if( _temp84
=='\n'){ goto _LL93;} else{ goto _LL94;} _LL94: if( _temp84 =='\r'){ goto _LL95;}
else{ goto _LL96;} _LL96: if( _temp84 =='\t'){ goto _LL97;} else{ goto _LL98;}
_LL98: if( _temp84 =='\v'){ goto _LL99;} else{ goto _LL100;} _LL100: if( _temp84
=='\\'){ goto _LL101;} else{ goto _LL102;} _LL102: if( _temp84 =='"'){ goto
_LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL87:({ struct _tagged_string
_temp106= t; char* _temp108= _temp106.curr +( j ++); if( _temp108 < _temp106.base?
1: _temp108 >= _temp106.last_plus_one){ _throw( Null_Exception);}* _temp108='\\';});({
struct _tagged_string _temp109= t; char* _temp111= _temp109.curr +( j ++); if(
_temp111 < _temp109.base? 1: _temp111 >= _temp109.last_plus_one){ _throw(
Null_Exception);}* _temp111='a';}); goto _LL85; _LL89:({ struct _tagged_string
_temp112= t; char* _temp114= _temp112.curr +( j ++); if( _temp114 < _temp112.base?
1: _temp114 >= _temp112.last_plus_one){ _throw( Null_Exception);}* _temp114='\\';});({
struct _tagged_string _temp115= t; char* _temp117= _temp115.curr +( j ++); if(
_temp117 < _temp115.base? 1: _temp117 >= _temp115.last_plus_one){ _throw(
Null_Exception);}* _temp117='b';}); goto _LL85; _LL91:({ struct _tagged_string
_temp118= t; char* _temp120= _temp118.curr +( j ++); if( _temp120 < _temp118.base?
1: _temp120 >= _temp118.last_plus_one){ _throw( Null_Exception);}* _temp120='\\';});({
struct _tagged_string _temp121= t; char* _temp123= _temp121.curr +( j ++); if(
_temp123 < _temp121.base? 1: _temp123 >= _temp121.last_plus_one){ _throw(
Null_Exception);}* _temp123='f';}); goto _LL85; _LL93:({ struct _tagged_string
_temp124= t; char* _temp126= _temp124.curr +( j ++); if( _temp126 < _temp124.base?
1: _temp126 >= _temp124.last_plus_one){ _throw( Null_Exception);}* _temp126='\\';});({
struct _tagged_string _temp127= t; char* _temp129= _temp127.curr +( j ++); if(
_temp129 < _temp127.base? 1: _temp129 >= _temp127.last_plus_one){ _throw(
Null_Exception);}* _temp129='n';}); goto _LL85; _LL95:({ struct _tagged_string
_temp130= t; char* _temp132= _temp130.curr +( j ++); if( _temp132 < _temp130.base?
1: _temp132 >= _temp130.last_plus_one){ _throw( Null_Exception);}* _temp132='\\';});({
struct _tagged_string _temp133= t; char* _temp135= _temp133.curr +( j ++); if(
_temp135 < _temp133.base? 1: _temp135 >= _temp133.last_plus_one){ _throw(
Null_Exception);}* _temp135='r';}); goto _LL85; _LL97:({ struct _tagged_string
_temp136= t; char* _temp138= _temp136.curr +( j ++); if( _temp138 < _temp136.base?
1: _temp138 >= _temp136.last_plus_one){ _throw( Null_Exception);}* _temp138='\\';});({
struct _tagged_string _temp139= t; char* _temp141= _temp139.curr +( j ++); if(
_temp141 < _temp139.base? 1: _temp141 >= _temp139.last_plus_one){ _throw(
Null_Exception);}* _temp141='t';}); goto _LL85; _LL99:({ struct _tagged_string
_temp142= t; char* _temp144= _temp142.curr +( j ++); if( _temp144 < _temp142.base?
1: _temp144 >= _temp142.last_plus_one){ _throw( Null_Exception);}* _temp144='\\';});({
struct _tagged_string _temp145= t; char* _temp147= _temp145.curr +( j ++); if(
_temp147 < _temp145.base? 1: _temp147 >= _temp145.last_plus_one){ _throw(
Null_Exception);}* _temp147='v';}); goto _LL85; _LL101:({ struct _tagged_string
_temp148= t; char* _temp150= _temp148.curr +( j ++); if( _temp150 < _temp148.base?
1: _temp150 >= _temp148.last_plus_one){ _throw( Null_Exception);}* _temp150='\\';});({
struct _tagged_string _temp151= t; char* _temp153= _temp151.curr +( j ++); if(
_temp153 < _temp151.base? 1: _temp153 >= _temp151.last_plus_one){ _throw(
Null_Exception);}* _temp153='\\';}); goto _LL85; _LL103:({ struct _tagged_string
_temp154= t; char* _temp156= _temp154.curr +( j ++); if( _temp156 < _temp154.base?
1: _temp156 >= _temp154.last_plus_one){ _throw( Null_Exception);}* _temp156='\\';});({
struct _tagged_string _temp157= t; char* _temp159= _temp157.curr +( j ++); if(
_temp159 < _temp157.base? 1: _temp159 >= _temp157.last_plus_one){ _throw(
Null_Exception);}* _temp159='"';}); goto _LL85; _LL105: if(( int) _temp84 >=(
int)' '?( int) _temp84 <=( int)'~': 0){({ struct _tagged_string _temp160= t;
char* _temp162= _temp160.curr +( j ++); if( _temp162 < _temp160.base? 1:
_temp162 >= _temp160.last_plus_one){ _throw( Null_Exception);}* _temp162=
_temp84;});} else{({ struct _tagged_string _temp163= t; char* _temp165= _temp163.curr
+( j ++); if( _temp165 < _temp163.base? 1: _temp165 >= _temp163.last_plus_one){
_throw( Null_Exception);}* _temp165='\\';});({ struct _tagged_string _temp166= t;
char* _temp168= _temp166.curr +( j ++); if( _temp168 < _temp166.base? 1:
_temp168 >= _temp166.last_plus_one){ _throw( Null_Exception);}* _temp168=( char)((
int)'0' +(( int) _temp84 >> 6 & 7));});({ struct _tagged_string _temp169= t;
char* _temp171= _temp169.curr +( j ++); if( _temp171 < _temp169.base? 1:
_temp171 >= _temp169.last_plus_one){ _throw( Null_Exception);}* _temp171=( char)((
int)'0' +(( int) _temp84 >> 3 & 7));});({ struct _tagged_string _temp172= t;
char* _temp174= _temp172.curr +( j ++); if( _temp174 < _temp172.base? 1:
_temp174 >= _temp172.last_plus_one){ _throw( Null_Exception);}* _temp174=( char)((
int)'0' +(( int) _temp84 & 7));});} goto _LL85; _LL85:;}} return t;}}}} static
struct Cyc_PP_Doc* Cyc_Absynpp_textptr( struct _tagged_string* s){ return Cyc_PP_text(*
s);} static char _temp177[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string=(
struct _tagged_string){ _temp177, _temp177, _temp177 + 9u}; static char _temp180[
9u]="volatile"; static struct _tagged_string Cyc_Absynpp_volatile_string=(
struct _tagged_string){ _temp180, _temp180, _temp180 + 9u}; static char _temp183[
6u]="const"; static struct _tagged_string Cyc_Absynpp_const_string=( struct
_tagged_string){ _temp183, _temp183, _temp183 + 6u}; static struct
_tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string; static
struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_string;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual* tq){ struct
Cyc_List_List* l= 0; if( tq->q_restrict){ l=({ struct Cyc_List_List* _temp184=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp184->hd=(
void*) Cyc_Absynpp_restrict_sp; _temp184->tl= l; _temp184;});} if( tq->q_volatile){
l=({ struct Cyc_List_List* _temp185=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp185->hd=( void*) Cyc_Absynpp_volatile_sp; _temp185->tl=
l; _temp185;});} if( tq->q_const){ l=({ struct Cyc_List_List* _temp186=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp186->hd=( void*)
Cyc_Absynpp_const_sp; _temp186->tl= l; _temp186;});} return Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp187=( char*)""; struct _tagged_string
_temp188; _temp188.curr= _temp187; _temp188.base= _temp187; _temp188.last_plus_one=
_temp187 + 1; _temp188;}),( struct _tagged_string)({ char* _temp189=( char*)" ";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 2; _temp190;}),( struct _tagged_string)({
char* _temp191=( char*)" "; struct _tagged_string _temp192; _temp192.curr=
_temp191; _temp192.base= _temp191; _temp192.last_plus_one= _temp191 + 2;
_temp192;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){ void* _temp193= k;
_LL195: if( _temp193 == Cyc_Absyn_AnyKind){ goto _LL196;} else{ goto _LL197;}
_LL197: if( _temp193 == Cyc_Absyn_MemKind){ goto _LL198;} else{ goto _LL199;}
_LL199: if( _temp193 == Cyc_Absyn_BoxKind){ goto _LL200;} else{ goto _LL201;}
_LL201: if( _temp193 == Cyc_Absyn_RgnKind){ goto _LL202;} else{ goto _LL203;}
_LL203: if( _temp193 == Cyc_Absyn_EffKind){ goto _LL204;} else{ goto _LL194;}
_LL196: return( struct _tagged_string)({ char* _temp205=( char*)"A"; struct
_tagged_string _temp206; _temp206.curr= _temp205; _temp206.base= _temp205;
_temp206.last_plus_one= _temp205 + 2; _temp206;}); _LL198: return( struct
_tagged_string)({ char* _temp207=( char*)"M"; struct _tagged_string _temp208;
_temp208.curr= _temp207; _temp208.base= _temp207; _temp208.last_plus_one=
_temp207 + 2; _temp208;}); _LL200: return( struct _tagged_string)({ char*
_temp209=( char*)"B"; struct _tagged_string _temp210; _temp210.curr= _temp209;
_temp210.base= _temp209; _temp210.last_plus_one= _temp209 + 2; _temp210;});
_LL202: return( struct _tagged_string)({ char* _temp211=( char*)"R"; struct
_tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 2; _temp212;}); _LL204: return( struct
_tagged_string)({ char* _temp213=( char*)"E"; struct _tagged_string _temp214;
_temp214.curr= _temp213; _temp214.base= _temp213; _temp214.last_plus_one=
_temp213 + 2; _temp214;}); _LL194:;} struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref* c){ void* _temp215=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c))->v; void* _temp221;
_LL217: if(( unsigned int) _temp215 > 1u?(( struct _enum_struct*) _temp215)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL222: _temp221=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp215)->f1; goto _LL218;} else{ goto _LL219;} _LL219: goto _LL220; _LL218:
return Cyc_Absynpp_kind2string( _temp221); _LL220: return( struct _tagged_string)({
char* _temp223=( char*)"?"; struct _tagged_string _temp224; _temp224.curr=
_temp223; _temp224.base= _temp223; _temp224.last_plus_one= _temp223 + 2;
_temp224;}); _LL216:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return
Cyc_PP_text( Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc(
struct Cyc_Absyn_Conref* c){ return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));}
struct Cyc_PP_Doc* Cyc_Absynpp_group( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seq( sep, ss), Cyc_PP_text(
stop));} struct Cyc_PP_Doc* Cyc_Absynpp_egroup( struct _tagged_string start,
struct _tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
if( ss == 0){ return Cyc_PP_nil_doc();} else{ return Cyc_Absynpp_cat3( Cyc_PP_text(
start), Cyc_PP_seq( sep, ss), Cyc_PP_text( stop));}} struct Cyc_PP_Doc* Cyc_Absynpp_groupl(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ return Cyc_Absynpp_cat3( Cyc_PP_text( start),
Cyc_PP_seql( sep, ss), Cyc_PP_text( stop));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_Absynpp_egroup(( struct _tagged_string)({
char* _temp225=( char*)"<"; struct _tagged_string _temp226; _temp226.curr=
_temp225; _temp226.base= _temp225; _temp226.last_plus_one= _temp225 + 2;
_temp226;}),( struct _tagged_string)({ char* _temp227=( char*)">"; struct
_tagged_string _temp228; _temp228.curr= _temp227; _temp228.base= _temp227;
_temp228.last_plus_one= _temp227 + 2; _temp228;}),( struct _tagged_string)({
char* _temp229=( char*)","; struct _tagged_string _temp230; _temp230.curr=
_temp229; _temp230.base= _temp229; _temp230.last_plus_one= _temp229 + 2;
_temp230;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void* _temp231=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp239; void* _temp241; _LL233: if(( unsigned int)
_temp231 > 1u?(( struct _enum_struct*) _temp231)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL240: _temp239=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp231)->f1;
if( _temp239 == Cyc_Absyn_BoxKind){ goto _LL234;} else{ goto _LL235;}} else{
goto _LL235;} _LL235: if(( unsigned int) _temp231 > 1u?(( struct _enum_struct*)
_temp231)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL242: _temp241=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp231)->f1; goto _LL236;} else{ goto
_LL237;} _LL237: goto _LL238; _LL234: return Cyc_Absynpp_textptr( tv->name);
_LL236: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp243=( char*)"::"; struct _tagged_string
_temp244; _temp244.curr= _temp243; _temp244.base= _temp243; _temp244.last_plus_one=
_temp243 + 3; _temp244;})), Cyc_Absynpp_kind2doc( _temp241)); _LL238: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp245=( char*)"/*::?*/"; struct _tagged_string _temp246; _temp246.curr=
_temp245; _temp246.base= _temp245; _temp246.last_plus_one= _temp245 + 8;
_temp246;}))); _LL232:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List*
tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp247=( char*)"<";
struct _tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 2; _temp248;}),( struct _tagged_string)({
char* _temp249=( char*)">"; struct _tagged_string _temp250; _temp250.curr=
_temp249; _temp250.base= _temp249; _temp250.last_plus_one= _temp249 + 2;
_temp250;}),( struct _tagged_string)({ char* _temp251=( char*)","; struct
_tagged_string _temp252; _temp252.curr= _temp251; _temp252.base= _temp251;
_temp252.last_plus_one= _temp251 + 2; _temp252;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp253=(
char*)"<"; struct _tagged_string _temp254; _temp254.curr= _temp253; _temp254.base=
_temp253; _temp254.last_plus_one= _temp253 + 2; _temp254;}),( struct
_tagged_string)({ char* _temp255=( char*)">"; struct _tagged_string _temp256;
_temp256.curr= _temp255; _temp256.base= _temp255; _temp256.last_plus_one=
_temp255 + 2; _temp256;}),( struct _tagged_string)({ char* _temp257=( char*)",";
struct _tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 2; _temp258;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp259=( char*)"(";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 2; _temp260;}),( struct _tagged_string)({
char* _temp261=( char*)")"; struct _tagged_string _temp262; _temp262.curr=
_temp261; _temp262.base= _temp261; _temp262.last_plus_one= _temp261 + 2;
_temp262;}),( struct _tagged_string)({ char* _temp263=( char*)","; struct
_tagged_string _temp264; _temp264.curr= _temp263; _temp264.base= _temp263;
_temp264.last_plus_one= _temp263 + 2; _temp264;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){
return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc(
struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();} return
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp265=( char*)" __attribute__";
struct _tagged_string _temp266; _temp266.curr= _temp265; _temp266.base= _temp265;
_temp266.last_plus_one= _temp265 + 15; _temp266;})), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp267=( char*)"(("; struct _tagged_string _temp268;
_temp268.curr= _temp267; _temp268.base= _temp267; _temp268.last_plus_one=
_temp267 + 3; _temp268;}),( struct _tagged_string)({ char* _temp269=( char*)"))";
struct _tagged_string _temp270; _temp270.curr= _temp269; _temp270.base= _temp269;
_temp270.last_plus_one= _temp269 + 3; _temp270;}),( struct _tagged_string)({
char* _temp271=( char*)","; struct _tagged_string _temp272; _temp272.curr=
_temp271; _temp272.base= _temp271; _temp272.last_plus_one= _temp271 + 2;
_temp272;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)));} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp273=( void*)
tms->hd; struct Cyc_Absyn_Tqual* _temp279; void* _temp281; void* _temp283;
_LL275: if(( unsigned int) _temp273 > 1u?(( struct _enum_struct*) _temp273)->tag
== Cyc_Absyn_Pointer_mod_tag: 0){ _LL284: _temp283=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp273)->f1; goto _LL282; _LL282: _temp281=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp273)->f2; goto _LL280; _LL280: _temp279=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp273)->f3; goto _LL276;} else{ goto _LL277;}
_LL277: goto _LL278; _LL276: return 1; _LL278: return 0; _LL274:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d, tms->tl); struct Cyc_PP_Doc*
p_rest= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp285=(
char*)"("; struct _tagged_string _temp286; _temp286.curr= _temp285; _temp286.base=
_temp285; _temp286.last_plus_one= _temp285 + 2; _temp286;})), rest, Cyc_PP_text((
struct _tagged_string)({ char* _temp287=( char*)")"; struct _tagged_string
_temp288; _temp288.curr= _temp287; _temp288.base= _temp287; _temp288.last_plus_one=
_temp287 + 2; _temp288;}))); void* _temp289=( void*) tms->hd; struct Cyc_Absyn_Exp*
_temp303; void* _temp305; struct Cyc_List_List* _temp307; struct Cyc_Position_Segment*
_temp309; int _temp311; struct Cyc_Position_Segment* _temp313; struct Cyc_List_List*
_temp315; struct Cyc_Absyn_Tqual* _temp317; void* _temp319; void* _temp321;
_LL291: if( _temp289 == Cyc_Absyn_Carray_mod){ goto _LL292;} else{ goto _LL293;}
_LL293: if(( unsigned int) _temp289 > 1u?(( struct _enum_struct*) _temp289)->tag
== Cyc_Absyn_ConstArray_mod_tag: 0){ _LL304: _temp303=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ConstArray_mod_struct*) _temp289)->f1; goto _LL294;} else{ goto
_LL295;} _LL295: if(( unsigned int) _temp289 > 1u?(( struct _enum_struct*)
_temp289)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL306: _temp305=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp289)->f1; goto _LL296;} else{ goto
_LL297;} _LL297: if(( unsigned int) _temp289 > 1u?(( struct _enum_struct*)
_temp289)->tag == Cyc_Absyn_Attributes_mod_tag: 0){ _LL310: _temp309=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*) _temp289)->f1;
goto _LL308; _LL308: _temp307=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp289)->f2; goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int)
_temp289 > 1u?(( struct _enum_struct*) _temp289)->tag == Cyc_Absyn_TypeParams_mod_tag:
0){ _LL316: _temp315=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp289)->f1; goto _LL314; _LL314: _temp313=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp289)->f2; goto _LL312; _LL312:
_temp311=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp289)->f3; goto
_LL300;} else{ goto _LL301;} _LL301: if(( unsigned int) _temp289 > 1u?(( struct
_enum_struct*) _temp289)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL322: _temp321=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp289)->f1; goto _LL320;
_LL320: _temp319=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp289)->f2;
goto _LL318; _LL318: _temp317=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp289)->f3; goto _LL302;} else{ goto _LL290;} _LL292: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp323=( char*)"[]"; struct _tagged_string _temp324;
_temp324.curr= _temp323; _temp324.base= _temp323; _temp324.last_plus_one=
_temp323 + 3; _temp324;}))); _LL294: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;} return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp325=( char*)"["; struct _tagged_string _temp326;
_temp326.curr= _temp325; _temp326.base= _temp325; _temp326.last_plus_one=
_temp325 + 2; _temp326;})), Cyc_Absynpp_exp2doc( _temp303), Cyc_PP_text(( struct
_tagged_string)({ char* _temp327=( char*)"]"; struct _tagged_string _temp328;
_temp328.curr= _temp327; _temp328.base= _temp327; _temp328.last_plus_one=
_temp327 + 2; _temp328;}))); _LL296: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;}{ void* _temp329= _temp305; struct Cyc_Core_Opt* _temp335; int
_temp337; struct Cyc_List_List* _temp339; struct Cyc_Position_Segment* _temp341;
struct Cyc_List_List* _temp343; _LL331: if((( struct _enum_struct*) _temp329)->tag
== Cyc_Absyn_WithTypes_tag){ _LL340: _temp339=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp329)->f1; goto _LL338; _LL338: _temp337=( int)((
struct Cyc_Absyn_WithTypes_struct*) _temp329)->f2; goto _LL336; _LL336: _temp335=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp329)->f3; goto
_LL332;} else{ goto _LL333;} _LL333: if((( struct _enum_struct*) _temp329)->tag
== Cyc_Absyn_NoTypes_tag){ _LL344: _temp343=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp329)->f1; goto _LL342; _LL342: _temp341=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp329)->f2; goto
_LL334;} else{ goto _LL330;} _LL332: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp339, _temp337, _temp335)); _LL334: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp345=( char*)"("; struct _tagged_string
_temp346; _temp346.curr= _temp345; _temp346.base= _temp345; _temp346.last_plus_one=
_temp345 + 2; _temp346;}),( struct _tagged_string)({ char* _temp347=( char*)")";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 2; _temp348;}),( struct _tagged_string)({
char* _temp349=( char*)","; struct _tagged_string _temp350; _temp350.curr=
_temp349; _temp350.base= _temp349; _temp350.last_plus_one= _temp349 + 2;
_temp350;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp343))); _LL330:;} _LL298: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest=
p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc( _temp307)); _LL300:
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} if( _temp311){ return
Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc( _temp315));} else{ return Cyc_Absynpp_cat2(
rest, Cyc_Absynpp_tvars2doc( _temp315));} _LL302: { struct Cyc_PP_Doc* ptr;{
void* _temp351= _temp321; struct Cyc_Absyn_Exp* _temp359; struct Cyc_Absyn_Exp*
_temp361; _LL353: if(( unsigned int) _temp351 > 1u?(( struct _enum_struct*)
_temp351)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL360: _temp359=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Nullable_ps_struct*) _temp351)->f1; goto _LL354;} else{ goto
_LL355;} _LL355: if(( unsigned int) _temp351 > 1u?(( struct _enum_struct*)
_temp351)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL362: _temp361=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp351)->f1; goto
_LL356;} else{ goto _LL357;} _LL357: if( _temp351 == Cyc_Absyn_TaggedArray_ps){
goto _LL358;} else{ goto _LL352;} _LL354: if( Cyc_Evexp_eval_const_uint_exp(
_temp359) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp363=( char*)"*"; struct _tagged_string _temp364; _temp364.curr= _temp363;
_temp364.base= _temp363; _temp364.last_plus_one= _temp363 + 2; _temp364;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp365=( char*)"*"; struct _tagged_string _temp366; _temp366.curr= _temp365;
_temp366.base= _temp365; _temp366.last_plus_one= _temp365 + 2; _temp366;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp367=( char*)"{"; struct _tagged_string
_temp368; _temp368.curr= _temp367; _temp368.base= _temp367; _temp368.last_plus_one=
_temp367 + 2; _temp368;})), Cyc_Absynpp_exp2doc( _temp359), Cyc_PP_text(( struct
_tagged_string)({ char* _temp369=( char*)"}"; struct _tagged_string _temp370;
_temp370.curr= _temp369; _temp370.base= _temp369; _temp370.last_plus_one=
_temp369 + 2; _temp370;})));} goto _LL352; _LL356: if( Cyc_Evexp_eval_const_uint_exp(
_temp361) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp371=( char*)"@"; struct _tagged_string _temp372; _temp372.curr= _temp371;
_temp372.base= _temp371; _temp372.last_plus_one= _temp371 + 2; _temp372;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp373=( char*)"@"; struct _tagged_string _temp374; _temp374.curr= _temp373;
_temp374.base= _temp373; _temp374.last_plus_one= _temp373 + 2; _temp374;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp375=( char*)"{"; struct _tagged_string
_temp376; _temp376.curr= _temp375; _temp376.base= _temp375; _temp376.last_plus_one=
_temp375 + 2; _temp376;})), Cyc_Absynpp_exp2doc( _temp361), Cyc_PP_text(( struct
_tagged_string)({ char* _temp377=( char*)"}"; struct _tagged_string _temp378;
_temp378.curr= _temp377; _temp378.base= _temp377; _temp378.last_plus_one=
_temp377 + 2; _temp378;})));} goto _LL352; _LL358: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp379=( char*)"?"; struct _tagged_string _temp380;
_temp380.curr= _temp379; _temp380.base= _temp379; _temp380.last_plus_one=
_temp379 + 2; _temp380;})); goto _LL352; _LL352:;}{ void* _temp381= Cyc_Tcutil_compress(
_temp319); _LL383: if( _temp381 == Cyc_Absyn_HeapRgn){ goto _LL384;} else{ goto
_LL385;} _LL385: goto _LL386; _LL384: return Cyc_Absynpp_cat2( ptr, rest);
_LL386: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp319), Cyc_PP_text((
struct _tagged_string)({ char* _temp387=( char*)" "; struct _tagged_string
_temp388; _temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 2; _temp388;})), rest); _LL382:;}} _LL290:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc(
void* t){ void* _temp389= Cyc_Tcutil_compress( t); _LL391: if( _temp389 == Cyc_Absyn_HeapRgn){
goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL392: return Cyc_PP_text((
struct _tagged_string)({ char* _temp395=( char*)"`H"; struct _tagged_string
_temp396; _temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 3; _temp396;})); _LL394: return Cyc_Absynpp_ntyp2doc( t); _LL390:;}
static void Cyc_Absynpp_effects2docs( struct Cyc_List_List** regions, struct Cyc_List_List**
effects, void* t){ void* _temp397= Cyc_Tcutil_compress( t); void* _temp405;
struct Cyc_List_List* _temp407; _LL399: if(( unsigned int) _temp397 > 4u?((
struct _enum_struct*) _temp397)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL406:
_temp405=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp397)->f1; goto
_LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp397 > 4u?(( struct
_enum_struct*) _temp397)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL408: _temp407=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp397)->f1; goto
_LL402;} else{ goto _LL403;} _LL403: goto _LL404; _LL400:* regions=({ struct Cyc_List_List*
_temp409=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp409->hd=( void*) Cyc_Absynpp_rgn2doc( _temp405); _temp409->tl=* regions;
_temp409;}); goto _LL398; _LL402: for( 0; _temp407 != 0; _temp407= _temp407->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*) _temp407->hd);} goto _LL398;
_LL404:* effects=({ struct Cyc_List_List* _temp410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp410->hd=( void*) Cyc_Absynpp_ntyp2doc(
t); _temp410->tl=* effects; _temp410;}); goto _LL398; _LL398:;} struct Cyc_PP_Doc*
Cyc_Absynpp_eff2doc( void* t){ struct Cyc_List_List* regions= 0; struct Cyc_List_List*
effects= 0; Cyc_Absynpp_effects2docs(& regions,& effects, t); regions=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( regions);
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp411=( char*)""; struct _tagged_string _temp412;
_temp412.curr= _temp411; _temp412.base= _temp411; _temp412.last_plus_one=
_temp411 + 1; _temp412;}),( struct _tagged_string)({ char* _temp413=( char*)"";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 1; _temp414;}),( struct _tagged_string)({
char* _temp415=( char*)"+"; struct _tagged_string _temp416; _temp416.curr=
_temp415; _temp416.base= _temp415; _temp416.last_plus_one= _temp415 + 2;
_temp416;}), effects);} else{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp417=( char*)"{"; struct _tagged_string
_temp418; _temp418.curr= _temp417; _temp418.base= _temp417; _temp418.last_plus_one=
_temp417 + 2; _temp418;}),( struct _tagged_string)({ char* _temp419=( char*)"}";
struct _tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 2; _temp420;}),( struct _tagged_string)({
char* _temp421=( char*)","; struct _tagged_string _temp422; _temp422.curr=
_temp421; _temp422.base= _temp421; _temp422.last_plus_one= _temp421 + 2;
_temp422;}), regions); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp423=( char*)""; struct _tagged_string _temp424; _temp424.curr= _temp423;
_temp424.base= _temp423; _temp424.last_plus_one= _temp423 + 1; _temp424;}),(
struct _tagged_string)({ char* _temp425=( char*)""; struct _tagged_string
_temp426; _temp426.curr= _temp425; _temp426.base= _temp425; _temp426.last_plus_one=
_temp425 + 1; _temp426;}),( struct _tagged_string)({ char* _temp427=( char*)"+";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 2; _temp428;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp429=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp429->hd=( void*) doc1; _temp429->tl= 0; _temp429;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void*
_temp430= t; struct Cyc_Absyn_Exp* _temp470; struct Cyc_Absyn_Tqual* _temp472;
void* _temp474; struct Cyc_Absyn_FnInfo _temp476; struct Cyc_Absyn_PtrInfo
_temp478; int _temp480; struct Cyc_Core_Opt* _temp482; void* _temp484; struct
Cyc_Absyn_Tvar* _temp486; struct Cyc_Absyn_Enumdecl** _temp488; struct Cyc_List_List*
_temp490; struct _tuple0* _temp492; struct Cyc_Absyn_Xenumdecl** _temp494;
struct _tuple0* _temp496; void* _temp498; void* _temp500; struct Cyc_List_List*
_temp502; struct Cyc_Absyn_Structdecl** _temp504; struct Cyc_List_List* _temp506;
struct _tuple0* _temp508; struct Cyc_Absyn_Uniondecl** _temp510; struct Cyc_List_List*
_temp512; struct _tuple0* _temp514; struct Cyc_Core_Opt* _temp516; struct Cyc_List_List*
_temp518; struct _tuple0* _temp520; void* _temp522; void* _temp524; struct Cyc_List_List*
_temp526; _LL432: if(( unsigned int) _temp430 > 4u?(( struct _enum_struct*)
_temp430)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL475: _temp474=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp430)->f1; goto _LL473; _LL473: _temp472=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp430)->f2;
goto _LL471; _LL471: _temp470=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp430)->f3; goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp430 > 4u?(( struct _enum_struct*) _temp430)->tag == Cyc_Absyn_FnType_tag: 0){
_LL477: _temp476=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp430)->f1; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp430 > 4u?(( struct _enum_struct*) _temp430)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL479: _temp478=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp430)->f1; goto _LL437;} else{ goto _LL438;} _LL438: if( _temp430 == Cyc_Absyn_VoidType){
goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_Evar_tag: 0){ _LL485: _temp484=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp430)->f1; goto _LL483; _LL483:
_temp482=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp430)->f2;
goto _LL481; _LL481: _temp480=( int)(( struct Cyc_Absyn_Evar_struct*) _temp430)->f3;
goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_VarType_tag: 0){ _LL487:
_temp486=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp430)->f1;
goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL493:
_temp492=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp430)->f1;
goto _LL491; _LL491: _temp490=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp430)->f2; goto _LL489; _LL489: _temp488=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp430)->f3; goto _LL445;} else{ goto
_LL446;} _LL446: if(( unsigned int) _temp430 > 4u?(( struct _enum_struct*)
_temp430)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL497: _temp496=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp430)->f1; goto _LL495; _LL495: _temp494=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp430)->f2;
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_IntType_tag: 0){ _LL501:
_temp500=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp430)->f1; goto _LL499;
_LL499: _temp498=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp430)->f2;
goto _LL449;} else{ goto _LL450;} _LL450: if( _temp430 == Cyc_Absyn_FloatType){
goto _LL451;} else{ goto _LL452;} _LL452: if( _temp430 == Cyc_Absyn_DoubleType){
goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL503:
_temp502=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp430)->f1;
goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_StructType_tag: 0){ _LL509:
_temp508=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp430)->f1;
goto _LL507; _LL507: _temp506=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp430)->f2; goto _LL505; _LL505: _temp504=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp430)->f3; goto _LL457;} else{ goto
_LL458;} _LL458: if(( unsigned int) _temp430 > 4u?(( struct _enum_struct*)
_temp430)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL515: _temp514=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp430)->f1; goto _LL513; _LL513: _temp512=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp430)->f2; goto
_LL511; _LL511: _temp510=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp430)->f3; goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int)
_temp430 > 4u?(( struct _enum_struct*) _temp430)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL521: _temp520=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp430)->f1; goto _LL519; _LL519: _temp518=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp430)->f2; goto _LL517; _LL517: _temp516=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp430)->f3;
goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp430 > 4u?((
struct _enum_struct*) _temp430)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL523:
_temp522=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp430)->f1; goto
_LL463;} else{ goto _LL464;} _LL464: if( _temp430 == Cyc_Absyn_HeapRgn){ goto
_LL465;} else{ goto _LL466;} _LL466: if(( unsigned int) _temp430 > 4u?(( struct
_enum_struct*) _temp430)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL525: _temp524=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp430)->f1; goto _LL467;} else{
goto _LL468;} _LL468: if(( unsigned int) _temp430 > 4u?(( struct _enum_struct*)
_temp430)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL527: _temp526=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp430)->f1; goto _LL469;} else{ goto _LL431;}
_LL433: return Cyc_PP_text(( struct _tagged_string)({ char* _temp528=( char*)"[[[array]]]";
struct _tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 12; _temp529;})); _LL435: return Cyc_PP_nil_doc();
_LL437: return Cyc_PP_nil_doc(); _LL439: s= Cyc_PP_text(( struct _tagged_string)({
char* _temp530=( char*)"void"; struct _tagged_string _temp531; _temp531.curr=
_temp530; _temp531.base= _temp530; _temp531.last_plus_one= _temp530 + 5;
_temp531;})); goto _LL431; _LL441: if( _temp482 != 0){ return Cyc_Absynpp_ntyp2doc((
void*) _temp482->v);} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp532=( char*)"%"; struct _tagged_string _temp533;
_temp533.curr= _temp532; _temp533.base= _temp532; _temp533.last_plus_one=
_temp532 + 2; _temp533;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp534=( char*)"("; struct _tagged_string _temp535; _temp535.curr= _temp534;
_temp535.base= _temp534; _temp535.last_plus_one= _temp534 + 2; _temp535;})), Cyc_PP_text(
xprintf("%d", _temp480)), Cyc_PP_text(( struct _tagged_string)({ char* _temp536=(
char*)")::"; struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base=
_temp536; _temp537.last_plus_one= _temp536 + 4; _temp537;})), Cyc_Absynpp_kind2doc(
_temp484));} goto _LL431; _LL443: s= Cyc_Absynpp_textptr( _temp486->name); if(
Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct
_tagged_string)({ char* _temp538=( char*)"::"; struct _tagged_string _temp539;
_temp539.curr= _temp538; _temp539.base= _temp538; _temp539.last_plus_one=
_temp538 + 3; _temp539;})), Cyc_Absynpp_ckind2doc( _temp486->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp486): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp540=( char*)"_ /* "; struct _tagged_string
_temp541; _temp541.curr= _temp540; _temp541.base= _temp540; _temp541.last_plus_one=
_temp540 + 6; _temp541;})), s, Cyc_PP_text(( struct _tagged_string)({ char*
_temp542=( char*)" */"; struct _tagged_string _temp543; _temp543.curr= _temp542;
_temp543.base= _temp542; _temp543.last_plus_one= _temp542 + 4; _temp543;})));}
goto _LL431; _LL445: if( _temp492 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp544=( char*)"enum "; struct _tagged_string
_temp545; _temp545.curr= _temp544; _temp545.base= _temp544; _temp545.last_plus_one=
_temp544 + 6; _temp545;})), Cyc_Absynpp_tps2doc( _temp490));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp546=( char*)"enum "; struct
_tagged_string _temp547; _temp547.curr= _temp546; _temp547.base= _temp546;
_temp547.last_plus_one= _temp546 + 6; _temp547;})), Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct _tuple0* _temp548= _temp492; if( _temp548 == 0){
_throw( Null_Exception);} _temp548;})), Cyc_Absynpp_tps2doc( _temp490));} goto
_LL431; _LL447: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp549=( char*)"xenum "; struct _tagged_string _temp550; _temp550.curr=
_temp549; _temp550.base= _temp549; _temp550.last_plus_one= _temp549 + 7;
_temp550;})), Cyc_Absynpp_qvar2doc( _temp496)); goto _LL431; _LL449: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp551= _temp500; _LL553:
if( _temp551 == Cyc_Absyn_Signed){ goto _LL554;} else{ goto _LL555;} _LL555: if(
_temp551 == Cyc_Absyn_Unsigned){ goto _LL556;} else{ goto _LL552;} _LL554: sns=(
struct _tagged_string)({ char* _temp557=( char*)""; struct _tagged_string
_temp558; _temp558.curr= _temp557; _temp558.base= _temp557; _temp558.last_plus_one=
_temp557 + 1; _temp558;}); goto _LL552; _LL556: sns=( struct _tagged_string)({
char* _temp559=( char*)"unsigned "; struct _tagged_string _temp560; _temp560.curr=
_temp559; _temp560.base= _temp559; _temp560.last_plus_one= _temp559 + 10;
_temp560;}); goto _LL552; _LL552:;}{ void* _temp561= _temp498; _LL563: if(
_temp561 == Cyc_Absyn_B1){ goto _LL564;} else{ goto _LL565;} _LL565: if(
_temp561 == Cyc_Absyn_B2){ goto _LL566;} else{ goto _LL567;} _LL567: if(
_temp561 == Cyc_Absyn_B4){ goto _LL568;} else{ goto _LL569;} _LL569: if(
_temp561 == Cyc_Absyn_B8){ goto _LL570;} else{ goto _LL562;} _LL564:{ void*
_temp571= _temp500; _LL573: if( _temp571 == Cyc_Absyn_Signed){ goto _LL574;}
else{ goto _LL575;} _LL575: if( _temp571 == Cyc_Absyn_Unsigned){ goto _LL576;}
else{ goto _LL572;} _LL574: sns=( struct _tagged_string)({ char* _temp577=( char*)"signed ";
struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 8; _temp578;}); goto _LL572; _LL576: sns=(
struct _tagged_string)({ char* _temp579=( char*)""; struct _tagged_string
_temp580; _temp580.curr= _temp579; _temp580.base= _temp579; _temp580.last_plus_one=
_temp579 + 1; _temp580;}); goto _LL572; _LL572:;} ts=( struct _tagged_string)({
char* _temp581=( char*)"char"; struct _tagged_string _temp582; _temp582.curr=
_temp581; _temp582.base= _temp581; _temp582.last_plus_one= _temp581 + 5;
_temp582;}); goto _LL562; _LL566: ts=( struct _tagged_string)({ char* _temp583=(
char*)"short"; struct _tagged_string _temp584; _temp584.curr= _temp583; _temp584.base=
_temp583; _temp584.last_plus_one= _temp583 + 6; _temp584;}); goto _LL562; _LL568:
ts=( struct _tagged_string)({ char* _temp585=( char*)"int"; struct
_tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 4; _temp586;}); goto _LL562; _LL570: ts=(
struct _tagged_string)({ char* _temp587=( char*)"long long"; struct
_tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 10; _temp588;}); goto _LL562; _LL562:;} s=
Cyc_PP_text(({ struct _tagged_string _temp589= sns; struct _tagged_string
_temp590= ts; xprintf("%.*s%.*s", _temp589.last_plus_one - _temp589.curr,
_temp589.curr, _temp590.last_plus_one - _temp590.curr, _temp590.curr);})); goto
_LL431;} _LL451: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp591=( char*)"float";
struct _tagged_string _temp592; _temp592.curr= _temp591; _temp592.base= _temp591;
_temp592.last_plus_one= _temp591 + 6; _temp592;})); goto _LL431; _LL453: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp593=( char*)"double"; struct _tagged_string
_temp594; _temp594.curr= _temp593; _temp594.base= _temp593; _temp594.last_plus_one=
_temp593 + 7; _temp594;})); goto _LL431; _LL455: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp595=( char*)"$"; struct _tagged_string
_temp596; _temp596.curr= _temp595; _temp596.base= _temp595; _temp596.last_plus_one=
_temp595 + 2; _temp596;})), Cyc_Absynpp_args2doc( _temp502)); goto _LL431;
_LL457: if( _temp508 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp597=( char*)"struct "; struct _tagged_string
_temp598; _temp598.curr= _temp597; _temp598.base= _temp597; _temp598.last_plus_one=
_temp597 + 8; _temp598;})), Cyc_Absynpp_tps2doc( _temp506));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp599=( char*)"struct "; struct
_tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 8; _temp600;})), _temp508 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp601= _temp508;
if( _temp601 == 0){ _throw( Null_Exception);} _temp601;})), Cyc_Absynpp_tps2doc(
_temp506));} goto _LL431; _LL459: if( _temp514 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp602=( char*)"union "; struct _tagged_string
_temp603; _temp603.curr= _temp602; _temp603.base= _temp602; _temp603.last_plus_one=
_temp602 + 7; _temp603;})), Cyc_Absynpp_tps2doc( _temp512));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp604=( char*)"union "; struct
_tagged_string _temp605; _temp605.curr= _temp604; _temp605.base= _temp604;
_temp605.last_plus_one= _temp604 + 7; _temp605;})), _temp514 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp606= _temp514;
if( _temp606 == 0){ _throw( Null_Exception);} _temp606;})), Cyc_Absynpp_tps2doc(
_temp512));} goto _LL431; _LL461: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp520), Cyc_Absynpp_tps2doc( _temp518)); goto _LL431; _LL463: s= Cyc_Absynpp_rgn2doc(
_temp522); goto _LL431; _LL465: s= Cyc_Absynpp_rgn2doc( t); goto _LL431; _LL467:
s= Cyc_Absynpp_eff2doc( t); goto _LL431; _LL469: s= Cyc_Absynpp_eff2doc( t);
goto _LL431; _LL431:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_string*) vo->v));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct
_tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp607=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp607->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)((* t).f1)->v));
_temp607;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp608=( char*)"("; struct _tagged_string _temp609;
_temp609.curr= _temp608; _temp609.base= _temp608; _temp609.last_plus_one=
_temp608 + 2; _temp609;}),( struct _tagged_string)({ char* _temp610=( char*)")";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 2; _temp611;}),( struct _tagged_string)({
char* _temp612=( char*)","; struct _tagged_string _temp613; _temp613.curr=
_temp612; _temp613.base= _temp612; _temp613.last_plus_one= _temp612 + 2;
_temp613;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp614=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp614->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp615=( char*)"..."; struct _tagged_string _temp616;
_temp616.curr= _temp615; _temp616.base= _temp615; _temp616.last_plus_one=
_temp615 + 4; _temp616;})); _temp614->tl= 0; _temp614;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp617=( char*)"("; struct _tagged_string
_temp618; _temp618.curr= _temp617; _temp618.base= _temp617; _temp618.last_plus_one=
_temp617 + 2; _temp618;}),( struct _tagged_string)({ char* _temp619=( char*)")";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 2; _temp620;}),( struct _tagged_string)({
char* _temp621=( char*)","; struct _tagged_string _temp622; _temp622.curr=
_temp621; _temp622.base= _temp621; _temp622.last_plus_one= _temp621 + 2;
_temp622;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp623=(
char*)"("; struct _tagged_string _temp624; _temp624.curr= _temp623; _temp624.base=
_temp623; _temp624.last_plus_one= _temp623 + 2; _temp624;}),( struct
_tagged_string)({ char* _temp625=( char*)")"; struct _tagged_string _temp626;
_temp626.curr= _temp625; _temp626.base= _temp625; _temp626.last_plus_one=
_temp625 + 2; _temp626;}),( struct _tagged_string)({ char* _temp627=( char*)";";
struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base= _temp627;
_temp628.last_plus_one= _temp627 + 2; _temp628;}),({ struct Cyc_List_List*
_temp629=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp629->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp631=( char*)""; struct _tagged_string _temp632; _temp632.curr= _temp631;
_temp632.base= _temp631; _temp632.last_plus_one= _temp631 + 1; _temp632;}),(
struct _tagged_string)({ char* _temp633=( char*)""; struct _tagged_string
_temp634; _temp634.curr= _temp633; _temp634.base= _temp633; _temp634.last_plus_one=
_temp633 + 1; _temp634;}),( struct _tagged_string)({ char* _temp635=( char*)",";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 2; _temp636;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp637=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp637->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp638=(
char*)"..."; struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base=
_temp638; _temp639.last_plus_one= _temp638 + 4; _temp639;})); _temp637->tl= 0;
_temp637;}))); _temp629->tl=({ struct Cyc_List_List* _temp630=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp630->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp630->tl= 0; _temp630;}); _temp629;}));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp640=( char*)"("; struct _tagged_string
_temp641; _temp641.curr= _temp640; _temp641.base= _temp640; _temp641.last_plus_one=
_temp640 + 2; _temp641;}),( struct _tagged_string)({ char* _temp642=( char*)")";
struct _tagged_string _temp643; _temp643.curr= _temp642; _temp643.base= _temp642;
_temp643.last_plus_one= _temp642 + 2; _temp643;}),( struct _tagged_string)({
char* _temp644=( char*)";"; struct _tagged_string _temp645; _temp645.curr=
_temp644; _temp645.base= _temp644; _temp645.last_plus_one= _temp644 + 2;
_temp645;}),({ struct Cyc_List_List* _temp646=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp646->hd=( void*) Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp648=( char*)""; struct _tagged_string _temp649;
_temp649.curr= _temp648; _temp649.base= _temp648; _temp649.last_plus_one=
_temp648 + 1; _temp649;}),( struct _tagged_string)({ char* _temp650=( char*)"";
struct _tagged_string _temp651; _temp651.curr= _temp650; _temp651.base= _temp650;
_temp651.last_plus_one= _temp650 + 1; _temp651;}),( struct _tagged_string)({
char* _temp652=( char*)","; struct _tagged_string _temp653; _temp653.curr=
_temp652; _temp653.base= _temp652; _temp653.last_plus_one= _temp652 + 2;
_temp653;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));
_temp646->tl=({ struct Cyc_List_List* _temp647=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp647->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp647->tl= 0; _temp647;}); _temp646;}));}}} struct _tuple1*
Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({ struct _tuple1* _temp654=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp654->f1=({ struct Cyc_Core_Opt*
_temp655=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp655->v=( void*)(* arg).f1; _temp655;}); _temp654->f2=(* arg).f2; _temp654->f3=(*
arg).f3; _temp654;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* prefix= 0;{ void* _temp656=(* q).f1;
struct Cyc_List_List* _temp668; struct Cyc_List_List* _temp670; struct Cyc_List_List*
_temp672; struct Cyc_List_List* _temp674; _LL658: if( _temp656 == Cyc_Absyn_Loc_n){
goto _LL659;} else{ goto _LL660;} _LL660: if(( unsigned int) _temp656 > 1u?((
struct _enum_struct*) _temp656)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL669:
_temp668=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp656)->f1;
if( _temp668 == 0){ goto _LL661;} else{ goto _LL662;}} else{ goto _LL662;}
_LL662: if(( unsigned int) _temp656 > 1u?(( struct _enum_struct*) _temp656)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL671: _temp670=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp656)->f1; if( _temp670 == 0){ goto _LL663;} else{
goto _LL664;}} else{ goto _LL664;} _LL664: if(( unsigned int) _temp656 > 1u?((
struct _enum_struct*) _temp656)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL673:
_temp672=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp656)->f1;
goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int) _temp656 > 1u?((
struct _enum_struct*) _temp656)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL675:
_temp674=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp656)->f1;
goto _LL667;} else{ goto _LL657;} _LL659: goto _LL661; _LL661: goto _LL663;
_LL663: return Cyc_Absynpp_var2doc((* q).f2); _LL665: _temp674= _temp672; goto
_LL667; _LL667: prefix= _temp674; goto _LL657; _LL657:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp676=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp676->hd=( void*)(* q).f2; _temp676->tl= 0; _temp676;})),( struct
_tagged_string)({ char* _temp677=( char*)"_"; struct _tagged_string _temp678;
_temp678.curr= _temp677; _temp678.base= _temp677; _temp678.last_plus_one=
_temp677 + 2; _temp678;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp679=( char*)""; struct _tagged_string
_temp680; _temp680.curr= _temp679; _temp680.base= _temp679; _temp680.last_plus_one=
_temp679 + 1; _temp680;}),( struct _tagged_string)({ char* _temp681=( char*)"::";
struct _tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 3; _temp682;}),( struct _tagged_string)({
char* _temp683=( char*)"::"; struct _tagged_string _temp684; _temp684.curr=
_temp683; _temp684.base= _temp683; _temp684.last_plus_one= _temp683 + 3;
_temp684;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_var2doc,
prefix)), Cyc_Absynpp_var2doc((* q).f2));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp685=(* v).f1; struct Cyc_List_List* _temp691; _LL687: if((
unsigned int) _temp685 > 1u?(( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL692: _temp691=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp685)->f1; goto _LL688;} else{ goto _LL689;} _LL689: goto _LL690; _LL688:
if((( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp,
_temp691, Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);}
else{ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp693=( char*)"/* wrong namespace : */ "; struct _tagged_string _temp694;
_temp694.curr= _temp693; _temp694.base= _temp693; _temp694.last_plus_one=
_temp693 + 25; _temp694;})), Cyc_Absynpp_qvar2doc( v));} _LL690: return Cyc_Absynpp_qvar2doc(
v); _LL686:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp695=( void*) e->r; void* _temp763; void* _temp765; struct _tuple0*
_temp767; struct _tuple0* _temp769; struct Cyc_List_List* _temp771; void*
_temp773; struct Cyc_Absyn_Exp* _temp775; struct Cyc_Core_Opt* _temp777; struct
Cyc_Absyn_Exp* _temp779; void* _temp781; struct Cyc_Absyn_Exp* _temp783; struct
Cyc_Absyn_Exp* _temp785; struct Cyc_Absyn_Exp* _temp787; struct Cyc_Absyn_Exp*
_temp789; struct Cyc_Absyn_Exp* _temp791; struct Cyc_Absyn_Exp* _temp793; struct
Cyc_List_List* _temp795; struct Cyc_Absyn_Exp* _temp797; struct Cyc_List_List*
_temp799; struct Cyc_Absyn_Exp* _temp801; struct Cyc_Absyn_Exp* _temp803; struct
Cyc_Absyn_Exp* _temp805; struct Cyc_List_List* _temp807; struct Cyc_Absyn_Exp*
_temp809; struct Cyc_Absyn_Exp* _temp811; void* _temp813; struct Cyc_Absyn_Exp*
_temp815; void* _temp817; struct Cyc_Absyn_Exp* _temp819; struct Cyc_Absyn_Exp*
_temp821; struct _tagged_string* _temp823; struct Cyc_Absyn_Exp* _temp825;
struct _tagged_string* _temp827; struct Cyc_Absyn_Exp* _temp829; struct Cyc_Absyn_Exp*
_temp831; struct Cyc_Absyn_Exp* _temp833; struct Cyc_List_List* _temp835; struct
Cyc_List_List* _temp837; struct _tuple1* _temp839; struct Cyc_List_List*
_temp841; int _temp843; struct Cyc_Absyn_Exp* _temp845; struct Cyc_Absyn_Exp*
_temp847; struct Cyc_Absyn_Vardecl* _temp849; struct Cyc_Absyn_Structdecl*
_temp851; struct Cyc_List_List* _temp853; struct Cyc_Core_Opt* _temp855; struct
_tuple0* _temp857; struct Cyc_Absyn_Enumfield* _temp859; struct Cyc_Absyn_Enumdecl*
_temp861; struct Cyc_List_List* _temp863; struct Cyc_Core_Opt* _temp865; struct
Cyc_Core_Opt* _temp867; struct Cyc_Absyn_Enumfield* _temp869; struct Cyc_Absyn_Xenumdecl*
_temp871; struct Cyc_List_List* _temp873; struct Cyc_Core_Opt* _temp875; void*
_temp877; struct Cyc_List_List* _temp879; struct Cyc_Core_Opt* _temp881; struct
Cyc_Absyn_Stmt* _temp883; struct Cyc_Absyn_Fndecl* _temp885; struct Cyc_Absyn_Exp*
_temp887; _LL697: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_Const_e_tag){
_LL764: _temp763=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp695)->f1;
goto _LL698;} else{ goto _LL699;} _LL699: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Var_e_tag){ _LL768: _temp767=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp695)->f1; goto _LL766; _LL766: _temp765=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp695)->f2; goto _LL700;} else{ goto _LL701;} _LL701: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL770: _temp769=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp695)->f1; goto
_LL702;} else{ goto _LL703;} _LL703: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Primop_e_tag){ _LL774: _temp773=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp695)->f1; goto _LL772; _LL772: _temp771=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp695)->f2; goto _LL704;} else{ goto _LL705;}
_LL705: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL780: _temp779=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp695)->f1; goto _LL778; _LL778: _temp777=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp695)->f2; goto _LL776; _LL776: _temp775=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp695)->f3;
goto _LL706;} else{ goto _LL707;} _LL707: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Increment_e_tag){ _LL784: _temp783=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp695)->f1; goto _LL782; _LL782:
_temp781=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp695)->f2; goto
_LL708;} else{ goto _LL709;} _LL709: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL790: _temp789=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp695)->f1; goto _LL788; _LL788:
_temp787=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp695)->f2; goto _LL786; _LL786: _temp785=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp695)->f3; goto _LL710;} else{ goto _LL711;}
_LL711: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL794: _temp793=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp695)->f1; goto _LL792; _LL792: _temp791=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp695)->f2; goto _LL712;} else{ goto _LL713;}
_LL713: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL798: _temp797=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp695)->f1; goto _LL796; _LL796: _temp795=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp695)->f2; goto _LL714;} else{ goto _LL715;}
_LL715: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_FnCall_e_tag){
_LL802: _temp801=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp695)->f1; goto _LL800; _LL800: _temp799=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp695)->f2; goto _LL716;} else{ goto _LL717;}
_LL717: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_Throw_e_tag){
_LL804: _temp803=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp695)->f1; goto _LL718;} else{ goto _LL719;} _LL719: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL806:
_temp805=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp695)->f1; goto _LL720;} else{ goto _LL721;} _LL721: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL810: _temp809=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp695)->f1;
goto _LL808; _LL808: _temp807=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp695)->f2; goto _LL722;} else{ goto _LL723;} _LL723: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Cast_e_tag){ _LL814: _temp813=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp695)->f1; goto _LL812; _LL812: _temp811=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp695)->f2; goto
_LL724;} else{ goto _LL725;} _LL725: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Address_e_tag){ _LL816: _temp815=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp695)->f1; goto _LL726;} else{ goto _LL727;}
_LL727: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL818: _temp817=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp695)->f1;
goto _LL728;} else{ goto _LL729;} _LL729: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL820: _temp819=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp695)->f1; goto _LL730;} else{ goto
_LL731;} _LL731: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_Deref_e_tag){
_LL822: _temp821=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp695)->f1; goto _LL732;} else{ goto _LL733;} _LL733: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_StructMember_e_tag){ _LL826: _temp825=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp695)->f1;
goto _LL824; _LL824: _temp823=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp695)->f2; goto _LL734;} else{ goto _LL735;} _LL735: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL830: _temp829=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp695)->f1;
goto _LL828; _LL828: _temp827=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp695)->f2; goto _LL736;} else{ goto _LL737;} _LL737: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Subscript_e_tag){ _LL834: _temp833=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp695)->f1;
goto _LL832; _LL832: _temp831=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp695)->f2; goto _LL738;} else{ goto _LL739;} _LL739: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Tuple_e_tag){ _LL836: _temp835=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp695)->f1; goto
_LL740;} else{ goto _LL741;} _LL741: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL840: _temp839=( struct _tuple1*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp695)->f1; goto _LL838; _LL838: _temp837=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp695)->f2;
goto _LL742;} else{ goto _LL743;} _LL743: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Array_e_tag){ _LL844: _temp843=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp695)->f1; goto _LL842; _LL842: _temp841=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp695)->f2; goto _LL744;} else{ goto _LL745;}
_LL745: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL850: _temp849=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp695)->f1; goto _LL848; _LL848: _temp847=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp695)->f2; goto _LL846; _LL846: _temp845=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp695)->f3;
goto _LL746;} else{ goto _LL747;} _LL747: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Struct_e_tag){ _LL858: _temp857=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp695)->f1; goto _LL856; _LL856: _temp855=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp695)->f2; goto _LL854; _LL854: _temp853=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp695)->f3; goto
_LL852; _LL852: _temp851=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp695)->f4; goto _LL748;} else{ goto _LL749;} _LL749: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Enum_e_tag){ _LL868: _temp867=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp695)->f1; goto
_LL866; _LL866: _temp865=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp695)->f2; goto _LL864; _LL864: _temp863=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp695)->f3; goto _LL862; _LL862: _temp861=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp695)->f4; goto
_LL860; _LL860: _temp859=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp695)->f5; goto _LL750;} else{ goto _LL751;} _LL751: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Xenum_e_tag){ _LL876: _temp875=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp695)->f1; goto
_LL874; _LL874: _temp873=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp695)->f2; goto _LL872; _LL872: _temp871=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp695)->f3; goto _LL870; _LL870: _temp869=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp695)->f4;
goto _LL752;} else{ goto _LL753;} _LL753: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL878: _temp877=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp695)->f1; goto _LL754;} else{ goto _LL755;} _LL755: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL882:
_temp881=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp695)->f1; goto _LL880; _LL880: _temp879=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp695)->f2; goto _LL756;} else{ goto
_LL757;} _LL757: if((( struct _enum_struct*) _temp695)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL884: _temp883=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp695)->f1; goto _LL758;} else{ goto _LL759;} _LL759: if((( struct
_enum_struct*) _temp695)->tag == Cyc_Absyn_Codegen_e_tag){ _LL886: _temp885=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp695)->f1;
goto _LL760;} else{ goto _LL761;} _LL761: if((( struct _enum_struct*) _temp695)->tag
== Cyc_Absyn_Fill_e_tag){ _LL888: _temp887=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp695)->f1; goto _LL762;} else{ goto _LL696;} _LL698: return 10000; _LL700:
return 10000; _LL702: return 10000; _LL704: { void* _temp889= _temp773; _LL891:
if( _temp889 == Cyc_Absyn_Plus){ goto _LL892;} else{ goto _LL893;} _LL893: if(
_temp889 == Cyc_Absyn_Times){ goto _LL894;} else{ goto _LL895;} _LL895: if(
_temp889 == Cyc_Absyn_Minus){ goto _LL896;} else{ goto _LL897;} _LL897: if(
_temp889 == Cyc_Absyn_Div){ goto _LL898;} else{ goto _LL899;} _LL899: if(
_temp889 == Cyc_Absyn_Mod){ goto _LL900;} else{ goto _LL901;} _LL901: if(
_temp889 == Cyc_Absyn_Eq){ goto _LL902;} else{ goto _LL903;} _LL903: if(
_temp889 == Cyc_Absyn_Neq){ goto _LL904;} else{ goto _LL905;} _LL905: if(
_temp889 == Cyc_Absyn_Gt){ goto _LL906;} else{ goto _LL907;} _LL907: if(
_temp889 == Cyc_Absyn_Lt){ goto _LL908;} else{ goto _LL909;} _LL909: if(
_temp889 == Cyc_Absyn_Gte){ goto _LL910;} else{ goto _LL911;} _LL911: if(
_temp889 == Cyc_Absyn_Lte){ goto _LL912;} else{ goto _LL913;} _LL913: if(
_temp889 == Cyc_Absyn_Not){ goto _LL914;} else{ goto _LL915;} _LL915: if(
_temp889 == Cyc_Absyn_Bitnot){ goto _LL916;} else{ goto _LL917;} _LL917: if(
_temp889 == Cyc_Absyn_Bitand){ goto _LL918;} else{ goto _LL919;} _LL919: if(
_temp889 == Cyc_Absyn_Bitor){ goto _LL920;} else{ goto _LL921;} _LL921: if(
_temp889 == Cyc_Absyn_Bitxor){ goto _LL922;} else{ goto _LL923;} _LL923: if(
_temp889 == Cyc_Absyn_Bitlshift){ goto _LL924;} else{ goto _LL925;} _LL925: if(
_temp889 == Cyc_Absyn_Bitlrshift){ goto _LL926;} else{ goto _LL927;} _LL927: if(
_temp889 == Cyc_Absyn_Bitarshift){ goto _LL928;} else{ goto _LL929;} _LL929: if(
_temp889 == Cyc_Absyn_Size){ goto _LL930;} else{ goto _LL931;} _LL931: if(
_temp889 == Cyc_Absyn_Printf){ goto _LL932;} else{ goto _LL933;} _LL933: if(
_temp889 == Cyc_Absyn_Fprintf){ goto _LL934;} else{ goto _LL935;} _LL935: if(
_temp889 == Cyc_Absyn_Xprintf){ goto _LL936;} else{ goto _LL937;} _LL937: if(
_temp889 == Cyc_Absyn_Scanf){ goto _LL938;} else{ goto _LL939;} _LL939: if(
_temp889 == Cyc_Absyn_Fscanf){ goto _LL940;} else{ goto _LL941;} _LL941: if(
_temp889 == Cyc_Absyn_Sscanf){ goto _LL942;} else{ goto _LL890;} _LL892: return
100; _LL894: return 110; _LL896: return 100; _LL898: return 110; _LL900: return
110; _LL902: return 70; _LL904: return 70; _LL906: return 80; _LL908: return 80;
_LL910: return 80; _LL912: return 80; _LL914: return 130; _LL916: return 130;
_LL918: return 60; _LL920: return 40; _LL922: return 50; _LL924: return 90;
_LL926: return 80; _LL928: return 80; _LL930: return 140; _LL932: return 130;
_LL934: return 130; _LL936: return 130; _LL938: return 130; _LL940: return 130;
_LL942: return 130; _LL890:;} _LL706: return 20; _LL708: return 20; _LL710:
return 30; _LL712: return 10; _LL714: return 140; _LL716: return 140; _LL718:
return 130; _LL720: return Cyc_Absynpp_exp_prec( _temp805); _LL722: return Cyc_Absynpp_exp_prec(
_temp809); _LL724: return 120; _LL726: return 130; _LL728: return 130; _LL730:
return 130; _LL732: return 130; _LL734: return 140; _LL736: return 140; _LL738:
return 140; _LL740: return 150; _LL742: goto _LL744; _LL744: goto _LL746; _LL746:
goto _LL748; _LL748: goto _LL750; _LL750: goto _LL752; _LL752: goto _LL754;
_LL754: return 140; _LL756: return 140; _LL758: return 10000; _LL760: return 140;
_LL762: return 140; _LL696:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*
e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp943=( void*) e->r; void* _temp1017; void*
_temp1019; struct Cyc_Absyn_Vardecl* _temp1021; struct _tuple0* _temp1023; void*
_temp1025; struct Cyc_Absyn_Vardecl* _temp1027; struct _tuple0* _temp1029; void*
_temp1031; struct _tuple0* _temp1033; struct _tuple0* _temp1035; struct Cyc_List_List*
_temp1037; void* _temp1039; struct Cyc_Absyn_Exp* _temp1041; struct Cyc_Core_Opt*
_temp1043; struct Cyc_Absyn_Exp* _temp1045; void* _temp1047; struct Cyc_Absyn_Exp*
_temp1049; struct Cyc_Absyn_Exp* _temp1051; struct Cyc_Absyn_Exp* _temp1053;
struct Cyc_Absyn_Exp* _temp1055; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Exp*
_temp1059; struct Cyc_List_List* _temp1061; struct Cyc_Absyn_Exp* _temp1063;
struct Cyc_List_List* _temp1065; struct Cyc_Absyn_Exp* _temp1067; struct Cyc_Absyn_Exp*
_temp1069; struct Cyc_Absyn_Exp* _temp1071; struct Cyc_List_List* _temp1073;
struct Cyc_Absyn_Exp* _temp1075; struct Cyc_Absyn_Exp* _temp1077; void*
_temp1079; struct Cyc_Absyn_Exp* _temp1081; void* _temp1083; struct Cyc_Absyn_Exp*
_temp1085; struct Cyc_Absyn_Exp* _temp1087; struct _tagged_string* _temp1089;
struct Cyc_Absyn_Exp* _temp1091; struct _tagged_string* _temp1093; struct Cyc_Absyn_Exp*
_temp1095; struct Cyc_Absyn_Exp* _temp1097; struct Cyc_Absyn_Exp* _temp1099;
struct Cyc_List_List* _temp1101; struct Cyc_List_List* _temp1103; struct _tuple1*
_temp1105; struct Cyc_List_List* _temp1107; int _temp1109; struct Cyc_List_List*
_temp1111; int _temp1113; struct Cyc_Absyn_Exp* _temp1115; struct Cyc_Absyn_Exp*
_temp1117; struct Cyc_Absyn_Vardecl* _temp1119; struct Cyc_Absyn_Structdecl*
_temp1121; struct Cyc_List_List* _temp1123; struct Cyc_Core_Opt* _temp1125;
struct _tuple0* _temp1127; struct Cyc_Absyn_Enumfield* _temp1129; struct Cyc_Absyn_Enumdecl*
_temp1131; struct Cyc_List_List* _temp1133; struct Cyc_Core_Opt* _temp1135;
struct Cyc_Core_Opt* _temp1137; struct Cyc_Absyn_Enumfield* _temp1139; struct
Cyc_Absyn_Xenumdecl* _temp1141; struct Cyc_List_List* _temp1143; struct Cyc_Core_Opt*
_temp1145; void* _temp1147; struct Cyc_List_List* _temp1149; struct Cyc_Core_Opt*
_temp1151; struct Cyc_Absyn_Stmt* _temp1153; struct Cyc_Absyn_Fndecl* _temp1155;
struct Cyc_Absyn_Exp* _temp1157; _LL945: if((( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_Const_e_tag){ _LL1018: _temp1017=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp943)->f1; goto _LL946;} else{ goto _LL947;} _LL947: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Var_e_tag){ _LL1024: _temp1023=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp943)->f1; goto _LL1020;
_LL1020: _temp1019=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp943)->f2; if((
unsigned int) _temp1019 > 1u?(( struct _enum_struct*) _temp1019)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL1022: _temp1021=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp1019)->f1; goto _LL948;} else{ goto _LL949;}} else{ goto _LL949;} _LL949:
if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Var_e_tag){ _LL1030:
_temp1029=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp943)->f1;
goto _LL1026; _LL1026: _temp1025=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp943)->f2; if(( unsigned int) _temp1025 > 1u?(( struct _enum_struct*)
_temp1025)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL1028: _temp1027=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp1025)->f1; goto _LL950;} else{ goto _LL951;}}
else{ goto _LL951;} _LL951: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Var_e_tag){
_LL1034: _temp1033=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp943)->f1;
goto _LL1032; _LL1032: _temp1031=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp943)->f2; goto _LL952;} else{ goto _LL953;} _LL953: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1036: _temp1035=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp943)->f1; goto
_LL954;} else{ goto _LL955;} _LL955: if((( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_Primop_e_tag){ _LL1040: _temp1039=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp943)->f1; goto _LL1038; _LL1038: _temp1037=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp943)->f2; goto _LL956;} else{ goto
_LL957;} _LL957: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1046: _temp1045=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp943)->f1; goto _LL1044; _LL1044: _temp1043=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp943)->f2; goto _LL1042; _LL1042: _temp1041=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp943)->f3;
goto _LL958;} else{ goto _LL959;} _LL959: if((( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1050: _temp1049=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp943)->f1; goto _LL1048; _LL1048:
_temp1047=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp943)->f2; goto
_LL960;} else{ goto _LL961;} _LL961: if((( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL1056: _temp1055=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp943)->f1; goto _LL1054; _LL1054:
_temp1053=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp943)->f2; goto _LL1052; _LL1052: _temp1051=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp943)->f3; goto _LL962;} else{ goto
_LL963;} _LL963: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL1060: _temp1059=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp943)->f1; goto _LL1058; _LL1058: _temp1057=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp943)->f2; goto _LL964;} else{ goto
_LL965;} _LL965: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL1064: _temp1063=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp943)->f1; goto _LL1062; _LL1062: _temp1061=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp943)->f2; goto _LL966;} else{ goto
_LL967;} _LL967: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1068: _temp1067=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp943)->f1; goto _LL1066; _LL1066: _temp1065=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp943)->f2; goto _LL968;} else{ goto
_LL969;} _LL969: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Throw_e_tag){
_LL1070: _temp1069=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp943)->f1; goto _LL970;} else{ goto _LL971;} _LL971: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1072:
_temp1071=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp943)->f1; goto _LL972;} else{ goto _LL973;} _LL973: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1076:
_temp1075=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp943)->f1; goto _LL1074; _LL1074: _temp1073=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp943)->f2; goto _LL974;} else{ goto
_LL975;} _LL975: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Cast_e_tag){
_LL1080: _temp1079=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp943)->f1;
goto _LL1078; _LL1078: _temp1077=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp943)->f2; goto _LL976;} else{ goto _LL977;} _LL977: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Address_e_tag){ _LL1082: _temp1081=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp943)->f1; goto
_LL978;} else{ goto _LL979;} _LL979: if((( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_Sizeoftyp_e_tag){ _LL1084: _temp1083=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp943)->f1; goto _LL980;} else{ goto _LL981;} _LL981: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1086: _temp1085=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp943)->f1;
goto _LL982;} else{ goto _LL983;} _LL983: if((( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1088: _temp1087=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp943)->f1; goto _LL984;} else{ goto _LL985;}
_LL985: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1092: _temp1091=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp943)->f1; goto _LL1090; _LL1090: _temp1089=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp943)->f2; goto _LL986;} else{ goto
_LL987;} _LL987: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1096: _temp1095=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp943)->f1; goto _LL1094; _LL1094: _temp1093=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp943)->f2; goto _LL988;} else{ goto
_LL989;} _LL989: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1100: _temp1099=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp943)->f1; goto _LL1098; _LL1098: _temp1097=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp943)->f2; goto _LL990;} else{ goto
_LL991;} _LL991: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1102: _temp1101=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp943)->f1; goto _LL992;} else{ goto _LL993;} _LL993: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1106:
_temp1105=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp943)->f1;
goto _LL1104; _LL1104: _temp1103=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp943)->f2; goto _LL994;} else{ goto _LL995;} _LL995: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Array_e_tag){ _LL1110: _temp1109=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp943)->f1; if( _temp1109 == 1){
goto _LL1108;} else{ goto _LL997;} _LL1108: _temp1107=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp943)->f2; goto _LL996;} else{ goto _LL997;}
_LL997: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Array_e_tag){
_LL1114: _temp1113=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp943)->f1;
goto _LL1112; _LL1112: _temp1111=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp943)->f2; goto _LL998;} else{ goto _LL999;} _LL999: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1120:
_temp1119=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp943)->f1; goto _LL1118; _LL1118: _temp1117=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp943)->f2; goto _LL1116; _LL1116:
_temp1115=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp943)->f3; goto _LL1000;} else{ goto _LL1001;} _LL1001: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Struct_e_tag){ _LL1128: _temp1127=(
struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp943)->f1; goto
_LL1126; _LL1126: _temp1125=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp943)->f2; goto _LL1124; _LL1124: _temp1123=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp943)->f3; goto _LL1122; _LL1122:
_temp1121=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp943)->f4; goto _LL1002;} else{ goto _LL1003;} _LL1003: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Enum_e_tag){ _LL1138: _temp1137=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp943)->f1; goto
_LL1136; _LL1136: _temp1135=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp943)->f2; goto _LL1134; _LL1134: _temp1133=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp943)->f3; goto _LL1132; _LL1132: _temp1131=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp943)->f4;
goto _LL1130; _LL1130: _temp1129=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp943)->f5; goto _LL1004;} else{ goto _LL1005;} _LL1005: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1146: _temp1145=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp943)->f1; goto
_LL1144; _LL1144: _temp1143=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp943)->f2; goto _LL1142; _LL1142: _temp1141=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp943)->f3; goto _LL1140; _LL1140:
_temp1139=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp943)->f4; goto _LL1006;} else{ goto _LL1007;} _LL1007: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1148: _temp1147=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp943)->f1; goto _LL1008;} else{
goto _LL1009;} _LL1009: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1152: _temp1151=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp943)->f1; goto _LL1150; _LL1150: _temp1149=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp943)->f2; goto _LL1010;} else{
goto _LL1011;} _LL1011: if((( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1154: _temp1153=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp943)->f1; goto _LL1012;} else{ goto _LL1013;} _LL1013: if((( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1156: _temp1155=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp943)->f1;
goto _LL1014;} else{ goto _LL1015;} _LL1015: if((( struct _enum_struct*)
_temp943)->tag == Cyc_Absyn_Fill_e_tag){ _LL1158: _temp1157=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp943)->f1; goto _LL1016;} else{ goto _LL944;}
_LL946: s= Cyc_Absynpp_cnst2doc( _temp1017); goto _LL944; _LL948: s= Cyc_Absynpp_qvar2doc(
_temp1023); if( _temp1021->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp1021->shadow)));} goto _LL944; _LL950: s= Cyc_Absynpp_qvar2doc(
_temp1029); if( _temp1027->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp1027->shadow)));} goto _LL944; _LL952: s= Cyc_Absynpp_qvar2doc(
_temp1033); goto _LL944; _LL954: s= Cyc_Absynpp_qvar2doc( _temp1035); goto
_LL944; _LL956: s= Cyc_Absynpp_primapp2doc( myprec, _temp1039, _temp1037); goto
_LL944; _LL958: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec, _temp1045),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1159=( char*)" "; struct
_tagged_string _temp1160; _temp1160.curr= _temp1159; _temp1160.base= _temp1159;
_temp1160.last_plus_one= _temp1159 + 2; _temp1160;})), _temp1043 == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp1161=( char*)""; struct _tagged_string
_temp1162; _temp1162.curr= _temp1161; _temp1162.base= _temp1161; _temp1162.last_plus_one=
_temp1161 + 1; _temp1162;})): Cyc_Absynpp_prim2doc(( void*) _temp1043->v), Cyc_PP_text((
struct _tagged_string)({ char* _temp1163=( char*)"= "; struct _tagged_string
_temp1164; _temp1164.curr= _temp1163; _temp1164.base= _temp1163; _temp1164.last_plus_one=
_temp1163 + 3; _temp1164;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1041));
goto _LL944; _LL960: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1049);{ void* _temp1165= _temp1047; _LL1167: if( _temp1165 == Cyc_Absyn_PreInc){
goto _LL1168;} else{ goto _LL1169;} _LL1169: if( _temp1165 == Cyc_Absyn_PreDec){
goto _LL1170;} else{ goto _LL1171;} _LL1171: if( _temp1165 == Cyc_Absyn_PostInc){
goto _LL1172;} else{ goto _LL1173;} _LL1173: if( _temp1165 == Cyc_Absyn_PostDec){
goto _LL1174;} else{ goto _LL1166;} _LL1168: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1175=( char*)"++"; struct _tagged_string
_temp1176; _temp1176.curr= _temp1175; _temp1176.base= _temp1175; _temp1176.last_plus_one=
_temp1175 + 3; _temp1176;})), es); goto _LL1166; _LL1170: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1177=( char*)"--"; struct
_tagged_string _temp1178; _temp1178.curr= _temp1177; _temp1178.base= _temp1177;
_temp1178.last_plus_one= _temp1177 + 3; _temp1178;})), es); goto _LL1166;
_LL1172: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1179=( char*)"++"; struct _tagged_string _temp1180; _temp1180.curr=
_temp1179; _temp1180.base= _temp1179; _temp1180.last_plus_one= _temp1179 + 3;
_temp1180;}))); goto _LL1166; _LL1174: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1181=( char*)"--"; struct _tagged_string
_temp1182; _temp1182.curr= _temp1181; _temp1182.base= _temp1181; _temp1182.last_plus_one=
_temp1181 + 3; _temp1182;}))); goto _LL1166; _LL1166:;} goto _LL944;} _LL962:{
struct _tuple8 _temp1184=({ struct _tuple8 _temp1183; _temp1183.f1=( void*)
_temp1053->r; _temp1183.f2=( void*) _temp1051->r; _temp1183;}); _LL1186: goto
_LL1187; _LL1187: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1055), Cyc_PP_text(( struct _tagged_string)({ char* _temp1188=( char*)" ? ";
struct _tagged_string _temp1189; _temp1189.curr= _temp1188; _temp1189.base=
_temp1188; _temp1189.last_plus_one= _temp1188 + 4; _temp1189;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1053), Cyc_PP_text(( struct _tagged_string)({ char* _temp1190=( char*)" : ";
struct _tagged_string _temp1191; _temp1191.curr= _temp1190; _temp1191.base=
_temp1190; _temp1191.last_plus_one= _temp1190 + 4; _temp1191;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1051)); goto _LL1185; _LL1185:;} goto _LL944; _LL964: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1059), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1192=( char*)", "; struct _tagged_string _temp1193;
_temp1193.curr= _temp1192; _temp1193.base= _temp1192; _temp1193.last_plus_one=
_temp1192 + 3; _temp1193;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1057));
goto _LL944; _LL966: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1063), Cyc_PP_text(( struct _tagged_string)({ char* _temp1194=( char*)"(";
struct _tagged_string _temp1195; _temp1195.curr= _temp1194; _temp1195.base=
_temp1194; _temp1195.last_plus_one= _temp1194 + 2; _temp1195;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1061), Cyc_PP_text(( struct _tagged_string)({ char* _temp1196=( char*)")";
struct _tagged_string _temp1197; _temp1197.curr= _temp1196; _temp1197.base=
_temp1196; _temp1197.last_plus_one= _temp1196 + 2; _temp1197;}))); goto _LL944;
_LL968: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1067), Cyc_PP_text((
struct _tagged_string)({ char* _temp1198=( char*)"("; struct _tagged_string
_temp1199; _temp1199.curr= _temp1198; _temp1199.base= _temp1198; _temp1199.last_plus_one=
_temp1198 + 2; _temp1199;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1065), Cyc_PP_text((
struct _tagged_string)({ char* _temp1200=( char*)")"; struct _tagged_string
_temp1201; _temp1201.curr= _temp1200; _temp1201.base= _temp1200; _temp1201.last_plus_one=
_temp1200 + 2; _temp1201;}))); goto _LL944; _LL970: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1202=( char*)"throw "; struct _tagged_string
_temp1203; _temp1203.curr= _temp1202; _temp1203.base= _temp1202; _temp1203.last_plus_one=
_temp1202 + 7; _temp1203;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1069));
goto _LL944; _LL972: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1071); goto
_LL944; _LL974: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1075); goto _LL944;
_LL976: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1204=( char*)"("; struct _tagged_string _temp1205; _temp1205.curr=
_temp1204; _temp1205.base= _temp1204; _temp1205.last_plus_one= _temp1204 + 2;
_temp1205;})), Cyc_Absynpp_typ2doc( _temp1079), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1206=( char*)")"; struct _tagged_string _temp1207;
_temp1207.curr= _temp1206; _temp1207.base= _temp1206; _temp1207.last_plus_one=
_temp1206 + 2; _temp1207;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1077));
goto _LL944; _LL978: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1208=( char*)"&"; struct _tagged_string _temp1209; _temp1209.curr=
_temp1208; _temp1209.base= _temp1208; _temp1209.last_plus_one= _temp1208 + 2;
_temp1209;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1081)); goto _LL944;
_LL980: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1210=( char*)"sizeof("; struct _tagged_string _temp1211; _temp1211.curr=
_temp1210; _temp1211.base= _temp1210; _temp1211.last_plus_one= _temp1210 + 8;
_temp1211;})), Cyc_Absynpp_typ2doc( _temp1083), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1212=( char*)")"; struct _tagged_string _temp1213;
_temp1213.curr= _temp1212; _temp1213.base= _temp1212; _temp1213.last_plus_one=
_temp1212 + 2; _temp1213;}))); goto _LL944; _LL982: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1214=( char*)"sizeof("; struct
_tagged_string _temp1215; _temp1215.curr= _temp1214; _temp1215.base= _temp1214;
_temp1215.last_plus_one= _temp1214 + 8; _temp1215;})), Cyc_Absynpp_exp2doc(
_temp1085), Cyc_PP_text(( struct _tagged_string)({ char* _temp1216=( char*)")";
struct _tagged_string _temp1217; _temp1217.curr= _temp1216; _temp1217.base=
_temp1216; _temp1217.last_plus_one= _temp1216 + 2; _temp1217;}))); goto _LL944;
_LL984: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1218=( char*)"*"; struct _tagged_string _temp1219; _temp1219.curr=
_temp1218; _temp1219.base= _temp1218; _temp1219.last_plus_one= _temp1218 + 2;
_temp1219;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1087)); goto _LL944;
_LL986: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1091), Cyc_PP_text((
struct _tagged_string)({ char* _temp1220=( char*)"."; struct _tagged_string
_temp1221; _temp1221.curr= _temp1220; _temp1221.base= _temp1220; _temp1221.last_plus_one=
_temp1220 + 2; _temp1221;})), Cyc_Absynpp_textptr( _temp1089)); goto _LL944;
_LL988: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1095), Cyc_PP_text((
struct _tagged_string)({ char* _temp1222=( char*)"->"; struct _tagged_string
_temp1223; _temp1223.curr= _temp1222; _temp1223.base= _temp1222; _temp1223.last_plus_one=
_temp1222 + 3; _temp1223;})), Cyc_Absynpp_textptr( _temp1093)); goto _LL944;
_LL990: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1099), Cyc_PP_text((
struct _tagged_string)({ char* _temp1224=( char*)"["; struct _tagged_string
_temp1225; _temp1225.curr= _temp1224; _temp1225.base= _temp1224; _temp1225.last_plus_one=
_temp1224 + 2; _temp1225;})), Cyc_Absynpp_exp2doc( _temp1097), Cyc_PP_text((
struct _tagged_string)({ char* _temp1226=( char*)"]"; struct _tagged_string
_temp1227; _temp1227.curr= _temp1226; _temp1227.base= _temp1226; _temp1227.last_plus_one=
_temp1226 + 2; _temp1227;}))); goto _LL944; _LL992: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1228=( char*)"$("; struct _tagged_string
_temp1229; _temp1229.curr= _temp1228; _temp1229.base= _temp1228; _temp1229.last_plus_one=
_temp1228 + 3; _temp1229;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1101), Cyc_PP_text((
struct _tagged_string)({ char* _temp1230=( char*)")"; struct _tagged_string
_temp1231; _temp1231.curr= _temp1230; _temp1231.base= _temp1230; _temp1231.last_plus_one=
_temp1230 + 2; _temp1231;}))); goto _LL944; _LL994: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1232=( char*)"("; struct _tagged_string
_temp1233; _temp1233.curr= _temp1232; _temp1233.base= _temp1232; _temp1233.last_plus_one=
_temp1232 + 2; _temp1233;})), Cyc_Absynpp_typ2doc((* _temp1105).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1234=( char*)")"; struct _tagged_string
_temp1235; _temp1235.curr= _temp1234; _temp1235.base= _temp1234; _temp1235.last_plus_one=
_temp1234 + 2; _temp1235;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1236=( char*)"{"; struct _tagged_string _temp1237; _temp1237.curr=
_temp1236; _temp1237.base= _temp1236; _temp1237.last_plus_one= _temp1236 + 2;
_temp1237;}),( struct _tagged_string)({ char* _temp1238=( char*)"}"; struct
_tagged_string _temp1239; _temp1239.curr= _temp1238; _temp1239.base= _temp1238;
_temp1239.last_plus_one= _temp1238 + 2; _temp1239;}),( struct _tagged_string)({
char* _temp1240=( char*)","; struct _tagged_string _temp1241; _temp1241.curr=
_temp1240; _temp1241.base= _temp1240; _temp1241.last_plus_one= _temp1240 + 2;
_temp1241;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1103))); goto _LL944; _LL996: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1242=( char*)"new {"; struct _tagged_string _temp1243; _temp1243.curr=
_temp1242; _temp1243.base= _temp1242; _temp1243.last_plus_one= _temp1242 + 6;
_temp1243;}),( struct _tagged_string)({ char* _temp1244=( char*)"}"; struct
_tagged_string _temp1245; _temp1245.curr= _temp1244; _temp1245.base= _temp1244;
_temp1245.last_plus_one= _temp1244 + 2; _temp1245;}),( struct _tagged_string)({
char* _temp1246=( char*)","; struct _tagged_string _temp1247; _temp1247.curr=
_temp1246; _temp1247.base= _temp1246; _temp1247.last_plus_one= _temp1246 + 2;
_temp1247;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1107)); goto _LL944; _LL998: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1248=( char*)"{"; struct _tagged_string _temp1249; _temp1249.curr=
_temp1248; _temp1249.base= _temp1248; _temp1249.last_plus_one= _temp1248 + 2;
_temp1249;}),( struct _tagged_string)({ char* _temp1250=( char*)"}"; struct
_tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base= _temp1250;
_temp1251.last_plus_one= _temp1250 + 2; _temp1251;}),( struct _tagged_string)({
char* _temp1252=( char*)","; struct _tagged_string _temp1253; _temp1253.curr=
_temp1252; _temp1253.base= _temp1252; _temp1253.last_plus_one= _temp1252 + 2;
_temp1253;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1111)); goto _LL944; _LL1000: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1254=( char*)"new {for "; struct _tagged_string
_temp1255; _temp1255.curr= _temp1254; _temp1255.base= _temp1254; _temp1255.last_plus_one=
_temp1254 + 10; _temp1255;})), Cyc_PP_text(*(* _temp1119->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1256=( char*)" < "; struct _tagged_string
_temp1257; _temp1257.curr= _temp1256; _temp1257.base= _temp1256; _temp1257.last_plus_one=
_temp1256 + 4; _temp1257;})), Cyc_Absynpp_exp2doc( _temp1117), Cyc_PP_text((
struct _tagged_string)({ char* _temp1258=( char*)" : "; struct _tagged_string
_temp1259; _temp1259.curr= _temp1258; _temp1259.base= _temp1258; _temp1259.last_plus_one=
_temp1258 + 4; _temp1259;})), Cyc_Absynpp_exp2doc( _temp1115), Cyc_PP_text((
struct _tagged_string)({ char* _temp1260=( char*)"}"; struct _tagged_string
_temp1261; _temp1261.curr= _temp1260; _temp1261.base= _temp1260; _temp1261.last_plus_one=
_temp1260 + 2; _temp1261;}))); goto _LL944; _LL1002: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1127), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1262=( char*)"{";
struct _tagged_string _temp1263; _temp1263.curr= _temp1262; _temp1263.base=
_temp1262; _temp1263.last_plus_one= _temp1262 + 2; _temp1263;}),( struct
_tagged_string)({ char* _temp1264=( char*)"}"; struct _tagged_string _temp1265;
_temp1265.curr= _temp1264; _temp1265.base= _temp1264; _temp1265.last_plus_one=
_temp1264 + 2; _temp1265;}),( struct _tagged_string)({ char* _temp1266=( char*)",";
struct _tagged_string _temp1267; _temp1267.curr= _temp1266; _temp1267.base=
_temp1266; _temp1267.last_plus_one= _temp1266 + 2; _temp1267;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1123))); goto _LL944; _LL1004: _temp1143= _temp1133;
_temp1139= _temp1129; goto _LL1006; _LL1006: if( _temp1143 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1139->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1139->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1268=( char*)"(";
struct _tagged_string _temp1269; _temp1269.curr= _temp1268; _temp1269.base=
_temp1268; _temp1269.last_plus_one= _temp1268 + 2; _temp1269;}),( struct
_tagged_string)({ char* _temp1270=( char*)")"; struct _tagged_string _temp1271;
_temp1271.curr= _temp1270; _temp1271.base= _temp1270; _temp1271.last_plus_one=
_temp1270 + 2; _temp1271;}),( struct _tagged_string)({ char* _temp1272=( char*)",";
struct _tagged_string _temp1273; _temp1273.curr= _temp1272; _temp1273.base=
_temp1272; _temp1273.last_plus_one= _temp1272 + 2; _temp1273;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1143)));} goto _LL944; _LL1008: { struct Cyc_PP_Doc*
inner_d;{ void* _temp1274= _temp1147; void* _temp1284; struct Cyc_Absyn_Enumfield*
_temp1286; struct Cyc_Absyn_Enumdecl* _temp1288; struct Cyc_Absyn_Enumfield*
_temp1290; struct Cyc_Absyn_Xenumdecl* _temp1292; struct _tuple0* _temp1294;
_LL1276: if((( struct _enum_struct*) _temp1274)->tag == Cyc_Absyn_Typ_m_tag){
_LL1285: _temp1284=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp1274)->f1;
goto _LL1277;} else{ goto _LL1278;} _LL1278: if((( struct _enum_struct*)
_temp1274)->tag == Cyc_Absyn_Enum_m_tag){ _LL1289: _temp1288=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_m_struct*) _temp1274)->f1; goto _LL1287; _LL1287:
_temp1286=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_m_struct*)
_temp1274)->f2; goto _LL1279;} else{ goto _LL1280;} _LL1280: if((( struct
_enum_struct*) _temp1274)->tag == Cyc_Absyn_Xenum_m_tag){ _LL1293: _temp1292=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_m_struct*) _temp1274)->f1;
goto _LL1291; _LL1291: _temp1290=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_m_struct*)
_temp1274)->f2; goto _LL1281;} else{ goto _LL1282;} _LL1282: if((( struct
_enum_struct*) _temp1274)->tag == Cyc_Absyn_Unresolved_m_tag){ _LL1295:
_temp1294=( struct _tuple0*)(( struct Cyc_Absyn_Unresolved_m_struct*) _temp1274)->f1;
goto _LL1283;} else{ goto _LL1275;} _LL1277: inner_d= Cyc_Absynpp_typ2doc(
_temp1284); goto _LL1275; _LL1279: _temp1290= _temp1286; goto _LL1281; _LL1281:
_temp1294= _temp1290->name; goto _LL1283; _LL1283: inner_d= Cyc_Absynpp_qvar2doc(
_temp1294); goto _LL1275; _LL1275:;} s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1296=( char*)"malloc(sizeof("; struct
_tagged_string _temp1297; _temp1297.curr= _temp1296; _temp1297.base= _temp1296;
_temp1297.last_plus_one= _temp1296 + 15; _temp1297;})), inner_d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1298=( char*)"))"; struct _tagged_string
_temp1299; _temp1299.curr= _temp1298; _temp1299.base= _temp1298; _temp1299.last_plus_one=
_temp1298 + 3; _temp1299;}))); goto _LL944;} _LL1010: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1300=( char*)"{"; struct _tagged_string
_temp1301; _temp1301.curr= _temp1300; _temp1301.base= _temp1300; _temp1301.last_plus_one=
_temp1300 + 2; _temp1301;}),( struct _tagged_string)({ char* _temp1302=( char*)"}";
struct _tagged_string _temp1303; _temp1303.curr= _temp1302; _temp1303.base=
_temp1302; _temp1303.last_plus_one= _temp1302 + 2; _temp1303;}),( struct
_tagged_string)({ char* _temp1304=( char*)","; struct _tagged_string _temp1305;
_temp1305.curr= _temp1304; _temp1305.base= _temp1304; _temp1305.last_plus_one=
_temp1304 + 2; _temp1305;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1149)); goto _LL944; _LL1012: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1306=( char*)"({ "; struct _tagged_string _temp1307;
_temp1307.curr= _temp1306; _temp1307.base= _temp1306; _temp1307.last_plus_one=
_temp1306 + 4; _temp1307;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1153)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1308=( char*)" })"; struct
_tagged_string _temp1309; _temp1309.curr= _temp1308; _temp1309.base= _temp1308;
_temp1309.last_plus_one= _temp1308 + 4; _temp1309;}))); goto _LL944; _LL1014: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1310=( char*)"codegen(";
struct _tagged_string _temp1311; _temp1311.curr= _temp1310; _temp1311.base=
_temp1310; _temp1311.last_plus_one= _temp1310 + 9; _temp1311;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1312=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1312->r=( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1313=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1313->tag= Cyc_Absyn_Fn_d_tag; _temp1313->f1= _temp1155;( void*) _temp1313;});
_temp1312->loc= e->loc; _temp1312;}))), Cyc_PP_text(( struct _tagged_string)({
char* _temp1314=( char*)")"; struct _tagged_string _temp1315; _temp1315.curr=
_temp1314; _temp1315.base= _temp1314; _temp1315.last_plus_one= _temp1314 + 2;
_temp1315;}))); goto _LL944; _LL1016: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1316=( char*)"fill("; struct _tagged_string
_temp1317; _temp1317.curr= _temp1316; _temp1317.base= _temp1316; _temp1317.last_plus_one=
_temp1316 + 6; _temp1317;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1157)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1318=( char*)")"; struct
_tagged_string _temp1319; _temp1319.curr= _temp1318; _temp1319.base= _temp1318;
_temp1319.last_plus_one= _temp1318 + 2; _temp1319;}))); goto _LL944; _LL944:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1320=( char*)"("; struct _tagged_string _temp1321; _temp1321.curr=
_temp1320; _temp1321.base= _temp1320; _temp1321.last_plus_one= _temp1320 + 2;
_temp1321;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1322=( char*)")";
struct _tagged_string _temp1323; _temp1323.curr= _temp1322; _temp1323.base=
_temp1322; _temp1323.last_plus_one= _temp1322 + 2; _temp1323;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1324= d;
struct Cyc_Absyn_Exp* _temp1330; struct _tagged_string* _temp1332; _LL1326: if(((
struct _enum_struct*) _temp1324)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1331:
_temp1330=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1324)->f1; goto _LL1327;} else{ goto _LL1328;} _LL1328: if((( struct
_enum_struct*) _temp1324)->tag == Cyc_Absyn_FieldName_tag){ _LL1333: _temp1332=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1324)->f1;
goto _LL1329;} else{ goto _LL1325;} _LL1327: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1334=( char*)".["; struct _tagged_string
_temp1335; _temp1335.curr= _temp1334; _temp1335.base= _temp1334; _temp1335.last_plus_one=
_temp1334 + 3; _temp1335;})), Cyc_Absynpp_exp2doc( _temp1330), Cyc_PP_text((
struct _tagged_string)({ char* _temp1336=( char*)"]"; struct _tagged_string
_temp1337; _temp1337.curr= _temp1336; _temp1337.base= _temp1336; _temp1337.last_plus_one=
_temp1336 + 2; _temp1337;}))); _LL1329: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1338=( char*)"."; struct _tagged_string
_temp1339; _temp1339.curr= _temp1338; _temp1339.base= _temp1338; _temp1339.last_plus_one=
_temp1338 + 2; _temp1339;})), Cyc_Absynpp_textptr( _temp1332)); _LL1325:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1340=( char*)""; struct _tagged_string
_temp1341; _temp1341.curr= _temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one=
_temp1340 + 1; _temp1341;}),( struct _tagged_string)({ char* _temp1342=( char*)"=";
struct _tagged_string _temp1343; _temp1343.curr= _temp1342; _temp1343.base=
_temp1342; _temp1343.last_plus_one= _temp1342 + 2; _temp1343;}),( struct
_tagged_string)({ char* _temp1344=( char*)"="; struct _tagged_string _temp1345;
_temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 2; _temp1345;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1346=( char*)""; struct _tagged_string _temp1347;
_temp1347.curr= _temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one=
_temp1346 + 1; _temp1347;}),( struct _tagged_string)({ char* _temp1348=( char*)"";
struct _tagged_string _temp1349; _temp1349.curr= _temp1348; _temp1349.base=
_temp1348; _temp1349.last_plus_one= _temp1348 + 1; _temp1349;}),( struct
_tagged_string)({ char* _temp1350=( char*)","; struct _tagged_string _temp1351;
_temp1351.curr= _temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one=
_temp1350 + 2; _temp1351;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1352= c; char _temp1372; void* _temp1374; short _temp1376;
void* _temp1378; int _temp1380; void* _temp1382; int _temp1384; void* _temp1386;
long long _temp1388; void* _temp1390; struct _tagged_string _temp1392; struct
_tagged_string _temp1394; int _temp1396; struct _tagged_string _temp1398; int
_temp1400; _LL1354: if(( unsigned int) _temp1352 > 1u?(( struct _enum_struct*)
_temp1352)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1375: _temp1374=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp1352)->f1; goto _LL1373; _LL1373:
_temp1372=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1352)->f2; goto
_LL1355;} else{ goto _LL1356;} _LL1356: if(( unsigned int) _temp1352 > 1u?((
struct _enum_struct*) _temp1352)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1379:
_temp1378=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1352)->f1; goto
_LL1377; _LL1377: _temp1376=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1352)->f2; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(( unsigned int)
_temp1352 > 1u?(( struct _enum_struct*) _temp1352)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1383: _temp1382=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1352)->f1;
if( _temp1382 == Cyc_Absyn_Signed){ goto _LL1381;} else{ goto _LL1360;} _LL1381:
_temp1380=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1352)->f2; goto _LL1359;}
else{ goto _LL1360;} _LL1360: if(( unsigned int) _temp1352 > 1u?(( struct
_enum_struct*) _temp1352)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1387: _temp1386=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1352)->f1; if( _temp1386 == Cyc_Absyn_Unsigned){
goto _LL1385;} else{ goto _LL1362;} _LL1385: _temp1384=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1352)->f2; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(( unsigned int)
_temp1352 > 1u?(( struct _enum_struct*) _temp1352)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL1391: _temp1390=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1352)->f1;
goto _LL1389; _LL1389: _temp1388=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1352)->f2; goto _LL1363;} else{ goto _LL1364;} _LL1364: if(( unsigned int)
_temp1352 > 1u?(( struct _enum_struct*) _temp1352)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1393: _temp1392=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1352)->f1; goto _LL1365;} else{ goto _LL1366;} _LL1366: if( _temp1352 ==
Cyc_Absyn_Null_c){ goto _LL1367;} else{ goto _LL1368;} _LL1368: if((
unsigned int) _temp1352 > 1u?(( struct _enum_struct*) _temp1352)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1397: _temp1396=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1352)->f1;
if( _temp1396 == 1){ goto _LL1395;} else{ goto _LL1370;} _LL1395: _temp1394=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1352)->f2; goto
_LL1369;} else{ goto _LL1370;} _LL1370: if(( unsigned int) _temp1352 > 1u?((
struct _enum_struct*) _temp1352)->tag == Cyc_Absyn_String_c_tag: 0){ _LL1401:
_temp1400=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1352)->f1; goto
_LL1399; _LL1399: _temp1398=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1352)->f2; goto _LL1371;} else{ goto _LL1353;} _LL1355: return Cyc_PP_text(({
struct _tagged_string _temp1402= Cyc_Absynpp_char_escape( _temp1372); xprintf("'%.*s'",
_temp1402.last_plus_one - _temp1402.curr, _temp1402.curr);})); _LL1357: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1376)); _LL1359: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1380)); _LL1361: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1384)); _LL1363: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1403=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1404; _temp1404.curr= _temp1403; _temp1404.base= _temp1403; _temp1404.last_plus_one=
_temp1403 + 27; _temp1404;})); _LL1365: return Cyc_PP_text( _temp1392); _LL1367:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1405=( char*)"null";
struct _tagged_string _temp1406; _temp1406.curr= _temp1405; _temp1406.base=
_temp1405; _temp1406.last_plus_one= _temp1405 + 5; _temp1406;})); _LL1369:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1407=(
char*)"new \""; struct _tagged_string _temp1408; _temp1408.curr= _temp1407;
_temp1408.base= _temp1407; _temp1408.last_plus_one= _temp1407 + 6; _temp1408;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1394)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1409=( char*)"\""; struct _tagged_string _temp1410;
_temp1410.curr= _temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one=
_temp1409 + 2; _temp1410;}))); _LL1371: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1411=( char*)"\""; struct _tagged_string
_temp1412; _temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 2; _temp1412;})), Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1398)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1413=( char*)"\""; struct
_tagged_string _temp1414; _temp1414.curr= _temp1413; _temp1414.base= _temp1413;
_temp1414.last_plus_one= _temp1413 + 2; _temp1414;}))); _LL1353:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == Cyc_Absyn_Size){ if( es == 0?
1: es->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1415=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1415->tag= Cyc_Core_Failure_tag; _temp1415->f1=({ struct _tagged_string
_temp1416= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args",
_temp1416.last_plus_one - _temp1416.curr, _temp1416.curr);});( struct
_xenum_struct*) _temp1415;}));}{ struct Cyc_PP_Doc* d= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return Cyc_Absynpp_cat2( d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1417=( char*)".size"; struct _tagged_string
_temp1418; _temp1418.curr= _temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one=
_temp1417 + 6; _temp1418;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){
struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1419=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1419->tag= Cyc_Core_Failure_tag; _temp1419->f1=({ struct _tagged_string
_temp1420= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1420.last_plus_one - _temp1420.curr, _temp1420.curr);});( struct
_xenum_struct*) _temp1419;}));} else{ if( ds->tl == 0){ return Cyc_Absynpp_cat3(
ps, Cyc_PP_text(( struct _tagged_string)({ char* _temp1421=( char*)" "; struct
_tagged_string _temp1422; _temp1422.curr= _temp1421; _temp1422.base= _temp1421;
_temp1422.last_plus_one= _temp1421 + 2; _temp1422;})),( struct Cyc_PP_Doc*) ds->hd);}
else{ if(( ds->tl)->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1423=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1423->tag= Cyc_Core_Failure_tag; _temp1423->f1=({ struct _tagged_string
_temp1424= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1424.last_plus_one - _temp1424.curr, _temp1424.curr);});( struct
_xenum_struct*) _temp1423;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)
ds->hd, Cyc_PP_text(( struct _tagged_string)({ char* _temp1425=( char*)" ";
struct _tagged_string _temp1426; _temp1426.curr= _temp1425; _temp1426.base=
_temp1425; _temp1426.last_plus_one= _temp1425 + 2; _temp1426;})), ps, Cyc_PP_text((
struct _tagged_string)({ char* _temp1427=( char*)" "; struct _tagged_string
_temp1428; _temp1428.curr= _temp1427; _temp1428.base= _temp1427; _temp1428.last_plus_one=
_temp1427 + 2; _temp1428;})),( struct Cyc_PP_Doc*)( ds->tl)->hd);}}}} else{
return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1429=( char*)"("; struct _tagged_string _temp1430; _temp1430.curr=
_temp1429; _temp1430.base= _temp1429; _temp1430.last_plus_one= _temp1429 + 2;
_temp1430;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1431=( char*)")"; struct _tagged_string _temp1432;
_temp1432.curr= _temp1431; _temp1432.base= _temp1431; _temp1432.last_plus_one=
_temp1431 + 2; _temp1432;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1433= p; _LL1435: if( _temp1433 == Cyc_Absyn_Plus){ goto
_LL1436;} else{ goto _LL1437;} _LL1437: if( _temp1433 == Cyc_Absyn_Times){ goto
_LL1438;} else{ goto _LL1439;} _LL1439: if( _temp1433 == Cyc_Absyn_Minus){ goto
_LL1440;} else{ goto _LL1441;} _LL1441: if( _temp1433 == Cyc_Absyn_Div){ goto
_LL1442;} else{ goto _LL1443;} _LL1443: if( _temp1433 == Cyc_Absyn_Mod){ goto
_LL1444;} else{ goto _LL1445;} _LL1445: if( _temp1433 == Cyc_Absyn_Eq){ goto
_LL1446;} else{ goto _LL1447;} _LL1447: if( _temp1433 == Cyc_Absyn_Neq){ goto
_LL1448;} else{ goto _LL1449;} _LL1449: if( _temp1433 == Cyc_Absyn_Gt){ goto
_LL1450;} else{ goto _LL1451;} _LL1451: if( _temp1433 == Cyc_Absyn_Lt){ goto
_LL1452;} else{ goto _LL1453;} _LL1453: if( _temp1433 == Cyc_Absyn_Gte){ goto
_LL1454;} else{ goto _LL1455;} _LL1455: if( _temp1433 == Cyc_Absyn_Lte){ goto
_LL1456;} else{ goto _LL1457;} _LL1457: if( _temp1433 == Cyc_Absyn_Not){ goto
_LL1458;} else{ goto _LL1459;} _LL1459: if( _temp1433 == Cyc_Absyn_Bitnot){ goto
_LL1460;} else{ goto _LL1461;} _LL1461: if( _temp1433 == Cyc_Absyn_Bitand){ goto
_LL1462;} else{ goto _LL1463;} _LL1463: if( _temp1433 == Cyc_Absyn_Bitor){ goto
_LL1464;} else{ goto _LL1465;} _LL1465: if( _temp1433 == Cyc_Absyn_Bitxor){ goto
_LL1466;} else{ goto _LL1467;} _LL1467: if( _temp1433 == Cyc_Absyn_Bitlshift){
goto _LL1468;} else{ goto _LL1469;} _LL1469: if( _temp1433 == Cyc_Absyn_Bitlrshift){
goto _LL1470;} else{ goto _LL1471;} _LL1471: if( _temp1433 == Cyc_Absyn_Bitarshift){
goto _LL1472;} else{ goto _LL1473;} _LL1473: if( _temp1433 == Cyc_Absyn_Size){
goto _LL1474;} else{ goto _LL1475;} _LL1475: if( _temp1433 == Cyc_Absyn_Printf){
goto _LL1476;} else{ goto _LL1477;} _LL1477: if( _temp1433 == Cyc_Absyn_Fprintf){
goto _LL1478;} else{ goto _LL1479;} _LL1479: if( _temp1433 == Cyc_Absyn_Xprintf){
goto _LL1480;} else{ goto _LL1481;} _LL1481: if( _temp1433 == Cyc_Absyn_Scanf){
goto _LL1482;} else{ goto _LL1483;} _LL1483: if( _temp1433 == Cyc_Absyn_Fscanf){
goto _LL1484;} else{ goto _LL1485;} _LL1485: if( _temp1433 == Cyc_Absyn_Sscanf){
goto _LL1486;} else{ goto _LL1434;} _LL1436: return( struct _tagged_string)({
char* _temp1487=( char*)"+"; struct _tagged_string _temp1488; _temp1488.curr=
_temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one= _temp1487 + 2;
_temp1488;}); _LL1438: return( struct _tagged_string)({ char* _temp1489=( char*)"*";
struct _tagged_string _temp1490; _temp1490.curr= _temp1489; _temp1490.base=
_temp1489; _temp1490.last_plus_one= _temp1489 + 2; _temp1490;}); _LL1440: return(
struct _tagged_string)({ char* _temp1491=( char*)"-"; struct _tagged_string
_temp1492; _temp1492.curr= _temp1491; _temp1492.base= _temp1491; _temp1492.last_plus_one=
_temp1491 + 2; _temp1492;}); _LL1442: return( struct _tagged_string)({ char*
_temp1493=( char*)"/"; struct _tagged_string _temp1494; _temp1494.curr=
_temp1493; _temp1494.base= _temp1493; _temp1494.last_plus_one= _temp1493 + 2;
_temp1494;}); _LL1444: return( struct _tagged_string)({ char* _temp1495=( char*)"%";
struct _tagged_string _temp1496; _temp1496.curr= _temp1495; _temp1496.base=
_temp1495; _temp1496.last_plus_one= _temp1495 + 2; _temp1496;}); _LL1446: return(
struct _tagged_string)({ char* _temp1497=( char*)"=="; struct _tagged_string
_temp1498; _temp1498.curr= _temp1497; _temp1498.base= _temp1497; _temp1498.last_plus_one=
_temp1497 + 3; _temp1498;}); _LL1448: return( struct _tagged_string)({ char*
_temp1499=( char*)"!="; struct _tagged_string _temp1500; _temp1500.curr=
_temp1499; _temp1500.base= _temp1499; _temp1500.last_plus_one= _temp1499 + 3;
_temp1500;}); _LL1450: return( struct _tagged_string)({ char* _temp1501=( char*)">";
struct _tagged_string _temp1502; _temp1502.curr= _temp1501; _temp1502.base=
_temp1501; _temp1502.last_plus_one= _temp1501 + 2; _temp1502;}); _LL1452: return(
struct _tagged_string)({ char* _temp1503=( char*)"<"; struct _tagged_string
_temp1504; _temp1504.curr= _temp1503; _temp1504.base= _temp1503; _temp1504.last_plus_one=
_temp1503 + 2; _temp1504;}); _LL1454: return( struct _tagged_string)({ char*
_temp1505=( char*)">="; struct _tagged_string _temp1506; _temp1506.curr=
_temp1505; _temp1506.base= _temp1505; _temp1506.last_plus_one= _temp1505 + 3;
_temp1506;}); _LL1456: return( struct _tagged_string)({ char* _temp1507=( char*)"<=";
struct _tagged_string _temp1508; _temp1508.curr= _temp1507; _temp1508.base=
_temp1507; _temp1508.last_plus_one= _temp1507 + 3; _temp1508;}); _LL1458: return(
struct _tagged_string)({ char* _temp1509=( char*)"!"; struct _tagged_string
_temp1510; _temp1510.curr= _temp1509; _temp1510.base= _temp1509; _temp1510.last_plus_one=
_temp1509 + 2; _temp1510;}); _LL1460: return( struct _tagged_string)({ char*
_temp1511=( char*)"~"; struct _tagged_string _temp1512; _temp1512.curr=
_temp1511; _temp1512.base= _temp1511; _temp1512.last_plus_one= _temp1511 + 2;
_temp1512;}); _LL1462: return( struct _tagged_string)({ char* _temp1513=( char*)"&";
struct _tagged_string _temp1514; _temp1514.curr= _temp1513; _temp1514.base=
_temp1513; _temp1514.last_plus_one= _temp1513 + 2; _temp1514;}); _LL1464: return(
struct _tagged_string)({ char* _temp1515=( char*)"|"; struct _tagged_string
_temp1516; _temp1516.curr= _temp1515; _temp1516.base= _temp1515; _temp1516.last_plus_one=
_temp1515 + 2; _temp1516;}); _LL1466: return( struct _tagged_string)({ char*
_temp1517=( char*)"^"; struct _tagged_string _temp1518; _temp1518.curr=
_temp1517; _temp1518.base= _temp1517; _temp1518.last_plus_one= _temp1517 + 2;
_temp1518;}); _LL1468: return( struct _tagged_string)({ char* _temp1519=( char*)"<<";
struct _tagged_string _temp1520; _temp1520.curr= _temp1519; _temp1520.base=
_temp1519; _temp1520.last_plus_one= _temp1519 + 3; _temp1520;}); _LL1470: return(
struct _tagged_string)({ char* _temp1521=( char*)">>"; struct _tagged_string
_temp1522; _temp1522.curr= _temp1521; _temp1522.base= _temp1521; _temp1522.last_plus_one=
_temp1521 + 3; _temp1522;}); _LL1472: return( struct _tagged_string)({ char*
_temp1523=( char*)">>>"; struct _tagged_string _temp1524; _temp1524.curr=
_temp1523; _temp1524.base= _temp1523; _temp1524.last_plus_one= _temp1523 + 4;
_temp1524;}); _LL1474: return( struct _tagged_string)({ char* _temp1525=( char*)"size";
struct _tagged_string _temp1526; _temp1526.curr= _temp1525; _temp1526.base=
_temp1525; _temp1526.last_plus_one= _temp1525 + 5; _temp1526;}); _LL1476: return(
struct _tagged_string)({ char* _temp1527=( char*)"printf"; struct _tagged_string
_temp1528; _temp1528.curr= _temp1527; _temp1528.base= _temp1527; _temp1528.last_plus_one=
_temp1527 + 7; _temp1528;}); _LL1478: return( struct _tagged_string)({ char*
_temp1529=( char*)"fprintf"; struct _tagged_string _temp1530; _temp1530.curr=
_temp1529; _temp1530.base= _temp1529; _temp1530.last_plus_one= _temp1529 + 8;
_temp1530;}); _LL1480: return( struct _tagged_string)({ char* _temp1531=( char*)"xprintf";
struct _tagged_string _temp1532; _temp1532.curr= _temp1531; _temp1532.base=
_temp1531; _temp1532.last_plus_one= _temp1531 + 8; _temp1532;}); _LL1482: return(
struct _tagged_string)({ char* _temp1533=( char*)"scanf"; struct _tagged_string
_temp1534; _temp1534.curr= _temp1533; _temp1534.base= _temp1533; _temp1534.last_plus_one=
_temp1533 + 6; _temp1534;}); _LL1484: return( struct _tagged_string)({ char*
_temp1535=( char*)"fscanf"; struct _tagged_string _temp1536; _temp1536.curr=
_temp1535; _temp1536.base= _temp1535; _temp1536.last_plus_one= _temp1535 + 7;
_temp1536;}); _LL1486: return( struct _tagged_string)({ char* _temp1537=( char*)"sscanf";
struct _tagged_string _temp1538; _temp1538.curr= _temp1537; _temp1538.base=
_temp1537; _temp1538.last_plus_one= _temp1537 + 7; _temp1538;}); _LL1434:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)((* t).f1)->v), Cyc_PP_text(( struct _tagged_string)({
char* _temp1539=( char*)" = "; struct _tagged_string _temp1540; _temp1540.curr=
_temp1539; _temp1540.base= _temp1539; _temp1540.last_plus_one= _temp1539 + 4;
_temp1540;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_cat2(
struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2){ return Cyc_PP_concat( d1, d2);}
struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, d3));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, d4)));} struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3,
Cyc_PP_concat( d4, d5))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, d6)))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, d7))))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat8( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6,
Cyc_PP_concat( d7, d8)))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat9( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7,
Cyc_PP_concat( d8, d9))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat10( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, d10)))))))));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat11( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct
Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct Cyc_PP_Doc*
d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, Cyc_PP_concat( d10,
d11))))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc*
d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct
Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11, struct Cyc_PP_Doc*
d12){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat(
d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8,
Cyc_PP_concat( d9, Cyc_PP_concat( d10, Cyc_PP_concat( d11, d12)))))))))));} int
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1541=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1547; struct Cyc_Absyn_Decl* _temp1549;
_LL1543: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1550: _temp1549=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1541)->f1; goto _LL1548; _LL1548:
_temp1547=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1541)->f2;
goto _LL1544;} else{ goto _LL1545;} _LL1545: goto _LL1546; _LL1544: return 1;
_LL1546: return 0; _LL1542:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1551=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1591; struct Cyc_Absyn_Stmt* _temp1593; struct Cyc_Absyn_Stmt*
_temp1595; struct Cyc_Absyn_Exp* _temp1597; struct Cyc_Absyn_Stmt* _temp1599;
struct Cyc_Absyn_Stmt* _temp1601; struct Cyc_Absyn_Exp* _temp1603; struct Cyc_Absyn_Stmt*
_temp1605; struct _tuple2 _temp1607; struct Cyc_Absyn_Stmt* _temp1609; struct
Cyc_Absyn_Exp* _temp1611; struct Cyc_Absyn_Stmt* _temp1613; struct Cyc_Absyn_Stmt*
_temp1615; struct Cyc_Absyn_Stmt* _temp1617; struct _tagged_string* _temp1619;
struct Cyc_Absyn_Stmt* _temp1621; struct _tuple2 _temp1623; struct Cyc_Absyn_Stmt*
_temp1625; struct Cyc_Absyn_Exp* _temp1627; struct _tuple2 _temp1629; struct Cyc_Absyn_Stmt*
_temp1631; struct Cyc_Absyn_Exp* _temp1633; struct Cyc_Absyn_Exp* _temp1635;
struct Cyc_List_List* _temp1637; struct Cyc_Absyn_Exp* _temp1639; struct Cyc_Absyn_Switch_clause**
_temp1641; struct Cyc_List_List* _temp1643; struct Cyc_Absyn_Switch_clause**
_temp1645; struct Cyc_List_List* _temp1647; struct Cyc_Absyn_Stmt* _temp1649;
struct Cyc_Absyn_Decl* _temp1651; struct Cyc_Absyn_Stmt* _temp1653; struct Cyc_Absyn_Stmt*
_temp1655; struct Cyc_Absyn_Stmt* _temp1657; struct _tagged_string* _temp1659;
struct _tuple2 _temp1661; struct Cyc_Absyn_Stmt* _temp1663; struct Cyc_Absyn_Exp*
_temp1665; struct Cyc_Absyn_Stmt* _temp1667; struct Cyc_List_List* _temp1669;
struct Cyc_Absyn_Stmt* _temp1671; _LL1553: if( _temp1551 == Cyc_Absyn_Skip_s){
goto _LL1554;} else{ goto _LL1555;} _LL1555: if(( unsigned int) _temp1551 > 1u?((
struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1592:
_temp1591=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1551)->f1;
goto _LL1556;} else{ goto _LL1557;} _LL1557: if(( unsigned int) _temp1551 > 1u?((
struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1596:
_temp1595=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1551)->f1;
goto _LL1594; _LL1594: _temp1593=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1551)->f2; goto _LL1558;} else{ goto _LL1559;} _LL1559: if(( unsigned int)
_temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1598: _temp1597=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1551)->f1; goto _LL1560;} else{ goto _LL1561;} _LL1561: if(( unsigned int)
_temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1604: _temp1603=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1551)->f1; goto _LL1602; _LL1602: _temp1601=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1551)->f2; goto _LL1600; _LL1600:
_temp1599=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1551)->f3; goto _LL1562;} else{ goto _LL1563;} _LL1563: if(( unsigned int)
_temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1608: _temp1607=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1551)->f1; _LL1612: _temp1611= _temp1607.f1; goto _LL1610; _LL1610:
_temp1609= _temp1607.f2; goto _LL1606; _LL1606: _temp1605=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1551)->f2; goto _LL1564;} else{ goto
_LL1565;} _LL1565: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1614: _temp1613=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1551)->f1; goto _LL1566;} else{ goto
_LL1567;} _LL1567: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1616: _temp1615=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1551)->f1; goto _LL1568;} else{ goto
_LL1569;} _LL1569: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1620: _temp1619=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1551)->f1; goto _LL1618;
_LL1618: _temp1617=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1551)->f2; goto _LL1570;} else{ goto _LL1571;} _LL1571: if(( unsigned int)
_temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1636: _temp1635=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1551)->f1; goto _LL1630; _LL1630: _temp1629=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1551)->f2; _LL1634: _temp1633= _temp1629.f1; goto _LL1632; _LL1632:
_temp1631= _temp1629.f2; goto _LL1624; _LL1624: _temp1623=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1551)->f3; _LL1628: _temp1627= _temp1623.f1;
goto _LL1626; _LL1626: _temp1625= _temp1623.f2; goto _LL1622; _LL1622: _temp1621=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1551)->f4; goto
_LL1572;} else{ goto _LL1573;} _LL1573: if(( unsigned int) _temp1551 > 1u?((
struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1640:
_temp1639=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1551)->f1; goto _LL1638; _LL1638: _temp1637=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1551)->f2; goto _LL1574;} else{ goto
_LL1575;} _LL1575: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1644: _temp1643=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1551)->f1; if( _temp1643 == 0){ goto
_LL1642;} else{ goto _LL1577;} _LL1642: _temp1641=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1551)->f2; goto _LL1576;} else{ goto
_LL1577;} _LL1577: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1648: _temp1647=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1551)->f1; goto _LL1646; _LL1646:
_temp1645=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1551)->f2; goto _LL1578;} else{ goto _LL1579;} _LL1579: if(( unsigned int)
_temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1652: _temp1651=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1551)->f1; goto _LL1650; _LL1650: _temp1649=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1551)->f2; goto _LL1580;} else{ goto
_LL1581;} _LL1581: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1654: _temp1653=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1551)->f1; goto _LL1582;} else{ goto
_LL1583;} _LL1583: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1656: _temp1655=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1551)->f1; goto _LL1584;} else{ goto
_LL1585;} _LL1585: if(( unsigned int) _temp1551 > 1u?(( struct _enum_struct*)
_temp1551)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1660: _temp1659=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1551)->f1; goto
_LL1658; _LL1658: _temp1657=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1551)->f2; goto _LL1586;} else{ goto _LL1587;} _LL1587: if(( unsigned int)
_temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1668: _temp1667=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1551)->f1; goto _LL1662; _LL1662: _temp1661=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1551)->f2; _LL1666: _temp1665= _temp1661.f1; goto _LL1664; _LL1664:
_temp1663= _temp1661.f2; goto _LL1588;} else{ goto _LL1589;} _LL1589: if((
unsigned int) _temp1551 > 1u?(( struct _enum_struct*) _temp1551)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1672: _temp1671=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1551)->f1; goto _LL1670; _LL1670: _temp1669=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1551)->f2; goto _LL1590;} else{ goto
_LL1552;} _LL1554: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1673=(
char*)";"; struct _tagged_string _temp1674; _temp1674.curr= _temp1673; _temp1674.base=
_temp1673; _temp1674.last_plus_one= _temp1673 + 2; _temp1674;})); goto _LL1552;
_LL1556: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1591), Cyc_PP_text((
struct _tagged_string)({ char* _temp1675=( char*)";"; struct _tagged_string
_temp1676; _temp1676.curr= _temp1675; _temp1676.base= _temp1675; _temp1676.last_plus_one=
_temp1675 + 2; _temp1676;}))); goto _LL1552; _LL1558: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1595)){ s= Cyc_Absynpp_cat6( Cyc_PP_text((
struct _tagged_string)({ char* _temp1677=( char*)"{ "; struct _tagged_string
_temp1678; _temp1678.curr= _temp1677; _temp1678.base= _temp1677; _temp1678.last_plus_one=
_temp1677 + 3; _temp1678;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1595)),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1679=( char*)"}";
struct _tagged_string _temp1680; _temp1680.curr= _temp1679; _temp1680.base=
_temp1679; _temp1680.last_plus_one= _temp1679 + 2; _temp1680;})), Cyc_PP_line_doc(),
Cyc_Absynpp_is_declaration( _temp1593)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1681=( char*)"{ "; struct _tagged_string _temp1682;
_temp1682.curr= _temp1681; _temp1682.base= _temp1681; _temp1682.last_plus_one=
_temp1681 + 3; _temp1682;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1593)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1683=( char*)"}"; struct
_tagged_string _temp1684; _temp1684.curr= _temp1683; _temp1684.base= _temp1683;
_temp1684.last_plus_one= _temp1683 + 2; _temp1684;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1593));} else{ if( Cyc_Absynpp_is_declaration( _temp1593)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1595), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1685=( char*)"{ "; struct _tagged_string _temp1686;
_temp1686.curr= _temp1685; _temp1686.base= _temp1685; _temp1686.last_plus_one=
_temp1685 + 3; _temp1686;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1593)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1687=( char*)"}"; struct
_tagged_string _temp1688; _temp1688.curr= _temp1687; _temp1688.base= _temp1687;
_temp1688.last_plus_one= _temp1687 + 2; _temp1688;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1689=( char*)""; struct _tagged_string
_temp1690; _temp1690.curr= _temp1689; _temp1690.base= _temp1689; _temp1690.last_plus_one=
_temp1689 + 1; _temp1690;}),({ struct Cyc_List_List* _temp1691=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1691->hd=( void*) _temp1595;
_temp1691->tl=({ struct Cyc_List_List* _temp1692=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1692->hd=( void*) _temp1593;
_temp1692->tl= 0; _temp1692;}); _temp1691;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1693=( char*)""; struct _tagged_string _temp1694;
_temp1694.curr= _temp1693; _temp1694.base= _temp1693; _temp1694.last_plus_one=
_temp1693 + 1; _temp1694;}),({ struct Cyc_List_List* _temp1695=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1695->hd=( void*) _temp1595;
_temp1695->tl=({ struct Cyc_List_List* _temp1696=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1696->hd=( void*) _temp1593;
_temp1696->tl= 0; _temp1696;}); _temp1695;}));} goto _LL1552; _LL1560: if(
_temp1597 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1697=(
char*)"return;"; struct _tagged_string _temp1698; _temp1698.curr= _temp1697;
_temp1698.base= _temp1697; _temp1698.last_plus_one= _temp1697 + 8; _temp1698;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1699=( char*)"return "; struct _tagged_string _temp1700; _temp1700.curr=
_temp1699; _temp1700.base= _temp1699; _temp1700.last_plus_one= _temp1699 + 8;
_temp1700;})), _temp1597 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1701= _temp1597; if( _temp1701 == 0){
_throw( Null_Exception);} _temp1701;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1702=( char*)";"; struct _tagged_string _temp1703; _temp1703.curr=
_temp1702; _temp1703.base= _temp1702; _temp1703.last_plus_one= _temp1702 + 2;
_temp1703;})));} goto _LL1552; _LL1562: { int print_else;{ void* _temp1704=(
void*) _temp1599->r; _LL1706: if( _temp1704 == Cyc_Absyn_Skip_s){ goto _LL1707;}
else{ goto _LL1708;} _LL1708: goto _LL1709; _LL1707: print_else= 0; goto _LL1705;
_LL1709: print_else= 1; goto _LL1705; _LL1705:;} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1710=( char*)"if ("; struct _tagged_string
_temp1711; _temp1711.curr= _temp1710; _temp1711.base= _temp1710; _temp1711.last_plus_one=
_temp1710 + 5; _temp1711;})), Cyc_Absynpp_exp2doc( _temp1603), Cyc_PP_text((
struct _tagged_string)({ char* _temp1712=( char*)") {"; struct _tagged_string
_temp1713; _temp1713.curr= _temp1712; _temp1713.base= _temp1712; _temp1713.last_plus_one=
_temp1712 + 4; _temp1713;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1601))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1714=( char*)"}"; struct _tagged_string _temp1715;
_temp1715.curr= _temp1714; _temp1715.base= _temp1714; _temp1715.last_plus_one=
_temp1714 + 2; _temp1715;})), print_else? Cyc_Absynpp_cat5( Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1716=( char*)"else {"; struct
_tagged_string _temp1717; _temp1717.curr= _temp1716; _temp1717.base= _temp1716;
_temp1717.last_plus_one= _temp1716 + 7; _temp1717;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1599))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1718=( char*)"}"; struct _tagged_string
_temp1719; _temp1719.curr= _temp1718; _temp1719.base= _temp1718; _temp1719.last_plus_one=
_temp1718 + 2; _temp1719;}))): Cyc_PP_nil_doc()); goto _LL1552;} _LL1564: s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1720=( char*)"while ("; struct
_tagged_string _temp1721; _temp1721.curr= _temp1720; _temp1721.base= _temp1720;
_temp1721.last_plus_one= _temp1720 + 8; _temp1721;})), Cyc_Absynpp_exp2doc(
_temp1611), Cyc_PP_text(( struct _tagged_string)({ char* _temp1722=( char*)") {";
struct _tagged_string _temp1723; _temp1723.curr= _temp1722; _temp1723.base=
_temp1722; _temp1723.last_plus_one= _temp1722 + 4; _temp1723;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1605))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1724=( char*)"}"; struct
_tagged_string _temp1725; _temp1725.curr= _temp1724; _temp1725.base= _temp1724;
_temp1725.last_plus_one= _temp1724 + 2; _temp1725;}))); goto _LL1552; _LL1566: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1726=( char*)"break;"; struct
_tagged_string _temp1727; _temp1727.curr= _temp1726; _temp1727.base= _temp1726;
_temp1727.last_plus_one= _temp1726 + 7; _temp1727;})); goto _LL1552; _LL1568: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1728=( char*)"continue;";
struct _tagged_string _temp1729; _temp1729.curr= _temp1728; _temp1729.base=
_temp1728; _temp1729.last_plus_one= _temp1728 + 10; _temp1729;})); goto _LL1552;
_LL1570: s= Cyc_PP_text(({ struct _tagged_string _temp1730=* _temp1619; xprintf("goto %.*s;",
_temp1730.last_plus_one - _temp1730.curr, _temp1730.curr);})); goto _LL1552;
_LL1572: s= Cyc_Absynpp_cat10( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1731=( char*)"for("; struct _tagged_string _temp1732; _temp1732.curr=
_temp1731; _temp1732.base= _temp1731; _temp1732.last_plus_one= _temp1731 + 5;
_temp1732;})), Cyc_Absynpp_exp2doc( _temp1635), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1733=( char*)"; "; struct _tagged_string _temp1734;
_temp1734.curr= _temp1733; _temp1734.base= _temp1733; _temp1734.last_plus_one=
_temp1733 + 3; _temp1734;})), Cyc_Absynpp_exp2doc( _temp1633), Cyc_PP_text((
struct _tagged_string)({ char* _temp1735=( char*)"; "; struct _tagged_string
_temp1736; _temp1736.curr= _temp1735; _temp1736.base= _temp1735; _temp1736.last_plus_one=
_temp1735 + 3; _temp1736;})), Cyc_Absynpp_exp2doc( _temp1627), Cyc_PP_text((
struct _tagged_string)({ char* _temp1737=( char*)") {"; struct _tagged_string
_temp1738; _temp1738.curr= _temp1737; _temp1738.base= _temp1737; _temp1738.last_plus_one=
_temp1737 + 4; _temp1738;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1621))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1739=( char*)"}"; struct _tagged_string _temp1740;
_temp1740.curr= _temp1739; _temp1740.base= _temp1739; _temp1740.last_plus_one=
_temp1739 + 2; _temp1740;}))); goto _LL1552; _LL1574: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1741=( char*)"switch ("; struct
_tagged_string _temp1742; _temp1742.curr= _temp1741; _temp1742.base= _temp1741;
_temp1742.last_plus_one= _temp1741 + 9; _temp1742;})), Cyc_Absynpp_exp2doc(
_temp1639), Cyc_PP_text(( struct _tagged_string)({ char* _temp1743=( char*)") {";
struct _tagged_string _temp1744; _temp1744.curr= _temp1743; _temp1744.base=
_temp1743; _temp1744.last_plus_one= _temp1743 + 4; _temp1744;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1637), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1745=( char*)"}"; struct _tagged_string
_temp1746; _temp1746.curr= _temp1745; _temp1746.base= _temp1745; _temp1746.last_plus_one=
_temp1745 + 2; _temp1746;}))); goto _LL1552; _LL1576: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1747=( char*)"fallthru;"; struct _tagged_string
_temp1748; _temp1748.curr= _temp1747; _temp1748.base= _temp1747; _temp1748.last_plus_one=
_temp1747 + 10; _temp1748;})); goto _LL1552; _LL1578: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1749=( char*)"fallthru("; struct
_tagged_string _temp1750; _temp1750.curr= _temp1749; _temp1750.base= _temp1749;
_temp1750.last_plus_one= _temp1749 + 10; _temp1750;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1647), Cyc_PP_text(( struct _tagged_string)({ char* _temp1751=( char*)");";
struct _tagged_string _temp1752; _temp1752.curr= _temp1751; _temp1752.base=
_temp1751; _temp1752.last_plus_one= _temp1751 + 3; _temp1752;}))); goto _LL1552;
_LL1580: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1651), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1649)); goto _LL1552; _LL1582: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1753=( char*)"cut "; struct
_tagged_string _temp1754; _temp1754.curr= _temp1753; _temp1754.base= _temp1753;
_temp1754.last_plus_one= _temp1753 + 5; _temp1754;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1653))); goto _LL1552; _LL1584: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1755=( char*)"splice "; struct _tagged_string
_temp1756; _temp1756.curr= _temp1755; _temp1756.base= _temp1755; _temp1756.last_plus_one=
_temp1755 + 8; _temp1756;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1655)));
goto _LL1552; _LL1586: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1657): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1659), Cyc_PP_text((
struct _tagged_string)({ char* _temp1757=( char*)": {"; struct _tagged_string
_temp1758; _temp1758.curr= _temp1757; _temp1758.base= _temp1757; _temp1758.last_plus_one=
_temp1757 + 4; _temp1758;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1657)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1759=( char*)"}"; struct _tagged_string _temp1760; _temp1760.curr=
_temp1759; _temp1760.base= _temp1759; _temp1760.last_plus_one= _temp1759 + 2;
_temp1760;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1659), Cyc_PP_text((
struct _tagged_string)({ char* _temp1761=( char*)": "; struct _tagged_string
_temp1762; _temp1762.curr= _temp1761; _temp1762.base= _temp1761; _temp1762.last_plus_one=
_temp1761 + 3; _temp1762;})), Cyc_Absynpp_stmt2doc( _temp1657));} goto _LL1552;
_LL1588: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1763=( char*)"do {"; struct _tagged_string _temp1764; _temp1764.curr=
_temp1763; _temp1764.base= _temp1763; _temp1764.last_plus_one= _temp1763 + 5;
_temp1764;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1667)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1765=( char*)"} while ("; struct _tagged_string _temp1766; _temp1766.curr=
_temp1765; _temp1766.base= _temp1765; _temp1766.last_plus_one= _temp1765 + 10;
_temp1766;})), Cyc_Absynpp_exp2doc( _temp1665), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1767=( char*)");"; struct _tagged_string _temp1768;
_temp1768.curr= _temp1767; _temp1768.base= _temp1767; _temp1768.last_plus_one=
_temp1767 + 3; _temp1768;}))); goto _LL1552; _LL1590: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1769=( char*)"try {"; struct _tagged_string
_temp1770; _temp1770.curr= _temp1769; _temp1770.base= _temp1769; _temp1770.last_plus_one=
_temp1769 + 6; _temp1770;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1671)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1771=( char*)"} catch {"; struct _tagged_string _temp1772; _temp1772.curr=
_temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one= _temp1771 + 10;
_temp1772;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1669)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1773=( char*)"}"; struct _tagged_string _temp1774; _temp1774.curr=
_temp1773; _temp1774.base= _temp1773; _temp1774.last_plus_one= _temp1773 + 2;
_temp1774;}))); goto _LL1552; _LL1552:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1775=( void*) p->r;
int _temp1807; void* _temp1809; char _temp1811; struct _tagged_string _temp1813;
struct Cyc_Absyn_Vardecl* _temp1815; struct Cyc_List_List* _temp1817; struct Cyc_Absyn_Pat*
_temp1819; struct Cyc_Absyn_Vardecl* _temp1821; struct _tuple0* _temp1823;
struct Cyc_List_List* _temp1825; struct Cyc_List_List* _temp1827; struct _tuple0*
_temp1829; struct Cyc_List_List* _temp1831; struct Cyc_List_List* _temp1833;
struct _tuple0* _temp1835; struct Cyc_List_List* _temp1837; struct Cyc_List_List*
_temp1839; struct Cyc_Core_Opt* _temp1841; struct Cyc_Absyn_Structdecl*
_temp1843; struct Cyc_Absyn_Enumfield* _temp1845; struct Cyc_Absyn_Enumdecl*
_temp1847; struct Cyc_List_List* _temp1849; struct Cyc_List_List* _temp1851;
struct Cyc_Core_Opt* _temp1853; struct _tuple0* _temp1855; struct Cyc_Absyn_Enumfield*
_temp1857; struct Cyc_Absyn_Xenumdecl* _temp1859; struct Cyc_List_List*
_temp1861; struct Cyc_List_List* _temp1863; struct _tuple0* _temp1865; _LL1777:
if( _temp1775 == Cyc_Absyn_Wild_p){ goto _LL1778;} else{ goto _LL1779;} _LL1779:
if( _temp1775 == Cyc_Absyn_Null_p){ goto _LL1780;} else{ goto _LL1781;} _LL1781:
if(( unsigned int) _temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag ==
Cyc_Absyn_Int_p_tag: 0){ _LL1810: _temp1809=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1775)->f1; goto _LL1808; _LL1808: _temp1807=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1775)->f2; goto _LL1782;} else{ goto _LL1783;} _LL1783: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1812: _temp1811=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1775)->f1;
goto _LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int) _temp1775 > 2u?((
struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1814:
_temp1813=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1775)->f1;
goto _LL1786;} else{ goto _LL1787;} _LL1787: if(( unsigned int) _temp1775 > 2u?((
struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1816:
_temp1815=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1775)->f1; goto _LL1788;} else{ goto _LL1789;} _LL1789: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1818: _temp1817=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1775)->f1; goto _LL1790;} else{ goto _LL1791;} _LL1791: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1820: _temp1819=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1775)->f1; goto _LL1792;} else{ goto _LL1793;} _LL1793: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1822: _temp1821=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1775)->f1; goto _LL1794;} else{ goto _LL1795;} _LL1795: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1824: _temp1823=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1775)->f1; goto _LL1796;} else{ goto _LL1797;} _LL1797: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1830: _temp1829=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1775)->f1; goto _LL1828; _LL1828: _temp1827=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1775)->f2; goto _LL1826; _LL1826:
_temp1825=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1775)->f3; goto _LL1798;} else{ goto _LL1799;} _LL1799: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1836: _temp1835=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1775)->f1; goto _LL1834; _LL1834: _temp1833=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1775)->f2; goto _LL1832; _LL1832:
_temp1831=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1775)->f3; goto _LL1800;} else{ goto _LL1801;} _LL1801: if(( unsigned int)
_temp1775 > 2u?(( struct _enum_struct*) _temp1775)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1844: _temp1843=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1775)->f1; goto _LL1842; _LL1842: _temp1841=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1775)->f2; goto _LL1840; _LL1840:
_temp1839=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1775)->f3; goto _LL1838; _LL1838: _temp1837=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1775)->f4; goto _LL1802;} else{ goto
_LL1803;} _LL1803: if(( unsigned int) _temp1775 > 2u?(( struct _enum_struct*)
_temp1775)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1856: _temp1855=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1775)->f1; goto _LL1854; _LL1854:
_temp1853=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1775)->f2;
goto _LL1852; _LL1852: _temp1851=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1775)->f3; goto _LL1850; _LL1850: _temp1849=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1775)->f4; goto _LL1848; _LL1848:
_temp1847=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1775)->f5; goto _LL1846; _LL1846: _temp1845=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1775)->f6; goto _LL1804;} else{ goto
_LL1805;} _LL1805: if(( unsigned int) _temp1775 > 2u?(( struct _enum_struct*)
_temp1775)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1866: _temp1865=( struct
_tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1775)->f1; goto _LL1864;
_LL1864: _temp1863=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1775)->f2; goto _LL1862; _LL1862: _temp1861=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1775)->f3; goto _LL1860; _LL1860:
_temp1859=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1775)->f4; goto _LL1858; _LL1858: _temp1857=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1775)->f5; goto _LL1806;} else{ goto
_LL1776;} _LL1778: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1867=(
char*)"_"; struct _tagged_string _temp1868; _temp1868.curr= _temp1867; _temp1868.base=
_temp1867; _temp1868.last_plus_one= _temp1867 + 2; _temp1868;})); goto _LL1776;
_LL1780: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1869=( char*)"null";
struct _tagged_string _temp1870; _temp1870.curr= _temp1869; _temp1870.base=
_temp1869; _temp1870.last_plus_one= _temp1869 + 5; _temp1870;})); goto _LL1776;
_LL1782: if( _temp1809 == Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1807));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1807));}
goto _LL1776; _LL1784: s= Cyc_PP_text(({ struct _tagged_string _temp1871= Cyc_Absynpp_char_escape(
_temp1811); xprintf("'%.*s'", _temp1871.last_plus_one - _temp1871.curr,
_temp1871.curr);})); goto _LL1776; _LL1786: s= Cyc_PP_text( _temp1813); goto
_LL1776; _LL1788: s= Cyc_Absynpp_qvar2doc( _temp1815->name); goto _LL1776;
_LL1790: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1872=( char*)"$("; struct _tagged_string _temp1873; _temp1873.curr=
_temp1872; _temp1873.base= _temp1872; _temp1873.last_plus_one= _temp1872 + 3;
_temp1873;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1874=( char*)","; struct _tagged_string
_temp1875; _temp1875.curr= _temp1874; _temp1875.base= _temp1874; _temp1875.last_plus_one=
_temp1874 + 2; _temp1875;}), _temp1817), Cyc_PP_text(( struct _tagged_string)({
char* _temp1876=( char*)")"; struct _tagged_string _temp1877; _temp1877.curr=
_temp1876; _temp1877.base= _temp1876; _temp1877.last_plus_one= _temp1876 + 2;
_temp1877;}))); goto _LL1776; _LL1792: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1878=( char*)"&"; struct _tagged_string _temp1879;
_temp1879.curr= _temp1878; _temp1879.base= _temp1878; _temp1879.last_plus_one=
_temp1878 + 2; _temp1879;})), Cyc_Absynpp_pat2doc( _temp1819)); goto _LL1776;
_LL1794: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1880=( char*)"*"; struct _tagged_string _temp1881; _temp1881.curr=
_temp1880; _temp1881.base= _temp1880; _temp1881.last_plus_one= _temp1880 + 2;
_temp1881;})), Cyc_Absynpp_qvar2doc( _temp1821->name)); goto _LL1776; _LL1796: s=
Cyc_Absynpp_qvar2doc( _temp1823); goto _LL1776; _LL1798: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1829), Cyc_Absynpp_tvars2doc( _temp1827), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1882=( char*)"("; struct _tagged_string
_temp1883; _temp1883.curr= _temp1882; _temp1883.base= _temp1882; _temp1883.last_plus_one=
_temp1882 + 2; _temp1883;}),( struct _tagged_string)({ char* _temp1884=( char*)")";
struct _tagged_string _temp1885; _temp1885.curr= _temp1884; _temp1885.base=
_temp1884; _temp1885.last_plus_one= _temp1884 + 2; _temp1885;}),( struct
_tagged_string)({ char* _temp1886=( char*)","; struct _tagged_string _temp1887;
_temp1887.curr= _temp1886; _temp1887.base= _temp1886; _temp1887.last_plus_one=
_temp1886 + 2; _temp1887;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1825))); goto _LL1776; _LL1800: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1835), Cyc_Absynpp_tvars2doc( _temp1833), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1888=( char*)"{"; struct _tagged_string _temp1889;
_temp1889.curr= _temp1888; _temp1889.base= _temp1888; _temp1889.last_plus_one=
_temp1888 + 2; _temp1889;}),( struct _tagged_string)({ char* _temp1890=( char*)"}";
struct _tagged_string _temp1891; _temp1891.curr= _temp1890; _temp1891.base=
_temp1890; _temp1891.last_plus_one= _temp1890 + 2; _temp1891;}),( struct
_tagged_string)({ char* _temp1892=( char*)","; struct _tagged_string _temp1893;
_temp1893.curr= _temp1892; _temp1893.base= _temp1892; _temp1893.last_plus_one=
_temp1892 + 2; _temp1893;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1831))); goto _LL1776; _LL1802: s= Cyc_Absynpp_cat3( _temp1843->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1843->name)->v),
Cyc_Absynpp_tvars2doc( _temp1839), Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1894=( char*)"{"; struct _tagged_string _temp1895; _temp1895.curr=
_temp1894; _temp1895.base= _temp1894; _temp1895.last_plus_one= _temp1894 + 2;
_temp1895;}),( struct _tagged_string)({ char* _temp1896=( char*)"}"; struct
_tagged_string _temp1897; _temp1897.curr= _temp1896; _temp1897.base= _temp1896;
_temp1897.last_plus_one= _temp1896 + 2; _temp1897;}),( struct _tagged_string)({
char* _temp1898=( char*)","; struct _tagged_string _temp1899; _temp1899.curr=
_temp1898; _temp1899.base= _temp1898; _temp1899.last_plus_one= _temp1898 + 2;
_temp1899;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1837))); goto _LL1776; _LL1804: _temp1865= _temp1855; _temp1863= _temp1851;
_temp1861= _temp1849; goto _LL1806; _LL1806: if( _temp1861 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1865);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1865), Cyc_Absynpp_tvars2doc(
_temp1863), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1900=( char*)"(";
struct _tagged_string _temp1901; _temp1901.curr= _temp1900; _temp1901.base=
_temp1900; _temp1901.last_plus_one= _temp1900 + 2; _temp1901;}),( struct
_tagged_string)({ char* _temp1902=( char*)")"; struct _tagged_string _temp1903;
_temp1903.curr= _temp1902; _temp1903.base= _temp1902; _temp1903.last_plus_one=
_temp1902 + 2; _temp1903;}),( struct _tagged_string)({ char* _temp1904=( char*)",";
struct _tagged_string _temp1905; _temp1905.curr= _temp1904; _temp1905.base=
_temp1904; _temp1905.last_plus_one= _temp1904 + 2; _temp1905;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1861)));} goto _LL1776; _LL1776:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1906=( char*)""; struct
_tagged_string _temp1907; _temp1907.curr= _temp1906; _temp1907.base= _temp1906;
_temp1907.last_plus_one= _temp1906 + 1; _temp1907;}),( struct _tagged_string)({
char* _temp1908=( char*)"="; struct _tagged_string _temp1909; _temp1909.curr=
_temp1908; _temp1909.base= _temp1908; _temp1909.last_plus_one= _temp1908 + 2;
_temp1909;}),( struct _tagged_string)({ char* _temp1910=( char*)"="; struct
_tagged_string _temp1911; _temp1911.curr= _temp1910; _temp1911.base= _temp1910;
_temp1911.last_plus_one= _temp1910 + 2; _temp1911;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
== Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1912=( char*)"default: "; struct _tagged_string
_temp1913; _temp1913.curr= _temp1912; _temp1913.base= _temp1912; _temp1913.last_plus_one=
_temp1912 + 10; _temp1913;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1914=( char*)"case "; struct
_tagged_string _temp1915; _temp1915.curr= _temp1914; _temp1915.base= _temp1914;
_temp1915.last_plus_one= _temp1914 + 6; _temp1915;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1916=( char*)": "; struct
_tagged_string _temp1917; _temp1917.curr= _temp1916; _temp1917.base= _temp1916;
_temp1917.last_plus_one= _temp1916 + 3; _temp1917;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1918=( char*)"case "; struct
_tagged_string _temp1919; _temp1919.curr= _temp1918; _temp1919.base= _temp1918;
_temp1919.last_plus_one= _temp1918 + 6; _temp1919;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1920=( char*)" && "; struct
_tagged_string _temp1921; _temp1921.curr= _temp1920; _temp1921.base= _temp1920;
_temp1921.last_plus_one= _temp1920 + 5; _temp1921;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1922= c->where_clause; if(
_temp1922 == 0){ _throw( Null_Exception);} _temp1922;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1923=( char*)": "; struct _tagged_string _temp1924;
_temp1924.curr= _temp1923; _temp1924.base= _temp1923; _temp1924.last_plus_one=
_temp1923 + 3; _temp1924;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp1925=( char*)""; struct _tagged_string _temp1926; _temp1926.curr=
_temp1925; _temp1926.base= _temp1925; _temp1926.last_plus_one= _temp1925 + 1;
_temp1926;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl*
d){ struct Cyc_PP_Doc* s;{ void* _temp1927=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1951; struct Cyc_Absyn_Structdecl* _temp1953; struct Cyc_Absyn_Uniondecl*
_temp1955; struct Cyc_Absyn_Vardecl* _temp1957; struct Cyc_Absyn_Vardecl
_temp1959; struct Cyc_List_List* _temp1960; struct Cyc_Core_Opt* _temp1962; int
_temp1964; struct Cyc_Absyn_Exp* _temp1966; void* _temp1968; struct Cyc_Absyn_Tqual*
_temp1970; struct _tuple0* _temp1972; void* _temp1974; struct Cyc_Absyn_Enumdecl*
_temp1976; struct Cyc_Absyn_Xenumdecl* _temp1978; int _temp1980; struct Cyc_Absyn_Exp*
_temp1982; struct Cyc_Core_Opt* _temp1984; struct Cyc_Core_Opt* _temp1986;
struct Cyc_Absyn_Pat* _temp1988; struct Cyc_Absyn_Typedefdecl* _temp1990; struct
Cyc_List_List* _temp1992; struct _tagged_string* _temp1994; struct Cyc_List_List*
_temp1996; struct _tuple0* _temp1998; struct Cyc_List_List* _temp2000; _LL1929:
if((( struct _enum_struct*) _temp1927)->tag == Cyc_Absyn_Fn_d_tag){ _LL1952:
_temp1951=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1927)->f1;
goto _LL1930;} else{ goto _LL1931;} _LL1931: if((( struct _enum_struct*)
_temp1927)->tag == Cyc_Absyn_Struct_d_tag){ _LL1954: _temp1953=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1927)->f1; goto _LL1932;} else{ goto
_LL1933;} _LL1933: if((( struct _enum_struct*) _temp1927)->tag == Cyc_Absyn_Union_d_tag){
_LL1956: _temp1955=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1927)->f1; goto _LL1934;} else{ goto _LL1935;} _LL1935: if((( struct
_enum_struct*) _temp1927)->tag == Cyc_Absyn_Var_d_tag){ _LL1958: _temp1957=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp1927)->f1;
_temp1959=* _temp1957; _LL1975: _temp1974=( void*) _temp1959.sc; goto _LL1973;
_LL1973: _temp1972=( struct _tuple0*) _temp1959.name; goto _LL1971; _LL1971:
_temp1970=( struct Cyc_Absyn_Tqual*) _temp1959.tq; goto _LL1969; _LL1969:
_temp1968=( void*) _temp1959.type; goto _LL1967; _LL1967: _temp1966=( struct Cyc_Absyn_Exp*)
_temp1959.initializer; goto _LL1965; _LL1965: _temp1964=( int) _temp1959.shadow;
goto _LL1963; _LL1963: _temp1962=( struct Cyc_Core_Opt*) _temp1959.region; goto
_LL1961; _LL1961: _temp1960=( struct Cyc_List_List*) _temp1959.attributes; goto
_LL1936;} else{ goto _LL1937;} _LL1937: if((( struct _enum_struct*) _temp1927)->tag
== Cyc_Absyn_Enum_d_tag){ _LL1977: _temp1976=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1927)->f1; goto _LL1938;} else{ goto
_LL1939;} _LL1939: if((( struct _enum_struct*) _temp1927)->tag == Cyc_Absyn_Xenum_d_tag){
_LL1979: _temp1978=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp1927)->f1; goto _LL1940;} else{ goto _LL1941;} _LL1941: if((( struct
_enum_struct*) _temp1927)->tag == Cyc_Absyn_Let_d_tag){ _LL1989: _temp1988=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1927)->f1; goto
_LL1987; _LL1987: _temp1986=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1927)->f2; goto _LL1985; _LL1985: _temp1984=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1927)->f3; goto _LL1983; _LL1983: _temp1982=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1927)->f4; goto
_LL1981; _LL1981: _temp1980=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1927)->f5;
goto _LL1942;} else{ goto _LL1943;} _LL1943: if((( struct _enum_struct*)
_temp1927)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1991: _temp1990=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1927)->f1; goto _LL1944;} else{ goto
_LL1945;} _LL1945: if((( struct _enum_struct*) _temp1927)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1995: _temp1994=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1927)->f1; goto _LL1993; _LL1993: _temp1992=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1927)->f2; goto _LL1946;} else{ goto
_LL1947;} _LL1947: if((( struct _enum_struct*) _temp1927)->tag == Cyc_Absyn_Using_d_tag){
_LL1999: _temp1998=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1927)->f1; goto _LL1997; _LL1997: _temp1996=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1927)->f2; goto _LL1948;} else{ goto
_LL1949;} _LL1949: if((( struct _enum_struct*) _temp1927)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2001: _temp2000=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1927)->f1; goto _LL1950;} else{ goto _LL1928;} _LL1930: { void* t=({ struct
Cyc_Absyn_FnType_struct* _temp2002=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp2002->tag= Cyc_Absyn_FnType_tag;
_temp2002->f1=({ struct Cyc_Absyn_FnInfo _temp2003; _temp2003.tvars= _temp1951->tvs;
_temp2003.effect= _temp1951->effect; _temp2003.ret_typ=( void*)(( void*)
_temp1951->ret_type); _temp2003.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1951->args); _temp2003.varargs= _temp1951->varargs; _temp2003.attributes= 0;
_temp2003;});( void*) _temp2002;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp1951->is_inline?(
struct _tagged_string)({ char* _temp2004=( char*)"inline "; struct
_tagged_string _temp2005; _temp2005.curr= _temp2004; _temp2005.base= _temp2004;
_temp2005.last_plus_one= _temp2004 + 8; _temp2005;}):( struct _tagged_string)({
char* _temp2006=( char*)""; struct _tagged_string _temp2007; _temp2007.curr=
_temp2006; _temp2007.base= _temp2006; _temp2007.last_plus_one= _temp2006 + 1;
_temp2007;})), Cyc_Absynpp_scope2doc(( void*) _temp1951->sc), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp2008=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2008->q_const= 0; _temp2008->q_volatile=
0; _temp2008->q_restrict= 0; _temp2008;}), t,({ struct Cyc_Core_Opt* _temp2009=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2009->v=(
void*) Cyc_Absynpp_cat2( Cyc_Absynpp_atts2doc( _temp1951->attributes), Cyc_Absynpp_typedef_name2doc(
_temp1951->name)); _temp2009;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2010=( char*)" {"; struct _tagged_string _temp2011; _temp2011.curr=
_temp2010; _temp2011.base= _temp2010; _temp2011.last_plus_one= _temp2010 + 3;
_temp2011;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp1951->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2012=( char*)"}"; struct _tagged_string _temp2013; _temp2013.curr=
_temp2012; _temp2013.base= _temp2012; _temp2013.last_plus_one= _temp2012 + 2;
_temp2013;}))); goto _LL1928;} _LL1932: if( _temp1953->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp1953->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2014=( char*)"struct "; struct _tagged_string
_temp2015; _temp2015.curr= _temp2014; _temp2015.base= _temp2014; _temp2015.last_plus_one=
_temp2014 + 8; _temp2015;})), _temp1953->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2016=( char*)""; struct _tagged_string _temp2017;
_temp2017.curr= _temp2016; _temp2017.base= _temp2016; _temp2017.last_plus_one=
_temp2016 + 1; _temp2017;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(
_temp1953->name)->v), Cyc_Absynpp_ktvars2doc( _temp1953->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2018=( char*)";"; struct _tagged_string
_temp2019; _temp2019.curr= _temp2018; _temp2019.base= _temp2018; _temp2019.last_plus_one=
_temp2018 + 2; _temp2019;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp1953->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2020=(
char*)"struct "; struct _tagged_string _temp2021; _temp2021.curr= _temp2020;
_temp2021.base= _temp2020; _temp2021.last_plus_one= _temp2020 + 8; _temp2021;})),
_temp1953->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2022=(
char*)""; struct _tagged_string _temp2023; _temp2023.curr= _temp2022; _temp2023.base=
_temp2022; _temp2023.last_plus_one= _temp2022 + 1; _temp2023;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)( _temp1953->name)->v), Cyc_Absynpp_ktvars2doc( _temp1953->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2024=( char*)" {"; struct
_tagged_string _temp2025; _temp2025.curr= _temp2024; _temp2025.base= _temp2024;
_temp2025.last_plus_one= _temp2024 + 3; _temp2025;})), Cyc_PP_nest( 2, _temp1953->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2026=( char*)""; struct
_tagged_string _temp2027; _temp2027.curr= _temp2026; _temp2027.base= _temp2026;
_temp2027.last_plus_one= _temp2026 + 1; _temp2027;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)( _temp1953->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2028=( char*)"}";
struct _tagged_string _temp2029; _temp2029.curr= _temp2028; _temp2029.base=
_temp2028; _temp2029.last_plus_one= _temp2028 + 2; _temp2029;})), Cyc_Absynpp_atts2doc(
_temp1953->attributes), Cyc_PP_text(( struct _tagged_string)({ char* _temp2030=(
char*)";"; struct _tagged_string _temp2031; _temp2031.curr= _temp2030; _temp2031.base=
_temp2030; _temp2031.last_plus_one= _temp2030 + 2; _temp2031;})));} goto _LL1928;
_LL1934: if( _temp1955->fields == 0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc((
void*) _temp1955->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2032=(
char*)"union "; struct _tagged_string _temp2033; _temp2033.curr= _temp2032;
_temp2033.base= _temp2032; _temp2033.last_plus_one= _temp2032 + 7; _temp2033;})),
_temp1955->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2034=(
char*)""; struct _tagged_string _temp2035; _temp2035.curr= _temp2034; _temp2035.base=
_temp2034; _temp2035.last_plus_one= _temp2034 + 1; _temp2035;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1955->name)->v), Cyc_Absynpp_tvars2doc( _temp1955->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2036=( char*)";"; struct
_tagged_string _temp2037; _temp2037.curr= _temp2036; _temp2037.base= _temp2036;
_temp2037.last_plus_one= _temp2036 + 2; _temp2037;})));} else{ s= Cyc_Absynpp_cat10(
Cyc_Absynpp_scope2doc(( void*) _temp1955->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2038=( char*)"union "; struct _tagged_string
_temp2039; _temp2039.curr= _temp2038; _temp2039.base= _temp2038; _temp2039.last_plus_one=
_temp2038 + 7; _temp2039;})), _temp1955->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2040=( char*)""; struct _tagged_string _temp2041;
_temp2041.curr= _temp2040; _temp2041.base= _temp2040; _temp2041.last_plus_one=
_temp2040 + 1; _temp2041;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1955->name)->v),
Cyc_Absynpp_tvars2doc( _temp1955->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp2042=( char*)" {"; struct _tagged_string _temp2043; _temp2043.curr=
_temp2042; _temp2043.base= _temp2042; _temp2043.last_plus_one= _temp2042 + 3;
_temp2043;})), Cyc_PP_nest( 2, _temp1955->fields == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2044=( char*)""; struct _tagged_string _temp2045;
_temp2045.curr= _temp2044; _temp2045.base= _temp2044; _temp2045.last_plus_one=
_temp2044 + 1; _temp2045;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)( _temp1955->fields)->v))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2046=( char*)"}"; struct _tagged_string
_temp2047; _temp2047.curr= _temp2046; _temp2047.base= _temp2046; _temp2047.last_plus_one=
_temp2046 + 2; _temp2047;})), Cyc_Absynpp_atts2doc( _temp1955->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2048=( char*)";"; struct _tagged_string
_temp2049; _temp2049.curr= _temp2048; _temp2049.base= _temp2048; _temp2049.last_plus_one=
_temp2048 + 2; _temp2049;})));} goto _LL1928; _LL1936: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp1972); if( _temp1964 > 0){ sn= Cyc_Absynpp_cat2(
sn, Cyc_PP_text( xprintf("%d", _temp1964)));} s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp1974), Cyc_Absynpp_tqtd2doc( _temp1970, _temp1968,({ struct Cyc_Core_Opt*
_temp2050=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2050->v=( void*) sn; _temp2050;})), Cyc_Absynpp_atts2doc( _temp1960),
_temp1966 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2051=( char*)"";
struct _tagged_string _temp2052; _temp2052.curr= _temp2051; _temp2052.base=
_temp2051; _temp2052.last_plus_one= _temp2051 + 1; _temp2052;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2053=( char*)" = "; struct
_tagged_string _temp2054; _temp2054.curr= _temp2053; _temp2054.base= _temp2053;
_temp2054.last_plus_one= _temp2053 + 4; _temp2054;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2055= _temp1966; if(
_temp2055 == 0){ _throw( Null_Exception);} _temp2055;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2056=( char*)";"; struct _tagged_string _temp2057;
_temp2057.curr= _temp2056; _temp2057.base= _temp2056; _temp2057.last_plus_one=
_temp2056 + 2; _temp2057;}))); goto _LL1928;} _LL1938: if( _temp1976->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp1976->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2058=( char*)"enum "; struct _tagged_string
_temp2059; _temp2059.curr= _temp2058; _temp2059.base= _temp2058; _temp2059.last_plus_one=
_temp2058 + 6; _temp2059;})), _temp1976->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2060=( char*)""; struct _tagged_string _temp2061;
_temp2061.curr= _temp2060; _temp2061.base= _temp2060; _temp2061.last_plus_one=
_temp2060 + 1; _temp2061;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(
_temp1976->name)->v), Cyc_Absynpp_ktvars2doc( _temp1976->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2062=( char*)";"; struct _tagged_string
_temp2063; _temp2063.curr= _temp2062; _temp2063.base= _temp2062; _temp2063.last_plus_one=
_temp2062 + 2; _temp2063;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc((
void*) _temp1976->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2064=(
char*)"enum "; struct _tagged_string _temp2065; _temp2065.curr= _temp2064;
_temp2065.base= _temp2064; _temp2065.last_plus_one= _temp2064 + 6; _temp2065;})),
_temp1976->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2066=(
char*)""; struct _tagged_string _temp2067; _temp2067.curr= _temp2066; _temp2067.base=
_temp2066; _temp2067.last_plus_one= _temp2066 + 1; _temp2067;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)( _temp1976->name)->v), Cyc_Absynpp_ktvars2doc( _temp1976->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2068=( char*)" {"; struct
_tagged_string _temp2069; _temp2069.curr= _temp2068; _temp2069.base= _temp2068;
_temp2069.last_plus_one= _temp2068 + 3; _temp2069;})), Cyc_PP_nest( 2, _temp1976->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2070=( char*)""; struct
_tagged_string _temp2071; _temp2071.curr= _temp2070; _temp2071.base= _temp2070;
_temp2071.last_plus_one= _temp2070 + 1; _temp2071;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)( _temp1976->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2072=( char*)"};";
struct _tagged_string _temp2073; _temp2073.curr= _temp2072; _temp2073.base=
_temp2072; _temp2073.last_plus_one= _temp2072 + 3; _temp2073;})));} goto _LL1928;
_LL1940: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) _temp1978->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2074=( char*)"xenum "; struct _tagged_string
_temp2075; _temp2075.curr= _temp2074; _temp2075.base= _temp2074; _temp2075.last_plus_one=
_temp2074 + 7; _temp2075;})), Cyc_Absynpp_typedef_name2doc( _temp1978->name),
_temp1978->fields == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2076=(
char*)";"; struct _tagged_string _temp2077; _temp2077.curr= _temp2076; _temp2077.base=
_temp2076; _temp2077.last_plus_one= _temp2076 + 2; _temp2077;})): Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2078=( char*)" {"; struct
_tagged_string _temp2079; _temp2079.curr= _temp2078; _temp2079.base= _temp2078;
_temp2079.last_plus_one= _temp2078 + 3; _temp2079;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp1978->fields))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2080=( char*)"};"; struct
_tagged_string _temp2081; _temp2081.curr= _temp2080; _temp2081.base= _temp2080;
_temp2081.last_plus_one= _temp2080 + 3; _temp2081;})))); goto _LL1928; _LL1942:
s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2082=(
char*)"let "; struct _tagged_string _temp2083; _temp2083.curr= _temp2082;
_temp2083.base= _temp2082; _temp2083.last_plus_one= _temp2082 + 5; _temp2083;})),
Cyc_Absynpp_pat2doc( _temp1988), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2084=( char*)" = "; struct _tagged_string _temp2085; _temp2085.curr=
_temp2084; _temp2085.base= _temp2084; _temp2085.last_plus_one= _temp2084 + 4;
_temp2085;})), Cyc_Absynpp_exp2doc( _temp1982), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2086=( char*)";"; struct _tagged_string _temp2087;
_temp2087.curr= _temp2086; _temp2087.base= _temp2086; _temp2087.last_plus_one=
_temp2086 + 2; _temp2087;}))); goto _LL1928; _LL1944: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp2088=( char*)"typedef "; struct
_tagged_string _temp2089; _temp2089.curr= _temp2088; _temp2089.base= _temp2088;
_temp2089.last_plus_one= _temp2088 + 9; _temp2089;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp2090=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2090->q_const= 0; _temp2090->q_volatile=
0; _temp2090->q_restrict= 0; _temp2090;}),( void*) _temp1990->defn,({ struct Cyc_Core_Opt*
_temp2091=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2091->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp1990->name),
Cyc_Absynpp_tvars2doc( _temp1990->tvs)); _temp2091;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2092=( char*)";"; struct _tagged_string _temp2093;
_temp2093.curr= _temp2092; _temp2093.base= _temp2092; _temp2093.last_plus_one=
_temp2092 + 2; _temp2093;}))); goto _LL1928; _LL1946: if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_add( _temp1994);} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2094=( char*)"namespace "; struct
_tagged_string _temp2095; _temp2095.curr= _temp2094; _temp2095.base= _temp2094;
_temp2095.last_plus_one= _temp2094 + 11; _temp2095;})), Cyc_Absynpp_textptr(
_temp1994), Cyc_PP_text(( struct _tagged_string)({ char* _temp2096=( char*)" {";
struct _tagged_string _temp2097; _temp2097.curr= _temp2096; _temp2097.base=
_temp2096; _temp2097.last_plus_one= _temp2096 + 3; _temp2097;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2098=( char*)""; struct _tagged_string
_temp2099; _temp2099.curr= _temp2098; _temp2099.base= _temp2098; _temp2099.last_plus_one=
_temp2098 + 1; _temp2099;}), _temp1992), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2100=( char*)"}"; struct _tagged_string _temp2101;
_temp2101.curr= _temp2100; _temp2101.base= _temp2100; _temp2101.last_plus_one=
_temp2100 + 2; _temp2101;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL1928; _LL1948: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2102=( char*)"using "; struct
_tagged_string _temp2103; _temp2103.curr= _temp2102; _temp2103.base= _temp2102;
_temp2103.last_plus_one= _temp2102 + 7; _temp2103;})), Cyc_Absynpp_qvar2doc(
_temp1998), Cyc_PP_text(( struct _tagged_string)({ char* _temp2104=( char*)" {";
struct _tagged_string _temp2105; _temp2105.curr= _temp2104; _temp2105.base=
_temp2104; _temp2105.last_plus_one= _temp2104 + 3; _temp2105;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2106=( char*)""; struct _tagged_string
_temp2107; _temp2107.curr= _temp2106; _temp2107.base= _temp2106; _temp2107.last_plus_one=
_temp2106 + 1; _temp2107;}), _temp1996), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2108=( char*)"}"; struct _tagged_string _temp2109;
_temp2109.curr= _temp2108; _temp2109.base= _temp2108; _temp2109.last_plus_one=
_temp2108 + 2; _temp2109;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2110=( char*)"/* using "; struct _tagged_string
_temp2111; _temp2111.curr= _temp2110; _temp2111.base= _temp2110; _temp2111.last_plus_one=
_temp2110 + 10; _temp2111;})), Cyc_Absynpp_qvar2doc( _temp1998), Cyc_PP_text((
struct _tagged_string)({ char* _temp2112=( char*)" { */"; struct _tagged_string
_temp2113; _temp2113.curr= _temp2112; _temp2113.base= _temp2112; _temp2113.last_plus_one=
_temp2112 + 6; _temp2113;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2114=( char*)""; struct _tagged_string _temp2115; _temp2115.curr=
_temp2114; _temp2115.base= _temp2114; _temp2115.last_plus_one= _temp2114 + 1;
_temp2115;}), _temp1996), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2116=( char*)"/* } */"; struct _tagged_string _temp2117; _temp2117.curr=
_temp2116; _temp2117.base= _temp2116; _temp2117.last_plus_one= _temp2116 + 8;
_temp2117;})));} goto _LL1928; _LL1950: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2118=( char*)"extern \"C\" {";
struct _tagged_string _temp2119; _temp2119.curr= _temp2118; _temp2119.base=
_temp2118; _temp2119.last_plus_one= _temp2118 + 13; _temp2119;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2120=( char*)""; struct _tagged_string
_temp2121; _temp2121.curr= _temp2120; _temp2121.base= _temp2120; _temp2121.last_plus_one=
_temp2120 + 1; _temp2121;}), _temp2000), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2122=( char*)"}"; struct _tagged_string _temp2123;
_temp2123.curr= _temp2122; _temp2123.base= _temp2122; _temp2123.last_plus_one=
_temp2122 + 2; _temp2123;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2124=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2125; _temp2125.curr= _temp2124; _temp2125.base= _temp2124;
_temp2125.last_plus_one= _temp2124 + 19; _temp2125;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2126=( char*)""; struct _tagged_string
_temp2127; _temp2127.curr= _temp2126; _temp2127.base= _temp2126; _temp2127.last_plus_one=
_temp2126 + 1; _temp2127;}), _temp2000), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2128=( char*)"/* } */"; struct _tagged_string
_temp2129; _temp2129.curr= _temp2128; _temp2129.base= _temp2128; _temp2129.last_plus_one=
_temp2128 + 8; _temp2129;})));} goto _LL1928; _LL1928:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2130= sc; _LL2132: if( _temp2130 ==
Cyc_Absyn_Static){ goto _LL2133;} else{ goto _LL2134;} _LL2134: if( _temp2130 ==
Cyc_Absyn_Public){ goto _LL2135;} else{ goto _LL2136;} _LL2136: if( _temp2130 ==
Cyc_Absyn_Extern){ goto _LL2137;} else{ goto _LL2138;} _LL2138: if( _temp2130 ==
Cyc_Absyn_ExternC){ goto _LL2139;} else{ goto _LL2140;} _LL2140: if( _temp2130
== Cyc_Absyn_Abstract){ goto _LL2141;} else{ goto _LL2131;} _LL2133: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2142=( char*)"static "; struct
_tagged_string _temp2143; _temp2143.curr= _temp2142; _temp2143.base= _temp2142;
_temp2143.last_plus_one= _temp2142 + 8; _temp2143;})); _LL2135: return Cyc_PP_nil_doc();
_LL2137: return Cyc_PP_text(( struct _tagged_string)({ char* _temp2144=( char*)"extern ";
struct _tagged_string _temp2145; _temp2145.curr= _temp2144; _temp2145.base=
_temp2144; _temp2145.last_plus_one= _temp2144 + 8; _temp2145;})); _LL2139:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2146=( char*)"extern \"C\" ";
struct _tagged_string _temp2147; _temp2147.curr= _temp2146; _temp2147.base=
_temp2146; _temp2147.last_plus_one= _temp2146 + 12; _temp2147;})); _LL2141:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2148=( char*)"abstract ";
struct _tagged_string _temp2149; _temp2149.curr= _temp2148; _temp2149.base=
_temp2148; _temp2149.last_plus_one= _temp2148 + 10; _temp2149;})); _LL2131:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2150= t; struct
Cyc_Absyn_Tvar* _temp2158; struct Cyc_List_List* _temp2160; _LL2152: if((
unsigned int) _temp2150 > 4u?(( struct _enum_struct*) _temp2150)->tag == Cyc_Absyn_VarType_tag:
0){ _LL2159: _temp2158=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2150)->f1; goto _LL2153;} else{ goto _LL2154;} _LL2154: if(( unsigned int)
_temp2150 > 4u?(( struct _enum_struct*) _temp2150)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL2161: _temp2160=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2150)->f1; goto _LL2155;} else{ goto _LL2156;} _LL2156: goto _LL2157;
_LL2153: return Cyc_Tcutil_is_temp_tvar( _temp2158); _LL2155: return(( int(*)(
int(* pred)( void*), struct Cyc_List_List* x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect,
_temp2160); _LL2157: return 0; _LL2151:;} struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual* tq, void* t){ void* _temp2162= t; struct Cyc_Absyn_Exp*
_temp2176; struct Cyc_Absyn_Tqual* _temp2178; void* _temp2180; struct Cyc_Absyn_PtrInfo
_temp2182; struct Cyc_Absyn_Conref* _temp2184; struct Cyc_Absyn_Tqual* _temp2186;
struct Cyc_Absyn_Conref* _temp2188; void* _temp2190; void* _temp2192; struct Cyc_Absyn_FnInfo
_temp2194; struct Cyc_List_List* _temp2196; int _temp2198; struct Cyc_List_List*
_temp2200; void* _temp2202; struct Cyc_Core_Opt* _temp2204; struct Cyc_List_List*
_temp2206; int _temp2208; struct Cyc_Core_Opt* _temp2210; void* _temp2212;
struct Cyc_Core_Opt* _temp2214; struct Cyc_List_List* _temp2216; struct _tuple0*
_temp2218; _LL2164: if(( unsigned int) _temp2162 > 4u?(( struct _enum_struct*)
_temp2162)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2181: _temp2180=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2162)->f1; goto _LL2179; _LL2179:
_temp2178=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2162)->f2; goto _LL2177; _LL2177: _temp2176=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2162)->f3; goto _LL2165;} else{ goto
_LL2166;} _LL2166: if(( unsigned int) _temp2162 > 4u?(( struct _enum_struct*)
_temp2162)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2183: _temp2182=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2162)->f1;
_LL2193: _temp2192=( void*) _temp2182.elt_typ; goto _LL2191; _LL2191: _temp2190=(
void*) _temp2182.rgn_typ; goto _LL2189; _LL2189: _temp2188=( struct Cyc_Absyn_Conref*)
_temp2182.nullable; goto _LL2187; _LL2187: _temp2186=( struct Cyc_Absyn_Tqual*)
_temp2182.tq; goto _LL2185; _LL2185: _temp2184=( struct Cyc_Absyn_Conref*)
_temp2182.bounds; goto _LL2167;} else{ goto _LL2168;} _LL2168: if(( unsigned int)
_temp2162 > 4u?(( struct _enum_struct*) _temp2162)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2195: _temp2194=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2162)->f1; _LL2207: _temp2206=( struct Cyc_List_List*) _temp2194.tvars;
goto _LL2205; _LL2205: _temp2204=( struct Cyc_Core_Opt*) _temp2194.effect; goto
_LL2203; _LL2203: _temp2202=( void*) _temp2194.ret_typ; goto _LL2201; _LL2201:
_temp2200=( struct Cyc_List_List*) _temp2194.args; goto _LL2199; _LL2199:
_temp2198=( int) _temp2194.varargs; goto _LL2197; _LL2197: _temp2196=( struct
Cyc_List_List*) _temp2194.attributes; goto _LL2169;} else{ goto _LL2170;}
_LL2170: if(( unsigned int) _temp2162 > 4u?(( struct _enum_struct*) _temp2162)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2213: _temp2212=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2162)->f1; goto _LL2211; _LL2211: _temp2210=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2162)->f2; goto _LL2209; _LL2209: _temp2208=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2162)->f3; goto _LL2171;} else{ goto
_LL2172;} _LL2172: if(( unsigned int) _temp2162 > 4u?(( struct _enum_struct*)
_temp2162)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2219: _temp2218=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2162)->f1; goto _LL2217;
_LL2217: _temp2216=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2162)->f2; goto _LL2215; _LL2215: _temp2214=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2162)->f3; goto _LL2173;} else{ goto
_LL2174;} _LL2174: goto _LL2175; _LL2165: { struct Cyc_List_List* _temp2222;
void* _temp2224; struct Cyc_Absyn_Tqual* _temp2226; struct _tuple4 _temp2220=
Cyc_Absynpp_to_tms( _temp2178, _temp2180); _LL2227: _temp2226= _temp2220.f1;
goto _LL2225; _LL2225: _temp2224= _temp2220.f2; goto _LL2223; _LL2223: _temp2222=
_temp2220.f3; goto _LL2221; _LL2221: { void* tm; if( Cyc_Evexp_eval_const_uint_exp(
_temp2176) ==( unsigned int) 0){ tm= Cyc_Absyn_Carray_mod;} else{ tm=({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2228=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2228->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp2228->f1= _temp2176;( void*) _temp2228;});} return({ struct _tuple4
_temp2229; _temp2229.f1= _temp2226; _temp2229.f2= _temp2224; _temp2229.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2230=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2230->hd=( void*) tm; _temp2230->tl=
_temp2222; _temp2230;}); _temp2229;});}} _LL2167: { struct Cyc_List_List*
_temp2233; void* _temp2235; struct Cyc_Absyn_Tqual* _temp2237; struct _tuple4
_temp2231= Cyc_Absynpp_to_tms( _temp2186, _temp2192); _LL2238: _temp2237=
_temp2231.f1; goto _LL2236; _LL2236: _temp2235= _temp2231.f2; goto _LL2234;
_LL2234: _temp2233= _temp2231.f3; goto _LL2232; _LL2232: { void* ps;{ struct
_tuple8 _temp2240=({ struct _tuple8 _temp2239; _temp2239.f1=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2188))->v; _temp2239.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2184))->v; _temp2239;}); void* _temp2250;
void* _temp2252; struct Cyc_Absyn_Exp* _temp2254; void* _temp2256; int _temp2258;
void* _temp2260; void* _temp2262; struct Cyc_Absyn_Exp* _temp2264; void*
_temp2266; int _temp2268; void* _temp2270; void* _temp2272; void* _temp2274;
_LL2242: _LL2257: _temp2256= _temp2240.f1; if(( unsigned int) _temp2256 > 1u?((
struct _enum_struct*) _temp2256)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2259:
_temp2258=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2256)->f1; if(
_temp2258 == 1){ goto _LL2251;} else{ goto _LL2244;}} else{ goto _LL2244;}
_LL2251: _temp2250= _temp2240.f2; if(( unsigned int) _temp2250 > 1u?(( struct
_enum_struct*) _temp2250)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2253:
_temp2252=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2250)->f1; if((
unsigned int) _temp2252 > 1u?(( struct _enum_struct*) _temp2252)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2255: _temp2254=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2252)->f1; goto _LL2243;} else{ goto _LL2244;}} else{ goto _LL2244;}
_LL2244: _LL2267: _temp2266= _temp2240.f1; if(( unsigned int) _temp2266 > 1u?((
struct _enum_struct*) _temp2266)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2269:
_temp2268=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2266)->f1; if(
_temp2268 == 0){ goto _LL2261;} else{ goto _LL2246;}} else{ goto _LL2246;}
_LL2261: _temp2260= _temp2240.f2; if(( unsigned int) _temp2260 > 1u?(( struct
_enum_struct*) _temp2260)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2263:
_temp2262=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2260)->f1; if((
unsigned int) _temp2262 > 1u?(( struct _enum_struct*) _temp2262)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2265: _temp2264=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2262)->f1; goto _LL2245;} else{ goto _LL2246;}} else{ goto _LL2246;}
_LL2246: _LL2275: _temp2274= _temp2240.f1; goto _LL2271; _LL2271: _temp2270=
_temp2240.f2; if(( unsigned int) _temp2270 > 1u?(( struct _enum_struct*)
_temp2270)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2273: _temp2272=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2270)->f1; if( _temp2272 == Cyc_Absyn_Unknown_b){
goto _LL2247;} else{ goto _LL2248;}} else{ goto _LL2248;} _LL2248: goto _LL2249;
_LL2243: ps=({ struct Cyc_Absyn_Nullable_ps_struct* _temp2276=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp2276->tag= Cyc_Absyn_Nullable_ps_tag;
_temp2276->f1= _temp2254;( void*) _temp2276;}); goto _LL2241; _LL2245: ps=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp2277=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp2277->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp2277->f1= _temp2264;( void*) _temp2277;}); goto _LL2241; _LL2247: ps= Cyc_Absyn_TaggedArray_ps;
goto _LL2241; _LL2249: ps= Cyc_Absyn_TaggedArray_ps; goto _LL2241; _LL2241:;}{
void* tm=({ struct Cyc_Absyn_Pointer_mod_struct* _temp2278=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2278->tag= Cyc_Absyn_Pointer_mod_tag;
_temp2278->f1=( void*) ps; _temp2278->f2=( void*) _temp2190; _temp2278->f3= tq;(
void*) _temp2278;}); return({ struct _tuple4 _temp2279; _temp2279.f1= _temp2237;
_temp2279.f2= _temp2235; _temp2279.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2280=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2280->hd=( void*) tm; _temp2280->tl= _temp2233; _temp2280;}); _temp2279;});}}}
_LL2169: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp2204 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*) _temp2204->v)){ _temp2204= 0; _temp2206= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2206);}}{ struct Cyc_List_List* _temp2283;
void* _temp2285; struct Cyc_Absyn_Tqual* _temp2287; struct _tuple4 _temp2281=
Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(), _temp2202); _LL2288: _temp2287=
_temp2281.f1; goto _LL2286; _LL2286: _temp2285= _temp2281.f2; goto _LL2284;
_LL2284: _temp2283= _temp2281.f3; goto _LL2282; _LL2282: { struct Cyc_List_List*
tms= _temp2283; if( _temp2196 != 0){ tms=({ struct Cyc_List_List* _temp2289=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2289->hd=(
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2290=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2290->tag= Cyc_Absyn_Attributes_mod_tag;
_temp2290->f1= 0; _temp2290->f2= _temp2196;( void*) _temp2290;}); _temp2289->tl=
tms; _temp2289;});} tms=({ struct Cyc_List_List* _temp2291=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2291->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2292=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2292->tag= Cyc_Absyn_Function_mod_tag; _temp2292->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2293=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2293->tag= Cyc_Absyn_WithTypes_tag; _temp2293->f1= _temp2200; _temp2293->f2=
_temp2198; _temp2293->f3= _temp2204;( void*) _temp2293;});( void*) _temp2292;});
_temp2291->tl= tms; _temp2291;}); if( _temp2206 != 0){ tms=({ struct Cyc_List_List*
_temp2294=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2294->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2295=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp2295->tag= Cyc_Absyn_TypeParams_mod_tag; _temp2295->f1= _temp2206;
_temp2295->f2= 0; _temp2295->f3= 1;( void*) _temp2295;}); _temp2294->tl= tms;
_temp2294;});} return({ struct _tuple4 _temp2296; _temp2296.f1= _temp2287;
_temp2296.f2= _temp2285; _temp2296.f3= tms; _temp2296;});}} _LL2171: if(
_temp2210 == 0){ return({ struct _tuple4 _temp2297; _temp2297.f1= tq; _temp2297.f2=
t; _temp2297.f3= 0; _temp2297;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)
_temp2210->v);} _LL2173: if( _temp2214 == 0? 1: ! Cyc_Absynpp_expand_typedefs){
return({ struct _tuple4 _temp2298; _temp2298.f1= tq; _temp2298.f2= t; _temp2298.f3=
0; _temp2298;});} else{ return Cyc_Absynpp_to_tms( tq,( void*) _temp2214->v);}
_LL2175: return({ struct _tuple4 _temp2299; _temp2299.f1= tq; _temp2299.f2= t;
_temp2299.f3= 0; _temp2299;}); _LL2163:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2302; void* _temp2304; struct Cyc_Absyn_Tqual* _temp2306; struct _tuple4
_temp2300= Cyc_Absynpp_to_tms( tq, t); _LL2307: _temp2306= _temp2300.f1; goto
_LL2305; _LL2305: _temp2304= _temp2300.f2; goto _LL2303; _LL2303: _temp2302=
_temp2300.f3; goto _LL2301; _LL2301: _temp2302=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2302); if( _temp2302 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2306), Cyc_Absynpp_ntyp2doc(
_temp2304));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2306),
Cyc_Absynpp_ntyp2doc( _temp2304), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2308=( char*)" "; struct _tagged_string _temp2309; _temp2309.curr=
_temp2308; _temp2309.base= _temp2308; _temp2309.last_plus_one= _temp2308 + 2;
_temp2309;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)
dopt->v, _temp2302));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc( struct
Cyc_Absyn_Structfield* f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc( f->tq,(
void*) f->type,({ struct Cyc_Core_Opt* _temp2310=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2310->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2310;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2311=( char*)";"; struct _tagged_string
_temp2312; _temp2312.curr= _temp2311; _temp2312.base= _temp2311; _temp2312.last_plus_one=
_temp2311 + 2; _temp2312;})));} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2313=( char*)""; struct _tagged_string _temp2314; _temp2314.curr= _temp2313;
_temp2314.base= _temp2313; _temp2314.last_plus_one= _temp2313 + 1; _temp2314;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ return Cyc_Absynpp_cat4( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->tag == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp2315=( char*)"=";
struct _tagged_string _temp2316; _temp2316.curr= _temp2315; _temp2316.base=
_temp2315; _temp2316.last_plus_one= _temp2315 + 2; _temp2316;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2317= f->tag; if( _temp2317
== 0){ _throw( Null_Exception);} _temp2317;}))), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,( struct _tagged_string)({ char*
_temp2318=( char*)","; struct _tagged_string _temp2319; _temp2319.curr=
_temp2318; _temp2319.base= _temp2318; _temp2319.last_plus_one= _temp2318 + 2;
_temp2319;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl= tdl->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd), 72, f); fprintf( f,"\n");}}
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){
return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct _tagged_string)({ char*
_temp2320=( char*)""; struct _tagged_string _temp2321; _temp2321.curr= _temp2320;
_temp2321.base= _temp2320; _temp2321.last_plus_one= _temp2320 + 1; _temp2321;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc, tdl)), 72);}
struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s),
72);} struct _tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string s= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc( t), 72);
Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return s;}} struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0* v){ return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);}
struct _tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);}