#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ void*
f1; int f2; } ; struct _tuple7{ struct Cyc_Absyn_Structdecl* f1; } ; struct
_tuple8{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; struct _tuple9{
struct Cyc_Absyn_Enumdecl* f1; } ; struct _tuple10{ struct Cyc_Absyn_Xenumdecl*
f1; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern char
Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
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
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl** f3; }
; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl** f2; } ; extern const
int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag;
struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag; struct
Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void* Cyc_Absyn_FloatType;
extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
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
struct Cyc_List_List*, struct Cyc_List_List*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern int Cyc_Tcexp_is_const_exp(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int new_block); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List*); static void Cyc_Tc_redecl_err( struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v, struct _tagged_string
new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
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
_LL175: if(( unsigned int) _temp173 > 5u?(( struct _enum_struct*) _temp173)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL180: _temp179=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp173)->f1; goto _LL176;} else{ goto _LL177;}
_LL177: goto _LL178; _LL176: return 1; _LL178: return 0; _LL174:;} static struct
Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List* atts){
void* _temp181= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp187;
struct Cyc_List_List* _temp189; struct Cyc_List_List** _temp191; int _temp192;
struct Cyc_List_List* _temp194; void* _temp196; struct Cyc_Core_Opt* _temp198;
struct Cyc_List_List* _temp200; _LL183: if(( unsigned int) _temp181 > 5u?((
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
_temp236->f1= te->ns;( void*) _temp236;}); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tc_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts); if(
sc == Cyc_Absyn_Extern? 1: sc == Cyc_Absyn_ExternC){ if( vd->initializer != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp237=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 47; _temp238;}));}} else{ if( ! Cyc_Tc_is_function_type(
t)){ for( 0; atts != 0; atts= atts->tl){ void* _temp239=( void*) atts->hd; int
_temp261; struct _tagged_string _temp263; _LL241: if(( unsigned int) _temp239 >
15u?(( struct _enum_struct*) _temp239)->tag == Cyc_Absyn_Aligned_att_tag: 0){
_LL262: _temp261=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp239)->f1;
goto _LL242;} else{ goto _LL243;} _LL243: if( _temp239 == Cyc_Absyn_Packed_att){
goto _LL244;} else{ goto _LL245;} _LL245: if(( unsigned int) _temp239 > 15u?((
struct _enum_struct*) _temp239)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL264:
_temp263=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp239)->f1; goto _LL246;} else{ goto _LL247;} _LL247: if( _temp239 == Cyc_Absyn_Nocommon_att){
goto _LL248;} else{ goto _LL249;} _LL249: if( _temp239 == Cyc_Absyn_Shared_att){
goto _LL250;} else{ goto _LL251;} _LL251: if( _temp239 == Cyc_Absyn_Unused_att){
goto _LL252;} else{ goto _LL253;} _LL253: if( _temp239 == Cyc_Absyn_Weak_att){
goto _LL254;} else{ goto _LL255;} _LL255: if( _temp239 == Cyc_Absyn_Dllimport_att){
goto _LL256;} else{ goto _LL257;} _LL257: if( _temp239 == Cyc_Absyn_Dllexport_att){
goto _LL258;} else{ goto _LL259;} _LL259: goto _LL260; _LL242: continue; _LL244:
continue; _LL246: continue; _LL248: continue; _LL250: continue; _LL252:
continue; _LL254: continue; _LL256: continue; _LL258: continue; _LL260: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp265= Cyc_Absyn_attribute2string(( void*) atts->hd);
struct _tagged_string _temp266= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp265.last_plus_one - _temp265.curr, _temp265.curr, _temp266.last_plus_one -
_temp266.curr, _temp266.curr);})); goto _LL240; _LL240:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp267= vd->initializer; if( _temp267 == 0){ _throw( Null_Exception);}
_temp267;});{ void* _temp268= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp274; struct Cyc_Absyn_Tqual* _temp276; void* _temp278; _LL270: if((
unsigned int) _temp268 > 5u?(( struct _enum_struct*) _temp268)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL279: _temp278=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp268)->f1;
goto _LL277; _LL277: _temp276=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp268)->f2; goto _LL275; _LL275: _temp274=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp268)->f3; goto _LL271;} else{ goto _LL272;}
_LL272: goto _LL273; _LL271: if( Cyc_Evexp_eval_const_uint_exp( _temp274) ==(
unsigned int) 0){ void* _temp280=( void*) e->r; void* _temp290; struct
_tagged_string _temp292; int _temp294; struct Cyc_List_List* _temp296; struct
Cyc_Core_Opt* _temp298; struct Cyc_List_List* _temp300; int _temp302; _LL282:
if((( struct _enum_struct*) _temp280)->tag == Cyc_Absyn_Const_e_tag){ _LL291:
_temp290=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp280)->f1; if((
unsigned int) _temp290 > 1u?(( struct _enum_struct*) _temp290)->tag == Cyc_Absyn_String_c_tag:
0){ _LL295: _temp294=( int)(( struct Cyc_Absyn_String_c_struct*) _temp290)->f1;
goto _LL293; _LL293: _temp292=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp290)->f2; goto _LL283;} else{ goto _LL284;}} else{ goto _LL284;} _LL284:
if((( struct _enum_struct*) _temp280)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL299: _temp298=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp280)->f1; goto _LL297; _LL297: _temp296=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp280)->f2; goto _LL285;} else{ goto
_LL286;} _LL286: if((( struct _enum_struct*) _temp280)->tag == Cyc_Absyn_Array_e_tag){
_LL303: _temp302=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp280)->f1; goto
_LL301; _LL301: _temp300=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp280)->f2; goto _LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL283: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp304=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp304->tag= Cyc_Absyn_ArrayType_tag; _temp304->f1=( void*) _temp278; _temp304->f2=
_temp276; _temp304->f3= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp305=
_temp292;( unsigned int)( _temp305.last_plus_one - _temp305.curr);}), 0);( void*)
_temp304;})); goto _LL281; _LL285: _temp300= _temp296; goto _LL287; _LL287: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp306=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp306->tag= Cyc_Absyn_ArrayType_tag; _temp306->f1=( void*) _temp278; _temp306->f2=
_temp276; _temp306->f3= Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp300), 0);( void*) _temp306;})); goto _LL281; _LL289:
goto _LL281; _LL281:;} goto _LL269; _LL273: goto _LL269; _LL269:;}}{ void* t2=
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_assign(
te, e, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp307= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp308= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp309= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp307.last_plus_one - _temp307.curr, _temp307.curr, _temp308.last_plus_one -
_temp308.curr, _temp308.curr, _temp309.last_plus_one - _temp309.curr, _temp309.curr);}));}
if( ! Cyc_Tcexp_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp310=( char*)"initializer is not a constant expression";
struct _tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 41; _temp311;}));}}}} else{ for( 0; atts != 0;
atts= atts->tl){ void* _temp312=( void*) atts->hd; int _temp330; int _temp332;
_LL314: if(( unsigned int) _temp312 > 15u?(( struct _enum_struct*) _temp312)->tag
== Cyc_Absyn_Regparm_att_tag: 0){ _LL331: _temp330=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp312)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if( _temp312 == Cyc_Absyn_Stdcall_att){
goto _LL317;} else{ goto _LL318;} _LL318: if( _temp312 == Cyc_Absyn_Cdecl_att){
goto _LL319;} else{ goto _LL320;} _LL320: if( _temp312 == Cyc_Absyn_Noreturn_att){
goto _LL321;} else{ goto _LL322;} _LL322: if( _temp312 == Cyc_Absyn_Const_att){
goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int) _temp312 > 15u?((
struct _enum_struct*) _temp312)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL333:
_temp332=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp312)->f1; goto
_LL325;} else{ goto _LL326;} _LL326: if( _temp312 == Cyc_Absyn_Packed_att){ goto
_LL327;} else{ goto _LL328;} _LL328: goto _LL329; _LL315: goto _LL317; _LL317:
goto _LL319; _LL319: goto _LL321; _LL321: goto _LL323; _LL323:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp334=(
char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 51; _temp335;})); goto _LL313; _LL325: goto
_LL327; _LL327: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp336= Cyc_Absyn_attribute2string((
void*) atts->hd); xprintf("bad attribute %.*s in function declaration", _temp336.last_plus_one
- _temp336.curr, _temp336.curr);})); goto _LL313; _LL329: continue; _LL313:;}}}
ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({
struct _tuple6* _temp337=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp337->f1=({ struct Cyc_Tcenv_VarRes_struct* _temp338=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp338->tag= Cyc_Tcenv_VarRes_tag;
_temp338->f1=( void*)({ struct Cyc_Absyn_Global_b_struct* _temp339=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp339->tag= Cyc_Absyn_Global_b_tag;
_temp339->f1= vd;( void*) _temp339;});( void*) _temp338;}); _temp337->f2=
is_redecl; _temp337;}));}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp340=(* fd->name).f1; struct Cyc_List_List* _temp348; struct Cyc_List_List*
_temp350; _LL342: if(( unsigned int) _temp340 > 1u?(( struct _enum_struct*)
_temp340)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL349: _temp348=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp340)->f1; if( _temp348 == 0){ goto _LL343;}
else{ goto _LL344;}} else{ goto _LL344;} _LL344: if(( unsigned int) _temp340 > 1u?((
struct _enum_struct*) _temp340)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL351:
_temp350=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp340)->f1;
goto _LL345;} else{ goto _LL346;} _LL346: goto _LL347; _LL343: goto _LL341;
_LL345:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp352=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp352=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp353=( char*)"tc: Abs_n in tcFndecl"; struct
_tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 22; _temp354;})};( struct _xenum_struct*)
_temp352;})); _LL347: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp355=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 43; _temp356;})); return; _LL341:;}(* fd->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp357=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp357->tag= Cyc_Absyn_Abs_n_tag;
_temp357->f1= te->ns;( void*) _temp357;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); int is_redecl= Cyc_Tc_var_redecl_okay(
ge, loc, v, t, sc, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts= atts->tl){ void* _temp358=( void*) atts->hd; int
_temp366; _LL360: if( _temp358 == Cyc_Absyn_Packed_att){ goto _LL361;} else{
goto _LL362;} _LL362: if(( unsigned int) _temp358 > 15u?(( struct _enum_struct*)
_temp358)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL367: _temp366=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp358)->f1; goto _LL363;} else{ goto
_LL364;} _LL364: goto _LL365; _LL361: goto _LL363; _LL363: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp368= Cyc_Absyn_attribute2string(( void*) atts->hd);
xprintf("bad attribute %.*s for function", _temp368.last_plus_one - _temp368.curr,
_temp368.curr);})); goto _LL359; _LL365: goto _LL359; _LL359:;}} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple6*
_temp369=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp369->f1=({
struct Cyc_Tcenv_VarRes_struct* _temp370=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp370->tag= Cyc_Tcenv_VarRes_tag;
_temp370->f1=( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp371=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp371->tag= Cyc_Absyn_Funname_b_tag;
_temp371->f1= fd;( void*) _temp371;});( void*) _temp370;}); _temp369->f2=
is_redecl; _temp369;})); te->le=({ struct Cyc_Core_Opt* _temp372=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp372->v=( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp373=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp373->tag= Cyc_Tcenv_Outermost_tag; _temp373->f1=( void*) Cyc_Tcenv_new_fenv(
fd);( void*) _temp373;}); _temp372;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if(
! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp374=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 49; _temp375;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp376=(* td->name).f1; struct Cyc_List_List*
_temp384; struct Cyc_List_List* _temp386; _LL378: if(( unsigned int) _temp376 >
1u?(( struct _enum_struct*) _temp376)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL385:
_temp384=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp376)->f1;
if( _temp384 == 0){ goto _LL379;} else{ goto _LL380;}} else{ goto _LL380;}
_LL380: if(( unsigned int) _temp376 > 1u?(( struct _enum_struct*) _temp376)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL387: _temp386=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp376)->f1; if( _temp386 == 0){ goto _LL381;} else{
goto _LL382;}} else{ goto _LL382;} _LL382: goto _LL383; _LL379: goto _LL377;
_LL381: goto _LL377; _LL383: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp388=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 43; _temp389;})); return; _LL377:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp390=* v; xprintf("redeclaration of typedef %.*s",
_temp390.last_plus_one - _temp390.curr, _temp390.curr);})); return;}(* td->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp391=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp391->tag= Cyc_Absyn_Abs_n_tag;
_temp391->f1= te->ns;( void*) _temp391;}); Cyc_Tcutil_check_unique_tvars( loc,
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs, Cyc_Absyn_AnyKind,( void*) td->defn);
ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs,
v, td);} static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp392=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 47; _temp393;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( sd->name)->v)).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp394=( void*)
atts->hd; int _temp402; _LL396: if( _temp394 == Cyc_Absyn_Packed_att){ goto
_LL397;} else{ goto _LL398;} _LL398: if(( unsigned int) _temp394 > 15u?(( struct
_enum_struct*) _temp394)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL403: _temp402=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp394)->f1; goto _LL399;} else{
goto _LL400;} _LL400: goto _LL401; _LL397: continue; _LL399: continue; _LL401:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp404= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp405=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp404.last_plus_one - _temp404.curr, _temp404.curr, _temp405.last_plus_one -
_temp405.curr, _temp405.curr);})); goto _LL395; _LL395:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp406=( void*) c->v; void*
_temp414; _LL408: if( _temp406 == Cyc_Absyn_No_constr){ goto _LL409;} else{ goto
_LL410;} _LL410: if(( unsigned int) _temp406 > 1u?(( struct _enum_struct*)
_temp406)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL415: _temp414=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp406)->f1; if( _temp414 == Cyc_Absyn_MemKind){
goto _LL411;} else{ goto _LL412;}} else{ goto _LL412;} _LL412: goto _LL413;
_LL409:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp416=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp416->tag= Cyc_Absyn_Eq_constr_tag; _temp416->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp416;})); continue; _LL411: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp417=* v; struct _tagged_string _temp418=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp417.last_plus_one - _temp417.curr, _temp417.curr, _temp418.last_plus_one -
_temp418.curr, _temp418.curr);})); continue; _LL413: continue; _LL407:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp419=(*(( struct _tuple0*)( sd->name)->v)).f1;
struct Cyc_List_List* _temp427; struct Cyc_List_List* _temp429; _LL421: if((
unsigned int) _temp419 > 1u?(( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL428: _temp427=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp419)->f1; if( _temp427 == 0){ goto _LL422;} else{ goto _LL423;}} else{ goto
_LL423;} _LL423: if(( unsigned int) _temp419 > 1u?(( struct _enum_struct*)
_temp419)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL430: _temp429=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp419)->f1; if( _temp429 == 0){ goto _LL424;}
else{ goto _LL425;}} else{ goto _LL425;} _LL425: goto _LL426; _LL422: goto
_LL420; _LL424: goto _LL420; _LL426: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp431=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 43; _temp432;})); return; _LL420:;}(*((
struct _tuple0*)( sd->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp433=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp433->tag= Cyc_Absyn_Abs_n_tag; _temp433->f1= te->ns;( void*) _temp433;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp434=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->structdecls, v); struct Cyc_Core_Opt _temp440; struct
Cyc_Absyn_Structdecl** _temp441; _LL436: if( _temp434 == 0){ goto _LL437;} else{
goto _LL438;} _LL438: if( _temp434 == 0){ goto _LL435;} else{ _temp440=*
_temp434; _LL442: _temp441=( struct Cyc_Absyn_Structdecl**) _temp440.v; goto
_LL439;} _LL437: ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, v,( struct Cyc_Absyn_Structdecl**)({ struct _tuple7* _temp443=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp443->f1= sd; _temp443;}));
if( sd->fields == 0){ return;} goto _LL435; _LL439: { struct Cyc_Absyn_Structdecl*
sd2=* _temp441; struct Cyc_List_List* tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp444=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp444.last_plus_one - _temp444.curr, _temp444.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp445=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp445->tag= Cyc_Absyn_Forward_constr_tag;
_temp445->f1= c2;( void*) _temp445;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp446=* v; struct
_tagged_string _temp447= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp448=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp449= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp446.last_plus_one - _temp446.curr, _temp446.curr, _temp447.last_plus_one -
_temp447.curr, _temp447.curr, _temp448.last_plus_one - _temp448.curr, _temp448.curr,
_temp449.last_plus_one - _temp449.curr, _temp449.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp450=( char*)"struct";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 7; _temp451;}), v));{ struct _tuple8 _temp453=({
struct _tuple8 _temp452; _temp452.f1= sd->fields; _temp452.f2= sd2->fields;
_temp452;}); struct Cyc_Core_Opt* _temp463; struct Cyc_Core_Opt* _temp465;
struct Cyc_Core_Opt* _temp467; struct Cyc_Core_Opt* _temp469; struct Cyc_Core_Opt*
_temp471; struct Cyc_Core_Opt* _temp473; struct Cyc_Core_Opt* _temp475; struct
Cyc_Core_Opt* _temp477; _LL455: _LL466: _temp465= _temp453.f1; if( _temp465 == 0){
goto _LL464;} else{ goto _LL457;} _LL464: _temp463= _temp453.f2; if( _temp463 ==
0){ goto _LL456;} else{ goto _LL457;} _LL457: _LL470: _temp469= _temp453.f1;
goto _LL468; _LL468: _temp467= _temp453.f2; if( _temp467 == 0){ goto _LL458;}
else{ goto _LL459;} _LL459: _LL474: _temp473= _temp453.f1; if( _temp473 == 0){
goto _LL472;} else{ goto _LL461;} _LL472: _temp471= _temp453.f2; goto _LL460;
_LL461: _LL478: _temp477= _temp453.f1; goto _LL476; _LL476: _temp475= _temp453.f2;
goto _LL462; _LL456: return; _LL458:* _temp441= sd; goto _LL454; _LL460: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL462: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp479=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp479->hd=( void*)({ struct _tuple4* _temp480=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp480->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp480->f2=({ struct Cyc_Absyn_VarType_struct* _temp481=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp481->tag= Cyc_Absyn_VarType_tag;
_temp481->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp481;}); _temp480;});
_temp479->tl= inst; _temp479;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes,
sd2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp482=* v;
xprintf("redeclaration of struct %.*s has different attributes", _temp482.last_plus_one
- _temp482.curr, _temp482.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
sd->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( sd2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp485; struct Cyc_List_List* _temp486; void* _temp488; struct Cyc_Absyn_Tqual*
_temp490; struct _tagged_string* _temp492; struct Cyc_Absyn_Structfield*
_temp483=( struct Cyc_Absyn_Structfield*) f1s->hd; _temp485=* _temp483; _LL493:
_temp492=( struct _tagged_string*) _temp485.name; goto _LL491; _LL491: _temp490=(
struct Cyc_Absyn_Tqual*) _temp485.tq; goto _LL489; _LL489: _temp488=( void*)
_temp485.type; goto _LL487; _LL487: _temp486=( struct Cyc_List_List*) _temp485.attributes;
goto _LL484; _LL484: { struct Cyc_Absyn_Structfield _temp496; struct Cyc_List_List*
_temp497; void* _temp499; struct Cyc_Absyn_Tqual* _temp501; struct
_tagged_string* _temp503; struct Cyc_Absyn_Structfield* _temp494=( struct Cyc_Absyn_Structfield*)
f2s->hd; _temp496=* _temp494; _LL504: _temp503=( struct _tagged_string*)
_temp496.name; goto _LL502; _LL502: _temp501=( struct Cyc_Absyn_Tqual*) _temp496.tq;
goto _LL500; _LL500: _temp499=( void*) _temp496.type; goto _LL498; _LL498:
_temp497=( struct Cyc_List_List*) _temp496.attributes; goto _LL495; _LL495: if(
Cyc_String_zstrptrcmp( _temp492, _temp503) != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp505=* v; struct _tagged_string _temp506=* _temp492; struct
_tagged_string _temp507=* _temp503; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp505.last_plus_one - _temp505.curr, _temp505.curr, _temp506.last_plus_one -
_temp506.curr, _temp506.curr, _temp507.last_plus_one - _temp507.curr, _temp507.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp486, _temp497)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp508=* v; struct _tagged_string _temp509=* _temp492; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp508.last_plus_one - _temp508.curr, _temp508.curr, _temp509.last_plus_one -
_temp509.curr, _temp509.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp490,
_temp501)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp510=* v; struct
_tagged_string _temp511=* _temp492; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp510.last_plus_one - _temp510.curr, _temp510.curr, _temp511.last_plus_one -
_temp511.curr, _temp511.curr);}));} Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp488);{ void* subst_t2= Cyc_Tcutil_substitute( inst, _temp499); if( ! Cyc_Tcutil_unify(
_temp488, subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp512=* v;
struct _tagged_string _temp513=* _temp492; struct _tagged_string _temp514= Cyc_Absynpp_typ2string(
_temp488); struct _tagged_string _temp515= Cyc_Absynpp_typ2string( subst_t2);
xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp512.last_plus_one - _temp512.curr, _temp512.curr, _temp513.last_plus_one -
_temp513.curr, _temp513.curr, _temp514.last_plus_one - _temp514.curr, _temp514.curr,
_temp515.last_plus_one - _temp515.curr, _temp515.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp516=* v; struct
_tagged_string _temp517=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp516.last_plus_one
- _temp516.curr, _temp516.curr, _temp517.last_plus_one - _temp517.curr, _temp517.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp518=* v;
struct _tagged_string _temp519=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp518.last_plus_one
- _temp518.curr, _temp518.curr, _temp519.last_plus_one - _temp519.curr, _temp519.curr);}));}
return;}} _LL454:;} goto _LL435;}} _LL435:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp520=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp520->sc=( void*)(( void*) sd->sc); _temp520->name= sd->name; _temp520->tvs=
tvs; _temp520->fields= 0; _temp520->attributes= 0; _temp520;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield _temp523; struct Cyc_List_List*
_temp524; void* _temp526; struct Cyc_Absyn_Tqual* _temp528; struct
_tagged_string* _temp530; struct Cyc_Absyn_Structfield* _temp521=( struct Cyc_Absyn_Structfield*)
fs->hd; _temp523=* _temp521; _LL531: _temp530=( struct _tagged_string*) _temp523.name;
goto _LL529; _LL529: _temp528=( struct Cyc_Absyn_Tqual*) _temp523.tq; goto
_LL527; _LL527: _temp526=( void*) _temp523.type; goto _LL525; _LL525: _temp524=(
struct Cyc_List_List*) _temp523.attributes; goto _LL522; _LL522: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp530)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp532=* _temp530;
xprintf("duplicate field %.*s in struct", _temp532.last_plus_one - _temp532.curr,
_temp532.curr);}));} prev_fields=({ struct Cyc_List_List* _temp533=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp533->hd=( void*) _temp530;
_temp533->tl= prev_fields; _temp533;}); Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp526); for( 0; _temp524 != 0; _temp524= _temp524->tl){ void* _temp534=( void*)
_temp524->hd; int _temp542; _LL536: if( _temp534 == Cyc_Absyn_Packed_att){ goto
_LL537;} else{ goto _LL538;} _LL538: if(( unsigned int) _temp534 > 15u?(( struct
_enum_struct*) _temp534)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL543: _temp542=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp534)->f1; goto _LL539;} else{
goto _LL540;} _LL540: goto _LL541; _LL537: continue; _LL539: continue; _LL541:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp544= Cyc_Absyn_attribute2string((
void*) _temp524->hd); struct _tagged_string _temp545=* v; struct _tagged_string
_temp546=* _temp530; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp544.last_plus_one - _temp544.curr, _temp544.curr, _temp545.last_plus_one -
_temp545.curr, _temp545.curr, _temp546.last_plus_one - _temp546.curr, _temp546.curr);}));
goto _LL535; _LL535:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp547=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp547->f1=({ struct Cyc_Tcenv_StructRes_struct* _temp548=( struct
Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp548->tag= Cyc_Tcenv_StructRes_tag; _temp548->f1= sd;( void*) _temp548;});
_temp547->f2= 1; _temp547;}));* sdp= sd;}}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp549=( char*)"anonymous enums are not allowed at top level";
struct _tagged_string _temp550; _temp550.curr= _temp549; _temp550.base= _temp549;
_temp550.last_plus_one= _temp549 + 45; _temp550;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ed->name)->v)).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void*
_temp551=( void*) c->v; void* _temp559; _LL553: if( _temp551 == Cyc_Absyn_No_constr){
goto _LL554;} else{ goto _LL555;} _LL555: if(( unsigned int) _temp551 > 1u?((
struct _enum_struct*) _temp551)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL560:
_temp559=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp551)->f1; if(
_temp559 == Cyc_Absyn_MemKind){ goto _LL556;} else{ goto _LL557;}} else{ goto
_LL557;} _LL557: goto _LL558; _LL554:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp561=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp561->tag= Cyc_Absyn_Eq_constr_tag; _temp561->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp561;})); goto _LL552; _LL556: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp562=* v; struct _tagged_string _temp563=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("enum %.*s attempts to abstract type variable %.*s of kind M",
_temp562.last_plus_one - _temp562.curr, _temp562.curr, _temp563.last_plus_one -
_temp563.curr, _temp563.curr);})); goto _LL552; _LL558: goto _LL552; _LL552:;}}{
void* _temp564=(*(( struct _tuple0*)( ed->name)->v)).f1; struct Cyc_List_List*
_temp572; struct Cyc_List_List* _temp574; _LL566: if(( unsigned int) _temp564 >
1u?(( struct _enum_struct*) _temp564)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL573:
_temp572=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp564)->f1;
if( _temp572 == 0){ goto _LL567;} else{ goto _LL568;}} else{ goto _LL568;}
_LL568: if(( unsigned int) _temp564 > 1u?(( struct _enum_struct*) _temp564)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL575: _temp574=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp564)->f1; if( _temp574 == 0){ goto _LL569;} else{
goto _LL570;}} else{ goto _LL570;} _LL570: goto _LL571; _LL567: goto _LL565;
_LL569: goto _LL565; _LL571: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp576=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp577; _temp577.curr= _temp576; _temp577.base= _temp576;
_temp577.last_plus_one= _temp576 + 43; _temp577;})); return; _LL565:;}(*((
struct _tuple0*)( ed->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp578=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp578->tag= Cyc_Absyn_Abs_n_tag; _temp578->f1= te->ns;( void*) _temp578;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp579=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->enumdecls, v); struct Cyc_Core_Opt _temp585; struct
Cyc_Absyn_Enumdecl** _temp586; _LL581: if( _temp579 == 0){ goto _LL582;} else{
goto _LL583;} _LL583: if( _temp579 == 0){ goto _LL580;} else{ _temp585=*
_temp579; _LL587: _temp586=( struct Cyc_Absyn_Enumdecl**) _temp585.v; goto
_LL584;} _LL582: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,( struct Cyc_Absyn_Enumdecl**)({ struct _tuple9* _temp588=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp588->f1= ed; _temp588;}));
if( ed->fields == 0){ return;} goto _LL580; _LL584: { struct Cyc_Absyn_Enumdecl*
ed2=* _temp586; struct Cyc_List_List* tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp589=* v; xprintf("redeclaration of enum %.*s has a different number of type parameters",
_temp589.last_plus_one - _temp589.curr, _temp589.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp590=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp590->tag= Cyc_Absyn_Forward_constr_tag;
_temp590->f1= c2;( void*) _temp590;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp591=* v; struct
_tagged_string _temp592= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp593=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp594= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of enum %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp591.last_plus_one - _temp591.curr, _temp591.curr, _temp592.last_plus_one -
_temp592.curr, _temp592.curr, _temp593.last_plus_one - _temp593.curr, _temp593.curr,
_temp594.last_plus_one - _temp594.curr, _temp594.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp595=( char*)"enum";
struct _tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 5; _temp596;}), v));{ struct _tuple8 _temp598=({
struct _tuple8 _temp597; _temp597.f1= ed->fields; _temp597.f2= ed2->fields;
_temp597;}); struct Cyc_Core_Opt* _temp608; struct Cyc_Core_Opt* _temp610;
struct Cyc_Core_Opt* _temp612; struct Cyc_Core_Opt* _temp614; struct Cyc_Core_Opt*
_temp616; struct Cyc_Core_Opt* _temp618; struct Cyc_Core_Opt* _temp620; struct
Cyc_Core_Opt* _temp622; _LL600: _LL611: _temp610= _temp598.f1; if( _temp610 == 0){
goto _LL609;} else{ goto _LL602;} _LL609: _temp608= _temp598.f2; if( _temp608 ==
0){ goto _LL601;} else{ goto _LL602;} _LL602: _LL615: _temp614= _temp598.f1;
goto _LL613; _LL613: _temp612= _temp598.f2; if( _temp612 == 0){ goto _LL603;}
else{ goto _LL604;} _LL604: _LL619: _temp618= _temp598.f1; if( _temp618 == 0){
goto _LL617;} else{ goto _LL606;} _LL617: _temp616= _temp598.f2; goto _LL605;
_LL606: _LL623: _temp622= _temp598.f1; goto _LL621; _LL621: _temp620= _temp598.f2;
goto _LL607; _LL601: return; _LL603:* _temp586= ed; goto _LL599; _LL605: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL607: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp624=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp624->hd=( void*)({ struct _tuple4* _temp625=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp625->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp625->f2=({ struct Cyc_Absyn_VarType_struct* _temp626=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp626->tag= Cyc_Absyn_VarType_tag;
_temp626->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp626;}); _temp625;});
_temp624->tl= inst; _temp624;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ed->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ed2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){(*(( struct Cyc_Absyn_Enumfield*)
f1s->hd)->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp627=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp627->tag= Cyc_Absyn_Abs_n_tag;
_temp627->f1= te->ns;( void*) _temp627;});{ struct Cyc_List_List* alltvs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs,(( struct Cyc_Absyn_Enumfield*) f1s->hd)->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Enumfield*) f1s->hd)->loc, alltvs);{ struct Cyc_List_List* typs=((
struct Cyc_Absyn_Enumfield*) f1s->hd)->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Enumfield*)
f1s->hd)->loc, te, alltvs, Cyc_Absyn_MemKind, t);}} Cyc_Tc_field_redecl_okay((
struct Cyc_Absyn_Enumfield*) f1s->hd,( struct Cyc_Absyn_Enumfield*) f2s->hd,
inst, te,( struct _tagged_string)({ char* _temp628=( char*)"enum"; struct
_tagged_string _temp629; _temp629.curr= _temp628; _temp629.base= _temp628;
_temp629.last_plus_one= _temp628 + 5; _temp629;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp630=* v; struct _tagged_string _temp631=*(*((
struct Cyc_Absyn_Enumfield*) f1s->hd)->name).f2; xprintf("redeclaration of enum %.*s has extra field %.*s",
_temp630.last_plus_one - _temp630.curr, _temp630.curr, _temp631.last_plus_one -
_temp631.curr, _temp631.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp632=* v; struct _tagged_string _temp633=*(*(( struct
Cyc_Absyn_Enumfield*) f2s->hd)->name).f2; xprintf("redeclaration of enum %.*s is missing field %.*s",
_temp632.last_plus_one - _temp632.curr, _temp632.curr, _temp633.last_plus_one -
_temp633.curr, _temp633.curr);}));} return;}} _LL599:;} goto _LL580;}} _LL580:;}{
struct Cyc_Absyn_Enumdecl** edp=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v);* edp=({
struct Cyc_Absyn_Enumdecl* _temp634=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp634->sc=( void*)(( void*) ed->sc);
_temp634->name= ed->name; _temp634->tvs= tvs; _temp634->fields= 0; _temp634;});{
struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(
ed->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*) fs->hd; if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){
Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp635=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp635.last_plus_one - _temp635.curr,
_temp635.curr);}));} prev_fields=({ struct Cyc_List_List* _temp636=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp636->hd=( void*)(* f->name).f2;
_temp636->tl= prev_fields; _temp636;}); if( f->tag != 0){ Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp637= f->tag; if( _temp637 ==
0){ _throw( Null_Exception);} _temp637;}));}{ struct Cyc_List_List* tvs= f->tvs;
for( 0; tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)
tvs->hd; struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp638=( void*) c->v; void*
_temp646; _LL640: if( _temp638 == Cyc_Absyn_No_constr){ goto _LL641;} else{ goto
_LL642;} _LL642: if(( unsigned int) _temp638 > 1u?(( struct _enum_struct*)
_temp638)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL647: _temp646=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp638)->f1; if( _temp646 == Cyc_Absyn_MemKind){
goto _LL643;} else{ goto _LL644;}} else{ goto _LL644;} _LL644: goto _LL645;
_LL641:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp648=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp648->tag= Cyc_Absyn_Eq_constr_tag; _temp648->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp648;})); goto _LL639; _LL643: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp649=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp649.last_plus_one - _temp649.curr, _temp649.curr);})); goto _LL639; _LL645:
goto _LL639; _LL639:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs);
Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List* typs= f->typs;
for( 0; typs != 0; typs= typs->tl){ void* t=(*(( struct _tuple5*) typs->hd)).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs, Cyc_Absyn_MemKind, t);}}(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp650=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp650->tag= Cyc_Absyn_Abs_n_tag;
_temp650->f1= te->ns;( void*) _temp650;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple6* _temp651=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp651->f1=({ struct Cyc_Tcenv_EnumRes_struct*
_temp652=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp652->tag= Cyc_Tcenv_EnumRes_tag; _temp652->f1= ed; _temp652->f2= f;( void*)
_temp652;}); _temp651->f2= 1; _temp651;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl**
data)) Cyc_Dict_insert)( ge->enumdecls, v,( struct Cyc_Absyn_Enumdecl**)({
struct _tuple9* _temp653=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp653->f1= ed; _temp653;}));* edp= ed;}}}} static void Cyc_Tc_tcXenumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Xenumdecl* xd){ struct _tagged_string* v=(* xd->name).f2;
struct Cyc_Core_Opt* xd2o;{ struct _handler_cons _temp654; _push_handler(&
_temp654);{ struct _xenum_struct* _temp655=( struct _xenum_struct*) setjmp(
_temp654.handler); if( ! _temp655){ xd2o= Cyc_Tcenv_lookup_xenumdecl( te, loc,
xd->name);; _pop_handler();} else{ struct _xenum_struct* _temp657= _temp655;
_LL659: if( _temp657->tag == Cyc_Dict_Absent_tag){ goto _LL660;} else{ goto
_LL661;} _LL661: goto _LL662; _LL660: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp663= Cyc_Absynpp_qvar2string( xd->name); xprintf("qualified xenum declaration %.*s is not an existing xenum",
_temp663.last_plus_one - _temp663.curr, _temp663.curr);})); return; _LL662:(
void) _throw( _temp657); _LL658:;}}}{ void* _temp664=(* xd->name).f1; struct Cyc_List_List*
_temp674; struct Cyc_List_List* _temp676; struct Cyc_List_List* _temp678; _LL666:
if( _temp664 == Cyc_Absyn_Loc_n){ goto _LL667;} else{ goto _LL668;} _LL668: if((
unsigned int) _temp664 > 1u?(( struct _enum_struct*) _temp664)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL675: _temp674=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp664)->f1; if( _temp674 == 0){ goto _LL669;} else{ goto _LL670;}} else{ goto
_LL670;} _LL670: if(( unsigned int) _temp664 > 1u?(( struct _enum_struct*)
_temp664)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL677: _temp676=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp664)->f1; goto _LL671;} else{ goto _LL672;}
_LL672: if(( unsigned int) _temp664 > 1u?(( struct _enum_struct*) _temp664)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL679: _temp678=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp664)->f1; goto _LL673;} else{ goto _LL665;} _LL667:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp680=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp680=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp681=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 19; _temp682;})};( struct _xenum_struct*)
_temp680;})); _LL669:(* xd->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp683=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp683->tag= Cyc_Absyn_Abs_n_tag; _temp683->f1= te->ns;( void*) _temp683;});
goto _LL665; _LL671:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp684=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp684=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp685=( char*)"tcXenumdecl: Rel_n non-null";
struct _tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 28; _temp686;})};( struct _xenum_struct*)
_temp684;})); _LL673:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp687=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp687=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp688=( char*)"tcXenumdecl: Abs_n"; struct
_tagged_string _temp689; _temp689.curr= _temp688; _temp689.base= _temp688;
_temp689.last_plus_one= _temp688 + 19; _temp689;})};( struct _xenum_struct*)
_temp687;})); _LL665:;} if( xd2o == 0){ struct Cyc_Absyn_Xenumdecl** x=( struct
Cyc_Absyn_Xenumdecl**)({ struct _tuple10* _temp690=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp690->f1=({ struct Cyc_Absyn_Xenumdecl* _temp691=(
struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp691->sc=( void*)(( void*) xd->sc); _temp691->name= xd->name; _temp691->fields=
0; _temp691;}); _temp690;}); xd2o=({ struct Cyc_Core_Opt* _temp692=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp692->v=( void*) x; _temp692;});
ge->xenumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Xenumdecl** data)) Cyc_Dict_insert)( ge->xenumdecls,
v, x);} else{ struct Cyc_Absyn_Xenumdecl* x=*(( struct Cyc_Absyn_Xenumdecl**)
xd2o->v);( void*)( x->sc=( void*)(( void*(*)( void* s1, struct Cyc_Core_Opt*
fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp693=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp693->v=( void*) 0;
_temp693;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp694=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp694->v=( void*) 0;
_temp694;}), loc,( struct _tagged_string)({ char* _temp695=( char*)"xenum";
struct _tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 6; _temp696;}), v));}{ struct Cyc_Absyn_Xenumdecl*
xd_result=*(( struct Cyc_Absyn_Xenumdecl**) xd2o->v); struct Cyc_List_List* fs=
xd->fields; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct
Cyc_Absyn_Enumfield*) fs->hd;{ void* _temp697=(* f->name).f1; struct Cyc_List_List*
_temp707; struct Cyc_List_List* _temp709; struct Cyc_List_List* _temp711; _LL699:
if(( unsigned int) _temp697 > 1u?(( struct _enum_struct*) _temp697)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL708: _temp707=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp697)->f1; if( _temp707 == 0){ goto _LL700;} else{ goto _LL701;}} else{ goto
_LL701;} _LL701: if(( unsigned int) _temp697 > 1u?(( struct _enum_struct*)
_temp697)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL710: _temp709=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp697)->f1; goto _LL702;} else{ goto _LL703;}
_LL703: if(( unsigned int) _temp697 > 1u?(( struct _enum_struct*) _temp697)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL712: _temp711=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp697)->f1; goto _LL704;} else{ goto _LL705;} _LL705:
if( _temp697 == Cyc_Absyn_Loc_n){ goto _LL706;} else{ goto _LL698;} _LL700:(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp713=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp713->tag= Cyc_Absyn_Abs_n_tag;
_temp713->f1= te->ns;( void*) _temp713;}); goto _LL698; _LL702: goto _LL704;
_LL704: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp714=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 40; _temp715;})); goto _LL698; _LL706:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp716=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp716=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp717=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp718; _temp718.curr= _temp717; _temp718.base= _temp717;
_temp718.last_plus_one= _temp717 + 19; _temp718;})};( struct _xenum_struct*)
_temp716;})); goto _LL698; _LL698:;}{ struct Cyc_List_List* tvs= f->tvs; for( 0;
tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*) tvs->hd;
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp719=( void*) c->v; void*
_temp727; _LL721: if( _temp719 == Cyc_Absyn_No_constr){ goto _LL722;} else{ goto
_LL723;} _LL723: if(( unsigned int) _temp719 > 1u?(( struct _enum_struct*)
_temp719)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL728: _temp727=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp719)->f1; if( _temp727 == Cyc_Absyn_MemKind){
goto _LL724;} else{ goto _LL725;}} else{ goto _LL725;} _LL725: goto _LL726;
_LL722:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp729=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp729->tag= Cyc_Absyn_Eq_constr_tag; _temp729->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp729;})); goto _LL720; _LL724: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp730=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp730.last_plus_one - _temp730.curr, _temp730.curr);})); goto _LL720; _LL726:
goto _LL720; _LL720:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type( f->loc, te,
tvs, Cyc_Absyn_MemKind, t);}}{ int dup= 0;{ struct Cyc_List_List* prev_fs=
xd_result->fields; for( 0; prev_fs != 0; prev_fs= prev_fs->tl){ struct Cyc_Absyn_Enumfield*
prev=( struct Cyc_Absyn_Enumfield*) prev_fs->hd; if( Cyc_Absyn_qvar_cmp( f->name,
prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp731=( char*)"xenum"; struct _tagged_string _temp732;
_temp732.curr= _temp731; _temp732.base= _temp731; _temp732.last_plus_one=
_temp731 + 6; _temp732;}), v); dup= 1; break;}}} if( dup){ continue;} xd_result->fields=({
struct Cyc_List_List* _temp733=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp733->hd=( void*) f; _temp733->tl= xd_result->fields;
_temp733;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List* ns_f;{ void*
_temp734=(* f->name).f1; struct Cyc_List_List* _temp742; struct Cyc_List_List*
_temp744; _LL736: if(( unsigned int) _temp734 > 1u?(( struct _enum_struct*)
_temp734)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL743: _temp742=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp734)->f1; goto _LL737;} else{ goto _LL738;}
_LL738: if( _temp734 == Cyc_Absyn_Loc_n){ goto _LL739;} else{ goto _LL740;}
_LL740: if(( unsigned int) _temp734 > 1u?(( struct _enum_struct*) _temp734)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL745: _temp744=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp734)->f1; goto _LL741;} else{ goto _LL735;} _LL737:
ns_f= _temp742; goto _LL735; _LL739: goto _LL741; _LL741:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp746=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp746=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp747=( char*)"tcXenumdecl: Rel_n or Loc_n";
struct _tagged_string _temp748; _temp748.curr= _temp747; _temp748.base= _temp747;
_temp748.last_plus_one= _temp747 + 28; _temp748;})};( struct _xenum_struct*)
_temp746;})); goto _LL735; _LL735:;}{ struct _handler_cons _temp749;
_push_handler(& _temp749);{ struct _xenum_struct* _temp750=( struct
_xenum_struct*) setjmp( _temp749.handler); if( ! _temp750){ ge_f=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
ns_f);; _pop_handler();} else{ struct _xenum_struct* _temp752= _temp750; _LL754:
if( _temp752->tag == Cyc_Dict_Absent_tag){ goto _LL755;} else{ goto _LL756;}
_LL756: goto _LL757; _LL755: Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp758= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xenum field %.*s",
_temp758.last_plus_one - _temp758.curr, _temp758.curr);})); return; _LL757:(
void) _throw( _temp752); _LL753:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp759=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp759->f1=({ struct Cyc_Tcenv_XenumRes_struct*
_temp760=( struct Cyc_Tcenv_XenumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XenumRes_struct));
_temp760->tag= Cyc_Tcenv_XenumRes_tag; _temp760->f1= xd_result; _temp760->f2= f;(
void*) _temp760;}); _temp759->f2= 1; _temp759;}));}}}}}} static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment* loc, void* sc){ void* _temp761= sc; _LL763: if(
_temp761 == Cyc_Absyn_Static){ goto _LL764;} else{ goto _LL765;} _LL765: if(
_temp761 == Cyc_Absyn_Abstract){ goto _LL766;} else{ goto _LL767;} _LL767: if(
_temp761 == Cyc_Absyn_Public){ goto _LL768;} else{ goto _LL769;} _LL769: if(
_temp761 == Cyc_Absyn_Extern){ goto _LL770;} else{ goto _LL771;} _LL771: if(
_temp761 == Cyc_Absyn_ExternC){ goto _LL772;} else{ goto _LL762;} _LL764: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp773=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp774; _temp774.curr= _temp773; _temp774.base= _temp773;
_temp774.last_plus_one= _temp773 + 44; _temp774;})); return 0; _LL766: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp775=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp776; _temp776.curr= _temp775; _temp776.base= _temp775;
_temp776.last_plus_one= _temp775 + 46; _temp776;})); return 0; _LL768: return 1;
_LL770: return 1; _LL772: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp777=( char*)"nested extern \"C\" declaration"; struct _tagged_string
_temp778; _temp778.curr= _temp777; _temp778.base= _temp777; _temp778.last_plus_one=
_temp777 + 30; _temp778;})); return 1; _LL762:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC){ struct
Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_List_List* ds=
ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; struct Cyc_Position_Segment* loc= d->loc; void* _temp779=( void*) d->r;
int _temp803; struct Cyc_Absyn_Exp* _temp805; struct Cyc_Core_Opt* _temp807;
struct Cyc_Core_Opt* _temp809; struct Cyc_Absyn_Pat* _temp811; struct Cyc_Absyn_Vardecl*
_temp813; struct Cyc_Absyn_Fndecl* _temp815; struct Cyc_Absyn_Typedefdecl*
_temp817; struct Cyc_Absyn_Structdecl* _temp819; struct Cyc_Absyn_Enumdecl*
_temp821; struct Cyc_Absyn_Xenumdecl* _temp823; struct Cyc_List_List* _temp825;
struct _tagged_string* _temp827; struct Cyc_List_List* _temp829; struct _tuple0*
_temp831; struct _tuple0 _temp833; struct _tagged_string* _temp834; void*
_temp836; struct Cyc_List_List* _temp838; _LL781: if(( unsigned int) _temp779 >
1u?(( struct _enum_struct*) _temp779)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL812:
_temp811=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp779)->f1;
goto _LL810; _LL810: _temp809=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp779)->f2; goto _LL808; _LL808: _temp807=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp779)->f3; goto _LL806; _LL806: _temp805=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp779)->f4; goto _LL804;
_LL804: _temp803=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp779)->f5; goto
_LL782;} else{ goto _LL783;} _LL783: if( _temp779 == Cyc_Absyn_Union_d){ goto
_LL784;} else{ goto _LL785;} _LL785: if(( unsigned int) _temp779 > 1u?(( struct
_enum_struct*) _temp779)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL814: _temp813=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp779)->f1; goto
_LL786;} else{ goto _LL787;} _LL787: if(( unsigned int) _temp779 > 1u?(( struct
_enum_struct*) _temp779)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL816: _temp815=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp779)->f1; goto
_LL788;} else{ goto _LL789;} _LL789: if(( unsigned int) _temp779 > 1u?(( struct
_enum_struct*) _temp779)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL818: _temp817=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp779)->f1;
goto _LL790;} else{ goto _LL791;} _LL791: if(( unsigned int) _temp779 > 1u?((
struct _enum_struct*) _temp779)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL820:
_temp819=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp779)->f1; goto _LL792;} else{ goto _LL793;} _LL793: if(( unsigned int)
_temp779 > 1u?(( struct _enum_struct*) _temp779)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL822: _temp821=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp779)->f1; goto _LL794;} else{ goto _LL795;} _LL795: if(( unsigned int)
_temp779 > 1u?(( struct _enum_struct*) _temp779)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL824: _temp823=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp779)->f1; goto _LL796;} else{ goto _LL797;} _LL797: if(( unsigned int)
_temp779 > 1u?(( struct _enum_struct*) _temp779)->tag == Cyc_Absyn_Namespace_d_tag:
0){ _LL828: _temp827=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp779)->f1; goto _LL826; _LL826: _temp825=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp779)->f2; goto _LL798;} else{ goto _LL799;}
_LL799: if(( unsigned int) _temp779 > 1u?(( struct _enum_struct*) _temp779)->tag
== Cyc_Absyn_Using_d_tag: 0){ _LL832: _temp831=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp779)->f1; _temp833=* _temp831; _LL837: _temp836= _temp833.f1; goto _LL835;
_LL835: _temp834= _temp833.f2; goto _LL830; _LL830: _temp829=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp779)->f2; goto _LL800;} else{ goto _LL801;}
_LL801: if(( unsigned int) _temp779 > 1u?(( struct _enum_struct*) _temp779)->tag
== Cyc_Absyn_ExternC_d_tag: 0){ _LL839: _temp838=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp779)->f1; goto _LL802;} else{ goto
_LL780;} _LL782: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp840=(
char*)"top level let-declarations are not implemented"; struct _tagged_string
_temp841; _temp841.curr= _temp840; _temp841.base= _temp840; _temp841.last_plus_one=
_temp840 + 47; _temp841;})); goto _LL780; _LL784: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp842=( char*)"union is not implemented"; struct
_tagged_string _temp843; _temp843.curr= _temp842; _temp843.base= _temp842;
_temp843.last_plus_one= _temp842 + 25; _temp843;})); goto _LL780; _LL786: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp813->sc): 0){( void*)(
_temp813->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcVardecl( te, ge, loc,
_temp813); goto _LL780; _LL788: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp815->sc): 0){( void*)( _temp815->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcFndecl( te, ge, loc, _temp815); goto _LL780; _LL790: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp817); goto _LL780; _LL792: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp819->sc): 0){( void*)( _temp819->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp819); goto _LL780; _LL794: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp821->sc): 0){( void*)( _temp821->sc=(
void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp821); goto
_LL780; _LL796: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp823->sc):
0){( void*)( _temp823->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcXenumdecl( te,
ge, loc, _temp823); goto _LL780; _LL798: { struct Cyc_List_List* ns= te->ns;
struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp844=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp844->hd=(
void*) _temp827; _temp844->tl= 0; _temp844;})); if( !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces, _temp827)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
ge->namespaces, _temp827); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
ns2, Cyc_Tcenv_empty_genv());} te->ns= ns2; Cyc_Tc_tc_decls( te, _temp825,
in_externC); te->ns= ns; goto _LL780;} _LL800: { struct _tagged_string* first;
struct Cyc_List_List* rest;{ void* _temp845= _temp836; struct Cyc_List_List*
_temp857; struct Cyc_List_List* _temp859; struct Cyc_List_List* _temp861; struct
Cyc_List_List _temp863; struct Cyc_List_List* _temp864; struct _tagged_string*
_temp866; struct Cyc_List_List* _temp868; struct Cyc_List_List _temp870; struct
Cyc_List_List* _temp871; struct _tagged_string* _temp873; _LL847: if( _temp845
== Cyc_Absyn_Loc_n){ goto _LL848;} else{ goto _LL849;} _LL849: if(( unsigned int)
_temp845 > 1u?(( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL858: _temp857=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp845)->f1; if( _temp857 == 0){ goto _LL850;} else{ goto _LL851;}} else{ goto
_LL851;} _LL851: if(( unsigned int) _temp845 > 1u?(( struct _enum_struct*)
_temp845)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL860: _temp859=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp845)->f1; if( _temp859 == 0){ goto _LL852;}
else{ goto _LL853;}} else{ goto _LL853;} _LL853: if(( unsigned int) _temp845 > 1u?((
struct _enum_struct*) _temp845)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL862:
_temp861=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp845)->f1;
if( _temp861 == 0){ goto _LL855;} else{ _temp863=* _temp861; _LL867: _temp866=(
struct _tagged_string*) _temp863.hd; goto _LL865; _LL865: _temp864=( struct Cyc_List_List*)
_temp863.tl; goto _LL854;}} else{ goto _LL855;} _LL855: if(( unsigned int)
_temp845 > 1u?(( struct _enum_struct*) _temp845)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL869: _temp868=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp845)->f1; if( _temp868 == 0){ goto _LL846;} else{ _temp870=* _temp868;
_LL874: _temp873=( struct _tagged_string*) _temp870.hd; goto _LL872; _LL872:
_temp871=( struct Cyc_List_List*) _temp870.tl; goto _LL856;}} else{ goto _LL846;}
_LL848: goto _LL850; _LL850: goto _LL852; _LL852: first= _temp834; rest= 0; goto
_LL846; _LL854: _temp873= _temp866; _temp871= _temp864; goto _LL856; _LL856:
first= _temp873; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp871,({ struct Cyc_List_List*
_temp875=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp875->hd=( void*) _temp834; _temp875->tl= 0; _temp875;})); goto _LL846;
_LL846:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace( te, loc,
first, rest); ge->availables=({ struct Cyc_List_List* _temp876=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp876->hd=( void*) ns2; _temp876->tl=
ge->availables; _temp876;}); Cyc_Tc_tc_decls( te, _temp829, in_externC); ge->availables=(
ge->availables)->tl; goto _LL780;}} _LL802: Cyc_Tc_tc_decls( te, _temp838, 1);
goto _LL780; _LL780:;}} static char _temp879[ 4u]="Cyc"; static struct
_tagged_string Cyc_Tc_cyc_string=( struct _tagged_string){ _temp879, _temp879,
_temp879 + 4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string;
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct Cyc_List_List*
ds){ if( add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp880=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp880->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp881=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp881->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp882=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp882->tag= Cyc_Absyn_Namespace_d_tag; _temp882->f1= Cyc_Tc_cyc_ns; _temp882->f2=
ds;( void*) _temp882;}); _temp881->loc= 0; _temp881;}); _temp880->tl= 0;
_temp880;});} Cyc_Tc_tc_decls( te, ds, 0);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Decl* d){ void* _temp883=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp893; struct Cyc_List_List* _temp895; struct Cyc_List_List**
_temp897; struct _tuple0* _temp898; struct Cyc_List_List* _temp900; struct Cyc_List_List**
_temp902; struct _tagged_string* _temp903; _LL885: if(( unsigned int) _temp883 >
1u?(( struct _enum_struct*) _temp883)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL894:
_temp893=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp883)->f1;
goto _LL886;} else{ goto _LL887;} _LL887: if(( unsigned int) _temp883 > 1u?((
struct _enum_struct*) _temp883)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL899:
_temp898=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp883)->f1;
goto _LL896; _LL896: _temp895=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp883)->f2; _temp897=&(( struct Cyc_Absyn_Using_d_struct*) _temp883)->f2;
goto _LL888;} else{ goto _LL889;} _LL889: if(( unsigned int) _temp883 > 1u?((
struct _enum_struct*) _temp883)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL904:
_temp903=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp883)->f1; goto _LL901; _LL901: _temp900=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp883)->f2; _temp902=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp883)->f2; goto _LL890;} else{ goto _LL891;} _LL891: goto _LL892; _LL886:
if(( void*) _temp893->sc != Cyc_Absyn_Extern){ return 1;}{ struct _tuple0
_temp907; struct _tagged_string* _temp908; void* _temp910; struct _tuple0*
_temp905= _temp893->name; _temp907=* _temp905; _LL911: _temp910= _temp907.f1;
goto _LL909; _LL909: _temp908= _temp907.f2; goto _LL906; _LL906: { struct Cyc_List_List*
ns;{ void* _temp912= _temp910; struct Cyc_List_List* _temp920; struct Cyc_List_List*
_temp922; _LL914: if( _temp912 == Cyc_Absyn_Loc_n){ goto _LL915;} else{ goto
_LL916;} _LL916: if(( unsigned int) _temp912 > 1u?(( struct _enum_struct*)
_temp912)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL921: _temp920=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp912)->f1; goto _LL917;} else{ goto _LL918;}
_LL918: if(( unsigned int) _temp912 > 1u?(( struct _enum_struct*) _temp912)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL923: _temp922=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp912)->f1; goto _LL919;} else{ goto _LL913;} _LL915:
ns= 0; goto _LL913; _LL917: ns= _temp920; goto _LL913; _LL919: ns= _temp922;
goto _LL913; _LL913:;}{ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
ns); return(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, _temp908)).f2;}}} _LL888:
_temp902= _temp897; goto _LL890; _LL890:* _temp902= Cyc_Tc_treeshake( te,*
_temp902); return 1; _LL892: return 1; _LL884:;} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Decl*), struct Cyc_Tcenv_Tenv*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, te, ds);}