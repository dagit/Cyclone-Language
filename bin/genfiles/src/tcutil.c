#include "cyc_include.h"

 struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1;
void* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct
_tuple6{ int f1; void* f2; } ; struct _tuple7{ void* f1; void* f2; } ; struct
_tuple8{ void* f1; struct Cyc_List_List* f2; } ; struct _tuple9{ struct _tuple8*
f1; struct _tuple8* f2; } ; struct _tuple10{ struct _tagged_string* f1; struct
Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple11{ void* f1; void* f2; void* f3;
} ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; } ;
struct _tuple13{ struct _tuple12* f1; void* f2; } ; struct _tuple14{ struct Cyc_Absyn_Structfield*
f1; int f2; } ; struct _tuple15{ struct Cyc_List_List* f1; void* f2; } ; struct
_tuple16{ struct Cyc_Absyn_Structfield* f1; void* f2; } ; typedef unsigned int
Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
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
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
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
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct
Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_flush_warnings(); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_integral_promote(
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
Cyc_Tcutil_new_tvar( void* k); extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*,
struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only( void* t); char Cyc_Tcutil_TypeErr_tag[
8u]="TypeErr"; extern void Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr(
struct Cyc_Position_Segment* loc, struct _tagged_string s){ Cyc_Position_post_error(
Cyc_Position_mk_err_elab( loc, s));} void* Cyc_Tcutil_impos( struct
_tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Tcutil_TypeErr_struct*
_temp1=( struct Cyc_Tcutil_TypeErr_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcutil_TypeErr_struct));
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
_temp865, _temp873); if( Cyc_Evexp_eval_const_uint_exp( _temp871) == Cyc_Evexp_eval_const_uint_exp(
_temp863)){ return;} goto _LL688; _LL714: { struct Cyc_List_List* inst= 0;
while( _temp891 != 0) { if( _temp907 == 0){ break;} inst=({ struct Cyc_List_List*
_temp970=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp970->hd=( void*)({ struct _tuple5* _temp971=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp971->f1=( struct Cyc_Absyn_Tvar*) _temp907->hd;
_temp971->f2=({ struct Cyc_Absyn_VarType_struct* _temp972=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp972->tag= Cyc_Absyn_VarType_tag;
_temp972->f1=( struct Cyc_Absyn_Tvar*) _temp891->hd;( void*) _temp972;});
_temp971;}); _temp970->tl= inst; _temp970;}); _temp891= _temp891->tl; _temp907=
_temp907->tl;} if( _temp907 != 0){ goto _LL688;} if( inst != 0){ Cyc_Tcutil_unify_it(({
struct Cyc_Absyn_FnType_struct* _temp973=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp973->tag= Cyc_Absyn_FnType_tag;
_temp973->f1=({ struct Cyc_Absyn_FnInfo _temp974; _temp974.tvars= 0; _temp974.effect=
_temp889; _temp974.ret_typ=( void*) _temp887; _temp974.args= _temp885; _temp974.varargs=
_temp883; _temp974.attributes= _temp881; _temp974;});( void*) _temp973;}), Cyc_Tcutil_substitute(
inst,({ struct Cyc_Absyn_FnType_struct* _temp975=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp975->tag= Cyc_Absyn_FnType_tag;
_temp975->f1=({ struct Cyc_Absyn_FnInfo _temp976; _temp976.tvars= 0; _temp976.effect=
_temp905; _temp976.ret_typ=( void*) _temp903; _temp976.args= _temp901; _temp976.varargs=
_temp899; _temp976.attributes= _temp897; _temp976;});( void*) _temp975;})));
return;} Cyc_Tcutil_unify_it( _temp887, _temp903); for( 0; _temp885 != 0?
_temp901 != 0: 0; _temp885= _temp885->tl, _temp901= _temp901->tl){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*) _temp885->hd)).f2,(*(( struct _tuple2*) _temp901->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*) _temp885->hd)).f3,(*(( struct _tuple2*) _temp901->hd)).f3);}
if( _temp885 != 0? 1: _temp901 != 0){ goto _LL688;} if( _temp883 != _temp899){
goto _LL688;} if(( _temp889 == 0? _temp905 != 0: 0)? 1:( _temp889 != 0? _temp905
== 0: 0)){ goto _LL688;} if( ! Cyc_Tcutil_unify_effect(( void*) _temp889->v,(
void*) _temp905->v)){ goto _LL688;} if( ! Cyc_Tcutil_same_atts( _temp897,
_temp881)){ goto _LL688;} return;} _LL716: for( 0; _temp911 != 0? _temp915 != 0:
0; _temp911= _temp911->tl, _temp915= _temp915->tl){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*) _temp911->hd)).f1,(*(( struct _tuple4*) _temp915->hd)).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*) _temp911->hd)).f2,(*(( struct _tuple4*) _temp915->hd)).f2);}
if( _temp911 == 0? _temp915 == 0: 0){ return;} goto _LL688; _LL718: return;
_LL720: Cyc_Tcutil_unify_it( _temp927, _temp923); return; _LL722: goto _LL724;
_LL724: goto _LL726; _LL726: goto _LL728; _LL728: if( Cyc_Tcutil_unify_effect(
t1, t2)){ return;} else{ goto _LL688;} _LL730: goto _LL688; _LL688:;}( void)
_throw(({ struct Cyc_Tcutil_Unify_struct* _temp977=( struct Cyc_Tcutil_Unify_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcutil_Unify_struct)); _temp977->tag= Cyc_Tcutil_Unify_tag;(
struct _xenum_struct*) _temp977;}));} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp978= Cyc_Tcutil_compress( t); void* _temp988; void* _temp990; _LL980:
if(( unsigned int) _temp978 > 4u?(( struct _enum_struct*) _temp978)->tag == Cyc_Absyn_IntType_tag:
0){ _LL991: _temp990=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp978)->f1;
goto _LL989; _LL989: _temp988=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp978)->f2; goto _LL981;} else{ goto _LL982;} _LL982: if( _temp978 == Cyc_Absyn_FloatType){
goto _LL983;} else{ goto _LL984;} _LL984: if( _temp978 == Cyc_Absyn_DoubleType){
goto _LL985;} else{ goto _LL986;} _LL986: goto _LL987; _LL981: return 1; _LL983:
return 1; _LL985: return 1; _LL987: return 0; _LL979:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp993=({ struct _tuple7 _temp992; _temp992.f1= t1; _temp992.f2=
t2; _temp992;}); void* _temp1015; void* _temp1017; void* _temp1019; void*
_temp1021; void* _temp1023; void* _temp1025; void* _temp1027; void* _temp1029;
void* _temp1031; void* _temp1033; void* _temp1035; void* _temp1037; void*
_temp1039; void* _temp1041; void* _temp1043; void* _temp1045; void* _temp1047;
void* _temp1049; void* _temp1051; void* _temp1053; void* _temp1055; void*
_temp1057; void* _temp1059; void* _temp1061; void* _temp1063; void* _temp1065;
void* _temp1067; void* _temp1069; void* _temp1071; void* _temp1073; void*
_temp1075; void* _temp1077; void* _temp1079; void* _temp1081; void* _temp1083;
void* _temp1085; void* _temp1087; void* _temp1089; void* _temp1091; void*
_temp1093; void* _temp1095; void* _temp1097; _LL995: _LL1018: _temp1017=
_temp993.f1; if( _temp1017 == Cyc_Absyn_DoubleType){ goto _LL1016;} else{ goto
_LL997;} _LL1016: _temp1015= _temp993.f2; if( _temp1015 == Cyc_Absyn_FloatType){
goto _LL996;} else{ goto _LL997;} _LL997: _LL1026: _temp1025= _temp993.f1; if(
_temp1025 == Cyc_Absyn_DoubleType){ goto _LL1020;} else{ goto _LL999;} _LL1020:
_temp1019= _temp993.f2; if(( unsigned int) _temp1019 > 4u?(( struct _enum_struct*)
_temp1019)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1024: _temp1023=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1019)->f1; goto _LL1022; _LL1022:
_temp1021=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1019)->f2; goto
_LL998;} else{ goto _LL999;} _LL999: _LL1034: _temp1033= _temp993.f1; if(
_temp1033 == Cyc_Absyn_FloatType){ goto _LL1028;} else{ goto _LL1001;} _LL1028:
_temp1027= _temp993.f2; if(( unsigned int) _temp1027 > 4u?(( struct _enum_struct*)
_temp1027)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1032: _temp1031=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1027)->f1; goto _LL1030; _LL1030:
_temp1029=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1027)->f2; goto
_LL1000;} else{ goto _LL1001;} _LL1001: _LL1042: _temp1041= _temp993.f1; if((
unsigned int) _temp1041 > 4u?(( struct _enum_struct*) _temp1041)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1046: _temp1045=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1041)->f1;
goto _LL1044; _LL1044: _temp1043=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1041)->f2; if( _temp1043 == Cyc_Absyn_B8){ goto _LL1036;} else{ goto
_LL1003;}} else{ goto _LL1003;} _LL1036: _temp1035= _temp993.f2; if((
unsigned int) _temp1035 > 4u?(( struct _enum_struct*) _temp1035)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1040: _temp1039=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1035)->f1;
goto _LL1038; _LL1038: _temp1037=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1035)->f2; if( _temp1037 == Cyc_Absyn_B8){ goto _LL1002;} else{ goto
_LL1003;}} else{ goto _LL1003;} _LL1003: _LL1050: _temp1049= _temp993.f1; if((
unsigned int) _temp1049 > 4u?(( struct _enum_struct*) _temp1049)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1054: _temp1053=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1049)->f1;
goto _LL1052; _LL1052: _temp1051=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1049)->f2; if( _temp1051 == Cyc_Absyn_B8){ goto _LL1048;} else{ goto
_LL1005;}} else{ goto _LL1005;} _LL1048: _temp1047= _temp993.f2; goto _LL1004;
_LL1005: _LL1058: _temp1057= _temp993.f1; if(( unsigned int) _temp1057 > 4u?((
struct _enum_struct*) _temp1057)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1062:
_temp1061=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1057)->f1; goto
_LL1060; _LL1060: _temp1059=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1057)->f2; if( _temp1059 == Cyc_Absyn_B4){ goto _LL1056;} else{ goto
_LL1007;}} else{ goto _LL1007;} _LL1056: _temp1055= _temp993.f2; if( _temp1055
== Cyc_Absyn_FloatType){ goto _LL1006;} else{ goto _LL1007;} _LL1007: _LL1070:
_temp1069= _temp993.f1; if(( unsigned int) _temp1069 > 4u?(( struct _enum_struct*)
_temp1069)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1074: _temp1073=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1069)->f1; goto _LL1072; _LL1072:
_temp1071=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1069)->f2; if(
_temp1071 == Cyc_Absyn_B4){ goto _LL1064;} else{ goto _LL1009;}} else{ goto
_LL1009;} _LL1064: _temp1063= _temp993.f2; if(( unsigned int) _temp1063 > 4u?((
struct _enum_struct*) _temp1063)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1068:
_temp1067=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1063)->f1; goto
_LL1066; _LL1066: _temp1065=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1063)->f2; if( _temp1065 == Cyc_Absyn_B2){ goto _LL1008;} else{ goto
_LL1009;}} else{ goto _LL1009;} _LL1009: _LL1082: _temp1081= _temp993.f1; if((
unsigned int) _temp1081 > 4u?(( struct _enum_struct*) _temp1081)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1086: _temp1085=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1081)->f1;
goto _LL1084; _LL1084: _temp1083=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1081)->f2; if( _temp1083 == Cyc_Absyn_B4){ goto _LL1076;} else{ goto
_LL1011;}} else{ goto _LL1011;} _LL1076: _temp1075= _temp993.f2; if((
unsigned int) _temp1075 > 4u?(( struct _enum_struct*) _temp1075)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1080: _temp1079=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1075)->f1;
goto _LL1078; _LL1078: _temp1077=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1075)->f2; if( _temp1077 == Cyc_Absyn_B1){ goto _LL1010;} else{ goto
_LL1011;}} else{ goto _LL1011;} _LL1011: _LL1094: _temp1093= _temp993.f1; if((
unsigned int) _temp1093 > 4u?(( struct _enum_struct*) _temp1093)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1098: _temp1097=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1093)->f1;
goto _LL1096; _LL1096: _temp1095=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1093)->f2; if( _temp1095 == Cyc_Absyn_B2){ goto _LL1088;} else{ goto
_LL1013;}} else{ goto _LL1013;} _LL1088: _temp1087= _temp993.f2; if((
unsigned int) _temp1087 > 4u?(( struct _enum_struct*) _temp1087)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1092: _temp1091=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1087)->f1;
goto _LL1090; _LL1090: _temp1089=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1087)->f2; if( _temp1089 == Cyc_Absyn_B1){ goto _LL1012;} else{ goto
_LL1013;}} else{ goto _LL1013;} _LL1013: goto _LL1014; _LL996: return 1; _LL998:
return 1; _LL1000: return 1; _LL1002: return 0; _LL1004: return 1; _LL1006:
return 1; _LL1008: return 1; _LL1010: return 1; _LL1012: return 1; _LL1014:
return 0; _LL994:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void*
t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct
Cyc_List_List* el= es; for( 0; el != 0; el= el->tl){ void* t1= Cyc_Tcutil_compress((
void*)((( struct Cyc_Absyn_Exp*) el->hd)->topt)->v); if( Cyc_Tcutil_is_arithmetic_type(
t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)
max_arith_type->v)){ max_arith_type=({ struct Cyc_Core_Opt* _temp1099=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1099->v=( void*)
t1; _temp1099;});}}}} if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)
max_arith_type->v)){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el= el->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*) el->hd,
t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) el->hd)->loc,({ struct
_tagged_string _temp1100= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1101= Cyc_Absynpp_typ2string(( void*)((( struct Cyc_Absyn_Exp*) el->hd)->topt)->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1100.last_plus_one
- _temp1100.curr, _temp1100.curr, _temp1101.last_plus_one - _temp1101.curr,
_temp1101.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1102= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1108; _LL1104: if(( unsigned int) _temp1102 > 4u?(( struct _enum_struct*)
_temp1102)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1109: _temp1108=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1102)->f1; goto
_LL1105;} else{ goto _LL1106;} _LL1106: goto _LL1107; _LL1105: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1103; _LL1107: return 0; _LL1103:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1110= Cyc_Tcutil_compress(
t); void* _temp1116; void* _temp1118; _LL1112: if(( unsigned int) _temp1110 > 4u?((
struct _enum_struct*) _temp1110)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1119:
_temp1118=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1110)->f1; goto
_LL1117; _LL1117: _temp1116=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1110)->f2; goto _LL1113;} else{ goto _LL1114;} _LL1114: goto _LL1115;
_LL1113: return 1; _LL1115: return 0; _LL1111:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1120=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1121; _temp1121.curr= _temp1120; _temp1121.base= _temp1120;
_temp1121.last_plus_one= _temp1120 + 44; _temp1121;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)( e->topt)->v)){ if( Cyc_Tcutil_will_lose_precision(( void*)( e->topt)->v,
Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char*
_temp1122=( char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1123; _temp1123.curr= _temp1122; _temp1123.base= _temp1122;
_temp1123.last_plus_one= _temp1122 + 44; _temp1123;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1125=({ struct _tuple7 _temp1124; _temp1124.f1= t1; _temp1124.f2= t2;
_temp1124;}); void* _temp1133; struct Cyc_Absyn_PtrInfo _temp1135; void*
_temp1137; struct Cyc_Absyn_PtrInfo _temp1139; void* _temp1141; struct Cyc_Absyn_Exp*
_temp1143; struct Cyc_Absyn_Tqual* _temp1145; void* _temp1147; void* _temp1149;
struct Cyc_Absyn_Exp* _temp1151; struct Cyc_Absyn_Tqual* _temp1153; void*
_temp1155; _LL1127: _LL1138: _temp1137= _temp1125.f1; if(( unsigned int)
_temp1137 > 4u?(( struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1140: _temp1139=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1137)->f1; goto _LL1134;} else{ goto _LL1129;} _LL1134: _temp1133=
_temp1125.f2; if(( unsigned int) _temp1133 > 4u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1136: _temp1135=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1133)->f1; goto
_LL1128;} else{ goto _LL1129;} _LL1129: _LL1150: _temp1149= _temp1125.f1; if((
unsigned int) _temp1149 > 4u?(( struct _enum_struct*) _temp1149)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1156: _temp1155=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1149)->f1;
goto _LL1154; _LL1154: _temp1153=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1149)->f2; goto _LL1152; _LL1152: _temp1151=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1149)->f3; goto _LL1142;} else{ goto
_LL1131;} _LL1142: _temp1141= _temp1125.f2; if(( unsigned int) _temp1141 > 4u?((
struct _enum_struct*) _temp1141)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1148:
_temp1147=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1141)->f1; goto
_LL1146; _LL1146: _temp1145=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1141)->f2; goto _LL1144; _LL1144: _temp1143=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1141)->f3; goto _LL1130;} else{ goto
_LL1131;} _LL1131: goto _LL1132; _LL1128: { int okay= 1; if( !(( int(*)( int(*
cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1139.nullable, _temp1135.nullable)){ void* _temp1157=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1139.nullable))->v; int _temp1163; _LL1159: if(( unsigned int) _temp1157 >
1u?(( struct _enum_struct*) _temp1157)->tag == Cyc_Absyn_Eq_constr_tag: 0){
_LL1164: _temp1163=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1157)->f1;
goto _LL1160;} else{ goto _LL1161;} _LL1161: goto _LL1162; _LL1160: okay= !
_temp1163; goto _LL1158; _LL1162:( void) _throw((( struct _xenum_struct*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1165=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1166; _temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 30; _temp1166;}))); _LL1158:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1139.bounds, _temp1135.bounds)){ struct _tuple7
_temp1168=({ struct _tuple7 _temp1167; _temp1167.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1139.bounds))->v;
_temp1167.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1135.bounds))->v; _temp1167;}); void* _temp1176;
void* _temp1178; void* _temp1180; void* _temp1182; struct Cyc_Absyn_Exp*
_temp1184; void* _temp1186; void* _temp1188; void* _temp1190; void* _temp1192;
_LL1170: _LL1181: _temp1180= _temp1168.f1; if(( unsigned int) _temp1180 > 1u?((
struct _enum_struct*) _temp1180)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1183:
_temp1182=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1180)->f1; if((
unsigned int) _temp1182 > 1u?(( struct _enum_struct*) _temp1182)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1185: _temp1184=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1182)->f1; goto _LL1177;} else{ goto _LL1172;}} else{ goto _LL1172;}
_LL1177: _temp1176= _temp1168.f2; if(( unsigned int) _temp1176 > 1u?(( struct
_enum_struct*) _temp1176)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1179:
_temp1178=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1176)->f1; if(
_temp1178 == Cyc_Absyn_Unknown_b){ goto _LL1171;} else{ goto _LL1172;}} else{
goto _LL1172;} _LL1172: _LL1191: _temp1190= _temp1168.f1; if(( unsigned int)
_temp1190 > 1u?(( struct _enum_struct*) _temp1190)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1193: _temp1192=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1190)->f1;
if( _temp1192 == Cyc_Absyn_Unknown_b){ goto _LL1187;} else{ goto _LL1174;}}
else{ goto _LL1174;} _LL1187: _temp1186= _temp1168.f2; if(( unsigned int)
_temp1186 > 1u?(( struct _enum_struct*) _temp1186)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL1189: _temp1188=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1186)->f1;
if( _temp1188 == Cyc_Absyn_Unknown_b){ goto _LL1173;} else{ goto _LL1174;}}
else{ goto _LL1174;} _LL1174: goto _LL1175; _LL1171: okay= 1; goto _LL1169;
_LL1173: okay= 1; goto _LL1169; _LL1175: okay= 0; goto _LL1169; _LL1169:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1139.elt_typ,( void*) _temp1135.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1139.rgn_typ,( void*) _temp1135.rgn_typ): 0)?
!( _temp1139.tq)->q_const? 1:( _temp1135.tq)->q_const: 0;} _LL1130: { int okay;
okay= Cyc_Evexp_eval_const_uint_exp( _temp1151) >= Cyc_Evexp_eval_const_uint_exp(
_temp1143); return( okay? Cyc_Tcutil_unify( _temp1155, _temp1147): 0)? !
_temp1153->q_const? 1: _temp1145->q_const: 0;} _LL1132: return Cyc_Tcutil_unify(
t1, t2); _LL1126:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type( t1)?
Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision( t1,
t2)){ Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ char* _temp1194=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1195; _temp1195.curr= _temp1194; _temp1195.base=
_temp1194; _temp1195.last_plus_one= _temp1194 + 44; _temp1195;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1196= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1197= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1196.last_plus_one - _temp1196.curr, _temp1196.curr, _temp1197.last_plus_one
- _temp1197.curr, _temp1197.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v);
void* t;{ void* _temp1198= old_typ; void* _temp1208; void* _temp1210; _LL1200:
if(( unsigned int) _temp1198 > 4u?(( struct _enum_struct*) _temp1198)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f1; goto _LL1209; _LL1209: _temp1208=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f2; goto _LL1201;} else{ goto _LL1202;} _LL1202: if( _temp1198 ==
Cyc_Absyn_FloatType){ goto _LL1203;} else{ goto _LL1204;} _LL1204: if( _temp1198
== Cyc_Absyn_DoubleType){ goto _LL1205;} else{ goto _LL1206;} _LL1206: goto
_LL1207; _LL1201:{ void* _temp1212= _temp1208; _LL1214: if( _temp1212 == Cyc_Absyn_B1){
goto _LL1215;} else{ goto _LL1216;} _LL1216: if( _temp1212 == Cyc_Absyn_B2){
goto _LL1217;} else{ goto _LL1218;} _LL1218: if( _temp1212 == Cyc_Absyn_B4){
goto _LL1219;} else{ goto _LL1220;} _LL1220: if( _temp1212 == Cyc_Absyn_B8){
goto _LL1221;} else{ goto _LL1213;} _LL1215: t= Cyc_Absyn_sint_t; goto _LL1213;
_LL1217: t= Cyc_Absyn_sint_t; goto _LL1213; _LL1219: t= _temp1210 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1213; _LL1221: t= _temp1210 == Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1213; _LL1213:;} goto _LL1199;
_LL1203: t= Cyc_Absyn_float_t; goto _LL1199; _LL1205: t= Cyc_Absyn_double_t;
goto _LL1199; _LL1207: return 0; _LL1199:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)(
e->topt)->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc,
t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1222= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1223= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1222.last_plus_one - _temp1222.curr, _temp1222.curr, _temp1223.last_plus_one
- _temp1223.curr, _temp1223.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1224= Cyc_Tcutil_compress( t); void* _temp1234; void*
_temp1236; _LL1226: if(( unsigned int) _temp1224 > 4u?(( struct _enum_struct*)
_temp1224)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1237: _temp1236=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1224)->f1; goto _LL1235; _LL1235:
_temp1234=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1224)->f2; goto
_LL1227;} else{ goto _LL1228;} _LL1228: if( _temp1224 == Cyc_Absyn_FloatType){
goto _LL1229;} else{ goto _LL1230;} _LL1230: if( _temp1224 == Cyc_Absyn_DoubleType){
goto _LL1231;} else{ goto _LL1232;} _LL1232: goto _LL1233; _LL1227: return 1;
_LL1229: return 1; _LL1231: return 1; _LL1233: return 0; _LL1225:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1238=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1238->f1= x->tq; _temp1238->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1238;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1239= t1; struct Cyc_List_List* _temp1249; struct Cyc_Absyn_Structdecl**
_temp1251; struct Cyc_List_List* _temp1253; struct _tuple1* _temp1255; _LL1241:
if( _temp1239 == Cyc_Absyn_VoidType){ goto _LL1242;} else{ goto _LL1243;}
_LL1243: if(( unsigned int) _temp1239 > 4u?(( struct _enum_struct*) _temp1239)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1250: _temp1249=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1239)->f1; goto _LL1244;} else{ goto
_LL1245;} _LL1245: if(( unsigned int) _temp1239 > 4u?(( struct _enum_struct*)
_temp1239)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1256: _temp1255=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1239)->f1; goto _LL1254;
_LL1254: _temp1253=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1239)->f2; goto _LL1252; _LL1252: _temp1251=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1239)->f3; goto _LL1246;} else{ goto
_LL1247;} _LL1247: goto _LL1248; _LL1242: return 0; _LL1244: return _temp1249;
_LL1246: if( _temp1251 == 0? 1: _temp1255 == 0){ return({ struct Cyc_List_List*
_temp1257=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1257->hd=( void*)({ struct _tuple4* _temp1258=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1258->f1= Cyc_Absyn_empty_tqual(); _temp1258->f2=
t1; _temp1258;}); _temp1257->tl= 0; _temp1257;});}{ struct Cyc_Absyn_Structdecl*
sd=* _temp1251; struct _tuple1 n=* _temp1255; struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1253); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1259=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1259->hd=( void*)({ struct _tuple4* _temp1260=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1260->f1= Cyc_Absyn_empty_tqual(); _temp1260->f2=
t1; _temp1260;}); _temp1259->tl= 0; _temp1259;});} return(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
inst,( struct Cyc_List_List*)( sd->fields)->v);} _LL1248: return({ struct Cyc_List_List*
_temp1261=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1261->hd=( void*)({ struct _tuple4* _temp1262=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1262->f1= Cyc_Absyn_empty_tqual(); _temp1262->f2=
t1; _temp1262;}); _temp1261->tl= 0; _temp1261;}); _LL1240:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2);
static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1, t2)){ return 1;}{ struct
Cyc_List_List* a= assume; for( 0; a != 0; a= a->tl){ if( Cyc_Tcutil_unify( t1,(*((
struct _tuple7*) a->hd)).f1)? Cyc_Tcutil_unify( t2,(*(( struct _tuple7*) a->hd)).f2):
0){ return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1264=({ struct _tuple7 _temp1263; _temp1263.f1= t1;
_temp1263.f2= t2; _temp1263;}); void* _temp1270; struct Cyc_Absyn_PtrInfo
_temp1272; struct Cyc_Absyn_Conref* _temp1274; struct Cyc_Absyn_Tqual* _temp1276;
struct Cyc_Absyn_Conref* _temp1278; void* _temp1280; void* _temp1282; void*
_temp1284; struct Cyc_Absyn_PtrInfo _temp1286; struct Cyc_Absyn_Conref*
_temp1288; struct Cyc_Absyn_Tqual* _temp1290; struct Cyc_Absyn_Conref* _temp1292;
void* _temp1294; void* _temp1296; _LL1266: _LL1285: _temp1284= _temp1264.f1; if((
unsigned int) _temp1284 > 4u?(( struct _enum_struct*) _temp1284)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1287: _temp1286=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1284)->f1; _LL1297: _temp1296=( void*) _temp1286.elt_typ; goto _LL1295;
_LL1295: _temp1294=( void*) _temp1286.rgn_typ; goto _LL1293; _LL1293: _temp1292=(
struct Cyc_Absyn_Conref*) _temp1286.nullable; goto _LL1291; _LL1291: _temp1290=(
struct Cyc_Absyn_Tqual*) _temp1286.tq; goto _LL1289; _LL1289: _temp1288=( struct
Cyc_Absyn_Conref*) _temp1286.bounds; goto _LL1271;} else{ goto _LL1268;} _LL1271:
_temp1270= _temp1264.f2; if(( unsigned int) _temp1270 > 4u?(( struct
_enum_struct*) _temp1270)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1273:
_temp1272=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1270)->f1; _LL1283: _temp1282=( void*) _temp1272.elt_typ; goto _LL1281;
_LL1281: _temp1280=( void*) _temp1272.rgn_typ; goto _LL1279; _LL1279: _temp1278=(
struct Cyc_Absyn_Conref*) _temp1272.nullable; goto _LL1277; _LL1277: _temp1276=(
struct Cyc_Absyn_Tqual*) _temp1272.tq; goto _LL1275; _LL1275: _temp1274=( struct
Cyc_Absyn_Conref*) _temp1272.bounds; goto _LL1267;} else{ goto _LL1268;} _LL1268:
goto _LL1269; _LL1267: if( _temp1290->q_const? ! _temp1276->q_const: 0){ return
0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1292,
_temp1278)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1292): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1278): 0){ return 0;}{ void* _temp1298= _temp1294; _LL1300: if( _temp1298
== Cyc_Absyn_HeapRgn){ goto _LL1301;} else{ goto _LL1302;} _LL1302: goto _LL1303;
_LL1301: goto _LL1299; _LL1303: Cyc_Tcutil_unify( _temp1294, _temp1280); goto
_LL1299; _LL1299:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1288, _temp1274)){ struct _tuple7 _temp1305=({ struct _tuple7 _temp1304;
_temp1304.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1288); _temp1304.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1274); _temp1304;}); void* _temp1313; void* _temp1315; struct Cyc_Absyn_Exp*
_temp1317; void* _temp1319; struct Cyc_Absyn_Exp* _temp1321; void* _temp1323;
struct Cyc_Absyn_Exp* _temp1325; _LL1307: _LL1316: _temp1315= _temp1305.f1; if((
unsigned int) _temp1315 > 1u?(( struct _enum_struct*) _temp1315)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1318: _temp1317=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1315)->f1; goto _LL1314;} else{ goto _LL1309;} _LL1314: _temp1313=
_temp1305.f2; if( _temp1313 == Cyc_Absyn_Unknown_b){ goto _LL1308;} else{ goto
_LL1309;} _LL1309: _LL1324: _temp1323= _temp1305.f1; if(( unsigned int)
_temp1323 > 1u?(( struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1326: _temp1325=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1323)->f1; goto _LL1320;} else{ goto _LL1311;} _LL1320: _temp1319=
_temp1305.f2; if(( unsigned int) _temp1319 > 1u?(( struct _enum_struct*)
_temp1319)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1322: _temp1321=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1319)->f1; goto _LL1310;} else{ goto
_LL1311;} _LL1311: goto _LL1312; _LL1308: goto _LL1306; _LL1310: if( Cyc_Evexp_eval_const_uint_exp(
_temp1325) < Cyc_Evexp_eval_const_uint_exp( _temp1321)){ return 0;} goto _LL1306;
_LL1312: return 0; _LL1306:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1327=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1327->hd=( void*)({ struct _tuple7* _temp1328=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1328->f1= t1; _temp1328->f2= t2; _temp1328;});
_temp1327->tl= assume; _temp1327;}), _temp1296, _temp1282); _LL1269: return 0;
_LL1265:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2= tqs2->tl, tqs1= tqs1->tl){ if( tqs1 == 0){ return 0;}{ struct
_tuple4 _temp1331; void* _temp1332; struct Cyc_Absyn_Tqual* _temp1334; struct
_tuple4* _temp1329=( struct _tuple4*) tqs1->hd; _temp1331=* _temp1329; _LL1335:
_temp1334= _temp1331.f1; goto _LL1333; _LL1333: _temp1332= _temp1331.f2; goto
_LL1330; _LL1330: { struct _tuple4 _temp1338; void* _temp1339; struct Cyc_Absyn_Tqual*
_temp1341; struct _tuple4* _temp1336=( struct _tuple4*) tqs2->hd; _temp1338=*
_temp1336; _LL1342: _temp1341= _temp1338.f1; goto _LL1340; _LL1340: _temp1339=
_temp1338.f2; goto _LL1337; _LL1337: if( _temp1341->q_const? Cyc_Tcutil_subtype(
te, assume, _temp1332, _temp1339): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1332, _temp1339)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;}{ void* _temp1343= t2; struct Cyc_Absyn_Uniondecl**
_temp1349; struct Cyc_List_List* _temp1351; struct _tuple1* _temp1353; _LL1345:
if(( unsigned int) _temp1343 > 4u?(( struct _enum_struct*) _temp1343)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL1354: _temp1353=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1343)->f1; goto _LL1352; _LL1352: _temp1351=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1343)->f2; goto _LL1350; _LL1350:
_temp1349=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1343)->f3; goto _LL1346;} else{ goto _LL1347;} _LL1347: goto _LL1348;
_LL1346: if( _temp1349 == 0){ goto _LL1344;}{ struct Cyc_Absyn_Uniondecl* ud=*
_temp1349; if( ud->fields == 0){ goto _LL1344;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
ud->tvs, _temp1351);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 1;}}}
goto _LL1344;}} _LL1348: goto _LL1344; _LL1344:;}{ void* _temp1355= t1; struct
Cyc_Absyn_Enumdecl** _temp1371; struct Cyc_List_List* _temp1373; struct _tuple1*
_temp1375; struct Cyc_Absyn_PtrInfo _temp1377; struct Cyc_Absyn_Conref*
_temp1379; struct Cyc_Absyn_Tqual* _temp1381; struct Cyc_Absyn_Conref* _temp1383;
void* _temp1385; void* _temp1387; struct Cyc_Absyn_Exp* _temp1389; struct Cyc_Absyn_Tqual*
_temp1391; void* _temp1393; void* _temp1395; void* _temp1397; _LL1357: if((
unsigned int) _temp1355 > 4u?(( struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1376: _temp1375=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1355)->f1; goto _LL1374; _LL1374: _temp1373=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1355)->f2; goto _LL1372; _LL1372:
_temp1371=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1355)->f3; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(( unsigned int)
_temp1355 > 4u?(( struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1378: _temp1377=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1355)->f1; _LL1388: _temp1387=( void*) _temp1377.elt_typ; goto _LL1386;
_LL1386: _temp1385=( void*) _temp1377.rgn_typ; goto _LL1384; _LL1384: _temp1383=(
struct Cyc_Absyn_Conref*) _temp1377.nullable; goto _LL1382; _LL1382: _temp1381=(
struct Cyc_Absyn_Tqual*) _temp1377.tq; goto _LL1380; _LL1380: _temp1379=( struct
Cyc_Absyn_Conref*) _temp1377.bounds; goto _LL1360;} else{ goto _LL1361;} _LL1361:
if(( unsigned int) _temp1355 > 4u?(( struct _enum_struct*) _temp1355)->tag ==
Cyc_Absyn_ArrayType_tag: 0){ _LL1394: _temp1393=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1355)->f1; goto _LL1392; _LL1392: _temp1391=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1355)->f2; goto _LL1390; _LL1390:
_temp1389=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1355)->f3; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(( unsigned int)
_temp1355 > 4u?(( struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1398: _temp1397=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1355)->f1;
goto _LL1396; _LL1396: _temp1395=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1355)->f2; goto _LL1364;} else{ goto _LL1365;} _LL1365: if( _temp1355 ==
Cyc_Absyn_FloatType){ goto _LL1366;} else{ goto _LL1367;} _LL1367: if( _temp1355
== Cyc_Absyn_DoubleType){ goto _LL1368;} else{ goto _LL1369;} _LL1369: goto
_LL1370; _LL1358: return Cyc_Tcutil_coerceable( t2); _LL1360: { void* _temp1399=
t2; struct Cyc_Absyn_PtrInfo _temp1405; struct Cyc_Absyn_Conref* _temp1407;
struct Cyc_Absyn_Tqual* _temp1409; struct Cyc_Absyn_Conref* _temp1411; void*
_temp1413; void* _temp1415; _LL1401: if(( unsigned int) _temp1399 > 4u?(( struct
_enum_struct*) _temp1399)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1406:
_temp1405=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1399)->f1; _LL1416: _temp1415=( void*) _temp1405.elt_typ; goto _LL1414;
_LL1414: _temp1413=( void*) _temp1405.rgn_typ; goto _LL1412; _LL1412: _temp1411=(
struct Cyc_Absyn_Conref*) _temp1405.nullable; goto _LL1410; _LL1410: _temp1409=(
struct Cyc_Absyn_Tqual*) _temp1405.tq; goto _LL1408; _LL1408: _temp1407=( struct
Cyc_Absyn_Conref*) _temp1405.bounds; goto _LL1402;} else{ goto _LL1403;} _LL1403:
goto _LL1404; _LL1402: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1417=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1417->hd=( void*)({ struct _tuple7* _temp1418=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1418->f1= t1; _temp1418->f2= t2; _temp1418;});
_temp1417->tl= 0; _temp1417;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te,( struct
Cyc_List_List*) assump, _temp1387, _temp1415)? ! _temp1381->q_const? 1:
_temp1409->q_const: 0; int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1379, _temp1407); if( ! bounds_ok){ struct _tuple7
_temp1420=({ struct _tuple7 _temp1419; _temp1419.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1379); _temp1419.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1407); _temp1419;}); void* _temp1426; struct Cyc_Absyn_Exp*
_temp1428; void* _temp1430; struct Cyc_Absyn_Exp* _temp1432; _LL1422: _LL1431:
_temp1430= _temp1420.f1; if(( unsigned int) _temp1430 > 1u?(( struct
_enum_struct*) _temp1430)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1433: _temp1432=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1430)->f1; goto
_LL1427;} else{ goto _LL1424;} _LL1427: _temp1426= _temp1420.f2; if((
unsigned int) _temp1426 > 1u?(( struct _enum_struct*) _temp1426)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1429: _temp1428=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1426)->f1; goto _LL1423;} else{ goto _LL1424;} _LL1424: goto _LL1425;
_LL1423: if( Cyc_Evexp_eval_const_uint_exp( _temp1432) >= Cyc_Evexp_eval_const_uint_exp(
_temp1428)){ bounds_ok= 1;} goto _LL1421; _LL1425: bounds_ok= 1; goto _LL1421;
_LL1421:;} return( bounds_ok? ptrsub: 0)? _temp1385 == Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1385, _temp1413): 0;} _LL1404: return 0; _LL1400:;} _LL1362: { void*
_temp1434= t2; struct Cyc_Absyn_Exp* _temp1440; struct Cyc_Absyn_Tqual*
_temp1442; void* _temp1444; _LL1436: if(( unsigned int) _temp1434 > 4u?(( struct
_enum_struct*) _temp1434)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1445:
_temp1444=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1434)->f1; goto
_LL1443; _LL1443: _temp1442=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1434)->f2; goto _LL1441; _LL1441: _temp1440=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1434)->f3; goto _LL1437;} else{ goto
_LL1438;} _LL1438: goto _LL1439; _LL1437: { int okay; okay= Cyc_Evexp_eval_const_uint_exp(
_temp1389) >= Cyc_Evexp_eval_const_uint_exp( _temp1440); return( okay? Cyc_Tcutil_unify(
_temp1393, _temp1444): 0)? ! _temp1391->q_const? 1: _temp1442->q_const: 0;}
_LL1439: return 0; _LL1435:;} _LL1364: goto _LL1366; _LL1366: goto _LL1368;
_LL1368: return Cyc_Tcutil_coerceable( t2); _LL1370: return 0; _LL1356:;}} void
Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e,
void* t){ if( ! Cyc_Tcutil_unify(( void*)( e->topt)->v, t)){ struct Cyc_Absyn_Exp*
inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp1446=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1446->tag= Cyc_Absyn_Cast_e_tag; _temp1446->f1=( void*) t; _temp1446->f2=
inner;( void*) _temp1446;})); e->topt=({ struct Cyc_Core_Opt* _temp1447=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1447->v=( void*) t;
_temp1447;});}} int Cyc_Tcutil_comparison_promote( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp1448= Cyc_Tcutil_compress(( void*)(
e->topt)->v); void* _temp1458; void* _temp1460; _LL1450: if(( unsigned int)
_temp1448 > 4u?(( struct _enum_struct*) _temp1448)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1461: _temp1460=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1448)->f1;
goto _LL1459; _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1448)->f2; goto _LL1451;} else{ goto _LL1452;} _LL1452: if( _temp1448 ==
Cyc_Absyn_FloatType){ goto _LL1453;} else{ goto _LL1454;} _LL1454: if( _temp1448
== Cyc_Absyn_DoubleType){ goto _LL1455;} else{ goto _LL1456;} _LL1456: goto
_LL1457; _LL1451:{ void* _temp1462= _temp1458; _LL1464: if( _temp1462 == Cyc_Absyn_B1){
goto _LL1465;} else{ goto _LL1466;} _LL1466: if( _temp1462 == Cyc_Absyn_B2){
goto _LL1467;} else{ goto _LL1468;} _LL1468: if( _temp1462 == Cyc_Absyn_B4){
goto _LL1469;} else{ goto _LL1470;} _LL1470: if( _temp1462 == Cyc_Absyn_B8){
goto _LL1471;} else{ goto _LL1463;} _LL1465: t= Cyc_Absyn_sint_t; goto _LL1463;
_LL1467: t= Cyc_Absyn_sint_t; goto _LL1463; _LL1469: t= _temp1460 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1463; _LL1471: return 1; _LL1463:;}
goto _LL1449; _LL1453: t= Cyc_Absyn_float_t; goto _LL1449; _LL1455: return 1;
_LL1457: return 0; _LL1449:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;}
int Cyc_Tcutil_arithmetic_promote( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ void* old_typ= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* t;{ void*
_temp1472= old_typ; void* _temp1486; void* _temp1488; struct Cyc_Absyn_Enumdecl**
_temp1490; struct Cyc_List_List* _temp1492; struct _tuple1* _temp1494; int
_temp1496; struct Cyc_Core_Opt* _temp1498; void* _temp1500; _LL1474: if((
unsigned int) _temp1472 > 4u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1489: _temp1488=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1472)->f1;
goto _LL1487; _LL1487: _temp1486=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1472)->f2; goto _LL1475;} else{ goto _LL1476;} _LL1476: if( _temp1472 ==
Cyc_Absyn_FloatType){ goto _LL1477;} else{ goto _LL1478;} _LL1478: if( _temp1472
== Cyc_Absyn_DoubleType){ goto _LL1479;} else{ goto _LL1480;} _LL1480: if((
unsigned int) _temp1472 > 4u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1495: _temp1494=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1472)->f1; goto _LL1493; _LL1493: _temp1492=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1472)->f2; goto _LL1491; _LL1491:
_temp1490=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1472)->f3; goto _LL1481;} else{ goto _LL1482;} _LL1482: if(( unsigned int)
_temp1472 > 4u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Evar_tag: 0){
_LL1501: _temp1500=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1472)->f1;
goto _LL1499; _LL1499: _temp1498=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1472)->f2; goto _LL1497; _LL1497: _temp1496=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1472)->f3; goto _LL1483;} else{ goto _LL1484;} _LL1484: goto _LL1485;
_LL1475:{ void* _temp1502= _temp1486; _LL1504: if( _temp1502 == Cyc_Absyn_B1){
goto _LL1505;} else{ goto _LL1506;} _LL1506: if( _temp1502 == Cyc_Absyn_B2){
goto _LL1507;} else{ goto _LL1508;} _LL1508: if( _temp1502 == Cyc_Absyn_B4){
goto _LL1509;} else{ goto _LL1510;} _LL1510: if( _temp1502 == Cyc_Absyn_B8){
goto _LL1511;} else{ goto _LL1503;} _LL1505: t= Cyc_Absyn_sint_t; goto _LL1503;
_LL1507: t= Cyc_Absyn_sint_t; goto _LL1503; _LL1509: t= _temp1488 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1503; _LL1511: t= _temp1488 == Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1503; _LL1503:;} goto _LL1473;
_LL1477: t= Cyc_Absyn_float_t; goto _LL1473; _LL1479: t= Cyc_Absyn_double_t;
goto _LL1473; _LL1481: t= Cyc_Absyn_sint_t; goto _LL1473; _LL1483: t= Cyc_Absyn_sint_t;
if( ! Cyc_Tcutil_unify( old_typ, t)){ return 0;} goto _LL1473; _LL1485: return 0;
_LL1473:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;} int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* t;{ void* _temp1512= old_typ; void* _temp1522; void*
_temp1524; struct Cyc_Absyn_Enumdecl** _temp1526; struct Cyc_List_List*
_temp1528; struct _tuple1* _temp1530; int _temp1532; struct Cyc_Core_Opt*
_temp1534; void* _temp1536; _LL1514: if(( unsigned int) _temp1512 > 4u?(( struct
_enum_struct*) _temp1512)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1525: _temp1524=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1512)->f1; goto _LL1523; _LL1523:
_temp1522=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1512)->f2; goto
_LL1515;} else{ goto _LL1516;} _LL1516: if(( unsigned int) _temp1512 > 4u?((
struct _enum_struct*) _temp1512)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL1531:
_temp1530=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1512)->f1;
goto _LL1529; _LL1529: _temp1528=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1512)->f2; goto _LL1527; _LL1527: _temp1526=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp1512)->f3; goto _LL1517;} else{ goto
_LL1518;} _LL1518: if(( unsigned int) _temp1512 > 4u?(( struct _enum_struct*)
_temp1512)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1537: _temp1536=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp1512)->f1; goto _LL1535; _LL1535: _temp1534=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1512)->f2; goto
_LL1533; _LL1533: _temp1532=( int)(( struct Cyc_Absyn_Evar_struct*) _temp1512)->f3;
goto _LL1519;} else{ goto _LL1520;} _LL1520: goto _LL1521; _LL1515:{ void*
_temp1538= _temp1522; _LL1540: if( _temp1538 == Cyc_Absyn_B1){ goto _LL1541;}
else{ goto _LL1542;} _LL1542: if( _temp1538 == Cyc_Absyn_B2){ goto _LL1543;}
else{ goto _LL1544;} _LL1544: if( _temp1538 == Cyc_Absyn_B4){ goto _LL1545;}
else{ goto _LL1546;} _LL1546: if( _temp1538 == Cyc_Absyn_B8){ goto _LL1547;}
else{ goto _LL1539;} _LL1541: t= Cyc_Absyn_sint_t; goto _LL1539; _LL1543: t= Cyc_Absyn_sint_t;
goto _LL1539; _LL1545: t= _temp1524 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL1539; _LL1547: t= _temp1524 == Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL1539; _LL1539:;} goto _LL1513; _LL1517: t= Cyc_Absyn_sint_t;
goto _LL1513; _LL1519: t= Cyc_Absyn_sint_t; if( ! Cyc_Tcutil_unify( old_typ, t)){
return 0;} goto _LL1513; _LL1521: return 0; _LL1513:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple7 _temp1549=({ struct _tuple7 _temp1548; _temp1548.f1= t1;
_temp1548.f2= t2; _temp1548;}); void* _temp1573; void* _temp1575; void*
_temp1577; void* _temp1579; void* _temp1581; void* _temp1583; void* _temp1585;
void* _temp1587; void* _temp1589; void* _temp1591; void* _temp1593; void*
_temp1595; void* _temp1597; void* _temp1599; void* _temp1601; void* _temp1603;
void* _temp1605; void* _temp1607; void* _temp1609; void* _temp1611; void*
_temp1613; void* _temp1615; void* _temp1617; void* _temp1619; void* _temp1621;
void* _temp1623; void* _temp1625; void* _temp1627; void* _temp1629; void*
_temp1631; void* _temp1633; void* _temp1635; _LL1551: _LL1576: _temp1575=
_temp1549.f1; if( _temp1575 == Cyc_Absyn_DoubleType){ goto _LL1574;} else{ goto
_LL1553;} _LL1574: _temp1573= _temp1549.f2; goto _LL1552; _LL1553: _LL1580:
_temp1579= _temp1549.f1; goto _LL1578; _LL1578: _temp1577= _temp1549.f2; if(
_temp1577 == Cyc_Absyn_DoubleType){ goto _LL1554;} else{ goto _LL1555;} _LL1555:
_LL1584: _temp1583= _temp1549.f1; if( _temp1583 == Cyc_Absyn_FloatType){ goto
_LL1582;} else{ goto _LL1557;} _LL1582: _temp1581= _temp1549.f2; goto _LL1556;
_LL1557: _LL1588: _temp1587= _temp1549.f1; goto _LL1586; _LL1586: _temp1585=
_temp1549.f2; if( _temp1585 == Cyc_Absyn_FloatType){ goto _LL1558;} else{ goto
_LL1559;} _LL1559: _LL1592: _temp1591= _temp1549.f1; if(( unsigned int)
_temp1591 > 4u?(( struct _enum_struct*) _temp1591)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1596: _temp1595=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1591)->f1;
if( _temp1595 == Cyc_Absyn_Unsigned){ goto _LL1594;} else{ goto _LL1561;}
_LL1594: _temp1593=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1591)->f2;
if( _temp1593 == Cyc_Absyn_B8){ goto _LL1590;} else{ goto _LL1561;}} else{ goto
_LL1561;} _LL1590: _temp1589= _temp1549.f2; goto _LL1560; _LL1561: _LL1604:
_temp1603= _temp1549.f1; goto _LL1598; _LL1598: _temp1597= _temp1549.f2; if((
unsigned int) _temp1597 > 4u?(( struct _enum_struct*) _temp1597)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1602: _temp1601=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1597)->f1;
if( _temp1601 == Cyc_Absyn_Unsigned){ goto _LL1600;} else{ goto _LL1563;}
_LL1600: _temp1599=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1597)->f2;
if( _temp1599 == Cyc_Absyn_B8){ goto _LL1562;} else{ goto _LL1563;}} else{ goto
_LL1563;} _LL1563: _LL1608: _temp1607= _temp1549.f1; if(( unsigned int)
_temp1607 > 4u?(( struct _enum_struct*) _temp1607)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1612: _temp1611=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1607)->f1;
if( _temp1611 == Cyc_Absyn_Signed){ goto _LL1610;} else{ goto _LL1565;} _LL1610:
_temp1609=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1607)->f2; if(
_temp1609 == Cyc_Absyn_B8){ goto _LL1606;} else{ goto _LL1565;}} else{ goto
_LL1565;} _LL1606: _temp1605= _temp1549.f2; goto _LL1564; _LL1565: _LL1620:
_temp1619= _temp1549.f1; goto _LL1614; _LL1614: _temp1613= _temp1549.f2; if((
unsigned int) _temp1613 > 4u?(( struct _enum_struct*) _temp1613)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1618: _temp1617=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1613)->f1;
if( _temp1617 == Cyc_Absyn_Signed){ goto _LL1616;} else{ goto _LL1567;} _LL1616:
_temp1615=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1613)->f2; if(
_temp1615 == Cyc_Absyn_B8){ goto _LL1566;} else{ goto _LL1567;}} else{ goto
_LL1567;} _LL1567: _LL1624: _temp1623= _temp1549.f1; if(( unsigned int)
_temp1623 > 4u?(( struct _enum_struct*) _temp1623)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1628: _temp1627=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1623)->f1;
if( _temp1627 == Cyc_Absyn_Unsigned){ goto _LL1626;} else{ goto _LL1569;}
_LL1626: _temp1625=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1623)->f2;
if( _temp1625 == Cyc_Absyn_B4){ goto _LL1622;} else{ goto _LL1569;}} else{ goto
_LL1569;} _LL1622: _temp1621= _temp1549.f2; goto _LL1568; _LL1569: _LL1636:
_temp1635= _temp1549.f1; goto _LL1630; _LL1630: _temp1629= _temp1549.f2; if((
unsigned int) _temp1629 > 4u?(( struct _enum_struct*) _temp1629)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1634: _temp1633=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1629)->f1;
if( _temp1633 == Cyc_Absyn_Unsigned){ goto _LL1632;} else{ goto _LL1571;}
_LL1632: _temp1631=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1629)->f2;
if( _temp1631 == Cyc_Absyn_B4){ goto _LL1570;} else{ goto _LL1571;}} else{ goto
_LL1571;} _LL1571: goto _LL1572; _LL1552: goto _LL1554; _LL1554: return Cyc_Absyn_DoubleType;
_LL1556: goto _LL1558; _LL1558: return Cyc_Absyn_FloatType; _LL1560: goto
_LL1562; _LL1562: return({ struct Cyc_Absyn_IntType_struct* _temp1637=( struct
Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1637->tag= Cyc_Absyn_IntType_tag; _temp1637->f1=( void*) Cyc_Absyn_Unsigned;
_temp1637->f2=( void*) Cyc_Absyn_B8;( void*) _temp1637;}); _LL1564: goto _LL1566;
_LL1566: return({ struct Cyc_Absyn_IntType_struct* _temp1638=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1638->tag= Cyc_Absyn_IntType_tag;
_temp1638->f1=( void*) Cyc_Absyn_Signed; _temp1638->f2=( void*) Cyc_Absyn_B8;(
void*) _temp1638;}); _LL1568: goto _LL1570; _LL1570: return({ struct Cyc_Absyn_IntType_struct*
_temp1639=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp1639->tag= Cyc_Absyn_IntType_tag; _temp1639->f1=( void*) Cyc_Absyn_Unsigned;
_temp1639->f2=( void*) Cyc_Absyn_B4;( void*) _temp1639;}); _LL1572: return({
struct Cyc_Absyn_IntType_struct* _temp1640=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp1640->tag= Cyc_Absyn_IntType_tag;
_temp1640->f1=( void*) Cyc_Absyn_Signed; _temp1640->f2=( void*) Cyc_Absyn_B4;(
void*) _temp1640;}); _LL1550:;} void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp* e){ void* _temp1641=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1647; struct Cyc_Core_Opt* _temp1649; struct Cyc_Absyn_Exp* _temp1651;
_LL1643: if((( struct _enum_struct*) _temp1641)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1652: _temp1651=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1641)->f1; goto _LL1650; _LL1650: _temp1649=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1641)->f2; if( _temp1649 == 0){ goto
_LL1648;} else{ goto _LL1645;} _LL1648: _temp1647=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1641)->f3; goto _LL1644;} else{ goto
_LL1645;} _LL1645: goto _LL1646; _LL1644: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1653=( char*)"assignment in test"; struct
_tagged_string _temp1654; _temp1654.curr= _temp1653; _temp1654.base= _temp1653;
_temp1654.last_plus_one= _temp1653 + 19; _temp1654;})); goto _LL1642; _LL1646:
goto _LL1642; _LL1642:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v == Cyc_Absyn_No_constr){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1655=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1655->tag= Cyc_Absyn_Forward_constr_tag;
_temp1655->f1= c2;( void*) _temp1655;})); return 1;} else{ if(( void*) c2->v ==
Cyc_Absyn_No_constr){( void*)( c2->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1656=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1656->tag= Cyc_Absyn_Forward_constr_tag;
_temp1656->f1= c1;( void*) _temp1656;})); return 1;} else{ void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq(
k1, k2)){( void*)( c2->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1657=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1657->tag= Cyc_Absyn_Forward_constr_tag;
_temp1657->f1= c1;( void*) _temp1657;})); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1658=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1658->tag= Cyc_Absyn_Forward_constr_tag;
_temp1658->f1= c2;( void*) _temp1658;})); return 1;} else{ return 0;}}}}} static
struct Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tvs == 0){ return({
struct Cyc_List_List* _temp1659=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1659->hd=( void*) tv; _temp1659->tl= 0; _temp1659;});}
else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ if(
Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs2->hd)->name, tv->name) == 0){
struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind; struct
Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1660=* tv->name; struct _tagged_string
_temp1661= Cyc_Absynpp_ckind2string( k1); struct _tagged_string _temp1662= Cyc_Absynpp_ckind2string(
k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1660.last_plus_one - _temp1660.curr, _temp1660.curr, _temp1661.last_plus_one
- _temp1661.curr, _temp1661.curr, _temp1662.last_plus_one - _temp1662.curr,
_temp1662.curr);}));} return tvs;} else{ if( tvs2->tl == 0){ tvs2->tl=({ struct
Cyc_List_List* _temp1663=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1663->hd=( void*) tv; _temp1663->tl= 0; _temp1663;}); return tvs;}}}}
return(( struct Cyc_List_List*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1664=( char*)"Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp1665; _temp1665.curr= _temp1664; _temp1665.base=
_temp1664; _temp1665.last_plus_one= _temp1664 + 50; _temp1665;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=
tvs->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b= b->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*) tvs->hd)->name,(( struct
Cyc_Absyn_Tvar*) b->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp1666=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1666->hd=( void*)(( struct Cyc_Absyn_Tvar*) tvs->hd);
_temp1666->tl= r; _temp1666;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} static int Cyc_Tcutil_tvar_counter= 0;
struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k){ struct _tagged_string s=
xprintf("#%d",( Cyc_Tcutil_tvar_counter ++)); return({ struct Cyc_Absyn_Tvar*
_temp1667=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp1667->name=({ struct _tagged_string* _temp1668=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1668[ 0]= s; _temp1668;});
_temp1667->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp1667;});} static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct
_tuple10* x){ return({ struct _tuple2* _temp1669=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp1669->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp1670=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1670->v=( void*)(* x).f1; _temp1670;}); _temp1669->f2=(* x).f2; _temp1669->f3=(*
x).f3; _temp1669;});} int Cyc_Tcutil_fntype_att( void* a){ void* _temp1671= a;
int _temp1685; _LL1673: if(( unsigned int) _temp1671 > 15u?(( struct
_enum_struct*) _temp1671)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL1686:
_temp1685=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp1671)->f1; goto
_LL1674;} else{ goto _LL1675;} _LL1675: if( _temp1671 == Cyc_Absyn_Stdcall_att){
goto _LL1676;} else{ goto _LL1677;} _LL1677: if( _temp1671 == Cyc_Absyn_Cdecl_att){
goto _LL1678;} else{ goto _LL1679;} _LL1679: if( _temp1671 == Cyc_Absyn_Noreturn_att){
goto _LL1680;} else{ goto _LL1681;} _LL1681: if( _temp1671 == Cyc_Absyn_Const_att){
goto _LL1682;} else{ goto _LL1683;} _LL1683: goto _LL1684; _LL1674: goto _LL1676;
_LL1676: goto _LL1678; _LL1678: goto _LL1680; _LL1680: goto _LL1682; _LL1682:
return 1; _LL1684: return 0; _LL1672:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts= atts->tl){ if( Cyc_Tcutil_fntype_att((
void*) atts->hd)){ fn_type_atts=({ struct Cyc_List_List* _temp1687=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1687->hd=( void*)(( void*) atts->hd);
_temp1687->tl= fn_type_atts; _temp1687;});}}} return({ struct Cyc_Absyn_FnType_struct*
_temp1688=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1688->tag= Cyc_Absyn_FnType_tag; _temp1688->f1=({ struct Cyc_Absyn_FnInfo
_temp1689; _temp1689.tvars= fd->tvs; _temp1689.effect= fd->effect; _temp1689.ret_typ=(
void*)(( void*) fd->ret_type); _temp1689.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp1689.varargs= fd->varargs; _temp1689.attributes=
fn_type_atts; _temp1689;});( void*) _temp1688;});} return( void*)( fd->cached_typ)->v;}
static void* Cyc_Tcutil_fst3( struct _tuple11* t){ return(* t).f1;} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp1690= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1696; struct Cyc_List_List* _temp1698; int _temp1700; struct Cyc_List_List*
_temp1702; void* _temp1704; struct Cyc_Core_Opt* _temp1706; struct Cyc_List_List*
_temp1708; _LL1692: if(( unsigned int) _temp1690 > 4u?(( struct _enum_struct*)
_temp1690)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1697: _temp1696=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1690)->f1; _LL1709: _temp1708=( struct Cyc_List_List*)
_temp1696.tvars; goto _LL1707; _LL1707: _temp1706=( struct Cyc_Core_Opt*)
_temp1696.effect; goto _LL1705; _LL1705: _temp1704=( void*) _temp1696.ret_typ;
goto _LL1703; _LL1703: _temp1702=( struct Cyc_List_List*) _temp1696.args; goto
_LL1701; _LL1701: _temp1700=( int) _temp1696.varargs; goto _LL1699; _LL1699:
_temp1698=( struct Cyc_List_List*) _temp1696.attributes; goto _LL1693;} else{
goto _LL1694;} _LL1694: goto _LL1695; _LL1693: fd->tvs= _temp1708; fd->effect=
_temp1706; goto _LL1691; _LL1695:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1710=( char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp1711; _temp1711.curr= _temp1710; _temp1711.base=
_temp1710; _temp1711.last_plus_one= _temp1710 + 38; _temp1711;})); return;
_LL1691:;} Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct
_tagged_string*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)((
struct _tagged_string*(*)( struct _tuple10* t)) Cyc_Tcutil_fst3, fd->args), loc,(
struct _tagged_string)({ char* _temp1712=( char*)"function declaration has repeated parameter";
struct _tagged_string _temp1713; _temp1713.curr= _temp1712; _temp1713.base=
_temp1712; _temp1713.last_plus_one= _temp1712 + 44; _temp1713;})); fd->cached_typ=({
struct Cyc_Core_Opt* _temp1714=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1714->v=( void*) t; _temp1714;});} static struct _tuple13*
Cyc_Tcutil_substitute_f1( struct _tuple2* y){ return({ struct _tuple13*
_temp1715=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1715->f1=({
struct _tuple12* _temp1716=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp1716->f1=(* y).f1; _temp1716->f2=(* y).f2; _temp1716;});
_temp1715->f2=(* y).f3; _temp1715;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple13* w){ void* _temp1719; struct _tuple12* _temp1721; struct
_tuple13 _temp1717=* w; _LL1722: _temp1721= _temp1717.f1; goto _LL1720; _LL1720:
_temp1719= _temp1717.f2; goto _LL1718; _LL1718: { struct Cyc_Absyn_Tqual*
_temp1725; struct Cyc_Core_Opt* _temp1727; struct _tuple12 _temp1723=* _temp1721;
_LL1728: _temp1727= _temp1723.f1; goto _LL1726; _LL1726: _temp1725= _temp1723.f2;
goto _LL1724; _LL1724: return({ struct _tuple2* _temp1729=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp1729->f1= _temp1727; _temp1729->f2=
_temp1725; _temp1729->f3= _temp1719; _temp1729;});}} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct Cyc_List_List* inst, struct Cyc_List_List* ts); void*
Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ void* _temp1730=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp1770; struct Cyc_Absyn_Structdecl**
_temp1772; struct Cyc_List_List* _temp1774; struct _tuple1* _temp1776; struct
Cyc_Absyn_Uniondecl** _temp1778; struct Cyc_List_List* _temp1780; struct _tuple1*
_temp1782; struct Cyc_Absyn_Enumdecl** _temp1784; struct Cyc_List_List*
_temp1786; struct _tuple1* _temp1788; struct Cyc_Core_Opt* _temp1790; struct Cyc_List_List*
_temp1792; struct _tuple1* _temp1794; struct Cyc_Absyn_Exp* _temp1796; struct
Cyc_Absyn_Tqual* _temp1798; void* _temp1800; struct Cyc_Absyn_PtrInfo _temp1802;
struct Cyc_Absyn_Conref* _temp1804; struct Cyc_Absyn_Tqual* _temp1806; struct
Cyc_Absyn_Conref* _temp1808; void* _temp1810; void* _temp1812; struct Cyc_Absyn_FnInfo
_temp1814; struct Cyc_List_List* _temp1816; int _temp1818; struct Cyc_List_List*
_temp1820; void* _temp1822; struct Cyc_Core_Opt* _temp1824; struct Cyc_List_List*
_temp1826; struct Cyc_List_List* _temp1828; int _temp1830; struct Cyc_Core_Opt*
_temp1832; void* _temp1834; void* _temp1836; struct Cyc_Absyn_Xenumdecl**
_temp1838; struct _tuple1* _temp1840; void* _temp1842; void* _temp1844; void*
_temp1846; struct Cyc_List_List* _temp1848; _LL1732: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_VarType_tag:
0){ _LL1771: _temp1770=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1730)->f1; goto _LL1733;} else{ goto _LL1734;} _LL1734: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_StructType_tag:
0){ _LL1777: _temp1776=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1730)->f1; goto _LL1775; _LL1775: _temp1774=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1730)->f2; goto _LL1773; _LL1773:
_temp1772=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1730)->f3; goto _LL1735;} else{ goto _LL1736;} _LL1736: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL1783: _temp1782=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1730)->f1; goto _LL1781; _LL1781: _temp1780=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1730)->f2; goto _LL1779; _LL1779:
_temp1778=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1730)->f3; goto _LL1737;} else{ goto _LL1738;} _LL1738: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1789: _temp1788=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1730)->f1; goto _LL1787; _LL1787: _temp1786=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1730)->f2; goto _LL1785; _LL1785:
_temp1784=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1730)->f3; goto _LL1739;} else{ goto _LL1740;} _LL1740: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_TypedefType_tag:
0){ _LL1795: _temp1794=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1730)->f1; goto _LL1793; _LL1793: _temp1792=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1730)->f2; goto _LL1791; _LL1791:
_temp1790=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1730)->f3; goto _LL1741;} else{ goto _LL1742;} _LL1742: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1801: _temp1800=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1730)->f1;
goto _LL1799; _LL1799: _temp1798=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1730)->f2; goto _LL1797; _LL1797: _temp1796=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1730)->f3; goto _LL1743;} else{ goto
_LL1744;} _LL1744: if(( unsigned int) _temp1730 > 4u?(( struct _enum_struct*)
_temp1730)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1803: _temp1802=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1730)->f1;
_LL1813: _temp1812=( void*) _temp1802.elt_typ; goto _LL1811; _LL1811: _temp1810=(
void*) _temp1802.rgn_typ; goto _LL1809; _LL1809: _temp1808=( struct Cyc_Absyn_Conref*)
_temp1802.nullable; goto _LL1807; _LL1807: _temp1806=( struct Cyc_Absyn_Tqual*)
_temp1802.tq; goto _LL1805; _LL1805: _temp1804=( struct Cyc_Absyn_Conref*)
_temp1802.bounds; goto _LL1745;} else{ goto _LL1746;} _LL1746: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1815: _temp1814=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1730)->f1; _LL1827: _temp1826=( struct Cyc_List_List*) _temp1814.tvars;
goto _LL1825; _LL1825: _temp1824=( struct Cyc_Core_Opt*) _temp1814.effect; goto
_LL1823; _LL1823: _temp1822=( void*) _temp1814.ret_typ; goto _LL1821; _LL1821:
_temp1820=( struct Cyc_List_List*) _temp1814.args; goto _LL1819; _LL1819:
_temp1818=( int) _temp1814.varargs; goto _LL1817; _LL1817: _temp1816=( struct
Cyc_List_List*) _temp1814.attributes; goto _LL1747;} else{ goto _LL1748;}
_LL1748: if(( unsigned int) _temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL1829: _temp1828=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1730)->f1; goto _LL1749;} else{ goto
_LL1750;} _LL1750: if(( unsigned int) _temp1730 > 4u?(( struct _enum_struct*)
_temp1730)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1835: _temp1834=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp1730)->f1; goto _LL1833; _LL1833: _temp1832=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1730)->f2; goto
_LL1831; _LL1831: _temp1830=( int)(( struct Cyc_Absyn_Evar_struct*) _temp1730)->f3;
goto _LL1751;} else{ goto _LL1752;} _LL1752: if(( unsigned int) _temp1730 > 4u?((
struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_RgnHandleType_tag: 0){
_LL1837: _temp1836=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1730)->f1;
goto _LL1753;} else{ goto _LL1754;} _LL1754: if( _temp1730 == Cyc_Absyn_VoidType){
goto _LL1755;} else{ goto _LL1756;} _LL1756: if(( unsigned int) _temp1730 > 4u?((
struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL1841:
_temp1840=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp1730)->f1;
goto _LL1839; _LL1839: _temp1838=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp1730)->f2; goto _LL1757;} else{ goto _LL1758;} _LL1758: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1845: _temp1844=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1730)->f1;
goto _LL1843; _LL1843: _temp1842=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1730)->f2; goto _LL1759;} else{ goto _LL1760;} _LL1760: if( _temp1730 ==
Cyc_Absyn_FloatType){ goto _LL1761;} else{ goto _LL1762;} _LL1762: if( _temp1730
== Cyc_Absyn_DoubleType){ goto _LL1763;} else{ goto _LL1764;} _LL1764: if(
_temp1730 == Cyc_Absyn_HeapRgn){ goto _LL1765;} else{ goto _LL1766;} _LL1766:
if(( unsigned int) _temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag ==
Cyc_Absyn_AccessEff_tag: 0){ _LL1847: _temp1846=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1730)->f1; goto _LL1767;} else{ goto _LL1768;} _LL1768: if(( unsigned int)
_temp1730 > 4u?(( struct _enum_struct*) _temp1730)->tag == Cyc_Absyn_JoinEff_tag:
0){ _LL1849: _temp1848=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1730)->f1; goto _LL1769;} else{ goto _LL1731;} _LL1733: { struct
_handler_cons _temp1850; _push_handler(& _temp1850);{ struct _xenum_struct*
_temp1851=( struct _xenum_struct*) setjmp( _temp1850.handler); if( ! _temp1851){{
void* _temp1852=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp1770); _npop_handler( 0u); return _temp1852;}; _pop_handler();} else{
struct _xenum_struct* _temp1854= _temp1851; _LL1856: if( _temp1854->tag == Cyc_Core_Not_found_tag){
goto _LL1857;} else{ goto _LL1858;} _LL1858: goto _LL1859; _LL1857: return t;
_LL1859:( void) _throw( _temp1854); _LL1855:;}}} _LL1735: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( inst, _temp1774); return new_ts == _temp1774? t:({
struct Cyc_Absyn_StructType_struct* _temp1860=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1860->tag= Cyc_Absyn_StructType_tag;
_temp1860->f1= _temp1776; _temp1860->f2= new_ts; _temp1860->f3= _temp1772;( void*)
_temp1860;});} _LL1737: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( inst,
_temp1780); return new_ts == _temp1780? t:({ struct Cyc_Absyn_UnionType_struct*
_temp1861=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1861->tag= Cyc_Absyn_UnionType_tag; _temp1861->f1= _temp1782; _temp1861->f2=
new_ts; _temp1861->f3= _temp1778;( void*) _temp1861;});} _LL1739: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( inst, _temp1786); return new_ts == _temp1786? t:({
struct Cyc_Absyn_EnumType_struct* _temp1862=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1862->tag= Cyc_Absyn_EnumType_tag;
_temp1862->f1= _temp1788; _temp1862->f2= new_ts; _temp1862->f3= _temp1784;( void*)
_temp1862;});} _LL1741: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( inst,
_temp1792); return new_ts == _temp1792? t:({ struct Cyc_Absyn_TypedefType_struct*
_temp1863=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp1863->tag= Cyc_Absyn_TypedefType_tag; _temp1863->f1= _temp1794; _temp1863->f2=
new_ts; _temp1863->f3= _temp1790;( void*) _temp1863;});} _LL1743: { void* new_t1=
Cyc_Tcutil_substitute( inst, _temp1800); return new_t1 == _temp1800? t:({ struct
Cyc_Absyn_ArrayType_struct* _temp1864=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1864->tag= Cyc_Absyn_ArrayType_tag;
_temp1864->f1=( void*) new_t1; _temp1864->f2= _temp1798; _temp1864->f3=
_temp1796;( void*) _temp1864;});} _LL1745: { void* new_t1= Cyc_Tcutil_substitute(
inst, _temp1812); void* new_r= Cyc_Tcutil_substitute( inst, _temp1810); if(
new_t1 == _temp1812? new_r == _temp1810: 0){ return t;} return({ struct Cyc_Absyn_PointerType_struct*
_temp1865=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1865->tag= Cyc_Absyn_PointerType_tag; _temp1865->f1=({ struct Cyc_Absyn_PtrInfo
_temp1866; _temp1866.elt_typ=( void*) new_t1; _temp1866.rgn_typ=( void*) new_r;
_temp1866.nullable= _temp1808; _temp1866.tq= _temp1806; _temp1866.bounds=
_temp1804; _temp1866;});( void*) _temp1865;});} _LL1747:{ struct Cyc_List_List*
p= _temp1826; for( 0; p != 0; p= p->tl){ inst=({ struct Cyc_List_List* _temp1867=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1867->hd=(
void*)({ struct _tuple5* _temp1868=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp1868->f1=( struct Cyc_Absyn_Tvar*) p->hd; _temp1868->f2=({
struct Cyc_Absyn_VarType_struct* _temp1869=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1869->tag= Cyc_Absyn_VarType_tag;
_temp1869->f1=( struct Cyc_Absyn_Tvar*) p->hd;( void*) _temp1869;}); _temp1868;});
_temp1867->tl= inst; _temp1867;});}}{ struct Cyc_List_List* _temp1872; struct
Cyc_List_List* _temp1874; struct _tuple0 _temp1870=(( struct _tuple0(*)( struct
Cyc_List_List* x)) Cyc_List_split)((( struct Cyc_List_List*(*)( struct _tuple13*(*
f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f1,
_temp1820)); _LL1875: _temp1874= _temp1870.f1; goto _LL1873; _LL1873: _temp1872=
_temp1870.f2; goto _LL1871; _LL1871: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs(
inst, _temp1872); struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1874, ts2)); struct Cyc_Core_Opt*
eff2; if( _temp1824 == 0){ eff2= 0;} else{ void* new_eff= Cyc_Tcutil_substitute(
inst,( void*) _temp1824->v); if( new_eff ==( void*) _temp1824->v){ eff2=
_temp1824;} else{ eff2=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1876=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp1876[ 0]=({
struct Cyc_Core_Opt _temp1877; _temp1877.v=( void*) new_eff; _temp1877;});
_temp1876;});}} return({ struct Cyc_Absyn_FnType_struct* _temp1878=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1878->tag= Cyc_Absyn_FnType_tag;
_temp1878->f1=({ struct Cyc_Absyn_FnInfo _temp1879; _temp1879.tvars= _temp1826;
_temp1879.effect= eff2; _temp1879.ret_typ=( void*) Cyc_Tcutil_substitute( inst,
_temp1822); _temp1879.args= args2; _temp1879.varargs= _temp1818; _temp1879.attributes=
_temp1816; _temp1879;});( void*) _temp1878;});}} _LL1749: { struct Cyc_List_List*
_temp1882; struct Cyc_List_List* _temp1884; struct _tuple0 _temp1880=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( _temp1828); _LL1885:
_temp1884= _temp1880.f1; goto _LL1883; _LL1883: _temp1882= _temp1880.f2; goto
_LL1881; _LL1881: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs( inst,
_temp1882); if( ts2 == _temp1882){ return t;}{ struct Cyc_List_List* tqts2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1884, ts2); return({ struct Cyc_Absyn_TupleType_struct* _temp1886=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1886->tag= Cyc_Absyn_TupleType_tag; _temp1886->f1= tqts2;( void*) _temp1886;});}}}
_LL1751: if( _temp1832 != 0){ return Cyc_Tcutil_substitute( inst,( void*)
_temp1832->v);} else{ return t;} _LL1753: { void* new_rt= Cyc_Tcutil_substitute(
inst, _temp1836); return new_rt == _temp1836? t:({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1887=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1887->tag= Cyc_Absyn_RgnHandleType_tag;
_temp1887->f1=( void*) new_rt;( void*) _temp1887;});} _LL1755: return t; _LL1757:
return t; _LL1759: return t; _LL1761: return t; _LL1763: return t; _LL1765:
return t; _LL1767: { void* new_r= Cyc_Tcutil_substitute( inst, _temp1846);
return new_r == _temp1846? t:({ struct Cyc_Absyn_AccessEff_struct* _temp1888=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp1888->tag= Cyc_Absyn_AccessEff_tag; _temp1888->f1=( void*) new_r;( void*)
_temp1888;});} _LL1769: { struct Cyc_List_List* new_es= Cyc_Tcutil_substs( inst,
_temp1848); return new_es == _temp1848? t:({ struct Cyc_Absyn_JoinEff_struct*
_temp1889=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1889->tag= Cyc_Absyn_JoinEff_tag; _temp1889->f1= new_es;( void*) _temp1889;});}
_LL1731:;} static struct Cyc_List_List* Cyc_Tcutil_substs( struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* old_hd=( void*)
ts->hd; struct Cyc_List_List* old_tl= ts->tl; void* new_hd= Cyc_Tcutil_substitute(
inst, old_hd); struct Cyc_List_List* new_tl= Cyc_Tcutil_substs( inst, old_tl);
if( old_hd == new_hd? old_tl == new_tl: 0){ return ts;} return( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1890=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1890[ 0]=({ struct Cyc_List_List _temp1891;
_temp1891.hd=( void*) new_hd; _temp1891.tl= new_tl; _temp1891;}); _temp1890;});}}
struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te,
void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp1892=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1892->tag= Cyc_Absyn_Const_e_tag;
_temp1892->f1=( void*) Cyc_Absyn_Null_c;( void*) _temp1892;}), loc);{ void*
_temp1893= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp1907; void*
_temp1909; void* _temp1911; void* _temp1913; void* _temp1915; _LL1895: if((
unsigned int) _temp1893 > 4u?(( struct _enum_struct*) _temp1893)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1908: _temp1907=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1893)->f1; goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int)
_temp1893 > 4u?(( struct _enum_struct*) _temp1893)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1912: _temp1911=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1893)->f1;
goto _LL1910; _LL1910: _temp1909=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1893)->f2; if( _temp1909 == Cyc_Absyn_B1){ goto _LL1898;} else{ goto
_LL1899;}} else{ goto _LL1899;} _LL1899: if(( unsigned int) _temp1893 > 4u?((
struct _enum_struct*) _temp1893)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1916:
_temp1915=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1893)->f1; goto
_LL1914; _LL1914: _temp1913=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1893)->f2; goto _LL1900;} else{ goto _LL1901;} _LL1901: if( _temp1893 ==
Cyc_Absyn_FloatType){ goto _LL1902;} else{ goto _LL1903;} _LL1903: if( _temp1893
== Cyc_Absyn_DoubleType){ goto _LL1904;} else{ goto _LL1905;} _LL1905: goto
_LL1906; _LL1896: goto _LL1894; _LL1898:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp1917=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1917->tag= Cyc_Absyn_Const_e_tag; _temp1917->f1=( void*)({ struct Cyc_Absyn_Char_c_struct*
_temp1918=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp1918->tag= Cyc_Absyn_Char_c_tag; _temp1918->f1=( void*) Cyc_Absyn_Signed;
_temp1918->f2='\000';( void*) _temp1918;});( void*) _temp1917;})); goto _LL1894;
_LL1900:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct* _temp1919=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1919->tag= Cyc_Absyn_Const_e_tag; _temp1919->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp1920=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1920->tag= Cyc_Absyn_Int_c_tag; _temp1920->f1=( void*) _temp1915; _temp1920->f2=
0;( void*) _temp1920;});( void*) _temp1919;})); if( _temp1913 != Cyc_Absyn_B4){
e= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Cast_e_struct* _temp1921=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1921->tag= Cyc_Absyn_Cast_e_tag;
_temp1921->f1=( void*) t; _temp1921->f2= e;( void*) _temp1921;}), loc);} goto
_LL1894; _LL1902:( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp1922=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1922->tag= Cyc_Absyn_Const_e_tag; _temp1922->f1=( void*)({ struct Cyc_Absyn_Float_c_struct*
_temp1923=( struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp1923->tag= Cyc_Absyn_Float_c_tag; _temp1923->f1=( struct _tagged_string)({
char* _temp1924=( char*)"0.0"; struct _tagged_string _temp1925; _temp1925.curr=
_temp1924; _temp1925.base= _temp1924; _temp1925.last_plus_one= _temp1924 + 4;
_temp1925;});( void*) _temp1923;});( void*) _temp1922;})); goto _LL1894; _LL1904:(
void*)( e->r=( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1926=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1926->tag= Cyc_Absyn_Cast_e_tag;
_temp1926->f1=( void*) t; _temp1926->f2= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp1927=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1927->tag= Cyc_Absyn_Const_e_tag; _temp1927->f1=( void*)({ struct Cyc_Absyn_Float_c_struct*
_temp1928=( struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp1928->tag= Cyc_Absyn_Float_c_tag; _temp1928->f1=( struct _tagged_string)({
char* _temp1929=( char*)"0.0"; struct _tagged_string _temp1930; _temp1930.curr=
_temp1929; _temp1930.base= _temp1929; _temp1930.last_plus_one= _temp1929 + 4;
_temp1930;});( void*) _temp1928;});( void*) _temp1927;}), loc);( void*)
_temp1926;})); goto _LL1894; _LL1906: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1931= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp1931.last_plus_one - _temp1931.curr, _temp1931.curr);})); goto _LL1894;
_LL1894:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp1932=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp1932->f1= tv; _temp1932->f2= Cyc_Absyn_new_evar( k); _temp1932;});}
struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp1933= Cyc_Tcutil_compress( t); int
_temp1973; struct Cyc_Core_Opt* _temp1975; void* _temp1977; struct Cyc_Absyn_Tvar*
_temp1979; struct Cyc_Absyn_Enumdecl** _temp1981; struct Cyc_Absyn_Enumdecl***
_temp1983; struct Cyc_List_List* _temp1984; struct _tuple1* _temp1986; struct
Cyc_Absyn_Xenumdecl** _temp1988; struct Cyc_Absyn_Xenumdecl*** _temp1990; struct
_tuple1* _temp1991; struct Cyc_Absyn_PtrInfo _temp1993; struct Cyc_Absyn_Conref*
_temp1995; struct Cyc_Absyn_Tqual* _temp1997; struct Cyc_Absyn_Conref* _temp1999;
void* _temp2001; void* _temp2003; void* _temp2005; void* _temp2007; struct Cyc_Absyn_Exp*
_temp2009; struct Cyc_Absyn_Tqual* _temp2011; void* _temp2013; struct Cyc_Absyn_FnInfo
_temp2015; struct Cyc_List_List* _temp2017; int _temp2019; struct Cyc_List_List*
_temp2021; void* _temp2023; struct Cyc_Core_Opt* _temp2025; struct Cyc_Core_Opt**
_temp2027; struct Cyc_List_List* _temp2028; struct Cyc_List_List** _temp2030;
struct Cyc_List_List* _temp2031; struct Cyc_Absyn_Structdecl** _temp2033; struct
Cyc_Absyn_Structdecl*** _temp2035; struct Cyc_List_List* _temp2036; struct
_tuple1* _temp2038; struct Cyc_Absyn_Uniondecl** _temp2040; struct Cyc_Absyn_Uniondecl***
_temp2042; struct Cyc_List_List* _temp2043; struct _tuple1* _temp2045; struct
Cyc_Core_Opt* _temp2047; struct Cyc_Core_Opt** _temp2049; struct Cyc_List_List*
_temp2050; struct _tuple1* _temp2052; void* _temp2054; void* _temp2056; struct
Cyc_List_List* _temp2058; _LL1935: if( _temp1933 == Cyc_Absyn_VoidType){ goto
_LL1936;} else{ goto _LL1937;} _LL1937: if(( unsigned int) _temp1933 > 4u?((
struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_Evar_tag: 0){ _LL1978:
_temp1977=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1933)->f1; goto _LL1976;
_LL1976: _temp1975=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp1933)->f2; goto _LL1974; _LL1974: _temp1973=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1933)->f3; goto _LL1938;} else{ goto _LL1939;} _LL1939: if(( unsigned int)
_temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_VarType_tag:
0){ _LL1980: _temp1979=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1933)->f1; goto _LL1940;} else{ goto _LL1941;} _LL1941: if(( unsigned int)
_temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL1987: _temp1986=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1933)->f1; goto _LL1985; _LL1985: _temp1984=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp1933)->f2; goto _LL1982; _LL1982:
_temp1981=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp1933)->f3; _temp1983=&(( struct Cyc_Absyn_EnumType_struct*) _temp1933)->f3;
goto _LL1942;} else{ goto _LL1943;} _LL1943: if(( unsigned int) _temp1933 > 4u?((
struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL1992:
_temp1991=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp1933)->f1;
goto _LL1989; _LL1989: _temp1988=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp1933)->f2; _temp1990=&(( struct Cyc_Absyn_XenumType_struct*) _temp1933)->f2;
goto _LL1944;} else{ goto _LL1945;} _LL1945: if(( unsigned int) _temp1933 > 4u?((
struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1994:
_temp1993=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1933)->f1; _LL2004: _temp2003=( void*) _temp1993.elt_typ; goto _LL2002;
_LL2002: _temp2001=( void*) _temp1993.rgn_typ; goto _LL2000; _LL2000: _temp1999=(
struct Cyc_Absyn_Conref*) _temp1993.nullable; goto _LL1998; _LL1998: _temp1997=(
struct Cyc_Absyn_Tqual*) _temp1993.tq; goto _LL1996; _LL1996: _temp1995=( struct
Cyc_Absyn_Conref*) _temp1993.bounds; goto _LL1946;} else{ goto _LL1947;} _LL1947:
if(( unsigned int) _temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL2008: _temp2007=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1933)->f1; goto _LL2006; _LL2006: _temp2005=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1933)->f2; goto _LL1948;} else{ goto _LL1949;} _LL1949: if( _temp1933 ==
Cyc_Absyn_FloatType){ goto _LL1950;} else{ goto _LL1951;} _LL1951: if( _temp1933
== Cyc_Absyn_DoubleType){ goto _LL1952;} else{ goto _LL1953;} _LL1953: if((
unsigned int) _temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2014: _temp2013=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1933)->f1;
goto _LL2012; _LL2012: _temp2011=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1933)->f2; goto _LL2010; _LL2010: _temp2009=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1933)->f3; goto _LL1954;} else{ goto
_LL1955;} _LL1955: if(( unsigned int) _temp1933 > 4u?(( struct _enum_struct*)
_temp1933)->tag == Cyc_Absyn_FnType_tag: 0){ _LL2016: _temp2015=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1933)->f1; _LL2029: _temp2028=( struct Cyc_List_List*)
_temp2015.tvars; _temp2030=&((( struct Cyc_Absyn_FnType_struct*) _temp1933)->f1).tvars;
goto _LL2026; _LL2026: _temp2025=( struct Cyc_Core_Opt*) _temp2015.effect;
_temp2027=&((( struct Cyc_Absyn_FnType_struct*) _temp1933)->f1).effect; goto
_LL2024; _LL2024: _temp2023=( void*) _temp2015.ret_typ; goto _LL2022; _LL2022:
_temp2021=( struct Cyc_List_List*) _temp2015.args; goto _LL2020; _LL2020:
_temp2019=( int) _temp2015.varargs; goto _LL2018; _LL2018: _temp2017=( struct
Cyc_List_List*) _temp2015.attributes; goto _LL1956;} else{ goto _LL1957;}
_LL1957: if(( unsigned int) _temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL2032: _temp2031=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1933)->f1; goto _LL1958;} else{ goto
_LL1959;} _LL1959: if(( unsigned int) _temp1933 > 4u?(( struct _enum_struct*)
_temp1933)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2039: _temp2038=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1933)->f1; goto _LL2037;
_LL2037: _temp2036=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1933)->f2; goto _LL2034; _LL2034: _temp2033=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1933)->f3; _temp2035=&(( struct Cyc_Absyn_StructType_struct*)
_temp1933)->f3; goto _LL1960;} else{ goto _LL1961;} _LL1961: if(( unsigned int)
_temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL2046: _temp2045=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1933)->f1; goto _LL2044; _LL2044: _temp2043=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1933)->f2; goto _LL2041; _LL2041:
_temp2040=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1933)->f3; _temp2042=&(( struct Cyc_Absyn_UnionType_struct*) _temp1933)->f3;
goto _LL1962;} else{ goto _LL1963;} _LL1963: if(( unsigned int) _temp1933 > 4u?((
struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2053:
_temp2052=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp1933)->f1;
goto _LL2051; _LL2051: _temp2050=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1933)->f2; goto _LL2048; _LL2048: _temp2047=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1933)->f3; _temp2049=&(( struct Cyc_Absyn_TypedefType_struct*)
_temp1933)->f3; goto _LL1964;} else{ goto _LL1965;} _LL1965: if( _temp1933 ==
Cyc_Absyn_HeapRgn){ goto _LL1966;} else{ goto _LL1967;} _LL1967: if((
unsigned int) _temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL2055: _temp2054=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1933)->f1; goto _LL1968;} else{ goto _LL1969;} _LL1969: if(( unsigned int)
_temp1933 > 4u?(( struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL2057: _temp2056=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1933)->f1;
goto _LL1970;} else{ goto _LL1971;} _LL1971: if(( unsigned int) _temp1933 > 4u?((
struct _enum_struct*) _temp1933)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2059:
_temp2058=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp1933)->f1;
goto _LL1972;} else{ goto _LL1934;} _LL1936: goto _LL1934; _LL1938: goto _LL1934;
_LL1940: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1979->kind); if(( void*) c->v == Cyc_Absyn_No_constr){(
void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2060=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2060->tag= Cyc_Absyn_Eq_constr_tag; _temp2060->f1=( void*) expected_kind;(
void*) _temp2060;}));} free_vars= Cyc_Tcutil_add_free_tvar( loc, free_vars,
_temp1979); goto _LL1934;} _LL1942: if( _temp1986 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2061=( char*)"missing enum name"; struct
_tagged_string _temp2062; _temp2062.curr= _temp2061; _temp2062.base= _temp2061;
_temp2062.last_plus_one= _temp2061 + 18; _temp2062;})); return free_vars;} if(*
_temp1983 == 0){ struct _handler_cons _temp2063; _push_handler(& _temp2063);{
struct _xenum_struct* _temp2064=( struct _xenum_struct*) setjmp( _temp2063.handler);
if( ! _temp2064){* _temp1983=( struct Cyc_Absyn_Enumdecl**) Cyc_Tcenv_lookup_enumdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2065= _temp1986; if( _temp2065
== 0){ _throw( Null_Exception);} _temp2065;}));; _pop_handler();} else{ struct
_xenum_struct* _temp2067= _temp2064; _LL2069: if( _temp2067->tag == Cyc_Dict_Absent_tag){
goto _LL2070;} else{ goto _LL2071;} _LL2071: goto _LL2072; _LL2070: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2074= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2073= _temp1986; if( _temp2073 == 0){ _throw(
Null_Exception);} _temp2073;})); xprintf("unbound type enum %.*s", _temp2074.last_plus_one
- _temp2074.curr, _temp2074.curr);})); return free_vars; _LL2072:( void) _throw(
_temp2067); _LL2068:;}}}{ struct Cyc_Absyn_Enumdecl* ed=*(* _temp1983); if( ed->name
!= 0){* _temp1986=({ struct _tuple1* _temp2075=( struct _tuple1*)( ed->name)->v;
unsigned int _temp2076= 0; if( _temp2076 >= 1u){ _throw( Null_Exception);}
_temp2075[ _temp2076];});}{ struct Cyc_List_List* tvs= ed->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1984); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2078= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2077= _temp1986; if( _temp2077 == 0){ _throw(
Null_Exception);} _temp2077;})); int _temp2079= lvs; int _temp2080= largs;
xprintf("enum %.*s expects %d type arguments, not %d", _temp2078.last_plus_one -
_temp2078.curr, _temp2078.curr, _temp2079, _temp2080);}));} for( 0; _temp1984 !=
0; _temp1984= _temp1984->tl, tvs= tvs->tl){ void* t1=( void*) _temp1984->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1934;}} _LL1944: if(* _temp1990 == 0){
struct _handler_cons _temp2081; _push_handler(& _temp2081);{ struct
_xenum_struct* _temp2082=( struct _xenum_struct*) setjmp( _temp2081.handler);
if( ! _temp2082){{ struct Cyc_Core_Opt* xed= Cyc_Tcenv_lookup_xenumdecl( te, loc,
_temp1991); if( xed == 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2083= Cyc_Absynpp_qvar2string( _temp1991); xprintf("unbound xenum %.*s",
_temp2083.last_plus_one - _temp2083.curr, _temp2083.curr);}));{ struct Cyc_List_List*
_temp2084= free_vars; _npop_handler( 0u); return _temp2084;}}* _temp1990=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_Xenumdecl**) xed->v);};
_pop_handler();} else{ struct _xenum_struct* _temp2086= _temp2082; _LL2088: if(
_temp2086->tag == Cyc_Dict_Absent_tag){ goto _LL2089;} else{ goto _LL2090;}
_LL2090: goto _LL2091; _LL2089: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2092= Cyc_Absynpp_qvar2string( _temp1991); xprintf("unbound xenum %.*s",
_temp2092.last_plus_one - _temp2092.curr, _temp2092.curr);})); return free_vars;
_LL2091:( void) _throw( _temp2086); _LL2087:;}}}{ struct Cyc_Absyn_Xenumdecl*
xed=*(* _temp1990);({ struct _tuple1* _temp2093= _temp1991; unsigned int
_temp2094= 0; if( _temp2094 >= 1u){ _throw( Null_Exception);} _temp2093[
_temp2094]=({ struct _tuple1* _temp2095= xed->name; unsigned int _temp2096= 0;
if( _temp2096 >= 1u){ _throw( Null_Exception);} _temp2095[ _temp2096];});});
goto _LL1934;} _LL1946: free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars, Cyc_Absyn_AnyKind, _temp2003); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, Cyc_Absyn_RgnKind, _temp2001);{ void* _temp2097=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1995))->v; void* _temp2103; struct Cyc_Absyn_Exp* _temp2105; _LL2099: if((
unsigned int) _temp2097 > 1u?(( struct _enum_struct*) _temp2097)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2104: _temp2103=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2097)->f1;
if(( unsigned int) _temp2103 > 1u?(( struct _enum_struct*) _temp2103)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL2106: _temp2105=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp2103)->f1; goto _LL2100;} else{ goto _LL2101;}}
else{ goto _LL2101;} _LL2101: goto _LL2102; _LL2100: Cyc_Evexp_eval_const_uint_exp(
_temp2105); goto _LL2098; _LL2102: goto _LL2098; _LL2098:;} goto _LL1934;
_LL1948: goto _LL1934; _LL1950: goto _LL1934; _LL1952: goto _LL1934; _LL1954:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_MemKind,
_temp2013); Cyc_Evexp_eval_const_uint_exp( _temp2009); goto _LL1934; _LL1956:
if( _temp2017 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2017
!= 0; _temp2017= _temp2017->tl){ if( ! Cyc_Tcutil_fntype_att(( void*) _temp2017->hd)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2107= Cyc_Absyn_attribute2string((
void*) _temp2017->hd); xprintf("bad function type attribute %.*s", _temp2107.last_plus_one
- _temp2107.curr, _temp2107.curr);}));} if(( void*) _temp2017->hd == Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*) _temp2017->hd == Cyc_Absyn_Cdecl_att){ if(
seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2030);{ struct Cyc_List_List*
b=* _temp2030; for( 0; b != 0; b= b->tl){ void* _temp2108=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)
b->hd)->kind))->v; void* _temp2114; _LL2110: if(( unsigned int) _temp2108 > 1u?((
struct _enum_struct*) _temp2108)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2115:
_temp2114=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2108)->f1; if(
_temp2114 == Cyc_Absyn_MemKind){ goto _LL2111;} else{ goto _LL2112;}} else{ goto
_LL2112;} _LL2112: goto _LL2113; _LL2111: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2116=*(( struct Cyc_Absyn_Tvar*) b->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2116.last_plus_one - _temp2116.curr, _temp2116.curr);})); goto _LL2109;
_LL2113: goto _LL2109; _LL2109:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars, Cyc_Absyn_MemKind,
_temp2023); for( 0; _temp2021 != 0; _temp2021= _temp2021->tl){ new_free_vars=
Cyc_Tcutil_check_valid_type( loc, te, new_free_vars, Cyc_Absyn_MemKind,(*((
struct _tuple2*) _temp2021->hd)).f3);} if(* _temp2027 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars, Cyc_Absyn_EffKind,( void*)(* _temp2027)->v);} else{
struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar( Cyc_Absyn_EffKind); new_free_vars=({
struct Cyc_List_List* _temp2117=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2117->hd=( void*) e; _temp2117->tl= new_free_vars;
_temp2117;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs=
new_free_vars; for( 0; tvs != 0; tvs= tvs->tl){ void* _temp2118=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind))->v; void* _temp2126; void* _temp2128;
_LL2120: if(( unsigned int) _temp2118 > 1u?(( struct _enum_struct*) _temp2118)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL2127: _temp2126=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2118)->f1; if( _temp2126 == Cyc_Absyn_RgnKind){ goto _LL2121;} else{ goto
_LL2122;}} else{ goto _LL2122;} _LL2122: if(( unsigned int) _temp2118 > 1u?((
struct _enum_struct*) _temp2118)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2129:
_temp2128=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2118)->f1; if(
_temp2128 == Cyc_Absyn_EffKind){ goto _LL2123;} else{ goto _LL2124;}} else{ goto
_LL2124;} _LL2124: goto _LL2125; _LL2121: effect=({ struct Cyc_List_List*
_temp2130=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2130->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2131=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2131->tag= Cyc_Absyn_AccessEff_tag; _temp2131->f1=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2132=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2132->tag= Cyc_Absyn_VarType_tag; _temp2132->f1=( struct Cyc_Absyn_Tvar*)
tvs->hd;( void*) _temp2132;});( void*) _temp2131;}); _temp2130->tl= effect;
_temp2130;}); goto _LL2119; _LL2123: effect=({ struct Cyc_List_List* _temp2133=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2133->hd=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp2134=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2134->tag= Cyc_Absyn_VarType_tag;
_temp2134->f1=( struct Cyc_Absyn_Tvar*) tvs->hd;( void*) _temp2134;}); _temp2133->tl=
effect; _temp2133;}); goto _LL2119; _LL2125: goto _LL2119; _LL2119:;}}*
_temp2027=({ struct Cyc_Core_Opt* _temp2135=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2135->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2136=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2136->tag= Cyc_Absyn_JoinEff_tag; _temp2136->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);( void*) _temp2136;});
_temp2135;});}} if(* _temp2030 != 0){ struct Cyc_List_List* new_btvs= 0;{ struct
Cyc_List_List* bs=* _temp2030; for( 0; bs != 0; bs= bs->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*) bs->hd);{ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) bs->hd)->kind); void* _temp2137=( void*) c->v; void*
_temp2145; _LL2139: if( _temp2137 == Cyc_Absyn_No_constr){ goto _LL2140;} else{
goto _LL2141;} _LL2141: if(( unsigned int) _temp2137 > 1u?(( struct _enum_struct*)
_temp2137)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2146: _temp2145=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2137)->f1; if( _temp2145 == Cyc_Absyn_MemKind){
goto _LL2142;} else{ goto _LL2143;}} else{ goto _LL2143;} _LL2143: goto _LL2144;
_LL2140: Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp2147=*(( struct Cyc_Absyn_Tvar*)
bs->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2147.last_plus_one - _temp2147.curr, _temp2147.curr);})); goto _LL2142;
_LL2142:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2148=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2148->tag= Cyc_Absyn_Eq_constr_tag; _temp2148->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp2148;})); goto _LL2138; _LL2144: goto _LL2138; _LL2138:;}}}
new_free_vars= Cyc_Tcutil_remove_bound_tvars( new_free_vars,* _temp2030);} for(
0; new_free_vars != 0; new_free_vars= new_free_vars->tl){ free_vars= Cyc_Tcutil_add_free_tvar(
loc, free_vars,( struct Cyc_Absyn_Tvar*) new_free_vars->hd);} goto _LL1934;}
_LL1958: for( 0; _temp2031 != 0; _temp2031= _temp2031->tl){ free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, Cyc_Absyn_MemKind,(*(( struct _tuple4*) _temp2031->hd)).f2);}
goto _LL1934; _LL1960: if( _temp2038 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2149=( char*)"missing struct name"; struct
_tagged_string _temp2150; _temp2150.curr= _temp2149; _temp2150.base= _temp2149;
_temp2150.last_plus_one= _temp2149 + 20; _temp2150;})); return free_vars;} if(*
_temp2035 == 0){ struct _handler_cons _temp2151; _push_handler(& _temp2151);{
struct _xenum_struct* _temp2152=( struct _xenum_struct*) setjmp( _temp2151.handler);
if( ! _temp2152){* _temp2035=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2153= _temp2038; if( _temp2153
== 0){ _throw( Null_Exception);} _temp2153;}));; _pop_handler();} else{ struct
_xenum_struct* _temp2155= _temp2152; _LL2157: if( _temp2155->tag == Cyc_Dict_Absent_tag){
goto _LL2158;} else{ goto _LL2159;} _LL2159: goto _LL2160; _LL2158: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2162= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2161= _temp2038; if( _temp2161 == 0){ _throw(
Null_Exception);} _temp2161;})); xprintf("unbound type struct %.*s", _temp2162.last_plus_one
- _temp2162.curr, _temp2162.curr);})); return free_vars; _LL2160:( void) _throw(
_temp2155); _LL2156:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(* _temp2035); if( sd->name
!= 0){* _temp2038=({ struct _tuple1* _temp2163=( struct _tuple1*)( sd->name)->v;
unsigned int _temp2164= 0; if( _temp2164 >= 1u){ _throw( Null_Exception);}
_temp2163[ _temp2164];});}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2036); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2166= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2165= _temp2038; if( _temp2165 == 0){ _throw(
Null_Exception);} _temp2165;})); int _temp2167= lvs; int _temp2168= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _temp2166.last_plus_one
- _temp2166.curr, _temp2166.curr, _temp2167, _temp2168);}));} for( 0; _temp2036
!= 0; _temp2036= _temp2036->tl, tvs= tvs->tl){ void* t1=( void*) _temp2036->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1934;}} _LL1962: if( _temp2045 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2169=( char*)"missing union name";
struct _tagged_string _temp2170; _temp2170.curr= _temp2169; _temp2170.base=
_temp2169; _temp2170.last_plus_one= _temp2169 + 19; _temp2170;})); return
free_vars;} if(* _temp2042 == 0){ struct _handler_cons _temp2171; _push_handler(&
_temp2171);{ struct _xenum_struct* _temp2172=( struct _xenum_struct*) setjmp(
_temp2171.handler); if( ! _temp2172){* _temp2042=( struct Cyc_Absyn_Uniondecl**)
Cyc_Tcenv_lookup_uniondecl( te, loc,( struct _tuple1*)({ struct _tuple1*
_temp2173= _temp2045; if( _temp2173 == 0){ _throw( Null_Exception);} _temp2173;}));;
_pop_handler();} else{ struct _xenum_struct* _temp2175= _temp2172; _LL2177: if(
_temp2175->tag == Cyc_Dict_Absent_tag){ goto _LL2178;} else{ goto _LL2179;}
_LL2179: goto _LL2180; _LL2178: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2182= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2181= _temp2045; if( _temp2181 == 0){ _throw( Null_Exception);} _temp2181;}));
xprintf("unbound type union %.*s", _temp2182.last_plus_one - _temp2182.curr,
_temp2182.curr);})); return free_vars; _LL2180:( void) _throw( _temp2175);
_LL2176:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(* _temp2042); if( ud->name != 0){*
_temp2045=({ struct _tuple1* _temp2183=( struct _tuple1*)( ud->name)->v;
unsigned int _temp2184= 0; if( _temp2184 >= 1u){ _throw( Null_Exception);}
_temp2183[ _temp2184];});}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2043); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2186= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2185= _temp2045; if( _temp2185 == 0){ _throw(
Null_Exception);} _temp2185;})); int _temp2187= lvs; int _temp2188= largs;
xprintf("union %.*s expects %d type arguments but was given %d", _temp2186.last_plus_one
- _temp2186.curr, _temp2186.curr, _temp2187, _temp2188);}));} for( 0; _temp2043
!= 0; _temp2043= _temp2043->tl, tvs= tvs->tl){ void* t1=( void*) _temp2043->hd;
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL1934;}} _LL1964: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2189; _push_handler(& _temp2189);{ struct
_xenum_struct* _temp2190=( struct _xenum_struct*) setjmp( _temp2189.handler);
if( ! _temp2190){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc, _temp2052);;
_pop_handler();} else{ struct _xenum_struct* _temp2192= _temp2190; _LL2194: if(
_temp2192->tag == Cyc_Dict_Absent_tag){ goto _LL2195;} else{ goto _LL2196;}
_LL2196: goto _LL2197; _LL2195: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2198= Cyc_Absynpp_qvar2string( _temp2052); xprintf("unbound typedef name %.*s",
_temp2198.last_plus_one - _temp2198.curr, _temp2198.curr);})); return free_vars;
_LL2197:( void) _throw( _temp2192); _LL2193:;}}}({ struct _tuple1* _temp2199=
_temp2052; unsigned int _temp2200= 0; if( _temp2200 >= 1u){ _throw(
Null_Exception);} _temp2199[ _temp2200]=({ struct _tuple1* _temp2201= td->name;
unsigned int _temp2202= 0; if( _temp2202 >= 1u){ _throw( Null_Exception);}
_temp2201[ _temp2202];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2050; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=
ts->tl, tvs= tvs->tl){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) tvs->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k,( void*) ts->hd); inst=({ struct Cyc_List_List* _temp2203=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2203->hd=(
void*)({ struct _tuple5* _temp2204=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp2204->f1=( struct Cyc_Absyn_Tvar*) tvs->hd; _temp2204->f2=( void*)
ts->hd; _temp2204;}); _temp2203->tl= inst; _temp2203;});} if( tvs != 0? 1: ts !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2205= Cyc_Absynpp_qvar2string(
_temp2052); xprintf("typedef %.*s expects a different number of arguments",
_temp2205.last_plus_one - _temp2205.curr, _temp2205.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2049=({
struct Cyc_Core_Opt* _temp2206=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2206->v=( void*) new_typ; _temp2206;}); goto _LL1934;}}}
_LL1966: goto _LL1934; _LL1968: _temp2056= _temp2054; goto _LL1970; _LL1970:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_RgnKind,
_temp2056); goto _LL1934; _LL1972: for( 0; _temp2058 != 0; _temp2058= _temp2058->tl){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, Cyc_Absyn_EffKind,(
void*) _temp2058->hd);} goto _LL1934; _LL1934:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind(
t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2207=
Cyc_Absynpp_typ2string( t); struct _tagged_string _temp2208= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_string _temp2209= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2207.last_plus_one - _temp2207.curr, _temp2207.curr, _temp2208.last_plus_one
- _temp2208.curr, _temp2208.curr, _temp2209.last_plus_one - _temp2209.curr,
_temp2209.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x= x->tl){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) x->hd)->kind); void* _temp2210=( void*) c->v; void*
_temp2218; _LL2212: if( _temp2210 == Cyc_Absyn_No_constr){ goto _LL2213;} else{
goto _LL2214;} _LL2214: if(( unsigned int) _temp2210 > 1u?(( struct _enum_struct*)
_temp2210)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2219: _temp2218=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2210)->f1; if( _temp2218 == Cyc_Absyn_MemKind){
goto _LL2215;} else{ goto _LL2216;}} else{ goto _LL2216;} _LL2216: goto _LL2217;
_LL2213: goto _LL2215; _LL2215:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2220=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2220->tag= Cyc_Absyn_Eq_constr_tag; _temp2220->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp2220;})); goto _LL2211; _LL2217: goto _LL2211; _LL2211:;}} if(
free_tvars != 0){ void* _temp2221= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2227; struct Cyc_List_List* _temp2229; int _temp2231; struct Cyc_List_List*
_temp2233; void* _temp2235; struct Cyc_Core_Opt* _temp2237; struct Cyc_List_List*
_temp2239; struct Cyc_List_List** _temp2241; _LL2223: if(( unsigned int)
_temp2221 > 4u?(( struct _enum_struct*) _temp2221)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2228: _temp2227=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2221)->f1; _LL2240: _temp2239=( struct Cyc_List_List*) _temp2227.tvars;
_temp2241=&((( struct Cyc_Absyn_FnType_struct*) _temp2221)->f1).tvars; goto
_LL2238; _LL2238: _temp2237=( struct Cyc_Core_Opt*) _temp2227.effect; goto
_LL2236; _LL2236: _temp2235=( void*) _temp2227.ret_typ; goto _LL2234; _LL2234:
_temp2233=( struct Cyc_List_List*) _temp2227.args; goto _LL2232; _LL2232:
_temp2231=( int) _temp2227.varargs; goto _LL2230; _LL2230: _temp2229=( struct
Cyc_List_List*) _temp2227.attributes; goto _LL2224;} else{ goto _LL2225;}
_LL2225: goto _LL2226; _LL2224: if(* _temp2241 == 0){* _temp2241= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; if(( int)({ struct _tagged_string _temp2242= s; char*
_temp2244= _temp2242.curr + 0; if( _temp2244 < _temp2242.base? 1: _temp2244 >=
_temp2242.last_plus_one){ _throw( Null_Exception);}* _temp2244;}) ==( int)'?'){
Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2245= s; xprintf("unbound type variable %.*s",
_temp2245.last_plus_one - _temp2245.curr, _temp2245.curr);}));}}} goto _LL2222;
_LL2226: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2246=*(( struct Cyc_Absyn_Tvar*)
free_tvars->hd)->name; xprintf("unbound type variable %.*s", _temp2246.last_plus_one
- _temp2246.curr, _temp2246.curr);})); return; _LL2222:;}} void Cyc_Tcutil_check_type(
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
_tagged_string _temp2247=* ftvn; struct _tagged_string _temp2248= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2249= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2247.last_plus_one - _temp2247.curr, _temp2247.curr, _temp2248.last_plus_one
- _temp2248.curr, _temp2248.curr, _temp2249.last_plus_one - _temp2249.curr,
_temp2249.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2250=* ftvn; struct _tagged_string _temp2251= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2250.last_plus_one -
_temp2250.curr, _temp2250.curr, _temp2251.last_plus_one - _temp2251.curr,
_temp2251.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs= vs->tl){ struct Cyc_List_List* vs2= vs->tl; for( 0; vs2 != 0; vs2=
vs2->tl){ if( cmp(( void*) vs->hd,( void*) vs2->hd) == 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2252= msg; struct _tagged_string _temp2253= a2string((
void*) vs->hd); xprintf("%.*s: %.*s", _temp2252.last_plus_one - _temp2252.curr,
_temp2252.curr, _temp2253.last_plus_one - _temp2253.curr, _temp2253.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2254=(
char*)"duplicate type variable"; struct _tagged_string _temp2255; _temp2255.curr=
_temp2254; _temp2255.base= _temp2254; _temp2255.last_plus_one= _temp2254 + 24;
_temp2255;}));} struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd){ if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2256=( char*)"inappropriate use of abstract struct"; struct _tagged_string
_temp2257; _temp2257.curr= _temp2256; _temp2257.base= _temp2256; _temp2257.last_plus_one=
_temp2256 + 37; _temp2257;}));}{ struct Cyc_List_List* fields= 0;{ struct Cyc_List_List*
sd_fields=( struct Cyc_List_List*)( sd->fields)->v; for( 0; sd_fields != 0;
sd_fields= sd_fields->tl){ fields=({ struct Cyc_List_List* _temp2258=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2258->hd=( void*)({
struct _tuple14* _temp2259=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp2259->f1=( struct Cyc_Absyn_Structfield*) sd_fields->hd;
_temp2259->f2= 0; _temp2259;}); _temp2258->tl= fields; _temp2258;});}} fields=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{
struct Cyc_List_List* ans= 0; for( 0; des != 0; des= des->tl){ struct _tuple15
_temp2262; void* _temp2263; struct Cyc_List_List* _temp2265; struct _tuple15*
_temp2260=( struct _tuple15*) des->hd; _temp2262=* _temp2260; _LL2266: _temp2265=
_temp2262.f1; goto _LL2264; _LL2264: _temp2263= _temp2262.f2; goto _LL2261;
_LL2261: if( _temp2265 == 0){ struct Cyc_List_List* fields2= fields; for( 0;
fields2 != 0; fields2= fields2->tl){ if( !(*(( struct _tuple14*) fields2->hd)).f2){(*((
struct _tuple14*) fields2->hd)).f2= 1;(*(( struct _tuple15*) des->hd)).f1=({
struct Cyc_List_List* _temp2267=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2267->hd=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp2268=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2268->tag= Cyc_Absyn_FieldName_tag; _temp2268->f1=((*(( struct _tuple14*)
fields2->hd)).f1)->name;( void*) _temp2268;}); _temp2267->tl= 0; _temp2267;});
ans=({ struct Cyc_List_List* _temp2269=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2269->hd=( void*)({ struct _tuple16*
_temp2270=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp2270->f1=(*((
struct _tuple14*) fields2->hd)).f1; _temp2270->f2= _temp2263; _temp2270;});
_temp2269->tl= ans; _temp2269;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2271=( char*)"too many arguments to struct";
struct _tagged_string _temp2272; _temp2272.curr= _temp2271; _temp2272.base=
_temp2271; _temp2272.last_plus_one= _temp2271 + 29; _temp2272;}));}} else{ if(
_temp2265->tl != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2273=( char*)"multiple designators are not supported"; struct
_tagged_string _temp2274; _temp2274.curr= _temp2273; _temp2274.base= _temp2273;
_temp2274.last_plus_one= _temp2273 + 39; _temp2274;}));} else{ void* _temp2275=(
void*) _temp2265->hd; struct Cyc_Absyn_Exp* _temp2281; struct _tagged_string*
_temp2283; _LL2277: if((( struct _enum_struct*) _temp2275)->tag == Cyc_Absyn_ArrayElement_tag){
_LL2282: _temp2281=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp2275)->f1; goto _LL2278;} else{ goto _LL2279;} _LL2279: if((( struct
_enum_struct*) _temp2275)->tag == Cyc_Absyn_FieldName_tag){ _LL2284: _temp2283=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2275)->f1;
goto _LL2280;} else{ goto _LL2276;} _LL2278: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2285=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2286; _temp2286.curr= _temp2285; _temp2286.base=
_temp2285; _temp2286.last_plus_one= _temp2285 + 44; _temp2286;})); goto _LL2276;
_LL2280: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=
fields2->tl){ if( Cyc_String_zstrptrcmp( _temp2283,((*(( struct _tuple14*)
fields2->hd)).f1)->name) == 0){ if((*(( struct _tuple14*) fields2->hd)).f2){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2287=* _temp2283; xprintf("field %.*s has already been used as an argument",
_temp2287.last_plus_one - _temp2287.curr, _temp2287.curr);}));}(*(( struct
_tuple14*) fields2->hd)).f2= 1; ans=({ struct Cyc_List_List* _temp2288=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2288->hd=( void*)({
struct _tuple16* _temp2289=( struct _tuple16*) GC_malloc( sizeof( struct
_tuple16)); _temp2289->f1=(*(( struct _tuple14*) fields2->hd)).f1; _temp2289->f2=
_temp2263; _temp2289;}); _temp2288->tl= ans; _temp2288;}); break;}} if( fields2
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2290=* _temp2283;
xprintf("bad field designator %.*s", _temp2290.last_plus_one - _temp2290.curr,
_temp2290.curr);}));} goto _LL2276;} _LL2276:;}}} for( 0; fields != 0; fields=
fields->tl){ if( !(*(( struct _tuple14*) fields->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2291=( char*)"too few arguments to struct";
struct _tagged_string _temp2292; _temp2292.curr= _temp2291; _temp2292.base=
_temp2291; _temp2292.last_plus_one= _temp2291 + 28; _temp2292;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp2293= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp2299; struct Cyc_Absyn_Conref* _temp2301;
struct Cyc_Absyn_Tqual* _temp2303; struct Cyc_Absyn_Conref* _temp2305; void*
_temp2307; void* _temp2309; _LL2295: if(( unsigned int) _temp2293 > 4u?(( struct
_enum_struct*) _temp2293)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2300:
_temp2299=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2293)->f1; _LL2310: _temp2309=( void*) _temp2299.elt_typ; goto _LL2308;
_LL2308: _temp2307=( void*) _temp2299.rgn_typ; goto _LL2306; _LL2306: _temp2305=(
struct Cyc_Absyn_Conref*) _temp2299.nullable; goto _LL2304; _LL2304: _temp2303=(
struct Cyc_Absyn_Tqual*) _temp2299.tq; goto _LL2302; _LL2302: _temp2301=( struct
Cyc_Absyn_Conref*) _temp2299.bounds; goto _LL2296;} else{ goto _LL2297;} _LL2297:
goto _LL2298; _LL2296: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2301); void*
_temp2311=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp2319; _LL2313: if(( unsigned int)
_temp2311 > 1u?(( struct _enum_struct*) _temp2311)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL2320: _temp2319=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2311)->f1;
if( _temp2319 == Cyc_Absyn_Unknown_b){ goto _LL2314;} else{ goto _LL2315;}}
else{ goto _LL2315;} _LL2315: if( _temp2311 == Cyc_Absyn_No_constr){ goto
_LL2316;} else{ goto _LL2317;} _LL2317: goto _LL2318; _LL2314: return 1; _LL2316:(
void*)( b->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2321=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2321->tag= Cyc_Absyn_Eq_constr_tag; _temp2321->f1=( void*) Cyc_Absyn_Unknown_b;(
void*) _temp2321;})); return 1; _LL2318: return 0; _LL2312:;} _LL2298: return 0;
_LL2294:;} struct _tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp2322;
_temp2322.f1= 0; _temp2322.f2= Cyc_Absyn_HeapRgn; _temp2322;}); void* _temp2323=(
void*) e->r; void* _temp2337; struct _tuple1* _temp2339; struct _tagged_string*
_temp2341; struct Cyc_Absyn_Exp* _temp2343; struct _tagged_string* _temp2345;
struct Cyc_Absyn_Exp* _temp2347; struct Cyc_Absyn_Exp* _temp2349; struct Cyc_Absyn_Exp*
_temp2351; struct Cyc_Absyn_Exp* _temp2353; _LL2325: if((( struct _enum_struct*)
_temp2323)->tag == Cyc_Absyn_Var_e_tag){ _LL2340: _temp2339=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp2323)->f1; goto _LL2338; _LL2338: _temp2337=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp2323)->f2; goto _LL2326;} else{
goto _LL2327;} _LL2327: if((( struct _enum_struct*) _temp2323)->tag == Cyc_Absyn_StructMember_e_tag){
_LL2344: _temp2343=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2323)->f1; goto _LL2342; _LL2342: _temp2341=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp2323)->f2; goto _LL2328;} else{
goto _LL2329;} _LL2329: if((( struct _enum_struct*) _temp2323)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL2348: _temp2347=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2323)->f1; goto _LL2346; _LL2346: _temp2345=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp2323)->f2; goto _LL2330;} else{
goto _LL2331;} _LL2331: if((( struct _enum_struct*) _temp2323)->tag == Cyc_Absyn_Deref_e_tag){
_LL2350: _temp2349=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp2323)->f1; goto _LL2332;} else{ goto _LL2333;} _LL2333: if((( struct
_enum_struct*) _temp2323)->tag == Cyc_Absyn_Subscript_e_tag){ _LL2354: _temp2353=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp2323)->f1;
goto _LL2352; _LL2352: _temp2351=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2323)->f2; goto _LL2334;} else{ goto _LL2335;} _LL2335: goto _LL2336;
_LL2326: { void* _temp2355= _temp2337; struct Cyc_Absyn_Fndecl* _temp2369;
struct Cyc_Absyn_Vardecl* _temp2371; struct Cyc_Absyn_Vardecl* _temp2373; struct
Cyc_Absyn_Vardecl* _temp2375; struct Cyc_Absyn_Vardecl* _temp2377; _LL2357: if(
_temp2355 == Cyc_Absyn_Unresolved_b){ goto _LL2358;} else{ goto _LL2359;}
_LL2359: if(( unsigned int) _temp2355 > 1u?(( struct _enum_struct*) _temp2355)->tag
== Cyc_Absyn_Funname_b_tag: 0){ _LL2370: _temp2369=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp2355)->f1; goto _LL2360;} else{ goto
_LL2361;} _LL2361: if(( unsigned int) _temp2355 > 1u?(( struct _enum_struct*)
_temp2355)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL2372: _temp2371=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp2355)->f1; goto _LL2362;} else{ goto
_LL2363;} _LL2363: if(( unsigned int) _temp2355 > 1u?(( struct _enum_struct*)
_temp2355)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL2374: _temp2373=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp2355)->f1; goto _LL2364;} else{ goto
_LL2365;} _LL2365: if(( unsigned int) _temp2355 > 1u?(( struct _enum_struct*)
_temp2355)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL2376: _temp2375=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp2355)->f1; goto _LL2366;} else{ goto
_LL2367;} _LL2367: if(( unsigned int) _temp2355 > 1u?(( struct _enum_struct*)
_temp2355)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL2378: _temp2377=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp2355)->f1; goto _LL2368;} else{ goto
_LL2356;} _LL2358: return bogus_ans; _LL2360: return({ struct _tuple6 _temp2379;
_temp2379.f1= 1; _temp2379.f2= Cyc_Absyn_HeapRgn; _temp2379;}); _LL2362: { void*
_temp2380= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_Exp*
_temp2386; struct Cyc_Absyn_Tqual* _temp2388; void* _temp2390; _LL2382: if((
unsigned int) _temp2380 > 4u?(( struct _enum_struct*) _temp2380)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2391: _temp2390=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2380)->f1;
goto _LL2389; _LL2389: _temp2388=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2380)->f2; goto _LL2387; _LL2387: _temp2386=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2380)->f3; goto _LL2383;} else{ goto
_LL2384;} _LL2384: goto _LL2385; _LL2383: return({ struct _tuple6 _temp2392;
_temp2392.f1= 1; _temp2392.f2= Cyc_Absyn_HeapRgn; _temp2392;}); _LL2385: return({
struct _tuple6 _temp2393; _temp2393.f1=( _temp2371->tq)->q_const; _temp2393.f2=
Cyc_Absyn_HeapRgn; _temp2393;}); _LL2381:;} _LL2364: { void* _temp2394= Cyc_Tcutil_compress((
void*)( e->topt)->v); struct Cyc_Absyn_Exp* _temp2400; struct Cyc_Absyn_Tqual*
_temp2402; void* _temp2404; _LL2396: if(( unsigned int) _temp2394 > 4u?(( struct
_enum_struct*) _temp2394)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2405:
_temp2404=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2394)->f1; goto
_LL2403; _LL2403: _temp2402=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2394)->f2; goto _LL2401; _LL2401: _temp2400=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2394)->f3; goto _LL2397;} else{ goto
_LL2398;} _LL2398: goto _LL2399; _LL2397: return({ struct _tuple6 _temp2406;
_temp2406.f1= 1; _temp2406.f2=( void*)( _temp2373->region)->v; _temp2406;});
_LL2399: return({ struct _tuple6 _temp2407; _temp2407.f1=( _temp2373->tq)->q_const;
_temp2407.f2=( void*)( _temp2373->region)->v; _temp2407;}); _LL2395:;} _LL2366:
_temp2377= _temp2375; goto _LL2368; _LL2368: return({ struct _tuple6 _temp2408;
_temp2408.f1=( _temp2377->tq)->q_const; _temp2408.f2=( void*)( _temp2377->region)->v;
_temp2408;}); _LL2356:;} _LL2328: { void* _temp2409= Cyc_Tcutil_compress(( void*)(
_temp2343->topt)->v); struct Cyc_Absyn_Structdecl** _temp2417; struct Cyc_Absyn_Structdecl*
_temp2419; struct Cyc_List_List* _temp2420; struct _tuple1* _temp2422; struct
Cyc_Absyn_Uniondecl** _temp2424; struct Cyc_Absyn_Uniondecl* _temp2426; struct
Cyc_List_List* _temp2427; struct _tuple1* _temp2429; _LL2411: if(( unsigned int)
_temp2409 > 4u?(( struct _enum_struct*) _temp2409)->tag == Cyc_Absyn_StructType_tag:
0){ _LL2423: _temp2422=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2409)->f1; goto _LL2421; _LL2421: _temp2420=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2409)->f2; goto _LL2418; _LL2418:
_temp2417=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2409)->f3; if( _temp2417 == 0){ goto _LL2413;} else{ _temp2419=* _temp2417;
goto _LL2412;}} else{ goto _LL2413;} _LL2413: if(( unsigned int) _temp2409 > 4u?((
struct _enum_struct*) _temp2409)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2430:
_temp2429=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2409)->f1;
goto _LL2428; _LL2428: _temp2427=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2409)->f2; goto _LL2425; _LL2425: _temp2424=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2409)->f3; if( _temp2424 == 0){ goto
_LL2415;} else{ _temp2426=* _temp2424; goto _LL2414;}} else{ goto _LL2415;}
_LL2415: goto _LL2416; _LL2412: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp2419, _temp2341); if( finfo != 0){ return({ struct _tuple6 _temp2431;
_temp2431.f1=( finfo->tq)->q_const; _temp2431.f2=( Cyc_Tcutil_addressof_props(
te, _temp2343)).f2; _temp2431;});} return bogus_ans;} _LL2414: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp2426, _temp2341); if( finfo != 0){
return({ struct _tuple6 _temp2432; _temp2432.f1=( finfo->tq)->q_const; _temp2432.f2=(
Cyc_Tcutil_addressof_props( te, _temp2343)).f2; _temp2432;});} goto _LL2416;}
_LL2416: return bogus_ans; _LL2410:;} _LL2330: { void* _temp2433= Cyc_Tcutil_compress((
void*)( _temp2347->topt)->v); struct Cyc_Absyn_PtrInfo _temp2439; struct Cyc_Absyn_Conref*
_temp2441; struct Cyc_Absyn_Tqual* _temp2443; struct Cyc_Absyn_Conref* _temp2445;
void* _temp2447; void* _temp2449; _LL2435: if(( unsigned int) _temp2433 > 4u?((
struct _enum_struct*) _temp2433)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2440:
_temp2439=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2433)->f1; _LL2450: _temp2449=( void*) _temp2439.elt_typ; goto _LL2448;
_LL2448: _temp2447=( void*) _temp2439.rgn_typ; goto _LL2446; _LL2446: _temp2445=(
struct Cyc_Absyn_Conref*) _temp2439.nullable; goto _LL2444; _LL2444: _temp2443=(
struct Cyc_Absyn_Tqual*) _temp2439.tq; goto _LL2442; _LL2442: _temp2441=( struct
Cyc_Absyn_Conref*) _temp2439.bounds; goto _LL2436;} else{ goto _LL2437;} _LL2437:
goto _LL2438; _LL2436: { void* _temp2451= Cyc_Tcutil_compress( _temp2449);
struct Cyc_Absyn_Structdecl** _temp2459; struct Cyc_Absyn_Structdecl* _temp2461;
struct Cyc_List_List* _temp2462; struct _tuple1* _temp2464; struct Cyc_Absyn_Uniondecl**
_temp2466; struct Cyc_Absyn_Uniondecl* _temp2468; struct Cyc_List_List*
_temp2469; struct _tuple1* _temp2471; _LL2453: if(( unsigned int) _temp2451 > 4u?((
struct _enum_struct*) _temp2451)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2465:
_temp2464=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2451)->f1;
goto _LL2463; _LL2463: _temp2462=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2451)->f2; goto _LL2460; _LL2460: _temp2459=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2451)->f3; if( _temp2459 == 0){ goto
_LL2455;} else{ _temp2461=* _temp2459; goto _LL2454;}} else{ goto _LL2455;}
_LL2455: if(( unsigned int) _temp2451 > 4u?(( struct _enum_struct*) _temp2451)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL2472: _temp2471=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp2451)->f1; goto _LL2470; _LL2470: _temp2469=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp2451)->f2;
goto _LL2467; _LL2467: _temp2466=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp2451)->f3; if( _temp2466 == 0){ goto _LL2457;} else{ _temp2468=* _temp2466;
goto _LL2456;}} else{ goto _LL2457;} _LL2457: goto _LL2458; _LL2454: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp2461,
_temp2345); if( finfo != 0){ return({ struct _tuple6 _temp2473; _temp2473.f1=(
finfo->tq)->q_const; _temp2473.f2= _temp2447; _temp2473;});} return bogus_ans;}
_LL2456: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp2468, _temp2345); if( finfo != 0){ return({ struct _tuple6 _temp2474;
_temp2474.f1=( finfo->tq)->q_const; _temp2474.f2= _temp2447; _temp2474;});}
return bogus_ans;} _LL2458: return bogus_ans; _LL2452:;} _LL2438: return
bogus_ans; _LL2434:;} _LL2332: { void* _temp2475= Cyc_Tcutil_compress(( void*)(
_temp2349->topt)->v); struct Cyc_Absyn_PtrInfo _temp2481; struct Cyc_Absyn_Conref*
_temp2483; struct Cyc_Absyn_Tqual* _temp2485; struct Cyc_Absyn_Conref* _temp2487;
void* _temp2489; void* _temp2491; _LL2477: if(( unsigned int) _temp2475 > 4u?((
struct _enum_struct*) _temp2475)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2482:
_temp2481=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2475)->f1; _LL2492: _temp2491=( void*) _temp2481.elt_typ; goto _LL2490;
_LL2490: _temp2489=( void*) _temp2481.rgn_typ; goto _LL2488; _LL2488: _temp2487=(
struct Cyc_Absyn_Conref*) _temp2481.nullable; goto _LL2486; _LL2486: _temp2485=(
struct Cyc_Absyn_Tqual*) _temp2481.tq; goto _LL2484; _LL2484: _temp2483=( struct
Cyc_Absyn_Conref*) _temp2481.bounds; goto _LL2478;} else{ goto _LL2479;} _LL2479:
goto _LL2480; _LL2478: return({ struct _tuple6 _temp2493; _temp2493.f1=
_temp2485->q_const; _temp2493.f2= _temp2489; _temp2493;}); _LL2480: return
bogus_ans; _LL2476:;} _LL2334: { void* t= Cyc_Tcutil_compress(( void*)(
_temp2353->topt)->v); void* _temp2494= t; struct Cyc_Absyn_Exp* _temp2504;
struct Cyc_Absyn_Tqual* _temp2506; void* _temp2508; struct Cyc_List_List*
_temp2510; struct Cyc_Absyn_PtrInfo _temp2512; struct Cyc_Absyn_Conref*
_temp2514; struct Cyc_Absyn_Tqual* _temp2516; struct Cyc_Absyn_Conref* _temp2518;
void* _temp2520; void* _temp2522; _LL2496: if(( unsigned int) _temp2494 > 4u?((
struct _enum_struct*) _temp2494)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2509:
_temp2508=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2494)->f1; goto
_LL2507; _LL2507: _temp2506=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2494)->f2; goto _LL2505; _LL2505: _temp2504=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2494)->f3; goto _LL2497;} else{ goto
_LL2498;} _LL2498: if(( unsigned int) _temp2494 > 4u?(( struct _enum_struct*)
_temp2494)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL2511: _temp2510=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2494)->f1; goto _LL2499;} else{ goto
_LL2500;} _LL2500: if(( unsigned int) _temp2494 > 4u?(( struct _enum_struct*)
_temp2494)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2513: _temp2512=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2494)->f1;
_LL2523: _temp2522=( void*) _temp2512.elt_typ; goto _LL2521; _LL2521: _temp2520=(
void*) _temp2512.rgn_typ; goto _LL2519; _LL2519: _temp2518=( struct Cyc_Absyn_Conref*)
_temp2512.nullable; goto _LL2517; _LL2517: _temp2516=( struct Cyc_Absyn_Tqual*)
_temp2512.tq; goto _LL2515; _LL2515: _temp2514=( struct Cyc_Absyn_Conref*)
_temp2512.bounds; goto _LL2501;} else{ goto _LL2502;} _LL2502: goto _LL2503;
_LL2497: return({ struct _tuple6 _temp2524; _temp2524.f1= _temp2506->q_const;
_temp2524.f2=( Cyc_Tcutil_addressof_props( te, _temp2353)).f2; _temp2524;});
_LL2499: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field( _temp2510,( int)
Cyc_Evexp_eval_const_uint_exp( _temp2351)); if( finfo != 0){ return({ struct
_tuple6 _temp2525; _temp2525.f1=((* finfo).f1)->q_const; _temp2525.f2=( Cyc_Tcutil_addressof_props(
te, _temp2353)).f2; _temp2525;});} return bogus_ans;} _LL2501: return({ struct
_tuple6 _temp2526; _temp2526.f1= _temp2516->q_const; _temp2526.f2= _temp2520;
_temp2526;}); _LL2503: return bogus_ans; _LL2495:;} _LL2336: Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ char* _temp2527=( char*)"unary & applied to non-lvalue";
struct _tagged_string _temp2528; _temp2528.curr= _temp2527; _temp2528.base=
_temp2527; _temp2528.last_plus_one= _temp2527 + 30; _temp2528;})); return
bogus_ans; _LL2324:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp2529= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp2535; struct Cyc_Absyn_Tqual* _temp2537; void*
_temp2539; _LL2531: if(( unsigned int) _temp2529 > 4u?(( struct _enum_struct*)
_temp2529)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2540: _temp2539=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2529)->f1; goto _LL2538; _LL2538:
_temp2537=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2529)->f2; goto _LL2536; _LL2536: _temp2535=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2529)->f3; goto _LL2532;} else{ goto
_LL2533;} _LL2533: goto _LL2534; _LL2532: { void* _temp2543; int _temp2545;
struct _tuple6 _temp2541= Cyc_Tcutil_addressof_props( te, e); _LL2546: _temp2545=
_temp2541.f1; goto _LL2544; _LL2544: _temp2543= _temp2541.f2; goto _LL2542;
_LL2542: return Cyc_Absyn_atb_typ( _temp2539, _temp2543, _temp2537,({ struct Cyc_Absyn_Upper_b_struct*
_temp2547=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp2547->tag= Cyc_Absyn_Upper_b_tag; _temp2547->f1= e;( void*) _temp2547;}));}
_LL2534: return e_typ; _LL2530:;} void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*
loc, unsigned int i, struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( b);{ void* _temp2548=(
void*) b->v; void* _temp2556; void* _temp2558; struct Cyc_Absyn_Exp* _temp2560;
_LL2550: if(( unsigned int) _temp2548 > 1u?(( struct _enum_struct*) _temp2548)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL2557: _temp2556=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2548)->f1; if( _temp2556 == Cyc_Absyn_Unknown_b){ goto _LL2551;} else{ goto
_LL2552;}} else{ goto _LL2552;} _LL2552: if(( unsigned int) _temp2548 > 1u?((
struct _enum_struct*) _temp2548)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2559:
_temp2558=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2548)->f1; if((
unsigned int) _temp2558 > 1u?(( struct _enum_struct*) _temp2558)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2561: _temp2560=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2558)->f1; goto _LL2553;} else{ goto _LL2554;}} else{ goto _LL2554;}
_LL2554: goto _LL2555; _LL2551: return; _LL2553: if( Cyc_Evexp_eval_const_uint_exp(
_temp2560) <= i){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2562=( char*)"dereference is out of bounds"; struct _tagged_string
_temp2563; _temp2563.curr= _temp2562; _temp2563.base= _temp2562; _temp2563.last_plus_one=
_temp2562 + 29; _temp2563;}));} return; _LL2555:( void*)( b->v=( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp2564=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2564->tag= Cyc_Absyn_Eq_constr_tag;
_temp2564->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp2565=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp2565->tag= Cyc_Absyn_Upper_b_tag;
_temp2565->f1= Cyc_Absyn_uint_exp( i +( unsigned int) 1, 0);( void*) _temp2565;});(
void*) _temp2564;})); return; _LL2549:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp2566= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Enumdecl** _temp2588; struct Cyc_List_List* _temp2590;
struct _tuple1* _temp2592; void* _temp2594; void* _temp2596; struct Cyc_Absyn_Exp*
_temp2598; struct Cyc_Absyn_Tqual* _temp2600; void* _temp2602; struct Cyc_List_List*
_temp2604; struct Cyc_Absyn_Structdecl** _temp2606; struct Cyc_List_List*
_temp2608; struct _tuple1* _temp2610; struct Cyc_Absyn_Uniondecl** _temp2612;
struct Cyc_List_List* _temp2614; struct _tuple1* _temp2616; _LL2568: if(
_temp2566 == Cyc_Absyn_VoidType){ goto _LL2569;} else{ goto _LL2570;} _LL2570:
if(( unsigned int) _temp2566 > 4u?(( struct _enum_struct*) _temp2566)->tag ==
Cyc_Absyn_EnumType_tag: 0){ _LL2593: _temp2592=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2566)->f1; goto _LL2591; _LL2591: _temp2590=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp2566)->f2; goto _LL2589; _LL2589:
_temp2588=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp2566)->f3; goto _LL2571;} else{ goto _LL2572;} _LL2572: if(( unsigned int)
_temp2566 > 4u?(( struct _enum_struct*) _temp2566)->tag == Cyc_Absyn_IntType_tag:
0){ _LL2597: _temp2596=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2566)->f1;
goto _LL2595; _LL2595: _temp2594=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2566)->f2; goto _LL2573;} else{ goto _LL2574;} _LL2574: if( _temp2566 ==
Cyc_Absyn_FloatType){ goto _LL2575;} else{ goto _LL2576;} _LL2576: if( _temp2566
== Cyc_Absyn_DoubleType){ goto _LL2577;} else{ goto _LL2578;} _LL2578: if((
unsigned int) _temp2566 > 4u?(( struct _enum_struct*) _temp2566)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2603: _temp2602=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2566)->f1;
goto _LL2601; _LL2601: _temp2600=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2566)->f2; goto _LL2599; _LL2599: _temp2598=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2566)->f3; goto _LL2579;} else{ goto
_LL2580;} _LL2580: if(( unsigned int) _temp2566 > 4u?(( struct _enum_struct*)
_temp2566)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL2605: _temp2604=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp2566)->f1; goto _LL2581;} else{ goto
_LL2582;} _LL2582: if(( unsigned int) _temp2566 > 4u?(( struct _enum_struct*)
_temp2566)->tag == Cyc_Absyn_StructType_tag: 0){ _LL2611: _temp2610=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2566)->f1; goto _LL2609;
_LL2609: _temp2608=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2566)->f2; goto _LL2607; _LL2607: _temp2606=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2566)->f3; goto _LL2583;} else{ goto
_LL2584;} _LL2584: if(( unsigned int) _temp2566 > 4u?(( struct _enum_struct*)
_temp2566)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL2617: _temp2616=( struct
_tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2566)->f1; goto _LL2615;
_LL2615: _temp2614=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2566)->f2; goto _LL2613; _LL2613: _temp2612=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2566)->f3; goto _LL2585;} else{ goto
_LL2586;} _LL2586: goto _LL2587; _LL2569: return 1; _LL2571: if( _temp2588 == 0){
return 0;}{ struct Cyc_Absyn_Enumdecl* ed=* _temp2588; if( ed->fields == 0){
return 0;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ed->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*)
fs->hd; if( f->tvs != 0? 1: f->typs != 0){ return 0;}}} return 1;} _LL2573:
return 1; _LL2575: return 1; _LL2577: return 1; _LL2579: return Cyc_Tcutil_bits_only(
_temp2602); _LL2581: for( 0; _temp2604 != 0; _temp2604= _temp2604->tl){ if( !
Cyc_Tcutil_bits_only((*(( struct _tuple4*) _temp2604->hd)).f2)){ return 0;}}
return 1; _LL2583: if( _temp2606 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl*
sd=* _temp2606; if( sd->fields == 0){ return 0;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp2608);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 0;}}}
return 1;}} _LL2585: if( _temp2612 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=* _temp2612; if( ud->fields == 0){ return 0;}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
ud->tvs, _temp2614);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v;
for( 0; fs != 0; fs= fs->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ return 0;}}}
return 1;}} _LL2587: return 0; _LL2567:;}