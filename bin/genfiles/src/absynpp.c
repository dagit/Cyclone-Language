#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple6{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple7{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; struct _tuple9{ void* f1; void* f2; } ; struct
_tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
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
char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int
w); extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_string s); extern
struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d); extern
struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2);
extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seq(
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep,
struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
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
struct Cyc_Buffer_t; typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern struct
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
void* t); extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
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
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual*
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt); extern
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fields);
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_qvar_to_Cids=
0; struct _tagged_string Cyc_Absynpp_char_escape( char c){ switch( c){ case '\a':
_LL0: return( struct _tagged_string)({ char* _temp2=( char*)"\\a"; struct
_tagged_string _temp3; _temp3.curr= _temp2; _temp3.base= _temp2; _temp3.last_plus_one=
_temp2 + 3; _temp3;}); case '\b': _LL1: return( struct _tagged_string)({ char*
_temp5=( char*)"\\b"; struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base=
_temp5; _temp6.last_plus_one= _temp5 + 3; _temp6;}); case '\f': _LL4: return(
struct _tagged_string)({ char* _temp8=( char*)"\\f"; struct _tagged_string
_temp9; _temp9.curr= _temp8; _temp9.base= _temp8; _temp9.last_plus_one= _temp8 +
3; _temp9;}); case '\n': _LL7: return( struct _tagged_string)({ char* _temp11=(
char*)"\\n"; struct _tagged_string _temp12; _temp12.curr= _temp11; _temp12.base=
_temp11; _temp12.last_plus_one= _temp11 + 3; _temp12;}); case '\r': _LL10:
return( struct _tagged_string)({ char* _temp14=( char*)"\\r"; struct
_tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14; _temp15.last_plus_one=
_temp14 + 3; _temp15;}); case '\t': _LL13: return( struct _tagged_string)({ char*
_temp17=( char*)"\\t"; struct _tagged_string _temp18; _temp18.curr= _temp17;
_temp18.base= _temp17; _temp18.last_plus_one= _temp17 + 3; _temp18;}); case '\v':
_LL16: return( struct _tagged_string)({ char* _temp20=( char*)"\\v"; struct
_tagged_string _temp21; _temp21.curr= _temp20; _temp21.base= _temp20; _temp21.last_plus_one=
_temp20 + 3; _temp21;}); case '\\': _LL19: return( struct _tagged_string)({ char*
_temp23=( char*)"\\\\"; struct _tagged_string _temp24; _temp24.curr= _temp23;
_temp24.base= _temp23; _temp24.last_plus_one= _temp23 + 3; _temp24;}); case '"':
_LL22: return( struct _tagged_string)({ char* _temp26=( char*)"\""; struct
_tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one=
_temp26 + 2; _temp27;}); case '\'': _LL25: return( struct _tagged_string)({ char*
_temp29=( char*)"\\'"; struct _tagged_string _temp30; _temp30.curr= _temp29;
_temp30.base= _temp29; _temp30.last_plus_one= _temp29 + 3; _temp30;}); default:
_LL28: if(( int) c >=( int)' '?( int) c <=( int)'~': 0){ struct _tagged_string t=
Cyc_Core_new_string( 1);({ struct _tagged_string _temp32= t; char* _temp34=
_temp32.curr + 0; if( _temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one){
_throw( Null_Exception);}* _temp34= c;}); return t;} else{ struct _tagged_string
t= Cyc_Core_new_string( 4); int j= 0;({ struct _tagged_string _temp35= t; char*
_temp37= _temp35.curr +( j ++); if( _temp37 < _temp35.base? 1: _temp37 >=
_temp35.last_plus_one){ _throw( Null_Exception);}* _temp37='\\';});({ struct
_tagged_string _temp38= t; char* _temp40= _temp38.curr +( j ++); if( _temp40 <
_temp38.base? 1: _temp40 >= _temp38.last_plus_one){ _throw( Null_Exception);}*
_temp40=( char)(( int)'0' +(( int) c >> 6 & 7));});({ struct _tagged_string
_temp41= t; char* _temp43= _temp41.curr +( j ++); if( _temp43 < _temp41.base? 1:
_temp43 >= _temp41.last_plus_one){ _throw( Null_Exception);}* _temp43=( char)((
int)'0' +(( int) c >> 3 & 7));});({ struct _tagged_string _temp44= t; char*
_temp46= _temp44.curr +( j ++); if( _temp46 < _temp44.base? 1: _temp46 >=
_temp44.last_plus_one){ _throw( Null_Exception);}* _temp46=( char)(( int)'0' +((
int) c & 7));}); return t;}}} static int Cyc_Absynpp_special( struct
_tagged_string s){ int sz=( int)(({ struct _tagged_string _temp47= s;(
unsigned int)( _temp47.last_plus_one - _temp47.curr);}) -( unsigned int) 1);{
int i= 0; for( 0; i < sz; i ++){ char c=({ struct _tagged_string _temp48= s;
char* _temp50= _temp48.curr + i; if( _temp50 < _temp48.base? 1: _temp50 >=
_temp48.last_plus_one){ _throw( Null_Exception);}* _temp50;}); if(((( int) c <=(
int)' '? 1:( int) c >=( int)'~')? 1:( int) c ==( int)'"')? 1:( int) c ==( int)'\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp51= s;( unsigned int)( _temp51.last_plus_one -
_temp51.curr);}) -( unsigned int) 1); if( n > 0?( int)({ struct _tagged_string
_temp52= s; char* _temp54= _temp52.curr + n; if( _temp54 < _temp52.base? 1:
_temp54 >= _temp52.last_plus_one){ _throw( Null_Exception);}* _temp54;}) ==( int)'\000':
0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ char _temp58=({ struct
_tagged_string _temp55= s; char* _temp57= _temp55.curr + i; if( _temp57 <
_temp55.base? 1: _temp57 >= _temp55.last_plus_one){ _throw( Null_Exception);}*
_temp57;}); _LL60: if( _temp58 =='\a'){ goto _LL61;} else{ goto _LL62;} _LL62:
if( _temp58 =='\b'){ goto _LL63;} else{ goto _LL64;} _LL64: if( _temp58 =='\f'){
goto _LL65;} else{ goto _LL66;} _LL66: if( _temp58 =='\n'){ goto _LL67;} else{
goto _LL68;} _LL68: if( _temp58 =='\r'){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp58 =='\t'){ goto _LL71;} else{ goto _LL72;} _LL72: if( _temp58 =='\v'){
goto _LL73;} else{ goto _LL74;} _LL74: if( _temp58 =='\\'){ goto _LL75;} else{
goto _LL76;} _LL76: if( _temp58 =='"'){ goto _LL77;} else{ goto _LL78;} _LL78:
goto _LL79; _LL61: goto _LL63; _LL63: goto _LL65; _LL65: goto _LL67; _LL67: goto
_LL69; _LL69: goto _LL71; _LL71: goto _LL73; _LL73: goto _LL75; _LL75: goto
_LL77; _LL77: len += 2; goto _LL59; _LL79: if(( int) _temp58 >=( int)' '?( int)
_temp58 <=( int)'~': 0){ len ++;} else{ len += 4;} goto _LL59; _LL59:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ char _temp83=({ struct _tagged_string _temp80= s; char* _temp82= _temp80.curr
+ i; if( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one){ _throw(
Null_Exception);}* _temp82;}); _LL85: if( _temp83 =='\a'){ goto _LL86;} else{
goto _LL87;} _LL87: if( _temp83 =='\b'){ goto _LL88;} else{ goto _LL89;} _LL89:
if( _temp83 =='\f'){ goto _LL90;} else{ goto _LL91;} _LL91: if( _temp83 =='\n'){
goto _LL92;} else{ goto _LL93;} _LL93: if( _temp83 =='\r'){ goto _LL94;} else{
goto _LL95;} _LL95: if( _temp83 =='\t'){ goto _LL96;} else{ goto _LL97;} _LL97:
if( _temp83 =='\v'){ goto _LL98;} else{ goto _LL99;} _LL99: if( _temp83 =='\\'){
goto _LL100;} else{ goto _LL101;} _LL101: if( _temp83 =='"'){ goto _LL102;}
else{ goto _LL103;} _LL103: goto _LL104; _LL86:({ struct _tagged_string _temp105=
t; char* _temp107= _temp105.curr +( j ++); if( _temp107 < _temp105.base? 1:
_temp107 >= _temp105.last_plus_one){ _throw( Null_Exception);}* _temp107='\\';});({
struct _tagged_string _temp108= t; char* _temp110= _temp108.curr +( j ++); if(
_temp110 < _temp108.base? 1: _temp110 >= _temp108.last_plus_one){ _throw(
Null_Exception);}* _temp110='a';}); goto _LL84; _LL88:({ struct _tagged_string
_temp111= t; char* _temp113= _temp111.curr +( j ++); if( _temp113 < _temp111.base?
1: _temp113 >= _temp111.last_plus_one){ _throw( Null_Exception);}* _temp113='\\';});({
struct _tagged_string _temp114= t; char* _temp116= _temp114.curr +( j ++); if(
_temp116 < _temp114.base? 1: _temp116 >= _temp114.last_plus_one){ _throw(
Null_Exception);}* _temp116='b';}); goto _LL84; _LL90:({ struct _tagged_string
_temp117= t; char* _temp119= _temp117.curr +( j ++); if( _temp119 < _temp117.base?
1: _temp119 >= _temp117.last_plus_one){ _throw( Null_Exception);}* _temp119='\\';});({
struct _tagged_string _temp120= t; char* _temp122= _temp120.curr +( j ++); if(
_temp122 < _temp120.base? 1: _temp122 >= _temp120.last_plus_one){ _throw(
Null_Exception);}* _temp122='f';}); goto _LL84; _LL92:({ struct _tagged_string
_temp123= t; char* _temp125= _temp123.curr +( j ++); if( _temp125 < _temp123.base?
1: _temp125 >= _temp123.last_plus_one){ _throw( Null_Exception);}* _temp125='\\';});({
struct _tagged_string _temp126= t; char* _temp128= _temp126.curr +( j ++); if(
_temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one){ _throw(
Null_Exception);}* _temp128='n';}); goto _LL84; _LL94:({ struct _tagged_string
_temp129= t; char* _temp131= _temp129.curr +( j ++); if( _temp131 < _temp129.base?
1: _temp131 >= _temp129.last_plus_one){ _throw( Null_Exception);}* _temp131='\\';});({
struct _tagged_string _temp132= t; char* _temp134= _temp132.curr +( j ++); if(
_temp134 < _temp132.base? 1: _temp134 >= _temp132.last_plus_one){ _throw(
Null_Exception);}* _temp134='r';}); goto _LL84; _LL96:({ struct _tagged_string
_temp135= t; char* _temp137= _temp135.curr +( j ++); if( _temp137 < _temp135.base?
1: _temp137 >= _temp135.last_plus_one){ _throw( Null_Exception);}* _temp137='\\';});({
struct _tagged_string _temp138= t; char* _temp140= _temp138.curr +( j ++); if(
_temp140 < _temp138.base? 1: _temp140 >= _temp138.last_plus_one){ _throw(
Null_Exception);}* _temp140='t';}); goto _LL84; _LL98:({ struct _tagged_string
_temp141= t; char* _temp143= _temp141.curr +( j ++); if( _temp143 < _temp141.base?
1: _temp143 >= _temp141.last_plus_one){ _throw( Null_Exception);}* _temp143='\\';});({
struct _tagged_string _temp144= t; char* _temp146= _temp144.curr +( j ++); if(
_temp146 < _temp144.base? 1: _temp146 >= _temp144.last_plus_one){ _throw(
Null_Exception);}* _temp146='v';}); goto _LL84; _LL100:({ struct _tagged_string
_temp147= t; char* _temp149= _temp147.curr +( j ++); if( _temp149 < _temp147.base?
1: _temp149 >= _temp147.last_plus_one){ _throw( Null_Exception);}* _temp149='\\';});({
struct _tagged_string _temp150= t; char* _temp152= _temp150.curr +( j ++); if(
_temp152 < _temp150.base? 1: _temp152 >= _temp150.last_plus_one){ _throw(
Null_Exception);}* _temp152='\\';}); goto _LL84; _LL102:({ struct _tagged_string
_temp153= t; char* _temp155= _temp153.curr +( j ++); if( _temp155 < _temp153.base?
1: _temp155 >= _temp153.last_plus_one){ _throw( Null_Exception);}* _temp155='\\';});({
struct _tagged_string _temp156= t; char* _temp158= _temp156.curr +( j ++); if(
_temp158 < _temp156.base? 1: _temp158 >= _temp156.last_plus_one){ _throw(
Null_Exception);}* _temp158='"';}); goto _LL84; _LL104: if(( int) _temp83 >=(
int)' '?( int) _temp83 <=( int)'~': 0){({ struct _tagged_string _temp159= t;
char* _temp161= _temp159.curr +( j ++); if( _temp161 < _temp159.base? 1:
_temp161 >= _temp159.last_plus_one){ _throw( Null_Exception);}* _temp161=
_temp83;});} else{({ struct _tagged_string _temp162= t; char* _temp164= _temp162.curr
+( j ++); if( _temp164 < _temp162.base? 1: _temp164 >= _temp162.last_plus_one){
_throw( Null_Exception);}* _temp164='\\';});({ struct _tagged_string _temp165= t;
char* _temp167= _temp165.curr +( j ++); if( _temp167 < _temp165.base? 1:
_temp167 >= _temp165.last_plus_one){ _throw( Null_Exception);}* _temp167=( char)((
int)'0' +(( int) _temp83 >> 6 & 7));});({ struct _tagged_string _temp168= t;
char* _temp170= _temp168.curr +( j ++); if( _temp170 < _temp168.base? 1:
_temp170 >= _temp168.last_plus_one){ _throw( Null_Exception);}* _temp170=( char)((
int)'0' +(( int) _temp83 >> 3 & 7));});({ struct _tagged_string _temp171= t;
char* _temp173= _temp171.curr +( j ++); if( _temp173 < _temp171.base? 1:
_temp173 >= _temp171.last_plus_one){ _throw( Null_Exception);}* _temp173=( char)((
int)'0' +(( int) _temp83 & 7));});} goto _LL84; _LL84:;}} return t;}}}} static
struct Cyc_PP_Doc* Cyc_Absynpp_textptr( struct _tagged_string* s){ return Cyc_PP_text(*
s);} static char _temp176[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string=(
struct _tagged_string){ _temp176, _temp176, _temp176 + 9u}; static char _temp179[
9u]="volatile"; static struct _tagged_string Cyc_Absynpp_volatile_string=(
struct _tagged_string){ _temp179, _temp179, _temp179 + 9u}; static char _temp182[
6u]="const"; static struct _tagged_string Cyc_Absynpp_const_string=( struct
_tagged_string){ _temp182, _temp182, _temp182 + 6u}; static struct
_tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string; static
struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_string;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual* tq){ struct
Cyc_List_List* l= 0; if( tq->q_restrict){ l=({ struct Cyc_List_List* _temp183=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp183->hd=(
void*) Cyc_Absynpp_restrict_sp; _temp183->tl= l; _temp183;});} if( tq->q_volatile){
l=({ struct Cyc_List_List* _temp184=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp184->hd=( void*) Cyc_Absynpp_volatile_sp; _temp184->tl=
l; _temp184;});} if( tq->q_const){ l=({ struct Cyc_List_List* _temp185=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp185->hd=( void*)
Cyc_Absynpp_const_sp; _temp185->tl= l; _temp185;});} return Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp186=( char*)""; struct _tagged_string
_temp187; _temp187.curr= _temp186; _temp187.base= _temp186; _temp187.last_plus_one=
_temp186 + 1; _temp187;}),( struct _tagged_string)({ char* _temp188=( char*)" ";
struct _tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 2; _temp189;}),( struct _tagged_string)({
char* _temp190=( char*)" "; struct _tagged_string _temp191; _temp191.curr=
_temp190; _temp191.base= _temp190; _temp191.last_plus_one= _temp190 + 2;
_temp191;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){ void* _temp192= k;
_LL194: if( _temp192 == Cyc_Absyn_AnyKind){ goto _LL195;} else{ goto _LL196;}
_LL196: if( _temp192 == Cyc_Absyn_MemKind){ goto _LL197;} else{ goto _LL198;}
_LL198: if( _temp192 == Cyc_Absyn_BoxKind){ goto _LL199;} else{ goto _LL200;}
_LL200: if( _temp192 == Cyc_Absyn_RgnKind){ goto _LL201;} else{ goto _LL202;}
_LL202: if( _temp192 == Cyc_Absyn_EffKind){ goto _LL203;} else{ goto _LL193;}
_LL195: return( struct _tagged_string)({ char* _temp204=( char*)"A"; struct
_tagged_string _temp205; _temp205.curr= _temp204; _temp205.base= _temp204;
_temp205.last_plus_one= _temp204 + 2; _temp205;}); _LL197: return( struct
_tagged_string)({ char* _temp206=( char*)"M"; struct _tagged_string _temp207;
_temp207.curr= _temp206; _temp207.base= _temp206; _temp207.last_plus_one=
_temp206 + 2; _temp207;}); _LL199: return( struct _tagged_string)({ char*
_temp208=( char*)"B"; struct _tagged_string _temp209; _temp209.curr= _temp208;
_temp209.base= _temp208; _temp209.last_plus_one= _temp208 + 2; _temp209;});
_LL201: return( struct _tagged_string)({ char* _temp210=( char*)"R"; struct
_tagged_string _temp211; _temp211.curr= _temp210; _temp211.base= _temp210;
_temp211.last_plus_one= _temp210 + 2; _temp211;}); _LL203: return( struct
_tagged_string)({ char* _temp212=( char*)"E"; struct _tagged_string _temp213;
_temp213.curr= _temp212; _temp213.base= _temp212; _temp213.last_plus_one=
_temp212 + 2; _temp213;}); _LL193:;} struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref* c){ void* _temp214=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c))->v; void* _temp220;
_LL216: if(( unsigned int) _temp214 > 1u?(( struct _enum_struct*) _temp214)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp214)->f1; goto _LL217;} else{ goto _LL218;} _LL218: goto _LL219; _LL217:
return Cyc_Absynpp_kind2string( _temp220); _LL219: return( struct _tagged_string)({
char* _temp222=( char*)"?"; struct _tagged_string _temp223; _temp223.curr=
_temp222; _temp223.base= _temp222; _temp223.last_plus_one= _temp222 + 2;
_temp223;}); _LL215:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return
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
char* _temp224=( char*)"<"; struct _tagged_string _temp225; _temp225.curr=
_temp224; _temp225.base= _temp224; _temp225.last_plus_one= _temp224 + 2;
_temp225;}),( struct _tagged_string)({ char* _temp226=( char*)">"; struct
_tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 2; _temp227;}),( struct _tagged_string)({
char* _temp228=( char*)","; struct _tagged_string _temp229; _temp229.curr=
_temp228; _temp229.base= _temp228; _temp229.last_plus_one= _temp228 + 2;
_temp229;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void* _temp230=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp238; void* _temp240; _LL232: if(( unsigned int)
_temp230 > 1u?(( struct _enum_struct*) _temp230)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp230)->f1;
if( _temp238 == Cyc_Absyn_BoxKind){ goto _LL233;} else{ goto _LL234;}} else{
goto _LL234;} _LL234: if(( unsigned int) _temp230 > 1u?(( struct _enum_struct*)
_temp230)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL241: _temp240=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp230)->f1; goto _LL235;} else{ goto
_LL236;} _LL236: goto _LL237; _LL233: return Cyc_Absynpp_textptr( tv->name);
_LL235: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp242=( char*)"::"; struct _tagged_string
_temp243; _temp243.curr= _temp242; _temp243.base= _temp242; _temp243.last_plus_one=
_temp242 + 3; _temp243;})), Cyc_Absynpp_kind2doc( _temp240)); _LL237: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp244=( char*)"::?"; struct _tagged_string _temp245; _temp245.curr= _temp244;
_temp245.base= _temp244; _temp245.last_plus_one= _temp244 + 4; _temp245;})));
_LL231:;} struct Cyc_PP_Doc* Cyc_Absynpp_kinded_tvars2doc( struct Cyc_List_List*
tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp246=( char*)"<";
struct _tagged_string _temp247; _temp247.curr= _temp246; _temp247.base= _temp246;
_temp247.last_plus_one= _temp246 + 2; _temp247;}),( struct _tagged_string)({
char* _temp248=( char*)">"; struct _tagged_string _temp249; _temp249.curr=
_temp248; _temp249.base= _temp248; _temp249.last_plus_one= _temp248 + 2;
_temp249;}),( struct _tagged_string)({ char* _temp250=( char*)","; struct
_tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 2; _temp251;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_kinded_tvars2doc(
tvs);} return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp252=(
char*)"<"; struct _tagged_string _temp253; _temp253.curr= _temp252; _temp253.base=
_temp252; _temp253.last_plus_one= _temp252 + 2; _temp253;}),( struct
_tagged_string)({ char* _temp254=( char*)">"; struct _tagged_string _temp255;
_temp255.curr= _temp254; _temp255.base= _temp254; _temp255.last_plus_one=
_temp254 + 2; _temp255;}),( struct _tagged_string)({ char* _temp256=( char*)",";
struct _tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 2; _temp257;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp258=( char*)"(";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 2; _temp259;}),( struct _tagged_string)({
char* _temp260=( char*)")"; struct _tagged_string _temp261; _temp261.curr=
_temp260; _temp261.base= _temp260; _temp261.last_plus_one= _temp260 + 2;
_temp261;}),( struct _tagged_string)({ char* _temp262=( char*)","; struct
_tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 2; _temp263;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){
return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc(
struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();} return
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp264=( char*)" __attribute__";
struct _tagged_string _temp265; _temp265.curr= _temp264; _temp265.base= _temp264;
_temp265.last_plus_one= _temp264 + 15; _temp265;})), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp266=( char*)"(("; struct _tagged_string _temp267;
_temp267.curr= _temp266; _temp267.base= _temp266; _temp267.last_plus_one=
_temp266 + 3; _temp267;}),( struct _tagged_string)({ char* _temp268=( char*)"))";
struct _tagged_string _temp269; _temp269.curr= _temp268; _temp269.base= _temp268;
_temp269.last_plus_one= _temp268 + 3; _temp269;}),( struct _tagged_string)({
char* _temp270=( char*)","; struct _tagged_string _temp271; _temp271.curr=
_temp270; _temp271.base= _temp270; _temp271.last_plus_one= _temp270 + 2;
_temp271;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)));} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp272=( void*)
tms->hd; struct Cyc_Absyn_Tqual* _temp278; void* _temp280; void* _temp282;
_LL274: if(( unsigned int) _temp272 > 1u?(( struct _enum_struct*) _temp272)->tag
== Cyc_Absyn_Pointer_mod_tag: 0){ _LL283: _temp282=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp272)->f1; goto _LL281; _LL281: _temp280=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp272)->f2; goto _LL279; _LL279: _temp278=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp272)->f3; goto _LL275;} else{ goto _LL276;}
_LL276: goto _LL277; _LL275: return 1; _LL277: return 0; _LL273:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d, tms->tl); struct Cyc_PP_Doc*
p_rest= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp284=(
char*)"("; struct _tagged_string _temp285; _temp285.curr= _temp284; _temp285.base=
_temp284; _temp285.last_plus_one= _temp284 + 2; _temp285;})), rest, Cyc_PP_text((
struct _tagged_string)({ char* _temp286=( char*)")"; struct _tagged_string
_temp287; _temp287.curr= _temp286; _temp287.base= _temp286; _temp287.last_plus_one=
_temp286 + 2; _temp287;}))); void* _temp288=( void*) tms->hd; struct Cyc_Absyn_Exp*
_temp302; void* _temp304; struct Cyc_List_List* _temp306; struct Cyc_Position_Segment*
_temp308; int _temp310; struct Cyc_Position_Segment* _temp312; struct Cyc_List_List*
_temp314; struct Cyc_Absyn_Tqual* _temp316; void* _temp318; void* _temp320;
_LL290: if( _temp288 == Cyc_Absyn_Carray_mod){ goto _LL291;} else{ goto _LL292;}
_LL292: if(( unsigned int) _temp288 > 1u?(( struct _enum_struct*) _temp288)->tag
== Cyc_Absyn_ConstArray_mod_tag: 0){ _LL303: _temp302=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ConstArray_mod_struct*) _temp288)->f1; goto _LL293;} else{ goto
_LL294;} _LL294: if(( unsigned int) _temp288 > 1u?(( struct _enum_struct*)
_temp288)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL305: _temp304=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp288)->f1; goto _LL295;} else{ goto
_LL296;} _LL296: if(( unsigned int) _temp288 > 1u?(( struct _enum_struct*)
_temp288)->tag == Cyc_Absyn_Attributes_mod_tag: 0){ _LL309: _temp308=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*) _temp288)->f1;
goto _LL307; _LL307: _temp306=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp288)->f2; goto _LL297;} else{ goto _LL298;} _LL298: if(( unsigned int)
_temp288 > 1u?(( struct _enum_struct*) _temp288)->tag == Cyc_Absyn_TypeParams_mod_tag:
0){ _LL315: _temp314=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp288)->f1; goto _LL313; _LL313: _temp312=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp288)->f2; goto _LL311; _LL311:
_temp310=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp288)->f3; goto
_LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp288 > 1u?(( struct
_enum_struct*) _temp288)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL321: _temp320=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp288)->f1; goto _LL319;
_LL319: _temp318=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp288)->f2;
goto _LL317; _LL317: _temp316=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp288)->f3; goto _LL301;} else{ goto _LL289;} _LL291: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp322=( char*)"[]"; struct _tagged_string _temp323;
_temp323.curr= _temp322; _temp323.base= _temp322; _temp323.last_plus_one=
_temp322 + 3; _temp323;}))); _LL293: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;} return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp324=( char*)"["; struct _tagged_string _temp325;
_temp325.curr= _temp324; _temp325.base= _temp324; _temp325.last_plus_one=
_temp324 + 2; _temp325;})), Cyc_Absynpp_exp2doc( _temp302), Cyc_PP_text(( struct
_tagged_string)({ char* _temp326=( char*)"]"; struct _tagged_string _temp327;
_temp327.curr= _temp326; _temp327.base= _temp326; _temp327.last_plus_one=
_temp326 + 2; _temp327;}))); _LL295: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;}{ void* _temp328= _temp304; struct Cyc_Core_Opt* _temp334; int
_temp336; struct Cyc_List_List* _temp338; struct Cyc_Position_Segment* _temp340;
struct Cyc_List_List* _temp342; _LL330: if((( struct _enum_struct*) _temp328)->tag
== Cyc_Absyn_WithTypes_tag){ _LL339: _temp338=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp328)->f1; goto _LL337; _LL337: _temp336=( int)((
struct Cyc_Absyn_WithTypes_struct*) _temp328)->f2; goto _LL335; _LL335: _temp334=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp328)->f3; goto
_LL331;} else{ goto _LL332;} _LL332: if((( struct _enum_struct*) _temp328)->tag
== Cyc_Absyn_NoTypes_tag){ _LL343: _temp342=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp328)->f1; goto _LL341; _LL341: _temp340=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp328)->f2; goto
_LL333;} else{ goto _LL329;} _LL331: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp338, _temp336, _temp334)); _LL333: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp344=( char*)"("; struct _tagged_string
_temp345; _temp345.curr= _temp344; _temp345.base= _temp344; _temp345.last_plus_one=
_temp344 + 2; _temp345;}),( struct _tagged_string)({ char* _temp346=( char*)")";
struct _tagged_string _temp347; _temp347.curr= _temp346; _temp347.base= _temp346;
_temp347.last_plus_one= _temp346 + 2; _temp347;}),( struct _tagged_string)({
char* _temp348=( char*)","; struct _tagged_string _temp349; _temp349.curr=
_temp348; _temp349.base= _temp348; _temp349.last_plus_one= _temp348 + 2;
_temp349;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp342))); _LL329:;} _LL297: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest=
p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc( _temp306)); _LL299:
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} if( _temp310){ return
Cyc_Absynpp_cat2( rest, Cyc_Absynpp_kinded_tvars2doc( _temp314));} else{ return
Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc( _temp314));} _LL301: { struct Cyc_PP_Doc*
ptr;{ void* _temp350= _temp320; struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Exp*
_temp360; _LL352: if(( unsigned int) _temp350 > 1u?(( struct _enum_struct*)
_temp350)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL359: _temp358=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Nullable_ps_struct*) _temp350)->f1; goto _LL353;} else{ goto
_LL354;} _LL354: if(( unsigned int) _temp350 > 1u?(( struct _enum_struct*)
_temp350)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL361: _temp360=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp350)->f1; goto
_LL355;} else{ goto _LL356;} _LL356: if( _temp350 == Cyc_Absyn_TaggedArray_ps){
goto _LL357;} else{ goto _LL351;} _LL353: if( Cyc_Evexp_eval_const_uint_exp(
_temp358) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp362=( char*)"*"; struct _tagged_string _temp363; _temp363.curr= _temp362;
_temp363.base= _temp362; _temp363.last_plus_one= _temp362 + 2; _temp363;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp364=( char*)"*"; struct _tagged_string _temp365; _temp365.curr= _temp364;
_temp365.base= _temp364; _temp365.last_plus_one= _temp364 + 2; _temp365;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp366=( char*)"{"; struct _tagged_string
_temp367; _temp367.curr= _temp366; _temp367.base= _temp366; _temp367.last_plus_one=
_temp366 + 2; _temp367;})), Cyc_Absynpp_exp2doc( _temp358), Cyc_PP_text(( struct
_tagged_string)({ char* _temp368=( char*)"}"; struct _tagged_string _temp369;
_temp369.curr= _temp368; _temp369.base= _temp368; _temp369.last_plus_one=
_temp368 + 2; _temp369;})));} goto _LL351; _LL355: if( Cyc_Evexp_eval_const_uint_exp(
_temp360) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp370=( char*)"@"; struct _tagged_string _temp371; _temp371.curr= _temp370;
_temp371.base= _temp370; _temp371.last_plus_one= _temp370 + 2; _temp371;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp372=( char*)"@"; struct _tagged_string _temp373; _temp373.curr= _temp372;
_temp373.base= _temp372; _temp373.last_plus_one= _temp372 + 2; _temp373;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp374=( char*)"{"; struct _tagged_string
_temp375; _temp375.curr= _temp374; _temp375.base= _temp374; _temp375.last_plus_one=
_temp374 + 2; _temp375;})), Cyc_Absynpp_exp2doc( _temp360), Cyc_PP_text(( struct
_tagged_string)({ char* _temp376=( char*)"}"; struct _tagged_string _temp377;
_temp377.curr= _temp376; _temp377.base= _temp376; _temp377.last_plus_one=
_temp376 + 2; _temp377;})));} goto _LL351; _LL357: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp378=( char*)"?"; struct _tagged_string _temp379;
_temp379.curr= _temp378; _temp379.base= _temp378; _temp379.last_plus_one=
_temp378 + 2; _temp379;})); goto _LL351; _LL351:;}{ void* _temp380= Cyc_Tcutil_compress(
_temp318); _LL382: if( _temp380 == Cyc_Absyn_HeapRgn){ goto _LL383;} else{ goto
_LL384;} _LL384: goto _LL385; _LL383: return Cyc_Absynpp_cat2( ptr, rest);
_LL385: return Cyc_Absynpp_cat3( ptr, Cyc_Absynpp_ntyp2doc( _temp318), rest);
_LL381:;}} _LL289:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void*
_temp386= Cyc_Tcutil_compress( t); _LL388: if( _temp386 == Cyc_Absyn_HeapRgn){
goto _LL389;} else{ goto _LL390;} _LL390: goto _LL391; _LL389: return Cyc_PP_text((
struct _tagged_string)({ char* _temp392=( char*)"`H"; struct _tagged_string
_temp393; _temp393.curr= _temp392; _temp393.base= _temp392; _temp393.last_plus_one=
_temp392 + 3; _temp393;})); _LL391: return Cyc_Absynpp_ntyp2doc( t); _LL387:;}
static struct _tuple8 Cyc_Absynpp_effects2docs( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp394= Cyc_Tcutil_compress(
t); void* _temp402; struct Cyc_List_List* _temp404; _LL396: if(( unsigned int)
_temp394 > 4u?(( struct _enum_struct*) _temp394)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL403: _temp402=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp394)->f1;
goto _LL397;} else{ goto _LL398;} _LL398: if(( unsigned int) _temp394 > 4u?((
struct _enum_struct*) _temp394)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL405:
_temp404=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp394)->f1;
goto _LL399;} else{ goto _LL400;} _LL400: goto _LL401; _LL397: regions=({ struct
Cyc_List_List* _temp406=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp406->hd=( void*) Cyc_Absynpp_rgn2doc( _temp402); _temp406->tl= regions;
_temp406;}); goto _LL395; _LL399: for( 0; _temp404 != 0; _temp404= _temp404->tl){
struct Cyc_List_List* _temp409; struct Cyc_List_List* _temp411; struct _tuple8
_temp407= Cyc_Absynpp_effects2docs(( void*) _temp404->hd); _LL412: _temp411=
_temp407.f1; goto _LL410; _LL410: _temp409= _temp407.f2; goto _LL408; _LL408:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp411, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp409, effects);} goto _LL395; _LL401: effects=({ struct Cyc_List_List*
_temp413=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp413->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp413->tl= effects; _temp413;});
goto _LL395; _LL395:;} return({ struct _tuple8 _temp414; _temp414.f1= regions;
_temp414.f2= effects; _temp414;});} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void*
t){ struct Cyc_List_List* _temp417; struct Cyc_List_List* _temp419; struct
_tuple8 _temp415= Cyc_Absynpp_effects2docs( t); _LL420: _temp419= _temp415.f1;
goto _LL418; _LL418: _temp417= _temp415.f2; goto _LL416; _LL416: _temp419=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp419);
_temp417=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp417);{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp421=( char*)"{"; struct _tagged_string _temp422; _temp422.curr=
_temp421; _temp422.base= _temp421; _temp422.last_plus_one= _temp421 + 2;
_temp422;}),( struct _tagged_string)({ char* _temp423=( char*)"}"; struct
_tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 2; _temp424;}),( struct _tagged_string)({
char* _temp425=( char*)","; struct _tagged_string _temp426; _temp426.curr=
_temp425; _temp426.base= _temp425; _temp426.last_plus_one= _temp425 + 2;
_temp426;}), _temp419); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp427=( char*)""; struct _tagged_string _temp428; _temp428.curr= _temp427;
_temp428.base= _temp427; _temp428.last_plus_one= _temp427 + 1; _temp428;}),(
struct _tagged_string)({ char* _temp429=( char*)""; struct _tagged_string
_temp430; _temp430.curr= _temp429; _temp430.base= _temp429; _temp430.last_plus_one=
_temp429 + 1; _temp430;}),( struct _tagged_string)({ char* _temp431=( char*)"+";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 2; _temp432;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp417,({ struct Cyc_List_List* _temp433=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp433->hd=( void*) doc1; _temp433->tl= 0;
_temp433;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp434= t; struct Cyc_Absyn_Exp* _temp474; struct Cyc_Absyn_Tqual*
_temp476; void* _temp478; struct Cyc_Absyn_FnInfo _temp480; struct Cyc_Absyn_PtrInfo
_temp482; int _temp484; struct Cyc_Core_Opt* _temp486; void* _temp488; struct
Cyc_Absyn_Tvar* _temp490; struct Cyc_Absyn_Enumdecl** _temp492; struct Cyc_List_List*
_temp494; struct _tuple0* _temp496; struct Cyc_Absyn_Xenumdecl** _temp498;
struct _tuple0* _temp500; void* _temp502; void* _temp504; struct Cyc_List_List*
_temp506; struct Cyc_Absyn_Structdecl** _temp508; struct Cyc_List_List* _temp510;
struct _tuple0* _temp512; struct Cyc_Absyn_Uniondecl** _temp514; struct Cyc_List_List*
_temp516; struct _tuple0* _temp518; struct Cyc_Core_Opt* _temp520; struct Cyc_List_List*
_temp522; struct _tuple0* _temp524; void* _temp526; void* _temp528; struct Cyc_List_List*
_temp530; _LL436: if(( unsigned int) _temp434 > 4u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL479: _temp478=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp434)->f1; goto _LL477; _LL477: _temp476=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp434)->f2;
goto _LL475; _LL475: _temp474=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp434)->f3; goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int)
_temp434 > 4u?(( struct _enum_struct*) _temp434)->tag == Cyc_Absyn_FnType_tag: 0){
_LL481: _temp480=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp434)->f1; goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp434 > 4u?(( struct _enum_struct*) _temp434)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL483: _temp482=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp434)->f1; goto _LL441;} else{ goto _LL442;} _LL442: if( _temp434 == Cyc_Absyn_VoidType){
goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_Evar_tag: 0){ _LL489: _temp488=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp434)->f1; goto _LL487; _LL487:
_temp486=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp434)->f2;
goto _LL485; _LL485: _temp484=( int)(( struct Cyc_Absyn_Evar_struct*) _temp434)->f3;
goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_VarType_tag: 0){ _LL491:
_temp490=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp434)->f1;
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL497:
_temp496=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp434)->f1;
goto _LL495; _LL495: _temp494=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp434)->f2; goto _LL493; _LL493: _temp492=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp434)->f3; goto _LL449;} else{ goto
_LL450;} _LL450: if(( unsigned int) _temp434 > 4u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL501: _temp500=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp434)->f1; goto _LL499; _LL499: _temp498=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp434)->f2;
goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_IntType_tag: 0){ _LL505:
_temp504=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp434)->f1; goto _LL503;
_LL503: _temp502=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp434)->f2;
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp434 == Cyc_Absyn_FloatType){
goto _LL455;} else{ goto _LL456;} _LL456: if( _temp434 == Cyc_Absyn_DoubleType){
goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL507:
_temp506=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp434)->f1;
goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_StructType_tag: 0){ _LL513:
_temp512=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp434)->f1;
goto _LL511; _LL511: _temp510=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp434)->f2; goto _LL509; _LL509: _temp508=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp434)->f3; goto _LL461;} else{ goto
_LL462;} _LL462: if(( unsigned int) _temp434 > 4u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL519: _temp518=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp434)->f1; goto _LL517; _LL517: _temp516=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp434)->f2; goto
_LL515; _LL515: _temp514=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp434)->f3; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp434 > 4u?(( struct _enum_struct*) _temp434)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL525: _temp524=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp434)->f1; goto _LL523; _LL523: _temp522=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp434)->f2; goto _LL521; _LL521: _temp520=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp434)->f3;
goto _LL465;} else{ goto _LL466;} _LL466: if(( unsigned int) _temp434 > 4u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL527:
_temp526=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp434)->f1; goto
_LL467;} else{ goto _LL468;} _LL468: if( _temp434 == Cyc_Absyn_HeapRgn){ goto
_LL469;} else{ goto _LL470;} _LL470: if(( unsigned int) _temp434 > 4u?(( struct
_enum_struct*) _temp434)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL529: _temp528=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp434)->f1; goto _LL471;} else{
goto _LL472;} _LL472: if(( unsigned int) _temp434 > 4u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL531: _temp530=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp434)->f1; goto _LL473;} else{ goto _LL435;}
_LL437: return Cyc_PP_text(( struct _tagged_string)({ char* _temp532=( char*)"[[[array]]]";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 12; _temp533;})); _LL439: return Cyc_PP_nil_doc();
_LL441: return Cyc_PP_nil_doc(); _LL443: s= Cyc_PP_text(( struct _tagged_string)({
char* _temp534=( char*)"void"; struct _tagged_string _temp535; _temp535.curr=
_temp534; _temp535.base= _temp534; _temp535.last_plus_one= _temp534 + 5;
_temp535;})); goto _LL435; _LL445: if( _temp486 != 0){ return Cyc_Absynpp_ntyp2doc((
void*) _temp486->v);} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp536=( char*)"%"; struct _tagged_string _temp537;
_temp537.curr= _temp536; _temp537.base= _temp536; _temp537.last_plus_one=
_temp536 + 2; _temp537;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp538=( char*)"("; struct _tagged_string _temp539; _temp539.curr= _temp538;
_temp539.base= _temp538; _temp539.last_plus_one= _temp538 + 2; _temp539;})), Cyc_PP_text(
xprintf("%d", _temp484)), Cyc_PP_text(( struct _tagged_string)({ char* _temp540=(
char*)")::"; struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base=
_temp540; _temp541.last_plus_one= _temp540 + 4; _temp541;})), Cyc_Absynpp_kind2doc(
_temp488));} goto _LL435; _LL447: if( Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3(
Cyc_Absynpp_textptr( _temp490->name), Cyc_PP_text(( struct _tagged_string)({
char* _temp542=( char*)"::"; struct _tagged_string _temp543; _temp543.curr=
_temp542; _temp543.base= _temp542; _temp543.last_plus_one= _temp542 + 3;
_temp543;})), Cyc_Absynpp_ckind2doc( _temp490->kind));} else{ s= Cyc_Absynpp_textptr(
_temp490->name);} goto _LL435; _LL449: if( _temp496 == 0){ s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp544=( char*)"enum "; struct
_tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 6; _temp545;})), Cyc_Absynpp_tps2doc(
_temp494));} else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp546=( char*)"enum "; struct _tagged_string _temp547; _temp547.curr=
_temp546; _temp547.base= _temp546; _temp547.last_plus_one= _temp546 + 6;
_temp547;})), Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp548=
_temp496; if( _temp548 == 0){ _throw( Null_Exception);} _temp548;})), Cyc_Absynpp_tps2doc(
_temp494));} goto _LL435; _LL451: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp549=( char*)"xenum "; struct _tagged_string
_temp550; _temp550.curr= _temp549; _temp550.base= _temp549; _temp550.last_plus_one=
_temp549 + 7; _temp550;})), Cyc_Absynpp_qvar2doc( _temp500)); goto _LL435;
_LL453: { struct _tagged_string sns; struct _tagged_string ts;{ void* _temp551=
_temp504; _LL553: if( _temp551 == Cyc_Absyn_Signed){ goto _LL554;} else{ goto
_LL555;} _LL555: if( _temp551 == Cyc_Absyn_Unsigned){ goto _LL556;} else{ goto
_LL552;} _LL554: sns=( struct _tagged_string)({ char* _temp557=( char*)"";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 1; _temp558;}); goto _LL552; _LL556: sns=(
struct _tagged_string)({ char* _temp559=( char*)"unsigned "; struct
_tagged_string _temp560; _temp560.curr= _temp559; _temp560.base= _temp559;
_temp560.last_plus_one= _temp559 + 10; _temp560;}); goto _LL552; _LL552:;}{ void*
_temp561= _temp502; _LL563: if( _temp561 == Cyc_Absyn_B1){ goto _LL564;} else{
goto _LL565;} _LL565: if( _temp561 == Cyc_Absyn_B2){ goto _LL566;} else{ goto
_LL567;} _LL567: if( _temp561 == Cyc_Absyn_B4){ goto _LL568;} else{ goto _LL569;}
_LL569: if( _temp561 == Cyc_Absyn_B8){ goto _LL570;} else{ goto _LL562;} _LL564:{
void* _temp571= _temp504; _LL573: if( _temp571 == Cyc_Absyn_Signed){ goto _LL574;}
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
_LL435;} _LL455: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp591=( char*)"float";
struct _tagged_string _temp592; _temp592.curr= _temp591; _temp592.base= _temp591;
_temp592.last_plus_one= _temp591 + 6; _temp592;})); goto _LL435; _LL457: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp593=( char*)"double"; struct _tagged_string
_temp594; _temp594.curr= _temp593; _temp594.base= _temp593; _temp594.last_plus_one=
_temp593 + 7; _temp594;})); goto _LL435; _LL459: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp595=( char*)"$"; struct _tagged_string
_temp596; _temp596.curr= _temp595; _temp596.base= _temp595; _temp596.last_plus_one=
_temp595 + 2; _temp596;})), Cyc_Absynpp_args2doc( _temp506)); goto _LL435;
_LL461: if( _temp512 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp597=( char*)"struct "; struct _tagged_string
_temp598; _temp598.curr= _temp597; _temp598.base= _temp597; _temp598.last_plus_one=
_temp597 + 8; _temp598;})), Cyc_Absynpp_tps2doc( _temp510));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp599=( char*)"struct "; struct
_tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 8; _temp600;})), _temp512 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp601= _temp512;
if( _temp601 == 0){ _throw( Null_Exception);} _temp601;})), Cyc_Absynpp_tps2doc(
_temp510));} goto _LL435; _LL463: if( _temp518 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp602=( char*)"union "; struct _tagged_string
_temp603; _temp603.curr= _temp602; _temp603.base= _temp602; _temp603.last_plus_one=
_temp602 + 7; _temp603;})), Cyc_Absynpp_tps2doc( _temp516));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp604=( char*)"union "; struct
_tagged_string _temp605; _temp605.curr= _temp604; _temp605.base= _temp604;
_temp605.last_plus_one= _temp604 + 7; _temp605;})), _temp518 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp606= _temp518;
if( _temp606 == 0){ _throw( Null_Exception);} _temp606;})), Cyc_Absynpp_tps2doc(
_temp516));} goto _LL435; _LL465: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp524), Cyc_Absynpp_tps2doc( _temp522)); goto _LL435; _LL467: s= Cyc_Absynpp_rgn2doc(
_temp526); goto _LL435; _LL469: s= Cyc_Absynpp_rgn2doc( t); goto _LL435; _LL471:
s= Cyc_Absynpp_eff2doc( t); goto _LL435; _LL473: s= Cyc_Absynpp_eff2doc( t);
goto _LL435; _LL435:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
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
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp654->f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp655=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp655->v=( void*)(* arg).f1; _temp655;}); _temp654->f2=(* arg).f2;
_temp654->f3=(* arg).f3; _temp654;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc(
struct _tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
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
_temp677 + 2; _temp678;})));} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp679=( char*)""; struct _tagged_string
_temp680; _temp680.curr= _temp679; _temp680.base= _temp679; _temp680.last_plus_one=
_temp679 + 1; _temp680;}),( struct _tagged_string)({ char* _temp681=( char*)"::";
struct _tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 3; _temp682;}),( struct _tagged_string)({
char* _temp683=( char*)"::"; struct _tagged_string _temp684; _temp684.curr=
_temp683; _temp684.base= _temp683; _temp684.last_plus_one= _temp683 + 3;
_temp684;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_var2doc,
prefix)), Cyc_Absynpp_var2doc((* q).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp685=( void*) e->r; void* _temp753; void*
_temp755; struct _tuple0* _temp757; struct _tuple0* _temp759; struct Cyc_List_List*
_temp761; void* _temp763; struct Cyc_Absyn_Exp* _temp765; struct Cyc_Core_Opt*
_temp767; struct Cyc_Absyn_Exp* _temp769; void* _temp771; struct Cyc_Absyn_Exp*
_temp773; struct Cyc_Absyn_Exp* _temp775; struct Cyc_Absyn_Exp* _temp777; struct
Cyc_Absyn_Exp* _temp779; struct Cyc_Absyn_Exp* _temp781; struct Cyc_Absyn_Exp*
_temp783; struct Cyc_List_List* _temp785; struct Cyc_Absyn_Exp* _temp787; struct
Cyc_List_List* _temp789; struct Cyc_Absyn_Exp* _temp791; struct Cyc_Absyn_Exp*
_temp793; struct Cyc_Absyn_Exp* _temp795; struct Cyc_List_List* _temp797; struct
Cyc_Absyn_Exp* _temp799; struct Cyc_Absyn_Exp* _temp801; void* _temp803; struct
Cyc_Absyn_Exp* _temp805; void* _temp807; struct Cyc_Absyn_Exp* _temp809; struct
Cyc_Absyn_Exp* _temp811; struct _tagged_string* _temp813; struct Cyc_Absyn_Exp*
_temp815; struct _tagged_string* _temp817; struct Cyc_Absyn_Exp* _temp819;
struct Cyc_Absyn_Exp* _temp821; struct Cyc_Absyn_Exp* _temp823; struct Cyc_List_List*
_temp825; struct Cyc_List_List* _temp827; struct _tuple1* _temp829; struct Cyc_List_List*
_temp831; int _temp833; struct Cyc_Absyn_Exp* _temp835; struct Cyc_Absyn_Exp*
_temp837; struct Cyc_Absyn_Vardecl* _temp839; struct Cyc_Absyn_Structdecl*
_temp841; struct Cyc_List_List* _temp843; struct Cyc_Core_Opt* _temp845; struct
_tuple0* _temp847; struct Cyc_Absyn_Enumfield* _temp849; struct Cyc_Absyn_Enumdecl*
_temp851; struct Cyc_List_List* _temp853; struct Cyc_Core_Opt* _temp855; struct
Cyc_Core_Opt* _temp857; struct Cyc_Absyn_Enumfield* _temp859; struct Cyc_Absyn_Xenumdecl*
_temp861; struct Cyc_List_List* _temp863; struct Cyc_Core_Opt* _temp865; void*
_temp867; struct Cyc_List_List* _temp869; struct Cyc_Core_Opt* _temp871; struct
Cyc_Absyn_Stmt* _temp873; struct Cyc_Absyn_Fndecl* _temp875; struct Cyc_Absyn_Exp*
_temp877; _LL687: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_Const_e_tag){
_LL754: _temp753=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp685)->f1;
goto _LL688;} else{ goto _LL689;} _LL689: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Var_e_tag){ _LL758: _temp757=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp685)->f1; goto _LL756; _LL756: _temp755=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp685)->f2; goto _LL690;} else{ goto _LL691;} _LL691: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL760: _temp759=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp685)->f1; goto
_LL692;} else{ goto _LL693;} _LL693: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Primop_e_tag){ _LL764: _temp763=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp685)->f1; goto _LL762; _LL762: _temp761=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp685)->f2; goto _LL694;} else{ goto _LL695;}
_LL695: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL770: _temp769=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp685)->f1; goto _LL768; _LL768: _temp767=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp685)->f2; goto _LL766; _LL766: _temp765=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp685)->f3;
goto _LL696;} else{ goto _LL697;} _LL697: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Increment_e_tag){ _LL774: _temp773=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp685)->f1; goto _LL772; _LL772:
_temp771=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp685)->f2; goto
_LL698;} else{ goto _LL699;} _LL699: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL780: _temp779=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp685)->f1; goto _LL778; _LL778:
_temp777=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp685)->f2; goto _LL776; _LL776: _temp775=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp685)->f3; goto _LL700;} else{ goto _LL701;}
_LL701: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL784: _temp783=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp685)->f1; goto _LL782; _LL782: _temp781=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp685)->f2; goto _LL702;} else{ goto _LL703;}
_LL703: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL788: _temp787=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp685)->f1; goto _LL786; _LL786: _temp785=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp685)->f2; goto _LL704;} else{ goto _LL705;}
_LL705: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_FnCall_e_tag){
_LL792: _temp791=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp685)->f1; goto _LL790; _LL790: _temp789=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp685)->f2; goto _LL706;} else{ goto _LL707;}
_LL707: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_Throw_e_tag){
_LL794: _temp793=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp685)->f1; goto _LL708;} else{ goto _LL709;} _LL709: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL796:
_temp795=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp685)->f1; goto _LL710;} else{ goto _LL711;} _LL711: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL800: _temp799=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp685)->f1;
goto _LL798; _LL798: _temp797=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp685)->f2; goto _LL712;} else{ goto _LL713;} _LL713: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Cast_e_tag){ _LL804: _temp803=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp685)->f1; goto _LL802; _LL802: _temp801=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp685)->f2; goto
_LL714;} else{ goto _LL715;} _LL715: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Address_e_tag){ _LL806: _temp805=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp685)->f1; goto _LL716;} else{ goto _LL717;}
_LL717: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL808: _temp807=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp685)->f1;
goto _LL718;} else{ goto _LL719;} _LL719: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL810: _temp809=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp685)->f1; goto _LL720;} else{ goto
_LL721;} _LL721: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_Deref_e_tag){
_LL812: _temp811=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp685)->f1; goto _LL722;} else{ goto _LL723;} _LL723: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_StructMember_e_tag){ _LL816: _temp815=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp685)->f1;
goto _LL814; _LL814: _temp813=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp685)->f2; goto _LL724;} else{ goto _LL725;} _LL725: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL820: _temp819=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp685)->f1;
goto _LL818; _LL818: _temp817=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp685)->f2; goto _LL726;} else{ goto _LL727;} _LL727: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Subscript_e_tag){ _LL824: _temp823=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp685)->f1;
goto _LL822; _LL822: _temp821=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp685)->f2; goto _LL728;} else{ goto _LL729;} _LL729: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Tuple_e_tag){ _LL826: _temp825=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp685)->f1; goto
_LL730;} else{ goto _LL731;} _LL731: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL830: _temp829=( struct _tuple1*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp685)->f1; goto _LL828; _LL828: _temp827=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp685)->f2;
goto _LL732;} else{ goto _LL733;} _LL733: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Array_e_tag){ _LL834: _temp833=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp685)->f1; goto _LL832; _LL832: _temp831=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp685)->f2; goto _LL734;} else{ goto _LL735;}
_LL735: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL840: _temp839=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp685)->f1; goto _LL838; _LL838: _temp837=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp685)->f2; goto _LL836; _LL836: _temp835=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp685)->f3;
goto _LL736;} else{ goto _LL737;} _LL737: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Struct_e_tag){ _LL848: _temp847=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp685)->f1; goto _LL846; _LL846: _temp845=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp685)->f2; goto _LL844; _LL844: _temp843=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp685)->f3; goto
_LL842; _LL842: _temp841=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp685)->f4; goto _LL738;} else{ goto _LL739;} _LL739: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Enum_e_tag){ _LL858: _temp857=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp685)->f1; goto
_LL856; _LL856: _temp855=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp685)->f2; goto _LL854; _LL854: _temp853=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp685)->f3; goto _LL852; _LL852: _temp851=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp685)->f4; goto
_LL850; _LL850: _temp849=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp685)->f5; goto _LL740;} else{ goto _LL741;} _LL741: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Xenum_e_tag){ _LL866: _temp865=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp685)->f1; goto
_LL864; _LL864: _temp863=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp685)->f2; goto _LL862; _LL862: _temp861=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp685)->f3; goto _LL860; _LL860: _temp859=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp685)->f4;
goto _LL742;} else{ goto _LL743;} _LL743: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL868: _temp867=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp685)->f1; goto _LL744;} else{ goto _LL745;} _LL745: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL872:
_temp871=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp685)->f1; goto _LL870; _LL870: _temp869=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp685)->f2; goto _LL746;} else{ goto
_LL747;} _LL747: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL874: _temp873=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp685)->f1; goto _LL748;} else{ goto _LL749;} _LL749: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_Codegen_e_tag){ _LL876: _temp875=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp685)->f1;
goto _LL750;} else{ goto _LL751;} _LL751: if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Fill_e_tag){ _LL878: _temp877=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp685)->f1; goto _LL752;} else{ goto _LL686;} _LL688: return 10000; _LL690:
return 10000; _LL692: return 10000; _LL694: { void* _temp879= _temp763; _LL881:
if( _temp879 == Cyc_Absyn_Plus){ goto _LL882;} else{ goto _LL883;} _LL883: if(
_temp879 == Cyc_Absyn_Times){ goto _LL884;} else{ goto _LL885;} _LL885: if(
_temp879 == Cyc_Absyn_Minus){ goto _LL886;} else{ goto _LL887;} _LL887: if(
_temp879 == Cyc_Absyn_Div){ goto _LL888;} else{ goto _LL889;} _LL889: if(
_temp879 == Cyc_Absyn_Mod){ goto _LL890;} else{ goto _LL891;} _LL891: if(
_temp879 == Cyc_Absyn_Eq){ goto _LL892;} else{ goto _LL893;} _LL893: if(
_temp879 == Cyc_Absyn_Neq){ goto _LL894;} else{ goto _LL895;} _LL895: if(
_temp879 == Cyc_Absyn_Gt){ goto _LL896;} else{ goto _LL897;} _LL897: if(
_temp879 == Cyc_Absyn_Lt){ goto _LL898;} else{ goto _LL899;} _LL899: if(
_temp879 == Cyc_Absyn_Gte){ goto _LL900;} else{ goto _LL901;} _LL901: if(
_temp879 == Cyc_Absyn_Lte){ goto _LL902;} else{ goto _LL903;} _LL903: if(
_temp879 == Cyc_Absyn_Not){ goto _LL904;} else{ goto _LL905;} _LL905: if(
_temp879 == Cyc_Absyn_Bitnot){ goto _LL906;} else{ goto _LL907;} _LL907: if(
_temp879 == Cyc_Absyn_Bitand){ goto _LL908;} else{ goto _LL909;} _LL909: if(
_temp879 == Cyc_Absyn_Bitor){ goto _LL910;} else{ goto _LL911;} _LL911: if(
_temp879 == Cyc_Absyn_Bitxor){ goto _LL912;} else{ goto _LL913;} _LL913: if(
_temp879 == Cyc_Absyn_Bitlshift){ goto _LL914;} else{ goto _LL915;} _LL915: if(
_temp879 == Cyc_Absyn_Bitlrshift){ goto _LL916;} else{ goto _LL917;} _LL917: if(
_temp879 == Cyc_Absyn_Bitarshift){ goto _LL918;} else{ goto _LL919;} _LL919: if(
_temp879 == Cyc_Absyn_Size){ goto _LL920;} else{ goto _LL921;} _LL921: if(
_temp879 == Cyc_Absyn_Printf){ goto _LL922;} else{ goto _LL923;} _LL923: if(
_temp879 == Cyc_Absyn_Fprintf){ goto _LL924;} else{ goto _LL925;} _LL925: if(
_temp879 == Cyc_Absyn_Xprintf){ goto _LL926;} else{ goto _LL927;} _LL927: if(
_temp879 == Cyc_Absyn_Scanf){ goto _LL928;} else{ goto _LL929;} _LL929: if(
_temp879 == Cyc_Absyn_Fscanf){ goto _LL930;} else{ goto _LL931;} _LL931: if(
_temp879 == Cyc_Absyn_Sscanf){ goto _LL932;} else{ goto _LL880;} _LL882: return
100; _LL884: return 110; _LL886: return 100; _LL888: return 110; _LL890: return
110; _LL892: return 70; _LL894: return 70; _LL896: return 80; _LL898: return 80;
_LL900: return 80; _LL902: return 80; _LL904: return 130; _LL906: return 130;
_LL908: return 60; _LL910: return 40; _LL912: return 50; _LL914: return 90;
_LL916: return 80; _LL918: return 80; _LL920: return 140; _LL922: return 130;
_LL924: return 130; _LL926: return 130; _LL928: return 130; _LL930: return 130;
_LL932: return 130; _LL880:;} _LL696: return 20; _LL698: return 20; _LL700:
return 30; _LL702: return 10; _LL704: return 140; _LL706: return 140; _LL708:
return 130; _LL710: return Cyc_Absynpp_exp_prec( _temp795); _LL712: return Cyc_Absynpp_exp_prec(
_temp799); _LL714: return 120; _LL716: return 130; _LL718: return 130; _LL720:
return 130; _LL722: return 130; _LL724: return 140; _LL726: return 140; _LL728:
return 140; _LL730: return 150; _LL732: goto _LL734; _LL734: goto _LL736; _LL736:
goto _LL738; _LL738: goto _LL740; _LL740: goto _LL742; _LL742: goto _LL744;
_LL744: return 140; _LL746: return 140; _LL748: return 10000; _LL750: return 140;
_LL752: return 140; _LL686:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*
e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp933=( void*) e->r; void* _temp1007; void*
_temp1009; struct Cyc_Absyn_Vardecl* _temp1011; struct _tuple0* _temp1013; void*
_temp1015; struct Cyc_Absyn_Vardecl* _temp1017; struct _tuple0* _temp1019; void*
_temp1021; struct _tuple0* _temp1023; struct _tuple0* _temp1025; struct Cyc_List_List*
_temp1027; void* _temp1029; struct Cyc_Absyn_Exp* _temp1031; struct Cyc_Core_Opt*
_temp1033; struct Cyc_Absyn_Exp* _temp1035; void* _temp1037; struct Cyc_Absyn_Exp*
_temp1039; struct Cyc_Absyn_Exp* _temp1041; struct Cyc_Absyn_Exp* _temp1043;
struct Cyc_Absyn_Exp* _temp1045; struct Cyc_Absyn_Exp* _temp1047; struct Cyc_Absyn_Exp*
_temp1049; struct Cyc_List_List* _temp1051; struct Cyc_Absyn_Exp* _temp1053;
struct Cyc_List_List* _temp1055; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Exp*
_temp1059; struct Cyc_Absyn_Exp* _temp1061; struct Cyc_List_List* _temp1063;
struct Cyc_Absyn_Exp* _temp1065; struct Cyc_Absyn_Exp* _temp1067; void*
_temp1069; struct Cyc_Absyn_Exp* _temp1071; void* _temp1073; struct Cyc_Absyn_Exp*
_temp1075; struct Cyc_Absyn_Exp* _temp1077; struct _tagged_string* _temp1079;
struct Cyc_Absyn_Exp* _temp1081; struct _tagged_string* _temp1083; struct Cyc_Absyn_Exp*
_temp1085; struct Cyc_Absyn_Exp* _temp1087; struct Cyc_Absyn_Exp* _temp1089;
struct Cyc_List_List* _temp1091; struct Cyc_List_List* _temp1093; struct _tuple1*
_temp1095; struct Cyc_List_List* _temp1097; int _temp1099; struct Cyc_List_List*
_temp1101; int _temp1103; struct Cyc_Absyn_Exp* _temp1105; struct Cyc_Absyn_Exp*
_temp1107; struct Cyc_Absyn_Vardecl* _temp1109; struct Cyc_Absyn_Structdecl*
_temp1111; struct Cyc_List_List* _temp1113; struct Cyc_Core_Opt* _temp1115;
struct _tuple0* _temp1117; struct Cyc_Absyn_Enumfield* _temp1119; struct Cyc_Absyn_Enumdecl*
_temp1121; struct Cyc_List_List* _temp1123; struct Cyc_Core_Opt* _temp1125;
struct Cyc_Core_Opt* _temp1127; struct Cyc_Absyn_Enumfield* _temp1129; struct
Cyc_Absyn_Xenumdecl* _temp1131; struct Cyc_List_List* _temp1133; struct Cyc_Core_Opt*
_temp1135; void* _temp1137; struct Cyc_List_List* _temp1139; struct Cyc_Core_Opt*
_temp1141; struct Cyc_Absyn_Stmt* _temp1143; struct Cyc_Absyn_Fndecl* _temp1145;
struct Cyc_Absyn_Exp* _temp1147; _LL935: if((( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Const_e_tag){ _LL1008: _temp1007=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp933)->f1; goto _LL936;} else{ goto _LL937;} _LL937: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Var_e_tag){ _LL1014: _temp1013=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp933)->f1; goto _LL1010;
_LL1010: _temp1009=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp933)->f2; if((
unsigned int) _temp1009 > 1u?(( struct _enum_struct*) _temp1009)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL1012: _temp1011=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp1009)->f1; goto _LL938;} else{ goto _LL939;}} else{ goto _LL939;} _LL939:
if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Var_e_tag){ _LL1020:
_temp1019=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp933)->f1;
goto _LL1016; _LL1016: _temp1015=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp933)->f2; if(( unsigned int) _temp1015 > 1u?(( struct _enum_struct*)
_temp1015)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL1018: _temp1017=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp1015)->f1; goto _LL940;} else{ goto _LL941;}}
else{ goto _LL941;} _LL941: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Var_e_tag){
_LL1024: _temp1023=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp933)->f1;
goto _LL1022; _LL1022: _temp1021=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp933)->f2; goto _LL942;} else{ goto _LL943;} _LL943: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1026: _temp1025=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp933)->f1; goto
_LL944;} else{ goto _LL945;} _LL945: if((( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Primop_e_tag){ _LL1030: _temp1029=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp933)->f1; goto _LL1028; _LL1028: _temp1027=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp933)->f2; goto _LL946;} else{ goto
_LL947;} _LL947: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1036: _temp1035=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp933)->f1; goto _LL1034; _LL1034: _temp1033=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp933)->f2; goto _LL1032; _LL1032: _temp1031=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp933)->f3;
goto _LL948;} else{ goto _LL949;} _LL949: if((( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1040: _temp1039=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp933)->f1; goto _LL1038; _LL1038:
_temp1037=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp933)->f2; goto
_LL950;} else{ goto _LL951;} _LL951: if((( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL1046: _temp1045=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp933)->f1; goto _LL1044; _LL1044:
_temp1043=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp933)->f2; goto _LL1042; _LL1042: _temp1041=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp933)->f3; goto _LL952;} else{ goto
_LL953;} _LL953: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL1050: _temp1049=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp933)->f1; goto _LL1048; _LL1048: _temp1047=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp933)->f2; goto _LL954;} else{ goto
_LL955;} _LL955: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL1054: _temp1053=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp933)->f1; goto _LL1052; _LL1052: _temp1051=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp933)->f2; goto _LL956;} else{ goto
_LL957;} _LL957: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1058: _temp1057=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp933)->f1; goto _LL1056; _LL1056: _temp1055=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp933)->f2; goto _LL958;} else{ goto
_LL959;} _LL959: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Throw_e_tag){
_LL1060: _temp1059=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp933)->f1; goto _LL960;} else{ goto _LL961;} _LL961: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1062:
_temp1061=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp933)->f1; goto _LL962;} else{ goto _LL963;} _LL963: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1066:
_temp1065=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp933)->f1; goto _LL1064; _LL1064: _temp1063=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp933)->f2; goto _LL964;} else{ goto
_LL965;} _LL965: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Cast_e_tag){
_LL1070: _temp1069=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp933)->f1;
goto _LL1068; _LL1068: _temp1067=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp933)->f2; goto _LL966;} else{ goto _LL967;} _LL967: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Address_e_tag){ _LL1072: _temp1071=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp933)->f1; goto
_LL968;} else{ goto _LL969;} _LL969: if((( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Sizeoftyp_e_tag){ _LL1074: _temp1073=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp933)->f1; goto _LL970;} else{ goto _LL971;} _LL971: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1076: _temp1075=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp933)->f1;
goto _LL972;} else{ goto _LL973;} _LL973: if((( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1078: _temp1077=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp933)->f1; goto _LL974;} else{ goto _LL975;}
_LL975: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1082: _temp1081=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp933)->f1; goto _LL1080; _LL1080: _temp1079=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp933)->f2; goto _LL976;} else{ goto
_LL977;} _LL977: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1086: _temp1085=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp933)->f1; goto _LL1084; _LL1084: _temp1083=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp933)->f2; goto _LL978;} else{ goto
_LL979;} _LL979: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1090: _temp1089=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp933)->f1; goto _LL1088; _LL1088: _temp1087=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp933)->f2; goto _LL980;} else{ goto
_LL981;} _LL981: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1092: _temp1091=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp933)->f1; goto _LL982;} else{ goto _LL983;} _LL983: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1096:
_temp1095=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp933)->f1;
goto _LL1094; _LL1094: _temp1093=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp933)->f2; goto _LL984;} else{ goto _LL985;} _LL985: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Array_e_tag){ _LL1100: _temp1099=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp933)->f1; if( _temp1099 == 1){
goto _LL1098;} else{ goto _LL987;} _LL1098: _temp1097=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp933)->f2; goto _LL986;} else{ goto _LL987;}
_LL987: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Array_e_tag){
_LL1104: _temp1103=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp933)->f1;
goto _LL1102; _LL1102: _temp1101=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp933)->f2; goto _LL988;} else{ goto _LL989;} _LL989: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1110:
_temp1109=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp933)->f1; goto _LL1108; _LL1108: _temp1107=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp933)->f2; goto _LL1106; _LL1106:
_temp1105=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp933)->f3; goto _LL990;} else{ goto _LL991;} _LL991: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Struct_e_tag){ _LL1118: _temp1117=(
struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp933)->f1; goto
_LL1116; _LL1116: _temp1115=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp933)->f2; goto _LL1114; _LL1114: _temp1113=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp933)->f3; goto _LL1112; _LL1112:
_temp1111=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp933)->f4; goto _LL992;} else{ goto _LL993;} _LL993: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Enum_e_tag){ _LL1128: _temp1127=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp933)->f1; goto
_LL1126; _LL1126: _temp1125=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp933)->f2; goto _LL1124; _LL1124: _temp1123=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp933)->f3; goto _LL1122; _LL1122: _temp1121=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp933)->f4;
goto _LL1120; _LL1120: _temp1119=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp933)->f5; goto _LL994;} else{ goto _LL995;} _LL995: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1136: _temp1135=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp933)->f1; goto
_LL1134; _LL1134: _temp1133=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp933)->f2; goto _LL1132; _LL1132: _temp1131=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp933)->f3; goto _LL1130; _LL1130:
_temp1129=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp933)->f4; goto _LL996;} else{ goto _LL997;} _LL997: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1138: _temp1137=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp933)->f1; goto _LL998;} else{
goto _LL999;} _LL999: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1142: _temp1141=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp933)->f1; goto _LL1140; _LL1140: _temp1139=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp933)->f2; goto _LL1000;} else{
goto _LL1001;} _LL1001: if((( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1144: _temp1143=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp933)->f1; goto _LL1002;} else{ goto _LL1003;} _LL1003: if((( struct
_enum_struct*) _temp933)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1146: _temp1145=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp933)->f1;
goto _LL1004;} else{ goto _LL1005;} _LL1005: if((( struct _enum_struct*)
_temp933)->tag == Cyc_Absyn_Fill_e_tag){ _LL1148: _temp1147=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp933)->f1; goto _LL1006;} else{ goto _LL934;}
_LL936: s= Cyc_Absynpp_cnst2doc( _temp1007); goto _LL934; _LL938: s= Cyc_Absynpp_qvar2doc(
_temp1013); if( _temp1011->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp1011->shadow)));} goto _LL934; _LL940: s= Cyc_Absynpp_qvar2doc(
_temp1019); if( _temp1017->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp1017->shadow)));} goto _LL934; _LL942: s= Cyc_Absynpp_qvar2doc(
_temp1023); goto _LL934; _LL944: s= Cyc_Absynpp_qvar2doc( _temp1025); goto
_LL934; _LL946: s= Cyc_Absynpp_primapp2doc( myprec, _temp1029, _temp1027); goto
_LL934; _LL948: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec, _temp1035),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1149=( char*)" "; struct
_tagged_string _temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149;
_temp1150.last_plus_one= _temp1149 + 2; _temp1150;})), _temp1033 == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp1151=( char*)""; struct _tagged_string
_temp1152; _temp1152.curr= _temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one=
_temp1151 + 1; _temp1152;})): Cyc_Absynpp_prim2doc(( void*) _temp1033->v), Cyc_PP_text((
struct _tagged_string)({ char* _temp1153=( char*)"= "; struct _tagged_string
_temp1154; _temp1154.curr= _temp1153; _temp1154.base= _temp1153; _temp1154.last_plus_one=
_temp1153 + 3; _temp1154;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1031));
goto _LL934; _LL950: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1039);{ void* _temp1155= _temp1037; _LL1157: if( _temp1155 == Cyc_Absyn_PreInc){
goto _LL1158;} else{ goto _LL1159;} _LL1159: if( _temp1155 == Cyc_Absyn_PreDec){
goto _LL1160;} else{ goto _LL1161;} _LL1161: if( _temp1155 == Cyc_Absyn_PostInc){
goto _LL1162;} else{ goto _LL1163;} _LL1163: if( _temp1155 == Cyc_Absyn_PostDec){
goto _LL1164;} else{ goto _LL1156;} _LL1158: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1165=( char*)"++"; struct _tagged_string
_temp1166; _temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 3; _temp1166;})), es); goto _LL1156; _LL1160: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1167=( char*)"--"; struct
_tagged_string _temp1168; _temp1168.curr= _temp1167; _temp1168.base= _temp1167;
_temp1168.last_plus_one= _temp1167 + 3; _temp1168;})), es); goto _LL1156;
_LL1162: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1169=( char*)"++"; struct _tagged_string _temp1170; _temp1170.curr=
_temp1169; _temp1170.base= _temp1169; _temp1170.last_plus_one= _temp1169 + 3;
_temp1170;}))); goto _LL1156; _LL1164: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1171=( char*)"--"; struct _tagged_string
_temp1172; _temp1172.curr= _temp1171; _temp1172.base= _temp1171; _temp1172.last_plus_one=
_temp1171 + 3; _temp1172;}))); goto _LL1156; _LL1156:;} goto _LL934;} _LL952:{
struct _tuple9 _temp1174=({ struct _tuple9 _temp1173; _temp1173.f1=( void*)
_temp1043->r; _temp1173.f2=( void*) _temp1041->r; _temp1173;}); _LL1176: goto
_LL1177; _LL1177: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1045), Cyc_PP_text(( struct _tagged_string)({ char* _temp1178=( char*)" ? ";
struct _tagged_string _temp1179; _temp1179.curr= _temp1178; _temp1179.base=
_temp1178; _temp1179.last_plus_one= _temp1178 + 4; _temp1179;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1043), Cyc_PP_text(( struct _tagged_string)({ char* _temp1180=( char*)" : ";
struct _tagged_string _temp1181; _temp1181.curr= _temp1180; _temp1181.base=
_temp1180; _temp1181.last_plus_one= _temp1180 + 4; _temp1181;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1041)); goto _LL1175; _LL1175:;} goto _LL934; _LL954: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1049), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1182=( char*)", "; struct _tagged_string _temp1183;
_temp1183.curr= _temp1182; _temp1183.base= _temp1182; _temp1183.last_plus_one=
_temp1182 + 3; _temp1183;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1047));
goto _LL934; _LL956: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1053), Cyc_PP_text(( struct _tagged_string)({ char* _temp1184=( char*)"(";
struct _tagged_string _temp1185; _temp1185.curr= _temp1184; _temp1185.base=
_temp1184; _temp1185.last_plus_one= _temp1184 + 2; _temp1185;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1051), Cyc_PP_text(( struct _tagged_string)({ char* _temp1186=( char*)")";
struct _tagged_string _temp1187; _temp1187.curr= _temp1186; _temp1187.base=
_temp1186; _temp1187.last_plus_one= _temp1186 + 2; _temp1187;}))); goto _LL934;
_LL958: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1057), Cyc_PP_text((
struct _tagged_string)({ char* _temp1188=( char*)"("; struct _tagged_string
_temp1189; _temp1189.curr= _temp1188; _temp1189.base= _temp1188; _temp1189.last_plus_one=
_temp1188 + 2; _temp1189;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1055), Cyc_PP_text((
struct _tagged_string)({ char* _temp1190=( char*)")"; struct _tagged_string
_temp1191; _temp1191.curr= _temp1190; _temp1191.base= _temp1190; _temp1191.last_plus_one=
_temp1190 + 2; _temp1191;}))); goto _LL934; _LL960: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1192=( char*)"throw "; struct _tagged_string
_temp1193; _temp1193.curr= _temp1192; _temp1193.base= _temp1192; _temp1193.last_plus_one=
_temp1192 + 7; _temp1193;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1059));
goto _LL934; _LL962: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1061); goto
_LL934; _LL964: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1065); goto _LL934;
_LL966: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1194=( char*)"("; struct _tagged_string _temp1195; _temp1195.curr=
_temp1194; _temp1195.base= _temp1194; _temp1195.last_plus_one= _temp1194 + 2;
_temp1195;})), Cyc_Absynpp_typ2doc( _temp1069), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1196=( char*)")"; struct _tagged_string _temp1197;
_temp1197.curr= _temp1196; _temp1197.base= _temp1196; _temp1197.last_plus_one=
_temp1196 + 2; _temp1197;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1067));
goto _LL934; _LL968: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1198=( char*)"&"; struct _tagged_string _temp1199; _temp1199.curr=
_temp1198; _temp1199.base= _temp1198; _temp1199.last_plus_one= _temp1198 + 2;
_temp1199;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1071)); goto _LL934;
_LL970: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1200=( char*)"sizeof("; struct _tagged_string _temp1201; _temp1201.curr=
_temp1200; _temp1201.base= _temp1200; _temp1201.last_plus_one= _temp1200 + 8;
_temp1201;})), Cyc_Absynpp_typ2doc( _temp1073), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1202=( char*)")"; struct _tagged_string _temp1203;
_temp1203.curr= _temp1202; _temp1203.base= _temp1202; _temp1203.last_plus_one=
_temp1202 + 2; _temp1203;}))); goto _LL934; _LL972: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1204=( char*)"sizeof("; struct
_tagged_string _temp1205; _temp1205.curr= _temp1204; _temp1205.base= _temp1204;
_temp1205.last_plus_one= _temp1204 + 8; _temp1205;})), Cyc_Absynpp_exp2doc(
_temp1075), Cyc_PP_text(( struct _tagged_string)({ char* _temp1206=( char*)")";
struct _tagged_string _temp1207; _temp1207.curr= _temp1206; _temp1207.base=
_temp1206; _temp1207.last_plus_one= _temp1206 + 2; _temp1207;}))); goto _LL934;
_LL974: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1208=( char*)"*"; struct _tagged_string _temp1209; _temp1209.curr=
_temp1208; _temp1209.base= _temp1208; _temp1209.last_plus_one= _temp1208 + 2;
_temp1209;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1077)); goto _LL934;
_LL976: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1081), Cyc_PP_text((
struct _tagged_string)({ char* _temp1210=( char*)"."; struct _tagged_string
_temp1211; _temp1211.curr= _temp1210; _temp1211.base= _temp1210; _temp1211.last_plus_one=
_temp1210 + 2; _temp1211;})), Cyc_Absynpp_textptr( _temp1079)); goto _LL934;
_LL978: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1085), Cyc_PP_text((
struct _tagged_string)({ char* _temp1212=( char*)"->"; struct _tagged_string
_temp1213; _temp1213.curr= _temp1212; _temp1213.base= _temp1212; _temp1213.last_plus_one=
_temp1212 + 3; _temp1213;})), Cyc_Absynpp_textptr( _temp1083)); goto _LL934;
_LL980: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1089), Cyc_PP_text((
struct _tagged_string)({ char* _temp1214=( char*)"["; struct _tagged_string
_temp1215; _temp1215.curr= _temp1214; _temp1215.base= _temp1214; _temp1215.last_plus_one=
_temp1214 + 2; _temp1215;})), Cyc_Absynpp_exp2doc( _temp1087), Cyc_PP_text((
struct _tagged_string)({ char* _temp1216=( char*)"]"; struct _tagged_string
_temp1217; _temp1217.curr= _temp1216; _temp1217.base= _temp1216; _temp1217.last_plus_one=
_temp1216 + 2; _temp1217;}))); goto _LL934; _LL982: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1218=( char*)"$("; struct _tagged_string
_temp1219; _temp1219.curr= _temp1218; _temp1219.base= _temp1218; _temp1219.last_plus_one=
_temp1218 + 3; _temp1219;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1091), Cyc_PP_text((
struct _tagged_string)({ char* _temp1220=( char*)")"; struct _tagged_string
_temp1221; _temp1221.curr= _temp1220; _temp1221.base= _temp1220; _temp1221.last_plus_one=
_temp1220 + 2; _temp1221;}))); goto _LL934; _LL984: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1222=( char*)"("; struct _tagged_string
_temp1223; _temp1223.curr= _temp1222; _temp1223.base= _temp1222; _temp1223.last_plus_one=
_temp1222 + 2; _temp1223;})), Cyc_Absynpp_typ2doc((* _temp1095).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1224=( char*)")"; struct _tagged_string
_temp1225; _temp1225.curr= _temp1224; _temp1225.base= _temp1224; _temp1225.last_plus_one=
_temp1224 + 2; _temp1225;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1226=( char*)"{"; struct _tagged_string _temp1227; _temp1227.curr=
_temp1226; _temp1227.base= _temp1226; _temp1227.last_plus_one= _temp1226 + 2;
_temp1227;}),( struct _tagged_string)({ char* _temp1228=( char*)"}"; struct
_tagged_string _temp1229; _temp1229.curr= _temp1228; _temp1229.base= _temp1228;
_temp1229.last_plus_one= _temp1228 + 2; _temp1229;}),( struct _tagged_string)({
char* _temp1230=( char*)","; struct _tagged_string _temp1231; _temp1231.curr=
_temp1230; _temp1231.base= _temp1230; _temp1231.last_plus_one= _temp1230 + 2;
_temp1231;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1093))); goto _LL934; _LL986: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1232=( char*)"new {"; struct _tagged_string _temp1233; _temp1233.curr=
_temp1232; _temp1233.base= _temp1232; _temp1233.last_plus_one= _temp1232 + 6;
_temp1233;}),( struct _tagged_string)({ char* _temp1234=( char*)"}"; struct
_tagged_string _temp1235; _temp1235.curr= _temp1234; _temp1235.base= _temp1234;
_temp1235.last_plus_one= _temp1234 + 2; _temp1235;}),( struct _tagged_string)({
char* _temp1236=( char*)","; struct _tagged_string _temp1237; _temp1237.curr=
_temp1236; _temp1237.base= _temp1236; _temp1237.last_plus_one= _temp1236 + 2;
_temp1237;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1097)); goto _LL934; _LL988: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1238=( char*)"{"; struct _tagged_string _temp1239; _temp1239.curr=
_temp1238; _temp1239.base= _temp1238; _temp1239.last_plus_one= _temp1238 + 2;
_temp1239;}),( struct _tagged_string)({ char* _temp1240=( char*)"}"; struct
_tagged_string _temp1241; _temp1241.curr= _temp1240; _temp1241.base= _temp1240;
_temp1241.last_plus_one= _temp1240 + 2; _temp1241;}),( struct _tagged_string)({
char* _temp1242=( char*)","; struct _tagged_string _temp1243; _temp1243.curr=
_temp1242; _temp1243.base= _temp1242; _temp1243.last_plus_one= _temp1242 + 2;
_temp1243;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1101)); goto _LL934; _LL990: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1244=( char*)"new {for "; struct _tagged_string
_temp1245; _temp1245.curr= _temp1244; _temp1245.base= _temp1244; _temp1245.last_plus_one=
_temp1244 + 10; _temp1245;})), Cyc_PP_text(*(* _temp1109->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1246=( char*)" < "; struct _tagged_string
_temp1247; _temp1247.curr= _temp1246; _temp1247.base= _temp1246; _temp1247.last_plus_one=
_temp1246 + 4; _temp1247;})), Cyc_Absynpp_exp2doc( _temp1107), Cyc_PP_text((
struct _tagged_string)({ char* _temp1248=( char*)" : "; struct _tagged_string
_temp1249; _temp1249.curr= _temp1248; _temp1249.base= _temp1248; _temp1249.last_plus_one=
_temp1248 + 4; _temp1249;})), Cyc_Absynpp_exp2doc( _temp1105), Cyc_PP_text((
struct _tagged_string)({ char* _temp1250=( char*)"}"; struct _tagged_string
_temp1251; _temp1251.curr= _temp1250; _temp1251.base= _temp1250; _temp1251.last_plus_one=
_temp1250 + 2; _temp1251;}))); goto _LL934; _LL992: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1117), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1252=( char*)"{";
struct _tagged_string _temp1253; _temp1253.curr= _temp1252; _temp1253.base=
_temp1252; _temp1253.last_plus_one= _temp1252 + 2; _temp1253;}),( struct
_tagged_string)({ char* _temp1254=( char*)"}"; struct _tagged_string _temp1255;
_temp1255.curr= _temp1254; _temp1255.base= _temp1254; _temp1255.last_plus_one=
_temp1254 + 2; _temp1255;}),( struct _tagged_string)({ char* _temp1256=( char*)",";
struct _tagged_string _temp1257; _temp1257.curr= _temp1256; _temp1257.base=
_temp1256; _temp1257.last_plus_one= _temp1256 + 2; _temp1257;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1113))); goto _LL934; _LL994: _temp1133= _temp1123;
_temp1129= _temp1119; goto _LL996; _LL996: if( _temp1133 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1129->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1129->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1258=( char*)"(";
struct _tagged_string _temp1259; _temp1259.curr= _temp1258; _temp1259.base=
_temp1258; _temp1259.last_plus_one= _temp1258 + 2; _temp1259;}),( struct
_tagged_string)({ char* _temp1260=( char*)")"; struct _tagged_string _temp1261;
_temp1261.curr= _temp1260; _temp1261.base= _temp1260; _temp1261.last_plus_one=
_temp1260 + 2; _temp1261;}),( struct _tagged_string)({ char* _temp1262=( char*)",";
struct _tagged_string _temp1263; _temp1263.curr= _temp1262; _temp1263.base=
_temp1262; _temp1263.last_plus_one= _temp1262 + 2; _temp1263;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1133)));} goto _LL934; _LL998: { struct Cyc_PP_Doc*
inner_d;{ void* _temp1264= _temp1137; void* _temp1274; struct Cyc_Absyn_Enumfield*
_temp1276; struct Cyc_Absyn_Enumdecl* _temp1278; struct Cyc_Absyn_Enumfield*
_temp1280; struct Cyc_Absyn_Xenumdecl* _temp1282; struct _tuple0* _temp1284;
_LL1266: if((( struct _enum_struct*) _temp1264)->tag == Cyc_Absyn_Typ_m_tag){
_LL1275: _temp1274=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp1264)->f1;
goto _LL1267;} else{ goto _LL1268;} _LL1268: if((( struct _enum_struct*)
_temp1264)->tag == Cyc_Absyn_Enum_m_tag){ _LL1279: _temp1278=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_m_struct*) _temp1264)->f1; goto _LL1277; _LL1277:
_temp1276=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_m_struct*)
_temp1264)->f2; goto _LL1269;} else{ goto _LL1270;} _LL1270: if((( struct
_enum_struct*) _temp1264)->tag == Cyc_Absyn_Xenum_m_tag){ _LL1283: _temp1282=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_m_struct*) _temp1264)->f1;
goto _LL1281; _LL1281: _temp1280=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_m_struct*)
_temp1264)->f2; goto _LL1271;} else{ goto _LL1272;} _LL1272: if((( struct
_enum_struct*) _temp1264)->tag == Cyc_Absyn_Unresolved_m_tag){ _LL1285:
_temp1284=( struct _tuple0*)(( struct Cyc_Absyn_Unresolved_m_struct*) _temp1264)->f1;
goto _LL1273;} else{ goto _LL1265;} _LL1267: inner_d= Cyc_Absynpp_typ2doc(
_temp1274); goto _LL1265; _LL1269: _temp1280= _temp1276; goto _LL1271; _LL1271:
_temp1284= _temp1280->name; goto _LL1273; _LL1273: inner_d= Cyc_Absynpp_qvar2doc(
_temp1284); goto _LL1265; _LL1265:;} s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1286=( char*)"malloc(sizeof("; struct
_tagged_string _temp1287; _temp1287.curr= _temp1286; _temp1287.base= _temp1286;
_temp1287.last_plus_one= _temp1286 + 15; _temp1287;})), inner_d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1288=( char*)"))"; struct _tagged_string
_temp1289; _temp1289.curr= _temp1288; _temp1289.base= _temp1288; _temp1289.last_plus_one=
_temp1288 + 3; _temp1289;}))); goto _LL934;} _LL1000: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1290=( char*)"{"; struct _tagged_string
_temp1291; _temp1291.curr= _temp1290; _temp1291.base= _temp1290; _temp1291.last_plus_one=
_temp1290 + 2; _temp1291;}),( struct _tagged_string)({ char* _temp1292=( char*)"}";
struct _tagged_string _temp1293; _temp1293.curr= _temp1292; _temp1293.base=
_temp1292; _temp1293.last_plus_one= _temp1292 + 2; _temp1293;}),( struct
_tagged_string)({ char* _temp1294=( char*)","; struct _tagged_string _temp1295;
_temp1295.curr= _temp1294; _temp1295.base= _temp1294; _temp1295.last_plus_one=
_temp1294 + 2; _temp1295;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1139)); goto _LL934; _LL1002: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1296=( char*)"({ "; struct _tagged_string _temp1297;
_temp1297.curr= _temp1296; _temp1297.base= _temp1296; _temp1297.last_plus_one=
_temp1296 + 4; _temp1297;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1143)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1298=( char*)" })"; struct
_tagged_string _temp1299; _temp1299.curr= _temp1298; _temp1299.base= _temp1298;
_temp1299.last_plus_one= _temp1298 + 4; _temp1299;}))); goto _LL934; _LL1004: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1300=( char*)"codegen(";
struct _tagged_string _temp1301; _temp1301.curr= _temp1300; _temp1301.base=
_temp1300; _temp1301.last_plus_one= _temp1300 + 9; _temp1301;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1302=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1302->r=( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1303=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1303->tag= Cyc_Absyn_Fn_d_tag; _temp1303->f1= _temp1145;( void*) _temp1303;});
_temp1302->loc= e->loc; _temp1302;}))), Cyc_PP_text(( struct _tagged_string)({
char* _temp1304=( char*)")"; struct _tagged_string _temp1305; _temp1305.curr=
_temp1304; _temp1305.base= _temp1304; _temp1305.last_plus_one= _temp1304 + 2;
_temp1305;}))); goto _LL934; _LL1006: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1306=( char*)"fill("; struct _tagged_string
_temp1307; _temp1307.curr= _temp1306; _temp1307.base= _temp1306; _temp1307.last_plus_one=
_temp1306 + 6; _temp1307;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1147)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1308=( char*)")"; struct
_tagged_string _temp1309; _temp1309.curr= _temp1308; _temp1309.base= _temp1308;
_temp1309.last_plus_one= _temp1308 + 2; _temp1309;}))); goto _LL934; _LL934:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1310=( char*)"("; struct _tagged_string _temp1311; _temp1311.curr=
_temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one= _temp1310 + 2;
_temp1311;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1312=( char*)")";
struct _tagged_string _temp1313; _temp1313.curr= _temp1312; _temp1313.base=
_temp1312; _temp1313.last_plus_one= _temp1312 + 2; _temp1313;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1314= d;
struct Cyc_Absyn_Exp* _temp1320; struct _tagged_string* _temp1322; _LL1316: if(((
struct _enum_struct*) _temp1314)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1321:
_temp1320=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1314)->f1; goto _LL1317;} else{ goto _LL1318;} _LL1318: if((( struct
_enum_struct*) _temp1314)->tag == Cyc_Absyn_FieldName_tag){ _LL1323: _temp1322=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1314)->f1;
goto _LL1319;} else{ goto _LL1315;} _LL1317: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1324=( char*)".["; struct _tagged_string
_temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one=
_temp1324 + 3; _temp1325;})), Cyc_Absynpp_exp2doc( _temp1320), Cyc_PP_text((
struct _tagged_string)({ char* _temp1326=( char*)"]"; struct _tagged_string
_temp1327; _temp1327.curr= _temp1326; _temp1327.base= _temp1326; _temp1327.last_plus_one=
_temp1326 + 2; _temp1327;}))); _LL1319: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1328=( char*)"."; struct _tagged_string
_temp1329; _temp1329.curr= _temp1328; _temp1329.base= _temp1328; _temp1329.last_plus_one=
_temp1328 + 2; _temp1329;})), Cyc_Absynpp_textptr( _temp1322)); _LL1315:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1330=( char*)""; struct _tagged_string
_temp1331; _temp1331.curr= _temp1330; _temp1331.base= _temp1330; _temp1331.last_plus_one=
_temp1330 + 1; _temp1331;}),( struct _tagged_string)({ char* _temp1332=( char*)"=";
struct _tagged_string _temp1333; _temp1333.curr= _temp1332; _temp1333.base=
_temp1332; _temp1333.last_plus_one= _temp1332 + 2; _temp1333;}),( struct
_tagged_string)({ char* _temp1334=( char*)"="; struct _tagged_string _temp1335;
_temp1335.curr= _temp1334; _temp1335.base= _temp1334; _temp1335.last_plus_one=
_temp1334 + 2; _temp1335;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1336=( char*)""; struct _tagged_string _temp1337;
_temp1337.curr= _temp1336; _temp1337.base= _temp1336; _temp1337.last_plus_one=
_temp1336 + 1; _temp1337;}),( struct _tagged_string)({ char* _temp1338=( char*)"";
struct _tagged_string _temp1339; _temp1339.curr= _temp1338; _temp1339.base=
_temp1338; _temp1339.last_plus_one= _temp1338 + 1; _temp1339;}),( struct
_tagged_string)({ char* _temp1340=( char*)","; struct _tagged_string _temp1341;
_temp1341.curr= _temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one=
_temp1340 + 2; _temp1341;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1342= c; char _temp1362; void* _temp1364; short _temp1366;
void* _temp1368; int _temp1370; void* _temp1372; int _temp1374; void* _temp1376;
long long _temp1378; void* _temp1380; struct _tagged_string _temp1382; struct
_tagged_string _temp1384; int _temp1386; struct _tagged_string _temp1388; int
_temp1390; _LL1344: if(( unsigned int) _temp1342 > 1u?(( struct _enum_struct*)
_temp1342)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1365: _temp1364=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp1342)->f1; goto _LL1363; _LL1363:
_temp1362=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1342)->f2; goto
_LL1345;} else{ goto _LL1346;} _LL1346: if(( unsigned int) _temp1342 > 1u?((
struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1369:
_temp1368=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1342)->f1; goto
_LL1367; _LL1367: _temp1366=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1342)->f2; goto _LL1347;} else{ goto _LL1348;} _LL1348: if(( unsigned int)
_temp1342 > 1u?(( struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1373: _temp1372=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1342)->f1;
if( _temp1372 == Cyc_Absyn_Signed){ goto _LL1371;} else{ goto _LL1350;} _LL1371:
_temp1370=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1342)->f2; goto _LL1349;}
else{ goto _LL1350;} _LL1350: if(( unsigned int) _temp1342 > 1u?(( struct
_enum_struct*) _temp1342)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1377: _temp1376=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1342)->f1; if( _temp1376 == Cyc_Absyn_Unsigned){
goto _LL1375;} else{ goto _LL1352;} _LL1375: _temp1374=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1342)->f2; goto _LL1351;} else{ goto _LL1352;} _LL1352: if(( unsigned int)
_temp1342 > 1u?(( struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL1381: _temp1380=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1342)->f1;
goto _LL1379; _LL1379: _temp1378=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1342)->f2; goto _LL1353;} else{ goto _LL1354;} _LL1354: if(( unsigned int)
_temp1342 > 1u?(( struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1383: _temp1382=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1342)->f1; goto _LL1355;} else{ goto _LL1356;} _LL1356: if( _temp1342 ==
Cyc_Absyn_Null_c){ goto _LL1357;} else{ goto _LL1358;} _LL1358: if((
unsigned int) _temp1342 > 1u?(( struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1387: _temp1386=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1342)->f1;
if( _temp1386 == 1){ goto _LL1385;} else{ goto _LL1360;} _LL1385: _temp1384=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1342)->f2; goto
_LL1359;} else{ goto _LL1360;} _LL1360: if(( unsigned int) _temp1342 > 1u?((
struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_String_c_tag: 0){ _LL1391:
_temp1390=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1342)->f1; goto
_LL1389; _LL1389: _temp1388=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1342)->f2; goto _LL1361;} else{ goto _LL1343;} _LL1345: return Cyc_PP_text(({
struct _tagged_string _temp1392= Cyc_Absynpp_char_escape( _temp1362); xprintf("'%.*s'",
_temp1392.last_plus_one - _temp1392.curr, _temp1392.curr);})); _LL1347: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1366)); _LL1349: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1370)); _LL1351: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1374)); _LL1353: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1393=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1394; _temp1394.curr= _temp1393; _temp1394.base= _temp1393; _temp1394.last_plus_one=
_temp1393 + 27; _temp1394;})); _LL1355: return Cyc_PP_text( _temp1382); _LL1357:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1395=( char*)"null";
struct _tagged_string _temp1396; _temp1396.curr= _temp1395; _temp1396.base=
_temp1395; _temp1396.last_plus_one= _temp1395 + 5; _temp1396;})); _LL1359:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1397=(
char*)"new \""; struct _tagged_string _temp1398; _temp1398.curr= _temp1397;
_temp1398.base= _temp1397; _temp1398.last_plus_one= _temp1397 + 6; _temp1398;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1384)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1399=( char*)"\""; struct _tagged_string _temp1400;
_temp1400.curr= _temp1399; _temp1400.base= _temp1399; _temp1400.last_plus_one=
_temp1399 + 2; _temp1400;}))); _LL1361: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1401=( char*)"\""; struct _tagged_string
_temp1402; _temp1402.curr= _temp1401; _temp1402.base= _temp1401; _temp1402.last_plus_one=
_temp1401 + 2; _temp1402;})), Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1388)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1403=( char*)"\""; struct
_tagged_string _temp1404; _temp1404.curr= _temp1403; _temp1404.base= _temp1403;
_temp1404.last_plus_one= _temp1403 + 2; _temp1404;}))); _LL1343:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == Cyc_Absyn_Size){ if( es == 0?
1: es->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1405=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1405->tag= Cyc_Core_Failure_tag; _temp1405->f1=({ struct _tagged_string
_temp1406= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args",
_temp1406.last_plus_one - _temp1406.curr, _temp1406.curr);});( struct
_xenum_struct*) _temp1405;}));}{ struct Cyc_PP_Doc* d= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return Cyc_Absynpp_cat2( d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1407=( char*)".size"; struct _tagged_string
_temp1408; _temp1408.curr= _temp1407; _temp1408.base= _temp1407; _temp1408.last_plus_one=
_temp1407 + 6; _temp1408;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){
struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1409=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1409->tag= Cyc_Core_Failure_tag; _temp1409->f1=({ struct _tagged_string
_temp1410= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1410.last_plus_one - _temp1410.curr, _temp1410.curr);});( struct
_xenum_struct*) _temp1409;}));} else{ if( ds->tl == 0){ return Cyc_Absynpp_cat3(
ps, Cyc_PP_text(( struct _tagged_string)({ char* _temp1411=( char*)" "; struct
_tagged_string _temp1412; _temp1412.curr= _temp1411; _temp1412.base= _temp1411;
_temp1412.last_plus_one= _temp1411 + 2; _temp1412;})),( struct Cyc_PP_Doc*) ds->hd);}
else{ if(( ds->tl)->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1413=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1413->tag= Cyc_Core_Failure_tag; _temp1413->f1=({ struct _tagged_string
_temp1414= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1414.last_plus_one - _temp1414.curr, _temp1414.curr);});( struct
_xenum_struct*) _temp1413;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)
ds->hd, Cyc_PP_text(( struct _tagged_string)({ char* _temp1415=( char*)" ";
struct _tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base=
_temp1415; _temp1416.last_plus_one= _temp1415 + 2; _temp1416;})), ps, Cyc_PP_text((
struct _tagged_string)({ char* _temp1417=( char*)" "; struct _tagged_string
_temp1418; _temp1418.curr= _temp1417; _temp1418.base= _temp1417; _temp1418.last_plus_one=
_temp1417 + 2; _temp1418;})),( struct Cyc_PP_Doc*)( ds->tl)->hd);}}}} else{
return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1419=( char*)"("; struct _tagged_string _temp1420; _temp1420.curr=
_temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one= _temp1419 + 2;
_temp1420;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1421=( char*)")"; struct _tagged_string _temp1422;
_temp1422.curr= _temp1421; _temp1422.base= _temp1421; _temp1422.last_plus_one=
_temp1421 + 2; _temp1422;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1423= p; _LL1425: if( _temp1423 == Cyc_Absyn_Plus){ goto
_LL1426;} else{ goto _LL1427;} _LL1427: if( _temp1423 == Cyc_Absyn_Times){ goto
_LL1428;} else{ goto _LL1429;} _LL1429: if( _temp1423 == Cyc_Absyn_Minus){ goto
_LL1430;} else{ goto _LL1431;} _LL1431: if( _temp1423 == Cyc_Absyn_Div){ goto
_LL1432;} else{ goto _LL1433;} _LL1433: if( _temp1423 == Cyc_Absyn_Mod){ goto
_LL1434;} else{ goto _LL1435;} _LL1435: if( _temp1423 == Cyc_Absyn_Eq){ goto
_LL1436;} else{ goto _LL1437;} _LL1437: if( _temp1423 == Cyc_Absyn_Neq){ goto
_LL1438;} else{ goto _LL1439;} _LL1439: if( _temp1423 == Cyc_Absyn_Gt){ goto
_LL1440;} else{ goto _LL1441;} _LL1441: if( _temp1423 == Cyc_Absyn_Lt){ goto
_LL1442;} else{ goto _LL1443;} _LL1443: if( _temp1423 == Cyc_Absyn_Gte){ goto
_LL1444;} else{ goto _LL1445;} _LL1445: if( _temp1423 == Cyc_Absyn_Lte){ goto
_LL1446;} else{ goto _LL1447;} _LL1447: if( _temp1423 == Cyc_Absyn_Not){ goto
_LL1448;} else{ goto _LL1449;} _LL1449: if( _temp1423 == Cyc_Absyn_Bitnot){ goto
_LL1450;} else{ goto _LL1451;} _LL1451: if( _temp1423 == Cyc_Absyn_Bitand){ goto
_LL1452;} else{ goto _LL1453;} _LL1453: if( _temp1423 == Cyc_Absyn_Bitor){ goto
_LL1454;} else{ goto _LL1455;} _LL1455: if( _temp1423 == Cyc_Absyn_Bitxor){ goto
_LL1456;} else{ goto _LL1457;} _LL1457: if( _temp1423 == Cyc_Absyn_Bitlshift){
goto _LL1458;} else{ goto _LL1459;} _LL1459: if( _temp1423 == Cyc_Absyn_Bitlrshift){
goto _LL1460;} else{ goto _LL1461;} _LL1461: if( _temp1423 == Cyc_Absyn_Bitarshift){
goto _LL1462;} else{ goto _LL1463;} _LL1463: if( _temp1423 == Cyc_Absyn_Size){
goto _LL1464;} else{ goto _LL1465;} _LL1465: if( _temp1423 == Cyc_Absyn_Printf){
goto _LL1466;} else{ goto _LL1467;} _LL1467: if( _temp1423 == Cyc_Absyn_Fprintf){
goto _LL1468;} else{ goto _LL1469;} _LL1469: if( _temp1423 == Cyc_Absyn_Xprintf){
goto _LL1470;} else{ goto _LL1471;} _LL1471: if( _temp1423 == Cyc_Absyn_Scanf){
goto _LL1472;} else{ goto _LL1473;} _LL1473: if( _temp1423 == Cyc_Absyn_Fscanf){
goto _LL1474;} else{ goto _LL1475;} _LL1475: if( _temp1423 == Cyc_Absyn_Sscanf){
goto _LL1476;} else{ goto _LL1424;} _LL1426: return( struct _tagged_string)({
char* _temp1477=( char*)"+"; struct _tagged_string _temp1478; _temp1478.curr=
_temp1477; _temp1478.base= _temp1477; _temp1478.last_plus_one= _temp1477 + 2;
_temp1478;}); _LL1428: return( struct _tagged_string)({ char* _temp1479=( char*)"*";
struct _tagged_string _temp1480; _temp1480.curr= _temp1479; _temp1480.base=
_temp1479; _temp1480.last_plus_one= _temp1479 + 2; _temp1480;}); _LL1430: return(
struct _tagged_string)({ char* _temp1481=( char*)"-"; struct _tagged_string
_temp1482; _temp1482.curr= _temp1481; _temp1482.base= _temp1481; _temp1482.last_plus_one=
_temp1481 + 2; _temp1482;}); _LL1432: return( struct _tagged_string)({ char*
_temp1483=( char*)"/"; struct _tagged_string _temp1484; _temp1484.curr=
_temp1483; _temp1484.base= _temp1483; _temp1484.last_plus_one= _temp1483 + 2;
_temp1484;}); _LL1434: return( struct _tagged_string)({ char* _temp1485=( char*)"%";
struct _tagged_string _temp1486; _temp1486.curr= _temp1485; _temp1486.base=
_temp1485; _temp1486.last_plus_one= _temp1485 + 2; _temp1486;}); _LL1436: return(
struct _tagged_string)({ char* _temp1487=( char*)"=="; struct _tagged_string
_temp1488; _temp1488.curr= _temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one=
_temp1487 + 3; _temp1488;}); _LL1438: return( struct _tagged_string)({ char*
_temp1489=( char*)"!="; struct _tagged_string _temp1490; _temp1490.curr=
_temp1489; _temp1490.base= _temp1489; _temp1490.last_plus_one= _temp1489 + 3;
_temp1490;}); _LL1440: return( struct _tagged_string)({ char* _temp1491=( char*)">";
struct _tagged_string _temp1492; _temp1492.curr= _temp1491; _temp1492.base=
_temp1491; _temp1492.last_plus_one= _temp1491 + 2; _temp1492;}); _LL1442: return(
struct _tagged_string)({ char* _temp1493=( char*)"<"; struct _tagged_string
_temp1494; _temp1494.curr= _temp1493; _temp1494.base= _temp1493; _temp1494.last_plus_one=
_temp1493 + 2; _temp1494;}); _LL1444: return( struct _tagged_string)({ char*
_temp1495=( char*)">="; struct _tagged_string _temp1496; _temp1496.curr=
_temp1495; _temp1496.base= _temp1495; _temp1496.last_plus_one= _temp1495 + 3;
_temp1496;}); _LL1446: return( struct _tagged_string)({ char* _temp1497=( char*)"<=";
struct _tagged_string _temp1498; _temp1498.curr= _temp1497; _temp1498.base=
_temp1497; _temp1498.last_plus_one= _temp1497 + 3; _temp1498;}); _LL1448: return(
struct _tagged_string)({ char* _temp1499=( char*)"!"; struct _tagged_string
_temp1500; _temp1500.curr= _temp1499; _temp1500.base= _temp1499; _temp1500.last_plus_one=
_temp1499 + 2; _temp1500;}); _LL1450: return( struct _tagged_string)({ char*
_temp1501=( char*)"~"; struct _tagged_string _temp1502; _temp1502.curr=
_temp1501; _temp1502.base= _temp1501; _temp1502.last_plus_one= _temp1501 + 2;
_temp1502;}); _LL1452: return( struct _tagged_string)({ char* _temp1503=( char*)"&";
struct _tagged_string _temp1504; _temp1504.curr= _temp1503; _temp1504.base=
_temp1503; _temp1504.last_plus_one= _temp1503 + 2; _temp1504;}); _LL1454: return(
struct _tagged_string)({ char* _temp1505=( char*)"|"; struct _tagged_string
_temp1506; _temp1506.curr= _temp1505; _temp1506.base= _temp1505; _temp1506.last_plus_one=
_temp1505 + 2; _temp1506;}); _LL1456: return( struct _tagged_string)({ char*
_temp1507=( char*)"^"; struct _tagged_string _temp1508; _temp1508.curr=
_temp1507; _temp1508.base= _temp1507; _temp1508.last_plus_one= _temp1507 + 2;
_temp1508;}); _LL1458: return( struct _tagged_string)({ char* _temp1509=( char*)"<<";
struct _tagged_string _temp1510; _temp1510.curr= _temp1509; _temp1510.base=
_temp1509; _temp1510.last_plus_one= _temp1509 + 3; _temp1510;}); _LL1460: return(
struct _tagged_string)({ char* _temp1511=( char*)">>"; struct _tagged_string
_temp1512; _temp1512.curr= _temp1511; _temp1512.base= _temp1511; _temp1512.last_plus_one=
_temp1511 + 3; _temp1512;}); _LL1462: return( struct _tagged_string)({ char*
_temp1513=( char*)">>>"; struct _tagged_string _temp1514; _temp1514.curr=
_temp1513; _temp1514.base= _temp1513; _temp1514.last_plus_one= _temp1513 + 4;
_temp1514;}); _LL1464: return( struct _tagged_string)({ char* _temp1515=( char*)"size";
struct _tagged_string _temp1516; _temp1516.curr= _temp1515; _temp1516.base=
_temp1515; _temp1516.last_plus_one= _temp1515 + 5; _temp1516;}); _LL1466: return(
struct _tagged_string)({ char* _temp1517=( char*)"printf"; struct _tagged_string
_temp1518; _temp1518.curr= _temp1517; _temp1518.base= _temp1517; _temp1518.last_plus_one=
_temp1517 + 7; _temp1518;}); _LL1468: return( struct _tagged_string)({ char*
_temp1519=( char*)"fprintf"; struct _tagged_string _temp1520; _temp1520.curr=
_temp1519; _temp1520.base= _temp1519; _temp1520.last_plus_one= _temp1519 + 8;
_temp1520;}); _LL1470: return( struct _tagged_string)({ char* _temp1521=( char*)"xprintf";
struct _tagged_string _temp1522; _temp1522.curr= _temp1521; _temp1522.base=
_temp1521; _temp1522.last_plus_one= _temp1521 + 8; _temp1522;}); _LL1472: return(
struct _tagged_string)({ char* _temp1523=( char*)"scanf"; struct _tagged_string
_temp1524; _temp1524.curr= _temp1523; _temp1524.base= _temp1523; _temp1524.last_plus_one=
_temp1523 + 6; _temp1524;}); _LL1474: return( struct _tagged_string)({ char*
_temp1525=( char*)"fscanf"; struct _tagged_string _temp1526; _temp1526.curr=
_temp1525; _temp1526.base= _temp1525; _temp1526.last_plus_one= _temp1525 + 7;
_temp1526;}); _LL1476: return( struct _tagged_string)({ char* _temp1527=( char*)"sscanf";
struct _tagged_string _temp1528; _temp1528.curr= _temp1527; _temp1528.base=
_temp1527; _temp1528.last_plus_one= _temp1527 + 7; _temp1528;}); _LL1424:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple10* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)((* t).f1)->v), Cyc_PP_text(( struct _tagged_string)({
char* _temp1529=( char*)" = "; struct _tagged_string _temp1530; _temp1530.curr=
_temp1529; _temp1530.base= _temp1529; _temp1530.last_plus_one= _temp1529 + 4;
_temp1530;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_cat2(
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1531=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1537; struct Cyc_Absyn_Decl* _temp1539;
_LL1533: if(( unsigned int) _temp1531 > 1u?(( struct _enum_struct*) _temp1531)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1540: _temp1539=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1531)->f1; goto _LL1538; _LL1538:
_temp1537=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1531)->f2;
goto _LL1534;} else{ goto _LL1535;} _LL1535: goto _LL1536; _LL1534: return 1;
_LL1536: return 0; _LL1532:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1541=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1581; struct Cyc_Absyn_Stmt* _temp1583; struct Cyc_Absyn_Stmt*
_temp1585; struct Cyc_Absyn_Exp* _temp1587; struct Cyc_Absyn_Stmt* _temp1589;
struct Cyc_Absyn_Stmt* _temp1591; struct Cyc_Absyn_Exp* _temp1593; struct Cyc_Absyn_Stmt*
_temp1595; struct _tuple2 _temp1597; struct Cyc_Absyn_Stmt* _temp1599; struct
Cyc_Absyn_Exp* _temp1601; struct Cyc_Absyn_Stmt* _temp1603; struct Cyc_Absyn_Stmt*
_temp1605; struct Cyc_Absyn_Stmt* _temp1607; struct _tagged_string* _temp1609;
struct Cyc_Absyn_Stmt* _temp1611; struct _tuple2 _temp1613; struct Cyc_Absyn_Stmt*
_temp1615; struct Cyc_Absyn_Exp* _temp1617; struct _tuple2 _temp1619; struct Cyc_Absyn_Stmt*
_temp1621; struct Cyc_Absyn_Exp* _temp1623; struct Cyc_Absyn_Exp* _temp1625;
struct Cyc_List_List* _temp1627; struct Cyc_Absyn_Exp* _temp1629; struct Cyc_Absyn_Switch_clause**
_temp1631; struct Cyc_List_List* _temp1633; struct Cyc_Absyn_Switch_clause**
_temp1635; struct Cyc_List_List* _temp1637; struct Cyc_Absyn_Stmt* _temp1639;
struct Cyc_Absyn_Decl* _temp1641; struct Cyc_Absyn_Stmt* _temp1643; struct Cyc_Absyn_Stmt*
_temp1645; struct Cyc_Absyn_Stmt* _temp1647; struct _tagged_string* _temp1649;
struct _tuple2 _temp1651; struct Cyc_Absyn_Stmt* _temp1653; struct Cyc_Absyn_Exp*
_temp1655; struct Cyc_Absyn_Stmt* _temp1657; struct Cyc_List_List* _temp1659;
struct Cyc_Absyn_Stmt* _temp1661; _LL1543: if( _temp1541 == Cyc_Absyn_Skip_s){
goto _LL1544;} else{ goto _LL1545;} _LL1545: if(( unsigned int) _temp1541 > 1u?((
struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1582:
_temp1581=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1541)->f1;
goto _LL1546;} else{ goto _LL1547;} _LL1547: if(( unsigned int) _temp1541 > 1u?((
struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1586:
_temp1585=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1541)->f1;
goto _LL1584; _LL1584: _temp1583=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1541)->f2; goto _LL1548;} else{ goto _LL1549;} _LL1549: if(( unsigned int)
_temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1588: _temp1587=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1541)->f1; goto _LL1550;} else{ goto _LL1551;} _LL1551: if(( unsigned int)
_temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1594: _temp1593=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1541)->f1; goto _LL1592; _LL1592: _temp1591=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1541)->f2; goto _LL1590; _LL1590:
_temp1589=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1541)->f3; goto _LL1552;} else{ goto _LL1553;} _LL1553: if(( unsigned int)
_temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1598: _temp1597=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1541)->f1; _LL1602: _temp1601= _temp1597.f1; goto _LL1600; _LL1600:
_temp1599= _temp1597.f2; goto _LL1596; _LL1596: _temp1595=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1541)->f2; goto _LL1554;} else{ goto
_LL1555;} _LL1555: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1604: _temp1603=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1541)->f1; goto _LL1556;} else{ goto
_LL1557;} _LL1557: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1606: _temp1605=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1541)->f1; goto _LL1558;} else{ goto
_LL1559;} _LL1559: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1610: _temp1609=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1541)->f1; goto _LL1608;
_LL1608: _temp1607=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1541)->f2; goto _LL1560;} else{ goto _LL1561;} _LL1561: if(( unsigned int)
_temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1626: _temp1625=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1541)->f1; goto _LL1620; _LL1620: _temp1619=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1541)->f2; _LL1624: _temp1623= _temp1619.f1; goto _LL1622; _LL1622:
_temp1621= _temp1619.f2; goto _LL1614; _LL1614: _temp1613=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1541)->f3; _LL1618: _temp1617= _temp1613.f1;
goto _LL1616; _LL1616: _temp1615= _temp1613.f2; goto _LL1612; _LL1612: _temp1611=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1541)->f4; goto
_LL1562;} else{ goto _LL1563;} _LL1563: if(( unsigned int) _temp1541 > 1u?((
struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1630:
_temp1629=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1541)->f1; goto _LL1628; _LL1628: _temp1627=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1541)->f2; goto _LL1564;} else{ goto
_LL1565;} _LL1565: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1634: _temp1633=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1541)->f1; if( _temp1633 == 0){ goto
_LL1632;} else{ goto _LL1567;} _LL1632: _temp1631=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1541)->f2; goto _LL1566;} else{ goto
_LL1567;} _LL1567: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1638: _temp1637=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1541)->f1; goto _LL1636; _LL1636:
_temp1635=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1541)->f2; goto _LL1568;} else{ goto _LL1569;} _LL1569: if(( unsigned int)
_temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1642: _temp1641=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1541)->f1; goto _LL1640; _LL1640: _temp1639=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1541)->f2; goto _LL1570;} else{ goto
_LL1571;} _LL1571: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1644: _temp1643=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1541)->f1; goto _LL1572;} else{ goto
_LL1573;} _LL1573: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1646: _temp1645=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1541)->f1; goto _LL1574;} else{ goto
_LL1575;} _LL1575: if(( unsigned int) _temp1541 > 1u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1650: _temp1649=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1541)->f1; goto
_LL1648; _LL1648: _temp1647=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1541)->f2; goto _LL1576;} else{ goto _LL1577;} _LL1577: if(( unsigned int)
_temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1658: _temp1657=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1541)->f1; goto _LL1652; _LL1652: _temp1651=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1541)->f2; _LL1656: _temp1655= _temp1651.f1; goto _LL1654; _LL1654:
_temp1653= _temp1651.f2; goto _LL1578;} else{ goto _LL1579;} _LL1579: if((
unsigned int) _temp1541 > 1u?(( struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1662: _temp1661=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1541)->f1; goto _LL1660; _LL1660: _temp1659=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1541)->f2; goto _LL1580;} else{ goto
_LL1542;} _LL1544: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1663=(
char*)";"; struct _tagged_string _temp1664; _temp1664.curr= _temp1663; _temp1664.base=
_temp1663; _temp1664.last_plus_one= _temp1663 + 2; _temp1664;})); goto _LL1542;
_LL1546: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1581), Cyc_PP_text((
struct _tagged_string)({ char* _temp1665=( char*)";"; struct _tagged_string
_temp1666; _temp1666.curr= _temp1665; _temp1666.base= _temp1665; _temp1666.last_plus_one=
_temp1665 + 2; _temp1666;}))); goto _LL1542; _LL1548: if( Cyc_Absynpp_is_declaration(
_temp1585)){ s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1667=( char*)"{ "; struct _tagged_string _temp1668; _temp1668.curr=
_temp1667; _temp1668.base= _temp1667; _temp1668.last_plus_one= _temp1667 + 3;
_temp1668;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1585)), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1669=( char*)"}"; struct
_tagged_string _temp1670; _temp1670.curr= _temp1669; _temp1670.base= _temp1669;
_temp1670.last_plus_one= _temp1669 + 2; _temp1670;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1583)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1671=( char*)"{ "; struct _tagged_string _temp1672; _temp1672.curr=
_temp1671; _temp1672.base= _temp1671; _temp1672.last_plus_one= _temp1671 + 3;
_temp1672;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1583)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1673=( char*)"}"; struct _tagged_string
_temp1674; _temp1674.curr= _temp1673; _temp1674.base= _temp1673; _temp1674.last_plus_one=
_temp1673 + 2; _temp1674;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1583));} else{ if( Cyc_Absynpp_is_declaration( _temp1583)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1585), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1675=( char*)"{ "; struct _tagged_string _temp1676;
_temp1676.curr= _temp1675; _temp1676.base= _temp1675; _temp1676.last_plus_one=
_temp1675 + 3; _temp1676;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1583)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1677=( char*)"}"; struct
_tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base= _temp1677;
_temp1678.last_plus_one= _temp1677 + 2; _temp1678;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1679=( char*)""; struct _tagged_string
_temp1680; _temp1680.curr= _temp1679; _temp1680.base= _temp1679; _temp1680.last_plus_one=
_temp1679 + 1; _temp1680;}),({ struct Cyc_List_List* _temp1681=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1681->hd=( void*) _temp1585;
_temp1681->tl=({ struct Cyc_List_List* _temp1682=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1682->hd=( void*) _temp1583;
_temp1682->tl= 0; _temp1682;}); _temp1681;}));}} goto _LL1542; _LL1550: if(
_temp1587 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1683=(
char*)"return;"; struct _tagged_string _temp1684; _temp1684.curr= _temp1683;
_temp1684.base= _temp1683; _temp1684.last_plus_one= _temp1683 + 8; _temp1684;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1685=( char*)"return "; struct _tagged_string _temp1686; _temp1686.curr=
_temp1685; _temp1686.base= _temp1685; _temp1686.last_plus_one= _temp1685 + 8;
_temp1686;})), _temp1587 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1687= _temp1587; if( _temp1687 == 0){
_throw( Null_Exception);} _temp1687;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1688=( char*)";"; struct _tagged_string _temp1689; _temp1689.curr=
_temp1688; _temp1689.base= _temp1688; _temp1689.last_plus_one= _temp1688 + 2;
_temp1689;})));} goto _LL1542; _LL1552: { int print_else;{ void* _temp1690=(
void*) _temp1589->r; _LL1692: if( _temp1690 == Cyc_Absyn_Skip_s){ goto _LL1693;}
else{ goto _LL1694;} _LL1694: goto _LL1695; _LL1693: print_else= 0; goto _LL1691;
_LL1695: print_else= 1; goto _LL1691; _LL1691:;} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1696=( char*)"if ("; struct _tagged_string
_temp1697; _temp1697.curr= _temp1696; _temp1697.base= _temp1696; _temp1697.last_plus_one=
_temp1696 + 5; _temp1697;})), Cyc_Absynpp_exp2doc( _temp1593), Cyc_PP_text((
struct _tagged_string)({ char* _temp1698=( char*)") {"; struct _tagged_string
_temp1699; _temp1699.curr= _temp1698; _temp1699.base= _temp1698; _temp1699.last_plus_one=
_temp1698 + 4; _temp1699;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1591))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1700=( char*)"}"; struct _tagged_string _temp1701;
_temp1701.curr= _temp1700; _temp1701.base= _temp1700; _temp1701.last_plus_one=
_temp1700 + 2; _temp1701;})), print_else? Cyc_Absynpp_cat5( Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1702=( char*)"else {"; struct
_tagged_string _temp1703; _temp1703.curr= _temp1702; _temp1703.base= _temp1702;
_temp1703.last_plus_one= _temp1702 + 7; _temp1703;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1589))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1704=( char*)"}"; struct _tagged_string
_temp1705; _temp1705.curr= _temp1704; _temp1705.base= _temp1704; _temp1705.last_plus_one=
_temp1704 + 2; _temp1705;}))): Cyc_PP_nil_doc()); goto _LL1542;} _LL1554: s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1706=( char*)"while ("; struct
_tagged_string _temp1707; _temp1707.curr= _temp1706; _temp1707.base= _temp1706;
_temp1707.last_plus_one= _temp1706 + 8; _temp1707;})), Cyc_Absynpp_exp2doc(
_temp1601), Cyc_PP_text(( struct _tagged_string)({ char* _temp1708=( char*)") {";
struct _tagged_string _temp1709; _temp1709.curr= _temp1708; _temp1709.base=
_temp1708; _temp1709.last_plus_one= _temp1708 + 4; _temp1709;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1595))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1710=( char*)"}"; struct
_tagged_string _temp1711; _temp1711.curr= _temp1710; _temp1711.base= _temp1710;
_temp1711.last_plus_one= _temp1710 + 2; _temp1711;}))); goto _LL1542; _LL1556: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1712=( char*)"break;"; struct
_tagged_string _temp1713; _temp1713.curr= _temp1712; _temp1713.base= _temp1712;
_temp1713.last_plus_one= _temp1712 + 7; _temp1713;})); goto _LL1542; _LL1558: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1714=( char*)"continue;";
struct _tagged_string _temp1715; _temp1715.curr= _temp1714; _temp1715.base=
_temp1714; _temp1715.last_plus_one= _temp1714 + 10; _temp1715;})); goto _LL1542;
_LL1560: s= Cyc_PP_text(({ struct _tagged_string _temp1716=* _temp1609; xprintf("goto %.*s;",
_temp1716.last_plus_one - _temp1716.curr, _temp1716.curr);})); goto _LL1542;
_LL1562: s= Cyc_Absynpp_cat10( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1717=( char*)"for("; struct _tagged_string _temp1718; _temp1718.curr=
_temp1717; _temp1718.base= _temp1717; _temp1718.last_plus_one= _temp1717 + 5;
_temp1718;})), Cyc_Absynpp_exp2doc( _temp1625), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1719=( char*)"; "; struct _tagged_string _temp1720;
_temp1720.curr= _temp1719; _temp1720.base= _temp1719; _temp1720.last_plus_one=
_temp1719 + 3; _temp1720;})), Cyc_Absynpp_exp2doc( _temp1623), Cyc_PP_text((
struct _tagged_string)({ char* _temp1721=( char*)"; "; struct _tagged_string
_temp1722; _temp1722.curr= _temp1721; _temp1722.base= _temp1721; _temp1722.last_plus_one=
_temp1721 + 3; _temp1722;})), Cyc_Absynpp_exp2doc( _temp1617), Cyc_PP_text((
struct _tagged_string)({ char* _temp1723=( char*)") {"; struct _tagged_string
_temp1724; _temp1724.curr= _temp1723; _temp1724.base= _temp1723; _temp1724.last_plus_one=
_temp1723 + 4; _temp1724;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1611))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1725=( char*)"}"; struct _tagged_string _temp1726;
_temp1726.curr= _temp1725; _temp1726.base= _temp1725; _temp1726.last_plus_one=
_temp1725 + 2; _temp1726;}))); goto _LL1542; _LL1564: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1727=( char*)"switch ("; struct
_tagged_string _temp1728; _temp1728.curr= _temp1727; _temp1728.base= _temp1727;
_temp1728.last_plus_one= _temp1727 + 9; _temp1728;})), Cyc_Absynpp_exp2doc(
_temp1629), Cyc_PP_text(( struct _tagged_string)({ char* _temp1729=( char*)") {";
struct _tagged_string _temp1730; _temp1730.curr= _temp1729; _temp1730.base=
_temp1729; _temp1730.last_plus_one= _temp1729 + 4; _temp1730;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1627), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1731=( char*)"}"; struct _tagged_string
_temp1732; _temp1732.curr= _temp1731; _temp1732.base= _temp1731; _temp1732.last_plus_one=
_temp1731 + 2; _temp1732;}))); goto _LL1542; _LL1566: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1733=( char*)"fallthru;"; struct _tagged_string
_temp1734; _temp1734.curr= _temp1733; _temp1734.base= _temp1733; _temp1734.last_plus_one=
_temp1733 + 10; _temp1734;})); goto _LL1542; _LL1568: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1735=( char*)"fallthru("; struct
_tagged_string _temp1736; _temp1736.curr= _temp1735; _temp1736.base= _temp1735;
_temp1736.last_plus_one= _temp1735 + 10; _temp1736;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1637), Cyc_PP_text(( struct _tagged_string)({ char* _temp1737=( char*)");";
struct _tagged_string _temp1738; _temp1738.curr= _temp1737; _temp1738.base=
_temp1737; _temp1738.last_plus_one= _temp1737 + 3; _temp1738;}))); goto _LL1542;
_LL1570: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1641), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1639)); goto _LL1542; _LL1572: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1739=( char*)"cut "; struct
_tagged_string _temp1740; _temp1740.curr= _temp1739; _temp1740.base= _temp1739;
_temp1740.last_plus_one= _temp1739 + 5; _temp1740;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1643))); goto _LL1542; _LL1574: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1741=( char*)"splice "; struct _tagged_string
_temp1742; _temp1742.curr= _temp1741; _temp1742.base= _temp1741; _temp1742.last_plus_one=
_temp1741 + 8; _temp1742;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1645)));
goto _LL1542; _LL1576: if( Cyc_Absynpp_is_declaration( _temp1647)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_textptr( _temp1649), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1743=( char*)": {"; struct _tagged_string _temp1744; _temp1744.curr=
_temp1743; _temp1744.base= _temp1743; _temp1744.last_plus_one= _temp1743 + 4;
_temp1744;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1647)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1745=( char*)"}"; struct _tagged_string _temp1746; _temp1746.curr=
_temp1745; _temp1746.base= _temp1745; _temp1746.last_plus_one= _temp1745 + 2;
_temp1746;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1649), Cyc_PP_text((
struct _tagged_string)({ char* _temp1747=( char*)": "; struct _tagged_string
_temp1748; _temp1748.curr= _temp1747; _temp1748.base= _temp1747; _temp1748.last_plus_one=
_temp1747 + 3; _temp1748;})), Cyc_Absynpp_stmt2doc( _temp1647));} goto _LL1542;
_LL1578: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1749=( char*)"do {"; struct _tagged_string _temp1750; _temp1750.curr=
_temp1749; _temp1750.base= _temp1749; _temp1750.last_plus_one= _temp1749 + 5;
_temp1750;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1657)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1751=( char*)"} while ("; struct _tagged_string _temp1752; _temp1752.curr=
_temp1751; _temp1752.base= _temp1751; _temp1752.last_plus_one= _temp1751 + 10;
_temp1752;})), Cyc_Absynpp_exp2doc( _temp1655), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1753=( char*)");"; struct _tagged_string _temp1754;
_temp1754.curr= _temp1753; _temp1754.base= _temp1753; _temp1754.last_plus_one=
_temp1753 + 3; _temp1754;}))); goto _LL1542; _LL1580: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1755=( char*)"try"; struct _tagged_string
_temp1756; _temp1756.curr= _temp1755; _temp1756.base= _temp1755; _temp1756.last_plus_one=
_temp1755 + 4; _temp1756;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1661)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1757=( char*)"catch {"; struct _tagged_string _temp1758; _temp1758.curr=
_temp1757; _temp1758.base= _temp1757; _temp1758.last_plus_one= _temp1757 + 8;
_temp1758;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1659)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1759=( char*)"}"; struct _tagged_string _temp1760; _temp1760.curr=
_temp1759; _temp1760.base= _temp1759; _temp1760.last_plus_one= _temp1759 + 2;
_temp1760;}))); goto _LL1542; _LL1542:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1761=( void*) p->r;
int _temp1793; void* _temp1795; char _temp1797; struct _tagged_string _temp1799;
struct Cyc_Absyn_Vardecl* _temp1801; struct Cyc_List_List* _temp1803; struct Cyc_Absyn_Pat*
_temp1805; struct Cyc_Absyn_Vardecl* _temp1807; struct _tuple0* _temp1809;
struct Cyc_List_List* _temp1811; struct Cyc_List_List* _temp1813; struct _tuple0*
_temp1815; struct Cyc_List_List* _temp1817; struct Cyc_List_List* _temp1819;
struct _tuple0* _temp1821; struct Cyc_List_List* _temp1823; struct Cyc_List_List*
_temp1825; struct Cyc_Core_Opt* _temp1827; struct Cyc_Absyn_Structdecl*
_temp1829; struct Cyc_Absyn_Enumfield* _temp1831; struct Cyc_Absyn_Enumdecl*
_temp1833; struct Cyc_List_List* _temp1835; struct Cyc_List_List* _temp1837;
struct Cyc_Core_Opt* _temp1839; struct _tuple0* _temp1841; struct Cyc_Absyn_Enumfield*
_temp1843; struct Cyc_Absyn_Xenumdecl* _temp1845; struct Cyc_List_List*
_temp1847; struct Cyc_List_List* _temp1849; struct _tuple0* _temp1851; _LL1763:
if( _temp1761 == Cyc_Absyn_Wild_p){ goto _LL1764;} else{ goto _LL1765;} _LL1765:
if( _temp1761 == Cyc_Absyn_Null_p){ goto _LL1766;} else{ goto _LL1767;} _LL1767:
if(( unsigned int) _temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag ==
Cyc_Absyn_Int_p_tag: 0){ _LL1796: _temp1795=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1761)->f1; goto _LL1794; _LL1794: _temp1793=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1761)->f2; goto _LL1768;} else{ goto _LL1769;} _LL1769: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1798: _temp1797=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1761)->f1;
goto _LL1770;} else{ goto _LL1771;} _LL1771: if(( unsigned int) _temp1761 > 2u?((
struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1800:
_temp1799=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1761)->f1;
goto _LL1772;} else{ goto _LL1773;} _LL1773: if(( unsigned int) _temp1761 > 2u?((
struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1802:
_temp1801=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1761)->f1; goto _LL1774;} else{ goto _LL1775;} _LL1775: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1804: _temp1803=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1761)->f1; goto _LL1776;} else{ goto _LL1777;} _LL1777: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1806: _temp1805=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1761)->f1; goto _LL1778;} else{ goto _LL1779;} _LL1779: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1808: _temp1807=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1761)->f1; goto _LL1780;} else{ goto _LL1781;} _LL1781: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1810: _temp1809=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1761)->f1; goto _LL1782;} else{ goto _LL1783;} _LL1783: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1816: _temp1815=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1761)->f1; goto _LL1814; _LL1814: _temp1813=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1761)->f2; goto _LL1812; _LL1812:
_temp1811=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1761)->f3; goto _LL1784;} else{ goto _LL1785;} _LL1785: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1822: _temp1821=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1761)->f1; goto _LL1820; _LL1820: _temp1819=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1761)->f2; goto _LL1818; _LL1818:
_temp1817=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1761)->f3; goto _LL1786;} else{ goto _LL1787;} _LL1787: if(( unsigned int)
_temp1761 > 2u?(( struct _enum_struct*) _temp1761)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1830: _temp1829=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1761)->f1; goto _LL1828; _LL1828: _temp1827=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1761)->f2; goto _LL1826; _LL1826:
_temp1825=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1761)->f3; goto _LL1824; _LL1824: _temp1823=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1761)->f4; goto _LL1788;} else{ goto
_LL1789;} _LL1789: if(( unsigned int) _temp1761 > 2u?(( struct _enum_struct*)
_temp1761)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1842: _temp1841=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1761)->f1; goto _LL1840; _LL1840:
_temp1839=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1761)->f2;
goto _LL1838; _LL1838: _temp1837=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1761)->f3; goto _LL1836; _LL1836: _temp1835=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1761)->f4; goto _LL1834; _LL1834:
_temp1833=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1761)->f5; goto _LL1832; _LL1832: _temp1831=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1761)->f6; goto _LL1790;} else{ goto
_LL1791;} _LL1791: if(( unsigned int) _temp1761 > 2u?(( struct _enum_struct*)
_temp1761)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1852: _temp1851=( struct
_tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1761)->f1; goto _LL1850;
_LL1850: _temp1849=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1761)->f2; goto _LL1848; _LL1848: _temp1847=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1761)->f3; goto _LL1846; _LL1846:
_temp1845=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1761)->f4; goto _LL1844; _LL1844: _temp1843=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1761)->f5; goto _LL1792;} else{ goto
_LL1762;} _LL1764: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1853=(
char*)"_"; struct _tagged_string _temp1854; _temp1854.curr= _temp1853; _temp1854.base=
_temp1853; _temp1854.last_plus_one= _temp1853 + 2; _temp1854;})); goto _LL1762;
_LL1766: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1855=( char*)"null";
struct _tagged_string _temp1856; _temp1856.curr= _temp1855; _temp1856.base=
_temp1855; _temp1856.last_plus_one= _temp1855 + 5; _temp1856;})); goto _LL1762;
_LL1768: if( _temp1795 == Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1793));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1793));}
goto _LL1762; _LL1770: s= Cyc_PP_text(({ struct _tagged_string _temp1857= Cyc_Absynpp_char_escape(
_temp1797); xprintf("'%.*s'", _temp1857.last_plus_one - _temp1857.curr,
_temp1857.curr);})); goto _LL1762; _LL1772: s= Cyc_PP_text( _temp1799); goto
_LL1762; _LL1774: s= Cyc_Absynpp_qvar2doc( _temp1801->name); goto _LL1762;
_LL1776: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1858=( char*)"$("; struct _tagged_string _temp1859; _temp1859.curr=
_temp1858; _temp1859.base= _temp1858; _temp1859.last_plus_one= _temp1858 + 3;
_temp1859;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1860=( char*)","; struct _tagged_string
_temp1861; _temp1861.curr= _temp1860; _temp1861.base= _temp1860; _temp1861.last_plus_one=
_temp1860 + 2; _temp1861;}), _temp1803), Cyc_PP_text(( struct _tagged_string)({
char* _temp1862=( char*)")"; struct _tagged_string _temp1863; _temp1863.curr=
_temp1862; _temp1863.base= _temp1862; _temp1863.last_plus_one= _temp1862 + 2;
_temp1863;}))); goto _LL1762; _LL1778: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1864=( char*)"&"; struct _tagged_string _temp1865;
_temp1865.curr= _temp1864; _temp1865.base= _temp1864; _temp1865.last_plus_one=
_temp1864 + 2; _temp1865;})), Cyc_Absynpp_pat2doc( _temp1805)); goto _LL1762;
_LL1780: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1866=( char*)"*"; struct _tagged_string _temp1867; _temp1867.curr=
_temp1866; _temp1867.base= _temp1866; _temp1867.last_plus_one= _temp1866 + 2;
_temp1867;})), Cyc_Absynpp_qvar2doc( _temp1807->name)); goto _LL1762; _LL1782: s=
Cyc_Absynpp_qvar2doc( _temp1809); goto _LL1762; _LL1784: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1815), Cyc_Absynpp_tvars2doc( _temp1813), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1868=( char*)"("; struct _tagged_string
_temp1869; _temp1869.curr= _temp1868; _temp1869.base= _temp1868; _temp1869.last_plus_one=
_temp1868 + 2; _temp1869;}),( struct _tagged_string)({ char* _temp1870=( char*)")";
struct _tagged_string _temp1871; _temp1871.curr= _temp1870; _temp1871.base=
_temp1870; _temp1871.last_plus_one= _temp1870 + 2; _temp1871;}),( struct
_tagged_string)({ char* _temp1872=( char*)","; struct _tagged_string _temp1873;
_temp1873.curr= _temp1872; _temp1873.base= _temp1872; _temp1873.last_plus_one=
_temp1872 + 2; _temp1873;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1811))); goto _LL1762; _LL1786: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1821), Cyc_Absynpp_tvars2doc( _temp1819), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1874=( char*)"{"; struct _tagged_string _temp1875;
_temp1875.curr= _temp1874; _temp1875.base= _temp1874; _temp1875.last_plus_one=
_temp1874 + 2; _temp1875;}),( struct _tagged_string)({ char* _temp1876=( char*)"}";
struct _tagged_string _temp1877; _temp1877.curr= _temp1876; _temp1877.base=
_temp1876; _temp1877.last_plus_one= _temp1876 + 2; _temp1877;}),( struct
_tagged_string)({ char* _temp1878=( char*)","; struct _tagged_string _temp1879;
_temp1879.curr= _temp1878; _temp1879.base= _temp1878; _temp1879.last_plus_one=
_temp1878 + 2; _temp1879;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1817))); goto _LL1762; _LL1788: s= Cyc_Absynpp_cat3( _temp1829->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1829->name)->v),
Cyc_Absynpp_tvars2doc( _temp1825), Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1880=( char*)"{"; struct _tagged_string _temp1881; _temp1881.curr=
_temp1880; _temp1881.base= _temp1880; _temp1881.last_plus_one= _temp1880 + 2;
_temp1881;}),( struct _tagged_string)({ char* _temp1882=( char*)"}"; struct
_tagged_string _temp1883; _temp1883.curr= _temp1882; _temp1883.base= _temp1882;
_temp1883.last_plus_one= _temp1882 + 2; _temp1883;}),( struct _tagged_string)({
char* _temp1884=( char*)","; struct _tagged_string _temp1885; _temp1885.curr=
_temp1884; _temp1885.base= _temp1884; _temp1885.last_plus_one= _temp1884 + 2;
_temp1885;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1823))); goto _LL1762; _LL1790: _temp1851= _temp1841; _temp1849= _temp1837;
_temp1847= _temp1835; goto _LL1792; _LL1792: if( _temp1847 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1851);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1851), Cyc_Absynpp_tvars2doc(
_temp1849), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1886=( char*)"(";
struct _tagged_string _temp1887; _temp1887.curr= _temp1886; _temp1887.base=
_temp1886; _temp1887.last_plus_one= _temp1886 + 2; _temp1887;}),( struct
_tagged_string)({ char* _temp1888=( char*)")"; struct _tagged_string _temp1889;
_temp1889.curr= _temp1888; _temp1889.base= _temp1888; _temp1889.last_plus_one=
_temp1888 + 2; _temp1889;}),( struct _tagged_string)({ char* _temp1890=( char*)",";
struct _tagged_string _temp1891; _temp1891.curr= _temp1890; _temp1891.base=
_temp1890; _temp1891.last_plus_one= _temp1890 + 2; _temp1891;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1847)));} goto _LL1762; _LL1762:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1892=( char*)""; struct
_tagged_string _temp1893; _temp1893.curr= _temp1892; _temp1893.base= _temp1892;
_temp1893.last_plus_one= _temp1892 + 1; _temp1893;}),( struct _tagged_string)({
char* _temp1894=( char*)"="; struct _tagged_string _temp1895; _temp1895.curr=
_temp1894; _temp1895.base= _temp1894; _temp1895.last_plus_one= _temp1894 + 2;
_temp1895;}),( struct _tagged_string)({ char* _temp1896=( char*)"="; struct
_tagged_string _temp1897; _temp1897.curr= _temp1896; _temp1897.base= _temp1896;
_temp1897.last_plus_one= _temp1896 + 2; _temp1897;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
== Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1898=( char*)"default: "; struct _tagged_string
_temp1899; _temp1899.curr= _temp1898; _temp1899.base= _temp1898; _temp1899.last_plus_one=
_temp1898 + 10; _temp1899;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1900=( char*)"case "; struct
_tagged_string _temp1901; _temp1901.curr= _temp1900; _temp1901.base= _temp1900;
_temp1901.last_plus_one= _temp1900 + 6; _temp1901;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1902=( char*)": "; struct
_tagged_string _temp1903; _temp1903.curr= _temp1902; _temp1903.base= _temp1902;
_temp1903.last_plus_one= _temp1902 + 3; _temp1903;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1904=( char*)"case "; struct
_tagged_string _temp1905; _temp1905.curr= _temp1904; _temp1905.base= _temp1904;
_temp1905.last_plus_one= _temp1904 + 6; _temp1905;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1906=( char*)" && "; struct
_tagged_string _temp1907; _temp1907.curr= _temp1906; _temp1907.base= _temp1906;
_temp1907.last_plus_one= _temp1906 + 5; _temp1907;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1908= c->where_clause; if(
_temp1908 == 0){ _throw( Null_Exception);} _temp1908;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1909=( char*)": "; struct _tagged_string _temp1910;
_temp1910.curr= _temp1909; _temp1910.base= _temp1909; _temp1910.last_plus_one=
_temp1909 + 3; _temp1910;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp1911=( char*)""; struct _tagged_string _temp1912; _temp1912.curr=
_temp1911; _temp1912.base= _temp1911; _temp1912.last_plus_one= _temp1911 + 1;
_temp1912;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl*
d){ struct Cyc_PP_Doc* s;{ void* _temp1913=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1937; struct Cyc_Absyn_Structdecl* _temp1939; struct Cyc_Absyn_Uniondecl*
_temp1941; struct Cyc_Absyn_Vardecl* _temp1943; struct Cyc_Absyn_Vardecl
_temp1945; struct Cyc_List_List* _temp1946; struct Cyc_Core_Opt* _temp1948; int
_temp1950; struct Cyc_Absyn_Exp* _temp1952; void* _temp1954; struct Cyc_Absyn_Tqual*
_temp1956; struct _tuple0* _temp1958; void* _temp1960; struct Cyc_Absyn_Enumdecl*
_temp1962; struct Cyc_Absyn_Xenumdecl* _temp1964; int _temp1966; struct Cyc_Absyn_Exp*
_temp1968; struct Cyc_Core_Opt* _temp1970; struct Cyc_Core_Opt* _temp1972;
struct Cyc_Absyn_Pat* _temp1974; struct Cyc_Absyn_Typedefdecl* _temp1976; struct
Cyc_List_List* _temp1978; struct _tagged_string* _temp1980; struct Cyc_List_List*
_temp1982; struct _tuple0* _temp1984; struct Cyc_List_List* _temp1986; _LL1915:
if((( struct _enum_struct*) _temp1913)->tag == Cyc_Absyn_Fn_d_tag){ _LL1938:
_temp1937=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1913)->f1;
goto _LL1916;} else{ goto _LL1917;} _LL1917: if((( struct _enum_struct*)
_temp1913)->tag == Cyc_Absyn_Struct_d_tag){ _LL1940: _temp1939=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1913)->f1; goto _LL1918;} else{ goto
_LL1919;} _LL1919: if((( struct _enum_struct*) _temp1913)->tag == Cyc_Absyn_Union_d_tag){
_LL1942: _temp1941=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1913)->f1; goto _LL1920;} else{ goto _LL1921;} _LL1921: if((( struct
_enum_struct*) _temp1913)->tag == Cyc_Absyn_Var_d_tag){ _LL1944: _temp1943=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp1913)->f1;
_temp1945=* _temp1943; _LL1961: _temp1960=( void*) _temp1945.sc; goto _LL1959;
_LL1959: _temp1958=( struct _tuple0*) _temp1945.name; goto _LL1957; _LL1957:
_temp1956=( struct Cyc_Absyn_Tqual*) _temp1945.tq; goto _LL1955; _LL1955:
_temp1954=( void*) _temp1945.type; goto _LL1953; _LL1953: _temp1952=( struct Cyc_Absyn_Exp*)
_temp1945.initializer; goto _LL1951; _LL1951: _temp1950=( int) _temp1945.shadow;
goto _LL1949; _LL1949: _temp1948=( struct Cyc_Core_Opt*) _temp1945.region; goto
_LL1947; _LL1947: _temp1946=( struct Cyc_List_List*) _temp1945.attributes; goto
_LL1922;} else{ goto _LL1923;} _LL1923: if((( struct _enum_struct*) _temp1913)->tag
== Cyc_Absyn_Enum_d_tag){ _LL1963: _temp1962=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1913)->f1; goto _LL1924;} else{ goto
_LL1925;} _LL1925: if((( struct _enum_struct*) _temp1913)->tag == Cyc_Absyn_Xenum_d_tag){
_LL1965: _temp1964=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp1913)->f1; goto _LL1926;} else{ goto _LL1927;} _LL1927: if((( struct
_enum_struct*) _temp1913)->tag == Cyc_Absyn_Let_d_tag){ _LL1975: _temp1974=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1913)->f1; goto
_LL1973; _LL1973: _temp1972=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1913)->f2; goto _LL1971; _LL1971: _temp1970=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1913)->f3; goto _LL1969; _LL1969: _temp1968=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1913)->f4; goto
_LL1967; _LL1967: _temp1966=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1913)->f5;
goto _LL1928;} else{ goto _LL1929;} _LL1929: if((( struct _enum_struct*)
_temp1913)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1977: _temp1976=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1913)->f1; goto _LL1930;} else{ goto
_LL1931;} _LL1931: if((( struct _enum_struct*) _temp1913)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1981: _temp1980=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1913)->f1; goto _LL1979; _LL1979: _temp1978=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1913)->f2; goto _LL1932;} else{ goto
_LL1933;} _LL1933: if((( struct _enum_struct*) _temp1913)->tag == Cyc_Absyn_Using_d_tag){
_LL1985: _temp1984=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1913)->f1; goto _LL1983; _LL1983: _temp1982=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1913)->f2; goto _LL1934;} else{ goto
_LL1935;} _LL1935: if((( struct _enum_struct*) _temp1913)->tag == Cyc_Absyn_ExternC_d_tag){
_LL1987: _temp1986=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1913)->f1; goto _LL1936;} else{ goto _LL1914;} _LL1916: { void* t=({ struct
Cyc_Absyn_FnType_struct* _temp1988=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1988->tag= Cyc_Absyn_FnType_tag;
_temp1988->f1=({ struct Cyc_Absyn_FnInfo _temp1989; _temp1989.tvars= _temp1937->tvs;
_temp1989.effect= _temp1937->effect; _temp1989.ret_typ=( void*)(( void*)
_temp1937->ret_type); _temp1989.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1937->args); _temp1989.varargs= _temp1937->varargs; _temp1989.attributes= 0;
_temp1989;});( void*) _temp1988;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp1937->is_inline?(
struct _tagged_string)({ char* _temp1990=( char*)"inline "; struct
_tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base= _temp1990;
_temp1991.last_plus_one= _temp1990 + 8; _temp1991;}):( struct _tagged_string)({
char* _temp1992=( char*)""; struct _tagged_string _temp1993; _temp1993.curr=
_temp1992; _temp1993.base= _temp1992; _temp1993.last_plus_one= _temp1992 + 1;
_temp1993;})), Cyc_Absynpp_scope2doc(( void*) _temp1937->sc), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp1994=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1994->q_const= 0; _temp1994->q_volatile=
0; _temp1994->q_restrict= 0; _temp1994;}), t,({ struct Cyc_Core_Opt* _temp1995=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1995->v=(
void*) Cyc_Absynpp_cat2( Cyc_Absynpp_atts2doc( _temp1937->attributes), Cyc_Absynpp_qvar2doc(
_temp1937->name)); _temp1995;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1996=( char*)" {"; struct _tagged_string _temp1997; _temp1997.curr=
_temp1996; _temp1997.base= _temp1996; _temp1997.last_plus_one= _temp1996 + 3;
_temp1997;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp1937->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp1998=( char*)"}"; struct _tagged_string _temp1999; _temp1999.curr=
_temp1998; _temp1999.base= _temp1998; _temp1999.last_plus_one= _temp1998 + 2;
_temp1999;}))); goto _LL1914;} _LL1918: if( _temp1939->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp1939->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2000=( char*)"struct "; struct _tagged_string
_temp2001; _temp2001.curr= _temp2000; _temp2001.base= _temp2000; _temp2001.last_plus_one=
_temp2000 + 8; _temp2001;})), _temp1939->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2002=( char*)""; struct _tagged_string _temp2003;
_temp2003.curr= _temp2002; _temp2003.base= _temp2002; _temp2003.last_plus_one=
_temp2002 + 1; _temp2003;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1939->name)->v),
Cyc_Absynpp_tvars2doc( _temp1939->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp2004=( char*)";"; struct _tagged_string _temp2005; _temp2005.curr=
_temp2004; _temp2005.base= _temp2004; _temp2005.last_plus_one= _temp2004 + 2;
_temp2005;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc(( void*)
_temp1939->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2006=( char*)"struct ";
struct _tagged_string _temp2007; _temp2007.curr= _temp2006; _temp2007.base=
_temp2006; _temp2007.last_plus_one= _temp2006 + 8; _temp2007;})), _temp1939->name
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2008=( char*)""; struct
_tagged_string _temp2009; _temp2009.curr= _temp2008; _temp2009.base= _temp2008;
_temp2009.last_plus_one= _temp2008 + 1; _temp2009;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1939->name)->v), Cyc_Absynpp_tvars2doc( _temp1939->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2010=( char*)" {"; struct
_tagged_string _temp2011; _temp2011.curr= _temp2010; _temp2011.base= _temp2010;
_temp2011.last_plus_one= _temp2010 + 3; _temp2011;})), Cyc_PP_nest( 2, _temp1939->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2012=( char*)""; struct
_tagged_string _temp2013; _temp2013.curr= _temp2012; _temp2013.base= _temp2012;
_temp2013.last_plus_one= _temp2012 + 1; _temp2013;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)( _temp1939->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2014=( char*)"}";
struct _tagged_string _temp2015; _temp2015.curr= _temp2014; _temp2015.base=
_temp2014; _temp2015.last_plus_one= _temp2014 + 2; _temp2015;})), Cyc_Absynpp_atts2doc(
_temp1939->attributes), Cyc_PP_text(( struct _tagged_string)({ char* _temp2016=(
char*)";"; struct _tagged_string _temp2017; _temp2017.curr= _temp2016; _temp2017.base=
_temp2016; _temp2017.last_plus_one= _temp2016 + 2; _temp2017;})));} goto _LL1914;
_LL1920: if( _temp1941->fields == 0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc((
void*) _temp1941->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2018=(
char*)"union "; struct _tagged_string _temp2019; _temp2019.curr= _temp2018;
_temp2019.base= _temp2018; _temp2019.last_plus_one= _temp2018 + 7; _temp2019;})),
_temp1941->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2020=(
char*)""; struct _tagged_string _temp2021; _temp2021.curr= _temp2020; _temp2021.base=
_temp2020; _temp2021.last_plus_one= _temp2020 + 1; _temp2021;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1941->name)->v), Cyc_Absynpp_tvars2doc( _temp1941->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2022=( char*)";"; struct
_tagged_string _temp2023; _temp2023.curr= _temp2022; _temp2023.base= _temp2022;
_temp2023.last_plus_one= _temp2022 + 2; _temp2023;})));} else{ s= Cyc_Absynpp_cat10(
Cyc_Absynpp_scope2doc(( void*) _temp1941->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2024=( char*)"union "; struct _tagged_string
_temp2025; _temp2025.curr= _temp2024; _temp2025.base= _temp2024; _temp2025.last_plus_one=
_temp2024 + 7; _temp2025;})), _temp1941->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2026=( char*)""; struct _tagged_string _temp2027;
_temp2027.curr= _temp2026; _temp2027.base= _temp2026; _temp2027.last_plus_one=
_temp2026 + 1; _temp2027;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1941->name)->v),
Cyc_Absynpp_tvars2doc( _temp1941->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp2028=( char*)" {"; struct _tagged_string _temp2029; _temp2029.curr=
_temp2028; _temp2029.base= _temp2028; _temp2029.last_plus_one= _temp2028 + 3;
_temp2029;})), Cyc_PP_nest( 2, _temp1941->fields == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2030=( char*)""; struct _tagged_string _temp2031;
_temp2031.curr= _temp2030; _temp2031.base= _temp2030; _temp2031.last_plus_one=
_temp2030 + 1; _temp2031;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)( _temp1941->fields)->v))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2032=( char*)"}"; struct _tagged_string
_temp2033; _temp2033.curr= _temp2032; _temp2033.base= _temp2032; _temp2033.last_plus_one=
_temp2032 + 2; _temp2033;})), Cyc_Absynpp_atts2doc( _temp1941->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2034=( char*)";"; struct _tagged_string
_temp2035; _temp2035.curr= _temp2034; _temp2035.base= _temp2034; _temp2035.last_plus_one=
_temp2034 + 2; _temp2035;})));} goto _LL1914; _LL1922: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_qvar2doc( _temp1958); if( _temp1950 > 0){ sn= Cyc_Absynpp_cat2( sn,
Cyc_PP_text( xprintf("%d", _temp1950)));} s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp1960), Cyc_Absynpp_tqtd2doc( _temp1956, _temp1954,({ struct Cyc_Core_Opt*
_temp2036=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2036->v=( void*) sn; _temp2036;})), Cyc_Absynpp_atts2doc( _temp1946),
_temp1952 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2037=( char*)"";
struct _tagged_string _temp2038; _temp2038.curr= _temp2037; _temp2038.base=
_temp2037; _temp2038.last_plus_one= _temp2037 + 1; _temp2038;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2039=( char*)" = "; struct
_tagged_string _temp2040; _temp2040.curr= _temp2039; _temp2040.base= _temp2039;
_temp2040.last_plus_one= _temp2039 + 4; _temp2040;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2041= _temp1952; if(
_temp2041 == 0){ _throw( Null_Exception);} _temp2041;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2042=( char*)";"; struct _tagged_string _temp2043;
_temp2043.curr= _temp2042; _temp2043.base= _temp2042; _temp2043.last_plus_one=
_temp2042 + 2; _temp2043;}))); goto _LL1914;} _LL1924: if( _temp1962->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp1962->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2044=( char*)"enum "; struct _tagged_string
_temp2045; _temp2045.curr= _temp2044; _temp2045.base= _temp2044; _temp2045.last_plus_one=
_temp2044 + 6; _temp2045;})), _temp1962->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2046=( char*)""; struct _tagged_string _temp2047;
_temp2047.curr= _temp2046; _temp2047.base= _temp2046; _temp2047.last_plus_one=
_temp2046 + 1; _temp2047;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1962->name)->v),
Cyc_Absynpp_tvars2doc( _temp1962->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp2048=( char*)";"; struct _tagged_string _temp2049; _temp2049.curr=
_temp2048; _temp2049.base= _temp2048; _temp2049.last_plus_one= _temp2048 + 2;
_temp2049;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc(( void*)
_temp1962->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2050=( char*)"enum ";
struct _tagged_string _temp2051; _temp2051.curr= _temp2050; _temp2051.base=
_temp2050; _temp2051.last_plus_one= _temp2050 + 6; _temp2051;})), _temp1962->name
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2052=( char*)""; struct
_tagged_string _temp2053; _temp2053.curr= _temp2052; _temp2053.base= _temp2052;
_temp2053.last_plus_one= _temp2052 + 1; _temp2053;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1962->name)->v), Cyc_Absynpp_tvars2doc( _temp1962->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2054=( char*)" {"; struct
_tagged_string _temp2055; _temp2055.curr= _temp2054; _temp2055.base= _temp2054;
_temp2055.last_plus_one= _temp2054 + 3; _temp2055;})), Cyc_PP_nest( 2, _temp1962->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2056=( char*)""; struct
_tagged_string _temp2057; _temp2057.curr= _temp2056; _temp2057.base= _temp2056;
_temp2057.last_plus_one= _temp2056 + 1; _temp2057;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)( _temp1962->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2058=( char*)"};";
struct _tagged_string _temp2059; _temp2059.curr= _temp2058; _temp2059.base=
_temp2058; _temp2059.last_plus_one= _temp2058 + 3; _temp2059;})));} goto _LL1914;
_LL1926: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) _temp1964->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2060=( char*)"xenum "; struct _tagged_string
_temp2061; _temp2061.curr= _temp2060; _temp2061.base= _temp2060; _temp2061.last_plus_one=
_temp2060 + 7; _temp2061;})), Cyc_Absynpp_qvar2doc( _temp1964->name), _temp1964->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2062=( char*)";"; struct
_tagged_string _temp2063; _temp2063.curr= _temp2062; _temp2063.base= _temp2062;
_temp2063.last_plus_one= _temp2062 + 2; _temp2063;})): Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp2064=( char*)" {"; struct _tagged_string
_temp2065; _temp2065.curr= _temp2064; _temp2065.base= _temp2064; _temp2065.last_plus_one=
_temp2064 + 3; _temp2065;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc( _temp1964->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2066=( char*)"};"; struct _tagged_string
_temp2067; _temp2067.curr= _temp2066; _temp2067.base= _temp2066; _temp2067.last_plus_one=
_temp2066 + 3; _temp2067;})))); goto _LL1914; _LL1928: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2068=( char*)"let "; struct _tagged_string
_temp2069; _temp2069.curr= _temp2068; _temp2069.base= _temp2068; _temp2069.last_plus_one=
_temp2068 + 5; _temp2069;})), Cyc_Absynpp_pat2doc( _temp1974), Cyc_PP_text((
struct _tagged_string)({ char* _temp2070=( char*)" = "; struct _tagged_string
_temp2071; _temp2071.curr= _temp2070; _temp2071.base= _temp2070; _temp2071.last_plus_one=
_temp2070 + 4; _temp2071;})), Cyc_Absynpp_exp2doc( _temp1968), Cyc_PP_text((
struct _tagged_string)({ char* _temp2072=( char*)";"; struct _tagged_string
_temp2073; _temp2073.curr= _temp2072; _temp2073.base= _temp2072; _temp2073.last_plus_one=
_temp2072 + 2; _temp2073;}))); goto _LL1914; _LL1930: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp2074=( char*)"typedef "; struct
_tagged_string _temp2075; _temp2075.curr= _temp2074; _temp2075.base= _temp2074;
_temp2075.last_plus_one= _temp2074 + 9; _temp2075;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp2076=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2076->q_const= 0; _temp2076->q_volatile=
0; _temp2076->q_restrict= 0; _temp2076;}),( void*) _temp1976->defn,({ struct Cyc_Core_Opt*
_temp2077=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2077->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1976->name),
Cyc_Absynpp_tvars2doc( _temp1976->tvs)); _temp2077;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2078=( char*)";"; struct _tagged_string _temp2079;
_temp2079.curr= _temp2078; _temp2079.base= _temp2078; _temp2079.last_plus_one=
_temp2078 + 2; _temp2079;}))); goto _LL1914; _LL1932: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2080=( char*)"namespace "; struct
_tagged_string _temp2081; _temp2081.curr= _temp2080; _temp2081.base= _temp2080;
_temp2081.last_plus_one= _temp2080 + 11; _temp2081;})), Cyc_Absynpp_textptr(
_temp1980), Cyc_PP_text(( struct _tagged_string)({ char* _temp2082=( char*)" {";
struct _tagged_string _temp2083; _temp2083.curr= _temp2082; _temp2083.base=
_temp2082; _temp2083.last_plus_one= _temp2082 + 3; _temp2083;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2084=( char*)""; struct _tagged_string
_temp2085; _temp2085.curr= _temp2084; _temp2085.base= _temp2084; _temp2085.last_plus_one=
_temp2084 + 1; _temp2085;}), _temp1978), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2086=( char*)"}"; struct _tagged_string _temp2087;
_temp2087.curr= _temp2086; _temp2087.base= _temp2086; _temp2087.last_plus_one=
_temp2086 + 2; _temp2087;}))); goto _LL1914; _LL1934: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2088=( char*)"using "; struct _tagged_string
_temp2089; _temp2089.curr= _temp2088; _temp2089.base= _temp2088; _temp2089.last_plus_one=
_temp2088 + 7; _temp2089;})), Cyc_Absynpp_qvar2doc( _temp1984), Cyc_PP_text((
struct _tagged_string)({ char* _temp2090=( char*)" {"; struct _tagged_string
_temp2091; _temp2091.curr= _temp2090; _temp2091.base= _temp2090; _temp2091.last_plus_one=
_temp2090 + 3; _temp2091;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2092=( char*)""; struct _tagged_string _temp2093; _temp2093.curr=
_temp2092; _temp2093.base= _temp2092; _temp2093.last_plus_one= _temp2092 + 1;
_temp2093;}), _temp1982), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2094=( char*)"}"; struct _tagged_string _temp2095; _temp2095.curr=
_temp2094; _temp2095.base= _temp2094; _temp2095.last_plus_one= _temp2094 + 2;
_temp2095;}))); goto _LL1914; _LL1936: s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2096=( char*)"extern \"C\" {"; struct
_tagged_string _temp2097; _temp2097.curr= _temp2096; _temp2097.base= _temp2096;
_temp2097.last_plus_one= _temp2096 + 13; _temp2097;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2098=( char*)""; struct _tagged_string
_temp2099; _temp2099.curr= _temp2098; _temp2099.base= _temp2098; _temp2099.last_plus_one=
_temp2098 + 1; _temp2099;}), _temp1986), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2100=( char*)"}"; struct _tagged_string _temp2101;
_temp2101.curr= _temp2100; _temp2101.base= _temp2100; _temp2101.last_plus_one=
_temp2100 + 2; _temp2101;}))); goto _LL1914; _LL1914:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2102= sc; _LL2104: if( _temp2102 ==
Cyc_Absyn_Static){ goto _LL2105;} else{ goto _LL2106;} _LL2106: if( _temp2102 ==
Cyc_Absyn_Public){ goto _LL2107;} else{ goto _LL2108;} _LL2108: if( _temp2102 ==
Cyc_Absyn_Extern){ goto _LL2109;} else{ goto _LL2110;} _LL2110: if( _temp2102 ==
Cyc_Absyn_ExternC){ goto _LL2111;} else{ goto _LL2112;} _LL2112: if( _temp2102
== Cyc_Absyn_Abstract){ goto _LL2113;} else{ goto _LL2103;} _LL2105: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2114=( char*)"static "; struct
_tagged_string _temp2115; _temp2115.curr= _temp2114; _temp2115.base= _temp2114;
_temp2115.last_plus_one= _temp2114 + 8; _temp2115;})); _LL2107: return Cyc_PP_nil_doc();
_LL2109: return Cyc_PP_text(( struct _tagged_string)({ char* _temp2116=( char*)"extern ";
struct _tagged_string _temp2117; _temp2117.curr= _temp2116; _temp2117.base=
_temp2116; _temp2117.last_plus_one= _temp2116 + 8; _temp2117;})); _LL2111:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2118=( char*)"extern \"C\"";
struct _tagged_string _temp2119; _temp2119.curr= _temp2118; _temp2119.base=
_temp2118; _temp2119.last_plus_one= _temp2118 + 11; _temp2119;})); _LL2113:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2120=( char*)"abstract ";
struct _tagged_string _temp2121; _temp2121.curr= _temp2120; _temp2121.base=
_temp2120; _temp2121.last_plus_one= _temp2120 + 10; _temp2121;})); _LL2103:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual* tq, void* t){ void*
_temp2122= t; struct Cyc_Absyn_Exp* _temp2136; struct Cyc_Absyn_Tqual* _temp2138;
void* _temp2140; struct Cyc_Absyn_PtrInfo _temp2142; struct Cyc_Absyn_Conref*
_temp2144; struct Cyc_Absyn_Tqual* _temp2146; struct Cyc_Absyn_Conref* _temp2148;
void* _temp2150; void* _temp2152; struct Cyc_Absyn_FnInfo _temp2154; struct Cyc_List_List*
_temp2156; int _temp2158; struct Cyc_List_List* _temp2160; void* _temp2162;
struct Cyc_Core_Opt* _temp2164; struct Cyc_List_List* _temp2166; int _temp2168;
struct Cyc_Core_Opt* _temp2170; void* _temp2172; struct Cyc_Core_Opt* _temp2174;
struct Cyc_List_List* _temp2176; struct _tuple0* _temp2178; _LL2124: if((
unsigned int) _temp2122 > 4u?(( struct _enum_struct*) _temp2122)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2141: _temp2140=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2122)->f1;
goto _LL2139; _LL2139: _temp2138=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2122)->f2; goto _LL2137; _LL2137: _temp2136=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2122)->f3; goto _LL2125;} else{ goto
_LL2126;} _LL2126: if(( unsigned int) _temp2122 > 4u?(( struct _enum_struct*)
_temp2122)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2143: _temp2142=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2122)->f1;
_LL2153: _temp2152=( void*) _temp2142.elt_typ; goto _LL2151; _LL2151: _temp2150=(
void*) _temp2142.rgn_typ; goto _LL2149; _LL2149: _temp2148=( struct Cyc_Absyn_Conref*)
_temp2142.nullable; goto _LL2147; _LL2147: _temp2146=( struct Cyc_Absyn_Tqual*)
_temp2142.tq; goto _LL2145; _LL2145: _temp2144=( struct Cyc_Absyn_Conref*)
_temp2142.bounds; goto _LL2127;} else{ goto _LL2128;} _LL2128: if(( unsigned int)
_temp2122 > 4u?(( struct _enum_struct*) _temp2122)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2155: _temp2154=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2122)->f1; _LL2167: _temp2166=( struct Cyc_List_List*) _temp2154.tvars;
goto _LL2165; _LL2165: _temp2164=( struct Cyc_Core_Opt*) _temp2154.effect; goto
_LL2163; _LL2163: _temp2162=( void*) _temp2154.ret_typ; goto _LL2161; _LL2161:
_temp2160=( struct Cyc_List_List*) _temp2154.args; goto _LL2159; _LL2159:
_temp2158=( int) _temp2154.varargs; goto _LL2157; _LL2157: _temp2156=( struct
Cyc_List_List*) _temp2154.attributes; goto _LL2129;} else{ goto _LL2130;}
_LL2130: if(( unsigned int) _temp2122 > 4u?(( struct _enum_struct*) _temp2122)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2173: _temp2172=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2122)->f1; goto _LL2171; _LL2171: _temp2170=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2122)->f2; goto _LL2169; _LL2169: _temp2168=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2122)->f3; goto _LL2131;} else{ goto
_LL2132;} _LL2132: if(( unsigned int) _temp2122 > 4u?(( struct _enum_struct*)
_temp2122)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2179: _temp2178=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2122)->f1; goto _LL2177;
_LL2177: _temp2176=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2122)->f2; goto _LL2175; _LL2175: _temp2174=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2122)->f3; goto _LL2133;} else{ goto
_LL2134;} _LL2134: goto _LL2135; _LL2125: { struct Cyc_List_List* _temp2182;
void* _temp2184; struct Cyc_Absyn_Tqual* _temp2186; struct _tuple4 _temp2180=
Cyc_Absynpp_to_tms( _temp2138, _temp2140); _LL2187: _temp2186= _temp2180.f1;
goto _LL2185; _LL2185: _temp2184= _temp2180.f2; goto _LL2183; _LL2183: _temp2182=
_temp2180.f3; goto _LL2181; _LL2181: { void* tm; if( Cyc_Evexp_eval_const_uint_exp(
_temp2136) ==( unsigned int) 0){ tm= Cyc_Absyn_Carray_mod;} else{ tm=({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2188=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2188->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp2188->f1= _temp2136;( void*) _temp2188;});} return({ struct _tuple4
_temp2189; _temp2189.f1= _temp2186; _temp2189.f2= _temp2184; _temp2189.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2190=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2190->hd=( void*) tm; _temp2190->tl=
_temp2182; _temp2190;}); _temp2189;});}} _LL2127: { struct Cyc_List_List*
_temp2193; void* _temp2195; struct Cyc_Absyn_Tqual* _temp2197; struct _tuple4
_temp2191= Cyc_Absynpp_to_tms( _temp2146, _temp2152); _LL2198: _temp2197=
_temp2191.f1; goto _LL2196; _LL2196: _temp2195= _temp2191.f2; goto _LL2194;
_LL2194: _temp2193= _temp2191.f3; goto _LL2192; _LL2192: { void* ps;{ struct
_tuple9 _temp2200=({ struct _tuple9 _temp2199; _temp2199.f1=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2148))->v; _temp2199.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2144))->v; _temp2199;}); void* _temp2210;
void* _temp2212; struct Cyc_Absyn_Exp* _temp2214; void* _temp2216; int _temp2218;
void* _temp2220; void* _temp2222; struct Cyc_Absyn_Exp* _temp2224; void*
_temp2226; int _temp2228; void* _temp2230; void* _temp2232; void* _temp2234;
_LL2202: _LL2217: _temp2216= _temp2200.f1; if(( unsigned int) _temp2216 > 1u?((
struct _enum_struct*) _temp2216)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2219:
_temp2218=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2216)->f1; if(
_temp2218 == 1){ goto _LL2211;} else{ goto _LL2204;}} else{ goto _LL2204;}
_LL2211: _temp2210= _temp2200.f2; if(( unsigned int) _temp2210 > 1u?(( struct
_enum_struct*) _temp2210)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2213:
_temp2212=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2210)->f1; if((
unsigned int) _temp2212 > 1u?(( struct _enum_struct*) _temp2212)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2215: _temp2214=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2212)->f1; goto _LL2203;} else{ goto _LL2204;}} else{ goto _LL2204;}
_LL2204: _LL2227: _temp2226= _temp2200.f1; if(( unsigned int) _temp2226 > 1u?((
struct _enum_struct*) _temp2226)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2229:
_temp2228=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2226)->f1; if(
_temp2228 == 0){ goto _LL2221;} else{ goto _LL2206;}} else{ goto _LL2206;}
_LL2221: _temp2220= _temp2200.f2; if(( unsigned int) _temp2220 > 1u?(( struct
_enum_struct*) _temp2220)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2223:
_temp2222=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2220)->f1; if((
unsigned int) _temp2222 > 1u?(( struct _enum_struct*) _temp2222)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2225: _temp2224=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2222)->f1; goto _LL2205;} else{ goto _LL2206;}} else{ goto _LL2206;}
_LL2206: _LL2235: _temp2234= _temp2200.f1; goto _LL2231; _LL2231: _temp2230=
_temp2200.f2; if(( unsigned int) _temp2230 > 1u?(( struct _enum_struct*)
_temp2230)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2233: _temp2232=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2230)->f1; if( _temp2232 == Cyc_Absyn_Unknown_b){
goto _LL2207;} else{ goto _LL2208;}} else{ goto _LL2208;} _LL2208: goto _LL2209;
_LL2203: ps=({ struct Cyc_Absyn_Nullable_ps_struct* _temp2236=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp2236->tag= Cyc_Absyn_Nullable_ps_tag;
_temp2236->f1= _temp2214;( void*) _temp2236;}); goto _LL2201; _LL2205: ps=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp2237=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp2237->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp2237->f1= _temp2224;( void*) _temp2237;}); goto _LL2201; _LL2207: ps= Cyc_Absyn_TaggedArray_ps;
goto _LL2201; _LL2209: ps= Cyc_Absyn_TaggedArray_ps; goto _LL2201; _LL2201:;}{
void* tm=({ struct Cyc_Absyn_Pointer_mod_struct* _temp2238=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2238->tag= Cyc_Absyn_Pointer_mod_tag;
_temp2238->f1=( void*) ps; _temp2238->f2=( void*) _temp2150; _temp2238->f3= tq;(
void*) _temp2238;}); return({ struct _tuple4 _temp2239; _temp2239.f1= _temp2197;
_temp2239.f2= _temp2195; _temp2239.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2240=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2240->hd=( void*) tm; _temp2240->tl= _temp2193; _temp2240;}); _temp2239;});}}}
_LL2129: { struct Cyc_List_List* _temp2243; void* _temp2245; struct Cyc_Absyn_Tqual*
_temp2247; struct _tuple4 _temp2241= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp2162); _LL2248: _temp2247= _temp2241.f1; goto _LL2246; _LL2246: _temp2245=
_temp2241.f2; goto _LL2244; _LL2244: _temp2243= _temp2241.f3; goto _LL2242;
_LL2242: { struct Cyc_List_List* tms= _temp2243; if( _temp2156 != 0){ tms=({
struct Cyc_List_List* _temp2249=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2249->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp2250=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp2250->tag= Cyc_Absyn_Attributes_mod_tag;
_temp2250->f1= 0; _temp2250->f2= _temp2156;( void*) _temp2250;}); _temp2249->tl=
tms; _temp2249;});} tms=({ struct Cyc_List_List* _temp2251=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2251->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2252=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2252->tag= Cyc_Absyn_Function_mod_tag; _temp2252->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2253=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2253->tag= Cyc_Absyn_WithTypes_tag; _temp2253->f1= _temp2160; _temp2253->f2=
_temp2158; _temp2253->f3= _temp2164;( void*) _temp2253;});( void*) _temp2252;});
_temp2251->tl= tms; _temp2251;}); if( _temp2166 != 0){ tms=({ struct Cyc_List_List*
_temp2254=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2254->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2255=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp2255->tag= Cyc_Absyn_TypeParams_mod_tag; _temp2255->f1= _temp2166;
_temp2255->f2= 0; _temp2255->f3= 1;( void*) _temp2255;}); _temp2254->tl= tms;
_temp2254;});} return({ struct _tuple4 _temp2256; _temp2256.f1= _temp2247;
_temp2256.f2= _temp2245; _temp2256.f3= tms; _temp2256;});}} _LL2131: if(
_temp2170 == 0){ return({ struct _tuple4 _temp2257; _temp2257.f1= tq; _temp2257.f2=
t; _temp2257.f3= 0; _temp2257;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)
_temp2170->v);} _LL2133: if( _temp2174 == 0? 1: ! Cyc_Absynpp_expand_typedefs){
return({ struct _tuple4 _temp2258; _temp2258.f1= tq; _temp2258.f2= t; _temp2258.f3=
0; _temp2258;});} else{ return Cyc_Absynpp_to_tms( tq,( void*) _temp2174->v);}
_LL2135: return({ struct _tuple4 _temp2259; _temp2259.f1= tq; _temp2259.f2= t;
_temp2259.f3= 0; _temp2259;}); _LL2123:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2262; void* _temp2264; struct Cyc_Absyn_Tqual* _temp2266; struct _tuple4
_temp2260= Cyc_Absynpp_to_tms( tq, t); _LL2267: _temp2266= _temp2260.f1; goto
_LL2265; _LL2265: _temp2264= _temp2260.f2; goto _LL2263; _LL2263: _temp2262=
_temp2260.f3; goto _LL2261; _LL2261: _temp2262=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2262); if( _temp2262 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2266), Cyc_Absynpp_ntyp2doc(
_temp2264));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2266),
Cyc_Absynpp_ntyp2doc( _temp2264), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2268=( char*)" "; struct _tagged_string _temp2269; _temp2269.curr=
_temp2268; _temp2269.base= _temp2268; _temp2269.last_plus_one= _temp2268 + 2;
_temp2269;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)
dopt->v, _temp2262));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc( struct
Cyc_Absyn_Structfield* f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc( f->tq,(
void*) f->type,({ struct Cyc_Core_Opt* _temp2270=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2270->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2270;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2271=( char*)";"; struct _tagged_string
_temp2272; _temp2272.curr= _temp2271; _temp2272.base= _temp2271; _temp2272.last_plus_one=
_temp2271 + 2; _temp2272;})));} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2273=( char*)""; struct _tagged_string _temp2274; _temp2274.curr= _temp2273;
_temp2274.base= _temp2273; _temp2274.last_plus_one= _temp2273 + 1; _temp2274;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( f->name), f->tag == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp2275=( char*)"=";
struct _tagged_string _temp2276; _temp2276.curr= _temp2275; _temp2276.base=
_temp2275; _temp2276.last_plus_one= _temp2275 + 2; _temp2276;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2277= f->tag; if( _temp2277
== 0){ _throw( Null_Exception);} _temp2277;}))), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,( struct _tagged_string)({ char*
_temp2278=( char*)","; struct _tagged_string _temp2279; _temp2279.curr=
_temp2278; _temp2279.base= _temp2278; _temp2279.last_plus_one= _temp2278 + 2;
_temp2279;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl= tdl->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd), 72, f); fprintf( f,"\n");}}
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){
return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct _tagged_string)({ char*
_temp2280=( char*)""; struct _tagged_string _temp2281; _temp2281.curr= _temp2280;
_temp2281.base= _temp2280; _temp2281.last_plus_one= _temp2280 + 1; _temp2281;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc, tdl)), 72);}
struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s),
72);} struct _tagged_string Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_typ2doc( t), 72);} struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0* v){ return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);}
struct _tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);}