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
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl;
typedef void* Cyc_Absyn_Designator; typedef struct _xenum_struct* Cyc_Absyn_StmtAnnot;
typedef void* Cyc_Absyn_Attribute; struct Cyc_Absyn_Structfield; typedef void*
Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual*
Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t;
typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t;
typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t;
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
Cyc_Absyn_stmt_annot_t; typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List*
Cyc_Absyn_attributes_t; typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t;
typedef void* Cyc_Absyn_mallocarg_t; extern void* Cyc_Absyn_Loc_n; extern const
int Cyc_Absyn_Rel_n_tag; struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List*
f1; } ; extern const int Cyc_Absyn_Abs_n_tag; struct Cyc_Absyn_Abs_n_struct{ int
tag; struct Cyc_List_List* f1; } ; extern void* Cyc_Absyn_Static; extern void*
Cyc_Absyn_Abstract; extern void* Cyc_Absyn_Public; extern void* Cyc_Absyn_Extern;
extern void* Cyc_Absyn_ExternC; struct Cyc_Absyn_Tqual{ int q_const; int
q_volatile; int q_restrict; } ; extern void* Cyc_Absyn_B1; extern void* Cyc_Absyn_B2;
extern void* Cyc_Absyn_B4; extern void* Cyc_Absyn_B8; extern void* Cyc_Absyn_AnyKind;
extern void* Cyc_Absyn_MemKind; extern void* Cyc_Absyn_BoxKind; extern void* Cyc_Absyn_RgnKind;
extern void* Cyc_Absyn_EffKind; extern void* Cyc_Absyn_Signed; extern void* Cyc_Absyn_Unsigned;
struct Cyc_Absyn_Conref{ void* v; } ; extern const int Cyc_Absyn_Eq_constr_tag;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_Forward_constr_tag;
struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1; }
; extern void* Cyc_Absyn_No_constr; struct Cyc_Absyn_Tvar{ struct _tagged_string*
name; struct Cyc_Absyn_Conref* kind; } ; extern void* Cyc_Absyn_Unknown_b;
extern const int Cyc_Absyn_Upper_b_tag; struct Cyc_Absyn_Upper_b_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual* tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int varargs; struct Cyc_List_List* attributes; } ; extern void* Cyc_Absyn_VoidType;
extern const int Cyc_Absyn_Evar_tag; struct Cyc_Absyn_Evar_struct{ int tag; void*
f1; struct Cyc_Core_Opt* f2; int f3; } ; extern const int Cyc_Absyn_VarType_tag;
struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ; extern
const int Cyc_Absyn_EnumType_tag; struct Cyc_Absyn_EnumType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl** f3; }
; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Xenumdecl** f2; } ; extern const
int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag;
struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag; struct
Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void* Cyc_Absyn_FloatType;
extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; extern const int
Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ; extern void*
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
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Array_e_tag;
struct Cyc_Absyn_Array_e_struct{ int tag; int f1; struct Cyc_List_List* f2; } ;
extern const int Cyc_Absyn_Comprehension_e_tag; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_Struct_e_tag; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; extern const int Cyc_Absyn_Enum_e_tag;
struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_Xenum_e_tag; struct Cyc_Absyn_Xenum_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Xenumdecl*
f3; struct Cyc_Absyn_Enumfield* f4; } ; extern const int Cyc_Absyn_Malloc_e_tag;
struct Cyc_Absyn_Malloc_e_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag;
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
Cyc_Absyn_Structdecl* f1; } ; extern void* Cyc_Absyn_Union_d; extern const int
Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl*
f1; } ; extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
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
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
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
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* enumdecls;
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
te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); extern int Cyc_Tcexp_is_const_exp(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); typedef void* Cyc_CfFlowInfo_LocalRoot;
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
_temp89->f1= 0; _temp89->f2= des;( void*) _temp89;})); return;}{ void* t=* topt;
void* _temp90= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp98; struct
Cyc_Absyn_Tqual* _temp100; void* _temp102; struct Cyc_Absyn_Structdecl**
_temp104; struct Cyc_List_List* _temp106; struct _tuple1* _temp108; _LL92: if((
unsigned int) _temp90 > 5u?(( struct _enum_struct*) _temp90)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL103: _temp102=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp90)->f1;
goto _LL101; _LL101: _temp100=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp90)->f2; goto _LL99; _LL99: _temp98=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp90)->f3; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp90
> 5u?(( struct _enum_struct*) _temp90)->tag == Cyc_Absyn_StructType_tag: 0){
_LL109: _temp108=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp90)->f1; goto _LL107; _LL107: _temp106=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp90)->f2; goto _LL105; _LL105: _temp104=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp90)->f3;
goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL93:( void*)( e->r=( void*)({
struct Cyc_Absyn_Array_e_struct* _temp110=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp110->tag= Cyc_Absyn_Array_e_tag;
_temp110->f1= 0; _temp110->f2= des;( void*) _temp110;})); goto _LL91; _LL95: if(
_temp104 == 0? 1: _temp108 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp111=( char*)"Compiler bug: struct type not properly set"; struct
_tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 43; _temp112;}));}( void*)( e->r=( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp113=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp113->tag= Cyc_Absyn_Struct_e_tag;
_temp113->f1=( struct _tuple1*)({ struct _tuple1* _temp114= _temp108; if(
_temp114 == 0){ _throw( Null_Exception);} _temp114;}); _temp113->f2= 0; _temp113->f3=
des; _temp113->f4=( struct Cyc_Absyn_Structdecl*)* _temp104;( void*) _temp113;}));
goto _LL91; _LL97:( void*)( e->r=( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp115=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp115->tag= Cyc_Absyn_Array_e_tag; _temp115->f1= 0; _temp115->f2= des;( void*)
_temp115;})); goto _LL91; _LL91:;}} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void
Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* es){ for(
0; es != 0; es= es->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*) es->hd);}}
void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e,
struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp(
te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){
Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp116= msg_part; struct
_tagged_string _temp117= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp116.last_plus_one - _temp116.curr, _temp116.curr, _temp117.last_plus_one -
_temp117.curr, _temp117.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp118= c; char _temp134; void* _temp136; short _temp138;
void* _temp140; long long _temp142; void* _temp144; struct _tagged_string
_temp146; int _temp148; void* _temp150; struct _tagged_string _temp152; int
_temp154; _LL120: if(( unsigned int) _temp118 > 1u?(( struct _enum_struct*)
_temp118)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL137: _temp136=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp118)->f1; goto _LL135; _LL135: _temp134=( char)((
struct Cyc_Absyn_Char_c_struct*) _temp118)->f2; goto _LL121;} else{ goto _LL122;}
_LL122: if(( unsigned int) _temp118 > 1u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_Short_c_tag: 0){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp118)->f1; goto _LL139; _LL139: _temp138=( short)(( struct Cyc_Absyn_Short_c_struct*)
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
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_String_c_tag: 0){ _LL155:
_temp154=( int)(( struct Cyc_Absyn_String_c_struct*) _temp118)->f1; goto _LL153;
_LL153: _temp152=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp118)->f2; goto _LL131;} else{ goto _LL132;} _LL132: if( _temp118 == Cyc_Absyn_Null_c){
goto _LL133;} else{ goto _LL119;} _LL121: t= _temp136 == Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL119; _LL123: t= _temp140 == Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t; goto _LL119; _LL125: t= _temp144 == Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL119; _LL127: t= Cyc_Absyn_float_t; goto _LL119;
_LL129: if( topt == 0){ t= _temp150 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp156=* topt; void* _temp166; void* _temp168; void* _temp170;
void* _temp172; void* _temp174; void* _temp176; _LL158: if(( unsigned int)
_temp156 > 5u?(( struct _enum_struct*) _temp156)->tag == Cyc_Absyn_IntType_tag:
0){ _LL169: _temp168=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp156)->f1;
goto _LL167; _LL167: _temp166=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp156)->f2; if( _temp166 == Cyc_Absyn_B1){ goto _LL159;} else{ goto _LL160;}}
else{ goto _LL160;} _LL160: if(( unsigned int) _temp156 > 5u?(( struct
_enum_struct*) _temp156)->tag == Cyc_Absyn_IntType_tag: 0){ _LL173: _temp172=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp156)->f1; goto _LL171; _LL171:
_temp170=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp156)->f2; if(
_temp170 == Cyc_Absyn_B2){ goto _LL161;} else{ goto _LL162;}} else{ goto _LL162;}
_LL162: if(( unsigned int) _temp156 > 5u?(( struct _enum_struct*) _temp156)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL177: _temp176=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp156)->f1; goto _LL175; _LL175: _temp174=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp156)->f2; goto _LL163;} else{ goto _LL164;} _LL164: goto _LL165; _LL159: t=
_temp168 == Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;( void*)( e->r=(
void*)({ struct Cyc_Absyn_Const_e_struct* _temp178=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp178->tag= Cyc_Absyn_Const_e_tag;
_temp178->f1=( void*)({ struct Cyc_Absyn_Char_c_struct* _temp179=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp179->tag= Cyc_Absyn_Char_c_tag;
_temp179->f1=( void*) _temp168; _temp179->f2=( char) _temp148;( void*) _temp179;});(
void*) _temp178;})); goto _LL157; _LL161: t= _temp172 == Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t;( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp180=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp180->tag= Cyc_Absyn_Const_e_tag; _temp180->f1=( void*)({ struct Cyc_Absyn_Short_c_struct*
_temp181=( struct Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp181->tag= Cyc_Absyn_Short_c_tag; _temp181->f1=( void*) _temp172; _temp181->f2=(
short) _temp148;( void*) _temp181;});( void*) _temp180;})); goto _LL157; _LL163:
t= _temp176 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto
_LL157; _LL165: t= _temp150 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL157; _LL157:;} goto _LL119; _LL131: { int len=( int)({ struct
_tagged_string _temp182= _temp152;( unsigned int)( _temp182.last_plus_one -
_temp182.curr);}); void* r=({ struct Cyc_Absyn_Const_e_struct* _temp183=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp183->tag= Cyc_Absyn_Const_e_tag; _temp183->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp184=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp184->tag= Cyc_Absyn_Int_c_tag; _temp184->f1=( void*) Cyc_Absyn_Unsigned;
_temp184->f2= len;( void*) _temp184;});( void*) _temp183;}); struct Cyc_Absyn_Exp*
elen= Cyc_Absyn_new_exp( r, loc); elen->topt=({ struct Cyc_Core_Opt* _temp185=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp185->v=(
void*) Cyc_Absyn_uint_t; _temp185;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_Upper_b_struct*
_temp186=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp186->tag= Cyc_Absyn_Upper_b_tag; _temp186->f1= elen;( void*) _temp186;}));
if( topt != 0){ void* _temp187= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp*
_temp195; struct Cyc_Absyn_Tqual* _temp197; void* _temp199; struct Cyc_Absyn_PtrInfo
_temp201; struct Cyc_Absyn_Conref* _temp203; struct Cyc_Absyn_Tqual* _temp205;
struct Cyc_Absyn_Conref* _temp207; void* _temp209; void* _temp211; _LL189: if((
unsigned int) _temp187 > 5u?(( struct _enum_struct*) _temp187)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp187)->f1;
goto _LL198; _LL198: _temp197=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp187)->f2; goto _LL196; _LL196: _temp195=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp187)->f3; goto _LL190;} else{ goto _LL191;}
_LL191: if(( unsigned int) _temp187 > 5u?(( struct _enum_struct*) _temp187)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL202: _temp201=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp187)->f1; _LL212: _temp211=( void*)
_temp201.elt_typ; goto _LL210; _LL210: _temp209=( void*) _temp201.rgn_typ; goto
_LL208; _LL208: _temp207=( struct Cyc_Absyn_Conref*) _temp201.nullable; goto
_LL206; _LL206: _temp205=( struct Cyc_Absyn_Tqual*) _temp201.tq; goto _LL204;
_LL204: _temp203=( struct Cyc_Absyn_Conref*) _temp201.bounds; goto _LL192;}
else{ goto _LL193;} _LL193: goto _LL194; _LL190: if( _temp154){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp213=( char*)"string is dynamically allocated -- returns char pointer";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 56; _temp214;}));}{ unsigned int len2= Cyc_Evexp_eval_const_uint_exp(
_temp195); if( ! Cyc_Tcutil_unify( _temp199, Cyc_Absyn_uchar_t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp215= Cyc_Absynpp_typ2string( _temp199);
xprintf("string constant cannot be used as %.*s array", _temp215.last_plus_one -
_temp215.curr, _temp215.curr);}));} if(( unsigned int) len != len2){ Cyc_Tcutil_terr(
loc, xprintf("string has length %d instead of %d", len,( int) len2));} return*
topt;} _LL192: if( ! Cyc_Tcutil_unify( _temp211, Cyc_Absyn_uchar_t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp216= Cyc_Absynpp_typ2string( _temp211);
xprintf("string cannot be used as %.*s pointer", _temp216.last_plus_one -
_temp216.curr, _temp216.curr);}));} if( ! Cyc_Tcutil_unify( _temp209, Cyc_Absyn_HeapRgn)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp217=( char*)"this string literal is heap allocated";
struct _tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 38; _temp218;}));} _temp203=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp203);{ void*
_temp219=( void*) _temp203->v; void* _temp227; struct Cyc_Absyn_Exp* _temp229;
void* _temp231; _LL221: if(( unsigned int) _temp219 > 1u?(( struct _enum_struct*)
_temp219)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL228: _temp227=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp219)->f1; if(( unsigned int) _temp227 >
1u?(( struct _enum_struct*) _temp227)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL230:
_temp229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp227)->f1;
goto _LL222;} else{ goto _LL223;}} else{ goto _LL223;} _LL223: if(( unsigned int)
_temp219 > 1u?(( struct _enum_struct*) _temp219)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL232: _temp231=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp219)->f1;
if( _temp231 == Cyc_Absyn_Unknown_b){ goto _LL224;} else{ goto _LL225;}} else{
goto _LL225;} _LL225: goto _LL226; _LL222: { unsigned int len2= Cyc_Evexp_eval_const_uint_exp(
_temp229); if(( unsigned int) len != len2){ Cyc_Tcutil_terr( loc, xprintf("string has length %d instead of %d",
len,( int) len2));} goto _LL220;} _LL224: e->topt=({ struct Cyc_Core_Opt*
_temp233=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp233->v=( void*) t; _temp233;}); Cyc_Tcutil_unchecked_cast( te, e,* topt);
goto _LL220; _LL226:( void*)( _temp203->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp234=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp234->tag= Cyc_Absyn_Eq_constr_tag; _temp234->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp235=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp235->tag= Cyc_Absyn_Upper_b_tag; _temp235->f1= elen;( void*) _temp235;});(
void*) _temp234;})); goto _LL220; _LL220:;} return* topt; _LL194: goto _LL188;
_LL188:;} goto _LL119;} _LL133: t=({ struct Cyc_Absyn_PointerType_struct*
_temp236=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp236->tag= Cyc_Absyn_PointerType_tag; _temp236->f1=({ struct Cyc_Absyn_PtrInfo
_temp237; _temp237.elt_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
_temp237.rgn_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind); _temp237.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 1); _temp237.tq= Cyc_Absyn_empty_tqual();
_temp237.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp237;});( void*) _temp236;}); goto _LL119; _LL119:;} return t;} static void*
Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tuple1* q, void* b){ void* _temp238= b; struct Cyc_Absyn_Vardecl*
_temp252; struct Cyc_Absyn_Fndecl* _temp254; struct Cyc_Absyn_Vardecl* _temp256;
struct Cyc_Absyn_Vardecl* _temp258; struct Cyc_Absyn_Vardecl* _temp260; _LL240:
if( _temp238 == Cyc_Absyn_Unresolved_b){ goto _LL241;} else{ goto _LL242;}
_LL242: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*) _temp238)->tag
== Cyc_Absyn_Global_b_tag: 0){ _LL253: _temp252=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp238)->f1; goto _LL243;} else{ goto
_LL244;} _LL244: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*)
_temp238)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL255: _temp254=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp238)->f1; goto _LL245;} else{ goto
_LL246;} _LL246: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*)
_temp238)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL257: _temp256=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp238)->f1; goto _LL247;} else{ goto _LL248;}
_LL248: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*) _temp238)->tag
== Cyc_Absyn_Local_b_tag: 0){ _LL259: _temp258=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp238)->f1; goto _LL249;} else{ goto _LL250;}
_LL250: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*) _temp238)->tag
== Cyc_Absyn_Param_b_tag: 0){ _LL261: _temp260=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp238)->f1; goto _LL251;} else{ goto _LL239;}
_LL241: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp262=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s", _temp262.last_plus_one
- _temp262.curr, _temp262.curr);})); _LL243:({ struct _tuple1* _temp263= q;
unsigned int _temp264= 0; if( _temp264 >= 1u){ _throw( Null_Exception);}
_temp263[ _temp264]=({ struct _tuple1* _temp265= _temp252->name; unsigned int
_temp266= 0; if( _temp266 >= 1u){ _throw( Null_Exception);} _temp265[ _temp266];});});
return( void*) _temp252->type; _LL245:({ struct _tuple1* _temp267= q;
unsigned int _temp268= 0; if( _temp268 >= 1u){ _throw( Null_Exception);}
_temp267[ _temp268]=({ struct _tuple1* _temp269= _temp254->name; unsigned int
_temp270= 0; if( _temp270 >= 1u){ _throw( Null_Exception);} _temp269[ _temp270];});});
return Cyc_Tcutil_fndecl2typ( _temp254); _LL247: _temp258= _temp256; goto _LL249;
_LL249: _temp260= _temp258; goto _LL251; _LL251:(* q).f1= Cyc_Absyn_Loc_n;
return( void*) _temp260->type; _LL239:;} static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* fmt, struct Cyc_List_List* args,
struct Cyc_List_List*(* type_getter)( struct _tagged_string, struct Cyc_Position_Segment*)){
struct Cyc_List_List* desc_types;{ void* _temp271=( void*) fmt->r; void*
_temp277; struct _tagged_string _temp279; int _temp281; _LL273: if((( struct
_enum_struct*) _temp271)->tag == Cyc_Absyn_Const_e_tag){ _LL278: _temp277=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp271)->f1; if(( unsigned int) _temp277 > 1u?((
struct _enum_struct*) _temp277)->tag == Cyc_Absyn_String_c_tag: 0){ _LL282:
_temp281=( int)(( struct Cyc_Absyn_String_c_struct*) _temp277)->f1; if( _temp281
== 0){ goto _LL280;} else{ goto _LL275;} _LL280: _temp279=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp277)->f2; goto _LL274;}
else{ goto _LL275;}} else{ goto _LL275;} _LL275: goto _LL276; _LL274: desc_types=
type_getter( _temp279, fmt->loc); goto _LL272; _LL276: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp283=( char*)"expecting a literal format string";
struct _tagged_string _temp284; _temp284.curr= _temp283; _temp284.base= _temp283;
_temp284.last_plus_one= _temp283 + 34; _temp284;})); return; _LL272:;} for( 0;
desc_types != 0? args != 0: 0; desc_types= desc_types->tl, args= args->tl){ void*
t=( void*) desc_types->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) args->hd;
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp285= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp286= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_temp285.last_plus_one - _temp285.curr, _temp285.curr, _temp286.last_plus_one -
_temp286.curr, _temp286.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp287=( char*)"too few arguments"; struct
_tagged_string _temp288; _temp288.curr= _temp287; _temp288.base= _temp287;
_temp288.last_plus_one= _temp287 + 18; _temp288;}));} if( args != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) args->hd)->loc,( struct _tagged_string)({ char* _temp289=(
char*)"too many arguments"; struct _tagged_string _temp290; _temp290.curr=
_temp289; _temp290.base= _temp289; _temp290.last_plus_one= _temp289 + 19;
_temp290;}));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp291= p; _LL293:
if( _temp291 == Cyc_Absyn_Plus){ goto _LL294;} else{ goto _LL295;} _LL295: if(
_temp291 == Cyc_Absyn_Minus){ goto _LL296;} else{ goto _LL297;} _LL297: if(
_temp291 == Cyc_Absyn_Not){ goto _LL298;} else{ goto _LL299;} _LL299: if(
_temp291 == Cyc_Absyn_Bitnot){ goto _LL300;} else{ goto _LL301;} _LL301: if(
_temp291 == Cyc_Absyn_Size){ goto _LL302;} else{ goto _LL303;} _LL303: goto
_LL304; _LL294: goto _LL296; _LL296: if( ! Cyc_Tcutil_arithmetic_promote( te, e)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp305= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp305.last_plus_one -
_temp305.curr, _temp305.curr);}));} return( void*)( e->topt)->v; _LL298: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp306= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp306.last_plus_one - _temp306.curr, _temp306.curr);}));} return Cyc_Absyn_sint_t;
_LL300: if( ! Cyc_Tcutil_integral_promote( te, e)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp307= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp307.last_plus_one - _temp307.curr, _temp307.curr);}));} return( void*)( e->topt)->v;
_LL302:{ void* _temp308= t; struct Cyc_Absyn_Exp* _temp316; struct Cyc_Absyn_Tqual*
_temp318; void* _temp320; struct Cyc_Absyn_PtrInfo _temp322; struct Cyc_Absyn_Conref*
_temp324; struct Cyc_Absyn_Tqual* _temp326; struct Cyc_Absyn_Conref* _temp328;
void* _temp330; void* _temp332; _LL310: if(( unsigned int) _temp308 > 5u?((
struct _enum_struct*) _temp308)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL321:
_temp320=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp308)->f1; goto
_LL319; _LL319: _temp318=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp308)->f2; goto _LL317; _LL317: _temp316=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp308)->f3; goto _LL311;} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp308 > 5u?(( struct _enum_struct*) _temp308)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL323: _temp322=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp308)->f1; _LL333: _temp332=( void*)
_temp322.elt_typ; goto _LL331; _LL331: _temp330=( void*) _temp322.rgn_typ; goto
_LL329; _LL329: _temp328=( struct Cyc_Absyn_Conref*) _temp322.nullable; goto
_LL327; _LL327: _temp326=( struct Cyc_Absyn_Tqual*) _temp322.tq; goto _LL325;
_LL325: _temp324=( struct Cyc_Absyn_Conref*) _temp322.bounds; goto _LL313;}
else{ goto _LL314;} _LL314: goto _LL315; _LL311: goto _LL309; _LL313: goto
_LL309; _LL315: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp334= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp334.last_plus_one
- _temp334.curr, _temp334.curr);})); goto _LL309; _LL309:;} return Cyc_Absyn_uint_t;
_LL304: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp335=( char*)"Non-unary primop"; struct
_tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 17; _temp336;})); _LL292:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* promoter)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*)){ if( ! promoter(
te, e1)){ Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp337= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); xprintf("type %.*s cannot be used here", _temp337.last_plus_one
- _temp337.curr, _temp337.curr);})); return Cyc_Absyn_wildtyp();} if( ! promoter(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp338= Cyc_Absynpp_typ2string((
void*)( e2->topt)->v); xprintf("type %.*s cannot be used here", _temp338.last_plus_one
- _temp338.curr, _temp338.curr);})); return Cyc_Absyn_wildtyp();}{ void* t1= Cyc_Tcutil_compress((
void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress(( void*)( e2->topt)->v);
void* res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast(
te, e1, res); Cyc_Tcutil_unchecked_cast( te, e2, res); return res;}} static void*
Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp339= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("expecting int but found %.*s", _temp339.last_plus_one - _temp339.curr,
_temp339.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v;
void* t2=( void*)( e2->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if(
Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp340= Cyc_Absynpp_typ2string(( void*)( e1->topt)->v);
struct _tagged_string _temp341= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp340.last_plus_one - _temp340.curr, _temp340.curr, _temp341.last_plus_one -
_temp341.curr, _temp341.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp342= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp343= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp342.last_plus_one - _temp342.curr, _temp342.curr, _temp343.last_plus_one -
_temp343.curr, _temp343.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
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
void*)( e2->topt)->v;{ struct _tuple0 _temp345=({ struct _tuple0 _temp344;
_temp344.f1= Cyc_Tcutil_compress( t1); _temp344.f2= Cyc_Tcutil_compress( t2);
_temp344;}); void* _temp351; struct Cyc_Absyn_PtrInfo _temp353; struct Cyc_Absyn_Conref*
_temp355; struct Cyc_Absyn_Tqual* _temp357; struct Cyc_Absyn_Conref* _temp359;
void* _temp361; void* _temp363; void* _temp365; struct Cyc_Absyn_PtrInfo
_temp367; struct Cyc_Absyn_Conref* _temp369; struct Cyc_Absyn_Tqual* _temp371;
struct Cyc_Absyn_Conref* _temp373; void* _temp375; void* _temp377; _LL347:
_LL366: _temp365= _temp345.f1; if(( unsigned int) _temp365 > 5u?(( struct
_enum_struct*) _temp365)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL368: _temp367=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp365)->f1;
_LL378: _temp377=( void*) _temp367.elt_typ; goto _LL376; _LL376: _temp375=( void*)
_temp367.rgn_typ; goto _LL374; _LL374: _temp373=( struct Cyc_Absyn_Conref*)
_temp367.nullable; goto _LL372; _LL372: _temp371=( struct Cyc_Absyn_Tqual*)
_temp367.tq; goto _LL370; _LL370: _temp369=( struct Cyc_Absyn_Conref*) _temp367.bounds;
goto _LL352;} else{ goto _LL349;} _LL352: _temp351= _temp345.f2; if((
unsigned int) _temp351 > 5u?(( struct _enum_struct*) _temp351)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL354: _temp353=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp351)->f1; _LL364: _temp363=( void*) _temp353.elt_typ; goto _LL362; _LL362:
_temp361=( void*) _temp353.rgn_typ; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Conref*)
_temp353.nullable; goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Tqual*)
_temp353.tq; goto _LL356; _LL356: _temp355=( struct Cyc_Absyn_Conref*) _temp353.bounds;
goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL348: if( Cyc_Tcutil_unify(
_temp377, _temp363)){ return Cyc_Absyn_sint_t;} goto _LL346; _LL350: goto _LL346;
_LL346:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp379= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp380= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp379.last_plus_one - _temp379.curr, _temp379.curr, _temp380.last_plus_one -
_temp380.curr, _temp380.curr);})); return Cyc_Absyn_wildtyp();}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp381= p; _LL383: if( _temp381 == Cyc_Absyn_Plus){ goto _LL384;} else{
goto _LL385;} _LL385: if( _temp381 == Cyc_Absyn_Minus){ goto _LL386;} else{ goto
_LL387;} _LL387: if( _temp381 == Cyc_Absyn_Times){ goto _LL388;} else{ goto
_LL389;} _LL389: if( _temp381 == Cyc_Absyn_Div){ goto _LL390;} else{ goto _LL391;}
_LL391: if( _temp381 == Cyc_Absyn_Mod){ goto _LL392;} else{ goto _LL393;} _LL393:
if( _temp381 == Cyc_Absyn_Bitand){ goto _LL394;} else{ goto _LL395;} _LL395: if(
_temp381 == Cyc_Absyn_Bitor){ goto _LL396;} else{ goto _LL397;} _LL397: if(
_temp381 == Cyc_Absyn_Bitxor){ goto _LL398;} else{ goto _LL399;} _LL399: if(
_temp381 == Cyc_Absyn_Bitlshift){ goto _LL400;} else{ goto _LL401;} _LL401: if(
_temp381 == Cyc_Absyn_Bitlrshift){ goto _LL402;} else{ goto _LL403;} _LL403: if(
_temp381 == Cyc_Absyn_Bitarshift){ goto _LL404;} else{ goto _LL405;} _LL405: if(
_temp381 == Cyc_Absyn_Eq){ goto _LL406;} else{ goto _LL407;} _LL407: if(
_temp381 == Cyc_Absyn_Neq){ goto _LL408;} else{ goto _LL409;} _LL409: if(
_temp381 == Cyc_Absyn_Gt){ goto _LL410;} else{ goto _LL411;} _LL411: if(
_temp381 == Cyc_Absyn_Lt){ goto _LL412;} else{ goto _LL413;} _LL413: if(
_temp381 == Cyc_Absyn_Gte){ goto _LL414;} else{ goto _LL415;} _LL415: if(
_temp381 == Cyc_Absyn_Lte){ goto _LL416;} else{ goto _LL417;} _LL417: goto
_LL418; _LL384: return Cyc_Tcexp_tcPlus( te, e1, e2); _LL386: return Cyc_Tcexp_tcMinus(
te, e1, e2); _LL388: goto _LL390; _LL390: return Cyc_Tcexp_tcArithBinop( te, e1,
e2, Cyc_Tcutil_arithmetic_promote); _LL392: goto _LL394; _LL394: goto _LL396;
_LL396: goto _LL398; _LL398: goto _LL400; _LL400: goto _LL402; _LL402: goto
_LL404; _LL404: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_integral_promote);
_LL406: goto _LL408; _LL408: goto _LL410; _LL410: goto _LL412; _LL412: goto
_LL414; _LL414: goto _LL416; _LL416: return Cyc_Tcexp_tcAnyBinop( te, loc, e1,
e2); _LL418:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp419=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp419=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp420=( char*)"bad binary primop"; struct
_tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 18; _temp421;})};( struct _xenum_struct*)
_temp419;})); _LL382:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_List_List*
es){ if( p == Cyc_Absyn_Minus?(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es) == 1: 0){ return Cyc_Tcexp_tcExp( te, topt,( struct Cyc_Absyn_Exp*) es->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp422= p; _LL424: if( _temp422
== Cyc_Absyn_Printf){ goto _LL425;} else{ goto _LL426;} _LL426: if( _temp422 ==
Cyc_Absyn_Fprintf){ goto _LL427;} else{ goto _LL428;} _LL428: if( _temp422 ==
Cyc_Absyn_Xprintf){ goto _LL429;} else{ goto _LL430;} _LL430: if( _temp422 ==
Cyc_Absyn_Scanf){ goto _LL431;} else{ goto _LL432;} _LL432: if( _temp422 == Cyc_Absyn_Fscanf){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp422 == Cyc_Absyn_Sscanf){
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL425: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp438=(
char*)"missing format string in printf"; struct _tagged_string _temp439;
_temp439.curr= _temp438; _temp439.base= _temp438; _temp439.last_plus_one=
_temp438 + 32; _temp439;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)
es->hd, es->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_VoidType; goto
_LL423; _LL427: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp440=( char*)"too few arguments to fprintf";
struct _tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 29; _temp441;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp442= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp442.last_plus_one - _temp442.curr, _temp442.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_VoidType; goto _LL423; _LL429: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp443=( char*)"missing format string in xprintf";
struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base= _temp443;
_temp444.last_plus_one= _temp443 + 33; _temp444;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*) es->hd, es->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_string_typ( Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)); goto _LL423;
_LL431: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp445=( char*)"missing format string in scanf"; struct _tagged_string
_temp446; _temp446.curr= _temp445; _temp446.base= _temp445; _temp446.last_plus_one=
_temp445 + 31; _temp446;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)
es->hd, es->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL423;
_LL433: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp447=( char*)"too few arguments to fscanf";
struct _tagged_string _temp448; _temp448.curr= _temp447; _temp448.base= _temp447;
_temp448.last_plus_one= _temp447 + 28; _temp448;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp449= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp449.last_plus_one - _temp449.curr, _temp449.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL423; _LL435: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp450=( char*)"sscanf is not supported"; struct
_tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 24; _temp451;})); t= Cyc_Absyn_sint_t; goto
_LL423; _LL437: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL452: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp454=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 35; _temp455;})); case 1: _LL453: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd); break; case 2: _LL456: t= Cyc_Tcexp_tcBinPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd,( struct Cyc_Absyn_Exp*)( es->tl)->hd);
break; default: _LL457: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp459=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 37; _temp460;}));} goto _LL423; _LL423:;}
return t;}} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){
Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp461=( char*)"increment/decrement of non-lvalue";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 34; _temp462;}));}{ void* t=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_arithmetic_promote( te, e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp463= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp463.last_plus_one
- _temp463.curr, _temp463.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp464=( char*)"conditional expression";
struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 23; _temp465;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp466=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp466->hd=( void*) e3; _temp466->tl=
0; _temp466;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp467=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp467->hd=(
void*) e2; _temp467->tl=( struct Cyc_List_List*) l1; _temp467;}); if( ! Cyc_Tcutil_coerce_list(
te, t,( struct Cyc_List_List*) l2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp468= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v); struct
_tagged_string _temp469= Cyc_Absynpp_typ2string(( void*)( e3->topt)->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp468.last_plus_one - _temp468.curr, _temp468.curr, _temp469.last_plus_one -
_temp469.curr, _temp469.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)&( e1->topt)->v), e2);{ void*
t1=( void*)( e1->topt)->v; void* t2=( void*)( e2->topt)->v;{ void* _temp470= Cyc_Tcutil_compress(
t1); struct Cyc_Absyn_Exp* _temp476; struct Cyc_Absyn_Tqual* _temp478; void*
_temp480; _LL472: if(( unsigned int) _temp470 > 5u?(( struct _enum_struct*)
_temp470)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL481: _temp480=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp470)->f1; goto _LL479; _LL479: _temp478=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp470)->f2;
goto _LL477; _LL477: _temp476=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp470)->f3; goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL473:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp482=( char*)"cannot assign to an array";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 26; _temp483;})); goto _LL471; _LL475: goto
_LL471; _LL471:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp484=( char*)"assignment to non-lvalue";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 25; _temp485;}));} if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp486= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp487= Cyc_Absynpp_typ2string(
t2); xprintf("type mismatch: %.*s != %.*s", _temp486.last_plus_one - _temp486.curr,
_temp486.curr, _temp487.last_plus_one - _temp487.curr, _temp487.curr);}));}}
else{ void* p=( void*) po->v; void* t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
p, e1, e2); if( !( Cyc_Tcutil_unify( t_result, t1)? 1: Cyc_Tcutil_coerceable(
t_result))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp488=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 42; _temp489;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)( e2->topt)->v;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp490=
t; struct Cyc_Absyn_PtrInfo _temp496; struct Cyc_Absyn_Conref* _temp498; struct
Cyc_Absyn_Tqual* _temp500; struct Cyc_Absyn_Conref* _temp502; void* _temp504;
void* _temp506; _LL492: if(( unsigned int) _temp490 > 5u?(( struct _enum_struct*)
_temp490)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL497: _temp496=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp490)->f1; _LL507: _temp506=( void*)
_temp496.elt_typ; goto _LL505; _LL505: _temp504=( void*) _temp496.rgn_typ; goto
_LL503; _LL503: _temp502=( struct Cyc_Absyn_Conref*) _temp496.nullable; goto
_LL501; _LL501: _temp500=( struct Cyc_Absyn_Tqual*) _temp496.tq; goto _LL499;
_LL499: _temp498=( struct Cyc_Absyn_Conref*) _temp496.bounds; goto _LL493;}
else{ goto _LL494;} _LL494: goto _LL495; _LL493: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp504); Cyc_Tcutil_check_nonzero_bound( loc, _temp498);{ void*
_temp508= Cyc_Tcutil_compress( _temp506); struct Cyc_Absyn_FnInfo _temp514;
struct Cyc_List_List* _temp516; int _temp518; struct Cyc_List_List* _temp520;
void* _temp522; struct Cyc_Core_Opt* _temp524; struct Cyc_List_List* _temp526;
_LL510: if(( unsigned int) _temp508 > 5u?(( struct _enum_struct*) _temp508)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL515: _temp514=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp508)->f1; _LL527: _temp526=( struct Cyc_List_List*)
_temp514.tvars; goto _LL525; _LL525: _temp524=( struct Cyc_Core_Opt*) _temp514.effect;
goto _LL523; _LL523: _temp522=( void*) _temp514.ret_typ; goto _LL521; _LL521:
_temp520=( struct Cyc_List_List*) _temp514.args; goto _LL519; _LL519: _temp518=(
int) _temp514.varargs; goto _LL517; _LL517: _temp516=( struct Cyc_List_List*)
_temp514.attributes; goto _LL511;} else{ goto _LL512;} _LL512: goto _LL513;
_LL511: if( _temp518){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp528=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 41; _temp529;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp522,* topt);} while( es != 0? _temp520 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*) es->hd; void* t2=(*(( struct _tuple2*) _temp520->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp530= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); struct _tagged_string _temp531= Cyc_Absynpp_typ2string(
t2); xprintf("actual argument has type %.*s but formal has type %.*s", _temp530.last_plus_one
- _temp530.curr, _temp530.curr, _temp531.last_plus_one - _temp531.curr, _temp531.curr);}));}
es= es->tl; _temp520= _temp520->tl;} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp532=( char*)"too many arguments for function";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 32; _temp533;}));} if( _temp520 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp534=( char*)"too few arguments for function";
struct _tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 31; _temp535;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*) _temp524->v); return _temp522; _LL513: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp536=( char*)"expected pointer to function";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 29; _temp537;})); _LL509:;} _LL495: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp538=( char*)"expected pointer to function";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 29; _temp539;})); _LL491:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e);{ void*
t=( void*)( e->topt)->v; if( ! Cyc_Tcutil_unify( t, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp540= Cyc_Absynpp_typ2string( t); xprintf("expected xenum exn but found %.*s",
_temp540.last_plus_one - _temp540.curr, _temp540.curr);}));} return Cyc_Absyn_wildtyp();}}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);{
void* _temp541= t1; struct Cyc_Absyn_PtrInfo _temp547; struct Cyc_Absyn_Conref*
_temp549; struct Cyc_Absyn_Tqual* _temp551; struct Cyc_Absyn_Conref* _temp553;
void* _temp555; void* _temp557; _LL543: if(( unsigned int) _temp541 > 5u?((
struct _enum_struct*) _temp541)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL548:
_temp547=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp541)->f1; _LL558: _temp557=( void*) _temp547.elt_typ; goto _LL556; _LL556:
_temp555=( void*) _temp547.rgn_typ; goto _LL554; _LL554: _temp553=( struct Cyc_Absyn_Conref*)
_temp547.nullable; goto _LL552; _LL552: _temp551=( struct Cyc_Absyn_Tqual*)
_temp547.tq; goto _LL550; _LL550: _temp549=( struct Cyc_Absyn_Conref*) _temp547.bounds;
goto _LL544;} else{ goto _LL545;} _LL545: goto _LL546; _LL544:{ void* _temp559=
Cyc_Tcutil_compress( _temp557); struct Cyc_Absyn_FnInfo _temp565; struct Cyc_List_List*
_temp567; int _temp569; struct Cyc_List_List* _temp571; void* _temp573; struct
Cyc_Core_Opt* _temp575; struct Cyc_List_List* _temp577; _LL561: if((
unsigned int) _temp559 > 5u?(( struct _enum_struct*) _temp559)->tag == Cyc_Absyn_FnType_tag:
0){ _LL566: _temp565=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp559)->f1; _LL578: _temp577=( struct Cyc_List_List*) _temp565.tvars; goto
_LL576; _LL576: _temp575=( struct Cyc_Core_Opt*) _temp565.effect; goto _LL574;
_LL574: _temp573=( void*) _temp565.ret_typ; goto _LL572; _LL572: _temp571=(
struct Cyc_List_List*) _temp565.args; goto _LL570; _LL570: _temp569=( int)
_temp565.varargs; goto _LL568; _LL568: _temp567=( struct Cyc_List_List*)
_temp565.attributes; goto _LL562;} else{ goto _LL563;} _LL563: goto _LL564;
_LL562: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp577)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp579=( char*)"type instantiation mismatch"; struct _tagged_string _temp580;
_temp580.curr= _temp579; _temp580.base= _temp579; _temp580.last_plus_one=
_temp579 + 28; _temp580;}));} for( 0; ts != 0; ts= ts->tl, _temp577= _temp577->tl){
void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) _temp577->hd)->kind); Cyc_Tcutil_check_type( loc, te,
Cyc_Tcenv_lookup_type_vars( te), k,( void*) ts->hd); instantiation=({ struct Cyc_List_List*
_temp581=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp581->hd=( void*)({ struct _tuple4* _temp582=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp582->f1=( struct Cyc_Absyn_Tvar*) _temp577->hd;
_temp582->f2=( void*) ts->hd; _temp582;}); _temp581->tl= instantiation; _temp581;});}{
void* new_fn_typ= Cyc_Tcutil_substitute( instantiation,({ struct Cyc_Absyn_FnType_struct*
_temp583=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp583->tag= Cyc_Absyn_FnType_tag; _temp583->f1=({ struct Cyc_Absyn_FnInfo
_temp584; _temp584.tvars= 0; _temp584.effect= _temp575; _temp584.ret_typ=( void*)
_temp573; _temp584.args= _temp571; _temp584.varargs= _temp569; _temp584.attributes=
_temp567; _temp584;});( void*) _temp583;})); void* new_typ=({ struct Cyc_Absyn_PointerType_struct*
_temp585=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp585->tag= Cyc_Absyn_PointerType_tag; _temp585->f1=({ struct Cyc_Absyn_PtrInfo
_temp586; _temp586.elt_typ=( void*) new_fn_typ; _temp586.rgn_typ=( void*)
_temp555; _temp586.nullable= _temp553; _temp586.tq= _temp551; _temp586.bounds=
_temp549; _temp586;});( void*) _temp585;}); return new_typ;}} _LL564: goto
_LL560; _LL560:;} goto _LL542; _LL546: goto _LL542; _LL542:;} return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp587= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp587.last_plus_one - _temp587.curr, _temp587.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp588= Cyc_Absynpp_typ2string( t2); struct _tagged_string _temp589= Cyc_Absynpp_typ2string(
t); xprintf("cannot cast %.*s to %.*s", _temp588.last_plus_one - _temp588.curr,
_temp588.curr, _temp589.last_plus_one - _temp589.curr, _temp589.curr);}));}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void** topt2=
0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual(); if( topt != 0){ void*
_temp590= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp596; struct
Cyc_Absyn_Conref* _temp598; struct Cyc_Absyn_Tqual* _temp600; struct Cyc_Absyn_Conref*
_temp602; void* _temp604; void* _temp606; _LL592: if(( unsigned int) _temp590 >
5u?(( struct _enum_struct*) _temp590)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL597: _temp596=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp590)->f1; _LL607: _temp606=( void*) _temp596.elt_typ; goto _LL605; _LL605:
_temp604=( void*) _temp596.rgn_typ; goto _LL603; _LL603: _temp602=( struct Cyc_Absyn_Conref*)
_temp596.nullable; goto _LL601; _LL601: _temp600=( struct Cyc_Absyn_Tqual*)
_temp596.tq; goto _LL599; _LL599: _temp598=( struct Cyc_Absyn_Conref*) _temp596.bounds;
goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL593: topt2=( void**)({
void** _temp608=( void**) GC_malloc( sizeof( void*) * 1); _temp608[ 0]= _temp606;
_temp608;}); tq2= _temp600; goto _LL591; _LL595: goto _LL591; _LL591:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp609=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp617; struct Cyc_List_List* _temp619; struct Cyc_Core_Opt* _temp621; struct
_tuple1* _temp623; struct Cyc_List_List* _temp625; _LL611: if((( struct
_enum_struct*) _temp609)->tag == Cyc_Absyn_Struct_e_tag){ _LL624: _temp623=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp609)->f1; goto _LL622;
_LL622: _temp621=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp609)->f2; goto _LL620; _LL620: _temp619=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp609)->f3; goto _LL618; _LL618: _temp617=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp609)->f4;
goto _LL612;} else{ goto _LL613;} _LL613: if((( struct _enum_struct*) _temp609)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL626: _temp625=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp609)->f1; goto _LL614;} else{ goto _LL615;}
_LL615: goto _LL616; _LL612: goto _LL614; _LL614: { void*(* ptr_maker)( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){ void*
_temp627= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp633; struct
Cyc_Absyn_Conref* _temp635; struct Cyc_Absyn_Tqual* _temp637; struct Cyc_Absyn_Conref*
_temp639; void* _temp641; void* _temp643; _LL629: if(( unsigned int) _temp627 >
5u?(( struct _enum_struct*) _temp627)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL634: _temp633=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp627)->f1; _LL644: _temp643=( void*) _temp633.elt_typ; goto _LL642; _LL642:
_temp641=( void*) _temp633.rgn_typ; goto _LL640; _LL640: _temp639=( struct Cyc_Absyn_Conref*)
_temp633.nullable; goto _LL638; _LL638: _temp637=( struct Cyc_Absyn_Tqual*)
_temp633.tq; goto _LL636; _LL636: _temp635=( struct Cyc_Absyn_Conref*) _temp633.bounds;
goto _LL630;} else{ goto _LL631;} _LL631: goto _LL632; _LL630: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp639)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL628; _LL632: goto _LL628; _LL628:;} return ptr_maker(( void*)( e->topt)->v,
Cyc_Absyn_HeapRgn, tq2);} _LL616: goto _LL610; _LL610:;}{ void* _temp647; int
_temp649; struct _tuple5 _temp645= Cyc_Tcutil_addressof_props( te, e); _LL650:
_temp649= _temp645.f1; goto _LL648; _LL648: _temp647= _temp645.f2; goto _LL646;
_LL646: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if( _temp649){ tq->q_const=
1;}{ void* t= Cyc_Absyn_at_typ(( void*)( e->topt)->v, _temp647, tq); return t;}}}}
static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), Cyc_Absyn_MemKind, t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp651= t; struct Cyc_Absyn_PtrInfo
_temp657; struct Cyc_Absyn_Conref* _temp659; struct Cyc_Absyn_Tqual* _temp661;
struct Cyc_Absyn_Conref* _temp663; void* _temp665; void* _temp667; _LL653: if((
unsigned int) _temp651 > 5u?(( struct _enum_struct*) _temp651)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL658: _temp657=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp651)->f1; _LL668: _temp667=( void*) _temp657.elt_typ; goto _LL666; _LL666:
_temp665=( void*) _temp657.rgn_typ; goto _LL664; _LL664: _temp663=( struct Cyc_Absyn_Conref*)
_temp657.nullable; goto _LL662; _LL662: _temp661=( struct Cyc_Absyn_Tqual*)
_temp657.tq; goto _LL660; _LL660: _temp659=( struct Cyc_Absyn_Conref*) _temp657.bounds;
goto _LL654;} else{ goto _LL655;} _LL655: goto _LL656; _LL654: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp665); Cyc_Tcutil_check_nonzero_bound( loc, _temp659); return
_temp667; _LL656: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp669= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp669.last_plus_one - _temp669.curr, _temp669.curr);})); _LL652:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* _temp670= t; struct Cyc_Absyn_Structdecl** _temp680;
struct Cyc_Absyn_Structdecl* _temp682; struct Cyc_List_List* _temp683; struct
_tuple1* _temp685; struct Cyc_Absyn_Exp* _temp688; struct Cyc_Absyn_Tqual*
_temp690; void* _temp692; struct Cyc_Absyn_PtrInfo _temp697; _LL672: if((
unsigned int) _temp670 > 5u?(( struct _enum_struct*) _temp670)->tag == Cyc_Absyn_StructType_tag:
0){ _LL686: _temp685=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp670)->f1; goto _LL684; _LL684: _temp683=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp670)->f2; goto _LL681; _LL681: _temp680=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp670)->f3;
if( _temp680 == 0){ goto _LL674;} else{ _temp682=* _temp680; goto _LL673;}}
else{ goto _LL674;} _LL674: if(( unsigned int) _temp670 > 5u?(( struct
_enum_struct*) _temp670)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL693: _temp692=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp670)->f1; goto _LL691; _LL691:
_temp690=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp670)->f2; goto _LL689; _LL689: _temp688=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp670)->f3; goto _LL687;} else{ goto _LL676;}
_LL687: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char* _temp694=(
char*)"size"; struct _tagged_string _temp695; _temp695.curr= _temp694; _temp695.base=
_temp694; _temp695.last_plus_one= _temp694 + 5; _temp695;})) == 0){ goto _LL675;}
else{ goto _LL676;} _LL676: if(( unsigned int) _temp670 > 5u?(( struct
_enum_struct*) _temp670)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL698: _temp697=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp670)->f1;
goto _LL696;} else{ goto _LL678;} _LL696: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp699=( char*)"size"; struct _tagged_string _temp700;
_temp700.curr= _temp699; _temp700.base= _temp699; _temp700.last_plus_one=
_temp699 + 5; _temp700;})) == 0){ goto _LL677;} else{ goto _LL678;} _LL678: goto
_LL679; _LL673: if( _temp685 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp701=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp702; _temp702.curr= _temp701; _temp702.base= _temp701;
_temp702.last_plus_one= _temp701 + 40; _temp702;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp682, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp704= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp703= _temp685; if( _temp703 == 0){ _throw(
Null_Exception);} _temp703;})); struct _tagged_string _temp705=* f; xprintf("struct %.*s has no %.*s field",
_temp704.last_plus_one - _temp704.curr, _temp704.curr, _temp705.last_plus_one -
_temp705.curr, _temp705.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp682->tvs,
_temp683); void* t2= Cyc_Tcutil_substitute( inst,( void*) finfo->type); return
t2;}} _LL675:( void*)( outer_e->r=( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp706=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp706->tag= Cyc_Absyn_Primop_e_tag; _temp706->f1=( void*) Cyc_Absyn_Size;
_temp706->f2=({ struct Cyc_List_List* _temp707=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp707->hd=( void*) e; _temp707->tl=
0; _temp707;});( void*) _temp706;})); return Cyc_Absyn_uint_t; _LL677:( void*)(
outer_e->r=( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp708=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp708->tag= Cyc_Absyn_Primop_e_tag;
_temp708->f1=( void*) Cyc_Absyn_Size; _temp708->f2=({ struct Cyc_List_List*
_temp709=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp709->hd=( void*) e; _temp709->tl= 0; _temp709;});( void*) _temp708;}));
return Cyc_Absyn_uint_t; _LL679: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp710=( char*)"size"; struct _tagged_string _temp711;
_temp711.curr= _temp710; _temp711.base= _temp710; _temp711.last_plus_one=
_temp710 + 5; _temp711;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp712= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or array, found %.*s",
_temp712.last_plus_one - _temp712.curr, _temp712.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp713= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, found %.*s",
_temp713.last_plus_one - _temp713.curr, _temp713.curr);}));} _LL671:;}} static
void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp714=
t; struct Cyc_Absyn_PtrInfo _temp720; struct Cyc_Absyn_Conref* _temp722; struct
Cyc_Absyn_Tqual* _temp724; struct Cyc_Absyn_Conref* _temp726; void* _temp728;
void* _temp730; _LL716: if(( unsigned int) _temp714 > 5u?(( struct _enum_struct*)
_temp714)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL721: _temp720=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp714)->f1; _LL731: _temp730=( void*)
_temp720.elt_typ; goto _LL729; _LL729: _temp728=( void*) _temp720.rgn_typ; goto
_LL727; _LL727: _temp726=( struct Cyc_Absyn_Conref*) _temp720.nullable; goto
_LL725; _LL725: _temp724=( struct Cyc_Absyn_Tqual*) _temp720.tq; goto _LL723;
_LL723: _temp722=( struct Cyc_Absyn_Conref*) _temp720.bounds; goto _LL717;}
else{ goto _LL718;} _LL718: goto _LL719; _LL717: Cyc_Tcutil_check_nonzero_bound(
loc, _temp722);{ void* _temp732= Cyc_Tcutil_compress( _temp730); struct Cyc_Absyn_Structdecl**
_temp738; struct Cyc_Absyn_Structdecl* _temp740; struct Cyc_List_List* _temp741;
struct _tuple1* _temp743; _LL734: if(( unsigned int) _temp732 > 5u?(( struct
_enum_struct*) _temp732)->tag == Cyc_Absyn_StructType_tag: 0){ _LL744: _temp743=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp732)->f1; goto
_LL742; _LL742: _temp741=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp732)->f2; goto _LL739; _LL739: _temp738=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp732)->f3; if( _temp738 == 0){ goto
_LL736;} else{ _temp740=* _temp738; goto _LL735;}} else{ goto _LL736;} _LL736:
goto _LL737; _LL735: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp740, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp746= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp745= _temp743; if( _temp745 == 0){ _throw( Null_Exception);}
_temp745;})); struct _tagged_string _temp747=* f; xprintf("struct %.*s has no %.*s field",
_temp746.last_plus_one - _temp746.curr, _temp746.curr, _temp747.last_plus_one -
_temp747.curr, _temp747.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp740->tvs,
_temp741); void* t3= Cyc_Tcutil_substitute( inst,( void*) finfo->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp728); return t3;}} _LL737: return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp748= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp748.last_plus_one - _temp748.curr, _temp748.curr);})); _LL733:;} _LL719:
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp749= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp749.last_plus_one -
_temp749.curr, _temp749.curr);})); _LL715:;}} static void* Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp750; _push_handler(& _temp750);{ struct
_xenum_struct* _temp751=( struct _xenum_struct*) setjmp( _temp750.handler); if(
! _temp751){{ void* _temp752=(*(( struct _tuple7*(*)( struct Cyc_List_List* x,
int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp752;};
_pop_handler();} else{ struct _xenum_struct* _temp754= _temp751; _LL756: if(
_temp754->tag == Cyc_List_Nth_tag){ goto _LL757;} else{ goto _LL758;} _LL758:
goto _LL759; _LL757: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL759:(
void) _throw( _temp754); _LL755:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return Cyc_Tcexp_expr_err(
te, e2->loc,({ struct _tagged_string _temp760= Cyc_Absynpp_typ2string( t2);
xprintf("expecting int subscript, found %.*s", _temp760.last_plus_one - _temp760.curr,
_temp760.curr);}));}{ void* _temp761= t1; struct Cyc_Absyn_PtrInfo _temp769;
struct Cyc_Absyn_Conref* _temp771; struct Cyc_Absyn_Tqual* _temp773; struct Cyc_Absyn_Conref*
_temp775; void* _temp777; void* _temp779; struct Cyc_List_List* _temp781; _LL763:
if(( unsigned int) _temp761 > 5u?(( struct _enum_struct*) _temp761)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL770: _temp769=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp761)->f1; _LL780: _temp779=( void*) _temp769.elt_typ; goto _LL778; _LL778:
_temp777=( void*) _temp769.rgn_typ; goto _LL776; _LL776: _temp775=( struct Cyc_Absyn_Conref*)
_temp769.nullable; goto _LL774; _LL774: _temp773=( struct Cyc_Absyn_Tqual*)
_temp769.tq; goto _LL772; _LL772: _temp771=( struct Cyc_Absyn_Conref*) _temp769.bounds;
goto _LL764;} else{ goto _LL765;} _LL765: if(( unsigned int) _temp761 > 5u?((
struct _enum_struct*) _temp761)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL782:
_temp781=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp761)->f1;
goto _LL766;} else{ goto _LL767;} _LL767: goto _LL768; _LL764: if( Cyc_Tcexp_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp771);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp771);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp777); return _temp779; _LL766: return Cyc_Tcexp_ithTupleType( te,
loc, _temp781, e2); _LL768: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp783= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp783.last_plus_one - _temp783.curr, _temp783.curr);})); _LL762:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp784= Cyc_Tcutil_compress(* topt); struct
Cyc_List_List* _temp790; _LL786: if(( unsigned int) _temp784 > 5u?(( struct
_enum_struct*) _temp784)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL791: _temp790=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp784)->f1; goto
_LL787;} else{ goto _LL788;} _LL788: goto _LL789; _LL787: if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp790) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp792=( char*)"tuple expression has the wrong number of fields"; struct
_tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 48; _temp793;})); goto _LL785;} for( 0; es !=
0; es= es->tl, _temp790= _temp790->tl){ void* topt2=(*(( struct _tuple7*)
_temp790->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& topt2,( struct Cyc_Absyn_Exp*)
es->hd); fields=({ struct Cyc_List_List* _temp794=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp794->hd=( void*)({ struct
_tuple7* _temp795=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp795[ 0]=({ struct _tuple7 _temp796; _temp796.f1=(*(( struct _tuple7*)
_temp790->hd)).f1; _temp796.f2=( void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v;
_temp796;}); _temp795;}); _temp794->tl= fields; _temp794;});} done= 1; goto
_LL785; _LL789: goto _LL785; _LL785:;} if( ! done){ for( 0; es != 0; es= es->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*) es->hd); fields=({
struct Cyc_List_List* _temp797=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp797->hd=( void*)({ struct _tuple7* _temp798=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp798[ 0]=({ struct
_tuple7 _temp799; _temp799.f1= Cyc_Absyn_empty_tqual(); _temp799.f2=( void*)(((
struct Cyc_Absyn_Exp*) es->hd)->topt)->v; _temp799;}); _temp798;}); _temp797->tl=
fields; _temp797;});}} return({ struct Cyc_Absyn_TupleType_struct* _temp800=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp800->tag= Cyc_Absyn_TupleType_tag; _temp800->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);( void*) _temp800;});}
static void* Cyc_Tcexp_tcCompoundLit( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp801=( char*)"tcCompoundLit";
struct _tagged_string _temp802; _temp802.curr= _temp801; _temp802.base= _temp801;
_temp802.last_plus_one= _temp801 + 14; _temp802;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, int heap_allocated, struct Cyc_List_List* des){ struct Cyc_List_List*
es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple6*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
void* sz_rexp=({ struct Cyc_Absyn_Const_e_struct* _temp803=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp803->tag= Cyc_Absyn_Const_e_tag;
_temp803->f1=( void*)({ struct Cyc_Absyn_Int_c_struct* _temp804=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp804->tag= Cyc_Absyn_Int_c_tag;
_temp804->f1=( void*) Cyc_Absyn_Unsigned; _temp804->f2=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es);( void*) _temp804;});( void*) _temp803;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp( sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp805=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp805->v=( void*) Cyc_Absyn_uint_t; _temp805;});{ void* res_t2; if(
heap_allocated){ res_t2= Cyc_Absyn_atb_typ( res, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),({
struct Cyc_Absyn_Upper_b_struct* _temp806=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp806->tag= Cyc_Absyn_Upper_b_tag;
_temp806->f1= sz_exp;( void*) _temp806;}));} else{ res_t2=({ struct Cyc_Absyn_ArrayType_struct*
_temp807=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp807->tag= Cyc_Absyn_ArrayType_tag; _temp807->f1=( void*) res; _temp807->f2=
Cyc_Absyn_empty_tqual(); _temp807->f3= sz_exp;( void*) _temp807;});} if( topt ==
0){ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer(
te, 0,( struct Cyc_Absyn_Exp*) es2->hd);}} else{ void* _temp808= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp816; struct Cyc_Absyn_Conref* _temp818;
struct Cyc_Absyn_Tqual* _temp820; struct Cyc_Absyn_Conref* _temp822; void*
_temp824; void* _temp826; struct Cyc_Absyn_Exp* _temp828; struct Cyc_Absyn_Tqual*
_temp830; void* _temp832; _LL810: if(( unsigned int) _temp808 > 5u?(( struct
_enum_struct*) _temp808)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL817: _temp816=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp808)->f1;
_LL827: _temp826=( void*) _temp816.elt_typ; goto _LL825; _LL825: _temp824=( void*)
_temp816.rgn_typ; goto _LL823; _LL823: _temp822=( struct Cyc_Absyn_Conref*)
_temp816.nullable; goto _LL821; _LL821: _temp820=( struct Cyc_Absyn_Tqual*)
_temp816.tq; goto _LL819; _LL819: _temp818=( struct Cyc_Absyn_Conref*) _temp816.bounds;
goto _LL811;} else{ goto _LL812;} _LL812: if(( unsigned int) _temp808 > 5u?((
struct _enum_struct*) _temp808)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL833:
_temp832=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp808)->f1; goto
_LL831; _LL831: _temp830=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp808)->f2; goto _LL829; _LL829: _temp828=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp808)->f3; goto _LL813;} else{ goto _LL814;}
_LL814: goto _LL815; _LL811: _temp832= _temp826; goto _LL813; _LL813: { void*
topt1= _temp832;{ struct Cyc_List_List* es1= es; for( 0; es1 != 0; es1= es1->tl){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es1->hd; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt1, e); Cyc_Tcutil_coerce_arg( te, e, _temp832);}} goto _LL809;}
_LL815: Cyc_Tcexp_tcExpList( te, es); goto _LL809; _LL809:;} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct
_tagged_string _temp834= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp834.last_plus_one - _temp834.curr, _temp834.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des= des->tl){ struct Cyc_List_List* ds=(*(( struct
_tuple6*) des->hd)).f1; if( ds != 0){ void* _temp835=( void*) ds->hd; struct
_tagged_string* _temp841; struct Cyc_Absyn_Exp* _temp843; _LL837: if((( struct
_enum_struct*) _temp835)->tag == Cyc_Absyn_FieldName_tag){ _LL842: _temp841=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp835)->f1;
goto _LL838;} else{ goto _LL839;} _LL839: if((( struct _enum_struct*) _temp835)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL844: _temp843=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp835)->f1; goto _LL840;} else{ goto
_LL836;} _LL838: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp845=(
char*)"only array index designators are supported"; struct _tagged_string
_temp846; _temp846.curr= _temp845; _temp846.base= _temp845; _temp846.last_plus_one=
_temp845 + 43; _temp846;})); goto _LL836; _LL840: Cyc_Tcexp_tcExpInitializer( te,
0, _temp843);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp843); if( i
!=( unsigned int) offset){ Cyc_Tcutil_terr( _temp843->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL836;} _LL836:;}}} if( topt != 0){ if( ! Cyc_Tcutil_unify(*
topt, res_t2)? Cyc_Tcutil_silent_castable( te, loc, res_t2,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp847=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp847->v=( void*) res_t2; _temp847;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_t2=* topt;}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp848= Cyc_Absynpp_typ2string((
void*)( bound->topt)->v); xprintf("expecting unsigned int, found %.*s", _temp848.last_plus_one
- _temp848.curr, _temp848.curr);}));} te= Cyc_Tcenv_new_block( te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2= 0; struct Cyc_Absyn_Conref**
nopt= 0; struct Cyc_Absyn_Tqual** tqopt= 0; struct Cyc_Absyn_Conref** bopt= 0;
if( topt != 0){ void* _temp849= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp855; _LL851: if(( unsigned int) _temp849 > 5u?(( struct _enum_struct*)
_temp849)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL856: _temp855=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp849)->f1; goto _LL852;} else{ goto
_LL853;} _LL853: goto _LL854; _LL852: pinfo= _temp855; topt2=( void**)(( void*)&
pinfo.elt_typ); nopt=( struct Cyc_Absyn_Conref**)& pinfo.nullable; tqopt=(
struct Cyc_Absyn_Tqual**)& pinfo.tq; bopt=( struct Cyc_Absyn_Conref**)& pinfo.bounds;
goto _LL850; _LL854: goto _LL850; _LL850:;} Cyc_Tcexp_tcExp( te, topt2, body);{
void* b_result; int bound_is_const= Cyc_Tcexp_is_const_exp( te, bound); if( bopt
!= 0){ void* _temp857=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)(* bopt))->v; void* _temp865; void* _temp867;
struct Cyc_Absyn_Exp* _temp869; _LL859: if(( unsigned int) _temp857 > 1u?((
struct _enum_struct*) _temp857)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL866:
_temp865=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp857)->f1; if(
_temp865 == Cyc_Absyn_Unknown_b){ goto _LL860;} else{ goto _LL861;}} else{ goto
_LL861;} _LL861: if(( unsigned int) _temp857 > 1u?(( struct _enum_struct*)
_temp857)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL868: _temp867=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp857)->f1; if(( unsigned int) _temp867 >
1u?(( struct _enum_struct*) _temp867)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL870:
_temp869=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp867)->f1;
goto _LL862;} else{ goto _LL863;}} else{ goto _LL863;} _LL863: goto _LL864;
_LL860: b_result= Cyc_Absyn_Unknown_b; goto _LL858; _LL862: if( bound_is_const?
Cyc_Evexp_eval_const_uint_exp( _temp869) <= Cyc_Evexp_eval_const_uint_exp( bound):
0){ b_result=({ struct Cyc_Absyn_Upper_b_struct* _temp871=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp871->tag= Cyc_Absyn_Upper_b_tag;
_temp871->f1= _temp869;( void*) _temp871;}); goto _LL858;} goto _LL864; _LL864:
b_result= bound_is_const?({ struct Cyc_Absyn_Upper_b_struct* _temp872=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp872->tag= Cyc_Absyn_Upper_b_tag; _temp872->f1= bound;( void*) _temp872;}):
Cyc_Absyn_Unknown_b; goto _LL858; _LL858:;} else{ b_result= bound_is_const?({
struct Cyc_Absyn_Upper_b_struct* _temp873=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp873->tag= Cyc_Absyn_Upper_b_tag;
_temp873->f1= bound;( void*) _temp873;}): Cyc_Absyn_Unknown_b;}{ struct Cyc_Absyn_Tqual*
tq_result= tqopt != 0?* tqopt: Cyc_Absyn_empty_tqual(); void*(* ptr_maker)( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b)= b_result == Cyc_Absyn_Unknown_b?
Cyc_Absyn_starb_typ: Cyc_Absyn_atb_typ; return ptr_maker(( void*)( body->topt)->v,
Cyc_Absyn_HeapRgn, tq_result, b_result);}}}} static void* Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct
Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt !=
0){ sd=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp874=*
sd_opt; if( _temp874 == 0){ _throw( Null_Exception);} _temp874;});} else{{
struct _handler_cons _temp875; _push_handler(& _temp875);{ struct _xenum_struct*
_temp876=( struct _xenum_struct*) setjmp( _temp875.handler); if( ! _temp876){ sd=*
Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();} else{ struct
_xenum_struct* _temp878= _temp876; _LL880: if( _temp878->tag == Cyc_Dict_Absent_tag){
goto _LL881;} else{ goto _LL882;} _LL882: goto _LL883; _LL881: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp884= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp884.last_plus_one - _temp884.curr, _temp884.curr);})); return topt != 0?*
topt: Cyc_Absyn_VoidType; _LL883:( void) _throw( _temp878); _LL879:;}}}* sd_opt=(
struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=( struct _tuple1*)( sd->name)->v;}}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var,
sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* res_typ=({ struct Cyc_Absyn_StructType_struct*
_temp885=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp885->tag= Cyc_Absyn_StructType_tag; _temp885->f1=( struct _tuple1*)* tn;
_temp885->f2= typs; _temp885->f3=( struct Cyc_Absyn_Structdecl**)({ struct Cyc_Absyn_Structdecl**
_temp886=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)
* 1); _temp886[ 0]= sd; _temp886;});( void*) _temp885;}); if( topt != 0){ Cyc_Tcutil_unify(
res_typ,* topt);}* otyps=({ struct Cyc_Core_Opt* _temp887=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp887->v=( void*) typs; _temp887;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields= fields->tl){ struct _tuple8
_temp890; struct Cyc_Absyn_Exp* _temp891; struct Cyc_Absyn_Structfield* _temp893;
struct _tuple8* _temp888=( struct _tuple8*) fields->hd; _temp890=* _temp888;
_LL894: _temp893= _temp890.f1; goto _LL892; _LL892: _temp891= _temp890.f2; goto
_LL889; _LL889: { void* inst_fieldtyp= Cyc_Tcutil_substitute( inst,( void*)
_temp893->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& inst_fieldtyp,
_temp891); if( ! Cyc_Tcutil_coerce_arg( te, _temp891, inst_fieldtyp)){ Cyc_Tcutil_terr(
_temp891->loc,({ struct _tagged_string _temp895=* _temp893->name; struct
_tagged_string _temp896= Cyc_Absynpp_qvar2string(* tn); struct _tagged_string
_temp897= Cyc_Absynpp_typ2string( inst_fieldtyp); struct _tagged_string _temp898=
Cyc_Absynpp_typ2string(( void*)( _temp891->topt)->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp895.last_plus_one - _temp895.curr, _temp895.curr, _temp896.last_plus_one -
_temp896.curr, _temp896.curr, _temp897.last_plus_one - _temp897.curr, _temp897.curr,
_temp898.last_plus_one - _temp898.curr, _temp898.curr);}));}}} return res_typ;}}}
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
Cyc_Core_Opt* _temp899=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp899->v=( void*) all_typs; _temp899;});* exist_ref=({ struct Cyc_Core_Opt*
_temp900=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp900->v=( void*) exist_typs; _temp900;});{ void* res=({ struct Cyc_Absyn_EnumType_struct*
_temp901=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp901->tag= Cyc_Absyn_EnumType_tag; _temp901->f1=( struct _tuple1*)(( struct
_tuple1*)( ed->name)->v); _temp901->f2= all_typs; _temp901->f3=( struct Cyc_Absyn_Enumdecl**)({
struct Cyc_Absyn_Enumdecl** _temp902=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*) * 1); _temp902[ 0]= ed; _temp902;});( void*)
_temp901;}); if( topt != 0){ Cyc_Tcutil_unify(* topt, res);}{ struct Cyc_List_List*
ts= ef->typs; for( 0; es != 0? ts != 0: 0; es= es->tl, ts= ts->tl){ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*) es->hd; void* t= Cyc_Tcutil_substitute( inst,(*((
struct _tuple7*) ts->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp905= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp906= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp907= e->topt == 0?( struct _tagged_string)({ char* _temp903=( char*)"?";
struct _tagged_string _temp904; _temp904.curr= _temp903; _temp904.base= _temp903;
_temp904.last_plus_one= _temp903 + 2; _temp904;}): Cyc_Absynpp_typ2string(( void*)(
e->topt)->v); xprintf("enum constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp905.last_plus_one - _temp905.curr, _temp905.curr, _temp906.last_plus_one -
_temp906.curr, _temp906.curr, _temp907.last_plus_one - _temp907.curr, _temp907.curr);}));}}
if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*) es->hd)->loc,({
struct _tagged_string _temp908= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for enum constructor %.*s",
_temp908.last_plus_one - _temp908.curr, _temp908.curr);}));} if( ts != 0){
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp909= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too few arguments for enum constructor %.*s", _temp909.last_plus_one
- _temp909.curr, _temp909.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXenum(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Xenumdecl*
xd, struct Cyc_Absyn_Enumfield* ef){ struct Cyc_List_List* exist_inst=(( struct
Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); void*
res=({ struct Cyc_Absyn_XenumType_struct* _temp910=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp910->tag= Cyc_Absyn_XenumType_tag;
_temp910->f1= xd->name; _temp910->f2=( struct Cyc_Absyn_Xenumdecl**)({ struct
Cyc_Absyn_Xenumdecl** _temp911=( struct Cyc_Absyn_Xenumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Xenumdecl*) * 1); _temp911[ 0]= xd; _temp911;});( void*)
_temp910;}); struct Cyc_List_List* ts= ef->typs;* exist_ref=({ struct Cyc_Core_Opt*
_temp912=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp912->v=( void*) exist_typs; _temp912;}); while( es != 0? ts != 0: 0) {
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd; void* t= Cyc_Tcutil_substitute(
exist_inst,(*(( struct _tuple7*) ts->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,(
void**)& t, e); es= es->tl; ts= ts->tl; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){
Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp913= Cyc_Absynpp_qvar2string(
ef->name); struct _tagged_string _temp914= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp915= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("xenum constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp913.last_plus_one - _temp913.curr, _temp913.curr, _temp914.last_plus_one -
_temp914.curr, _temp914.curr, _temp915.last_plus_one - _temp915.curr, _temp915.curr);}));}}
if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct
_tagged_string _temp916= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for xenum constructor %.*s",
_temp916.last_plus_one - _temp916.curr, _temp916.curr);}));} else{ if( ts != 0){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string
_temp917= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xenum constructor %.*s",
_temp917.last_plus_one - _temp917.curr, _temp917.curr);}));}} return res;}
static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void** ma_ptr){ void* _temp918=* ma_ptr; void* _temp928;
struct _tuple1* _temp930; struct Cyc_Absyn_Enumfield* _temp932; struct Cyc_Absyn_Enumdecl*
_temp934; struct Cyc_Absyn_Enumfield* _temp936; struct Cyc_Absyn_Xenumdecl*
_temp938; _LL920: if((( struct _enum_struct*) _temp918)->tag == Cyc_Absyn_Typ_m_tag){
_LL929: _temp928=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp918)->f1; goto
_LL921;} else{ goto _LL922;} _LL922: if((( struct _enum_struct*) _temp918)->tag
== Cyc_Absyn_Unresolved_m_tag){ _LL931: _temp930=( struct _tuple1*)(( struct Cyc_Absyn_Unresolved_m_struct*)
_temp918)->f1; goto _LL923;} else{ goto _LL924;} _LL924: if((( struct
_enum_struct*) _temp918)->tag == Cyc_Absyn_Enum_m_tag){ _LL935: _temp934=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_m_struct*) _temp918)->f1;
goto _LL933; _LL933: _temp932=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_m_struct*)
_temp918)->f2; goto _LL925;} else{ goto _LL926;} _LL926: if((( struct
_enum_struct*) _temp918)->tag == Cyc_Absyn_Xenum_m_tag){ _LL939: _temp938=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_m_struct*) _temp918)->f1;
goto _LL937; _LL937: _temp936=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_m_struct*)
_temp918)->f2; goto _LL927;} else{ goto _LL919;} _LL921: { void*(* ptr_maker)(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){
void* _temp940= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp946;
struct Cyc_Absyn_Conref* _temp948; struct Cyc_Absyn_Tqual* _temp950; struct Cyc_Absyn_Conref*
_temp952; void* _temp954; void* _temp956; _LL942: if(( unsigned int) _temp940 >
5u?(( struct _enum_struct*) _temp940)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL947: _temp946=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp940)->f1; _LL957: _temp956=( void*) _temp946.elt_typ; goto _LL955; _LL955:
_temp954=( void*) _temp946.rgn_typ; goto _LL953; _LL953: _temp952=( struct Cyc_Absyn_Conref*)
_temp946.nullable; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Tqual*)
_temp946.tq; goto _LL949; _LL949: _temp948=( struct Cyc_Absyn_Conref*) _temp946.bounds;
goto _LL943;} else{ goto _LL944;} _LL944: goto _LL945; _LL943: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp952)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL941; _LL945: goto _LL941; _LL941:;} return ptr_maker( _temp928, Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual());} _LL923:{ struct _handler_cons _temp958; _push_handler(&
_temp958);{ struct _xenum_struct* _temp959=( struct _xenum_struct*) setjmp(
_temp958.handler); if( ! _temp959){{ void* _temp960= Cyc_Tcenv_lookup_ordinary(
te, loc, _temp930); struct Cyc_Absyn_Enumfield* _temp968; struct Cyc_Absyn_Enumdecl*
_temp970; struct Cyc_Absyn_Enumfield* _temp972; struct Cyc_Absyn_Xenumdecl*
_temp974; _LL962: if((( struct _enum_struct*) _temp960)->tag == Cyc_Tcenv_EnumRes_tag){
_LL971: _temp970=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp960)->f1; goto _LL969; _LL969: _temp968=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp960)->f2; goto _LL963;} else{ goto _LL964;}
_LL964: if((( struct _enum_struct*) _temp960)->tag == Cyc_Tcenv_XenumRes_tag){
_LL975: _temp974=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp960)->f1; goto _LL973; _LL973: _temp972=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp960)->f2; goto _LL965;} else{ goto
_LL966;} _LL966: goto _LL967; _LL963:* ma_ptr=({ struct Cyc_Absyn_Enum_m_struct*
_temp976=( struct Cyc_Absyn_Enum_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_m_struct));
_temp976->tag= Cyc_Absyn_Enum_m_tag; _temp976->f1= _temp970; _temp976->f2=
_temp968;( void*) _temp976;});{ void* _temp977= Cyc_Tcexp_tcMalloc( te, loc,
topt, ma_ptr); _npop_handler( 0u); return _temp977;} _LL965:* ma_ptr=({ struct
Cyc_Absyn_Xenum_m_struct* _temp978=( struct Cyc_Absyn_Xenum_m_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Xenum_m_struct)); _temp978->tag= Cyc_Absyn_Xenum_m_tag;
_temp978->f1= _temp974; _temp978->f2= _temp972;( void*) _temp978;});{ void*
_temp979= Cyc_Tcexp_tcMalloc( te, loc, topt, ma_ptr); _npop_handler( 0u); return
_temp979;} _LL967: goto _LL961; _LL961:;}; _pop_handler();} else{ struct
_xenum_struct* _temp981= _temp959; _LL983: if( _temp981->tag == Cyc_Dict_Absent_tag){
goto _LL984;} else{ goto _LL985;} _LL985: goto _LL986; _LL984: goto _LL982;
_LL986:( void) _throw( _temp981); _LL982:;}}} Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp987= Cyc_Absynpp_qvar2string( _temp930); xprintf("malloc argument %.*s is neither a type nor a variant",
_temp987.last_plus_one - _temp987.curr, _temp987.curr);})); if( topt != 0){
return* topt;} return Cyc_Absyn_VoidType; _LL925: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp988=( char*)"malloc of enum variant not yet implemented";
struct _tagged_string _temp989; _temp989.curr= _temp988; _temp989.base= _temp988;
_temp989.last_plus_one= _temp988 + 43; _temp989;}));{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp934->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); return({ struct Cyc_Absyn_EnumType_struct* _temp990=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp990->tag= Cyc_Absyn_EnumType_tag; _temp990->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp934->name)->v); _temp990->f2= all_typs; _temp990->f3=( struct
Cyc_Absyn_Enumdecl**)({ struct Cyc_Absyn_Enumdecl** _temp991=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*) * 1); _temp991[ 0]= _temp934;
_temp991;});( void*) _temp990;});} _LL927: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp992=( char*)"malloc of xenum variant not yet implemented";
struct _tagged_string _temp993; _temp993.curr= _temp992; _temp993.base= _temp992;
_temp993.last_plus_one= _temp992 + 44; _temp993;})); return({ struct Cyc_Absyn_XenumType_struct*
_temp994=( struct Cyc_Absyn_XenumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct));
_temp994->tag= Cyc_Absyn_XenumType_tag; _temp994->f1= _temp938->name; _temp994->f2=(
struct Cyc_Absyn_Xenumdecl**)({ struct Cyc_Absyn_Xenumdecl** _temp995=( struct
Cyc_Absyn_Xenumdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl*) * 1);
_temp995[ 0]= _temp938; _temp995;});( void*) _temp994;}); _LL919:;} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp996=( void*) s->r; struct Cyc_Absyn_Exp* _temp1006;
struct Cyc_Absyn_Stmt* _temp1008; struct Cyc_Absyn_Stmt* _temp1010; struct Cyc_Absyn_Stmt*
_temp1012; struct Cyc_Absyn_Decl* _temp1014; _LL998: if(( unsigned int) _temp996
> 1u?(( struct _enum_struct*) _temp996)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1007:
_temp1006=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp996)->f1;
goto _LL999;} else{ goto _LL1000;} _LL1000: if(( unsigned int) _temp996 > 1u?((
struct _enum_struct*) _temp996)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1011:
_temp1010=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp996)->f1;
goto _LL1009; _LL1009: _temp1008=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp996)->f2; goto _LL1001;} else{ goto _LL1002;} _LL1002: if(( unsigned int)
_temp996 > 1u?(( struct _enum_struct*) _temp996)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL1015: _temp1014=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp996)->f1; goto _LL1013; _LL1013: _temp1012=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp996)->f2; goto _LL1003;} else{ goto
_LL1004;} _LL1004: goto _LL1005; _LL999: return( void*)( _temp1006->topt)->v;
_LL1001: s= _temp1008; continue; _LL1003: s= _temp1012; continue; _LL1005:
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1016=(
char*)"statement expression must end with expression"; struct _tagged_string
_temp1017; _temp1017.curr= _temp1016; _temp1017.base= _temp1016; _temp1017.last_plus_one=
_temp1016 + 46; _temp1017;})); _LL997:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1018=( char*)"tcCodegen"; struct _tagged_string
_temp1019; _temp1019.curr= _temp1018; _temp1019.base= _temp1018; _temp1019.last_plus_one=
_temp1018 + 10; _temp1019;}));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1020=(
char*)"tcFill"; struct _tagged_string _temp1021; _temp1021.curr= _temp1020;
_temp1021.base= _temp1020; _temp1021.last_plus_one= _temp1020 + 7; _temp1021;}));}
static int Cyc_Tcexp_cnst_exp( struct Cyc_Tcenv_Tenv* te, int var_okay, struct
Cyc_Absyn_Exp* e){ void* _temp1022=( void*) e->r; void* _temp1054; void*
_temp1056; void* _temp1058; struct _tuple1* _temp1060; struct Cyc_Absyn_Exp*
_temp1062; struct Cyc_Absyn_Exp* _temp1064; struct Cyc_Absyn_Exp* _temp1066;
struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_Exp* _temp1070; struct Cyc_List_List*
_temp1072; struct Cyc_Absyn_Exp* _temp1074; struct Cyc_Absyn_Exp* _temp1076;
void* _temp1078; struct Cyc_Absyn_Exp* _temp1080; struct Cyc_List_List*
_temp1082; int _temp1084; struct Cyc_Absyn_Structdecl* _temp1086; struct Cyc_List_List*
_temp1088; struct Cyc_Core_Opt* _temp1090; struct _tuple1* _temp1092; struct Cyc_List_List*
_temp1094; void* _temp1096; struct Cyc_List_List* _temp1098; struct Cyc_Absyn_Enumfield*
_temp1100; struct Cyc_Absyn_Enumdecl* _temp1102; struct Cyc_List_List* _temp1104;
struct Cyc_Core_Opt* _temp1106; struct Cyc_Core_Opt* _temp1108; struct Cyc_Absyn_Enumfield*
_temp1110; struct Cyc_Absyn_Xenumdecl* _temp1112; struct Cyc_List_List*
_temp1114; struct Cyc_Core_Opt* _temp1116; _LL1024: if((( struct _enum_struct*)
_temp1022)->tag == Cyc_Absyn_Const_e_tag){ _LL1055: _temp1054=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp1022)->f1; goto _LL1025;} else{ goto _LL1026;}
_LL1026: if((( struct _enum_struct*) _temp1022)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL1057: _temp1056=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp1022)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: if((( struct _enum_struct*)
_temp1022)->tag == Cyc_Absyn_Var_e_tag){ _LL1061: _temp1060=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1022)->f1; goto _LL1059; _LL1059: _temp1058=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1022)->f2; goto _LL1029;} else{
goto _LL1030;} _LL1030: if((( struct _enum_struct*) _temp1022)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1067: _temp1066=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f1; goto _LL1065; _LL1065: _temp1064=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1022)->f2; goto _LL1063; _LL1063:
_temp1062=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1022)->f3; goto _LL1031;} else{ goto _LL1032;} _LL1032: if((( struct
_enum_struct*) _temp1022)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1071: _temp1070=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1022)->f1; goto
_LL1069; _LL1069: _temp1068=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1022)->f2; goto _LL1033;} else{ goto _LL1034;} _LL1034: if((( struct
_enum_struct*) _temp1022)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1075:
_temp1074=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1022)->f1; goto _LL1073; _LL1073: _temp1072=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1022)->f2; goto _LL1035;} else{
goto _LL1036;} _LL1036: if((( struct _enum_struct*) _temp1022)->tag == Cyc_Absyn_Cast_e_tag){
_LL1079: _temp1078=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1022)->f1;
goto _LL1077; _LL1077: _temp1076=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1022)->f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if((( struct
_enum_struct*) _temp1022)->tag == Cyc_Absyn_Address_e_tag){ _LL1081: _temp1080=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp1022)->f1;
goto _LL1039;} else{ goto _LL1040;} _LL1040: if((( struct _enum_struct*)
_temp1022)->tag == Cyc_Absyn_Array_e_tag){ _LL1085: _temp1084=( int)(( struct
Cyc_Absyn_Array_e_struct*) _temp1022)->f1; goto _LL1083; _LL1083: _temp1082=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1022)->f2; goto
_LL1041;} else{ goto _LL1042;} _LL1042: if((( struct _enum_struct*) _temp1022)->tag
== Cyc_Absyn_Struct_e_tag){ _LL1093: _temp1092=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1022)->f1; goto _LL1091; _LL1091: _temp1090=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1022)->f2; goto _LL1089; _LL1089:
_temp1088=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1022)->f3; goto _LL1087; _LL1087: _temp1086=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1022)->f4; goto _LL1043;} else{ goto
_LL1044;} _LL1044: if((( struct _enum_struct*) _temp1022)->tag == Cyc_Absyn_Primop_e_tag){
_LL1097: _temp1096=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1022)->f1;
goto _LL1095; _LL1095: _temp1094=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1022)->f2; goto _LL1045;} else{ goto _LL1046;} _LL1046: if((( struct
_enum_struct*) _temp1022)->tag == Cyc_Absyn_Tuple_e_tag){ _LL1099: _temp1098=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1022)->f1; goto
_LL1047;} else{ goto _LL1048;} _LL1048: if((( struct _enum_struct*) _temp1022)->tag
== Cyc_Absyn_Enum_e_tag){ _LL1109: _temp1108=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp1022)->f1; goto _LL1107; _LL1107: _temp1106=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1022)->f2; goto
_LL1105; _LL1105: _temp1104=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1022)->f3; goto _LL1103; _LL1103: _temp1102=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1022)->f4; goto _LL1101; _LL1101:
_temp1100=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1022)->f5; goto _LL1049;} else{ goto _LL1050;} _LL1050: if((( struct
_enum_struct*) _temp1022)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1117: _temp1116=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp1022)->f1; goto
_LL1115; _LL1115: _temp1114=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1022)->f2; goto _LL1113; _LL1113: _temp1112=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp1022)->f3; goto _LL1111; _LL1111:
_temp1110=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1022)->f4; goto _LL1051;} else{ goto _LL1052;} _LL1052: goto _LL1053;
_LL1025: return 1; _LL1027: return 1; _LL1029: { struct _handler_cons _temp1118;
_push_handler(& _temp1118);{ struct _xenum_struct* _temp1119=( struct
_xenum_struct*) setjmp( _temp1118.handler); if( ! _temp1119){{ void* _temp1120=
Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp1060); void* _temp1128; struct Cyc_Absyn_Fndecl*
_temp1130; void* _temp1132; struct Cyc_Absyn_Vardecl* _temp1134; _LL1122: if(((
struct _enum_struct*) _temp1120)->tag == Cyc_Tcenv_VarRes_tag){ _LL1129:
_temp1128=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp1120)->f1; if((
unsigned int) _temp1128 > 1u?(( struct _enum_struct*) _temp1128)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL1131: _temp1130=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp1128)->f1; goto _LL1123;} else{ goto _LL1124;}} else{ goto _LL1124;}
_LL1124: if((( struct _enum_struct*) _temp1120)->tag == Cyc_Tcenv_VarRes_tag){
_LL1133: _temp1132=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp1120)->f1;
if(( unsigned int) _temp1132 > 1u?(( struct _enum_struct*) _temp1132)->tag ==
Cyc_Absyn_Global_b_tag: 0){ _LL1135: _temp1134=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp1132)->f1; goto _LL1125;} else{ goto
_LL1126;}} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1123: { int _temp1136=
1; _npop_handler( 0u); return _temp1136;} _LL1125: { void* _temp1137= Cyc_Tcutil_compress((
void*) _temp1134->type); struct Cyc_Absyn_Exp* _temp1143; struct Cyc_Absyn_Tqual*
_temp1145; void* _temp1147; _LL1139: if(( unsigned int) _temp1137 > 5u?(( struct
_enum_struct*) _temp1137)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1148:
_temp1147=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1137)->f1; goto
_LL1146; _LL1146: _temp1145=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1137)->f2; goto _LL1144; _LL1144: _temp1143=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1137)->f3; goto _LL1140;} else{ goto
_LL1141;} _LL1141: goto _LL1142; _LL1140: { int _temp1149= 1; _npop_handler( 0u);
return _temp1149;} _LL1142: { int _temp1150= var_okay; _npop_handler( 0u);
return _temp1150;} _LL1138:;} _LL1127: { int _temp1151= var_okay; _npop_handler(
0u); return _temp1151;} _LL1121:;}; _pop_handler();} else{ struct _xenum_struct*
_temp1153= _temp1119; _LL1155: if( _temp1153->tag == Cyc_Dict_Absent_tag){ goto
_LL1156;} else{ goto _LL1157;} _LL1157: goto _LL1158; _LL1156: return 0; _LL1158:(
void) _throw( _temp1153); _LL1154:;}}} _LL1031: return( Cyc_Tcexp_cnst_exp( te,
0, _temp1066)? Cyc_Tcexp_cnst_exp( te, 0, _temp1064): 0)? Cyc_Tcexp_cnst_exp( te,
0, _temp1062): 0; _LL1033: return Cyc_Tcexp_cnst_exp( te, 0, _temp1070)? Cyc_Tcexp_cnst_exp(
te, 0, _temp1068): 0; _LL1035: return Cyc_Tcexp_cnst_exp( te, var_okay,
_temp1074); _LL1037: return Cyc_Tcexp_cnst_exp( te, var_okay, _temp1076);
_LL1039: return Cyc_Tcexp_cnst_exp( te, 1, _temp1080); _LL1041: _temp1088=
_temp1082; goto _LL1043; _LL1043: for( 0; _temp1088 != 0; _temp1088= _temp1088->tl){
if( ! Cyc_Tcexp_cnst_exp( te, 0,(*(( struct _tuple6*) _temp1088->hd)).f2)){
return 0;}} return 1; _LL1045: _temp1098= _temp1094; goto _LL1047; _LL1047:
_temp1104= _temp1098; goto _LL1049; _LL1049: _temp1114= _temp1104; goto _LL1051;
_LL1051: for( 0; _temp1114 != 0; _temp1114= _temp1114->tl){ if( ! Cyc_Tcexp_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*) _temp1114->hd)){ return 0;}} return 1; _LL1053:
return 0; _LL1023:;} int Cyc_Tcexp_is_const_exp( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ return Cyc_Tcexp_cnst_exp( te, 0, e);} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1159= t; struct Cyc_Absyn_Exp*
_temp1165; struct Cyc_Absyn_Tqual* _temp1167; void* _temp1169; _LL1161: if((
unsigned int) _temp1159 > 5u?(( struct _enum_struct*) _temp1159)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1170: _temp1169=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f1;
goto _LL1168; _LL1168: _temp1167=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1159)->f2; goto _LL1166; _LL1166: _temp1165=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f3; goto _LL1162;} else{ goto
_LL1163;} _LL1163: goto _LL1164; _LL1162: { void* _temp1173; int _temp1175;
struct _tuple5 _temp1171= Cyc_Tcutil_addressof_props( te, e); _LL1176: _temp1175=
_temp1171.f1; goto _LL1174; _LL1174: _temp1173= _temp1171.f2; goto _LL1172;
_LL1172: t= Cyc_Absyn_atb_typ( _temp1169, _temp1173, _temp1167,({ struct Cyc_Absyn_Upper_b_struct*
_temp1177=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1177->tag= Cyc_Absyn_Upper_b_tag; _temp1177->f1= _temp1165;( void*)
_temp1177;})); Cyc_Tcutil_unchecked_cast( te, e, t); return t;} _LL1164: return
t; _LL1160:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1178=( void*) e->r; struct Cyc_List_List* _temp1186; int _temp1188;
void* _temp1190; struct _tagged_string _temp1192; int _temp1194; _LL1180: if(((
struct _enum_struct*) _temp1178)->tag == Cyc_Absyn_Array_e_tag){ _LL1189:
_temp1188=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp1178)->f1; goto
_LL1187; _LL1187: _temp1186=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1178)->f2; goto _LL1181;} else{ goto _LL1182;} _LL1182: if((( struct
_enum_struct*) _temp1178)->tag == Cyc_Absyn_Const_e_tag){ _LL1191: _temp1190=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1178)->f1; if(( unsigned int)
_temp1190 > 1u?(( struct _enum_struct*) _temp1190)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1195: _temp1194=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1190)->f1;
goto _LL1193; _LL1193: _temp1192=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1190)->f2; goto _LL1183;} else{ goto _LL1184;}} else{ goto _LL1184;}
_LL1184: goto _LL1185; _LL1181: return t; _LL1183: return t; _LL1185: t= Cyc_Tcutil_compress(
t);{ void* _temp1196= t; struct Cyc_Absyn_Exp* _temp1202; struct Cyc_Absyn_Tqual*
_temp1204; void* _temp1206; _LL1198: if(( unsigned int) _temp1196 > 5u?(( struct
_enum_struct*) _temp1196)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1207:
_temp1206=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1196)->f1; goto
_LL1205; _LL1205: _temp1204=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1196)->f2; goto _LL1203; _LL1203: _temp1202=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1196)->f3; goto _LL1199;} else{ goto
_LL1200;} _LL1200: goto _LL1201; _LL1199: { void* _temp1210; int _temp1212;
struct _tuple5 _temp1208= Cyc_Tcutil_addressof_props( te, e); _LL1213: _temp1212=
_temp1208.f1; goto _LL1211; _LL1211: _temp1210= _temp1208.f2; goto _LL1209;
_LL1209: t= Cyc_Absyn_atb_typ( _temp1206, _temp1210, _temp1204,({ struct Cyc_Absyn_Upper_b_struct*
_temp1214=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1214->tag= Cyc_Absyn_Upper_b_tag; _temp1214->f1= _temp1202;( void*)
_temp1214;})); Cyc_Tcutil_unchecked_cast( te, e, t); return t;} _LL1201: return
t; _LL1197:;} _LL1179:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1215=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1221; _LL1217: if((( struct _enum_struct*) _temp1215)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL1222: _temp1221=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1215)->f1; goto _LL1218;} else{
goto _LL1219;} _LL1219: goto _LL1220; _LL1218: Cyc_Tcexp_tcExpNoInst( te, topt,
_temp1221); e->topt= _temp1221->topt; goto _LL1216; _LL1220: Cyc_Tcexp_tcExpNoInst(
te, topt, e);( void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* _temp1223= Cyc_Tcutil_compress(( void*)( e->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1229; struct Cyc_Absyn_Conref* _temp1231; struct
Cyc_Absyn_Tqual* _temp1233; struct Cyc_Absyn_Conref* _temp1235; void* _temp1237;
void* _temp1239; _LL1225: if(( unsigned int) _temp1223 > 5u?(( struct
_enum_struct*) _temp1223)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1230:
_temp1229=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1223)->f1; _LL1240: _temp1239=( void*) _temp1229.elt_typ; goto _LL1238;
_LL1238: _temp1237=( void*) _temp1229.rgn_typ; goto _LL1236; _LL1236: _temp1235=(
struct Cyc_Absyn_Conref*) _temp1229.nullable; goto _LL1234; _LL1234: _temp1233=(
struct Cyc_Absyn_Tqual*) _temp1229.tq; goto _LL1232; _LL1232: _temp1231=( struct
Cyc_Absyn_Conref*) _temp1229.bounds; goto _LL1226;} else{ goto _LL1227;} _LL1227:
goto _LL1228; _LL1226:{ void* _temp1241= Cyc_Tcutil_compress( _temp1239); struct
Cyc_Absyn_FnInfo _temp1247; struct Cyc_List_List* _temp1249; int _temp1251;
struct Cyc_List_List* _temp1253; void* _temp1255; struct Cyc_Core_Opt* _temp1257;
struct Cyc_List_List* _temp1259; _LL1243: if(( unsigned int) _temp1241 > 5u?((
struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1248:
_temp1247=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1241)->f1; _LL1260: _temp1259=( struct Cyc_List_List*) _temp1247.tvars;
goto _LL1258; _LL1258: _temp1257=( struct Cyc_Core_Opt*) _temp1247.effect; goto
_LL1256; _LL1256: _temp1255=( void*) _temp1247.ret_typ; goto _LL1254; _LL1254:
_temp1253=( struct Cyc_List_List*) _temp1247.args; goto _LL1252; _LL1252:
_temp1251=( int) _temp1247.varargs; goto _LL1250; _LL1250: _temp1249=( struct
Cyc_List_List*) _temp1247.attributes; goto _LL1244;} else{ goto _LL1245;}
_LL1245: goto _LL1246; _LL1244: if( _temp1259 != 0){ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1259); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,({ struct Cyc_Absyn_FnType_struct*
_temp1261=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1261->tag= Cyc_Absyn_FnType_tag; _temp1261->f1=({ struct Cyc_Absyn_FnInfo
_temp1262; _temp1262.tvars= 0; _temp1262.effect= _temp1257; _temp1262.ret_typ=(
void*) _temp1255; _temp1262.args= _temp1253; _temp1262.varargs= _temp1251;
_temp1262.attributes= _temp1249; _temp1262;});( void*) _temp1261;})); void*
new_typ=({ struct Cyc_Absyn_PointerType_struct* _temp1263=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1263->tag= Cyc_Absyn_PointerType_tag;
_temp1263->f1=({ struct Cyc_Absyn_PtrInfo _temp1264; _temp1264.elt_typ=( void*)
ftyp; _temp1264.rgn_typ=( void*) _temp1237; _temp1264.nullable= _temp1235;
_temp1264.tq= _temp1233; _temp1264.bounds= _temp1231; _temp1264;});( void*)
_temp1263;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1265=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1265->tag= Cyc_Absyn_Instantiate_e_tag;
_temp1265->f1= inner; _temp1265->f2= ts;( void*) _temp1265;})); e->topt=({
struct Cyc_Core_Opt* _temp1266=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1266->v=( void*) new_typ; _temp1266;});} goto _LL1242;
_LL1246: goto _LL1242; _LL1242:;} goto _LL1224; _LL1228: goto _LL1224; _LL1224:;}
goto _LL1216; _LL1216:;} return( void*)( e->topt)->v;} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1267=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1333; struct _tuple1* _temp1335; struct Cyc_List_List* _temp1337; struct
Cyc_Absyn_Exp* _temp1339; struct Cyc_List_List* _temp1341; struct Cyc_Core_Opt*
_temp1343; void* _temp1345; void* _temp1347; struct _tuple1* _temp1349; struct
Cyc_List_List* _temp1351; void* _temp1353; void* _temp1355; struct Cyc_Absyn_Exp*
_temp1357; struct Cyc_Absyn_Exp* _temp1359; struct Cyc_Core_Opt* _temp1361;
struct Cyc_Absyn_Exp* _temp1363; struct Cyc_Absyn_Exp* _temp1365; struct Cyc_Absyn_Exp*
_temp1367; struct Cyc_Absyn_Exp* _temp1369; struct Cyc_Absyn_Exp* _temp1371;
struct Cyc_Absyn_Exp* _temp1373; struct Cyc_List_List* _temp1375; struct Cyc_Absyn_Exp*
_temp1377; struct Cyc_Absyn_Exp* _temp1379; struct Cyc_List_List* _temp1381;
struct Cyc_Absyn_Exp* _temp1383; struct Cyc_Absyn_Exp* _temp1385; void*
_temp1387; struct Cyc_Absyn_Exp* _temp1389; void* _temp1391; struct Cyc_Absyn_Exp*
_temp1393; struct _tagged_string* _temp1395; struct Cyc_Absyn_Exp* _temp1397;
struct _tagged_string* _temp1399; struct Cyc_Absyn_Exp* _temp1401; struct Cyc_Absyn_Exp*
_temp1403; struct Cyc_Absyn_Exp* _temp1405; struct Cyc_List_List* _temp1407;
struct Cyc_List_List* _temp1409; struct _tuple2* _temp1411; struct Cyc_List_List*
_temp1413; int _temp1415; struct Cyc_Absyn_Stmt* _temp1417; struct Cyc_Absyn_Fndecl*
_temp1419; struct Cyc_Absyn_Exp* _temp1421; struct Cyc_Absyn_Exp* _temp1423;
struct Cyc_Absyn_Exp* _temp1425; struct Cyc_Absyn_Vardecl* _temp1427; struct Cyc_Absyn_Structdecl*
_temp1429; struct Cyc_Absyn_Structdecl** _temp1431; struct Cyc_List_List*
_temp1432; struct Cyc_Core_Opt* _temp1434; struct Cyc_Core_Opt** _temp1436;
struct _tuple1* _temp1437; struct _tuple1** _temp1439; struct Cyc_Absyn_Enumfield*
_temp1440; struct Cyc_Absyn_Enumdecl* _temp1442; struct Cyc_List_List* _temp1444;
struct Cyc_Core_Opt* _temp1446; struct Cyc_Core_Opt** _temp1448; struct Cyc_Core_Opt*
_temp1449; struct Cyc_Core_Opt** _temp1451; struct Cyc_Absyn_Enumfield*
_temp1452; struct Cyc_Absyn_Xenumdecl* _temp1454; struct Cyc_List_List*
_temp1456; struct Cyc_Core_Opt* _temp1458; struct Cyc_Core_Opt** _temp1460; void*
_temp1461; void** _temp1463; _LL1269: if((( struct _enum_struct*) _temp1267)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL1334: _temp1333=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1267)->f1; goto _LL1270;} else{
goto _LL1271;} _LL1271: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL1336: _temp1335=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1267)->f1; goto _LL1272;} else{ goto _LL1273;} _LL1273: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1340:
_temp1339=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1267)->f1; goto _LL1338; _LL1338: _temp1337=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1267)->f2; goto _LL1274;} else{
goto _LL1275;} _LL1275: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1344: _temp1343=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1267)->f1; goto _LL1342; _LL1342: _temp1341=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1267)->f2; goto _LL1276;} else{
goto _LL1277;} _LL1277: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Const_e_tag){
_LL1346: _temp1345=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1267)->f1;
goto _LL1278;} else{ goto _LL1279;} _LL1279: if((( struct _enum_struct*)
_temp1267)->tag == Cyc_Absyn_Var_e_tag){ _LL1350: _temp1349=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1267)->f1; goto _LL1348; _LL1348: _temp1347=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1267)->f2; goto _LL1280;} else{
goto _LL1281;} _LL1281: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Primop_e_tag){
_LL1354: _temp1353=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f1;
goto _LL1352; _LL1352: _temp1351=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1267)->f2; goto _LL1282;} else{ goto _LL1283;} _LL1283: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Increment_e_tag){ _LL1358: _temp1357=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1267)->f1;
goto _LL1356; _LL1356: _temp1355=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1267)->f2; goto _LL1284;} else{ goto _LL1285;} _LL1285: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1364: _temp1363=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1267)->f1;
goto _LL1362; _LL1362: _temp1361=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1267)->f2; goto _LL1360; _LL1360: _temp1359=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1267)->f3; goto _LL1286;} else{ goto
_LL1287;} _LL1287: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1370: _temp1369=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1267)->f1; goto _LL1368; _LL1368: _temp1367=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1267)->f2; goto _LL1366; _LL1366:
_temp1365=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1267)->f3; goto _LL1288;} else{ goto _LL1289;} _LL1289: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1374: _temp1373=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1267)->f1; goto
_LL1372; _LL1372: _temp1371=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1267)->f2; goto _LL1290;} else{ goto _LL1291;} _LL1291: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1378: _temp1377=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1267)->f1; goto
_LL1376; _LL1376: _temp1375=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1267)->f2; goto _LL1292;} else{ goto _LL1293;} _LL1293: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Throw_e_tag){ _LL1380: _temp1379=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1267)->f1; goto
_LL1294;} else{ goto _LL1295;} _LL1295: if((( struct _enum_struct*) _temp1267)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1384: _temp1383=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1267)->f1; goto _LL1382; _LL1382:
_temp1381=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1267)->f2; goto _LL1296;} else{ goto _LL1297;} _LL1297: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Cast_e_tag){ _LL1388: _temp1387=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1267)->f1; goto _LL1386; _LL1386:
_temp1385=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1267)->f2;
goto _LL1298;} else{ goto _LL1299;} _LL1299: if((( struct _enum_struct*)
_temp1267)->tag == Cyc_Absyn_Address_e_tag){ _LL1390: _temp1389=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1267)->f1; goto _LL1300;} else{ goto
_LL1301;} _LL1301: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL1392: _temp1391=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp1267)->f1;
goto _LL1302;} else{ goto _LL1303;} _LL1303: if((( struct _enum_struct*)
_temp1267)->tag == Cyc_Absyn_Deref_e_tag){ _LL1394: _temp1393=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1267)->f1; goto _LL1304;} else{ goto
_LL1305;} _LL1305: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1398: _temp1397=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1267)->f1; goto _LL1396; _LL1396: _temp1395=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1267)->f2; goto _LL1306;} else{
goto _LL1307;} _LL1307: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1402: _temp1401=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1267)->f1; goto _LL1400; _LL1400: _temp1399=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1267)->f2; goto _LL1308;} else{
goto _LL1309;} _LL1309: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1406: _temp1405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1267)->f1; goto _LL1404; _LL1404: _temp1403=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1267)->f2; goto _LL1310;} else{ goto
_LL1311;} _LL1311: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1408: _temp1407=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1267)->f1; goto _LL1312;} else{ goto _LL1313;} _LL1313: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1412:
_temp1411=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1267)->f1;
goto _LL1410; _LL1410: _temp1409=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1267)->f2; goto _LL1314;} else{ goto _LL1315;} _LL1315: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Array_e_tag){ _LL1416: _temp1415=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp1267)->f1; goto _LL1414; _LL1414:
_temp1413=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1267)->f2;
goto _LL1316;} else{ goto _LL1317;} _LL1317: if((( struct _enum_struct*)
_temp1267)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1418: _temp1417=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1267)->f1; goto _LL1318;} else{ goto
_LL1319;} _LL1319: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1420: _temp1419=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1267)->f1; goto _LL1320;} else{ goto _LL1321;} _LL1321: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Fill_e_tag){ _LL1422: _temp1421=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1267)->f1; goto
_LL1322;} else{ goto _LL1323;} _LL1323: if((( struct _enum_struct*) _temp1267)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1428: _temp1427=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1267)->f1; goto _LL1426; _LL1426:
_temp1425=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1267)->f2; goto _LL1424; _LL1424: _temp1423=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1267)->f3; goto _LL1324;} else{
goto _LL1325;} _LL1325: if((( struct _enum_struct*) _temp1267)->tag == Cyc_Absyn_Struct_e_tag){
_LL1438: _temp1437=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1267)->f1; _temp1439=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1267)->f1;
goto _LL1435; _LL1435: _temp1434=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1267)->f2; _temp1436=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1267)->f2;
goto _LL1433; _LL1433: _temp1432=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1267)->f3; goto _LL1430; _LL1430: _temp1429=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1267)->f4; _temp1431=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1267)->f4; goto _LL1326;} else{ goto _LL1327;} _LL1327: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Enum_e_tag){ _LL1450: _temp1449=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1267)->f1;
_temp1451=&(( struct Cyc_Absyn_Enum_e_struct*) _temp1267)->f1; goto _LL1447;
_LL1447: _temp1446=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1267)->f2; _temp1448=&(( struct Cyc_Absyn_Enum_e_struct*) _temp1267)->f2;
goto _LL1445; _LL1445: _temp1444=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1267)->f3; goto _LL1443; _LL1443: _temp1442=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1267)->f4; goto _LL1441; _LL1441:
_temp1440=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1267)->f5; goto _LL1328;} else{ goto _LL1329;} _LL1329: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1459: _temp1458=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp1267)->f1;
_temp1460=&(( struct Cyc_Absyn_Xenum_e_struct*) _temp1267)->f1; goto _LL1457;
_LL1457: _temp1456=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1267)->f2; goto _LL1455; _LL1455: _temp1454=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp1267)->f3; goto _LL1453; _LL1453:
_temp1452=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1267)->f4; goto _LL1330;} else{ goto _LL1331;} _LL1331: if((( struct
_enum_struct*) _temp1267)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1462: _temp1461=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1267)->f1; _temp1463=&(( struct
Cyc_Absyn_Malloc_e_struct*) _temp1267)->f1; goto _LL1332;} else{ goto _LL1268;}
_LL1270: Cyc_Tcexp_tcExpNoInst( te, 0, _temp1333); return; _LL1272: Cyc_Tcexp_resolve_unknown_id(
te, e, _temp1335); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1274: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1339, _temp1337); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1276: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1341); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1278: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1345, e);
goto _LL1268; _LL1280: t= Cyc_Tcexp_tcVar( te, loc, _temp1349, _temp1347); goto
_LL1268; _LL1282: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1353, _temp1351);
goto _LL1268; _LL1284: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1357,
_temp1355); goto _LL1268; _LL1286: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1363, _temp1361, _temp1359); goto _LL1268; _LL1288: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1369, _temp1367, _temp1365); goto _LL1268; _LL1290: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1373, _temp1371); goto _LL1268; _LL1292: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1377, _temp1375); goto _LL1268; _LL1294: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1379); goto _LL1268; _LL1296: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1383, _temp1381); goto _LL1268; _LL1298: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1387, _temp1385); goto _LL1268; _LL1300: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1389); goto _LL1268; _LL1302: t= Cyc_Tcexp_tcSizeof( te, loc,
topt, _temp1391); goto _LL1268; _LL1304: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp1393); goto _LL1268; _LL1306: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp1397, _temp1395); goto _LL1268; _LL1308: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp1401, _temp1399); goto _LL1268; _LL1310: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1405, _temp1403); goto _LL1268; _LL1312: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1407); goto _LL1268; _LL1314: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1411, _temp1409); goto _LL1268; _LL1316: t= Cyc_Tcexp_tcArray(
te, loc, topt, e, _temp1415, _temp1413); goto _LL1268; _LL1318: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1417); goto _LL1268; _LL1320: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1419); goto _LL1268; _LL1322: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1421); goto _LL1268; _LL1324: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1427, _temp1425, _temp1423); goto _LL1268; _LL1326: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1439, _temp1436, _temp1432, _temp1431); goto _LL1268;
_LL1328: t= Cyc_Tcexp_tcEnum( te, loc, topt, _temp1451, _temp1448, _temp1444,
_temp1442, _temp1440); goto _LL1268; _LL1330: t= Cyc_Tcexp_tcXenum( te, loc,
topt, _temp1460, _temp1456, _temp1454, _temp1452); goto _LL1268; _LL1332: t= Cyc_Tcexp_tcMalloc(
te, loc, topt, _temp1463); goto _LL1268; _LL1268:;} e->topt=({ struct Cyc_Core_Opt*
_temp1464=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1464->v=( void*) t; _temp1464;});}