#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ void*
f1; int f2; } ; struct _tuple7{ struct Cyc_Absyn_Structdecl* f1; } ; struct
_tuple8{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; struct _tuple9{
struct Cyc_Absyn_Uniondecl* f1; } ; struct _tuple10{ struct Cyc_Absyn_Enumdecl*
f1; } ; struct _tuple11{ struct Cyc_Absyn_Xenumdecl* f1; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
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
void* t); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcexp_is_const_exp( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
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
_temp236->f1= te->ns;( void*) _temp236;}); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tc_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts); if(
sc == Cyc_Absyn_Extern? 1: sc == Cyc_Absyn_ExternC){ if( vd->initializer != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp237=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 47; _temp238;}));}} else{ if( ! Cyc_Tc_is_function_type(
t)){ for( 0; atts != 0; atts= atts->tl){ void* _temp239=( void*) atts->hd; int
_temp259; struct _tagged_string _temp261; _LL241: if(( unsigned int) _temp239 >
15u?(( struct _enum_struct*) _temp239)->tag == Cyc_Absyn_Aligned_att_tag: 0){
_LL260: _temp259=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp239)->f1;
goto _LL242;} else{ goto _LL243;} _LL243: if(( unsigned int) _temp239 > 15u?((
struct _enum_struct*) _temp239)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL262:
_temp261=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp239)->f1; goto _LL244;} else{ goto _LL245;} _LL245: if( _temp239 == Cyc_Absyn_Nocommon_att){
goto _LL246;} else{ goto _LL247;} _LL247: if( _temp239 == Cyc_Absyn_Shared_att){
goto _LL248;} else{ goto _LL249;} _LL249: if( _temp239 == Cyc_Absyn_Unused_att){
goto _LL250;} else{ goto _LL251;} _LL251: if( _temp239 == Cyc_Absyn_Weak_att){
goto _LL252;} else{ goto _LL253;} _LL253: if( _temp239 == Cyc_Absyn_Dllimport_att){
goto _LL254;} else{ goto _LL255;} _LL255: if( _temp239 == Cyc_Absyn_Dllexport_att){
goto _LL256;} else{ goto _LL257;} _LL257: goto _LL258; _LL242: continue; _LL244:
continue; _LL246: continue; _LL248: continue; _LL250: continue; _LL252:
continue; _LL254: continue; _LL256: continue; _LL258: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp263= Cyc_Absyn_attribute2string(( void*) atts->hd);
struct _tagged_string _temp264= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp263.last_plus_one - _temp263.curr, _temp263.curr, _temp264.last_plus_one -
_temp264.curr, _temp264.curr);})); goto _LL240; _LL240:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp265= vd->initializer; if( _temp265 == 0){ _throw( Null_Exception);}
_temp265;});{ void* _temp266= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp272; struct Cyc_Absyn_Tqual* _temp274; void* _temp276; _LL268: if((
unsigned int) _temp266 > 4u?(( struct _enum_struct*) _temp266)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL277: _temp276=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp266)->f1;
goto _LL275; _LL275: _temp274=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp266)->f2; goto _LL273; _LL273: _temp272=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp266)->f3; goto _LL269;} else{ goto _LL270;}
_LL270: goto _LL271; _LL269: if( Cyc_Evexp_eval_const_uint_exp( _temp272) ==(
unsigned int) 0){ void* _temp278=( void*) e->r; void* _temp288; struct
_tagged_string _temp290; int _temp292; struct Cyc_List_List* _temp294; struct
Cyc_Core_Opt* _temp296; struct Cyc_List_List* _temp298; int _temp300; _LL280:
if((( struct _enum_struct*) _temp278)->tag == Cyc_Absyn_Const_e_tag){ _LL289:
_temp288=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp278)->f1; if((
unsigned int) _temp288 > 1u?(( struct _enum_struct*) _temp288)->tag == Cyc_Absyn_String_c_tag:
0){ _LL293: _temp292=( int)(( struct Cyc_Absyn_String_c_struct*) _temp288)->f1;
goto _LL291; _LL291: _temp290=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp288)->f2; goto _LL281;} else{ goto _LL282;}} else{ goto _LL282;} _LL282:
if((( struct _enum_struct*) _temp278)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL297: _temp296=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp278)->f1; goto _LL295; _LL295: _temp294=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp278)->f2; goto _LL283;} else{ goto
_LL284;} _LL284: if((( struct _enum_struct*) _temp278)->tag == Cyc_Absyn_Array_e_tag){
_LL301: _temp300=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp278)->f1; goto
_LL299; _LL299: _temp298=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp278)->f2; goto _LL285;} else{ goto _LL286;} _LL286: goto _LL287; _LL281: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp302=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp302->tag= Cyc_Absyn_ArrayType_tag; _temp302->f1=( void*) _temp276; _temp302->f2=
_temp274; _temp302->f3= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp303=
_temp290;( unsigned int)( _temp303.last_plus_one - _temp303.curr);}), 0);( void*)
_temp302;})); goto _LL279; _LL283: _temp298= _temp294; goto _LL285; _LL285: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp304=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp304->tag= Cyc_Absyn_ArrayType_tag; _temp304->f1=( void*) _temp276; _temp304->f2=
_temp274; _temp304->f3= Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp298), 0);( void*) _temp304;})); goto _LL279; _LL287:
goto _LL279; _LL279:;} goto _LL267; _LL271: goto _LL267; _LL267:;}}{ void* t2=
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_assign(
te, e, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp305= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp306= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp307= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp305.last_plus_one - _temp305.curr, _temp305.curr, _temp306.last_plus_one -
_temp306.curr, _temp306.curr, _temp307.last_plus_one - _temp307.curr, _temp307.curr);}));}
if( ! Cyc_Tcexp_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp308=( char*)"initializer is not a constant expression";
struct _tagged_string _temp309; _temp309.curr= _temp308; _temp309.base= _temp308;
_temp309.last_plus_one= _temp308 + 41; _temp309;}));}}}} else{ for( 0; atts != 0;
atts= atts->tl){ void* _temp310=( void*) atts->hd; int _temp328; int _temp330;
_LL312: if(( unsigned int) _temp310 > 15u?(( struct _enum_struct*) _temp310)->tag
== Cyc_Absyn_Regparm_att_tag: 0){ _LL329: _temp328=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp310)->f1; goto _LL313;} else{ goto _LL314;} _LL314: if( _temp310 == Cyc_Absyn_Stdcall_att){
goto _LL315;} else{ goto _LL316;} _LL316: if( _temp310 == Cyc_Absyn_Cdecl_att){
goto _LL317;} else{ goto _LL318;} _LL318: if( _temp310 == Cyc_Absyn_Noreturn_att){
goto _LL319;} else{ goto _LL320;} _LL320: if( _temp310 == Cyc_Absyn_Const_att){
goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp310 > 15u?((
struct _enum_struct*) _temp310)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL331:
_temp330=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp310)->f1; goto
_LL323;} else{ goto _LL324;} _LL324: if( _temp310 == Cyc_Absyn_Packed_att){ goto
_LL325;} else{ goto _LL326;} _LL326: goto _LL327; _LL313: goto _LL315; _LL315:
goto _LL317; _LL317: goto _LL319; _LL319: goto _LL321; _LL321:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp332=(
char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 51; _temp333;})); goto _LL311; _LL323: goto
_LL325; _LL325: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp334= Cyc_Absyn_attribute2string((
void*) atts->hd); xprintf("bad attribute %.*s in function declaration", _temp334.last_plus_one
- _temp334.curr, _temp334.curr);})); goto _LL311; _LL327: continue; _LL311:;}}}
ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({
struct _tuple6* _temp335=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp335->f1=({ struct Cyc_Tcenv_VarRes_struct* _temp336=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp336->tag= Cyc_Tcenv_VarRes_tag;
_temp336->f1=( void*)({ struct Cyc_Absyn_Global_b_struct* _temp337=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp337->tag= Cyc_Absyn_Global_b_tag;
_temp337->f1= vd;( void*) _temp337;});( void*) _temp336;}); _temp335->f2=
is_redecl; _temp335;}));}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp338=(* fd->name).f1; struct Cyc_List_List* _temp346; struct Cyc_List_List*
_temp348; _LL340: if(( unsigned int) _temp338 > 1u?(( struct _enum_struct*)
_temp338)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL347: _temp346=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp338)->f1; if( _temp346 == 0){ goto _LL341;}
else{ goto _LL342;}} else{ goto _LL342;} _LL342: if(( unsigned int) _temp338 > 1u?((
struct _enum_struct*) _temp338)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL349:
_temp348=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp338)->f1;
goto _LL343;} else{ goto _LL344;} _LL344: goto _LL345; _LL341: goto _LL339;
_LL343:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp350=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp350->tag= Cyc_Core_Impossible_tag; _temp350->f1=( struct _tagged_string)({
char* _temp351=( char*)"tc: Abs_n in tcFndecl"; struct _tagged_string _temp352;
_temp352.curr= _temp351; _temp352.base= _temp351; _temp352.last_plus_one=
_temp351 + 22; _temp352;});( struct _xenum_struct*) _temp350;})); _LL345: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp353=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 43; _temp354;})); return; _LL339:;}(* fd->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp355=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp355->tag= Cyc_Absyn_Abs_n_tag;
_temp355->f1= te->ns;( void*) _temp355;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); int is_redecl= Cyc_Tc_var_redecl_okay(
ge, loc, v, t, sc, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts= atts->tl){ void* _temp356=( void*) atts->hd; int
_temp364; _LL358: if( _temp356 == Cyc_Absyn_Packed_att){ goto _LL359;} else{
goto _LL360;} _LL360: if(( unsigned int) _temp356 > 15u?(( struct _enum_struct*)
_temp356)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL365: _temp364=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp356)->f1; goto _LL361;} else{ goto
_LL362;} _LL362: goto _LL363; _LL359: goto _LL361; _LL361: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp366= Cyc_Absyn_attribute2string(( void*) atts->hd);
xprintf("bad attribute %.*s for function", _temp366.last_plus_one - _temp366.curr,
_temp366.curr);})); goto _LL357; _LL363: goto _LL357; _LL357:;}} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple6*
_temp367=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp367->f1=({
struct Cyc_Tcenv_VarRes_struct* _temp368=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp368->tag= Cyc_Tcenv_VarRes_tag;
_temp368->f1=( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp369=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp369->tag= Cyc_Absyn_Funname_b_tag;
_temp369->f1= fd;( void*) _temp369;});( void*) _temp368;}); _temp367->f2=
is_redecl; _temp367;})); te->le=({ struct Cyc_Core_Opt* _temp370=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp370->v=( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp371=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp371->tag= Cyc_Tcenv_Outermost_tag; _temp371->f1=( void*) Cyc_Tcenv_new_fenv(
fd);( void*) _temp371;}); _temp370;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if(
! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp372=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 49; _temp373;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp374=(* td->name).f1; struct Cyc_List_List*
_temp382; struct Cyc_List_List* _temp384; _LL376: if(( unsigned int) _temp374 >
1u?(( struct _enum_struct*) _temp374)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL383:
_temp382=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp374)->f1;
if( _temp382 == 0){ goto _LL377;} else{ goto _LL378;}} else{ goto _LL378;}
_LL378: if(( unsigned int) _temp374 > 1u?(( struct _enum_struct*) _temp374)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL385: _temp384=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp374)->f1; if( _temp384 == 0){ goto _LL379;} else{
goto _LL380;}} else{ goto _LL380;} _LL380: goto _LL381; _LL377: goto _LL375;
_LL379: goto _LL375; _LL381: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp386=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp387; _temp387.curr= _temp386; _temp387.base= _temp386;
_temp387.last_plus_one= _temp386 + 43; _temp387;})); return; _LL375:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp388=* v; xprintf("redeclaration of typedef %.*s",
_temp388.last_plus_one - _temp388.curr, _temp388.curr);})); return;}(* td->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp389=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp389->tag= Cyc_Absyn_Abs_n_tag;
_temp389->f1= te->ns;( void*) _temp389;}); Cyc_Tcutil_check_unique_tvars( loc,
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs, Cyc_Absyn_AnyKind,( void*) td->defn);
ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs,
v, td);} static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp390=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 47; _temp391;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( sd->name)->v)).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp392=( void*)
atts->hd; int _temp400; _LL394: if( _temp392 == Cyc_Absyn_Packed_att){ goto
_LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp392 > 15u?(( struct
_enum_struct*) _temp392)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL401: _temp400=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp392)->f1; goto _LL397;} else{
goto _LL398;} _LL398: goto _LL399; _LL395: continue; _LL397: continue; _LL399:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp402= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp403=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp402.last_plus_one - _temp402.curr, _temp402.curr, _temp403.last_plus_one -
_temp403.curr, _temp403.curr);})); goto _LL393; _LL393:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp404=( void*) c->v; void*
_temp412; _LL406: if( _temp404 == Cyc_Absyn_No_constr){ goto _LL407;} else{ goto
_LL408;} _LL408: if(( unsigned int) _temp404 > 1u?(( struct _enum_struct*)
_temp404)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL413: _temp412=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp404)->f1; if( _temp412 == Cyc_Absyn_MemKind){
goto _LL409;} else{ goto _LL410;}} else{ goto _LL410;} _LL410: goto _LL411;
_LL407:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp414=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp414->tag= Cyc_Absyn_Eq_constr_tag; _temp414->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp414;})); continue; _LL409: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp415=* v; struct _tagged_string _temp416=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp415.last_plus_one - _temp415.curr, _temp415.curr, _temp416.last_plus_one -
_temp416.curr, _temp416.curr);})); continue; _LL411: continue; _LL405:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp417=(*(( struct _tuple0*)( sd->name)->v)).f1;
struct Cyc_List_List* _temp425; struct Cyc_List_List* _temp427; _LL419: if((
unsigned int) _temp417 > 1u?(( struct _enum_struct*) _temp417)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL426: _temp425=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp417)->f1; if( _temp425 == 0){ goto _LL420;} else{ goto _LL421;}} else{ goto
_LL421;} _LL421: if(( unsigned int) _temp417 > 1u?(( struct _enum_struct*)
_temp417)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL428: _temp427=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp417)->f1; if( _temp427 == 0){ goto _LL422;}
else{ goto _LL423;}} else{ goto _LL423;} _LL423: goto _LL424; _LL420: goto
_LL418; _LL422: goto _LL418; _LL424: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp429=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 43; _temp430;})); return; _LL418:;}(*((
struct _tuple0*)( sd->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp431=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp431->tag= Cyc_Absyn_Abs_n_tag; _temp431->f1= te->ns;( void*) _temp431;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp432=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->structdecls, v); struct Cyc_Core_Opt _temp438; struct
Cyc_Absyn_Structdecl** _temp439; _LL434: if( _temp432 == 0){ goto _LL435;} else{
goto _LL436;} _LL436: if( _temp432 == 0){ goto _LL433;} else{ _temp438=*
_temp432; _LL440: _temp439=( struct Cyc_Absyn_Structdecl**) _temp438.v; goto
_LL437;} _LL435: ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, v,( struct Cyc_Absyn_Structdecl**)({ struct _tuple7* _temp441=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp441->f1= sd; _temp441;}));
if( sd->fields == 0){ return;} goto _LL433; _LL437: { struct Cyc_Absyn_Structdecl*
sd2=* _temp439; struct Cyc_List_List* tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp442=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp442.last_plus_one - _temp442.curr, _temp442.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp443=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp443->tag= Cyc_Absyn_Forward_constr_tag;
_temp443->f1= c2;( void*) _temp443;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp444=* v; struct
_tagged_string _temp445= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp446=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp447= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp444.last_plus_one - _temp444.curr, _temp444.curr, _temp445.last_plus_one -
_temp445.curr, _temp445.curr, _temp446.last_plus_one - _temp446.curr, _temp446.curr,
_temp447.last_plus_one - _temp447.curr, _temp447.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp448=( char*)"struct";
struct _tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 7; _temp449;}), v));{ struct _tuple8 _temp451=({
struct _tuple8 _temp450; _temp450.f1= sd->fields; _temp450.f2= sd2->fields;
_temp450;}); struct Cyc_Core_Opt* _temp461; struct Cyc_Core_Opt* _temp463;
struct Cyc_Core_Opt* _temp465; struct Cyc_Core_Opt* _temp467; struct Cyc_Core_Opt*
_temp469; struct Cyc_Core_Opt* _temp471; struct Cyc_Core_Opt* _temp473; struct
Cyc_Core_Opt* _temp475; _LL453: _LL464: _temp463= _temp451.f1; if( _temp463 == 0){
goto _LL462;} else{ goto _LL455;} _LL462: _temp461= _temp451.f2; if( _temp461 ==
0){ goto _LL454;} else{ goto _LL455;} _LL455: _LL468: _temp467= _temp451.f1;
goto _LL466; _LL466: _temp465= _temp451.f2; if( _temp465 == 0){ goto _LL456;}
else{ goto _LL457;} _LL457: _LL472: _temp471= _temp451.f1; if( _temp471 == 0){
goto _LL470;} else{ goto _LL459;} _LL470: _temp469= _temp451.f2; goto _LL458;
_LL459: _LL476: _temp475= _temp451.f1; goto _LL474; _LL474: _temp473= _temp451.f2;
goto _LL460; _LL454: return; _LL456:* _temp439= sd; goto _LL452; _LL458: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL460: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp477=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp477->hd=( void*)({ struct _tuple4* _temp478=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp478->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp478->f2=({ struct Cyc_Absyn_VarType_struct* _temp479=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp479->tag= Cyc_Absyn_VarType_tag;
_temp479->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp479;}); _temp478;});
_temp477->tl= inst; _temp477;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes,
sd2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp480=* v;
xprintf("redeclaration of struct %.*s has different attributes", _temp480.last_plus_one
- _temp480.curr, _temp480.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
sd->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( sd2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp483; struct Cyc_List_List* _temp484; void* _temp486; struct Cyc_Absyn_Tqual*
_temp488; struct _tagged_string* _temp490; struct Cyc_Absyn_Structfield*
_temp481=( struct Cyc_Absyn_Structfield*) f1s->hd; _temp483=* _temp481; _LL491:
_temp490=( struct _tagged_string*) _temp483.name; goto _LL489; _LL489: _temp488=(
struct Cyc_Absyn_Tqual*) _temp483.tq; goto _LL487; _LL487: _temp486=( void*)
_temp483.type; goto _LL485; _LL485: _temp484=( struct Cyc_List_List*) _temp483.attributes;
goto _LL482; _LL482: { struct Cyc_Absyn_Structfield _temp494; struct Cyc_List_List*
_temp495; void* _temp497; struct Cyc_Absyn_Tqual* _temp499; struct
_tagged_string* _temp501; struct Cyc_Absyn_Structfield* _temp492=( struct Cyc_Absyn_Structfield*)
f2s->hd; _temp494=* _temp492; _LL502: _temp501=( struct _tagged_string*)
_temp494.name; goto _LL500; _LL500: _temp499=( struct Cyc_Absyn_Tqual*) _temp494.tq;
goto _LL498; _LL498: _temp497=( void*) _temp494.type; goto _LL496; _LL496:
_temp495=( struct Cyc_List_List*) _temp494.attributes; goto _LL493; _LL493: if(
Cyc_String_zstrptrcmp( _temp490, _temp501) != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp503=* v; struct _tagged_string _temp504=* _temp490; struct
_tagged_string _temp505=* _temp501; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp503.last_plus_one - _temp503.curr, _temp503.curr, _temp504.last_plus_one -
_temp504.curr, _temp504.curr, _temp505.last_plus_one - _temp505.curr, _temp505.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp484, _temp495)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp506=* v; struct _tagged_string _temp507=* _temp490; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp506.last_plus_one - _temp506.curr, _temp506.curr, _temp507.last_plus_one -
_temp507.curr, _temp507.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp488,
_temp499)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp508=* v; struct
_tagged_string _temp509=* _temp490; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp508.last_plus_one - _temp508.curr, _temp508.curr, _temp509.last_plus_one -
_temp509.curr, _temp509.curr);}));} Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp486);{ void* subst_t2= Cyc_Tcutil_substitute( inst, _temp497); if( ! Cyc_Tcutil_unify(
_temp486, subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp510=* v;
struct _tagged_string _temp511=* _temp490; struct _tagged_string _temp512= Cyc_Absynpp_typ2string(
_temp486); struct _tagged_string _temp513= Cyc_Absynpp_typ2string( subst_t2);
xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp510.last_plus_one - _temp510.curr, _temp510.curr, _temp511.last_plus_one -
_temp511.curr, _temp511.curr, _temp512.last_plus_one - _temp512.curr, _temp512.curr,
_temp513.last_plus_one - _temp513.curr, _temp513.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp514=* v; struct
_tagged_string _temp515=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp514.last_plus_one
- _temp514.curr, _temp514.curr, _temp515.last_plus_one - _temp515.curr, _temp515.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp516=* v;
struct _tagged_string _temp517=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp516.last_plus_one
- _temp516.curr, _temp516.curr, _temp517.last_plus_one - _temp517.curr, _temp517.curr);}));}
return;}} _LL452:;} goto _LL433;}} _LL433:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp518=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp518->sc=( void*)(( void*) sd->sc); _temp518->name= sd->name; _temp518->tvs=
tvs; _temp518->fields= 0; _temp518->attributes= 0; _temp518;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield _temp521; struct Cyc_List_List*
_temp522; void* _temp524; struct Cyc_Absyn_Tqual* _temp526; struct
_tagged_string* _temp528; struct Cyc_Absyn_Structfield* _temp519=( struct Cyc_Absyn_Structfield*)
fs->hd; _temp521=* _temp519; _LL529: _temp528=( struct _tagged_string*) _temp521.name;
goto _LL527; _LL527: _temp526=( struct Cyc_Absyn_Tqual*) _temp521.tq; goto
_LL525; _LL525: _temp524=( void*) _temp521.type; goto _LL523; _LL523: _temp522=(
struct Cyc_List_List*) _temp521.attributes; goto _LL520; _LL520: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp528)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp530=* _temp528;
xprintf("duplicate field %.*s in struct", _temp530.last_plus_one - _temp530.curr,
_temp530.curr);}));} prev_fields=({ struct Cyc_List_List* _temp531=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp531->hd=( void*) _temp528;
_temp531->tl= prev_fields; _temp531;}); Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp524); for( 0; _temp522 != 0; _temp522= _temp522->tl){ void* _temp532=( void*)
_temp522->hd; int _temp540; _LL534: if( _temp532 == Cyc_Absyn_Packed_att){ goto
_LL535;} else{ goto _LL536;} _LL536: if(( unsigned int) _temp532 > 15u?(( struct
_enum_struct*) _temp532)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL541: _temp540=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp532)->f1; goto _LL537;} else{
goto _LL538;} _LL538: goto _LL539; _LL535: continue; _LL537: continue; _LL539:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp542= Cyc_Absyn_attribute2string((
void*) _temp522->hd); struct _tagged_string _temp543=* v; struct _tagged_string
_temp544=* _temp528; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp542.last_plus_one - _temp542.curr, _temp542.curr, _temp543.last_plus_one -
_temp543.curr, _temp543.curr, _temp544.last_plus_one - _temp544.curr, _temp544.curr);}));
goto _LL533; _LL533:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp545=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp545->f1=({ struct Cyc_Tcenv_StructRes_struct* _temp546=( struct
Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp546->tag= Cyc_Tcenv_StructRes_tag; _temp546->f1= sd;( void*) _temp546;});
_temp545->f2= 1; _temp545;}));* sdp= sd;}}}}} static void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp547=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 46; _temp548;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ud->name)->v)).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp549=( void*)
atts->hd; int _temp557; _LL551: if( _temp549 == Cyc_Absyn_Packed_att){ goto
_LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp549 > 15u?(( struct
_enum_struct*) _temp549)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL558: _temp557=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp549)->f1; goto _LL554;} else{
goto _LL555;} _LL555: goto _LL556; _LL552: continue; _LL554: continue; _LL556:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp559= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp560=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp559.last_plus_one - _temp559.curr, _temp559.curr, _temp560.last_plus_one -
_temp560.curr, _temp560.curr);})); goto _LL550; _LL550:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp561=( void*) c->v; void*
_temp569; _LL563: if( _temp561 == Cyc_Absyn_No_constr){ goto _LL564;} else{ goto
_LL565;} _LL565: if(( unsigned int) _temp561 > 1u?(( struct _enum_struct*)
_temp561)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL570: _temp569=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp561)->f1; if( _temp569 == Cyc_Absyn_MemKind){
goto _LL566;} else{ goto _LL567;}} else{ goto _LL567;} _LL567: goto _LL568;
_LL564:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp571=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp571->tag= Cyc_Absyn_Eq_constr_tag; _temp571->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp571;})); continue; _LL566: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp572=* v; struct _tagged_string _temp573=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("union %.*s attempts to abstract type variable %.*s of kind M",
_temp572.last_plus_one - _temp572.curr, _temp572.curr, _temp573.last_plus_one -
_temp573.curr, _temp573.curr);})); continue; _LL568: continue; _LL562:;}}{
struct Cyc_List_List* tvs= ud->tvs;{ void* _temp574=(*(( struct _tuple0*)( ud->name)->v)).f1;
struct Cyc_List_List* _temp582; struct Cyc_List_List* _temp584; _LL576: if((
unsigned int) _temp574 > 1u?(( struct _enum_struct*) _temp574)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL583: _temp582=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp574)->f1; if( _temp582 == 0){ goto _LL577;} else{ goto _LL578;}} else{ goto
_LL578;} _LL578: if(( unsigned int) _temp574 > 1u?(( struct _enum_struct*)
_temp574)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL585: _temp584=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp574)->f1; if( _temp584 == 0){ goto _LL579;}
else{ goto _LL580;}} else{ goto _LL580;} _LL580: goto _LL581; _LL577: goto
_LL575; _LL579: goto _LL575; _LL581: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp586=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp587; _temp587.curr= _temp586; _temp587.base= _temp586;
_temp587.last_plus_one= _temp586 + 43; _temp587;})); return; _LL575:;}(*((
struct _tuple0*)( ud->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp588=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp588->tag= Cyc_Absyn_Abs_n_tag; _temp588->f1= te->ns;( void*) _temp588;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp589=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct Cyc_Core_Opt _temp595; struct
Cyc_Absyn_Uniondecl** _temp596; _LL591: if( _temp589 == 0){ goto _LL592;} else{
goto _LL593;} _LL593: if( _temp589 == 0){ goto _LL590;} else{ _temp595=*
_temp589; _LL597: _temp596=( struct Cyc_Absyn_Uniondecl**) _temp595.v; goto
_LL594;} _LL592: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, v,( struct Cyc_Absyn_Uniondecl**)({ struct _tuple9* _temp598=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp598->f1= ud; _temp598;}));
if( ud->fields == 0){ return;} goto _LL590; _LL594: { struct Cyc_Absyn_Uniondecl*
ud2=* _temp596; struct Cyc_List_List* tvs2= ud2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp599=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp599.last_plus_one - _temp599.curr, _temp599.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp600=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp600->tag= Cyc_Absyn_Forward_constr_tag;
_temp600->f1= c2;( void*) _temp600;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp601=* v; struct
_tagged_string _temp602= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp603=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp604= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp601.last_plus_one - _temp601.curr, _temp601.curr, _temp602.last_plus_one -
_temp602.curr, _temp602.curr, _temp603.last_plus_one - _temp603.curr, _temp603.curr,
_temp604.last_plus_one - _temp604.curr, _temp604.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp605=( char*)"union";
struct _tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 6; _temp606;}), v));{ struct _tuple8 _temp608=({
struct _tuple8 _temp607; _temp607.f1= ud->fields; _temp607.f2= ud2->fields;
_temp607;}); struct Cyc_Core_Opt* _temp618; struct Cyc_Core_Opt* _temp620;
struct Cyc_Core_Opt* _temp622; struct Cyc_Core_Opt* _temp624; struct Cyc_Core_Opt*
_temp626; struct Cyc_Core_Opt* _temp628; struct Cyc_Core_Opt* _temp630; struct
Cyc_Core_Opt* _temp632; _LL610: _LL621: _temp620= _temp608.f1; if( _temp620 == 0){
goto _LL619;} else{ goto _LL612;} _LL619: _temp618= _temp608.f2; if( _temp618 ==
0){ goto _LL611;} else{ goto _LL612;} _LL612: _LL625: _temp624= _temp608.f1;
goto _LL623; _LL623: _temp622= _temp608.f2; if( _temp622 == 0){ goto _LL613;}
else{ goto _LL614;} _LL614: _LL629: _temp628= _temp608.f1; if( _temp628 == 0){
goto _LL627;} else{ goto _LL616;} _LL627: _temp626= _temp608.f2; goto _LL615;
_LL616: _LL633: _temp632= _temp608.f1; goto _LL631; _LL631: _temp630= _temp608.f2;
goto _LL617; _LL611: return; _LL613:* _temp596= ud; goto _LL609; _LL615: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL617: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp634=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp634->hd=( void*)({ struct _tuple4* _temp635=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp635->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp635->f2=({ struct Cyc_Absyn_VarType_struct* _temp636=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp636->tag= Cyc_Absyn_VarType_tag;
_temp636->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp636;}); _temp635;});
_temp634->tl= inst; _temp634;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes,
ud2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp637=* v;
xprintf("redeclaration of union %.*s has different attributes", _temp637.last_plus_one
- _temp637.curr, _temp637.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ud->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ud2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp640; struct Cyc_List_List* _temp641; void* _temp643; struct Cyc_Absyn_Tqual*
_temp645; struct _tagged_string* _temp647; struct Cyc_Absyn_Structfield*
_temp638=( struct Cyc_Absyn_Structfield*) f1s->hd; _temp640=* _temp638; _LL648:
_temp647=( struct _tagged_string*) _temp640.name; goto _LL646; _LL646: _temp645=(
struct Cyc_Absyn_Tqual*) _temp640.tq; goto _LL644; _LL644: _temp643=( void*)
_temp640.type; goto _LL642; _LL642: _temp641=( struct Cyc_List_List*) _temp640.attributes;
goto _LL639; _LL639: { struct Cyc_Absyn_Structfield _temp651; struct Cyc_List_List*
_temp652; void* _temp654; struct Cyc_Absyn_Tqual* _temp656; struct
_tagged_string* _temp658; struct Cyc_Absyn_Structfield* _temp649=( struct Cyc_Absyn_Structfield*)
f2s->hd; _temp651=* _temp649; _LL659: _temp658=( struct _tagged_string*)
_temp651.name; goto _LL657; _LL657: _temp656=( struct Cyc_Absyn_Tqual*) _temp651.tq;
goto _LL655; _LL655: _temp654=( void*) _temp651.type; goto _LL653; _LL653:
_temp652=( struct Cyc_List_List*) _temp651.attributes; goto _LL650; _LL650: if(
Cyc_String_zstrptrcmp( _temp647, _temp658) != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp660=* v; struct _tagged_string _temp661=* _temp647; struct
_tagged_string _temp662=* _temp658; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp660.last_plus_one - _temp660.curr, _temp660.curr, _temp661.last_plus_one -
_temp661.curr, _temp661.curr, _temp662.last_plus_one - _temp662.curr, _temp662.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp641, _temp652)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp663=* v; struct _tagged_string _temp664=* _temp647; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp663.last_plus_one - _temp663.curr, _temp663.curr, _temp664.last_plus_one -
_temp664.curr, _temp664.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp645,
_temp656)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp665=* v; struct
_tagged_string _temp666=* _temp647; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp665.last_plus_one - _temp665.curr, _temp665.curr, _temp666.last_plus_one -
_temp666.curr, _temp666.curr);}));} Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp643);{ void* subst_t2= Cyc_Tcutil_substitute( inst, _temp654); if( ! Cyc_Tcutil_unify(
_temp643, subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp667=* v;
struct _tagged_string _temp668=* _temp647; struct _tagged_string _temp669= Cyc_Absynpp_typ2string(
_temp643); struct _tagged_string _temp670= Cyc_Absynpp_typ2string( subst_t2);
xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp667.last_plus_one - _temp667.curr, _temp667.curr, _temp668.last_plus_one -
_temp668.curr, _temp668.curr, _temp669.last_plus_one - _temp669.curr, _temp669.curr,
_temp670.last_plus_one - _temp670.curr, _temp670.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp671=* v; struct
_tagged_string _temp672=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp671.last_plus_one
- _temp671.curr, _temp671.curr, _temp672.last_plus_one - _temp672.curr, _temp672.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp673=* v;
struct _tagged_string _temp674=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp673.last_plus_one
- _temp673.curr, _temp673.curr, _temp674.last_plus_one - _temp674.curr, _temp674.curr);}));}
return;}} _LL609:;} goto _LL590;}} _LL590:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp675=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp675->sc=( void*)(( void*) ud->sc); _temp675->name= ud->name; _temp675->tvs=
tvs; _temp675->fields= 0; _temp675->attributes= 0; _temp675;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( ud->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield
_temp678; struct Cyc_List_List* _temp679; void* _temp681; struct Cyc_Absyn_Tqual*
_temp683; struct _tagged_string* _temp685; struct Cyc_Absyn_Structfield*
_temp676=( struct Cyc_Absyn_Structfield*) fs->hd; _temp678=* _temp676; _LL686:
_temp685=( struct _tagged_string*) _temp678.name; goto _LL684; _LL684: _temp683=(
struct Cyc_Absyn_Tqual*) _temp678.tq; goto _LL682; _LL682: _temp681=( void*)
_temp678.type; goto _LL680; _LL680: _temp679=( struct Cyc_List_List*) _temp678.attributes;
goto _LL677; _LL677: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp685)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp687=* _temp685; xprintf("duplicate field %.*s in union",
_temp687.last_plus_one - _temp687.curr, _temp687.curr);}));} prev_fields=({
struct Cyc_List_List* _temp688=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp688->hd=( void*) _temp685; _temp688->tl=
prev_fields; _temp688;}); Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp681); if( ! Cyc_Tcutil_bits_only( _temp681)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp689=* _temp685; struct _tagged_string _temp690=* v;
xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp689.last_plus_one - _temp689.curr, _temp689.curr, _temp690.last_plus_one -
_temp690.curr, _temp690.curr);}));} for( 0; _temp679 != 0; _temp679= _temp679->tl){
void* _temp691=( void*) _temp679->hd; int _temp699; _LL693: if( _temp691 == Cyc_Absyn_Packed_att){
goto _LL694;} else{ goto _LL695;} _LL695: if(( unsigned int) _temp691 > 15u?((
struct _enum_struct*) _temp691)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL700:
_temp699=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp691)->f1; goto
_LL696;} else{ goto _LL697;} _LL697: goto _LL698; _LL694: continue; _LL696:
continue; _LL698: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp701= Cyc_Absyn_attribute2string((
void*) _temp679->hd); struct _tagged_string _temp702=* v; struct _tagged_string
_temp703=* _temp685; xprintf("bad attribute %.*s in union %.*s, member %.*s",
_temp701.last_plus_one - _temp701.curr, _temp701.curr, _temp702.last_plus_one -
_temp702.curr, _temp702.curr, _temp703.last_plus_one - _temp703.curr, _temp703.curr);}));
goto _LL692; _LL692:;}}}* udp= ud;}}}}} static void Cyc_Tc_tcEnumdecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Enumdecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp704=( char*)"anonymous enums are not allowed at top level";
struct _tagged_string _temp705; _temp705.curr= _temp704; _temp705.base= _temp704;
_temp705.last_plus_one= _temp704 + 45; _temp705;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ed->name)->v)).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void*
_temp706=( void*) c->v; void* _temp714; _LL708: if( _temp706 == Cyc_Absyn_No_constr){
goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int) _temp706 > 1u?((
struct _enum_struct*) _temp706)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL715:
_temp714=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp706)->f1; if(
_temp714 == Cyc_Absyn_MemKind){ goto _LL711;} else{ goto _LL712;}} else{ goto
_LL712;} _LL712: goto _LL713; _LL709:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp716=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp716->tag= Cyc_Absyn_Eq_constr_tag; _temp716->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp716;})); goto _LL707; _LL711: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp717=* v; struct _tagged_string _temp718=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("enum %.*s attempts to abstract type variable %.*s of kind M",
_temp717.last_plus_one - _temp717.curr, _temp717.curr, _temp718.last_plus_one -
_temp718.curr, _temp718.curr);})); goto _LL707; _LL713: goto _LL707; _LL707:;}}{
void* _temp719=(*(( struct _tuple0*)( ed->name)->v)).f1; struct Cyc_List_List*
_temp727; struct Cyc_List_List* _temp729; _LL721: if(( unsigned int) _temp719 >
1u?(( struct _enum_struct*) _temp719)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL728:
_temp727=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp719)->f1;
if( _temp727 == 0){ goto _LL722;} else{ goto _LL723;}} else{ goto _LL723;}
_LL723: if(( unsigned int) _temp719 > 1u?(( struct _enum_struct*) _temp719)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL730: _temp729=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp719)->f1; if( _temp729 == 0){ goto _LL724;} else{
goto _LL725;}} else{ goto _LL725;} _LL725: goto _LL726; _LL722: goto _LL720;
_LL724: goto _LL720; _LL726: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp731=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 43; _temp732;})); return; _LL720:;}(*((
struct _tuple0*)( ed->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp733=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp733->tag= Cyc_Absyn_Abs_n_tag; _temp733->f1= te->ns;( void*) _temp733;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp734=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->enumdecls, v); struct Cyc_Core_Opt _temp740; struct
Cyc_Absyn_Enumdecl** _temp741; _LL736: if( _temp734 == 0){ goto _LL737;} else{
goto _LL738;} _LL738: if( _temp734 == 0){ goto _LL735;} else{ _temp740=*
_temp734; _LL742: _temp741=( struct Cyc_Absyn_Enumdecl**) _temp740.v; goto
_LL739;} _LL737: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,( struct Cyc_Absyn_Enumdecl**)({ struct _tuple10* _temp743=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp743->f1= ed;
_temp743;})); if( ed->fields == 0){ return;} goto _LL735; _LL739: { struct Cyc_Absyn_Enumdecl*
ed2=* _temp741; struct Cyc_List_List* tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp744=* v; xprintf("redeclaration of enum %.*s has a different number of type parameters",
_temp744.last_plus_one - _temp744.curr, _temp744.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp745=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp745->tag= Cyc_Absyn_Forward_constr_tag;
_temp745->f1= c2;( void*) _temp745;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp746=* v; struct
_tagged_string _temp747= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp748=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp749= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of enum %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp746.last_plus_one - _temp746.curr, _temp746.curr, _temp747.last_plus_one -
_temp747.curr, _temp747.curr, _temp748.last_plus_one - _temp748.curr, _temp748.curr,
_temp749.last_plus_one - _temp749.curr, _temp749.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp750=( char*)"enum";
struct _tagged_string _temp751; _temp751.curr= _temp750; _temp751.base= _temp750;
_temp751.last_plus_one= _temp750 + 5; _temp751;}), v));{ struct _tuple8 _temp753=({
struct _tuple8 _temp752; _temp752.f1= ed->fields; _temp752.f2= ed2->fields;
_temp752;}); struct Cyc_Core_Opt* _temp763; struct Cyc_Core_Opt* _temp765;
struct Cyc_Core_Opt* _temp767; struct Cyc_Core_Opt* _temp769; struct Cyc_Core_Opt*
_temp771; struct Cyc_Core_Opt* _temp773; struct Cyc_Core_Opt* _temp775; struct
Cyc_Core_Opt* _temp777; _LL755: _LL766: _temp765= _temp753.f1; if( _temp765 == 0){
goto _LL764;} else{ goto _LL757;} _LL764: _temp763= _temp753.f2; if( _temp763 ==
0){ goto _LL756;} else{ goto _LL757;} _LL757: _LL770: _temp769= _temp753.f1;
goto _LL768; _LL768: _temp767= _temp753.f2; if( _temp767 == 0){ goto _LL758;}
else{ goto _LL759;} _LL759: _LL774: _temp773= _temp753.f1; if( _temp773 == 0){
goto _LL772;} else{ goto _LL761;} _LL772: _temp771= _temp753.f2; goto _LL760;
_LL761: _LL778: _temp777= _temp753.f1; goto _LL776; _LL776: _temp775= _temp753.f2;
goto _LL762; _LL756: return; _LL758:* _temp741= ed; goto _LL754; _LL760: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL762: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp779=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp779->hd=( void*)({ struct _tuple4* _temp780=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp780->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp780->f2=({ struct Cyc_Absyn_VarType_struct* _temp781=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp781->tag= Cyc_Absyn_VarType_tag;
_temp781->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp781;}); _temp780;});
_temp779->tl= inst; _temp779;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ed->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ed2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){(*(( struct Cyc_Absyn_Enumfield*)
f1s->hd)->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp782=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp782->tag= Cyc_Absyn_Abs_n_tag;
_temp782->f1= te->ns;( void*) _temp782;});{ struct Cyc_List_List* alltvs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs,(( struct Cyc_Absyn_Enumfield*) f1s->hd)->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Enumfield*) f1s->hd)->loc, alltvs);{ struct Cyc_List_List* typs=((
struct Cyc_Absyn_Enumfield*) f1s->hd)->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Enumfield*)
f1s->hd)->loc, te, alltvs, Cyc_Absyn_MemKind, t);}} Cyc_Tc_field_redecl_okay((
struct Cyc_Absyn_Enumfield*) f1s->hd,( struct Cyc_Absyn_Enumfield*) f2s->hd,
inst, te,( struct _tagged_string)({ char* _temp783=( char*)"enum"; struct
_tagged_string _temp784; _temp784.curr= _temp783; _temp784.base= _temp783;
_temp784.last_plus_one= _temp783 + 5; _temp784;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp785=* v; struct _tagged_string _temp786=*(*((
struct Cyc_Absyn_Enumfield*) f1s->hd)->name).f2; xprintf("redeclaration of enum %.*s has extra field %.*s",
_temp785.last_plus_one - _temp785.curr, _temp785.curr, _temp786.last_plus_one -
_temp786.curr, _temp786.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp787=* v; struct _tagged_string _temp788=*(*(( struct
Cyc_Absyn_Enumfield*) f2s->hd)->name).f2; xprintf("redeclaration of enum %.*s is missing field %.*s",
_temp787.last_plus_one - _temp787.curr, _temp787.curr, _temp788.last_plus_one -
_temp788.curr, _temp788.curr);}));} return;}} _LL754:;} goto _LL735;}} _LL735:;}{
struct Cyc_Absyn_Enumdecl** edp=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v);* edp=({
struct Cyc_Absyn_Enumdecl* _temp789=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp789->sc=( void*)(( void*) ed->sc);
_temp789->name= ed->name; _temp789->tvs= tvs; _temp789->fields= 0; _temp789;});{
struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(
ed->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*) fs->hd; if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){
Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp790=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp790.last_plus_one - _temp790.curr,
_temp790.curr);}));} prev_fields=({ struct Cyc_List_List* _temp791=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp791->hd=( void*)(* f->name).f2;
_temp791->tl= prev_fields; _temp791;}); if( f->tag != 0){ Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp792= f->tag; if( _temp792 ==
0){ _throw( Null_Exception);} _temp792;}));}{ struct Cyc_List_List* tvs= f->tvs;
for( 0; tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)
tvs->hd; struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp793=( void*) c->v; void*
_temp801; _LL795: if( _temp793 == Cyc_Absyn_No_constr){ goto _LL796;} else{ goto
_LL797;} _LL797: if(( unsigned int) _temp793 > 1u?(( struct _enum_struct*)
_temp793)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL802: _temp801=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp793)->f1; if( _temp801 == Cyc_Absyn_MemKind){
goto _LL798;} else{ goto _LL799;}} else{ goto _LL799;} _LL799: goto _LL800;
_LL796:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp803=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp803->tag= Cyc_Absyn_Eq_constr_tag; _temp803->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp803;})); goto _LL794; _LL798: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp804=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp804.last_plus_one - _temp804.curr, _temp804.curr);})); goto _LL794; _LL800:
goto _LL794; _LL794:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs);
Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List* typs= f->typs;
for( 0; typs != 0; typs= typs->tl){ void* t=(*(( struct _tuple5*) typs->hd)).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs, Cyc_Absyn_MemKind, t);}}(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp805=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp805->tag= Cyc_Absyn_Abs_n_tag;
_temp805->f1= te->ns;( void*) _temp805;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple6* _temp806=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp806->f1=({ struct Cyc_Tcenv_EnumRes_struct*
_temp807=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp807->tag= Cyc_Tcenv_EnumRes_tag; _temp807->f1= ed; _temp807->f2= f;( void*)
_temp807;}); _temp806->f2= 1; _temp806;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl**
data)) Cyc_Dict_insert)( ge->enumdecls, v,( struct Cyc_Absyn_Enumdecl**)({
struct _tuple10* _temp808=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp808->f1= ed; _temp808;}));* edp= ed;}}}} static void Cyc_Tc_tcXenumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Xenumdecl* xd){ struct _tagged_string* v=(* xd->name).f2;
struct Cyc_Core_Opt* xd2o;{ struct _handler_cons _temp809; _push_handler(&
_temp809);{ struct _xenum_struct* _temp810=( struct _xenum_struct*) setjmp(
_temp809.handler); if( ! _temp810){ xd2o= Cyc_Tcenv_lookup_xenumdecl( te, loc,
xd->name);; _pop_handler();} else{ struct _xenum_struct* _temp812= _temp810;
_LL814: if( _temp812->tag == Cyc_Dict_Absent_tag){ goto _LL815;} else{ goto
_LL816;} _LL816: goto _LL817; _LL815: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp818= Cyc_Absynpp_qvar2string( xd->name); xprintf("qualified xenum declaration %.*s is not an existing xenum",
_temp818.last_plus_one - _temp818.curr, _temp818.curr);})); return; _LL817:(
void) _throw( _temp812); _LL813:;}}}{ void* _temp819=(* xd->name).f1; struct Cyc_List_List*
_temp829; struct Cyc_List_List* _temp831; struct Cyc_List_List* _temp833; _LL821:
if( _temp819 == Cyc_Absyn_Loc_n){ goto _LL822;} else{ goto _LL823;} _LL823: if((
unsigned int) _temp819 > 1u?(( struct _enum_struct*) _temp819)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL830: _temp829=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp819)->f1; if( _temp829 == 0){ goto _LL824;} else{ goto _LL825;}} else{ goto
_LL825;} _LL825: if(( unsigned int) _temp819 > 1u?(( struct _enum_struct*)
_temp819)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL832: _temp831=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp819)->f1; goto _LL826;} else{ goto _LL827;}
_LL827: if(( unsigned int) _temp819 > 1u?(( struct _enum_struct*) _temp819)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL834: _temp833=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp819)->f1; goto _LL828;} else{ goto _LL820;} _LL822:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp835=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp835->tag= Cyc_Core_Impossible_tag;
_temp835->f1=( struct _tagged_string)({ char* _temp836=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp837; _temp837.curr= _temp836; _temp837.base= _temp836;
_temp837.last_plus_one= _temp836 + 19; _temp837;});( struct _xenum_struct*)
_temp835;})); _LL824:(* xd->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp838=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp838->tag= Cyc_Absyn_Abs_n_tag; _temp838->f1= te->ns;( void*) _temp838;});
goto _LL820; _LL826:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp839=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp839->tag= Cyc_Core_Impossible_tag; _temp839->f1=( struct _tagged_string)({
char* _temp840=( char*)"tcXenumdecl: Rel_n non-null"; struct _tagged_string
_temp841; _temp841.curr= _temp840; _temp841.base= _temp840; _temp841.last_plus_one=
_temp840 + 28; _temp841;});( struct _xenum_struct*) _temp839;})); _LL828:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp842=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp842->tag= Cyc_Core_Impossible_tag;
_temp842->f1=( struct _tagged_string)({ char* _temp843=( char*)"tcXenumdecl: Abs_n";
struct _tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 19; _temp844;});( struct _xenum_struct*)
_temp842;})); _LL820:;} if( xd2o == 0){ struct Cyc_Absyn_Xenumdecl** x=( struct
Cyc_Absyn_Xenumdecl**)({ struct _tuple11* _temp845=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp845->f1=({ struct Cyc_Absyn_Xenumdecl* _temp846=(
struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp846->sc=( void*)(( void*) xd->sc); _temp846->name= xd->name; _temp846->fields=
0; _temp846;}); _temp845;}); xd2o=({ struct Cyc_Core_Opt* _temp847=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp847->v=( void*) x; _temp847;});
ge->xenumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Xenumdecl** data)) Cyc_Dict_insert)( ge->xenumdecls,
v, x);} else{ struct Cyc_Absyn_Xenumdecl* x=*(( struct Cyc_Absyn_Xenumdecl**)
xd2o->v);( void*)( x->sc=( void*)(( void*(*)( void* s1, struct Cyc_Core_Opt*
fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp848=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp848->v=( void*) 0;
_temp848;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp849=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp849->v=( void*) 0;
_temp849;}), loc,( struct _tagged_string)({ char* _temp850=( char*)"xenum";
struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 6; _temp851;}), v));}{ struct Cyc_Absyn_Xenumdecl*
xd_result=*(( struct Cyc_Absyn_Xenumdecl**) xd2o->v); struct Cyc_List_List* fs=
xd->fields; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct
Cyc_Absyn_Enumfield*) fs->hd;{ void* _temp852=(* f->name).f1; struct Cyc_List_List*
_temp862; struct Cyc_List_List* _temp864; struct Cyc_List_List* _temp866; _LL854:
if(( unsigned int) _temp852 > 1u?(( struct _enum_struct*) _temp852)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL863: _temp862=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp852)->f1; if( _temp862 == 0){ goto _LL855;} else{ goto _LL856;}} else{ goto
_LL856;} _LL856: if(( unsigned int) _temp852 > 1u?(( struct _enum_struct*)
_temp852)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL865: _temp864=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp852)->f1; goto _LL857;} else{ goto _LL858;}
_LL858: if(( unsigned int) _temp852 > 1u?(( struct _enum_struct*) _temp852)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL867: _temp866=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp852)->f1; goto _LL859;} else{ goto _LL860;} _LL860:
if( _temp852 == Cyc_Absyn_Loc_n){ goto _LL861;} else{ goto _LL853;} _LL855:(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp868=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp868->tag= Cyc_Absyn_Abs_n_tag;
_temp868->f1= te->ns;( void*) _temp868;}); goto _LL853; _LL857: goto _LL859;
_LL859: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp869=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp870; _temp870.curr= _temp869; _temp870.base= _temp869;
_temp870.last_plus_one= _temp869 + 40; _temp870;})); goto _LL853; _LL861:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp871=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp871->tag= Cyc_Core_Impossible_tag;
_temp871->f1=( struct _tagged_string)({ char* _temp872=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 19; _temp873;});( struct _xenum_struct*)
_temp871;})); goto _LL853; _LL853:;}{ struct Cyc_List_List* tvs= f->tvs; for( 0;
tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*) tvs->hd;
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
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
goto _LL875; _LL875:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type( f->loc, te,
tvs, Cyc_Absyn_MemKind, t);}}{ int dup= 0;{ struct Cyc_List_List* prev_fs=
xd_result->fields; for( 0; prev_fs != 0; prev_fs= prev_fs->tl){ struct Cyc_Absyn_Enumfield*
prev=( struct Cyc_Absyn_Enumfield*) prev_fs->hd; if( Cyc_Absyn_qvar_cmp( f->name,
prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp886=( char*)"xenum"; struct _tagged_string _temp887;
_temp887.curr= _temp886; _temp887.base= _temp886; _temp887.last_plus_one=
_temp886 + 6; _temp887;}), v); dup= 1; break;}}} if( dup){ continue;} xd_result->fields=({
struct Cyc_List_List* _temp888=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp888->hd=( void*) f; _temp888->tl= xd_result->fields;
_temp888;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List* ns_f;{ void*
_temp889=(* f->name).f1; struct Cyc_List_List* _temp897; struct Cyc_List_List*
_temp899; _LL891: if(( unsigned int) _temp889 > 1u?(( struct _enum_struct*)
_temp889)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL898: _temp897=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp889)->f1; goto _LL892;} else{ goto _LL893;}
_LL893: if( _temp889 == Cyc_Absyn_Loc_n){ goto _LL894;} else{ goto _LL895;}
_LL895: if(( unsigned int) _temp889 > 1u?(( struct _enum_struct*) _temp889)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL900: _temp899=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp889)->f1; goto _LL896;} else{ goto _LL890;} _LL892:
ns_f= _temp897; goto _LL890; _LL894: goto _LL896; _LL896:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp901=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp901->tag= Cyc_Core_Impossible_tag;
_temp901->f1=( struct _tagged_string)({ char* _temp902=( char*)"tcXenumdecl: Rel_n or Loc_n";
struct _tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 28; _temp903;});( struct _xenum_struct*)
_temp901;})); goto _LL890; _LL890:;}{ struct _handler_cons _temp904;
_push_handler(& _temp904);{ struct _xenum_struct* _temp905=( struct
_xenum_struct*) setjmp( _temp904.handler); if( ! _temp905){ ge_f=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
ns_f);; _pop_handler();} else{ struct _xenum_struct* _temp907= _temp905; _LL909:
if( _temp907->tag == Cyc_Dict_Absent_tag){ goto _LL910;} else{ goto _LL911;}
_LL911: goto _LL912; _LL910: Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp913= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xenum field %.*s",
_temp913.last_plus_one - _temp913.curr, _temp913.curr);})); return; _LL912:(
void) _throw( _temp907); _LL908:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp914=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp914->f1=({ struct Cyc_Tcenv_XenumRes_struct*
_temp915=( struct Cyc_Tcenv_XenumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XenumRes_struct));
_temp915->tag= Cyc_Tcenv_XenumRes_tag; _temp915->f1= xd_result; _temp915->f2= f;(
void*) _temp915;}); _temp914->f2= 1; _temp914;}));}}}}}} static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment* loc, void* sc){ void* _temp916= sc; _LL918: if(
_temp916 == Cyc_Absyn_Static){ goto _LL919;} else{ goto _LL920;} _LL920: if(
_temp916 == Cyc_Absyn_Abstract){ goto _LL921;} else{ goto _LL922;} _LL922: if(
_temp916 == Cyc_Absyn_Public){ goto _LL923;} else{ goto _LL924;} _LL924: if(
_temp916 == Cyc_Absyn_Extern){ goto _LL925;} else{ goto _LL926;} _LL926: if(
_temp916 == Cyc_Absyn_ExternC){ goto _LL927;} else{ goto _LL917;} _LL919: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp928=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp929; _temp929.curr= _temp928; _temp929.base= _temp928;
_temp929.last_plus_one= _temp928 + 44; _temp929;})); return 0; _LL921: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp930=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp931; _temp931.curr= _temp930; _temp931.base= _temp930;
_temp931.last_plus_one= _temp930 + 46; _temp931;})); return 0; _LL923: return 1;
_LL925: return 1; _LL927: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp932=( char*)"nested extern \"C\" declaration"; struct _tagged_string
_temp933; _temp933.curr= _temp932; _temp933.base= _temp932; _temp933.last_plus_one=
_temp932 + 30; _temp933;})); return 1; _LL917:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC){ struct
Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_List_List* ds=
ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; struct Cyc_Position_Segment* loc= d->loc; void* _temp934=( void*) d->r;
int _temp958; struct Cyc_Absyn_Exp* _temp960; struct Cyc_Core_Opt* _temp962;
struct Cyc_Core_Opt* _temp964; struct Cyc_Absyn_Pat* _temp966; struct Cyc_Absyn_Vardecl*
_temp968; struct Cyc_Absyn_Fndecl* _temp970; struct Cyc_Absyn_Typedefdecl*
_temp972; struct Cyc_Absyn_Structdecl* _temp974; struct Cyc_Absyn_Uniondecl*
_temp976; struct Cyc_Absyn_Enumdecl* _temp978; struct Cyc_Absyn_Xenumdecl*
_temp980; struct Cyc_List_List* _temp982; struct _tagged_string* _temp984;
struct Cyc_List_List* _temp986; struct _tuple0* _temp988; struct _tuple0
_temp990; struct _tagged_string* _temp991; void* _temp993; struct Cyc_List_List*
_temp995; _LL936: if((( struct _enum_struct*) _temp934)->tag == Cyc_Absyn_Let_d_tag){
_LL967: _temp966=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp934)->f1; goto _LL965; _LL965: _temp964=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp934)->f2; goto _LL963; _LL963: _temp962=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp934)->f3; goto _LL961;
_LL961: _temp960=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp934)->f4; goto _LL959; _LL959: _temp958=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp934)->f5; goto _LL937;} else{ goto _LL938;} _LL938: if((( struct
_enum_struct*) _temp934)->tag == Cyc_Absyn_Var_d_tag){ _LL969: _temp968=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp934)->f1; goto _LL939;}
else{ goto _LL940;} _LL940: if((( struct _enum_struct*) _temp934)->tag == Cyc_Absyn_Fn_d_tag){
_LL971: _temp970=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp934)->f1; goto _LL941;} else{ goto _LL942;} _LL942: if((( struct
_enum_struct*) _temp934)->tag == Cyc_Absyn_Typedef_d_tag){ _LL973: _temp972=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp934)->f1;
goto _LL943;} else{ goto _LL944;} _LL944: if((( struct _enum_struct*) _temp934)->tag
== Cyc_Absyn_Struct_d_tag){ _LL975: _temp974=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp934)->f1; goto _LL945;} else{ goto
_LL946;} _LL946: if((( struct _enum_struct*) _temp934)->tag == Cyc_Absyn_Union_d_tag){
_LL977: _temp976=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp934)->f1; goto _LL947;} else{ goto _LL948;} _LL948: if((( struct
_enum_struct*) _temp934)->tag == Cyc_Absyn_Enum_d_tag){ _LL979: _temp978=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp934)->f1;
goto _LL949;} else{ goto _LL950;} _LL950: if((( struct _enum_struct*) _temp934)->tag
== Cyc_Absyn_Xenum_d_tag){ _LL981: _temp980=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp934)->f1; goto _LL951;} else{ goto _LL952;}
_LL952: if((( struct _enum_struct*) _temp934)->tag == Cyc_Absyn_Namespace_d_tag){
_LL985: _temp984=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp934)->f1; goto _LL983; _LL983: _temp982=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp934)->f2; goto _LL953;} else{ goto _LL954;}
_LL954: if((( struct _enum_struct*) _temp934)->tag == Cyc_Absyn_Using_d_tag){
_LL989: _temp988=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp934)->f1;
_temp990=* _temp988; _LL994: _temp993= _temp990.f1; goto _LL992; _LL992:
_temp991= _temp990.f2; goto _LL987; _LL987: _temp986=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp934)->f2; goto _LL955;} else{ goto _LL956;}
_LL956: if((( struct _enum_struct*) _temp934)->tag == Cyc_Absyn_ExternC_d_tag){
_LL996: _temp995=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp934)->f1; goto _LL957;} else{ goto _LL935;} _LL937: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp997=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp998; _temp998.curr= _temp997; _temp998.base= _temp997;
_temp998.last_plus_one= _temp997 + 47; _temp998;})); goto _LL935; _LL939: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp968->sc): 0){( void*)(
_temp968->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcVardecl( te, ge, loc,
_temp968); goto _LL935; _LL941: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp970->sc): 0){( void*)( _temp970->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcFndecl( te, ge, loc, _temp970); goto _LL935; _LL943: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp972); goto _LL935; _LL945: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp974->sc): 0){( void*)( _temp974->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp974); goto _LL935; _LL947: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp976->sc): 0){( void*)( _temp976->sc=(
void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcUniondecl( te, ge, loc, _temp976); goto
_LL935; _LL949: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp978->sc):
0){( void*)( _temp978->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcEnumdecl( te,
ge, loc, _temp978); goto _LL935; _LL951: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp980->sc): 0){( void*)( _temp980->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcXenumdecl( te, ge, loc, _temp980); goto _LL935; _LL953: { struct Cyc_List_List*
ns= te->ns; struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp999=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp999->hd=( void*) _temp984; _temp999->tl= 0; _temp999;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp984)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp984); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp982, in_externC); te->ns= ns; goto _LL935;}
_LL955: { struct _tagged_string* first; struct Cyc_List_List* rest;{ void*
_temp1000= _temp993; struct Cyc_List_List* _temp1012; struct Cyc_List_List*
_temp1014; struct Cyc_List_List* _temp1016; struct Cyc_List_List _temp1018;
struct Cyc_List_List* _temp1019; struct _tagged_string* _temp1021; struct Cyc_List_List*
_temp1023; struct Cyc_List_List _temp1025; struct Cyc_List_List* _temp1026;
struct _tagged_string* _temp1028; _LL1002: if( _temp1000 == Cyc_Absyn_Loc_n){
goto _LL1003;} else{ goto _LL1004;} _LL1004: if(( unsigned int) _temp1000 > 1u?((
struct _enum_struct*) _temp1000)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1013:
_temp1012=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1000)->f1;
if( _temp1012 == 0){ goto _LL1005;} else{ goto _LL1006;}} else{ goto _LL1006;}
_LL1006: if(( unsigned int) _temp1000 > 1u?(( struct _enum_struct*) _temp1000)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1015: _temp1014=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1000)->f1; if( _temp1014 == 0){ goto _LL1007;}
else{ goto _LL1008;}} else{ goto _LL1008;} _LL1008: if(( unsigned int) _temp1000
> 1u?(( struct _enum_struct*) _temp1000)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1017: _temp1016=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1000)->f1; if( _temp1016 == 0){ goto _LL1010;} else{ _temp1018=* _temp1016;
_LL1022: _temp1021=( struct _tagged_string*) _temp1018.hd; goto _LL1020; _LL1020:
_temp1019=( struct Cyc_List_List*) _temp1018.tl; goto _LL1009;}} else{ goto
_LL1010;} _LL1010: if(( unsigned int) _temp1000 > 1u?(( struct _enum_struct*)
_temp1000)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1024: _temp1023=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1000)->f1; if( _temp1023 == 0){ goto
_LL1001;} else{ _temp1025=* _temp1023; _LL1029: _temp1028=( struct
_tagged_string*) _temp1025.hd; goto _LL1027; _LL1027: _temp1026=( struct Cyc_List_List*)
_temp1025.tl; goto _LL1011;}} else{ goto _LL1001;} _LL1003: goto _LL1005;
_LL1005: goto _LL1007; _LL1007: first= _temp991; rest= 0; goto _LL1001; _LL1009:
_temp1028= _temp1021; _temp1026= _temp1019; goto _LL1011; _LL1011: first=
_temp1028; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp1026,({ struct Cyc_List_List* _temp1030=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1030->hd=( void*) _temp991;
_temp1030->tl= 0; _temp1030;})); goto _LL1001; _LL1001:;}{ struct Cyc_List_List*
ns2= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp1031=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1031->hd=( void*) ns2; _temp1031->tl= ge->availables;
_temp1031;}); Cyc_Tc_tc_decls( te, _temp986, in_externC); ge->availables=( ge->availables)->tl;
goto _LL935;}} _LL957: Cyc_Tc_tc_decls( te, _temp995, 1); goto _LL935; _LL935:;}}
static char _temp1034[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp1034, _temp1034, _temp1034 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1035=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1035->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1036=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1036->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1037=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1037->tag= Cyc_Absyn_Namespace_d_tag; _temp1037->f1= Cyc_Tc_cyc_ns;
_temp1037->f2= ds;( void*) _temp1037;}); _temp1036->loc= 0; _temp1036;});
_temp1035->tl= 0; _temp1035;});} Cyc_Tc_tc_decls( te, ds, 0);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Decl* d){ void* _temp1038=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1048; struct Cyc_List_List* _temp1050; struct Cyc_List_List**
_temp1052; struct _tuple0* _temp1053; struct Cyc_List_List* _temp1055; struct
Cyc_List_List** _temp1057; struct _tagged_string* _temp1058; _LL1040: if(((
struct _enum_struct*) _temp1038)->tag == Cyc_Absyn_Var_d_tag){ _LL1049:
_temp1048=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1038)->f1; goto _LL1041;} else{ goto _LL1042;} _LL1042: if((( struct
_enum_struct*) _temp1038)->tag == Cyc_Absyn_Using_d_tag){ _LL1054: _temp1053=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1038)->f1; goto
_LL1051; _LL1051: _temp1050=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1038)->f2; _temp1052=&(( struct Cyc_Absyn_Using_d_struct*) _temp1038)->f2;
goto _LL1043;} else{ goto _LL1044;} _LL1044: if((( struct _enum_struct*)
_temp1038)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1059: _temp1058=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1038)->f1; goto
_LL1056; _LL1056: _temp1055=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1038)->f2; _temp1057=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp1038)->f2;
goto _LL1045;} else{ goto _LL1046;} _LL1046: goto _LL1047; _LL1041: if(( void*)
_temp1048->sc != Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp1062; struct
_tagged_string* _temp1063; void* _temp1065; struct _tuple0* _temp1060= _temp1048->name;
_temp1062=* _temp1060; _LL1066: _temp1065= _temp1062.f1; goto _LL1064; _LL1064:
_temp1063= _temp1062.f2; goto _LL1061; _LL1061: { struct Cyc_List_List* ns;{
void* _temp1067= _temp1065; struct Cyc_List_List* _temp1075; struct Cyc_List_List*
_temp1077; _LL1069: if( _temp1067 == Cyc_Absyn_Loc_n){ goto _LL1070;} else{ goto
_LL1071;} _LL1071: if(( unsigned int) _temp1067 > 1u?(( struct _enum_struct*)
_temp1067)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1076: _temp1075=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1067)->f1; goto _LL1072;} else{ goto
_LL1073;} _LL1073: if(( unsigned int) _temp1067 > 1u?(( struct _enum_struct*)
_temp1067)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1078: _temp1077=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1067)->f1; goto _LL1074;} else{ goto
_LL1068;} _LL1070: ns= 0; goto _LL1068; _LL1072: ns= _temp1075; goto _LL1068;
_LL1074: ns= _temp1077; goto _LL1068; _LL1068:;}{ struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, ns); return(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, _temp1063)).f2;}}}
_LL1043: _temp1057= _temp1052; goto _LL1045; _LL1045:* _temp1057= Cyc_Tc_treeshake(
te,* _temp1057); return 1; _LL1047: return 1; _LL1039:;} struct Cyc_List_List*
Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ return((
struct Cyc_List_List*(*)( int(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Decl*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed,
te, ds);}