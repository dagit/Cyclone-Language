#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ;
struct _tuple5{ void* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_Dict_Dict* f1; struct Cyc_List_List* f2; } ; struct _tuple7{ struct Cyc_Dict_Dict*
f1; struct Cyc_Set_Set** f2; } ; typedef unsigned int Cyc_uint; typedef char*
Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp(
int, int); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[
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
Cyc_List_tl( struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_mem( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_is_empty(
struct Cyc_Set_Set* s); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{
char* tag; } ; extern void* Cyc_Set_choose( struct Cyc_Set_Set* s); struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{
char* tag; } ; extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; typedef
struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
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
; typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_compress(
void* t); typedef void* Cyc_CfFlowInfo_LocalRoot; typedef void* Cyc_CfFlowInfo_Field;
struct Cyc_CfFlowInfo_Place; typedef void* Cyc_CfFlowInfo_Escaped; typedef void*
Cyc_CfFlowInfo_InitLevel; typedef void* Cyc_CfFlowInfo_InitState; typedef void*
Cyc_CfFlowInfo_PathInfo; typedef void* Cyc_CfFlowInfo_FlowInfo; typedef void*
Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t; typedef
struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; const int Cyc_CfFlowInfo_VarRoot_tag=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; const int Cyc_CfFlowInfo_MallocPt_tag= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const int Cyc_CfFlowInfo_StructF_tag= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
const int Cyc_CfFlowInfo_TupleF_tag= 1; struct Cyc_CfFlowInfo_TupleF_struct{ int
tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; void* Cyc_CfFlowInfo_Esc=( void*) 0; void* Cyc_CfFlowInfo_Unesc=(
void*) 1; void* Cyc_CfFlowInfo_NoneIL=( void*) 0; void* Cyc_CfFlowInfo_ThisIL=(
void*) 1; void* Cyc_CfFlowInfo_AllIL=( void*) 2; const int Cyc_CfFlowInfo_UnknownIS_tag=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
const int Cyc_CfFlowInfo_MustPointTo_tag= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; const int Cyc_CfFlowInfo_LeafPI_tag=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; const int Cyc_CfFlowInfo_TuplePI_tag=
1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
const int Cyc_CfFlowInfo_StructPI_tag= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; void* Cyc_CfFlowInfo_BottomFL=( void*) 0;
const int Cyc_CfFlowInfo_InitsFL_tag= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct _temp1={ 0u,(
void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp0={ 0u,( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp1)}; void*
Cyc_CfFlowInfo_leaf_unesc_none=( struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp0;
static struct Cyc_CfFlowInfo_UnknownIS_struct _temp3={ 0u,( void*)(( void*) 1u),(
void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_LeafPI_struct _temp2={ 0u,(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp3)}; void* Cyc_CfFlowInfo_leaf_unesc_this=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp2; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp5={ 0u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp4={ 0u,( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp5)}; void*
Cyc_CfFlowInfo_leaf_unesc_all=( struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp4;
static struct Cyc_CfFlowInfo_UnknownIS_struct _temp7={ 0u,( void*)(( void*) 0u),(
void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_LeafPI_struct _temp6={ 0u,(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp7)}; void* Cyc_CfFlowInfo_leaf_esc_none=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp6; static struct Cyc_CfFlowInfo_UnknownIS_struct
_temp9={ 0u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_LeafPI_struct
_temp8={ 0u,( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp9)}; void*
Cyc_CfFlowInfo_leaf_esc_this=( struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp8;
static struct Cyc_CfFlowInfo_UnknownIS_struct _temp11={ 0u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct _temp10={ 0u,(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)& _temp11)}; void* Cyc_CfFlowInfo_leaf_esc_all=(
struct Cyc_CfFlowInfo_LeafPI_struct*)& _temp10; void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1=
esc; _temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31;
void* _temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 == Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{ goto _LL17;}
_LL28: _temp27= _temp13.f2; if( _temp27 == Cyc_CfFlowInfo_NoneIL){ goto _LL16;}
else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33 == Cyc_CfFlowInfo_Unesc){
goto _LL32;} else{ goto _LL19;} _LL32: _temp31= _temp13.f2; if( _temp31 == Cyc_CfFlowInfo_ThisIL){
goto _LL18;} else{ goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37
== Cyc_CfFlowInfo_Unesc){ goto _LL36;} else{ goto _LL21;} _LL36: _temp35=
_temp13.f2; if( _temp35 == Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;}
_LL21: _LL42: _temp41= _temp13.f1; if( _temp41 == Cyc_CfFlowInfo_Esc){ goto
_LL40;} else{ goto _LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 == Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
== Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43= _temp13.f2;
if( _temp43 == Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{ goto _LL25;} _LL25:
_LL50: _temp49= _temp13.f1; if( _temp49 == Cyc_CfFlowInfo_Esc){ goto _LL48;}
else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 == Cyc_CfFlowInfo_AllIL){
goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(((
struct _enum_struct*) _temp66)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL69:
_temp68=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(((
struct _enum_struct*) _temp62)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL65:
_temp64=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if((( struct _enum_struct*) _temp74)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL77:
_temp76=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(((
struct _enum_struct*) _temp70)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL73:
_temp72=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if((( struct _enum_struct*) _temp82)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL85:
_temp84=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(((
struct _enum_struct*) _temp78)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL81:
_temp80=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if((( struct _enum_struct*) _temp90)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL93:
_temp92=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(((
struct _enum_struct*) _temp86)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL89:
_temp88=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_string* _temp111; void* _temp113;
struct _tagged_string* _temp115; void* _temp117; int _temp119; void* _temp121;
struct _tagged_string* _temp123; void* _temp125; struct _tagged_string* _temp127;
void* _temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110:
_temp109= _temp95.f1; if((( struct _enum_struct*) _temp109)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL112: _temp111=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp109)->f1; goto _LL106;} else{ goto _LL99;} _LL106: _temp105= _temp95.f2;
if((( struct _enum_struct*) _temp105)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL108:
_temp107=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if((( struct
_enum_struct*) _temp117)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL120: _temp119=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp117)->f1; goto _LL114;} else{
goto _LL101;} _LL114: _temp113= _temp95.f2; if((( struct _enum_struct*) _temp113)->tag
== Cyc_CfFlowInfo_StructF_tag){ _LL116: _temp115=( struct _tagged_string*)((
struct Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1; goto _LL100;} else{ goto
_LL101;} _LL101: _LL126: _temp125= _temp95.f1; if((( struct _enum_struct*)
_temp125)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL128: _temp127=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp125)->f1; goto
_LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2; if((( struct
_enum_struct*) _temp121)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL124: _temp123=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp121)->f1;
goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133= _temp95.f1; if(((
struct _enum_struct*) _temp133)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL136:
_temp135=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1; goto
_LL130;} else{ goto _LL96;} _LL130: _temp129= _temp95.f2; if((( struct
_enum_struct*) _temp129)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL132: _temp131=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto _LL104;} else{
goto _LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return(( int(*)(
struct _tagged_string*, struct _tagged_string*)) Cyc_Core_ptrcmp)( _temp127,
_temp123); _LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void*
_temp137= Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp145; struct Cyc_Absyn_Structdecl**
_temp147; struct Cyc_Absyn_Structdecl* _temp149; struct Cyc_Absyn_Structdecl
_temp150; struct Cyc_List_List* _temp151; struct Cyc_Core_Opt* _temp153; struct
Cyc_Core_Opt _temp155; struct Cyc_List_List* _temp156; struct Cyc_List_List*
_temp158; struct Cyc_Core_Opt* _temp160; void* _temp162; struct Cyc_List_List*
_temp164; struct _tuple0* _temp166; _LL139: if(( unsigned int) _temp137 > 4u?((
struct _enum_struct*) _temp137)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL146:
_temp145=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp137)->f1;
goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 4u?((
struct _enum_struct*) _temp137)->tag == Cyc_Absyn_StructType_tag: 0){ _LL167:
_temp166=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp137)->f1;
goto _LL165; _LL165: _temp164=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f2; goto _LL148; _LL148: _temp147=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp137)->f3; if( _temp147 == 0){ goto
_LL143;} else{ _temp149=* _temp147; _temp150=* _temp149; _LL163: _temp162=( void*)
_temp150.sc; goto _LL161; _LL161: _temp160=( struct Cyc_Core_Opt*) _temp150.name;
goto _LL159; _LL159: _temp158=( struct Cyc_List_List*) _temp150.tvs; goto _LL154;
_LL154: _temp153=( struct Cyc_Core_Opt*) _temp150.fields; if( _temp153 == 0){
goto _LL143;} else{ _temp155=* _temp153; _LL157: _temp156=( struct Cyc_List_List*)
_temp155.v; goto _LL152;} _LL152: _temp151=( struct Cyc_List_List*) _temp150.attributes;
goto _LL142;}} else{ goto _LL143;} _LL143: goto _LL144; _LL140: { struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{
int i= 0; for( 0; _temp145 != 0; _temp145= _temp145->tl, ++ i){ struct _tuple4
_temp170; void* _temp171; struct Cyc_Absyn_Tqual* _temp173; struct _tuple4*
_temp168=( struct _tuple4*) _temp145->hd; _temp170=* _temp168; _LL174: _temp173=
_temp170.f1; goto _LL172; _LL172: _temp171= _temp170.f2; goto _LL169; _LL169: d=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)(
d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp171));}} return({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp175=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp175->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp175->f1= d;( void*) _temp175;});}
_LL142: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp156 != 0; _temp156= _temp156->tl){ struct Cyc_Absyn_Structfield
_temp178; struct Cyc_List_List* _temp179; struct Cyc_Core_Opt* _temp181; void*
_temp183; struct Cyc_Absyn_Tqual* _temp185; struct _tagged_string* _temp187;
struct Cyc_Absyn_Structfield* _temp176=( struct Cyc_Absyn_Structfield*) _temp156->hd;
_temp178=* _temp176; _LL188: _temp187=( struct _tagged_string*) _temp178.name;
goto _LL186; _LL186: _temp185=( struct Cyc_Absyn_Tqual*) _temp178.tq; goto
_LL184; _LL184: _temp183=( void*) _temp178.type; goto _LL182; _LL182: _temp181=(
struct Cyc_Core_Opt*) _temp178.width; goto _LL180; _LL180: _temp179=( struct Cyc_List_List*)
_temp178.attributes; goto _LL177; _LL177: if( Cyc_String_strcmp(* _temp187,(
struct _tagged_string)({ char* _temp189=( char*)""; struct _tagged_string
_temp190; _temp190.curr= _temp189; _temp190.base= _temp189; _temp190.last_plus_one=
_temp189 + 1; _temp190;})) != 0){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( d, _temp187, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp183));}} return({ struct Cyc_CfFlowInfo_StructPI_struct* _temp191=( struct
Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp191->tag= Cyc_CfFlowInfo_StructPI_tag; _temp191->f1= d;( void*) _temp191;});}
_LL144: return Cyc_CfFlowInfo_leaf_unesc_none; _LL138:;} void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place){ struct Cyc_CfFlowInfo_Place
_temp194; struct Cyc_List_List* _temp195; void* _temp197; struct Cyc_CfFlowInfo_Place*
_temp192= place; _temp194=* _temp192; _LL198: _temp197=( void*) _temp194.root;
goto _LL196; _LL196: _temp195=( struct Cyc_List_List*) _temp194.fields; goto
_LL193; _LL193: { void* ans=(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( d, _temp197); for( 0; _temp195 != 0; _temp195= _temp195->tl){
struct _tuple3 _temp200=({ struct _tuple3 _temp199; _temp199.f1= ans; _temp199.f2=(
void*) _temp195->hd; _temp199;}); void* _temp208; int _temp210; void* _temp212;
struct Cyc_Dict_Dict* _temp214; void* _temp216; struct _tagged_string* _temp218;
void* _temp220; struct Cyc_Dict_Dict* _temp222; _LL202: _LL213: _temp212=
_temp200.f1; if((( struct _enum_struct*) _temp212)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL215: _temp214=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp212)->f1; goto _LL209;} else{ goto _LL204;} _LL209: _temp208= _temp200.f2;
if((( struct _enum_struct*) _temp208)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL211:
_temp210=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp208)->f1; goto
_LL203;} else{ goto _LL204;} _LL204: _LL221: _temp220= _temp200.f1; if((( struct
_enum_struct*) _temp220)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL223: _temp222=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp220)->f1;
goto _LL217;} else{ goto _LL206;} _LL217: _temp216= _temp200.f2; if((( struct
_enum_struct*) _temp216)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL219: _temp218=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp216)->f1;
goto _LL205;} else{ goto _LL206;} _LL206: goto _LL207; _LL203: ans=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp214, _temp210); goto
_LL201; _LL205: ans=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( _temp222, _temp218); goto _LL201; _LL207:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp224=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp224->tag= Cyc_Core_Impossible_tag;
_temp224->f1=( struct _tagged_string)({ char* _temp225=( char*)"bad lookup_place";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 17; _temp226;});( struct _xenum_struct*)
_temp224;})); _LL201:;} return ans;}} void* Cyc_CfFlowInfo_insert_place_rec(
void* insert_path, struct Cyc_List_List* fields, void* pinfo){ if( fields == 0){
return pinfo;}{ struct _tuple5 _temp228=({ struct _tuple5 _temp227; _temp227.f1=
insert_path; _temp227.f2= fields; _temp227;}); struct Cyc_List_List* _temp236;
struct Cyc_List_List _temp238; struct Cyc_List_List* _temp239; void* _temp241;
int _temp243; void* _temp245; struct Cyc_Dict_Dict* _temp247; struct Cyc_List_List*
_temp249; struct Cyc_List_List _temp251; struct Cyc_List_List* _temp252; void*
_temp254; struct _tagged_string* _temp256; void* _temp258; struct Cyc_Dict_Dict*
_temp260; _LL230: _LL246: _temp245= _temp228.f1; if((( struct _enum_struct*)
_temp245)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL248: _temp247=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp245)->f1; goto _LL237;} else{ goto
_LL232;} _LL237: _temp236= _temp228.f2; if( _temp236 == 0){ goto _LL232;} else{
_temp238=* _temp236; _LL242: _temp241=( void*) _temp238.hd; if((( struct
_enum_struct*) _temp241)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL244: _temp243=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp241)->f1; goto _LL240;} else{
goto _LL232;} _LL240: _temp239=( struct Cyc_List_List*) _temp238.tl; goto _LL231;}
_LL232: _LL259: _temp258= _temp228.f1; if((( struct _enum_struct*) _temp258)->tag
== Cyc_CfFlowInfo_StructPI_tag){ _LL261: _temp260=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp258)->f1; goto _LL250;} else{ goto
_LL234;} _LL250: _temp249= _temp228.f2; if( _temp249 == 0){ goto _LL234;} else{
_temp251=* _temp249; _LL255: _temp254=( void*) _temp251.hd; if((( struct
_enum_struct*) _temp254)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL257: _temp256=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp254)->f1;
goto _LL253;} else{ goto _LL234;} _LL253: _temp252=( struct Cyc_List_List*)
_temp251.tl; goto _LL233;} _LL234: goto _LL235; _LL231: { void* next_path=((
void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp247,
_temp243); return({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp262=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp262->tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp262->f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp247, _temp243, Cyc_CfFlowInfo_insert_place_rec(
next_path, _temp239, pinfo));( void*) _temp262;});} _LL233: { void* next_path=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp260, _temp256); return({ struct Cyc_CfFlowInfo_StructPI_struct* _temp263=(
struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp263->tag= Cyc_CfFlowInfo_StructPI_tag; _temp263->f1=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
_temp260, _temp256, Cyc_CfFlowInfo_insert_place_rec( next_path, _temp252, pinfo));(
void*) _temp263;});} _LL235:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp264=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp264->tag= Cyc_Core_Impossible_tag; _temp264->f1=( struct _tagged_string)({
char* _temp265=( char*)"bad insert_place"; struct _tagged_string _temp266;
_temp266.curr= _temp265; _temp266.base= _temp265; _temp266.last_plus_one=
_temp265 + 17; _temp266;});( struct _xenum_struct*) _temp264;})); _LL229:;}}
struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place, void* pinfo){ struct Cyc_CfFlowInfo_Place
_temp269; struct Cyc_List_List* _temp270; void* _temp272; struct Cyc_CfFlowInfo_Place*
_temp267= place; _temp269=* _temp267; _LL273: _temp272=( void*) _temp269.root;
goto _LL271; _LL271: _temp270=( struct Cyc_List_List*) _temp269.fields; goto
_LL268; _LL268: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
key, void* data)) Cyc_Dict_insert)( d, _temp272, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp272),
_temp270, pinfo));} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env,
void* path); int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){
void* _temp274= state; void* _temp282; void* _temp284; void* _temp286; void*
_temp288; struct Cyc_CfFlowInfo_Place* _temp290; _LL276: if((( struct
_enum_struct*) _temp274)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL285: _temp284=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp274)->f1; goto _LL283;
_LL283: _temp282=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp274)->f2;
if( _temp282 == Cyc_CfFlowInfo_AllIL){ goto _LL277;} else{ goto _LL278;}} else{
goto _LL278;} _LL278: if((( struct _enum_struct*) _temp274)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL289: _temp288=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp274)->f1;
goto _LL287; _LL287: _temp286=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp274)->f2; goto _LL279;} else{ goto _LL280;} _LL280: if((( struct
_enum_struct*) _temp274)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL291:
_temp290=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp274)->f1; goto _LL281;} else{ goto _LL275;} _LL277: return 1; _LL279:
return 0; _LL281: { struct _tuple6 _temp294; struct Cyc_List_List* _temp295;
struct Cyc_List_List** _temp297; struct Cyc_Dict_Dict* _temp298; struct _tuple6*
_temp292= env; _temp294=* _temp292; _LL299: _temp298= _temp294.f1; goto _LL296;
_LL296: _temp295= _temp294.f2; _temp297=&(* _temp292).f2; goto _LL293; _LL293:
if((( int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp297, _temp290)){ return 1;}* _temp297=({ struct Cyc_List_List* _temp300=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp300[
0]=({ struct Cyc_List_List _temp301; _temp301.hd=( void*) _temp290; _temp301.tl=*
_temp297; _temp301;}); _temp300;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp298, _temp290));} _LL275:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp302= path; void* _temp310; struct Cyc_Dict_Dict* _temp312; struct Cyc_Dict_Dict*
_temp314; _LL304: if((( struct _enum_struct*) _temp302)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL311: _temp310=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp302)->f1;
goto _LL305;} else{ goto _LL306;} _LL306: if((( struct _enum_struct*) _temp302)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL313: _temp312=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp302)->f1; goto _LL307;} else{ goto
_LL308;} _LL308: if((( struct _enum_struct*) _temp302)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL315: _temp314=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp302)->f1; goto _LL309;} else{ goto _LL303;} _LL305: return Cyc_CfFlowInfo_isAllInit_is(
env, _temp310); _LL307: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp312); _LL309: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_string*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp314); _LL303:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 isInitEnv=({ struct _tuple6 _temp316;
_temp316.f1= pinfo_dict; _temp316.f2= 0; _temp316;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, pinfo);} void* Cyc_CfFlowInfo_pinfo_getplaces_esc( struct _tuple7*
env, void* pinfo){ struct _tuple7 _temp319; struct Cyc_Set_Set** _temp320;
struct Cyc_Dict_Dict* _temp322; struct _tuple7* _temp317= env; _temp319=*
_temp317; _LL323: _temp322= _temp319.f1; goto _LL321; _LL321: _temp320= _temp319.f2;
goto _LL318; _LL318: { void* _temp324= pinfo; void* _temp334; struct Cyc_CfFlowInfo_Place*
_temp336; void* _temp338; void* _temp340; void* _temp342; struct Cyc_Dict_Dict*
_temp344; struct Cyc_Dict_Dict* _temp346; _LL326: if((( struct _enum_struct*)
_temp324)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL335: _temp334=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp324)->f1; if((( struct _enum_struct*)
_temp334)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL337: _temp336=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp334)->f1; goto _LL327;} else{
goto _LL328;}} else{ goto _LL328;} _LL328: if((( struct _enum_struct*) _temp324)->tag
== Cyc_CfFlowInfo_LeafPI_tag){ _LL339: _temp338=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp324)->f1; if((( struct _enum_struct*) _temp338)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL343: _temp342=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp338)->f1;
goto _LL341; _LL341: _temp340=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp338)->f2; goto _LL329;} else{ goto _LL330;}} else{ goto _LL330;} _LL330:
if((( struct _enum_struct*) _temp324)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL345: _temp344=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp324)->f1; goto _LL331;} else{ goto _LL332;} _LL332: if((( struct
_enum_struct*) _temp324)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL347: _temp346=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp324)->f1;
goto _LL333;} else{ goto _LL325;} _LL327:* _temp320=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
_temp320, _temp336); if( Cyc_CfFlowInfo_isAllInit( _temp322, Cyc_CfFlowInfo_lookup_place(
_temp322, _temp336))){ return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc, Cyc_CfFlowInfo_AllIL);}
return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc, Cyc_CfFlowInfo_ThisIL);
_LL329: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc, _temp340); _LL331:
return({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp348=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp348->tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp348->f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp344);( void*) _temp348;}); _LL333: return({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp349=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp349->tag= Cyc_CfFlowInfo_StructPI_tag; _temp349->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp346);( void*)
_temp349;}); _LL325:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set**
places_ptr, void* a, void* pinfo){ void* _temp350= pinfo; void* _temp360; void*
_temp362; void* _temp364; void* _temp366; struct Cyc_CfFlowInfo_Place* _temp368;
struct Cyc_Dict_Dict* _temp370; struct Cyc_Dict_Dict* _temp372; _LL352: if(((
struct _enum_struct*) _temp350)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL361:
_temp360=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp350)->f1; if(((
struct _enum_struct*) _temp360)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL365:
_temp364=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp360)->f1; goto
_LL363; _LL363: _temp362=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp360)->f2; goto _LL353;} else{ goto _LL354;}} else{ goto _LL354;} _LL354:
if((( struct _enum_struct*) _temp350)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL367:
_temp366=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp350)->f1; if(((
struct _enum_struct*) _temp366)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL369:
_temp368=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp366)->f1; goto _LL355;} else{ goto _LL356;}} else{ goto _LL356;} _LL356:
if((( struct _enum_struct*) _temp350)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL371: _temp370=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp350)->f1; goto _LL357;} else{ goto _LL358;} _LL358: if((( struct
_enum_struct*) _temp350)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL373: _temp372=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp350)->f1;
goto _LL359;} else{ goto _LL351;} _LL353: return; _LL355:* places_ptr=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
places_ptr, _temp368); return; _LL357:(( void(*)( void(* f)( struct Cyc_Set_Set**,
int, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp370); return; _LL359:(( void(*)( void(* f)( struct Cyc_Set_Set**,
struct _tagged_string*, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, struct
_tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr,
_temp372); return; _LL351:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_Set_Set* places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct
Cyc_Set_Set* s)) Cyc_Set_is_empty)( places)) { struct Cyc_CfFlowInfo_Place*
place=(( struct Cyc_CfFlowInfo_Place*(*)( struct Cyc_Set_Set* s)) Cyc_Set_choose)(
places); places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_delete)( places, place);{ void* place_old_pinfo;{ struct
_handler_cons _temp374; _push_handler(& _temp374);{ struct _xenum_struct*
_temp375=( struct _xenum_struct*) setjmp( _temp374.handler); if( ! _temp375){
place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, place);; _pop_handler();} else{
struct _xenum_struct* _temp377= _temp375; _LL379: if( _temp377->tag == Cyc_Dict_Absent_tag){
goto _LL380;} else{ goto _LL381;} _LL381: goto _LL382; _LL380: continue; _LL382:(
void) _throw( _temp377); _LL378:;}}}{ struct _tuple7 places_env=({ struct
_tuple7 _temp383; _temp383.f1= d; _temp383.f2=& places; _temp383;}); void*
place_new_pinfo= Cyc_CfFlowInfo_pinfo_getplaces_esc(& places_env,
place_old_pinfo); if( place_old_pinfo != place_new_pinfo){ d= Cyc_CfFlowInfo_insert_place(
d, place, place_new_pinfo);}}}} return d;} static struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr=
0; struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict*
d){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp384=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp384[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp384;});}{ struct Cyc_Set_Set* places=* Cyc_CfFlowInfo_empty_place_set_ptr;((
void(*)( struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
places, 0, pinfo); return Cyc_CfFlowInfo_escape_these( places, d);}} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp385= pinfo; void* _temp395; void* _temp397;
void* _temp399; void* _temp401; struct Cyc_Dict_Dict* _temp403; struct Cyc_Dict_Dict*
_temp405; _LL387: if((( struct _enum_struct*) _temp385)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL396: _temp395=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp385)->f1;
if((( struct _enum_struct*) _temp395)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL400: _temp399=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp395)->f1;
if( _temp399 == Cyc_CfFlowInfo_Esc){ goto _LL398;} else{ goto _LL389;} _LL398:
_temp397=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp395)->f2; goto
_LL388;} else{ goto _LL389;}} else{ goto _LL389;} _LL389: if((( struct
_enum_struct*) _temp385)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL402: _temp401=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp385)->f1; goto _LL390;}
else{ goto _LL391;} _LL391: if((( struct _enum_struct*) _temp385)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL404: _temp403=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp385)->f1; goto _LL392;} else{ goto _LL393;} _LL393: if((( struct
_enum_struct*) _temp385)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL406: _temp405=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp385)->f1;
goto _LL394;} else{ goto _LL386;} _LL388: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc,
level); _LL390: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc, level);
_LL392: return({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp407=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp407->tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp407->f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp403);( void*) _temp407;}); _LL394: return({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp408=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp408->tag= Cyc_CfFlowInfo_StructPI_tag; _temp408->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_assign_unknown_dict, level, _temp405);( void*) _temp408;});
_LL386:;} struct Cyc_CfFlowInfo_JoinEnv{ struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict*
d2; struct Cyc_Set_Set** places; } ; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t;
void* Cyc_CfFlowInfo_join_escape( void* esc1, void* esc2){ struct _tuple3
_temp410=({ struct _tuple3 _temp409; _temp409.f1= esc1; _temp409.f2= esc2;
_temp409;}); void* _temp418; void* _temp420; void* _temp422; void* _temp424;
_LL412: _LL421: _temp420= _temp410.f1; if( _temp420 == Cyc_CfFlowInfo_Esc){ goto
_LL419;} else{ goto _LL414;} _LL419: _temp418= _temp410.f2; goto _LL413; _LL414:
_LL425: _temp424= _temp410.f1; goto _LL423; _LL423: _temp422= _temp410.f2; if(
_temp422 == Cyc_CfFlowInfo_Esc){ goto _LL415;} else{ goto _LL416;} _LL416: goto
_LL417; _LL413: goto _LL415; _LL415: return Cyc_CfFlowInfo_Esc; _LL417: return
Cyc_CfFlowInfo_Unesc; _LL411:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1,
void* il2){ struct _tuple3 _temp427=({ struct _tuple3 _temp426; _temp426.f1= il1;
_temp426.f2= il2; _temp426;}); void* _temp439; void* _temp441; void* _temp443;
void* _temp445; void* _temp447; void* _temp449; void* _temp451; void* _temp453;
_LL429: _LL442: _temp441= _temp427.f1; if( _temp441 == Cyc_CfFlowInfo_NoneIL){
goto _LL440;} else{ goto _LL431;} _LL440: _temp439= _temp427.f2; goto _LL430;
_LL431: _LL446: _temp445= _temp427.f1; goto _LL444; _LL444: _temp443= _temp427.f2;
if( _temp443 == Cyc_CfFlowInfo_NoneIL){ goto _LL432;} else{ goto _LL433;} _LL433:
_LL450: _temp449= _temp427.f1; if( _temp449 == Cyc_CfFlowInfo_ThisIL){ goto
_LL448;} else{ goto _LL435;} _LL448: _temp447= _temp427.f2; goto _LL434; _LL435:
_LL454: _temp453= _temp427.f1; goto _LL452; _LL452: _temp451= _temp427.f2; if(
_temp451 == Cyc_CfFlowInfo_ThisIL){ goto _LL436;} else{ goto _LL437;} _LL437:
goto _LL438; _LL430: goto _LL432; _LL432: return Cyc_CfFlowInfo_NoneIL; _LL434:
goto _LL436; _LL436: return Cyc_CfFlowInfo_ThisIL; _LL438: return Cyc_CfFlowInfo_AllIL;
_LL428:;} void* Cyc_CfFlowInfo_join_initstate( struct Cyc_CfFlowInfo_JoinEnv*
env, void* state1, void* state2){ struct _tuple3 _temp456=({ struct _tuple3
_temp455; _temp455.f1= state1; _temp455.f2= state2; _temp455;}); void* _temp466;
void* _temp468; void* _temp470; void* _temp472; void* _temp474; void* _temp476;
void* _temp478; struct Cyc_CfFlowInfo_Place* _temp480; void* _temp482; void*
_temp484; void* _temp486; void* _temp488; void* _temp490; void* _temp492; void*
_temp494; struct Cyc_CfFlowInfo_Place* _temp496; void* _temp498; struct Cyc_CfFlowInfo_Place*
_temp500; void* _temp502; struct Cyc_CfFlowInfo_Place* _temp504; _LL458: _LL473:
_temp472= _temp456.f1; if((( struct _enum_struct*) _temp472)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL477: _temp476=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp472)->f1;
goto _LL475; _LL475: _temp474=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp472)->f2; goto _LL467;} else{ goto _LL460;} _LL467: _temp466= _temp456.f2;
if((( struct _enum_struct*) _temp466)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL471: _temp470=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp466)->f1;
goto _LL469; _LL469: _temp468=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp466)->f2; goto _LL459;} else{ goto _LL460;} _LL460: _LL483: _temp482=
_temp456.f1; if((( struct _enum_struct*) _temp482)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL487: _temp486=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp482)->f1;
goto _LL485; _LL485: _temp484=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp482)->f2; goto _LL479;} else{ goto _LL462;} _LL479: _temp478= _temp456.f2;
if((( struct _enum_struct*) _temp478)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL481: _temp480=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp478)->f1; goto _LL461;} else{ goto _LL462;} _LL462: _LL495: _temp494=
_temp456.f1; if((( struct _enum_struct*) _temp494)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL497: _temp496=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp494)->f1; goto _LL489;} else{ goto _LL464;} _LL489: _temp488= _temp456.f2;
if((( struct _enum_struct*) _temp488)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL493: _temp492=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp488)->f1;
goto _LL491; _LL491: _temp490=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp488)->f2; goto _LL463;} else{ goto _LL464;} _LL464: _LL503: _temp502=
_temp456.f1; if((( struct _enum_struct*) _temp502)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL505: _temp504=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp502)->f1; goto _LL499;} else{ goto _LL457;} _LL499: _temp498= _temp456.f2;
if((( struct _enum_struct*) _temp498)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL501: _temp500=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp498)->f1; goto _LL465;} else{ goto _LL457;} _LL459: return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_join_escape( _temp476, _temp470), Cyc_CfFlowInfo_join_initlevel(
_temp474, _temp468)); _LL461: { struct _tuple6 isInitEnv=({ struct _tuple6
_temp506; _temp506.f1= env->d2; _temp506.f2= 0; _temp506;}); if( _temp484 == Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d2,
_temp480)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp480);
return Cyc_CfFlowInfo_mkLeafPI( _temp486, _temp484);} return Cyc_CfFlowInfo_mkLeafPI(
_temp486, Cyc_CfFlowInfo_join_initlevel( _temp484, Cyc_CfFlowInfo_ThisIL));}
_LL463: { struct _tuple6 isInitEnv=({ struct _tuple6 _temp507; _temp507.f1= env->d1;
_temp507.f2= 0; _temp507;}); if( _temp490 == Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d1, _temp496)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp496); return Cyc_CfFlowInfo_mkLeafPI(
_temp492, _temp490);} return Cyc_CfFlowInfo_mkLeafPI( _temp492, Cyc_CfFlowInfo_join_initlevel(
Cyc_CfFlowInfo_ThisIL, _temp490));} _LL465: if( Cyc_CfFlowInfo_place_cmp(
_temp504, _temp500) == 0){ return({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp508=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp508->tag= Cyc_CfFlowInfo_LeafPI_tag; _temp508->f1=( void*) state1;( void*)
_temp508;});}{ struct _tuple6 isInitEnv1=({ struct _tuple6 _temp509; _temp509.f1=
env->d1; _temp509.f2= 0; _temp509;}); struct _tuple6 isInitEnv2=({ struct
_tuple6 _temp510; _temp510.f1= env->d2; _temp510.f2= 0; _temp510;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv1, Cyc_CfFlowInfo_lookup_place( env->d1, _temp504))? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv2, Cyc_CfFlowInfo_lookup_place( env->d2, _temp500)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp504);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp500); return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc, Cyc_CfFlowInfo_AllIL);}
return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Unesc, Cyc_CfFlowInfo_ThisIL);}
_LL457:;} void* Cyc_CfFlowInfo_join_pathinfo( struct Cyc_CfFlowInfo_JoinEnv* env,
void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return pinfo1;}{ struct
_tuple3 _temp512=({ struct _tuple3 _temp511; _temp511.f1= pinfo1; _temp511.f2=
pinfo2; _temp511;}); void* _temp522; void* _temp524; void* _temp526; void*
_temp528; void* _temp530; struct Cyc_Dict_Dict* _temp532; void* _temp534; struct
Cyc_Dict_Dict* _temp536; void* _temp538; struct Cyc_Dict_Dict* _temp540; void*
_temp542; struct Cyc_Dict_Dict* _temp544; _LL514: _LL527: _temp526= _temp512.f1;
if((( struct _enum_struct*) _temp526)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL529:
_temp528=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp526)->f1; goto
_LL523;} else{ goto _LL516;} _LL523: _temp522= _temp512.f2; if((( struct
_enum_struct*) _temp522)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL525: _temp524=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp522)->f1; goto _LL515;}
else{ goto _LL516;} _LL516: _LL535: _temp534= _temp512.f1; if((( struct
_enum_struct*) _temp534)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL537: _temp536=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp534)->f1;
goto _LL531;} else{ goto _LL518;} _LL531: _temp530= _temp512.f2; if((( struct
_enum_struct*) _temp530)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL533: _temp532=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp530)->f1;
goto _LL517;} else{ goto _LL518;} _LL518: _LL543: _temp542= _temp512.f1; if(((
struct _enum_struct*) _temp542)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL545:
_temp544=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp542)->f1; goto _LL539;} else{ goto _LL520;} _LL539: _temp538= _temp512.f2;
if((( struct _enum_struct*) _temp538)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL541: _temp540=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp538)->f1; goto _LL519;} else{ goto _LL520;} _LL520: goto _LL521; _LL515:
if( _temp528 == _temp524){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp528, _temp524); _LL517: if( _temp536 == _temp532){ return pinfo1;}
return({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp546=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp546->tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp546->f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp536, _temp532);( void*) _temp546;}); _LL519: if( _temp544 == _temp540){
return pinfo1;} return({ struct Cyc_CfFlowInfo_StructPI_struct* _temp547=(
struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp547->tag= Cyc_CfFlowInfo_StructPI_tag; _temp547->f1=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp544, _temp540);( void*) _temp547;});
_LL521:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp548=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp548->tag= Cyc_Core_Impossible_tag; _temp548->f1=( struct _tagged_string)({
char* _temp549=( char*)"join_pathinfo"; struct _tagged_string _temp550; _temp550.curr=
_temp549; _temp550.base= _temp549; _temp550.last_plus_one= _temp549 + 14;
_temp550;});( struct _xenum_struct*) _temp548;})); _LL513:;}} void* Cyc_CfFlowInfo_join_flow(
void* f1, void* f2){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp551=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp551[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp551;});} if( f1 == f2){ return f1;}{ struct _tuple3 _temp553=({ struct
_tuple3 _temp552; _temp552.f1= f1; _temp552.f2= f2; _temp552;}); void* _temp561;
void* _temp563; void* _temp565; void* _temp567; void* _temp569; struct Cyc_Dict_Dict*
_temp571; void* _temp573; struct Cyc_Dict_Dict* _temp575; _LL555: _LL564:
_temp563= _temp553.f1; if( _temp563 == Cyc_CfFlowInfo_BottomFL){ goto _LL562;}
else{ goto _LL557;} _LL562: _temp561= _temp553.f2; goto _LL556; _LL557: _LL568:
_temp567= _temp553.f1; goto _LL566; _LL566: _temp565= _temp553.f2; if( _temp565
== Cyc_CfFlowInfo_BottomFL){ goto _LL558;} else{ goto _LL559;} _LL559: _LL574:
_temp573= _temp553.f1; if(( unsigned int) _temp573 > 1u?(( struct _enum_struct*)
_temp573)->tag == Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL576: _temp575=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp573)->f1; goto _LL570;} else{ goto
_LL554;} _LL570: _temp569= _temp553.f2; if(( unsigned int) _temp569 > 1u?((
struct _enum_struct*) _temp569)->tag == Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL572:
_temp571=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp569)->f1; goto _LL560;} else{ goto _LL554;} _LL556: return f2; _LL558:
return f1; _LL560: if( _temp575 == _temp571){ return f1;}{ struct Cyc_Set_Set*
places=* Cyc_CfFlowInfo_empty_place_set_ptr; struct Cyc_CfFlowInfo_JoinEnv
joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp577; _temp577.d1= _temp575;
_temp577.d2= _temp571; _temp577.places=& places; _temp577;}); struct Cyc_Dict_Dict*
outdict=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
joinenv, _temp575, _temp571); return({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp578=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp578->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp578->f1= Cyc_CfFlowInfo_escape_these(
places, outdict);( void*) _temp578;});} _LL554:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx(
void* state1, void* state2){ struct _tuple3 _temp580=({ struct _tuple3 _temp579;
_temp579.f1= state1; _temp579.f2= state2; _temp579;}); void* _temp588; void*
_temp590; void* _temp592; void* _temp594; void* _temp596; void* _temp598; void*
_temp600; struct Cyc_CfFlowInfo_Place* _temp602; void* _temp604; struct Cyc_CfFlowInfo_Place*
_temp606; _LL582: _LL595: _temp594= _temp580.f1; if((( struct _enum_struct*)
_temp594)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL599: _temp598=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp594)->f1; goto _LL597; _LL597:
_temp596=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp594)->f2; goto
_LL589;} else{ goto _LL584;} _LL589: _temp588= _temp580.f2; if((( struct
_enum_struct*) _temp588)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL593: _temp592=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp588)->f1; goto _LL591;
_LL591: _temp590=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp588)->f2;
goto _LL583;} else{ goto _LL584;} _LL584: _LL605: _temp604= _temp580.f1; if(((
struct _enum_struct*) _temp604)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL607:
_temp606=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp604)->f1; goto _LL601;} else{ goto _LL586;} _LL601: _temp600= _temp580.f2;
if((( struct _enum_struct*) _temp600)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL603: _temp602=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp600)->f1; goto _LL585;} else{ goto _LL586;} _LL586: goto _LL587; _LL583:
return Cyc_CfFlowInfo_join_escape( _temp598, _temp592) == _temp592? Cyc_CfFlowInfo_join_initlevel(
_temp596, _temp590) == _temp590: 0; _LL585: return Cyc_CfFlowInfo_place_cmp(
_temp606, _temp602) == 0; _LL587: return 0; _LL581:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp609=({ struct _tuple3 _temp608; _temp608.f1= pinfo1;
_temp608.f2= pinfo2; _temp608;}); void* _temp619; void* _temp621; void* _temp623;
void* _temp625; void* _temp627; struct Cyc_Dict_Dict* _temp629; void* _temp631;
struct Cyc_Dict_Dict* _temp633; void* _temp635; struct Cyc_Dict_Dict* _temp637;
void* _temp639; struct Cyc_Dict_Dict* _temp641; _LL611: _LL624: _temp623=
_temp609.f1; if((( struct _enum_struct*) _temp623)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL626: _temp625=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp623)->f1;
goto _LL620;} else{ goto _LL613;} _LL620: _temp619= _temp609.f2; if((( struct
_enum_struct*) _temp619)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL622: _temp621=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp619)->f1; goto _LL612;}
else{ goto _LL613;} _LL613: _LL632: _temp631= _temp609.f1; if((( struct
_enum_struct*) _temp631)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL634: _temp633=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp631)->f1;
goto _LL628;} else{ goto _LL615;} _LL628: _temp627= _temp609.f2; if((( struct
_enum_struct*) _temp627)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL630: _temp629=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp627)->f1;
goto _LL614;} else{ goto _LL615;} _LL615: _LL640: _temp639= _temp609.f1; if(((
struct _enum_struct*) _temp639)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL642:
_temp641=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp639)->f1; goto _LL636;} else{ goto _LL617;} _LL636: _temp635= _temp609.f2;
if((( struct _enum_struct*) _temp635)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL638: _temp637=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp635)->f1; goto _LL616;} else{ goto _LL617;} _LL617: goto _LL618; _LL612:
return _temp625 == _temp621? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp625, _temp621); _LL614: return _temp633 == _temp629? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp633, _temp629); _LL616: return _temp641 == _temp637? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp641, _temp637);
_LL618:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp643=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp643->tag= Cyc_Core_Impossible_tag; _temp643->f1=( struct _tagged_string)({
char* _temp644=( char*)"pinfo_lessthan"; struct _tagged_string _temp645;
_temp645.curr= _temp644; _temp645.base= _temp644; _temp645.last_plus_one=
_temp644 + 15; _temp645;});( struct _xenum_struct*) _temp643;})); _LL610:;}} int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2){ if( f1 == f2){ return
1;}{ struct _tuple3 _temp647=({ struct _tuple3 _temp646; _temp646.f1= f1;
_temp646.f2= f2; _temp646;}); void* _temp655; void* _temp657; void* _temp659;
void* _temp661; void* _temp663; struct Cyc_Dict_Dict* _temp665; void* _temp667;
struct Cyc_Dict_Dict* _temp669; _LL649: _LL658: _temp657= _temp647.f1; if(
_temp657 == Cyc_CfFlowInfo_BottomFL){ goto _LL656;} else{ goto _LL651;} _LL656:
_temp655= _temp647.f2; goto _LL650; _LL651: _LL662: _temp661= _temp647.f1; goto
_LL660; _LL660: _temp659= _temp647.f2; if( _temp659 == Cyc_CfFlowInfo_BottomFL){
goto _LL652;} else{ goto _LL653;} _LL653: _LL668: _temp667= _temp647.f1; if((
unsigned int) _temp667 > 1u?(( struct _enum_struct*) _temp667)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL670: _temp669=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp667)->f1; goto _LL664;} else{ goto _LL648;} _LL664: _temp663= _temp647.f2;
if(( unsigned int) _temp663 > 1u?(( struct _enum_struct*) _temp663)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL666: _temp665=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp663)->f1; goto _LL654;} else{ goto _LL648;} _LL650: return 1; _LL652:
return 0; _LL654: if( _temp669 == _temp665){ return 1;} return(( int(*)( int(* f)(
void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp665, _temp669); _LL648:;}}