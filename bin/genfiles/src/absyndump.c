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
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* xenumdecls; struct Cyc_Dict_Dict*
typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; }
; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
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
_temp148 > 4u?(( struct _enum_struct*) _temp148)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL157: _temp156=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp148)->f1;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp148 > 4u?((
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
_temp175= t; struct Cyc_Absyn_Exp* _temp237; struct Cyc_Absyn_Tqual* _temp239;
void* _temp241; struct Cyc_Absyn_FnInfo _temp243; struct Cyc_Absyn_PtrInfo
_temp245; struct Cyc_Absyn_Tvar* _temp247; int _temp249; struct Cyc_Core_Opt*
_temp251; void* _temp253; int _temp255; struct Cyc_Core_Opt* _temp257; struct
Cyc_Core_Opt _temp259; void* _temp260; void* _temp262; struct Cyc_Absyn_Enumdecl**
_temp264; struct Cyc_List_List* _temp266; struct _tuple0* _temp268; struct Cyc_Absyn_Enumdecl**
_temp270; struct Cyc_List_List* _temp272; struct _tuple0* _temp274; struct Cyc_Absyn_Xenumdecl**
_temp276; struct _tuple0* _temp278; void* _temp280; void* _temp282; void*
_temp284; void* _temp286; void* _temp288; void* _temp290; void* _temp292; void*
_temp294; void* _temp296; void* _temp298; void* _temp300; void* _temp302; void*
_temp304; void* _temp306; void* _temp308; void* _temp310; struct Cyc_List_List*
_temp312; struct Cyc_Absyn_Structdecl** _temp314; struct Cyc_List_List* _temp316;
struct _tuple0* _temp318; struct Cyc_Absyn_Structdecl** _temp320; struct Cyc_List_List*
_temp322; struct _tuple0* _temp324; struct Cyc_Absyn_Uniondecl** _temp326;
struct Cyc_List_List* _temp328; struct _tuple0* _temp330; struct Cyc_Absyn_Uniondecl**
_temp332; struct Cyc_List_List* _temp334; struct _tuple0* _temp336; struct Cyc_Core_Opt*
_temp338; struct Cyc_List_List* _temp340; struct _tuple0* _temp342; void*
_temp344; void* _temp346; struct Cyc_List_List* _temp348; _LL177: if((
unsigned int) _temp175 > 4u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL242: _temp241=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp175)->f1;
goto _LL240; _LL240: _temp239=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f2; goto _LL238; _LL238: _temp237=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp175)->f3; goto _LL178;} else{ goto _LL179;}
_LL179: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL244: _temp243=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp175)->f1; goto _LL180;} else{ goto _LL181;}
_LL181: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL246: _temp245=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp175)->f1; goto _LL182;} else{ goto
_LL183;} _LL183: if( _temp175 == Cyc_Absyn_VoidType){ goto _LL184;} else{ goto
_LL185;} _LL185: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*)
_temp175)->tag == Cyc_Absyn_VarType_tag: 0){ _LL248: _temp247=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp175)->f1; goto _LL186;} else{ goto _LL187;}
_LL187: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL254: _temp253=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp175)->f1; goto _LL252; _LL252: _temp251=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp175)->f2; if( _temp251 == 0){ goto _LL250;} else{
goto _LL189;} _LL250: _temp249=( int)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f3;
goto _LL188;} else{ goto _LL189;} _LL189: if(( unsigned int) _temp175 > 4u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_Evar_tag: 0){ _LL263: _temp262=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f1; goto _LL258; _LL258:
_temp257=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f2;
if( _temp257 == 0){ goto _LL191;} else{ _temp259=* _temp257; _LL261: _temp260=(
void*) _temp259.v; goto _LL256;} _LL256: _temp255=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp175)->f3; goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp175 > 4u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL269: _temp268=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp175)->f1; if( _temp268 == 0){ goto _LL267;} else{ goto _LL193;} _LL267:
_temp266=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp175)->f2;
goto _LL265; _LL265: _temp264=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp175)->f3; goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp175 > 4u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL275: _temp274=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp175)->f1; goto _LL273; _LL273: _temp272=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp175)->f2; goto _LL271; _LL271: _temp270=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp175)->f3;
goto _LL194;} else{ goto _LL195;} _LL195: if(( unsigned int) _temp175 > 4u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL279:
_temp278=( struct _tuple0*)(( struct Cyc_Absyn_XenumType_struct*) _temp175)->f1;
goto _LL277; _LL277: _temp276=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp175)->f2; goto _LL196;} else{ goto _LL197;} _LL197: if(( unsigned int)
_temp175 > 4u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag:
0){ _LL283: _temp282=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1;
if( _temp282 == Cyc_Absyn_Signed){ goto _LL281;} else{ goto _LL199;} _LL281:
_temp280=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if(
_temp280 == Cyc_Absyn_B1){ goto _LL198;} else{ goto _LL199;}} else{ goto _LL199;}
_LL199: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL287: _temp286=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f1; if( _temp286 == Cyc_Absyn_Unsigned){ goto _LL285;} else{ goto
_LL201;} _LL285: _temp284=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2;
if( _temp284 == Cyc_Absyn_B1){ goto _LL200;} else{ goto _LL201;}} else{ goto
_LL201;} _LL201: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*)
_temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL291: _temp290=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp290 == Cyc_Absyn_Signed){
goto _LL289;} else{ goto _LL203;} _LL289: _temp288=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp288 == Cyc_Absyn_B2){ goto _LL202;} else{ goto _LL203;}}
else{ goto _LL203;} _LL203: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL295: _temp294=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp294 == Cyc_Absyn_Unsigned){
goto _LL293;} else{ goto _LL205;} _LL293: _temp292=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp292 == Cyc_Absyn_B2){ goto _LL204;} else{ goto _LL205;}}
else{ goto _LL205;} _LL205: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL299: _temp298=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp298 == Cyc_Absyn_Signed){
goto _LL297;} else{ goto _LL207;} _LL297: _temp296=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp296 == Cyc_Absyn_B4){ goto _LL206;} else{ goto _LL207;}}
else{ goto _LL207;} _LL207: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL303: _temp302=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp302 == Cyc_Absyn_Unsigned){
goto _LL301;} else{ goto _LL209;} _LL301: _temp300=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp300 == Cyc_Absyn_B4){ goto _LL208;} else{ goto _LL209;}}
else{ goto _LL209;} _LL209: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL307: _temp306=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp306 == Cyc_Absyn_Signed){
goto _LL305;} else{ goto _LL211;} _LL305: _temp304=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp304 == Cyc_Absyn_B8){ goto _LL210;} else{ goto _LL211;}}
else{ goto _LL211;} _LL211: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL311: _temp310=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if( _temp310 == Cyc_Absyn_Unsigned){
goto _LL309;} else{ goto _LL213;} _LL309: _temp308=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp308 == Cyc_Absyn_B8){ goto _LL212;} else{ goto _LL213;}}
else{ goto _LL213;} _LL213: if( _temp175 == Cyc_Absyn_FloatType){ goto _LL214;}
else{ goto _LL215;} _LL215: if( _temp175 == Cyc_Absyn_DoubleType){ goto _LL216;}
else{ goto _LL217;} _LL217: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL313: _temp312=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp175)->f1; goto
_LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_StructType_tag: 0){ _LL319: _temp318=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f1; if(
_temp318 == 0){ goto _LL317;} else{ goto _LL221;} _LL317: _temp316=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp175)->f2; goto _LL315; _LL315:
_temp314=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp175)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp175 > 4u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_StructType_tag:
0){ _LL325: _temp324=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp175)->f1; goto _LL323; _LL323: _temp322=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp175)->f2; goto _LL321; _LL321: _temp320=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f3;
goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp175 > 4u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL331:
_temp330=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp175)->f1;
if( _temp330 == 0){ goto _LL329;} else{ goto _LL225;} _LL329: _temp328=( struct
Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp175)->f2; goto _LL327;
_LL327: _temp326=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp175)->f3; goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int)
_temp175 > 4u?(( struct _enum_struct*) _temp175)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL337: _temp336=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp175)->f1; goto _LL335; _LL335: _temp334=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp175)->f2; goto _LL333; _LL333: _temp332=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp175)->f3;
goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int) _temp175 > 4u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL343:
_temp342=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp175)->f1;
goto _LL341; _LL341: _temp340=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp175)->f2; goto _LL339; _LL339: _temp338=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp175)->f3; goto _LL228;} else{ goto _LL229;}
_LL229: if(( unsigned int) _temp175 > 4u?(( struct _enum_struct*) _temp175)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL345: _temp344=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp175)->f1; goto _LL230;} else{ goto _LL231;} _LL231: if( _temp175 == Cyc_Absyn_HeapRgn){
goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int) _temp175 > 4u?((
struct _enum_struct*) _temp175)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL347:
_temp346=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp175)->f1; goto
_LL234;} else{ goto _LL235;} _LL235: if(( unsigned int) _temp175 > 4u?(( struct
_enum_struct*) _temp175)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL349: _temp348=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp175)->f1; goto
_LL236;} else{ goto _LL176;} _LL178: return; _LL180: return; _LL182: return;
_LL184: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp350=( char*)"void";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 5; _temp351;})); return; _LL186: Cyc_Absyndump_dump_str(
_temp247->name); return; _LL188: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp352=( char*)"%"; struct _tagged_string _temp353; _temp353.curr=
_temp352; _temp353.base= _temp352; _temp353.last_plus_one= _temp352 + 2;
_temp353;})); Cyc_Absyndump_dumpkind( _temp253); Cyc_Absyndump_dump( xprintf("(%d)",
_temp249)); return; _LL190: Cyc_Absyndump_dumpntyp( _temp260); return; _LL192:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp354=( char*)"enum";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 5; _temp355;})); Cyc_Absyndump_dumptps(
_temp266); return; _LL194: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp356=( char*)"enum"; struct _tagged_string _temp357; _temp357.curr= _temp356;
_temp357.base= _temp356; _temp357.last_plus_one= _temp356 + 5; _temp357;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp358= _temp274; if( _temp358 == 0){
_throw( Null_Exception);} _temp358;})); Cyc_Absyndump_dumptps( _temp272);
return; _LL196: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp359=(
char*)"xenum"; struct _tagged_string _temp360; _temp360.curr= _temp359; _temp360.base=
_temp359; _temp360.last_plus_one= _temp359 + 6; _temp360;})); Cyc_Absyndump_dumpqvar(
_temp278); return; _LL198: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp361=( char*)"signed char"; struct _tagged_string _temp362; _temp362.curr=
_temp361; _temp362.base= _temp361; _temp362.last_plus_one= _temp361 + 12;
_temp362;})); return; _LL200: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp363=( char*)"char"; struct _tagged_string _temp364; _temp364.curr= _temp363;
_temp364.base= _temp363; _temp364.last_plus_one= _temp363 + 5; _temp364;}));
return; _LL202: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp365=(
char*)"short"; struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base=
_temp365; _temp366.last_plus_one= _temp365 + 6; _temp366;})); return; _LL204:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp367=( char*)"unsigned short";
struct _tagged_string _temp368; _temp368.curr= _temp367; _temp368.base= _temp367;
_temp368.last_plus_one= _temp367 + 15; _temp368;})); return; _LL206: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp369=( char*)"int"; struct _tagged_string
_temp370; _temp370.curr= _temp369; _temp370.base= _temp369; _temp370.last_plus_one=
_temp369 + 4; _temp370;})); return; _LL208: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp371=( char*)"unsigned int"; struct _tagged_string
_temp372; _temp372.curr= _temp371; _temp372.base= _temp371; _temp372.last_plus_one=
_temp371 + 13; _temp372;})); return; _LL210: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp373=( char*)"long long"; struct _tagged_string
_temp374; _temp374.curr= _temp373; _temp374.base= _temp373; _temp374.last_plus_one=
_temp373 + 10; _temp374;})); return; _LL212: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp375=( char*)"unsigned long long"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 19; _temp376;})); return; _LL214: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp377=( char*)"float"; struct _tagged_string
_temp378; _temp378.curr= _temp377; _temp378.base= _temp377; _temp378.last_plus_one=
_temp377 + 6; _temp378;})); return; _LL216: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp379=( char*)"double"; struct _tagged_string
_temp380; _temp380.curr= _temp379; _temp380.base= _temp379; _temp380.last_plus_one=
_temp379 + 7; _temp380;})); return; _LL218: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp312); return; _LL220: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp381=( char*)"struct"; struct _tagged_string
_temp382; _temp382.curr= _temp381; _temp382.base= _temp381; _temp382.last_plus_one=
_temp381 + 7; _temp382;})); Cyc_Absyndump_dumptps( _temp316); return; _LL222:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp383=( char*)"struct";
struct _tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 7; _temp384;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp385= _temp324; if( _temp385 == 0){
_throw( Null_Exception);} _temp385;})); Cyc_Absyndump_dumptps( _temp322);
return; _LL224: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp386=(
char*)"union"; struct _tagged_string _temp387; _temp387.curr= _temp386; _temp387.base=
_temp386; _temp387.last_plus_one= _temp386 + 6; _temp387;})); Cyc_Absyndump_dumptps(
_temp328); return; _LL226: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp388=( char*)"union"; struct _tagged_string _temp389; _temp389.curr=
_temp388; _temp389.base= _temp388; _temp389.last_plus_one= _temp388 + 6;
_temp389;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct _tuple0*
_temp390= _temp336; if( _temp390 == 0){ _throw( Null_Exception);} _temp390;}));
Cyc_Absyndump_dumptps( _temp334); return; _LL228: Cyc_Absyndump_dumpqvar(
_temp342), Cyc_Absyndump_dumptps( _temp340); return; _LL230: Cyc_Absyndump_dumprgn(
_temp344); return; _LL232: return; _LL234: return; _LL236: return; _LL176:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*) vo->v);}} void Cyc_Absyndump_dumpfunarg( struct _tuple1*
t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)(* t).f2,(*
t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args= args->tl){ Cyc_Absyndump_dumpfunarg(( struct
_tuple1*) args->hd); if( args->tl != 0? 1: varargs){ Cyc_Absyndump_dump_char((
int)',');}} if( varargs){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp391=( char*)"..."; struct _tagged_string _temp392; _temp392.curr= _temp391;
_temp392.base= _temp391; _temp392.last_plus_one= _temp391 + 4; _temp392;}));}
if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)
effopt->v);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp(
void* t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(
0, t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp393= d; struct Cyc_Absyn_Exp* _temp399; struct
_tagged_string* _temp401; _LL395: if((( struct _enum_struct*) _temp393)->tag ==
Cyc_Absyn_ArrayElement_tag){ _LL400: _temp399=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp393)->f1; goto _LL396;} else{ goto _LL397;}
_LL397: if((( struct _enum_struct*) _temp393)->tag == Cyc_Absyn_FieldName_tag){
_LL402: _temp401=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp393)->f1; goto _LL398;} else{ goto _LL394;} _LL396: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp403=( char*)".["; struct _tagged_string
_temp404; _temp404.curr= _temp403; _temp404.base= _temp403; _temp404.last_plus_one=
_temp403 + 3; _temp404;})); Cyc_Absyndump_dumpexp( _temp399); Cyc_Absyndump_dump_char((
int)']'); goto _LL394; _LL398: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp401); goto _LL394; _LL394:;} void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp405=(
char*)""; struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base=
_temp405; _temp406.last_plus_one= _temp405 + 1; _temp406;}),( struct
_tagged_string)({ char* _temp407=( char*)"="; struct _tagged_string _temp408;
_temp408.curr= _temp407; _temp408.base= _temp407; _temp408.last_plus_one=
_temp407 + 2; _temp408;}),( struct _tagged_string)({ char* _temp409=( char*)"=";
struct _tagged_string _temp410; _temp410.curr= _temp409; _temp410.base= _temp409;
_temp410.last_plus_one= _temp409 + 2; _temp410;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp411=(
char*)""; struct _tagged_string _temp412; _temp412.curr= _temp411; _temp412.base=
_temp411; _temp412.last_plus_one= _temp411 + 1; _temp412;}),( struct
_tagged_string)({ char* _temp413=( char*)""; struct _tagged_string _temp414;
_temp414.curr= _temp413; _temp414.base= _temp413; _temp414.last_plus_one=
_temp413 + 1; _temp414;}),( struct _tagged_string)({ char* _temp415=( char*)",";
struct _tagged_string _temp416; _temp416.curr= _temp415; _temp416.base= _temp415;
_temp416.last_plus_one= _temp415 + 2; _temp416;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp417=( char*)"("; struct _tagged_string _temp418; _temp418.curr= _temp417;
_temp418.base= _temp417; _temp418.last_plus_one= _temp417 + 2; _temp418;}));}{
void* _temp419=( void*) e->r; void* _temp509; char _temp511; void* _temp513;
void* _temp515; short _temp517; void* _temp519; void* _temp521; int _temp523;
void* _temp525; void* _temp527; int _temp529; void* _temp531; void* _temp533;
long long _temp535; void* _temp537; void* _temp539; struct _tagged_string
_temp541; void* _temp543; void* _temp545; struct _tagged_string _temp547; struct
_tuple0* _temp549; void* _temp551; struct _tuple0* _temp553; struct Cyc_List_List*
_temp555; void* _temp557; struct Cyc_Absyn_Exp* _temp559; struct Cyc_Core_Opt*
_temp561; struct Cyc_Absyn_Exp* _temp563; void* _temp565; struct Cyc_Absyn_Exp*
_temp567; void* _temp569; struct Cyc_Absyn_Exp* _temp571; void* _temp573; struct
Cyc_Absyn_Exp* _temp575; void* _temp577; struct Cyc_Absyn_Exp* _temp579; struct
Cyc_Absyn_Exp* _temp581; struct Cyc_Absyn_Exp* _temp583; struct Cyc_Absyn_Exp*
_temp585; struct Cyc_Absyn_Exp* _temp587; struct Cyc_Absyn_Exp* _temp589; struct
Cyc_List_List* _temp591; struct Cyc_Absyn_Exp* _temp593; struct Cyc_List_List*
_temp595; struct Cyc_Absyn_Exp* _temp597; struct Cyc_Absyn_Exp* _temp599; struct
Cyc_Absyn_Exp* _temp601; struct Cyc_List_List* _temp603; struct Cyc_Absyn_Exp*
_temp605; struct Cyc_Absyn_Exp* _temp607; void* _temp609; struct Cyc_Absyn_Exp*
_temp611; struct Cyc_Absyn_Exp* _temp613; void* _temp615; struct Cyc_Absyn_Exp*
_temp617; struct Cyc_Absyn_Exp* _temp619; struct _tagged_string* _temp621;
struct Cyc_Absyn_Exp* _temp623; struct _tagged_string* _temp625; struct Cyc_Absyn_Exp*
_temp627; struct Cyc_Absyn_Exp* _temp629; struct Cyc_Absyn_Exp* _temp631; struct
Cyc_List_List* _temp633; struct Cyc_List_List* _temp635; struct _tuple1*
_temp637; struct Cyc_List_List* _temp639; struct Cyc_Absyn_Exp* _temp641; struct
Cyc_Absyn_Exp* _temp643; struct Cyc_Absyn_Vardecl* _temp645; struct Cyc_Absyn_Structdecl*
_temp647; struct Cyc_List_List* _temp649; struct Cyc_Core_Opt* _temp651; struct
_tuple0* _temp653; struct Cyc_Absyn_Enumfield* _temp655; struct Cyc_Absyn_Enumdecl*
_temp657; struct Cyc_List_List* _temp659; struct Cyc_Core_Opt* _temp661; struct
Cyc_Core_Opt* _temp663; struct Cyc_Absyn_Enumfield* _temp665; struct Cyc_Absyn_Xenumdecl*
_temp667; struct Cyc_List_List* _temp669; struct Cyc_Core_Opt* _temp671; void*
_temp673; struct Cyc_List_List* _temp675; struct Cyc_Core_Opt* _temp677; struct
Cyc_Absyn_Stmt* _temp679; struct Cyc_Absyn_Fndecl* _temp681; struct Cyc_Absyn_Exp*
_temp683; _LL421: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){
_LL510: _temp509=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp509 > 1u?(( struct _enum_struct*) _temp509)->tag == Cyc_Absyn_Char_c_tag:
0){ _LL514: _temp513=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp509)->f1;
goto _LL512; _LL512: _temp511=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp509)->f2; goto _LL422;} else{ goto _LL423;}} else{ goto _LL423;} _LL423:
if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){ _LL516:
_temp515=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp515 > 1u?(( struct _enum_struct*) _temp515)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL520: _temp519=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp515)->f1;
goto _LL518; _LL518: _temp517=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp515)->f2; goto _LL424;} else{ goto _LL425;}} else{ goto _LL425;} _LL425:
if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){ _LL522:
_temp521=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp521 > 1u?(( struct _enum_struct*) _temp521)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL526: _temp525=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp521)->f1;
if( _temp525 == Cyc_Absyn_Signed){ goto _LL524;} else{ goto _LL427;} _LL524:
_temp523=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp521)->f2; goto _LL426;}
else{ goto _LL427;}} else{ goto _LL427;} _LL427: if((( struct _enum_struct*)
_temp419)->tag == Cyc_Absyn_Const_e_tag){ _LL528: _temp527=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp419)->f1; if(( unsigned int) _temp527 > 1u?(( struct _enum_struct*)
_temp527)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL532: _temp531=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp527)->f1; if( _temp531 == Cyc_Absyn_Unsigned){
goto _LL530;} else{ goto _LL429;} _LL530: _temp529=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp527)->f2; goto _LL428;} else{ goto _LL429;}} else{ goto _LL429;} _LL429:
if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){ _LL534:
_temp533=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp533 > 1u?(( struct _enum_struct*) _temp533)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL538: _temp537=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp533)->f1;
goto _LL536; _LL536: _temp535=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp533)->f2; goto _LL430;} else{ goto _LL431;}} else{ goto _LL431;} _LL431:
if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){ _LL540:
_temp539=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp539 > 1u?(( struct _enum_struct*) _temp539)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL542: _temp541=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp539)->f1; goto _LL432;} else{ goto _LL433;}} else{ goto _LL433;} _LL433:
if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){ _LL544:
_temp543=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if(
_temp543 == Cyc_Absyn_Null_c){ goto _LL434;} else{ goto _LL435;}} else{ goto
_LL435;} _LL435: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Const_e_tag){
_LL546: _temp545=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp545 > 1u?(( struct _enum_struct*) _temp545)->tag == Cyc_Absyn_String_c_tag:
0){ _LL548: _temp547=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp545)->f1; goto _LL436;} else{ goto _LL437;}} else{ goto _LL437;} _LL437:
if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL550:
_temp549=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp419)->f1;
goto _LL438;} else{ goto _LL439;} _LL439: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Var_e_tag){ _LL554: _temp553=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp419)->f1; goto _LL552; _LL552: _temp551=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp419)->f2; goto _LL440;} else{ goto _LL441;} _LL441: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Primop_e_tag){ _LL558: _temp557=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp419)->f1; goto _LL556; _LL556:
_temp555=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp419)->f2;
goto _LL442;} else{ goto _LL443;} _LL443: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL564: _temp563=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp419)->f1; goto _LL562; _LL562: _temp561=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp419)->f2; goto
_LL560; _LL560: _temp559=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp419)->f3; goto _LL444;} else{ goto _LL445;} _LL445: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Increment_e_tag){ _LL568: _temp567=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp419)->f1;
goto _LL566; _LL566: _temp565=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f2; if( _temp565 == Cyc_Absyn_PreInc){ goto _LL446;} else{ goto
_LL447;}} else{ goto _LL447;} _LL447: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Increment_e_tag){ _LL572: _temp571=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp419)->f1; goto _LL570; _LL570:
_temp569=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp419)->f2; if(
_temp569 == Cyc_Absyn_PreDec){ goto _LL448;} else{ goto _LL449;}} else{ goto
_LL449;} _LL449: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Increment_e_tag){
_LL576: _temp575=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f1; goto _LL574; _LL574: _temp573=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f2; if( _temp573 == Cyc_Absyn_PostInc){ goto _LL450;} else{ goto
_LL451;}} else{ goto _LL451;} _LL451: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Increment_e_tag){ _LL580: _temp579=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp419)->f1; goto _LL578; _LL578:
_temp577=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp419)->f2; if(
_temp577 == Cyc_Absyn_PostDec){ goto _LL452;} else{ goto _LL453;}} else{ goto
_LL453;} _LL453: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Conditional_e_tag){
_LL586: _temp585=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp419)->f1; goto _LL584; _LL584: _temp583=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp419)->f2; goto _LL582; _LL582: _temp581=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp419)->f3;
goto _LL454;} else{ goto _LL455;} _LL455: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL590: _temp589=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp419)->f1; goto _LL588; _LL588: _temp587=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp419)->f2; goto
_LL456;} else{ goto _LL457;} _LL457: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL594: _temp593=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp419)->f1; goto _LL592; _LL592:
_temp591=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp419)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_FnCall_e_tag){ _LL598: _temp597=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp419)->f1; goto
_LL596; _LL596: _temp595=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp419)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Throw_e_tag){ _LL600: _temp599=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp419)->f1; goto
_LL462;} else{ goto _LL463;} _LL463: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL602: _temp601=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp419)->f1; goto _LL464;} else{
goto _LL465;} _LL465: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL606: _temp605=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp419)->f1; goto _LL604; _LL604: _temp603=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp419)->f2; goto _LL466;} else{ goto _LL467;}
_LL467: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Cast_e_tag){
_LL610: _temp609=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp419)->f1; goto
_LL608; _LL608: _temp607=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp419)->f2; goto _LL468;} else{ goto _LL469;} _LL469: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Address_e_tag){ _LL612: _temp611=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp419)->f1; goto
_LL470;} else{ goto _LL471;} _LL471: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_New_e_tag){ _LL614: _temp613=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp419)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL616: _temp615=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp419)->f1; goto _LL474;}
else{ goto _LL475;} _LL475: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL618: _temp617=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp419)->f1; goto _LL476;} else{ goto _LL477;} _LL477: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Deref_e_tag){ _LL620: _temp619=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp419)->f1; goto
_LL478;} else{ goto _LL479;} _LL479: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL624: _temp623=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp419)->f1; goto _LL622; _LL622:
_temp621=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp419)->f2; goto _LL480;} else{ goto _LL481;} _LL481: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL628: _temp627=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp419)->f1;
goto _LL626; _LL626: _temp625=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp419)->f2; goto _LL482;} else{ goto _LL483;} _LL483: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Subscript_e_tag){ _LL632: _temp631=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp419)->f1;
goto _LL630; _LL630: _temp629=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp419)->f2; goto _LL484;} else{ goto _LL485;} _LL485: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Tuple_e_tag){ _LL634: _temp633=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp419)->f1; goto
_LL486;} else{ goto _LL487;} _LL487: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL638: _temp637=( struct _tuple1*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp419)->f1; goto _LL636; _LL636: _temp635=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp419)->f2;
goto _LL488;} else{ goto _LL489;} _LL489: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Array_e_tag){ _LL640: _temp639=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp419)->f1; goto _LL490;} else{ goto _LL491;}
_LL491: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL646: _temp645=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp419)->f1; goto _LL644; _LL644: _temp643=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp419)->f2; goto _LL642; _LL642: _temp641=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp419)->f3;
goto _LL492;} else{ goto _LL493;} _LL493: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Struct_e_tag){ _LL654: _temp653=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp419)->f1; goto _LL652; _LL652: _temp651=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp419)->f2; goto _LL650; _LL650: _temp649=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp419)->f3; goto
_LL648; _LL648: _temp647=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp419)->f4; goto _LL494;} else{ goto _LL495;} _LL495: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Enum_e_tag){ _LL664: _temp663=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp419)->f1; goto
_LL662; _LL662: _temp661=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp419)->f2; goto _LL660; _LL660: _temp659=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp419)->f3; goto _LL658; _LL658: _temp657=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp419)->f4; goto
_LL656; _LL656: _temp655=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp419)->f5; goto _LL496;} else{ goto _LL497;} _LL497: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Xenum_e_tag){ _LL672: _temp671=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp419)->f1; goto
_LL670; _LL670: _temp669=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp419)->f2; goto _LL668; _LL668: _temp667=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp419)->f3; goto _LL666; _LL666: _temp665=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp419)->f4;
goto _LL498;} else{ goto _LL499;} _LL499: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL674: _temp673=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp419)->f1; goto _LL500;} else{ goto _LL501;} _LL501: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL678:
_temp677=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp419)->f1; goto _LL676; _LL676: _temp675=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp419)->f2; goto _LL502;} else{ goto
_LL503;} _LL503: if((( struct _enum_struct*) _temp419)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL680: _temp679=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp419)->f1; goto _LL504;} else{ goto _LL505;} _LL505: if((( struct
_enum_struct*) _temp419)->tag == Cyc_Absyn_Codegen_e_tag){ _LL682: _temp681=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp419)->f1;
goto _LL506;} else{ goto _LL507;} _LL507: if((( struct _enum_struct*) _temp419)->tag
== Cyc_Absyn_Fill_e_tag){ _LL684: _temp683=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp419)->f1; goto _LL508;} else{ goto _LL420;} _LL422: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp511)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL420; _LL424: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp685=( char*)"(short)"; struct _tagged_string _temp686; _temp686.curr=
_temp685; _temp686.base= _temp685; _temp686.last_plus_one= _temp685 + 8;
_temp686;})); Cyc_Absyndump_dump_nospace( Cyc_Core_string_of_int(( int) _temp517));
goto _LL420; _LL426: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp523));
goto _LL420; _LL428: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp529)); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp687=( char*)"u"; struct _tagged_string
_temp688; _temp688.curr= _temp687; _temp688.base= _temp687; _temp688.last_plus_one=
_temp687 + 2; _temp688;})); goto _LL420; _LL430: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp689=( char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp690; _temp690.curr= _temp689; _temp690.base= _temp689;
_temp690.last_plus_one= _temp689 + 27; _temp690;})); goto _LL420; _LL432: Cyc_Absyndump_dump(
_temp541); goto _LL420; _LL434: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp691=( char*)"null"; struct _tagged_string _temp692; _temp692.curr=
_temp691; _temp692.base= _temp691; _temp692.last_plus_one= _temp691 + 5;
_temp692;})); goto _LL420; _LL436: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp547)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL420; _LL438: _temp553= _temp549; goto _LL440; _LL440: Cyc_Absyndump_dumpqvar(
_temp553); goto _LL420; _LL442: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp557); if( ! Cyc_Absyn_is_format_prim( _temp557)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp555)){ case 1: _LL693: if( _temp557 ==
Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp555->hd); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp695=(
char*)".size"; struct _tagged_string _temp696; _temp696.curr= _temp695; _temp696.base=
_temp695; _temp696.last_plus_one= _temp695 + 6; _temp696;}));} else{ Cyc_Absyndump_dump(
pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*) _temp555->hd);}
break; case 2: _LL694: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp555->hd); Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,(
struct Cyc_Absyn_Exp*)( _temp555->tl)->hd); break; default: _LL697:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp699=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp699->tag= Cyc_Core_Failure_tag;
_temp699->f1=( struct _tagged_string)({ char* _temp700=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 47; _temp701;});( struct _xenum_struct*)
_temp699;}));}} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp702=( char*)"("; struct _tagged_string
_temp703; _temp703.curr= _temp702; _temp703.base= _temp702; _temp703.last_plus_one=
_temp702 + 2; _temp703;})); Cyc_Absyndump_dumpexps_prec( 20, _temp555); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp704=( char*)")"; struct _tagged_string
_temp705; _temp705.curr= _temp704; _temp705.base= _temp704; _temp705.last_plus_one=
_temp704 + 2; _temp705;}));} goto _LL420;} _LL444: Cyc_Absyndump_dumpexp_prec(
myprec, _temp563); if( _temp561 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*) _temp561->v));} Cyc_Absyndump_dump_nospace(( struct _tagged_string)({
char* _temp706=( char*)"="; struct _tagged_string _temp707; _temp707.curr=
_temp706; _temp707.base= _temp706; _temp707.last_plus_one= _temp706 + 2;
_temp707;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp559); goto _LL420; _LL446:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp708=( char*)"++";
struct _tagged_string _temp709; _temp709.curr= _temp708; _temp709.base= _temp708;
_temp709.last_plus_one= _temp708 + 3; _temp709;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp567); goto _LL420; _LL448: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp710=( char*)"--"; struct _tagged_string _temp711;
_temp711.curr= _temp710; _temp711.base= _temp710; _temp711.last_plus_one=
_temp710 + 3; _temp711;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp571); goto
_LL420; _LL450: Cyc_Absyndump_dumpexp_prec( myprec, _temp575); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp712=( char*)"++"; struct _tagged_string
_temp713; _temp713.curr= _temp712; _temp713.base= _temp712; _temp713.last_plus_one=
_temp712 + 3; _temp713;})); goto _LL420; _LL452: Cyc_Absyndump_dumpexp_prec(
myprec, _temp579); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp714=(
char*)"--"; struct _tagged_string _temp715; _temp715.curr= _temp714; _temp715.base=
_temp714; _temp715.last_plus_one= _temp714 + 3; _temp715;})); goto _LL420;
_LL454: Cyc_Absyndump_dumpexp_prec( myprec, _temp585); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp583); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp581); goto _LL420; _LL456:
Cyc_Absyndump_dumpexp_prec( myprec, _temp589); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp587); goto _LL420; _LL458: _temp597=
_temp593; _temp595= _temp591; goto _LL460; _LL460: Cyc_Absyndump_dumpexp_prec(
myprec, _temp597); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp716=( char*)"("; struct _tagged_string _temp717; _temp717.curr= _temp716;
_temp717.base= _temp716; _temp717.last_plus_one= _temp716 + 2; _temp717;})); Cyc_Absyndump_dumpexps_prec(
20, _temp595); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp718=( char*)")"; struct _tagged_string _temp719; _temp719.curr= _temp718;
_temp719.base= _temp718; _temp719.last_plus_one= _temp718 + 2; _temp719;}));
goto _LL420; _LL462: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp720=( char*)"throw"; struct _tagged_string _temp721; _temp721.curr=
_temp720; _temp721.base= _temp720; _temp721.last_plus_one= _temp720 + 6;
_temp721;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp599); goto _LL420; _LL464:
_temp605= _temp601; goto _LL466; _LL466: Cyc_Absyndump_dumpexp_prec( inprec,
_temp605); goto _LL420; _LL468: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp609); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp607); goto _LL420; _LL470: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp611); goto _LL420; _LL472: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp722=( char*)"new "; struct _tagged_string _temp723;
_temp723.curr= _temp722; _temp723.base= _temp722; _temp723.last_plus_one=
_temp722 + 5; _temp723;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp613); goto
_LL420; _LL474: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp724=(
char*)"sizeof("; struct _tagged_string _temp725; _temp725.curr= _temp724;
_temp725.base= _temp724; _temp725.last_plus_one= _temp724 + 8; _temp725;})); Cyc_Absyndump_dumptyp(
_temp615); Cyc_Absyndump_dump_char(( int)')'); goto _LL420; _LL476: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp726=( char*)"sizeof("; struct _tagged_string
_temp727; _temp727.curr= _temp726; _temp727.base= _temp726; _temp727.last_plus_one=
_temp726 + 8; _temp727;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp617); Cyc_Absyndump_dump_char((
int)')'); goto _LL420; _LL478: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp619); goto _LL420; _LL480: Cyc_Absyndump_dumpexp_prec( myprec,
_temp623); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp621); goto _LL420; _LL482: Cyc_Absyndump_dumpexp_prec( myprec, _temp627);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp728=( char*)"->";
struct _tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 3; _temp729;})); Cyc_Absyndump_dump_nospace(*
_temp625); goto _LL420; _LL484: Cyc_Absyndump_dumpexp_prec( myprec, _temp631);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp629); Cyc_Absyndump_dump_char((
int)']'); goto _LL420; _LL486: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp730=( char*)"$("; struct _tagged_string _temp731; _temp731.curr=
_temp730; _temp731.base= _temp730; _temp731.last_plus_one= _temp730 + 3;
_temp731;})); Cyc_Absyndump_dumpexps_prec( 20, _temp633); Cyc_Absyndump_dump_char((
int)')'); goto _LL420; _LL488: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp637).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp635,( struct _tagged_string)({ char* _temp732=( char*)"{"; struct
_tagged_string _temp733; _temp733.curr= _temp732; _temp733.base= _temp732;
_temp733.last_plus_one= _temp732 + 2; _temp733;}),( struct _tagged_string)({
char* _temp734=( char*)"}"; struct _tagged_string _temp735; _temp735.curr=
_temp734; _temp735.base= _temp734; _temp735.last_plus_one= _temp734 + 2;
_temp735;}),( struct _tagged_string)({ char* _temp736=( char*)","; struct
_tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 2; _temp737;})); goto _LL420; _LL490:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp639,( struct _tagged_string)({ char* _temp738=( char*)"{";
struct _tagged_string _temp739; _temp739.curr= _temp738; _temp739.base= _temp738;
_temp739.last_plus_one= _temp738 + 2; _temp739;}),( struct _tagged_string)({
char* _temp740=( char*)"}"; struct _tagged_string _temp741; _temp741.curr=
_temp740; _temp741.base= _temp740; _temp741.last_plus_one= _temp740 + 2;
_temp741;}),( struct _tagged_string)({ char* _temp742=( char*)","; struct
_tagged_string _temp743; _temp743.curr= _temp742; _temp743.base= _temp742;
_temp743.last_plus_one= _temp742 + 2; _temp743;})); goto _LL420; _LL492: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp744=( char*)"new {for"; struct
_tagged_string _temp745; _temp745.curr= _temp744; _temp745.base= _temp744;
_temp745.last_plus_one= _temp744 + 9; _temp745;})); Cyc_Absyndump_dump_str((*
_temp645->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp643); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp641);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL420; _LL494: Cyc_Absyndump_dumpqvar(
_temp653);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp649,( struct
_tagged_string)({ char* _temp746=( char*)"{"; struct _tagged_string _temp747;
_temp747.curr= _temp746; _temp747.base= _temp746; _temp747.last_plus_one=
_temp746 + 2; _temp747;}),( struct _tagged_string)({ char* _temp748=( char*)"}";
struct _tagged_string _temp749; _temp749.curr= _temp748; _temp749.base= _temp748;
_temp749.last_plus_one= _temp748 + 2; _temp749;}),( struct _tagged_string)({
char* _temp750=( char*)","; struct _tagged_string _temp751; _temp751.curr=
_temp750; _temp751.base= _temp750; _temp751.last_plus_one= _temp750 + 2;
_temp751;})); goto _LL420; _LL496: _temp669= _temp659; _temp665= _temp655; goto
_LL498; _LL498: Cyc_Absyndump_dumpqvar( _temp665->name); if( _temp669 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp669,( struct _tagged_string)({ char* _temp752=( char*)"(";
struct _tagged_string _temp753; _temp753.curr= _temp752; _temp753.base= _temp752;
_temp753.last_plus_one= _temp752 + 2; _temp753;}),( struct _tagged_string)({
char* _temp754=( char*)")"; struct _tagged_string _temp755; _temp755.curr=
_temp754; _temp755.base= _temp754; _temp755.last_plus_one= _temp754 + 2;
_temp755;}),( struct _tagged_string)({ char* _temp756=( char*)","; struct
_tagged_string _temp757; _temp757.curr= _temp756; _temp757.base= _temp756;
_temp757.last_plus_one= _temp756 + 2; _temp757;}));} goto _LL420; _LL500: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp758=( char*)"malloc(sizeof("; struct
_tagged_string _temp759; _temp759.curr= _temp758; _temp759.base= _temp758;
_temp759.last_plus_one= _temp758 + 15; _temp759;}));{ void* _temp760= _temp673;
void* _temp770; struct Cyc_Absyn_Enumfield* _temp772; struct Cyc_Absyn_Enumdecl*
_temp774; struct Cyc_Absyn_Enumfield* _temp776; struct Cyc_Absyn_Xenumdecl*
_temp778; struct _tuple0* _temp780; _LL762: if((( struct _enum_struct*) _temp760)->tag
== Cyc_Absyn_Typ_m_tag){ _LL771: _temp770=( void*)(( struct Cyc_Absyn_Typ_m_struct*)
_temp760)->f1; goto _LL763;} else{ goto _LL764;} _LL764: if((( struct
_enum_struct*) _temp760)->tag == Cyc_Absyn_Enum_m_tag){ _LL775: _temp774=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_m_struct*) _temp760)->f1;
goto _LL773; _LL773: _temp772=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_m_struct*)
_temp760)->f2; goto _LL765;} else{ goto _LL766;} _LL766: if((( struct
_enum_struct*) _temp760)->tag == Cyc_Absyn_Xenum_m_tag){ _LL779: _temp778=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_m_struct*) _temp760)->f1;
goto _LL777; _LL777: _temp776=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_m_struct*)
_temp760)->f2; goto _LL767;} else{ goto _LL768;} _LL768: if((( struct
_enum_struct*) _temp760)->tag == Cyc_Absyn_Unresolved_m_tag){ _LL781: _temp780=(
struct _tuple0*)(( struct Cyc_Absyn_Unresolved_m_struct*) _temp760)->f1; goto
_LL769;} else{ goto _LL761;} _LL763: Cyc_Absyndump_dumptyp( _temp770); goto
_LL761; _LL765: _temp776= _temp772; goto _LL767; _LL767: _temp780= _temp776->name;
goto _LL769; _LL769: Cyc_Absyndump_dumpqvar( _temp780); goto _LL761; _LL761:;}
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp782=( char*)"))";
struct _tagged_string _temp783; _temp783.curr= _temp782; _temp783.base= _temp782;
_temp783.last_plus_one= _temp782 + 3; _temp783;})); goto _LL420; _LL502:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp675,( struct _tagged_string)({ char* _temp784=( char*)"{";
struct _tagged_string _temp785; _temp785.curr= _temp784; _temp785.base= _temp784;
_temp785.last_plus_one= _temp784 + 2; _temp785;}),( struct _tagged_string)({
char* _temp786=( char*)"}"; struct _tagged_string _temp787; _temp787.curr=
_temp786; _temp787.base= _temp786; _temp787.last_plus_one= _temp786 + 2;
_temp787;}),( struct _tagged_string)({ char* _temp788=( char*)","; struct
_tagged_string _temp789; _temp789.curr= _temp788; _temp789.base= _temp788;
_temp789.last_plus_one= _temp788 + 2; _temp789;})); goto _LL420; _LL504: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp790=( char*)"({"; struct _tagged_string
_temp791; _temp791.curr= _temp790; _temp791.base= _temp790; _temp791.last_plus_one=
_temp790 + 3; _temp791;})); Cyc_Absyndump_dumpstmt( _temp679); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp792=( char*)"})"; struct _tagged_string
_temp793; _temp793.curr= _temp792; _temp793.base= _temp792; _temp793.last_plus_one=
_temp792 + 3; _temp793;})); goto _LL420; _LL506: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp794=( char*)"codegen("; struct _tagged_string
_temp795; _temp795.curr= _temp794; _temp795.base= _temp794; _temp795.last_plus_one=
_temp794 + 9; _temp795;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp796=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp796->r=( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp797=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp797->tag= Cyc_Absyn_Fn_d_tag;
_temp797->f1= _temp681;( void*) _temp797;}); _temp796->loc= e->loc; _temp796;}));
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp798=( char*)")"; struct
_tagged_string _temp799; _temp799.curr= _temp798; _temp799.base= _temp798;
_temp799.last_plus_one= _temp798 + 2; _temp799;})); goto _LL420; _LL508: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp800=( char*)"fill("; struct _tagged_string
_temp801; _temp801.curr= _temp800; _temp801.base= _temp800; _temp801.last_plus_one=
_temp800 + 6; _temp801;})); Cyc_Absyndump_dumpexp( _temp683); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp802=( char*)")"; struct _tagged_string
_temp803; _temp803.curr= _temp802; _temp803.base= _temp802; _temp803.last_plus_one=
_temp802 + 2; _temp803;})); goto _LL420; _LL420:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause* c=( struct Cyc_Absyn_Switch_clause*)
scs->hd; if( c->where_clause == 0?( void*)( c->pattern)->r == Cyc_Absyn_Wild_p:
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp804=( char*)"default:";
struct _tagged_string _temp805; _temp805.curr= _temp804; _temp805.base= _temp804;
_temp805.last_plus_one= _temp804 + 9; _temp805;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp806=( char*)"case"; struct _tagged_string
_temp807; _temp807.curr= _temp806; _temp807.base= _temp806; _temp807.last_plus_one=
_temp806 + 5; _temp807;})); Cyc_Absyndump_dumppat( c->pattern); if( c->where_clause
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp808=( char*)"&&";
struct _tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 3; _temp809;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp810= c->where_clause; if(
_temp810 == 0){ _throw( Null_Exception);} _temp810;}));} Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp811=( char*)":"; struct _tagged_string
_temp812; _temp812.curr= _temp811; _temp812.base= _temp811; _temp812.last_plus_one=
_temp811 + 2; _temp812;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp813=( void*) s->r; struct Cyc_Absyn_Exp*
_temp855; struct Cyc_Absyn_Stmt* _temp857; struct Cyc_Absyn_Stmt* _temp859;
struct Cyc_Absyn_Exp* _temp861; struct Cyc_Absyn_Exp* _temp863; struct Cyc_Absyn_Stmt*
_temp865; struct Cyc_Absyn_Stmt* _temp867; struct Cyc_Absyn_Exp* _temp869;
struct Cyc_Absyn_Stmt* _temp871; struct _tuple2 _temp873; struct Cyc_Absyn_Stmt*
_temp875; struct Cyc_Absyn_Exp* _temp877; struct Cyc_Absyn_Stmt* _temp879;
struct Cyc_Absyn_Stmt* _temp881; struct Cyc_Absyn_Stmt* _temp883; struct
_tagged_string* _temp885; struct Cyc_Absyn_Switch_clause** _temp887; struct Cyc_List_List*
_temp889; struct Cyc_Absyn_Switch_clause** _temp891; struct Cyc_List_List*
_temp893; struct Cyc_Absyn_Stmt* _temp895; struct _tuple2 _temp897; struct Cyc_Absyn_Stmt*
_temp899; struct Cyc_Absyn_Exp* _temp901; struct _tuple2 _temp903; struct Cyc_Absyn_Stmt*
_temp905; struct Cyc_Absyn_Exp* _temp907; struct Cyc_Absyn_Exp* _temp909; struct
Cyc_List_List* _temp911; struct Cyc_Absyn_Exp* _temp913; struct Cyc_Absyn_Stmt*
_temp915; struct Cyc_Absyn_Decl* _temp917; struct Cyc_Absyn_Stmt* _temp919;
struct Cyc_Absyn_Stmt* _temp921; struct Cyc_Absyn_Stmt* _temp923; struct
_tagged_string* _temp925; struct _tuple2 _temp927; struct Cyc_Absyn_Stmt*
_temp929; struct Cyc_Absyn_Exp* _temp931; struct Cyc_Absyn_Stmt* _temp933;
struct Cyc_List_List* _temp935; struct Cyc_Absyn_Stmt* _temp937; _LL815: if(
_temp813 == Cyc_Absyn_Skip_s){ goto _LL816;} else{ goto _LL817;} _LL817: if((
unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL856: _temp855=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp813)->f1; goto _LL818;} else{ goto _LL819;} _LL819: if(( unsigned int)
_temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Seq_s_tag: 0){
_LL860: _temp859=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp813)->f1; goto _LL858; _LL858: _temp857=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp813)->f2; goto _LL820;} else{ goto _LL821;} _LL821:
if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL862: _temp861=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp813)->f1; if( _temp861 == 0){ goto _LL822;} else{ goto _LL823;}} else{ goto
_LL823;} _LL823: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*)
_temp813)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL864: _temp863=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp813)->f1; goto _LL824;} else{ goto
_LL825;} _LL825: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*)
_temp813)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL870: _temp869=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp813)->f1; goto _LL868; _LL868:
_temp867=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp813)->f2; goto _LL866; _LL866: _temp865=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp813)->f3; goto _LL826;} else{ goto _LL827;}
_LL827: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag
== Cyc_Absyn_While_s_tag: 0){ _LL874: _temp873=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp813)->f1; _LL878: _temp877= _temp873.f1; goto _LL876; _LL876: _temp875=
_temp873.f2; goto _LL872; _LL872: _temp871=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp813)->f2; goto _LL828;} else{ goto _LL829;}
_LL829: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL880: _temp879=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp813)->f1; goto _LL830;} else{ goto _LL831;}
_LL831: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag
== Cyc_Absyn_Continue_s_tag: 0){ _LL882: _temp881=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp813)->f1; goto _LL832;} else{ goto
_LL833;} _LL833: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*)
_temp813)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL886: _temp885=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp813)->f1; goto _LL884;
_LL884: _temp883=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp813)->f2; goto _LL834;} else{ goto _LL835;} _LL835: if(( unsigned int)
_temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL890: _temp889=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp813)->f1; if( _temp889 == 0){ goto _LL888;} else{ goto _LL837;} _LL888:
_temp887=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp813)->f2; goto _LL836;} else{ goto _LL837;} _LL837: if(( unsigned int)
_temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL894: _temp893=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp813)->f1; goto _LL892; _LL892: _temp891=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp813)->f2; goto _LL838;} else{ goto
_LL839;} _LL839: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*)
_temp813)->tag == Cyc_Absyn_For_s_tag: 0){ _LL910: _temp909=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp813)->f1; goto _LL904; _LL904: _temp903=(
struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp813)->f2; _LL908:
_temp907= _temp903.f1; goto _LL906; _LL906: _temp905= _temp903.f2; goto _LL898;
_LL898: _temp897=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp813)->f3;
_LL902: _temp901= _temp897.f1; goto _LL900; _LL900: _temp899= _temp897.f2; goto
_LL896; _LL896: _temp895=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp813)->f4; goto _LL840;} else{ goto _LL841;} _LL841: if(( unsigned int)
_temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL914: _temp913=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp813)->f1; goto _LL912; _LL912: _temp911=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp813)->f2; goto _LL842;} else{ goto _LL843;}
_LL843: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL918: _temp917=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp813)->f1; goto _LL916; _LL916: _temp915=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp813)->f2; goto _LL844;}
else{ goto _LL845;} _LL845: if(( unsigned int) _temp813 > 1u?(( struct
_enum_struct*) _temp813)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL920: _temp919=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp813)->f1; goto
_LL846;} else{ goto _LL847;} _LL847: if(( unsigned int) _temp813 > 1u?(( struct
_enum_struct*) _temp813)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL922: _temp921=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp813)->f1; goto
_LL848;} else{ goto _LL849;} _LL849: if(( unsigned int) _temp813 > 1u?(( struct
_enum_struct*) _temp813)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL926: _temp925=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp813)->f1; goto
_LL924; _LL924: _temp923=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp813)->f2; goto _LL850;} else{ goto _LL851;} _LL851: if(( unsigned int)
_temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL934: _temp933=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp813)->f1; goto _LL928; _LL928: _temp927=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp813)->f2; _LL932: _temp931= _temp927.f1; goto _LL930; _LL930: _temp929=
_temp927.f2; goto _LL852;} else{ goto _LL853;} _LL853: if(( unsigned int)
_temp813 > 1u?(( struct _enum_struct*) _temp813)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL938: _temp937=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp813)->f1; goto _LL936; _LL936: _temp935=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp813)->f2; goto _LL854;} else{ goto _LL814;}
_LL816: Cyc_Absyndump_dump_semi(); goto _LL814; _LL818: Cyc_Absyndump_dumpexp(
_temp855); Cyc_Absyndump_dump_semi(); goto _LL814; _LL820: if( Cyc_Absynpp_is_declaration(
_temp859)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp859);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp859);}
if( Cyc_Absynpp_is_declaration( _temp857)){ Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp857); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dumpstmt( _temp857);} goto _LL814; _LL822: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp939=( char*)"return;"; struct _tagged_string
_temp940; _temp940.curr= _temp939; _temp940.base= _temp939; _temp940.last_plus_one=
_temp939 + 8; _temp940;})); goto _LL814; _LL824: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp941=( char*)"return"; struct _tagged_string
_temp942; _temp942.curr= _temp941; _temp942.base= _temp941; _temp942.last_plus_one=
_temp941 + 7; _temp942;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp943= _temp863; if( _temp943 == 0){ _throw(
Null_Exception);} _temp943;})); Cyc_Absyndump_dump_semi(); goto _LL814; _LL826:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp944=( char*)"if(";
struct _tagged_string _temp945; _temp945.curr= _temp944; _temp945.base= _temp944;
_temp945.last_plus_one= _temp944 + 4; _temp945;})); Cyc_Absyndump_dumpexp(
_temp869); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp946=(
char*)"){"; struct _tagged_string _temp947; _temp947.curr= _temp946; _temp947.base=
_temp946; _temp947.last_plus_one= _temp946 + 3; _temp947;})); Cyc_Absyndump_dumpstmt(
_temp867); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp948=( void*) _temp865->r;
_LL950: if( _temp948 == Cyc_Absyn_Skip_s){ goto _LL951;} else{ goto _LL952;}
_LL952: goto _LL953; _LL951: goto _LL949; _LL953: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp954=( char*)"else{"; struct _tagged_string _temp955;
_temp955.curr= _temp954; _temp955.base= _temp954; _temp955.last_plus_one=
_temp954 + 6; _temp955;})); Cyc_Absyndump_dumpstmt( _temp865); Cyc_Absyndump_dump_char((
int)'}'); goto _LL949; _LL949:;} goto _LL814; _LL828: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp956=( char*)"while("; struct _tagged_string
_temp957; _temp957.curr= _temp956; _temp957.base= _temp956; _temp957.last_plus_one=
_temp956 + 7; _temp957;})); Cyc_Absyndump_dumpexp( _temp877); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp958=( char*)") {"; struct _tagged_string
_temp959; _temp959.curr= _temp958; _temp959.base= _temp958; _temp959.last_plus_one=
_temp958 + 4; _temp959;})); Cyc_Absyndump_dumpstmt( _temp871); Cyc_Absyndump_dump_char((
int)'}'); goto _LL814; _LL830: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp960=( char*)"break;"; struct _tagged_string _temp961; _temp961.curr=
_temp960; _temp961.base= _temp960; _temp961.last_plus_one= _temp960 + 7;
_temp961;})); goto _LL814; _LL832: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp962=( char*)"continue;"; struct _tagged_string _temp963; _temp963.curr=
_temp962; _temp963.base= _temp962; _temp963.last_plus_one= _temp962 + 10;
_temp963;})); goto _LL814; _LL834: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp964=( char*)"goto"; struct _tagged_string _temp965; _temp965.curr=
_temp964; _temp965.base= _temp964; _temp965.last_plus_one= _temp964 + 5;
_temp965;})); Cyc_Absyndump_dump_str( _temp885); Cyc_Absyndump_dump_semi(); goto
_LL814; _LL836: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp966=(
char*)"fallthru;"; struct _tagged_string _temp967; _temp967.curr= _temp966;
_temp967.base= _temp966; _temp967.last_plus_one= _temp966 + 10; _temp967;}));
goto _LL814; _LL838: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp968=( char*)"fallthru("; struct _tagged_string _temp969; _temp969.curr=
_temp968; _temp969.base= _temp968; _temp969.last_plus_one= _temp968 + 10;
_temp969;})); Cyc_Absyndump_dumpexps_prec( 20, _temp893); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp970=( char*)");"; struct _tagged_string
_temp971; _temp971.curr= _temp970; _temp971.base= _temp970; _temp971.last_plus_one=
_temp970 + 3; _temp971;})); goto _LL814; _LL840: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp972=( char*)"for("; struct _tagged_string _temp973;
_temp973.curr= _temp972; _temp973.base= _temp972; _temp973.last_plus_one=
_temp972 + 5; _temp973;})); Cyc_Absyndump_dumpexp( _temp909); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp907); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp901); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp974=(
char*)"){"; struct _tagged_string _temp975; _temp975.curr= _temp974; _temp975.base=
_temp974; _temp975.last_plus_one= _temp974 + 3; _temp975;})); Cyc_Absyndump_dumpstmt(
_temp895); Cyc_Absyndump_dump_char(( int)'}'); goto _LL814; _LL842: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp976=( char*)"switch("; struct _tagged_string
_temp977; _temp977.curr= _temp976; _temp977.base= _temp976; _temp977.last_plus_one=
_temp976 + 8; _temp977;})); Cyc_Absyndump_dumpexp( _temp913); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp978=( char*)"){"; struct _tagged_string
_temp979; _temp979.curr= _temp978; _temp979.base= _temp978; _temp979.last_plus_one=
_temp978 + 3; _temp979;})); Cyc_Absyndump_dumpswitchclauses( _temp911); Cyc_Absyndump_dump_char((
int)'}'); goto _LL814; _LL844: Cyc_Absyndump_dumpdecl( _temp917); Cyc_Absyndump_dumpstmt(
_temp915); goto _LL814; _LL846: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp980=( char*)"cut"; struct _tagged_string _temp981; _temp981.curr=
_temp980; _temp981.base= _temp980; _temp981.last_plus_one= _temp980 + 4;
_temp981;})); Cyc_Absyndump_dumpstmt( _temp919); goto _LL814; _LL848: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp982=( char*)"splice"; struct _tagged_string
_temp983; _temp983.curr= _temp982; _temp983.base= _temp982; _temp983.last_plus_one=
_temp982 + 7; _temp983;})); Cyc_Absyndump_dumpstmt( _temp921); goto _LL814;
_LL850: if( Cyc_Absynpp_is_declaration( _temp923)){ Cyc_Absyndump_dump_str(
_temp925); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp984=(
char*)": {"; struct _tagged_string _temp985; _temp985.curr= _temp984; _temp985.base=
_temp984; _temp985.last_plus_one= _temp984 + 4; _temp985;})); Cyc_Absyndump_dumpstmt(
_temp923); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dump_str(
_temp925); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt( _temp923);}
goto _LL814; _LL852: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp986=( char*)"do {"; struct _tagged_string _temp987; _temp987.curr= _temp986;
_temp987.base= _temp986; _temp987.last_plus_one= _temp986 + 5; _temp987;})); Cyc_Absyndump_dumpstmt(
_temp933); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp988=(
char*)"} while ("; struct _tagged_string _temp989; _temp989.curr= _temp988;
_temp989.base= _temp988; _temp989.last_plus_one= _temp988 + 10; _temp989;}));
Cyc_Absyndump_dumpexp( _temp931); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp990=( char*)");"; struct _tagged_string _temp991;
_temp991.curr= _temp990; _temp991.base= _temp990; _temp991.last_plus_one=
_temp990 + 3; _temp991;})); goto _LL814; _LL854: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp992=( char*)"try"; struct _tagged_string _temp993;
_temp993.curr= _temp992; _temp993.base= _temp992; _temp993.last_plus_one=
_temp992 + 4; _temp993;})); Cyc_Absyndump_dumpstmt( _temp937); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp994=( char*)"catch {"; struct _tagged_string
_temp995; _temp995.curr= _temp994; _temp995.base= _temp994; _temp995.last_plus_one=
_temp994 + 8; _temp995;})); Cyc_Absyndump_dumpswitchclauses( _temp935); Cyc_Absyndump_dump_char((
int)'}'); goto _LL814; _LL814:;} void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char* _temp996=(
char*)""; struct _tagged_string _temp997; _temp997.curr= _temp996; _temp997.base=
_temp996; _temp997.last_plus_one= _temp996 + 1; _temp997;}),( struct
_tagged_string)({ char* _temp998=( char*)"="; struct _tagged_string _temp999;
_temp999.curr= _temp998; _temp999.base= _temp998; _temp999.last_plus_one=
_temp998 + 2; _temp999;}),( struct _tagged_string)({ char* _temp1000=( char*)"=";
struct _tagged_string _temp1001; _temp1001.curr= _temp1000; _temp1001.base=
_temp1000; _temp1001.last_plus_one= _temp1000 + 2; _temp1001;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1002=(
void*) p->r; int _temp1036; void* _temp1038; int _temp1040; void* _temp1042;
char _temp1044; struct _tagged_string _temp1046; struct Cyc_Absyn_Vardecl*
_temp1048; struct Cyc_List_List* _temp1050; struct Cyc_Absyn_Pat* _temp1052;
struct Cyc_Absyn_Vardecl* _temp1054; struct _tuple0* _temp1056; struct Cyc_List_List*
_temp1058; struct Cyc_List_List* _temp1060; struct _tuple0* _temp1062; struct
Cyc_List_List* _temp1064; struct Cyc_List_List* _temp1066; struct _tuple0*
_temp1068; struct Cyc_List_List* _temp1070; struct Cyc_List_List* _temp1072;
struct Cyc_Core_Opt* _temp1074; struct Cyc_Absyn_Structdecl* _temp1076; struct
Cyc_Absyn_Enumfield* _temp1078; struct Cyc_Absyn_Enumdecl* _temp1080; struct Cyc_List_List*
_temp1082; struct Cyc_List_List* _temp1084; struct Cyc_Core_Opt* _temp1086;
struct _tuple0* _temp1088; struct Cyc_Absyn_Enumfield* _temp1090; struct Cyc_Absyn_Xenumdecl*
_temp1092; struct Cyc_List_List* _temp1094; struct Cyc_List_List* _temp1096;
struct _tuple0* _temp1098; _LL1004: if( _temp1002 == Cyc_Absyn_Wild_p){ goto
_LL1005;} else{ goto _LL1006;} _LL1006: if( _temp1002 == Cyc_Absyn_Null_p){ goto
_LL1007;} else{ goto _LL1008;} _LL1008: if(( unsigned int) _temp1002 > 2u?((
struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1039:
_temp1038=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1002)->f1; if(
_temp1038 == Cyc_Absyn_Signed){ goto _LL1037;} else{ goto _LL1010;} _LL1037:
_temp1036=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1002)->f2; goto _LL1009;}
else{ goto _LL1010;} _LL1010: if(( unsigned int) _temp1002 > 2u?(( struct
_enum_struct*) _temp1002)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1043: _temp1042=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1002)->f1; if( _temp1042 == Cyc_Absyn_Unsigned){
goto _LL1041;} else{ goto _LL1012;} _LL1041: _temp1040=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1002)->f2; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1045: _temp1044=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1002)->f1;
goto _LL1013;} else{ goto _LL1014;} _LL1014: if(( unsigned int) _temp1002 > 2u?((
struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1047:
_temp1046=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1002)->f1;
goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int) _temp1002 > 2u?((
struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1049:
_temp1048=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1002)->f1; goto _LL1017;} else{ goto _LL1018;} _LL1018: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1051: _temp1050=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1002)->f1; goto _LL1019;} else{ goto _LL1020;} _LL1020: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1053: _temp1052=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1002)->f1; goto _LL1021;} else{ goto _LL1022;} _LL1022: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1055: _temp1054=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1002)->f1; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1057: _temp1056=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1002)->f1; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1063: _temp1062=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1002)->f1; goto _LL1061; _LL1061: _temp1060=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1002)->f2; goto _LL1059; _LL1059:
_temp1058=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1002)->f3; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1069: _temp1068=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1002)->f1; goto _LL1067; _LL1067: _temp1066=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1002)->f2; goto _LL1065; _LL1065:
_temp1064=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1002)->f3; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( unsigned int)
_temp1002 > 2u?(( struct _enum_struct*) _temp1002)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1077: _temp1076=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1002)->f1; goto _LL1075; _LL1075: _temp1074=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1002)->f2; goto _LL1073; _LL1073:
_temp1072=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1002)->f3; goto _LL1071; _LL1071: _temp1070=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1002)->f4; goto _LL1031;} else{ goto
_LL1032;} _LL1032: if(( unsigned int) _temp1002 > 2u?(( struct _enum_struct*)
_temp1002)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1089: _temp1088=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1002)->f1; goto _LL1087; _LL1087:
_temp1086=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1002)->f2;
goto _LL1085; _LL1085: _temp1084=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1002)->f3; goto _LL1083; _LL1083: _temp1082=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1002)->f4; goto _LL1081; _LL1081:
_temp1080=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1002)->f5; goto _LL1079; _LL1079: _temp1078=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1002)->f6; goto _LL1033;} else{ goto
_LL1034;} _LL1034: if(( unsigned int) _temp1002 > 2u?(( struct _enum_struct*)
_temp1002)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1099: _temp1098=( struct
_tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1002)->f1; goto _LL1097;
_LL1097: _temp1096=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1002)->f2; goto _LL1095; _LL1095: _temp1094=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1002)->f3; goto _LL1093; _LL1093:
_temp1092=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1002)->f4; goto _LL1091; _LL1091: _temp1090=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1002)->f5; goto _LL1035;} else{ goto
_LL1003;} _LL1005: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1003; _LL1007:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1100=( char*)"null";
struct _tagged_string _temp1101; _temp1101.curr= _temp1100; _temp1101.base=
_temp1100; _temp1101.last_plus_one= _temp1100 + 5; _temp1101;})); goto _LL1003;
_LL1009: Cyc_Absyndump_dump( xprintf("%d", _temp1036)); goto _LL1003; _LL1011:
Cyc_Absyndump_dump( xprintf("%u",( unsigned int) _temp1040)); goto _LL1003;
_LL1013: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1102=( char*)"'";
struct _tagged_string _temp1103; _temp1103.curr= _temp1102; _temp1103.base=
_temp1102; _temp1103.last_plus_one= _temp1102 + 2; _temp1103;})); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp1044)); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1104=( char*)"'"; struct _tagged_string _temp1105;
_temp1105.curr= _temp1104; _temp1105.base= _temp1104; _temp1105.last_plus_one=
_temp1104 + 2; _temp1105;})); goto _LL1003; _LL1015: Cyc_Absyndump_dump(
_temp1046); goto _LL1003; _LL1017: Cyc_Absyndump_dumpqvar( _temp1048->name);
goto _LL1003; _LL1019:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1050,( struct
_tagged_string)({ char* _temp1106=( char*)"$("; struct _tagged_string _temp1107;
_temp1107.curr= _temp1106; _temp1107.base= _temp1106; _temp1107.last_plus_one=
_temp1106 + 3; _temp1107;}),( struct _tagged_string)({ char* _temp1108=( char*)")";
struct _tagged_string _temp1109; _temp1109.curr= _temp1108; _temp1109.base=
_temp1108; _temp1109.last_plus_one= _temp1108 + 2; _temp1109;}),( struct
_tagged_string)({ char* _temp1110=( char*)","; struct _tagged_string _temp1111;
_temp1111.curr= _temp1110; _temp1111.base= _temp1110; _temp1111.last_plus_one=
_temp1110 + 2; _temp1111;})); goto _LL1003; _LL1021: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1112=( char*)"&"; struct _tagged_string _temp1113;
_temp1113.curr= _temp1112; _temp1113.base= _temp1112; _temp1113.last_plus_one=
_temp1112 + 2; _temp1113;})); Cyc_Absyndump_dumppat( _temp1052); goto _LL1003;
_LL1023: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1114=( char*)"*";
struct _tagged_string _temp1115; _temp1115.curr= _temp1114; _temp1115.base=
_temp1114; _temp1115.last_plus_one= _temp1114 + 2; _temp1115;})); Cyc_Absyndump_dumpqvar(
_temp1054->name); goto _LL1003; _LL1025: Cyc_Absyndump_dumpqvar( _temp1056);
goto _LL1003; _LL1027: Cyc_Absyndump_dumpqvar( _temp1062); Cyc_Absyndump_dumptvars(
_temp1060);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1058,( struct
_tagged_string)({ char* _temp1116=( char*)"("; struct _tagged_string _temp1117;
_temp1117.curr= _temp1116; _temp1117.base= _temp1116; _temp1117.last_plus_one=
_temp1116 + 2; _temp1117;}),( struct _tagged_string)({ char* _temp1118=( char*)")";
struct _tagged_string _temp1119; _temp1119.curr= _temp1118; _temp1119.base=
_temp1118; _temp1119.last_plus_one= _temp1118 + 2; _temp1119;}),( struct
_tagged_string)({ char* _temp1120=( char*)","; struct _tagged_string _temp1121;
_temp1121.curr= _temp1120; _temp1121.base= _temp1120; _temp1121.last_plus_one=
_temp1120 + 2; _temp1121;})); goto _LL1003; _LL1029: Cyc_Absyndump_dumpqvar(
_temp1068); Cyc_Absyndump_dumptvars( _temp1066);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1064,( struct _tagged_string)({ char* _temp1122=( char*)"{"; struct
_tagged_string _temp1123; _temp1123.curr= _temp1122; _temp1123.base= _temp1122;
_temp1123.last_plus_one= _temp1122 + 2; _temp1123;}),( struct _tagged_string)({
char* _temp1124=( char*)"}"; struct _tagged_string _temp1125; _temp1125.curr=
_temp1124; _temp1125.base= _temp1124; _temp1125.last_plus_one= _temp1124 + 2;
_temp1125;}),( struct _tagged_string)({ char* _temp1126=( char*)","; struct
_tagged_string _temp1127; _temp1127.curr= _temp1126; _temp1127.base= _temp1126;
_temp1127.last_plus_one= _temp1126 + 2; _temp1127;})); goto _LL1003; _LL1031:
if( _temp1076->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1076->name)->v);}
Cyc_Absyndump_dumptvars( _temp1072);(( void(*)( void(* f)( struct _tuple8*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1070,( struct _tagged_string)({ char* _temp1128=( char*)"{"; struct
_tagged_string _temp1129; _temp1129.curr= _temp1128; _temp1129.base= _temp1128;
_temp1129.last_plus_one= _temp1128 + 2; _temp1129;}),( struct _tagged_string)({
char* _temp1130=( char*)"}"; struct _tagged_string _temp1131; _temp1131.curr=
_temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one= _temp1130 + 2;
_temp1131;}),( struct _tagged_string)({ char* _temp1132=( char*)","; struct
_tagged_string _temp1133; _temp1133.curr= _temp1132; _temp1133.base= _temp1132;
_temp1133.last_plus_one= _temp1132 + 2; _temp1133;})); goto _LL1003; _LL1033:
_temp1098= _temp1088; _temp1096= _temp1084; _temp1094= _temp1082; goto _LL1035;
_LL1035: Cyc_Absyndump_dumpqvar( _temp1098); Cyc_Absyndump_dumptvars( _temp1096);
if( _temp1094 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1094,( struct
_tagged_string)({ char* _temp1134=( char*)"("; struct _tagged_string _temp1135;
_temp1135.curr= _temp1134; _temp1135.base= _temp1134; _temp1135.last_plus_one=
_temp1134 + 2; _temp1135;}),( struct _tagged_string)({ char* _temp1136=( char*)")";
struct _tagged_string _temp1137; _temp1137.curr= _temp1136; _temp1137.base=
_temp1136; _temp1137.last_plus_one= _temp1136 + 2; _temp1137;}),( struct
_tagged_string)({ char* _temp1138=( char*)","; struct _tagged_string _temp1139;
_temp1139.curr= _temp1138; _temp1139.base= _temp1138; _temp1139.last_plus_one=
_temp1138 + 2; _temp1139;}));} goto _LL1003; _LL1003:;} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1140= ef->tag; if( _temp1140 == 0){ _throw(
Null_Exception);} _temp1140;}));} if( ef->typs != 0){ Cyc_Absyndump_dumpargs( ef->typs);}}
void Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)(
void(* f)( struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields,( struct _tagged_string)({ char* _temp1141=( char*)","; struct
_tagged_string _temp1142; _temp1142.curr= _temp1141; _temp1142.base= _temp1141;
_temp1142.last_plus_one= _temp1141 + 2; _temp1142;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields= fields->tl){ struct
Cyc_Absyn_Structfield _temp1145; struct Cyc_List_List* _temp1146; struct Cyc_Core_Opt*
_temp1148; void* _temp1150; struct Cyc_Absyn_Tqual* _temp1152; struct
_tagged_string* _temp1154; struct Cyc_Absyn_Structfield* _temp1143=( struct Cyc_Absyn_Structfield*)
fields->hd; _temp1145=* _temp1143; _LL1155: _temp1154=( struct _tagged_string*)
_temp1145.name; goto _LL1153; _LL1153: _temp1152=( struct Cyc_Absyn_Tqual*)
_temp1145.tq; goto _LL1151; _LL1151: _temp1150=( void*) _temp1145.type; goto
_LL1149; _LL1149: _temp1148=( struct Cyc_Core_Opt*) _temp1145.width; goto
_LL1147; _LL1147: _temp1146=( struct Cyc_List_List*) _temp1145.attributes; goto
_LL1144; _LL1144:(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct
_tagged_string*), struct _tagged_string*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)
_temp1152, _temp1150, Cyc_Absyndump_dump_str, _temp1154); Cyc_Absyndump_dumpatts(
_temp1146); if( _temp1148 != 0){ Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_temp1148->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void* _temp1156=( void*)
d->r; struct Cyc_Absyn_Fndecl* _temp1180; struct Cyc_Absyn_Structdecl* _temp1182;
struct Cyc_Absyn_Uniondecl* _temp1184; struct Cyc_Absyn_Vardecl* _temp1186;
struct Cyc_Absyn_Vardecl _temp1188; struct Cyc_List_List* _temp1189; struct Cyc_Core_Opt*
_temp1191; int _temp1193; struct Cyc_Absyn_Exp* _temp1195; void* _temp1197;
struct Cyc_Absyn_Tqual* _temp1199; struct _tuple0* _temp1201; void* _temp1203;
struct Cyc_Absyn_Enumdecl* _temp1205; struct Cyc_Absyn_Xenumdecl* _temp1207; int
_temp1209; struct Cyc_Absyn_Exp* _temp1211; struct Cyc_Core_Opt* _temp1213;
struct Cyc_Core_Opt* _temp1215; struct Cyc_Absyn_Pat* _temp1217; struct Cyc_Absyn_Typedefdecl*
_temp1219; struct Cyc_List_List* _temp1221; struct _tagged_string* _temp1223;
struct Cyc_List_List* _temp1225; struct _tuple0* _temp1227; struct Cyc_List_List*
_temp1229; _LL1158: if((( struct _enum_struct*) _temp1156)->tag == Cyc_Absyn_Fn_d_tag){
_LL1181: _temp1180=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1156)->f1; goto _LL1159;} else{ goto _LL1160;} _LL1160: if((( struct
_enum_struct*) _temp1156)->tag == Cyc_Absyn_Struct_d_tag){ _LL1183: _temp1182=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp1156)->f1;
goto _LL1161;} else{ goto _LL1162;} _LL1162: if((( struct _enum_struct*)
_temp1156)->tag == Cyc_Absyn_Union_d_tag){ _LL1185: _temp1184=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp1156)->f1; goto _LL1163;} else{ goto
_LL1164;} _LL1164: if((( struct _enum_struct*) _temp1156)->tag == Cyc_Absyn_Var_d_tag){
_LL1187: _temp1186=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1156)->f1; _temp1188=* _temp1186; _LL1204: _temp1203=( void*) _temp1188.sc;
goto _LL1202; _LL1202: _temp1201=( struct _tuple0*) _temp1188.name; goto _LL1200;
_LL1200: _temp1199=( struct Cyc_Absyn_Tqual*) _temp1188.tq; goto _LL1198;
_LL1198: _temp1197=( void*) _temp1188.type; goto _LL1196; _LL1196: _temp1195=(
struct Cyc_Absyn_Exp*) _temp1188.initializer; goto _LL1194; _LL1194: _temp1193=(
int) _temp1188.shadow; goto _LL1192; _LL1192: _temp1191=( struct Cyc_Core_Opt*)
_temp1188.region; goto _LL1190; _LL1190: _temp1189=( struct Cyc_List_List*)
_temp1188.attributes; goto _LL1165;} else{ goto _LL1166;} _LL1166: if((( struct
_enum_struct*) _temp1156)->tag == Cyc_Absyn_Enum_d_tag){ _LL1206: _temp1205=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1156)->f1;
goto _LL1167;} else{ goto _LL1168;} _LL1168: if((( struct _enum_struct*)
_temp1156)->tag == Cyc_Absyn_Xenum_d_tag){ _LL1208: _temp1207=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1156)->f1; goto _LL1169;} else{ goto
_LL1170;} _LL1170: if((( struct _enum_struct*) _temp1156)->tag == Cyc_Absyn_Let_d_tag){
_LL1218: _temp1217=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1156)->f1; goto _LL1216; _LL1216: _temp1215=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1156)->f2; goto _LL1214; _LL1214: _temp1213=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1156)->f3; goto
_LL1212; _LL1212: _temp1211=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1156)->f4; goto _LL1210; _LL1210: _temp1209=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1156)->f5; goto _LL1171;} else{ goto _LL1172;} _LL1172: if((( struct
_enum_struct*) _temp1156)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1220: _temp1219=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1156)->f1;
goto _LL1173;} else{ goto _LL1174;} _LL1174: if((( struct _enum_struct*)
_temp1156)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1224: _temp1223=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1156)->f1; goto
_LL1222; _LL1222: _temp1221=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1156)->f2; goto _LL1175;} else{ goto _LL1176;} _LL1176: if((( struct
_enum_struct*) _temp1156)->tag == Cyc_Absyn_Using_d_tag){ _LL1228: _temp1227=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1156)->f1; goto
_LL1226; _LL1226: _temp1225=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1156)->f2; goto _LL1177;} else{ goto _LL1178;} _LL1178: if((( struct
_enum_struct*) _temp1156)->tag == Cyc_Absyn_ExternC_d_tag){ _LL1230: _temp1229=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp1156)->f1;
goto _LL1179;} else{ goto _LL1157;} _LL1159: if( _temp1180->is_inline){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1231=( char*)"inline"; struct _tagged_string
_temp1232; _temp1232.curr= _temp1231; _temp1232.base= _temp1231; _temp1232.last_plus_one=
_temp1231 + 7; _temp1232;}));} Cyc_Absyndump_dumpscope(( void*) _temp1180->sc);{
void* t=({ struct Cyc_Absyn_FnType_struct* _temp1233=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1233->tag= Cyc_Absyn_FnType_tag;
_temp1233->f1=({ struct Cyc_Absyn_FnInfo _temp1234; _temp1234.tvars= _temp1180->tvs;
_temp1234.effect= _temp1180->effect; _temp1234.ret_typ=( void*)(( void*)
_temp1180->ret_type); _temp1234.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1180->args); _temp1234.varargs= _temp1180->varargs; _temp1234.attributes= 0;
_temp1234;});( void*) _temp1233;});(( void(*)( struct Cyc_Absyn_Tqual*, void*,
void(* f)( struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(
0, t, Cyc_Absyndump_dump_atts_qvar, _temp1180); Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp1180->body); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1157;} _LL1161: Cyc_Absyndump_dumpscope(( void*) _temp1182->sc); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1235=( char*)"struct"; struct _tagged_string
_temp1236; _temp1236.curr= _temp1235; _temp1236.base= _temp1235; _temp1236.last_plus_one=
_temp1235 + 7; _temp1236;})); if( _temp1182->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)( _temp1182->name)->v);} Cyc_Absyndump_dumptvars( _temp1182->tvs);
if( _temp1182->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)( _temp1182->fields)->v);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1237=( char*)"}";
struct _tagged_string _temp1238; _temp1238.curr= _temp1237; _temp1238.base=
_temp1237; _temp1238.last_plus_one= _temp1237 + 2; _temp1238;})); Cyc_Absyndump_dumpatts(
_temp1182->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1239=( char*)";"; struct _tagged_string _temp1240; _temp1240.curr=
_temp1239; _temp1240.base= _temp1239; _temp1240.last_plus_one= _temp1239 + 2;
_temp1240;}));} goto _LL1157; _LL1163: Cyc_Absyndump_dumpscope(( void*)
_temp1184->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1241=(
char*)"union"; struct _tagged_string _temp1242; _temp1242.curr= _temp1241;
_temp1242.base= _temp1241; _temp1242.last_plus_one= _temp1241 + 6; _temp1242;}));
if( _temp1184->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1184->name)->v);}
Cyc_Absyndump_dumptvars( _temp1184->tvs); if( _temp1184->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)( _temp1184->fields)->v); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1243=( char*)"}"; struct _tagged_string _temp1244;
_temp1244.curr= _temp1243; _temp1244.base= _temp1243; _temp1244.last_plus_one=
_temp1243 + 2; _temp1244;})); Cyc_Absyndump_dumpatts( _temp1184->attributes);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1245=( char*)";";
struct _tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base=
_temp1245; _temp1246.last_plus_one= _temp1245 + 2; _temp1246;}));} goto _LL1157;
_LL1165: Cyc_Absyndump_dumpscope( _temp1203);(( void(*)( struct Cyc_Absyn_Tqual*,
void*, void(* f)( struct _tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)((
struct Cyc_Absyn_Tqual*) _temp1199, _temp1197, Cyc_Absyndump_dumpqvar, _temp1201);
Cyc_Absyndump_dumpatts( _temp1189); if( _temp1195 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1247= _temp1195; if( _temp1247 == 0){ _throw( Null_Exception);} _temp1247;}));}
Cyc_Absyndump_dump_semi(); goto _LL1157; _LL1167: Cyc_Absyndump_dumpscope(( void*)
_temp1205->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1248=(
char*)"enum"; struct _tagged_string _temp1249; _temp1249.curr= _temp1248;
_temp1249.base= _temp1248; _temp1249.last_plus_one= _temp1248 + 5; _temp1249;}));
if( _temp1205->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1205->name)->v);}
Cyc_Absyndump_dumptvars( _temp1205->tvs); Cyc_Absyndump_dump_char(( int)'{');
if( _temp1205->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields(( struct Cyc_List_List*)( _temp1205->fields)->v);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1250=( char*)"};";
struct _tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base=
_temp1250; _temp1251.last_plus_one= _temp1250 + 3; _temp1251;}));} goto _LL1157;
_LL1169: Cyc_Absyndump_dumpscope(( void*) _temp1207->sc); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1252=( char*)"xenum"; struct _tagged_string
_temp1253; _temp1253.curr= _temp1252; _temp1253.base= _temp1252; _temp1253.last_plus_one=
_temp1252 + 6; _temp1253;})); Cyc_Absyndump_dumpqvar( _temp1207->name); Cyc_Absyndump_dump_char((
int)'{'); if( _temp1207->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1207->fields); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1254=( char*)"};"; struct _tagged_string
_temp1255; _temp1255.curr= _temp1254; _temp1255.base= _temp1254; _temp1255.last_plus_one=
_temp1254 + 3; _temp1255;}));} goto _LL1157; _LL1171: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1256=( char*)"let"; struct _tagged_string
_temp1257; _temp1257.curr= _temp1256; _temp1257.base= _temp1256; _temp1257.last_plus_one=
_temp1256 + 4; _temp1257;})); Cyc_Absyndump_dumppat( _temp1217); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1211); Cyc_Absyndump_dump_semi(); goto
_LL1157; _LL1173: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1258=(
char*)"typedef"; struct _tagged_string _temp1259; _temp1259.curr= _temp1258;
_temp1259.base= _temp1258; _temp1259.last_plus_one= _temp1258 + 8; _temp1259;}));((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)( 0,( void*) _temp1219->defn,
Cyc_Absyndump_dumptypedefname, _temp1219); Cyc_Absyndump_dump_semi(); goto
_LL1157; _LL1175: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1260=(
char*)"namespace"; struct _tagged_string _temp1261; _temp1261.curr= _temp1260;
_temp1261.base= _temp1260; _temp1261.last_plus_one= _temp1260 + 10; _temp1261;}));
Cyc_Absyndump_dump_str( _temp1223); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1221 != 0; _temp1221= _temp1221->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1221->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1157; _LL1177: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1262=( char*)"using"; struct _tagged_string
_temp1263; _temp1263.curr= _temp1262; _temp1263.base= _temp1262; _temp1263.last_plus_one=
_temp1262 + 6; _temp1263;})); Cyc_Absyndump_dumpqvar( _temp1227); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1225 != 0; _temp1225= _temp1225->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1225->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1157; _LL1179: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1264=( char*)"extern \"C\" {"; struct _tagged_string _temp1265; _temp1265.curr=
_temp1264; _temp1265.base= _temp1264; _temp1265.last_plus_one= _temp1264 + 13;
_temp1265;})); for( 0; _temp1229 != 0; _temp1229= _temp1229->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1229->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1157; _LL1157:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp*
e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=( unsigned int) 1){
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1266=( void*) tms->hd;
struct Cyc_Absyn_Tqual* _temp1284; void* _temp1286; void* _temp1288; struct Cyc_Absyn_Exp*
_temp1290; struct Cyc_Absyn_Tqual* _temp1292; void* _temp1294; void* _temp1296;
struct Cyc_Absyn_Exp* _temp1298; struct Cyc_Absyn_Tqual* _temp1300; void*
_temp1302; void* _temp1304; struct Cyc_Absyn_Tqual* _temp1306; void* _temp1308;
struct Cyc_Absyn_Tvar* _temp1310; void* _temp1312; struct Cyc_Absyn_Exp*
_temp1314; struct Cyc_Absyn_Tqual* _temp1316; void* _temp1318; struct Cyc_Absyn_Tvar*
_temp1320; void* _temp1322; struct Cyc_Absyn_Exp* _temp1324; struct Cyc_Absyn_Tqual*
_temp1326; void* _temp1328; struct Cyc_Absyn_Tvar* _temp1330; void* _temp1332;
struct Cyc_Absyn_Tqual* _temp1334; void* _temp1336; void* _temp1338; _LL1268:
if(( unsigned int) _temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag ==
Cyc_Absyn_Pointer_mod_tag: 0){ _LL1289: _temp1288=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; if(( unsigned int) _temp1288 > 1u?(( struct _enum_struct*)
_temp1288)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1291: _temp1290=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1288)->f1; goto
_LL1287;} else{ goto _LL1270;} _LL1287: _temp1286=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; if( _temp1286 == Cyc_Absyn_HeapRgn){ goto _LL1285;} else{ goto
_LL1270;} _LL1285: _temp1284=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f3; goto _LL1269;} else{ goto _LL1270;} _LL1270: if(( unsigned int)
_temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1297: _temp1296=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; if(( unsigned int) _temp1296 > 1u?(( struct _enum_struct*)
_temp1296)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1299: _temp1298=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1296)->f1; goto
_LL1295;} else{ goto _LL1272;} _LL1295: _temp1294=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; if( _temp1294 == Cyc_Absyn_HeapRgn){ goto _LL1293;} else{ goto
_LL1272;} _LL1293: _temp1292=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f3; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(( unsigned int)
_temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1305: _temp1304=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; if( _temp1304 == Cyc_Absyn_TaggedArray_ps){ goto _LL1303;} else{
goto _LL1274;} _LL1303: _temp1302=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; if( _temp1302 == Cyc_Absyn_HeapRgn){ goto _LL1301;} else{ goto
_LL1274;} _LL1301: _temp1300=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f3; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(( unsigned int)
_temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1313: _temp1312=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; if(( unsigned int) _temp1312 > 1u?(( struct _enum_struct*)
_temp1312)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1315: _temp1314=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1312)->f1; goto
_LL1309;} else{ goto _LL1276;} _LL1309: _temp1308=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; if(( unsigned int) _temp1308 > 4u?(( struct _enum_struct*)
_temp1308)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1311: _temp1310=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1308)->f1; goto _LL1307;} else{ goto
_LL1276;} _LL1307: _temp1306=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f3; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(( unsigned int)
_temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1323: _temp1322=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; if(( unsigned int) _temp1322 > 1u?(( struct _enum_struct*)
_temp1322)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1325: _temp1324=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1322)->f1; goto
_LL1319;} else{ goto _LL1278;} _LL1319: _temp1318=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; if(( unsigned int) _temp1318 > 4u?(( struct _enum_struct*)
_temp1318)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1321: _temp1320=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1318)->f1; goto _LL1317;} else{ goto
_LL1278;} _LL1317: _temp1316=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f3; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(( unsigned int)
_temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1333: _temp1332=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; if( _temp1332 == Cyc_Absyn_TaggedArray_ps){ goto _LL1329;} else{
goto _LL1280;} _LL1329: _temp1328=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; if(( unsigned int) _temp1328 > 4u?(( struct _enum_struct*)
_temp1328)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1331: _temp1330=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1328)->f1; goto _LL1327;} else{ goto
_LL1280;} _LL1327: _temp1326=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f3; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(( unsigned int)
_temp1266 > 1u?(( struct _enum_struct*) _temp1266)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1339: _temp1338=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f1; goto _LL1337; _LL1337: _temp1336=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1266)->f2; goto _LL1335; _LL1335: _temp1334=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1266)->f3; goto _LL1281;} else{ goto
_LL1282;} _LL1282: goto _LL1283; _LL1269: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1290);(( void(*)( struct Cyc_List_List* tms,
void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return;
_LL1271: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1298);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1273: Cyc_Absyndump_dump_char((
int)'?');(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(
tms->tl, f, a); return; _LL1275: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1314); Cyc_Absyndump_dump_str( _temp1310->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1277: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1324); Cyc_Absyndump_dump_str( _temp1320->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1279: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1330->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1281:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp1340=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1340->tag= Cyc_Core_Impossible_tag;
_temp1340->f1=( struct _tagged_string)({ char* _temp1341=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1342; _temp1342.curr= _temp1341; _temp1342.base=
_temp1341; _temp1342.last_plus_one= _temp1341 + 25; _temp1342;});( struct
_xenum_struct*) _temp1340;})); _LL1283: { int next_is_pointer= 0; if( tms->tl !=
0){ void* _temp1343=( void*)( tms->tl)->hd; struct Cyc_Absyn_Tqual* _temp1349;
void* _temp1351; void* _temp1353; _LL1345: if(( unsigned int) _temp1343 > 1u?((
struct _enum_struct*) _temp1343)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1354:
_temp1353=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1343)->f1; goto
_LL1352; _LL1352: _temp1351=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1343)->f2; goto _LL1350; _LL1350: _temp1349=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1343)->f3; goto _LL1346;} else{ goto
_LL1347;} _LL1347: goto _LL1348; _LL1346: next_is_pointer= 1; goto _LL1344;
_LL1348: goto _LL1344; _LL1344:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1355=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp1371;
void* _temp1373; struct Cyc_Core_Opt* _temp1375; int _temp1377; struct Cyc_List_List*
_temp1379; void* _temp1381; struct Cyc_Position_Segment* _temp1383; struct Cyc_List_List*
_temp1385; int _temp1387; struct Cyc_Position_Segment* _temp1389; struct Cyc_List_List*
_temp1391; struct Cyc_List_List* _temp1393; struct Cyc_Position_Segment*
_temp1395; struct Cyc_Absyn_Tqual* _temp1397; void* _temp1399; void* _temp1401;
_LL1357: if( _temp1355 == Cyc_Absyn_Carray_mod){ goto _LL1358;} else{ goto
_LL1359;} _LL1359: if(( unsigned int) _temp1355 > 1u?(( struct _enum_struct*)
_temp1355)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL1372: _temp1371=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1355)->f1; goto
_LL1360;} else{ goto _LL1361;} _LL1361: if(( unsigned int) _temp1355 > 1u?((
struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL1374:
_temp1373=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1355)->f1; if(((
struct _enum_struct*) _temp1373)->tag == Cyc_Absyn_WithTypes_tag){ _LL1380:
_temp1379=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1373)->f1; goto _LL1378; _LL1378: _temp1377=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1373)->f2; goto _LL1376; _LL1376: _temp1375=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_WithTypes_struct*) _temp1373)->f3; goto _LL1362;} else{ goto
_LL1363;}} else{ goto _LL1363;} _LL1363: if(( unsigned int) _temp1355 > 1u?((
struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL1382:
_temp1381=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1355)->f1; if(((
struct _enum_struct*) _temp1381)->tag == Cyc_Absyn_NoTypes_tag){ _LL1386:
_temp1385=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1381)->f1;
goto _LL1384; _LL1384: _temp1383=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1381)->f2; goto _LL1364;} else{ goto _LL1365;}} else{ goto _LL1365;}
_LL1365: if(( unsigned int) _temp1355 > 1u?(( struct _enum_struct*) _temp1355)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL1392: _temp1391=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1355)->f1; goto _LL1390; _LL1390:
_temp1389=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1355)->f2; goto _LL1388; _LL1388: _temp1387=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1355)->f3; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(( unsigned int)
_temp1355 > 1u?(( struct _enum_struct*) _temp1355)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL1396: _temp1395=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1355)->f1; goto _LL1394; _LL1394: _temp1393=( struct Cyc_List_List*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1355)->f2; goto _LL1368;} else{
goto _LL1369;} _LL1369: if(( unsigned int) _temp1355 > 1u?(( struct _enum_struct*)
_temp1355)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1402: _temp1401=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1355)->f1; goto _LL1400; _LL1400:
_temp1399=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1355)->f2; goto
_LL1398; _LL1398: _temp1397=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1355)->f3; goto _LL1370;} else{ goto _LL1356;} _LL1358: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1403=( char*)"[]"; struct _tagged_string
_temp1404; _temp1404.curr= _temp1403; _temp1404.base= _temp1403; _temp1404.last_plus_one=
_temp1403 + 3; _temp1404;})); goto _LL1356; _LL1360: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1371); Cyc_Absyndump_dump_char(( int)']');
goto _LL1356; _LL1362: Cyc_Absyndump_dumpfunargs( _temp1379, _temp1377,
_temp1375); goto _LL1356; _LL1364:(( void(*)( void(* f)( struct _tagged_string*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1385,( struct _tagged_string)({ char* _temp1405=( char*)"("; struct
_tagged_string _temp1406; _temp1406.curr= _temp1405; _temp1406.base= _temp1405;
_temp1406.last_plus_one= _temp1405 + 2; _temp1406;}),( struct _tagged_string)({
char* _temp1407=( char*)")"; struct _tagged_string _temp1408; _temp1408.curr=
_temp1407; _temp1408.base= _temp1407; _temp1408.last_plus_one= _temp1407 + 2;
_temp1408;}),( struct _tagged_string)({ char* _temp1409=( char*)","; struct
_tagged_string _temp1410; _temp1410.curr= _temp1409; _temp1410.base= _temp1409;
_temp1410.last_plus_one= _temp1409 + 2; _temp1410;})); goto _LL1356; _LL1366:
if( _temp1387){ Cyc_Absyndump_dumpkindedtvars( _temp1391);} else{ Cyc_Absyndump_dumptvars(
_temp1391);} goto _LL1356; _LL1368: Cyc_Absyndump_dumpatts( _temp1393); goto
_LL1356; _LL1370:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1411=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1411->tag= Cyc_Core_Impossible_tag; _temp1411->f1=( struct _tagged_string)({
char* _temp1412=( char*)"dumptms"; struct _tagged_string _temp1413; _temp1413.curr=
_temp1412; _temp1413.base= _temp1412; _temp1413.last_plus_one= _temp1412 + 8;
_temp1413;});( struct _xenum_struct*) _temp1411;})); _LL1356:;} return;} _LL1267:;}}
static struct Cyc_Absyn_Tqual Cyc_Absyndump_empty_tq_v=( struct Cyc_Absyn_Tqual){.q_const=
0,.q_volatile= 0,.q_restrict= 0}; static struct Cyc_Absyn_Tqual* Cyc_Absyndump_empty_tq=&
Cyc_Absyndump_empty_tq_v; void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual*
tq, void* t, void(* f)( void*), void* a){ if( tq == 0){ tq=( struct Cyc_Absyn_Tqual*)
Cyc_Absyndump_empty_tq;}{ struct Cyc_List_List* _temp1417; void* _temp1419;
struct Cyc_Absyn_Tqual* _temp1421; struct _tuple4 _temp1415= Cyc_Absynpp_to_tms((
struct Cyc_Absyn_Tqual*)({ struct Cyc_Absyn_Tqual* _temp1414= tq; if( _temp1414
== 0){ _throw( Null_Exception);} _temp1414;}), t); _LL1422: _temp1421= _temp1415.f1;
goto _LL1420; _LL1420: _temp1419= _temp1415.f2; goto _LL1418; _LL1418: _temp1417=
_temp1415.f3; goto _LL1416; _LL1416: Cyc_Absyndump_dumptq( _temp1421); Cyc_Absyndump_dumpntyp(
_temp1419);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1417), f, a);}} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl= tdl->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) tdl->hd);}}