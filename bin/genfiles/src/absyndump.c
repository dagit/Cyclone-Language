#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Pat* f2; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
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
char* tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_string src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
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
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl** f3; }
; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl** f2; } ; extern const
int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag;
struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag; struct
Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void* Cyc_Absyn_FloatType;
extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; extern const int
Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ; extern void*
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
Cyc_Absyn_Structdecl* f1; } ; extern void* Cyc_Absyn_Union_d; extern const int
Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl*
f1; } ; extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
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
; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern int Cyc_Absyn_is_format_prim( void* p); extern struct _tagged_string
Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_string Cyc_Absynpp_char_escape( char); extern struct _tagged_string Cyc_Absynpp_string_escape(
struct _tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void*
p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual* tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved;
extern const int Cyc_Tcenv_VarRes_tag; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Tcenv_EnumRes_tag;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; extern const int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* enumdecls;
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
void* t); extern void Cyc_Absyndump_dumptyp( void*); extern void Cyc_Absyndump_dumpntyp(
void* t); extern void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp*); extern void
Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt*);
extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*); extern void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual*, void*, void(* f)( void*), void*); typedef struct
_tagged_string Cyc_Absyndump_dump_string_t; struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=&
Cyc_Stdio_stdout; void Cyc_Absyndump_ignore( void* x){ return;} static
unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp0= s;(
unsigned int)( _temp0.last_plus_one - _temp0.curr);}); if( !(( int)({ struct
_tagged_string _temp1= s; char* _temp3= _temp1.curr +( sz - 1); if( _temp3 <
_temp1.base? 1: _temp3 >= _temp1.last_plus_one){ _throw( Null_Exception);}*
_temp3;}))){ -- sz;} Cyc_Absyndump_pos +=( unsigned int)( sz + 1); if( Cyc_Absyndump_pos
>( unsigned int) 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc((
int)'\n',( struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} else{ Cyc_Stdio_fputc((
int)' ',( struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp4= s;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);}); if( !(( int)({ struct
_tagged_string _temp5= s; char* _temp7= _temp5.curr +( sz - 1); if( _temp7 <
_temp5.base? 1: _temp7 >= _temp5.last_plus_one){ _throw( Null_Exception);}*
_temp7;}))){ -- sz;} Cyc_Absyndump_pos +=( unsigned int) sz; Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char(
int c){ ++ Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,( struct Cyc_Stdio___sFILE*)*
Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str( struct _tagged_string* s){
Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char((
int)';');} void Cyc_Absyndump_dump_sep( void(* f)( void*), struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0; l->tl != 0; l= l->tl){
f(( void*) l->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*) l->hd);} void
Cyc_Absyndump_dump_sep_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0; l->tl != 0; l= l->tl){
f( env,( void*) l->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*) l->hd);}
void Cyc_Absyndump_group( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_group_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end, struct
_tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace( end);}
void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ if(
l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* nsl= 0;{ void* _temp8=(* v).f1; struct Cyc_List_List*
_temp20; struct Cyc_List_List* _temp22; struct Cyc_List_List* _temp24; struct
Cyc_List_List* _temp26; _LL10: if( _temp8 == Cyc_Absyn_Loc_n){ goto _LL11;}
else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 > 1u?(( struct _enum_struct*)
_temp8)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL21: _temp20=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp8)->f1; if( _temp20 == 0){ goto _LL13;}
else{ goto _LL14;}} else{ goto _LL14;} _LL14: if(( unsigned int) _temp8 > 1u?((
struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL23: _temp22=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp8)->f1; if(
_temp22 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto _LL16;} _LL16: if((
unsigned int) _temp8 > 1u?(( struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL25: _temp24=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp8)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp8 >
1u?(( struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL27:
_temp26=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp8)->f1;
goto _LL19;} else{ goto _LL9;} _LL11: goto _LL13; _LL13: goto _LL15; _LL15: Cyc_Absyndump_dump_str((*
v).f2); return; _LL17: _temp26= _temp24; goto _LL19; _LL19: nsl= _temp26; goto
_LL9; _LL9:;} Cyc_Absyndump_dump_str(( struct _tagged_string*) nsl->hd); for(
nsl= nsl->tl; nsl != 0; nsl= nsl->tl){ if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp28=( char*)"::"; struct _tagged_string _temp29; _temp29.curr= _temp28;
_temp29.base= _temp28; _temp29.last_plus_one= _temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*) nsl->hd));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp30=( char*)"_"; struct _tagged_string
_temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 2; _temp31;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp32=( char*)"::"; struct _tagged_string _temp33;
_temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one= _temp32 + 3;
_temp33;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual* tq){ if( tq->q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp34=( char*)"restrict"; struct _tagged_string
_temp35; _temp35.curr= _temp34; _temp35.base= _temp34; _temp35.last_plus_one=
_temp34 + 9; _temp35;}));} if( tq->q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp36=( char*)"volatile"; struct _tagged_string
_temp37; _temp37.curr= _temp36; _temp37.base= _temp36; _temp37.last_plus_one=
_temp36 + 9; _temp37;}));} if( tq->q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp38=( char*)"const"; struct _tagged_string _temp39;
_temp39.curr= _temp38; _temp39.base= _temp38; _temp39.last_plus_one= _temp38 + 6;
_temp39;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp40= sc; _LL42:
if( _temp40 == Cyc_Absyn_Static){ goto _LL43;} else{ goto _LL44;} _LL44: if(
_temp40 == Cyc_Absyn_Public){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp40
== Cyc_Absyn_Extern){ goto _LL47;} else{ goto _LL48;} _LL48: if( _temp40 == Cyc_Absyn_ExternC){
goto _LL49;} else{ goto _LL50;} _LL50: if( _temp40 == Cyc_Absyn_Abstract){ goto
_LL51;} else{ goto _LL41;} _LL43: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp52=( char*)"static"; struct _tagged_string _temp53; _temp53.curr=
_temp52; _temp53.base= _temp52; _temp53.last_plus_one= _temp52 + 7; _temp53;}));
return; _LL45: return; _LL47: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp54=( char*)"extern"; struct _tagged_string _temp55; _temp55.curr= _temp54;
_temp55.base= _temp54; _temp55.last_plus_one= _temp54 + 7; _temp55;})); return;
_LL49: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp56=( char*)"extern \"C\"";
struct _tagged_string _temp57; _temp57.curr= _temp56; _temp57.base= _temp56;
_temp57.last_plus_one= _temp56 + 11; _temp57;})); return; _LL51: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp58=( char*)"abstract"; struct _tagged_string
_temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 9; _temp59;})); return; _LL41:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp60= k; _LL62: if( _temp60 == Cyc_Absyn_AnyKind){ goto _LL63;} else{
goto _LL64;} _LL64: if( _temp60 == Cyc_Absyn_MemKind){ goto _LL65;} else{ goto
_LL66;} _LL66: if( _temp60 == Cyc_Absyn_BoxKind){ goto _LL67;} else{ goto _LL68;}
_LL68: if( _temp60 == Cyc_Absyn_RgnKind){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp60 == Cyc_Absyn_EffKind){ goto _LL71;} else{ goto _LL61;} _LL63: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp72=( char*)"A"; struct _tagged_string
_temp73; _temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one=
_temp72 + 2; _temp73;})); return; _LL65: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp74=( char*)"M"; struct _tagged_string _temp75;
_temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one= _temp74 + 2;
_temp75;})); return; _LL67: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp76=( char*)"B"; struct _tagged_string _temp77; _temp77.curr= _temp76;
_temp77.base= _temp76; _temp77.last_plus_one= _temp76 + 2; _temp77;})); return;
_LL69: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp78=( char*)"R";
struct _tagged_string _temp79; _temp79.curr= _temp78; _temp79.base= _temp78;
_temp79.last_plus_one= _temp78 + 2; _temp79;})); return; _LL71: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp80=( char*)"E"; struct _tagged_string
_temp81; _temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one=
_temp80 + 2; _temp81;})); return; _LL61:;} void Cyc_Absyndump_dumptps( struct
Cyc_List_List* ts){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp, ts,( struct _tagged_string)({
char* _temp82=( char*)"<"; struct _tagged_string _temp83; _temp83.curr= _temp82;
_temp83.base= _temp82; _temp83.last_plus_one= _temp82 + 2; _temp83;}),( struct
_tagged_string)({ char* _temp84=( char*)">"; struct _tagged_string _temp85;
_temp85.curr= _temp84; _temp85.base= _temp84; _temp85.last_plus_one= _temp84 + 2;
_temp85;}),( struct _tagged_string)({ char* _temp86=( char*)","; struct
_tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86; _temp87.last_plus_one=
_temp86 + 2; _temp87;}));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp88=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp96; void* _temp98; _LL90: if(( unsigned int) _temp88 >
1u?(( struct _enum_struct*) _temp88)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL97:
_temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp88)->f1; if( _temp96
== Cyc_Absyn_BoxKind){ goto _LL91;} else{ goto _LL92;}} else{ goto _LL92;} _LL92:
if(( unsigned int) _temp88 > 1u?(( struct _enum_struct*) _temp88)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL99: _temp98=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp88)->f1;
goto _LL93;} else{ goto _LL94;} _LL94: goto _LL95; _LL91: goto _LL89; _LL93: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp100=( char*)"::"; struct _tagged_string
_temp101; _temp101.curr= _temp100; _temp101.base= _temp100; _temp101.last_plus_one=
_temp100 + 3; _temp101;})); Cyc_Absyndump_dumpkind( _temp98); goto _LL89; _LL95:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp102=( char*)"::?";
struct _tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 4; _temp103;})); goto _LL89; _LL89:;}} void
Cyc_Absyndump_dumptvars( struct Cyc_List_List* tvs){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({ char* _temp104=( char*)"<";
struct _tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 2; _temp105;}),( struct _tagged_string)({
char* _temp106=( char*)">"; struct _tagged_string _temp107; _temp107.curr=
_temp106; _temp107.base= _temp106; _temp107.last_plus_one= _temp106 + 2;
_temp107;}),( struct _tagged_string)({ char* _temp108=( char*)","; struct
_tagged_string _temp109; _temp109.curr= _temp108; _temp109.base= _temp108;
_temp109.last_plus_one= _temp108 + 2; _temp109;}));} void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
tvs,( struct _tagged_string)({ char* _temp110=( char*)"<"; struct _tagged_string
_temp111; _temp111.curr= _temp110; _temp111.base= _temp110; _temp111.last_plus_one=
_temp110 + 2; _temp111;}),( struct _tagged_string)({ char* _temp112=( char*)">";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 2; _temp113;}),( struct _tagged_string)({
char* _temp114=( char*)","; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 2;
_temp115;}));} void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((
struct Cyc_Absyn_Tqual*)(* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts,( struct _tagged_string)({ char* _temp116=( char*)"("; struct _tagged_string
_temp117; _temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 2; _temp117;}),( struct _tagged_string)({ char* _temp118=( char*)")";
struct _tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}),( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp122=( char*)" __attribute__(("; struct _tagged_string _temp123; _temp123.curr=
_temp122; _temp123.base= _temp122; _temp123.last_plus_one= _temp122 + 17;
_temp123;})); for( 0; atts != 0; atts= atts->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*) atts->hd)); if( atts->tl != 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp124=( char*)","; struct _tagged_string _temp125;
_temp125.curr= _temp124; _temp125.base= _temp124; _temp125.last_plus_one=
_temp124 + 2; _temp125;}));}} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp126=( char*)")) "; struct _tagged_string _temp127; _temp127.curr= _temp126;
_temp127.base= _temp126; _temp127.last_plus_one= _temp126 + 4; _temp127;}));}
int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List* tms){ if( tms == 0){
return 0;}{ void* _temp128=( void*) tms->hd; struct Cyc_Absyn_Tqual* _temp134;
void* _temp136; void* _temp138; _LL130: if(( unsigned int) _temp128 > 1u?((
struct _enum_struct*) _temp128)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL139:
_temp138=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp128)->f1; goto
_LL137; _LL137: _temp136=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp128)->f2; goto _LL135; _LL135: _temp134=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp128)->f3; goto _LL131;} else{ goto _LL132;}
_LL132: goto _LL133; _LL131: return 1; _LL133: return 0; _LL129:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp140= Cyc_Tcutil_compress( t); _LL142:
if( _temp140 == Cyc_Absyn_HeapRgn){ goto _LL143;} else{ goto _LL144;} _LL144:
goto _LL145; _LL143: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp146=( char*)"`H"; struct _tagged_string _temp147; _temp147.curr= _temp146;
_temp147.base= _temp146; _temp147.last_plus_one= _temp146 + 3; _temp147;}));
goto _LL141; _LL145: Cyc_Absyndump_dumpntyp( t); goto _LL141; _LL141:;} static
struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp148= Cyc_Tcutil_compress(
t); void* _temp156; struct Cyc_List_List* _temp158; _LL150: if(( unsigned int)
_temp148 > 5u?(( struct _enum_struct*) _temp148)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL157: _temp156=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp148)->f1;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp148 > 5u?((
struct _enum_struct*) _temp148)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL159:
_temp158=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp148)->f1;
goto _LL153;} else{ goto _LL154;} _LL154: goto _LL155; _LL151: regions=({ struct
Cyc_List_List* _temp160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp160->hd=( void*) _temp156; _temp160->tl= regions; _temp160;}); goto _LL149;
_LL153: for( 0; _temp158 != 0; _temp158= _temp158->tl){ struct Cyc_List_List*
_temp163; struct Cyc_List_List* _temp165; struct _tuple6 _temp161= Cyc_Absyndump_effects_split((
void*) _temp158->hd); _LL166: _temp165= _temp161.f1; goto _LL164; _LL164:
_temp163= _temp161.f2; goto _LL162; _LL162: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp165, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp163, effects);} goto _LL149;
_LL155: effects=({ struct Cyc_List_List* _temp167=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp167->hd=( void*) t; _temp167->tl=
effects; _temp167;}); goto _LL149; _LL149:;} return({ struct _tuple6 _temp168;
_temp168.f1= regions; _temp168.f2= effects; _temp168;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp171; struct Cyc_List_List* _temp173; struct
_tuple6 _temp169= Cyc_Absyndump_effects_split( t); _LL174: _temp173= _temp169.f1;
goto _LL172; _LL172: _temp171= _temp169.f2; goto _LL170; _LL170: _temp173=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp173);
_temp171=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp171); for( 0; _temp171 != 0; _temp171= _temp171->tl){ Cyc_Absyndump_dumpntyp((
void*) _temp171->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp173 != 0; _temp173= _temp173->tl){ Cyc_Absyndump_dumprgn((
void*) _temp173->hd); if( _temp173->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}}
Cyc_Absyndump_dump_char(( int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void*
_temp175= t; struct Cyc_Absyn_Exp* _temp235; struct Cyc_Absyn_Tqual* _temp237;
void* _temp239; struct Cyc_Absyn_FnInfo _temp241; struct Cyc_Absyn_PtrInfo
_temp243; struct Cyc_Absyn_Tvar* _temp245; int _temp247; struct Cyc_Core_Opt*
_temp249; void* _temp251; int _temp253; struct Cyc_Core_Opt* _temp255; struct
Cyc_Core_Opt _temp257; void* _temp258; void* _temp260; struct Cyc_Absyn_Enumdecl**
_temp262; struct Cyc_List_List* _temp264; struct _tuple0* _temp266; struct Cyc_Absyn_Enumdecl**
_temp268; struct Cyc_List_List* _temp270; struct _tuple0* _temp272; struct Cyc_Absyn_Xenumdecl**
_temp274; struct _tuple0* _temp276; void* _temp278; void* _temp280; void*
_temp282; void* _temp284; void* _temp286; void* _temp288; void* _temp290; void*
_temp292; void* _temp294; void* _temp296; void* _temp298; void* _temp300; void*
_temp302; void* _temp304; void* _temp306; void* _temp308; struct Cyc_List_List*
_temp310; struct Cyc_Absyn_Structdecl** _temp312; struct Cyc_List_List* _temp314;
struct _tuple0* _temp316; struct Cyc_Absyn_Structdecl** _temp318; struct Cyc_List_List*
_temp320; struct _tuple0* _temp322; struct Cyc_Core_Opt* _temp324; struct Cyc_List_List*
_temp326; struct _tuple0* _temp328; void* _temp330; void* _temp332; struct Cyc_List_List*
_temp334; _LL177: if(( unsigned int) _temp175 > 5u?(( struct _enum_struct*)
_temp175)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL240: _temp239=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp175)->f1; goto _LL238; _LL238: _temp237=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp175)->f2;
goto _LL236; _LL236: _temp235=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f3; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp175 > 5u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_FnType_tag: 0){
_LL242: _temp241=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp175)->f1; goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int)
_temp175 > 5u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL244: _temp243=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp175)->f1; goto _LL182;} else{ goto _LL183;} _LL183: if( _temp175 == Cyc_Absyn_VoidType){
goto _LL184;} else{ goto _LL185;} _LL185: if(( unsigned int) _temp175 > 5u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_VarType_tag: 0){ _LL246:
_temp245=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp175)->f1;
goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp175 > 5u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_Evar_tag: 0){ _LL252: _temp251=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f1; goto _LL250; _LL250:
_temp249=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f2;
if( _temp249 == 0){ goto _LL248;} else{ goto _LL189;} _LL248: _temp247=( int)((
struct Cyc_Absyn_Evar_struct*) _temp175)->f3; goto _LL188;} else{ goto _LL189;}
_LL189: if(( unsigned int) _temp175 > 5u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL261: _temp260=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp175)->f1; goto _LL256; _LL256: _temp255=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp175)->f2; if( _temp255 == 0){ goto _LL191;} else{
_temp257=* _temp255; _LL259: _temp258=( void*) _temp257.v; goto _LL254;} _LL254:
_temp253=( int)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f3; goto _LL190;}
else{ goto _LL191;} _LL191: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL267: _temp266=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp175)->f1; if(
_temp266 == 0){ goto _LL265;} else{ goto _LL193;} _LL265: _temp264=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp175)->f2; goto _LL263; _LL263: _temp262=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp175)->f3;
goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int) _temp175 > 5u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL273:
_temp272=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp175)->f1;
goto _LL271; _LL271: _temp270=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp175)->f2; goto _LL269; _LL269: _temp268=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp175)->f3; goto _LL194;} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp175 > 5u?(( struct _enum_struct*)
_temp175)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL277: _temp276=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp175)->f1; goto _LL275; _LL275: _temp274=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp175)->f2;
goto _LL196;} else{ goto _LL197;} _LL197: if(( unsigned int) _temp175 > 5u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL281:
_temp280=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if(
_temp280 == Cyc_Absyn_Signed){ goto _LL279;} else{ goto _LL199;} _LL279:
_temp278=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if(
_temp278 == Cyc_Absyn_B1){ goto _LL198;} else{ goto _LL199;}} else{ goto _LL199;}
_LL199: if(( unsigned int) _temp175 > 5u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL285: _temp284=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f1; if( _temp284 == Cyc_Absyn_Unsigned){ goto _LL283;} else{ goto
_LL201;} _LL283: _temp282=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2;
if( _temp282 == Cyc_Absyn_B1){ goto _LL200;} else{ goto _LL201;}} else{ goto
_LL201;} _LL201: if(( unsigned int) _temp175 > 5u?(( struct _enum_struct*)
_temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL289: _temp288=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp288 == Cyc_Absyn_Signed){
goto _LL287;} else{ goto _LL203;} _LL287: _temp286=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp286 == Cyc_Absyn_B2){ goto _LL202;} else{ goto _LL203;}}
else{ goto _LL203;} _LL203: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL293: _temp292=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp292 == Cyc_Absyn_Unsigned){
goto _LL291;} else{ goto _LL205;} _LL291: _temp290=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp290 == Cyc_Absyn_B2){ goto _LL204;} else{ goto _LL205;}}
else{ goto _LL205;} _LL205: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL297: _temp296=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp296 == Cyc_Absyn_Signed){
goto _LL295;} else{ goto _LL207;} _LL295: _temp294=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp294 == Cyc_Absyn_B4){ goto _LL206;} else{ goto _LL207;}}
else{ goto _LL207;} _LL207: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL301: _temp300=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp300 == Cyc_Absyn_Unsigned){
goto _LL299;} else{ goto _LL209;} _LL299: _temp298=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp298 == Cyc_Absyn_B4){ goto _LL208;} else{ goto _LL209;}}
else{ goto _LL209;} _LL209: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL305: _temp304=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp304 == Cyc_Absyn_Signed){
goto _LL303;} else{ goto _LL211;} _LL303: _temp302=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp302 == Cyc_Absyn_B8){ goto _LL210;} else{ goto _LL211;}}
else{ goto _LL211;} _LL211: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL309: _temp308=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp308 == Cyc_Absyn_Unsigned){
goto _LL307;} else{ goto _LL213;} _LL307: _temp306=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp306 == Cyc_Absyn_B8){ goto _LL212;} else{ goto _LL213;}}
else{ goto _LL213;} _LL213: if( _temp175 == Cyc_Absyn_FloatType){ goto _LL214;}
else{ goto _LL215;} _LL215: if( _temp175 == Cyc_Absyn_DoubleType){ goto _LL216;}
else{ goto _LL217;} _LL217: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL311: _temp310=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp175)->f1; goto
_LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_StructType_tag: 0){ _LL317: _temp316=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f1; if(
_temp316 == 0){ goto _LL315;} else{ goto _LL221;} _LL315: _temp314=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp175)->f2; goto _LL313; _LL313:
_temp312=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp175)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp175 > 5u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_StructType_tag:
0){ _LL323: _temp322=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp175)->f1; goto _LL321; _LL321: _temp320=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp175)->f2; goto _LL319; _LL319: _temp318=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f3;
goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp175 > 5u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL329:
_temp328=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp175)->f1;
goto _LL327; _LL327: _temp326=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp175)->f2; goto _LL325; _LL325: _temp324=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp175)->f3; goto _LL224;} else{ goto _LL225;}
_LL225: if( _temp175 == Cyc_Absyn_UnionType){ goto _LL226;} else{ goto _LL227;}
_LL227: if(( unsigned int) _temp175 > 5u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL331: _temp330=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp175)->f1; goto _LL228;} else{ goto _LL229;} _LL229: if( _temp175 == Cyc_Absyn_HeapRgn){
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp175 > 5u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL333:
_temp332=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp175)->f1; goto
_LL232;} else{ goto _LL233;} _LL233: if(( unsigned int) _temp175 > 5u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL335: _temp334=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp175)->f1; goto
_LL234;} else{ goto _LL176;} _LL178: return; _LL180: return; _LL182: return;
_LL184: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp336=( char*)"void";
struct _tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 5; _temp337;})); return; _LL186: Cyc_Absyndump_dump_str(
_temp245->name); return; _LL188: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp338=( char*)"%"; struct _tagged_string _temp339; _temp339.curr=
_temp338; _temp339.base= _temp338; _temp339.last_plus_one= _temp338 + 2;
_temp339;})); Cyc_Absyndump_dumpkind( _temp251); Cyc_Absyndump_dump( xprintf("(%d)",
_temp247)); return; _LL190: Cyc_Absyndump_dumpntyp( _temp258); return; _LL192:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp340=( char*)"enum";
struct _tagged_string _temp341; _temp341.curr= _temp340; _temp341.base= _temp340;
_temp341.last_plus_one= _temp340 + 5; _temp341;})); Cyc_Absyndump_dumptps(
_temp264); return; _LL194: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp342=( char*)"enum"; struct _tagged_string _temp343; _temp343.curr= _temp342;
_temp343.base= _temp342; _temp343.last_plus_one= _temp342 + 5; _temp343;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp344= _temp272; if( _temp344 == 0){
_throw( Null_Exception);} _temp344;})); Cyc_Absyndump_dumptps( _temp270);
return; _LL196: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp345=(
char*)"xenum"; struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base=
_temp345; _temp346.last_plus_one= _temp345 + 6; _temp346;})); Cyc_Absyndump_dumpqvar(
_temp276); return; _LL198: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp347=( char*)"signed char"; struct _tagged_string _temp348; _temp348.curr=
_temp347; _temp348.base= _temp347; _temp348.last_plus_one= _temp347 + 12;
_temp348;})); return; _LL200: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp349=( char*)"char"; struct _tagged_string _temp350; _temp350.curr= _temp349;
_temp350.base= _temp349; _temp350.last_plus_one= _temp349 + 5; _temp350;}));
return; _LL202: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp351=(
char*)"short"; struct _tagged_string _temp352; _temp352.curr= _temp351; _temp352.base=
_temp351; _temp352.last_plus_one= _temp351 + 6; _temp352;})); return; _LL204:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp353=( char*)"unsigned short";
struct _tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 15; _temp354;})); return; _LL206: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp355=( char*)"int"; struct _tagged_string
_temp356; _temp356.curr= _temp355; _temp356.base= _temp355; _temp356.last_plus_one=
_temp355 + 4; _temp356;})); return; _LL208: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp357=( char*)"unsigned int"; struct _tagged_string
_temp358; _temp358.curr= _temp357; _temp358.base= _temp357; _temp358.last_plus_one=
_temp357 + 13; _temp358;})); return; _LL210: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp359=( char*)"long long"; struct _tagged_string
_temp360; _temp360.curr= _temp359; _temp360.base= _temp359; _temp360.last_plus_one=
_temp359 + 10; _temp360;})); return; _LL212: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp361=( char*)"unsigned long long"; struct
_tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 19; _temp362;})); return; _LL214: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp363=( char*)"float"; struct _tagged_string
_temp364; _temp364.curr= _temp363; _temp364.base= _temp363; _temp364.last_plus_one=
_temp363 + 6; _temp364;})); return; _LL216: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp365=( char*)"double"; struct _tagged_string
_temp366; _temp366.curr= _temp365; _temp366.base= _temp365; _temp366.last_plus_one=
_temp365 + 7; _temp366;})); return; _LL218: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp310); return; _LL220: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp367=( char*)"struct"; struct _tagged_string
_temp368; _temp368.curr= _temp367; _temp368.base= _temp367; _temp368.last_plus_one=
_temp367 + 7; _temp368;})); Cyc_Absyndump_dumptps( _temp314); return; _LL222:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp369=( char*)"struct";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 7; _temp370;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp371= _temp322; if( _temp371 == 0){
_throw( Null_Exception);} _temp371;})); Cyc_Absyndump_dumptps( _temp320);
return; _LL224: Cyc_Absyndump_dumpqvar( _temp328), Cyc_Absyndump_dumptps(
_temp326); return; _LL226: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp372=( char*)"union {/* unimplemented */}"; struct _tagged_string _temp373;
_temp373.curr= _temp372; _temp373.base= _temp372; _temp373.last_plus_one=
_temp372 + 28; _temp373;})); return; _LL228: Cyc_Absyndump_dumprgn( _temp330);
return; _LL230: return; _LL232: return; _LL234: return; _LL176:;} void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str(( struct
_tagged_string*) vo->v);}} void Cyc_Absyndump_dumpfunarg( struct _tuple1* t){((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)(* t).f2,(*
t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args= args->tl){ Cyc_Absyndump_dumpfunarg(( struct
_tuple1*) args->hd); if( args->tl != 0? 1: varargs){ Cyc_Absyndump_dump_char((
int)',');}} if( varargs){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp374=( char*)"..."; struct _tagged_string _temp375; _temp375.curr= _temp374;
_temp375.base= _temp374; _temp375.last_plus_one= _temp374 + 4; _temp375;}));}
if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)
effopt->v);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp(
void* t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(
0, t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp376= d; struct Cyc_Absyn_Exp* _temp382; struct
_tagged_string* _temp384; _LL378: if((( struct _enum_struct*) _temp376)->tag ==
Cyc_Absyn_ArrayElement_tag){ _LL383: _temp382=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp376)->f1; goto _LL379;} else{ goto _LL380;}
_LL380: if((( struct _enum_struct*) _temp376)->tag == Cyc_Absyn_FieldName_tag){
_LL385: _temp384=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp376)->f1; goto _LL381;} else{ goto _LL377;} _LL379: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp386=( char*)".["; struct _tagged_string
_temp387; _temp387.curr= _temp386; _temp387.base= _temp386; _temp387.last_plus_one=
_temp386 + 3; _temp387;})); Cyc_Absyndump_dumpexp( _temp382); Cyc_Absyndump_dump_char((
int)']'); goto _LL377; _LL381: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp384); goto _LL377; _LL377:;} void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp388=(
char*)""; struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base=
_temp388; _temp389.last_plus_one= _temp388 + 1; _temp389;}),( struct
_tagged_string)({ char* _temp390=( char*)"="; struct _tagged_string _temp391;
_temp391.curr= _temp390; _temp391.base= _temp390; _temp391.last_plus_one=
_temp390 + 2; _temp391;}),( struct _tagged_string)({ char* _temp392=( char*)"=";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 2; _temp393;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp394=(
char*)""; struct _tagged_string _temp395; _temp395.curr= _temp394; _temp395.base=
_temp394; _temp395.last_plus_one= _temp394 + 1; _temp395;}),( struct
_tagged_string)({ char* _temp396=( char*)""; struct _tagged_string _temp397;
_temp397.curr= _temp396; _temp397.base= _temp396; _temp397.last_plus_one=
_temp396 + 1; _temp397;}),( struct _tagged_string)({ char* _temp398=( char*)",";
struct _tagged_string _temp399; _temp399.curr= _temp398; _temp399.base= _temp398;
_temp399.last_plus_one= _temp398 + 2; _temp399;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp400=( char*)"("; struct _tagged_string _temp401; _temp401.curr= _temp400;
_temp401.base= _temp400; _temp401.last_plus_one= _temp400 + 2; _temp401;}));}{
void* _temp402=( void*) e->r; void* _temp490; char _temp492; void* _temp494;
void* _temp496; short _temp498; void* _temp500; void* _temp502; int _temp504;
void* _temp506; void* _temp508; int _temp510; void* _temp512; void* _temp514;
long long _temp516; void* _temp518; void* _temp520; struct _tagged_string
_temp522; void* _temp524; void* _temp526; struct _tagged_string _temp528; int
_temp530; struct _tuple0* _temp532; void* _temp534; struct _tuple0* _temp536;
struct Cyc_List_List* _temp538; void* _temp540; struct Cyc_Absyn_Exp* _temp542;
struct Cyc_Core_Opt* _temp544; struct Cyc_Absyn_Exp* _temp546; void* _temp548;
struct Cyc_Absyn_Exp* _temp550; void* _temp552; struct Cyc_Absyn_Exp* _temp554;
void* _temp556; struct Cyc_Absyn_Exp* _temp558; void* _temp560; struct Cyc_Absyn_Exp*
_temp562; struct Cyc_Absyn_Exp* _temp564; struct Cyc_Absyn_Exp* _temp566; struct
Cyc_Absyn_Exp* _temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp*
_temp572; struct Cyc_List_List* _temp574; struct Cyc_Absyn_Exp* _temp576; struct
Cyc_List_List* _temp578; struct Cyc_Absyn_Exp* _temp580; struct Cyc_Absyn_Exp*
_temp582; struct Cyc_Absyn_Exp* _temp584; struct Cyc_List_List* _temp586; struct
Cyc_Absyn_Exp* _temp588; struct Cyc_Absyn_Exp* _temp590; void* _temp592; struct
Cyc_Absyn_Exp* _temp594; void* _temp596; struct Cyc_Absyn_Exp* _temp598; struct
Cyc_Absyn_Exp* _temp600; struct _tagged_string* _temp602; struct Cyc_Absyn_Exp*
_temp604; struct _tagged_string* _temp606; struct Cyc_Absyn_Exp* _temp608;
struct Cyc_Absyn_Exp* _temp610; struct Cyc_Absyn_Exp* _temp612; struct Cyc_List_List*
_temp614; struct Cyc_List_List* _temp616; struct _tuple1* _temp618; struct Cyc_List_List*
_temp620; int _temp622; struct Cyc_Absyn_Exp* _temp624; struct Cyc_Absyn_Exp*
_temp626; struct Cyc_Absyn_Vardecl* _temp628; struct Cyc_Absyn_Structdecl*
_temp630; struct Cyc_List_List* _temp632; struct Cyc_Core_Opt* _temp634; struct
_tuple0* _temp636; struct Cyc_Absyn_Enumfield* _temp638; struct Cyc_Absyn_Enumdecl*
_temp640; struct Cyc_List_List* _temp642; struct Cyc_Core_Opt* _temp644; struct
Cyc_Core_Opt* _temp646; struct Cyc_Absyn_Enumfield* _temp648; struct Cyc_Absyn_Xenumdecl*
_temp650; struct Cyc_List_List* _temp652; struct Cyc_Core_Opt* _temp654; void*
_temp656; struct Cyc_List_List* _temp658; struct Cyc_Core_Opt* _temp660; struct
Cyc_Absyn_Stmt* _temp662; struct Cyc_Absyn_Fndecl* _temp664; struct Cyc_Absyn_Exp*
_temp666; _LL404: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){
_LL491: _temp490=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if((
unsigned int) _temp490 > 1u?(( struct _enum_struct*) _temp490)->tag == Cyc_Absyn_Char_c_tag:
0){ _LL495: _temp494=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp490)->f1;
goto _LL493; _LL493: _temp492=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp490)->f2; goto _LL405;} else{ goto _LL406;}} else{ goto _LL406;} _LL406:
if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){ _LL497:
_temp496=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if((
unsigned int) _temp496 > 1u?(( struct _enum_struct*) _temp496)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL501: _temp500=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp496)->f1;
goto _LL499; _LL499: _temp498=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp496)->f2; goto _LL407;} else{ goto _LL408;}} else{ goto _LL408;} _LL408:
if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){ _LL503:
_temp502=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if((
unsigned int) _temp502 > 1u?(( struct _enum_struct*) _temp502)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL507: _temp506=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp502)->f1;
if( _temp506 == Cyc_Absyn_Signed){ goto _LL505;} else{ goto _LL410;} _LL505:
_temp504=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp502)->f2; goto _LL409;}
else{ goto _LL410;}} else{ goto _LL410;} _LL410: if((( struct _enum_struct*)
_temp402)->tag == Cyc_Absyn_Const_e_tag){ _LL509: _temp508=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp402)->f1; if(( unsigned int) _temp508 > 1u?(( struct _enum_struct*)
_temp508)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL513: _temp512=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp508)->f1; if( _temp512 == Cyc_Absyn_Unsigned){
goto _LL511;} else{ goto _LL412;} _LL511: _temp510=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp508)->f2; goto _LL411;} else{ goto _LL412;}} else{ goto _LL412;} _LL412:
if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){ _LL515:
_temp514=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if((
unsigned int) _temp514 > 1u?(( struct _enum_struct*) _temp514)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL519: _temp518=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp514)->f1;
goto _LL517; _LL517: _temp516=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp514)->f2; goto _LL413;} else{ goto _LL414;}} else{ goto _LL414;} _LL414:
if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){ _LL521:
_temp520=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if((
unsigned int) _temp520 > 1u?(( struct _enum_struct*) _temp520)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL523: _temp522=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp520)->f1; goto _LL415;} else{ goto _LL416;}} else{ goto _LL416;} _LL416:
if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){ _LL525:
_temp524=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if(
_temp524 == Cyc_Absyn_Null_c){ goto _LL417;} else{ goto _LL418;}} else{ goto
_LL418;} _LL418: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Const_e_tag){
_LL527: _temp526=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp402)->f1; if((
unsigned int) _temp526 > 1u?(( struct _enum_struct*) _temp526)->tag == Cyc_Absyn_String_c_tag:
0){ _LL531: _temp530=( int)(( struct Cyc_Absyn_String_c_struct*) _temp526)->f1;
goto _LL529; _LL529: _temp528=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp526)->f2; goto _LL419;} else{ goto _LL420;}} else{ goto _LL420;} _LL420:
if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL533:
_temp532=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp402)->f1;
goto _LL421;} else{ goto _LL422;} _LL422: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Var_e_tag){ _LL537: _temp536=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp402)->f1; goto _LL535; _LL535: _temp534=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp402)->f2; goto _LL423;} else{ goto _LL424;} _LL424: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Primop_e_tag){ _LL541: _temp540=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp402)->f1; goto _LL539; _LL539:
_temp538=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp402)->f2;
goto _LL425;} else{ goto _LL426;} _LL426: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL547: _temp546=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp402)->f1; goto _LL545; _LL545: _temp544=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp402)->f2; goto
_LL543; _LL543: _temp542=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp402)->f3; goto _LL427;} else{ goto _LL428;} _LL428: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Increment_e_tag){ _LL551: _temp550=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp402)->f1;
goto _LL549; _LL549: _temp548=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp402)->f2; if( _temp548 == Cyc_Absyn_PreInc){ goto _LL429;} else{ goto
_LL430;}} else{ goto _LL430;} _LL430: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Increment_e_tag){ _LL555: _temp554=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp402)->f1; goto _LL553; _LL553:
_temp552=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp402)->f2; if(
_temp552 == Cyc_Absyn_PreDec){ goto _LL431;} else{ goto _LL432;}} else{ goto
_LL432;} _LL432: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Increment_e_tag){
_LL559: _temp558=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp402)->f1; goto _LL557; _LL557: _temp556=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp402)->f2; if( _temp556 == Cyc_Absyn_PostInc){ goto _LL433;} else{ goto
_LL434;}} else{ goto _LL434;} _LL434: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Increment_e_tag){ _LL563: _temp562=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp402)->f1; goto _LL561; _LL561:
_temp560=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp402)->f2; if(
_temp560 == Cyc_Absyn_PostDec){ goto _LL435;} else{ goto _LL436;}} else{ goto
_LL436;} _LL436: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Conditional_e_tag){
_LL569: _temp568=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp402)->f1; goto _LL567; _LL567: _temp566=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp402)->f2; goto _LL565; _LL565: _temp564=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp402)->f3;
goto _LL437;} else{ goto _LL438;} _LL438: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL573: _temp572=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp402)->f1; goto _LL571; _LL571: _temp570=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp402)->f2; goto
_LL439;} else{ goto _LL440;} _LL440: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL577: _temp576=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp402)->f1; goto _LL575; _LL575:
_temp574=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp402)->f2; goto _LL441;} else{ goto _LL442;} _LL442: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_FnCall_e_tag){ _LL581: _temp580=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp402)->f1; goto
_LL579; _LL579: _temp578=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp402)->f2; goto _LL443;} else{ goto _LL444;} _LL444: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Throw_e_tag){ _LL583: _temp582=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp402)->f1; goto
_LL445;} else{ goto _LL446;} _LL446: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL585: _temp584=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp402)->f1; goto _LL447;} else{
goto _LL448;} _LL448: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL589: _temp588=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp402)->f1; goto _LL587; _LL587: _temp586=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp402)->f2; goto _LL449;} else{ goto _LL450;}
_LL450: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Cast_e_tag){
_LL593: _temp592=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp402)->f1; goto
_LL591; _LL591: _temp590=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp402)->f2; goto _LL451;} else{ goto _LL452;} _LL452: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Address_e_tag){ _LL595: _temp594=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp402)->f1; goto
_LL453;} else{ goto _LL454;} _LL454: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Sizeoftyp_e_tag){ _LL597: _temp596=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp402)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL599: _temp598=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp402)->f1;
goto _LL457;} else{ goto _LL458;} _LL458: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Deref_e_tag){ _LL601: _temp600=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp402)->f1; goto _LL459;} else{ goto _LL460;}
_LL460: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_StructMember_e_tag){
_LL605: _temp604=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp402)->f1; goto _LL603; _LL603: _temp602=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp402)->f2; goto _LL461;} else{ goto _LL462;}
_LL462: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL609: _temp608=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp402)->f1; goto _LL607; _LL607: _temp606=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp402)->f2; goto _LL463;} else{ goto _LL464;}
_LL464: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Subscript_e_tag){
_LL613: _temp612=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp402)->f1; goto _LL611; _LL611: _temp610=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp402)->f2; goto _LL465;} else{ goto _LL466;}
_LL466: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Tuple_e_tag){
_LL615: _temp614=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp402)->f1; goto _LL467;} else{ goto _LL468;} _LL468: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL619: _temp618=(
struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp402)->f1; goto
_LL617; _LL617: _temp616=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp402)->f2; goto _LL469;} else{ goto _LL470;} _LL470: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Array_e_tag){ _LL623: _temp622=( int)((
struct Cyc_Absyn_Array_e_struct*) _temp402)->f1; goto _LL621; _LL621: _temp620=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp402)->f2; goto
_LL471;} else{ goto _LL472;} _LL472: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL629: _temp628=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp402)->f1; goto _LL627; _LL627:
_temp626=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp402)->f2; goto _LL625; _LL625: _temp624=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp402)->f3; goto _LL473;} else{ goto
_LL474;} _LL474: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_Struct_e_tag){
_LL637: _temp636=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp402)->f1; goto _LL635; _LL635: _temp634=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp402)->f2; goto _LL633; _LL633: _temp632=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp402)->f3; goto
_LL631; _LL631: _temp630=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp402)->f4; goto _LL475;} else{ goto _LL476;} _LL476: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Enum_e_tag){ _LL647: _temp646=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp402)->f1; goto
_LL645; _LL645: _temp644=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp402)->f2; goto _LL643; _LL643: _temp642=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp402)->f3; goto _LL641; _LL641: _temp640=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp402)->f4; goto
_LL639; _LL639: _temp638=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp402)->f5; goto _LL477;} else{ goto _LL478;} _LL478: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Xenum_e_tag){ _LL655: _temp654=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp402)->f1; goto
_LL653; _LL653: _temp652=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp402)->f2; goto _LL651; _LL651: _temp650=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp402)->f3; goto _LL649; _LL649: _temp648=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp402)->f4;
goto _LL479;} else{ goto _LL480;} _LL480: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL657: _temp656=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp402)->f1; goto _LL481;} else{ goto _LL482;} _LL482: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL661:
_temp660=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp402)->f1; goto _LL659; _LL659: _temp658=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp402)->f2; goto _LL483;} else{ goto
_LL484;} _LL484: if((( struct _enum_struct*) _temp402)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL663: _temp662=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp402)->f1; goto _LL485;} else{ goto _LL486;} _LL486: if((( struct
_enum_struct*) _temp402)->tag == Cyc_Absyn_Codegen_e_tag){ _LL665: _temp664=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp402)->f1;
goto _LL487;} else{ goto _LL488;} _LL488: if((( struct _enum_struct*) _temp402)->tag
== Cyc_Absyn_Fill_e_tag){ _LL667: _temp666=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp402)->f1; goto _LL489;} else{ goto _LL403;} _LL405: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp492)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL403; _LL407: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp668=( char*)"(short)"; struct _tagged_string _temp669; _temp669.curr=
_temp668; _temp669.base= _temp668; _temp669.last_plus_one= _temp668 + 8;
_temp669;})); Cyc_Absyndump_dump_nospace( Cyc_Core_string_of_int(( int) _temp498));
goto _LL403; _LL409: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp504));
goto _LL403; _LL411: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp510)); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp670=( char*)"u"; struct _tagged_string
_temp671; _temp671.curr= _temp670; _temp671.base= _temp670; _temp671.last_plus_one=
_temp670 + 2; _temp671;})); goto _LL403; _LL413: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp672=( char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 27; _temp673;})); goto _LL403; _LL415: Cyc_Absyndump_dump(
_temp522); goto _LL403; _LL417: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp674=( char*)"null"; struct _tagged_string _temp675; _temp675.curr=
_temp674; _temp675.base= _temp674; _temp675.last_plus_one= _temp674 + 5;
_temp675;})); goto _LL403; _LL419: if( _temp530){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp676=( char*)"new "; struct _tagged_string _temp677;
_temp677.curr= _temp676; _temp677.base= _temp676; _temp677.last_plus_one=
_temp676 + 5; _temp677;}));} Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp528)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL403; _LL421: _temp536= _temp532; goto _LL423; _LL423: Cyc_Absyndump_dumpqvar(
_temp536); goto _LL403; _LL425: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp540); if( ! Cyc_Absyn_is_format_prim( _temp540)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp538)){ case 1: _LL678: if( _temp540 ==
Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp538->hd); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp680=(
char*)".size"; struct _tagged_string _temp681; _temp681.curr= _temp680; _temp681.base=
_temp680; _temp681.last_plus_one= _temp680 + 6; _temp681;}));} else{ Cyc_Absyndump_dump(
pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*) _temp538->hd);}
break; case 2: _LL679: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp538->hd); Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,(
struct Cyc_Absyn_Exp*)( _temp538->tl)->hd); break; default: _LL682:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp684=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp684=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp685=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 47; _temp686;})};( struct _xenum_struct*)
_temp684;}));}} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp687=( char*)"("; struct _tagged_string
_temp688; _temp688.curr= _temp687; _temp688.base= _temp687; _temp688.last_plus_one=
_temp687 + 2; _temp688;})); Cyc_Absyndump_dumpexps_prec( 20, _temp538); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp689=( char*)")"; struct _tagged_string
_temp690; _temp690.curr= _temp689; _temp690.base= _temp689; _temp690.last_plus_one=
_temp689 + 2; _temp690;}));} goto _LL403;} _LL427: Cyc_Absyndump_dumpexp_prec(
myprec, _temp546); if( _temp544 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*) _temp544->v));} Cyc_Absyndump_dump_nospace(( struct _tagged_string)({
char* _temp691=( char*)"="; struct _tagged_string _temp692; _temp692.curr=
_temp691; _temp692.base= _temp691; _temp692.last_plus_one= _temp691 + 2;
_temp692;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp542); goto _LL403; _LL429:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp693=( char*)"++";
struct _tagged_string _temp694; _temp694.curr= _temp693; _temp694.base= _temp693;
_temp694.last_plus_one= _temp693 + 3; _temp694;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp550); goto _LL403; _LL431: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp695=( char*)"--"; struct _tagged_string _temp696;
_temp696.curr= _temp695; _temp696.base= _temp695; _temp696.last_plus_one=
_temp695 + 3; _temp696;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp554); goto
_LL403; _LL433: Cyc_Absyndump_dumpexp_prec( myprec, _temp558); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp697=( char*)"++"; struct _tagged_string
_temp698; _temp698.curr= _temp697; _temp698.base= _temp697; _temp698.last_plus_one=
_temp697 + 3; _temp698;})); goto _LL403; _LL435: Cyc_Absyndump_dumpexp_prec(
myprec, _temp562); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp699=(
char*)"--"; struct _tagged_string _temp700; _temp700.curr= _temp699; _temp700.base=
_temp699; _temp700.last_plus_one= _temp699 + 3; _temp700;})); goto _LL403;
_LL437: Cyc_Absyndump_dumpexp_prec( myprec, _temp568); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp566); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp564); goto _LL403; _LL439:
Cyc_Absyndump_dumpexp_prec( myprec, _temp572); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp570); goto _LL403; _LL441: _temp580=
_temp576; _temp578= _temp574; goto _LL443; _LL443: Cyc_Absyndump_dumpexp_prec(
myprec, _temp580); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp701=( char*)"("; struct _tagged_string _temp702; _temp702.curr= _temp701;
_temp702.base= _temp701; _temp702.last_plus_one= _temp701 + 2; _temp702;})); Cyc_Absyndump_dumpexps_prec(
20, _temp578); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp703=( char*)")"; struct _tagged_string _temp704; _temp704.curr= _temp703;
_temp704.base= _temp703; _temp704.last_plus_one= _temp703 + 2; _temp704;}));
goto _LL403; _LL445: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp705=( char*)"throw"; struct _tagged_string _temp706; _temp706.curr=
_temp705; _temp706.base= _temp705; _temp706.last_plus_one= _temp705 + 6;
_temp706;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp582); goto _LL403; _LL447:
_temp588= _temp584; goto _LL449; _LL449: Cyc_Absyndump_dumpexp_prec( inprec,
_temp588); goto _LL403; _LL451: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp592); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp590); goto _LL403; _LL453: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp594); goto _LL403; _LL455: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp707=( char*)"sizeof("; struct _tagged_string
_temp708; _temp708.curr= _temp707; _temp708.base= _temp707; _temp708.last_plus_one=
_temp707 + 8; _temp708;})); Cyc_Absyndump_dumptyp( _temp596); Cyc_Absyndump_dump_char((
int)')'); goto _LL403; _LL457: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp709=( char*)"sizeof("; struct _tagged_string _temp710; _temp710.curr=
_temp709; _temp710.base= _temp709; _temp710.last_plus_one= _temp709 + 8;
_temp710;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp598); Cyc_Absyndump_dump_char((
int)')'); goto _LL403; _LL459: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp600); goto _LL403; _LL461: Cyc_Absyndump_dumpexp_prec( myprec,
_temp604); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp602); goto _LL403; _LL463: Cyc_Absyndump_dumpexp_prec( myprec, _temp608);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp711=( char*)"->";
struct _tagged_string _temp712; _temp712.curr= _temp711; _temp712.base= _temp711;
_temp712.last_plus_one= _temp711 + 3; _temp712;})); Cyc_Absyndump_dump_nospace(*
_temp606); goto _LL403; _LL465: Cyc_Absyndump_dumpexp_prec( myprec, _temp612);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp610); Cyc_Absyndump_dump_char((
int)']'); goto _LL403; _LL467: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp713=( char*)"$("; struct _tagged_string _temp714; _temp714.curr=
_temp713; _temp714.base= _temp713; _temp714.last_plus_one= _temp713 + 3;
_temp714;})); Cyc_Absyndump_dumpexps_prec( 20, _temp614); Cyc_Absyndump_dump_char((
int)')'); goto _LL403; _LL469: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp618).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp616,( struct _tagged_string)({ char* _temp715=( char*)"{"; struct
_tagged_string _temp716; _temp716.curr= _temp715; _temp716.base= _temp715;
_temp716.last_plus_one= _temp715 + 2; _temp716;}),( struct _tagged_string)({
char* _temp717=( char*)"}"; struct _tagged_string _temp718; _temp718.curr=
_temp717; _temp718.base= _temp717; _temp718.last_plus_one= _temp717 + 2;
_temp718;}),( struct _tagged_string)({ char* _temp719=( char*)","; struct
_tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 2; _temp720;})); goto _LL403; _LL471:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp620,( struct _tagged_string)({ char* _temp721=( char*)"new {";
struct _tagged_string _temp722; _temp722.curr= _temp721; _temp722.base= _temp721;
_temp722.last_plus_one= _temp721 + 6; _temp722;}),( struct _tagged_string)({
char* _temp723=( char*)"}"; struct _tagged_string _temp724; _temp724.curr=
_temp723; _temp724.base= _temp723; _temp724.last_plus_one= _temp723 + 2;
_temp724;}),( struct _tagged_string)({ char* _temp725=( char*)","; struct
_tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 2; _temp726;})); goto _LL403; _LL473: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp727=( char*)"new {for"; struct
_tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 9; _temp728;})); Cyc_Absyndump_dump_str((*
_temp628->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp626); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp624);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL403; _LL475: Cyc_Absyndump_dumpqvar(
_temp636);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp632,( struct
_tagged_string)({ char* _temp729=( char*)"{"; struct _tagged_string _temp730;
_temp730.curr= _temp729; _temp730.base= _temp729; _temp730.last_plus_one=
_temp729 + 2; _temp730;}),( struct _tagged_string)({ char* _temp731=( char*)"}";
struct _tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 2; _temp732;}),( struct _tagged_string)({
char* _temp733=( char*)","; struct _tagged_string _temp734; _temp734.curr=
_temp733; _temp734.base= _temp733; _temp734.last_plus_one= _temp733 + 2;
_temp734;})); goto _LL403; _LL477: _temp652= _temp642; _temp648= _temp638; goto
_LL479; _LL479: Cyc_Absyndump_dumpqvar( _temp648->name); if( _temp652 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp652,( struct _tagged_string)({ char* _temp735=( char*)"(";
struct _tagged_string _temp736; _temp736.curr= _temp735; _temp736.base= _temp735;
_temp736.last_plus_one= _temp735 + 2; _temp736;}),( struct _tagged_string)({
char* _temp737=( char*)")"; struct _tagged_string _temp738; _temp738.curr=
_temp737; _temp738.base= _temp737; _temp738.last_plus_one= _temp737 + 2;
_temp738;}),( struct _tagged_string)({ char* _temp739=( char*)","; struct
_tagged_string _temp740; _temp740.curr= _temp739; _temp740.base= _temp739;
_temp740.last_plus_one= _temp739 + 2; _temp740;}));} goto _LL403; _LL481: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp741=( char*)"malloc(sizeof("; struct
_tagged_string _temp742; _temp742.curr= _temp741; _temp742.base= _temp741;
_temp742.last_plus_one= _temp741 + 15; _temp742;}));{ void* _temp743= _temp656;
void* _temp753; struct Cyc_Absyn_Enumfield* _temp755; struct Cyc_Absyn_Enumdecl*
_temp757; struct Cyc_Absyn_Enumfield* _temp759; struct Cyc_Absyn_Xenumdecl*
_temp761; struct _tuple0* _temp763; _LL745: if((( struct _enum_struct*) _temp743)->tag
== Cyc_Absyn_Typ_m_tag){ _LL754: _temp753=( void*)(( struct Cyc_Absyn_Typ_m_struct*)
_temp743)->f1; goto _LL746;} else{ goto _LL747;} _LL747: if((( struct
_enum_struct*) _temp743)->tag == Cyc_Absyn_Enum_m_tag){ _LL758: _temp757=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_m_struct*) _temp743)->f1;
goto _LL756; _LL756: _temp755=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_m_struct*)
_temp743)->f2; goto _LL748;} else{ goto _LL749;} _LL749: if((( struct
_enum_struct*) _temp743)->tag == Cyc_Absyn_Xenum_m_tag){ _LL762: _temp761=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_m_struct*) _temp743)->f1;
goto _LL760; _LL760: _temp759=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_m_struct*)
_temp743)->f2; goto _LL750;} else{ goto _LL751;} _LL751: if((( struct
_enum_struct*) _temp743)->tag == Cyc_Absyn_Unresolved_m_tag){ _LL764: _temp763=(
struct _tuple0*)(( struct Cyc_Absyn_Unresolved_m_struct*) _temp743)->f1; goto
_LL752;} else{ goto _LL744;} _LL746: Cyc_Absyndump_dumptyp( _temp753); goto
_LL744; _LL748: _temp759= _temp755; goto _LL750; _LL750: _temp763= _temp759->name;
goto _LL752; _LL752: Cyc_Absyndump_dumpqvar( _temp763); goto _LL744; _LL744:;}
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp765=( char*)"))";
struct _tagged_string _temp766; _temp766.curr= _temp765; _temp766.base= _temp765;
_temp766.last_plus_one= _temp765 + 3; _temp766;})); goto _LL403; _LL483:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp658,( struct _tagged_string)({ char* _temp767=( char*)"{";
struct _tagged_string _temp768; _temp768.curr= _temp767; _temp768.base= _temp767;
_temp768.last_plus_one= _temp767 + 2; _temp768;}),( struct _tagged_string)({
char* _temp769=( char*)"}"; struct _tagged_string _temp770; _temp770.curr=
_temp769; _temp770.base= _temp769; _temp770.last_plus_one= _temp769 + 2;
_temp770;}),( struct _tagged_string)({ char* _temp771=( char*)","; struct
_tagged_string _temp772; _temp772.curr= _temp771; _temp772.base= _temp771;
_temp772.last_plus_one= _temp771 + 2; _temp772;})); goto _LL403; _LL485: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp773=( char*)"({"; struct _tagged_string
_temp774; _temp774.curr= _temp773; _temp774.base= _temp773; _temp774.last_plus_one=
_temp773 + 3; _temp774;})); Cyc_Absyndump_dumpstmt( _temp662); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp775=( char*)"})"; struct _tagged_string
_temp776; _temp776.curr= _temp775; _temp776.base= _temp775; _temp776.last_plus_one=
_temp775 + 3; _temp776;})); goto _LL403; _LL487: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp777=( char*)"codegen("; struct _tagged_string
_temp778; _temp778.curr= _temp777; _temp778.base= _temp777; _temp778.last_plus_one=
_temp777 + 9; _temp778;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp779=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp779->r=( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp780=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp780->tag= Cyc_Absyn_Fn_d_tag;
_temp780->f1= _temp664;( void*) _temp780;}); _temp779->loc= e->loc; _temp779;}));
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp781=( char*)")"; struct
_tagged_string _temp782; _temp782.curr= _temp781; _temp782.base= _temp781;
_temp782.last_plus_one= _temp781 + 2; _temp782;})); goto _LL403; _LL489: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp783=( char*)"fill("; struct _tagged_string
_temp784; _temp784.curr= _temp783; _temp784.base= _temp783; _temp784.last_plus_one=
_temp783 + 6; _temp784;})); Cyc_Absyndump_dumpexp( _temp666); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp785=( char*)")"; struct _tagged_string
_temp786; _temp786.curr= _temp785; _temp786.base= _temp785; _temp786.last_plus_one=
_temp785 + 2; _temp786;})); goto _LL403; _LL403:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause* c=( struct Cyc_Absyn_Switch_clause*)
scs->hd; if( c->where_clause == 0?( void*)( c->pattern)->r == Cyc_Absyn_Wild_p:
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp787=( char*)"default:";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 9; _temp788;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp789=( char*)"case"; struct _tagged_string
_temp790; _temp790.curr= _temp789; _temp790.base= _temp789; _temp790.last_plus_one=
_temp789 + 5; _temp790;})); Cyc_Absyndump_dumppat( c->pattern); if( c->where_clause
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp791=( char*)"&&";
struct _tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 3; _temp792;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp793= c->where_clause; if(
_temp793 == 0){ _throw( Null_Exception);} _temp793;}));} Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp794=( char*)":"; struct _tagged_string
_temp795; _temp795.curr= _temp794; _temp795.base= _temp794; _temp795.last_plus_one=
_temp794 + 2; _temp795;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp796=( void*) s->r; struct Cyc_Absyn_Exp*
_temp838; struct Cyc_Absyn_Stmt* _temp840; struct Cyc_Absyn_Stmt* _temp842;
struct Cyc_Absyn_Exp* _temp844; struct Cyc_Absyn_Exp* _temp846; struct Cyc_Absyn_Stmt*
_temp848; struct Cyc_Absyn_Stmt* _temp850; struct Cyc_Absyn_Exp* _temp852;
struct Cyc_Absyn_Stmt* _temp854; struct _tuple2 _temp856; struct Cyc_Absyn_Stmt*
_temp858; struct Cyc_Absyn_Exp* _temp860; struct Cyc_Absyn_Stmt* _temp862;
struct Cyc_Absyn_Stmt* _temp864; struct Cyc_Absyn_Stmt* _temp866; struct
_tagged_string* _temp868; struct Cyc_Absyn_Switch_clause** _temp870; struct Cyc_List_List*
_temp872; struct Cyc_Absyn_Switch_clause** _temp874; struct Cyc_List_List*
_temp876; struct Cyc_Absyn_Stmt* _temp878; struct _tuple2 _temp880; struct Cyc_Absyn_Stmt*
_temp882; struct Cyc_Absyn_Exp* _temp884; struct _tuple2 _temp886; struct Cyc_Absyn_Stmt*
_temp888; struct Cyc_Absyn_Exp* _temp890; struct Cyc_Absyn_Exp* _temp892; struct
Cyc_List_List* _temp894; struct Cyc_Absyn_Exp* _temp896; struct Cyc_Absyn_Stmt*
_temp898; struct Cyc_Absyn_Decl* _temp900; struct Cyc_Absyn_Stmt* _temp902;
struct Cyc_Absyn_Stmt* _temp904; struct Cyc_Absyn_Stmt* _temp906; struct
_tagged_string* _temp908; struct _tuple2 _temp910; struct Cyc_Absyn_Stmt*
_temp912; struct Cyc_Absyn_Exp* _temp914; struct Cyc_Absyn_Stmt* _temp916;
struct Cyc_List_List* _temp918; struct Cyc_Absyn_Stmt* _temp920; _LL798: if(
_temp796 == Cyc_Absyn_Skip_s){ goto _LL799;} else{ goto _LL800;} _LL800: if((
unsigned int) _temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL839: _temp838=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp796)->f1; goto _LL801;} else{ goto _LL802;} _LL802: if(( unsigned int)
_temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Seq_s_tag: 0){
_LL843: _temp842=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp796)->f1; goto _LL841; _LL841: _temp840=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp796)->f2; goto _LL803;} else{ goto _LL804;} _LL804:
if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL845: _temp844=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp796)->f1; if( _temp844 == 0){ goto _LL805;} else{ goto _LL806;}} else{ goto
_LL806;} _LL806: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*)
_temp796)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL847: _temp846=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp796)->f1; goto _LL807;} else{ goto
_LL808;} _LL808: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*)
_temp796)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL853: _temp852=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp796)->f1; goto _LL851; _LL851:
_temp850=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp796)->f2; goto _LL849; _LL849: _temp848=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp796)->f3; goto _LL809;} else{ goto _LL810;}
_LL810: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*) _temp796)->tag
== Cyc_Absyn_While_s_tag: 0){ _LL857: _temp856=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp796)->f1; _LL861: _temp860= _temp856.f1; goto _LL859; _LL859: _temp858=
_temp856.f2; goto _LL855; _LL855: _temp854=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp796)->f2; goto _LL811;} else{ goto _LL812;}
_LL812: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*) _temp796)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL863: _temp862=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp796)->f1; goto _LL813;} else{ goto _LL814;}
_LL814: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*) _temp796)->tag
== Cyc_Absyn_Continue_s_tag: 0){ _LL865: _temp864=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp796)->f1; goto _LL815;} else{ goto
_LL816;} _LL816: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*)
_temp796)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL869: _temp868=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp796)->f1; goto _LL867;
_LL867: _temp866=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp796)->f2; goto _LL817;} else{ goto _LL818;} _LL818: if(( unsigned int)
_temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL873: _temp872=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp796)->f1; if( _temp872 == 0){ goto _LL871;} else{ goto _LL820;} _LL871:
_temp870=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp796)->f2; goto _LL819;} else{ goto _LL820;} _LL820: if(( unsigned int)
_temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL877: _temp876=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp796)->f1; goto _LL875; _LL875: _temp874=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp796)->f2; goto _LL821;} else{ goto
_LL822;} _LL822: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*)
_temp796)->tag == Cyc_Absyn_For_s_tag: 0){ _LL893: _temp892=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp796)->f1; goto _LL887; _LL887: _temp886=(
struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp796)->f2; _LL891:
_temp890= _temp886.f1; goto _LL889; _LL889: _temp888= _temp886.f2; goto _LL881;
_LL881: _temp880=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp796)->f3;
_LL885: _temp884= _temp880.f1; goto _LL883; _LL883: _temp882= _temp880.f2; goto
_LL879; _LL879: _temp878=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp796)->f4; goto _LL823;} else{ goto _LL824;} _LL824: if(( unsigned int)
_temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL897: _temp896=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp796)->f1; goto _LL895; _LL895: _temp894=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp796)->f2; goto _LL825;} else{ goto _LL826;}
_LL826: if(( unsigned int) _temp796 > 1u?(( struct _enum_struct*) _temp796)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL901: _temp900=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp796)->f1; goto _LL899; _LL899: _temp898=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp796)->f2; goto _LL827;}
else{ goto _LL828;} _LL828: if(( unsigned int) _temp796 > 1u?(( struct
_enum_struct*) _temp796)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL903: _temp902=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp796)->f1; goto
_LL829;} else{ goto _LL830;} _LL830: if(( unsigned int) _temp796 > 1u?(( struct
_enum_struct*) _temp796)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL905: _temp904=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp796)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: if(( unsigned int) _temp796 > 1u?(( struct
_enum_struct*) _temp796)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL909: _temp908=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp796)->f1; goto
_LL907; _LL907: _temp906=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp796)->f2; goto _LL833;} else{ goto _LL834;} _LL834: if(( unsigned int)
_temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL917: _temp916=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp796)->f1; goto _LL911; _LL911: _temp910=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp796)->f2; _LL915: _temp914= _temp910.f1; goto _LL913; _LL913: _temp912=
_temp910.f2; goto _LL835;} else{ goto _LL836;} _LL836: if(( unsigned int)
_temp796 > 1u?(( struct _enum_struct*) _temp796)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL921: _temp920=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp796)->f1; goto _LL919; _LL919: _temp918=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp796)->f2; goto _LL837;} else{ goto _LL797;}
_LL799: Cyc_Absyndump_dump_semi(); goto _LL797; _LL801: Cyc_Absyndump_dumpexp(
_temp838); Cyc_Absyndump_dump_semi(); goto _LL797; _LL803: if( Cyc_Absynpp_is_declaration(
_temp842)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp842);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp842);}
if( Cyc_Absynpp_is_declaration( _temp840)){ Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp840); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dumpstmt( _temp840);} goto _LL797; _LL805: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp922=( char*)"return;"; struct _tagged_string
_temp923; _temp923.curr= _temp922; _temp923.base= _temp922; _temp923.last_plus_one=
_temp922 + 8; _temp923;})); goto _LL797; _LL807: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp924=( char*)"return"; struct _tagged_string
_temp925; _temp925.curr= _temp924; _temp925.base= _temp924; _temp925.last_plus_one=
_temp924 + 7; _temp925;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp926= _temp846; if( _temp926 == 0){ _throw(
Null_Exception);} _temp926;})); Cyc_Absyndump_dump_semi(); goto _LL797; _LL809:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp927=( char*)"if(";
struct _tagged_string _temp928; _temp928.curr= _temp927; _temp928.base= _temp927;
_temp928.last_plus_one= _temp927 + 4; _temp928;})); Cyc_Absyndump_dumpexp(
_temp852); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp929=(
char*)"){"; struct _tagged_string _temp930; _temp930.curr= _temp929; _temp930.base=
_temp929; _temp930.last_plus_one= _temp929 + 3; _temp930;})); Cyc_Absyndump_dumpstmt(
_temp850); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp931=( void*) _temp848->r;
_LL933: if( _temp931 == Cyc_Absyn_Skip_s){ goto _LL934;} else{ goto _LL935;}
_LL935: goto _LL936; _LL934: goto _LL932; _LL936: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp937=( char*)"else{"; struct _tagged_string _temp938;
_temp938.curr= _temp937; _temp938.base= _temp937; _temp938.last_plus_one=
_temp937 + 6; _temp938;})); Cyc_Absyndump_dumpstmt( _temp848); Cyc_Absyndump_dump_char((
int)'}'); goto _LL932; _LL932:;} goto _LL797; _LL811: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp939=( char*)"while("; struct _tagged_string
_temp940; _temp940.curr= _temp939; _temp940.base= _temp939; _temp940.last_plus_one=
_temp939 + 7; _temp940;})); Cyc_Absyndump_dumpexp( _temp860); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp941=( char*)") {"; struct _tagged_string
_temp942; _temp942.curr= _temp941; _temp942.base= _temp941; _temp942.last_plus_one=
_temp941 + 4; _temp942;})); Cyc_Absyndump_dumpstmt( _temp854); Cyc_Absyndump_dump_char((
int)'}'); goto _LL797; _LL813: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp943=( char*)"break;"; struct _tagged_string _temp944; _temp944.curr=
_temp943; _temp944.base= _temp943; _temp944.last_plus_one= _temp943 + 7;
_temp944;})); goto _LL797; _LL815: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp945=( char*)"continue;"; struct _tagged_string _temp946; _temp946.curr=
_temp945; _temp946.base= _temp945; _temp946.last_plus_one= _temp945 + 10;
_temp946;})); goto _LL797; _LL817: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp947=( char*)"goto"; struct _tagged_string _temp948; _temp948.curr=
_temp947; _temp948.base= _temp947; _temp948.last_plus_one= _temp947 + 5;
_temp948;})); Cyc_Absyndump_dump_str( _temp868); Cyc_Absyndump_dump_semi(); goto
_LL797; _LL819: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp949=(
char*)"fallthru;"; struct _tagged_string _temp950; _temp950.curr= _temp949;
_temp950.base= _temp949; _temp950.last_plus_one= _temp949 + 10; _temp950;}));
goto _LL797; _LL821: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp951=( char*)"fallthru("; struct _tagged_string _temp952; _temp952.curr=
_temp951; _temp952.base= _temp951; _temp952.last_plus_one= _temp951 + 10;
_temp952;})); Cyc_Absyndump_dumpexps_prec( 20, _temp876); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp953=( char*)");"; struct _tagged_string
_temp954; _temp954.curr= _temp953; _temp954.base= _temp953; _temp954.last_plus_one=
_temp953 + 3; _temp954;})); goto _LL797; _LL823: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp955=( char*)"for("; struct _tagged_string _temp956;
_temp956.curr= _temp955; _temp956.base= _temp955; _temp956.last_plus_one=
_temp955 + 5; _temp956;})); Cyc_Absyndump_dumpexp( _temp892); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp890); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp884); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp957=(
char*)"){"; struct _tagged_string _temp958; _temp958.curr= _temp957; _temp958.base=
_temp957; _temp958.last_plus_one= _temp957 + 3; _temp958;})); Cyc_Absyndump_dumpstmt(
_temp878); Cyc_Absyndump_dump_char(( int)'}'); goto _LL797; _LL825: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp959=( char*)"switch("; struct _tagged_string
_temp960; _temp960.curr= _temp959; _temp960.base= _temp959; _temp960.last_plus_one=
_temp959 + 8; _temp960;})); Cyc_Absyndump_dumpexp( _temp896); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp961=( char*)"){"; struct _tagged_string
_temp962; _temp962.curr= _temp961; _temp962.base= _temp961; _temp962.last_plus_one=
_temp961 + 3; _temp962;})); Cyc_Absyndump_dumpswitchclauses( _temp894); Cyc_Absyndump_dump_char((
int)'}'); goto _LL797; _LL827: Cyc_Absyndump_dumpdecl( _temp900); Cyc_Absyndump_dumpstmt(
_temp898); goto _LL797; _LL829: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp963=( char*)"cut"; struct _tagged_string _temp964; _temp964.curr=
_temp963; _temp964.base= _temp963; _temp964.last_plus_one= _temp963 + 4;
_temp964;})); Cyc_Absyndump_dumpstmt( _temp902); goto _LL797; _LL831: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp965=( char*)"splice"; struct _tagged_string
_temp966; _temp966.curr= _temp965; _temp966.base= _temp965; _temp966.last_plus_one=
_temp965 + 7; _temp966;})); Cyc_Absyndump_dumpstmt( _temp904); goto _LL797;
_LL833: if( Cyc_Absynpp_is_declaration( _temp906)){ Cyc_Absyndump_dump_str(
_temp908); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp967=(
char*)": {"; struct _tagged_string _temp968; _temp968.curr= _temp967; _temp968.base=
_temp967; _temp968.last_plus_one= _temp967 + 4; _temp968;})); Cyc_Absyndump_dumpstmt(
_temp906); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dump_str(
_temp908); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt( _temp906);}
goto _LL797; _LL835: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp969=( char*)"do {"; struct _tagged_string _temp970; _temp970.curr= _temp969;
_temp970.base= _temp969; _temp970.last_plus_one= _temp969 + 5; _temp970;})); Cyc_Absyndump_dumpstmt(
_temp916); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp971=(
char*)"} while ("; struct _tagged_string _temp972; _temp972.curr= _temp971;
_temp972.base= _temp971; _temp972.last_plus_one= _temp971 + 10; _temp972;}));
Cyc_Absyndump_dumpexp( _temp914); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp973=( char*)");"; struct _tagged_string _temp974;
_temp974.curr= _temp973; _temp974.base= _temp973; _temp974.last_plus_one=
_temp973 + 3; _temp974;})); goto _LL797; _LL837: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp975=( char*)"try"; struct _tagged_string _temp976;
_temp976.curr= _temp975; _temp976.base= _temp975; _temp976.last_plus_one=
_temp975 + 4; _temp976;})); Cyc_Absyndump_dumpstmt( _temp920); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp977=( char*)"catch {"; struct _tagged_string
_temp978; _temp978.curr= _temp977; _temp978.base= _temp977; _temp978.last_plus_one=
_temp977 + 8; _temp978;})); Cyc_Absyndump_dumpswitchclauses( _temp918); Cyc_Absyndump_dump_char((
int)'}'); goto _LL797; _LL797:;} void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char* _temp979=(
char*)""; struct _tagged_string _temp980; _temp980.curr= _temp979; _temp980.base=
_temp979; _temp980.last_plus_one= _temp979 + 1; _temp980;}),( struct
_tagged_string)({ char* _temp981=( char*)"="; struct _tagged_string _temp982;
_temp982.curr= _temp981; _temp982.base= _temp981; _temp982.last_plus_one=
_temp981 + 2; _temp982;}),( struct _tagged_string)({ char* _temp983=( char*)"=";
struct _tagged_string _temp984; _temp984.curr= _temp983; _temp984.base= _temp983;
_temp984.last_plus_one= _temp983 + 2; _temp984;})); Cyc_Absyndump_dumppat((* dp).f2);}
void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp985=( void*) p->r;
int _temp1019; void* _temp1021; int _temp1023; void* _temp1025; char _temp1027;
struct _tagged_string _temp1029; struct Cyc_Absyn_Vardecl* _temp1031; struct Cyc_List_List*
_temp1033; struct Cyc_Absyn_Pat* _temp1035; struct Cyc_Absyn_Vardecl* _temp1037;
struct _tuple0* _temp1039; struct Cyc_List_List* _temp1041; struct Cyc_List_List*
_temp1043; struct _tuple0* _temp1045; struct Cyc_List_List* _temp1047; struct
Cyc_List_List* _temp1049; struct _tuple0* _temp1051; struct Cyc_List_List*
_temp1053; struct Cyc_List_List* _temp1055; struct Cyc_Core_Opt* _temp1057;
struct Cyc_Absyn_Structdecl* _temp1059; struct Cyc_Absyn_Enumfield* _temp1061;
struct Cyc_Absyn_Enumdecl* _temp1063; struct Cyc_List_List* _temp1065; struct
Cyc_List_List* _temp1067; struct Cyc_Core_Opt* _temp1069; struct _tuple0*
_temp1071; struct Cyc_Absyn_Enumfield* _temp1073; struct Cyc_Absyn_Xenumdecl*
_temp1075; struct Cyc_List_List* _temp1077; struct Cyc_List_List* _temp1079;
struct _tuple0* _temp1081; _LL987: if( _temp985 == Cyc_Absyn_Wild_p){ goto
_LL988;} else{ goto _LL989;} _LL989: if( _temp985 == Cyc_Absyn_Null_p){ goto
_LL990;} else{ goto _LL991;} _LL991: if(( unsigned int) _temp985 > 2u?(( struct
_enum_struct*) _temp985)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1022: _temp1021=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp985)->f1; if( _temp1021 == Cyc_Absyn_Signed){
goto _LL1020;} else{ goto _LL993;} _LL1020: _temp1019=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp985)->f2; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Int_p_tag: 0){
_LL1026: _temp1025=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp985)->f1; if(
_temp1025 == Cyc_Absyn_Unsigned){ goto _LL1024;} else{ goto _LL995;} _LL1024:
_temp1023=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp985)->f2; goto _LL994;}
else{ goto _LL995;} _LL995: if(( unsigned int) _temp985 > 2u?(( struct
_enum_struct*) _temp985)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1028: _temp1027=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp985)->f1; goto _LL996;} else{ goto
_LL997;} _LL997: if(( unsigned int) _temp985 > 2u?(( struct _enum_struct*)
_temp985)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1030: _temp1029=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp985)->f1; goto _LL998;}
else{ goto _LL999;} _LL999: if(( unsigned int) _temp985 > 2u?(( struct
_enum_struct*) _temp985)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1032: _temp1031=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp985)->f1; goto
_LL1000;} else{ goto _LL1001;} _LL1001: if(( unsigned int) _temp985 > 2u?((
struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1034:
_temp1033=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp985)->f1;
goto _LL1002;} else{ goto _LL1003;} _LL1003: if(( unsigned int) _temp985 > 2u?((
struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1036:
_temp1035=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp985)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1038: _temp1037=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp985)->f1; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1040: _temp1039=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp985)->f1; goto _LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1046: _temp1045=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp985)->f1; goto _LL1044; _LL1044: _temp1043=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp985)->f2; goto _LL1042; _LL1042:
_temp1041=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp985)->f3; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1052: _temp1051=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp985)->f1; goto _LL1050; _LL1050: _temp1049=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp985)->f2; goto _LL1048; _LL1048:
_temp1047=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp985)->f3; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1060: _temp1059=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp985)->f1; goto _LL1058; _LL1058: _temp1057=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp985)->f2; goto _LL1056; _LL1056: _temp1055=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp985)->f3; goto
_LL1054; _LL1054: _temp1053=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp985)->f4; goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int)
_temp985 > 2u?(( struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Enum_p_tag: 0){
_LL1072: _temp1071=( struct _tuple0*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp985)->f1; goto _LL1070; _LL1070: _temp1069=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp985)->f2; goto _LL1068; _LL1068: _temp1067=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp985)->f3; goto
_LL1066; _LL1066: _temp1065=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp985)->f4; goto _LL1064; _LL1064: _temp1063=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp985)->f5; goto _LL1062; _LL1062: _temp1061=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp985)->f6;
goto _LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp985 > 2u?((
struct _enum_struct*) _temp985)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1082:
_temp1081=( struct _tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp985)->f1;
goto _LL1080; _LL1080: _temp1079=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp985)->f2; goto _LL1078; _LL1078: _temp1077=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp985)->f3; goto _LL1076; _LL1076:
_temp1075=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp985)->f4; goto _LL1074; _LL1074: _temp1073=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp985)->f5; goto _LL1018;} else{ goto
_LL986;} _LL988: Cyc_Absyndump_dump_char(( int)'_'); goto _LL986; _LL990: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1083=( char*)"null"; struct _tagged_string
_temp1084; _temp1084.curr= _temp1083; _temp1084.base= _temp1083; _temp1084.last_plus_one=
_temp1083 + 5; _temp1084;})); goto _LL986; _LL992: Cyc_Absyndump_dump( xprintf("%d",
_temp1019)); goto _LL986; _LL994: Cyc_Absyndump_dump( xprintf("%u",(
unsigned int) _temp1023)); goto _LL986; _LL996: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1085=( char*)"'"; struct _tagged_string _temp1086;
_temp1086.curr= _temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one=
_temp1085 + 2; _temp1086;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp1027)); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1087=( char*)"'"; struct _tagged_string _temp1088; _temp1088.curr=
_temp1087; _temp1088.base= _temp1087; _temp1088.last_plus_one= _temp1087 + 2;
_temp1088;})); goto _LL986; _LL998: Cyc_Absyndump_dump( _temp1029); goto _LL986;
_LL1000: Cyc_Absyndump_dumpqvar( _temp1031->name); goto _LL986; _LL1002:(( void(*)(
void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp1033,( struct _tagged_string)({ char* _temp1089=(
char*)"$("; struct _tagged_string _temp1090; _temp1090.curr= _temp1089;
_temp1090.base= _temp1089; _temp1090.last_plus_one= _temp1089 + 3; _temp1090;}),(
struct _tagged_string)({ char* _temp1091=( char*)")"; struct _tagged_string
_temp1092; _temp1092.curr= _temp1091; _temp1092.base= _temp1091; _temp1092.last_plus_one=
_temp1091 + 2; _temp1092;}),( struct _tagged_string)({ char* _temp1093=( char*)",";
struct _tagged_string _temp1094; _temp1094.curr= _temp1093; _temp1094.base=
_temp1093; _temp1094.last_plus_one= _temp1093 + 2; _temp1094;})); goto _LL986;
_LL1004: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1095=( char*)"&";
struct _tagged_string _temp1096; _temp1096.curr= _temp1095; _temp1096.base=
_temp1095; _temp1096.last_plus_one= _temp1095 + 2; _temp1096;})); Cyc_Absyndump_dumppat(
_temp1035); goto _LL986; _LL1006: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1097=( char*)"*"; struct _tagged_string _temp1098; _temp1098.curr=
_temp1097; _temp1098.base= _temp1097; _temp1098.last_plus_one= _temp1097 + 2;
_temp1098;})); Cyc_Absyndump_dumpqvar( _temp1037->name); goto _LL986; _LL1008:
Cyc_Absyndump_dumpqvar( _temp1039); goto _LL986; _LL1010: Cyc_Absyndump_dumpqvar(
_temp1045); Cyc_Absyndump_dumptvars( _temp1043);(( void(*)( void(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1041,( struct _tagged_string)({ char* _temp1099=( char*)"("; struct
_tagged_string _temp1100; _temp1100.curr= _temp1099; _temp1100.base= _temp1099;
_temp1100.last_plus_one= _temp1099 + 2; _temp1100;}),( struct _tagged_string)({
char* _temp1101=( char*)")"; struct _tagged_string _temp1102; _temp1102.curr=
_temp1101; _temp1102.base= _temp1101; _temp1102.last_plus_one= _temp1101 + 2;
_temp1102;}),( struct _tagged_string)({ char* _temp1103=( char*)","; struct
_tagged_string _temp1104; _temp1104.curr= _temp1103; _temp1104.base= _temp1103;
_temp1104.last_plus_one= _temp1103 + 2; _temp1104;})); goto _LL986; _LL1012: Cyc_Absyndump_dumpqvar(
_temp1051); Cyc_Absyndump_dumptvars( _temp1049);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1047,( struct _tagged_string)({ char* _temp1105=( char*)"{"; struct
_tagged_string _temp1106; _temp1106.curr= _temp1105; _temp1106.base= _temp1105;
_temp1106.last_plus_one= _temp1105 + 2; _temp1106;}),( struct _tagged_string)({
char* _temp1107=( char*)"}"; struct _tagged_string _temp1108; _temp1108.curr=
_temp1107; _temp1108.base= _temp1107; _temp1108.last_plus_one= _temp1107 + 2;
_temp1108;}),( struct _tagged_string)({ char* _temp1109=( char*)","; struct
_tagged_string _temp1110; _temp1110.curr= _temp1109; _temp1110.base= _temp1109;
_temp1110.last_plus_one= _temp1109 + 2; _temp1110;})); goto _LL986; _LL1014: if(
_temp1059->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1059->name)->v);}
Cyc_Absyndump_dumptvars( _temp1055);(( void(*)( void(* f)( struct _tuple8*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1053,( struct _tagged_string)({ char* _temp1111=( char*)"{"; struct
_tagged_string _temp1112; _temp1112.curr= _temp1111; _temp1112.base= _temp1111;
_temp1112.last_plus_one= _temp1111 + 2; _temp1112;}),( struct _tagged_string)({
char* _temp1113=( char*)"}"; struct _tagged_string _temp1114; _temp1114.curr=
_temp1113; _temp1114.base= _temp1113; _temp1114.last_plus_one= _temp1113 + 2;
_temp1114;}),( struct _tagged_string)({ char* _temp1115=( char*)","; struct
_tagged_string _temp1116; _temp1116.curr= _temp1115; _temp1116.base= _temp1115;
_temp1116.last_plus_one= _temp1115 + 2; _temp1116;})); goto _LL986; _LL1016:
_temp1081= _temp1071; _temp1079= _temp1067; _temp1077= _temp1065; goto _LL1018;
_LL1018: Cyc_Absyndump_dumpqvar( _temp1081); Cyc_Absyndump_dumptvars( _temp1079);
if( _temp1077 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1077,( struct
_tagged_string)({ char* _temp1117=( char*)"("; struct _tagged_string _temp1118;
_temp1118.curr= _temp1117; _temp1118.base= _temp1117; _temp1118.last_plus_one=
_temp1117 + 2; _temp1118;}),( struct _tagged_string)({ char* _temp1119=( char*)")";
struct _tagged_string _temp1120; _temp1120.curr= _temp1119; _temp1120.base=
_temp1119; _temp1120.last_plus_one= _temp1119 + 2; _temp1120;}),( struct
_tagged_string)({ char* _temp1121=( char*)","; struct _tagged_string _temp1122;
_temp1122.curr= _temp1121; _temp1122.base= _temp1121; _temp1122.last_plus_one=
_temp1121 + 2; _temp1122;}));} goto _LL986; _LL986:;} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1123= ef->tag; if( _temp1123 == 0){ _throw(
Null_Exception);} _temp1123;}));} if( ef->typs != 0){ Cyc_Absyndump_dumpargs( ef->typs);}}
void Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)(
void(* f)( struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields,( struct _tagged_string)({ char* _temp1124=( char*)","; struct
_tagged_string _temp1125; _temp1125.curr= _temp1124; _temp1125.base= _temp1124;
_temp1125.last_plus_one= _temp1124 + 2; _temp1125;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields= fields->tl){ struct
Cyc_Absyn_Structfield _temp1128; struct Cyc_List_List* _temp1129; void*
_temp1131; struct Cyc_Absyn_Tqual* _temp1133; struct _tagged_string* _temp1135;
struct Cyc_Absyn_Structfield* _temp1126=( struct Cyc_Absyn_Structfield*) fields->hd;
_temp1128=* _temp1126; _LL1136: _temp1135=( struct _tagged_string*) _temp1128.name;
goto _LL1134; _LL1134: _temp1133=( struct Cyc_Absyn_Tqual*) _temp1128.tq; goto
_LL1132; _LL1132: _temp1131=( void*) _temp1128.type; goto _LL1130; _LL1130:
_temp1129=( struct Cyc_List_List*) _temp1128.attributes; goto _LL1127; _LL1127:((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct _tagged_string*),
struct _tagged_string*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)
_temp1133, _temp1131, Cyc_Absyndump_dump_str, _temp1135); Cyc_Absyndump_dumpatts(
_temp1129); Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void* _temp1137=( void*)
d->r; struct Cyc_Absyn_Fndecl* _temp1161; struct Cyc_Absyn_Structdecl* _temp1163;
struct Cyc_Absyn_Vardecl* _temp1165; struct Cyc_Absyn_Vardecl _temp1167; struct
Cyc_List_List* _temp1168; struct Cyc_Core_Opt* _temp1170; int _temp1172; struct
Cyc_Absyn_Exp* _temp1174; void* _temp1176; struct Cyc_Absyn_Tqual* _temp1178;
struct _tuple0* _temp1180; void* _temp1182; struct Cyc_Absyn_Enumdecl* _temp1184;
struct Cyc_Absyn_Xenumdecl* _temp1186; int _temp1188; struct Cyc_Absyn_Exp*
_temp1190; struct Cyc_Core_Opt* _temp1192; struct Cyc_Core_Opt* _temp1194;
struct Cyc_Absyn_Pat* _temp1196; struct Cyc_Absyn_Typedefdecl* _temp1198; struct
Cyc_List_List* _temp1200; struct _tagged_string* _temp1202; struct Cyc_List_List*
_temp1204; struct _tuple0* _temp1206; struct Cyc_List_List* _temp1208; _LL1139:
if(( unsigned int) _temp1137 > 1u?(( struct _enum_struct*) _temp1137)->tag ==
Cyc_Absyn_Fn_d_tag: 0){ _LL1162: _temp1161=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Fn_d_struct*) _temp1137)->f1; goto _LL1140;} else{ goto _LL1141;}
_LL1141: if(( unsigned int) _temp1137 > 1u?(( struct _enum_struct*) _temp1137)->tag
== Cyc_Absyn_Struct_d_tag: 0){ _LL1164: _temp1163=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1137)->f1; goto _LL1142;} else{ goto
_LL1143;} _LL1143: if(( unsigned int) _temp1137 > 1u?(( struct _enum_struct*)
_temp1137)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL1166: _temp1165=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1137)->f1; _temp1167=* _temp1165; _LL1183:
_temp1182=( void*) _temp1167.sc; goto _LL1181; _LL1181: _temp1180=( struct
_tuple0*) _temp1167.name; goto _LL1179; _LL1179: _temp1178=( struct Cyc_Absyn_Tqual*)
_temp1167.tq; goto _LL1177; _LL1177: _temp1176=( void*) _temp1167.type; goto
_LL1175; _LL1175: _temp1174=( struct Cyc_Absyn_Exp*) _temp1167.initializer; goto
_LL1173; _LL1173: _temp1172=( int) _temp1167.shadow; goto _LL1171; _LL1171:
_temp1170=( struct Cyc_Core_Opt*) _temp1167.region; goto _LL1169; _LL1169:
_temp1168=( struct Cyc_List_List*) _temp1167.attributes; goto _LL1144;} else{
goto _LL1145;} _LL1145: if( _temp1137 == Cyc_Absyn_Union_d){ goto _LL1146;}
else{ goto _LL1147;} _LL1147: if(( unsigned int) _temp1137 > 1u?(( struct
_enum_struct*) _temp1137)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1185: _temp1184=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1137)->f1;
goto _LL1148;} else{ goto _LL1149;} _LL1149: if(( unsigned int) _temp1137 > 1u?((
struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1187:
_temp1186=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp1137)->f1; goto _LL1150;} else{ goto _LL1151;} _LL1151: if(( unsigned int)
_temp1137 > 1u?(( struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_Let_d_tag:
0){ _LL1197: _temp1196=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1137)->f1; goto _LL1195; _LL1195: _temp1194=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1137)->f2; goto _LL1193; _LL1193: _temp1192=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1137)->f3; goto
_LL1191; _LL1191: _temp1190=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1137)->f4; goto _LL1189; _LL1189: _temp1188=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1137)->f5; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(( unsigned int)
_temp1137 > 1u?(( struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_Typedef_d_tag:
0){ _LL1199: _temp1198=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1137)->f1; goto _LL1154;} else{ goto _LL1155;} _LL1155: if(( unsigned int)
_temp1137 > 1u?(( struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_Namespace_d_tag:
0){ _LL1203: _temp1202=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1137)->f1; goto _LL1201; _LL1201: _temp1200=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1137)->f2; goto _LL1156;} else{ goto
_LL1157;} _LL1157: if(( unsigned int) _temp1137 > 1u?(( struct _enum_struct*)
_temp1137)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL1207: _temp1206=( struct
_tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1137)->f1; goto _LL1205;
_LL1205: _temp1204=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1137)->f2; goto _LL1158;} else{ goto _LL1159;} _LL1159: if(( unsigned int)
_temp1137 > 1u?(( struct _enum_struct*) _temp1137)->tag == Cyc_Absyn_ExternC_d_tag:
0){ _LL1209: _temp1208=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1137)->f1; goto _LL1160;} else{ goto _LL1138;} _LL1140: if( _temp1161->is_inline){
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1210=( char*)"inline";
struct _tagged_string _temp1211; _temp1211.curr= _temp1210; _temp1211.base=
_temp1210; _temp1211.last_plus_one= _temp1210 + 7; _temp1211;}));} Cyc_Absyndump_dumpscope((
void*) _temp1161->sc);{ void* t=({ struct Cyc_Absyn_FnType_struct* _temp1212=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1212->tag= Cyc_Absyn_FnType_tag; _temp1212->f1=({ struct Cyc_Absyn_FnInfo
_temp1213; _temp1213.tvars= _temp1161->tvs; _temp1213.effect= _temp1161->effect;
_temp1213.ret_typ=( void*)(( void*) _temp1161->ret_type); _temp1213.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1161->args); _temp1213.varargs=
_temp1161->varargs; _temp1213.attributes= 0; _temp1213;});( void*) _temp1212;});((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)( 0, t, Cyc_Absyndump_dump_atts_qvar,
_temp1161); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1161->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1138;} _LL1142:
Cyc_Absyndump_dumpscope(( void*) _temp1163->sc); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1214=( char*)"struct"; struct _tagged_string
_temp1215; _temp1215.curr= _temp1214; _temp1215.base= _temp1214; _temp1215.last_plus_one=
_temp1214 + 7; _temp1215;})); if( _temp1163->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)( _temp1163->name)->v);} Cyc_Absyndump_dumptvars( _temp1163->tvs);
if( _temp1163->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)( _temp1163->fields)->v);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1216=( char*)"}";
struct _tagged_string _temp1217; _temp1217.curr= _temp1216; _temp1217.base=
_temp1216; _temp1217.last_plus_one= _temp1216 + 2; _temp1217;})); Cyc_Absyndump_dumpatts(
_temp1163->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1218=( char*)";"; struct _tagged_string _temp1219; _temp1219.curr=
_temp1218; _temp1219.base= _temp1218; _temp1219.last_plus_one= _temp1218 + 2;
_temp1219;}));} goto _LL1138; _LL1144: Cyc_Absyndump_dumpscope( _temp1182);((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct _tuple0*), struct
_tuple0*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*) _temp1178,
_temp1176, Cyc_Absyndump_dumpqvar, _temp1180); Cyc_Absyndump_dumpatts( _temp1168);
if( _temp1174 != 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1220= _temp1174; if(
_temp1220 == 0){ _throw( Null_Exception);} _temp1220;}));} Cyc_Absyndump_dump_semi();
goto _LL1138; _LL1146: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1221=( char*)"**UnionDecl**"; struct _tagged_string _temp1222; _temp1222.curr=
_temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one= _temp1221 + 14;
_temp1222;})); goto _LL1138; _LL1148: Cyc_Absyndump_dumpscope(( void*) _temp1184->sc);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1223=( char*)"enum";
struct _tagged_string _temp1224; _temp1224.curr= _temp1223; _temp1224.base=
_temp1223; _temp1224.last_plus_one= _temp1223 + 5; _temp1224;})); if( _temp1184->name
!= 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1184->name)->v);} Cyc_Absyndump_dumptvars(
_temp1184->tvs); Cyc_Absyndump_dump_char(( int)'{'); if( _temp1184->fields == 0){
Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)( _temp1184->fields)->v); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1225=( char*)"};"; struct _tagged_string
_temp1226; _temp1226.curr= _temp1225; _temp1226.base= _temp1225; _temp1226.last_plus_one=
_temp1225 + 3; _temp1226;}));} goto _LL1138; _LL1150: Cyc_Absyndump_dumpscope((
void*) _temp1186->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1227=( char*)"xenum"; struct _tagged_string _temp1228; _temp1228.curr=
_temp1227; _temp1228.base= _temp1227; _temp1228.last_plus_one= _temp1227 + 6;
_temp1228;})); Cyc_Absyndump_dumpqvar( _temp1186->name); Cyc_Absyndump_dump_char((
int)'{'); if( _temp1186->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1186->fields); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1229=( char*)"};"; struct _tagged_string
_temp1230; _temp1230.curr= _temp1229; _temp1230.base= _temp1229; _temp1230.last_plus_one=
_temp1229 + 3; _temp1230;}));} goto _LL1138; _LL1152: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1231=( char*)"let"; struct _tagged_string
_temp1232; _temp1232.curr= _temp1231; _temp1232.base= _temp1231; _temp1232.last_plus_one=
_temp1231 + 4; _temp1232;})); Cyc_Absyndump_dumppat( _temp1196); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1190); Cyc_Absyndump_dump_semi(); goto
_LL1138; _LL1154: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1233=(
char*)"typedef"; struct _tagged_string _temp1234; _temp1234.curr= _temp1233;
_temp1234.base= _temp1233; _temp1234.last_plus_one= _temp1233 + 8; _temp1234;}));((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)( 0,( void*) _temp1198->defn,
Cyc_Absyndump_dumptypedefname, _temp1198); Cyc_Absyndump_dump_semi(); goto
_LL1138; _LL1156: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1235=(
char*)"namespace"; struct _tagged_string _temp1236; _temp1236.curr= _temp1235;
_temp1236.base= _temp1235; _temp1236.last_plus_one= _temp1235 + 10; _temp1236;}));
Cyc_Absyndump_dump_str( _temp1202); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1200 != 0; _temp1200= _temp1200->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1200->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1138; _LL1158: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1237=( char*)"using"; struct _tagged_string
_temp1238; _temp1238.curr= _temp1237; _temp1238.base= _temp1237; _temp1238.last_plus_one=
_temp1237 + 6; _temp1238;})); Cyc_Absyndump_dumpqvar( _temp1206); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1204 != 0; _temp1204= _temp1204->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1204->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1138; _LL1160: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1239=( char*)"extern \"C\" {"; struct _tagged_string _temp1240; _temp1240.curr=
_temp1239; _temp1240.base= _temp1239; _temp1240.last_plus_one= _temp1239 + 13;
_temp1240;})); for( 0; _temp1208 != 0; _temp1208= _temp1208->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1208->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1138; _LL1138:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp*
e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=( unsigned int) 1){
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1241=( void*) tms->hd;
struct Cyc_Absyn_Tqual* _temp1259; void* _temp1261; void* _temp1263; struct Cyc_Absyn_Exp*
_temp1265; struct Cyc_Absyn_Tqual* _temp1267; void* _temp1269; void* _temp1271;
struct Cyc_Absyn_Exp* _temp1273; struct Cyc_Absyn_Tqual* _temp1275; void*
_temp1277; void* _temp1279; struct Cyc_Absyn_Tqual* _temp1281; void* _temp1283;
struct Cyc_Absyn_Tvar* _temp1285; void* _temp1287; struct Cyc_Absyn_Exp*
_temp1289; struct Cyc_Absyn_Tqual* _temp1291; void* _temp1293; struct Cyc_Absyn_Tvar*
_temp1295; void* _temp1297; struct Cyc_Absyn_Exp* _temp1299; struct Cyc_Absyn_Tqual*
_temp1301; void* _temp1303; struct Cyc_Absyn_Tvar* _temp1305; void* _temp1307;
struct Cyc_Absyn_Tqual* _temp1309; void* _temp1311; void* _temp1313; _LL1243:
if(( unsigned int) _temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag ==
Cyc_Absyn_Pointer_mod_tag: 0){ _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; if(( unsigned int) _temp1263 > 1u?(( struct _enum_struct*)
_temp1263)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1266: _temp1265=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1263)->f1; goto
_LL1262;} else{ goto _LL1245;} _LL1262: _temp1261=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; if( _temp1261 == Cyc_Absyn_HeapRgn){ goto _LL1260;} else{ goto
_LL1245;} _LL1260: _temp1259=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f3; goto _LL1244;} else{ goto _LL1245;} _LL1245: if(( unsigned int)
_temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1272: _temp1271=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; if(( unsigned int) _temp1271 > 1u?(( struct _enum_struct*)
_temp1271)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1274: _temp1273=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1271)->f1; goto
_LL1270;} else{ goto _LL1247;} _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; if( _temp1269 == Cyc_Absyn_HeapRgn){ goto _LL1268;} else{ goto
_LL1247;} _LL1268: _temp1267=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f3; goto _LL1246;} else{ goto _LL1247;} _LL1247: if(( unsigned int)
_temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1280: _temp1279=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; if( _temp1279 == Cyc_Absyn_TaggedArray_ps){ goto _LL1278;} else{
goto _LL1249;} _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; if( _temp1277 == Cyc_Absyn_HeapRgn){ goto _LL1276;} else{ goto
_LL1249;} _LL1276: _temp1275=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f3; goto _LL1248;} else{ goto _LL1249;} _LL1249: if(( unsigned int)
_temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1288: _temp1287=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; if(( unsigned int) _temp1287 > 1u?(( struct _enum_struct*)
_temp1287)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1290: _temp1289=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1287)->f1; goto
_LL1284;} else{ goto _LL1251;} _LL1284: _temp1283=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; if(( unsigned int) _temp1283 > 5u?(( struct _enum_struct*)
_temp1283)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1286: _temp1285=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1283)->f1; goto _LL1282;} else{ goto
_LL1251;} _LL1282: _temp1281=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f3; goto _LL1250;} else{ goto _LL1251;} _LL1251: if(( unsigned int)
_temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1298: _temp1297=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; if(( unsigned int) _temp1297 > 1u?(( struct _enum_struct*)
_temp1297)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1300: _temp1299=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1297)->f1; goto
_LL1294;} else{ goto _LL1253;} _LL1294: _temp1293=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; if(( unsigned int) _temp1293 > 5u?(( struct _enum_struct*)
_temp1293)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1296: _temp1295=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1293)->f1; goto _LL1292;} else{ goto
_LL1253;} _LL1292: _temp1291=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f3; goto _LL1252;} else{ goto _LL1253;} _LL1253: if(( unsigned int)
_temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1308: _temp1307=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; if( _temp1307 == Cyc_Absyn_TaggedArray_ps){ goto _LL1304;} else{
goto _LL1255;} _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; if(( unsigned int) _temp1303 > 5u?(( struct _enum_struct*)
_temp1303)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1306: _temp1305=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1303)->f1; goto _LL1302;} else{ goto
_LL1255;} _LL1302: _temp1301=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f3; goto _LL1254;} else{ goto _LL1255;} _LL1255: if(( unsigned int)
_temp1241 > 1u?(( struct _enum_struct*) _temp1241)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1314: _temp1313=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f1; goto _LL1312; _LL1312: _temp1311=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1241)->f2; goto _LL1310; _LL1310: _temp1309=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1241)->f3; goto _LL1256;} else{ goto
_LL1257;} _LL1257: goto _LL1258; _LL1244: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1265);(( void(*)( struct Cyc_List_List* tms,
void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return;
_LL1246: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1273);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1248: Cyc_Absyndump_dump_char((
int)'?');(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(
tms->tl, f, a); return; _LL1250: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1289); Cyc_Absyndump_dump_str( _temp1285->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1252: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1299); Cyc_Absyndump_dump_str( _temp1295->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1254: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1305->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1256:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp1315=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp1315=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp1316=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1317; _temp1317.curr= _temp1316; _temp1317.base=
_temp1316; _temp1317.last_plus_one= _temp1316 + 25; _temp1317;})};( struct
_xenum_struct*) _temp1315;})); _LL1258: { int next_is_pointer= 0; if( tms->tl !=
0){ void* _temp1318=( void*)( tms->tl)->hd; struct Cyc_Absyn_Tqual* _temp1324;
void* _temp1326; void* _temp1328; _LL1320: if(( unsigned int) _temp1318 > 1u?((
struct _enum_struct*) _temp1318)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1329:
_temp1328=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1318)->f1; goto
_LL1327; _LL1327: _temp1326=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1318)->f2; goto _LL1325; _LL1325: _temp1324=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1318)->f3; goto _LL1321;} else{ goto
_LL1322;} _LL1322: goto _LL1323; _LL1321: next_is_pointer= 1; goto _LL1319;
_LL1323: goto _LL1319; _LL1319:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1330=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp1346;
void* _temp1348; struct Cyc_Core_Opt* _temp1350; int _temp1352; struct Cyc_List_List*
_temp1354; void* _temp1356; struct Cyc_Position_Segment* _temp1358; struct Cyc_List_List*
_temp1360; int _temp1362; struct Cyc_Position_Segment* _temp1364; struct Cyc_List_List*
_temp1366; struct Cyc_List_List* _temp1368; struct Cyc_Position_Segment*
_temp1370; struct Cyc_Absyn_Tqual* _temp1372; void* _temp1374; void* _temp1376;
_LL1332: if( _temp1330 == Cyc_Absyn_Carray_mod){ goto _LL1333;} else{ goto
_LL1334;} _LL1334: if(( unsigned int) _temp1330 > 1u?(( struct _enum_struct*)
_temp1330)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL1347: _temp1346=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1330)->f1; goto
_LL1335;} else{ goto _LL1336;} _LL1336: if(( unsigned int) _temp1330 > 1u?((
struct _enum_struct*) _temp1330)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL1349:
_temp1348=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1330)->f1; if(((
struct _enum_struct*) _temp1348)->tag == Cyc_Absyn_WithTypes_tag){ _LL1355:
_temp1354=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1348)->f1; goto _LL1353; _LL1353: _temp1352=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1348)->f2; goto _LL1351; _LL1351: _temp1350=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_WithTypes_struct*) _temp1348)->f3; goto _LL1337;} else{ goto
_LL1338;}} else{ goto _LL1338;} _LL1338: if(( unsigned int) _temp1330 > 1u?((
struct _enum_struct*) _temp1330)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL1357:
_temp1356=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1330)->f1; if(((
struct _enum_struct*) _temp1356)->tag == Cyc_Absyn_NoTypes_tag){ _LL1361:
_temp1360=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1356)->f1;
goto _LL1359; _LL1359: _temp1358=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1356)->f2; goto _LL1339;} else{ goto _LL1340;}} else{ goto _LL1340;}
_LL1340: if(( unsigned int) _temp1330 > 1u?(( struct _enum_struct*) _temp1330)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL1367: _temp1366=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1330)->f1; goto _LL1365; _LL1365:
_temp1364=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1330)->f2; goto _LL1363; _LL1363: _temp1362=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1330)->f3; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(( unsigned int)
_temp1330 > 1u?(( struct _enum_struct*) _temp1330)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL1371: _temp1370=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1330)->f1; goto _LL1369; _LL1369: _temp1368=( struct Cyc_List_List*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1330)->f2; goto _LL1343;} else{
goto _LL1344;} _LL1344: if(( unsigned int) _temp1330 > 1u?(( struct _enum_struct*)
_temp1330)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1377: _temp1376=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1330)->f1; goto _LL1375; _LL1375:
_temp1374=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1330)->f2; goto
_LL1373; _LL1373: _temp1372=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1330)->f3; goto _LL1345;} else{ goto _LL1331;} _LL1333: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1378=( char*)"[]"; struct _tagged_string
_temp1379; _temp1379.curr= _temp1378; _temp1379.base= _temp1378; _temp1379.last_plus_one=
_temp1378 + 3; _temp1379;})); goto _LL1331; _LL1335: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1346); Cyc_Absyndump_dump_char(( int)']');
goto _LL1331; _LL1337: Cyc_Absyndump_dumpfunargs( _temp1354, _temp1352,
_temp1350); goto _LL1331; _LL1339:(( void(*)( void(* f)( struct _tagged_string*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1360,( struct _tagged_string)({ char* _temp1380=( char*)"("; struct
_tagged_string _temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380;
_temp1381.last_plus_one= _temp1380 + 2; _temp1381;}),( struct _tagged_string)({
char* _temp1382=( char*)")"; struct _tagged_string _temp1383; _temp1383.curr=
_temp1382; _temp1383.base= _temp1382; _temp1383.last_plus_one= _temp1382 + 2;
_temp1383;}),( struct _tagged_string)({ char* _temp1384=( char*)","; struct
_tagged_string _temp1385; _temp1385.curr= _temp1384; _temp1385.base= _temp1384;
_temp1385.last_plus_one= _temp1384 + 2; _temp1385;})); goto _LL1331; _LL1341:
if( _temp1362){ Cyc_Absyndump_dumpkindedtvars( _temp1366);} else{ Cyc_Absyndump_dumptvars(
_temp1366);} goto _LL1331; _LL1343: Cyc_Absyndump_dumpatts( _temp1368); goto
_LL1331; _LL1345:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1386=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp1386=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp1387=( char*)"dumptms"; struct
_tagged_string _temp1388; _temp1388.curr= _temp1387; _temp1388.base= _temp1387;
_temp1388.last_plus_one= _temp1387 + 8; _temp1388;})};( struct _xenum_struct*)
_temp1386;})); _LL1331:;} return;} _LL1242:;}} static struct Cyc_Absyn_Tqual Cyc_Absyndump_empty_tq_v=(
struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict= 0}; static
struct Cyc_Absyn_Tqual* Cyc_Absyndump_empty_tq=& Cyc_Absyndump_empty_tq_v; void
Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual* tq, void* t, void(* f)( void*),
void* a){ if( tq == 0){ tq=( struct Cyc_Absyn_Tqual*) Cyc_Absyndump_empty_tq;}{
struct Cyc_List_List* _temp1392; void* _temp1394; struct Cyc_Absyn_Tqual*
_temp1396; struct _tuple4 _temp1390= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual*)({
struct Cyc_Absyn_Tqual* _temp1389= tq; if( _temp1389 == 0){ _throw(
Null_Exception);} _temp1389;}), t); _LL1397: _temp1396= _temp1390.f1; goto
_LL1395; _LL1395: _temp1394= _temp1390.f2; goto _LL1393; _LL1393: _temp1392=
_temp1390.f3; goto _LL1391; _LL1391: Cyc_Absyndump_dumptq( _temp1396); Cyc_Absyndump_dumpntyp(
_temp1394);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1392), f, a);}} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl= tdl->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) tdl->hd);}}