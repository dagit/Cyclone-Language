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
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl;
typedef void* Cyc_Absyn_Designator; typedef struct _xenum_struct* Cyc_Absyn_StmtAnnot;
typedef void* Cyc_Absyn_Attribute; struct Cyc_Absyn_Structfield; typedef void*
Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual; typedef void*
Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar*
Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref*
Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl; typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl;
typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl; typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl; typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct*
Cyc_Absyn_stmt_annot_t; typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List*
Cyc_Absyn_attributes_t; typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t;
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
} ; extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; extern const int
Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ; extern void*
Cyc_Absyn_HeapRgn; extern const int Cyc_Absyn_AccessEff_tag; struct Cyc_Absyn_AccessEff_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_JoinEff_tag; struct Cyc_Absyn_JoinEff_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_NoTypes_tag;
struct Cyc_Absyn_NoTypes_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; extern const int Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{
int tag; struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef
void* Cyc_Absyn_Pointer_Sort; extern const int Cyc_Absyn_NonNullable_ps_tag;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Nullable_ps_tag; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern void* Cyc_Absyn_TaggedArray_ps;
extern const int Cyc_Absyn_Regparm_att_tag; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; extern void* Cyc_Absyn_Stdcall_att; extern void* Cyc_Absyn_Cdecl_att;
extern void* Cyc_Absyn_Noreturn_att; extern void* Cyc_Absyn_Const_att; extern
const int Cyc_Absyn_Aligned_att_tag; struct Cyc_Absyn_Aligned_att_struct{ int
tag; int f1; } ; extern void* Cyc_Absyn_Packed_att; extern const int Cyc_Absyn_Section_att_tag;
struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_string f1; } ;
extern void* Cyc_Absyn_Nocommon_att; extern void* Cyc_Absyn_Shared_att; extern
void* Cyc_Absyn_Unused_att; extern void* Cyc_Absyn_Weak_att; extern void* Cyc_Absyn_Dllimport_att;
extern void* Cyc_Absyn_Dllexport_att; extern void* Cyc_Absyn_No_instrument_function_att;
extern void* Cyc_Absyn_Constructor_att; extern void* Cyc_Absyn_Destructor_att;
extern void* Cyc_Absyn_No_check_memory_usage_att; extern void* Cyc_Absyn_Carray_mod;
extern const int Cyc_Absyn_ConstArray_mod_tag; struct Cyc_Absyn_ConstArray_mod_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Pointer_mod_tag;
struct Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual*
f3; } ; extern const int Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
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
Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ;
extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{ int tag;
struct _tuple0* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
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
int Cyc_Absyn_Sizeof_e_tag; struct Cyc_Absyn_Sizeof_e_struct{ int tag; void* f1;
} ; extern const int Cyc_Absyn_Deref_e_tag; struct Cyc_Absyn_Deref_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_StructMember_e_tag;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; extern const int Cyc_Absyn_StructArrow_e_tag;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; extern const int Cyc_Absyn_Subscript_e_tag; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; extern const
int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; extern const int Cyc_Absyn_CompoundLit_e_tag; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Array_e_tag;
struct Cyc_Absyn_Array_e_struct{ int tag; int f1; struct Cyc_List_List* f2; } ;
extern const int Cyc_Absyn_Comprehension_e_tag; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_Struct_e_tag; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; extern const int Cyc_Absyn_Enum_e_tag;
struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_Xenum_e_tag; struct Cyc_Absyn_Xenum_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Xenumdecl*
f3; struct Cyc_Absyn_Enumfield* f4; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag;
struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_StmtExp_e_tag; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern const
int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Fill_e_tag; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; extern void* Cyc_Absyn_Skip_s;
extern const int Cyc_Absyn_Exp_s_tag; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Seq_s_tag; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_Return_s_tag; struct Cyc_Absyn_Return_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_IfThenElse_s_tag; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
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
Cyc_Absyn_Structdecl* f1; } ; extern void* Cyc_Absyn_Union_d; extern const int
Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl*
f1; } ; extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
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
structdecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* xenumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; typedef void* Cyc_Tcenv_Jumpee;
extern void* Cyc_Tcenv_NotLoop_j; extern void* Cyc_Tcenv_CaseEnd_j; extern void*
Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag; struct Cyc_Tcenv_Stmt_j_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t;
typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
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
_temp394 > 5u?(( struct _enum_struct*) _temp394)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL403: _temp402=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp394)->f1;
goto _LL397;} else{ goto _LL398;} _LL398: if(( unsigned int) _temp394 > 5u?((
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
struct _tuple0* _temp512; struct Cyc_Core_Opt* _temp514; struct Cyc_List_List*
_temp516; struct _tuple0* _temp518; void* _temp520; void* _temp522; struct Cyc_List_List*
_temp524; _LL436: if(( unsigned int) _temp434 > 5u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL479: _temp478=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp434)->f1; goto _LL477; _LL477: _temp476=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp434)->f2;
goto _LL475; _LL475: _temp474=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp434)->f3; goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int)
_temp434 > 5u?(( struct _enum_struct*) _temp434)->tag == Cyc_Absyn_FnType_tag: 0){
_LL481: _temp480=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp434)->f1; goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp434 > 5u?(( struct _enum_struct*) _temp434)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL483: _temp482=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp434)->f1; goto _LL441;} else{ goto _LL442;} _LL442: if( _temp434 == Cyc_Absyn_VoidType){
goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_Evar_tag: 0){ _LL489: _temp488=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp434)->f1; goto _LL487; _LL487:
_temp486=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp434)->f2;
goto _LL485; _LL485: _temp484=( int)(( struct Cyc_Absyn_Evar_struct*) _temp434)->f3;
goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_VarType_tag: 0){ _LL491:
_temp490=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp434)->f1;
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL497:
_temp496=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp434)->f1;
goto _LL495; _LL495: _temp494=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp434)->f2; goto _LL493; _LL493: _temp492=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp434)->f3; goto _LL449;} else{ goto
_LL450;} _LL450: if(( unsigned int) _temp434 > 5u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL501: _temp500=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp434)->f1; goto _LL499; _LL499: _temp498=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp434)->f2;
goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_IntType_tag: 0){ _LL505:
_temp504=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp434)->f1; goto _LL503;
_LL503: _temp502=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp434)->f2;
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp434 == Cyc_Absyn_FloatType){
goto _LL455;} else{ goto _LL456;} _LL456: if( _temp434 == Cyc_Absyn_DoubleType){
goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL507:
_temp506=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp434)->f1;
goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_StructType_tag: 0){ _LL513:
_temp512=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp434)->f1;
goto _LL511; _LL511: _temp510=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp434)->f2; goto _LL509; _LL509: _temp508=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp434)->f3; goto _LL461;} else{ goto
_LL462;} _LL462: if(( unsigned int) _temp434 > 5u?(( struct _enum_struct*)
_temp434)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL519: _temp518=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp434)->f1; goto _LL517;
_LL517: _temp516=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp434)->f2; goto _LL515; _LL515: _temp514=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp434)->f3; goto _LL463;} else{ goto _LL464;}
_LL464: if(( unsigned int) _temp434 > 5u?(( struct _enum_struct*) _temp434)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL521: _temp520=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp434)->f1; goto _LL465;} else{ goto _LL466;} _LL466: if( _temp434 == Cyc_Absyn_UnionType){
goto _LL467;} else{ goto _LL468;} _LL468: if( _temp434 == Cyc_Absyn_HeapRgn){
goto _LL469;} else{ goto _LL470;} _LL470: if(( unsigned int) _temp434 > 5u?((
struct _enum_struct*) _temp434)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL523:
_temp522=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp434)->f1; goto
_LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp434 > 5u?(( struct
_enum_struct*) _temp434)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL525: _temp524=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp434)->f1; goto
_LL473;} else{ goto _LL435;} _LL437: return Cyc_PP_text(( struct _tagged_string)({
char* _temp526=( char*)"[[[array]]]"; struct _tagged_string _temp527; _temp527.curr=
_temp526; _temp527.base= _temp526; _temp527.last_plus_one= _temp526 + 12;
_temp527;})); _LL439: return Cyc_PP_nil_doc(); _LL441: return Cyc_PP_nil_doc();
_LL443: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp528=( char*)"void";
struct _tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 5; _temp529;})); goto _LL435; _LL445: if(
_temp486 != 0){ return Cyc_Absynpp_ntyp2doc(( void*) _temp486->v);} else{ s= Cyc_Absynpp_cat5(
Cyc_PP_text(( struct _tagged_string)({ char* _temp530=( char*)"%"; struct
_tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 2; _temp531;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp532=( char*)"("; struct _tagged_string _temp533;
_temp533.curr= _temp532; _temp533.base= _temp532; _temp533.last_plus_one=
_temp532 + 2; _temp533;})), Cyc_PP_text( xprintf("%d", _temp484)), Cyc_PP_text((
struct _tagged_string)({ char* _temp534=( char*)")::"; struct _tagged_string
_temp535; _temp535.curr= _temp534; _temp535.base= _temp534; _temp535.last_plus_one=
_temp534 + 4; _temp535;})), Cyc_Absynpp_kind2doc( _temp488));} goto _LL435;
_LL447: if( Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr(
_temp490->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp536=( char*)"::";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 3; _temp537;})), Cyc_Absynpp_ckind2doc(
_temp490->kind));} else{ s= Cyc_Absynpp_textptr( _temp490->name);} goto _LL435;
_LL449: if( _temp496 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp538=( char*)"enum "; struct _tagged_string _temp539;
_temp539.curr= _temp538; _temp539.base= _temp538; _temp539.last_plus_one=
_temp538 + 6; _temp539;})), Cyc_Absynpp_tps2doc( _temp494));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp540=( char*)"enum "; struct
_tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 6; _temp541;})), Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct _tuple0* _temp542= _temp496; if( _temp542 == 0){
_throw( Null_Exception);} _temp542;})), Cyc_Absynpp_tps2doc( _temp494));} goto
_LL435; _LL451: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp543=( char*)"xenum "; struct _tagged_string _temp544; _temp544.curr=
_temp543; _temp544.base= _temp543; _temp544.last_plus_one= _temp543 + 7;
_temp544;})), Cyc_Absynpp_qvar2doc( _temp500)); goto _LL435; _LL453: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp545= _temp504; _LL547:
if( _temp545 == Cyc_Absyn_Signed){ goto _LL548;} else{ goto _LL549;} _LL549: if(
_temp545 == Cyc_Absyn_Unsigned){ goto _LL550;} else{ goto _LL546;} _LL548: sns=(
struct _tagged_string)({ char* _temp551=( char*)""; struct _tagged_string
_temp552; _temp552.curr= _temp551; _temp552.base= _temp551; _temp552.last_plus_one=
_temp551 + 1; _temp552;}); goto _LL546; _LL550: sns=( struct _tagged_string)({
char* _temp553=( char*)"unsigned "; struct _tagged_string _temp554; _temp554.curr=
_temp553; _temp554.base= _temp553; _temp554.last_plus_one= _temp553 + 10;
_temp554;}); goto _LL546; _LL546:;}{ void* _temp555= _temp502; _LL557: if(
_temp555 == Cyc_Absyn_B1){ goto _LL558;} else{ goto _LL559;} _LL559: if(
_temp555 == Cyc_Absyn_B2){ goto _LL560;} else{ goto _LL561;} _LL561: if(
_temp555 == Cyc_Absyn_B4){ goto _LL562;} else{ goto _LL563;} _LL563: if(
_temp555 == Cyc_Absyn_B8){ goto _LL564;} else{ goto _LL556;} _LL558:{ void*
_temp565= _temp504; _LL567: if( _temp565 == Cyc_Absyn_Signed){ goto _LL568;}
else{ goto _LL569;} _LL569: if( _temp565 == Cyc_Absyn_Unsigned){ goto _LL570;}
else{ goto _LL566;} _LL568: sns=( struct _tagged_string)({ char* _temp571=( char*)"signed ";
struct _tagged_string _temp572; _temp572.curr= _temp571; _temp572.base= _temp571;
_temp572.last_plus_one= _temp571 + 8; _temp572;}); goto _LL566; _LL570: sns=(
struct _tagged_string)({ char* _temp573=( char*)""; struct _tagged_string
_temp574; _temp574.curr= _temp573; _temp574.base= _temp573; _temp574.last_plus_one=
_temp573 + 1; _temp574;}); goto _LL566; _LL566:;} ts=( struct _tagged_string)({
char* _temp575=( char*)"char"; struct _tagged_string _temp576; _temp576.curr=
_temp575; _temp576.base= _temp575; _temp576.last_plus_one= _temp575 + 5;
_temp576;}); goto _LL556; _LL560: ts=( struct _tagged_string)({ char* _temp577=(
char*)"short"; struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base=
_temp577; _temp578.last_plus_one= _temp577 + 6; _temp578;}); goto _LL556; _LL562:
ts=( struct _tagged_string)({ char* _temp579=( char*)"int"; struct
_tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 4; _temp580;}); goto _LL556; _LL564: ts=(
struct _tagged_string)({ char* _temp581=( char*)"long long"; struct
_tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 10; _temp582;}); goto _LL556; _LL556:;} s=
Cyc_PP_text(({ struct _tagged_string _temp583= sns; struct _tagged_string
_temp584= ts; xprintf("%.*s%.*s", _temp583.last_plus_one - _temp583.curr,
_temp583.curr, _temp584.last_plus_one - _temp584.curr, _temp584.curr);})); goto
_LL435;} _LL455: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp585=( char*)"float";
struct _tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 6; _temp586;})); goto _LL435; _LL457: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp587=( char*)"double"; struct _tagged_string
_temp588; _temp588.curr= _temp587; _temp588.base= _temp587; _temp588.last_plus_one=
_temp587 + 7; _temp588;})); goto _LL435; _LL459: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp589=( char*)"$"; struct _tagged_string
_temp590; _temp590.curr= _temp589; _temp590.base= _temp589; _temp590.last_plus_one=
_temp589 + 2; _temp590;})), Cyc_Absynpp_args2doc( _temp506)); goto _LL435;
_LL461: if( _temp512 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp591=( char*)"struct "; struct _tagged_string
_temp592; _temp592.curr= _temp591; _temp592.base= _temp591; _temp592.last_plus_one=
_temp591 + 8; _temp592;})), Cyc_Absynpp_tps2doc( _temp510));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp593=( char*)"struct "; struct
_tagged_string _temp594; _temp594.curr= _temp593; _temp594.base= _temp593;
_temp594.last_plus_one= _temp593 + 8; _temp594;})), _temp512 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp595= _temp512;
if( _temp595 == 0){ _throw( Null_Exception);} _temp595;})), Cyc_Absynpp_tps2doc(
_temp510));} goto _LL435; _LL463: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp518), Cyc_Absynpp_tps2doc( _temp516)); goto _LL435; _LL465: s= Cyc_Absynpp_rgn2doc(
_temp520); goto _LL435; _LL467: s= Cyc_PP_text(( struct _tagged_string)({ char*
_temp596=( char*)"union {unimplemented}"; struct _tagged_string _temp597;
_temp597.curr= _temp596; _temp597.base= _temp596; _temp597.last_plus_one=
_temp596 + 22; _temp597;})); goto _LL435; _LL469: s= Cyc_Absynpp_rgn2doc( t);
goto _LL435; _LL471: s= Cyc_Absynpp_eff2doc( t); goto _LL435; _LL473: s= Cyc_Absynpp_eff2doc(
t); goto _LL435; _LL435:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*) vo->v));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp598=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp598->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)((* t).f1)->v));
_temp598;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp599=( char*)"("; struct _tagged_string _temp600;
_temp600.curr= _temp599; _temp600.base= _temp599; _temp600.last_plus_one=
_temp599 + 2; _temp600;}),( struct _tagged_string)({ char* _temp601=( char*)")";
struct _tagged_string _temp602; _temp602.curr= _temp601; _temp602.base= _temp601;
_temp602.last_plus_one= _temp601 + 2; _temp602;}),( struct _tagged_string)({
char* _temp603=( char*)","; struct _tagged_string _temp604; _temp604.curr=
_temp603; _temp604.base= _temp603; _temp604.last_plus_one= _temp603 + 2;
_temp604;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp605=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp605->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp606=( char*)"..."; struct _tagged_string _temp607;
_temp607.curr= _temp606; _temp607.base= _temp606; _temp607.last_plus_one=
_temp606 + 4; _temp607;})); _temp605->tl= 0; _temp605;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp608=( char*)"("; struct _tagged_string
_temp609; _temp609.curr= _temp608; _temp609.base= _temp608; _temp609.last_plus_one=
_temp608 + 2; _temp609;}),( struct _tagged_string)({ char* _temp610=( char*)")";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 2; _temp611;}),( struct _tagged_string)({
char* _temp612=( char*)","; struct _tagged_string _temp613; _temp613.curr=
_temp612; _temp613.base= _temp612; _temp613.last_plus_one= _temp612 + 2;
_temp613;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp614=(
char*)"("; struct _tagged_string _temp615; _temp615.curr= _temp614; _temp615.base=
_temp614; _temp615.last_plus_one= _temp614 + 2; _temp615;}),( struct
_tagged_string)({ char* _temp616=( char*)")"; struct _tagged_string _temp617;
_temp617.curr= _temp616; _temp617.base= _temp616; _temp617.last_plus_one=
_temp616 + 2; _temp617;}),( struct _tagged_string)({ char* _temp618=( char*)";";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 2; _temp619;}),({ struct Cyc_List_List*
_temp620=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp620->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp622=( char*)""; struct _tagged_string _temp623; _temp623.curr= _temp622;
_temp623.base= _temp622; _temp623.last_plus_one= _temp622 + 1; _temp623;}),(
struct _tagged_string)({ char* _temp624=( char*)""; struct _tagged_string
_temp625; _temp625.curr= _temp624; _temp625.base= _temp624; _temp625.last_plus_one=
_temp624 + 1; _temp625;}),( struct _tagged_string)({ char* _temp626=( char*)",";
struct _tagged_string _temp627; _temp627.curr= _temp626; _temp627.base= _temp626;
_temp627.last_plus_one= _temp626 + 2; _temp627;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp628=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp628->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp629=(
char*)"..."; struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base=
_temp629; _temp630.last_plus_one= _temp629 + 4; _temp630;})); _temp628->tl= 0;
_temp628;}))); _temp620->tl=({ struct Cyc_List_List* _temp621=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp621->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp621->tl= 0; _temp621;}); _temp620;}));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp631=( char*)"("; struct _tagged_string
_temp632; _temp632.curr= _temp631; _temp632.base= _temp631; _temp632.last_plus_one=
_temp631 + 2; _temp632;}),( struct _tagged_string)({ char* _temp633=( char*)")";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 2; _temp634;}),( struct _tagged_string)({
char* _temp635=( char*)";"; struct _tagged_string _temp636; _temp636.curr=
_temp635; _temp636.base= _temp635; _temp636.last_plus_one= _temp635 + 2;
_temp636;}),({ struct Cyc_List_List* _temp637=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp637->hd=( void*) Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp639=( char*)""; struct _tagged_string _temp640;
_temp640.curr= _temp639; _temp640.base= _temp639; _temp640.last_plus_one=
_temp639 + 1; _temp640;}),( struct _tagged_string)({ char* _temp641=( char*)"";
struct _tagged_string _temp642; _temp642.curr= _temp641; _temp642.base= _temp641;
_temp642.last_plus_one= _temp641 + 1; _temp642;}),( struct _tagged_string)({
char* _temp643=( char*)","; struct _tagged_string _temp644; _temp644.curr=
_temp643; _temp644.base= _temp643; _temp644.last_plus_one= _temp643 + 2;
_temp644;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));
_temp637->tl=({ struct Cyc_List_List* _temp638=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp638->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp638->tl= 0; _temp638;}); _temp637;}));}}} struct _tuple1*
Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({ struct _tuple1* _temp645=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp645->f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp646=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp646->v=( void*)(* arg).f1; _temp646;}); _temp645->f2=(* arg).f2;
_temp645->f3=(* arg).f3; _temp645;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc(
struct _tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* prefix= 0;{ void* _temp647=(* q).f1;
struct Cyc_List_List* _temp659; struct Cyc_List_List* _temp661; struct Cyc_List_List*
_temp663; struct Cyc_List_List* _temp665; _LL649: if( _temp647 == Cyc_Absyn_Loc_n){
goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp647 > 1u?((
struct _enum_struct*) _temp647)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL660:
_temp659=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp647)->f1;
if( _temp659 == 0){ goto _LL652;} else{ goto _LL653;}} else{ goto _LL653;}
_LL653: if(( unsigned int) _temp647 > 1u?(( struct _enum_struct*) _temp647)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL662: _temp661=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp647)->f1; if( _temp661 == 0){ goto _LL654;} else{
goto _LL655;}} else{ goto _LL655;} _LL655: if(( unsigned int) _temp647 > 1u?((
struct _enum_struct*) _temp647)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL664:
_temp663=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp647)->f1;
goto _LL656;} else{ goto _LL657;} _LL657: if(( unsigned int) _temp647 > 1u?((
struct _enum_struct*) _temp647)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL666:
_temp665=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp647)->f1;
goto _LL658;} else{ goto _LL648;} _LL650: goto _LL652; _LL652: goto _LL654;
_LL654: return Cyc_Absynpp_var2doc((* q).f2); _LL656: _temp665= _temp663; goto
_LL658; _LL658: prefix= _temp665; goto _LL648; _LL648:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp667=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp667->hd=( void*)(* q).f2; _temp667->tl= 0; _temp667;})),( struct
_tagged_string)({ char* _temp668=( char*)"_"; struct _tagged_string _temp669;
_temp669.curr= _temp668; _temp669.base= _temp668; _temp669.last_plus_one=
_temp668 + 2; _temp669;})));} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp670=( char*)""; struct _tagged_string
_temp671; _temp671.curr= _temp670; _temp671.base= _temp670; _temp671.last_plus_one=
_temp670 + 1; _temp671;}),( struct _tagged_string)({ char* _temp672=( char*)"::";
struct _tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 3; _temp673;}),( struct _tagged_string)({
char* _temp674=( char*)"::"; struct _tagged_string _temp675; _temp675.curr=
_temp674; _temp675.base= _temp674; _temp675.last_plus_one= _temp674 + 3;
_temp675;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_var2doc,
prefix)), Cyc_Absynpp_var2doc((* q).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp676=( void*) e->r; void* _temp740; void*
_temp742; struct _tuple0* _temp744; struct _tuple0* _temp746; struct Cyc_List_List*
_temp748; void* _temp750; struct Cyc_Absyn_Exp* _temp752; struct Cyc_Core_Opt*
_temp754; struct Cyc_Absyn_Exp* _temp756; void* _temp758; struct Cyc_Absyn_Exp*
_temp760; struct Cyc_Absyn_Exp* _temp762; struct Cyc_Absyn_Exp* _temp764; struct
Cyc_Absyn_Exp* _temp766; struct Cyc_Absyn_Exp* _temp768; struct Cyc_Absyn_Exp*
_temp770; struct Cyc_List_List* _temp772; struct Cyc_Absyn_Exp* _temp774; struct
Cyc_List_List* _temp776; struct Cyc_Absyn_Exp* _temp778; struct Cyc_Absyn_Exp*
_temp780; struct Cyc_Absyn_Exp* _temp782; struct Cyc_List_List* _temp784; struct
Cyc_Absyn_Exp* _temp786; struct Cyc_Absyn_Exp* _temp788; void* _temp790; struct
Cyc_Absyn_Exp* _temp792; void* _temp794; struct Cyc_Absyn_Exp* _temp796; struct
_tagged_string* _temp798; struct Cyc_Absyn_Exp* _temp800; struct _tagged_string*
_temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Exp* _temp806; struct
Cyc_Absyn_Exp* _temp808; struct Cyc_List_List* _temp810; struct Cyc_List_List*
_temp812; struct _tuple1* _temp814; struct Cyc_List_List* _temp816; int _temp818;
struct Cyc_Absyn_Exp* _temp820; struct Cyc_Absyn_Exp* _temp822; struct Cyc_Absyn_Vardecl*
_temp824; struct Cyc_Absyn_Structdecl* _temp826; struct Cyc_List_List* _temp828;
struct Cyc_Core_Opt* _temp830; struct _tuple0* _temp832; struct Cyc_Absyn_Enumfield*
_temp834; struct Cyc_Absyn_Enumdecl* _temp836; struct Cyc_List_List* _temp838;
struct Cyc_Core_Opt* _temp840; struct Cyc_Core_Opt* _temp842; struct Cyc_Absyn_Enumfield*
_temp844; struct Cyc_Absyn_Xenumdecl* _temp846; struct Cyc_List_List* _temp848;
struct Cyc_Core_Opt* _temp850; struct Cyc_List_List* _temp852; struct Cyc_Core_Opt*
_temp854; struct Cyc_Absyn_Stmt* _temp856; struct Cyc_Absyn_Fndecl* _temp858;
struct Cyc_Absyn_Exp* _temp860; _LL678: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_Const_e_tag){ _LL741: _temp740=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp676)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Var_e_tag){ _LL745: _temp744=( struct
_tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp676)->f1; goto _LL743; _LL743:
_temp742=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp676)->f2; goto _LL681;}
else{ goto _LL682;} _LL682: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL747: _temp746=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp676)->f1; goto _LL683;} else{ goto _LL684;} _LL684: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Primop_e_tag){ _LL751: _temp750=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp676)->f1; goto _LL749; _LL749:
_temp748=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp676)->f2;
goto _LL685;} else{ goto _LL686;} _LL686: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL757: _temp756=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp676)->f1; goto _LL755; _LL755: _temp754=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp676)->f2; goto
_LL753; _LL753: _temp752=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp676)->f3; goto _LL687;} else{ goto _LL688;} _LL688: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Increment_e_tag){ _LL761: _temp760=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp676)->f1;
goto _LL759; _LL759: _temp758=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp676)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Conditional_e_tag){ _LL767: _temp766=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp676)->f1;
goto _LL765; _LL765: _temp764=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp676)->f2; goto _LL763; _LL763: _temp762=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp676)->f3; goto _LL691;} else{ goto _LL692;}
_LL692: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL771: _temp770=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp676)->f1; goto _LL769; _LL769: _temp768=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp676)->f2; goto _LL693;} else{ goto _LL694;}
_LL694: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL775: _temp774=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp676)->f1; goto _LL773; _LL773: _temp772=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp676)->f2; goto _LL695;} else{ goto _LL696;}
_LL696: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_FnCall_e_tag){
_LL779: _temp778=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp676)->f1; goto _LL777; _LL777: _temp776=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp676)->f2; goto _LL697;} else{ goto _LL698;}
_LL698: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Throw_e_tag){
_LL781: _temp780=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp676)->f1; goto _LL699;} else{ goto _LL700;} _LL700: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL783:
_temp782=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp676)->f1; goto _LL701;} else{ goto _LL702;} _LL702: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL787: _temp786=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp676)->f1;
goto _LL785; _LL785: _temp784=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp676)->f2; goto _LL703;} else{ goto _LL704;} _LL704: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Cast_e_tag){ _LL791: _temp790=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp676)->f1; goto _LL789; _LL789: _temp788=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp676)->f2; goto
_LL705;} else{ goto _LL706;} _LL706: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_Address_e_tag){ _LL793: _temp792=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp676)->f1; goto _LL707;} else{ goto _LL708;}
_LL708: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL795: _temp794=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp676)->f1;
goto _LL709;} else{ goto _LL710;} _LL710: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_Deref_e_tag){ _LL797: _temp796=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp676)->f1; goto _LL711;} else{ goto _LL712;}
_LL712: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_StructMember_e_tag){
_LL801: _temp800=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp676)->f1; goto _LL799; _LL799: _temp798=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp676)->f2; goto _LL713;} else{ goto _LL714;}
_LL714: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL805: _temp804=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp676)->f1; goto _LL803; _LL803: _temp802=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp676)->f2; goto _LL715;} else{ goto _LL716;}
_LL716: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Subscript_e_tag){
_LL809: _temp808=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp676)->f1; goto _LL807; _LL807: _temp806=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp676)->f2; goto _LL717;} else{ goto _LL718;}
_LL718: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Tuple_e_tag){
_LL811: _temp810=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp676)->f1; goto _LL719;} else{ goto _LL720;} _LL720: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL815: _temp814=(
struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp676)->f1; goto
_LL813; _LL813: _temp812=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp676)->f2; goto _LL721;} else{ goto _LL722;} _LL722: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Array_e_tag){ _LL819: _temp818=( int)((
struct Cyc_Absyn_Array_e_struct*) _temp676)->f1; goto _LL817; _LL817: _temp816=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp676)->f2; goto
_LL723;} else{ goto _LL724;} _LL724: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL825: _temp824=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp676)->f1; goto _LL823; _LL823:
_temp822=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp676)->f2; goto _LL821; _LL821: _temp820=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp676)->f3; goto _LL725;} else{ goto
_LL726;} _LL726: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Struct_e_tag){
_LL833: _temp832=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp676)->f1; goto _LL831; _LL831: _temp830=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp676)->f2; goto _LL829; _LL829: _temp828=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp676)->f3; goto
_LL827; _LL827: _temp826=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp676)->f4; goto _LL727;} else{ goto _LL728;} _LL728: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Enum_e_tag){ _LL843: _temp842=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp676)->f1; goto
_LL841; _LL841: _temp840=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp676)->f2; goto _LL839; _LL839: _temp838=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp676)->f3; goto _LL837; _LL837: _temp836=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp676)->f4; goto
_LL835; _LL835: _temp834=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp676)->f5; goto _LL729;} else{ goto _LL730;} _LL730: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_Xenum_e_tag){ _LL851: _temp850=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp676)->f1; goto
_LL849; _LL849: _temp848=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp676)->f2; goto _LL847; _LL847: _temp846=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp676)->f3; goto _LL845; _LL845: _temp844=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp676)->f4;
goto _LL731;} else{ goto _LL732;} _LL732: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_UnresolvedMem_e_tag){ _LL855: _temp854=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp676)->f1; goto _LL853; _LL853:
_temp852=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp676)->f2; goto _LL733;} else{ goto _LL734;} _LL734: if((( struct
_enum_struct*) _temp676)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL857: _temp856=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp676)->f1;
goto _LL735;} else{ goto _LL736;} _LL736: if((( struct _enum_struct*) _temp676)->tag
== Cyc_Absyn_Codegen_e_tag){ _LL859: _temp858=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp676)->f1; goto _LL737;} else{ goto
_LL738;} _LL738: if((( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Fill_e_tag){
_LL861: _temp860=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp676)->f1; goto _LL739;} else{ goto _LL677;} _LL679: return 10000; _LL681:
return 10000; _LL683: return 10000; _LL685: { void* _temp862= _temp750; _LL864:
if( _temp862 == Cyc_Absyn_Plus){ goto _LL865;} else{ goto _LL866;} _LL866: if(
_temp862 == Cyc_Absyn_Times){ goto _LL867;} else{ goto _LL868;} _LL868: if(
_temp862 == Cyc_Absyn_Minus){ goto _LL869;} else{ goto _LL870;} _LL870: if(
_temp862 == Cyc_Absyn_Div){ goto _LL871;} else{ goto _LL872;} _LL872: if(
_temp862 == Cyc_Absyn_Mod){ goto _LL873;} else{ goto _LL874;} _LL874: if(
_temp862 == Cyc_Absyn_Eq){ goto _LL875;} else{ goto _LL876;} _LL876: if(
_temp862 == Cyc_Absyn_Neq){ goto _LL877;} else{ goto _LL878;} _LL878: if(
_temp862 == Cyc_Absyn_Gt){ goto _LL879;} else{ goto _LL880;} _LL880: if(
_temp862 == Cyc_Absyn_Lt){ goto _LL881;} else{ goto _LL882;} _LL882: if(
_temp862 == Cyc_Absyn_Gte){ goto _LL883;} else{ goto _LL884;} _LL884: if(
_temp862 == Cyc_Absyn_Lte){ goto _LL885;} else{ goto _LL886;} _LL886: if(
_temp862 == Cyc_Absyn_Not){ goto _LL887;} else{ goto _LL888;} _LL888: if(
_temp862 == Cyc_Absyn_Bitnot){ goto _LL889;} else{ goto _LL890;} _LL890: if(
_temp862 == Cyc_Absyn_Bitand){ goto _LL891;} else{ goto _LL892;} _LL892: if(
_temp862 == Cyc_Absyn_Bitor){ goto _LL893;} else{ goto _LL894;} _LL894: if(
_temp862 == Cyc_Absyn_Bitxor){ goto _LL895;} else{ goto _LL896;} _LL896: if(
_temp862 == Cyc_Absyn_Bitlshift){ goto _LL897;} else{ goto _LL898;} _LL898: if(
_temp862 == Cyc_Absyn_Bitlrshift){ goto _LL899;} else{ goto _LL900;} _LL900: if(
_temp862 == Cyc_Absyn_Bitarshift){ goto _LL901;} else{ goto _LL902;} _LL902: if(
_temp862 == Cyc_Absyn_Size){ goto _LL903;} else{ goto _LL904;} _LL904: if(
_temp862 == Cyc_Absyn_Printf){ goto _LL905;} else{ goto _LL906;} _LL906: if(
_temp862 == Cyc_Absyn_Fprintf){ goto _LL907;} else{ goto _LL908;} _LL908: if(
_temp862 == Cyc_Absyn_Xprintf){ goto _LL909;} else{ goto _LL910;} _LL910: if(
_temp862 == Cyc_Absyn_Scanf){ goto _LL911;} else{ goto _LL912;} _LL912: if(
_temp862 == Cyc_Absyn_Fscanf){ goto _LL913;} else{ goto _LL914;} _LL914: if(
_temp862 == Cyc_Absyn_Sscanf){ goto _LL915;} else{ goto _LL863;} _LL865: return
100; _LL867: return 110; _LL869: return 100; _LL871: return 110; _LL873: return
110; _LL875: return 70; _LL877: return 70; _LL879: return 80; _LL881: return 80;
_LL883: return 80; _LL885: return 80; _LL887: return 130; _LL889: return 130;
_LL891: return 60; _LL893: return 40; _LL895: return 50; _LL897: return 90;
_LL899: return 80; _LL901: return 80; _LL903: return 140; _LL905: return 130;
_LL907: return 130; _LL909: return 130; _LL911: return 130; _LL913: return 130;
_LL915: return 130; _LL863:;} _LL687: return 20; _LL689: return 20; _LL691:
return 30; _LL693: return 10; _LL695: return 140; _LL697: return 140; _LL699:
return 130; _LL701: return Cyc_Absynpp_exp_prec( _temp782); _LL703: return Cyc_Absynpp_exp_prec(
_temp786); _LL705: return 120; _LL707: return 130; _LL709: return 130; _LL711:
return 130; _LL713: return 140; _LL715: return 140; _LL717: return 140; _LL719:
return 150; _LL721: return 140; _LL723: return 140; _LL725: return 140; _LL727:
return 140; _LL729: return 140; _LL731: return 140; _LL733: return 140; _LL735:
return 10000; _LL737: return 140; _LL739: return 140; _LL677:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp916=(
void*) e->r; void* _temp986; void* _temp988; struct Cyc_Absyn_Vardecl* _temp990;
struct _tuple0* _temp992; void* _temp994; struct Cyc_Absyn_Vardecl* _temp996;
struct _tuple0* _temp998; void* _temp1000; struct _tuple0* _temp1002; struct
_tuple0* _temp1004; struct Cyc_List_List* _temp1006; void* _temp1008; struct Cyc_Absyn_Exp*
_temp1010; struct Cyc_Core_Opt* _temp1012; struct Cyc_Absyn_Exp* _temp1014; void*
_temp1016; struct Cyc_Absyn_Exp* _temp1018; struct Cyc_Absyn_Exp* _temp1020;
struct Cyc_Absyn_Exp* _temp1022; struct Cyc_Absyn_Exp* _temp1024; struct Cyc_Absyn_Exp*
_temp1026; struct Cyc_Absyn_Exp* _temp1028; struct Cyc_List_List* _temp1030;
struct Cyc_Absyn_Exp* _temp1032; struct Cyc_List_List* _temp1034; struct Cyc_Absyn_Exp*
_temp1036; struct Cyc_Absyn_Exp* _temp1038; struct Cyc_Absyn_Exp* _temp1040;
struct Cyc_List_List* _temp1042; struct Cyc_Absyn_Exp* _temp1044; struct Cyc_Absyn_Exp*
_temp1046; void* _temp1048; struct Cyc_Absyn_Exp* _temp1050; void* _temp1052;
struct Cyc_Absyn_Exp* _temp1054; struct _tagged_string* _temp1056; struct Cyc_Absyn_Exp*
_temp1058; struct _tagged_string* _temp1060; struct Cyc_Absyn_Exp* _temp1062;
struct Cyc_Absyn_Exp* _temp1064; struct Cyc_Absyn_Exp* _temp1066; struct Cyc_List_List*
_temp1068; struct Cyc_List_List* _temp1070; struct _tuple1* _temp1072; struct
Cyc_List_List* _temp1074; int _temp1076; struct Cyc_List_List* _temp1078; int
_temp1080; struct Cyc_Absyn_Exp* _temp1082; struct Cyc_Absyn_Exp* _temp1084;
struct Cyc_Absyn_Vardecl* _temp1086; struct Cyc_Absyn_Structdecl* _temp1088;
struct Cyc_List_List* _temp1090; struct Cyc_Core_Opt* _temp1092; struct _tuple0*
_temp1094; struct Cyc_Absyn_Enumfield* _temp1096; struct Cyc_Absyn_Enumdecl*
_temp1098; struct Cyc_List_List* _temp1100; struct Cyc_Core_Opt* _temp1102;
struct Cyc_Core_Opt* _temp1104; struct Cyc_Absyn_Enumfield* _temp1106; struct
Cyc_Absyn_Xenumdecl* _temp1108; struct Cyc_List_List* _temp1110; struct Cyc_Core_Opt*
_temp1112; struct Cyc_List_List* _temp1114; struct Cyc_Core_Opt* _temp1116;
struct Cyc_Absyn_Stmt* _temp1118; struct Cyc_Absyn_Fndecl* _temp1120; struct Cyc_Absyn_Exp*
_temp1122; _LL918: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Const_e_tag){
_LL987: _temp986=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp916)->f1;
goto _LL919;} else{ goto _LL920;} _LL920: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_Var_e_tag){ _LL993: _temp992=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp916)->f1; goto _LL989; _LL989: _temp988=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp916)->f2; if(( unsigned int) _temp988 > 1u?(( struct _enum_struct*)
_temp988)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL991: _temp990=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp988)->f1; goto _LL921;} else{ goto _LL922;}}
else{ goto _LL922;} _LL922: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Var_e_tag){
_LL999: _temp998=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp916)->f1;
goto _LL995; _LL995: _temp994=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp916)->f2; if(( unsigned int) _temp994 > 1u?(( struct _enum_struct*)
_temp994)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL997: _temp996=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp994)->f1; goto _LL923;} else{ goto _LL924;}}
else{ goto _LL924;} _LL924: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Var_e_tag){
_LL1003: _temp1002=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp916)->f1;
goto _LL1001; _LL1001: _temp1000=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp916)->f2; goto _LL925;} else{ goto _LL926;} _LL926: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1005: _temp1004=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp916)->f1; goto
_LL927;} else{ goto _LL928;} _LL928: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_Primop_e_tag){ _LL1009: _temp1008=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp916)->f1; goto _LL1007; _LL1007: _temp1006=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp916)->f2; goto _LL929;} else{ goto
_LL930;} _LL930: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1015: _temp1014=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp916)->f1; goto _LL1013; _LL1013: _temp1012=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp916)->f2; goto _LL1011; _LL1011: _temp1010=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp916)->f3;
goto _LL931;} else{ goto _LL932;} _LL932: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1019: _temp1018=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp916)->f1; goto _LL1017; _LL1017:
_temp1016=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp916)->f2; goto
_LL933;} else{ goto _LL934;} _LL934: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL1025: _temp1024=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp916)->f1; goto _LL1023; _LL1023:
_temp1022=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp916)->f2; goto _LL1021; _LL1021: _temp1020=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp916)->f3; goto _LL935;} else{ goto
_LL936;} _LL936: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL1029: _temp1028=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp916)->f1; goto _LL1027; _LL1027: _temp1026=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp916)->f2; goto _LL937;} else{ goto
_LL938;} _LL938: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL1033: _temp1032=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp916)->f1; goto _LL1031; _LL1031: _temp1030=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp916)->f2; goto _LL939;} else{ goto
_LL940;} _LL940: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1037: _temp1036=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp916)->f1; goto _LL1035; _LL1035: _temp1034=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp916)->f2; goto _LL941;} else{ goto
_LL942;} _LL942: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Throw_e_tag){
_LL1039: _temp1038=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp916)->f1; goto _LL943;} else{ goto _LL944;} _LL944: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1041:
_temp1040=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp916)->f1; goto _LL945;} else{ goto _LL946;} _LL946: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1045:
_temp1044=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp916)->f1; goto _LL1043; _LL1043: _temp1042=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp916)->f2; goto _LL947;} else{ goto
_LL948;} _LL948: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Cast_e_tag){
_LL1049: _temp1048=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp916)->f1;
goto _LL1047; _LL1047: _temp1046=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp916)->f2; goto _LL949;} else{ goto _LL950;} _LL950: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Address_e_tag){ _LL1051: _temp1050=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp916)->f1; goto
_LL951;} else{ goto _LL952;} _LL952: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_Sizeof_e_tag){ _LL1053: _temp1052=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp916)->f1; goto _LL953;} else{ goto _LL954;} _LL954: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Deref_e_tag){ _LL1055: _temp1054=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp916)->f1; goto
_LL955;} else{ goto _LL956;} _LL956: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL1059: _temp1058=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp916)->f1; goto _LL1057; _LL1057:
_temp1056=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp916)->f2; goto _LL957;} else{ goto _LL958;} _LL958: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL1063:
_temp1062=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp916)->f1; goto _LL1061; _LL1061: _temp1060=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp916)->f2; goto _LL959;} else{ goto
_LL960;} _LL960: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1067: _temp1066=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp916)->f1; goto _LL1065; _LL1065: _temp1064=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp916)->f2; goto _LL961;} else{ goto
_LL962;} _LL962: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1069: _temp1068=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp916)->f1; goto _LL963;} else{ goto _LL964;} _LL964: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1073:
_temp1072=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp916)->f1;
goto _LL1071; _LL1071: _temp1070=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp916)->f2; goto _LL965;} else{ goto _LL966;} _LL966: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Array_e_tag){ _LL1077: _temp1076=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp916)->f1; if( _temp1076 == 1){
goto _LL1075;} else{ goto _LL968;} _LL1075: _temp1074=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp916)->f2; goto _LL967;} else{ goto _LL968;}
_LL968: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_Array_e_tag){
_LL1081: _temp1080=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp916)->f1;
goto _LL1079; _LL1079: _temp1078=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp916)->f2; goto _LL969;} else{ goto _LL970;} _LL970: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1087:
_temp1086=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp916)->f1; goto _LL1085; _LL1085: _temp1084=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp916)->f2; goto _LL1083; _LL1083:
_temp1082=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp916)->f3; goto _LL971;} else{ goto _LL972;} _LL972: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Struct_e_tag){ _LL1095: _temp1094=(
struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp916)->f1; goto
_LL1093; _LL1093: _temp1092=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp916)->f2; goto _LL1091; _LL1091: _temp1090=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp916)->f3; goto _LL1089; _LL1089:
_temp1088=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp916)->f4; goto _LL973;} else{ goto _LL974;} _LL974: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Enum_e_tag){ _LL1105: _temp1104=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp916)->f1; goto
_LL1103; _LL1103: _temp1102=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp916)->f2; goto _LL1101; _LL1101: _temp1100=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp916)->f3; goto _LL1099; _LL1099: _temp1098=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp916)->f4;
goto _LL1097; _LL1097: _temp1096=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp916)->f5; goto _LL975;} else{ goto _LL976;} _LL976: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1113: _temp1112=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp916)->f1; goto
_LL1111; _LL1111: _temp1110=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp916)->f2; goto _LL1109; _LL1109: _temp1108=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp916)->f3; goto _LL1107; _LL1107:
_temp1106=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp916)->f4; goto _LL977;} else{ goto _LL978;} _LL978: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1117:
_temp1116=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp916)->f1; goto _LL1115; _LL1115: _temp1114=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp916)->f2; goto _LL979;} else{
goto _LL980;} _LL980: if((( struct _enum_struct*) _temp916)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1119: _temp1118=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp916)->f1; goto _LL981;} else{ goto _LL982;} _LL982: if((( struct
_enum_struct*) _temp916)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1121: _temp1120=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp916)->f1;
goto _LL983;} else{ goto _LL984;} _LL984: if((( struct _enum_struct*) _temp916)->tag
== Cyc_Absyn_Fill_e_tag){ _LL1123: _temp1122=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Fill_e_struct*) _temp916)->f1; goto _LL985;} else{ goto _LL917;}
_LL919: s= Cyc_Absynpp_cnst2doc( _temp986); goto _LL917; _LL921: s= Cyc_Absynpp_qvar2doc(
_temp992); if( _temp990->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp990->shadow)));} goto _LL917; _LL923: s= Cyc_Absynpp_qvar2doc(
_temp998); if( _temp996->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp996->shadow)));} goto _LL917; _LL925: s= Cyc_Absynpp_qvar2doc(
_temp1002); goto _LL917; _LL927: s= Cyc_Absynpp_qvar2doc( _temp1004); goto
_LL917; _LL929: s= Cyc_Absynpp_primapp2doc( myprec, _temp1008, _temp1006); goto
_LL917; _LL931: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec, _temp1014),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1124=( char*)" "; struct
_tagged_string _temp1125; _temp1125.curr= _temp1124; _temp1125.base= _temp1124;
_temp1125.last_plus_one= _temp1124 + 2; _temp1125;})), _temp1012 == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp1126=( char*)""; struct _tagged_string
_temp1127; _temp1127.curr= _temp1126; _temp1127.base= _temp1126; _temp1127.last_plus_one=
_temp1126 + 1; _temp1127;})): Cyc_Absynpp_prim2doc(( void*) _temp1012->v), Cyc_PP_text((
struct _tagged_string)({ char* _temp1128=( char*)"= "; struct _tagged_string
_temp1129; _temp1129.curr= _temp1128; _temp1129.base= _temp1128; _temp1129.last_plus_one=
_temp1128 + 3; _temp1129;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1010));
goto _LL917; _LL933: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1018);{ void* _temp1130= _temp1016; _LL1132: if( _temp1130 == Cyc_Absyn_PreInc){
goto _LL1133;} else{ goto _LL1134;} _LL1134: if( _temp1130 == Cyc_Absyn_PreDec){
goto _LL1135;} else{ goto _LL1136;} _LL1136: if( _temp1130 == Cyc_Absyn_PostInc){
goto _LL1137;} else{ goto _LL1138;} _LL1138: if( _temp1130 == Cyc_Absyn_PostDec){
goto _LL1139;} else{ goto _LL1131;} _LL1133: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1140=( char*)"++"; struct _tagged_string
_temp1141; _temp1141.curr= _temp1140; _temp1141.base= _temp1140; _temp1141.last_plus_one=
_temp1140 + 3; _temp1141;})), es); goto _LL1131; _LL1135: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1142=( char*)"--"; struct
_tagged_string _temp1143; _temp1143.curr= _temp1142; _temp1143.base= _temp1142;
_temp1143.last_plus_one= _temp1142 + 3; _temp1143;})), es); goto _LL1131;
_LL1137: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1144=( char*)"++"; struct _tagged_string _temp1145; _temp1145.curr=
_temp1144; _temp1145.base= _temp1144; _temp1145.last_plus_one= _temp1144 + 3;
_temp1145;}))); goto _LL1131; _LL1139: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1146=( char*)"--"; struct _tagged_string
_temp1147; _temp1147.curr= _temp1146; _temp1147.base= _temp1146; _temp1147.last_plus_one=
_temp1146 + 3; _temp1147;}))); goto _LL1131; _LL1131:;} goto _LL917;} _LL935:{
struct _tuple9 _temp1149=({ struct _tuple9 _temp1148; _temp1148.f1=( void*)
_temp1022->r; _temp1148.f2=( void*) _temp1020->r; _temp1148;}); _LL1151: goto
_LL1152; _LL1152: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1024), Cyc_PP_text(( struct _tagged_string)({ char* _temp1153=( char*)" ? ";
struct _tagged_string _temp1154; _temp1154.curr= _temp1153; _temp1154.base=
_temp1153; _temp1154.last_plus_one= _temp1153 + 4; _temp1154;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1022), Cyc_PP_text(( struct _tagged_string)({ char* _temp1155=( char*)" : ";
struct _tagged_string _temp1156; _temp1156.curr= _temp1155; _temp1156.base=
_temp1155; _temp1156.last_plus_one= _temp1155 + 4; _temp1156;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1020)); goto _LL1150; _LL1150:;} goto _LL917; _LL937: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1028), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1157=( char*)", "; struct _tagged_string _temp1158;
_temp1158.curr= _temp1157; _temp1158.base= _temp1157; _temp1158.last_plus_one=
_temp1157 + 3; _temp1158;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1026));
goto _LL917; _LL939: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1032), Cyc_PP_text(( struct _tagged_string)({ char* _temp1159=( char*)"(";
struct _tagged_string _temp1160; _temp1160.curr= _temp1159; _temp1160.base=
_temp1159; _temp1160.last_plus_one= _temp1159 + 2; _temp1160;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1030), Cyc_PP_text(( struct _tagged_string)({ char* _temp1161=( char*)")";
struct _tagged_string _temp1162; _temp1162.curr= _temp1161; _temp1162.base=
_temp1161; _temp1162.last_plus_one= _temp1161 + 2; _temp1162;}))); goto _LL917;
_LL941: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1036), Cyc_PP_text((
struct _tagged_string)({ char* _temp1163=( char*)"("; struct _tagged_string
_temp1164; _temp1164.curr= _temp1163; _temp1164.base= _temp1163; _temp1164.last_plus_one=
_temp1163 + 2; _temp1164;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1034), Cyc_PP_text((
struct _tagged_string)({ char* _temp1165=( char*)")"; struct _tagged_string
_temp1166; _temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 2; _temp1166;}))); goto _LL917; _LL943: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1167=( char*)"throw "; struct _tagged_string
_temp1168; _temp1168.curr= _temp1167; _temp1168.base= _temp1167; _temp1168.last_plus_one=
_temp1167 + 7; _temp1168;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1038));
goto _LL917; _LL945: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1040); goto
_LL917; _LL947: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1044); goto _LL917;
_LL949: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1169=( char*)"("; struct _tagged_string _temp1170; _temp1170.curr=
_temp1169; _temp1170.base= _temp1169; _temp1170.last_plus_one= _temp1169 + 2;
_temp1170;})), Cyc_Absynpp_typ2doc( _temp1048), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1171=( char*)")"; struct _tagged_string _temp1172;
_temp1172.curr= _temp1171; _temp1172.base= _temp1171; _temp1172.last_plus_one=
_temp1171 + 2; _temp1172;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1046));
goto _LL917; _LL951: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1173=( char*)"&"; struct _tagged_string _temp1174; _temp1174.curr=
_temp1173; _temp1174.base= _temp1173; _temp1174.last_plus_one= _temp1173 + 2;
_temp1174;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1050)); goto _LL917;
_LL953: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1175=( char*)"sizeof("; struct _tagged_string _temp1176; _temp1176.curr=
_temp1175; _temp1176.base= _temp1175; _temp1176.last_plus_one= _temp1175 + 8;
_temp1176;})), Cyc_Absynpp_typ2doc( _temp1052), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1177=( char*)")"; struct _tagged_string _temp1178;
_temp1178.curr= _temp1177; _temp1178.base= _temp1177; _temp1178.last_plus_one=
_temp1177 + 2; _temp1178;}))); goto _LL917; _LL955: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1179=( char*)"*"; struct _tagged_string
_temp1180; _temp1180.curr= _temp1179; _temp1180.base= _temp1179; _temp1180.last_plus_one=
_temp1179 + 2; _temp1180;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1054));
goto _LL917; _LL957: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1058), Cyc_PP_text(( struct _tagged_string)({ char* _temp1181=( char*)".";
struct _tagged_string _temp1182; _temp1182.curr= _temp1181; _temp1182.base=
_temp1181; _temp1182.last_plus_one= _temp1181 + 2; _temp1182;})), Cyc_Absynpp_textptr(
_temp1056)); goto _LL917; _LL959: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1062), Cyc_PP_text(( struct _tagged_string)({ char* _temp1183=(
char*)"->"; struct _tagged_string _temp1184; _temp1184.curr= _temp1183;
_temp1184.base= _temp1183; _temp1184.last_plus_one= _temp1183 + 3; _temp1184;})),
Cyc_Absynpp_textptr( _temp1060)); goto _LL917; _LL961: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1066), Cyc_PP_text(( struct _tagged_string)({ char* _temp1185=(
char*)"["; struct _tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base=
_temp1185; _temp1186.last_plus_one= _temp1185 + 2; _temp1186;})), Cyc_Absynpp_exp2doc(
_temp1064), Cyc_PP_text(( struct _tagged_string)({ char* _temp1187=( char*)"]";
struct _tagged_string _temp1188; _temp1188.curr= _temp1187; _temp1188.base=
_temp1187; _temp1188.last_plus_one= _temp1187 + 2; _temp1188;}))); goto _LL917;
_LL963: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1189=( char*)"$("; struct _tagged_string _temp1190; _temp1190.curr=
_temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one= _temp1189 + 3;
_temp1190;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1068), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1191=( char*)")"; struct _tagged_string _temp1192;
_temp1192.curr= _temp1191; _temp1192.base= _temp1191; _temp1192.last_plus_one=
_temp1191 + 2; _temp1192;}))); goto _LL917; _LL965: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1193=( char*)"("; struct _tagged_string
_temp1194; _temp1194.curr= _temp1193; _temp1194.base= _temp1193; _temp1194.last_plus_one=
_temp1193 + 2; _temp1194;})), Cyc_Absynpp_typ2doc((* _temp1072).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1195=( char*)")"; struct _tagged_string
_temp1196; _temp1196.curr= _temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one=
_temp1195 + 2; _temp1196;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1197=( char*)"{"; struct _tagged_string _temp1198; _temp1198.curr=
_temp1197; _temp1198.base= _temp1197; _temp1198.last_plus_one= _temp1197 + 2;
_temp1198;}),( struct _tagged_string)({ char* _temp1199=( char*)"}"; struct
_tagged_string _temp1200; _temp1200.curr= _temp1199; _temp1200.base= _temp1199;
_temp1200.last_plus_one= _temp1199 + 2; _temp1200;}),( struct _tagged_string)({
char* _temp1201=( char*)","; struct _tagged_string _temp1202; _temp1202.curr=
_temp1201; _temp1202.base= _temp1201; _temp1202.last_plus_one= _temp1201 + 2;
_temp1202;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1070))); goto _LL917; _LL967: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1203=( char*)"new {"; struct _tagged_string _temp1204; _temp1204.curr=
_temp1203; _temp1204.base= _temp1203; _temp1204.last_plus_one= _temp1203 + 6;
_temp1204;}),( struct _tagged_string)({ char* _temp1205=( char*)"}"; struct
_tagged_string _temp1206; _temp1206.curr= _temp1205; _temp1206.base= _temp1205;
_temp1206.last_plus_one= _temp1205 + 2; _temp1206;}),( struct _tagged_string)({
char* _temp1207=( char*)","; struct _tagged_string _temp1208; _temp1208.curr=
_temp1207; _temp1208.base= _temp1207; _temp1208.last_plus_one= _temp1207 + 2;
_temp1208;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1074)); goto _LL917; _LL969: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1209=( char*)"{"; struct _tagged_string _temp1210; _temp1210.curr=
_temp1209; _temp1210.base= _temp1209; _temp1210.last_plus_one= _temp1209 + 2;
_temp1210;}),( struct _tagged_string)({ char* _temp1211=( char*)"}"; struct
_tagged_string _temp1212; _temp1212.curr= _temp1211; _temp1212.base= _temp1211;
_temp1212.last_plus_one= _temp1211 + 2; _temp1212;}),( struct _tagged_string)({
char* _temp1213=( char*)","; struct _tagged_string _temp1214; _temp1214.curr=
_temp1213; _temp1214.base= _temp1213; _temp1214.last_plus_one= _temp1213 + 2;
_temp1214;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1078)); goto _LL917; _LL971: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1215=( char*)"new {for "; struct _tagged_string
_temp1216; _temp1216.curr= _temp1215; _temp1216.base= _temp1215; _temp1216.last_plus_one=
_temp1215 + 10; _temp1216;})), Cyc_PP_text(*(* _temp1086->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1217=( char*)" < "; struct _tagged_string
_temp1218; _temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 4; _temp1218;})), Cyc_Absynpp_exp2doc( _temp1084), Cyc_PP_text((
struct _tagged_string)({ char* _temp1219=( char*)" : "; struct _tagged_string
_temp1220; _temp1220.curr= _temp1219; _temp1220.base= _temp1219; _temp1220.last_plus_one=
_temp1219 + 4; _temp1220;})), Cyc_Absynpp_exp2doc( _temp1082), Cyc_PP_text((
struct _tagged_string)({ char* _temp1221=( char*)"}"; struct _tagged_string
_temp1222; _temp1222.curr= _temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one=
_temp1221 + 2; _temp1222;}))); goto _LL917; _LL973: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1094), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1223=( char*)"{";
struct _tagged_string _temp1224; _temp1224.curr= _temp1223; _temp1224.base=
_temp1223; _temp1224.last_plus_one= _temp1223 + 2; _temp1224;}),( struct
_tagged_string)({ char* _temp1225=( char*)"}"; struct _tagged_string _temp1226;
_temp1226.curr= _temp1225; _temp1226.base= _temp1225; _temp1226.last_plus_one=
_temp1225 + 2; _temp1226;}),( struct _tagged_string)({ char* _temp1227=( char*)",";
struct _tagged_string _temp1228; _temp1228.curr= _temp1227; _temp1228.base=
_temp1227; _temp1228.last_plus_one= _temp1227 + 2; _temp1228;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1090))); goto _LL917; _LL975: _temp1110= _temp1100;
_temp1106= _temp1096; goto _LL977; _LL977: if( _temp1110 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1106->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1106->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1229=( char*)"(";
struct _tagged_string _temp1230; _temp1230.curr= _temp1229; _temp1230.base=
_temp1229; _temp1230.last_plus_one= _temp1229 + 2; _temp1230;}),( struct
_tagged_string)({ char* _temp1231=( char*)")"; struct _tagged_string _temp1232;
_temp1232.curr= _temp1231; _temp1232.base= _temp1231; _temp1232.last_plus_one=
_temp1231 + 2; _temp1232;}),( struct _tagged_string)({ char* _temp1233=( char*)",";
struct _tagged_string _temp1234; _temp1234.curr= _temp1233; _temp1234.base=
_temp1233; _temp1234.last_plus_one= _temp1233 + 2; _temp1234;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1110)));} goto _LL917; _LL979: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1235=( char*)"{"; struct _tagged_string
_temp1236; _temp1236.curr= _temp1235; _temp1236.base= _temp1235; _temp1236.last_plus_one=
_temp1235 + 2; _temp1236;}),( struct _tagged_string)({ char* _temp1237=( char*)"}";
struct _tagged_string _temp1238; _temp1238.curr= _temp1237; _temp1238.base=
_temp1237; _temp1238.last_plus_one= _temp1237 + 2; _temp1238;}),( struct
_tagged_string)({ char* _temp1239=( char*)","; struct _tagged_string _temp1240;
_temp1240.curr= _temp1239; _temp1240.base= _temp1239; _temp1240.last_plus_one=
_temp1239 + 2; _temp1240;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1114)); goto _LL917; _LL981: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1241=( char*)"({ "; struct _tagged_string _temp1242;
_temp1242.curr= _temp1241; _temp1242.base= _temp1241; _temp1242.last_plus_one=
_temp1241 + 4; _temp1242;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1118)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1243=( char*)" })"; struct
_tagged_string _temp1244; _temp1244.curr= _temp1243; _temp1244.base= _temp1243;
_temp1244.last_plus_one= _temp1243 + 4; _temp1244;}))); goto _LL917; _LL983: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1245=( char*)"codegen(";
struct _tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base=
_temp1245; _temp1246.last_plus_one= _temp1245 + 9; _temp1246;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1247=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1247->r=( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1248=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1248->tag= Cyc_Absyn_Fn_d_tag; _temp1248->f1= _temp1120;( void*) _temp1248;});
_temp1247->loc= e->loc; _temp1247;}))), Cyc_PP_text(( struct _tagged_string)({
char* _temp1249=( char*)")"; struct _tagged_string _temp1250; _temp1250.curr=
_temp1249; _temp1250.base= _temp1249; _temp1250.last_plus_one= _temp1249 + 2;
_temp1250;}))); goto _LL917; _LL985: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1251=( char*)"fill("; struct _tagged_string
_temp1252; _temp1252.curr= _temp1251; _temp1252.base= _temp1251; _temp1252.last_plus_one=
_temp1251 + 6; _temp1252;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1122)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1253=( char*)")"; struct
_tagged_string _temp1254; _temp1254.curr= _temp1253; _temp1254.base= _temp1253;
_temp1254.last_plus_one= _temp1253 + 2; _temp1254;}))); goto _LL917; _LL917:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1255=( char*)"("; struct _tagged_string _temp1256; _temp1256.curr=
_temp1255; _temp1256.base= _temp1255; _temp1256.last_plus_one= _temp1255 + 2;
_temp1256;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1257=( char*)")";
struct _tagged_string _temp1258; _temp1258.curr= _temp1257; _temp1258.base=
_temp1257; _temp1258.last_plus_one= _temp1257 + 2; _temp1258;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1259= d;
struct Cyc_Absyn_Exp* _temp1265; struct _tagged_string* _temp1267; _LL1261: if(((
struct _enum_struct*) _temp1259)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1266:
_temp1265=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1259)->f1; goto _LL1262;} else{ goto _LL1263;} _LL1263: if((( struct
_enum_struct*) _temp1259)->tag == Cyc_Absyn_FieldName_tag){ _LL1268: _temp1267=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1259)->f1;
goto _LL1264;} else{ goto _LL1260;} _LL1262: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1269=( char*)".["; struct _tagged_string
_temp1270; _temp1270.curr= _temp1269; _temp1270.base= _temp1269; _temp1270.last_plus_one=
_temp1269 + 3; _temp1270;})), Cyc_Absynpp_exp2doc( _temp1265), Cyc_PP_text((
struct _tagged_string)({ char* _temp1271=( char*)"]"; struct _tagged_string
_temp1272; _temp1272.curr= _temp1271; _temp1272.base= _temp1271; _temp1272.last_plus_one=
_temp1271 + 2; _temp1272;}))); _LL1264: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1273=( char*)"."; struct _tagged_string
_temp1274; _temp1274.curr= _temp1273; _temp1274.base= _temp1273; _temp1274.last_plus_one=
_temp1273 + 2; _temp1274;})), Cyc_Absynpp_textptr( _temp1267)); _LL1260:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1275=( char*)""; struct _tagged_string
_temp1276; _temp1276.curr= _temp1275; _temp1276.base= _temp1275; _temp1276.last_plus_one=
_temp1275 + 1; _temp1276;}),( struct _tagged_string)({ char* _temp1277=( char*)"=";
struct _tagged_string _temp1278; _temp1278.curr= _temp1277; _temp1278.base=
_temp1277; _temp1278.last_plus_one= _temp1277 + 2; _temp1278;}),( struct
_tagged_string)({ char* _temp1279=( char*)"="; struct _tagged_string _temp1280;
_temp1280.curr= _temp1279; _temp1280.base= _temp1279; _temp1280.last_plus_one=
_temp1279 + 2; _temp1280;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1281=( char*)""; struct _tagged_string _temp1282;
_temp1282.curr= _temp1281; _temp1282.base= _temp1281; _temp1282.last_plus_one=
_temp1281 + 1; _temp1282;}),( struct _tagged_string)({ char* _temp1283=( char*)"";
struct _tagged_string _temp1284; _temp1284.curr= _temp1283; _temp1284.base=
_temp1283; _temp1284.last_plus_one= _temp1283 + 1; _temp1284;}),( struct
_tagged_string)({ char* _temp1285=( char*)","; struct _tagged_string _temp1286;
_temp1286.curr= _temp1285; _temp1286.base= _temp1285; _temp1286.last_plus_one=
_temp1285 + 2; _temp1286;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1287= c; char _temp1307; void* _temp1309; short _temp1311;
void* _temp1313; int _temp1315; void* _temp1317; int _temp1319; void* _temp1321;
long long _temp1323; void* _temp1325; struct _tagged_string _temp1327; struct
_tagged_string _temp1329; int _temp1331; struct _tagged_string _temp1333; int
_temp1335; _LL1289: if(( unsigned int) _temp1287 > 1u?(( struct _enum_struct*)
_temp1287)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1310: _temp1309=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp1287)->f1; goto _LL1308; _LL1308:
_temp1307=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1287)->f2; goto
_LL1290;} else{ goto _LL1291;} _LL1291: if(( unsigned int) _temp1287 > 1u?((
struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1314:
_temp1313=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1287)->f1; goto
_LL1312; _LL1312: _temp1311=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1287)->f2; goto _LL1292;} else{ goto _LL1293;} _LL1293: if(( unsigned int)
_temp1287 > 1u?(( struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1318: _temp1317=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1287)->f1;
if( _temp1317 == Cyc_Absyn_Signed){ goto _LL1316;} else{ goto _LL1295;} _LL1316:
_temp1315=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1287)->f2; goto _LL1294;}
else{ goto _LL1295;} _LL1295: if(( unsigned int) _temp1287 > 1u?(( struct
_enum_struct*) _temp1287)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1322: _temp1321=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1287)->f1; if( _temp1321 == Cyc_Absyn_Unsigned){
goto _LL1320;} else{ goto _LL1297;} _LL1320: _temp1319=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1287)->f2; goto _LL1296;} else{ goto _LL1297;} _LL1297: if(( unsigned int)
_temp1287 > 1u?(( struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL1326: _temp1325=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1287)->f1;
goto _LL1324; _LL1324: _temp1323=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1287)->f2; goto _LL1298;} else{ goto _LL1299;} _LL1299: if(( unsigned int)
_temp1287 > 1u?(( struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1328: _temp1327=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1287)->f1; goto _LL1300;} else{ goto _LL1301;} _LL1301: if( _temp1287 ==
Cyc_Absyn_Null_c){ goto _LL1302;} else{ goto _LL1303;} _LL1303: if((
unsigned int) _temp1287 > 1u?(( struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1332: _temp1331=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1287)->f1;
if( _temp1331 == 1){ goto _LL1330;} else{ goto _LL1305;} _LL1330: _temp1329=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1287)->f2; goto
_LL1304;} else{ goto _LL1305;} _LL1305: if(( unsigned int) _temp1287 > 1u?((
struct _enum_struct*) _temp1287)->tag == Cyc_Absyn_String_c_tag: 0){ _LL1336:
_temp1335=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1287)->f1; goto
_LL1334; _LL1334: _temp1333=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1287)->f2; goto _LL1306;} else{ goto _LL1288;} _LL1290: return Cyc_PP_text(({
struct _tagged_string _temp1337= Cyc_Absynpp_char_escape( _temp1307); xprintf("'%.*s'",
_temp1337.last_plus_one - _temp1337.curr, _temp1337.curr);})); _LL1292: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1311)); _LL1294: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1315)); _LL1296: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1319)); _LL1298: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1338=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1339; _temp1339.curr= _temp1338; _temp1339.base= _temp1338; _temp1339.last_plus_one=
_temp1338 + 27; _temp1339;})); _LL1300: return Cyc_PP_text( _temp1327); _LL1302:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1340=( char*)"null";
struct _tagged_string _temp1341; _temp1341.curr= _temp1340; _temp1341.base=
_temp1340; _temp1341.last_plus_one= _temp1340 + 5; _temp1341;})); _LL1304:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1342=(
char*)"new \""; struct _tagged_string _temp1343; _temp1343.curr= _temp1342;
_temp1343.base= _temp1342; _temp1343.last_plus_one= _temp1342 + 6; _temp1343;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1329)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1344=( char*)"\""; struct _tagged_string _temp1345;
_temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 2; _temp1345;}))); _LL1306: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1346=( char*)"\""; struct _tagged_string
_temp1347; _temp1347.curr= _temp1346; _temp1347.base= _temp1346; _temp1347.last_plus_one=
_temp1346 + 2; _temp1347;})), Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1333)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1348=( char*)"\""; struct
_tagged_string _temp1349; _temp1349.curr= _temp1348; _temp1349.base= _temp1348;
_temp1349.last_plus_one= _temp1348 + 2; _temp1349;}))); _LL1288:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == Cyc_Absyn_Size){ if( es == 0?
1: es->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1350=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1350=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=({
struct _tagged_string _temp1351= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args",
_temp1351.last_plus_one - _temp1351.curr, _temp1351.curr);})};( struct
_xenum_struct*) _temp1350;}));}{ struct Cyc_PP_Doc* d= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return Cyc_Absynpp_cat2( d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1352=( char*)".size"; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 6; _temp1353;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){
struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1354=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1354=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=({
struct _tagged_string _temp1355= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1355.last_plus_one - _temp1355.curr, _temp1355.curr);})};( struct
_xenum_struct*) _temp1354;}));} else{ if( ds->tl == 0){ return Cyc_Absynpp_cat3(
ps, Cyc_PP_text(( struct _tagged_string)({ char* _temp1356=( char*)" "; struct
_tagged_string _temp1357; _temp1357.curr= _temp1356; _temp1357.base= _temp1356;
_temp1357.last_plus_one= _temp1356 + 2; _temp1357;})),( struct Cyc_PP_Doc*) ds->hd);}
else{ if(( ds->tl)->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1358=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1358=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=({
struct _tagged_string _temp1359= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1359.last_plus_one - _temp1359.curr, _temp1359.curr);})};( struct
_xenum_struct*) _temp1358;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)
ds->hd, Cyc_PP_text(( struct _tagged_string)({ char* _temp1360=( char*)" ";
struct _tagged_string _temp1361; _temp1361.curr= _temp1360; _temp1361.base=
_temp1360; _temp1361.last_plus_one= _temp1360 + 2; _temp1361;})), ps, Cyc_PP_text((
struct _tagged_string)({ char* _temp1362=( char*)" "; struct _tagged_string
_temp1363; _temp1363.curr= _temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one=
_temp1362 + 2; _temp1363;})),( struct Cyc_PP_Doc*)( ds->tl)->hd);}}}} else{
return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1364=( char*)"("; struct _tagged_string _temp1365; _temp1365.curr=
_temp1364; _temp1365.base= _temp1364; _temp1365.last_plus_one= _temp1364 + 2;
_temp1365;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1366=( char*)")"; struct _tagged_string _temp1367;
_temp1367.curr= _temp1366; _temp1367.base= _temp1366; _temp1367.last_plus_one=
_temp1366 + 2; _temp1367;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1368= p; _LL1370: if( _temp1368 == Cyc_Absyn_Plus){ goto
_LL1371;} else{ goto _LL1372;} _LL1372: if( _temp1368 == Cyc_Absyn_Times){ goto
_LL1373;} else{ goto _LL1374;} _LL1374: if( _temp1368 == Cyc_Absyn_Minus){ goto
_LL1375;} else{ goto _LL1376;} _LL1376: if( _temp1368 == Cyc_Absyn_Div){ goto
_LL1377;} else{ goto _LL1378;} _LL1378: if( _temp1368 == Cyc_Absyn_Mod){ goto
_LL1379;} else{ goto _LL1380;} _LL1380: if( _temp1368 == Cyc_Absyn_Eq){ goto
_LL1381;} else{ goto _LL1382;} _LL1382: if( _temp1368 == Cyc_Absyn_Neq){ goto
_LL1383;} else{ goto _LL1384;} _LL1384: if( _temp1368 == Cyc_Absyn_Gt){ goto
_LL1385;} else{ goto _LL1386;} _LL1386: if( _temp1368 == Cyc_Absyn_Lt){ goto
_LL1387;} else{ goto _LL1388;} _LL1388: if( _temp1368 == Cyc_Absyn_Gte){ goto
_LL1389;} else{ goto _LL1390;} _LL1390: if( _temp1368 == Cyc_Absyn_Lte){ goto
_LL1391;} else{ goto _LL1392;} _LL1392: if( _temp1368 == Cyc_Absyn_Not){ goto
_LL1393;} else{ goto _LL1394;} _LL1394: if( _temp1368 == Cyc_Absyn_Bitnot){ goto
_LL1395;} else{ goto _LL1396;} _LL1396: if( _temp1368 == Cyc_Absyn_Bitand){ goto
_LL1397;} else{ goto _LL1398;} _LL1398: if( _temp1368 == Cyc_Absyn_Bitor){ goto
_LL1399;} else{ goto _LL1400;} _LL1400: if( _temp1368 == Cyc_Absyn_Bitxor){ goto
_LL1401;} else{ goto _LL1402;} _LL1402: if( _temp1368 == Cyc_Absyn_Bitlshift){
goto _LL1403;} else{ goto _LL1404;} _LL1404: if( _temp1368 == Cyc_Absyn_Bitlrshift){
goto _LL1405;} else{ goto _LL1406;} _LL1406: if( _temp1368 == Cyc_Absyn_Bitarshift){
goto _LL1407;} else{ goto _LL1408;} _LL1408: if( _temp1368 == Cyc_Absyn_Size){
goto _LL1409;} else{ goto _LL1410;} _LL1410: if( _temp1368 == Cyc_Absyn_Printf){
goto _LL1411;} else{ goto _LL1412;} _LL1412: if( _temp1368 == Cyc_Absyn_Fprintf){
goto _LL1413;} else{ goto _LL1414;} _LL1414: if( _temp1368 == Cyc_Absyn_Xprintf){
goto _LL1415;} else{ goto _LL1416;} _LL1416: if( _temp1368 == Cyc_Absyn_Scanf){
goto _LL1417;} else{ goto _LL1418;} _LL1418: if( _temp1368 == Cyc_Absyn_Fscanf){
goto _LL1419;} else{ goto _LL1420;} _LL1420: if( _temp1368 == Cyc_Absyn_Sscanf){
goto _LL1421;} else{ goto _LL1369;} _LL1371: return( struct _tagged_string)({
char* _temp1422=( char*)"+"; struct _tagged_string _temp1423; _temp1423.curr=
_temp1422; _temp1423.base= _temp1422; _temp1423.last_plus_one= _temp1422 + 2;
_temp1423;}); _LL1373: return( struct _tagged_string)({ char* _temp1424=( char*)"*";
struct _tagged_string _temp1425; _temp1425.curr= _temp1424; _temp1425.base=
_temp1424; _temp1425.last_plus_one= _temp1424 + 2; _temp1425;}); _LL1375: return(
struct _tagged_string)({ char* _temp1426=( char*)"-"; struct _tagged_string
_temp1427; _temp1427.curr= _temp1426; _temp1427.base= _temp1426; _temp1427.last_plus_one=
_temp1426 + 2; _temp1427;}); _LL1377: return( struct _tagged_string)({ char*
_temp1428=( char*)"/"; struct _tagged_string _temp1429; _temp1429.curr=
_temp1428; _temp1429.base= _temp1428; _temp1429.last_plus_one= _temp1428 + 2;
_temp1429;}); _LL1379: return( struct _tagged_string)({ char* _temp1430=( char*)"%";
struct _tagged_string _temp1431; _temp1431.curr= _temp1430; _temp1431.base=
_temp1430; _temp1431.last_plus_one= _temp1430 + 2; _temp1431;}); _LL1381: return(
struct _tagged_string)({ char* _temp1432=( char*)"=="; struct _tagged_string
_temp1433; _temp1433.curr= _temp1432; _temp1433.base= _temp1432; _temp1433.last_plus_one=
_temp1432 + 3; _temp1433;}); _LL1383: return( struct _tagged_string)({ char*
_temp1434=( char*)"!="; struct _tagged_string _temp1435; _temp1435.curr=
_temp1434; _temp1435.base= _temp1434; _temp1435.last_plus_one= _temp1434 + 3;
_temp1435;}); _LL1385: return( struct _tagged_string)({ char* _temp1436=( char*)">";
struct _tagged_string _temp1437; _temp1437.curr= _temp1436; _temp1437.base=
_temp1436; _temp1437.last_plus_one= _temp1436 + 2; _temp1437;}); _LL1387: return(
struct _tagged_string)({ char* _temp1438=( char*)"<"; struct _tagged_string
_temp1439; _temp1439.curr= _temp1438; _temp1439.base= _temp1438; _temp1439.last_plus_one=
_temp1438 + 2; _temp1439;}); _LL1389: return( struct _tagged_string)({ char*
_temp1440=( char*)">="; struct _tagged_string _temp1441; _temp1441.curr=
_temp1440; _temp1441.base= _temp1440; _temp1441.last_plus_one= _temp1440 + 3;
_temp1441;}); _LL1391: return( struct _tagged_string)({ char* _temp1442=( char*)"<=";
struct _tagged_string _temp1443; _temp1443.curr= _temp1442; _temp1443.base=
_temp1442; _temp1443.last_plus_one= _temp1442 + 3; _temp1443;}); _LL1393: return(
struct _tagged_string)({ char* _temp1444=( char*)"!"; struct _tagged_string
_temp1445; _temp1445.curr= _temp1444; _temp1445.base= _temp1444; _temp1445.last_plus_one=
_temp1444 + 2; _temp1445;}); _LL1395: return( struct _tagged_string)({ char*
_temp1446=( char*)"~"; struct _tagged_string _temp1447; _temp1447.curr=
_temp1446; _temp1447.base= _temp1446; _temp1447.last_plus_one= _temp1446 + 2;
_temp1447;}); _LL1397: return( struct _tagged_string)({ char* _temp1448=( char*)"&";
struct _tagged_string _temp1449; _temp1449.curr= _temp1448; _temp1449.base=
_temp1448; _temp1449.last_plus_one= _temp1448 + 2; _temp1449;}); _LL1399: return(
struct _tagged_string)({ char* _temp1450=( char*)"|"; struct _tagged_string
_temp1451; _temp1451.curr= _temp1450; _temp1451.base= _temp1450; _temp1451.last_plus_one=
_temp1450 + 2; _temp1451;}); _LL1401: return( struct _tagged_string)({ char*
_temp1452=( char*)"^"; struct _tagged_string _temp1453; _temp1453.curr=
_temp1452; _temp1453.base= _temp1452; _temp1453.last_plus_one= _temp1452 + 2;
_temp1453;}); _LL1403: return( struct _tagged_string)({ char* _temp1454=( char*)"<<";
struct _tagged_string _temp1455; _temp1455.curr= _temp1454; _temp1455.base=
_temp1454; _temp1455.last_plus_one= _temp1454 + 3; _temp1455;}); _LL1405: return(
struct _tagged_string)({ char* _temp1456=( char*)">>"; struct _tagged_string
_temp1457; _temp1457.curr= _temp1456; _temp1457.base= _temp1456; _temp1457.last_plus_one=
_temp1456 + 3; _temp1457;}); _LL1407: return( struct _tagged_string)({ char*
_temp1458=( char*)">>>"; struct _tagged_string _temp1459; _temp1459.curr=
_temp1458; _temp1459.base= _temp1458; _temp1459.last_plus_one= _temp1458 + 4;
_temp1459;}); _LL1409: return( struct _tagged_string)({ char* _temp1460=( char*)"size";
struct _tagged_string _temp1461; _temp1461.curr= _temp1460; _temp1461.base=
_temp1460; _temp1461.last_plus_one= _temp1460 + 5; _temp1461;}); _LL1411: return(
struct _tagged_string)({ char* _temp1462=( char*)"printf"; struct _tagged_string
_temp1463; _temp1463.curr= _temp1462; _temp1463.base= _temp1462; _temp1463.last_plus_one=
_temp1462 + 7; _temp1463;}); _LL1413: return( struct _tagged_string)({ char*
_temp1464=( char*)"fprintf"; struct _tagged_string _temp1465; _temp1465.curr=
_temp1464; _temp1465.base= _temp1464; _temp1465.last_plus_one= _temp1464 + 8;
_temp1465;}); _LL1415: return( struct _tagged_string)({ char* _temp1466=( char*)"xprintf";
struct _tagged_string _temp1467; _temp1467.curr= _temp1466; _temp1467.base=
_temp1466; _temp1467.last_plus_one= _temp1466 + 8; _temp1467;}); _LL1417: return(
struct _tagged_string)({ char* _temp1468=( char*)"scanf"; struct _tagged_string
_temp1469; _temp1469.curr= _temp1468; _temp1469.base= _temp1468; _temp1469.last_plus_one=
_temp1468 + 6; _temp1469;}); _LL1419: return( struct _tagged_string)({ char*
_temp1470=( char*)"fscanf"; struct _tagged_string _temp1471; _temp1471.curr=
_temp1470; _temp1471.base= _temp1470; _temp1471.last_plus_one= _temp1470 + 7;
_temp1471;}); _LL1421: return( struct _tagged_string)({ char* _temp1472=( char*)"sscanf";
struct _tagged_string _temp1473; _temp1473.curr= _temp1472; _temp1473.base=
_temp1472; _temp1473.last_plus_one= _temp1472 + 7; _temp1473;}); _LL1369:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple10* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)((* t).f1)->v), Cyc_PP_text(( struct _tagged_string)({
char* _temp1474=( char*)" = "; struct _tagged_string _temp1475; _temp1475.curr=
_temp1474; _temp1475.base= _temp1474; _temp1475.last_plus_one= _temp1474 + 4;
_temp1475;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_cat2(
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
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1476=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1482; struct Cyc_Absyn_Decl* _temp1484;
_LL1478: if(( unsigned int) _temp1476 > 1u?(( struct _enum_struct*) _temp1476)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1485: _temp1484=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1476)->f1; goto _LL1483; _LL1483:
_temp1482=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1476)->f2;
goto _LL1479;} else{ goto _LL1480;} _LL1480: goto _LL1481; _LL1479: return 1;
_LL1481: return 0; _LL1477:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1486=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1526; struct Cyc_Absyn_Stmt* _temp1528; struct Cyc_Absyn_Stmt*
_temp1530; struct Cyc_Absyn_Exp* _temp1532; struct Cyc_Absyn_Stmt* _temp1534;
struct Cyc_Absyn_Stmt* _temp1536; struct Cyc_Absyn_Exp* _temp1538; struct Cyc_Absyn_Stmt*
_temp1540; struct _tuple2 _temp1542; struct Cyc_Absyn_Stmt* _temp1544; struct
Cyc_Absyn_Exp* _temp1546; struct Cyc_Absyn_Stmt* _temp1548; struct Cyc_Absyn_Stmt*
_temp1550; struct Cyc_Absyn_Stmt* _temp1552; struct _tagged_string* _temp1554;
struct Cyc_Absyn_Stmt* _temp1556; struct _tuple2 _temp1558; struct Cyc_Absyn_Stmt*
_temp1560; struct Cyc_Absyn_Exp* _temp1562; struct _tuple2 _temp1564; struct Cyc_Absyn_Stmt*
_temp1566; struct Cyc_Absyn_Exp* _temp1568; struct Cyc_Absyn_Exp* _temp1570;
struct Cyc_List_List* _temp1572; struct Cyc_Absyn_Exp* _temp1574; struct Cyc_Absyn_Switch_clause**
_temp1576; struct Cyc_List_List* _temp1578; struct Cyc_Absyn_Switch_clause**
_temp1580; struct Cyc_List_List* _temp1582; struct Cyc_Absyn_Stmt* _temp1584;
struct Cyc_Absyn_Decl* _temp1586; struct Cyc_Absyn_Stmt* _temp1588; struct Cyc_Absyn_Stmt*
_temp1590; struct Cyc_Absyn_Stmt* _temp1592; struct _tagged_string* _temp1594;
struct _tuple2 _temp1596; struct Cyc_Absyn_Stmt* _temp1598; struct Cyc_Absyn_Exp*
_temp1600; struct Cyc_Absyn_Stmt* _temp1602; struct Cyc_List_List* _temp1604;
struct Cyc_Absyn_Stmt* _temp1606; _LL1488: if( _temp1486 == Cyc_Absyn_Skip_s){
goto _LL1489;} else{ goto _LL1490;} _LL1490: if(( unsigned int) _temp1486 > 1u?((
struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1527:
_temp1526=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1486)->f1;
goto _LL1491;} else{ goto _LL1492;} _LL1492: if(( unsigned int) _temp1486 > 1u?((
struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1531:
_temp1530=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1486)->f1;
goto _LL1529; _LL1529: _temp1528=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1486)->f2; goto _LL1493;} else{ goto _LL1494;} _LL1494: if(( unsigned int)
_temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1533: _temp1532=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1486)->f1; goto _LL1495;} else{ goto _LL1496;} _LL1496: if(( unsigned int)
_temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1539: _temp1538=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1486)->f1; goto _LL1537; _LL1537: _temp1536=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1486)->f2; goto _LL1535; _LL1535:
_temp1534=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1486)->f3; goto _LL1497;} else{ goto _LL1498;} _LL1498: if(( unsigned int)
_temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1543: _temp1542=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1486)->f1; _LL1547: _temp1546= _temp1542.f1; goto _LL1545; _LL1545:
_temp1544= _temp1542.f2; goto _LL1541; _LL1541: _temp1540=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1486)->f2; goto _LL1499;} else{ goto
_LL1500;} _LL1500: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1549: _temp1548=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1486)->f1; goto _LL1501;} else{ goto
_LL1502;} _LL1502: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1551: _temp1550=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1486)->f1; goto _LL1503;} else{ goto
_LL1504;} _LL1504: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1555: _temp1554=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1486)->f1; goto _LL1553;
_LL1553: _temp1552=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1486)->f2; goto _LL1505;} else{ goto _LL1506;} _LL1506: if(( unsigned int)
_temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1571: _temp1570=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1486)->f1; goto _LL1565; _LL1565: _temp1564=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1486)->f2; _LL1569: _temp1568= _temp1564.f1; goto _LL1567; _LL1567:
_temp1566= _temp1564.f2; goto _LL1559; _LL1559: _temp1558=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1486)->f3; _LL1563: _temp1562= _temp1558.f1;
goto _LL1561; _LL1561: _temp1560= _temp1558.f2; goto _LL1557; _LL1557: _temp1556=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1486)->f4; goto
_LL1507;} else{ goto _LL1508;} _LL1508: if(( unsigned int) _temp1486 > 1u?((
struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1575:
_temp1574=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1486)->f1; goto _LL1573; _LL1573: _temp1572=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1486)->f2; goto _LL1509;} else{ goto
_LL1510;} _LL1510: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1579: _temp1578=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1486)->f1; if( _temp1578 == 0){ goto
_LL1577;} else{ goto _LL1512;} _LL1577: _temp1576=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1486)->f2; goto _LL1511;} else{ goto
_LL1512;} _LL1512: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1583: _temp1582=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1486)->f1; goto _LL1581; _LL1581:
_temp1580=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1486)->f2; goto _LL1513;} else{ goto _LL1514;} _LL1514: if(( unsigned int)
_temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1587: _temp1586=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1486)->f1; goto _LL1585; _LL1585: _temp1584=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1486)->f2; goto _LL1515;} else{ goto
_LL1516;} _LL1516: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1589: _temp1588=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1486)->f1; goto _LL1517;} else{ goto
_LL1518;} _LL1518: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1591: _temp1590=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1486)->f1; goto _LL1519;} else{ goto
_LL1520;} _LL1520: if(( unsigned int) _temp1486 > 1u?(( struct _enum_struct*)
_temp1486)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1595: _temp1594=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1486)->f1; goto
_LL1593; _LL1593: _temp1592=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1486)->f2; goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( unsigned int)
_temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1603: _temp1602=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1486)->f1; goto _LL1597; _LL1597: _temp1596=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1486)->f2; _LL1601: _temp1600= _temp1596.f1; goto _LL1599; _LL1599:
_temp1598= _temp1596.f2; goto _LL1523;} else{ goto _LL1524;} _LL1524: if((
unsigned int) _temp1486 > 1u?(( struct _enum_struct*) _temp1486)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1607: _temp1606=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1486)->f1; goto _LL1605; _LL1605: _temp1604=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1486)->f2; goto _LL1525;} else{ goto
_LL1487;} _LL1489: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1608=(
char*)";"; struct _tagged_string _temp1609; _temp1609.curr= _temp1608; _temp1609.base=
_temp1608; _temp1609.last_plus_one= _temp1608 + 2; _temp1609;})); goto _LL1487;
_LL1491: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1526), Cyc_PP_text((
struct _tagged_string)({ char* _temp1610=( char*)";"; struct _tagged_string
_temp1611; _temp1611.curr= _temp1610; _temp1611.base= _temp1610; _temp1611.last_plus_one=
_temp1610 + 2; _temp1611;}))); goto _LL1487; _LL1493: if( Cyc_Absynpp_is_declaration(
_temp1530)){ s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1612=( char*)"{ "; struct _tagged_string _temp1613; _temp1613.curr=
_temp1612; _temp1613.base= _temp1612; _temp1613.last_plus_one= _temp1612 + 3;
_temp1613;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1530)), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1614=( char*)"}"; struct
_tagged_string _temp1615; _temp1615.curr= _temp1614; _temp1615.base= _temp1614;
_temp1615.last_plus_one= _temp1614 + 2; _temp1615;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1528)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1616=( char*)"{ "; struct _tagged_string _temp1617; _temp1617.curr=
_temp1616; _temp1617.base= _temp1616; _temp1617.last_plus_one= _temp1616 + 3;
_temp1617;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1528)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1618=( char*)"}"; struct _tagged_string
_temp1619; _temp1619.curr= _temp1618; _temp1619.base= _temp1618; _temp1619.last_plus_one=
_temp1618 + 2; _temp1619;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1528));} else{ if( Cyc_Absynpp_is_declaration( _temp1528)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1530), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1620=( char*)"{ "; struct _tagged_string _temp1621;
_temp1621.curr= _temp1620; _temp1621.base= _temp1620; _temp1621.last_plus_one=
_temp1620 + 3; _temp1621;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1528)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1622=( char*)"}"; struct
_tagged_string _temp1623; _temp1623.curr= _temp1622; _temp1623.base= _temp1622;
_temp1623.last_plus_one= _temp1622 + 2; _temp1623;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1624=( char*)""; struct _tagged_string
_temp1625; _temp1625.curr= _temp1624; _temp1625.base= _temp1624; _temp1625.last_plus_one=
_temp1624 + 1; _temp1625;}),({ struct Cyc_List_List* _temp1626=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1626->hd=( void*) _temp1530;
_temp1626->tl=({ struct Cyc_List_List* _temp1627=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1627->hd=( void*) _temp1528;
_temp1627->tl= 0; _temp1627;}); _temp1626;}));}} goto _LL1487; _LL1495: if(
_temp1532 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1628=(
char*)"return;"; struct _tagged_string _temp1629; _temp1629.curr= _temp1628;
_temp1629.base= _temp1628; _temp1629.last_plus_one= _temp1628 + 8; _temp1629;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1630=( char*)"return "; struct _tagged_string _temp1631; _temp1631.curr=
_temp1630; _temp1631.base= _temp1630; _temp1631.last_plus_one= _temp1630 + 8;
_temp1631;})), _temp1532 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1632= _temp1532; if( _temp1632 == 0){
_throw( Null_Exception);} _temp1632;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1633=( char*)";"; struct _tagged_string _temp1634; _temp1634.curr=
_temp1633; _temp1634.base= _temp1633; _temp1634.last_plus_one= _temp1633 + 2;
_temp1634;})));} goto _LL1487; _LL1497: { int print_else;{ void* _temp1635=(
void*) _temp1534->r; _LL1637: if( _temp1635 == Cyc_Absyn_Skip_s){ goto _LL1638;}
else{ goto _LL1639;} _LL1639: goto _LL1640; _LL1638: print_else= 0; goto _LL1636;
_LL1640: print_else= 1; goto _LL1636; _LL1636:;} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1641=( char*)"if ("; struct _tagged_string
_temp1642; _temp1642.curr= _temp1641; _temp1642.base= _temp1641; _temp1642.last_plus_one=
_temp1641 + 5; _temp1642;})), Cyc_Absynpp_exp2doc( _temp1538), Cyc_PP_text((
struct _tagged_string)({ char* _temp1643=( char*)") {"; struct _tagged_string
_temp1644; _temp1644.curr= _temp1643; _temp1644.base= _temp1643; _temp1644.last_plus_one=
_temp1643 + 4; _temp1644;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1536))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1645=( char*)"}"; struct _tagged_string _temp1646;
_temp1646.curr= _temp1645; _temp1646.base= _temp1645; _temp1646.last_plus_one=
_temp1645 + 2; _temp1646;})), print_else? Cyc_Absynpp_cat5( Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1647=( char*)"else {"; struct
_tagged_string _temp1648; _temp1648.curr= _temp1647; _temp1648.base= _temp1647;
_temp1648.last_plus_one= _temp1647 + 7; _temp1648;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1534))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1649=( char*)"}"; struct _tagged_string
_temp1650; _temp1650.curr= _temp1649; _temp1650.base= _temp1649; _temp1650.last_plus_one=
_temp1649 + 2; _temp1650;}))): Cyc_PP_nil_doc()); goto _LL1487;} _LL1499: s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1651=( char*)"while ("; struct
_tagged_string _temp1652; _temp1652.curr= _temp1651; _temp1652.base= _temp1651;
_temp1652.last_plus_one= _temp1651 + 8; _temp1652;})), Cyc_Absynpp_exp2doc(
_temp1546), Cyc_PP_text(( struct _tagged_string)({ char* _temp1653=( char*)") {";
struct _tagged_string _temp1654; _temp1654.curr= _temp1653; _temp1654.base=
_temp1653; _temp1654.last_plus_one= _temp1653 + 4; _temp1654;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1540))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1655=( char*)"}"; struct
_tagged_string _temp1656; _temp1656.curr= _temp1655; _temp1656.base= _temp1655;
_temp1656.last_plus_one= _temp1655 + 2; _temp1656;}))); goto _LL1487; _LL1501: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1657=( char*)"break;"; struct
_tagged_string _temp1658; _temp1658.curr= _temp1657; _temp1658.base= _temp1657;
_temp1658.last_plus_one= _temp1657 + 7; _temp1658;})); goto _LL1487; _LL1503: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1659=( char*)"continue;";
struct _tagged_string _temp1660; _temp1660.curr= _temp1659; _temp1660.base=
_temp1659; _temp1660.last_plus_one= _temp1659 + 10; _temp1660;})); goto _LL1487;
_LL1505: s= Cyc_PP_text(({ struct _tagged_string _temp1661=* _temp1554; xprintf("goto %.*s;",
_temp1661.last_plus_one - _temp1661.curr, _temp1661.curr);})); goto _LL1487;
_LL1507: s= Cyc_Absynpp_cat10( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1662=( char*)"for("; struct _tagged_string _temp1663; _temp1663.curr=
_temp1662; _temp1663.base= _temp1662; _temp1663.last_plus_one= _temp1662 + 5;
_temp1663;})), Cyc_Absynpp_exp2doc( _temp1570), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1664=( char*)"; "; struct _tagged_string _temp1665;
_temp1665.curr= _temp1664; _temp1665.base= _temp1664; _temp1665.last_plus_one=
_temp1664 + 3; _temp1665;})), Cyc_Absynpp_exp2doc( _temp1568), Cyc_PP_text((
struct _tagged_string)({ char* _temp1666=( char*)"; "; struct _tagged_string
_temp1667; _temp1667.curr= _temp1666; _temp1667.base= _temp1666; _temp1667.last_plus_one=
_temp1666 + 3; _temp1667;})), Cyc_Absynpp_exp2doc( _temp1562), Cyc_PP_text((
struct _tagged_string)({ char* _temp1668=( char*)") {"; struct _tagged_string
_temp1669; _temp1669.curr= _temp1668; _temp1669.base= _temp1668; _temp1669.last_plus_one=
_temp1668 + 4; _temp1669;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1556))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1670=( char*)"}"; struct _tagged_string _temp1671;
_temp1671.curr= _temp1670; _temp1671.base= _temp1670; _temp1671.last_plus_one=
_temp1670 + 2; _temp1671;}))); goto _LL1487; _LL1509: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1672=( char*)"switch ("; struct
_tagged_string _temp1673; _temp1673.curr= _temp1672; _temp1673.base= _temp1672;
_temp1673.last_plus_one= _temp1672 + 9; _temp1673;})), Cyc_Absynpp_exp2doc(
_temp1574), Cyc_PP_text(( struct _tagged_string)({ char* _temp1674=( char*)") {";
struct _tagged_string _temp1675; _temp1675.curr= _temp1674; _temp1675.base=
_temp1674; _temp1675.last_plus_one= _temp1674 + 4; _temp1675;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1572), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1676=( char*)"}"; struct _tagged_string
_temp1677; _temp1677.curr= _temp1676; _temp1677.base= _temp1676; _temp1677.last_plus_one=
_temp1676 + 2; _temp1677;}))); goto _LL1487; _LL1511: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1678=( char*)"fallthru;"; struct _tagged_string
_temp1679; _temp1679.curr= _temp1678; _temp1679.base= _temp1678; _temp1679.last_plus_one=
_temp1678 + 10; _temp1679;})); goto _LL1487; _LL1513: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1680=( char*)"fallthru("; struct
_tagged_string _temp1681; _temp1681.curr= _temp1680; _temp1681.base= _temp1680;
_temp1681.last_plus_one= _temp1680 + 10; _temp1681;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1582), Cyc_PP_text(( struct _tagged_string)({ char* _temp1682=( char*)");";
struct _tagged_string _temp1683; _temp1683.curr= _temp1682; _temp1683.base=
_temp1682; _temp1683.last_plus_one= _temp1682 + 3; _temp1683;}))); goto _LL1487;
_LL1515: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1586), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1584)); goto _LL1487; _LL1517: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1684=( char*)"cut "; struct
_tagged_string _temp1685; _temp1685.curr= _temp1684; _temp1685.base= _temp1684;
_temp1685.last_plus_one= _temp1684 + 5; _temp1685;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1588))); goto _LL1487; _LL1519: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1686=( char*)"splice "; struct _tagged_string
_temp1687; _temp1687.curr= _temp1686; _temp1687.base= _temp1686; _temp1687.last_plus_one=
_temp1686 + 8; _temp1687;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1590)));
goto _LL1487; _LL1521: if( Cyc_Absynpp_is_declaration( _temp1592)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_textptr( _temp1594), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1688=( char*)": {"; struct _tagged_string _temp1689; _temp1689.curr=
_temp1688; _temp1689.base= _temp1688; _temp1689.last_plus_one= _temp1688 + 4;
_temp1689;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1592)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1690=( char*)"}"; struct _tagged_string _temp1691; _temp1691.curr=
_temp1690; _temp1691.base= _temp1690; _temp1691.last_plus_one= _temp1690 + 2;
_temp1691;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1594), Cyc_PP_text((
struct _tagged_string)({ char* _temp1692=( char*)": "; struct _tagged_string
_temp1693; _temp1693.curr= _temp1692; _temp1693.base= _temp1692; _temp1693.last_plus_one=
_temp1692 + 3; _temp1693;})), Cyc_Absynpp_stmt2doc( _temp1592));} goto _LL1487;
_LL1523: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1694=( char*)"do {"; struct _tagged_string _temp1695; _temp1695.curr=
_temp1694; _temp1695.base= _temp1694; _temp1695.last_plus_one= _temp1694 + 5;
_temp1695;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1602)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1696=( char*)"} while ("; struct _tagged_string _temp1697; _temp1697.curr=
_temp1696; _temp1697.base= _temp1696; _temp1697.last_plus_one= _temp1696 + 10;
_temp1697;})), Cyc_Absynpp_exp2doc( _temp1600), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1698=( char*)");"; struct _tagged_string _temp1699;
_temp1699.curr= _temp1698; _temp1699.base= _temp1698; _temp1699.last_plus_one=
_temp1698 + 3; _temp1699;}))); goto _LL1487; _LL1525: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1700=( char*)"try"; struct _tagged_string
_temp1701; _temp1701.curr= _temp1700; _temp1701.base= _temp1700; _temp1701.last_plus_one=
_temp1700 + 4; _temp1701;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1606)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1702=( char*)"catch {"; struct _tagged_string _temp1703; _temp1703.curr=
_temp1702; _temp1703.base= _temp1702; _temp1703.last_plus_one= _temp1702 + 8;
_temp1703;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1604)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1704=( char*)"}"; struct _tagged_string _temp1705; _temp1705.curr=
_temp1704; _temp1705.base= _temp1704; _temp1705.last_plus_one= _temp1704 + 2;
_temp1705;}))); goto _LL1487; _LL1487:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1706=( void*) p->r;
int _temp1738; void* _temp1740; char _temp1742; struct _tagged_string _temp1744;
struct Cyc_Absyn_Vardecl* _temp1746; struct Cyc_List_List* _temp1748; struct Cyc_Absyn_Pat*
_temp1750; struct Cyc_Absyn_Vardecl* _temp1752; struct _tuple0* _temp1754;
struct Cyc_List_List* _temp1756; struct Cyc_List_List* _temp1758; struct _tuple0*
_temp1760; struct Cyc_List_List* _temp1762; struct Cyc_List_List* _temp1764;
struct _tuple0* _temp1766; struct Cyc_List_List* _temp1768; struct Cyc_List_List*
_temp1770; struct Cyc_Core_Opt* _temp1772; struct Cyc_Absyn_Structdecl*
_temp1774; struct Cyc_Absyn_Enumfield* _temp1776; struct Cyc_Absyn_Enumdecl*
_temp1778; struct Cyc_List_List* _temp1780; struct Cyc_List_List* _temp1782;
struct Cyc_Core_Opt* _temp1784; struct _tuple0* _temp1786; struct Cyc_Absyn_Enumfield*
_temp1788; struct Cyc_Absyn_Xenumdecl* _temp1790; struct Cyc_List_List*
_temp1792; struct Cyc_List_List* _temp1794; struct _tuple0* _temp1796; _LL1708:
if( _temp1706 == Cyc_Absyn_Wild_p){ goto _LL1709;} else{ goto _LL1710;} _LL1710:
if( _temp1706 == Cyc_Absyn_Null_p){ goto _LL1711;} else{ goto _LL1712;} _LL1712:
if(( unsigned int) _temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag ==
Cyc_Absyn_Int_p_tag: 0){ _LL1741: _temp1740=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1706)->f1; goto _LL1739; _LL1739: _temp1738=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1706)->f2; goto _LL1713;} else{ goto _LL1714;} _LL1714: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1743: _temp1742=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1706)->f1;
goto _LL1715;} else{ goto _LL1716;} _LL1716: if(( unsigned int) _temp1706 > 2u?((
struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1745:
_temp1744=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1706)->f1;
goto _LL1717;} else{ goto _LL1718;} _LL1718: if(( unsigned int) _temp1706 > 2u?((
struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1747:
_temp1746=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1706)->f1; goto _LL1719;} else{ goto _LL1720;} _LL1720: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1749: _temp1748=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1706)->f1; goto _LL1721;} else{ goto _LL1722;} _LL1722: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1751: _temp1750=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1706)->f1; goto _LL1723;} else{ goto _LL1724;} _LL1724: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1753: _temp1752=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1706)->f1; goto _LL1725;} else{ goto _LL1726;} _LL1726: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1755: _temp1754=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1706)->f1; goto _LL1727;} else{ goto _LL1728;} _LL1728: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1761: _temp1760=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1706)->f1; goto _LL1759; _LL1759: _temp1758=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1706)->f2; goto _LL1757; _LL1757:
_temp1756=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1706)->f3; goto _LL1729;} else{ goto _LL1730;} _LL1730: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1767: _temp1766=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1706)->f1; goto _LL1765; _LL1765: _temp1764=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1706)->f2; goto _LL1763; _LL1763:
_temp1762=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1706)->f3; goto _LL1731;} else{ goto _LL1732;} _LL1732: if(( unsigned int)
_temp1706 > 2u?(( struct _enum_struct*) _temp1706)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1775: _temp1774=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1706)->f1; goto _LL1773; _LL1773: _temp1772=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1706)->f2; goto _LL1771; _LL1771:
_temp1770=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1706)->f3; goto _LL1769; _LL1769: _temp1768=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1706)->f4; goto _LL1733;} else{ goto
_LL1734;} _LL1734: if(( unsigned int) _temp1706 > 2u?(( struct _enum_struct*)
_temp1706)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1787: _temp1786=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1706)->f1; goto _LL1785; _LL1785:
_temp1784=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1706)->f2;
goto _LL1783; _LL1783: _temp1782=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1706)->f3; goto _LL1781; _LL1781: _temp1780=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1706)->f4; goto _LL1779; _LL1779:
_temp1778=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1706)->f5; goto _LL1777; _LL1777: _temp1776=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1706)->f6; goto _LL1735;} else{ goto
_LL1736;} _LL1736: if(( unsigned int) _temp1706 > 2u?(( struct _enum_struct*)
_temp1706)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1797: _temp1796=( struct
_tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1706)->f1; goto _LL1795;
_LL1795: _temp1794=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1706)->f2; goto _LL1793; _LL1793: _temp1792=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1706)->f3; goto _LL1791; _LL1791:
_temp1790=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1706)->f4; goto _LL1789; _LL1789: _temp1788=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1706)->f5; goto _LL1737;} else{ goto
_LL1707;} _LL1709: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1798=(
char*)"_"; struct _tagged_string _temp1799; _temp1799.curr= _temp1798; _temp1799.base=
_temp1798; _temp1799.last_plus_one= _temp1798 + 2; _temp1799;})); goto _LL1707;
_LL1711: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1800=( char*)"null";
struct _tagged_string _temp1801; _temp1801.curr= _temp1800; _temp1801.base=
_temp1800; _temp1801.last_plus_one= _temp1800 + 5; _temp1801;})); goto _LL1707;
_LL1713: if( _temp1740 == Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1738));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1738));}
goto _LL1707; _LL1715: s= Cyc_PP_text(({ struct _tagged_string _temp1802= Cyc_Absynpp_char_escape(
_temp1742); xprintf("'%.*s'", _temp1802.last_plus_one - _temp1802.curr,
_temp1802.curr);})); goto _LL1707; _LL1717: s= Cyc_PP_text( _temp1744); goto
_LL1707; _LL1719: s= Cyc_Absynpp_qvar2doc( _temp1746->name); goto _LL1707;
_LL1721: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1803=( char*)"$("; struct _tagged_string _temp1804; _temp1804.curr=
_temp1803; _temp1804.base= _temp1803; _temp1804.last_plus_one= _temp1803 + 3;
_temp1804;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1805=( char*)","; struct _tagged_string
_temp1806; _temp1806.curr= _temp1805; _temp1806.base= _temp1805; _temp1806.last_plus_one=
_temp1805 + 2; _temp1806;}), _temp1748), Cyc_PP_text(( struct _tagged_string)({
char* _temp1807=( char*)")"; struct _tagged_string _temp1808; _temp1808.curr=
_temp1807; _temp1808.base= _temp1807; _temp1808.last_plus_one= _temp1807 + 2;
_temp1808;}))); goto _LL1707; _LL1723: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1809=( char*)"&"; struct _tagged_string _temp1810;
_temp1810.curr= _temp1809; _temp1810.base= _temp1809; _temp1810.last_plus_one=
_temp1809 + 2; _temp1810;})), Cyc_Absynpp_pat2doc( _temp1750)); goto _LL1707;
_LL1725: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1811=( char*)"*"; struct _tagged_string _temp1812; _temp1812.curr=
_temp1811; _temp1812.base= _temp1811; _temp1812.last_plus_one= _temp1811 + 2;
_temp1812;})), Cyc_Absynpp_qvar2doc( _temp1752->name)); goto _LL1707; _LL1727: s=
Cyc_Absynpp_qvar2doc( _temp1754); goto _LL1707; _LL1729: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1760), Cyc_Absynpp_tvars2doc( _temp1758), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1813=( char*)"("; struct _tagged_string
_temp1814; _temp1814.curr= _temp1813; _temp1814.base= _temp1813; _temp1814.last_plus_one=
_temp1813 + 2; _temp1814;}),( struct _tagged_string)({ char* _temp1815=( char*)")";
struct _tagged_string _temp1816; _temp1816.curr= _temp1815; _temp1816.base=
_temp1815; _temp1816.last_plus_one= _temp1815 + 2; _temp1816;}),( struct
_tagged_string)({ char* _temp1817=( char*)","; struct _tagged_string _temp1818;
_temp1818.curr= _temp1817; _temp1818.base= _temp1817; _temp1818.last_plus_one=
_temp1817 + 2; _temp1818;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1756))); goto _LL1707; _LL1731: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1766), Cyc_Absynpp_tvars2doc( _temp1764), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1819=( char*)"{"; struct _tagged_string _temp1820;
_temp1820.curr= _temp1819; _temp1820.base= _temp1819; _temp1820.last_plus_one=
_temp1819 + 2; _temp1820;}),( struct _tagged_string)({ char* _temp1821=( char*)"}";
struct _tagged_string _temp1822; _temp1822.curr= _temp1821; _temp1822.base=
_temp1821; _temp1822.last_plus_one= _temp1821 + 2; _temp1822;}),( struct
_tagged_string)({ char* _temp1823=( char*)","; struct _tagged_string _temp1824;
_temp1824.curr= _temp1823; _temp1824.base= _temp1823; _temp1824.last_plus_one=
_temp1823 + 2; _temp1824;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1762))); goto _LL1707; _LL1733: s= Cyc_Absynpp_cat3( _temp1774->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1774->name)->v),
Cyc_Absynpp_tvars2doc( _temp1770), Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1825=( char*)"{"; struct _tagged_string _temp1826; _temp1826.curr=
_temp1825; _temp1826.base= _temp1825; _temp1826.last_plus_one= _temp1825 + 2;
_temp1826;}),( struct _tagged_string)({ char* _temp1827=( char*)"}"; struct
_tagged_string _temp1828; _temp1828.curr= _temp1827; _temp1828.base= _temp1827;
_temp1828.last_plus_one= _temp1827 + 2; _temp1828;}),( struct _tagged_string)({
char* _temp1829=( char*)","; struct _tagged_string _temp1830; _temp1830.curr=
_temp1829; _temp1830.base= _temp1829; _temp1830.last_plus_one= _temp1829 + 2;
_temp1830;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1768))); goto _LL1707; _LL1735: _temp1796= _temp1786; _temp1794= _temp1782;
_temp1792= _temp1780; goto _LL1737; _LL1737: if( _temp1792 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1796);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1796), Cyc_Absynpp_tvars2doc(
_temp1794), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1831=( char*)"(";
struct _tagged_string _temp1832; _temp1832.curr= _temp1831; _temp1832.base=
_temp1831; _temp1832.last_plus_one= _temp1831 + 2; _temp1832;}),( struct
_tagged_string)({ char* _temp1833=( char*)")"; struct _tagged_string _temp1834;
_temp1834.curr= _temp1833; _temp1834.base= _temp1833; _temp1834.last_plus_one=
_temp1833 + 2; _temp1834;}),( struct _tagged_string)({ char* _temp1835=( char*)",";
struct _tagged_string _temp1836; _temp1836.curr= _temp1835; _temp1836.base=
_temp1835; _temp1836.last_plus_one= _temp1835 + 2; _temp1836;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1792)));} goto _LL1707; _LL1707:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1837=( char*)""; struct
_tagged_string _temp1838; _temp1838.curr= _temp1837; _temp1838.base= _temp1837;
_temp1838.last_plus_one= _temp1837 + 1; _temp1838;}),( struct _tagged_string)({
char* _temp1839=( char*)"="; struct _tagged_string _temp1840; _temp1840.curr=
_temp1839; _temp1840.base= _temp1839; _temp1840.last_plus_one= _temp1839 + 2;
_temp1840;}),( struct _tagged_string)({ char* _temp1841=( char*)"="; struct
_tagged_string _temp1842; _temp1842.curr= _temp1841; _temp1842.base= _temp1841;
_temp1842.last_plus_one= _temp1841 + 2; _temp1842;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
== Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1843=( char*)"default: "; struct _tagged_string
_temp1844; _temp1844.curr= _temp1843; _temp1844.base= _temp1843; _temp1844.last_plus_one=
_temp1843 + 10; _temp1844;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1845=( char*)"case "; struct
_tagged_string _temp1846; _temp1846.curr= _temp1845; _temp1846.base= _temp1845;
_temp1846.last_plus_one= _temp1845 + 6; _temp1846;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1847=( char*)": "; struct
_tagged_string _temp1848; _temp1848.curr= _temp1847; _temp1848.base= _temp1847;
_temp1848.last_plus_one= _temp1847 + 3; _temp1848;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1849=( char*)"case "; struct
_tagged_string _temp1850; _temp1850.curr= _temp1849; _temp1850.base= _temp1849;
_temp1850.last_plus_one= _temp1849 + 6; _temp1850;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1851=( char*)" && "; struct
_tagged_string _temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851;
_temp1852.last_plus_one= _temp1851 + 5; _temp1852;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1853= c->where_clause; if(
_temp1853 == 0){ _throw( Null_Exception);} _temp1853;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1854=( char*)": "; struct _tagged_string _temp1855;
_temp1855.curr= _temp1854; _temp1855.base= _temp1854; _temp1855.last_plus_one=
_temp1854 + 3; _temp1855;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp1856=( char*)""; struct _tagged_string _temp1857; _temp1857.curr=
_temp1856; _temp1857.base= _temp1856; _temp1857.last_plus_one= _temp1856 + 1;
_temp1857;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl*
d){ struct Cyc_PP_Doc* s;{ void* _temp1858=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1882; struct Cyc_Absyn_Structdecl* _temp1884; struct Cyc_Absyn_Vardecl*
_temp1886; struct Cyc_Absyn_Vardecl _temp1888; struct Cyc_List_List* _temp1889;
struct Cyc_Core_Opt* _temp1891; int _temp1893; struct Cyc_Absyn_Exp* _temp1895;
void* _temp1897; struct Cyc_Absyn_Tqual* _temp1899; struct _tuple0* _temp1901;
void* _temp1903; struct Cyc_Absyn_Enumdecl* _temp1905; struct Cyc_Absyn_Xenumdecl*
_temp1907; int _temp1909; struct Cyc_Absyn_Exp* _temp1911; struct Cyc_Core_Opt*
_temp1913; struct Cyc_Core_Opt* _temp1915; struct Cyc_Absyn_Pat* _temp1917;
struct Cyc_Absyn_Typedefdecl* _temp1919; struct Cyc_List_List* _temp1921; struct
_tagged_string* _temp1923; struct Cyc_List_List* _temp1925; struct _tuple0*
_temp1927; struct Cyc_List_List* _temp1929; _LL1860: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Fn_d_tag: 0){
_LL1883: _temp1882=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1858)->f1; goto _LL1861;} else{ goto _LL1862;} _LL1862: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL1885: _temp1884=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp1858)->f1; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Var_d_tag:
0){ _LL1887: _temp1886=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1858)->f1; _temp1888=* _temp1886; _LL1904: _temp1903=( void*) _temp1888.sc;
goto _LL1902; _LL1902: _temp1901=( struct _tuple0*) _temp1888.name; goto _LL1900;
_LL1900: _temp1899=( struct Cyc_Absyn_Tqual*) _temp1888.tq; goto _LL1898;
_LL1898: _temp1897=( void*) _temp1888.type; goto _LL1896; _LL1896: _temp1895=(
struct Cyc_Absyn_Exp*) _temp1888.initializer; goto _LL1894; _LL1894: _temp1893=(
int) _temp1888.shadow; goto _LL1892; _LL1892: _temp1891=( struct Cyc_Core_Opt*)
_temp1888.region; goto _LL1890; _LL1890: _temp1889=( struct Cyc_List_List*)
_temp1888.attributes; goto _LL1865;} else{ goto _LL1866;} _LL1866: if( _temp1858
== Cyc_Absyn_Union_d){ goto _LL1867;} else{ goto _LL1868;} _LL1868: if((
unsigned int) _temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Enum_d_tag:
0){ _LL1906: _temp1905=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp1858)->f1; goto _LL1869;} else{ goto _LL1870;} _LL1870: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL1908: _temp1907=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp1858)->f1; goto _LL1871;} else{ goto _LL1872;} _LL1872: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Let_d_tag:
0){ _LL1918: _temp1917=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1858)->f1; goto _LL1916; _LL1916: _temp1915=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1858)->f2; goto _LL1914; _LL1914: _temp1913=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1858)->f3; goto
_LL1912; _LL1912: _temp1911=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1858)->f4; goto _LL1910; _LL1910: _temp1909=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1858)->f5; goto _LL1873;} else{ goto _LL1874;} _LL1874: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Typedef_d_tag:
0){ _LL1920: _temp1919=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1858)->f1; goto _LL1875;} else{ goto _LL1876;} _LL1876: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_Namespace_d_tag:
0){ _LL1924: _temp1923=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1858)->f1; goto _LL1922; _LL1922: _temp1921=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1858)->f2; goto _LL1877;} else{ goto
_LL1878;} _LL1878: if(( unsigned int) _temp1858 > 1u?(( struct _enum_struct*)
_temp1858)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL1928: _temp1927=( struct
_tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1858)->f1; goto _LL1926;
_LL1926: _temp1925=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1858)->f2; goto _LL1879;} else{ goto _LL1880;} _LL1880: if(( unsigned int)
_temp1858 > 1u?(( struct _enum_struct*) _temp1858)->tag == Cyc_Absyn_ExternC_d_tag:
0){ _LL1930: _temp1929=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1858)->f1; goto _LL1881;} else{ goto _LL1859;} _LL1861: { void* t=({ struct
Cyc_Absyn_FnType_struct* _temp1931=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1931->tag= Cyc_Absyn_FnType_tag;
_temp1931->f1=({ struct Cyc_Absyn_FnInfo _temp1932; _temp1932.tvars= _temp1882->tvs;
_temp1932.effect= _temp1882->effect; _temp1932.ret_typ=( void*)(( void*)
_temp1882->ret_type); _temp1932.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1882->args); _temp1932.varargs= _temp1882->varargs; _temp1932.attributes= 0;
_temp1932;});( void*) _temp1931;}); s= Cyc_Absynpp_cat8( Cyc_PP_text( _temp1882->is_inline?(
struct _tagged_string)({ char* _temp1933=( char*)"inline "; struct
_tagged_string _temp1934; _temp1934.curr= _temp1933; _temp1934.base= _temp1933;
_temp1934.last_plus_one= _temp1933 + 8; _temp1934;}):( struct _tagged_string)({
char* _temp1935=( char*)""; struct _tagged_string _temp1936; _temp1936.curr=
_temp1935; _temp1936.base= _temp1935; _temp1936.last_plus_one= _temp1935 + 1;
_temp1936;})), Cyc_Absynpp_scope2doc(( void*) _temp1882->sc), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp1937=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1937->q_const= 0; _temp1937->q_volatile=
0; _temp1937->q_restrict= 0; _temp1937;}), t,({ struct Cyc_Core_Opt* _temp1938=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1938->v=(
void*) Cyc_Absynpp_qvar2doc( _temp1882->name); _temp1938;})), Cyc_Absynpp_atts2doc(
_temp1882->attributes), Cyc_PP_text(( struct _tagged_string)({ char* _temp1939=(
char*)" {"; struct _tagged_string _temp1940; _temp1940.curr= _temp1939;
_temp1940.base= _temp1939; _temp1940.last_plus_one= _temp1939 + 3; _temp1940;})),
Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp1882->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp1941=( char*)"}"; struct _tagged_string _temp1942; _temp1942.curr=
_temp1941; _temp1942.base= _temp1941; _temp1942.last_plus_one= _temp1941 + 2;
_temp1942;}))); goto _LL1859;} _LL1863: if( _temp1884->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp1884->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1943=( char*)"struct "; struct _tagged_string
_temp1944; _temp1944.curr= _temp1943; _temp1944.base= _temp1943; _temp1944.last_plus_one=
_temp1943 + 8; _temp1944;})), _temp1884->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp1945=( char*)""; struct _tagged_string _temp1946;
_temp1946.curr= _temp1945; _temp1946.base= _temp1945; _temp1946.last_plus_one=
_temp1945 + 1; _temp1946;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1884->name)->v),
Cyc_Absynpp_tvars2doc( _temp1884->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp1947=( char*)";"; struct _tagged_string _temp1948; _temp1948.curr=
_temp1947; _temp1948.base= _temp1947; _temp1948.last_plus_one= _temp1947 + 2;
_temp1948;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc(( void*)
_temp1884->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp1949=( char*)"struct ";
struct _tagged_string _temp1950; _temp1950.curr= _temp1949; _temp1950.base=
_temp1949; _temp1950.last_plus_one= _temp1949 + 8; _temp1950;})), _temp1884->name
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1951=( char*)""; struct
_tagged_string _temp1952; _temp1952.curr= _temp1951; _temp1952.base= _temp1951;
_temp1952.last_plus_one= _temp1951 + 1; _temp1952;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1884->name)->v), Cyc_Absynpp_tvars2doc( _temp1884->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1953=( char*)" {"; struct
_tagged_string _temp1954; _temp1954.curr= _temp1953; _temp1954.base= _temp1953;
_temp1954.last_plus_one= _temp1953 + 3; _temp1954;})), Cyc_PP_nest( 2, _temp1884->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1955=( char*)""; struct
_tagged_string _temp1956; _temp1956.curr= _temp1955; _temp1956.base= _temp1955;
_temp1956.last_plus_one= _temp1955 + 1; _temp1956;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)( _temp1884->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1957=( char*)"}";
struct _tagged_string _temp1958; _temp1958.curr= _temp1957; _temp1958.base=
_temp1957; _temp1958.last_plus_one= _temp1957 + 2; _temp1958;})), Cyc_Absynpp_atts2doc(
_temp1884->attributes), Cyc_PP_text(( struct _tagged_string)({ char* _temp1959=(
char*)";"; struct _tagged_string _temp1960; _temp1960.curr= _temp1959; _temp1960.base=
_temp1959; _temp1960.last_plus_one= _temp1959 + 2; _temp1960;})));} goto _LL1859;
_LL1865: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_qvar2doc( _temp1901); if(
_temp1893 > 0){ sn= Cyc_Absynpp_cat2( sn, Cyc_PP_text( xprintf("%d", _temp1893)));}
s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc( _temp1903), Cyc_Absynpp_tqtd2doc(
_temp1899, _temp1897,({ struct Cyc_Core_Opt* _temp1961=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1961->v=( void*) sn; _temp1961;})),
Cyc_Absynpp_atts2doc( _temp1889), _temp1895 == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp1962=( char*)""; struct _tagged_string _temp1963;
_temp1963.curr= _temp1962; _temp1963.base= _temp1962; _temp1963.last_plus_one=
_temp1962 + 1; _temp1963;})): Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1964=( char*)" = "; struct _tagged_string _temp1965;
_temp1965.curr= _temp1964; _temp1965.base= _temp1964; _temp1965.last_plus_one=
_temp1964 + 4; _temp1965;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1966= _temp1895; if( _temp1966 == 0){ _throw(
Null_Exception);} _temp1966;}))), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1967=( char*)";"; struct _tagged_string _temp1968; _temp1968.curr=
_temp1967; _temp1968.base= _temp1967; _temp1968.last_plus_one= _temp1967 + 2;
_temp1968;}))); goto _LL1859;} _LL1867: s= Cyc_PP_text(( struct _tagged_string)({
char* _temp1969=( char*)"**UnionDecl**"; struct _tagged_string _temp1970;
_temp1970.curr= _temp1969; _temp1970.base= _temp1969; _temp1970.last_plus_one=
_temp1969 + 14; _temp1970;})); goto _LL1859; _LL1869: if( _temp1905->fields == 0){
s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp1905->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp1971=( char*)"enum "; struct _tagged_string
_temp1972; _temp1972.curr= _temp1971; _temp1972.base= _temp1971; _temp1972.last_plus_one=
_temp1971 + 6; _temp1972;})), _temp1905->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp1973=( char*)""; struct _tagged_string _temp1974;
_temp1974.curr= _temp1973; _temp1974.base= _temp1973; _temp1974.last_plus_one=
_temp1973 + 1; _temp1974;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1905->name)->v),
Cyc_Absynpp_tvars2doc( _temp1905->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp1975=( char*)";"; struct _tagged_string _temp1976; _temp1976.curr=
_temp1975; _temp1976.base= _temp1975; _temp1976.last_plus_one= _temp1975 + 2;
_temp1976;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc(( void*)
_temp1905->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp1977=( char*)"enum ";
struct _tagged_string _temp1978; _temp1978.curr= _temp1977; _temp1978.base=
_temp1977; _temp1978.last_plus_one= _temp1977 + 6; _temp1978;})), _temp1905->name
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1979=( char*)""; struct
_tagged_string _temp1980; _temp1980.curr= _temp1979; _temp1980.base= _temp1979;
_temp1980.last_plus_one= _temp1979 + 1; _temp1980;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1905->name)->v), Cyc_Absynpp_tvars2doc( _temp1905->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1981=( char*)" {"; struct
_tagged_string _temp1982; _temp1982.curr= _temp1981; _temp1982.base= _temp1981;
_temp1982.last_plus_one= _temp1981 + 3; _temp1982;})), Cyc_PP_nest( 2, _temp1905->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1983=( char*)""; struct
_tagged_string _temp1984; _temp1984.curr= _temp1983; _temp1984.base= _temp1983;
_temp1984.last_plus_one= _temp1983 + 1; _temp1984;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)( _temp1905->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1985=( char*)"};";
struct _tagged_string _temp1986; _temp1986.curr= _temp1985; _temp1986.base=
_temp1985; _temp1986.last_plus_one= _temp1985 + 3; _temp1986;})));} goto _LL1859;
_LL1871: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) _temp1907->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp1987=( char*)"xenum "; struct _tagged_string
_temp1988; _temp1988.curr= _temp1987; _temp1988.base= _temp1987; _temp1988.last_plus_one=
_temp1987 + 7; _temp1988;})), Cyc_Absynpp_qvar2doc( _temp1907->name), _temp1907->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1989=( char*)";"; struct
_tagged_string _temp1990; _temp1990.curr= _temp1989; _temp1990.base= _temp1989;
_temp1990.last_plus_one= _temp1989 + 2; _temp1990;})): Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1991=( char*)" {"; struct _tagged_string
_temp1992; _temp1992.curr= _temp1991; _temp1992.base= _temp1991; _temp1992.last_plus_one=
_temp1991 + 3; _temp1992;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc( _temp1907->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1993=( char*)"};"; struct _tagged_string
_temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993; _temp1994.last_plus_one=
_temp1993 + 3; _temp1994;})))); goto _LL1859; _LL1873: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp1995=( char*)"let "; struct _tagged_string
_temp1996; _temp1996.curr= _temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one=
_temp1995 + 5; _temp1996;})), Cyc_Absynpp_pat2doc( _temp1917), Cyc_PP_text((
struct _tagged_string)({ char* _temp1997=( char*)" = "; struct _tagged_string
_temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one=
_temp1997 + 4; _temp1998;})), Cyc_Absynpp_exp2doc( _temp1911), Cyc_PP_text((
struct _tagged_string)({ char* _temp1999=( char*)";"; struct _tagged_string
_temp2000; _temp2000.curr= _temp1999; _temp2000.base= _temp1999; _temp2000.last_plus_one=
_temp1999 + 2; _temp2000;}))); goto _LL1859; _LL1875: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp2001=( char*)"typedef "; struct
_tagged_string _temp2002; _temp2002.curr= _temp2001; _temp2002.base= _temp2001;
_temp2002.last_plus_one= _temp2001 + 9; _temp2002;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp2003=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2003->q_const= 0; _temp2003->q_volatile=
0; _temp2003->q_restrict= 0; _temp2003;}),( void*) _temp1919->defn,({ struct Cyc_Core_Opt*
_temp2004=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2004->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1919->name),
Cyc_Absynpp_tvars2doc( _temp1919->tvs)); _temp2004;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2005=( char*)";"; struct _tagged_string _temp2006;
_temp2006.curr= _temp2005; _temp2006.base= _temp2005; _temp2006.last_plus_one=
_temp2005 + 2; _temp2006;}))); goto _LL1859; _LL1877: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2007=( char*)"namespace "; struct
_tagged_string _temp2008; _temp2008.curr= _temp2007; _temp2008.base= _temp2007;
_temp2008.last_plus_one= _temp2007 + 11; _temp2008;})), Cyc_Absynpp_textptr(
_temp1923), Cyc_PP_text(( struct _tagged_string)({ char* _temp2009=( char*)" {";
struct _tagged_string _temp2010; _temp2010.curr= _temp2009; _temp2010.base=
_temp2009; _temp2010.last_plus_one= _temp2009 + 3; _temp2010;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2011=( char*)""; struct _tagged_string
_temp2012; _temp2012.curr= _temp2011; _temp2012.base= _temp2011; _temp2012.last_plus_one=
_temp2011 + 1; _temp2012;}), _temp1921), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2013=( char*)"}"; struct _tagged_string _temp2014;
_temp2014.curr= _temp2013; _temp2014.base= _temp2013; _temp2014.last_plus_one=
_temp2013 + 2; _temp2014;}))); goto _LL1859; _LL1879: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp2015=( char*)"using "; struct _tagged_string
_temp2016; _temp2016.curr= _temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one=
_temp2015 + 7; _temp2016;})), Cyc_Absynpp_qvar2doc( _temp1927), Cyc_PP_text((
struct _tagged_string)({ char* _temp2017=( char*)" {"; struct _tagged_string
_temp2018; _temp2018.curr= _temp2017; _temp2018.base= _temp2017; _temp2018.last_plus_one=
_temp2017 + 3; _temp2018;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2019=( char*)""; struct _tagged_string _temp2020; _temp2020.curr=
_temp2019; _temp2020.base= _temp2019; _temp2020.last_plus_one= _temp2019 + 1;
_temp2020;}), _temp1925), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2021=( char*)"}"; struct _tagged_string _temp2022; _temp2022.curr=
_temp2021; _temp2022.base= _temp2021; _temp2022.last_plus_one= _temp2021 + 2;
_temp2022;}))); goto _LL1859; _LL1881: s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2023=( char*)"extern \"C\" {"; struct
_tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base= _temp2023;
_temp2024.last_plus_one= _temp2023 + 13; _temp2024;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2025=( char*)""; struct _tagged_string
_temp2026; _temp2026.curr= _temp2025; _temp2026.base= _temp2025; _temp2026.last_plus_one=
_temp2025 + 1; _temp2026;}), _temp1929), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2027=( char*)"}"; struct _tagged_string _temp2028;
_temp2028.curr= _temp2027; _temp2028.base= _temp2027; _temp2028.last_plus_one=
_temp2027 + 2; _temp2028;}))); goto _LL1859; _LL1859:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2029= sc; _LL2031: if( _temp2029 ==
Cyc_Absyn_Static){ goto _LL2032;} else{ goto _LL2033;} _LL2033: if( _temp2029 ==
Cyc_Absyn_Public){ goto _LL2034;} else{ goto _LL2035;} _LL2035: if( _temp2029 ==
Cyc_Absyn_Extern){ goto _LL2036;} else{ goto _LL2037;} _LL2037: if( _temp2029 ==
Cyc_Absyn_ExternC){ goto _LL2038;} else{ goto _LL2039;} _LL2039: if( _temp2029
== Cyc_Absyn_Abstract){ goto _LL2040;} else{ goto _LL2030;} _LL2032: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2041=( char*)"static "; struct
_tagged_string _temp2042; _temp2042.curr= _temp2041; _temp2042.base= _temp2041;
_temp2042.last_plus_one= _temp2041 + 8; _temp2042;})); _LL2034: return Cyc_PP_nil_doc();
_LL2036: return Cyc_PP_text(( struct _tagged_string)({ char* _temp2043=( char*)"extern ";
struct _tagged_string _temp2044; _temp2044.curr= _temp2043; _temp2044.base=
_temp2043; _temp2044.last_plus_one= _temp2043 + 8; _temp2044;})); _LL2038:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2045=( char*)"extern \"C\"";
struct _tagged_string _temp2046; _temp2046.curr= _temp2045; _temp2046.base=
_temp2045; _temp2046.last_plus_one= _temp2045 + 11; _temp2046;})); _LL2040:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2047=( char*)"abstract ";
struct _tagged_string _temp2048; _temp2048.curr= _temp2047; _temp2048.base=
_temp2047; _temp2048.last_plus_one= _temp2047 + 10; _temp2048;})); _LL2030:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual* tq, void* t){ void*
_temp2049= t; struct Cyc_Absyn_Exp* _temp2063; struct Cyc_Absyn_Tqual* _temp2065;
void* _temp2067; struct Cyc_Absyn_PtrInfo _temp2069; struct Cyc_Absyn_Conref*
_temp2071; struct Cyc_Absyn_Tqual* _temp2073; struct Cyc_Absyn_Conref* _temp2075;
void* _temp2077; void* _temp2079; struct Cyc_Absyn_FnInfo _temp2081; struct Cyc_List_List*
_temp2083; int _temp2085; struct Cyc_List_List* _temp2087; void* _temp2089;
struct Cyc_Core_Opt* _temp2091; struct Cyc_List_List* _temp2093; int _temp2095;
struct Cyc_Core_Opt* _temp2097; void* _temp2099; struct Cyc_Core_Opt* _temp2101;
struct Cyc_List_List* _temp2103; struct _tuple0* _temp2105; _LL2051: if((
unsigned int) _temp2049 > 5u?(( struct _enum_struct*) _temp2049)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2068: _temp2067=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2049)->f1;
goto _LL2066; _LL2066: _temp2065=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2049)->f2; goto _LL2064; _LL2064: _temp2063=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2049)->f3; goto _LL2052;} else{ goto
_LL2053;} _LL2053: if(( unsigned int) _temp2049 > 5u?(( struct _enum_struct*)
_temp2049)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2070: _temp2069=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2049)->f1;
_LL2080: _temp2079=( void*) _temp2069.elt_typ; goto _LL2078; _LL2078: _temp2077=(
void*) _temp2069.rgn_typ; goto _LL2076; _LL2076: _temp2075=( struct Cyc_Absyn_Conref*)
_temp2069.nullable; goto _LL2074; _LL2074: _temp2073=( struct Cyc_Absyn_Tqual*)
_temp2069.tq; goto _LL2072; _LL2072: _temp2071=( struct Cyc_Absyn_Conref*)
_temp2069.bounds; goto _LL2054;} else{ goto _LL2055;} _LL2055: if(( unsigned int)
_temp2049 > 5u?(( struct _enum_struct*) _temp2049)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2082: _temp2081=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2049)->f1; _LL2094: _temp2093=( struct Cyc_List_List*) _temp2081.tvars;
goto _LL2092; _LL2092: _temp2091=( struct Cyc_Core_Opt*) _temp2081.effect; goto
_LL2090; _LL2090: _temp2089=( void*) _temp2081.ret_typ; goto _LL2088; _LL2088:
_temp2087=( struct Cyc_List_List*) _temp2081.args; goto _LL2086; _LL2086:
_temp2085=( int) _temp2081.varargs; goto _LL2084; _LL2084: _temp2083=( struct
Cyc_List_List*) _temp2081.attributes; goto _LL2056;} else{ goto _LL2057;}
_LL2057: if(( unsigned int) _temp2049 > 5u?(( struct _enum_struct*) _temp2049)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2100: _temp2099=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2049)->f1; goto _LL2098; _LL2098: _temp2097=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2049)->f2; goto _LL2096; _LL2096: _temp2095=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2049)->f3; goto _LL2058;} else{ goto
_LL2059;} _LL2059: if(( unsigned int) _temp2049 > 5u?(( struct _enum_struct*)
_temp2049)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2106: _temp2105=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2049)->f1; goto _LL2104;
_LL2104: _temp2103=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2049)->f2; goto _LL2102; _LL2102: _temp2101=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2049)->f3; goto _LL2060;} else{ goto
_LL2061;} _LL2061: goto _LL2062; _LL2052: { struct Cyc_List_List* _temp2109;
void* _temp2111; struct Cyc_Absyn_Tqual* _temp2113; struct _tuple4 _temp2107=
Cyc_Absynpp_to_tms( _temp2065, _temp2067); _LL2114: _temp2113= _temp2107.f1;
goto _LL2112; _LL2112: _temp2111= _temp2107.f2; goto _LL2110; _LL2110: _temp2109=
_temp2107.f3; goto _LL2108; _LL2108: { void* tm; if( Cyc_Evexp_eval_const_uint_exp(
_temp2063) ==( unsigned int) 0){ tm= Cyc_Absyn_Carray_mod;} else{ tm=({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2115=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2115->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp2115->f1= _temp2063;( void*) _temp2115;});} return({ struct _tuple4
_temp2116; _temp2116.f1= _temp2113; _temp2116.f2= _temp2111; _temp2116.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2117=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2117->hd=( void*) tm; _temp2117->tl=
_temp2109; _temp2117;}); _temp2116;});}} _LL2054: { struct Cyc_List_List*
_temp2120; void* _temp2122; struct Cyc_Absyn_Tqual* _temp2124; struct _tuple4
_temp2118= Cyc_Absynpp_to_tms( _temp2073, _temp2079); _LL2125: _temp2124=
_temp2118.f1; goto _LL2123; _LL2123: _temp2122= _temp2118.f2; goto _LL2121;
_LL2121: _temp2120= _temp2118.f3; goto _LL2119; _LL2119: { void* ps;{ struct
_tuple9 _temp2127=({ struct _tuple9 _temp2126; _temp2126.f1=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2075))->v; _temp2126.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2071))->v; _temp2126;}); void* _temp2137;
void* _temp2139; struct Cyc_Absyn_Exp* _temp2141; void* _temp2143; int _temp2145;
void* _temp2147; void* _temp2149; struct Cyc_Absyn_Exp* _temp2151; void*
_temp2153; int _temp2155; void* _temp2157; void* _temp2159; void* _temp2161;
_LL2129: _LL2144: _temp2143= _temp2127.f1; if(( unsigned int) _temp2143 > 1u?((
struct _enum_struct*) _temp2143)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2146:
_temp2145=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2143)->f1; if(
_temp2145 == 1){ goto _LL2138;} else{ goto _LL2131;}} else{ goto _LL2131;}
_LL2138: _temp2137= _temp2127.f2; if(( unsigned int) _temp2137 > 1u?(( struct
_enum_struct*) _temp2137)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2140:
_temp2139=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2137)->f1; if((
unsigned int) _temp2139 > 1u?(( struct _enum_struct*) _temp2139)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2142: _temp2141=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2139)->f1; goto _LL2130;} else{ goto _LL2131;}} else{ goto _LL2131;}
_LL2131: _LL2154: _temp2153= _temp2127.f1; if(( unsigned int) _temp2153 > 1u?((
struct _enum_struct*) _temp2153)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2156:
_temp2155=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2153)->f1; if(
_temp2155 == 0){ goto _LL2148;} else{ goto _LL2133;}} else{ goto _LL2133;}
_LL2148: _temp2147= _temp2127.f2; if(( unsigned int) _temp2147 > 1u?(( struct
_enum_struct*) _temp2147)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2150:
_temp2149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2147)->f1; if((
unsigned int) _temp2149 > 1u?(( struct _enum_struct*) _temp2149)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2152: _temp2151=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2149)->f1; goto _LL2132;} else{ goto _LL2133;}} else{ goto _LL2133;}
_LL2133: _LL2162: _temp2161= _temp2127.f1; goto _LL2158; _LL2158: _temp2157=
_temp2127.f2; if(( unsigned int) _temp2157 > 1u?(( struct _enum_struct*)
_temp2157)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2160: _temp2159=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2157)->f1; if( _temp2159 == Cyc_Absyn_Unknown_b){
goto _LL2134;} else{ goto _LL2135;}} else{ goto _LL2135;} _LL2135: goto _LL2136;
_LL2130: ps=({ struct Cyc_Absyn_Nullable_ps_struct* _temp2163=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp2163->tag= Cyc_Absyn_Nullable_ps_tag;
_temp2163->f1= _temp2141;( void*) _temp2163;}); goto _LL2128; _LL2132: ps=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp2164=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp2164->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp2164->f1= _temp2151;( void*) _temp2164;}); goto _LL2128; _LL2134: ps= Cyc_Absyn_TaggedArray_ps;
goto _LL2128; _LL2136: ps= Cyc_Absyn_TaggedArray_ps; goto _LL2128; _LL2128:;}{
void* tm=({ struct Cyc_Absyn_Pointer_mod_struct* _temp2165=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2165->tag= Cyc_Absyn_Pointer_mod_tag;
_temp2165->f1=( void*) ps; _temp2165->f2=( void*) _temp2077; _temp2165->f3= tq;(
void*) _temp2165;}); return({ struct _tuple4 _temp2166; _temp2166.f1= _temp2124;
_temp2166.f2= _temp2122; _temp2166.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2167=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2167->hd=( void*) tm; _temp2167->tl= _temp2120; _temp2167;}); _temp2166;});}}}
_LL2056: { struct Cyc_List_List* _temp2170; void* _temp2172; struct Cyc_Absyn_Tqual*
_temp2174; struct _tuple4 _temp2168= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp2089); _LL2175: _temp2174= _temp2168.f1; goto _LL2173; _LL2173: _temp2172=
_temp2168.f2; goto _LL2171; _LL2171: _temp2170= _temp2168.f3; goto _LL2169;
_LL2169: { struct Cyc_List_List* tms=({ struct Cyc_List_List* _temp2176=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2176->hd=( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2177=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2177->tag= Cyc_Absyn_Function_mod_tag;
_temp2177->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2178=( struct
Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2178->tag= Cyc_Absyn_WithTypes_tag; _temp2178->f1= _temp2087; _temp2178->f2=
_temp2085; _temp2178->f3= _temp2091;( void*) _temp2178;});( void*) _temp2177;});
_temp2176->tl= _temp2170; _temp2176;}); if( _temp2093 != 0){ tms=({ struct Cyc_List_List*
_temp2179=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2179->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2180=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp2180->tag= Cyc_Absyn_TypeParams_mod_tag; _temp2180->f1= _temp2093;
_temp2180->f2= 0; _temp2180->f3= 1;( void*) _temp2180;}); _temp2179->tl= tms;
_temp2179;});} if( _temp2083 != 0){ tms=({ struct Cyc_List_List* _temp2181=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2181->hd=(
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2182=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2182->tag= Cyc_Absyn_Attributes_mod_tag;
_temp2182->f1= 0; _temp2182->f2= _temp2083;( void*) _temp2182;}); _temp2181->tl=
tms; _temp2181;});} return({ struct _tuple4 _temp2183; _temp2183.f1= _temp2174;
_temp2183.f2= _temp2172; _temp2183.f3= tms; _temp2183;});}} _LL2058: if(
_temp2097 == 0){ return({ struct _tuple4 _temp2184; _temp2184.f1= tq; _temp2184.f2=
t; _temp2184.f3= 0; _temp2184;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)
_temp2097->v);} _LL2060: if( _temp2101 == 0? 1: ! Cyc_Absynpp_expand_typedefs){
return({ struct _tuple4 _temp2185; _temp2185.f1= tq; _temp2185.f2= t; _temp2185.f3=
0; _temp2185;});} else{ return Cyc_Absynpp_to_tms( tq,( void*) _temp2101->v);}
_LL2062: return({ struct _tuple4 _temp2186; _temp2186.f1= tq; _temp2186.f2= t;
_temp2186.f3= 0; _temp2186;}); _LL2050:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2189; void* _temp2191; struct Cyc_Absyn_Tqual* _temp2193; struct _tuple4
_temp2187= Cyc_Absynpp_to_tms( tq, t); _LL2194: _temp2193= _temp2187.f1; goto
_LL2192; _LL2192: _temp2191= _temp2187.f2; goto _LL2190; _LL2190: _temp2189=
_temp2187.f3; goto _LL2188; _LL2188: _temp2189=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2189); if( _temp2189 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2193), Cyc_Absynpp_ntyp2doc(
_temp2191));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2193),
Cyc_Absynpp_ntyp2doc( _temp2191), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2195=( char*)" "; struct _tagged_string _temp2196; _temp2196.curr=
_temp2195; _temp2196.base= _temp2195; _temp2196.last_plus_one= _temp2195 + 2;
_temp2196;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)
dopt->v, _temp2189));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc( struct
Cyc_Absyn_Structfield* f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc( f->tq,(
void*) f->type,({ struct Cyc_Core_Opt* _temp2197=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2197->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2197;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2198=( char*)";"; struct _tagged_string
_temp2199; _temp2199.curr= _temp2198; _temp2199.base= _temp2198; _temp2199.last_plus_one=
_temp2198 + 2; _temp2199;})));} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2200=( char*)""; struct _tagged_string _temp2201; _temp2201.curr= _temp2200;
_temp2201.base= _temp2200; _temp2201.last_plus_one= _temp2200 + 1; _temp2201;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( f->name), f->tag == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp2202=( char*)"=";
struct _tagged_string _temp2203; _temp2203.curr= _temp2202; _temp2203.base=
_temp2202; _temp2203.last_plus_one= _temp2202 + 2; _temp2203;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2204= f->tag; if( _temp2204
== 0){ _throw( Null_Exception);} _temp2204;}))), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,( struct _tagged_string)({ char*
_temp2205=( char*)","; struct _tagged_string _temp2206; _temp2206.curr=
_temp2205; _temp2206.base= _temp2205; _temp2206.last_plus_one= _temp2205 + 2;
_temp2206;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl= tdl->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd), 72, f); fprintf( f,"\n");}}
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){
return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct _tagged_string)({ char*
_temp2207=( char*)""; struct _tagged_string _temp2208; _temp2208.curr= _temp2207;
_temp2208.base= _temp2207; _temp2208.last_plus_one= _temp2207 + 1; _temp2208;}),((
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