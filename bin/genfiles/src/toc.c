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
; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple1* name); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*,
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
char c, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp( int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment*);
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
struct Cyc_Toc_Toc_Unimplemented_struct));* _temp2=( struct Cyc_Toc_Toc_Unimplemented_struct){.tag=
Cyc_Toc_Toc_Unimplemented_tag};( struct _xenum_struct*) _temp2;}));} static void*
Cyc_Toc_toc_impos( struct _tagged_string s){({ struct _tagged_string _temp3= s;
fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp3.last_plus_one - _temp3.curr, _temp3.curr);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({
struct Cyc_Toc_Toc_Impossible_struct* _temp4=( struct Cyc_Toc_Toc_Impossible_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Toc_Toc_Impossible_struct));* _temp4=(
struct Cyc_Toc_Toc_Impossible_struct){.tag= Cyc_Toc_Toc_Impossible_tag};( struct
_xenum_struct*) _temp4;}));} char Cyc_Toc_Match_error_tag[ 12u]="Match_error";
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
_temp66; void* _temp68; void* _temp70; _LL56: if(( unsigned int) _temp54 > 5u?((
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
_temp123; _LL117: if(( unsigned int) _temp115 > 5u?(( struct _enum_struct*)
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
static struct Cyc_Absyn_TypedefType_struct _temp142={ 11u,& Cyc_Toc_longlong_v,
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
_temp180; _LL161: if(( unsigned int) _temp159 > 5u?(( struct _enum_struct*)
_temp159)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL172: _temp171=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp159)->f1; goto _LL170; _LL170: _temp169=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp159)->f2;
goto _LL168; _LL168: _temp167=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp159)->f3; goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int)
_temp159 > 5u?(( struct _enum_struct*) _temp159)->tag == Cyc_Absyn_Evar_tag: 0){
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
struct Cyc_List_List* _temp286; struct Cyc_List_List* _temp288; struct Cyc_Absyn_Structdecl**
_temp290; struct Cyc_List_List* _temp292; struct _tuple1* _temp294; struct Cyc_Core_Opt*
_temp296; struct Cyc_List_List* _temp298; struct _tuple1* _temp300; void*
_temp302; void* _temp304; struct Cyc_List_List* _temp306; _LL185: if( _temp183
== Cyc_Absyn_VoidType){ goto _LL186;} else{ goto _LL187;} _LL187: if((
unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_Evar_tag:
0){ _LL230: _temp229=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f1;
goto _LL228; _LL228: _temp227=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp183)->f2; if( _temp227 == 0){ goto _LL226;} else{ goto _LL189;} _LL226:
_temp225=( int)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f3; goto _LL188;}
else{ goto _LL189;} _LL189: if(( unsigned int) _temp183 > 5u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_Evar_tag: 0){ _LL239: _temp238=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp183)->f1; goto _LL234; _LL234: _temp233=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp183)->f2; if(
_temp233 == 0){ goto _LL191;} else{ _temp235=* _temp233; _LL237: _temp236=( void*)
_temp235.v; goto _LL232;} _LL232: _temp231=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp183)->f3; goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_VarType_tag:
0){ _LL241: _temp240=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp183)->f1; goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL247: _temp246=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp183)->f1; goto _LL245; _LL245: _temp244=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp183)->f2; goto _LL243; _LL243: _temp242=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp183)->f3;
goto _LL194;} else{ goto _LL195;} _LL195: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL251:
_temp250=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*) _temp183)->f1;
goto _LL249; _LL249: _temp248=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp183)->f2; goto _LL196;} else{ goto _LL197;} _LL197: if(( unsigned int)
_temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL253: _temp252=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp183)->f1; _LL263: _temp262=( void*) _temp252.elt_typ; goto _LL261; _LL261:
_temp260=( void*) _temp252.rgn_typ; goto _LL259; _LL259: _temp258=( struct Cyc_Absyn_Conref*)
_temp252.nullable; goto _LL257; _LL257: _temp256=( struct Cyc_Absyn_Tqual*)
_temp252.tq; goto _LL255; _LL255: _temp254=( struct Cyc_Absyn_Conref*) _temp252.bounds;
goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_IntType_tag: 0){ _LL267:
_temp266=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f1; goto _LL265;
_LL265: _temp264=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f2;
goto _LL200;} else{ goto _LL201;} _LL201: if( _temp183 == Cyc_Absyn_FloatType){
goto _LL202;} else{ goto _LL203;} _LL203: if( _temp183 == Cyc_Absyn_DoubleType){
goto _LL204;} else{ goto _LL205;} _LL205: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL273:
_temp272=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp183)->f1; goto
_LL271; _LL271: _temp270=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp183)->f2; goto _LL269; _LL269: _temp268=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp183)->f3; goto _LL206;} else{ goto _LL207;}
_LL207: if(( unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL275: _temp274=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp183)->f1; _LL287: _temp286=( struct Cyc_List_List*)
_temp274.tvars; goto _LL285; _LL285: _temp284=( struct Cyc_Core_Opt*) _temp274.effect;
goto _LL283; _LL283: _temp282=( void*) _temp274.ret_typ; goto _LL281; _LL281:
_temp280=( struct Cyc_List_List*) _temp274.args; goto _LL279; _LL279: _temp278=(
int) _temp274.varargs; goto _LL277; _LL277: _temp276=( struct Cyc_List_List*)
_temp274.attributes; goto _LL208;} else{ goto _LL209;} _LL209: if(( unsigned int)
_temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL289: _temp288=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp183)->f1; goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp183 > 5u?(( struct _enum_struct*) _temp183)->tag == Cyc_Absyn_StructType_tag:
0){ _LL295: _temp294=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp183)->f1; goto _LL293; _LL293: _temp292=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp183)->f2; goto _LL291; _LL291: _temp290=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp183)->f3;
goto _LL212;} else{ goto _LL213;} _LL213: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL301:
_temp300=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp183)->f1;
goto _LL299; _LL299: _temp298=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp183)->f2; goto _LL297; _LL297: _temp296=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp183)->f3; goto _LL214;} else{ goto _LL215;}
_LL215: if(( unsigned int) _temp183 > 5u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL303: _temp302=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp183)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if( _temp183 == Cyc_Absyn_HeapRgn){
goto _LL218;} else{ goto _LL219;} _LL219: if( _temp183 == Cyc_Absyn_UnionType){
goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp183 > 5u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL305:
_temp304=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp183)->f1; goto
_LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp183 > 5u?(( struct
_enum_struct*) _temp183)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL307: _temp306=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp183)->f1; goto
_LL224;} else{ goto _LL184;} _LL186: return t; _LL188: return Cyc_Absyn_VoidType;
_LL190: return Cyc_Toc_typ_to_c( _temp236); _LL192: return Cyc_Absyn_void_star_typ();
_LL194: if( _temp246 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp308=( char*)"anonymous enum"; struct
_tagged_string _temp309; _temp309.curr= _temp308; _temp309.base= _temp308;
_temp309.last_plus_one= _temp308 + 15; _temp309;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp310=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp310->tag= Cyc_Absyn_TypedefType_tag; _temp310->f1=( struct _tuple1*)({
struct _tuple1* _temp311= _temp246; if( _temp311 == 0){ _throw( Null_Exception);}
_temp311;}); _temp310->f2= 0; _temp310->f3=({ struct Cyc_Core_Opt* _temp312=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp312->v=(
void*) Cyc_Absyn_void_star_typ(); _temp312;});( void*) _temp310;}); _LL196:
return({ struct Cyc_Absyn_TypedefType_struct* _temp313=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp313->tag= Cyc_Absyn_TypedefType_tag;
_temp313->f1= _temp250; _temp313->f2= 0; _temp313->f3=({ struct Cyc_Core_Opt*
_temp314=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp314->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp),
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp314;});( void*) _temp313;}); _LL198:
_temp262= Cyc_Toc_typ_to_c_array( _temp262);{ void* _temp315=( void*) _temp254->v;
void* _temp321; _LL317: if(( unsigned int) _temp315 > 1u?(( struct _enum_struct*)
_temp315)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL322: _temp321=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp315)->f1; if( _temp321 == Cyc_Absyn_Unknown_b){
goto _LL318;} else{ goto _LL319;}} else{ goto _LL319;} _LL319: goto _LL320;
_LL318: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp262,
_temp256); return Cyc_Absyn_strct( n);} _LL320: return Cyc_Absyn_star_typ(
_temp262, Cyc_Absyn_HeapRgn, _temp256); _LL316:;} _LL200: return t; _LL202:
return t; _LL204: return t; _LL206: return({ struct Cyc_Absyn_ArrayType_struct*
_temp323=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp323->tag= Cyc_Absyn_ArrayType_tag; _temp323->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp272); _temp323->f2= _temp270; _temp323->f3= _temp268;( void*) _temp323;});
_LL208: return({ struct Cyc_Absyn_FnType_struct* _temp324=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp324->tag= Cyc_Absyn_FnType_tag;
_temp324->f1=({ struct Cyc_Absyn_FnInfo _temp325; _temp325.tvars= 0; _temp325.effect=
0; _temp325.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp282); _temp325.args=((
struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp280); _temp325.varargs= _temp278;
_temp325.attributes= _temp276; _temp325;});( void*) _temp324;}); _LL210:
_temp288=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp288);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp288); return Cyc_Absyn_strct( n);}
_LL212: if( _temp294 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp326=( char*)"anonymous struct"; struct
_tagged_string _temp327; _temp327.curr= _temp326; _temp327.base= _temp326;
_temp327.last_plus_one= _temp326 + 17; _temp327;}));} return Cyc_Absyn_strctq((
struct _tuple1*)({ struct _tuple1* _temp328= _temp294; if( _temp328 == 0){
_throw( Null_Exception);} _temp328;})); _LL214: if( _temp296 == 0){ return((
void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp329=( char*)"Toc::typ_to_c: unresolved TypedefType"; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 38; _temp330;}));} return({ struct Cyc_Absyn_TypedefType_struct*
_temp331=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp331->tag= Cyc_Absyn_TypedefType_tag; _temp331->f1= _temp300; _temp331->f2=
0; _temp331->f3=({ struct Cyc_Core_Opt* _temp332=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp332->v=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp296->v); _temp332;});( void*) _temp331;}); _LL216: return Cyc_Absyn_void_star_typ();
_LL218: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp333=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 55; _temp334;})); _LL220: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp335=( char*)"uniontype"; struct _tagged_string _temp336; _temp336.curr=
_temp335; _temp336.base= _temp335; _temp336.last_plus_one= _temp335 + 10;
_temp336;})); _LL222: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp337=( char*)"Toc::typ_to_c: type translation passed the access eff";
struct _tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 54; _temp338;})); _LL224: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp339=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 52; _temp340;})); _LL184:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp341= t; struct Cyc_Absyn_Exp* _temp347; struct Cyc_Absyn_Tqual*
_temp349; void* _temp351; _LL343: if(( unsigned int) _temp341 > 5u?(( struct
_enum_struct*) _temp341)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL352: _temp351=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp341)->f1; goto _LL350; _LL350:
_temp349=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp341)->f2; goto _LL348; _LL348: _temp347=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp341)->f3; goto _LL344;} else{ goto _LL345;}
_LL345: goto _LL346; _LL344: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp351, Cyc_Absyn_HeapRgn, _temp349), e, l); _LL346: return e; _LL342:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp353= t; struct Cyc_Absyn_Exp*
_temp359; struct Cyc_Absyn_Tqual* _temp361; void* _temp363; _LL355: if((
unsigned int) _temp353 > 5u?(( struct _enum_struct*) _temp353)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL364: _temp363=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp353)->f1;
goto _LL362; _LL362: _temp361=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp353)->f2; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp353)->f3; goto _LL356;} else{ goto _LL357;}
_LL357: goto _LL358; _LL356: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp363, Cyc_Absyn_HeapRgn, _temp361), e, l); _LL358: return Cyc_Absyn_cast_exp(
t, e, l); _LL354:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp365= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp397; void* _temp399; void* _temp401; struct Cyc_Absyn_Exp*
_temp403; struct Cyc_Absyn_Tqual* _temp405; void* _temp407; struct Cyc_Absyn_FnInfo
_temp409; struct Cyc_List_List* _temp411; struct Cyc_Absyn_Structdecl** _temp413;
struct Cyc_List_List* _temp415; struct _tuple1* _temp417; struct Cyc_Absyn_Enumdecl**
_temp419; struct Cyc_List_List* _temp421; struct _tuple1* _temp423; struct Cyc_Absyn_Xenumdecl**
_temp425; struct _tuple1* _temp427; struct Cyc_Absyn_PtrInfo _temp429; void*
_temp431; _LL367: if( _temp365 == Cyc_Absyn_VoidType){ goto _LL368;} else{ goto
_LL369;} _LL369: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*)
_temp365)->tag == Cyc_Absyn_VarType_tag: 0){ _LL398: _temp397=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp365)->f1; goto _LL370;} else{ goto _LL371;}
_LL371: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*) _temp365)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp365)->f1; goto _LL400; _LL400: _temp399=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp365)->f2; goto _LL372;} else{ goto _LL373;} _LL373: if( _temp365 == Cyc_Absyn_FloatType){
goto _LL374;} else{ goto _LL375;} _LL375: if( _temp365 == Cyc_Absyn_DoubleType){
goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp365 > 5u?((
struct _enum_struct*) _temp365)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL408:
_temp407=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp365)->f1; goto
_LL406; _LL406: _temp405=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp365)->f2; goto _LL404; _LL404: _temp403=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp365)->f3; goto _LL378;} else{ goto _LL379;}
_LL379: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*) _temp365)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL410: _temp409=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp365)->f1; goto _LL380;} else{ goto _LL381;}
_LL381: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*) _temp365)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL412: _temp411=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp365)->f1; goto _LL382;} else{ goto
_LL383;} _LL383: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*)
_temp365)->tag == Cyc_Absyn_StructType_tag: 0){ _LL418: _temp417=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp365)->f1; goto _LL416;
_LL416: _temp415=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp365)->f2; goto _LL414; _LL414: _temp413=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp365)->f3; goto _LL384;} else{ goto
_LL385;} _LL385: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*)
_temp365)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL424: _temp423=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp365)->f1; goto _LL422; _LL422: _temp421=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp365)->f2; goto
_LL420; _LL420: _temp419=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp365)->f3; goto _LL386;} else{ goto _LL387;} _LL387: if(( unsigned int)
_temp365 > 5u?(( struct _enum_struct*) _temp365)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL428: _temp427=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp365)->f1; goto _LL426; _LL426: _temp425=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp365)->f2; goto _LL388;} else{ goto
_LL389;} _LL389: if( _temp365 == Cyc_Absyn_UnionType){ goto _LL390;} else{ goto
_LL391;} _LL391: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*)
_temp365)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL430: _temp429=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp365)->f1; goto _LL392;} else{ goto
_LL393;} _LL393: if(( unsigned int) _temp365 > 5u?(( struct _enum_struct*)
_temp365)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL432: _temp431=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp365)->f1; goto _LL394;} else{ goto
_LL395;} _LL395: goto _LL396; _LL368: return 1; _LL370: return 0; _LL372: return
1; _LL374: return 1; _LL376: return 0; _LL378: return Cyc_Toc_atomic_typ(
_temp407); _LL380: return 1; _LL382: for( 0; _temp411 != 0; _temp411= _temp411->tl){
if( ! Cyc_Toc_atomic_typ((*(( struct _tuple5*) _temp411->hd)).f2)){ return 0;}}
return 1; _LL384: return 0; _LL386: return 0; _LL388: return 0; _LL390: return 0;
_LL392: return 0; _LL394: return 0; _LL396:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp433= Cyc_Absynpp_typ2string( t);
xprintf("atomic_typ:  bad type %.*s", _temp433.last_plus_one - _temp433.curr,
_temp433.curr);})); return 0; _LL366:;} static int Cyc_Toc_is_void_star( void* t){
void* _temp434= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp440;
struct Cyc_Absyn_Conref* _temp442; struct Cyc_Absyn_Tqual* _temp444; struct Cyc_Absyn_Conref*
_temp446; void* _temp448; void* _temp450; _LL436: if(( unsigned int) _temp434 >
5u?(( struct _enum_struct*) _temp434)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL441: _temp440=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp434)->f1; _LL451: _temp450=( void*) _temp440.elt_typ; goto _LL449; _LL449:
_temp448=( void*) _temp440.rgn_typ; goto _LL447; _LL447: _temp446=( struct Cyc_Absyn_Conref*)
_temp440.nullable; goto _LL445; _LL445: _temp444=( struct Cyc_Absyn_Tqual*)
_temp440.tq; goto _LL443; _LL443: _temp442=( struct Cyc_Absyn_Conref*) _temp440.bounds;
goto _LL437;} else{ goto _LL438;} _LL438: goto _LL439; _LL437: { void* _temp452=
Cyc_Tcutil_compress( _temp450); _LL454: if( _temp452 == Cyc_Absyn_VoidType){
goto _LL455;} else{ goto _LL456;} _LL456: goto _LL457; _LL455: return 1; _LL457:
return 0; _LL453:;} _LL439: return 0; _LL435:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp458= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp466; struct Cyc_Absyn_Structdecl* _temp468;
struct Cyc_List_List* _temp469; struct _tuple1* _temp471; struct Cyc_Absyn_Structdecl**
_temp473; struct Cyc_List_List* _temp475; struct _tuple1* _temp477; _LL460: if((
unsigned int) _temp458 > 5u?(( struct _enum_struct*) _temp458)->tag == Cyc_Absyn_StructType_tag:
0){ _LL472: _temp471=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp458)->f1; goto _LL470; _LL470: _temp469=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp458)->f2; goto _LL467; _LL467: _temp466=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp458)->f3;
if( _temp466 == 0){ goto _LL462;} else{ _temp468=* _temp466; goto _LL461;}}
else{ goto _LL462;} _LL462: if(( unsigned int) _temp458 > 5u?(( struct
_enum_struct*) _temp458)->tag == Cyc_Absyn_StructType_tag: 0){ _LL478: _temp477=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp458)->f1; goto
_LL476; _LL476: _temp475=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp458)->f2; goto _LL474; _LL474: _temp473=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp458)->f3; if( _temp473 == 0){ goto
_LL463;} else{ goto _LL464;}} else{ goto _LL464;} _LL464: goto _LL465; _LL461:
if( _temp471 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp479=( char*)"StructType name not properly set";
struct _tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 33; _temp480;}));} if( _temp468->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp482= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp481=
_temp471; if( _temp481 == 0){ _throw( Null_Exception);} _temp481;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp482.last_plus_one - _temp482.curr, _temp482.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp468->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)
fields->hd)->name) == 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp484= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp483= _temp471; if( _temp483 == 0){ _throw( Null_Exception);}
_temp483;})); struct _tagged_string _temp485=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp484.last_plus_one - _temp484.curr, _temp484.curr, _temp485.last_plus_one -
_temp485.curr, _temp485.curr);})); return 0;} _LL463: if( _temp477 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp486=( char*)"StructType name not properly set"; struct _tagged_string
_temp487; _temp487.curr= _temp486; _temp487.base= _temp486; _temp487.last_plus_one=
_temp486 + 33; _temp487;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp489= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp488= _temp477; if( _temp488 == 0){ _throw( Null_Exception);}
_temp488;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp489.last_plus_one
- _temp489.curr, _temp489.curr);})); return 0; _LL465:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp490= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp490.last_plus_one - _temp490.curr,
_temp490.curr);})); return 0; _LL459:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp491=( void*) e->r; struct _tagged_string*
_temp499; struct Cyc_Absyn_Exp* _temp501; struct _tagged_string* _temp503;
struct Cyc_Absyn_Exp* _temp505; _LL493: if((( struct _enum_struct*) _temp491)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL502: _temp501=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp491)->f1; goto _LL500; _LL500:
_temp499=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp491)->f2; goto _LL494;} else{ goto _LL495;} _LL495: if((( struct
_enum_struct*) _temp491)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL506: _temp505=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp491)->f1;
goto _LL504; _LL504: _temp503=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp491)->f2; goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL494:
return Cyc_Toc_is_poly_field(( void*)( _temp501->topt)->v, _temp499); _LL496: {
void* _temp507= Cyc_Tcutil_compress(( void*)( _temp505->topt)->v); struct Cyc_Absyn_PtrInfo
_temp513; struct Cyc_Absyn_Conref* _temp515; struct Cyc_Absyn_Tqual* _temp517;
struct Cyc_Absyn_Conref* _temp519; void* _temp521; void* _temp523; _LL509: if((
unsigned int) _temp507 > 5u?(( struct _enum_struct*) _temp507)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL514: _temp513=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp507)->f1; _LL524: _temp523=( void*) _temp513.elt_typ; goto _LL522; _LL522:
_temp521=( void*) _temp513.rgn_typ; goto _LL520; _LL520: _temp519=( struct Cyc_Absyn_Conref*)
_temp513.nullable; goto _LL518; _LL518: _temp517=( struct Cyc_Absyn_Tqual*)
_temp513.tq; goto _LL516; _LL516: _temp515=( struct Cyc_Absyn_Conref*) _temp513.bounds;
goto _LL510;} else{ goto _LL511;} _LL511: goto _LL512; _LL510: return Cyc_Toc_is_poly_field(
_temp523, _temp503); _LL512:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp525= Cyc_Absynpp_typ2string(( void*)( _temp505->topt)->v);
xprintf("is_poly_project:  bad type %.*s", _temp525.last_plus_one - _temp525.curr,
_temp525.curr);})); return 0; _LL508:;} _LL498: return 0; _LL492:;} static char
_temp528[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp528, _temp528, _temp528 + 10u}; static struct Cyc_Absyn_Abs_n_struct
_temp529={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp529,.f2=& Cyc_Toc_gc_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var, 0),({
struct Cyc_List_List* _temp530=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp530->hd=( void*) s; _temp530->tl= 0; _temp530;}), 0);}
static char _temp533[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp533, _temp533, _temp533 + 17u}; static struct Cyc_Absyn_Abs_n_struct
_temp534={ 1u, 0}; static struct _tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct
_tuple1){.f1=( struct Cyc_Absyn_Abs_n_struct*)& _temp534,.f2=& Cyc_Toc_gc_malloc_atomic_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var, 0),({
struct Cyc_List_List* _temp535=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp535->hd=( void*) s; _temp535->tl= 0; _temp535;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp537=({ struct _tuple0
_temp536; _temp536.f1= t1; _temp536.f2= t2; _temp536;}); void* _temp543; struct
Cyc_Absyn_PtrInfo _temp545; void* _temp547; struct Cyc_Absyn_PtrInfo _temp549;
_LL539: _LL548: _temp547= _temp537.f1; if(( unsigned int) _temp547 > 5u?((
struct _enum_struct*) _temp547)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL550:
_temp549=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp547)->f1; goto _LL544;} else{ goto _LL541;} _LL544: _temp543= _temp537.f2;
if(( unsigned int) _temp543 > 5u?(( struct _enum_struct*) _temp543)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL546: _temp545=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp543)->f1; goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL540: {
int n1=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp549.nullable);
int n2=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp545.nullable);
void* b1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp549.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp545.bounds); struct Cyc_List_List* convs= 0;{ struct _tuple0 _temp552=({
struct _tuple0 _temp551; _temp551.f1= b1; _temp551.f2= b2; _temp551;}); void*
_temp562; struct Cyc_Absyn_Exp* _temp564; void* _temp566; struct Cyc_Absyn_Exp*
_temp568; void* _temp570; void* _temp572; void* _temp574; void* _temp576; struct
Cyc_Absyn_Exp* _temp578; void* _temp580; struct Cyc_Absyn_Exp* _temp582; void*
_temp584; _LL554: _LL567: _temp566= _temp552.f1; if(( unsigned int) _temp566 > 1u?((
struct _enum_struct*) _temp566)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL569:
_temp568=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp566)->f1;
goto _LL563;} else{ goto _LL556;} _LL563: _temp562= _temp552.f2; if((
unsigned int) _temp562 > 1u?(( struct _enum_struct*) _temp562)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL565: _temp564=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp562)->f1; goto _LL555;} else{ goto _LL556;} _LL556: _LL573: _temp572=
_temp552.f1; if( _temp572 == Cyc_Absyn_Unknown_b){ goto _LL571;} else{ goto
_LL558;} _LL571: _temp570= _temp552.f2; if( _temp570 == Cyc_Absyn_Unknown_b){
goto _LL557;} else{ goto _LL558;} _LL558: _LL577: _temp576= _temp552.f1; if((
unsigned int) _temp576 > 1u?(( struct _enum_struct*) _temp576)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL579: _temp578=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp576)->f1; goto _LL575;} else{ goto _LL560;} _LL575: _temp574= _temp552.f2;
if( _temp574 == Cyc_Absyn_Unknown_b){ goto _LL559;} else{ goto _LL560;} _LL560:
_LL585: _temp584= _temp552.f1; if( _temp584 == Cyc_Absyn_Unknown_b){ goto _LL581;}
else{ goto _LL553;} _LL581: _temp580= _temp552.f2; if(( unsigned int) _temp580 >
1u?(( struct _enum_struct*) _temp580)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL583:
_temp582=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp580)->f1;
goto _LL561;} else{ goto _LL553;} _LL555: if( n1? ! n2: 0){ convs=({ struct Cyc_List_List*
_temp586=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp586->hd=( void*)({ struct Cyc_Toc_NullCheck_struct* _temp587=( struct Cyc_Toc_NullCheck_struct*)
GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct)); _temp587->tag= Cyc_Toc_NullCheck_tag;
_temp587->f1=( void*) t2;( void*) _temp587;}); _temp586->tl= convs; _temp586;});}
goto _LL553; _LL557: goto _LL553; _LL559: convs=({ struct Cyc_List_List*
_temp588=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp588->hd=( void*)({ struct Cyc_Toc_TagPtr_struct* _temp589=( struct Cyc_Toc_TagPtr_struct*)
GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct)); _temp589->tag= Cyc_Toc_TagPtr_tag;
_temp589->f1=( void*) Cyc_Toc_typ_to_c( t1); _temp589->f2= _temp578;( void*)
_temp589;}); _temp588->tl= convs; _temp588;}); goto _LL553; _LL561: convs=({
struct Cyc_List_List* _temp590=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp590->hd=( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp591=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp591->tag= Cyc_Toc_UntagPtr_tag; _temp591->f1=( void*) Cyc_Toc_typ_to_c( t1);
_temp591->f2= _temp582; _temp591->f3= n2;( void*) _temp591;}); _temp590->tl=
convs; _temp590;}); goto _LL553; _LL553:;} return convs;} _LL542: return 0;
_LL538:;}} static char _temp594[ 7u]="_throw"; static struct _tagged_string Cyc_Toc__throw_string=(
struct _tagged_string){ _temp594, _temp594, _temp594 + 7u}; static struct Cyc_Absyn_Abs_n_struct
_temp595={ 1u, 0}; static struct _tuple1 Cyc_Toc__throw_var=( struct _tuple1){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp595,.f2=& Cyc_Toc__throw_string}; static
struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({ struct Cyc_List_List* _temp596=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp596->hd=(
void*) e; _temp596->tl= 0; _temp596;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv(
int toplevel, void* t, struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c(
t); if( toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp597=( char*)"can't do null-check conversion at top-level";
struct _tagged_string _temp598; _temp598.curr= _temp597; _temp598.base= _temp597;
_temp598.last_plus_one= _temp597 + 44; _temp598;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp599= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp605; struct Cyc_Absyn_Conref* _temp607; struct Cyc_Absyn_Tqual*
_temp609; struct Cyc_Absyn_Conref* _temp611; void* _temp613; void* _temp615;
_LL601: if(( unsigned int) _temp599 > 5u?(( struct _enum_struct*) _temp599)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL606: _temp605=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp599)->f1; _LL616: _temp615=( void*)
_temp605.elt_typ; goto _LL614; _LL614: _temp613=( void*) _temp605.rgn_typ; goto
_LL612; _LL612: _temp611=( struct Cyc_Absyn_Conref*) _temp605.nullable; goto
_LL610; _LL610: _temp609=( struct Cyc_Absyn_Tqual*) _temp605.tq; goto _LL608;
_LL608: _temp607=( struct Cyc_Absyn_Conref*) _temp605.bounds; goto _LL602;}
else{ goto _LL603;} _LL603: goto _LL604; _LL602: elt_typ= _temp615;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp615, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL600;} _LL604: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp617=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp618;
_temp618.curr= _temp617; _temp618.base= _temp617; _temp618.last_plus_one=
_temp617 + 28; _temp618;})); _LL600:;} if( toplevel){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp619=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp619->tag= Cyc_Absyn_ArrayType_tag;
_temp619->f1=( void*) elt_typ; _temp619->f2= Cyc_Toc_mt_tq; _temp619->f3= sz;(
void*) _temp619;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp620=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp620->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp621=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp621->tag= Cyc_Absyn_Var_d_tag; _temp621->f1= vd;( void*) _temp621;}), 0);
_temp620->tl= Cyc_Toc_temp_topdecls; _temp620;});{ struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp( xexp,
sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp622=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp622->hd=( void*)({ struct _tuple7* _temp627=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp627->f1= 0; _temp627->f2= xexp; _temp627;});
_temp622->tl=({ struct Cyc_List_List* _temp623=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp623->hd=( void*)({ struct
_tuple7* _temp626=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp626->f1= 0; _temp626->f2= xexp; _temp626;}); _temp623->tl=({ struct Cyc_List_List*
_temp624=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp624->hd=( void*)({ struct _tuple7* _temp625=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp625->f1= 0; _temp625->f2= xplussz; _temp625;});
_temp624->tl= 0; _temp624;}); _temp623;}); _temp622;}), 0); return urm_exp;}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp628=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp629; _temp629.curr= _temp628; _temp629.base= _temp628; _temp629.last_plus_one=
_temp628 + 45; _temp629;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
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
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs= cs->tl){ void* _temp630=(
void*) cs->hd; void* _temp638; struct Cyc_Absyn_Exp* _temp640; void* _temp642;
int _temp644; struct Cyc_Absyn_Exp* _temp646; void* _temp648; _LL632: if(((
struct _enum_struct*) _temp630)->tag == Cyc_Toc_NullCheck_tag){ _LL639: _temp638=(
void*)(( struct Cyc_Toc_NullCheck_struct*) _temp630)->f1; goto _LL633;} else{
goto _LL634;} _LL634: if((( struct _enum_struct*) _temp630)->tag == Cyc_Toc_TagPtr_tag){
_LL643: _temp642=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp630)->f1; goto
_LL641; _LL641: _temp640=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp630)->f2; goto _LL635;} else{ goto _LL636;} _LL636: if((( struct
_enum_struct*) _temp630)->tag == Cyc_Toc_UntagPtr_tag){ _LL649: _temp648=( void*)((
struct Cyc_Toc_UntagPtr_struct*) _temp630)->f1; goto _LL647; _LL647: _temp646=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*) _temp630)->f2; goto
_LL645; _LL645: _temp644=( int)(( struct Cyc_Toc_UntagPtr_struct*) _temp630)->f3;
goto _LL637;} else{ goto _LL631;} _LL633: e= Cyc_Toc_null_check_conv( toplevel,
_temp638, e); goto _LL631; _LL635: e= Cyc_Toc_tag_ptr_conv( toplevel, _temp642,
_temp640, e); goto _LL631; _LL637: e= Cyc_Toc_untag_ptr_conv( toplevel, _temp648,
_temp646, _temp644, e); goto _LL631; _LL631:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp650=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp650->break_lab= 0; _temp650->continue_lab= 0; _temp650->fallthru_info=
0; _temp650->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple1*,
struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp650->toplevel= 1;
_temp650;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp651=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp651->break_lab= e->break_lab; _temp651->continue_lab=
e->continue_lab; _temp651->fallthru_info= e->fallthru_info; _temp651->varmap= e->varmap;
_temp651->toplevel= e->toplevel; _temp651;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp652=(* x).f1; struct
Cyc_List_List* _temp658; _LL654: if(( unsigned int) _temp652 > 1u?(( struct
_enum_struct*) _temp652)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL659: _temp658=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp652)->f1; goto
_LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL655:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp660=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp660=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=({ struct _tagged_string _temp661= Cyc_Absynpp_qvar2string(
x); xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp661.last_plus_one -
_temp661.curr, _temp661.curr);})};( struct _xenum_struct*) _temp660;})); _LL657:
goto _LL653; _LL653:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key, struct
Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return ans;}} static
struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env( struct Cyc_Toc_Env*
e, struct _tagged_string* break_l, struct _tagged_string* fallthru_l, struct Cyc_List_List*
fallthru_binders, struct Cyc_Toc_Env* next_case_env){ struct Cyc_List_List*
fallthru_vars= 0; for( 0; fallthru_binders != 0; fallthru_binders=
fallthru_binders->tl){ fallthru_vars=({ struct Cyc_List_List* _temp662=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp662->hd=( void*)((
struct Cyc_Absyn_Vardecl*) fallthru_binders->hd)->name; _temp662->tl=
fallthru_vars; _temp662;});} fallthru_vars=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( fallthru_vars);{ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp663=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp663->v=( void*)
break_l; _temp663;}); ans->fallthru_info=({ struct Cyc_Core_Opt* _temp664=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp664->v=(
void*)({ struct _tuple8* _temp665=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp665->f1= fallthru_l; _temp665->f2= fallthru_vars; _temp665->f3=
next_case_env->varmap; _temp665;}); _temp664;}); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp666=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp666->v=( void*) break_l; _temp666;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp667=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp667->v=( void*)({
struct _tuple8* _temp668=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp668->f1= next_l; _temp668->f2= 0; _temp668->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp668;}); _temp667;}); return ans;} static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp669=( char*)"expecting a literal format string";
struct _tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 34; _temp670;}));}{ struct _tagged_string
fmt_str;{ void* _temp671=( void*)(( struct Cyc_Absyn_Exp*) args->hd)->r; void*
_temp677; struct _tagged_string _temp679; int _temp681; _LL673: if((( struct
_enum_struct*) _temp671)->tag == Cyc_Absyn_Const_e_tag){ _LL678: _temp677=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp671)->f1; if(( unsigned int) _temp677 > 1u?((
struct _enum_struct*) _temp677)->tag == Cyc_Absyn_String_c_tag: 0){ _LL682:
_temp681=( int)(( struct Cyc_Absyn_String_c_struct*) _temp677)->f1; goto _LL680;
_LL680: _temp679=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp677)->f2; goto _LL674;} else{ goto _LL675;}} else{ goto _LL675;} _LL675:
goto _LL676; _LL674: fmt_str= _temp679; goto _LL672; _LL676: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp683=( char*)"expecting a literal format string"; struct _tagged_string
_temp684; _temp684.curr= _temp683; _temp684.base= _temp683; _temp684.last_plus_one=
_temp683 + 34; _temp684;})); _LL672:;}{ int len=( int)({ struct _tagged_string
_temp685= fmt_str;( unsigned int)( _temp685.last_plus_one - _temp685.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(( int)({ struct _tagged_string _temp686=
fmt_str; char* _temp688= _temp686.curr + i; if( _temp688 < _temp686.base? 1:
_temp688 >= _temp686.last_plus_one){ _throw( Null_Exception);}* _temp688;}) ==(
int)'%'){ i ++; if( i < len?( int)({ struct _tagged_string _temp689= fmt_str;
char* _temp691= _temp689.curr + i; if( _temp691 < _temp689.base? 1: _temp691 >=
_temp689.last_plus_one){ _throw( Null_Exception);}* _temp691;}) ==( int)'s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp692=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp692->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp692->tl= args; _temp692;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args= args->tl; typs= typs->tl;{
struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct
Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){ char c=({ struct
_tagged_string _temp693= fmt_str; char* _temp695= _temp693.curr + i; if(
_temp695 < _temp693.base? 1: _temp695 >= _temp693.last_plus_one){ _throw(
Null_Exception);}* _temp695;}); rev_fmt=({ struct Cyc_List_List* _temp696=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp696->hd=(
void*)(( int) c); _temp696->tl= rev_fmt; _temp696;}); if(( int) c !=( int)'%'){
continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp697=( char*)"bad format string"; struct
_tagged_string _temp698; _temp698.curr= _temp697; _temp698.base= _temp697;
_temp698.last_plus_one= _temp697 + 18; _temp698;}));}{ struct _tuple9 _temp701;
int _temp702; char _temp704; struct Cyc_List_List* _temp706; struct Cyc_List_List*
_temp708; struct Cyc_List_List* _temp710; struct Cyc_List_List* _temp712; struct
_tuple9* _temp699=( struct _tuple9*) x->v; _temp701=* _temp699; _LL713: _temp712=
_temp701.f1; goto _LL711; _LL711: _temp710= _temp701.f2; goto _LL709; _LL709:
_temp708= _temp701.f3; goto _LL707; _LL707: _temp706= _temp701.f4; goto _LL705;
_LL705: _temp704= _temp701.f5; goto _LL703; _LL703: _temp702= _temp701.f6; goto
_LL700; _LL700: i= _temp702 - 1; if(( int) _temp704 !=( int)'s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp714=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp714->hd=( void*) _temp712; _temp714->tl=({ struct Cyc_List_List* _temp715=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp715->hd=(
void*) _temp710; _temp715->tl=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp716->hd=( void*) _temp708;
_temp716->tl=({ struct Cyc_List_List* _temp717=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp717->hd=( void*) _temp706;
_temp717->tl= 0; _temp717;}); _temp716;}); _temp715;}); _temp714;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp718->hd=( void*)(( int) _temp704); _temp718->tl=
rev_fmt; _temp718;});{ struct Cyc_List_List* _temp719= _temp710; struct Cyc_List_List
_temp726; struct Cyc_List_List* _temp727; int _temp729; _LL721: if( _temp719 ==
0){ goto _LL723;} else{ _temp726=* _temp719; _LL730: _temp729=( int) _temp726.hd;
goto _LL728; _LL728: _temp727=( struct Cyc_List_List*) _temp726.tl; if( _temp727
== 0){ goto _LL725;} else{ goto _LL723;}} _LL725: if( _temp729 ==( int)'*'){
goto _LL722;} else{ goto _LL723;} _LL723: goto _LL724; _LL722: { struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp731=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp731->hd=( void*)({
struct _tuple10* _temp732=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp732->f1= temp; _temp732->f2=( void*) typs->hd; _temp732->f3=( struct Cyc_Absyn_Exp*)
args->hd; _temp732;}); _temp731->tl= rev_temps; _temp731;}); rev_result=({
struct Cyc_List_List* _temp733=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp733->hd=( void*) Cyc_Absyn_var_exp( temp, 0);
_temp733->tl= rev_result; _temp733;}); args= args->tl; typs= typs->tl; goto
_LL720;} _LL724: goto _LL720; _LL720:;}{ struct Cyc_List_List* _temp734=
_temp708; struct Cyc_List_List _temp741; struct Cyc_List_List* _temp742; struct
Cyc_List_List _temp744; struct Cyc_List_List* _temp745; int _temp747; int
_temp749; _LL736: if( _temp734 == 0){ goto _LL738;} else{ _temp741=* _temp734;
_LL750: _temp749=( int) _temp741.hd; goto _LL743; _LL743: _temp742=( struct Cyc_List_List*)
_temp741.tl; if( _temp742 == 0){ goto _LL738;} else{ _temp744=* _temp742; _LL748:
_temp747=( int) _temp744.hd; goto _LL746; _LL746: _temp745=( struct Cyc_List_List*)
_temp744.tl; if( _temp745 == 0){ goto _LL740;} else{ goto _LL738;}}} _LL740: if(
_temp749 ==( int)'.'? _temp747 ==( int)'*': 0){ goto _LL737;} else{ goto _LL738;}
_LL738: goto _LL739; _LL737: { struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp751=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp751->hd=( void*)({ struct _tuple10*
_temp752=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp752->f1=
temp; _temp752->f2=( void*) typs->hd; _temp752->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp752;}); _temp751->tl= rev_temps; _temp751;}); rev_result=({ struct Cyc_List_List*
_temp753=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp753->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp753->tl= rev_result;
_temp753;}); args= args->tl; typs= typs->tl; goto _LL735;} _LL739: goto _LL735;
_LL735:;} if(( int) _temp704 !=( int)'%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp754=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp754.last_plus_one
- _temp754.curr, _temp754.curr);}));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp755=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp755->hd=( void*)({ struct _tuple10*
_temp756=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp756->f1=
temp; _temp756->f2=( void*) typs->hd; _temp756->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp756;}); _temp755->tl= rev_temps; _temp755;}); rev_result=({ struct Cyc_List_List*
_temp757=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp757->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp757->tl= rev_result;
_temp757;}); args= args->tl; typs= typs->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp758=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp758->hd=( void*)(( int)'.'); _temp758->tl= rev_fmt; _temp758;}); rev_fmt=({
struct Cyc_List_List* _temp759=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp759->hd=( void*)(( int)'*'); _temp759->tl= rev_fmt;
_temp759;}); rev_fmt=({ struct Cyc_List_List* _temp760=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp760->hd=( void*)(( int)'s');
_temp760->tl= rev_fmt; _temp760;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp761=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp761->hd=( void*)({ struct _tuple10*
_temp762=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp762->f1=
temp; _temp762->f2=( void*) typs->hd; _temp762->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp762;}); _temp761->tl= rev_temps; _temp761;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp763=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp763->hd=( void*) size_exp; _temp763->tl= rev_result; _temp763;});
rev_result=({ struct Cyc_List_List* _temp764=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp764->hd=( void*) cstring_exp; _temp764->tl=
rev_result; _temp764;}); args= args->tl; typs= typs->tl;}}}}}}{ struct Cyc_List_List*
es=({ struct Cyc_List_List* _temp765=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp765->hd=( void*) Cyc_Absyn_string_exp( 0, Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp765->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp765;}); if( aopt != 0){ es=({ struct Cyc_List_List* _temp766=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp766->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp766->tl=( struct Cyc_List_List*)
es; _temp766;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p,( struct Cyc_List_List*)
es, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps
!= 0; rev_temps= rev_temps->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*)
rev_temps->hd)).f1,(*(( struct _tuple10*) rev_temps->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple10*) rev_temps->hd)).f3, s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}}
static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp767=( char*)"Missing type in primop "; struct _tagged_string _temp768;
_temp768.curr= _temp767; _temp768.base= _temp767; _temp768.last_plus_one=
_temp767 + 24; _temp768;}));} return Cyc_Toc_typ_to_c(( void*)( e->topt)->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp769=( char*)"Missing type in primop "; struct _tagged_string _temp770;
_temp770.curr= _temp769; _temp770.base= _temp769; _temp770.last_plus_one=
_temp769 + 24; _temp770;}));} return( void*)( e->topt)->v;} static struct
_tuple5* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple5*
_temp771=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp771->f1=
Cyc_Toc_mt_tq; _temp771->f2= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); _temp771;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple7* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple7* _temp772=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp772[ 0]=({ struct
_tuple7 _temp773; _temp773.f1= 0; _temp773.f2= e; _temp773;}); _temp772;});}
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
_temp774=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp774[ 0]= xprintf("f%d", i); _temp774;}), 0),( struct Cyc_Absyn_Exp*) es->hd,
0), 0), s, 0);}} return Cyc_Toc_make_struct( x, Cyc_Absyn_strct( n), s, pointer,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_List_List* dles, struct _tuple1*
tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es= es->tl){ struct _tuple7 _temp777;
struct Cyc_Absyn_Exp* _temp778; struct Cyc_List_List* _temp780; struct _tuple7*
_temp775=( struct _tuple7*) es->hd; _temp777=* _temp775; _LL781: _temp780=
_temp777.f1; goto _LL779; _LL779: _temp778= _temp777.f2; goto _LL776; _LL776:
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)( _temp778->topt)->v): 0; if(
_temp780 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp782=( char*)"empty designator list"; struct
_tagged_string _temp783; _temp783.curr= _temp782; _temp783.base= _temp782;
_temp783.last_plus_one= _temp782 + 22; _temp783;}));} if( _temp780->tl != 0){
struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp(
y, 0); for( 0; _temp780 != 0; _temp780= _temp780->tl){ void* _temp784=( void*)
_temp780->hd; struct _tagged_string* _temp790; _LL786: if((( struct _enum_struct*)
_temp784)->tag == Cyc_Absyn_FieldName_tag){ _LL791: _temp790=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp784)->f1; goto
_LL787;} else{ goto _LL788;} _LL788: goto _LL789; _LL787: if( Cyc_Toc_is_poly_field(
struct_type, _temp790)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp790, 0), yexp, 0), 0), s, 0); goto _LL785; _LL789:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp792=( char*)"array designator in struct";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 27; _temp793;}))); _LL785:;} Cyc_Toc_exp_to_c(
nv, _temp778); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp778, 0), 0), s, 0);} else{ void* _temp794=( void*) _temp780->hd;
struct _tagged_string* _temp800; _LL796: if((( struct _enum_struct*) _temp794)->tag
== Cyc_Absyn_FieldName_tag){ _LL801: _temp800=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp794)->f1; goto _LL797;} else{ goto _LL798;}
_LL798: goto _LL799; _LL797: Cyc_Toc_exp_to_c( nv, _temp778); if( Cyc_Toc_is_poly_field(
struct_type, _temp800)){ _temp778= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp778, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp800, 0), _temp778, 0), 0), s, 0); goto _LL795; _LL799:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp802=( char*)"array designator in struct";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 27; _temp803;}))); _LL795:;}}} return Cyc_Toc_make_struct(
x, Cyc_Absyn_strctq( tdn), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp804=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp804->tag= Cyc_Absyn_Increment_e_tag; _temp804->f1= e1; _temp804->f2=( void*)
incr;( void*) _temp804;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp805=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp815; struct Cyc_Absyn_Exp* _temp817; void* _temp819; struct _tagged_string*
_temp821; struct Cyc_Absyn_Exp* _temp823; _LL807: if((( struct _enum_struct*)
_temp805)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL816: _temp815=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp805)->f1; goto _LL808;} else{ goto
_LL809;} _LL809: if((( struct _enum_struct*) _temp805)->tag == Cyc_Absyn_Cast_e_tag){
_LL820: _temp819=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp805)->f1; goto
_LL818; _LL818: _temp817=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp805)->f2; goto _LL810;} else{ goto _LL811;} _LL811: if((( struct
_enum_struct*) _temp805)->tag == Cyc_Absyn_StructMember_e_tag){ _LL824: _temp823=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp805)->f1;
goto _LL822; _LL822: _temp821=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp805)->f2; goto _LL812;} else{ goto _LL813;} _LL813: goto _LL814; _LL808:((
void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp815, fs, f, f_env); goto _LL806; _LL810:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp817, fs, f, f_env); goto
_LL806; _LL812:( void*)( e1->r=( void*)(( void*) _temp823->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp825=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp825->hd=( void*) _temp821; _temp825->tl= fs;
_temp825;}), f, f_env); goto _LL806; _LL814: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp826= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp826.last_plus_one - _temp826.curr, _temp826.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_string*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)((
void*)( f( e1_copy, f_env))->r)); goto _LL806;} _LL806:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp827=( void*) s->r;
struct Cyc_Absyn_Exp* _temp837; struct Cyc_Absyn_Stmt* _temp839; struct Cyc_Absyn_Decl*
_temp841; struct Cyc_Absyn_Stmt* _temp843; struct Cyc_Absyn_Stmt* _temp845;
_LL829: if(( unsigned int) _temp827 > 1u?(( struct _enum_struct*) _temp827)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL838: _temp837=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp827)->f1; goto _LL830;} else{ goto _LL831;} _LL831:
if(( unsigned int) _temp827 > 1u?(( struct _enum_struct*) _temp827)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL842: _temp841=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp827)->f1; goto _LL840; _LL840: _temp839=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp827)->f2; goto _LL832;} else{ goto _LL833;}
_LL833: if(( unsigned int) _temp827 > 1u?(( struct _enum_struct*) _temp827)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL846: _temp845=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp827)->f1; goto _LL844; _LL844: _temp843=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp827)->f2; goto _LL834;}
else{ goto _LL835;} _LL835: goto _LL836; _LL830:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp837, fs, f, f_env); goto
_LL828; _LL832:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp839, fs, f, f_env); goto _LL828; _LL834:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp843, fs, f, f_env); goto
_LL828; _LL836:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp847= Cyc_Absynpp_stmt2string( s); xprintf("lvalue_assign_stmt: %.*s",
_temp847.last_plus_one - _temp847.curr, _temp847.curr);})); goto _LL828; _LL828:;}
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
_tagged_string _temp848= Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s",
_temp848.last_plus_one - _temp848.curr, _temp848.curr);})); return;}{ void*
old_typ=( void*)( e->topt)->v; void* _temp849= r; void* _temp925; void* _temp928;
struct _tagged_string _temp930; int _temp932; void* _temp934; void* _temp936;
struct _tuple1* _temp938; struct _tuple1* _temp940; struct Cyc_List_List*
_temp942; void* _temp944; void* _temp946; struct Cyc_Absyn_Exp* _temp948; struct
Cyc_Absyn_Exp* _temp950; struct Cyc_Core_Opt* _temp952; struct Cyc_Absyn_Exp*
_temp954; struct Cyc_Absyn_Exp* _temp956; struct Cyc_Absyn_Exp* _temp958; struct
Cyc_Absyn_Exp* _temp960; struct Cyc_Absyn_Exp* _temp962; struct Cyc_Absyn_Exp*
_temp964; struct Cyc_List_List* _temp966; struct Cyc_Absyn_Exp* _temp968; struct
Cyc_List_List* _temp970; struct Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Exp*
_temp974; struct Cyc_Absyn_Exp* _temp976; struct Cyc_List_List* _temp978; struct
Cyc_Absyn_Exp* _temp980; struct Cyc_Absyn_Exp* _temp982; struct Cyc_Absyn_Exp**
_temp984; void* _temp985; void** _temp987; struct Cyc_Absyn_Exp* _temp988; void*
_temp990; struct Cyc_Absyn_Exp* _temp992; struct Cyc_Absyn_Exp* _temp994; struct
_tagged_string* _temp996; struct Cyc_Absyn_Exp* _temp998; struct _tagged_string*
_temp1000; struct Cyc_Absyn_Exp* _temp1002; struct Cyc_Absyn_Exp* _temp1004;
struct Cyc_Absyn_Exp* _temp1006; struct Cyc_List_List* _temp1008; struct Cyc_List_List*
_temp1010; struct _tuple2* _temp1012; struct Cyc_List_List* _temp1014; int
_temp1016; struct Cyc_Absyn_Exp* _temp1018; struct Cyc_Absyn_Exp* _temp1020;
struct Cyc_Absyn_Vardecl* _temp1022; struct Cyc_Absyn_Structdecl* _temp1024;
struct Cyc_List_List* _temp1026; struct Cyc_Core_Opt* _temp1028; struct _tuple1*
_temp1030; struct Cyc_Absyn_Enumfield* _temp1032; struct Cyc_Absyn_Enumdecl*
_temp1034; struct Cyc_List_List* _temp1036; struct Cyc_Core_Opt* _temp1038;
struct Cyc_Core_Opt* _temp1040; struct Cyc_Absyn_Enumfield* _temp1042; struct
Cyc_Absyn_Enumdecl* _temp1044; struct Cyc_List_List* _temp1046; struct Cyc_Core_Opt*
_temp1048; struct Cyc_Core_Opt* _temp1050; struct Cyc_Absyn_Enumfield* _temp1052;
struct Cyc_Absyn_Xenumdecl* _temp1054; struct Cyc_List_List* _temp1056; struct
Cyc_Core_Opt* _temp1058; void* _temp1060; void* _temp1062; void* _temp1064;
struct Cyc_Absyn_Stmt* _temp1066; struct Cyc_List_List* _temp1068; struct Cyc_Core_Opt*
_temp1070; struct Cyc_Absyn_Fndecl* _temp1072; struct Cyc_Absyn_Exp* _temp1074;
_LL851: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Const_e_tag){
_LL926: _temp925=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp849)->f1; if(
_temp925 == Cyc_Absyn_Null_c){ goto _LL852;} else{ goto _LL853;}} else{ goto
_LL853;} _LL853: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Const_e_tag){
_LL929: _temp928=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp849)->f1; if((
unsigned int) _temp928 > 1u?(( struct _enum_struct*) _temp928)->tag == Cyc_Absyn_String_c_tag:
0){ _LL933: _temp932=( int)(( struct Cyc_Absyn_String_c_struct*) _temp928)->f1;
goto _LL931; _LL931: _temp930=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp928)->f2; goto _LL927;} else{ goto _LL855;}} else{ goto _LL855;} _LL927:
if( _temp932){ goto _LL854;} else{ goto _LL855;} _LL855: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Const_e_tag){ _LL935: _temp934=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp849)->f1; goto _LL856;} else{ goto _LL857;}
_LL857: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Var_e_tag){
_LL939: _temp938=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp849)->f1;
goto _LL937; _LL937: _temp936=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp849)->f2; goto _LL858;} else{ goto _LL859;} _LL859: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL941: _temp940=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp849)->f1; goto
_LL860;} else{ goto _LL861;} _LL861: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Primop_e_tag){ _LL945: _temp944=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp849)->f1; goto _LL943; _LL943: _temp942=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp849)->f2; goto _LL862;} else{ goto _LL863;}
_LL863: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Increment_e_tag){
_LL949: _temp948=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp849)->f1; goto _LL947; _LL947: _temp946=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp849)->f2; goto _LL864;} else{ goto _LL865;} _LL865: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL955: _temp954=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp849)->f1;
goto _LL953; _LL953: _temp952=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp849)->f2; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp849)->f3; goto _LL866;} else{ goto _LL867;}
_LL867: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Conditional_e_tag){
_LL961: _temp960=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp849)->f1; goto _LL959; _LL959: _temp958=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp849)->f2; goto _LL957; _LL957: _temp956=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp849)->f3;
goto _LL868;} else{ goto _LL869;} _LL869: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL965: _temp964=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp849)->f1; goto _LL963; _LL963: _temp962=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp849)->f2; goto
_LL870;} else{ goto _LL871;} _LL871: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL969: _temp968=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp849)->f1; goto _LL967; _LL967:
_temp966=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp849)->f2; goto _LL872;} else{ goto _LL873;} _LL873: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_FnCall_e_tag){ _LL973: _temp972=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp849)->f1; goto
_LL971; _LL971: _temp970=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp849)->f2; goto _LL874;} else{ goto _LL875;} _LL875: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Throw_e_tag){ _LL975: _temp974=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp849)->f1; goto
_LL876;} else{ goto _LL877;} _LL877: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL977: _temp976=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp849)->f1; goto _LL878;} else{
goto _LL879;} _LL879: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL981: _temp980=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp849)->f1; goto _LL979; _LL979: _temp978=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp849)->f2; goto _LL880;} else{ goto _LL881;}
_LL881: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Cast_e_tag){
_LL986: _temp985=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp849)->f1;
_temp987=&(( struct Cyc_Absyn_Cast_e_struct*) _temp849)->f1; goto _LL983; _LL983:
_temp982=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp849)->f2;
_temp984=&(( struct Cyc_Absyn_Cast_e_struct*) _temp849)->f2; goto _LL882;} else{
goto _LL883;} _LL883: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Address_e_tag){
_LL989: _temp988=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp849)->f1; goto _LL884;} else{ goto _LL885;} _LL885: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL991: _temp990=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp849)->f1; goto _LL886;}
else{ goto _LL887;} _LL887: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL993: _temp992=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp849)->f1; goto _LL888;} else{ goto _LL889;} _LL889: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Deref_e_tag){ _LL995: _temp994=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp849)->f1; goto
_LL890;} else{ goto _LL891;} _LL891: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL999: _temp998=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp849)->f1; goto _LL997; _LL997:
_temp996=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp849)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL1003:
_temp1002=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp849)->f1; goto _LL1001; _LL1001: _temp1000=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp849)->f2; goto _LL894;} else{ goto
_LL895;} _LL895: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1007: _temp1006=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp849)->f1; goto _LL1005; _LL1005: _temp1004=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp849)->f2; goto _LL896;} else{ goto
_LL897;} _LL897: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1009: _temp1008=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp849)->f1; goto _LL898;} else{ goto _LL899;} _LL899: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1013:
_temp1012=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp849)->f1;
goto _LL1011; _LL1011: _temp1010=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp849)->f2; goto _LL900;} else{ goto _LL901;} _LL901: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Array_e_tag){ _LL1017: _temp1016=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp849)->f1; goto _LL1015; _LL1015:
_temp1014=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp849)->f2;
goto _LL902;} else{ goto _LL903;} _LL903: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1023: _temp1022=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp849)->f1; goto _LL1021; _LL1021:
_temp1020=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp849)->f2; goto _LL1019; _LL1019: _temp1018=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp849)->f3; goto _LL904;} else{
goto _LL905;} _LL905: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Struct_e_tag){
_LL1031: _temp1030=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp849)->f1; goto _LL1029; _LL1029: _temp1028=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp849)->f2; goto _LL1027; _LL1027: _temp1026=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp849)->f3; goto
_LL1025; _LL1025: _temp1024=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp849)->f4; goto _LL906;} else{ goto _LL907;} _LL907: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Enum_e_tag){ _LL1041: _temp1040=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp849)->f1; goto
_LL1039; _LL1039: _temp1038=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp849)->f2; goto _LL1037; _LL1037: _temp1036=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp849)->f3; if( _temp1036 == 0){ goto
_LL1035;} else{ goto _LL909;} _LL1035: _temp1034=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp849)->f4; goto _LL1033; _LL1033: _temp1032=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp849)->f5;
goto _LL908;} else{ goto _LL909;} _LL909: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Enum_e_tag){ _LL1051: _temp1050=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp849)->f1; goto _LL1049; _LL1049: _temp1048=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp849)->f2; goto
_LL1047; _LL1047: _temp1046=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp849)->f3; goto _LL1045; _LL1045: _temp1044=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp849)->f4; goto _LL1043; _LL1043: _temp1042=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp849)->f5;
goto _LL910;} else{ goto _LL911;} _LL911: if((( struct _enum_struct*) _temp849)->tag
== Cyc_Absyn_Xenum_e_tag){ _LL1059: _temp1058=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp849)->f1; goto _LL1057; _LL1057: _temp1056=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp849)->f2; goto
_LL1055; _LL1055: _temp1054=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp849)->f3; goto _LL1053; _LL1053: _temp1052=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp849)->f4; goto _LL912;} else{ goto _LL913;}
_LL913: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1061: _temp1060=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp849)->f1;
if((( struct _enum_struct*) _temp1060)->tag == Cyc_Absyn_Typ_m_tag){ _LL1063:
_temp1062=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp1060)->f1; goto _LL914;}
else{ goto _LL915;}} else{ goto _LL915;} _LL915: if((( struct _enum_struct*)
_temp849)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1065: _temp1064=( void*)(( struct
Cyc_Absyn_Malloc_e_struct*) _temp849)->f1; goto _LL916;} else{ goto _LL917;}
_LL917: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1067: _temp1066=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp849)->f1; goto _LL918;} else{ goto _LL919;} _LL919: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1071:
_temp1070=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp849)->f1; goto _LL1069; _LL1069: _temp1068=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp849)->f2; goto _LL920;} else{
goto _LL921;} _LL921: if((( struct _enum_struct*) _temp849)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1073: _temp1072=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp849)->f1; goto _LL922;} else{ goto _LL923;} _LL923: if((( struct
_enum_struct*) _temp849)->tag == Cyc_Absyn_Fill_e_tag){ _LL1075: _temp1074=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp849)->f1; goto
_LL924;} else{ goto _LL850;} _LL852:{ void* _temp1076= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1082; struct Cyc_Absyn_Conref* _temp1084;
struct Cyc_Absyn_Tqual* _temp1086; struct Cyc_Absyn_Conref* _temp1088; void*
_temp1090; void* _temp1092; _LL1078: if(( unsigned int) _temp1076 > 5u?(( struct
_enum_struct*) _temp1076)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1083:
_temp1082=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1076)->f1; _LL1093: _temp1092=( void*) _temp1082.elt_typ; goto _LL1091;
_LL1091: _temp1090=( void*) _temp1082.rgn_typ; goto _LL1089; _LL1089: _temp1088=(
struct Cyc_Absyn_Conref*) _temp1082.nullable; goto _LL1087; _LL1087: _temp1086=(
struct Cyc_Absyn_Tqual*) _temp1082.tq; goto _LL1085; _LL1085: _temp1084=( struct
Cyc_Absyn_Conref*) _temp1082.bounds; goto _LL1079;} else{ goto _LL1080;} _LL1080:
goto _LL1081; _LL1079:{ void* _temp1094=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1084); struct Cyc_Absyn_Exp* _temp1100; _LL1096: if(
_temp1094 == Cyc_Absyn_Unknown_b){ goto _LL1097;} else{ goto _LL1098;} _LL1098:
if(( unsigned int) _temp1094 > 1u?(( struct _enum_struct*) _temp1094)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL1101: _temp1100=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1094)->f1; goto _LL1099;} else{ goto _LL1095;}
_LL1097: { struct _tuple7* zero=({ struct _tuple7* _temp1102=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7)); _temp1102->f1= 0; _temp1102->f2= Cyc_Absyn_signed_int_exp(
0, 0); _temp1102;}); struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List* _temp1103=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1103->hd=( void*) zero; _temp1103->tl=({ struct Cyc_List_List*
_temp1104=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1104->hd=( void*) zero; _temp1104->tl=({ struct Cyc_List_List* _temp1105=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1105->hd=(
void*) zero; _temp1105->tl= 0; _temp1105;}); _temp1104;}); _temp1103;}), 0); if(
nv->toplevel){( void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct(
Cyc_Toc_add_tagged_ptr_type( Cyc_Toc_typ_to_c( _temp1092), Cyc_Toc_mt_tq));
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*)
um, Cyc_Absyn_exp_stmt( xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));} goto _LL1095;} _LL1099:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1095; _LL1095:;} goto _LL1077; _LL1081:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1106= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1106.last_plus_one -
_temp1106.curr, _temp1106.curr);})); goto _LL1077; _LL1077:;} goto _LL850;
_LL854:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1107=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1107->f1= Cyc_Toc_abs_ns; _temp1107->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1107;}), 0),({ struct Cyc_List_List* _temp1108=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1108->hd=( void*) Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp1109=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1109->tag= Cyc_Absyn_Const_e_tag;
_temp1109->f1=( void*)({ struct Cyc_Absyn_String_c_struct* _temp1110=( struct
Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1110->tag= Cyc_Absyn_String_c_tag; _temp1110->f1= 0; _temp1110->f2=
_temp930;( void*) _temp1110;});( void*) _temp1109;}), 0); _temp1108->tl= 0;
_temp1108;}), 0))->r)); goto _LL850; _LL856: goto _LL850; _LL858:{ struct
_handler_cons _temp1111; _push_handler(& _temp1111);{ struct _xenum_struct*
_temp1112=( struct _xenum_struct*) setjmp( _temp1111.handler); if( ! _temp1112){(
void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict*
d, struct _tuple1* key)) Cyc_Dict_lookup)( nv->varmap, _temp938))->r));;
_pop_handler();} else{ struct _xenum_struct* _temp1114= _temp1112; _LL1116: if(
_temp1114->tag == Cyc_Dict_Absent_tag){ goto _LL1117;} else{ goto _LL1118;}
_LL1118: goto _LL1119; _LL1117:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp1120= Cyc_Absynpp_qvar2string( _temp938); xprintf("Can't find %.*s in exp_to_c, Var\n",
_temp1120.last_plus_one - _temp1120.curr, _temp1120.curr);})); return; _LL1119:(
void) _throw( _temp1114); _LL1115:;}}} goto _LL850; _LL860:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1121=(
char*)"unknownid"; struct _tagged_string _temp1122; _temp1122.curr= _temp1121;
_temp1122.base= _temp1121; _temp1122.last_plus_one= _temp1121 + 10; _temp1122;}));
goto _LL850; _LL862: { struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp942); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp942);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp942);{ void* _temp1123= _temp944; _LL1125: if( _temp1123 == Cyc_Absyn_Size){
goto _LL1126;} else{ goto _LL1127;} _LL1127: if( _temp1123 == Cyc_Absyn_Printf){
goto _LL1128;} else{ goto _LL1129;} _LL1129: if( _temp1123 == Cyc_Absyn_Xprintf){
goto _LL1130;} else{ goto _LL1131;} _LL1131: if( _temp1123 == Cyc_Absyn_Fprintf){
goto _LL1132;} else{ goto _LL1133;} _LL1133: if( _temp1123 == Cyc_Absyn_Sscanf){
goto _LL1134;} else{ goto _LL1135;} _LL1135: if( _temp1123 == Cyc_Absyn_Plus){
goto _LL1136;} else{ goto _LL1137;} _LL1137: if( _temp1123 == Cyc_Absyn_Minus){
goto _LL1138;} else{ goto _LL1139;} _LL1139: if( _temp1123 == Cyc_Absyn_Eq){
goto _LL1140;} else{ goto _LL1141;} _LL1141: if( _temp1123 == Cyc_Absyn_Neq){
goto _LL1142;} else{ goto _LL1143;} _LL1143: if( _temp1123 == Cyc_Absyn_Gt){
goto _LL1144;} else{ goto _LL1145;} _LL1145: if( _temp1123 == Cyc_Absyn_Gte){
goto _LL1146;} else{ goto _LL1147;} _LL1147: if( _temp1123 == Cyc_Absyn_Lt){
goto _LL1148;} else{ goto _LL1149;} _LL1149: if( _temp1123 == Cyc_Absyn_Lte){
goto _LL1150;} else{ goto _LL1151;} _LL1151: goto _LL1152; _LL1126: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*) _temp942->hd;{ void* _temp1153=( void*)( arg->topt)->v;
struct Cyc_Absyn_Exp* _temp1161; struct Cyc_Absyn_Tqual* _temp1163; void*
_temp1165; struct Cyc_Absyn_PtrInfo _temp1167; struct Cyc_Absyn_Conref*
_temp1169; struct Cyc_Absyn_Tqual* _temp1171; struct Cyc_Absyn_Conref* _temp1173;
void* _temp1175; void* _temp1177; _LL1155: if(( unsigned int) _temp1153 > 5u?((
struct _enum_struct*) _temp1153)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1166:
_temp1165=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1153)->f1; goto
_LL1164; _LL1164: _temp1163=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1153)->f2; goto _LL1162; _LL1162: _temp1161=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1153)->f3; goto _LL1156;} else{ goto
_LL1157;} _LL1157: if(( unsigned int) _temp1153 > 5u?(( struct _enum_struct*)
_temp1153)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1168: _temp1167=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1153)->f1;
_LL1178: _temp1177=( void*) _temp1167.elt_typ; goto _LL1176; _LL1176: _temp1175=(
void*) _temp1167.rgn_typ; goto _LL1174; _LL1174: _temp1173=( struct Cyc_Absyn_Conref*)
_temp1167.nullable; goto _LL1172; _LL1172: _temp1171=( struct Cyc_Absyn_Tqual*)
_temp1167.tq; goto _LL1170; _LL1170: _temp1169=( struct Cyc_Absyn_Conref*)
_temp1167.bounds; goto _LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160;
_LL1156:( void*)( e->r=( void*)(( void*) _temp1161->r)); goto _LL1154; _LL1158: {
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)( arg->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1154;}
_LL1160:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1179= Cyc_Absynpp_typ2string(( void*) typs->hd); struct
_tagged_string _temp1180= Cyc_Absynpp_typ2string(( void*)( arg->topt)->v);
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1179.last_plus_one
- _temp1179.curr, _temp1179.curr, _temp1180.last_plus_one - _temp1180.curr,
_temp1180.curr);})); return; _LL1154:;} goto _LL1124;} _LL1128:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp944, 0, _temp942, typs))->r)); goto
_LL1124; _LL1130:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp944,
0, _temp942, typs))->r)); goto _LL1124; _LL1132: if( _temp942 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1181=( char*)"fprintf without arguments"; struct _tagged_string _temp1182;
_temp1182.curr= _temp1181; _temp1182.base= _temp1181; _temp1182.last_plus_one=
_temp1181 + 26; _temp1182;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp944,({ struct Cyc_Core_Opt* _temp1183=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1183->v=( void*)(( struct Cyc_Absyn_Exp*)
_temp942->hd); _temp1183;}), _temp942->tl, typs->tl))->r)); goto _LL1124;
_LL1134: if( _temp942 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1184=( char*)"sscanf without arguments";
struct _tagged_string _temp1185; _temp1185.curr= _temp1184; _temp1185.base=
_temp1184; _temp1185.last_plus_one= _temp1184 + 25; _temp1185;}));}( void*)(((
struct Cyc_Absyn_Exp*) _temp942->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*) _temp942->hd)->r,(( struct
Cyc_Absyn_Exp*) _temp942->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1124;
_LL1136: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp942->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp942->tl)->hd; struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp,
0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1186=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1186->v=( void*) Cyc_Absyn_Plus; _temp1186;}), e2, 0);
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1124; _LL1138: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp942->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp942->tl)->hd; void*
old_typ_e2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1187=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1187->v=( void*) Cyc_Absyn_Minus; _temp1187;}), e2,
0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1124; _LL1140: goto _LL1142; _LL1142:
goto _LL1144; _LL1144: goto _LL1146; _LL1146: goto _LL1148; _LL1148: goto
_LL1150; _LL1150: { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp942->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp942->tl)->hd; void* t1=(
void*) old_typs->hd; void* t2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1124;} _LL1152: goto
_LL1124; _LL1124:;} goto _LL850;} _LL864: { void* e2_cyc_typ=( void*)( _temp948->topt)->v;
Cyc_Toc_exp_to_c( nv, _temp948); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp946 == Cyc_Absyn_PreInc? 1: _temp946 == Cyc_Absyn_PreDec){
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ), Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp946, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp948, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple1* temp=
Cyc_Toc_temp_var(); struct _tuple1* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ), Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp946, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp948, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp948)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp948, 0, Cyc_Toc_incr_lvalue,
_temp946);( void*)( e->r=( void*)(( void*) _temp948->r));}} goto _LL850;} _LL866: {
int e1_poly= Cyc_Toc_is_poly_project( _temp954); void* e1_old_typ=( void*)(
_temp954->topt)->v; void* e2_old_typ=( void*)( _temp950->topt)->v; Cyc_Toc_exp_to_c(
nv, _temp954); Cyc_Toc_exp_to_c( nv, _temp950); if( _temp952 != 0?( void*)
_temp952->v == Cyc_Absyn_Plus? 1:( void*) _temp952->v == Cyc_Absyn_Minus: 0){
if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp954->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp954->r, 0),
Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ( e2_old_typ)){(
void*)( _temp950->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp950->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if( e1_poly){( void*)(
_temp950->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp950->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp954)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp954, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1188=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1188[ 0]=({ struct _tuple11 _temp1189; _temp1189.f1=
_temp952; _temp1189.f2= _temp950; _temp1189;}); _temp1188;}));( void*)( e->r=(
void*)(( void*) _temp954->r));} goto _LL850;} _LL868: Cyc_Toc_exp_to_c( nv,
_temp960); Cyc_Toc_exp_to_c( nv, _temp958); Cyc_Toc_exp_to_c( nv, _temp956);
goto _LL850; _LL870: Cyc_Toc_exp_to_c( nv, _temp964); Cyc_Toc_exp_to_c( nv,
_temp962); goto _LL850; _LL872: _temp972= _temp968; _temp970= _temp966; goto
_LL874; _LL874: Cyc_Toc_exp_to_c( nv, _temp972);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp970); goto _LL850; _LL876: Cyc_Toc_exp_to_c(
nv, _temp974);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp974), 0))->r)); goto _LL850; _LL878: Cyc_Toc_exp_to_c(
nv, _temp976); goto _LL850; _LL880: Cyc_Toc_exp_to_c( nv, _temp980);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); for( 0; _temp978 != 0; _temp978=
_temp978->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp978->hd); if( k != Cyc_Absyn_EffKind?
k != Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp980, 0))->r)); break;}} goto _LL850;} _LL882: { void* old_t2=( void*)((*
_temp984)->topt)->v; void* new_typ=* _temp987;* _temp987= Cyc_Toc_typ_to_c(
new_typ); Cyc_Toc_exp_to_c( nv,* _temp984);* _temp984= Cyc_Toc_convert_exp( nv->toplevel,
old_t2, new_typ,* _temp984); goto _LL850;} _LL884:{ void* _temp1190=( void*)
_temp988->r; struct Cyc_Absyn_Structdecl* _temp1198; struct Cyc_List_List*
_temp1200; struct Cyc_Core_Opt* _temp1202; struct _tuple1* _temp1204; struct Cyc_List_List*
_temp1206; _LL1192: if((( struct _enum_struct*) _temp1190)->tag == Cyc_Absyn_Struct_e_tag){
_LL1205: _temp1204=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1190)->f1; goto _LL1203; _LL1203: _temp1202=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1190)->f2; goto _LL1201; _LL1201:
_temp1200=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1190)->f3; goto _LL1199; _LL1199: _temp1198=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1190)->f4; goto _LL1193;} else{ goto
_LL1194;} _LL1194: if((( struct _enum_struct*) _temp1190)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1207: _temp1206=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1190)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: goto _LL1197;
_LL1193: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1208= Cyc_Position_string_of_segment( _temp988->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1208.last_plus_one -
_temp1208.curr, _temp1208.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)( _temp988->topt)->v, 1, _temp1200, _temp1204))->r)); goto _LL1191;
_LL1195: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1209= Cyc_Position_string_of_segment( _temp988->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1209.last_plus_one -
_temp1209.curr, _temp1209.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1206))->r)); goto _LL1191; _LL1197: Cyc_Toc_exp_to_c( nv, _temp988);
if( ! Cyc_Absyn_is_lvalue( _temp988)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int),
int f_env)) Cyc_Toc_lvalue_assign)( _temp988, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*) _temp988->r));} goto _LL1191; _LL1191:;} goto
_LL850; _LL886:( void*)( e->r=( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1210=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1210->tag= Cyc_Absyn_Sizeoftyp_e_tag; _temp1210->f1=( void*) Cyc_Toc_typ_to_c_array(
_temp990);( void*) _temp1210;})); goto _LL850; _LL888: Cyc_Toc_exp_to_c( nv,
_temp992); goto _LL850; _LL890: Cyc_Toc_exp_to_c( nv, _temp994);{ void*
_temp1211= Cyc_Tcutil_compress(( void*)( _temp994->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1217; struct Cyc_Absyn_Conref* _temp1219; struct Cyc_Absyn_Tqual* _temp1221;
struct Cyc_Absyn_Conref* _temp1223; void* _temp1225; void* _temp1227; _LL1213:
if(( unsigned int) _temp1211 > 5u?(( struct _enum_struct*) _temp1211)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1218: _temp1217=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1211)->f1; _LL1228: _temp1227=( void*)
_temp1217.elt_typ; goto _LL1226; _LL1226: _temp1225=( void*) _temp1217.rgn_typ;
goto _LL1224; _LL1224: _temp1223=( struct Cyc_Absyn_Conref*) _temp1217.nullable;
goto _LL1222; _LL1222: _temp1221=( struct Cyc_Absyn_Tqual*) _temp1217.tq; goto
_LL1220; _LL1220: _temp1219=( struct Cyc_Absyn_Conref*) _temp1217.bounds; goto
_LL1214;} else{ goto _LL1215;} _LL1215: goto _LL1216; _LL1214:{ void* _temp1229=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1219); struct
Cyc_Absyn_Exp* _temp1235; _LL1231: if( _temp1229 == Cyc_Absyn_Unknown_b){ goto
_LL1232;} else{ goto _LL1233;} _LL1233: if(( unsigned int) _temp1229 > 1u?((
struct _enum_struct*) _temp1229)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1236:
_temp1235=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1229)->f1;
goto _LL1234;} else{ goto _LL1230;} _LL1232: { struct _tuple1* x= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1227); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1221); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1221); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp994, s1, 0), 0))->r));
goto _LL1230;} _LL1234: { int nullable= Cyc_Toc_is_nullable(( void*)( _temp994->topt)->v);
if( nullable){ struct _tuple1* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)( _temp994->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( _temp1235->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp994, s3, 0), 0))->r));} goto _LL1230;} _LL1230:;} goto _LL1212; _LL1216:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1237=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1238; _temp1238.curr= _temp1237; _temp1238.base= _temp1237; _temp1238.last_plus_one=
_temp1237 + 29; _temp1238;})); goto _LL1212; _LL1212:;} goto _LL850; _LL892: Cyc_Toc_exp_to_c(
nv, _temp998); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL850; _LL894: Cyc_Toc_exp_to_c( nv, _temp1002);
if( Cyc_Toc_is_nullable(( void*)( _temp1002->topt)->v)){ struct _tuple1* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)( _temp1002->topt)->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1000, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1002, s3, 0), 0))->r));} else{ if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}} goto _LL850; _LL896: { void*
e1_typ= Cyc_Tcutil_compress(( void*)( _temp1006->topt)->v); Cyc_Toc_exp_to_c( nv,
_temp1006); Cyc_Toc_exp_to_c( nv, _temp1004);( void*)( _temp1006->r=( void*)((
void*)( Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp((
void*) _temp1006->r, 0), 0))->r));{ void* _temp1239= e1_typ; struct Cyc_List_List*
_temp1249; struct Cyc_Absyn_Exp* _temp1251; struct Cyc_Absyn_Tqual* _temp1253;
void* _temp1255; struct Cyc_Absyn_PtrInfo _temp1257; struct Cyc_Absyn_Conref*
_temp1259; struct Cyc_Absyn_Tqual* _temp1261; struct Cyc_Absyn_Conref* _temp1263;
void* _temp1265; void* _temp1267; _LL1241: if(( unsigned int) _temp1239 > 5u?((
struct _enum_struct*) _temp1239)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1250:
_temp1249=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1239)->f1; goto _LL1242;} else{ goto _LL1243;} _LL1243: if(( unsigned int)
_temp1239 > 5u?(( struct _enum_struct*) _temp1239)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1256: _temp1255=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1239)->f1;
goto _LL1254; _LL1254: _temp1253=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1239)->f2; goto _LL1252; _LL1252: _temp1251=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1239)->f3; goto _LL1244;} else{ goto
_LL1245;} _LL1245: if(( unsigned int) _temp1239 > 5u?(( struct _enum_struct*)
_temp1239)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1258: _temp1257=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1239)->f1;
_LL1268: _temp1267=( void*) _temp1257.elt_typ; goto _LL1266; _LL1266: _temp1265=(
void*) _temp1257.rgn_typ; goto _LL1264; _LL1264: _temp1263=( struct Cyc_Absyn_Conref*)
_temp1257.nullable; goto _LL1262; _LL1262: _temp1261=( struct Cyc_Absyn_Tqual*)
_temp1257.tq; goto _LL1260; _LL1260: _temp1259=( struct Cyc_Absyn_Conref*)
_temp1257.bounds; goto _LL1246;} else{ goto _LL1247;} _LL1247: goto _LL1248;
_LL1242: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1004) +(
unsigned int) 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1006,({ struct _tagged_string* _temp1269=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1269[ 0]= xprintf("f%d",(
int) i); _temp1269;}), 0))->r)); goto _LL1240;} _LL1244:( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1006, _temp1004, _temp1251, _temp1255,
_temp1253, 0))->r)); goto _LL1240; _LL1246:{ void* _temp1270=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1259); struct Cyc_Absyn_Exp*
_temp1276; _LL1272: if(( unsigned int) _temp1270 > 1u?(( struct _enum_struct*)
_temp1270)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1277: _temp1276=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1270)->f1; goto _LL1273;} else{ goto
_LL1274;} _LL1274: if( _temp1270 == Cyc_Absyn_Unknown_b){ goto _LL1275;} else{
goto _LL1271;} _LL1273:( void*)( e->r=( void*)(( void*)( Cyc_Toc_known_size_subscript(
_temp1006, _temp1004, _temp1276, _temp1267, _temp1261, 0))->r)); goto _LL1271;
_LL1275: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1267); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1261); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1261); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1004, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1006, s1, 0), 0))->r)); goto _LL1271;} _LL1271:;}
goto _LL1240; _LL1248:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1278=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1279; _temp1279.curr= _temp1278; _temp1279.base=
_temp1278; _temp1279.last_plus_one= _temp1278 + 49; _temp1279;})); goto _LL1240;
_LL1240:;} goto _LL850;} _LL898: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp1008))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1008); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1008 != 0; _temp1008= _temp1008->tl, i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp1008->hd);{ struct Cyc_List_List* des=({ struct
Cyc_List_List* _temp1280=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1280->hd=( void*)({ struct Cyc_Absyn_FieldName_struct* _temp1281=( struct
Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1281->tag= Cyc_Absyn_FieldName_tag; _temp1281->f1=({ struct _tagged_string*
_temp1282=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1282[ 0]= xprintf("f%d", i); _temp1282;});( void*) _temp1281;});
_temp1280->tl= 0; _temp1280;}); dles=({ struct Cyc_List_List* _temp1283=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1283->hd=( void*)({
struct _tuple7* _temp1284=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1284->f1=( struct Cyc_List_List*) des; _temp1284->f2=( struct Cyc_Absyn_Exp*)
_temp1008->hd; _temp1284;}); _temp1283->tl= dles; _temp1283;});}}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp(
0, dles, 0), 0))->r));} goto _LL850; _LL900:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1285=( char*)"compoundlit";
struct _tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base=
_temp1285; _temp1286.last_plus_one= _temp1285 + 12; _temp1286;})); goto _LL850;
_LL902: if( nv->toplevel? 1: ! _temp1016){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1014, 0))->r));{ struct Cyc_List_List* dles= _temp1014; for( 0; dles !=
0; dles= dles->tl){ struct _tuple7 _temp1289; struct Cyc_Absyn_Exp* _temp1290;
struct Cyc_List_List* _temp1292; struct _tuple7* _temp1287=( struct _tuple7*)
dles->hd; _temp1289=* _temp1287; _LL1293: _temp1292= _temp1289.f1; goto _LL1291;
_LL1291: _temp1290= _temp1289.f2; goto _LL1288; _LL1288: Cyc_Toc_exp_to_c( nv,
_temp1290);}}} else{ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1014) - 1;{ struct Cyc_List_List* dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1014);
for( 0; dles != 0; dles= dles->tl){ struct _tuple7 _temp1296; struct Cyc_Absyn_Exp*
_temp1297; struct Cyc_List_List* _temp1299; struct _tuple7* _temp1294=( struct
_tuple7*) dles->hd; _temp1296=* _temp1294; _LL1300: _temp1299= _temp1296.f1;
goto _LL1298; _LL1298: _temp1297= _temp1296.f2; goto _LL1295; _LL1295: { struct
Cyc_Absyn_Exp* e_index; if( _temp1299 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if( _temp1299->tl != 0){(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1301=( char*)"multiple designators in array";
struct _tagged_string _temp1302; _temp1302.curr= _temp1301; _temp1302.base=
_temp1301; _temp1302.last_plus_one= _temp1301 + 30; _temp1302;}));}{ void* d=(
void*) _temp1299->hd; void* _temp1303= d; struct Cyc_Absyn_Exp* _temp1309;
struct _tagged_string* _temp1311; _LL1305: if((( struct _enum_struct*) _temp1303)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1310: _temp1309=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1303)->f1; goto _LL1306;} else{ goto
_LL1307;} _LL1307: if((( struct _enum_struct*) _temp1303)->tag == Cyc_Absyn_FieldName_tag){
_LL1312: _temp1311=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1303)->f1; goto _LL1308;} else{ goto _LL1304;} _LL1306: Cyc_Toc_exp_to_c(
nv, _temp1309); e_index= _temp1309; goto _LL1304; _LL1308: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1313=( char*)"field name designators in array"; struct _tagged_string
_temp1314; _temp1314.curr= _temp1313; _temp1314.base= _temp1313; _temp1314.last_plus_one=
_temp1313 + 32; _temp1314;})); goto _LL1304; _LL1304:;}} Cyc_Toc_exp_to_c( nv,
_temp1297); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1297, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1315= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1321; struct Cyc_Absyn_Conref* _temp1323; struct Cyc_Absyn_Tqual* _temp1325;
struct Cyc_Absyn_Conref* _temp1327; void* _temp1329; void* _temp1331; _LL1317:
if(( unsigned int) _temp1315 > 5u?(( struct _enum_struct*) _temp1315)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1322: _temp1321=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1315)->f1; _LL1332: _temp1331=( void*)
_temp1321.elt_typ; goto _LL1330; _LL1330: _temp1329=( void*) _temp1321.rgn_typ;
goto _LL1328; _LL1328: _temp1327=( struct Cyc_Absyn_Conref*) _temp1321.nullable;
goto _LL1326; _LL1326: _temp1325=( struct Cyc_Absyn_Tqual*) _temp1321.tq; goto
_LL1324; _LL1324: _temp1323=( struct Cyc_Absyn_Conref*) _temp1321.bounds; goto
_LL1318;} else{ goto _LL1319;} _LL1319: goto _LL1320; _LL1318: old_elt_typ=
_temp1331; goto _LL1316; _LL1320: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1333=( char*)"exp_to_c:array expression doesn't have array type";
struct _tagged_string _temp1334; _temp1334.curr= _temp1333; _temp1334.base=
_temp1333; _temp1334.last_plus_one= _temp1333 + 50; _temp1334;})); goto _LL1316;
_LL1316:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1014), 0), 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ,
e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r));}}} goto _LL850; _LL904: { struct _tuple1*
x1= _temp1022->name; int is_tagged_ptr= 0;{ void* _temp1335= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1341; struct Cyc_Absyn_Conref* _temp1343;
struct Cyc_Absyn_Tqual* _temp1345; struct Cyc_Absyn_Conref* _temp1347; void*
_temp1349; void* _temp1351; _LL1337: if(( unsigned int) _temp1335 > 5u?(( struct
_enum_struct*) _temp1335)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1342:
_temp1341=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1335)->f1; _LL1352: _temp1351=( void*) _temp1341.elt_typ; goto _LL1350;
_LL1350: _temp1349=( void*) _temp1341.rgn_typ; goto _LL1348; _LL1348: _temp1347=(
struct Cyc_Absyn_Conref*) _temp1341.nullable; goto _LL1346; _LL1346: _temp1345=(
struct Cyc_Absyn_Tqual*) _temp1341.tq; goto _LL1344; _LL1344: _temp1343=( struct
Cyc_Absyn_Conref*) _temp1341.bounds; goto _LL1338;} else{ goto _LL1339;} _LL1339:
goto _LL1340; _LL1338:{ void* _temp1353=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1343))->v; void*
_temp1359; _LL1355: if(( unsigned int) _temp1353 > 1u?(( struct _enum_struct*)
_temp1353)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1360: _temp1359=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1353)->f1; if( _temp1359 == Cyc_Absyn_Unknown_b){
goto _LL1356;} else{ goto _LL1357;}} else{ goto _LL1357;} _LL1357: goto _LL1358;
_LL1356: is_tagged_ptr= 1; goto _LL1354; _LL1358: goto _LL1354; _LL1354:;} goto
_LL1336; _LL1340:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1361=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1362; _temp1362.curr= _temp1361; _temp1362.base=
_temp1361; _temp1362.last_plus_one= _temp1361 + 42; _temp1362;})); goto _LL1336;
_LL1336:;}{ void* old_elt_typ=( void*)( _temp1018->topt)->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1020);{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp(
x1,({ struct Cyc_Absyn_Local_b_struct* _temp1363=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1363->tag= Cyc_Absyn_Local_b_tag;
_temp1363->f1= _temp1022;( void*) _temp1363;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1018);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1018, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1364=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1364->hd=( void*)({ struct _tuple10* _temp1365=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1365->f1= max;
_temp1365->f2= Cyc_Absyn_uint_t; _temp1365->f3=( struct Cyc_Absyn_Exp*)
_temp1020; _temp1365;}); _temp1364->tl= 0; _temp1364;}); struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0)), 0); decls=({
struct Cyc_List_List* _temp1366=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1366->hd=( void*)({ struct _tuple10* _temp1367=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1367->f1= a;
_temp1367->f2= ptr_typ; _temp1367->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1367;});
_temp1366->tl= decls; _temp1366;}); decls=({ struct Cyc_List_List* _temp1368=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1368->hd=(
void*)({ struct _tuple10* _temp1369=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1369->f1= x1; _temp1369->f2= Cyc_Absyn_uint_t; _temp1369->f3=
0; _temp1369;}); _temp1368->tl= decls; _temp1368;}); if( is_tagged_ptr){ struct
_tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp*
binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1370=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1370->hd=(
void*)({ struct _tuple7* _temp1375=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp1375->f1= 0; _temp1375->f2= Cyc_Absyn_var_exp( a, 0); _temp1375;});
_temp1370->tl=({ struct Cyc_List_List* _temp1371=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1371->hd=( void*)({ struct
_tuple7* _temp1374=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1374->f1= 0; _temp1374->f2= Cyc_Absyn_var_exp( a, 0); _temp1374;});
_temp1371->tl=({ struct Cyc_List_List* _temp1372=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1372->hd=( void*)({ struct
_tuple7* _temp1373=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1373->f1= 0; _temp1373->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1373;}); _temp1372->tl= 0; _temp1372;});
_temp1371;}); _temp1370;}), 0); decls=({ struct Cyc_List_List* _temp1376=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1376->hd=(
void*)({ struct _tuple10* _temp1377=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1377->f1= b; _temp1377->f2= t; _temp1377->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1377;}); _temp1376->tl= decls; _temp1376;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Exp* _temp1380; void*
_temp1382; struct _tuple1* _temp1384; struct _tuple10 _temp1378=*(( struct
_tuple10*) ds->hd); _LL1385: _temp1384= _temp1378.f1; goto _LL1383; _LL1383:
_temp1382= _temp1378.f2; goto _LL1381; _LL1381: _temp1380= _temp1378.f3; goto
_LL1379; _LL1379: s= Cyc_Absyn_declare_stmt( _temp1384, _temp1382, _temp1380, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r)); goto
_LL850;}}}} _LL906: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv, old_typ, 0, _temp1026, _temp1030))->r));} else{ struct _tuple1* n= _temp1030;
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp1387= _temp1024 == 0?(( struct Cyc_Absyn_Structdecl*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1386= Cyc_Absynpp_qvar2string(
_temp1030); xprintf("no structdecl %.*s", _temp1386.last_plus_one - _temp1386.curr,
_temp1386.curr);})): _temp1024; if( _temp1387 == 0){ _throw( Null_Exception);}
_temp1387;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1026,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields= fields->tl){ struct _tuple12 _temp1390;
struct Cyc_Absyn_Exp* _temp1391; struct Cyc_Absyn_Structfield* _temp1393; struct
_tuple12* _temp1388=( struct _tuple12*) fields->hd; _temp1390=* _temp1388;
_LL1394: _temp1393= _temp1390.f1; goto _LL1392; _LL1392: _temp1391= _temp1390.f2;
goto _LL1389; _LL1389: { void* fieldtyp=( void*) _temp1393->type; Cyc_Toc_exp_to_c(
nv, _temp1391); if( Cyc_Toc_is_void_star( fieldtyp)){( void*)( _temp1391->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1391->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0, _temp1026, 0), 0))->r));}
goto _LL850; _LL908: { struct _tuple1* qv= _temp1032->name; if( ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* fields= _temp1034->fields == 0? 0:( struct
Cyc_List_List*)( _temp1034->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,(( struct
Cyc_Absyn_Enumfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Enumfield*)
fields->hd)->typs == 0){ tag_count ++;} fields= fields->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0), 0))->r));} goto _LL850;} _LL910: { struct _tuple1*
qv= _temp1042->name; struct Cyc_List_List* tqts= _temp1042->typs; struct _tuple1*
x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0);
void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1395=( char*)"_struct"; struct _tagged_string
_temp1396; _temp1396.curr= _temp1395; _temp1396.base= _temp1395; _temp1396.last_plus_one=
_temp1395 + 8; _temp1396;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Toc_mt_tq); if( nv->toplevel){ int tag_count= 0; struct Cyc_List_List*
fields= _temp1044->fields == 0? 0:( struct Cyc_List_List*)( _temp1044->fields)->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Enumfield*) fields->hd)->name)
!= 0) { if((( struct Cyc_Absyn_Enumfield*) fields->hd)->typs != 0){ tag_count ++;}
fields= fields->tl;}{ struct Cyc_List_List* dles= 0; for( 0; _temp1046 != 0;
_temp1046= _temp1046->tl, tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct
Cyc_Absyn_Exp*) _temp1046->hd; void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple5*) tqts->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1397=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1397[ 0]=({ struct Cyc_List_List _temp1398;
_temp1398.hd=( void*)({ struct _tuple7* _temp1399=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7) * 1); _temp1399[ 0]=({ struct _tuple7 _temp1400;
_temp1400.f1= 0; _temp1400.f2= cur_e; _temp1400;}); _temp1399;}); _temp1398.tl=
dles; _temp1398;}); _temp1397;});}{ struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0); dles=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1401=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1401[ 0]=({ struct Cyc_List_List _temp1402; _temp1402.hd=( void*)({ struct
_tuple7* _temp1403=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp1403[ 0]=({ struct _tuple7 _temp1404; _temp1404.f1= 0; _temp1404.f2=
tag_exp; _temp1404;}); _temp1403;}); _temp1402.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1402;}); _temp1401;});{
struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles, 0); struct
Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x, strct_typ,( struct Cyc_Absyn_Exp*)
init_exp); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List* _temp1405=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1405->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1406=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1406->tag= Cyc_Absyn_Var_d_tag;
_temp1406->f1= vd;( void*) _temp1406;}), 0); _temp1405->tl= Cyc_Toc_temp_topdecls;
_temp1405;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( pstrct_typ,
Cyc_Absyn_address_exp( xexp, 0), 0))->r));}}}} else{ struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_sizeoftyp_exp( strct_typ, 0); int atomic= 1; struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1407=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1407->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structarrow_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1408=( char*)"_tag"; struct
_tagged_string _temp1409; _temp1409.curr= _temp1408; _temp1409.base= _temp1408;
_temp1409.last_plus_one= _temp1408 + 5; _temp1409;})), 0), 0); _temp1407->tl= 0;
_temp1407;});{ int i= 1; for( 0; _temp1046 != 0;( _temp1046= _temp1046->tl, i ++),
tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1046->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple5*) tqts->hd)).f2); if( !
Cyc_Toc_atomic_typ(( void*)( cur_e->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,
cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structarrow_exp(
xexp,({ struct _tagged_string* _temp1410=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1410[ 0]= xprintf("f%d", i); _temp1410;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1411=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1411->hd=( void*) a; _temp1411->tl=(
struct Cyc_List_List*) rev_assign; _temp1411;});}}}{ struct Cyc_Absyn_Exp* e1=
Cyc_Absyn_cast_exp( pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr(
e2), 0); struct Cyc_Absyn_Stmt* final_cast= Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c( old_typ), xexp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1412=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1412->hd=( void*) final_cast; _temp1412->tl=( struct Cyc_List_List*)
rev_assign; _temp1412;})), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}}
goto _LL850;} _LL912: { struct _tuple1* qv= _temp1052->name; struct _tuple1* x=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); void*
strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1413=( char*)"_struct"; struct _tagged_string
_temp1414; _temp1414.curr= _temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one=
_temp1413 + 8; _temp1414;}))); void* pstrct_typ= Cyc_Absyn_cstar_typ( strct_typ,
Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1415=( char*)"_tag"; struct
_tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base= _temp1415;
_temp1416.last_plus_one= _temp1415 + 5; _temp1416;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1056); dles=({ struct Cyc_List_List*
_temp1417=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1417->hd=( void*)({ struct _tuple7* _temp1418=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1418->f1= 0; _temp1418->f2= tag_exp; _temp1418;});
_temp1417->tl= dles; _temp1417;});{ struct Cyc_Absyn_Exp* init_exp= Cyc_Absyn_unresolvedmem_exp(
0, dles, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( x,
strct_typ,( struct Cyc_Absyn_Exp*) init_exp); Cyc_Toc_temp_topdecls=({ struct
Cyc_List_List* _temp1419=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1419->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1420=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1420->tag= Cyc_Absyn_Var_d_tag; _temp1420->f1= vd;( void*) _temp1420;}), 0);
_temp1419->tl= Cyc_Toc_temp_topdecls; _temp1419;});( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), Cyc_Absyn_address_exp(
xexp, 0), 0))->r));}} else{ int atomic= 1; struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp(
strct_typ, 0); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1056 != 0;
_temp1056= _temp1056->tl, i ++){ if( ! Cyc_Toc_atomic_typ(( void*)((( struct Cyc_Absyn_Exp*)
_temp1056->hd)->topt)->v)){ atomic= 0;} Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_temp1056->hd);{ struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1421=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1421->hd=(
void*)({ struct Cyc_Absyn_FieldName_struct* _temp1422=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1422->tag= Cyc_Absyn_FieldName_tag;
_temp1422->f1=({ struct _tagged_string* _temp1423=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1423[ 0]= xprintf("f%d", i);
_temp1423;});( void*) _temp1422;}); _temp1421->tl= 0; _temp1421;}); dles=({
struct Cyc_List_List* _temp1424=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1424->hd=( void*)({ struct _tuple7* _temp1425=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1425->f1=( struct Cyc_List_List*)
des; _temp1425->f2=( struct Cyc_Absyn_Exp*) _temp1056->hd; _temp1425;});
_temp1424->tl= dles; _temp1424;});}}} dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( dles);{ struct Cyc_Absyn_Exp* e1= Cyc_Absyn_cast_exp(
pstrct_typ, atomic? Cyc_Toc_malloc_atomic( e2): Cyc_Toc_malloc_ptr( e2), 0);
struct Cyc_List_List* d=({ struct Cyc_List_List* _temp1426=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1426->hd=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp1427=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1427->tag= Cyc_Absyn_FieldName_tag; _temp1427->f1= Cyc_Toc_tag_sp;( void*)
_temp1427;}); _temp1426->tl= 0; _temp1426;}); struct Cyc_List_List* dles2=({
struct Cyc_List_List* _temp1428=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1428->hd=( void*)({ struct _tuple7* _temp1429=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1429->f1=( struct Cyc_List_List*)
d; _temp1429->f2= tag_exp; _temp1429;}); _temp1428->tl= dles; _temp1428;});
struct Cyc_Absyn_Exp* strct_exp= Cyc_Absyn_unresolvedmem_exp( 0, dles2, 0);
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_deref_exp( xexp, 0),
Cyc_Absyn_cast_exp( strct_typ, strct_exp, 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt(
s1, Cyc_Absyn_exp_stmt( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( old_typ), xexp, 0),
0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, pstrct_typ,( struct Cyc_Absyn_Exp*) e1, s2, 0), 0))->r));}} goto _LL850;}
_LL914: { void* t_c= Cyc_Toc_typ_to_c( _temp1062);( void*)( e->r=( void*)(( void*)(
Cyc_Toc_malloc_exp( _temp1062, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r)); goto
_LL850;} _LL916:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1430=( char*)"unimplemented Malloc_e form";
struct _tagged_string _temp1431; _temp1431.curr= _temp1430; _temp1431.base=
_temp1430; _temp1431.last_plus_one= _temp1430 + 28; _temp1431;})); goto _LL850;
_LL918: Cyc_Toc_stmt_to_c( nv, _temp1066); goto _LL850; _LL920:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1432=( char*)"UnresolvedMem"; struct _tagged_string _temp1433; _temp1433.curr=
_temp1432; _temp1433.base= _temp1432; _temp1433.last_plus_one= _temp1432 + 14;
_temp1433;})); goto _LL850; _LL922:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1434=( char*)"codegen"; struct
_tagged_string _temp1435; _temp1435.curr= _temp1434; _temp1435.base= _temp1434;
_temp1435.last_plus_one= _temp1434 + 8; _temp1435;})); goto _LL850; _LL924:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1436=( char*)"fill"; struct _tagged_string _temp1437; _temp1437.curr=
_temp1436; _temp1437.base= _temp1436; _temp1437.last_plus_one= _temp1436 + 5;
_temp1437;})); goto _LL850; _LL850:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1438=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1470; struct Cyc_Absyn_Vardecl _temp1472; struct Cyc_List_List* _temp1473;
struct Cyc_Core_Opt* _temp1475; int _temp1477; struct Cyc_Absyn_Exp* _temp1479;
void* _temp1481; struct Cyc_Absyn_Tqual* _temp1483; struct _tuple1* _temp1485;
void* _temp1487; struct Cyc_Absyn_Vardecl* _temp1489; int _temp1491; void*
_temp1493; char _temp1495; struct _tagged_string _temp1497; struct Cyc_Absyn_Enumfield*
_temp1499; struct Cyc_Absyn_Enumdecl* _temp1501; struct Cyc_List_List* _temp1503;
struct Cyc_List_List* _temp1505; struct Cyc_Core_Opt* _temp1507; struct _tuple1*
_temp1509; struct Cyc_Absyn_Enumfield* _temp1511; struct Cyc_Absyn_Xenumdecl*
_temp1513; struct Cyc_List_List* _temp1515; struct Cyc_List_List* _temp1517;
struct _tuple1* _temp1519; struct Cyc_List_List* _temp1521; struct Cyc_List_List*
_temp1523; struct Cyc_List_List* _temp1525; struct Cyc_Core_Opt* _temp1527;
struct Cyc_Absyn_Structdecl* _temp1529; struct Cyc_Absyn_Pat* _temp1531; struct
_tuple1* _temp1533; struct Cyc_List_List* _temp1535; struct Cyc_List_List*
_temp1537; struct _tuple1* _temp1539; struct Cyc_List_List* _temp1541; struct
Cyc_List_List* _temp1543; struct _tuple1* _temp1545; _LL1440: if( _temp1438 ==
Cyc_Absyn_Wild_p){ goto _LL1441;} else{ goto _LL1442;} _LL1442: if((
unsigned int) _temp1438 > 2u?(( struct _enum_struct*) _temp1438)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL1471: _temp1470=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1438)->f1; _temp1472=* _temp1470; _LL1488: _temp1487=( void*) _temp1472.sc;
goto _LL1486; _LL1486: _temp1485=( struct _tuple1*) _temp1472.name; goto _LL1484;
_LL1484: _temp1483=( struct Cyc_Absyn_Tqual*) _temp1472.tq; goto _LL1482;
_LL1482: _temp1481=( void*) _temp1472.type; goto _LL1480; _LL1480: _temp1479=(
struct Cyc_Absyn_Exp*) _temp1472.initializer; goto _LL1478; _LL1478: _temp1477=(
int) _temp1472.shadow; goto _LL1476; _LL1476: _temp1475=( struct Cyc_Core_Opt*)
_temp1472.region; goto _LL1474; _LL1474: _temp1473=( struct Cyc_List_List*)
_temp1472.attributes; goto _LL1443;} else{ goto _LL1444;} _LL1444: if((
unsigned int) _temp1438 > 2u?(( struct _enum_struct*) _temp1438)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1490: _temp1489=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1438)->f1; goto _LL1445;} else{ goto _LL1446;} _LL1446: if( _temp1438 ==
Cyc_Absyn_Null_p){ goto _LL1447;} else{ goto _LL1448;} _LL1448: if((
unsigned int) _temp1438 > 2u?(( struct _enum_struct*) _temp1438)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1494: _temp1493=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1438)->f1;
goto _LL1492; _LL1492: _temp1491=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1438)->f2; goto _LL1449;} else{ goto _LL1450;} _LL1450: if(( unsigned int)
_temp1438 > 2u?(( struct _enum_struct*) _temp1438)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1496: _temp1495=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1438)->f1;
goto _LL1451;} else{ goto _LL1452;} _LL1452: if(( unsigned int) _temp1438 > 2u?((
struct _enum_struct*) _temp1438)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1498:
_temp1497=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1438)->f1;
goto _LL1453;} else{ goto _LL1454;} _LL1454: if(( unsigned int) _temp1438 > 2u?((
struct _enum_struct*) _temp1438)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1510:
_temp1509=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1438)->f1;
goto _LL1508; _LL1508: _temp1507=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1438)->f2; goto _LL1506; _LL1506: _temp1505=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1438)->f3; goto _LL1504; _LL1504:
_temp1503=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1438)->f4;
goto _LL1502; _LL1502: _temp1501=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1438)->f5; goto _LL1500; _LL1500: _temp1499=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1438)->f6; goto _LL1455;} else{ goto
_LL1456;} _LL1456: if(( unsigned int) _temp1438 > 2u?(( struct _enum_struct*)
_temp1438)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1520: _temp1519=( struct
_tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1438)->f1; goto _LL1518;
_LL1518: _temp1517=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1438)->f2; goto _LL1516; _LL1516: _temp1515=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1438)->f3; goto _LL1514; _LL1514:
_temp1513=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1438)->f4; goto _LL1512; _LL1512: _temp1511=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1438)->f5; goto _LL1457;} else{ goto
_LL1458;} _LL1458: if(( unsigned int) _temp1438 > 2u?(( struct _enum_struct*)
_temp1438)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1522: _temp1521=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp1438)->f1; goto _LL1459;} else{ goto
_LL1460;} _LL1460: if(( unsigned int) _temp1438 > 2u?(( struct _enum_struct*)
_temp1438)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL1530: _temp1529=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1438)->f1; goto _LL1528; _LL1528:
_temp1527=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1438)->f2;
goto _LL1526; _LL1526: _temp1525=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1438)->f3; goto _LL1524; _LL1524: _temp1523=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1438)->f4; goto _LL1461;} else{ goto
_LL1462;} _LL1462: if(( unsigned int) _temp1438 > 2u?(( struct _enum_struct*)
_temp1438)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1532: _temp1531=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1438)->f1; goto _LL1463;} else{ goto
_LL1464;} _LL1464: if(( unsigned int) _temp1438 > 2u?(( struct _enum_struct*)
_temp1438)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1534: _temp1533=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1438)->f1; goto _LL1465;}
else{ goto _LL1466;} _LL1466: if(( unsigned int) _temp1438 > 2u?(( struct
_enum_struct*) _temp1438)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1540:
_temp1539=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1438)->f1;
goto _LL1538; _LL1538: _temp1537=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1438)->f2; goto _LL1536; _LL1536: _temp1535=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1438)->f3; goto _LL1467;} else{
goto _LL1468;} _LL1468: if(( unsigned int) _temp1438 > 2u?(( struct _enum_struct*)
_temp1438)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1546: _temp1545=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1438)->f1;
goto _LL1544; _LL1544: _temp1543=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1438)->f2; goto _LL1542; _LL1542: _temp1541=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1438)->f3; goto _LL1469;} else{
goto _LL1439;} _LL1441: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1439;
_LL1443: nv= Cyc_Toc_add_varmap( nv, _temp1485, r); s= Cyc_Absyn_goto_stmt(
succ_lab, 0); goto _LL1439; _LL1445: { struct _tuple1* v= Cyc_Toc_temp_var();
decls=({ struct Cyc_List_List* _temp1547=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1547->hd=( void*)({ struct _tuple14*
_temp1548=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1548->f1=
v; _temp1548->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1548;}); _temp1547->tl= decls; _temp1547;}); nv= Cyc_Toc_add_varmap( nv,
_temp1489->name, Cyc_Absyn_var_exp( v, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1439;} _LL1447: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1439; _LL1449: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1493, _temp1491, 0), succ_lab, fail_lab); goto _LL1439;
_LL1451: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1495, 0),
succ_lab, fail_lab); goto _LL1439; _LL1453: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1497, 0), succ_lab, fail_lab); goto _LL1439; _LL1455: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1503);
struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1499->name,( struct
_tagged_string)({ char* _temp1549=( char*)"_struct"; struct _tagged_string
_temp1550; _temp1550.curr= _temp1549; _temp1550.base= _temp1549; _temp1550.last_plus_one=
_temp1549 + 8; _temp1550;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1503); for( 0; rps != 0; rps= rps->tl, cnt --){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=(
void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string*
lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List* _temp1551=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1551->hd=( void*)({
struct _tuple14* _temp1552=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp1552->f1= v; _temp1552->f2= t2c; _temp1552;}); _temp1551->tl=
decls; _temp1551;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1553=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1553[ 0]= xprintf("f%d", cnt); _temp1553;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp(
t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1554=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1554[ 0]= xprintf("f%d", cnt); _temp1554;}), 0), 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List* _temp1555=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1555->hd=(
void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1555->tl= ss; _temp1555;});
succ_lab= lab;}}}} if( _temp1503 == 0){ s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_var_exp(
_temp1509, 0), succ_lab, fail_lab);} else{ unsigned int max_tag= 0;{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)( _temp1501->fields)->v; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->typs
== 0){ if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1556=( char*)"can't deal with explicit tags yet";
struct _tagged_string _temp1557; _temp1557.curr= _temp1556; _temp1557.base=
_temp1556; _temp1557.last_plus_one= _temp1556 + 34; _temp1557;}));} max_tag ++;}}}{
struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__enum_struct_sp),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp( e3,
Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1509,( struct _tagged_string)({ char* _temp1558=(
char*)"_tag"; struct _tagged_string _temp1559; _temp1559.curr= _temp1558;
_temp1559.base= _temp1558; _temp1559.last_plus_one= _temp1558 + 5; _temp1559;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag >( unsigned int) 0? Cyc_Absyn_and_exp(
e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);}}
goto _LL1439;} _LL1457: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp1515); struct _tuple1* xfstrct= Cyc_Toc_collapse_qvar_tag(
_temp1511->name,( struct _tagged_string)({ char* _temp1560=( char*)"_struct";
struct _tagged_string _temp1561; _temp1561.curr= _temp1560; _temp1561.base=
_temp1560; _temp1561.last_plus_one= _temp1560 + 8; _temp1561;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1515); for( 0; rps != 0; rps= rps->tl, cnt --){ struct
Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1562=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1562->hd=( void*)({ struct _tuple14*
_temp1563=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1563->f1=
v; _temp1563->f2= Cyc_Toc_typ_to_c( t2); _temp1563;}); _temp1562->tl= decls;
_temp1562;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1564=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1564[ 0]= xprintf("f%d", cnt); _temp1564;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1565=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1565[ 0]= xprintf("f%d", cnt);
_temp1565;}), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0);
ss=({ struct Cyc_List_List* _temp1566=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1566->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1566->tl= ss; _temp1566;}); succ_lab= lab;}}}} if( _temp1515 == 0){
s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0),
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1519,( struct _tagged_string)({
char* _temp1567=( char*)"_tag"; struct _tagged_string _temp1568; _temp1568.curr=
_temp1567; _temp1568.base= _temp1567; _temp1568.last_plus_one= _temp1567 + 5;
_temp1568;})), 0), succ_lab, fail_lab);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structmember_exp( e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e=
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1519,( struct _tagged_string)({
char* _temp1569=( char*)"_tag"; struct _tagged_string _temp1570; _temp1570.curr=
_temp1569; _temp1570.base= _temp1569; _temp1570.last_plus_one= _temp1569 + 5;
_temp1570;})), 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0),
Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} goto
_LL1439;} _LL1459: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1521);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1521); for( 0; rps != 0; rps= rps->tl,
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct
_tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1571=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1571->hd=( void*)({ struct _tuple14* _temp1572=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1572->f1= v; _temp1572->f2= Cyc_Toc_typ_to_c(
t2); _temp1572;}); _temp1571->tl= decls; _temp1571;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1573=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1573[ 0]= xprintf("f%d", cnt);
_temp1573;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1574=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1574[ 0]= xprintf("f%d", cnt); _temp1574;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1575->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1575->tl= ss; _temp1575;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1439;} _LL1461: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1523); for( 0; dlps != 0; dlps= dlps->tl){ struct _tuple15* tup=( struct
_tuple15*) dlps->hd; struct Cyc_Absyn_Pat* p2=(* tup).f2; struct _tagged_string*
f;{ void* _temp1576=( void*)((* tup).f1)->hd; struct _tagged_string* _temp1582;
_LL1578: if((( struct _enum_struct*) _temp1576)->tag == Cyc_Absyn_FieldName_tag){
_LL1583: _temp1582=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1576)->f1; goto _LL1579;} else{ goto _LL1580;} _LL1580: goto _LL1581;
_LL1579: f= _temp1582; goto _LL1577; _LL1581:( void) _throw(({ struct Cyc_Toc_Match_error_struct*
_temp1584=( struct Cyc_Toc_Match_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Toc_Match_error_struct));* _temp1584=( struct Cyc_Toc_Match_error_struct){.tag=
Cyc_Toc_Match_error_tag};( struct _xenum_struct*) _temp1584;})); _LL1577:;}{
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; void*
t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1585=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1585->hd=( void*)({ struct _tuple14*
_temp1586=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1586->f1=
v; _temp1586->f2= t2c; _temp1586;}); _temp1585->tl= decls; _temp1585;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path, f, 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp(
r, f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1587=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1587->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1587->tl= ss; _temp1587;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1439;} _LL1463: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( _temp1531->topt)->v; decls=({ struct Cyc_List_List* _temp1588=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1588->hd=(
void*)({ struct _tuple14* _temp1589=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp1589->f1= v; _temp1589->f2= Cyc_Toc_typ_to_c( t2);
_temp1589;}); _temp1588->tl= decls; _temp1588;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path, 0), _temp1531,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0), s2, 0); if( Cyc_Toc_really_is_nullable(
t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);} else{ s= s3;} goto
_LL1439;}}} _LL1465: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1590=( char*)"unknownid";
struct _tagged_string _temp1591; _temp1591.curr= _temp1590; _temp1591.base=
_temp1590; _temp1591.last_plus_one= _temp1590 + 10; _temp1591;})); goto _LL1439;
_LL1467: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1592=( char*)"unknowncall"; struct
_tagged_string _temp1593; _temp1593.curr= _temp1592; _temp1593.base= _temp1592;
_temp1593.last_plus_one= _temp1592 + 12; _temp1593;})); goto _LL1439; _LL1469: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1594=( char*)"unknownfields"; struct _tagged_string
_temp1595; _temp1595.curr= _temp1594; _temp1595.base= _temp1594; _temp1595.last_plus_one=
_temp1594 + 14; _temp1595;})); goto _LL1439; _LL1439:;} return({ struct _tuple13
_temp1596; _temp1596.f1= nv; _temp1596.f2= decls; _temp1596.f3= s; _temp1596;});}
static struct _tuple16* Cyc_Toc_gen_label( struct Cyc_Absyn_Switch_clause* sc){
return({ struct _tuple16* _temp1597=( struct _tuple16*) GC_malloc( sizeof(
struct _tuple16)); _temp1597->f1= Cyc_Toc_fresh_label(); _temp1597->f2= Cyc_Toc_fresh_label();
_temp1597->f3= sc; _temp1597;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
scs){ Cyc_Toc_exp_to_c( nv, e);{ void* t=( void*)( e->topt)->v; int
leave_as_switch;{ void* _temp1598= Cyc_Tcutil_compress( t); void* _temp1604;
void* _temp1606; _LL1600: if(( unsigned int) _temp1598 > 5u?(( struct
_enum_struct*) _temp1598)->tag == Cyc_Absyn_IntType_tag: 0){ _LL1607: _temp1606=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1598)->f1; goto _LL1605; _LL1605:
_temp1604=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1598)->f2; goto
_LL1601;} else{ goto _LL1602;} _LL1602: goto _LL1603; _LL1601: leave_as_switch=
1; goto _LL1599; _LL1603: leave_as_switch= 0; goto _LL1599; _LL1599:;}{ struct
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
succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1608=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1608->hd=( void*) res.f1;
_temp1608->tl= nvs; _temp1608;}); decls= res.f2; test_stmts=({ struct Cyc_List_List*
_temp1609=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1609->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*) lscs2->hd)).f1,
res.f3, 0); _temp1609->tl= test_stmts; _temp1609;});} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1610= sc->where_clause;
if( _temp1610 == 0){ _throw( Null_Exception);} _temp1610;}); struct
_tagged_string* intermed_lab= Cyc_Toc_fresh_label(); struct _tuple13 res= Cyc_Toc_xlate_pat(
nv, t, r, path, sc->pattern, intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c(
res.f1, e); nvs=({ struct Cyc_List_List* _temp1611=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1611->hd=( void*) res.f1;
_temp1611->tl= nvs; _temp1611;}); decls= res.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt(
e, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( intermed_lab, s2, 0);
test_stmts=({ struct Cyc_List_List* _temp1612=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1612->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*) lscs2->hd)).f1, Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0);
_temp1612->tl= test_stmts; _temp1612;});}}}}{ struct Cyc_Absyn_Stmt* test_stmt=
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
decls; decls != 0; decls= decls->tl){ struct _tuple14 _temp1615; void* _temp1616;
struct _tuple1* _temp1618; struct _tuple14* _temp1613=( struct _tuple14*) decls->hd;
_temp1615=* _temp1613; _LL1619: _temp1618= _temp1615.f1; goto _LL1617; _LL1617:
_temp1616= _temp1615.f2; goto _LL1614; _LL1614: res= Cyc_Absyn_declare_stmt(
_temp1618, _temp1616, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)( e->topt)->v),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1620=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1620[ 0]=({ struct Cyc_List_List _temp1621;
_temp1621.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0); _temp1621.tl=
0; _temp1621;}); _temp1620;}), 0), 0);} void Cyc_Toc_do_npop_before( int n,
struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}}
static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s){
while( 1) { void* _temp1622=( void*) s->r; struct Cyc_Absyn_Exp* _temp1660;
struct Cyc_Absyn_Stmt* _temp1662; struct Cyc_Absyn_Stmt* _temp1664; struct Cyc_Absyn_Exp*
_temp1666; struct Cyc_Absyn_Stmt* _temp1668; struct Cyc_Absyn_Stmt* _temp1670;
struct Cyc_Absyn_Exp* _temp1672; struct Cyc_Absyn_Stmt* _temp1674; struct
_tuple3 _temp1676; struct Cyc_Absyn_Stmt* _temp1678; struct Cyc_Absyn_Exp*
_temp1680; struct Cyc_Absyn_Stmt* _temp1682; struct Cyc_Absyn_Stmt* _temp1684;
struct Cyc_Absyn_Stmt* _temp1686; struct _tagged_string* _temp1688; struct Cyc_Absyn_Stmt*
_temp1690; struct _tuple3 _temp1692; struct Cyc_Absyn_Stmt* _temp1694; struct
Cyc_Absyn_Exp* _temp1696; struct _tuple3 _temp1698; struct Cyc_Absyn_Stmt*
_temp1700; struct Cyc_Absyn_Exp* _temp1702; struct Cyc_Absyn_Exp* _temp1704;
struct Cyc_List_List* _temp1706; struct Cyc_Absyn_Exp* _temp1708; struct Cyc_Absyn_Switch_clause**
_temp1710; struct Cyc_List_List* _temp1712; struct Cyc_Absyn_Stmt* _temp1714;
struct Cyc_Absyn_Decl* _temp1716; struct Cyc_Absyn_Stmt* _temp1718; struct Cyc_Absyn_Stmt*
_temp1720; struct Cyc_Absyn_Stmt* _temp1722; struct _tagged_string* _temp1724;
struct _tuple3 _temp1726; struct Cyc_Absyn_Stmt* _temp1728; struct Cyc_Absyn_Exp*
_temp1730; struct Cyc_Absyn_Stmt* _temp1732; struct Cyc_List_List* _temp1734;
struct Cyc_Absyn_Stmt* _temp1736; _LL1624: if( _temp1622 == Cyc_Absyn_Skip_s){
goto _LL1625;} else{ goto _LL1626;} _LL1626: if(( unsigned int) _temp1622 > 1u?((
struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1661:
_temp1660=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1622)->f1;
goto _LL1627;} else{ goto _LL1628;} _LL1628: if(( unsigned int) _temp1622 > 1u?((
struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1665:
_temp1664=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1622)->f1;
goto _LL1663; _LL1663: _temp1662=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1622)->f2; goto _LL1629;} else{ goto _LL1630;} _LL1630: if(( unsigned int)
_temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1667: _temp1666=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1622)->f1; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(( unsigned int)
_temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1673: _temp1672=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1622)->f1; goto _LL1671; _LL1671: _temp1670=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1622)->f2; goto _LL1669; _LL1669:
_temp1668=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1622)->f3; goto _LL1633;} else{ goto _LL1634;} _LL1634: if(( unsigned int)
_temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1677: _temp1676=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1622)->f1; _LL1681: _temp1680= _temp1676.f1; goto _LL1679; _LL1679:
_temp1678= _temp1676.f2; goto _LL1675; _LL1675: _temp1674=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1622)->f2; goto _LL1635;} else{ goto
_LL1636;} _LL1636: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1683: _temp1682=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1622)->f1; goto _LL1637;} else{ goto
_LL1638;} _LL1638: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1685: _temp1684=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1622)->f1; goto _LL1639;} else{ goto
_LL1640;} _LL1640: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1689: _temp1688=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1622)->f1; goto _LL1687;
_LL1687: _temp1686=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1622)->f2; goto _LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int)
_temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1705: _temp1704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1622)->f1; goto _LL1699; _LL1699: _temp1698=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1622)->f2; _LL1703: _temp1702= _temp1698.f1; goto _LL1701; _LL1701:
_temp1700= _temp1698.f2; goto _LL1693; _LL1693: _temp1692=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1622)->f3; _LL1697: _temp1696= _temp1692.f1;
goto _LL1695; _LL1695: _temp1694= _temp1692.f2; goto _LL1691; _LL1691: _temp1690=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1622)->f4; goto
_LL1643;} else{ goto _LL1644;} _LL1644: if(( unsigned int) _temp1622 > 1u?((
struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1709:
_temp1708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1622)->f1; goto _LL1707; _LL1707: _temp1706=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1622)->f2; goto _LL1645;} else{ goto
_LL1646;} _LL1646: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1713: _temp1712=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1622)->f1; goto _LL1711; _LL1711:
_temp1710=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1622)->f2; goto _LL1647;} else{ goto _LL1648;} _LL1648: if(( unsigned int)
_temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1717: _temp1716=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1622)->f1; goto _LL1715; _LL1715: _temp1714=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1622)->f2; goto _LL1649;} else{ goto
_LL1650;} _LL1650: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1719: _temp1718=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1622)->f1; goto _LL1651;} else{ goto
_LL1652;} _LL1652: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1721: _temp1720=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1622)->f1; goto _LL1653;} else{ goto
_LL1654;} _LL1654: if(( unsigned int) _temp1622 > 1u?(( struct _enum_struct*)
_temp1622)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1725: _temp1724=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1622)->f1; goto
_LL1723; _LL1723: _temp1722=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1622)->f2; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(( unsigned int)
_temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1733: _temp1732=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1622)->f1; goto _LL1727; _LL1727: _temp1726=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1622)->f2; _LL1731: _temp1730= _temp1726.f1; goto _LL1729; _LL1729:
_temp1728= _temp1726.f2; goto _LL1657;} else{ goto _LL1658;} _LL1658: if((
unsigned int) _temp1622 > 1u?(( struct _enum_struct*) _temp1622)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1737: _temp1736=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1622)->f1; goto _LL1735; _LL1735: _temp1734=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1622)->f2; goto _LL1659;} else{ goto
_LL1623;} _LL1625: return; _LL1627: Cyc_Toc_exp_to_c( nv, _temp1660); return;
_LL1629: Cyc_Toc_stmt_to_c( nv, _temp1664); s= _temp1662; continue; _LL1631: {
struct Cyc_Core_Opt* topt= 0; if( _temp1666 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1738=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1738->v=( void*) Cyc_Toc_typ_to_c(( void*)( _temp1666->topt)->v); _temp1738;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1739=
_temp1666; if( _temp1739 == 0){ _throw( Null_Exception);} _temp1739;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*) topt->v,
_temp1666, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth),
retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth, s);}}
return;} _LL1633: Cyc_Toc_exp_to_c( nv, _temp1672); Cyc_Toc_stmt_to_c( nv,
_temp1670); s= _temp1668; continue; _LL1635: Cyc_Toc_exp_to_c( nv, _temp1680);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1674); return; _LL1637: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)( nv->break_lab)->v, 0))->r));}{ int dest_depth= _temp1682 == 0?
0: _temp1682->try_depth; Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s);
return;} _LL1639: if( nv->continue_lab != 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_string*)( nv->continue_lab)->v, 0))->r));}
Cyc_Toc_do_npop_before( s->try_depth - _temp1684->try_depth, s); return; _LL1641:
Cyc_Toc_do_npop_before( s->try_depth - _temp1686->try_depth, s); return; _LL1643:
Cyc_Toc_exp_to_c( nv, _temp1704); Cyc_Toc_exp_to_c( nv, _temp1702); Cyc_Toc_exp_to_c(
nv, _temp1696); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1690); return;
_LL1645: Cyc_Toc_xlate_switch( nv, s, _temp1708, _temp1706); return; _LL1647:
if( nv->fallthru_info == 0){( void) _throw((( struct _xenum_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1740=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp1741; _temp1741.curr=
_temp1740; _temp1741.base= _temp1740; _temp1741.last_plus_one= _temp1740 + 29;
_temp1741;})));}{ struct _tuple8 _temp1744; struct Cyc_Dict_Dict* _temp1745;
struct Cyc_List_List* _temp1747; struct _tagged_string* _temp1749; struct
_tuple8* _temp1742=( struct _tuple8*)( nv->fallthru_info)->v; _temp1744=*
_temp1742; _LL1750: _temp1749= _temp1744.f1; goto _LL1748; _LL1748: _temp1747=
_temp1744.f2; goto _LL1746; _LL1746: _temp1745= _temp1744.f3; goto _LL1743;
_LL1743: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1749, 0); Cyc_Toc_do_npop_before(
s->try_depth -((* _temp1710)->body)->try_depth, s2);{ struct Cyc_List_List* vs2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1747);
struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp1712); for( 0; vs2 != 0; vs2= vs2->tl, es2= es2->tl){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) es2->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
_temp1745,( struct _tuple1*) vs2->hd),( struct Cyc_Absyn_Exp*) es2->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1649:{ void*
_temp1751=( void*) _temp1716->r; int _temp1757; struct Cyc_Absyn_Exp* _temp1759;
struct Cyc_Core_Opt* _temp1761; struct Cyc_Core_Opt* _temp1763; struct Cyc_Absyn_Pat*
_temp1765; _LL1753: if(( unsigned int) _temp1751 > 1u?(( struct _enum_struct*)
_temp1751)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1766: _temp1765=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1751)->f1; goto _LL1764; _LL1764: _temp1763=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1751)->f2; goto
_LL1762; _LL1762: _temp1761=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1751)->f3; goto _LL1760; _LL1760: _temp1759=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1751)->f4; goto _LL1758; _LL1758: _temp1757=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1751)->f5; goto _LL1754;} else{ goto
_LL1755;} _LL1755: goto _LL1756; _LL1754: { int var_case= 0;{ void* _temp1767=(
void*) _temp1765->r; struct Cyc_Absyn_Vardecl* _temp1773; _LL1769: if((
unsigned int) _temp1767 > 2u?(( struct _enum_struct*) _temp1767)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL1774: _temp1773=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1767)->f1; goto _LL1770;} else{ goto _LL1771;} _LL1771: goto _LL1772;
_LL1770: var_case= 1; _temp1773->initializer=( struct Cyc_Absyn_Exp*) _temp1759;(
void*)( _temp1716->r=( void*)({ struct Cyc_Absyn_Var_d_struct* _temp1775=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1775->tag= Cyc_Absyn_Var_d_tag; _temp1775->f1= _temp1773;( void*) _temp1775;}));
goto _LL1768; _LL1772: goto _LL1768; _LL1768:;} if( var_case){ goto _LL1756;}(
void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv, _temp1765, _temp1761,
_temp1759, _temp1757, _temp1714))->r)); goto _LL1752;} _LL1756: { struct Cyc_List_List*
_temp1779; struct Cyc_Toc_Env* _temp1781; struct _tuple17 _temp1777= Cyc_Toc_decls_to_c(
nv,({ struct Cyc_List_List* _temp1776=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1776->hd=( void*) _temp1716; _temp1776->tl=
0; _temp1776;}), 0); _LL1782: _temp1781= _temp1777.f1; goto _LL1780; _LL1780:
_temp1779= _temp1777.f2; goto _LL1778; _LL1778: Cyc_Toc_stmt_to_c( _temp1781,
_temp1714); for( 0; _temp1779 != 0; _temp1779= _temp1779->tl){ _temp1714= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*) _temp1779->hd, _temp1714, 0);}( void*)( s->r=( void*)((
void*) _temp1714->r)); goto _LL1752;} _LL1752:;} return; _LL1651:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1783=( char*)"cut"; struct _tagged_string _temp1784; _temp1784.curr=
_temp1783; _temp1784.base= _temp1783; _temp1784.last_plus_one= _temp1783 + 4;
_temp1784;})); return; _LL1653:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1785=( char*)"splice"; struct _tagged_string
_temp1786; _temp1786.curr= _temp1785; _temp1786.base= _temp1785; _temp1786.last_plus_one=
_temp1785 + 7; _temp1786;})); return; _LL1655: s= _temp1722; continue; _LL1657:
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1732); Cyc_Toc_exp_to_c( nv,
_temp1730); return; _LL1659: { struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); void*
h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp1787=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1787->f1= Cyc_Toc_abs_ns; _temp1787->f2= Cyc_Toc__push_handler_sp;
_temp1787;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1788=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1788->f1= Cyc_Toc_abs_ns; _temp1788->f2= Cyc_Toc_setjmp_sp; _temp1788;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1*
_temp1789=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1789->f1=
Cyc_Toc_abs_ns; _temp1789->f2= Cyc_Toc__pop_handler_sp; _temp1789;}), 0); e_exp->topt=({
struct Cyc_Core_Opt* _temp1790=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1790->v=( void*) e_typ; _temp1790;}); nv= Cyc_Toc_add_varmap(
nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1736);{ struct Cyc_Absyn_Stmt*
tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp1736, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl* x_vd= Cyc_Absyn_new_vardecl(
x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt* _temp1791=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1791->v=( void*)
Cyc_Absyn_exn_typ; _temp1791;});{ struct Cyc_Absyn_Pat* p=({ struct Cyc_Absyn_Pat*
_temp1792=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof( struct Cyc_Absyn_Pat));
_temp1792->r=( void*)({ struct Cyc_Absyn_Var_p_struct* _temp1794=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp1794->tag= Cyc_Absyn_Var_p_tag;
_temp1794->f1= x_vd;( void*) _temp1794;}); _temp1792->topt=({ struct Cyc_Core_Opt*
_temp1793=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1793->v=( void*) Cyc_Absyn_exn_typ; _temp1793;}); _temp1792->loc= 0;
_temp1792;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp1795=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1795->v=( void*) Cyc_Absyn_VoidType; _temp1795;});{ struct
Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp1796=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp1796->pattern= p;
_temp1796->pat_vars=({ struct Cyc_Core_Opt* _temp1797=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1797->v=( void*)({ struct Cyc_List_List*
_temp1798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1798->hd=( void*) x_vd; _temp1798->tl= 0; _temp1798;}); _temp1797;});
_temp1796->where_clause= 0; _temp1796->body= rs; _temp1796->loc= 0; _temp1796;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1734,({
struct Cyc_List_List* _temp1799=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1799->hd=( void*) default_case; _temp1799->tl= 0;
_temp1799;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp1800=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1800->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1800->tl= 0; _temp1800;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp1801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1801->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp1801->tl= 0;
_temp1801;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp( Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL1623:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args= args->tl){(*(( struct _tuple18*) args->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple18*) args->hd)).f3);{ struct _tuple1* x=({ struct _tuple1*
_temp1802=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1802->f1=
Cyc_Absyn_Loc_n; _temp1802->f2=(*(( struct _tuple18*) args->hd)).f1; _temp1802;});
nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp1803= s; _LL1805: if( _temp1803 == Cyc_Absyn_Abstract){ goto _LL1806;}
else{ goto _LL1807;} _LL1807: if( _temp1803 == Cyc_Absyn_ExternC){ goto _LL1808;}
else{ goto _LL1809;} _LL1809: goto _LL1810; _LL1806: return Cyc_Absyn_Public;
_LL1808: return Cyc_Absyn_Extern; _LL1810: return s; _LL1804:;} struct Cyc_Core_Opt*
Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c( struct Cyc_Absyn_Structdecl*
s){ struct _tuple1* n=( struct _tuple1*)( s->name)->v; if( Cyc_Toc_structs_so_far
== 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1811=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1811->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp1811;});}{ int seen_defn_before; struct Cyc_Core_Opt*
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
struct Cyc_Core_Opt* _temp1812=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1812->v=( void*) n; _temp1812;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)( s->fields)->v; for( 0; fields != 0; fields= fields->tl){( void*)(((
struct Cyc_Absyn_Structfield*) fields->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*) fields->hd)->type));}}}}} static struct
Cyc_Core_Opt* Cyc_Toc_enums_so_far= 0; static struct Cyc_List_List* Cyc_Toc_enumdecl_to_c(
struct Cyc_Absyn_Enumdecl* ed, struct Cyc_List_List* res){ struct _tuple1* n=(
struct _tuple1*)( ed->name)->v; if( Cyc_Toc_enums_so_far == 0){ Cyc_Toc_enums_so_far=({
struct Cyc_Core_Opt* _temp1813=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1813->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1813;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
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
_temp1814=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1814->name= n; _temp1814->tvs= 0; _temp1814->defn=( void*) Cyc_Absyn_void_star_typ();
_temp1814;}); res=({ struct Cyc_List_List* _temp1815=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1815->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1816=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1816->tag= Cyc_Absyn_Typedef_d_tag;
_temp1816->f1= tdef;( void*) _temp1816;}), 0); _temp1815->tl= res; _temp1815;});}{
void* t=({ struct Cyc_Absyn_TypedefType_struct* _temp1817=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1817->tag= Cyc_Absyn_TypedefType_tag;
_temp1817->f1= n; _temp1817->f2= 0; _temp1817->f3=({ struct Cyc_Core_Opt*
_temp1818=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1818->v=( void*) Cyc_Absyn_void_star_typ(); _temp1818;});( void*) _temp1817;});
int tag_count= 0; int box_count= 0;{ struct Cyc_List_List* fields= ed->fields ==
0? 0:( struct Cyc_List_List*)( ed->fields)->v; for( 0; fields != 0; fields=
fields->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fields->hd;
if( f->tag != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1819=( char*)"can't deal with enum tags yet";
struct _tagged_string _temp1820; _temp1820.curr= _temp1819; _temp1820.base=
_temp1819; _temp1820.last_plus_one= _temp1819 + 30; _temp1820;}));} if( f->typs
== 0){ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc == Cyc_Absyn_Extern? 0:(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Absyn_signed_int_exp((
tag_count ++), 0), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( f->name,
t, eopt);( void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({
struct Cyc_Absyn_Tqual* _temp1821=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1821->q_const= 1; _temp1821->q_volatile=
0; _temp1821->q_restrict= 0; _temp1821;}); res=({ struct Cyc_List_List*
_temp1822=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1822->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct*
_temp1823=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1823->tag= Cyc_Absyn_Var_d_tag; _temp1823->f1= vd;( void*) _temp1823;}), 0);
_temp1822->tl= res; _temp1822;});} else{ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc
== Cyc_Absyn_Extern? 0:( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
box_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1824=( char*)"_tag"; struct
_tagged_string _temp1825; _temp1825.curr= _temp1824; _temp1825.base= _temp1824;
_temp1825.last_plus_one= _temp1824 + 5; _temp1825;})), Cyc_Absyn_sint_t, eopt);(
void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({ struct
Cyc_Absyn_Tqual* _temp1826=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Tqual)); _temp1826->q_const= 1; _temp1826->q_volatile= 0;
_temp1826->q_restrict= 0; _temp1826;}); res=({ struct Cyc_List_List* _temp1827=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1827->hd=(
void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Var_d_struct* _temp1828=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1828->tag= Cyc_Absyn_Var_d_tag; _temp1828->f1= vd;( void*) _temp1828;}), 0);
_temp1827->tl= res; _temp1827;}); if( ! seen_defn_before){ struct Cyc_List_List*
fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl,
i ++){ struct _tagged_string* fname=( struct _tagged_string*)({ struct
_tagged_string* _temp1829=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1829[ 0]= xprintf("f%d", i); _temp1829;}); struct Cyc_Absyn_Structfield*
f=({ struct Cyc_Absyn_Structfield* _temp1830=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1830->name= fname;
_temp1830->tq=(*(( struct _tuple5*) ts->hd)).f1; _temp1830->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple5*) ts->hd)).f2); _temp1830->attributes= 0; _temp1830;}); fs=({
struct Cyc_List_List* _temp1831=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1831->hd=( void*) f; _temp1831->tl= fs; _temp1831;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);
fs=({ struct Cyc_List_List* _temp1832=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1832->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1833=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1833->name= Cyc_Toc_tag_sp; _temp1833->tq= Cyc_Toc_mt_tq; _temp1833->type=(
void*) Cyc_Absyn_sint_t; _temp1833->attributes= 0; _temp1833;}); _temp1832->tl=
fs; _temp1832;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp1834=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp1834->sc=( void*) Cyc_Absyn_Public; _temp1834->name=({ struct Cyc_Core_Opt*
_temp1836=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1836->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp1837=( char*)"_struct"; struct _tagged_string _temp1838; _temp1838.curr=
_temp1837; _temp1838.base= _temp1837; _temp1838.last_plus_one= _temp1837 + 8;
_temp1838;})); _temp1836;}); _temp1834->tvs= 0; _temp1834->fields=({ struct Cyc_Core_Opt*
_temp1835=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1835->v=( void*) fs; _temp1835;}); _temp1834->attributes= 0; _temp1834;});
res=({ struct Cyc_List_List* _temp1839=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1839->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp1840=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1840->tag= Cyc_Absyn_Struct_d_tag;
_temp1840->f1= sd;( void*) _temp1840;}), 0); _temp1839->tl= res; _temp1839;});}}}}}
return res;}}} static struct Cyc_Core_Opt* Cyc_Toc_xenums_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xenumdecl_to_c( struct Cyc_Absyn_Xenumdecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xenum_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp1841=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp1841->name= n; _temp1841->tvs= 0; _temp1841->defn=( void*) t; _temp1841;});
res=({ struct Cyc_List_List* _temp1842=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1842->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Typedef_d_struct* _temp1843=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp1843->tag= Cyc_Absyn_Typedef_d_tag;
_temp1843->f1= td;( void*) _temp1843;}), 0); _temp1842->tl= res; _temp1842;});}
else{ if( Cyc_Toc_xenums_so_far == 0){ Cyc_Toc_xenums_so_far=({ struct Cyc_Core_Opt*
_temp1844=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1844->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1844;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*) fs->hd; if( f->tag
!= 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp1845=( char*)"xenum:  can't deal with explicit tags";
struct _tagged_string _temp1846; _temp1846.curr= _temp1845; _temp1846.base=
_temp1845; _temp1846.last_plus_one= _temp1845 + 38; _temp1846;}));}{ struct
_tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp1847=* fn;( unsigned int)( _temp1847.last_plus_one -
_temp1847.curr);}), 0); void* tag_typ=({ struct Cyc_Absyn_ArrayType_struct*
_temp1848=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1848->tag= Cyc_Absyn_ArrayType_tag; _temp1848->f1=( void*) Cyc_Absyn_uchar_t;
_temp1848->f2= Cyc_Toc_mt_tq; _temp1848->f3= sz_exp;( void*) _temp1848;});
struct Cyc_Absyn_Exp* initopt= 0; if(( void*) xd->sc != Cyc_Absyn_Extern){
initopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_string_exp( 0,* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp1849=( char*)"_tag"; struct _tagged_string
_temp1850; _temp1850.curr= _temp1849; _temp1850.base= _temp1849; _temp1850.last_plus_one=
_temp1849 + 5; _temp1850;})), tag_typ, initopt);( void*)( tag_decl->sc=( void*)((
void*) xd->sc)); res=({ struct Cyc_List_List* _temp1851=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1851->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp1852=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1852->tag= Cyc_Absyn_Var_d_tag;
_temp1852->f1= tag_decl;( void*) _temp1852;}), 0); _temp1851->tl= res; _temp1851;});{
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
_temp1853=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1853[ 0]= xprintf("f%d", i); _temp1853;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp1854=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1854->name= field_n;
_temp1854->tq=(*(( struct _tuple5*) ts->hd)).f1; _temp1854->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple5*) ts->hd)).f2); _temp1854->attributes= 0; _temp1854;}); fields=({
struct Cyc_List_List* _temp1855=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1855->hd=( void*) newf; _temp1855->tl= fields;
_temp1855;});}} fields=({ struct Cyc_List_List* _temp1856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1856->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1857=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1857->name= Cyc_Toc_tag_sp; _temp1857->tq= Cyc_Toc_mt_tq; _temp1857->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp1857->attributes=
0; _temp1857;}); _temp1856->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields); _temp1856;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp1858=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1858->sc=( void*) Cyc_Absyn_Public;
_temp1858->name=({ struct Cyc_Core_Opt* _temp1860=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1860->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp1861=( char*)"_struct"; struct
_tagged_string _temp1862; _temp1862.curr= _temp1861; _temp1862.base= _temp1861;
_temp1862.last_plus_one= _temp1861 + 8; _temp1862;})); _temp1860;}); _temp1858->tvs=
0; _temp1858->fields=({ struct Cyc_Core_Opt* _temp1859=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1859->v=( void*) fields;
_temp1859;}); _temp1858->attributes= 0; _temp1858;}); res=({ struct Cyc_List_List*
_temp1863=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1863->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp1864=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1864->tag= Cyc_Absyn_Struct_d_tag; _temp1864->f1= strct_decl;( void*)
_temp1864;}), 0); _temp1863->tl= res; _temp1863;});}}}}}}}} return res;} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s){ void* t=( void*) topt->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp1867; struct Cyc_List_List* _temp1869; struct Cyc_Toc_Env* _temp1871;
struct _tuple13 _temp1865= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL1872: _temp1871=
_temp1865.f1; goto _LL1870; _LL1870: _temp1869= _temp1865.f2; goto _LL1868;
_LL1868: _temp1867= _temp1865.f3; goto _LL1866; _LL1866: Cyc_Toc_stmt_to_c(
_temp1871, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1867, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp1873=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1873->f1= Cyc_Toc_abs_ns; _temp1873->f2= Cyc_Toc_Match_Exception_sp;
_temp1873;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp1867, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp1869 != 0; _temp1869= _temp1869->tl){
struct _tuple14 _temp1876; void* _temp1877; struct _tuple1* _temp1879; struct
_tuple14* _temp1874=( struct _tuple14*) _temp1869->hd; _temp1876=* _temp1874;
_LL1880: _temp1879= _temp1876.f1; goto _LL1878; _LL1878: _temp1877= _temp1876.f2;
goto _LL1875; _LL1875: s= Cyc_Absyn_declare_stmt( _temp1879, _temp1877, 0, s, 0);}
return s;}}} static struct _tuple17 Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top){ struct Cyc_List_List* res= 0; for( 0; ds !=
0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) ds->hd; void*
_temp1881=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp1905; struct Cyc_Absyn_Fndecl*
_temp1907; int _temp1909; struct Cyc_Absyn_Exp* _temp1911; struct Cyc_Core_Opt*
_temp1913; struct Cyc_Core_Opt* _temp1915; struct Cyc_Absyn_Pat* _temp1917;
struct Cyc_Absyn_Structdecl* _temp1919; struct Cyc_Absyn_Enumdecl* _temp1921;
struct Cyc_Absyn_Xenumdecl* _temp1923; struct Cyc_Absyn_Typedefdecl* _temp1925;
struct Cyc_List_List* _temp1927; struct _tagged_string* _temp1929; struct Cyc_List_List*
_temp1931; struct _tuple1* _temp1933; struct Cyc_List_List* _temp1935; _LL1883:
if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*) _temp1881)->tag ==
Cyc_Absyn_Var_d_tag: 0){ _LL1906: _temp1905=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1881)->f1; goto _LL1884;} else{ goto
_LL1885;} _LL1885: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL1908: _temp1907=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1881)->f1; goto _LL1886;} else{ goto _LL1887;}
_LL1887: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*) _temp1881)->tag
== Cyc_Absyn_Let_d_tag: 0){ _LL1918: _temp1917=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp1881)->f1; goto _LL1916; _LL1916: _temp1915=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1881)->f2; goto
_LL1914; _LL1914: _temp1913=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1881)->f3; goto _LL1912; _LL1912: _temp1911=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1881)->f4; goto _LL1910; _LL1910: _temp1909=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1881)->f5; goto _LL1888;} else{ goto
_LL1889;} _LL1889: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL1920: _temp1919=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1881)->f1; goto _LL1890;} else{ goto
_LL1891;} _LL1891: if( _temp1881 == Cyc_Absyn_Union_d){ goto _LL1892;} else{
goto _LL1893;} _LL1893: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1922: _temp1921=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1881)->f1; goto _LL1894;} else{ goto
_LL1895;} _LL1895: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1924: _temp1923=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1881)->f1; goto _LL1896;} else{ goto
_LL1897;} _LL1897: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL1926: _temp1925=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1881)->f1; goto _LL1898;} else{ goto
_LL1899;} _LL1899: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1930: _temp1929=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1881)->f1; goto
_LL1928; _LL1928: _temp1927=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1881)->f2; goto _LL1900;} else{ goto _LL1901;} _LL1901: if(( unsigned int)
_temp1881 > 1u?(( struct _enum_struct*) _temp1881)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1934: _temp1933=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1881)->f1; goto _LL1932; _LL1932: _temp1931=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1881)->f2; goto _LL1902;} else{ goto
_LL1903;} _LL1903: if(( unsigned int) _temp1881 > 1u?(( struct _enum_struct*)
_temp1881)->tag == Cyc_Absyn_ExternC_d_tag: 0){ _LL1936: _temp1935=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1881)->f1; goto _LL1904;} else{ goto
_LL1882;} _LL1884: { struct _tuple1* c_name= _temp1905->name; if(( void*)
_temp1905->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1* _temp1937=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1937->f1= Cyc_Toc_abs_ns;
_temp1937->f2=(* c_name).f2; _temp1937;});} if( _temp1905->initializer != 0){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1938=
_temp1905->initializer; if( _temp1938 == 0){ _throw( Null_Exception);} _temp1938;}));}
if( nv->toplevel){ res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls, res); Cyc_Toc_temp_topdecls=
0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp1905->name, Cyc_Absyn_varb_exp(
c_name,({ struct Cyc_Absyn_Global_b_struct* _temp1939=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp1939->tag= Cyc_Absyn_Global_b_tag;
_temp1939->f1= _temp1905;( void*) _temp1939;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp1905->name, Cyc_Absyn_varb_exp( c_name,({ struct Cyc_Absyn_Local_b_struct*
_temp1940=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1940->tag= Cyc_Absyn_Local_b_tag; _temp1940->f1= _temp1905;( void*)
_temp1940;}), 0));} _temp1905->name= c_name;( void*)( _temp1905->sc=( void*) Cyc_Toc_scope_to_c((
void*) _temp1905->sc));( void*)( _temp1905->type=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1905->type)); res=({ struct Cyc_List_List* _temp1941=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1941->hd=( void*) d; _temp1941->tl=
res; _temp1941;}); goto _LL1882;} _LL1886: { struct _tuple1* c_name= _temp1907->name;
if(( void*) _temp1907->sc == Cyc_Absyn_ExternC){ c_name=({ struct _tuple1*
_temp1942=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1942->f1=
Cyc_Toc_abs_ns; _temp1942->f2=(* c_name).f2; _temp1942;});} nv= Cyc_Toc_add_varmap(
nv, _temp1907->name, Cyc_Absyn_var_exp( c_name, 0)); _temp1907->name= c_name;
Cyc_Toc_fndecl_to_c( nv, _temp1907); res=({ struct Cyc_List_List* _temp1943=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1943->hd=(
void*) d; _temp1943->tl= res; _temp1943;}); goto _LL1882;} _LL1888:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1944=( char*)"letdecl"; struct _tagged_string _temp1945; _temp1945.curr=
_temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one= _temp1944 + 8;
_temp1945;})); goto _LL1882; _LL1890: Cyc_Toc_structdecl_to_c( _temp1919); res=({
struct Cyc_List_List* _temp1946=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1946->hd=( void*) d; _temp1946->tl= res; _temp1946;});
goto _LL1882; _LL1892:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1947=( char*)"uniondecl"; struct
_tagged_string _temp1948; _temp1948.curr= _temp1947; _temp1948.base= _temp1947;
_temp1948.last_plus_one= _temp1947 + 10; _temp1948;})); goto _LL1882; _LL1894:
res= Cyc_Toc_enumdecl_to_c( _temp1921, res); goto _LL1882; _LL1896: res= Cyc_Toc_xenumdecl_to_c(
_temp1923, res); goto _LL1882; _LL1898: _temp1925->name= _temp1925->name;
_temp1925->tvs= 0;( void*)( _temp1925->defn=( void*) Cyc_Toc_typ_to_c_array((
void*) _temp1925->defn)); res=({ struct Cyc_List_List* _temp1949=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1949->hd=( void*) d; _temp1949->tl=
res; _temp1949;}); goto _LL1882; _LL1900: _temp1931= _temp1927; goto _LL1902;
_LL1902: _temp1935= _temp1931; goto _LL1904; _LL1904: { struct _tuple17 p= Cyc_Toc_decls_to_c(
nv, _temp1935, top); nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( p.f2), res); goto _LL1882;} _LL1882:;}
return({ struct _tuple17 _temp1950; _temp1950.f1= nv; _temp1950.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp1950;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_enums_so_far= 0; Cyc_Toc_xenums_so_far= 0;} struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List* _temp1953;
struct Cyc_Toc_Env* _temp1955; struct _tuple17 _temp1951= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL1956: _temp1955= _temp1951.f1; goto _LL1954;
_LL1954: _temp1953= _temp1951.f2; goto _LL1952; _LL1952: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp1953);}}