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
e))));} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path= path->tl){ void* _temp68=( void*)(( struct Cyc_Absyn_Exp*) path->hd)->r;
struct Cyc_Absyn_Exp* _temp82; struct Cyc_Absyn_Exp* _temp84; struct
_tagged_string* _temp86; struct Cyc_Absyn_Exp* _temp88; struct _tagged_string*
_temp90; struct Cyc_Absyn_Exp* _temp92; struct Cyc_Absyn_Exp* _temp94; struct
Cyc_Absyn_Exp* _temp96; _LL70: if((( struct _enum_struct*) _temp68)->tag == Cyc_Absyn_Address_e_tag){
_LL83: _temp82=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp68)->f1; goto _LL71;} else{ goto _LL72;} _LL72: if((( struct _enum_struct*)
_temp68)->tag == Cyc_Absyn_Deref_e_tag){ _LL85: _temp84=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp68)->f1; goto _LL73;} else{ goto _LL74;}
_LL74: if((( struct _enum_struct*) _temp68)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL89: _temp88=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp68)->f1; goto _LL87; _LL87: _temp86=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp68)->f2; goto _LL75;} else{ goto _LL76;} _LL76: if((( struct _enum_struct*)
_temp68)->tag == Cyc_Absyn_StructMember_e_tag){ _LL93: _temp92=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp68)->f1; goto _LL91; _LL91:
_temp90=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp68)->f2; goto _LL77;} else{ goto _LL78;} _LL78: if((( struct _enum_struct*)
_temp68)->tag == Cyc_Absyn_Subscript_e_tag){ _LL97: _temp96=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp68)->f1; goto _LL95; _LL95: _temp94=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp68)->f2;
goto _LL79;} else{ goto _LL80;} _LL80: goto _LL81; _LL71: path->tl == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp98=( char*)"new_control_flow.cyc:235 path->tl == null";
struct _tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98;
_temp99.last_plus_one= _temp98 + 42; _temp99;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL73: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL69; _LL75: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp90= _temp86; goto _LL77; _LL77: absop= Cyc_CfAbsexp_mkMemberOp(
absop,({ struct Cyc_CfFlowInfo_StructF_struct* _temp100=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp100->tag= Cyc_CfFlowInfo_StructF_tag;
_temp100->f1= _temp90;( void*) _temp100;})); goto _LL69; _LL79: { unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp94); absop= Cyc_CfAbsexp_mkMemberOp( absop,({
struct Cyc_CfFlowInfo_TupleF_struct* _temp101=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp101->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp101->f1=( int) i;( void*) _temp101;})); goto
_LL69;} _LL81:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp102=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp102->tag= Cyc_Core_Impossible_tag; _temp102->f1=( struct _tagged_string)({
char* _temp103=( char*)"abstract_exp: unexpected acces path element"; struct
_tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 44; _temp104;});( struct _xenum_struct*)
_temp102;})); _LL69:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp105=(
void*) e->r; struct Cyc_Absyn_Exp* _temp191; struct Cyc_Absyn_Exp* _temp193;
struct Cyc_List_List* _temp195; struct Cyc_Absyn_Exp* _temp197; void* _temp199;
int _temp201; void* _temp203; void* _temp205; int _temp207; void* _temp209; void*
_temp211; void* _temp213; void* _temp215; struct Cyc_Absyn_Fndecl* _temp217;
struct _tuple1* _temp219; void* _temp221; struct Cyc_Absyn_Vardecl* _temp223;
struct _tuple1* _temp225; void* _temp227; struct Cyc_Absyn_Vardecl* _temp229;
struct _tuple1* _temp231; void* _temp233; struct Cyc_Absyn_Vardecl* _temp235;
struct _tuple1* _temp237; void* _temp239; struct Cyc_Absyn_Vardecl* _temp241;
struct _tuple1* _temp243; struct Cyc_Absyn_Enumfield* _temp245; struct Cyc_Absyn_Enumdecl*
_temp247; struct Cyc_List_List* _temp249; struct Cyc_Core_Opt* _temp251; struct
Cyc_Core_Opt* _temp253; struct Cyc_Absyn_Enumfield* _temp255; struct Cyc_Absyn_Xenumdecl*
_temp257; struct Cyc_List_List* _temp259; struct Cyc_Core_Opt* _temp261; void*
_temp263; void* _temp265; void* _temp267; struct Cyc_Absyn_Exp* _temp269; struct
Cyc_Core_Opt* _temp271; struct Cyc_Absyn_Exp* _temp273; struct Cyc_Absyn_Exp*
_temp275; struct Cyc_Core_Opt* _temp277; struct Cyc_Absyn_Exp* _temp279; struct
Cyc_List_List* _temp281; void* _temp283; struct Cyc_Absyn_Exp* _temp285; void*
_temp287; struct Cyc_List_List* _temp289; struct Cyc_Absyn_Exp* _temp291; struct
Cyc_Absyn_Exp* _temp293; struct Cyc_Absyn_Exp* _temp295; struct Cyc_Absyn_Exp*
_temp297; struct Cyc_Absyn_Exp* _temp299; struct _tagged_string* _temp301;
struct Cyc_Absyn_Exp* _temp303; struct _tagged_string* _temp305; struct Cyc_Absyn_Exp*
_temp307; struct Cyc_List_List* _temp309; struct Cyc_Absyn_Structdecl* _temp311;
struct Cyc_List_List* _temp313; struct Cyc_Core_Opt* _temp315; struct _tuple1*
_temp317; struct Cyc_List_List* _temp319; int _temp321; void* _temp323; struct
Cyc_Absyn_Exp* _temp325; struct Cyc_Absyn_Exp* _temp327; struct Cyc_Absyn_Exp*
_temp329; struct Cyc_Absyn_Exp* _temp331; struct Cyc_Absyn_Exp* _temp333; struct
Cyc_Absyn_Stmt* _temp335; struct Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Exp*
_temp339; struct Cyc_Absyn_Exp* _temp341; struct Cyc_Absyn_Exp* _temp343; struct
Cyc_Absyn_Vardecl* _temp345; void* _temp347; struct _tuple1* _temp349; struct
_tuple1* _temp351; struct Cyc_List_List* _temp353; struct Cyc_Absyn_Exp*
_temp355; struct Cyc_List_List* _temp357; struct Cyc_Core_Opt* _temp359; struct
Cyc_List_List* _temp361; struct _tuple2* _temp363; struct Cyc_Absyn_Fndecl*
_temp365; struct Cyc_Absyn_Exp* _temp367; _LL107: if((( struct _enum_struct*)
_temp105)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL192: _temp191=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp105)->f1; goto _LL108;} else{
goto _LL109;} _LL109: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL194: _temp193=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp105)->f1; goto _LL110;} else{ goto _LL111;} _LL111: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL198: _temp197=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp105)->f1;
goto _LL196; _LL196: _temp195=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp105)->f2; goto _LL112;} else{ goto _LL113;} _LL113: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Const_e_tag){ _LL200: _temp199=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp105)->f1; if(( unsigned int) _temp199 > 1u?((
struct _enum_struct*) _temp199)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL204:
_temp203=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp199)->f1; if( _temp203
== Cyc_Absyn_Signed){ goto _LL202;} else{ goto _LL115;} _LL202: _temp201=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp199)->f2; if( _temp201 == 0){ goto _LL114;}
else{ goto _LL115;}} else{ goto _LL115;}} else{ goto _LL115;} _LL115: if(((
struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Const_e_tag){ _LL206: _temp205=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp105)->f1; if(( unsigned int)
_temp205 > 1u?(( struct _enum_struct*) _temp205)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL210: _temp209=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp205)->f1; if(
_temp209 == Cyc_Absyn_Signed){ goto _LL208;} else{ goto _LL117;} _LL208:
_temp207=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp205)->f2; if( _temp207 ==
1){ goto _LL116;} else{ goto _LL117;}} else{ goto _LL117;}} else{ goto _LL117;}
_LL117: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL212: _temp211=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp105)->f1;
goto _LL118;} else{ goto _LL119;} _LL119: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Const_e_tag){ _LL214: _temp213=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp105)->f1; goto _LL120;} else{ goto _LL121;} _LL121: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){ _LL220: _temp219=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1; goto _LL216; _LL216:
_temp215=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f2; if((
unsigned int) _temp215 > 1u?(( struct _enum_struct*) _temp215)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL218: _temp217=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp215)->f1; goto _LL122;} else{ goto _LL123;}} else{ goto _LL123;} _LL123:
if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){ _LL226:
_temp225=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL222; _LL222: _temp221=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp221 > 1u?(( struct _enum_struct*)
_temp221)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL224: _temp223=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp221)->f1; goto _LL124;} else{ goto
_LL125;}} else{ goto _LL125;} _LL125: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Var_e_tag){ _LL232: _temp231=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f1; goto _LL228; _LL228: _temp227=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp227 > 1u?(( struct _enum_struct*)
_temp227)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL230: _temp229=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp227)->f1; goto _LL126;} else{ goto _LL127;}}
else{ goto _LL127;} _LL127: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){
_LL238: _temp237=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL234; _LL234: _temp233=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp233 > 1u?(( struct _enum_struct*)
_temp233)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL236: _temp235=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp233)->f1; goto _LL128;} else{ goto _LL129;}}
else{ goto _LL129;} _LL129: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){
_LL244: _temp243=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL240; _LL240: _temp239=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp239 > 1u?(( struct _enum_struct*)
_temp239)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL242: _temp241=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp239)->f1; goto _LL130;} else{ goto _LL131;}}
else{ goto _LL131;} _LL131: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Enum_e_tag){
_LL254: _temp253=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp105)->f1; goto _LL252; _LL252: _temp251=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp105)->f2; goto _LL250; _LL250: _temp249=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp105)->f3; goto _LL248;
_LL248: _temp247=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp105)->f4; goto _LL246; _LL246: _temp245=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp105)->f5; goto _LL132;} else{ goto _LL133;}
_LL133: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Xenum_e_tag){
_LL262: _temp261=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp105)->f1; goto _LL260; _LL260: _temp259=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp105)->f2; goto _LL258; _LL258: _temp257=( struct
Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp105)->f3; goto
_LL256; _LL256: _temp255=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp105)->f4; goto _LL134;} else{ goto _LL135;} _LL135: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Malloc_e_tag){ _LL264: _temp263=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp105)->f1; if((( struct
_enum_struct*) _temp263)->tag == Cyc_Absyn_Typ_m_tag){ _LL266: _temp265=( void*)((
struct Cyc_Absyn_Typ_m_struct*) _temp263)->f1; goto _LL136;} else{ goto _LL137;}}
else{ goto _LL137;} _LL137: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Malloc_e_tag){
_LL268: _temp267=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp105)->f1;
goto _LL138;} else{ goto _LL139;} _LL139: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL274: _temp273=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp105)->f1; goto _LL272; _LL272: _temp271=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f2; if(
_temp271 == 0){ goto _LL270;} else{ goto _LL141;} _LL270: _temp269=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f3; goto _LL140;} else{ goto
_LL141;} _LL141: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL280: _temp279=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp105)->f1; goto _LL278; _LL278: _temp277=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp105)->f2; goto _LL276; _LL276: _temp275=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f3;
goto _LL142;} else{ goto _LL143;} _LL143: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Primop_e_tag){ _LL284: _temp283=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp105)->f1; goto _LL282; _LL282: _temp281=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp105)->f2; goto _LL144;} else{ goto _LL145;}
_LL145: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Cast_e_tag){
_LL288: _temp287=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp105)->f1; goto
_LL286; _LL286: _temp285=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp105)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_FnCall_e_tag){ _LL292: _temp291=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp105)->f1; goto
_LL290; _LL290: _temp289=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp105)->f2; goto _LL148;} else{ goto _LL149;} _LL149: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Subscript_e_tag){ _LL296: _temp295=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp105)->f1;
goto _LL294; _LL294: _temp293=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp105)->f2; goto _LL150;} else{ goto _LL151;} _LL151: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Address_e_tag){ _LL298: _temp297=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp105)->f1; goto
_LL152;} else{ goto _LL153;} _LL153: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Deref_e_tag){ _LL300: _temp299=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp105)->f1; goto _LL154;} else{ goto _LL155;}
_LL155: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL304: _temp303=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp105)->f1; goto _LL302; _LL302: _temp301=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp105)->f2; goto _LL156;} else{ goto _LL157;}
_LL157: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StructMember_e_tag){
_LL308: _temp307=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp105)->f1; goto _LL306; _LL306: _temp305=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp105)->f2; goto _LL158;} else{ goto _LL159;}
_LL159: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Tuple_e_tag){
_LL310: _temp309=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp105)->f1; goto _LL160;} else{ goto _LL161;} _LL161: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Struct_e_tag){ _LL318: _temp317=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp105)->f1; goto _LL316;
_LL316: _temp315=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp105)->f2; goto _LL314; _LL314: _temp313=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp105)->f3; goto _LL312; _LL312: _temp311=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp105)->f4;
goto _LL162;} else{ goto _LL163;} _LL163: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Array_e_tag){ _LL322: _temp321=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp105)->f1; goto _LL320; _LL320: _temp319=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp105)->f2; goto _LL164;} else{ goto _LL165;}
_LL165: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Increment_e_tag){
_LL326: _temp325=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp105)->f1; goto _LL324; _LL324: _temp323=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp105)->f2; goto _LL166;} else{ goto _LL167;} _LL167: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Throw_e_tag){ _LL328: _temp327=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp105)->f1; goto
_LL168;} else{ goto _LL169;} _LL169: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL334: _temp333=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp105)->f1; goto _LL332; _LL332:
_temp331=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp105)->f2; goto _LL330; _LL330: _temp329=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp105)->f3; goto _LL170;} else{ goto _LL171;}
_LL171: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL336: _temp335=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp105)->f1; goto _LL172;} else{ goto _LL173;} _LL173: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL340: _temp339=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp105)->f1; goto
_LL338; _LL338: _temp337=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp105)->f2; goto _LL174;} else{ goto _LL175;} _LL175: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL346:
_temp345=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp105)->f1; goto _LL344; _LL344: _temp343=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp105)->f2; goto _LL342; _LL342: _temp341=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp105)->f3;
goto _LL176;} else{ goto _LL177;} _LL177: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Var_e_tag){ _LL350: _temp349=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f1; goto _LL348; _LL348: _temp347=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if( _temp347 == Cyc_Absyn_Unresolved_b){ goto _LL178;} else{ goto
_LL179;}} else{ goto _LL179;} _LL179: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL352: _temp351=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp105)->f1; goto _LL180;} else{ goto _LL181;} _LL181: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL356: _temp355=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp105)->f1;
goto _LL354; _LL354: _temp353=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp105)->f2; goto _LL182;} else{ goto _LL183;} _LL183: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL360:
_temp359=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp105)->f1; goto _LL358; _LL358: _temp357=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp105)->f2; goto _LL184;} else{ goto
_LL185;} _LL185: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL364: _temp363=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp105)->f1; goto _LL362; _LL362: _temp361=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp105)->f2; goto _LL186;} else{ goto _LL187;}
_LL187: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Codegen_e_tag){
_LL366: _temp365=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp105)->f1; goto _LL188;} else{ goto _LL189;} _LL189: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Fill_e_tag){ _LL368: _temp367=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp105)->f1; goto
_LL190;} else{ goto _LL106;} _LL108: _temp193= _temp191; goto _LL110; _LL110:
_temp197= _temp193; goto _LL112; _LL112: return Cyc_NewControlFlow_abstract_exp(
env, _temp197); _LL114: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp369=( char*)"new_control_flow.cyc:258 env.access_path==null";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 47; _temp370;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp371; _temp371.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp373;
_temp373.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp373.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp373;}); _temp371.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp372; _temp372.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp372.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp372;}); _temp371.lvalues= 0; _temp371;}); _LL116: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp374=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 47; _temp375;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp376; _temp376.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp378;
_temp378.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp378.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp378;});
_temp376.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp377; _temp377.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp377.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp377;}); _temp376.lvalues= 0; _temp376;}); _LL118: goto _LL120; _LL120: goto
_LL122; _LL122: goto _LL124; return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL124:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL126:
_temp235= _temp229; goto _LL128; _LL128: _temp241= _temp235; goto _LL130; _LL130: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp241)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp379=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp379[ 0]=({ struct Cyc_List_List
_temp380; _temp380.hd=( void*) absop; _temp380.tl= 0; _temp380;}); _temp379;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL132: _temp259= _temp249;
_temp255= _temp245; goto _LL134; _LL134: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp381=( char*)"new_control_flow.cyc:293 env.access_path == null";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 49; _temp382;})); if( _temp259 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_enum_root( env, e,
_temp255);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp259 != 0; _temp259= _temp259->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp383=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp383[ 0]=({ struct Cyc_List_List _temp384; _temp384.hd=( void*) Cyc_CfAbsexp_mkMemberOp(
Cyc_CfAbsexp_mkMallocOp( e),({ struct Cyc_CfFlowInfo_TupleF_struct* _temp385=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp385->tag= Cyc_CfFlowInfo_TupleF_tag; _temp385->f1= i;( void*) _temp385;}));
_temp384.tl= 0; _temp384;}); _temp383;})); struct Cyc_NewControlFlow_AbsSyn syn=
Cyc_NewControlFlow_abstract_exp( elt_env,( struct Cyc_Absyn_Exp*) _temp259->hd);
void* _temp388; void* _temp390; struct Cyc_NewControlFlow_AbsSynOne _temp386=
Cyc_NewControlFlow_meet_absexp( syn); _LL391: _temp390=( void*) _temp386.inner_exp;
goto _LL389; _LL389: _temp388=( void*) _temp386.assigns; goto _LL387; _LL387:
es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp390, es_exp);
useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp388, useexp);}}
return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp, useexp);} _LL136: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp265); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE(
e), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL138:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp392=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp392->tag= Cyc_Core_Impossible_tag; _temp392->f1=( struct _tagged_string)({
char* _temp393=( char*)"NewControlFlow: Malloc_e of unsupported variant"; struct
_tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 48; _temp394;});( struct _xenum_struct*)
_temp392;})); _LL140: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp273); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp269); void* _temp397; void*
_temp399; struct Cyc_NewControlFlow_AbsSynOne _temp395= Cyc_NewControlFlow_meet_absexp(
syn1); _LL400: _temp399=( void*) _temp395.inner_exp; goto _LL398; _LL398:
_temp397=( void*) _temp395.assigns; goto _LL396; _LL396: { void* _temp403; void*
_temp405; struct Cyc_NewControlFlow_AbsSynOne _temp401= Cyc_NewControlFlow_meet_absexp(
syn2); _LL406: _temp405=( void*) _temp401.inner_exp; goto _LL404; _LL404:
_temp403=( void*) _temp401.assigns; goto _LL402; _LL402: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp399,
_temp405), _temp403); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp397);}}}
_LL142: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp407=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp407[ 0]=({ struct Cyc_List_List _temp408;
_temp408.hd=( void*) _temp279; _temp408.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp409=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp409[ 0]=({ struct Cyc_List_List _temp410; _temp410.hd=( void*) _temp275;
_temp410.tl= 0; _temp410;}); _temp409;}); _temp408;}); _temp407;}), 0); _LL144:
return Cyc_NewControlFlow_use_exp_unordered( env, _temp281, 0); _LL146: { int
both_leaf= 1;{ void* _temp411=( void*)( _temp285->topt)->v; struct Cyc_Absyn_Structdecl**
_temp419; struct Cyc_List_List* _temp421; struct _tuple1* _temp423; struct Cyc_List_List*
_temp425; _LL413: if(( unsigned int) _temp411 > 4u?(( struct _enum_struct*)
_temp411)->tag == Cyc_Absyn_StructType_tag: 0){ _LL424: _temp423=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp411)->f1; goto _LL422;
_LL422: _temp421=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp411)->f2; goto _LL420; _LL420: _temp419=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp411)->f3; goto _LL414;} else{ goto
_LL415;} _LL415: if(( unsigned int) _temp411 > 4u?(( struct _enum_struct*)
_temp411)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL426: _temp425=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp411)->f1; goto _LL416;} else{ goto
_LL417;} _LL417: goto _LL418; _LL414: goto _LL416; _LL416: both_leaf= 0; goto
_LL412; _LL418: goto _LL412; _LL412:;}{ void* _temp427=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp435; struct Cyc_List_List* _temp437; struct
_tuple1* _temp439; struct Cyc_List_List* _temp441; _LL429: if(( unsigned int)
_temp427 > 4u?(( struct _enum_struct*) _temp427)->tag == Cyc_Absyn_StructType_tag:
0){ _LL440: _temp439=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp427)->f1; goto _LL438; _LL438: _temp437=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp427)->f2; goto _LL436; _LL436: _temp435=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp427)->f3;
goto _LL430;} else{ goto _LL431;} _LL431: if(( unsigned int) _temp427 > 4u?((
struct _enum_struct*) _temp427)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL442:
_temp441=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp427)->f1;
goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL430: goto _LL432;
_LL432: both_leaf= 0; goto _LL428; _LL434: goto _LL428; _LL428:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp285);} return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp443=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp443[ 0]=({ struct Cyc_List_List
_temp444; _temp444.hd=( void*) _temp285; _temp444.tl= 0; _temp444;}); _temp443;}),
env.lhsides);} _LL148: return Cyc_NewControlFlow_use_exp_unordered( env,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp445=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp445[ 0]=({ struct Cyc_List_List
_temp446; _temp446.hd=( void*) _temp291; _temp446.tl= _temp289; _temp446;});
_temp445;}), 0); _LL150: { void* _temp447= Cyc_Tcutil_compress(( void*)(
_temp295->topt)->v); struct Cyc_List_List* _temp453; _LL449: if(( unsigned int)
_temp447 > 4u?(( struct _enum_struct*) _temp447)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL454: _temp453=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp447)->f1; goto _LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL450:
env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp455=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp455[
0]=({ struct Cyc_List_List _temp456; _temp456.hd=( void*) e; _temp456.tl= env.access_path;
_temp456;}); _temp455;}); return Cyc_NewControlFlow_abstract_exp( env, _temp295);
_LL452: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp457=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp457[ 0]=({ struct Cyc_List_List _temp458;
_temp458.hd=( void*) _temp295; _temp458.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp459=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp459[ 0]=({ struct Cyc_List_List _temp460; _temp460.hd=( void*) _temp293;
_temp460.tl= 0; _temp460;}); _temp459;}); _temp458;}); _temp457;}), Cyc_NewControlFlow_use_it_list());
_LL448:;} _LL152:{ void* _temp461=( void*) _temp297->r; struct Cyc_Absyn_Structdecl*
_temp469; struct Cyc_List_List* _temp471; struct Cyc_Core_Opt* _temp473; struct
_tuple1* _temp475; struct Cyc_List_List* _temp477; _LL463: if((( struct
_enum_struct*) _temp461)->tag == Cyc_Absyn_Struct_e_tag){ _LL476: _temp475=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp461)->f1; goto _LL474;
_LL474: _temp473=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp461)->f2; goto _LL472; _LL472: _temp471=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp461)->f3; goto _LL470; _LL470: _temp469=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp461)->f4;
goto _LL464;} else{ goto _LL465;} _LL465: if((( struct _enum_struct*) _temp461)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL478: _temp477=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp461)->f1; goto _LL466;} else{ goto _LL467;}
_LL467: goto _LL468; _LL464: goto _LL466; _LL466: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp297->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp479=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp479[ 0]=({ struct Cyc_List_List
_temp480; _temp480.hd=( void*) malloc_op; _temp480.tl= 0; _temp480;}); _temp479;}));
inner_env.access_path= 0;{ void* _temp483; void* _temp485; struct Cyc_NewControlFlow_AbsSynOne
_temp481= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp297)); _LL486: _temp485=( void*) _temp481.inner_exp; goto _LL484;
_LL484: _temp483=( void*) _temp481.assigns; goto _LL482; _LL482: { struct Cyc_NewControlFlow_AbsSyn
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
char* _temp507=( char*)"new_control_flow.cyc:411 Evexp::eval_const_uint_exp(e2) == 0";
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
struct _tagged_string)({ char* _temp578=( char*)"new_control_flow.cyc:439 Evexp::eval_const_uint_exp(e2) == 0";
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
_temp582;})); _LL519:;} return Cyc_NewControlFlow_abstract_exp( env, _temp297);
_LL154:{ struct Cyc_List_List* _temp585= env.access_path; struct Cyc_List_List
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
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp299);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL590: goto _LL586; _LL586:;} _temp303= _temp299; goto _LL156; _LL156:
_temp307= _temp303; goto _LL158; _LL158: env.access_path=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp605=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp605[ 0]=({ struct Cyc_List_List _temp606;
_temp606.hd=( void*) e; _temp606.tl= env.access_path; _temp606;}); _temp605;});{
struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env,
_temp307); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;} _LL160: { struct Cyc_List_List* _temp607= env.access_path; struct
Cyc_List_List _temp615; struct Cyc_List_List* _temp616; struct Cyc_Absyn_Exp*
_temp618; struct Cyc_Absyn_Exp _temp620; struct Cyc_Position_Segment* _temp621;
void* _temp623; struct Cyc_Absyn_Exp* _temp625; struct Cyc_Absyn_Exp* _temp627;
struct Cyc_Core_Opt* _temp629; _LL609: if( _temp607 == 0){ goto _LL610;} else{
goto _LL611;} _LL611: if( _temp607 == 0){ goto _LL613;} else{ _temp615=*
_temp607; _LL619: _temp618=( struct Cyc_Absyn_Exp*) _temp615.hd; _temp620=*
_temp618; _LL630: _temp629=( struct Cyc_Core_Opt*) _temp620.topt; goto _LL624;
_LL624: _temp623=( void*) _temp620.r; if((( struct _enum_struct*) _temp623)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL628: _temp627=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp623)->f1; goto _LL626; _LL626:
_temp625=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp623)->f2; goto _LL622;} else{ goto _LL613;} _LL622: _temp621=( struct Cyc_Position_Segment*)
_temp620.loc; goto _LL617; _LL617: _temp616=( struct Cyc_List_List*) _temp615.tl;
goto _LL612;} _LL613: goto _LL614; _LL610: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp309 != 0;
_temp309= _temp309->tl, ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp631=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp631[ 0]=({ struct Cyc_List_List _temp632; _temp632.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp632.tl= ith_lvals; _temp632;}); _temp631;});} else{ ith_lvals=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp633=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp633[ 0]=({ struct Cyc_List_List _temp634;
_temp634.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp635=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp635->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp635->f1= j;( void*) _temp635;})); _temp634.tl= ith_lvals; _temp634;});
_temp633;});}}}{ struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp309->hd);
void* _temp638; void* _temp640; struct Cyc_NewControlFlow_AbsSynOne _temp636=
Cyc_NewControlFlow_meet_absexp( ith_syn); _LL641: _temp640=( void*) _temp636.inner_exp;
goto _LL639; _LL639: _temp638=( void*) _temp636.assigns; goto _LL637; _LL637:
ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, ansexp, _temp640);
assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), assignexp,
_temp638);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);} _LL612: {
int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp625); struct Cyc_NewControlFlow_AbsEnv
other_env= Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp616;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();{
int j= 0; for( 0; _temp309 != 0; _temp309= _temp309->tl, ++ j){ if( i == j){
void* _temp644; void* _temp646; struct Cyc_NewControlFlow_AbsSynOne _temp642=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*) _temp309->hd)); _LL647: _temp646=( void*) _temp642.inner_exp;
goto _LL645; _LL645: _temp644=( void*) _temp642.assigns; goto _LL643; _LL643:
assignexp= _temp644; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp646);} else{ void* _temp650; void* _temp652; struct Cyc_NewControlFlow_AbsSynOne
_temp648= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,( struct Cyc_Absyn_Exp*) _temp309->hd)); _LL653: _temp652=( void*)
_temp648.inner_exp; goto _LL651; _LL651: _temp650=( void*) _temp648.assigns;
goto _LL649; _LL649: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp652);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp654=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp654[ 0]=({ struct Cyc_List_List _temp655;
_temp655.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp655.tl= 0; _temp655;});
_temp654;}), assignexp);}} _LL614:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp656=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp656->tag= Cyc_Core_Impossible_tag; _temp656->f1=( struct _tagged_string)({
char* _temp657=( char*)"abstract_exp: unexpected access path to Tuple"; struct
_tagged_string _temp658; _temp658.curr= _temp657; _temp658.base= _temp657;
_temp658.last_plus_one= _temp657 + 46; _temp658;});( struct _xenum_struct*)
_temp656;})); _LL608:;} _LL162: { struct Cyc_List_List* _temp659= env.access_path;
struct Cyc_List_List _temp667; struct Cyc_List_List* _temp668; struct Cyc_Absyn_Exp*
_temp670; struct Cyc_Absyn_Exp _temp672; struct Cyc_Position_Segment* _temp673;
void* _temp675; struct _tagged_string* _temp677; struct Cyc_Absyn_Exp* _temp679;
struct Cyc_Core_Opt* _temp681; _LL661: if( _temp659 == 0){ goto _LL662;} else{
goto _LL663;} _LL663: if( _temp659 == 0){ goto _LL665;} else{ _temp667=*
_temp659; _LL671: _temp670=( struct Cyc_Absyn_Exp*) _temp667.hd; _temp672=*
_temp670; _LL682: _temp681=( struct Cyc_Core_Opt*) _temp672.topt; goto _LL676;
_LL676: _temp675=( void*) _temp672.r; if((( struct _enum_struct*) _temp675)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL680: _temp679=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp675)->f1; goto _LL678; _LL678:
_temp677=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp675)->f2; goto _LL674;} else{ goto _LL665;} _LL674: _temp673=( struct Cyc_Position_Segment*)
_temp672.loc; goto _LL669; _LL669: _temp668=( struct Cyc_List_List*) _temp667.tl;
goto _LL664;} _LL665: goto _LL666; _LL662: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp313 != 0; _temp313=
_temp313->tl){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls=
env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)
ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp683=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp683[
0]=({ struct Cyc_List_List _temp684; _temp684.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp684.tl= ith_lvals; _temp684;}); _temp683;});} else{ struct Cyc_List_List*
ds=(*(( struct _tuple5*) _temp313->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void*
_temp685=( void*) ds->hd; struct Cyc_Absyn_Exp* _temp691; struct _tagged_string*
_temp693; _LL687: if((( struct _enum_struct*) _temp685)->tag == Cyc_Absyn_ArrayElement_tag){
_LL692: _temp691=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp685)->f1; goto _LL688;} else{ goto _LL689;} _LL689: if((( struct
_enum_struct*) _temp685)->tag == Cyc_Absyn_FieldName_tag){ _LL694: _temp693=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp685)->f1;
goto _LL690;} else{ goto _LL686;} _LL688:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp695=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp695->tag= Cyc_Core_Impossible_tag; _temp695->f1=( struct _tagged_string)({
char* _temp696=( char*)"bad struct designator"; struct _tagged_string _temp697;
_temp697.curr= _temp696; _temp697.base= _temp696; _temp697.last_plus_one=
_temp696 + 22; _temp697;});( struct _xenum_struct*) _temp695;})); _LL690:
ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp698=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp698[ 0]=({ struct Cyc_List_List
_temp699; _temp699.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct
Cyc_CfFlowInfo_StructF_struct* _temp700=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp700->tag= Cyc_CfFlowInfo_StructF_tag;
_temp700->f1= _temp693;( void*) _temp700;})); _temp699.tl= ith_lvals; _temp699;});
_temp698;}); goto _LL686; _LL686:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp313->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp313->hd)).f2); void* _temp703; void* _temp705; struct Cyc_NewControlFlow_AbsSynOne
_temp701= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL706: _temp705=( void*)
_temp701.inner_exp; goto _LL704; _LL704: _temp703=( void*) _temp701.assigns;
goto _LL702; _LL702: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp705); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp703);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL664: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp668;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp313 != 0; _temp313=
_temp313->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp313->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp707=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp713; struct _tagged_string* _temp715; _LL709: if(((
struct _enum_struct*) _temp707)->tag == Cyc_Absyn_ArrayElement_tag){ _LL714:
_temp713=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp707)->f1; goto _LL710;} else{ goto _LL711;} _LL711: if((( struct
_enum_struct*) _temp707)->tag == Cyc_Absyn_FieldName_tag){ _LL716: _temp715=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp707)->f1;
goto _LL712;} else{ goto _LL708;} _LL710:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp717=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp717->tag= Cyc_Core_Impossible_tag; _temp717->f1=( struct _tagged_string)({
char* _temp718=( char*)"bad struct designator"; struct _tagged_string _temp719;
_temp719.curr= _temp718; _temp719.base= _temp718; _temp719.last_plus_one=
_temp718 + 22; _temp719;});( struct _xenum_struct*) _temp717;})); _LL712: if(
Cyc_String_zstrptrcmp( _temp677, _temp715) == 0){ used= 1;} goto _LL708; _LL708:;}}
if( used){ void* _temp722; void* _temp724; struct Cyc_NewControlFlow_AbsSynOne
_temp720= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*) _temp313->hd)).f2)); _LL725: _temp724=( void*) _temp720.inner_exp;
goto _LL723; _LL723: _temp722=( void*) _temp720.assigns; goto _LL721; _LL721:
assignexp= _temp722; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp724);} else{ void* _temp728; void* _temp730; struct Cyc_NewControlFlow_AbsSynOne
_temp726= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp313->hd)).f2)); _LL731: _temp730=( void*)
_temp726.inner_exp; goto _LL729; _LL729: _temp728=( void*) _temp726.assigns;
goto _LL727; _LL727: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp730);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL666:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp732=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp732->tag= Cyc_Core_Impossible_tag; _temp732->f1=( struct _tagged_string)({
char* _temp733=( char*)"abstract_exp: unexpected access path to Struct"; struct
_tagged_string _temp734; _temp734.curr= _temp733; _temp734.base= _temp733;
_temp734.last_plus_one= _temp733 + 47; _temp734;});( struct _xenum_struct*)
_temp732;})); _LL660:;} _LL164: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp319), 0); _LL166: return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp735=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp735[ 0]=({ struct Cyc_List_List
_temp736; _temp736.hd=( void*) _temp325; _temp736.tl= 0; _temp736;}); _temp735;}),
0); _LL168: { struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp327); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
esyn)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL170: { struct Cyc_List_List* _temp739; struct Cyc_NewControlFlow_AbsSynOne
_temp741; struct Cyc_NewControlFlow_AbsSynOne _temp743; struct Cyc_NewControlFlow_AbsSyn
_temp737= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp333); _LL744: _temp743=( struct Cyc_NewControlFlow_AbsSynOne) _temp737.when_true;
goto _LL742; _LL742: _temp741=( struct Cyc_NewControlFlow_AbsSynOne) _temp737.when_false;
goto _LL740; _LL740: _temp739=( struct Cyc_List_List*) _temp737.lvalues; goto
_LL738; _LL738: { struct Cyc_List_List* _temp747; struct Cyc_NewControlFlow_AbsSynOne
_temp749; struct Cyc_NewControlFlow_AbsSynOne _temp751; struct Cyc_NewControlFlow_AbsSyn
_temp745= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp331); _LL752: _temp751=( struct Cyc_NewControlFlow_AbsSynOne) _temp745.when_true;
goto _LL750; _LL750: _temp749=( struct Cyc_NewControlFlow_AbsSynOne) _temp745.when_false;
goto _LL748; _LL748: _temp747=( struct Cyc_List_List*) _temp745.lvalues; goto
_LL746; _LL746: { struct Cyc_List_List* _temp755; struct Cyc_NewControlFlow_AbsSynOne
_temp757; struct Cyc_NewControlFlow_AbsSynOne _temp759; struct Cyc_NewControlFlow_AbsSyn
_temp753= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp329); _LL760: _temp759=( struct Cyc_NewControlFlow_AbsSynOne) _temp753.when_true;
goto _LL758; _LL758: _temp757=( struct Cyc_NewControlFlow_AbsSynOne) _temp753.when_false;
goto _LL756; _LL756: _temp755=( struct Cyc_List_List*) _temp753.lvalues; goto
_LL754; _LL754: { void* true_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp743.inner_exp,( void*) _temp743.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp751.inner_exp,( void*) _temp751.assigns)),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp741.inner_exp,( void*) _temp741.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp759.inner_exp,( void*) _temp759.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp743.inner_exp,( void*) _temp743.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp749.inner_exp,( void*) _temp749.assigns)), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp741.inner_exp,( void*) _temp741.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp757.inner_exp,( void*) _temp757.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp761; _temp761.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp763;
_temp763.inner_exp=( void*) true_exp; _temp763.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp763;}); _temp761.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp762; _temp762.inner_exp=( void*) false_exp; _temp762.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp762;}); _temp761.lvalues= 0; _temp761;});}}}}
_LL172: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp335);{
struct Cyc_Absyn_Stmt* last_s= _temp335; while( 1) { void* _temp764=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp774; struct Cyc_Absyn_Stmt* _temp776;
struct Cyc_Absyn_Stmt* _temp778; struct Cyc_Absyn_Decl* _temp780; struct Cyc_Absyn_Exp*
_temp782; _LL766: if(( unsigned int) _temp764 > 1u?(( struct _enum_struct*)
_temp764)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL777: _temp776=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp764)->f1; goto _LL775; _LL775: _temp774=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp764)->f2; goto
_LL767;} else{ goto _LL768;} _LL768: if(( unsigned int) _temp764 > 1u?(( struct
_enum_struct*) _temp764)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL781: _temp780=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp764)->f1; goto
_LL779; _LL779: _temp778=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp764)->f2; goto _LL769;} else{ goto _LL770;} _LL770: if(( unsigned int)
_temp764 > 1u?(( struct _enum_struct*) _temp764)->tag == Cyc_Absyn_Exp_s_tag: 0){
_LL783: _temp782=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp764)->f1; goto _LL771;} else{ goto _LL772;} _LL772: goto _LL773; _LL767:
last_s= _temp774; continue; _LL769: last_s= _temp778; continue; _LL771: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp782); void* _temp786; void* _temp788; struct Cyc_NewControlFlow_AbsSynOne
_temp784= Cyc_NewControlFlow_meet_absexp( syn); _LL789: _temp788=( void*)
_temp784.inner_exp; goto _LL787; _LL787: _temp786=( void*) _temp784.assigns;
goto _LL785; _LL785:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp788, _temp786));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp335), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL773:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp790=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp790->tag= Cyc_Core_Impossible_tag; _temp790->f1=( struct _tagged_string)({
char* _temp791=( char*)"abstract_exp: ill-formed StmtExp"; struct _tagged_string
_temp792; _temp792.curr= _temp791; _temp792.base= _temp791; _temp792.last_plus_one=
_temp791 + 33; _temp792;});( struct _xenum_struct*) _temp790;})); _LL765:;}}}
_LL174: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp339); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp337); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp793; _temp793.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp795; _temp795.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp795.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp795;});
_temp793.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp794; _temp794.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp794.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp794;}); _temp793.lvalues= 0; _temp793;});}} _LL176: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp796=( char*)"new_control_flow.cyc:687 env.access_path==null";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 47; _temp797;})); Cyc_NewControlFlow_add_var_root(
env, _temp345);( env.shared)->comprehension_vars=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp798=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp798[ 0]=({ struct Cyc_List_List _temp799;
_temp799.hd=( void*) _temp345; _temp799.tl=( env.shared)->comprehension_vars;
_temp799;}); _temp798;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp802; void* _temp804; struct Cyc_NewControlFlow_AbsSynOne
_temp800= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp343)); _LL805: _temp804=( void*) _temp800.inner_exp; goto _LL803;
_LL803: _temp802=( void*) _temp800.assigns; goto _LL801; _LL801: { void*
_temp808; void* _temp810; struct Cyc_NewControlFlow_AbsSynOne _temp806= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp341)); _LL811: _temp810=( void*)
_temp806.inner_exp; goto _LL809; _LL809: _temp808=( void*) _temp806.assigns;
goto _LL807; _LL807: { void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp804, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp802, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp345),
Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, _temp810, _temp808), Cyc_CfAbsexp_mkSkipAE())))); return
Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()));}}} _LL178: goto _LL180; _LL180: goto _LL182;
_LL182: goto _LL184; _LL184: goto _LL186; _LL186: goto _LL188; _LL188: goto
_LL190; _LL190:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp812=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp812->tag= Cyc_Core_Impossible_tag; _temp812->f1=( struct _tagged_string)({
char* _temp813=( char*)"abstract_exp, unexpected exp form"; struct
_tagged_string _temp814; _temp814.curr= _temp813; _temp814.base= _temp813;
_temp814.last_plus_one= _temp813 + 34; _temp814;});( struct _xenum_struct*)
_temp812;})); _LL106:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp817; void* _temp819; struct Cyc_NewControlFlow_AbsSynOne
_temp815= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL820: _temp819=( void*) _temp815.inner_exp; goto _LL818; _LL818: _temp817=(
void*) _temp815.assigns; goto _LL816; _LL816: return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp819, _temp817);} struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp821; _temp821.f1= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns); _temp821.f2=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp821;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp824;
struct Cyc_Position_Segment* _temp825; struct Cyc_Absyn_Stmt* _temp827; struct
Cyc_Absyn_Exp* _temp829; struct Cyc_Core_Opt* _temp831; struct Cyc_Absyn_Pat*
_temp833; struct Cyc_Absyn_Switch_clause* _temp822=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp824=* _temp822; _LL834: _temp833=( struct Cyc_Absyn_Pat*) _temp824.pattern;
goto _LL832; _LL832: _temp831=( struct Cyc_Core_Opt*) _temp824.pat_vars; goto
_LL830; _LL830: _temp829=( struct Cyc_Absyn_Exp*) _temp824.where_clause; goto
_LL828; _LL828: _temp827=( struct Cyc_Absyn_Stmt*) _temp824.body; goto _LL826;
_LL826: _temp825=( struct Cyc_Position_Segment*) _temp824.loc; goto _LL823;
_LL823: if( _temp831 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp835=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp835->tag= Cyc_Core_Impossible_tag; _temp835->f1=( struct _tagged_string)({
char* _temp836=( char*)"switch clause vds not set"; struct _tagged_string
_temp837; _temp837.curr= _temp836; _temp837.base= _temp836; _temp837.last_plus_one=
_temp836 + 26; _temp837;});( struct _xenum_struct*) _temp835;}));}{ struct Cyc_List_List*
vds=( struct Cyc_List_List*) _temp831->v; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp829 != 0){ void* _temp841;
void* _temp843; struct _tuple0 _temp839= Cyc_NewControlFlow_abstract_guard( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp838= _temp829; if( _temp838
== 0){ _throw( Null_Exception);} _temp838;})); _LL844: _temp843= _temp839.f1;
goto _LL842; _LL842: _temp841= _temp839.f2; goto _LL840; _LL840: ans=( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp845[ 0]=({ struct Cyc_List_List
_temp846; _temp846.hd=( void*) _temp843; _temp846.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp847=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp847[ 0]=({ struct Cyc_List_List _temp848;
_temp848.hd=( void*) _temp841; _temp848.tl= ans; _temp848;}); _temp847;});
_temp846;}); _temp845;});} Cyc_NewControlFlow_cf_prepass( env, _temp827);}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp849=( char*)"new_control_flow.cyc:739 env.lhsides == null";
struct _tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 45; _temp850;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp851=( char*)"new_control_flow.cyc:740 env.access_path == null";
struct _tagged_string _temp852; _temp852.curr= _temp851; _temp852.base= _temp851;
_temp852.last_plus_one= _temp851 + 49; _temp852;}));{ void* _temp853=( void*) s->r;
struct Cyc_Absyn_Exp* _temp899; struct Cyc_Absyn_Stmt* _temp901; struct Cyc_Absyn_Stmt*
_temp903; struct Cyc_Absyn_Exp* _temp905; struct Cyc_Absyn_Exp* _temp907; struct
Cyc_Absyn_Stmt* _temp909; struct Cyc_Absyn_Stmt* _temp911; struct Cyc_Absyn_Exp*
_temp913; struct _tuple3 _temp915; struct Cyc_Absyn_Stmt* _temp917; struct Cyc_Absyn_Exp*
_temp919; struct Cyc_Absyn_Stmt* _temp921; struct Cyc_Absyn_Stmt* _temp923;
struct _tuple3 _temp925; struct Cyc_Absyn_Stmt* _temp927; struct Cyc_Absyn_Exp*
_temp929; struct Cyc_Absyn_Stmt* _temp931; struct Cyc_Absyn_Stmt* _temp933;
struct Cyc_Absyn_Stmt* _temp935; struct _tagged_string* _temp937; struct Cyc_Absyn_Stmt*
_temp939; struct _tagged_string* _temp941; struct Cyc_Absyn_Stmt* _temp943;
struct _tuple3 _temp945; struct Cyc_Absyn_Stmt* _temp947; struct Cyc_Absyn_Exp*
_temp949; struct _tuple3 _temp951; struct Cyc_Absyn_Stmt* _temp953; struct Cyc_Absyn_Exp*
_temp955; struct Cyc_Absyn_Exp* _temp957; struct Cyc_List_List* _temp959; struct
Cyc_Absyn_Exp* _temp961; struct Cyc_Absyn_Switch_clause** _temp963; struct Cyc_List_List*
_temp965; struct Cyc_Absyn_Stmt* _temp967; struct Cyc_Absyn_Decl* _temp969;
struct Cyc_Absyn_Decl _temp971; struct Cyc_Position_Segment* _temp972; void*
_temp974; struct Cyc_Absyn_Vardecl* _temp976; struct Cyc_Absyn_Stmt* _temp978;
struct Cyc_Absyn_Decl* _temp980; struct Cyc_Absyn_Decl _temp982; struct Cyc_Position_Segment*
_temp983; void* _temp985; int _temp987; struct Cyc_Absyn_Exp* _temp989; struct
Cyc_Core_Opt* _temp991; struct Cyc_Core_Opt* _temp993; struct Cyc_Core_Opt
_temp995; struct Cyc_List_List* _temp996; struct Cyc_Absyn_Pat* _temp998; struct
Cyc_Absyn_Stmt* _temp1000; struct Cyc_Absyn_Decl* _temp1002; struct Cyc_Absyn_Stmt*
_temp1004; struct Cyc_Absyn_Stmt* _temp1006; struct Cyc_Absyn_Stmt* _temp1008;
struct _tagged_string* _temp1010; struct Cyc_List_List* _temp1012; struct Cyc_Absyn_Stmt*
_temp1014; _LL855: if( _temp853 == Cyc_Absyn_Skip_s){ goto _LL856;} else{ goto
_LL857;} _LL857: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*)
_temp853)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL900: _temp899=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp853)->f1; goto _LL858;} else{ goto _LL859;}
_LL859: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL904: _temp903=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp853)->f1; goto _LL902; _LL902: _temp901=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp853)->f2; goto _LL860;}
else{ goto _LL861;} _LL861: if(( unsigned int) _temp853 > 1u?(( struct
_enum_struct*) _temp853)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL906: _temp905=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp853)->f1; if(
_temp905 == 0){ goto _LL862;} else{ goto _LL863;}} else{ goto _LL863;} _LL863:
if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL908: _temp907=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp853)->f1; goto _LL864;} else{ goto _LL865;} _LL865: if(( unsigned int)
_temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL914: _temp913=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp853)->f1; goto _LL912; _LL912: _temp911=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp853)->f2; goto _LL910; _LL910: _temp909=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp853)->f3;
goto _LL866;} else{ goto _LL867;} _LL867: if(( unsigned int) _temp853 > 1u?((
struct _enum_struct*) _temp853)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL922: _temp921=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp853)->f1; goto
_LL916; _LL916: _temp915=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp853)->f2; _LL920: _temp919= _temp915.f1; goto _LL918; _LL918: _temp917=
_temp915.f2; goto _LL868;} else{ goto _LL869;} _LL869: if(( unsigned int)
_temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_While_s_tag:
0){ _LL926: _temp925=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp853)->f1; _LL930: _temp929= _temp925.f1; goto _LL928; _LL928: _temp927=
_temp925.f2; goto _LL924; _LL924: _temp923=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp853)->f2; goto _LL870;} else{ goto _LL871;}
_LL871: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL932: _temp931=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp853)->f1; goto _LL872;} else{ goto _LL873;}
_LL873: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Continue_s_tag: 0){ _LL934: _temp933=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp853)->f1; goto _LL874;} else{ goto
_LL875;} _LL875: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*)
_temp853)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL938: _temp937=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp853)->f1; goto _LL936;
_LL936: _temp935=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp853)->f2; if( _temp935 == 0){ goto _LL876;} else{ goto _LL877;}} else{ goto
_LL877;} _LL877: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*)
_temp853)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL942: _temp941=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp853)->f1; goto _LL940;
_LL940: _temp939=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp853)->f2; goto _LL878;} else{ goto _LL879;} _LL879: if(( unsigned int)
_temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_For_s_tag: 0){
_LL958: _temp957=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp853)->f1; goto _LL952; _LL952: _temp951=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp853)->f2; _LL956: _temp955= _temp951.f1; goto _LL954; _LL954: _temp953=
_temp951.f2; goto _LL946; _LL946: _temp945=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp853)->f3; _LL950: _temp949= _temp945.f1; goto _LL948; _LL948: _temp947=
_temp945.f2; goto _LL944; _LL944: _temp943=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp853)->f4; goto _LL880;} else{ goto _LL881;} _LL881:
if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL962: _temp961=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp853)->f1; goto _LL960; _LL960: _temp959=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp853)->f2; goto _LL882;} else{ goto _LL883;}
_LL883: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL966: _temp965=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp853)->f1; goto _LL964; _LL964:
_temp963=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp853)->f2; goto _LL884;} else{ goto _LL885;} _LL885: if(( unsigned int)
_temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL970: _temp969=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp853)->f1; _temp971=* _temp969; _LL975: _temp974=( void*) _temp971.r; if(((
struct _enum_struct*) _temp974)->tag == Cyc_Absyn_Var_d_tag){ _LL977: _temp976=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp974)->f1; goto
_LL973;} else{ goto _LL887;} _LL973: _temp972=( struct Cyc_Position_Segment*)
_temp971.loc; goto _LL968; _LL968: _temp967=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp853)->f2; goto _LL886;} else{ goto _LL887;}
_LL887: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL981: _temp980=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp853)->f1; _temp982=* _temp980; _LL986: _temp985=(
void*) _temp982.r; if((( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Let_d_tag){
_LL999: _temp998=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp985)->f1; goto _LL994; _LL994: _temp993=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp985)->f2; if( _temp993 == 0){ goto _LL889;} else{
_temp995=* _temp993; _LL997: _temp996=( struct Cyc_List_List*) _temp995.v; goto
_LL992;} _LL992: _temp991=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp985)->f3; goto _LL990; _LL990: _temp989=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp985)->f4; goto _LL988; _LL988: _temp987=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp985)->f5; goto _LL984;} else{ goto _LL889;}
_LL984: _temp983=( struct Cyc_Position_Segment*) _temp982.loc; goto _LL979;
_LL979: _temp978=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp853)->f2; goto _LL888;} else{ goto _LL889;} _LL889: if(( unsigned int)
_temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL1003: _temp1002=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp853)->f1; goto _LL1001; _LL1001: _temp1000=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp853)->f2; goto _LL890;} else{ goto _LL891;}
_LL891: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Cut_s_tag: 0){ _LL1005: _temp1004=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp853)->f1; goto _LL892;} else{ goto _LL893;}
_LL893: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*) _temp853)->tag
== Cyc_Absyn_Splice_s_tag: 0){ _LL1007: _temp1006=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp853)->f1; goto _LL894;} else{ goto
_LL895;} _LL895: if(( unsigned int) _temp853 > 1u?(( struct _enum_struct*)
_temp853)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1011: _temp1010=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp853)->f1; goto _LL1009;
_LL1009: _temp1008=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp853)->f2; goto _LL896;} else{ goto _LL897;} _LL897: if(( unsigned int)
_temp853 > 1u?(( struct _enum_struct*) _temp853)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1015: _temp1014=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp853)->f1; goto _LL1013; _LL1013: _temp1012=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp853)->f2; goto _LL898;} else{ goto
_LL854;} _LL856: return; _LL858: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
env, _temp899); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1016=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1016[ 0]=({ struct Cyc_List_List _temp1017;
_temp1017.hd=( void*) ae; _temp1017.tl= 0; _temp1017;}); _temp1016;})); return;}
_LL860: Cyc_NewControlFlow_cf_prepass( env, _temp903); Cyc_NewControlFlow_cf_prepass(
env, _temp901); return; _LL862: return; _LL864: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1018= _temp907; if( _temp1018 == 0){ _throw( Null_Exception);} _temp1018;}));
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1019=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1019[ 0]=({ struct Cyc_List_List _temp1020; _temp1020.hd=( void*) ae;
_temp1020.tl= 0; _temp1020;}); _temp1019;})); return;} _LL866: { void* _temp1023;
void* _temp1025; struct _tuple0 _temp1021= Cyc_NewControlFlow_abstract_guard(
env, _temp913); _LL1026: _temp1025= _temp1021.f1; goto _LL1024; _LL1024:
_temp1023= _temp1021.f2; goto _LL1022; _LL1022: Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1027=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1027[ 0]=({ struct Cyc_List_List
_temp1028; _temp1028.hd=( void*) _temp1025; _temp1028.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1029=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1029[ 0]=({ struct Cyc_List_List _temp1030;
_temp1030.hd=( void*) _temp1023; _temp1030.tl= 0; _temp1030;}); _temp1029;});
_temp1028;}); _temp1027;})); Cyc_NewControlFlow_cf_prepass( env, _temp911); Cyc_NewControlFlow_cf_prepass(
env, _temp909); return;} _LL868: _temp929= _temp919; _temp927= _temp917;
_temp923= _temp921; goto _LL870; _LL870: { void* _temp1033; void* _temp1035;
struct _tuple0 _temp1031= Cyc_NewControlFlow_abstract_guard( env, _temp929);
_LL1036: _temp1035= _temp1031.f1; goto _LL1034; _LL1034: _temp1033= _temp1031.f2;
goto _LL1032; _LL1032: Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1037=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1037[ 0]=({ struct Cyc_List_List _temp1038;
_temp1038.hd=( void*) _temp1035; _temp1038.tl=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1039=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1039[ 0]=({ struct Cyc_List_List _temp1040; _temp1040.hd=( void*)
_temp1033; _temp1040.tl= 0; _temp1040;}); _temp1039;}); _temp1038;}); _temp1037;}));
Cyc_NewControlFlow_cf_prepass( env, _temp923); return;} _LL872: return; _LL874:
return; _LL876:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1041=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1041->tag= Cyc_Core_Impossible_tag; _temp1041->f1=( struct _tagged_string)({
char* _temp1042=( char*)"cf_prepass: goto w/o destination"; struct
_tagged_string _temp1043; _temp1043.curr= _temp1042; _temp1043.base= _temp1042;
_temp1043.last_plus_one= _temp1042 + 33; _temp1043;});( struct _xenum_struct*)
_temp1041;})); _LL878: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1044= _temp939; if(
_temp1044 == 0){ _throw( Null_Exception);} _temp1044;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1045=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base=
_temp1045; _temp1046.last_plus_one= _temp1045 + 45; _temp1046;})); break;}
my_encloser= next_encloser;} return;} _LL880: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp957); void* _temp1049; void* _temp1051; struct _tuple0 _temp1047= Cyc_NewControlFlow_abstract_guard(
env, _temp955); _LL1052: _temp1051= _temp1047.f1; goto _LL1050; _LL1050:
_temp1049= _temp1047.f2; goto _LL1048; _LL1048: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp949); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1053=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1053[ 0]=({ struct Cyc_List_List _temp1054;
_temp1054.hd=( void*) ae1; _temp1054.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1055=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1055[ 0]=({ struct Cyc_List_List _temp1056; _temp1056.hd=( void*) _temp1051;
_temp1056.tl=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1057=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1057[ 0]=({
struct Cyc_List_List _temp1058; _temp1058.hd=( void*) _temp1049; _temp1058.tl=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1059=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1059[ 0]=({ struct Cyc_List_List
_temp1060; _temp1060.hd=( void*) ae3; _temp1060.tl= 0; _temp1060;}); _temp1059;});
_temp1058;}); _temp1057;}); _temp1056;}); _temp1055;}); _temp1054;}); _temp1053;}));
Cyc_NewControlFlow_cf_prepass( env, _temp943); return;}} _LL882: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp961); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp959); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1061=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1061[ 0]=({ struct Cyc_List_List _temp1062;
_temp1062.hd=( void*) ae; _temp1062.tl= ael; _temp1062;}); _temp1061;}));
return;} _LL884: { void* _temp1065; void* _temp1067; struct Cyc_NewControlFlow_AbsSynOne
_temp1063= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp965, 0)); _LL1068: _temp1067=( void*) _temp1063.inner_exp; goto
_LL1066; _LL1066: _temp1065=( void*) _temp1063.assigns; goto _LL1064; _LL1064: {
void* ae= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp1067, _temp1065);
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1069=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1069[ 0]=({ struct Cyc_List_List _temp1070; _temp1070.hd=( void*) ae;
_temp1070.tl= 0; _temp1070;}); _temp1069;})); return;}} _LL886: Cyc_NewControlFlow_add_var_root(
env, _temp976); if( _temp976->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1071=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1071[ 0]=({ struct Cyc_List_List _temp1072; _temp1072.hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp976); _temp1072.tl= 0; _temp1072;}); _temp1071;})); void* ae= Cyc_NewControlFlow_abstract_exp_top(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1073= _temp976->initializer;
if( _temp1073 == 0){ _throw( Null_Exception);} _temp1073;})); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1074=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1074[ 0]=({ struct Cyc_List_List
_temp1075; _temp1075.hd=( void*) ae; _temp1075.tl= 0; _temp1075;}); _temp1074;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp967); return; _LL888: for( 0; _temp996
!= 0; _temp996= _temp996->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)
_temp996->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp989); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1076=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1076[ 0]=({ struct Cyc_List_List _temp1077;
_temp1077.hd=( void*) ae; _temp1077.tl= 0; _temp1077;}); _temp1076;})); Cyc_NewControlFlow_cf_prepass(
env, _temp978); return;} _LL890: goto _LL892; _LL892: goto _LL894; _LL894:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp1078=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1078->tag= Cyc_Core_Impossible_tag;
_temp1078->f1=( struct _tagged_string)({ char* _temp1079=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1080; _temp1080.curr= _temp1079; _temp1080.base=
_temp1079; _temp1080.last_plus_one= _temp1079 + 26; _temp1080;});( struct
_xenum_struct*) _temp1078;})); _LL896: Cyc_NewControlFlow_cf_prepass( env,
_temp1008); return; _LL898: Cyc_NewControlFlow_cf_prepass( env, _temp1014);{
struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1012);
Cyc_NewControlFlow_cf_set_absexps( s, ael); return;} _LL854:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1081=( char*)"new_control_flow.cyc:868 annot->visited == iteration_num";
struct _tagged_string _temp1082; _temp1082.curr= _temp1081; _temp1082.base=
_temp1081; _temp1082.last_plus_one= _temp1081 + 57; _temp1082;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1083= flow; struct Cyc_Dict_Dict* _temp1089; _LL1085: if( _temp1083
== Cyc_CfFlowInfo_BottomFL){ goto _LL1086;} else{ goto _LL1087;} _LL1087: if((
unsigned int) _temp1083 > 1u?(( struct _enum_struct*) _temp1083)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1090: _temp1089=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1083)->f1; goto _LL1088;} else{ goto _LL1084;} _LL1086: return Cyc_CfFlowInfo_BottomFL;
_LL1088: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1091=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1091->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1091->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1091;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots, root)); _temp1089=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1089, root, pinfo);} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1092=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1092->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1092->f1= _temp1089;( void*) _temp1092;}); _LL1084:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1093= flow; struct Cyc_Dict_Dict* _temp1099; _LL1095: if( _temp1093
== Cyc_CfFlowInfo_BottomFL){ goto _LL1096;} else{ goto _LL1097;} _LL1097: if((
unsigned int) _temp1093 > 1u?(( struct _enum_struct*) _temp1093)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1100: _temp1099=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1093)->f1; goto _LL1098;} else{ goto _LL1094;} _LL1096: return Cyc_CfFlowInfo_BottomFL;
_LL1098: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1101=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1101->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1101->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1101;}); _temp1099=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1099, root,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots, root));} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1102=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1102->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1102->f1= _temp1099;( void*) _temp1102;}); _LL1094:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)
aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1103; _temp1103.f1=
out_flow; _temp1103.f2= out_flow; _temp1103;});} return({ struct _tuple0
_temp1104; _temp1104.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1104.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1104;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1107; struct Cyc_Position_Segment* _temp1108; struct Cyc_Absyn_Stmt*
_temp1110; struct Cyc_Absyn_Exp* _temp1112; struct Cyc_Core_Opt* _temp1114;
struct Cyc_Absyn_Pat* _temp1116; struct Cyc_Absyn_Switch_clause* _temp1105=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1107=* _temp1105; _LL1117:
_temp1116=( struct Cyc_Absyn_Pat*) _temp1107.pattern; goto _LL1115; _LL1115:
_temp1114=( struct Cyc_Core_Opt*) _temp1107.pat_vars; goto _LL1113; _LL1113:
_temp1112=( struct Cyc_Absyn_Exp*) _temp1107.where_clause; goto _LL1111; _LL1111:
_temp1110=( struct Cyc_Absyn_Stmt*) _temp1107.body; goto _LL1109; _LL1109:
_temp1108=( struct Cyc_Position_Segment*) _temp1107.loc; goto _LL1106; _LL1106: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1114->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1114->v);{ void* body_outflow;
if( _temp1112 != 0){ void* _temp1121; void* _temp1123; struct _tuple0 _temp1119=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1118= _temp1112; if( _temp1118 == 0){ _throw( Null_Exception);} _temp1118;}))->loc,
aes, body_inflow); _LL1124: _temp1123= _temp1119.f1; goto _LL1122; _LL1122:
_temp1121= _temp1119.f2; goto _LL1120; _LL1120: aes=( aes->tl)->tl; in_flow=
_temp1121; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1110,
_temp1123);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1110, body_inflow);}{ void* _temp1125= body_outflow; _LL1127: if( _temp1125
== Cyc_CfFlowInfo_BottomFL){ goto _LL1128;} else{ goto _LL1129;} _LL1129: goto
_LL1130; _LL1128: goto _LL1126; _LL1130: Cyc_Tcutil_terr( _temp1110->loc,(
struct _tagged_string)({ char* _temp1131=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1132; _temp1132.curr= _temp1131; _temp1132.base=
_temp1131; _temp1132.last_plus_one= _temp1131 + 38; _temp1132;})); goto _LL1126;
_LL1126:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1133=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1175; struct Cyc_Absyn_Exp* _temp1177; struct Cyc_Absyn_Exp*
_temp1179; struct Cyc_Absyn_Stmt* _temp1181; struct Cyc_Absyn_Stmt* _temp1183;
struct Cyc_Absyn_Stmt* _temp1185; struct Cyc_Absyn_Stmt* _temp1187; struct Cyc_Absyn_Exp*
_temp1189; struct Cyc_Absyn_Stmt* _temp1191; struct _tuple3 _temp1193; struct
Cyc_Absyn_Stmt* _temp1195; struct Cyc_Absyn_Exp* _temp1197; struct _tuple3
_temp1199; struct Cyc_Absyn_Stmt* _temp1201; struct Cyc_Absyn_Exp* _temp1203;
struct Cyc_Absyn_Stmt* _temp1205; struct Cyc_Absyn_Stmt* _temp1207; struct
_tuple3 _temp1209; struct Cyc_Absyn_Stmt* _temp1211; struct Cyc_Absyn_Exp*
_temp1213; struct _tuple3 _temp1215; struct Cyc_Absyn_Stmt* _temp1217; struct
Cyc_Absyn_Exp* _temp1219; struct Cyc_Absyn_Exp* _temp1221; struct Cyc_Absyn_Stmt*
_temp1223; struct Cyc_Absyn_Switch_clause** _temp1225; struct Cyc_Absyn_Switch_clause*
_temp1227; struct Cyc_List_List* _temp1228; struct Cyc_Absyn_Stmt* _temp1230;
struct Cyc_Absyn_Stmt* _temp1232; struct Cyc_Absyn_Stmt* _temp1234; struct
_tagged_string* _temp1236; struct Cyc_List_List* _temp1238; struct Cyc_Absyn_Exp*
_temp1240; struct Cyc_List_List* _temp1242; struct Cyc_Absyn_Stmt* _temp1244;
struct Cyc_Absyn_Stmt* _temp1246; struct Cyc_Absyn_Decl* _temp1248; struct Cyc_Absyn_Decl
_temp1250; struct Cyc_Position_Segment* _temp1251; void* _temp1253; struct Cyc_Absyn_Vardecl*
_temp1255; struct Cyc_Absyn_Stmt* _temp1257; struct Cyc_Absyn_Decl* _temp1259;
struct Cyc_Absyn_Decl _temp1261; struct Cyc_Position_Segment* _temp1262; void*
_temp1264; int _temp1266; struct Cyc_Absyn_Exp* _temp1268; struct Cyc_Core_Opt*
_temp1270; struct Cyc_Core_Opt* _temp1272; struct Cyc_Core_Opt _temp1274; struct
Cyc_List_List* _temp1275; struct Cyc_Absyn_Pat* _temp1277; struct Cyc_Absyn_Stmt*
_temp1279; struct _tagged_string* _temp1281; _LL1135: if( _temp1133 == Cyc_Absyn_Skip_s){
goto _LL1136;} else{ goto _LL1137;} _LL1137: if(( unsigned int) _temp1133 > 1u?((
struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1176:
_temp1175=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1133)->f1; if( _temp1175 == 0){ goto _LL1138;} else{ goto _LL1139;}} else{
goto _LL1139;} _LL1139: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1178: _temp1177=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1133)->f1; goto _LL1140;} else{ goto
_LL1141;} _LL1141: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1180: _temp1179=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp1133)->f1; goto _LL1142;} else{ goto
_LL1143;} _LL1143: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1184: _temp1183=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1133)->f1; goto _LL1182; _LL1182: _temp1181=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1133)->f2; goto
_LL1144;} else{ goto _LL1145;} _LL1145: if(( unsigned int) _temp1133 > 1u?((
struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1190:
_temp1189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1133)->f1; goto _LL1188; _LL1188: _temp1187=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1133)->f2; goto _LL1186; _LL1186:
_temp1185=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1133)->f3; goto _LL1146;} else{ goto _LL1147;} _LL1147: if(( unsigned int)
_temp1133 > 1u?(( struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1194: _temp1193=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1133)->f1; _LL1198: _temp1197= _temp1193.f1; goto _LL1196; _LL1196:
_temp1195= _temp1193.f2; goto _LL1192; _LL1192: _temp1191=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1133)->f2; goto _LL1148;} else{ goto
_LL1149;} _LL1149: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1206: _temp1205=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1133)->f1; goto _LL1200; _LL1200: _temp1199=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1133)->f2; _LL1204:
_temp1203= _temp1199.f1; goto _LL1202; _LL1202: _temp1201= _temp1199.f2; goto
_LL1150;} else{ goto _LL1151;} _LL1151: if(( unsigned int) _temp1133 > 1u?((
struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1222:
_temp1221=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1133)->f1;
goto _LL1216; _LL1216: _temp1215=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1133)->f2; _LL1220: _temp1219= _temp1215.f1; goto _LL1218; _LL1218:
_temp1217= _temp1215.f2; goto _LL1210; _LL1210: _temp1209=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1133)->f3; _LL1214: _temp1213= _temp1209.f1;
goto _LL1212; _LL1212: _temp1211= _temp1209.f2; goto _LL1208; _LL1208: _temp1207=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1133)->f4; goto
_LL1152;} else{ goto _LL1153;} _LL1153: if(( unsigned int) _temp1133 > 1u?((
struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1224:
_temp1223=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1133)->f1; if( _temp1223 == 0){ goto _LL1154;} else{ goto _LL1155;}} else{
goto _LL1155;} _LL1155: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1229: _temp1228=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1133)->f1; goto _LL1226; _LL1226:
_temp1225=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1133)->f2; if( _temp1225 == 0){ goto _LL1157;} else{ _temp1227=* _temp1225;
goto _LL1156;}} else{ goto _LL1157;} _LL1157: if(( unsigned int) _temp1133 > 1u?((
struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1231:
_temp1230=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1133)->f1; goto _LL1158;} else{ goto _LL1159;} _LL1159: if(( unsigned int)
_temp1133 > 1u?(( struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1233: _temp1232=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1133)->f1; goto _LL1160;} else{ goto _LL1161;} _LL1161: if(( unsigned int)
_temp1133 > 1u?(( struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1237: _temp1236=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1133)->f1; goto _LL1235; _LL1235: _temp1234=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1133)->f2; goto _LL1162;} else{ goto
_LL1163;} _LL1163: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1241: _temp1240=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1133)->f1; goto _LL1239; _LL1239:
_temp1238=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1133)->f2; goto _LL1164;} else{ goto _LL1165;} _LL1165: if(( unsigned int)
_temp1133 > 1u?(( struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1245: _temp1244=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1133)->f1; goto _LL1243; _LL1243: _temp1242=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1133)->f2; goto _LL1166;} else{ goto
_LL1167;} _LL1167: if(( unsigned int) _temp1133 > 1u?(( struct _enum_struct*)
_temp1133)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1249: _temp1248=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1133)->f1; _temp1250=* _temp1248; _LL1254:
_temp1253=( void*) _temp1250.r; if((( struct _enum_struct*) _temp1253)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1256: _temp1255=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1253)->f1; goto _LL1252;} else{ goto _LL1169;}
_LL1252: _temp1251=( struct Cyc_Position_Segment*) _temp1250.loc; goto _LL1247;
_LL1247: _temp1246=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1133)->f2; goto _LL1168;} else{ goto _LL1169;} _LL1169: if(( unsigned int)
_temp1133 > 1u?(( struct _enum_struct*) _temp1133)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1260: _temp1259=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1133)->f1; _temp1261=* _temp1259; _LL1265: _temp1264=( void*) _temp1261.r;
if((( struct _enum_struct*) _temp1264)->tag == Cyc_Absyn_Let_d_tag){ _LL1278:
_temp1277=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1264)->f1;
goto _LL1273; _LL1273: _temp1272=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1264)->f2; if( _temp1272 == 0){ goto _LL1171;} else{ _temp1274=* _temp1272;
_LL1276: _temp1275=( struct Cyc_List_List*) _temp1274.v; goto _LL1271;} _LL1271:
_temp1270=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1264)->f3;
goto _LL1269; _LL1269: _temp1268=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1264)->f4; goto _LL1267; _LL1267: _temp1266=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1264)->f5; goto _LL1263;} else{ goto _LL1171;} _LL1263: _temp1262=( struct
Cyc_Position_Segment*) _temp1261.loc; goto _LL1258; _LL1258: _temp1257=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1133)->f2; goto _LL1170;}
else{ goto _LL1171;} _LL1171: if(( unsigned int) _temp1133 > 1u?(( struct
_enum_struct*) _temp1133)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1282: _temp1281=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1133)->f1; goto
_LL1280; _LL1280: _temp1279=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1133)->f2; goto _LL1172;} else{ goto _LL1173;} _LL1173: goto _LL1174;
_LL1136: return in_flow; _LL1138: return Cyc_CfFlowInfo_BottomFL; _LL1140: Cyc_CfAbsexp_eval_absexp(
env, _temp1177->loc,( void*) aes->hd, in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1142: return Cyc_CfAbsexp_eval_absexp( env, _temp1179->loc,( void*) aes->hd,
in_flow); _LL1144: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1181,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1183, in_flow)); _LL1146: { void*
_temp1285; void* _temp1287; struct _tuple0 _temp1283= Cyc_NewControlFlow_cf_evalguard(
env, _temp1189->loc, aes, in_flow); _LL1288: _temp1287= _temp1283.f1; goto
_LL1286; _LL1286: _temp1285= _temp1283.f2; goto _LL1284; _LL1284: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1187, _temp1287), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1185, _temp1285));} _LL1148: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1195))->flow; void* _temp1291; void* _temp1293; struct _tuple0
_temp1289= Cyc_NewControlFlow_cf_evalguard( env, _temp1197->loc, aes, e_inflow);
_LL1294: _temp1293= _temp1289.f1; goto _LL1292; _LL1292: _temp1291= _temp1289.f2;
goto _LL1290; _LL1290: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1191, _temp1293); Cyc_NewControlFlow_update_flow( _temp1195,
body_outflow); return _temp1291;}} _LL1150: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1205, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1201))->flow; void* _temp1297; void* _temp1299; struct
_tuple0 _temp1295= Cyc_NewControlFlow_cf_evalguard( env, _temp1203->loc, aes,
e_inflow); _LL1300: _temp1299= _temp1295.f1; goto _LL1298; _LL1298: _temp1297=
_temp1295.f2; goto _LL1296; _LL1296: Cyc_NewControlFlow_update_flow( _temp1205,
_temp1299); return _temp1297;} _LL1152: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1221->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1217))->flow; void* _temp1303; void* _temp1305; struct _tuple0
_temp1301= Cyc_NewControlFlow_cf_evalguard( env, _temp1219->loc, aes->tl,
e2_inflow); _LL1306: _temp1305= _temp1301.f1; goto _LL1304; _LL1304: _temp1303=
_temp1301.f2; goto _LL1302; _LL1302: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1207, _temp1305); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1211))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1213->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1217, e3_outflow); return _temp1303;}} _LL1154: return Cyc_CfFlowInfo_BottomFL;
_LL1156: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1227->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1230=( struct
Cyc_Absyn_Stmt*) _temp1227->body; goto _LL1158;} _LL1158: _temp1232= _temp1230;
goto _LL1160; _LL1160: _temp1234= _temp1232; goto _LL1162; _LL1162: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1307= _temp1234; if(
_temp1307 == 0){ _throw( Null_Exception);} _temp1307;}), in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1164: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1240->loc,( void*) aes->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1238, aes->tl,
in_flow); return Cyc_CfFlowInfo_BottomFL; _LL1166: { struct Cyc_NewControlFlow_AnalEnv
_temp1310; void* _temp1311; int _temp1313; struct Cyc_Dict_Dict* _temp1315;
struct Cyc_NewControlFlow_AnalEnv* _temp1308= env; _temp1310=* _temp1308;
_LL1316: _temp1315=( struct Cyc_Dict_Dict*) _temp1310.roots; goto _LL1314;
_LL1314: _temp1313=( int) _temp1310.in_try; goto _LL1312; _LL1312: _temp1311=(
void*) _temp1310.tryflow; goto _LL1309; _LL1309: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1244, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1313;( void*)( env->tryflow=( void*) _temp1311); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1242,
aes, scs_inflow); return s_outflow;}} _LL1168: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1317; _temp1317.hd=( void*) _temp1255; _temp1317.tl= 0;
_temp1317;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1255->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1318= e; if( _temp1318 == 0){ _throw( Null_Exception);}
_temp1318;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1246, body_inflow);} _LL1170: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1275); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1268->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1275); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1257, body_inflow);} _LL1172: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1279, in_flow); _LL1174:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1319=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1319->tag= Cyc_Core_Impossible_tag; _temp1319->f1=( struct _tagged_string)({
char* _temp1320=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1321; _temp1321.curr= _temp1320; _temp1321.base=
_temp1320; _temp1321.last_plus_one= _temp1320 + 62; _temp1321;});( struct
_xenum_struct*) _temp1319;})); _LL1134:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1322=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_EmptyAnnot_struct)); _temp1322->tag= Cyc_Absyn_EmptyAnnot_tag;(
struct _xenum_struct*) _temp1322;});{ void* _temp1323=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp1345; struct Cyc_Absyn_Stmt* _temp1347; struct Cyc_Absyn_Stmt* _temp1349;
struct Cyc_Absyn_Stmt* _temp1351; struct Cyc_Absyn_Exp* _temp1353; struct Cyc_Absyn_Stmt*
_temp1355; struct _tuple3 _temp1357; struct Cyc_Absyn_Stmt* _temp1359; struct
Cyc_Absyn_Exp* _temp1361; struct _tuple3 _temp1363; struct Cyc_Absyn_Stmt*
_temp1365; struct Cyc_Absyn_Exp* _temp1367; struct Cyc_Absyn_Exp* _temp1369;
struct Cyc_Absyn_Stmt* _temp1371; struct _tuple3 _temp1373; struct Cyc_Absyn_Stmt*
_temp1375; struct Cyc_Absyn_Exp* _temp1377; struct _tuple3 _temp1379; struct Cyc_Absyn_Stmt*
_temp1381; struct Cyc_Absyn_Exp* _temp1383; struct Cyc_Absyn_Stmt* _temp1385;
struct Cyc_Absyn_Stmt* _temp1387; struct _tagged_string* _temp1389; struct Cyc_Absyn_Stmt*
_temp1391; struct Cyc_Absyn_Decl* _temp1393; struct Cyc_List_List* _temp1395;
struct Cyc_Absyn_Stmt* _temp1397; struct Cyc_List_List* _temp1399; struct Cyc_Absyn_Exp*
_temp1401; _LL1325: if(( unsigned int) _temp1323 > 1u?(( struct _enum_struct*)
_temp1323)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1348: _temp1347=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1323)->f1; goto _LL1346; _LL1346: _temp1345=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1323)->f2; goto
_LL1326;} else{ goto _LL1327;} _LL1327: if(( unsigned int) _temp1323 > 1u?((
struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1354:
_temp1353=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1323)->f1; goto _LL1352; _LL1352: _temp1351=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1323)->f2; goto _LL1350; _LL1350:
_temp1349=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1323)->f3; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(( unsigned int)
_temp1323 > 1u?(( struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1370: _temp1369=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1323)->f1; goto _LL1364; _LL1364: _temp1363=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1323)->f2; _LL1368: _temp1367= _temp1363.f1; goto _LL1366; _LL1366:
_temp1365= _temp1363.f2; goto _LL1358; _LL1358: _temp1357=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1323)->f3; _LL1362: _temp1361= _temp1357.f1;
goto _LL1360; _LL1360: _temp1359= _temp1357.f2; goto _LL1356; _LL1356: _temp1355=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1323)->f4; goto
_LL1330;} else{ goto _LL1331;} _LL1331: if(( unsigned int) _temp1323 > 1u?((
struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1374:
_temp1373=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1323)->f1;
_LL1378: _temp1377= _temp1373.f1; goto _LL1376; _LL1376: _temp1375= _temp1373.f2;
goto _LL1372; _LL1372: _temp1371=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1323)->f2; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(( unsigned int)
_temp1323 > 1u?(( struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1386: _temp1385=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1323)->f1; goto _LL1380; _LL1380: _temp1379=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1323)->f2; _LL1384: _temp1383= _temp1379.f1; goto _LL1382; _LL1382:
_temp1381= _temp1379.f2; goto _LL1334;} else{ goto _LL1335;} _LL1335: if((
unsigned int) _temp1323 > 1u?(( struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1390: _temp1389=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1323)->f1; goto _LL1388; _LL1388: _temp1387=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1323)->f2; goto _LL1336;} else{ goto
_LL1337;} _LL1337: if(( unsigned int) _temp1323 > 1u?(( struct _enum_struct*)
_temp1323)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1394: _temp1393=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1323)->f1; goto _LL1392; _LL1392:
_temp1391=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1323)->f2;
goto _LL1338;} else{ goto _LL1339;} _LL1339: if(( unsigned int) _temp1323 > 1u?((
struct _enum_struct*) _temp1323)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1398:
_temp1397=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1323)->f1; goto _LL1396; _LL1396: _temp1395=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1323)->f2; goto _LL1340;} else{ goto
_LL1341;} _LL1341: if(( unsigned int) _temp1323 > 1u?(( struct _enum_struct*)
_temp1323)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1402: _temp1401=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1323)->f1; goto _LL1400; _LL1400:
_temp1399=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1323)->f2; goto _LL1342;} else{ goto _LL1343;} _LL1343: goto _LL1344;
_LL1326: _temp1351= _temp1347; _temp1349= _temp1345; goto _LL1328; _LL1328: Cyc_NewControlFlow_cf_postpass(
_temp1351); Cyc_NewControlFlow_cf_postpass( _temp1349); return; _LL1330: Cyc_NewControlFlow_cf_postpass(
_temp1359); _temp1375= _temp1355; _temp1371= _temp1365; goto _LL1332; _LL1332:
_temp1385= _temp1371; _temp1381= _temp1375; goto _LL1334; _LL1334: Cyc_NewControlFlow_cf_postpass(
_temp1381); _temp1387= _temp1385; goto _LL1336; _LL1336: _temp1391= _temp1387;
goto _LL1338; _LL1338: Cyc_NewControlFlow_cf_postpass( _temp1391); return;
_LL1340: Cyc_NewControlFlow_cf_postpass( _temp1397); _temp1399= _temp1395; goto
_LL1342; _LL1342: for( 0; _temp1399 != 0; _temp1399= _temp1399->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1399->hd)->body);} return; _LL1344:
return; _LL1324:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1403; _temp1403.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1404=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv)
* 1); _temp1404[ 0]=({ struct Cyc_NewControlFlow_SharedAbsEnv _temp1405;
_temp1405.all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1405.comprehension_vars= 0;
_temp1405;}); _temp1404;}); _temp1403.lhsides= 0; _temp1403.access_path= 0;
_temp1403;}); struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1408; struct Cyc_List_List*
_temp1410; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1412; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1414; struct Cyc_List_List* _temp1415; struct Cyc_Dict_Dict* _temp1417;
struct Cyc_NewControlFlow_AbsEnv _temp1406= absenv; _LL1413: _temp1412=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1406.shared; _temp1414=* _temp1412;
_LL1418: _temp1417=( struct Cyc_Dict_Dict*) _temp1414.all_roots; goto _LL1416;
_LL1416: _temp1415=( struct Cyc_List_List*) _temp1414.comprehension_vars; goto
_LL1411; _LL1411: _temp1410=( struct Cyc_List_List*) _temp1406.lhsides; goto
_LL1409; _LL1409: _temp1408=( struct Cyc_List_List*) _temp1406.access_path; goto
_LL1407; _LL1407: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1419=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)
* 1); _temp1419[ 0]=({ struct Cyc_NewControlFlow_AnalEnv _temp1420; _temp1420.roots=
_temp1417; _temp1420.in_try= 0; _temp1420.tryflow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1420;}); _temp1419;}); void* in_flow=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1421=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1421->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp1421->f1=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);(
void*) _temp1421;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( analenv,
in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow( analenv,
in_flow, _temp1415);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1422= out_flow; _LL1424: if( _temp1422 == Cyc_CfFlowInfo_BottomFL){
goto _LL1425;} else{ goto _LL1426;} _LL1426: goto _LL1427; _LL1425: goto _LL1423;
_LL1427: if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp1428=( char*)"function may complete without returning a value";
struct _tagged_string _temp1429; _temp1429.curr= _temp1428; _temp1429.base=
_temp1428; _temp1429.last_plus_one= _temp1428 + 48; _temp1429;}));} goto _LL1423;
_LL1423:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1430=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1440; struct
Cyc_List_List* _temp1442; struct _tuple1* _temp1444; struct Cyc_List_List*
_temp1446; struct _tagged_string* _temp1448; _LL1432: if((( struct _enum_struct*)
_temp1430)->tag == Cyc_Absyn_Fn_d_tag){ _LL1441: _temp1440=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1430)->f1; goto _LL1433;} else{ goto _LL1434;}
_LL1434: if((( struct _enum_struct*) _temp1430)->tag == Cyc_Absyn_Using_d_tag){
_LL1445: _temp1444=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1430)->f1; goto _LL1443; _LL1443: _temp1442=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1430)->f2; goto _LL1435;} else{ goto
_LL1436;} _LL1436: if((( struct _enum_struct*) _temp1430)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1449: _temp1448=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1430)->f1; goto _LL1447; _LL1447: _temp1446=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1430)->f2; goto _LL1437;} else{ goto
_LL1438;} _LL1438: goto _LL1439; _LL1433: Cyc_NewControlFlow_cf_check_fun(
_temp1440); goto _LL1431; _LL1435: _temp1446= _temp1442; goto _LL1437; _LL1437:
Cyc_NewControlFlow_cf_check( _temp1446); goto _LL1431; _LL1439: goto _LL1431;
_LL1431:;}}