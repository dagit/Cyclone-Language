#include "cyc_include.h"

 struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1;
void* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct
_tuple6{ int f1; void* f2; } ; struct _tuple7{ void* f1; void* f2; } ; struct
_tuple8{ void* f1; struct Cyc_List_List* f2; } ; struct _tuple9{ struct _tuple8*
f1; struct _tuple8* f2; } ; struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; struct _tuple11{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3; } ; struct _tuple12{ void* f1; void* f2; void* f3; } ; struct _tuple13{
struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; } ; struct _tuple14{ struct
_tuple13* f1; void* f2; } ; struct _tuple15{ struct Cyc_Absyn_Structfield* f1;
int f2; } ; struct _tuple16{ struct Cyc_List_List* f1; void* f2; } ; struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; void* f2; } ; struct _tuple18{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp(
int, int); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter2(
void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_exists_c( int(* pred)( void*, void*), void*
env, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct _tuple0 Cyc_List_split(
struct Cyc_List_List* x); extern void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct Cyc_List_List*
Cyc_Position_strings_of_segments( struct Cyc_List_List*); typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
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
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl;
struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator; typedef struct
_xenum_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute; struct
Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope;
typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t;
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
int q_const: 1; int q_volatile: 1; int q_restrict: 1; } ; extern void* Cyc_Absyn_B1;
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
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Enumdecl** f3; } ; extern const int Cyc_Absyn_XenumType_tag;
struct Cyc_Absyn_XenumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Xenumdecl**
f2; } ; extern const int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void*
Cyc_Absyn_FloatType; extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern const int Cyc_Absyn_UnionType_tag; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; extern const int Cyc_Absyn_RgnHandleType_tag; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ; extern void*
Cyc_Absyn_Null_c; extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times;
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
int tag; struct _tuple1* f1; } ; extern const int Cyc_Absyn_Enum_m_tag; struct
Cyc_Absyn_Enum_m_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield*
f2; } ; extern const int Cyc_Absyn_Xenum_m_tag; struct Cyc_Absyn_Xenum_m_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
extern const int Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple1* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; extern
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
int Cyc_Absyn_New_e_tag; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; extern const int Cyc_Absyn_Sizeoftyp_e_tag; struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_Sizeofexp_e_tag; struct Cyc_Absyn_Sizeofexp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Deref_e_tag;
struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_StructMember_e_tag; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const
int Cyc_Absyn_StructArrow_e_tag; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const int Cyc_Absyn_Subscript_e_tag;
struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_CompoundLit_e_tag;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
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
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Break_s_tag;
struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Continue_s_tag; struct Cyc_Absyn_Continue_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_For_s_tag; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4; } ; extern
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
struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3
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
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6; } ; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual* tq; void* type; struct
Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt*
cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_List_List* tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; extern const
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
_tuple1* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_FieldName_tag; struct
Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ; extern char
Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; }
; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern int
Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); extern struct _tuple4* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*, int); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[
7u]; struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved;
extern const int Cyc_Tcenv_VarRes_tag; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Tcenv_EnumRes_tag;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; extern const int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* xenumdecls; struct Cyc_Dict_Dict*
typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; }
; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void*
Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern struct
Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strconcat( struct _tagged_string, struct _tagged_string); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; }
; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_arithmetic_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_comparison_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern struct _tuple5* Cyc_Tcutil_make_inst_var(
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
struct Cyc_Absyn_Exp* e); extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); extern int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*); extern int
Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*); extern int
Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); char Cyc_Tcutil_TypeErr_tag[ 8u]="TypeErr"; extern
void Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment*
loc, struct _tagged_string s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_string msg){({ struct
_tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Tcutil_TypeErr_struct* _temp1=(
struct Cyc_Tcutil_TypeErr_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_TypeErr_struct));
_temp1->tag= Cyc_Tcutil_TypeErr_tag;( struct _xenum_struct*) _temp1;}));} static
struct _tagged_string Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return*
tv->name;} void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs
!= 0; tvs= tvs->tl){({ struct _tagged_string _temp2= Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*) tvs->hd); struct _tagged_string _temp3= Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ",
_temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one - _temp3.curr,
_temp3.curr);});} fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
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
unsigned int) _temp9 > 4u?(( struct _enum_struct*) _temp9)->tag == Cyc_Absyn_Evar_tag:
0){ _LL26: _temp25=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp9)->f1; goto
_LL24; _LL24: _temp23=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; if( _temp23 == 0){ goto _LL22;} else{ goto _LL13;} _LL22: _temp21=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp9)->f3; goto _LL12;} else{ goto _LL13;}
_LL13: if(( unsigned int) _temp9 > 4u?(( struct _enum_struct*) _temp9)->tag ==
Cyc_Absyn_Evar_tag: 0){ _LL33: _temp32=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f1; goto _LL30; _LL30: _temp29=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; _temp31=&(( struct Cyc_Absyn_Evar_struct*) _temp9)->f2; goto _LL28;
_LL28: _temp27=( int)(( struct Cyc_Absyn_Evar_struct*) _temp9)->f3; goto _LL14;}
else{ goto _LL15;} _LL15: if(( unsigned int) _temp9 > 4u?(( struct _enum_struct*)
_temp9)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL39: _temp38=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp9)->f1; goto _LL37; _LL37: _temp36=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f2; goto
_LL35; _LL35: _temp34=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f3; if( _temp34 == 0){ goto _LL16;} else{ goto _LL17;}} else{ goto
_LL17;} _LL17: if(( unsigned int) _temp9 > 4u?(( struct _enum_struct*) _temp9)->tag
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
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple7 _temp50=({ struct
_tuple7 _temp49; _temp49.f1= k1; _temp49.f2= k2; _temp49;}); void* _temp60; void*
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
void* _temp72= Cyc_Tcutil_compress( t); int _temp116; struct Cyc_Core_Opt*
_temp118; void* _temp120; struct Cyc_Absyn_Tvar* _temp122; void* _temp124; void*
_temp126; struct Cyc_Absyn_FnInfo _temp128; void* _temp130; struct Cyc_Absyn_Enumdecl**
_temp132; struct Cyc_List_List* _temp134; struct _tuple1* _temp136; struct Cyc_Absyn_Xenumdecl**
_temp138; struct _tuple1* _temp140; struct Cyc_Absyn_Structdecl** _temp142;
struct Cyc_List_List* _temp144; struct _tuple1* _temp146; struct Cyc_Absyn_Uniondecl**
_temp148; struct Cyc_List_List* _temp150; struct _tuple1* _temp152; struct Cyc_Absyn_Structdecl**
_temp154; struct Cyc_List_List* _temp156; struct _tuple1* _temp158; struct Cyc_Absyn_Uniondecl**
_temp160; struct Cyc_List_List* _temp162; struct _tuple1* _temp164; struct Cyc_Absyn_PtrInfo
_temp166; struct Cyc_Absyn_Exp* _temp168; struct Cyc_Absyn_Tqual* _temp170; void*
_temp172; struct Cyc_List_List* _temp174; struct Cyc_Core_Opt* _temp176; struct
Cyc_List_List* _temp178; struct _tuple1* _temp180; void* _temp182; struct Cyc_List_List*
_temp184; _LL74: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_Evar_tag: 0){ _LL121: _temp120=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp72)->f1; goto _LL119; _LL119: _temp118=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp72)->f2; goto _LL117; _LL117: _temp116=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp72)->f3; goto _LL75;} else{ goto _LL76;} _LL76: if(( unsigned int) _temp72
> 4u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_VarType_tag: 0){ _LL123:
_temp122=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp72)->f1;
goto _LL77;} else{ goto _LL78;} _LL78: if( _temp72 == Cyc_Absyn_VoidType){ goto
_LL79;} else{ goto _LL80;} _LL80: if(( unsigned int) _temp72 > 4u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_IntType_tag: 0){ _LL127: _temp126=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp72)->f1; goto _LL125; _LL125:
_temp124=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp72)->f2; goto _LL81;}
else{ goto _LL82;} _LL82: if( _temp72 == Cyc_Absyn_FloatType){ goto _LL83;}
else{ goto _LL84;} _LL84: if( _temp72 == Cyc_Absyn_DoubleType){ goto _LL85;}
else{ goto _LL86;} _LL86: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_FnType_tag: 0){ _LL129: _temp128=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp72)->f1; goto _LL87;} else{ goto _LL88;}
_LL88: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL131: _temp130=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp72)->f1; goto _LL89;} else{ goto _LL90;} _LL90: if( _temp72 == Cyc_Absyn_HeapRgn){
goto _LL91;} else{ goto _LL92;} _LL92: if(( unsigned int) _temp72 > 4u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL137: _temp136=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp72)->f1; goto _LL135;
_LL135: _temp134=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp72)->f2; goto _LL133; _LL133: _temp132=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp72)->f3; goto _LL93;} else{ goto _LL94;}
_LL94: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_XenumType_tag: 0){ _LL141: _temp140=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp72)->f1; goto _LL139; _LL139: _temp138=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp72)->f2; goto _LL95;} else{ goto _LL96;}
_LL96: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL147: _temp146=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp72)->f1; goto _LL145; _LL145: _temp144=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp72)->f2; goto _LL143; _LL143: _temp142=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp72)->f3;
if( _temp142 == 0){ goto _LL97;} else{ goto _LL98;}} else{ goto _LL98;} _LL98:
if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL153: _temp152=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp72)->f1; goto _LL151; _LL151: _temp150=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp72)->f2; goto _LL149; _LL149: _temp148=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp72)->f3;
if( _temp148 == 0){ goto _LL99;} else{ goto _LL100;}} else{ goto _LL100;} _LL100:
if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_StructType_tag:
0){ _LL159: _temp158=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp72)->f1; goto _LL157; _LL157: _temp156=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp72)->f2; goto _LL155; _LL155: _temp154=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp72)->f3;
goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int) _temp72 > 4u?((
struct _enum_struct*) _temp72)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL165:
_temp164=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp72)->f1;
goto _LL163; _LL163: _temp162=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp72)->f2; goto _LL161; _LL161: _temp160=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp72)->f3; goto _LL103;} else{ goto
_LL104;} _LL104: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL167: _temp166=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp72)->f1; goto _LL105;} else{ goto
_LL106;} _LL106: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL173: _temp172=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp72)->f1; goto _LL171; _LL171: _temp170=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp72)->f2;
goto _LL169; _LL169: _temp168=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp72)->f3; goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int)
_temp72 > 4u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL175: _temp174=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp72)->f1; goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int)
_temp72 > 4u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL181: _temp180=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp72)->f1; goto _LL179; _LL179: _temp178=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp72)->f2; goto _LL177; _LL177: _temp176=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp72)->f3; goto
_LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp72 > 4u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL183: _temp182=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp72)->f1; goto _LL113;} else{
goto _LL114;} _LL114: if(( unsigned int) _temp72 > 4u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL185: _temp184=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp72)->f1; goto _LL115;} else{ goto _LL73;}
_LL75: return _temp120; _LL77: return(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp122->kind); _LL79: return Cyc_Absyn_MemKind; _LL81:
return _temp124 == Cyc_Absyn_B4? Cyc_Absyn_BoxKind: Cyc_Absyn_MemKind; _LL83:
return Cyc_Absyn_MemKind; _LL85: return Cyc_Absyn_MemKind; _LL87: return Cyc_Absyn_MemKind;
_LL89: return Cyc_Absyn_BoxKind; _LL91: return Cyc_Absyn_RgnKind; _LL93: return
Cyc_Absyn_BoxKind; _LL95: return Cyc_Absyn_BoxKind; _LL97: return Cyc_Absyn_AnyKind;
_LL99: return Cyc_Absyn_AnyKind; _LL101: return Cyc_Absyn_MemKind; _LL103:
return Cyc_Absyn_MemKind; _LL105: { void* _temp186=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp166.bounds))->v;
void* _temp196; void* _temp198; struct Cyc_Absyn_Exp* _temp200; struct Cyc_Absyn_Conref*
_temp202; _LL188: if(( unsigned int) _temp186 > 1u?(( struct _enum_struct*)
_temp186)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL197: _temp196=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp186)->f1; if( _temp196 == Cyc_Absyn_Unknown_b){
goto _LL189;} else{ goto _LL190;}} else{ goto _LL190;} _LL190: if(( unsigned int)
_temp186 > 1u?(( struct _enum_struct*) _temp186)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL199: _temp198=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp186)->f1;
if(( unsigned int) _temp198 > 1u?(( struct _enum_struct*) _temp198)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL201: _temp200=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp198)->f1; goto _LL191;} else{ goto _LL192;}} else{ goto _LL192;} _LL192:
if( _temp186 == Cyc_Absyn_No_constr){ goto _LL193;} else{ goto _LL194;} _LL194:
if(( unsigned int) _temp186 > 1u?(( struct _enum_struct*) _temp186)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL203: _temp202=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp186)->f1; goto _LL195;} else{ goto _LL187;} _LL189: return Cyc_Absyn_MemKind;
_LL191: return Cyc_Absyn_BoxKind; _LL193: return Cyc_Absyn_MemKind; _LL195:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp204=( char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp205; _temp205.curr= _temp204; _temp205.base= _temp204;
_temp205.last_plus_one= _temp204 + 43; _temp205;})); _LL187:;} _LL107: return
Cyc_Absyn_MemKind; _LL109: return Cyc_Absyn_MemKind; _LL111: return(( void*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp206=
Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s", _temp206.last_plus_one
- _temp206.curr, _temp206.curr);})); _LL113: return Cyc_Absyn_EffKind; _LL115:
return Cyc_Absyn_EffKind; _LL73:;} char Cyc_Tcutil_Unify_tag[ 6u]="Unify";
struct Cyc_Tcutil_Unify_struct{ char* tag; } ; int Cyc_Tcutil_unify( void* t1,
void* t2){ struct _handler_cons _temp207; _push_handler(& _temp207);{ struct
_xenum_struct* _temp208=( struct _xenum_struct*) setjmp( _temp207.handler); if(
! _temp208){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp209= 1; _npop_handler( 0u);
return _temp209;}; _pop_handler();} else{ struct _xenum_struct* _temp211=
_temp208; _LL213: if( _temp211->tag == Cyc_Tcutil_Unify_tag){ goto _LL214;}
else{ goto _LL215;} _LL215: goto _LL216; _LL214: return 0; _LL216:( void) _throw(
_temp211); _LL212:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp217= Cyc_Tcutil_compress(
t); int _temp245; struct Cyc_Core_Opt* _temp247; void* _temp249; struct Cyc_Absyn_PtrInfo
_temp251; struct Cyc_Absyn_Exp* _temp253; struct Cyc_Absyn_Tqual* _temp255; void*
_temp257; struct Cyc_Absyn_FnInfo _temp259; struct Cyc_List_List* _temp261; int
_temp263; struct Cyc_List_List* _temp265; void* _temp267; struct Cyc_Core_Opt*
_temp269; struct Cyc_List_List* _temp271; struct Cyc_List_List* _temp273; void*
_temp275; struct Cyc_Core_Opt* _temp277; struct Cyc_List_List* _temp279; struct
_tuple1* _temp281; struct Cyc_Absyn_Enumdecl** _temp283; struct Cyc_List_List*
_temp285; struct _tuple1* _temp287; struct Cyc_Absyn_Uniondecl** _temp289;
struct Cyc_List_List* _temp291; struct _tuple1* _temp293; struct Cyc_Absyn_Structdecl**
_temp295; struct Cyc_List_List* _temp297; struct _tuple1* _temp299; void*
_temp301; struct Cyc_List_List* _temp303; _LL219: if(( unsigned int) _temp217 >
4u?(( struct _enum_struct*) _temp217)->tag == Cyc_Absyn_Evar_tag: 0){ _LL250:
_temp249=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp217)->f1; goto _LL248;
_LL248: _temp247=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp217)->f2; goto _LL246; _LL246: _temp245=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp217)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp217 > 4u?(( struct _enum_struct*) _temp217)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL252: _temp251=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp217)->f1; goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int)
_temp217 > 4u?(( struct _enum_struct*) _temp217)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL258: _temp257=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp217)->f1;
goto _LL256; _LL256: _temp255=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp217)->f2; goto _LL254; _LL254: _temp253=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp217)->f3; goto _LL224;} else{ goto _LL225;}
_LL225: if(( unsigned int) _temp217 > 4u?(( struct _enum_struct*) _temp217)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL260: _temp259=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp217)->f1; _LL272: _temp271=( struct Cyc_List_List*)
_temp259.tvars; goto _LL270; _LL270: _temp269=( struct Cyc_Core_Opt*) _temp259.effect;
goto _LL268; _LL268: _temp267=( void*) _temp259.ret_typ; goto _LL266; _LL266:
_temp265=( struct Cyc_List_List*) _temp259.args; goto _LL264; _LL264: _temp263=(
int) _temp259.varargs; goto _LL262; _LL262: _temp261=( struct Cyc_List_List*)
_temp259.attributes; goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int)
_temp217 > 4u?(( struct _enum_struct*) _temp217)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL274: _temp273=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp217)->f1; goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int)
_temp217 > 4u?(( struct _enum_struct*) _temp217)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL276: _temp275=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp217)->f1;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp217 > 4u?((
struct _enum_struct*) _temp217)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL282:
_temp281=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp217)->f1;
goto _LL280; _LL280: _temp279=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp217)->f2; goto _LL278; _LL278: _temp277=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp217)->f3; goto _LL232;} else{ goto _LL233;}
_LL233: if(( unsigned int) _temp217 > 4u?(( struct _enum_struct*) _temp217)->tag
== Cyc_Absyn_EnumType_tag: 0){ _LL288: _temp287=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp217)->f1; goto _LL286; _LL286: _temp285=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp217)->f2; goto _LL284; _LL284: _temp283=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp217)->f3;
goto _LL234;} else{ goto _LL235;} _LL235: if(( unsigned int) _temp217 > 4u?((
struct _enum_struct*) _temp217)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL294:
_temp293=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp217)->f1;
goto _LL292; _LL292: _temp291=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp217)->f2; goto _LL290; _LL290: _temp289=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp217)->f3; goto _LL236;} else{ goto
_LL237;} _LL237: if(( unsigned int) _temp217 > 4u?(( struct _enum_struct*)
_temp217)->tag == Cyc_Absyn_StructType_tag: 0){ _LL300: _temp299=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp217)->f1; goto _LL298;
_LL298: _temp297=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp217)->f2; goto _LL296; _LL296: _temp295=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp217)->f3; goto _LL238;} else{ goto
_LL239;} _LL239: if(( unsigned int) _temp217 > 4u?(( struct _enum_struct*)
_temp217)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL302: _temp301=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp217)->f1; goto _LL240;} else{ goto
_LL241;} _LL241: if(( unsigned int) _temp217 > 4u?(( struct _enum_struct*)
_temp217)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL304: _temp303=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp217)->f1; goto _LL242;} else{ goto _LL243;}
_LL243: goto _LL244; _LL220: if( t == evar){( void) _throw(({ struct Cyc_Tcutil_Unify_struct*
_temp305=( struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp305->tag= Cyc_Tcutil_Unify_tag;( struct _xenum_struct*) _temp305;}));}
else{ if( _temp247 != 0){ Cyc_Tcutil_occurs( evar,( void*) _temp247->v);}} goto
_LL218; _LL222: Cyc_Tcutil_occurs( evar,( void*) _temp251.elt_typ); Cyc_Tcutil_occurs(
evar,( void*) _temp251.rgn_typ); goto _LL218; _LL224: Cyc_Tcutil_occurs( evar,
_temp257); goto _LL218; _LL226: if( _temp269 != 0){ Cyc_Tcutil_occurs( evar,(
void*) _temp269->v);} Cyc_Tcutil_occurs( evar, _temp267); for( 0; _temp265 != 0;
_temp265= _temp265->tl){ Cyc_Tcutil_occurs( evar,(*(( struct _tuple2*) _temp265->hd)).f3);}
goto _LL218; _LL228: for( 0; _temp273 != 0; _temp273= _temp273->tl){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple4*) _temp273->hd)).f2);} goto _LL218; _LL230: Cyc_Tcutil_occurs(
evar, _temp275); goto _LL218; _LL232: _temp285= _temp279; goto _LL234; _LL234:
_temp291= _temp285; goto _LL236; _LL236: _temp297= _temp291; goto _LL238; _LL238:
Cyc_Tcutil_occurslist( evar, _temp297); goto _LL218; _LL240: Cyc_Tcutil_occurs(
evar, _temp301); goto _LL218; _LL242: Cyc_Tcutil_occurslist( evar, _temp303);
goto _LL218; _LL244: goto _LL218; _LL218:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* ts){ for( 0; ts != 0; ts= ts->tl){ Cyc_Tcutil_occurs(
evar,( void*) ts->hd);}} static void Cyc_Tcutil_unify_list( struct Cyc_List_List*
t1, struct Cyc_List_List* t2){ struct _handler_cons _temp306; _push_handler(&
_temp306);{ struct _xenum_struct* _temp307=( struct _xenum_struct*) setjmp(
_temp306.handler); if( ! _temp307){(( void(*)( void(* f)( void*, void*), struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it,
t1, t2);; _pop_handler();} else{ struct _xenum_struct* _temp309= _temp307;
_LL311: if( _temp309->tag == Cyc_List_List_mismatch_tag){ goto _LL312;} else{
goto _LL313;} _LL313: goto _LL314; _LL312:( void) _throw(({ struct Cyc_Tcutil_Unify_struct*
_temp315=( struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp315->tag= Cyc_Tcutil_Unify_tag;( struct _xenum_struct*) _temp315;}));
_LL314:( void) _throw( _temp309); _LL310:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2){ if(( tq1->q_const !=
tq2->q_const? 1: tq1->q_volatile != tq2->q_volatile)? 1: tq1->q_restrict != tq2->q_restrict){(
void) _throw(({ struct Cyc_Tcutil_Unify_struct* _temp316=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct)); _temp316->tag= Cyc_Tcutil_Unify_tag;(
struct _xenum_struct*) _temp316;}));}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual*
tq1, struct Cyc_Absyn_Tqual* tq2){ return( tq1->q_const == tq2->q_const? tq1->q_volatile
== tq2->q_volatile: 0)? tq1->q_restrict == tq2->q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp317=( void*) x->v; struct Cyc_Absyn_Conref*
_temp325; void* _temp327; _LL319: if( _temp317 == Cyc_Absyn_No_constr){ goto
_LL320;} else{ goto _LL321;} _LL321: if(( unsigned int) _temp317 > 1u?(( struct
_enum_struct*) _temp317)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL326:
_temp325=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp317)->f1; goto _LL322;} else{ goto _LL323;} _LL323: if(( unsigned int)
_temp317 > 1u?(( struct _enum_struct*) _temp317)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL328: _temp327=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp317)->f1;
goto _LL324;} else{ goto _LL318;} _LL320:( void*)( x->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp329=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp329->tag= Cyc_Absyn_Forward_constr_tag;
_temp329->f1= y;( void*) _temp329;})); return; _LL322:( void) _throw((( struct
_xenum_struct*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp330=( char*)"unify_conref: forward after compress";
struct _tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 37; _temp331;}))); _LL324: { void* _temp332=(
void*) y->v; struct Cyc_Absyn_Conref* _temp340; void* _temp342; _LL334: if(
_temp332 == Cyc_Absyn_No_constr){ goto _LL335;} else{ goto _LL336;} _LL336: if((
unsigned int) _temp332 > 1u?(( struct _enum_struct*) _temp332)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL341: _temp340=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp332)->f1; goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int)
_temp332 > 1u?(( struct _enum_struct*) _temp332)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL343: _temp342=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp332)->f1;
goto _LL339;} else{ goto _LL333;} _LL335:( void*)( y->v=( void*)(( void*) x->v));
return; _LL337:( void) _throw((( struct _xenum_struct*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp344=( char*)"unify_conref: forward after compress(2)";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 40; _temp345;}))); _LL339: if( cmp( _temp327,
_temp342) != 0){( void) _throw(({ struct Cyc_Tcutil_Unify_struct* _temp346=(
struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct));
_temp346->tag= Cyc_Tcutil_Unify_tag;( struct _xenum_struct*) _temp346;}));}
return; _LL333:;} _LL318:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct
_handler_cons _temp347; _push_handler(& _temp347);{ struct _xenum_struct*
_temp348=( struct _xenum_struct*) setjmp( _temp347.handler); if( ! _temp348){((
void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int _temp349= 1; _npop_handler( 0u);
return _temp349;}; _pop_handler();} else{ struct _xenum_struct* _temp351=
_temp348; _LL353: if( _temp351->tag == Cyc_Tcutil_Unify_tag){ goto _LL354;}
else{ goto _LL355;} _LL355: goto _LL356; _LL354: return 0; _LL356:( void) _throw(
_temp351); _LL352:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple7 _temp358=({ struct _tuple7 _temp357; _temp357.f1= b1; _temp357.f2=
b2; _temp357;}); void* _temp368; void* _temp370; void* _temp372; void* _temp374;
void* _temp376; void* _temp378; void* _temp380; struct Cyc_Absyn_Exp* _temp382;
void* _temp384; struct Cyc_Absyn_Exp* _temp386; _LL360: _LL371: _temp370=
_temp358.f1; if( _temp370 == Cyc_Absyn_Unknown_b){ goto _LL369;} else{ goto
_LL362;} _LL369: _temp368= _temp358.f2; if( _temp368 == Cyc_Absyn_Unknown_b){
goto _LL361;} else{ goto _LL362;} _LL362: _LL375: _temp374= _temp358.f1; if(
_temp374 == Cyc_Absyn_Unknown_b){ goto _LL373;} else{ goto _LL364;} _LL373:
_temp372= _temp358.f2; goto _LL363; _LL364: _LL379: _temp378= _temp358.f1; goto
_LL377; _LL377: _temp376= _temp358.f2; if( _temp376 == Cyc_Absyn_Unknown_b){
goto _LL365;} else{ goto _LL366;} _LL366: _LL385: _temp384= _temp358.f1; if((
unsigned int) _temp384 > 1u?(( struct _enum_struct*) _temp384)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL387: _temp386=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp384)->f1; goto _LL381;} else{ goto _LL359;} _LL381: _temp380= _temp358.f2;
if(( unsigned int) _temp380 > 1u?(( struct _enum_struct*) _temp380)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL383: _temp382=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp380)->f1; goto _LL367;} else{ goto _LL359;} _LL361: return 0; _LL363:
return - 1; _LL365: return 1; _LL367: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp386); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp382); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL359:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple7 _temp389=({
struct _tuple7 _temp388; _temp388.f1= a1; _temp388.f2= a2; _temp388;}); void*
_temp399; int _temp401; void* _temp403; int _temp405; void* _temp407; int
_temp409; void* _temp411; int _temp413; void* _temp415; struct _tagged_string
_temp417; void* _temp419; struct _tagged_string _temp421; _LL391: _LL404:
_temp403= _temp389.f1; if(( unsigned int) _temp403 > 15u?(( struct _enum_struct*)
_temp403)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL406: _temp405=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp403)->f1; goto _LL400;} else{ goto
_LL393;} _LL400: _temp399= _temp389.f2; if(( unsigned int) _temp399 > 15u?((
struct _enum_struct*) _temp399)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL402:
_temp401=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp399)->f1; goto
_LL392;} else{ goto _LL393;} _LL393: _LL412: _temp411= _temp389.f1; if((
unsigned int) _temp411 > 15u?(( struct _enum_struct*) _temp411)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL414: _temp413=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp411)->f1;
goto _LL408;} else{ goto _LL395;} _LL408: _temp407= _temp389.f2; if((
unsigned int) _temp407 > 15u?(( struct _enum_struct*) _temp407)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL410: _temp409=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp407)->f1;
goto _LL394;} else{ goto _LL395;} _LL395: _LL420: _temp419= _temp389.f1; if((
unsigned int) _temp419 > 15u?(( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Section_att_tag:
0){ _LL422: _temp421=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp419)->f1; goto _LL416;} else{ goto _LL397;} _LL416: _temp415= _temp389.f2;
if(( unsigned int) _temp415 > 15u?(( struct _enum_struct*) _temp415)->tag == Cyc_Absyn_Section_att_tag:
0){ _LL418: _temp417=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp415)->f1; goto _LL396;} else{ goto _LL397;} _LL397: goto _LL398; _LL392:
_temp413= _temp405; _temp409= _temp401; goto _LL394; _LL394: return _temp413 ==
_temp409; _LL396: return Cyc_String_strcmp( _temp421, _temp417) == 0; _LL398:
return 0; _LL390:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct
Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a= a->tl){ if(
!(( int(*)( int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*) a->hd, a2)){ return 0;}}}{ struct Cyc_List_List* a=
a2; for( 0; a != 0; a= a->tl){ if( !(( int(*)( int(* pred)( void*, void*), void*
env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)
a->hd, a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp423= e; struct Cyc_List_List*
_temp429; struct Cyc_List_List** _temp431; _LL425: if(( unsigned int) _temp423 >
4u?(( struct _enum_struct*) _temp423)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL430:
_temp429=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp423)->f1;
_temp431=&(( struct Cyc_Absyn_JoinEff_struct*) _temp423)->f1; goto _LL426;}
else{ goto _LL427;} _LL427: goto _LL428; _LL426: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp431; for( 0; effs != 0; effs= effs->tl){ void* eff=(
void*) effs->hd; Cyc_Tcutil_normalize_effect( eff);{ void* _temp432= Cyc_Tcutil_compress(
eff); struct Cyc_List_List* _temp438; _LL434: if(( unsigned int) _temp432 > 4u?((
struct _enum_struct*) _temp432)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL439:
_temp438=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp432)->f1;
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL435: nested_join= 1;
goto _LL433; _LL437: goto _LL433; _LL433:;}}} if( ! nested_join){ return;}{
struct Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp431; for( 0;
effs != 0; effs= effs->tl){ void* _temp440= Cyc_Tcutil_compress(( void*) effs->hd);
struct Cyc_List_List* _temp446; _LL442: if(( unsigned int) _temp440 > 4u?((
struct _enum_struct*) _temp440)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL447:
_temp446=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp440)->f1;
goto _LL443;} else{ goto _LL444;} _LL444: goto _LL445; _LL443: effects=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(
_temp446, effects); goto _LL441; _LL445: effects=({ struct Cyc_List_List*
_temp448=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp448->hd=( void*) _temp440; _temp448->tl= effects; _temp448;}); goto _LL441;
_LL441:;}}* _temp431=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); goto _LL424;}} _LL428: goto _LL424; _LL424:;}} static struct _tuple8*
Cyc_Tcutil_get_effect_evar( void* t){ void* _temp449= Cyc_Tcutil_compress( t);
struct Cyc_List_List* _temp457; struct Cyc_List_List _temp459; struct Cyc_List_List*
_temp460; void* _temp462; int _temp464; struct Cyc_Core_Opt* _temp466; struct
Cyc_Core_Opt** _temp468; void* _temp469; void** _temp471; _LL451: if((
unsigned int) _temp449 > 4u?(( struct _enum_struct*) _temp449)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL458: _temp457=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp449)->f1; if( _temp457 == 0){ goto _LL453;} else{ _temp459=* _temp457;
_LL463: _temp462=( void*) _temp459.hd; goto _LL461; _LL461: _temp460=( struct
Cyc_List_List*) _temp459.tl; goto _LL452;}} else{ goto _LL453;} _LL453: if((
unsigned int) _temp449 > 4u?(( struct _enum_struct*) _temp449)->tag == Cyc_Absyn_Evar_tag:
0){ _LL470: _temp469=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp449)->f1;
_temp471=&(( struct Cyc_Absyn_Evar_struct*) _temp449)->f1; goto _LL467; _LL467:
_temp466=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp449)->f2;
_temp468=&(( struct Cyc_Absyn_Evar_struct*) _temp449)->f2; goto _LL465; _LL465:
_temp464=( int)(( struct Cyc_Absyn_Evar_struct*) _temp449)->f3; goto _LL454;}
else{ goto _LL455;} _LL455: goto _LL456; _LL452: { void* _temp472= Cyc_Tcutil_compress(
_temp462); int _temp478; struct Cyc_Core_Opt* _temp480; void* _temp482; _LL474:
if(( unsigned int) _temp472 > 4u?(( struct _enum_struct*) _temp472)->tag == Cyc_Absyn_Evar_tag:
0){ _LL483: _temp482=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp472)->f1;
goto _LL481; _LL481: _temp480=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp472)->f2; goto _LL479; _LL479: _temp478=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp472)->f3; goto _LL475;} else{ goto _LL476;} _LL476: goto _LL477; _LL475:
return({ struct _tuple8* _temp484=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp484->f1= _temp462; _temp484->f2= _temp460; _temp484;}); _LL477:
return 0; _LL473:;} _LL454: if(* _temp471 != Cyc_Absyn_EffKind){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp485=( char*)"effect evar has wrong kind"; struct _tagged_string _temp486;
_temp486.curr= _temp485; _temp486.base= _temp485; _temp486.last_plus_one=
_temp485 + 27; _temp486;}));} return({ struct _tuple8* _temp487=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp487->f1= t; _temp487->f2= 0; _temp487;});
_LL456: return 0; _LL450:;} int Cyc_Tcutil_region_in_effect( int constrain, void*
r, void* e){ r= Cyc_Tcutil_compress( r); if( r == Cyc_Absyn_HeapRgn){ return 1;}{
void* _temp488= Cyc_Tcutil_compress( e); void* _temp498; struct Cyc_List_List*
_temp500; int _temp502; struct Cyc_Core_Opt* _temp504; struct Cyc_Core_Opt**
_temp506; void* _temp507; void** _temp509; _LL490: if(( unsigned int) _temp488 >
4u?(( struct _enum_struct*) _temp488)->tag == Cyc_Absyn_AccessEff_tag: 0){
_LL499: _temp498=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp488)->f1;
goto _LL491;} else{ goto _LL492;} _LL492: if(( unsigned int) _temp488 > 4u?((
struct _enum_struct*) _temp488)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL501:
_temp500=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp488)->f1;
goto _LL493;} else{ goto _LL494;} _LL494: if(( unsigned int) _temp488 > 4u?((
struct _enum_struct*) _temp488)->tag == Cyc_Absyn_Evar_tag: 0){ _LL508: _temp507=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp488)->f1; _temp509=&(( struct Cyc_Absyn_Evar_struct*)
_temp488)->f1; goto _LL505; _LL505: _temp504=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp488)->f2; _temp506=&(( struct Cyc_Absyn_Evar_struct*)
_temp488)->f2; goto _LL503; _LL503: _temp502=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp488)->f3; goto _LL495;} else{ goto _LL496;} _LL496: goto _LL497; _LL491:
if( constrain){ return Cyc_Tcutil_unify( r, _temp498);} Cyc_Tcutil_compress(
_temp498); if( r == _temp498){ return 1;}{ struct _tuple7 _temp511=({ struct
_tuple7 _temp510; _temp510.f1= r; _temp510.f2= _temp498; _temp510;}); void*
_temp517; struct Cyc_Absyn_Tvar* _temp519; void* _temp521; struct Cyc_Absyn_Tvar*
_temp523; _LL513: _LL522: _temp521= _temp511.f1; if(( unsigned int) _temp521 > 4u?((
struct _enum_struct*) _temp521)->tag == Cyc_Absyn_VarType_tag: 0){ _LL524:
_temp523=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp521)->f1;
goto _LL518;} else{ goto _LL515;} _LL518: _temp517= _temp511.f2; if((
unsigned int) _temp517 > 4u?(( struct _enum_struct*) _temp517)->tag == Cyc_Absyn_VarType_tag:
0){ _LL520: _temp519=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp517)->f1; goto _LL514;} else{ goto _LL515;} _LL515: goto _LL516; _LL514:
return Cyc_Absyn_tvar_cmp( _temp523, _temp519) == 0; _LL516: return 0; _LL512:;}
_LL493: for( 0; _temp500 != 0; _temp500= _temp500->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*) _temp500->hd)){ return 1;}} return 0; _LL495: if(*
_temp509 != Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp525=( char*)"effect evar has wrong kind";
struct _tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 27; _temp526;}));} if( ! constrain){ return 0;}{
void* ev= Cyc_Absyn_new_evar( Cyc_Absyn_EffKind); void* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp527=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp527->tag= Cyc_Absyn_JoinEff_tag; _temp527->f1=({ struct Cyc_List_List*
_temp528=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp528->hd=( void*) ev; _temp528->tl=({ struct Cyc_List_List* _temp529=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp529->hd=(
void*)({ struct Cyc_Absyn_AccessEff_struct* _temp530=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp530->tag= Cyc_Absyn_AccessEff_tag;
_temp530->f1=( void*) r;( void*) _temp530;}); _temp529->tl= 0; _temp529;});
_temp528;});( void*) _temp527;});* _temp506=({ struct Cyc_Core_Opt* _temp531=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp531->v=(
void*) new_typ; _temp531;}); return 1;} _LL497: return 0; _LL489:;}} static int
Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar* v, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp532= e; struct Cyc_Absyn_Tvar* _temp542;
struct Cyc_List_List* _temp544; int _temp546; struct Cyc_Core_Opt* _temp548;
struct Cyc_Core_Opt** _temp550; void* _temp551; void** _temp553; _LL534: if((
unsigned int) _temp532 > 4u?(( struct _enum_struct*) _temp532)->tag == Cyc_Absyn_VarType_tag:
0){ _LL543: _temp542=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp532)->f1; goto _LL535;} else{ goto _LL536;} _LL536: if(( unsigned int)
_temp532 > 4u?(( struct _enum_struct*) _temp532)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL545: _temp544=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp532)->f1; goto _LL537;} else{ goto _LL538;} _LL538: if(( unsigned int)
_temp532 > 4u?(( struct _enum_struct*) _temp532)->tag == Cyc_Absyn_Evar_tag: 0){
_LL552: _temp551=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp532)->f1;
_temp553=&(( struct Cyc_Absyn_Evar_struct*) _temp532)->f1; goto _LL549; _LL549:
_temp548=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp532)->f2;
_temp550=&(( struct Cyc_Absyn_Evar_struct*) _temp532)->f2; goto _LL547; _LL547:
_temp546=( int)(( struct Cyc_Absyn_Evar_struct*) _temp532)->f3; goto _LL539;}
else{ goto _LL540;} _LL540: goto _LL541; _LL535: return Cyc_Absyn_tvar_cmp( v,
_temp542) == 0; _LL537: for( 0; _temp544 != 0; _temp544= _temp544->tl){ if( Cyc_Tcutil_variable_in_effect(
constrain, v,( void*) _temp544->hd)){ return 1;}} return 0; _LL539: if(*
_temp553 != Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp554=( char*)"effect evar has wrong kind";
struct _tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 27; _temp555;}));}{ void* ev= Cyc_Absyn_new_evar(
Cyc_Absyn_EffKind); void* new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp556=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp556->tag= Cyc_Absyn_JoinEff_tag; _temp556->f1=({ struct Cyc_List_List*
_temp557=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp557->hd=( void*) ev; _temp557->tl=({ struct Cyc_List_List* _temp558=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp558->hd=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp559=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp559->tag= Cyc_Absyn_VarType_tag;
_temp559->f1= v;( void*) _temp559;}); _temp558->tl= 0; _temp558;}); _temp557;});(
void*) _temp556;});* _temp550=({ struct Cyc_Core_Opt* _temp560=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp560->v=( void*) new_typ; _temp560;});
return 1;} _LL541: return 0; _LL533:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp561= e; struct Cyc_List_List*
_temp569; int _temp571; struct Cyc_Core_Opt* _temp573; void* _temp575; _LL563:
if(( unsigned int) _temp561 > 4u?(( struct _enum_struct*) _temp561)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL570: _temp569=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp561)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(( unsigned int)
_temp561 > 4u?(( struct _enum_struct*) _temp561)->tag == Cyc_Absyn_Evar_tag: 0){
_LL576: _temp575=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp561)->f1; goto
_LL574; _LL574: _temp573=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp561)->f2; goto _LL572; _LL572: _temp571=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp561)->f3; goto _LL566;} else{ goto _LL567;} _LL567: goto _LL568; _LL564:
for( 0; _temp569 != 0; _temp569= _temp569->tl){ if( Cyc_Tcutil_evar_in_effect(
evar,( void*) _temp569->hd)){ return 1;}} return 0; _LL566: return evar == e;
_LL568: return 0; _LL562:;}} int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2){{ struct _tuple8* _temp577= Cyc_Tcutil_get_effect_evar( e2);
struct _tuple8 _temp583; struct Cyc_List_List* _temp584; void* _temp586; _LL579:
if( _temp577 == 0){ goto _LL581;} else{ _temp583=* _temp577; _LL587: _temp586=
_temp583.f1; goto _LL585; _LL585: _temp584= _temp583.f2; goto _LL580;} _LL581:
goto _LL582; _LL580: { void* ev= Cyc_Absyn_new_evar( Cyc_Absyn_EffKind); void*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp588=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp588->tag= Cyc_Absyn_JoinEff_tag;
_temp588->f1=({ struct Cyc_List_List* _temp589=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp589->hd=( void*) ev; _temp589->tl=({
struct Cyc_List_List* _temp590=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp590->hd=( void*) e1; _temp590->tl= 0; _temp590;});
_temp589;});( void*) _temp588;}); return Cyc_Tcutil_unify( _temp586, new_typ);}
_LL582: goto _LL578; _LL578:;}{ void* _temp591= Cyc_Tcutil_compress( e1); struct
Cyc_List_List* _temp603; void* _temp605; struct Cyc_Absyn_Tvar* _temp607; int
_temp609; struct Cyc_Core_Opt* _temp611; struct Cyc_Core_Opt** _temp613; void*
_temp614; _LL593: if(( unsigned int) _temp591 > 4u?(( struct _enum_struct*)
_temp591)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL604: _temp603=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp591)->f1; goto _LL594;} else{ goto _LL595;}
_LL595: if(( unsigned int) _temp591 > 4u?(( struct _enum_struct*) _temp591)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL606: _temp605=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp591)->f1; goto _LL596;} else{ goto _LL597;} _LL597: if(( unsigned int)
_temp591 > 4u?(( struct _enum_struct*) _temp591)->tag == Cyc_Absyn_VarType_tag:
0){ _LL608: _temp607=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp591)->f1; goto _LL598;} else{ goto _LL599;} _LL599: if(( unsigned int)
_temp591 > 4u?(( struct _enum_struct*) _temp591)->tag == Cyc_Absyn_Evar_tag: 0){
_LL615: _temp614=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp591)->f1; goto
_LL612; _LL612: _temp611=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp591)->f2; _temp613=&(( struct Cyc_Absyn_Evar_struct*) _temp591)->f2; goto
_LL610; _LL610: _temp609=( int)(( struct Cyc_Absyn_Evar_struct*) _temp591)->f3;
goto _LL600;} else{ goto _LL601;} _LL601: goto _LL602; _LL594: for( 0; _temp603
!= 0; _temp603= _temp603->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*) _temp603->hd, e2)){ return 0;}} return 1; _LL596: return( Cyc_Tcutil_region_in_effect(
0, _temp605, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp605, e2))? 1: Cyc_Tcutil_unify(
_temp605, Cyc_Absyn_HeapRgn); _LL598: return Cyc_Tcutil_variable_in_effect( 0,
_temp607, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp607, e2); _LL600: if( !
Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp613=({ struct Cyc_Core_Opt*
_temp616=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp616->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp617=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp617->tag= Cyc_Absyn_JoinEff_tag;
_temp617->f1= 0;( void*) _temp617;}); _temp616;});} else{* _temp613=({ struct
Cyc_Core_Opt* _temp618=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp618->v=( void*) e2; _temp618;});}} return 1; _LL602: return(( int(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp619=
Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s", _temp619.last_plus_one
- _temp619.curr, _temp619.curr);})); _LL592:;}} static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp621=({ struct _tuple9 _temp620; _temp620.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp620.f2= Cyc_Tcutil_get_effect_evar( e2); _temp620;}); struct _tuple8*
_temp627; struct _tuple8 _temp629; struct Cyc_List_List* _temp630; void*
_temp632; struct _tuple8* _temp634; struct _tuple8 _temp636; struct Cyc_List_List*
_temp637; void* _temp639; _LL623: _LL635: _temp634= _temp621.f1; if( _temp634 ==
0){ goto _LL625;} else{ _temp636=* _temp634; _LL640: _temp639= _temp636.f1; goto
_LL638; _LL638: _temp637= _temp636.f2; goto _LL628;} _LL628: _temp627= _temp621.f2;
if( _temp627 == 0){ goto _LL625;} else{ _temp629=* _temp627; _LL633: _temp632=
_temp629.f1; goto _LL631; _LL631: _temp630= _temp629.f2; goto _LL624;} _LL625:
goto _LL626; _LL624: { void* ev= Cyc_Absyn_new_evar( Cyc_Absyn_EffKind); void*
new_typ=({ struct Cyc_Absyn_JoinEff_struct* _temp641=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp641->tag= Cyc_Absyn_JoinEff_tag;
_temp641->f1=({ struct Cyc_List_List* _temp642=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp642->hd=( void*) ev; _temp642->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp637, _temp630); _temp642;});( void*) _temp641;}); return Cyc_Tcutil_unify(
_temp639, new_typ)? Cyc_Tcutil_unify( _temp632, new_typ): 0;} _LL626: return Cyc_Tcutil_subset_effect(
0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1): 0; _LL622:;}} void Cyc_Tcutil_unify_it(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);
if( t1 == t2){ return;}{ void* _temp643= t1; int _temp649; struct Cyc_Core_Opt*
_temp651; struct Cyc_Core_Opt** _temp653; void* _temp654; _LL645: if((
unsigned int) _temp643 > 4u?(( struct _enum_struct*) _temp643)->tag == Cyc_Absyn_Evar_tag:
0){ _LL655: _temp654=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp643)->f1;
goto _LL652; _LL652: _temp651=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp643)->f2; _temp653=&(( struct Cyc_Absyn_Evar_struct*) _temp643)->f2; goto
_LL650; _LL650: _temp649=( int)(( struct Cyc_Absyn_Evar_struct*) _temp643)->f3;
goto _LL646;} else{ goto _LL647;} _LL647: goto _LL648; _LL646: Cyc_Tcutil_occurs(
t1, t2);{ void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( kind2,
_temp654)){* _temp653=({ struct Cyc_Core_Opt* _temp656=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp656->v=( void*) t2; _temp656;});
return;} else{{ void* _temp657= t2; int _temp665; struct Cyc_Core_Opt* _temp667;
struct Cyc_Core_Opt** _temp669; void* _temp670; struct Cyc_Absyn_PtrInfo
_temp673; _LL659: if(( unsigned int) _temp657 > 4u?(( struct _enum_struct*)
_temp657)->tag == Cyc_Absyn_Evar_tag: 0){ _LL671: _temp670=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp657)->f1; goto _LL668; _LL668: _temp667=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp657)->f2; _temp669=&(( struct Cyc_Absyn_Evar_struct*)
_temp657)->f2; goto _LL666; _LL666: _temp665=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp657)->f3; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp657 > 4u?(( struct _enum_struct*) _temp657)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL674: _temp673=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp657)->f1; goto _LL672;} else{ goto _LL663;} _LL672: if( _temp654 == Cyc_Absyn_BoxKind){
goto _LL662;} else{ goto _LL663;} _LL663: goto _LL664; _LL660: if( Cyc_Tcutil_kind_leq(
_temp654, kind2)){* _temp669=({ struct Cyc_Core_Opt* _temp675=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp675->v=( void*) t1; _temp675;});
return;} goto _LL658; _LL662: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp673.bounds);{ void*
_temp676=( void*) c->v; _LL678: if( _temp676 == Cyc_Absyn_No_constr){ goto
_LL679;} else{ goto _LL680;} _LL680: goto _LL681; _LL679:( void*)( c->v=( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp682=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp682->tag= Cyc_Absyn_Eq_constr_tag;
_temp682->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp683=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp683->tag= Cyc_Absyn_Upper_b_tag;
_temp683->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp683;});( void*)
_temp682;}));* _temp653=({ struct Cyc_Core_Opt* _temp684=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp684->v=( void*) t2; _temp684;});
return; _LL681: goto _LL677; _LL677:;} goto _LL658;} _LL664: goto _LL658; _LL658:;}(
void) _throw(({ struct Cyc_Tcutil_Unify_struct* _temp685=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct)); _temp685->tag= Cyc_Tcutil_Unify_tag;(
struct _xenum_struct*) _temp685;}));}} _LL648: goto _LL644; _LL644:;}{ struct
_tuple7 _temp687=({ struct _tuple7 _temp686; _temp686.f1= t2; _temp686.f2= t1;
_temp686;}); void* _temp731; void* _temp733; int _temp735; struct Cyc_Core_Opt*
_temp737; void* _temp739; void* _temp741; void* _temp743; void* _temp745; struct
Cyc_Absyn_Tvar* _temp747; void* _temp749; struct Cyc_Absyn_Tvar* _temp751; void*
_temp753; struct Cyc_Absyn_Structdecl** _temp755; struct Cyc_List_List* _temp757;
struct _tuple1* _temp759; void* _temp761; struct Cyc_Absyn_Structdecl** _temp763;
struct Cyc_List_List* _temp765; struct _tuple1* _temp767; void* _temp769; struct
Cyc_Absyn_Uniondecl** _temp771; struct Cyc_List_List* _temp773; struct _tuple1*
_temp775; void* _temp777; struct Cyc_Absyn_Uniondecl** _temp779; struct Cyc_List_List*
_temp781; struct _tuple1* _temp783; void* _temp785; struct Cyc_Absyn_Enumdecl**
_temp787; struct Cyc_List_List* _temp789; struct _tuple1* _temp791; void*
_temp793; struct Cyc_Absyn_Enumdecl** _temp795; struct Cyc_List_List* _temp797;
struct _tuple1* _temp799; void* _temp801; struct Cyc_Absyn_Xenumdecl** _temp803;
struct _tuple1* _temp805; void* _temp807; struct Cyc_Absyn_Xenumdecl** _temp809;
struct _tuple1* _temp811; void* _temp813; struct Cyc_Absyn_PtrInfo _temp815;
struct Cyc_Absyn_Conref* _temp817; struct Cyc_Absyn_Tqual* _temp819; struct Cyc_Absyn_Conref*
_temp821; void* _temp823; void* _temp825; void* _temp827; struct Cyc_Absyn_PtrInfo
_temp829; struct Cyc_Absyn_Conref* _temp831; struct Cyc_Absyn_Tqual* _temp833;
struct Cyc_Absyn_Conref* _temp835; void* _temp837; void* _temp839; void*
_temp841; void* _temp843; void* _temp845; void* _temp847; void* _temp849; void*
_temp851; void* _temp853; void* _temp855; void* _temp857; void* _temp859; void*
_temp861; struct Cyc_Absyn_Exp* _temp863; struct Cyc_Absyn_Tqual* _temp865; void*
_temp867; void* _temp869; struct Cyc_Absyn_Exp* _temp871; struct Cyc_Absyn_Tqual*
_temp873; void* _temp875; void* _temp877; struct Cyc_Absyn_FnInfo _temp879;
struct Cyc_List_List* _temp881; int _temp883; struct Cyc_List_List* _temp885;
void* _temp887; struct Cyc_Core_Opt* _temp889; struct Cyc_List_List* _temp891;
void* _temp893; struct Cyc_Absyn_FnInfo _temp895; struct Cyc_List_List* _temp897;
int _temp899; struct Cyc_List_List* _temp901; void* _temp903; struct Cyc_Core_Opt*
_temp905; struct Cyc_List_List* _temp907; void* _temp909; struct Cyc_List_List*
_temp911; void* _temp913; struct Cyc_List_List* _temp915; void* _temp917; void*
_temp919; void* _temp921; void* _temp923; void* _temp925; void* _temp927; void*
_temp929; void* _temp931; struct Cyc_List_List* _temp933; void* _temp935; struct
Cyc_List_List* _temp937; void* _temp939; void* _temp941; void* _temp943; void*
_temp945; void* _temp947; void* _temp949; void* _temp951; _LL689: _LL734:
_temp733= _temp687.f1; if(( unsigned int) _temp733 > 4u?(( struct _enum_struct*)
_temp733)->tag == Cyc_Absyn_Evar_tag: 0){ _LL740: _temp739=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp733)->f1; goto _LL738; _LL738: _temp737=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp733)->f2; goto _LL736; _LL736: _temp735=( int)((
struct Cyc_Absyn_Evar_struct*) _temp733)->f3; goto _LL732;} else{ goto _LL691;}
_LL732: _temp731= _temp687.f2; goto _LL690; _LL691: _LL744: _temp743= _temp687.f1;
if( _temp743 == Cyc_Absyn_VoidType){ goto _LL742;} else{ goto _LL693;} _LL742:
_temp741= _temp687.f2; if( _temp741 == Cyc_Absyn_VoidType){ goto _LL692;} else{
goto _LL693;} _LL693: _LL750: _temp749= _temp687.f1; if(( unsigned int) _temp749
> 4u?(( struct _enum_struct*) _temp749)->tag == Cyc_Absyn_VarType_tag: 0){
_LL752: _temp751=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp749)->f1; goto _LL746;} else{ goto _LL695;} _LL746: _temp745= _temp687.f2;
if(( unsigned int) _temp745 > 4u?(( struct _enum_struct*) _temp745)->tag == Cyc_Absyn_VarType_tag:
0){ _LL748: _temp747=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp745)->f1; goto _LL694;} else{ goto _LL695;} _LL695: _LL762: _temp761=
_temp687.f1; if(( unsigned int) _temp761 > 4u?(( struct _enum_struct*) _temp761)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL768: _temp767=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp761)->f1; goto _LL766; _LL766: _temp765=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp761)->f2;
goto _LL764; _LL764: _temp763=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp761)->f3; goto _LL754;} else{ goto _LL697;} _LL754: _temp753= _temp687.f2;
if(( unsigned int) _temp753 > 4u?(( struct _enum_struct*) _temp753)->tag == Cyc_Absyn_StructType_tag:
0){ _LL760: _temp759=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp753)->f1; goto _LL758; _LL758: _temp757=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp753)->f2; goto _LL756; _LL756: _temp755=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp753)->f3;
goto _LL696;} else{ goto _LL697;} _LL697: _LL778: _temp777= _temp687.f1; if((
unsigned int) _temp777 > 4u?(( struct _enum_struct*) _temp777)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL784: _temp783=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp777)->f1; goto _LL782; _LL782: _temp781=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp777)->f2; goto _LL780; _LL780: _temp779=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp777)->f3;
goto _LL770;} else{ goto _LL699;} _LL770: _temp769= _temp687.f2; if((
unsigned int) _temp769 > 4u?(( struct _enum_struct*) _temp769)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL776: _temp775=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp769)->f1; goto _LL774; _LL774: _temp773=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp769)->f2; goto _LL772; _LL772: _temp771=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp769)->f3;
goto _LL698;} else{ goto _LL699;} _LL699: _LL794: _temp793= _temp687.f1; if((
unsigned int) _temp793 > 4u?(( struct _enum_struct*) _temp793)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL800: _temp799=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp793)->f1; goto _LL798; _LL798: _temp797=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp793)->f2; goto _LL796; _LL796: _temp795=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp793)->f3;
goto _LL786;} else{ goto _LL701;} _LL786: _temp785= _temp687.f2; if((
unsigned int) _temp785 > 4u?(( struct _enum_struct*) _temp785)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL792: _temp791=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp785)->f1; goto _LL790; _LL790: _temp789=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp785)->f2; goto _LL788; _LL788: _temp787=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp785)->f3;
goto _LL700;} else{ goto _LL701;} _LL701: _LL808: _temp807= _temp687.f1; if((
unsigned int) _temp807 > 4u?(( struct _enum_struct*) _temp807)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL812: _temp811=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp807)->f1; goto _LL810; _LL810: _temp809=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp807)->f2; goto _LL802;} else{ goto
_LL703;} _LL802: _temp801= _temp687.f2; if(( unsigned int) _temp801 > 4u?((
struct _enum_struct*) _temp801)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL806:
_temp805=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp801)->f1;
goto _LL804; _LL804: _temp803=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp801)->f2; goto _LL702;} else{ goto _LL703;} _LL703: _LL828: _temp827=
_temp687.f1; if(( unsigned int) _temp827 > 4u?(( struct _enum_struct*) _temp827)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL830: _temp829=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp827)->f1; _LL840: _temp839=( void*)
_temp829.elt_typ; goto _LL838; _LL838: _temp837=( void*) _temp829.rgn_typ; goto
_LL836; _LL836: _temp835=( struct Cyc_Absyn_Conref*) _temp829.nullable; goto
_LL834; _LL834: _temp833=( struct Cyc_Absyn_Tqual*) _temp829.tq; goto _LL832;
_LL832: _temp831=( struct Cyc_Absyn_Conref*) _temp829.bounds; goto _LL814;}
else{ goto _LL705;} _LL814: _temp813= _temp687.f2; if(( unsigned int) _temp813 >
4u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL816: _temp815=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp813)->f1; _LL826: _temp825=( void*) _temp815.elt_typ; goto _LL824; _LL824:
_temp823=( void*) _temp815.rgn_typ; goto _LL822; _LL822: _temp821=( struct Cyc_Absyn_Conref*)
_temp815.nullable; goto _LL820; _LL820: _temp819=( struct Cyc_Absyn_Tqual*)
_temp815.tq; goto _LL818; _LL818: _temp817=( struct Cyc_Absyn_Conref*) _temp815.bounds;
goto _LL704;} else{ goto _LL705;} _LL705: _LL848: _temp847= _temp687.f1; if((
unsigned int) _temp847 > 4u?(( struct _enum_struct*) _temp847)->tag == Cyc_Absyn_IntType_tag:
0){ _LL852: _temp851=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp847)->f1;
goto _LL850; _LL850: _temp849=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp847)->f2; goto _LL842;} else{ goto _LL707;} _LL842: _temp841= _temp687.f2;
if(( unsigned int) _temp841 > 4u?(( struct _enum_struct*) _temp841)->tag == Cyc_Absyn_IntType_tag:
0){ _LL846: _temp845=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp841)->f1;
goto _LL844; _LL844: _temp843=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp841)->f2; goto _LL706;} else{ goto _LL707;} _LL707: _LL856: _temp855=
_temp687.f1; if( _temp855 == Cyc_Absyn_FloatType){ goto _LL854;} else{ goto
_LL709;} _LL854: _temp853= _temp687.f2; if( _temp853 == Cyc_Absyn_FloatType){
goto _LL708;} else{ goto _LL709;} _LL709: _LL860: _temp859= _temp687.f1; if(
_temp859 == Cyc_Absyn_DoubleType){ goto _LL858;} else{ goto _LL711;} _LL858:
_temp857= _temp687.f2; if( _temp857 == Cyc_Absyn_DoubleType){ goto _LL710;}
else{ goto _LL711;} _LL711: _LL870: _temp869= _temp687.f1; if(( unsigned int)
_temp869 > 4u?(( struct _enum_struct*) _temp869)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL876: _temp875=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp869)->f1;
goto _LL874; _LL874: _temp873=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp869)->f2; goto _LL872; _LL872: _temp871=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp869)->f3; goto _LL862;} else{ goto _LL713;}
_LL862: _temp861= _temp687.f2; if(( unsigned int) _temp861 > 4u?(( struct
_enum_struct*) _temp861)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL868: _temp867=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp861)->f1; goto _LL866; _LL866:
_temp865=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp861)->f2; goto _LL864; _LL864: _temp863=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp861)->f3; goto _LL712;} else{ goto _LL713;}
_LL713: _LL894: _temp893= _temp687.f1; if(( unsigned int) _temp893 > 4u?((
struct _enum_struct*) _temp893)->tag == Cyc_Absyn_FnType_tag: 0){ _LL896:
_temp895=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp893)->f1;
_LL908: _temp907=( struct Cyc_List_List*) _temp895.tvars; goto _LL906; _LL906:
_temp905=( struct Cyc_Core_Opt*) _temp895.effect; goto _LL904; _LL904: _temp903=(
void*) _temp895.ret_typ; goto _LL902; _LL902: _temp901=( struct Cyc_List_List*)
_temp895.args; goto _LL900; _LL900: _temp899=( int) _temp895.varargs; goto
_LL898; _LL898: _temp897=( struct Cyc_List_List*) _temp895.attributes; goto
_LL878;} else{ goto _LL715;} _LL878: _temp877= _temp687.f2; if(( unsigned int)
_temp877 > 4u?(( struct _enum_struct*) _temp877)->tag == Cyc_Absyn_FnType_tag: 0){
_LL880: _temp879=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp877)->f1; _LL892: _temp891=( struct Cyc_List_List*) _temp879.tvars; goto
_LL890; _LL890: _temp889=( struct Cyc_Core_Opt*) _temp879.effect; goto _LL888;
_LL888: _temp887=( void*) _temp879.ret_typ; goto _LL886; _LL886: _temp885=(
struct Cyc_List_List*) _temp879.args; goto _LL884; _LL884: _temp883=( int)
_temp879.varargs; goto _LL882; _LL882: _temp881=( struct Cyc_List_List*)
_temp879.attributes; goto _LL714;} else{ goto _LL715;} _LL715: _LL914: _temp913=
_temp687.f1; if(( unsigned int) _temp913 > 4u?(( struct _enum_struct*) _temp913)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL916: _temp915=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp913)->f1; goto _LL910;} else{ goto
_LL717;} _LL910: _temp909= _temp687.f2; if(( unsigned int) _temp909 > 4u?((
struct _enum_struct*) _temp909)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL912:
_temp911=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp909)->f1;
goto _LL716;} else{ goto _LL717;} _LL717: _LL920: _temp919= _temp687.f1; if(
_temp919 == Cyc_Absyn_HeapRgn){ goto _LL918;} else{ goto _LL719;} _LL918:
_temp917= _temp687.f2; if( _temp917 == Cyc_Absyn_HeapRgn){ goto _LL718;} else{
goto _LL719;} _LL719: _LL926: _temp925= _temp687.f1; if(( unsigned int) _temp925
> 4u?(( struct _enum_struct*) _temp925)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL928: _temp927=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp925)->f1;
goto _LL922;} else{ goto _LL721;} _LL922: _temp921= _temp687.f2; if((
unsigned int) _temp921 > 4u?(( struct _enum_struct*) _temp921)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL924: _temp923=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp921)->f1;
goto _LL720;} else{ goto _LL721;} _LL721: _LL932: _temp931= _temp687.f1; if((
unsigned int) _temp931 > 4u?(( struct _enum_struct*) _temp931)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL934: _temp933=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp931)->f1; goto _LL930;} else{ goto _LL723;} _LL930: _temp929= _temp687.f2;
goto _LL722; _LL723: _LL940: _temp939= _temp687.f1; goto _LL936; _LL936:
_temp935= _temp687.f2; if(( unsigned int) _temp935 > 4u?(( struct _enum_struct*)
_temp935)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL938: _temp937=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp935)->f1; goto _LL724;} else{ goto _LL725;}
_LL725: _LL944: _temp943= _temp687.f1; if(( unsigned int) _temp943 > 4u?((
struct _enum_struct*) _temp943)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL946:
_temp945=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp943)->f1; goto
_LL942;} else{ goto _LL727;} _LL942: _temp941= _temp687.f2; goto _LL726; _LL727:
_LL952: _temp951= _temp687.f1; goto _LL948; _LL948: _temp947= _temp687.f2; if((
unsigned int) _temp947 > 4u?(( struct _enum_struct*) _temp947)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL950: _temp949=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp947)->f1;
goto _LL728;} else{ goto _LL729;} _LL729: goto _LL730; _LL690: Cyc_Tcutil_unify_it(
t2, t1); return; _LL692: return; _LL694: { struct _tagged_string* x2= _temp751->name;
struct _tagged_string* x1= _temp747->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp751->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp747->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp953=* x2; struct _tagged_string _temp954= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp955= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp953.last_plus_one - _temp953.curr, _temp953.curr, _temp954.last_plus_one -
_temp954.curr, _temp954.curr, _temp955.last_plus_one - _temp955.curr, _temp955.curr);}));}
return;} goto _LL688;} _LL696: if((( _temp759 != 0? _temp767 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*)({ struct _tuple1* _temp956= _temp759; if( _temp956 == 0){
_throw( Null_Exception);} _temp956;}),( struct _tuple1*)({ struct _tuple1*
_temp957= _temp767; if( _temp957 == 0){ _throw( Null_Exception);} _temp957;}))
== 0: 0)? 1:( _temp759 == 0? _temp767 == 0: 0)){ Cyc_Tcutil_unify_list( _temp757,
_temp765); return;} goto _LL688; _LL698: if((( _temp775 != 0? _temp783 != 0: 0)?
Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1* _temp958= _temp775; if(
_temp958 == 0){ _throw( Null_Exception);} _temp958;}),( struct _tuple1*)({
struct _tuple1* _temp959= _temp783; if( _temp959 == 0){ _throw( Null_Exception);}
_temp959;})) == 0: 0)? 1:( _temp775 == 0? _temp783 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp773, _temp781); return;} goto _LL688; _LL700: if((( _temp791 != 0? _temp799
!= 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1* _temp960=
_temp791; if( _temp960 == 0){ _throw( Null_Exception);} _temp960;}),( struct
_tuple1*)({ struct _tuple1* _temp961= _temp799; if( _temp961 == 0){ _throw(
Null_Exception);} _temp961;})) == 0: 0)? 1:( _temp791 == 0? _temp799 == 0: 0)){
Cyc_Tcutil_unify_list( _temp789, _temp797); return;} goto _LL688; _LL702: if(*
_temp809 ==* _temp803){ return;} goto _LL688; _LL704: Cyc_Tcutil_unify_it(
_temp825, _temp839); Cyc_Tcutil_unify_it( _temp837, _temp823); Cyc_Tcutil_unify_tqual(
_temp819, _temp833);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp817, _temp831);{ void* _temp962=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp817))->v; void*
_temp968; _LL964: if(( unsigned int) _temp962 > 1u?(( struct _enum_struct*)
_temp962)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL969: _temp968=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp962)->f1; if( _temp968 == Cyc_Absyn_Unknown_b){
goto _LL965;} else{ goto _LL966;}} else{ goto _LL966;} _LL966: goto _LL967;
_LL965: return; _LL967: goto _LL963; _LL963:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp821, _temp835); return; _LL706: if( _temp845 == _temp851?
_temp843 == _temp849: 0){ return;} goto _LL688; _LL708: return; _LL710: return;
_LL712: Cyc_Tcutil_unify_it( _temp867, _temp875); Cyc_Tcutil_unify_tqual(
_temp865, _temp873); if( _temp871 == _temp863){ return;} if( _temp871 == 0? 1:
_temp863 == 0){ goto _LL688;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp970= _temp871; if( _temp970 == 0){ _throw(
Null_Exception);} _temp970;})) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp971= _temp863; if( _temp971 == 0){ _throw(
Null_Exception);} _temp971;}))){ return;} goto _LL688; _LL714: { struct Cyc_List_List*
inst= 0; while( _temp891 != 0) { if( _temp907 == 0){ break;} inst=({ struct Cyc_List_List*
_temp972=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp972->hd=( void*)({ struct _tuple5* _temp973=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp973->f1=( struct Cyc_Absyn_Tvar*) _temp907->hd;
_temp973->f2=({ struct Cyc_Absyn_VarType_struct* _temp974=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp974->tag= Cyc_Absyn_VarType_tag;
_temp974->f1=( struct Cyc_Absyn_Tvar*) _temp891->hd;( void*) _temp974;});
_temp973;}); _temp972->tl= inst; _temp972;}); _temp891= _temp891->tl; _temp907=
_temp907->tl;} if( _temp907 != 0){ goto _LL688;} if( inst != 0){ Cyc_Tcutil_unify_it(({
struct Cyc_Absyn_FnType_struct* _temp975=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp975->tag= Cyc_Absyn_FnType_tag;
_temp975->f1=({ struct Cyc_Absyn_FnInfo _temp976; _temp976.tvars= 0; _temp976.effect=
_temp889; _temp976.ret_typ=( void*) _temp887; _temp976.args= _temp885; _temp976.varargs=
_temp883; _temp976.attributes= _temp881; _temp976;});( void*) _temp975;}), Cyc_Tcutil_substitute(
inst,({ struct Cyc_Absyn_FnType_struct* _temp977=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp977->tag= Cyc_Absyn_FnType_tag;
_temp977->f1=({ struct Cyc_Absyn_FnInfo _temp978; _temp978.tvars= 0; _temp978.effect=
_temp905; _temp978.ret_typ=( void*) _temp903; _temp978.args= _temp901; _temp978.varargs=
_temp899; _temp978.attributes= _temp897; _temp978;});( void*) _temp977;})));
return;} Cyc_Tcutil_unify_it( _temp887, _temp903); for( 0; _temp885 != 0?
_temp901 != 0: 0; _temp885= _temp885->tl, _temp901= _temp901->tl){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*) _temp885->hd)).f2,(*(( struct _tuple2*) _temp901->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*) _temp885->hd)).f3,(*(( struct _tuple2*) _temp901->hd)).f3);}
if( _temp885 != 0? 1: _temp901 != 0){ goto _LL688;} if( _temp883 != _temp899){
goto _LL688;}{ int bad_effect= 0;{ struct _tuple10 _temp980=({ struct _tuple10
_temp979; _temp979.f1= _temp889; _temp979.f2= _temp905; _temp979;}); struct Cyc_Core_Opt*
_temp990; struct Cyc_Core_Opt* _temp992; struct Cyc_Core_Opt* _temp994; struct
Cyc_Core_Opt* _temp996; struct Cyc_Core_Opt* _temp998; struct Cyc_Core_Opt*
_temp1000; _LL982: _LL993: _temp992= _temp980.f1; if( _temp992 == 0){ goto
_LL991;} else{ goto _LL984;} _LL991: _temp990= _temp980.f2; if( _temp990 == 0){
goto _LL983;} else{ goto _LL984;} _LL984: _LL997: _temp996= _temp980.f1; if(
_temp996 == 0){ goto _LL995;} else{ goto _LL986;} _LL995: _temp994= _temp980.f2;
goto _LL985; _LL986: _LL1001: _temp1000= _temp980.f1; goto _LL999; _LL999:
_temp998= _temp980.f2; if( _temp998 == 0){ goto _LL987;} else{ goto _LL988;}
_LL988: goto _LL989; _LL983: goto _LL981; _LL985: goto _LL987; _LL987:
bad_effect= 1; goto _LL981; _LL989: bad_effect= ! Cyc_Tcutil_unify_effect(( void*)
_temp889->v,( void*) _temp905->v); goto _LL981; _LL981:;} if( bad_effect){ goto
_LL688;} if( ! Cyc_Tcutil_same_atts( _temp897, _temp881)){ goto _LL688;} return;}}
_LL716: for( 0; _temp911 != 0? _temp915 != 0: 0; _temp911= _temp911->tl,
_temp915= _temp915->tl){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*) _temp911->hd)).f1,(*((
struct _tuple4*) _temp915->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)
_temp911->hd)).f2,(*(( struct _tuple4*) _temp915->hd)).f2);} if( _temp911 == 0?
_temp915 == 0: 0){ return;} goto _LL688; _LL718: return; _LL720: Cyc_Tcutil_unify_it(
_temp927, _temp923); return; _LL722: goto _LL724; _LL724: goto _LL726; _LL726:
goto _LL728; _LL728: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL688;} _LL730: goto _LL688; _LL688:;}( void) _throw(({ struct Cyc_Tcutil_Unify_struct*
_temp1002=( struct Cyc_Tcutil_Unify_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcutil_Unify_struct)); _temp1002->tag= Cyc_Tcutil_Unify_tag;( struct
_xenum_struct*) _temp1002;}));} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp1003= Cyc_Tcutil_compress( t); void* _temp1013; void* _temp1015;
_LL1005: if(( unsigned int) _temp1003 > 4u?(( struct _enum_struct*) _temp1003)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL1016: _temp1015=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1003)->f1; goto _LL1014; _LL1014: _temp1013=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1003)->f2; goto _LL1006;} else{ goto _LL1007;} _LL1007: if( _temp1003 ==
Cyc_Absyn_FloatType){ goto _LL1008;} else{ goto _LL1009;} _LL1009: if( _temp1003
== Cyc_Absyn_DoubleType){ goto _LL1010;} else{ goto _LL1011;} _LL1011: goto
_LL1012; _LL1006: return 1; _LL1008: return 1; _LL1010: return 1; _LL1012:
return 0; _LL1004:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1018=({ struct _tuple7 _temp1017; _temp1017.f1= t1; _temp1017.f2= t2;
_temp1017;}); void* _temp1040; void* _temp1042; void* _temp1044; void* _temp1046;
void* _temp1048; void* _temp1050; void* _temp1052; void* _temp1054; void*
_temp1056; void* _temp1058; void* _temp1060; void* _temp1062; void* _temp1064;
void* _temp1066; void* _temp1068; void* _temp1070; void* _temp1072; void*
_temp1074; void* _temp1076; void* _temp1078; void* _temp1080; void* _temp1082;
void* _temp1084; void* _temp1086; void* _temp1088; void* _temp1090; void*
_temp1092; void* _temp1094; void* _temp1096; void* _temp1098; void* _temp1100;
void* _temp1102; void* _temp1104; void* _temp1106; void* _temp1108; void*
_temp1110; void* _temp1112; void* _temp1114; void* _temp1116; void* _temp1118;
void* _temp1120; void* _temp1122; _LL1020: _LL1043: _temp1042= _temp1018.f1; if(
_temp1042 == Cyc_Absyn_DoubleType){ goto _LL1041;} else{ goto _LL1022;} _LL1041:
_temp1040= _temp1018.f2; if( _temp1040 == Cyc_Absyn_FloatType){ goto _LL1021;}
else{ goto _LL1022;} _LL1022: _LL1051: _temp1050= _temp1018.f1; if( _temp1050 ==
Cyc_Absyn_DoubleType){ goto _LL1045;} else{ goto _LL1024;} _LL1045: _temp1044=
_temp1018.f2; if(( unsigned int) _temp1044 > 4u?(( struct _enum_struct*)
_temp1044)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1049: _temp1048=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1044)->f1; goto _LL1047; _LL1047:
_temp1046=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1044)->f2; goto
_LL1023;} else{ goto _LL1024;} _LL1024: _LL1059: _temp1058= _temp1018.f1; if(
_temp1058 == Cyc_Absyn_FloatType){ goto _LL1053;} else{ goto _LL1026;} _LL1053:
_temp1052= _temp1018.f2; if(( unsigned int) _temp1052 > 4u?(( struct
_enum_struct*) _temp1052)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1057: _temp1056=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1052)->f1; goto _LL1055; _LL1055:
_temp1054=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1052)->f2; goto
_LL1025;} else{ goto _LL1026;} _LL1026: _LL1067: _temp1066= _temp1018.f1; if((
unsigned int) _temp1066 > 4u?(( struct _enum_struct*) _temp1066)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1071: _temp1070=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1066)->f1;
goto _LL1069; _LL1069: _temp1068=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1066)->f2; if( _temp1068 == Cyc_Absyn_B8){ goto _LL1061;} else{ goto
_LL1028;}} else{ goto _LL1028;} _LL1061: _temp1060= _temp1018.f2; if((
unsigned int) _temp1060 > 4u?(( struct _enum_struct*) _temp1060)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1065: _temp1064=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1060)->f1;
goto _LL1063; _LL1063: _temp1062=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1060)->f2; if( _temp1062 == Cyc_Absyn_B8){ goto _LL1027;} else{ goto
_LL1028;}} else{ goto _LL1028;} _LL1028: _LL1075: _temp1074= _temp1018.f1; if((
unsigned int) _temp1074 > 4u?(( struct _enum_struct*) _temp1074)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1079: _temp1078=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1074)->f1;
goto _LL1077; _LL1077: _temp1076=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1074)->f2; if( _temp1076 == Cyc_Absyn_B8){ goto _LL1073;} else{ goto
_LL1030;}} else{ goto _LL1030;} _LL1073: _temp1072= _temp1018.f2; goto _LL1029;
_LL1030: _LL1083: _temp1082= _temp1018.f1; if(( unsigned int) _temp1082 > 4u?((
struct _enum_struct*) _temp1082)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1087:
_temp1086=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1082)->f1; goto
_LL1085; _LL1085: _temp1084=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1082)->f2; if( _temp1084 == Cyc_Absyn_B4){ goto _LL1081;} else{ goto
_LL1032;}} else{ goto _LL1032;} _LL1081: _temp1080= _temp1018.f2; if( _temp1080
== Cyc_Absyn_FloatType){ goto _LL1031;} else{ goto _LL1032;} _LL1032: _LL1095:
_temp1094= _temp1018.f1; if(( unsigned int) _temp1094 > 4u?(( struct
_enum_struct*) _temp1094)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1099: _temp1098=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1094)->f1; goto _LL1097; _LL1097:
_temp1096=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1094)->f2; if(
_temp1096 == Cyc_Absyn_B4){ goto _LL1089;} else{ goto _LL1034;}} else{ goto
_LL1034;} _LL1089: _temp1088= _temp1018.f2; if(( unsigned int) _temp1088 > 4u?((
struct _enum_struct*) _temp1088)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1093:
_temp1092=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1088)->f1; goto
_LL1091; _LL1091: _temp1090=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1088)->f2; if( _temp1090 == Cyc_Absyn_B2){ goto _LL1033;} else{ goto
_LL1034;}} else{ goto _LL1034;} _LL1034: _LL1107: _temp1106= _temp1018.f1; if((
unsigned int) _temp1106 > 4u?(( struct _enum_struct*) _temp1106)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1111: _temp1110=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1106)->f1;
goto _LL1109; _LL1109: _temp1108=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1106)->f2; if( _temp1108 == Cyc_Absyn_B4){ goto _LL1101;} else{ goto
_LL1036;}} else{ goto _LL1036;} _LL1101: _temp1100= _temp1018.f2; if((
unsigned int) _temp1100 > 4u?(( struct _enum_struct*) _temp1100)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1100)->f1;
goto _LL1103; _LL1103: _temp1102=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1100)->f2; if( _temp1102 == Cyc_Absyn_B1){ goto _LL1035;} else{ goto
_LL1036;}} else{ goto _LL1036;} _LL1036: _LL1119: _temp1118= _temp1018.f1; if((
unsigned int) _temp1118 > 4u?(( struct _enum_struct*) _temp1118)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1123: _temp1122=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1118)->f1;
goto _LL1121; _LL1121: _temp1120=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1118)->f2; if( _temp1120 == Cyc_Absyn_B2){ goto _LL1113;} else{ goto
_LL1038;}} else{ goto _LL1038;} _LL1113: _temp1112= _temp1018.f2; if((
unsigned int) _temp1112 > 4u?(( struct _enum_struct*) _temp1112)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1117: _temp1116=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1112)->f1;
goto _LL1115; _LL1115: _temp1114=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1112)->f2; if( _temp1114 == Cyc_Absyn_B1){ goto _LL1037;} else{ goto
_LL1038;}} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1021: return 1;
_LL1023: return 1; _LL1025: return 1; _LL1027: return 0; _LL1029: return 1;
_LL1031: return 1; _LL1033: return 1; _LL1035: return 1; _LL1037: return 1;
_LL1039: return 0; _LL1019:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el= el->tl){ void* t1= Cyc_Tcutil_compress((
void*)((( struct Cyc_Absyn_Exp*) el->hd)->topt)->v); if( Cyc_Tcutil_is_arithmetic_type(
t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)
max_arith_type->v)){ max_arith_type=({ struct Cyc_Core_Opt* _temp1124=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1124->v=( void*)
t1; _temp1124;});}}}} if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)
max_arith_type->v)){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el= el->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*) el->hd,
t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) el->hd)->loc,({ struct
_tagged_string _temp1125= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1126= Cyc_Absynpp_typ2string(( void*)((( struct Cyc_Absyn_Exp*) el->hd)->topt)->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1125.last_plus_one
- _temp1125.curr, _temp1125.curr, _temp1126.last_plus_one - _temp1126.curr,
_temp1126.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1127= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1133; _LL1129: if(( unsigned int) _temp1127 > 4u?(( struct _enum_struct*)
_temp1127)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1134: _temp1133=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1127)->f1; goto
_LL1130;} else{ goto _LL1131;} _LL1131: goto _LL1132; _LL1130: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1128; _LL1132: return 0; _LL1128:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1135= Cyc_Tcutil_compress(
t); void* _temp1141; void* _temp1143; _LL1137: if(( unsigned int) _temp1135 > 4u?((
struct _enum_struct*) _temp1135)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1144:
_temp1143=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1135)->f1; goto
_LL1142; _LL1142: _temp1141=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1135)->f2; goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140;
_LL1138: return 1; _LL1140: return 0; _LL1136:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1145=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1146; _temp1146.curr= _temp1145; _temp1146.base= _temp1145;
_temp1146.last_plus_one= _temp1145 + 44; _temp1146;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1147=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1148; _temp1148.curr= _temp1147; _temp1148.base= _temp1147;
_temp1148.last_plus_one= _temp1147 + 44; _temp1148;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1150=({ struct _tuple7 _temp1149; _temp1149.f1= t1; _temp1149.f2= t2;
_temp1149;}); void* _temp1158; struct Cyc_Absyn_PtrInfo _temp1160; void*
_temp1162; struct Cyc_Absyn_PtrInfo _temp1164; void* _temp1166; struct Cyc_Absyn_Exp*
_temp1168; struct Cyc_Absyn_Tqual* _temp1170; void* _temp1172; void* _temp1174;
struct Cyc_Absyn_Exp* _temp1176; struct Cyc_Absyn_Tqual* _temp1178; void*
_temp1180; _LL1152: _LL1163: _temp1162= _temp1150.f1; if(( unsigned int)
_temp1162 > 4u?(( struct _enum_struct*) _temp1162)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1165: _temp1164=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1162)->f1; goto _LL1159;} else{ goto _LL1154;} _LL1159: _temp1158=
_temp1150.f2; if(( unsigned int) _temp1158 > 4u?(( struct _enum_struct*)
_temp1158)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1161: _temp1160=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1158)->f1; goto
_LL1153;} else{ goto _LL1154;} _LL1154: _LL1175: _temp1174= _temp1150.f1; if((
unsigned int) _temp1174 > 4u?(( struct _enum_struct*) _temp1174)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1181: _temp1180=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1174)->f1;
goto _LL1179; _LL1179: _temp1178=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1174)->f2; goto _LL1177; _LL1177: _temp1176=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1174)->f3; goto _LL1167;} else{ goto
_LL1156;} _LL1167: _temp1166= _temp1150.f2; if(( unsigned int) _temp1166 > 4u?((
struct _enum_struct*) _temp1166)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1173:
_temp1172=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1166)->f1; goto
_LL1171; _LL1171: _temp1170=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1166)->f2; goto _LL1169; _LL1169: _temp1168=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1166)->f3; goto _LL1155;} else{ goto
_LL1156;} _LL1156: goto _LL1157; _LL1153: { int okay= 1; if( !(( int(*)( int(*
cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1164.nullable, _temp1160.nullable)){ void* _temp1182=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1164.nullable))->v; int _temp1188; _LL1184: if(( unsigned int) _temp1182 >
1u?(( struct _enum_struct*) _temp1182)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL1189: _temp1188=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1182)->f1;
goto _LL1185;} else{ goto _LL1186;} _LL1186: goto _LL1187; _LL1185: okay= !
_temp1188; goto _LL1183; _LL1187:( void) _throw((( struct _xenum_struct*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1190=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1191; _temp1191.curr= _temp1190; _temp1191.base= _temp1190; _temp1191.last_plus_one=
_temp1190 + 30; _temp1191;}))); _LL1183:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1164.bounds, _temp1160.bounds)){ struct _tuple7
_temp1193=({ struct _tuple7 _temp1192; _temp1192.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1164.bounds))->v;
_temp1192.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1160.bounds))->v; _temp1192;}); void* _temp1201;
void* _temp1203; void* _temp1205; void* _temp1207; struct Cyc_Absyn_Exp*
_temp1209; void* _temp1211; void* _temp1213; void* _temp1215; void* _temp1217;
_LL1195: _LL1206: _temp1205= _temp1193.f1; if(( unsigned int) _temp1205 > 1u?((
struct _enum_struct*) _temp1205)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1208:
_temp1207=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1205)->f1; if((
unsigned int) _temp1207 > 1u?(( struct _enum_struct*) _temp1207)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1210: _temp1209=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1207)->f1; goto _LL1202;} else{ goto _LL1197;}} else{ goto _LL1197;}
_LL1202: _temp1201= _temp1193.f2; if(( unsigned int) _temp1201 > 1u?(( struct
_enum_struct*) _temp1201)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1204:
_temp1203=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1201)->f1; if(
_temp1203 == Cyc_Absyn_Unknown_b){ goto _LL1196;} else{ goto _LL1197;}} else{
goto _LL1197;} _LL1197: _LL1216: _temp1215= _temp1193.f1; if(( unsigned int)
_temp1215 > 1u?(( struct _enum_struct*) _temp1215)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1218: _temp1217=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1215)->f1;
if( _temp1217 == Cyc_Absyn_Unknown_b){ goto _LL1212;} else{ goto _LL1199;}}
else{ goto _LL1199;} _LL1212: _temp1211= _temp1193.f2; if(( unsigned int)
_temp1211 > 1u?(( struct _enum_struct*) _temp1211)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1214: _temp1213=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1211)->f1;
if( _temp1213 == Cyc_Absyn_Unknown_b){ goto _LL1198;} else{ goto _LL1199;}}
else{ goto _LL1199;} _LL1199: goto _LL1200; _LL1196: okay= 1; goto _LL1194;
_LL1198: okay= 1; goto _LL1194; _LL1200: okay= 0; goto _LL1194; _LL1194:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1164.elt_typ,( void*) _temp1160.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1164.rgn_typ,( void*) _temp1160.rgn_typ): 0)?
!( _temp1164.tq)->q_const? 1:( _temp1160.tq)->q_const: 0;} _LL1155: { int okay;
okay=( _temp1176 != 0? _temp1168 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1219= _temp1176; if(
_temp1219 == 0){ _throw( Null_Exception);} _temp1219;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1220= _temp1168; if(
_temp1220 == 0){ _throw( Null_Exception);} _temp1220;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1180, _temp1172): 0)? ! _temp1178->q_const? 1: _temp1170->q_const: 0;}
_LL1157: return Cyc_Tcutil_unify( t1, t2); _LL1151:;}} int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress((
void*)( e->topt)->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char* _temp1221=(
char*)"integral size mismatch; conversion supplied"; struct _tagged_string
_temp1222; _temp1222.curr= _temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one=
_temp1221 + 44; _temp1222;}));} Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1223= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1224= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1223.last_plus_one - _temp1223.curr, _temp1223.curr, _temp1224.last_plus_one
- _temp1224.curr, _temp1224.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v);
void* t;{ void* _temp1225= old_typ; void* _temp1235; void* _temp1237; _LL1227:
if(( unsigned int) _temp1225 > 4u?(( struct _enum_struct*) _temp1225)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL1238: _temp1237=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1225)->f1; goto _LL1236; _LL1236: _temp1235=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1225)->f2; goto _LL1228;} else{ goto _LL1229;} _LL1229: if( _temp1225 ==
Cyc_Absyn_FloatType){ goto _LL1230;} else{ goto _LL1231;} _LL1231: if( _temp1225
== Cyc_Absyn_DoubleType){ goto _LL1232;} else{ goto _LL1233;} _LL1233: goto
_LL1234; _LL1228:{ void* _temp1239= _temp1235; _LL1241: if( _temp1239 == Cyc_Absyn_B1){
goto _LL1242;} else{ goto _LL1243;} _LL1243: if( _temp1239 == Cyc_Absyn_B2){
goto _LL1244;} else{ goto _LL1245;} _LL1245: if( _temp1239 == Cyc_Absyn_B4){
goto _LL1246;} else{ goto _LL1247;} _LL1247: if( _temp1239 == Cyc_Absyn_B8){
goto _LL1248;} else{ goto _LL1240;} _LL1242: t= Cyc_Absyn_sint_t; goto _LL1240;
_LL1244: t= Cyc_Absyn_sint_t; goto _LL1240; _LL1246: t= _temp1237 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1240; _LL1248: t= _temp1237 == Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1240; _LL1240:;} goto _LL1226;
_LL1230: t= Cyc_Absyn_float_t; goto _LL1226; _LL1232: t= Cyc_Absyn_double_t;
goto _LL1226; _LL1234: return 0; _LL1226:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc,
t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1249= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1250= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1249.last_plus_one - _temp1249.curr, _temp1249.curr, _temp1250.last_plus_one
- _temp1250.curr, _temp1250.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1251= Cyc_Tcutil_compress( t); void* _temp1261; void*
_temp1263; _LL1253: if(( unsigned int) _temp1251 > 4u?(( struct _enum_struct*)
_temp1251)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1264: _temp1263=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1251)->f1; goto _LL1262; _LL1262:
_temp1261=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1251)->f2; goto
_LL1254;} else{ goto _LL1255;} _LL1255: if( _temp1251 == Cyc_Absyn_FloatType){
goto _LL1256;} else{ goto _LL1257;} _LL1257: if( _temp1251 == Cyc_Absyn_DoubleType){
goto _LL1258;} else{ goto _LL1259;} _LL1259: goto _LL1260; _LL1254: return 1;
_LL1256: return 1; _LL1258: return 1; _LL1260: return 0; _LL1252:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1265=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1265->f1= x->tq; _temp1265->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1265;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1266= t1; struct Cyc_List_List* _temp1276; struct Cyc_Absyn_Structdecl**
_temp1278; struct Cyc_List_List* _temp1280; struct _tuple1* _temp1282; _LL1268:
if( _temp1266 == Cyc_Absyn_VoidType){ goto _LL1269;} else{ goto _LL1270;}
_LL1270: if(( unsigned int) _temp1266 > 4u?(( struct _enum_struct*) _temp1266)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1277: _temp1276=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1266)->f1; goto _LL1271;} else{ goto
_LL1272;} _LL1272: if(( unsigned int) _temp1266 > 4u?(( struct _enum_struct*)
_temp1266)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1283: _temp1282=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1266)->f1; goto _LL1281;
_LL1281: _temp1280=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1266)->f2; goto _LL1279; _LL1279: _temp1278=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1266)->f3; goto _LL1273;} else{ goto
_LL1274;} _LL1274: goto _LL1275; _LL1269: return 0; _LL1271: return _temp1276;
_LL1273: if( _temp1278 == 0? 1: _temp1282 == 0){ return({ struct Cyc_List_List*
_temp1284=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1284->hd=( void*)({ struct _tuple4* _temp1285=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1285->f1= Cyc_Absyn_empty_tqual(); _temp1285->f2=
t1; _temp1285;}); _temp1284->tl= 0; _temp1284;});}{ struct Cyc_Absyn_Structdecl*
sd=* _temp1278; struct _tuple1 n=* _temp1282; struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1280); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1286=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1286->hd=( void*)({ struct _tuple4* _temp1287=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1287->f1= Cyc_Absyn_empty_tqual(); _temp1287->f2=
t1; _temp1287;}); _temp1286->tl= 0; _temp1286;});} return(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
inst,( struct Cyc_List_List*)( sd->fields)->v);} _LL1275: return({ struct Cyc_List_List*
_temp1288=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1288->hd=( void*)({ struct _tuple4* _temp1289=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1289->f1= Cyc_Absyn_empty_tqual(); _temp1289->f2=
t1; _temp1289;}); _temp1288->tl= 0; _temp1288;}); _LL1267:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2);
static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1, t2)){ return 1;}{ struct
Cyc_List_List* a= assume; for( 0; a != 0; a= a->tl){ if( Cyc_Tcutil_unify( t1,(*((
struct _tuple7*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*(( struct _tuple7*) a->hd)).f2):
0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1291=({ struct _tuple7 _temp1290; _temp1290.f1= t1;
_temp1290.f2= t2; _temp1290;}); void* _temp1297; struct Cyc_Absyn_PtrInfo
_temp1299; struct Cyc_Absyn_Conref* _temp1301; struct Cyc_Absyn_Tqual* _temp1303;
struct Cyc_Absyn_Conref* _temp1305; void* _temp1307; void* _temp1309; void*
_temp1311; struct Cyc_Absyn_PtrInfo _temp1313; struct Cyc_Absyn_Conref*
_temp1315; struct Cyc_Absyn_Tqual* _temp1317; struct Cyc_Absyn_Conref* _temp1319;
void* _temp1321; void* _temp1323; _LL1293: _LL1312: _temp1311= _temp1291.f1; if((
unsigned int) _temp1311 > 4u?(( struct _enum_struct*) _temp1311)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1314: _temp1313=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1311)->f1; _LL1324: _temp1323=( void*) _temp1313.elt_typ; goto _LL1322;
_LL1322: _temp1321=( void*) _temp1313.rgn_typ; goto _LL1320; _LL1320: _temp1319=(
struct Cyc_Absyn_Conref*) _temp1313.nullable; goto _LL1318; _LL1318: _temp1317=(
struct Cyc_Absyn_Tqual*) _temp1313.tq; goto _LL1316; _LL1316: _temp1315=( struct
Cyc_Absyn_Conref*) _temp1313.bounds; goto _LL1298;} else{ goto _LL1295;} _LL1298:
_temp1297= _temp1291.f2; if(( unsigned int) _temp1297 > 4u?(( struct
_enum_struct*) _temp1297)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1300:
_temp1299=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1297)->f1; _LL1310: _temp1309=( void*) _temp1299.elt_typ; goto _LL1308;
_LL1308: _temp1307=( void*) _temp1299.rgn_typ; goto _LL1306; _LL1306: _temp1305=(
struct Cyc_Absyn_Conref*) _temp1299.nullable; goto _LL1304; _LL1304: _temp1303=(
struct Cyc_Absyn_Tqual*) _temp1299.tq; goto _LL1302; _LL1302: _temp1301=( struct
Cyc_Absyn_Conref*) _temp1299.bounds; goto _LL1294;} else{ goto _LL1295;} _LL1295:
goto _LL1296; _LL1294: if( _temp1317->q_const? ! _temp1303->q_const: 0){ return
0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1319,
_temp1305)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1319): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1305): 0){ return 0;}{ void* _temp1325= _temp1321; _LL1327: if( _temp1325
== Cyc_Absyn_HeapRgn){ goto _LL1328;} else{ goto _LL1329;} _LL1329: goto _LL1330;
_LL1328: goto _LL1326; _LL1330: Cyc_Tcutil_unify( _temp1321, _temp1307); goto
_LL1326; _LL1326:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1315, _temp1301)){ struct _tuple7 _temp1332=({ struct _tuple7 _temp1331;
_temp1331.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1315); _temp1331.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1301); _temp1331;}); void* _temp1340; void* _temp1342; struct Cyc_Absyn_Exp*
_temp1344; void* _temp1346; struct Cyc_Absyn_Exp* _temp1348; void* _temp1350;
struct Cyc_Absyn_Exp* _temp1352; _LL1334: _LL1343: _temp1342= _temp1332.f1; if((
unsigned int) _temp1342 > 1u?(( struct _enum_struct*) _temp1342)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1345: _temp1344=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1342)->f1; goto _LL1341;} else{ goto _LL1336;} _LL1341: _temp1340=
_temp1332.f2; if( _temp1340 == Cyc_Absyn_Unknown_b){ goto _LL1335;} else{ goto
_LL1336;} _LL1336: _LL1351: _temp1350= _temp1332.f1; if(( unsigned int)
_temp1350 > 1u?(( struct _enum_struct*) _temp1350)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1353: _temp1352=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1350)->f1; goto _LL1347;} else{ goto _LL1338;} _LL1347: _temp1346=
_temp1332.f2; if(( unsigned int) _temp1346 > 1u?(( struct _enum_struct*)
_temp1346)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1349: _temp1348=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1346)->f1; goto _LL1337;} else{ goto
_LL1338;} _LL1338: goto _LL1339; _LL1335: goto _LL1333; _LL1337: if( Cyc_Evexp_eval_const_uint_exp(
_temp1352) < Cyc_Evexp_eval_const_uint_exp( _temp1348)){ return 0;} goto _LL1333;
_LL1339: return 0; _LL1333:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1354=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1354->hd=( void*)({ struct _tuple7* _temp1355=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1355->f1= t1; _temp1355->f2= t2; _temp1355;});
_temp1354->tl= assume; _temp1354;}), _temp1323, _temp1309); _LL1296: return 0;
_LL1292:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2= tqs2->tl, tqs1= tqs1->tl){ if( tqs1 == 0){ return 0;}{ struct
_tuple4 _temp1358; void* _temp1359; struct Cyc_Absyn_Tqual* _temp1361; struct
_tuple4* _temp1356=( struct _tuple4*) tqs1->hd; _temp1358=* _temp1356; _LL1362:
_temp1361= _temp1358.f1; goto _LL1360; _LL1360: _temp1359= _temp1358.f2; goto
_LL1357; _LL1357: { struct _tuple4 _temp1365; void* _temp1366; struct Cyc_Absyn_Tqual*
_temp1368; struct _tuple4* _temp1363=( struct _tuple4*) tqs2->hd; _temp1365=*
_temp1363; _LL1369: _temp1368= _temp1365.f1; goto _LL1367; _LL1367: _temp1366=
_temp1365.f2; goto _LL1364; _LL1364: if( _temp1368->q_const? Cyc_Tcutil_subtype(
te, assume, _temp1359, _temp1366): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1359, _temp1366)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;}{ void* _temp1370= t2; struct Cyc_Absyn_Uniondecl**
_temp1376; struct Cyc_List_List* _temp1378; struct _tuple1* _temp1380; _LL1372:
if(( unsigned int) _temp1370 > 4u?(( struct _enum_struct*) _temp1370)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL1381: _temp1380=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1370)->f1; goto _LL1379; _LL1379: _temp1378=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1370)->f2; goto _LL1377; _LL1377:
_temp1376=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1370)->f3; goto _LL1373;} else{ goto _LL1374;} _LL1374: goto _LL1375;
_LL1373: if( _temp1376 == 0){ goto _LL1371;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp1376; if( ud->fields == 0){ goto _LL1371;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
ud->tvs, _temp1378);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 1;}}}
goto _LL1371;}} _LL1375: goto _LL1371; _LL1371:;}{ void* _temp1382= t1; struct
Cyc_Absyn_Enumdecl** _temp1398; struct Cyc_List_List* _temp1400; struct _tuple1*
_temp1402; struct Cyc_Absyn_PtrInfo _temp1404; struct Cyc_Absyn_Conref*
_temp1406; struct Cyc_Absyn_Tqual* _temp1408; struct Cyc_Absyn_Conref* _temp1410;
void* _temp1412; void* _temp1414; struct Cyc_Absyn_Exp* _temp1416; struct Cyc_Absyn_Tqual*
_temp1418; void* _temp1420; void* _temp1422; void* _temp1424; _LL1384: if((
unsigned int) _temp1382 > 4u?(( struct _enum_struct*) _temp1382)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1403: _temp1402=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1382)->f1; goto _LL1401; _LL1401: _temp1400=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1382)->f2; goto _LL1399; _LL1399:
_temp1398=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1382)->f3; goto _LL1385;} else{ goto _LL1386;} _LL1386: if(( unsigned int)
_temp1382 > 4u?(( struct _enum_struct*) _temp1382)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1405: _temp1404=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1382)->f1; _LL1415: _temp1414=( void*) _temp1404.elt_typ; goto _LL1413;
_LL1413: _temp1412=( void*) _temp1404.rgn_typ; goto _LL1411; _LL1411: _temp1410=(
struct Cyc_Absyn_Conref*) _temp1404.nullable; goto _LL1409; _LL1409: _temp1408=(
struct Cyc_Absyn_Tqual*) _temp1404.tq; goto _LL1407; _LL1407: _temp1406=( struct
Cyc_Absyn_Conref*) _temp1404.bounds; goto _LL1387;} else{ goto _LL1388;} _LL1388:
if(( unsigned int) _temp1382 > 4u?(( struct _enum_struct*) _temp1382)->tag ==
Cyc_Absyn_ArrayType_tag: 0){ _LL1421: _temp1420=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1382)->f1; goto _LL1419; _LL1419: _temp1418=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1382)->f2; goto _LL1417; _LL1417:
_temp1416=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1382)->f3; goto _LL1389;} else{ goto _LL1390;} _LL1390: if(( unsigned int)
_temp1382 > 4u?(( struct _enum_struct*) _temp1382)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1425: _temp1424=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1382)->f1;
goto _LL1423; _LL1423: _temp1422=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1382)->f2; goto _LL1391;} else{ goto _LL1392;} _LL1392: if( _temp1382 ==
Cyc_Absyn_FloatType){ goto _LL1393;} else{ goto _LL1394;} _LL1394: if( _temp1382
== Cyc_Absyn_DoubleType){ goto _LL1395;} else{ goto _LL1396;} _LL1396: goto
_LL1397; _LL1385: return Cyc_Tcutil_coerceable( t2); _LL1387: { void* _temp1426=
t2; struct Cyc_Absyn_PtrInfo _temp1432; struct Cyc_Absyn_Conref* _temp1434;
struct Cyc_Absyn_Tqual* _temp1436; struct Cyc_Absyn_Conref* _temp1438; void*
_temp1440; void* _temp1442; _LL1428: if(( unsigned int) _temp1426 > 4u?(( struct
_enum_struct*) _temp1426)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1433:
_temp1432=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1426)->f1; _LL1443: _temp1442=( void*) _temp1432.elt_typ; goto _LL1441;
_LL1441: _temp1440=( void*) _temp1432.rgn_typ; goto _LL1439; _LL1439: _temp1438=(
struct Cyc_Absyn_Conref*) _temp1432.nullable; goto _LL1437; _LL1437: _temp1436=(
struct Cyc_Absyn_Tqual*) _temp1432.tq; goto _LL1435; _LL1435: _temp1434=( struct
Cyc_Absyn_Conref*) _temp1432.bounds; goto _LL1429;} else{ goto _LL1430;} _LL1430:
goto _LL1431; _LL1429: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1444=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1444->hd=( void*)({ struct _tuple7* _temp1445=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1445->f1= t1; _temp1445->f2= t2; _temp1445;});
_temp1444->tl= 0; _temp1444;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te, assump,
_temp1414, _temp1442)? ! _temp1408->q_const? 1: _temp1436->q_const: 0; int
bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1406, _temp1434); if( ! bounds_ok){ struct _tuple7 _temp1447=({ struct
_tuple7 _temp1446; _temp1446.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1406); _temp1446.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1434); _temp1446;}); void* _temp1453; struct Cyc_Absyn_Exp* _temp1455; void*
_temp1457; struct Cyc_Absyn_Exp* _temp1459; _LL1449: _LL1458: _temp1457=
_temp1447.f1; if(( unsigned int) _temp1457 > 1u?(( struct _enum_struct*)
_temp1457)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1460: _temp1459=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1457)->f1; goto _LL1454;} else{ goto
_LL1451;} _LL1454: _temp1453= _temp1447.f2; if(( unsigned int) _temp1453 > 1u?((
struct _enum_struct*) _temp1453)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1456:
_temp1455=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1453)->f1;
goto _LL1450;} else{ goto _LL1451;} _LL1451: goto _LL1452; _LL1450: if( Cyc_Evexp_eval_const_uint_exp(
_temp1459) >= Cyc_Evexp_eval_const_uint_exp( _temp1455)){ bounds_ok= 1;} goto
_LL1448; _LL1452: bounds_ok= 1; goto _LL1448; _LL1448:;} return( bounds_ok?
ptrsub: 0)? _temp1412 == Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify( _temp1412,
_temp1440): 0;} _LL1431: return 0; _LL1427:;} _LL1389: { void* _temp1461= t2;
struct Cyc_Absyn_Exp* _temp1467; struct Cyc_Absyn_Tqual* _temp1469; void*
_temp1471; _LL1463: if(( unsigned int) _temp1461 > 4u?(( struct _enum_struct*)
_temp1461)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1472: _temp1471=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1461)->f1; goto _LL1470; _LL1470:
_temp1469=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1461)->f2; goto _LL1468; _LL1468: _temp1467=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1461)->f3; goto _LL1464;} else{ goto
_LL1465;} _LL1465: goto _LL1466; _LL1464: { int okay; okay=( _temp1416 != 0?
_temp1467 != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1473= _temp1416; if( _temp1473 == 0){ _throw(
Null_Exception);} _temp1473;})) >= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1474= _temp1467; if( _temp1474 == 0){ _throw(
Null_Exception);} _temp1474;})): 0; return( okay? Cyc_Tcutil_unify( _temp1420,
_temp1471): 0)? ! _temp1418->q_const? 1: _temp1469->q_const: 0;} _LL1466: return
0; _LL1462:;} _LL1391: goto _LL1393; _LL1393: goto _LL1395; _LL1395: return Cyc_Tcutil_coerceable(
t2); _LL1397: return 0; _LL1383:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)( e->topt)->v,
t)){ struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp1475=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1475->tag= Cyc_Absyn_Cast_e_tag;
_temp1475->f1=( void*) t; _temp1475->f2= inner;( void*) _temp1475;})); e->topt=({
struct Cyc_Core_Opt* _temp1476=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1476->v=( void*) t; _temp1476;});}} int Cyc_Tcutil_comparison_promote(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp1477=
Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp1487; void* _temp1489;
_LL1479: if(( unsigned int) _temp1477 > 4u?(( struct _enum_struct*) _temp1477)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL1490: _temp1489=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1477)->f1; goto _LL1488; _LL1488: _temp1487=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1477)->f2; goto _LL1480;} else{ goto _LL1481;} _LL1481: if( _temp1477 ==
Cyc_Absyn_FloatType){ goto _LL1482;} else{ goto _LL1483;} _LL1483: if( _temp1477
== Cyc_Absyn_DoubleType){ goto _LL1484;} else{ goto _LL1485;} _LL1485: goto
_LL1486; _LL1480:{ void* _temp1491= _temp1487; _LL1493: if( _temp1491 == Cyc_Absyn_B1){
goto _LL1494;} else{ goto _LL1495;} _LL1495: if( _temp1491 == Cyc_Absyn_B2){
goto _LL1496;} else{ goto _LL1497;} _LL1497: if( _temp1491 == Cyc_Absyn_B4){
goto _LL1498;} else{ goto _LL1499;} _LL1499: if( _temp1491 == Cyc_Absyn_B8){
goto _LL1500;} else{ goto _LL1492;} _LL1494: t= Cyc_Absyn_sint_t; goto _LL1492;
_LL1496: t= Cyc_Absyn_sint_t; goto _LL1492; _LL1498: t= _temp1489 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1492; _LL1500: return 1; _LL1492:;}
goto _LL1478; _LL1482: t= Cyc_Absyn_float_t; goto _LL1478; _LL1484: return 1;
_LL1486: return 0; _LL1478:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;}
int Cyc_Tcutil_arithmetic_promote( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* t;{ void*
_temp1501= old_typ; void* _temp1515; void* _temp1517; struct Cyc_Absyn_Enumdecl**
_temp1519; struct Cyc_List_List* _temp1521; struct _tuple1* _temp1523; int
_temp1525; struct Cyc_Core_Opt* _temp1527; void* _temp1529; _LL1503: if((
unsigned int) _temp1501 > 4u?(( struct _enum_struct*) _temp1501)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1518: _temp1517=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1501)->f1;
goto _LL1516; _LL1516: _temp1515=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1501)->f2; goto _LL1504;} else{ goto _LL1505;} _LL1505: if( _temp1501 ==
Cyc_Absyn_FloatType){ goto _LL1506;} else{ goto _LL1507;} _LL1507: if( _temp1501
== Cyc_Absyn_DoubleType){ goto _LL1508;} else{ goto _LL1509;} _LL1509: if((
unsigned int) _temp1501 > 4u?(( struct _enum_struct*) _temp1501)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1524: _temp1523=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1501)->f1; goto _LL1522; _LL1522: _temp1521=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1501)->f2; goto _LL1520; _LL1520:
_temp1519=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1501)->f3; goto _LL1510;} else{ goto _LL1511;} _LL1511: if(( unsigned int)
_temp1501 > 4u?(( struct _enum_struct*) _temp1501)->tag == Cyc_Absyn_Evar_tag: 0){
_LL1530: _temp1529=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1501)->f1;
goto _LL1528; _LL1528: _temp1527=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1501)->f2; goto _LL1526; _LL1526: _temp1525=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1501)->f3; goto _LL1512;} else{ goto _LL1513;} _LL1513: goto _LL1514;
_LL1504:{ void* _temp1531= _temp1515; _LL1533: if( _temp1531 == Cyc_Absyn_B1){
goto _LL1534;} else{ goto _LL1535;} _LL1535: if( _temp1531 == Cyc_Absyn_B2){
goto _LL1536;} else{ goto _LL1537;} _LL1537: if( _temp1531 == Cyc_Absyn_B4){
goto _LL1538;} else{ goto _LL1539;} _LL1539: if( _temp1531 == Cyc_Absyn_B8){
goto _LL1540;} else{ goto _LL1532;} _LL1534: t= Cyc_Absyn_sint_t; goto _LL1532;
_LL1536: t= Cyc_Absyn_sint_t; goto _LL1532; _LL1538: t= _temp1517 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1532; _LL1540: t= _temp1517 == Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1532; _LL1532:;} goto _LL1502;
_LL1506: t= Cyc_Absyn_float_t; goto _LL1502; _LL1508: t= Cyc_Absyn_double_t;
goto _LL1502; _LL1510: t= Cyc_Absyn_sint_t; goto _LL1502; _LL1512: t= Cyc_Absyn_sint_t;
if( ! Cyc_Tcutil_unify( old_typ, t)){ return 0;} goto _LL1502; _LL1514: return 0;
_LL1502:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;} int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* t;{ void* _temp1541= old_typ; void* _temp1551; void*
_temp1553; struct Cyc_Absyn_Enumdecl** _temp1555; struct Cyc_List_List*
_temp1557; struct _tuple1* _temp1559; int _temp1561; struct Cyc_Core_Opt*
_temp1563; void* _temp1565; _LL1543: if(( unsigned int) _temp1541 > 4u?(( struct
_enum_struct*) _temp1541)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1554: _temp1553=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1541)->f1; goto _LL1552; _LL1552:
_temp1551=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1541)->f2; goto
_LL1544;} else{ goto _LL1545;} _LL1545: if(( unsigned int) _temp1541 > 4u?((
struct _enum_struct*) _temp1541)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL1560:
_temp1559=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1541)->f1;
goto _LL1558; _LL1558: _temp1557=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1541)->f2; goto _LL1556; _LL1556: _temp1555=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp1541)->f3; goto _LL1546;} else{ goto
_LL1547;} _LL1547: if(( unsigned int) _temp1541 > 4u?(( struct _enum_struct*)
_temp1541)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1566: _temp1565=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp1541)->f1; goto _LL1564; _LL1564: _temp1563=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1541)->f2; goto
_LL1562; _LL1562: _temp1561=( int)(( struct Cyc_Absyn_Evar_struct*) _temp1541)->f3;
goto _LL1548;} else{ goto _LL1549;} _LL1549: goto _LL1550; _LL1544:{ void*
_temp1567= _temp1551; _LL1569: if( _temp1567 == Cyc_Absyn_B1){ goto _LL1570;}
else{ goto _LL1571;} _LL1571: if( _temp1567 == Cyc_Absyn_B2){ goto _LL1572;}
else{ goto _LL1573;} _LL1573: if( _temp1567 == Cyc_Absyn_B4){ goto _LL1574;}
else{ goto _LL1575;} _LL1575: if( _temp1567 == Cyc_Absyn_B8){ goto _LL1576;}
else{ goto _LL1568;} _LL1570: t= Cyc_Absyn_sint_t; goto _LL1568; _LL1572: t= Cyc_Absyn_sint_t;
goto _LL1568; _LL1574: t= _temp1553 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL1568; _LL1576: t= _temp1553 == Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL1568; _LL1568:;} goto _LL1542; _LL1546: t= Cyc_Absyn_sint_t;
goto _LL1542; _LL1548: t= Cyc_Absyn_sint_t; if( ! Cyc_Tcutil_unify( old_typ, t)){
return 0;} goto _LL1542; _LL1550: return 0; _LL1542:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple7 _temp1578=({ struct _tuple7 _temp1577; _temp1577.f1= t1;
_temp1577.f2= t2; _temp1577;}); void* _temp1602; void* _temp1604; void*
_temp1606; void* _temp1608; void* _temp1610; void* _temp1612; void* _temp1614;
void* _temp1616; void* _temp1618; void* _temp1620; void* _temp1622; void*
_temp1624; void* _temp1626; void* _temp1628; void* _temp1630; void* _temp1632;
void* _temp1634; void* _temp1636; void* _temp1638; void* _temp1640; void*
_temp1642; void* _temp1644; void* _temp1646; void* _temp1648; void* _temp1650;
void* _temp1652; void* _temp1654; void* _temp1656; void* _temp1658; void*
_temp1660; void* _temp1662; void* _temp1664; _LL1580: _LL1605: _temp1604=
_temp1578.f1; if( _temp1604 == Cyc_Absyn_DoubleType){ goto _LL1603;} else{ goto
_LL1582;} _LL1603: _temp1602= _temp1578.f2; goto _LL1581; _LL1582: _LL1609:
_temp1608= _temp1578.f1; goto _LL1607; _LL1607: _temp1606= _temp1578.f2; if(
_temp1606 == Cyc_Absyn_DoubleType){ goto _LL1583;} else{ goto _LL1584;} _LL1584:
_LL1613: _temp1612= _temp1578.f1; if( _temp1612 == Cyc_Absyn_FloatType){ goto
_LL1611;} else{ goto _LL1586;} _LL1611: _temp1610= _temp1578.f2; goto _LL1585;
_LL1586: _LL1617: _temp1616= _temp1578.f1; goto _LL1615; _LL1615: _temp1614=
_temp1578.f2; if( _temp1614 == Cyc_Absyn_FloatType){ goto _LL1587;} else{ goto
_LL1588;} _LL1588: _LL1621: _temp1620= _temp1578.f1; if(( unsigned int)
_temp1620 > 4u?(( struct _enum_struct*) _temp1620)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1625: _temp1624=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1620)->f1;
if( _temp1624 == Cyc_Absyn_Unsigned){ goto _LL1623;} else{ goto _LL1590;}
_LL1623: _temp1622=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1620)->f2;
if( _temp1622 == Cyc_Absyn_B8){ goto _LL1619;} else{ goto _LL1590;}} else{ goto
_LL1590;} _LL1619: _temp1618= _temp1578.f2; goto _LL1589; _LL1590: _LL1633:
_temp1632= _temp1578.f1; goto _LL1627; _LL1627: _temp1626= _temp1578.f2; if((
unsigned int) _temp1626 > 4u?(( struct _enum_struct*) _temp1626)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1631: _temp1630=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1626)->f1;
if( _temp1630 == Cyc_Absyn_Unsigned){ goto _LL1629;} else{ goto _LL1592;}
_LL1629: _temp1628=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1626)->f2;
if( _temp1628 == Cyc_Absyn_B8){ goto _LL1591;} else{ goto _LL1592;}} else{ goto
_LL1592;} _LL1592: _LL1637: _temp1636= _temp1578.f1; if(( unsigned int)
_temp1636 > 4u?(( struct _enum_struct*) _temp1636)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1641: _temp1640=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f1;
if( _temp1640 == Cyc_Absyn_Signed){ goto _LL1639;} else{ goto _LL1594;} _LL1639:
_temp1638=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f2; if(
_temp1638 == Cyc_Absyn_B8){ goto _LL1635;} else{ goto _LL1594;}} else{ goto
_LL1594;} _LL1635: _temp1634= _temp1578.f2; goto _LL1593; _LL1594: _LL1649:
_temp1648= _temp1578.f1; goto _LL1643; _LL1643: _temp1642= _temp1578.f2; if((
unsigned int) _temp1642 > 4u?(( struct _enum_struct*) _temp1642)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1647: _temp1646=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1642)->f1;
if( _temp1646 == Cyc_Absyn_Signed){ goto _LL1645;} else{ goto _LL1596;} _LL1645:
_temp1644=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1642)->f2; if(
_temp1644 == Cyc_Absyn_B8){ goto _LL1595;} else{ goto _LL1596;}} else{ goto
_LL1596;} _LL1596: _LL1653: _temp1652= _temp1578.f1; if(( unsigned int)
_temp1652 > 4u?(( struct _enum_struct*) _temp1652)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1657: _temp1656=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1652)->f1;
if( _temp1656 == Cyc_Absyn_Unsigned){ goto _LL1655;} else{ goto _LL1598;}
_LL1655: _temp1654=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1652)->f2;
if( _temp1654 == Cyc_Absyn_B4){ goto _LL1651;} else{ goto _LL1598;}} else{ goto
_LL1598;} _LL1651: _temp1650= _temp1578.f2; goto _LL1597; _LL1598: _LL1665:
_temp1664= _temp1578.f1; goto _LL1659; _LL1659: _temp1658= _temp1578.f2; if((
unsigned int) _temp1658 > 4u?(( struct _enum_struct*) _temp1658)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1663: _temp1662=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1658)->f1;
if( _temp1662 == Cyc_Absyn_Unsigned){ goto _LL1661;} else{ goto _LL1600;}
_LL1661: _temp1660=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1658)->f2;
if( _temp1660 == Cyc_Absyn_B4){ goto _LL1599;} else{ goto _LL1600;}} else{ goto
_LL1600;} _LL1600: goto _LL1601; _LL1581: goto _LL1583; _LL1583: return Cyc_Absyn_DoubleType;
_LL1585: goto _LL1587; _LL1587: return Cyc_Absyn_FloatType; _LL1589: goto
_LL1591; _LL1591: return({ struct Cyc_Absyn_IntType_struct* _temp1666=( struct
Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1666->tag= Cyc_Absyn_IntType_tag; _temp1666->f1=( void*) Cyc_Absyn_Unsigned;
_temp1666->f2=( void*) Cyc_Absyn_B8;( void*) _temp1666;}); _LL1593: goto _LL1595;
_LL1595: return({ struct Cyc_Absyn_IntType_struct* _temp1667=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1667->tag= Cyc_Absyn_IntType_tag;
_temp1667->f1=( void*) Cyc_Absyn_Signed; _temp1667->f2=( void*) Cyc_Absyn_B8;(
void*) _temp1667;}); _LL1597: goto _LL1599; _LL1599: return({ struct Cyc_Absyn_IntType_struct*
_temp1668=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1668->tag= Cyc_Absyn_IntType_tag; _temp1668->f1=( void*) Cyc_Absyn_Unsigned;
_temp1668->f2=( void*) Cyc_Absyn_B4;( void*) _temp1668;}); _LL1601: return({
struct Cyc_Absyn_IntType_struct* _temp1669=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1669->tag= Cyc_Absyn_IntType_tag;
_temp1669->f1=( void*) Cyc_Absyn_Signed; _temp1669->f2=( void*) Cyc_Absyn_B4;(
void*) _temp1669;}); _LL1579:;} void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp* e){ void* _temp1670=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1676; struct Cyc_Core_Opt* _temp1678; struct Cyc_Absyn_Exp* _temp1680;
_LL1672: if((( struct _enum_struct*) _temp1670)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1681: _temp1680=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1670)->f1; goto _LL1679; _LL1679: _temp1678=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1670)->f2; if( _temp1678 == 0){ goto
_LL1677;} else{ goto _LL1674;} _LL1677: _temp1676=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1670)->f3; goto _LL1673;} else{ goto
_LL1674;} _LL1674: goto _LL1675; _LL1673: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1682=( char*)"assignment in test"; struct
_tagged_string _temp1683; _temp1683.curr= _temp1682; _temp1683.base= _temp1682;
_temp1683.last_plus_one= _temp1682 + 19; _temp1683;})); goto _LL1671; _LL1675:
goto _LL1671; _LL1671:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v == Cyc_Absyn_No_constr){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1684=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1684->tag= Cyc_Absyn_Forward_constr_tag;
_temp1684->f1= c2;( void*) _temp1684;})); return 1;} else{ if(( void*) c2->v ==
Cyc_Absyn_No_constr){( void*)( c2->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1685=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1685->tag= Cyc_Absyn_Forward_constr_tag;
_temp1685->f1= c1;( void*) _temp1685;})); return 1;} else{ void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq(
k1, k2)){( void*)( c2->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1686=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1686->tag= Cyc_Absyn_Forward_constr_tag;
_temp1686->f1= c1;( void*) _temp1686;})); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1687=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1687->tag= Cyc_Absyn_Forward_constr_tag;
_temp1687->f1= c2;( void*) _temp1687;})); return 1;} else{ return 0;}}}}} static
struct Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tvs == 0){ return({
struct Cyc_List_List* _temp1688=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1688->hd=( void*) tv; _temp1688->tl= 0; _temp1688;});}
else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ if(
Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) == 0){
struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct
Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1689=* tv->name; struct _tagged_string
_temp1690= Cyc_Absynpp_ckind2string( k1); struct _tagged_string _temp1691= Cyc_Absynpp_ckind2string(
k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1689.last_plus_one - _temp1689.curr, _temp1689.curr, _temp1690.last_plus_one
- _temp1690.curr, _temp1690.curr, _temp1691.last_plus_one - _temp1691.curr,
_temp1691.curr);}));} return tvs;} else{ if( tvs2->tl == 0){ tvs2->tl=({ struct
Cyc_List_List* _temp1692=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1692->hd=( void*) tv; _temp1692->tl= 0; _temp1692;}); return tvs;}}}}
return(( struct Cyc_List_List*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1693=( char*)"Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp1694; _temp1694.curr= _temp1693; _temp1694.base=
_temp1693; _temp1694.last_plus_one= _temp1693 + 50; _temp1694;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=
tvs->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b= b->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs->hd)->name,(( struct
Cyc_Absyn_Tvar*) b->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp1695=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1695->hd=( void*)(( struct Cyc_Absyn_Tvar*) tvs->hd);
_temp1695->tl= r; _temp1695;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} static int Cyc_Tcutil_tvar_counter= 0;
struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k){ struct _tagged_string s=
xprintf("#%d",( Cyc_Tcutil_tvar_counter ++)); return({ struct Cyc_Absyn_Tvar*
_temp1696=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp1696->name=({ struct _tagged_string* _temp1697=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1697[ 0]= s; _temp1697;});
_temp1696->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp1696;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_string s=* t->name; return( int)({ struct _tagged_string
_temp1698= s; char* _temp1700= _temp1698.curr + 0; if( _temp1700 < _temp1698.base?
1: _temp1700 >= _temp1698.last_plus_one){ _throw( Null_Exception);}* _temp1700;})
==( int)'#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if(
! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_string s= Cyc_String_strconcat((
struct _tagged_string)({ char* _temp1701=( char*)"`"; struct _tagged_string
_temp1702; _temp1702.curr= _temp1701; _temp1702.base= _temp1701; _temp1702.last_plus_one=
_temp1701 + 2; _temp1702;}),* t->name);({ struct _tagged_string _temp1703= s;
char* _temp1705= _temp1703.curr + 1; if( _temp1705 < _temp1703.base? 1:
_temp1705 >= _temp1703.last_plus_one){ _throw( Null_Exception);}* _temp1705='t';});
t->name=({ struct _tagged_string* _temp1706=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1706[ 0]= s; _temp1706;});}} static
struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({ struct
_tuple2* _temp1707=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1707->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1708=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1708->v=( void*)(*
x).f1; _temp1708;}); _temp1707->f2=(* x).f2; _temp1707->f3=(* x).f3; _temp1707;});}
int Cyc_Tcutil_fntype_att( void* a){ void* _temp1709= a; int _temp1723; _LL1711:
if(( unsigned int) _temp1709 > 15u?(( struct _enum_struct*) _temp1709)->tag ==
Cyc_Absyn_Regparm_att_tag: 0){ _LL1724: _temp1723=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp1709)->f1; goto _LL1712;} else{ goto _LL1713;} _LL1713: if( _temp1709 ==
Cyc_Absyn_Stdcall_att){ goto _LL1714;} else{ goto _LL1715;} _LL1715: if(
_temp1709 == Cyc_Absyn_Cdecl_att){ goto _LL1716;} else{ goto _LL1717;} _LL1717:
if( _temp1709 == Cyc_Absyn_Noreturn_att){ goto _LL1718;} else{ goto _LL1719;}
_LL1719: if( _temp1709 == Cyc_Absyn_Const_att){ goto _LL1720;} else{ goto
_LL1721;} _LL1721: goto _LL1722; _LL1712: goto _LL1714; _LL1714: goto _LL1716;
_LL1716: goto _LL1718; _LL1718: goto _LL1720; _LL1720: return 1; _LL1722: return
0; _LL1710:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ
== 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts= atts->tl){ if( Cyc_Tcutil_fntype_att(( void*) atts->hd)){
fn_type_atts=({ struct Cyc_List_List* _temp1725=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1725->hd=( void*)(( void*) atts->hd);
_temp1725->tl= fn_type_atts; _temp1725;});}}} return({ struct Cyc_Absyn_FnType_struct*
_temp1726=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1726->tag= Cyc_Absyn_FnType_tag; _temp1726->f1=({ struct Cyc_Absyn_FnInfo
_temp1727; _temp1727.tvars= fd->tvs; _temp1727.effect= fd->effect; _temp1727.ret_typ=(
void*)(( void*) fd->ret_type); _temp1727.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple11*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp1727.varargs= fd->varargs; _temp1727.attributes=
fn_type_atts; _temp1727;});( void*) _temp1726;});} return( void*)( fd->cached_typ)->v;}
static void* Cyc_Tcutil_fst3( struct _tuple12* t){ return(* t).f1;} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp1728= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1734; struct Cyc_List_List* _temp1736; int _temp1738; struct Cyc_List_List*
_temp1740; void* _temp1742; struct Cyc_Core_Opt* _temp1744; struct Cyc_List_List*
_temp1746; _LL1730: if(( unsigned int) _temp1728 > 4u?(( struct _enum_struct*)
_temp1728)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1735: _temp1734=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1728)->f1; _LL1747: _temp1746=( struct Cyc_List_List*)
_temp1734.tvars; goto _LL1745; _LL1745: _temp1744=( struct Cyc_Core_Opt*)
_temp1734.effect; goto _LL1743; _LL1743: _temp1742=( void*) _temp1734.ret_typ;
goto _LL1741; _LL1741: _temp1740=( struct Cyc_List_List*) _temp1734.args; goto
_LL1739; _LL1739: _temp1738=( int) _temp1734.varargs; goto _LL1737; _LL1737:
_temp1736=( struct Cyc_List_List*) _temp1734.attributes; goto _LL1731;} else{
goto _LL1732;} _LL1732: goto _LL1733; _LL1731: fd->tvs= _temp1746; fd->effect=
_temp1744; goto _LL1729; _LL1733:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1748=( char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp1749; _temp1749.curr= _temp1748; _temp1749.base=
_temp1748; _temp1749.last_plus_one= _temp1748 + 38; _temp1749;})); return;
_LL1729:;} Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct _tuple11*), struct Cyc_List_List* x)) Cyc_List_map)((
struct _tagged_string*(*)( struct _tuple11* t)) Cyc_Tcutil_fst3, fd->args), loc,(
struct _tagged_string)({ char* _temp1750=( char*)"function declaration has repeated parameter";
struct _tagged_string _temp1751; _temp1751.curr= _temp1750; _temp1751.base=
_temp1750; _temp1751.last_plus_one= _temp1750 + 44; _temp1751;})); fd->cached_typ=({
struct Cyc_Core_Opt* _temp1752=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1752->v=( void*) t; _temp1752;});} static struct _tuple14*
Cyc_Tcutil_substitute_f1( struct _tuple2* y){ return({ struct _tuple14*
_temp1753=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1753->f1=({
struct _tuple13* _temp1754=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp1754->f1=(* y).f1; _temp1754->f2=(* y).f2; _temp1754;});
_temp1753->f2=(* y).f3; _temp1753;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp1757; struct _tuple13* _temp1759; struct
_tuple14 _temp1755=* w; _LL1760: _temp1759= _temp1755.f1; goto _LL1758; _LL1758:
_temp1757= _temp1755.f2; goto _LL1756; _LL1756: { struct Cyc_Absyn_Tqual*
_temp1763; struct Cyc_Core_Opt* _temp1765; struct _tuple13 _temp1761=* _temp1759;
_LL1766: _temp1765= _temp1761.f1; goto _LL1764; _LL1764: _temp1763= _temp1761.f2;
goto _LL1762; _LL1762: return({ struct _tuple2* _temp1767=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp1767->f1= _temp1765; _temp1767->f2=
_temp1763; _temp1767->f3= _temp1757; _temp1767;});}} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts); void*
Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ void* _temp1768=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp1808; struct Cyc_Absyn_Structdecl**
_temp1810; struct Cyc_List_List* _temp1812; struct _tuple1* _temp1814; struct
Cyc_Absyn_Uniondecl** _temp1816; struct Cyc_List_List* _temp1818; struct _tuple1*
_temp1820; struct Cyc_Absyn_Enumdecl** _temp1822; struct Cyc_List_List*
_temp1824; struct _tuple1* _temp1826; struct Cyc_Core_Opt* _temp1828; struct Cyc_List_List*
_temp1830; struct _tuple1* _temp1832; struct Cyc_Absyn_Exp* _temp1834; struct
Cyc_Absyn_Tqual* _temp1836; void* _temp1838; struct Cyc_Absyn_PtrInfo _temp1840;
struct Cyc_Absyn_Conref* _temp1842; struct Cyc_Absyn_Tqual* _temp1844; struct
Cyc_Absyn_Conref* _temp1846; void* _temp1848; void* _temp1850; struct Cyc_Absyn_FnInfo
_temp1852; struct Cyc_List_List* _temp1854; int _temp1856; struct Cyc_List_List*
_temp1858; void* _temp1860; struct Cyc_Core_Opt* _temp1862; struct Cyc_List_List*
_temp1864; struct Cyc_List_List* _temp1866; int _temp1868; struct Cyc_Core_Opt*
_temp1870; void* _temp1872; void* _temp1874; struct Cyc_Absyn_Xenumdecl**
_temp1876; struct _tuple1* _temp1878; void* _temp1880; void* _temp1882; void*
_temp1884; struct Cyc_List_List* _temp1886; _LL1770: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_VarType_tag:
0){ _LL1809: _temp1808=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1768)->f1; goto _LL1771;} else{ goto _LL1772;} _LL1772: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_StructType_tag:
0){ _LL1815: _temp1814=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1768)->f1; goto _LL1813; _LL1813: _temp1812=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1768)->f2; goto _LL1811; _LL1811:
_temp1810=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1768)->f3; goto _LL1773;} else{ goto _LL1774;} _LL1774: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL1821: _temp1820=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1768)->f1; goto _LL1819; _LL1819: _temp1818=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1768)->f2; goto _LL1817; _LL1817:
_temp1816=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1768)->f3; goto _LL1775;} else{ goto _LL1776;} _LL1776: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1827: _temp1826=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1768)->f1; goto _LL1825; _LL1825: _temp1824=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1768)->f2; goto _LL1823; _LL1823:
_temp1822=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1768)->f3; goto _LL1777;} else{ goto _LL1778;} _LL1778: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL1833: _temp1832=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1768)->f1; goto _LL1831; _LL1831: _temp1830=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1768)->f2; goto _LL1829; _LL1829:
_temp1828=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1768)->f3; goto _LL1779;} else{ goto _LL1780;} _LL1780: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1839: _temp1838=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1768)->f1;
goto _LL1837; _LL1837: _temp1836=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1768)->f2; goto _LL1835; _LL1835: _temp1834=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1768)->f3; goto _LL1781;} else{ goto
_LL1782;} _LL1782: if(( unsigned int) _temp1768 > 4u?(( struct _enum_struct*)
_temp1768)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1841: _temp1840=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1768)->f1;
_LL1851: _temp1850=( void*) _temp1840.elt_typ; goto _LL1849; _LL1849: _temp1848=(
void*) _temp1840.rgn_typ; goto _LL1847; _LL1847: _temp1846=( struct Cyc_Absyn_Conref*)
_temp1840.nullable; goto _LL1845; _LL1845: _temp1844=( struct Cyc_Absyn_Tqual*)
_temp1840.tq; goto _LL1843; _LL1843: _temp1842=( struct Cyc_Absyn_Conref*)
_temp1840.bounds; goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1853: _temp1852=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1768)->f1; _LL1865: _temp1864=( struct Cyc_List_List*) _temp1852.tvars;
goto _LL1863; _LL1863: _temp1862=( struct Cyc_Core_Opt*) _temp1852.effect; goto
_LL1861; _LL1861: _temp1860=( void*) _temp1852.ret_typ; goto _LL1859; _LL1859:
_temp1858=( struct Cyc_List_List*) _temp1852.args; goto _LL1857; _LL1857:
_temp1856=( int) _temp1852.varargs; goto _LL1855; _LL1855: _temp1854=( struct
Cyc_List_List*) _temp1852.attributes; goto _LL1785;} else{ goto _LL1786;}
_LL1786: if(( unsigned int) _temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1867: _temp1866=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1768)->f1; goto _LL1787;} else{ goto
_LL1788;} _LL1788: if(( unsigned int) _temp1768 > 4u?(( struct _enum_struct*)
_temp1768)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1873: _temp1872=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp1768)->f1; goto _LL1871; _LL1871: _temp1870=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1768)->f2; goto
_LL1869; _LL1869: _temp1868=( int)(( struct Cyc_Absyn_Evar_struct*) _temp1768)->f3;
goto _LL1789;} else{ goto _LL1790;} _LL1790: if(( unsigned int) _temp1768 > 4u?((
struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL1875: _temp1874=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1768)->f1;
goto _LL1791;} else{ goto _LL1792;} _LL1792: if( _temp1768 == Cyc_Absyn_VoidType){
goto _LL1793;} else{ goto _LL1794;} _LL1794: if(( unsigned int) _temp1768 > 4u?((
struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL1879:
_temp1878=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp1768)->f1;
goto _LL1877; _LL1877: _temp1876=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp1768)->f2; goto _LL1795;} else{ goto _LL1796;} _LL1796: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1883: _temp1882=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1768)->f1;
goto _LL1881; _LL1881: _temp1880=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1768)->f2; goto _LL1797;} else{ goto _LL1798;} _LL1798: if( _temp1768 ==
Cyc_Absyn_FloatType){ goto _LL1799;} else{ goto _LL1800;} _LL1800: if( _temp1768
== Cyc_Absyn_DoubleType){ goto _LL1801;} else{ goto _LL1802;} _LL1802: if(
_temp1768 == Cyc_Absyn_HeapRgn){ goto _LL1803;} else{ goto _LL1804;} _LL1804:
if(( unsigned int) _temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag ==
Cyc_Absyn_AccessEff_tag: 0){ _LL1885: _temp1884=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1768)->f1; goto _LL1805;} else{ goto _LL1806;} _LL1806: if(( unsigned int)
_temp1768 > 4u?(( struct _enum_struct*) _temp1768)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL1887: _temp1886=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1768)->f1; goto _LL1807;} else{ goto _LL1769;} _LL1771: { struct
_handler_cons _temp1888; _push_handler(& _temp1888);{ struct _xenum_struct*
_temp1889=( struct _xenum_struct*) setjmp( _temp1888.handler); if( ! _temp1889){{
void* _temp1890=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp1808); _npop_handler( 0u); return _temp1890;}; _pop_handler();} else{
struct _xenum_struct* _temp1892= _temp1889; _LL1894: if( _temp1892->tag == Cyc_Core_Not_found_tag){
goto _LL1895;} else{ goto _LL1896;} _LL1896: goto _LL1897; _LL1895: return t;
_LL1897:( void) _throw( _temp1892); _LL1893:;}}} _LL1773: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( inst, _temp1812); return new_ts == _temp1812? t:({
struct Cyc_Absyn_StructType_struct* _temp1898=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1898->tag= Cyc_Absyn_StructType_tag;
_temp1898->f1= _temp1814; _temp1898->f2= new_ts; _temp1898->f3= _temp1810;( void*)
_temp1898;});} _LL1775: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( inst,
_temp1818); return new_ts == _temp1818? t:({ struct Cyc_Absyn_UnionType_struct*
_temp1899=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1899->tag= Cyc_Absyn_UnionType_tag; _temp1899->f1= _temp1820; _temp1899->f2=
new_ts; _temp1899->f3= _temp1816;( void*) _temp1899;});} _LL1777: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( inst, _temp1824); return new_ts == _temp1824? t:({
struct Cyc_Absyn_EnumType_struct* _temp1900=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1900->tag= Cyc_Absyn_EnumType_tag;
_temp1900->f1= _temp1826; _temp1900->f2= new_ts; _temp1900->f3= _temp1822;( void*)
_temp1900;});} _LL1779: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( inst,
_temp1830); return new_ts == _temp1830? t:({ struct Cyc_Absyn_TypedefType_struct*
_temp1901=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp1901->tag= Cyc_Absyn_TypedefType_tag; _temp1901->f1= _temp1832; _temp1901->f2=
new_ts; _temp1901->f3= _temp1828;( void*) _temp1901;});} _LL1781: { void* new_t1=
Cyc_Tcutil_substitute( inst, _temp1838); return new_t1 == _temp1838? t:({ struct
Cyc_Absyn_ArrayType_struct* _temp1902=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1902->tag= Cyc_Absyn_ArrayType_tag;
_temp1902->f1=( void*) new_t1; _temp1902->f2= _temp1836; _temp1902->f3=
_temp1834;( void*) _temp1902;});} _LL1783: { void* new_t1= Cyc_Tcutil_substitute(
inst, _temp1850); void* new_r= Cyc_Tcutil_substitute( inst, _temp1848); if(
new_t1 == _temp1850? new_r == _temp1848: 0){ return t;} return({ struct Cyc_Absyn_PointerType_struct*
_temp1903=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1903->tag= Cyc_Absyn_PointerType_tag; _temp1903->f1=({ struct Cyc_Absyn_PtrInfo
_temp1904; _temp1904.elt_typ=( void*) new_t1; _temp1904.rgn_typ=( void*) new_r;
_temp1904.nullable= _temp1846; _temp1904.tq= _temp1844; _temp1904.bounds=
_temp1842; _temp1904;});( void*) _temp1903;});} _LL1785:{ struct Cyc_List_List*
p= _temp1864; for( 0; p != 0; p= p->tl){ inst=({ struct Cyc_List_List* _temp1905=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1905->hd=(
void*)({ struct _tuple5* _temp1906=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp1906->f1=( struct Cyc_Absyn_Tvar*) p->hd; _temp1906->f2=({
struct Cyc_Absyn_VarType_struct* _temp1907=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1907->tag= Cyc_Absyn_VarType_tag;
_temp1907->f1=( struct Cyc_Absyn_Tvar*) p->hd;( void*) _temp1907;}); _temp1906;});
_temp1905->tl= inst; _temp1905;});}}{ struct Cyc_List_List* _temp1910; struct
Cyc_List_List* _temp1912; struct _tuple0 _temp1908=(( struct _tuple0(*)( struct
Cyc_List_List* x)) Cyc_List_split)((( struct Cyc_List_List*(*)( struct _tuple14*(*
f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f1,
_temp1858)); _LL1913: _temp1912= _temp1908.f1; goto _LL1911; _LL1911: _temp1910=
_temp1908.f2; goto _LL1909; _LL1909: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs(
inst, _temp1910); struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1912, ts2)); struct Cyc_Core_Opt*
eff2; if( _temp1862 == 0){ eff2= 0;} else{ void* new_eff= Cyc_Tcutil_substitute(
inst,( void*) _temp1862->v); if( new_eff ==( void*) _temp1862->v){ eff2=
_temp1862;} else{ eff2=({ struct Cyc_Core_Opt* _temp1914=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp1914[ 0]=({ struct Cyc_Core_Opt
_temp1915; _temp1915.v=( void*) new_eff; _temp1915;}); _temp1914;});}} return({
struct Cyc_Absyn_FnType_struct* _temp1916=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1916->tag= Cyc_Absyn_FnType_tag;
_temp1916->f1=({ struct Cyc_Absyn_FnInfo _temp1917; _temp1917.tvars= _temp1864;
_temp1917.effect= eff2; _temp1917.ret_typ=( void*) Cyc_Tcutil_substitute( inst,
_temp1860); _temp1917.args= args2; _temp1917.varargs= _temp1856; _temp1917.attributes=
_temp1854; _temp1917;});( void*) _temp1916;});}} _LL1787: { struct Cyc_List_List*
_temp1920; struct Cyc_List_List* _temp1922; struct _tuple0 _temp1918=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( _temp1866); _LL1923:
_temp1922= _temp1918.f1; goto _LL1921; _LL1921: _temp1920= _temp1918.f2; goto
_LL1919; _LL1919: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs( inst,
_temp1920); if( ts2 == _temp1920){ return t;}{ struct Cyc_List_List* tqts2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1922, ts2); return({ struct Cyc_Absyn_TupleType_struct* _temp1924=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1924->tag= Cyc_Absyn_TupleType_tag; _temp1924->f1= tqts2;( void*) _temp1924;});}}}
_LL1789: if( _temp1870 != 0){ return Cyc_Tcutil_substitute( inst,( void*)
_temp1870->v);} else{ return t;} _LL1791: { void* new_rt= Cyc_Tcutil_substitute(
inst, _temp1874); return new_rt == _temp1874? t:({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1925=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1925->tag= Cyc_Absyn_RgnHandleType_tag;
_temp1925->f1=( void*) new_rt;( void*) _temp1925;});} _LL1793: return t; _LL1795:
return t; _LL1797: return t; _LL1799: return t; _LL1801: return t; _LL1803:
return t; _LL1805: { void* new_r= Cyc_Tcutil_substitute( inst, _temp1884);
return new_r == _temp1884? t:({ struct Cyc_Absyn_AccessEff_struct* _temp1926=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp1926->tag= Cyc_Absyn_AccessEff_tag; _temp1926->f1=( void*) new_r;( void*)
_temp1926;});} _LL1807: { struct Cyc_List_List* new_es= Cyc_Tcutil_substs( inst,
_temp1886); return new_es == _temp1886? t:({ struct Cyc_Absyn_JoinEff_struct*
_temp1927=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1927->tag= Cyc_Absyn_JoinEff_tag; _temp1927->f1= new_es;( void*) _temp1927;});}
_LL1769:;} static struct Cyc_List_List* Cyc_Tcutil_substs( struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* old_hd=( void*)
ts->hd; struct Cyc_List_List* old_tl= ts->tl; void* new_hd= Cyc_Tcutil_substitute(
inst, old_hd); struct Cyc_List_List* new_tl= Cyc_Tcutil_substs( inst, old_tl);
if( old_hd == new_hd? old_tl == new_tl: 0){ return ts;} return({ struct Cyc_List_List*
_temp1928=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1928[ 0]=({ struct Cyc_List_List _temp1929; _temp1929.hd=( void*) new_hd;
_temp1929.tl= new_tl; _temp1929;}); _temp1928;});}} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer(
struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment* loc){ struct
Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp1930=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1930->tag= Cyc_Absyn_Const_e_tag; _temp1930->f1=( void*) Cyc_Absyn_Null_c;(
void*) _temp1930;}), loc);{ void* _temp1931= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp1945; void* _temp1947; void* _temp1949; void* _temp1951; void* _temp1953;
_LL1933: if(( unsigned int) _temp1931 > 4u?(( struct _enum_struct*) _temp1931)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1946: _temp1945=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1931)->f1; goto _LL1934;} else{ goto
_LL1935;} _LL1935: if(( unsigned int) _temp1931 > 4u?(( struct _enum_struct*)
_temp1931)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1950: _temp1949=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1931)->f1; goto _LL1948; _LL1948:
_temp1947=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1931)->f2; if(
_temp1947 == Cyc_Absyn_B1){ goto _LL1936;} else{ goto _LL1937;}} else{ goto
_LL1937;} _LL1937: if(( unsigned int) _temp1931 > 4u?(( struct _enum_struct*)
_temp1931)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1954: _temp1953=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1931)->f1; goto _LL1952; _LL1952:
_temp1951=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1931)->f2; goto
_LL1938;} else{ goto _LL1939;} _LL1939: if( _temp1931 == Cyc_Absyn_FloatType){
goto _LL1940;} else{ goto _LL1941;} _LL1941: if( _temp1931 == Cyc_Absyn_DoubleType){
goto _LL1942;} else{ goto _LL1943;} _LL1943: goto _LL1944; _LL1934: goto _LL1932;
_LL1936:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct* _temp1955=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1955->tag= Cyc_Absyn_Const_e_tag; _temp1955->f1=( void*)({ struct Cyc_Absyn_Char_c_struct*
_temp1956=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp1956->tag= Cyc_Absyn_Char_c_tag; _temp1956->f1=( void*) Cyc_Absyn_Signed;
_temp1956->f2='\000';( void*) _temp1956;});( void*) _temp1955;})); goto _LL1932;
_LL1938:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct* _temp1957=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1957->tag= Cyc_Absyn_Const_e_tag; _temp1957->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp1958=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1958->tag= Cyc_Absyn_Int_c_tag; _temp1958->f1=( void*) _temp1953; _temp1958->f2=
0;( void*) _temp1958;});( void*) _temp1957;})); if( _temp1951 != Cyc_Absyn_B4){
e= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Cast_e_struct* _temp1959=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1959->tag= Cyc_Absyn_Cast_e_tag;
_temp1959->f1=( void*) t; _temp1959->f2= e;( void*) _temp1959;}), loc);} goto
_LL1932; _LL1940:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp1960=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1960->tag= Cyc_Absyn_Const_e_tag; _temp1960->f1=( void*)({ struct Cyc_Absyn_Float_c_struct*
_temp1961=( struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp1961->tag= Cyc_Absyn_Float_c_tag; _temp1961->f1=( struct _tagged_string)({
char* _temp1962=( char*)"0.0"; struct _tagged_string _temp1963; _temp1963.curr=
_temp1962; _temp1963.base= _temp1962; _temp1963.last_plus_one= _temp1962 + 4;
_temp1963;});( void*) _temp1961;});( void*) _temp1960;})); goto _LL1932; _LL1942:(
void*)( e->r=( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1964=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1964->tag= Cyc_Absyn_Cast_e_tag;
_temp1964->f1=( void*) t; _temp1964->f2= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp1965=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1965->tag= Cyc_Absyn_Const_e_tag; _temp1965->f1=( void*)({ struct Cyc_Absyn_Float_c_struct*
_temp1966=( struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp1966->tag= Cyc_Absyn_Float_c_tag; _temp1966->f1=( struct _tagged_string)({
char* _temp1967=( char*)"0.0"; struct _tagged_string _temp1968; _temp1968.curr=
_temp1967; _temp1968.base= _temp1967; _temp1968.last_plus_one= _temp1967 + 4;
_temp1968;});( void*) _temp1966;});( void*) _temp1965;}), loc);( void*)
_temp1964;})); goto _LL1932; _LL1944: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1969= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp1969.last_plus_one - _temp1969.curr, _temp1969.curr);})); goto _LL1932;
_LL1932:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp1970=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp1970->f1= tv; _temp1970->f2= Cyc_Absyn_new_evar( k); _temp1970;});}
struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp1971= Cyc_Tcutil_compress( t); int
_temp2011; struct Cyc_Core_Opt* _temp2013; void* _temp2015; struct Cyc_Absyn_Tvar*
_temp2017; struct Cyc_Absyn_Enumdecl** _temp2019; struct Cyc_Absyn_Enumdecl***
_temp2021; struct Cyc_List_List* _temp2022; struct _tuple1* _temp2024; struct
Cyc_Absyn_Xenumdecl** _temp2026; struct Cyc_Absyn_Xenumdecl*** _temp2028; struct
_tuple1* _temp2029; struct Cyc_Absyn_PtrInfo _temp2031; struct Cyc_Absyn_Conref*
_temp2033; struct Cyc_Absyn_Tqual* _temp2035; struct Cyc_Absyn_Conref* _temp2037;
void* _temp2039; void* _temp2041; void* _temp2043; void* _temp2045; struct Cyc_Absyn_Exp*
_temp2047; struct Cyc_Absyn_Tqual* _temp2049; void* _temp2051; struct Cyc_Absyn_FnInfo
_temp2053; struct Cyc_List_List* _temp2055; int _temp2057; struct Cyc_List_List*
_temp2059; void* _temp2061; struct Cyc_Core_Opt* _temp2063; struct Cyc_Core_Opt**
_temp2065; struct Cyc_List_List* _temp2066; struct Cyc_List_List** _temp2068;
struct Cyc_List_List* _temp2069; struct Cyc_Absyn_Structdecl** _temp2071; struct
Cyc_Absyn_Structdecl*** _temp2073; struct Cyc_List_List* _temp2074; struct
_tuple1* _temp2076; struct Cyc_Absyn_Uniondecl** _temp2078; struct Cyc_Absyn_Uniondecl***
_temp2080; struct Cyc_List_List* _temp2081; struct _tuple1* _temp2083; struct
Cyc_Core_Opt* _temp2085; struct Cyc_Core_Opt** _temp2087; struct Cyc_List_List*
_temp2088; struct _tuple1* _temp2090; void* _temp2092; void* _temp2094; struct
Cyc_List_List* _temp2096; _LL1973: if( _temp1971 == Cyc_Absyn_VoidType){ goto
_LL1974;} else{ goto _LL1975;} _LL1975: if(( unsigned int) _temp1971 > 4u?((
struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_Evar_tag: 0){ _LL2016:
_temp2015=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1971)->f1; goto _LL2014;
_LL2014: _temp2013=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1971)->f2; goto _LL2012; _LL2012: _temp2011=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1971)->f3; goto _LL1976;} else{ goto _LL1977;} _LL1977: if(( unsigned int)
_temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_VarType_tag:
0){ _LL2018: _temp2017=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1971)->f1; goto _LL1978;} else{ goto _LL1979;} _LL1979: if(( unsigned int)
_temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL2025: _temp2024=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1971)->f1; goto _LL2023; _LL2023: _temp2022=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1971)->f2; goto _LL2020; _LL2020:
_temp2019=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1971)->f3; _temp2021=&(( struct Cyc_Absyn_EnumType_struct*) _temp1971)->f3;
goto _LL1980;} else{ goto _LL1981;} _LL1981: if(( unsigned int) _temp1971 > 4u?((
struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL2030:
_temp2029=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp1971)->f1;
goto _LL2027; _LL2027: _temp2026=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp1971)->f2; _temp2028=&(( struct Cyc_Absyn_XenumType_struct*) _temp1971)->f2;
goto _LL1982;} else{ goto _LL1983;} _LL1983: if(( unsigned int) _temp1971 > 4u?((
struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2032:
_temp2031=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1971)->f1; _LL2042: _temp2041=( void*) _temp2031.elt_typ; goto _LL2040;
_LL2040: _temp2039=( void*) _temp2031.rgn_typ; goto _LL2038; _LL2038: _temp2037=(
struct Cyc_Absyn_Conref*) _temp2031.nullable; goto _LL2036; _LL2036: _temp2035=(
struct Cyc_Absyn_Tqual*) _temp2031.tq; goto _LL2034; _LL2034: _temp2033=( struct
Cyc_Absyn_Conref*) _temp2031.bounds; goto _LL1984;} else{ goto _LL1985;} _LL1985:
if(( unsigned int) _temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL2046: _temp2045=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1971)->f1; goto _LL2044; _LL2044: _temp2043=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1971)->f2; goto _LL1986;} else{ goto _LL1987;} _LL1987: if( _temp1971 ==
Cyc_Absyn_FloatType){ goto _LL1988;} else{ goto _LL1989;} _LL1989: if( _temp1971
== Cyc_Absyn_DoubleType){ goto _LL1990;} else{ goto _LL1991;} _LL1991: if((
unsigned int) _temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2052: _temp2051=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1971)->f1;
goto _LL2050; _LL2050: _temp2049=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1971)->f2; goto _LL2048; _LL2048: _temp2047=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1971)->f3; goto _LL1992;} else{ goto
_LL1993;} _LL1993: if(( unsigned int) _temp1971 > 4u?(( struct _enum_struct*)
_temp1971)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2054: _temp2053=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1971)->f1; _LL2067: _temp2066=( struct Cyc_List_List*)
_temp2053.tvars; _temp2068=&((( struct Cyc_Absyn_FnType_struct*) _temp1971)->f1).tvars;
goto _LL2064; _LL2064: _temp2063=( struct Cyc_Core_Opt*) _temp2053.effect;
_temp2065=&((( struct Cyc_Absyn_FnType_struct*) _temp1971)->f1).effect; goto
_LL2062; _LL2062: _temp2061=( void*) _temp2053.ret_typ; goto _LL2060; _LL2060:
_temp2059=( struct Cyc_List_List*) _temp2053.args; goto _LL2058; _LL2058:
_temp2057=( int) _temp2053.varargs; goto _LL2056; _LL2056: _temp2055=( struct
Cyc_List_List*) _temp2053.attributes; goto _LL1994;} else{ goto _LL1995;}
_LL1995: if(( unsigned int) _temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL2070: _temp2069=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1971)->f1; goto _LL1996;} else{ goto
_LL1997;} _LL1997: if(( unsigned int) _temp1971 > 4u?(( struct _enum_struct*)
_temp1971)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2077: _temp2076=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1971)->f1; goto _LL2075;
_LL2075: _temp2074=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1971)->f2; goto _LL2072; _LL2072: _temp2071=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1971)->f3; _temp2073=&(( struct Cyc_Absyn_StructType_struct*)
_temp1971)->f3; goto _LL1998;} else{ goto _LL1999;} _LL1999: if(( unsigned int)
_temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL2084: _temp2083=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1971)->f1; goto _LL2082; _LL2082: _temp2081=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1971)->f2; goto _LL2079; _LL2079:
_temp2078=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1971)->f3; _temp2080=&(( struct Cyc_Absyn_UnionType_struct*) _temp1971)->f3;
goto _LL2000;} else{ goto _LL2001;} _LL2001: if(( unsigned int) _temp1971 > 4u?((
struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2091:
_temp2090=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp1971)->f1;
goto _LL2089; _LL2089: _temp2088=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1971)->f2; goto _LL2086; _LL2086: _temp2085=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1971)->f3; _temp2087=&(( struct Cyc_Absyn_TypedefType_struct*)
_temp1971)->f3; goto _LL2002;} else{ goto _LL2003;} _LL2003: if( _temp1971 ==
Cyc_Absyn_HeapRgn){ goto _LL2004;} else{ goto _LL2005;} _LL2005: if((
unsigned int) _temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL2093: _temp2092=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1971)->f1; goto _LL2006;} else{ goto _LL2007;} _LL2007: if(( unsigned int)
_temp1971 > 4u?(( struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL2095: _temp2094=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1971)->f1;
goto _LL2008;} else{ goto _LL2009;} _LL2009: if(( unsigned int) _temp1971 > 4u?((
struct _enum_struct*) _temp1971)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2097:
_temp2096=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp1971)->f1;
goto _LL2010;} else{ goto _LL1972;} _LL1974: goto _LL1972; _LL1976: goto _LL1972;
_LL1978: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2017->kind); if(( void*) c->v == Cyc_Absyn_No_constr){(
void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2098=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2098->tag= Cyc_Absyn_Eq_constr_tag; _temp2098->f1=( void*) expected_kind;(
void*) _temp2098;}));} free_vars= Cyc_Tcutil_add_free_tvar( loc, free_vars,
_temp2017); goto _LL1972;} _LL1980: if( _temp2024 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2099=( char*)"missing enum name"; struct
_tagged_string _temp2100; _temp2100.curr= _temp2099; _temp2100.base= _temp2099;
_temp2100.last_plus_one= _temp2099 + 18; _temp2100;})); return free_vars;} if(*
_temp2021 == 0){ struct _handler_cons _temp2101; _push_handler(& _temp2101);{
struct _xenum_struct* _temp2102=( struct _xenum_struct*) setjmp( _temp2101.handler);
if( ! _temp2102){* _temp2021=( struct Cyc_Absyn_Enumdecl**) Cyc_Tcenv_lookup_enumdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2103= _temp2024; if( _temp2103
== 0){ _throw( Null_Exception);} _temp2103;}));; _pop_handler();} else{ struct
_xenum_struct* _temp2105= _temp2102; _LL2107: if( _temp2105->tag == Cyc_Dict_Absent_tag){
goto _LL2108;} else{ goto _LL2109;} _LL2109: goto _LL2110; _LL2108: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2112= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2111= _temp2024; if( _temp2111 == 0){ _throw(
Null_Exception);} _temp2111;})); xprintf("unbound type enum %.*s", _temp2112.last_plus_one
- _temp2112.curr, _temp2112.curr);})); return free_vars; _LL2110:( void) _throw(
_temp2105); _LL2106:;}}}{ struct Cyc_Absyn_Enumdecl* ed=*(* _temp2021); if( ed->name
!= 0){* _temp2024=({ struct _tuple1* _temp2113=( struct _tuple1*)( ed->name)->v;
unsigned int _temp2114= 0; if( _temp2114 >= 1u){ _throw( Null_Exception);}
_temp2113[ _temp2114];});}{ struct Cyc_List_List* tvs= ed->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2022); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2116= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2115= _temp2024; if( _temp2115 == 0){ _throw(
Null_Exception);} _temp2115;})); int _temp2117= lvs; int _temp2118= largs;
xprintf("enum %.*s expects %d type arguments, not %d", _temp2116.last_plus_one -
_temp2116.curr, _temp2116.curr, _temp2117, _temp2118);}));} for( 0; _temp2022 !=
0; _temp2022= _temp2022->tl, tvs= tvs->tl){ void* t1=( void*) _temp2022->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1972;}} _LL1982: if(* _temp2028 == 0){
struct _handler_cons _temp2119; _push_handler(& _temp2119);{ struct
_xenum_struct* _temp2120=( struct _xenum_struct*) setjmp( _temp2119.handler);
if( ! _temp2120){{ struct Cyc_Core_Opt* xed= Cyc_Tcenv_lookup_xenumdecl( te, loc,
_temp2029); if( xed == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2121= Cyc_Absynpp_qvar2string( _temp2029); xprintf("unbound xenum %.*s",
_temp2121.last_plus_one - _temp2121.curr, _temp2121.curr);}));{ struct Cyc_List_List*
_temp2122= free_vars; _npop_handler( 0u); return _temp2122;}}* _temp2028=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_Xenumdecl**) xed->v);};
_pop_handler();} else{ struct _xenum_struct* _temp2124= _temp2120; _LL2126: if(
_temp2124->tag == Cyc_Dict_Absent_tag){ goto _LL2127;} else{ goto _LL2128;}
_LL2128: goto _LL2129; _LL2127: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2130= Cyc_Absynpp_qvar2string( _temp2029); xprintf("unbound xenum %.*s",
_temp2130.last_plus_one - _temp2130.curr, _temp2130.curr);})); return free_vars;
_LL2129:( void) _throw( _temp2124); _LL2125:;}}}{ struct Cyc_Absyn_Xenumdecl*
xed=*(* _temp2028);({ struct _tuple1* _temp2131= _temp2029; unsigned int
_temp2132= 0; if( _temp2132 >= 1u){ _throw( Null_Exception);} _temp2131[
_temp2132]=({ struct _tuple1* _temp2133= xed->name; unsigned int _temp2134= 0;
if( _temp2134 >= 1u){ _throw( Null_Exception);} _temp2133[ _temp2134];});});
goto _LL1972;} _LL1984: free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars, Cyc_Absyn_AnyKind, _temp2041); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, Cyc_Absyn_RgnKind, _temp2039);{ void* _temp2135=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2033))->v; void* _temp2141; struct Cyc_Absyn_Exp* _temp2143; _LL2137: if((
unsigned int) _temp2135 > 1u?(( struct _enum_struct*) _temp2135)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2142: _temp2141=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2135)->f1;
if(( unsigned int) _temp2141 > 1u?(( struct _enum_struct*) _temp2141)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL2144: _temp2143=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp2141)->f1; goto _LL2138;} else{ goto _LL2139;}}
else{ goto _LL2139;} _LL2139: goto _LL2140; _LL2138: Cyc_Evexp_eval_const_uint_exp(
_temp2143); goto _LL2136; _LL2140: goto _LL2136; _LL2136:;} goto _LL1972;
_LL1986: goto _LL1972; _LL1988: goto _LL1972; _LL1990: goto _LL1972; _LL1992:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_MemKind,
_temp2051); if( _temp2047 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2145= _temp2047; if( _temp2145 == 0){ _throw(
Null_Exception);} _temp2145;}))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp2146=( char*)"array bounds expression is not constant"; struct
_tagged_string _temp2147; _temp2147.curr= _temp2146; _temp2147.base= _temp2146;
_temp2147.last_plus_one= _temp2146 + 40; _temp2147;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2148= _temp2047; if(
_temp2148 == 0){ _throw( Null_Exception);} _temp2148;})); goto _LL1972; _LL1994:
if( _temp2055 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2055
!= 0; _temp2055= _temp2055->tl){ if( ! Cyc_Tcutil_fntype_att(( void*) _temp2055->hd)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2149= Cyc_Absyn_attribute2string((
void*) _temp2055->hd); xprintf("bad function type attribute %.*s", _temp2149.last_plus_one
- _temp2149.curr, _temp2149.curr);}));} if(( void*) _temp2055->hd == Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*) _temp2055->hd == Cyc_Absyn_Cdecl_att){ if(
seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2068);{ struct Cyc_List_List*
b=* _temp2068; for( 0; b != 0; b= b->tl){ void* _temp2150=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)
b->hd)->kind))->v; void* _temp2156; _LL2152: if(( unsigned int) _temp2150 > 1u?((
struct _enum_struct*) _temp2150)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2157:
_temp2156=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2150)->f1; if(
_temp2156 == Cyc_Absyn_MemKind){ goto _LL2153;} else{ goto _LL2154;}} else{ goto
_LL2154;} _LL2154: goto _LL2155; _LL2153: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2158=*(( struct Cyc_Absyn_Tvar*) b->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2158.last_plus_one - _temp2158.curr, _temp2158.curr);})); goto _LL2151;
_LL2155: goto _LL2151; _LL2151:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars, Cyc_Absyn_MemKind,
_temp2061); for( 0; _temp2059 != 0; _temp2059= _temp2059->tl){ new_free_vars=
Cyc_Tcutil_check_valid_type( loc, te, new_free_vars, Cyc_Absyn_MemKind,(*((
struct _tuple2*) _temp2059->hd)).f3);} if(* _temp2065 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars, Cyc_Absyn_EffKind,( void*)(* _temp2065)->v);} else{
struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar( Cyc_Absyn_EffKind); new_free_vars=({
struct Cyc_List_List* _temp2159=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2159->hd=( void*) e; _temp2159->tl= new_free_vars;
_temp2159;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs=
new_free_vars; for( 0; tvs != 0; tvs= tvs->tl){ void* _temp2160=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp2168; void* _temp2170;
_LL2162: if(( unsigned int) _temp2160 > 1u?(( struct _enum_struct*) _temp2160)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL2169: _temp2168=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2160)->f1; if( _temp2168 == Cyc_Absyn_RgnKind){ goto _LL2163;} else{ goto
_LL2164;}} else{ goto _LL2164;} _LL2164: if(( unsigned int) _temp2160 > 1u?((
struct _enum_struct*) _temp2160)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2171:
_temp2170=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2160)->f1; if(
_temp2170 == Cyc_Absyn_EffKind){ goto _LL2165;} else{ goto _LL2166;}} else{ goto
_LL2166;} _LL2166: goto _LL2167; _LL2163: effect=({ struct Cyc_List_List*
_temp2172=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2172->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2173=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2173->tag= Cyc_Absyn_AccessEff_tag; _temp2173->f1=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2174=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2174->tag= Cyc_Absyn_VarType_tag; _temp2174->f1=( struct Cyc_Absyn_Tvar*)
tvs->hd;( void*) _temp2174;});( void*) _temp2173;}); _temp2172->tl= effect;
_temp2172;}); goto _LL2161; _LL2165: effect=({ struct Cyc_List_List* _temp2175=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2175->hd=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp2176=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2176->tag= Cyc_Absyn_VarType_tag;
_temp2176->f1=( struct Cyc_Absyn_Tvar*) tvs->hd;( void*) _temp2176;}); _temp2175->tl=
effect; _temp2175;}); goto _LL2161; _LL2167: goto _LL2161; _LL2161:;}}*
_temp2065=({ struct Cyc_Core_Opt* _temp2177=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2177->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2178=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2178->tag= Cyc_Absyn_JoinEff_tag; _temp2178->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);( void*) _temp2178;});
_temp2177;});}} if(* _temp2068 != 0){ struct Cyc_List_List* new_btvs= 0;{ struct
Cyc_List_List* bs=* _temp2068; for( 0; bs != 0; bs= bs->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*) bs->hd);{ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp2179=( void*) c->v; void*
_temp2187; _LL2181: if( _temp2179 == Cyc_Absyn_No_constr){ goto _LL2182;} else{
goto _LL2183;} _LL2183: if(( unsigned int) _temp2179 > 1u?(( struct _enum_struct*)
_temp2179)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2188: _temp2187=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2179)->f1; if( _temp2187 == Cyc_Absyn_MemKind){
goto _LL2184;} else{ goto _LL2185;}} else{ goto _LL2185;} _LL2185: goto _LL2186;
_LL2182: Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp2189=*(( struct Cyc_Absyn_Tvar*)
bs->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2189.last_plus_one - _temp2189.curr, _temp2189.curr);})); goto _LL2184;
_LL2184:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2190=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2190->tag= Cyc_Absyn_Eq_constr_tag; _temp2190->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp2190;})); goto _LL2180; _LL2186: goto _LL2180; _LL2180:;}}}
new_free_vars= Cyc_Tcutil_remove_bound_tvars( new_free_vars,* _temp2068);} for(
0; new_free_vars != 0; new_free_vars= new_free_vars->tl){ free_vars= Cyc_Tcutil_add_free_tvar(
loc, free_vars,( struct Cyc_Absyn_Tvar*) new_free_vars->hd);} goto _LL1972;}
_LL1996: for( 0; _temp2069 != 0; _temp2069= _temp2069->tl){ free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp2069->hd)).f2);}
goto _LL1972; _LL1998: if( _temp2076 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2191=( char*)"missing struct name"; struct
_tagged_string _temp2192; _temp2192.curr= _temp2191; _temp2192.base= _temp2191;
_temp2192.last_plus_one= _temp2191 + 20; _temp2192;})); return free_vars;} if(*
_temp2073 == 0){ struct _handler_cons _temp2193; _push_handler(& _temp2193);{
struct _xenum_struct* _temp2194=( struct _xenum_struct*) setjmp( _temp2193.handler);
if( ! _temp2194){* _temp2073=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2195= _temp2076; if( _temp2195
== 0){ _throw( Null_Exception);} _temp2195;}));; _pop_handler();} else{ struct
_xenum_struct* _temp2197= _temp2194; _LL2199: if( _temp2197->tag == Cyc_Dict_Absent_tag){
goto _LL2200;} else{ goto _LL2201;} _LL2201: goto _LL2202; _LL2200: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2204= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2203= _temp2076; if( _temp2203 == 0){ _throw(
Null_Exception);} _temp2203;})); xprintf("unbound type struct %.*s", _temp2204.last_plus_one
- _temp2204.curr, _temp2204.curr);})); return free_vars; _LL2202:( void) _throw(
_temp2197); _LL2198:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(* _temp2073); if( sd->name
!= 0){* _temp2076=({ struct _tuple1* _temp2205=( struct _tuple1*)( sd->name)->v;
unsigned int _temp2206= 0; if( _temp2206 >= 1u){ _throw( Null_Exception);}
_temp2205[ _temp2206];});}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2074); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2208= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2207= _temp2076; if( _temp2207 == 0){ _throw(
Null_Exception);} _temp2207;})); int _temp2209= lvs; int _temp2210= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _temp2208.last_plus_one
- _temp2208.curr, _temp2208.curr, _temp2209, _temp2210);}));} for( 0; _temp2074
!= 0; _temp2074= _temp2074->tl, tvs= tvs->tl){ void* t1=( void*) _temp2074->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1972;}} _LL2000: if( _temp2083 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2211=( char*)"missing union name";
struct _tagged_string _temp2212; _temp2212.curr= _temp2211; _temp2212.base=
_temp2211; _temp2212.last_plus_one= _temp2211 + 19; _temp2212;})); return
free_vars;} if(* _temp2080 == 0){ struct _handler_cons _temp2213; _push_handler(&
_temp2213);{ struct _xenum_struct* _temp2214=( struct _xenum_struct*) setjmp(
_temp2213.handler); if( ! _temp2214){* _temp2080=( struct Cyc_Absyn_Uniondecl**)
Cyc_Tcenv_lookup_uniondecl( te, loc,( struct _tuple1*)({ struct _tuple1*
_temp2215= _temp2083; if( _temp2215 == 0){ _throw( Null_Exception);} _temp2215;}));;
_pop_handler();} else{ struct _xenum_struct* _temp2217= _temp2214; _LL2219: if(
_temp2217->tag == Cyc_Dict_Absent_tag){ goto _LL2220;} else{ goto _LL2221;}
_LL2221: goto _LL2222; _LL2220: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2224= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2223= _temp2083; if( _temp2223 == 0){ _throw( Null_Exception);} _temp2223;}));
xprintf("unbound type union %.*s", _temp2224.last_plus_one - _temp2224.curr,
_temp2224.curr);})); return free_vars; _LL2222:( void) _throw( _temp2217);
_LL2218:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(* _temp2080); if( ud->name != 0){*
_temp2083=({ struct _tuple1* _temp2225=( struct _tuple1*)( ud->name)->v;
unsigned int _temp2226= 0; if( _temp2226 >= 1u){ _throw( Null_Exception);}
_temp2225[ _temp2226];});}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2081); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2228= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2227= _temp2083; if( _temp2227 == 0){ _throw(
Null_Exception);} _temp2227;})); int _temp2229= lvs; int _temp2230= largs;
xprintf("union %.*s expects %d type arguments but was given %d", _temp2228.last_plus_one
- _temp2228.curr, _temp2228.curr, _temp2229, _temp2230);}));} for( 0; _temp2081
!= 0; _temp2081= _temp2081->tl, tvs= tvs->tl){ void* t1=( void*) _temp2081->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1972;}} _LL2002: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2231; _push_handler(& _temp2231);{ struct
_xenum_struct* _temp2232=( struct _xenum_struct*) setjmp( _temp2231.handler);
if( ! _temp2232){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc, _temp2090);;
_pop_handler();} else{ struct _xenum_struct* _temp2234= _temp2232; _LL2236: if(
_temp2234->tag == Cyc_Dict_Absent_tag){ goto _LL2237;} else{ goto _LL2238;}
_LL2238: goto _LL2239; _LL2237: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2240= Cyc_Absynpp_qvar2string( _temp2090); xprintf("unbound typedef name %.*s",
_temp2240.last_plus_one - _temp2240.curr, _temp2240.curr);})); return free_vars;
_LL2239:( void) _throw( _temp2234); _LL2235:;}}}({ struct _tuple1* _temp2241=
_temp2090; unsigned int _temp2242= 0; if( _temp2242 >= 1u){ _throw(
Null_Exception);} _temp2241[ _temp2242]=({ struct _tuple1* _temp2243= td->name;
unsigned int _temp2244= 0; if( _temp2244 >= 1u){ _throw( Null_Exception);}
_temp2243[ _temp2244];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2088; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=
ts->tl, tvs= tvs->tl){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp2245=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2245->hd=(
void*)({ struct _tuple5* _temp2246=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp2246->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp2246->f2=( void*)
ts->hd; _temp2246;}); _temp2245->tl= inst; _temp2245;});} if( tvs != 0? 1: ts !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2247= Cyc_Absynpp_qvar2string(
_temp2090); xprintf("typedef %.*s expects a different number of arguments",
_temp2247.last_plus_one - _temp2247.curr, _temp2247.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2087=({
struct Cyc_Core_Opt* _temp2248=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2248->v=( void*) new_typ; _temp2248;}); goto _LL1972;}}}
_LL2004: goto _LL1972; _LL2006: _temp2094= _temp2092; goto _LL2008; _LL2008:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_RgnKind,
_temp2094); goto _LL1972; _LL2010: for( 0; _temp2096 != 0; _temp2096= _temp2096->tl){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_EffKind,(
void*) _temp2096->hd);} goto _LL1972; _LL1972:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind(
t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2249=
Cyc_Absynpp_typ2string( t); struct _tagged_string _temp2250= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_string _temp2251= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2249.last_plus_one - _temp2249.curr, _temp2249.curr, _temp2250.last_plus_one
- _temp2250.curr, _temp2250.curr, _temp2251.last_plus_one - _temp2251.curr,
_temp2251.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x= x->tl){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) x->hd)->kind); void* _temp2252=( void*) c->v; void*
_temp2260; _LL2254: if( _temp2252 == Cyc_Absyn_No_constr){ goto _LL2255;} else{
goto _LL2256;} _LL2256: if(( unsigned int) _temp2252 > 1u?(( struct _enum_struct*)
_temp2252)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2261: _temp2260=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2252)->f1; if( _temp2260 == Cyc_Absyn_MemKind){
goto _LL2257;} else{ goto _LL2258;}} else{ goto _LL2258;} _LL2258: goto _LL2259;
_LL2255: goto _LL2257; _LL2257:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2262=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2262->tag= Cyc_Absyn_Eq_constr_tag; _temp2262->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp2262;})); goto _LL2253; _LL2259: goto _LL2253; _LL2253:;}} if(
free_tvars != 0){ void* _temp2263= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2269; struct Cyc_List_List* _temp2271; int _temp2273; struct Cyc_List_List*
_temp2275; void* _temp2277; struct Cyc_Core_Opt* _temp2279; struct Cyc_List_List*
_temp2281; struct Cyc_List_List** _temp2283; _LL2265: if(( unsigned int)
_temp2263 > 4u?(( struct _enum_struct*) _temp2263)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2270: _temp2269=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2263)->f1; _LL2282: _temp2281=( struct Cyc_List_List*) _temp2269.tvars;
_temp2283=&((( struct Cyc_Absyn_FnType_struct*) _temp2263)->f1).tvars; goto
_LL2280; _LL2280: _temp2279=( struct Cyc_Core_Opt*) _temp2269.effect; goto
_LL2278; _LL2278: _temp2277=( void*) _temp2269.ret_typ; goto _LL2276; _LL2276:
_temp2275=( struct Cyc_List_List*) _temp2269.args; goto _LL2274; _LL2274:
_temp2273=( int) _temp2269.varargs; goto _LL2272; _LL2272: _temp2271=( struct
Cyc_List_List*) _temp2269.attributes; goto _LL2266;} else{ goto _LL2267;}
_LL2267: goto _LL2268; _LL2266: if(* _temp2283 == 0){* _temp2283= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; if(( int)({ struct _tagged_string _temp2284= s; char*
_temp2286= _temp2284.curr + 0; if( _temp2286 < _temp2284.base? 1: _temp2286 >=
_temp2284.last_plus_one){ _throw( Null_Exception);}* _temp2286;}) ==( int)'?'){
Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2287= s; xprintf("unbound type variable %.*s",
_temp2287.last_plus_one - _temp2287.curr, _temp2287.curr);}));}}} goto _LL2264;
_LL2268: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2288=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; xprintf("unbound type variable %.*s", _temp2288.last_plus_one
- _temp2288.curr, _temp2288.curr);})); return; _LL2264:;}} void Cyc_Tcutil_check_type(
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
_tagged_string _temp2289=* ftvn; struct _tagged_string _temp2290= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2291= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2289.last_plus_one - _temp2289.curr, _temp2289.curr, _temp2290.last_plus_one
- _temp2290.curr, _temp2290.curr, _temp2291.last_plus_one - _temp2291.curr,
_temp2291.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2292=* ftvn; struct _tagged_string _temp2293= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2292.last_plus_one -
_temp2292.curr, _temp2292.curr, _temp2293.last_plus_one - _temp2293.curr,
_temp2293.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs= vs->tl){ struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 != 0; vs2=
vs2->tl){ if( cmp(( void*) vs->hd,( void*) vs2->hd) == 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2294= msg; struct _tagged_string _temp2295= a2string((
void*) vs->hd); xprintf("%.*s: %.*s", _temp2294.last_plus_one - _temp2294.curr,
_temp2294.curr, _temp2295.last_plus_one - _temp2295.curr, _temp2295.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2296=(
char*)"duplicate type variable"; struct _tagged_string _temp2297; _temp2297.curr=
_temp2296; _temp2297.base= _temp2296; _temp2297.last_plus_one= _temp2296 + 24;
_temp2297;}));} struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd){ if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2298=( char*)"inappropriate use of abstract struct"; struct _tagged_string
_temp2299; _temp2299.curr= _temp2298; _temp2299.base= _temp2298; _temp2299.last_plus_one=
_temp2298 + 37; _temp2299;}));}{ struct Cyc_List_List* fields= 0;{ struct Cyc_List_List*
sd_fields=( struct Cyc_List_List*)( sd->fields)->v; for( 0; sd_fields != 0;
sd_fields= sd_fields->tl){ if( Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)
sd_fields->hd)->name,( struct _tagged_string)({ char* _temp2300=( char*)"";
struct _tagged_string _temp2301; _temp2301.curr= _temp2300; _temp2301.base=
_temp2300; _temp2301.last_plus_one= _temp2300 + 1; _temp2301;})) != 0){ fields=({
struct Cyc_List_List* _temp2302=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2302->hd=( void*)({ struct _tuple15* _temp2303=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2303->f1=( struct
Cyc_Absyn_Structfield*) sd_fields->hd; _temp2303->f2= 0; _temp2303;}); _temp2302->tl=
fields; _temp2302;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des= des->tl){ struct _tuple16 _temp2306; void* _temp2307; struct Cyc_List_List*
_temp2309; struct _tuple16* _temp2304=( struct _tuple16*) des->hd; _temp2306=*
_temp2304; _LL2310: _temp2309= _temp2306.f1; goto _LL2308; _LL2308: _temp2307=
_temp2306.f2; goto _LL2305; _LL2305: if( _temp2309 == 0){ struct Cyc_List_List*
fields2= fields; for( 0; fields2 != 0; fields2= fields2->tl){ if( !(*(( struct
_tuple15*) fields2->hd)).f2){(*(( struct _tuple15*) fields2->hd)).f2= 1;(*((
struct _tuple16*) des->hd)).f1=({ struct Cyc_List_List* _temp2311=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2311->hd=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp2312=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2312->tag= Cyc_Absyn_FieldName_tag; _temp2312->f1=((*(( struct _tuple15*)
fields2->hd)).f1)->name;( void*) _temp2312;}); _temp2311->tl= 0; _temp2311;});
ans=({ struct Cyc_List_List* _temp2313=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2313->hd=( void*)({ struct _tuple17*
_temp2314=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2314->f1=(*((
struct _tuple15*) fields2->hd)).f1; _temp2314->f2= _temp2307; _temp2314;});
_temp2313->tl= ans; _temp2313;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2315=( char*)"too many arguments to struct";
struct _tagged_string _temp2316; _temp2316.curr= _temp2315; _temp2316.base=
_temp2315; _temp2316.last_plus_one= _temp2315 + 29; _temp2316;}));}} else{ if(
_temp2309->tl != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2317=( char*)"multiple designators are not supported"; struct
_tagged_string _temp2318; _temp2318.curr= _temp2317; _temp2318.base= _temp2317;
_temp2318.last_plus_one= _temp2317 + 39; _temp2318;}));} else{ void* _temp2319=(
void*) _temp2309->hd; struct Cyc_Absyn_Exp* _temp2325; struct _tagged_string*
_temp2327; _LL2321: if((( struct _enum_struct*) _temp2319)->tag == Cyc_Absyn_ArrayElement_tag){
_LL2326: _temp2325=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp2319)->f1; goto _LL2322;} else{ goto _LL2323;} _LL2323: if((( struct
_enum_struct*) _temp2319)->tag == Cyc_Absyn_FieldName_tag){ _LL2328: _temp2327=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2319)->f1;
goto _LL2324;} else{ goto _LL2320;} _LL2322: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2329=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2330; _temp2330.curr= _temp2329; _temp2330.base=
_temp2329; _temp2330.last_plus_one= _temp2329 + 44; _temp2330;})); goto _LL2320;
_LL2324: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=
fields2->tl){ if( Cyc_String_zstrptrcmp( _temp2327,((*(( struct _tuple15*)
fields2->hd)).f1)->name) == 0){ if((*(( struct _tuple15*) fields2->hd)).f2){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2331=* _temp2327; xprintf("field %.*s has already been used as an argument",
_temp2331.last_plus_one - _temp2331.curr, _temp2331.curr);}));}(*(( struct
_tuple15*) fields2->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp2332=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2332->hd=( void*)({
struct _tuple17* _temp2333=( struct _tuple17*) GC_malloc( sizeof( struct
_tuple17)); _temp2333->f1=(*(( struct _tuple15*) fields2->hd)).f1; _temp2333->f2=
_temp2307; _temp2333;}); _temp2332->tl= ans; _temp2332;}); break;}} if( fields2
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2334=* _temp2327;
xprintf("bad field designator %.*s", _temp2334.last_plus_one - _temp2334.curr,
_temp2334.curr);}));} goto _LL2320;} _LL2320:;}}} for( 0; fields != 0; fields=
fields->tl){ if( !(*(( struct _tuple15*) fields->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2335=( char*)"too few arguments to struct";
struct _tagged_string _temp2336; _temp2336.curr= _temp2335; _temp2336.base=
_temp2335; _temp2336.last_plus_one= _temp2335 + 28; _temp2336;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp2337= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp2343; struct Cyc_Absyn_Conref* _temp2345;
struct Cyc_Absyn_Tqual* _temp2347; struct Cyc_Absyn_Conref* _temp2349; void*
_temp2351; void* _temp2353; _LL2339: if(( unsigned int) _temp2337 > 4u?(( struct
_enum_struct*) _temp2337)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2344:
_temp2343=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2337)->f1; _LL2354: _temp2353=( void*) _temp2343.elt_typ; goto _LL2352;
_LL2352: _temp2351=( void*) _temp2343.rgn_typ; goto _LL2350; _LL2350: _temp2349=(
struct Cyc_Absyn_Conref*) _temp2343.nullable; goto _LL2348; _LL2348: _temp2347=(
struct Cyc_Absyn_Tqual*) _temp2343.tq; goto _LL2346; _LL2346: _temp2345=( struct
Cyc_Absyn_Conref*) _temp2343.bounds; goto _LL2340;} else{ goto _LL2341;} _LL2341:
goto _LL2342; _LL2340: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2345); void*
_temp2355=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp2363; _LL2357: if(( unsigned int)
_temp2355 > 1u?(( struct _enum_struct*) _temp2355)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2364: _temp2363=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2355)->f1;
if( _temp2363 == Cyc_Absyn_Unknown_b){ goto _LL2358;} else{ goto _LL2359;}}
else{ goto _LL2359;} _LL2359: if( _temp2355 == Cyc_Absyn_No_constr){ goto
_LL2360;} else{ goto _LL2361;} _LL2361: goto _LL2362; _LL2358: return 1; _LL2360:(
void*)( b->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2365=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2365->tag= Cyc_Absyn_Eq_constr_tag; _temp2365->f1=( void*) Cyc_Absyn_Unknown_b;(
void*) _temp2365;})); return 1; _LL2362: return 0; _LL2356:;} _LL2342: return 0;
_LL2338:;} struct _tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp2366;
_temp2366.f1= 0; _temp2366.f2= Cyc_Absyn_HeapRgn; _temp2366;}); void* _temp2367=(
void*) e->r; void* _temp2381; struct _tuple1* _temp2383; struct _tagged_string*
_temp2385; struct Cyc_Absyn_Exp* _temp2387; struct _tagged_string* _temp2389;
struct Cyc_Absyn_Exp* _temp2391; struct Cyc_Absyn_Exp* _temp2393; struct Cyc_Absyn_Exp*
_temp2395; struct Cyc_Absyn_Exp* _temp2397; _LL2369: if((( struct _enum_struct*)
_temp2367)->tag == Cyc_Absyn_Var_e_tag){ _LL2384: _temp2383=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp2367)->f1; goto _LL2382; _LL2382: _temp2381=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp2367)->f2; goto _LL2370;} else{
goto _LL2371;} _LL2371: if((( struct _enum_struct*) _temp2367)->tag == Cyc_Absyn_StructMember_e_tag){
_LL2388: _temp2387=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2367)->f1; goto _LL2386; _LL2386: _temp2385=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp2367)->f2; goto _LL2372;} else{
goto _LL2373;} _LL2373: if((( struct _enum_struct*) _temp2367)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL2392: _temp2391=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2367)->f1; goto _LL2390; _LL2390: _temp2389=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp2367)->f2; goto _LL2374;} else{
goto _LL2375;} _LL2375: if((( struct _enum_struct*) _temp2367)->tag == Cyc_Absyn_Deref_e_tag){
_LL2394: _temp2393=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp2367)->f1; goto _LL2376;} else{ goto _LL2377;} _LL2377: if((( struct
_enum_struct*) _temp2367)->tag == Cyc_Absyn_Subscript_e_tag){ _LL2398: _temp2397=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp2367)->f1;
goto _LL2396; _LL2396: _temp2395=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2367)->f2; goto _LL2378;} else{ goto _LL2379;} _LL2379: goto _LL2380;
_LL2370: { void* _temp2399= _temp2381; struct Cyc_Absyn_Fndecl* _temp2413;
struct Cyc_Absyn_Vardecl* _temp2415; struct Cyc_Absyn_Vardecl* _temp2417; struct
Cyc_Absyn_Vardecl* _temp2419; struct Cyc_Absyn_Vardecl* _temp2421; _LL2401: if(
_temp2399 == Cyc_Absyn_Unresolved_b){ goto _LL2402;} else{ goto _LL2403;}
_LL2403: if(( unsigned int) _temp2399 > 1u?(( struct _enum_struct*) _temp2399)->tag
== Cyc_Absyn_Funname_b_tag: 0){ _LL2414: _temp2413=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp2399)->f1; goto _LL2404;} else{ goto
_LL2405;} _LL2405: if(( unsigned int) _temp2399 > 1u?(( struct _enum_struct*)
_temp2399)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL2416: _temp2415=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp2399)->f1; goto _LL2406;} else{ goto
_LL2407;} _LL2407: if(( unsigned int) _temp2399 > 1u?(( struct _enum_struct*)
_temp2399)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL2418: _temp2417=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp2399)->f1; goto _LL2408;} else{ goto
_LL2409;} _LL2409: if(( unsigned int) _temp2399 > 1u?(( struct _enum_struct*)
_temp2399)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL2420: _temp2419=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp2399)->f1; goto _LL2410;} else{ goto
_LL2411;} _LL2411: if(( unsigned int) _temp2399 > 1u?(( struct _enum_struct*)
_temp2399)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL2422: _temp2421=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp2399)->f1; goto _LL2412;} else{ goto
_LL2400;} _LL2402: return bogus_ans; _LL2404: return({ struct _tuple6 _temp2423;
_temp2423.f1= 1; _temp2423.f2= Cyc_Absyn_HeapRgn; _temp2423;}); _LL2406: { void*
_temp2424= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_Exp*
_temp2430; struct Cyc_Absyn_Tqual* _temp2432; void* _temp2434; _LL2426: if((
unsigned int) _temp2424 > 4u?(( struct _enum_struct*) _temp2424)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2435: _temp2434=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2424)->f1;
goto _LL2433; _LL2433: _temp2432=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2424)->f2; goto _LL2431; _LL2431: _temp2430=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2424)->f3; goto _LL2427;} else{ goto
_LL2428;} _LL2428: goto _LL2429; _LL2427: return({ struct _tuple6 _temp2436;
_temp2436.f1= 1; _temp2436.f2= Cyc_Absyn_HeapRgn; _temp2436;}); _LL2429: return({
struct _tuple6 _temp2437; _temp2437.f1=( _temp2415->tq)->q_const; _temp2437.f2=
Cyc_Absyn_HeapRgn; _temp2437;}); _LL2425:;} _LL2408: { void* _temp2438= Cyc_Tcutil_compress((
void*)( e->topt)->v); struct Cyc_Absyn_Exp* _temp2444; struct Cyc_Absyn_Tqual*
_temp2446; void* _temp2448; _LL2440: if(( unsigned int) _temp2438 > 4u?(( struct
_enum_struct*) _temp2438)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2449:
_temp2448=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2438)->f1; goto
_LL2447; _LL2447: _temp2446=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2438)->f2; goto _LL2445; _LL2445: _temp2444=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2438)->f3; goto _LL2441;} else{ goto
_LL2442;} _LL2442: goto _LL2443; _LL2441: return({ struct _tuple6 _temp2450;
_temp2450.f1= 1; _temp2450.f2=( void*)( _temp2417->region)->v; _temp2450;});
_LL2443: return({ struct _tuple6 _temp2451; _temp2451.f1=( _temp2417->tq)->q_const;
_temp2451.f2=( void*)( _temp2417->region)->v; _temp2451;}); _LL2439:;} _LL2410:
_temp2421= _temp2419; goto _LL2412; _LL2412: return({ struct _tuple6 _temp2452;
_temp2452.f1=( _temp2421->tq)->q_const; _temp2452.f2=( void*)( _temp2421->region)->v;
_temp2452;}); _LL2400:;} _LL2372: { void* _temp2453= Cyc_Tcutil_compress(( void*)(
_temp2387->topt)->v); struct Cyc_Absyn_Structdecl** _temp2461; struct Cyc_Absyn_Structdecl*
_temp2463; struct Cyc_List_List* _temp2464; struct _tuple1* _temp2466; struct
Cyc_Absyn_Uniondecl** _temp2468; struct Cyc_Absyn_Uniondecl* _temp2470; struct
Cyc_List_List* _temp2471; struct _tuple1* _temp2473; _LL2455: if(( unsigned int)
_temp2453 > 4u?(( struct _enum_struct*) _temp2453)->tag == Cyc_Absyn_StructType_tag:
0){ _LL2467: _temp2466=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2453)->f1; goto _LL2465; _LL2465: _temp2464=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2453)->f2; goto _LL2462; _LL2462:
_temp2461=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2453)->f3; if( _temp2461 == 0){ goto _LL2457;} else{ _temp2463=* _temp2461;
goto _LL2456;}} else{ goto _LL2457;} _LL2457: if(( unsigned int) _temp2453 > 4u?((
struct _enum_struct*) _temp2453)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2474:
_temp2473=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2453)->f1;
goto _LL2472; _LL2472: _temp2471=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2453)->f2; goto _LL2469; _LL2469: _temp2468=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2453)->f3; if( _temp2468 == 0){ goto
_LL2459;} else{ _temp2470=* _temp2468; goto _LL2458;}} else{ goto _LL2459;}
_LL2459: goto _LL2460; _LL2456: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp2463, _temp2385); if( finfo != 0? finfo->width != 0: 0){ return({ struct
_tuple6 _temp2475; _temp2475.f1=( finfo->tq)->q_const; _temp2475.f2=( Cyc_Tcutil_addressof_props(
te, _temp2387)).f2; _temp2475;});} return bogus_ans;} _LL2458: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp2470, _temp2385); if( finfo != 0){
return({ struct _tuple6 _temp2476; _temp2476.f1=( finfo->tq)->q_const; _temp2476.f2=(
Cyc_Tcutil_addressof_props( te, _temp2387)).f2; _temp2476;});} goto _LL2460;}
_LL2460: return bogus_ans; _LL2454:;} _LL2374: { void* _temp2477= Cyc_Tcutil_compress((
void*)( _temp2391->topt)->v); struct Cyc_Absyn_PtrInfo _temp2483; struct Cyc_Absyn_Conref*
_temp2485; struct Cyc_Absyn_Tqual* _temp2487; struct Cyc_Absyn_Conref* _temp2489;
void* _temp2491; void* _temp2493; _LL2479: if(( unsigned int) _temp2477 > 4u?((
struct _enum_struct*) _temp2477)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2484:
_temp2483=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2477)->f1; _LL2494: _temp2493=( void*) _temp2483.elt_typ; goto _LL2492;
_LL2492: _temp2491=( void*) _temp2483.rgn_typ; goto _LL2490; _LL2490: _temp2489=(
struct Cyc_Absyn_Conref*) _temp2483.nullable; goto _LL2488; _LL2488: _temp2487=(
struct Cyc_Absyn_Tqual*) _temp2483.tq; goto _LL2486; _LL2486: _temp2485=( struct
Cyc_Absyn_Conref*) _temp2483.bounds; goto _LL2480;} else{ goto _LL2481;} _LL2481:
goto _LL2482; _LL2480: { void* _temp2495= Cyc_Tcutil_compress( _temp2493);
struct Cyc_Absyn_Structdecl** _temp2503; struct Cyc_Absyn_Structdecl* _temp2505;
struct Cyc_List_List* _temp2506; struct _tuple1* _temp2508; struct Cyc_Absyn_Uniondecl**
_temp2510; struct Cyc_Absyn_Uniondecl* _temp2512; struct Cyc_List_List*
_temp2513; struct _tuple1* _temp2515; _LL2497: if(( unsigned int) _temp2495 > 4u?((
struct _enum_struct*) _temp2495)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2509:
_temp2508=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2495)->f1;
goto _LL2507; _LL2507: _temp2506=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2495)->f2; goto _LL2504; _LL2504: _temp2503=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2495)->f3; if( _temp2503 == 0){ goto
_LL2499;} else{ _temp2505=* _temp2503; goto _LL2498;}} else{ goto _LL2499;}
_LL2499: if(( unsigned int) _temp2495 > 4u?(( struct _enum_struct*) _temp2495)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL2516: _temp2515=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp2495)->f1; goto _LL2514; _LL2514: _temp2513=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp2495)->f2;
goto _LL2511; _LL2511: _temp2510=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp2495)->f3; if( _temp2510 == 0){ goto _LL2501;} else{ _temp2512=* _temp2510;
goto _LL2500;}} else{ goto _LL2501;} _LL2501: goto _LL2502; _LL2498: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp2505,
_temp2389); if( finfo != 0? finfo->width != 0: 0){ return({ struct _tuple6
_temp2517; _temp2517.f1=( finfo->tq)->q_const; _temp2517.f2= _temp2491;
_temp2517;});} return bogus_ans;} _LL2500: { struct Cyc_Absyn_Structfield* finfo=
Cyc_Absyn_lookup_union_field( _temp2512, _temp2389); if( finfo != 0){ return({
struct _tuple6 _temp2518; _temp2518.f1=( finfo->tq)->q_const; _temp2518.f2=
_temp2491; _temp2518;});} return bogus_ans;} _LL2502: return bogus_ans; _LL2496:;}
_LL2482: return bogus_ans; _LL2478:;} _LL2376: { void* _temp2519= Cyc_Tcutil_compress((
void*)( _temp2393->topt)->v); struct Cyc_Absyn_PtrInfo _temp2525; struct Cyc_Absyn_Conref*
_temp2527; struct Cyc_Absyn_Tqual* _temp2529; struct Cyc_Absyn_Conref* _temp2531;
void* _temp2533; void* _temp2535; _LL2521: if(( unsigned int) _temp2519 > 4u?((
struct _enum_struct*) _temp2519)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2526:
_temp2525=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2519)->f1; _LL2536: _temp2535=( void*) _temp2525.elt_typ; goto _LL2534;
_LL2534: _temp2533=( void*) _temp2525.rgn_typ; goto _LL2532; _LL2532: _temp2531=(
struct Cyc_Absyn_Conref*) _temp2525.nullable; goto _LL2530; _LL2530: _temp2529=(
struct Cyc_Absyn_Tqual*) _temp2525.tq; goto _LL2528; _LL2528: _temp2527=( struct
Cyc_Absyn_Conref*) _temp2525.bounds; goto _LL2522;} else{ goto _LL2523;} _LL2523:
goto _LL2524; _LL2522: return({ struct _tuple6 _temp2537; _temp2537.f1=
_temp2529->q_const; _temp2537.f2= _temp2533; _temp2537;}); _LL2524: return
bogus_ans; _LL2520:;} _LL2378: { void* t= Cyc_Tcutil_compress(( void*)(
_temp2397->topt)->v); void* _temp2538= t; struct Cyc_Absyn_Exp* _temp2548;
struct Cyc_Absyn_Tqual* _temp2550; void* _temp2552; struct Cyc_List_List*
_temp2554; struct Cyc_Absyn_PtrInfo _temp2556; struct Cyc_Absyn_Conref*
_temp2558; struct Cyc_Absyn_Tqual* _temp2560; struct Cyc_Absyn_Conref* _temp2562;
void* _temp2564; void* _temp2566; _LL2540: if(( unsigned int) _temp2538 > 4u?((
struct _enum_struct*) _temp2538)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2553:
_temp2552=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2538)->f1; goto
_LL2551; _LL2551: _temp2550=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2538)->f2; goto _LL2549; _LL2549: _temp2548=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2538)->f3; goto _LL2541;} else{ goto
_LL2542;} _LL2542: if(( unsigned int) _temp2538 > 4u?(( struct _enum_struct*)
_temp2538)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL2555: _temp2554=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2538)->f1; goto _LL2543;} else{ goto
_LL2544;} _LL2544: if(( unsigned int) _temp2538 > 4u?(( struct _enum_struct*)
_temp2538)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2557: _temp2556=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2538)->f1;
_LL2567: _temp2566=( void*) _temp2556.elt_typ; goto _LL2565; _LL2565: _temp2564=(
void*) _temp2556.rgn_typ; goto _LL2563; _LL2563: _temp2562=( struct Cyc_Absyn_Conref*)
_temp2556.nullable; goto _LL2561; _LL2561: _temp2560=( struct Cyc_Absyn_Tqual*)
_temp2556.tq; goto _LL2559; _LL2559: _temp2558=( struct Cyc_Absyn_Conref*)
_temp2556.bounds; goto _LL2545;} else{ goto _LL2546;} _LL2546: goto _LL2547;
_LL2541: return({ struct _tuple6 _temp2568; _temp2568.f1= _temp2550->q_const;
_temp2568.f2=( Cyc_Tcutil_addressof_props( te, _temp2397)).f2; _temp2568;});
_LL2543: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field( _temp2554,( int)
Cyc_Evexp_eval_const_uint_exp( _temp2395)); if( finfo != 0){ return({ struct
_tuple6 _temp2569; _temp2569.f1=((* finfo).f1)->q_const; _temp2569.f2=( Cyc_Tcutil_addressof_props(
te, _temp2397)).f2; _temp2569;});} return bogus_ans;} _LL2545: return({ struct
_tuple6 _temp2570; _temp2570.f1= _temp2560->q_const; _temp2570.f2= _temp2564;
_temp2570;}); _LL2547: return bogus_ans; _LL2539:;} _LL2380: Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ char* _temp2571=( char*)"unary & applied to non-lvalue";
struct _tagged_string _temp2572; _temp2572.curr= _temp2571; _temp2572.base=
_temp2571; _temp2572.last_plus_one= _temp2571 + 30; _temp2572;})); return
bogus_ans; _LL2368:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp2573= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp2579; struct Cyc_Absyn_Tqual* _temp2581; void*
_temp2583; _LL2575: if(( unsigned int) _temp2573 > 4u?(( struct _enum_struct*)
_temp2573)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2584: _temp2583=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2573)->f1; goto _LL2582; _LL2582:
_temp2581=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2573)->f2; goto _LL2580; _LL2580: _temp2579=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2573)->f3; goto _LL2576;} else{ goto
_LL2577;} _LL2577: goto _LL2578; _LL2576: { void* _temp2587; int _temp2589;
struct _tuple6 _temp2585= Cyc_Tcutil_addressof_props( te, e); _LL2590: _temp2589=
_temp2585.f1; goto _LL2588; _LL2588: _temp2587= _temp2585.f2; goto _LL2586;
_LL2586: return Cyc_Absyn_atb_typ( _temp2583, _temp2587, _temp2581,({ struct Cyc_Absyn_Upper_b_struct*
_temp2591=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp2591->tag= Cyc_Absyn_Upper_b_tag; _temp2591->f1= e;( void*) _temp2591;}));}
_LL2578: return e_typ; _LL2574:;} void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*
loc, unsigned int i, struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( b);{ void* _temp2592=(
void*) b->v; void* _temp2600; void* _temp2602; struct Cyc_Absyn_Exp* _temp2604;
_LL2594: if(( unsigned int) _temp2592 > 1u?(( struct _enum_struct*) _temp2592)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL2601: _temp2600=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2592)->f1; if( _temp2600 == Cyc_Absyn_Unknown_b){ goto _LL2595;} else{ goto
_LL2596;}} else{ goto _LL2596;} _LL2596: if(( unsigned int) _temp2592 > 1u?((
struct _enum_struct*) _temp2592)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2603:
_temp2602=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2592)->f1; if((
unsigned int) _temp2602 > 1u?(( struct _enum_struct*) _temp2602)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2605: _temp2604=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2602)->f1; goto _LL2597;} else{ goto _LL2598;}} else{ goto _LL2598;}
_LL2598: goto _LL2599; _LL2595: return; _LL2597: if( Cyc_Evexp_eval_const_uint_exp(
_temp2604) <= i){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2606=( char*)"dereference is out of bounds"; struct _tagged_string
_temp2607; _temp2607.curr= _temp2606; _temp2607.base= _temp2606; _temp2607.last_plus_one=
_temp2606 + 29; _temp2607;}));} return; _LL2599:( void*)( b->v=( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp2608=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2608->tag= Cyc_Absyn_Eq_constr_tag;
_temp2608->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp2609=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp2609->tag= Cyc_Absyn_Upper_b_tag;
_temp2609->f1= Cyc_Absyn_uint_exp( i +( unsigned int) 1, 0);( void*) _temp2609;});(
void*) _temp2608;})); return; _LL2593:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp2610= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Enumdecl** _temp2632; struct Cyc_List_List* _temp2634;
struct _tuple1* _temp2636; void* _temp2638; void* _temp2640; struct Cyc_Absyn_Exp*
_temp2642; struct Cyc_Absyn_Tqual* _temp2644; void* _temp2646; struct Cyc_List_List*
_temp2648; struct Cyc_Absyn_Structdecl** _temp2650; struct Cyc_List_List*
_temp2652; struct _tuple1* _temp2654; struct Cyc_Absyn_Uniondecl** _temp2656;
struct Cyc_List_List* _temp2658; struct _tuple1* _temp2660; _LL2612: if(
_temp2610 == Cyc_Absyn_VoidType){ goto _LL2613;} else{ goto _LL2614;} _LL2614:
if(( unsigned int) _temp2610 > 4u?(( struct _enum_struct*) _temp2610)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL2637: _temp2636=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2610)->f1; goto _LL2635; _LL2635: _temp2634=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp2610)->f2; goto _LL2633; _LL2633:
_temp2632=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp2610)->f3; goto _LL2615;} else{ goto _LL2616;} _LL2616: if(( unsigned int)
_temp2610 > 4u?(( struct _enum_struct*) _temp2610)->tag == Cyc_Absyn_IntType_tag:
0){ _LL2641: _temp2640=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2610)->f1;
goto _LL2639; _LL2639: _temp2638=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2610)->f2; goto _LL2617;} else{ goto _LL2618;} _LL2618: if( _temp2610 ==
Cyc_Absyn_FloatType){ goto _LL2619;} else{ goto _LL2620;} _LL2620: if( _temp2610
== Cyc_Absyn_DoubleType){ goto _LL2621;} else{ goto _LL2622;} _LL2622: if((
unsigned int) _temp2610 > 4u?(( struct _enum_struct*) _temp2610)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2647: _temp2646=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2610)->f1;
goto _LL2645; _LL2645: _temp2644=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2610)->f2; goto _LL2643; _LL2643: _temp2642=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2610)->f3; goto _LL2623;} else{ goto
_LL2624;} _LL2624: if(( unsigned int) _temp2610 > 4u?(( struct _enum_struct*)
_temp2610)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL2649: _temp2648=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2610)->f1; goto _LL2625;} else{ goto
_LL2626;} _LL2626: if(( unsigned int) _temp2610 > 4u?(( struct _enum_struct*)
_temp2610)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2655: _temp2654=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2610)->f1; goto _LL2653;
_LL2653: _temp2652=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2610)->f2; goto _LL2651; _LL2651: _temp2650=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2610)->f3; goto _LL2627;} else{ goto
_LL2628;} _LL2628: if(( unsigned int) _temp2610 > 4u?(( struct _enum_struct*)
_temp2610)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2661: _temp2660=( struct
_tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2610)->f1; goto _LL2659;
_LL2659: _temp2658=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2610)->f2; goto _LL2657; _LL2657: _temp2656=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2610)->f3; goto _LL2629;} else{ goto
_LL2630;} _LL2630: goto _LL2631; _LL2613: return 1; _LL2615: if( _temp2632 == 0){
return 0;}{ struct Cyc_Absyn_Enumdecl* ed=* _temp2632; if( ed->fields == 0){
return 0;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ed->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*)
fs->hd; if( f->tvs != 0? 1: f->typs != 0){ return 0;}}} return 1;} _LL2617:
return 1; _LL2619: return 1; _LL2621: return 1; _LL2623: return Cyc_Tcutil_bits_only(
_temp2646); _LL2625: for( 0; _temp2648 != 0; _temp2648= _temp2648->tl){ if( !
Cyc_Tcutil_bits_only((*(( struct _tuple4*) _temp2648->hd)).f2)){ return 0;}}
return 1; _LL2627: if( _temp2650 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl*
sd=* _temp2650; if( sd->fields == 0){ return 0;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp2652);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 0;}}}
return 1;}} _LL2629: if( _temp2656 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=* _temp2656; if( ud->fields == 0){ return 0;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
ud->tvs, _temp2658);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 0;}}}
return 1;}} _LL2631: return 0; _LL2611:;} static int Cyc_Tcutil_cnst_exp( struct
Cyc_Tcenv_Tenv* te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp2662=(
void*) e->r; void* _temp2698; void* _temp2700; struct Cyc_Absyn_Exp* _temp2702;
void* _temp2704; struct _tuple1* _temp2706; struct Cyc_Absyn_Exp* _temp2708;
struct Cyc_Absyn_Exp* _temp2710; struct Cyc_Absyn_Exp* _temp2712; struct Cyc_Absyn_Exp*
_temp2714; struct Cyc_Absyn_Exp* _temp2716; struct Cyc_List_List* _temp2718;
struct Cyc_Absyn_Exp* _temp2720; struct Cyc_Absyn_Exp* _temp2722; void*
_temp2724; struct Cyc_Absyn_Exp* _temp2726; struct Cyc_Absyn_Exp* _temp2728;
struct Cyc_Absyn_Exp* _temp2730; struct Cyc_Absyn_Vardecl* _temp2732; struct Cyc_List_List*
_temp2734; struct Cyc_Absyn_Structdecl* _temp2736; struct Cyc_List_List*
_temp2738; struct Cyc_Core_Opt* _temp2740; struct _tuple1* _temp2742; struct Cyc_List_List*
_temp2744; void* _temp2746; struct Cyc_List_List* _temp2748; struct Cyc_Absyn_Enumfield*
_temp2750; struct Cyc_Absyn_Enumdecl* _temp2752; struct Cyc_List_List* _temp2754;
struct Cyc_Core_Opt* _temp2756; struct Cyc_Core_Opt* _temp2758; struct Cyc_Absyn_Enumfield*
_temp2760; struct Cyc_Absyn_Xenumdecl* _temp2762; struct Cyc_List_List*
_temp2764; struct Cyc_Core_Opt* _temp2766; _LL2664: if((( struct _enum_struct*)
_temp2662)->tag == Cyc_Absyn_Const_e_tag){ _LL2699: _temp2698=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp2662)->f1; goto _LL2665;} else{ goto _LL2666;}
_LL2666: if((( struct _enum_struct*) _temp2662)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL2701: _temp2700=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp2662)->f1;
goto _LL2667;} else{ goto _LL2668;} _LL2668: if((( struct _enum_struct*)
_temp2662)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL2703: _temp2702=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp2662)->f1; goto _LL2669;} else{ goto
_LL2670;} _LL2670: if((( struct _enum_struct*) _temp2662)->tag == Cyc_Absyn_Var_e_tag){
_LL2707: _temp2706=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2662)->f1; goto _LL2705; _LL2705: _temp2704=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2662)->f2; goto _LL2671;} else{ goto _LL2672;} _LL2672: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Conditional_e_tag){ _LL2713:
_temp2712=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp2662)->f1; goto _LL2711; _LL2711: _temp2710=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp2662)->f2; goto _LL2709; _LL2709:
_temp2708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp2662)->f3; goto _LL2673;} else{ goto _LL2674;} _LL2674: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL2717: _temp2716=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp2662)->f1; goto
_LL2715; _LL2715: _temp2714=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp2662)->f2; goto _LL2675;} else{ goto _LL2676;} _LL2676: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL2721:
_temp2720=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp2662)->f1; goto _LL2719; _LL2719: _temp2718=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp2662)->f2; goto _LL2677;} else{
goto _LL2678;} _LL2678: if((( struct _enum_struct*) _temp2662)->tag == Cyc_Absyn_Cast_e_tag){
_LL2725: _temp2724=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp2662)->f1;
goto _LL2723; _LL2723: _temp2722=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp2662)->f2; goto _LL2679;} else{ goto _LL2680;} _LL2680: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Address_e_tag){ _LL2727: _temp2726=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp2662)->f1;
goto _LL2681;} else{ goto _LL2682;} _LL2682: if((( struct _enum_struct*)
_temp2662)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL2733: _temp2732=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2662)->f1;
goto _LL2731; _LL2731: _temp2730=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2662)->f2; goto _LL2729; _LL2729: _temp2728=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2662)->f3; goto _LL2683;} else{
goto _LL2684;} _LL2684: if((( struct _enum_struct*) _temp2662)->tag == Cyc_Absyn_Array_e_tag){
_LL2735: _temp2734=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp2662)->f1; goto _LL2685;} else{ goto _LL2686;} _LL2686: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Struct_e_tag){ _LL2743: _temp2742=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp2662)->f1; goto
_LL2741; _LL2741: _temp2740=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp2662)->f2; goto _LL2739; _LL2739: _temp2738=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp2662)->f3; goto _LL2737; _LL2737:
_temp2736=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp2662)->f4; goto _LL2687;} else{ goto _LL2688;} _LL2688: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Primop_e_tag){ _LL2747: _temp2746=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp2662)->f1; goto _LL2745;
_LL2745: _temp2744=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp2662)->f2; goto _LL2689;} else{ goto _LL2690;} _LL2690: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Tuple_e_tag){ _LL2749: _temp2748=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp2662)->f1; goto
_LL2691;} else{ goto _LL2692;} _LL2692: if((( struct _enum_struct*) _temp2662)->tag
== Cyc_Absyn_Enum_e_tag){ _LL2759: _temp2758=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp2662)->f1; goto _LL2757; _LL2757: _temp2756=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp2662)->f2; goto
_LL2755; _LL2755: _temp2754=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp2662)->f3; goto _LL2753; _LL2753: _temp2752=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp2662)->f4; goto _LL2751; _LL2751:
_temp2750=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp2662)->f5; goto _LL2693;} else{ goto _LL2694;} _LL2694: if((( struct
_enum_struct*) _temp2662)->tag == Cyc_Absyn_Xenum_e_tag){ _LL2767: _temp2766=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp2662)->f1; goto
_LL2765; _LL2765: _temp2764=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp2662)->f2; goto _LL2763; _LL2763: _temp2762=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp2662)->f3; goto _LL2761; _LL2761:
_temp2760=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp2662)->f4; goto _LL2695;} else{ goto _LL2696;} _LL2696: goto _LL2697;
_LL2665: return 1; _LL2667: return 1; _LL2669: return Cyc_Tcutil_cnst_exp( te, 1,
_temp2702); _LL2671: { struct _handler_cons _temp2768; _push_handler(& _temp2768);{
struct _xenum_struct* _temp2769=( struct _xenum_struct*) setjmp( _temp2768.handler);
if( ! _temp2769){{ void* _temp2770= Cyc_Tcenv_lookup_ordinary( te, e->loc,
_temp2706); void* _temp2778; struct Cyc_Absyn_Fndecl* _temp2780; void* _temp2782;
struct Cyc_Absyn_Vardecl* _temp2784; _LL2772: if((( struct _enum_struct*)
_temp2770)->tag == Cyc_Tcenv_VarRes_tag){ _LL2779: _temp2778=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp2770)->f1; if(( unsigned int) _temp2778 > 1u?((
struct _enum_struct*) _temp2778)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL2781:
_temp2780=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp2778)->f1; goto _LL2773;} else{ goto _LL2774;}} else{ goto _LL2774;}
_LL2774: if((( struct _enum_struct*) _temp2770)->tag == Cyc_Tcenv_VarRes_tag){
_LL2783: _temp2782=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp2770)->f1;
if(( unsigned int) _temp2782 > 1u?(( struct _enum_struct*) _temp2782)->tag ==
Cyc_Absyn_Global_b_tag: 0){ _LL2785: _temp2784=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp2782)->f1; goto _LL2775;} else{ goto
_LL2776;}} else{ goto _LL2776;} _LL2776: goto _LL2777; _LL2773: { int _temp2786=
1; _npop_handler( 0u); return _temp2786;} _LL2775: { void* _temp2787= Cyc_Tcutil_compress((
void*) _temp2784->type); struct Cyc_Absyn_Exp* _temp2793; struct Cyc_Absyn_Tqual*
_temp2795; void* _temp2797; _LL2789: if(( unsigned int) _temp2787 > 4u?(( struct
_enum_struct*) _temp2787)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2798:
_temp2797=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2787)->f1; goto
_LL2796; _LL2796: _temp2795=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2787)->f2; goto _LL2794; _LL2794: _temp2793=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2787)->f3; goto _LL2790;} else{ goto
_LL2791;} _LL2791: goto _LL2792; _LL2790: { int _temp2799= 1; _npop_handler( 0u);
return _temp2799;} _LL2792: { int _temp2800= var_okay; _npop_handler( 0u);
return _temp2800;} _LL2788:;} _LL2777: { int _temp2801= var_okay; _npop_handler(
0u); return _temp2801;} _LL2771:;}; _pop_handler();} else{ struct _xenum_struct*
_temp2803= _temp2769; _LL2805: if( _temp2803->tag == Cyc_Dict_Absent_tag){ goto
_LL2806;} else{ goto _LL2807;} _LL2807: goto _LL2808; _LL2806: return 0; _LL2808:(
void) _throw( _temp2803); _LL2804:;}}} _LL2673: return( Cyc_Tcutil_cnst_exp( te,
0, _temp2712)? Cyc_Tcutil_cnst_exp( te, 0, _temp2710): 0)? Cyc_Tcutil_cnst_exp(
te, 0, _temp2708): 0; _LL2675: return Cyc_Tcutil_cnst_exp( te, 0, _temp2716)?
Cyc_Tcutil_cnst_exp( te, 0, _temp2714): 0; _LL2677: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp2720); _LL2679: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp2722); _LL2681: return Cyc_Tcutil_cnst_exp( te, 1, _temp2726); _LL2683:
return Cyc_Tcutil_cnst_exp( te, 0, _temp2730)? Cyc_Tcutil_cnst_exp( te, 0,
_temp2728): 0; _LL2685: _temp2738= _temp2734; goto _LL2687; _LL2687: for( 0;
_temp2738 != 0; _temp2738= _temp2738->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*((
struct _tuple18*) _temp2738->hd)).f2)){ return 0;}} return 1; _LL2689: _temp2748=
_temp2744; goto _LL2691; _LL2691: _temp2754= _temp2748; goto _LL2693; _LL2693:
_temp2764= _temp2754; goto _LL2695; _LL2695: for( 0; _temp2764 != 0; _temp2764=
_temp2764->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp2764->hd)){ return 0;}} return 1; _LL2697: return 0; _LL2663:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);}