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
== 0){ Cyc_NewControlFlow_unknown_list=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp39=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp39[ 0]=({ struct Cyc_List_List _temp40; _temp40.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
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
lhsides != 0; lhsides= lhsides->tl){ l=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp60=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp60[ 0]=({ struct Cyc_List_List _temp61; _temp61.hd=( void*) Cyc_CfAbsexp_mkAssignAE((
void*) lhsides->hd, ae); _temp61.tl= l; _temp61;}); _temp60;});}} return Cyc_CfAbsexp_mkGroupAE_l(
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
inner_e){ env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp68=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp68[ 0]=({ struct Cyc_List_List _temp69; _temp69.hd=( void*) path_e; _temp69.tl=
env.access_path; _temp69;}); _temp68;});{ struct Cyc_NewControlFlow_AbsSyn syn1=
Cyc_NewControlFlow_abstract_exp( env, inner_e); if( syn1.lvalues == 0){ syn1.lvalues=
Cyc_NewControlFlow_use_it_list();} return syn1;}} void* Cyc_NewControlFlow_destruct_path(
struct Cyc_NewControlFlow_AbsEnv env, void* absop){{ struct Cyc_List_List* path=
env.access_path; for( 0; path != 0; path= path->tl){ void* _temp70=( void*)((
struct Cyc_Absyn_Exp*) path->hd)->r; struct Cyc_Absyn_Exp* _temp84; struct Cyc_Absyn_Exp*
_temp86; struct _tagged_string* _temp88; struct Cyc_Absyn_Exp* _temp90; struct
_tagged_string* _temp92; struct Cyc_Absyn_Exp* _temp94; struct Cyc_Absyn_Exp*
_temp96; struct Cyc_Absyn_Exp* _temp98; _LL72: if((( struct _enum_struct*)
_temp70)->tag == Cyc_Absyn_Address_e_tag){ _LL85: _temp84=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp70)->f1; goto _LL73;} else{ goto _LL74;}
_LL74: if((( struct _enum_struct*) _temp70)->tag == Cyc_Absyn_Deref_e_tag){
_LL87: _temp86=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp70)->f1; goto _LL75;} else{ goto _LL76;} _LL76: if((( struct _enum_struct*)
_temp70)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL91: _temp90=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp70)->f1; goto _LL89; _LL89: _temp88=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp70)->f2;
goto _LL77;} else{ goto _LL78;} _LL78: if((( struct _enum_struct*) _temp70)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL95: _temp94=( struct Cyc_Absyn_Exp*)((
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
void*) e->r; struct Cyc_Absyn_Exp* _temp193; struct Cyc_Absyn_Exp* _temp195;
struct Cyc_List_List* _temp197; struct Cyc_Absyn_Exp* _temp199; void* _temp201;
int _temp203; void* _temp205; void* _temp207; int _temp209; void* _temp211; void*
_temp213; void* _temp215; void* _temp217; struct Cyc_Absyn_Fndecl* _temp219;
struct _tuple1* _temp221; void* _temp223; struct Cyc_Absyn_Vardecl* _temp225;
struct _tuple1* _temp227; void* _temp229; struct Cyc_Absyn_Vardecl* _temp231;
struct _tuple1* _temp233; void* _temp235; struct Cyc_Absyn_Vardecl* _temp237;
struct _tuple1* _temp239; void* _temp241; struct Cyc_Absyn_Vardecl* _temp243;
struct _tuple1* _temp245; struct Cyc_Absyn_Enumfield* _temp247; struct Cyc_Absyn_Enumdecl*
_temp249; struct Cyc_List_List* _temp251; struct Cyc_Core_Opt* _temp253; struct
Cyc_Core_Opt* _temp255; struct Cyc_Absyn_Enumfield* _temp257; struct Cyc_Absyn_Xenumdecl*
_temp259; struct Cyc_List_List* _temp261; struct Cyc_Core_Opt* _temp263; void*
_temp265; void* _temp267; void* _temp269; struct Cyc_Absyn_Exp* _temp271; struct
Cyc_Core_Opt* _temp273; struct Cyc_Absyn_Exp* _temp275; struct Cyc_Absyn_Exp*
_temp277; struct Cyc_Core_Opt* _temp279; struct Cyc_Absyn_Exp* _temp281; struct
Cyc_List_List* _temp283; void* _temp285; struct Cyc_Absyn_Exp* _temp287; void*
_temp289; struct Cyc_List_List* _temp291; struct Cyc_Absyn_Exp* _temp293; struct
Cyc_Absyn_Exp* _temp295; struct Cyc_Absyn_Exp* _temp297; struct Cyc_Absyn_Exp*
_temp299; struct Cyc_Absyn_Exp* _temp301; struct _tagged_string* _temp303;
struct Cyc_Absyn_Exp* _temp305; struct _tagged_string* _temp307; struct Cyc_Absyn_Exp*
_temp309; struct Cyc_List_List* _temp311; struct Cyc_Absyn_Structdecl* _temp313;
struct Cyc_List_List* _temp315; struct Cyc_Core_Opt* _temp317; struct _tuple1*
_temp319; struct Cyc_List_List* _temp321; int _temp323; void* _temp325; struct
Cyc_Absyn_Exp* _temp327; struct Cyc_Absyn_Exp* _temp329; struct Cyc_Absyn_Exp*
_temp331; struct Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp* _temp335; struct
Cyc_Absyn_Stmt* _temp337; struct Cyc_Absyn_Exp* _temp339; struct Cyc_Absyn_Exp*
_temp341; struct Cyc_Absyn_Exp* _temp343; struct Cyc_Absyn_Exp* _temp345; struct
Cyc_Absyn_Vardecl* _temp347; void* _temp349; struct _tuple1* _temp351; struct
_tuple1* _temp353; struct Cyc_List_List* _temp355; struct Cyc_Absyn_Exp*
_temp357; struct Cyc_List_List* _temp359; struct Cyc_Core_Opt* _temp361; struct
Cyc_List_List* _temp363; struct _tuple2* _temp365; struct Cyc_Absyn_Fndecl*
_temp367; struct Cyc_Absyn_Exp* _temp369; _LL109: if((( struct _enum_struct*)
_temp107)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL194: _temp193=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp107)->f1; goto _LL110;} else{
goto _LL111;} _LL111: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL196: _temp195=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp107)->f1; goto _LL112;} else{ goto _LL113;} _LL113: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL200: _temp199=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp107)->f1;
goto _LL198; _LL198: _temp197=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp107)->f2; goto _LL114;} else{ goto _LL115;} _LL115: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Const_e_tag){ _LL202: _temp201=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp107)->f1; if(( unsigned int) _temp201 > 1u?((
struct _enum_struct*) _temp201)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL206:
_temp205=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp201)->f1; if( _temp205
== Cyc_Absyn_Signed){ goto _LL204;} else{ goto _LL117;} _LL204: _temp203=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp201)->f2; if( _temp203 == 0){ goto _LL116;}
else{ goto _LL117;}} else{ goto _LL117;}} else{ goto _LL117;} _LL117: if(((
struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Const_e_tag){ _LL208: _temp207=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp107)->f1; if(( unsigned int)
_temp207 > 1u?(( struct _enum_struct*) _temp207)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL212: _temp211=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp207)->f1; if(
_temp211 == Cyc_Absyn_Signed){ goto _LL210;} else{ goto _LL119;} _LL210:
_temp209=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp207)->f2; if( _temp209 ==
1){ goto _LL118;} else{ goto _LL119;}} else{ goto _LL119;}} else{ goto _LL119;}
_LL119: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL214: _temp213=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp107)->f1;
goto _LL120;} else{ goto _LL121;} _LL121: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Const_e_tag){ _LL216: _temp215=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp107)->f1; goto _LL122;} else{ goto _LL123;} _LL123: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){ _LL222: _temp221=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1; goto _LL218; _LL218:
_temp217=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f2; if((
unsigned int) _temp217 > 1u?(( struct _enum_struct*) _temp217)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL220: _temp219=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp217)->f1; goto _LL124;} else{ goto _LL125;}} else{ goto _LL125;} _LL125:
if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){ _LL228:
_temp227=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1;
goto _LL224; _LL224: _temp223=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp223 > 1u?(( struct _enum_struct*)
_temp223)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL226: _temp225=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp223)->f1; goto _LL126;} else{ goto
_LL127;}} else{ goto _LL127;} _LL127: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Var_e_tag){ _LL234: _temp233=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f1; goto _LL230; _LL230: _temp229=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp229 > 1u?(( struct _enum_struct*)
_temp229)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL232: _temp231=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp229)->f1; goto _LL128;} else{ goto _LL129;}}
else{ goto _LL129;} _LL129: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){
_LL240: _temp239=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1;
goto _LL236; _LL236: _temp235=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp235 > 1u?(( struct _enum_struct*)
_temp235)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL238: _temp237=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp235)->f1; goto _LL130;} else{ goto _LL131;}}
else{ goto _LL131;} _LL131: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Var_e_tag){
_LL246: _temp245=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp107)->f1;
goto _LL242; _LL242: _temp241=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if(( unsigned int) _temp241 > 1u?(( struct _enum_struct*)
_temp241)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL244: _temp243=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp241)->f1; goto _LL132;} else{ goto _LL133;}}
else{ goto _LL133;} _LL133: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Enum_e_tag){
_LL256: _temp255=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp107)->f1; goto _LL254; _LL254: _temp253=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp107)->f2; goto _LL252; _LL252: _temp251=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp107)->f3; goto _LL250;
_LL250: _temp249=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp107)->f4; goto _LL248; _LL248: _temp247=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp107)->f5; goto _LL134;} else{ goto _LL135;}
_LL135: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Xenum_e_tag){
_LL264: _temp263=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp107)->f1; goto _LL262; _LL262: _temp261=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp107)->f2; goto _LL260; _LL260: _temp259=( struct
Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp107)->f3; goto
_LL258; _LL258: _temp257=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp107)->f4; goto _LL136;} else{ goto _LL137;} _LL137: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Malloc_e_tag){ _LL266: _temp265=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp107)->f1; if((( struct
_enum_struct*) _temp265)->tag == Cyc_Absyn_Typ_m_tag){ _LL268: _temp267=( void*)((
struct Cyc_Absyn_Typ_m_struct*) _temp265)->f1; goto _LL138;} else{ goto _LL139;}}
else{ goto _LL139;} _LL139: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Malloc_e_tag){
_LL270: _temp269=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp107)->f1;
goto _LL140;} else{ goto _LL141;} _LL141: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL276: _temp275=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp107)->f1; goto _LL274; _LL274: _temp273=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp107)->f2; if(
_temp273 == 0){ goto _LL272;} else{ goto _LL143;} _LL272: _temp271=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp107)->f3; goto _LL142;} else{ goto
_LL143;} _LL143: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL282: _temp281=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp107)->f1; goto _LL280; _LL280: _temp279=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp107)->f2; goto _LL278; _LL278: _temp277=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp107)->f3;
goto _LL144;} else{ goto _LL145;} _LL145: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Primop_e_tag){ _LL286: _temp285=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp107)->f1; goto _LL284; _LL284: _temp283=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp107)->f2; goto _LL146;} else{ goto _LL147;}
_LL147: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Cast_e_tag){
_LL290: _temp289=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp107)->f1; goto
_LL288; _LL288: _temp287=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp107)->f2; goto _LL148;} else{ goto _LL149;} _LL149: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_FnCall_e_tag){ _LL294: _temp293=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp107)->f1; goto
_LL292; _LL292: _temp291=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp107)->f2; goto _LL150;} else{ goto _LL151;} _LL151: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Subscript_e_tag){ _LL298: _temp297=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp107)->f1;
goto _LL296; _LL296: _temp295=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp107)->f2; goto _LL152;} else{ goto _LL153;} _LL153: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Address_e_tag){ _LL300: _temp299=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp107)->f1; goto
_LL154;} else{ goto _LL155;} _LL155: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Deref_e_tag){ _LL302: _temp301=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp107)->f1; goto _LL156;} else{ goto _LL157;}
_LL157: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL306: _temp305=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp107)->f1; goto _LL304; _LL304: _temp303=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp107)->f2; goto _LL158;} else{ goto _LL159;}
_LL159: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_StructMember_e_tag){
_LL310: _temp309=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp107)->f1; goto _LL308; _LL308: _temp307=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp107)->f2; goto _LL160;} else{ goto _LL161;}
_LL161: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Tuple_e_tag){
_LL312: _temp311=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp107)->f1; goto _LL162;} else{ goto _LL163;} _LL163: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Struct_e_tag){ _LL320: _temp319=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp107)->f1; goto _LL318;
_LL318: _temp317=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp107)->f2; goto _LL316; _LL316: _temp315=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp107)->f3; goto _LL314; _LL314: _temp313=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp107)->f4;
goto _LL164;} else{ goto _LL165;} _LL165: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Array_e_tag){ _LL324: _temp323=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp107)->f1; goto _LL322; _LL322: _temp321=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp107)->f2; goto _LL166;} else{ goto _LL167;}
_LL167: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Increment_e_tag){
_LL328: _temp327=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp107)->f1; goto _LL326; _LL326: _temp325=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp107)->f2; goto _LL168;} else{ goto _LL169;} _LL169: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Throw_e_tag){ _LL330: _temp329=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp107)->f1; goto
_LL170;} else{ goto _LL171;} _LL171: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL336: _temp335=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp107)->f1; goto _LL334; _LL334:
_temp333=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp107)->f2; goto _LL332; _LL332: _temp331=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp107)->f3; goto _LL172;} else{ goto _LL173;}
_LL173: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL338: _temp337=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp107)->f1; goto _LL174;} else{ goto _LL175;} _LL175: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL342: _temp341=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp107)->f1; goto
_LL340; _LL340: _temp339=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp107)->f2; goto _LL176;} else{ goto _LL177;} _LL177: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL348:
_temp347=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp107)->f1; goto _LL346; _LL346: _temp345=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp107)->f2; goto _LL344; _LL344: _temp343=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp107)->f3;
goto _LL178;} else{ goto _LL179;} _LL179: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_Var_e_tag){ _LL352: _temp351=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f1; goto _LL350; _LL350: _temp349=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp107)->f2; if( _temp349 == Cyc_Absyn_Unresolved_b){ goto _LL180;} else{ goto
_LL181;}} else{ goto _LL181;} _LL181: if((( struct _enum_struct*) _temp107)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL354: _temp353=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp107)->f1; goto _LL182;} else{ goto _LL183;} _LL183: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL358: _temp357=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp107)->f1;
goto _LL356; _LL356: _temp355=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp107)->f2; goto _LL184;} else{ goto _LL185;} _LL185: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL362:
_temp361=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp107)->f1; goto _LL360; _LL360: _temp359=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp107)->f2; goto _LL186;} else{ goto
_LL187;} _LL187: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL366: _temp365=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp107)->f1; goto _LL364; _LL364: _temp363=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp107)->f2; goto _LL188;} else{ goto _LL189;}
_LL189: if((( struct _enum_struct*) _temp107)->tag == Cyc_Absyn_Codegen_e_tag){
_LL368: _temp367=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp107)->f1; goto _LL190;} else{ goto _LL191;} _LL191: if((( struct
_enum_struct*) _temp107)->tag == Cyc_Absyn_Fill_e_tag){ _LL370: _temp369=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp107)->f1; goto
_LL192;} else{ goto _LL108;} _LL110: _temp195= _temp193; goto _LL112; _LL112:
_temp199= _temp195; goto _LL114; _LL114: return Cyc_NewControlFlow_abstract_exp(
env, _temp199); _LL116: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp371=( char*)"new_control_flow.cyc:266 env.access_path==null";
struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 47; _temp372;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp373; _temp373.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp375;
_temp375.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp375.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp375;}); _temp373.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp374; _temp374.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp374.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp374;}); _temp373.lvalues= 0; _temp373;}); _LL118: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp376=( char*)"new_control_flow.cyc:271 env.access_path==null";
struct _tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 47; _temp377;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp378; _temp378.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp380;
_temp380.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp380.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp380;});
_temp378.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp379; _temp379.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp379.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp379;}); _temp378.lvalues= 0; _temp378;}); _LL120: goto _LL122; _LL122: goto
_LL124; _LL124: goto _LL126; return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL126:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL128:
_temp237= _temp231; goto _LL130; _LL130: _temp243= _temp237; goto _LL132; _LL132: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp243)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp381=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp381[ 0]=({ struct Cyc_List_List
_temp382; _temp382.hd=( void*) absop; _temp382.tl= 0; _temp382;}); _temp381;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL134: _temp261= _temp251;
_temp257= _temp247; goto _LL136; _LL136: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp383=( char*)"new_control_flow.cyc:301 env.access_path == null";
struct _tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 49; _temp384;})); if( _temp261 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_enum_root( env, e,
_temp257);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp261 != 0; _temp261= _temp261->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp385=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp385[ 0]=({ struct Cyc_List_List _temp386; _temp386.hd=( void*) Cyc_CfAbsexp_mkMemberOp(
Cyc_CfAbsexp_mkMallocOp( e),({ struct Cyc_CfFlowInfo_TupleF_struct* _temp387=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp387->tag= Cyc_CfFlowInfo_TupleF_tag; _temp387->f1= i;( void*) _temp387;}));
_temp386.tl= 0; _temp386;}); _temp385;})); struct Cyc_NewControlFlow_AbsSyn syn=
Cyc_NewControlFlow_abstract_exp( elt_env,( struct Cyc_Absyn_Exp*) _temp261->hd);
void* _temp390; void* _temp392; struct Cyc_NewControlFlow_AbsSynOne _temp388=
Cyc_NewControlFlow_meet_absexp( syn); _LL393: _temp392=( void*) _temp388.inner_exp;
goto _LL391; _LL391: _temp390=( void*) _temp388.assigns; goto _LL389; _LL389:
es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp392, es_exp);
useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp390, useexp);}}
return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp, useexp);} _LL138: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp267); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE(
e), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL140:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp394=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp394->tag= Cyc_Core_Impossible_tag; _temp394->f1=( struct _tagged_string)({
char* _temp395=( char*)"NewControlFlow: Malloc_e of unsupported variant"; struct
_tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 48; _temp396;});( struct _xenum_struct*)
_temp394;})); _LL142: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp275); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp271); void* _temp399; void*
_temp401; struct Cyc_NewControlFlow_AbsSynOne _temp397= Cyc_NewControlFlow_meet_absexp(
syn1); _LL402: _temp401=( void*) _temp397.inner_exp; goto _LL400; _LL400:
_temp399=( void*) _temp397.assigns; goto _LL398; _LL398: { void* _temp405; void*
_temp407; struct Cyc_NewControlFlow_AbsSynOne _temp403= Cyc_NewControlFlow_meet_absexp(
syn2); _LL408: _temp407=( void*) _temp403.inner_exp; goto _LL406; _LL406:
_temp405=( void*) _temp403.assigns; goto _LL404; _LL404: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp401,
_temp407), _temp405); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp399);}}}
_LL144: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp409=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp409[ 0]=({ struct Cyc_List_List _temp410;
_temp410.hd=( void*) _temp281; _temp410.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp411=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp411[ 0]=({ struct Cyc_List_List _temp412; _temp412.hd=( void*) _temp277;
_temp412.tl= 0; _temp412;}); _temp411;}); _temp410;}); _temp409;}), 0); _LL146:
return Cyc_NewControlFlow_use_exp_unordered( env, _temp283, 0); _LL148: { int
both_leaf= 1;{ void* _temp413=( void*)( _temp287->topt)->v; struct Cyc_Absyn_Structdecl**
_temp421; struct Cyc_List_List* _temp423; struct _tuple1* _temp425; struct Cyc_List_List*
_temp427; _LL415: if(( unsigned int) _temp413 > 4u?(( struct _enum_struct*)
_temp413)->tag == Cyc_Absyn_StructType_tag: 0){ _LL426: _temp425=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp413)->f1; goto _LL424;
_LL424: _temp423=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp413)->f2; goto _LL422; _LL422: _temp421=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp413)->f3; goto _LL416;} else{ goto
_LL417;} _LL417: if(( unsigned int) _temp413 > 4u?(( struct _enum_struct*)
_temp413)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL428: _temp427=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp413)->f1; goto _LL418;} else{ goto
_LL419;} _LL419: goto _LL420; _LL416: goto _LL418; _LL418: both_leaf= 0; goto
_LL414; _LL420: goto _LL414; _LL414:;}{ void* _temp429=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp437; struct Cyc_List_List* _temp439; struct
_tuple1* _temp441; struct Cyc_List_List* _temp443; _LL431: if(( unsigned int)
_temp429 > 4u?(( struct _enum_struct*) _temp429)->tag == Cyc_Absyn_StructType_tag:
0){ _LL442: _temp441=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp429)->f1; goto _LL440; _LL440: _temp439=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp429)->f2; goto _LL438; _LL438: _temp437=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp429)->f3;
goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int) _temp429 > 4u?((
struct _enum_struct*) _temp429)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL444:
_temp443=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp429)->f1;
goto _LL434;} else{ goto _LL435;} _LL435: goto _LL436; _LL432: goto _LL434;
_LL434: both_leaf= 0; goto _LL430; _LL436: goto _LL430; _LL430:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp287);} return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp445=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp445[ 0]=({ struct Cyc_List_List
_temp446; _temp446.hd=( void*) _temp287; _temp446.tl= 0; _temp446;}); _temp445;}),
env.lhsides);} _LL150: return Cyc_NewControlFlow_use_exp_unordered( env,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp447=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp447[ 0]=({ struct Cyc_List_List
_temp448; _temp448.hd=( void*) _temp293; _temp448.tl= _temp291; _temp448;});
_temp447;}), 0); _LL152: { void* _temp449= Cyc_Tcutil_compress(( void*)(
_temp297->topt)->v); struct Cyc_List_List* _temp455; _LL451: if(( unsigned int)
_temp449 > 4u?(( struct _enum_struct*) _temp449)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL456: _temp455=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp449)->f1; goto _LL452;} else{ goto _LL453;} _LL453: goto _LL454; _LL452:
return Cyc_NewControlFlow_descend_path( env, e, _temp297); _LL454: return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp457=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp457[ 0]=({ struct Cyc_List_List
_temp458; _temp458.hd=( void*) _temp297; _temp458.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp459=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp459[ 0]=({ struct Cyc_List_List _temp460;
_temp460.hd=( void*) _temp295; _temp460.tl= 0; _temp460;}); _temp459;});
_temp458;}); _temp457;}), Cyc_NewControlFlow_use_it_list()); _LL450:;} _LL154:{
void* _temp461=( void*) _temp299->r; struct Cyc_Absyn_Structdecl* _temp469;
struct Cyc_List_List* _temp471; struct Cyc_Core_Opt* _temp473; struct _tuple1*
_temp475; struct Cyc_List_List* _temp477; _LL463: if((( struct _enum_struct*)
_temp461)->tag == Cyc_Absyn_Struct_e_tag){ _LL476: _temp475=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp461)->f1; goto _LL474; _LL474: _temp473=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp461)->f2; goto
_LL472; _LL472: _temp471=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp461)->f3; goto _LL470; _LL470: _temp469=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp461)->f4; goto _LL464;} else{ goto
_LL465;} _LL465: if((( struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Tuple_e_tag){
_LL478: _temp477=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp461)->f1; goto _LL466;} else{ goto _LL467;} _LL467: goto _LL468; _LL464:
goto _LL466; _LL466: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)( _temp299->topt)->v);{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp479=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp479[ 0]=({ struct Cyc_List_List _temp480; _temp480.hd=( void*)
malloc_op; _temp480.tl= 0; _temp480;}); _temp479;})); inner_env.access_path= 0;{
void* _temp483; void* _temp485; struct Cyc_NewControlFlow_AbsSynOne _temp481=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( inner_env,
_temp299)); _LL486: _temp485=( void*) _temp481.inner_exp; goto _LL484; _LL484:
_temp483=( void*) _temp481.assigns; goto _LL482; _LL482: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp485, _temp483); if( env.access_path
!= 0){ void* _temp487=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp497; struct Cyc_Absyn_Exp* _temp499; struct Cyc_Absyn_Exp*
_temp501; struct _tagged_string* _temp503; struct Cyc_Absyn_Exp* _temp505;
_LL489: if((( struct _enum_struct*) _temp487)->tag == Cyc_Absyn_Subscript_e_tag){
_LL500: _temp499=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp487)->f1; goto _LL498; _LL498: _temp497=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp487)->f2; goto _LL490;} else{ goto _LL491;}
_LL491: if((( struct _enum_struct*) _temp487)->tag == Cyc_Absyn_Deref_e_tag){
_LL502: _temp501=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp487)->f1; goto _LL492;} else{ goto _LL493;} _LL493: if((( struct
_enum_struct*) _temp487)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL506: _temp505=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp487)->f1;
goto _LL504; _LL504: _temp503=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp487)->f2; goto _LL494;} else{ goto _LL495;} _LL495: goto _LL496; _LL490:
Cyc_Evexp_eval_const_uint_exp( _temp497) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp507=( char*)"new_control_flow.cyc:418 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 61; _temp508;})); goto _LL492; _LL492: env.access_path=(
env.access_path)->tl; ans.lvalues=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp509=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp509[ 0]=({ struct Cyc_List_List _temp510; _temp510.hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp510.tl= 0; _temp510;}); _temp509;}); goto _LL488; _LL494:
env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp511=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp511[
0]=({ struct Cyc_List_List _temp512; _temp512.hd=( void*) Cyc_Absyn_structmember_exp(
_temp505, _temp503, 0); _temp512.tl=( env.access_path)->tl; _temp512;});
_temp511;}); ans.lvalues=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp513=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp513[ 0]=({ struct Cyc_List_List _temp514; _temp514.hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp514.tl= 0; _temp514;}); _temp513;}); goto _LL488; _LL496:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp515=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp515->tag= Cyc_Core_Impossible_tag;
_temp515->f1=( struct _tagged_string)({ char* _temp516=( char*)"bad malloc access path";
struct _tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 23; _temp517;});( struct _xenum_struct*)
_temp515;})); _LL488:;} return ans;}}}} _LL468: goto _LL462; _LL462:;}{ struct
Cyc_List_List* _temp518= env.access_path; struct Cyc_List_List _temp530; struct
Cyc_List_List* _temp531; struct Cyc_Absyn_Exp* _temp533; struct Cyc_Absyn_Exp
_temp535; struct Cyc_Position_Segment* _temp536; void* _temp538; struct Cyc_Absyn_Exp*
_temp540; struct Cyc_Absyn_Exp* _temp542; struct Cyc_Core_Opt* _temp544; struct
Cyc_List_List _temp546; struct Cyc_List_List* _temp547; struct Cyc_Absyn_Exp*
_temp549; struct Cyc_Absyn_Exp _temp551; struct Cyc_Position_Segment* _temp552;
void* _temp554; struct Cyc_Absyn_Exp* _temp556; struct Cyc_Core_Opt* _temp558;
struct Cyc_List_List _temp560; struct Cyc_List_List* _temp561; struct Cyc_Absyn_Exp*
_temp563; struct Cyc_Absyn_Exp _temp565; struct Cyc_Position_Segment* _temp566;
void* _temp568; struct _tagged_string* _temp570; struct Cyc_Absyn_Exp* _temp572;
struct Cyc_Core_Opt* _temp574; _LL520: if( _temp518 == 0){ goto _LL521;} else{
goto _LL522;} _LL522: if( _temp518 == 0){ goto _LL524;} else{ _temp530=*
_temp518; _LL534: _temp533=( struct Cyc_Absyn_Exp*) _temp530.hd; _temp535=*
_temp533; _LL545: _temp544=( struct Cyc_Core_Opt*) _temp535.topt; goto _LL539;
_LL539: _temp538=( void*) _temp535.r; if((( struct _enum_struct*) _temp538)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL543: _temp542=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp538)->f1; goto _LL541; _LL541:
_temp540=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp538)->f2; goto _LL537;} else{ goto _LL524;} _LL537: _temp536=( struct Cyc_Position_Segment*)
_temp535.loc; goto _LL532; _LL532: _temp531=( struct Cyc_List_List*) _temp530.tl;
goto _LL523;} _LL524: if( _temp518 == 0){ goto _LL526;} else{ _temp546=*
_temp518; _LL550: _temp549=( struct Cyc_Absyn_Exp*) _temp546.hd; _temp551=*
_temp549; _LL559: _temp558=( struct Cyc_Core_Opt*) _temp551.topt; goto _LL555;
_LL555: _temp554=( void*) _temp551.r; if((( struct _enum_struct*) _temp554)->tag
== Cyc_Absyn_Deref_e_tag){ _LL557: _temp556=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp554)->f1; goto _LL553;} else{ goto _LL526;}
_LL553: _temp552=( struct Cyc_Position_Segment*) _temp551.loc; goto _LL548;
_LL548: _temp547=( struct Cyc_List_List*) _temp546.tl; goto _LL525;} _LL526: if(
_temp518 == 0){ goto _LL528;} else{ _temp560=* _temp518; _LL564: _temp563=(
struct Cyc_Absyn_Exp*) _temp560.hd; _temp565=* _temp563; _LL575: _temp574=(
struct Cyc_Core_Opt*) _temp565.topt; goto _LL569; _LL569: _temp568=( void*)
_temp565.r; if((( struct _enum_struct*) _temp568)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL573: _temp572=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp568)->f1; goto _LL571; _LL571: _temp570=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp568)->f2; goto _LL567;} else{ goto _LL528;}
_LL567: _temp566=( struct Cyc_Position_Segment*) _temp565.loc; goto _LL562;
_LL562: _temp561=( struct Cyc_List_List*) _temp560.tl; goto _LL527;} _LL528:
goto _LL529; _LL521: env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp576=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp576[ 0]=({ struct Cyc_List_List _temp577; _temp577.hd=( void*) e; _temp577.tl=
env.access_path; _temp577;}); _temp576;}); goto _LL519; _LL523: Cyc_Evexp_eval_const_uint_exp(
_temp540) ==( unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp578=( char*)"new_control_flow.cyc:446 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp579; _temp579.curr= _temp578; _temp579.base= _temp578;
_temp579.last_plus_one= _temp578 + 61; _temp579;})); _temp547= _temp531; goto
_LL525; _LL525: env.access_path= _temp547; goto _LL519; _LL527: env.access_path=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp580=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp580[ 0]=({ struct Cyc_List_List
_temp581; _temp581.hd=( void*) Cyc_Absyn_structmember_exp( _temp572, _temp570, 0);
_temp581.tl= _temp561; _temp581;}); _temp580;}); goto _LL519; _LL529:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp582=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp582->tag= Cyc_Core_Impossible_tag;
_temp582->f1=( struct _tagged_string)({ char* _temp583=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 36; _temp584;});( struct _xenum_struct*)
_temp582;})); _LL519:;} return Cyc_NewControlFlow_abstract_exp( env, _temp299);
_LL156: { struct Cyc_List_List* _temp585= env.access_path; struct Cyc_List_List
_temp591; struct Cyc_List_List* _temp592; struct Cyc_Absyn_Exp* _temp594; struct
Cyc_Absyn_Exp _temp596; struct Cyc_Position_Segment* _temp597; void* _temp599;
struct Cyc_Absyn_Exp* _temp601; struct Cyc_Core_Opt* _temp603; _LL587: if(
_temp585 == 0){ goto _LL589;} else{ _temp591=* _temp585; _LL595: _temp594=(
struct Cyc_Absyn_Exp*) _temp591.hd; _temp596=* _temp594; _LL604: _temp603=(
struct Cyc_Core_Opt*) _temp596.topt; goto _LL600; _LL600: _temp599=( void*)
_temp596.r; if((( struct _enum_struct*) _temp599)->tag == Cyc_Absyn_Address_e_tag){
_LL602: _temp601=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp599)->f1; goto _LL598;} else{ goto _LL589;} _LL598: _temp597=( struct Cyc_Position_Segment*)
_temp596.loc; goto _LL593; _LL593: _temp592=( struct Cyc_List_List*) _temp591.tl;
goto _LL588;} _LL589: goto _LL590; _LL588: env.access_path= _temp592;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp301);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL590: return Cyc_NewControlFlow_descend_path( env, e, _temp301); _LL586:;}
_LL158: { void* _temp605= Cyc_Tcutil_compress(( void*)( _temp305->topt)->v);
struct Cyc_Absyn_PtrInfo _temp611; struct Cyc_Absyn_Conref* _temp613; struct Cyc_Absyn_Tqual*
_temp615; struct Cyc_Absyn_Conref* _temp617; void* _temp619; void* _temp621;
_LL607: if(( unsigned int) _temp605 > 4u?(( struct _enum_struct*) _temp605)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL612: _temp611=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp605)->f1; _LL622: _temp621=( void*)
_temp611.elt_typ; goto _LL620; _LL620: _temp619=( void*) _temp611.rgn_typ; goto
_LL618; _LL618: _temp617=( struct Cyc_Absyn_Conref*) _temp611.nullable; goto
_LL616; _LL616: _temp615=( struct Cyc_Absyn_Tqual*) _temp611.tq; goto _LL614;
_LL614: _temp613=( struct Cyc_Absyn_Conref*) _temp611.bounds; goto _LL608;}
else{ goto _LL609;} _LL609: goto _LL610; _LL608:{ void* _temp623= Cyc_Tcutil_compress(
_temp621); struct Cyc_Absyn_Structdecl** _temp631; struct Cyc_List_List*
_temp633; struct _tuple1* _temp635; struct Cyc_Absyn_Uniondecl** _temp637;
struct Cyc_List_List* _temp639; struct _tuple1* _temp641; _LL625: if((
unsigned int) _temp623 > 4u?(( struct _enum_struct*) _temp623)->tag == Cyc_Absyn_StructType_tag:
0){ _LL636: _temp635=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp623)->f1; goto _LL634; _LL634: _temp633=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp623)->f2; goto _LL632; _LL632: _temp631=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp623)->f3;
goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int) _temp623 > 4u?((
struct _enum_struct*) _temp623)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL642:
_temp641=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp623)->f1;
goto _LL640; _LL640: _temp639=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp623)->f2; goto _LL638; _LL638: _temp637=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp623)->f3; goto _LL628;} else{ goto
_LL629;} _LL629: goto _LL630; _LL626: return Cyc_NewControlFlow_descend_path(
env, e, _temp305); _LL628: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp305); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL630: goto _LL624; _LL624:;} goto _LL610; _LL610:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp643=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp643->tag= Cyc_Core_Impossible_tag;
_temp643->f1=( struct _tagged_string)({ char* _temp644=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp645; _temp645.curr= _temp644; _temp645.base= _temp644;
_temp645.last_plus_one= _temp644 + 40; _temp645;});( struct _xenum_struct*)
_temp643;})); _LL606:;} _LL160: { void* _temp646= Cyc_Tcutil_compress(( void*)(
_temp309->topt)->v); struct Cyc_Absyn_Structdecl** _temp654; struct Cyc_List_List*
_temp656; struct _tuple1* _temp658; struct Cyc_Absyn_Uniondecl** _temp660;
struct Cyc_List_List* _temp662; struct _tuple1* _temp664; _LL648: if((
unsigned int) _temp646 > 4u?(( struct _enum_struct*) _temp646)->tag == Cyc_Absyn_StructType_tag:
0){ _LL659: _temp658=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp646)->f1; goto _LL657; _LL657: _temp656=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp646)->f2; goto _LL655; _LL655: _temp654=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp646)->f3;
goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int) _temp646 > 4u?((
struct _enum_struct*) _temp646)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL665:
_temp664=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp646)->f1;
goto _LL663; _LL663: _temp662=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp646)->f2; goto _LL661; _LL661: _temp660=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp646)->f3; goto _LL651;} else{ goto
_LL652;} _LL652: goto _LL653; _LL649: return Cyc_NewControlFlow_descend_path(
env, e, _temp309); _LL651: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp309); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL653:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp666=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp666->tag= Cyc_Core_Impossible_tag; _temp666->f1=( struct _tagged_string)({
char* _temp667=( char*)"NewControlFlow: bad type in StructMember"; struct
_tagged_string _temp668; _temp668.curr= _temp667; _temp668.base= _temp667;
_temp668.last_plus_one= _temp667 + 41; _temp668;});( struct _xenum_struct*)
_temp666;})); _LL647:;} _LL162: { struct Cyc_List_List* _temp669= env.access_path;
struct Cyc_List_List _temp677; struct Cyc_List_List* _temp678; struct Cyc_Absyn_Exp*
_temp680; struct Cyc_Absyn_Exp _temp682; struct Cyc_Position_Segment* _temp683;
void* _temp685; struct Cyc_Absyn_Exp* _temp687; struct Cyc_Absyn_Exp* _temp689;
struct Cyc_Core_Opt* _temp691; _LL671: if( _temp669 == 0){ goto _LL672;} else{
goto _LL673;} _LL673: if( _temp669 == 0){ goto _LL675;} else{ _temp677=*
_temp669; _LL681: _temp680=( struct Cyc_Absyn_Exp*) _temp677.hd; _temp682=*
_temp680; _LL692: _temp691=( struct Cyc_Core_Opt*) _temp682.topt; goto _LL686;
_LL686: _temp685=( void*) _temp682.r; if((( struct _enum_struct*) _temp685)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL690: _temp689=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp685)->f1; goto _LL688; _LL688:
_temp687=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp685)->f2; goto _LL684;} else{ goto _LL675;} _LL684: _temp683=( struct Cyc_Position_Segment*)
_temp682.loc; goto _LL679; _LL679: _temp678=( struct Cyc_List_List*) _temp677.tl;
goto _LL674;} _LL675: goto _LL676; _LL672: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp311 != 0;
_temp311= _temp311->tl, ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp693=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp693[ 0]=({ struct Cyc_List_List _temp694; _temp694.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp694.tl= ith_lvals; _temp694;}); _temp693;});} else{ ith_lvals=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp695=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp695[ 0]=({ struct Cyc_List_List _temp696;
_temp696.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp697=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp697->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp697->f1= j;( void*) _temp697;})); _temp696.tl= ith_lvals; _temp696;});
_temp695;});}}}{ struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp311->hd);
void* _temp700; void* _temp702; struct Cyc_NewControlFlow_AbsSynOne _temp698=
Cyc_NewControlFlow_meet_absexp( ith_syn); _LL703: _temp702=( void*) _temp698.inner_exp;
goto _LL701; _LL701: _temp700=( void*) _temp698.assigns; goto _LL699; _LL699:
ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, ansexp, _temp702);
assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), assignexp,
_temp700);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);} _LL674: {
int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp687); struct Cyc_NewControlFlow_AbsEnv
other_env= Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp678;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();{
int j= 0; for( 0; _temp311 != 0; _temp311= _temp311->tl, ++ j){ if( i == j){
void* _temp706; void* _temp708; struct Cyc_NewControlFlow_AbsSynOne _temp704=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*) _temp311->hd)); _LL709: _temp708=( void*) _temp704.inner_exp;
goto _LL707; _LL707: _temp706=( void*) _temp704.assigns; goto _LL705; _LL705:
assignexp= _temp706; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp708);} else{ void* _temp712; void* _temp714; struct Cyc_NewControlFlow_AbsSynOne
_temp710= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,( struct Cyc_Absyn_Exp*) _temp311->hd)); _LL715: _temp714=( void*)
_temp710.inner_exp; goto _LL713; _LL713: _temp712=( void*) _temp710.assigns;
goto _LL711; _LL711: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp714);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp716=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp716[ 0]=({ struct Cyc_List_List _temp717;
_temp717.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp717.tl= 0; _temp717;});
_temp716;}), assignexp);}} _LL676:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp718=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp718->tag= Cyc_Core_Impossible_tag; _temp718->f1=( struct _tagged_string)({
char* _temp719=( char*)"abstract_exp: unexpected access path to Tuple"; struct
_tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 46; _temp720;});( struct _xenum_struct*)
_temp718;})); _LL670:;} _LL164: { struct Cyc_List_List* _temp721= env.access_path;
struct Cyc_List_List _temp729; struct Cyc_List_List* _temp730; struct Cyc_Absyn_Exp*
_temp732; struct Cyc_Absyn_Exp _temp734; struct Cyc_Position_Segment* _temp735;
void* _temp737; struct _tagged_string* _temp739; struct Cyc_Absyn_Exp* _temp741;
struct Cyc_Core_Opt* _temp743; _LL723: if( _temp721 == 0){ goto _LL724;} else{
goto _LL725;} _LL725: if( _temp721 == 0){ goto _LL727;} else{ _temp729=*
_temp721; _LL733: _temp732=( struct Cyc_Absyn_Exp*) _temp729.hd; _temp734=*
_temp732; _LL744: _temp743=( struct Cyc_Core_Opt*) _temp734.topt; goto _LL738;
_LL738: _temp737=( void*) _temp734.r; if((( struct _enum_struct*) _temp737)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL742: _temp741=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp737)->f1; goto _LL740; _LL740:
_temp739=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp737)->f2; goto _LL736;} else{ goto _LL727;} _LL736: _temp735=( struct Cyc_Position_Segment*)
_temp734.loc; goto _LL731; _LL731: _temp730=( struct Cyc_List_List*) _temp729.tl;
goto _LL726;} _LL727: goto _LL728; _LL724: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp315 != 0; _temp315=
_temp315->tl){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls=
env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)
ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp745=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp745[
0]=({ struct Cyc_List_List _temp746; _temp746.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp746.tl= ith_lvals; _temp746;}); _temp745;});} else{ struct Cyc_List_List*
ds=(*(( struct _tuple5*) _temp315->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void*
_temp747=( void*) ds->hd; struct Cyc_Absyn_Exp* _temp753; struct _tagged_string*
_temp755; _LL749: if((( struct _enum_struct*) _temp747)->tag == Cyc_Absyn_ArrayElement_tag){
_LL754: _temp753=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp747)->f1; goto _LL750;} else{ goto _LL751;} _LL751: if((( struct
_enum_struct*) _temp747)->tag == Cyc_Absyn_FieldName_tag){ _LL756: _temp755=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp747)->f1;
goto _LL752;} else{ goto _LL748;} _LL750:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp757=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp757->tag= Cyc_Core_Impossible_tag; _temp757->f1=( struct _tagged_string)({
char* _temp758=( char*)"bad struct designator"; struct _tagged_string _temp759;
_temp759.curr= _temp758; _temp759.base= _temp758; _temp759.last_plus_one=
_temp758 + 22; _temp759;});( struct _xenum_struct*) _temp757;})); _LL752:
ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp760=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp760[ 0]=({ struct Cyc_List_List
_temp761; _temp761.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct
Cyc_CfFlowInfo_StructF_struct* _temp762=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp762->tag= Cyc_CfFlowInfo_StructF_tag;
_temp762->f1= _temp755;( void*) _temp762;})); _temp761.tl= ith_lvals; _temp761;});
_temp760;}); goto _LL748; _LL748:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp315->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp315->hd)).f2); void* _temp765; void* _temp767; struct Cyc_NewControlFlow_AbsSynOne
_temp763= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL768: _temp767=( void*)
_temp763.inner_exp; goto _LL766; _LL766: _temp765=( void*) _temp763.assigns;
goto _LL764; _LL764: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp767); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp765);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL726: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp730;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp315 != 0; _temp315=
_temp315->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp315->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp769=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp775; struct _tagged_string* _temp777; _LL771: if(((
struct _enum_struct*) _temp769)->tag == Cyc_Absyn_ArrayElement_tag){ _LL776:
_temp775=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp769)->f1; goto _LL772;} else{ goto _LL773;} _LL773: if((( struct
_enum_struct*) _temp769)->tag == Cyc_Absyn_FieldName_tag){ _LL778: _temp777=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp769)->f1;
goto _LL774;} else{ goto _LL770;} _LL772:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp779=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp779->tag= Cyc_Core_Impossible_tag; _temp779->f1=( struct _tagged_string)({
char* _temp780=( char*)"bad struct designator"; struct _tagged_string _temp781;
_temp781.curr= _temp780; _temp781.base= _temp780; _temp781.last_plus_one=
_temp780 + 22; _temp781;});( struct _xenum_struct*) _temp779;})); _LL774: if(
Cyc_String_zstrptrcmp( _temp739, _temp777) == 0){ used= 1;} goto _LL770; _LL770:;}}
if( used){ void* _temp784; void* _temp786; struct Cyc_NewControlFlow_AbsSynOne
_temp782= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*) _temp315->hd)).f2)); _LL787: _temp786=( void*) _temp782.inner_exp;
goto _LL785; _LL785: _temp784=( void*) _temp782.assigns; goto _LL783; _LL783:
assignexp= _temp784; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp786);} else{ void* _temp790; void* _temp792; struct Cyc_NewControlFlow_AbsSynOne
_temp788= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp315->hd)).f2)); _LL793: _temp792=( void*)
_temp788.inner_exp; goto _LL791; _LL791: _temp790=( void*) _temp788.assigns;
goto _LL789; _LL789: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp792);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL728:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp794=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp794->tag= Cyc_Core_Impossible_tag; _temp794->f1=( struct _tagged_string)({
char* _temp795=( char*)"abstract_exp: unexpected access path to Struct"; struct
_tagged_string _temp796; _temp796.curr= _temp795; _temp796.base= _temp795;
_temp796.last_plus_one= _temp795 + 47; _temp796;});( struct _xenum_struct*)
_temp794;})); _LL722:;} _LL166: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp321), 0); _LL168: return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp797[ 0]=({ struct Cyc_List_List
_temp798; _temp798.hd=( void*) _temp327; _temp798.tl= 0; _temp798;}); _temp797;}),
0); _LL170: { struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp329); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
esyn)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL172: { struct Cyc_List_List* _temp801; struct Cyc_NewControlFlow_AbsSynOne
_temp803; struct Cyc_NewControlFlow_AbsSynOne _temp805; struct Cyc_NewControlFlow_AbsSyn
_temp799= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp335); _LL806: _temp805=( struct Cyc_NewControlFlow_AbsSynOne) _temp799.when_true;
goto _LL804; _LL804: _temp803=( struct Cyc_NewControlFlow_AbsSynOne) _temp799.when_false;
goto _LL802; _LL802: _temp801=( struct Cyc_List_List*) _temp799.lvalues; goto
_LL800; _LL800: { struct Cyc_List_List* _temp809; struct Cyc_NewControlFlow_AbsSynOne
_temp811; struct Cyc_NewControlFlow_AbsSynOne _temp813; struct Cyc_NewControlFlow_AbsSyn
_temp807= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp333); _LL814: _temp813=( struct Cyc_NewControlFlow_AbsSynOne) _temp807.when_true;
goto _LL812; _LL812: _temp811=( struct Cyc_NewControlFlow_AbsSynOne) _temp807.when_false;
goto _LL810; _LL810: _temp809=( struct Cyc_List_List*) _temp807.lvalues; goto
_LL808; _LL808: { struct Cyc_List_List* _temp817; struct Cyc_NewControlFlow_AbsSynOne
_temp819; struct Cyc_NewControlFlow_AbsSynOne _temp821; struct Cyc_NewControlFlow_AbsSyn
_temp815= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp331); _LL822: _temp821=( struct Cyc_NewControlFlow_AbsSynOne) _temp815.when_true;
goto _LL820; _LL820: _temp819=( struct Cyc_NewControlFlow_AbsSynOne) _temp815.when_false;
goto _LL818; _LL818: _temp817=( struct Cyc_List_List*) _temp815.lvalues; goto
_LL816; _LL816: { void* true_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp805.inner_exp,( void*) _temp805.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp813.inner_exp,( void*) _temp813.assigns)),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp803.inner_exp,( void*) _temp803.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp821.inner_exp,( void*) _temp821.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp805.inner_exp,( void*) _temp805.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp811.inner_exp,( void*) _temp811.assigns)), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp803.inner_exp,( void*) _temp803.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp819.inner_exp,( void*) _temp819.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp823; _temp823.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp825;
_temp825.inner_exp=( void*) true_exp; _temp825.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp825;}); _temp823.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp824; _temp824.inner_exp=( void*) false_exp; _temp824.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp824;}); _temp823.lvalues= 0; _temp823;});}}}}
_LL174: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp337);{
struct Cyc_Absyn_Stmt* last_s= _temp337; while( 1) { void* _temp826=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp836; struct Cyc_Absyn_Stmt* _temp838;
struct Cyc_Absyn_Stmt* _temp840; struct Cyc_Absyn_Decl* _temp842; struct Cyc_Absyn_Exp*
_temp844; _LL828: if(( unsigned int) _temp826 > 1u?(( struct _enum_struct*)
_temp826)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL839: _temp838=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp826)->f1; goto _LL837; _LL837: _temp836=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp826)->f2; goto
_LL829;} else{ goto _LL830;} _LL830: if(( unsigned int) _temp826 > 1u?(( struct
_enum_struct*) _temp826)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL843: _temp842=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp826)->f1; goto
_LL841; _LL841: _temp840=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp826)->f2; goto _LL831;} else{ goto _LL832;} _LL832: if(( unsigned int)
_temp826 > 1u?(( struct _enum_struct*) _temp826)->tag == Cyc_Absyn_Exp_s_tag: 0){
_LL845: _temp844=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp826)->f1; goto _LL833;} else{ goto _LL834;} _LL834: goto _LL835; _LL829:
last_s= _temp836; continue; _LL831: last_s= _temp840; continue; _LL833: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp844); void* _temp848; void* _temp850; struct Cyc_NewControlFlow_AbsSynOne
_temp846= Cyc_NewControlFlow_meet_absexp( syn); _LL851: _temp850=( void*)
_temp846.inner_exp; goto _LL849; _LL849: _temp848=( void*) _temp846.assigns;
goto _LL847; _LL847:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp850, _temp848));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp337), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL835:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp852=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp852->tag= Cyc_Core_Impossible_tag; _temp852->f1=( struct _tagged_string)({
char* _temp853=( char*)"abstract_exp: ill-formed StmtExp"; struct _tagged_string
_temp854; _temp854.curr= _temp853; _temp854.base= _temp853; _temp854.last_plus_one=
_temp853 + 33; _temp854;});( struct _xenum_struct*) _temp852;})); _LL827:;}}}
_LL176: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp341); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp339); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp855; _temp855.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp857; _temp857.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp857.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp857;});
_temp855.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp856; _temp856.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp856.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp856;}); _temp855.lvalues= 0; _temp855;});}} _LL178: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp858=( char*)"new_control_flow.cyc:713 env.access_path==null";
struct _tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 47; _temp859;})); Cyc_NewControlFlow_add_var_root(
env, _temp347);( env.shared)->comprehension_vars=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp860=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp860[ 0]=({ struct Cyc_List_List _temp861;
_temp861.hd=( void*) _temp347; _temp861.tl=( env.shared)->comprehension_vars;
_temp861;}); _temp860;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp864; void* _temp866; struct Cyc_NewControlFlow_AbsSynOne
_temp862= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp345)); _LL867: _temp866=( void*) _temp862.inner_exp; goto _LL865;
_LL865: _temp864=( void*) _temp862.assigns; goto _LL863; _LL863: { void*
_temp870; void* _temp872; struct Cyc_NewControlFlow_AbsSynOne _temp868= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp343)); _LL873: _temp872=( void*)
_temp868.inner_exp; goto _LL871; _LL871: _temp870=( void*) _temp868.assigns;
goto _LL869; _LL869: { void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp866, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp864, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp347),
Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, _temp872, _temp870), Cyc_CfAbsexp_mkSkipAE())))); return
Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()));}}} _LL180: goto _LL182; _LL182: goto _LL184;
_LL184: goto _LL186; _LL186: goto _LL188; _LL188: goto _LL190; _LL190: goto
_LL192; _LL192:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp874=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp874->tag= Cyc_Core_Impossible_tag; _temp874->f1=( struct _tagged_string)({
char* _temp875=( char*)"abstract_exp, unexpected exp form"; struct
_tagged_string _temp876; _temp876.curr= _temp875; _temp876.base= _temp875;
_temp876.last_plus_one= _temp875 + 34; _temp876;});( struct _xenum_struct*)
_temp874;})); _LL108:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp879; void* _temp881; struct Cyc_NewControlFlow_AbsSynOne
_temp877= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL882: _temp881=( void*) _temp877.inner_exp; goto _LL880; _LL880: _temp879=(
void*) _temp877.assigns; goto _LL878; _LL878: return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp881, _temp879);} struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp883; _temp883.f1= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns); _temp883.f2=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp883;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp886;
struct Cyc_Position_Segment* _temp887; struct Cyc_Absyn_Stmt* _temp889; struct
Cyc_Absyn_Exp* _temp891; struct Cyc_Core_Opt* _temp893; struct Cyc_Absyn_Pat*
_temp895; struct Cyc_Absyn_Switch_clause* _temp884=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp886=* _temp884; _LL896: _temp895=( struct Cyc_Absyn_Pat*) _temp886.pattern;
goto _LL894; _LL894: _temp893=( struct Cyc_Core_Opt*) _temp886.pat_vars; goto
_LL892; _LL892: _temp891=( struct Cyc_Absyn_Exp*) _temp886.where_clause; goto
_LL890; _LL890: _temp889=( struct Cyc_Absyn_Stmt*) _temp886.body; goto _LL888;
_LL888: _temp887=( struct Cyc_Position_Segment*) _temp886.loc; goto _LL885;
_LL885: if( _temp893 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp897=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp897->tag= Cyc_Core_Impossible_tag; _temp897->f1=( struct _tagged_string)({
char* _temp898=( char*)"switch clause vds not set"; struct _tagged_string
_temp899; _temp899.curr= _temp898; _temp899.base= _temp898; _temp899.last_plus_one=
_temp898 + 26; _temp899;});( struct _xenum_struct*) _temp897;}));}{ struct Cyc_List_List*
vds=( struct Cyc_List_List*) _temp893->v; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp891 != 0){ void* _temp903;
void* _temp905; struct _tuple0 _temp901= Cyc_NewControlFlow_abstract_guard( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp900= _temp891; if( _temp900
== 0){ _throw( Null_Exception);} _temp900;})); _LL906: _temp905= _temp901.f1;
goto _LL904; _LL904: _temp903= _temp901.f2; goto _LL902; _LL902: ans=( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp907=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp907[ 0]=({ struct Cyc_List_List
_temp908; _temp908.hd=( void*) _temp905; _temp908.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp909=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp909[ 0]=({ struct Cyc_List_List _temp910;
_temp910.hd=( void*) _temp903; _temp910.tl= ans; _temp910;}); _temp909;});
_temp908;}); _temp907;});} Cyc_NewControlFlow_cf_prepass( env, _temp889);}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp911=( char*)"new_control_flow.cyc:765 env.lhsides == null";
struct _tagged_string _temp912; _temp912.curr= _temp911; _temp912.base= _temp911;
_temp912.last_plus_one= _temp911 + 45; _temp912;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp913=( char*)"new_control_flow.cyc:766 env.access_path == null";
struct _tagged_string _temp914; _temp914.curr= _temp913; _temp914.base= _temp913;
_temp914.last_plus_one= _temp913 + 49; _temp914;}));{ void* _temp915=( void*) s->r;
struct Cyc_Absyn_Exp* _temp961; struct Cyc_Absyn_Stmt* _temp963; struct Cyc_Absyn_Stmt*
_temp965; struct Cyc_Absyn_Exp* _temp967; struct Cyc_Absyn_Exp* _temp969; struct
Cyc_Absyn_Stmt* _temp971; struct Cyc_Absyn_Stmt* _temp973; struct Cyc_Absyn_Exp*
_temp975; struct _tuple3 _temp977; struct Cyc_Absyn_Stmt* _temp979; struct Cyc_Absyn_Exp*
_temp981; struct Cyc_Absyn_Stmt* _temp983; struct Cyc_Absyn_Stmt* _temp985;
struct _tuple3 _temp987; struct Cyc_Absyn_Stmt* _temp989; struct Cyc_Absyn_Exp*
_temp991; struct Cyc_Absyn_Stmt* _temp993; struct Cyc_Absyn_Stmt* _temp995;
struct Cyc_Absyn_Stmt* _temp997; struct _tagged_string* _temp999; struct Cyc_Absyn_Stmt*
_temp1001; struct _tagged_string* _temp1003; struct Cyc_Absyn_Stmt* _temp1005;
struct _tuple3 _temp1007; struct Cyc_Absyn_Stmt* _temp1009; struct Cyc_Absyn_Exp*
_temp1011; struct _tuple3 _temp1013; struct Cyc_Absyn_Stmt* _temp1015; struct
Cyc_Absyn_Exp* _temp1017; struct Cyc_Absyn_Exp* _temp1019; struct Cyc_List_List*
_temp1021; struct Cyc_Absyn_Exp* _temp1023; struct Cyc_Absyn_Switch_clause**
_temp1025; struct Cyc_List_List* _temp1027; struct Cyc_Absyn_Stmt* _temp1029;
struct Cyc_Absyn_Decl* _temp1031; struct Cyc_Absyn_Decl _temp1033; struct Cyc_Position_Segment*
_temp1034; void* _temp1036; struct Cyc_Absyn_Vardecl* _temp1038; struct Cyc_Absyn_Stmt*
_temp1040; struct Cyc_Absyn_Decl* _temp1042; struct Cyc_Absyn_Decl _temp1044;
struct Cyc_Position_Segment* _temp1045; void* _temp1047; int _temp1049; struct
Cyc_Absyn_Exp* _temp1051; struct Cyc_Core_Opt* _temp1053; struct Cyc_Core_Opt*
_temp1055; struct Cyc_Core_Opt _temp1057; struct Cyc_List_List* _temp1058;
struct Cyc_Absyn_Pat* _temp1060; struct Cyc_Absyn_Stmt* _temp1062; struct Cyc_Absyn_Decl*
_temp1064; struct Cyc_Absyn_Stmt* _temp1066; struct Cyc_Absyn_Stmt* _temp1068;
struct Cyc_Absyn_Stmt* _temp1070; struct _tagged_string* _temp1072; struct Cyc_List_List*
_temp1074; struct Cyc_Absyn_Stmt* _temp1076; _LL917: if( _temp915 == Cyc_Absyn_Skip_s){
goto _LL918;} else{ goto _LL919;} _LL919: if(( unsigned int) _temp915 > 1u?((
struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL962:
_temp961=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp915)->f1;
goto _LL920;} else{ goto _LL921;} _LL921: if(( unsigned int) _temp915 > 1u?((
struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL966:
_temp965=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp915)->f1;
goto _LL964; _LL964: _temp963=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp915)->f2; goto _LL922;} else{ goto _LL923;} _LL923: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL968: _temp967=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp915)->f1; if( _temp967 == 0){ goto _LL924;} else{ goto _LL925;}} else{ goto
_LL925;} _LL925: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*)
_temp915)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL970: _temp969=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp915)->f1; goto _LL926;} else{ goto
_LL927;} _LL927: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*)
_temp915)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL976: _temp975=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp915)->f1; goto _LL974; _LL974:
_temp973=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp915)->f2; goto _LL972; _LL972: _temp971=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp915)->f3; goto _LL928;} else{ goto _LL929;}
_LL929: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*) _temp915)->tag
== Cyc_Absyn_Do_s_tag: 0){ _LL984: _temp983=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Do_s_struct*) _temp915)->f1; goto _LL978; _LL978: _temp977=( struct
_tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp915)->f2; _LL982: _temp981=
_temp977.f1; goto _LL980; _LL980: _temp979= _temp977.f2; goto _LL930;} else{
goto _LL931;} _LL931: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*)
_temp915)->tag == Cyc_Absyn_While_s_tag: 0){ _LL988: _temp987=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp915)->f1; _LL992: _temp991= _temp987.f1;
goto _LL990; _LL990: _temp989= _temp987.f2; goto _LL986; _LL986: _temp985=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp915)->f2; goto
_LL932;} else{ goto _LL933;} _LL933: if(( unsigned int) _temp915 > 1u?(( struct
_enum_struct*) _temp915)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL994: _temp993=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp915)->f1; goto
_LL934;} else{ goto _LL935;} _LL935: if(( unsigned int) _temp915 > 1u?(( struct
_enum_struct*) _temp915)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL996: _temp995=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*) _temp915)->f1;
goto _LL936;} else{ goto _LL937;} _LL937: if(( unsigned int) _temp915 > 1u?((
struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1000:
_temp999=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp915)->f1;
goto _LL998; _LL998: _temp997=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp915)->f2; if( _temp997 == 0){ goto _LL938;} else{ goto _LL939;}} else{ goto
_LL939;} _LL939: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*)
_temp915)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1004: _temp1003=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp915)->f1; goto _LL1002;
_LL1002: _temp1001=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp915)->f2; goto _LL940;} else{ goto _LL941;} _LL941: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_For_s_tag: 0){
_LL1020: _temp1019=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp915)->f1; goto _LL1014; _LL1014: _temp1013=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp915)->f2; _LL1018: _temp1017= _temp1013.f1; goto _LL1016; _LL1016:
_temp1015= _temp1013.f2; goto _LL1008; _LL1008: _temp1007=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp915)->f3; _LL1012: _temp1011= _temp1007.f1;
goto _LL1010; _LL1010: _temp1009= _temp1007.f2; goto _LL1006; _LL1006: _temp1005=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp915)->f4; goto
_LL942;} else{ goto _LL943;} _LL943: if(( unsigned int) _temp915 > 1u?(( struct
_enum_struct*) _temp915)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1024: _temp1023=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp915)->f1; goto
_LL1022; _LL1022: _temp1021=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp915)->f2; goto _LL944;} else{ goto _LL945;} _LL945: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL1028: _temp1027=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp915)->f1; goto _LL1026; _LL1026: _temp1025=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp915)->f2; goto _LL946;} else{ goto
_LL947;} _LL947: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*)
_temp915)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1032: _temp1031=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp915)->f1; _temp1033=* _temp1031; _LL1037:
_temp1036=( void*) _temp1033.r; if((( struct _enum_struct*) _temp1036)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1039: _temp1038=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1036)->f1; goto _LL1035;} else{ goto _LL949;}
_LL1035: _temp1034=( struct Cyc_Position_Segment*) _temp1033.loc; goto _LL1030;
_LL1030: _temp1029=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp915)->f2; goto _LL948;} else{ goto _LL949;} _LL949: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL1043: _temp1042=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp915)->f1; _temp1044=* _temp1042; _LL1048: _temp1047=( void*) _temp1044.r;
if((( struct _enum_struct*) _temp1047)->tag == Cyc_Absyn_Let_d_tag){ _LL1061:
_temp1060=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1047)->f1;
goto _LL1056; _LL1056: _temp1055=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1047)->f2; if( _temp1055 == 0){ goto _LL951;} else{ _temp1057=* _temp1055;
_LL1059: _temp1058=( struct Cyc_List_List*) _temp1057.v; goto _LL1054;} _LL1054:
_temp1053=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1047)->f3;
goto _LL1052; _LL1052: _temp1051=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1047)->f4; goto _LL1050; _LL1050: _temp1049=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1047)->f5; goto _LL1046;} else{ goto _LL951;} _LL1046: _temp1045=( struct
Cyc_Position_Segment*) _temp1044.loc; goto _LL1041; _LL1041: _temp1040=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp915)->f2; goto _LL950;}
else{ goto _LL951;} _LL951: if(( unsigned int) _temp915 > 1u?(( struct
_enum_struct*) _temp915)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1065: _temp1064=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp915)->f1; goto
_LL1063; _LL1063: _temp1062=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp915)->f2; goto _LL952;} else{ goto _LL953;} _LL953: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Cut_s_tag: 0){
_LL1067: _temp1066=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp915)->f1; goto _LL954;} else{ goto _LL955;} _LL955: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1069: _temp1068=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp915)->f1; goto _LL956;} else{ goto _LL957;} _LL957: if(( unsigned int)
_temp915 > 1u?(( struct _enum_struct*) _temp915)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1073: _temp1072=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp915)->f1; goto _LL1071; _LL1071: _temp1070=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp915)->f2; goto _LL958;} else{ goto _LL959;}
_LL959: if(( unsigned int) _temp915 > 1u?(( struct _enum_struct*) _temp915)->tag
== Cyc_Absyn_TryCatch_s_tag: 0){ _LL1077: _temp1076=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp915)->f1; goto _LL1075; _LL1075:
_temp1074=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp915)->f2; goto _LL960;} else{ goto _LL916;} _LL918: return; _LL920: { void*
ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp961); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1078=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1078[ 0]=({ struct Cyc_List_List
_temp1079; _temp1079.hd=( void*) ae; _temp1079.tl= 0; _temp1079;}); _temp1078;}));
return;} _LL922: Cyc_NewControlFlow_cf_prepass( env, _temp965); Cyc_NewControlFlow_cf_prepass(
env, _temp963); return; _LL924: return; _LL926: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1080= _temp969; if( _temp1080 == 0){ _throw( Null_Exception);} _temp1080;}));
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1081=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1081[ 0]=({ struct Cyc_List_List _temp1082; _temp1082.hd=( void*) ae;
_temp1082.tl= 0; _temp1082;}); _temp1081;})); return;} _LL928: { void* _temp1085;
void* _temp1087; struct _tuple0 _temp1083= Cyc_NewControlFlow_abstract_guard(
env, _temp975); _LL1088: _temp1087= _temp1083.f1; goto _LL1086; _LL1086:
_temp1085= _temp1083.f2; goto _LL1084; _LL1084: Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1089=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1089[ 0]=({ struct Cyc_List_List
_temp1090; _temp1090.hd=( void*) _temp1087; _temp1090.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1091=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1091[ 0]=({ struct Cyc_List_List _temp1092;
_temp1092.hd=( void*) _temp1085; _temp1092.tl= 0; _temp1092;}); _temp1091;});
_temp1090;}); _temp1089;})); Cyc_NewControlFlow_cf_prepass( env, _temp973); Cyc_NewControlFlow_cf_prepass(
env, _temp971); return;} _LL930: _temp991= _temp981; _temp989= _temp979;
_temp985= _temp983; goto _LL932; _LL932: { void* _temp1095; void* _temp1097;
struct _tuple0 _temp1093= Cyc_NewControlFlow_abstract_guard( env, _temp991);
_LL1098: _temp1097= _temp1093.f1; goto _LL1096; _LL1096: _temp1095= _temp1093.f2;
goto _LL1094; _LL1094: Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1099=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1099[ 0]=({ struct Cyc_List_List _temp1100;
_temp1100.hd=( void*) _temp1097; _temp1100.tl=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1101=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1101[ 0]=({ struct Cyc_List_List _temp1102; _temp1102.hd=( void*)
_temp1095; _temp1102.tl= 0; _temp1102;}); _temp1101;}); _temp1100;}); _temp1099;}));
Cyc_NewControlFlow_cf_prepass( env, _temp985); return;} _LL934: return; _LL936:
return; _LL938:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1103=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1103->tag= Cyc_Core_Impossible_tag; _temp1103->f1=( struct _tagged_string)({
char* _temp1104=( char*)"cf_prepass: goto w/o destination"; struct
_tagged_string _temp1105; _temp1105.curr= _temp1104; _temp1105.base= _temp1104;
_temp1105.last_plus_one= _temp1104 + 33; _temp1105;});( struct _xenum_struct*)
_temp1103;})); _LL940: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1106= _temp1001; if(
_temp1106 == 0){ _throw( Null_Exception);} _temp1106;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1107=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base=
_temp1107; _temp1108.last_plus_one= _temp1107 + 45; _temp1108;})); break;}
my_encloser= next_encloser;} return;} _LL942: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1019); void* _temp1111; void* _temp1113; struct _tuple0 _temp1109= Cyc_NewControlFlow_abstract_guard(
env, _temp1017); _LL1114: _temp1113= _temp1109.f1; goto _LL1112; _LL1112:
_temp1111= _temp1109.f2; goto _LL1110; _LL1110: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1011); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1115=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1115[ 0]=({ struct Cyc_List_List _temp1116;
_temp1116.hd=( void*) ae1; _temp1116.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1117=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1117[ 0]=({ struct Cyc_List_List _temp1118; _temp1118.hd=( void*) _temp1113;
_temp1118.tl=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1119=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1119[ 0]=({
struct Cyc_List_List _temp1120; _temp1120.hd=( void*) _temp1111; _temp1120.tl=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1121=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1121[ 0]=({ struct Cyc_List_List
_temp1122; _temp1122.hd=( void*) ae3; _temp1122.tl= 0; _temp1122;}); _temp1121;});
_temp1120;}); _temp1119;}); _temp1118;}); _temp1117;}); _temp1116;}); _temp1115;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1005); return;}} _LL944: { void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1023); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env,
_temp1021); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1123=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1123[ 0]=({ struct Cyc_List_List _temp1124;
_temp1124.hd=( void*) ae; _temp1124.tl= ael; _temp1124;}); _temp1123;}));
return;} _LL946: { void* _temp1127; void* _temp1129; struct Cyc_NewControlFlow_AbsSynOne
_temp1125= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1027, 0)); _LL1130: _temp1129=( void*) _temp1125.inner_exp; goto
_LL1128; _LL1128: _temp1127=( void*) _temp1125.assigns; goto _LL1126; _LL1126: {
void* ae= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp1129, _temp1127);
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1131=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1131[ 0]=({ struct Cyc_List_List _temp1132; _temp1132.hd=( void*) ae;
_temp1132.tl= 0; _temp1132;}); _temp1131;})); return;}} _LL948: Cyc_NewControlFlow_add_var_root(
env, _temp1038); if( _temp1038->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1133=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1133[ 0]=({ struct Cyc_List_List _temp1134; _temp1134.hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp1038); _temp1134.tl= 0; _temp1134;}); _temp1133;})); void* ae= Cyc_NewControlFlow_abstract_exp_top(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1135= _temp1038->initializer;
if( _temp1135 == 0){ _throw( Null_Exception);} _temp1135;})); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1136=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1136[ 0]=({ struct Cyc_List_List
_temp1137; _temp1137.hd=( void*) ae; _temp1137.tl= 0; _temp1137;}); _temp1136;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1029); return; _LL950: for( 0;
_temp1058 != 0; _temp1058= _temp1058->tl){ Cyc_NewControlFlow_add_var_root( env,(
struct Cyc_Absyn_Vardecl*) _temp1058->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1051); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1138[ 0]=({ struct Cyc_List_List
_temp1139; _temp1139.hd=( void*) ae; _temp1139.tl= 0; _temp1139;}); _temp1138;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1040); return;} _LL952: goto _LL954;
_LL954: goto _LL956; _LL956:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1140=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1140->tag= Cyc_Core_Impossible_tag; _temp1140->f1=( struct _tagged_string)({
char* _temp1141=( char*)"cf_prepass: bad stmt form"; struct _tagged_string
_temp1142; _temp1142.curr= _temp1141; _temp1142.base= _temp1141; _temp1142.last_plus_one=
_temp1141 + 26; _temp1142;});( struct _xenum_struct*) _temp1140;})); _LL958: Cyc_NewControlFlow_cf_prepass(
env, _temp1070); return; _LL960: Cyc_NewControlFlow_cf_prepass( env, _temp1076);{
struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1074);
Cyc_NewControlFlow_cf_set_absexps( s, ael); return;} _LL916:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1143=( char*)"new_control_flow.cyc:894 annot->visited == iteration_num";
struct _tagged_string _temp1144; _temp1144.curr= _temp1143; _temp1144.base=
_temp1143; _temp1144.last_plus_one= _temp1143 + 57; _temp1144;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1145= flow; struct Cyc_Dict_Dict* _temp1151; _LL1147: if( _temp1145
== Cyc_CfFlowInfo_BottomFL){ goto _LL1148;} else{ goto _LL1149;} _LL1149: if((
unsigned int) _temp1145 > 1u?(( struct _enum_struct*) _temp1145)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1152: _temp1151=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1145)->f1; goto _LL1150;} else{ goto _LL1146;} _LL1148: return Cyc_CfFlowInfo_BottomFL;
_LL1150: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1153=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1153->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1153->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1153;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots, root)); _temp1151=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1151, root, pinfo);} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1154=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1154->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1154->f1= _temp1151;( void*) _temp1154;}); _LL1146:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1155= flow; struct Cyc_Dict_Dict* _temp1161; _LL1157: if( _temp1155
== Cyc_CfFlowInfo_BottomFL){ goto _LL1158;} else{ goto _LL1159;} _LL1159: if((
unsigned int) _temp1155 > 1u?(( struct _enum_struct*) _temp1155)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1162: _temp1161=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1155)->f1; goto _LL1160;} else{ goto _LL1156;} _LL1158: return Cyc_CfFlowInfo_BottomFL;
_LL1160: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1163=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1163->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1163->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1163;}); _temp1161=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1161, root,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots, root));} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1164=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1164->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1164->f1= _temp1161;( void*) _temp1164;}); _LL1156:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)
aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1165; _temp1165.f1=
out_flow; _temp1165.f2= out_flow; _temp1165;});} return({ struct _tuple0
_temp1166; _temp1166.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1166.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1166;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1169; struct Cyc_Position_Segment* _temp1170; struct Cyc_Absyn_Stmt*
_temp1172; struct Cyc_Absyn_Exp* _temp1174; struct Cyc_Core_Opt* _temp1176;
struct Cyc_Absyn_Pat* _temp1178; struct Cyc_Absyn_Switch_clause* _temp1167=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1169=* _temp1167; _LL1179:
_temp1178=( struct Cyc_Absyn_Pat*) _temp1169.pattern; goto _LL1177; _LL1177:
_temp1176=( struct Cyc_Core_Opt*) _temp1169.pat_vars; goto _LL1175; _LL1175:
_temp1174=( struct Cyc_Absyn_Exp*) _temp1169.where_clause; goto _LL1173; _LL1173:
_temp1172=( struct Cyc_Absyn_Stmt*) _temp1169.body; goto _LL1171; _LL1171:
_temp1170=( struct Cyc_Position_Segment*) _temp1169.loc; goto _LL1168; _LL1168: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1176->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1176->v);{ void* body_outflow;
if( _temp1174 != 0){ void* _temp1183; void* _temp1185; struct _tuple0 _temp1181=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1180= _temp1174; if( _temp1180 == 0){ _throw( Null_Exception);} _temp1180;}))->loc,
aes, body_inflow); _LL1186: _temp1185= _temp1181.f1; goto _LL1184; _LL1184:
_temp1183= _temp1181.f2; goto _LL1182; _LL1182: aes=( aes->tl)->tl; in_flow=
_temp1183; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1172,
_temp1185);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1172, body_inflow);}{ void* _temp1187= body_outflow; _LL1189: if( _temp1187
== Cyc_CfFlowInfo_BottomFL){ goto _LL1190;} else{ goto _LL1191;} _LL1191: goto
_LL1192; _LL1190: goto _LL1188; _LL1192: Cyc_Tcutil_terr( _temp1172->loc,(
struct _tagged_string)({ char* _temp1193=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1194; _temp1194.curr= _temp1193; _temp1194.base=
_temp1193; _temp1194.last_plus_one= _temp1193 + 38; _temp1194;})); goto _LL1188;
_LL1188:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1195=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1237; struct Cyc_Absyn_Exp* _temp1239; struct Cyc_Absyn_Exp*
_temp1241; struct Cyc_Absyn_Stmt* _temp1243; struct Cyc_Absyn_Stmt* _temp1245;
struct Cyc_Absyn_Stmt* _temp1247; struct Cyc_Absyn_Stmt* _temp1249; struct Cyc_Absyn_Exp*
_temp1251; struct Cyc_Absyn_Stmt* _temp1253; struct _tuple3 _temp1255; struct
Cyc_Absyn_Stmt* _temp1257; struct Cyc_Absyn_Exp* _temp1259; struct _tuple3
_temp1261; struct Cyc_Absyn_Stmt* _temp1263; struct Cyc_Absyn_Exp* _temp1265;
struct Cyc_Absyn_Stmt* _temp1267; struct Cyc_Absyn_Stmt* _temp1269; struct
_tuple3 _temp1271; struct Cyc_Absyn_Stmt* _temp1273; struct Cyc_Absyn_Exp*
_temp1275; struct _tuple3 _temp1277; struct Cyc_Absyn_Stmt* _temp1279; struct
Cyc_Absyn_Exp* _temp1281; struct Cyc_Absyn_Exp* _temp1283; struct Cyc_Absyn_Stmt*
_temp1285; struct Cyc_Absyn_Switch_clause** _temp1287; struct Cyc_Absyn_Switch_clause*
_temp1289; struct Cyc_List_List* _temp1290; struct Cyc_Absyn_Stmt* _temp1292;
struct Cyc_Absyn_Stmt* _temp1294; struct Cyc_Absyn_Stmt* _temp1296; struct
_tagged_string* _temp1298; struct Cyc_List_List* _temp1300; struct Cyc_Absyn_Exp*
_temp1302; struct Cyc_List_List* _temp1304; struct Cyc_Absyn_Stmt* _temp1306;
struct Cyc_Absyn_Stmt* _temp1308; struct Cyc_Absyn_Decl* _temp1310; struct Cyc_Absyn_Decl
_temp1312; struct Cyc_Position_Segment* _temp1313; void* _temp1315; struct Cyc_Absyn_Vardecl*
_temp1317; struct Cyc_Absyn_Stmt* _temp1319; struct Cyc_Absyn_Decl* _temp1321;
struct Cyc_Absyn_Decl _temp1323; struct Cyc_Position_Segment* _temp1324; void*
_temp1326; int _temp1328; struct Cyc_Absyn_Exp* _temp1330; struct Cyc_Core_Opt*
_temp1332; struct Cyc_Core_Opt* _temp1334; struct Cyc_Core_Opt _temp1336; struct
Cyc_List_List* _temp1337; struct Cyc_Absyn_Pat* _temp1339; struct Cyc_Absyn_Stmt*
_temp1341; struct _tagged_string* _temp1343; _LL1197: if( _temp1195 == Cyc_Absyn_Skip_s){
goto _LL1198;} else{ goto _LL1199;} _LL1199: if(( unsigned int) _temp1195 > 1u?((
struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1238:
_temp1237=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1195)->f1; if( _temp1237 == 0){ goto _LL1200;} else{ goto _LL1201;}} else{
goto _LL1201;} _LL1201: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1240: _temp1239=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1195)->f1; goto _LL1202;} else{ goto
_LL1203;} _LL1203: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1242: _temp1241=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp1195)->f1; goto _LL1204;} else{ goto
_LL1205;} _LL1205: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1246: _temp1245=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1195)->f1; goto _LL1244; _LL1244: _temp1243=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1195)->f2; goto
_LL1206;} else{ goto _LL1207;} _LL1207: if(( unsigned int) _temp1195 > 1u?((
struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1252:
_temp1251=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1195)->f1; goto _LL1250; _LL1250: _temp1249=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1195)->f2; goto _LL1248; _LL1248:
_temp1247=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1195)->f3; goto _LL1208;} else{ goto _LL1209;} _LL1209: if(( unsigned int)
_temp1195 > 1u?(( struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1256: _temp1255=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1195)->f1; _LL1260: _temp1259= _temp1255.f1; goto _LL1258; _LL1258:
_temp1257= _temp1255.f2; goto _LL1254; _LL1254: _temp1253=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1195)->f2; goto _LL1210;} else{ goto
_LL1211;} _LL1211: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1268: _temp1267=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1195)->f1; goto _LL1262; _LL1262: _temp1261=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1195)->f2; _LL1266:
_temp1265= _temp1261.f1; goto _LL1264; _LL1264: _temp1263= _temp1261.f2; goto
_LL1212;} else{ goto _LL1213;} _LL1213: if(( unsigned int) _temp1195 > 1u?((
struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1284:
_temp1283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1195)->f1;
goto _LL1278; _LL1278: _temp1277=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1195)->f2; _LL1282: _temp1281= _temp1277.f1; goto _LL1280; _LL1280:
_temp1279= _temp1277.f2; goto _LL1272; _LL1272: _temp1271=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1195)->f3; _LL1276: _temp1275= _temp1271.f1;
goto _LL1274; _LL1274: _temp1273= _temp1271.f2; goto _LL1270; _LL1270: _temp1269=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1195)->f4; goto
_LL1214;} else{ goto _LL1215;} _LL1215: if(( unsigned int) _temp1195 > 1u?((
struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1286:
_temp1285=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1195)->f1; if( _temp1285 == 0){ goto _LL1216;} else{ goto _LL1217;}} else{
goto _LL1217;} _LL1217: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1291: _temp1290=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1195)->f1; goto _LL1288; _LL1288:
_temp1287=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1195)->f2; if( _temp1287 == 0){ goto _LL1219;} else{ _temp1289=* _temp1287;
goto _LL1218;}} else{ goto _LL1219;} _LL1219: if(( unsigned int) _temp1195 > 1u?((
struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1293:
_temp1292=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1195)->f1; goto _LL1220;} else{ goto _LL1221;} _LL1221: if(( unsigned int)
_temp1195 > 1u?(( struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1295: _temp1294=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1195)->f1; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(( unsigned int)
_temp1195 > 1u?(( struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1299: _temp1298=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1195)->f1; goto _LL1297; _LL1297: _temp1296=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1195)->f2; goto _LL1224;} else{ goto
_LL1225;} _LL1225: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1303: _temp1302=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1195)->f1; goto _LL1301; _LL1301:
_temp1300=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1195)->f2; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(( unsigned int)
_temp1195 > 1u?(( struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1307: _temp1306=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1195)->f1; goto _LL1305; _LL1305: _temp1304=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1195)->f2; goto _LL1228;} else{ goto
_LL1229;} _LL1229: if(( unsigned int) _temp1195 > 1u?(( struct _enum_struct*)
_temp1195)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1311: _temp1310=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1195)->f1; _temp1312=* _temp1310; _LL1316:
_temp1315=( void*) _temp1312.r; if((( struct _enum_struct*) _temp1315)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1318: _temp1317=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1315)->f1; goto _LL1314;} else{ goto _LL1231;}
_LL1314: _temp1313=( struct Cyc_Position_Segment*) _temp1312.loc; goto _LL1309;
_LL1309: _temp1308=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1195)->f2; goto _LL1230;} else{ goto _LL1231;} _LL1231: if(( unsigned int)
_temp1195 > 1u?(( struct _enum_struct*) _temp1195)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1322: _temp1321=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1195)->f1; _temp1323=* _temp1321; _LL1327: _temp1326=( void*) _temp1323.r;
if((( struct _enum_struct*) _temp1326)->tag == Cyc_Absyn_Let_d_tag){ _LL1340:
_temp1339=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1326)->f1;
goto _LL1335; _LL1335: _temp1334=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1326)->f2; if( _temp1334 == 0){ goto _LL1233;} else{ _temp1336=* _temp1334;
_LL1338: _temp1337=( struct Cyc_List_List*) _temp1336.v; goto _LL1333;} _LL1333:
_temp1332=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1326)->f3;
goto _LL1331; _LL1331: _temp1330=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1326)->f4; goto _LL1329; _LL1329: _temp1328=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1326)->f5; goto _LL1325;} else{ goto _LL1233;} _LL1325: _temp1324=( struct
Cyc_Position_Segment*) _temp1323.loc; goto _LL1320; _LL1320: _temp1319=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1195)->f2; goto _LL1232;}
else{ goto _LL1233;} _LL1233: if(( unsigned int) _temp1195 > 1u?(( struct
_enum_struct*) _temp1195)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1344: _temp1343=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1195)->f1; goto
_LL1342; _LL1342: _temp1341=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1195)->f2; goto _LL1234;} else{ goto _LL1235;} _LL1235: goto _LL1236;
_LL1198: return in_flow; _LL1200: return Cyc_CfFlowInfo_BottomFL; _LL1202: Cyc_CfAbsexp_eval_absexp(
env, _temp1239->loc,( void*) aes->hd, in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1204: return Cyc_CfAbsexp_eval_absexp( env, _temp1241->loc,( void*) aes->hd,
in_flow); _LL1206: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1243,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1245, in_flow)); _LL1208: { void*
_temp1347; void* _temp1349; struct _tuple0 _temp1345= Cyc_NewControlFlow_cf_evalguard(
env, _temp1251->loc, aes, in_flow); _LL1350: _temp1349= _temp1345.f1; goto
_LL1348; _LL1348: _temp1347= _temp1345.f2; goto _LL1346; _LL1346: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1249, _temp1349), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1247, _temp1347));} _LL1210: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1257))->flow; void* _temp1353; void* _temp1355; struct _tuple0
_temp1351= Cyc_NewControlFlow_cf_evalguard( env, _temp1259->loc, aes, e_inflow);
_LL1356: _temp1355= _temp1351.f1; goto _LL1354; _LL1354: _temp1353= _temp1351.f2;
goto _LL1352; _LL1352: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1253, _temp1355); Cyc_NewControlFlow_update_flow( _temp1257,
body_outflow); return _temp1353;}} _LL1212: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1267, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1263))->flow; void* _temp1359; void* _temp1361; struct
_tuple0 _temp1357= Cyc_NewControlFlow_cf_evalguard( env, _temp1265->loc, aes,
e_inflow); _LL1362: _temp1361= _temp1357.f1; goto _LL1360; _LL1360: _temp1359=
_temp1357.f2; goto _LL1358; _LL1358: Cyc_NewControlFlow_update_flow( _temp1267,
_temp1361); return _temp1359;} _LL1214: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1283->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1279))->flow; void* _temp1365; void* _temp1367; struct _tuple0
_temp1363= Cyc_NewControlFlow_cf_evalguard( env, _temp1281->loc, aes->tl,
e2_inflow); _LL1368: _temp1367= _temp1363.f1; goto _LL1366; _LL1366: _temp1365=
_temp1363.f2; goto _LL1364; _LL1364: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1269, _temp1367); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1273))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1275->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1279, e3_outflow); return _temp1365;}} _LL1216: return Cyc_CfFlowInfo_BottomFL;
_LL1218: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1289->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1292=( struct
Cyc_Absyn_Stmt*) _temp1289->body; goto _LL1220;} _LL1220: _temp1294= _temp1292;
goto _LL1222; _LL1222: _temp1296= _temp1294; goto _LL1224; _LL1224: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1369= _temp1296; if(
_temp1369 == 0){ _throw( Null_Exception);} _temp1369;}), in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1226: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1302->loc,( void*) aes->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1300, aes->tl,
in_flow); return Cyc_CfFlowInfo_BottomFL; _LL1228: { struct Cyc_NewControlFlow_AnalEnv
_temp1372; void* _temp1373; int _temp1375; struct Cyc_Dict_Dict* _temp1377;
struct Cyc_NewControlFlow_AnalEnv* _temp1370= env; _temp1372=* _temp1370;
_LL1378: _temp1377=( struct Cyc_Dict_Dict*) _temp1372.roots; goto _LL1376;
_LL1376: _temp1375=( int) _temp1372.in_try; goto _LL1374; _LL1374: _temp1373=(
void*) _temp1372.tryflow; goto _LL1371; _LL1371: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1306, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1375;( void*)( env->tryflow=( void*) _temp1373); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1304,
aes, scs_inflow); return s_outflow;}} _LL1230: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1379; _temp1379.hd=( void*) _temp1317; _temp1379.tl= 0;
_temp1379;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1317->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1380= e; if( _temp1380 == 0){ _throw( Null_Exception);}
_temp1380;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1308, body_inflow);} _LL1232: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1337); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1330->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1337); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1319, body_inflow);} _LL1234: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1341, in_flow); _LL1236:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1381=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1381->tag= Cyc_Core_Impossible_tag; _temp1381->f1=( struct _tagged_string)({
char* _temp1382=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1383; _temp1383.curr= _temp1382; _temp1383.base=
_temp1382; _temp1383.last_plus_one= _temp1382 + 62; _temp1383;});( struct
_xenum_struct*) _temp1381;})); _LL1196:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1384=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_EmptyAnnot_struct)); _temp1384->tag= Cyc_Absyn_EmptyAnnot_tag;(
struct _xenum_struct*) _temp1384;});{ void* _temp1385=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp1407; struct Cyc_Absyn_Stmt* _temp1409; struct Cyc_Absyn_Stmt* _temp1411;
struct Cyc_Absyn_Stmt* _temp1413; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_Absyn_Stmt*
_temp1417; struct _tuple3 _temp1419; struct Cyc_Absyn_Stmt* _temp1421; struct
Cyc_Absyn_Exp* _temp1423; struct _tuple3 _temp1425; struct Cyc_Absyn_Stmt*
_temp1427; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_Absyn_Exp* _temp1431;
struct Cyc_Absyn_Stmt* _temp1433; struct _tuple3 _temp1435; struct Cyc_Absyn_Stmt*
_temp1437; struct Cyc_Absyn_Exp* _temp1439; struct _tuple3 _temp1441; struct Cyc_Absyn_Stmt*
_temp1443; struct Cyc_Absyn_Exp* _temp1445; struct Cyc_Absyn_Stmt* _temp1447;
struct Cyc_Absyn_Stmt* _temp1449; struct _tagged_string* _temp1451; struct Cyc_Absyn_Stmt*
_temp1453; struct Cyc_Absyn_Decl* _temp1455; struct Cyc_List_List* _temp1457;
struct Cyc_Absyn_Stmt* _temp1459; struct Cyc_List_List* _temp1461; struct Cyc_Absyn_Exp*
_temp1463; _LL1387: if(( unsigned int) _temp1385 > 1u?(( struct _enum_struct*)
_temp1385)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1410: _temp1409=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1385)->f1; goto _LL1408; _LL1408: _temp1407=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1385)->f2; goto
_LL1388;} else{ goto _LL1389;} _LL1389: if(( unsigned int) _temp1385 > 1u?((
struct _enum_struct*) _temp1385)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1416:
_temp1415=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1385)->f1; goto _LL1414; _LL1414: _temp1413=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1385)->f2; goto _LL1412; _LL1412:
_temp1411=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1385)->f3; goto _LL1390;} else{ goto _LL1391;} _LL1391: if(( unsigned int)
_temp1385 > 1u?(( struct _enum_struct*) _temp1385)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1432: _temp1431=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1385)->f1; goto _LL1426; _LL1426: _temp1425=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1385)->f2; _LL1430: _temp1429= _temp1425.f1; goto _LL1428; _LL1428:
_temp1427= _temp1425.f2; goto _LL1420; _LL1420: _temp1419=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1385)->f3; _LL1424: _temp1423= _temp1419.f1;
goto _LL1422; _LL1422: _temp1421= _temp1419.f2; goto _LL1418; _LL1418: _temp1417=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1385)->f4; goto
_LL1392;} else{ goto _LL1393;} _LL1393: if(( unsigned int) _temp1385 > 1u?((
struct _enum_struct*) _temp1385)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1436:
_temp1435=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1385)->f1;
_LL1440: _temp1439= _temp1435.f1; goto _LL1438; _LL1438: _temp1437= _temp1435.f2;
goto _LL1434; _LL1434: _temp1433=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1385)->f2; goto _LL1394;} else{ goto _LL1395;} _LL1395: if(( unsigned int)
_temp1385 > 1u?(( struct _enum_struct*) _temp1385)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1448: _temp1447=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1385)->f1; goto _LL1442; _LL1442: _temp1441=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1385)->f2; _LL1446: _temp1445= _temp1441.f1; goto _LL1444; _LL1444:
_temp1443= _temp1441.f2; goto _LL1396;} else{ goto _LL1397;} _LL1397: if((
unsigned int) _temp1385 > 1u?(( struct _enum_struct*) _temp1385)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1452: _temp1451=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1385)->f1; goto _LL1450; _LL1450: _temp1449=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1385)->f2; goto _LL1398;} else{ goto
_LL1399;} _LL1399: if(( unsigned int) _temp1385 > 1u?(( struct _enum_struct*)
_temp1385)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1456: _temp1455=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1385)->f1; goto _LL1454; _LL1454:
_temp1453=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1385)->f2;
goto _LL1400;} else{ goto _LL1401;} _LL1401: if(( unsigned int) _temp1385 > 1u?((
struct _enum_struct*) _temp1385)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1460:
_temp1459=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1385)->f1; goto _LL1458; _LL1458: _temp1457=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1385)->f2; goto _LL1402;} else{ goto
_LL1403;} _LL1403: if(( unsigned int) _temp1385 > 1u?(( struct _enum_struct*)
_temp1385)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1464: _temp1463=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1385)->f1; goto _LL1462; _LL1462:
_temp1461=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1385)->f2; goto _LL1404;} else{ goto _LL1405;} _LL1405: goto _LL1406;
_LL1388: _temp1413= _temp1409; _temp1411= _temp1407; goto _LL1390; _LL1390: Cyc_NewControlFlow_cf_postpass(
_temp1413); Cyc_NewControlFlow_cf_postpass( _temp1411); return; _LL1392: Cyc_NewControlFlow_cf_postpass(
_temp1421); _temp1437= _temp1417; _temp1433= _temp1427; goto _LL1394; _LL1394:
_temp1447= _temp1433; _temp1443= _temp1437; goto _LL1396; _LL1396: Cyc_NewControlFlow_cf_postpass(
_temp1443); _temp1449= _temp1447; goto _LL1398; _LL1398: _temp1453= _temp1449;
goto _LL1400; _LL1400: Cyc_NewControlFlow_cf_postpass( _temp1453); return;
_LL1402: Cyc_NewControlFlow_cf_postpass( _temp1459); _temp1461= _temp1457; goto
_LL1404; _LL1404: for( 0; _temp1461 != 0; _temp1461= _temp1461->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1461->hd)->body);} return; _LL1406:
return; _LL1386:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1465; _temp1465.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1466=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv)
* 1); _temp1466[ 0]=({ struct Cyc_NewControlFlow_SharedAbsEnv _temp1467;
_temp1467.all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1467.comprehension_vars= 0;
_temp1467;}); _temp1466;}); _temp1465.lhsides= 0; _temp1465.access_path= 0;
_temp1465;}); struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1470; struct Cyc_List_List*
_temp1472; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1474; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1476; struct Cyc_List_List* _temp1477; struct Cyc_Dict_Dict* _temp1479;
struct Cyc_NewControlFlow_AbsEnv _temp1468= absenv; _LL1475: _temp1474=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1468.shared; _temp1476=* _temp1474;
_LL1480: _temp1479=( struct Cyc_Dict_Dict*) _temp1476.all_roots; goto _LL1478;
_LL1478: _temp1477=( struct Cyc_List_List*) _temp1476.comprehension_vars; goto
_LL1473; _LL1473: _temp1472=( struct Cyc_List_List*) _temp1468.lhsides; goto
_LL1471; _LL1471: _temp1470=( struct Cyc_List_List*) _temp1468.access_path; goto
_LL1469; _LL1469: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1481=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)
* 1); _temp1481[ 0]=({ struct Cyc_NewControlFlow_AnalEnv _temp1482; _temp1482.roots=
_temp1479; _temp1482.in_try= 0; _temp1482.tryflow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1482;}); _temp1481;}); void* in_flow=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1483=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1483->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp1483->f1=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);(
void*) _temp1483;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( analenv,
in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow( analenv,
in_flow, _temp1477);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1484= out_flow; _LL1486: if( _temp1484 == Cyc_CfFlowInfo_BottomFL){
goto _LL1487;} else{ goto _LL1488;} _LL1488: goto _LL1489; _LL1487: goto _LL1485;
_LL1489: if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp1490=( char*)"function may complete without returning a value";
struct _tagged_string _temp1491; _temp1491.curr= _temp1490; _temp1491.base=
_temp1490; _temp1491.last_plus_one= _temp1490 + 48; _temp1491;}));} goto _LL1485;
_LL1485:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1492=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1502; struct
Cyc_List_List* _temp1504; struct _tuple1* _temp1506; struct Cyc_List_List*
_temp1508; struct _tagged_string* _temp1510; _LL1494: if((( struct _enum_struct*)
_temp1492)->tag == Cyc_Absyn_Fn_d_tag){ _LL1503: _temp1502=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1492)->f1; goto _LL1495;} else{ goto _LL1496;}
_LL1496: if((( struct _enum_struct*) _temp1492)->tag == Cyc_Absyn_Using_d_tag){
_LL1507: _temp1506=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1492)->f1; goto _LL1505; _LL1505: _temp1504=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1492)->f2; goto _LL1497;} else{ goto
_LL1498;} _LL1498: if((( struct _enum_struct*) _temp1492)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1511: _temp1510=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1492)->f1; goto _LL1509; _LL1509: _temp1508=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1492)->f2; goto _LL1499;} else{ goto
_LL1500;} _LL1500: goto _LL1501; _LL1495: Cyc_NewControlFlow_cf_check_fun(
_temp1502); goto _LL1493; _LL1497: _temp1508= _temp1504; goto _LL1499; _LL1499:
Cyc_NewControlFlow_cf_check( _temp1508); goto _LL1493; _LL1501: goto _LL1493;
_LL1493:;}}