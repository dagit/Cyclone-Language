#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
int f1; void* f2; } ; struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{
struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u]; struct
Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u];
struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name; typedef
struct _tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
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
; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_star_typ( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_at_typ( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; } ;
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void*
Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv*); extern void
Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern void Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* eff); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern void
Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_integral_promote( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_arithmetic_promote( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_comparison_promote( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*);
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*
bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_nonzero_bound( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern struct _tuple5 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
extern const int Cyc_CfFlowInfo_VarRoot_tag; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern const int Cyc_CfFlowInfo_MallocPt_tag;
struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_CfFlowInfo_StructF_tag; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; extern const int Cyc_CfFlowInfo_TupleF_tag;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; extern void* Cyc_CfFlowInfo_Esc;
extern void* Cyc_CfFlowInfo_Unesc; extern void* Cyc_CfFlowInfo_NoneIL; extern
void* Cyc_CfFlowInfo_ThisIL; extern void* Cyc_CfFlowInfo_AllIL; extern const int
Cyc_CfFlowInfo_UnknownIS_tag; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag;
void* f1; void* f2; } ; extern const int Cyc_CfFlowInfo_MustPointTo_tag; struct
Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1; } ;
extern const int Cyc_CfFlowInfo_LeafPI_tag; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; extern const int Cyc_CfFlowInfo_TuplePI_tag; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern const int Cyc_CfFlowInfo_StructPI_tag;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_BottomFL; extern const int Cyc_CfFlowInfo_InitsFL_tag;
struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp();}
static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp0; _push_handler(& _temp0);{
struct _xenum_struct* _temp1=( struct _xenum_struct*) setjmp( _temp0.handler);
if( ! _temp1){{ void* _temp2= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp12; struct Cyc_Absyn_Enumfield* _temp14; struct Cyc_Absyn_Enumdecl* _temp16;
struct Cyc_Absyn_Enumfield* _temp18; struct Cyc_Absyn_Xenumdecl* _temp20; struct
Cyc_Absyn_Structdecl* _temp22; _LL4: if((( struct _enum_struct*) _temp2)->tag ==
Cyc_Tcenv_VarRes_tag){ _LL13: _temp12=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: if((( struct _enum_struct*)
_temp2)->tag == Cyc_Tcenv_EnumRes_tag){ _LL17: _temp16=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f1; goto _LL15; _LL15: _temp14=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f2;
goto _LL7;} else{ goto _LL8;} _LL8: if((( struct _enum_struct*) _temp2)->tag ==
Cyc_Tcenv_XenumRes_tag){ _LL21: _temp20=( struct Cyc_Absyn_Xenumdecl*)(( struct
Cyc_Tcenv_XenumRes_struct*) _temp2)->f1; goto _LL19; _LL19: _temp18=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp2)->f2; goto _LL9;} else{ goto _LL10;}
_LL10: if((( struct _enum_struct*) _temp2)->tag == Cyc_Tcenv_StructRes_tag){
_LL23: _temp22=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp2)->f1; goto _LL11;} else{ goto _LL3;} _LL5:( void*)( e->r=( void*)({
struct Cyc_Absyn_Var_e_struct* _temp24=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp24->tag= Cyc_Absyn_Var_e_tag;
_temp24->f1= q; _temp24->f2=( void*) _temp12;( void*) _temp24;})); goto _LL3;
_LL7:( void*)( e->r=( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp25=( struct
Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp25->tag= Cyc_Absyn_Enum_e_tag; _temp25->f1= 0; _temp25->f2= 0; _temp25->f3=
0; _temp25->f4= _temp16; _temp25->f5= _temp14;( void*) _temp25;})); goto _LL3;
_LL9:( void*)( e->r=( void*)({ struct Cyc_Absyn_Xenum_e_struct* _temp26=( struct
Cyc_Absyn_Xenum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_e_struct));
_temp26->tag= Cyc_Absyn_Xenum_e_tag; _temp26->f1= 0; _temp26->f2= 0; _temp26->f3=
_temp20; _temp26->f4= _temp18;( void*) _temp26;})); goto _LL3; _LL11: Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp27= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp27.last_plus_one - _temp27.curr, _temp27.curr);}));( void*)( e->r=( void*)({
struct Cyc_Absyn_Var_e_struct* _temp28=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp28->tag= Cyc_Absyn_Var_e_tag;
_temp28->f1= q; _temp28->f2=( void*) Cyc_Absyn_Unresolved_b;( void*) _temp28;}));
goto _LL3; _LL3:;}; _pop_handler();} else{ struct _xenum_struct* _temp30= _temp1;
_LL32: if( _temp30->tag == Cyc_Dict_Absent_tag){ goto _LL33;} else{ goto _LL34;}
_LL34: goto _LL35; _LL33:( void*)( e->r=( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp36=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp36->tag= Cyc_Absyn_Var_e_tag; _temp36->f1= q; _temp36->f2=( void*) Cyc_Absyn_Unresolved_b;(
void*) _temp36;})); goto _LL31; _LL35:( void) _throw( _temp30); _LL31:;}}}
static struct _tuple6* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){
return({ struct _tuple6* _temp37=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp37->f1= 0; _temp37->f2= e; _temp37;});} static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* es){ void* _temp38=( void*) e1->r; struct _tuple1* _temp44;
_LL40: if((( struct _enum_struct*) _temp38)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL45: _temp44=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp38)->f1; goto _LL41;} else{ goto _LL42;} _LL42: goto _LL43; _LL41: { struct
_handler_cons _temp46; _push_handler(& _temp46);{ struct _xenum_struct* _temp47=(
struct _xenum_struct*) setjmp( _temp46.handler); if( ! _temp47){{ void* _temp48=
Cyc_Tcenv_lookup_ordinary( te, e1->loc, _temp44); void* _temp58; struct Cyc_Absyn_Enumfield*
_temp60; struct Cyc_Absyn_Enumdecl* _temp62; struct Cyc_Absyn_Enumfield* _temp64;
struct Cyc_Absyn_Xenumdecl* _temp66; struct Cyc_Absyn_Structdecl* _temp68; _LL50:
if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_VarRes_tag){ _LL59:
_temp58=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp48)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_EnumRes_tag){
_LL63: _temp62=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp48)->f1; goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp48)->f2; goto _LL53;} else{ goto _LL54;} _LL54:
if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_XenumRes_tag){ _LL67:
_temp66=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp48)->f1; goto _LL65; _LL65: _temp64=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_XenumRes_struct*) _temp48)->f2; goto _LL55;} else{ goto _LL56;} _LL56:
if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_StructRes_tag){ _LL69:
_temp68=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp48)->f1; goto _LL57;} else{ goto _LL49;} _LL51:( void*)( e->r=( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp70=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp70->tag= Cyc_Absyn_FnCall_e_tag;
_temp70->f1= e1; _temp70->f2= es;( void*) _temp70;})); _npop_handler( 0u);
return; _LL53: if( _temp60->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp71= Cyc_Absynpp_qvar2string( _temp60->name); xprintf("%.*s is a constant, not a function",
_temp71.last_plus_one - _temp71.curr, _temp71.curr);}));}( void*)( e->r=( void*)({
struct Cyc_Absyn_Enum_e_struct* _temp72=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp72->tag= Cyc_Absyn_Enum_e_tag;
_temp72->f1= 0; _temp72->f2= 0; _temp72->f3= es; _temp72->f4= _temp62; _temp72->f5=
_temp60;( void*) _temp72;})); _npop_handler( 0u); return; _LL55: if( _temp64->typs
== 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp73= Cyc_Absynpp_qvar2string(
_temp64->name); xprintf("%.*s is a constant, not a function", _temp73.last_plus_one
- _temp73.curr, _temp73.curr);}));}( void*)( e->r=( void*)({ struct Cyc_Absyn_Xenum_e_struct*
_temp74=( struct Cyc_Absyn_Xenum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_e_struct));
_temp74->tag= Cyc_Absyn_Xenum_e_tag; _temp74->f1= 0; _temp74->f2= es; _temp74->f3=
_temp66; _temp74->f4= _temp64;( void*) _temp74;})); _npop_handler( 0u); return;
_LL57: { struct Cyc_List_List* des=(( struct Cyc_List_List*(*)( struct _tuple6*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg,
es); if( _temp68->name != 0){( void*)( e->r=( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp75=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp75->tag= Cyc_Absyn_Struct_e_tag; _temp75->f1=( struct _tuple1*)( _temp68->name)->v;
_temp75->f2= 0; _temp75->f3= des; _temp75->f4=( struct Cyc_Absyn_Structdecl*)
_temp68;( void*) _temp75;}));} else{ Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ char* _temp76=( char*)"missing struct name"; struct
_tagged_string _temp77; _temp77.curr= _temp76; _temp77.base= _temp76; _temp77.last_plus_one=
_temp76 + 20; _temp77;}));( void*)( e->r=( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp78=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp78->tag= Cyc_Absyn_Struct_e_tag; _temp78->f1= _temp44; _temp78->f2= 0;
_temp78->f3= des; _temp78->f4=( struct Cyc_Absyn_Structdecl*) _temp68;( void*)
_temp78;}));} _npop_handler( 0u); return;} _LL49:;}; _pop_handler();} else{
struct _xenum_struct* _temp80= _temp47; _LL82: if( _temp80->tag == Cyc_Dict_Absent_tag){
goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: Cyc_Tcutil_terr( e1->loc,({
struct _tagged_string _temp86= Cyc_Absynpp_qvar2string( _temp44); xprintf("unbound identifier %.*s",
_temp86.last_plus_one - _temp86.curr, _temp86.curr);}));( void*)( e->r=( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp87=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp87->tag= Cyc_Absyn_FnCall_e_tag;
_temp87->f1= e1; _temp87->f2= es;( void*) _temp87;})); return; _LL85:( void)
_throw( _temp80); _LL81:;}}} _LL43:( void*)( e->r=( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp88=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp88->tag= Cyc_Absyn_FnCall_e_tag; _temp88->f1= e1; _temp88->f2= es;( void*)
_temp88;})); return; _LL39:;} static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=(
void*)({ struct Cyc_Absyn_Array_e_struct* _temp89=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp89->tag= Cyc_Absyn_Array_e_tag;
_temp89->f1= des;( void*) _temp89;})); return;}{ void* t=* topt; void* _temp90=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp98; struct Cyc_Absyn_Tqual*
_temp100; void* _temp102; struct Cyc_Absyn_Structdecl** _temp104; struct Cyc_List_List*
_temp106; struct _tuple1* _temp108; _LL92: if(( unsigned int) _temp90 > 4u?((
struct _enum_struct*) _temp90)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL103:
_temp102=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp90)->f1; goto
_LL101; _LL101: _temp100=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp90)->f2; goto _LL99; _LL99: _temp98=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp90)->f3; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp90
> 4u?(( struct _enum_struct*) _temp90)->tag == Cyc_Absyn_StructType_tag: 0){
_LL109: _temp108=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp90)->f1; goto _LL107; _LL107: _temp106=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp90)->f2; goto _LL105; _LL105: _temp104=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp90)->f3;
goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL93:( void*)( e->r=( void*)({
struct Cyc_Absyn_Array_e_struct* _temp110=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp110->tag= Cyc_Absyn_Array_e_tag;
_temp110->f1= des;( void*) _temp110;})); goto _LL91; _LL95: if( _temp104 == 0? 1:
_temp108 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp111=(
char*)"Compiler bug: struct type not properly set"; struct _tagged_string
_temp112; _temp112.curr= _temp111; _temp112.base= _temp111; _temp112.last_plus_one=
_temp111 + 43; _temp112;}));}( void*)( e->r=( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp113=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp113->tag= Cyc_Absyn_Struct_e_tag; _temp113->f1=( struct _tuple1*)({ struct
_tuple1* _temp114= _temp108; if( _temp114 == 0){ _throw( Null_Exception);}
_temp114;}); _temp113->f2= 0; _temp113->f3= des; _temp113->f4=( struct Cyc_Absyn_Structdecl*)*
_temp104;( void*) _temp113;})); goto _LL91; _LL97:( void*)( e->r=( void*)({
struct Cyc_Absyn_Array_e_struct* _temp115=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp115->tag= Cyc_Absyn_Array_e_tag;
_temp115->f1= des;( void*) _temp115;})); goto _LL91; _LL91:;}} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void*
Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
es){ for( 0; es != 0; es= es->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
es->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp(
te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){
Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp116= msg_part; struct
_tagged_string _temp117= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp116.last_plus_one - _temp116.curr, _temp116.curr, _temp117.last_plus_one -
_temp117.curr, _temp117.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp118= c; char _temp134; void* _temp136; short _temp138;
void* _temp140; long long _temp142; void* _temp144; struct _tagged_string
_temp146; int _temp148; void* _temp150; struct _tagged_string _temp152; _LL120:
if(( unsigned int) _temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Char_c_tag:
0){ _LL137: _temp136=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp118)->f1;
goto _LL135; _LL135: _temp134=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp118)->f2; goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp118)->f1;
goto _LL139; _LL139: _temp138=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp118)->f2; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp118)->f1;
goto _LL143; _LL143: _temp142=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp118)->f2; goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL147: _temp146=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp118)->f1; goto _LL127;} else{ goto _LL128;} _LL128: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL151: _temp150=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp118)->f1; goto
_LL149; _LL149: _temp148=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp118)->f2;
goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int) _temp118 > 1u?((
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_String_c_tag: 0){ _LL153:
_temp152=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp118)->f1;
goto _LL131;} else{ goto _LL132;} _LL132: if( _temp118 == Cyc_Absyn_Null_c){
goto _LL133;} else{ goto _LL119;} _LL121: t= _temp136 == Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL119; _LL123: t= _temp140 == Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t; goto _LL119; _LL125: t= _temp144 == Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL119; _LL127: t= Cyc_Absyn_float_t; goto _LL119;
_LL129: if( topt == 0){ t= _temp150 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp154=* topt; void* _temp164; void* _temp166; void* _temp168;
void* _temp170; void* _temp172; void* _temp174; _LL156: if(( unsigned int)
_temp154 > 4u?(( struct _enum_struct*) _temp154)->tag == Cyc_Absyn_IntType_tag:
0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp154)->f1;
goto _LL165; _LL165: _temp164=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp154)->f2; if( _temp164 == Cyc_Absyn_B1){ goto _LL157;} else{ goto _LL158;}}
else{ goto _LL158;} _LL158: if(( unsigned int) _temp154 > 4u?(( struct
_enum_struct*) _temp154)->tag == Cyc_Absyn_IntType_tag: 0){ _LL171: _temp170=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp154)->f1; goto _LL169; _LL169:
_temp168=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp154)->f2; if(
_temp168 == Cyc_Absyn_B2){ goto _LL159;} else{ goto _LL160;}} else{ goto _LL160;}
_LL160: if(( unsigned int) _temp154 > 4u?(( struct _enum_struct*) _temp154)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL175: _temp174=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp154)->f1; goto _LL173; _LL173: _temp172=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp154)->f2; goto _LL161;} else{ goto _LL162;} _LL162: goto _LL163; _LL157: t=
_temp166 == Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;( void*)( e->r=(
void*)({ struct Cyc_Absyn_Const_e_struct* _temp176=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp176->tag= Cyc_Absyn_Const_e_tag;
_temp176->f1=( void*)({ struct Cyc_Absyn_Char_c_struct* _temp177=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp177->tag= Cyc_Absyn_Char_c_tag;
_temp177->f1=( void*) _temp166; _temp177->f2=( char) _temp148;( void*) _temp177;});(
void*) _temp176;})); goto _LL155; _LL159: t= _temp170 == Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t;( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp178=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp178->tag= Cyc_Absyn_Const_e_tag; _temp178->f1=( void*)({ struct Cyc_Absyn_Short_c_struct*
_temp179=( struct Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp179->tag= Cyc_Absyn_Short_c_tag; _temp179->f1=( void*) _temp170; _temp179->f2=(
short) _temp148;( void*) _temp179;});( void*) _temp178;})); goto _LL155; _LL161:
t= _temp174 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto
_LL155; _LL163: t= _temp150 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL155; _LL155:;} goto _LL119; _LL131: { int len=( int)({ struct
_tagged_string _temp180= _temp152;( unsigned int)( _temp180.last_plus_one -
_temp180.curr);}); void* r=({ struct Cyc_Absyn_Const_e_struct* _temp181=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp181->tag= Cyc_Absyn_Const_e_tag; _temp181->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp182=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp182->tag= Cyc_Absyn_Int_c_tag; _temp182->f1=( void*) Cyc_Absyn_Unsigned;
_temp182->f2= len;( void*) _temp182;});( void*) _temp181;}); struct Cyc_Absyn_Exp*
elen= Cyc_Absyn_new_exp( r, loc); elen->topt=({ struct Cyc_Core_Opt* _temp183=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp183->v=(
void*) Cyc_Absyn_uint_t; _temp183;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_Upper_b_struct*
_temp184=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp184->tag= Cyc_Absyn_Upper_b_tag; _temp184->f1= elen;( void*) _temp184;}));
if( topt != 0){ void* _temp185= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp*
_temp193; struct Cyc_Absyn_Tqual* _temp195; void* _temp197; struct Cyc_Absyn_PtrInfo
_temp199; struct Cyc_Absyn_Conref* _temp201; struct Cyc_Absyn_Tqual* _temp203;
struct Cyc_Absyn_Conref* _temp205; void* _temp207; void* _temp209; _LL187: if((
unsigned int) _temp185 > 4u?(( struct _enum_struct*) _temp185)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp185)->f1;
goto _LL196; _LL196: _temp195=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp185)->f2; goto _LL194; _LL194: _temp193=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp185)->f3; goto _LL188;} else{ goto _LL189;}
_LL189: if(( unsigned int) _temp185 > 4u?(( struct _enum_struct*) _temp185)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL200: _temp199=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp185)->f1; _LL210: _temp209=( void*)
_temp199.elt_typ; goto _LL208; _LL208: _temp207=( void*) _temp199.rgn_typ; goto
_LL206; _LL206: _temp205=( struct Cyc_Absyn_Conref*) _temp199.nullable; goto
_LL204; _LL204: _temp203=( struct Cyc_Absyn_Tqual*) _temp199.tq; goto _LL202;
_LL202: _temp201=( struct Cyc_Absyn_Conref*) _temp199.bounds; goto _LL190;}
else{ goto _LL191;} _LL191: goto _LL192; _LL188: return({ struct Cyc_Absyn_ArrayType_struct*
_temp211=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp211->tag= Cyc_Absyn_ArrayType_tag; _temp211->f1=( void*) Cyc_Absyn_uchar_t;
_temp211->f2= _temp195; _temp211->f3=( struct Cyc_Absyn_Exp*) elen;( void*)
_temp211;}); _LL190: if( ! Cyc_Tcutil_unify(* topt, t)? Cyc_Tcutil_silent_castable(
te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp212=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp212->v=( void*) t; _temp212;});
Cyc_Tcutil_unchecked_cast( te, e,* topt); t=* topt;} goto _LL186; _LL192: goto
_LL186; _LL186:;} return t;} _LL133: t=({ struct Cyc_Absyn_PointerType_struct*
_temp213=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp213->tag= Cyc_Absyn_PointerType_tag; _temp213->f1=({ struct Cyc_Absyn_PtrInfo
_temp214; _temp214.elt_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
_temp214.rgn_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind); _temp214.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 1); _temp214.tq= Cyc_Absyn_empty_tqual();
_temp214.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp214;});( void*) _temp213;}); goto _LL119; _LL119:;} return t;} static void*
Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tuple1* q, void* b){ void* _temp215= b; struct Cyc_Absyn_Vardecl*
_temp229; struct Cyc_Absyn_Fndecl* _temp231; struct Cyc_Absyn_Vardecl* _temp233;
struct Cyc_Absyn_Vardecl* _temp235; struct Cyc_Absyn_Vardecl* _temp237; _LL217:
if( _temp215 == Cyc_Absyn_Unresolved_b){ goto _LL218;} else{ goto _LL219;}
_LL219: if(( unsigned int) _temp215 > 1u?(( struct _enum_struct*) _temp215)->tag
== Cyc_Absyn_Global_b_tag: 0){ _LL230: _temp229=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp215)->f1; goto _LL220;} else{ goto
_LL221;} _LL221: if(( unsigned int) _temp215 > 1u?(( struct _enum_struct*)
_temp215)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL232: _temp231=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp215)->f1; goto _LL222;} else{ goto
_LL223;} _LL223: if(( unsigned int) _temp215 > 1u?(( struct _enum_struct*)
_temp215)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL234: _temp233=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp215)->f1; goto _LL224;} else{ goto _LL225;}
_LL225: if(( unsigned int) _temp215 > 1u?(( struct _enum_struct*) _temp215)->tag
== Cyc_Absyn_Local_b_tag: 0){ _LL236: _temp235=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp215)->f1; goto _LL226;} else{ goto _LL227;}
_LL227: if(( unsigned int) _temp215 > 1u?(( struct _enum_struct*) _temp215)->tag
== Cyc_Absyn_Param_b_tag: 0){ _LL238: _temp237=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp215)->f1; goto _LL228;} else{ goto _LL216;}
_LL218: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp239=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s", _temp239.last_plus_one
- _temp239.curr, _temp239.curr);})); _LL220:({ struct _tuple1* _temp240= q;
unsigned int _temp241= 0; if( _temp241 >= 1u){ _throw( Null_Exception);}
_temp240[ _temp241]=({ struct _tuple1* _temp242= _temp229->name; unsigned int
_temp243= 0; if( _temp243 >= 1u){ _throw( Null_Exception);} _temp242[ _temp243];});});
return( void*) _temp229->type; _LL222:({ struct _tuple1* _temp244= q;
unsigned int _temp245= 0; if( _temp245 >= 1u){ _throw( Null_Exception);}
_temp244[ _temp245]=({ struct _tuple1* _temp246= _temp231->name; unsigned int
_temp247= 0; if( _temp247 >= 1u){ _throw( Null_Exception);} _temp246[ _temp247];});});
return Cyc_Tcutil_fndecl2typ( _temp231); _LL224: _temp235= _temp233; goto _LL226;
_LL226: _temp237= _temp235; goto _LL228; _LL228:(* q).f1= Cyc_Absyn_Loc_n;
return( void*) _temp237->type; _LL216:;} static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* fmt, struct Cyc_List_List* args,
struct Cyc_List_List*(* type_getter)( struct _tagged_string, struct Cyc_Position_Segment*)){
struct Cyc_List_List* desc_types;{ void* _temp248=( void*) fmt->r; void*
_temp254; struct _tagged_string _temp256; _LL250: if((( struct _enum_struct*)
_temp248)->tag == Cyc_Absyn_Const_e_tag){ _LL255: _temp254=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp248)->f1; if(( unsigned int) _temp254 > 1u?(( struct _enum_struct*)
_temp254)->tag == Cyc_Absyn_String_c_tag: 0){ _LL257: _temp256=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp254)->f1; goto _LL251;}
else{ goto _LL252;}} else{ goto _LL252;} _LL252: goto _LL253; _LL251: desc_types=
type_getter( _temp256, fmt->loc); goto _LL249; _LL253: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp258=( char*)"expecting a literal format string";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 34; _temp259;})); return; _LL249:;} for( 0;
desc_types != 0? args != 0: 0; desc_types= desc_types->tl, args= args->tl){ void*
t=( void*) desc_types->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) args->hd;
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp260= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp261= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_temp260.last_plus_one - _temp260.curr, _temp260.curr, _temp261.last_plus_one -
_temp261.curr, _temp261.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp262=( char*)"too few arguments"; struct
_tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 18; _temp263;}));} if( args != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) args->hd)->loc,( struct _tagged_string)({ char* _temp264=(
char*)"too many arguments"; struct _tagged_string _temp265; _temp265.curr=
_temp264; _temp265.base= _temp264; _temp265.last_plus_one= _temp264 + 19;
_temp265;}));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp266= p; _LL268:
if( _temp266 == Cyc_Absyn_Plus){ goto _LL269;} else{ goto _LL270;} _LL270: if(
_temp266 == Cyc_Absyn_Minus){ goto _LL271;} else{ goto _LL272;} _LL272: if(
_temp266 == Cyc_Absyn_Not){ goto _LL273;} else{ goto _LL274;} _LL274: if(
_temp266 == Cyc_Absyn_Bitnot){ goto _LL275;} else{ goto _LL276;} _LL276: if(
_temp266 == Cyc_Absyn_Size){ goto _LL277;} else{ goto _LL278;} _LL278: goto
_LL279; _LL269: goto _LL271; _LL271: if( ! Cyc_Tcutil_arithmetic_promote( te, e)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp280= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp280.last_plus_one -
_temp280.curr, _temp280.curr);}));} return( void*)( e->topt)->v; _LL273: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp281= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp281.last_plus_one - _temp281.curr, _temp281.curr);}));} return Cyc_Absyn_sint_t;
_LL275: if( ! Cyc_Tcutil_integral_promote( te, e)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp282= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp282.last_plus_one - _temp282.curr, _temp282.curr);}));} return( void*)( e->topt)->v;
_LL277:{ void* _temp283= t; struct Cyc_Absyn_Exp* _temp291; struct Cyc_Absyn_Tqual*
_temp293; void* _temp295; struct Cyc_Absyn_PtrInfo _temp297; struct Cyc_Absyn_Conref*
_temp299; struct Cyc_Absyn_Tqual* _temp301; struct Cyc_Absyn_Conref* _temp303;
void* _temp305; void* _temp307; _LL285: if(( unsigned int) _temp283 > 4u?((
struct _enum_struct*) _temp283)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL296:
_temp295=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp283)->f1; goto
_LL294; _LL294: _temp293=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp283)->f2; goto _LL292; _LL292: _temp291=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp283)->f3; goto _LL286;} else{ goto _LL287;}
_LL287: if(( unsigned int) _temp283 > 4u?(( struct _enum_struct*) _temp283)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL298: _temp297=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp283)->f1; _LL308: _temp307=( void*)
_temp297.elt_typ; goto _LL306; _LL306: _temp305=( void*) _temp297.rgn_typ; goto
_LL304; _LL304: _temp303=( struct Cyc_Absyn_Conref*) _temp297.nullable; goto
_LL302; _LL302: _temp301=( struct Cyc_Absyn_Tqual*) _temp297.tq; goto _LL300;
_LL300: _temp299=( struct Cyc_Absyn_Conref*) _temp297.bounds; goto _LL288;}
else{ goto _LL289;} _LL289: goto _LL290; _LL286: goto _LL284; _LL288: goto
_LL284; _LL290: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp309= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp309.last_plus_one
- _temp309.curr, _temp309.curr);})); goto _LL284; _LL284:;} return Cyc_Absyn_uint_t;
_LL279: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp310=( char*)"Non-unary primop"; struct
_tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 17; _temp311;})); _LL267:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* promoter)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*)){ if( ! promoter(
te, e1)){ Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp312= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); xprintf("type %.*s cannot be used here", _temp312.last_plus_one
- _temp312.curr, _temp312.curr);})); return Cyc_Absyn_wildtyp();} if( ! promoter(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp313= Cyc_Absynpp_typ2string((
void*)( e2->topt)->v); xprintf("type %.*s cannot be used here", _temp313.last_plus_one
- _temp313.curr, _temp313.curr);})); return Cyc_Absyn_wildtyp();}{ void* t1= Cyc_Tcutil_compress((
void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress(( void*)( e2->topt)->v);
void* res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast(
te, e1, res); Cyc_Tcutil_unchecked_cast( te, e2, res); return res;}} static void*
Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp314= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("expecting int but found %.*s", _temp314.last_plus_one - _temp314.curr,
_temp314.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v;
void* t2=( void*)( e2->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if(
Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp315= Cyc_Absynpp_typ2string(( void*)( e1->topt)->v);
struct _tagged_string _temp316= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp315.last_plus_one - _temp315.curr, _temp315.curr, _temp316.last_plus_one -
_temp316.curr, _temp316.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp317= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp318= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp317.last_plus_one - _temp317.curr, _temp317.curr, _temp318.last_plus_one -
_temp318.curr, _temp318.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ int e1_is_unboxed_arith= Cyc_Tcutil_comparison_promote(
te, e1); int e2_is_unboxed_arith= Cyc_Tcutil_comparison_promote( te, e2); void*
t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( e1_is_unboxed_arith? e2_is_unboxed_arith: 0){ void*
res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast( te, e1,
res); Cyc_Tcutil_unchecked_cast( te, e2, res); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar( Cyc_Absyn_BoxKind)):
0){ return Cyc_Absyn_sint_t;} else{ void* t1=( void*)( e1->topt)->v; void* t2=(
void*)( e2->topt)->v;{ struct _tuple0 _temp320=({ struct _tuple0 _temp319;
_temp319.f1= Cyc_Tcutil_compress( t1); _temp319.f2= Cyc_Tcutil_compress( t2);
_temp319;}); void* _temp326; struct Cyc_Absyn_PtrInfo _temp328; struct Cyc_Absyn_Conref*
_temp330; struct Cyc_Absyn_Tqual* _temp332; struct Cyc_Absyn_Conref* _temp334;
void* _temp336; void* _temp338; void* _temp340; struct Cyc_Absyn_PtrInfo
_temp342; struct Cyc_Absyn_Conref* _temp344; struct Cyc_Absyn_Tqual* _temp346;
struct Cyc_Absyn_Conref* _temp348; void* _temp350; void* _temp352; _LL322:
_LL341: _temp340= _temp320.f1; if(( unsigned int) _temp340 > 4u?(( struct
_enum_struct*) _temp340)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL343: _temp342=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp340)->f1;
_LL353: _temp352=( void*) _temp342.elt_typ; goto _LL351; _LL351: _temp350=( void*)
_temp342.rgn_typ; goto _LL349; _LL349: _temp348=( struct Cyc_Absyn_Conref*)
_temp342.nullable; goto _LL347; _LL347: _temp346=( struct Cyc_Absyn_Tqual*)
_temp342.tq; goto _LL345; _LL345: _temp344=( struct Cyc_Absyn_Conref*) _temp342.bounds;
goto _LL327;} else{ goto _LL324;} _LL327: _temp326= _temp320.f2; if((
unsigned int) _temp326 > 4u?(( struct _enum_struct*) _temp326)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL329: _temp328=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp326)->f1; _LL339: _temp338=( void*) _temp328.elt_typ; goto _LL337; _LL337:
_temp336=( void*) _temp328.rgn_typ; goto _LL335; _LL335: _temp334=( struct Cyc_Absyn_Conref*)
_temp328.nullable; goto _LL333; _LL333: _temp332=( struct Cyc_Absyn_Tqual*)
_temp328.tq; goto _LL331; _LL331: _temp330=( struct Cyc_Absyn_Conref*) _temp328.bounds;
goto _LL323;} else{ goto _LL324;} _LL324: goto _LL325; _LL323: if( Cyc_Tcutil_unify(
_temp352, _temp338)){ return Cyc_Absyn_sint_t;} goto _LL321; _LL325: goto _LL321;
_LL321:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp354= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp355= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp354.last_plus_one - _temp354.curr, _temp354.curr, _temp355.last_plus_one -
_temp355.curr, _temp355.curr);})); return Cyc_Absyn_wildtyp();}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp356= p; _LL358: if( _temp356 == Cyc_Absyn_Plus){ goto _LL359;} else{
goto _LL360;} _LL360: if( _temp356 == Cyc_Absyn_Minus){ goto _LL361;} else{ goto
_LL362;} _LL362: if( _temp356 == Cyc_Absyn_Times){ goto _LL363;} else{ goto
_LL364;} _LL364: if( _temp356 == Cyc_Absyn_Div){ goto _LL365;} else{ goto _LL366;}
_LL366: if( _temp356 == Cyc_Absyn_Mod){ goto _LL367;} else{ goto _LL368;} _LL368:
if( _temp356 == Cyc_Absyn_Bitand){ goto _LL369;} else{ goto _LL370;} _LL370: if(
_temp356 == Cyc_Absyn_Bitor){ goto _LL371;} else{ goto _LL372;} _LL372: if(
_temp356 == Cyc_Absyn_Bitxor){ goto _LL373;} else{ goto _LL374;} _LL374: if(
_temp356 == Cyc_Absyn_Bitlshift){ goto _LL375;} else{ goto _LL376;} _LL376: if(
_temp356 == Cyc_Absyn_Bitlrshift){ goto _LL377;} else{ goto _LL378;} _LL378: if(
_temp356 == Cyc_Absyn_Bitarshift){ goto _LL379;} else{ goto _LL380;} _LL380: if(
_temp356 == Cyc_Absyn_Eq){ goto _LL381;} else{ goto _LL382;} _LL382: if(
_temp356 == Cyc_Absyn_Neq){ goto _LL383;} else{ goto _LL384;} _LL384: if(
_temp356 == Cyc_Absyn_Gt){ goto _LL385;} else{ goto _LL386;} _LL386: if(
_temp356 == Cyc_Absyn_Lt){ goto _LL387;} else{ goto _LL388;} _LL388: if(
_temp356 == Cyc_Absyn_Gte){ goto _LL389;} else{ goto _LL390;} _LL390: if(
_temp356 == Cyc_Absyn_Lte){ goto _LL391;} else{ goto _LL392;} _LL392: goto
_LL393; _LL359: return Cyc_Tcexp_tcPlus( te, e1, e2); _LL361: return Cyc_Tcexp_tcMinus(
te, e1, e2); _LL363: goto _LL365; _LL365: return Cyc_Tcexp_tcArithBinop( te, e1,
e2, Cyc_Tcutil_arithmetic_promote); _LL367: goto _LL369; _LL369: goto _LL371;
_LL371: goto _LL373; _LL373: goto _LL375; _LL375: goto _LL377; _LL377: goto
_LL379; _LL379: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_integral_promote);
_LL381: goto _LL383; _LL383: goto _LL385; _LL385: goto _LL387; _LL387: goto
_LL389; _LL389: goto _LL391; _LL391: return Cyc_Tcexp_tcAnyBinop( te, loc, e1,
e2); _LL393:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp394=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp394->tag= Cyc_Core_Failure_tag; _temp394->f1=( struct _tagged_string)({
char* _temp395=( char*)"bad binary primop"; struct _tagged_string _temp396;
_temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 18; _temp396;});( struct _xenum_struct*) _temp394;})); _LL357:;}
static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_List_List* es){ if( p == Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*) es->hd);} Cyc_Tcexp_tcExpList( te, es);{ void*
t;{ void* _temp397= p; _LL399: if( _temp397 == Cyc_Absyn_Printf){ goto _LL400;}
else{ goto _LL401;} _LL401: if( _temp397 == Cyc_Absyn_Fprintf){ goto _LL402;}
else{ goto _LL403;} _LL403: if( _temp397 == Cyc_Absyn_Xprintf){ goto _LL404;}
else{ goto _LL405;} _LL405: if( _temp397 == Cyc_Absyn_Scanf){ goto _LL406;}
else{ goto _LL407;} _LL407: if( _temp397 == Cyc_Absyn_Fscanf){ goto _LL408;}
else{ goto _LL409;} _LL409: if( _temp397 == Cyc_Absyn_Sscanf){ goto _LL410;}
else{ goto _LL411;} _LL411: goto _LL412; _LL400: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp413=( char*)"missing format string in printf";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 32; _temp414;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*) es->hd, es->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_VoidType; goto _LL398; _LL402: if( es == 0? 1: es->tl == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp415=( char*)"too few arguments to fprintf";
struct _tagged_string _temp416; _temp416.curr= _temp415; _temp416.base= _temp415;
_temp416.last_plus_one= _temp415 + 29; _temp416;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp417= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp417.last_plus_one - _temp417.curr, _temp417.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_VoidType; goto _LL398; _LL404: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp418=( char*)"missing format string in xprintf";
struct _tagged_string _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 33; _temp419;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*) es->hd, es->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_string_typ( Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)); goto _LL398;
_LL406: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp420=( char*)"missing format string in scanf"; struct _tagged_string
_temp421; _temp421.curr= _temp420; _temp421.base= _temp420; _temp421.last_plus_one=
_temp420 + 31; _temp421;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)
es->hd, es->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL398;
_LL408: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp422=( char*)"too few arguments to fscanf";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 28; _temp423;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp424= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp424.last_plus_one - _temp424.curr, _temp424.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL398; _LL410: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp425=( char*)"sscanf is not supported"; struct
_tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 24; _temp426;})); t= Cyc_Absyn_sint_t; goto
_LL398; _LL412: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL427: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp429=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 35; _temp430;})); case 1: _LL428: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd); break; case 2: _LL431: t= Cyc_Tcexp_tcBinPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd,( struct Cyc_Absyn_Exp*)( es->tl)->hd);
break; default: _LL432: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp434=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 37; _temp435;}));} goto _LL398; _LL398:;}
return t;}} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){
Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp436=( char*)"increment/decrement of non-lvalue";
struct _tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 34; _temp437;}));}{ void* t=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_arithmetic_promote( te, e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp438= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp438.last_plus_one
- _temp438.curr, _temp438.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp439=( char*)"conditional expression";
struct _tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 23; _temp440;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp441=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp441->hd=( void*) e3; _temp441->tl=
0; _temp441;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp442=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp442->hd=(
void*) e2; _temp442->tl=( struct Cyc_List_List*) l1; _temp442;}); if( ! Cyc_Tcutil_coerce_list(
te, t,( struct Cyc_List_List*) l2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp443= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v); struct
_tagged_string _temp444= Cyc_Absynpp_typ2string(( void*)( e3->topt)->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp443.last_plus_one - _temp443.curr, _temp443.curr, _temp444.last_plus_one -
_temp444.curr, _temp444.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)&( e1->topt)->v), e2);{ void*
t1=( void*)( e1->topt)->v; void* t2=( void*)( e2->topt)->v;{ void* _temp445= Cyc_Tcutil_compress(
t1); struct Cyc_Absyn_Exp* _temp451; struct Cyc_Absyn_Tqual* _temp453; void*
_temp455; _LL447: if(( unsigned int) _temp445 > 4u?(( struct _enum_struct*)
_temp445)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL456: _temp455=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp445)->f1; goto _LL454; _LL454: _temp453=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp445)->f2;
goto _LL452; _LL452: _temp451=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp445)->f3; goto _LL448;} else{ goto _LL449;} _LL449: goto _LL450; _LL448:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp457=( char*)"cannot assign to an array";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 26; _temp458;})); goto _LL446; _LL450: goto
_LL446; _LL446:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp459=( char*)"assignment to non-lvalue";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 25; _temp460;}));} if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp461= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp462= Cyc_Absynpp_typ2string(
t2); xprintf("type mismatch: %.*s != %.*s", _temp461.last_plus_one - _temp461.curr,
_temp461.curr, _temp462.last_plus_one - _temp462.curr, _temp462.curr);}));}}
else{ void* p=( void*) po->v; void* t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
p, e1, e2); if( !( Cyc_Tcutil_unify( t_result, t1)? 1: Cyc_Tcutil_coerceable(
t_result))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp463=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 42; _temp464;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)( e2->topt)->v;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp465=
t; struct Cyc_Absyn_PtrInfo _temp471; struct Cyc_Absyn_Conref* _temp473; struct
Cyc_Absyn_Tqual* _temp475; struct Cyc_Absyn_Conref* _temp477; void* _temp479;
void* _temp481; _LL467: if(( unsigned int) _temp465 > 4u?(( struct _enum_struct*)
_temp465)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL472: _temp471=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp465)->f1; _LL482: _temp481=( void*)
_temp471.elt_typ; goto _LL480; _LL480: _temp479=( void*) _temp471.rgn_typ; goto
_LL478; _LL478: _temp477=( struct Cyc_Absyn_Conref*) _temp471.nullable; goto
_LL476; _LL476: _temp475=( struct Cyc_Absyn_Tqual*) _temp471.tq; goto _LL474;
_LL474: _temp473=( struct Cyc_Absyn_Conref*) _temp471.bounds; goto _LL468;}
else{ goto _LL469;} _LL469: goto _LL470; _LL468: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp479); Cyc_Tcutil_check_nonzero_bound( loc, _temp473);{ void*
_temp483= Cyc_Tcutil_compress( _temp481); struct Cyc_Absyn_FnInfo _temp489;
struct Cyc_List_List* _temp491; int _temp493; struct Cyc_List_List* _temp495;
void* _temp497; struct Cyc_Core_Opt* _temp499; struct Cyc_List_List* _temp501;
_LL485: if(( unsigned int) _temp483 > 4u?(( struct _enum_struct*) _temp483)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL490: _temp489=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp483)->f1; _LL502: _temp501=( struct Cyc_List_List*)
_temp489.tvars; goto _LL500; _LL500: _temp499=( struct Cyc_Core_Opt*) _temp489.effect;
goto _LL498; _LL498: _temp497=( void*) _temp489.ret_typ; goto _LL496; _LL496:
_temp495=( struct Cyc_List_List*) _temp489.args; goto _LL494; _LL494: _temp493=(
int) _temp489.varargs; goto _LL492; _LL492: _temp491=( struct Cyc_List_List*)
_temp489.attributes; goto _LL486;} else{ goto _LL487;} _LL487: goto _LL488;
_LL486: if( _temp493){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp503=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 41; _temp504;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp497,* topt);} while( es != 0? _temp495 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*) es->hd; void* t2=(*(( struct _tuple2*) _temp495->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp505= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); struct _tagged_string _temp506= Cyc_Absynpp_typ2string(
t2); xprintf("actual argument has type %.*s but formal has type %.*s", _temp505.last_plus_one
- _temp505.curr, _temp505.curr, _temp506.last_plus_one - _temp506.curr, _temp506.curr);}));}
es= es->tl; _temp495= _temp495->tl;} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp507=( char*)"too many arguments for function";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 32; _temp508;}));} if( _temp495 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp509=( char*)"too few arguments for function";
struct _tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 31; _temp510;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*) _temp499->v); return _temp497; _LL488: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp511=( char*)"expected pointer to function";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 29; _temp512;})); _LL484:;} _LL470: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp513=( char*)"expected pointer to function";
struct _tagged_string _temp514; _temp514.curr= _temp513; _temp514.base= _temp513;
_temp514.last_plus_one= _temp513 + 29; _temp514;})); _LL466:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e);{ void*
t=( void*)( e->topt)->v; if( ! Cyc_Tcutil_unify( t, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp515= Cyc_Absynpp_typ2string( t); xprintf("expected xenum exn but found %.*s",
_temp515.last_plus_one - _temp515.curr, _temp515.curr);}));} return Cyc_Absyn_wildtyp();}}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);{
void* _temp516= t1; struct Cyc_Absyn_PtrInfo _temp522; struct Cyc_Absyn_Conref*
_temp524; struct Cyc_Absyn_Tqual* _temp526; struct Cyc_Absyn_Conref* _temp528;
void* _temp530; void* _temp532; _LL518: if(( unsigned int) _temp516 > 4u?((
struct _enum_struct*) _temp516)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL523:
_temp522=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp516)->f1; _LL533: _temp532=( void*) _temp522.elt_typ; goto _LL531; _LL531:
_temp530=( void*) _temp522.rgn_typ; goto _LL529; _LL529: _temp528=( struct Cyc_Absyn_Conref*)
_temp522.nullable; goto _LL527; _LL527: _temp526=( struct Cyc_Absyn_Tqual*)
_temp522.tq; goto _LL525; _LL525: _temp524=( struct Cyc_Absyn_Conref*) _temp522.bounds;
goto _LL519;} else{ goto _LL520;} _LL520: goto _LL521; _LL519:{ void* _temp534=
Cyc_Tcutil_compress( _temp532); struct Cyc_Absyn_FnInfo _temp540; struct Cyc_List_List*
_temp542; int _temp544; struct Cyc_List_List* _temp546; void* _temp548; struct
Cyc_Core_Opt* _temp550; struct Cyc_List_List* _temp552; _LL536: if((
unsigned int) _temp534 > 4u?(( struct _enum_struct*) _temp534)->tag == Cyc_Absyn_FnType_tag:
0){ _LL541: _temp540=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp534)->f1; _LL553: _temp552=( struct Cyc_List_List*) _temp540.tvars; goto
_LL551; _LL551: _temp550=( struct Cyc_Core_Opt*) _temp540.effect; goto _LL549;
_LL549: _temp548=( void*) _temp540.ret_typ; goto _LL547; _LL547: _temp546=(
struct Cyc_List_List*) _temp540.args; goto _LL545; _LL545: _temp544=( int)
_temp540.varargs; goto _LL543; _LL543: _temp542=( struct Cyc_List_List*)
_temp540.attributes; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539;
_LL537: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp552)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp554=( char*)"type instantiation mismatch"; struct _tagged_string _temp555;
_temp555.curr= _temp554; _temp555.base= _temp554; _temp555.last_plus_one=
_temp554 + 28; _temp555;}));} for( 0; ts != 0; ts= ts->tl, _temp552= _temp552->tl){
void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) _temp552->hd)->kind); Cyc_Tcutil_check_type( loc, te,
Cyc_Tcenv_lookup_type_vars( te), k,( void*) ts->hd); instantiation=({ struct Cyc_List_List*
_temp556=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp556->hd=( void*)({ struct _tuple4* _temp557=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp557->f1=( struct Cyc_Absyn_Tvar*) _temp552->hd;
_temp557->f2=( void*) ts->hd; _temp557;}); _temp556->tl= instantiation; _temp556;});}{
void* new_fn_typ= Cyc_Tcutil_substitute( instantiation,({ struct Cyc_Absyn_FnType_struct*
_temp558=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp558->tag= Cyc_Absyn_FnType_tag; _temp558->f1=({ struct Cyc_Absyn_FnInfo
_temp559; _temp559.tvars= 0; _temp559.effect= _temp550; _temp559.ret_typ=( void*)
_temp548; _temp559.args= _temp546; _temp559.varargs= _temp544; _temp559.attributes=
_temp542; _temp559;});( void*) _temp558;})); void* new_typ=({ struct Cyc_Absyn_PointerType_struct*
_temp560=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp560->tag= Cyc_Absyn_PointerType_tag; _temp560->f1=({ struct Cyc_Absyn_PtrInfo
_temp561; _temp561.elt_typ=( void*) new_fn_typ; _temp561.rgn_typ=( void*)
_temp530; _temp561.nullable= _temp528; _temp561.tq= _temp526; _temp561.bounds=
_temp524; _temp561;});( void*) _temp560;}); return new_typ;}} _LL539: goto
_LL535; _LL535:;} goto _LL517; _LL521: goto _LL517; _LL517:;} return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp562= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp562.last_plus_one - _temp562.curr, _temp562.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp563= Cyc_Absynpp_typ2string( t2); struct _tagged_string _temp564= Cyc_Absynpp_typ2string(
t); xprintf("cannot cast %.*s to %.*s", _temp563.last_plus_one - _temp563.curr,
_temp563.curr, _temp564.last_plus_one - _temp564.curr, _temp564.curr);}));}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void** topt2=
0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual(); if( topt != 0){ void*
_temp565= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp571; struct
Cyc_Absyn_Conref* _temp573; struct Cyc_Absyn_Tqual* _temp575; struct Cyc_Absyn_Conref*
_temp577; void* _temp579; void* _temp581; _LL567: if(( unsigned int) _temp565 >
4u?(( struct _enum_struct*) _temp565)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL572: _temp571=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp565)->f1; _LL582: _temp581=( void*) _temp571.elt_typ; goto _LL580; _LL580:
_temp579=( void*) _temp571.rgn_typ; goto _LL578; _LL578: _temp577=( struct Cyc_Absyn_Conref*)
_temp571.nullable; goto _LL576; _LL576: _temp575=( struct Cyc_Absyn_Tqual*)
_temp571.tq; goto _LL574; _LL574: _temp573=( struct Cyc_Absyn_Conref*) _temp571.bounds;
goto _LL568;} else{ goto _LL569;} _LL569: goto _LL570; _LL568: topt2=({ void**
_temp583=( void**) GC_malloc( sizeof( void*) * 1); _temp583[ 0]= _temp581;
_temp583;}); tq2= _temp575; goto _LL566; _LL570: goto _LL566; _LL566:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp584=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp592; struct Cyc_List_List* _temp594; struct Cyc_Core_Opt* _temp596; struct
_tuple1* _temp598; struct Cyc_List_List* _temp600; _LL586: if((( struct
_enum_struct*) _temp584)->tag == Cyc_Absyn_Struct_e_tag){ _LL599: _temp598=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp584)->f1; goto _LL597;
_LL597: _temp596=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp584)->f2; goto _LL595; _LL595: _temp594=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp584)->f3; goto _LL593; _LL593: _temp592=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp584)->f4;
goto _LL587;} else{ goto _LL588;} _LL588: if((( struct _enum_struct*) _temp584)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL601: _temp600=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp584)->f1; goto _LL589;} else{ goto _LL590;}
_LL590: goto _LL591; _LL587: goto _LL589; _LL589: { void*(* ptr_maker)( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){ void*
_temp602= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp608; struct
Cyc_Absyn_Conref* _temp610; struct Cyc_Absyn_Tqual* _temp612; struct Cyc_Absyn_Conref*
_temp614; void* _temp616; void* _temp618; _LL604: if(( unsigned int) _temp602 >
4u?(( struct _enum_struct*) _temp602)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL609: _temp608=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp602)->f1; _LL619: _temp618=( void*) _temp608.elt_typ; goto _LL617; _LL617:
_temp616=( void*) _temp608.rgn_typ; goto _LL615; _LL615: _temp614=( struct Cyc_Absyn_Conref*)
_temp608.nullable; goto _LL613; _LL613: _temp612=( struct Cyc_Absyn_Tqual*)
_temp608.tq; goto _LL611; _LL611: _temp610=( struct Cyc_Absyn_Conref*) _temp608.bounds;
goto _LL605;} else{ goto _LL606;} _LL606: goto _LL607; _LL605: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp614)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL603; _LL607: goto _LL603; _LL603:;} return ptr_maker(( void*)( e->topt)->v,
Cyc_Absyn_HeapRgn, tq2);} _LL591: goto _LL585; _LL585:;}{ void* _temp622; int
_temp624; struct _tuple5 _temp620= Cyc_Tcutil_addressof_props( te, e); _LL625:
_temp624= _temp620.f1; goto _LL623; _LL623: _temp622= _temp620.f2; goto _LL621;
_LL621: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if( _temp624){ tq->q_const=
1;}{ void* t= Cyc_Absyn_at_typ(( void*)( e->topt)->v, _temp622, tq); return t;}}}}
static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), Cyc_Absyn_MemKind, t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp626= t; struct Cyc_Absyn_PtrInfo
_temp632; struct Cyc_Absyn_Conref* _temp634; struct Cyc_Absyn_Tqual* _temp636;
struct Cyc_Absyn_Conref* _temp638; void* _temp640; void* _temp642; _LL628: if((
unsigned int) _temp626 > 4u?(( struct _enum_struct*) _temp626)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL633: _temp632=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp626)->f1; _LL643: _temp642=( void*) _temp632.elt_typ; goto _LL641; _LL641:
_temp640=( void*) _temp632.rgn_typ; goto _LL639; _LL639: _temp638=( struct Cyc_Absyn_Conref*)
_temp632.nullable; goto _LL637; _LL637: _temp636=( struct Cyc_Absyn_Tqual*)
_temp632.tq; goto _LL635; _LL635: _temp634=( struct Cyc_Absyn_Conref*) _temp632.bounds;
goto _LL629;} else{ goto _LL630;} _LL630: goto _LL631; _LL629: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp640); Cyc_Tcutil_check_nonzero_bound( loc, _temp634); return
_temp642; _LL631: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp644= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp644.last_plus_one - _temp644.curr, _temp644.curr);})); _LL627:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* _temp645= t; struct Cyc_Absyn_Structdecl** _temp657;
struct Cyc_Absyn_Structdecl* _temp659; struct Cyc_List_List* _temp660; struct
_tuple1* _temp662; struct Cyc_Absyn_Uniondecl** _temp664; struct Cyc_Absyn_Uniondecl*
_temp666; struct Cyc_List_List* _temp667; struct _tuple1* _temp669; struct Cyc_Absyn_Exp*
_temp672; struct Cyc_Absyn_Tqual* _temp674; void* _temp676; struct Cyc_Absyn_PtrInfo
_temp681; _LL647: if(( unsigned int) _temp645 > 4u?(( struct _enum_struct*)
_temp645)->tag == Cyc_Absyn_StructType_tag: 0){ _LL663: _temp662=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp645)->f1; goto _LL661;
_LL661: _temp660=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp645)->f2; goto _LL658; _LL658: _temp657=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp645)->f3; if( _temp657 == 0){ goto
_LL649;} else{ _temp659=* _temp657; goto _LL648;}} else{ goto _LL649;} _LL649:
if(( unsigned int) _temp645 > 4u?(( struct _enum_struct*) _temp645)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL670: _temp669=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp645)->f1; goto _LL668; _LL668: _temp667=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp645)->f2; goto _LL665; _LL665: _temp664=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp645)->f3;
if( _temp664 == 0){ goto _LL651;} else{ _temp666=* _temp664; goto _LL650;}}
else{ goto _LL651;} _LL651: if(( unsigned int) _temp645 > 4u?(( struct
_enum_struct*) _temp645)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL677: _temp676=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp645)->f1; goto _LL675; _LL675:
_temp674=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp645)->f2; goto _LL673; _LL673: _temp672=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp645)->f3; goto _LL671;} else{ goto _LL653;}
_LL671: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char* _temp678=(
char*)"size"; struct _tagged_string _temp679; _temp679.curr= _temp678; _temp679.base=
_temp678; _temp679.last_plus_one= _temp678 + 5; _temp679;})) == 0){ goto _LL652;}
else{ goto _LL653;} _LL653: if(( unsigned int) _temp645 > 4u?(( struct
_enum_struct*) _temp645)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL682: _temp681=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp645)->f1;
goto _LL680;} else{ goto _LL655;} _LL680: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp683=( char*)"size"; struct _tagged_string _temp684;
_temp684.curr= _temp683; _temp684.base= _temp683; _temp684.last_plus_one=
_temp683 + 5; _temp684;})) == 0){ goto _LL654;} else{ goto _LL655;} _LL655: goto
_LL656; _LL648: if( _temp662 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp685=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 40; _temp686;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp659, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp688= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp687= _temp662; if( _temp687 == 0){ _throw(
Null_Exception);} _temp687;})); struct _tagged_string _temp689=* f; xprintf("struct %.*s has no %.*s field",
_temp688.last_plus_one - _temp688.curr, _temp688.curr, _temp689.last_plus_one -
_temp689.curr, _temp689.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp659->tvs,
_temp660); void* t2= Cyc_Tcutil_substitute( inst,( void*) finfo->type); return
t2;}} _LL650: if( _temp669 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp690=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp691; _temp691.curr= _temp690; _temp691.base= _temp690;
_temp691.last_plus_one= _temp690 + 39; _temp691;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp666, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp693= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp692= _temp669; if( _temp692 == 0){ _throw(
Null_Exception);} _temp692;})); struct _tagged_string _temp694=* f; xprintf("union %.*s has no %.*s field",
_temp693.last_plus_one - _temp693.curr, _temp693.curr, _temp694.last_plus_one -
_temp694.curr, _temp694.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp666->tvs,
_temp667); void* t2= Cyc_Tcutil_substitute( inst,( void*) finfo->type); return
t2;}} _LL652:( void*)( outer_e->r=( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp695=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp695->tag= Cyc_Absyn_Primop_e_tag; _temp695->f1=( void*) Cyc_Absyn_Size;
_temp695->f2=({ struct Cyc_List_List* _temp696=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp696->hd=( void*) e; _temp696->tl=
0; _temp696;});( void*) _temp695;})); return Cyc_Absyn_uint_t; _LL654:( void*)(
outer_e->r=( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp697=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp697->tag= Cyc_Absyn_Primop_e_tag;
_temp697->f1=( void*) Cyc_Absyn_Size; _temp697->f2=({ struct Cyc_List_List*
_temp698=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp698->hd=( void*) e; _temp698->tl= 0; _temp698;});( void*) _temp697;}));
return Cyc_Absyn_uint_t; _LL656: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp699=( char*)"size"; struct _tagged_string _temp700;
_temp700.curr= _temp699; _temp700.base= _temp699; _temp700.last_plus_one=
_temp699 + 5; _temp700;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp701= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp701.last_plus_one - _temp701.curr, _temp701.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp702= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp702.last_plus_one - _temp702.curr, _temp702.curr);}));} _LL646:;}} static
void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp703=
t; struct Cyc_Absyn_PtrInfo _temp709; struct Cyc_Absyn_Conref* _temp711; struct
Cyc_Absyn_Tqual* _temp713; struct Cyc_Absyn_Conref* _temp715; void* _temp717;
void* _temp719; _LL705: if(( unsigned int) _temp703 > 4u?(( struct _enum_struct*)
_temp703)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL710: _temp709=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp703)->f1; _LL720: _temp719=( void*)
_temp709.elt_typ; goto _LL718; _LL718: _temp717=( void*) _temp709.rgn_typ; goto
_LL716; _LL716: _temp715=( struct Cyc_Absyn_Conref*) _temp709.nullable; goto
_LL714; _LL714: _temp713=( struct Cyc_Absyn_Tqual*) _temp709.tq; goto _LL712;
_LL712: _temp711=( struct Cyc_Absyn_Conref*) _temp709.bounds; goto _LL706;}
else{ goto _LL707;} _LL707: goto _LL708; _LL706: Cyc_Tcutil_check_nonzero_bound(
loc, _temp711);{ void* _temp721= Cyc_Tcutil_compress( _temp719); struct Cyc_Absyn_Structdecl**
_temp729; struct Cyc_Absyn_Structdecl* _temp731; struct Cyc_List_List* _temp732;
struct _tuple1* _temp734; struct Cyc_Absyn_Uniondecl** _temp736; struct Cyc_Absyn_Uniondecl*
_temp738; struct Cyc_List_List* _temp739; struct _tuple1* _temp741; _LL723: if((
unsigned int) _temp721 > 4u?(( struct _enum_struct*) _temp721)->tag == Cyc_Absyn_StructType_tag:
0){ _LL735: _temp734=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp721)->f1; goto _LL733; _LL733: _temp732=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp721)->f2; goto _LL730; _LL730: _temp729=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp721)->f3;
if( _temp729 == 0){ goto _LL725;} else{ _temp731=* _temp729; goto _LL724;}}
else{ goto _LL725;} _LL725: if(( unsigned int) _temp721 > 4u?(( struct
_enum_struct*) _temp721)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL742: _temp741=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp721)->f1; goto
_LL740; _LL740: _temp739=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp721)->f2; goto _LL737; _LL737: _temp736=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp721)->f3; if( _temp736 == 0){ goto
_LL727;} else{ _temp738=* _temp736; goto _LL726;}} else{ goto _LL727;} _LL727:
goto _LL728; _LL724: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp731, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp744= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp743= _temp734; if( _temp743 == 0){ _throw( Null_Exception);}
_temp743;})); struct _tagged_string _temp745=* f; xprintf("struct %.*s has no %.*s field",
_temp744.last_plus_one - _temp744.curr, _temp744.curr, _temp745.last_plus_one -
_temp745.curr, _temp745.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp731->tvs,
_temp732); void* t3= Cyc_Tcutil_substitute( inst,( void*) finfo->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp717); return t3;}} _LL726: { struct Cyc_Absyn_Structfield* finfo=
Cyc_Absyn_lookup_union_field( _temp738, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp747= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp746= _temp741; if( _temp746 == 0){ _throw(
Null_Exception);} _temp746;})); struct _tagged_string _temp748=* f; xprintf("union %.*s has no %.*s field",
_temp747.last_plus_one - _temp747.curr, _temp747.curr, _temp748.last_plus_one -
_temp748.curr, _temp748.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp738->tvs,
_temp739); void* t3= Cyc_Tcutil_substitute( inst,( void*) finfo->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp717); return t3;}} _LL728: return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp749= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp749.last_plus_one - _temp749.curr, _temp749.curr);})); _LL722:;} _LL708:
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp750= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp750.last_plus_one -
_temp750.curr, _temp750.curr);})); _LL704:;}} static void* Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp751; _push_handler(& _temp751);{ struct
_xenum_struct* _temp752=( struct _xenum_struct*) setjmp( _temp751.handler); if(
! _temp752){{ void* _temp753=(*(( struct _tuple7*(*)( struct Cyc_List_List* x,
int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp753;};
_pop_handler();} else{ struct _xenum_struct* _temp755= _temp752; _LL757: if(
_temp755->tag == Cyc_List_Nth_tag){ goto _LL758;} else{ goto _LL759;} _LL759:
goto _LL760; _LL758: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL760:(
void) _throw( _temp755); _LL756:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return Cyc_Tcexp_expr_err(
te, e2->loc,({ struct _tagged_string _temp761= Cyc_Absynpp_typ2string( t2);
xprintf("expecting int subscript, found %.*s", _temp761.last_plus_one - _temp761.curr,
_temp761.curr);}));}{ void* _temp762= t1; struct Cyc_Absyn_PtrInfo _temp770;
struct Cyc_Absyn_Conref* _temp772; struct Cyc_Absyn_Tqual* _temp774; struct Cyc_Absyn_Conref*
_temp776; void* _temp778; void* _temp780; struct Cyc_List_List* _temp782; _LL764:
if(( unsigned int) _temp762 > 4u?(( struct _enum_struct*) _temp762)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL771: _temp770=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp762)->f1; _LL781: _temp780=( void*) _temp770.elt_typ; goto _LL779; _LL779:
_temp778=( void*) _temp770.rgn_typ; goto _LL777; _LL777: _temp776=( struct Cyc_Absyn_Conref*)
_temp770.nullable; goto _LL775; _LL775: _temp774=( struct Cyc_Absyn_Tqual*)
_temp770.tq; goto _LL773; _LL773: _temp772=( struct Cyc_Absyn_Conref*) _temp770.bounds;
goto _LL765;} else{ goto _LL766;} _LL766: if(( unsigned int) _temp762 > 4u?((
struct _enum_struct*) _temp762)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL783:
_temp782=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp762)->f1;
goto _LL767;} else{ goto _LL768;} _LL768: goto _LL769; _LL765: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp772);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp772);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp778); return _temp780; _LL767: return Cyc_Tcexp_ithTupleType( te,
loc, _temp782, e2); _LL769: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp784= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp784.last_plus_one - _temp784.curr, _temp784.curr);})); _LL763:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp785= Cyc_Tcutil_compress(* topt); struct
Cyc_List_List* _temp791; _LL787: if(( unsigned int) _temp785 > 4u?(( struct
_enum_struct*) _temp785)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL792: _temp791=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp785)->f1; goto
_LL788;} else{ goto _LL789;} _LL789: goto _LL790; _LL788: if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp791) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp793=( char*)"tuple expression has the wrong number of fields"; struct
_tagged_string _temp794; _temp794.curr= _temp793; _temp794.base= _temp793;
_temp794.last_plus_one= _temp793 + 48; _temp794;})); goto _LL786;} for( 0; es !=
0; es= es->tl, _temp791= _temp791->tl){ void* topt2=(*(( struct _tuple7*)
_temp791->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& topt2,( struct Cyc_Absyn_Exp*)
es->hd); fields=({ struct Cyc_List_List* _temp795=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp795->hd=( void*)({ struct
_tuple7* _temp796=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp796[ 0]=({ struct _tuple7 _temp797; _temp797.f1=(*(( struct _tuple7*)
_temp791->hd)).f1; _temp797.f2=( void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v;
_temp797;}); _temp796;}); _temp795->tl= fields; _temp795;});} done= 1; goto
_LL786; _LL790: goto _LL786; _LL786:;} if( ! done){ for( 0; es != 0; es= es->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*) es->hd); fields=({
struct Cyc_List_List* _temp798=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp798->hd=( void*)({ struct _tuple7* _temp799=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp799[ 0]=({ struct
_tuple7 _temp800; _temp800.f1= Cyc_Absyn_empty_tqual(); _temp800.f2=( void*)(((
struct Cyc_Absyn_Exp*) es->hd)->topt)->v; _temp800;}); _temp799;}); _temp798->tl=
fields; _temp798;});}} return({ struct Cyc_Absyn_TupleType_struct* _temp801=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp801->tag= Cyc_Absyn_TupleType_tag; _temp801->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);( void*) _temp801;});}
static void* Cyc_Tcexp_tcCompoundLit( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp802=( char*)"tcCompoundLit";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 14; _temp803;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(
Cyc_Absyn_MemKind); void* sz_rexp=({ struct Cyc_Absyn_Const_e_struct* _temp804=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp804->tag= Cyc_Absyn_Const_e_tag; _temp804->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp805=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp805->tag= Cyc_Absyn_Int_c_tag; _temp805->f1=( void*) Cyc_Absyn_Unsigned;
_temp805->f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);( void*)
_temp805;});( void*) _temp804;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp(
sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp806=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp806->v=( void*) Cyc_Absyn_uint_t;
_temp806;});{ void* res_t2=({ struct Cyc_Absyn_ArrayType_struct* _temp807=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp807->tag= Cyc_Absyn_ArrayType_tag; _temp807->f1=( void*) res; _temp807->f2=
Cyc_Absyn_empty_tqual(); _temp807->f3=( struct Cyc_Absyn_Exp*) sz_exp;( void*)
_temp807;});{ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2= es2->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*) es2->hd);}}
if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
es->hd)->loc,({ struct _tagged_string _temp808= Cyc_Absynpp_typ2string( res);
xprintf("elements of array do not all have the same type (%.*s)", _temp808.last_plus_one
- _temp808.curr, _temp808.curr);}));}{ int offset= 0; for( 0; des != 0; offset
++, des= des->tl){ struct Cyc_List_List* ds=(*(( struct _tuple6*) des->hd)).f1;
if( ds != 0){ void* _temp809=( void*) ds->hd; struct _tagged_string* _temp815;
struct Cyc_Absyn_Exp* _temp817; _LL811: if((( struct _enum_struct*) _temp809)->tag
== Cyc_Absyn_FieldName_tag){ _LL816: _temp815=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp809)->f1; goto _LL812;} else{ goto _LL813;}
_LL813: if((( struct _enum_struct*) _temp809)->tag == Cyc_Absyn_ArrayElement_tag){
_LL818: _temp817=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp809)->f1; goto _LL814;} else{ goto _LL810;} _LL812: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp819=( char*)"only array index designators are supported";
struct _tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 43; _temp820;})); goto _LL810; _LL814: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp817);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp817); if(
i !=( unsigned int) offset){ Cyc_Tcutil_terr( _temp817->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL810;} _LL810:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp821= Cyc_Absynpp_typ2string((
void*)( bound->topt)->v); xprintf("expecting unsigned int, found %.*s", _temp821.last_plus_one
- _temp821.curr, _temp821.curr);}));} if( te->le != 0){ te= Cyc_Tcenv_new_block(
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo;
void** topt2= 0; struct Cyc_Absyn_Tqual** tqopt= 0; if( topt != 0){ void*
_temp822= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp830; struct
Cyc_Absyn_Exp* _temp832; struct Cyc_Absyn_Tqual* _temp834; struct Cyc_Absyn_Tqual**
_temp836; void* _temp837; void** _temp839; _LL824: if(( unsigned int) _temp822 >
4u?(( struct _enum_struct*) _temp822)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL831: _temp830=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp822)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if(( unsigned int)
_temp822 > 4u?(( struct _enum_struct*) _temp822)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL838: _temp837=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp822)->f1;
_temp839=&(( struct Cyc_Absyn_ArrayType_struct*) _temp822)->f1; goto _LL835;
_LL835: _temp834=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp822)->f2; _temp836=&(( struct Cyc_Absyn_ArrayType_struct*) _temp822)->f2;
goto _LL833; _LL833: _temp832=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp822)->f3; goto _LL827;} else{ goto _LL828;} _LL828: goto _LL829; _LL825:
pinfo= _temp830; topt2=( void**)(( void*)& pinfo.elt_typ); tqopt=( struct Cyc_Absyn_Tqual**)&
pinfo.tq; goto _LL823; _LL827: topt2=( void**) _temp839; tqopt=( struct Cyc_Absyn_Tqual**)
_temp836; goto _LL823; _LL829: goto _LL823; _LL823:;}{ void* t= Cyc_Tcexp_tcExp(
te, topt2, body); return({ struct Cyc_Absyn_ArrayType_struct* _temp840=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp840->tag= Cyc_Absyn_ArrayType_tag; _temp840->f1=( void*) t; _temp840->f2=
tqopt == 0? Cyc_Absyn_empty_tqual():* tqopt; _temp840->f3=( struct Cyc_Absyn_Exp*)
bound;( void*) _temp840;});}}} static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp841=* sd_opt; if( _temp841 == 0){ _throw(
Null_Exception);} _temp841;});} else{{ struct _handler_cons _temp842;
_push_handler(& _temp842);{ struct _xenum_struct* _temp843=( struct
_xenum_struct*) setjmp( _temp842.handler); if( ! _temp843){ sd=* Cyc_Tcenv_lookup_structdecl(
te, loc,* tn);; _pop_handler();} else{ struct _xenum_struct* _temp845= _temp843;
_LL847: if( _temp845->tag == Cyc_Dict_Absent_tag){ goto _LL848;} else{ goto
_LL849;} _LL849: goto _LL850; _LL848: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp851= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp851.last_plus_one - _temp851.curr, _temp851.curr);})); return topt != 0?*
topt: Cyc_Absyn_VoidType; _LL850:( void) _throw( _temp845); _LL846:;}}}* sd_opt=(
struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=( struct _tuple1*)( sd->name)->v;}}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var,
sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* res_typ=({ struct Cyc_Absyn_StructType_struct*
_temp852=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp852->tag= Cyc_Absyn_StructType_tag; _temp852->f1=( struct _tuple1*)* tn;
_temp852->f2= typs; _temp852->f3=({ struct Cyc_Absyn_Structdecl** _temp853=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)
* 1); _temp853[ 0]= sd; _temp853;});( void*) _temp852;}); if( topt != 0){ Cyc_Tcutil_unify(
res_typ,* topt);}* otyps=({ struct Cyc_Core_Opt* _temp854=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp854->v=( void*) typs; _temp854;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields= fields->tl){ struct _tuple8
_temp857; struct Cyc_Absyn_Exp* _temp858; struct Cyc_Absyn_Structfield* _temp860;
struct _tuple8* _temp855=( struct _tuple8*) fields->hd; _temp857=* _temp855;
_LL861: _temp860= _temp857.f1; goto _LL859; _LL859: _temp858= _temp857.f2; goto
_LL856; _LL856: { void* inst_fieldtyp= Cyc_Tcutil_substitute( inst,( void*)
_temp860->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& inst_fieldtyp,
_temp858); if( ! Cyc_Tcutil_coerce_arg( te, _temp858, inst_fieldtyp)){ Cyc_Tcutil_terr(
_temp858->loc,({ struct _tagged_string _temp862=* _temp860->name; struct
_tagged_string _temp863= Cyc_Absynpp_qvar2string(* tn); struct _tagged_string
_temp864= Cyc_Absynpp_typ2string( inst_fieldtyp); struct _tagged_string _temp865=
Cyc_Absynpp_typ2string(( void*)( _temp858->topt)->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp862.last_plus_one - _temp862.curr, _temp862.curr, _temp863.last_plus_one -
_temp863.curr, _temp863.curr, _temp864.last_plus_one - _temp864.curr, _temp864.curr,
_temp865.last_plus_one - _temp865.curr, _temp865.curr);}));}}} return res_typ;}}}
static void* Cyc_Tcexp_tcEnum( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Enumdecl* ed, struct Cyc_Absyn_Enumfield*
ef){ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var,
ed->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); struct
Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( all_inst, exist_inst);* all_ref=({ struct
Cyc_Core_Opt* _temp866=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp866->v=( void*) all_typs; _temp866;});* exist_ref=({ struct Cyc_Core_Opt*
_temp867=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp867->v=( void*) exist_typs; _temp867;});{ void* res=({ struct Cyc_Absyn_EnumType_struct*
_temp868=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp868->tag= Cyc_Absyn_EnumType_tag; _temp868->f1=( struct _tuple1*)(( struct
_tuple1*)( ed->name)->v); _temp868->f2= all_typs; _temp868->f3=({ struct Cyc_Absyn_Enumdecl**
_temp869=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)
* 1); _temp869[ 0]= ed; _temp869;});( void*) _temp868;}); if( topt != 0){ Cyc_Tcutil_unify(*
topt, res);}{ struct Cyc_List_List* ts= ef->typs; for( 0; es != 0? ts != 0: 0;
es= es->tl, ts= ts->tl){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd;
void* t= Cyc_Tcutil_substitute( inst,(*(( struct _tuple7*) ts->hd)).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp872= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp873= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp874= e->topt == 0?( struct _tagged_string)({ char* _temp870=( char*)"?";
struct _tagged_string _temp871; _temp871.curr= _temp870; _temp871.base= _temp870;
_temp871.last_plus_one= _temp870 + 2; _temp871;}): Cyc_Absynpp_typ2string(( void*)(
e->topt)->v); xprintf("enum constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp872.last_plus_one - _temp872.curr, _temp872.curr, _temp873.last_plus_one -
_temp873.curr, _temp873.curr, _temp874.last_plus_one - _temp874.curr, _temp874.curr);}));}}
if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*) es->hd)->loc,({
struct _tagged_string _temp875= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for enum constructor %.*s",
_temp875.last_plus_one - _temp875.curr, _temp875.curr);}));} if( ts != 0){
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp876= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too few arguments for enum constructor %.*s", _temp876.last_plus_one
- _temp876.curr, _temp876.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXenum(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Xenumdecl*
xd, struct Cyc_Absyn_Enumfield* ef){ struct Cyc_List_List* exist_inst=(( struct
Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); void*
res=({ struct Cyc_Absyn_XenumType_struct* _temp877=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp877->tag= Cyc_Absyn_XenumType_tag;
_temp877->f1= xd->name; _temp877->f2=({ struct Cyc_Absyn_Xenumdecl** _temp878=(
struct Cyc_Absyn_Xenumdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl*) *
1); _temp878[ 0]= xd; _temp878;});( void*) _temp877;}); struct Cyc_List_List* ts=
ef->typs;* exist_ref=({ struct Cyc_Core_Opt* _temp879=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp879->v=( void*) exist_typs;
_temp879;}); while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
es->hd; void* t= Cyc_Tcutil_substitute( exist_inst,(*(( struct _tuple7*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); es= es->tl; ts= ts->tl; if( !
Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp880= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp881= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp882= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("xenum constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp880.last_plus_one - _temp880.curr, _temp880.curr, _temp881.last_plus_one -
_temp881.curr, _temp881.curr, _temp882.last_plus_one - _temp882.curr, _temp882.curr);}));}}
if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct
_tagged_string _temp883= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for xenum constructor %.*s",
_temp883.last_plus_one - _temp883.curr, _temp883.curr);}));} else{ if( ts != 0){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string
_temp884= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xenum constructor %.*s",
_temp884.last_plus_one - _temp884.curr, _temp884.curr);}));}} return res;}
static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void** ma_ptr){ void* _temp885=* ma_ptr; void* _temp895;
struct _tuple1* _temp897; struct Cyc_Absyn_Enumfield* _temp899; struct Cyc_Absyn_Enumdecl*
_temp901; struct Cyc_Absyn_Enumfield* _temp903; struct Cyc_Absyn_Xenumdecl*
_temp905; _LL887: if((( struct _enum_struct*) _temp885)->tag == Cyc_Absyn_Typ_m_tag){
_LL896: _temp895=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp885)->f1; goto
_LL888;} else{ goto _LL889;} _LL889: if((( struct _enum_struct*) _temp885)->tag
== Cyc_Absyn_Unresolved_m_tag){ _LL898: _temp897=( struct _tuple1*)(( struct Cyc_Absyn_Unresolved_m_struct*)
_temp885)->f1; goto _LL890;} else{ goto _LL891;} _LL891: if((( struct
_enum_struct*) _temp885)->tag == Cyc_Absyn_Enum_m_tag){ _LL902: _temp901=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_m_struct*) _temp885)->f1;
goto _LL900; _LL900: _temp899=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_m_struct*)
_temp885)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if((( struct
_enum_struct*) _temp885)->tag == Cyc_Absyn_Xenum_m_tag){ _LL906: _temp905=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_m_struct*) _temp885)->f1;
goto _LL904; _LL904: _temp903=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_m_struct*)
_temp885)->f2; goto _LL894;} else{ goto _LL886;} _LL888: Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), Cyc_Absyn_MemKind, _temp895);{ void*(*
ptr_maker)( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp907= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp913; struct Cyc_Absyn_Conref* _temp915; struct Cyc_Absyn_Tqual* _temp917;
struct Cyc_Absyn_Conref* _temp919; void* _temp921; void* _temp923; _LL909: if((
unsigned int) _temp907 > 4u?(( struct _enum_struct*) _temp907)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL914: _temp913=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp907)->f1; _LL924: _temp923=( void*) _temp913.elt_typ; goto _LL922; _LL922:
_temp921=( void*) _temp913.rgn_typ; goto _LL920; _LL920: _temp919=( struct Cyc_Absyn_Conref*)
_temp913.nullable; goto _LL918; _LL918: _temp917=( struct Cyc_Absyn_Tqual*)
_temp913.tq; goto _LL916; _LL916: _temp915=( struct Cyc_Absyn_Conref*) _temp913.bounds;
goto _LL910;} else{ goto _LL911;} _LL911: goto _LL912; _LL910: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp919)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL908; _LL912: goto _LL908; _LL908:;} return ptr_maker( _temp895, Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual());} _LL890:{ struct _handler_cons _temp925; _push_handler(&
_temp925);{ struct _xenum_struct* _temp926=( struct _xenum_struct*) setjmp(
_temp925.handler); if( ! _temp926){{ void* _temp927= Cyc_Tcenv_lookup_ordinary(
te, loc, _temp897); struct Cyc_Absyn_Enumfield* _temp935; struct Cyc_Absyn_Enumdecl*
_temp937; struct Cyc_Absyn_Enumfield* _temp939; struct Cyc_Absyn_Xenumdecl*
_temp941; _LL929: if((( struct _enum_struct*) _temp927)->tag == Cyc_Tcenv_EnumRes_tag){
_LL938: _temp937=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp927)->f1; goto _LL936; _LL936: _temp935=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp927)->f2; goto _LL930;} else{ goto _LL931;}
_LL931: if((( struct _enum_struct*) _temp927)->tag == Cyc_Tcenv_XenumRes_tag){
_LL942: _temp941=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp927)->f1; goto _LL940; _LL940: _temp939=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp927)->f2; goto _LL932;} else{ goto
_LL933;} _LL933: goto _LL934; _LL930:* ma_ptr=({ struct Cyc_Absyn_Enum_m_struct*
_temp943=( struct Cyc_Absyn_Enum_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_m_struct));
_temp943->tag= Cyc_Absyn_Enum_m_tag; _temp943->f1= _temp937; _temp943->f2=
_temp935;( void*) _temp943;});{ void* _temp944= Cyc_Tcexp_tcMalloc( te, loc,
topt, ma_ptr); _npop_handler( 0u); return _temp944;} _LL932:* ma_ptr=({ struct
Cyc_Absyn_Xenum_m_struct* _temp945=( struct Cyc_Absyn_Xenum_m_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Xenum_m_struct)); _temp945->tag= Cyc_Absyn_Xenum_m_tag;
_temp945->f1= _temp941; _temp945->f2= _temp939;( void*) _temp945;});{ void*
_temp946= Cyc_Tcexp_tcMalloc( te, loc, topt, ma_ptr); _npop_handler( 0u); return
_temp946;} _LL934: goto _LL928; _LL928:;}; _pop_handler();} else{ struct
_xenum_struct* _temp948= _temp926; _LL950: if( _temp948->tag == Cyc_Dict_Absent_tag){
goto _LL951;} else{ goto _LL952;} _LL952: goto _LL953; _LL951: goto _LL949;
_LL953:( void) _throw( _temp948); _LL949:;}}} Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp954= Cyc_Absynpp_qvar2string( _temp897); xprintf("malloc argument %.*s is neither a type nor a variant",
_temp954.last_plus_one - _temp954.curr, _temp954.curr);})); if( topt != 0){
return* topt;} return Cyc_Absyn_VoidType; _LL892: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp955=( char*)"malloc of enum variant not yet implemented";
struct _tagged_string _temp956; _temp956.curr= _temp955; _temp956.base= _temp955;
_temp956.last_plus_one= _temp955 + 43; _temp956;}));{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp901->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); return({ struct Cyc_Absyn_EnumType_struct* _temp957=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp957->tag= Cyc_Absyn_EnumType_tag; _temp957->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp901->name)->v); _temp957->f2= all_typs; _temp957->f3=({ struct
Cyc_Absyn_Enumdecl** _temp958=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*) * 1); _temp958[ 0]= _temp901; _temp958;});( void*)
_temp957;});} _LL894: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp959=( char*)"malloc of xenum variant not yet implemented"; struct
_tagged_string _temp960; _temp960.curr= _temp959; _temp960.base= _temp959;
_temp960.last_plus_one= _temp959 + 44; _temp960;})); return({ struct Cyc_Absyn_XenumType_struct*
_temp961=( struct Cyc_Absyn_XenumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct));
_temp961->tag= Cyc_Absyn_XenumType_tag; _temp961->f1= _temp905->name; _temp961->f2=({
struct Cyc_Absyn_Xenumdecl** _temp962=( struct Cyc_Absyn_Xenumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Xenumdecl*) * 1); _temp962[ 0]= _temp905; _temp962;});(
void*) _temp961;}); _LL886:;} static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Stmt* s){
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( te, s), s, 1); Cyc_NewControlFlow_set_encloser(
s, Cyc_Tcenv_get_encloser( te)); while( 1) { void* _temp963=( void*) s->r;
struct Cyc_Absyn_Exp* _temp973; struct Cyc_Absyn_Stmt* _temp975; struct Cyc_Absyn_Stmt*
_temp977; struct Cyc_Absyn_Stmt* _temp979; struct Cyc_Absyn_Decl* _temp981;
_LL965: if(( unsigned int) _temp963 > 1u?(( struct _enum_struct*) _temp963)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL974: _temp973=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp963)->f1; goto _LL966;} else{ goto _LL967;} _LL967:
if(( unsigned int) _temp963 > 1u?(( struct _enum_struct*) _temp963)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL978: _temp977=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp963)->f1; goto _LL976; _LL976: _temp975=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp963)->f2; goto _LL968;} else{ goto _LL969;} _LL969:
if(( unsigned int) _temp963 > 1u?(( struct _enum_struct*) _temp963)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL982: _temp981=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp963)->f1; goto _LL980; _LL980: _temp979=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp963)->f2; goto _LL970;} else{ goto _LL971;}
_LL971: goto _LL972; _LL966: return( void*)( _temp973->topt)->v; _LL968: s=
_temp975; continue; _LL970: s= _temp979; continue; _LL972: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp983=( char*)"statement expression must end with expression";
struct _tagged_string _temp984; _temp984.curr= _temp983; _temp984.base= _temp983;
_temp984.last_plus_one= _temp983 + 46; _temp984;})); _LL964:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp985=( char*)"tcCodegen"; struct _tagged_string
_temp986; _temp986.curr= _temp985; _temp986.base= _temp985; _temp986.last_plus_one=
_temp985 + 10; _temp986;}));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp987=(
char*)"tcFill"; struct _tagged_string _temp988; _temp988.curr= _temp987;
_temp988.base= _temp987; _temp988.last_plus_one= _temp987 + 7; _temp988;}));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void*
_temp989=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1001; struct Cyc_Absyn_Exp*
_temp1003; struct Cyc_Absyn_Vardecl* _temp1005; struct Cyc_List_List* _temp1007;
struct Cyc_Core_Opt* _temp1009; struct Cyc_List_List* _temp1011; void* _temp1013;
struct _tagged_string _temp1015; _LL991: if((( struct _enum_struct*) _temp989)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1006: _temp1005=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp989)->f1; goto _LL1004; _LL1004:
_temp1003=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp989)->f2; goto _LL1002; _LL1002: _temp1001=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp989)->f3; goto _LL992;} else{
goto _LL993;} _LL993: if((( struct _enum_struct*) _temp989)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1010: _temp1009=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp989)->f1; goto _LL1008; _LL1008: _temp1007=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp989)->f2; goto _LL994;} else{
goto _LL995;} _LL995: if((( struct _enum_struct*) _temp989)->tag == Cyc_Absyn_Array_e_tag){
_LL1012: _temp1011=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp989)->f1; goto _LL996;} else{ goto _LL997;} _LL997: if((( struct
_enum_struct*) _temp989)->tag == Cyc_Absyn_Const_e_tag){ _LL1014: _temp1013=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp989)->f1; if(( unsigned int)
_temp1013 > 1u?(( struct _enum_struct*) _temp1013)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1016: _temp1015=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1013)->f1; goto _LL998;} else{ goto _LL999;}} else{ goto _LL999;} _LL999:
goto _LL1000; _LL992: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te, topt, e1);
void* _temp1017= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp* _temp1023;
struct Cyc_Absyn_Tqual* _temp1025; void* _temp1027; _LL1019: if(( unsigned int)
_temp1017 > 4u?(( struct _enum_struct*) _temp1017)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1028: _temp1027=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1017)->f1;
goto _LL1026; _LL1026: _temp1025=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1017)->f2; goto _LL1024; _LL1024: _temp1023=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1017)->f3; goto _LL1020;} else{ goto
_LL1021;} _LL1021: goto _LL1022; _LL1020: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1003)?({ struct Cyc_Absyn_Upper_b_struct* _temp1029=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1029->tag= Cyc_Absyn_Upper_b_tag;
_temp1029->f1= _temp1003;( void*) _temp1029;}): Cyc_Absyn_Unknown_b; void*
res_typ=({ struct Cyc_Absyn_PointerType_struct* _temp1030=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1030->tag= Cyc_Absyn_PointerType_tag;
_temp1030->f1=({ struct Cyc_Absyn_PtrInfo _temp1031; _temp1031.elt_typ=( void*)
_temp1027; _temp1031.rgn_typ=( void*) Cyc_Absyn_HeapRgn; _temp1031.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1031.tq=
_temp1025; _temp1031.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp1031;});( void*) _temp1030;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*
topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1032=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1032->v=( void*) res_typ; _temp1032;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;} _LL1022: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1033=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1034; _temp1034.curr= _temp1033; _temp1034.base= _temp1033;
_temp1034.last_plus_one= _temp1033 + 45; _temp1034;})); _LL1018:;} _LL994:( void*)(
e1->r=( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1035=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1035->tag= Cyc_Absyn_Array_e_tag;
_temp1035->f1= _temp1007;( void*) _temp1035;})); _temp1011= _temp1007; goto
_LL996; _LL996: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1036= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp1042; struct Cyc_Absyn_Conref* _temp1044;
struct Cyc_Absyn_Tqual* _temp1046; struct Cyc_Absyn_Conref* _temp1048; void*
_temp1050; void* _temp1052; void** _temp1054; _LL1038: if(( unsigned int)
_temp1036 > 4u?(( struct _enum_struct*) _temp1036)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1043: _temp1042=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1036)->f1; _LL1053: _temp1052=( void*) _temp1042.elt_typ; _temp1054=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1036)->f1).elt_typ; goto _LL1051;
_LL1051: _temp1050=( void*) _temp1042.rgn_typ; goto _LL1049; _LL1049: _temp1048=(
struct Cyc_Absyn_Conref*) _temp1042.nullable; goto _LL1047; _LL1047: _temp1046=(
struct Cyc_Absyn_Tqual*) _temp1042.tq; goto _LL1045; _LL1045: _temp1044=( struct
Cyc_Absyn_Conref*) _temp1042.bounds; goto _LL1039;} else{ goto _LL1040;} _LL1040:
goto _LL1041; _LL1039: elt_typ_opt=( void**) _temp1054; goto _LL1037; _LL1041:
goto _LL1037; _LL1037:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1055= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1061; struct Cyc_Absyn_Tqual* _temp1063; void*
_temp1065; _LL1057: if(( unsigned int) _temp1055 > 4u?(( struct _enum_struct*)
_temp1055)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1066: _temp1065=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1055)->f1; goto _LL1064; _LL1064:
_temp1063=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1055)->f2; goto _LL1062; _LL1062: _temp1061=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1055)->f3; goto _LL1058;} else{ goto
_LL1059;} _LL1059: goto _LL1060; _LL1058: res_typ=({ struct Cyc_Absyn_PointerType_struct*
_temp1067=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1067->tag= Cyc_Absyn_PointerType_tag; _temp1067->f1=({ struct Cyc_Absyn_PtrInfo
_temp1068; _temp1068.elt_typ=( void*) _temp1065; _temp1068.rgn_typ=( void*) Cyc_Absyn_HeapRgn;
_temp1068.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1068.tq= _temp1063; _temp1068.bounds=(( struct Cyc_Absyn_Conref*(*)( void*
x)) Cyc_Absyn_new_conref)(({ struct Cyc_Absyn_Upper_b_struct* _temp1069=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1069->tag= Cyc_Absyn_Upper_b_tag; _temp1069->f1=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1070= _temp1061; if( _temp1070 == 0){ _throw(
Null_Exception);} _temp1070;});( void*) _temp1069;})); _temp1068;});( void*)
_temp1067;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1071=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1071->v=( void*)
res_typ; _temp1071;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}}
goto _LL1056; _LL1060: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1072=( char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1073; _temp1073.curr= _temp1072; _temp1073.base=
_temp1072; _temp1073.last_plus_one= _temp1072 + 50; _temp1073;})); _LL1056:;}
return res_typ;}} _LL998: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(), Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),({
struct Cyc_Absyn_Upper_b_struct* _temp1074=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1074->tag= Cyc_Absyn_Upper_b_tag;
_temp1074->f1= Cyc_Absyn_uint_exp( 1, 0);( void*) _temp1074;}));} _LL1000: {
void** topt2= 0; if( topt != 0){ void* _temp1075= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1081; struct Cyc_Absyn_Conref* _temp1083; struct
Cyc_Absyn_Tqual* _temp1085; struct Cyc_Absyn_Conref* _temp1087; void* _temp1089;
void* _temp1091; void** _temp1093; _LL1077: if(( unsigned int) _temp1075 > 4u?((
struct _enum_struct*) _temp1075)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1082:
_temp1081=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1075)->f1; _LL1092: _temp1091=( void*) _temp1081.elt_typ; _temp1093=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1075)->f1).elt_typ; goto _LL1090;
_LL1090: _temp1089=( void*) _temp1081.rgn_typ; goto _LL1088; _LL1088: _temp1087=(
struct Cyc_Absyn_Conref*) _temp1081.nullable; goto _LL1086; _LL1086: _temp1085=(
struct Cyc_Absyn_Tqual*) _temp1081.tq; goto _LL1084; _LL1084: _temp1083=( struct
Cyc_Absyn_Conref*) _temp1081.bounds; goto _LL1078;} else{ goto _LL1079;} _LL1079:
goto _LL1080; _LL1078: topt2=( void**) _temp1093; goto _LL1076; _LL1080: goto
_LL1076; _LL1076:;}{ void* telt= Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=({
struct Cyc_Absyn_PointerType_struct* _temp1094=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1094->tag= Cyc_Absyn_PointerType_tag;
_temp1094->f1=({ struct Cyc_Absyn_PtrInfo _temp1095; _temp1095.elt_typ=( void*)
telt; _temp1095.rgn_typ=( void*) Cyc_Absyn_HeapRgn; _temp1095.nullable=(( struct
Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1095.tq= Cyc_Absyn_empty_tqual();
_temp1095.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(({
struct Cyc_Absyn_Upper_b_struct* _temp1096=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1096->tag= Cyc_Absyn_Upper_b_tag;
_temp1096->f1= Cyc_Absyn_uint_exp( 1, 0);( void*) _temp1096;})); _temp1095;});(
void*) _temp1094;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(* topt, res_typ)?
Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1097=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1097->v=( void*) res_typ; _temp1097;}); Cyc_Tcutil_unchecked_cast( te, e,*
topt); res_typ=* topt;}} return res_typ;}} _LL990:;} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1098= t; struct Cyc_Absyn_Exp*
_temp1104; struct Cyc_Absyn_Tqual* _temp1106; void* _temp1108; _LL1100: if((
unsigned int) _temp1098 > 4u?(( struct _enum_struct*) _temp1098)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1109: _temp1108=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1098)->f1;
goto _LL1107; _LL1107: _temp1106=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1098)->f2; goto _LL1105; _LL1105: _temp1104=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1098)->f3; goto _LL1101;} else{ goto
_LL1102;} _LL1102: goto _LL1103; _LL1101: { void* _temp1112; int _temp1114;
struct _tuple5 _temp1110= Cyc_Tcutil_addressof_props( te, e); _LL1115: _temp1114=
_temp1110.f1; goto _LL1113; _LL1113: _temp1112= _temp1110.f2; goto _LL1111;
_LL1111: { void* b= _temp1104 == 0? Cyc_Absyn_Unknown_b:({ struct Cyc_Absyn_Upper_b_struct*
_temp1116=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1116->tag= Cyc_Absyn_Upper_b_tag; _temp1116->f1=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1117= _temp1104; if( _temp1117 == 0){ _throw(
Null_Exception);} _temp1117;});( void*) _temp1116;}); t= Cyc_Absyn_atb_typ(
_temp1108, _temp1112, _temp1106, b); Cyc_Tcutil_unchecked_cast( te, e, t);
return t;}} _LL1103: return t; _LL1099:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1118=( void*) e->r; struct Cyc_List_List* _temp1128;
struct Cyc_Absyn_Exp* _temp1130; struct Cyc_Absyn_Exp* _temp1132; struct Cyc_Absyn_Vardecl*
_temp1134; void* _temp1136; struct _tagged_string _temp1138; _LL1120: if(((
struct _enum_struct*) _temp1118)->tag == Cyc_Absyn_Array_e_tag){ _LL1129:
_temp1128=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1118)->f1;
goto _LL1121;} else{ goto _LL1122;} _LL1122: if((( struct _enum_struct*)
_temp1118)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1135: _temp1134=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1118)->f1;
goto _LL1133; _LL1133: _temp1132=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1118)->f2; goto _LL1131; _LL1131: _temp1130=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1118)->f3; goto _LL1123;} else{
goto _LL1124;} _LL1124: if((( struct _enum_struct*) _temp1118)->tag == Cyc_Absyn_Const_e_tag){
_LL1137: _temp1136=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1118)->f1;
if(( unsigned int) _temp1136 > 1u?(( struct _enum_struct*) _temp1136)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1139: _temp1138=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1136)->f1; goto _LL1125;} else{ goto _LL1126;}}
else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1121: return t; _LL1123: return t;
_LL1125: return t; _LL1127: t= Cyc_Tcutil_compress( t);{ void* _temp1140= t;
struct Cyc_Absyn_Exp* _temp1146; struct Cyc_Absyn_Tqual* _temp1148; void*
_temp1150; _LL1142: if(( unsigned int) _temp1140 > 4u?(( struct _enum_struct*)
_temp1140)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1151: _temp1150=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1140)->f1; goto _LL1149; _LL1149:
_temp1148=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1140)->f2; goto _LL1147; _LL1147: _temp1146=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1140)->f3; goto _LL1143;} else{ goto
_LL1144;} _LL1144: goto _LL1145; _LL1143: { void* _temp1154; int _temp1156;
struct _tuple5 _temp1152= Cyc_Tcutil_addressof_props( te, e); _LL1157: _temp1156=
_temp1152.f1; goto _LL1155; _LL1155: _temp1154= _temp1152.f2; goto _LL1153;
_LL1153: { void* b= _temp1146 == 0? Cyc_Absyn_Unknown_b:({ struct Cyc_Absyn_Upper_b_struct*
_temp1158=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1158->tag= Cyc_Absyn_Upper_b_tag; _temp1158->f1=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1159= _temp1146; if( _temp1159 == 0){ _throw(
Null_Exception);} _temp1159;});( void*) _temp1158;}); t= Cyc_Absyn_atb_typ(
_temp1150, _temp1154, _temp1148, b); Cyc_Tcutil_unchecked_cast( te, e, t);
return t;}} _LL1145: return t; _LL1141:;} _LL1119:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1160=( void*) e->r; struct Cyc_Absyn_Exp* _temp1166; _LL1162: if((( struct
_enum_struct*) _temp1160)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1167:
_temp1166=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1160)->f1; goto _LL1163;} else{ goto _LL1164;} _LL1164: goto _LL1165;
_LL1163: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1166); e->topt= _temp1166->topt;
goto _LL1161; _LL1165: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)(( e->topt)->v=(
void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)( e->topt)->v)));{
void* _temp1168= Cyc_Tcutil_compress(( void*)( e->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1174; struct Cyc_Absyn_Conref* _temp1176; struct Cyc_Absyn_Tqual* _temp1178;
struct Cyc_Absyn_Conref* _temp1180; void* _temp1182; void* _temp1184; _LL1170:
if(( unsigned int) _temp1168 > 4u?(( struct _enum_struct*) _temp1168)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1175: _temp1174=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1168)->f1; _LL1185: _temp1184=( void*)
_temp1174.elt_typ; goto _LL1183; _LL1183: _temp1182=( void*) _temp1174.rgn_typ;
goto _LL1181; _LL1181: _temp1180=( struct Cyc_Absyn_Conref*) _temp1174.nullable;
goto _LL1179; _LL1179: _temp1178=( struct Cyc_Absyn_Tqual*) _temp1174.tq; goto
_LL1177; _LL1177: _temp1176=( struct Cyc_Absyn_Conref*) _temp1174.bounds; goto
_LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173; _LL1171:{ void* _temp1186=
Cyc_Tcutil_compress( _temp1184); struct Cyc_Absyn_FnInfo _temp1192; struct Cyc_List_List*
_temp1194; int _temp1196; struct Cyc_List_List* _temp1198; void* _temp1200;
struct Cyc_Core_Opt* _temp1202; struct Cyc_List_List* _temp1204; _LL1188: if((
unsigned int) _temp1186 > 4u?(( struct _enum_struct*) _temp1186)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1193: _temp1192=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1186)->f1; _LL1205: _temp1204=( struct Cyc_List_List*) _temp1192.tvars;
goto _LL1203; _LL1203: _temp1202=( struct Cyc_Core_Opt*) _temp1192.effect; goto
_LL1201; _LL1201: _temp1200=( void*) _temp1192.ret_typ; goto _LL1199; _LL1199:
_temp1198=( struct Cyc_List_List*) _temp1192.args; goto _LL1197; _LL1197:
_temp1196=( int) _temp1192.varargs; goto _LL1195; _LL1195: _temp1194=( struct
Cyc_List_List*) _temp1192.attributes; goto _LL1189;} else{ goto _LL1190;}
_LL1190: goto _LL1191; _LL1189: if( _temp1204 != 0){ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1204); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,({ struct Cyc_Absyn_FnType_struct*
_temp1206=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1206->tag= Cyc_Absyn_FnType_tag; _temp1206->f1=({ struct Cyc_Absyn_FnInfo
_temp1207; _temp1207.tvars= 0; _temp1207.effect= _temp1202; _temp1207.ret_typ=(
void*) _temp1200; _temp1207.args= _temp1198; _temp1207.varargs= _temp1196;
_temp1207.attributes= _temp1194; _temp1207;});( void*) _temp1206;})); void*
new_typ=({ struct Cyc_Absyn_PointerType_struct* _temp1208=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1208->tag= Cyc_Absyn_PointerType_tag;
_temp1208->f1=({ struct Cyc_Absyn_PtrInfo _temp1209; _temp1209.elt_typ=( void*)
ftyp; _temp1209.rgn_typ=( void*) _temp1182; _temp1209.nullable= _temp1180;
_temp1209.tq= _temp1178; _temp1209.bounds= _temp1176; _temp1209;});( void*)
_temp1208;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1210=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1210->tag= Cyc_Absyn_Instantiate_e_tag;
_temp1210->f1= inner; _temp1210->f2= ts;( void*) _temp1210;})); e->topt=({
struct Cyc_Core_Opt* _temp1211=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1211->v=( void*) new_typ; _temp1211;});} goto _LL1187;
_LL1191: goto _LL1187; _LL1187:;} goto _LL1169; _LL1173: goto _LL1169; _LL1169:;}
goto _LL1161; _LL1161:;} return( void*)( e->topt)->v;} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1212=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1282; struct _tuple1* _temp1284; struct Cyc_List_List* _temp1286; struct
Cyc_Absyn_Exp* _temp1288; struct Cyc_List_List* _temp1290; struct Cyc_Core_Opt*
_temp1292; void* _temp1294; void* _temp1296; struct _tuple1* _temp1298; struct
Cyc_List_List* _temp1300; void* _temp1302; void* _temp1304; struct Cyc_Absyn_Exp*
_temp1306; struct Cyc_Absyn_Exp* _temp1308; struct Cyc_Core_Opt* _temp1310;
struct Cyc_Absyn_Exp* _temp1312; struct Cyc_Absyn_Exp* _temp1314; struct Cyc_Absyn_Exp*
_temp1316; struct Cyc_Absyn_Exp* _temp1318; struct Cyc_Absyn_Exp* _temp1320;
struct Cyc_Absyn_Exp* _temp1322; struct Cyc_List_List* _temp1324; struct Cyc_Absyn_Exp*
_temp1326; struct Cyc_Absyn_Exp* _temp1328; struct Cyc_List_List* _temp1330;
struct Cyc_Absyn_Exp* _temp1332; struct Cyc_Absyn_Exp* _temp1334; void*
_temp1336; struct Cyc_Absyn_Exp* _temp1338; struct Cyc_Absyn_Exp* _temp1340;
struct Cyc_Absyn_Exp* _temp1342; void* _temp1344; struct Cyc_Absyn_Exp*
_temp1346; struct _tagged_string* _temp1348; struct Cyc_Absyn_Exp* _temp1350;
struct _tagged_string* _temp1352; struct Cyc_Absyn_Exp* _temp1354; struct Cyc_Absyn_Exp*
_temp1356; struct Cyc_Absyn_Exp* _temp1358; struct Cyc_List_List* _temp1360;
struct Cyc_List_List* _temp1362; struct _tuple2* _temp1364; struct Cyc_List_List*
_temp1366; struct Cyc_Absyn_Stmt* _temp1368; struct Cyc_Absyn_Fndecl* _temp1370;
struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp* _temp1374; struct Cyc_Absyn_Exp*
_temp1376; struct Cyc_Absyn_Vardecl* _temp1378; struct Cyc_Absyn_Structdecl*
_temp1380; struct Cyc_Absyn_Structdecl** _temp1382; struct Cyc_List_List*
_temp1383; struct Cyc_Core_Opt* _temp1385; struct Cyc_Core_Opt** _temp1387;
struct _tuple1* _temp1388; struct _tuple1** _temp1390; struct Cyc_Absyn_Enumfield*
_temp1391; struct Cyc_Absyn_Enumdecl* _temp1393; struct Cyc_List_List* _temp1395;
struct Cyc_Core_Opt* _temp1397; struct Cyc_Core_Opt** _temp1399; struct Cyc_Core_Opt*
_temp1400; struct Cyc_Core_Opt** _temp1402; struct Cyc_Absyn_Enumfield*
_temp1403; struct Cyc_Absyn_Xenumdecl* _temp1405; struct Cyc_List_List*
_temp1407; struct Cyc_Core_Opt* _temp1409; struct Cyc_Core_Opt** _temp1411; void*
_temp1412; void** _temp1414; _LL1214: if((( struct _enum_struct*) _temp1212)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL1283: _temp1282=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1212)->f1; goto _LL1215;} else{
goto _LL1216;} _LL1216: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL1285: _temp1284=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1212)->f1; goto _LL1217;} else{ goto _LL1218;} _LL1218: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1289:
_temp1288=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1212)->f1; goto _LL1287; _LL1287: _temp1286=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1212)->f2; goto _LL1219;} else{
goto _LL1220;} _LL1220: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1293: _temp1292=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1212)->f1; goto _LL1291; _LL1291: _temp1290=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1212)->f2; goto _LL1221;} else{
goto _LL1222;} _LL1222: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Const_e_tag){
_LL1295: _temp1294=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1212)->f1;
goto _LL1223;} else{ goto _LL1224;} _LL1224: if((( struct _enum_struct*)
_temp1212)->tag == Cyc_Absyn_Var_e_tag){ _LL1299: _temp1298=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1212)->f1; goto _LL1297; _LL1297: _temp1296=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1212)->f2; goto _LL1225;} else{
goto _LL1226;} _LL1226: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Primop_e_tag){
_LL1303: _temp1302=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1212)->f1;
goto _LL1301; _LL1301: _temp1300=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1212)->f2; goto _LL1227;} else{ goto _LL1228;} _LL1228: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Increment_e_tag){ _LL1307: _temp1306=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1212)->f1;
goto _LL1305; _LL1305: _temp1304=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1212)->f2; goto _LL1229;} else{ goto _LL1230;} _LL1230: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1313: _temp1312=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1212)->f1;
goto _LL1311; _LL1311: _temp1310=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1212)->f2; goto _LL1309; _LL1309: _temp1308=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1212)->f3; goto _LL1231;} else{ goto
_LL1232;} _LL1232: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1319: _temp1318=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1212)->f1; goto _LL1317; _LL1317: _temp1316=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1212)->f2; goto _LL1315; _LL1315:
_temp1314=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1212)->f3; goto _LL1233;} else{ goto _LL1234;} _LL1234: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1323: _temp1322=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1212)->f1; goto
_LL1321; _LL1321: _temp1320=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1212)->f2; goto _LL1235;} else{ goto _LL1236;} _LL1236: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1327: _temp1326=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1212)->f1; goto
_LL1325; _LL1325: _temp1324=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1212)->f2; goto _LL1237;} else{ goto _LL1238;} _LL1238: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Throw_e_tag){ _LL1329: _temp1328=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1212)->f1; goto
_LL1239;} else{ goto _LL1240;} _LL1240: if((( struct _enum_struct*) _temp1212)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1333: _temp1332=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1212)->f1; goto _LL1331; _LL1331:
_temp1330=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1212)->f2; goto _LL1241;} else{ goto _LL1242;} _LL1242: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Cast_e_tag){ _LL1337: _temp1336=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1212)->f1; goto _LL1335; _LL1335:
_temp1334=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1212)->f2;
goto _LL1243;} else{ goto _LL1244;} _LL1244: if((( struct _enum_struct*)
_temp1212)->tag == Cyc_Absyn_Address_e_tag){ _LL1339: _temp1338=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1212)->f1; goto _LL1245;} else{ goto
_LL1246;} _LL1246: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_New_e_tag){
_LL1341: _temp1340=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1212)->f1; goto _LL1247;} else{ goto _LL1248;} _LL1248: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1343: _temp1342=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1212)->f1;
goto _LL1249;} else{ goto _LL1250;} _LL1250: if((( struct _enum_struct*)
_temp1212)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1345: _temp1344=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1212)->f1; goto _LL1251;} else{ goto
_LL1252;} _LL1252: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Deref_e_tag){
_LL1347: _temp1346=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1212)->f1; goto _LL1253;} else{ goto _LL1254;} _LL1254: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1351:
_temp1350=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1212)->f1; goto _LL1349; _LL1349: _temp1348=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1212)->f2; goto _LL1255;} else{
goto _LL1256;} _LL1256: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1355: _temp1354=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1212)->f1; goto _LL1353; _LL1353: _temp1352=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1212)->f2; goto _LL1257;} else{
goto _LL1258;} _LL1258: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1359: _temp1358=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1212)->f1; goto _LL1357; _LL1357: _temp1356=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1212)->f2; goto _LL1259;} else{ goto
_LL1260;} _LL1260: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1361: _temp1360=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1212)->f1; goto _LL1261;} else{ goto _LL1262;} _LL1262: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1365:
_temp1364=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1212)->f1;
goto _LL1363; _LL1363: _temp1362=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1212)->f2; goto _LL1263;} else{ goto _LL1264;} _LL1264: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Array_e_tag){ _LL1367: _temp1366=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1212)->f1; goto
_LL1265;} else{ goto _LL1266;} _LL1266: if((( struct _enum_struct*) _temp1212)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL1369: _temp1368=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1212)->f1; goto _LL1267;} else{ goto
_LL1268;} _LL1268: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1371: _temp1370=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1212)->f1; goto _LL1269;} else{ goto _LL1270;} _LL1270: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Fill_e_tag){ _LL1373: _temp1372=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1212)->f1; goto
_LL1271;} else{ goto _LL1272;} _LL1272: if((( struct _enum_struct*) _temp1212)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1379: _temp1378=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1212)->f1; goto _LL1377; _LL1377:
_temp1376=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1212)->f2; goto _LL1375; _LL1375: _temp1374=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1212)->f3; goto _LL1273;} else{
goto _LL1274;} _LL1274: if((( struct _enum_struct*) _temp1212)->tag == Cyc_Absyn_Struct_e_tag){
_LL1389: _temp1388=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1212)->f1; _temp1390=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1212)->f1;
goto _LL1386; _LL1386: _temp1385=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1212)->f2; _temp1387=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1212)->f2;
goto _LL1384; _LL1384: _temp1383=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1212)->f3; goto _LL1381; _LL1381: _temp1380=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1212)->f4; _temp1382=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1212)->f4; goto _LL1275;} else{ goto _LL1276;} _LL1276: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Enum_e_tag){ _LL1401: _temp1400=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1212)->f1;
_temp1402=&(( struct Cyc_Absyn_Enum_e_struct*) _temp1212)->f1; goto _LL1398;
_LL1398: _temp1397=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1212)->f2; _temp1399=&(( struct Cyc_Absyn_Enum_e_struct*) _temp1212)->f2;
goto _LL1396; _LL1396: _temp1395=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1212)->f3; goto _LL1394; _LL1394: _temp1393=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1212)->f4; goto _LL1392; _LL1392:
_temp1391=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1212)->f5; goto _LL1277;} else{ goto _LL1278;} _LL1278: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1410: _temp1409=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp1212)->f1;
_temp1411=&(( struct Cyc_Absyn_Xenum_e_struct*) _temp1212)->f1; goto _LL1408;
_LL1408: _temp1407=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1212)->f2; goto _LL1406; _LL1406: _temp1405=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp1212)->f3; goto _LL1404; _LL1404:
_temp1403=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1212)->f4; goto _LL1279;} else{ goto _LL1280;} _LL1280: if((( struct
_enum_struct*) _temp1212)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1413: _temp1412=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1212)->f1; _temp1414=&(( struct
Cyc_Absyn_Malloc_e_struct*) _temp1212)->f1; goto _LL1281;} else{ goto _LL1213;}
_LL1215: Cyc_Tcexp_tcExpNoInst( te, 0, _temp1282); return; _LL1217: Cyc_Tcexp_resolve_unknown_id(
te, e, _temp1284); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1219: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1288, _temp1286); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1221: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1290); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1223: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1294, e);
goto _LL1213; _LL1225: t= Cyc_Tcexp_tcVar( te, loc, _temp1298, _temp1296); goto
_LL1213; _LL1227: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1302, _temp1300);
goto _LL1213; _LL1229: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1306,
_temp1304); goto _LL1213; _LL1231: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1312, _temp1310, _temp1308); goto _LL1213; _LL1233: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1318, _temp1316, _temp1314); goto _LL1213; _LL1235: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1322, _temp1320); goto _LL1213; _LL1237: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1326, _temp1324); goto _LL1213; _LL1239: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1328); goto _LL1213; _LL1241: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1332, _temp1330); goto _LL1213; _LL1243: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1336, _temp1334); goto _LL1213; _LL1245: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1338); goto _LL1213; _LL1247: t= Cyc_Tcexp_tcNew( te, loc,
topt, e, _temp1340); goto _LL1213; _LL1249: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1342); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1213;}
_LL1251: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1344); goto _LL1213; _LL1253:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1346); goto _LL1213; _LL1255: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1350, _temp1348); goto _LL1213; _LL1257: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1354, _temp1352); goto _LL1213; _LL1259: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1358, _temp1356); goto _LL1213; _LL1261: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1360); goto _LL1213; _LL1263: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1364, _temp1362); goto _LL1213; _LL1265: { void** elt_topt= 0;
if( topt != 0){ void* _temp1415= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp*
_temp1421; struct Cyc_Absyn_Tqual* _temp1423; void* _temp1425; void** _temp1427;
_LL1417: if(( unsigned int) _temp1415 > 4u?(( struct _enum_struct*) _temp1415)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL1426: _temp1425=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1415)->f1; _temp1427=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1415)->f1;
goto _LL1424; _LL1424: _temp1423=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1415)->f2; goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1415)->f3; goto _LL1418;} else{ goto
_LL1419;} _LL1419: goto _LL1420; _LL1418: elt_topt=( void**) _temp1427; goto
_LL1416; _LL1420: goto _LL1416; _LL1416:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1366); goto _LL1213;} _LL1267: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1368); goto _LL1213; _LL1269: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1370); goto _LL1213; _LL1271: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1372);
goto _LL1213; _LL1273: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1378,
_temp1376, _temp1374); goto _LL1213; _LL1275: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1390, _temp1387, _temp1383, _temp1382); goto _LL1213; _LL1277: t= Cyc_Tcexp_tcEnum(
te, loc, topt, _temp1402, _temp1399, _temp1395, _temp1393, _temp1391); goto
_LL1213; _LL1279: t= Cyc_Tcexp_tcXenum( te, loc, topt, _temp1411, _temp1407,
_temp1405, _temp1403); goto _LL1213; _LL1281: t= Cyc_Tcexp_tcMalloc( te, loc,
topt, _temp1414); goto _LL1213; _LL1213:;} e->topt=({ struct Cyc_Core_Opt*
_temp1428=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1428->v=( void*) t; _temp1428;});}