#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct _tagged_string* f1; struct Cyc_List_List* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ struct
_tagged_string* f1; void* f2; } ; struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; struct _tuple8{ struct _tagged_string* f1; struct
Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; struct _tuple9{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4;
char f5; int f6; } ; struct _tuple10{ struct _tuple1* f1; void* f2; struct Cyc_Absyn_Exp*
f3; } ; struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Stmt*
f3; } ; struct _tuple14{ struct _tuple1* f1; void* f2; } ; struct _tuple15{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple16{ struct
_tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; struct _tuple17{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; } ;
struct _tuple18{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;
} ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
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
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern void Cyc_List_iter_c( void(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_revappend(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_flatten( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strconcat( struct _tagged_string, struct _tagged_string); extern
struct _tagged_string Cyc_String_implode( struct Cyc_List_List* c); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex; extern
void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
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
; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple1* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple1* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct _tuple1*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple1*, void*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple1*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_string*
v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple1*); extern struct _tagged_string Cyc_Absynpp_prim2string( void* p);
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
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern struct Cyc_Core_Opt*
Cyc_Formatstr_parse_conversionspecification( struct _tagged_string s, int i);
extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List* ds); extern int
Cyc_Toc_check_for_null; static struct Cyc_Absyn_Abs_n_struct _temp0={ 1u, 0};
void* Cyc_Toc_abs_ns=( struct Cyc_Absyn_Abs_n_struct*)& _temp0; struct Cyc_Absyn_Tqual
Cyc_Toc_mt_tq_v=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict=
0}; struct Cyc_Absyn_Tqual* Cyc_Toc_mt_tq=& Cyc_Toc_mt_tq_v; int Cyc_Toc_check_for_null=
0; char Cyc_Toc_Toc_Unimplemented_tag[ 18u]="Toc_Unimplemented"; struct Cyc_Toc_Toc_Unimplemented_struct{
char* tag; } ; char Cyc_Toc_Toc_Impossible_tag[ 15u]="Toc_Impossible"; struct
Cyc_Toc_Toc_Impossible_struct{ char* tag; } ; static void* Cyc_Toc_unimp( struct
_tagged_string s){({ struct _tagged_string _temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp1.last_plus_one - _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Toc_Toc_Unimplemented_struct*
_temp2=( struct Cyc_Toc_Toc_Unimplemented_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Toc_Toc_Unimplemented_struct)); _temp2->tag= Cyc_Toc_Toc_Unimplemented_tag;(
struct _xenum_struct*) _temp2;}));} static void* Cyc_Toc_toc_impos( struct
_tagged_string s){({ struct _tagged_string _temp3= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp3.last_plus_one - _temp3.curr, _temp3.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Toc_Toc_Impossible_struct*
_temp4=( struct Cyc_Toc_Toc_Impossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Toc_Impossible_struct)); _temp4->tag= Cyc_Toc_Toc_Impossible_tag;(
struct _xenum_struct*) _temp4;}));} char Cyc_Toc_Match_error_tag[ 12u]="Match_error";
struct Cyc_Toc_Match_error_struct{ char* tag; } ; static char _temp7[ 5u]="curr";
static struct _tagged_string Cyc_Toc_curr_string=( struct _tagged_string){
_temp7, _temp7, _temp7 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static char _temp10[ 14u]="last_plus_one"; static struct
_tagged_string Cyc_Toc_last_plus_one_string=( struct _tagged_string){ _temp10,
_temp10, _temp10 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp13[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string=( struct _tagged_string){ _temp13, _temp13,
_temp13 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string;
static char _temp16[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string=(
struct _tagged_string){ _temp16, _temp16, _temp16 + 15u}; static struct
_tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static char _temp19[ 13u]="_enum_struct"; static struct _tagged_string Cyc_Toc__enum_struct_string=(
struct _tagged_string){ _temp19, _temp19, _temp19 + 13u}; static struct
_tagged_string* Cyc_Toc__enum_struct_sp=& Cyc_Toc__enum_struct_string; static
char _temp22[ 14u]="_xenum_struct"; static struct _tagged_string Cyc_Toc__xenum_struct_string=(
struct _tagged_string){ _temp22, _temp22, _temp22 + 14u}; static struct
_tagged_string* Cyc_Toc__xenum_struct_sp=& Cyc_Toc__xenum_struct_string; static
char _temp25[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=( struct
_tagged_string){ _temp25, _temp25, _temp25 + 2u}; static struct _tagged_string*
Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp28[ 4u]="tag"; static struct
_tagged_string Cyc_Toc_tag_string=( struct _tagged_string){ _temp28, _temp28,
_temp28 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _temp31[ 14u]="_handler_cons"; static struct _tagged_string Cyc_Toc__handler_cons_string=(
struct _tagged_string){ _temp31, _temp31, _temp31 + 14u}; static struct
_tagged_string* Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string; static
char _temp34[ 7u]="setjmp"; static struct _tagged_string Cyc_Toc_setjmp_string=(
struct _tagged_string){ _temp34, _temp34, _temp34 + 7u}; static struct
_tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string; static char _temp37[
14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp37, _temp37, _temp37 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp40[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp40, _temp40, _temp40 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp43[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp43, _temp43, _temp43 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp46[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp46, _temp46, _temp46 + 14u}; static struct Cyc_Absyn_Abs_n_struct
_temp47={ 1u, 0}; static struct _tuple1 Cyc_Toc__npop_handler_pr=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp47,.f2=& Cyc_Toc__npop_handler_string};
static struct _tuple1* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static char _temp50[ 16u]="Match_Exception"; static struct _tagged_string Cyc_Toc_Match_Exception_string=(
struct _tagged_string){ _temp50, _temp50, _temp50 + 16u}; static struct
_tagged_string* Cyc_Toc_Match_Exception_sp=& Cyc_Toc_Match_Exception_string;
static char _temp53[ 15u]="Cyc_new_string"; static struct _tagged_string Cyc_Toc_Cyc_new_string_string=(
struct _tagged_string){ _temp53, _temp53, _temp53 + 15u}; static struct
_tagged_string* Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string;
static int Cyc_Toc_is_nullable( void* t){ if( ! Cyc_Toc_check_for_null){ return
0;}{ void* _temp54= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp60;
struct Cyc_Absyn_Conref* _temp62; struct Cyc_Absyn_Tqual* _temp64; struct Cyc_Absyn_Conref*
_temp66; void* _temp68; void* _temp70; _LL56: if(( unsigned int) _temp54 > 4u?((
struct _enum_struct*) _temp54)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL61:
_temp60=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp54)->f1; _LL71: _temp70=( void*) _temp60.elt_typ; goto _LL69; _LL69:
_temp68=( void*) _temp60.rgn_typ; goto _LL67; _LL67: _temp66=( struct Cyc_Absyn_Conref*)
_temp60.nullable; goto _LL65; _LL65: _temp64=( struct Cyc_Absyn_Tqual*) _temp60.tq;
goto _LL63; _LL63: _temp62=( struct Cyc_Absyn_Conref*) _temp60.bounds; goto
_LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL57: { void* _temp72=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp66))->v; int _temp78; _LL74: if(( unsigned int) _temp72 > 1u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL79: _temp78=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp72)->f1; goto _LL75;} else{ goto
_LL76;} _LL76: goto _LL77; _LL75: return _temp78; _LL77: return 0; _LL73:;}
_LL59:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp80=( char*)"is_nullable"; struct _tagged_string
_temp81; _temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one=
_temp80 + 12; _temp81;})); return 0; _LL55:;}} static int Cyc_Toc_really_is_nullable(
void* t){ int b= Cyc_Toc_check_for_null; Cyc_Toc_check_for_null= 1;{ int ans=
Cyc_Toc_is_nullable( t); Cyc_Toc_check_for_null= b; return ans;}} static struct
_tuple1* Cyc_Toc_collapse_qvar_tag( struct _tuple1* x, struct _tagged_string tag){
return({ struct _tuple1* _temp82=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp82->f1=(* x).f1; _temp82->f2=({ struct _tagged_string* _temp83=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp83[
0]= Cyc_String_strconcat(*(* x).f2, tag); _temp83;}); _temp82;});} static struct
Cyc_List_List* Cyc_Toc_added_decls= 0; static void Cyc_Toc_add_tuple_decl(
struct _tagged_string* x, struct Cyc_List_List* ts){ struct Cyc_List_List* fs= 0;{
int i= 1; for( 0; ts != 0; ts= ts->tl, i ++){ fs=({ struct Cyc_List_List*
_temp84=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp84->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp85=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp85->name=({ struct
_tagged_string* _temp86=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp86[ 0]= xprintf("f%d", i); _temp86;}); _temp85->tq=
Cyc_Toc_mt_tq; _temp85->type=( void*)(( void*) ts->hd); _temp85->attributes= 0;
_temp85;}); _temp84->tl= fs; _temp84;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl* _temp87=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp87->sc=( void*) Cyc_Absyn_Public;
_temp87->name=({ struct Cyc_Core_Opt* _temp89=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp89->v=( void*)({ struct _tuple1* _temp90=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp90->f1= Cyc_Toc_abs_ns;
_temp90->f2= x; _temp90;}); _temp89;}); _temp87->tvs= 0; _temp87->fields=({
struct Cyc_Core_Opt* _temp88=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp88->v=( void*) fs; _temp88;}); _temp87->attributes= 0;
_temp87;}); Cyc_Toc_added_decls=({ struct Cyc_List_List* _temp91=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp91->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp92=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp92->tag= Cyc_Absyn_Struct_d_tag;
_temp92->f1= sd;( void*) _temp92;}), 0); _temp91->tl= Cyc_Toc_added_decls;
_temp91;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp93=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp93->name= Cyc_Toc_curr_sp; _temp93->tq= Cyc_Toc_mt_tq; _temp93->type=( void*)
tptr; _temp93->attributes= 0; _temp93;}); struct Cyc_Absyn_Structfield* f2=({
struct Cyc_Absyn_Structfield* _temp94=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp94->name= Cyc_Toc_base_sp; _temp94->tq=
Cyc_Toc_mt_tq; _temp94->type=( void*) tptr; _temp94->attributes= 0; _temp94;});
struct Cyc_Absyn_Structfield* f3=({ struct Cyc_Absyn_Structfield* _temp95=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp95->name= Cyc_Toc_last_plus_one_sp; _temp95->tq= Cyc_Toc_mt_tq; _temp95->type=(
void*) tptr; _temp95->attributes= 0; _temp95;}); struct Cyc_List_List* fs=({
struct Cyc_List_List* _temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp96->hd=( void*) f1; _temp96->tl=({ struct Cyc_List_List*
_temp97=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp97->hd=( void*) f2; _temp97->tl=({ struct Cyc_List_List* _temp98=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp98->hd=( void*)
f3; _temp98->tl= 0; _temp98;}); _temp97;}); _temp96;}); struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl* _temp99=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp99->sc=( void*) Cyc_Absyn_Public;
_temp99->name=({ struct Cyc_Core_Opt* _temp101=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp101->v=( void*)({ struct _tuple1* _temp102=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp102->f1= Cyc_Toc_abs_ns;
_temp102->f2= x; _temp102;}); _temp101;}); _temp99->tvs= 0; _temp99->fields=({
struct Cyc_Core_Opt* _temp100=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp100->v=( void*)(( struct Cyc_List_List*) fs); _temp100;});
_temp99->attributes= 0; _temp99;}); Cyc_Toc_added_decls=({ struct Cyc_List_List*
_temp103=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp103->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp104=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp104->tag= Cyc_Absyn_Struct_d_tag; _temp104->f1= sd;( void*) _temp104;}), 0);
_temp103->tl= Cyc_Toc_added_decls; _temp103;});} static struct Cyc_List_List*
Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter= 0; static struct
_tagged_string* Cyc_Toc_add_tuple_type( struct Cyc_List_List* tqs0){{ struct Cyc_List_List*
tts= Cyc_Toc_tuple_types; for( 0; tts != 0; tts= tts->tl){ struct _tuple4
_temp107; struct Cyc_List_List* _temp108; struct _tagged_string* _temp110;
struct _tuple4* _temp105=( struct _tuple4*) tts->hd; _temp107=* _temp105; _LL111:
_temp110= _temp107.f1; goto _LL109; _LL109: _temp108= _temp107.f2; goto _LL106;
_LL106: { int okay= 1; struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0?
_temp108 != 0: 0; tqs= tqs->tl, _temp108= _temp108->tl){ if( ! Cyc_Tcutil_unify((*((
struct _tuple5*) tqs->hd)).f2,( void*) _temp108->hd)){ okay= 0; break;}} if(( !
okay? 1: tqs != 0)? 1: _temp108 != 0){ continue;} return _temp110;}}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp112=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp112[ 0]= xprintf("_tuple%d",(
Cyc_Toc_tuple_type_counter ++)); _temp112;}); struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple5*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts);
Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp113->hd=( void*)({ struct
_tuple4* _temp114=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp114->f1= x; _temp114->f2= ts; _temp114;}); _temp113->tl= Cyc_Toc_tuple_types;
_temp113;}); return x;}} static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types=
0; static int Cyc_Toc_tagged_ptr_counter= 0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type(
void* t, struct Cyc_Absyn_Tqual* tq){{ void* _temp115= t; void* _temp121; void*
_temp123; _LL117: if(( unsigned int) _temp115 > 4u?(( struct _enum_struct*)
_temp115)->tag == Cyc_Absyn_IntType_tag: 0){ _LL124: _temp123=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp115)->f1; if( _temp123 == Cyc_Absyn_Unsigned){
goto _LL122;} else{ goto _LL119;} _LL122: _temp121=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp115)->f2; if( _temp121 == Cyc_Absyn_B1){ goto _LL118;} else{ goto _LL119;}}
else{ goto _LL119;} _LL119: goto _LL120; _LL118: return Cyc_Toc__tagged_string_sp;
_LL120: goto _LL116; _LL116:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts= tts->tl){ struct _tuple6 _temp127; void* _temp128; struct
_tagged_string* _temp130; struct _tuple6* _temp125=( struct _tuple6*) tts->hd;
_temp127=* _temp125; _LL131: _temp130= _temp127.f1; goto _LL129; _LL129:
_temp128= _temp127.f2; goto _LL126; _LL126: if( ! Cyc_Tcutil_unify( t, _temp128)){
continue;} return _temp130;}}{ struct _tagged_string* x=({ struct _tagged_string*
_temp132=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp132[ 0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter ++));
_temp132;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp133=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp133->hd=( void*)({ struct _tuple6* _temp134=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp134->f1= x; _temp134->f2=
t; _temp134;}); _temp133->tl= Cyc_Toc_tagged_ptr_types; _temp133;}); return x;}}
static int Cyc_Toc_temp_var_counter= 0; static struct _tuple1* Cyc_Toc_temp_var(){
return({ struct _tuple1* _temp135=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp135->f1= Cyc_Toc_abs_ns; _temp135->f2=({ struct _tagged_string*
_temp136=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp136[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp136;});
_temp135;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp137=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp137[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp137;});} static struct
Cyc_List_List* Cyc_Toc_temp_topdecls= 0; static char _temp140[ 10u]="_LongLong";
static struct _tagged_string Cyc_Toc_long_long_string=( struct _tagged_string){
_temp140, _temp140, _temp140 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp141={ 1u, 0}; static struct _tuple1 Cyc_Toc_longlong_v=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp141,.f2=& Cyc_Toc_long_long_string};
static struct Cyc_Absyn_TypedefType_struct _temp142={ 12u,& Cyc_Toc_longlong_v,
0, 0}; static void* Cyc_Toc_longlong_typ=( struct Cyc_Absyn_TypedefType_struct*)&
_temp142; static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple2* Cyc_Toc_arg_to_c(
struct _tuple2* a){ void* _temp145; struct Cyc_Absyn_Tqual* _temp147; struct Cyc_Core_Opt*
_temp149; struct _tuple2 _temp143=* a; _LL150: _temp149= _temp143.f1; goto
_LL148; _LL148: _temp147= _temp143.f2; goto _LL146; _LL146: _temp145= _temp143.f3;
goto _LL144; _LL144: return({ struct _tuple2* _temp151=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp151->f1= _temp149; _temp151->f2=
_temp147; _temp151->f3= Cyc_Toc_typ_to_c( _temp145); _temp151;});} static struct
_tuple5* Cyc_Toc_typ_to_c_f( struct _tuple5* x){ void* _temp154; struct Cyc_Absyn_Tqual*
_temp156; struct _tuple5 _temp152=* x; _LL157: _temp156= _temp152.f1; goto
_LL155; _LL155: _temp154= _temp152.f2; goto _LL153; _LL153: return({ struct
_tuple5* _temp158=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp158->f1= _temp156; _temp158->f2= Cyc_Toc_typ_to_c( _temp154); _temp158;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp159= t; struct Cyc_Absyn_Exp*
_temp167; struct Cyc_Absyn_Tqual* _temp169; void* _temp171; int _temp173; struct
Cyc_Core_Opt* _temp175; struct Cyc_Core_Opt _temp177; void* _temp178; void*
_temp180; _LL161: if(( unsigned int) _temp159 > 4u?(( struct _enum_struct*)
_temp159)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL172: _temp171=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp159)->f1; goto _LL170; _LL170: _temp169=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp159)->f2;
goto _LL168; _LL168: _temp167=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp159)->f3; goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int)
_temp159 > 4u?(( struct _enum_struct*) _temp159)->tag == Cyc_Absyn_Evar_tag: 0){
_LL181: _temp180=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp159)->f1; goto
_LL176; _LL176: _temp175=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp159)->f2; if( _temp175 == 0){ goto _LL165;} else{ _temp177=* _temp175;
_LL179: _temp178=( void*) _temp177.v; goto _LL174;} _LL174: _temp173=( int)((
struct Cyc_Absyn_Evar_struct*) _temp159)->f3; goto _LL164;} else{ goto _LL165;}
_LL165: goto _LL166; _LL162: return({ struct Cyc_Absyn_ArrayType_struct*
_temp182=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp182->tag= Cyc_Absyn_ArrayType_tag; _temp182->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp171); _temp182->f2= _temp169; _temp182->f3= _temp167;( void*) _temp182;});
_LL164: return Cyc_Toc_typ_to_c_array( _temp178); _LL166: return Cyc_Toc_typ_to_c(
t); _LL160:;} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp183= t; int
_temp225; struct Cyc_Core_Opt* _temp227; void* _temp229; int _temp231; struct
Cyc_Core_Opt* _temp233; struct Cyc_Core_Opt _temp235; void* _temp236; void*
_temp238; struct Cyc_Absyn_Tvar* _temp240; struct Cyc_Absyn_Enumdecl** _temp242;
struct Cyc_List_List* _temp244; struct _tuple1* _temp246; struct Cyc_Absyn_Xenumdecl**
_temp248; struct _tuple1* _temp250; struct Cyc_Absyn_PtrInfo _temp252; struct
Cyc_Absyn_Conref* _temp254; struct Cyc_Absyn_Tqual* _temp256; struct Cyc_Absyn_Conref*
_temp258; void* _temp260; void* _temp262; void* _temp264; void* _temp266; struct
Cyc_Absyn_Exp* _temp268; struct Cyc_Absyn_Tqual* _temp270; void* _temp272;
struct Cyc_Absyn_FnInfo _temp274; struct Cyc_List_List* _temp276; int _temp278;
struct Cyc_List_List* _temp280; void* _temp282; struct Cyc_Core_Opt* _temp284;
struct Cyc_List_List* _temp286; struct Cyc_List_List* _temp288; struct Cyc_Absyn_Uniondecl**
_temp290; struct Cyc_List_List* _temp292; struct _tuple1* _temp294; struct Cyc_Absyn_Structdecl**
_temp296; struct Cyc_List_List* _temp298; struct _tuple1* _temp300; struct Cyc_Core_Opt*
_temp302; struct Cyc_List_List* _temp304; struct _tuple1* _temp306; void*
_temp308; void* _temp310; struct Cyc_List_List* _temp312; _LL185: if( _temp183
== Cyc_Absyn_VoidType){ goto _LL186;} else{ goto _LL187;} _LL187: if((
unsigned int) _temp183 > 4u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_Evar_tag:
0){ _LL230: _temp229=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f1;
goto _LL228; _LL228: _temp227=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp183)->f2; if( _temp227 == 0){ goto _LL226;} else{ goto _LL189;} _LL226:
_temp225=( int)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f3; goto _LL188;}
else{ goto _LL189;} _LL189: if(( unsigned int) _temp183 > 4u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_Evar_tag: 0){ _LL239: _temp238=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp183)->f1; goto _LL234; _LL234: _temp233=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f2; if(
_temp233 == 0){ goto _LL191;} else{ _temp235=* _temp233; _LL237: _temp236=( void*)
_temp235.v; goto _LL232;} _LL232: _temp231=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp183)->f3; goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp183 > 4u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_VarType_tag:
0){ _LL241: _temp240=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp183)->f1; goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp183 > 4u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL247: _temp246=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp183)->f1; goto _LL245; _LL245: _temp244=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp183)->f2; goto _LL243; _LL243: _temp242=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp183)->f3;
goto _LL194;} else{ goto _LL195;} _LL195: if(( unsigned int) _temp183 > 4u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL251:
_temp250=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp183)->f1;
goto _LL249; _LL249: _temp248=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp183)->f2; goto _LL196;} else{ goto _LL197;} _LL197: if(( unsigned int)
_temp183 > 4u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL253: _temp252=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp183)->f1; _LL263: _temp262=( void*) _temp252.elt_typ; goto _LL261; _LL261:
_temp260=( void*) _temp252.rgn_typ; goto _LL259; _LL259: _temp258=( struct Cyc_Absyn_Conref*)
_temp252.nullable; goto _LL257; _LL257: _temp256=( struct Cyc_Absyn_Tqual*)
_temp252.tq; goto _LL255; _LL255: _temp254=( struct Cyc_Absyn_Conref*) _temp252.bounds;
goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int) _temp183 > 4u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_IntType_tag: 0){ _LL267:
_temp266=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f1; goto _LL265;
_LL265: _temp264=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f2;
goto _LL200;} else{ goto _LL201;} _LL201: if( _temp183 == Cyc_Absyn_FloatType){
goto _LL202;} else{ goto _LL203;} _LL203: if( _temp183 == Cyc_Absyn_DoubleType){
goto _LL204;} else{ goto _LL205;} _LL205: if(( unsigned int) _temp183 > 4u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL273:
_temp272=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp183)->f1; goto
_LL271; _LL271: _temp270=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp183)->f2; goto _LL269; _LL269: _temp268=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp183)->f3; goto _LL206;} else{ goto _LL207;}
_LL207: if(( unsigned int) _temp183 > 4u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL275: _temp274=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp183)->f1; _LL287: _temp286=( struct Cyc_List_List*)
_temp274.tvars; goto _LL285; _LL285: _temp284=( struct Cyc_Core_Opt*) _temp274.effect;
goto _LL283; _LL283: _temp282=( void*) _temp274.ret_typ; goto _LL281; _LL281:
_temp280=( struct Cyc_List_List*) _temp274.args; goto _LL279; _LL279: _temp278=(
int) _temp274.varargs; goto _LL277; _LL277: _temp276=( struct Cyc_List_List*)
_temp274.attributes; goto _LL208;} else{ goto _LL209;} _LL209: if(( unsigned int)
_temp183 > 4u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL289: _temp288=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp183)->f1; goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp183 > 4u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL295: _temp294=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp183)->f1; goto _LL293; _LL293: _temp292=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp183)->f2; goto _LL291; _LL291: _temp290=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp183)->f3;
goto _LL212;} else{ goto _LL213;} _LL213: if(( unsigned int) _temp183 > 4u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_StructType_tag: 0){ _LL301:
_temp300=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp183)->f1;
goto _LL299; _LL299: _temp298=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp183)->f2; goto _LL297; _LL297: _temp296=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp183)->f3; goto _LL214;} else{ goto
_LL215;} _LL215: if(( unsigned int) _temp183 > 4u?(( struct _enum_struct*)
_temp183)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL307: _temp306=( struct
_tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp183)->f1; goto _LL305;
_LL305: _temp304=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp183)->f2; goto _LL303; _LL303: _temp302=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp183)->f3; goto _LL216;} else{ goto _LL217;}
_LL217: if(( unsigned int) _temp183 > 4u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL309: _temp308=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp183)->f1; goto _LL218;} else{ goto _LL219;} _LL219: if( _temp183 == Cyc_Absyn_HeapRgn){
goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp183 > 4u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL311:
_temp310=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp183)->f1; goto
_LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp183 > 4u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL313: _temp312=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp183)->f1; goto
_LL224;} else{ goto _LL184;} _LL186: return t; _LL188: return Cyc_Absyn_VoidType;
_LL190: return Cyc_Toc_typ_to_c( _temp236); _LL192: return Cyc_Absyn_void_star_typ();
_LL194: if( _temp246 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp314=( char*)"anonymous enum"; struct
_tagged_string _temp315; _temp315.curr= _temp314; _temp315.base= _temp314;
_temp315.last_plus_one= _temp314 + 15; _temp315;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp316=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp316->tag= Cyc_Absyn_TypedefType_tag; _temp316->f1=( struct _tuple1*)({
struct _tuple1* _temp317= _temp246; if( _temp317 == 0){ _throw( Null_Exception);}
_temp317;}); _temp316->f2= 0; _temp316->f3=({ struct Cyc_Core_Opt* _temp318=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp318->v=(
void*) Cyc_Absyn_void_star_typ(); _temp318;});( void*) _temp316;}); _LL196:
return({ struct Cyc_Absyn_TypedefType_struct* _temp319=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp319->tag= Cyc_Absyn_TypedefType_tag;
_temp319->f1= _temp250; _temp319->f2= 0; _temp319->f3=({ struct Cyc_Core_Opt*
_temp320=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp320->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp),
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp320;});( void*) _temp319;}); _LL198:
_temp262= Cyc_Toc_typ_to_c_array( _temp262);{ void* _temp321=( void*) _temp254->v;
void* _temp327; _LL323: if(( unsigned int) _temp321 > 1u?(( struct _enum_struct*)
_temp321)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL328: _temp327=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp321)->f1; if( _temp327 == Cyc_Absyn_Unknown_b){
goto _LL324;} else{ goto _LL325;}} else{ goto _LL325;} _LL325: goto _LL326;
_LL324: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp262,
_temp256); return Cyc_Absyn_strct( n);} _LL326: return Cyc_Absyn_star_typ(
_temp262, Cyc_Absyn_HeapRgn, _temp256); _LL322:;} _LL200: return t; _LL202:
return t; _LL204: return t; _LL206: return({ struct Cyc_Absyn_ArrayType_struct*
_temp329=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp329->tag= Cyc_Absyn_ArrayType_tag; _temp329->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp272); _temp329->f2= _temp270; _temp329->f3= _temp268;( void*) _temp329;});
_LL208: return({ struct Cyc_Absyn_FnType_struct* _temp330=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp330->tag= Cyc_Absyn_FnType_tag;
_temp330->f1=({ struct Cyc_Absyn_FnInfo _temp331; _temp331.tvars= 0; _temp331.effect=
0; _temp331.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp282); _temp331.args=((
struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp280); _temp331.varargs= _temp278;
_temp331.attributes= _temp276; _temp331;});( void*) _temp330;}); _LL210:
_temp288=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp288);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp288); return Cyc_Absyn_strct( n);}
_LL212: if( _temp294 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp332=( char*)"anonymous union"; struct
_tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 16; _temp333;}));} return Cyc_Absyn_unionq_typ((
struct _tuple1*)({ struct _tuple1* _temp334= _temp294; if( _temp334 == 0){
_throw( Null_Exception);} _temp334;})); _LL214: if( _temp300 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp335=( char*)"anonymous struct"; struct _tagged_string _temp336; _temp336.curr=
_temp335; _temp336.base= _temp335; _temp336.last_plus_one= _temp335 + 17;
_temp336;}));} return Cyc_Absyn_strctq(( struct _tuple1*)({ struct _tuple1*
_temp337= _temp300; if( _temp337 == 0){ _throw( Null_Exception);} _temp337;}));
_LL216: if( _temp302 == 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp338=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 38; _temp339;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp340=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp340->tag= Cyc_Absyn_TypedefType_tag; _temp340->f1= _temp306; _temp340->f2=
0; _temp340->f3=({ struct Cyc_Core_Opt* _temp341=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp341->v=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp302->v); _temp341;});( void*) _temp340;}); _LL218: return Cyc_Absyn_void_star_typ();
_LL220: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp342=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 55; _temp343;})); _LL222: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp344=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 54; _temp345;})); _LL224: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp346=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp347; _temp347.curr= _temp346; _temp347.base= _temp346;
_temp347.last_plus_one= _temp346 + 52; _temp347;})); _LL184:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp348= t; struct Cyc_Absyn_Exp* _temp354; struct Cyc_Absyn_Tqual*
_temp356; void* _temp358; _LL350: if(( unsigned int) _temp348 > 4u?(( struct
_enum_struct*) _temp348)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL359: _temp358=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp348)->f1; goto _LL357; _LL357:
_temp356=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp348)->f2; goto _LL355; _LL355: _temp354=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp348)->f3; goto _LL351;} else{ goto _LL352;}
_LL352: goto _LL353; _LL351: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp358, Cyc_Absyn_HeapRgn, _temp356), e, l); _LL353: return e; _LL349:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp360= t; struct Cyc_Absyn_Exp*
_temp366; struct Cyc_Absyn_Tqual* _temp368; void* _temp370; _LL362: if((
unsigned int) _temp360 > 4u?(( struct _enum_struct*) _temp360)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL371: _temp370=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp360)->f1;
goto _LL369; _LL369: _temp368=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp360)->f2; goto _LL367; _LL367: _temp366=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp360)->f3; goto _LL363;} else{ goto _LL364;}
_LL364: goto _LL365; _LL363: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp370, Cyc_Absyn_HeapRgn, _temp368), e, l); _LL365: return Cyc_Absyn_cast_exp(
t, e, l); _LL361:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp372= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp404; void* _temp406; void* _temp408; struct Cyc_Absyn_Exp*
_temp410; struct Cyc_Absyn_Tqual* _temp412; void* _temp414; struct Cyc_Absyn_FnInfo
_temp416; struct Cyc_List_List* _temp418; struct Cyc_Absyn_Structdecl** _temp420;
struct Cyc_List_List* _temp422; struct _tuple1* _temp424; struct Cyc_Absyn_Enumdecl**
_temp426; struct Cyc_List_List* _temp428; struct _tuple1* _temp430; struct Cyc_Absyn_Xenumdecl**
_temp432; struct _tuple1* _temp434; struct Cyc_Absyn_Uniondecl** _temp436;
struct Cyc_List_List* _temp438; struct _tuple1* _temp440; struct Cyc_Absyn_PtrInfo
_temp442; void* _temp444; _LL374: if( _temp372 == Cyc_Absyn_VoidType){ goto
_LL375;} else{ goto _LL376;} _LL376: if(( unsigned int) _temp372 > 4u?(( struct
_enum_struct*) _temp372)->tag == Cyc_Absyn_VarType_tag: 0){ _LL405: _temp404=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp372)->f1; goto
_LL377;} else{ goto _LL378;} _LL378: if(( unsigned int) _temp372 > 4u?(( struct
_enum_struct*) _temp372)->tag == Cyc_Absyn_IntType_tag: 0){ _LL409: _temp408=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp372)->f1; goto _LL407; _LL407:
_temp406=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp372)->f2; goto _LL379;}
else{ goto _LL380;} _LL380: if( _temp372 == Cyc_Absyn_FloatType){ goto _LL381;}
else{ goto _LL382;} _LL382: if( _temp372 == Cyc_Absyn_DoubleType){ goto _LL383;}
else{ goto _LL384;} _LL384: if(( unsigned int) _temp372 > 4u?(( struct
_enum_struct*) _temp372)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL415: _temp414=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp372)->f1; goto _LL413; _LL413:
_temp412=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp372)->f2; goto _LL411; _LL411: _temp410=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp372)->f3; goto _LL385;} else{ goto _LL386;}
_LL386: if(( unsigned int) _temp372 > 4u?(( struct _enum_struct*) _temp372)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL417: _temp416=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp372)->f1; goto _LL387;} else{ goto _LL388;}
_LL388: if(( unsigned int) _temp372 > 4u?(( struct _enum_struct*) _temp372)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL419: _temp418=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp372)->f1; goto _LL389;} else{ goto
_LL390;} _LL390: if(( unsigned int) _temp372 > 4u?(( struct _enum_struct*)
_temp372)->tag == Cyc_Absyn_StructType_tag: 0){ _LL425: _temp424=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp372)->f1; goto _LL423;
_LL423: _temp422=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp372)->f2; goto _LL421; _LL421: _temp420=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp372)->f3; goto _LL391;} else{ goto
_LL392;} _LL392: if(( unsigned int) _temp372 > 4u?(( struct _enum_struct*)
_temp372)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL431: _temp430=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp372)->f1; goto _LL429; _LL429: _temp428=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp372)->f2; goto
_LL427; _LL427: _temp426=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp372)->f3; goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int)
_temp372 > 4u?(( struct _enum_struct*) _temp372)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL435: _temp434=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp372)->f1; goto _LL433; _LL433: _temp432=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp372)->f2; goto _LL395;} else{ goto
_LL396;} _LL396: if(( unsigned int) _temp372 > 4u?(( struct _enum_struct*)
_temp372)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL441: _temp440=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp372)->f1; goto _LL439; _LL439: _temp438=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp372)->f2; goto
_LL437; _LL437: _temp436=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp372)->f3; goto _LL397;} else{ goto _LL398;} _LL398: if(( unsigned int)
_temp372 > 4u?(( struct _enum_struct*) _temp372)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL443: _temp442=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp372)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int)
_temp372 > 4u?(( struct _enum_struct*) _temp372)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL445: _temp444=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp372)->f1;
goto _LL401;} else{ goto _LL402;} _LL402: goto _LL403; _LL375: return 1; _LL377:
return 0; _LL379: return 1; _LL381: return 1; _LL383: return 0; _LL385: return
Cyc_Toc_atomic_typ( _temp414); _LL387: return 1; _LL389: for( 0; _temp418 != 0;
_temp418= _temp418->tl){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple5*) _temp418->hd)).f2)){
return 0;}} return 1; _LL391: return 0; _LL393: return 0; _LL395: return 0;
_LL397: return 0; _LL399: return 0; _LL401: return 0; _LL403:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp446= Cyc_Absynpp_typ2string(
t); xprintf("atomic_typ:  bad type %.*s", _temp446.last_plus_one - _temp446.curr,
_temp446.curr);})); return 0; _LL373:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp447= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp453;
struct Cyc_Absyn_Conref* _temp455; struct Cyc_Absyn_Tqual* _temp457; struct Cyc_Absyn_Conref*
_temp459; void* _temp461; void* _temp463; _LL449: if(( unsigned int) _temp447 >
4u?(( struct _enum_struct*) _temp447)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL454: _temp453=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp447)->f1; _LL464: _temp463=( void*) _temp453.elt_typ; goto _LL462; _LL462:
_temp461=( void*) _temp453.rgn_typ; goto _LL460; _LL460: _temp459=( struct Cyc_Absyn_Conref*)
_temp453.nullable; goto _LL458; _LL458: _temp457=( struct Cyc_Absyn_Tqual*)
_temp453.tq; goto _LL456; _LL456: _temp455=( struct Cyc_Absyn_Conref*) _temp453.bounds;
goto _LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL450: { void* _temp465=
Cyc_Tcutil_compress( _temp463); _LL467: if( _temp465 == Cyc_Absyn_VoidType){
goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL468: return 1; _LL470:
return 0; _LL466:;} _LL452: return 0; _LL448:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp471= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp483; struct Cyc_Absyn_Structdecl* _temp485;
struct Cyc_List_List* _temp486; struct _tuple1* _temp488; struct Cyc_Absyn_Structdecl**
_temp490; struct Cyc_List_List* _temp492; struct _tuple1* _temp494; struct Cyc_Absyn_Uniondecl**
_temp496; struct Cyc_Absyn_Uniondecl* _temp498; struct Cyc_List_List* _temp499;
struct _tuple1* _temp501; struct Cyc_Absyn_Uniondecl** _temp503; struct Cyc_List_List*
_temp505; struct _tuple1* _temp507; _LL473: if(( unsigned int) _temp471 > 4u?((
struct _enum_struct*) _temp471)->tag == Cyc_Absyn_StructType_tag: 0){ _LL489:
_temp488=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp471)->f1;
goto _LL487; _LL487: _temp486=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp471)->f2; goto _LL484; _LL484: _temp483=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp471)->f3; if( _temp483 == 0){ goto
_LL475;} else{ _temp485=* _temp483; goto _LL474;}} else{ goto _LL475;} _LL475:
if(( unsigned int) _temp471 > 4u?(( struct _enum_struct*) _temp471)->tag == Cyc_Absyn_StructType_tag:
0){ _LL495: _temp494=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp471)->f1; goto _LL493; _LL493: _temp492=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp471)->f2; goto _LL491; _LL491: _temp490=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp471)->f3;
if( _temp490 == 0){ goto _LL476;} else{ goto _LL477;}} else{ goto _LL477;}
_LL477: if(( unsigned int) _temp471 > 4u?(( struct _enum_struct*) _temp471)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL502: _temp501=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp471)->f1; goto _LL500; _LL500: _temp499=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp471)->f2; goto _LL497; _LL497: _temp496=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp471)->f3;
if( _temp496 == 0){ goto _LL479;} else{ _temp498=* _temp496; goto _LL478;}}
else{ goto _LL479;} _LL479: if(( unsigned int) _temp471 > 4u?(( struct
_enum_struct*) _temp471)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL508: _temp507=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp471)->f1; goto
_LL506; _LL506: _temp505=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp471)->f2; goto _LL504; _LL504: _temp503=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp471)->f3; if( _temp503 == 0){ goto
_LL480;} else{ goto _LL481;}} else{ goto _LL481;} _LL481: goto _LL482; _LL474:
if( _temp488 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp509=( char*)"StructType name not properly set";
struct _tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 33; _temp510;}));} if( _temp485->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp512= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp511=
_temp488; if( _temp511 == 0){ _throw( Null_Exception);} _temp511;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp512.last_plus_one - _temp512.curr, _temp512.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp485->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)
fields->hd)->name) == 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp514= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp513= _temp488; if( _temp513 == 0){ _throw( Null_Exception);}
_temp513;})); struct _tagged_string _temp515=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp514.last_plus_one - _temp514.curr, _temp514.curr, _temp515.last_plus_one -
_temp515.curr, _temp515.curr);})); return 0;} _LL476: if( _temp494 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp516=( char*)"StructType name not properly set"; struct _tagged_string
_temp517; _temp517.curr= _temp516; _temp517.base= _temp516; _temp517.last_plus_one=
_temp516 + 33; _temp517;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp519= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp518= _temp494; if( _temp518 == 0){ _throw( Null_Exception);}
_temp518;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp519.last_plus_one
- _temp519.curr, _temp519.curr);})); return 0; _LL478: if( _temp501 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp520=( char*)"UnionType name not properly set"; struct _tagged_string
_temp521; _temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 32; _temp521;}));} if( _temp498->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp523= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp522= _temp501; if( _temp522 == 0){
_throw( Null_Exception);} _temp522;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp523.last_plus_one - _temp523.curr, _temp523.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp498->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)
fields->hd)->name) == 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp525= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp524= _temp501; if( _temp524 == 0){ _throw( Null_Exception);}
_temp524;})); struct _tagged_string _temp526=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp525.last_plus_one - _temp525.curr, _temp525.curr, _temp526.last_plus_one -
_temp526.curr, _temp526.curr);})); return 0;} _LL480: if( _temp507 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp527=( char*)"UnionType name not properly set"; struct _tagged_string
_temp528; _temp528.curr= _temp527; _temp528.base= _temp527; _temp528.last_plus_one=
_temp527 + 32; _temp528;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp530= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp529= _temp507; if( _temp529 == 0){ _throw( Null_Exception);}
_temp529;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp530.last_plus_one
- _temp530.curr, _temp530.curr);})); return 0; _LL482:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp531= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp531.last_plus_one - _temp531.curr,
_temp531.curr);})); return 0; _LL472:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp532=( void*) e->r; struct _tagged_string*
_temp540; struct Cyc_Absyn_Exp* _temp542; struct _tagged_string* _temp544;
struct Cyc_Absyn_Exp* _temp546; _LL534: if((( struct _enum_struct*) _temp532)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL543: _temp542=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp532)->f1; goto _LL541; _LL541:
_temp540=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp532)->f2; goto _LL535;} else{ goto _LL536;} _LL536: if((( struct
_enum_struct*) _temp532)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL547: _temp546=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp532)->f1;
goto _LL545; _LL545: _temp544=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp532)->f2; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539; _LL535:
return Cyc_Toc_is_poly_field(( void*)( _temp542->topt)->v, _temp540); _LL537: {
void* _temp548= Cyc_Tcutil_compress(( void*)( _temp546->topt)->v); struct Cyc_Absyn_PtrInfo
_temp554; struct Cyc_Absyn_Conref* _temp556; struct Cyc_Absyn_Tqual* _temp558;
struct Cyc_Absyn_Conref* _temp560; void* _temp562; void* _temp564; _LL550: if((
unsigned int) _temp548 > 4u?(( struct _enum_struct*) _temp548)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL555: _temp554=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp548)->f1; _LL565: _temp564=( void*) _temp554.elt_typ; goto _LL563; _LL563:
_temp562=( void*) _temp554.rgn_typ; goto _LL561; _LL561: _temp560=( struct Cyc_Absyn_Conref*)
_temp554.nullable; goto _LL559; _LL559: _temp558=( struct Cyc_Absyn_Tqual*)
_temp554.tq; goto _LL557; _LL557: _temp556=( struct Cyc_Absyn_Conref*) _temp554.bounds;
goto _LL551;} else{ goto _LL552;} _LL552: goto _LL553; _LL551: return Cyc_Toc_is_poly_field(
_temp564, _temp544); _LL553:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp566= Cyc_Absynpp_typ2string(( void*)( _temp546->topt)->v);
xprintf("is_poly_project:  bad type %.*s", _temp566.last_plus_one - _temp566.curr,
_temp566.curr);})); return 0; _LL549:;} _LL539: return 0; _LL533:;} static char
_temp569[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp569, _temp569, _temp569 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp570={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp570,.f2=& Cyc_Toc_gc_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var, 0),({
struct Cyc_List_List* _temp571=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp571->hd=( void*) s; _temp571->tl= 0; _temp571;}), 0);}
static char _temp574[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp574, _temp574, _temp574 + 17u}; static struct Cyc_Absyn_Abs_n_struct
_temp575={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp575,.f2=& Cyc_Toc_gc_malloc_atomic_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var, 0),({
struct Cyc_List_List* _temp576=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp576->hd=( void*) s; _temp576->tl= 0; _temp576;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp578=({ struct _tuple0
_temp577; _temp577.f1= t1; _temp577.f2= t2; _temp577;}); void* _temp584; struct
Cyc_Absyn_PtrInfo _temp586; void* _temp588; struct Cyc_Absyn_PtrInfo _temp590;
_LL580: _LL589: _temp588= _temp578.f1; if(( unsigned int) _temp588 > 4u?((
struct _enum_struct*) _temp588)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL591:
_temp590=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp588)->f1; goto _LL585;} else{ goto _LL582;} _LL585: _temp584= _temp578.f2;
if(( unsigned int) _temp584 > 4u?(( struct _enum_struct*) _temp584)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL587: _temp586=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp584)->f1; goto _LL581;} else{ goto _LL582;} _LL582: goto _LL583; _LL581: {
int n1=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp590.nullable);
int n2=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp586.nullable);
void* b1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp590.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp586.bounds); struct Cyc_List_List* convs= 0;{ struct _tuple0 _temp593=({
struct _tuple0 _temp592; _temp592.f1= b1; _temp592.f2= b2; _temp592;}); void*
_temp603; struct Cyc_Absyn_Exp* _temp605; void* _temp607; struct Cyc_Absyn_Exp*
_temp609; void* _temp611; void* _temp613; void* _temp615; void* _temp617; struct
Cyc_Absyn_Exp* _temp619; void* _temp621; struct Cyc_Absyn_Exp* _temp623; void*
_temp625; _LL595: _LL608: _temp607= _temp593.f1; if(( unsigned int) _temp607 > 1u?((
struct _enum_struct*) _temp607)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL610:
_temp609=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp607)->f1;
goto _LL604;} else{ goto _LL597;} _LL604: _temp603= _temp593.f2; if((
unsigned int) _temp603 > 1u?(( struct _enum_struct*) _temp603)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL606: _temp605=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp603)->f1; goto _LL596;} else{ goto _LL597;} _LL597: _LL614: _temp613=
_temp593.f1; if( _temp613 == Cyc_Absyn_Unknown_b){ goto _LL612;} else{ goto
_LL599;} _LL612: _temp611= _temp593.f2; if( _temp611 == Cyc_Absyn_Unknown_b){
goto _LL598;} else{ goto _LL599;} _LL599: _LL618: _temp617= _temp593.f1; if((
unsigned int) _temp617 > 1u?(( struct _enum_struct*) _temp617)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL620: _temp619=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp617)->f1; goto _LL616;} else{ goto _LL601;} _LL616: _temp615= _temp593.f2;
if( _temp615 == Cyc_Absyn_Unknown_b){ goto _LL600;} else{ goto _LL601;} _LL601:
_LL626: _temp625= _temp593.f1; if( _temp625 == Cyc_Absyn_Unknown_b){ goto _LL622;}
else{ goto _LL594;} _LL622: _temp621= _temp593.f2; if(( unsigned int) _temp621 >
1u?(( struct _enum_struct*) _temp621)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL624:
_temp623=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp621)->f1;
goto _LL602;} else{ goto _LL594;} _LL596: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List*
_temp627=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp627->hd=( void*)({ struct Cyc_Toc_NullCheck_struct* _temp628=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp628->tag= Cyc_Toc_NullCheck_tag;
_temp628->f1=( void*) t2;( void*) _temp628;}); _temp627->tl= convs; _temp627;});}
goto _LL594; _LL598: goto _LL594; _LL600: convs=({ struct Cyc_List_List*
_temp629=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp629->hd=( void*)({ struct Cyc_Toc_TagPtr_struct* _temp630=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp630->tag= Cyc_Toc_TagPtr_tag;
_temp630->f1=( void*) Cyc_Toc_typ_to_c( t1); _temp630->f2= _temp619;( void*)
_temp630;}); _temp629->tl= convs; _temp629;}); goto _LL594; _LL602: convs=({
struct Cyc_List_List* _temp631=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp631->hd=( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp632=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp632->tag= Cyc_Toc_UntagPtr_tag; _temp632->f1=( void*) Cyc_Toc_typ_to_c( t1);
_temp632->f2= _temp623; _temp632->f3= n2;( void*) _temp632;}); _temp631->tl=
convs; _temp631;}); goto _LL594; _LL594:;} return convs;} _LL583: return 0;
_LL579:;}} static char _temp635[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp635, _temp635, _temp635 + 7u}; static struct Cyc_Absyn_Abs_n_struct
_temp636={ 1u, 0}; static struct _tuple1 Cyc_Toc__throw_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp636,.f2=& Cyc_Toc__throw_string}; static
struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({ struct Cyc_List_List* _temp637=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp637->hd=(
void*) e; _temp637->tl= 0; _temp637;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c(
t); if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp638=( char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 44; _temp639;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp640= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp646; struct Cyc_Absyn_Conref* _temp648; struct Cyc_Absyn_Tqual*
_temp650; struct Cyc_Absyn_Conref* _temp652; void* _temp654; void* _temp656;
_LL642: if(( unsigned int) _temp640 > 4u?(( struct _enum_struct*) _temp640)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL647: _temp646=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp640)->f1; _LL657: _temp656=( void*)
_temp646.elt_typ; goto _LL655; _LL655: _temp654=( void*) _temp646.rgn_typ; goto
_LL653; _LL653: _temp652=( struct Cyc_Absyn_Conref*) _temp646.nullable; goto
_LL651; _LL651: _temp650=( struct Cyc_Absyn_Tqual*) _temp646.tq; goto _LL649;
_LL649: _temp648=( struct Cyc_Absyn_Conref*) _temp646.bounds; goto _LL643;}
else{ goto _LL644;} _LL644: goto _LL645; _LL643: elt_typ= _temp656;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp656, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL641;} _LL645: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp658=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp659;
_temp659.curr= _temp658; _temp659.base= _temp658; _temp659.last_plus_one=
_temp658 + 28; _temp659;})); _LL641:;} if( toplevel){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp660=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp660->tag= Cyc_Absyn_ArrayType_tag;
_temp660->f1=( void*) elt_typ; _temp660->f2= Cyc_Toc_mt_tq; _temp660->f3= sz;(
void*) _temp660;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp661=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp661->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp662=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp662->tag= Cyc_Absyn_Var_d_tag; _temp662->f1= vd;( void*) _temp662;}), 0);
_temp661->tl= Cyc_Toc_temp_topdecls; _temp661;});{ struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp663=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp663->hd=( void*)({ struct _tuple7* _temp668=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp668->f1= 0; _temp668->f2= xexp; _temp668;});
_temp663->tl=({ struct Cyc_List_List* _temp664=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp664->hd=( void*)({ struct
_tuple7* _temp667=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp667->f1= 0; _temp667->f2= xexp; _temp667;}); _temp664->tl=({ struct Cyc_List_List*
_temp665=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp665->hd=( void*)({ struct _tuple7* _temp666=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp666->f1= 0; _temp666->f2= xplussz; _temp666;});
_temp665->tl= 0; _temp665;}); _temp664;}); _temp663;}), 0); return urm_exp;}}
else{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
b, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_signed_int_exp( szvalue, 0), 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( b, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_var_exp(
a, 0), 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( b, 0), Cyc_Toc_curr_sp, 0), Cyc_Absyn_var_exp( a, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( b, strt_typ, 0, s, 0); s= Cyc_Absyn_declare_stmt(
a, Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq),( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, e, 0), s, 0); return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp*
Cyc_Toc_untag_ptr_conv( int toplevel, void* t1, struct Cyc_Absyn_Exp* sz, int
nullable, struct Cyc_Absyn_Exp* e){ if( toplevel){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp669=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp670; _temp670.curr= _temp669; _temp670.base= _temp669; _temp670.last_plus_one=
_temp669 + 45; _temp670;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
szvalue=( int) Cyc_Evexp_eval_const_uint_exp( sz); struct Cyc_Absyn_Exp* xcurr=
Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xcurr, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_add_exp( xcurr, Cyc_Absyn_signed_int_exp( szvalue, 0), 0); struct
Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_gt_exp( e1, e2, 0); struct Cyc_Absyn_Exp*
e4= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp( xcurr, Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), e3, 0); struct Cyc_Absyn_Exp* e5= ! nullable? e4:
Cyc_Absyn_and_exp( Cyc_Absyn_neq_exp( xcurr, Cyc_Absyn_uint_exp( 0, 0), 0), e4,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt( e5, Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt(
0), 0), s, 0); s= Cyc_Absyn_declare_stmt( x, t1,( struct Cyc_Absyn_Exp*) e, s, 0);
return Cyc_Absyn_stmt_exp( s, 0);}} static struct Cyc_Absyn_Exp* Cyc_Toc_convert_exp(
int toplevel, void* t1, void* t2, struct Cyc_Absyn_Exp* e){{ struct Cyc_List_List*
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs= cs->tl){ void* _temp671=(
void*) cs->hd; void* _temp679; struct Cyc_Absyn_Exp* _temp681; void* _temp683;
int _temp685; struct Cyc_Absyn_Exp* _temp687; void* _temp689; _LL673: if(((
struct _enum_struct*) _temp671)->tag == Cyc_Toc_NullCheck_tag){ _LL680: _temp679=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp671)->f1; goto _LL674;} else{
goto _LL675;} _LL675: if((( struct _enum_struct*) _temp671)->tag == Cyc_Toc_TagPtr_tag){
_LL684: _temp683=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp671)->f1; goto
_LL682; _LL682: _temp681=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp671)->f2; goto _LL676;} else{ goto _LL677;} _LL677: if((( struct
_enum_struct*) _temp671)->tag == Cyc_Toc_UntagPtr_tag){ _LL690: _temp689=( void*)((
struct Cyc_Toc_UntagPtr_struct*) _temp671)->f1; goto _LL688; _LL688: _temp687=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*) _temp671)->f2; goto
_LL686; _LL686: _temp685=( int)(( struct Cyc_Toc_UntagPtr_struct*) _temp671)->f3;
goto _LL678;} else{ goto _LL672;} _LL674: e= Cyc_Toc_null_check_conv( toplevel,
_temp679, e); goto _LL672; _LL676: e= Cyc_Toc_tag_ptr_conv( toplevel, _temp683,
_temp681, e); goto _LL672; _LL678: e= Cyc_Toc_untag_ptr_conv( toplevel, _temp689,
_temp687, _temp685, e); goto _LL672; _LL672:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp691=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp691->break_lab= 0; _temp691->continue_lab= 0; _temp691->fallthru_info=
0; _temp691->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple1*,
struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp691->toplevel= 1;
_temp691;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp692=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp692->break_lab= e->break_lab; _temp692->continue_lab=
e->continue_lab; _temp692->fallthru_info= e->fallthru_info; _temp692->varmap= e->varmap;
_temp692->toplevel= e->toplevel; _temp692;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp693=(* x).f1; struct
Cyc_List_List* _temp699; _LL695: if(( unsigned int) _temp693 > 1u?(( struct
_enum_struct*) _temp693)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL700: _temp699=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp693)->f1; goto
_LL696;} else{ goto _LL697;} _LL697: goto _LL698; _LL696:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp701=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp701->tag= Cyc_Core_Impossible_tag;
_temp701->f1=({ struct _tagged_string _temp702= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp702.last_plus_one - _temp702.curr,
_temp702.curr);});( struct _xenum_struct*) _temp701;})); _LL698: goto _LL694;
_LL694:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key, struct Cyc_Absyn_Exp*
data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab= 0; ans->continue_lab= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
fallthru_vars= 0; for( 0; fallthru_binders != 0; fallthru_binders=
fallthru_binders->tl){ fallthru_vars=({ struct Cyc_List_List* _temp703=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp703->hd=( void*)((
struct Cyc_Absyn_Vardecl*) fallthru_binders->hd)->name; _temp703->tl=
fallthru_vars; _temp703;});} fallthru_vars=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( fallthru_vars);{ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp704=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp704->v=( void*)
break_l; _temp704;}); ans->fallthru_info=({ struct Cyc_Core_Opt* _temp705=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp705->v=(
void*)({ struct _tuple8* _temp706=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp706->f1= fallthru_l; _temp706->f2= fallthru_vars; _temp706->f3=
next_case_env->varmap; _temp706;}); _temp705;}); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp707=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp707->v=( void*) break_l; _temp707;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp708=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp708->v=( void*)({
struct _tuple8* _temp709=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp709->f1= next_l; _temp709->f2= 0; _temp709->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp709;}); _temp708;}); return ans;} static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp710=( char*)"expecting a literal format string";
struct _tagged_string _temp711; _temp711.curr= _temp710; _temp711.base= _temp710;
_temp711.last_plus_one= _temp710 + 34; _temp711;}));}{ struct _tagged_string
fmt_str;{ void* _temp712=( void*)(( struct Cyc_Absyn_Exp*) args->hd)->r; void*
_temp718; struct _tagged_string _temp720; int _temp722; _LL714: if((( struct
_enum_struct*) _temp712)->tag == Cyc_Absyn_Const_e_tag){ _LL719: _temp718=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp712)->f1; if(( unsigned int) _temp718 > 1u?((
struct _enum_struct*) _temp718)->tag == Cyc_Absyn_String_c_tag: 0){ _LL723:
_temp722=( int)(( struct Cyc_Absyn_String_c_struct*) _temp718)->f1; goto _LL721;
_LL721: _temp720=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp718)->f2; goto _LL715;} else{ goto _LL716;}} else{ goto _LL716;} _LL716:
goto _LL717; _LL715: fmt_str= _temp720; goto _LL713; _LL717: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp724=( char*)"expecting a literal format string"; struct _tagged_string
_temp725; _temp725.curr= _temp724; _temp725.base= _temp724; _temp725.last_plus_one=
_temp724 + 34; _temp725;})); _LL713:;}{ int len=( int)({ struct _tagged_string
_temp726= fmt_str;( unsigned int)( _temp726.last_plus_one - _temp726.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(( int)({ struct _tagged_string _temp727=
fmt_str; char* _temp729= _temp727.curr + i; if( _temp729 < _temp727.base? 1:
_temp729 >= _temp727.last_plus_one){ _throw( Null_Exception);}* _temp729;}) ==(
int)'%'){ i ++; if( i < len?( int)({ struct _tagged_string _temp730= fmt_str;
char* _temp732= _temp730.curr + i; if( _temp732 < _temp730.base? 1: _temp732 >=
_temp730.last_plus_one){ _throw( Null_Exception);}* _temp732;}) ==( int)'s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp733=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp733->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp733->tl= args; _temp733;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args= args->tl; typs= typs->tl;{
struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct
Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){ char c=({ struct
_tagged_string _temp734= fmt_str; char* _temp736= _temp734.curr + i; if(
_temp736 < _temp734.base? 1: _temp736 >= _temp734.last_plus_one){ _throw(
Null_Exception);}* _temp736;}); rev_fmt=({ struct Cyc_List_List* _temp737=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp737->hd=(
void*)(( int) c); _temp737->tl= rev_fmt; _temp737;}); if(( int) c !=( int)'%'){
continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp738=( char*)"bad format string"; struct
_tagged_string _temp739; _temp739.curr= _temp738; _temp739.base= _temp738;
_temp739.last_plus_one= _temp738 + 18; _temp739;}));}{ struct _tuple9 _temp742;
int _temp743; char _temp745; struct Cyc_List_List* _temp747; struct Cyc_List_List*
_temp749; struct Cyc_List_List* _temp751; struct Cyc_List_List* _temp753; struct
_tuple9* _temp740=( struct _tuple9*) x->v; _temp742=* _temp740; _LL754: _temp753=
_temp742.f1; goto _LL752; _LL752: _temp751= _temp742.f2; goto _LL750; _LL750:
_temp749= _temp742.f3; goto _LL748; _LL748: _temp747= _temp742.f4; goto _LL746;
_LL746: _temp745= _temp742.f5; goto _LL744; _LL744: _temp743= _temp742.f6; goto
_LL741; _LL741: i= _temp743 - 1; if(( int) _temp745 !=( int)'s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp755->hd=( void*) _temp753; _temp755->tl=({ struct Cyc_List_List* _temp756=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp756->hd=(
void*) _temp751; _temp756->tl=({ struct Cyc_List_List* _temp757=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp757->hd=( void*) _temp749;
_temp757->tl=({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp758->hd=( void*) _temp747;
_temp758->tl= 0; _temp758;}); _temp757;}); _temp756;}); _temp755;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp759->hd=( void*)(( int) _temp745); _temp759->tl=
rev_fmt; _temp759;});{ struct Cyc_List_List* _temp760= _temp751; struct Cyc_List_List
_temp767; struct Cyc_List_List* _temp768; int _temp770; _LL762: if( _temp760 ==
0){ goto _LL764;} else{ _temp767=* _temp760; _LL771: _temp770=( int) _temp767.hd;
goto _LL769; _LL769: _temp768=( struct Cyc_List_List*) _temp767.tl; if( _temp768
== 0){ goto _LL766;} else{ goto _LL764;}} _LL766: if( _temp770 ==( int)'*'){
goto _LL763;} else{ goto _LL764;} _LL764: goto _LL765; _LL763: { struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp772=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp772->hd=( void*)({
struct _tuple10* _temp773=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp773->f1= temp; _temp773->f2=( void*) typs->hd; _temp773->f3=( struct Cyc_Absyn_Exp*)
args->hd; _temp773;}); _temp772->tl= rev_temps; _temp772;}); rev_result=({
struct Cyc_List_List* _temp774=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp774->hd=( void*) Cyc_Absyn_var_exp( temp, 0);
_temp774->tl= rev_result; _temp774;}); args= args->tl; typs= typs->tl; goto
_LL761;} _LL765: goto _LL761; _LL761:;}{ struct Cyc_List_List* _temp775=
_temp749; struct Cyc_List_List _temp782; struct Cyc_List_List* _temp783; struct
Cyc_List_List _temp785; struct Cyc_List_List* _temp786; int _temp788; int
_temp790; _LL777: if( _temp775 == 0){ goto _LL779;} else{ _temp782=* _temp775;
_LL791: _temp790=( int) _temp782.hd; goto _LL784; _LL784: _temp783=( struct Cyc_List_List*)
_temp782.tl; if( _temp783 == 0){ goto _LL779;} else{ _temp785=* _temp783; _LL789:
_temp788=( int) _temp785.hd; goto _LL787; _LL787: _temp786=( struct Cyc_List_List*)
_temp785.tl; if( _temp786 == 0){ goto _LL781;} else{ goto _LL779;}}} _LL781: if(
_temp790 ==( int)'.'? _temp788 ==( int)'*': 0){ goto _LL778;} else{ goto _LL779;}
_LL779: goto _LL780; _LL778: { struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp792=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp792->hd=( void*)({ struct _tuple10*
_temp793=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp793->f1=
temp; _temp793->f2=( void*) typs->hd; _temp793->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp793;}); _temp792->tl= rev_temps; _temp792;}); rev_result=({ struct Cyc_List_List*
_temp794=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp794->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp794->tl= rev_result;
_temp794;}); args= args->tl; typs= typs->tl; goto _LL776;} _LL780: goto _LL776;
_LL776:;} if(( int) _temp745 !=( int)'%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp795=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp795.last_plus_one
- _temp795.curr, _temp795.curr);}));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp796=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp796->hd=( void*)({ struct _tuple10*
_temp797=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp797->f1=
temp; _temp797->f2=( void*) typs->hd; _temp797->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp797;}); _temp796->tl= rev_temps; _temp796;}); rev_result=({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp798->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp798->tl= rev_result;
_temp798;}); args= args->tl; typs= typs->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp799=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp799->hd=( void*)(( int)'.'); _temp799->tl= rev_fmt; _temp799;}); rev_fmt=({
struct Cyc_List_List* _temp800=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp800->hd=( void*)(( int)'*'); _temp800->tl= rev_fmt;
_temp800;}); rev_fmt=({ struct Cyc_List_List* _temp801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp801->hd=( void*)(( int)'s');
_temp801->tl= rev_fmt; _temp801;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp802=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp802->hd=( void*)({ struct _tuple10*
_temp803=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp803->f1=
temp; _temp803->f2=( void*) typs->hd; _temp803->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp803;}); _temp802->tl= rev_temps; _temp802;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp804=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp804->hd=( void*) size_exp; _temp804->tl= rev_result; _temp804;});
rev_result=({ struct Cyc_List_List* _temp805=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp805->hd=( void*) cstring_exp; _temp805->tl=
rev_result; _temp805;}); args= args->tl; typs= typs->tl;}}}}}}{ struct Cyc_List_List*
es=({ struct Cyc_List_List* _temp806=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp806->hd=( void*) Cyc_Absyn_string_exp( 0, Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp806->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp806;}); if( aopt != 0){ es=({ struct Cyc_List_List* _temp807=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp807->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp807->tl=( struct Cyc_List_List*)
es; _temp807;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p,( struct Cyc_List_List*)
es, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps
!= 0; rev_temps= rev_temps->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*)
rev_temps->hd)).f1,(*(( struct _tuple10*) rev_temps->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple10*) rev_temps->hd)).f3, s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}}
static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp808=( char*)"Missing type in primop "; struct _tagged_string _temp809;
_temp809.curr= _temp808; _temp809.base= _temp808; _temp809.last_plus_one=
_temp808 + 24; _temp809;}));} return Cyc_Toc_typ_to_c(( void*)( e->topt)->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp810=( char*)"Missing type in primop "; struct _tagged_string _temp811;
_temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 24; _temp811;}));} return( void*)( e->topt)->v;} static struct
_tuple5* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple5*
_temp812=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp812->f1=
Cyc_Toc_mt_tq; _temp812->f2= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); _temp812;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple7* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple7* _temp813=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp813[ 0]=({ struct
_tuple7 _temp814; _temp814.f1= 0; _temp814.f2= e; _temp814;}); _temp813;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct _tuple1* x, void*
struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); eo=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic( se):
Cyc_Toc_malloc_ptr( se), 0);}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple(
struct Cyc_Toc_Env* nv, int pointer, struct Cyc_List_List* es){ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, es); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; es=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es); for( 0; es != 0; es= es->tl, -- i){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) es->hd); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v): 0; s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string*
_temp815=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp815[ 0]= xprintf("f%d", i); _temp815;}), 0),( struct Cyc_Absyn_Exp*) es->hd,
0), 0), s, 0);}} return Cyc_Toc_make_struct( x, Cyc_Absyn_strct( n), s, pointer,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_List_List* dles, struct _tuple1*
tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es= es->tl){ struct _tuple7 _temp818;
struct Cyc_Absyn_Exp* _temp819; struct Cyc_List_List* _temp821; struct _tuple7*
_temp816=( struct _tuple7*) es->hd; _temp818=* _temp816; _LL822: _temp821=
_temp818.f1; goto _LL820; _LL820: _temp819= _temp818.f2; goto _LL817; _LL817:
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)( _temp819->topt)->v): 0; if(
_temp821 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp823=( char*)"empty designator list"; struct
_tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 22; _temp824;}));} if( _temp821->tl != 0){
struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp(
y, 0); for( 0; _temp821 != 0; _temp821= _temp821->tl){ void* _temp825=( void*)
_temp821->hd; struct _tagged_string* _temp831; _LL827: if((( struct _enum_struct*)
_temp825)->tag == Cyc_Absyn_FieldName_tag){ _LL832: _temp831=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp825)->f1; goto
_LL828;} else{ goto _LL829;} _LL829: goto _LL830; _LL828: if( Cyc_Toc_is_poly_field(
struct_type, _temp831)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp831, 0), yexp, 0), 0), s, 0); goto _LL826; _LL830:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp833=( char*)"array designator in struct";
struct _tagged_string _temp834; _temp834.curr= _temp833; _temp834.base= _temp833;
_temp834.last_plus_one= _temp833 + 27; _temp834;}))); _LL826:;} Cyc_Toc_exp_to_c(
nv, _temp819); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp819, 0), 0), s, 0);} else{ void* _temp835=( void*) _temp821->hd;
struct _tagged_string* _temp841; _LL837: if((( struct _enum_struct*) _temp835)->tag
== Cyc_Absyn_FieldName_tag){ _LL842: _temp841=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp835)->f1; goto _LL838;} else{ goto _LL839;}
_LL839: goto _LL840; _LL838: Cyc_Toc_exp_to_c( nv, _temp819); if( Cyc_Toc_is_poly_field(
struct_type, _temp841)){ _temp819= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp819, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp841, 0), _temp819, 0), 0), s, 0); goto _LL836; _LL840:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp843=( char*)"array designator in struct";
struct _tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 27; _temp844;}))); _LL836:;}}} return Cyc_Toc_make_struct(
x, Cyc_Absyn_strctq( tdn), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp845=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp845->tag= Cyc_Absyn_Increment_e_tag; _temp845->f1= e1; _temp845->f2=( void*)
incr;( void*) _temp845;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp846=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp856; struct Cyc_Absyn_Exp* _temp858; void* _temp860; struct _tagged_string*
_temp862; struct Cyc_Absyn_Exp* _temp864; _LL848: if((( struct _enum_struct*)
_temp846)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL857: _temp856=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp846)->f1; goto _LL849;} else{ goto
_LL850;} _LL850: if((( struct _enum_struct*) _temp846)->tag == Cyc_Absyn_Cast_e_tag){
_LL861: _temp860=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp846)->f1; goto
_LL859; _LL859: _temp858=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp846)->f2; goto _LL851;} else{ goto _LL852;} _LL852: if((( struct
_enum_struct*) _temp846)->tag == Cyc_Absyn_StructMember_e_tag){ _LL865: _temp864=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp846)->f1;
goto _LL863; _LL863: _temp862=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp846)->f2; goto _LL853;} else{ goto _LL854;} _LL854: goto _LL855; _LL849:((
void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp856, fs, f, f_env); goto _LL847; _LL851:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp858, fs, f, f_env); goto
_LL847; _LL853:( void*)( e1->r=( void*)(( void*) _temp864->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp866=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp866->hd=( void*) _temp862; _temp866->tl= fs;
_temp866;}), f, f_env); goto _LL847; _LL855: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp867= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp867.last_plus_one - _temp867.curr, _temp867.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_string*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)((
void*)( f( e1_copy, f_env))->r)); goto _LL847;} _LL847:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp868=( void*) s->r;
struct Cyc_Absyn_Exp* _temp878; struct Cyc_Absyn_Stmt* _temp880; struct Cyc_Absyn_Decl*
_temp882; struct Cyc_Absyn_Stmt* _temp884; struct Cyc_Absyn_Stmt* _temp886;
_LL870: if(( unsigned int) _temp868 > 1u?(( struct _enum_struct*) _temp868)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL879: _temp878=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp868)->f1; goto _LL871;} else{ goto _LL872;} _LL872:
if(( unsigned int) _temp868 > 1u?(( struct _enum_struct*) _temp868)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL883: _temp882=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp868)->f1; goto _LL881; _LL881: _temp880=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp868)->f2; goto _LL873;} else{ goto _LL874;}
_LL874: if(( unsigned int) _temp868 > 1u?(( struct _enum_struct*) _temp868)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL887: _temp886=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp868)->f1; goto _LL885; _LL885: _temp884=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp868)->f2; goto _LL875;}
else{ goto _LL876;} _LL876: goto _LL877; _LL871:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp878, fs, f, f_env); goto
_LL869; _LL873:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp880, fs, f, f_env); goto _LL869; _LL875:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp884, fs, f, f_env); goto
_LL869; _LL877:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp888= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp888.last_plus_one - _temp888.curr, _temp888.curr);})); goto _LL869; _LL869:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript( struct Cyc_Absyn_Exp*
arr, struct Cyc_Absyn_Exp* ind, struct Cyc_Absyn_Exp* esz, void* elt_typ, struct
Cyc_Absyn_Tqual* tq, int take_address){ unsigned int sz= Cyc_Evexp_eval_const_uint_exp(
esz); struct _tuple1* a= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
void* ta= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( elt_typ), tq); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_prim2_exp( Cyc_Absyn_Gte, Cyc_Absyn_var_exp( i, 0), Cyc_Absyn_uint_exp(
sz, 0), 0); struct Cyc_Absyn_Exp* e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt*
s4= Cyc_Absyn_ifthenelse_stmt( e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp*
e5= Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0),
0); if( take_address){ e5= Cyc_Absyn_address_exp( e5, 0);}{ struct Cyc_Absyn_Stmt*
s6= Cyc_Absyn_exp_stmt( e5, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
s4, s6, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt( i, Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) ind, s3, 0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
a, ta,( struct Cyc_Absyn_Exp*) arr, s1, 0), 0);}} static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp889= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp889.last_plus_one - _temp889.curr, _temp889.curr);})); return;}{ void*
old_typ=( void*)( e->topt)->v; void* _temp890= r; void* _temp966; void* _temp969;
struct _tagged_string _temp971; int _temp973; void* _temp975; void* _temp977;
struct _tuple1* _temp979; struct _tuple1* _temp981; struct Cyc_List_List*
_temp983; void* _temp985; void* _temp987; struct Cyc_Absyn_Exp* _temp989; struct
Cyc_Absyn_Exp* _temp991; struct Cyc_Core_Opt* _temp993; struct Cyc_Absyn_Exp*
_temp995; struct Cyc_Absyn_Exp* _temp997; struct Cyc_Absyn_Exp* _temp999; struct
Cyc_Absyn_Exp* _temp1001; struct Cyc_Absyn_Exp* _temp1003; struct Cyc_Absyn_Exp*
_temp1005; struct Cyc_List_List* _temp1007; struct Cyc_Absyn_Exp* _temp1009;
struct Cyc_List_List* _temp1011; struct Cyc_Absyn_Exp* _temp1013; struct Cyc_Absyn_Exp*
_temp1015; struct Cyc_Absyn_Exp* _temp1017; struct Cyc_List_List* _temp1019;
struct Cyc_Absyn_Exp* _temp1021; struct Cyc_Absyn_Exp* _temp1023; struct Cyc_Absyn_Exp**
_temp1025; void* _temp1026; void** _temp1028; struct Cyc_Absyn_Exp* _temp1029;
void* _temp1031; struct Cyc_Absyn_Exp* _temp1033; struct Cyc_Absyn_Exp*
_temp1035; struct _tagged_string* _temp1037; struct Cyc_Absyn_Exp* _temp1039;
struct _tagged_string* _temp1041; struct Cyc_Absyn_Exp* _temp1043; struct Cyc_Absyn_Exp*
_temp1045; struct Cyc_Absyn_Exp* _temp1047; struct Cyc_List_List* _temp1049;
struct Cyc_List_List* _temp1051; struct _tuple2* _temp1053; struct Cyc_List_List*
_temp1055; int _temp1057; struct Cyc_Absyn_Exp* _temp1059; struct Cyc_Absyn_Exp*
_temp1061; struct Cyc_Absyn_Vardecl* _temp1063; struct Cyc_Absyn_Structdecl*
_temp1065; struct Cyc_List_List* _temp1067; struct Cyc_Core_Opt* _temp1069;
struct _tuple1* _temp1071; struct Cyc_Absyn_Enumfield* _temp1073; struct Cyc_Absyn_Enumdecl*
_temp1075; struct Cyc_List_List* _temp1077; struct Cyc_Core_Opt* _temp1079;
struct Cyc_Core_Opt* _temp1081; struct Cyc_Absyn_Enumfield* _temp1083; struct
Cyc_Absyn_Enumdecl* _temp1085; struct Cyc_List_List* _temp1087; struct Cyc_Core_Opt*
_temp1089; struct Cyc_Core_Opt* _temp1091; struct Cyc_Absyn_Enumfield* _temp1093;
struct Cyc_Absyn_Xenumdecl* _temp1095; struct Cyc_List_List* _temp1097; struct
Cyc_Core_Opt* _temp1099; void* _temp1101; void* _temp1103; void* _temp1105;
struct Cyc_Absyn_Stmt* _temp1107; struct Cyc_List_List* _temp1109; struct Cyc_Core_Opt*
_temp1111; struct Cyc_Absyn_Fndecl* _temp1113; struct Cyc_Absyn_Exp* _temp1115;
_LL892: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Const_e_tag){
_LL967: _temp966=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp890)->f1; if(
_temp966 == Cyc_Absyn_Null_c){ goto _LL893;} else{ goto _LL894;}} else{ goto
_LL894;} _LL894: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Const_e_tag){
_LL970: _temp969=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp890)->f1; if((
unsigned int) _temp969 > 1u?(( struct _enum_struct*) _temp969)->tag == Cyc_Absyn_String_c_tag:
0){ _LL974: _temp973=( int)(( struct Cyc_Absyn_String_c_struct*) _temp969)->f1;
goto _LL972; _LL972: _temp971=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp969)->f2; goto _LL968;} else{ goto _LL896;}} else{ goto _LL896;} _LL968:
if( _temp973){ goto _LL895;} else{ goto _LL896;} _LL896: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_Const_e_tag){ _LL976: _temp975=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp890)->f1; goto _LL897;} else{ goto _LL898;}
_LL898: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Var_e_tag){
_LL980: _temp979=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp890)->f1;
goto _LL978; _LL978: _temp977=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp890)->f2; goto _LL899;} else{ goto _LL900;} _LL900: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL982: _temp981=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp890)->f1; goto
_LL901;} else{ goto _LL902;} _LL902: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_Primop_e_tag){ _LL986: _temp985=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp890)->f1; goto _LL984; _LL984: _temp983=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp890)->f2; goto _LL903;} else{ goto _LL904;}
_LL904: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Increment_e_tag){
_LL990: _temp989=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp890)->f1; goto _LL988; _LL988: _temp987=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp890)->f2; goto _LL905;} else{ goto _LL906;} _LL906: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL996: _temp995=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp890)->f1;
goto _LL994; _LL994: _temp993=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp890)->f2; goto _LL992; _LL992: _temp991=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp890)->f3; goto _LL907;} else{ goto _LL908;}
_LL908: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1002: _temp1001=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp890)->f1; goto _LL1000; _LL1000: _temp999=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp890)->f2; goto _LL998; _LL998: _temp997=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp890)->f3;
goto _LL909;} else{ goto _LL910;} _LL910: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL1006: _temp1005=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp890)->f1; goto _LL1004; _LL1004: _temp1003=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp890)->f2; goto
_LL911;} else{ goto _LL912;} _LL912: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL1010: _temp1009=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp890)->f1; goto _LL1008; _LL1008:
_temp1007=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp890)->f2; goto _LL913;} else{ goto _LL914;} _LL914: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1014: _temp1013=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp890)->f1; goto
_LL1012; _LL1012: _temp1011=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp890)->f2; goto _LL915;} else{ goto _LL916;} _LL916: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_Throw_e_tag){ _LL1016: _temp1015=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp890)->f1; goto
_LL917;} else{ goto _LL918;} _LL918: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL1018: _temp1017=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp890)->f1; goto _LL919;} else{
goto _LL920;} _LL920: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL1022: _temp1021=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp890)->f1; goto _LL1020; _LL1020: _temp1019=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp890)->f2; goto _LL921;} else{ goto
_LL922;} _LL922: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Cast_e_tag){
_LL1027: _temp1026=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp890)->f1;
_temp1028=&(( struct Cyc_Absyn_Cast_e_struct*) _temp890)->f1; goto _LL1024;
_LL1024: _temp1023=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp890)->f2; _temp1025=&(( struct Cyc_Absyn_Cast_e_struct*) _temp890)->f2;
goto _LL923;} else{ goto _LL924;} _LL924: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_Address_e_tag){ _LL1030: _temp1029=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp890)->f1; goto _LL925;} else{ goto
_LL926;} _LL926: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1032: _temp1031=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp890)->f1;
goto _LL927;} else{ goto _LL928;} _LL928: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL1034: _temp1033=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp890)->f1; goto _LL929;} else{ goto
_LL930;} _LL930: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Deref_e_tag){
_LL1036: _temp1035=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp890)->f1; goto _LL931;} else{ goto _LL932;} _LL932: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1040:
_temp1039=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp890)->f1; goto _LL1038; _LL1038: _temp1037=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp890)->f2; goto _LL933;} else{ goto
_LL934;} _LL934: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1044: _temp1043=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp890)->f1; goto _LL1042; _LL1042: _temp1041=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp890)->f2; goto _LL935;} else{ goto
_LL936;} _LL936: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1048: _temp1047=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp890)->f1; goto _LL1046; _LL1046: _temp1045=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp890)->f2; goto _LL937;} else{ goto
_LL938;} _LL938: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1050: _temp1049=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp890)->f1; goto _LL939;} else{ goto _LL940;} _LL940: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1054:
_temp1053=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp890)->f1;
goto _LL1052; _LL1052: _temp1051=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp890)->f2; goto _LL941;} else{ goto _LL942;} _LL942: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_Array_e_tag){ _LL1058: _temp1057=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp890)->f1; goto _LL1056; _LL1056:
_temp1055=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp890)->f2;
goto _LL943;} else{ goto _LL944;} _LL944: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1064: _temp1063=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp890)->f1; goto _LL1062; _LL1062:
_temp1061=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp890)->f2; goto _LL1060; _LL1060: _temp1059=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp890)->f3; goto _LL945;} else{
goto _LL946;} _LL946: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Struct_e_tag){
_LL1072: _temp1071=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp890)->f1; goto _LL1070; _LL1070: _temp1069=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp890)->f2; goto _LL1068; _LL1068: _temp1067=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp890)->f3; goto
_LL1066; _LL1066: _temp1065=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp890)->f4; goto _LL947;} else{ goto _LL948;} _LL948: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_Enum_e_tag){ _LL1082: _temp1081=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp890)->f1; goto
_LL1080; _LL1080: _temp1079=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp890)->f2; goto _LL1078; _LL1078: _temp1077=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp890)->f3; if( _temp1077 == 0){ goto
_LL1076;} else{ goto _LL950;} _LL1076: _temp1075=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp890)->f4; goto _LL1074; _LL1074: _temp1073=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp890)->f5;
goto _LL949;} else{ goto _LL950;} _LL950: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_Enum_e_tag){ _LL1092: _temp1091=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp890)->f1; goto _LL1090; _LL1090: _temp1089=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp890)->f2; goto
_LL1088; _LL1088: _temp1087=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp890)->f3; goto _LL1086; _LL1086: _temp1085=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp890)->f4; goto _LL1084; _LL1084: _temp1083=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp890)->f5;
goto _LL951;} else{ goto _LL952;} _LL952: if((( struct _enum_struct*) _temp890)->tag
== Cyc_Absyn_Xenum_e_tag){ _LL1100: _temp1099=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp890)->f1; goto _LL1098; _LL1098: _temp1097=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp890)->f2; goto
_LL1096; _LL1096: _temp1095=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp890)->f3; goto _LL1094; _LL1094: _temp1093=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp890)->f4; goto _LL953;} else{ goto _LL954;}
_LL954: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1102: _temp1101=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp890)->f1;
if((( struct _enum_struct*) _temp1101)->tag == Cyc_Absyn_Typ_m_tag){ _LL1104:
_temp1103=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp1101)->f1; goto _LL955;}
else{ goto _LL956;}} else{ goto _LL956;} _LL956: if((( struct _enum_struct*)
_temp890)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1106: _temp1105=( void*)(( struct
Cyc_Absyn_Malloc_e_struct*) _temp890)->f1; goto _LL957;} else{ goto _LL958;}
_LL958: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1108: _temp1107=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp890)->f1; goto _LL959;} else{ goto _LL960;} _LL960: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1112:
_temp1111=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp890)->f1; goto _LL1110; _LL1110: _temp1109=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp890)->f2; goto _LL961;} else{
goto _LL962;} _LL962: if((( struct _enum_struct*) _temp890)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1114: _temp1113=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp890)->f1; goto _LL963;} else{ goto _LL964;} _LL964: if((( struct
_enum_struct*) _temp890)->tag == Cyc_Absyn_Fill_e_tag){ _LL1116: _temp1115=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp890)->f1; goto
_LL965;} else{ goto _LL891;} _LL893:{ void* _temp1117= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1123; struct Cyc_Absyn_Conref* _temp1125;
struct Cyc_Absyn_Tqual* _temp1127; struct Cyc_Absyn_Conref* _temp1129; void*
_temp1131; void* _temp1133; _LL1119: if(( unsigned int) _temp1117 > 4u?(( struct
_enum_struct*) _temp1117)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1124:
_temp1123=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1117)->f1; _LL1134: _temp1133=( void*) _temp1123.elt_typ; goto _LL1132;
_LL1132: _temp1131=( void*) _temp1123.rgn_typ; goto _LL1130; _LL1130: _temp1129=(
struct Cyc_Absyn_Conref*) _temp1123.nullable; goto _LL1128; _LL1128: _temp1127=(
struct Cyc_Absyn_Tqual*) _temp1123.tq; goto _LL1126; _LL1126: _temp1125=( struct
Cyc_Absyn_Conref*) _temp1123.bounds; goto _LL1120;} else{ goto _LL1121;} _LL1121:
goto _LL1122; _LL1120:{ void* _temp1135=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1125); struct Cyc_Absyn_Exp* _temp1141; _LL1137: if(
_temp1135 == Cyc_Absyn_Unknown_b){ goto _LL1138;} else{ goto _LL1139;} _LL1139:
if(( unsigned int) _temp1135 > 1u?(( struct _enum_struct*) _temp1135)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1142: _temp1141=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1135)->f1; goto _LL1140;} else{ goto _LL1136;}
_LL1138: { struct _tuple7* zero=({ struct _tuple7* _temp1143=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7)); _temp1143->f1= 0; _temp1143->f2= Cyc_Absyn_signed_int_exp(
0, 0); _temp1143;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List* _temp1144=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1144->hd=( void*) zero; _temp1144->tl=({ struct Cyc_List_List*
_temp1145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1145->hd=( void*) zero; _temp1145->tl=({ struct Cyc_List_List* _temp1146=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1146->hd=(
void*) zero; _temp1146->tl= 0; _temp1146;}); _temp1145;}); _temp1144;}), 0); if(
nv->toplevel){( void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct(
Cyc_Toc_add_tagged_ptr_type( Cyc_Toc_typ_to_c( _temp1133), Cyc_Toc_mt_tq));
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*)
um, Cyc_Absyn_exp_stmt( xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));} goto _LL1136;} _LL1140:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1136; _LL1136:;} goto _LL1118; _LL1122:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1147= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1147.last_plus_one -
_temp1147.curr, _temp1147.curr);})); goto _LL1118; _LL1118:;} goto _LL891;
_LL895:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1148=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1148->f1= Cyc_Toc_abs_ns; _temp1148->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1148;}), 0),({ struct Cyc_List_List* _temp1149=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1149->hd=( void*) Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp1150=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1150->tag= Cyc_Absyn_Const_e_tag;
_temp1150->f1=( void*)({ struct Cyc_Absyn_String_c_struct* _temp1151=( struct
Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1151->tag= Cyc_Absyn_String_c_tag; _temp1151->f1= 0; _temp1151->f2=
_temp971;( void*) _temp1151;});( void*) _temp1150;}), 0); _temp1149->tl= 0;
_temp1149;}), 0))->r)); goto _LL891; _LL897: goto _LL891; _LL899:{ struct
_handler_cons _temp1152; _push_handler(& _temp1152);{ struct _xenum_struct*
_temp1153=( struct _xenum_struct*) setjmp( _temp1152.handler); if( ! _temp1153){(
void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict*
d, struct _tuple1* key)) Cyc_Dict_lookup)( nv->varmap, _temp979))->r));;
_pop_handler();} else{ struct _xenum_struct* _temp1155= _temp1153; _LL1157: if(
_temp1155->tag == Cyc_Dict_Absent_tag){ goto _LL1158;} else{ goto _LL1159;}
_LL1159: goto _LL1160; _LL1158:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1161= Cyc_Absynpp_qvar2string( _temp979); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1161.last_plus_one - _temp1161.curr, _temp1161.curr);})); return; _LL1160:(
void) _throw( _temp1155); _LL1156:;}}} goto _LL891; _LL901:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1162=(
char*)"unknownid"; struct _tagged_string _temp1163; _temp1163.curr= _temp1162;
_temp1163.base= _temp1162; _temp1163.last_plus_one= _temp1162 + 10; _temp1163;}));
goto _LL891; _LL903: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp983); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp983);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp983);{ void* _temp1164= _temp985; _LL1166: if( _temp1164 == Cyc_Absyn_Size){
goto _LL1167;} else{ goto _LL1168;} _LL1168: if( _temp1164 == Cyc_Absyn_Printf){
goto _LL1169;} else{ goto _LL1170;} _LL1170: if( _temp1164 == Cyc_Absyn_Xprintf){
goto _LL1171;} else{ goto _LL1172;} _LL1172: if( _temp1164 == Cyc_Absyn_Fprintf){
goto _LL1173;} else{ goto _LL1174;} _LL1174: if( _temp1164 == Cyc_Absyn_Sscanf){
goto _LL1175;} else{ goto _LL1176;} _LL1176: if( _temp1164 == Cyc_Absyn_Plus){
goto _LL1177;} else{ goto _LL1178;} _LL1178: if( _temp1164 == Cyc_Absyn_Minus){
goto _LL1179;} else{ goto _LL1180;} _LL1180: if( _temp1164 == Cyc_Absyn_Eq){
goto _LL1181;} else{ goto _LL1182;} _LL1182: if( _temp1164 == Cyc_Absyn_Neq){
goto _LL1183;} else{ goto _LL1184;} _LL1184: if( _temp1164 == Cyc_Absyn_Gt){
goto _LL1185;} else{ goto _LL1186;} _LL1186: if( _temp1164 == Cyc_Absyn_Gte){
goto _LL1187;} else{ goto _LL1188;} _LL1188: if( _temp1164 == Cyc_Absyn_Lt){
goto _LL1189;} else{ goto _LL1190;} _LL1190: if( _temp1164 == Cyc_Absyn_Lte){
goto _LL1191;} else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1167: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*) _temp983->hd;{ void* _temp1194=( void*)( arg->topt)->v;
struct Cyc_Absyn_Exp* _temp1202; struct Cyc_Absyn_Tqual* _temp1204; void*
_temp1206; struct Cyc_Absyn_PtrInfo _temp1208; struct Cyc_Absyn_Conref*
_temp1210; struct Cyc_Absyn_Tqual* _temp1212; struct Cyc_Absyn_Conref* _temp1214;
void* _temp1216; void* _temp1218; _LL1196: if(( unsigned int) _temp1194 > 4u?((
struct _enum_struct*) _temp1194)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1207:
_temp1206=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1194)->f1; goto
_LL1205; _LL1205: _temp1204=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1194)->f2; goto _LL1203; _LL1203: _temp1202=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1194)->f3; goto _LL1197;} else{ goto
_LL1198;} _LL1198: if(( unsigned int) _temp1194 > 4u?(( struct _enum_struct*)
_temp1194)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1209: _temp1208=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1194)->f1;
_LL1219: _temp1218=( void*) _temp1208.elt_typ; goto _LL1217; _LL1217: _temp1216=(
void*) _temp1208.rgn_typ; goto _LL1215; _LL1215: _temp1214=( struct Cyc_Absyn_Conref*)
_temp1208.nullable; goto _LL1213; _LL1213: _temp1212=( struct Cyc_Absyn_Tqual*)
_temp1208.tq; goto _LL1211; _LL1211: _temp1210=( struct Cyc_Absyn_Conref*)
_temp1208.bounds; goto _LL1199;} else{ goto _LL1200;} _LL1200: goto _LL1201;
_LL1197:( void*)( e->r=( void*)(( void*) _temp1202->r)); goto _LL1195; _LL1199: {
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)( arg->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1195;}
_LL1201:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1220= Cyc_Absynpp_typ2string(( void*) typs->hd); struct
_tagged_string _temp1221= Cyc_Absynpp_typ2string(( void*)( arg->topt)->v);
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1220.last_plus_one
- _temp1220.curr, _temp1220.curr, _temp1221.last_plus_one - _temp1221.curr,
_temp1221.curr);})); return; _LL1195:;} goto _LL1165;} _LL1169:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp985, 0, _temp983, typs))->r)); goto
_LL1165; _LL1171:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp985,
0, _temp983, typs))->r)); goto _LL1165; _LL1173: if( _temp983 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1222=( char*)"fprintf without arguments"; struct _tagged_string _temp1223;
_temp1223.curr= _temp1222; _temp1223.base= _temp1222; _temp1223.last_plus_one=
_temp1222 + 26; _temp1223;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp985,({ struct Cyc_Core_Opt* _temp1224=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1224->v=( void*)(( struct Cyc_Absyn_Exp*)
_temp983->hd); _temp1224;}), _temp983->tl, typs->tl))->r)); goto _LL1165;
_LL1175: if( _temp983 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1225=( char*)"sscanf without arguments";
struct _tagged_string _temp1226; _temp1226.curr= _temp1225; _temp1226.base=
_temp1225; _temp1226.last_plus_one= _temp1225 + 25; _temp1226;}));}( void*)(((
struct Cyc_Absyn_Exp*) _temp983->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*) _temp983->hd)->r,(( struct
Cyc_Absyn_Exp*) _temp983->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1165;
_LL1177: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp983->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp983->tl)->hd; struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp,
0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1227=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1227->v=( void*) Cyc_Absyn_Plus; _temp1227;}), e2, 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1165; _LL1179: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp983->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp983->tl)->hd; void*
old_typ_e2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1228=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1228->v=( void*) Cyc_Absyn_Minus; _temp1228;}), e2,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1165; _LL1181: goto _LL1183; _LL1183:
goto _LL1185; _LL1185: goto _LL1187; _LL1187: goto _LL1189; _LL1189: goto
_LL1191; _LL1191: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp983->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp983->tl)->hd; void* t1=(
void*) old_typs->hd; void* t2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1165;} _LL1193: goto
_LL1165; _LL1165:;} goto _LL891;} _LL905: { void* e2_cyc_typ=( void*)( _temp989->topt)->v;
Cyc_Toc_exp_to_c( nv, _temp989); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp987 == Cyc_Absyn_PreInc? 1: _temp987 == Cyc_Absyn_PreDec){
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ), Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp987, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp989, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple1* temp=
Cyc_Toc_temp_var(); struct _tuple1* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ), Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp987, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp989, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp989)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp989, 0, Cyc_Toc_incr_lvalue,
_temp987);( void*)( e->r=( void*)(( void*) _temp989->r));}} goto _LL891;} _LL907: {
int e1_poly= Cyc_Toc_is_poly_project( _temp995); void* e1_old_typ=( void*)(
_temp995->topt)->v; void* e2_old_typ=( void*)( _temp991->topt)->v; Cyc_Toc_exp_to_c(
nv, _temp995); Cyc_Toc_exp_to_c( nv, _temp991); if( _temp993 != 0?( void*)
_temp993->v == Cyc_Absyn_Plus? 1:( void*) _temp993->v == Cyc_Absyn_Minus: 0){
if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp995->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp995->r, 0),
Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ( e2_old_typ)){(
void*)( _temp991->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp991->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if( e1_poly){( void*)(
_temp991->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp991->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp995)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp995, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1229=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1229[ 0]=({ struct _tuple11 _temp1230; _temp1230.f1=
_temp993; _temp1230.f2= _temp991; _temp1230;}); _temp1229;}));( void*)( e->r=(
void*)(( void*) _temp995->r));} goto _LL891;} _LL909: Cyc_Toc_exp_to_c( nv,
_temp1001); Cyc_Toc_exp_to_c( nv, _temp999); Cyc_Toc_exp_to_c( nv, _temp997);
goto _LL891; _LL911: Cyc_Toc_exp_to_c( nv, _temp1005); Cyc_Toc_exp_to_c( nv,
_temp1003); goto _LL891; _LL913: _temp1013= _temp1009; _temp1011= _temp1007;
goto _LL915; _LL915: Cyc_Toc_exp_to_c( nv, _temp1013);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1011); goto _LL891; _LL917: Cyc_Toc_exp_to_c(
nv, _temp1015);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1015), 0))->r)); goto _LL891; _LL919: Cyc_Toc_exp_to_c(
nv, _temp1017); goto _LL891; _LL921: Cyc_Toc_exp_to_c( nv, _temp1021);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); for( 0; _temp1019 != 0;
_temp1019= _temp1019->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp1019->hd);
if( k != Cyc_Absyn_EffKind? k != Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)((
void*)( Cyc_Toc_array_to_ptr_cast( t_inst, _temp1021, 0))->r)); break;}} goto
_LL891;} _LL923: { void* old_t2=( void*)((* _temp1025)->topt)->v; void* new_typ=*
_temp1028;* _temp1028= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,*
_temp1025);* _temp1025= Cyc_Toc_convert_exp( nv->toplevel, old_t2, new_typ,*
_temp1025); goto _LL891;} _LL925:{ void* _temp1231=( void*) _temp1029->r; struct
Cyc_Absyn_Structdecl* _temp1239; struct Cyc_List_List* _temp1241; struct Cyc_Core_Opt*
_temp1243; struct _tuple1* _temp1245; struct Cyc_List_List* _temp1247; _LL1233:
if((( struct _enum_struct*) _temp1231)->tag == Cyc_Absyn_Struct_e_tag){ _LL1246:
_temp1245=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1231)->f1;
goto _LL1244; _LL1244: _temp1243=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1231)->f2; goto _LL1242; _LL1242: _temp1241=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1231)->f3; goto _LL1240; _LL1240:
_temp1239=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1231)->f4; goto _LL1234;} else{ goto _LL1235;} _LL1235: if((( struct
_enum_struct*) _temp1231)->tag == Cyc_Absyn_Tuple_e_tag){ _LL1248: _temp1247=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1231)->f1; goto
_LL1236;} else{ goto _LL1237;} _LL1237: goto _LL1238; _LL1234: if( nv->toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string
_temp1249= Cyc_Position_string_of_segment( _temp1029->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1249.last_plus_one - _temp1249.curr, _temp1249.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)( _temp1029->topt)->v, 1,
_temp1241, _temp1245))->r)); goto _LL1232; _LL1236: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1250= Cyc_Position_string_of_segment(
_temp1029->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1250.last_plus_one - _temp1250.curr, _temp1250.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1247))->r)); goto _LL1232;
_LL1238: Cyc_Toc_exp_to_c( nv, _temp1029); if( ! Cyc_Absyn_is_lvalue( _temp1029)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1029,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*) _temp1029->r));}
goto _LL1232; _LL1232:;} goto _LL891; _LL927:( void*)( e->r=( void*)({ struct
Cyc_Absyn_Sizeoftyp_e_struct* _temp1251=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp1251->tag= Cyc_Absyn_Sizeoftyp_e_tag;
_temp1251->f1=( void*) Cyc_Toc_typ_to_c_array( _temp1031);( void*) _temp1251;}));
goto _LL891; _LL929: Cyc_Toc_exp_to_c( nv, _temp1033); goto _LL891; _LL931: Cyc_Toc_exp_to_c(
nv, _temp1035);{ void* _temp1252= Cyc_Tcutil_compress(( void*)( _temp1035->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1258; struct Cyc_Absyn_Conref* _temp1260; struct
Cyc_Absyn_Tqual* _temp1262; struct Cyc_Absyn_Conref* _temp1264; void* _temp1266;
void* _temp1268; _LL1254: if(( unsigned int) _temp1252 > 4u?(( struct
_enum_struct*) _temp1252)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1259:
_temp1258=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1252)->f1; _LL1269: _temp1268=( void*) _temp1258.elt_typ; goto _LL1267;
_LL1267: _temp1266=( void*) _temp1258.rgn_typ; goto _LL1265; _LL1265: _temp1264=(
struct Cyc_Absyn_Conref*) _temp1258.nullable; goto _LL1263; _LL1263: _temp1262=(
struct Cyc_Absyn_Tqual*) _temp1258.tq; goto _LL1261; _LL1261: _temp1260=( struct
Cyc_Absyn_Conref*) _temp1258.bounds; goto _LL1255;} else{ goto _LL1256;} _LL1256:
goto _LL1257; _LL1255:{ void* _temp1270=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1260); struct Cyc_Absyn_Exp* _temp1276; _LL1272: if(
_temp1270 == Cyc_Absyn_Unknown_b){ goto _LL1273;} else{ goto _LL1274;} _LL1274:
if(( unsigned int) _temp1270 > 1u?(( struct _enum_struct*) _temp1270)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1277: _temp1276=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1270)->f1; goto _LL1275;} else{ goto _LL1271;}
_LL1273: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
void* ta1= Cyc_Toc_typ_to_c_array( _temp1268); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1262); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1262); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt(
test_exp, s4, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s3, s5, 0); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_declare_stmt( a, ta2,( struct
Cyc_Absyn_Exp*) a_init, s2, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1035, s1, 0), 0))->r));
goto _LL1271;} _LL1275: { int nullable= Cyc_Toc_is_nullable(( void*)( _temp1035->topt)->v);
if( nullable){ struct _tuple1* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)( _temp1035->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( _temp1276->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1035, s3, 0), 0))->r));} goto _LL1271;} _LL1271:;} goto _LL1253; _LL1257:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1278=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1279; _temp1279.curr= _temp1278; _temp1279.base= _temp1278; _temp1279.last_plus_one=
_temp1278 + 29; _temp1279;})); goto _LL1253; _LL1253:;} goto _LL891; _LL933: Cyc_Toc_exp_to_c(
nv, _temp1039); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL891; _LL935: Cyc_Toc_exp_to_c( nv, _temp1043);
if( Cyc_Toc_is_nullable(( void*)( _temp1043->topt)->v)){ struct _tuple1* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)( _temp1043->topt)->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1041, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1043, s3, 0), 0))->r));} else{ if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}} goto _LL891; _LL937: { void*
e1_typ= Cyc_Tcutil_compress(( void*)( _temp1047->topt)->v); Cyc_Toc_exp_to_c( nv,
_temp1047); Cyc_Toc_exp_to_c( nv, _temp1045);( void*)( _temp1047->r=( void*)((
void*)( Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp((
void*) _temp1047->r, 0), 0))->r));{ void* _temp1280= e1_typ; struct Cyc_List_List*
_temp1290; struct Cyc_Absyn_Exp* _temp1292; struct Cyc_Absyn_Tqual* _temp1294;
void* _temp1296; struct Cyc_Absyn_PtrInfo _temp1298; struct Cyc_Absyn_Conref*
_temp1300; struct Cyc_Absyn_Tqual* _temp1302; struct Cyc_Absyn_Conref* _temp1304;
void* _temp1306; void* _temp1308; _LL1282: if(( unsigned int) _temp1280 > 4u?((
struct _enum_struct*) _temp1280)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1291:
_temp1290=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1280)->f1; goto _LL1283;} else{ goto _LL1284;} _LL1284: if(( unsigned int)
_temp1280 > 4u?(( struct _enum_struct*) _temp1280)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1297: _temp1296=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1280)->f1;
goto _LL1295; _LL1295: _temp1294=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1280)->f2; goto _LL1293; _LL1293: _temp1292=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1280)->f3; goto _LL1285;} else{ goto
_LL1286;} _LL1286: if(( unsigned int) _temp1280 > 4u?(( struct _enum_struct*)
_temp1280)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1299: _temp1298=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1280)->f1;
_LL1309: _temp1308=( void*) _temp1298.elt_typ; goto _LL1307; _LL1307: _temp1306=(
void*) _temp1298.rgn_typ; goto _LL1305; _LL1305: _temp1304=( struct Cyc_Absyn_Conref*)
_temp1298.nullable; goto _LL1303; _LL1303: _temp1302=( struct Cyc_Absyn_Tqual*)
_temp1298.tq; goto _LL1301; _LL1301: _temp1300=( struct Cyc_Absyn_Conref*)
_temp1298.bounds; goto _LL1287;} else{ goto _LL1288;} _LL1288: goto _LL1289;
_LL1283: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1045) +(
unsigned int) 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1047,({ struct _tagged_string* _temp1310=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1310[ 0]= xprintf("f%d",(
int) i); _temp1310;}), 0))->r)); goto _LL1281;} _LL1285:( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1047, _temp1045, _temp1292, _temp1296,
_temp1294, 0))->r)); goto _LL1281; _LL1287:{ void* _temp1311=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1300); struct Cyc_Absyn_Exp*
_temp1317; _LL1313: if(( unsigned int) _temp1311 > 1u?(( struct _enum_struct*)
_temp1311)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1318: _temp1317=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1311)->f1; goto _LL1314;} else{ goto
_LL1315;} _LL1315: if( _temp1311 == Cyc_Absyn_Unknown_b){ goto _LL1316;} else{
goto _LL1312;} _LL1314:( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1047, _temp1045, _temp1317, _temp1308, _temp1302, 0))->r)); goto _LL1312;
_LL1316: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1308); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1302); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1302); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1045, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1047, s1, 0), 0))->r)); goto _LL1312;} _LL1312:;}
goto _LL1281; _LL1289:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1319=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1320; _temp1320.curr= _temp1319; _temp1320.base=
_temp1319; _temp1320.last_plus_one= _temp1319 + 49; _temp1320;})); goto _LL1281;
_LL1281:;} goto _LL891;} _LL939: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp1049))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1049); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1049 != 0; _temp1049= _temp1049->tl, i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp1049->hd);{ struct Cyc_List_List* des=({ struct
Cyc_List_List* _temp1321=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1321->hd=( void*)({ struct Cyc_Absyn_FieldName_struct* _temp1322=( struct
Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1322->tag= Cyc_Absyn_FieldName_tag; _temp1322->f1=({ struct _tagged_string*
_temp1323=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1323[ 0]= xprintf("f%d", i); _temp1323;});( void*) _temp1322;});
_temp1321->tl= 0; _temp1321;}); dles=({ struct Cyc_List_List* _temp1324=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1324->hd=( void*)({
struct _tuple7* _temp1325=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1325->f1=( struct Cyc_List_List*) des; _temp1325->f2=( struct Cyc_Absyn_Exp*)
_temp1049->hd; _temp1325;}); _temp1324->tl= dles; _temp1324;});}}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp(
0, dles, 0), 0))->r));} goto _LL891; _LL941:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1326=( char*)"compoundlit";
struct _tagged_string _temp1327; _temp1327.curr= _temp1326; _temp1327.base=
_temp1326; _temp1327.last_plus_one= _temp1326 + 12; _temp1327;})); goto _LL891;
_LL943: if( nv->toplevel? 1: ! _temp1057){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1055, 0))->r));{ struct Cyc_List_List* dles= _temp1055; for( 0; dles !=
0; dles= dles->tl){ struct _tuple7 _temp1330; struct Cyc_Absyn_Exp* _temp1331;
struct Cyc_List_List* _temp1333; struct _tuple7* _temp1328=( struct _tuple7*)
dles->hd; _temp1330=* _temp1328; _LL1334: _temp1333= _temp1330.f1; goto _LL1332;
_LL1332: _temp1331= _temp1330.f2; goto _LL1329; _LL1329: Cyc_Toc_exp_to_c( nv,
_temp1331);}}} else{ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1055) - 1;{ struct Cyc_List_List* dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1055);
for( 0; dles != 0; dles= dles->tl){ struct _tuple7 _temp1337; struct Cyc_Absyn_Exp*
_temp1338; struct Cyc_List_List* _temp1340; struct _tuple7* _temp1335=( struct
_tuple7*) dles->hd; _temp1337=* _temp1335; _LL1341: _temp1340= _temp1337.f1;
goto _LL1339; _LL1339: _temp1338= _temp1337.f2; goto _LL1336; _LL1336: { struct
Cyc_Absyn_Exp* e_index; if( _temp1340 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if( _temp1340->tl != 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1342=( char*)"multiple designators in array";
struct _tagged_string _temp1343; _temp1343.curr= _temp1342; _temp1343.base=
_temp1342; _temp1343.last_plus_one= _temp1342 + 30; _temp1343;}));}{ void* d=(
void*) _temp1340->hd; void* _temp1344= d; struct Cyc_Absyn_Exp* _temp1350;
struct _tagged_string* _temp1352; _LL1346: if((( struct _enum_struct*) _temp1344)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1351: _temp1350=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1344)->f1; goto _LL1347;} else{ goto
_LL1348;} _LL1348: if((( struct _enum_struct*) _temp1344)->tag == Cyc_Absyn_FieldName_tag){
_LL1353: _temp1352=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1344)->f1; goto _LL1349;} else{ goto _LL1345;} _LL1347: Cyc_Toc_exp_to_c(
nv, _temp1350); e_index= _temp1350; goto _LL1345; _LL1349: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1354=( char*)"field name designators in array"; struct _tagged_string
_temp1355; _temp1355.curr= _temp1354; _temp1355.base= _temp1354; _temp1355.last_plus_one=
_temp1354 + 32; _temp1355;})); goto _LL1345; _LL1345:;}} Cyc_Toc_exp_to_c( nv,
_temp1338); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1338, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1356= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1362; struct Cyc_Absyn_Conref* _temp1364; struct Cyc_Absyn_Tqual* _temp1366;
struct Cyc_Absyn_Conref* _temp1368; void* _temp1370; void* _temp1372; _LL1358:
if(( unsigned int) _temp1356 > 4u?(( struct _enum_struct*) _temp1356)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1363: _temp1362=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1356)->f1; _LL1373: _temp1372=( void*)
_temp1362.elt_typ; goto _LL1371; _LL1371: _temp1370=( void*) _temp1362.rgn_typ;
goto _LL1369; _LL1369: _temp1368=( struct Cyc_Absyn_Conref*) _temp1362.nullable;
goto _LL1367; _LL1367: _temp1366=( struct Cyc_Absyn_Tqual*) _temp1362.tq; goto
_LL1365; _LL1365: _temp1364=( struct Cyc_Absyn_Conref*) _temp1362.bounds; goto
_LL1359;} else{ goto _LL1360;} _LL1360: goto _LL1361; _LL1359: old_elt_typ=
_temp1372; goto _LL1357; _LL1361: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1374=( char*)"exp_to_c:array expression doesn't have array type";
struct _tagged_string _temp1375; _temp1375.curr= _temp1374; _temp1375.base=
_temp1374; _temp1375.last_plus_one= _temp1374 + 50; _temp1375;})); goto _LL1357;
_LL1357:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1055), 0), 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ,
e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r));}}} goto _LL891; _LL945: { struct _tuple1*
x1= _temp1063->name; int is_tagged_ptr= 0;{ void* _temp1376= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1382; struct Cyc_Absyn_Conref* _temp1384;
struct Cyc_Absyn_Tqual* _temp1386; struct Cyc_Absyn_Conref* _temp1388; void*
_temp1390; void* _temp1392; _LL1378: if(( unsigned int) _temp1376 > 4u?(( struct
_enum_struct*) _temp1376)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1383:
_temp1382=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1376)->f1; _LL1393: _temp1392=( void*) _temp1382.elt_typ; goto _LL1391;
_LL1391: _temp1390=( void*) _temp1382.rgn_typ; goto _LL1389; _LL1389: _temp1388=(
struct Cyc_Absyn_Conref*) _temp1382.nullable; goto _LL1387; _LL1387: _temp1386=(
struct Cyc_Absyn_Tqual*) _temp1382.tq; goto _LL1385; _LL1385: _temp1384=( struct
Cyc_Absyn_Conref*) _temp1382.bounds; goto _LL1379;} else{ goto _LL1380;} _LL1380:
goto _LL1381; _LL1379:{ void* _temp1394=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1384))->v; void*
_temp1400; _LL1396: if(( unsigned int) _temp1394 > 1u?(( struct _enum_struct*)
_temp1394)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1401: _temp1400=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1394)->f1; if( _temp1400 == Cyc_Absyn_Unknown_b){
goto _LL1397;} else{ goto _LL1398;}} else{ goto _LL1398;} _LL1398: goto _LL1399;
_LL1397: is_tagged_ptr= 1; goto _LL1395; _LL1399: goto _LL1395; _LL1395:;} goto
_LL1377; _LL1381:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1402=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base=
_temp1402; _temp1403.last_plus_one= _temp1402 + 42; _temp1403;})); goto _LL1377;
_LL1377:;}{ void* old_elt_typ=( void*)( _temp1059->topt)->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1061);{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp(
x1,({ struct Cyc_Absyn_Local_b_struct* _temp1404=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1404->tag= Cyc_Absyn_Local_b_tag;
_temp1404->f1= _temp1063;( void*) _temp1404;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1059);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1059, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1405=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1405->hd=( void*)({ struct _tuple10* _temp1406=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1406->f1= max;
_temp1406->f2= Cyc_Absyn_uint_t; _temp1406->f3=( struct Cyc_Absyn_Exp*)
_temp1061; _temp1406;}); _temp1405->tl= 0; _temp1405;}); struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0)), 0); decls=({
struct Cyc_List_List* _temp1407=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1407->hd=( void*)({ struct _tuple10* _temp1408=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1408->f1= a;
_temp1408->f2= ptr_typ; _temp1408->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1408;});
_temp1407->tl= decls; _temp1407;}); decls=({ struct Cyc_List_List* _temp1409=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1409->hd=(
void*)({ struct _tuple10* _temp1410=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1410->f1= x1; _temp1410->f2= Cyc_Absyn_uint_t; _temp1410->f3=
0; _temp1410;}); _temp1409->tl= decls; _temp1409;}); if( is_tagged_ptr){ struct
_tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp*
binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1411=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1411->hd=(
void*)({ struct _tuple7* _temp1416=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp1416->f1= 0; _temp1416->f2= Cyc_Absyn_var_exp( a, 0); _temp1416;});
_temp1411->tl=({ struct Cyc_List_List* _temp1412=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1412->hd=( void*)({ struct
_tuple7* _temp1415=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1415->f1= 0; _temp1415->f2= Cyc_Absyn_var_exp( a, 0); _temp1415;});
_temp1412->tl=({ struct Cyc_List_List* _temp1413=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1413->hd=( void*)({ struct
_tuple7* _temp1414=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1414->f1= 0; _temp1414->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1414;}); _temp1413->tl= 0; _temp1413;});
_temp1412;}); _temp1411;}), 0); decls=({ struct Cyc_List_List* _temp1417=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1417->hd=(
void*)({ struct _tuple10* _temp1418=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1418->f1= b; _temp1418->f2= t; _temp1418->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1418;}); _temp1417->tl= decls; _temp1417;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Exp* _temp1421; void*
_temp1423; struct _tuple1* _temp1425; struct _tuple10 _temp1419=*(( struct
_tuple10*) ds->hd); _LL1426: _temp1425= _temp1419.f1; goto _LL1424; _LL1424:
_temp1423= _temp1419.f2; goto _LL1422; _LL1422: _temp1421= _temp1419.f3; goto
_LL1420; _LL1420: s= Cyc_Absyn_declare_stmt( _temp1425, _temp1423, _temp1421, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r)); goto
_LL891;}}}} _LL947: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, _temp1067, _temp1071))->r));} else{ struct _tuple1* n= _temp1071;
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp1428= _temp1065 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1427= Cyc_Absynpp_qvar2string(
_temp1071); xprintf("no structdecl %.*s", _temp1427.last_plus_one - _temp1427.curr,
_temp1427.curr);})): _temp1065; if( _temp1428 == 0){ _throw( Null_Exception);}
_temp1428;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1067,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields= fields->tl){ struct _tuple12 _temp1431;
struct Cyc_Absyn_Exp* _temp1432; struct Cyc_Absyn_Structfield* _temp1434; struct
_tuple12* _temp1429=( struct _tuple12*) fields->hd; _temp1431=* _temp1429;
_LL1435: _temp1434= _temp1431.f1; goto _LL1433; _LL1433: _temp1432= _temp1431.f2;
goto _LL1430; _LL1430: { void* fieldtyp=( void*) _temp1434->type; Cyc_Toc_exp_to_c(
nv, _temp1432); if( Cyc_Toc_is_void_star( fieldtyp)){( void*)( _temp1432->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1432->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0, _temp1067, 0), 0))->r));}
goto _LL891; _LL949: { struct _tuple1* qv= _temp1073->name; if( ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* fields= _temp1075->fields == 0? 0:( struct
Cyc_List_List*)( _temp1075->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,(( struct
Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs == 0){ tag_count ++;} fields= fields->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0), 0))->r));} goto _LL891;} _LL951: { struct _tuple1*
qv= _temp1083->name; struct Cyc_List_List* tqts= _temp1083->typs; struct _tuple1*
x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0);
void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1436=( char*)"_struct"; struct _tagged_string
_temp1437; _temp1437.curr= _temp1436; _temp1437.base= _temp1436; _temp1437.last_plus_one=
_temp1436 + 8; _temp1437;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Toc_mt_tq); if( nv->toplevel){ int tag_count= 0; struct Cyc_List_List*
fields= _temp1085->fields == 0? 0:( struct Cyc_List_List*)( _temp1085->fields)->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Enumfield*) fields->hd)->name)
!= 0) { if((( struct Cyc_Absyn_Enumfield*) fields->hd)->typs != 0){ tag_count ++;}
fields= fields->tl;}{ struct Cyc_List_List* dles= 0; for( 0; _temp1087 != 0;
_temp1087= _temp1087->tl, tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct
Cyc_Absyn_Exp*) _temp1087->hd; void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple5*) tqts->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1438=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1438[ 0]=({ struct Cyc_List_List _temp1439;
_temp1439.hd=( void*)({ struct _tuple7* _temp1440=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7) * 1); _temp1440[ 0]=({ struct _tuple7 _temp1441;
_temp1441.f1= 0; _temp1441.f2= cur_e; _temp1441;}); _temp1440;}); _temp1439.tl=
dles; _temp1439;}); _temp1438;});}{ struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0); dles=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1442=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1442[ 0]=({ struct Cyc_List_List _temp1443; _temp1443.hd=( void*)({ struct
_tuple7* _temp1444=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp1444[ 0]=({ struct _tuple7 _temp1445; _temp1445.f1= 0; _temp1445.f2=
tag_exp; _temp1445;}); _temp1444;}); _temp1443.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1443;}); _temp1442;});{
struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles, 0); struct
Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x, strct_typ,( struct Cyc_Absyn_Exp*)
init_exp); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List* _temp1446=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1446->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1447=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1447->tag= Cyc_Absyn_Var_d_tag;
_temp1447->f1= vd;( void*) _temp1447;}), 0); _temp1446->tl= Cyc_Toc_temp_topdecls;
_temp1446;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( pstrct_typ,
Cyc_Absyn_address_exp( xexp, 0), 0))->r));}}}} else{ struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_sizeoftyp_exp( strct_typ, 0); int atomic= 1; struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1448=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1448->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structarrow_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1449=( char*)"_tag"; struct
_tagged_string _temp1450; _temp1450.curr= _temp1449; _temp1450.base= _temp1449;
_temp1450.last_plus_one= _temp1449 + 5; _temp1450;})), 0), 0); _temp1448->tl= 0;
_temp1448;});{ int i= 1; for( 0; _temp1087 != 0;( _temp1087= _temp1087->tl, i ++),
tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1087->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple5*) tqts->hd)).f2); if( !
Cyc_Toc_atomic_typ(( void*)( cur_e->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,
cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp(
xexp,({ struct _tagged_string* _temp1451=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1451[ 0]= xprintf("f%d", i); _temp1451;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1452->hd=( void*) a; _temp1452->tl=(
struct Cyc_List_List*) rev_assign; _temp1452;});}}}{ struct Cyc_Absyn_Exp* e1=
Cyc_Absyn_cast_exp( pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr(
e2), 0); struct Cyc_Absyn_Stmt* final_cast= Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c( old_typ), xexp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1453=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1453->hd=( void*) final_cast; _temp1453->tl=( struct Cyc_List_List*)
rev_assign; _temp1453;})), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}}
goto _LL891;} _LL953: { struct _tuple1* qv= _temp1093->name; struct Cyc_List_List*
tqts= _temp1093->typs; struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1454=( char*)"_struct"; struct
_tagged_string _temp1455; _temp1455.curr= _temp1454; _temp1455.base= _temp1454;
_temp1455.last_plus_one= _temp1454 + 8; _temp1455;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ(
strct_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1456=( char*)"_tag"; struct
_tagged_string _temp1457; _temp1457.curr= _temp1456; _temp1457.base= _temp1456;
_temp1457.last_plus_one= _temp1456 + 5; _temp1457;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1097); dles=({ struct Cyc_List_List*
_temp1458=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1458->hd=( void*)({ struct _tuple7* _temp1459=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1459->f1= 0; _temp1459->f2= tag_exp; _temp1459;});
_temp1458->tl= dles; _temp1458;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1460=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1460->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1461=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1461->tag= Cyc_Absyn_Var_d_tag; _temp1461->f1= vd;( void*) _temp1461;}), 0);
_temp1460->tl= Cyc_Toc_temp_topdecls; _temp1460;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), Cyc_Absyn_address_exp(
xexp, 0), 0))->r));}} else{ struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp(
strct_typ, 0); int atomic= 1; struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1462=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1462->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1463=( char*)"_tag"; struct _tagged_string
_temp1464; _temp1464.curr= _temp1463; _temp1464.base= _temp1463; _temp1464.last_plus_one=
_temp1463 + 5; _temp1464;})), 0), 0); _temp1462->tl= 0; _temp1462;});{ int i= 1;
for( 0; _temp1097 != 0;( _temp1097= _temp1097->tl, i ++), tqts= tqts->tl){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1097->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple5*) tqts->hd)).f2); if( ! Cyc_Toc_atomic_typ((
void*)( cur_e->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp( xexp,({ struct
_tagged_string* _temp1465=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1465[ 0]= xprintf("f%d", i); _temp1465;}), 0), cur_e,
0); rev_assign=({ struct Cyc_List_List* _temp1466=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1466->hd=( void*) a; _temp1466->tl=(
struct Cyc_List_List*) rev_assign; _temp1466;});}}}{ struct Cyc_Absyn_Exp* e1=
Cyc_Absyn_cast_exp( pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr(
e2), 0); struct Cyc_Absyn_Stmt* final_cast= Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c( old_typ), xexp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1467=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1467->hd=( void*) final_cast; _temp1467->tl=( struct Cyc_List_List*)
rev_assign; _temp1467;})), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}}
goto _LL891;} _LL955: { void* t_c= Cyc_Toc_typ_to_c( _temp1103);( void*)( e->r=(
void*)(( void*)( Cyc_Toc_malloc_exp( _temp1103, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));
goto _LL891;} _LL957:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1468=( char*)"unimplemented Malloc_e form";
struct _tagged_string _temp1469; _temp1469.curr= _temp1468; _temp1469.base=
_temp1468; _temp1469.last_plus_one= _temp1468 + 28; _temp1469;})); goto _LL891;
_LL959: Cyc_Toc_stmt_to_c( nv, _temp1107); goto _LL891; _LL961:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1470=( char*)"UnresolvedMem"; struct _tagged_string _temp1471; _temp1471.curr=
_temp1470; _temp1471.base= _temp1470; _temp1471.last_plus_one= _temp1470 + 14;
_temp1471;})); goto _LL891; _LL963:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1472=( char*)"codegen"; struct
_tagged_string _temp1473; _temp1473.curr= _temp1472; _temp1473.base= _temp1472;
_temp1473.last_plus_one= _temp1472 + 8; _temp1473;})); goto _LL891; _LL965:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1474=( char*)"fill"; struct _tagged_string _temp1475; _temp1475.curr=
_temp1474; _temp1475.base= _temp1474; _temp1475.last_plus_one= _temp1474 + 5;
_temp1475;})); goto _LL891; _LL891:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1476=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1508; struct Cyc_Absyn_Vardecl _temp1510; struct Cyc_List_List* _temp1511;
struct Cyc_Core_Opt* _temp1513; int _temp1515; struct Cyc_Absyn_Exp* _temp1517;
void* _temp1519; struct Cyc_Absyn_Tqual* _temp1521; struct _tuple1* _temp1523;
void* _temp1525; struct Cyc_Absyn_Vardecl* _temp1527; int _temp1529; void*
_temp1531; char _temp1533; struct _tagged_string _temp1535; struct Cyc_Absyn_Enumfield*
_temp1537; struct Cyc_Absyn_Enumdecl* _temp1539; struct Cyc_List_List* _temp1541;
struct Cyc_List_List* _temp1543; struct Cyc_Core_Opt* _temp1545; struct _tuple1*
_temp1547; struct Cyc_Absyn_Enumfield* _temp1549; struct Cyc_Absyn_Xenumdecl*
_temp1551; struct Cyc_List_List* _temp1553; struct Cyc_List_List* _temp1555;
struct _tuple1* _temp1557; struct Cyc_List_List* _temp1559; struct Cyc_List_List*
_temp1561; struct Cyc_List_List* _temp1563; struct Cyc_Core_Opt* _temp1565;
struct Cyc_Absyn_Structdecl* _temp1567; struct Cyc_Absyn_Pat* _temp1569; struct
_tuple1* _temp1571; struct Cyc_List_List* _temp1573; struct Cyc_List_List*
_temp1575; struct _tuple1* _temp1577; struct Cyc_List_List* _temp1579; struct
Cyc_List_List* _temp1581; struct _tuple1* _temp1583; _LL1478: if( _temp1476 ==
Cyc_Absyn_Wild_p){ goto _LL1479;} else{ goto _LL1480;} _LL1480: if((
unsigned int) _temp1476 > 2u?(( struct _enum_struct*) _temp1476)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL1509: _temp1508=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1476)->f1; _temp1510=* _temp1508; _LL1526: _temp1525=( void*) _temp1510.sc;
goto _LL1524; _LL1524: _temp1523=( struct _tuple1*) _temp1510.name; goto _LL1522;
_LL1522: _temp1521=( struct Cyc_Absyn_Tqual*) _temp1510.tq; goto _LL1520;
_LL1520: _temp1519=( void*) _temp1510.type; goto _LL1518; _LL1518: _temp1517=(
struct Cyc_Absyn_Exp*) _temp1510.initializer; goto _LL1516; _LL1516: _temp1515=(
int) _temp1510.shadow; goto _LL1514; _LL1514: _temp1513=( struct Cyc_Core_Opt*)
_temp1510.region; goto _LL1512; _LL1512: _temp1511=( struct Cyc_List_List*)
_temp1510.attributes; goto _LL1481;} else{ goto _LL1482;} _LL1482: if((
unsigned int) _temp1476 > 2u?(( struct _enum_struct*) _temp1476)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1528: _temp1527=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1476)->f1; goto _LL1483;} else{ goto _LL1484;} _LL1484: if( _temp1476 ==
Cyc_Absyn_Null_p){ goto _LL1485;} else{ goto _LL1486;} _LL1486: if((
unsigned int) _temp1476 > 2u?(( struct _enum_struct*) _temp1476)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1532: _temp1531=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1476)->f1;
goto _LL1530; _LL1530: _temp1529=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1476)->f2; goto _LL1487;} else{ goto _LL1488;} _LL1488: if(( unsigned int)
_temp1476 > 2u?(( struct _enum_struct*) _temp1476)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1534: _temp1533=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1476)->f1;
goto _LL1489;} else{ goto _LL1490;} _LL1490: if(( unsigned int) _temp1476 > 2u?((
struct _enum_struct*) _temp1476)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1536:
_temp1535=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1476)->f1;
goto _LL1491;} else{ goto _LL1492;} _LL1492: if(( unsigned int) _temp1476 > 2u?((
struct _enum_struct*) _temp1476)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1548:
_temp1547=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1476)->f1;
goto _LL1546; _LL1546: _temp1545=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1476)->f2; goto _LL1544; _LL1544: _temp1543=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1476)->f3; goto _LL1542; _LL1542:
_temp1541=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1476)->f4;
goto _LL1540; _LL1540: _temp1539=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1476)->f5; goto _LL1538; _LL1538: _temp1537=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1476)->f6; goto _LL1493;} else{ goto
_LL1494;} _LL1494: if(( unsigned int) _temp1476 > 2u?(( struct _enum_struct*)
_temp1476)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1558: _temp1557=( struct
_tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1476)->f1; goto _LL1556;
_LL1556: _temp1555=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1476)->f2; goto _LL1554; _LL1554: _temp1553=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1476)->f3; goto _LL1552; _LL1552:
_temp1551=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1476)->f4; goto _LL1550; _LL1550: _temp1549=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1476)->f5; goto _LL1495;} else{ goto
_LL1496;} _LL1496: if(( unsigned int) _temp1476 > 2u?(( struct _enum_struct*)
_temp1476)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1560: _temp1559=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp1476)->f1; goto _LL1497;} else{ goto
_LL1498;} _LL1498: if(( unsigned int) _temp1476 > 2u?(( struct _enum_struct*)
_temp1476)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL1568: _temp1567=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1476)->f1; goto _LL1566; _LL1566:
_temp1565=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1476)->f2;
goto _LL1564; _LL1564: _temp1563=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1476)->f3; goto _LL1562; _LL1562: _temp1561=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1476)->f4; goto _LL1499;} else{ goto
_LL1500;} _LL1500: if(( unsigned int) _temp1476 > 2u?(( struct _enum_struct*)
_temp1476)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1570: _temp1569=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1476)->f1; goto _LL1501;} else{ goto
_LL1502;} _LL1502: if(( unsigned int) _temp1476 > 2u?(( struct _enum_struct*)
_temp1476)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1572: _temp1571=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1476)->f1; goto _LL1503;}
else{ goto _LL1504;} _LL1504: if(( unsigned int) _temp1476 > 2u?(( struct
_enum_struct*) _temp1476)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1578:
_temp1577=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1476)->f1;
goto _LL1576; _LL1576: _temp1575=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1476)->f2; goto _LL1574; _LL1574: _temp1573=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1476)->f3; goto _LL1505;} else{
goto _LL1506;} _LL1506: if(( unsigned int) _temp1476 > 2u?(( struct _enum_struct*)
_temp1476)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1584: _temp1583=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1476)->f1;
goto _LL1582; _LL1582: _temp1581=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1476)->f2; goto _LL1580; _LL1580: _temp1579=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1476)->f3; goto _LL1507;} else{
goto _LL1477;} _LL1479: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1477;
_LL1481: nv= Cyc_Toc_add_varmap( nv, _temp1523, r); s= Cyc_Absyn_goto_stmt(
succ_lab, 0); goto _LL1477; _LL1483: { struct _tuple1* v= Cyc_Toc_temp_var();
decls=({ struct Cyc_List_List* _temp1585=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1585->hd=( void*)({ struct _tuple14*
_temp1586=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1586->f1=
v; _temp1586->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1586;}); _temp1585->tl= decls; _temp1585;}); nv= Cyc_Toc_add_varmap( nv,
_temp1527->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1477;} _LL1485: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1477; _LL1487: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1531, _temp1529, 0), succ_lab, fail_lab); goto _LL1477;
_LL1489: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1533, 0),
succ_lab, fail_lab); goto _LL1477; _LL1491: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1535, 0), succ_lab, fail_lab); goto _LL1477; _LL1493: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1541);
struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1537->name,( struct
_tagged_string)({ char* _temp1587=( char*)"_struct"; struct _tagged_string
_temp1588; _temp1588.curr= _temp1587; _temp1588.base= _temp1587; _temp1588.last_plus_one=
_temp1587 + 8; _temp1588;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1541); for( 0; rps != 0; rps= rps->tl, cnt --){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=(
void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string*
lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp1589=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1589->hd=( void*)({
struct _tuple14* _temp1590=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp1590->f1= v; _temp1590->f2= t2c; _temp1590;}); _temp1589->tl=
decls; _temp1589;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1591=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1591[ 0]= xprintf("f%d", cnt); _temp1591;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp(
t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1592=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1592[ 0]= xprintf("f%d", cnt); _temp1592;}), 0), 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List* _temp1593=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1593->hd=(
void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1593->tl= ss; _temp1593;});
succ_lab= lab;}}}} if( _temp1541 == 0){ s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_var_exp(
_temp1547, 0), succ_lab, fail_lab);} else{ unsigned int max_tag= 0;{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)( _temp1539->fields)->v; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->typs
== 0){ if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1594=( char*)"can't deal with explicit tags yet";
struct _tagged_string _temp1595; _temp1595.curr= _temp1594; _temp1595.base=
_temp1594; _temp1595.last_plus_one= _temp1594 + 34; _temp1595;}));} max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__enum_struct_sp),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp( e3,
Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1547,( struct _tagged_string)({ char* _temp1596=(
char*)"_tag"; struct _tagged_string _temp1597; _temp1597.curr= _temp1596;
_temp1597.base= _temp1596; _temp1597.last_plus_one= _temp1596 + 5; _temp1597;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag >( unsigned int) 0? Cyc_Absyn_and_exp(
e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}}
goto _LL1477;} _LL1495: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1553); struct _tuple1* xfstrct= Cyc_Toc_collapse_qvar_tag(
_temp1549->name,( struct _tagged_string)({ char* _temp1598=( char*)"_struct";
struct _tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base=
_temp1598; _temp1599.last_plus_one= _temp1598 + 8; _temp1599;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1553); for( 0; rps != 0; rps= rps->tl, cnt --){ struct
Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1600=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1600->hd=( void*)({ struct _tuple14*
_temp1601=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1601->f1=
v; _temp1601->f2= Cyc_Toc_typ_to_c( t2); _temp1601;}); _temp1600->tl= decls;
_temp1600;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1602=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1602[ 0]= xprintf("f%d", cnt); _temp1602;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1603=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1603[ 0]= xprintf("f%d", cnt);
_temp1603;}), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0);
ss=({ struct Cyc_List_List* _temp1604=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1604->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1604->tl= ss; _temp1604;}); succ_lab= lab;}}}} if( _temp1553 == 0){
s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0),
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1557,( struct _tagged_string)({
char* _temp1605=( char*)"_tag"; struct _tagged_string _temp1606; _temp1606.curr=
_temp1605; _temp1606.base= _temp1605; _temp1606.last_plus_one= _temp1605 + 5;
_temp1606;})), 0), succ_lab, fail_lab);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structmember_exp( e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e=
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1557,( struct _tagged_string)({
char* _temp1607=( char*)"_tag"; struct _tagged_string _temp1608; _temp1608.curr=
_temp1607; _temp1608.base= _temp1607; _temp1608.last_plus_one= _temp1607 + 5;
_temp1608;})), 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0),
Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} goto
_LL1477;} _LL1497: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1559);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1559); for( 0; rps != 0; rps= rps->tl,
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct
_tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1609=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1609->hd=( void*)({ struct _tuple14* _temp1610=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1610->f1= v; _temp1610->f2= Cyc_Toc_typ_to_c(
t2); _temp1610;}); _temp1609->tl= decls; _temp1609;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1611=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1611[ 0]= xprintf("f%d", cnt);
_temp1611;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1612=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1612[ 0]= xprintf("f%d", cnt); _temp1612;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1613=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1613->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1613->tl= ss; _temp1613;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1477;} _LL1499: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1561); for( 0; dlps != 0; dlps= dlps->tl){ struct _tuple15* tup=( struct
_tuple15*) dlps->hd; struct Cyc_Absyn_Pat* p2=(* tup).f2; struct _tagged_string*
f;{ void* _temp1614=( void*)((* tup).f1)->hd; struct _tagged_string* _temp1620;
_LL1616: if((( struct _enum_struct*) _temp1614)->tag == Cyc_Absyn_FieldName_tag){
_LL1621: _temp1620=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1614)->f1; goto _LL1617;} else{ goto _LL1618;} _LL1618: goto _LL1619;
_LL1617: f= _temp1620; goto _LL1615; _LL1619:( void) _throw(({ struct Cyc_Toc_Match_error_struct*
_temp1622=( struct Cyc_Toc_Match_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Match_error_struct)); _temp1622->tag= Cyc_Toc_Match_error_tag;( struct
_xenum_struct*) _temp1622;})); _LL1615:;}{ struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1623=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1623->hd=( void*)({ struct _tuple14* _temp1624=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1624->f1= v; _temp1624->f2= t2c;
_temp1624;}); _temp1623->tl= decls; _temp1623;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r,
f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1625=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1625->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1625->tl= ss; _temp1625;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1477;} _LL1501: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( _temp1569->topt)->v; decls=({ struct Cyc_List_List* _temp1626=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1626->hd=(
void*)({ struct _tuple14* _temp1627=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp1627->f1= v; _temp1627->f2= Cyc_Toc_typ_to_c( t2);
_temp1627;}); _temp1626->tl= decls; _temp1626;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path, 0), _temp1569,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0), s2, 0); if( Cyc_Toc_really_is_nullable(
t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);} else{ s= s3;} goto
_LL1477;}}} _LL1503: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1628=( char*)"unknownid";
struct _tagged_string _temp1629; _temp1629.curr= _temp1628; _temp1629.base=
_temp1628; _temp1629.last_plus_one= _temp1628 + 10; _temp1629;})); goto _LL1477;
_LL1505: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1630=( char*)"unknowncall"; struct
_tagged_string _temp1631; _temp1631.curr= _temp1630; _temp1631.base= _temp1630;
_temp1631.last_plus_one= _temp1630 + 12; _temp1631;})); goto _LL1477; _LL1507: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1632=( char*)"unknownfields"; struct _tagged_string
_temp1633; _temp1633.curr= _temp1632; _temp1633.base= _temp1632; _temp1633.last_plus_one=
_temp1632 + 14; _temp1633;})); goto _LL1477; _LL1477:;} return({ struct _tuple13
_temp1634; _temp1634.f1= nv; _temp1634.f2= decls; _temp1634.f3= s; _temp1634;});}
static struct _tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){
return({ struct _tuple16* _temp1635=( struct _tuple16*) GC_malloc( sizeof(
struct _tuple16)); _temp1635->f1= Cyc_Toc_fresh_label(); _temp1635->f2= Cyc_Toc_fresh_label();
_temp1635->f3= sc; _temp1635;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)( e->topt)->v; int
leave_as_switch;{ void* _temp1636= Cyc_Tcutil_compress( t); void* _temp1642;
void* _temp1644; _LL1638: if(( unsigned int) _temp1636 > 4u?(( struct
_enum_struct*) _temp1636)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1645: _temp1644=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f1; goto _LL1643; _LL1643:
_temp1642=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f2; goto
_LL1639;} else{ goto _LL1640;} _LL1640: goto _LL1641; _LL1639: leave_as_switch=
1; goto _LL1637; _LL1641: leave_as_switch= 0; goto _LL1637; _LL1637:;}{ struct
Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1= scs1->tl){ if(( struct Cyc_List_List*)(((
struct Cyc_Absyn_Switch_clause*) scs1->hd)->pat_vars)->v != 0){ leave_as_switch=
0; break;}}} if( leave_as_switch){ struct _tagged_string* next_l= Cyc_Toc_fresh_label();{
struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1= scs1->tl){ struct Cyc_Absyn_Stmt*
s=(( struct Cyc_Absyn_Switch_clause*) scs1->hd)->body;(( struct Cyc_Absyn_Switch_clause*)
scs1->hd)->body= Cyc_Absyn_label_stmt( next_l, s, 0); next_l= Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env( nv, next_l), s);}} return;}{
struct _tuple1* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp(
v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0); struct
_tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2= lscs2->tl){ struct
Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*) lscs2->hd)).f3; struct
_tagged_string* fail_lab= lscs2->tl == 0? end_l:(*(( struct _tuple16*)( lscs2->tl)->hd)).f1;
struct _tagged_string* succ_lab=(*(( struct _tuple16*) lscs2->hd)).f2; if( sc->where_clause
== 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1646=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1646->hd=( void*) res.f1;
_temp1646->tl= nvs; _temp1646;}); decls= res.f2; test_stmts=({ struct Cyc_List_List*
_temp1647=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1647->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*) lscs2->hd)).f1,
res.f3, 0); _temp1647->tl= test_stmts; _temp1647;});} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1648= sc->where_clause;
if( _temp1648 == 0){ _throw( Null_Exception);} _temp1648;}); struct
_tagged_string* intermed_lab= Cyc_Toc_fresh_label(); struct _tuple13 res= Cyc_Toc_xlate_pat(
nv, t, r, path, sc->pattern, intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c(
res.f1, e); nvs=({ struct Cyc_List_List* _temp1649=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1649->hd=( void*) res.f1;
_temp1649->tl= nvs; _temp1649;}); decls= res.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt(
e, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( intermed_lab, s2, 0);
test_stmts=({ struct Cyc_List_List* _temp1650=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1650->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*) lscs2->hd)).f1, Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0);
_temp1650->tl= test_stmts; _temp1650;});}}}}{ struct Cyc_Absyn_Stmt* test_stmt=
Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0; lscs->tl != 0; lscs= lscs->tl, nvs= nvs->tl){ struct Cyc_Toc_Env* e=(
struct Cyc_Toc_Env*) nvs->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple16*)
lscs->hd)).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( e,
end_l,(*(( struct _tuple16*)( lscs->tl)->hd)).f2,( struct Cyc_List_List*)(((*((
struct _tuple16*)( lscs->tl)->hd)).f3)->pat_vars)->v,( struct Cyc_Toc_Env*)( nvs->tl)->hd),
s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*(( struct
_tuple16*) lscs->hd)).f2, s, 0), 0);}{ struct Cyc_Toc_Env* e=( struct Cyc_Toc_Env*)
nvs->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple16*) lscs->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env( e, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple16*) lscs->hd)).f2, s, 0), 0);}}{
struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for(
decls; decls != 0; decls= decls->tl){ struct _tuple14 _temp1653; void* _temp1654;
struct _tuple1* _temp1656; struct _tuple14* _temp1651=( struct _tuple14*) decls->hd;
_temp1653=* _temp1651; _LL1657: _temp1656= _temp1653.f1; goto _LL1655; _LL1655:
_temp1654= _temp1653.f2; goto _LL1652; _LL1652: res= Cyc_Absyn_declare_stmt(
_temp1656, _temp1654, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)( e->topt)->v),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1658=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1658[ 0]=({ struct Cyc_List_List _temp1659;
_temp1659.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0); _temp1659.tl=
0; _temp1659;}); _temp1658;}), 0), 0);} void Cyc_Toc_do_npop_before( int n,
struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}}
static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s){
while( 1) { void* _temp1660=( void*) s->r; struct Cyc_Absyn_Exp* _temp1698;
struct Cyc_Absyn_Stmt* _temp1700; struct Cyc_Absyn_Stmt* _temp1702; struct Cyc_Absyn_Exp*
_temp1704; struct Cyc_Absyn_Stmt* _temp1706; struct Cyc_Absyn_Stmt* _temp1708;
struct Cyc_Absyn_Exp* _temp1710; struct Cyc_Absyn_Stmt* _temp1712; struct
_tuple3 _temp1714; struct Cyc_Absyn_Stmt* _temp1716; struct Cyc_Absyn_Exp*
_temp1718; struct Cyc_Absyn_Stmt* _temp1720; struct Cyc_Absyn_Stmt* _temp1722;
struct Cyc_Absyn_Stmt* _temp1724; struct _tagged_string* _temp1726; struct Cyc_Absyn_Stmt*
_temp1728; struct _tuple3 _temp1730; struct Cyc_Absyn_Stmt* _temp1732; struct
Cyc_Absyn_Exp* _temp1734; struct _tuple3 _temp1736; struct Cyc_Absyn_Stmt*
_temp1738; struct Cyc_Absyn_Exp* _temp1740; struct Cyc_Absyn_Exp* _temp1742;
struct Cyc_List_List* _temp1744; struct Cyc_Absyn_Exp* _temp1746; struct Cyc_Absyn_Switch_clause**
_temp1748; struct Cyc_List_List* _temp1750; struct Cyc_Absyn_Stmt* _temp1752;
struct Cyc_Absyn_Decl* _temp1754; struct Cyc_Absyn_Stmt* _temp1756; struct Cyc_Absyn_Stmt*
_temp1758; struct Cyc_Absyn_Stmt* _temp1760; struct _tagged_string* _temp1762;
struct _tuple3 _temp1764; struct Cyc_Absyn_Stmt* _temp1766; struct Cyc_Absyn_Exp*
_temp1768; struct Cyc_Absyn_Stmt* _temp1770; struct Cyc_List_List* _temp1772;
struct Cyc_Absyn_Stmt* _temp1774; _LL1662: if( _temp1660 == Cyc_Absyn_Skip_s){
goto _LL1663;} else{ goto _LL1664;} _LL1664: if(( unsigned int) _temp1660 > 1u?((
struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1699:
_temp1698=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1660)->f1;
goto _LL1665;} else{ goto _LL1666;} _LL1666: if(( unsigned int) _temp1660 > 1u?((
struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1703:
_temp1702=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1660)->f1;
goto _LL1701; _LL1701: _temp1700=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1660)->f2; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(( unsigned int)
_temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1705: _temp1704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1660)->f1; goto _LL1669;} else{ goto _LL1670;} _LL1670: if(( unsigned int)
_temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1711: _temp1710=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1660)->f1; goto _LL1709; _LL1709: _temp1708=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1660)->f2; goto _LL1707; _LL1707:
_temp1706=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1660)->f3; goto _LL1671;} else{ goto _LL1672;} _LL1672: if(( unsigned int)
_temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1715: _temp1714=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1660)->f1; _LL1719: _temp1718= _temp1714.f1; goto _LL1717; _LL1717:
_temp1716= _temp1714.f2; goto _LL1713; _LL1713: _temp1712=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1660)->f2; goto _LL1673;} else{ goto
_LL1674;} _LL1674: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1721: _temp1720=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1660)->f1; goto _LL1675;} else{ goto
_LL1676;} _LL1676: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1723: _temp1722=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1660)->f1; goto _LL1677;} else{ goto
_LL1678;} _LL1678: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1727: _temp1726=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1660)->f1; goto _LL1725;
_LL1725: _temp1724=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1660)->f2; goto _LL1679;} else{ goto _LL1680;} _LL1680: if(( unsigned int)
_temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1743: _temp1742=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1660)->f1; goto _LL1737; _LL1737: _temp1736=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1660)->f2; _LL1741: _temp1740= _temp1736.f1; goto _LL1739; _LL1739:
_temp1738= _temp1736.f2; goto _LL1731; _LL1731: _temp1730=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1660)->f3; _LL1735: _temp1734= _temp1730.f1;
goto _LL1733; _LL1733: _temp1732= _temp1730.f2; goto _LL1729; _LL1729: _temp1728=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1660)->f4; goto
_LL1681;} else{ goto _LL1682;} _LL1682: if(( unsigned int) _temp1660 > 1u?((
struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1747:
_temp1746=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1660)->f1; goto _LL1745; _LL1745: _temp1744=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1660)->f2; goto _LL1683;} else{ goto
_LL1684;} _LL1684: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1751: _temp1750=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1660)->f1; goto _LL1749; _LL1749:
_temp1748=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1660)->f2; goto _LL1685;} else{ goto _LL1686;} _LL1686: if(( unsigned int)
_temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1755: _temp1754=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1660)->f1; goto _LL1753; _LL1753: _temp1752=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1660)->f2; goto _LL1687;} else{ goto
_LL1688;} _LL1688: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1757: _temp1756=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1660)->f1; goto _LL1689;} else{ goto
_LL1690;} _LL1690: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1759: _temp1758=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1660)->f1; goto _LL1691;} else{ goto
_LL1692;} _LL1692: if(( unsigned int) _temp1660 > 1u?(( struct _enum_struct*)
_temp1660)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1763: _temp1762=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1660)->f1; goto
_LL1761; _LL1761: _temp1760=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1660)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(( unsigned int)
_temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1771: _temp1770=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1660)->f1; goto _LL1765; _LL1765: _temp1764=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1660)->f2; _LL1769: _temp1768= _temp1764.f1; goto _LL1767; _LL1767:
_temp1766= _temp1764.f2; goto _LL1695;} else{ goto _LL1696;} _LL1696: if((
unsigned int) _temp1660 > 1u?(( struct _enum_struct*) _temp1660)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1775: _temp1774=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1660)->f1; goto _LL1773; _LL1773: _temp1772=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1660)->f2; goto _LL1697;} else{ goto
_LL1661;} _LL1663: return; _LL1665: Cyc_Toc_exp_to_c( nv, _temp1698); return;
_LL1667: Cyc_Toc_stmt_to_c( nv, _temp1702); s= _temp1700; continue; _LL1669: {
struct Cyc_Core_Opt* topt= 0; if( _temp1704 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1776=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1776->v=( void*) Cyc_Toc_typ_to_c(( void*)( _temp1704->topt)->v); _temp1776;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1777=
_temp1704; if( _temp1777 == 0){ _throw( Null_Exception);} _temp1777;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*) topt->v,
_temp1704, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth),
retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth, s);}}
return;} _LL1671: Cyc_Toc_exp_to_c( nv, _temp1710); Cyc_Toc_stmt_to_c( nv,
_temp1708); s= _temp1706; continue; _LL1673: Cyc_Toc_exp_to_c( nv, _temp1718);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1712); return; _LL1675: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)( nv->break_lab)->v, 0))->r));}{ int dest_depth= _temp1720 == 0?
0: _temp1720->try_depth; Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s);
return;} _LL1677: if( nv->continue_lab != 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_string*)( nv->continue_lab)->v, 0))->r));}
Cyc_Toc_do_npop_before( s->try_depth - _temp1722->try_depth, s); return; _LL1679:
Cyc_Toc_do_npop_before( s->try_depth - _temp1724->try_depth, s); return; _LL1681:
Cyc_Toc_exp_to_c( nv, _temp1742); Cyc_Toc_exp_to_c( nv, _temp1740); Cyc_Toc_exp_to_c(
nv, _temp1734); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1728); return;
_LL1683: Cyc_Toc_xlate_switch( nv, s, _temp1746, _temp1744); return; _LL1685:
if( nv->fallthru_info == 0){( void) _throw((( struct _xenum_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1778=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp1779; _temp1779.curr=
_temp1778; _temp1779.base= _temp1778; _temp1779.last_plus_one= _temp1778 + 29;
_temp1779;})));}{ struct _tuple8 _temp1782; struct Cyc_Dict_Dict* _temp1783;
struct Cyc_List_List* _temp1785; struct _tagged_string* _temp1787; struct
_tuple8* _temp1780=( struct _tuple8*)( nv->fallthru_info)->v; _temp1782=*
_temp1780; _LL1788: _temp1787= _temp1782.f1; goto _LL1786; _LL1786: _temp1785=
_temp1782.f2; goto _LL1784; _LL1784: _temp1783= _temp1782.f3; goto _LL1781;
_LL1781: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1787, 0); Cyc_Toc_do_npop_before(
s->try_depth -((* _temp1748)->body)->try_depth, s2);{ struct Cyc_List_List* vs2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1785);
struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp1750); for( 0; vs2 != 0; vs2= vs2->tl, es2= es2->tl){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) es2->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
_temp1783,( struct _tuple1*) vs2->hd),( struct Cyc_Absyn_Exp*) es2->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1687:{ void*
_temp1789=( void*) _temp1754->r; int _temp1795; struct Cyc_Absyn_Exp* _temp1797;
struct Cyc_Core_Opt* _temp1799; struct Cyc_Core_Opt* _temp1801; struct Cyc_Absyn_Pat*
_temp1803; _LL1791: if((( struct _enum_struct*) _temp1789)->tag == Cyc_Absyn_Let_d_tag){
_LL1804: _temp1803=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1789)->f1; goto _LL1802; _LL1802: _temp1801=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1789)->f2; goto _LL1800; _LL1800: _temp1799=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1789)->f3; goto
_LL1798; _LL1798: _temp1797=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1789)->f4; goto _LL1796; _LL1796: _temp1795=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1789)->f5; goto _LL1792;} else{ goto _LL1793;} _LL1793: goto _LL1794;
_LL1792: { int var_case= 0;{ void* _temp1805=( void*) _temp1803->r; struct Cyc_Absyn_Vardecl*
_temp1811; _LL1807: if(( unsigned int) _temp1805 > 2u?(( struct _enum_struct*)
_temp1805)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1812: _temp1811=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1805)->f1; goto _LL1808;} else{ goto
_LL1809;} _LL1809: goto _LL1810; _LL1808: var_case= 1; _temp1811->initializer=(
struct Cyc_Absyn_Exp*) _temp1797;( void*)( _temp1754->r=( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1813=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1813->tag= Cyc_Absyn_Var_d_tag; _temp1813->f1= _temp1811;( void*) _temp1813;}));
goto _LL1806; _LL1810: goto _LL1806; _LL1806:;} if( var_case){ goto _LL1794;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp1803, _temp1799,
_temp1797, _temp1795, _temp1752))->r)); goto _LL1790;} _LL1794: { struct Cyc_List_List*
_temp1817; struct Cyc_Toc_Env* _temp1819; struct _tuple17 _temp1815= Cyc_Toc_decls_to_c(
nv,({ struct Cyc_List_List* _temp1814=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1814->hd=( void*) _temp1754; _temp1814->tl=
0; _temp1814;}), 0); _LL1820: _temp1819= _temp1815.f1; goto _LL1818; _LL1818:
_temp1817= _temp1815.f2; goto _LL1816; _LL1816: Cyc_Toc_stmt_to_c( _temp1819,
_temp1752); for( 0; _temp1817 != 0; _temp1817= _temp1817->tl){ _temp1752= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*) _temp1817->hd, _temp1752, 0);}( void*)( s->r=( void*)((
void*) _temp1752->r)); goto _LL1790;} _LL1790:;} return; _LL1689:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1821=( char*)"cut"; struct _tagged_string _temp1822; _temp1822.curr=
_temp1821; _temp1822.base= _temp1821; _temp1822.last_plus_one= _temp1821 + 4;
_temp1822;})); return; _LL1691:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1823=( char*)"splice"; struct _tagged_string
_temp1824; _temp1824.curr= _temp1823; _temp1824.base= _temp1823; _temp1824.last_plus_one=
_temp1823 + 7; _temp1824;})); return; _LL1693: s= _temp1760; continue; _LL1695:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1770); Cyc_Toc_exp_to_c( nv,
_temp1768); return; _LL1697: { struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp1825=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1825->f1= Cyc_Toc_abs_ns; _temp1825->f2= Cyc_Toc__push_handler_sp;
_temp1825;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1826=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1826->f1= Cyc_Toc_abs_ns; _temp1826->f2= Cyc_Toc_setjmp_sp; _temp1826;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1*
_temp1827=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1827->f1=
Cyc_Toc_abs_ns; _temp1827->f2= Cyc_Toc__pop_handler_sp; _temp1827;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp1828=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1828->v=( void*) e_typ; _temp1828;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1774);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp1774, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp1829=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1829->v=( void*)
Cyc_Absyn_exn_typ; _temp1829;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp1830=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp1830->r=( void*)({ struct Cyc_Absyn_Var_p_struct* _temp1832=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp1832->tag= Cyc_Absyn_Var_p_tag;
_temp1832->f1= x_vd;( void*) _temp1832;}); _temp1830->topt=({ struct Cyc_Core_Opt*
_temp1831=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1831->v=( void*) Cyc_Absyn_exn_typ; _temp1831;}); _temp1830->loc= 0;
_temp1830;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp1833=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1833->v=( void*) Cyc_Absyn_VoidType; _temp1833;});{ struct
Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp1834=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp1834->pattern= p;
_temp1834->pat_vars=({ struct Cyc_Core_Opt* _temp1835=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1835->v=( void*)({ struct Cyc_List_List*
_temp1836=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1836->hd=( void*) x_vd; _temp1836->tl= 0; _temp1836;}); _temp1835;});
_temp1834->where_clause= 0; _temp1834->body= rs; _temp1834->loc= 0; _temp1834;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1772,({
struct Cyc_List_List* _temp1837=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1837->hd=( void*) default_case; _temp1837->tl= 0;
_temp1837;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp1838=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1838->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1838->tl= 0; _temp1838;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp1839=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1839->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp1839->tl= 0;
_temp1839;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp( Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL1661:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args= args->tl){(*(( struct _tuple18*) args->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple18*) args->hd)).f3);{ struct _tuple1* x=({ struct _tuple1*
_temp1840=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1840->f1=
Cyc_Absyn_Loc_n; _temp1840->f2=(*(( struct _tuple18*) args->hd)).f1; _temp1840;});
nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp1841= s; _LL1843: if( _temp1841 == Cyc_Absyn_Abstract){ goto _LL1844;}
else{ goto _LL1845;} _LL1845: if( _temp1841 == Cyc_Absyn_ExternC){ goto _LL1846;}
else{ goto _LL1847;} _LL1847: goto _LL1848; _LL1844: return Cyc_Absyn_Public;
_LL1846: return Cyc_Absyn_Extern; _LL1848: return s; _LL1842:;} struct Cyc_Core_Opt*
Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c( struct Cyc_Absyn_Structdecl*
s){ struct _tuple1* n=( struct _tuple1*)( s->name)->v; if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1849=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1849->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp1849;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(*
n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_structs_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(* n).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*) dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)(
Cyc_Toc_structs_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(* n).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc == Cyc_Absyn_Extern?
1:( void*) s->sc == Cyc_Absyn_Static)? 1:( void*) s->sc == Cyc_Absyn_ExternC)?
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp1850=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1850->v=( void*) n; _temp1850;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)( s->fields)->v; for( 0; fields != 0; fields= fields->tl){( void*)(((
struct Cyc_Absyn_Structfield*) fields->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) fields->hd)->type));}}}}} struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far= 0; static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl*
u){ struct _tuple1* n=( struct _tuple1*)( u->name)->v; if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp1851=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1851->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp1851;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*) Cyc_Toc_unions_so_far->v,(*
n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_unions_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_unions_so_far->v,(* n).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*) dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)(
Cyc_Toc_unions_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_unions_so_far->v,(* n).f2, u)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)(((( void*) u->sc == Cyc_Absyn_Extern?
1:( void*) u->sc == Cyc_Absyn_Static)? 1:( void*) u->sc == Cyc_Absyn_ExternC)?
Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp1852=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1852->v=( void*) n; _temp1852;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)( u->fields)->v; for( 0; fields != 0; fields= fields->tl){( void*)(((
struct Cyc_Absyn_Structfield*) fields->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) fields->hd)->type));}}}}} static struct
Cyc_Core_Opt* Cyc_Toc_enums_so_far= 0; static struct Cyc_List_List* Cyc_Toc_enumdecl_to_c(
struct Cyc_Absyn_Enumdecl* ed, struct Cyc_List_List* res){ struct _tuple1* n=(
struct _tuple1*)( ed->name)->v; if( Cyc_Toc_enums_so_far == 0){ Cyc_Toc_enums_so_far=({
struct Cyc_Core_Opt* _temp1853=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1853->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1853;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*) Cyc_Toc_enums_so_far->v,(* n).f2);
if( dopt == 0){ seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)(
Cyc_Toc_enums_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_enums_so_far->v,(* n).f2, ed));} else{
seen_before= 1; if((( struct Cyc_Absyn_Enumdecl*) dopt->v)->fields == 0){
seen_defn_before= 0;(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(( struct
Cyc_Dict_Dict*) Cyc_Toc_enums_so_far->v,(* n).f2, ed);} else{ seen_defn_before=
1;}} if( ! seen_before){ struct Cyc_Absyn_Typedefdecl* tdef=({ struct Cyc_Absyn_Typedefdecl*
_temp1854=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1854->name= n; _temp1854->tvs= 0; _temp1854->defn=( void*) Cyc_Absyn_void_star_typ();
_temp1854;}); res=({ struct Cyc_List_List* _temp1855=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1855->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1856=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1856->tag= Cyc_Absyn_Typedef_d_tag;
_temp1856->f1= tdef;( void*) _temp1856;}), 0); _temp1855->tl= res; _temp1855;});}{
void* t=({ struct Cyc_Absyn_TypedefType_struct* _temp1857=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1857->tag= Cyc_Absyn_TypedefType_tag;
_temp1857->f1= n; _temp1857->f2= 0; _temp1857->f3=({ struct Cyc_Core_Opt*
_temp1858=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1858->v=( void*) Cyc_Absyn_void_star_typ(); _temp1858;});( void*) _temp1857;});
int tag_count= 0; int box_count= 0;{ struct Cyc_List_List* fields= ed->fields ==
0? 0:( struct Cyc_List_List*)( ed->fields)->v; for( 0; fields != 0; fields=
fields->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fields->hd;
if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1859=( char*)"can't deal with enum tags yet";
struct _tagged_string _temp1860; _temp1860.curr= _temp1859; _temp1860.base=
_temp1859; _temp1860.last_plus_one= _temp1859 + 30; _temp1860;}));} if( f->typs
== 0){ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc == Cyc_Absyn_Extern? 0:(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Absyn_signed_int_exp((
tag_count ++), 0), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( f->name,
t, eopt);( void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({
struct Cyc_Absyn_Tqual* _temp1861=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1861->q_const= 1; _temp1861->q_volatile=
0; _temp1861->q_restrict= 0; _temp1861;}); res=({ struct Cyc_List_List*
_temp1862=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1862->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1863=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1863->tag= Cyc_Absyn_Var_d_tag; _temp1863->f1= vd;( void*) _temp1863;}), 0);
_temp1862->tl= res; _temp1862;});} else{ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc
== Cyc_Absyn_Extern? 0:( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1864=( char*)"_tag"; struct
_tagged_string _temp1865; _temp1865.curr= _temp1864; _temp1865.base= _temp1864;
_temp1865.last_plus_one= _temp1864 + 5; _temp1865;})), Cyc_Absyn_sint_t, eopt);(
void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({ struct
Cyc_Absyn_Tqual* _temp1866=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Tqual)); _temp1866->q_const= 1; _temp1866->q_volatile= 0;
_temp1866->q_restrict= 0; _temp1866;}); res=({ struct Cyc_List_List* _temp1867=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1867->hd=(
void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct* _temp1868=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1868->tag= Cyc_Absyn_Var_d_tag; _temp1868->f1= vd;( void*) _temp1868;}), 0);
_temp1867->tl= res; _temp1867;}); if( ! seen_defn_before){ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl,
i ++){ struct _tagged_string* fname=( struct _tagged_string*)({ struct
_tagged_string* _temp1869=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1869[ 0]= xprintf("f%d", i); _temp1869;}); struct Cyc_Absyn_Structfield*
f=({ struct Cyc_Absyn_Structfield* _temp1870=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1870->name= fname;
_temp1870->tq=(*(( struct _tuple5*) ts->hd)).f1; _temp1870->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple5*) ts->hd)).f2); _temp1870->attributes= 0; _temp1870;}); fs=({
struct Cyc_List_List* _temp1871=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1871->hd=( void*) f; _temp1871->tl= fs; _temp1871;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);
fs=({ struct Cyc_List_List* _temp1872=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1872->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1873=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1873->name= Cyc_Toc_tag_sp; _temp1873->tq= Cyc_Toc_mt_tq; _temp1873->type=(
void*) Cyc_Absyn_sint_t; _temp1873->attributes= 0; _temp1873;}); _temp1872->tl=
fs; _temp1872;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp1874=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp1874->sc=( void*) Cyc_Absyn_Public; _temp1874->name=({ struct Cyc_Core_Opt*
_temp1876=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1876->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp1877=( char*)"_struct"; struct _tagged_string _temp1878; _temp1878.curr=
_temp1877; _temp1878.base= _temp1877; _temp1878.last_plus_one= _temp1877 + 8;
_temp1878;})); _temp1876;}); _temp1874->tvs= 0; _temp1874->fields=({ struct Cyc_Core_Opt*
_temp1875=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1875->v=( void*) fs; _temp1875;}); _temp1874->attributes= 0; _temp1874;});
res=({ struct Cyc_List_List* _temp1879=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1879->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp1880=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1880->tag= Cyc_Absyn_Struct_d_tag;
_temp1880->f1= sd;( void*) _temp1880;}), 0); _temp1879->tl= res; _temp1879;});}}}}}
return res;}}} static struct Cyc_Core_Opt* Cyc_Toc_xenums_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xenumdecl_to_c( struct Cyc_Absyn_Xenumdecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp1881=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1881->name= n; _temp1881->tvs= 0; _temp1881->defn=( void*) t; _temp1881;});
res=({ struct Cyc_List_List* _temp1882=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1882->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1883=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1883->tag= Cyc_Absyn_Typedef_d_tag;
_temp1883->f1= td;( void*) _temp1883;}), 0); _temp1882->tl= res; _temp1882;});}
else{ if( Cyc_Toc_xenums_so_far == 0){ Cyc_Toc_xenums_so_far=({ struct Cyc_Core_Opt*
_temp1884=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1884->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1884;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->tag
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1885=( char*)"xenum:  can't deal with explicit tags";
struct _tagged_string _temp1886; _temp1886.curr= _temp1885; _temp1886.base=
_temp1885; _temp1886.last_plus_one= _temp1885 + 38; _temp1886;}));}{ struct
_tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp1887=* fn;( unsigned int)( _temp1887.last_plus_one -
_temp1887.curr);}), 0); void* tag_typ=({ struct Cyc_Absyn_ArrayType_struct*
_temp1888=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1888->tag= Cyc_Absyn_ArrayType_tag; _temp1888->f1=( void*) Cyc_Absyn_uchar_t;
_temp1888->f2= Cyc_Toc_mt_tq; _temp1888->f3= sz_exp;( void*) _temp1888;});
struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc != Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp( 0,* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp1889=( char*)"_tag"; struct _tagged_string
_temp1890; _temp1890.curr= _temp1889; _temp1890.base= _temp1889; _temp1890.last_plus_one=
_temp1889 + 5; _temp1890;})), tag_typ, initopt);( void*)( tag_decl->sc=( void*)((
void*) xd->sc)); res=({ struct Cyc_List_List* _temp1891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1891->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1892=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1892->tag= Cyc_Absyn_Var_d_tag;
_temp1892->f1= tag_decl;( void*) _temp1892;}), 0); _temp1891->tl= res; _temp1891;});{
int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*) Cyc_Toc_xenums_so_far->v, fn); if( dopt == 0){
seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_xenums_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Xenumdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_xenums_so_far->v, fn, xd));} else{ seen_before= 1;
if((( struct Cyc_Absyn_Xenumdecl*) dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)(
Cyc_Toc_xenums_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Xenumdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_xenums_so_far->v, fn, xd)); seen_defn_before= 0;}
else{ seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0;
int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl, i ++){
struct _tagged_string* field_n=( struct _tagged_string*)({ struct _tagged_string*
_temp1893=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1893[ 0]= xprintf("f%d", i); _temp1893;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp1894=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1894->name= field_n;
_temp1894->tq=(*(( struct _tuple5*) ts->hd)).f1; _temp1894->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple5*) ts->hd)).f2); _temp1894->attributes= 0; _temp1894;}); fields=({
struct Cyc_List_List* _temp1895=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1895->hd=( void*) newf; _temp1895->tl= fields;
_temp1895;});}} fields=({ struct Cyc_List_List* _temp1896=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1896->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1897=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1897->name= Cyc_Toc_tag_sp; _temp1897->tq= Cyc_Toc_mt_tq; _temp1897->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp1897->attributes=
0; _temp1897;}); _temp1896->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields); _temp1896;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp1898=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1898->sc=( void*) Cyc_Absyn_Public;
_temp1898->name=({ struct Cyc_Core_Opt* _temp1900=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1900->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1901=( char*)"_struct"; struct
_tagged_string _temp1902; _temp1902.curr= _temp1901; _temp1902.base= _temp1901;
_temp1902.last_plus_one= _temp1901 + 8; _temp1902;})); _temp1900;}); _temp1898->tvs=
0; _temp1898->fields=({ struct Cyc_Core_Opt* _temp1899=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1899->v=( void*) fields;
_temp1899;}); _temp1898->attributes= 0; _temp1898;}); res=({ struct Cyc_List_List*
_temp1903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1903->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp1904=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1904->tag= Cyc_Absyn_Struct_d_tag; _temp1904->f1= strct_decl;( void*)
_temp1904;}), 0); _temp1903->tl= res; _temp1903;});}}}}}}}} return res;} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s){ void* t=( void*) topt->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp1907; struct Cyc_List_List* _temp1909; struct Cyc_Toc_Env* _temp1911;
struct _tuple13 _temp1905= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL1912: _temp1911=
_temp1905.f1; goto _LL1910; _LL1910: _temp1909= _temp1905.f2; goto _LL1908;
_LL1908: _temp1907= _temp1905.f3; goto _LL1906; _LL1906: Cyc_Toc_stmt_to_c(
_temp1911, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1907, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1913=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1913->f1= Cyc_Toc_abs_ns; _temp1913->f2= Cyc_Toc_Match_Exception_sp;
_temp1913;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1907, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp1909 != 0; _temp1909= _temp1909->tl){
struct _tuple14 _temp1916; void* _temp1917; struct _tuple1* _temp1919; struct
_tuple14* _temp1914=( struct _tuple14*) _temp1909->hd; _temp1916=* _temp1914;
_LL1920: _temp1919= _temp1916.f1; goto _LL1918; _LL1918: _temp1917= _temp1916.f2;
goto _LL1915; _LL1915: s= Cyc_Absyn_declare_stmt( _temp1919, _temp1917, 0, s, 0);}
return s;}}} static struct _tuple17 Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top){ struct Cyc_List_List* res= 0; for( 0; ds !=
0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) ds->hd; void*
_temp1921=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp1945; struct Cyc_Absyn_Fndecl*
_temp1947; int _temp1949; struct Cyc_Absyn_Exp* _temp1951; struct Cyc_Core_Opt*
_temp1953; struct Cyc_Core_Opt* _temp1955; struct Cyc_Absyn_Pat* _temp1957;
struct Cyc_Absyn_Structdecl* _temp1959; struct Cyc_Absyn_Uniondecl* _temp1961;
struct Cyc_Absyn_Enumdecl* _temp1963; struct Cyc_Absyn_Xenumdecl* _temp1965;
struct Cyc_Absyn_Typedefdecl* _temp1967; struct Cyc_List_List* _temp1969; struct
_tagged_string* _temp1971; struct Cyc_List_List* _temp1973; struct _tuple1*
_temp1975; struct Cyc_List_List* _temp1977; _LL1923: if((( struct _enum_struct*)
_temp1921)->tag == Cyc_Absyn_Var_d_tag){ _LL1946: _temp1945=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1921)->f1; goto _LL1924;} else{ goto
_LL1925;} _LL1925: if((( struct _enum_struct*) _temp1921)->tag == Cyc_Absyn_Fn_d_tag){
_LL1948: _temp1947=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1921)->f1; goto _LL1926;} else{ goto _LL1927;} _LL1927: if((( struct
_enum_struct*) _temp1921)->tag == Cyc_Absyn_Let_d_tag){ _LL1958: _temp1957=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1921)->f1; goto
_LL1956; _LL1956: _temp1955=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1921)->f2; goto _LL1954; _LL1954: _temp1953=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1921)->f3; goto _LL1952; _LL1952: _temp1951=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1921)->f4; goto
_LL1950; _LL1950: _temp1949=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1921)->f5;
goto _LL1928;} else{ goto _LL1929;} _LL1929: if((( struct _enum_struct*)
_temp1921)->tag == Cyc_Absyn_Struct_d_tag){ _LL1960: _temp1959=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1921)->f1; goto _LL1930;} else{ goto
_LL1931;} _LL1931: if((( struct _enum_struct*) _temp1921)->tag == Cyc_Absyn_Union_d_tag){
_LL1962: _temp1961=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1921)->f1; goto _LL1932;} else{ goto _LL1933;} _LL1933: if((( struct
_enum_struct*) _temp1921)->tag == Cyc_Absyn_Enum_d_tag){ _LL1964: _temp1963=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1921)->f1;
goto _LL1934;} else{ goto _LL1935;} _LL1935: if((( struct _enum_struct*)
_temp1921)->tag == Cyc_Absyn_Xenum_d_tag){ _LL1966: _temp1965=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1921)->f1; goto _LL1936;} else{ goto
_LL1937;} _LL1937: if((( struct _enum_struct*) _temp1921)->tag == Cyc_Absyn_Typedef_d_tag){
_LL1968: _temp1967=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1921)->f1; goto _LL1938;} else{ goto _LL1939;} _LL1939: if((( struct
_enum_struct*) _temp1921)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1972: _temp1971=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1921)->f1;
goto _LL1970; _LL1970: _temp1969=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1921)->f2; goto _LL1940;} else{ goto _LL1941;} _LL1941: if((( struct
_enum_struct*) _temp1921)->tag == Cyc_Absyn_Using_d_tag){ _LL1976: _temp1975=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp1921)->f1; goto
_LL1974; _LL1974: _temp1973=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1921)->f2; goto _LL1942;} else{ goto _LL1943;} _LL1943: if((( struct
_enum_struct*) _temp1921)->tag == Cyc_Absyn_ExternC_d_tag){ _LL1978: _temp1977=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp1921)->f1;
goto _LL1944;} else{ goto _LL1922;} _LL1924: { struct _tuple1* c_name= _temp1945->name;
if(( void*) _temp1945->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1*
_temp1979=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1979->f1=
Cyc_Toc_abs_ns; _temp1979->f2=(* c_name).f2; _temp1979;});} if( _temp1945->initializer
!= 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1980= _temp1945->initializer; if( _temp1980 == 0){ _throw( Null_Exception);}
_temp1980;}));} if( nv->toplevel){ res=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls, res); Cyc_Toc_temp_topdecls=
0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp1945->name, Cyc_Absyn_varb_exp(
c_name,({ struct Cyc_Absyn_Global_b_struct* _temp1981=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp1981->tag= Cyc_Absyn_Global_b_tag;
_temp1981->f1= _temp1945;( void*) _temp1981;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp1945->name, Cyc_Absyn_varb_exp( c_name,({ struct Cyc_Absyn_Local_b_struct*
_temp1982=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1982->tag= Cyc_Absyn_Local_b_tag; _temp1982->f1= _temp1945;( void*)
_temp1982;}), 0));} _temp1945->name= c_name;( void*)( _temp1945->sc=( void*) Cyc_Toc_scope_to_c((
void*) _temp1945->sc));( void*)( _temp1945->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1945->type)); res=({ struct Cyc_List_List* _temp1983=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1983->hd=( void*) d; _temp1983->tl=
res; _temp1983;}); goto _LL1922;} _LL1926: { struct _tuple1* c_name= _temp1947->name;
if(( void*) _temp1947->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1*
_temp1984=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1984->f1=
Cyc_Toc_abs_ns; _temp1984->f2=(* c_name).f2; _temp1984;});} nv= Cyc_Toc_add_varmap(
nv, _temp1947->name, Cyc_Absyn_var_exp( c_name, 0)); _temp1947->name= c_name;
Cyc_Toc_fndecl_to_c( nv, _temp1947); res=({ struct Cyc_List_List* _temp1985=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1985->hd=(
void*) d; _temp1985->tl= res; _temp1985;}); goto _LL1922;} _LL1928:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1986=( char*)"letdecl"; struct _tagged_string _temp1987; _temp1987.curr=
_temp1986; _temp1987.base= _temp1986; _temp1987.last_plus_one= _temp1986 + 8;
_temp1987;})); goto _LL1922; _LL1930: Cyc_Toc_structdecl_to_c( _temp1959); res=({
struct Cyc_List_List* _temp1988=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1988->hd=( void*) d; _temp1988->tl= res; _temp1988;});
goto _LL1922; _LL1932: Cyc_Toc_uniondecl_to_c( _temp1961); res=({ struct Cyc_List_List*
_temp1989=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1989->hd=( void*) d; _temp1989->tl= res; _temp1989;}); goto _LL1922;
_LL1934: res= Cyc_Toc_enumdecl_to_c( _temp1963, res); goto _LL1922; _LL1936: res=
Cyc_Toc_xenumdecl_to_c( _temp1965, res); goto _LL1922; _LL1938: _temp1967->name=
_temp1967->name; _temp1967->tvs= 0;( void*)( _temp1967->defn=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1967->defn)); res=({ struct Cyc_List_List* _temp1990=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1990->hd=( void*) d; _temp1990->tl=
res; _temp1990;}); goto _LL1922; _LL1940: _temp1973= _temp1969; goto _LL1942;
_LL1942: _temp1977= _temp1973; goto _LL1944; _LL1944: { struct _tuple17 p= Cyc_Toc_decls_to_c(
nv, _temp1977, top); nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( p.f2), res); goto _LL1922;} _LL1922:;}
return({ struct _tuple17 _temp1991; _temp1991.f1= nv; _temp1991.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp1991;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_enums_so_far= 0; Cyc_Toc_xenums_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List* _temp1994;
struct Cyc_Toc_Env* _temp1996; struct _tuple17 _temp1992= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL1997: _temp1996= _temp1992.f1; goto _LL1995;
_LL1995: _temp1994= _temp1992.f2; goto _LL1993; _LL1993: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp1994);}}