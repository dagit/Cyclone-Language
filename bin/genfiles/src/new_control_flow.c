#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd(
struct _tuple0*); extern int Cyc_Core_intcmp( int, int); extern char Cyc_Core_InvalidArg_tag[
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern void* Cyc_Assert_AssertFail(
struct _tagged_string msg); struct Cyc_Lineno_Pos{ struct _tagged_string
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
; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); typedef void* Cyc_CfFlowInfo_LocalRoot; typedef void* Cyc_CfFlowInfo_Field;
struct Cyc_CfFlowInfo_Place; typedef void* Cyc_CfFlowInfo_Escaped; typedef void*
Cyc_CfFlowInfo_InitLevel; typedef void* Cyc_CfFlowInfo_InitState; typedef void*
Cyc_CfFlowInfo_PathInfo; typedef void* Cyc_CfFlowInfo_FlowInfo; typedef void*
Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t; typedef
struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; extern const int Cyc_CfFlowInfo_VarRoot_tag;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_CfFlowInfo_MallocPt_tag; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_CfFlowInfo_StructF_tag;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
extern const int Cyc_CfFlowInfo_TupleF_tag; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; extern void* Cyc_CfFlowInfo_Esc; extern void* Cyc_CfFlowInfo_Unesc;
extern void* Cyc_CfFlowInfo_NoneIL; extern void* Cyc_CfFlowInfo_ThisIL; extern
void* Cyc_CfFlowInfo_AllIL; extern const int Cyc_CfFlowInfo_UnknownIS_tag;
struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ; extern
const int Cyc_CfFlowInfo_MustPointTo_tag; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; extern const int Cyc_CfFlowInfo_LeafPI_tag;
struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; extern const int Cyc_CfFlowInfo_TuplePI_tag;
struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern const int Cyc_CfFlowInfo_StructPI_tag; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_BottomFL;
extern const int Cyc_CfFlowInfo_InitsFL_tag; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct
Cyc_CfAbsexp_BadAbsexp_struct{ char* tag; } ; typedef void* Cyc_CfAbsexp_Absexp;
typedef void* Cyc_CfAbsexp_Absexpgroup; extern void* Cyc_CfAbsexp_OrderedG;
extern void* Cyc_CfAbsexp_UnorderedG; extern void* Cyc_CfAbsexp_OneofG; typedef
void* Cyc_CfAbsexp_Absop; typedef void* Cyc_CfAbsexp_absexp_t; typedef void* Cyc_CfAbsexp_absexpgroup_t;
typedef void* Cyc_CfAbsexp_absop_t; extern void* Cyc_CfAbsexp_mkAnyOrderG();
extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp(
void* ao); extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd);
extern void* Cyc_CfAbsexp_mkMemberOp( void* ao, void* f); extern void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern
void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE(); extern
void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; typedef struct Cyc_Absyn_Vardecl* Cyc_NewControlFlow_vardecl_t;
typedef struct Cyc_Absyn_Stmt* Cyc_NewControlFlow_stmt_t; typedef void* Cyc_NewControlFlow_typ_t;
struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt* encloser; int
visited; void* flow; struct Cyc_List_List* absexps; } ; typedef struct Cyc_NewControlFlow_CFStmtAnnot
Cyc_NewControlFlow_cf_stmt_annot_t; char Cyc_NewControlFlow_CFAnnot_tag[ 8u]="CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{ char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1; } ; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){ enclosee->annot=({ struct Cyc_NewControlFlow_CFAnnot_struct*
_temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_CFAnnot_struct)); _temp0->tag= Cyc_NewControlFlow_CFAnnot_tag;
_temp0->f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp1; _temp1.encloser=
encloser; _temp1.visited= 0; _temp1.flow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1.absexps= 0; _temp1;});( struct _xenum_struct*) _temp0;});} static struct
Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt*
s){ struct _xenum_struct* _temp2= s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp8; struct Cyc_NewControlFlow_CFStmtAnnot* _temp10; _LL4: if((*(( struct
_xenum_struct*) _temp2)).tag == Cyc_NewControlFlow_CFAnnot_tag){ _LL9: _temp8=((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp2)->f1; _temp10=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: goto _LL7; _LL5: return _temp10;
_LL7:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp11=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp11->tag= Cyc_Core_Impossible_tag;
_temp11->f1=( struct _tagged_string)({ char* _temp12=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12;
_temp13.last_plus_one= _temp12 + 37; _temp13;});( struct _xenum_struct*) _temp11;}));
_LL3:;} void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps= ael;}
struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict* all_roots; struct
Cyc_List_List* comprehension_vars; } ; struct Cyc_NewControlFlow_AbsEnv{ struct
Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides; struct
Cyc_List_List* access_path; } ; typedef struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_absenv_t;
struct Cyc_NewControlFlow_AbsSynOne{ void* inner_exp; void* assigns; } ; typedef
struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_abssyn_one_t; struct Cyc_NewControlFlow_AbsSyn{
struct Cyc_NewControlFlow_AbsSynOne when_true; struct Cyc_NewControlFlow_AbsSynOne
when_false; struct Cyc_List_List* lvalues; } ; typedef struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abssyn_t; void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp14->tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp14->f1= vd;( void*) _temp14;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp15=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp15->tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp15->f1= e;( void*) _temp15;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
void Cyc_NewControlFlow_add_enum_root( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Enumfield* ef){ struct Cyc_Absyn_Enumfield
_temp18; struct Cyc_Position_Segment* _temp19; struct Cyc_List_List* _temp21;
struct Cyc_List_List* _temp23; struct Cyc_Absyn_Exp* _temp25; struct _tuple1*
_temp27; struct Cyc_Absyn_Enumfield* _temp16= ef; _temp18=* _temp16; _LL28:
_temp27=( struct _tuple1*) _temp18.name; goto _LL26; _LL26: _temp25=( struct Cyc_Absyn_Exp*)
_temp18.tag; goto _LL24; _LL24: _temp23=( struct Cyc_List_List*) _temp18.tvs;
goto _LL22; _LL22: _temp21=( struct Cyc_List_List*) _temp18.typs; goto _LL20;
_LL20: _temp19=( struct Cyc_Position_Segment*) _temp18.loc; goto _LL17; _LL17: {
struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( int, int)))
Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0; _temp21 != 0; _temp21=
_temp21->tl, ++ i){ struct _tuple4 _temp31; void* _temp32; struct Cyc_Absyn_Tqual*
_temp34; struct _tuple4* _temp29=( struct _tuple4*) _temp21->hd; _temp31=*
_temp29; _LL35: _temp34= _temp31.f1; goto _LL33; _LL33: _temp32= _temp31.f2;
goto _LL30; _LL30: d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int
key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp32));}}( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp36=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp36->tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp36->f1= e;( void*) _temp36;}),({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp37=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp37->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp37->f1= d;( void*) _temp37;}));}}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp38; _temp38.shared= src.shared; _temp38.lhsides= new_lhs; _temp38.access_path=
src.access_path; _temp38;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
== 0){ Cyc_NewControlFlow_unknown_list=({ struct Cyc_List_List* _temp39=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp39->hd=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp39->tl= 0; _temp39;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv ans= Cyc_NewControlFlow_change_lhs( src,
Cyc_NewControlFlow_use_it_list()); ans.access_path= 0; return ans;} struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn syn){ struct
Cyc_List_List* _temp42; struct Cyc_NewControlFlow_AbsSynOne _temp44; void*
_temp46; void* _temp48; struct Cyc_NewControlFlow_AbsSynOne _temp50; void*
_temp52; void* _temp54; struct Cyc_NewControlFlow_AbsSyn _temp40= syn; _LL51:
_temp50=( struct Cyc_NewControlFlow_AbsSynOne) _temp40.when_true; _LL55: _temp54=(
void*) _temp50.inner_exp; goto _LL53; _LL53: _temp52=( void*) _temp50.assigns;
goto _LL45; _LL45: _temp44=( struct Cyc_NewControlFlow_AbsSynOne) _temp40.when_false;
_LL49: _temp48=( void*) _temp44.inner_exp; goto _LL47; _LL47: _temp46=( void*)
_temp44.assigns; goto _LL43; _LL43: _temp42=( struct Cyc_List_List*) _temp40.lvalues;
goto _LL41; _LL41: { void* inn= _temp54 == _temp48? _temp54: Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OneofG, _temp54, _temp48); void* ass= _temp52 == _temp46? _temp52:
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, _temp52, _temp46); return({ struct
Cyc_NewControlFlow_AbsSynOne _temp56; _temp56.inner_exp=( void*) inn; _temp56.assigns=(
void*) ass; _temp56;});}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_mkSyn_tf(
void* ae, struct Cyc_List_List* lvals, void* assigns){ struct Cyc_NewControlFlow_AbsSynOne
one_way=({ struct Cyc_NewControlFlow_AbsSynOne _temp57; _temp57.inner_exp=( void*)
ae; _temp57.assigns=( void*) assigns; _temp57;}); return({ struct Cyc_NewControlFlow_AbsSyn
_temp58; _temp58.when_true= one_way; _temp58.when_false= one_way; _temp58.lvalues=
lvals; _temp58;});} void* Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv
env, void* ae){ if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(( env.lhsides)->tl
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)( env.lhsides)->hd, ae);}{ struct
Cyc_List_List* l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0;
lhsides != 0; lhsides= lhsides->tl){ l=({ struct Cyc_List_List* _temp59=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp59->hd=( void*)
Cyc_CfAbsexp_mkAssignAE(( void*) lhsides->hd, ae); _temp59->tl= l; _temp59;});}}
return Cyc_CfAbsexp_mkGroupAE_l( Cyc_CfAbsexp_OneofG, l);}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_use_exp_unordered(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* es, struct Cyc_List_List*
lhs){ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* uses_exp= Cyc_CfAbsexp_mkSkipAE();
for( 0; es != 0; es= es->tl){ struct Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*) es->hd); void* _temp62; void* _temp64; struct
Cyc_NewControlFlow_AbsSynOne _temp60= Cyc_NewControlFlow_meet_absexp( syn);
_LL65: _temp64=( void*) _temp60.inner_exp; goto _LL63; _LL63: _temp62=( void*)
_temp60.assigns; goto _LL61; _LL61: es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
_temp64, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp62, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* es_exp,
void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE(
e), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e))));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp66=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp66->hd=( void*) path_e; _temp66->tl=
env.access_path; _temp66;});{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path= path->tl){ void* _temp67=( void*)(( struct Cyc_Absyn_Exp*) path->hd)->r;
struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp* _temp83; struct
_tagged_string* _temp85; struct Cyc_Absyn_Exp* _temp87; struct _tagged_string*
_temp89; struct Cyc_Absyn_Exp* _temp91; struct Cyc_Absyn_Exp* _temp93; struct
Cyc_Absyn_Exp* _temp95; _LL69: if((( struct _enum_struct*) _temp67)->tag == Cyc_Absyn_Address_e_tag){
_LL82: _temp81=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp67)->f1; goto _LL70;} else{ goto _LL71;} _LL71: if((( struct _enum_struct*)
_temp67)->tag == Cyc_Absyn_Deref_e_tag){ _LL84: _temp83=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp67)->f1; goto _LL72;} else{ goto _LL73;}
_LL73: if((( struct _enum_struct*) _temp67)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL88: _temp87=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp67)->f1; goto _LL86; _LL86: _temp85=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp67)->f2; goto _LL74;} else{ goto _LL75;} _LL75: if((( struct _enum_struct*)
_temp67)->tag == Cyc_Absyn_StructMember_e_tag){ _LL92: _temp91=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp67)->f1; goto _LL90; _LL90:
_temp89=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp67)->f2; goto _LL76;} else{ goto _LL77;} _LL77: if((( struct _enum_struct*)
_temp67)->tag == Cyc_Absyn_Subscript_e_tag){ _LL96: _temp95=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp67)->f1; goto _LL94; _LL94: _temp93=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp67)->f2;
goto _LL78;} else{ goto _LL79;} _LL79: goto _LL80; _LL70: path->tl == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp97=( char*)"new_control_flow.cyc:243 path->tl == null";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 42; _temp98;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL72: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL68; _LL74: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp89= _temp85; goto _LL76; _LL76: absop= Cyc_CfAbsexp_mkMemberOp(
absop,({ struct Cyc_CfFlowInfo_StructF_struct* _temp99=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp99->tag= Cyc_CfFlowInfo_StructF_tag;
_temp99->f1= _temp89;( void*) _temp99;})); goto _LL68; _LL78: { unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp93); absop= Cyc_CfAbsexp_mkMemberOp( absop,({
struct Cyc_CfFlowInfo_TupleF_struct* _temp100=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp100->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp100->f1=( int) i;( void*) _temp100;})); goto
_LL68;} _LL80:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp101=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp101->tag= Cyc_Core_Impossible_tag; _temp101->f1=( struct _tagged_string)({
char* _temp102=( char*)"abstract_exp: unexpected acces path element"; struct
_tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 44; _temp103;});( struct _xenum_struct*)
_temp101;})); _LL68:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp104=(
void*) e->r; struct Cyc_Absyn_Exp* _temp192; struct Cyc_Absyn_Exp* _temp194;
struct Cyc_List_List* _temp196; struct Cyc_Absyn_Exp* _temp198; void* _temp200;
int _temp202; void* _temp204; void* _temp206; int _temp208; void* _temp210; void*
_temp212; void* _temp214; void* _temp216; struct Cyc_Absyn_Fndecl* _temp218;
struct _tuple1* _temp220; void* _temp222; struct Cyc_Absyn_Vardecl* _temp224;
struct _tuple1* _temp226; void* _temp228; struct Cyc_Absyn_Vardecl* _temp230;
struct _tuple1* _temp232; void* _temp234; struct Cyc_Absyn_Vardecl* _temp236;
struct _tuple1* _temp238; void* _temp240; struct Cyc_Absyn_Vardecl* _temp242;
struct _tuple1* _temp244; struct Cyc_Absyn_Enumfield* _temp246; struct Cyc_Absyn_Enumdecl*
_temp248; struct Cyc_List_List* _temp250; struct Cyc_Core_Opt* _temp252; struct
Cyc_Core_Opt* _temp254; struct Cyc_Absyn_Enumfield* _temp256; struct Cyc_Absyn_Xenumdecl*
_temp258; struct Cyc_List_List* _temp260; struct Cyc_Core_Opt* _temp262; void*
_temp264; void* _temp266; void* _temp268; struct Cyc_Absyn_Exp* _temp270; struct
Cyc_Core_Opt* _temp272; struct Cyc_Absyn_Exp* _temp274; struct Cyc_Absyn_Exp*
_temp276; struct Cyc_Core_Opt* _temp278; struct Cyc_Absyn_Exp* _temp280; struct
Cyc_List_List* _temp282; void* _temp284; struct Cyc_Absyn_Exp* _temp286; void*
_temp288; struct Cyc_List_List* _temp290; struct Cyc_Absyn_Exp* _temp292; struct
Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Exp* _temp296; struct Cyc_Absyn_Exp*
_temp298; struct Cyc_Absyn_Exp* _temp300; struct Cyc_Absyn_Exp* _temp302; struct
_tagged_string* _temp304; struct Cyc_Absyn_Exp* _temp306; struct _tagged_string*
_temp308; struct Cyc_Absyn_Exp* _temp310; struct Cyc_List_List* _temp312; struct
Cyc_Absyn_Structdecl* _temp314; struct Cyc_List_List* _temp316; struct Cyc_Core_Opt*
_temp318; struct _tuple1* _temp320; struct Cyc_List_List* _temp322; void*
_temp324; struct Cyc_Absyn_Exp* _temp326; struct Cyc_Absyn_Exp* _temp328; struct
Cyc_Absyn_Exp* _temp330; struct Cyc_Absyn_Exp* _temp332; struct Cyc_Absyn_Exp*
_temp334; struct Cyc_Absyn_Stmt* _temp336; struct Cyc_Absyn_Exp* _temp338;
struct Cyc_Absyn_Exp* _temp340; struct Cyc_Absyn_Exp* _temp342; struct Cyc_Absyn_Exp*
_temp344; struct Cyc_Absyn_Vardecl* _temp346; void* _temp348; struct _tuple1*
_temp350; struct _tuple1* _temp352; struct Cyc_List_List* _temp354; struct Cyc_Absyn_Exp*
_temp356; struct Cyc_List_List* _temp358; struct Cyc_Core_Opt* _temp360; struct
Cyc_List_List* _temp362; struct _tuple2* _temp364; struct Cyc_Absyn_Fndecl*
_temp366; struct Cyc_Absyn_Exp* _temp368; _LL106: if((( struct _enum_struct*)
_temp104)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL193: _temp192=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp104)->f1; goto _LL107;} else{
goto _LL108;} _LL108: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL195: _temp194=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp104)->f1; goto _LL109;} else{ goto _LL110;} _LL110: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL199: _temp198=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp104)->f1;
goto _LL197; _LL197: _temp196=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp104)->f2; goto _LL111;} else{ goto _LL112;} _LL112: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Const_e_tag){ _LL201: _temp200=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp104)->f1; if(( unsigned int) _temp200 > 1u?((
struct _enum_struct*) _temp200)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL205:
_temp204=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp200)->f1; if( _temp204
== Cyc_Absyn_Signed){ goto _LL203;} else{ goto _LL114;} _LL203: _temp202=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp200)->f2; if( _temp202 == 0){ goto _LL113;}
else{ goto _LL114;}} else{ goto _LL114;}} else{ goto _LL114;} _LL114: if(((
struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Const_e_tag){ _LL207: _temp206=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp104)->f1; if(( unsigned int)
_temp206 > 1u?(( struct _enum_struct*) _temp206)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL211: _temp210=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp206)->f1; if(
_temp210 == Cyc_Absyn_Signed){ goto _LL209;} else{ goto _LL116;} _LL209:
_temp208=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp206)->f2; if( _temp208 ==
1){ goto _LL115;} else{ goto _LL116;}} else{ goto _LL116;}} else{ goto _LL116;}
_LL116: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL213: _temp212=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp104)->f1;
goto _LL117;} else{ goto _LL118;} _LL118: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Const_e_tag){ _LL215: _temp214=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp104)->f1; goto _LL119;} else{ goto _LL120;} _LL120: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Var_e_tag){ _LL221: _temp220=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp104)->f1; goto _LL217; _LL217:
_temp216=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp104)->f2; if((
unsigned int) _temp216 > 1u?(( struct _enum_struct*) _temp216)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL219: _temp218=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp216)->f1; goto _LL121;} else{ goto _LL122;}} else{ goto _LL122;} _LL122:
if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Var_e_tag){ _LL227:
_temp226=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp104)->f1;
goto _LL223; _LL223: _temp222=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f2; if(( unsigned int) _temp222 > 1u?(( struct _enum_struct*)
_temp222)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL225: _temp224=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp222)->f1; goto _LL123;} else{ goto
_LL124;}} else{ goto _LL124;} _LL124: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Var_e_tag){ _LL233: _temp232=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f1; goto _LL229; _LL229: _temp228=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f2; if(( unsigned int) _temp228 > 1u?(( struct _enum_struct*)
_temp228)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL231: _temp230=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp228)->f1; goto _LL125;} else{ goto _LL126;}}
else{ goto _LL126;} _LL126: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Var_e_tag){
_LL239: _temp238=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp104)->f1;
goto _LL235; _LL235: _temp234=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f2; if(( unsigned int) _temp234 > 1u?(( struct _enum_struct*)
_temp234)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL237: _temp236=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp234)->f1; goto _LL127;} else{ goto _LL128;}}
else{ goto _LL128;} _LL128: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Var_e_tag){
_LL245: _temp244=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp104)->f1;
goto _LL241; _LL241: _temp240=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f2; if(( unsigned int) _temp240 > 1u?(( struct _enum_struct*)
_temp240)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL243: _temp242=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp240)->f1; goto _LL129;} else{ goto _LL130;}}
else{ goto _LL130;} _LL130: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Enum_e_tag){
_LL255: _temp254=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp104)->f1; goto _LL253; _LL253: _temp252=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp104)->f2; goto _LL251; _LL251: _temp250=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp104)->f3; goto _LL249;
_LL249: _temp248=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp104)->f4; goto _LL247; _LL247: _temp246=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp104)->f5; goto _LL131;} else{ goto _LL132;}
_LL132: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Xenum_e_tag){
_LL263: _temp262=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp104)->f1; goto _LL261; _LL261: _temp260=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp104)->f2; goto _LL259; _LL259: _temp258=( struct
Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp104)->f3; goto
_LL257; _LL257: _temp256=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp104)->f4; goto _LL133;} else{ goto _LL134;} _LL134: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Malloc_e_tag){ _LL265: _temp264=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp104)->f1; if((( struct
_enum_struct*) _temp264)->tag == Cyc_Absyn_Typ_m_tag){ _LL267: _temp266=( void*)((
struct Cyc_Absyn_Typ_m_struct*) _temp264)->f1; goto _LL135;} else{ goto _LL136;}}
else{ goto _LL136;} _LL136: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Malloc_e_tag){
_LL269: _temp268=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp104)->f1;
goto _LL137;} else{ goto _LL138;} _LL138: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL275: _temp274=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp104)->f1; goto _LL273; _LL273: _temp272=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp104)->f2; if(
_temp272 == 0){ goto _LL271;} else{ goto _LL140;} _LL271: _temp270=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp104)->f3; goto _LL139;} else{ goto
_LL140;} _LL140: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL281: _temp280=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp104)->f1; goto _LL279; _LL279: _temp278=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp104)->f2; goto _LL277; _LL277: _temp276=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp104)->f3;
goto _LL141;} else{ goto _LL142;} _LL142: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Primop_e_tag){ _LL285: _temp284=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp104)->f1; goto _LL283; _LL283: _temp282=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp104)->f2; goto _LL143;} else{ goto _LL144;}
_LL144: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Cast_e_tag){
_LL289: _temp288=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp104)->f1; goto
_LL287; _LL287: _temp286=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp104)->f2; goto _LL145;} else{ goto _LL146;} _LL146: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_FnCall_e_tag){ _LL293: _temp292=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp104)->f1; goto
_LL291; _LL291: _temp290=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp104)->f2; goto _LL147;} else{ goto _LL148;} _LL148: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Subscript_e_tag){ _LL297: _temp296=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp104)->f1;
goto _LL295; _LL295: _temp294=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp104)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_New_e_tag){ _LL299: _temp298=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp104)->f1; goto _LL151;}
else{ goto _LL152;} _LL152: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Address_e_tag){
_LL301: _temp300=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp104)->f1; goto _LL153;} else{ goto _LL154;} _LL154: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Deref_e_tag){ _LL303: _temp302=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp104)->f1; goto
_LL155;} else{ goto _LL156;} _LL156: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL307: _temp306=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp104)->f1; goto _LL305; _LL305:
_temp304=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp104)->f2; goto _LL157;} else{ goto _LL158;} _LL158: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_StructMember_e_tag){ _LL311: _temp310=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp104)->f1;
goto _LL309; _LL309: _temp308=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp104)->f2; goto _LL159;} else{ goto _LL160;} _LL160: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Tuple_e_tag){ _LL313: _temp312=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp104)->f1; goto
_LL161;} else{ goto _LL162;} _LL162: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Struct_e_tag){ _LL321: _temp320=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp104)->f1; goto _LL319; _LL319: _temp318=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp104)->f2; goto _LL317; _LL317: _temp316=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp104)->f3; goto
_LL315; _LL315: _temp314=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp104)->f4; goto _LL163;} else{ goto _LL164;} _LL164: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Array_e_tag){ _LL323: _temp322=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp104)->f1; goto
_LL165;} else{ goto _LL166;} _LL166: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Increment_e_tag){ _LL327: _temp326=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp104)->f1; goto _LL325; _LL325:
_temp324=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp104)->f2; goto
_LL167;} else{ goto _LL168;} _LL168: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Throw_e_tag){ _LL329: _temp328=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Throw_e_struct*) _temp104)->f1; goto _LL169;} else{ goto _LL170;}
_LL170: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Conditional_e_tag){
_LL335: _temp334=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp104)->f1; goto _LL333; _LL333: _temp332=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp104)->f2; goto _LL331; _LL331: _temp330=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp104)->f3;
goto _LL171;} else{ goto _LL172;} _LL172: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL337: _temp336=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_StmtExp_e_struct*) _temp104)->f1; goto _LL173;} else{ goto _LL174;}
_LL174: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL341: _temp340=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp104)->f1; goto _LL339; _LL339: _temp338=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp104)->f2; goto _LL175;} else{ goto _LL176;}
_LL176: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL347: _temp346=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp104)->f1; goto _LL345; _LL345: _temp344=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp104)->f2; goto _LL343; _LL343: _temp342=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp104)->f3;
goto _LL177;} else{ goto _LL178;} _LL178: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_Var_e_tag){ _LL351: _temp350=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f1; goto _LL349; _LL349: _temp348=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp104)->f2; if( _temp348 == Cyc_Absyn_Unresolved_b){ goto _LL179;} else{ goto
_LL180;}} else{ goto _LL180;} _LL180: if((( struct _enum_struct*) _temp104)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL353: _temp352=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp104)->f1; goto _LL181;} else{ goto _LL182;} _LL182: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL357: _temp356=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp104)->f1;
goto _LL355; _LL355: _temp354=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp104)->f2; goto _LL183;} else{ goto _LL184;} _LL184: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL361:
_temp360=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp104)->f1; goto _LL359; _LL359: _temp358=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp104)->f2; goto _LL185;} else{ goto
_LL186;} _LL186: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL365: _temp364=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp104)->f1; goto _LL363; _LL363: _temp362=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp104)->f2; goto _LL187;} else{ goto _LL188;}
_LL188: if((( struct _enum_struct*) _temp104)->tag == Cyc_Absyn_Codegen_e_tag){
_LL367: _temp366=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp104)->f1; goto _LL189;} else{ goto _LL190;} _LL190: if((( struct
_enum_struct*) _temp104)->tag == Cyc_Absyn_Fill_e_tag){ _LL369: _temp368=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp104)->f1; goto
_LL191;} else{ goto _LL105;} _LL107: _temp194= _temp192; goto _LL109; _LL109:
_temp198= _temp194; goto _LL111; _LL111: return Cyc_NewControlFlow_abstract_exp(
env, _temp198); _LL113: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp370=( char*)"new_control_flow.cyc:266 env.access_path==null";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 47; _temp371;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp372; _temp372.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp374;
_temp374.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp374.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp374;}); _temp372.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp373; _temp373.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp373.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp373;}); _temp372.lvalues= 0; _temp372;}); _LL115: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp375=( char*)"new_control_flow.cyc:271 env.access_path==null";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 47; _temp376;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp377; _temp377.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp379;
_temp379.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp379.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp379;});
_temp377.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp378; _temp378.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp378.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp378;}); _temp377.lvalues= 0; _temp377;}); _LL117: goto _LL119; _LL119: goto
_LL121; _LL121: goto _LL123; return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL123:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL125:
_temp236= _temp230; goto _LL127; _LL127: _temp242= _temp236; goto _LL129; _LL129: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp242)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp380=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp380->hd=( void*) absop; _temp380->tl= 0; _temp380;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL131: _temp260= _temp250;
_temp256= _temp246; goto _LL133; _LL133: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp381=( char*)"new_control_flow.cyc:301 env.access_path == null";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 49; _temp382;})); if( _temp260 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_enum_root( env, e,
_temp256);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp260 != 0; _temp260= _temp260->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp383=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp383->hd=(
void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp( e),({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp384=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp384->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp384->f1= i;( void*) _temp384;})); _temp383->tl= 0; _temp383;})); struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct Cyc_Absyn_Exp*) _temp260->hd);
void* _temp387; void* _temp389; struct Cyc_NewControlFlow_AbsSynOne _temp385=
Cyc_NewControlFlow_meet_absexp( syn); _LL390: _temp389=( void*) _temp385.inner_exp;
goto _LL388; _LL388: _temp387=( void*) _temp385.assigns; goto _LL386; _LL386:
es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp389, es_exp);
useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp387, useexp);}}
return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp, useexp);} _LL135: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp266); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE(
e), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL137:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp391=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp391->tag= Cyc_Core_Impossible_tag; _temp391->f1=( struct _tagged_string)({
char* _temp392=( char*)"NewControlFlow: Malloc_e of unsupported variant"; struct
_tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 48; _temp393;});( struct _xenum_struct*)
_temp391;})); _LL139: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp274); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp270); void* _temp396; void*
_temp398; struct Cyc_NewControlFlow_AbsSynOne _temp394= Cyc_NewControlFlow_meet_absexp(
syn1); _LL399: _temp398=( void*) _temp394.inner_exp; goto _LL397; _LL397:
_temp396=( void*) _temp394.assigns; goto _LL395; _LL395: { void* _temp402; void*
_temp404; struct Cyc_NewControlFlow_AbsSynOne _temp400= Cyc_NewControlFlow_meet_absexp(
syn2); _LL405: _temp404=( void*) _temp400.inner_exp; goto _LL403; _LL403:
_temp402=( void*) _temp400.assigns; goto _LL401; _LL401: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp398,
_temp404), _temp402); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp396);}}}
_LL141: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp406=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp406->hd=( void*) _temp280; _temp406->tl=({ struct Cyc_List_List* _temp407=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp407->hd=(
void*) _temp276; _temp407->tl= 0; _temp407;}); _temp406;}), 0); _LL143: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp282, 0); _LL145: { int both_leaf=
1;{ void* _temp408=( void*)( _temp286->topt)->v; struct Cyc_Absyn_Structdecl**
_temp416; struct Cyc_List_List* _temp418; struct _tuple1* _temp420; struct Cyc_List_List*
_temp422; _LL410: if(( unsigned int) _temp408 > 4u?(( struct _enum_struct*)
_temp408)->tag == Cyc_Absyn_StructType_tag: 0){ _LL421: _temp420=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp408)->f1; goto _LL419;
_LL419: _temp418=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp408)->f2; goto _LL417; _LL417: _temp416=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp408)->f3; goto _LL411;} else{ goto
_LL412;} _LL412: if(( unsigned int) _temp408 > 4u?(( struct _enum_struct*)
_temp408)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL423: _temp422=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp408)->f1; goto _LL413;} else{ goto
_LL414;} _LL414: goto _LL415; _LL411: goto _LL413; _LL413: both_leaf= 0; goto
_LL409; _LL415: goto _LL409; _LL409:;}{ void* _temp424=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp432; struct Cyc_List_List* _temp434; struct
_tuple1* _temp436; struct Cyc_List_List* _temp438; _LL426: if(( unsigned int)
_temp424 > 4u?(( struct _enum_struct*) _temp424)->tag == Cyc_Absyn_StructType_tag:
0){ _LL437: _temp436=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp424)->f1; goto _LL435; _LL435: _temp434=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp424)->f2; goto _LL433; _LL433: _temp432=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp424)->f3;
goto _LL427;} else{ goto _LL428;} _LL428: if(( unsigned int) _temp424 > 4u?((
struct _enum_struct*) _temp424)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL439:
_temp438=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp424)->f1;
goto _LL429;} else{ goto _LL430;} _LL430: goto _LL431; _LL427: goto _LL429;
_LL429: both_leaf= 0; goto _LL425; _LL431: goto _LL425; _LL425:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp286);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp440=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp440->hd=( void*) _temp286; _temp440->tl= 0;
_temp440;}), env.lhsides);} _LL147: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp441=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp441->hd=( void*) _temp292; _temp441->tl=
_temp290; _temp441;}), 0); _LL149: { void* _temp442= Cyc_Tcutil_compress(( void*)(
_temp296->topt)->v); struct Cyc_List_List* _temp448; _LL444: if(( unsigned int)
_temp442 > 4u?(( struct _enum_struct*) _temp442)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL449: _temp448=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp442)->f1; goto _LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL445:
return Cyc_NewControlFlow_descend_path( env, e, _temp296); _LL447: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp450=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp450->hd=( void*) _temp296; _temp450->tl=({
struct Cyc_List_List* _temp451=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp451->hd=( void*) _temp294; _temp451->tl= 0;
_temp451;}); _temp450;}), Cyc_NewControlFlow_use_it_list()); _LL443:;} _LL151: {
void* _temp452=( void*) _temp298->r; struct Cyc_Absyn_Structdecl* _temp460;
struct Cyc_List_List* _temp462; struct Cyc_Core_Opt* _temp464; struct _tuple1*
_temp466; struct Cyc_List_List* _temp468; _LL454: if((( struct _enum_struct*)
_temp452)->tag == Cyc_Absyn_Struct_e_tag){ _LL467: _temp466=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp452)->f1; goto _LL465; _LL465: _temp464=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp452)->f2; goto
_LL463; _LL463: _temp462=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp452)->f3; goto _LL461; _LL461: _temp460=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp452)->f4; goto _LL455;} else{ goto
_LL456;} _LL456: if((( struct _enum_struct*) _temp452)->tag == Cyc_Absyn_Tuple_e_tag){
_LL469: _temp468=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp452)->f1; goto _LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL455:
goto _LL457; _LL457: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)( _temp298->topt)->v);{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp470=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp470->hd=(
void*) malloc_op; _temp470->tl= 0; _temp470;})); inner_env.access_path= 0;{ void*
_temp473; void* _temp475; struct Cyc_NewControlFlow_AbsSynOne _temp471= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp298)); _LL476: _temp475=( void*)
_temp471.inner_exp; goto _LL474; _LL474: _temp473=( void*) _temp471.assigns;
goto _LL472; _LL472: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp475, _temp473); if( env.access_path != 0){ void* _temp477=( void*)((
struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r; struct Cyc_Absyn_Exp* _temp487;
struct Cyc_Absyn_Exp* _temp489; struct Cyc_Absyn_Exp* _temp491; struct
_tagged_string* _temp493; struct Cyc_Absyn_Exp* _temp495; _LL479: if((( struct
_enum_struct*) _temp477)->tag == Cyc_Absyn_Subscript_e_tag){ _LL490: _temp489=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp477)->f1;
goto _LL488; _LL488: _temp487=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp477)->f2; goto _LL480;} else{ goto _LL481;} _LL481: if((( struct
_enum_struct*) _temp477)->tag == Cyc_Absyn_Deref_e_tag){ _LL492: _temp491=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp477)->f1; goto
_LL482;} else{ goto _LL483;} _LL483: if((( struct _enum_struct*) _temp477)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL496: _temp495=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp477)->f1; goto _LL494; _LL494:
_temp493=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp477)->f2; goto _LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL480:
Cyc_Evexp_eval_const_uint_exp( _temp487) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp497=( char*)"new_control_flow.cyc:414 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 61; _temp498;})); goto _LL482; _LL482: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp499=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp499->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp499->tl= 0; _temp499;}); goto _LL478; _LL484: env.access_path=({
struct Cyc_List_List* _temp500=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp500->hd=( void*) Cyc_Absyn_structmember_exp(
_temp495, _temp493, 0); _temp500->tl=( env.access_path)->tl; _temp500;}); ans.lvalues=({
struct Cyc_List_List* _temp501=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp501->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp501->tl= 0; _temp501;}); goto _LL478; _LL486:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp502=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp502->tag= Cyc_Core_Impossible_tag;
_temp502->f1=( struct _tagged_string)({ char* _temp503=( char*)"bad malloc access path";
struct _tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 23; _temp504;});( struct _xenum_struct*)
_temp502;})); _LL478:;} return ans;}}}} _LL459: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp505=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp505->hd=( void*) _temp298; _temp505->tl= 0;
_temp505;}), 0); _LL453:;} _LL153:{ void* _temp506=( void*) _temp300->r; struct
Cyc_Absyn_Structdecl* _temp514; struct Cyc_List_List* _temp516; struct Cyc_Core_Opt*
_temp518; struct _tuple1* _temp520; struct Cyc_List_List* _temp522; _LL508: if(((
struct _enum_struct*) _temp506)->tag == Cyc_Absyn_Struct_e_tag){ _LL521:
_temp520=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp506)->f1;
goto _LL519; _LL519: _temp518=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp506)->f2; goto _LL517; _LL517: _temp516=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp506)->f3; goto _LL515; _LL515: _temp514=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp506)->f4;
goto _LL509;} else{ goto _LL510;} _LL510: if((( struct _enum_struct*) _temp506)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL523: _temp522=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp506)->f1; goto _LL511;} else{ goto _LL512;}
_LL512: goto _LL513; _LL509: goto _LL511; _LL511: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp300->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp524=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp524->hd=( void*) malloc_op; _temp524->tl= 0;
_temp524;})); inner_env.access_path= 0;{ void* _temp527; void* _temp529; struct
Cyc_NewControlFlow_AbsSynOne _temp525= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp300)); _LL530: _temp529=( void*) _temp525.inner_exp; goto _LL528;
_LL528: _temp527=( void*) _temp525.assigns; goto _LL526; _LL526: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp529, _temp527); if( env.access_path
!= 0){ void* _temp531=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp541; struct Cyc_Absyn_Exp* _temp543; struct Cyc_Absyn_Exp*
_temp545; struct _tagged_string* _temp547; struct Cyc_Absyn_Exp* _temp549;
_LL533: if((( struct _enum_struct*) _temp531)->tag == Cyc_Absyn_Subscript_e_tag){
_LL544: _temp543=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp531)->f1; goto _LL542; _LL542: _temp541=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp531)->f2; goto _LL534;} else{ goto _LL535;}
_LL535: if((( struct _enum_struct*) _temp531)->tag == Cyc_Absyn_Deref_e_tag){
_LL546: _temp545=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp531)->f1; goto _LL536;} else{ goto _LL537;} _LL537: if((( struct
_enum_struct*) _temp531)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL550: _temp549=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp531)->f1;
goto _LL548; _LL548: _temp547=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp531)->f2; goto _LL538;} else{ goto _LL539;} _LL539: goto _LL540; _LL534:
Cyc_Evexp_eval_const_uint_exp( _temp541) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp551=( char*)"new_control_flow.cyc:450 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 61; _temp552;})); goto _LL536; _LL536: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp553=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp553->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp553->tl= 0; _temp553;}); goto _LL532; _LL538: env.access_path=({
struct Cyc_List_List* _temp554=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp554->hd=( void*) Cyc_Absyn_structmember_exp(
_temp549, _temp547, 0); _temp554->tl=( env.access_path)->tl; _temp554;}); ans.lvalues=({
struct Cyc_List_List* _temp555=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp555->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp555->tl= 0; _temp555;}); goto _LL532; _LL540:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp556=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp556->tag= Cyc_Core_Impossible_tag;
_temp556->f1=( struct _tagged_string)({ char* _temp557=( char*)"bad malloc access path";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 23; _temp558;});( struct _xenum_struct*)
_temp556;})); _LL532:;} return ans;}}}} _LL513: goto _LL507; _LL507:;}{ struct
Cyc_List_List* _temp559= env.access_path; struct Cyc_List_List _temp571; struct
Cyc_List_List* _temp572; struct Cyc_Absyn_Exp* _temp574; struct Cyc_Absyn_Exp
_temp576; struct Cyc_Position_Segment* _temp577; void* _temp579; struct Cyc_Absyn_Exp*
_temp581; struct Cyc_Absyn_Exp* _temp583; struct Cyc_Core_Opt* _temp585; struct
Cyc_List_List _temp587; struct Cyc_List_List* _temp588; struct Cyc_Absyn_Exp*
_temp590; struct Cyc_Absyn_Exp _temp592; struct Cyc_Position_Segment* _temp593;
void* _temp595; struct Cyc_Absyn_Exp* _temp597; struct Cyc_Core_Opt* _temp599;
struct Cyc_List_List _temp601; struct Cyc_List_List* _temp602; struct Cyc_Absyn_Exp*
_temp604; struct Cyc_Absyn_Exp _temp606; struct Cyc_Position_Segment* _temp607;
void* _temp609; struct _tagged_string* _temp611; struct Cyc_Absyn_Exp* _temp613;
struct Cyc_Core_Opt* _temp615; _LL561: if( _temp559 == 0){ goto _LL562;} else{
goto _LL563;} _LL563: if( _temp559 == 0){ goto _LL565;} else{ _temp571=*
_temp559; _LL575: _temp574=( struct Cyc_Absyn_Exp*) _temp571.hd; _temp576=*
_temp574; _LL586: _temp585=( struct Cyc_Core_Opt*) _temp576.topt; goto _LL580;
_LL580: _temp579=( void*) _temp576.r; if((( struct _enum_struct*) _temp579)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL584: _temp583=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp579)->f1; goto _LL582; _LL582:
_temp581=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp579)->f2; goto _LL578;} else{ goto _LL565;} _LL578: _temp577=( struct Cyc_Position_Segment*)
_temp576.loc; goto _LL573; _LL573: _temp572=( struct Cyc_List_List*) _temp571.tl;
goto _LL564;} _LL565: if( _temp559 == 0){ goto _LL567;} else{ _temp587=*
_temp559; _LL591: _temp590=( struct Cyc_Absyn_Exp*) _temp587.hd; _temp592=*
_temp590; _LL600: _temp599=( struct Cyc_Core_Opt*) _temp592.topt; goto _LL596;
_LL596: _temp595=( void*) _temp592.r; if((( struct _enum_struct*) _temp595)->tag
== Cyc_Absyn_Deref_e_tag){ _LL598: _temp597=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp595)->f1; goto _LL594;} else{ goto _LL567;}
_LL594: _temp593=( struct Cyc_Position_Segment*) _temp592.loc; goto _LL589;
_LL589: _temp588=( struct Cyc_List_List*) _temp587.tl; goto _LL566;} _LL567: if(
_temp559 == 0){ goto _LL569;} else{ _temp601=* _temp559; _LL605: _temp604=(
struct Cyc_Absyn_Exp*) _temp601.hd; _temp606=* _temp604; _LL616: _temp615=(
struct Cyc_Core_Opt*) _temp606.topt; goto _LL610; _LL610: _temp609=( void*)
_temp606.r; if((( struct _enum_struct*) _temp609)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL614: _temp613=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp609)->f1; goto _LL612; _LL612: _temp611=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp609)->f2; goto _LL608;} else{ goto _LL569;}
_LL608: _temp607=( struct Cyc_Position_Segment*) _temp606.loc; goto _LL603;
_LL603: _temp602=( struct Cyc_List_List*) _temp601.tl; goto _LL568;} _LL569:
goto _LL570; _LL562: env.access_path=({ struct Cyc_List_List* _temp617=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp617->hd=( void*)
e; _temp617->tl= env.access_path; _temp617;}); goto _LL560; _LL564: Cyc_Evexp_eval_const_uint_exp(
_temp581) ==( unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp618=( char*)"new_control_flow.cyc:478 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 61; _temp619;})); _temp588= _temp572; goto
_LL566; _LL566: env.access_path= _temp588; goto _LL560; _LL568: env.access_path=({
struct Cyc_List_List* _temp620=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp620->hd=( void*) Cyc_Absyn_structmember_exp(
_temp613, _temp611, 0); _temp620->tl= _temp602; _temp620;}); goto _LL560; _LL570:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp621=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp621->tag= Cyc_Core_Impossible_tag;
_temp621->f1=( struct _tagged_string)({ char* _temp622=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp623; _temp623.curr= _temp622; _temp623.base= _temp622;
_temp623.last_plus_one= _temp622 + 36; _temp623;});( struct _xenum_struct*)
_temp621;})); _LL560:;} return Cyc_NewControlFlow_abstract_exp( env, _temp300);
_LL155: { struct Cyc_List_List* _temp624= env.access_path; struct Cyc_List_List
_temp630; struct Cyc_List_List* _temp631; struct Cyc_Absyn_Exp* _temp633; struct
Cyc_Absyn_Exp _temp635; struct Cyc_Position_Segment* _temp636; void* _temp638;
struct Cyc_Absyn_Exp* _temp640; struct Cyc_Core_Opt* _temp642; _LL626: if(
_temp624 == 0){ goto _LL628;} else{ _temp630=* _temp624; _LL634: _temp633=(
struct Cyc_Absyn_Exp*) _temp630.hd; _temp635=* _temp633; _LL643: _temp642=(
struct Cyc_Core_Opt*) _temp635.topt; goto _LL639; _LL639: _temp638=( void*)
_temp635.r; if((( struct _enum_struct*) _temp638)->tag == Cyc_Absyn_Address_e_tag){
_LL641: _temp640=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp638)->f1; goto _LL637;} else{ goto _LL628;} _LL637: _temp636=( struct Cyc_Position_Segment*)
_temp635.loc; goto _LL632; _LL632: _temp631=( struct Cyc_List_List*) _temp630.tl;
goto _LL627;} _LL628: goto _LL629; _LL627: env.access_path= _temp631;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp302);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL629: return Cyc_NewControlFlow_descend_path( env, e, _temp302); _LL625:;}
_LL157: { void* _temp644= Cyc_Tcutil_compress(( void*)( _temp306->topt)->v);
struct Cyc_Absyn_PtrInfo _temp650; struct Cyc_Absyn_Conref* _temp652; struct Cyc_Absyn_Tqual*
_temp654; struct Cyc_Absyn_Conref* _temp656; void* _temp658; void* _temp660;
_LL646: if(( unsigned int) _temp644 > 4u?(( struct _enum_struct*) _temp644)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL651: _temp650=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp644)->f1; _LL661: _temp660=( void*)
_temp650.elt_typ; goto _LL659; _LL659: _temp658=( void*) _temp650.rgn_typ; goto
_LL657; _LL657: _temp656=( struct Cyc_Absyn_Conref*) _temp650.nullable; goto
_LL655; _LL655: _temp654=( struct Cyc_Absyn_Tqual*) _temp650.tq; goto _LL653;
_LL653: _temp652=( struct Cyc_Absyn_Conref*) _temp650.bounds; goto _LL647;}
else{ goto _LL648;} _LL648: goto _LL649; _LL647:{ void* _temp662= Cyc_Tcutil_compress(
_temp660); struct Cyc_Absyn_Structdecl** _temp670; struct Cyc_List_List*
_temp672; struct _tuple1* _temp674; struct Cyc_Absyn_Uniondecl** _temp676;
struct Cyc_List_List* _temp678; struct _tuple1* _temp680; _LL664: if((
unsigned int) _temp662 > 4u?(( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_StructType_tag:
0){ _LL675: _temp674=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp662)->f1; goto _LL673; _LL673: _temp672=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp662)->f2; goto _LL671; _LL671: _temp670=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp662)->f3;
goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int) _temp662 > 4u?((
struct _enum_struct*) _temp662)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL681:
_temp680=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp662)->f1;
goto _LL679; _LL679: _temp678=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp662)->f2; goto _LL677; _LL677: _temp676=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp662)->f3; goto _LL667;} else{ goto
_LL668;} _LL668: goto _LL669; _LL665: return Cyc_NewControlFlow_descend_path(
env, e, _temp306); _LL667: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp306); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL669: goto _LL663; _LL663:;} goto _LL649; _LL649:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp682=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp682->tag= Cyc_Core_Impossible_tag; _temp682->f1=( struct _tagged_string)({
char* _temp683=( char*)"NewControlFlow: bad type in StructArrow"; struct
_tagged_string _temp684; _temp684.curr= _temp683; _temp684.base= _temp683;
_temp684.last_plus_one= _temp683 + 40; _temp684;});( struct _xenum_struct*)
_temp682;})); _LL645:;} _LL159: { void* _temp685= Cyc_Tcutil_compress(( void*)(
_temp310->topt)->v); struct Cyc_Absyn_Structdecl** _temp693; struct Cyc_List_List*
_temp695; struct _tuple1* _temp697; struct Cyc_Absyn_Uniondecl** _temp699;
struct Cyc_List_List* _temp701; struct _tuple1* _temp703; _LL687: if((
unsigned int) _temp685 > 4u?(( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_StructType_tag:
0){ _LL698: _temp697=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp685)->f1; goto _LL696; _LL696: _temp695=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp685)->f2; goto _LL694; _LL694: _temp693=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp685)->f3;
goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int) _temp685 > 4u?((
struct _enum_struct*) _temp685)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL704:
_temp703=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp685)->f1;
goto _LL702; _LL702: _temp701=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp685)->f2; goto _LL700; _LL700: _temp699=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp685)->f3; goto _LL690;} else{ goto
_LL691;} _LL691: goto _LL692; _LL688: return Cyc_NewControlFlow_descend_path(
env, e, _temp310); _LL690: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp310); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL692:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp705=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp705->tag= Cyc_Core_Impossible_tag; _temp705->f1=( struct _tagged_string)({
char* _temp706=( char*)"NewControlFlow: bad type in StructMember"; struct
_tagged_string _temp707; _temp707.curr= _temp706; _temp707.base= _temp706;
_temp707.last_plus_one= _temp706 + 41; _temp707;});( struct _xenum_struct*)
_temp705;})); _LL686:;} _LL161: { struct Cyc_List_List* _temp708= env.access_path;
struct Cyc_List_List _temp716; struct Cyc_List_List* _temp717; struct Cyc_Absyn_Exp*
_temp719; struct Cyc_Absyn_Exp _temp721; struct Cyc_Position_Segment* _temp722;
void* _temp724; struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Exp* _temp728;
struct Cyc_Core_Opt* _temp730; _LL710: if( _temp708 == 0){ goto _LL711;} else{
goto _LL712;} _LL712: if( _temp708 == 0){ goto _LL714;} else{ _temp716=*
_temp708; _LL720: _temp719=( struct Cyc_Absyn_Exp*) _temp716.hd; _temp721=*
_temp719; _LL731: _temp730=( struct Cyc_Core_Opt*) _temp721.topt; goto _LL725;
_LL725: _temp724=( void*) _temp721.r; if((( struct _enum_struct*) _temp724)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL729: _temp728=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp724)->f1; goto _LL727; _LL727:
_temp726=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp724)->f2; goto _LL723;} else{ goto _LL714;} _LL723: _temp722=( struct Cyc_Position_Segment*)
_temp721.loc; goto _LL718; _LL718: _temp717=( struct Cyc_List_List*) _temp716.tl;
goto _LL713;} _LL714: goto _LL715; _LL711: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp312 != 0;
_temp312= _temp312->tl, ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=({ struct Cyc_List_List* _temp732=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp732->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp732->tl= ith_lvals; _temp732;});} else{ ith_lvals=({ struct Cyc_List_List*
_temp733=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp733->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp734=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp734->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp734->f1= j;( void*) _temp734;})); _temp733->tl= ith_lvals; _temp733;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp312->hd); void* _temp737; void*
_temp739; struct Cyc_NewControlFlow_AbsSynOne _temp735= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL740: _temp739=( void*) _temp735.inner_exp; goto _LL738; _LL738:
_temp737=( void*) _temp735.assigns; goto _LL736; _LL736: ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, ansexp, _temp739); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp737);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL713: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp726); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp717;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp312 != 0;
_temp312= _temp312->tl, ++ j){ if( i == j){ void* _temp743; void* _temp745;
struct Cyc_NewControlFlow_AbsSynOne _temp741= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*) _temp312->hd));
_LL746: _temp745=( void*) _temp741.inner_exp; goto _LL744; _LL744: _temp743=(
void*) _temp741.assigns; goto _LL742; _LL742: assignexp= _temp743; ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, ansexp, _temp745);} else{ void* _temp749; void*
_temp751; struct Cyc_NewControlFlow_AbsSynOne _temp747= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*) _temp312->hd));
_LL752: _temp751=( void*) _temp747.inner_exp; goto _LL750; _LL750: _temp749=(
void*) _temp747.assigns; goto _LL748; _LL748: ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, ansexp, _temp751);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp753->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp753->tl= 0; _temp753;}), assignexp);}} _LL715:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp754=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp754->tag= Cyc_Core_Impossible_tag; _temp754->f1=( struct _tagged_string)({
char* _temp755=( char*)"abstract_exp: unexpected access path to Tuple"; struct
_tagged_string _temp756; _temp756.curr= _temp755; _temp756.base= _temp755;
_temp756.last_plus_one= _temp755 + 46; _temp756;});( struct _xenum_struct*)
_temp754;})); _LL709:;} _LL163: { struct Cyc_List_List* _temp757= env.access_path;
struct Cyc_List_List _temp765; struct Cyc_List_List* _temp766; struct Cyc_Absyn_Exp*
_temp768; struct Cyc_Absyn_Exp _temp770; struct Cyc_Position_Segment* _temp771;
void* _temp773; struct _tagged_string* _temp775; struct Cyc_Absyn_Exp* _temp777;
struct Cyc_Core_Opt* _temp779; _LL759: if( _temp757 == 0){ goto _LL760;} else{
goto _LL761;} _LL761: if( _temp757 == 0){ goto _LL763;} else{ _temp765=*
_temp757; _LL769: _temp768=( struct Cyc_Absyn_Exp*) _temp765.hd; _temp770=*
_temp768; _LL780: _temp779=( struct Cyc_Core_Opt*) _temp770.topt; goto _LL774;
_LL774: _temp773=( void*) _temp770.r; if((( struct _enum_struct*) _temp773)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL778: _temp777=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp773)->f1; goto _LL776; _LL776:
_temp775=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp773)->f2; goto _LL772;} else{ goto _LL763;} _LL772: _temp771=( struct Cyc_Position_Segment*)
_temp770.loc; goto _LL767; _LL767: _temp766=( struct Cyc_List_List*) _temp765.tl;
goto _LL762;} _LL763: goto _LL764; _LL760: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp316 != 0; _temp316=
_temp316->tl){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls=
env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)
ls->hd)){ ith_lvals=({ struct Cyc_List_List* _temp781=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp781->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp781->tl= ith_lvals; _temp781;});} else{ struct Cyc_List_List* ds=(*((
struct _tuple5*) _temp316->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp782=(
void*) ds->hd; struct Cyc_Absyn_Exp* _temp788; struct _tagged_string* _temp790;
_LL784: if((( struct _enum_struct*) _temp782)->tag == Cyc_Absyn_ArrayElement_tag){
_LL789: _temp788=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp782)->f1; goto _LL785;} else{ goto _LL786;} _LL786: if((( struct
_enum_struct*) _temp782)->tag == Cyc_Absyn_FieldName_tag){ _LL791: _temp790=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp782)->f1;
goto _LL787;} else{ goto _LL783;} _LL785:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp792=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp792->tag= Cyc_Core_Impossible_tag; _temp792->f1=( struct _tagged_string)({
char* _temp793=( char*)"bad struct designator"; struct _tagged_string _temp794;
_temp794.curr= _temp793; _temp794.base= _temp793; _temp794.last_plus_one=
_temp793 + 22; _temp794;});( struct _xenum_struct*) _temp792;})); _LL787:
ith_lvals=({ struct Cyc_List_List* _temp795=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp795->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,({ struct Cyc_CfFlowInfo_StructF_struct* _temp796=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp796->tag= Cyc_CfFlowInfo_StructF_tag;
_temp796->f1= _temp790;( void*) _temp796;})); _temp795->tl= ith_lvals; _temp795;});
goto _LL783; _LL783:;}}}}{ void** ith_topt=( void*)&(((*(( struct _tuple5*)
_temp316->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*(( struct _tuple5*) _temp316->hd)).f2);
void* _temp799; void* _temp801; struct Cyc_NewControlFlow_AbsSynOne _temp797=
Cyc_NewControlFlow_meet_absexp( ith_syn); _LL802: _temp801=( void*) _temp797.inner_exp;
goto _LL800; _LL800: _temp799=( void*) _temp797.assigns; goto _LL798; _LL798:
ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, ansexp, _temp801);
assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), assignexp,
_temp799);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);} _LL762: {
struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs( env,
0); other_env.access_path= 0; env.access_path= _temp766;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp316 != 0; _temp316=
_temp316->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp316->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp803=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp809; struct _tagged_string* _temp811; _LL805: if(((
struct _enum_struct*) _temp803)->tag == Cyc_Absyn_ArrayElement_tag){ _LL810:
_temp809=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp803)->f1; goto _LL806;} else{ goto _LL807;} _LL807: if((( struct
_enum_struct*) _temp803)->tag == Cyc_Absyn_FieldName_tag){ _LL812: _temp811=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp803)->f1;
goto _LL808;} else{ goto _LL804;} _LL806:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp813=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp813->tag= Cyc_Core_Impossible_tag; _temp813->f1=( struct _tagged_string)({
char* _temp814=( char*)"bad struct designator"; struct _tagged_string _temp815;
_temp815.curr= _temp814; _temp815.base= _temp814; _temp815.last_plus_one=
_temp814 + 22; _temp815;});( struct _xenum_struct*) _temp813;})); _LL808: if(
Cyc_String_zstrptrcmp( _temp775, _temp811) == 0){ used= 1;} goto _LL804; _LL804:;}}
if( used){ void* _temp818; void* _temp820; struct Cyc_NewControlFlow_AbsSynOne
_temp816= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*) _temp316->hd)).f2)); _LL821: _temp820=( void*) _temp816.inner_exp;
goto _LL819; _LL819: _temp818=( void*) _temp816.assigns; goto _LL817; _LL817:
assignexp= _temp818; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp820);} else{ void* _temp824; void* _temp826; struct Cyc_NewControlFlow_AbsSynOne
_temp822= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp316->hd)).f2)); _LL827: _temp826=( void*)
_temp822.inner_exp; goto _LL825; _LL825: _temp824=( void*) _temp822.assigns;
goto _LL823; _LL823: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp826);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL764:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp828=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp828->tag= Cyc_Core_Impossible_tag; _temp828->f1=( struct _tagged_string)({
char* _temp829=( char*)"abstract_exp: unexpected access path to Struct"; struct
_tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 47; _temp830;});( struct _xenum_struct*)
_temp828;})); _LL758:;} _LL165: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp322), 0); _LL167: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp831=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp831->hd=( void*) _temp326; _temp831->tl= 0;
_temp831;}), 0); _LL169: { struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp328); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
esyn)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL171: { struct Cyc_List_List* _temp834; struct Cyc_NewControlFlow_AbsSynOne
_temp836; struct Cyc_NewControlFlow_AbsSynOne _temp838; struct Cyc_NewControlFlow_AbsSyn
_temp832= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp334); _LL839: _temp838=( struct Cyc_NewControlFlow_AbsSynOne) _temp832.when_true;
goto _LL837; _LL837: _temp836=( struct Cyc_NewControlFlow_AbsSynOne) _temp832.when_false;
goto _LL835; _LL835: _temp834=( struct Cyc_List_List*) _temp832.lvalues; goto
_LL833; _LL833: { struct Cyc_List_List* _temp842; struct Cyc_NewControlFlow_AbsSynOne
_temp844; struct Cyc_NewControlFlow_AbsSynOne _temp846; struct Cyc_NewControlFlow_AbsSyn
_temp840= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp332); _LL847: _temp846=( struct Cyc_NewControlFlow_AbsSynOne) _temp840.when_true;
goto _LL845; _LL845: _temp844=( struct Cyc_NewControlFlow_AbsSynOne) _temp840.when_false;
goto _LL843; _LL843: _temp842=( struct Cyc_List_List*) _temp840.lvalues; goto
_LL841; _LL841: { struct Cyc_List_List* _temp850; struct Cyc_NewControlFlow_AbsSynOne
_temp852; struct Cyc_NewControlFlow_AbsSynOne _temp854; struct Cyc_NewControlFlow_AbsSyn
_temp848= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp330); _LL855: _temp854=( struct Cyc_NewControlFlow_AbsSynOne) _temp848.when_true;
goto _LL853; _LL853: _temp852=( struct Cyc_NewControlFlow_AbsSynOne) _temp848.when_false;
goto _LL851; _LL851: _temp850=( struct Cyc_List_List*) _temp848.lvalues; goto
_LL849; _LL849: { void* true_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp838.inner_exp,( void*) _temp838.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp846.inner_exp,( void*) _temp846.assigns)),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp836.inner_exp,( void*) _temp836.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp854.inner_exp,( void*) _temp854.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp838.inner_exp,( void*) _temp838.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp844.inner_exp,( void*) _temp844.assigns)), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp836.inner_exp,( void*) _temp836.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp852.inner_exp,( void*) _temp852.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp856; _temp856.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp858;
_temp858.inner_exp=( void*) true_exp; _temp858.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp858;}); _temp856.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp857; _temp857.inner_exp=( void*) false_exp; _temp857.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp857;}); _temp856.lvalues= 0; _temp856;});}}}}
_LL173: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp336);{
struct Cyc_Absyn_Stmt* last_s= _temp336; while( 1) { void* _temp859=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp869; struct Cyc_Absyn_Stmt* _temp871;
struct Cyc_Absyn_Stmt* _temp873; struct Cyc_Absyn_Decl* _temp875; struct Cyc_Absyn_Exp*
_temp877; _LL861: if(( unsigned int) _temp859 > 1u?(( struct _enum_struct*)
_temp859)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL872: _temp871=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp859)->f1; goto _LL870; _LL870: _temp869=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp859)->f2; goto
_LL862;} else{ goto _LL863;} _LL863: if(( unsigned int) _temp859 > 1u?(( struct
_enum_struct*) _temp859)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL876: _temp875=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp859)->f1; goto
_LL874; _LL874: _temp873=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp859)->f2; goto _LL864;} else{ goto _LL865;} _LL865: if(( unsigned int)
_temp859 > 1u?(( struct _enum_struct*) _temp859)->tag == Cyc_Absyn_Exp_s_tag: 0){
_LL878: _temp877=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp859)->f1; goto _LL866;} else{ goto _LL867;} _LL867: goto _LL868; _LL862:
last_s= _temp869; continue; _LL864: last_s= _temp873; continue; _LL866: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp877); void* _temp881; void* _temp883; struct Cyc_NewControlFlow_AbsSynOne
_temp879= Cyc_NewControlFlow_meet_absexp( syn); _LL884: _temp883=( void*)
_temp879.inner_exp; goto _LL882; _LL882: _temp881=( void*) _temp879.assigns;
goto _LL880; _LL880:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp883, _temp881));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp336), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL868:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp885=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp885->tag= Cyc_Core_Impossible_tag; _temp885->f1=( struct _tagged_string)({
char* _temp886=( char*)"abstract_exp: ill-formed StmtExp"; struct _tagged_string
_temp887; _temp887.curr= _temp886; _temp887.base= _temp886; _temp887.last_plus_one=
_temp886 + 33; _temp887;});( struct _xenum_struct*) _temp885;})); _LL860:;}}}
_LL175: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp340); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp338); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp888; _temp888.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp890; _temp890.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp890.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp890;});
_temp888.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp889; _temp889.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp889.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp889;}); _temp888.lvalues= 0; _temp888;});}} _LL177: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp891=( char*)"new_control_flow.cyc:745 env.access_path==null";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 47; _temp892;})); Cyc_NewControlFlow_add_var_root(
env, _temp346);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp893=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp893->hd=( void*) _temp346; _temp893->tl=( env.shared)->comprehension_vars;
_temp893;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp896; void* _temp898; struct Cyc_NewControlFlow_AbsSynOne
_temp894= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp344)); _LL899: _temp898=( void*) _temp894.inner_exp; goto _LL897;
_LL897: _temp896=( void*) _temp894.assigns; goto _LL895; _LL895: { void*
_temp902; void* _temp904; struct Cyc_NewControlFlow_AbsSynOne _temp900= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp342)); _LL905: _temp904=( void*)
_temp900.inner_exp; goto _LL903; _LL903: _temp902=( void*) _temp900.assigns;
goto _LL901; _LL901: { void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp898, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp896, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp346),
Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, _temp904, _temp902), Cyc_CfAbsexp_mkSkipAE())))); return
Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()));}}} _LL179: goto _LL181; _LL181: goto _LL183;
_LL183: goto _LL185; _LL185: goto _LL187; _LL187: goto _LL189; _LL189: goto
_LL191; _LL191:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp906=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp906->tag= Cyc_Core_Impossible_tag; _temp906->f1=( struct _tagged_string)({
char* _temp907=( char*)"abstract_exp, unexpected exp form"; struct
_tagged_string _temp908; _temp908.curr= _temp907; _temp908.base= _temp907;
_temp908.last_plus_one= _temp907 + 34; _temp908;});( struct _xenum_struct*)
_temp906;})); _LL105:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp911; void* _temp913; struct Cyc_NewControlFlow_AbsSynOne
_temp909= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL914: _temp913=( void*) _temp909.inner_exp; goto _LL912; _LL912: _temp911=(
void*) _temp909.assigns; goto _LL910; _LL910: return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp913, _temp911);} struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp915; _temp915.f1= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns); _temp915.f2=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp915;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp918;
struct Cyc_Position_Segment* _temp919; struct Cyc_Absyn_Stmt* _temp921; struct
Cyc_Absyn_Exp* _temp923; struct Cyc_Core_Opt* _temp925; struct Cyc_Absyn_Pat*
_temp927; struct Cyc_Absyn_Switch_clause* _temp916=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp918=* _temp916; _LL928: _temp927=( struct Cyc_Absyn_Pat*) _temp918.pattern;
goto _LL926; _LL926: _temp925=( struct Cyc_Core_Opt*) _temp918.pat_vars; goto
_LL924; _LL924: _temp923=( struct Cyc_Absyn_Exp*) _temp918.where_clause; goto
_LL922; _LL922: _temp921=( struct Cyc_Absyn_Stmt*) _temp918.body; goto _LL920;
_LL920: _temp919=( struct Cyc_Position_Segment*) _temp918.loc; goto _LL917;
_LL917: if( _temp925 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp929=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp929->tag= Cyc_Core_Impossible_tag; _temp929->f1=( struct _tagged_string)({
char* _temp930=( char*)"switch clause vds not set"; struct _tagged_string
_temp931; _temp931.curr= _temp930; _temp931.base= _temp930; _temp931.last_plus_one=
_temp930 + 26; _temp931;});( struct _xenum_struct*) _temp929;}));}{ struct Cyc_List_List*
vds=( struct Cyc_List_List*) _temp925->v; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp923 != 0){ void* _temp935;
void* _temp937; struct _tuple0 _temp933= Cyc_NewControlFlow_abstract_guard( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp932= _temp923; if( _temp932
== 0){ _throw( Null_Exception);} _temp932;})); _LL938: _temp937= _temp933.f1;
goto _LL936; _LL936: _temp935= _temp933.f2; goto _LL934; _LL934: ans=({ struct
Cyc_List_List* _temp939=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp939->hd=( void*) _temp937; _temp939->tl=({ struct Cyc_List_List* _temp940=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp940->hd=(
void*) _temp935; _temp940->tl= ans; _temp940;}); _temp939;});} Cyc_NewControlFlow_cf_prepass(
env, _temp921);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp941=( char*)"new_control_flow.cyc:797 env.lhsides == null"; struct
_tagged_string _temp942; _temp942.curr= _temp941; _temp942.base= _temp941;
_temp942.last_plus_one= _temp941 + 45; _temp942;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp943=( char*)"new_control_flow.cyc:798 env.access_path == null";
struct _tagged_string _temp944; _temp944.curr= _temp943; _temp944.base= _temp943;
_temp944.last_plus_one= _temp943 + 49; _temp944;}));{ void* _temp945=( void*) s->r;
struct Cyc_Absyn_Exp* _temp991; struct Cyc_Absyn_Stmt* _temp993; struct Cyc_Absyn_Stmt*
_temp995; struct Cyc_Absyn_Exp* _temp997; struct Cyc_Absyn_Exp* _temp999; struct
Cyc_Absyn_Stmt* _temp1001; struct Cyc_Absyn_Stmt* _temp1003; struct Cyc_Absyn_Exp*
_temp1005; struct _tuple3 _temp1007; struct Cyc_Absyn_Stmt* _temp1009; struct
Cyc_Absyn_Exp* _temp1011; struct Cyc_Absyn_Stmt* _temp1013; struct Cyc_Absyn_Stmt*
_temp1015; struct _tuple3 _temp1017; struct Cyc_Absyn_Stmt* _temp1019; struct
Cyc_Absyn_Exp* _temp1021; struct Cyc_Absyn_Stmt* _temp1023; struct Cyc_Absyn_Stmt*
_temp1025; struct Cyc_Absyn_Stmt* _temp1027; struct _tagged_string* _temp1029;
struct Cyc_Absyn_Stmt* _temp1031; struct _tagged_string* _temp1033; struct Cyc_Absyn_Stmt*
_temp1035; struct _tuple3 _temp1037; struct Cyc_Absyn_Stmt* _temp1039; struct
Cyc_Absyn_Exp* _temp1041; struct _tuple3 _temp1043; struct Cyc_Absyn_Stmt*
_temp1045; struct Cyc_Absyn_Exp* _temp1047; struct Cyc_Absyn_Exp* _temp1049;
struct Cyc_List_List* _temp1051; struct Cyc_Absyn_Exp* _temp1053; struct Cyc_Absyn_Switch_clause**
_temp1055; struct Cyc_List_List* _temp1057; struct Cyc_Absyn_Stmt* _temp1059;
struct Cyc_Absyn_Decl* _temp1061; struct Cyc_Absyn_Decl _temp1063; struct Cyc_Position_Segment*
_temp1064; void* _temp1066; struct Cyc_Absyn_Vardecl* _temp1068; struct Cyc_Absyn_Stmt*
_temp1070; struct Cyc_Absyn_Decl* _temp1072; struct Cyc_Absyn_Decl _temp1074;
struct Cyc_Position_Segment* _temp1075; void* _temp1077; int _temp1079; struct
Cyc_Absyn_Exp* _temp1081; struct Cyc_Core_Opt* _temp1083; struct Cyc_Core_Opt*
_temp1085; struct Cyc_Core_Opt _temp1087; struct Cyc_List_List* _temp1088;
struct Cyc_Absyn_Pat* _temp1090; struct Cyc_Absyn_Stmt* _temp1092; struct Cyc_Absyn_Decl*
_temp1094; struct Cyc_Absyn_Stmt* _temp1096; struct Cyc_Absyn_Stmt* _temp1098;
struct Cyc_Absyn_Stmt* _temp1100; struct _tagged_string* _temp1102; struct Cyc_List_List*
_temp1104; struct Cyc_Absyn_Stmt* _temp1106; _LL947: if( _temp945 == Cyc_Absyn_Skip_s){
goto _LL948;} else{ goto _LL949;} _LL949: if(( unsigned int) _temp945 > 1u?((
struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL992:
_temp991=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp945)->f1;
goto _LL950;} else{ goto _LL951;} _LL951: if(( unsigned int) _temp945 > 1u?((
struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL996:
_temp995=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp945)->f1;
goto _LL994; _LL994: _temp993=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp945)->f2; goto _LL952;} else{ goto _LL953;} _LL953: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL998: _temp997=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp945)->f1; if( _temp997 == 0){ goto _LL954;} else{ goto _LL955;}} else{ goto
_LL955;} _LL955: if(( unsigned int) _temp945 > 1u?(( struct _enum_struct*)
_temp945)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1000: _temp999=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp945)->f1; goto _LL956;} else{ goto
_LL957;} _LL957: if(( unsigned int) _temp945 > 1u?(( struct _enum_struct*)
_temp945)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1006: _temp1005=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp945)->f1; goto
_LL1004; _LL1004: _temp1003=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp945)->f2; goto _LL1002; _LL1002: _temp1001=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp945)->f3; goto _LL958;} else{ goto
_LL959;} _LL959: if(( unsigned int) _temp945 > 1u?(( struct _enum_struct*)
_temp945)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1014: _temp1013=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp945)->f1; goto _LL1008; _LL1008: _temp1007=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp945)->f2; _LL1012:
_temp1011= _temp1007.f1; goto _LL1010; _LL1010: _temp1009= _temp1007.f2; goto
_LL960;} else{ goto _LL961;} _LL961: if(( unsigned int) _temp945 > 1u?(( struct
_enum_struct*) _temp945)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1018: _temp1017=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp945)->f1; _LL1022:
_temp1021= _temp1017.f1; goto _LL1020; _LL1020: _temp1019= _temp1017.f2; goto
_LL1016; _LL1016: _temp1015=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp945)->f2; goto _LL962;} else{ goto _LL963;} _LL963: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL1024: _temp1023=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp945)->f1; goto _LL964;} else{ goto _LL965;} _LL965: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1026: _temp1025=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp945)->f1; goto _LL966;} else{ goto _LL967;} _LL967: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Goto_s_tag: 0){
_LL1030: _temp1029=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp945)->f1; goto _LL1028; _LL1028: _temp1027=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp945)->f2; if( _temp1027 == 0){ goto _LL968;}
else{ goto _LL969;}} else{ goto _LL969;} _LL969: if(( unsigned int) _temp945 > 1u?((
struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1034:
_temp1033=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp945)->f1;
goto _LL1032; _LL1032: _temp1031=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp945)->f2; goto _LL970;} else{ goto _LL971;} _LL971: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_For_s_tag: 0){
_LL1050: _temp1049=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp945)->f1; goto _LL1044; _LL1044: _temp1043=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp945)->f2; _LL1048: _temp1047= _temp1043.f1; goto _LL1046; _LL1046:
_temp1045= _temp1043.f2; goto _LL1038; _LL1038: _temp1037=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp945)->f3; _LL1042: _temp1041= _temp1037.f1;
goto _LL1040; _LL1040: _temp1039= _temp1037.f2; goto _LL1036; _LL1036: _temp1035=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp945)->f4; goto
_LL972;} else{ goto _LL973;} _LL973: if(( unsigned int) _temp945 > 1u?(( struct
_enum_struct*) _temp945)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1054: _temp1053=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp945)->f1; goto
_LL1052; _LL1052: _temp1051=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp945)->f2; goto _LL974;} else{ goto _LL975;} _LL975: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL1058: _temp1057=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp945)->f1; goto _LL1056; _LL1056: _temp1055=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp945)->f2; goto _LL976;} else{ goto
_LL977;} _LL977: if(( unsigned int) _temp945 > 1u?(( struct _enum_struct*)
_temp945)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1062: _temp1061=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp945)->f1; _temp1063=* _temp1061; _LL1067:
_temp1066=( void*) _temp1063.r; if((( struct _enum_struct*) _temp1066)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1069: _temp1068=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1066)->f1; goto _LL1065;} else{ goto _LL979;}
_LL1065: _temp1064=( struct Cyc_Position_Segment*) _temp1063.loc; goto _LL1060;
_LL1060: _temp1059=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp945)->f2; goto _LL978;} else{ goto _LL979;} _LL979: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL1073: _temp1072=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp945)->f1; _temp1074=* _temp1072; _LL1078: _temp1077=( void*) _temp1074.r;
if((( struct _enum_struct*) _temp1077)->tag == Cyc_Absyn_Let_d_tag){ _LL1091:
_temp1090=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1077)->f1;
goto _LL1086; _LL1086: _temp1085=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1077)->f2; if( _temp1085 == 0){ goto _LL981;} else{ _temp1087=* _temp1085;
_LL1089: _temp1088=( struct Cyc_List_List*) _temp1087.v; goto _LL1084;} _LL1084:
_temp1083=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1077)->f3;
goto _LL1082; _LL1082: _temp1081=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1077)->f4; goto _LL1080; _LL1080: _temp1079=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1077)->f5; goto _LL1076;} else{ goto _LL981;} _LL1076: _temp1075=( struct
Cyc_Position_Segment*) _temp1074.loc; goto _LL1071; _LL1071: _temp1070=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp945)->f2; goto _LL980;}
else{ goto _LL981;} _LL981: if(( unsigned int) _temp945 > 1u?(( struct
_enum_struct*) _temp945)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1095: _temp1094=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp945)->f1; goto
_LL1093; _LL1093: _temp1092=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp945)->f2; goto _LL982;} else{ goto _LL983;} _LL983: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Cut_s_tag: 0){
_LL1097: _temp1096=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp945)->f1; goto _LL984;} else{ goto _LL985;} _LL985: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1099: _temp1098=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp945)->f1; goto _LL986;} else{ goto _LL987;} _LL987: if(( unsigned int)
_temp945 > 1u?(( struct _enum_struct*) _temp945)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1103: _temp1102=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp945)->f1; goto _LL1101; _LL1101: _temp1100=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp945)->f2; goto _LL988;} else{ goto _LL989;}
_LL989: if(( unsigned int) _temp945 > 1u?(( struct _enum_struct*) _temp945)->tag
== Cyc_Absyn_TryCatch_s_tag: 0){ _LL1107: _temp1106=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp945)->f1; goto _LL1105; _LL1105:
_temp1104=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp945)->f2; goto _LL990;} else{ goto _LL946;} _LL948: return; _LL950: { void*
ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp991); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1108=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1108->hd=( void*) ae; _temp1108->tl= 0; _temp1108;}));
return;} _LL952: Cyc_NewControlFlow_cf_prepass( env, _temp995); Cyc_NewControlFlow_cf_prepass(
env, _temp993); return; _LL954: return; _LL956: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1109= _temp999; if( _temp1109 == 0){ _throw( Null_Exception);} _temp1109;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1110=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1110->hd=( void*)
ae; _temp1110->tl= 0; _temp1110;})); return;} _LL958: { void* _temp1113; void*
_temp1115; struct _tuple0 _temp1111= Cyc_NewControlFlow_abstract_guard( env,
_temp1005); _LL1116: _temp1115= _temp1111.f1; goto _LL1114; _LL1114: _temp1113=
_temp1111.f2; goto _LL1112; _LL1112: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1117=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1117->hd=( void*) _temp1115; _temp1117->tl=({
struct Cyc_List_List* _temp1118=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1118->hd=( void*) _temp1113; _temp1118->tl= 0;
_temp1118;}); _temp1117;})); Cyc_NewControlFlow_cf_prepass( env, _temp1003); Cyc_NewControlFlow_cf_prepass(
env, _temp1001); return;} _LL960: _temp1021= _temp1011; _temp1019= _temp1009;
_temp1015= _temp1013; goto _LL962; _LL962: { void* _temp1121; void* _temp1123;
struct _tuple0 _temp1119= Cyc_NewControlFlow_abstract_guard( env, _temp1021);
_LL1124: _temp1123= _temp1119.f1; goto _LL1122; _LL1122: _temp1121= _temp1119.f2;
goto _LL1120; _LL1120: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1125=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1125->hd=( void*) _temp1123; _temp1125->tl=({ struct Cyc_List_List*
_temp1126=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1126->hd=( void*) _temp1121; _temp1126->tl= 0; _temp1126;}); _temp1125;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1015); return;} _LL964: return; _LL966:
return; _LL968:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1127=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1127->tag= Cyc_Core_Impossible_tag; _temp1127->f1=( struct _tagged_string)({
char* _temp1128=( char*)"cf_prepass: goto w/o destination"; struct
_tagged_string _temp1129; _temp1129.curr= _temp1128; _temp1129.base= _temp1128;
_temp1129.last_plus_one= _temp1128 + 33; _temp1129;});( struct _xenum_struct*)
_temp1127;})); _LL970: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1130= _temp1031; if(
_temp1130 == 0){ _throw( Null_Exception);} _temp1130;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1131=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1132; _temp1132.curr= _temp1131; _temp1132.base=
_temp1131; _temp1132.last_plus_one= _temp1131 + 45; _temp1132;})); break;}
my_encloser= next_encloser;} return;} _LL972: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1049); void* _temp1135; void* _temp1137; struct _tuple0 _temp1133= Cyc_NewControlFlow_abstract_guard(
env, _temp1047); _LL1138: _temp1137= _temp1133.f1; goto _LL1136; _LL1136:
_temp1135= _temp1133.f2; goto _LL1134; _LL1134: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1041); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1139=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1139->hd=( void*) ae1; _temp1139->tl=({ struct Cyc_List_List* _temp1140=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1140->hd=(
void*) _temp1137; _temp1140->tl=({ struct Cyc_List_List* _temp1141=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1141->hd=( void*) _temp1135;
_temp1141->tl=({ struct Cyc_List_List* _temp1142=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1142->hd=( void*) ae3; _temp1142->tl=
0; _temp1142;}); _temp1141;}); _temp1140;}); _temp1139;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1035); return;}} _LL974: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1053); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1051); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1143=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1143->hd=( void*) ae; _temp1143->tl= ael; _temp1143;})); return;} _LL976: {
void* _temp1146; void* _temp1148; struct Cyc_NewControlFlow_AbsSynOne _temp1144=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1057, 0)); _LL1149: _temp1148=( void*) _temp1144.inner_exp; goto _LL1147;
_LL1147: _temp1146=( void*) _temp1144.assigns; goto _LL1145; _LL1145: { void* ae=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp1148, _temp1146); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1150=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1150->hd=( void*) ae; _temp1150->tl= 0; _temp1150;}));
return;}} _LL978: Cyc_NewControlFlow_add_var_root( env, _temp1068); if(
_temp1068->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv init_env= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp1151=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1151->hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp1068); _temp1151->tl= 0; _temp1151;})); void* ae= Cyc_NewControlFlow_abstract_exp_top(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1152= _temp1068->initializer;
if( _temp1152 == 0){ _throw( Null_Exception);} _temp1152;})); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1153=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1153->hd=( void*) ae; _temp1153->tl= 0; _temp1153;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1059); return; _LL980: for( 0;
_temp1088 != 0; _temp1088= _temp1088->tl){ Cyc_NewControlFlow_add_var_root( env,(
struct Cyc_Absyn_Vardecl*) _temp1088->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1081); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1154=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1154->hd=( void*) ae; _temp1154->tl= 0; _temp1154;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1070); return;} _LL982: goto _LL984;
_LL984: goto _LL986; _LL986:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1155=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1155->tag= Cyc_Core_Impossible_tag; _temp1155->f1=( struct _tagged_string)({
char* _temp1156=( char*)"cf_prepass: bad stmt form"; struct _tagged_string
_temp1157; _temp1157.curr= _temp1156; _temp1157.base= _temp1156; _temp1157.last_plus_one=
_temp1156 + 26; _temp1157;});( struct _xenum_struct*) _temp1155;})); _LL988: Cyc_NewControlFlow_cf_prepass(
env, _temp1100); return; _LL990: Cyc_NewControlFlow_cf_prepass( env, _temp1106);{
struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1104);
Cyc_NewControlFlow_cf_set_absexps( s, ael); return;} _LL946:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1158=( char*)"new_control_flow.cyc:926 annot->visited == iteration_num";
struct _tagged_string _temp1159; _temp1159.curr= _temp1158; _temp1159.base=
_temp1158; _temp1159.last_plus_one= _temp1158 + 57; _temp1159;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1160= flow; struct Cyc_Dict_Dict* _temp1166; _LL1162: if( _temp1160
== Cyc_CfFlowInfo_BottomFL){ goto _LL1163;} else{ goto _LL1164;} _LL1164: if((
unsigned int) _temp1160 > 1u?(( struct _enum_struct*) _temp1160)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1167: _temp1166=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1160)->f1; goto _LL1165;} else{ goto _LL1161;} _LL1163: return Cyc_CfFlowInfo_BottomFL;
_LL1165: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1168=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1168->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1168->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1168;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots, root)); _temp1166=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1166, root, pinfo);} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1169=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1169->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1169->f1= _temp1166;( void*) _temp1169;}); _LL1161:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1170= flow; struct Cyc_Dict_Dict* _temp1176; _LL1172: if( _temp1170
== Cyc_CfFlowInfo_BottomFL){ goto _LL1173;} else{ goto _LL1174;} _LL1174: if((
unsigned int) _temp1170 > 1u?(( struct _enum_struct*) _temp1170)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1177: _temp1176=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1170)->f1; goto _LL1175;} else{ goto _LL1171;} _LL1173: return Cyc_CfFlowInfo_BottomFL;
_LL1175: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1178=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1178->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1178->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1178;}); _temp1176=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1176, root,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots, root));} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1179=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1179->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1179->f1= _temp1176;( void*) _temp1179;}); _LL1171:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)
aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1180; _temp1180.f1=
out_flow; _temp1180.f2= out_flow; _temp1180;});} return({ struct _tuple0
_temp1181; _temp1181.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1181.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1181;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1184; struct Cyc_Position_Segment* _temp1185; struct Cyc_Absyn_Stmt*
_temp1187; struct Cyc_Absyn_Exp* _temp1189; struct Cyc_Core_Opt* _temp1191;
struct Cyc_Absyn_Pat* _temp1193; struct Cyc_Absyn_Switch_clause* _temp1182=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1184=* _temp1182; _LL1194:
_temp1193=( struct Cyc_Absyn_Pat*) _temp1184.pattern; goto _LL1192; _LL1192:
_temp1191=( struct Cyc_Core_Opt*) _temp1184.pat_vars; goto _LL1190; _LL1190:
_temp1189=( struct Cyc_Absyn_Exp*) _temp1184.where_clause; goto _LL1188; _LL1188:
_temp1187=( struct Cyc_Absyn_Stmt*) _temp1184.body; goto _LL1186; _LL1186:
_temp1185=( struct Cyc_Position_Segment*) _temp1184.loc; goto _LL1183; _LL1183: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1191->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1191->v);{ void* body_outflow;
if( _temp1189 != 0){ void* _temp1198; void* _temp1200; struct _tuple0 _temp1196=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1195= _temp1189; if( _temp1195 == 0){ _throw( Null_Exception);} _temp1195;}))->loc,
aes, body_inflow); _LL1201: _temp1200= _temp1196.f1; goto _LL1199; _LL1199:
_temp1198= _temp1196.f2; goto _LL1197; _LL1197: aes=( aes->tl)->tl; in_flow=
_temp1198; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1187,
_temp1200);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1187, body_inflow);}{ void* _temp1202= body_outflow; _LL1204: if( _temp1202
== Cyc_CfFlowInfo_BottomFL){ goto _LL1205;} else{ goto _LL1206;} _LL1206: goto
_LL1207; _LL1205: goto _LL1203; _LL1207: Cyc_Tcutil_terr( _temp1187->loc,(
struct _tagged_string)({ char* _temp1208=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1209; _temp1209.curr= _temp1208; _temp1209.base=
_temp1208; _temp1209.last_plus_one= _temp1208 + 38; _temp1209;})); goto _LL1203;
_LL1203:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1210=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1252; struct Cyc_Absyn_Exp* _temp1254; struct Cyc_Absyn_Exp*
_temp1256; struct Cyc_Absyn_Stmt* _temp1258; struct Cyc_Absyn_Stmt* _temp1260;
struct Cyc_Absyn_Stmt* _temp1262; struct Cyc_Absyn_Stmt* _temp1264; struct Cyc_Absyn_Exp*
_temp1266; struct Cyc_Absyn_Stmt* _temp1268; struct _tuple3 _temp1270; struct
Cyc_Absyn_Stmt* _temp1272; struct Cyc_Absyn_Exp* _temp1274; struct _tuple3
_temp1276; struct Cyc_Absyn_Stmt* _temp1278; struct Cyc_Absyn_Exp* _temp1280;
struct Cyc_Absyn_Stmt* _temp1282; struct Cyc_Absyn_Stmt* _temp1284; struct
_tuple3 _temp1286; struct Cyc_Absyn_Stmt* _temp1288; struct Cyc_Absyn_Exp*
_temp1290; struct _tuple3 _temp1292; struct Cyc_Absyn_Stmt* _temp1294; struct
Cyc_Absyn_Exp* _temp1296; struct Cyc_Absyn_Exp* _temp1298; struct Cyc_Absyn_Stmt*
_temp1300; struct Cyc_Absyn_Switch_clause** _temp1302; struct Cyc_Absyn_Switch_clause*
_temp1304; struct Cyc_List_List* _temp1305; struct Cyc_Absyn_Stmt* _temp1307;
struct Cyc_Absyn_Stmt* _temp1309; struct Cyc_Absyn_Stmt* _temp1311; struct
_tagged_string* _temp1313; struct Cyc_List_List* _temp1315; struct Cyc_Absyn_Exp*
_temp1317; struct Cyc_List_List* _temp1319; struct Cyc_Absyn_Stmt* _temp1321;
struct Cyc_Absyn_Stmt* _temp1323; struct Cyc_Absyn_Decl* _temp1325; struct Cyc_Absyn_Decl
_temp1327; struct Cyc_Position_Segment* _temp1328; void* _temp1330; struct Cyc_Absyn_Vardecl*
_temp1332; struct Cyc_Absyn_Stmt* _temp1334; struct Cyc_Absyn_Decl* _temp1336;
struct Cyc_Absyn_Decl _temp1338; struct Cyc_Position_Segment* _temp1339; void*
_temp1341; int _temp1343; struct Cyc_Absyn_Exp* _temp1345; struct Cyc_Core_Opt*
_temp1347; struct Cyc_Core_Opt* _temp1349; struct Cyc_Core_Opt _temp1351; struct
Cyc_List_List* _temp1352; struct Cyc_Absyn_Pat* _temp1354; struct Cyc_Absyn_Stmt*
_temp1356; struct _tagged_string* _temp1358; _LL1212: if( _temp1210 == Cyc_Absyn_Skip_s){
goto _LL1213;} else{ goto _LL1214;} _LL1214: if(( unsigned int) _temp1210 > 1u?((
struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1253:
_temp1252=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1210)->f1; if( _temp1252 == 0){ goto _LL1215;} else{ goto _LL1216;}} else{
goto _LL1216;} _LL1216: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1255: _temp1254=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1210)->f1; goto _LL1217;} else{ goto
_LL1218;} _LL1218: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1257: _temp1256=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp1210)->f1; goto _LL1219;} else{ goto
_LL1220;} _LL1220: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1261: _temp1260=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1210)->f1; goto _LL1259; _LL1259: _temp1258=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1210)->f2; goto
_LL1221;} else{ goto _LL1222;} _LL1222: if(( unsigned int) _temp1210 > 1u?((
struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1267:
_temp1266=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1210)->f1; goto _LL1265; _LL1265: _temp1264=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1210)->f2; goto _LL1263; _LL1263:
_temp1262=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1210)->f3; goto _LL1223;} else{ goto _LL1224;} _LL1224: if(( unsigned int)
_temp1210 > 1u?(( struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1271: _temp1270=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1210)->f1; _LL1275: _temp1274= _temp1270.f1; goto _LL1273; _LL1273:
_temp1272= _temp1270.f2; goto _LL1269; _LL1269: _temp1268=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1210)->f2; goto _LL1225;} else{ goto
_LL1226;} _LL1226: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1283: _temp1282=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1210)->f1; goto _LL1277; _LL1277: _temp1276=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1210)->f2; _LL1281:
_temp1280= _temp1276.f1; goto _LL1279; _LL1279: _temp1278= _temp1276.f2; goto
_LL1227;} else{ goto _LL1228;} _LL1228: if(( unsigned int) _temp1210 > 1u?((
struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1299:
_temp1298=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1210)->f1;
goto _LL1293; _LL1293: _temp1292=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1210)->f2; _LL1297: _temp1296= _temp1292.f1; goto _LL1295; _LL1295:
_temp1294= _temp1292.f2; goto _LL1287; _LL1287: _temp1286=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1210)->f3; _LL1291: _temp1290= _temp1286.f1;
goto _LL1289; _LL1289: _temp1288= _temp1286.f2; goto _LL1285; _LL1285: _temp1284=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1210)->f4; goto
_LL1229;} else{ goto _LL1230;} _LL1230: if(( unsigned int) _temp1210 > 1u?((
struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1301:
_temp1300=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1210)->f1; if( _temp1300 == 0){ goto _LL1231;} else{ goto _LL1232;}} else{
goto _LL1232;} _LL1232: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1306: _temp1305=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1210)->f1; goto _LL1303; _LL1303:
_temp1302=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1210)->f2; if( _temp1302 == 0){ goto _LL1234;} else{ _temp1304=* _temp1302;
goto _LL1233;}} else{ goto _LL1234;} _LL1234: if(( unsigned int) _temp1210 > 1u?((
struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1308:
_temp1307=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1210)->f1; goto _LL1235;} else{ goto _LL1236;} _LL1236: if(( unsigned int)
_temp1210 > 1u?(( struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1310: _temp1309=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1210)->f1; goto _LL1237;} else{ goto _LL1238;} _LL1238: if(( unsigned int)
_temp1210 > 1u?(( struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1314: _temp1313=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1210)->f1; goto _LL1312; _LL1312: _temp1311=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1210)->f2; goto _LL1239;} else{ goto
_LL1240;} _LL1240: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1318: _temp1317=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1210)->f1; goto _LL1316; _LL1316:
_temp1315=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1210)->f2; goto _LL1241;} else{ goto _LL1242;} _LL1242: if(( unsigned int)
_temp1210 > 1u?(( struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1322: _temp1321=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1210)->f1; goto _LL1320; _LL1320: _temp1319=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1210)->f2; goto _LL1243;} else{ goto
_LL1244;} _LL1244: if(( unsigned int) _temp1210 > 1u?(( struct _enum_struct*)
_temp1210)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1326: _temp1325=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1210)->f1; _temp1327=* _temp1325; _LL1331:
_temp1330=( void*) _temp1327.r; if((( struct _enum_struct*) _temp1330)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1333: _temp1332=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1330)->f1; goto _LL1329;} else{ goto _LL1246;}
_LL1329: _temp1328=( struct Cyc_Position_Segment*) _temp1327.loc; goto _LL1324;
_LL1324: _temp1323=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1210)->f2; goto _LL1245;} else{ goto _LL1246;} _LL1246: if(( unsigned int)
_temp1210 > 1u?(( struct _enum_struct*) _temp1210)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1337: _temp1336=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1210)->f1; _temp1338=* _temp1336; _LL1342: _temp1341=( void*) _temp1338.r;
if((( struct _enum_struct*) _temp1341)->tag == Cyc_Absyn_Let_d_tag){ _LL1355:
_temp1354=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1341)->f1;
goto _LL1350; _LL1350: _temp1349=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1341)->f2; if( _temp1349 == 0){ goto _LL1248;} else{ _temp1351=* _temp1349;
_LL1353: _temp1352=( struct Cyc_List_List*) _temp1351.v; goto _LL1348;} _LL1348:
_temp1347=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1341)->f3;
goto _LL1346; _LL1346: _temp1345=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1341)->f4; goto _LL1344; _LL1344: _temp1343=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1341)->f5; goto _LL1340;} else{ goto _LL1248;} _LL1340: _temp1339=( struct
Cyc_Position_Segment*) _temp1338.loc; goto _LL1335; _LL1335: _temp1334=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1210)->f2; goto _LL1247;}
else{ goto _LL1248;} _LL1248: if(( unsigned int) _temp1210 > 1u?(( struct
_enum_struct*) _temp1210)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1359: _temp1358=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1210)->f1; goto
_LL1357; _LL1357: _temp1356=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1210)->f2; goto _LL1249;} else{ goto _LL1250;} _LL1250: goto _LL1251;
_LL1213: return in_flow; _LL1215: return Cyc_CfFlowInfo_BottomFL; _LL1217: Cyc_CfAbsexp_eval_absexp(
env, _temp1254->loc,( void*) aes->hd, in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1219: return Cyc_CfAbsexp_eval_absexp( env, _temp1256->loc,( void*) aes->hd,
in_flow); _LL1221: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1258,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1260, in_flow)); _LL1223: { void*
_temp1362; void* _temp1364; struct _tuple0 _temp1360= Cyc_NewControlFlow_cf_evalguard(
env, _temp1266->loc, aes, in_flow); _LL1365: _temp1364= _temp1360.f1; goto
_LL1363; _LL1363: _temp1362= _temp1360.f2; goto _LL1361; _LL1361: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1264, _temp1364), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1262, _temp1362));} _LL1225: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1272))->flow; void* _temp1368; void* _temp1370; struct _tuple0
_temp1366= Cyc_NewControlFlow_cf_evalguard( env, _temp1274->loc, aes, e_inflow);
_LL1371: _temp1370= _temp1366.f1; goto _LL1369; _LL1369: _temp1368= _temp1366.f2;
goto _LL1367; _LL1367: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1268, _temp1370); Cyc_NewControlFlow_update_flow( _temp1272,
body_outflow); return _temp1368;}} _LL1227: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1282, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1278))->flow; void* _temp1374; void* _temp1376; struct
_tuple0 _temp1372= Cyc_NewControlFlow_cf_evalguard( env, _temp1280->loc, aes,
e_inflow); _LL1377: _temp1376= _temp1372.f1; goto _LL1375; _LL1375: _temp1374=
_temp1372.f2; goto _LL1373; _LL1373: Cyc_NewControlFlow_update_flow( _temp1282,
_temp1376); return _temp1374;} _LL1229: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1298->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1294))->flow; void* _temp1380; void* _temp1382; struct _tuple0
_temp1378= Cyc_NewControlFlow_cf_evalguard( env, _temp1296->loc, aes->tl,
e2_inflow); _LL1383: _temp1382= _temp1378.f1; goto _LL1381; _LL1381: _temp1380=
_temp1378.f2; goto _LL1379; _LL1379: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1284, _temp1382); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1288))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1290->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1294, e3_outflow); return _temp1380;}} _LL1231: return Cyc_CfFlowInfo_BottomFL;
_LL1233: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1304->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1307=( struct
Cyc_Absyn_Stmt*) _temp1304->body; goto _LL1235;} _LL1235: _temp1309= _temp1307;
goto _LL1237; _LL1237: _temp1311= _temp1309; goto _LL1239; _LL1239: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1384= _temp1311; if(
_temp1384 == 0){ _throw( Null_Exception);} _temp1384;}), in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1241: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1317->loc,( void*) aes->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1315, aes->tl,
in_flow); return Cyc_CfFlowInfo_BottomFL; _LL1243: { struct Cyc_NewControlFlow_AnalEnv
_temp1387; void* _temp1388; int _temp1390; struct Cyc_Dict_Dict* _temp1392;
struct Cyc_NewControlFlow_AnalEnv* _temp1385= env; _temp1387=* _temp1385;
_LL1393: _temp1392=( struct Cyc_Dict_Dict*) _temp1387.roots; goto _LL1391;
_LL1391: _temp1390=( int) _temp1387.in_try; goto _LL1389; _LL1389: _temp1388=(
void*) _temp1387.tryflow; goto _LL1386; _LL1386: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1321, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1390;( void*)( env->tryflow=( void*) _temp1388); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1319,
aes, scs_inflow); return s_outflow;}} _LL1245: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1394; _temp1394.hd=( void*) _temp1332; _temp1394.tl= 0;
_temp1394;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1332->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1395= e; if( _temp1395 == 0){ _throw( Null_Exception);}
_temp1395;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1323, body_inflow);} _LL1247: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1352); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1345->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1352); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1334, body_inflow);} _LL1249: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1356, in_flow); _LL1251:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1396=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1396->tag= Cyc_Core_Impossible_tag; _temp1396->f1=( struct _tagged_string)({
char* _temp1397=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1398; _temp1398.curr= _temp1397; _temp1398.base=
_temp1397; _temp1398.last_plus_one= _temp1397 + 62; _temp1398;});( struct
_xenum_struct*) _temp1396;})); _LL1211:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1399=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_EmptyAnnot_struct)); _temp1399->tag= Cyc_Absyn_EmptyAnnot_tag;(
struct _xenum_struct*) _temp1399;});{ void* _temp1400=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp1422; struct Cyc_Absyn_Stmt* _temp1424; struct Cyc_Absyn_Stmt* _temp1426;
struct Cyc_Absyn_Stmt* _temp1428; struct Cyc_Absyn_Exp* _temp1430; struct Cyc_Absyn_Stmt*
_temp1432; struct _tuple3 _temp1434; struct Cyc_Absyn_Stmt* _temp1436; struct
Cyc_Absyn_Exp* _temp1438; struct _tuple3 _temp1440; struct Cyc_Absyn_Stmt*
_temp1442; struct Cyc_Absyn_Exp* _temp1444; struct Cyc_Absyn_Exp* _temp1446;
struct Cyc_Absyn_Stmt* _temp1448; struct _tuple3 _temp1450; struct Cyc_Absyn_Stmt*
_temp1452; struct Cyc_Absyn_Exp* _temp1454; struct _tuple3 _temp1456; struct Cyc_Absyn_Stmt*
_temp1458; struct Cyc_Absyn_Exp* _temp1460; struct Cyc_Absyn_Stmt* _temp1462;
struct Cyc_Absyn_Stmt* _temp1464; struct _tagged_string* _temp1466; struct Cyc_Absyn_Stmt*
_temp1468; struct Cyc_Absyn_Decl* _temp1470; struct Cyc_List_List* _temp1472;
struct Cyc_Absyn_Stmt* _temp1474; struct Cyc_List_List* _temp1476; struct Cyc_Absyn_Exp*
_temp1478; _LL1402: if(( unsigned int) _temp1400 > 1u?(( struct _enum_struct*)
_temp1400)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1425: _temp1424=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1400)->f1; goto _LL1423; _LL1423: _temp1422=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1400)->f2; goto
_LL1403;} else{ goto _LL1404;} _LL1404: if(( unsigned int) _temp1400 > 1u?((
struct _enum_struct*) _temp1400)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1431:
_temp1430=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1400)->f1; goto _LL1429; _LL1429: _temp1428=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1400)->f2; goto _LL1427; _LL1427:
_temp1426=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1400)->f3; goto _LL1405;} else{ goto _LL1406;} _LL1406: if(( unsigned int)
_temp1400 > 1u?(( struct _enum_struct*) _temp1400)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1447: _temp1446=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1400)->f1; goto _LL1441; _LL1441: _temp1440=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1400)->f2; _LL1445: _temp1444= _temp1440.f1; goto _LL1443; _LL1443:
_temp1442= _temp1440.f2; goto _LL1435; _LL1435: _temp1434=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1400)->f3; _LL1439: _temp1438= _temp1434.f1;
goto _LL1437; _LL1437: _temp1436= _temp1434.f2; goto _LL1433; _LL1433: _temp1432=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1400)->f4; goto
_LL1407;} else{ goto _LL1408;} _LL1408: if(( unsigned int) _temp1400 > 1u?((
struct _enum_struct*) _temp1400)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1451:
_temp1450=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1400)->f1;
_LL1455: _temp1454= _temp1450.f1; goto _LL1453; _LL1453: _temp1452= _temp1450.f2;
goto _LL1449; _LL1449: _temp1448=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1400)->f2; goto _LL1409;} else{ goto _LL1410;} _LL1410: if(( unsigned int)
_temp1400 > 1u?(( struct _enum_struct*) _temp1400)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1463: _temp1462=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1400)->f1; goto _LL1457; _LL1457: _temp1456=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1400)->f2; _LL1461: _temp1460= _temp1456.f1; goto _LL1459; _LL1459:
_temp1458= _temp1456.f2; goto _LL1411;} else{ goto _LL1412;} _LL1412: if((
unsigned int) _temp1400 > 1u?(( struct _enum_struct*) _temp1400)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1467: _temp1466=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1400)->f1; goto _LL1465; _LL1465: _temp1464=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1400)->f2; goto _LL1413;} else{ goto
_LL1414;} _LL1414: if(( unsigned int) _temp1400 > 1u?(( struct _enum_struct*)
_temp1400)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1471: _temp1470=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1400)->f1; goto _LL1469; _LL1469:
_temp1468=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1400)->f2;
goto _LL1415;} else{ goto _LL1416;} _LL1416: if(( unsigned int) _temp1400 > 1u?((
struct _enum_struct*) _temp1400)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1475:
_temp1474=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1400)->f1; goto _LL1473; _LL1473: _temp1472=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1400)->f2; goto _LL1417;} else{ goto
_LL1418;} _LL1418: if(( unsigned int) _temp1400 > 1u?(( struct _enum_struct*)
_temp1400)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1479: _temp1478=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1400)->f1; goto _LL1477; _LL1477:
_temp1476=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1400)->f2; goto _LL1419;} else{ goto _LL1420;} _LL1420: goto _LL1421;
_LL1403: _temp1428= _temp1424; _temp1426= _temp1422; goto _LL1405; _LL1405: Cyc_NewControlFlow_cf_postpass(
_temp1428); Cyc_NewControlFlow_cf_postpass( _temp1426); return; _LL1407: Cyc_NewControlFlow_cf_postpass(
_temp1436); _temp1452= _temp1432; _temp1448= _temp1442; goto _LL1409; _LL1409:
_temp1462= _temp1448; _temp1458= _temp1452; goto _LL1411; _LL1411: Cyc_NewControlFlow_cf_postpass(
_temp1458); _temp1464= _temp1462; goto _LL1413; _LL1413: _temp1468= _temp1464;
goto _LL1415; _LL1415: Cyc_NewControlFlow_cf_postpass( _temp1468); return;
_LL1417: Cyc_NewControlFlow_cf_postpass( _temp1474); _temp1476= _temp1472; goto
_LL1419; _LL1419: for( 0; _temp1476 != 0; _temp1476= _temp1476->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1476->hd)->body);} return; _LL1421:
return; _LL1401:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1480; _temp1480.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1481=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1481->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1481->comprehension_vars=
0; _temp1481;}); _temp1480.lhsides= 0; _temp1480.access_path= 0; _temp1480;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1484; struct Cyc_List_List*
_temp1486; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1488; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1490; struct Cyc_List_List* _temp1491; struct Cyc_Dict_Dict* _temp1493;
struct Cyc_NewControlFlow_AbsEnv _temp1482= absenv; _LL1489: _temp1488=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1482.shared; _temp1490=* _temp1488;
_LL1494: _temp1493=( struct Cyc_Dict_Dict*) _temp1490.all_roots; goto _LL1492;
_LL1492: _temp1491=( struct Cyc_List_List*) _temp1490.comprehension_vars; goto
_LL1487; _LL1487: _temp1486=( struct Cyc_List_List*) _temp1482.lhsides; goto
_LL1485; _LL1485: _temp1484=( struct Cyc_List_List*) _temp1482.access_path; goto
_LL1483; _LL1483: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1495=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1495->roots= _temp1493; _temp1495->in_try= 0; _temp1495->tryflow=( void*)
Cyc_CfFlowInfo_BottomFL; _temp1495;}); void* in_flow=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1496=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1496->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp1496->f1=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);(
void*) _temp1496;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( analenv,
in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow( analenv,
in_flow, _temp1491);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1497= out_flow; _LL1499: if( _temp1497 == Cyc_CfFlowInfo_BottomFL){
goto _LL1500;} else{ goto _LL1501;} _LL1501: goto _LL1502; _LL1500: goto _LL1498;
_LL1502: if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp1503=( char*)"function may complete without returning a value";
struct _tagged_string _temp1504; _temp1504.curr= _temp1503; _temp1504.base=
_temp1503; _temp1504.last_plus_one= _temp1503 + 48; _temp1504;}));} goto _LL1498;
_LL1498:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1505=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1515; struct
Cyc_List_List* _temp1517; struct _tuple1* _temp1519; struct Cyc_List_List*
_temp1521; struct _tagged_string* _temp1523; _LL1507: if((( struct _enum_struct*)
_temp1505)->tag == Cyc_Absyn_Fn_d_tag){ _LL1516: _temp1515=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1505)->f1; goto _LL1508;} else{ goto _LL1509;}
_LL1509: if((( struct _enum_struct*) _temp1505)->tag == Cyc_Absyn_Using_d_tag){
_LL1520: _temp1519=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1505)->f1; goto _LL1518; _LL1518: _temp1517=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1505)->f2; goto _LL1510;} else{ goto
_LL1511;} _LL1511: if((( struct _enum_struct*) _temp1505)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1524: _temp1523=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1505)->f1; goto _LL1522; _LL1522: _temp1521=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1505)->f2; goto _LL1512;} else{ goto
_LL1513;} _LL1513: goto _LL1514; _LL1508: Cyc_NewControlFlow_cf_check_fun(
_temp1515); goto _LL1506; _LL1510: _temp1521= _temp1517; goto _LL1512; _LL1512:
Cyc_NewControlFlow_cf_check( _temp1521); goto _LL1506; _LL1514: goto _LL1506;
_LL1506:;}}