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
Cyc_NewControlFlow_CFAnnot_struct));* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct){.tag=
Cyc_NewControlFlow_CFAnnot_tag,.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot
_temp1; _temp1.encloser= encloser; _temp1.visited= 0; _temp1.flow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1.absexps= 0; _temp1;})};( struct _xenum_struct*) _temp0;});} static struct
Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt*
s){ struct _xenum_struct* _temp2= s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp8; struct Cyc_NewControlFlow_CFStmtAnnot* _temp10; _LL4: if((*(( struct
_xenum_struct*) _temp2)).tag == Cyc_NewControlFlow_CFAnnot_tag){ _LL9: _temp8=((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp2)->f1; _temp10=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: goto _LL7; _LL5: return _temp10;
_LL7:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp11=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp11=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp12=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12;
_temp13.last_plus_one= _temp12 + 37; _temp13;})};( struct _xenum_struct*)
_temp11;})); _LL3:;} void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps=
ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict* all_roots;
struct Cyc_List_List* comprehension_vars; } ; struct Cyc_NewControlFlow_AbsEnv{
struct Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides;
struct Cyc_List_List* access_path; } ; typedef struct Cyc_NewControlFlow_AbsEnv
Cyc_NewControlFlow_absenv_t; struct Cyc_NewControlFlow_AbsSynOne{ void*
inner_exp; void* assigns; } ; typedef struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_abssyn_one_t;
struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne when_true;
struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List* lvalues; }
; typedef struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abssyn_t; void Cyc_NewControlFlow_add_var_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)((
env.shared)->all_roots,({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=( struct
Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp14->f1= vd;( void*) _temp14;}),
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(( void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)((
env.shared)->all_roots,({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp15=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp15->tag= Cyc_CfFlowInfo_MallocPt_tag; _temp15->f1= e;( void*) _temp15;}),
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));} void Cyc_NewControlFlow_add_enum_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Enumfield*
ef){ struct Cyc_Absyn_Enumfield _temp18; struct Cyc_Position_Segment* _temp19;
struct Cyc_List_List* _temp21; struct Cyc_List_List* _temp23; struct Cyc_Absyn_Exp*
_temp25; struct _tuple1* _temp27; struct Cyc_Absyn_Enumfield* _temp16= ef;
_temp18=* _temp16; _LL28: _temp27=( struct _tuple1*) _temp18.name; goto _LL26;
_LL26: _temp25=( struct Cyc_Absyn_Exp*) _temp18.tag; goto _LL24; _LL24: _temp23=(
struct Cyc_List_List*) _temp18.tvs; goto _LL22; _LL22: _temp21=( struct Cyc_List_List*)
_temp18.typs; goto _LL20; _LL20: _temp19=( struct Cyc_Position_Segment*) _temp18.loc;
goto _LL17; _LL17: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0; _temp21
!= 0; _temp21= _temp21->tl, ++ i){ struct _tuple4 _temp31; void* _temp32; struct
Cyc_Absyn_Tqual* _temp34; struct _tuple4* _temp29=( struct _tuple4*) _temp21->hd;
_temp31=* _temp29; _LL35: _temp34= _temp31.f1; goto _LL33; _LL33: _temp32=
_temp31.f2; goto _LL30; _LL30: d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
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
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp102=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp103=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 44; _temp104;})};( struct _xenum_struct*)
_temp102;})); _LL69:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp105=(
void*) e->r; struct Cyc_Absyn_Exp* _temp189; struct Cyc_List_List* _temp191;
struct Cyc_Absyn_Exp* _temp193; void* _temp195; int _temp197; void* _temp199;
void* _temp201; int _temp203; void* _temp205; void* _temp207; void* _temp209;
void* _temp211; struct Cyc_Absyn_Fndecl* _temp213; struct _tuple1* _temp215;
void* _temp217; struct Cyc_Absyn_Vardecl* _temp219; struct _tuple1* _temp221;
void* _temp223; struct Cyc_Absyn_Vardecl* _temp225; struct _tuple1* _temp227;
void* _temp229; struct Cyc_Absyn_Vardecl* _temp231; struct _tuple1* _temp233;
void* _temp235; struct Cyc_Absyn_Vardecl* _temp237; struct _tuple1* _temp239;
struct Cyc_Absyn_Enumfield* _temp241; struct Cyc_Absyn_Enumdecl* _temp243;
struct Cyc_List_List* _temp245; struct Cyc_Core_Opt* _temp247; struct Cyc_Core_Opt*
_temp249; struct Cyc_Absyn_Enumfield* _temp251; struct Cyc_Absyn_Xenumdecl*
_temp253; struct Cyc_List_List* _temp255; struct Cyc_Core_Opt* _temp257; void*
_temp259; void* _temp261; void* _temp263; struct Cyc_Absyn_Exp* _temp265; struct
Cyc_Core_Opt* _temp267; struct Cyc_Absyn_Exp* _temp269; struct Cyc_Absyn_Exp*
_temp271; struct Cyc_Core_Opt* _temp273; struct Cyc_Absyn_Exp* _temp275; struct
Cyc_List_List* _temp277; void* _temp279; struct Cyc_Absyn_Exp* _temp281; void*
_temp283; struct Cyc_List_List* _temp285; struct Cyc_Absyn_Exp* _temp287; struct
Cyc_Absyn_Exp* _temp289; struct Cyc_Absyn_Exp* _temp291; struct Cyc_Absyn_Exp*
_temp293; struct Cyc_Absyn_Exp* _temp295; struct _tagged_string* _temp297;
struct Cyc_Absyn_Exp* _temp299; struct _tagged_string* _temp301; struct Cyc_Absyn_Exp*
_temp303; struct Cyc_List_List* _temp305; struct Cyc_Absyn_Structdecl* _temp307;
struct Cyc_List_List* _temp309; struct Cyc_Core_Opt* _temp311; struct _tuple1*
_temp313; struct Cyc_List_List* _temp315; int _temp317; void* _temp319; struct
Cyc_Absyn_Exp* _temp321; struct Cyc_Absyn_Exp* _temp323; struct Cyc_Absyn_Exp*
_temp325; struct Cyc_Absyn_Exp* _temp327; struct Cyc_Absyn_Exp* _temp329; struct
Cyc_Absyn_Stmt* _temp331; struct Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp*
_temp335; struct Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Exp* _temp339; struct
Cyc_Absyn_Vardecl* _temp341; void* _temp343; struct _tuple1* _temp345; struct
_tuple1* _temp347; struct Cyc_List_List* _temp349; struct Cyc_Absyn_Exp*
_temp351; struct Cyc_List_List* _temp353; struct Cyc_Core_Opt* _temp355; struct
Cyc_List_List* _temp357; struct _tuple2* _temp359; struct Cyc_Absyn_Fndecl*
_temp361; struct Cyc_Absyn_Exp* _temp363; _LL107: if((( struct _enum_struct*)
_temp105)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL190: _temp189=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp105)->f1; goto _LL108;} else{
goto _LL109;} _LL109: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL194: _temp193=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp105)->f1; goto _LL192; _LL192: _temp191=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp105)->f2; goto _LL110;} else{ goto _LL111;}
_LL111: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Const_e_tag){
_LL196: _temp195=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp105)->f1; if((
unsigned int) _temp195 > 1u?(( struct _enum_struct*) _temp195)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp195)->f1;
if( _temp199 == Cyc_Absyn_Signed){ goto _LL198;} else{ goto _LL113;} _LL198:
_temp197=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp195)->f2; if( _temp197 ==
0){ goto _LL112;} else{ goto _LL113;}} else{ goto _LL113;}} else{ goto _LL113;}
_LL113: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Const_e_tag){
_LL202: _temp201=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp105)->f1; if((
unsigned int) _temp201 > 1u?(( struct _enum_struct*) _temp201)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL206: _temp205=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp201)->f1;
if( _temp205 == Cyc_Absyn_Signed){ goto _LL204;} else{ goto _LL115;} _LL204:
_temp203=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp201)->f2; if( _temp203 ==
1){ goto _LL114;} else{ goto _LL115;}} else{ goto _LL115;}} else{ goto _LL115;}
_LL115: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL208: _temp207=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp105)->f1;
goto _LL116;} else{ goto _LL117;} _LL117: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Const_e_tag){ _LL210: _temp209=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp105)->f1; goto _LL118;} else{ goto _LL119;} _LL119: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){ _LL216: _temp215=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1; goto _LL212; _LL212:
_temp211=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f2; if((
unsigned int) _temp211 > 1u?(( struct _enum_struct*) _temp211)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL214: _temp213=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp211)->f1; goto _LL120;} else{ goto _LL121;}} else{ goto _LL121;} _LL121:
if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){ _LL222:
_temp221=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL218; _LL218: _temp217=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp217 > 1u?(( struct _enum_struct*)
_temp217)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL220: _temp219=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp217)->f1; goto _LL122;} else{ goto
_LL123;}} else{ goto _LL123;} _LL123: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Var_e_tag){ _LL228: _temp227=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f1; goto _LL224; _LL224: _temp223=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp223 > 1u?(( struct _enum_struct*)
_temp223)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL226: _temp225=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp223)->f1; goto _LL124;} else{ goto _LL125;}}
else{ goto _LL125;} _LL125: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){
_LL234: _temp233=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL230; _LL230: _temp229=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp229 > 1u?(( struct _enum_struct*)
_temp229)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL232: _temp231=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp229)->f1; goto _LL126;} else{ goto _LL127;}}
else{ goto _LL127;} _LL127: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){
_LL240: _temp239=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL236; _LL236: _temp235=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp235 > 1u?(( struct _enum_struct*)
_temp235)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL238: _temp237=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp235)->f1; goto _LL128;} else{ goto _LL129;}}
else{ goto _LL129;} _LL129: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Enum_e_tag){
_LL250: _temp249=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp105)->f1; goto _LL248; _LL248: _temp247=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp105)->f2; goto _LL246; _LL246: _temp245=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp105)->f3; goto _LL244;
_LL244: _temp243=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp105)->f4; goto _LL242; _LL242: _temp241=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp105)->f5; goto _LL130;} else{ goto _LL131;}
_LL131: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Xenum_e_tag){
_LL258: _temp257=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp105)->f1; goto _LL256; _LL256: _temp255=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp105)->f2; goto _LL254; _LL254: _temp253=( struct
Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp105)->f3; goto
_LL252; _LL252: _temp251=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp105)->f4; goto _LL132;} else{ goto _LL133;} _LL133: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Malloc_e_tag){ _LL260: _temp259=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp105)->f1; if((( struct
_enum_struct*) _temp259)->tag == Cyc_Absyn_Typ_m_tag){ _LL262: _temp261=( void*)((
struct Cyc_Absyn_Typ_m_struct*) _temp259)->f1; goto _LL134;} else{ goto _LL135;}}
else{ goto _LL135;} _LL135: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Malloc_e_tag){
_LL264: _temp263=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp105)->f1;
goto _LL136;} else{ goto _LL137;} _LL137: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL270: _temp269=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp105)->f1; goto _LL268; _LL268: _temp267=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f2; if(
_temp267 == 0){ goto _LL266;} else{ goto _LL139;} _LL266: _temp265=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f3; goto _LL138;} else{ goto
_LL139;} _LL139: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL276: _temp275=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp105)->f1; goto _LL274; _LL274: _temp273=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp105)->f2; goto _LL272; _LL272: _temp271=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f3;
goto _LL140;} else{ goto _LL141;} _LL141: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Primop_e_tag){ _LL280: _temp279=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp105)->f1; goto _LL278; _LL278: _temp277=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp105)->f2; goto _LL142;} else{ goto _LL143;}
_LL143: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Cast_e_tag){
_LL284: _temp283=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp105)->f1; goto
_LL282; _LL282: _temp281=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp105)->f2; goto _LL144;} else{ goto _LL145;} _LL145: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_FnCall_e_tag){ _LL288: _temp287=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp105)->f1; goto
_LL286; _LL286: _temp285=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp105)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Subscript_e_tag){ _LL292: _temp291=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp105)->f1;
goto _LL290; _LL290: _temp289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp105)->f2; goto _LL148;} else{ goto _LL149;} _LL149: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Address_e_tag){ _LL294: _temp293=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp105)->f1; goto
_LL150;} else{ goto _LL151;} _LL151: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Deref_e_tag){ _LL296: _temp295=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp105)->f1; goto _LL152;} else{ goto _LL153;}
_LL153: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL300: _temp299=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp105)->f1; goto _LL298; _LL298: _temp297=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp105)->f2; goto _LL154;} else{ goto _LL155;}
_LL155: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StructMember_e_tag){
_LL304: _temp303=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp105)->f1; goto _LL302; _LL302: _temp301=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp105)->f2; goto _LL156;} else{ goto _LL157;}
_LL157: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Tuple_e_tag){
_LL306: _temp305=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp105)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Struct_e_tag){ _LL314: _temp313=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp105)->f1; goto _LL312;
_LL312: _temp311=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp105)->f2; goto _LL310; _LL310: _temp309=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp105)->f3; goto _LL308; _LL308: _temp307=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp105)->f4;
goto _LL160;} else{ goto _LL161;} _LL161: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Array_e_tag){ _LL318: _temp317=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp105)->f1; goto _LL316; _LL316: _temp315=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp105)->f2; goto _LL162;} else{ goto _LL163;}
_LL163: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Increment_e_tag){
_LL322: _temp321=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp105)->f1; goto _LL320; _LL320: _temp319=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp105)->f2; goto _LL164;} else{ goto _LL165;} _LL165: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Throw_e_tag){ _LL324: _temp323=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp105)->f1; goto
_LL166;} else{ goto _LL167;} _LL167: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL330: _temp329=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp105)->f1; goto _LL328; _LL328:
_temp327=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp105)->f2; goto _LL326; _LL326: _temp325=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp105)->f3; goto _LL168;} else{ goto _LL169;}
_LL169: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL332: _temp331=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp105)->f1; goto _LL170;} else{ goto _LL171;} _LL171: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL336: _temp335=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp105)->f1; goto
_LL334; _LL334: _temp333=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp105)->f2; goto _LL172;} else{ goto _LL173;} _LL173: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL342:
_temp341=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp105)->f1; goto _LL340; _LL340: _temp339=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp105)->f2; goto _LL338; _LL338: _temp337=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp105)->f3;
goto _LL174;} else{ goto _LL175;} _LL175: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Var_e_tag){ _LL346: _temp345=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f1; goto _LL344; _LL344: _temp343=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if( _temp343 == Cyc_Absyn_Unresolved_b){ goto _LL176;} else{ goto
_LL177;}} else{ goto _LL177;} _LL177: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL348: _temp347=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp105)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL352: _temp351=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp105)->f1;
goto _LL350; _LL350: _temp349=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp105)->f2; goto _LL180;} else{ goto _LL181;} _LL181: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL356:
_temp355=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp105)->f1; goto _LL354; _LL354: _temp353=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp105)->f2; goto _LL182;} else{ goto
_LL183;} _LL183: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL360: _temp359=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp105)->f1; goto _LL358; _LL358: _temp357=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp105)->f2; goto _LL184;} else{ goto _LL185;}
_LL185: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Codegen_e_tag){
_LL362: _temp361=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp105)->f1; goto _LL186;} else{ goto _LL187;} _LL187: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Fill_e_tag){ _LL364: _temp363=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp105)->f1; goto
_LL188;} else{ goto _LL106;} _LL108: _temp193= _temp189; goto _LL110; _LL110:
return Cyc_NewControlFlow_abstract_exp( env, _temp193); _LL112: env.access_path
== 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp365=( char*)"new_control_flow.cyc:257 env.access_path==null";
struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 47; _temp366;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp367; _temp367.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp369;
_temp369.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp369.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp369;}); _temp367.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp368; _temp368.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp368.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp368;}); _temp367.lvalues= 0; _temp367;}); _LL114: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp370=( char*)"new_control_flow.cyc:262 env.access_path==null";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 47; _temp371;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp372; _temp372.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp374;
_temp374.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp374.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp374;});
_temp372.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp373; _temp373.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp373.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp373;}); _temp372.lvalues= 0; _temp372;}); _LL116: goto _LL118; _LL118: goto
_LL120; _LL120: goto _LL122; return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL122:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL124:
_temp231= _temp225; goto _LL126; _LL126: _temp237= _temp231; goto _LL128; _LL128: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp237)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp375=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp375[ 0]=({ struct Cyc_List_List
_temp376; _temp376.hd=( void*) absop; _temp376.tl= 0; _temp376;}); _temp375;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL130: _temp255= _temp245;
_temp251= _temp241; goto _LL132; _LL132: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp377=( char*)"new_control_flow.cyc:292 env.access_path == null";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 49; _temp378;})); if( _temp255 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_enum_root( env, e,
_temp251);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp255 != 0; _temp255= _temp255->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp379=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp379[ 0]=({ struct Cyc_List_List _temp380; _temp380.hd=( void*) Cyc_CfAbsexp_mkMemberOp(
Cyc_CfAbsexp_mkMallocOp( e),({ struct Cyc_CfFlowInfo_TupleF_struct* _temp381=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp381->tag= Cyc_CfFlowInfo_TupleF_tag; _temp381->f1= i;( void*) _temp381;}));
_temp380.tl= 0; _temp380;}); _temp379;})); struct Cyc_NewControlFlow_AbsSyn syn=
Cyc_NewControlFlow_abstract_exp( elt_env,( struct Cyc_Absyn_Exp*) _temp255->hd);
void* _temp384; void* _temp386; struct Cyc_NewControlFlow_AbsSynOne _temp382=
Cyc_NewControlFlow_meet_absexp( syn); _LL387: _temp386=( void*) _temp382.inner_exp;
goto _LL385; _LL385: _temp384=( void*) _temp382.assigns; goto _LL383; _LL383:
es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp386, es_exp);
useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp384, useexp);}}
return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp, useexp);} _LL134: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp261); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE(
e), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL136:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp388=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp388=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp389=( char*)"NewControlFlow: Malloc_e of unsupported variant";
struct _tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 48; _temp390;})};( struct _xenum_struct*)
_temp388;})); _LL138: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp269); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp265); void* _temp393; void*
_temp395; struct Cyc_NewControlFlow_AbsSynOne _temp391= Cyc_NewControlFlow_meet_absexp(
syn1); _LL396: _temp395=( void*) _temp391.inner_exp; goto _LL394; _LL394:
_temp393=( void*) _temp391.assigns; goto _LL392; _LL392: { void* _temp399; void*
_temp401; struct Cyc_NewControlFlow_AbsSynOne _temp397= Cyc_NewControlFlow_meet_absexp(
syn2); _LL402: _temp401=( void*) _temp397.inner_exp; goto _LL400; _LL400:
_temp399=( void*) _temp397.assigns; goto _LL398; _LL398: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp395,
_temp401), _temp399); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp393);}}}
_LL140: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp403=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp403[ 0]=({ struct Cyc_List_List _temp404;
_temp404.hd=( void*) _temp275; _temp404.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp405=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp405[ 0]=({ struct Cyc_List_List _temp406; _temp406.hd=( void*) _temp271;
_temp406.tl= 0; _temp406;}); _temp405;}); _temp404;}); _temp403;}), 0); _LL142:
return Cyc_NewControlFlow_use_exp_unordered( env, _temp277, 0); _LL144: { int
both_leaf= 1;{ void* _temp407=( void*)( _temp281->topt)->v; struct Cyc_Absyn_Structdecl**
_temp415; struct Cyc_List_List* _temp417; struct _tuple1* _temp419; struct Cyc_List_List*
_temp421; _LL409: if(( unsigned int) _temp407 > 5u?(( struct _enum_struct*)
_temp407)->tag == Cyc_Absyn_StructType_tag: 0){ _LL420: _temp419=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp407)->f1; goto _LL418;
_LL418: _temp417=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp407)->f2; goto _LL416; _LL416: _temp415=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp407)->f3; goto _LL410;} else{ goto
_LL411;} _LL411: if(( unsigned int) _temp407 > 5u?(( struct _enum_struct*)
_temp407)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL422: _temp421=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp407)->f1; goto _LL412;} else{ goto
_LL413;} _LL413: goto _LL414; _LL410: goto _LL412; _LL412: both_leaf= 0; goto
_LL408; _LL414: goto _LL408; _LL408:;}{ void* _temp423=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp431; struct Cyc_List_List* _temp433; struct
_tuple1* _temp435; struct Cyc_List_List* _temp437; _LL425: if(( unsigned int)
_temp423 > 5u?(( struct _enum_struct*) _temp423)->tag == Cyc_Absyn_StructType_tag:
0){ _LL436: _temp435=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp423)->f1; goto _LL434; _LL434: _temp433=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp423)->f2; goto _LL432; _LL432: _temp431=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp423)->f3;
goto _LL426;} else{ goto _LL427;} _LL427: if(( unsigned int) _temp423 > 5u?((
struct _enum_struct*) _temp423)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL438:
_temp437=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp423)->f1;
goto _LL428;} else{ goto _LL429;} _LL429: goto _LL430; _LL426: goto _LL428;
_LL428: both_leaf= 0; goto _LL424; _LL430: goto _LL424; _LL424:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp281);} return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp439=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp439[ 0]=({ struct Cyc_List_List
_temp440; _temp440.hd=( void*) _temp281; _temp440.tl= 0; _temp440;}); _temp439;}),
env.lhsides);} _LL146: return Cyc_NewControlFlow_use_exp_unordered( env,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp441=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp441[ 0]=({ struct Cyc_List_List
_temp442; _temp442.hd=( void*) _temp287; _temp442.tl= _temp285; _temp442;});
_temp441;}), 0); _LL148: { void* _temp443= Cyc_Tcutil_compress(( void*)(
_temp291->topt)->v); struct Cyc_List_List* _temp449; _LL445: if(( unsigned int)
_temp443 > 5u?(( struct _enum_struct*) _temp443)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL450: _temp449=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp443)->f1; goto _LL446;} else{ goto _LL447;} _LL447: goto _LL448; _LL446:
env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp451=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp451[
0]=({ struct Cyc_List_List _temp452; _temp452.hd=( void*) e; _temp452.tl= env.access_path;
_temp452;}); _temp451;}); return Cyc_NewControlFlow_abstract_exp( env, _temp291);
_LL448: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp453=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp453[ 0]=({ struct Cyc_List_List _temp454;
_temp454.hd=( void*) _temp291; _temp454.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp455=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp455[ 0]=({ struct Cyc_List_List _temp456; _temp456.hd=( void*) _temp289;
_temp456.tl= 0; _temp456;}); _temp455;}); _temp454;}); _temp453;}), Cyc_NewControlFlow_use_it_list());
_LL444:;} _LL150:{ void* _temp457=( void*) _temp293->r; struct Cyc_Absyn_Structdecl*
_temp465; struct Cyc_List_List* _temp467; struct Cyc_Core_Opt* _temp469; struct
_tuple1* _temp471; struct Cyc_List_List* _temp473; _LL459: if((( struct
_enum_struct*) _temp457)->tag == Cyc_Absyn_Struct_e_tag){ _LL472: _temp471=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp457)->f1; goto _LL470;
_LL470: _temp469=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp457)->f2; goto _LL468; _LL468: _temp467=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp457)->f3; goto _LL466; _LL466: _temp465=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp457)->f4;
goto _LL460;} else{ goto _LL461;} _LL461: if((( struct _enum_struct*) _temp457)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL474: _temp473=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp457)->f1; goto _LL462;} else{ goto _LL463;}
_LL463: goto _LL464; _LL460: goto _LL462; _LL462: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp293->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp475=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp475[ 0]=({ struct Cyc_List_List
_temp476; _temp476.hd=( void*) malloc_op; _temp476.tl= 0; _temp476;}); _temp475;}));
inner_env.access_path= 0;{ void* _temp479; void* _temp481; struct Cyc_NewControlFlow_AbsSynOne
_temp477= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp293)); _LL482: _temp481=( void*) _temp477.inner_exp; goto _LL480;
_LL480: _temp479=( void*) _temp477.assigns; goto _LL478; _LL478: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp481, _temp479); if( env.access_path
!= 0){ void* _temp483=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp493; struct Cyc_Absyn_Exp* _temp495; struct Cyc_Absyn_Exp*
_temp497; struct _tagged_string* _temp499; struct Cyc_Absyn_Exp* _temp501;
_LL485: if((( struct _enum_struct*) _temp483)->tag == Cyc_Absyn_Subscript_e_tag){
_LL496: _temp495=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp483)->f1; goto _LL494; _LL494: _temp493=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp483)->f2; goto _LL486;} else{ goto _LL487;}
_LL487: if((( struct _enum_struct*) _temp483)->tag == Cyc_Absyn_Deref_e_tag){
_LL498: _temp497=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp483)->f1; goto _LL488;} else{ goto _LL489;} _LL489: if((( struct
_enum_struct*) _temp483)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL502: _temp501=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp483)->f1;
goto _LL500; _LL500: _temp499=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp483)->f2; goto _LL490;} else{ goto _LL491;} _LL491: goto _LL492; _LL486:
Cyc_Evexp_eval_const_uint_exp( _temp493) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp503=( char*)"new_control_flow.cyc:410 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 61; _temp504;})); goto _LL488; _LL488: env.access_path=(
env.access_path)->tl; ans.lvalues=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp505=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp505[ 0]=({ struct Cyc_List_List _temp506; _temp506.hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp506.tl= 0; _temp506;}); _temp505;}); goto _LL484; _LL490:
env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp507=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp507[
0]=({ struct Cyc_List_List _temp508; _temp508.hd=( void*) Cyc_Absyn_structmember_exp(
_temp501, _temp499, 0); _temp508.tl=( env.access_path)->tl; _temp508;});
_temp507;}); ans.lvalues=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp509=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp509[ 0]=({ struct Cyc_List_List _temp510; _temp510.hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp510.tl= 0; _temp510;}); _temp509;}); goto _LL484; _LL492:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp511=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp511=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp512=( char*)"bad malloc access path";
struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 23; _temp513;})};( struct _xenum_struct*)
_temp511;})); _LL484:;} return ans;}}}} _LL464: goto _LL458; _LL458:;}{ struct
Cyc_List_List* _temp514= env.access_path; struct Cyc_List_List _temp526; struct
Cyc_List_List* _temp527; struct Cyc_Absyn_Exp* _temp529; struct Cyc_Absyn_Exp
_temp531; struct Cyc_Position_Segment* _temp532; void* _temp534; struct Cyc_Absyn_Exp*
_temp536; struct Cyc_Absyn_Exp* _temp538; struct Cyc_Core_Opt* _temp540; struct
Cyc_List_List _temp542; struct Cyc_List_List* _temp543; struct Cyc_Absyn_Exp*
_temp545; struct Cyc_Absyn_Exp _temp547; struct Cyc_Position_Segment* _temp548;
void* _temp550; struct Cyc_Absyn_Exp* _temp552; struct Cyc_Core_Opt* _temp554;
struct Cyc_List_List _temp556; struct Cyc_List_List* _temp557; struct Cyc_Absyn_Exp*
_temp559; struct Cyc_Absyn_Exp _temp561; struct Cyc_Position_Segment* _temp562;
void* _temp564; struct _tagged_string* _temp566; struct Cyc_Absyn_Exp* _temp568;
struct Cyc_Core_Opt* _temp570; _LL516: if( _temp514 == 0){ goto _LL517;} else{
goto _LL518;} _LL518: if( _temp514 == 0){ goto _LL520;} else{ _temp526=*
_temp514; _LL530: _temp529=( struct Cyc_Absyn_Exp*) _temp526.hd; _temp531=*
_temp529; _LL541: _temp540=( struct Cyc_Core_Opt*) _temp531.topt; goto _LL535;
_LL535: _temp534=( void*) _temp531.r; if((( struct _enum_struct*) _temp534)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL539: _temp538=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp534)->f1; goto _LL537; _LL537:
_temp536=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp534)->f2; goto _LL533;} else{ goto _LL520;} _LL533: _temp532=( struct Cyc_Position_Segment*)
_temp531.loc; goto _LL528; _LL528: _temp527=( struct Cyc_List_List*) _temp526.tl;
goto _LL519;} _LL520: if( _temp514 == 0){ goto _LL522;} else{ _temp542=*
_temp514; _LL546: _temp545=( struct Cyc_Absyn_Exp*) _temp542.hd; _temp547=*
_temp545; _LL555: _temp554=( struct Cyc_Core_Opt*) _temp547.topt; goto _LL551;
_LL551: _temp550=( void*) _temp547.r; if((( struct _enum_struct*) _temp550)->tag
== Cyc_Absyn_Deref_e_tag){ _LL553: _temp552=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp550)->f1; goto _LL549;} else{ goto _LL522;}
_LL549: _temp548=( struct Cyc_Position_Segment*) _temp547.loc; goto _LL544;
_LL544: _temp543=( struct Cyc_List_List*) _temp542.tl; goto _LL521;} _LL522: if(
_temp514 == 0){ goto _LL524;} else{ _temp556=* _temp514; _LL560: _temp559=(
struct Cyc_Absyn_Exp*) _temp556.hd; _temp561=* _temp559; _LL571: _temp570=(
struct Cyc_Core_Opt*) _temp561.topt; goto _LL565; _LL565: _temp564=( void*)
_temp561.r; if((( struct _enum_struct*) _temp564)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL569: _temp568=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp564)->f1; goto _LL567; _LL567: _temp566=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp564)->f2; goto _LL563;} else{ goto _LL524;}
_LL563: _temp562=( struct Cyc_Position_Segment*) _temp561.loc; goto _LL558;
_LL558: _temp557=( struct Cyc_List_List*) _temp556.tl; goto _LL523;} _LL524:
goto _LL525; _LL517: env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp572=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp572[ 0]=({ struct Cyc_List_List _temp573; _temp573.hd=( void*) e; _temp573.tl=
env.access_path; _temp573;}); _temp572;}); goto _LL515; _LL519: Cyc_Evexp_eval_const_uint_exp(
_temp536) ==( unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp574=( char*)"new_control_flow.cyc:438 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp575; _temp575.curr= _temp574; _temp575.base= _temp574;
_temp575.last_plus_one= _temp574 + 61; _temp575;})); _temp543= _temp527; goto
_LL521; _LL521: env.access_path= _temp543; goto _LL515; _LL523: env.access_path=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp576=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp576[ 0]=({ struct Cyc_List_List
_temp577; _temp577.hd=( void*) Cyc_Absyn_structmember_exp( _temp568, _temp566, 0);
_temp577.tl= _temp557; _temp577;}); _temp576;}); goto _LL515; _LL525:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp578=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp578=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp579=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 36; _temp580;})};( struct _xenum_struct*)
_temp578;})); _LL515:;} return Cyc_NewControlFlow_abstract_exp( env, _temp293);
_LL152:{ struct Cyc_List_List* _temp581= env.access_path; struct Cyc_List_List
_temp587; struct Cyc_List_List* _temp588; struct Cyc_Absyn_Exp* _temp590; struct
Cyc_Absyn_Exp _temp592; struct Cyc_Position_Segment* _temp593; void* _temp595;
struct Cyc_Absyn_Exp* _temp597; struct Cyc_Core_Opt* _temp599; _LL583: if(
_temp581 == 0){ goto _LL585;} else{ _temp587=* _temp581; _LL591: _temp590=(
struct Cyc_Absyn_Exp*) _temp587.hd; _temp592=* _temp590; _LL600: _temp599=(
struct Cyc_Core_Opt*) _temp592.topt; goto _LL596; _LL596: _temp595=( void*)
_temp592.r; if((( struct _enum_struct*) _temp595)->tag == Cyc_Absyn_Address_e_tag){
_LL598: _temp597=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp595)->f1; goto _LL594;} else{ goto _LL585;} _LL594: _temp593=( struct Cyc_Position_Segment*)
_temp592.loc; goto _LL589; _LL589: _temp588=( struct Cyc_List_List*) _temp587.tl;
goto _LL584;} _LL585: goto _LL586; _LL584: env.access_path= _temp588;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp295);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL586: goto _LL582; _LL582:;} _temp299= _temp295; goto _LL154; _LL154:
_temp303= _temp299; goto _LL156; _LL156: env.access_path=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp601=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp601[ 0]=({ struct Cyc_List_List _temp602;
_temp602.hd=( void*) e; _temp602.tl= env.access_path; _temp602;}); _temp601;});{
struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env,
_temp303); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;} _LL158: { struct Cyc_List_List* _temp603= env.access_path; struct
Cyc_List_List _temp611; struct Cyc_List_List* _temp612; struct Cyc_Absyn_Exp*
_temp614; struct Cyc_Absyn_Exp _temp616; struct Cyc_Position_Segment* _temp617;
void* _temp619; struct Cyc_Absyn_Exp* _temp621; struct Cyc_Absyn_Exp* _temp623;
struct Cyc_Core_Opt* _temp625; _LL605: if( _temp603 == 0){ goto _LL606;} else{
goto _LL607;} _LL607: if( _temp603 == 0){ goto _LL609;} else{ _temp611=*
_temp603; _LL615: _temp614=( struct Cyc_Absyn_Exp*) _temp611.hd; _temp616=*
_temp614; _LL626: _temp625=( struct Cyc_Core_Opt*) _temp616.topt; goto _LL620;
_LL620: _temp619=( void*) _temp616.r; if((( struct _enum_struct*) _temp619)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL624: _temp623=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp619)->f1; goto _LL622; _LL622:
_temp621=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp619)->f2; goto _LL618;} else{ goto _LL609;} _LL618: _temp617=( struct Cyc_Position_Segment*)
_temp616.loc; goto _LL613; _LL613: _temp612=( struct Cyc_List_List*) _temp611.tl;
goto _LL608;} _LL609: goto _LL610; _LL606: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp305 != 0;
_temp305= _temp305->tl, ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp627=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp627[ 0]=({ struct Cyc_List_List _temp628; _temp628.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp628.tl= ith_lvals; _temp628;}); _temp627;});} else{ ith_lvals=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp629=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp629[ 0]=({ struct Cyc_List_List _temp630;
_temp630.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp631=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp631->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp631->f1= j;( void*) _temp631;})); _temp630.tl= ith_lvals; _temp630;});
_temp629;});}}}{ struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp305->hd);
void* _temp634; void* _temp636; struct Cyc_NewControlFlow_AbsSynOne _temp632=
Cyc_NewControlFlow_meet_absexp( ith_syn); _LL637: _temp636=( void*) _temp632.inner_exp;
goto _LL635; _LL635: _temp634=( void*) _temp632.assigns; goto _LL633; _LL633:
ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, ansexp, _temp636);
assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), assignexp,
_temp634);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);} _LL608: {
int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp621); struct Cyc_NewControlFlow_AbsEnv
other_env= Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp612;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();{
int j= 0; for( 0; _temp305 != 0; _temp305= _temp305->tl, ++ j){ if( i == j){
void* _temp640; void* _temp642; struct Cyc_NewControlFlow_AbsSynOne _temp638=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*) _temp305->hd)); _LL643: _temp642=( void*) _temp638.inner_exp;
goto _LL641; _LL641: _temp640=( void*) _temp638.assigns; goto _LL639; _LL639:
assignexp= _temp640; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp642);} else{ void* _temp646; void* _temp648; struct Cyc_NewControlFlow_AbsSynOne
_temp644= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,( struct Cyc_Absyn_Exp*) _temp305->hd)); _LL649: _temp648=( void*)
_temp644.inner_exp; goto _LL647; _LL647: _temp646=( void*) _temp644.assigns;
goto _LL645; _LL645: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp648);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp650=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp650[ 0]=({ struct Cyc_List_List _temp651;
_temp651.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp651.tl= 0; _temp651;});
_temp650;}), assignexp);}} _LL610:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp652=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp652=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp653=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp654; _temp654.curr= _temp653; _temp654.base= _temp653;
_temp654.last_plus_one= _temp653 + 46; _temp654;})};( struct _xenum_struct*)
_temp652;})); _LL604:;} _LL160: { struct Cyc_List_List* _temp655= env.access_path;
struct Cyc_List_List _temp663; struct Cyc_List_List* _temp664; struct Cyc_Absyn_Exp*
_temp666; struct Cyc_Absyn_Exp _temp668; struct Cyc_Position_Segment* _temp669;
void* _temp671; struct _tagged_string* _temp673; struct Cyc_Absyn_Exp* _temp675;
struct Cyc_Core_Opt* _temp677; _LL657: if( _temp655 == 0){ goto _LL658;} else{
goto _LL659;} _LL659: if( _temp655 == 0){ goto _LL661;} else{ _temp663=*
_temp655; _LL667: _temp666=( struct Cyc_Absyn_Exp*) _temp663.hd; _temp668=*
_temp666; _LL678: _temp677=( struct Cyc_Core_Opt*) _temp668.topt; goto _LL672;
_LL672: _temp671=( void*) _temp668.r; if((( struct _enum_struct*) _temp671)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL676: _temp675=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp671)->f1; goto _LL674; _LL674:
_temp673=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp671)->f2; goto _LL670;} else{ goto _LL661;} _LL670: _temp669=( struct Cyc_Position_Segment*)
_temp668.loc; goto _LL665; _LL665: _temp664=( struct Cyc_List_List*) _temp663.tl;
goto _LL660;} _LL661: goto _LL662; _LL658: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp309 != 0; _temp309=
_temp309->tl){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls=
env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)
ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp679=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp679[
0]=({ struct Cyc_List_List _temp680; _temp680.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp680.tl= ith_lvals; _temp680;}); _temp679;});} else{ struct Cyc_List_List*
ds=(*(( struct _tuple5*) _temp309->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void*
_temp681=( void*) ds->hd; struct Cyc_Absyn_Exp* _temp687; struct _tagged_string*
_temp689; _LL683: if((( struct _enum_struct*) _temp681)->tag == Cyc_Absyn_ArrayElement_tag){
_LL688: _temp687=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp681)->f1; goto _LL684;} else{ goto _LL685;} _LL685: if((( struct
_enum_struct*) _temp681)->tag == Cyc_Absyn_FieldName_tag){ _LL690: _temp689=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp681)->f1;
goto _LL686;} else{ goto _LL682;} _LL684:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp691=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp691=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp692=( char*)"bad struct designator"; struct
_tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 22; _temp693;})};( struct _xenum_struct*)
_temp691;})); _LL686: ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp694=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp694[ 0]=({ struct Cyc_List_List _temp695; _temp695.hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,({ struct Cyc_CfFlowInfo_StructF_struct* _temp696=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp696->tag= Cyc_CfFlowInfo_StructF_tag;
_temp696->f1= _temp689;( void*) _temp696;})); _temp695.tl= ith_lvals; _temp695;});
_temp694;}); goto _LL682; _LL682:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp309->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp309->hd)).f2); void* _temp699; void* _temp701; struct Cyc_NewControlFlow_AbsSynOne
_temp697= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL702: _temp701=( void*)
_temp697.inner_exp; goto _LL700; _LL700: _temp699=( void*) _temp697.assigns;
goto _LL698; _LL698: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp701); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp699);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL660: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp664;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp309 != 0; _temp309=
_temp309->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp309->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp703=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp709; struct _tagged_string* _temp711; _LL705: if(((
struct _enum_struct*) _temp703)->tag == Cyc_Absyn_ArrayElement_tag){ _LL710:
_temp709=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp703)->f1; goto _LL706;} else{ goto _LL707;} _LL707: if((( struct
_enum_struct*) _temp703)->tag == Cyc_Absyn_FieldName_tag){ _LL712: _temp711=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp703)->f1;
goto _LL708;} else{ goto _LL704;} _LL706:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp713=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp713=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp714=( char*)"bad struct designator"; struct
_tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 22; _temp715;})};( struct _xenum_struct*)
_temp713;})); _LL708: if( Cyc_String_zstrptrcmp( _temp673, _temp711) == 0){ used=
1;} goto _LL704; _LL704:;}} if( used){ void* _temp718; void* _temp720; struct
Cyc_NewControlFlow_AbsSynOne _temp716= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple5*) _temp309->hd)).f2)); _LL721: _temp720=( void*)
_temp716.inner_exp; goto _LL719; _LL719: _temp718=( void*) _temp716.assigns;
goto _LL717; _LL717: assignexp= _temp718; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp720);} else{ void* _temp724; void* _temp726; struct Cyc_NewControlFlow_AbsSynOne
_temp722= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp309->hd)).f2)); _LL727: _temp726=( void*)
_temp722.inner_exp; goto _LL725; _LL725: _temp724=( void*) _temp722.assigns;
goto _LL723; _LL723: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp726);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL662:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp728=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp728=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp729=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 47; _temp730;})};( struct _xenum_struct*)
_temp728;})); _LL656:;} _LL162: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp315), 0); _LL164: return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp731=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp731[ 0]=({ struct Cyc_List_List
_temp732; _temp732.hd=( void*) _temp321; _temp732.tl= 0; _temp732;}); _temp731;}),
0); _LL166: { struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp323); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
esyn)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL168: { struct Cyc_List_List* _temp735; struct Cyc_NewControlFlow_AbsSynOne
_temp737; struct Cyc_NewControlFlow_AbsSynOne _temp739; struct Cyc_NewControlFlow_AbsSyn
_temp733= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp329); _LL740: _temp739=( struct Cyc_NewControlFlow_AbsSynOne) _temp733.when_true;
goto _LL738; _LL738: _temp737=( struct Cyc_NewControlFlow_AbsSynOne) _temp733.when_false;
goto _LL736; _LL736: _temp735=( struct Cyc_List_List*) _temp733.lvalues; goto
_LL734; _LL734: { struct Cyc_List_List* _temp743; struct Cyc_NewControlFlow_AbsSynOne
_temp745; struct Cyc_NewControlFlow_AbsSynOne _temp747; struct Cyc_NewControlFlow_AbsSyn
_temp741= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp327); _LL748: _temp747=( struct Cyc_NewControlFlow_AbsSynOne) _temp741.when_true;
goto _LL746; _LL746: _temp745=( struct Cyc_NewControlFlow_AbsSynOne) _temp741.when_false;
goto _LL744; _LL744: _temp743=( struct Cyc_List_List*) _temp741.lvalues; goto
_LL742; _LL742: { struct Cyc_List_List* _temp751; struct Cyc_NewControlFlow_AbsSynOne
_temp753; struct Cyc_NewControlFlow_AbsSynOne _temp755; struct Cyc_NewControlFlow_AbsSyn
_temp749= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp325); _LL756: _temp755=( struct Cyc_NewControlFlow_AbsSynOne) _temp749.when_true;
goto _LL754; _LL754: _temp753=( struct Cyc_NewControlFlow_AbsSynOne) _temp749.when_false;
goto _LL752; _LL752: _temp751=( struct Cyc_List_List*) _temp749.lvalues; goto
_LL750; _LL750: { void* true_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp739.inner_exp,( void*) _temp739.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp747.inner_exp,( void*) _temp747.assigns)),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp737.inner_exp,( void*) _temp737.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp755.inner_exp,( void*) _temp755.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp739.inner_exp,( void*) _temp739.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp745.inner_exp,( void*) _temp745.assigns)), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp737.inner_exp,( void*) _temp737.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp753.inner_exp,( void*) _temp753.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp757; _temp757.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp759;
_temp759.inner_exp=( void*) true_exp; _temp759.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp759;}); _temp757.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp758; _temp758.inner_exp=( void*) false_exp; _temp758.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp758;}); _temp757.lvalues= 0; _temp757;});}}}}
_LL170: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp331);{
struct Cyc_Absyn_Stmt* last_s= _temp331; while( 1) { void* _temp760=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp770; struct Cyc_Absyn_Stmt* _temp772;
struct Cyc_Absyn_Stmt* _temp774; struct Cyc_Absyn_Decl* _temp776; struct Cyc_Absyn_Exp*
_temp778; _LL762: if(( unsigned int) _temp760 > 1u?(( struct _enum_struct*)
_temp760)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL773: _temp772=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp760)->f1; goto _LL771; _LL771: _temp770=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp760)->f2; goto
_LL763;} else{ goto _LL764;} _LL764: if(( unsigned int) _temp760 > 1u?(( struct
_enum_struct*) _temp760)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL777: _temp776=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp760)->f1; goto
_LL775; _LL775: _temp774=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp760)->f2; goto _LL765;} else{ goto _LL766;} _LL766: if(( unsigned int)
_temp760 > 1u?(( struct _enum_struct*) _temp760)->tag == Cyc_Absyn_Exp_s_tag: 0){
_LL779: _temp778=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp760)->f1; goto _LL767;} else{ goto _LL768;} _LL768: goto _LL769; _LL763:
last_s= _temp770; continue; _LL765: last_s= _temp774; continue; _LL767: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp778); void* _temp782; void* _temp784; struct Cyc_NewControlFlow_AbsSynOne
_temp780= Cyc_NewControlFlow_meet_absexp( syn); _LL785: _temp784=( void*)
_temp780.inner_exp; goto _LL783; _LL783: _temp782=( void*) _temp780.assigns;
goto _LL781; _LL781:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp784, _temp782));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp331), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL769:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp786=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp786=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp787=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 33; _temp788;})};( struct _xenum_struct*)
_temp786;})); _LL761:;}}} _LL172: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp335); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp333); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp789; _temp789.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp791; _temp791.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp791.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp791;});
_temp789.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp790; _temp790.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp790.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp790;}); _temp789.lvalues= 0; _temp789;});}} _LL174: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp792=( char*)"new_control_flow.cyc:684 env.access_path==null";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 47; _temp793;})); Cyc_NewControlFlow_add_var_root(
env, _temp341);( env.shared)->comprehension_vars=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp794=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp794[ 0]=({ struct Cyc_List_List _temp795;
_temp795.hd=( void*) _temp341; _temp795.tl=( env.shared)->comprehension_vars;
_temp795;}); _temp794;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp798; void* _temp800; struct Cyc_NewControlFlow_AbsSynOne
_temp796= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp339)); _LL801: _temp800=( void*) _temp796.inner_exp; goto _LL799;
_LL799: _temp798=( void*) _temp796.assigns; goto _LL797; _LL797: { void*
_temp804; void* _temp806; struct Cyc_NewControlFlow_AbsSynOne _temp802= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp337)); _LL807: _temp806=( void*)
_temp802.inner_exp; goto _LL805; _LL805: _temp804=( void*) _temp802.assigns;
goto _LL803; _LL803: { void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp800, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp798, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp341),
Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, _temp806, _temp804), Cyc_CfAbsexp_mkSkipAE())))); return
Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()));}}} _LL176: goto _LL178; _LL178: goto _LL180;
_LL180: goto _LL182; _LL182: goto _LL184; _LL184: goto _LL186; _LL186: goto
_LL188; _LL188:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp808=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp808=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp809=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp810; _temp810.curr= _temp809; _temp810.base= _temp809;
_temp810.last_plus_one= _temp809 + 34; _temp810;})};( struct _xenum_struct*)
_temp808;})); _LL106:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp813; void* _temp815; struct Cyc_NewControlFlow_AbsSynOne
_temp811= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL816: _temp815=( void*) _temp811.inner_exp; goto _LL814; _LL814: _temp813=(
void*) _temp811.assigns; goto _LL812; _LL812: return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp815, _temp813);} struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp817; _temp817.f1= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns); _temp817.f2=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp817;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp820;
struct Cyc_Position_Segment* _temp821; struct Cyc_Absyn_Stmt* _temp823; struct
Cyc_Absyn_Exp* _temp825; struct Cyc_Core_Opt* _temp827; struct Cyc_Absyn_Pat*
_temp829; struct Cyc_Absyn_Switch_clause* _temp818=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp820=* _temp818; _LL830: _temp829=( struct Cyc_Absyn_Pat*) _temp820.pattern;
goto _LL828; _LL828: _temp827=( struct Cyc_Core_Opt*) _temp820.pat_vars; goto
_LL826; _LL826: _temp825=( struct Cyc_Absyn_Exp*) _temp820.where_clause; goto
_LL824; _LL824: _temp823=( struct Cyc_Absyn_Stmt*) _temp820.body; goto _LL822;
_LL822: _temp821=( struct Cyc_Position_Segment*) _temp820.loc; goto _LL819;
_LL819: if( _temp827 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp831=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp831=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp832=( char*)"switch clause vds not set";
struct _tagged_string _temp833; _temp833.curr= _temp832; _temp833.base= _temp832;
_temp833.last_plus_one= _temp832 + 26; _temp833;})};( struct _xenum_struct*)
_temp831;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*) _temp827->v;
for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root( env,( struct
Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp825 != 0){ void* _temp837; void*
_temp839; struct _tuple0 _temp835= Cyc_NewControlFlow_abstract_guard( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp834= _temp825; if( _temp834
== 0){ _throw( Null_Exception);} _temp834;})); _LL840: _temp839= _temp835.f1;
goto _LL838; _LL838: _temp837= _temp835.f2; goto _LL836; _LL836: ans=( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp841[ 0]=({ struct Cyc_List_List
_temp842; _temp842.hd=( void*) _temp839; _temp842.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp843=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp843[ 0]=({ struct Cyc_List_List _temp844;
_temp844.hd=( void*) _temp837; _temp844.tl= ans; _temp844;}); _temp843;});
_temp842;}); _temp841;});} Cyc_NewControlFlow_cf_prepass( env, _temp823);}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp845=( char*)"new_control_flow.cyc:736 env.lhsides == null";
struct _tagged_string _temp846; _temp846.curr= _temp845; _temp846.base= _temp845;
_temp846.last_plus_one= _temp845 + 45; _temp846;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp847=( char*)"new_control_flow.cyc:737 env.access_path == null";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 49; _temp848;}));{ void* _temp849=( void*) s->r;
struct Cyc_Absyn_Exp* _temp895; struct Cyc_Absyn_Stmt* _temp897; struct Cyc_Absyn_Stmt*
_temp899; struct Cyc_Absyn_Exp* _temp901; struct Cyc_Absyn_Exp* _temp903; struct
Cyc_Absyn_Stmt* _temp905; struct Cyc_Absyn_Stmt* _temp907; struct Cyc_Absyn_Exp*
_temp909; struct _tuple3 _temp911; struct Cyc_Absyn_Stmt* _temp913; struct Cyc_Absyn_Exp*
_temp915; struct Cyc_Absyn_Stmt* _temp917; struct Cyc_Absyn_Stmt* _temp919;
struct _tuple3 _temp921; struct Cyc_Absyn_Stmt* _temp923; struct Cyc_Absyn_Exp*
_temp925; struct Cyc_Absyn_Stmt* _temp927; struct Cyc_Absyn_Stmt* _temp929;
struct Cyc_Absyn_Stmt* _temp931; struct _tagged_string* _temp933; struct Cyc_Absyn_Stmt*
_temp935; struct _tagged_string* _temp937; struct Cyc_Absyn_Stmt* _temp939;
struct _tuple3 _temp941; struct Cyc_Absyn_Stmt* _temp943; struct Cyc_Absyn_Exp*
_temp945; struct _tuple3 _temp947; struct Cyc_Absyn_Stmt* _temp949; struct Cyc_Absyn_Exp*
_temp951; struct Cyc_Absyn_Exp* _temp953; struct Cyc_List_List* _temp955; struct
Cyc_Absyn_Exp* _temp957; struct Cyc_Absyn_Switch_clause** _temp959; struct Cyc_List_List*
_temp961; struct Cyc_Absyn_Stmt* _temp963; struct Cyc_Absyn_Decl* _temp965;
struct Cyc_Absyn_Decl _temp967; struct Cyc_Position_Segment* _temp968; void*
_temp970; struct Cyc_Absyn_Vardecl* _temp972; struct Cyc_Absyn_Stmt* _temp974;
struct Cyc_Absyn_Decl* _temp976; struct Cyc_Absyn_Decl _temp978; struct Cyc_Position_Segment*
_temp979; void* _temp981; int _temp983; struct Cyc_Absyn_Exp* _temp985; struct
Cyc_Core_Opt* _temp987; struct Cyc_Core_Opt* _temp989; struct Cyc_Core_Opt
_temp991; struct Cyc_List_List* _temp992; struct Cyc_Absyn_Pat* _temp994; struct
Cyc_Absyn_Stmt* _temp996; struct Cyc_Absyn_Decl* _temp998; struct Cyc_Absyn_Stmt*
_temp1000; struct Cyc_Absyn_Stmt* _temp1002; struct Cyc_Absyn_Stmt* _temp1004;
struct _tagged_string* _temp1006; struct Cyc_List_List* _temp1008; struct Cyc_Absyn_Stmt*
_temp1010; _LL851: if( _temp849 == Cyc_Absyn_Skip_s){ goto _LL852;} else{ goto
_LL853;} _LL853: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*)
_temp849)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL896: _temp895=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp849)->f1; goto _LL854;} else{ goto _LL855;}
_LL855: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL900: _temp899=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp849)->f1; goto _LL898; _LL898: _temp897=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp849)->f2; goto _LL856;}
else{ goto _LL857;} _LL857: if(( unsigned int) _temp849 > 1u?(( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL902: _temp901=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp849)->f1; if(
_temp901 == 0){ goto _LL858;} else{ goto _LL859;}} else{ goto _LL859;} _LL859:
if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL904: _temp903=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp849)->f1; goto _LL860;} else{ goto _LL861;} _LL861: if(( unsigned int)
_temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL910: _temp909=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp849)->f1; goto _LL908; _LL908: _temp907=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp849)->f2; goto _LL906; _LL906: _temp905=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp849)->f3;
goto _LL862;} else{ goto _LL863;} _LL863: if(( unsigned int) _temp849 > 1u?((
struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL918: _temp917=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp849)->f1; goto
_LL912; _LL912: _temp911=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp849)->f2; _LL916: _temp915= _temp911.f1; goto _LL914; _LL914: _temp913=
_temp911.f2; goto _LL864;} else{ goto _LL865;} _LL865: if(( unsigned int)
_temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_While_s_tag:
0){ _LL922: _temp921=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp849)->f1; _LL926: _temp925= _temp921.f1; goto _LL924; _LL924: _temp923=
_temp921.f2; goto _LL920; _LL920: _temp919=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp849)->f2; goto _LL866;} else{ goto _LL867;}
_LL867: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL928: _temp927=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp849)->f1; goto _LL868;} else{ goto _LL869;}
_LL869: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Continue_s_tag: 0){ _LL930: _temp929=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp849)->f1; goto _LL870;} else{ goto
_LL871;} _LL871: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*)
_temp849)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL934: _temp933=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp849)->f1; goto _LL932;
_LL932: _temp931=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp849)->f2; if( _temp931 == 0){ goto _LL872;} else{ goto _LL873;}} else{ goto
_LL873;} _LL873: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*)
_temp849)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL938: _temp937=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp849)->f1; goto _LL936;
_LL936: _temp935=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp849)->f2; goto _LL874;} else{ goto _LL875;} _LL875: if(( unsigned int)
_temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_For_s_tag: 0){
_LL954: _temp953=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp849)->f1; goto _LL948; _LL948: _temp947=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp849)->f2; _LL952: _temp951= _temp947.f1; goto _LL950; _LL950: _temp949=
_temp947.f2; goto _LL942; _LL942: _temp941=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp849)->f3; _LL946: _temp945= _temp941.f1; goto _LL944; _LL944: _temp943=
_temp941.f2; goto _LL940; _LL940: _temp939=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp849)->f4; goto _LL876;} else{ goto _LL877;} _LL877:
if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL958: _temp957=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp849)->f1; goto _LL956; _LL956: _temp955=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp849)->f2; goto _LL878;} else{ goto _LL879;}
_LL879: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL962: _temp961=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp849)->f1; goto _LL960; _LL960:
_temp959=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp849)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if(( unsigned int)
_temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL966: _temp965=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp849)->f1; _temp967=* _temp965; _LL971: _temp970=( void*) _temp967.r; if((
unsigned int) _temp970 > 1u?(( struct _enum_struct*) _temp970)->tag == Cyc_Absyn_Var_d_tag:
0){ _LL973: _temp972=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp970)->f1; goto _LL969;} else{ goto _LL883;} _LL969: _temp968=( struct Cyc_Position_Segment*)
_temp967.loc; goto _LL964; _LL964: _temp963=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp849)->f2; goto _LL882;} else{ goto _LL883;}
_LL883: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL977: _temp976=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp849)->f1; _temp978=* _temp976; _LL982: _temp981=(
void*) _temp978.r; if(( unsigned int) _temp981 > 1u?(( struct _enum_struct*)
_temp981)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL995: _temp994=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp981)->f1; goto _LL990; _LL990: _temp989=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp981)->f2; if(
_temp989 == 0){ goto _LL885;} else{ _temp991=* _temp989; _LL993: _temp992=(
struct Cyc_List_List*) _temp991.v; goto _LL988;} _LL988: _temp987=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp981)->f3; goto _LL986; _LL986: _temp985=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp981)->f4; goto
_LL984; _LL984: _temp983=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp981)->f5;
goto _LL980;} else{ goto _LL885;} _LL980: _temp979=( struct Cyc_Position_Segment*)
_temp978.loc; goto _LL975; _LL975: _temp974=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp849)->f2; goto _LL884;} else{ goto _LL885;}
_LL885: if(( unsigned int) _temp849 > 1u?(( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL999: _temp998=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp849)->f1; goto _LL997; _LL997: _temp996=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp849)->f2; goto _LL886;}
else{ goto _LL887;} _LL887: if(( unsigned int) _temp849 > 1u?(( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1001: _temp1000=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp849)->f1; goto
_LL888;} else{ goto _LL889;} _LL889: if(( unsigned int) _temp849 > 1u?(( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1003: _temp1002=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp849)->f1; goto
_LL890;} else{ goto _LL891;} _LL891: if(( unsigned int) _temp849 > 1u?(( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1007: _temp1006=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp849)->f1; goto
_LL1005; _LL1005: _temp1004=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp849)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if(( unsigned int)
_temp849 > 1u?(( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1011: _temp1010=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp849)->f1; goto _LL1009; _LL1009: _temp1008=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp849)->f2; goto _LL894;} else{ goto
_LL850;} _LL852: return; _LL854: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
env, _temp895); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1012=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1012[ 0]=({ struct Cyc_List_List _temp1013;
_temp1013.hd=( void*) ae; _temp1013.tl= 0; _temp1013;}); _temp1012;})); return;}
_LL856: Cyc_NewControlFlow_cf_prepass( env, _temp899); Cyc_NewControlFlow_cf_prepass(
env, _temp897); return; _LL858: return; _LL860: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1014= _temp903; if( _temp1014 == 0){ _throw( Null_Exception);} _temp1014;}));
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1015=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1015[ 0]=({ struct Cyc_List_List _temp1016; _temp1016.hd=( void*) ae;
_temp1016.tl= 0; _temp1016;}); _temp1015;})); return;} _LL862: { void* _temp1019;
void* _temp1021; struct _tuple0 _temp1017= Cyc_NewControlFlow_abstract_guard(
env, _temp909); _LL1022: _temp1021= _temp1017.f1; goto _LL1020; _LL1020:
_temp1019= _temp1017.f2; goto _LL1018; _LL1018: Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1023=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1023[ 0]=({ struct Cyc_List_List
_temp1024; _temp1024.hd=( void*) _temp1021; _temp1024.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1025=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1025[ 0]=({ struct Cyc_List_List _temp1026;
_temp1026.hd=( void*) _temp1019; _temp1026.tl= 0; _temp1026;}); _temp1025;});
_temp1024;}); _temp1023;})); Cyc_NewControlFlow_cf_prepass( env, _temp907); Cyc_NewControlFlow_cf_prepass(
env, _temp905); return;} _LL864: _temp925= _temp915; _temp923= _temp913;
_temp919= _temp917; goto _LL866; _LL866: { void* _temp1029; void* _temp1031;
struct _tuple0 _temp1027= Cyc_NewControlFlow_abstract_guard( env, _temp925);
_LL1032: _temp1031= _temp1027.f1; goto _LL1030; _LL1030: _temp1029= _temp1027.f2;
goto _LL1028; _LL1028: Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1033=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1033[ 0]=({ struct Cyc_List_List _temp1034;
_temp1034.hd=( void*) _temp1031; _temp1034.tl=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1035=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1035[ 0]=({ struct Cyc_List_List _temp1036; _temp1036.hd=( void*)
_temp1029; _temp1036.tl= 0; _temp1036;}); _temp1035;}); _temp1034;}); _temp1033;}));
Cyc_NewControlFlow_cf_prepass( env, _temp919); return;} _LL868: return; _LL870:
return; _LL872:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1037=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp1037=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp1038=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1039; _temp1039.curr= _temp1038; _temp1039.base=
_temp1038; _temp1039.last_plus_one= _temp1038 + 33; _temp1039;})};( struct
_xenum_struct*) _temp1037;})); _LL874: { struct Cyc_Absyn_Stmt* my_encloser=(
Cyc_NewControlFlow_get_stmt_annot( s))->encloser; struct Cyc_Absyn_Stmt*
dest_encloser=( Cyc_NewControlFlow_get_stmt_annot(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Absyn_Stmt* _temp1040= _temp935; if( _temp1040 == 0){ _throw(
Null_Exception);} _temp1040;})))->encloser; while( dest_encloser != my_encloser) {
struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1041=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1042; _temp1042.curr= _temp1041; _temp1042.base=
_temp1041; _temp1042.last_plus_one= _temp1041 + 45; _temp1042;})); break;}
my_encloser= next_encloser;} return;} _LL876: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp953); void* _temp1045; void* _temp1047; struct _tuple0 _temp1043= Cyc_NewControlFlow_abstract_guard(
env, _temp951); _LL1048: _temp1047= _temp1043.f1; goto _LL1046; _LL1046:
_temp1045= _temp1043.f2; goto _LL1044; _LL1044: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp945); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1049=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1049[ 0]=({ struct Cyc_List_List _temp1050;
_temp1050.hd=( void*) ae1; _temp1050.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1051=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1051[ 0]=({ struct Cyc_List_List _temp1052; _temp1052.hd=( void*) _temp1047;
_temp1052.tl=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1053=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1053[ 0]=({
struct Cyc_List_List _temp1054; _temp1054.hd=( void*) _temp1045; _temp1054.tl=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1055=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1055[ 0]=({ struct Cyc_List_List
_temp1056; _temp1056.hd=( void*) ae3; _temp1056.tl= 0; _temp1056;}); _temp1055;});
_temp1054;}); _temp1053;}); _temp1052;}); _temp1051;}); _temp1050;}); _temp1049;}));
Cyc_NewControlFlow_cf_prepass( env, _temp939); return;}} _LL878: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp957); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp955); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1057=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1057[ 0]=({ struct Cyc_List_List _temp1058;
_temp1058.hd=( void*) ae; _temp1058.tl= ael; _temp1058;}); _temp1057;}));
return;} _LL880: { void* _temp1061; void* _temp1063; struct Cyc_NewControlFlow_AbsSynOne
_temp1059= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp961, 0)); _LL1064: _temp1063=( void*) _temp1059.inner_exp; goto
_LL1062; _LL1062: _temp1061=( void*) _temp1059.assigns; goto _LL1060; _LL1060: {
void* ae= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp1063, _temp1061);
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1065=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1065[ 0]=({ struct Cyc_List_List _temp1066; _temp1066.hd=( void*) ae;
_temp1066.tl= 0; _temp1066;}); _temp1065;})); return;}} _LL882: Cyc_NewControlFlow_add_var_root(
env, _temp972); if( _temp972->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1067[ 0]=({ struct Cyc_List_List _temp1068; _temp1068.hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp972); _temp1068.tl= 0; _temp1068;}); _temp1067;})); void* ae= Cyc_NewControlFlow_abstract_exp_top(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1069= _temp972->initializer;
if( _temp1069 == 0){ _throw( Null_Exception);} _temp1069;})); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1070=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1070[ 0]=({ struct Cyc_List_List
_temp1071; _temp1071.hd=( void*) ae; _temp1071.tl= 0; _temp1071;}); _temp1070;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp963); return; _LL884: for( 0; _temp992
!= 0; _temp992= _temp992->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)
_temp992->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp985); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1072=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1072[ 0]=({ struct Cyc_List_List _temp1073;
_temp1073.hd=( void*) ae; _temp1073.tl= 0; _temp1073;}); _temp1072;})); Cyc_NewControlFlow_cf_prepass(
env, _temp974); return;} _LL886: goto _LL888; _LL888: goto _LL890; _LL890:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp1074=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp1074=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp1075=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1076; _temp1076.curr= _temp1075; _temp1076.base=
_temp1075; _temp1076.last_plus_one= _temp1075 + 26; _temp1076;})};( struct
_xenum_struct*) _temp1074;})); _LL892: Cyc_NewControlFlow_cf_prepass( env,
_temp1004); return; _LL894: Cyc_NewControlFlow_cf_prepass( env, _temp1010);{
struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1008);
Cyc_NewControlFlow_cf_set_absexps( s, ael); return;} _LL850:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1077=( char*)"new_control_flow.cyc:865 annot->visited == iteration_num";
struct _tagged_string _temp1078; _temp1078.curr= _temp1077; _temp1078.base=
_temp1077; _temp1078.last_plus_one= _temp1077 + 57; _temp1078;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1079= flow; struct Cyc_Dict_Dict* _temp1085; _LL1081: if( _temp1079
== Cyc_CfFlowInfo_BottomFL){ goto _LL1082;} else{ goto _LL1083;} _LL1083: if((
unsigned int) _temp1079 > 1u?(( struct _enum_struct*) _temp1079)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1086: _temp1085=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1079)->f1; goto _LL1084;} else{ goto _LL1080;} _LL1082: return Cyc_CfFlowInfo_BottomFL;
_LL1084: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1087=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1087->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1087->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1087;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots, root)); _temp1085=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1085, root, pinfo);} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1088=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1088->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1088->f1= _temp1085;( void*) _temp1088;}); _LL1080:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1089= flow; struct Cyc_Dict_Dict* _temp1095; _LL1091: if( _temp1089
== Cyc_CfFlowInfo_BottomFL){ goto _LL1092;} else{ goto _LL1093;} _LL1093: if((
unsigned int) _temp1089 > 1u?(( struct _enum_struct*) _temp1089)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1096: _temp1095=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1089)->f1; goto _LL1094;} else{ goto _LL1090;} _LL1092: return Cyc_CfFlowInfo_BottomFL;
_LL1094: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1097=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1097->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1097->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1097;}); _temp1095=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1095, root,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots, root));} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1098=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1098->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1098->f1= _temp1095;( void*) _temp1098;}); _LL1090:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)
aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1099; _temp1099.f1=
out_flow; _temp1099.f2= out_flow; _temp1099;});} return({ struct _tuple0
_temp1100; _temp1100.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1100.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1100;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1103; struct Cyc_Position_Segment* _temp1104; struct Cyc_Absyn_Stmt*
_temp1106; struct Cyc_Absyn_Exp* _temp1108; struct Cyc_Core_Opt* _temp1110;
struct Cyc_Absyn_Pat* _temp1112; struct Cyc_Absyn_Switch_clause* _temp1101=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1103=* _temp1101; _LL1113:
_temp1112=( struct Cyc_Absyn_Pat*) _temp1103.pattern; goto _LL1111; _LL1111:
_temp1110=( struct Cyc_Core_Opt*) _temp1103.pat_vars; goto _LL1109; _LL1109:
_temp1108=( struct Cyc_Absyn_Exp*) _temp1103.where_clause; goto _LL1107; _LL1107:
_temp1106=( struct Cyc_Absyn_Stmt*) _temp1103.body; goto _LL1105; _LL1105:
_temp1104=( struct Cyc_Position_Segment*) _temp1103.loc; goto _LL1102; _LL1102: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1110->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1110->v);{ void* body_outflow;
if( _temp1108 != 0){ void* _temp1117; void* _temp1119; struct _tuple0 _temp1115=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1114= _temp1108; if( _temp1114 == 0){ _throw( Null_Exception);} _temp1114;}))->loc,
aes, body_inflow); _LL1120: _temp1119= _temp1115.f1; goto _LL1118; _LL1118:
_temp1117= _temp1115.f2; goto _LL1116; _LL1116: aes=( aes->tl)->tl; in_flow=
_temp1117; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1106,
_temp1119);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1106, body_inflow);}{ void* _temp1121= body_outflow; _LL1123: if( _temp1121
== Cyc_CfFlowInfo_BottomFL){ goto _LL1124;} else{ goto _LL1125;} _LL1125: goto
_LL1126; _LL1124: goto _LL1122; _LL1126: Cyc_Tcutil_terr( _temp1106->loc,(
struct _tagged_string)({ char* _temp1127=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1128; _temp1128.curr= _temp1127; _temp1128.base=
_temp1127; _temp1128.last_plus_one= _temp1127 + 38; _temp1128;})); goto _LL1122;
_LL1122:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1129=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1171; struct Cyc_Absyn_Exp* _temp1173; struct Cyc_Absyn_Exp*
_temp1175; struct Cyc_Absyn_Stmt* _temp1177; struct Cyc_Absyn_Stmt* _temp1179;
struct Cyc_Absyn_Stmt* _temp1181; struct Cyc_Absyn_Stmt* _temp1183; struct Cyc_Absyn_Exp*
_temp1185; struct Cyc_Absyn_Stmt* _temp1187; struct _tuple3 _temp1189; struct
Cyc_Absyn_Stmt* _temp1191; struct Cyc_Absyn_Exp* _temp1193; struct _tuple3
_temp1195; struct Cyc_Absyn_Stmt* _temp1197; struct Cyc_Absyn_Exp* _temp1199;
struct Cyc_Absyn_Stmt* _temp1201; struct Cyc_Absyn_Stmt* _temp1203; struct
_tuple3 _temp1205; struct Cyc_Absyn_Stmt* _temp1207; struct Cyc_Absyn_Exp*
_temp1209; struct _tuple3 _temp1211; struct Cyc_Absyn_Stmt* _temp1213; struct
Cyc_Absyn_Exp* _temp1215; struct Cyc_Absyn_Exp* _temp1217; struct Cyc_Absyn_Stmt*
_temp1219; struct Cyc_Absyn_Switch_clause** _temp1221; struct Cyc_Absyn_Switch_clause*
_temp1223; struct Cyc_List_List* _temp1224; struct Cyc_Absyn_Stmt* _temp1226;
struct Cyc_Absyn_Stmt* _temp1228; struct Cyc_Absyn_Stmt* _temp1230; struct
_tagged_string* _temp1232; struct Cyc_List_List* _temp1234; struct Cyc_Absyn_Exp*
_temp1236; struct Cyc_List_List* _temp1238; struct Cyc_Absyn_Stmt* _temp1240;
struct Cyc_Absyn_Stmt* _temp1242; struct Cyc_Absyn_Decl* _temp1244; struct Cyc_Absyn_Decl
_temp1246; struct Cyc_Position_Segment* _temp1247; void* _temp1249; struct Cyc_Absyn_Vardecl*
_temp1251; struct Cyc_Absyn_Stmt* _temp1253; struct Cyc_Absyn_Decl* _temp1255;
struct Cyc_Absyn_Decl _temp1257; struct Cyc_Position_Segment* _temp1258; void*
_temp1260; int _temp1262; struct Cyc_Absyn_Exp* _temp1264; struct Cyc_Core_Opt*
_temp1266; struct Cyc_Core_Opt* _temp1268; struct Cyc_Core_Opt _temp1270; struct
Cyc_List_List* _temp1271; struct Cyc_Absyn_Pat* _temp1273; struct Cyc_Absyn_Stmt*
_temp1275; struct _tagged_string* _temp1277; _LL1131: if( _temp1129 == Cyc_Absyn_Skip_s){
goto _LL1132;} else{ goto _LL1133;} _LL1133: if(( unsigned int) _temp1129 > 1u?((
struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1172:
_temp1171=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1129)->f1; if( _temp1171 == 0){ goto _LL1134;} else{ goto _LL1135;}} else{
goto _LL1135;} _LL1135: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1174: _temp1173=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1129)->f1; goto _LL1136;} else{ goto
_LL1137;} _LL1137: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1176: _temp1175=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp1129)->f1; goto _LL1138;} else{ goto
_LL1139;} _LL1139: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1180: _temp1179=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1129)->f1; goto _LL1178; _LL1178: _temp1177=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1129)->f2; goto
_LL1140;} else{ goto _LL1141;} _LL1141: if(( unsigned int) _temp1129 > 1u?((
struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1186:
_temp1185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1129)->f1; goto _LL1184; _LL1184: _temp1183=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1129)->f2; goto _LL1182; _LL1182:
_temp1181=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1129)->f3; goto _LL1142;} else{ goto _LL1143;} _LL1143: if(( unsigned int)
_temp1129 > 1u?(( struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1190: _temp1189=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1129)->f1; _LL1194: _temp1193= _temp1189.f1; goto _LL1192; _LL1192:
_temp1191= _temp1189.f2; goto _LL1188; _LL1188: _temp1187=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1129)->f2; goto _LL1144;} else{ goto
_LL1145;} _LL1145: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1202: _temp1201=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1129)->f1; goto _LL1196; _LL1196: _temp1195=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1129)->f2; _LL1200:
_temp1199= _temp1195.f1; goto _LL1198; _LL1198: _temp1197= _temp1195.f2; goto
_LL1146;} else{ goto _LL1147;} _LL1147: if(( unsigned int) _temp1129 > 1u?((
struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1218:
_temp1217=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1129)->f1;
goto _LL1212; _LL1212: _temp1211=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1129)->f2; _LL1216: _temp1215= _temp1211.f1; goto _LL1214; _LL1214:
_temp1213= _temp1211.f2; goto _LL1206; _LL1206: _temp1205=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1129)->f3; _LL1210: _temp1209= _temp1205.f1;
goto _LL1208; _LL1208: _temp1207= _temp1205.f2; goto _LL1204; _LL1204: _temp1203=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1129)->f4; goto
_LL1148;} else{ goto _LL1149;} _LL1149: if(( unsigned int) _temp1129 > 1u?((
struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1220:
_temp1219=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1129)->f1; if( _temp1219 == 0){ goto _LL1150;} else{ goto _LL1151;}} else{
goto _LL1151;} _LL1151: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1225: _temp1224=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1129)->f1; goto _LL1222; _LL1222:
_temp1221=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1129)->f2; if( _temp1221 == 0){ goto _LL1153;} else{ _temp1223=* _temp1221;
goto _LL1152;}} else{ goto _LL1153;} _LL1153: if(( unsigned int) _temp1129 > 1u?((
struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1227:
_temp1226=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1129)->f1; goto _LL1154;} else{ goto _LL1155;} _LL1155: if(( unsigned int)
_temp1129 > 1u?(( struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1229: _temp1228=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1129)->f1; goto _LL1156;} else{ goto _LL1157;} _LL1157: if(( unsigned int)
_temp1129 > 1u?(( struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1233: _temp1232=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1129)->f1; goto _LL1231; _LL1231: _temp1230=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1129)->f2; goto _LL1158;} else{ goto
_LL1159;} _LL1159: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1237: _temp1236=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1129)->f1; goto _LL1235; _LL1235:
_temp1234=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1129)->f2; goto _LL1160;} else{ goto _LL1161;} _LL1161: if(( unsigned int)
_temp1129 > 1u?(( struct _enum_struct*) _temp1129)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1241: _temp1240=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1129)->f1; goto _LL1239; _LL1239: _temp1238=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1129)->f2; goto _LL1162;} else{ goto
_LL1163;} _LL1163: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1245: _temp1244=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1129)->f1; _temp1246=* _temp1244; _LL1250:
_temp1249=( void*) _temp1246.r; if(( unsigned int) _temp1249 > 1u?(( struct
_enum_struct*) _temp1249)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL1252: _temp1251=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp1249)->f1;
goto _LL1248;} else{ goto _LL1165;} _LL1248: _temp1247=( struct Cyc_Position_Segment*)
_temp1246.loc; goto _LL1243; _LL1243: _temp1242=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1129)->f2; goto _LL1164;} else{ goto
_LL1165;} _LL1165: if(( unsigned int) _temp1129 > 1u?(( struct _enum_struct*)
_temp1129)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1256: _temp1255=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1129)->f1; _temp1257=* _temp1255; _LL1261:
_temp1260=( void*) _temp1257.r; if(( unsigned int) _temp1260 > 1u?(( struct
_enum_struct*) _temp1260)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1274: _temp1273=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1260)->f1; goto
_LL1269; _LL1269: _temp1268=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1260)->f2; if( _temp1268 == 0){ goto _LL1167;} else{ _temp1270=* _temp1268;
_LL1272: _temp1271=( struct Cyc_List_List*) _temp1270.v; goto _LL1267;} _LL1267:
_temp1266=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1260)->f3;
goto _LL1265; _LL1265: _temp1264=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1260)->f4; goto _LL1263; _LL1263: _temp1262=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1260)->f5; goto _LL1259;} else{ goto _LL1167;} _LL1259: _temp1258=( struct
Cyc_Position_Segment*) _temp1257.loc; goto _LL1254; _LL1254: _temp1253=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1129)->f2; goto _LL1166;}
else{ goto _LL1167;} _LL1167: if(( unsigned int) _temp1129 > 1u?(( struct
_enum_struct*) _temp1129)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1278: _temp1277=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1129)->f1; goto
_LL1276; _LL1276: _temp1275=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1129)->f2; goto _LL1168;} else{ goto _LL1169;} _LL1169: goto _LL1170;
_LL1132: return in_flow; _LL1134: return Cyc_CfFlowInfo_BottomFL; _LL1136: Cyc_CfAbsexp_eval_absexp(
env, _temp1173->loc,( void*) aes->hd, in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1138: return Cyc_CfAbsexp_eval_absexp( env, _temp1175->loc,( void*) aes->hd,
in_flow); _LL1140: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1177,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1179, in_flow)); _LL1142: { void*
_temp1281; void* _temp1283; struct _tuple0 _temp1279= Cyc_NewControlFlow_cf_evalguard(
env, _temp1185->loc, aes, in_flow); _LL1284: _temp1283= _temp1279.f1; goto
_LL1282; _LL1282: _temp1281= _temp1279.f2; goto _LL1280; _LL1280: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1183, _temp1283), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1181, _temp1281));} _LL1144: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1191))->flow; void* _temp1287; void* _temp1289; struct _tuple0
_temp1285= Cyc_NewControlFlow_cf_evalguard( env, _temp1193->loc, aes, e_inflow);
_LL1290: _temp1289= _temp1285.f1; goto _LL1288; _LL1288: _temp1287= _temp1285.f2;
goto _LL1286; _LL1286: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1187, _temp1289); Cyc_NewControlFlow_update_flow( _temp1191,
body_outflow); return _temp1287;}} _LL1146: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1201, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1197))->flow; void* _temp1293; void* _temp1295; struct
_tuple0 _temp1291= Cyc_NewControlFlow_cf_evalguard( env, _temp1199->loc, aes,
e_inflow); _LL1296: _temp1295= _temp1291.f1; goto _LL1294; _LL1294: _temp1293=
_temp1291.f2; goto _LL1292; _LL1292: Cyc_NewControlFlow_update_flow( _temp1201,
_temp1295); return _temp1293;} _LL1148: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1217->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1213))->flow; void* _temp1299; void* _temp1301; struct _tuple0
_temp1297= Cyc_NewControlFlow_cf_evalguard( env, _temp1215->loc, aes->tl,
e2_inflow); _LL1302: _temp1301= _temp1297.f1; goto _LL1300; _LL1300: _temp1299=
_temp1297.f2; goto _LL1298; _LL1298: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1203, _temp1301); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1207))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1209->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1213, e3_outflow); return _temp1299;}} _LL1150: return Cyc_CfFlowInfo_BottomFL;
_LL1152: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1223->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1226=( struct
Cyc_Absyn_Stmt*) _temp1223->body; goto _LL1154;} _LL1154: _temp1228= _temp1226;
goto _LL1156; _LL1156: _temp1230= _temp1228; goto _LL1158; _LL1158: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1303= _temp1230; if(
_temp1303 == 0){ _throw( Null_Exception);} _temp1303;}), in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1160: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1236->loc,( void*) aes->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1234, aes->tl,
in_flow); return Cyc_CfFlowInfo_BottomFL; _LL1162: { struct Cyc_NewControlFlow_AnalEnv
_temp1306; void* _temp1307; int _temp1309; struct Cyc_Dict_Dict* _temp1311;
struct Cyc_NewControlFlow_AnalEnv* _temp1304= env; _temp1306=* _temp1304;
_LL1312: _temp1311=( struct Cyc_Dict_Dict*) _temp1306.roots; goto _LL1310;
_LL1310: _temp1309=( int) _temp1306.in_try; goto _LL1308; _LL1308: _temp1307=(
void*) _temp1306.tryflow; goto _LL1305; _LL1305: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1240, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1309;( void*)( env->tryflow=( void*) _temp1307); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1238,
aes, scs_inflow); return s_outflow;}} _LL1164: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1313; _temp1313.hd=( void*) _temp1251; _temp1313.tl= 0;
_temp1313;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1251->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1314= e; if( _temp1314 == 0){ _throw( Null_Exception);}
_temp1314;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1242, body_inflow);} _LL1166: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1271); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1264->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1271); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1253, body_inflow);} _LL1168: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1275, in_flow); _LL1170:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1315=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp1315=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp1316=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1317; _temp1317.curr= _temp1316; _temp1317.base=
_temp1316; _temp1317.last_plus_one= _temp1316 + 62; _temp1317;})};( struct
_xenum_struct*) _temp1315;})); _LL1130:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1318=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_EmptyAnnot_struct));* _temp1318=( struct Cyc_Absyn_EmptyAnnot_struct){.tag=
Cyc_Absyn_EmptyAnnot_tag};( struct _xenum_struct*) _temp1318;});{ void*
_temp1319=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1341; struct Cyc_Absyn_Stmt*
_temp1343; struct Cyc_Absyn_Stmt* _temp1345; struct Cyc_Absyn_Stmt* _temp1347;
struct Cyc_Absyn_Exp* _temp1349; struct Cyc_Absyn_Stmt* _temp1351; struct
_tuple3 _temp1353; struct Cyc_Absyn_Stmt* _temp1355; struct Cyc_Absyn_Exp*
_temp1357; struct _tuple3 _temp1359; struct Cyc_Absyn_Stmt* _temp1361; struct
Cyc_Absyn_Exp* _temp1363; struct Cyc_Absyn_Exp* _temp1365; struct Cyc_Absyn_Stmt*
_temp1367; struct _tuple3 _temp1369; struct Cyc_Absyn_Stmt* _temp1371; struct
Cyc_Absyn_Exp* _temp1373; struct _tuple3 _temp1375; struct Cyc_Absyn_Stmt*
_temp1377; struct Cyc_Absyn_Exp* _temp1379; struct Cyc_Absyn_Stmt* _temp1381;
struct Cyc_Absyn_Stmt* _temp1383; struct _tagged_string* _temp1385; struct Cyc_Absyn_Stmt*
_temp1387; struct Cyc_Absyn_Decl* _temp1389; struct Cyc_List_List* _temp1391;
struct Cyc_Absyn_Stmt* _temp1393; struct Cyc_List_List* _temp1395; struct Cyc_Absyn_Exp*
_temp1397; _LL1321: if(( unsigned int) _temp1319 > 1u?(( struct _enum_struct*)
_temp1319)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1344: _temp1343=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1319)->f1; goto _LL1342; _LL1342: _temp1341=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1319)->f2; goto
_LL1322;} else{ goto _LL1323;} _LL1323: if(( unsigned int) _temp1319 > 1u?((
struct _enum_struct*) _temp1319)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1350:
_temp1349=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1319)->f1; goto _LL1348; _LL1348: _temp1347=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1319)->f2; goto _LL1346; _LL1346:
_temp1345=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1319)->f3; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(( unsigned int)
_temp1319 > 1u?(( struct _enum_struct*) _temp1319)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1366: _temp1365=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1319)->f1; goto _LL1360; _LL1360: _temp1359=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1319)->f2; _LL1364: _temp1363= _temp1359.f1; goto _LL1362; _LL1362:
_temp1361= _temp1359.f2; goto _LL1354; _LL1354: _temp1353=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1319)->f3; _LL1358: _temp1357= _temp1353.f1;
goto _LL1356; _LL1356: _temp1355= _temp1353.f2; goto _LL1352; _LL1352: _temp1351=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1319)->f4; goto
_LL1326;} else{ goto _LL1327;} _LL1327: if(( unsigned int) _temp1319 > 1u?((
struct _enum_struct*) _temp1319)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1370:
_temp1369=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1319)->f1;
_LL1374: _temp1373= _temp1369.f1; goto _LL1372; _LL1372: _temp1371= _temp1369.f2;
goto _LL1368; _LL1368: _temp1367=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1319)->f2; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(( unsigned int)
_temp1319 > 1u?(( struct _enum_struct*) _temp1319)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1382: _temp1381=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1319)->f1; goto _LL1376; _LL1376: _temp1375=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1319)->f2; _LL1380: _temp1379= _temp1375.f1; goto _LL1378; _LL1378:
_temp1377= _temp1375.f2; goto _LL1330;} else{ goto _LL1331;} _LL1331: if((
unsigned int) _temp1319 > 1u?(( struct _enum_struct*) _temp1319)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1386: _temp1385=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1319)->f1; goto _LL1384; _LL1384: _temp1383=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1319)->f2; goto _LL1332;} else{ goto
_LL1333;} _LL1333: if(( unsigned int) _temp1319 > 1u?(( struct _enum_struct*)
_temp1319)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1390: _temp1389=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1319)->f1; goto _LL1388; _LL1388:
_temp1387=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1319)->f2;
goto _LL1334;} else{ goto _LL1335;} _LL1335: if(( unsigned int) _temp1319 > 1u?((
struct _enum_struct*) _temp1319)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1394:
_temp1393=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1319)->f1; goto _LL1392; _LL1392: _temp1391=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1319)->f2; goto _LL1336;} else{ goto
_LL1337;} _LL1337: if(( unsigned int) _temp1319 > 1u?(( struct _enum_struct*)
_temp1319)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1398: _temp1397=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1319)->f1; goto _LL1396; _LL1396:
_temp1395=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1319)->f2; goto _LL1338;} else{ goto _LL1339;} _LL1339: goto _LL1340;
_LL1322: _temp1347= _temp1343; _temp1345= _temp1341; goto _LL1324; _LL1324: Cyc_NewControlFlow_cf_postpass(
_temp1347); Cyc_NewControlFlow_cf_postpass( _temp1345); return; _LL1326: Cyc_NewControlFlow_cf_postpass(
_temp1355); _temp1371= _temp1351; _temp1367= _temp1361; goto _LL1328; _LL1328:
_temp1381= _temp1367; _temp1377= _temp1371; goto _LL1330; _LL1330: Cyc_NewControlFlow_cf_postpass(
_temp1377); _temp1383= _temp1381; goto _LL1332; _LL1332: _temp1387= _temp1383;
goto _LL1334; _LL1334: Cyc_NewControlFlow_cf_postpass( _temp1387); return;
_LL1336: Cyc_NewControlFlow_cf_postpass( _temp1393); _temp1395= _temp1391; goto
_LL1338; _LL1338: for( 0; _temp1395 != 0; _temp1395= _temp1395->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1395->hd)->body);} return; _LL1340:
return; _LL1320:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1399; _temp1399.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1400=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv)
* 1); _temp1400[ 0]=({ struct Cyc_NewControlFlow_SharedAbsEnv _temp1401;
_temp1401.all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1401.comprehension_vars= 0;
_temp1401;}); _temp1400;}); _temp1399.lhsides= 0; _temp1399.access_path= 0;
_temp1399;}); struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1404; struct Cyc_List_List*
_temp1406; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1408; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1410; struct Cyc_List_List* _temp1411; struct Cyc_Dict_Dict* _temp1413;
struct Cyc_NewControlFlow_AbsEnv _temp1402= absenv; _LL1409: _temp1408=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1402.shared; _temp1410=* _temp1408;
_LL1414: _temp1413=( struct Cyc_Dict_Dict*) _temp1410.all_roots; goto _LL1412;
_LL1412: _temp1411=( struct Cyc_List_List*) _temp1410.comprehension_vars; goto
_LL1407; _LL1407: _temp1406=( struct Cyc_List_List*) _temp1402.lhsides; goto
_LL1405; _LL1405: _temp1404=( struct Cyc_List_List*) _temp1402.access_path; goto
_LL1403; _LL1403: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1415=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)
* 1); _temp1415[ 0]=({ struct Cyc_NewControlFlow_AnalEnv _temp1416; _temp1416.roots=
_temp1413; _temp1416.in_try= 0; _temp1416.tryflow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1416;}); _temp1415;}); void* in_flow=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1417=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1417->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp1417->f1=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);(
void*) _temp1417;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( analenv,
in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow( analenv,
in_flow, _temp1411);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1418= out_flow; _LL1420: if( _temp1418 == Cyc_CfFlowInfo_BottomFL){
goto _LL1421;} else{ goto _LL1422;} _LL1422: goto _LL1423; _LL1421: goto _LL1419;
_LL1423: if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp1424=( char*)"function may complete without returning a value";
struct _tagged_string _temp1425; _temp1425.curr= _temp1424; _temp1425.base=
_temp1424; _temp1425.last_plus_one= _temp1424 + 48; _temp1425;}));} goto _LL1419;
_LL1419:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1426=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1436; struct
Cyc_List_List* _temp1438; struct _tuple1* _temp1440; struct Cyc_List_List*
_temp1442; struct _tagged_string* _temp1444; _LL1428: if(( unsigned int)
_temp1426 > 1u?(( struct _enum_struct*) _temp1426)->tag == Cyc_Absyn_Fn_d_tag: 0){
_LL1437: _temp1436=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1426)->f1; goto _LL1429;} else{ goto _LL1430;} _LL1430: if(( unsigned int)
_temp1426 > 1u?(( struct _enum_struct*) _temp1426)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1441: _temp1440=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1426)->f1; goto _LL1439; _LL1439: _temp1438=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1426)->f2; goto _LL1431;} else{ goto
_LL1432;} _LL1432: if(( unsigned int) _temp1426 > 1u?(( struct _enum_struct*)
_temp1426)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1445: _temp1444=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1426)->f1; goto
_LL1443; _LL1443: _temp1442=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1426)->f2; goto _LL1433;} else{ goto _LL1434;} _LL1434: goto _LL1435;
_LL1429: Cyc_NewControlFlow_cf_check_fun( _temp1436); goto _LL1427; _LL1431:
_temp1442= _temp1438; goto _LL1433; _LL1433: Cyc_NewControlFlow_cf_check(
_temp1442); goto _LL1427; _LL1435: goto _LL1427; _LL1427:;}}