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
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp39[ 0]=({
struct Cyc_List_List _temp40; _temp40.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp40.tl= 0; _temp40;}); _temp39;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv ans= Cyc_NewControlFlow_change_lhs( src,
Cyc_NewControlFlow_use_it_list()); ans.access_path= 0; return ans;} struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn syn){ struct
Cyc_List_List* _temp43; struct Cyc_NewControlFlow_AbsSynOne _temp45; void*
_temp47; void* _temp49; struct Cyc_NewControlFlow_AbsSynOne _temp51; void*
_temp53; void* _temp55; struct Cyc_NewControlFlow_AbsSyn _temp41= syn; _LL52:
_temp51=( struct Cyc_NewControlFlow_AbsSynOne) _temp41.when_true; _LL56: _temp55=(
void*) _temp51.inner_exp; goto _LL54; _LL54: _temp53=( void*) _temp51.assigns;
goto _LL46; _LL46: _temp45=( struct Cyc_NewControlFlow_AbsSynOne) _temp41.when_false;
_LL50: _temp49=( void*) _temp45.inner_exp; goto _LL48; _LL48: _temp47=( void*)
_temp45.assigns; goto _LL44; _LL44: _temp43=( struct Cyc_List_List*) _temp41.lvalues;
goto _LL42; _LL42: { void* inn= _temp55 == _temp49? _temp55: Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OneofG, _temp55, _temp49); void* ass= _temp53 == _temp47? _temp53:
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, _temp53, _temp47); return({ struct
Cyc_NewControlFlow_AbsSynOne _temp57; _temp57.inner_exp=( void*) inn; _temp57.assigns=(
void*) ass; _temp57;});}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_mkSyn_tf(
void* ae, struct Cyc_List_List* lvals, void* assigns){ struct Cyc_NewControlFlow_AbsSynOne
one_way=({ struct Cyc_NewControlFlow_AbsSynOne _temp58; _temp58.inner_exp=( void*)
ae; _temp58.assigns=( void*) assigns; _temp58;}); return({ struct Cyc_NewControlFlow_AbsSyn
_temp59; _temp59.when_true= one_way; _temp59.when_false= one_way; _temp59.lvalues=
lvals; _temp59;});} void* Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv
env, void* ae){ if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(( env.lhsides)->tl
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)( env.lhsides)->hd, ae);}{ struct
Cyc_List_List* l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0;
lhsides != 0; lhsides= lhsides->tl){ l=({ struct Cyc_List_List* _temp60=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp60[ 0]=({
struct Cyc_List_List _temp61; _temp61.hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)
lhsides->hd, ae); _temp61.tl= l; _temp61;}); _temp60;});}} return Cyc_CfAbsexp_mkGroupAE_l(
Cyc_CfAbsexp_OneofG, l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es= es->tl){ struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*) es->hd); void* _temp64; void* _temp66; struct Cyc_NewControlFlow_AbsSynOne
_temp62= Cyc_NewControlFlow_meet_absexp( syn); _LL67: _temp66=( void*) _temp62.inner_exp;
goto _LL65; _LL65: _temp64=( void*) _temp62.assigns; goto _LL63; _LL63: es_exp=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp66, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp64, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* es_exp,
void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE(
e), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e))));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp68=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp68[ 0]=({ struct Cyc_List_List
_temp69; _temp69.hd=( void*) path_e; _temp69.tl= env.access_path; _temp69;});
_temp68;});{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path= path->tl){ void* _temp70=( void*)(( struct Cyc_Absyn_Exp*) path->hd)->r;
struct Cyc_Absyn_Exp* _temp84; struct Cyc_Absyn_Exp* _temp86; struct
_tagged_string* _temp88; struct Cyc_Absyn_Exp* _temp90; struct _tagged_string*
_temp92; struct Cyc_Absyn_Exp* _temp94; struct Cyc_Absyn_Exp* _temp96; struct
Cyc_Absyn_Exp* _temp98; _LL72: if((( struct _enum_struct*) _temp70)->tag == Cyc_Absyn_Address_e_tag){
_LL85: _temp84=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp70)->f1; goto _LL73;} else{ goto _LL74;} _LL74: if((( struct _enum_struct*)
_temp70)->tag == Cyc_Absyn_Deref_e_tag){ _LL87: _temp86=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp70)->f1; goto _LL75;} else{ goto _LL76;}
_LL76: if((( struct _enum_struct*) _temp70)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL91: _temp90=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp70)->f1; goto _LL89; _LL89: _temp88=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp70)->f2; goto _LL77;} else{ goto _LL78;} _LL78: if((( struct _enum_struct*)
_temp70)->tag == Cyc_Absyn_StructMember_e_tag){ _LL95: _temp94=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp70)->f1; goto _LL93; _LL93:
_temp92=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp70)->f2; goto _LL79;} else{ goto _LL80;} _LL80: if((( struct _enum_struct*)
_temp70)->tag == Cyc_Absyn_Subscript_e_tag){ _LL99: _temp98=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp70)->f1; goto _LL97; _LL97: _temp96=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp70)->f2;
goto _LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL73: path->tl == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp100=( char*)"new_control_flow.cyc:243 path->tl == null";
struct _tagged_string _temp101; _temp101.curr= _temp100; _temp101.base= _temp100;
_temp101.last_plus_one= _temp100 + 42; _temp101;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL75: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL71; _LL77: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp92= _temp88; goto _LL79; _LL79: absop= Cyc_CfAbsexp_mkMemberOp(
absop,({ struct Cyc_CfFlowInfo_StructF_struct* _temp102=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp102->tag= Cyc_CfFlowInfo_StructF_tag;
_temp102->f1= _temp92;( void*) _temp102;})); goto _LL71; _LL81: { unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp96); absop= Cyc_CfAbsexp_mkMemberOp( absop,({
struct Cyc_CfFlowInfo_TupleF_struct* _temp103=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp103->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp103->f1=( int) i;( void*) _temp103;})); goto
_LL71;} _LL83:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp104=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp104->tag= Cyc_Core_Impossible_tag; _temp104->f1=( struct _tagged_string)({
char* _temp105=( char*)"abstract_exp: unexpected acces path element"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 44; _temp106;});( struct _xenum_struct*)
_temp104;})); _LL71:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp107=(
void*) e->r; struct Cyc_Absyn_Exp* _temp195; struct Cyc_Absyn_Exp* _temp197;
struct Cyc_List_List* _temp199; struct Cyc_Absyn_Exp* _temp201; void* _temp203;
int _temp205; void* _temp207; void* _temp209; int _temp211; void* _temp213; void*
_temp215; void* _temp217; void* _temp219; struct Cyc_Absyn_Fndecl* _temp221;
struct _tuple1* _temp223; void* _temp225; struct Cyc_Absyn_Vardecl* _temp227;
struct _tuple1* _temp229; void* _temp231; struct Cyc_Absyn_Vardecl* _temp233;
struct _tuple1* _temp235; void* _temp237; struct Cyc_Absyn_Vardecl* _temp239;
struct _tuple1* _temp241; void* _temp243; struct Cyc_Absyn_Vardecl* _temp245;
struct _tuple1* _temp247; struct Cyc_Absyn_Enumfield* _temp249; struct Cyc_Absyn_Enumdecl*
_temp251; struct Cyc_List_List* _temp253; struct Cyc_Core_Opt* _temp255; struct
Cyc_Core_Opt* _temp257; struct Cyc_Absyn_Enumfield* _temp259; struct Cyc_Absyn_Xenumdecl*
_temp261; struct Cyc_List_List* _temp263; struct Cyc_Core_Opt* _temp265; void*
_temp267; void* _temp269; void* _temp271; struct Cyc_Absyn_Exp* _temp273; struct
Cyc_Core_Opt* _temp275; struct Cyc_Absyn_Exp* _temp277; struct Cyc_Absyn_Exp*
_temp279; struct Cyc_Core_Opt* _temp281; struct Cyc_Absyn_Exp* _temp283; struct
Cyc_List_List* _temp285; void* _temp287; struct Cyc_Absyn_Exp* _temp289; void*
_temp291; struct Cyc_List_List* _temp293; struct Cyc_Absyn_Exp* _temp295; struct
Cyc_Absyn_Exp* _temp297; struct Cyc_Absyn_Exp* _temp299; struct Cyc_Absyn_Exp*
_temp301; struct Cyc_Absyn_Exp* _temp303; struct Cyc_Absyn_Exp* _temp305; struct
_tagged_string* _temp307; struct Cyc_Absyn_Exp* _temp309; struct _tagged_string*
_temp311; struct Cyc_Absyn_Exp* _temp313; struct Cyc_List_List* _temp315; struct
Cyc_Absyn_Structdecl* _temp317; struct Cyc_List_List* _temp319; struct Cyc_Core_Opt*
_temp321; struct _tuple1* _temp323; struct Cyc_List_List* _temp325; void*
_temp327; struct Cyc_Absyn_Exp* _temp329; struct Cyc_Absyn_Exp* _temp331; struct
Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp* _temp335; struct Cyc_Absyn_Exp*
_temp337; struct Cyc_Absyn_Stmt* _temp339; struct Cyc_Absyn_Exp* _temp341;
struct Cyc_Absyn_Exp* _temp343; struct Cyc_Absyn_Exp* _temp345; struct Cyc_Absyn_Exp*
_temp347; struct Cyc_Absyn_Vardecl* _temp349; void* _temp351; struct _tuple1*
_temp353; struct _tuple1* _temp355; struct Cyc_List_List* _temp357; struct Cyc_Absyn_Exp*
_temp359; struct Cyc_List_List* _temp361; struct Cyc_Core_Opt* _temp363; struct
Cyc_List_List* _temp365; struct _tuple2* _temp367; struct Cyc_Absyn_Fndecl*
_temp369; struct Cyc_Absyn_Exp* _temp371; _LL109: if((( struct _enum_struct*)
_temp107)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL196: _temp195=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp107)->f1; goto _LL110;} else{
goto _LL111;} _LL111: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL198: _temp197=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp107)->f1; goto _LL112;} else{ goto _LL113;} _LL113: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL202: _temp201=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp107)->f1;
goto _LL200; _LL200: _temp199=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp107)->f2; goto _LL114;} else{ goto _LL115;} _LL115: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Const_e_tag){ _LL204: _temp203=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp107)->f1; if(( unsigned int) _temp203 > 1u?((
struct _enum_struct*) _temp203)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL208:
_temp207=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp203)->f1; if( _temp207
== Cyc_Absyn_Signed){ goto _LL206;} else{ goto _LL117;} _LL206: _temp205=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp203)->f2; if( _temp205 == 0){ goto _LL116;}
else{ goto _LL117;}} else{ goto _LL117;}} else{ goto _LL117;} _LL117: if(((
struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Const_e_tag){ _LL210: _temp209=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp107)->f1; if(( unsigned int)
_temp209 > 1u?(( struct _enum_struct*) _temp209)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL214: _temp213=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp209)->f1; if(
_temp213 == Cyc_Absyn_Signed){ goto _LL212;} else{ goto _LL119;} _LL212:
_temp211=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp209)->f2; if( _temp211 ==
1){ goto _LL118;} else{ goto _LL119;}} else{ goto _LL119;}} else{ goto _LL119;}
_LL119: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL216: _temp215=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp107)->f1;
goto _LL120;} else{ goto _LL121;} _LL121: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Const_e_tag){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp107)->f1; goto _LL122;} else{ goto _LL123;} _LL123: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){ _LL224: _temp223=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1; goto _LL220; _LL220:
_temp219=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f2; if((
unsigned int) _temp219 > 1u?(( struct _enum_struct*) _temp219)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL222: _temp221=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp219)->f1; goto _LL124;} else{ goto _LL125;}} else{ goto _LL125;} _LL125:
if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){ _LL230:
_temp229=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1;
goto _LL226; _LL226: _temp225=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp225 > 1u?(( struct _enum_struct*)
_temp225)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL228: _temp227=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp225)->f1; goto _LL126;} else{ goto
_LL127;}} else{ goto _LL127;} _LL127: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Var_e_tag){ _LL236: _temp235=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f1; goto _LL232; _LL232: _temp231=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp231 > 1u?(( struct _enum_struct*)
_temp231)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL234: _temp233=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp231)->f1; goto _LL128;} else{ goto _LL129;}}
else{ goto _LL129;} _LL129: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){
_LL242: _temp241=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1;
goto _LL238; _LL238: _temp237=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp237 > 1u?(( struct _enum_struct*)
_temp237)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL240: _temp239=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp237)->f1; goto _LL130;} else{ goto _LL131;}}
else{ goto _LL131;} _LL131: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){
_LL248: _temp247=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1;
goto _LL244; _LL244: _temp243=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp243 > 1u?(( struct _enum_struct*)
_temp243)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL246: _temp245=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp243)->f1; goto _LL132;} else{ goto _LL133;}}
else{ goto _LL133;} _LL133: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Enum_e_tag){
_LL258: _temp257=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp107)->f1; goto _LL256; _LL256: _temp255=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp107)->f2; goto _LL254; _LL254: _temp253=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp107)->f3; goto _LL252;
_LL252: _temp251=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp107)->f4; goto _LL250; _LL250: _temp249=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp107)->f5; goto _LL134;} else{ goto _LL135;}
_LL135: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Xenum_e_tag){
_LL266: _temp265=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp107)->f1; goto _LL264; _LL264: _temp263=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp107)->f2; goto _LL262; _LL262: _temp261=( struct
Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp107)->f3; goto
_LL260; _LL260: _temp259=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp107)->f4; goto _LL136;} else{ goto _LL137;} _LL137: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Malloc_e_tag){ _LL268: _temp267=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp107)->f1; if((( struct
_enum_struct*) _temp267)->tag == Cyc_Absyn_Typ_m_tag){ _LL270: _temp269=( void*)((
struct Cyc_Absyn_Typ_m_struct*) _temp267)->f1; goto _LL138;} else{ goto _LL139;}}
else{ goto _LL139;} _LL139: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Malloc_e_tag){
_LL272: _temp271=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp107)->f1;
goto _LL140;} else{ goto _LL141;} _LL141: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL278: _temp277=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp107)->f1; goto _LL276; _LL276: _temp275=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp107)->f2; if(
_temp275 == 0){ goto _LL274;} else{ goto _LL143;} _LL274: _temp273=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp107)->f3; goto _LL142;} else{ goto
_LL143;} _LL143: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL284: _temp283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp107)->f1; goto _LL282; _LL282: _temp281=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp107)->f2; goto _LL280; _LL280: _temp279=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp107)->f3;
goto _LL144;} else{ goto _LL145;} _LL145: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Primop_e_tag){ _LL288: _temp287=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp107)->f1; goto _LL286; _LL286: _temp285=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp107)->f2; goto _LL146;} else{ goto _LL147;}
_LL147: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Cast_e_tag){
_LL292: _temp291=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp107)->f1; goto
_LL290; _LL290: _temp289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp107)->f2; goto _LL148;} else{ goto _LL149;} _LL149: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_FnCall_e_tag){ _LL296: _temp295=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp107)->f1; goto
_LL294; _LL294: _temp293=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp107)->f2; goto _LL150;} else{ goto _LL151;} _LL151: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Subscript_e_tag){ _LL300: _temp299=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp107)->f1;
goto _LL298; _LL298: _temp297=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp107)->f2; goto _LL152;} else{ goto _LL153;} _LL153: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_New_e_tag){ _LL302: _temp301=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp107)->f1; goto _LL154;}
else{ goto _LL155;} _LL155: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Address_e_tag){
_LL304: _temp303=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp107)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Deref_e_tag){ _LL306: _temp305=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp107)->f1; goto
_LL158;} else{ goto _LL159;} _LL159: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL310: _temp309=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp107)->f1; goto _LL308; _LL308:
_temp307=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp107)->f2; goto _LL160;} else{ goto _LL161;} _LL161: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_StructMember_e_tag){ _LL314: _temp313=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp107)->f1;
goto _LL312; _LL312: _temp311=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp107)->f2; goto _LL162;} else{ goto _LL163;} _LL163: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Tuple_e_tag){ _LL316: _temp315=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp107)->f1; goto
_LL164;} else{ goto _LL165;} _LL165: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Struct_e_tag){ _LL324: _temp323=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp107)->f1; goto _LL322; _LL322: _temp321=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp107)->f2; goto _LL320; _LL320: _temp319=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp107)->f3; goto
_LL318; _LL318: _temp317=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp107)->f4; goto _LL166;} else{ goto _LL167;} _LL167: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Array_e_tag){ _LL326: _temp325=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp107)->f1; goto
_LL168;} else{ goto _LL169;} _LL169: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Increment_e_tag){ _LL330: _temp329=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp107)->f1; goto _LL328; _LL328:
_temp327=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp107)->f2; goto
_LL170;} else{ goto _LL171;} _LL171: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Throw_e_tag){ _LL332: _temp331=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Throw_e_struct*) _temp107)->f1; goto _LL172;} else{ goto _LL173;}
_LL173: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Conditional_e_tag){
_LL338: _temp337=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp107)->f1; goto _LL336; _LL336: _temp335=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp107)->f2; goto _LL334; _LL334: _temp333=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp107)->f3;
goto _LL174;} else{ goto _LL175;} _LL175: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL340: _temp339=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_StmtExp_e_struct*) _temp107)->f1; goto _LL176;} else{ goto _LL177;}
_LL177: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL344: _temp343=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp107)->f1; goto _LL342; _LL342: _temp341=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp107)->f2; goto _LL178;} else{ goto _LL179;}
_LL179: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL350: _temp349=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp107)->f1; goto _LL348; _LL348: _temp347=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp107)->f2; goto _LL346; _LL346: _temp345=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp107)->f3;
goto _LL180;} else{ goto _LL181;} _LL181: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Var_e_tag){ _LL354: _temp353=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f1; goto _LL352; _LL352: _temp351=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if( _temp351 == Cyc_Absyn_Unresolved_b){ goto _LL182;} else{ goto
_LL183;}} else{ goto _LL183;} _LL183: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL356: _temp355=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp107)->f1; goto _LL184;} else{ goto _LL185;} _LL185: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL360: _temp359=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp107)->f1;
goto _LL358; _LL358: _temp357=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp107)->f2; goto _LL186;} else{ goto _LL187;} _LL187: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL364:
_temp363=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp107)->f1; goto _LL362; _LL362: _temp361=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp107)->f2; goto _LL188;} else{ goto
_LL189;} _LL189: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL368: _temp367=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp107)->f1; goto _LL366; _LL366: _temp365=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp107)->f2; goto _LL190;} else{ goto _LL191;}
_LL191: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Codegen_e_tag){
_LL370: _temp369=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp107)->f1; goto _LL192;} else{ goto _LL193;} _LL193: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Fill_e_tag){ _LL372: _temp371=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp107)->f1; goto
_LL194;} else{ goto _LL108;} _LL110: _temp197= _temp195; goto _LL112; _LL112:
_temp201= _temp197; goto _LL114; _LL114: return Cyc_NewControlFlow_abstract_exp(
env, _temp201); _LL116: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp373=( char*)"new_control_flow.cyc:266 env.access_path==null";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 47; _temp374;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp375; _temp375.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp377;
_temp377.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp377.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp377;}); _temp375.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp376; _temp376.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp376.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp376;}); _temp375.lvalues= 0; _temp375;}); _LL118: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp378=( char*)"new_control_flow.cyc:271 env.access_path==null";
struct _tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 47; _temp379;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp380; _temp380.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp382;
_temp382.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp382.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp382;});
_temp380.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp381; _temp381.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp381.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp381;}); _temp380.lvalues= 0; _temp380;}); _LL120: goto _LL122; _LL122: goto
_LL124; _LL124: goto _LL126; return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL126:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL128:
_temp239= _temp233; goto _LL130; _LL130: _temp245= _temp239; goto _LL132; _LL132: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp245)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp383=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp383[ 0]=({ struct Cyc_List_List _temp384;
_temp384.hd=( void*) absop; _temp384.tl= 0; _temp384;}); _temp383;}), Cyc_NewControlFlow_make_assigns(
env, absop));} _LL134: _temp263= _temp253; _temp259= _temp249; goto _LL136;
_LL136: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp385=( char*)"new_control_flow.cyc:301 env.access_path == null";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 49; _temp386;})); if( _temp263 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_enum_root( env, e,
_temp259);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp263 != 0; _temp263= _temp263->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp387=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp387[
0]=({ struct Cyc_List_List _temp388; _temp388.hd=( void*) Cyc_CfAbsexp_mkMemberOp(
Cyc_CfAbsexp_mkMallocOp( e),({ struct Cyc_CfFlowInfo_TupleF_struct* _temp389=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp389->tag= Cyc_CfFlowInfo_TupleF_tag; _temp389->f1= i;( void*) _temp389;}));
_temp388.tl= 0; _temp388;}); _temp387;})); struct Cyc_NewControlFlow_AbsSyn syn=
Cyc_NewControlFlow_abstract_exp( elt_env,( struct Cyc_Absyn_Exp*) _temp263->hd);
void* _temp392; void* _temp394; struct Cyc_NewControlFlow_AbsSynOne _temp390=
Cyc_NewControlFlow_meet_absexp( syn); _LL395: _temp394=( void*) _temp390.inner_exp;
goto _LL393; _LL393: _temp392=( void*) _temp390.assigns; goto _LL391; _LL391:
es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp394, es_exp);
useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp392, useexp);}}
return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp, useexp);} _LL138: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp269); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE(
e), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL140:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp396=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp396->tag= Cyc_Core_Impossible_tag; _temp396->f1=( struct _tagged_string)({
char* _temp397=( char*)"NewControlFlow: Malloc_e of unsupported variant"; struct
_tagged_string _temp398; _temp398.curr= _temp397; _temp398.base= _temp397;
_temp398.last_plus_one= _temp397 + 48; _temp398;});( struct _xenum_struct*)
_temp396;})); _LL142: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp277); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp273); void* _temp401; void*
_temp403; struct Cyc_NewControlFlow_AbsSynOne _temp399= Cyc_NewControlFlow_meet_absexp(
syn1); _LL404: _temp403=( void*) _temp399.inner_exp; goto _LL402; _LL402:
_temp401=( void*) _temp399.assigns; goto _LL400; _LL400: { void* _temp407; void*
_temp409; struct Cyc_NewControlFlow_AbsSynOne _temp405= Cyc_NewControlFlow_meet_absexp(
syn2); _LL410: _temp409=( void*) _temp405.inner_exp; goto _LL408; _LL408:
_temp407=( void*) _temp405.assigns; goto _LL406; _LL406: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp403,
_temp409), _temp407); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp401);}}}
_LL144: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp411=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp411[ 0]=({ struct Cyc_List_List _temp412; _temp412.hd=( void*) _temp283;
_temp412.tl=({ struct Cyc_List_List* _temp413=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp413[ 0]=({ struct Cyc_List_List
_temp414; _temp414.hd=( void*) _temp279; _temp414.tl= 0; _temp414;}); _temp413;});
_temp412;}); _temp411;}), 0); _LL146: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp285, 0); _LL148: { int both_leaf= 1;{ void* _temp415=( void*)(
_temp289->topt)->v; struct Cyc_Absyn_Structdecl** _temp423; struct Cyc_List_List*
_temp425; struct _tuple1* _temp427; struct Cyc_List_List* _temp429; _LL417: if((
unsigned int) _temp415 > 4u?(( struct _enum_struct*) _temp415)->tag == Cyc_Absyn_StructType_tag:
0){ _LL428: _temp427=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp415)->f1; goto _LL426; _LL426: _temp425=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp415)->f2; goto _LL424; _LL424: _temp423=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp415)->f3;
goto _LL418;} else{ goto _LL419;} _LL419: if(( unsigned int) _temp415 > 4u?((
struct _enum_struct*) _temp415)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL430:
_temp429=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp415)->f1;
goto _LL420;} else{ goto _LL421;} _LL421: goto _LL422; _LL418: goto _LL420;
_LL420: both_leaf= 0; goto _LL416; _LL422: goto _LL416; _LL416:;}{ void*
_temp431=( void*)( e->topt)->v; struct Cyc_Absyn_Structdecl** _temp439; struct
Cyc_List_List* _temp441; struct _tuple1* _temp443; struct Cyc_List_List*
_temp445; _LL433: if(( unsigned int) _temp431 > 4u?(( struct _enum_struct*)
_temp431)->tag == Cyc_Absyn_StructType_tag: 0){ _LL444: _temp443=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp431)->f1; goto _LL442;
_LL442: _temp441=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f2; goto _LL440; _LL440: _temp439=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp431)->f3; goto _LL434;} else{ goto
_LL435;} _LL435: if(( unsigned int) _temp431 > 4u?(( struct _enum_struct*)
_temp431)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL446: _temp445=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp431)->f1; goto _LL436;} else{ goto
_LL437;} _LL437: goto _LL438; _LL434: goto _LL436; _LL436: both_leaf= 0; goto
_LL432; _LL438: goto _LL432; _LL432:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp(
env, _temp289);} return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp447=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp447[ 0]=({ struct Cyc_List_List _temp448; _temp448.hd=( void*) _temp289;
_temp448.tl= 0; _temp448;}); _temp447;}), env.lhsides);} _LL150: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp449=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp449[ 0]=({ struct Cyc_List_List
_temp450; _temp450.hd=( void*) _temp295; _temp450.tl= _temp293; _temp450;});
_temp449;}), 0); _LL152: { void* _temp451= Cyc_Tcutil_compress(( void*)(
_temp299->topt)->v); struct Cyc_List_List* _temp457; _LL453: if(( unsigned int)
_temp451 > 4u?(( struct _enum_struct*) _temp451)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL458: _temp457=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp451)->f1; goto _LL454;} else{ goto _LL455;} _LL455: goto _LL456; _LL454:
return Cyc_NewControlFlow_descend_path( env, e, _temp299); _LL456: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp459=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp459[ 0]=({ struct Cyc_List_List
_temp460; _temp460.hd=( void*) _temp299; _temp460.tl=({ struct Cyc_List_List*
_temp461=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp461[ 0]=({ struct Cyc_List_List _temp462; _temp462.hd=( void*) _temp297;
_temp462.tl= 0; _temp462;}); _temp461;}); _temp460;}); _temp459;}), Cyc_NewControlFlow_use_it_list());
_LL452:;} _LL154: { void* _temp463=( void*) _temp301->r; struct Cyc_Absyn_Structdecl*
_temp471; struct Cyc_List_List* _temp473; struct Cyc_Core_Opt* _temp475; struct
_tuple1* _temp477; struct Cyc_List_List* _temp479; _LL465: if((( struct
_enum_struct*) _temp463)->tag == Cyc_Absyn_Struct_e_tag){ _LL478: _temp477=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp463)->f1; goto _LL476;
_LL476: _temp475=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp463)->f2; goto _LL474; _LL474: _temp473=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp463)->f3; goto _LL472; _LL472: _temp471=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp463)->f4;
goto _LL466;} else{ goto _LL467;} _LL467: if((( struct _enum_struct*) _temp463)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL480: _temp479=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp463)->f1; goto _LL468;} else{ goto _LL469;}
_LL469: goto _LL470; _LL466: goto _LL468; _LL468: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp301->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp481=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp481[ 0]=({ struct Cyc_List_List _temp482;
_temp482.hd=( void*) malloc_op; _temp482.tl= 0; _temp482;}); _temp481;}));
inner_env.access_path= 0;{ void* _temp485; void* _temp487; struct Cyc_NewControlFlow_AbsSynOne
_temp483= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp301)); _LL488: _temp487=( void*) _temp483.inner_exp; goto _LL486;
_LL486: _temp485=( void*) _temp483.assigns; goto _LL484; _LL484: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp487, _temp485); if( env.access_path
!= 0){ void* _temp489=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp499; struct Cyc_Absyn_Exp* _temp501; struct Cyc_Absyn_Exp*
_temp503; struct _tagged_string* _temp505; struct Cyc_Absyn_Exp* _temp507;
_LL491: if((( struct _enum_struct*) _temp489)->tag == Cyc_Absyn_Subscript_e_tag){
_LL502: _temp501=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp489)->f1; goto _LL500; _LL500: _temp499=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp489)->f2; goto _LL492;} else{ goto _LL493;}
_LL493: if((( struct _enum_struct*) _temp489)->tag == Cyc_Absyn_Deref_e_tag){
_LL504: _temp503=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp489)->f1; goto _LL494;} else{ goto _LL495;} _LL495: if((( struct
_enum_struct*) _temp489)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL508: _temp507=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp489)->f1;
goto _LL506; _LL506: _temp505=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp489)->f2; goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL492:
Cyc_Evexp_eval_const_uint_exp( _temp499) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp509=( char*)"new_control_flow.cyc:414 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 61; _temp510;})); goto _LL494; _LL494: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp511=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp511[ 0]=({ struct Cyc_List_List
_temp512; _temp512.hd=( void*) Cyc_NewControlFlow_destruct_path( env, malloc_op);
_temp512.tl= 0; _temp512;}); _temp511;}); goto _LL490; _LL496: env.access_path=({
struct Cyc_List_List* _temp513=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp513[ 0]=({ struct Cyc_List_List _temp514;
_temp514.hd=( void*) Cyc_Absyn_structmember_exp( _temp507, _temp505, 0);
_temp514.tl=( env.access_path)->tl; _temp514;}); _temp513;}); ans.lvalues=({
struct Cyc_List_List* _temp515=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp515[ 0]=({ struct Cyc_List_List _temp516;
_temp516.hd=( void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp516.tl=
0; _temp516;}); _temp515;}); goto _LL490; _LL498:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp517=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp517->tag= Cyc_Core_Impossible_tag; _temp517->f1=( struct _tagged_string)({
char* _temp518=( char*)"bad malloc access path"; struct _tagged_string _temp519;
_temp519.curr= _temp518; _temp519.base= _temp518; _temp519.last_plus_one=
_temp518 + 23; _temp519;});( struct _xenum_struct*) _temp517;})); _LL490:;}
return ans;}}}} _LL470: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp520=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp520->hd=( void*) _temp301; _temp520->tl= 0;
_temp520;}), 0); _LL464:;} _LL156:{ void* _temp521=( void*) _temp303->r; struct
Cyc_Absyn_Structdecl* _temp529; struct Cyc_List_List* _temp531; struct Cyc_Core_Opt*
_temp533; struct _tuple1* _temp535; struct Cyc_List_List* _temp537; _LL523: if(((
struct _enum_struct*) _temp521)->tag == Cyc_Absyn_Struct_e_tag){ _LL536:
_temp535=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp521)->f1;
goto _LL534; _LL534: _temp533=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp521)->f2; goto _LL532; _LL532: _temp531=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp521)->f3; goto _LL530; _LL530: _temp529=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp521)->f4;
goto _LL524;} else{ goto _LL525;} _LL525: if((( struct _enum_struct*) _temp521)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL538: _temp537=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp521)->f1; goto _LL526;} else{ goto _LL527;}
_LL527: goto _LL528; _LL524: goto _LL526; _LL526: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp303->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp539=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp539[ 0]=({ struct Cyc_List_List _temp540;
_temp540.hd=( void*) malloc_op; _temp540.tl= 0; _temp540;}); _temp539;}));
inner_env.access_path= 0;{ void* _temp543; void* _temp545; struct Cyc_NewControlFlow_AbsSynOne
_temp541= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp303)); _LL546: _temp545=( void*) _temp541.inner_exp; goto _LL544;
_LL544: _temp543=( void*) _temp541.assigns; goto _LL542; _LL542: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp545, _temp543); if( env.access_path
!= 0){ void* _temp547=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp557; struct Cyc_Absyn_Exp* _temp559; struct Cyc_Absyn_Exp*
_temp561; struct _tagged_string* _temp563; struct Cyc_Absyn_Exp* _temp565;
_LL549: if((( struct _enum_struct*) _temp547)->tag == Cyc_Absyn_Subscript_e_tag){
_LL560: _temp559=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp547)->f1; goto _LL558; _LL558: _temp557=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp547)->f2; goto _LL550;} else{ goto _LL551;}
_LL551: if((( struct _enum_struct*) _temp547)->tag == Cyc_Absyn_Deref_e_tag){
_LL562: _temp561=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp547)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if((( struct
_enum_struct*) _temp547)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL566: _temp565=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp547)->f1;
goto _LL564; _LL564: _temp563=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp547)->f2; goto _LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL550:
Cyc_Evexp_eval_const_uint_exp( _temp557) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp567=( char*)"new_control_flow.cyc:450 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp568; _temp568.curr= _temp567; _temp568.base= _temp567;
_temp568.last_plus_one= _temp567 + 61; _temp568;})); goto _LL552; _LL552: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp569=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp569[ 0]=({ struct Cyc_List_List
_temp570; _temp570.hd=( void*) Cyc_NewControlFlow_destruct_path( env, malloc_op);
_temp570.tl= 0; _temp570;}); _temp569;}); goto _LL548; _LL554: env.access_path=({
struct Cyc_List_List* _temp571=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp571[ 0]=({ struct Cyc_List_List _temp572;
_temp572.hd=( void*) Cyc_Absyn_structmember_exp( _temp565, _temp563, 0);
_temp572.tl=( env.access_path)->tl; _temp572;}); _temp571;}); ans.lvalues=({
struct Cyc_List_List* _temp573=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp573[ 0]=({ struct Cyc_List_List _temp574;
_temp574.hd=( void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp574.tl=
0; _temp574;}); _temp573;}); goto _LL548; _LL556:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp575=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp575->tag= Cyc_Core_Impossible_tag; _temp575->f1=( struct _tagged_string)({
char* _temp576=( char*)"bad malloc access path"; struct _tagged_string _temp577;
_temp577.curr= _temp576; _temp577.base= _temp576; _temp577.last_plus_one=
_temp576 + 23; _temp577;});( struct _xenum_struct*) _temp575;})); _LL548:;}
return ans;}}}} _LL528: goto _LL522; _LL522:;}{ struct Cyc_List_List* _temp578=
env.access_path; struct Cyc_List_List _temp590; struct Cyc_List_List* _temp591;
struct Cyc_Absyn_Exp* _temp593; struct Cyc_Absyn_Exp _temp595; struct Cyc_Position_Segment*
_temp596; void* _temp598; struct Cyc_Absyn_Exp* _temp600; struct Cyc_Absyn_Exp*
_temp602; struct Cyc_Core_Opt* _temp604; struct Cyc_List_List _temp606; struct
Cyc_List_List* _temp607; struct Cyc_Absyn_Exp* _temp609; struct Cyc_Absyn_Exp
_temp611; struct Cyc_Position_Segment* _temp612; void* _temp614; struct Cyc_Absyn_Exp*
_temp616; struct Cyc_Core_Opt* _temp618; struct Cyc_List_List _temp620; struct
Cyc_List_List* _temp621; struct Cyc_Absyn_Exp* _temp623; struct Cyc_Absyn_Exp
_temp625; struct Cyc_Position_Segment* _temp626; void* _temp628; struct
_tagged_string* _temp630; struct Cyc_Absyn_Exp* _temp632; struct Cyc_Core_Opt*
_temp634; _LL580: if( _temp578 == 0){ goto _LL581;} else{ goto _LL582;} _LL582:
if( _temp578 == 0){ goto _LL584;} else{ _temp590=* _temp578; _LL594: _temp593=(
struct Cyc_Absyn_Exp*) _temp590.hd; _temp595=* _temp593; _LL605: _temp604=(
struct Cyc_Core_Opt*) _temp595.topt; goto _LL599; _LL599: _temp598=( void*)
_temp595.r; if((( struct _enum_struct*) _temp598)->tag == Cyc_Absyn_Subscript_e_tag){
_LL603: _temp602=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp598)->f1; goto _LL601; _LL601: _temp600=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp598)->f2; goto _LL597;} else{ goto _LL584;}
_LL597: _temp596=( struct Cyc_Position_Segment*) _temp595.loc; goto _LL592;
_LL592: _temp591=( struct Cyc_List_List*) _temp590.tl; goto _LL583;} _LL584: if(
_temp578 == 0){ goto _LL586;} else{ _temp606=* _temp578; _LL610: _temp609=(
struct Cyc_Absyn_Exp*) _temp606.hd; _temp611=* _temp609; _LL619: _temp618=(
struct Cyc_Core_Opt*) _temp611.topt; goto _LL615; _LL615: _temp614=( void*)
_temp611.r; if((( struct _enum_struct*) _temp614)->tag == Cyc_Absyn_Deref_e_tag){
_LL617: _temp616=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp614)->f1; goto _LL613;} else{ goto _LL586;} _LL613: _temp612=( struct Cyc_Position_Segment*)
_temp611.loc; goto _LL608; _LL608: _temp607=( struct Cyc_List_List*) _temp606.tl;
goto _LL585;} _LL586: if( _temp578 == 0){ goto _LL588;} else{ _temp620=*
_temp578; _LL624: _temp623=( struct Cyc_Absyn_Exp*) _temp620.hd; _temp625=*
_temp623; _LL635: _temp634=( struct Cyc_Core_Opt*) _temp625.topt; goto _LL629;
_LL629: _temp628=( void*) _temp625.r; if((( struct _enum_struct*) _temp628)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL633: _temp632=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp628)->f1; goto _LL631; _LL631:
_temp630=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp628)->f2; goto _LL627;} else{ goto _LL588;} _LL627: _temp626=( struct Cyc_Position_Segment*)
_temp625.loc; goto _LL622; _LL622: _temp621=( struct Cyc_List_List*) _temp620.tl;
goto _LL587;} _LL588: goto _LL589; _LL581: env.access_path=({ struct Cyc_List_List*
_temp636=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp636[ 0]=({ struct Cyc_List_List _temp637; _temp637.hd=( void*) e; _temp637.tl=
env.access_path; _temp637;}); _temp636;}); goto _LL579; _LL583: Cyc_Evexp_eval_const_uint_exp(
_temp600) ==( unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp638=( char*)"new_control_flow.cyc:478 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 61; _temp639;})); _temp607= _temp591; goto
_LL585; _LL585: env.access_path= _temp607; goto _LL579; _LL587: env.access_path=({
struct Cyc_List_List* _temp640=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp640[ 0]=({ struct Cyc_List_List _temp641;
_temp641.hd=( void*) Cyc_Absyn_structmember_exp( _temp632, _temp630, 0);
_temp641.tl= _temp621; _temp641;}); _temp640;}); goto _LL579; _LL589:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp642=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp642->tag= Cyc_Core_Impossible_tag;
_temp642->f1=( struct _tagged_string)({ char* _temp643=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp644; _temp644.curr= _temp643; _temp644.base= _temp643;
_temp644.last_plus_one= _temp643 + 36; _temp644;});( struct _xenum_struct*)
_temp642;})); _LL579:;} return Cyc_NewControlFlow_abstract_exp( env, _temp303);
_LL158: { struct Cyc_List_List* _temp645= env.access_path; struct Cyc_List_List
_temp651; struct Cyc_List_List* _temp652; struct Cyc_Absyn_Exp* _temp654; struct
Cyc_Absyn_Exp _temp656; struct Cyc_Position_Segment* _temp657; void* _temp659;
struct Cyc_Absyn_Exp* _temp661; struct Cyc_Core_Opt* _temp663; _LL647: if(
_temp645 == 0){ goto _LL649;} else{ _temp651=* _temp645; _LL655: _temp654=(
struct Cyc_Absyn_Exp*) _temp651.hd; _temp656=* _temp654; _LL664: _temp663=(
struct Cyc_Core_Opt*) _temp656.topt; goto _LL660; _LL660: _temp659=( void*)
_temp656.r; if((( struct _enum_struct*) _temp659)->tag == Cyc_Absyn_Address_e_tag){
_LL662: _temp661=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp659)->f1; goto _LL658;} else{ goto _LL649;} _LL658: _temp657=( struct Cyc_Position_Segment*)
_temp656.loc; goto _LL653; _LL653: _temp652=( struct Cyc_List_List*) _temp651.tl;
goto _LL648;} _LL649: goto _LL650; _LL648: env.access_path= _temp652;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp305);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL650: return Cyc_NewControlFlow_descend_path( env, e, _temp305); _LL646:;}
_LL160: { void* _temp665= Cyc_Tcutil_compress(( void*)( _temp309->topt)->v);
struct Cyc_Absyn_PtrInfo _temp671; struct Cyc_Absyn_Conref* _temp673; struct Cyc_Absyn_Tqual*
_temp675; struct Cyc_Absyn_Conref* _temp677; void* _temp679; void* _temp681;
_LL667: if(( unsigned int) _temp665 > 4u?(( struct _enum_struct*) _temp665)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL672: _temp671=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp665)->f1; _LL682: _temp681=( void*)
_temp671.elt_typ; goto _LL680; _LL680: _temp679=( void*) _temp671.rgn_typ; goto
_LL678; _LL678: _temp677=( struct Cyc_Absyn_Conref*) _temp671.nullable; goto
_LL676; _LL676: _temp675=( struct Cyc_Absyn_Tqual*) _temp671.tq; goto _LL674;
_LL674: _temp673=( struct Cyc_Absyn_Conref*) _temp671.bounds; goto _LL668;}
else{ goto _LL669;} _LL669: goto _LL670; _LL668:{ void* _temp683= Cyc_Tcutil_compress(
_temp681); struct Cyc_Absyn_Structdecl** _temp691; struct Cyc_List_List*
_temp693; struct _tuple1* _temp695; struct Cyc_Absyn_Uniondecl** _temp697;
struct Cyc_List_List* _temp699; struct _tuple1* _temp701; _LL685: if((
unsigned int) _temp683 > 4u?(( struct _enum_struct*) _temp683)->tag == Cyc_Absyn_StructType_tag:
0){ _LL696: _temp695=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp683)->f1; goto _LL694; _LL694: _temp693=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp683)->f2; goto _LL692; _LL692: _temp691=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp683)->f3;
goto _LL686;} else{ goto _LL687;} _LL687: if(( unsigned int) _temp683 > 4u?((
struct _enum_struct*) _temp683)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL702:
_temp701=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp683)->f1;
goto _LL700; _LL700: _temp699=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp683)->f2; goto _LL698; _LL698: _temp697=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp683)->f3; goto _LL688;} else{ goto
_LL689;} _LL689: goto _LL690; _LL686: return Cyc_NewControlFlow_descend_path(
env, e, _temp309); _LL688: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp309); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL690: goto _LL684; _LL684:;} goto _LL670; _LL670:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp703=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp703->tag= Cyc_Core_Impossible_tag;
_temp703->f1=( struct _tagged_string)({ char* _temp704=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp705; _temp705.curr= _temp704; _temp705.base= _temp704;
_temp705.last_plus_one= _temp704 + 40; _temp705;});( struct _xenum_struct*)
_temp703;})); _LL666:;} _LL162: { void* _temp706= Cyc_Tcutil_compress(( void*)(
_temp313->topt)->v); struct Cyc_Absyn_Structdecl** _temp714; struct Cyc_List_List*
_temp716; struct _tuple1* _temp718; struct Cyc_Absyn_Uniondecl** _temp720;
struct Cyc_List_List* _temp722; struct _tuple1* _temp724; _LL708: if((
unsigned int) _temp706 > 4u?(( struct _enum_struct*) _temp706)->tag == Cyc_Absyn_StructType_tag:
0){ _LL719: _temp718=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp706)->f1; goto _LL717; _LL717: _temp716=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp706)->f2; goto _LL715; _LL715: _temp714=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp706)->f3;
goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int) _temp706 > 4u?((
struct _enum_struct*) _temp706)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL725:
_temp724=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp706)->f1;
goto _LL723; _LL723: _temp722=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp706)->f2; goto _LL721; _LL721: _temp720=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp706)->f3; goto _LL711;} else{ goto
_LL712;} _LL712: goto _LL713; _LL709: return Cyc_NewControlFlow_descend_path(
env, e, _temp313); _LL711: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp313); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL713:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp726=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp726->tag= Cyc_Core_Impossible_tag; _temp726->f1=( struct _tagged_string)({
char* _temp727=( char*)"NewControlFlow: bad type in StructMember"; struct
_tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 41; _temp728;});( struct _xenum_struct*)
_temp726;})); _LL707:;} _LL164: { struct Cyc_List_List* _temp729= env.access_path;
struct Cyc_List_List _temp737; struct Cyc_List_List* _temp738; struct Cyc_Absyn_Exp*
_temp740; struct Cyc_Absyn_Exp _temp742; struct Cyc_Position_Segment* _temp743;
void* _temp745; struct Cyc_Absyn_Exp* _temp747; struct Cyc_Absyn_Exp* _temp749;
struct Cyc_Core_Opt* _temp751; _LL731: if( _temp729 == 0){ goto _LL732;} else{
goto _LL733;} _LL733: if( _temp729 == 0){ goto _LL735;} else{ _temp737=*
_temp729; _LL741: _temp740=( struct Cyc_Absyn_Exp*) _temp737.hd; _temp742=*
_temp740; _LL752: _temp751=( struct Cyc_Core_Opt*) _temp742.topt; goto _LL746;
_LL746: _temp745=( void*) _temp742.r; if((( struct _enum_struct*) _temp745)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL750: _temp749=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp745)->f1; goto _LL748; _LL748:
_temp747=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp745)->f2; goto _LL744;} else{ goto _LL735;} _LL744: _temp743=( struct Cyc_Position_Segment*)
_temp742.loc; goto _LL739; _LL739: _temp738=( struct Cyc_List_List*) _temp737.tl;
goto _LL734;} _LL735: goto _LL736; _LL732: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp315 != 0;
_temp315= _temp315->tl, ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp753[ 0]=({ struct Cyc_List_List
_temp754; _temp754.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp754.tl=
ith_lvals; _temp754;}); _temp753;});} else{ ith_lvals=({ struct Cyc_List_List*
_temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp755[ 0]=({ struct Cyc_List_List _temp756; _temp756.hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct* _temp757=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp757->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp757->f1= j;( void*) _temp757;})); _temp756.tl=
ith_lvals; _temp756;}); _temp755;});}}}{ struct Cyc_NewControlFlow_AbsSyn
ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
ith_lvals),( struct Cyc_Absyn_Exp*) _temp315->hd); void* _temp760; void*
_temp762; struct Cyc_NewControlFlow_AbsSynOne _temp758= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL763: _temp762=( void*) _temp758.inner_exp; goto _LL761; _LL761:
_temp760=( void*) _temp758.assigns; goto _LL759; _LL759: ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, ansexp, _temp762); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp760);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL734: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp747); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp738;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp315 != 0;
_temp315= _temp315->tl, ++ j){ if( i == j){ void* _temp766; void* _temp768;
struct Cyc_NewControlFlow_AbsSynOne _temp764= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*) _temp315->hd));
_LL769: _temp768=( void*) _temp764.inner_exp; goto _LL767; _LL767: _temp766=(
void*) _temp764.assigns; goto _LL765; _LL765: assignexp= _temp766; ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, ansexp, _temp768);} else{ void* _temp772; void*
_temp774; struct Cyc_NewControlFlow_AbsSynOne _temp770= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*) _temp315->hd));
_LL775: _temp774=( void*) _temp770.inner_exp; goto _LL773; _LL773: _temp772=(
void*) _temp770.assigns; goto _LL771; _LL771: ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, ansexp, _temp774);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp776=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp776[ 0]=({ struct Cyc_List_List
_temp777; _temp777.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp777.tl= 0;
_temp777;}); _temp776;}), assignexp);}} _LL736:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp778=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp778->tag= Cyc_Core_Impossible_tag; _temp778->f1=( struct _tagged_string)({
char* _temp779=( char*)"abstract_exp: unexpected access path to Tuple"; struct
_tagged_string _temp780; _temp780.curr= _temp779; _temp780.base= _temp779;
_temp780.last_plus_one= _temp779 + 46; _temp780;});( struct _xenum_struct*)
_temp778;})); _LL730:;} _LL166: { struct Cyc_List_List* _temp781= env.access_path;
struct Cyc_List_List _temp789; struct Cyc_List_List* _temp790; struct Cyc_Absyn_Exp*
_temp792; struct Cyc_Absyn_Exp _temp794; struct Cyc_Position_Segment* _temp795;
void* _temp797; struct _tagged_string* _temp799; struct Cyc_Absyn_Exp* _temp801;
struct Cyc_Core_Opt* _temp803; _LL783: if( _temp781 == 0){ goto _LL784;} else{
goto _LL785;} _LL785: if( _temp781 == 0){ goto _LL787;} else{ _temp789=*
_temp781; _LL793: _temp792=( struct Cyc_Absyn_Exp*) _temp789.hd; _temp794=*
_temp792; _LL804: _temp803=( struct Cyc_Core_Opt*) _temp794.topt; goto _LL798;
_LL798: _temp797=( void*) _temp794.r; if((( struct _enum_struct*) _temp797)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL802: _temp801=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp797)->f1; goto _LL800; _LL800:
_temp799=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp797)->f2; goto _LL796;} else{ goto _LL787;} _LL796: _temp795=( struct Cyc_Position_Segment*)
_temp794.loc; goto _LL791; _LL791: _temp790=( struct Cyc_List_List*) _temp789.tl;
goto _LL786;} _LL787: goto _LL788; _LL784: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp319 != 0; _temp319=
_temp319->tl){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls=
env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)
ls->hd)){ ith_lvals=({ struct Cyc_List_List* _temp805=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp805[ 0]=({ struct Cyc_List_List
_temp806; _temp806.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp806.tl=
ith_lvals; _temp806;}); _temp805;});} else{ struct Cyc_List_List* ds=(*(( struct
_tuple5*) _temp319->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp807=( void*)
ds->hd; struct Cyc_Absyn_Exp* _temp813; struct _tagged_string* _temp815; _LL809:
if((( struct _enum_struct*) _temp807)->tag == Cyc_Absyn_ArrayElement_tag){
_LL814: _temp813=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp807)->f1; goto _LL810;} else{ goto _LL811;} _LL811: if((( struct
_enum_struct*) _temp807)->tag == Cyc_Absyn_FieldName_tag){ _LL816: _temp815=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp807)->f1;
goto _LL812;} else{ goto _LL808;} _LL810:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp817=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp817->tag= Cyc_Core_Impossible_tag; _temp817->f1=( struct _tagged_string)({
char* _temp818=( char*)"bad struct designator"; struct _tagged_string _temp819;
_temp819.curr= _temp818; _temp819.base= _temp818; _temp819.last_plus_one=
_temp818 + 22; _temp819;});( struct _xenum_struct*) _temp817;})); _LL812:
ith_lvals=({ struct Cyc_List_List* _temp820=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp820[ 0]=({ struct Cyc_List_List
_temp821; _temp821.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct
Cyc_CfFlowInfo_StructF_struct* _temp822=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp822->tag= Cyc_CfFlowInfo_StructF_tag;
_temp822->f1= _temp815;( void*) _temp822;})); _temp821.tl= ith_lvals; _temp821;});
_temp820;}); goto _LL808; _LL808:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp319->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp319->hd)).f2); void* _temp825; void* _temp827; struct Cyc_NewControlFlow_AbsSynOne
_temp823= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL828: _temp827=( void*)
_temp823.inner_exp; goto _LL826; _LL826: _temp825=( void*) _temp823.assigns;
goto _LL824; _LL824: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp827); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp825);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL786: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp790;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp319 != 0; _temp319=
_temp319->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp319->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp829=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp835; struct _tagged_string* _temp837; _LL831: if(((
struct _enum_struct*) _temp829)->tag == Cyc_Absyn_ArrayElement_tag){ _LL836:
_temp835=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp829)->f1; goto _LL832;} else{ goto _LL833;} _LL833: if((( struct
_enum_struct*) _temp829)->tag == Cyc_Absyn_FieldName_tag){ _LL838: _temp837=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp829)->f1;
goto _LL834;} else{ goto _LL830;} _LL832:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp839=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp839->tag= Cyc_Core_Impossible_tag; _temp839->f1=( struct _tagged_string)({
char* _temp840=( char*)"bad struct designator"; struct _tagged_string _temp841;
_temp841.curr= _temp840; _temp841.base= _temp840; _temp841.last_plus_one=
_temp840 + 22; _temp841;});( struct _xenum_struct*) _temp839;})); _LL834: if(
Cyc_String_zstrptrcmp( _temp799, _temp837) == 0){ used= 1;} goto _LL830; _LL830:;}}
if( used){ void* _temp844; void* _temp846; struct Cyc_NewControlFlow_AbsSynOne
_temp842= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*) _temp319->hd)).f2)); _LL847: _temp846=( void*) _temp842.inner_exp;
goto _LL845; _LL845: _temp844=( void*) _temp842.assigns; goto _LL843; _LL843:
assignexp= _temp844; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp846);} else{ void* _temp850; void* _temp852; struct Cyc_NewControlFlow_AbsSynOne
_temp848= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp319->hd)).f2)); _LL853: _temp852=( void*)
_temp848.inner_exp; goto _LL851; _LL851: _temp850=( void*) _temp848.assigns;
goto _LL849; _LL849: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp852);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL788:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp854=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp854->tag= Cyc_Core_Impossible_tag; _temp854->f1=( struct _tagged_string)({
char* _temp855=( char*)"abstract_exp: unexpected access path to Struct"; struct
_tagged_string _temp856; _temp856.curr= _temp855; _temp856.base= _temp855;
_temp856.last_plus_one= _temp855 + 47; _temp856;});( struct _xenum_struct*)
_temp854;})); _LL782:;} _LL168: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp325), 0); _LL170: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp857=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp857[ 0]=({ struct Cyc_List_List
_temp858; _temp858.hd=( void*) _temp329; _temp858.tl= 0; _temp858;}); _temp857;}),
0); _LL172: { struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp331); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
esyn)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL174: { struct Cyc_List_List* _temp861; struct Cyc_NewControlFlow_AbsSynOne
_temp863; struct Cyc_NewControlFlow_AbsSynOne _temp865; struct Cyc_NewControlFlow_AbsSyn
_temp859= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp337); _LL866: _temp865=( struct Cyc_NewControlFlow_AbsSynOne) _temp859.when_true;
goto _LL864; _LL864: _temp863=( struct Cyc_NewControlFlow_AbsSynOne) _temp859.when_false;
goto _LL862; _LL862: _temp861=( struct Cyc_List_List*) _temp859.lvalues; goto
_LL860; _LL860: { struct Cyc_List_List* _temp869; struct Cyc_NewControlFlow_AbsSynOne
_temp871; struct Cyc_NewControlFlow_AbsSynOne _temp873; struct Cyc_NewControlFlow_AbsSyn
_temp867= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp335); _LL874: _temp873=( struct Cyc_NewControlFlow_AbsSynOne) _temp867.when_true;
goto _LL872; _LL872: _temp871=( struct Cyc_NewControlFlow_AbsSynOne) _temp867.when_false;
goto _LL870; _LL870: _temp869=( struct Cyc_List_List*) _temp867.lvalues; goto
_LL868; _LL868: { struct Cyc_List_List* _temp877; struct Cyc_NewControlFlow_AbsSynOne
_temp879; struct Cyc_NewControlFlow_AbsSynOne _temp881; struct Cyc_NewControlFlow_AbsSyn
_temp875= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp333); _LL882: _temp881=( struct Cyc_NewControlFlow_AbsSynOne) _temp875.when_true;
goto _LL880; _LL880: _temp879=( struct Cyc_NewControlFlow_AbsSynOne) _temp875.when_false;
goto _LL878; _LL878: _temp877=( struct Cyc_List_List*) _temp875.lvalues; goto
_LL876; _LL876: { void* true_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp865.inner_exp,( void*) _temp865.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp873.inner_exp,( void*) _temp873.assigns)),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp863.inner_exp,( void*) _temp863.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp881.inner_exp,( void*) _temp881.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp865.inner_exp,( void*) _temp865.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp871.inner_exp,( void*) _temp871.assigns)), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp863.inner_exp,( void*) _temp863.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp879.inner_exp,( void*) _temp879.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp883; _temp883.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp885;
_temp885.inner_exp=( void*) true_exp; _temp885.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp885;}); _temp883.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp884; _temp884.inner_exp=( void*) false_exp; _temp884.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp884;}); _temp883.lvalues= 0; _temp883;});}}}}
_LL176: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp339);{
struct Cyc_Absyn_Stmt* last_s= _temp339; while( 1) { void* _temp886=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp896; struct Cyc_Absyn_Stmt* _temp898;
struct Cyc_Absyn_Stmt* _temp900; struct Cyc_Absyn_Decl* _temp902; struct Cyc_Absyn_Exp*
_temp904; _LL888: if(( unsigned int) _temp886 > 1u?(( struct _enum_struct*)
_temp886)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL899: _temp898=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp886)->f1; goto _LL897; _LL897: _temp896=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp886)->f2; goto
_LL889;} else{ goto _LL890;} _LL890: if(( unsigned int) _temp886 > 1u?(( struct
_enum_struct*) _temp886)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL903: _temp902=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp886)->f1; goto
_LL901; _LL901: _temp900=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp886)->f2; goto _LL891;} else{ goto _LL892;} _LL892: if(( unsigned int)
_temp886 > 1u?(( struct _enum_struct*) _temp886)->tag == Cyc_Absyn_Exp_s_tag: 0){
_LL905: _temp904=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp886)->f1; goto _LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL889:
last_s= _temp896; continue; _LL891: last_s= _temp900; continue; _LL893: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp904); void* _temp908; void* _temp910; struct Cyc_NewControlFlow_AbsSynOne
_temp906= Cyc_NewControlFlow_meet_absexp( syn); _LL911: _temp910=( void*)
_temp906.inner_exp; goto _LL909; _LL909: _temp908=( void*) _temp906.assigns;
goto _LL907; _LL907:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp910, _temp908));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp339), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL895:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp912=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp912->tag= Cyc_Core_Impossible_tag; _temp912->f1=( struct _tagged_string)({
char* _temp913=( char*)"abstract_exp: ill-formed StmtExp"; struct _tagged_string
_temp914; _temp914.curr= _temp913; _temp914.base= _temp913; _temp914.last_plus_one=
_temp913 + 33; _temp914;});( struct _xenum_struct*) _temp912;})); _LL887:;}}}
_LL178: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp343); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp341); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp915; _temp915.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp917; _temp917.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp917.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp917;});
_temp915.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp916; _temp916.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp916.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp916;}); _temp915.lvalues= 0; _temp915;});}} _LL180: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp918=( char*)"new_control_flow.cyc:745 env.access_path==null";
struct _tagged_string _temp919; _temp919.curr= _temp918; _temp919.base= _temp918;
_temp919.last_plus_one= _temp918 + 47; _temp919;})); Cyc_NewControlFlow_add_var_root(
env, _temp349);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp920=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp920[ 0]=({ struct Cyc_List_List _temp921; _temp921.hd=( void*) _temp349;
_temp921.tl=( env.shared)->comprehension_vars; _temp921;}); _temp920;});{ struct
Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env( env); void*
_temp924; void* _temp926; struct Cyc_NewControlFlow_AbsSynOne _temp922= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp347)); _LL927: _temp926=( void*)
_temp922.inner_exp; goto _LL925; _LL925: _temp924=( void*) _temp922.assigns;
goto _LL923; _LL923: { void* _temp930; void* _temp932; struct Cyc_NewControlFlow_AbsSynOne
_temp928= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp345)); _LL933: _temp932=( void*) _temp928.inner_exp; goto _LL931;
_LL931: _temp930=( void*) _temp928.assigns; goto _LL929; _LL929: { void* ansexp=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp926, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, _temp924, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp349), Cyc_CfAbsexp_mkUnknownOp()),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp932, _temp930), Cyc_CfAbsexp_mkSkipAE())))); return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}}}
_LL182: goto _LL184; _LL184: goto _LL186; _LL186: goto _LL188; _LL188: goto
_LL190; _LL190: goto _LL192; _LL192: goto _LL194; _LL194:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp934=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp934->tag= Cyc_Core_Impossible_tag;
_temp934->f1=( struct _tagged_string)({ char* _temp935=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 34; _temp936;});( struct _xenum_struct*)
_temp934;})); _LL108:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp939; void* _temp941; struct Cyc_NewControlFlow_AbsSynOne
_temp937= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL942: _temp941=( void*) _temp937.inner_exp; goto _LL940; _LL940: _temp939=(
void*) _temp937.assigns; goto _LL938; _LL938: return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp941, _temp939);} struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp943; _temp943.f1= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns); _temp943.f2=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp943;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp946;
struct Cyc_Position_Segment* _temp947; struct Cyc_Absyn_Stmt* _temp949; struct
Cyc_Absyn_Exp* _temp951; struct Cyc_Core_Opt* _temp953; struct Cyc_Absyn_Pat*
_temp955; struct Cyc_Absyn_Switch_clause* _temp944=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp946=* _temp944; _LL956: _temp955=( struct Cyc_Absyn_Pat*) _temp946.pattern;
goto _LL954; _LL954: _temp953=( struct Cyc_Core_Opt*) _temp946.pat_vars; goto
_LL952; _LL952: _temp951=( struct Cyc_Absyn_Exp*) _temp946.where_clause; goto
_LL950; _LL950: _temp949=( struct Cyc_Absyn_Stmt*) _temp946.body; goto _LL948;
_LL948: _temp947=( struct Cyc_Position_Segment*) _temp946.loc; goto _LL945;
_LL945: if( _temp953 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp957=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp957->tag= Cyc_Core_Impossible_tag; _temp957->f1=( struct _tagged_string)({
char* _temp958=( char*)"switch clause vds not set"; struct _tagged_string
_temp959; _temp959.curr= _temp958; _temp959.base= _temp958; _temp959.last_plus_one=
_temp958 + 26; _temp959;});( struct _xenum_struct*) _temp957;}));}{ struct Cyc_List_List*
vds=( struct Cyc_List_List*) _temp953->v; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp951 != 0){ void* _temp963;
void* _temp965; struct _tuple0 _temp961= Cyc_NewControlFlow_abstract_guard( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp960= _temp951; if( _temp960
== 0){ _throw( Null_Exception);} _temp960;})); _LL966: _temp965= _temp961.f1;
goto _LL964; _LL964: _temp963= _temp961.f2; goto _LL962; _LL962: ans=({ struct
Cyc_List_List* _temp967=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp967[ 0]=({ struct Cyc_List_List _temp968; _temp968.hd=( void*)
_temp965; _temp968.tl=({ struct Cyc_List_List* _temp969=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp969[ 0]=({ struct Cyc_List_List
_temp970; _temp970.hd=( void*) _temp963; _temp970.tl= ans; _temp970;}); _temp969;});
_temp968;}); _temp967;});} Cyc_NewControlFlow_cf_prepass( env, _temp949);}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp971=( char*)"new_control_flow.cyc:797 env.lhsides == null";
struct _tagged_string _temp972; _temp972.curr= _temp971; _temp972.base= _temp971;
_temp972.last_plus_one= _temp971 + 45; _temp972;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp973=( char*)"new_control_flow.cyc:798 env.access_path == null";
struct _tagged_string _temp974; _temp974.curr= _temp973; _temp974.base= _temp973;
_temp974.last_plus_one= _temp973 + 49; _temp974;}));{ void* _temp975=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1021; struct Cyc_Absyn_Stmt* _temp1023; struct Cyc_Absyn_Stmt*
_temp1025; struct Cyc_Absyn_Exp* _temp1027; struct Cyc_Absyn_Exp* _temp1029;
struct Cyc_Absyn_Stmt* _temp1031; struct Cyc_Absyn_Stmt* _temp1033; struct Cyc_Absyn_Exp*
_temp1035; struct _tuple3 _temp1037; struct Cyc_Absyn_Stmt* _temp1039; struct
Cyc_Absyn_Exp* _temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct Cyc_Absyn_Stmt*
_temp1045; struct _tuple3 _temp1047; struct Cyc_Absyn_Stmt* _temp1049; struct
Cyc_Absyn_Exp* _temp1051; struct Cyc_Absyn_Stmt* _temp1053; struct Cyc_Absyn_Stmt*
_temp1055; struct Cyc_Absyn_Stmt* _temp1057; struct _tagged_string* _temp1059;
struct Cyc_Absyn_Stmt* _temp1061; struct _tagged_string* _temp1063; struct Cyc_Absyn_Stmt*
_temp1065; struct _tuple3 _temp1067; struct Cyc_Absyn_Stmt* _temp1069; struct
Cyc_Absyn_Exp* _temp1071; struct _tuple3 _temp1073; struct Cyc_Absyn_Stmt*
_temp1075; struct Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_Exp* _temp1079;
struct Cyc_List_List* _temp1081; struct Cyc_Absyn_Exp* _temp1083; struct Cyc_Absyn_Switch_clause**
_temp1085; struct Cyc_List_List* _temp1087; struct Cyc_Absyn_Stmt* _temp1089;
struct Cyc_Absyn_Decl* _temp1091; struct Cyc_Absyn_Decl _temp1093; struct Cyc_Position_Segment*
_temp1094; void* _temp1096; struct Cyc_Absyn_Vardecl* _temp1098; struct Cyc_Absyn_Stmt*
_temp1100; struct Cyc_Absyn_Decl* _temp1102; struct Cyc_Absyn_Decl _temp1104;
struct Cyc_Position_Segment* _temp1105; void* _temp1107; int _temp1109; struct
Cyc_Absyn_Exp* _temp1111; struct Cyc_Core_Opt* _temp1113; struct Cyc_Core_Opt*
_temp1115; struct Cyc_Core_Opt _temp1117; struct Cyc_List_List* _temp1118;
struct Cyc_Absyn_Pat* _temp1120; struct Cyc_Absyn_Stmt* _temp1122; struct Cyc_Absyn_Decl*
_temp1124; struct Cyc_Absyn_Stmt* _temp1126; struct Cyc_Absyn_Stmt* _temp1128;
struct Cyc_Absyn_Stmt* _temp1130; struct _tagged_string* _temp1132; struct Cyc_List_List*
_temp1134; struct Cyc_Absyn_Stmt* _temp1136; _LL977: if( _temp975 == Cyc_Absyn_Skip_s){
goto _LL978;} else{ goto _LL979;} _LL979: if(( unsigned int) _temp975 > 1u?((
struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1022:
_temp1021=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp975)->f1;
goto _LL980;} else{ goto _LL981;} _LL981: if(( unsigned int) _temp975 > 1u?((
struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1026:
_temp1025=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp975)->f1;
goto _LL1024; _LL1024: _temp1023=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp975)->f2; goto _LL982;} else{ goto _LL983;} _LL983: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1028: _temp1027=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp975)->f1; if( _temp1027 == 0){ goto _LL984;} else{ goto _LL985;}} else{
goto _LL985;} _LL985: if(( unsigned int) _temp975 > 1u?(( struct _enum_struct*)
_temp975)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1030: _temp1029=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp975)->f1; goto _LL986;} else{ goto
_LL987;} _LL987: if(( unsigned int) _temp975 > 1u?(( struct _enum_struct*)
_temp975)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1036: _temp1035=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp975)->f1; goto
_LL1034; _LL1034: _temp1033=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp975)->f2; goto _LL1032; _LL1032: _temp1031=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp975)->f3; goto _LL988;} else{ goto
_LL989;} _LL989: if(( unsigned int) _temp975 > 1u?(( struct _enum_struct*)
_temp975)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1044: _temp1043=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp975)->f1; goto _LL1038; _LL1038: _temp1037=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp975)->f2; _LL1042:
_temp1041= _temp1037.f1; goto _LL1040; _LL1040: _temp1039= _temp1037.f2; goto
_LL990;} else{ goto _LL991;} _LL991: if(( unsigned int) _temp975 > 1u?(( struct
_enum_struct*) _temp975)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1048: _temp1047=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp975)->f1; _LL1052:
_temp1051= _temp1047.f1; goto _LL1050; _LL1050: _temp1049= _temp1047.f2; goto
_LL1046; _LL1046: _temp1045=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp975)->f2; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL1054: _temp1053=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp975)->f1; goto _LL994;} else{ goto _LL995;} _LL995: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1056: _temp1055=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp975)->f1; goto _LL996;} else{ goto _LL997;} _LL997: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Goto_s_tag: 0){
_LL1060: _temp1059=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp975)->f1; goto _LL1058; _LL1058: _temp1057=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp975)->f2; if( _temp1057 == 0){ goto _LL998;}
else{ goto _LL999;}} else{ goto _LL999;} _LL999: if(( unsigned int) _temp975 > 1u?((
struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1064:
_temp1063=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp975)->f1;
goto _LL1062; _LL1062: _temp1061=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp975)->f2; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_For_s_tag: 0){
_LL1080: _temp1079=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp975)->f1; goto _LL1074; _LL1074: _temp1073=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp975)->f2; _LL1078: _temp1077= _temp1073.f1; goto _LL1076; _LL1076:
_temp1075= _temp1073.f2; goto _LL1068; _LL1068: _temp1067=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp975)->f3; _LL1072: _temp1071= _temp1067.f1;
goto _LL1070; _LL1070: _temp1069= _temp1067.f2; goto _LL1066; _LL1066: _temp1065=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp975)->f4; goto
_LL1002;} else{ goto _LL1003;} _LL1003: if(( unsigned int) _temp975 > 1u?((
struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1084:
_temp1083=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp975)->f1;
goto _LL1082; _LL1082: _temp1081=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp975)->f2; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL1088: _temp1087=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp975)->f1; goto _LL1086; _LL1086: _temp1085=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp975)->f2; goto _LL1006;} else{ goto
_LL1007;} _LL1007: if(( unsigned int) _temp975 > 1u?(( struct _enum_struct*)
_temp975)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1092: _temp1091=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp975)->f1; _temp1093=* _temp1091; _LL1097:
_temp1096=( void*) _temp1093.r; if((( struct _enum_struct*) _temp1096)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1099: _temp1098=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1096)->f1; goto _LL1095;} else{ goto _LL1009;}
_LL1095: _temp1094=( struct Cyc_Position_Segment*) _temp1093.loc; goto _LL1090;
_LL1090: _temp1089=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp975)->f2; goto _LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL1103: _temp1102=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp975)->f1; _temp1104=* _temp1102; _LL1108: _temp1107=( void*) _temp1104.r;
if((( struct _enum_struct*) _temp1107)->tag == Cyc_Absyn_Let_d_tag){ _LL1121:
_temp1120=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1107)->f1;
goto _LL1116; _LL1116: _temp1115=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1107)->f2; if( _temp1115 == 0){ goto _LL1011;} else{ _temp1117=* _temp1115;
_LL1119: _temp1118=( struct Cyc_List_List*) _temp1117.v; goto _LL1114;} _LL1114:
_temp1113=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1107)->f3;
goto _LL1112; _LL1112: _temp1111=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1107)->f4; goto _LL1110; _LL1110: _temp1109=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1107)->f5; goto _LL1106;} else{ goto _LL1011;} _LL1106: _temp1105=( struct
Cyc_Position_Segment*) _temp1104.loc; goto _LL1101; _LL1101: _temp1100=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp975)->f2; goto _LL1010;}
else{ goto _LL1011;} _LL1011: if(( unsigned int) _temp975 > 1u?(( struct
_enum_struct*) _temp975)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1125: _temp1124=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp975)->f1; goto
_LL1123; _LL1123: _temp1122=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp975)->f2; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Cut_s_tag: 0){
_LL1127: _temp1126=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp975)->f1; goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1129: _temp1128=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp975)->f1; goto _LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int)
_temp975 > 1u?(( struct _enum_struct*) _temp975)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1133: _temp1132=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp975)->f1; goto _LL1131; _LL1131: _temp1130=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp975)->f2; goto _LL1018;} else{ goto
_LL1019;} _LL1019: if(( unsigned int) _temp975 > 1u?(( struct _enum_struct*)
_temp975)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1137: _temp1136=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp975)->f1; goto _LL1135; _LL1135:
_temp1134=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp975)->f2; goto _LL1020;} else{ goto _LL976;} _LL978: return; _LL980: { void*
ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1021); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1138=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1138[ 0]=({ struct Cyc_List_List _temp1139;
_temp1139.hd=( void*) ae; _temp1139.tl= 0; _temp1139;}); _temp1138;})); return;}
_LL982: Cyc_NewControlFlow_cf_prepass( env, _temp1025); Cyc_NewControlFlow_cf_prepass(
env, _temp1023); return; _LL984: return; _LL986: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1140= _temp1029; if( _temp1140 == 0){ _throw( Null_Exception);} _temp1140;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1141=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1141[ 0]=({
struct Cyc_List_List _temp1142; _temp1142.hd=( void*) ae; _temp1142.tl= 0;
_temp1142;}); _temp1141;})); return;} _LL988: { void* _temp1145; void* _temp1147;
struct _tuple0 _temp1143= Cyc_NewControlFlow_abstract_guard( env, _temp1035);
_LL1148: _temp1147= _temp1143.f1; goto _LL1146; _LL1146: _temp1145= _temp1143.f2;
goto _LL1144; _LL1144: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1149=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1149[ 0]=({ struct Cyc_List_List _temp1150; _temp1150.hd=( void*) _temp1147;
_temp1150.tl=({ struct Cyc_List_List* _temp1151=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1151[ 0]=({ struct Cyc_List_List
_temp1152; _temp1152.hd=( void*) _temp1145; _temp1152.tl= 0; _temp1152;});
_temp1151;}); _temp1150;}); _temp1149;})); Cyc_NewControlFlow_cf_prepass( env,
_temp1033); Cyc_NewControlFlow_cf_prepass( env, _temp1031); return;} _LL990:
_temp1051= _temp1041; _temp1049= _temp1039; _temp1045= _temp1043; goto _LL992;
_LL992: { void* _temp1155; void* _temp1157; struct _tuple0 _temp1153= Cyc_NewControlFlow_abstract_guard(
env, _temp1051); _LL1158: _temp1157= _temp1153.f1; goto _LL1156; _LL1156:
_temp1155= _temp1153.f2; goto _LL1154; _LL1154: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1159=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1159[ 0]=({ struct Cyc_List_List _temp1160;
_temp1160.hd=( void*) _temp1157; _temp1160.tl=({ struct Cyc_List_List* _temp1161=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1161[
0]=({ struct Cyc_List_List _temp1162; _temp1162.hd=( void*) _temp1155; _temp1162.tl=
0; _temp1162;}); _temp1161;}); _temp1160;}); _temp1159;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1045); return;} _LL994: return; _LL996: return; _LL998:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp1163=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1163->tag= Cyc_Core_Impossible_tag;
_temp1163->f1=( struct _tagged_string)({ char* _temp1164=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1165; _temp1165.curr= _temp1164; _temp1165.base=
_temp1164; _temp1165.last_plus_one= _temp1164 + 33; _temp1165;});( struct
_xenum_struct*) _temp1163;})); _LL1000: { struct Cyc_Absyn_Stmt* my_encloser=(
Cyc_NewControlFlow_get_stmt_annot( s))->encloser; struct Cyc_Absyn_Stmt*
dest_encloser=( Cyc_NewControlFlow_get_stmt_annot(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Absyn_Stmt* _temp1166= _temp1061; if( _temp1166 == 0){ _throw(
Null_Exception);} _temp1166;})))->encloser; while( dest_encloser != my_encloser) {
struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1167=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1168; _temp1168.curr= _temp1167; _temp1168.base=
_temp1167; _temp1168.last_plus_one= _temp1167 + 45; _temp1168;})); break;}
my_encloser= next_encloser;} return;} _LL1002: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1079); void* _temp1171; void* _temp1173; struct _tuple0 _temp1169= Cyc_NewControlFlow_abstract_guard(
env, _temp1077); _LL1174: _temp1173= _temp1169.f1; goto _LL1172; _LL1172:
_temp1171= _temp1169.f2; goto _LL1170; _LL1170: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1071); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1175=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1175[ 0]=({ struct Cyc_List_List _temp1176; _temp1176.hd=( void*) ae1;
_temp1176.tl=({ struct Cyc_List_List* _temp1177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1177[ 0]=({ struct Cyc_List_List
_temp1178; _temp1178.hd=( void*) _temp1173; _temp1178.tl=({ struct Cyc_List_List*
_temp1179=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1179[ 0]=({ struct Cyc_List_List _temp1180; _temp1180.hd=( void*) _temp1171;
_temp1180.tl=({ struct Cyc_List_List* _temp1181=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1181[ 0]=({ struct Cyc_List_List
_temp1182; _temp1182.hd=( void*) ae3; _temp1182.tl= 0; _temp1182;}); _temp1181;});
_temp1180;}); _temp1179;}); _temp1178;}); _temp1177;}); _temp1176;}); _temp1175;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1065); return;}} _LL1004: { void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1083); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env,
_temp1081); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1183=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1183[ 0]=({ struct Cyc_List_List _temp1184; _temp1184.hd=( void*) ae;
_temp1184.tl= ael; _temp1184;}); _temp1183;})); return;} _LL1006: { void*
_temp1187; void* _temp1189; struct Cyc_NewControlFlow_AbsSynOne _temp1185= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_use_exp_unordered( env, _temp1087, 0)); _LL1190: _temp1189=(
void*) _temp1185.inner_exp; goto _LL1188; _LL1188: _temp1187=( void*) _temp1185.assigns;
goto _LL1186; _LL1186: { void* ae= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp1189, _temp1187); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1191=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1191[ 0]=({ struct Cyc_List_List _temp1192; _temp1192.hd=( void*) ae;
_temp1192.tl= 0; _temp1192;}); _temp1191;})); return;}} _LL1008: Cyc_NewControlFlow_add_var_root(
env, _temp1098); if( _temp1098->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1193=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1193[
0]=({ struct Cyc_List_List _temp1194; _temp1194.hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp1098); _temp1194.tl= 0; _temp1194;}); _temp1193;})); void* ae= Cyc_NewControlFlow_abstract_exp_top(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1195= _temp1098->initializer;
if( _temp1195 == 0){ _throw( Null_Exception);} _temp1195;})); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1196=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1196[ 0]=({ struct Cyc_List_List _temp1197;
_temp1197.hd=( void*) ae; _temp1197.tl= 0; _temp1197;}); _temp1196;}));} Cyc_NewControlFlow_cf_prepass(
env, _temp1089); return; _LL1010: for( 0; _temp1118 != 0; _temp1118= _temp1118->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*) _temp1118->hd);}{
void* ae= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp1111); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1198=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1198[ 0]=({ struct Cyc_List_List _temp1199; _temp1199.hd=( void*) ae;
_temp1199.tl= 0; _temp1199;}); _temp1198;})); Cyc_NewControlFlow_cf_prepass( env,
_temp1100); return;} _LL1012: goto _LL1014; _LL1014: goto _LL1016; _LL1016:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1200=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1200->tag= Cyc_Core_Impossible_tag;
_temp1200->f1=( struct _tagged_string)({ char* _temp1201=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1202; _temp1202.curr= _temp1201; _temp1202.base=
_temp1201; _temp1202.last_plus_one= _temp1201 + 26; _temp1202;});( struct
_xenum_struct*) _temp1200;})); _LL1018: Cyc_NewControlFlow_cf_prepass( env,
_temp1130); return; _LL1020: Cyc_NewControlFlow_cf_prepass( env, _temp1136);{
struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1134);
Cyc_NewControlFlow_cf_set_absexps( s, ael); return;} _LL976:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1203=( char*)"new_control_flow.cyc:926 annot->visited == iteration_num";
struct _tagged_string _temp1204; _temp1204.curr= _temp1203; _temp1204.base=
_temp1203; _temp1204.last_plus_one= _temp1203 + 57; _temp1204;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1205= flow; struct Cyc_Dict_Dict* _temp1211; _LL1207: if( _temp1205
== Cyc_CfFlowInfo_BottomFL){ goto _LL1208;} else{ goto _LL1209;} _LL1209: if((
unsigned int) _temp1205 > 1u?(( struct _enum_struct*) _temp1205)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1212: _temp1211=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1205)->f1; goto _LL1210;} else{ goto _LL1206;} _LL1208: return Cyc_CfFlowInfo_BottomFL;
_LL1210: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1213=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1213->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1213->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1213;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots, root)); _temp1211=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1211, root, pinfo);} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1214=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1214->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1214->f1= _temp1211;( void*) _temp1214;}); _LL1206:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1215= flow; struct Cyc_Dict_Dict* _temp1221; _LL1217: if( _temp1215
== Cyc_CfFlowInfo_BottomFL){ goto _LL1218;} else{ goto _LL1219;} _LL1219: if((
unsigned int) _temp1215 > 1u?(( struct _enum_struct*) _temp1215)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1222: _temp1221=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1215)->f1; goto _LL1220;} else{ goto _LL1216;} _LL1218: return Cyc_CfFlowInfo_BottomFL;
_LL1220: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1223=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1223->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1223->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1223;}); _temp1221=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1221, root,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots, root));} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1224=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1224->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1224->f1= _temp1221;( void*) _temp1224;}); _LL1216:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)
aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1225; _temp1225.f1=
out_flow; _temp1225.f2= out_flow; _temp1225;});} return({ struct _tuple0
_temp1226; _temp1226.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1226.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1226;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1229; struct Cyc_Position_Segment* _temp1230; struct Cyc_Absyn_Stmt*
_temp1232; struct Cyc_Absyn_Exp* _temp1234; struct Cyc_Core_Opt* _temp1236;
struct Cyc_Absyn_Pat* _temp1238; struct Cyc_Absyn_Switch_clause* _temp1227=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1229=* _temp1227; _LL1239:
_temp1238=( struct Cyc_Absyn_Pat*) _temp1229.pattern; goto _LL1237; _LL1237:
_temp1236=( struct Cyc_Core_Opt*) _temp1229.pat_vars; goto _LL1235; _LL1235:
_temp1234=( struct Cyc_Absyn_Exp*) _temp1229.where_clause; goto _LL1233; _LL1233:
_temp1232=( struct Cyc_Absyn_Stmt*) _temp1229.body; goto _LL1231; _LL1231:
_temp1230=( struct Cyc_Position_Segment*) _temp1229.loc; goto _LL1228; _LL1228: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1236->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1236->v);{ void* body_outflow;
if( _temp1234 != 0){ void* _temp1243; void* _temp1245; struct _tuple0 _temp1241=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1240= _temp1234; if( _temp1240 == 0){ _throw( Null_Exception);} _temp1240;}))->loc,
aes, body_inflow); _LL1246: _temp1245= _temp1241.f1; goto _LL1244; _LL1244:
_temp1243= _temp1241.f2; goto _LL1242; _LL1242: aes=( aes->tl)->tl; in_flow=
_temp1243; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1232,
_temp1245);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1232, body_inflow);}{ void* _temp1247= body_outflow; _LL1249: if( _temp1247
== Cyc_CfFlowInfo_BottomFL){ goto _LL1250;} else{ goto _LL1251;} _LL1251: goto
_LL1252; _LL1250: goto _LL1248; _LL1252: Cyc_Tcutil_terr( _temp1232->loc,(
struct _tagged_string)({ char* _temp1253=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1254; _temp1254.curr= _temp1253; _temp1254.base=
_temp1253; _temp1254.last_plus_one= _temp1253 + 38; _temp1254;})); goto _LL1248;
_LL1248:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1255=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1297; struct Cyc_Absyn_Exp* _temp1299; struct Cyc_Absyn_Exp*
_temp1301; struct Cyc_Absyn_Stmt* _temp1303; struct Cyc_Absyn_Stmt* _temp1305;
struct Cyc_Absyn_Stmt* _temp1307; struct Cyc_Absyn_Stmt* _temp1309; struct Cyc_Absyn_Exp*
_temp1311; struct Cyc_Absyn_Stmt* _temp1313; struct _tuple3 _temp1315; struct
Cyc_Absyn_Stmt* _temp1317; struct Cyc_Absyn_Exp* _temp1319; struct _tuple3
_temp1321; struct Cyc_Absyn_Stmt* _temp1323; struct Cyc_Absyn_Exp* _temp1325;
struct Cyc_Absyn_Stmt* _temp1327; struct Cyc_Absyn_Stmt* _temp1329; struct
_tuple3 _temp1331; struct Cyc_Absyn_Stmt* _temp1333; struct Cyc_Absyn_Exp*
_temp1335; struct _tuple3 _temp1337; struct Cyc_Absyn_Stmt* _temp1339; struct
Cyc_Absyn_Exp* _temp1341; struct Cyc_Absyn_Exp* _temp1343; struct Cyc_Absyn_Stmt*
_temp1345; struct Cyc_Absyn_Switch_clause** _temp1347; struct Cyc_Absyn_Switch_clause*
_temp1349; struct Cyc_List_List* _temp1350; struct Cyc_Absyn_Stmt* _temp1352;
struct Cyc_Absyn_Stmt* _temp1354; struct Cyc_Absyn_Stmt* _temp1356; struct
_tagged_string* _temp1358; struct Cyc_List_List* _temp1360; struct Cyc_Absyn_Exp*
_temp1362; struct Cyc_List_List* _temp1364; struct Cyc_Absyn_Stmt* _temp1366;
struct Cyc_Absyn_Stmt* _temp1368; struct Cyc_Absyn_Decl* _temp1370; struct Cyc_Absyn_Decl
_temp1372; struct Cyc_Position_Segment* _temp1373; void* _temp1375; struct Cyc_Absyn_Vardecl*
_temp1377; struct Cyc_Absyn_Stmt* _temp1379; struct Cyc_Absyn_Decl* _temp1381;
struct Cyc_Absyn_Decl _temp1383; struct Cyc_Position_Segment* _temp1384; void*
_temp1386; int _temp1388; struct Cyc_Absyn_Exp* _temp1390; struct Cyc_Core_Opt*
_temp1392; struct Cyc_Core_Opt* _temp1394; struct Cyc_Core_Opt _temp1396; struct
Cyc_List_List* _temp1397; struct Cyc_Absyn_Pat* _temp1399; struct Cyc_Absyn_Stmt*
_temp1401; struct _tagged_string* _temp1403; _LL1257: if( _temp1255 == Cyc_Absyn_Skip_s){
goto _LL1258;} else{ goto _LL1259;} _LL1259: if(( unsigned int) _temp1255 > 1u?((
struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1298:
_temp1297=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1255)->f1; if( _temp1297 == 0){ goto _LL1260;} else{ goto _LL1261;}} else{
goto _LL1261;} _LL1261: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1300: _temp1299=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1255)->f1; goto _LL1262;} else{ goto
_LL1263;} _LL1263: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1302: _temp1301=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp1255)->f1; goto _LL1264;} else{ goto
_LL1265;} _LL1265: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1306: _temp1305=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1255)->f1; goto _LL1304; _LL1304: _temp1303=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1255)->f2; goto
_LL1266;} else{ goto _LL1267;} _LL1267: if(( unsigned int) _temp1255 > 1u?((
struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1312:
_temp1311=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1255)->f1; goto _LL1310; _LL1310: _temp1309=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1255)->f2; goto _LL1308; _LL1308:
_temp1307=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1255)->f3; goto _LL1268;} else{ goto _LL1269;} _LL1269: if(( unsigned int)
_temp1255 > 1u?(( struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1316: _temp1315=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1255)->f1; _LL1320: _temp1319= _temp1315.f1; goto _LL1318; _LL1318:
_temp1317= _temp1315.f2; goto _LL1314; _LL1314: _temp1313=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1255)->f2; goto _LL1270;} else{ goto
_LL1271;} _LL1271: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1328: _temp1327=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1255)->f1; goto _LL1322; _LL1322: _temp1321=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1255)->f2; _LL1326:
_temp1325= _temp1321.f1; goto _LL1324; _LL1324: _temp1323= _temp1321.f2; goto
_LL1272;} else{ goto _LL1273;} _LL1273: if(( unsigned int) _temp1255 > 1u?((
struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1344:
_temp1343=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1255)->f1;
goto _LL1338; _LL1338: _temp1337=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1255)->f2; _LL1342: _temp1341= _temp1337.f1; goto _LL1340; _LL1340:
_temp1339= _temp1337.f2; goto _LL1332; _LL1332: _temp1331=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1255)->f3; _LL1336: _temp1335= _temp1331.f1;
goto _LL1334; _LL1334: _temp1333= _temp1331.f2; goto _LL1330; _LL1330: _temp1329=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1255)->f4; goto
_LL1274;} else{ goto _LL1275;} _LL1275: if(( unsigned int) _temp1255 > 1u?((
struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1346:
_temp1345=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1255)->f1; if( _temp1345 == 0){ goto _LL1276;} else{ goto _LL1277;}} else{
goto _LL1277;} _LL1277: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1351: _temp1350=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1255)->f1; goto _LL1348; _LL1348:
_temp1347=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1255)->f2; if( _temp1347 == 0){ goto _LL1279;} else{ _temp1349=* _temp1347;
goto _LL1278;}} else{ goto _LL1279;} _LL1279: if(( unsigned int) _temp1255 > 1u?((
struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1353:
_temp1352=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1255)->f1; goto _LL1280;} else{ goto _LL1281;} _LL1281: if(( unsigned int)
_temp1255 > 1u?(( struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1355: _temp1354=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1255)->f1; goto _LL1282;} else{ goto _LL1283;} _LL1283: if(( unsigned int)
_temp1255 > 1u?(( struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1359: _temp1358=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1255)->f1; goto _LL1357; _LL1357: _temp1356=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1255)->f2; goto _LL1284;} else{ goto
_LL1285;} _LL1285: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1363: _temp1362=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1255)->f1; goto _LL1361; _LL1361:
_temp1360=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1255)->f2; goto _LL1286;} else{ goto _LL1287;} _LL1287: if(( unsigned int)
_temp1255 > 1u?(( struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1367: _temp1366=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1255)->f1; goto _LL1365; _LL1365: _temp1364=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1255)->f2; goto _LL1288;} else{ goto
_LL1289;} _LL1289: if(( unsigned int) _temp1255 > 1u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1371: _temp1370=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1255)->f1; _temp1372=* _temp1370; _LL1376:
_temp1375=( void*) _temp1372.r; if((( struct _enum_struct*) _temp1375)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1378: _temp1377=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1375)->f1; goto _LL1374;} else{ goto _LL1291;}
_LL1374: _temp1373=( struct Cyc_Position_Segment*) _temp1372.loc; goto _LL1369;
_LL1369: _temp1368=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1255)->f2; goto _LL1290;} else{ goto _LL1291;} _LL1291: if(( unsigned int)
_temp1255 > 1u?(( struct _enum_struct*) _temp1255)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1382: _temp1381=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1255)->f1; _temp1383=* _temp1381; _LL1387: _temp1386=( void*) _temp1383.r;
if((( struct _enum_struct*) _temp1386)->tag == Cyc_Absyn_Let_d_tag){ _LL1400:
_temp1399=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1386)->f1;
goto _LL1395; _LL1395: _temp1394=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1386)->f2; if( _temp1394 == 0){ goto _LL1293;} else{ _temp1396=* _temp1394;
_LL1398: _temp1397=( struct Cyc_List_List*) _temp1396.v; goto _LL1393;} _LL1393:
_temp1392=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1386)->f3;
goto _LL1391; _LL1391: _temp1390=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1386)->f4; goto _LL1389; _LL1389: _temp1388=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1386)->f5; goto _LL1385;} else{ goto _LL1293;} _LL1385: _temp1384=( struct
Cyc_Position_Segment*) _temp1383.loc; goto _LL1380; _LL1380: _temp1379=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1255)->f2; goto _LL1292;}
else{ goto _LL1293;} _LL1293: if(( unsigned int) _temp1255 > 1u?(( struct
_enum_struct*) _temp1255)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1404: _temp1403=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1255)->f1; goto
_LL1402; _LL1402: _temp1401=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1255)->f2; goto _LL1294;} else{ goto _LL1295;} _LL1295: goto _LL1296;
_LL1258: return in_flow; _LL1260: return Cyc_CfFlowInfo_BottomFL; _LL1262: Cyc_CfAbsexp_eval_absexp(
env, _temp1299->loc,( void*) aes->hd, in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1264: return Cyc_CfAbsexp_eval_absexp( env, _temp1301->loc,( void*) aes->hd,
in_flow); _LL1266: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1303,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1305, in_flow)); _LL1268: { void*
_temp1407; void* _temp1409; struct _tuple0 _temp1405= Cyc_NewControlFlow_cf_evalguard(
env, _temp1311->loc, aes, in_flow); _LL1410: _temp1409= _temp1405.f1; goto
_LL1408; _LL1408: _temp1407= _temp1405.f2; goto _LL1406; _LL1406: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1309, _temp1409), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1307, _temp1407));} _LL1270: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1317))->flow; void* _temp1413; void* _temp1415; struct _tuple0
_temp1411= Cyc_NewControlFlow_cf_evalguard( env, _temp1319->loc, aes, e_inflow);
_LL1416: _temp1415= _temp1411.f1; goto _LL1414; _LL1414: _temp1413= _temp1411.f2;
goto _LL1412; _LL1412: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1313, _temp1415); Cyc_NewControlFlow_update_flow( _temp1317,
body_outflow); return _temp1413;}} _LL1272: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1327, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1323))->flow; void* _temp1419; void* _temp1421; struct
_tuple0 _temp1417= Cyc_NewControlFlow_cf_evalguard( env, _temp1325->loc, aes,
e_inflow); _LL1422: _temp1421= _temp1417.f1; goto _LL1420; _LL1420: _temp1419=
_temp1417.f2; goto _LL1418; _LL1418: Cyc_NewControlFlow_update_flow( _temp1327,
_temp1421); return _temp1419;} _LL1274: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1343->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1339))->flow; void* _temp1425; void* _temp1427; struct _tuple0
_temp1423= Cyc_NewControlFlow_cf_evalguard( env, _temp1341->loc, aes->tl,
e2_inflow); _LL1428: _temp1427= _temp1423.f1; goto _LL1426; _LL1426: _temp1425=
_temp1423.f2; goto _LL1424; _LL1424: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1329, _temp1427); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1333))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1335->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1339, e3_outflow); return _temp1425;}} _LL1276: return Cyc_CfFlowInfo_BottomFL;
_LL1278: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1349->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1352=( struct
Cyc_Absyn_Stmt*) _temp1349->body; goto _LL1280;} _LL1280: _temp1354= _temp1352;
goto _LL1282; _LL1282: _temp1356= _temp1354; goto _LL1284; _LL1284: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1429= _temp1356; if(
_temp1429 == 0){ _throw( Null_Exception);} _temp1429;}), in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1286: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1362->loc,( void*) aes->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1360, aes->tl,
in_flow); return Cyc_CfFlowInfo_BottomFL; _LL1288: { struct Cyc_NewControlFlow_AnalEnv
_temp1432; void* _temp1433; int _temp1435; struct Cyc_Dict_Dict* _temp1437;
struct Cyc_NewControlFlow_AnalEnv* _temp1430= env; _temp1432=* _temp1430;
_LL1438: _temp1437=( struct Cyc_Dict_Dict*) _temp1432.roots; goto _LL1436;
_LL1436: _temp1435=( int) _temp1432.in_try; goto _LL1434; _LL1434: _temp1433=(
void*) _temp1432.tryflow; goto _LL1431; _LL1431: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1366, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1435;( void*)( env->tryflow=( void*) _temp1433); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1364,
aes, scs_inflow); return s_outflow;}} _LL1290: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1439; _temp1439.hd=( void*) _temp1377; _temp1439.tl= 0;
_temp1439;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1377->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1440= e; if( _temp1440 == 0){ _throw( Null_Exception);}
_temp1440;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1368, body_inflow);} _LL1292: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1397); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1390->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1397); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1379, body_inflow);} _LL1294: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1401, in_flow); _LL1296:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1441=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1441->tag= Cyc_Core_Impossible_tag; _temp1441->f1=( struct _tagged_string)({
char* _temp1442=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1443; _temp1443.curr= _temp1442; _temp1443.base=
_temp1442; _temp1443.last_plus_one= _temp1442 + 62; _temp1443;});( struct
_xenum_struct*) _temp1441;})); _LL1256:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1444=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_EmptyAnnot_struct)); _temp1444->tag= Cyc_Absyn_EmptyAnnot_tag;(
struct _xenum_struct*) _temp1444;});{ void* _temp1445=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp1467; struct Cyc_Absyn_Stmt* _temp1469; struct Cyc_Absyn_Stmt* _temp1471;
struct Cyc_Absyn_Stmt* _temp1473; struct Cyc_Absyn_Exp* _temp1475; struct Cyc_Absyn_Stmt*
_temp1477; struct _tuple3 _temp1479; struct Cyc_Absyn_Stmt* _temp1481; struct
Cyc_Absyn_Exp* _temp1483; struct _tuple3 _temp1485; struct Cyc_Absyn_Stmt*
_temp1487; struct Cyc_Absyn_Exp* _temp1489; struct Cyc_Absyn_Exp* _temp1491;
struct Cyc_Absyn_Stmt* _temp1493; struct _tuple3 _temp1495; struct Cyc_Absyn_Stmt*
_temp1497; struct Cyc_Absyn_Exp* _temp1499; struct _tuple3 _temp1501; struct Cyc_Absyn_Stmt*
_temp1503; struct Cyc_Absyn_Exp* _temp1505; struct Cyc_Absyn_Stmt* _temp1507;
struct Cyc_Absyn_Stmt* _temp1509; struct _tagged_string* _temp1511; struct Cyc_Absyn_Stmt*
_temp1513; struct Cyc_Absyn_Decl* _temp1515; struct Cyc_List_List* _temp1517;
struct Cyc_Absyn_Stmt* _temp1519; struct Cyc_List_List* _temp1521; struct Cyc_Absyn_Exp*
_temp1523; _LL1447: if(( unsigned int) _temp1445 > 1u?(( struct _enum_struct*)
_temp1445)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1470: _temp1469=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1445)->f1; goto _LL1468; _LL1468: _temp1467=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1445)->f2; goto
_LL1448;} else{ goto _LL1449;} _LL1449: if(( unsigned int) _temp1445 > 1u?((
struct _enum_struct*) _temp1445)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1476:
_temp1475=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1445)->f1; goto _LL1474; _LL1474: _temp1473=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1445)->f2; goto _LL1472; _LL1472:
_temp1471=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1445)->f3; goto _LL1450;} else{ goto _LL1451;} _LL1451: if(( unsigned int)
_temp1445 > 1u?(( struct _enum_struct*) _temp1445)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1492: _temp1491=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1445)->f1; goto _LL1486; _LL1486: _temp1485=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1445)->f2; _LL1490: _temp1489= _temp1485.f1; goto _LL1488; _LL1488:
_temp1487= _temp1485.f2; goto _LL1480; _LL1480: _temp1479=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1445)->f3; _LL1484: _temp1483= _temp1479.f1;
goto _LL1482; _LL1482: _temp1481= _temp1479.f2; goto _LL1478; _LL1478: _temp1477=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1445)->f4; goto
_LL1452;} else{ goto _LL1453;} _LL1453: if(( unsigned int) _temp1445 > 1u?((
struct _enum_struct*) _temp1445)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1496:
_temp1495=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1445)->f1;
_LL1500: _temp1499= _temp1495.f1; goto _LL1498; _LL1498: _temp1497= _temp1495.f2;
goto _LL1494; _LL1494: _temp1493=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1445)->f2; goto _LL1454;} else{ goto _LL1455;} _LL1455: if(( unsigned int)
_temp1445 > 1u?(( struct _enum_struct*) _temp1445)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1508: _temp1507=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1445)->f1; goto _LL1502; _LL1502: _temp1501=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1445)->f2; _LL1506: _temp1505= _temp1501.f1; goto _LL1504; _LL1504:
_temp1503= _temp1501.f2; goto _LL1456;} else{ goto _LL1457;} _LL1457: if((
unsigned int) _temp1445 > 1u?(( struct _enum_struct*) _temp1445)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1512: _temp1511=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1445)->f1; goto _LL1510; _LL1510: _temp1509=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1445)->f2; goto _LL1458;} else{ goto
_LL1459;} _LL1459: if(( unsigned int) _temp1445 > 1u?(( struct _enum_struct*)
_temp1445)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1516: _temp1515=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1445)->f1; goto _LL1514; _LL1514:
_temp1513=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1445)->f2;
goto _LL1460;} else{ goto _LL1461;} _LL1461: if(( unsigned int) _temp1445 > 1u?((
struct _enum_struct*) _temp1445)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1520:
_temp1519=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1445)->f1; goto _LL1518; _LL1518: _temp1517=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1445)->f2; goto _LL1462;} else{ goto
_LL1463;} _LL1463: if(( unsigned int) _temp1445 > 1u?(( struct _enum_struct*)
_temp1445)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1524: _temp1523=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1445)->f1; goto _LL1522; _LL1522:
_temp1521=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1445)->f2; goto _LL1464;} else{ goto _LL1465;} _LL1465: goto _LL1466;
_LL1448: _temp1473= _temp1469; _temp1471= _temp1467; goto _LL1450; _LL1450: Cyc_NewControlFlow_cf_postpass(
_temp1473); Cyc_NewControlFlow_cf_postpass( _temp1471); return; _LL1452: Cyc_NewControlFlow_cf_postpass(
_temp1481); _temp1497= _temp1477; _temp1493= _temp1487; goto _LL1454; _LL1454:
_temp1507= _temp1493; _temp1503= _temp1497; goto _LL1456; _LL1456: Cyc_NewControlFlow_cf_postpass(
_temp1503); _temp1509= _temp1507; goto _LL1458; _LL1458: _temp1513= _temp1509;
goto _LL1460; _LL1460: Cyc_NewControlFlow_cf_postpass( _temp1513); return;
_LL1462: Cyc_NewControlFlow_cf_postpass( _temp1519); _temp1521= _temp1517; goto
_LL1464; _LL1464: for( 0; _temp1521 != 0; _temp1521= _temp1521->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1521->hd)->body);} return; _LL1466:
return; _LL1446:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1525; _temp1525.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1526=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv)
* 1); _temp1526[ 0]=({ struct Cyc_NewControlFlow_SharedAbsEnv _temp1527;
_temp1527.all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1527.comprehension_vars= 0;
_temp1527;}); _temp1526;}); _temp1525.lhsides= 0; _temp1525.access_path= 0;
_temp1525;}); struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1530; struct Cyc_List_List*
_temp1532; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1534; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1536; struct Cyc_List_List* _temp1537; struct Cyc_Dict_Dict* _temp1539;
struct Cyc_NewControlFlow_AbsEnv _temp1528= absenv; _LL1535: _temp1534=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1528.shared; _temp1536=* _temp1534;
_LL1540: _temp1539=( struct Cyc_Dict_Dict*) _temp1536.all_roots; goto _LL1538;
_LL1538: _temp1537=( struct Cyc_List_List*) _temp1536.comprehension_vars; goto
_LL1533; _LL1533: _temp1532=( struct Cyc_List_List*) _temp1528.lhsides; goto
_LL1531; _LL1531: _temp1530=( struct Cyc_List_List*) _temp1528.access_path; goto
_LL1529; _LL1529: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1541=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)
* 1); _temp1541[ 0]=({ struct Cyc_NewControlFlow_AnalEnv _temp1542; _temp1542.roots=
_temp1539; _temp1542.in_try= 0; _temp1542.tryflow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1542;}); _temp1541;}); void* in_flow=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1543=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1543->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp1543->f1=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);(
void*) _temp1543;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( analenv,
in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow( analenv,
in_flow, _temp1537);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1544= out_flow; _LL1546: if( _temp1544 == Cyc_CfFlowInfo_BottomFL){
goto _LL1547;} else{ goto _LL1548;} _LL1548: goto _LL1549; _LL1547: goto _LL1545;
_LL1549: if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp1550=( char*)"function may complete without returning a value";
struct _tagged_string _temp1551; _temp1551.curr= _temp1550; _temp1551.base=
_temp1550; _temp1551.last_plus_one= _temp1550 + 48; _temp1551;}));} goto _LL1545;
_LL1545:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1552=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1562; struct
Cyc_List_List* _temp1564; struct _tuple1* _temp1566; struct Cyc_List_List*
_temp1568; struct _tagged_string* _temp1570; _LL1554: if((( struct _enum_struct*)
_temp1552)->tag == Cyc_Absyn_Fn_d_tag){ _LL1563: _temp1562=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1552)->f1; goto _LL1555;} else{ goto _LL1556;}
_LL1556: if((( struct _enum_struct*) _temp1552)->tag == Cyc_Absyn_Using_d_tag){
_LL1567: _temp1566=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1552)->f1; goto _LL1565; _LL1565: _temp1564=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1552)->f2; goto _LL1557;} else{ goto
_LL1558;} _LL1558: if((( struct _enum_struct*) _temp1552)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1571: _temp1570=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1552)->f1; goto _LL1569; _LL1569: _temp1568=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1552)->f2; goto _LL1559;} else{ goto
_LL1560;} _LL1560: goto _LL1561; _LL1555: Cyc_NewControlFlow_cf_check_fun(
_temp1562); goto _LL1553; _LL1557: _temp1568= _temp1564; goto _LL1559; _LL1559:
Cyc_NewControlFlow_cf_check( _temp1568); goto _LL1553; _LL1561: goto _LL1553;
_LL1553:;}}