#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual*
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple9{ struct Cyc_Absyn_Enumdecl* f1; } ; struct _tuple10{
struct Cyc_Absyn_Xenumdecl* f1; } ; struct _tuple11{ struct Cyc_Tcpat_Con_s* f1;
struct Cyc_List_List* f2; } ; struct _tuple12{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple13{ int f1; struct
Cyc_Position_Segment* f2; } ; struct _tuple14{ void* f1; struct _tuple13* f2; }
; struct _tuple15{ int f1; } ; struct _tuple16{ struct Cyc_Position_Segment* f1;
int f2; } ; struct _tuple17{ void* f1; int f2; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd(
struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(*
f)( void*, int), void* env); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; struct Cyc_Dict_Absent_struct{ char* tag; } ; struct Cyc_Set_Set; typedef
struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern char
Cyc_Set_Absent_tag[ 7u]; struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
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
; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); typedef
void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag; struct Cyc_Tcenv_StructRes_struct{
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void*
Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*,
void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern struct _tuple5* Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Pat* p, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); static struct _tuple6* Cyc_Tcpat_null_designator(
struct Cyc_Absyn_Pat* p){ return({ struct _tuple6* _temp0=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp0->f1= 0; _temp0->f2= p; _temp0;});}
static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} struct _tuple5* Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** region_opt){ void* t; struct Cyc_List_List*
tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp1=( void*) p->r;
int _temp33; void* _temp35; char _temp37; struct _tagged_string _temp39; struct
Cyc_Absyn_Vardecl* _temp41; struct Cyc_Absyn_Vardecl* _temp43; struct Cyc_Absyn_Pat*
_temp45; struct Cyc_List_List* _temp47; struct Cyc_List_List* _temp49; struct
Cyc_List_List* _temp51; struct Cyc_Core_Opt* _temp53; struct Cyc_Core_Opt**
_temp55; struct Cyc_Absyn_Structdecl* _temp56; struct Cyc_Absyn_Enumfield*
_temp58; struct Cyc_Absyn_Enumdecl* _temp60; struct Cyc_List_List* _temp62;
struct Cyc_List_List* _temp64; struct Cyc_Core_Opt* _temp66; struct _tuple1*
_temp68; struct Cyc_Absyn_Enumfield* _temp70; struct Cyc_Absyn_Xenumdecl*
_temp72; struct Cyc_List_List* _temp74; struct Cyc_List_List* _temp76; struct
_tuple1* _temp78; struct _tuple1* _temp80; struct Cyc_List_List* _temp82; struct
Cyc_List_List* _temp84; struct _tuple1* _temp86; struct Cyc_List_List* _temp88;
struct Cyc_List_List* _temp90; struct _tuple1* _temp92; _LL3: if( _temp1 == Cyc_Absyn_Wild_p){
goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int) _temp1 > 2u?(( struct
_enum_struct*) _temp1)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL36: _temp35=( void*)((
struct Cyc_Absyn_Int_p_struct*) _temp1)->f1; goto _LL34; _LL34: _temp33=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1)->f2; goto _LL6;} else{ goto _LL7;} _LL7:
if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL38: _temp37=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1)->f1; goto
_LL8;} else{ goto _LL9;} _LL9: if(( unsigned int) _temp1 > 2u?(( struct
_enum_struct*) _temp1)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL40: _temp39=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1)->f1; goto
_LL10;} else{ goto _LL11;} _LL11: if( _temp1 == Cyc_Absyn_Null_p){ goto _LL12;}
else{ goto _LL13;} _LL13: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*)
_temp1)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL42: _temp41=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1)->f1; goto _LL14;} else{ goto _LL15;}
_LL15: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_Reference_p_tag: 0){ _LL44: _temp43=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp1)->f1; goto _LL16;} else{ goto _LL17;}
_LL17: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_Pointer_p_tag: 0){ _LL46: _temp45=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Pointer_p_struct*) _temp1)->f1; goto _LL18;} else{ goto _LL19;} _LL19:
if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL48: _temp47=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1)->f1; goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL57:
_temp56=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f1; goto _LL54; _LL54: _temp53=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f2; _temp55=&(( struct Cyc_Absyn_Struct_p_struct*) _temp1)->f2; goto
_LL52; _LL52: _temp51=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f3; goto _LL50; _LL50: _temp49=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f4; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL69:
_temp68=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1)->f1; goto
_LL67; _LL67: _temp66=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f2; goto _LL65; _LL65: _temp64=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f3; goto _LL63; _LL63: _temp62=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f4; goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp1)->f5; goto _LL59; _LL59: _temp58=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1)->f6; goto _LL24;} else{ goto _LL25;}
_LL25: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_Xenum_p_tag: 0){ _LL79: _temp78=( struct _tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1)->f1; goto _LL77; _LL77: _temp76=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1)->f2; goto _LL75; _LL75: _temp74=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1)->f3; goto _LL73; _LL73: _temp72=( struct Cyc_Absyn_Xenumdecl*)(( struct
Cyc_Absyn_Xenum_p_struct*) _temp1)->f4; goto _LL71; _LL71: _temp70=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1)->f5; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp1 > 2u?(( struct _enum_struct*) _temp1)->tag ==
Cyc_Absyn_UnknownId_p_tag: 0){ _LL81: _temp80=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1)->f1; goto _LL28;} else{ goto _LL29;} _LL29: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL87: _temp86=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f1; goto _LL85; _LL85: _temp84=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f2; goto _LL83; _LL83: _temp82=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f3; goto _LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp1 >
2u?(( struct _enum_struct*) _temp1)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL93: _temp92=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f1; goto _LL91; _LL91: _temp90=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f2; goto _LL89; _LL89: _temp88=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f3; goto _LL32;} else{ goto _LL2;} _LL4: t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
goto _LL2; _LL6: t= _temp35 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL2; _LL8: t= Cyc_Absyn_uchar_t; goto _LL2; _LL10: t= Cyc_Absyn_float_t;
goto _LL2; _LL12: t=({ struct Cyc_Absyn_PointerType_struct* _temp94=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp94->tag= Cyc_Absyn_PointerType_tag;
_temp94->f1=({ struct Cyc_Absyn_PtrInfo _temp95; _temp95.elt_typ=( void*) Cyc_Absyn_new_evar(
Cyc_Absyn_AnyKind); _temp95.rgn_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind);
_temp95.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp95.tq= Cyc_Absyn_empty_tqual(); _temp95.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp95;});( void*) _temp94;}); goto _LL2; _LL14: t=
Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);( void*)( _temp41->type=( void*) t);
_temp41->tq= Cyc_Absyn_empty_tqual(); _temp41->shadow= 0; v_result=({ struct Cyc_List_List*
_temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*) _temp41; _temp96->tl= v_result; _temp96;}); goto _LL2;
_LL16: t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind); if( region_opt == 0){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ char* _temp97=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 45; _temp98;})); goto _LL2;}( void*)( _temp43->type=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp99=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp99->tag= Cyc_Absyn_PointerType_tag;
_temp99->f1=({ struct Cyc_Absyn_PtrInfo _temp100; _temp100.elt_typ=( void*) t;
_temp100.rgn_typ=( void*)* region_opt; _temp100.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp100.tq= Cyc_Absyn_empty_tqual();
_temp100.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp100;});( void*) _temp99;})); _temp43->tq= Cyc_Absyn_empty_tqual(); _temp43->shadow=
0; v_result=({ struct Cyc_List_List* _temp101=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp101->hd=( void*) _temp43; _temp101->tl=
v_result; _temp101;}); goto _LL2; _LL18: { void* ptr_rgn= Cyc_Absyn_new_evar(
Cyc_Absyn_RgnKind); struct _tuple5 _temp104; struct Cyc_List_List* _temp105;
struct Cyc_List_List* _temp107; struct _tuple5* _temp102= Cyc_Tcpat_tcPat( te,
_temp45,( void**)& ptr_rgn); _temp104=* _temp102; _LL108: _temp107= _temp104.f1;
goto _LL106; _LL106: _temp105= _temp104.f2; goto _LL103; _LL103: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp107); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp105); t=({ struct
Cyc_Absyn_PointerType_struct* _temp109=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp109->tag= Cyc_Absyn_PointerType_tag;
_temp109->f1=({ struct Cyc_Absyn_PtrInfo _temp110; _temp110.elt_typ=( void*)((
void*)( _temp45->topt)->v); _temp110.rgn_typ=( void*) ptr_rgn; _temp110.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp110.tq= Cyc_Absyn_empty_tqual();
_temp110.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp110;});( void*) _temp109;}); goto _LL2;} _LL20: { struct Cyc_List_List* ts=
0; for( 0; _temp47 != 0; _temp47= _temp47->tl){ struct _tuple5 _temp113; struct
Cyc_List_List* _temp114; struct Cyc_List_List* _temp116; struct _tuple5*
_temp111= Cyc_Tcpat_tcPat( te,( struct Cyc_Absyn_Pat*) _temp47->hd, region_opt);
_temp113=* _temp111; _LL117: _temp116= _temp113.f1; goto _LL115; _LL115:
_temp114= _temp113.f2; goto _LL112; _LL112: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp116); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp114); ts=({ struct Cyc_List_List*
_temp118=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp118->hd=( void*)({ struct _tuple7* _temp119=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp119->f1= Cyc_Absyn_empty_tqual(); _temp119->f2=(
void*)((( struct Cyc_Absyn_Pat*) _temp47->hd)->topt)->v; _temp119;}); _temp118->tl=
ts; _temp118;});} t=({ struct Cyc_Absyn_TupleType_struct* _temp120=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp120->tag= Cyc_Absyn_TupleType_tag;
_temp120->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ts);( void*) _temp120;}); goto _LL2;} _LL22: if( _temp51 != 0){ return(( struct
_tuple5*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp121=( char*)"tcPat: struct<...> not implemented"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 35; _temp122;}));}{ struct Cyc_List_List*
dps0= _temp49; for( 0; dps0 != 0; dps0= dps0->tl){ struct _tuple5 _temp125;
struct Cyc_List_List* _temp126; struct Cyc_List_List* _temp128; struct _tuple5*
_temp123= Cyc_Tcpat_tcPat( te,(*(( struct _tuple6*) dps0->hd)).f2, region_opt);
_temp125=* _temp123; _LL129: _temp128= _temp125.f1; goto _LL127; _LL127:
_temp126= _temp125.f2; goto _LL124; _LL124: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp128); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp126);}}{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp56->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst);* _temp55=({ struct Cyc_Core_Opt* _temp130=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp130->v=( void*) all_typs;
_temp130;}); t=({ struct Cyc_Absyn_StructType_struct* _temp131=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp131->tag= Cyc_Absyn_StructType_tag;
_temp131->f1=( struct _tuple1*)(( struct _tuple1*)( _temp56->name)->v); _temp131->f2=
all_typs; _temp131->f3=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp132=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp132[ 0]= _temp56;
_temp132;}));( void*) _temp131;});{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc, _temp49, _temp56); for( 0;
fields != 0; fields= fields->tl){ struct _tuple8 _temp135; struct Cyc_Absyn_Pat*
_temp136; struct Cyc_Absyn_Structfield* _temp138; struct _tuple8* _temp133=(
struct _tuple8*) fields->hd; _temp135=* _temp133; _LL139: _temp138= _temp135.f1;
goto _LL137; _LL137: _temp136= _temp135.f2; goto _LL134; _LL134: { void*
inst_fieldtyp= Cyc_Tcutil_substitute( all_inst,( void*) _temp138->type); if( !
Cyc_Tcutil_unify(( void*)( _temp136->topt)->v, inst_fieldtyp)){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp140=* _temp138->name; struct _tagged_string
_temp141= Cyc_Absynpp_typ2string( inst_fieldtyp); struct _tagged_string _temp142=
Cyc_Absynpp_typ2string(( void*)( _temp136->topt)->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp140.last_plus_one - _temp140.curr, _temp140.curr, _temp141.last_plus_one -
_temp141.curr, _temp141.curr, _temp142.last_plus_one - _temp142.curr, _temp142.curr);}));}}}
goto _LL2;}} _LL24: { void* heap= Cyc_Absyn_HeapRgn; struct Cyc_Tcenv_Tenv* te2=
Cyc_Tcenv_add_type_vars( p->loc, te, _temp64); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp64);{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp60->tvs); struct Cyc_List_List* all_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, all_inst); t=({ struct
Cyc_Absyn_EnumType_struct* _temp143=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp143->tag= Cyc_Absyn_EnumType_tag;
_temp143->f1=( struct _tuple1*)(( struct _tuple1*)( _temp60->name)->v); _temp143->f2=
all_typs; _temp143->f3=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_Enumdecl**)({
struct _tuple9* _temp144=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp144->f1= _temp60; _temp144;}));( void*) _temp143;});{ struct Cyc_List_List*
tqts= _temp58->typs; for( 0; _temp62 != 0? tqts != 0: 0; _temp62= _temp62->tl,
tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) _temp62->hd;
struct _tuple5 _temp147; struct Cyc_List_List* _temp148; struct Cyc_List_List*
_temp150; struct _tuple5* _temp145= Cyc_Tcpat_tcPat( te2, p2,( void**)& heap);
_temp147=* _temp145; _LL151: _temp150= _temp147.f1; goto _LL149; _LL149:
_temp148= _temp147.f2; goto _LL146; _LL146: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp150); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp148);{ void* field_typ= Cyc_Tcutil_substitute(
all_inst,(*(( struct _tuple7*) tqts->hd)).f2); if( ! Cyc_Tcutil_unify(( void*)(
p2->topt)->v, field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string
_temp152= Cyc_Absynpp_qvar2string( _temp68); struct _tagged_string _temp153= Cyc_Absynpp_typ2string(
field_typ); struct _tagged_string _temp154= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp152.last_plus_one -
_temp152.curr, _temp152.curr, _temp153.last_plus_one - _temp153.curr, _temp153.curr,
_temp154.last_plus_one - _temp154.curr, _temp154.curr);}));}}} if( _temp62 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp155= Cyc_Absynpp_qvar2string(
_temp68); xprintf("too many arguments for enum constructor %.*s", _temp155.last_plus_one
- _temp155.curr, _temp155.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp156= Cyc_Absynpp_qvar2string( _temp68); xprintf("too few arguments for enum constructor %.*s",
_temp156.last_plus_one - _temp156.curr, _temp156.curr);}));} goto _LL2;}}} _LL26: {
void* heap= Cyc_Absyn_HeapRgn; struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
p->loc, te, _temp76); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp76); t=({ struct
Cyc_Absyn_XenumType_struct* _temp157=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp157->tag= Cyc_Absyn_XenumType_tag;
_temp157->f1= _temp72->name; _temp157->f2=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_Xenumdecl**)({ struct _tuple10* _temp158=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp158->f1= _temp72; _temp158;}));( void*)
_temp157;});{ struct Cyc_List_List* tqts= _temp70->typs; for( 0; _temp74 != 0?
tqts != 0: 0; _temp74= _temp74->tl, tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) _temp74->hd; struct _tuple5 _temp161; struct Cyc_List_List*
_temp162; struct Cyc_List_List* _temp164; struct _tuple5* _temp159= Cyc_Tcpat_tcPat(
te2, p2,( void**)& heap); _temp161=* _temp159; _LL165: _temp164= _temp161.f1;
goto _LL163; _LL163: _temp162= _temp161.f2; goto _LL160; _LL160: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp164); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp162);{ void*
field_typ=(*(( struct _tuple7*) tqts->hd)).f2; if( ! Cyc_Tcutil_unify(( void*)(
p2->topt)->v, field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string
_temp166= Cyc_Absynpp_qvar2string( _temp78); struct _tagged_string _temp167= Cyc_Absynpp_typ2string(
field_typ); struct _tagged_string _temp168= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp166.last_plus_one -
_temp166.curr, _temp166.curr, _temp167.last_plus_one - _temp167.curr, _temp167.curr,
_temp168.last_plus_one - _temp168.curr, _temp168.curr);}));}}} if( _temp74 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp169= Cyc_Absynpp_qvar2string(
_temp78); xprintf("too many arguments for xenum constructor %.*s", _temp169.last_plus_one
- _temp169.curr, _temp169.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp170= Cyc_Absynpp_qvar2string( _temp78); xprintf("too few arguments for xenum constructor %.*s",
_temp170.last_plus_one - _temp170.curr, _temp170.curr);}));} goto _LL2;}} _LL28:{
struct _handler_cons _temp171; _push_handler(& _temp171);{ struct _xenum_struct*
_temp172=( struct _xenum_struct*) setjmp( _temp171.handler); if( ! _temp172){{
void* _temp173= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp80); struct Cyc_Absyn_Structdecl*
_temp183; struct Cyc_Absyn_Enumfield* _temp185; struct Cyc_Absyn_Enumdecl*
_temp187; struct Cyc_Absyn_Enumfield* _temp189; struct Cyc_Absyn_Xenumdecl*
_temp191; void* _temp193; _LL175: if((( struct _enum_struct*) _temp173)->tag ==
Cyc_Tcenv_StructRes_tag){ _LL184: _temp183=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp173)->f1; goto _LL176;} else{ goto
_LL177;} _LL177: if((( struct _enum_struct*) _temp173)->tag == Cyc_Tcenv_EnumRes_tag){
_LL188: _temp187=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp173)->f1; goto _LL186; _LL186: _temp185=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp173)->f2; goto _LL178;} else{ goto _LL179;}
_LL179: if((( struct _enum_struct*) _temp173)->tag == Cyc_Tcenv_XenumRes_tag){
_LL192: _temp191=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp173)->f1; goto _LL190; _LL190: _temp189=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp173)->f2; goto _LL180;} else{ goto
_LL181;} _LL181: if((( struct _enum_struct*) _temp173)->tag == Cyc_Tcenv_VarRes_tag){
_LL194: _temp193=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp173)->f1; goto
_LL182;} else{ goto _LL174;} _LL176: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp195=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
_temp196.last_plus_one= _temp195 + 45; _temp196;})); t= Cyc_Absyn_wildtyp();
goto _LL174; _LL178:( void*)( p->r=( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp197=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp197->tag= Cyc_Absyn_Enum_p_tag; _temp197->f1= _temp185->name; _temp197->f2=
0; _temp197->f3= 0; _temp197->f4= 0; _temp197->f5= _temp187; _temp197->f6=
_temp185;( void*) _temp197;}));{ struct _tuple5* _temp198= Cyc_Tcpat_tcPat( te,
p, region_opt); _npop_handler( 0u); return _temp198;} _LL180:( void*)( p->r=(
void*)({ struct Cyc_Absyn_Xenum_p_struct* _temp199=( struct Cyc_Absyn_Xenum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenum_p_struct)); _temp199->tag= Cyc_Absyn_Xenum_p_tag;
_temp199->f1= _temp189->name; _temp199->f2= 0; _temp199->f3= 0; _temp199->f4=
_temp191; _temp199->f5= _temp189;( void*) _temp199;}));{ struct _tuple5*
_temp200= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp200;} _LL182:{ void* _temp201=(* _temp80).f1; struct Cyc_List_List*
_temp209; _LL203: if( _temp201 == Cyc_Absyn_Loc_n){ goto _LL204;} else{ goto
_LL205;} _LL205: if(( unsigned int) _temp201 > 1u?(( struct _enum_struct*)
_temp201)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL210: _temp209=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp201)->f1; if( _temp209 == 0){ goto _LL206;}
else{ goto _LL207;}} else{ goto _LL207;} _LL207: goto _LL208; _LL204: goto
_LL206; _LL206:(* _temp80).f1= Cyc_Absyn_Loc_n;( void*)( p->r=( void*)({ struct
Cyc_Absyn_Var_p_struct* _temp211=( struct Cyc_Absyn_Var_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Var_p_struct)); _temp211->tag= Cyc_Absyn_Var_p_tag;
_temp211->f1= Cyc_Absyn_new_vardecl( _temp80, Cyc_Absyn_VoidType, 0);( void*)
_temp211;}));{ struct _tuple5* _temp212= Cyc_Tcpat_tcPat( te, p, region_opt);
_npop_handler( 0u); return _temp212;} _LL208: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp213=( char*)"qualified variable in pattern"; struct
_tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 30; _temp214;})); goto _LL202; _LL202:;} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp215= Cyc_Absynpp_qvar2string( _temp80);
xprintf("unresolved identifier %.*s in pattern", _temp215.last_plus_one -
_temp215.curr, _temp215.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL174; _LL174:;};
_pop_handler();} else{ struct _xenum_struct* _temp217= _temp172; _LL219: if(
_temp217->tag == Cyc_Dict_Absent_tag){ goto _LL220;} else{ goto _LL221;} _LL221:
goto _LL222; _LL220:{ void* _temp223=(* _temp80).f1; struct Cyc_List_List*
_temp231; _LL225: if( _temp223 == Cyc_Absyn_Loc_n){ goto _LL226;} else{ goto
_LL227;} _LL227: if(( unsigned int) _temp223 > 1u?(( struct _enum_struct*)
_temp223)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL232: _temp231=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp223)->f1; if( _temp231 == 0){ goto _LL228;}
else{ goto _LL229;}} else{ goto _LL229;} _LL229: goto _LL230; _LL226: goto
_LL228; _LL228:(* _temp80).f1= Cyc_Absyn_Loc_n;( void*)( p->r=( void*)({ struct
Cyc_Absyn_Var_p_struct* _temp233=( struct Cyc_Absyn_Var_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Var_p_struct)); _temp233->tag= Cyc_Absyn_Var_p_tag;
_temp233->f1= Cyc_Absyn_new_vardecl( _temp80, Cyc_Absyn_VoidType, 0);( void*)
_temp233;})); return Cyc_Tcpat_tcPat( te, p, region_opt); _LL230: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp234= Cyc_Absynpp_qvar2string( _temp80);
xprintf("unbound identifier %.*s in pattern", _temp234.last_plus_one - _temp234.curr,
_temp234.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL224; _LL224:;} goto _LL218;
_LL222:( void) _throw( _temp217); _LL218:;}}} goto _LL2; _LL30:{ struct
_handler_cons _temp235; _push_handler(& _temp235);{ struct _xenum_struct*
_temp236=( struct _xenum_struct*) setjmp( _temp235.handler); if( ! _temp236){{
void* _temp237= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp86); struct Cyc_Absyn_Structdecl*
_temp247; struct Cyc_Absyn_Enumfield* _temp249; struct Cyc_Absyn_Enumdecl*
_temp251; struct Cyc_Absyn_Enumfield* _temp253; struct Cyc_Absyn_Xenumdecl*
_temp255; void* _temp257; _LL239: if((( struct _enum_struct*) _temp237)->tag ==
Cyc_Tcenv_StructRes_tag){ _LL248: _temp247=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp237)->f1; goto _LL240;} else{ goto
_LL241;} _LL241: if((( struct _enum_struct*) _temp237)->tag == Cyc_Tcenv_EnumRes_tag){
_LL252: _temp251=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp237)->f1; goto _LL250; _LL250: _temp249=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp237)->f2; goto _LL242;} else{ goto _LL243;}
_LL243: if((( struct _enum_struct*) _temp237)->tag == Cyc_Tcenv_XenumRes_tag){
_LL256: _temp255=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp237)->f1; goto _LL254; _LL254: _temp253=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp237)->f2; goto _LL244;} else{ goto
_LL245;} _LL245: if((( struct _enum_struct*) _temp237)->tag == Cyc_Tcenv_VarRes_tag){
_LL258: _temp257=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp237)->f1; goto
_LL246;} else{ goto _LL238;} _LL240: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp82);( void*)( p->r=( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp259=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp259->tag= Cyc_Absyn_Struct_p_tag; _temp259->f1= _temp247; _temp259->f2= 0;
_temp259->f3= _temp84; _temp259->f4= x;( void*) _temp259;}));{ struct _tuple5*
_temp260= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp260;}} _LL242:( void*)( p->r=( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp261=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp261->tag= Cyc_Absyn_Enum_p_tag; _temp261->f1= _temp249->name; _temp261->f2=
0; _temp261->f3= _temp84; _temp261->f4= _temp82; _temp261->f5= _temp251;
_temp261->f6= _temp249;( void*) _temp261;}));{ struct _tuple5* _temp262= Cyc_Tcpat_tcPat(
te, p, region_opt); _npop_handler( 0u); return _temp262;} _LL244:( void*)( p->r=(
void*)({ struct Cyc_Absyn_Xenum_p_struct* _temp263=( struct Cyc_Absyn_Xenum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenum_p_struct)); _temp263->tag= Cyc_Absyn_Xenum_p_tag;
_temp263->f1= _temp253->name; _temp263->f2= _temp84; _temp263->f3= _temp82;
_temp263->f4= _temp255; _temp263->f5= _temp253;( void*) _temp263;}));{ struct
_tuple5* _temp264= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp264;} _LL246: goto _LL238; _LL238:;}; _pop_handler();} else{ struct
_xenum_struct* _temp266= _temp236; _LL268: if( _temp266->tag == Cyc_Dict_Absent_tag){
goto _LL269;} else{ goto _LL270;} _LL270: goto _LL271; _LL269: goto _LL267;
_LL271:( void) _throw( _temp266); _LL267:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp272= Cyc_Absynpp_qvar2string( _temp86); xprintf("%.*s is not a constructor in pattern",
_temp272.last_plus_one - _temp272.curr, _temp272.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL2; _LL32:{ struct _handler_cons _temp273; _push_handler(& _temp273);{
struct _xenum_struct* _temp274=( struct _xenum_struct*) setjmp( _temp273.handler);
if( ! _temp274){{ void* _temp275= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp92);
struct Cyc_Absyn_Structdecl* _temp285; struct Cyc_Absyn_Enumfield* _temp287;
struct Cyc_Absyn_Enumdecl* _temp289; struct Cyc_Absyn_Enumfield* _temp291;
struct Cyc_Absyn_Xenumdecl* _temp293; void* _temp295; _LL277: if((( struct
_enum_struct*) _temp275)->tag == Cyc_Tcenv_StructRes_tag){ _LL286: _temp285=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp275)->f1;
goto _LL278;} else{ goto _LL279;} _LL279: if((( struct _enum_struct*) _temp275)->tag
== Cyc_Tcenv_EnumRes_tag){ _LL290: _temp289=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp275)->f1; goto _LL288; _LL288: _temp287=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp275)->f2;
goto _LL280;} else{ goto _LL281;} _LL281: if((( struct _enum_struct*) _temp275)->tag
== Cyc_Tcenv_XenumRes_tag){ _LL294: _temp293=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp275)->f1; goto _LL292; _LL292: _temp291=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_XenumRes_struct*) _temp275)->f2;
goto _LL282;} else{ goto _LL283;} _LL283: if((( struct _enum_struct*) _temp275)->tag
== Cyc_Tcenv_VarRes_tag){ _LL296: _temp295=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp275)->f1; goto _LL284;} else{ goto _LL276;} _LL278:( void*)( p->r=( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp297=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp297->tag= Cyc_Absyn_Struct_p_tag;
_temp297->f1= _temp285; _temp297->f2= 0; _temp297->f3= _temp90; _temp297->f4=
_temp88;( void*) _temp297;}));{ struct _tuple5* _temp298= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp298;} _LL280: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp299=( char*)"enum with designators not implemented";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 38; _temp300;})); t= Cyc_Absyn_wildtyp();
goto _LL276; _LL282: Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp301=( char*)"xenum with designators not implemented"; struct _tagged_string
_temp302; _temp302.curr= _temp301; _temp302.base= _temp301; _temp302.last_plus_one=
_temp301 + 39; _temp302;})); t= Cyc_Absyn_wildtyp(); goto _LL276; _LL284: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp303= Cyc_Absynpp_qvar2string( _temp92);
xprintf("unbound struct %.*s in pattern", _temp303.last_plus_one - _temp303.curr,
_temp303.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL276; _LL276:;}; _pop_handler();}
else{ struct _xenum_struct* _temp305= _temp274; _LL307: if( _temp305->tag == Cyc_Dict_Absent_tag){
goto _LL308;} else{ goto _LL309;} _LL309: goto _LL310; _LL308: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp311= Cyc_Absynpp_qvar2string( _temp92);
xprintf("unbound struct %.*s in pattern", _temp311.last_plus_one - _temp311.curr,
_temp311.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL306; _LL310:( void) _throw(
_temp305); _LL306:;}}} goto _LL2; _LL2:;} p->topt=({ struct Cyc_Core_Opt*
_temp312=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp312->v=( void*) t; _temp312;}); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Tcpat_get_name, v_result), p->loc,( struct _tagged_string)({
char* _temp313=( char*)"pattern contains a repeated variable"; struct
_tagged_string _temp314; _temp314.curr= _temp313; _temp314.base= _temp313;
_temp314.last_plus_one= _temp313 + 37; _temp314;})); return({ struct _tuple5*
_temp315=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp315->f1=
tv_result; _temp315->f2= v_result; _temp315;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp316=( void*) p->r;
struct Cyc_Absyn_Pat* _temp330; struct Cyc_List_List* _temp332; struct Cyc_List_List*
_temp334; struct Cyc_Core_Opt* _temp336; struct Cyc_Absyn_Structdecl* _temp338;
struct Cyc_Absyn_Enumfield* _temp340; struct Cyc_Absyn_Xenumdecl* _temp342;
struct Cyc_List_List* _temp344; struct Cyc_List_List* _temp346; struct _tuple1*
_temp348; struct Cyc_Absyn_Enumfield* _temp350; struct Cyc_Absyn_Enumdecl*
_temp352; struct Cyc_List_List* _temp354; struct Cyc_List_List* _temp356; struct
Cyc_Core_Opt* _temp358; struct _tuple1* _temp360; struct Cyc_List_List* _temp362;
_LL318: if(( unsigned int) _temp316 > 2u?(( struct _enum_struct*) _temp316)->tag
== Cyc_Absyn_Pointer_p_tag: 0){ _LL331: _temp330=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp316)->f1; goto _LL319;} else{ goto
_LL320;} _LL320: if(( unsigned int) _temp316 > 2u?(( struct _enum_struct*)
_temp316)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL339: _temp338=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp316)->f1; goto _LL337; _LL337: _temp336=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp316)->f2; goto
_LL335; _LL335: _temp334=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp316)->f3; goto _LL333; _LL333: _temp332=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp316)->f4; goto _LL321;} else{ goto _LL322;}
_LL322: if(( unsigned int) _temp316 > 2u?(( struct _enum_struct*) _temp316)->tag
== Cyc_Absyn_Xenum_p_tag: 0){ _LL349: _temp348=( struct _tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp316)->f1; goto _LL347; _LL347: _temp346=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_p_struct*) _temp316)->f2; goto _LL345; _LL345: _temp344=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp316)->f3; goto _LL343;
_LL343: _temp342=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp316)->f4; goto _LL341; _LL341: _temp340=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp316)->f5; goto _LL323;} else{ goto _LL324;}
_LL324: if(( unsigned int) _temp316 > 2u?(( struct _enum_struct*) _temp316)->tag
== Cyc_Absyn_Enum_p_tag: 0){ _LL361: _temp360=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp316)->f1; goto _LL359; _LL359: _temp358=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp316)->f2; goto _LL357; _LL357: _temp356=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp316)->f3; goto _LL355;
_LL355: _temp354=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp316)->f4; goto _LL353; _LL353: _temp352=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp316)->f5; goto _LL351; _LL351: _temp350=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp316)->f6;
goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp316 > 2u?((
struct _enum_struct*) _temp316)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL363:
_temp362=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp316)->f1;
goto _LL327;} else{ goto _LL328;} _LL328: goto _LL329; _LL319: Cyc_Tcpat_check_pat_regions(
te, _temp330);{ void* _temp364=( void*)( p->topt)->v; struct Cyc_Absyn_PtrInfo
_temp370; struct Cyc_Absyn_Conref* _temp372; struct Cyc_Absyn_Tqual* _temp374;
struct Cyc_Absyn_Conref* _temp376; void* _temp378; void* _temp380; _LL366: if((
unsigned int) _temp364 > 5u?(( struct _enum_struct*) _temp364)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL371: _temp370=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp364)->f1; _LL381: _temp380=( void*) _temp370.elt_typ; goto _LL379; _LL379:
_temp378=( void*) _temp370.rgn_typ; goto _LL377; _LL377: _temp376=( struct Cyc_Absyn_Conref*)
_temp370.nullable; goto _LL375; _LL375: _temp374=( struct Cyc_Absyn_Tqual*)
_temp370.tq; goto _LL373; _LL373: _temp372=( struct Cyc_Absyn_Conref*) _temp370.bounds;
goto _LL367;} else{ goto _LL368;} _LL368: goto _LL369; _LL367: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp378); return; _LL369:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp382=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp382->tag= Cyc_Core_Impossible_tag; _temp382->f1=( struct _tagged_string)({
char* _temp383=( char*)"check_pat_regions: bad pointer type"; struct
_tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 36; _temp384;});( struct _xenum_struct*)
_temp382;})); _LL365:;} _LL321: _temp344=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp332); goto _LL323; _LL323: _temp354=
_temp344; goto _LL325; _LL325: _temp362= _temp354; goto _LL327; _LL327: for( 0;
_temp362 != 0; _temp362= _temp362->tl){ Cyc_Tcpat_check_pat_regions( te,( struct
Cyc_Absyn_Pat*) _temp362->hd);} return; _LL329: return; _LL317:;} typedef void*
Cyc_Tcpat_Name_value; static const int Cyc_Tcpat_Name_v_tag= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Tcpat_Int_v_tag= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ;
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; typedef void* Cyc_Tcpat_Simple_pat;
static void* Cyc_Tcpat_Any=( void*) 0; static const int Cyc_Tcpat_Con_tag= 0;
struct Cyc_Tcpat_Con_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; typedef void* Cyc_Tcpat_simple_pat_t; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp385=( void*)
c1->name; struct _tagged_string _temp391; int _temp393; _LL387: if((( struct
_enum_struct*) _temp385)->tag == Cyc_Tcpat_Name_v_tag){ _LL392: _temp391=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp385)->f1; goto
_LL388;} else{ goto _LL389;} _LL389: if((( struct _enum_struct*) _temp385)->tag
== Cyc_Tcpat_Int_v_tag){ _LL394: _temp393=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp385)->f1; goto _LL390;} else{ goto _LL386;} _LL388: { void* _temp395=( void*)
c2->name; struct _tagged_string _temp401; int _temp403; _LL397: if((( struct
_enum_struct*) _temp395)->tag == Cyc_Tcpat_Name_v_tag){ _LL402: _temp401=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp395)->f1; goto
_LL398;} else{ goto _LL399;} _LL399: if((( struct _enum_struct*) _temp395)->tag
== Cyc_Tcpat_Int_v_tag){ _LL404: _temp403=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp395)->f1; goto _LL400;} else{ goto _LL396;} _LL398: return Cyc_String_zstrcmp(
_temp391, _temp401); _LL400: return - 1; _LL396:;} _LL390: { void* _temp405=(
void*) c2->name; struct _tagged_string _temp411; int _temp413; _LL407: if(((
struct _enum_struct*) _temp405)->tag == Cyc_Tcpat_Name_v_tag){ _LL412: _temp411=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp405)->f1; goto
_LL408;} else{ goto _LL409;} _LL409: if((( struct _enum_struct*) _temp405)->tag
== Cyc_Tcpat_Int_v_tag){ _LL414: _temp413=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp405)->f1; goto _LL410;} else{ goto _LL406;} _LL408: return 1; _LL410:
return _temp393 - _temp413; _LL406:;} _LL386:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){
return(( struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct
Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt
Cyc_Tcpat_one_opt=( struct Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt=( struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char
_temp418[ 6u]="tuple"; static struct Cyc_Tcpat_Name_v_struct _temp415={ 0u,(
struct _tagged_string){ _temp418, _temp418, _temp418 + 6u}}; static void* Cyc_Tcpat_tuple_name=(
struct Cyc_Tcpat_Name_v_struct*)& _temp415; static char _temp422[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct _temp419={ 0u,( struct _tagged_string){
_temp422, _temp422, _temp422 + 5u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0=(
struct Cyc_Tcpat_Con_s){.name=( void*)(( struct Cyc_Tcpat_Name_v_struct*)&
_temp419),.arity= 0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static
char _temp426[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct _temp423={ 0u,(
struct _tagged_string){ _temp426, _temp426, _temp426 + 4u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( struct Cyc_Tcpat_Name_v_struct*)&
_temp423),.arity= 1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static
char _temp430[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct _temp427={ 0u,(
struct _tagged_string){ _temp430, _temp430, _temp430 + 4u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( struct Cyc_Tcpat_Name_v_struct*)&
_temp427),.arity= 1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp431=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp431->name=(
void*)({ struct Cyc_Tcpat_Int_v_struct* _temp432=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp432->tag= Cyc_Tcpat_Int_v_tag;
_temp432->f1= i;( void*) _temp432;}); _temp431->arity= 0; _temp431->span= 0;
_temp431;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({
struct Cyc_Tcpat_Con_s* _temp433=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp433->name=( void*)({ struct Cyc_Tcpat_Int_v_struct*
_temp435=( struct Cyc_Tcpat_Int_v_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp435->tag= Cyc_Tcpat_Int_v_tag; _temp435->f1=( int) c;( void*) _temp435;});
_temp433->arity= 0; _temp433->span=({ struct Cyc_Core_Opt* _temp434=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp434->v=( void*) 256;
_temp434;}); _temp433;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con(
struct _tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp436=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp436->name=( void*)({ struct
Cyc_Tcpat_Name_v_struct* _temp437=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp437->tag= Cyc_Tcpat_Name_v_tag;
_temp437->f1= f;( void*) _temp437;}); _temp436->arity= 0; _temp436->span= 0;
_temp436;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_double_con( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_s* _temp438=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp438->name=( void*)({ struct
Cyc_Tcpat_Name_v_struct* _temp439=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp439->tag= Cyc_Tcpat_Name_v_tag;
_temp439->f1= d;( void*) _temp439;}); _temp438->arity= 0; _temp438->span= 0;
_temp438;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({
struct Cyc_Tcpat_Con_s* _temp440=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp440->name=( void*) Cyc_Tcpat_tuple_name; _temp440->arity=
i; _temp440->span= Cyc_Tcpat_one_opt_ptr; _temp440;});} static void* Cyc_Tcpat_null_pat(){
return({ struct Cyc_Tcpat_Con_struct* _temp441=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp441->tag= Cyc_Tcpat_Con_tag;
_temp441->f1= Cyc_Tcpat_null_con; _temp441->f2= 0;( void*) _temp441;});} static
void* Cyc_Tcpat_int_pat( int i){ return({ struct Cyc_Tcpat_Con_struct* _temp442=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp442->tag= Cyc_Tcpat_Con_tag; _temp442->f1= Cyc_Tcpat_int_con( i); _temp442->f2=
0;( void*) _temp442;});} static void* Cyc_Tcpat_char_pat( char c){ return({
struct Cyc_Tcpat_Con_struct* _temp443=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp443->tag= Cyc_Tcpat_Con_tag;
_temp443->f1= Cyc_Tcpat_char_con( c); _temp443->f2= 0;( void*) _temp443;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_string f){ return({ struct Cyc_Tcpat_Con_struct*
_temp444=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp444->tag= Cyc_Tcpat_Con_tag; _temp444->f1= Cyc_Tcpat_float_con( f);
_temp444->f2= 0;( void*) _temp444;});} static void* Cyc_Tcpat_double_pat( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_struct* _temp445=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp445->tag= Cyc_Tcpat_Con_tag;
_temp445->f1= Cyc_Tcpat_double_con( d); _temp445->f2= 0;( void*) _temp445;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return({ struct Cyc_Tcpat_Con_struct*
_temp446=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp446->tag= Cyc_Tcpat_Con_tag; _temp446->f1= Cyc_Tcpat_null_ptr_con; _temp446->f2=({
struct Cyc_List_List* _temp447=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp447->hd=( void*) p; _temp447->tl= 0; _temp447;});(
void*) _temp446;});} static void* Cyc_Tcpat_ptr_pat( void* p){ return({ struct
Cyc_Tcpat_Con_struct* _temp448=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp448->tag= Cyc_Tcpat_Con_tag;
_temp448->f1= Cyc_Tcpat_ptr_con; _temp448->f2=({ struct Cyc_List_List* _temp449=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp449->hd=(
void*) p; _temp449->tl= 0; _temp449;});( void*) _temp448;});} static void* Cyc_Tcpat_tuple_pat(
struct Cyc_List_List* ss){ return({ struct Cyc_Tcpat_Con_struct* _temp450=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp450->tag= Cyc_Tcpat_Con_tag; _temp450->f1= Cyc_Tcpat_tuple_con((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ss)); _temp450->f2= ss;( void*)
_temp450;});} static void* Cyc_Tcpat_con_pat( struct _tagged_string con_name,
struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){ struct Cyc_Tcpat_Con_s* c=({
struct Cyc_Tcpat_Con_s* _temp451=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp451->name=( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp452=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp452->tag= Cyc_Tcpat_Name_v_tag; _temp452->f1= con_name;( void*) _temp452;});
_temp451->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ps);
_temp451->span= span; _temp451;}); return({ struct Cyc_Tcpat_Con_struct*
_temp453=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp453->tag= Cyc_Tcpat_Con_tag; _temp453->f1= c; _temp453->f2= ps;( void*)
_temp453;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp454=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp486; int
_temp488; void* _temp490; char _temp492; struct _tagged_string _temp494; struct
Cyc_List_List* _temp496; struct Cyc_Absyn_Vardecl* _temp498; struct Cyc_Absyn_Pat*
_temp500; struct Cyc_List_List* _temp502; struct Cyc_List_List* _temp504; struct
Cyc_Core_Opt* _temp506; struct Cyc_Absyn_Structdecl* _temp508; struct Cyc_Absyn_Enumfield*
_temp510; struct Cyc_Absyn_Enumdecl* _temp512; struct Cyc_List_List* _temp514;
struct Cyc_List_List* _temp516; struct Cyc_Core_Opt* _temp518; struct _tuple1*
_temp520; struct Cyc_Absyn_Enumfield* _temp522; struct Cyc_Absyn_Xenumdecl*
_temp524; struct Cyc_List_List* _temp526; struct Cyc_List_List* _temp528; struct
_tuple1* _temp530; struct _tuple1* _temp532; struct Cyc_List_List* _temp534;
struct Cyc_List_List* _temp536; struct _tuple1* _temp538; struct Cyc_List_List*
_temp540; struct Cyc_List_List* _temp542; struct _tuple1* _temp544; _LL456: if(
_temp454 == Cyc_Absyn_Wild_p){ goto _LL457;} else{ goto _LL458;} _LL458: if((
unsigned int) _temp454 > 2u?(( struct _enum_struct*) _temp454)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL487: _temp486=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp454)->f1; goto _LL459;} else{ goto _LL460;} _LL460: if( _temp454 == Cyc_Absyn_Null_p){
goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp454 > 2u?((
struct _enum_struct*) _temp454)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL491:
_temp490=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp454)->f1; goto _LL489;
_LL489: _temp488=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp454)->f2; goto
_LL463;} else{ goto _LL464;} _LL464: if(( unsigned int) _temp454 > 2u?(( struct
_enum_struct*) _temp454)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL493: _temp492=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp454)->f1; goto _LL465;} else{ goto
_LL466;} _LL466: if(( unsigned int) _temp454 > 2u?(( struct _enum_struct*)
_temp454)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL495: _temp494=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp454)->f1; goto _LL467;}
else{ goto _LL468;} _LL468: if(( unsigned int) _temp454 > 2u?(( struct
_enum_struct*) _temp454)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL497: _temp496=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp454)->f1; goto
_LL469;} else{ goto _LL470;} _LL470: if(( unsigned int) _temp454 > 2u?(( struct
_enum_struct*) _temp454)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL499: _temp498=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp454)->f1;
goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int) _temp454 > 2u?((
struct _enum_struct*) _temp454)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL501:
_temp500=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp454)->f1;
goto _LL473;} else{ goto _LL474;} _LL474: if(( unsigned int) _temp454 > 2u?((
struct _enum_struct*) _temp454)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL509:
_temp508=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp454)->f1; goto _LL507; _LL507: _temp506=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp454)->f2; goto _LL505; _LL505: _temp504=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp454)->f3; goto
_LL503; _LL503: _temp502=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp454)->f4; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp454 > 2u?(( struct _enum_struct*) _temp454)->tag == Cyc_Absyn_Enum_p_tag: 0){
_LL521: _temp520=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp454)->f1;
goto _LL519; _LL519: _temp518=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp454)->f2; goto _LL517; _LL517: _temp516=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp454)->f3; goto _LL515; _LL515: _temp514=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*) _temp454)->f4; goto _LL513;
_LL513: _temp512=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp454)->f5; goto _LL511; _LL511: _temp510=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp454)->f6; goto _LL477;} else{ goto _LL478;}
_LL478: if(( unsigned int) _temp454 > 2u?(( struct _enum_struct*) _temp454)->tag
== Cyc_Absyn_Xenum_p_tag: 0){ _LL531: _temp530=( struct _tuple1*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp454)->f1; goto _LL529; _LL529: _temp528=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_p_struct*) _temp454)->f2; goto _LL527; _LL527: _temp526=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp454)->f3; goto _LL525;
_LL525: _temp524=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp454)->f4; goto _LL523; _LL523: _temp522=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp454)->f5; goto _LL479;} else{ goto _LL480;}
_LL480: if(( unsigned int) _temp454 > 2u?(( struct _enum_struct*) _temp454)->tag
== Cyc_Absyn_UnknownId_p_tag: 0){ _LL533: _temp532=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownId_p_struct*) _temp454)->f1; goto _LL481;} else{ goto _LL482;}
_LL482: if(( unsigned int) _temp454 > 2u?(( struct _enum_struct*) _temp454)->tag
== Cyc_Absyn_UnknownCall_p_tag: 0){ _LL539: _temp538=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp454)->f1; goto _LL537; _LL537: _temp536=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp454)->f2;
goto _LL535; _LL535: _temp534=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp454)->f3; goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp454 > 2u?(( struct _enum_struct*) _temp454)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL545: _temp544=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp454)->f1; goto _LL543; _LL543: _temp542=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp454)->f2; goto _LL541; _LL541: _temp540=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp454)->f3;
goto _LL485;} else{ goto _LL455;} _LL457: s= Cyc_Tcpat_Any; goto _LL455; _LL459:
s= Cyc_Tcpat_Any; goto _LL455; _LL461: s= Cyc_Tcpat_null_pat(); goto _LL455;
_LL463: s= Cyc_Tcpat_int_pat( _temp488); goto _LL455; _LL465: s= Cyc_Tcpat_char_pat(
_temp492); goto _LL455; _LL467: s= Cyc_Tcpat_float_pat( _temp494); goto _LL455;
_LL469: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp496)); goto _LL455; _LL471: s= Cyc_Tcpat_Any; goto _LL455; _LL473: { void*
ss= Cyc_Tcpat_compile_pat( _temp500);{ void* _temp546= Cyc_Tcutil_compress((
void*)( p->topt)->v); struct Cyc_Absyn_PtrInfo _temp552; struct Cyc_Absyn_Conref*
_temp554; struct Cyc_Absyn_Tqual* _temp556; struct Cyc_Absyn_Conref* _temp558;
void* _temp560; void* _temp562; _LL548: if(( unsigned int) _temp546 > 5u?((
struct _enum_struct*) _temp546)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL553:
_temp552=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp546)->f1; _LL563: _temp562=( void*) _temp552.elt_typ; goto _LL561; _LL561:
_temp560=( void*) _temp552.rgn_typ; goto _LL559; _LL559: _temp558=( struct Cyc_Absyn_Conref*)
_temp552.nullable; goto _LL557; _LL557: _temp556=( struct Cyc_Absyn_Tqual*)
_temp552.tq; goto _LL555; _LL555: _temp554=( struct Cyc_Absyn_Conref*) _temp552.bounds;
goto _LL549;} else{ goto _LL550;} _LL550: goto _LL551; _LL549: { int is_nullable=
0; int still_working= 1; while( still_working) { void* _temp564=( void*)
_temp558->v; struct Cyc_Absyn_Conref* _temp572; int _temp574; _LL566: if((
unsigned int) _temp564 > 1u?(( struct _enum_struct*) _temp564)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL573: _temp572=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp564)->f1; goto _LL567;} else{ goto _LL568;} _LL568: if( _temp564 == Cyc_Absyn_No_constr){
goto _LL569;} else{ goto _LL570;} _LL570: if(( unsigned int) _temp564 > 1u?((
struct _enum_struct*) _temp564)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL575:
_temp574=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp564)->f1; goto _LL571;}
else{ goto _LL565;} _LL567:( void*)( _temp558->v=( void*)(( void*) _temp572->v));
continue; _LL569:( void*)( _temp558->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp576=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Eq_constr_struct)); _temp576->tag= Cyc_Absyn_Eq_constr_tag; _temp576->f1=(
void*) 0;( void*) _temp576;})); is_nullable= 0; still_working= 0; goto _LL565;
_LL571: is_nullable=( int) _temp574; still_working= 0; goto _LL565; _LL565:;}
if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat( ss);} else{ s= Cyc_Tcpat_ptr_pat(
ss);} goto _LL547;} _LL551: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp577=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 44; _temp578;})); goto _LL547; _LL547:;} goto
_LL455;} _LL475: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)( _temp508->fields)->v; for( 0; fields != 0; fields=
fields->tl){ int found= 0;{ struct Cyc_List_List* dlps0= _temp502; for( 0; !
found? dlps0 != 0: 0; dlps0= dlps0->tl){ struct _tuple6 _temp581; struct Cyc_Absyn_Pat*
_temp582; struct Cyc_List_List* _temp584; struct _tuple6* _temp579=( struct
_tuple6*) dlps0->hd; _temp581=* _temp579; _LL585: _temp584= _temp581.f1; goto
_LL583; _LL583: _temp582= _temp581.f2; goto _LL580; _LL580: { struct Cyc_List_List*
_temp586= _temp584; struct Cyc_List_List _temp592; struct Cyc_List_List*
_temp593; void* _temp595; struct _tagged_string* _temp597; _LL588: if( _temp586
== 0){ goto _LL590;} else{ _temp592=* _temp586; _LL596: _temp595=( void*)
_temp592.hd; if((( struct _enum_struct*) _temp595)->tag == Cyc_Absyn_FieldName_tag){
_LL598: _temp597=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp595)->f1; goto _LL594;} else{ goto _LL590;} _LL594: _temp593=( struct Cyc_List_List*)
_temp592.tl; if( _temp593 == 0){ goto _LL589;} else{ goto _LL590;}} _LL590: goto
_LL591; _LL589: if( Cyc_String_zstrptrcmp( _temp597,(( struct Cyc_Absyn_Structfield*)
fields->hd)->name) == 0){ ps=({ struct Cyc_List_List* _temp599=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp599->hd=( void*) Cyc_Tcpat_compile_pat(
_temp582); _temp599->tl= ps; _temp599;}); found= 1;} goto _LL587; _LL591:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp600=( char*)"bad designator(s)"; struct _tagged_string _temp601; _temp601.curr=
_temp600; _temp601.base= _temp600; _temp601.last_plus_one= _temp600 + 18;
_temp601;})); goto _LL587; _LL587:;}}} if( ! found){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp602=(
char*)"bad designator"; struct _tagged_string _temp603; _temp603.curr= _temp602;
_temp603.base= _temp602; _temp603.last_plus_one= _temp602 + 15; _temp603;}));}}}
s= Cyc_Tcpat_tuple_pat( ps); goto _LL455;} _LL477: { int span=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)( _temp512->fields)->v);
s= Cyc_Tcpat_con_pat(*(* _temp510->name).f2,({ struct Cyc_Core_Opt* _temp604=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp604->v=(
void*) span; _temp604;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp514)); goto
_LL455;} _LL479: s= Cyc_Tcpat_con_pat(*(* _temp522->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp526)); goto _LL455; _LL481: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp605=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 23; _temp606;})); goto _LL455; _LL483: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp607=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp608; _temp608.curr= _temp607; _temp608.base= _temp607; _temp608.last_plus_one=
_temp607 + 25; _temp608;})); goto _LL455; _LL485: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp609=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp610; _temp610.curr=
_temp609; _temp610.base= _temp609; _temp610.last_plus_one= _temp609 + 27;
_temp610;})); goto _LL455; _LL455:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
const int Cyc_Tcpat_Pos_tag= 0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; const int Cyc_Tcpat_Neg_tag= 1; struct Cyc_Tcpat_Neg_struct{
int tag; struct Cyc_Set_Set* f1; } ; typedef void* Cyc_Tcpat_term_desc_t;
typedef void* Cyc_Tcpat_Access; static void* Cyc_Tcpat_Obj=( void*) 0; static
const int Cyc_Tcpat_Sel_tag= 0; struct Cyc_Tcpat_Sel_struct{ int tag; int f1;
void* f2; } ; typedef void* Cyc_Tcpat_access_t; typedef void* Cyc_Tcpat_Decision;
static void* Cyc_Tcpat_Failure=( void*) 0; static const int Cyc_Tcpat_Success_tag=
0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq_tag=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; void* f1; struct Cyc_Tcpat_Con_s* f2;
void* f3; void* f4; } ; typedef void* Cyc_Tcpat_decision_t; typedef void* Cyc_Tcpat_Answer;
static void* Cyc_Tcpat_Yes=( void*) 0; static void* Cyc_Tcpat_No=( void*) 1;
static void* Cyc_Tcpat_Maybe=( void*) 2; typedef void* Cyc_Tcpat_answer_t;
static void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void*
_temp611= td; struct Cyc_Set_Set* _temp617; struct Cyc_List_List* _temp619;
struct Cyc_Tcpat_Con_s* _temp621; _LL613: if((( struct _enum_struct*) _temp611)->tag
== Cyc_Tcpat_Neg_tag){ _LL618: _temp617=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp611)->f1; goto _LL614;} else{ goto _LL615;} _LL615: if((( struct
_enum_struct*) _temp611)->tag == Cyc_Tcpat_Pos_tag){ _LL622: _temp621=( struct
Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp611)->f1; goto _LL620;
_LL620: _temp619=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp611)->f2; goto _LL616;} else{ goto _LL612;} _LL614: if((( int(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp617, c)){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp623=( char*)"add_neg called when constructor already in set"; struct
_tagged_string _temp624; _temp624.curr= _temp623; _temp624.base= _temp623;
_temp624.last_plus_one= _temp623 + 47; _temp624;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp617) + 1 >=( int)( c->span)->v:
0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp625=( char*)"add_neg called when |cs U {c}| >= span(c)"; struct
_tagged_string _temp626; _temp626.curr= _temp625; _temp626.base= _temp625;
_temp626.last_plus_one= _temp625 + 42; _temp626;}));} return({ struct Cyc_Tcpat_Neg_struct*
_temp627=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp627->tag= Cyc_Tcpat_Neg_tag; _temp627->f1=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp617, c);(
void*) _temp627;}); _LL616: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp628=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp629; _temp629.curr= _temp628; _temp629.base= _temp628;
_temp629.last_plus_one= _temp628 + 35; _temp629;})); _LL612:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp630= td; struct Cyc_List_List*
_temp636; struct Cyc_Tcpat_Con_s* _temp638; struct Cyc_Set_Set* _temp640; _LL632:
if((( struct _enum_struct*) _temp630)->tag == Cyc_Tcpat_Pos_tag){ _LL639:
_temp638=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp630)->f1;
goto _LL637; _LL637: _temp636=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp630)->f2; goto _LL633;} else{ goto _LL634;} _LL634: if((( struct
_enum_struct*) _temp630)->tag == Cyc_Tcpat_Neg_tag){ _LL641: _temp640=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp630)->f1; goto _LL635;} else{
goto _LL631;} _LL633: if( Cyc_Tcpat_compare_con( c, _temp638) == 0){ return Cyc_Tcpat_Yes;}
else{ return Cyc_Tcpat_No;} _LL635: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp640, c)){ return Cyc_Tcpat_No;}
else{ if( c->span != 0?( int)( c->span)->v ==(( int(*)( struct Cyc_Set_Set* s))
Cyc_Set_cardinality)( _temp640) + 1: 0){ return Cyc_Tcpat_Yes;} else{ return Cyc_Tcpat_Maybe;}}
_LL631:;} static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List*
ctxt, void* dsc){ struct Cyc_List_List* _temp642= ctxt; struct Cyc_List_List
_temp648; struct Cyc_List_List* _temp649; struct _tuple11* _temp651; struct
_tuple11 _temp653; struct Cyc_List_List* _temp654; struct Cyc_Tcpat_Con_s*
_temp656; _LL644: if( _temp642 == 0){ goto _LL645;} else{ goto _LL646;} _LL646:
if( _temp642 == 0){ goto _LL643;} else{ _temp648=* _temp642; _LL652: _temp651=(
struct _tuple11*) _temp648.hd; _temp653=* _temp651; _LL657: _temp656= _temp653.f1;
goto _LL655; _LL655: _temp654= _temp653.f2; goto _LL650; _LL650: _temp649=(
struct Cyc_List_List*) _temp648.tl; goto _LL647;} _LL645: return 0; _LL647:
return({ struct Cyc_List_List* _temp658=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp658->hd=( void*)({ struct _tuple11*
_temp659=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp659->f1=
_temp656; _temp659->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp660=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp660->hd=(
void*) dsc; _temp660->tl= _temp654; _temp660;}); _temp659;}); _temp658->tl=
_temp649; _temp658;}); _LL643:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp661= ctxt; struct Cyc_List_List
_temp667; struct Cyc_List_List* _temp668; struct _tuple11* _temp670; struct
_tuple11 _temp672; struct Cyc_List_List* _temp673; struct Cyc_Tcpat_Con_s*
_temp675; _LL663: if( _temp661 == 0){ goto _LL664;} else{ goto _LL665;} _LL665:
if( _temp661 == 0){ goto _LL662;} else{ _temp667=* _temp661; _LL671: _temp670=(
struct _tuple11*) _temp667.hd; _temp672=* _temp670; _LL676: _temp675= _temp672.f1;
goto _LL674; _LL674: _temp673= _temp672.f2; goto _LL669; _LL669: _temp668=(
struct Cyc_List_List*) _temp667.tl; goto _LL666;} _LL664: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp677=( char*)"norm_context: empty context"; struct _tagged_string _temp678;
_temp678.curr= _temp677; _temp678.base= _temp677; _temp678.last_plus_one=
_temp677 + 28; _temp678;})); _LL666: return Cyc_Tcpat_augment( _temp668,({
struct Cyc_Tcpat_Pos_struct* _temp679=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp679->tag= Cyc_Tcpat_Pos_tag;
_temp679->f1= _temp675; _temp679->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp673);( void*) _temp679;})); _LL662:;} static void* Cyc_Tcpat_build_desc(
struct Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct
_tuple5 _temp681=({ struct _tuple5 _temp680; _temp680.f1= ctxt; _temp680.f2=
work; _temp680;}); struct Cyc_List_List* _temp691; struct Cyc_List_List*
_temp693; struct Cyc_List_List* _temp695; struct Cyc_List_List* _temp697; struct
Cyc_List_List* _temp699; struct Cyc_List_List* _temp701; struct Cyc_List_List*
_temp703; struct Cyc_List_List _temp705; struct Cyc_List_List* _temp706; struct
_tuple12* _temp708; struct _tuple12 _temp710; struct Cyc_List_List* _temp711;
struct Cyc_List_List* _temp713; struct Cyc_List_List* _temp715; struct Cyc_List_List*
_temp717; struct Cyc_List_List _temp719; struct Cyc_List_List* _temp720; struct
_tuple11* _temp722; struct _tuple11 _temp724; struct Cyc_List_List* _temp725;
struct Cyc_Tcpat_Con_s* _temp727; _LL683: _LL694: _temp693= _temp681.f1; if(
_temp693 == 0){ goto _LL692;} else{ goto _LL685;} _LL692: _temp691= _temp681.f2;
if( _temp691 == 0){ goto _LL684;} else{ goto _LL685;} _LL685: _LL698: _temp697=
_temp681.f1; if( _temp697 == 0){ goto _LL696;} else{ goto _LL687;} _LL696:
_temp695= _temp681.f2; goto _LL686; _LL687: _LL702: _temp701= _temp681.f1; goto
_LL700; _LL700: _temp699= _temp681.f2; if( _temp699 == 0){ goto _LL688;} else{
goto _LL689;} _LL689: _LL718: _temp717= _temp681.f1; if( _temp717 == 0){ goto
_LL682;} else{ _temp719=* _temp717; _LL723: _temp722=( struct _tuple11*)
_temp719.hd; _temp724=* _temp722; _LL728: _temp727= _temp724.f1; goto _LL726;
_LL726: _temp725= _temp724.f2; goto _LL721; _LL721: _temp720=( struct Cyc_List_List*)
_temp719.tl; goto _LL704;} _LL704: _temp703= _temp681.f2; if( _temp703 == 0){
goto _LL682;} else{ _temp705=* _temp703; _LL709: _temp708=( struct _tuple12*)
_temp705.hd; _temp710=* _temp708; _LL716: _temp715= _temp710.f1; goto _LL714;
_LL714: _temp713= _temp710.f2; goto _LL712; _LL712: _temp711= _temp710.f3; goto
_LL707; _LL707: _temp706=( struct Cyc_List_List*) _temp705.tl; goto _LL690;}
_LL684: return dsc; _LL686: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp729=( char*)"build_desc: ctxt and work don't match";
struct _tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 38; _temp730;})); _LL688: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp731=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp732; _temp732.curr= _temp731; _temp732.base= _temp731; _temp732.last_plus_one=
_temp731 + 38; _temp732;})); _LL690: { void* td=({ struct Cyc_Tcpat_Pos_struct*
_temp733=( struct Cyc_Tcpat_Pos_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp733->tag= Cyc_Tcpat_Pos_tag; _temp733->f1= _temp727; _temp733->f2=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp725),({
struct Cyc_List_List* _temp734=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp734->hd=( void*) dsc; _temp734->tl= _temp711;
_temp734;}));( void*) _temp733;}); return Cyc_Tcpat_build_desc( _temp720, td,
_temp706);} _LL682:;} static void* Cyc_Tcpat_match( void* p, void* obj, void*
dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp735=
allmrules; struct Cyc_List_List _temp741; struct Cyc_List_List* _temp742; struct
_tuple0* _temp744; struct _tuple0 _temp746; void* _temp747; void* _temp749;
_LL737: if( _temp735 == 0){ goto _LL738;} else{ goto _LL739;} _LL739: if(
_temp735 == 0){ goto _LL736;} else{ _temp741=* _temp735; _LL745: _temp744=(
struct _tuple0*) _temp741.hd; _temp746=* _temp744; _LL750: _temp749= _temp746.f1;
goto _LL748; _LL748: _temp747= _temp746.f2; goto _LL743; _LL743: _temp742=(
struct Cyc_List_List*) _temp741.tl; goto _LL740;} _LL738: return Cyc_Tcpat_Failure;
_LL740: return(( void*(*)( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp749, Cyc_Tcpat_Obj, dsc, 0, 0, _temp747, _temp742);
_LL736:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){
return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(({
struct Cyc_Tcpat_Neg_struct* _temp751=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp751->tag= Cyc_Tcpat_Neg_tag;
_temp751->f1= Cyc_Tcpat_empty_con_set();( void*) _temp751;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp752= work; struct Cyc_List_List _temp760; struct Cyc_List_List* _temp761;
struct _tuple12* _temp763; struct _tuple12 _temp765; struct Cyc_List_List*
_temp766; struct Cyc_List_List* _temp768; struct Cyc_List_List* _temp770; struct
Cyc_List_List _temp772; struct Cyc_List_List* _temp773; struct _tuple12*
_temp775; struct _tuple12 _temp777; struct Cyc_List_List* _temp778; struct Cyc_List_List*
_temp780; struct Cyc_List_List* _temp782; _LL754: if( _temp752 == 0){ goto
_LL755;} else{ goto _LL756;} _LL756: if( _temp752 == 0){ goto _LL758;} else{
_temp760=* _temp752; _LL764: _temp763=( struct _tuple12*) _temp760.hd; _temp765=*
_temp763; _LL771: _temp770= _temp765.f1; if( _temp770 == 0){ goto _LL769;} else{
goto _LL758;} _LL769: _temp768= _temp765.f2; if( _temp768 == 0){ goto _LL767;}
else{ goto _LL758;} _LL767: _temp766= _temp765.f3; if( _temp766 == 0){ goto
_LL762;} else{ goto _LL758;} _LL762: _temp761=( struct Cyc_List_List*) _temp760.tl;
goto _LL757;} _LL758: if( _temp752 == 0){ goto _LL753;} else{ _temp772=*
_temp752; _LL776: _temp775=( struct _tuple12*) _temp772.hd; _temp777=* _temp775;
_LL783: _temp782= _temp777.f1; goto _LL781; _LL781: _temp780= _temp777.f2; goto
_LL779; _LL779: _temp778= _temp777.f3; goto _LL774; _LL774: _temp773=( struct
Cyc_List_List*) _temp772.tl; goto _LL759;} _LL755: return({ struct Cyc_Tcpat_Success_struct*
_temp784=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp784->tag= Cyc_Tcpat_Success_tag; _temp784->f1=( void*) right_hand_side;(
void*) _temp784;}); _LL757: return(( void*(*)( struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
Cyc_Tcpat_norm_context( ctx), _temp761, right_hand_side, rules); _LL759: if((
_temp782 == 0? 1: _temp780 == 0)? 1: _temp778 == 0){ return(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp785=(
char*)"tcpat:and_match: malformed work frame"; struct _tagged_string _temp786;
_temp786.curr= _temp785; _temp786.base= _temp785; _temp786.last_plus_one=
_temp785 + 38; _temp786;}));}{ struct Cyc_List_List _temp790; struct Cyc_List_List*
_temp791; void* _temp793; struct Cyc_List_List* _temp788=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp787= _temp782; if( _temp787 == 0){ _throw(
Null_Exception);} _temp787;}); _temp790=* _temp788; _LL794: _temp793=( void*)
_temp790.hd; goto _LL792; _LL792: _temp791=( struct Cyc_List_List*) _temp790.tl;
goto _LL789; _LL789: { struct Cyc_List_List _temp798; struct Cyc_List_List*
_temp799; void* _temp801; struct Cyc_List_List* _temp796=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp795= _temp780; if( _temp795 == 0){ _throw(
Null_Exception);} _temp795;}); _temp798=* _temp796; _LL802: _temp801=( void*)
_temp798.hd; goto _LL800; _LL800: _temp799=( struct Cyc_List_List*) _temp798.tl;
goto _LL797; _LL797: { struct Cyc_List_List _temp806; struct Cyc_List_List*
_temp807; void* _temp809; struct Cyc_List_List* _temp804=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp803= _temp778; if( _temp803 == 0){ _throw(
Null_Exception);} _temp803;}); _temp806=* _temp804; _LL810: _temp809=( void*)
_temp806.hd; goto _LL808; _LL808: _temp807=( struct Cyc_List_List*) _temp806.tl;
goto _LL805; _LL805: { struct _tuple12* wf=({ struct _tuple12* _temp811=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp811->f1= _temp791;
_temp811->f2= _temp799; _temp811->f3= _temp807; _temp811;}); return(( void*(*)(
void* p, void* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp793, _temp801, _temp809, ctx,({ struct Cyc_List_List* _temp812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp812->hd=( void*) wf; _temp812->tl=
_temp773; _temp812;}), right_hand_side, rules);}}}} _LL753:;} static struct Cyc_List_List*
Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp813=
dsc; struct Cyc_Set_Set* _temp819; struct Cyc_List_List* _temp821; struct Cyc_Tcpat_Con_s*
_temp823; _LL815: if((( struct _enum_struct*) _temp813)->tag == Cyc_Tcpat_Neg_tag){
_LL820: _temp819=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp813)->f1;
goto _LL816;} else{ goto _LL817;} _LL817: if((( struct _enum_struct*) _temp813)->tag
== Cyc_Tcpat_Pos_tag){ _LL824: _temp823=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*)
_temp813)->f1; goto _LL822; _LL822: _temp821=( struct Cyc_List_List*)(( struct
Cyc_Tcpat_Pos_struct*) _temp813)->f2; goto _LL818;} else{ goto _LL814;} _LL816: {
void* any=({ struct Cyc_Tcpat_Neg_struct* _temp825=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp825->tag= Cyc_Tcpat_Neg_tag;
_temp825->f1= Cyc_Tcpat_empty_con_set();( void*) _temp825;}); struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ res=({ struct Cyc_List_List*
_temp826=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp826->hd=( void*) any; _temp826->tl= res; _temp826;});}} return res;} _LL818:
return _temp821; _LL814:;} static void* Cyc_Tcpat_getoarg( void* obj, int i){
return({ struct Cyc_Tcpat_Sel_struct* _temp827=( struct Cyc_Tcpat_Sel_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct)); _temp827->tag= Cyc_Tcpat_Sel_tag;
_temp827->f1= i + 1; _temp827->f2=( void*) obj;( void*) _temp827;});} static
struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, void*
obj){ return(( struct Cyc_List_List*(*)( int n, void*(* f)( void*, int), void*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules){
void* _temp828= p; struct Cyc_List_List* _temp834; struct Cyc_Tcpat_Con_s*
_temp836; _LL830: if( _temp828 == Cyc_Tcpat_Any){ goto _LL831;} else{ goto
_LL832;} _LL832: if(( unsigned int) _temp828 > 1u?(( struct _enum_struct*)
_temp828)->tag == Cyc_Tcpat_Con_tag: 0){ _LL837: _temp836=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Con_struct*) _temp828)->f1; goto _LL835; _LL835: _temp834=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*) _temp828)->f2; goto
_LL833;} else{ goto _LL829;} _LL831: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc), work,
right_hand_side, rules); _LL833: { void* _temp838= Cyc_Tcpat_static_match(
_temp836, dsc); _LL840: if( _temp838 == Cyc_Tcpat_Yes){ goto _LL841;} else{ goto
_LL842;} _LL842: if( _temp838 == Cyc_Tcpat_No){ goto _LL843;} else{ goto _LL844;}
_LL844: if( _temp838 == Cyc_Tcpat_Maybe){ goto _LL845;} else{ goto _LL839;}
_LL841: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List* _temp846=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp846->hd=( void*)({
struct _tuple11* _temp847=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp847->f1= _temp836; _temp847->f2= 0; _temp847;}); _temp846->tl= ctx;
_temp846;}); struct _tuple12* work_frame=({ struct _tuple12* _temp848=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp848->f1= _temp834;
_temp848->f2= Cyc_Tcpat_getoargs( _temp836, obj); _temp848->f3= Cyc_Tcpat_getdargs(
_temp836, dsc); _temp848;}); struct Cyc_List_List* work2=({ struct Cyc_List_List*
_temp849=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp849->hd=( void*) work_frame; _temp849->tl= work; _temp849;}); return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(( struct Cyc_List_List*) ctx2,(
struct Cyc_List_List*) work2, right_hand_side, rules);} _LL843: return(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL845: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp850=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp850->hd=( void*)({ struct _tuple11* _temp851=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp851->f1= _temp836; _temp851->f2= 0; _temp851;});
_temp850->tl= ctx; _temp850;}); struct _tuple12* work_frame=({ struct _tuple12*
_temp852=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp852->f1=
_temp834; _temp852->f2= Cyc_Tcpat_getoargs( _temp836, obj); _temp852->f3= Cyc_Tcpat_getdargs(
_temp836, dsc); _temp852;}); struct Cyc_List_List* work2=({ struct Cyc_List_List*
_temp853=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp853->hd=( void*) work_frame; _temp853->tl= work; _temp853;}); void* s=((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(( struct Cyc_List_List*)
ctx2,( struct Cyc_List_List*) work2, right_hand_side, rules); void* f=(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp836), work), rules); return({ struct Cyc_Tcpat_IfEq_struct*
_temp854=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp854->tag= Cyc_Tcpat_IfEq_tag; _temp854->f1=( void*) obj; _temp854->f2=
_temp836; _temp854->f3=( void*) s; _temp854->f4=( void*) f;( void*) _temp854;});}
_LL839:;} _LL829:;} static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(*
not_exhaust)( void*), void* env1, void(* rhs_appears)( void*, void*), void* env2){
void* _temp855= d; void* _temp863; void* _temp865; void* _temp867; struct Cyc_Tcpat_Con_s*
_temp869; void* _temp871; _LL857: if( _temp855 == Cyc_Tcpat_Failure){ goto
_LL858;} else{ goto _LL859;} _LL859: if(( unsigned int) _temp855 > 1u?(( struct
_enum_struct*) _temp855)->tag == Cyc_Tcpat_Success_tag: 0){ _LL864: _temp863=(
void*)(( struct Cyc_Tcpat_Success_struct*) _temp855)->f1; goto _LL860;} else{
goto _LL861;} _LL861: if(( unsigned int) _temp855 > 1u?(( struct _enum_struct*)
_temp855)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL872: _temp871=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp855)->f1; goto _LL870; _LL870: _temp869=( struct Cyc_Tcpat_Con_s*)(( struct
Cyc_Tcpat_IfEq_struct*) _temp855)->f2; goto _LL868; _LL868: _temp867=( void*)((
struct Cyc_Tcpat_IfEq_struct*) _temp855)->f3; goto _LL866; _LL866: _temp865=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp855)->f4; goto _LL862;} else{ goto
_LL856;} _LL858: not_exhaust( env1); goto _LL856; _LL860: rhs_appears( env2,
_temp863); goto _LL856; _LL862:(( void(*)( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp867, not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(*
not_exhaust)( void*), void* env1, void(* rhs_appears)( void*, void*), void* env2))
Cyc_Tcpat_check_exhaust_overlap)( _temp865, not_exhaust, env1, rhs_appears, env2);
goto _LL856; _LL856:;} static struct _tuple14* Cyc_Tcpat_get_match( int* r,
struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple13* rhs=({ struct _tuple13* _temp873=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp873->f1= 0; _temp873->f2=( swc->pattern)->loc;
_temp873;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp874=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp874->hd=( void*) sp0; _temp874->tl=({ struct Cyc_List_List*
_temp875=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp875->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp875->tl= 0; _temp875;});
_temp874;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp876=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp876->hd=( void*) sp0; _temp876->tl=({ struct Cyc_List_List* _temp877=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp877->hd=(
void*) Cyc_Tcpat_Any; _temp877->tl= 0; _temp877;}); _temp876;}));} return({
struct _tuple14* _temp878=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp878->f1= sp; _temp878->f2= rhs; _temp878;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp879=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp880; _temp880.curr= _temp879; _temp880.base= _temp879; _temp880.last_plus_one=
_temp879 + 31; _temp880;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple13* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple14*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,( int*)({
struct _tuple15* _temp881=( struct _tuple15*) GC_malloc_atomic( sizeof( struct
_tuple15)); _temp881->f1= 0; _temp881;}), swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple13*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules= match_rules->tl){ struct _tuple13 _temp884;
struct Cyc_Position_Segment* _temp885; int _temp887; struct _tuple13* _temp882=(*((
struct _tuple14*) match_rules->hd)).f2; _temp884=* _temp882; _LL888: _temp887=
_temp884.f1; goto _LL886; _LL886: _temp885= _temp884.f2; goto _LL883; _LL883:
if( ! _temp887){ Cyc_Tcutil_terr( _temp885,( struct _tagged_string)({ char*
_temp889=( char*)"redundant pattern"; struct _tagged_string _temp890; _temp890.curr=
_temp889; _temp890.base= _temp889; _temp890.last_plus_one= _temp889 + 18;
_temp890;}));}}} static void Cyc_Tcpat_not_exhaust_warn( struct _tuple16* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({ char* _temp891=(
char*)"pattern not exhaustive"; struct _tagged_string _temp892; _temp892.curr=
_temp891; _temp892.base= _temp891; _temp892.last_plus_one= _temp891 + 23;
_temp892;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp893=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp893->hd=( void*)({ struct _tuple17*
_temp894=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp894->f1=
Cyc_Tcpat_compile_pat( p); _temp894->f2= 0; _temp894;}); _temp893->tl= 0;
_temp893;}); void* dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)((
struct Cyc_List_List*) match_rules); struct _tuple16* exhaust_env=({ struct
_tuple16* _temp895=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp895->f1= loc; _temp895->f2= 1; _temp895;});(( void(*)( void* d, void(*
not_exhaust)( struct _tuple16*), struct _tuple16* env1, void(* rhs_appears)( int,
int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_warn,
exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(* exhaust_env).f2;} static struct
_tuple14* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple13* rhs=({ struct _tuple13*
_temp896=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp896->f1=
0; _temp896->f2=( swc->pattern)->loc; _temp896;}); return({ struct _tuple14*
_temp897=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp897->f1=
sp0; _temp897->f2= rhs; _temp897;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple14*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple13*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=
match_rules->tl){ struct _tuple13* pair=(*(( struct _tuple14*) match_rules->hd)).f2;
if( !(* pair).f1){ Cyc_Tcutil_terr((* pair).f2,( struct _tagged_string)({ char*
_temp898=( char*)"redundant pattern"; struct _tagged_string _temp899; _temp899.curr=
_temp898; _temp899.base= _temp898; _temp899.last_plus_one= _temp898 + 18;
_temp899;}));}}}