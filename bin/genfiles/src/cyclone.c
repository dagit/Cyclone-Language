#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tagged_ptr0{
int* curr; int* base; int* last_plus_one; } ; struct _tuple3{ void* f1; int f2;
} ; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; struct _tuple6{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct _tuple7{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ;
struct _tuple8{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple9{ struct _tagged_string f1; } ; struct _tuple10{ struct _tagged_string f1;
void* f2; struct _tagged_string f3; } ; typedef unsigned int Cyc_uint; typedef
char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
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
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern char Cyc_Arg_Error_tag[ 6u];
struct Cyc_Arg_Error_struct{ char* tag; } ; extern char Cyc_Arg_Bad_tag[ 4u];
struct Cyc_Arg_Bad_struct{ char* tag; struct _tagged_string f1; } ; typedef void*
Cyc_Arg_Spec; extern const int Cyc_Arg_Unit_spec_tag; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; extern const int Cyc_Arg_Set_spec_tag; struct Cyc_Arg_Set_spec_struct{
int tag; int* f1; } ; extern const int Cyc_Arg_Clear_spec_tag; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1; } ; extern const int Cyc_Arg_String_spec_tag; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; extern const int Cyc_Arg_Int_spec_tag;
struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; extern const int
Cyc_Arg_Rest_spec_tag; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t; typedef void* Cyc_Arg_spec_t;
extern void Cyc_Arg_parse( struct Cyc_List_List* specs, void(* anonfun)( struct
_tagged_string), struct _tagged_string errmsg); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef
unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_remove(
struct _tagged_string); extern int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_string fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); extern int Cyc_String_strcmp( struct _tagged_string
s1, struct _tagged_string s2); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List*, struct _tagged_string); struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern struct _tagged_string Cyc_Filename_chop_extension( struct
_tagged_string); struct Cyc_Id_id; typedef struct Cyc_Id_id* Cyc_Id_Id; struct
Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string line;
int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{ char*
tag; } ; extern void Cyc_Position_reset_position( struct _tagged_string); struct
Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex; extern
void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern int Cyc_Position_error_p(); typedef struct _tagged_string*
Cyc_Absyn_field_name; typedef struct _tagged_string* Cyc_Absyn_var; typedef
struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace;
typedef struct _tuple0* Cyc_Absyn_qvar; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
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
; extern char Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f); typedef void* Cyc_Struct_or_union;
typedef void* Cyc_struct_or_union_t; typedef void* Cyc_Blockitem; typedef void*
Cyc_blockitem_t; typedef void* Cyc_Type_specifier; typedef void* Cyc_type_specifier_t;
typedef void* Cyc_Storage_class; typedef void* Cyc_storage_class_t; struct Cyc_Declaration_spec;
typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t; struct Cyc_Declarator;
typedef struct Cyc_Declarator* Cyc_declarator_t; struct Cyc_Abstractdeclarator;
typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t; typedef struct
_xenum_struct* Cyc_YYSTYPE; extern char Cyc_Okay_tok_tag[ 9u]; struct Cyc_Okay_tok_struct{
char* tag; } ; extern char Cyc_Int_tok_tag[ 8u]; struct Cyc_Int_tok_struct{ char*
tag; struct _tuple3* f1; } ; extern char Cyc_Char_tok_tag[ 9u]; struct Cyc_Char_tok_struct{
char* tag; char f1; } ; extern char Cyc_Pointer_Sort_tok_tag[ 17u]; struct Cyc_Pointer_Sort_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Short_tok_tag[ 10u]; struct Cyc_Short_tok_struct{
char* tag; short f1; } ; extern char Cyc_String_tok_tag[ 11u]; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stringopt_tok_tag[ 14u];
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; extern
char Cyc_Type_tok_tag[ 9u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_TypeList_tok_tag[ 13u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok_tag[ 8u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_ExpList_tok_tag[ 12u];
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_Primop_tok_tag[ 11u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1;
} ; extern char Cyc_Primopopt_tok_tag[ 14u]; struct Cyc_Primopopt_tok_struct{
char* tag; struct Cyc_Core_Opt* f1; } ; extern char Cyc_QualId_tok_tag[ 11u];
struct Cyc_QualId_tok_struct{ char* tag; struct _tuple0* f1; } ; extern char Cyc_Stmt_tok_tag[
9u]; struct Cyc_Stmt_tok_struct{ char* tag; struct Cyc_Absyn_Stmt* f1; } ;
extern char Cyc_SwitchClauseList_tok_tag[ 21u]; struct Cyc_SwitchClauseList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok_tag[ 12u];
struct Cyc_Pattern_tok_struct{ char* tag; struct Cyc_Absyn_Pat* f1; } ; extern
char Cyc_PatternList_tok_tag[ 16u]; struct Cyc_PatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_FnDecl_tok_tag[ 11u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_DeclList_tok_tag[ 13u];
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_DeclSpec_tok_tag[ 13u]; struct Cyc_DeclSpec_tok_struct{ char* tag;
struct Cyc_Declaration_spec* f1; } ; extern char Cyc_InitDecl_tok_tag[ 13u];
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple4* f1; } ; extern char
Cyc_InitDeclList_tok_tag[ 17u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_StorageClass_tok_tag[ 17u]; struct
Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_TypeSpecifier_tok_tag[
18u]; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char
Cyc_QualSpecList_tok_tag[ 17u]; struct Cyc_QualSpecList_tok_struct{ char* tag;
struct _tuple5* f1; } ; extern char Cyc_TypeQual_tok_tag[ 13u]; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual* f1; } ; extern char Cyc_StructFieldDeclList_tok_tag[
24u]; struct Cyc_StructFieldDeclList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclListList_tok_tag[ 28u]; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Declarator_tok_tag[ 15u];
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Declarator* f1; } ;
extern char Cyc_DeclaratorExpopt_tok_tag[ 21u]; struct Cyc_DeclaratorExpopt_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_DeclaratorExpoptList_tok_tag[
25u]; struct Cyc_DeclaratorExpoptList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_AbstractDeclarator_tok_tag[ 23u]; struct Cyc_AbstractDeclarator_tok_struct{
char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern char Cyc_EnumeratorField_tok_tag[
20u]; struct Cyc_EnumeratorField_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield*
f1; } ; extern char Cyc_EnumeratorFieldList_tok_tag[ 24u]; struct Cyc_EnumeratorFieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDecl_tok_tag[ 14u];
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple1* f1; } ; extern char
Cyc_ParamDeclList_tok_tag[ 18u]; struct Cyc_ParamDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_ParamDeclListBool_tok_tag[ 22u];
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple7* f1; } ;
extern char Cyc_StructOrUnion_tok_tag[ 18u]; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; extern char Cyc_IdList_tok_tag[ 11u]; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Designator_tok_tag[ 15u];
struct Cyc_Designator_tok_struct{ char* tag; void* f1; } ; extern char Cyc_DesignatorList_tok_tag[
19u]; struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_TypeModifierList_tok_tag[ 21u]; struct Cyc_TypeModifierList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Rgn_tok_tag[ 8u];
struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ; extern char Cyc_InitializerList_tok_tag[
20u]; struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_FieldPattern_tok_tag[ 17u]; struct Cyc_FieldPattern_tok_struct{
char* tag; struct _tuple8* f1; } ; extern char Cyc_FieldPatternList_tok_tag[ 21u];
struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_BlockItem_tok_tag[ 14u]; struct Cyc_BlockItem_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_Kind_tok_tag[ 9u]; struct Cyc_Kind_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Attribute_tok_tag[ 14u]; struct Cyc_Attribute_tok_struct{
char* tag; void* f1; } ; extern char Cyc_AttributeList_tok_tag[ 18u]; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); struct Cyc_Set_Set;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern char Cyc_Tcutil_TypeErr_tag[ 8u];
struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void Cyc_Tcutil_flush_warnings();
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern int Cyc_Toc_check_for_null; typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
extern const int Cyc_CfFlowInfo_VarRoot_tag; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern const int Cyc_CfFlowInfo_MallocPt_tag;
struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_CfFlowInfo_StructF_tag; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; extern const int Cyc_CfFlowInfo_TupleF_tag;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; extern void* Cyc_CfFlowInfo_Esc;
extern void* Cyc_CfFlowInfo_Unesc; extern void* Cyc_CfFlowInfo_NoneIL; extern
void* Cyc_CfFlowInfo_ThisIL; extern void* Cyc_CfFlowInfo_AllIL; extern const int
Cyc_CfFlowInfo_UnknownIS_tag; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag;
void* f1; void* f2; } ; extern const int Cyc_CfFlowInfo_MustPointTo_tag; struct
Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1; } ;
extern const int Cyc_CfFlowInfo_LeafPI_tag; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; extern const int Cyc_CfFlowInfo_TuplePI_tag; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern const int Cyc_CfFlowInfo_StructPI_tag;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_BottomFL; extern const int Cyc_CfFlowInfo_InitsFL_tag;
struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict* roots; int in_try; void*
tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv* Cyc_NewControlFlow_analenv_t;
void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); extern void Cyc_Lex_lex_init();
static int Cyc_pp_r= 0; static int Cyc_up_r= 0; static int Cyc_noexpand_r= 0;
static int Cyc_noshake_r= 0; static int Cyc_tc_r= 0; static int Cyc_toc_r= 0;
static int Cyc_v_r= 0; static int Cyc_q_r= 0; static int Cyc_add_cyc_namespace_r=
1; int* Cyc_pp=& Cyc_pp_r; int* Cyc_up=& Cyc_up_r; int* Cyc_noexpand=& Cyc_noexpand_r;
int* Cyc_noshake=& Cyc_noshake_r; int* Cyc_tc=& Cyc_tc_r; int* Cyc_toc=& Cyc_toc_r;
int* Cyc_v=& Cyc_v_r; int* Cyc_q=& Cyc_q_r; int* Cyc_add_cyc_namespace=& Cyc_add_cyc_namespace_r;
static struct _tagged_string* Cyc_out_file_name_opt= 0; static void Cyc_set_out_file_name(
struct _tagged_string s){ Cyc_out_file_name_opt=( struct _tagged_string*)({
struct _tuple9* _temp0=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp0->f1= s; _temp0;});} static void Cyc_set_check_null(){ Cyc_Toc_check_for_null=
1;} static struct Cyc_List_List* Cyc_includes= 0; static void Cyc_add_include(
struct _tagged_string s){ Cyc_includes=({ struct Cyc_List_List* _temp1=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)((
struct _tagged_string*)({ struct _tagged_string* _temp2=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp2[ 0]= s; _temp2;}));
_temp1->tl= Cyc_includes; _temp1;});} static struct Cyc_List_List* Cyc_defines=
0; static void Cyc_add_define( struct _tagged_string s){ Cyc_defines=({ struct
Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3->hd=( void*)(( struct _tagged_string*)({ struct _tagged_string* _temp4=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4[
0]= s; _temp4;})); _temp3->tl= Cyc_defines; _temp3;});} int Cyc_compile_failure=
0; struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct _tagged_string filename,
struct _tagged_string mode, struct _tagged_string msg_part){ struct
_handler_cons _temp5; _push_handler(& _temp5);{ struct _xenum_struct* _temp6=(
struct _xenum_struct*) setjmp( _temp5.handler); if( ! _temp6){{ struct Cyc_Stdio___sFILE*
_temp7=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open( filename, mode);
_npop_handler( 0u); return _temp7;}; _pop_handler();} else{ struct _xenum_struct*
_temp9= _temp6; _LL11: goto _LL12; _LL13: goto _LL14; _LL12: Cyc_compile_failure=
1;({ struct _tagged_string _temp15= msg_part; struct _tagged_string _temp16=
filename; fprintf( Cyc_Stdio_stderr,"\nError: couldn't open %.*s %.*s\n",
_temp15.last_plus_one - _temp15.curr, _temp15.curr, _temp16.last_plus_one -
_temp16.curr, _temp16.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return 0; _LL14:( void) _throw( _temp9); _LL10:;}}} struct
Cyc_List_List* Cyc_do_stage( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( void*, struct Cyc_List_List*), void* env, struct Cyc_List_List* tds, struct
_tagged_string preprocfile){ struct _tagged_string exn_string=( struct
_tagged_string)({ char* _temp17=( char*)""; struct _tagged_string _temp18;
_temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one= _temp17 + 1;
_temp18;}); struct _tagged_string explain_string=( struct _tagged_string)({ char*
_temp19=( char*)""; struct _tagged_string _temp20; _temp20.curr= _temp19;
_temp20.base= _temp19; _temp20.last_plus_one= _temp19 + 1; _temp20;}); int
other_exn= 0; struct _xenum_struct* ex=({ struct Cyc_Core_Impossible_struct*
_temp21=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp21->tag= Cyc_Core_Impossible_tag; _temp21->f1=( struct _tagged_string)({
char* _temp22=( char*)""; struct _tagged_string _temp23; _temp23.curr= _temp22;
_temp23.base= _temp22; _temp23.last_plus_one= _temp22 + 1; _temp23;});( struct
_xenum_struct*) _temp21;}); struct Cyc_List_List* ans= 0;{ struct _handler_cons
_temp24; _push_handler(& _temp24);{ struct _xenum_struct* _temp25=( struct
_xenum_struct*) setjmp( _temp24.handler); if( ! _temp25){ ans= f( env, tds);;
_pop_handler();} else{ struct _xenum_struct* _temp27= _temp25; struct
_tagged_string _temp37; _LL29: if((*(( struct _xenum_struct*) _temp27)).tag ==
Cyc_Core_Impossible_tag){ _LL38: _temp37=(( struct Cyc_Core_Impossible_struct*)
_temp27)->f1; goto _LL30;} else{ goto _LL31;} _LL31: if( _temp27->tag == Cyc_Dict_Absent_tag){
goto _LL32;} else{ goto _LL33;} _LL33: goto _LL34; _LL35: goto _LL36; _LL30:
exn_string=( struct _tagged_string)({ char* _temp39=( char*)"Exception Core::Impossible";
struct _tagged_string _temp40; _temp40.curr= _temp39; _temp40.base= _temp39;
_temp40.last_plus_one= _temp39 + 27; _temp40;}); explain_string= _temp37; goto
_LL28; _LL32: exn_string=( struct _tagged_string)({ char* _temp41=( char*)"Exception Dict::Absent";
struct _tagged_string _temp42; _temp42.curr= _temp41; _temp42.base= _temp41;
_temp42.last_plus_one= _temp41 + 23; _temp42;}); goto _LL28; _LL34: ex= _temp27;
other_exn= 1; exn_string=( struct _tagged_string)({ char* _temp43=( char*)"Uncaught exception";
struct _tagged_string _temp44; _temp44.curr= _temp43; _temp44.base= _temp43;
_temp44.last_plus_one= _temp43 + 19; _temp44;}); goto _LL28; _LL36:( void)
_throw( _temp27); _LL28:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,( struct _tagged_string)({ char* _temp45=(
char*)""; struct _tagged_string _temp46; _temp46.curr= _temp45; _temp46.base=
_temp45; _temp46.last_plus_one= _temp45 + 1; _temp46;})) != 0){ Cyc_compile_failure=
1;({ struct _tagged_string _temp47= exn_string; struct _tagged_string _temp48=
stage_name; struct _tagged_string _temp49= explain_string; fprintf( Cyc_Stdio_stderr,"\n%.*s thrown during %.*s: %.*s",
_temp47.last_plus_one - _temp47.curr, _temp47.curr, _temp48.last_plus_one -
_temp48.curr, _temp48.curr, _temp49.last_plus_one - _temp49.curr, _temp49.curr);});}
if( Cyc_compile_failure){ fprintf( Cyc_Stdio_stderr,"\nCOMPILATION FAILED!\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); Cyc_Stdio_remove(
preprocfile); if( other_exn){( void) _throw( ex);} return ans;} if(* Cyc_v){({
struct _tagged_string _temp50= stage_name; fprintf( Cyc_Stdio_stderr,"%.*s completed.\n",
_temp50.last_plus_one - _temp50.curr, _temp50.curr);});} else{ if( !(* Cyc_q)){
fprintf( Cyc_Stdio_stderr,".");}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return ans;} struct Cyc_List_List* Cyc_do_parse( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init(); return Cyc_Parse_parse_file(
f);} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= !(* Cyc_noexpand); Cyc_Tc_tc(
te, Cyc_add_cyc_namespace_r? Cyc_toc_r: 0, tds); if( !(* Cyc_noshake)){ tds= Cyc_Tc_treeshake(
te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck( int ignore, struct
Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds); return tds;} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_print( struct Cyc_Stdio___sFILE* out_file,
struct Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= !(* Cyc_noexpand); Cyc_Absynpp_decls_first=(
Cyc_Absynpp_qvar_to_Cids=* Cyc_toc); Cyc_Absynpp_print_using_stmts=( Cyc_Absynpp_print_externC_stmts=
!(* Cyc_tc)); if(* Cyc_toc){ fprintf( out_file,"#include \"cyc_include.h\"\n\n");
Cyc_Absynpp_qvar_to_Cids= 1;} if(* Cyc_pp){ Cyc_Absynpp_decllist2file( tds,
out_file);} else{ if(* Cyc_up){ Cyc_Absyndump_dumpdecllist2file( tds, out_file);}}
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) out_file); return tds;} void Cyc_compile_file(
struct _tagged_string filename){ struct _tagged_string basename= Cyc_Filename_chop_extension(
filename); struct _tagged_string preprocfile= Cyc_String_strconcat( basename,(
struct _tagged_string)({ char* _temp51=( char*)".i"; struct _tagged_string
_temp52; _temp52.curr= _temp51; _temp52.base= _temp51; _temp52.last_plus_one=
_temp51 + 3; _temp52;})); if( !(* Cyc_q)? 1:* Cyc_v){({ struct _tagged_string
_temp53= basename; fprintf( Cyc_Stdio_stderr,"Compiling file: %.*s", _temp53.last_plus_one
- _temp53.curr, _temp53.curr);});} if(* Cyc_v){ fprintf( Cyc_Stdio_stderr,"\n");}{
struct _tagged_string inc_string= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp54=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp54->hd=( void*)({ struct _tagged_string* _temp55=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp55[ 0]=( struct
_tagged_string)({ char* _temp56=( char*)""; struct _tagged_string _temp57;
_temp57.curr= _temp56; _temp57.base= _temp56; _temp57.last_plus_one= _temp56 + 1;
_temp57;}); _temp55;}); _temp54->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_includes); _temp54;}),( struct _tagged_string)({ char*
_temp58=( char*)" -I"; struct _tagged_string _temp59; _temp59.curr= _temp58;
_temp59.base= _temp58; _temp59.last_plus_one= _temp58 + 4; _temp59;})); struct
_tagged_string def_string= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp60=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp60->hd=( void*)({ struct _tagged_string* _temp61=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp61[ 0]=( struct
_tagged_string)({ char* _temp62=( char*)""; struct _tagged_string _temp63;
_temp63.curr= _temp62; _temp63.base= _temp62; _temp63.last_plus_one= _temp62 + 1;
_temp63;}); _temp61;}); _temp60->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( Cyc_defines); _temp60;}),( struct _tagged_string)({ char*
_temp64=( char*)" -D"; struct _tagged_string _temp65; _temp65.curr= _temp64;
_temp65.base= _temp64; _temp65.last_plus_one= _temp64 + 4; _temp65;})); struct
Cyc_Stdio___sFILE* f0= Cyc_try_file_open( filename,( struct _tagged_string)({
char* _temp66=( char*)"r"; struct _tagged_string _temp67; _temp67.curr= _temp66;
_temp67.base= _temp66; _temp67.last_plus_one= _temp66 + 2; _temp67;}),( struct
_tagged_string)({ char* _temp68=( char*)"input file"; struct _tagged_string
_temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 11; _temp69;})); if( Cyc_compile_failure){ return;} Cyc_Stdio_fclose((
struct Cyc_Stdio___sFILE*)(( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp70= f0; if( _temp70 == 0){ _throw( Null_Exception);} _temp70;})));{ struct
_tagged_string cmd=({ struct _tagged_string _temp71= inc_string; struct
_tagged_string _temp72= def_string; struct _tagged_string _temp73= filename;
struct _tagged_string _temp74= preprocfile; xprintf("gcc -x c -E -U__GNUC__ %.*s %.*s %.*s > %.*s",
_temp71.last_plus_one - _temp71.curr, _temp71.curr, _temp72.last_plus_one -
_temp72.curr, _temp72.curr, _temp73.last_plus_one - _temp73.curr, _temp73.curr,
_temp74.last_plus_one - _temp74.curr, _temp74.curr);}); if(* Cyc_v){({ struct
_tagged_string _temp75= cmd; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp75.last_plus_one
- _temp75.curr, _temp75.curr);});} if( system( string_to_Cstring( cmd)) != 0){
Cyc_compile_failure= 1; fprintf( Cyc_Stdio_stderr,"\nError: pre-processing failed!\n");
return;} Cyc_Position_reset_position( preprocfile);{ struct Cyc_Stdio___sFILE*
in_file= Cyc_try_file_open( preprocfile,( struct _tagged_string)({ char* _temp76=(
char*)"r"; struct _tagged_string _temp77; _temp77.curr= _temp76; _temp77.base=
_temp76; _temp77.last_plus_one= _temp76 + 2; _temp77;}),( struct _tagged_string)({
char* _temp78=( char*)"file"; struct _tagged_string _temp79; _temp79.curr=
_temp78; _temp79.base= _temp78; _temp79.last_plus_one= _temp78 + 5; _temp79;}));
struct Cyc_Stdio___sFILE* out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;
if( Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0; if( ! Cyc_compile_failure){
tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*), struct Cyc_Stdio___sFILE*
env, struct Cyc_List_List* tds, struct _tagged_string preprocfile)) Cyc_do_stage)((
struct _tagged_string)({ char* _temp80=( char*)"parsing"; struct _tagged_string
_temp81; _temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one=
_temp80 + 8; _temp81;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp82= in_file; if( _temp82 == 0){ _throw( Null_Exception);} _temp82;}), tds,
preprocfile); Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp83= in_file; if( _temp83 == 0){ _throw( Null_Exception);} _temp83;}));} if((
! Cyc_compile_failure? tds != 0: 0)?* Cyc_tc: 0){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds,
struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp84=( char*)"type checking"; struct _tagged_string _temp85; _temp85.curr=
_temp84; _temp85.base= _temp84; _temp85.last_plus_one= _temp84 + 14; _temp85;}),
Cyc_do_typecheck, Cyc_Tcenv_tc_init(), tds, preprocfile);} if( ! Cyc_compile_failure?*
Cyc_tc: 0){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds, struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp86=( char*)"control-flow checking"; struct _tagged_string _temp87;
_temp87.curr= _temp86; _temp87.base= _temp86; _temp87.last_plus_one= _temp86 +
22; _temp87;}), Cyc_do_cfcheck, 1, tds, preprocfile);} if( ! Cyc_compile_failure?*
Cyc_toc: 0){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds, struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp88=( char*)"translation to C"; struct _tagged_string _temp89; _temp89.curr=
_temp88; _temp89.base= _temp88; _temp89.last_plus_one= _temp88 + 17; _temp89;}),
Cyc_do_translate, 1, tds, preprocfile);} if( ! Cyc_compile_failure? Cyc_out_file_name_opt
!= 0: 0){ out_file= Cyc_try_file_open(* Cyc_out_file_name_opt,( struct
_tagged_string)({ char* _temp90=( char*)"w"; struct _tagged_string _temp91;
_temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one= _temp90 + 2;
_temp91;}),( struct _tagged_string)({ char* _temp92=( char*)"output file";
struct _tagged_string _temp93; _temp93.curr= _temp92; _temp93.base= _temp92;
_temp93.last_plus_one= _temp92 + 12; _temp93;}));} if(( ! Cyc_compile_failure?
tds != 0: 0)?* Cyc_pp? 1:* Cyc_up: 0){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds,
struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp94=( char*)"printing"; struct _tagged_string _temp95; _temp95.curr=
_temp94; _temp95.base= _temp94; _temp95.last_plus_one= _temp94 + 9; _temp95;}),
Cyc_do_print,( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE* _temp96=
out_file; if( _temp96 == 0){ _throw( Null_Exception);} _temp96;}), tds,
preprocfile);} if( out_file != 0? out_file != Cyc_Stdio_stdout: 0){ Cyc_Stdio_file_close((
struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE* _temp97= out_file; if(
_temp97 == 0){ _throw( Null_Exception);} _temp97;}));} fprintf( Cyc_Stdio_stderr,"\n");
if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();} Cyc_Stdio_remove(
preprocfile); return;}}}}} int Cyc_main(){ struct _tuple10 specs[ 15]={({ struct
_tuple10 _temp98; _temp98.f1=( struct _tagged_string)({ char* _temp102=( char*)"-nocyc";
struct _tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 7; _temp103;}); _temp98.f2=({ struct Cyc_Arg_Clear_spec_struct*
_temp101=( struct Cyc_Arg_Clear_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp101->tag= Cyc_Arg_Clear_spec_tag; _temp101->f1= Cyc_add_cyc_namespace;(
void*) _temp101;}); _temp98.f3=( struct _tagged_string)({ char* _temp99=( char*)"does not add implicit namespace Cyc";
struct _tagged_string _temp100; _temp100.curr= _temp99; _temp100.base= _temp99;
_temp100.last_plus_one= _temp99 + 36; _temp100;}); _temp98;}),({ struct _tuple10
_temp104; _temp104.f1=( struct _tagged_string)({ char* _temp108=( char*)"-q";
struct _tagged_string _temp109; _temp109.curr= _temp108; _temp109.base= _temp108;
_temp109.last_plus_one= _temp108 + 3; _temp109;}); _temp104.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp107=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp107->tag= Cyc_Arg_Set_spec_tag; _temp107->f1= Cyc_q;( void*) _temp107;});
_temp104.f3=( struct _tagged_string)({ char* _temp105=( char*)"compile quietly";
struct _tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 16; _temp106;}); _temp104;}),({ struct
_tuple10 _temp110; _temp110.f1=( struct _tagged_string)({ char* _temp114=( char*)"-v";
struct _tagged_string _temp115; _temp115.curr= _temp114; _temp115.base= _temp114;
_temp115.last_plus_one= _temp114 + 3; _temp115;}); _temp110.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp113=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp113->tag= Cyc_Arg_Set_spec_tag; _temp113->f1= Cyc_v;( void*) _temp113;});
_temp110.f3=( struct _tagged_string)({ char* _temp111=( char*)"print compilation stages verbosely";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 35; _temp112;}); _temp110;}),({ struct
_tuple10 _temp116; _temp116.f1=( struct _tagged_string)({ char* _temp120=( char*)"-check_null";
struct _tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 12; _temp121;}); _temp116.f2=({ struct Cyc_Arg_Unit_spec_struct*
_temp119=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp119->tag= Cyc_Arg_Unit_spec_tag; _temp119->f1= Cyc_set_check_null;( void*)
_temp119;}); _temp116.f3=( struct _tagged_string)({ char* _temp117=( char*)"turn on null-checking in generated C code";
struct _tagged_string _temp118; _temp118.curr= _temp117; _temp118.base= _temp117;
_temp118.last_plus_one= _temp117 + 42; _temp118;}); _temp116;}),({ struct
_tuple10 _temp122; _temp122.f1=( struct _tagged_string)({ char* _temp126=( char*)"-D";
struct _tagged_string _temp127; _temp127.curr= _temp126; _temp127.base= _temp126;
_temp127.last_plus_one= _temp126 + 3; _temp127;}); _temp122.f2=({ struct Cyc_Arg_String_spec_struct*
_temp125=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp125->tag= Cyc_Arg_String_spec_tag; _temp125->f1= Cyc_add_define;( void*)
_temp125;}); _temp122.f3=( struct _tagged_string)({ char* _temp123=( char*)"pass definition to pre-processor";
struct _tagged_string _temp124; _temp124.curr= _temp123; _temp124.base= _temp123;
_temp124.last_plus_one= _temp123 + 33; _temp124;}); _temp122;}),({ struct
_tuple10 _temp128; _temp128.f1=( struct _tagged_string)({ char* _temp132=( char*)"-I";
struct _tagged_string _temp133; _temp133.curr= _temp132; _temp133.base= _temp132;
_temp133.last_plus_one= _temp132 + 3; _temp133;}); _temp128.f2=({ struct Cyc_Arg_String_spec_struct*
_temp131=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp131->tag= Cyc_Arg_String_spec_tag; _temp131->f1= Cyc_add_include;( void*)
_temp131;}); _temp128.f3=( struct _tagged_string)({ char* _temp129=( char*)"include directory";
struct _tagged_string _temp130; _temp130.curr= _temp129; _temp130.base= _temp129;
_temp130.last_plus_one= _temp129 + 18; _temp130;}); _temp128;}),({ struct
_tuple10 _temp134; _temp134.f1=( struct _tagged_string)({ char* _temp138=( char*)"-toc";
struct _tagged_string _temp139; _temp139.curr= _temp138; _temp139.base= _temp138;
_temp139.last_plus_one= _temp138 + 5; _temp139;}); _temp134.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp137=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp137->tag= Cyc_Arg_Set_spec_tag; _temp137->f1= Cyc_toc;( void*) _temp137;});
_temp134.f3=( struct _tagged_string)({ char* _temp135=( char*)"translate to C";
struct _tagged_string _temp136; _temp136.curr= _temp135; _temp136.base= _temp135;
_temp136.last_plus_one= _temp135 + 15; _temp136;}); _temp134;}),({ struct
_tuple10 _temp140; _temp140.f1=( struct _tagged_string)({ char* _temp144=( char*)"-tc";
struct _tagged_string _temp145; _temp145.curr= _temp144; _temp145.base= _temp144;
_temp145.last_plus_one= _temp144 + 4; _temp145;}); _temp140.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp143=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp143->tag= Cyc_Arg_Set_spec_tag; _temp143->f1= Cyc_tc;( void*) _temp143;});
_temp140.f3=( struct _tagged_string)({ char* _temp141=( char*)"type check";
struct _tagged_string _temp142; _temp142.curr= _temp141; _temp142.base= _temp141;
_temp142.last_plus_one= _temp141 + 11; _temp142;}); _temp140;}),({ struct
_tuple10 _temp146; _temp146.f1=( struct _tagged_string)({ char* _temp150=( char*)"-noremoveunused";
struct _tagged_string _temp151; _temp151.curr= _temp150; _temp151.base= _temp150;
_temp151.last_plus_one= _temp150 + 16; _temp151;}); _temp146.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp149=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp149->tag= Cyc_Arg_Set_spec_tag; _temp149->f1= Cyc_noshake;( void*) _temp149;});
_temp146.f3=( struct _tagged_string)({ char* _temp147=( char*)"don't remove externed variables that aren't used";
struct _tagged_string _temp148; _temp148.curr= _temp147; _temp148.base= _temp147;
_temp148.last_plus_one= _temp147 + 49; _temp148;}); _temp146;}),({ struct
_tuple10 _temp152; _temp152.f1=( struct _tagged_string)({ char* _temp156=( char*)"-noexpandtypedefs";
struct _tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 18; _temp157;}); _temp152.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp155=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp155->tag= Cyc_Arg_Set_spec_tag; _temp155->f1= Cyc_noexpand;( void*)
_temp155;}); _temp152.f3=( struct _tagged_string)({ char* _temp153=( char*)"don't expand typedefs in pretty printing";
struct _tagged_string _temp154; _temp154.curr= _temp153; _temp154.base= _temp153;
_temp154.last_plus_one= _temp153 + 41; _temp154;}); _temp152;}),({ struct
_tuple10 _temp158; _temp158.f1=( struct _tagged_string)({ char* _temp162=( char*)"-printalltvars";
struct _tagged_string _temp163; _temp163.curr= _temp162; _temp163.base= _temp162;
_temp163.last_plus_one= _temp162 + 15; _temp163;}); _temp158.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp161=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp161->tag= Cyc_Arg_Set_spec_tag; _temp161->f1=& Cyc_Absynpp_print_all_tvars;(
void*) _temp161;}); _temp158.f3=( struct _tagged_string)({ char* _temp159=( char*)"print all type variables (even implicit default effects)";
struct _tagged_string _temp160; _temp160.curr= _temp159; _temp160.base= _temp159;
_temp160.last_plus_one= _temp159 + 57; _temp160;}); _temp158;}),({ struct
_tuple10 _temp164; _temp164.f1=( struct _tagged_string)({ char* _temp168=( char*)"-printallkinds";
struct _tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 15; _temp169;}); _temp164.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp167=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp167->tag= Cyc_Arg_Set_spec_tag; _temp167->f1=& Cyc_Absynpp_print_all_kinds;(
void*) _temp167;}); _temp164.f3=( struct _tagged_string)({ char* _temp165=( char*)"always print kinds of type variables";
struct _tagged_string _temp166; _temp166.curr= _temp165; _temp166.base= _temp165;
_temp166.last_plus_one= _temp165 + 37; _temp166;}); _temp164;}),({ struct
_tuple10 _temp170; _temp170.f1=( struct _tagged_string)({ char* _temp174=( char*)"-up";
struct _tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 4; _temp175;}); _temp170.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp173=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp173->tag= Cyc_Arg_Set_spec_tag; _temp173->f1= Cyc_up;( void*) _temp173;});
_temp170.f3=( struct _tagged_string)({ char* _temp171=( char*)"ugly print";
struct _tagged_string _temp172; _temp172.curr= _temp171; _temp172.base= _temp171;
_temp172.last_plus_one= _temp171 + 11; _temp172;}); _temp170;}),({ struct
_tuple10 _temp176; _temp176.f1=( struct _tagged_string)({ char* _temp180=( char*)"-pp";
struct _tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 4; _temp181;}); _temp176.f2=({ struct Cyc_Arg_Set_spec_struct*
_temp179=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp179->tag= Cyc_Arg_Set_spec_tag; _temp179->f1= Cyc_pp;( void*) _temp179;});
_temp176.f3=( struct _tagged_string)({ char* _temp177=( char*)"pretty print";
struct _tagged_string _temp178; _temp178.curr= _temp177; _temp178.base= _temp177;
_temp178.last_plus_one= _temp177 + 13; _temp178;}); _temp176;}),({ struct
_tuple10 _temp182; _temp182.f1=( struct _tagged_string)({ char* _temp186=( char*)"-o";
struct _tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 3; _temp187;}); _temp182.f2=({ struct Cyc_Arg_String_spec_struct*
_temp185=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp185->tag= Cyc_Arg_String_spec_tag; _temp185->f1= Cyc_set_out_file_name;(
void*) _temp185;}); _temp182.f3=( struct _tagged_string)({ char* _temp183=( char*)"Output file name";
struct _tagged_string _temp184; _temp184.curr= _temp183; _temp184.base= _temp183;
_temp184.last_plus_one= _temp183 + 17; _temp184;}); _temp182;})}; struct Cyc_List_List
l1=({ struct Cyc_List_List _temp188; _temp188.hd=( void*)({ struct _tuple10*
_temp189=( struct _tuple10*) specs; unsigned int _temp190= 0; if( _temp190 >= 15u){
_throw( Null_Exception);}& _temp189[ _temp190];}); _temp188.tl= 0; _temp188;});
struct Cyc_List_List l2=({ struct Cyc_List_List _temp191; _temp191.hd=( void*)({
struct _tuple10* _temp192=( struct _tuple10*) specs; unsigned int _temp193= 1;
if( _temp193 >= 15u){ _throw( Null_Exception);}& _temp192[ _temp193];});
_temp191.tl=( struct Cyc_List_List*)& l1; _temp191;}); struct Cyc_List_List l3=({
struct Cyc_List_List _temp194; _temp194.hd=( void*)({ struct _tuple10* _temp195=(
struct _tuple10*) specs; unsigned int _temp196= 2; if( _temp196 >= 15u){ _throw(
Null_Exception);}& _temp195[ _temp196];}); _temp194.tl=( struct Cyc_List_List*)&
l2; _temp194;}); struct Cyc_List_List l4=({ struct Cyc_List_List _temp197;
_temp197.hd=( void*)({ struct _tuple10* _temp198=( struct _tuple10*) specs;
unsigned int _temp199= 3; if( _temp199 >= 15u){ _throw( Null_Exception);}&
_temp198[ _temp199];}); _temp197.tl=( struct Cyc_List_List*)& l3; _temp197;});
struct Cyc_List_List l5=({ struct Cyc_List_List _temp200; _temp200.hd=( void*)({
struct _tuple10* _temp201=( struct _tuple10*) specs; unsigned int _temp202= 4;
if( _temp202 >= 15u){ _throw( Null_Exception);}& _temp201[ _temp202];});
_temp200.tl=( struct Cyc_List_List*)& l4; _temp200;}); struct Cyc_List_List l6=({
struct Cyc_List_List _temp203; _temp203.hd=( void*)({ struct _tuple10* _temp204=(
struct _tuple10*) specs; unsigned int _temp205= 5; if( _temp205 >= 15u){ _throw(
Null_Exception);}& _temp204[ _temp205];}); _temp203.tl=( struct Cyc_List_List*)&
l5; _temp203;}); struct Cyc_List_List l7=({ struct Cyc_List_List _temp206;
_temp206.hd=( void*)({ struct _tuple10* _temp207=( struct _tuple10*) specs;
unsigned int _temp208= 6; if( _temp208 >= 15u){ _throw( Null_Exception);}&
_temp207[ _temp208];}); _temp206.tl=( struct Cyc_List_List*)& l6; _temp206;});
struct Cyc_List_List l8=({ struct Cyc_List_List _temp209; _temp209.hd=( void*)({
struct _tuple10* _temp210=( struct _tuple10*) specs; unsigned int _temp211= 7;
if( _temp211 >= 15u){ _throw( Null_Exception);}& _temp210[ _temp211];});
_temp209.tl=( struct Cyc_List_List*)& l7; _temp209;}); struct Cyc_List_List l9=({
struct Cyc_List_List _temp212; _temp212.hd=( void*)({ struct _tuple10* _temp213=(
struct _tuple10*) specs; unsigned int _temp214= 8; if( _temp214 >= 15u){ _throw(
Null_Exception);}& _temp213[ _temp214];}); _temp212.tl=( struct Cyc_List_List*)&
l8; _temp212;}); struct Cyc_List_List l10=({ struct Cyc_List_List _temp215;
_temp215.hd=( void*)({ struct _tuple10* _temp216=( struct _tuple10*) specs;
unsigned int _temp217= 9; if( _temp217 >= 15u){ _throw( Null_Exception);}&
_temp216[ _temp217];}); _temp215.tl=( struct Cyc_List_List*)& l9; _temp215;});
struct Cyc_List_List l11=({ struct Cyc_List_List _temp218; _temp218.hd=( void*)({
struct _tuple10* _temp219=( struct _tuple10*) specs; unsigned int _temp220= 10;
if( _temp220 >= 15u){ _throw( Null_Exception);}& _temp219[ _temp220];});
_temp218.tl=( struct Cyc_List_List*)& l10; _temp218;}); struct Cyc_List_List l12=({
struct Cyc_List_List _temp221; _temp221.hd=( void*)({ struct _tuple10* _temp222=(
struct _tuple10*) specs; unsigned int _temp223= 11; if( _temp223 >= 15u){ _throw(
Null_Exception);}& _temp222[ _temp223];}); _temp221.tl=( struct Cyc_List_List*)&
l11; _temp221;}); struct Cyc_List_List l13=({ struct Cyc_List_List _temp224;
_temp224.hd=( void*)({ struct _tuple10* _temp225=( struct _tuple10*) specs;
unsigned int _temp226= 12; if( _temp226 >= 15u){ _throw( Null_Exception);}&
_temp225[ _temp226];}); _temp224.tl=( struct Cyc_List_List*)& l12; _temp224;});
struct Cyc_List_List l14=({ struct Cyc_List_List _temp227; _temp227.hd=( void*)({
struct _tuple10* _temp228=( struct _tuple10*) specs; unsigned int _temp229= 13;
if( _temp229 >= 15u){ _throw( Null_Exception);}& _temp228[ _temp229];});
_temp227.tl=( struct Cyc_List_List*)& l13; _temp227;}); struct Cyc_List_List l15=({
struct Cyc_List_List _temp230; _temp230.hd=( void*)({ struct _tuple10* _temp231=(
struct _tuple10*) specs; unsigned int _temp232= 14; if( _temp232 >= 15u){ _throw(
Null_Exception);}& _temp231[ _temp232];}); _temp230.tl=( struct Cyc_List_List*)&
l14; _temp230;}); struct Cyc_List_List* options=& l15; Cyc_Arg_parse(( struct
Cyc_List_List*) options, Cyc_compile_file,( struct _tagged_string)({ char*
_temp233=( char*)"Options:"; struct _tagged_string _temp234; _temp234.curr=
_temp233; _temp234.base= _temp233; _temp234.last_plus_one= _temp233 + 9;
_temp234;})); return Cyc_compile_failure? 1: 0;}