#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple6{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple7{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{ void* f1; void* f2; } ;
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List*
x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc*
Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); extern struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql(
struct _tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern const unsigned int Cyc_Position_Lex; extern const unsigned int Cyc_Position_Parse;
extern const unsigned int Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef
struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_XTunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_XTunionFieldInfo; typedef void* Cyc_Absyn_Type; typedef void*
Cyc_Absyn_Funcparams; typedef void* Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst;
typedef void* Cyc_Absyn_Primop; typedef void* Cyc_Absyn_Incrementor; typedef
void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp; typedef void* Cyc_Absyn_Raw_stmt;
struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat; struct Cyc_Absyn_Pat;
typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl;
struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_XTuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator;
typedef struct _xtunion_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_Mallocarg; typedef void*
Cyc_Absyn_scope_t; typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual_t; typedef
void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar*
Cyc_Absyn_tvar_t; typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref*
Cyc_Absyn_conref_t; typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_XTunionInfo Cyc_Absyn_xtunion_info_t; typedef struct
Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef struct Cyc_Absyn_XTunionFieldInfo
Cyc_Absyn_xtunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void*
Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t; typedef void*
Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_XTuniondecl*
Cyc_Absyn_xtuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl_t;
typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl_t;
typedef void* Cyc_Absyn_designator_t; typedef struct _xtunion_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; typedef void* Cyc_Absyn_mallocarg_t;
extern const unsigned int Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag;
struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_Abs_n_tag; struct Cyc_Absyn_Abs_n_struct{ int tag; struct
Cyc_List_List* f1; } ; extern const unsigned int Cyc_Absyn_Static; extern const
unsigned int Cyc_Absyn_Abstract; extern const unsigned int Cyc_Absyn_Public;
extern const unsigned int Cyc_Absyn_Extern; extern const unsigned int Cyc_Absyn_ExternC;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; extern const unsigned int Cyc_Absyn_B1; extern const unsigned int Cyc_Absyn_B2;
extern const unsigned int Cyc_Absyn_B4; extern const unsigned int Cyc_Absyn_B8;
extern const unsigned int Cyc_Absyn_AnyKind; extern const unsigned int Cyc_Absyn_MemKind;
extern const unsigned int Cyc_Absyn_BoxKind; extern const unsigned int Cyc_Absyn_RgnKind;
extern const unsigned int Cyc_Absyn_EffKind; extern const unsigned int Cyc_Absyn_Signed;
extern const unsigned int Cyc_Absyn_Unsigned; struct Cyc_Absyn_Conref{ void* v;
} ; extern const int Cyc_Absyn_Eq_constr_tag; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_Forward_constr_tag; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; extern const unsigned int Cyc_Absyn_No_constr;
struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; extern const unsigned int Cyc_Absyn_Unknown_b; extern const int Cyc_Absyn_Upper_b_tag;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple0* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple0* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple0* name; struct Cyc_List_List* targs;
struct _tuple0* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple0* name; struct
_tuple0* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
xtufd; } ; extern const unsigned int Cyc_Absyn_VoidType; extern const int Cyc_Absyn_Evar_tag;
struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int f3;
} ; extern const int Cyc_Absyn_VarType_tag; struct Cyc_Absyn_VarType_struct{ int
tag; struct Cyc_Absyn_Tvar* f1; } ; extern const int Cyc_Absyn_TunionType_tag;
struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1; } ;
extern const int Cyc_Absyn_XTunionType_tag; struct Cyc_Absyn_XTunionType_struct{
int tag; struct Cyc_Absyn_XTunionInfo f1; } ; extern const int Cyc_Absyn_TunionFieldType_tag;
struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; extern const int Cyc_Absyn_XTunionFieldType_tag; struct Cyc_Absyn_XTunionFieldType_struct{
int tag; struct Cyc_Absyn_XTunionFieldInfo f1; } ; extern const int Cyc_Absyn_PointerType_tag;
struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; } ;
extern const int Cyc_Absyn_IntType_tag; struct Cyc_Absyn_IntType_struct{ int tag;
void* f1; void* f2; } ; extern const unsigned int Cyc_Absyn_FloatType; extern
const unsigned int Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern const int Cyc_Absyn_UnionType_tag; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; extern const int Cyc_Absyn_EnumType_tag; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; extern const int
Cyc_Absyn_RgnHandleType_tag; struct Cyc_Absyn_RgnHandleType_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; extern const unsigned int Cyc_Absyn_HeapRgn; extern const int Cyc_Absyn_AccessEff_tag;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_JoinEff_tag;
struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_NoTypes_tag; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; extern const int
Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef void* Cyc_Absyn_Pointer_Sort;
extern const int Cyc_Absyn_NonNullable_ps_tag; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Nullable_ps_tag;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const unsigned int Cyc_Absyn_TaggedArray_ps; extern const int Cyc_Absyn_Regparm_att_tag;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; extern const
unsigned int Cyc_Absyn_Stdcall_att; extern const unsigned int Cyc_Absyn_Cdecl_att;
extern const unsigned int Cyc_Absyn_Noreturn_att; extern const unsigned int Cyc_Absyn_Const_att;
extern const int Cyc_Absyn_Aligned_att_tag; struct Cyc_Absyn_Aligned_att_struct{
int tag; int f1; } ; extern const unsigned int Cyc_Absyn_Packed_att; extern
const int Cyc_Absyn_Section_att_tag; struct Cyc_Absyn_Section_att_struct{ int
tag; struct _tagged_string f1; } ; extern const unsigned int Cyc_Absyn_Nocommon_att;
extern const unsigned int Cyc_Absyn_Shared_att; extern const unsigned int Cyc_Absyn_Unused_att;
extern const unsigned int Cyc_Absyn_Weak_att; extern const unsigned int Cyc_Absyn_Dllimport_att;
extern const unsigned int Cyc_Absyn_Dllexport_att; extern const unsigned int Cyc_Absyn_No_instrument_function_att;
extern const unsigned int Cyc_Absyn_Constructor_att; extern const unsigned int
Cyc_Absyn_Destructor_att; extern const unsigned int Cyc_Absyn_No_check_memory_usage_att;
extern const unsigned int Cyc_Absyn_Carray_mod; extern const int Cyc_Absyn_ConstArray_mod_tag;
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
Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ; extern const
unsigned int Cyc_Absyn_Null_c; extern const unsigned int Cyc_Absyn_Plus; extern
const unsigned int Cyc_Absyn_Times; extern const unsigned int Cyc_Absyn_Minus;
extern const unsigned int Cyc_Absyn_Div; extern const unsigned int Cyc_Absyn_Mod;
extern const unsigned int Cyc_Absyn_Eq; extern const unsigned int Cyc_Absyn_Neq;
extern const unsigned int Cyc_Absyn_Gt; extern const unsigned int Cyc_Absyn_Lt;
extern const unsigned int Cyc_Absyn_Gte; extern const unsigned int Cyc_Absyn_Lte;
extern const unsigned int Cyc_Absyn_Not; extern const unsigned int Cyc_Absyn_Bitnot;
extern const unsigned int Cyc_Absyn_Bitand; extern const unsigned int Cyc_Absyn_Bitor;
extern const unsigned int Cyc_Absyn_Bitxor; extern const unsigned int Cyc_Absyn_Bitlshift;
extern const unsigned int Cyc_Absyn_Bitlrshift; extern const unsigned int Cyc_Absyn_Bitarshift;
extern const unsigned int Cyc_Absyn_Size; extern const unsigned int Cyc_Absyn_Printf;
extern const unsigned int Cyc_Absyn_Fprintf; extern const unsigned int Cyc_Absyn_Xprintf;
extern const unsigned int Cyc_Absyn_Scanf; extern const unsigned int Cyc_Absyn_Fscanf;
extern const unsigned int Cyc_Absyn_Sscanf; extern const unsigned int Cyc_Absyn_PreInc;
extern const unsigned int Cyc_Absyn_PostInc; extern const unsigned int Cyc_Absyn_PreDec;
extern const unsigned int Cyc_Absyn_PostDec; extern const int Cyc_Absyn_Typ_m_tag;
struct Cyc_Absyn_Typ_m_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_Unresolved_m_tag;
struct Cyc_Absyn_Unresolved_m_struct{ int tag; struct _tuple0* f1; } ; extern
const int Cyc_Absyn_Tunion_m_tag; struct Cyc_Absyn_Tunion_m_struct{ int tag;
struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ; extern
const int Cyc_Absyn_XTunion_m_tag; struct Cyc_Absyn_XTunion_m_struct{ int tag;
struct Cyc_Absyn_XTuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ; extern
const int Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag; void*
f1; } ; extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{ int
tag; struct _tuple0* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
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
int Cyc_Absyn_Tunion_e_tag; struct Cyc_Absyn_Tunion_e_struct{ int tag; struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl*
f4; struct Cyc_Absyn_Tunionfield* f5; } ; extern const int Cyc_Absyn_XTunion_e_tag;
struct Cyc_Absyn_XTunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_XTuniondecl* f3; struct Cyc_Absyn_Tunionfield* f4; } ;
extern const int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; extern const int Cyc_Absyn_Malloc_e_tag; struct Cyc_Absyn_Malloc_e_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag; struct
Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_StmtExp_e_tag; struct Cyc_Absyn_StmtExp_e_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Codegen_e_tag;
struct Cyc_Absyn_Codegen_e_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
extern const int Cyc_Absyn_Fill_e_tag; struct Cyc_Absyn_Fill_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt;
void* r; struct Cyc_Position_Segment* loc; } ; extern const unsigned int Cyc_Absyn_Skip_s;
extern const int Cyc_Absyn_Exp_s_tag; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Seq_s_tag; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_Return_s_tag; struct Cyc_Absyn_Return_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_IfThenElse_s_tag; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
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
int try_depth; struct _xtunion_struct* annot; } ; extern const unsigned int Cyc_Absyn_Wild_p;
extern const int Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern const unsigned int Cyc_Absyn_Null_p;
extern const int Cyc_Absyn_Int_p_tag; struct Cyc_Absyn_Int_p_struct{ int tag;
void* f1; int f2; } ; extern const int Cyc_Absyn_Char_p_tag; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; extern const int Cyc_Absyn_Float_p_tag; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; extern const int Cyc_Absyn_Tuple_p_tag;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; extern const int Cyc_Absyn_Reference_p_tag; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern
const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; extern const int Cyc_Absyn_Tunion_p_tag;
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; extern const int Cyc_Absyn_XTunion_p_tag;
struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; extern const int Cyc_Absyn_Enum_p_tag; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
extern const unsigned int Cyc_Absyn_Unresolved_b; extern const int Cyc_Absyn_Global_b_tag;
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Tuniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
tvs; void* defn; } ; extern const int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern const int Cyc_Absyn_Fn_d_tag;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; extern
const int Cyc_Absyn_Let_d_tag; struct Cyc_Absyn_Let_d_struct{ int tag; struct
Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp*
f4; int f5; } ; extern const int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Absyn_Union_d_tag;
struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
extern const int Cyc_Absyn_Tunion_d_tag; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; extern const int Cyc_Absyn_XTunion_d_tag;
struct Cyc_Absyn_XTunion_d_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1; }
; extern const int Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
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
; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim(
void* p); extern struct _tagged_string Cyc_Absyn_attribute2string( void*);
struct Cyc_Buffer_t; typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern struct
_tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag;
struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
extern const int Cyc_Tcenv_TunionRes_tag; struct Cyc_Tcenv_TunionRes_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
extern const int Cyc_Tcenv_XTunionRes_tag; struct Cyc_Tcenv_XTunionRes_struct{
int tag; struct Cyc_Absyn_XTuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
extern const int Cyc_Tcenv_EnumRes_tag; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef void*
Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces;
struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict*
tuniondecls; struct Cyc_Dict_Dict* xtuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; typedef void* Cyc_Tcenv_Jumpee;
extern const unsigned int Cyc_Tcenv_NotLoop_j; extern const unsigned int Cyc_Tcenv_CaseEnd_j;
extern const unsigned int Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d); extern struct _tagged_string
Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_kind2string(
void*); extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*);
extern struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl);
extern struct _tagged_string Cyc_Absynpp_prim2string( void* p); extern struct
_tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p); extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual* tq, void* t); extern struct Cyc_PP_Doc* Cyc_Absynpp_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat12(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat11( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat10( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat9( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat8( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat7(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat6( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat5( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat4(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat2( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct
_tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc( struct
Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual*
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids= 0; int Cyc_Absynpp_decls_first=
0; int Cyc_Absynpp_dont_rewrite_temp_tvars= 0; int Cyc_Absynpp_print_all_tvars=
0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_print_using_stmts= 0; int
Cyc_Absynpp_print_externC_stmts= 0; static struct Cyc_List_List* Cyc_Absynpp_curr_namespace=
0; static void Cyc_Absynpp_curr_namespace_add( struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= Cyc_Absynpp_curr_namespace;
_temp0;});} static void Cyc_Absynpp_curr_namespace_drop(){ Cyc_Absynpp_curr_namespace=
Cyc_Absynpp_curr_namespace->tl;} struct _tagged_string Cyc_Absynpp_char_escape(
char c){ switch( c){ case '\a': _LL1: return( struct _tagged_string)({ char*
_temp3=( char*)"\\a"; struct _tagged_string _temp4; _temp4.curr= _temp3; _temp4.base=
_temp3; _temp4.last_plus_one= _temp3 + 3; _temp4;}); case '\b': _LL2: return(
struct _tagged_string)({ char* _temp6=( char*)"\\b"; struct _tagged_string
_temp7; _temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one= _temp6 +
3; _temp7;}); case '\f': _LL5: return( struct _tagged_string)({ char* _temp9=(
char*)"\\f"; struct _tagged_string _temp10; _temp10.curr= _temp9; _temp10.base=
_temp9; _temp10.last_plus_one= _temp9 + 3; _temp10;}); case '\n': _LL8: return(
struct _tagged_string)({ char* _temp12=( char*)"\\n"; struct _tagged_string
_temp13; _temp13.curr= _temp12; _temp13.base= _temp12; _temp13.last_plus_one=
_temp12 + 3; _temp13;}); case '\r': _LL11: return( struct _tagged_string)({ char*
_temp15=( char*)"\\r"; struct _tagged_string _temp16; _temp16.curr= _temp15;
_temp16.base= _temp15; _temp16.last_plus_one= _temp15 + 3; _temp16;}); case '\t':
_LL14: return( struct _tagged_string)({ char* _temp18=( char*)"\\t"; struct
_tagged_string _temp19; _temp19.curr= _temp18; _temp19.base= _temp18; _temp19.last_plus_one=
_temp18 + 3; _temp19;}); case '\v': _LL17: return( struct _tagged_string)({ char*
_temp21=( char*)"\\v"; struct _tagged_string _temp22; _temp22.curr= _temp21;
_temp22.base= _temp21; _temp22.last_plus_one= _temp21 + 3; _temp22;}); case '\\':
_LL20: return( struct _tagged_string)({ char* _temp24=( char*)"\\\\"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}); case '"': _LL23: return( struct _tagged_string)({ char*
_temp27=( char*)"\""; struct _tagged_string _temp28; _temp28.curr= _temp27;
_temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 2; _temp28;}); case '\'':
_LL26: return( struct _tagged_string)({ char* _temp30=( char*)"\\'"; struct
_tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 3; _temp31;}); default: _LL29: if(( int) c >=( int)' '?( int) c <=(
int)'~': 0){ struct _tagged_string t= Cyc_Core_new_string( 1);({ struct
_tagged_string _temp33= t; char* _temp35= _temp33.curr + 0; if( _temp35 <
_temp33.base? 1: _temp35 >= _temp33.last_plus_one){ _throw( Null_Exception);}*
_temp35= c;}); return t;} else{ struct _tagged_string t= Cyc_Core_new_string( 4);
int j= 0;({ struct _tagged_string _temp36= t; char* _temp38= _temp36.curr +( j
++); if( _temp38 < _temp36.base? 1: _temp38 >= _temp36.last_plus_one){ _throw(
Null_Exception);}* _temp38='\\';});({ struct _tagged_string _temp39= t; char*
_temp41= _temp39.curr +( j ++); if( _temp41 < _temp39.base? 1: _temp41 >=
_temp39.last_plus_one){ _throw( Null_Exception);}* _temp41=( char)(( int)'0' +((
int) c >> 6 & 7));});({ struct _tagged_string _temp42= t; char* _temp44= _temp42.curr
+( j ++); if( _temp44 < _temp42.base? 1: _temp44 >= _temp42.last_plus_one){
_throw( Null_Exception);}* _temp44=( char)(( int)'0' +(( int) c >> 3 & 7));});({
struct _tagged_string _temp45= t; char* _temp47= _temp45.curr +( j ++); if(
_temp47 < _temp45.base? 1: _temp47 >= _temp45.last_plus_one){ _throw(
Null_Exception);}* _temp47=( char)(( int)'0' +(( int) c & 7));}); return t;}}}
static int Cyc_Absynpp_special( struct _tagged_string s){ int sz=( int)(({
struct _tagged_string _temp48= s;( unsigned int)( _temp48.last_plus_one -
_temp48.curr);}) -( unsigned int) 1);{ int i= 0; for( 0; i < sz; i ++){ char c=({
struct _tagged_string _temp49= s; char* _temp51= _temp49.curr + i; if( _temp51 <
_temp49.base? 1: _temp51 >= _temp49.last_plus_one){ _throw( Null_Exception);}*
_temp51;}); if(((( int) c <=( int)' '? 1:( int) c >=( int)'~')? 1:( int) c ==(
int)'"')? 1:( int) c ==( int)'\\'){ return 1;}}} return 0;} struct
_tagged_string Cyc_Absynpp_string_escape( struct _tagged_string s){ if( ! Cyc_Absynpp_special(
s)){ return s;}{ int n=( int)(({ struct _tagged_string _temp52= s;( unsigned int)(
_temp52.last_plus_one - _temp52.curr);}) -( unsigned int) 1); if( n > 0?( int)({
struct _tagged_string _temp53= s; char* _temp55= _temp53.curr + n; if( _temp55 <
_temp53.base? 1: _temp55 >= _temp53.last_plus_one){ _throw( Null_Exception);}*
_temp55;}) ==( int)'\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i
++){ char _temp59=({ struct _tagged_string _temp56= s; char* _temp58= _temp56.curr
+ i; if( _temp58 < _temp56.base? 1: _temp58 >= _temp56.last_plus_one){ _throw(
Null_Exception);}* _temp58;}); _LL61: if( _temp59 =='\a'){ goto _LL62;} else{
goto _LL63;} _LL63: if( _temp59 =='\b'){ goto _LL64;} else{ goto _LL65;} _LL65:
if( _temp59 =='\f'){ goto _LL66;} else{ goto _LL67;} _LL67: if( _temp59 =='\n'){
goto _LL68;} else{ goto _LL69;} _LL69: if( _temp59 =='\r'){ goto _LL70;} else{
goto _LL71;} _LL71: if( _temp59 =='\t'){ goto _LL72;} else{ goto _LL73;} _LL73:
if( _temp59 =='\v'){ goto _LL74;} else{ goto _LL75;} _LL75: if( _temp59 =='\\'){
goto _LL76;} else{ goto _LL77;} _LL77: if( _temp59 =='"'){ goto _LL78;} else{
goto _LL79;} _LL79: goto _LL80; _LL62: goto _LL64; _LL64: goto _LL66; _LL66:
goto _LL68; _LL68: goto _LL70; _LL70: goto _LL72; _LL72: goto _LL74; _LL74: goto
_LL76; _LL76: goto _LL78; _LL78: len += 2; goto _LL60; _LL80: if(( int) _temp59
>=( int)' '?( int) _temp59 <=( int)'~': 0){ len ++;} else{ len += 4;} goto _LL60;
_LL60:;}}{ struct _tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i=
0; for( 0; i <= n; i ++){ char _temp84=({ struct _tagged_string _temp81= s; char*
_temp83= _temp81.curr + i; if( _temp83 < _temp81.base? 1: _temp83 >= _temp81.last_plus_one){
_throw( Null_Exception);}* _temp83;}); _LL86: if( _temp84 =='\a'){ goto _LL87;}
else{ goto _LL88;} _LL88: if( _temp84 =='\b'){ goto _LL89;} else{ goto _LL90;}
_LL90: if( _temp84 =='\f'){ goto _LL91;} else{ goto _LL92;} _LL92: if( _temp84
=='\n'){ goto _LL93;} else{ goto _LL94;} _LL94: if( _temp84 =='\r'){ goto _LL95;}
else{ goto _LL96;} _LL96: if( _temp84 =='\t'){ goto _LL97;} else{ goto _LL98;}
_LL98: if( _temp84 =='\v'){ goto _LL99;} else{ goto _LL100;} _LL100: if( _temp84
=='\\'){ goto _LL101;} else{ goto _LL102;} _LL102: if( _temp84 =='"'){ goto
_LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL87:({ struct _tagged_string
_temp106= t; char* _temp108= _temp106.curr +( j ++); if( _temp108 < _temp106.base?
1: _temp108 >= _temp106.last_plus_one){ _throw( Null_Exception);}* _temp108='\\';});({
struct _tagged_string _temp109= t; char* _temp111= _temp109.curr +( j ++); if(
_temp111 < _temp109.base? 1: _temp111 >= _temp109.last_plus_one){ _throw(
Null_Exception);}* _temp111='a';}); goto _LL85; _LL89:({ struct _tagged_string
_temp112= t; char* _temp114= _temp112.curr +( j ++); if( _temp114 < _temp112.base?
1: _temp114 >= _temp112.last_plus_one){ _throw( Null_Exception);}* _temp114='\\';});({
struct _tagged_string _temp115= t; char* _temp117= _temp115.curr +( j ++); if(
_temp117 < _temp115.base? 1: _temp117 >= _temp115.last_plus_one){ _throw(
Null_Exception);}* _temp117='b';}); goto _LL85; _LL91:({ struct _tagged_string
_temp118= t; char* _temp120= _temp118.curr +( j ++); if( _temp120 < _temp118.base?
1: _temp120 >= _temp118.last_plus_one){ _throw( Null_Exception);}* _temp120='\\';});({
struct _tagged_string _temp121= t; char* _temp123= _temp121.curr +( j ++); if(
_temp123 < _temp121.base? 1: _temp123 >= _temp121.last_plus_one){ _throw(
Null_Exception);}* _temp123='f';}); goto _LL85; _LL93:({ struct _tagged_string
_temp124= t; char* _temp126= _temp124.curr +( j ++); if( _temp126 < _temp124.base?
1: _temp126 >= _temp124.last_plus_one){ _throw( Null_Exception);}* _temp126='\\';});({
struct _tagged_string _temp127= t; char* _temp129= _temp127.curr +( j ++); if(
_temp129 < _temp127.base? 1: _temp129 >= _temp127.last_plus_one){ _throw(
Null_Exception);}* _temp129='n';}); goto _LL85; _LL95:({ struct _tagged_string
_temp130= t; char* _temp132= _temp130.curr +( j ++); if( _temp132 < _temp130.base?
1: _temp132 >= _temp130.last_plus_one){ _throw( Null_Exception);}* _temp132='\\';});({
struct _tagged_string _temp133= t; char* _temp135= _temp133.curr +( j ++); if(
_temp135 < _temp133.base? 1: _temp135 >= _temp133.last_plus_one){ _throw(
Null_Exception);}* _temp135='r';}); goto _LL85; _LL97:({ struct _tagged_string
_temp136= t; char* _temp138= _temp136.curr +( j ++); if( _temp138 < _temp136.base?
1: _temp138 >= _temp136.last_plus_one){ _throw( Null_Exception);}* _temp138='\\';});({
struct _tagged_string _temp139= t; char* _temp141= _temp139.curr +( j ++); if(
_temp141 < _temp139.base? 1: _temp141 >= _temp139.last_plus_one){ _throw(
Null_Exception);}* _temp141='t';}); goto _LL85; _LL99:({ struct _tagged_string
_temp142= t; char* _temp144= _temp142.curr +( j ++); if( _temp144 < _temp142.base?
1: _temp144 >= _temp142.last_plus_one){ _throw( Null_Exception);}* _temp144='\\';});({
struct _tagged_string _temp145= t; char* _temp147= _temp145.curr +( j ++); if(
_temp147 < _temp145.base? 1: _temp147 >= _temp145.last_plus_one){ _throw(
Null_Exception);}* _temp147='v';}); goto _LL85; _LL101:({ struct _tagged_string
_temp148= t; char* _temp150= _temp148.curr +( j ++); if( _temp150 < _temp148.base?
1: _temp150 >= _temp148.last_plus_one){ _throw( Null_Exception);}* _temp150='\\';});({
struct _tagged_string _temp151= t; char* _temp153= _temp151.curr +( j ++); if(
_temp153 < _temp151.base? 1: _temp153 >= _temp151.last_plus_one){ _throw(
Null_Exception);}* _temp153='\\';}); goto _LL85; _LL103:({ struct _tagged_string
_temp154= t; char* _temp156= _temp154.curr +( j ++); if( _temp156 < _temp154.base?
1: _temp156 >= _temp154.last_plus_one){ _throw( Null_Exception);}* _temp156='\\';});({
struct _tagged_string _temp157= t; char* _temp159= _temp157.curr +( j ++); if(
_temp159 < _temp157.base? 1: _temp159 >= _temp157.last_plus_one){ _throw(
Null_Exception);}* _temp159='"';}); goto _LL85; _LL105: if(( int) _temp84 >=(
int)' '?( int) _temp84 <=( int)'~': 0){({ struct _tagged_string _temp160= t;
char* _temp162= _temp160.curr +( j ++); if( _temp162 < _temp160.base? 1:
_temp162 >= _temp160.last_plus_one){ _throw( Null_Exception);}* _temp162=
_temp84;});} else{({ struct _tagged_string _temp163= t; char* _temp165= _temp163.curr
+( j ++); if( _temp165 < _temp163.base? 1: _temp165 >= _temp163.last_plus_one){
_throw( Null_Exception);}* _temp165='\\';});({ struct _tagged_string _temp166= t;
char* _temp168= _temp166.curr +( j ++); if( _temp168 < _temp166.base? 1:
_temp168 >= _temp166.last_plus_one){ _throw( Null_Exception);}* _temp168=( char)((
int)'0' +(( int) _temp84 >> 6 & 7));});({ struct _tagged_string _temp169= t;
char* _temp171= _temp169.curr +( j ++); if( _temp171 < _temp169.base? 1:
_temp171 >= _temp169.last_plus_one){ _throw( Null_Exception);}* _temp171=( char)((
int)'0' +(( int) _temp84 >> 3 & 7));});({ struct _tagged_string _temp172= t;
char* _temp174= _temp172.curr +( j ++); if( _temp174 < _temp172.base? 1:
_temp174 >= _temp172.last_plus_one){ _throw( Null_Exception);}* _temp174=( char)((
int)'0' +(( int) _temp84 & 7));});} goto _LL85; _LL85:;}} return t;}}}} static
struct Cyc_PP_Doc* Cyc_Absynpp_textptr( struct _tagged_string* s){ return Cyc_PP_text(*
s);} static char _temp177[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string=(
struct _tagged_string){ _temp177, _temp177, _temp177 + 9u}; static char _temp180[
9u]="volatile"; static struct _tagged_string Cyc_Absynpp_volatile_string=(
struct _tagged_string){ _temp180, _temp180, _temp180 + 9u}; static char _temp183[
6u]="const"; static struct _tagged_string Cyc_Absynpp_const_string=( struct
_tagged_string){ _temp183, _temp183, _temp183 + 6u}; static struct
_tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string; static
struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_string;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual* tq){ struct
Cyc_List_List* l= 0; if( tq->q_restrict){ l=({ struct Cyc_List_List* _temp184=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp184->hd=(
void*) Cyc_Absynpp_restrict_sp; _temp184->tl= l; _temp184;});} if( tq->q_volatile){
l=({ struct Cyc_List_List* _temp185=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp185->hd=( void*) Cyc_Absynpp_volatile_sp; _temp185->tl=
l; _temp185;});} if( tq->q_const){ l=({ struct Cyc_List_List* _temp186=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp186->hd=( void*)
Cyc_Absynpp_const_sp; _temp186->tl= l; _temp186;});} return Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp187=( char*)""; struct _tagged_string
_temp188; _temp188.curr= _temp187; _temp188.base= _temp187; _temp188.last_plus_one=
_temp187 + 1; _temp188;}),( struct _tagged_string)({ char* _temp189=( char*)" ";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 2; _temp190;}),( struct _tagged_string)({
char* _temp191=( char*)" "; struct _tagged_string _temp192; _temp192.curr=
_temp191; _temp192.base= _temp191; _temp192.last_plus_one= _temp191 + 2;
_temp192;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){ void* _temp193= k;
_LL195: if(( int) _temp193 == Cyc_Absyn_AnyKind){ goto _LL196;} else{ goto
_LL197;} _LL197: if(( int) _temp193 == Cyc_Absyn_MemKind){ goto _LL198;} else{
goto _LL199;} _LL199: if(( int) _temp193 == Cyc_Absyn_BoxKind){ goto _LL200;}
else{ goto _LL201;} _LL201: if(( int) _temp193 == Cyc_Absyn_RgnKind){ goto
_LL202;} else{ goto _LL203;} _LL203: if(( int) _temp193 == Cyc_Absyn_EffKind){
goto _LL204;} else{ goto _LL194;} _LL196: return( struct _tagged_string)({ char*
_temp205=( char*)"A"; struct _tagged_string _temp206; _temp206.curr= _temp205;
_temp206.base= _temp205; _temp206.last_plus_one= _temp205 + 2; _temp206;});
_LL198: return( struct _tagged_string)({ char* _temp207=( char*)"M"; struct
_tagged_string _temp208; _temp208.curr= _temp207; _temp208.base= _temp207;
_temp208.last_plus_one= _temp207 + 2; _temp208;}); _LL200: return( struct
_tagged_string)({ char* _temp209=( char*)"B"; struct _tagged_string _temp210;
_temp210.curr= _temp209; _temp210.base= _temp209; _temp210.last_plus_one=
_temp209 + 2; _temp210;}); _LL202: return( struct _tagged_string)({ char*
_temp211=( char*)"R"; struct _tagged_string _temp212; _temp212.curr= _temp211;
_temp212.base= _temp211; _temp212.last_plus_one= _temp211 + 2; _temp212;});
_LL204: return( struct _tagged_string)({ char* _temp213=( char*)"E"; struct
_tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 2; _temp214;}); _LL194:;} struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void*
_temp215=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( c))->v; void* _temp221; _LL217: if(( unsigned int)
_temp215 > 1u?(( struct _tunion_struct*) _temp215)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL222: _temp221=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp215)->f1;
goto _LL218;} else{ goto _LL219;} _LL219: goto _LL220; _LL218: return Cyc_Absynpp_kind2string(
_temp221); _LL220: return( struct _tagged_string)({ char* _temp223=( char*)"?";
struct _tagged_string _temp224; _temp224.curr= _temp223; _temp224.base= _temp223;
_temp224.last_plus_one= _temp223 + 2; _temp224;}); _LL216:;} struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string( k));}
struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){ return
Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_group(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ return Cyc_Absynpp_cat3( Cyc_PP_text( start),
Cyc_PP_seq( sep, ss), Cyc_PP_text( stop));} struct Cyc_PP_Doc* Cyc_Absynpp_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ if( ss == 0){ return Cyc_PP_nil_doc();} else{
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seq( sep, ss), Cyc_PP_text(
stop));}} struct Cyc_PP_Doc* Cyc_Absynpp_groupl( struct _tagged_string start,
struct _tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seql( sep, ss), Cyc_PP_text(
stop));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){
return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp225=( char*)"<";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 2; _temp226;}),( struct _tagged_string)({
char* _temp227=( char*)">"; struct _tagged_string _temp228; _temp228.curr=
_temp227; _temp228.base= _temp227; _temp228.last_plus_one= _temp227 + 2;
_temp228;}),( struct _tagged_string)({ char* _temp229=( char*)","; struct
_tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 2; _temp230;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp231=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp239; void* _temp241;
_LL233: if(( unsigned int) _temp231 > 1u?(( struct _tunion_struct*) _temp231)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL240: _temp239=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp231)->f1; if(( int) _temp239 == Cyc_Absyn_BoxKind){ goto _LL234;} else{
goto _LL235;}} else{ goto _LL235;} _LL235: if(( unsigned int) _temp231 > 1u?((
struct _tunion_struct*) _temp231)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL242:
_temp241=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp231)->f1; goto
_LL236;} else{ goto _LL237;} _LL237: goto _LL238; _LL234: return Cyc_Absynpp_textptr(
tv->name); _LL236: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp243=( char*)"::"; struct _tagged_string
_temp244; _temp244.curr= _temp243; _temp244.base= _temp243; _temp244.last_plus_one=
_temp243 + 3; _temp244;})), Cyc_Absynpp_kind2doc( _temp241)); _LL238: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp245=( char*)"/*::?*/"; struct _tagged_string _temp246; _temp246.curr=
_temp245; _temp246.base= _temp245; _temp246.last_plus_one= _temp245 + 8;
_temp246;}))); _LL232:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List*
tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp247=( char*)"<";
struct _tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 2; _temp248;}),( struct _tagged_string)({
char* _temp249=( char*)">"; struct _tagged_string _temp250; _temp250.curr=
_temp249; _temp250.base= _temp249; _temp250.last_plus_one= _temp249 + 2;
_temp250;}),( struct _tagged_string)({ char* _temp251=( char*)","; struct
_tagged_string _temp252; _temp252.curr= _temp251; _temp252.base= _temp251;
_temp252.last_plus_one= _temp251 + 2; _temp252;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp253=(
char*)"<"; struct _tagged_string _temp254; _temp254.curr= _temp253; _temp254.base=
_temp253; _temp254.last_plus_one= _temp253 + 2; _temp254;}),( struct
_tagged_string)({ char* _temp255=( char*)">"; struct _tagged_string _temp256;
_temp256.curr= _temp255; _temp256.base= _temp255; _temp256.last_plus_one=
_temp255 + 2; _temp256;}),( struct _tagged_string)({ char* _temp257=( char*)",";
struct _tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 2; _temp258;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp259=( char*)"(";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 2; _temp260;}),( struct _tagged_string)({
char* _temp261=( char*)")"; struct _tagged_string _temp262; _temp262.curr=
_temp261; _temp262.base= _temp261; _temp262.last_plus_one= _temp261 + 2;
_temp262;}),( struct _tagged_string)({ char* _temp263=( char*)","; struct
_tagged_string _temp264; _temp264.curr= _temp263; _temp264.base= _temp263;
_temp264.last_plus_one= _temp263 + 2; _temp264;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){
return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc(
struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();} return
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp265=( char*)" __attribute__";
struct _tagged_string _temp266; _temp266.curr= _temp265; _temp266.base= _temp265;
_temp266.last_plus_one= _temp265 + 15; _temp266;})), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp267=( char*)"(("; struct _tagged_string _temp268;
_temp268.curr= _temp267; _temp268.base= _temp267; _temp268.last_plus_one=
_temp267 + 3; _temp268;}),( struct _tagged_string)({ char* _temp269=( char*)"))";
struct _tagged_string _temp270; _temp270.curr= _temp269; _temp270.base= _temp269;
_temp270.last_plus_one= _temp269 + 3; _temp270;}),( struct _tagged_string)({
char* _temp271=( char*)","; struct _tagged_string _temp272; _temp272.curr=
_temp271; _temp272.base= _temp271; _temp272.last_plus_one= _temp271 + 2;
_temp272;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts)));} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp273=( void*)
tms->hd; struct Cyc_Absyn_Tqual* _temp279; void* _temp281; void* _temp283;
_LL275: if(( unsigned int) _temp273 > 1u?(( struct _tunion_struct*) _temp273)->tag
== Cyc_Absyn_Pointer_mod_tag: 0){ _LL284: _temp283=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp273)->f1; goto _LL282; _LL282: _temp281=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp273)->f2; goto _LL280; _LL280: _temp279=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp273)->f3; goto _LL276;} else{ goto _LL277;}
_LL277: goto _LL278; _LL276: return 1; _LL278: return 0; _LL274:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d, tms->tl); struct Cyc_PP_Doc*
p_rest= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp285=(
char*)"("; struct _tagged_string _temp286; _temp286.curr= _temp285; _temp286.base=
_temp285; _temp286.last_plus_one= _temp285 + 2; _temp286;})), rest, Cyc_PP_text((
struct _tagged_string)({ char* _temp287=( char*)")"; struct _tagged_string
_temp288; _temp288.curr= _temp287; _temp288.base= _temp287; _temp288.last_plus_one=
_temp287 + 2; _temp288;}))); void* _temp289=( void*) tms->hd; struct Cyc_Absyn_Exp*
_temp303; void* _temp305; struct Cyc_List_List* _temp307; struct Cyc_Position_Segment*
_temp309; int _temp311; struct Cyc_Position_Segment* _temp313; struct Cyc_List_List*
_temp315; struct Cyc_Absyn_Tqual* _temp317; void* _temp319; void* _temp321;
_LL291: if(( int) _temp289 == Cyc_Absyn_Carray_mod){ goto _LL292;} else{ goto
_LL293;} _LL293: if(( unsigned int) _temp289 > 1u?(( struct _tunion_struct*)
_temp289)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL304: _temp303=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp289)->f1; goto
_LL294;} else{ goto _LL295;} _LL295: if(( unsigned int) _temp289 > 1u?(( struct
_tunion_struct*) _temp289)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL306:
_temp305=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp289)->f1; goto
_LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp289 > 1u?(( struct
_tunion_struct*) _temp289)->tag == Cyc_Absyn_Attributes_mod_tag: 0){ _LL310:
_temp309=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp289)->f1; goto _LL308; _LL308: _temp307=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp289)->f2; goto _LL298;} else{ goto _LL299;}
_LL299: if(( unsigned int) _temp289 > 1u?(( struct _tunion_struct*) _temp289)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL316: _temp315=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp289)->f1; goto _LL314; _LL314:
_temp313=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp289)->f2; goto _LL312; _LL312: _temp311=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp289)->f3; goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int)
_temp289 > 1u?(( struct _tunion_struct*) _temp289)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL322: _temp321=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp289)->f1;
goto _LL320; _LL320: _temp319=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp289)->f2; goto _LL318; _LL318: _temp317=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp289)->f3; goto _LL302;} else{ goto _LL290;}
_LL292: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2(
rest, Cyc_PP_text(( struct _tagged_string)({ char* _temp323=( char*)"[]"; struct
_tagged_string _temp324; _temp324.curr= _temp323; _temp324.base= _temp323;
_temp324.last_plus_one= _temp323 + 3; _temp324;}))); _LL294: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;} return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp325=( char*)"["; struct _tagged_string _temp326;
_temp326.curr= _temp325; _temp326.base= _temp325; _temp326.last_plus_one=
_temp325 + 2; _temp326;})), Cyc_Absynpp_exp2doc( _temp303), Cyc_PP_text(( struct
_tagged_string)({ char* _temp327=( char*)"]"; struct _tagged_string _temp328;
_temp328.curr= _temp327; _temp328.base= _temp327; _temp328.last_plus_one=
_temp327 + 2; _temp328;}))); _LL296: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;}{ void* _temp329= _temp305; struct Cyc_Core_Opt* _temp335; int
_temp337; struct Cyc_List_List* _temp339; struct Cyc_Position_Segment* _temp341;
struct Cyc_List_List* _temp343; _LL331: if((( struct _tunion_struct*) _temp329)->tag
== Cyc_Absyn_WithTypes_tag){ _LL340: _temp339=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp329)->f1; goto _LL338; _LL338: _temp337=( int)((
struct Cyc_Absyn_WithTypes_struct*) _temp329)->f2; goto _LL336; _LL336: _temp335=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp329)->f3; goto
_LL332;} else{ goto _LL333;} _LL333: if((( struct _tunion_struct*) _temp329)->tag
== Cyc_Absyn_NoTypes_tag){ _LL344: _temp343=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp329)->f1; goto _LL342; _LL342: _temp341=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp329)->f2; goto
_LL334;} else{ goto _LL330;} _LL332: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp339, _temp337, _temp335)); _LL334: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp345=( char*)"("; struct _tagged_string
_temp346; _temp346.curr= _temp345; _temp346.base= _temp345; _temp346.last_plus_one=
_temp345 + 2; _temp346;}),( struct _tagged_string)({ char* _temp347=( char*)")";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 2; _temp348;}),( struct _tagged_string)({
char* _temp349=( char*)","; struct _tagged_string _temp350; _temp350.curr=
_temp349; _temp350.base= _temp349; _temp350.last_plus_one= _temp349 + 2;
_temp350;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp343))); _LL330:;} _LL298: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest=
p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_atts2doc( _temp307)); _LL300:
if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest= p_rest;} if( _temp311){ return
Cyc_Absynpp_cat2( rest, Cyc_Absynpp_ktvars2doc( _temp315));} else{ return Cyc_Absynpp_cat2(
rest, Cyc_Absynpp_tvars2doc( _temp315));} _LL302: { struct Cyc_PP_Doc* ptr;{
void* _temp351= _temp321; struct Cyc_Absyn_Exp* _temp359; struct Cyc_Absyn_Exp*
_temp361; _LL353: if(( unsigned int) _temp351 > 1u?(( struct _tunion_struct*)
_temp351)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL360: _temp359=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Nullable_ps_struct*) _temp351)->f1; goto _LL354;} else{ goto
_LL355;} _LL355: if(( unsigned int) _temp351 > 1u?(( struct _tunion_struct*)
_temp351)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL362: _temp361=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp351)->f1; goto
_LL356;} else{ goto _LL357;} _LL357: if(( int) _temp351 == Cyc_Absyn_TaggedArray_ps){
goto _LL358;} else{ goto _LL352;} _LL354: if( Cyc_Evexp_eval_const_uint_exp(
_temp359) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp363=( char*)"*"; struct _tagged_string _temp364; _temp364.curr= _temp363;
_temp364.base= _temp363; _temp364.last_plus_one= _temp363 + 2; _temp364;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp365=( char*)"*"; struct _tagged_string _temp366; _temp366.curr= _temp365;
_temp366.base= _temp365; _temp366.last_plus_one= _temp365 + 2; _temp366;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp367=( char*)"{"; struct _tagged_string
_temp368; _temp368.curr= _temp367; _temp368.base= _temp367; _temp368.last_plus_one=
_temp367 + 2; _temp368;})), Cyc_Absynpp_exp2doc( _temp359), Cyc_PP_text(( struct
_tagged_string)({ char* _temp369=( char*)"}"; struct _tagged_string _temp370;
_temp370.curr= _temp369; _temp370.base= _temp369; _temp370.last_plus_one=
_temp369 + 2; _temp370;})));} goto _LL352; _LL356: if( Cyc_Evexp_eval_const_uint_exp(
_temp361) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp371=( char*)"@"; struct _tagged_string _temp372; _temp372.curr= _temp371;
_temp372.base= _temp371; _temp372.last_plus_one= _temp371 + 2; _temp372;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp373=( char*)"@"; struct _tagged_string _temp374; _temp374.curr= _temp373;
_temp374.base= _temp373; _temp374.last_plus_one= _temp373 + 2; _temp374;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp375=( char*)"{"; struct _tagged_string
_temp376; _temp376.curr= _temp375; _temp376.base= _temp375; _temp376.last_plus_one=
_temp375 + 2; _temp376;})), Cyc_Absynpp_exp2doc( _temp361), Cyc_PP_text(( struct
_tagged_string)({ char* _temp377=( char*)"}"; struct _tagged_string _temp378;
_temp378.curr= _temp377; _temp378.base= _temp377; _temp378.last_plus_one=
_temp377 + 2; _temp378;})));} goto _LL352; _LL358: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp379=( char*)"?"; struct _tagged_string _temp380;
_temp380.curr= _temp379; _temp380.base= _temp379; _temp380.last_plus_one=
_temp379 + 2; _temp380;})); goto _LL352; _LL352:;}{ void* _temp381= Cyc_Tcutil_compress(
_temp319); _LL383: if(( int) _temp381 == Cyc_Absyn_HeapRgn){ goto _LL384;} else{
goto _LL385;} _LL385: goto _LL386; _LL384: return Cyc_Absynpp_cat2( ptr, rest);
_LL386: return Cyc_Absynpp_cat4( ptr, Cyc_Absynpp_ntyp2doc( _temp319), Cyc_PP_text((
struct _tagged_string)({ char* _temp387=( char*)" "; struct _tagged_string
_temp388; _temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 2; _temp388;})), rest); _LL382:;}} _LL290:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc(
void* t){ void* _temp389= Cyc_Tcutil_compress( t); _LL391: if(( int) _temp389 ==
Cyc_Absyn_HeapRgn){ goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394;
_LL392: return Cyc_PP_text(( struct _tagged_string)({ char* _temp395=( char*)"`H";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 3; _temp396;})); _LL394: return Cyc_Absynpp_ntyp2doc(
t); _LL390:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp397= Cyc_Tcutil_compress(
t); void* _temp405; struct Cyc_List_List* _temp407; _LL399: if(( unsigned int)
_temp397 > 4u?(( struct _tunion_struct*) _temp397)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL406: _temp405=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp397)->f1;
goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp397 > 4u?((
struct _tunion_struct*) _temp397)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL408:
_temp407=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp397)->f1;
goto _LL402;} else{ goto _LL403;} _LL403: goto _LL404; _LL400:* regions=({
struct Cyc_List_List* _temp409=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp409->hd=( void*) Cyc_Absynpp_rgn2doc( _temp405);
_temp409->tl=* regions; _temp409;}); goto _LL398; _LL402: for( 0; _temp407 != 0;
_temp407= _temp407->tl){ Cyc_Absynpp_effects2docs( regions, effects,( void*)
_temp407->hd);} goto _LL398; _LL404:* effects=({ struct Cyc_List_List* _temp410=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp410->hd=(
void*) Cyc_Absynpp_ntyp2doc( t); _temp410->tl=* effects; _temp410;}); goto
_LL398; _LL398:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(& regions,&
effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); if( regions == 0? effects != 0: 0){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp411=( char*)""; struct _tagged_string _temp412;
_temp412.curr= _temp411; _temp412.base= _temp411; _temp412.last_plus_one=
_temp411 + 1; _temp412;}),( struct _tagged_string)({ char* _temp413=( char*)"";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 1; _temp414;}),( struct _tagged_string)({
char* _temp415=( char*)"+"; struct _tagged_string _temp416; _temp416.curr=
_temp415; _temp416.base= _temp415; _temp416.last_plus_one= _temp415 + 2;
_temp416;}), effects);} else{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp417=( char*)"{"; struct _tagged_string
_temp418; _temp418.curr= _temp417; _temp418.base= _temp417; _temp418.last_plus_one=
_temp417 + 2; _temp418;}),( struct _tagged_string)({ char* _temp419=( char*)"}";
struct _tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 2; _temp420;}),( struct _tagged_string)({
char* _temp421=( char*)","; struct _tagged_string _temp422; _temp422.curr=
_temp421; _temp422.base= _temp421; _temp422.last_plus_one= _temp421 + 2;
_temp422;}), regions); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp423=( char*)""; struct _tagged_string _temp424; _temp424.curr= _temp423;
_temp424.base= _temp423; _temp424.last_plus_one= _temp423 + 1; _temp424;}),(
struct _tagged_string)({ char* _temp425=( char*)""; struct _tagged_string
_temp426; _temp426.curr= _temp425; _temp426.base= _temp425; _temp426.last_plus_one=
_temp425 + 1; _temp426;}),( struct _tagged_string)({ char* _temp427=( char*)"+";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 2; _temp428;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp429=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp429->hd=( void*) doc1; _temp429->tl= 0; _temp429;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void*
_temp430= t; struct Cyc_Absyn_Exp* _temp476; struct Cyc_Absyn_Tqual* _temp478;
void* _temp480; struct Cyc_Absyn_FnInfo _temp482; struct Cyc_Absyn_PtrInfo
_temp484; int _temp486; struct Cyc_Core_Opt* _temp488; void* _temp490; struct
Cyc_Absyn_Tvar* _temp492; struct Cyc_Absyn_TunionInfo _temp494; struct Cyc_Absyn_Tuniondecl*
_temp496; void* _temp498; struct Cyc_List_List* _temp500; struct _tuple0*
_temp502; struct Cyc_Absyn_XTunionInfo _temp504; struct Cyc_Absyn_XTuniondecl*
_temp506; void* _temp508; struct _tuple0* _temp510; struct Cyc_Absyn_TunionFieldInfo
_temp512; struct Cyc_Absyn_Tunionfield* _temp514; struct Cyc_Absyn_Tuniondecl*
_temp516; struct _tuple0* _temp518; struct Cyc_List_List* _temp520; struct
_tuple0* _temp522; struct Cyc_Absyn_XTunionFieldInfo _temp524; struct Cyc_Absyn_Tunionfield*
_temp526; struct Cyc_Absyn_XTuniondecl* _temp528; struct _tuple0* _temp530;
struct _tuple0* _temp532; void* _temp534; void* _temp536; struct Cyc_List_List*
_temp538; struct Cyc_Absyn_Structdecl** _temp540; struct Cyc_List_List* _temp542;
struct _tuple0* _temp544; struct Cyc_Absyn_Uniondecl** _temp546; struct Cyc_List_List*
_temp548; struct _tuple0* _temp550; struct Cyc_Absyn_Enumdecl* _temp552; struct
_tuple0* _temp554; struct Cyc_Core_Opt* _temp556; struct Cyc_List_List* _temp558;
struct _tuple0* _temp560; void* _temp562; void* _temp564; struct Cyc_List_List*
_temp566; _LL432: if(( unsigned int) _temp430 > 4u?(( struct _tunion_struct*)
_temp430)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL481: _temp480=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp430)->f1; goto _LL479; _LL479: _temp478=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp430)->f2;
goto _LL477; _LL477: _temp476=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp430)->f3; goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_FnType_tag:
0){ _LL483: _temp482=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp430)->f1; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL485: _temp484=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp430)->f1; goto _LL437;} else{ goto _LL438;} _LL438: if(( int) _temp430 ==
Cyc_Absyn_VoidType){ goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_Evar_tag: 0){
_LL491: _temp490=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp430)->f1; goto
_LL489; _LL489: _temp488=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp430)->f2; goto _LL487; _LL487: _temp486=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp430)->f3; goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_VarType_tag:
0){ _LL493: _temp492=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp430)->f1; goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL495: _temp494=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp430)->f1; _LL503: _temp502=( struct _tuple0*) _temp494.name; goto _LL501;
_LL501: _temp500=( struct Cyc_List_List*) _temp494.targs; goto _LL499; _LL499:
_temp498=( void*) _temp494.rgn; goto _LL497; _LL497: _temp496=( struct Cyc_Absyn_Tuniondecl*)
_temp494.tud; goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL505: _temp504=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp430)->f1; _LL511: _temp510=( struct _tuple0*) _temp504.name; goto _LL509;
_LL509: _temp508=( void*) _temp504.rgn; goto _LL507; _LL507: _temp506=( struct
Cyc_Absyn_XTuniondecl*) _temp504.xtud; goto _LL447;} else{ goto _LL448;} _LL448:
if(( unsigned int) _temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL513: _temp512=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp430)->f1; _LL523: _temp522=(
struct _tuple0*) _temp512.name; goto _LL521; _LL521: _temp520=( struct Cyc_List_List*)
_temp512.targs; goto _LL519; _LL519: _temp518=( struct _tuple0*) _temp512.fname;
goto _LL517; _LL517: _temp516=( struct Cyc_Absyn_Tuniondecl*) _temp512.tud; goto
_LL515; _LL515: _temp514=( struct Cyc_Absyn_Tunionfield*) _temp512.tufd; goto
_LL449;} else{ goto _LL450;} _LL450: if(( unsigned int) _temp430 > 4u?(( struct
_tunion_struct*) _temp430)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL525:
_temp524=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp430)->f1; _LL533: _temp532=( struct _tuple0*) _temp524.name; goto _LL531;
_LL531: _temp530=( struct _tuple0*) _temp524.fname; goto _LL529; _LL529:
_temp528=( struct Cyc_Absyn_XTuniondecl*) _temp524.xtud; goto _LL527; _LL527:
_temp526=( struct Cyc_Absyn_Tunionfield*) _temp524.xtufd; goto _LL451;} else{
goto _LL452;} _LL452: if(( unsigned int) _temp430 > 4u?(( struct _tunion_struct*)
_temp430)->tag == Cyc_Absyn_IntType_tag: 0){ _LL537: _temp536=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp430)->f1; goto _LL535; _LL535: _temp534=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp430)->f2; goto _LL453;} else{ goto _LL454;}
_LL454: if(( int) _temp430 == Cyc_Absyn_FloatType){ goto _LL455;} else{ goto
_LL456;} _LL456: if(( int) _temp430 == Cyc_Absyn_DoubleType){ goto _LL457;}
else{ goto _LL458;} _LL458: if(( unsigned int) _temp430 > 4u?(( struct
_tunion_struct*) _temp430)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL539: _temp538=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp430)->f1; goto
_LL459;} else{ goto _LL460;} _LL460: if(( unsigned int) _temp430 > 4u?(( struct
_tunion_struct*) _temp430)->tag == Cyc_Absyn_StructType_tag: 0){ _LL545:
_temp544=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp430)->f1;
goto _LL543; _LL543: _temp542=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp430)->f2; goto _LL541; _LL541: _temp540=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp430)->f3; goto _LL461;} else{ goto
_LL462;} _LL462: if(( unsigned int) _temp430 > 4u?(( struct _tunion_struct*)
_temp430)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL551: _temp550=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp430)->f1; goto _LL549; _LL549: _temp548=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp430)->f2; goto
_LL547; _LL547: _temp546=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp430)->f3; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL555: _temp554=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp430)->f1; goto _LL553; _LL553: _temp552=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp430)->f2; goto _LL465;} else{ goto
_LL466;} _LL466: if(( unsigned int) _temp430 > 4u?(( struct _tunion_struct*)
_temp430)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL561: _temp560=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp430)->f1; goto _LL559;
_LL559: _temp558=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp430)->f2; goto _LL557; _LL557: _temp556=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp430)->f3; goto _LL467;} else{ goto _LL468;}
_LL468: if(( unsigned int) _temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL563: _temp562=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp430)->f1; goto _LL469;} else{ goto _LL470;} _LL470: if(( int) _temp430 ==
Cyc_Absyn_HeapRgn){ goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp430 > 4u?(( struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL565: _temp564=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp430)->f1;
goto _LL473;} else{ goto _LL474;} _LL474: if(( unsigned int) _temp430 > 4u?((
struct _tunion_struct*) _temp430)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL567:
_temp566=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp430)->f1;
goto _LL475;} else{ goto _LL431;} _LL433: return Cyc_PP_text(( struct
_tagged_string)({ char* _temp568=( char*)"[[[array]]]"; struct _tagged_string
_temp569; _temp569.curr= _temp568; _temp569.base= _temp568; _temp569.last_plus_one=
_temp568 + 12; _temp569;})); _LL435: return Cyc_PP_nil_doc(); _LL437: return Cyc_PP_nil_doc();
_LL439: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp570=( char*)"void";
struct _tagged_string _temp571; _temp571.curr= _temp570; _temp571.base= _temp570;
_temp571.last_plus_one= _temp570 + 5; _temp571;})); goto _LL431; _LL441: if(
_temp488 != 0){ return Cyc_Absynpp_ntyp2doc(( void*) _temp488->v);} else{ s= Cyc_Absynpp_cat5(
Cyc_PP_text(( struct _tagged_string)({ char* _temp572=( char*)"%"; struct
_tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 2; _temp573;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp574=( char*)"("; struct _tagged_string _temp575;
_temp575.curr= _temp574; _temp575.base= _temp574; _temp575.last_plus_one=
_temp574 + 2; _temp575;})), Cyc_PP_text( xprintf("%d", _temp486)), Cyc_PP_text((
struct _tagged_string)({ char* _temp576=( char*)")::"; struct _tagged_string
_temp577; _temp577.curr= _temp576; _temp577.base= _temp576; _temp577.last_plus_one=
_temp576 + 4; _temp577;})), Cyc_Absynpp_kind2doc( _temp490));} goto _LL431;
_LL443: s= Cyc_Absynpp_textptr( _temp492->name); if( Cyc_Absynpp_print_all_kinds){
s= Cyc_Absynpp_cat3( s, Cyc_PP_text(( struct _tagged_string)({ char* _temp578=(
char*)"::"; struct _tagged_string _temp579; _temp579.curr= _temp578; _temp579.base=
_temp578; _temp579.last_plus_one= _temp578 + 3; _temp579;})), Cyc_Absynpp_ckind2doc(
_temp492->kind));} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp492): 0){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp580=( char*)"_ /* "; struct _tagged_string _temp581; _temp581.curr=
_temp580; _temp581.base= _temp580; _temp581.last_plus_one= _temp580 + 6;
_temp581;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp582=( char*)" */";
struct _tagged_string _temp583; _temp583.curr= _temp582; _temp583.base= _temp582;
_temp583.last_plus_one= _temp582 + 4; _temp583;})));} goto _LL431; _LL445:{ void*
_temp584= Cyc_Tcutil_compress( _temp498); _LL586: if(( int) _temp584 == Cyc_Absyn_HeapRgn){
goto _LL587;} else{ goto _LL588;} _LL588: goto _LL589; _LL587: s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp590=( char*)"tunion "; struct
_tagged_string _temp591; _temp591.curr= _temp590; _temp591.base= _temp590;
_temp591.last_plus_one= _temp590 + 8; _temp591;})), Cyc_Absynpp_qvar2doc(
_temp502), Cyc_Absynpp_tps2doc( _temp500)); goto _LL585; _LL589: s= Cyc_Absynpp_cat5(
Cyc_PP_text(( struct _tagged_string)({ char* _temp592=( char*)"tunion "; struct
_tagged_string _temp593; _temp593.curr= _temp592; _temp593.base= _temp592;
_temp593.last_plus_one= _temp592 + 8; _temp593;})), Cyc_Absynpp_typ2doc(
_temp498), Cyc_PP_text(( struct _tagged_string)({ char* _temp594=( char*)" ";
struct _tagged_string _temp595; _temp595.curr= _temp594; _temp595.base= _temp594;
_temp595.last_plus_one= _temp594 + 2; _temp595;})), Cyc_Absynpp_qvar2doc(
_temp502), Cyc_Absynpp_tps2doc( _temp500)); goto _LL585; _LL585:;} goto _LL431;
_LL447:{ void* _temp596= Cyc_Tcutil_compress( _temp508); _LL598: if(( int)
_temp596 == Cyc_Absyn_HeapRgn){ goto _LL599;} else{ goto _LL600;} _LL600: goto
_LL601; _LL599: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp602=( char*)"xtunion "; struct _tagged_string _temp603; _temp603.curr=
_temp602; _temp603.base= _temp602; _temp603.last_plus_one= _temp602 + 9;
_temp603;})), Cyc_Absynpp_qvar2doc( _temp510)); goto _LL597; _LL601: s= Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp604=( char*)"xtunion "; struct
_tagged_string _temp605; _temp605.curr= _temp604; _temp605.base= _temp604;
_temp605.last_plus_one= _temp604 + 9; _temp605;})), Cyc_Absynpp_typ2doc(
_temp508), Cyc_PP_text(( struct _tagged_string)({ char* _temp606=( char*)" ";
struct _tagged_string _temp607; _temp607.curr= _temp606; _temp607.base= _temp606;
_temp607.last_plus_one= _temp606 + 2; _temp607;})), Cyc_Absynpp_qvar2doc(
_temp510)); goto _LL597; _LL597:;} goto _LL431; _LL449: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp608=( char*)"tunion "; struct _tagged_string
_temp609; _temp609.curr= _temp608; _temp609.base= _temp608; _temp609.last_plus_one=
_temp608 + 8; _temp609;})), Cyc_Absynpp_qvar2doc( _temp522), Cyc_PP_text((
struct _tagged_string)({ char* _temp610=( char*)"."; struct _tagged_string
_temp611; _temp611.curr= _temp610; _temp611.base= _temp610; _temp611.last_plus_one=
_temp610 + 2; _temp611;})), Cyc_Absynpp_textptr((* _temp518).f2), Cyc_Absynpp_tps2doc(
_temp520)); goto _LL431; _LL451: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp612=( char*)"xtunion "; struct _tagged_string
_temp613; _temp613.curr= _temp612; _temp613.base= _temp612; _temp613.last_plus_one=
_temp612 + 9; _temp613;})), Cyc_Absynpp_qvar2doc( _temp532), Cyc_PP_text((
struct _tagged_string)({ char* _temp614=( char*)"."; struct _tagged_string
_temp615; _temp615.curr= _temp614; _temp615.base= _temp614; _temp615.last_plus_one=
_temp614 + 2; _temp615;})), Cyc_Absynpp_textptr((* _temp530).f2)); goto _LL431;
_LL453: { struct _tagged_string sns; struct _tagged_string ts;{ void* _temp616=
_temp536; _LL618: if(( int) _temp616 == Cyc_Absyn_Signed){ goto _LL619;} else{
goto _LL620;} _LL620: if(( int) _temp616 == Cyc_Absyn_Unsigned){ goto _LL621;}
else{ goto _LL617;} _LL619: sns=( struct _tagged_string)({ char* _temp622=( char*)"";
struct _tagged_string _temp623; _temp623.curr= _temp622; _temp623.base= _temp622;
_temp623.last_plus_one= _temp622 + 1; _temp623;}); goto _LL617; _LL621: sns=(
struct _tagged_string)({ char* _temp624=( char*)"unsigned "; struct
_tagged_string _temp625; _temp625.curr= _temp624; _temp625.base= _temp624;
_temp625.last_plus_one= _temp624 + 10; _temp625;}); goto _LL617; _LL617:;}{ void*
_temp626= _temp534; _LL628: if(( int) _temp626 == Cyc_Absyn_B1){ goto _LL629;}
else{ goto _LL630;} _LL630: if(( int) _temp626 == Cyc_Absyn_B2){ goto _LL631;}
else{ goto _LL632;} _LL632: if(( int) _temp626 == Cyc_Absyn_B4){ goto _LL633;}
else{ goto _LL634;} _LL634: if(( int) _temp626 == Cyc_Absyn_B8){ goto _LL635;}
else{ goto _LL627;} _LL629:{ void* _temp636= _temp536; _LL638: if(( int)
_temp636 == Cyc_Absyn_Signed){ goto _LL639;} else{ goto _LL640;} _LL640: if((
int) _temp636 == Cyc_Absyn_Unsigned){ goto _LL641;} else{ goto _LL637;} _LL639:
sns=( struct _tagged_string)({ char* _temp642=( char*)"signed "; struct
_tagged_string _temp643; _temp643.curr= _temp642; _temp643.base= _temp642;
_temp643.last_plus_one= _temp642 + 8; _temp643;}); goto _LL637; _LL641: sns=(
struct _tagged_string)({ char* _temp644=( char*)""; struct _tagged_string
_temp645; _temp645.curr= _temp644; _temp645.base= _temp644; _temp645.last_plus_one=
_temp644 + 1; _temp645;}); goto _LL637; _LL637:;} ts=( struct _tagged_string)({
char* _temp646=( char*)"char"; struct _tagged_string _temp647; _temp647.curr=
_temp646; _temp647.base= _temp646; _temp647.last_plus_one= _temp646 + 5;
_temp647;}); goto _LL627; _LL631: ts=( struct _tagged_string)({ char* _temp648=(
char*)"short"; struct _tagged_string _temp649; _temp649.curr= _temp648; _temp649.base=
_temp648; _temp649.last_plus_one= _temp648 + 6; _temp649;}); goto _LL627; _LL633:
ts=( struct _tagged_string)({ char* _temp650=( char*)"int"; struct
_tagged_string _temp651; _temp651.curr= _temp650; _temp651.base= _temp650;
_temp651.last_plus_one= _temp650 + 4; _temp651;}); goto _LL627; _LL635: ts=(
struct _tagged_string)({ char* _temp652=( char*)"long long"; struct
_tagged_string _temp653; _temp653.curr= _temp652; _temp653.base= _temp652;
_temp653.last_plus_one= _temp652 + 10; _temp653;}); goto _LL627; _LL627:;} s=
Cyc_PP_text(({ struct _tagged_string _temp654= sns; struct _tagged_string
_temp655= ts; xprintf("%.*s%.*s", _temp654.last_plus_one - _temp654.curr,
_temp654.curr, _temp655.last_plus_one - _temp655.curr, _temp655.curr);})); goto
_LL431;} _LL455: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp656=( char*)"float";
struct _tagged_string _temp657; _temp657.curr= _temp656; _temp657.base= _temp656;
_temp657.last_plus_one= _temp656 + 6; _temp657;})); goto _LL431; _LL457: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp658=( char*)"double"; struct _tagged_string
_temp659; _temp659.curr= _temp658; _temp659.base= _temp658; _temp659.last_plus_one=
_temp658 + 7; _temp659;})); goto _LL431; _LL459: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp660=( char*)"$"; struct _tagged_string
_temp661; _temp661.curr= _temp660; _temp661.base= _temp660; _temp661.last_plus_one=
_temp660 + 2; _temp661;})), Cyc_Absynpp_args2doc( _temp538)); goto _LL431;
_LL461: if( _temp544 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp662=( char*)"struct "; struct _tagged_string
_temp663; _temp663.curr= _temp662; _temp663.base= _temp662; _temp663.last_plus_one=
_temp662 + 8; _temp663;})), Cyc_Absynpp_tps2doc( _temp542));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp664=( char*)"struct "; struct
_tagged_string _temp665; _temp665.curr= _temp664; _temp665.base= _temp664;
_temp665.last_plus_one= _temp664 + 8; _temp665;})), _temp544 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp666= _temp544;
if( _temp666 == 0){ _throw( Null_Exception);} _temp666;})), Cyc_Absynpp_tps2doc(
_temp542));} goto _LL431; _LL463: if( _temp550 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp667=( char*)"union "; struct _tagged_string
_temp668; _temp668.curr= _temp667; _temp668.base= _temp667; _temp668.last_plus_one=
_temp667 + 7; _temp668;})), Cyc_Absynpp_tps2doc( _temp548));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp669=( char*)"union "; struct
_tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 7; _temp670;})), _temp550 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp671= _temp550;
if( _temp671 == 0){ _throw( Null_Exception);} _temp671;})), Cyc_Absynpp_tps2doc(
_temp548));} goto _LL431; _LL465: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp672=( char*)"enum "; struct _tagged_string _temp673;
_temp673.curr= _temp672; _temp673.base= _temp672; _temp673.last_plus_one=
_temp672 + 6; _temp673;})), Cyc_Absynpp_qvar2doc( _temp554)); goto _LL431;
_LL467: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp560), Cyc_Absynpp_tps2doc(
_temp558)); goto _LL431; _LL469: s= Cyc_Absynpp_rgn2doc( _temp562); goto _LL431;
_LL471: s= Cyc_Absynpp_rgn2doc( t); goto _LL431; _LL473: s= Cyc_Absynpp_eff2doc(
t); goto _LL431; _LL475: s= Cyc_Absynpp_eff2doc( t); goto _LL431; _LL431:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*) vo->v));}
struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp674=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp674->v=( void*) Cyc_PP_text(*((
struct _tagged_string*)((* t).f1)->v)); _temp674;}); return Cyc_Absynpp_tqtd2doc((*
t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc( struct Cyc_List_List*
args, int varargs, struct Cyc_Core_Opt* effopt){ if( effopt == 0){ if( varargs){
return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp675=( char*)"(";
struct _tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 2; _temp676;}),( struct _tagged_string)({
char* _temp677=( char*)")"; struct _tagged_string _temp678; _temp678.curr=
_temp677; _temp678.base= _temp677; _temp678.last_plus_one= _temp677 + 2;
_temp678;}),( struct _tagged_string)({ char* _temp679=( char*)","; struct
_tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 2; _temp680;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp681=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp681->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp682=(
char*)"..."; struct _tagged_string _temp683; _temp683.curr= _temp682; _temp683.base=
_temp682; _temp683.last_plus_one= _temp682 + 4; _temp683;})); _temp681->tl= 0;
_temp681;})));} else{ return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp684=( char*)"("; struct _tagged_string _temp685; _temp685.curr= _temp684;
_temp685.base= _temp684; _temp685.last_plus_one= _temp684 + 2; _temp685;}),(
struct _tagged_string)({ char* _temp686=( char*)")"; struct _tagged_string
_temp687; _temp687.curr= _temp686; _temp687.base= _temp686; _temp687.last_plus_one=
_temp686 + 2; _temp687;}),( struct _tagged_string)({ char* _temp688=( char*)",";
struct _tagged_string _temp689; _temp689.curr= _temp688; _temp689.base= _temp688;
_temp689.last_plus_one= _temp688 + 2; _temp689;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_funarg2doc, args));}} else{ if( varargs){ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp690=( char*)"("; struct _tagged_string
_temp691; _temp691.curr= _temp690; _temp691.base= _temp690; _temp691.last_plus_one=
_temp690 + 2; _temp691;}),( struct _tagged_string)({ char* _temp692=( char*)")";
struct _tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 2; _temp693;}),( struct _tagged_string)({
char* _temp694=( char*)";"; struct _tagged_string _temp695; _temp695.curr=
_temp694; _temp695.base= _temp694; _temp695.last_plus_one= _temp694 + 2;
_temp695;}),({ struct Cyc_List_List* _temp696=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp696->hd=( void*) Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp698=( char*)""; struct _tagged_string _temp699;
_temp699.curr= _temp698; _temp699.base= _temp698; _temp699.last_plus_one=
_temp698 + 1; _temp699;}),( struct _tagged_string)({ char* _temp700=( char*)"";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 1; _temp701;}),( struct _tagged_string)({
char* _temp702=( char*)","; struct _tagged_string _temp703; _temp703.curr=
_temp702; _temp703.base= _temp702; _temp703.last_plus_one= _temp702 + 2;
_temp703;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp704=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp704->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp705=( char*)"..."; struct _tagged_string _temp706;
_temp706.curr= _temp705; _temp706.base= _temp705; _temp706.last_plus_one=
_temp705 + 4; _temp706;})); _temp704->tl= 0; _temp704;}))); _temp696->tl=({
struct Cyc_List_List* _temp697=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp697->hd=( void*) Cyc_Absynpp_eff2doc(( void*)
effopt->v); _temp697->tl= 0; _temp697;}); _temp696;}));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp707=( char*)"("; struct _tagged_string
_temp708; _temp708.curr= _temp707; _temp708.base= _temp707; _temp708.last_plus_one=
_temp707 + 2; _temp708;}),( struct _tagged_string)({ char* _temp709=( char*)")";
struct _tagged_string _temp710; _temp710.curr= _temp709; _temp710.base= _temp709;
_temp710.last_plus_one= _temp709 + 2; _temp710;}),( struct _tagged_string)({
char* _temp711=( char*)";"; struct _tagged_string _temp712; _temp712.curr=
_temp711; _temp712.base= _temp711; _temp712.last_plus_one= _temp711 + 2;
_temp712;}),({ struct Cyc_List_List* _temp713=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp713->hd=( void*) Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp715=( char*)""; struct _tagged_string _temp716;
_temp716.curr= _temp715; _temp716.base= _temp715; _temp716.last_plus_one=
_temp715 + 1; _temp716;}),( struct _tagged_string)({ char* _temp717=( char*)"";
struct _tagged_string _temp718; _temp718.curr= _temp717; _temp718.base= _temp717;
_temp718.last_plus_one= _temp717 + 1; _temp718;}),( struct _tagged_string)({
char* _temp719=( char*)","; struct _tagged_string _temp720; _temp720.curr=
_temp719; _temp720.base= _temp719; _temp720.last_plus_one= _temp719 + 2;
_temp720;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));
_temp713->tl=({ struct Cyc_List_List* _temp714=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp714->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp714->tl= 0; _temp714;}); _temp713;}));}}} struct _tuple1*
Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({ struct _tuple1* _temp721=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp721->f1=({ struct Cyc_Core_Opt*
_temp722=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp722->v=( void*)(* arg).f1; _temp722;}); _temp721->f2=(* arg).f2; _temp721->f3=(*
arg).f3; _temp721;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* prefix= 0;{ void* _temp723=(* q).f1;
struct Cyc_List_List* _temp735; struct Cyc_List_List* _temp737; struct Cyc_List_List*
_temp739; struct Cyc_List_List* _temp741; _LL725: if(( int) _temp723 == Cyc_Absyn_Loc_n){
goto _LL726;} else{ goto _LL727;} _LL727: if(( unsigned int) _temp723 > 1u?((
struct _tunion_struct*) _temp723)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL736:
_temp735=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp723)->f1;
if( _temp735 == 0){ goto _LL728;} else{ goto _LL729;}} else{ goto _LL729;}
_LL729: if(( unsigned int) _temp723 > 1u?(( struct _tunion_struct*) _temp723)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL738: _temp737=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp723)->f1; if( _temp737 == 0){ goto _LL730;} else{
goto _LL731;}} else{ goto _LL731;} _LL731: if(( unsigned int) _temp723 > 1u?((
struct _tunion_struct*) _temp723)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL740:
_temp739=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp723)->f1;
goto _LL732;} else{ goto _LL733;} _LL733: if(( unsigned int) _temp723 > 1u?((
struct _tunion_struct*) _temp723)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL742:
_temp741=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp723)->f1;
goto _LL734;} else{ goto _LL724;} _LL726: goto _LL728; _LL728: goto _LL730;
_LL730: return Cyc_Absynpp_var2doc((* q).f2); _LL732: _temp741= _temp739; goto
_LL734; _LL734: prefix= _temp741; goto _LL724; _LL724:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp743=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp743->hd=( void*)(* q).f2; _temp743->tl= 0; _temp743;})),( struct
_tagged_string)({ char* _temp744=( char*)"_"; struct _tagged_string _temp745;
_temp745.curr= _temp744; _temp745.base= _temp744; _temp745.last_plus_one=
_temp744 + 2; _temp745;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, prefix, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp746=( char*)""; struct _tagged_string
_temp747; _temp747.curr= _temp746; _temp747.base= _temp746; _temp747.last_plus_one=
_temp746 + 1; _temp747;}),( struct _tagged_string)({ char* _temp748=( char*)"::";
struct _tagged_string _temp749; _temp749.curr= _temp748; _temp749.base= _temp748;
_temp749.last_plus_one= _temp748 + 3; _temp749;}),( struct _tagged_string)({
char* _temp750=( char*)"::"; struct _tagged_string _temp751; _temp751.curr=
_temp750; _temp751.base= _temp750; _temp751.last_plus_one= _temp750 + 3;
_temp751;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_var2doc,
prefix)), Cyc_Absynpp_var2doc((* q).f2));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp752=(* v).f1; struct Cyc_List_List* _temp758; _LL754: if((
unsigned int) _temp752 > 1u?(( struct _tunion_struct*) _temp752)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL759: _temp758=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp752)->f1; goto _LL755;} else{ goto _LL756;} _LL756: goto _LL757; _LL755:
if((( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp,
_temp758, Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);}
else{ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp760=( char*)"/* wrong namespace : */ "; struct _tagged_string _temp761;
_temp761.curr= _temp760; _temp761.base= _temp760; _temp761.last_plus_one=
_temp760 + 25; _temp761;})), Cyc_Absynpp_qvar2doc( v));} _LL757: return Cyc_Absynpp_qvar2doc(
v); _LL753:;}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp762=( void*) e->r; void* _temp834; void* _temp836; struct _tuple0*
_temp838; struct _tuple0* _temp840; struct Cyc_List_List* _temp842; void*
_temp844; struct Cyc_Absyn_Exp* _temp846; struct Cyc_Core_Opt* _temp848; struct
Cyc_Absyn_Exp* _temp850; void* _temp852; struct Cyc_Absyn_Exp* _temp854; struct
Cyc_Absyn_Exp* _temp856; struct Cyc_Absyn_Exp* _temp858; struct Cyc_Absyn_Exp*
_temp860; struct Cyc_Absyn_Exp* _temp862; struct Cyc_Absyn_Exp* _temp864; struct
Cyc_List_List* _temp866; struct Cyc_Absyn_Exp* _temp868; struct Cyc_List_List*
_temp870; struct Cyc_Absyn_Exp* _temp872; struct Cyc_Absyn_Exp* _temp874; struct
Cyc_Absyn_Exp* _temp876; struct Cyc_List_List* _temp878; struct Cyc_Absyn_Exp*
_temp880; struct Cyc_Absyn_Exp* _temp882; void* _temp884; struct Cyc_Absyn_Exp*
_temp886; struct Cyc_Absyn_Exp* _temp888; void* _temp890; struct Cyc_Absyn_Exp*
_temp892; struct Cyc_Absyn_Exp* _temp894; struct _tagged_string* _temp896;
struct Cyc_Absyn_Exp* _temp898; struct _tagged_string* _temp900; struct Cyc_Absyn_Exp*
_temp902; struct Cyc_Absyn_Exp* _temp904; struct Cyc_Absyn_Exp* _temp906; struct
Cyc_List_List* _temp908; struct Cyc_List_List* _temp910; struct _tuple1*
_temp912; struct Cyc_List_List* _temp914; struct Cyc_Absyn_Exp* _temp916; struct
Cyc_Absyn_Exp* _temp918; struct Cyc_Absyn_Vardecl* _temp920; struct Cyc_Absyn_Structdecl*
_temp922; struct Cyc_List_List* _temp924; struct Cyc_Core_Opt* _temp926; struct
_tuple0* _temp928; struct Cyc_Absyn_Tunionfield* _temp930; struct Cyc_Absyn_Tuniondecl*
_temp932; struct Cyc_List_List* _temp934; struct Cyc_Core_Opt* _temp936; struct
Cyc_Core_Opt* _temp938; struct Cyc_Absyn_Tunionfield* _temp940; struct Cyc_Absyn_XTuniondecl*
_temp942; struct Cyc_List_List* _temp944; struct Cyc_Core_Opt* _temp946; struct
Cyc_Absyn_Enumfield* _temp948; struct Cyc_Absyn_Enumdecl* _temp950; struct
_tuple0* _temp952; void* _temp954; struct Cyc_List_List* _temp956; struct Cyc_Core_Opt*
_temp958; struct Cyc_Absyn_Stmt* _temp960; struct Cyc_Absyn_Fndecl* _temp962;
struct Cyc_Absyn_Exp* _temp964; _LL764: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Const_e_tag){ _LL835: _temp834=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp762)->f1; goto _LL765;} else{ goto _LL766;} _LL766: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Var_e_tag){ _LL839: _temp838=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp762)->f1; goto _LL837;
_LL837: _temp836=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp762)->f2; goto
_LL767;} else{ goto _LL768;} _LL768: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL841: _temp840=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp762)->f1; goto _LL769;} else{ goto _LL770;} _LL770: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Primop_e_tag){ _LL845: _temp844=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp762)->f1; goto _LL843; _LL843:
_temp842=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp762)->f2;
goto _LL771;} else{ goto _LL772;} _LL772: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL851: _temp850=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp762)->f1; goto _LL849; _LL849: _temp848=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp762)->f2; goto
_LL847; _LL847: _temp846=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp762)->f3; goto _LL773;} else{ goto _LL774;} _LL774: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Increment_e_tag){ _LL855: _temp854=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp762)->f1;
goto _LL853; _LL853: _temp852=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp762)->f2; goto _LL775;} else{ goto _LL776;} _LL776: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Conditional_e_tag){ _LL861:
_temp860=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp762)->f1; goto _LL859; _LL859: _temp858=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp762)->f2; goto _LL857; _LL857: _temp856=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp762)->f3;
goto _LL777;} else{ goto _LL778;} _LL778: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL865: _temp864=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp762)->f1; goto _LL863; _LL863: _temp862=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp762)->f2; goto
_LL779;} else{ goto _LL780;} _LL780: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL869: _temp868=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp762)->f1; goto _LL867; _LL867:
_temp866=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp762)->f2; goto _LL781;} else{ goto _LL782;} _LL782: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_FnCall_e_tag){ _LL873: _temp872=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp762)->f1; goto
_LL871; _LL871: _temp870=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp762)->f2; goto _LL783;} else{ goto _LL784;} _LL784: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Throw_e_tag){ _LL875: _temp874=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp762)->f1; goto
_LL785;} else{ goto _LL786;} _LL786: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL877: _temp876=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp762)->f1; goto _LL787;} else{
goto _LL788;} _LL788: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL881: _temp880=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp762)->f1; goto _LL879; _LL879: _temp878=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp762)->f2; goto _LL789;} else{ goto _LL790;}
_LL790: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Cast_e_tag){
_LL885: _temp884=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp762)->f1; goto
_LL883; _LL883: _temp882=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp762)->f2; goto _LL791;} else{ goto _LL792;} _LL792: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_New_e_tag){ _LL887: _temp886=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp762)->f1; goto
_LL793;} else{ goto _LL794;} _LL794: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Address_e_tag){ _LL889: _temp888=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp762)->f1; goto _LL795;} else{ goto _LL796;}
_LL796: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL891: _temp890=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp762)->f1;
goto _LL797;} else{ goto _LL798;} _LL798: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL893: _temp892=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp762)->f1; goto _LL799;} else{ goto
_LL800;} _LL800: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Deref_e_tag){
_LL895: _temp894=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp762)->f1; goto _LL801;} else{ goto _LL802;} _LL802: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_StructMember_e_tag){ _LL899:
_temp898=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp762)->f1; goto _LL897; _LL897: _temp896=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp762)->f2; goto _LL803;} else{ goto _LL804;}
_LL804: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL903: _temp902=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp762)->f1; goto _LL901; _LL901: _temp900=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp762)->f2; goto _LL805;} else{ goto _LL806;}
_LL806: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Subscript_e_tag){
_LL907: _temp906=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp762)->f1; goto _LL905; _LL905: _temp904=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp762)->f2; goto _LL807;} else{ goto _LL808;}
_LL808: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Tuple_e_tag){
_LL909: _temp908=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp762)->f1; goto _LL809;} else{ goto _LL810;} _LL810: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL913:
_temp912=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp762)->f1;
goto _LL911; _LL911: _temp910=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp762)->f2; goto _LL811;} else{ goto _LL812;} _LL812: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Array_e_tag){ _LL915: _temp914=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp762)->f1; goto
_LL813;} else{ goto _LL814;} _LL814: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL921: _temp920=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp762)->f1; goto _LL919; _LL919:
_temp918=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp762)->f2; goto _LL917; _LL917: _temp916=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp762)->f3; goto _LL815;} else{ goto
_LL816;} _LL816: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Struct_e_tag){
_LL929: _temp928=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp762)->f1; goto _LL927; _LL927: _temp926=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp762)->f2; goto _LL925; _LL925: _temp924=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp762)->f3; goto
_LL923; _LL923: _temp922=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp762)->f4; goto _LL817;} else{ goto _LL818;} _LL818: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Tunion_e_tag){ _LL939: _temp938=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp762)->f1; goto
_LL937; _LL937: _temp936=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp762)->f2; goto _LL935; _LL935: _temp934=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp762)->f3; goto _LL933; _LL933: _temp932=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp762)->f4;
goto _LL931; _LL931: _temp930=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp762)->f5; goto _LL819;} else{ goto _LL820;} _LL820: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_XTunion_e_tag){ _LL947: _temp946=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp762)->f1; goto
_LL945; _LL945: _temp944=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp762)->f2; goto _LL943; _LL943: _temp942=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp762)->f3; goto _LL941; _LL941: _temp940=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp762)->f4;
goto _LL821;} else{ goto _LL822;} _LL822: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Enum_e_tag){ _LL953: _temp952=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp762)->f1; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp762)->f2; goto _LL949; _LL949: _temp948=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp762)->f3;
goto _LL823;} else{ goto _LL824;} _LL824: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL955: _temp954=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp762)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL959:
_temp958=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp762)->f1; goto _LL957; _LL957: _temp956=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp762)->f2; goto _LL827;} else{ goto
_LL828;} _LL828: if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL961: _temp960=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp762)->f1; goto _LL829;} else{ goto _LL830;} _LL830: if((( struct
_tunion_struct*) _temp762)->tag == Cyc_Absyn_Codegen_e_tag){ _LL963: _temp962=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp762)->f1;
goto _LL831;} else{ goto _LL832;} _LL832: if((( struct _tunion_struct*) _temp762)->tag
== Cyc_Absyn_Fill_e_tag){ _LL965: _temp964=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp762)->f1; goto _LL833;} else{ goto _LL763;} _LL765: return 10000; _LL767:
return 10000; _LL769: return 10000; _LL771: { void* _temp966= _temp844; _LL968:
if(( int) _temp966 == Cyc_Absyn_Plus){ goto _LL969;} else{ goto _LL970;} _LL970:
if(( int) _temp966 == Cyc_Absyn_Times){ goto _LL971;} else{ goto _LL972;} _LL972:
if(( int) _temp966 == Cyc_Absyn_Minus){ goto _LL973;} else{ goto _LL974;} _LL974:
if(( int) _temp966 == Cyc_Absyn_Div){ goto _LL975;} else{ goto _LL976;} _LL976:
if(( int) _temp966 == Cyc_Absyn_Mod){ goto _LL977;} else{ goto _LL978;} _LL978:
if(( int) _temp966 == Cyc_Absyn_Eq){ goto _LL979;} else{ goto _LL980;} _LL980:
if(( int) _temp966 == Cyc_Absyn_Neq){ goto _LL981;} else{ goto _LL982;} _LL982:
if(( int) _temp966 == Cyc_Absyn_Gt){ goto _LL983;} else{ goto _LL984;} _LL984:
if(( int) _temp966 == Cyc_Absyn_Lt){ goto _LL985;} else{ goto _LL986;} _LL986:
if(( int) _temp966 == Cyc_Absyn_Gte){ goto _LL987;} else{ goto _LL988;} _LL988:
if(( int) _temp966 == Cyc_Absyn_Lte){ goto _LL989;} else{ goto _LL990;} _LL990:
if(( int) _temp966 == Cyc_Absyn_Not){ goto _LL991;} else{ goto _LL992;} _LL992:
if(( int) _temp966 == Cyc_Absyn_Bitnot){ goto _LL993;} else{ goto _LL994;}
_LL994: if(( int) _temp966 == Cyc_Absyn_Bitand){ goto _LL995;} else{ goto _LL996;}
_LL996: if(( int) _temp966 == Cyc_Absyn_Bitor){ goto _LL997;} else{ goto _LL998;}
_LL998: if(( int) _temp966 == Cyc_Absyn_Bitxor){ goto _LL999;} else{ goto
_LL1000;} _LL1000: if(( int) _temp966 == Cyc_Absyn_Bitlshift){ goto _LL1001;}
else{ goto _LL1002;} _LL1002: if(( int) _temp966 == Cyc_Absyn_Bitlrshift){ goto
_LL1003;} else{ goto _LL1004;} _LL1004: if(( int) _temp966 == Cyc_Absyn_Bitarshift){
goto _LL1005;} else{ goto _LL1006;} _LL1006: if(( int) _temp966 == Cyc_Absyn_Size){
goto _LL1007;} else{ goto _LL1008;} _LL1008: if(( int) _temp966 == Cyc_Absyn_Printf){
goto _LL1009;} else{ goto _LL1010;} _LL1010: if(( int) _temp966 == Cyc_Absyn_Fprintf){
goto _LL1011;} else{ goto _LL1012;} _LL1012: if(( int) _temp966 == Cyc_Absyn_Xprintf){
goto _LL1013;} else{ goto _LL1014;} _LL1014: if(( int) _temp966 == Cyc_Absyn_Scanf){
goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( int) _temp966 == Cyc_Absyn_Fscanf){
goto _LL1017;} else{ goto _LL1018;} _LL1018: if(( int) _temp966 == Cyc_Absyn_Sscanf){
goto _LL1019;} else{ goto _LL967;} _LL969: return 100; _LL971: return 110;
_LL973: return 100; _LL975: return 110; _LL977: return 110; _LL979: return 70;
_LL981: return 70; _LL983: return 80; _LL985: return 80; _LL987: return 80;
_LL989: return 80; _LL991: return 130; _LL993: return 130; _LL995: return 60;
_LL997: return 40; _LL999: return 50; _LL1001: return 90; _LL1003: return 80;
_LL1005: return 80; _LL1007: return 140; _LL1009: return 130; _LL1011: return
130; _LL1013: return 130; _LL1015: return 130; _LL1017: return 130; _LL1019:
return 130; _LL967:;} _LL773: return 20; _LL775: return 20; _LL777: return 30;
_LL779: return 10; _LL781: return 140; _LL783: return 140; _LL785: return 130;
_LL787: return Cyc_Absynpp_exp_prec( _temp876); _LL789: return Cyc_Absynpp_exp_prec(
_temp880); _LL791: return 120; _LL793: return 15; _LL795: return 130; _LL797:
return 130; _LL799: return 130; _LL801: return 130; _LL803: return 140; _LL805:
return 140; _LL807: return 140; _LL809: return 150; _LL811: goto _LL813; _LL813:
goto _LL815; _LL815: goto _LL817; _LL817: goto _LL819; _LL819: goto _LL821;
_LL821: goto _LL823; _LL823: goto _LL825; _LL825: return 140; _LL827: return 140;
_LL829: return 10000; _LL831: return 140; _LL833: return 140; _LL763:;} struct
Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp1020=(
void*) e->r; void* _temp1096; void* _temp1098; struct Cyc_Absyn_Vardecl*
_temp1100; struct _tuple0* _temp1102; void* _temp1104; struct Cyc_Absyn_Vardecl*
_temp1106; struct _tuple0* _temp1108; void* _temp1110; struct _tuple0* _temp1112;
struct _tuple0* _temp1114; struct Cyc_List_List* _temp1116; void* _temp1118;
struct Cyc_Absyn_Exp* _temp1120; struct Cyc_Core_Opt* _temp1122; struct Cyc_Absyn_Exp*
_temp1124; void* _temp1126; struct Cyc_Absyn_Exp* _temp1128; struct Cyc_Absyn_Exp*
_temp1130; struct Cyc_Absyn_Exp* _temp1132; struct Cyc_Absyn_Exp* _temp1134;
struct Cyc_Absyn_Exp* _temp1136; struct Cyc_Absyn_Exp* _temp1138; struct Cyc_List_List*
_temp1140; struct Cyc_Absyn_Exp* _temp1142; struct Cyc_List_List* _temp1144;
struct Cyc_Absyn_Exp* _temp1146; struct Cyc_Absyn_Exp* _temp1148; struct Cyc_Absyn_Exp*
_temp1150; struct Cyc_List_List* _temp1152; struct Cyc_Absyn_Exp* _temp1154;
struct Cyc_Absyn_Exp* _temp1156; void* _temp1158; struct Cyc_Absyn_Exp*
_temp1160; struct Cyc_Absyn_Exp* _temp1162; void* _temp1164; struct Cyc_Absyn_Exp*
_temp1166; struct Cyc_Absyn_Exp* _temp1168; struct _tagged_string* _temp1170;
struct Cyc_Absyn_Exp* _temp1172; struct _tagged_string* _temp1174; struct Cyc_Absyn_Exp*
_temp1176; struct Cyc_Absyn_Exp* _temp1178; struct Cyc_Absyn_Exp* _temp1180;
struct Cyc_List_List* _temp1182; struct Cyc_List_List* _temp1184; struct _tuple1*
_temp1186; struct Cyc_List_List* _temp1188; struct Cyc_Absyn_Exp* _temp1190;
struct Cyc_Absyn_Exp* _temp1192; struct Cyc_Absyn_Vardecl* _temp1194; struct Cyc_Absyn_Structdecl*
_temp1196; struct Cyc_List_List* _temp1198; struct Cyc_Core_Opt* _temp1200;
struct _tuple0* _temp1202; struct Cyc_Absyn_Tunionfield* _temp1204; struct Cyc_Absyn_Tuniondecl*
_temp1206; struct Cyc_List_List* _temp1208; struct Cyc_Core_Opt* _temp1210;
struct Cyc_Core_Opt* _temp1212; struct Cyc_Absyn_Tunionfield* _temp1214; struct
Cyc_Absyn_XTuniondecl* _temp1216; struct Cyc_List_List* _temp1218; struct Cyc_Core_Opt*
_temp1220; struct Cyc_Absyn_Enumfield* _temp1222; struct Cyc_Absyn_Enumdecl*
_temp1224; struct _tuple0* _temp1226; void* _temp1228; struct Cyc_List_List*
_temp1230; struct Cyc_Core_Opt* _temp1232; struct Cyc_Absyn_Stmt* _temp1234;
struct Cyc_Absyn_Fndecl* _temp1236; struct Cyc_Absyn_Exp* _temp1238; _LL1022:
if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Const_e_tag){ _LL1097:
_temp1096=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1020)->f1; goto
_LL1023;} else{ goto _LL1024;} _LL1024: if((( struct _tunion_struct*) _temp1020)->tag
== Cyc_Absyn_Var_e_tag){ _LL1103: _temp1102=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1020)->f1; goto _LL1099; _LL1099: _temp1098=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1020)->f2; if(( unsigned int) _temp1098 > 1u?(( struct _tunion_struct*)
_temp1098)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL1101: _temp1100=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp1098)->f1; goto _LL1025;} else{ goto
_LL1026;}} else{ goto _LL1026;} _LL1026: if((( struct _tunion_struct*) _temp1020)->tag
== Cyc_Absyn_Var_e_tag){ _LL1109: _temp1108=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1020)->f1; goto _LL1105; _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1020)->f2; if(( unsigned int) _temp1104 > 1u?(( struct _tunion_struct*)
_temp1104)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL1107: _temp1106=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp1104)->f1; goto _LL1027;} else{ goto
_LL1028;}} else{ goto _LL1028;} _LL1028: if((( struct _tunion_struct*) _temp1020)->tag
== Cyc_Absyn_Var_e_tag){ _LL1113: _temp1112=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1020)->f1; goto _LL1111; _LL1111: _temp1110=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1020)->f2; goto _LL1029;} else{ goto _LL1030;} _LL1030: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1115:
_temp1114=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1020)->f1;
goto _LL1031;} else{ goto _LL1032;} _LL1032: if((( struct _tunion_struct*)
_temp1020)->tag == Cyc_Absyn_Primop_e_tag){ _LL1119: _temp1118=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1020)->f1; goto _LL1117; _LL1117: _temp1116=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1020)->f2; goto
_LL1033;} else{ goto _LL1034;} _LL1034: if((( struct _tunion_struct*) _temp1020)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1125: _temp1124=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1020)->f1; goto _LL1123; _LL1123:
_temp1122=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1020)->f2; goto _LL1121; _LL1121: _temp1120=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1020)->f3; goto _LL1035;} else{ goto
_LL1036;} _LL1036: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Increment_e_tag){
_LL1129: _temp1128=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1020)->f1; goto _LL1127; _LL1127: _temp1126=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1020)->f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1135:
_temp1134=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1020)->f1; goto _LL1133; _LL1133: _temp1132=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1020)->f2; goto _LL1131; _LL1131:
_temp1130=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1020)->f3; goto _LL1039;} else{ goto _LL1040;} _LL1040: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1139: _temp1138=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1020)->f1; goto
_LL1137; _LL1137: _temp1136=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1020)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1143:
_temp1142=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1020)->f1; goto _LL1141; _LL1141: _temp1140=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1020)->f2; goto _LL1043;} else{
goto _LL1044;} _LL1044: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1147: _temp1146=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1020)->f1; goto _LL1145; _LL1145: _temp1144=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1020)->f2; goto _LL1045;} else{ goto
_LL1046;} _LL1046: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Throw_e_tag){
_LL1149: _temp1148=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1020)->f1; goto _LL1047;} else{ goto _LL1048;} _LL1048: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1151:
_temp1150=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1020)->f1; goto _LL1049;} else{ goto _LL1050;} _LL1050: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1155:
_temp1154=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1020)->f1; goto _LL1153; _LL1153: _temp1152=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1020)->f2; goto _LL1051;} else{
goto _LL1052;} _LL1052: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Cast_e_tag){
_LL1159: _temp1158=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1020)->f1;
goto _LL1157; _LL1157: _temp1156=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1020)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_Address_e_tag){ _LL1161: _temp1160=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp1020)->f1;
goto _LL1055;} else{ goto _LL1056;} _LL1056: if((( struct _tunion_struct*)
_temp1020)->tag == Cyc_Absyn_New_e_tag){ _LL1163: _temp1162=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1020)->f1; goto _LL1057;} else{ goto
_LL1058;} _LL1058: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1165: _temp1164=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1020)->f1;
goto _LL1059;} else{ goto _LL1060;} _LL1060: if((( struct _tunion_struct*)
_temp1020)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1167: _temp1166=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1020)->f1; goto _LL1061;} else{ goto
_LL1062;} _LL1062: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Deref_e_tag){
_LL1169: _temp1168=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1020)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1173:
_temp1172=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1020)->f1; goto _LL1171; _LL1171: _temp1170=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1020)->f2; goto _LL1065;} else{
goto _LL1066;} _LL1066: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1177: _temp1176=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1020)->f1; goto _LL1175; _LL1175: _temp1174=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1020)->f2; goto _LL1067;} else{
goto _LL1068;} _LL1068: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1181: _temp1180=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1020)->f1; goto _LL1179; _LL1179: _temp1178=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1020)->f2; goto _LL1069;} else{ goto
_LL1070;} _LL1070: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1183: _temp1182=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1020)->f1; goto _LL1071;} else{ goto _LL1072;} _LL1072: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1187:
_temp1186=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1020)->f1;
goto _LL1185; _LL1185: _temp1184=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1020)->f2; goto _LL1073;} else{ goto _LL1074;} _LL1074: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_Array_e_tag){ _LL1189: _temp1188=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1020)->f1; goto
_LL1075;} else{ goto _LL1076;} _LL1076: if((( struct _tunion_struct*) _temp1020)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1195: _temp1194=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1020)->f1; goto _LL1193; _LL1193:
_temp1192=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1020)->f2; goto _LL1191; _LL1191: _temp1190=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1020)->f3; goto _LL1077;} else{
goto _LL1078;} _LL1078: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Struct_e_tag){
_LL1203: _temp1202=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1020)->f1; goto _LL1201; _LL1201: _temp1200=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1020)->f2; goto _LL1199; _LL1199:
_temp1198=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1020)->f3; goto _LL1197; _LL1197: _temp1196=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1020)->f4; goto _LL1079;} else{ goto
_LL1080;} _LL1080: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1213: _temp1212=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1020)->f1; goto _LL1211; _LL1211: _temp1210=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1020)->f2; goto _LL1209; _LL1209:
_temp1208=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1020)->f3; goto _LL1207; _LL1207: _temp1206=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1020)->f4; goto _LL1205; _LL1205:
_temp1204=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1020)->f5; goto _LL1081;} else{ goto _LL1082;} _LL1082: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1221: _temp1220=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1020)->f1; goto
_LL1219; _LL1219: _temp1218=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1020)->f2; goto _LL1217; _LL1217: _temp1216=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1020)->f3; goto _LL1215; _LL1215:
_temp1214=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1020)->f4; goto _LL1083;} else{ goto _LL1084;} _LL1084: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_Enum_e_tag){ _LL1227: _temp1226=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1020)->f1; goto _LL1225;
_LL1225: _temp1224=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1020)->f2; goto _LL1223; _LL1223: _temp1222=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1020)->f3; goto _LL1085;} else{ goto
_LL1086;} _LL1086: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1229: _temp1228=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1020)->f1;
goto _LL1087;} else{ goto _LL1088;} _LL1088: if((( struct _tunion_struct*)
_temp1020)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1233: _temp1232=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1020)->f1; goto
_LL1231; _LL1231: _temp1230=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1020)->f2; goto _LL1089;} else{ goto _LL1090;} _LL1090: if((( struct
_tunion_struct*) _temp1020)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1235: _temp1234=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1020)->f1;
goto _LL1091;} else{ goto _LL1092;} _LL1092: if((( struct _tunion_struct*)
_temp1020)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1237: _temp1236=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1020)->f1; goto _LL1093;} else{ goto
_LL1094;} _LL1094: if((( struct _tunion_struct*) _temp1020)->tag == Cyc_Absyn_Fill_e_tag){
_LL1239: _temp1238=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp1020)->f1; goto _LL1095;} else{ goto _LL1021;} _LL1023: s= Cyc_Absynpp_cnst2doc(
_temp1096); goto _LL1021; _LL1025: s= Cyc_Absynpp_qvar2doc( _temp1102); goto
_LL1021; _LL1027: s= Cyc_Absynpp_qvar2doc( _temp1108); goto _LL1021; _LL1029: s=
Cyc_Absynpp_qvar2doc( _temp1112); goto _LL1021; _LL1031: s= Cyc_Absynpp_qvar2doc(
_temp1114); goto _LL1021; _LL1033: s= Cyc_Absynpp_primapp2doc( myprec, _temp1118,
_temp1116); goto _LL1021; _LL1035: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1124), Cyc_PP_text(( struct _tagged_string)({ char* _temp1240=(
char*)" "; struct _tagged_string _temp1241; _temp1241.curr= _temp1240; _temp1241.base=
_temp1240; _temp1241.last_plus_one= _temp1240 + 2; _temp1241;})), _temp1122 == 0?
Cyc_PP_text(( struct _tagged_string)({ char* _temp1242=( char*)""; struct
_tagged_string _temp1243; _temp1243.curr= _temp1242; _temp1243.base= _temp1242;
_temp1243.last_plus_one= _temp1242 + 1; _temp1243;})): Cyc_Absynpp_prim2doc((
void*) _temp1122->v), Cyc_PP_text(( struct _tagged_string)({ char* _temp1244=(
char*)"= "; struct _tagged_string _temp1245; _temp1245.curr= _temp1244;
_temp1245.base= _temp1244; _temp1245.last_plus_one= _temp1244 + 3; _temp1245;})),
Cyc_Absynpp_exp2doc_prec( myprec, _temp1120)); goto _LL1021; _LL1037: { struct
Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec, _temp1128);{ void* _temp1246=
_temp1126; _LL1248: if(( int) _temp1246 == Cyc_Absyn_PreInc){ goto _LL1249;}
else{ goto _LL1250;} _LL1250: if(( int) _temp1246 == Cyc_Absyn_PreDec){ goto
_LL1251;} else{ goto _LL1252;} _LL1252: if(( int) _temp1246 == Cyc_Absyn_PostInc){
goto _LL1253;} else{ goto _LL1254;} _LL1254: if(( int) _temp1246 == Cyc_Absyn_PostDec){
goto _LL1255;} else{ goto _LL1247;} _LL1249: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1256=( char*)"++"; struct _tagged_string
_temp1257; _temp1257.curr= _temp1256; _temp1257.base= _temp1256; _temp1257.last_plus_one=
_temp1256 + 3; _temp1257;})), es); goto _LL1247; _LL1251: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1258=( char*)"--"; struct
_tagged_string _temp1259; _temp1259.curr= _temp1258; _temp1259.base= _temp1258;
_temp1259.last_plus_one= _temp1258 + 3; _temp1259;})), es); goto _LL1247;
_LL1253: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1260=( char*)"++"; struct _tagged_string _temp1261; _temp1261.curr=
_temp1260; _temp1261.base= _temp1260; _temp1261.last_plus_one= _temp1260 + 3;
_temp1261;}))); goto _LL1247; _LL1255: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1262=( char*)"--"; struct _tagged_string
_temp1263; _temp1263.curr= _temp1262; _temp1263.base= _temp1262; _temp1263.last_plus_one=
_temp1262 + 3; _temp1263;}))); goto _LL1247; _LL1247:;} goto _LL1021;} _LL1039:{
struct _tuple8 _temp1265=({ struct _tuple8 _temp1264; _temp1264.f1=( void*)
_temp1132->r; _temp1264.f2=( void*) _temp1130->r; _temp1264;}); _LL1267: goto
_LL1268; _LL1268: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1134), Cyc_PP_text(( struct _tagged_string)({ char* _temp1269=( char*)" ? ";
struct _tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base=
_temp1269; _temp1270.last_plus_one= _temp1269 + 4; _temp1270;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1132), Cyc_PP_text(( struct _tagged_string)({ char* _temp1271=( char*)" : ";
struct _tagged_string _temp1272; _temp1272.curr= _temp1271; _temp1272.base=
_temp1271; _temp1272.last_plus_one= _temp1271 + 4; _temp1272;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1130)); goto _LL1266; _LL1266:;} goto _LL1021; _LL1041: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1138), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1273=( char*)", "; struct _tagged_string _temp1274;
_temp1274.curr= _temp1273; _temp1274.base= _temp1273; _temp1274.last_plus_one=
_temp1273 + 3; _temp1274;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1136));
goto _LL1021; _LL1043: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1142), Cyc_PP_text(( struct _tagged_string)({ char* _temp1275=( char*)"(";
struct _tagged_string _temp1276; _temp1276.curr= _temp1275; _temp1276.base=
_temp1275; _temp1276.last_plus_one= _temp1275 + 2; _temp1276;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1140), Cyc_PP_text(( struct _tagged_string)({ char* _temp1277=( char*)")";
struct _tagged_string _temp1278; _temp1278.curr= _temp1277; _temp1278.base=
_temp1277; _temp1278.last_plus_one= _temp1277 + 2; _temp1278;}))); goto _LL1021;
_LL1045: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1146), Cyc_PP_text((
struct _tagged_string)({ char* _temp1279=( char*)"("; struct _tagged_string
_temp1280; _temp1280.curr= _temp1279; _temp1280.base= _temp1279; _temp1280.last_plus_one=
_temp1279 + 2; _temp1280;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1144), Cyc_PP_text((
struct _tagged_string)({ char* _temp1281=( char*)")"; struct _tagged_string
_temp1282; _temp1282.curr= _temp1281; _temp1282.base= _temp1281; _temp1282.last_plus_one=
_temp1281 + 2; _temp1282;}))); goto _LL1021; _LL1047: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1283=( char*)"throw "; struct _tagged_string
_temp1284; _temp1284.curr= _temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one=
_temp1283 + 7; _temp1284;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1148));
goto _LL1021; _LL1049: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1150); goto
_LL1021; _LL1051: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1154); goto _LL1021;
_LL1053: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1285=( char*)"("; struct _tagged_string _temp1286; _temp1286.curr=
_temp1285; _temp1286.base= _temp1285; _temp1286.last_plus_one= _temp1285 + 2;
_temp1286;})), Cyc_Absynpp_typ2doc( _temp1158), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1287=( char*)")"; struct _tagged_string _temp1288;
_temp1288.curr= _temp1287; _temp1288.base= _temp1287; _temp1288.last_plus_one=
_temp1287 + 2; _temp1288;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1156));
goto _LL1021; _LL1055: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1289=( char*)"&"; struct _tagged_string _temp1290; _temp1290.curr=
_temp1289; _temp1290.base= _temp1289; _temp1290.last_plus_one= _temp1289 + 2;
_temp1290;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1160)); goto _LL1021;
_LL1057: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1291=( char*)"new "; struct _tagged_string _temp1292; _temp1292.curr=
_temp1291; _temp1292.base= _temp1291; _temp1292.last_plus_one= _temp1291 + 5;
_temp1292;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1162)); goto _LL1021;
_LL1059: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1293=( char*)"sizeof("; struct _tagged_string _temp1294; _temp1294.curr=
_temp1293; _temp1294.base= _temp1293; _temp1294.last_plus_one= _temp1293 + 8;
_temp1294;})), Cyc_Absynpp_typ2doc( _temp1164), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1295=( char*)")"; struct _tagged_string _temp1296;
_temp1296.curr= _temp1295; _temp1296.base= _temp1295; _temp1296.last_plus_one=
_temp1295 + 2; _temp1296;}))); goto _LL1021; _LL1061: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1297=( char*)"sizeof("; struct
_tagged_string _temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297;
_temp1298.last_plus_one= _temp1297 + 8; _temp1298;})), Cyc_Absynpp_exp2doc(
_temp1166), Cyc_PP_text(( struct _tagged_string)({ char* _temp1299=( char*)")";
struct _tagged_string _temp1300; _temp1300.curr= _temp1299; _temp1300.base=
_temp1299; _temp1300.last_plus_one= _temp1299 + 2; _temp1300;}))); goto _LL1021;
_LL1063: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1301=( char*)"*"; struct _tagged_string _temp1302; _temp1302.curr=
_temp1301; _temp1302.base= _temp1301; _temp1302.last_plus_one= _temp1301 + 2;
_temp1302;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1168)); goto _LL1021;
_LL1065: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1172), Cyc_PP_text((
struct _tagged_string)({ char* _temp1303=( char*)"."; struct _tagged_string
_temp1304; _temp1304.curr= _temp1303; _temp1304.base= _temp1303; _temp1304.last_plus_one=
_temp1303 + 2; _temp1304;})), Cyc_Absynpp_textptr( _temp1170)); goto _LL1021;
_LL1067: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec, _temp1176), Cyc_PP_text((
struct _tagged_string)({ char* _temp1305=( char*)"->"; struct _tagged_string
_temp1306; _temp1306.curr= _temp1305; _temp1306.base= _temp1305; _temp1306.last_plus_one=
_temp1305 + 3; _temp1306;})), Cyc_Absynpp_textptr( _temp1174)); goto _LL1021;
_LL1069: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1180), Cyc_PP_text((
struct _tagged_string)({ char* _temp1307=( char*)"["; struct _tagged_string
_temp1308; _temp1308.curr= _temp1307; _temp1308.base= _temp1307; _temp1308.last_plus_one=
_temp1307 + 2; _temp1308;})), Cyc_Absynpp_exp2doc( _temp1178), Cyc_PP_text((
struct _tagged_string)({ char* _temp1309=( char*)"]"; struct _tagged_string
_temp1310; _temp1310.curr= _temp1309; _temp1310.base= _temp1309; _temp1310.last_plus_one=
_temp1309 + 2; _temp1310;}))); goto _LL1021; _LL1071: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1311=( char*)"$("; struct _tagged_string
_temp1312; _temp1312.curr= _temp1311; _temp1312.base= _temp1311; _temp1312.last_plus_one=
_temp1311 + 3; _temp1312;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1182), Cyc_PP_text((
struct _tagged_string)({ char* _temp1313=( char*)")"; struct _tagged_string
_temp1314; _temp1314.curr= _temp1313; _temp1314.base= _temp1313; _temp1314.last_plus_one=
_temp1313 + 2; _temp1314;}))); goto _LL1021; _LL1073: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1315=( char*)"("; struct _tagged_string
_temp1316; _temp1316.curr= _temp1315; _temp1316.base= _temp1315; _temp1316.last_plus_one=
_temp1315 + 2; _temp1316;})), Cyc_Absynpp_typ2doc((* _temp1186).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1317=( char*)")"; struct _tagged_string
_temp1318; _temp1318.curr= _temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one=
_temp1317 + 2; _temp1318;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1319=( char*)"{"; struct _tagged_string _temp1320; _temp1320.curr=
_temp1319; _temp1320.base= _temp1319; _temp1320.last_plus_one= _temp1319 + 2;
_temp1320;}),( struct _tagged_string)({ char* _temp1321=( char*)"}"; struct
_tagged_string _temp1322; _temp1322.curr= _temp1321; _temp1322.base= _temp1321;
_temp1322.last_plus_one= _temp1321 + 2; _temp1322;}),( struct _tagged_string)({
char* _temp1323=( char*)","; struct _tagged_string _temp1324; _temp1324.curr=
_temp1323; _temp1324.base= _temp1323; _temp1324.last_plus_one= _temp1323 + 2;
_temp1324;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1184))); goto _LL1021; _LL1075: s= Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1325=( char*)"{"; struct _tagged_string _temp1326;
_temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 2; _temp1326;}),( struct _tagged_string)({ char* _temp1327=( char*)"}";
struct _tagged_string _temp1328; _temp1328.curr= _temp1327; _temp1328.base=
_temp1327; _temp1328.last_plus_one= _temp1327 + 2; _temp1328;}),( struct
_tagged_string)({ char* _temp1329=( char*)","; struct _tagged_string _temp1330;
_temp1330.curr= _temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one=
_temp1329 + 2; _temp1330;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1188)); goto _LL1021; _LL1077: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1331=( char*)"new {for "; struct _tagged_string
_temp1332; _temp1332.curr= _temp1331; _temp1332.base= _temp1331; _temp1332.last_plus_one=
_temp1331 + 10; _temp1332;})), Cyc_PP_text(*(* _temp1194->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1333=( char*)" < "; struct _tagged_string
_temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 4; _temp1334;})), Cyc_Absynpp_exp2doc( _temp1192), Cyc_PP_text((
struct _tagged_string)({ char* _temp1335=( char*)" : "; struct _tagged_string
_temp1336; _temp1336.curr= _temp1335; _temp1336.base= _temp1335; _temp1336.last_plus_one=
_temp1335 + 4; _temp1336;})), Cyc_Absynpp_exp2doc( _temp1190), Cyc_PP_text((
struct _tagged_string)({ char* _temp1337=( char*)"}"; struct _tagged_string
_temp1338; _temp1338.curr= _temp1337; _temp1338.base= _temp1337; _temp1338.last_plus_one=
_temp1337 + 2; _temp1338;}))); goto _LL1021; _LL1079: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1202), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1339=( char*)"{";
struct _tagged_string _temp1340; _temp1340.curr= _temp1339; _temp1340.base=
_temp1339; _temp1340.last_plus_one= _temp1339 + 2; _temp1340;}),( struct
_tagged_string)({ char* _temp1341=( char*)"}"; struct _tagged_string _temp1342;
_temp1342.curr= _temp1341; _temp1342.base= _temp1341; _temp1342.last_plus_one=
_temp1341 + 2; _temp1342;}),( struct _tagged_string)({ char* _temp1343=( char*)",";
struct _tagged_string _temp1344; _temp1344.curr= _temp1343; _temp1344.base=
_temp1343; _temp1344.last_plus_one= _temp1343 + 2; _temp1344;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1198))); goto _LL1021; _LL1081: _temp1218= _temp1208;
_temp1214= _temp1204; goto _LL1083; _LL1083: if( _temp1218 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1214->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1214->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1345=( char*)"(";
struct _tagged_string _temp1346; _temp1346.curr= _temp1345; _temp1346.base=
_temp1345; _temp1346.last_plus_one= _temp1345 + 2; _temp1346;}),( struct
_tagged_string)({ char* _temp1347=( char*)")"; struct _tagged_string _temp1348;
_temp1348.curr= _temp1347; _temp1348.base= _temp1347; _temp1348.last_plus_one=
_temp1347 + 2; _temp1348;}),( struct _tagged_string)({ char* _temp1349=( char*)",";
struct _tagged_string _temp1350; _temp1350.curr= _temp1349; _temp1350.base=
_temp1349; _temp1350.last_plus_one= _temp1349 + 2; _temp1350;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1218)));} goto _LL1021; _LL1085: s= Cyc_Absynpp_qvar2doc(
_temp1226); goto _LL1021; _LL1087: { struct Cyc_PP_Doc* inner_d;{ void*
_temp1351= _temp1228; void* _temp1361; struct Cyc_Absyn_Tunionfield* _temp1363;
struct Cyc_Absyn_Tuniondecl* _temp1365; struct Cyc_Absyn_Tunionfield* _temp1367;
struct Cyc_Absyn_XTuniondecl* _temp1369; struct _tuple0* _temp1371; _LL1353: if(((
struct _tunion_struct*) _temp1351)->tag == Cyc_Absyn_Typ_m_tag){ _LL1362:
_temp1361=( void*)(( struct Cyc_Absyn_Typ_m_struct*) _temp1351)->f1; goto
_LL1354;} else{ goto _LL1355;} _LL1355: if((( struct _tunion_struct*) _temp1351)->tag
== Cyc_Absyn_Tunion_m_tag){ _LL1366: _temp1365=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_m_struct*) _temp1351)->f1; goto _LL1364; _LL1364:
_temp1363=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_m_struct*)
_temp1351)->f2; goto _LL1356;} else{ goto _LL1357;} _LL1357: if((( struct
_tunion_struct*) _temp1351)->tag == Cyc_Absyn_XTunion_m_tag){ _LL1370: _temp1369=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_m_struct*) _temp1351)->f1;
goto _LL1368; _LL1368: _temp1367=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_m_struct*)
_temp1351)->f2; goto _LL1358;} else{ goto _LL1359;} _LL1359: if((( struct
_tunion_struct*) _temp1351)->tag == Cyc_Absyn_Unresolved_m_tag){ _LL1372:
_temp1371=( struct _tuple0*)(( struct Cyc_Absyn_Unresolved_m_struct*) _temp1351)->f1;
goto _LL1360;} else{ goto _LL1352;} _LL1354: inner_d= Cyc_Absynpp_typ2doc(
_temp1361); goto _LL1352; _LL1356: _temp1367= _temp1363; goto _LL1358; _LL1358:
_temp1371= _temp1367->name; goto _LL1360; _LL1360: inner_d= Cyc_Absynpp_qvar2doc(
_temp1371); goto _LL1352; _LL1352:;} s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1373=( char*)"malloc(sizeof("; struct
_tagged_string _temp1374; _temp1374.curr= _temp1373; _temp1374.base= _temp1373;
_temp1374.last_plus_one= _temp1373 + 15; _temp1374;})), inner_d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1375=( char*)"))"; struct _tagged_string
_temp1376; _temp1376.curr= _temp1375; _temp1376.base= _temp1375; _temp1376.last_plus_one=
_temp1375 + 3; _temp1376;}))); goto _LL1021;} _LL1089: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1377=( char*)"{"; struct _tagged_string
_temp1378; _temp1378.curr= _temp1377; _temp1378.base= _temp1377; _temp1378.last_plus_one=
_temp1377 + 2; _temp1378;}),( struct _tagged_string)({ char* _temp1379=( char*)"}";
struct _tagged_string _temp1380; _temp1380.curr= _temp1379; _temp1380.base=
_temp1379; _temp1380.last_plus_one= _temp1379 + 2; _temp1380;}),( struct
_tagged_string)({ char* _temp1381=( char*)","; struct _tagged_string _temp1382;
_temp1382.curr= _temp1381; _temp1382.base= _temp1381; _temp1382.last_plus_one=
_temp1381 + 2; _temp1382;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1230)); goto _LL1021; _LL1091: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1383=( char*)"({ "; struct _tagged_string _temp1384;
_temp1384.curr= _temp1383; _temp1384.base= _temp1383; _temp1384.last_plus_one=
_temp1383 + 4; _temp1384;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1234)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1385=( char*)" })"; struct
_tagged_string _temp1386; _temp1386.curr= _temp1385; _temp1386.base= _temp1385;
_temp1386.last_plus_one= _temp1385 + 4; _temp1386;}))); goto _LL1021; _LL1093: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1387=( char*)"codegen(";
struct _tagged_string _temp1388; _temp1388.curr= _temp1387; _temp1388.base=
_temp1387; _temp1388.last_plus_one= _temp1387 + 9; _temp1388;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1389=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1389->r=( void*)(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp1390=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp1390[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp1391; _temp1391.tag= Cyc_Absyn_Fn_d_tag; _temp1391.f1= _temp1236; _temp1391;});
_temp1390;})); _temp1389->loc= e->loc; _temp1389;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1392=( char*)")"; struct _tagged_string _temp1393;
_temp1393.curr= _temp1392; _temp1393.base= _temp1392; _temp1393.last_plus_one=
_temp1392 + 2; _temp1393;}))); goto _LL1021; _LL1095: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1394=( char*)"fill("; struct _tagged_string
_temp1395; _temp1395.curr= _temp1394; _temp1395.base= _temp1394; _temp1395.last_plus_one=
_temp1394 + 6; _temp1395;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1238)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1396=( char*)")"; struct
_tagged_string _temp1397; _temp1397.curr= _temp1396; _temp1397.base= _temp1396;
_temp1397.last_plus_one= _temp1396 + 2; _temp1397;}))); goto _LL1021; _LL1021:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1398=( char*)"("; struct _tagged_string _temp1399; _temp1399.curr=
_temp1398; _temp1399.base= _temp1398; _temp1399.last_plus_one= _temp1398 + 2;
_temp1399;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1400=( char*)")";
struct _tagged_string _temp1401; _temp1401.curr= _temp1400; _temp1401.base=
_temp1400; _temp1401.last_plus_one= _temp1400 + 2; _temp1401;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1402= d;
struct Cyc_Absyn_Exp* _temp1408; struct _tagged_string* _temp1410; _LL1404: if(((
struct _tunion_struct*) _temp1402)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1409:
_temp1408=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1402)->f1; goto _LL1405;} else{ goto _LL1406;} _LL1406: if((( struct
_tunion_struct*) _temp1402)->tag == Cyc_Absyn_FieldName_tag){ _LL1411: _temp1410=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1402)->f1;
goto _LL1407;} else{ goto _LL1403;} _LL1405: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1412=( char*)".["; struct _tagged_string
_temp1413; _temp1413.curr= _temp1412; _temp1413.base= _temp1412; _temp1413.last_plus_one=
_temp1412 + 3; _temp1413;})), Cyc_Absynpp_exp2doc( _temp1408), Cyc_PP_text((
struct _tagged_string)({ char* _temp1414=( char*)"]"; struct _tagged_string
_temp1415; _temp1415.curr= _temp1414; _temp1415.base= _temp1414; _temp1415.last_plus_one=
_temp1414 + 2; _temp1415;}))); _LL1407: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1416=( char*)"."; struct _tagged_string
_temp1417; _temp1417.curr= _temp1416; _temp1417.base= _temp1416; _temp1417.last_plus_one=
_temp1416 + 2; _temp1417;})), Cyc_Absynpp_textptr( _temp1410)); _LL1403:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1418=( char*)""; struct _tagged_string
_temp1419; _temp1419.curr= _temp1418; _temp1419.base= _temp1418; _temp1419.last_plus_one=
_temp1418 + 1; _temp1419;}),( struct _tagged_string)({ char* _temp1420=( char*)"=";
struct _tagged_string _temp1421; _temp1421.curr= _temp1420; _temp1421.base=
_temp1420; _temp1421.last_plus_one= _temp1420 + 2; _temp1421;}),( struct
_tagged_string)({ char* _temp1422=( char*)"="; struct _tagged_string _temp1423;
_temp1423.curr= _temp1422; _temp1423.base= _temp1422; _temp1423.last_plus_one=
_temp1422 + 2; _temp1423;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1424=( char*)""; struct _tagged_string _temp1425;
_temp1425.curr= _temp1424; _temp1425.base= _temp1424; _temp1425.last_plus_one=
_temp1424 + 1; _temp1425;}),( struct _tagged_string)({ char* _temp1426=( char*)"";
struct _tagged_string _temp1427; _temp1427.curr= _temp1426; _temp1427.base=
_temp1426; _temp1427.last_plus_one= _temp1426 + 1; _temp1427;}),( struct
_tagged_string)({ char* _temp1428=( char*)","; struct _tagged_string _temp1429;
_temp1429.curr= _temp1428; _temp1429.base= _temp1428; _temp1429.last_plus_one=
_temp1428 + 2; _temp1429;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1430= c; char _temp1448; void* _temp1450; short _temp1452;
void* _temp1454; int _temp1456; void* _temp1458; int _temp1460; void* _temp1462;
long long _temp1464; void* _temp1466; struct _tagged_string _temp1468; struct
_tagged_string _temp1470; _LL1432: if(( unsigned int) _temp1430 > 1u?(( struct
_tunion_struct*) _temp1430)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1451: _temp1450=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1430)->f1; goto _LL1449; _LL1449:
_temp1448=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1430)->f2; goto
_LL1433;} else{ goto _LL1434;} _LL1434: if(( unsigned int) _temp1430 > 1u?((
struct _tunion_struct*) _temp1430)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1455:
_temp1454=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1430)->f1; goto
_LL1453; _LL1453: _temp1452=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1430)->f2; goto _LL1435;} else{ goto _LL1436;} _LL1436: if(( unsigned int)
_temp1430 > 1u?(( struct _tunion_struct*) _temp1430)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1430)->f1;
if(( int) _temp1458 == Cyc_Absyn_Signed){ goto _LL1457;} else{ goto _LL1438;}
_LL1457: _temp1456=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1430)->f2; goto
_LL1437;} else{ goto _LL1438;} _LL1438: if(( unsigned int) _temp1430 > 1u?((
struct _tunion_struct*) _temp1430)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1463:
_temp1462=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1430)->f1; if(( int)
_temp1462 == Cyc_Absyn_Unsigned){ goto _LL1461;} else{ goto _LL1440;} _LL1461:
_temp1460=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1430)->f2; goto _LL1439;}
else{ goto _LL1440;} _LL1440: if(( unsigned int) _temp1430 > 1u?(( struct
_tunion_struct*) _temp1430)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL1467:
_temp1466=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1430)->f1; goto
_LL1465; _LL1465: _temp1464=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1430)->f2; goto _LL1441;} else{ goto _LL1442;} _LL1442: if(( unsigned int)
_temp1430 > 1u?(( struct _tunion_struct*) _temp1430)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1469: _temp1468=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1430)->f1; goto _LL1443;} else{ goto _LL1444;} _LL1444: if(( int) _temp1430
== Cyc_Absyn_Null_c){ goto _LL1445;} else{ goto _LL1446;} _LL1446: if((
unsigned int) _temp1430 > 1u?(( struct _tunion_struct*) _temp1430)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1471: _temp1470=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1430)->f1; goto _LL1447;} else{ goto _LL1431;} _LL1433: return Cyc_PP_text(({
struct _tagged_string _temp1472= Cyc_Absynpp_char_escape( _temp1448); xprintf("'%.*s'",
_temp1472.last_plus_one - _temp1472.curr, _temp1472.curr);})); _LL1435: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1452)); _LL1437: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1456)); _LL1439: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1460)); _LL1441: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1473=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1474; _temp1474.curr= _temp1473; _temp1474.base= _temp1473; _temp1474.last_plus_one=
_temp1473 + 27; _temp1474;})); _LL1443: return Cyc_PP_text( _temp1468); _LL1445:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1475=( char*)"null";
struct _tagged_string _temp1476; _temp1476.curr= _temp1475; _temp1476.base=
_temp1475; _temp1476.last_plus_one= _temp1475 + 5; _temp1476;})); _LL1447:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1477=(
char*)"\""; struct _tagged_string _temp1478; _temp1478.curr= _temp1477;
_temp1478.base= _temp1477; _temp1478.last_plus_one= _temp1477 + 2; _temp1478;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1470)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1479=( char*)"\""; struct _tagged_string _temp1480;
_temp1480.curr= _temp1479; _temp1480.base= _temp1479; _temp1480.last_plus_one=
_temp1479 + 2; _temp1480;}))); _LL1431:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1: es->tl != 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp1481=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1481[ 0]=({ struct Cyc_Core_Failure_struct _temp1482; _temp1482.tag= Cyc_Core_Failure_tag;
_temp1482.f1=({ struct _tagged_string _temp1483= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1483.last_plus_one
- _temp1483.curr, _temp1483.curr);}); _temp1482;}); _temp1481;}));}{ struct Cyc_PP_Doc*
d= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*) es->hd); return Cyc_Absynpp_cat2(
d, Cyc_PP_text(( struct _tagged_string)({ char* _temp1484=( char*)".size";
struct _tagged_string _temp1485; _temp1485.curr= _temp1484; _temp1485.base=
_temp1484; _temp1485.last_plus_one= _temp1484 + 6; _temp1485;})));}} else{ if( !
Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp1486=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1486[ 0]=({ struct Cyc_Core_Failure_struct _temp1487; _temp1487.tag= Cyc_Core_Failure_tag;
_temp1487.f1=({ struct _tagged_string _temp1488= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1488.last_plus_one -
_temp1488.curr, _temp1488.curr);}); _temp1487;}); _temp1486;}));} else{ if( ds->tl
== 0){ return Cyc_Absynpp_cat3( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1489=( char*)" "; struct _tagged_string _temp1490; _temp1490.curr=
_temp1489; _temp1490.base= _temp1489; _temp1490.last_plus_one= _temp1489 + 2;
_temp1490;})),( struct Cyc_PP_Doc*) ds->hd);} else{ if(( ds->tl)->tl != 0){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp1491=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1491[ 0]=({ struct Cyc_Core_Failure_struct _temp1492; _temp1492.tag= Cyc_Core_Failure_tag;
_temp1492.f1=({ struct _tagged_string _temp1493= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1493.last_plus_one
- _temp1493.curr, _temp1493.curr);}); _temp1492;}); _temp1491;}));} else{ return
Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*) ds->hd, Cyc_PP_text(( struct
_tagged_string)({ char* _temp1494=( char*)" "; struct _tagged_string _temp1495;
_temp1495.curr= _temp1494; _temp1495.base= _temp1494; _temp1495.last_plus_one=
_temp1494 + 2; _temp1495;})), ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1496=( char*)" "; struct _tagged_string _temp1497; _temp1497.curr=
_temp1496; _temp1497.base= _temp1496; _temp1497.last_plus_one= _temp1496 + 2;
_temp1497;})),( struct Cyc_PP_Doc*)( ds->tl)->hd);}}}} else{ return Cyc_Absynpp_cat4(
ps, Cyc_PP_text(( struct _tagged_string)({ char* _temp1498=( char*)"("; struct
_tagged_string _temp1499; _temp1499.curr= _temp1498; _temp1499.base= _temp1498;
_temp1499.last_plus_one= _temp1498 + 2; _temp1499;})), Cyc_Absynpp_exps2doc_prec(
20, es), Cyc_PP_text(( struct _tagged_string)({ char* _temp1500=( char*)")";
struct _tagged_string _temp1501; _temp1501.curr= _temp1500; _temp1501.base=
_temp1500; _temp1501.last_plus_one= _temp1500 + 2; _temp1501;})));}}} struct
_tagged_string Cyc_Absynpp_prim2str( void* p){ void* _temp1502= p; _LL1504: if((
int) _temp1502 == Cyc_Absyn_Plus){ goto _LL1505;} else{ goto _LL1506;} _LL1506:
if(( int) _temp1502 == Cyc_Absyn_Times){ goto _LL1507;} else{ goto _LL1508;}
_LL1508: if(( int) _temp1502 == Cyc_Absyn_Minus){ goto _LL1509;} else{ goto
_LL1510;} _LL1510: if(( int) _temp1502 == Cyc_Absyn_Div){ goto _LL1511;} else{
goto _LL1512;} _LL1512: if(( int) _temp1502 == Cyc_Absyn_Mod){ goto _LL1513;}
else{ goto _LL1514;} _LL1514: if(( int) _temp1502 == Cyc_Absyn_Eq){ goto _LL1515;}
else{ goto _LL1516;} _LL1516: if(( int) _temp1502 == Cyc_Absyn_Neq){ goto
_LL1517;} else{ goto _LL1518;} _LL1518: if(( int) _temp1502 == Cyc_Absyn_Gt){
goto _LL1519;} else{ goto _LL1520;} _LL1520: if(( int) _temp1502 == Cyc_Absyn_Lt){
goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( int) _temp1502 == Cyc_Absyn_Gte){
goto _LL1523;} else{ goto _LL1524;} _LL1524: if(( int) _temp1502 == Cyc_Absyn_Lte){
goto _LL1525;} else{ goto _LL1526;} _LL1526: if(( int) _temp1502 == Cyc_Absyn_Not){
goto _LL1527;} else{ goto _LL1528;} _LL1528: if(( int) _temp1502 == Cyc_Absyn_Bitnot){
goto _LL1529;} else{ goto _LL1530;} _LL1530: if(( int) _temp1502 == Cyc_Absyn_Bitand){
goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( int) _temp1502 == Cyc_Absyn_Bitor){
goto _LL1533;} else{ goto _LL1534;} _LL1534: if(( int) _temp1502 == Cyc_Absyn_Bitxor){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if(( int) _temp1502 == Cyc_Absyn_Bitlshift){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( int) _temp1502 == Cyc_Absyn_Bitlrshift){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if(( int) _temp1502 == Cyc_Absyn_Bitarshift){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if(( int) _temp1502 == Cyc_Absyn_Size){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if(( int) _temp1502 == Cyc_Absyn_Printf){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if(( int) _temp1502 == Cyc_Absyn_Fprintf){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if(( int) _temp1502 == Cyc_Absyn_Xprintf){
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( int) _temp1502 == Cyc_Absyn_Scanf){
goto _LL1551;} else{ goto _LL1552;} _LL1552: if(( int) _temp1502 == Cyc_Absyn_Fscanf){
goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( int) _temp1502 == Cyc_Absyn_Sscanf){
goto _LL1555;} else{ goto _LL1503;} _LL1505: return( struct _tagged_string)({
char* _temp1556=( char*)"+"; struct _tagged_string _temp1557; _temp1557.curr=
_temp1556; _temp1557.base= _temp1556; _temp1557.last_plus_one= _temp1556 + 2;
_temp1557;}); _LL1507: return( struct _tagged_string)({ char* _temp1558=( char*)"*";
struct _tagged_string _temp1559; _temp1559.curr= _temp1558; _temp1559.base=
_temp1558; _temp1559.last_plus_one= _temp1558 + 2; _temp1559;}); _LL1509: return(
struct _tagged_string)({ char* _temp1560=( char*)"-"; struct _tagged_string
_temp1561; _temp1561.curr= _temp1560; _temp1561.base= _temp1560; _temp1561.last_plus_one=
_temp1560 + 2; _temp1561;}); _LL1511: return( struct _tagged_string)({ char*
_temp1562=( char*)"/"; struct _tagged_string _temp1563; _temp1563.curr=
_temp1562; _temp1563.base= _temp1562; _temp1563.last_plus_one= _temp1562 + 2;
_temp1563;}); _LL1513: return( struct _tagged_string)({ char* _temp1564=( char*)"%";
struct _tagged_string _temp1565; _temp1565.curr= _temp1564; _temp1565.base=
_temp1564; _temp1565.last_plus_one= _temp1564 + 2; _temp1565;}); _LL1515: return(
struct _tagged_string)({ char* _temp1566=( char*)"=="; struct _tagged_string
_temp1567; _temp1567.curr= _temp1566; _temp1567.base= _temp1566; _temp1567.last_plus_one=
_temp1566 + 3; _temp1567;}); _LL1517: return( struct _tagged_string)({ char*
_temp1568=( char*)"!="; struct _tagged_string _temp1569; _temp1569.curr=
_temp1568; _temp1569.base= _temp1568; _temp1569.last_plus_one= _temp1568 + 3;
_temp1569;}); _LL1519: return( struct _tagged_string)({ char* _temp1570=( char*)">";
struct _tagged_string _temp1571; _temp1571.curr= _temp1570; _temp1571.base=
_temp1570; _temp1571.last_plus_one= _temp1570 + 2; _temp1571;}); _LL1521: return(
struct _tagged_string)({ char* _temp1572=( char*)"<"; struct _tagged_string
_temp1573; _temp1573.curr= _temp1572; _temp1573.base= _temp1572; _temp1573.last_plus_one=
_temp1572 + 2; _temp1573;}); _LL1523: return( struct _tagged_string)({ char*
_temp1574=( char*)">="; struct _tagged_string _temp1575; _temp1575.curr=
_temp1574; _temp1575.base= _temp1574; _temp1575.last_plus_one= _temp1574 + 3;
_temp1575;}); _LL1525: return( struct _tagged_string)({ char* _temp1576=( char*)"<=";
struct _tagged_string _temp1577; _temp1577.curr= _temp1576; _temp1577.base=
_temp1576; _temp1577.last_plus_one= _temp1576 + 3; _temp1577;}); _LL1527: return(
struct _tagged_string)({ char* _temp1578=( char*)"!"; struct _tagged_string
_temp1579; _temp1579.curr= _temp1578; _temp1579.base= _temp1578; _temp1579.last_plus_one=
_temp1578 + 2; _temp1579;}); _LL1529: return( struct _tagged_string)({ char*
_temp1580=( char*)"~"; struct _tagged_string _temp1581; _temp1581.curr=
_temp1580; _temp1581.base= _temp1580; _temp1581.last_plus_one= _temp1580 + 2;
_temp1581;}); _LL1531: return( struct _tagged_string)({ char* _temp1582=( char*)"&";
struct _tagged_string _temp1583; _temp1583.curr= _temp1582; _temp1583.base=
_temp1582; _temp1583.last_plus_one= _temp1582 + 2; _temp1583;}); _LL1533: return(
struct _tagged_string)({ char* _temp1584=( char*)"|"; struct _tagged_string
_temp1585; _temp1585.curr= _temp1584; _temp1585.base= _temp1584; _temp1585.last_plus_one=
_temp1584 + 2; _temp1585;}); _LL1535: return( struct _tagged_string)({ char*
_temp1586=( char*)"^"; struct _tagged_string _temp1587; _temp1587.curr=
_temp1586; _temp1587.base= _temp1586; _temp1587.last_plus_one= _temp1586 + 2;
_temp1587;}); _LL1537: return( struct _tagged_string)({ char* _temp1588=( char*)"<<";
struct _tagged_string _temp1589; _temp1589.curr= _temp1588; _temp1589.base=
_temp1588; _temp1589.last_plus_one= _temp1588 + 3; _temp1589;}); _LL1539: return(
struct _tagged_string)({ char* _temp1590=( char*)">>"; struct _tagged_string
_temp1591; _temp1591.curr= _temp1590; _temp1591.base= _temp1590; _temp1591.last_plus_one=
_temp1590 + 3; _temp1591;}); _LL1541: return( struct _tagged_string)({ char*
_temp1592=( char*)">>>"; struct _tagged_string _temp1593; _temp1593.curr=
_temp1592; _temp1593.base= _temp1592; _temp1593.last_plus_one= _temp1592 + 4;
_temp1593;}); _LL1543: return( struct _tagged_string)({ char* _temp1594=( char*)"size";
struct _tagged_string _temp1595; _temp1595.curr= _temp1594; _temp1595.base=
_temp1594; _temp1595.last_plus_one= _temp1594 + 5; _temp1595;}); _LL1545: return(
struct _tagged_string)({ char* _temp1596=( char*)"printf"; struct _tagged_string
_temp1597; _temp1597.curr= _temp1596; _temp1597.base= _temp1596; _temp1597.last_plus_one=
_temp1596 + 7; _temp1597;}); _LL1547: return( struct _tagged_string)({ char*
_temp1598=( char*)"fprintf"; struct _tagged_string _temp1599; _temp1599.curr=
_temp1598; _temp1599.base= _temp1598; _temp1599.last_plus_one= _temp1598 + 8;
_temp1599;}); _LL1549: return( struct _tagged_string)({ char* _temp1600=( char*)"xprintf";
struct _tagged_string _temp1601; _temp1601.curr= _temp1600; _temp1601.base=
_temp1600; _temp1601.last_plus_one= _temp1600 + 8; _temp1601;}); _LL1551: return(
struct _tagged_string)({ char* _temp1602=( char*)"scanf"; struct _tagged_string
_temp1603; _temp1603.curr= _temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one=
_temp1602 + 6; _temp1603;}); _LL1553: return( struct _tagged_string)({ char*
_temp1604=( char*)"fscanf"; struct _tagged_string _temp1605; _temp1605.curr=
_temp1604; _temp1605.base= _temp1604; _temp1605.last_plus_one= _temp1604 + 7;
_temp1605;}); _LL1555: return( struct _tagged_string)({ char* _temp1606=( char*)"sscanf";
struct _tagged_string _temp1607; _temp1607.curr= _temp1606; _temp1607.base=
_temp1606; _temp1607.last_plus_one= _temp1606 + 7; _temp1607;}); _LL1503:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)((* t).f1)->v), Cyc_PP_text(( struct _tagged_string)({
char* _temp1608=( char*)" = "; struct _tagged_string _temp1609; _temp1609.curr=
_temp1608; _temp1609.base= _temp1608; _temp1609.last_plus_one= _temp1608 + 4;
_temp1609;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_cat2(
struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2){ return Cyc_PP_concat( d1, d2);}
struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, d3));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, d4)));} struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3,
Cyc_PP_concat( d4, d5))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, d6)))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, d7))))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat8( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6,
Cyc_PP_concat( d7, d8)))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat9( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7,
Cyc_PP_concat( d8, d9))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat10( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, d10)))))))));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat11( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct
Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct Cyc_PP_Doc*
d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, Cyc_PP_concat( d10,
d11))))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc*
d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct
Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11, struct Cyc_PP_Doc*
d12){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat(
d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8,
Cyc_PP_concat( d9, Cyc_PP_concat( d10, Cyc_PP_concat( d11, d12)))))))))));} int
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1610=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1616; struct Cyc_Absyn_Decl* _temp1618;
_LL1612: if(( unsigned int) _temp1610 > 1u?(( struct _tunion_struct*) _temp1610)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1619: _temp1618=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1610)->f1; goto _LL1617; _LL1617:
_temp1616=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1610)->f2;
goto _LL1613;} else{ goto _LL1614;} _LL1614: goto _LL1615; _LL1613: return 1;
_LL1615: return 0; _LL1611:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1620=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1660; struct Cyc_Absyn_Stmt* _temp1662; struct Cyc_Absyn_Stmt*
_temp1664; struct Cyc_Absyn_Exp* _temp1666; struct Cyc_Absyn_Stmt* _temp1668;
struct Cyc_Absyn_Stmt* _temp1670; struct Cyc_Absyn_Exp* _temp1672; struct Cyc_Absyn_Stmt*
_temp1674; struct _tuple2 _temp1676; struct Cyc_Absyn_Stmt* _temp1678; struct
Cyc_Absyn_Exp* _temp1680; struct Cyc_Absyn_Stmt* _temp1682; struct Cyc_Absyn_Stmt*
_temp1684; struct Cyc_Absyn_Stmt* _temp1686; struct _tagged_string* _temp1688;
struct Cyc_Absyn_Stmt* _temp1690; struct _tuple2 _temp1692; struct Cyc_Absyn_Stmt*
_temp1694; struct Cyc_Absyn_Exp* _temp1696; struct _tuple2 _temp1698; struct Cyc_Absyn_Stmt*
_temp1700; struct Cyc_Absyn_Exp* _temp1702; struct Cyc_Absyn_Exp* _temp1704;
struct Cyc_List_List* _temp1706; struct Cyc_Absyn_Exp* _temp1708; struct Cyc_Absyn_Switch_clause**
_temp1710; struct Cyc_List_List* _temp1712; struct Cyc_Absyn_Switch_clause**
_temp1714; struct Cyc_List_List* _temp1716; struct Cyc_Absyn_Stmt* _temp1718;
struct Cyc_Absyn_Decl* _temp1720; struct Cyc_Absyn_Stmt* _temp1722; struct Cyc_Absyn_Stmt*
_temp1724; struct Cyc_Absyn_Stmt* _temp1726; struct _tagged_string* _temp1728;
struct _tuple2 _temp1730; struct Cyc_Absyn_Stmt* _temp1732; struct Cyc_Absyn_Exp*
_temp1734; struct Cyc_Absyn_Stmt* _temp1736; struct Cyc_List_List* _temp1738;
struct Cyc_Absyn_Stmt* _temp1740; _LL1622: if(( int) _temp1620 == Cyc_Absyn_Skip_s){
goto _LL1623;} else{ goto _LL1624;} _LL1624: if(( unsigned int) _temp1620 > 1u?((
struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1661:
_temp1660=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1620)->f1;
goto _LL1625;} else{ goto _LL1626;} _LL1626: if(( unsigned int) _temp1620 > 1u?((
struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1665:
_temp1664=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1620)->f1;
goto _LL1663; _LL1663: _temp1662=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1620)->f2; goto _LL1627;} else{ goto _LL1628;} _LL1628: if(( unsigned int)
_temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1667: _temp1666=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1620)->f1; goto _LL1629;} else{ goto _LL1630;} _LL1630: if(( unsigned int)
_temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1673: _temp1672=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1620)->f1; goto _LL1671; _LL1671: _temp1670=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1620)->f2; goto _LL1669; _LL1669:
_temp1668=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1620)->f3; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(( unsigned int)
_temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1677: _temp1676=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1620)->f1; _LL1681: _temp1680= _temp1676.f1; goto _LL1679; _LL1679:
_temp1678= _temp1676.f2; goto _LL1675; _LL1675: _temp1674=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1620)->f2; goto _LL1633;} else{ goto
_LL1634;} _LL1634: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1683: _temp1682=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1620)->f1; goto _LL1635;} else{ goto
_LL1636;} _LL1636: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1685: _temp1684=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1620)->f1; goto _LL1637;} else{ goto
_LL1638;} _LL1638: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1689: _temp1688=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1620)->f1; goto _LL1687;
_LL1687: _temp1686=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1620)->f2; goto _LL1639;} else{ goto _LL1640;} _LL1640: if(( unsigned int)
_temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1705: _temp1704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1620)->f1; goto _LL1699; _LL1699: _temp1698=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1620)->f2; _LL1703: _temp1702= _temp1698.f1; goto _LL1701; _LL1701:
_temp1700= _temp1698.f2; goto _LL1693; _LL1693: _temp1692=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1620)->f3; _LL1697: _temp1696= _temp1692.f1;
goto _LL1695; _LL1695: _temp1694= _temp1692.f2; goto _LL1691; _LL1691: _temp1690=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1620)->f4; goto
_LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int) _temp1620 > 1u?((
struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1709:
_temp1708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1620)->f1; goto _LL1707; _LL1707: _temp1706=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1620)->f2; goto _LL1643;} else{ goto
_LL1644;} _LL1644: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1713: _temp1712=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1620)->f1; if( _temp1712 == 0){ goto
_LL1711;} else{ goto _LL1646;} _LL1711: _temp1710=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1620)->f2; goto _LL1645;} else{ goto
_LL1646;} _LL1646: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1717: _temp1716=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1620)->f1; goto _LL1715; _LL1715:
_temp1714=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1620)->f2; goto _LL1647;} else{ goto _LL1648;} _LL1648: if(( unsigned int)
_temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1721: _temp1720=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1620)->f1; goto _LL1719; _LL1719: _temp1718=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1620)->f2; goto _LL1649;} else{ goto
_LL1650;} _LL1650: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1723: _temp1722=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1620)->f1; goto _LL1651;} else{ goto
_LL1652;} _LL1652: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1725: _temp1724=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1620)->f1; goto _LL1653;} else{ goto
_LL1654;} _LL1654: if(( unsigned int) _temp1620 > 1u?(( struct _tunion_struct*)
_temp1620)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1729: _temp1728=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1620)->f1; goto
_LL1727; _LL1727: _temp1726=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1620)->f2; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(( unsigned int)
_temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1737: _temp1736=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1620)->f1; goto _LL1731; _LL1731: _temp1730=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1620)->f2; _LL1735: _temp1734= _temp1730.f1; goto _LL1733; _LL1733:
_temp1732= _temp1730.f2; goto _LL1657;} else{ goto _LL1658;} _LL1658: if((
unsigned int) _temp1620 > 1u?(( struct _tunion_struct*) _temp1620)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1741: _temp1740=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1620)->f1; goto _LL1739; _LL1739: _temp1738=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1620)->f2; goto _LL1659;} else{ goto
_LL1621;} _LL1623: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1742=(
char*)";"; struct _tagged_string _temp1743; _temp1743.curr= _temp1742; _temp1743.base=
_temp1742; _temp1743.last_plus_one= _temp1742 + 2; _temp1743;})); goto _LL1621;
_LL1625: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1660), Cyc_PP_text((
struct _tagged_string)({ char* _temp1744=( char*)";"; struct _tagged_string
_temp1745; _temp1745.curr= _temp1744; _temp1745.base= _temp1744; _temp1745.last_plus_one=
_temp1744 + 2; _temp1745;}))); goto _LL1621; _LL1627: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1664)){ s= Cyc_Absynpp_cat6( Cyc_PP_text((
struct _tagged_string)({ char* _temp1746=( char*)"{ "; struct _tagged_string
_temp1747; _temp1747.curr= _temp1746; _temp1747.base= _temp1746; _temp1747.last_plus_one=
_temp1746 + 3; _temp1747;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1664)),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1748=( char*)"}";
struct _tagged_string _temp1749; _temp1749.curr= _temp1748; _temp1749.base=
_temp1748; _temp1749.last_plus_one= _temp1748 + 2; _temp1749;})), Cyc_PP_line_doc(),
Cyc_Absynpp_is_declaration( _temp1662)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1750=( char*)"{ "; struct _tagged_string _temp1751;
_temp1751.curr= _temp1750; _temp1751.base= _temp1750; _temp1751.last_plus_one=
_temp1750 + 3; _temp1751;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1662)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1752=( char*)"}"; struct
_tagged_string _temp1753; _temp1753.curr= _temp1752; _temp1753.base= _temp1752;
_temp1753.last_plus_one= _temp1752 + 2; _temp1753;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1662));} else{ if( Cyc_Absynpp_is_declaration( _temp1662)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1664), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1754=( char*)"{ "; struct _tagged_string _temp1755;
_temp1755.curr= _temp1754; _temp1755.base= _temp1754; _temp1755.last_plus_one=
_temp1754 + 3; _temp1755;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1662)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1756=( char*)"}"; struct
_tagged_string _temp1757; _temp1757.curr= _temp1756; _temp1757.base= _temp1756;
_temp1757.last_plus_one= _temp1756 + 2; _temp1757;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1758=( char*)""; struct _tagged_string
_temp1759; _temp1759.curr= _temp1758; _temp1759.base= _temp1758; _temp1759.last_plus_one=
_temp1758 + 1; _temp1759;}),({ struct Cyc_List_List* _temp1760=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1760->hd=( void*) _temp1664;
_temp1760->tl=({ struct Cyc_List_List* _temp1761=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1761->hd=( void*) _temp1662;
_temp1761->tl= 0; _temp1761;}); _temp1760;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,( struct
_tagged_string)({ char* _temp1762=( char*)""; struct _tagged_string _temp1763;
_temp1763.curr= _temp1762; _temp1763.base= _temp1762; _temp1763.last_plus_one=
_temp1762 + 1; _temp1763;}),({ struct Cyc_List_List* _temp1764=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1764->hd=( void*) _temp1664;
_temp1764->tl=({ struct Cyc_List_List* _temp1765=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1765->hd=( void*) _temp1662;
_temp1765->tl= 0; _temp1765;}); _temp1764;}));} goto _LL1621; _LL1629: if(
_temp1666 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1766=(
char*)"return;"; struct _tagged_string _temp1767; _temp1767.curr= _temp1766;
_temp1767.base= _temp1766; _temp1767.last_plus_one= _temp1766 + 8; _temp1767;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1768=( char*)"return "; struct _tagged_string _temp1769; _temp1769.curr=
_temp1768; _temp1769.base= _temp1768; _temp1769.last_plus_one= _temp1768 + 8;
_temp1769;})), _temp1666 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1770= _temp1666; if( _temp1770 == 0){
_throw( Null_Exception);} _temp1770;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1771=( char*)";"; struct _tagged_string _temp1772; _temp1772.curr=
_temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one= _temp1771 + 2;
_temp1772;})));} goto _LL1621; _LL1631: { int print_else;{ void* _temp1773=(
void*) _temp1668->r; _LL1775: if(( int) _temp1773 == Cyc_Absyn_Skip_s){ goto
_LL1776;} else{ goto _LL1777;} _LL1777: goto _LL1778; _LL1776: print_else= 0;
goto _LL1774; _LL1778: print_else= 1; goto _LL1774; _LL1774:;} s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1779=( char*)"if ("; struct
_tagged_string _temp1780; _temp1780.curr= _temp1779; _temp1780.base= _temp1779;
_temp1780.last_plus_one= _temp1779 + 5; _temp1780;})), Cyc_Absynpp_exp2doc(
_temp1672), Cyc_PP_text(( struct _tagged_string)({ char* _temp1781=( char*)") {";
struct _tagged_string _temp1782; _temp1782.curr= _temp1781; _temp1782.base=
_temp1781; _temp1782.last_plus_one= _temp1781 + 4; _temp1782;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1670))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1783=( char*)"}"; struct
_tagged_string _temp1784; _temp1784.curr= _temp1783; _temp1784.base= _temp1783;
_temp1784.last_plus_one= _temp1783 + 2; _temp1784;})), print_else? Cyc_Absynpp_cat5(
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1785=( char*)"else {";
struct _tagged_string _temp1786; _temp1786.curr= _temp1785; _temp1786.base=
_temp1785; _temp1786.last_plus_one= _temp1785 + 7; _temp1786;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1668))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1787=( char*)"}"; struct
_tagged_string _temp1788; _temp1788.curr= _temp1787; _temp1788.base= _temp1787;
_temp1788.last_plus_one= _temp1787 + 2; _temp1788;}))): Cyc_PP_nil_doc()); goto
_LL1621;} _LL1633: s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({
char* _temp1789=( char*)"while ("; struct _tagged_string _temp1790; _temp1790.curr=
_temp1789; _temp1790.base= _temp1789; _temp1790.last_plus_one= _temp1789 + 8;
_temp1790;})), Cyc_Absynpp_exp2doc( _temp1680), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1791=( char*)") {"; struct _tagged_string _temp1792;
_temp1792.curr= _temp1791; _temp1792.base= _temp1791; _temp1792.last_plus_one=
_temp1791 + 4; _temp1792;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1674))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1793=( char*)"}"; struct _tagged_string _temp1794;
_temp1794.curr= _temp1793; _temp1794.base= _temp1793; _temp1794.last_plus_one=
_temp1793 + 2; _temp1794;}))); goto _LL1621; _LL1635: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1795=( char*)"break;"; struct _tagged_string
_temp1796; _temp1796.curr= _temp1795; _temp1796.base= _temp1795; _temp1796.last_plus_one=
_temp1795 + 7; _temp1796;})); goto _LL1621; _LL1637: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1797=( char*)"continue;"; struct _tagged_string
_temp1798; _temp1798.curr= _temp1797; _temp1798.base= _temp1797; _temp1798.last_plus_one=
_temp1797 + 10; _temp1798;})); goto _LL1621; _LL1639: s= Cyc_PP_text(({ struct
_tagged_string _temp1799=* _temp1688; xprintf("goto %.*s;", _temp1799.last_plus_one
- _temp1799.curr, _temp1799.curr);})); goto _LL1621; _LL1641: s= Cyc_Absynpp_cat10(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1800=( char*)"for("; struct
_tagged_string _temp1801; _temp1801.curr= _temp1800; _temp1801.base= _temp1800;
_temp1801.last_plus_one= _temp1800 + 5; _temp1801;})), Cyc_Absynpp_exp2doc(
_temp1704), Cyc_PP_text(( struct _tagged_string)({ char* _temp1802=( char*)"; ";
struct _tagged_string _temp1803; _temp1803.curr= _temp1802; _temp1803.base=
_temp1802; _temp1803.last_plus_one= _temp1802 + 3; _temp1803;})), Cyc_Absynpp_exp2doc(
_temp1702), Cyc_PP_text(( struct _tagged_string)({ char* _temp1804=( char*)"; ";
struct _tagged_string _temp1805; _temp1805.curr= _temp1804; _temp1805.base=
_temp1804; _temp1805.last_plus_one= _temp1804 + 3; _temp1805;})), Cyc_Absynpp_exp2doc(
_temp1696), Cyc_PP_text(( struct _tagged_string)({ char* _temp1806=( char*)") {";
struct _tagged_string _temp1807; _temp1807.curr= _temp1806; _temp1807.base=
_temp1806; _temp1807.last_plus_one= _temp1806 + 4; _temp1807;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1690))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1808=( char*)"}"; struct
_tagged_string _temp1809; _temp1809.curr= _temp1808; _temp1809.base= _temp1808;
_temp1809.last_plus_one= _temp1808 + 2; _temp1809;}))); goto _LL1621; _LL1643: s=
Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char* _temp1810=( char*)"switch (";
struct _tagged_string _temp1811; _temp1811.curr= _temp1810; _temp1811.base=
_temp1810; _temp1811.last_plus_one= _temp1810 + 9; _temp1811;})), Cyc_Absynpp_exp2doc(
_temp1708), Cyc_PP_text(( struct _tagged_string)({ char* _temp1812=( char*)") {";
struct _tagged_string _temp1813; _temp1813.curr= _temp1812; _temp1813.base=
_temp1812; _temp1813.last_plus_one= _temp1812 + 4; _temp1813;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1706), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1814=( char*)"}"; struct _tagged_string
_temp1815; _temp1815.curr= _temp1814; _temp1815.base= _temp1814; _temp1815.last_plus_one=
_temp1814 + 2; _temp1815;}))); goto _LL1621; _LL1645: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1816=( char*)"fallthru;"; struct _tagged_string
_temp1817; _temp1817.curr= _temp1816; _temp1817.base= _temp1816; _temp1817.last_plus_one=
_temp1816 + 10; _temp1817;})); goto _LL1621; _LL1647: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1818=( char*)"fallthru("; struct
_tagged_string _temp1819; _temp1819.curr= _temp1818; _temp1819.base= _temp1818;
_temp1819.last_plus_one= _temp1818 + 10; _temp1819;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1716), Cyc_PP_text(( struct _tagged_string)({ char* _temp1820=( char*)");";
struct _tagged_string _temp1821; _temp1821.curr= _temp1820; _temp1821.base=
_temp1820; _temp1821.last_plus_one= _temp1820 + 3; _temp1821;}))); goto _LL1621;
_LL1649: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1720), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1718)); goto _LL1621; _LL1651: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1822=( char*)"cut "; struct
_tagged_string _temp1823; _temp1823.curr= _temp1822; _temp1823.base= _temp1822;
_temp1823.last_plus_one= _temp1822 + 5; _temp1823;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1722))); goto _LL1621; _LL1653: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1824=( char*)"splice "; struct _tagged_string
_temp1825; _temp1825.curr= _temp1824; _temp1825.base= _temp1824; _temp1825.last_plus_one=
_temp1824 + 8; _temp1825;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1724)));
goto _LL1621; _LL1655: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1726): 0){ s= Cyc_Absynpp_cat6( Cyc_Absynpp_textptr( _temp1728), Cyc_PP_text((
struct _tagged_string)({ char* _temp1826=( char*)": {"; struct _tagged_string
_temp1827; _temp1827.curr= _temp1826; _temp1827.base= _temp1826; _temp1827.last_plus_one=
_temp1826 + 4; _temp1827;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1726)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1828=( char*)"}"; struct _tagged_string _temp1829; _temp1829.curr=
_temp1828; _temp1829.base= _temp1828; _temp1829.last_plus_one= _temp1828 + 2;
_temp1829;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1728), Cyc_PP_text((
struct _tagged_string)({ char* _temp1830=( char*)": "; struct _tagged_string
_temp1831; _temp1831.curr= _temp1830; _temp1831.base= _temp1830; _temp1831.last_plus_one=
_temp1830 + 3; _temp1831;})), Cyc_Absynpp_stmt2doc( _temp1726));} goto _LL1621;
_LL1657: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1832=( char*)"do {"; struct _tagged_string _temp1833; _temp1833.curr=
_temp1832; _temp1833.base= _temp1832; _temp1833.last_plus_one= _temp1832 + 5;
_temp1833;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1736)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1834=( char*)"} while ("; struct _tagged_string _temp1835; _temp1835.curr=
_temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one= _temp1834 + 10;
_temp1835;})), Cyc_Absynpp_exp2doc( _temp1734), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1836=( char*)");"; struct _tagged_string _temp1837;
_temp1837.curr= _temp1836; _temp1837.base= _temp1836; _temp1837.last_plus_one=
_temp1836 + 3; _temp1837;}))); goto _LL1621; _LL1659: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1838=( char*)"try {"; struct _tagged_string
_temp1839; _temp1839.curr= _temp1838; _temp1839.base= _temp1838; _temp1839.last_plus_one=
_temp1838 + 6; _temp1839;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1740)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1840=( char*)"} catch {"; struct _tagged_string _temp1841; _temp1841.curr=
_temp1840; _temp1841.base= _temp1840; _temp1841.last_plus_one= _temp1840 + 10;
_temp1841;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1738)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1842=( char*)"}"; struct _tagged_string _temp1843; _temp1843.curr=
_temp1842; _temp1843.base= _temp1842; _temp1843.last_plus_one= _temp1842 + 2;
_temp1843;}))); goto _LL1621; _LL1621:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1844=( void*) p->r;
int _temp1878; void* _temp1880; char _temp1882; struct _tagged_string _temp1884;
struct Cyc_Absyn_Vardecl* _temp1886; struct Cyc_List_List* _temp1888; struct Cyc_Absyn_Pat*
_temp1890; struct Cyc_Absyn_Vardecl* _temp1892; struct _tuple0* _temp1894;
struct Cyc_List_List* _temp1896; struct Cyc_List_List* _temp1898; struct _tuple0*
_temp1900; struct Cyc_List_List* _temp1902; struct Cyc_List_List* _temp1904;
struct _tuple0* _temp1906; struct Cyc_List_List* _temp1908; struct Cyc_List_List*
_temp1910; struct Cyc_Core_Opt* _temp1912; struct Cyc_Absyn_Structdecl*
_temp1914; struct Cyc_Absyn_Enumfield* _temp1916; struct Cyc_Absyn_Enumdecl*
_temp1918; struct _tuple0* _temp1920; struct Cyc_Absyn_Tunionfield* _temp1922;
struct Cyc_Absyn_Tuniondecl* _temp1924; struct Cyc_List_List* _temp1926; struct
Cyc_List_List* _temp1928; struct Cyc_Core_Opt* _temp1930; struct _tuple0*
_temp1932; struct Cyc_Absyn_Tunionfield* _temp1934; struct Cyc_Absyn_XTuniondecl*
_temp1936; struct Cyc_List_List* _temp1938; struct Cyc_List_List* _temp1940;
struct _tuple0* _temp1942; _LL1846: if(( int) _temp1844 == Cyc_Absyn_Wild_p){
goto _LL1847;} else{ goto _LL1848;} _LL1848: if(( int) _temp1844 == Cyc_Absyn_Null_p){
goto _LL1849;} else{ goto _LL1850;} _LL1850: if(( unsigned int) _temp1844 > 2u?((
struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1881:
_temp1880=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1844)->f1; goto
_LL1879; _LL1879: _temp1878=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1844)->f2;
goto _LL1851;} else{ goto _LL1852;} _LL1852: if(( unsigned int) _temp1844 > 2u?((
struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1883:
_temp1882=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1844)->f1; goto
_LL1853;} else{ goto _LL1854;} _LL1854: if(( unsigned int) _temp1844 > 2u?((
struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1885:
_temp1884=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1844)->f1;
goto _LL1855;} else{ goto _LL1856;} _LL1856: if(( unsigned int) _temp1844 > 2u?((
struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1887:
_temp1886=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1844)->f1; goto _LL1857;} else{ goto _LL1858;} _LL1858: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1889: _temp1888=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1844)->f1; goto _LL1859;} else{ goto _LL1860;} _LL1860: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1891: _temp1890=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1844)->f1; goto _LL1861;} else{ goto _LL1862;} _LL1862: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1893: _temp1892=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1844)->f1; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1895: _temp1894=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1844)->f1; goto _LL1865;} else{ goto _LL1866;} _LL1866: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1901: _temp1900=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1844)->f1; goto _LL1899; _LL1899: _temp1898=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1844)->f2; goto _LL1897; _LL1897:
_temp1896=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1844)->f3; goto _LL1867;} else{ goto _LL1868;} _LL1868: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1907: _temp1906=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1844)->f1; goto _LL1905; _LL1905: _temp1904=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1844)->f2; goto _LL1903; _LL1903:
_temp1902=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1844)->f3; goto _LL1869;} else{ goto _LL1870;} _LL1870: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1915: _temp1914=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1844)->f1; goto _LL1913; _LL1913: _temp1912=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1844)->f2; goto _LL1911; _LL1911:
_temp1910=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1844)->f3; goto _LL1909; _LL1909: _temp1908=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1844)->f4; goto _LL1871;} else{ goto
_LL1872;} _LL1872: if(( unsigned int) _temp1844 > 2u?(( struct _tunion_struct*)
_temp1844)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1921: _temp1920=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1844)->f1; goto _LL1919; _LL1919:
_temp1918=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1844)->f2; goto _LL1917; _LL1917: _temp1916=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1844)->f3; goto _LL1873;} else{ goto
_LL1874;} _LL1874: if(( unsigned int) _temp1844 > 2u?(( struct _tunion_struct*)
_temp1844)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1933: _temp1932=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1844)->f1; goto _LL1931;
_LL1931: _temp1930=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1844)->f2; goto _LL1929; _LL1929: _temp1928=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1844)->f3; goto _LL1927; _LL1927:
_temp1926=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1844)->f4; goto _LL1925; _LL1925: _temp1924=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1844)->f5; goto _LL1923; _LL1923:
_temp1922=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1844)->f6; goto _LL1875;} else{ goto _LL1876;} _LL1876: if(( unsigned int)
_temp1844 > 2u?(( struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1943: _temp1942=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1844)->f1; goto _LL1941; _LL1941: _temp1940=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1844)->f2; goto _LL1939; _LL1939:
_temp1938=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1844)->f3; goto _LL1937; _LL1937: _temp1936=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1844)->f4; goto _LL1935; _LL1935:
_temp1934=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1844)->f5; goto _LL1877;} else{ goto _LL1845;} _LL1847: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp1944=( char*)"_"; struct _tagged_string
_temp1945; _temp1945.curr= _temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one=
_temp1944 + 2; _temp1945;})); goto _LL1845; _LL1849: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1946=( char*)"null"; struct _tagged_string
_temp1947; _temp1947.curr= _temp1946; _temp1947.base= _temp1946; _temp1947.last_plus_one=
_temp1946 + 5; _temp1947;})); goto _LL1845; _LL1851: if( _temp1880 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d", _temp1878));} else{ s= Cyc_PP_text(
xprintf("%u",( unsigned int) _temp1878));} goto _LL1845; _LL1853: s= Cyc_PP_text(({
struct _tagged_string _temp1948= Cyc_Absynpp_char_escape( _temp1882); xprintf("'%.*s'",
_temp1948.last_plus_one - _temp1948.curr, _temp1948.curr);})); goto _LL1845;
_LL1855: s= Cyc_PP_text( _temp1884); goto _LL1845; _LL1857: s= Cyc_Absynpp_qvar2doc(
_temp1886->name); goto _LL1845; _LL1859: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1949=( char*)"$("; struct _tagged_string
_temp1950; _temp1950.curr= _temp1949; _temp1950.base= _temp1949; _temp1950.last_plus_one=
_temp1949 + 3; _temp1950;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,( struct _tagged_string)({ char* _temp1951=(
char*)","; struct _tagged_string _temp1952; _temp1952.curr= _temp1951; _temp1952.base=
_temp1951; _temp1952.last_plus_one= _temp1951 + 2; _temp1952;}), _temp1888), Cyc_PP_text((
struct _tagged_string)({ char* _temp1953=( char*)")"; struct _tagged_string
_temp1954; _temp1954.curr= _temp1953; _temp1954.base= _temp1953; _temp1954.last_plus_one=
_temp1953 + 2; _temp1954;}))); goto _LL1845; _LL1861: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1955=( char*)"&"; struct _tagged_string
_temp1956; _temp1956.curr= _temp1955; _temp1956.base= _temp1955; _temp1956.last_plus_one=
_temp1955 + 2; _temp1956;})), Cyc_Absynpp_pat2doc( _temp1890)); goto _LL1845;
_LL1863: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1957=( char*)"*"; struct _tagged_string _temp1958; _temp1958.curr=
_temp1957; _temp1958.base= _temp1957; _temp1958.last_plus_one= _temp1957 + 2;
_temp1958;})), Cyc_Absynpp_qvar2doc( _temp1892->name)); goto _LL1845; _LL1865: s=
Cyc_Absynpp_qvar2doc( _temp1894); goto _LL1845; _LL1867: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1900), Cyc_Absynpp_tvars2doc( _temp1898), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1959=( char*)"("; struct _tagged_string
_temp1960; _temp1960.curr= _temp1959; _temp1960.base= _temp1959; _temp1960.last_plus_one=
_temp1959 + 2; _temp1960;}),( struct _tagged_string)({ char* _temp1961=( char*)")";
struct _tagged_string _temp1962; _temp1962.curr= _temp1961; _temp1962.base=
_temp1961; _temp1962.last_plus_one= _temp1961 + 2; _temp1962;}),( struct
_tagged_string)({ char* _temp1963=( char*)","; struct _tagged_string _temp1964;
_temp1964.curr= _temp1963; _temp1964.base= _temp1963; _temp1964.last_plus_one=
_temp1963 + 2; _temp1964;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1896))); goto _LL1845; _LL1869: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1906), Cyc_Absynpp_tvars2doc( _temp1904), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1965=( char*)"{"; struct _tagged_string _temp1966;
_temp1966.curr= _temp1965; _temp1966.base= _temp1965; _temp1966.last_plus_one=
_temp1965 + 2; _temp1966;}),( struct _tagged_string)({ char* _temp1967=( char*)"}";
struct _tagged_string _temp1968; _temp1968.curr= _temp1967; _temp1968.base=
_temp1967; _temp1968.last_plus_one= _temp1967 + 2; _temp1968;}),( struct
_tagged_string)({ char* _temp1969=( char*)","; struct _tagged_string _temp1970;
_temp1970.curr= _temp1969; _temp1970.base= _temp1969; _temp1970.last_plus_one=
_temp1969 + 2; _temp1970;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1902))); goto _LL1845; _LL1871: s= Cyc_Absynpp_cat3( _temp1914->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1914->name)->v),
Cyc_Absynpp_tvars2doc( _temp1910), Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1971=( char*)"{"; struct _tagged_string _temp1972; _temp1972.curr=
_temp1971; _temp1972.base= _temp1971; _temp1972.last_plus_one= _temp1971 + 2;
_temp1972;}),( struct _tagged_string)({ char* _temp1973=( char*)"}"; struct
_tagged_string _temp1974; _temp1974.curr= _temp1973; _temp1974.base= _temp1973;
_temp1974.last_plus_one= _temp1973 + 2; _temp1974;}),( struct _tagged_string)({
char* _temp1975=( char*)","; struct _tagged_string _temp1976; _temp1976.curr=
_temp1975; _temp1976.base= _temp1975; _temp1976.last_plus_one= _temp1975 + 2;
_temp1976;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1908))); goto _LL1845; _LL1873: s= Cyc_Absynpp_qvar2doc( _temp1920); goto
_LL1845; _LL1875: _temp1942= _temp1932; _temp1940= _temp1928; _temp1938=
_temp1926; goto _LL1877; _LL1877: if( _temp1938 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1942);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1942), Cyc_Absynpp_tvars2doc(
_temp1940), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1977=( char*)"(";
struct _tagged_string _temp1978; _temp1978.curr= _temp1977; _temp1978.base=
_temp1977; _temp1978.last_plus_one= _temp1977 + 2; _temp1978;}),( struct
_tagged_string)({ char* _temp1979=( char*)")"; struct _tagged_string _temp1980;
_temp1980.curr= _temp1979; _temp1980.base= _temp1979; _temp1980.last_plus_one=
_temp1979 + 2; _temp1980;}),( struct _tagged_string)({ char* _temp1981=( char*)",";
struct _tagged_string _temp1982; _temp1982.curr= _temp1981; _temp1982.base=
_temp1981; _temp1982.last_plus_one= _temp1981 + 2; _temp1982;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1938)));} goto _LL1845; _LL1845:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1983=( char*)""; struct
_tagged_string _temp1984; _temp1984.curr= _temp1983; _temp1984.base= _temp1983;
_temp1984.last_plus_one= _temp1983 + 1; _temp1984;}),( struct _tagged_string)({
char* _temp1985=( char*)"="; struct _tagged_string _temp1986; _temp1986.curr=
_temp1985; _temp1986.base= _temp1985; _temp1986.last_plus_one= _temp1985 + 2;
_temp1986;}),( struct _tagged_string)({ char* _temp1987=( char*)"="; struct
_tagged_string _temp1988; _temp1988.curr= _temp1987; _temp1988.base= _temp1987;
_temp1988.last_plus_one= _temp1987 + 2; _temp1988;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1989=( char*)"default: "; struct _tagged_string
_temp1990; _temp1990.curr= _temp1989; _temp1990.base= _temp1989; _temp1990.last_plus_one=
_temp1989 + 10; _temp1990;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1991=( char*)"case "; struct
_tagged_string _temp1992; _temp1992.curr= _temp1991; _temp1992.base= _temp1991;
_temp1992.last_plus_one= _temp1991 + 6; _temp1992;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1993=( char*)": "; struct
_tagged_string _temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993;
_temp1994.last_plus_one= _temp1993 + 3; _temp1994;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1995=( char*)"case "; struct
_tagged_string _temp1996; _temp1996.curr= _temp1995; _temp1996.base= _temp1995;
_temp1996.last_plus_one= _temp1995 + 6; _temp1996;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1997=( char*)" && "; struct
_tagged_string _temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997;
_temp1998.last_plus_one= _temp1997 + 5; _temp1998;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1999= c->where_clause; if(
_temp1999 == 0){ _throw( Null_Exception);} _temp1999;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2000=( char*)": "; struct _tagged_string _temp2001;
_temp2001.curr= _temp2000; _temp2001.base= _temp2000; _temp2001.last_plus_one=
_temp2000 + 3; _temp2001;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp2002=( char*)""; struct _tagged_string _temp2003; _temp2003.curr=
_temp2002; _temp2003.base= _temp2002; _temp2003.last_plus_one= _temp2002 + 1;
_temp2003;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield*
f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( f->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2004=( char*)" = "; struct _tagged_string _temp2005; _temp2005.curr=
_temp2004; _temp2005.base= _temp2004; _temp2005.last_plus_one= _temp2004 + 4;
_temp2005;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp2006= f->tag; if( _temp2006 == 0){ _throw( Null_Exception);} _temp2006;})));}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,(
struct _tagged_string)({ char* _temp2007=( char*)","; struct _tagged_string
_temp2008; _temp2008.curr= _temp2007; _temp2008.base= _temp2007; _temp2008.last_plus_one=
_temp2007 + 2; _temp2008;}), fs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2009=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2035; struct Cyc_Absyn_Structdecl* _temp2037;
struct Cyc_Absyn_Uniondecl* _temp2039; struct Cyc_Absyn_Vardecl* _temp2041;
struct Cyc_Absyn_Vardecl _temp2043; struct Cyc_List_List* _temp2044; struct Cyc_Core_Opt*
_temp2046; int _temp2048; struct Cyc_Absyn_Exp* _temp2050; void* _temp2052;
struct Cyc_Absyn_Tqual* _temp2054; struct _tuple0* _temp2056; void* _temp2058;
struct Cyc_Absyn_Tuniondecl* _temp2060; struct Cyc_Absyn_XTuniondecl* _temp2062;
int _temp2064; struct Cyc_Absyn_Exp* _temp2066; struct Cyc_Core_Opt* _temp2068;
struct Cyc_Core_Opt* _temp2070; struct Cyc_Absyn_Pat* _temp2072; struct Cyc_Absyn_Enumdecl*
_temp2074; struct Cyc_Absyn_Enumdecl _temp2076; struct Cyc_List_List* _temp2077;
struct _tuple0* _temp2079; void* _temp2081; struct Cyc_Absyn_Typedefdecl*
_temp2083; struct Cyc_List_List* _temp2085; struct _tagged_string* _temp2087;
struct Cyc_List_List* _temp2089; struct _tuple0* _temp2091; struct Cyc_List_List*
_temp2093; _LL2011: if((( struct _tunion_struct*) _temp2009)->tag == Cyc_Absyn_Fn_d_tag){
_LL2036: _temp2035=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp2009)->f1; goto _LL2012;} else{ goto _LL2013;} _LL2013: if((( struct
_tunion_struct*) _temp2009)->tag == Cyc_Absyn_Struct_d_tag){ _LL2038: _temp2037=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp2009)->f1;
goto _LL2014;} else{ goto _LL2015;} _LL2015: if((( struct _tunion_struct*)
_temp2009)->tag == Cyc_Absyn_Union_d_tag){ _LL2040: _temp2039=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp2009)->f1; goto _LL2016;} else{ goto
_LL2017;} _LL2017: if((( struct _tunion_struct*) _temp2009)->tag == Cyc_Absyn_Var_d_tag){
_LL2042: _temp2041=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2009)->f1; _temp2043=* _temp2041; _LL2059: _temp2058=( void*) _temp2043.sc;
goto _LL2057; _LL2057: _temp2056=( struct _tuple0*) _temp2043.name; goto _LL2055;
_LL2055: _temp2054=( struct Cyc_Absyn_Tqual*) _temp2043.tq; goto _LL2053;
_LL2053: _temp2052=( void*) _temp2043.type; goto _LL2051; _LL2051: _temp2050=(
struct Cyc_Absyn_Exp*) _temp2043.initializer; goto _LL2049; _LL2049: _temp2048=(
int) _temp2043.shadow; goto _LL2047; _LL2047: _temp2046=( struct Cyc_Core_Opt*)
_temp2043.region; goto _LL2045; _LL2045: _temp2044=( struct Cyc_List_List*)
_temp2043.attributes; goto _LL2018;} else{ goto _LL2019;} _LL2019: if((( struct
_tunion_struct*) _temp2009)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2061: _temp2060=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp2009)->f1;
goto _LL2020;} else{ goto _LL2021;} _LL2021: if((( struct _tunion_struct*)
_temp2009)->tag == Cyc_Absyn_XTunion_d_tag){ _LL2063: _temp2062=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp2009)->f1; goto _LL2022;} else{ goto
_LL2023;} _LL2023: if((( struct _tunion_struct*) _temp2009)->tag == Cyc_Absyn_Let_d_tag){
_LL2073: _temp2072=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2009)->f1; goto _LL2071; _LL2071: _temp2070=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2009)->f2; goto _LL2069; _LL2069: _temp2068=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2009)->f3; goto
_LL2067; _LL2067: _temp2066=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2009)->f4; goto _LL2065; _LL2065: _temp2064=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2009)->f5; goto _LL2024;} else{ goto _LL2025;} _LL2025: if((( struct
_tunion_struct*) _temp2009)->tag == Cyc_Absyn_Enum_d_tag){ _LL2075: _temp2074=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2009)->f1;
_temp2076=* _temp2074; _LL2082: _temp2081=( void*) _temp2076.sc; goto _LL2080;
_LL2080: _temp2079=( struct _tuple0*) _temp2076.name; goto _LL2078; _LL2078:
_temp2077=( struct Cyc_List_List*) _temp2076.fields; goto _LL2026;} else{ goto
_LL2027;} _LL2027: if((( struct _tunion_struct*) _temp2009)->tag == Cyc_Absyn_Typedef_d_tag){
_LL2084: _temp2083=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2009)->f1; goto _LL2028;} else{ goto _LL2029;} _LL2029: if((( struct
_tunion_struct*) _temp2009)->tag == Cyc_Absyn_Namespace_d_tag){ _LL2088:
_temp2087=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2009)->f1; goto _LL2086; _LL2086: _temp2085=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2009)->f2; goto _LL2030;} else{ goto
_LL2031;} _LL2031: if((( struct _tunion_struct*) _temp2009)->tag == Cyc_Absyn_Using_d_tag){
_LL2092: _temp2091=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2009)->f1; goto _LL2090; _LL2090: _temp2089=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2009)->f2; goto _LL2032;} else{ goto
_LL2033;} _LL2033: if((( struct _tunion_struct*) _temp2009)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2094: _temp2093=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2009)->f1; goto _LL2034;} else{ goto _LL2010;} _LL2012: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2095=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2095[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2096; _temp2096.tag= Cyc_Absyn_FnType_tag; _temp2096.f1=({ struct Cyc_Absyn_FnInfo
_temp2097; _temp2097.tvars= _temp2035->tvs; _temp2097.effect= _temp2035->effect;
_temp2097.ret_typ=( void*)(( void*) _temp2035->ret_type); _temp2097.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2035->args); _temp2097.varargs=
_temp2035->varargs; _temp2097.attributes= 0; _temp2097;}); _temp2096;});
_temp2095;}); s= Cyc_Absynpp_cat7( Cyc_PP_text( _temp2035->is_inline?( struct
_tagged_string)({ char* _temp2098=( char*)"inline "; struct _tagged_string
_temp2099; _temp2099.curr= _temp2098; _temp2099.base= _temp2098; _temp2099.last_plus_one=
_temp2098 + 8; _temp2099;}):( struct _tagged_string)({ char* _temp2100=( char*)"";
struct _tagged_string _temp2101; _temp2101.curr= _temp2100; _temp2101.base=
_temp2100; _temp2101.last_plus_one= _temp2100 + 1; _temp2101;})), Cyc_Absynpp_scope2doc((
void*) _temp2035->sc), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2102=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2102->q_const= 0; _temp2102->q_volatile= 0; _temp2102->q_restrict= 0;
_temp2102;}), t,({ struct Cyc_Core_Opt* _temp2103=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2103->v=( void*) Cyc_Absynpp_cat2(
Cyc_Absynpp_atts2doc( _temp2035->attributes), Cyc_Absynpp_typedef_name2doc(
_temp2035->name)); _temp2103;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2104=( char*)" {"; struct _tagged_string _temp2105; _temp2105.curr=
_temp2104; _temp2105.base= _temp2104; _temp2105.last_plus_one= _temp2104 + 3;
_temp2105;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc(
_temp2035->body))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2106=( char*)"}"; struct _tagged_string _temp2107; _temp2107.curr=
_temp2106; _temp2107.base= _temp2106; _temp2107.last_plus_one= _temp2106 + 2;
_temp2107;}))); goto _LL2010;} _LL2014: if( _temp2037->fields == 0){ s= Cyc_Absynpp_cat5(
Cyc_Absynpp_scope2doc(( void*) _temp2037->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2108=( char*)"struct "; struct _tagged_string
_temp2109; _temp2109.curr= _temp2108; _temp2109.base= _temp2108; _temp2109.last_plus_one=
_temp2108 + 8; _temp2109;})), _temp2037->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2110=( char*)""; struct _tagged_string _temp2111;
_temp2111.curr= _temp2110; _temp2111.base= _temp2110; _temp2111.last_plus_one=
_temp2110 + 1; _temp2111;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(
_temp2037->name)->v), Cyc_Absynpp_ktvars2doc( _temp2037->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2112=( char*)";"; struct _tagged_string
_temp2113; _temp2113.curr= _temp2112; _temp2113.base= _temp2112; _temp2113.last_plus_one=
_temp2112 + 2; _temp2113;})));} else{ s= Cyc_Absynpp_cat10( Cyc_Absynpp_scope2doc((
void*) _temp2037->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2114=(
char*)"struct "; struct _tagged_string _temp2115; _temp2115.curr= _temp2114;
_temp2115.base= _temp2114; _temp2115.last_plus_one= _temp2114 + 8; _temp2115;})),
_temp2037->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2116=(
char*)""; struct _tagged_string _temp2117; _temp2117.curr= _temp2116; _temp2117.base=
_temp2116; _temp2117.last_plus_one= _temp2116 + 1; _temp2117;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)( _temp2037->name)->v), Cyc_Absynpp_ktvars2doc( _temp2037->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2118=( char*)" {"; struct
_tagged_string _temp2119; _temp2119.curr= _temp2118; _temp2119.base= _temp2118;
_temp2119.last_plus_one= _temp2118 + 3; _temp2119;})), Cyc_PP_nest( 2, _temp2037->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2120=( char*)""; struct
_tagged_string _temp2121; _temp2121.curr= _temp2120; _temp2121.base= _temp2120;
_temp2121.last_plus_one= _temp2120 + 1; _temp2121;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)( _temp2037->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2122=( char*)"}";
struct _tagged_string _temp2123; _temp2123.curr= _temp2122; _temp2123.base=
_temp2122; _temp2123.last_plus_one= _temp2122 + 2; _temp2123;})), Cyc_Absynpp_atts2doc(
_temp2037->attributes), Cyc_PP_text(( struct _tagged_string)({ char* _temp2124=(
char*)";"; struct _tagged_string _temp2125; _temp2125.curr= _temp2124; _temp2125.base=
_temp2124; _temp2125.last_plus_one= _temp2124 + 2; _temp2125;})));} goto _LL2010;
_LL2016: if( _temp2039->fields == 0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc((
void*) _temp2039->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2126=(
char*)"union "; struct _tagged_string _temp2127; _temp2127.curr= _temp2126;
_temp2127.base= _temp2126; _temp2127.last_plus_one= _temp2126 + 7; _temp2127;})),
_temp2039->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2128=(
char*)""; struct _tagged_string _temp2129; _temp2129.curr= _temp2128; _temp2129.base=
_temp2128; _temp2129.last_plus_one= _temp2128 + 1; _temp2129;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp2039->name)->v), Cyc_Absynpp_tvars2doc( _temp2039->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2130=( char*)";"; struct
_tagged_string _temp2131; _temp2131.curr= _temp2130; _temp2131.base= _temp2130;
_temp2131.last_plus_one= _temp2130 + 2; _temp2131;})));} else{ s= Cyc_Absynpp_cat10(
Cyc_Absynpp_scope2doc(( void*) _temp2039->sc), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2132=( char*)"union "; struct _tagged_string
_temp2133; _temp2133.curr= _temp2132; _temp2133.base= _temp2132; _temp2133.last_plus_one=
_temp2132 + 7; _temp2133;})), _temp2039->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2134=( char*)""; struct _tagged_string _temp2135;
_temp2135.curr= _temp2134; _temp2135.base= _temp2134; _temp2135.last_plus_one=
_temp2134 + 1; _temp2135;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp2039->name)->v),
Cyc_Absynpp_tvars2doc( _temp2039->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp2136=( char*)" {"; struct _tagged_string _temp2137; _temp2137.curr=
_temp2136; _temp2137.base= _temp2136; _temp2137.last_plus_one= _temp2136 + 3;
_temp2137;})), Cyc_PP_nest( 2, _temp2039->fields == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp2138=( char*)""; struct _tagged_string _temp2139;
_temp2139.curr= _temp2138; _temp2139.base= _temp2138; _temp2139.last_plus_one=
_temp2138 + 1; _temp2139;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)( _temp2039->fields)->v))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp2140=( char*)"}"; struct _tagged_string
_temp2141; _temp2141.curr= _temp2140; _temp2141.base= _temp2140; _temp2141.last_plus_one=
_temp2140 + 2; _temp2141;})), Cyc_Absynpp_atts2doc( _temp2039->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2142=( char*)";"; struct _tagged_string
_temp2143; _temp2143.curr= _temp2142; _temp2143.base= _temp2142; _temp2143.last_plus_one=
_temp2142 + 2; _temp2143;})));} goto _LL2010; _LL2018: { struct Cyc_PP_Doc* sn=
Cyc_Absynpp_typedef_name2doc( _temp2056); s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(
_temp2058), Cyc_Absynpp_tqtd2doc( _temp2054, _temp2052,({ struct Cyc_Core_Opt*
_temp2144=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2144->v=( void*) sn; _temp2144;})), Cyc_Absynpp_atts2doc( _temp2044),
_temp2050 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2145=( char*)"";
struct _tagged_string _temp2146; _temp2146.curr= _temp2145; _temp2146.base=
_temp2145; _temp2146.last_plus_one= _temp2145 + 1; _temp2146;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2147=( char*)" = "; struct
_tagged_string _temp2148; _temp2148.curr= _temp2147; _temp2148.base= _temp2147;
_temp2148.last_plus_one= _temp2147 + 4; _temp2148;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2149= _temp2050; if(
_temp2149 == 0){ _throw( Null_Exception);} _temp2149;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2150=( char*)";"; struct _tagged_string _temp2151;
_temp2151.curr= _temp2150; _temp2151.base= _temp2150; _temp2151.last_plus_one=
_temp2150 + 2; _temp2151;}))); goto _LL2010;} _LL2020: if( _temp2060->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp2060->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2152=( char*)"tunion "; struct
_tagged_string _temp2153; _temp2153.curr= _temp2152; _temp2153.base= _temp2152;
_temp2153.last_plus_one= _temp2152 + 8; _temp2153;})), _temp2060->name == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp2154=( char*)""; struct _tagged_string
_temp2155; _temp2155.curr= _temp2154; _temp2155.base= _temp2154; _temp2155.last_plus_one=
_temp2154 + 1; _temp2155;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(
_temp2060->name)->v), Cyc_Absynpp_ktvars2doc( _temp2060->tvs), Cyc_PP_text((
struct _tagged_string)({ char* _temp2156=( char*)";"; struct _tagged_string
_temp2157; _temp2157.curr= _temp2156; _temp2157.base= _temp2156; _temp2157.last_plus_one=
_temp2156 + 2; _temp2157;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc((
void*) _temp2060->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp2158=(
char*)"tunion "; struct _tagged_string _temp2159; _temp2159.curr= _temp2158;
_temp2159.base= _temp2158; _temp2159.last_plus_one= _temp2158 + 8; _temp2159;})),
_temp2060->name == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2160=(
char*)""; struct _tagged_string _temp2161; _temp2161.curr= _temp2160; _temp2161.base=
_temp2160; _temp2161.last_plus_one= _temp2160 + 1; _temp2161;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)( _temp2060->name)->v), Cyc_Absynpp_ktvars2doc( _temp2060->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2162=( char*)" {"; struct
_tagged_string _temp2163; _temp2163.curr= _temp2162; _temp2163.base= _temp2162;
_temp2163.last_plus_one= _temp2162 + 3; _temp2163;})), Cyc_PP_nest( 2, _temp2060->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp2164=( char*)""; struct
_tagged_string _temp2165; _temp2165.curr= _temp2164; _temp2165.base= _temp2164;
_temp2165.last_plus_one= _temp2164 + 1; _temp2165;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)( _temp2060->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2166=( char*)"};";
struct _tagged_string _temp2167; _temp2167.curr= _temp2166; _temp2167.base=
_temp2166; _temp2167.last_plus_one= _temp2166 + 3; _temp2167;})));} goto _LL2010;
_LL2022: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) _temp2062->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp2168=( char*)"xtunion "; struct
_tagged_string _temp2169; _temp2169.curr= _temp2168; _temp2169.base= _temp2168;
_temp2169.last_plus_one= _temp2168 + 9; _temp2169;})), Cyc_Absynpp_typedef_name2doc(
_temp2062->name), _temp2062->fields == 0? Cyc_PP_text(( struct _tagged_string)({
char* _temp2170=( char*)";"; struct _tagged_string _temp2171; _temp2171.curr=
_temp2170; _temp2171.base= _temp2170; _temp2171.last_plus_one= _temp2170 + 2;
_temp2171;})): Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2172=( char*)" {"; struct _tagged_string _temp2173; _temp2173.curr=
_temp2172; _temp2173.base= _temp2172; _temp2173.last_plus_one= _temp2172 + 3;
_temp2173;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_tunionfields2doc(
_temp2062->fields))), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2174=( char*)"};"; struct _tagged_string _temp2175; _temp2175.curr=
_temp2174; _temp2175.base= _temp2174; _temp2175.last_plus_one= _temp2174 + 3;
_temp2175;})))); goto _LL2010; _LL2024: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp2176=( char*)"let "; struct _tagged_string
_temp2177; _temp2177.curr= _temp2176; _temp2177.base= _temp2176; _temp2177.last_plus_one=
_temp2176 + 5; _temp2177;})), Cyc_Absynpp_pat2doc( _temp2072), Cyc_PP_text((
struct _tagged_string)({ char* _temp2178=( char*)" = "; struct _tagged_string
_temp2179; _temp2179.curr= _temp2178; _temp2179.base= _temp2178; _temp2179.last_plus_one=
_temp2178 + 4; _temp2179;})), Cyc_Absynpp_exp2doc( _temp2066), Cyc_PP_text((
struct _tagged_string)({ char* _temp2180=( char*)";"; struct _tagged_string
_temp2181; _temp2181.curr= _temp2180; _temp2181.base= _temp2180; _temp2181.last_plus_one=
_temp2180 + 2; _temp2181;}))); goto _LL2010; _LL2026: s= Cyc_Absynpp_cat7( Cyc_Absynpp_scope2doc(
_temp2081), Cyc_PP_text(( struct _tagged_string)({ char* _temp2182=( char*)"enum ";
struct _tagged_string _temp2183; _temp2183.curr= _temp2182; _temp2183.base=
_temp2182; _temp2183.last_plus_one= _temp2182 + 6; _temp2183;})), Cyc_Absynpp_qvar2doc(
_temp2079), Cyc_PP_text(( struct _tagged_string)({ char* _temp2184=( char*)" {";
struct _tagged_string _temp2185; _temp2185.curr= _temp2184; _temp2185.base=
_temp2184; _temp2185.last_plus_one= _temp2184 + 3; _temp2185;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_enumfields2doc( _temp2077))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp2186=( char*)"};";
struct _tagged_string _temp2187; _temp2187.curr= _temp2186; _temp2187.base=
_temp2186; _temp2187.last_plus_one= _temp2186 + 3; _temp2187;}))); goto _LL2010;
_LL2028: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2188=( char*)"typedef "; struct _tagged_string _temp2189; _temp2189.curr=
_temp2188; _temp2189.base= _temp2188; _temp2189.last_plus_one= _temp2188 + 9;
_temp2189;})), Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual* _temp2190=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2190->q_const= 0; _temp2190->q_volatile= 0; _temp2190->q_restrict= 0;
_temp2190;}),( void*) _temp2083->defn,({ struct Cyc_Core_Opt* _temp2191=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2191->v=( void*)
Cyc_Absynpp_cat2( Cyc_Absynpp_typedef_name2doc( _temp2083->name), Cyc_Absynpp_tvars2doc(
_temp2083->tvs)); _temp2191;})), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2192=( char*)";"; struct _tagged_string _temp2193; _temp2193.curr=
_temp2192; _temp2193.base= _temp2192; _temp2193.last_plus_one= _temp2192 + 2;
_temp2193;}))); goto _LL2010; _LL2030: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2087);} s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp2194=( char*)"namespace "; struct _tagged_string _temp2195; _temp2195.curr=
_temp2194; _temp2195.base= _temp2194; _temp2195.last_plus_one= _temp2194 + 11;
_temp2195;})), Cyc_Absynpp_textptr( _temp2087), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2196=( char*)" {"; struct _tagged_string _temp2197;
_temp2197.curr= _temp2196; _temp2197.base= _temp2196; _temp2197.last_plus_one=
_temp2196 + 3; _temp2197;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2198=( char*)""; struct _tagged_string _temp2199; _temp2199.curr=
_temp2198; _temp2199.base= _temp2198; _temp2199.last_plus_one= _temp2198 + 1;
_temp2199;}), _temp2085), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2200=( char*)"}"; struct _tagged_string _temp2201; _temp2201.curr=
_temp2200; _temp2201.base= _temp2200; _temp2201.last_plus_one= _temp2200 + 2;
_temp2201;}))); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2010; _LL2032: if( Cyc_Absynpp_print_using_stmts){ s= Cyc_Absynpp_cat7(
Cyc_PP_text(( struct _tagged_string)({ char* _temp2202=( char*)"using "; struct
_tagged_string _temp2203; _temp2203.curr= _temp2202; _temp2203.base= _temp2202;
_temp2203.last_plus_one= _temp2202 + 7; _temp2203;})), Cyc_Absynpp_qvar2doc(
_temp2091), Cyc_PP_text(( struct _tagged_string)({ char* _temp2204=( char*)" {";
struct _tagged_string _temp2205; _temp2205.curr= _temp2204; _temp2205.base=
_temp2204; _temp2205.last_plus_one= _temp2204 + 3; _temp2205;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2206=( char*)""; struct _tagged_string
_temp2207; _temp2207.curr= _temp2206; _temp2207.base= _temp2206; _temp2207.last_plus_one=
_temp2206 + 1; _temp2207;}), _temp2089), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2208=( char*)"}"; struct _tagged_string _temp2209;
_temp2209.curr= _temp2208; _temp2209.base= _temp2208; _temp2209.last_plus_one=
_temp2208 + 2; _temp2209;})));} else{ s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2210=( char*)"/* using "; struct _tagged_string
_temp2211; _temp2211.curr= _temp2210; _temp2211.base= _temp2210; _temp2211.last_plus_one=
_temp2210 + 10; _temp2211;})), Cyc_Absynpp_qvar2doc( _temp2091), Cyc_PP_text((
struct _tagged_string)({ char* _temp2212=( char*)" { */"; struct _tagged_string
_temp2213; _temp2213.curr= _temp2212; _temp2213.base= _temp2212; _temp2213.last_plus_one=
_temp2212 + 6; _temp2213;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2214=( char*)""; struct _tagged_string _temp2215; _temp2215.curr=
_temp2214; _temp2215.base= _temp2214; _temp2215.last_plus_one= _temp2214 + 1;
_temp2215;}), _temp2089), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2216=( char*)"/* } */"; struct _tagged_string _temp2217; _temp2217.curr=
_temp2216; _temp2217.base= _temp2216; _temp2217.last_plus_one= _temp2216 + 8;
_temp2217;})));} goto _LL2010; _LL2034: if( Cyc_Absynpp_print_externC_stmts){ s=
Cyc_Absynpp_cat5( Cyc_PP_text(( struct _tagged_string)({ char* _temp2218=( char*)"extern \"C\" {";
struct _tagged_string _temp2219; _temp2219.curr= _temp2218; _temp2219.base=
_temp2218; _temp2219.last_plus_one= _temp2218 + 13; _temp2219;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2220=( char*)""; struct _tagged_string
_temp2221; _temp2221.curr= _temp2220; _temp2221.base= _temp2220; _temp2221.last_plus_one=
_temp2220 + 1; _temp2221;}), _temp2093), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2222=( char*)"}"; struct _tagged_string _temp2223;
_temp2223.curr= _temp2222; _temp2223.base= _temp2222; _temp2223.last_plus_one=
_temp2222 + 2; _temp2223;})));} else{ s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2224=( char*)"/* extern \"C\" { */"; struct
_tagged_string _temp2225; _temp2225.curr= _temp2224; _temp2225.base= _temp2224;
_temp2225.last_plus_one= _temp2224 + 19; _temp2225;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2226=( char*)""; struct _tagged_string
_temp2227; _temp2227.curr= _temp2226; _temp2227.base= _temp2226; _temp2227.last_plus_one=
_temp2226 + 1; _temp2227;}), _temp2093), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2228=( char*)"/* } */"; struct _tagged_string
_temp2229; _temp2229.curr= _temp2228; _temp2229.base= _temp2228; _temp2229.last_plus_one=
_temp2228 + 8; _temp2229;})));} goto _LL2010; _LL2010:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2230= sc; _LL2232: if(( int)
_temp2230 == Cyc_Absyn_Static){ goto _LL2233;} else{ goto _LL2234;} _LL2234: if((
int) _temp2230 == Cyc_Absyn_Public){ goto _LL2235;} else{ goto _LL2236;} _LL2236:
if(( int) _temp2230 == Cyc_Absyn_Extern){ goto _LL2237;} else{ goto _LL2238;}
_LL2238: if(( int) _temp2230 == Cyc_Absyn_ExternC){ goto _LL2239;} else{ goto
_LL2240;} _LL2240: if(( int) _temp2230 == Cyc_Absyn_Abstract){ goto _LL2241;}
else{ goto _LL2231;} _LL2233: return Cyc_PP_text(( struct _tagged_string)({ char*
_temp2242=( char*)"static "; struct _tagged_string _temp2243; _temp2243.curr=
_temp2242; _temp2243.base= _temp2242; _temp2243.last_plus_one= _temp2242 + 8;
_temp2243;})); _LL2235: return Cyc_PP_nil_doc(); _LL2237: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2244=( char*)"extern "; struct
_tagged_string _temp2245; _temp2245.curr= _temp2244; _temp2245.base= _temp2244;
_temp2245.last_plus_one= _temp2244 + 8; _temp2245;})); _LL2239: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2246=( char*)"extern \"C\" "; struct
_tagged_string _temp2247; _temp2247.curr= _temp2246; _temp2247.base= _temp2246;
_temp2247.last_plus_one= _temp2246 + 12; _temp2247;})); _LL2241: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2248=( char*)"abstract "; struct
_tagged_string _temp2249; _temp2249.curr= _temp2248; _temp2249.base= _temp2248;
_temp2249.last_plus_one= _temp2248 + 10; _temp2249;})); _LL2231:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp2250= t; struct Cyc_Absyn_Tvar* _temp2258; struct Cyc_List_List*
_temp2260; _LL2252: if(( unsigned int) _temp2250 > 4u?(( struct _tunion_struct*)
_temp2250)->tag == Cyc_Absyn_VarType_tag: 0){ _LL2259: _temp2258=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp2250)->f1; goto _LL2253;} else{ goto
_LL2254;} _LL2254: if(( unsigned int) _temp2250 > 4u?(( struct _tunion_struct*)
_temp2250)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL2261: _temp2260=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2250)->f1; goto _LL2255;} else{ goto
_LL2256;} _LL2256: goto _LL2257; _LL2253: return Cyc_Tcutil_is_temp_tvar(
_temp2258); _LL2255: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2260);
_LL2257: return 0; _LL2251:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual*
tq, void* t){ void* _temp2262= t; struct Cyc_Absyn_Exp* _temp2276; struct Cyc_Absyn_Tqual*
_temp2278; void* _temp2280; struct Cyc_Absyn_PtrInfo _temp2282; struct Cyc_Absyn_Conref*
_temp2284; struct Cyc_Absyn_Tqual* _temp2286; struct Cyc_Absyn_Conref* _temp2288;
void* _temp2290; void* _temp2292; struct Cyc_Absyn_FnInfo _temp2294; struct Cyc_List_List*
_temp2296; int _temp2298; struct Cyc_List_List* _temp2300; void* _temp2302;
struct Cyc_Core_Opt* _temp2304; struct Cyc_List_List* _temp2306; int _temp2308;
struct Cyc_Core_Opt* _temp2310; void* _temp2312; struct Cyc_Core_Opt* _temp2314;
struct Cyc_List_List* _temp2316; struct _tuple0* _temp2318; _LL2264: if((
unsigned int) _temp2262 > 4u?(( struct _tunion_struct*) _temp2262)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL2281: _temp2280=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2262)->f1;
goto _LL2279; _LL2279: _temp2278=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2262)->f2; goto _LL2277; _LL2277: _temp2276=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2262)->f3; goto _LL2265;} else{ goto
_LL2266;} _LL2266: if(( unsigned int) _temp2262 > 4u?(( struct _tunion_struct*)
_temp2262)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2283: _temp2282=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2262)->f1;
_LL2293: _temp2292=( void*) _temp2282.elt_typ; goto _LL2291; _LL2291: _temp2290=(
void*) _temp2282.rgn_typ; goto _LL2289; _LL2289: _temp2288=( struct Cyc_Absyn_Conref*)
_temp2282.nullable; goto _LL2287; _LL2287: _temp2286=( struct Cyc_Absyn_Tqual*)
_temp2282.tq; goto _LL2285; _LL2285: _temp2284=( struct Cyc_Absyn_Conref*)
_temp2282.bounds; goto _LL2267;} else{ goto _LL2268;} _LL2268: if(( unsigned int)
_temp2262 > 4u?(( struct _tunion_struct*) _temp2262)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2295: _temp2294=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2262)->f1; _LL2307: _temp2306=( struct Cyc_List_List*) _temp2294.tvars;
goto _LL2305; _LL2305: _temp2304=( struct Cyc_Core_Opt*) _temp2294.effect; goto
_LL2303; _LL2303: _temp2302=( void*) _temp2294.ret_typ; goto _LL2301; _LL2301:
_temp2300=( struct Cyc_List_List*) _temp2294.args; goto _LL2299; _LL2299:
_temp2298=( int) _temp2294.varargs; goto _LL2297; _LL2297: _temp2296=( struct
Cyc_List_List*) _temp2294.attributes; goto _LL2269;} else{ goto _LL2270;}
_LL2270: if(( unsigned int) _temp2262 > 4u?(( struct _tunion_struct*) _temp2262)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL2313: _temp2312=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2262)->f1; goto _LL2311; _LL2311: _temp2310=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2262)->f2; goto _LL2309; _LL2309: _temp2308=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2262)->f3; goto _LL2271;} else{ goto
_LL2272;} _LL2272: if(( unsigned int) _temp2262 > 4u?(( struct _tunion_struct*)
_temp2262)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2319: _temp2318=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2262)->f1; goto _LL2317;
_LL2317: _temp2316=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2262)->f2; goto _LL2315; _LL2315: _temp2314=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2262)->f3; goto _LL2273;} else{ goto
_LL2274;} _LL2274: goto _LL2275; _LL2265: { struct Cyc_List_List* _temp2322;
void* _temp2324; struct Cyc_Absyn_Tqual* _temp2326; struct _tuple4 _temp2320=
Cyc_Absynpp_to_tms( _temp2278, _temp2280); _LL2327: _temp2326= _temp2320.f1;
goto _LL2325; _LL2325: _temp2324= _temp2320.f2; goto _LL2323; _LL2323: _temp2322=
_temp2320.f3; goto _LL2321; _LL2321: { void* tm; if( _temp2276 == 0){ tm=( void*)
Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2328=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2328[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2329; _temp2329.tag= Cyc_Absyn_ConstArray_mod_tag; _temp2329.f1=( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2330= _temp2276; if( _temp2330 == 0){
_throw( Null_Exception);} _temp2330;}); _temp2329;}); _temp2328;});} return({
struct _tuple4 _temp2331; _temp2331.f1= _temp2326; _temp2331.f2= _temp2324;
_temp2331.f3=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2332=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2332->hd=( void*)
tm; _temp2332->tl= _temp2322; _temp2332;}); _temp2331;});}} _LL2267: { struct
Cyc_List_List* _temp2335; void* _temp2337; struct Cyc_Absyn_Tqual* _temp2339;
struct _tuple4 _temp2333= Cyc_Absynpp_to_tms( _temp2286, _temp2292); _LL2340:
_temp2339= _temp2333.f1; goto _LL2338; _LL2338: _temp2337= _temp2333.f2; goto
_LL2336; _LL2336: _temp2335= _temp2333.f3; goto _LL2334; _LL2334: { void* ps;{
struct _tuple8 _temp2342=({ struct _tuple8 _temp2341; _temp2341.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2288))->v; _temp2341.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2284))->v; _temp2341;}); void* _temp2352;
void* _temp2354; struct Cyc_Absyn_Exp* _temp2356; void* _temp2358; int _temp2360;
void* _temp2362; void* _temp2364; struct Cyc_Absyn_Exp* _temp2366; void*
_temp2368; int _temp2370; void* _temp2372; void* _temp2374; void* _temp2376;
_LL2344: _LL2359: _temp2358= _temp2342.f1; if(( unsigned int) _temp2358 > 1u?((
struct _tunion_struct*) _temp2358)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2361:
_temp2360=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2358)->f1; if(
_temp2360 == 1){ goto _LL2353;} else{ goto _LL2346;}} else{ goto _LL2346;}
_LL2353: _temp2352= _temp2342.f2; if(( unsigned int) _temp2352 > 1u?(( struct
_tunion_struct*) _temp2352)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2355:
_temp2354=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2352)->f1; if((
unsigned int) _temp2354 > 1u?(( struct _tunion_struct*) _temp2354)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2357: _temp2356=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2354)->f1; goto _LL2345;} else{ goto _LL2346;}} else{ goto _LL2346;}
_LL2346: _LL2369: _temp2368= _temp2342.f1; if(( unsigned int) _temp2368 > 1u?((
struct _tunion_struct*) _temp2368)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2371:
_temp2370=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2368)->f1; if(
_temp2370 == 0){ goto _LL2363;} else{ goto _LL2348;}} else{ goto _LL2348;}
_LL2363: _temp2362= _temp2342.f2; if(( unsigned int) _temp2362 > 1u?(( struct
_tunion_struct*) _temp2362)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2365:
_temp2364=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2362)->f1; if((
unsigned int) _temp2364 > 1u?(( struct _tunion_struct*) _temp2364)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2367: _temp2366=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2364)->f1; goto _LL2347;} else{ goto _LL2348;}} else{ goto _LL2348;}
_LL2348: _LL2377: _temp2376= _temp2342.f1; goto _LL2373; _LL2373: _temp2372=
_temp2342.f2; if(( unsigned int) _temp2372 > 1u?(( struct _tunion_struct*)
_temp2372)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2375: _temp2374=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2372)->f1; if(( int) _temp2374 == Cyc_Absyn_Unknown_b){
goto _LL2349;} else{ goto _LL2350;}} else{ goto _LL2350;} _LL2350: goto _LL2351;
_LL2345: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2378=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2378[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2379; _temp2379.tag=
Cyc_Absyn_Nullable_ps_tag; _temp2379.f1= _temp2356; _temp2379;}); _temp2378;});
goto _LL2343; _LL2347: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2380=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2380[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2381; _temp2381.tag= Cyc_Absyn_NonNullable_ps_tag; _temp2381.f1= _temp2366;
_temp2381;}); _temp2380;}); goto _LL2343; _LL2349: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2343; _LL2351: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2343;
_LL2343:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2382=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2382[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2383; _temp2383.tag=
Cyc_Absyn_Pointer_mod_tag; _temp2383.f1=( void*) ps; _temp2383.f2=( void*)
_temp2290; _temp2383.f3= tq; _temp2383;}); _temp2382;}); return({ struct _tuple4
_temp2384; _temp2384.f1= _temp2339; _temp2384.f2= _temp2337; _temp2384.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2385=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2385->hd=( void*) tm; _temp2385->tl=
_temp2335; _temp2385;}); _temp2384;});}}} _LL2269: if( ! Cyc_Absynpp_print_all_tvars){
if( _temp2304 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*) _temp2304->v)){
_temp2304= 0; _temp2306= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2306);}}{ struct Cyc_List_List* _temp2388;
void* _temp2390; struct Cyc_Absyn_Tqual* _temp2392; struct _tuple4 _temp2386=
Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(), _temp2302); _LL2393: _temp2392=
_temp2386.f1; goto _LL2391; _LL2391: _temp2390= _temp2386.f2; goto _LL2389;
_LL2389: _temp2388= _temp2386.f3; goto _LL2387; _LL2387: { struct Cyc_List_List*
tms= _temp2388; if( _temp2296 != 0){ tms=({ struct Cyc_List_List* _temp2394=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2394->hd=(
void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2395=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2395[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2396; _temp2396.tag= Cyc_Absyn_Attributes_mod_tag;
_temp2396.f1= 0; _temp2396.f2= _temp2296; _temp2396;}); _temp2395;})); _temp2394->tl=
tms; _temp2394;});} tms=({ struct Cyc_List_List* _temp2397=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2397->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2398=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2398[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2399; _temp2399.tag= Cyc_Absyn_Function_mod_tag;
_temp2399.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2400=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2400[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2401; _temp2401.tag= Cyc_Absyn_WithTypes_tag;
_temp2401.f1= _temp2300; _temp2401.f2= _temp2298; _temp2401.f3= _temp2304;
_temp2401;}); _temp2400;})); _temp2399;}); _temp2398;})); _temp2397->tl= tms;
_temp2397;}); if( _temp2306 != 0){ tms=({ struct Cyc_List_List* _temp2402=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2402->hd=(
void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2403=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2403[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2404; _temp2404.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp2404.f1= _temp2306; _temp2404.f2= 0; _temp2404.f3= 1; _temp2404;});
_temp2403;})); _temp2402->tl= tms; _temp2402;});} return({ struct _tuple4
_temp2405; _temp2405.f1= _temp2392; _temp2405.f2= _temp2390; _temp2405.f3= tms;
_temp2405;});}} _LL2271: if( _temp2310 == 0){ return({ struct _tuple4 _temp2406;
_temp2406.f1= tq; _temp2406.f2= t; _temp2406.f3= 0; _temp2406;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*) _temp2310->v);} _LL2273: if( _temp2314 == 0? 1:
! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp2407; _temp2407.f1=
tq; _temp2407.f2= t; _temp2407.f3= 0; _temp2407;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*) _temp2314->v);} _LL2275: return({ struct _tuple4 _temp2408;
_temp2408.f1= tq; _temp2408.f2= t; _temp2408.f3= 0; _temp2408;}); _LL2263:;}
struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual* tq, void* t,
struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp2411; void* _temp2413;
struct Cyc_Absyn_Tqual* _temp2415; struct _tuple4 _temp2409= Cyc_Absynpp_to_tms(
tq, t); _LL2416: _temp2415= _temp2409.f1; goto _LL2414; _LL2414: _temp2413=
_temp2409.f2; goto _LL2412; _LL2412: _temp2411= _temp2409.f3; goto _LL2410;
_LL2410: _temp2411=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2411); if( _temp2411 == 0? dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc(
_temp2415), Cyc_Absynpp_ntyp2doc( _temp2413));} else{ return Cyc_Absynpp_cat4(
Cyc_Absynpp_tqual2doc( _temp2415), Cyc_Absynpp_ntyp2doc( _temp2413), Cyc_PP_text((
struct _tagged_string)({ char* _temp2417=( char*)" "; struct _tagged_string
_temp2418; _temp2418.curr= _temp2417; _temp2418.base= _temp2417; _temp2418.last_plus_one=
_temp2417 + 2; _temp2418;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*) dopt->v, _temp2411));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return Cyc_Absynpp_cat5(
Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2419=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2419->v=(
void*) Cyc_Absynpp_textptr( f->name); _temp2419;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2420=( char*)":"; struct _tagged_string _temp2421;
_temp2421.curr= _temp2420; _temp2421.base= _temp2420; _temp2421.last_plus_one=
_temp2420 + 2; _temp2421;})), Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)( f->width)->v),
Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text(( struct _tagged_string)({
char* _temp2422=( char*)";"; struct _tagged_string _temp2423; _temp2423.curr=
_temp2422; _temp2423.base= _temp2422; _temp2423.last_plus_one= _temp2422 + 2;
_temp2423;})));} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_tqtd2doc( f->tq,(
void*) f->type,({ struct Cyc_Core_Opt* _temp2424=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2424->v=( void*) Cyc_Absynpp_textptr(
f->name); _temp2424;})), Cyc_Absynpp_atts2doc( f->attributes), Cyc_PP_text((
struct _tagged_string)({ char* _temp2425=( char*)";"; struct _tagged_string
_temp2426; _temp2426.curr= _temp2425; _temp2426.base= _temp2425; _temp2426.last_plus_one=
_temp2425 + 2; _temp2426;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char*
_temp2427=( char*)""; struct _tagged_string _temp2428; _temp2428.curr= _temp2427;
_temp2428.base= _temp2427; _temp2428.last_plus_one= _temp2427 + 1; _temp2428;}),
fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct Cyc_Absyn_Tunionfield*
f){ return Cyc_Absynpp_cat3( Cyc_Absynpp_typedef_name2doc( f->name), f->tvs == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,( struct _tagged_string)({ char*
_temp2429=( char*)","; struct _tagged_string _temp2430; _temp2430.curr=
_temp2429; _temp2430.base= _temp2429; _temp2430.last_plus_one= _temp2429 + 2;
_temp2430;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl= tdl->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd), 72, f); fprintf( f,"\n");}}
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){
return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct _tagged_string)({ char*
_temp2431=( char*)""; struct _tagged_string _temp2432; _temp2432.curr= _temp2431;
_temp2432.base= _temp2431; _temp2432.last_plus_one= _temp2431 + 1; _temp2432;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc, tdl)), 72);}
struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s),
72);} struct _tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string s= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc( t), 72);
Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return s;}} struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0* v){ return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);}
struct _tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);}