#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple4{ struct Cyc_Absyn_Switch_clause*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple5{ void*
f1; int f2; } ; struct _tuple6{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; struct Cyc_Dict_Absent_struct{ char* tag; } ; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct
Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key); struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_SlowDict_Dict; typedef struct Cyc_SlowDict_Dict*
Cyc_SlowDict_hdict_t; typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_dict_t;
extern char Cyc_SlowDict_Present_tag[ 8u]; extern char Cyc_SlowDict_Absent_tag[
7u]; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* comp)( void*,
void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d); extern
struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void*
key, void* data); extern struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct
Cyc_SlowDict_Dict* d, void* key); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete(
struct Cyc_SlowDict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; typedef struct _tagged_string* Cyc_Absyn_field_name;
typedef struct _tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
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
; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Xenumdecl*
Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_exn_typ; extern struct _tagged_string
Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern struct
Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern void* Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct
_tuple3* pat_typ, struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv* te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern
void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct _tagged_string*, struct Cyc_Absyn_Stmt**); extern struct _tuple4* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv*); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; }
; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern
int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void* e2); extern int
Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); char Cyc_Tcenv_Env_error_tag[ 10u]="Env_error";
struct Cyc_Tcenv_Env_error_struct{ char* tag; } ; void* Cyc_Tcenv_env_err(
struct _tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Tcenv_Env_error_struct*
_temp1=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcenv_Env_error_struct)); _temp1->tag= Cyc_Tcenv_Env_error_tag;( struct
_xenum_struct*) _temp1;}));} struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv; const
int Cyc_Tcenv_VarRes_tag= 0; const int Cyc_Tcenv_StructRes_tag= 1; const int Cyc_Tcenv_EnumRes_tag=
2; const int Cyc_Tcenv_XenumRes_tag= 3; const int Cyc_Tcenv_Outermost_tag= 0;
const int Cyc_Tcenv_Frame_tag= 1; const int Cyc_Tcenv_Hidden_tag= 2; void* Cyc_Tcenv_NotLoop_j=(
void*) 0; void* Cyc_Tcenv_CaseEnd_j=( void*) 1; void* Cyc_Tcenv_FnEnd_j=( void*)
2; const int Cyc_Tcenv_Stmt_j_tag= 0; struct Cyc_Tcenv_CtrlEnv{ void*
continue_stmt; void* break_stmt; struct _tuple4* fallthru_clause; void*
next_stmt; int try_depth; } ; typedef struct Cyc_Tcenv_CtrlEnv* Cyc_Tcenv_ctrl_env_t;
struct Cyc_Tcenv_SharedFenv{ void* return_typ; struct Cyc_Dict_Dict* seen_labels;
struct Cyc_SlowDict_Dict* needed_labels; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv*
shared; struct Cyc_List_List* type_vars; struct Cyc_Dict_Dict* locals; struct
Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env; void* capability;
void* curr_rgn; } ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp2;});{ struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
top_ns= 0; Cyc_Absyn_exn_xed=({ struct Cyc_Absyn_Xenumdecl* _temp3=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl) * 1); _temp3[ 0]=({ struct Cyc_Absyn_Xenumdecl
_temp4; _temp4.sc=( void*) Cyc_Absyn_Extern; _temp4.name= Cyc_Absyn_exn_name;
_temp4.fields= 0; _temp4;}); _temp3;}); Cyc_Absyn_exn_typ=({ struct Cyc_Absyn_XenumType_struct*
_temp5=( struct Cyc_Absyn_XenumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct));
_temp5->tag= Cyc_Absyn_XenumType_tag; _temp5->f1= Cyc_Absyn_exn_name; _temp5->f2=(
struct Cyc_Absyn_Xenumdecl**)& Cyc_Absyn_exn_xed;( void*) _temp5;}); ge->xenumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Xenumdecl** data)) Cyc_Dict_insert)( ge->xenumdecls,(* Cyc_Absyn_exn_name).f2,&
Cyc_Absyn_exn_xed);{ struct Cyc_Dict_Dict* ae=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), top_ns, ge);
return({ struct Cyc_Tcenv_Tenv* _temp6=( struct Cyc_Tcenv_Tenv*) GC_malloc(
sizeof( struct Cyc_Tcenv_Tenv)); _temp6->ns= top_ns; _temp6->ae= ae; _temp6->le=
0; _temp6;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp7=( char*)"Tcenv::outer_namespace"; struct _tagged_string _temp8; _temp8.curr=
_temp7; _temp8.base= _temp7; _temp8.last_plus_one= _temp7 + 23; _temp8;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)((((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( ns))->tl);}
static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct
_tagged_string* v, struct Cyc_List_List* nss){ for( 0; nss != 0; nss= nss->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)
nss->hd); struct _handler_cons _temp9; _push_handler(& _temp9);{ struct
_xenum_struct* _temp10=( struct _xenum_struct*) setjmp( _temp9.handler); if( !
_temp10){ lookup( ge2, v); Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp11=*
v; xprintf("%.*s is ambiguous", _temp11.last_plus_one - _temp11.curr, _temp11.curr);}));;
_pop_handler();} else{ struct _xenum_struct* _temp13= _temp10; _LL15: if(
_temp13->tag == Cyc_Dict_Absent_tag){ goto _LL16;} else{ goto _LL17;} _LL17:
goto _LL18; _LL16: goto _LL14; _LL18:( void) _throw( _temp13); _LL14:;}}}
return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v){ struct Cyc_List_List* cns= te->ns;
void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp19; _push_handler(& _temp19);{ struct
_xenum_struct* _temp20=( struct _xenum_struct*) setjmp( _temp19.handler); if( !
_temp20){ result= lookup( ge, v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)(
te, loc, lookup, v, ge->availables);{ void* _temp21= result; _npop_handler( 0u);
return _temp21;}; _pop_handler();} else{ struct _xenum_struct* _temp23= _temp20;
_LL25: if( _temp23->tag == Cyc_Dict_Absent_tag){ goto _LL26;} else{ goto _LL27;}
_LL27: goto _LL28; _LL26: goto _LL24; _LL28:( void) _throw( _temp23); _LL24:;}}}{
struct Cyc_List_List* nss= ge->availables; for( 0; nss != 0; nss= nss->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)
nss->hd); struct _handler_cons _temp29; _push_handler(& _temp29);{ struct
_xenum_struct* _temp30=( struct _xenum_struct*) setjmp( _temp29.handler); if( !
_temp30){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)(
te, loc, lookup, v, nss->tl);{ void* _temp31= result; _npop_handler( 0u); return
_temp31;}; _pop_handler();} else{ struct _xenum_struct* _temp33= _temp30; _LL35:
if( _temp33->tag == Cyc_Dict_Absent_tag){ goto _LL36;} else{ goto _LL37;} _LL37:
goto _LL38; _LL36: goto _LL34; _LL38:( void) _throw( _temp33); _LL34:;}}}} if(
cns == 0){( void) _throw(({ struct Cyc_Dict_Absent_struct* _temp39=( struct Cyc_Dict_Absent_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct)); _temp39->tag= Cyc_Dict_Absent_tag;(
struct _xenum_struct*) _temp39;}));} cns= Cyc_Tcenv_outer_namespace( cns);}}
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_string* v){ struct _tuple5* ans=(( struct _tuple5*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2=
1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp42;
struct _tagged_string* _temp43; void* _temp45; struct _tuple0* _temp40= q;
_temp42=* _temp40; _LL46: _temp45= _temp42.f1; goto _LL44; _LL44: _temp43=
_temp42.f2; goto _LL41; _LL41: { void* _temp47= _temp45; struct Cyc_List_List*
_temp57; struct Cyc_List_List* _temp59; struct Cyc_List_List _temp61; struct Cyc_List_List*
_temp62; struct _tagged_string* _temp64; struct Cyc_List_List* _temp66; _LL49:
if( _temp47 == Cyc_Absyn_Loc_n){ goto _LL50;} else{ goto _LL51;} _LL51: if((
unsigned int) _temp47 > 1u?(( struct _enum_struct*) _temp47)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL58: _temp57=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp47)->f1; if( _temp57 == 0){ goto _LL52;} else{ goto _LL53;}} else{ goto
_LL53;} _LL53: if(( unsigned int) _temp47 > 1u?(( struct _enum_struct*) _temp47)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL60: _temp59=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp47)->f1; if( _temp59 == 0){ goto _LL55;} else{ _temp61=* _temp59; _LL65:
_temp64=( struct _tagged_string*) _temp61.hd; goto _LL63; _LL63: _temp62=(
struct Cyc_List_List*) _temp61.tl; goto _LL54;}} else{ goto _LL55;} _LL55: if((
unsigned int) _temp47 > 1u?(( struct _enum_struct*) _temp47)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL67: _temp66=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp47)->f1; goto _LL56;} else{ goto _LL48;} _LL50: goto _LL52; _LL52: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f,
_temp43); _LL54: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp64, _temp62); return Cyc_Tcenv_lookup_ordinary_global_f( ge, _temp43);}
_LL56: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp66), _temp43); _LL48:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* cns= te->ns;
struct Cyc_List_List* possible_results= 0; while( 1) { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, cns);{ struct Cyc_List_List* nss= ge->availables; for(
0; nss != 0; nss= nss->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) nss->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp68=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp68->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp69=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp69->hd=( void*) n; _temp69->tl= ns; _temp69;})); _temp68->tl=
possible_results; _temp68;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp70=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp70->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp71=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp71->hd=( void*) n; _temp71->tl= ns; _temp71;})); _temp70->tl=
possible_results; _temp70;});} if( possible_results != 0){ if( possible_results->tl
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp72=* n; xprintf("%.*s is ambiguous",
_temp72.last_plus_one - _temp72.curr, _temp72.curr);}));} return( struct Cyc_List_List*)
possible_results->hd;} if( cns == 0){( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp73=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));
_temp73->tag= Cyc_Dict_Absent_tag;( struct _xenum_struct*) _temp73;}));} cns=
Cyc_Tcenv_outer_namespace( cns);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp76;
struct _tagged_string* _temp77; void* _temp79; struct _tuple0* _temp74= q;
_temp76=* _temp74; _LL80: _temp79= _temp76.f1; goto _LL78; _LL78: _temp77=
_temp76.f2; goto _LL75; _LL75: { void* _temp81= _temp79; struct Cyc_List_List*
_temp91; struct Cyc_List_List* _temp93; struct Cyc_List_List* _temp95; struct
Cyc_List_List _temp97; struct Cyc_List_List* _temp98; struct _tagged_string*
_temp100; _LL83: if( _temp81 == Cyc_Absyn_Loc_n){ goto _LL84;} else{ goto _LL85;}
_LL85: if(( unsigned int) _temp81 > 1u?(( struct _enum_struct*) _temp81)->tag ==
Cyc_Absyn_Rel_n_tag: 0){ _LL92: _temp91=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp81)->f1; if( _temp91 == 0){ goto _LL86;} else{ goto _LL87;}} else{ goto
_LL87;} _LL87: if(( unsigned int) _temp81 > 1u?(( struct _enum_struct*) _temp81)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL94: _temp93=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp81)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp81
> 1u?(( struct _enum_struct*) _temp81)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL96:
_temp95=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp81)->f1;
if( _temp95 == 0){ goto _LL82;} else{ _temp97=* _temp95; _LL101: _temp100=(
struct _tagged_string*) _temp97.hd; goto _LL99; _LL99: _temp98=( struct Cyc_List_List*)
_temp97.tl; goto _LL90;}} else{ goto _LL82;} _LL84: goto _LL86; _LL86: return((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_structdecl_f, _temp77); _LL88: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp93); return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
_temp77);} _LL90: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp100, _temp98); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls, _temp77);}
_LL82:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp104; struct _tagged_string* _temp105; void* _temp107; struct _tuple0*
_temp102= q; _temp104=* _temp102; _LL108: _temp107= _temp104.f1; goto _LL106;
_LL106: _temp105= _temp104.f2; goto _LL103; _LL103: { void* _temp109= _temp107;
struct Cyc_List_List* _temp119; struct Cyc_List_List* _temp121; struct Cyc_List_List*
_temp123; struct Cyc_List_List _temp125; struct Cyc_List_List* _temp126; struct
_tagged_string* _temp128; _LL111: if( _temp109 == Cyc_Absyn_Loc_n){ goto _LL112;}
else{ goto _LL113;} _LL113: if(( unsigned int) _temp109 > 1u?(( struct
_enum_struct*) _temp109)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL120: _temp119=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp109)->f1; if(
_temp119 == 0){ goto _LL114;} else{ goto _LL115;}} else{ goto _LL115;} _LL115:
if(( unsigned int) _temp109 > 1u?(( struct _enum_struct*) _temp109)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL122: _temp121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp109)->f1; goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int)
_temp109 > 1u?(( struct _enum_struct*) _temp109)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL124: _temp123=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp109)->f1; if( _temp123 == 0){ goto _LL110;} else{ _temp125=* _temp123;
_LL129: _temp128=( struct _tagged_string*) _temp125.hd; goto _LL127; _LL127:
_temp126=( struct Cyc_List_List*) _temp125.tl; goto _LL118;}} else{ goto _LL110;}
_LL112: goto _LL114; _LL114: return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp105);
_LL116: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp121); return((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, _temp105);} _LL118: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp128, _temp126); return(( struct
Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->uniondecls, _temp105);} _LL110:;}} static struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v);} struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp132; struct _tagged_string*
_temp133; void* _temp135; struct _tuple0* _temp130= q; _temp132=* _temp130;
_LL136: _temp135= _temp132.f1; goto _LL134; _LL134: _temp133= _temp132.f2; goto
_LL131; _LL131: { void* _temp137= _temp135; struct Cyc_List_List* _temp147;
struct Cyc_List_List* _temp149; struct Cyc_List_List _temp151; struct Cyc_List_List*
_temp152; struct _tagged_string* _temp154; struct Cyc_List_List* _temp156;
_LL139: if( _temp137 == Cyc_Absyn_Loc_n){ goto _LL140;} else{ goto _LL141;}
_LL141: if(( unsigned int) _temp137 > 1u?(( struct _enum_struct*) _temp137)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL148: _temp147=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp137)->f1; if( _temp147 == 0){ goto _LL142;} else{
goto _LL143;}} else{ goto _LL143;} _LL143: if(( unsigned int) _temp137 > 1u?((
struct _enum_struct*) _temp137)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL150:
_temp149=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp137)->f1;
if( _temp149 == 0){ goto _LL145;} else{ _temp151=* _temp149; _LL155: _temp154=(
struct _tagged_string*) _temp151.hd; goto _LL153; _LL153: _temp152=( struct Cyc_List_List*)
_temp151.tl; goto _LL144;}} else{ goto _LL145;} _LL145: if(( unsigned int)
_temp137 > 1u?(( struct _enum_struct*) _temp137)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL157: _temp156=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp137)->f1; goto _LL146;} else{ goto _LL138;} _LL140: goto _LL142; _LL142:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_enumdecl_f, _temp133); _LL144: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace(
te, loc, _temp154, _temp152); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
_temp133);} _LL146: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp156); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, _temp133);} _LL138:;}}
static struct Cyc_Absyn_Xenumdecl** Cyc_Tcenv_lookup_xenumdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Xenumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xenumdecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp160;
struct _tagged_string* _temp161; void* _temp163; struct _tuple0* _temp158= q;
_temp160=* _temp158; _LL164: _temp163= _temp160.f1; goto _LL162; _LL162:
_temp161= _temp160.f2; goto _LL159; _LL159: { void* _temp165= _temp163; struct
Cyc_List_List* _temp175; struct Cyc_List_List* _temp177; struct Cyc_List_List
_temp179; struct Cyc_List_List* _temp180; struct _tagged_string* _temp182;
struct Cyc_List_List* _temp184; _LL167: if(( unsigned int) _temp165 > 1u?((
struct _enum_struct*) _temp165)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL176:
_temp175=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp165)->f1;
if( _temp175 == 0){ goto _LL168;} else{ goto _LL169;}} else{ goto _LL169;}
_LL169: if( _temp165 == Cyc_Absyn_Loc_n){ goto _LL170;} else{ goto _LL171;}
_LL171: if(( unsigned int) _temp165 > 1u?(( struct _enum_struct*) _temp165)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL178: _temp177=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp165)->f1; if( _temp177 == 0){ goto _LL173;} else{
_temp179=* _temp177; _LL183: _temp182=( struct _tagged_string*) _temp179.hd;
goto _LL181; _LL181: _temp180=( struct Cyc_List_List*) _temp179.tl; goto _LL172;}}
else{ goto _LL173;} _LL173: if(( unsigned int) _temp165 > 1u?(( struct
_enum_struct*) _temp165)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL185: _temp184=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp165)->f1; goto
_LL174;} else{ goto _LL166;} _LL168: { struct _handler_cons _temp186;
_push_handler(& _temp186);{ struct _xenum_struct* _temp187=( struct
_xenum_struct*) setjmp( _temp186.handler); if( ! _temp187){{ struct Cyc_Core_Opt*
_temp189=({ struct Cyc_Core_Opt* _temp188=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp188->v=( void*)(( struct Cyc_Absyn_Xenumdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Xenumdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xenumdecl_f, _temp161);
_temp188;}); _npop_handler( 0u); return _temp189;}; _pop_handler();} else{
struct _xenum_struct* _temp191= _temp187; _LL193: if( _temp191->tag == Cyc_Dict_Absent_tag){
goto _LL194;} else{ goto _LL195;} _LL195: goto _LL196; _LL194: return 0; _LL196:(
void) _throw( _temp191); _LL192:;}}} _LL170: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp197=( char*)"lookup_xenumdecl: impossible"; struct
_tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 29; _temp198;})); return 0; _LL172: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp199; _push_handler(& _temp199);{
struct _xenum_struct* _temp200=( struct _xenum_struct*) setjmp( _temp199.handler);
if( ! _temp200){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp182, _temp180);;
_pop_handler();} else{ struct _xenum_struct* _temp202= _temp200; _LL204: if(
_temp202->tag == Cyc_Dict_Absent_tag){ goto _LL205;} else{ goto _LL206;} _LL206:
goto _LL207; _LL205: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp208=( char*)"bad qualified name for xenum"; struct _tagged_string _temp209;
_temp209.curr= _temp208; _temp209.base= _temp208; _temp209.last_plus_one=
_temp208 + 29; _temp209;}));( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp210=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));
_temp210->tag= Cyc_Dict_Absent_tag;( struct _xenum_struct*) _temp210;})); _LL207:(
void) _throw( _temp202); _LL203:;}}} return({ struct Cyc_Core_Opt* _temp211=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp211->v=(
void*)(( struct Cyc_Absyn_Xenumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->xenumdecls, _temp161); _temp211;});}
_LL174: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp184); return({
struct Cyc_Core_Opt* _temp212=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp212->v=( void*)(( struct Cyc_Absyn_Xenumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xenumdecls,
_temp161); _temp212;});} _LL166:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp215; struct _tagged_string* _temp216; void* _temp218; struct _tuple0*
_temp213= q; _temp215=* _temp213; _LL219: _temp218= _temp215.f1; goto _LL217;
_LL217: _temp216= _temp215.f2; goto _LL214; _LL214: { void* _temp220= _temp218;
struct Cyc_List_List* _temp230; struct Cyc_List_List* _temp232; struct Cyc_List_List
_temp234; struct Cyc_List_List* _temp235; struct _tagged_string* _temp237;
struct Cyc_List_List* _temp239; _LL222: if( _temp220 == Cyc_Absyn_Loc_n){ goto
_LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp220 > 1u?(( struct
_enum_struct*) _temp220)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL231: _temp230=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp220)->f1; if(
_temp230 == 0){ goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;} _LL226:
if(( unsigned int) _temp220 > 1u?(( struct _enum_struct*) _temp220)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL233: _temp232=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp220)->f1; if( _temp232 == 0){ goto _LL228;} else{ _temp234=* _temp232;
_LL238: _temp237=( struct _tagged_string*) _temp234.hd; goto _LL236; _LL236:
_temp235=( struct Cyc_List_List*) _temp234.tl; goto _LL227;}} else{ goto _LL228;}
_LL228: if(( unsigned int) _temp220 > 1u?(( struct _enum_struct*) _temp220)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL240: _temp239=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp220)->f1; goto _LL229;} else{ goto _LL221;} _LL223:
goto _LL225; _LL225: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_typedefdecl_f, _temp216); _LL227: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp237, _temp235); return(( struct
Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->typedefs, _temp216);} _LL229: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp239); return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
_temp216);} _LL221:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({
struct Cyc_Tcenv_Genv* _temp241=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Genv)); _temp241->namespaces=( struct Cyc_Set_Set*) Cyc_Tcutil_empty_var_set->v;
_temp241->structdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp241->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp241->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp241->xenumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp241->typedefs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp241->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp241->availables= 0; _temp241;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp242= f; void* _temp250; void* _temp252; void* _temp254;
void* _temp256; void* _temp258; _LL244: if((( struct _enum_struct*) _temp242)->tag
== Cyc_Tcenv_Outermost_tag){ _LL251: _temp250=( void*)(( struct Cyc_Tcenv_Outermost_struct*)
_temp242)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if((( struct
_enum_struct*) _temp242)->tag == Cyc_Tcenv_Frame_tag){ _LL255: _temp254=( void*)((
struct Cyc_Tcenv_Frame_struct*) _temp242)->f1; goto _LL253; _LL253: _temp252=(
void*)(( struct Cyc_Tcenv_Frame_struct*) _temp242)->f2; goto _LL247;} else{ goto
_LL248;} _LL248: if((( struct _enum_struct*) _temp242)->tag == Cyc_Tcenv_Hidden_tag){
_LL259: _temp258=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp242)->f1; goto
_LL257; _LL257: _temp256=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp242)->f2;
goto _LL249;} else{ goto _LL243;} _LL245: return _temp250; _LL247: return
_temp254; _LL249: f= _temp256; goto _LL243; _LL243:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp260= f; void* _temp268; void* _temp270; void*
_temp272; void* _temp274; void* _temp276; _LL262: if((( struct _enum_struct*)
_temp260)->tag == Cyc_Tcenv_Outermost_tag){ _LL269: _temp268=( void*)(( struct
Cyc_Tcenv_Outermost_struct*) _temp260)->f1; goto _LL263;} else{ goto _LL264;}
_LL264: if((( struct _enum_struct*) _temp260)->tag == Cyc_Tcenv_Frame_tag){
_LL273: _temp272=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp260)->f1; goto
_LL271; _LL271: _temp270=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp260)->f2;
goto _LL265;} else{ goto _LL266;} _LL266: if((( struct _enum_struct*) _temp260)->tag
== Cyc_Tcenv_Hidden_tag){ _LL277: _temp276=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp260)->f1; goto _LL275; _LL275: _temp274=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp260)->f2; goto _LL267;} else{ goto _LL261;} _LL263: return({ struct Cyc_Tcenv_Outermost_struct*
_temp278=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp278->tag= Cyc_Tcenv_Outermost_tag; _temp278->f1=( void*) x;( void*)
_temp278;}); _LL265: return({ struct Cyc_Tcenv_Frame_struct* _temp279=( struct
Cyc_Tcenv_Frame_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct));
_temp279->tag= Cyc_Tcenv_Frame_tag; _temp279->f1=( void*) x; _temp279->f2=( void*)
_temp270;( void*) _temp279;}); _LL267: return({ struct Cyc_Tcenv_Hidden_struct*
_temp280=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp280->tag= Cyc_Tcenv_Hidden_tag; _temp280->f1=( void*) _temp276; _temp280->f2=(
void*)(( void*(*)( void* f, void* x)) Cyc_Tcenv_put_visible)( _temp274, x);(
void*) _temp280;}); _LL261:;} void* Cyc_Tcenv_apply_to_visible( void*(* g)( void*),
void* f){ return(( void*(*)( void* f, void* x)) Cyc_Tcenv_put_visible)( f, g(((
void*(*)( void* f)) Cyc_Tcenv_get_visible)( f)));} void* Cyc_Tcenv_map_frames(
void*(* g)( void*), void* f){ void* _temp281= f; void* _temp289; void* _temp291;
void* _temp293; void* _temp295; void* _temp297; _LL283: if((( struct
_enum_struct*) _temp281)->tag == Cyc_Tcenv_Outermost_tag){ _LL290: _temp289=(
void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp281)->f1; goto _LL284;} else{
goto _LL285;} _LL285: if((( struct _enum_struct*) _temp281)->tag == Cyc_Tcenv_Frame_tag){
_LL294: _temp293=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp281)->f1; goto
_LL292; _LL292: _temp291=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp281)->f2;
goto _LL286;} else{ goto _LL287;} _LL287: if((( struct _enum_struct*) _temp281)->tag
== Cyc_Tcenv_Hidden_tag){ _LL298: _temp297=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp281)->f1; goto _LL296; _LL296: _temp295=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp281)->f2; goto _LL288;} else{ goto _LL282;} _LL284: return({ struct Cyc_Tcenv_Outermost_struct*
_temp299=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp299->tag= Cyc_Tcenv_Outermost_tag; _temp299->f1=( void*) g( _temp289);(
void*) _temp299;}); _LL286: return({ struct Cyc_Tcenv_Frame_struct* _temp300=(
struct Cyc_Tcenv_Frame_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct));
_temp300->tag= Cyc_Tcenv_Frame_tag; _temp300->f1=( void*) g( _temp293); _temp300->f2=(
void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)( g,
_temp291);( void*) _temp300;}); _LL288: return({ struct Cyc_Tcenv_Hidden_struct*
_temp301=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp301->tag= Cyc_Tcenv_Hidden_tag; _temp301->f1=( void*) g( _temp297);
_temp301->f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp295);( void*) _temp301;}); _LL282:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){( void) _throw(({ struct Cyc_Tcenv_Env_error_struct*
_temp302=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcenv_Env_error_struct)); _temp302->tag= Cyc_Tcenv_Env_error_tag;( struct
_xenum_struct*) _temp302;}));} return(( struct Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)((
void*)( te->le)->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( void) _throw(({ struct Cyc_Tcenv_Env_error_struct*
_temp303=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcenv_Env_error_struct)); _temp303->tag= Cyc_Tcenv_Env_error_tag;( struct
_xenum_struct*) _temp303;}));} return({ struct Cyc_Tcenv_Tenv* _temp304=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp304->ns= te->ns;
_temp304->ae= te->ae; _temp304->le=({ struct Cyc_Core_Opt* _temp305=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp305->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)( te->le)->v, fe);
_temp305;}); _temp304;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp306=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp306->shared= f->shared;
_temp306->type_vars= f->type_vars; _temp306->locals= f->locals; _temp306->encloser=
f->encloser; _temp306->ctrl_env= f->ctrl_env; _temp306->capability=( void*)((
void*) f->capability); _temp306->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp306;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct
Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct
Cyc_Tcenv_Fenv* _temp307=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp307->shared= f->shared; _temp307->type_vars= f->type_vars; _temp307->locals=
f->locals; _temp307->encloser= f->encloser; _temp307->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp308=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp308->continue_stmt=( void*)(( void*) c->continue_stmt); _temp308->break_stmt=(
void*)(( void*) c->break_stmt); _temp308->fallthru_clause= c->fallthru_clause;
_temp308->next_stmt=( void*)(( void*) c->next_stmt); _temp308->try_depth= c->try_depth;
_temp308;}); _temp307->capability=( void*)(( void*) f->capability); _temp307->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp307;});} void* Cyc_Tcenv_return_typ( struct
Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string msg))
Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp309=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp310; _temp310.curr= _temp309; _temp310.base= _temp309;
_temp310.last_plus_one= _temp309 + 29; _temp310;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp311=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp312; _temp312.curr= _temp311; _temp312.base= _temp311; _temp312.last_plus_one=
_temp311 + 32; _temp312;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_List_List* new_tvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, fe->type_vars);
Cyc_Tcutil_check_unique_tvars( loc, new_tvs); fe->type_vars= new_tvs; return Cyc_Tcenv_put_fenv(
te, fe);}} static int Cyc_Tcenv_shadow_ctr= 1; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp313=( char*)"Tcenv: unexpected add_local_var";
struct _tagged_string _temp314; _temp314.curr= _temp313; _temp314.base= _temp313;
_temp314.last_plus_one= _temp313 + 32; _temp314;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2;{ void* _temp315=(* vd->name).f1; _LL317: if( _temp315 == Cyc_Absyn_Loc_n){
goto _LL318;} else{ goto _LL319;} _LL319: goto _LL320; _LL318: goto _LL316;
_LL320:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp321=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp321->tag= Cyc_Core_Impossible_tag; _temp321->f1=( struct _tagged_string)({
char* _temp322=( char*)"add_local_var: called with Rel_n"; struct _tagged_string
_temp323; _temp323.curr= _temp322; _temp323.base= _temp322; _temp323.last_plus_one=
_temp322 + 33; _temp323;});( struct _xenum_struct*) _temp321;})); goto _LL316;
_LL316:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_member)( fe->locals, v)){ vd->shadow=( ++ Cyc_Tcenv_shadow_ctr);} fe->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
void* data)) Cyc_Dict_insert)( fe->locals, v,({ struct Cyc_Absyn_Local_b_struct*
_temp324=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp324->tag= Cyc_Absyn_Local_b_tag; _temp324->f1= vd;( void*) _temp324;}));
return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp325=( char*)"Tcenv: unexpected add_pat_var";
struct _tagged_string _temp326; _temp326.curr= _temp325; _temp326.base= _temp325;
_temp326.last_plus_one= _temp325 + 30; _temp326;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2; if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_member)( fe->locals, v)){ vd->shadow=( ++ Cyc_Tcenv_shadow_ctr);} fe->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
void* data)) Cyc_Dict_insert)( fe->locals, v,({ struct Cyc_Absyn_Pat_b_struct*
_temp327=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp327->tag= Cyc_Absyn_Pat_b_tag; _temp327->f1= vd;( void*) _temp327;}));
return Cyc_Tcenv_put_fenv( te, fe);}} void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct
_tuple0 _temp330; struct _tagged_string* _temp331; void* _temp333; struct
_tuple0* _temp328= q; _temp330=* _temp328; _LL334: _temp333= _temp330.f1; goto
_LL332; _LL332: _temp331= _temp330.f2; goto _LL329; _LL329: { void* _temp335=
_temp333; struct Cyc_List_List* _temp344; _LL337: if( _temp335 == Cyc_Absyn_Loc_n){
goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int) _temp335 > 1u?((
struct _enum_struct*) _temp335)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL345:
_temp344=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp335)->f1;
if( _temp344 == 0){ goto _LL343;} else{ goto _LL341;}} else{ goto _LL341;}
_LL343: if( te->le != 0){ goto _LL340;} else{ goto _LL341;} _LL341: goto _LL342;
_LL338: if( te->le == 0){( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp346=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));
_temp346->tag= Cyc_Dict_Absent_tag;( struct _xenum_struct*) _temp346;}));} else{
goto _LL340;} _LL340: { struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct _handler_cons _temp347; _push_handler(& _temp347);{ struct _xenum_struct*
_temp348=( struct _xenum_struct*) setjmp( _temp347.handler); if( ! _temp348){{
void* _temp350=({ struct Cyc_Tcenv_VarRes_struct* _temp349=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp349->tag= Cyc_Tcenv_VarRes_tag;
_temp349->f1=( void*)(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( fe->locals, _temp331);( void*) _temp349;});
_npop_handler( 0u); return _temp350;}; _pop_handler();} else{ struct
_xenum_struct* _temp352= _temp348; _LL354: if( _temp352->tag == Cyc_Dict_Absent_tag){
goto _LL355;} else{ goto _LL356;} _LL356: goto _LL357; _LL355: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL357:( void) _throw( _temp352); _LL353:;}}} _LL342: { struct
_handler_cons _temp358; _push_handler(& _temp358);{ struct _xenum_struct*
_temp359=( struct _xenum_struct*) setjmp( _temp358.handler); if( ! _temp359){{
void* _temp360= Cyc_Tcenv_lookup_ordinary_global( te, loc, q); _npop_handler( 0u);
return _temp360;}; _pop_handler();} else{ struct _xenum_struct* _temp362=
_temp359; _LL364: if( _temp362->tag == Cyc_Dict_Absent_tag){ goto _LL365;} else{
goto _LL366;} _LL366: goto _LL367; _LL365: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp368= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifer: %.*s",
_temp368.last_plus_one - _temp368.curr, _temp368.curr);})); return({ struct Cyc_Tcenv_VarRes_struct*
_temp369=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp369->tag= Cyc_Tcenv_VarRes_tag; _temp369->f1=( void*) Cyc_Absyn_Unresolved_b;(
void*) _temp369;}); _LL367:( void) _throw( _temp362); _LL363:;}}} _LL336:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp370=( char*)"Tcenv: unexpected process_continue"; struct _tagged_string
_temp371; _temp371.curr= _temp370; _temp371.base= _temp370; _temp371.last_plus_one=
_temp370 + 35; _temp371;}));}{ void* _temp372=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp382; _LL374: if(( unsigned int) _temp372 > 3u?((
struct _enum_struct*) _temp372)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL383:
_temp382=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp372)->f1;
goto _LL375;} else{ goto _LL376;} _LL376: if( _temp372 == Cyc_Tcenv_NotLoop_j){
goto _LL377;} else{ goto _LL378;} _LL378: if( _temp372 == Cyc_Tcenv_CaseEnd_j){
goto _LL379;} else{ goto _LL380;} _LL380: if( _temp372 == Cyc_Tcenv_FnEnd_j){
goto _LL381;} else{ goto _LL373;} _LL375: _temp382->non_local_preds=({ struct
Cyc_List_List* _temp384=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp384->hd=( void*) s; _temp384->tl= _temp382->non_local_preds; _temp384;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp382; return; _LL377: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp385=( char*)"continue not in a loop"; struct
_tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 23; _temp386;})); return; _LL379:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp387=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp388; _temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 32; _temp388;})); return; _LL381:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp389=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 32; _temp390;})); return; _LL373:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp391=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 32; _temp392;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp393=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp403; _LL395: if(( unsigned int) _temp393 > 3u?((
struct _enum_struct*) _temp393)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL404:
_temp403=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp393)->f1;
goto _LL396;} else{ goto _LL397;} _LL397: if( _temp393 == Cyc_Tcenv_NotLoop_j){
goto _LL398;} else{ goto _LL399;} _LL399: if( _temp393 == Cyc_Tcenv_FnEnd_j){
goto _LL400;} else{ goto _LL401;} _LL401: if( _temp393 == Cyc_Tcenv_CaseEnd_j){
goto _LL402;} else{ goto _LL394;} _LL396: _temp403->non_local_preds=({ struct
Cyc_List_List* _temp405=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp405->hd=( void*) s; _temp405->tl= _temp403->non_local_preds; _temp405;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp403; return; _LL398: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp406=( char*)"break not in a loop or switch";
struct _tagged_string _temp407; _temp407.curr= _temp406; _temp407.base= _temp406;
_temp407.last_plus_one= _temp406 + 30; _temp407;})); return; _LL400: if(( void*)(
fe->shared)->return_typ != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,( struct
_tagged_string)({ char* _temp408=( char*)"break causes function not to return a value";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 44; _temp409;}));} return; _LL402: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp410=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 56; _temp411;})); return; _LL394:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp412=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp413; _temp413.curr= _temp412; _temp413.base= _temp412; _temp413.last_plus_one=
_temp412 + 31; _temp413;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* sopt2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( sopt2 == 0){ struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* slopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, l); if( slopt
== 0){ slopt=({ struct Cyc_Core_Opt* _temp414=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp414->v=( void*) 0; _temp414;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( needed, l,({ struct Cyc_List_List*
_temp415=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp415->hd=( void*) s; _temp415->tl=( struct Cyc_List_List*) slopt->v;
_temp415;}));} else{(( struct Cyc_Absyn_Stmt*) sopt2->v)->non_local_preds=({
struct Cyc_List_List* _temp416=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp416->hd=( void*) s; _temp416->tl=(( struct Cyc_Absyn_Stmt*)
sopt2->v)->non_local_preds; _temp416;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*) sopt2->v);}}} struct _tuple4* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp417=( char*)"Tcenv: unexpected process_fallthru";
struct _tagged_string _temp418; _temp418.curr= _temp417; _temp418.base= _temp417;
_temp418.last_plus_one= _temp417 + 35; _temp418;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct _tuple4* ans=( fe->ctrl_env)->fallthru_clause;
if( ans != 0){(((* ans).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp419=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp419->hd=( void*) s; _temp419->tl=(((* ans).f1)->body)->non_local_preds;
_temp419;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp420=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp420[ 0]=(* ans).f1; _temp420;});} return ans;}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct _tuple3* pat_typ,
struct Cyc_Absyn_Switch_clause* clause){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp421=( char*)"Tcenv: unexpected set_fallthru"; struct _tagged_string
_temp422; _temp422.curr= _temp421; _temp422.base= _temp421; _temp422.last_plus_one=
_temp421 + 31; _temp422;}));}{ struct Cyc_List_List* ft_typ= 0;{ struct Cyc_List_List*
vds=(* pat_typ).f2; for( 0; vds != 0; vds= vds->tl){ ft_typ=({ struct Cyc_List_List*
_temp423=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp423->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*) vds->hd)->type);
_temp423->tl= ft_typ; _temp423;});}} ft_typ=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( ft_typ);{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({ struct _tuple4*
_temp424=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp424->f1=
clause; _temp424->f2=(* pat_typ).f1; _temp424->f3= ft_typ; _temp424;}); return
Cyc_Tcenv_put_fenv( te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp425=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 33; _temp426;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp427=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 30; _temp428;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp429=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp429->tag= Cyc_Tcenv_Stmt_j_tag;
_temp429->f1= continue_dest;( void*) _temp429;}));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp430=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp430->tag= Cyc_Tcenv_Stmt_j_tag;
_temp430->f1= continue_dest;( void*) _temp430;})); return Cyc_Tcenv_put_fenv( te,
fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp431=( char*)"Tcenv: unexpected enter_try";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 28; _temp432;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp433=( char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp434; _temp434.curr= _temp433; _temp434.base= _temp433;
_temp434.last_plus_one= _temp433 + 32; _temp434;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp435=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 32; _temp436;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*) Cyc_Tcenv_CaseEnd_j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp437=( char*)"Tcenv: unexpected set_next"; struct _tagged_string
_temp438; _temp438.curr= _temp437; _temp438.base= _temp437; _temp438.last_plus_one=
_temp437 + 27; _temp438;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=( void*) j); return
Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp439=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 28; _temp440;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* sl_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, v); if( sl_opt
!= 0){( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_delete)( needed, v);{ struct Cyc_List_List*
sl=( struct Cyc_List_List*) sl_opt->v; s->non_local_preds= sl; for( 0; sl != 0;
sl= sl->tl){ void* _temp441=( void*)(( struct Cyc_Absyn_Stmt*) sl->hd)->r;
struct Cyc_Absyn_Stmt* _temp447; struct Cyc_Absyn_Stmt** _temp449; struct
_tagged_string* _temp450; _LL443: if(( unsigned int) _temp441 > 1u?(( struct
_enum_struct*) _temp441)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL451: _temp450=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp441)->f1; goto
_LL448; _LL448: _temp447=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp441)->f2; _temp449=&(( struct Cyc_Absyn_Goto_s_struct*) _temp441)->f2; goto
_LL444;} else{ goto _LL445;} _LL445: goto _LL446; _LL444:* _temp449=( struct Cyc_Absyn_Stmt*)
s; goto _LL442; _LL446:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp452=( char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 42; _temp453;})); goto _LL442; _LL442:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
fe->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct _tagged_string
_temp454=* v; xprintf("Repeated label: %.*s", _temp454.last_plus_one - _temp454.curr,
_temp454.curr);}));}( fe->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( fe->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp455=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 31; _temp456;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp457=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp458; _temp458.curr= _temp457; _temp458.base= _temp457; _temp458.last_plus_one=
_temp457 + 31; _temp458;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp459=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 28; _temp460;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar*
block_rgn= Cyc_Tcutil_new_tvar( Cyc_Absyn_RgnKind); void* block_typ=({ struct
Cyc_Absyn_VarType_struct* _temp461=( struct Cyc_Absyn_VarType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp461->tag= Cyc_Absyn_VarType_tag;
_temp461->f1= block_rgn;( void*) _temp461;}); fe->type_vars=({ struct Cyc_List_List*
_temp462=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp462->hd=( void*) block_rgn; _temp462->tl= fe->type_vars; _temp462;});( void*)(
fe->capability=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp463=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp463->tag= Cyc_Absyn_JoinEff_tag;
_temp463->f1=({ struct Cyc_List_List* _temp464=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp464->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp466=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp466->tag= Cyc_Absyn_AccessEff_tag; _temp466->f1=( void*) block_typ;( void*)
_temp466;}); _temp464->tl=({ struct Cyc_List_List* _temp465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp465->hd=( void*)(( void*) fe->capability);
_temp465->tl= 0; _temp465;}); _temp464;});( void*) _temp463;}));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return Cyc_Absyn_HeapRgn;} return(
void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp467= Cyc_Absynpp_typ2string(
rgn); xprintf("Expression accesses unavailable region %.*s", _temp467.last_plus_one
- _temp467.curr, _temp467.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=({ struct Cyc_Absyn_JoinEff_struct*
_temp468=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp468->tag= Cyc_Absyn_JoinEff_tag; _temp468->f1= 0;( void*) _temp468;});}
else{ struct Cyc_Tcenv_Fenv* fenv= Cyc_Tcenv_get_fenv( te); capability=( void*)
fenv->capability;} if( ! Cyc_Tcutil_subset_effect( 1, eff, capability)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp469= Cyc_Absynpp_typ2string( capability);
struct _tagged_string _temp470= Cyc_Absynpp_typ2string( eff); xprintf("Capability %.*s does not cover function's effect %.*s",
_temp469.last_plus_one - _temp469.curr, _temp469.curr, _temp470.last_plus_one -
_temp470.curr, _temp470.curr);}));}} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar* rgn0= Cyc_Tcutil_new_tvar( Cyc_Absyn_RgnKind);
void* param_rgn=({ struct Cyc_Absyn_VarType_struct* _temp471=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp471->tag= Cyc_Absyn_VarType_tag;
_temp471->f1= rgn0;( void*) _temp471;}); struct Cyc_List_List* vds= 0;{ struct
Cyc_List_List* args= fd->args; for( 0; args != 0; args= args->tl){ struct Cyc_Absyn_Vardecl*
vd=({ struct Cyc_Absyn_Vardecl* _temp472=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp472[ 0]=({ struct Cyc_Absyn_Vardecl
_temp473; _temp473.sc=( void*) Cyc_Absyn_Public; _temp473.name=({ struct _tuple0*
_temp476=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) * 1); _temp476[ 0]=({
struct _tuple0 _temp477; _temp477.f1= Cyc_Absyn_Loc_n; _temp477.f2=(*(( struct
_tuple6*) args->hd)).f1; _temp477;}); _temp476;}); _temp473.tq=(*(( struct
_tuple6*) args->hd)).f2; _temp473.type=( void*)(*(( struct _tuple6*) args->hd)).f3;
_temp473.initializer= 0; _temp473.shadow= 0; _temp473.region=({ struct Cyc_Core_Opt*
_temp474=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp474[ 0]=({ struct Cyc_Core_Opt _temp475; _temp475.v=( void*) param_rgn;
_temp475;}); _temp474;}); _temp473.attributes= 0; _temp473;}); _temp472;}); vds=({
struct Cyc_List_List* _temp478=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp478[ 0]=({ struct Cyc_List_List _temp479;
_temp479.hd=( void*) vd; _temp479.tl= vds; _temp479;}); _temp478;}); locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
void* data)) Cyc_Dict_insert)( locals,(*(( struct _tuple6*) args->hd)).f1,({
struct Cyc_Absyn_Param_b_struct* _temp480=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp480->tag= Cyc_Absyn_Param_b_tag;
_temp480->f1= vd;( void*) _temp480;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp481=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp481[ 0]=({ struct Cyc_Core_Opt _temp482; _temp482.v=( void*) vds; _temp482;});
_temp481;}); return({ struct Cyc_Tcenv_Fenv* _temp483=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp483->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp490=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp490->return_typ=( void*)(( void*) fd->ret_type); _temp490->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp490->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp490;});
_temp483->type_vars=({ struct Cyc_List_List* _temp489=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp489->hd=( void*) rgn0; _temp489->tl=
fd->tvs; _temp489;}); _temp483->locals= locals; _temp483->encloser= fd->body;
_temp483->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp488=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp488->continue_stmt=( void*)
Cyc_Tcenv_NotLoop_j; _temp488->break_stmt=( void*) Cyc_Tcenv_NotLoop_j; _temp488->fallthru_clause=
0; _temp488->next_stmt=( void*) Cyc_Tcenv_FnEnd_j; _temp488->try_depth= 0;
_temp488;}); _temp483->capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp484=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp484->tag= Cyc_Absyn_JoinEff_tag; _temp484->f1=({ struct Cyc_List_List*
_temp485=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp485->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp487=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp487->tag= Cyc_Absyn_AccessEff_tag;
_temp487->f1=( void*) param_rgn;( void*) _temp487;}); _temp485->tl=({ struct Cyc_List_List*
_temp486=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp486->hd=( void*)(( void*)( fd->effect)->v); _temp486->tl= 0; _temp486;});
_temp485;});( void*) _temp484;}); _temp483->curr_rgn=( void*) param_rgn;
_temp483;});}