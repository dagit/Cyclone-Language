#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ void*
f1; int f2; } ; struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; struct _tuple8{ struct Cyc_Absyn_Xenumdecl* f1; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
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
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_mem( int(* compare)( void*, void*), struct
Cyc_List_List* l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c( int(*
f)( void*, void*), void* env, struct Cyc_List_List* l); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
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
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
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
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_List_List* tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct
Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct
Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tagged_string*,
struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
struct Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string*
v, struct _tagged_string new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} static void* Cyc_Tc_scope_redecl_okay( void*
s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct
Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string* v){{
struct _tuple3 _temp5=({ struct _tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2;
_temp4;}); void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; void* _temp51; void*
_temp53; void* _temp55; void* _temp57; void* _temp59; void* _temp61; void*
_temp63; void* _temp65; void* _temp68; void* _temp70; void* _temp72; void*
_temp74; void* _temp76; void* _temp78; void* _temp80; void* _temp82; void*
_temp85; void* _temp87; void* _temp89; void* _temp91; _LL7: _LL38: _temp37=
_temp5.f1; if( _temp37 == Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;}
_LL36: _temp35= _temp5.f2; if( _temp35 == Cyc_Absyn_ExternC){ goto _LL8;} else{
goto _LL9;} _LL9: _LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39= _temp5.f2;
if( _temp39 == Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11: _LL46:
_temp45= _temp5.f1; if( _temp45 == Cyc_Absyn_Static){ goto _LL44;} else{ goto
_LL13;} _LL44: _temp43= _temp5.f2; if( _temp43 == Cyc_Absyn_Static){ goto _LL12;}
else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1; goto _LL48; _LL48: _temp47=
_temp5.f2; if( _temp47 == Cyc_Absyn_Static){ goto _LL14;} else{ goto _LL15;}
_LL15: _LL54: _temp53= _temp5.f1; if( _temp53 == Cyc_Absyn_Static){ goto _LL52;}
else{ goto _LL17;} _LL52: _temp51= _temp5.f2; if( _temp51 == Cyc_Absyn_Public){
goto _LL16;} else{ goto _LL17;} _LL17: _LL58: _temp57= _temp5.f1; if( _temp57 ==
Cyc_Absyn_Abstract){ goto _LL56;} else{ goto _LL19;} _LL56: _temp55= _temp5.f2;
if( _temp55 == Cyc_Absyn_Public){ goto _LL18;} else{ goto _LL19;} _LL19: _LL62:
_temp61= _temp5.f1; goto _LL60; _LL60: _temp59= _temp5.f2; if( _temp59 == Cyc_Absyn_Public){
goto _LL20;} else{ goto _LL21;} _LL21: _LL66: _temp65= _temp5.f1; if( _temp65 ==
Cyc_Absyn_Static){ goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2;
if( _temp63 == Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71:
_temp70= _temp5.f1; if( _temp70 == Cyc_Absyn_Abstract){ goto _LL69;} else{ goto
_LL25;} _LL69: _temp68= _temp5.f2; if( _temp68 == Cyc_Absyn_Extern){ goto _LL67;}
else{ goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;}
_LL25: _LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(
_temp72 == Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27: _LL79:
_temp78= _temp5.f1; if( _temp78 == Cyc_Absyn_Static){ goto _LL77;} else{ goto
_LL29;} _LL77: _temp76= _temp5.f2; if( _temp76 == Cyc_Absyn_Abstract){ goto
_LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if( _temp82 == Cyc_Absyn_Public){
goto _LL81;} else{ goto _LL31;} _LL81: _temp80= _temp5.f2; if( _temp80 == Cyc_Absyn_Abstract){
goto _LL30;} else{ goto _LL31;} _LL31: _LL88: _temp87= _temp5.f1; if( _temp87 ==
Cyc_Absyn_Extern){ goto _LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2;
if( _temp85 == Cyc_Absyn_Abstract){ goto _LL84;} else{ goto _LL33;} _LL84: if(
fields1 != 0){ goto _LL32;} else{ goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1;
goto _LL90; _LL90: _temp89= _temp5.f2; if( _temp89 == Cyc_Absyn_Abstract){ goto
_LL34;} else{ goto _LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,(
struct _tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1= Cyc_Absyn_Public; goto _LL6; _LL22: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static"; struct
_tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1= Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} static void Cyc_Tc_field_redecl_okay( struct Cyc_Absyn_Enumfield* f1,
struct Cyc_Absyn_Enumfield* f2, struct Cyc_List_List* inst, struct Cyc_Tcenv_Tenv*
te, struct _tagged_string t, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp129= t; struct _tagged_string
_temp130=* v; struct _tagged_string _temp131=*(* f1->name).f2; struct
_tagged_string _temp132=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp129.last_plus_one - _temp129.curr, _temp129.curr, _temp130.last_plus_one -
_temp130.curr, _temp130.curr, _temp131.last_plus_one - _temp131.curr, _temp131.curr,
_temp132.last_plus_one - _temp132.curr, _temp132.curr);}));} if((( f1->tag == 0?
f2->tag != 0: 0)? 1:( f1->tag != 0? f2->tag == 0: 0))? 1:(( f1->tag != 0? f2->tag
!= 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp133= f1->tag; if( _temp133 == 0){ _throw( Null_Exception);} _temp133;})) !=
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp134= f2->tag; if( _temp134 == 0){ _throw( Null_Exception);} _temp134;})): 0)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp135= t; struct _tagged_string
_temp136=* v; struct _tagged_string _temp137=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s: tag mismatch for field %.*s",
_temp135.last_plus_one - _temp135.curr, _temp135.curr, _temp136.last_plus_one -
_temp136.curr, _temp136.curr, _temp137.last_plus_one - _temp137.curr, _temp137.curr);}));}{
struct Cyc_List_List* tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs1) !=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp138= t; struct _tagged_string _temp139=* v; struct
_tagged_string _temp140=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp138.last_plus_one - _temp138.curr, _temp138.curr, _temp139.last_plus_one -
_temp139.curr, _temp139.curr, _temp140.last_plus_one - _temp140.curr, _temp140.curr);}));}
for( 0; tvs1 != 0; tvs1= tvs1->tl, tvs2= tvs2->tl){ inst=({ struct Cyc_List_List*
_temp141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp141->hd=( void*)({ struct _tuple4* _temp142=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp142->f1=( struct Cyc_Absyn_Tvar*) tvs2->hd;
_temp142->f2=({ struct Cyc_Absyn_VarType_struct* _temp143=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp143->tag= Cyc_Absyn_VarType_tag;
_temp143->f1=( struct Cyc_Absyn_Tvar*) tvs1->hd;( void*) _temp143;}); _temp142;});
_temp141->tl= inst; _temp141;});}{ struct Cyc_List_List* typs1= f1->typs; struct
Cyc_List_List* typs2= f2->typs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
typs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( typs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp144= t; struct _tagged_string _temp145=* v;
struct _tagged_string _temp146=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp144.last_plus_one - _temp144.curr, _temp144.curr, _temp145.last_plus_one -
_temp145.curr, _temp145.curr, _temp146.last_plus_one - _temp146.curr, _temp146.curr);}));}
for( 0; typs1 != 0; typs1= typs1->tl, typs2= typs2->tl){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple5*) typs1->hd)).f1,(*(( struct _tuple5*) typs2->hd)).f1)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp147= t; struct _tagged_string _temp148=* v;
struct _tagged_string _temp149=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp147.last_plus_one - _temp147.curr, _temp147.curr, _temp148.last_plus_one -
_temp148.curr, _temp148.curr, _temp149.last_plus_one - _temp149.curr, _temp149.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*) typs2->hd)).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*) typs1->hd)).f2, subst_t2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp150= t; struct _tagged_string _temp151=* v;
struct _tagged_string _temp152=*(* f1->name).f2; struct _tagged_string _temp153=
Cyc_Absynpp_typ2string((*(( struct _tuple5*) typs1->hd)).f2); struct
_tagged_string _temp154= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp150.last_plus_one - _temp150.curr, _temp150.curr, _temp151.last_plus_one -
_temp151.curr, _temp151.curr, _temp152.last_plus_one - _temp152.curr, _temp152.curr,
_temp153.last_plus_one - _temp153.curr, _temp153.curr, _temp154.last_plus_one -
_temp154.curr, _temp154.curr);}));}}}}}} static int Cyc_Tc_var_redecl_okay(
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts){ struct Cyc_Core_Opt*
ans=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)( ge->ordinaries, name); if( ans == 0){ return 0;}(*((
struct _tuple6*) ans->v)).f2= 1;{ void* _temp155=(*(( struct _tuple6*) ans->v)).f1;
void* _temp161; struct Cyc_Absyn_Vardecl* _temp163; _LL157: if((( struct
_enum_struct*) _temp155)->tag == Cyc_Tcenv_VarRes_tag){ _LL162: _temp161=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp155)->f1; if(( unsigned int) _temp161 > 1u?((
struct _enum_struct*) _temp161)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL164:
_temp163=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp161)->f1; goto _LL158;} else{ goto _LL159;}} else{ goto _LL159;} _LL159:
goto _LL160; _LL158: if( sc == Cyc_Absyn_Static?( void*) _temp163->sc != Cyc_Absyn_Static:
0){ Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp165=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp165.last_plus_one - _temp165.curr, _temp165.curr);}));} if(( void*)
_temp163->sc == Cyc_Absyn_Static? sc != Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn(
loc,({ struct _tagged_string _temp166=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp166.last_plus_one - _temp166.curr, _temp166.curr);}));} if((( void*)
_temp163->sc == Cyc_Absyn_ExternC? sc != Cyc_Absyn_ExternC: 0)? 1:(( void*)
_temp163->sc != Cyc_Absyn_ExternC? sc == Cyc_Absyn_ExternC: 0)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp167=* name; xprintf("incompatible redeclaration of %.*s",
_temp167.last_plus_one - _temp167.curr, _temp167.curr);}));} else{ if((( sc !=
Cyc_Absyn_Extern?( void*) _temp163->sc != Cyc_Absyn_Extern: 0)? !( sc == Cyc_Absyn_Static?(
void*) _temp163->sc == Cyc_Absyn_Static: 0): 0)? !( sc == Cyc_Absyn_Public?(
void*) _temp163->sc == Cyc_Absyn_Public: 0): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp168=* name; xprintf("incompatible redeclaration of %.*s",
_temp168.last_plus_one - _temp168.curr, _temp168.curr);}));}} if( ! Cyc_Tcutil_unify((
void*) _temp163->type, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp169=* name; struct _tagged_string _temp170= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp171= Cyc_Absynpp_typ2string(( void*) _temp163->type);
xprintf("redeclaration of %.*s at a different type: %.*s != %.*s", _temp169.last_plus_one
- _temp169.curr, _temp169.curr, _temp170.last_plus_one - _temp170.curr, _temp170.curr,
_temp171.last_plus_one - _temp171.curr, _temp171.curr);}));} if( ! Cyc_Tcutil_same_atts(
atts, _temp163->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp172=* name; xprintf("redeclaration of %.*s with different attributes",
_temp172.last_plus_one - _temp172.curr, _temp172.curr);}));} goto _LL156; _LL160:
goto _LL156; _LL156:;} return 1;} static int Cyc_Tc_is_function_type( void* t){
void* _temp173= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp179;
_LL175: if(( unsigned int) _temp173 > 4u?(( struct _enum_struct*) _temp173)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL180: _temp179=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp173)->f1; goto _LL176;} else{ goto _LL177;}
_LL177: goto _LL178; _LL176: return 1; _LL178: return 0; _LL174:;} static struct
Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List* atts){
void* _temp181= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp187;
struct Cyc_List_List* _temp189; struct Cyc_List_List** _temp191; int _temp192;
struct Cyc_List_List* _temp194; void* _temp196; struct Cyc_Core_Opt* _temp198;
struct Cyc_List_List* _temp200; _LL183: if(( unsigned int) _temp181 > 4u?((
struct _enum_struct*) _temp181)->tag == Cyc_Absyn_FnType_tag: 0){ _LL188:
_temp187=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp181)->f1;
_LL201: _temp200=( struct Cyc_List_List*) _temp187.tvars; goto _LL199; _LL199:
_temp198=( struct Cyc_Core_Opt*) _temp187.effect; goto _LL197; _LL197: _temp196=(
void*) _temp187.ret_typ; goto _LL195; _LL195: _temp194=( struct Cyc_List_List*)
_temp187.args; goto _LL193; _LL193: _temp192=( int) _temp187.varargs; goto
_LL190; _LL190: _temp189=( struct Cyc_List_List*) _temp187.attributes; _temp191=&(((
struct Cyc_Absyn_FnType_struct*) _temp181)->f1).attributes; goto _LL184;} else{
goto _LL185;} _LL185: goto _LL186; _LL184: { struct Cyc_List_List* res_atts= 0;
for( 0; atts != 0; atts= atts->tl){ void* _temp202=( void*) atts->hd; int
_temp216; _LL204: if(( unsigned int) _temp202 > 15u?(( struct _enum_struct*)
_temp202)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL217: _temp216=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp202)->f1; goto _LL205;} else{ goto
_LL206;} _LL206: if( _temp202 == Cyc_Absyn_Stdcall_att){ goto _LL207;} else{
goto _LL208;} _LL208: if( _temp202 == Cyc_Absyn_Cdecl_att){ goto _LL209;} else{
goto _LL210;} _LL210: if( _temp202 == Cyc_Absyn_Noreturn_att){ goto _LL211;}
else{ goto _LL212;} _LL212: if( _temp202 == Cyc_Absyn_Const_att){ goto _LL213;}
else{ goto _LL214;} _LL214: goto _LL215; _LL205: goto _LL207; _LL207: goto
_LL209; _LL209: goto _LL211; _LL211: goto _LL213; _LL213:* _temp191=({ struct
Cyc_List_List* _temp218=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp218->hd=( void*)(( void*) atts->hd); _temp218->tl=* _temp191; _temp218;});
continue; _LL215: res_atts=({ struct Cyc_List_List* _temp219=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp219->hd=( void*)(( void*) atts->hd);
_temp219->tl= res_atts; _temp219;}); continue; _LL203:;} return res_atts;}
_LL186: return(( struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp220=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 22; _temp221;})); _LL182:;} static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd){ struct _tagged_string* v=(* vd->name).f2;
void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts=
vd->attributes;{ void* _temp222=(* vd->name).f1; struct Cyc_List_List* _temp230;
struct Cyc_List_List* _temp232; _LL224: if(( unsigned int) _temp222 > 1u?((
struct _enum_struct*) _temp222)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL231:
_temp230=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp222)->f1;
if( _temp230 == 0){ goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;}
_LL226: if(( unsigned int) _temp222 > 1u?(( struct _enum_struct*) _temp222)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL233: _temp232=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp222)->f1; if( _temp232 == 0){ goto _LL227;} else{
goto _LL228;}} else{ goto _LL228;} _LL228: goto _LL229; _LL225: goto _LL223;
_LL227: goto _LL223; _LL229: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp234=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 43; _temp235;})); return; _LL223:;}(* vd->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp236=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp236->tag= Cyc_Absyn_Abs_n_tag;
_temp236->f1= te->ns;( void*) _temp236;});{ void* _temp237= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Exp* _temp244; struct Cyc_Absyn_Tqual* _temp246; void*
_temp248; _LL239: if(( unsigned int) _temp237 > 4u?(( struct _enum_struct*)
_temp237)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL249: _temp248=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp237)->f1; goto _LL247; _LL247: _temp246=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp237)->f2;
goto _LL245; _LL245: _temp244=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp237)->f3; if( _temp244 == 0){ goto _LL243;} else{ goto _LL241;}} else{ goto
_LL241;} _LL243: if( vd->initializer != 0){ goto _LL240;} else{ goto _LL241;}
_LL241: goto _LL242; _LL240:{ void* _temp250=( void*)( vd->initializer)->r; void*
_temp260; struct _tagged_string _temp262; struct Cyc_List_List* _temp264; struct
Cyc_Core_Opt* _temp266; struct Cyc_List_List* _temp268; _LL252: if((( struct
_enum_struct*) _temp250)->tag == Cyc_Absyn_Const_e_tag){ _LL261: _temp260=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp250)->f1; if(( unsigned int) _temp260 > 1u?((
struct _enum_struct*) _temp260)->tag == Cyc_Absyn_String_c_tag: 0){ _LL263:
_temp262=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp260)->f1;
goto _LL253;} else{ goto _LL254;}} else{ goto _LL254;} _LL254: if((( struct
_enum_struct*) _temp250)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL267:
_temp266=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp250)->f1; goto _LL265; _LL265: _temp264=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp250)->f2; goto _LL255;} else{ goto
_LL256;} _LL256: if((( struct _enum_struct*) _temp250)->tag == Cyc_Absyn_Array_e_tag){
_LL269: _temp268=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp250)->f1; goto _LL257;} else{ goto _LL258;} _LL258: goto _LL259; _LL253: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp270=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp270->tag= Cyc_Absyn_ArrayType_tag; _temp270->f1=( void*) _temp248; _temp270->f2=
_temp246; _temp270->f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(({ struct
_tagged_string _temp271= _temp262;( unsigned int)( _temp271.last_plus_one -
_temp271.curr);}), 0);( void*) _temp270;})); goto _LL251; _LL255: _temp268=
_temp264; goto _LL257; _LL257: t=( void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp272=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp272->tag= Cyc_Absyn_ArrayType_tag; _temp272->f1=( void*) _temp248; _temp272->f2=
_temp246; _temp272->f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp268), 0);(
void*) _temp272;})); goto _LL251; _LL259: goto _LL251; _LL251:;} goto _LL238;
_LL242: goto _LL238; _LL238:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tc_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t, atts);}{
int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts); if( sc == Cyc_Absyn_Extern?
1: sc == Cyc_Absyn_ExternC){ if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp273=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp274; _temp274.curr= _temp273; _temp274.base= _temp273;
_temp274.last_plus_one= _temp273 + 47; _temp274;}));}} else{ if( ! Cyc_Tc_is_function_type(
t)){ for( 0; atts != 0; atts= atts->tl){ void* _temp275=( void*) atts->hd; int
_temp295; struct _tagged_string _temp297; _LL277: if(( unsigned int) _temp275 >
15u?(( struct _enum_struct*) _temp275)->tag == Cyc_Absyn_Aligned_att_tag: 0){
_LL296: _temp295=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp275)->f1;
goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int) _temp275 > 15u?((
struct _enum_struct*) _temp275)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL298:
_temp297=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp275)->f1; goto _LL280;} else{ goto _LL281;} _LL281: if( _temp275 == Cyc_Absyn_Nocommon_att){
goto _LL282;} else{ goto _LL283;} _LL283: if( _temp275 == Cyc_Absyn_Shared_att){
goto _LL284;} else{ goto _LL285;} _LL285: if( _temp275 == Cyc_Absyn_Unused_att){
goto _LL286;} else{ goto _LL287;} _LL287: if( _temp275 == Cyc_Absyn_Weak_att){
goto _LL288;} else{ goto _LL289;} _LL289: if( _temp275 == Cyc_Absyn_Dllimport_att){
goto _LL290;} else{ goto _LL291;} _LL291: if( _temp275 == Cyc_Absyn_Dllexport_att){
goto _LL292;} else{ goto _LL293;} _LL293: goto _LL294; _LL278: continue; _LL280:
continue; _LL282: continue; _LL284: continue; _LL286: continue; _LL288:
continue; _LL290: continue; _LL292: continue; _LL294: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp299= Cyc_Absyn_attribute2string(( void*) atts->hd);
struct _tagged_string _temp300= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp299.last_plus_one - _temp299.curr, _temp299.curr, _temp300.last_plus_one -
_temp300.curr, _temp300.curr);})); goto _LL276; _LL276:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp301= vd->initializer; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301;});}{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( !
Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp302= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp303= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp304= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp302.last_plus_one - _temp302.curr, _temp302.curr, _temp303.last_plus_one -
_temp303.curr, _temp303.curr, _temp304.last_plus_one - _temp304.curr, _temp304.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp305=( char*)"initializer is not a constant expression";
struct _tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 41; _temp306;}));}}}} else{ for( 0; atts != 0;
atts= atts->tl){ void* _temp307=( void*) atts->hd; int _temp325; int _temp327;
_LL309: if(( unsigned int) _temp307 > 15u?(( struct _enum_struct*) _temp307)->tag
== Cyc_Absyn_Regparm_att_tag: 0){ _LL326: _temp325=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp307)->f1; goto _LL310;} else{ goto _LL311;} _LL311: if( _temp307 == Cyc_Absyn_Stdcall_att){
goto _LL312;} else{ goto _LL313;} _LL313: if( _temp307 == Cyc_Absyn_Cdecl_att){
goto _LL314;} else{ goto _LL315;} _LL315: if( _temp307 == Cyc_Absyn_Noreturn_att){
goto _LL316;} else{ goto _LL317;} _LL317: if( _temp307 == Cyc_Absyn_Const_att){
goto _LL318;} else{ goto _LL319;} _LL319: if(( unsigned int) _temp307 > 15u?((
struct _enum_struct*) _temp307)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL328:
_temp327=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp307)->f1; goto
_LL320;} else{ goto _LL321;} _LL321: if( _temp307 == Cyc_Absyn_Packed_att){ goto
_LL322;} else{ goto _LL323;} _LL323: goto _LL324; _LL310: goto _LL312; _LL312:
goto _LL314; _LL314: goto _LL316; _LL316: goto _LL318; _LL318:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp329=(
char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 51; _temp330;})); goto _LL308; _LL320: goto
_LL322; _LL322: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp331= Cyc_Absyn_attribute2string((
void*) atts->hd); xprintf("bad attribute %.*s in function declaration", _temp331.last_plus_one
- _temp331.curr, _temp331.curr);})); goto _LL308; _LL324: continue; _LL308:;}}}
ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({
struct _tuple6* _temp332=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp332->f1=({ struct Cyc_Tcenv_VarRes_struct* _temp333=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp333->tag= Cyc_Tcenv_VarRes_tag;
_temp333->f1=( void*)({ struct Cyc_Absyn_Global_b_struct* _temp334=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp334->tag= Cyc_Absyn_Global_b_tag;
_temp334->f1= vd;( void*) _temp334;});( void*) _temp333;}); _temp332->f2=
is_redecl; _temp332;}));}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp335=(* fd->name).f1; struct Cyc_List_List* _temp343; struct Cyc_List_List*
_temp345; _LL337: if(( unsigned int) _temp335 > 1u?(( struct _enum_struct*)
_temp335)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL344: _temp343=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp335)->f1; if( _temp343 == 0){ goto _LL338;}
else{ goto _LL339;}} else{ goto _LL339;} _LL339: if(( unsigned int) _temp335 > 1u?((
struct _enum_struct*) _temp335)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL346:
_temp345=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp335)->f1;
goto _LL340;} else{ goto _LL341;} _LL341: goto _LL342; _LL338: goto _LL336;
_LL340:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp347=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp347->tag= Cyc_Core_Impossible_tag; _temp347->f1=( struct _tagged_string)({
char* _temp348=( char*)"tc: Abs_n in tcFndecl"; struct _tagged_string _temp349;
_temp349.curr= _temp348; _temp349.base= _temp348; _temp349.last_plus_one=
_temp348 + 22; _temp349;});( struct _xenum_struct*) _temp347;})); _LL342: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp350=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 43; _temp351;})); return; _LL336:;}(* fd->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp352=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp352->tag= Cyc_Absyn_Abs_n_tag;
_temp352->f1= te->ns;( void*) _temp352;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); int is_redecl= Cyc_Tc_var_redecl_okay(
ge, loc, v, t, sc, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts= atts->tl){ void* _temp353=( void*) atts->hd; int
_temp361; _LL355: if( _temp353 == Cyc_Absyn_Packed_att){ goto _LL356;} else{
goto _LL357;} _LL357: if(( unsigned int) _temp353 > 15u?(( struct _enum_struct*)
_temp353)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL362: _temp361=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp353)->f1; goto _LL358;} else{ goto
_LL359;} _LL359: goto _LL360; _LL356: goto _LL358; _LL358: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp363= Cyc_Absyn_attribute2string(( void*) atts->hd);
xprintf("bad attribute %.*s for function", _temp363.last_plus_one - _temp363.curr,
_temp363.curr);})); goto _LL354; _LL360: goto _LL354; _LL354:;}} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple6*
_temp364=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp364->f1=({
struct Cyc_Tcenv_VarRes_struct* _temp365=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp365->tag= Cyc_Tcenv_VarRes_tag;
_temp365->f1=( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp366=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp366->tag= Cyc_Absyn_Funname_b_tag;
_temp366->f1= fd;( void*) _temp366;});( void*) _temp365;}); _temp364->f2=
is_redecl; _temp364;})); te->le=({ struct Cyc_Core_Opt* _temp367=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp367->v=( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp368=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp368->tag= Cyc_Tcenv_Outermost_tag; _temp368->f1=( void*) Cyc_Tcenv_new_fenv(
fd);( void*) _temp368;}); _temp367;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if(
! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp369=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 49; _temp370;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp371=(* td->name).f1; struct Cyc_List_List*
_temp379; struct Cyc_List_List* _temp381; _LL373: if(( unsigned int) _temp371 >
1u?(( struct _enum_struct*) _temp371)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL380:
_temp379=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp371)->f1;
if( _temp379 == 0){ goto _LL374;} else{ goto _LL375;}} else{ goto _LL375;}
_LL375: if(( unsigned int) _temp371 > 1u?(( struct _enum_struct*) _temp371)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL382: _temp381=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp371)->f1; if( _temp381 == 0){ goto _LL376;} else{
goto _LL377;}} else{ goto _LL377;} _LL377: goto _LL378; _LL374: goto _LL372;
_LL376: goto _LL372; _LL378: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp383=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 43; _temp384;})); return; _LL372:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp385=* v; xprintf("redeclaration of typedef %.*s",
_temp385.last_plus_one - _temp385.curr, _temp385.curr);})); return;}(* td->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp386=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp386->tag= Cyc_Absyn_Abs_n_tag;
_temp386->f1= te->ns;( void*) _temp386;}); Cyc_Tcutil_check_unique_tvars( loc,
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs, Cyc_Absyn_AnyKind,( void*) td->defn);
ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs,
v, td);} static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp387=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 47; _temp388;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( sd->name)->v)).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp389=( void*)
atts->hd; int _temp397; _LL391: if( _temp389 == Cyc_Absyn_Packed_att){ goto
_LL392;} else{ goto _LL393;} _LL393: if(( unsigned int) _temp389 > 15u?(( struct
_enum_struct*) _temp389)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL398: _temp397=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp389)->f1; goto _LL394;} else{
goto _LL395;} _LL395: goto _LL396; _LL392: continue; _LL394: continue; _LL396:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp399= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp400=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp399.last_plus_one - _temp399.curr, _temp399.curr, _temp400.last_plus_one -
_temp400.curr, _temp400.curr);})); goto _LL390; _LL390:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp401=( void*) c->v; void*
_temp409; _LL403: if( _temp401 == Cyc_Absyn_No_constr){ goto _LL404;} else{ goto
_LL405;} _LL405: if(( unsigned int) _temp401 > 1u?(( struct _enum_struct*)
_temp401)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL410: _temp409=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp401)->f1; if( _temp409 == Cyc_Absyn_MemKind){
goto _LL406;} else{ goto _LL407;}} else{ goto _LL407;} _LL407: goto _LL408;
_LL404:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp411=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp411->tag= Cyc_Absyn_Eq_constr_tag; _temp411->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp411;})); continue; _LL406: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp412=* v; struct _tagged_string _temp413=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp412.last_plus_one - _temp412.curr, _temp412.curr, _temp413.last_plus_one -
_temp413.curr, _temp413.curr);})); continue; _LL408: continue; _LL402:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp414=(*(( struct _tuple0*)( sd->name)->v)).f1;
struct Cyc_List_List* _temp422; struct Cyc_List_List* _temp424; _LL416: if((
unsigned int) _temp414 > 1u?(( struct _enum_struct*) _temp414)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL423: _temp422=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp414)->f1; if( _temp422 == 0){ goto _LL417;} else{ goto _LL418;}} else{ goto
_LL418;} _LL418: if(( unsigned int) _temp414 > 1u?(( struct _enum_struct*)
_temp414)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL425: _temp424=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp414)->f1; if( _temp424 == 0){ goto _LL419;}
else{ goto _LL420;}} else{ goto _LL420;} _LL420: goto _LL421; _LL417: goto
_LL415; _LL419: goto _LL415; _LL421: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp426=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 43; _temp427;})); return; _LL415:;}(*((
struct _tuple0*)( sd->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp428=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp428->tag= Cyc_Absyn_Abs_n_tag; _temp428->f1= te->ns;( void*) _temp428;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp429=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->structdecls, v); struct Cyc_Core_Opt _temp435; struct
Cyc_Absyn_Structdecl** _temp436; _LL431: if( _temp429 == 0){ goto _LL432;} else{
goto _LL433;} _LL433: if( _temp429 == 0){ goto _LL430;} else{ _temp435=*
_temp429; _LL437: _temp436=( struct Cyc_Absyn_Structdecl**) _temp435.v; goto
_LL434;} _LL432: ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, v,({ struct Cyc_Absyn_Structdecl** _temp438=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp438[ 0]= sd; _temp438;}));
if( sd->fields == 0){ return;} goto _LL430; _LL434: { struct Cyc_Absyn_Structdecl*
sd2=* _temp436; struct Cyc_List_List* tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp439=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp439.last_plus_one - _temp439.curr, _temp439.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp440=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp440->tag= Cyc_Absyn_Forward_constr_tag;
_temp440->f1= c2;( void*) _temp440;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp441=* v; struct
_tagged_string _temp442= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp443=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp444= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp441.last_plus_one - _temp441.curr, _temp441.curr, _temp442.last_plus_one -
_temp442.curr, _temp442.curr, _temp443.last_plus_one - _temp443.curr, _temp443.curr,
_temp444.last_plus_one - _temp444.curr, _temp444.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp445=( char*)"struct";
struct _tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 7; _temp446;}), v));{ struct _tuple7 _temp448=({
struct _tuple7 _temp447; _temp447.f1= sd->fields; _temp447.f2= sd2->fields;
_temp447;}); struct Cyc_Core_Opt* _temp458; struct Cyc_Core_Opt* _temp460;
struct Cyc_Core_Opt* _temp462; struct Cyc_Core_Opt* _temp464; struct Cyc_Core_Opt*
_temp466; struct Cyc_Core_Opt* _temp468; struct Cyc_Core_Opt* _temp470; struct
Cyc_Core_Opt* _temp472; _LL450: _LL461: _temp460= _temp448.f1; if( _temp460 == 0){
goto _LL459;} else{ goto _LL452;} _LL459: _temp458= _temp448.f2; if( _temp458 ==
0){ goto _LL451;} else{ goto _LL452;} _LL452: _LL465: _temp464= _temp448.f1;
goto _LL463; _LL463: _temp462= _temp448.f2; if( _temp462 == 0){ goto _LL453;}
else{ goto _LL454;} _LL454: _LL469: _temp468= _temp448.f1; if( _temp468 == 0){
goto _LL467;} else{ goto _LL456;} _LL467: _temp466= _temp448.f2; goto _LL455;
_LL456: _LL473: _temp472= _temp448.f1; goto _LL471; _LL471: _temp470= _temp448.f2;
goto _LL457; _LL451: return; _LL453:* _temp436= sd; goto _LL449; _LL455: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL457: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp474=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp474->hd=( void*)({ struct _tuple4* _temp475=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp475->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp475->f2=({ struct Cyc_Absyn_VarType_struct* _temp476=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp476->tag= Cyc_Absyn_VarType_tag;
_temp476->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp476;}); _temp475;});
_temp474->tl= inst; _temp474;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes,
sd2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp477=* v;
xprintf("redeclaration of struct %.*s has different attributes", _temp477.last_plus_one
- _temp477.curr, _temp477.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
sd->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( sd2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp480; struct Cyc_List_List* _temp481; struct Cyc_Core_Opt* _temp483; void*
_temp485; struct Cyc_Absyn_Tqual* _temp487; struct _tagged_string* _temp489;
struct Cyc_Absyn_Structfield* _temp478=( struct Cyc_Absyn_Structfield*) f1s->hd;
_temp480=* _temp478; _LL490: _temp489=( struct _tagged_string*) _temp480.name;
goto _LL488; _LL488: _temp487=( struct Cyc_Absyn_Tqual*) _temp480.tq; goto
_LL486; _LL486: _temp485=( void*) _temp480.type; goto _LL484; _LL484: _temp483=(
struct Cyc_Core_Opt*) _temp480.width; goto _LL482; _LL482: _temp481=( struct Cyc_List_List*)
_temp480.attributes; goto _LL479; _LL479: { struct Cyc_Absyn_Structfield
_temp493; struct Cyc_List_List* _temp494; struct Cyc_Core_Opt* _temp496; void*
_temp498; struct Cyc_Absyn_Tqual* _temp500; struct _tagged_string* _temp502;
struct Cyc_Absyn_Structfield* _temp491=( struct Cyc_Absyn_Structfield*) f2s->hd;
_temp493=* _temp491; _LL503: _temp502=( struct _tagged_string*) _temp493.name;
goto _LL501; _LL501: _temp500=( struct Cyc_Absyn_Tqual*) _temp493.tq; goto
_LL499; _LL499: _temp498=( void*) _temp493.type; goto _LL497; _LL497: _temp496=(
struct Cyc_Core_Opt*) _temp493.width; goto _LL495; _LL495: _temp494=( struct Cyc_List_List*)
_temp493.attributes; goto _LL492; _LL492: if( Cyc_String_zstrptrcmp( _temp489,
_temp502) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp504=* v;
struct _tagged_string _temp505=* _temp489; struct _tagged_string _temp506=*
_temp502; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp504.last_plus_one - _temp504.curr, _temp504.curr, _temp505.last_plus_one -
_temp505.curr, _temp505.curr, _temp506.last_plus_one - _temp506.curr, _temp506.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp481, _temp494)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp507=* v; struct _tagged_string _temp508=* _temp489; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp507.last_plus_one - _temp507.curr, _temp507.curr, _temp508.last_plus_one -
_temp508.curr, _temp508.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp487,
_temp500)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp509=* v; struct
_tagged_string _temp510=* _temp489; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp509.last_plus_one - _temp509.curr, _temp509.curr, _temp510.last_plus_one -
_temp510.curr, _temp510.curr);}));} if( _temp483 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _temp483->v): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp511=* v; struct _tagged_string _temp512=* _temp489; xprintf("redeclaration of struct %.*s: bad bitfield %.*s",
_temp511.last_plus_one - _temp511.curr, _temp511.curr, _temp512.last_plus_one -
_temp512.curr, _temp512.curr);}));} else{ if((( _temp483 != 0? _temp496 != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _temp483->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp496->v): 0)? 1: _temp483 != _temp496){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp513=* v; struct _tagged_string _temp514=*
_temp489; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp513.last_plus_one - _temp513.curr, _temp513.curr, _temp514.last_plus_one -
_temp514.curr, _temp514.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp485);{ void* subst_t2= Cyc_Tcutil_substitute( inst, _temp498); if( ! Cyc_Tcutil_unify(
_temp485, subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp515=* v;
struct _tagged_string _temp516=* _temp489; struct _tagged_string _temp517= Cyc_Absynpp_typ2string(
_temp485); struct _tagged_string _temp518= Cyc_Absynpp_typ2string( subst_t2);
xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp515.last_plus_one - _temp515.curr, _temp515.curr, _temp516.last_plus_one -
_temp516.curr, _temp516.curr, _temp517.last_plus_one - _temp517.curr, _temp517.curr,
_temp518.last_plus_one - _temp518.curr, _temp518.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp519=* v; struct
_tagged_string _temp520=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp519.last_plus_one
- _temp519.curr, _temp519.curr, _temp520.last_plus_one - _temp520.curr, _temp520.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp521=* v;
struct _tagged_string _temp522=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp521.last_plus_one
- _temp521.curr, _temp521.curr, _temp522.last_plus_one - _temp522.curr, _temp522.curr);}));}
return;}} _LL449:;} goto _LL430;}} _LL430:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp523=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp523->sc=( void*)(( void*) sd->sc); _temp523->name= sd->name; _temp523->tvs=
tvs; _temp523->fields= 0; _temp523->attributes= 0; _temp523;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield _temp526; struct Cyc_List_List*
_temp527; struct Cyc_Core_Opt* _temp529; void* _temp531; struct Cyc_Absyn_Tqual*
_temp533; struct _tagged_string* _temp535; struct Cyc_Absyn_Structfield*
_temp524=( struct Cyc_Absyn_Structfield*) fs->hd; _temp526=* _temp524; _LL536:
_temp535=( struct _tagged_string*) _temp526.name; goto _LL534; _LL534: _temp533=(
struct Cyc_Absyn_Tqual*) _temp526.tq; goto _LL532; _LL532: _temp531=( void*)
_temp526.type; goto _LL530; _LL530: _temp529=( struct Cyc_Core_Opt*) _temp526.width;
goto _LL528; _LL528: _temp527=( struct Cyc_List_List*) _temp526.attributes; goto
_LL525; _LL525: if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp535)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp537=* _temp535; xprintf("duplicate field %.*s in struct",
_temp537.last_plus_one - _temp537.curr, _temp537.curr);}));} if( Cyc_String_strcmp(*
_temp535,( struct _tagged_string)({ char* _temp538=( char*)""; struct
_tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 1; _temp539;})) != 0){ prev_fields=({ struct
Cyc_List_List* _temp540=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp540->hd=( void*) _temp535; _temp540->tl= prev_fields; _temp540;});} Cyc_Tcutil_check_type(
loc, te, tvs, Cyc_Absyn_MemKind, _temp531); if( _temp529 != 0){ unsigned int w=
0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _temp529->v)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp541=* _temp535; xprintf("bitfield %.*s does not have constant width",
_temp541.last_plus_one - _temp541.curr, _temp541.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp529->v);}{ void* _temp542= Cyc_Tcutil_compress(
_temp531); void* _temp548; void* _temp550; _LL544: if(( unsigned int) _temp542 >
4u?(( struct _enum_struct*) _temp542)->tag == Cyc_Absyn_IntType_tag: 0){ _LL551:
_temp550=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp542)->f1; goto _LL549;
_LL549: _temp548=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp542)->f2;
goto _LL545;} else{ goto _LL546;} _LL546: goto _LL547; _LL545:{ void* _temp552=
_temp548; _LL554: if( _temp552 == Cyc_Absyn_B1){ goto _LL555;} else{ goto _LL556;}
_LL556: if( _temp552 == Cyc_Absyn_B2){ goto _LL557;} else{ goto _LL558;} _LL558:
if( _temp552 == Cyc_Absyn_B4){ goto _LL559;} else{ goto _LL560;} _LL560: if(
_temp552 == Cyc_Absyn_B8){ goto _LL561;} else{ goto _LL553;} _LL555: if( w >(
unsigned int) 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp562=(
char*)"bitfield larger than type"; struct _tagged_string _temp563; _temp563.curr=
_temp562; _temp563.base= _temp562; _temp563.last_plus_one= _temp562 + 26;
_temp563;}));} goto _LL553; _LL557: if( w >( unsigned int) 16){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp564=( char*)"bitfield larger than type";
struct _tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 26; _temp565;}));} goto _LL553; _LL559: if( w
>( unsigned int) 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp566=( char*)"bitfield larger than type"; struct _tagged_string _temp567;
_temp567.curr= _temp566; _temp567.base= _temp566; _temp567.last_plus_one=
_temp566 + 26; _temp567;}));} goto _LL553; _LL561: if( w >( unsigned int) 64){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp568=( char*)"bitfield larger than type";
struct _tagged_string _temp569; _temp569.curr= _temp568; _temp569.base= _temp568;
_temp569.last_plus_one= _temp568 + 26; _temp569;}));} goto _LL553; _LL553:;}
goto _LL543; _LL547: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp570=*
_temp535; struct _tagged_string _temp571= Cyc_Absynpp_typ2string( _temp531);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp570.last_plus_one
- _temp570.curr, _temp570.curr, _temp571.last_plus_one - _temp571.curr, _temp571.curr);}));
goto _LL543; _LL543:;}} for( 0; _temp527 != 0; _temp527= _temp527->tl){ void*
_temp572=( void*) _temp527->hd; int _temp580; _LL574: if( _temp572 == Cyc_Absyn_Packed_att){
goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int) _temp572 > 15u?((
struct _enum_struct*) _temp572)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL581:
_temp580=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp572)->f1; goto
_LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL575: continue; _LL577:
continue; _LL579: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp582= Cyc_Absyn_attribute2string((
void*) _temp527->hd); struct _tagged_string _temp583=* v; struct _tagged_string
_temp584=* _temp535; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp582.last_plus_one - _temp582.curr, _temp582.curr, _temp583.last_plus_one -
_temp583.curr, _temp583.curr, _temp584.last_plus_one - _temp584.curr, _temp584.curr);}));
goto _LL573; _LL573:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp585=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp585->f1=({ struct Cyc_Tcenv_StructRes_struct* _temp586=( struct
Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp586->tag= Cyc_Tcenv_StructRes_tag; _temp586->f1= sd;( void*) _temp586;});
_temp585->f2= 1; _temp585;}));* sdp= sd;}}}}} static void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp587=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 46; _temp588;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ud->name)->v)).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp589=( void*)
atts->hd; int _temp597; _LL591: if( _temp589 == Cyc_Absyn_Packed_att){ goto
_LL592;} else{ goto _LL593;} _LL593: if(( unsigned int) _temp589 > 15u?(( struct
_enum_struct*) _temp589)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL598: _temp597=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp589)->f1; goto _LL594;} else{
goto _LL595;} _LL595: goto _LL596; _LL592: continue; _LL594: continue; _LL596:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp599= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp600=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp599.last_plus_one - _temp599.curr, _temp599.curr, _temp600.last_plus_one -
_temp600.curr, _temp600.curr);})); goto _LL590; _LL590:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp601=( void*) c->v; void*
_temp609; _LL603: if( _temp601 == Cyc_Absyn_No_constr){ goto _LL604;} else{ goto
_LL605;} _LL605: if(( unsigned int) _temp601 > 1u?(( struct _enum_struct*)
_temp601)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL610: _temp609=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp601)->f1; if( _temp609 == Cyc_Absyn_MemKind){
goto _LL606;} else{ goto _LL607;}} else{ goto _LL607;} _LL607: goto _LL608;
_LL604:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp611=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp611->tag= Cyc_Absyn_Eq_constr_tag; _temp611->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp611;})); continue; _LL606: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp612=* v; struct _tagged_string _temp613=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("union %.*s attempts to abstract type variable %.*s of kind M",
_temp612.last_plus_one - _temp612.curr, _temp612.curr, _temp613.last_plus_one -
_temp613.curr, _temp613.curr);})); continue; _LL608: continue; _LL602:;}}{
struct Cyc_List_List* tvs= ud->tvs;{ void* _temp614=(*(( struct _tuple0*)( ud->name)->v)).f1;
struct Cyc_List_List* _temp622; struct Cyc_List_List* _temp624; _LL616: if((
unsigned int) _temp614 > 1u?(( struct _enum_struct*) _temp614)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL623: _temp622=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp614)->f1; if( _temp622 == 0){ goto _LL617;} else{ goto _LL618;}} else{ goto
_LL618;} _LL618: if(( unsigned int) _temp614 > 1u?(( struct _enum_struct*)
_temp614)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL625: _temp624=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp614)->f1; if( _temp624 == 0){ goto _LL619;}
else{ goto _LL620;}} else{ goto _LL620;} _LL620: goto _LL621; _LL617: goto
_LL615; _LL619: goto _LL615; _LL621: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp626=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp627; _temp627.curr= _temp626; _temp627.base= _temp626;
_temp627.last_plus_one= _temp626 + 43; _temp627;})); return; _LL615:;}(*((
struct _tuple0*)( ud->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp628=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp628->tag= Cyc_Absyn_Abs_n_tag; _temp628->f1= te->ns;( void*) _temp628;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp629=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct Cyc_Core_Opt _temp635; struct
Cyc_Absyn_Uniondecl** _temp636; _LL631: if( _temp629 == 0){ goto _LL632;} else{
goto _LL633;} _LL633: if( _temp629 == 0){ goto _LL630;} else{ _temp635=*
_temp629; _LL637: _temp636=( struct Cyc_Absyn_Uniondecl**) _temp635.v; goto
_LL634;} _LL632: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl** _temp638=( struct Cyc_Absyn_Uniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*)); _temp638[ 0]= ud; _temp638;}));
if( ud->fields == 0){ return;} goto _LL630; _LL634: { struct Cyc_Absyn_Uniondecl*
ud2=* _temp636; struct Cyc_List_List* tvs2= ud2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp639=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp639.last_plus_one - _temp639.curr, _temp639.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp640=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp640->tag= Cyc_Absyn_Forward_constr_tag;
_temp640->f1= c2;( void*) _temp640;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp641=* v; struct
_tagged_string _temp642= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp643=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp644= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp641.last_plus_one - _temp641.curr, _temp641.curr, _temp642.last_plus_one -
_temp642.curr, _temp642.curr, _temp643.last_plus_one - _temp643.curr, _temp643.curr,
_temp644.last_plus_one - _temp644.curr, _temp644.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp645=( char*)"union";
struct _tagged_string _temp646; _temp646.curr= _temp645; _temp646.base= _temp645;
_temp646.last_plus_one= _temp645 + 6; _temp646;}), v));{ struct _tuple7 _temp648=({
struct _tuple7 _temp647; _temp647.f1= ud->fields; _temp647.f2= ud2->fields;
_temp647;}); struct Cyc_Core_Opt* _temp658; struct Cyc_Core_Opt* _temp660;
struct Cyc_Core_Opt* _temp662; struct Cyc_Core_Opt* _temp664; struct Cyc_Core_Opt*
_temp666; struct Cyc_Core_Opt* _temp668; struct Cyc_Core_Opt* _temp670; struct
Cyc_Core_Opt* _temp672; _LL650: _LL661: _temp660= _temp648.f1; if( _temp660 == 0){
goto _LL659;} else{ goto _LL652;} _LL659: _temp658= _temp648.f2; if( _temp658 ==
0){ goto _LL651;} else{ goto _LL652;} _LL652: _LL665: _temp664= _temp648.f1;
goto _LL663; _LL663: _temp662= _temp648.f2; if( _temp662 == 0){ goto _LL653;}
else{ goto _LL654;} _LL654: _LL669: _temp668= _temp648.f1; if( _temp668 == 0){
goto _LL667;} else{ goto _LL656;} _LL667: _temp666= _temp648.f2; goto _LL655;
_LL656: _LL673: _temp672= _temp648.f1; goto _LL671; _LL671: _temp670= _temp648.f2;
goto _LL657; _LL651: return; _LL653:* _temp636= ud; goto _LL649; _LL655: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL657: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp674=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp674->hd=( void*)({ struct _tuple4* _temp675=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp675->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp675->f2=({ struct Cyc_Absyn_VarType_struct* _temp676=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp676->tag= Cyc_Absyn_VarType_tag;
_temp676->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp676;}); _temp675;});
_temp674->tl= inst; _temp674;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes,
ud2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp677=* v;
xprintf("redeclaration of union %.*s has different attributes", _temp677.last_plus_one
- _temp677.curr, _temp677.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ud->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ud2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp680; struct Cyc_List_List* _temp681; struct Cyc_Core_Opt* _temp683; void*
_temp685; struct Cyc_Absyn_Tqual* _temp687; struct _tagged_string* _temp689;
struct Cyc_Absyn_Structfield* _temp678=( struct Cyc_Absyn_Structfield*) f1s->hd;
_temp680=* _temp678; _LL690: _temp689=( struct _tagged_string*) _temp680.name;
goto _LL688; _LL688: _temp687=( struct Cyc_Absyn_Tqual*) _temp680.tq; goto
_LL686; _LL686: _temp685=( void*) _temp680.type; goto _LL684; _LL684: _temp683=(
struct Cyc_Core_Opt*) _temp680.width; goto _LL682; _LL682: _temp681=( struct Cyc_List_List*)
_temp680.attributes; goto _LL679; _LL679: { struct Cyc_Absyn_Structfield
_temp693; struct Cyc_List_List* _temp694; struct Cyc_Core_Opt* _temp696; void*
_temp698; struct Cyc_Absyn_Tqual* _temp700; struct _tagged_string* _temp702;
struct Cyc_Absyn_Structfield* _temp691=( struct Cyc_Absyn_Structfield*) f2s->hd;
_temp693=* _temp691; _LL703: _temp702=( struct _tagged_string*) _temp693.name;
goto _LL701; _LL701: _temp700=( struct Cyc_Absyn_Tqual*) _temp693.tq; goto
_LL699; _LL699: _temp698=( void*) _temp693.type; goto _LL697; _LL697: _temp696=(
struct Cyc_Core_Opt*) _temp693.width; goto _LL695; _LL695: _temp694=( struct Cyc_List_List*)
_temp693.attributes; goto _LL692; _LL692: if( Cyc_String_zstrptrcmp( _temp689,
_temp702) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp704=* v;
struct _tagged_string _temp705=* _temp689; struct _tagged_string _temp706=*
_temp702; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp704.last_plus_one - _temp704.curr, _temp704.curr, _temp705.last_plus_one -
_temp705.curr, _temp705.curr, _temp706.last_plus_one - _temp706.curr, _temp706.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp681, _temp694)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp707=* v; struct _tagged_string _temp708=* _temp689; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp707.last_plus_one - _temp707.curr, _temp707.curr, _temp708.last_plus_one -
_temp708.curr, _temp708.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp687,
_temp700)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp709=* v; struct
_tagged_string _temp710=* _temp689; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp709.last_plus_one - _temp709.curr, _temp709.curr, _temp710.last_plus_one -
_temp710.curr, _temp710.curr);}));} if( _temp683 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _temp683->v): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp711=* v; struct _tagged_string _temp712=* _temp689; xprintf("redeclaration of struct %.*s: bad bitfield %.*s",
_temp711.last_plus_one - _temp711.curr, _temp711.curr, _temp712.last_plus_one -
_temp712.curr, _temp712.curr);}));} else{ if((( _temp683 != 0? _temp696 != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _temp683->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp696->v): 0)? 1: _temp683 != _temp696){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp713=* v; struct _tagged_string _temp714=*
_temp689; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp713.last_plus_one - _temp713.curr, _temp713.curr, _temp714.last_plus_one -
_temp714.curr, _temp714.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp685);{ void* subst_t2= Cyc_Tcutil_substitute( inst, _temp698); if( ! Cyc_Tcutil_unify(
_temp685, subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp715=* v;
struct _tagged_string _temp716=* _temp689; struct _tagged_string _temp717= Cyc_Absynpp_typ2string(
_temp685); struct _tagged_string _temp718= Cyc_Absynpp_typ2string( subst_t2);
xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp715.last_plus_one - _temp715.curr, _temp715.curr, _temp716.last_plus_one -
_temp716.curr, _temp716.curr, _temp717.last_plus_one - _temp717.curr, _temp717.curr,
_temp718.last_plus_one - _temp718.curr, _temp718.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp719=* v; struct
_tagged_string _temp720=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp719.last_plus_one
- _temp719.curr, _temp719.curr, _temp720.last_plus_one - _temp720.curr, _temp720.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp721=* v;
struct _tagged_string _temp722=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp721.last_plus_one
- _temp721.curr, _temp721.curr, _temp722.last_plus_one - _temp722.curr, _temp722.curr);}));}
return;}} _LL649:;} goto _LL630;}} _LL630:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp723=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp723->sc=( void*)(( void*) ud->sc); _temp723->name= ud->name; _temp723->tvs=
tvs; _temp723->fields= 0; _temp723->attributes= 0; _temp723;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( ud->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield
_temp726; struct Cyc_List_List* _temp727; struct Cyc_Core_Opt* _temp729; void*
_temp731; struct Cyc_Absyn_Tqual* _temp733; struct _tagged_string* _temp735;
struct Cyc_Absyn_Structfield* _temp724=( struct Cyc_Absyn_Structfield*) fs->hd;
_temp726=* _temp724; _LL736: _temp735=( struct _tagged_string*) _temp726.name;
goto _LL734; _LL734: _temp733=( struct Cyc_Absyn_Tqual*) _temp726.tq; goto
_LL732; _LL732: _temp731=( void*) _temp726.type; goto _LL730; _LL730: _temp729=(
struct Cyc_Core_Opt*) _temp726.width; goto _LL728; _LL728: _temp727=( struct Cyc_List_List*)
_temp726.attributes; goto _LL725; _LL725: if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields, _temp735)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp737=* _temp735; xprintf("duplicate field %.*s in union",
_temp737.last_plus_one - _temp737.curr, _temp737.curr);}));} prev_fields=({
struct Cyc_List_List* _temp738=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp738->hd=( void*) _temp735; _temp738->tl=
prev_fields; _temp738;}); Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp731); if( _temp729 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _temp729->v)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp739=* _temp735; xprintf("bitfield %.*s does not have constant width",
_temp739.last_plus_one - _temp739.curr, _temp739.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp729->v);}{ void* _temp740= Cyc_Tcutil_compress(
_temp731); void* _temp746; void* _temp748; _LL742: if(( unsigned int) _temp740 >
4u?(( struct _enum_struct*) _temp740)->tag == Cyc_Absyn_IntType_tag: 0){ _LL749:
_temp748=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp740)->f1; goto _LL747;
_LL747: _temp746=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp740)->f2;
goto _LL743;} else{ goto _LL744;} _LL744: goto _LL745; _LL743:{ void* _temp750=
_temp746; _LL752: if( _temp750 == Cyc_Absyn_B1){ goto _LL753;} else{ goto _LL754;}
_LL754: if( _temp750 == Cyc_Absyn_B2){ goto _LL755;} else{ goto _LL756;} _LL756:
if( _temp750 == Cyc_Absyn_B4){ goto _LL757;} else{ goto _LL758;} _LL758: if(
_temp750 == Cyc_Absyn_B8){ goto _LL759;} else{ goto _LL751;} _LL753: if( w >(
unsigned int) 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp760=(
char*)"bitfield larger than type"; struct _tagged_string _temp761; _temp761.curr=
_temp760; _temp761.base= _temp760; _temp761.last_plus_one= _temp760 + 26;
_temp761;}));} goto _LL751; _LL755: if( w >( unsigned int) 16){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp762=( char*)"bitfield larger than type";
struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 26; _temp763;}));} goto _LL751; _LL757: if( w
>( unsigned int) 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp764=( char*)"bitfield larger than type"; struct _tagged_string _temp765;
_temp765.curr= _temp764; _temp765.base= _temp764; _temp765.last_plus_one=
_temp764 + 26; _temp765;}));} goto _LL751; _LL759: if( w >( unsigned int) 64){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp766=( char*)"bitfield larger than type";
struct _tagged_string _temp767; _temp767.curr= _temp766; _temp767.base= _temp766;
_temp767.last_plus_one= _temp766 + 26; _temp767;}));} goto _LL751; _LL751:;}
goto _LL741; _LL745: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp768=*
_temp735; struct _tagged_string _temp769= Cyc_Absynpp_typ2string( _temp731);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp768.last_plus_one
- _temp768.curr, _temp768.curr, _temp769.last_plus_one - _temp769.curr, _temp769.curr);}));
goto _LL741; _LL741:;}} if( ! Cyc_Tcutil_bits_only( _temp731)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp770=* _temp735; struct _tagged_string _temp771=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp770.last_plus_one - _temp770.curr, _temp770.curr, _temp771.last_plus_one -
_temp771.curr, _temp771.curr);}));} for( 0; _temp727 != 0; _temp727= _temp727->tl){
void* _temp772=( void*) _temp727->hd; int _temp780; _LL774: if( _temp772 == Cyc_Absyn_Packed_att){
goto _LL775;} else{ goto _LL776;} _LL776: if(( unsigned int) _temp772 > 15u?((
struct _enum_struct*) _temp772)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL781:
_temp780=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp772)->f1; goto
_LL777;} else{ goto _LL778;} _LL778: goto _LL779; _LL775: continue; _LL777:
continue; _LL779: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp782= Cyc_Absyn_attribute2string((
void*) _temp727->hd); struct _tagged_string _temp783=* v; struct _tagged_string
_temp784=* _temp735; xprintf("bad attribute %.*s in union %.*s, member %.*s",
_temp782.last_plus_one - _temp782.curr, _temp782.curr, _temp783.last_plus_one -
_temp783.curr, _temp783.curr, _temp784.last_plus_one - _temp784.curr, _temp784.curr);}));
goto _LL773; _LL773:;}}}* udp= ud;}}}}} static void Cyc_Tc_tcEnumdecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Enumdecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp785=( char*)"anonymous enums are not allowed at top level";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 45; _temp786;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ed->name)->v)).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void*
_temp787=( void*) c->v; void* _temp795; _LL789: if( _temp787 == Cyc_Absyn_No_constr){
goto _LL790;} else{ goto _LL791;} _LL791: if(( unsigned int) _temp787 > 1u?((
struct _enum_struct*) _temp787)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL796:
_temp795=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp787)->f1; if(
_temp795 == Cyc_Absyn_MemKind){ goto _LL792;} else{ goto _LL793;}} else{ goto
_LL793;} _LL793: goto _LL794; _LL790:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp797=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp797->tag= Cyc_Absyn_Eq_constr_tag; _temp797->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp797;})); goto _LL788; _LL792: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp798=* v; struct _tagged_string _temp799=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("enum %.*s attempts to abstract type variable %.*s of kind M",
_temp798.last_plus_one - _temp798.curr, _temp798.curr, _temp799.last_plus_one -
_temp799.curr, _temp799.curr);})); goto _LL788; _LL794: goto _LL788; _LL788:;}}{
void* _temp800=(*(( struct _tuple0*)( ed->name)->v)).f1; struct Cyc_List_List*
_temp808; struct Cyc_List_List* _temp810; _LL802: if(( unsigned int) _temp800 >
1u?(( struct _enum_struct*) _temp800)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL809:
_temp808=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp800)->f1;
if( _temp808 == 0){ goto _LL803;} else{ goto _LL804;}} else{ goto _LL804;}
_LL804: if(( unsigned int) _temp800 > 1u?(( struct _enum_struct*) _temp800)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL811: _temp810=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp800)->f1; if( _temp810 == 0){ goto _LL805;} else{
goto _LL806;}} else{ goto _LL806;} _LL806: goto _LL807; _LL803: goto _LL801;
_LL805: goto _LL801; _LL807: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp812=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp813; _temp813.curr= _temp812; _temp813.base= _temp812;
_temp813.last_plus_one= _temp812 + 43; _temp813;})); return; _LL801:;}(*((
struct _tuple0*)( ed->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp814=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp814->tag= Cyc_Absyn_Abs_n_tag; _temp814->f1= te->ns;( void*) _temp814;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp815=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->enumdecls, v); struct Cyc_Core_Opt _temp821; struct
Cyc_Absyn_Enumdecl** _temp822; _LL817: if( _temp815 == 0){ goto _LL818;} else{
goto _LL819;} _LL819: if( _temp815 == 0){ goto _LL816;} else{ _temp821=*
_temp815; _LL823: _temp822=( struct Cyc_Absyn_Enumdecl**) _temp821.v; goto
_LL820;} _LL818: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp824=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp824[ 0]= ed; _temp824;}));
if( ed->fields == 0){ return;} goto _LL816; _LL820: { struct Cyc_Absyn_Enumdecl*
ed2=* _temp822; struct Cyc_List_List* tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp825=* v; xprintf("redeclaration of enum %.*s has a different number of type parameters",
_temp825.last_plus_one - _temp825.curr, _temp825.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp826=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp826->tag= Cyc_Absyn_Forward_constr_tag;
_temp826->f1= c2;( void*) _temp826;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp827=* v; struct
_tagged_string _temp828= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp829=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp830= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of enum %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp827.last_plus_one - _temp827.curr, _temp827.curr, _temp828.last_plus_one -
_temp828.curr, _temp828.curr, _temp829.last_plus_one - _temp829.curr, _temp829.curr,
_temp830.last_plus_one - _temp830.curr, _temp830.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp831=( char*)"enum";
struct _tagged_string _temp832; _temp832.curr= _temp831; _temp832.base= _temp831;
_temp832.last_plus_one= _temp831 + 5; _temp832;}), v));{ struct _tuple7 _temp834=({
struct _tuple7 _temp833; _temp833.f1= ed->fields; _temp833.f2= ed2->fields;
_temp833;}); struct Cyc_Core_Opt* _temp844; struct Cyc_Core_Opt* _temp846;
struct Cyc_Core_Opt* _temp848; struct Cyc_Core_Opt* _temp850; struct Cyc_Core_Opt*
_temp852; struct Cyc_Core_Opt* _temp854; struct Cyc_Core_Opt* _temp856; struct
Cyc_Core_Opt* _temp858; _LL836: _LL847: _temp846= _temp834.f1; if( _temp846 == 0){
goto _LL845;} else{ goto _LL838;} _LL845: _temp844= _temp834.f2; if( _temp844 ==
0){ goto _LL837;} else{ goto _LL838;} _LL838: _LL851: _temp850= _temp834.f1;
goto _LL849; _LL849: _temp848= _temp834.f2; if( _temp848 == 0){ goto _LL839;}
else{ goto _LL840;} _LL840: _LL855: _temp854= _temp834.f1; if( _temp854 == 0){
goto _LL853;} else{ goto _LL842;} _LL853: _temp852= _temp834.f2; goto _LL841;
_LL842: _LL859: _temp858= _temp834.f1; goto _LL857; _LL857: _temp856= _temp834.f2;
goto _LL843; _LL837: return; _LL839:* _temp822= ed; goto _LL835; _LL841: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL843: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp860=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp860->hd=( void*)({ struct _tuple4* _temp861=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp861->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp861->f2=({ struct Cyc_Absyn_VarType_struct* _temp862=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp862->tag= Cyc_Absyn_VarType_tag;
_temp862->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp862;}); _temp861;});
_temp860->tl= inst; _temp860;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ed->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ed2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){(*(( struct Cyc_Absyn_Enumfield*)
f1s->hd)->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp863=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp863->tag= Cyc_Absyn_Abs_n_tag;
_temp863->f1= te->ns;( void*) _temp863;});{ struct Cyc_List_List* alltvs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs,(( struct Cyc_Absyn_Enumfield*) f1s->hd)->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Enumfield*) f1s->hd)->loc, alltvs);{ struct Cyc_List_List* typs=((
struct Cyc_Absyn_Enumfield*) f1s->hd)->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Enumfield*)
f1s->hd)->loc, te, alltvs, Cyc_Absyn_MemKind, t);}} Cyc_Tc_field_redecl_okay((
struct Cyc_Absyn_Enumfield*) f1s->hd,( struct Cyc_Absyn_Enumfield*) f2s->hd,
inst, te,( struct _tagged_string)({ char* _temp864=( char*)"enum"; struct
_tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 5; _temp865;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp866=* v; struct _tagged_string _temp867=*(*((
struct Cyc_Absyn_Enumfield*) f1s->hd)->name).f2; xprintf("redeclaration of enum %.*s has extra field %.*s",
_temp866.last_plus_one - _temp866.curr, _temp866.curr, _temp867.last_plus_one -
_temp867.curr, _temp867.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp868=* v; struct _tagged_string _temp869=*(*(( struct
Cyc_Absyn_Enumfield*) f2s->hd)->name).f2; xprintf("redeclaration of enum %.*s is missing field %.*s",
_temp868.last_plus_one - _temp868.curr, _temp868.curr, _temp869.last_plus_one -
_temp869.curr, _temp869.curr);}));} return;}} _LL835:;} goto _LL816;}} _LL816:;}{
struct Cyc_Absyn_Enumdecl** edp=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v);* edp=({
struct Cyc_Absyn_Enumdecl* _temp870=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp870->sc=( void*)(( void*) ed->sc);
_temp870->name= ed->name; _temp870->tvs= tvs; _temp870->fields= 0; _temp870;});{
struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(
ed->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*) fs->hd; if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){
Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp871=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp871.last_plus_one - _temp871.curr,
_temp871.curr);}));} prev_fields=({ struct Cyc_List_List* _temp872=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp872->hd=( void*)(* f->name).f2;
_temp872->tl= prev_fields; _temp872;}); if( f->tag != 0){ Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp873= f->tag; if( _temp873 ==
0){ _throw( Null_Exception);} _temp873;}));}{ struct Cyc_List_List* tvs= f->tvs;
for( 0; tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)
tvs->hd; struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp874=( void*) c->v; void*
_temp882; _LL876: if( _temp874 == Cyc_Absyn_No_constr){ goto _LL877;} else{ goto
_LL878;} _LL878: if(( unsigned int) _temp874 > 1u?(( struct _enum_struct*)
_temp874)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL883: _temp882=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp874)->f1; if( _temp882 == Cyc_Absyn_MemKind){
goto _LL879;} else{ goto _LL880;}} else{ goto _LL880;} _LL880: goto _LL881;
_LL877:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp884=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp884->tag= Cyc_Absyn_Eq_constr_tag; _temp884->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp884;})); goto _LL875; _LL879: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp885=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp885.last_plus_one - _temp885.curr, _temp885.curr);})); goto _LL875; _LL881:
goto _LL875; _LL875:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs);
Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List* typs= f->typs;
for( 0; typs != 0; typs= typs->tl){ void* t=(*(( struct _tuple5*) typs->hd)).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs, Cyc_Absyn_MemKind, t);}}(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp886=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp886->tag= Cyc_Absyn_Abs_n_tag;
_temp886->f1= te->ns;( void*) _temp886;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple6* _temp887=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp887->f1=({ struct Cyc_Tcenv_EnumRes_struct*
_temp888=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp888->tag= Cyc_Tcenv_EnumRes_tag; _temp888->f1= ed; _temp888->f2= f;( void*)
_temp888;}); _temp887->f2= 1; _temp887;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl**
data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl**
_temp889=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*));
_temp889[ 0]= ed; _temp889;}));* edp= ed;}}}} static void Cyc_Tc_tcXenumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Xenumdecl* xd){ struct _tagged_string* v=(* xd->name).f2;
struct Cyc_Core_Opt* xd2o;{ struct _handler_cons _temp890; _push_handler(&
_temp890);{ struct _xenum_struct* _temp891=( struct _xenum_struct*) setjmp(
_temp890.handler); if( ! _temp891){ xd2o= Cyc_Tcenv_lookup_xenumdecl( te, loc,
xd->name);; _pop_handler();} else{ struct _xenum_struct* _temp893= _temp891;
_LL895: if( _temp893->tag == Cyc_Dict_Absent_tag){ goto _LL896;} else{ goto
_LL897;} _LL897: goto _LL898; _LL896: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp899= Cyc_Absynpp_qvar2string( xd->name); xprintf("qualified xenum declaration %.*s is not an existing xenum",
_temp899.last_plus_one - _temp899.curr, _temp899.curr);})); return; _LL898:(
void) _throw( _temp893); _LL894:;}}}{ void* _temp900=(* xd->name).f1; struct Cyc_List_List*
_temp910; struct Cyc_List_List* _temp912; struct Cyc_List_List* _temp914; _LL902:
if( _temp900 == Cyc_Absyn_Loc_n){ goto _LL903;} else{ goto _LL904;} _LL904: if((
unsigned int) _temp900 > 1u?(( struct _enum_struct*) _temp900)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL911: _temp910=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp900)->f1; if( _temp910 == 0){ goto _LL905;} else{ goto _LL906;}} else{ goto
_LL906;} _LL906: if(( unsigned int) _temp900 > 1u?(( struct _enum_struct*)
_temp900)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL913: _temp912=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp900)->f1; goto _LL907;} else{ goto _LL908;}
_LL908: if(( unsigned int) _temp900 > 1u?(( struct _enum_struct*) _temp900)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL915: _temp914=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp900)->f1; goto _LL909;} else{ goto _LL901;} _LL903:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp916=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp916->tag= Cyc_Core_Impossible_tag;
_temp916->f1=( struct _tagged_string)({ char* _temp917=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp918; _temp918.curr= _temp917; _temp918.base= _temp917;
_temp918.last_plus_one= _temp917 + 19; _temp918;});( struct _xenum_struct*)
_temp916;})); _LL905:(* xd->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp919=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp919->tag= Cyc_Absyn_Abs_n_tag; _temp919->f1= te->ns;( void*) _temp919;});
goto _LL901; _LL907:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp920=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp920->tag= Cyc_Core_Impossible_tag; _temp920->f1=( struct _tagged_string)({
char* _temp921=( char*)"tcXenumdecl: Rel_n non-null"; struct _tagged_string
_temp922; _temp922.curr= _temp921; _temp922.base= _temp921; _temp922.last_plus_one=
_temp921 + 28; _temp922;});( struct _xenum_struct*) _temp920;})); _LL909:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp923=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp923->tag= Cyc_Core_Impossible_tag;
_temp923->f1=( struct _tagged_string)({ char* _temp924=( char*)"tcXenumdecl: Abs_n";
struct _tagged_string _temp925; _temp925.curr= _temp924; _temp925.base= _temp924;
_temp925.last_plus_one= _temp924 + 19; _temp925;});( struct _xenum_struct*)
_temp923;})); _LL901:;} if( xd2o == 0){ struct Cyc_Absyn_Xenumdecl** x=( struct
Cyc_Absyn_Xenumdecl**)({ struct _tuple8* _temp926=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp926->f1=({ struct Cyc_Absyn_Xenumdecl* _temp927=(
struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp927->sc=( void*)(( void*) xd->sc); _temp927->name= xd->name; _temp927->fields=
0; _temp927;}); _temp926;}); xd2o=({ struct Cyc_Core_Opt* _temp928=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp928->v=( void*) x; _temp928;});
ge->xenumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Xenumdecl** data)) Cyc_Dict_insert)( ge->xenumdecls,
v, x);} else{ struct Cyc_Absyn_Xenumdecl* x=*(( struct Cyc_Absyn_Xenumdecl**)
xd2o->v);( void*)( x->sc=( void*)(( void*(*)( void* s1, struct Cyc_Core_Opt*
fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp929=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp929->v=( void*) 0;
_temp929;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp930=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp930->v=( void*) 0;
_temp930;}), loc,( struct _tagged_string)({ char* _temp931=( char*)"xenum";
struct _tagged_string _temp932; _temp932.curr= _temp931; _temp932.base= _temp931;
_temp932.last_plus_one= _temp931 + 6; _temp932;}), v));}{ struct Cyc_Absyn_Xenumdecl*
xd_result=*(( struct Cyc_Absyn_Xenumdecl**) xd2o->v); struct Cyc_List_List* fs=
xd->fields; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct
Cyc_Absyn_Enumfield*) fs->hd;{ void* _temp933=(* f->name).f1; struct Cyc_List_List*
_temp943; struct Cyc_List_List* _temp945; struct Cyc_List_List* _temp947; _LL935:
if(( unsigned int) _temp933 > 1u?(( struct _enum_struct*) _temp933)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL944: _temp943=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp933)->f1; if( _temp943 == 0){ goto _LL936;} else{ goto _LL937;}} else{ goto
_LL937;} _LL937: if(( unsigned int) _temp933 > 1u?(( struct _enum_struct*)
_temp933)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL946: _temp945=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp933)->f1; goto _LL938;} else{ goto _LL939;}
_LL939: if(( unsigned int) _temp933 > 1u?(( struct _enum_struct*) _temp933)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL948: _temp947=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp933)->f1; goto _LL940;} else{ goto _LL941;} _LL941:
if( _temp933 == Cyc_Absyn_Loc_n){ goto _LL942;} else{ goto _LL934;} _LL936:(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp949=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp949->tag= Cyc_Absyn_Abs_n_tag;
_temp949->f1= te->ns;( void*) _temp949;}); goto _LL934; _LL938: goto _LL940;
_LL940: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp950=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp951; _temp951.curr= _temp950; _temp951.base= _temp950;
_temp951.last_plus_one= _temp950 + 40; _temp951;})); goto _LL934; _LL942:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp952=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp952->tag= Cyc_Core_Impossible_tag;
_temp952->f1=( struct _tagged_string)({ char* _temp953=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp954; _temp954.curr= _temp953; _temp954.base= _temp953;
_temp954.last_plus_one= _temp953 + 19; _temp954;});( struct _xenum_struct*)
_temp952;})); goto _LL934; _LL934:;}{ struct Cyc_List_List* tvs= f->tvs; for( 0;
tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*) tvs->hd;
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp955=( void*) c->v; void*
_temp963; _LL957: if( _temp955 == Cyc_Absyn_No_constr){ goto _LL958;} else{ goto
_LL959;} _LL959: if(( unsigned int) _temp955 > 1u?(( struct _enum_struct*)
_temp955)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL964: _temp963=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp955)->f1; if( _temp963 == Cyc_Absyn_MemKind){
goto _LL960;} else{ goto _LL961;}} else{ goto _LL961;} _LL961: goto _LL962;
_LL958:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp965=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp965->tag= Cyc_Absyn_Eq_constr_tag; _temp965->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp965;})); goto _LL956; _LL960: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp966=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp966.last_plus_one - _temp966.curr, _temp966.curr);})); goto _LL956; _LL962:
goto _LL956; _LL956:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type( f->loc, te,
tvs, Cyc_Absyn_MemKind, t);}}{ int dup= 0;{ struct Cyc_List_List* prev_fs=
xd_result->fields; for( 0; prev_fs != 0; prev_fs= prev_fs->tl){ struct Cyc_Absyn_Enumfield*
prev=( struct Cyc_Absyn_Enumfield*) prev_fs->hd; if( Cyc_Absyn_qvar_cmp( f->name,
prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp967=( char*)"xenum"; struct _tagged_string _temp968;
_temp968.curr= _temp967; _temp968.base= _temp967; _temp968.last_plus_one=
_temp967 + 6; _temp968;}), v); dup= 1; break;}}} if( dup){ continue;} xd_result->fields=({
struct Cyc_List_List* _temp969=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp969->hd=( void*) f; _temp969->tl= xd_result->fields;
_temp969;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List* ns_f;{ void*
_temp970=(* f->name).f1; struct Cyc_List_List* _temp978; struct Cyc_List_List*
_temp980; _LL972: if(( unsigned int) _temp970 > 1u?(( struct _enum_struct*)
_temp970)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL979: _temp978=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp970)->f1; goto _LL973;} else{ goto _LL974;}
_LL974: if( _temp970 == Cyc_Absyn_Loc_n){ goto _LL975;} else{ goto _LL976;}
_LL976: if(( unsigned int) _temp970 > 1u?(( struct _enum_struct*) _temp970)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL981: _temp980=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp970)->f1; goto _LL977;} else{ goto _LL971;} _LL973:
ns_f= _temp978; goto _LL971; _LL975: goto _LL977; _LL977:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp982=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp982->tag= Cyc_Core_Impossible_tag;
_temp982->f1=( struct _tagged_string)({ char* _temp983=( char*)"tcXenumdecl: Rel_n or Loc_n";
struct _tagged_string _temp984; _temp984.curr= _temp983; _temp984.base= _temp983;
_temp984.last_plus_one= _temp983 + 28; _temp984;});( struct _xenum_struct*)
_temp982;})); goto _LL971; _LL971:;}{ struct _handler_cons _temp985;
_push_handler(& _temp985);{ struct _xenum_struct* _temp986=( struct
_xenum_struct*) setjmp( _temp985.handler); if( ! _temp986){ ge_f=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
ns_f);; _pop_handler();} else{ struct _xenum_struct* _temp988= _temp986; _LL990:
if( _temp988->tag == Cyc_Dict_Absent_tag){ goto _LL991;} else{ goto _LL992;}
_LL992: goto _LL993; _LL991: Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp994= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xenum field %.*s",
_temp994.last_plus_one - _temp994.curr, _temp994.curr);})); return; _LL993:(
void) _throw( _temp988); _LL989:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp995=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp995->f1=({ struct Cyc_Tcenv_XenumRes_struct*
_temp996=( struct Cyc_Tcenv_XenumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XenumRes_struct));
_temp996->tag= Cyc_Tcenv_XenumRes_tag; _temp996->f1= xd_result; _temp996->f2= f;(
void*) _temp996;}); _temp995->f2= 1; _temp995;}));}}}}}} static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment* loc, void* sc){ void* _temp997= sc; _LL999: if(
_temp997 == Cyc_Absyn_Static){ goto _LL1000;} else{ goto _LL1001;} _LL1001: if(
_temp997 == Cyc_Absyn_Abstract){ goto _LL1002;} else{ goto _LL1003;} _LL1003:
if( _temp997 == Cyc_Absyn_Public){ goto _LL1004;} else{ goto _LL1005;} _LL1005:
if( _temp997 == Cyc_Absyn_Extern){ goto _LL1006;} else{ goto _LL1007;} _LL1007:
if( _temp997 == Cyc_Absyn_ExternC){ goto _LL1008;} else{ goto _LL998;} _LL1000:
Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1009=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1010; _temp1010.curr= _temp1009; _temp1010.base=
_temp1009; _temp1010.last_plus_one= _temp1009 + 44; _temp1010;})); return 0;
_LL1002: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1011=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1012; _temp1012.curr= _temp1011; _temp1012.base=
_temp1011; _temp1012.last_plus_one= _temp1011 + 46; _temp1012;})); return 0;
_LL1004: return 1; _LL1006: return 1; _LL1008: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1013=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1014; _temp1014.curr= _temp1013; _temp1014.base=
_temp1013; _temp1014.last_plus_one= _temp1013 + 30; _temp1014;})); return 1;
_LL998:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*) ds->hd; struct Cyc_Position_Segment* loc= d->loc;
void* _temp1015=( void*) d->r; int _temp1039; struct Cyc_Absyn_Exp* _temp1041;
struct Cyc_Core_Opt* _temp1043; struct Cyc_Core_Opt* _temp1045; struct Cyc_Absyn_Pat*
_temp1047; struct Cyc_Absyn_Vardecl* _temp1049; struct Cyc_Absyn_Fndecl*
_temp1051; struct Cyc_Absyn_Typedefdecl* _temp1053; struct Cyc_Absyn_Structdecl*
_temp1055; struct Cyc_Absyn_Uniondecl* _temp1057; struct Cyc_Absyn_Enumdecl*
_temp1059; struct Cyc_Absyn_Xenumdecl* _temp1061; struct Cyc_List_List*
_temp1063; struct _tagged_string* _temp1065; struct Cyc_List_List* _temp1067;
struct _tuple0* _temp1069; struct _tuple0 _temp1071; struct _tagged_string*
_temp1072; void* _temp1074; struct Cyc_List_List* _temp1076; _LL1017: if(((
struct _enum_struct*) _temp1015)->tag == Cyc_Absyn_Let_d_tag){ _LL1048:
_temp1047=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1015)->f1;
goto _LL1046; _LL1046: _temp1045=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1015)->f2; goto _LL1044; _LL1044: _temp1043=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1015)->f3; goto _LL1042; _LL1042: _temp1041=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1015)->f4; goto
_LL1040; _LL1040: _temp1039=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1015)->f5;
goto _LL1018;} else{ goto _LL1019;} _LL1019: if((( struct _enum_struct*)
_temp1015)->tag == Cyc_Absyn_Var_d_tag){ _LL1050: _temp1049=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1015)->f1; goto _LL1020;} else{ goto
_LL1021;} _LL1021: if((( struct _enum_struct*) _temp1015)->tag == Cyc_Absyn_Fn_d_tag){
_LL1052: _temp1051=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1015)->f1; goto _LL1022;} else{ goto _LL1023;} _LL1023: if((( struct
_enum_struct*) _temp1015)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1054: _temp1053=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1015)->f1;
goto _LL1024;} else{ goto _LL1025;} _LL1025: if((( struct _enum_struct*)
_temp1015)->tag == Cyc_Absyn_Struct_d_tag){ _LL1056: _temp1055=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1015)->f1; goto _LL1026;} else{ goto
_LL1027;} _LL1027: if((( struct _enum_struct*) _temp1015)->tag == Cyc_Absyn_Union_d_tag){
_LL1058: _temp1057=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1015)->f1; goto _LL1028;} else{ goto _LL1029;} _LL1029: if((( struct
_enum_struct*) _temp1015)->tag == Cyc_Absyn_Enum_d_tag){ _LL1060: _temp1059=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1015)->f1;
goto _LL1030;} else{ goto _LL1031;} _LL1031: if((( struct _enum_struct*)
_temp1015)->tag == Cyc_Absyn_Xenum_d_tag){ _LL1062: _temp1061=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1015)->f1; goto _LL1032;} else{ goto
_LL1033;} _LL1033: if((( struct _enum_struct*) _temp1015)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1066: _temp1065=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1015)->f1; goto _LL1064; _LL1064: _temp1063=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1015)->f2; goto _LL1034;} else{ goto
_LL1035;} _LL1035: if((( struct _enum_struct*) _temp1015)->tag == Cyc_Absyn_Using_d_tag){
_LL1070: _temp1069=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1015)->f1; _temp1071=* _temp1069; _LL1075: _temp1074= _temp1071.f1; goto
_LL1073; _LL1073: _temp1072= _temp1071.f2; goto _LL1068; _LL1068: _temp1067=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1015)->f2; goto
_LL1036;} else{ goto _LL1037;} _LL1037: if((( struct _enum_struct*) _temp1015)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL1077: _temp1076=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1015)->f1; goto _LL1038;} else{ goto
_LL1016;} _LL1018: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1078=( char*)"top level let-declarations are not implemented"; struct
_tagged_string _temp1079; _temp1079.curr= _temp1078; _temp1079.base= _temp1078;
_temp1079.last_plus_one= _temp1078 + 47; _temp1079;})); goto _LL1016; _LL1020:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1049->sc): 0){( void*)(
_temp1049->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcVardecl( te, ge, loc,
_temp1049); goto _LL1016; _LL1022: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1051->sc): 0){( void*)( _temp1051->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1051); goto _LL1016; _LL1024: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1053); goto _LL1016; _LL1026: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1055->sc): 0){( void*)( _temp1055->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1055); goto _LL1016; _LL1028: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1057->sc): 0){( void*)(
_temp1057->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcUniondecl( te, ge, loc,
_temp1057); goto _LL1016; _LL1030: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1059->sc): 0){( void*)( _temp1059->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcEnumdecl( te, ge, loc, _temp1059); goto _LL1016; _LL1032: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1061->sc): 0){( void*)(
_temp1061->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcXenumdecl( te, ge, loc,
_temp1061); goto _LL1016; _LL1034: { struct Cyc_List_List* ns= te->ns; struct
Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp1080=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1080->hd=(
void*) _temp1065; _temp1080->tl= 0; _temp1080;})); if( !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces, _temp1065)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
ge->namespaces, _temp1065); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
ns2, Cyc_Tcenv_empty_genv());} te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1063,
in_externC); te->ns= ns; goto _LL1016;} _LL1036: { struct _tagged_string* first;
struct Cyc_List_List* rest;{ void* _temp1081= _temp1074; struct Cyc_List_List*
_temp1093; struct Cyc_List_List* _temp1095; struct Cyc_List_List* _temp1097;
struct Cyc_List_List _temp1099; struct Cyc_List_List* _temp1100; struct
_tagged_string* _temp1102; struct Cyc_List_List* _temp1104; struct Cyc_List_List
_temp1106; struct Cyc_List_List* _temp1107; struct _tagged_string* _temp1109;
_LL1083: if( _temp1081 == Cyc_Absyn_Loc_n){ goto _LL1084;} else{ goto _LL1085;}
_LL1085: if(( unsigned int) _temp1081 > 1u?(( struct _enum_struct*) _temp1081)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL1094: _temp1093=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp1081)->f1; if( _temp1093 == 0){ goto _LL1086;}
else{ goto _LL1087;}} else{ goto _LL1087;} _LL1087: if(( unsigned int) _temp1081
> 1u?(( struct _enum_struct*) _temp1081)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL1096: _temp1095=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1081)->f1; if( _temp1095 == 0){ goto _LL1088;} else{ goto _LL1089;}} else{
goto _LL1089;} _LL1089: if(( unsigned int) _temp1081 > 1u?(( struct _enum_struct*)
_temp1081)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1098: _temp1097=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1081)->f1; if( _temp1097 == 0){ goto
_LL1091;} else{ _temp1099=* _temp1097; _LL1103: _temp1102=( struct
_tagged_string*) _temp1099.hd; goto _LL1101; _LL1101: _temp1100=( struct Cyc_List_List*)
_temp1099.tl; goto _LL1090;}} else{ goto _LL1091;} _LL1091: if(( unsigned int)
_temp1081 > 1u?(( struct _enum_struct*) _temp1081)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1105: _temp1104=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1081)->f1; if( _temp1104 == 0){ goto _LL1082;} else{ _temp1106=* _temp1104;
_LL1110: _temp1109=( struct _tagged_string*) _temp1106.hd; goto _LL1108; _LL1108:
_temp1107=( struct Cyc_List_List*) _temp1106.tl; goto _LL1092;}} else{ goto
_LL1082;} _LL1084: goto _LL1086; _LL1086: goto _LL1088; _LL1088: first=
_temp1072; rest= 0; goto _LL1082; _LL1090: _temp1109= _temp1102; _temp1107=
_temp1100; goto _LL1092; _LL1092: first= _temp1109; rest=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1107,({
struct Cyc_List_List* _temp1111=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1111->hd=( void*) _temp1072; _temp1111->tl= 0;
_temp1111;})); goto _LL1082; _LL1082:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace(
te, loc, first, rest); ge->availables=({ struct Cyc_List_List* _temp1112=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1112->hd=(
void*) ns2; _temp1112->tl= ge->availables; _temp1112;}); Cyc_Tc_tc_decls( te,
_temp1067, in_externC); ge->availables=( ge->availables)->tl; goto _LL1016;}}
_LL1038: Cyc_Tc_tc_decls( te, _temp1076, 1); goto _LL1016; _LL1016:;}} static
char _temp1115[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp1115, _temp1115, _temp1115 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1116=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1116->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1117=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1117->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1118=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1118->tag= Cyc_Absyn_Namespace_d_tag; _temp1118->f1= Cyc_Tc_cyc_ns;
_temp1118->f2= ds;( void*) _temp1118;}); _temp1117->loc= 0; _temp1117;});
_temp1116->tl= 0; _temp1116;});} Cyc_Tc_tc_decls( te, ds, 0);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Decl* d){ void* _temp1119=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1129; struct Cyc_List_List* _temp1131; struct Cyc_List_List**
_temp1133; struct _tuple0* _temp1134; struct Cyc_List_List* _temp1136; struct
Cyc_List_List** _temp1138; struct _tagged_string* _temp1139; _LL1121: if(((
struct _enum_struct*) _temp1119)->tag == Cyc_Absyn_Var_d_tag){ _LL1130:
_temp1129=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1119)->f1; goto _LL1122;} else{ goto _LL1123;} _LL1123: if((( struct
_enum_struct*) _temp1119)->tag == Cyc_Absyn_Using_d_tag){ _LL1135: _temp1134=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1119)->f1; goto
_LL1132; _LL1132: _temp1131=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1119)->f2; _temp1133=&(( struct Cyc_Absyn_Using_d_struct*) _temp1119)->f2;
goto _LL1124;} else{ goto _LL1125;} _LL1125: if((( struct _enum_struct*)
_temp1119)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1140: _temp1139=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1119)->f1; goto
_LL1137; _LL1137: _temp1136=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1119)->f2; _temp1138=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp1119)->f2;
goto _LL1126;} else{ goto _LL1127;} _LL1127: goto _LL1128; _LL1122: if(( void*)
_temp1129->sc != Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp1143; struct
_tagged_string* _temp1144; void* _temp1146; struct _tuple0* _temp1141= _temp1129->name;
_temp1143=* _temp1141; _LL1147: _temp1146= _temp1143.f1; goto _LL1145; _LL1145:
_temp1144= _temp1143.f2; goto _LL1142; _LL1142: { struct Cyc_List_List* ns;{
void* _temp1148= _temp1146; struct Cyc_List_List* _temp1156; struct Cyc_List_List*
_temp1158; _LL1150: if( _temp1148 == Cyc_Absyn_Loc_n){ goto _LL1151;} else{ goto
_LL1152;} _LL1152: if(( unsigned int) _temp1148 > 1u?(( struct _enum_struct*)
_temp1148)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1157: _temp1156=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1148)->f1; goto _LL1153;} else{ goto
_LL1154;} _LL1154: if(( unsigned int) _temp1148 > 1u?(( struct _enum_struct*)
_temp1148)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1159: _temp1158=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1148)->f1; goto _LL1155;} else{ goto
_LL1149;} _LL1151: ns= 0; goto _LL1149; _LL1153: ns= _temp1156; goto _LL1149;
_LL1155: ns= _temp1158; goto _LL1149; _LL1149:;}{ struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, ns); return(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, _temp1144)).f2;}}}
_LL1124: _temp1138= _temp1133; goto _LL1126; _LL1126:* _temp1138= Cyc_Tc_treeshake(
te,* _temp1138); return 1; _LL1128: return 1; _LL1120:;} struct Cyc_List_List*
Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ return((
struct Cyc_List_List*(*)( int(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Decl*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed,
te, ds);}