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
typedef void* Cyc_Position_Error_kind; extern const unsigned int Cyc_Position_Lex;
extern const unsigned int Cyc_Position_Parse; extern const unsigned int Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef
struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple1* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple1* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple1* name; struct Cyc_List_List* targs;
struct _tuple1* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple1* name; struct
_tuple1* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern const int Cyc_Absyn_UnionType_tag; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; extern const int Cyc_Absyn_EnumType_tag; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; extern const int
Cyc_Absyn_RgnHandleType_tag; struct Cyc_Absyn_RgnHandleType_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_Unresolved_m_struct{ int tag; struct _tuple1* f1; } ; extern
const int Cyc_Absyn_Tunion_m_tag; struct Cyc_Absyn_Tunion_m_struct{ int tag;
struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ; extern
const int Cyc_Absyn_XTunion_m_tag; struct Cyc_Absyn_XTunion_m_struct{ int tag;
struct Cyc_Absyn_XTuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ; extern
const int Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag; void*
f1; } ; extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{ int
tag; struct _tuple1* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
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
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; extern const
int Cyc_Absyn_Tunion_e_tag; struct Cyc_Absyn_Tunion_e_struct{ int tag; struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl*
f4; struct Cyc_Absyn_Tunionfield* f5; } ; extern const int Cyc_Absyn_XTunion_e_tag;
struct Cyc_Absyn_XTunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_XTuniondecl* f3; struct Cyc_Absyn_Tunionfield* f4; } ;
extern const int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; extern const int Cyc_Absyn_XTunion_p_tag;
struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; extern const int Cyc_Absyn_Enum_p_tag; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual* tq; void* type; struct
Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt*
cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Tuniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple1*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple1*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
char* tag; } ; extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_unify( void*, void*); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment* loc, struct
Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); extern int Cyc_Toc_check_for_null; struct Cyc_Absyn_Abs_n_struct
Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value;
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq_v=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile=
0,.q_restrict= 0}; struct Cyc_Absyn_Tqual* Cyc_Toc_mt_tq=& Cyc_Toc_mt_tq_v; int
Cyc_Toc_check_for_null= 0; char Cyc_Toc_Toc_Unimplemented_tag[ 18u]="Toc_Unimplemented";
struct Cyc_Toc_Toc_Unimplemented_struct{ char* tag; } ; char Cyc_Toc_Toc_Impossible_tag[
15u]="Toc_Impossible"; struct Cyc_Toc_Toc_Impossible_struct{ char* tag; } ;
static void* Cyc_Toc_unimp( struct _tagged_string s){({ struct _tagged_string
_temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one - _temp1.curr,
_temp1.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Unimplemented_struct*
_temp2=( struct Cyc_Toc_Toc_Unimplemented_struct*) GC_malloc( sizeof( struct Cyc_Toc_Toc_Unimplemented_struct));
_temp2[ 0]=({ struct Cyc_Toc_Toc_Unimplemented_struct _temp3; _temp3.tag= Cyc_Toc_Toc_Unimplemented_tag;
_temp3;}); _temp2;}));} static void* Cyc_Toc_toc_impos( struct _tagged_string s){({
struct _tagged_string _temp4= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp4.last_plus_one
- _temp4.curr, _temp4.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Impossible_struct*
_temp5=( struct Cyc_Toc_Toc_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Toc_Toc_Impossible_struct));
_temp5[ 0]=({ struct Cyc_Toc_Toc_Impossible_struct _temp6; _temp6.tag= Cyc_Toc_Toc_Impossible_tag;
_temp6;}); _temp5;}));} char Cyc_Toc_Match_error_tag[ 12u]="Match_error"; struct
Cyc_Toc_Match_error_struct{ char* tag; } ; static char _temp9[ 5u]="curr";
static struct _tagged_string Cyc_Toc_curr_string=( struct _tagged_string){
_temp9, _temp9, _temp9 + 5u}; static struct _tagged_string* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static char _temp12[ 14u]="last_plus_one"; static struct
_tagged_string Cyc_Toc_last_plus_one_string=( struct _tagged_string){ _temp12,
_temp12, _temp12 + 14u}; static struct _tagged_string* Cyc_Toc_last_plus_one_sp=&
Cyc_Toc_last_plus_one_string; static char _temp15[ 5u]="base"; static struct
_tagged_string Cyc_Toc_base_string=( struct _tagged_string){ _temp15, _temp15,
_temp15 + 5u}; static struct _tagged_string* Cyc_Toc_base_sp=& Cyc_Toc_base_string;
static char _temp18[ 15u]="_tagged_string"; static struct _tagged_string Cyc_Toc__tagged_string_string=(
struct _tagged_string){ _temp18, _temp18, _temp18 + 15u}; static struct
_tagged_string* Cyc_Toc__tagged_string_sp=& Cyc_Toc__tagged_string_string;
static char _temp21[ 15u]="_tunion_struct"; static struct _tagged_string Cyc_Toc__tunion_struct_string=(
struct _tagged_string){ _temp21, _temp21, _temp21 + 15u}; static struct
_tagged_string* Cyc_Toc__tunion_struct_sp=& Cyc_Toc__tunion_struct_string;
static char _temp24[ 16u]="_xtunion_struct"; static struct _tagged_string Cyc_Toc__xtunion_struct_string=(
struct _tagged_string){ _temp24, _temp24, _temp24 + 16u}; static struct
_tagged_string* Cyc_Toc__xtunion_struct_sp=& Cyc_Toc__xtunion_struct_string;
static char _temp27[ 2u]="v"; static struct _tagged_string Cyc_Toc_v_string=(
struct _tagged_string){ _temp27, _temp27, _temp27 + 2u}; static struct
_tagged_string* Cyc_Toc_v_sp=& Cyc_Toc_v_string; static char _temp30[ 4u]="tag";
static struct _tagged_string Cyc_Toc_tag_string=( struct _tagged_string){
_temp30, _temp30, _temp30 + 4u}; static struct _tagged_string* Cyc_Toc_tag_sp=&
Cyc_Toc_tag_string; static char _temp33[ 14u]="_handler_cons"; static struct
_tagged_string Cyc_Toc__handler_cons_string=( struct _tagged_string){ _temp33,
_temp33, _temp33 + 14u}; static struct _tagged_string* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static char _temp36[ 7u]="setjmp"; static struct
_tagged_string Cyc_Toc_setjmp_string=( struct _tagged_string){ _temp36, _temp36,
_temp36 + 7u}; static struct _tagged_string* Cyc_Toc_setjmp_sp=& Cyc_Toc_setjmp_string;
static char _temp39[ 14u]="_push_handler"; static struct _tagged_string Cyc_Toc__push_handler_string=(
struct _tagged_string){ _temp39, _temp39, _temp39 + 14u}; static struct
_tagged_string* Cyc_Toc__push_handler_sp=& Cyc_Toc__push_handler_string; static
char _temp42[ 13u]="_pop_handler"; static struct _tagged_string Cyc_Toc__pop_handler_string=(
struct _tagged_string){ _temp42, _temp42, _temp42 + 13u}; static struct
_tagged_string* Cyc_Toc__pop_handler_sp=& Cyc_Toc__pop_handler_string; static
char _temp45[ 8u]="handler"; static struct _tagged_string Cyc_Toc_handler_string=(
struct _tagged_string){ _temp45, _temp45, _temp45 + 8u}; static struct
_tagged_string* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static char _temp48[
14u]="_npop_handler"; static struct _tagged_string Cyc_Toc__npop_handler_string=(
struct _tagged_string){ _temp48, _temp48, _temp48 + 14u}; static struct _tuple1
Cyc_Toc__npop_handler_pr=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple1* Cyc_Toc__npop_handler_qv=&
Cyc_Toc__npop_handler_pr; static char _temp51[ 16u]="Match_Exception"; static
struct _tagged_string Cyc_Toc_Match_Exception_string=( struct _tagged_string){
_temp51, _temp51, _temp51 + 16u}; static struct _tagged_string* Cyc_Toc_Match_Exception_sp=&
Cyc_Toc_Match_Exception_string; static char _temp54[ 15u]="Cyc_new_string";
static struct _tagged_string Cyc_Toc_Cyc_new_string_string=( struct
_tagged_string){ _temp54, _temp54, _temp54 + 15u}; static struct _tagged_string*
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static int Cyc_Toc_is_nullable(
void* t){ if( ! Cyc_Toc_check_for_null){ return 0;}{ void* _temp55= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp61; struct Cyc_Absyn_Conref* _temp63; struct
Cyc_Absyn_Tqual* _temp65; struct Cyc_Absyn_Conref* _temp67; void* _temp69; void*
_temp71; _LL57: if(( unsigned int) _temp55 > 4u?(( struct _tunion_struct*)
_temp55)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL62: _temp61=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp55)->f1; _LL72: _temp71=( void*)
_temp61.elt_typ; goto _LL70; _LL70: _temp69=( void*) _temp61.rgn_typ; goto _LL68;
_LL68: _temp67=( struct Cyc_Absyn_Conref*) _temp61.nullable; goto _LL66; _LL66:
_temp65=( struct Cyc_Absyn_Tqual*) _temp61.tq; goto _LL64; _LL64: _temp63=(
struct Cyc_Absyn_Conref*) _temp61.bounds; goto _LL58;} else{ goto _LL59;} _LL59:
goto _LL60; _LL58: { void* _temp73=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp67))->v; int
_temp79; _LL75: if(( unsigned int) _temp73 > 1u?(( struct _tunion_struct*)
_temp73)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL80: _temp79=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp73)->f1; goto _LL76;} else{ goto _LL77;} _LL77: goto _LL78; _LL76: return
_temp79; _LL78: return 0; _LL74:;} _LL60:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp81=( char*)"is_nullable";
struct _tagged_string _temp82; _temp82.curr= _temp81; _temp82.base= _temp81;
_temp82.last_plus_one= _temp81 + 12; _temp82;})); return 0; _LL56:;}} static int
Cyc_Toc_really_is_nullable( void* t){ int b= Cyc_Toc_check_for_null; Cyc_Toc_check_for_null=
1;{ int ans= Cyc_Toc_is_nullable( t); Cyc_Toc_check_for_null= b; return ans;}}
static struct _tuple1* Cyc_Toc_collapse_qvar_tag( struct _tuple1* x, struct
_tagged_string tag){ return({ struct _tuple1* _temp83=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp83->f1=(* x).f1; _temp83->f2=({ struct
_tagged_string* _temp84=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp84[ 0]= Cyc_String_strconcat(*(* x).f2, tag); _temp84;});
_temp83;});} static struct Cyc_List_List* Cyc_Toc_added_decls= 0; static void
Cyc_Toc_add_tuple_decl( struct _tagged_string* x, struct Cyc_List_List* ts){
struct Cyc_List_List* fs= 0;{ int i= 1; for( 0; ts != 0; ts= ts->tl, i ++){ fs=({
struct Cyc_List_List* _temp85=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp85->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp86=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp86->name=({ struct _tagged_string* _temp87=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp87[ 0]= xprintf("f%d", i);
_temp87;}); _temp86->tq= Cyc_Toc_mt_tq; _temp86->type=( void*)(( void*) ts->hd);
_temp86->width= 0; _temp86->attributes= 0; _temp86;}); _temp85->tl= fs; _temp85;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp88=( struct
Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp88->sc=(
void*)(( void*) Cyc_Absyn_Public); _temp88->name=({ struct Cyc_Core_Opt* _temp90=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp90->v=(
void*)({ struct _tuple1* _temp91=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp91->f1= Cyc_Toc_abs_ns; _temp91->f2= x; _temp91;}); _temp90;});
_temp88->tvs= 0; _temp88->fields=({ struct Cyc_Core_Opt* _temp89=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp89->v=( void*) fs; _temp89;});
_temp88->attributes= 0; _temp88;}); Cyc_Toc_added_decls=({ struct Cyc_List_List*
_temp92=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp92->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp93=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp93[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp94; _temp94.tag= Cyc_Absyn_Struct_d_tag;
_temp94.f1= sd; _temp94;}); _temp93;}), 0); _temp92->tl= Cyc_Toc_added_decls;
_temp92;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp95=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp95->name= Cyc_Toc_curr_sp; _temp95->tq= Cyc_Toc_mt_tq; _temp95->type=( void*)
tptr; _temp95->width= 0; _temp95->attributes= 0; _temp95;}); struct Cyc_Absyn_Structfield*
f2=({ struct Cyc_Absyn_Structfield* _temp96=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp96->name= Cyc_Toc_base_sp;
_temp96->tq= Cyc_Toc_mt_tq; _temp96->type=( void*) tptr; _temp96->width= 0;
_temp96->attributes= 0; _temp96;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp97=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp97->name= Cyc_Toc_last_plus_one_sp;
_temp97->tq= Cyc_Toc_mt_tq; _temp97->type=( void*) tptr; _temp97->width= 0;
_temp97->attributes= 0; _temp97;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp98=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp98->hd=( void*) f1; _temp98->tl=({ struct Cyc_List_List* _temp99=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp99->hd=( void*)
f2; _temp99->tl=({ struct Cyc_List_List* _temp100=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp100->hd=( void*) f3; _temp100->tl=
0; _temp100;}); _temp99;}); _temp98;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp101=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp101->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp101->name=({ struct Cyc_Core_Opt* _temp103=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp103->v=( void*)({ struct _tuple1*
_temp104=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp104->f1=
Cyc_Toc_abs_ns; _temp104->f2= x; _temp104;}); _temp103;}); _temp101->tvs= 0;
_temp101->fields=({ struct Cyc_Core_Opt* _temp102=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp102->v=( void*)(( struct Cyc_List_List*)
fs); _temp102;}); _temp101->attributes= 0; _temp101;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp105->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp106=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp106[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp107; _temp107.tag= Cyc_Absyn_Struct_d_tag;
_temp107.f1= sd; _temp107;}); _temp106;}), 0); _temp105->tl= Cyc_Toc_added_decls;
_temp105;});} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int
Cyc_Toc_tuple_type_counter= 0; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* tts= Cyc_Toc_tuple_types;
for( 0; tts != 0; tts= tts->tl){ struct _tuple4 _temp110; struct Cyc_List_List*
_temp111; struct _tagged_string* _temp113; struct _tuple4* _temp108=( struct
_tuple4*) tts->hd; _temp110=* _temp108; _LL114: _temp113= _temp110.f1; goto
_LL112; _LL112: _temp111= _temp110.f2; goto _LL109; _LL109: { int okay= 1;
struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0? _temp111 != 0: 0; tqs= tqs->tl,
_temp111= _temp111->tl){ if( ! Cyc_Tcutil_unify((*(( struct _tuple5*) tqs->hd)).f2,(
void*) _temp111->hd)){ okay= 0; break;}} if(( ! okay? 1: tqs != 0)? 1: _temp111
!= 0){ continue;} return _temp113;}}}{ struct _tagged_string* x=({ struct
_tagged_string* _temp115=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp115[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp115;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp116=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp116->hd=( void*)({ struct _tuple4* _temp117=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp117->f1= x; _temp117->f2=
ts; _temp117;}); _temp116->tl= Cyc_Toc_tuple_types; _temp116;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type( void* t, struct
Cyc_Absyn_Tqual* tq){{ void* _temp118= t; void* _temp124; void* _temp126; _LL120:
if(( unsigned int) _temp118 > 4u?(( struct _tunion_struct*) _temp118)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL127: _temp126=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f1; if(( int) _temp126 == Cyc_Absyn_Unsigned){ goto _LL125;} else{
goto _LL122;} _LL125: _temp124=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f2; if(( int) _temp124 == Cyc_Absyn_B1){ goto _LL121;} else{ goto
_LL122;}} else{ goto _LL122;} _LL122: goto _LL123; _LL121: return Cyc_Toc__tagged_string_sp;
_LL123: goto _LL119; _LL119:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts= tts->tl){ struct _tuple6 _temp130; void* _temp131; struct
_tagged_string* _temp133; struct _tuple6* _temp128=( struct _tuple6*) tts->hd;
_temp130=* _temp128; _LL134: _temp133= _temp130.f1; goto _LL132; _LL132:
_temp131= _temp130.f2; goto _LL129; _LL129: if( ! Cyc_Tcutil_unify( t, _temp131)){
continue;} return _temp133;}}{ struct _tagged_string* x=({ struct _tagged_string*
_temp135=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp135[ 0]= xprintf("_tagged_ptr%d",( Cyc_Toc_tagged_ptr_counter ++));
_temp135;}); Cyc_Toc_add_tagged_ptr_decl( x, t); Cyc_Toc_tagged_ptr_types=({
struct Cyc_List_List* _temp136=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp136->hd=( void*)({ struct _tuple6* _temp137=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp137->f1= x; _temp137->f2=
t; _temp137;}); _temp136->tl= Cyc_Toc_tagged_ptr_types; _temp136;}); return x;}}
static int Cyc_Toc_temp_var_counter= 0; static struct _tuple1* Cyc_Toc_temp_var(){
return({ struct _tuple1* _temp138=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp138->f1= Cyc_Toc_abs_ns; _temp138->f2=({ struct _tagged_string*
_temp139=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp139[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter ++)); _temp139;});
_temp138;});} static int Cyc_Toc_fresh_label_counter= 0; static struct
_tagged_string* Cyc_Toc_fresh_label(){ return({ struct _tagged_string* _temp140=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp140[
0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter ++)); _temp140;});} static struct
Cyc_List_List* Cyc_Toc_temp_topdecls= 0; static void* Cyc_Toc_typ_to_c( void* t);
static struct _tuple2* Cyc_Toc_arg_to_c( struct _tuple2* a){ void* _temp143;
struct Cyc_Absyn_Tqual* _temp145; struct Cyc_Core_Opt* _temp147; struct _tuple2
_temp141=* a; _LL148: _temp147= _temp141.f1; goto _LL146; _LL146: _temp145=
_temp141.f2; goto _LL144; _LL144: _temp143= _temp141.f3; goto _LL142; _LL142:
return({ struct _tuple2* _temp149=( struct _tuple2*) GC_malloc( sizeof( struct
_tuple2)); _temp149->f1= _temp147; _temp149->f2= _temp145; _temp149->f3= Cyc_Toc_typ_to_c(
_temp143); _temp149;});} static struct _tuple5* Cyc_Toc_typ_to_c_f( struct
_tuple5* x){ void* _temp152; struct Cyc_Absyn_Tqual* _temp154; struct _tuple5
_temp150=* x; _LL155: _temp154= _temp150.f1; goto _LL153; _LL153: _temp152=
_temp150.f2; goto _LL151; _LL151: return({ struct _tuple5* _temp156=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp156->f1= _temp154; _temp156->f2=
Cyc_Toc_typ_to_c( _temp152); _temp156;});} static void* Cyc_Toc_typ_to_c_array(
void* t){ void* _temp157= t; struct Cyc_Absyn_Exp* _temp165; struct Cyc_Absyn_Tqual*
_temp167; void* _temp169; int _temp171; struct Cyc_Core_Opt* _temp173; struct
Cyc_Core_Opt _temp175; void* _temp176; void* _temp178; _LL159: if(( unsigned int)
_temp157 > 4u?(( struct _tunion_struct*) _temp157)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL170: _temp169=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp157)->f1;
goto _LL168; _LL168: _temp167=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp157)->f2; goto _LL166; _LL166: _temp165=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp157)->f3; goto _LL160;} else{ goto _LL161;}
_LL161: if(( unsigned int) _temp157 > 4u?(( struct _tunion_struct*) _temp157)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL179: _temp178=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp157)->f1; goto _LL174; _LL174: _temp173=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp157)->f2; if( _temp173 == 0){ goto _LL163;} else{
_temp175=* _temp173; _LL177: _temp176=( void*) _temp175.v; goto _LL172;} _LL172:
_temp171=( int)(( struct Cyc_Absyn_Evar_struct*) _temp157)->f3; goto _LL162;}
else{ goto _LL163;} _LL163: goto _LL164; _LL160: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp180=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp180[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp181; _temp181.tag= Cyc_Absyn_ArrayType_tag;
_temp181.f1=( void*) Cyc_Toc_typ_to_c_array( _temp169); _temp181.f2= _temp167;
_temp181.f3= _temp165; _temp181;}); _temp180;}); _LL162: return Cyc_Toc_typ_to_c_array(
_temp176); _LL164: return Cyc_Toc_typ_to_c( t); _LL158:;} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp182= t; int _temp230; struct Cyc_Core_Opt* _temp232; void*
_temp234; int _temp236; struct Cyc_Core_Opt* _temp238; struct Cyc_Core_Opt
_temp240; void* _temp241; void* _temp243; struct Cyc_Absyn_Tvar* _temp245;
struct Cyc_Absyn_TunionInfo _temp247; struct Cyc_Absyn_Tuniondecl* _temp249;
void* _temp251; struct Cyc_List_List* _temp253; struct _tuple1* _temp255; struct
Cyc_Absyn_XTunionInfo _temp257; struct Cyc_Absyn_XTuniondecl* _temp259; void*
_temp261; struct _tuple1* _temp263; struct Cyc_Absyn_TunionFieldInfo _temp265;
struct Cyc_Absyn_Tunionfield* _temp267; struct Cyc_Absyn_Tuniondecl* _temp269;
struct _tuple1* _temp271; struct Cyc_List_List* _temp273; struct _tuple1*
_temp275; struct Cyc_Absyn_XTunionFieldInfo _temp277; struct Cyc_Absyn_Tunionfield*
_temp279; struct Cyc_Absyn_XTuniondecl* _temp281; struct _tuple1* _temp283;
struct _tuple1* _temp285; struct Cyc_Absyn_PtrInfo _temp287; struct Cyc_Absyn_Conref*
_temp289; struct Cyc_Absyn_Tqual* _temp291; struct Cyc_Absyn_Conref* _temp293;
void* _temp295; void* _temp297; void* _temp299; void* _temp301; struct Cyc_Absyn_Exp*
_temp303; struct Cyc_Absyn_Tqual* _temp305; void* _temp307; struct Cyc_Absyn_FnInfo
_temp309; struct Cyc_List_List* _temp311; int _temp313; struct Cyc_List_List*
_temp315; void* _temp317; struct Cyc_Core_Opt* _temp319; struct Cyc_List_List*
_temp321; struct Cyc_List_List* _temp323; struct Cyc_Absyn_Uniondecl** _temp325;
struct Cyc_List_List* _temp327; struct _tuple1* _temp329; struct Cyc_Absyn_Structdecl**
_temp331; struct Cyc_List_List* _temp333; struct _tuple1* _temp335; struct Cyc_Absyn_Enumdecl*
_temp337; struct _tuple1* _temp339; struct Cyc_Core_Opt* _temp341; struct Cyc_List_List*
_temp343; struct _tuple1* _temp345; void* _temp347; void* _temp349; struct Cyc_List_List*
_temp351; _LL184: if(( int) _temp182 == Cyc_Absyn_VoidType){ goto _LL185;} else{
goto _LL186;} _LL186: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*)
_temp182)->tag == Cyc_Absyn_Evar_tag: 0){ _LL235: _temp234=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp182)->f1; goto _LL233; _LL233: _temp232=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp182)->f2; if( _temp232 == 0){ goto _LL231;} else{
goto _LL188;} _LL231: _temp230=( int)(( struct Cyc_Absyn_Evar_struct*) _temp182)->f3;
goto _LL187;} else{ goto _LL188;} _LL188: if(( unsigned int) _temp182 > 4u?((
struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_Evar_tag: 0){ _LL244:
_temp243=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp182)->f1; goto _LL239;
_LL239: _temp238=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp182)->f2; if( _temp238 == 0){ goto _LL190;} else{ _temp240=* _temp238;
_LL242: _temp241=( void*) _temp240.v; goto _LL237;} _LL237: _temp236=( int)((
struct Cyc_Absyn_Evar_struct*) _temp182)->f3; goto _LL189;} else{ goto _LL190;}
_LL190: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL246: _temp245=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp182)->f1; goto _LL191;} else{ goto _LL192;}
_LL192: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL248: _temp247=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp182)->f1; _LL256: _temp255=( struct
_tuple1*) _temp247.name; goto _LL254; _LL254: _temp253=( struct Cyc_List_List*)
_temp247.targs; goto _LL252; _LL252: _temp251=( void*) _temp247.rgn; goto _LL250;
_LL250: _temp249=( struct Cyc_Absyn_Tuniondecl*) _temp247.tud; goto _LL193;}
else{ goto _LL194;} _LL194: if(( unsigned int) _temp182 > 4u?(( struct
_tunion_struct*) _temp182)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL258:
_temp257=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp182)->f1; _LL264: _temp263=( struct _tuple1*) _temp257.name; goto _LL262;
_LL262: _temp261=( void*) _temp257.rgn; goto _LL260; _LL260: _temp259=( struct
Cyc_Absyn_XTuniondecl*) _temp257.xtud; goto _LL195;} else{ goto _LL196;} _LL196:
if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL266: _temp265=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp182)->f1; _LL276: _temp275=(
struct _tuple1*) _temp265.name; goto _LL274; _LL274: _temp273=( struct Cyc_List_List*)
_temp265.targs; goto _LL272; _LL272: _temp271=( struct _tuple1*) _temp265.fname;
goto _LL270; _LL270: _temp269=( struct Cyc_Absyn_Tuniondecl*) _temp265.tud; goto
_LL268; _LL268: _temp267=( struct Cyc_Absyn_Tunionfield*) _temp265.tufd; goto
_LL197;} else{ goto _LL198;} _LL198: if(( unsigned int) _temp182 > 4u?(( struct
_tunion_struct*) _temp182)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL278:
_temp277=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp182)->f1; _LL286: _temp285=( struct _tuple1*) _temp277.name; goto _LL284;
_LL284: _temp283=( struct _tuple1*) _temp277.fname; goto _LL282; _LL282:
_temp281=( struct Cyc_Absyn_XTuniondecl*) _temp277.xtud; goto _LL280; _LL280:
_temp279=( struct Cyc_Absyn_Tunionfield*) _temp277.xtufd; goto _LL199;} else{
goto _LL200;} _LL200: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*)
_temp182)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL288: _temp287=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp182)->f1; _LL298: _temp297=( void*)
_temp287.elt_typ; goto _LL296; _LL296: _temp295=( void*) _temp287.rgn_typ; goto
_LL294; _LL294: _temp293=( struct Cyc_Absyn_Conref*) _temp287.nullable; goto
_LL292; _LL292: _temp291=( struct Cyc_Absyn_Tqual*) _temp287.tq; goto _LL290;
_LL290: _temp289=( struct Cyc_Absyn_Conref*) _temp287.bounds; goto _LL201;}
else{ goto _LL202;} _LL202: if(( unsigned int) _temp182 > 4u?(( struct
_tunion_struct*) _temp182)->tag == Cyc_Absyn_IntType_tag: 0){ _LL302: _temp301=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp182)->f1; goto _LL300; _LL300:
_temp299=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp182)->f2; goto _LL203;}
else{ goto _LL204;} _LL204: if(( int) _temp182 == Cyc_Absyn_FloatType){ goto
_LL205;} else{ goto _LL206;} _LL206: if(( int) _temp182 == Cyc_Absyn_DoubleType){
goto _LL207;} else{ goto _LL208;} _LL208: if(( unsigned int) _temp182 > 4u?((
struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL308:
_temp307=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp182)->f1; goto
_LL306; _LL306: _temp305=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp182)->f2; goto _LL304; _LL304: _temp303=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp182)->f3; goto _LL209;} else{ goto _LL210;}
_LL210: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL310: _temp309=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp182)->f1; _LL322: _temp321=( struct Cyc_List_List*)
_temp309.tvars; goto _LL320; _LL320: _temp319=( struct Cyc_Core_Opt*) _temp309.effect;
goto _LL318; _LL318: _temp317=( void*) _temp309.ret_typ; goto _LL316; _LL316:
_temp315=( struct Cyc_List_List*) _temp309.args; goto _LL314; _LL314: _temp313=(
int) _temp309.varargs; goto _LL312; _LL312: _temp311=( struct Cyc_List_List*)
_temp309.attributes; goto _LL211;} else{ goto _LL212;} _LL212: if(( unsigned int)
_temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL324: _temp323=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp182)->f1; goto _LL213;} else{ goto _LL214;} _LL214: if(( unsigned int)
_temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL330: _temp329=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp182)->f1; goto _LL328; _LL328: _temp327=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp182)->f2; goto _LL326; _LL326: _temp325=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp182)->f3;
goto _LL215;} else{ goto _LL216;} _LL216: if(( unsigned int) _temp182 > 4u?((
struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_StructType_tag: 0){ _LL336:
_temp335=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp182)->f1;
goto _LL334; _LL334: _temp333=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp182)->f2; goto _LL332; _LL332: _temp331=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp182)->f3; goto _LL217;} else{ goto
_LL218;} _LL218: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*)
_temp182)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL340: _temp339=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp182)->f1; goto _LL338; _LL338: _temp337=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp182)->f2;
goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp182 > 4u?((
struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL346:
_temp345=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp182)->f1;
goto _LL344; _LL344: _temp343=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp182)->f2; goto _LL342; _LL342: _temp341=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp182)->f3; goto _LL221;} else{ goto _LL222;}
_LL222: if(( unsigned int) _temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL348: _temp347=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp182)->f1; goto _LL223;} else{ goto _LL224;} _LL224: if(( int) _temp182 ==
Cyc_Absyn_HeapRgn){ goto _LL225;} else{ goto _LL226;} _LL226: if(( unsigned int)
_temp182 > 4u?(( struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL350: _temp349=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp182)->f1;
goto _LL227;} else{ goto _LL228;} _LL228: if(( unsigned int) _temp182 > 4u?((
struct _tunion_struct*) _temp182)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL352:
_temp351=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp182)->f1;
goto _LL229;} else{ goto _LL183;} _LL185: return t; _LL187: return( void*) Cyc_Absyn_VoidType;
_LL189: return Cyc_Toc_typ_to_c( _temp241); _LL191: return Cyc_Absyn_void_star_typ();
_LL193: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp353=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp353[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp354; _temp354.tag= Cyc_Absyn_TypedefType_tag;
_temp354.f1=( struct _tuple1*) _temp255; _temp354.f2= 0; _temp354.f3=({ struct
Cyc_Core_Opt* _temp355=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp355->v=( void*) Cyc_Absyn_void_star_typ(); _temp355;}); _temp354;});
_temp353;}); _LL195: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp356=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp356[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp357; _temp357.tag= Cyc_Absyn_TypedefType_tag;
_temp357.f1= _temp263; _temp357.f2= 0; _temp357.f3=({ struct Cyc_Core_Opt*
_temp358=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp358->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp),(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp358;}); _temp357;}); _temp356;});
_LL197: if( _temp267->typs == 0){ return Cyc_Absyn_uint_t;} else{ return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag( _temp271,( struct _tagged_string)({ char* _temp359=(
char*)"_struct"; struct _tagged_string _temp360; _temp360.curr= _temp359;
_temp360.base= _temp359; _temp360.last_plus_one= _temp359 + 8; _temp360;})));}
_LL199: return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp283,( struct
_tagged_string)({ char* _temp361=( char*)"_struct"; struct _tagged_string
_temp362; _temp362.curr= _temp361; _temp362.base= _temp361; _temp362.last_plus_one=
_temp361 + 8; _temp362;}))); _LL201: _temp297= Cyc_Toc_typ_to_c_array( _temp297);{
void* _temp363=( void*) _temp289->v; void* _temp369; _LL365: if(( unsigned int)
_temp363 > 1u?(( struct _tunion_struct*) _temp363)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL370: _temp369=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp363)->f1;
if(( int) _temp369 == Cyc_Absyn_Unknown_b){ goto _LL366;} else{ goto _LL367;}}
else{ goto _LL367;} _LL367: goto _LL368; _LL366: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp297, _temp291); return Cyc_Absyn_strct( n);} _LL368: return Cyc_Absyn_star_typ(
_temp297,( void*) Cyc_Absyn_HeapRgn, _temp291); _LL364:;} _LL203: return t;
_LL205: return t; _LL207: return t; _LL209: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp371=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp371[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp372; _temp372.tag= Cyc_Absyn_ArrayType_tag;
_temp372.f1=( void*) Cyc_Toc_typ_to_c_array( _temp307); _temp372.f2= _temp305;
_temp372.f3= _temp303; _temp372;}); _temp371;}); _LL211: return( void*)({ struct
Cyc_Absyn_FnType_struct* _temp373=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp373[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp374; _temp374.tag= Cyc_Absyn_FnType_tag; _temp374.f1=({ struct Cyc_Absyn_FnInfo
_temp375; _temp375.tvars= 0; _temp375.effect= 0; _temp375.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp317); _temp375.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp315); _temp375.varargs= _temp313; _temp375.attributes= _temp311; _temp375;});
_temp374;}); _temp373;}); _LL213: _temp323=(( struct Cyc_List_List*(*)( struct
_tuple5*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp323);{ struct _tagged_string* n= Cyc_Toc_add_tuple_type( _temp323); return
Cyc_Absyn_strct( n);} _LL215: if( _temp329 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp376=(
char*)"anonymous union"; struct _tagged_string _temp377; _temp377.curr= _temp376;
_temp377.base= _temp376; _temp377.last_plus_one= _temp376 + 16; _temp377;}));}
return Cyc_Absyn_unionq_typ(( struct _tuple1*)({ struct _tuple1* _temp378=
_temp329; if( _temp378 == 0){ _throw( Null_Exception);} _temp378;})); _LL217:
if( _temp335 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp379=( char*)"anonymous struct"; struct
_tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 17; _temp380;}));} return Cyc_Absyn_strctq((
struct _tuple1*)({ struct _tuple1* _temp381= _temp335; if( _temp381 == 0){
_throw( Null_Exception);} _temp381;})); _LL219: return t; _LL221: if( _temp341
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp382=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base= _temp382;
_temp383.last_plus_one= _temp382 + 38; _temp383;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp384=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp384[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp385; _temp385.tag= Cyc_Absyn_TypedefType_tag;
_temp385.f1= _temp345; _temp385.f2= 0; _temp385.f3=({ struct Cyc_Core_Opt*
_temp386=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp386->v=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp341->v); _temp386;});
_temp385;}); _temp384;}); _LL223: return Cyc_Absyn_void_star_typ(); _LL225:
return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp387=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 55; _temp388;})); _LL227: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp389=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 54; _temp390;})); _LL229: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp391=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 52; _temp392;})); _LL183:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp393= t; struct Cyc_Absyn_Exp* _temp399; struct Cyc_Absyn_Tqual*
_temp401; void* _temp403; _LL395: if(( unsigned int) _temp393 > 4u?(( struct
_tunion_struct*) _temp393)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL404: _temp403=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp393)->f1; goto _LL402; _LL402:
_temp401=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp393)->f2; goto _LL400; _LL400: _temp399=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp393)->f3; goto _LL396;} else{ goto _LL397;}
_LL397: goto _LL398; _LL396: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp403,( void*) Cyc_Absyn_HeapRgn, _temp401), e, l); _LL398: return e; _LL394:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp405= t; struct Cyc_Absyn_Exp*
_temp411; struct Cyc_Absyn_Tqual* _temp413; void* _temp415; _LL407: if((
unsigned int) _temp405 > 4u?(( struct _tunion_struct*) _temp405)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp405)->f1;
goto _LL414; _LL414: _temp413=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp405)->f2; goto _LL412; _LL412: _temp411=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp405)->f3; goto _LL408;} else{ goto _LL409;}
_LL409: goto _LL410; _LL408: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp415,( void*) Cyc_Absyn_HeapRgn, _temp413), e, l); _LL410: return Cyc_Absyn_cast_exp(
t, e, l); _LL406:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp417= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp453; void* _temp455; void* _temp457; struct Cyc_Absyn_Exp*
_temp459; struct Cyc_Absyn_Tqual* _temp461; void* _temp463; struct Cyc_Absyn_FnInfo
_temp465; struct Cyc_List_List* _temp467; struct Cyc_Absyn_Structdecl** _temp469;
struct Cyc_List_List* _temp471; struct _tuple1* _temp473; struct Cyc_Absyn_TunionInfo
_temp475; struct Cyc_Absyn_XTunionInfo _temp477; struct Cyc_Absyn_TunionFieldInfo
_temp479; struct Cyc_Absyn_Tunionfield* _temp481; struct Cyc_Absyn_Tuniondecl*
_temp483; struct _tuple1* _temp485; struct Cyc_List_List* _temp487; struct
_tuple1* _temp489; struct Cyc_Absyn_XTunionFieldInfo _temp491; struct Cyc_Absyn_Uniondecl**
_temp493; struct Cyc_List_List* _temp495; struct _tuple1* _temp497; struct Cyc_Absyn_PtrInfo
_temp499; void* _temp501; _LL419: if(( int) _temp417 == Cyc_Absyn_VoidType){
goto _LL420;} else{ goto _LL421;} _LL421: if(( unsigned int) _temp417 > 4u?((
struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_VarType_tag: 0){ _LL454:
_temp453=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp417)->f1;
goto _LL422;} else{ goto _LL423;} _LL423: if(( unsigned int) _temp417 > 4u?((
struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_IntType_tag: 0){ _LL458:
_temp457=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp417)->f1; goto _LL456;
_LL456: _temp455=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp417)->f2;
goto _LL424;} else{ goto _LL425;} _LL425: if(( int) _temp417 == Cyc_Absyn_FloatType){
goto _LL426;} else{ goto _LL427;} _LL427: if(( int) _temp417 == Cyc_Absyn_DoubleType){
goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int) _temp417 > 4u?((
struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL464:
_temp463=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp417)->f1; goto
_LL462; _LL462: _temp461=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp417)->f2; goto _LL460; _LL460: _temp459=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp417)->f3; goto _LL430;} else{ goto _LL431;}
_LL431: if(( unsigned int) _temp417 > 4u?(( struct _tunion_struct*) _temp417)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL466: _temp465=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp417)->f1; goto _LL432;} else{ goto _LL433;}
_LL433: if(( unsigned int) _temp417 > 4u?(( struct _tunion_struct*) _temp417)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL468: _temp467=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp417)->f1; goto _LL434;} else{ goto
_LL435;} _LL435: if(( unsigned int) _temp417 > 4u?(( struct _tunion_struct*)
_temp417)->tag == Cyc_Absyn_StructType_tag: 0){ _LL474: _temp473=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp417)->f1; goto _LL472;
_LL472: _temp471=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp417)->f2; goto _LL470; _LL470: _temp469=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp417)->f3; goto _LL436;} else{ goto
_LL437;} _LL437: if(( unsigned int) _temp417 > 4u?(( struct _tunion_struct*)
_temp417)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL476: _temp475=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp417)->f1; goto _LL438;} else{ goto
_LL439;} _LL439: if(( unsigned int) _temp417 > 4u?(( struct _tunion_struct*)
_temp417)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL478: _temp477=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp417)->f1; goto _LL440;} else{ goto
_LL441;} _LL441: if(( unsigned int) _temp417 > 4u?(( struct _tunion_struct*)
_temp417)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL480: _temp479=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp417)->f1;
_LL490: _temp489=( struct _tuple1*) _temp479.name; goto _LL488; _LL488: _temp487=(
struct Cyc_List_List*) _temp479.targs; goto _LL486; _LL486: _temp485=( struct
_tuple1*) _temp479.fname; goto _LL484; _LL484: _temp483=( struct Cyc_Absyn_Tuniondecl*)
_temp479.tud; goto _LL482; _LL482: _temp481=( struct Cyc_Absyn_Tunionfield*)
_temp479.tufd; goto _LL442;} else{ goto _LL443;} _LL443: if(( unsigned int)
_temp417 > 4u?(( struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_XTunionFieldType_tag:
0){ _LL492: _temp491=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp417)->f1; goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int)
_temp417 > 4u?(( struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL498: _temp497=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp417)->f1; goto _LL496; _LL496: _temp495=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp417)->f2; goto _LL494; _LL494: _temp493=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp417)->f3;
goto _LL446;} else{ goto _LL447;} _LL447: if(( unsigned int) _temp417 > 4u?((
struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL500:
_temp499=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp417)->f1; goto _LL448;} else{ goto _LL449;} _LL449: if(( unsigned int)
_temp417 > 4u?(( struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL502: _temp501=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp417)->f1;
goto _LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL420: return 1; _LL422:
return 0; _LL424: return 1; _LL426: return 1; _LL428: return 0; _LL430: return
Cyc_Toc_atomic_typ( _temp463); _LL432: return 1; _LL434: for( 0; _temp467 != 0;
_temp467= _temp467->tl){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple5*) _temp467->hd)).f2)){
return 0;}} return 1; _LL436: return 0; _LL438: return 0; _LL440: return 0;
_LL442: return _temp481->typs == 0; _LL444: return 0; _LL446: return 0; _LL448:
return 0; _LL450: return 0; _LL452:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp503= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp503.last_plus_one - _temp503.curr, _temp503.curr);})); return 0; _LL418:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp504= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp510; struct Cyc_Absyn_Conref* _temp512; struct
Cyc_Absyn_Tqual* _temp514; struct Cyc_Absyn_Conref* _temp516; void* _temp518;
void* _temp520; _LL506: if(( unsigned int) _temp504 > 4u?(( struct
_tunion_struct*) _temp504)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL511:
_temp510=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp504)->f1; _LL521: _temp520=( void*) _temp510.elt_typ; goto _LL519; _LL519:
_temp518=( void*) _temp510.rgn_typ; goto _LL517; _LL517: _temp516=( struct Cyc_Absyn_Conref*)
_temp510.nullable; goto _LL515; _LL515: _temp514=( struct Cyc_Absyn_Tqual*)
_temp510.tq; goto _LL513; _LL513: _temp512=( struct Cyc_Absyn_Conref*) _temp510.bounds;
goto _LL507;} else{ goto _LL508;} _LL508: goto _LL509; _LL507: { void* _temp522=
Cyc_Tcutil_compress( _temp520); _LL524: if(( int) _temp522 == Cyc_Absyn_VoidType){
goto _LL525;} else{ goto _LL526;} _LL526: goto _LL527; _LL525: return 1; _LL527:
return 0; _LL523:;} _LL509: return 0; _LL505:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp528= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp540; struct Cyc_Absyn_Structdecl* _temp542;
struct Cyc_List_List* _temp543; struct _tuple1* _temp545; struct Cyc_Absyn_Structdecl**
_temp547; struct Cyc_List_List* _temp549; struct _tuple1* _temp551; struct Cyc_Absyn_Uniondecl**
_temp553; struct Cyc_Absyn_Uniondecl* _temp555; struct Cyc_List_List* _temp556;
struct _tuple1* _temp558; struct Cyc_Absyn_Uniondecl** _temp560; struct Cyc_List_List*
_temp562; struct _tuple1* _temp564; _LL530: if(( unsigned int) _temp528 > 4u?((
struct _tunion_struct*) _temp528)->tag == Cyc_Absyn_StructType_tag: 0){ _LL546:
_temp545=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp528)->f1;
goto _LL544; _LL544: _temp543=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp528)->f2; goto _LL541; _LL541: _temp540=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp528)->f3; if( _temp540 == 0){ goto
_LL532;} else{ _temp542=* _temp540; goto _LL531;}} else{ goto _LL532;} _LL532:
if(( unsigned int) _temp528 > 4u?(( struct _tunion_struct*) _temp528)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL552: _temp551=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp528)->f1; goto _LL550; _LL550: _temp549=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp528)->f2; goto _LL548; _LL548: _temp547=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp528)->f3;
if( _temp547 == 0){ goto _LL533;} else{ goto _LL534;}} else{ goto _LL534;}
_LL534: if(( unsigned int) _temp528 > 4u?(( struct _tunion_struct*) _temp528)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL559: _temp558=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp528)->f1; goto _LL557; _LL557: _temp556=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp528)->f2; goto _LL554; _LL554: _temp553=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp528)->f3;
if( _temp553 == 0){ goto _LL536;} else{ _temp555=* _temp553; goto _LL535;}}
else{ goto _LL536;} _LL536: if(( unsigned int) _temp528 > 4u?(( struct
_tunion_struct*) _temp528)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL565: _temp564=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp528)->f1; goto
_LL563; _LL563: _temp562=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp528)->f2; goto _LL561; _LL561: _temp560=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp528)->f3; if( _temp560 == 0){ goto
_LL537;} else{ goto _LL538;}} else{ goto _LL538;} _LL538: goto _LL539; _LL531:
if( _temp545 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp566=( char*)"StructType name not properly set";
struct _tagged_string _temp567; _temp567.curr= _temp566; _temp567.base= _temp566;
_temp567.last_plus_one= _temp566 + 33; _temp567;}));} if( _temp542->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp569= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp568=
_temp545; if( _temp568 == 0){ _throw( Null_Exception);} _temp568;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp569.last_plus_one - _temp569.curr, _temp569.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp542->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)
fields->hd)->name) == 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp571= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp570= _temp545; if( _temp570 == 0){ _throw( Null_Exception);}
_temp570;})); struct _tagged_string _temp572=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp571.last_plus_one - _temp571.curr, _temp571.curr, _temp572.last_plus_one -
_temp572.curr, _temp572.curr);})); return 0;} _LL533: if( _temp551 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp573=( char*)"StructType name not properly set"; struct _tagged_string
_temp574; _temp574.curr= _temp573; _temp574.base= _temp573; _temp574.last_plus_one=
_temp573 + 33; _temp574;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp576= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp575= _temp551; if( _temp575 == 0){ _throw( Null_Exception);}
_temp575;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp576.last_plus_one
- _temp576.curr, _temp576.curr);})); return 0; _LL535: if( _temp558 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp577=( char*)"UnionType name not properly set"; struct _tagged_string
_temp578; _temp578.curr= _temp577; _temp578.base= _temp577; _temp578.last_plus_one=
_temp577 + 32; _temp578;}));} if( _temp555->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp580= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp579= _temp558; if( _temp579 == 0){
_throw( Null_Exception);} _temp579;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp580.last_plus_one - _temp580.curr, _temp580.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)( _temp555->fields)->v; for( 0; fields != 0;
fields= fields->tl){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)
fields->hd)->name) == 0){ return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type);}}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp582= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp581= _temp558; if( _temp581 == 0){ _throw( Null_Exception);}
_temp581;})); struct _tagged_string _temp583=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp582.last_plus_one - _temp582.curr, _temp582.curr, _temp583.last_plus_one -
_temp583.curr, _temp583.curr);})); return 0;} _LL537: if( _temp564 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp584=( char*)"UnionType name not properly set"; struct _tagged_string
_temp585; _temp585.curr= _temp584; _temp585.base= _temp584; _temp585.last_plus_one=
_temp584 + 32; _temp585;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp587= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp586= _temp564; if( _temp586 == 0){ _throw( Null_Exception);}
_temp586;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp587.last_plus_one
- _temp587.curr, _temp587.curr);})); return 0; _LL539:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp588= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp588.last_plus_one - _temp588.curr,
_temp588.curr);})); return 0; _LL529:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp589=( void*) e->r; struct _tagged_string*
_temp597; struct Cyc_Absyn_Exp* _temp599; struct _tagged_string* _temp601;
struct Cyc_Absyn_Exp* _temp603; _LL591: if((( struct _tunion_struct*) _temp589)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL600: _temp599=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp589)->f1; goto _LL598; _LL598:
_temp597=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp589)->f2; goto _LL592;} else{ goto _LL593;} _LL593: if((( struct
_tunion_struct*) _temp589)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL604:
_temp603=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp589)->f1; goto _LL602; _LL602: _temp601=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp589)->f2; goto _LL594;} else{ goto _LL595;}
_LL595: goto _LL596; _LL592: return Cyc_Toc_is_poly_field(( void*)( _temp599->topt)->v,
_temp597); _LL594: { void* _temp605= Cyc_Tcutil_compress(( void*)( _temp603->topt)->v);
struct Cyc_Absyn_PtrInfo _temp611; struct Cyc_Absyn_Conref* _temp613; struct Cyc_Absyn_Tqual*
_temp615; struct Cyc_Absyn_Conref* _temp617; void* _temp619; void* _temp621;
_LL607: if(( unsigned int) _temp605 > 4u?(( struct _tunion_struct*) _temp605)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL612: _temp611=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp605)->f1; _LL622: _temp621=( void*)
_temp611.elt_typ; goto _LL620; _LL620: _temp619=( void*) _temp611.rgn_typ; goto
_LL618; _LL618: _temp617=( struct Cyc_Absyn_Conref*) _temp611.nullable; goto
_LL616; _LL616: _temp615=( struct Cyc_Absyn_Tqual*) _temp611.tq; goto _LL614;
_LL614: _temp613=( struct Cyc_Absyn_Conref*) _temp611.bounds; goto _LL608;}
else{ goto _LL609;} _LL609: goto _LL610; _LL608: return Cyc_Toc_is_poly_field(
_temp621, _temp601); _LL610:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp623= Cyc_Absynpp_typ2string(( void*)( _temp603->topt)->v);
xprintf("is_poly_project:  bad type %.*s", _temp623.last_plus_one - _temp623.curr,
_temp623.curr);})); return 0; _LL606:;} _LL596: return 0; _LL590:;} static char
_temp626[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp626, _temp626, _temp626 + 10u}; static struct
_tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp627=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp627->hd=( void*) s; _temp627->tl= 0; _temp627;}), 0);}
static char _temp630[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp630, _temp630, _temp630 + 17u}; static struct
_tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp631=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp631->hd=( void*) s; _temp631->tl= 0; _temp631;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp633=({ struct _tuple0
_temp632; _temp632.f1= t1; _temp632.f2= t2; _temp632;}); void* _temp639; struct
Cyc_Absyn_PtrInfo _temp641; void* _temp643; struct Cyc_Absyn_PtrInfo _temp645;
_LL635: _LL644: _temp643= _temp633.f1; if(( unsigned int) _temp643 > 4u?((
struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL646:
_temp645=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp643)->f1; goto _LL640;} else{ goto _LL637;} _LL640: _temp639= _temp633.f2;
if(( unsigned int) _temp639 > 4u?(( struct _tunion_struct*) _temp639)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL642: _temp641=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp639)->f1; goto _LL636;} else{ goto
_LL637;} _LL637: goto _LL638; _LL636: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp645.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp641.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp645.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp641.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple0 _temp648=({ struct _tuple0 _temp647; _temp647.f1= b1; _temp647.f2=
b2; _temp647;}); void* _temp658; struct Cyc_Absyn_Exp* _temp660; void* _temp662;
struct Cyc_Absyn_Exp* _temp664; void* _temp666; void* _temp668; void* _temp670;
void* _temp672; struct Cyc_Absyn_Exp* _temp674; void* _temp676; struct Cyc_Absyn_Exp*
_temp678; void* _temp680; _LL650: _LL663: _temp662= _temp648.f1; if((
unsigned int) _temp662 > 1u?(( struct _tunion_struct*) _temp662)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL665: _temp664=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp662)->f1; goto _LL659;} else{ goto _LL652;} _LL659: _temp658= _temp648.f2;
if(( unsigned int) _temp658 > 1u?(( struct _tunion_struct*) _temp658)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL661: _temp660=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp658)->f1; goto _LL651;} else{ goto _LL652;}
_LL652: _LL669: _temp668= _temp648.f1; if(( int) _temp668 == Cyc_Absyn_Unknown_b){
goto _LL667;} else{ goto _LL654;} _LL667: _temp666= _temp648.f2; if(( int)
_temp666 == Cyc_Absyn_Unknown_b){ goto _LL653;} else{ goto _LL654;} _LL654:
_LL673: _temp672= _temp648.f1; if(( unsigned int) _temp672 > 1u?(( struct
_tunion_struct*) _temp672)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL675: _temp674=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp672)->f1; goto
_LL671;} else{ goto _LL656;} _LL671: _temp670= _temp648.f2; if(( int) _temp670
== Cyc_Absyn_Unknown_b){ goto _LL655;} else{ goto _LL656;} _LL656: _LL681:
_temp680= _temp648.f1; if(( int) _temp680 == Cyc_Absyn_Unknown_b){ goto _LL677;}
else{ goto _LL649;} _LL677: _temp676= _temp648.f2; if(( unsigned int) _temp676 >
1u?(( struct _tunion_struct*) _temp676)->tag == Cyc_Absyn_Upper_b_tag: 0){
_LL679: _temp678=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp676)->f1; goto _LL657;} else{ goto _LL649;} _LL651: if( n1? ! n2: 0){ convs=({
struct Cyc_List_List* _temp682=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp682->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp683=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp683[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp684; _temp684.tag= Cyc_Toc_NullCheck_tag;
_temp684.f1=( void*) t2; _temp684;}); _temp683;})); _temp682->tl= convs;
_temp682;});} goto _LL649; _LL653: goto _LL649; _LL655: convs=({ struct Cyc_List_List*
_temp685=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp685->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp686=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp686[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp687; _temp687.tag= Cyc_Toc_TagPtr_tag;
_temp687.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp687.f2= _temp674; _temp687;});
_temp686;})); _temp685->tl= convs; _temp685;}); goto _LL649; _LL657: convs=({
struct Cyc_List_List* _temp688=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp688->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp689=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp689[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp690; _temp690.tag= Cyc_Toc_UntagPtr_tag;
_temp690.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp690.f2= _temp678; _temp690.f3=
n2; _temp690;}); _temp689;})); _temp688->tl= convs; _temp688;}); goto _LL649;
_LL649:;} return convs;} _LL638: return 0; _LL634:;}} static char _temp693[ 7u]="_throw";
static struct _tagged_string Cyc_Toc__throw_string=( struct _tagged_string){
_temp693, _temp693, _temp693 + 7u}; static struct _tuple1 Cyc_Toc__throw_var=(
struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__throw_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({
struct Cyc_List_List* _temp694=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp694->hd=( void*) e; _temp694->tl= 0; _temp694;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp695=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 44; _temp696;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp697= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp703; struct Cyc_Absyn_Conref* _temp705; struct Cyc_Absyn_Tqual*
_temp707; struct Cyc_Absyn_Conref* _temp709; void* _temp711; void* _temp713;
_LL699: if(( unsigned int) _temp697 > 4u?(( struct _tunion_struct*) _temp697)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL704: _temp703=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp697)->f1; _LL714: _temp713=( void*)
_temp703.elt_typ; goto _LL712; _LL712: _temp711=( void*) _temp703.rgn_typ; goto
_LL710; _LL710: _temp709=( struct Cyc_Absyn_Conref*) _temp703.nullable; goto
_LL708; _LL708: _temp707=( struct Cyc_Absyn_Tqual*) _temp703.tq; goto _LL706;
_LL706: _temp705=( struct Cyc_Absyn_Conref*) _temp703.bounds; goto _LL700;}
else{ goto _LL701;} _LL701: goto _LL702; _LL700: elt_typ= _temp713;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp713, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL698;} _LL702: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp715=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp716;
_temp716.curr= _temp715; _temp716.base= _temp715; _temp716.last_plus_one=
_temp715 + 28; _temp716;})); _LL698:;} if( toplevel){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp717=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp717[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp718; _temp718.tag= Cyc_Absyn_ArrayType_tag;
_temp718.f1=( void*) elt_typ; _temp718.f2= Cyc_Toc_mt_tq; _temp718.f3=( struct
Cyc_Absyn_Exp*) sz; _temp718;}); _temp717;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp719->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp720=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp720[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp721; _temp721.tag= Cyc_Absyn_Var_d_tag;
_temp721.f1= vd; _temp721;}); _temp720;}), 0); _temp719->tl= Cyc_Toc_temp_topdecls;
_temp719;});{ struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp722=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp722->hd=( void*)({ struct _tuple7* _temp727=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp727->f1= 0; _temp727->f2=
xexp; _temp727;}); _temp722->tl=({ struct Cyc_List_List* _temp723=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp723->hd=( void*)({ struct
_tuple7* _temp726=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp726->f1= 0; _temp726->f2= xexp; _temp726;}); _temp723->tl=({ struct Cyc_List_List*
_temp724=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp724->hd=( void*)({ struct _tuple7* _temp725=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp725->f1= 0; _temp725->f2= xplussz; _temp725;});
_temp724->tl= 0; _temp724;}); _temp723;}); _temp722;}), 0); return urm_exp;}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp728=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp729; _temp729.curr= _temp728; _temp729.base= _temp728; _temp729.last_plus_one=
_temp728 + 45; _temp729;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
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
cs= Cyc_Toc_conversion( t1, t2); for( 0; cs != 0; cs= cs->tl){ void* _temp730=(
void*) cs->hd; void* _temp738; struct Cyc_Absyn_Exp* _temp740; void* _temp742;
int _temp744; struct Cyc_Absyn_Exp* _temp746; void* _temp748; _LL732: if(((
struct _tunion_struct*) _temp730)->tag == Cyc_Toc_NullCheck_tag){ _LL739:
_temp738=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp730)->f1; goto _LL733;}
else{ goto _LL734;} _LL734: if((( struct _tunion_struct*) _temp730)->tag == Cyc_Toc_TagPtr_tag){
_LL743: _temp742=( void*)(( struct Cyc_Toc_TagPtr_struct*) _temp730)->f1; goto
_LL741; _LL741: _temp740=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_TagPtr_struct*)
_temp730)->f2; goto _LL735;} else{ goto _LL736;} _LL736: if((( struct
_tunion_struct*) _temp730)->tag == Cyc_Toc_UntagPtr_tag){ _LL749: _temp748=(
void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp730)->f1; goto _LL747; _LL747:
_temp746=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*) _temp730)->f2;
goto _LL745; _LL745: _temp744=( int)(( struct Cyc_Toc_UntagPtr_struct*) _temp730)->f3;
goto _LL737;} else{ goto _LL731;} _LL733: e= Cyc_Toc_null_check_conv( toplevel,
_temp738, e); goto _LL731; _LL735: e= Cyc_Toc_tag_ptr_conv( toplevel, _temp742,
_temp740, e); goto _LL731; _LL737: e= Cyc_Toc_untag_ptr_conv( toplevel, _temp748,
_temp746, _temp744, e); goto _LL731; _LL731:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp750=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp750->break_lab= 0; _temp750->continue_lab= 0;
_temp750->fallthru_info= 0; _temp750->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp750->toplevel= 1; _temp750;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp751=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp751->break_lab= e->break_lab;
_temp751->continue_lab= e->continue_lab; _temp751->fallthru_info= e->fallthru_info;
_temp751->varmap= e->varmap; _temp751->toplevel= e->toplevel; _temp751;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp752=(* x).f1; struct Cyc_List_List*
_temp758; _LL754: if(( unsigned int) _temp752 > 1u?(( struct _tunion_struct*)
_temp752)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL759: _temp758=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp752)->f1; goto _LL755;} else{ goto _LL756;}
_LL756: goto _LL757; _LL755:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp760=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp760[ 0]=({ struct
Cyc_Core_Impossible_struct _temp761; _temp761.tag= Cyc_Core_Impossible_tag;
_temp761.f1=({ struct _tagged_string _temp762= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp762.last_plus_one - _temp762.curr,
_temp762.curr);}); _temp761;}); _temp760;})); _LL757: goto _LL753; _LL753:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple1* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* fallthru_vars= 0; for( 0; fallthru_binders
!= 0; fallthru_binders= fallthru_binders->tl){ fallthru_vars=({ struct Cyc_List_List*
_temp763=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp763->hd=( void*)(( struct Cyc_Absyn_Vardecl*) fallthru_binders->hd)->name;
_temp763->tl= fallthru_vars; _temp763;});} fallthru_vars=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fallthru_vars);{ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp764=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp764->v=(
void*) break_l; _temp764;}); ans->fallthru_info=({ struct Cyc_Core_Opt* _temp765=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp765->v=(
void*)({ struct _tuple8* _temp766=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp766->f1= fallthru_l; _temp766->f2= fallthru_vars; _temp766->f3=
next_case_env->varmap; _temp766;}); _temp765;}); return ans;}} static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env( struct Cyc_Toc_Env* e, struct _tagged_string*
break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp767=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp767->v=( void*) break_l; _temp767;}); ans->fallthru_info= 0;
return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env*
e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt* _temp768=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp768->v=( void*)({
struct _tuple8* _temp769=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp769->f1= next_l; _temp769->f2= 0; _temp769->f3=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp769;}); _temp768;}); return ans;} static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c(
void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List* args, struct Cyc_List_List*
typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp770=( char*)"expecting a literal format string";
struct _tagged_string _temp771; _temp771.curr= _temp770; _temp771.base= _temp770;
_temp771.last_plus_one= _temp770 + 34; _temp771;}));}{ struct _tagged_string
fmt_str;{ void* _temp772=( void*)(( struct Cyc_Absyn_Exp*) args->hd)->r; void*
_temp778; struct _tagged_string _temp780; _LL774: if((( struct _tunion_struct*)
_temp772)->tag == Cyc_Absyn_Const_e_tag){ _LL779: _temp778=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp772)->f1; if(( unsigned int) _temp778 > 1u?(( struct _tunion_struct*)
_temp778)->tag == Cyc_Absyn_String_c_tag: 0){ _LL781: _temp780=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp778)->f1; goto _LL775;}
else{ goto _LL776;}} else{ goto _LL776;} _LL776: goto _LL777; _LL775: fmt_str=
_temp780; goto _LL773; _LL777: return(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp782=(
char*)"expecting a literal format string"; struct _tagged_string _temp783;
_temp783.curr= _temp782; _temp783.base= _temp782; _temp783.last_plus_one=
_temp782 + 34; _temp783;})); _LL773:;}{ int len=( int)({ struct _tagged_string
_temp784= fmt_str;( unsigned int)( _temp784.last_plus_one - _temp784.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(( int)({ struct _tagged_string _temp785=
fmt_str; char* _temp787= _temp785.curr + i; if( _temp787 < _temp785.base? 1:
_temp787 >= _temp785.last_plus_one){ _throw( Null_Exception);}* _temp787;}) ==(
int)'%'){ i ++; if( i < len?( int)({ struct _tagged_string _temp788= fmt_str;
char* _temp790= _temp788.curr + i; if( _temp790 < _temp788.base? 1: _temp790 >=
_temp788.last_plus_one){ _throw( Null_Exception);}* _temp790;}) ==( int)'s': 0){
break;}}} if( i == len){ if( aopt != 0){ args=({ struct Cyc_List_List* _temp791=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp791->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp791->tl= args; _temp791;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args= args->tl; typs= typs->tl;{
struct Cyc_List_List* rev_fmt= 0; struct Cyc_List_List* rev_result= 0; struct
Cyc_List_List* rev_temps= 0; for( i= 0; i < len; i ++){ char c=({ struct
_tagged_string _temp792= fmt_str; char* _temp794= _temp792.curr + i; if(
_temp794 < _temp792.base? 1: _temp794 >= _temp792.last_plus_one){ _throw(
Null_Exception);}* _temp794;}); rev_fmt=({ struct Cyc_List_List* _temp795=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp795->hd=(
void*)(( int) c); _temp795->tl= rev_fmt; _temp795;}); if(( int) c !=( int)'%'){
continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp796=( char*)"bad format string"; struct
_tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 18; _temp797;}));}{ struct _tuple9 _temp800;
int _temp801; char _temp803; struct Cyc_List_List* _temp805; struct Cyc_List_List*
_temp807; struct Cyc_List_List* _temp809; struct Cyc_List_List* _temp811; struct
_tuple9* _temp798=( struct _tuple9*) x->v; _temp800=* _temp798; _LL812: _temp811=
_temp800.f1; goto _LL810; _LL810: _temp809= _temp800.f2; goto _LL808; _LL808:
_temp807= _temp800.f3; goto _LL806; _LL806: _temp805= _temp800.f4; goto _LL804;
_LL804: _temp803= _temp800.f5; goto _LL802; _LL802: _temp801= _temp800.f6; goto
_LL799; _LL799: i= _temp801 - 1; if(( int) _temp803 !=( int)'s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp813=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp813->hd=( void*) _temp811; _temp813->tl=({ struct Cyc_List_List* _temp814=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp814->hd=(
void*) _temp809; _temp814->tl=({ struct Cyc_List_List* _temp815=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp815->hd=( void*) _temp807;
_temp815->tl=({ struct Cyc_List_List* _temp816=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp816->hd=( void*) _temp805;
_temp816->tl= 0; _temp816;}); _temp815;}); _temp814;}); _temp813;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp817=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp817->hd=( void*)(( int) _temp803); _temp817->tl=
rev_fmt; _temp817;});{ struct Cyc_List_List* _temp818= _temp809; struct Cyc_List_List
_temp825; struct Cyc_List_List* _temp826; int _temp828; _LL820: if( _temp818 ==
0){ goto _LL822;} else{ _temp825=* _temp818; _LL829: _temp828=( int) _temp825.hd;
goto _LL827; _LL827: _temp826=( struct Cyc_List_List*) _temp825.tl; if( _temp826
== 0){ goto _LL824;} else{ goto _LL822;}} _LL824: if( _temp828 ==( int)'*'){
goto _LL821;} else{ goto _LL822;} _LL822: goto _LL823; _LL821: { struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp830=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp830->hd=( void*)({
struct _tuple10* _temp831=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp831->f1= temp; _temp831->f2=( void*) typs->hd; _temp831->f3=( struct Cyc_Absyn_Exp*)
args->hd; _temp831;}); _temp830->tl= rev_temps; _temp830;}); rev_result=({
struct Cyc_List_List* _temp832=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp832->hd=( void*) Cyc_Absyn_var_exp( temp, 0);
_temp832->tl= rev_result; _temp832;}); args= args->tl; typs= typs->tl; goto
_LL819;} _LL823: goto _LL819; _LL819:;}{ struct Cyc_List_List* _temp833=
_temp807; struct Cyc_List_List _temp840; struct Cyc_List_List* _temp841; struct
Cyc_List_List _temp843; struct Cyc_List_List* _temp844; int _temp846; int
_temp848; _LL835: if( _temp833 == 0){ goto _LL837;} else{ _temp840=* _temp833;
_LL849: _temp848=( int) _temp840.hd; goto _LL842; _LL842: _temp841=( struct Cyc_List_List*)
_temp840.tl; if( _temp841 == 0){ goto _LL837;} else{ _temp843=* _temp841; _LL847:
_temp846=( int) _temp843.hd; goto _LL845; _LL845: _temp844=( struct Cyc_List_List*)
_temp843.tl; if( _temp844 == 0){ goto _LL839;} else{ goto _LL837;}}} _LL839: if(
_temp848 ==( int)'.'? _temp846 ==( int)'*': 0){ goto _LL836;} else{ goto _LL837;}
_LL837: goto _LL838; _LL836: { struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp850=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp850->hd=( void*)({ struct _tuple10*
_temp851=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp851->f1=
temp; _temp851->f2=( void*) typs->hd; _temp851->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp851;}); _temp850->tl= rev_temps; _temp850;}); rev_result=({ struct Cyc_List_List*
_temp852=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp852->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp852->tl= rev_result;
_temp852;}); args= args->tl; typs= typs->tl; goto _LL834;} _LL838: goto _LL834;
_LL834:;} if(( int) _temp803 !=( int)'%'){ if( args == 0? 1: typs == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp853=
Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s", _temp853.last_plus_one
- _temp853.curr, _temp853.curr);}));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp854=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp854->hd=( void*)({ struct _tuple10*
_temp855=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp855->f1=
temp; _temp855->f2=( void*) typs->hd; _temp855->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp855;}); _temp854->tl= rev_temps; _temp854;}); rev_result=({ struct Cyc_List_List*
_temp856=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp856->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp856->tl= rev_result;
_temp856;}); args= args->tl; typs= typs->tl;}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp857=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp857->hd=( void*)(( int)'.'); _temp857->tl= rev_fmt; _temp857;}); rev_fmt=({
struct Cyc_List_List* _temp858=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp858->hd=( void*)(( int)'*'); _temp858->tl= rev_fmt;
_temp858;}); rev_fmt=({ struct Cyc_List_List* _temp859=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp859->hd=( void*)(( int)'s');
_temp859->tl= rev_fmt; _temp859;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp860=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp860->hd=( void*)({ struct _tuple10*
_temp861=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp861->f1=
temp; _temp861->f2=( void*) typs->hd; _temp861->f3=( struct Cyc_Absyn_Exp*) args->hd;
_temp861;}); _temp860->tl= rev_temps; _temp860;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp862=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp862->hd=( void*) size_exp; _temp862->tl= rev_result; _temp862;});
rev_result=({ struct Cyc_List_List* _temp863=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp863->hd=( void*) cstring_exp; _temp863->tl=
rev_result; _temp863;}); args= args->tl; typs= typs->tl;}}}}}}{ struct Cyc_List_List*
es=({ struct Cyc_List_List* _temp864=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp864->hd=( void*) Cyc_Absyn_string_exp( Cyc_String_implode(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( rev_fmt)), 0);
_temp864->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_result); _temp864;}); if( aopt != 0){ es=({ struct Cyc_List_List* _temp865=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp865->hd=(
void*)(( struct Cyc_Absyn_Exp*) aopt->v); _temp865->tl= es; _temp865;});}{
struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0; rev_temps= rev_temps->tl){
s= Cyc_Absyn_declare_stmt((*(( struct _tuple10*) rev_temps->hd)).f1,(*(( struct
_tuple10*) rev_temps->hd)).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)
rev_temps->hd)).f3, s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}} static void*
Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp866=( char*)"Missing type in primop "; struct _tagged_string _temp867;
_temp867.curr= _temp866; _temp867.base= _temp866; _temp867.last_plus_one=
_temp866 + 24; _temp867;}));} return Cyc_Toc_typ_to_c(( void*)( e->topt)->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp868=( char*)"Missing type in primop "; struct _tagged_string _temp869;
_temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 24; _temp869;}));} return( void*)( e->topt)->v;} static struct
_tuple5* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple5*
_temp870=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp870->f1=
Cyc_Toc_mt_tq; _temp870->f2= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); _temp870;});}
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e);
static struct _tuple7* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple7* _temp871=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp871[ 0]=({ struct
_tuple7 _temp872; _temp872.f1= 0; _temp872.f2= e; _temp872;}); _temp871;});}
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
_temp873=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp873[ 0]= xprintf("f%d", i); _temp873;}), 0),( struct Cyc_Absyn_Exp*) es->hd,
0), 0), s, 0);}} return Cyc_Toc_make_struct( x, Cyc_Absyn_strct( n), s, pointer,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_List_List* dles, struct _tuple1*
tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es= es->tl){ struct _tuple7 _temp876;
struct Cyc_Absyn_Exp* _temp877; struct Cyc_List_List* _temp879; struct _tuple7*
_temp874=( struct _tuple7*) es->hd; _temp876=* _temp874; _LL880: _temp879=
_temp876.f1; goto _LL878; _LL878: _temp877= _temp876.f2; goto _LL875; _LL875:
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)( _temp877->topt)->v): 0; if(
_temp879 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp881=( char*)"empty designator list"; struct
_tagged_string _temp882; _temp882.curr= _temp881; _temp882.base= _temp881;
_temp882.last_plus_one= _temp881 + 22; _temp882;}));} if( _temp879->tl != 0){
struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp(
y, 0); for( 0; _temp879 != 0; _temp879= _temp879->tl){ void* _temp883=( void*)
_temp879->hd; struct _tagged_string* _temp889; _LL885: if((( struct
_tunion_struct*) _temp883)->tag == Cyc_Absyn_FieldName_tag){ _LL890: _temp889=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp883)->f1;
goto _LL886;} else{ goto _LL887;} _LL887: goto _LL888; _LL886: if( Cyc_Toc_is_poly_field(
struct_type, _temp889)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp889, 0), yexp, 0), 0), s, 0); goto _LL884; _LL888:( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp891=( char*)"array designator in struct";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 27; _temp892;}))); _LL884:;} Cyc_Toc_exp_to_c(
nv, _temp877); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp877, 0), 0), s, 0);} else{ void* _temp893=( void*) _temp879->hd;
struct _tagged_string* _temp899; _LL895: if((( struct _tunion_struct*) _temp893)->tag
== Cyc_Absyn_FieldName_tag){ _LL900: _temp899=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp893)->f1; goto _LL896;} else{ goto _LL897;}
_LL897: goto _LL898; _LL896: Cyc_Toc_exp_to_c( nv, _temp877); if( Cyc_Toc_is_poly_field(
struct_type, _temp899)){ _temp877= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp877, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
lval_f( xexp, _temp899, 0), _temp877, 0), 0), s, 0); goto _LL894; _LL898:( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp901=( char*)"array designator in struct";
struct _tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 27; _temp902;}))); _LL894:;}}} return Cyc_Toc_make_struct(
x, Cyc_Absyn_strctq( tdn), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp903=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp903[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp904; _temp904.tag= Cyc_Absyn_Increment_e_tag;
_temp904.f1= e1; _temp904.f2=( void*) incr; _temp904;}); _temp903;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp905=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp915; struct Cyc_Absyn_Exp*
_temp917; void* _temp919; struct _tagged_string* _temp921; struct Cyc_Absyn_Exp*
_temp923; _LL907: if((( struct _tunion_struct*) _temp905)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL916: _temp915=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp905)->f1; goto _LL908;} else{ goto _LL909;} _LL909: if((( struct
_tunion_struct*) _temp905)->tag == Cyc_Absyn_Cast_e_tag){ _LL920: _temp919=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp905)->f1; goto _LL918; _LL918:
_temp917=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp905)->f2;
goto _LL910;} else{ goto _LL911;} _LL911: if((( struct _tunion_struct*) _temp905)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL924: _temp923=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp905)->f1; goto _LL922; _LL922:
_temp921=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp905)->f2; goto _LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL908:((
void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp915, fs, f, f_env); goto _LL906; _LL910:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp917, fs, f, f_env); goto
_LL906; _LL912:( void*)( e1->r=( void*)(( void*) _temp923->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp925=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp925->hd=( void*) _temp921; _temp925->tl= fs;
_temp925;}), f, f_env); goto _LL906; _LL914: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp926= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp926.last_plus_one - _temp926.curr, _temp926.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs= fs->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_string*) fs->hd, e1_copy->loc);}( void*)( e1->r=( void*)((
void*)( f( e1_copy, f_env))->r)); goto _LL906;} _LL906:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp927=( void*) s->r;
struct Cyc_Absyn_Exp* _temp937; struct Cyc_Absyn_Stmt* _temp939; struct Cyc_Absyn_Decl*
_temp941; struct Cyc_Absyn_Stmt* _temp943; struct Cyc_Absyn_Stmt* _temp945;
_LL929: if(( unsigned int) _temp927 > 1u?(( struct _tunion_struct*) _temp927)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL938: _temp937=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp927)->f1; goto _LL930;} else{ goto _LL931;} _LL931:
if(( unsigned int) _temp927 > 1u?(( struct _tunion_struct*) _temp927)->tag ==
Cyc_Absyn_Decl_s_tag: 0){ _LL942: _temp941=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp927)->f1; goto _LL940; _LL940: _temp939=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp927)->f2; goto _LL932;}
else{ goto _LL933;} _LL933: if(( unsigned int) _temp927 > 1u?(( struct
_tunion_struct*) _temp927)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL946: _temp945=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp927)->f1; goto
_LL944; _LL944: _temp943=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp927)->f2; goto _LL934;} else{ goto _LL935;} _LL935: goto _LL936; _LL930:((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp937, fs, f, f_env); goto _LL928; _LL932:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp939, fs, f, f_env); goto
_LL928; _LL934:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp943, fs, f, f_env); goto _LL928; _LL936:(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(({ struct _tagged_string _temp947= Cyc_Absynpp_stmt2string( s);
xprintf("lvalue_assign_stmt: %.*s", _temp947.last_plus_one - _temp947.curr,
_temp947.curr);})); goto _LL928; _LL928:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
struct Cyc_Absyn_Exp* arr, struct Cyc_Absyn_Exp* ind, struct Cyc_Absyn_Exp* esz,
void* elt_typ, struct Cyc_Absyn_Tqual* tq, int take_address){ unsigned int sz=
Cyc_Evexp_eval_const_uint_exp( esz); struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* i= Cyc_Toc_temp_var(); void* ta= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
elt_typ), tq); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte,
Cyc_Absyn_var_exp( i, 0), Cyc_Absyn_uint_exp( sz, 0), 0); struct Cyc_Absyn_Exp*
e4= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt*
s5= Cyc_Absyn_exp_stmt( e4, 0); struct Cyc_Absyn_Stmt* s4= Cyc_Absyn_ifthenelse_stmt(
e3, s5, Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_var_exp( i, 0), 0); if( take_address){ e5=
Cyc_Absyn_address_exp( e5, 0);}{ struct Cyc_Absyn_Stmt* s6= Cyc_Absyn_exp_stmt(
e5, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s4, s6, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( i, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) ind, s3,
0); return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( a, ta,( struct Cyc_Absyn_Exp*)
arr, s1, 0), 0);}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s); static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp948= Cyc_Absynpp_exp2string(
e); xprintf("exp_to_c: no type for %.*s", _temp948.last_plus_one - _temp948.curr,
_temp948.curr);})); return;}{ void* old_typ=( void*)( e->topt)->v; void*
_temp949= r; void* _temp1027; void* _temp1029; void* _temp1031; struct _tuple1*
_temp1033; struct _tuple1* _temp1035; struct Cyc_List_List* _temp1037; void*
_temp1039; void* _temp1041; struct Cyc_Absyn_Exp* _temp1043; struct Cyc_Absyn_Exp*
_temp1045; struct Cyc_Core_Opt* _temp1047; struct Cyc_Absyn_Exp* _temp1049;
struct Cyc_Absyn_Exp* _temp1051; struct Cyc_Absyn_Exp* _temp1053; struct Cyc_Absyn_Exp*
_temp1055; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Exp* _temp1059;
struct Cyc_List_List* _temp1061; struct Cyc_Absyn_Exp* _temp1063; struct Cyc_List_List*
_temp1065; struct Cyc_Absyn_Exp* _temp1067; struct Cyc_Absyn_Exp* _temp1069;
struct Cyc_Absyn_Exp* _temp1071; struct Cyc_List_List* _temp1073; struct Cyc_Absyn_Exp*
_temp1075; struct Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_Exp** _temp1079;
void* _temp1080; void** _temp1082; struct Cyc_Absyn_Exp* _temp1083; struct Cyc_Absyn_Exp*
_temp1085; void* _temp1087; struct Cyc_Absyn_Exp* _temp1089; struct Cyc_Absyn_Exp*
_temp1091; struct _tagged_string* _temp1093; struct Cyc_Absyn_Exp* _temp1095;
struct _tagged_string* _temp1097; struct Cyc_Absyn_Exp* _temp1099; struct Cyc_Absyn_Exp*
_temp1101; struct Cyc_Absyn_Exp* _temp1103; struct Cyc_List_List* _temp1105;
struct Cyc_List_List* _temp1107; struct _tuple2* _temp1109; struct Cyc_List_List*
_temp1111; struct Cyc_Absyn_Exp* _temp1113; struct Cyc_Absyn_Exp* _temp1115;
struct Cyc_Absyn_Vardecl* _temp1117; struct Cyc_Absyn_Structdecl* _temp1119;
struct Cyc_List_List* _temp1121; struct Cyc_Core_Opt* _temp1123; struct _tuple1*
_temp1125; struct Cyc_Absyn_Tunionfield* _temp1127; struct Cyc_Absyn_Tuniondecl*
_temp1129; struct Cyc_List_List* _temp1131; struct Cyc_Core_Opt* _temp1133;
struct Cyc_Core_Opt* _temp1135; struct Cyc_Absyn_Tunionfield* _temp1137; struct
Cyc_Absyn_Tuniondecl* _temp1139; struct Cyc_List_List* _temp1141; struct Cyc_Core_Opt*
_temp1143; struct Cyc_Core_Opt* _temp1145; struct Cyc_Absyn_Tunionfield*
_temp1147; struct Cyc_Absyn_XTuniondecl* _temp1149; struct Cyc_List_List*
_temp1151; struct Cyc_Core_Opt* _temp1153; struct Cyc_Absyn_Enumfield* _temp1155;
struct Cyc_Absyn_Enumdecl* _temp1157; struct _tuple1* _temp1159; void* _temp1161;
void* _temp1163; void* _temp1165; struct Cyc_Absyn_Stmt* _temp1167; struct Cyc_List_List*
_temp1169; struct Cyc_Core_Opt* _temp1171; struct Cyc_Absyn_Fndecl* _temp1173;
struct Cyc_Absyn_Exp* _temp1175; _LL951: if((( struct _tunion_struct*) _temp949)->tag
== Cyc_Absyn_Const_e_tag){ _LL1028: _temp1027=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp949)->f1; if(( int) _temp1027 == Cyc_Absyn_Null_c){ goto _LL952;} else{
goto _LL953;}} else{ goto _LL953;} _LL953: if((( struct _tunion_struct*)
_temp949)->tag == Cyc_Absyn_Const_e_tag){ _LL1030: _temp1029=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp949)->f1; goto _LL954;} else{ goto _LL955;}
_LL955: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Var_e_tag){
_LL1034: _temp1033=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp949)->f1;
goto _LL1032; _LL1032: _temp1031=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp949)->f2; goto _LL956;} else{ goto _LL957;} _LL957: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1036:
_temp1035=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp949)->f1;
goto _LL958;} else{ goto _LL959;} _LL959: if((( struct _tunion_struct*) _temp949)->tag
== Cyc_Absyn_Primop_e_tag){ _LL1040: _temp1039=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp949)->f1; goto _LL1038; _LL1038: _temp1037=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp949)->f2; goto _LL960;} else{ goto
_LL961;} _LL961: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Increment_e_tag){
_LL1044: _temp1043=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp949)->f1; goto _LL1042; _LL1042: _temp1041=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp949)->f2; goto _LL962;} else{ goto _LL963;} _LL963: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1050: _temp1049=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp949)->f1;
goto _LL1048; _LL1048: _temp1047=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp949)->f2; goto _LL1046; _LL1046: _temp1045=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp949)->f3; goto _LL964;} else{ goto
_LL965;} _LL965: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1056: _temp1055=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp949)->f1; goto _LL1054; _LL1054: _temp1053=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp949)->f2; goto _LL1052; _LL1052:
_temp1051=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp949)->f3; goto _LL966;} else{ goto _LL967;} _LL967: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1060: _temp1059=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp949)->f1; goto
_LL1058; _LL1058: _temp1057=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp949)->f2; goto _LL968;} else{ goto _LL969;} _LL969: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1064:
_temp1063=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp949)->f1; goto _LL1062; _LL1062: _temp1061=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp949)->f2; goto _LL970;} else{ goto
_LL971;} _LL971: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1068: _temp1067=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp949)->f1; goto _LL1066; _LL1066: _temp1065=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp949)->f2; goto _LL972;} else{ goto
_LL973;} _LL973: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Throw_e_tag){
_LL1070: _temp1069=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp949)->f1; goto _LL974;} else{ goto _LL975;} _LL975: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1072:
_temp1071=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp949)->f1; goto _LL976;} else{ goto _LL977;} _LL977: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1076:
_temp1075=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp949)->f1; goto _LL1074; _LL1074: _temp1073=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp949)->f2; goto _LL978;} else{ goto
_LL979;} _LL979: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Cast_e_tag){
_LL1081: _temp1080=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp949)->f1;
_temp1082=&(( struct Cyc_Absyn_Cast_e_struct*) _temp949)->f1; goto _LL1078;
_LL1078: _temp1077=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp949)->f2; _temp1079=&(( struct Cyc_Absyn_Cast_e_struct*) _temp949)->f2;
goto _LL980;} else{ goto _LL981;} _LL981: if((( struct _tunion_struct*) _temp949)->tag
== Cyc_Absyn_Address_e_tag){ _LL1084: _temp1083=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp949)->f1; goto _LL982;} else{ goto
_LL983;} _LL983: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_New_e_tag){
_LL1086: _temp1085=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp949)->f1; goto _LL984;} else{ goto _LL985;} _LL985: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1088:
_temp1087=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp949)->f1; goto
_LL986;} else{ goto _LL987;} _LL987: if((( struct _tunion_struct*) _temp949)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL1090: _temp1089=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp949)->f1; goto _LL988;} else{ goto
_LL989;} _LL989: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Deref_e_tag){
_LL1092: _temp1091=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp949)->f1; goto _LL990;} else{ goto _LL991;} _LL991: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1096:
_temp1095=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp949)->f1; goto _LL1094; _LL1094: _temp1093=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp949)->f2; goto _LL992;} else{ goto
_LL993;} _LL993: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1100: _temp1099=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp949)->f1; goto _LL1098; _LL1098: _temp1097=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp949)->f2; goto _LL994;} else{ goto
_LL995;} _LL995: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1104: _temp1103=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp949)->f1; goto _LL1102; _LL1102: _temp1101=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp949)->f2; goto _LL996;} else{ goto
_LL997;} _LL997: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1106: _temp1105=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp949)->f1; goto _LL998;} else{ goto _LL999;} _LL999: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1110:
_temp1109=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp949)->f1;
goto _LL1108; _LL1108: _temp1107=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp949)->f2; goto _LL1000;} else{ goto _LL1001;} _LL1001: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_Array_e_tag){ _LL1112: _temp1111=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp949)->f1; goto
_LL1002;} else{ goto _LL1003;} _LL1003: if((( struct _tunion_struct*) _temp949)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1118: _temp1117=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp949)->f1; goto _LL1116; _LL1116:
_temp1115=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp949)->f2; goto _LL1114; _LL1114: _temp1113=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp949)->f3; goto _LL1004;} else{
goto _LL1005;} _LL1005: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Struct_e_tag){
_LL1126: _temp1125=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp949)->f1; goto _LL1124; _LL1124: _temp1123=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp949)->f2; goto _LL1122; _LL1122: _temp1121=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp949)->f3; goto
_LL1120; _LL1120: _temp1119=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp949)->f4; goto _LL1006;} else{ goto _LL1007;} _LL1007: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1136: _temp1135=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f1; goto
_LL1134; _LL1134: _temp1133=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp949)->f2; goto _LL1132; _LL1132: _temp1131=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f3; if( _temp1131 == 0){ goto
_LL1130;} else{ goto _LL1009;} _LL1130: _temp1129=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f4; goto _LL1128; _LL1128:
_temp1127=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp949)->f5; goto _LL1008;} else{ goto _LL1009;} _LL1009: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1146: _temp1145=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f1; goto
_LL1144; _LL1144: _temp1143=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp949)->f2; goto _LL1142; _LL1142: _temp1141=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f3; goto _LL1140; _LL1140:
_temp1139=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp949)->f4; goto _LL1138; _LL1138: _temp1137=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f5; goto _LL1010;} else{ goto
_LL1011;} _LL1011: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_XTunion_e_tag){
_LL1154: _temp1153=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp949)->f1; goto _LL1152; _LL1152: _temp1151=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp949)->f2; goto _LL1150; _LL1150:
_temp1149=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp949)->f3; goto _LL1148; _LL1148: _temp1147=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp949)->f4; goto _LL1012;} else{ goto
_LL1013;} _LL1013: if((( struct _tunion_struct*) _temp949)->tag == Cyc_Absyn_Enum_e_tag){
_LL1160: _temp1159=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp949)->f1; goto _LL1158; _LL1158: _temp1157=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp949)->f2; goto _LL1156; _LL1156: _temp1155=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp949)->f3;
goto _LL1014;} else{ goto _LL1015;} _LL1015: if((( struct _tunion_struct*)
_temp949)->tag == Cyc_Absyn_Malloc_e_tag){ _LL1162: _temp1161=( void*)(( struct
Cyc_Absyn_Malloc_e_struct*) _temp949)->f1; if((( struct _tunion_struct*)
_temp1161)->tag == Cyc_Absyn_Typ_m_tag){ _LL1164: _temp1163=( void*)(( struct
Cyc_Absyn_Typ_m_struct*) _temp1161)->f1; goto _LL1016;} else{ goto _LL1017;}}
else{ goto _LL1017;} _LL1017: if((( struct _tunion_struct*) _temp949)->tag ==
Cyc_Absyn_Malloc_e_tag){ _LL1166: _temp1165=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp949)->f1; goto _LL1018;} else{ goto _LL1019;} _LL1019: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1168: _temp1167=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp949)->f1;
goto _LL1020;} else{ goto _LL1021;} _LL1021: if((( struct _tunion_struct*)
_temp949)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1172: _temp1171=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp949)->f1; goto
_LL1170; _LL1170: _temp1169=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp949)->f2; goto _LL1022;} else{ goto _LL1023;} _LL1023: if((( struct
_tunion_struct*) _temp949)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1174: _temp1173=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp949)->f1;
goto _LL1024;} else{ goto _LL1025;} _LL1025: if((( struct _tunion_struct*)
_temp949)->tag == Cyc_Absyn_Fill_e_tag){ _LL1176: _temp1175=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp949)->f1; goto _LL1026;} else{ goto _LL950;}
_LL952:{ void* _temp1177= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1183; struct Cyc_Absyn_Conref* _temp1185; struct Cyc_Absyn_Tqual* _temp1187;
struct Cyc_Absyn_Conref* _temp1189; void* _temp1191; void* _temp1193; _LL1179:
if(( unsigned int) _temp1177 > 4u?(( struct _tunion_struct*) _temp1177)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1184: _temp1183=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1177)->f1; _LL1194: _temp1193=( void*)
_temp1183.elt_typ; goto _LL1192; _LL1192: _temp1191=( void*) _temp1183.rgn_typ;
goto _LL1190; _LL1190: _temp1189=( struct Cyc_Absyn_Conref*) _temp1183.nullable;
goto _LL1188; _LL1188: _temp1187=( struct Cyc_Absyn_Tqual*) _temp1183.tq; goto
_LL1186; _LL1186: _temp1185=( struct Cyc_Absyn_Conref*) _temp1183.bounds; goto
_LL1180;} else{ goto _LL1181;} _LL1181: goto _LL1182; _LL1180:{ void* _temp1195=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1185); struct
Cyc_Absyn_Exp* _temp1201; _LL1197: if(( int) _temp1195 == Cyc_Absyn_Unknown_b){
goto _LL1198;} else{ goto _LL1199;} _LL1199: if(( unsigned int) _temp1195 > 1u?((
struct _tunion_struct*) _temp1195)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1202:
_temp1201=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1195)->f1;
goto _LL1200;} else{ goto _LL1196;} _LL1198: { struct _tuple7* zero=({ struct
_tuple7* _temp1203=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1203->f1= 0; _temp1203->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1203;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1204=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1204->hd=( void*) zero; _temp1204->tl=({ struct Cyc_List_List* _temp1205=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1205->hd=(
void*) zero; _temp1205->tl=({ struct Cyc_List_List* _temp1206=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1206->hd=( void*) zero;
_temp1206->tl= 0; _temp1206;}); _temp1205;}); _temp1204;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1193), Cyc_Toc_mt_tq)); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1196;} _LL1200:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1196; _LL1196:;} goto _LL1178; _LL1182:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1207= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1207.last_plus_one -
_temp1207.curr, _temp1207.curr);})); goto _LL1178; _LL1178:;} goto _LL950;
_LL954: goto _LL950; _LL956:{ struct _handler_cons _temp1208; _push_handler(&
_temp1208);{ struct _xtunion_struct* _temp1209=( struct _xtunion_struct*) setjmp(
_temp1208.handler); if( ! _temp1209){( void*)( e->r=( void*)(( void*)((( struct
Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
nv->varmap, _temp1033))->r));; _pop_handler();} else{ struct _xtunion_struct*
_temp1211= _temp1209; _LL1213: if( _temp1211->tag == Cyc_Dict_Absent_tag){ goto
_LL1214;} else{ goto _LL1215;} _LL1215: goto _LL1216; _LL1214:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1217= Cyc_Absynpp_qvar2string(
_temp1033); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1217.last_plus_one
- _temp1217.curr, _temp1217.curr);})); return; _LL1216:( void) _throw( _temp1211);
_LL1212:;}}} goto _LL950; _LL958:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1218=( char*)"unknownid"; struct
_tagged_string _temp1219; _temp1219.curr= _temp1218; _temp1219.base= _temp1218;
_temp1219.last_plus_one= _temp1218 + 10; _temp1219;})); goto _LL950; _LL960: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1037); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1037);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1037);{ void* _temp1220= _temp1039; _LL1222: if(( int) _temp1220 == Cyc_Absyn_Size){
goto _LL1223;} else{ goto _LL1224;} _LL1224: if(( int) _temp1220 == Cyc_Absyn_Printf){
goto _LL1225;} else{ goto _LL1226;} _LL1226: if(( int) _temp1220 == Cyc_Absyn_Xprintf){
goto _LL1227;} else{ goto _LL1228;} _LL1228: if(( int) _temp1220 == Cyc_Absyn_Fprintf){
goto _LL1229;} else{ goto _LL1230;} _LL1230: if(( int) _temp1220 == Cyc_Absyn_Sscanf){
goto _LL1231;} else{ goto _LL1232;} _LL1232: if(( int) _temp1220 == Cyc_Absyn_Plus){
goto _LL1233;} else{ goto _LL1234;} _LL1234: if(( int) _temp1220 == Cyc_Absyn_Minus){
goto _LL1235;} else{ goto _LL1236;} _LL1236: if(( int) _temp1220 == Cyc_Absyn_Eq){
goto _LL1237;} else{ goto _LL1238;} _LL1238: if(( int) _temp1220 == Cyc_Absyn_Neq){
goto _LL1239;} else{ goto _LL1240;} _LL1240: if(( int) _temp1220 == Cyc_Absyn_Gt){
goto _LL1241;} else{ goto _LL1242;} _LL1242: if(( int) _temp1220 == Cyc_Absyn_Gte){
goto _LL1243;} else{ goto _LL1244;} _LL1244: if(( int) _temp1220 == Cyc_Absyn_Lt){
goto _LL1245;} else{ goto _LL1246;} _LL1246: if(( int) _temp1220 == Cyc_Absyn_Lte){
goto _LL1247;} else{ goto _LL1248;} _LL1248: goto _LL1249; _LL1223: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*) _temp1037->hd;{ void* _temp1250=( void*)( arg->topt)->v;
struct Cyc_Absyn_Exp* _temp1258; struct Cyc_Absyn_Tqual* _temp1260; void*
_temp1262; struct Cyc_Absyn_PtrInfo _temp1264; struct Cyc_Absyn_Conref*
_temp1266; struct Cyc_Absyn_Tqual* _temp1268; struct Cyc_Absyn_Conref* _temp1270;
void* _temp1272; void* _temp1274; _LL1252: if(( unsigned int) _temp1250 > 4u?((
struct _tunion_struct*) _temp1250)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1263:
_temp1262=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1250)->f1; goto
_LL1261; _LL1261: _temp1260=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1250)->f2; goto _LL1259; _LL1259: _temp1258=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1250)->f3; goto _LL1253;} else{ goto
_LL1254;} _LL1254: if(( unsigned int) _temp1250 > 4u?(( struct _tunion_struct*)
_temp1250)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1265: _temp1264=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1250)->f1;
_LL1275: _temp1274=( void*) _temp1264.elt_typ; goto _LL1273; _LL1273: _temp1272=(
void*) _temp1264.rgn_typ; goto _LL1271; _LL1271: _temp1270=( struct Cyc_Absyn_Conref*)
_temp1264.nullable; goto _LL1269; _LL1269: _temp1268=( struct Cyc_Absyn_Tqual*)
_temp1264.tq; goto _LL1267; _LL1267: _temp1266=( struct Cyc_Absyn_Conref*)
_temp1264.bounds; goto _LL1255;} else{ goto _LL1256;} _LL1256: goto _LL1257;
_LL1253:( void*)( e->r=( void*)(( void*) _temp1258->r)); goto _LL1251; _LL1255: {
struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)( arg->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp, 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0);
struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, Cyc_Absyn_subtract_exp(
e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt( e4, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( temp,
temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1251;}
_LL1257:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1276= Cyc_Absynpp_typ2string(( void*) typs->hd); struct
_tagged_string _temp1277= Cyc_Absynpp_typ2string(( void*)( arg->topt)->v);
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1276.last_plus_one
- _temp1276.curr, _temp1276.curr, _temp1277.last_plus_one - _temp1277.curr,
_temp1277.curr);})); return; _LL1251:;} goto _LL1221;} _LL1225:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1039, 0, _temp1037, typs))->r)); goto
_LL1221; _LL1227:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1039, 0, _temp1037, typs))->r)); goto _LL1221; _LL1229: if( _temp1037 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1278=( char*)"fprintf without arguments"; struct _tagged_string
_temp1279; _temp1279.curr= _temp1278; _temp1279.base= _temp1278; _temp1279.last_plus_one=
_temp1278 + 26; _temp1279;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1039,({ struct Cyc_Core_Opt* _temp1280=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1280->v=( void*)(( struct Cyc_Absyn_Exp*)
_temp1037->hd); _temp1280;}), _temp1037->tl, typs->tl))->r)); goto _LL1221;
_LL1231: if( _temp1037 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1281=( char*)"sscanf without arguments";
struct _tagged_string _temp1282; _temp1282.curr= _temp1281; _temp1282.base=
_temp1281; _temp1282.last_plus_one= _temp1281 + 25; _temp1282;}));}( void*)(((
struct Cyc_Absyn_Exp*) _temp1037->hd)->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*) _temp1037->hd)->r,(( struct
Cyc_Absyn_Exp*) _temp1037->hd)->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1221;
_LL1233: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp1037->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp1037->tl)->hd; struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1283=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1283->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1283;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1221; _LL1235: if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp1037->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)( _temp1037->tl)->hd; void*
old_typ_e2=( void*)( old_typs->tl)->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));}{ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1284=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1284->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1284;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1221;
_LL1237: goto _LL1239; _LL1239: goto _LL1241; _LL1241: goto _LL1243; _LL1243:
goto _LL1245; _LL1245: goto _LL1247; _LL1247: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*) _temp1037->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(
_temp1037->tl)->hd; void* t1=( void*) old_typs->hd; void* t2=( void*)( old_typs->tl)->hd;
if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){( void*)( e1->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp,
0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ( t2)){( void*)( e2->r=( void*)((
void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} goto _LL1221;} _LL1249: goto _LL1221; _LL1221:;} goto _LL950;} _LL962: {
void* e2_cyc_typ=( void*)( _temp1043->topt)->v; Cyc_Toc_exp_to_c( nv, _temp1043);
if( Cyc_Tcutil_is_tagged_pointer_typ( e2_cyc_typ)){ if( _temp1041 ==( void*) Cyc_Absyn_PreInc?
1: _temp1041 ==( void*) Cyc_Absyn_PreDec){ struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp(
Cyc_Absyn_var_exp( temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0),
_temp1041, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1043, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{
struct _tuple1* temp= Cyc_Toc_temp_var(); struct _tuple1* temp2= Cyc_Toc_temp_var();
void* temp2_typ= Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0);
struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1041, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s,
0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
_temp1043, 0), s, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}}
else{ if( ! Cyc_Absyn_is_lvalue( _temp1043)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1043, 0, Cyc_Toc_incr_lvalue,
_temp1041);( void*)( e->r=( void*)(( void*) _temp1043->r));}} goto _LL950;}
_LL964: { int e1_poly= Cyc_Toc_is_poly_project( _temp1049); void* e1_old_typ=(
void*)( _temp1049->topt)->v; void* e2_old_typ=( void*)( _temp1045->topt)->v; Cyc_Toc_exp_to_c(
nv, _temp1049); Cyc_Toc_exp_to_c( nv, _temp1045); if( _temp1047 != 0?( void*)
_temp1047->v ==( void*) Cyc_Absyn_Plus? 1:( void*) _temp1047->v ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1049->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1049->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1045->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1045->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1045->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1045->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1049)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1049, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1285=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1285[ 0]=({ struct _tuple11 _temp1286; _temp1286.f1=
_temp1047; _temp1286.f2= _temp1045; _temp1286;}); _temp1285;}));( void*)( e->r=(
void*)(( void*) _temp1049->r));} goto _LL950;} _LL966: Cyc_Toc_exp_to_c( nv,
_temp1055); Cyc_Toc_exp_to_c( nv, _temp1053); Cyc_Toc_exp_to_c( nv, _temp1051);
goto _LL950; _LL968: Cyc_Toc_exp_to_c( nv, _temp1059); Cyc_Toc_exp_to_c( nv,
_temp1057); goto _LL950; _LL970: _temp1067= _temp1063; _temp1065= _temp1061;
goto _LL972; _LL972: Cyc_Toc_exp_to_c( nv, _temp1067);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1065); goto _LL950; _LL974: Cyc_Toc_exp_to_c(
nv, _temp1069);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1069), 0))->r)); goto _LL950; _LL976: Cyc_Toc_exp_to_c(
nv, _temp1071); goto _LL950; _LL978: Cyc_Toc_exp_to_c( nv, _temp1075);{ void*
t_inst= Cyc_Toc_typ_to_c(( void*)( e->topt)->v); for( 0; _temp1073 != 0;
_temp1073= _temp1073->tl){ void* k= Cyc_Tcutil_typ_kind(( void*) _temp1073->hd);
if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind: 0){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( t_inst, _temp1075, 0))->r));
break;}} goto _LL950;} _LL980: { void* old_t2=( void*)((* _temp1079)->topt)->v;
void* new_typ=* _temp1082;* _temp1082= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv,* _temp1079);* _temp1079= Cyc_Toc_convert_exp( nv->toplevel, old_t2, new_typ,*
_temp1079); goto _LL950;} _LL982:{ void* _temp1287=( void*) _temp1083->r; struct
Cyc_Absyn_Structdecl* _temp1295; struct Cyc_List_List* _temp1297; struct Cyc_Core_Opt*
_temp1299; struct _tuple1* _temp1301; struct Cyc_List_List* _temp1303; _LL1289:
if((( struct _tunion_struct*) _temp1287)->tag == Cyc_Absyn_Struct_e_tag){
_LL1302: _temp1301=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1287)->f1; goto _LL1300; _LL1300: _temp1299=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1287)->f2; goto _LL1298; _LL1298:
_temp1297=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1287)->f3; goto _LL1296; _LL1296: _temp1295=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1287)->f4; goto _LL1290;} else{ goto
_LL1291;} _LL1291: if((( struct _tunion_struct*) _temp1287)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1304: _temp1303=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1287)->f1; goto _LL1292;} else{ goto _LL1293;} _LL1293: goto _LL1294;
_LL1290: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1305= Cyc_Position_string_of_segment( _temp1083->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1305.last_plus_one -
_temp1305.curr, _temp1305.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)( _temp1083->topt)->v, 1, _temp1297, _temp1301))->r)); goto _LL1288;
_LL1292: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1306= Cyc_Position_string_of_segment( _temp1083->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1306.last_plus_one -
_temp1306.curr, _temp1306.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1303))->r)); goto _LL1288; _LL1294: Cyc_Toc_exp_to_c( nv, _temp1083);
if( ! Cyc_Absyn_is_lvalue( _temp1083)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int),
int f_env)) Cyc_Toc_lvalue_assign)( _temp1083, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*) _temp1083->r));} goto _LL1288; _LL1288:;} goto
_LL950; _LL984: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1307= Cyc_Position_string_of_segment( _temp1085->loc);
xprintf("%.*s: new at top-level", _temp1307.last_plus_one - _temp1307.curr,
_temp1307.curr);}));}{ void* _temp1308=( void*) _temp1085->r; struct Cyc_List_List*
_temp1322; void* _temp1324; struct _tagged_string _temp1326; struct Cyc_Absyn_Exp*
_temp1328; struct Cyc_Absyn_Exp* _temp1330; struct Cyc_Absyn_Vardecl* _temp1332;
struct Cyc_Absyn_Structdecl* _temp1334; struct Cyc_List_List* _temp1336; struct
Cyc_Core_Opt* _temp1338; struct _tuple1* _temp1340; struct Cyc_List_List*
_temp1342; _LL1310: if((( struct _tunion_struct*) _temp1308)->tag == Cyc_Absyn_Array_e_tag){
_LL1323: _temp1322=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1308)->f1; goto _LL1311;} else{ goto _LL1312;} _LL1312: if((( struct
_tunion_struct*) _temp1308)->tag == Cyc_Absyn_Const_e_tag){ _LL1325: _temp1324=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1308)->f1; if(( unsigned int)
_temp1324 > 1u?(( struct _tunion_struct*) _temp1324)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1327: _temp1326=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1324)->f1; goto _LL1313;} else{ goto _LL1314;}} else{ goto _LL1314;}
_LL1314: if((( struct _tunion_struct*) _temp1308)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL1333: _temp1332=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1308)->f1; goto _LL1331; _LL1331: _temp1330=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1308)->f2; goto _LL1329; _LL1329:
_temp1328=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1308)->f3; goto _LL1315;} else{ goto _LL1316;} _LL1316: if((( struct
_tunion_struct*) _temp1308)->tag == Cyc_Absyn_Struct_e_tag){ _LL1341: _temp1340=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1308)->f1; goto
_LL1339; _LL1339: _temp1338=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1308)->f2; goto _LL1337; _LL1337: _temp1336=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1308)->f3; goto _LL1335; _LL1335:
_temp1334=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1308)->f4; goto _LL1317;} else{ goto _LL1318;} _LL1318: if((( struct
_tunion_struct*) _temp1308)->tag == Cyc_Absyn_Tuple_e_tag){ _LL1343: _temp1342=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1308)->f1; goto
_LL1319;} else{ goto _LL1320;} _LL1320: goto _LL1321; _LL1311: { struct _tuple1*
x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
x, 0), 0); int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1322) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1322); for( 0; dles != 0; dles= dles->tl){
struct _tuple7 _temp1346; struct Cyc_Absyn_Exp* _temp1347; struct Cyc_List_List*
_temp1349; struct _tuple7* _temp1344=( struct _tuple7*) dles->hd; _temp1346=*
_temp1344; _LL1350: _temp1349= _temp1346.f1; goto _LL1348; _LL1348: _temp1347=
_temp1346.f2; goto _LL1345; _LL1345: { struct Cyc_Absyn_Exp* e_index; if(
_temp1349 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{ if(
_temp1349->tl != 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1351=( char*)"multiple designators in array";
struct _tagged_string _temp1352; _temp1352.curr= _temp1351; _temp1352.base=
_temp1351; _temp1352.last_plus_one= _temp1351 + 30; _temp1352;}));}{ void* d=(
void*) _temp1349->hd; void* _temp1353= d; struct Cyc_Absyn_Exp* _temp1359;
struct _tagged_string* _temp1361; _LL1355: if((( struct _tunion_struct*)
_temp1353)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1360: _temp1359=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1353)->f1; goto _LL1356;} else{ goto
_LL1357;} _LL1357: if((( struct _tunion_struct*) _temp1353)->tag == Cyc_Absyn_FieldName_tag){
_LL1362: _temp1361=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1353)->f1; goto _LL1358;} else{ goto _LL1354;} _LL1356: Cyc_Toc_exp_to_c(
nv, _temp1359); e_index= _temp1359; goto _LL1354; _LL1358: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1363=( char*)"field name designators in array"; struct _tagged_string
_temp1364; _temp1364.curr= _temp1363; _temp1364.base= _temp1363; _temp1364.last_plus_one=
_temp1363 + 32; _temp1364;})); goto _LL1354; _LL1354:;}} Cyc_Toc_exp_to_c( nv,
_temp1347); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1347, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1365= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1371; struct Cyc_Absyn_Conref* _temp1373; struct Cyc_Absyn_Tqual* _temp1375;
struct Cyc_Absyn_Conref* _temp1377; void* _temp1379; void* _temp1381; _LL1367:
if(( unsigned int) _temp1365 > 4u?(( struct _tunion_struct*) _temp1365)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1372: _temp1371=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1365)->f1; _LL1382: _temp1381=( void*)
_temp1371.elt_typ; goto _LL1380; _LL1380: _temp1379=( void*) _temp1371.rgn_typ;
goto _LL1378; _LL1378: _temp1377=( struct Cyc_Absyn_Conref*) _temp1371.nullable;
goto _LL1376; _LL1376: _temp1375=( struct Cyc_Absyn_Tqual*) _temp1371.tq; goto
_LL1374; _LL1374: _temp1373=( struct Cyc_Absyn_Conref*) _temp1371.bounds; goto
_LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370; _LL1368: old_elt_typ=
_temp1381; goto _LL1366; _LL1370: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1383=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1384; _temp1384.curr= _temp1383; _temp1384.base=
_temp1383; _temp1384.last_plus_one= _temp1383 + 52; _temp1384;})); goto _LL1366;
_LL1366:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1322), 0), 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ,
e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1309;}}} _LL1313:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct _tuple1*
_temp1385=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1385->f1=
Cyc_Toc_abs_ns; _temp1385->f2= Cyc_Toc_Cyc_new_string_sp; _temp1385;}), 0),({
struct Cyc_List_List* _temp1386=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1386->hd=( void*) Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Const_e_struct* _temp1387=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1387[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1388; _temp1388.tag= Cyc_Absyn_Const_e_tag;
_temp1388.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1389=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1389[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1390; _temp1390.tag= Cyc_Absyn_String_c_tag;
_temp1390.f1= _temp1326; _temp1390;}); _temp1389;})); _temp1388;}); _temp1387;}),
0); _temp1386->tl= 0; _temp1386;}), 0))->r)); goto _LL1309; _LL1315: { struct
_tuple1* x1= _temp1332->name; int is_tagged_ptr= 0;{ void* _temp1391= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1397; struct Cyc_Absyn_Conref* _temp1399;
struct Cyc_Absyn_Tqual* _temp1401; struct Cyc_Absyn_Conref* _temp1403; void*
_temp1405; void* _temp1407; _LL1393: if(( unsigned int) _temp1391 > 4u?(( struct
_tunion_struct*) _temp1391)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1398:
_temp1397=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1391)->f1; _LL1408: _temp1407=( void*) _temp1397.elt_typ; goto _LL1406;
_LL1406: _temp1405=( void*) _temp1397.rgn_typ; goto _LL1404; _LL1404: _temp1403=(
struct Cyc_Absyn_Conref*) _temp1397.nullable; goto _LL1402; _LL1402: _temp1401=(
struct Cyc_Absyn_Tqual*) _temp1397.tq; goto _LL1400; _LL1400: _temp1399=( struct
Cyc_Absyn_Conref*) _temp1397.bounds; goto _LL1394;} else{ goto _LL1395;} _LL1395:
goto _LL1396; _LL1394:{ void* _temp1409=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1399))->v; void*
_temp1415; _LL1411: if(( unsigned int) _temp1409 > 1u?(( struct _tunion_struct*)
_temp1409)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1416: _temp1415=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1409)->f1; if(( int) _temp1415 == Cyc_Absyn_Unknown_b){
goto _LL1412;} else{ goto _LL1413;}} else{ goto _LL1413;} _LL1413: goto _LL1414;
_LL1412: is_tagged_ptr= 1; goto _LL1410; _LL1414: goto _LL1410; _LL1410:;} goto
_LL1392; _LL1396:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1417=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1418; _temp1418.curr= _temp1417; _temp1418.base=
_temp1417; _temp1418.last_plus_one= _temp1417 + 42; _temp1418;})); goto _LL1392;
_LL1392:;}{ void* old_elt_typ=( void*)( _temp1328->topt)->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1330);{ struct Cyc_Toc_Env* nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp(
x1,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp1419=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1419[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1420; _temp1420.tag= Cyc_Absyn_Local_b_tag;
_temp1420.f1= _temp1332; _temp1420;}); _temp1419;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1328);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1328, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1421=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1421->hd=( void*)({ struct _tuple10* _temp1422=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1422->f1= max;
_temp1422->f2= Cyc_Absyn_uint_t; _temp1422->f3=( struct Cyc_Absyn_Exp*)
_temp1330; _temp1422;}); _temp1421->tl= 0; _temp1421;}); struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0)), 0); decls=({
struct Cyc_List_List* _temp1423=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1423->hd=( void*)({ struct _tuple10* _temp1424=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1424->f1= a;
_temp1424->f2= ptr_typ; _temp1424->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1424;});
_temp1423->tl= decls; _temp1423;}); decls=({ struct Cyc_List_List* _temp1425=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1425->hd=(
void*)({ struct _tuple10* _temp1426=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1426->f1= x1; _temp1426->f2= Cyc_Absyn_uint_t; _temp1426->f3=
0; _temp1426;}); _temp1425->tl= decls; _temp1425;}); if( is_tagged_ptr){ struct
_tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp*
binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1427=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1427->hd=(
void*)({ struct _tuple7* _temp1432=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp1432->f1= 0; _temp1432->f2= Cyc_Absyn_var_exp( a, 0); _temp1432;});
_temp1427->tl=({ struct Cyc_List_List* _temp1428=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1428->hd=( void*)({ struct
_tuple7* _temp1431=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1431->f1= 0; _temp1431->f2= Cyc_Absyn_var_exp( a, 0); _temp1431;});
_temp1428->tl=({ struct Cyc_List_List* _temp1429=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1429->hd=( void*)({ struct
_tuple7* _temp1430=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1430->f1= 0; _temp1430->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1430;}); _temp1429->tl= 0; _temp1429;});
_temp1428;}); _temp1427;}), 0); decls=({ struct Cyc_List_List* _temp1433=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1433->hd=(
void*)({ struct _tuple10* _temp1434=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1434->f1= b; _temp1434->f2= t; _temp1434->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1434;}); _temp1433->tl= decls; _temp1433;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Exp* _temp1437; void*
_temp1439; struct _tuple1* _temp1441; struct _tuple10 _temp1435=*(( struct
_tuple10*) ds->hd); _LL1442: _temp1441= _temp1435.f1; goto _LL1440; _LL1440:
_temp1439= _temp1435.f2; goto _LL1438; _LL1438: _temp1437= _temp1435.f3; goto
_LL1436; _LL1436: s= Cyc_Absyn_declare_stmt( _temp1441, _temp1439, _temp1437, s,
0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r)); goto
_LL1309;}}}} _LL1317:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,(
void*)( _temp1085->topt)->v, 1, _temp1336, _temp1340))->r)); goto _LL1309;
_LL1319:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1342))->r));
goto _LL1309; _LL1321: { void* old_elt_typ=( void*)( _temp1085->topt)->v; struct
_tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( x, 0), 0); struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp(
0, 0); Cyc_Toc_exp_to_c( nv, _temp1085); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1085, 0), s,
0);{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);
struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1309;}} _LL1309:;} goto _LL950;
_LL986:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1443=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1443[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1444; _temp1444.tag=
Cyc_Absyn_Sizeoftyp_e_tag; _temp1444.f1=( void*) Cyc_Toc_typ_to_c_array(
_temp1087); _temp1444;}); _temp1443;}))); goto _LL950; _LL988: Cyc_Toc_exp_to_c(
nv, _temp1089); goto _LL950; _LL990: Cyc_Toc_exp_to_c( nv, _temp1091);{ void*
_temp1445= Cyc_Tcutil_compress(( void*)( _temp1091->topt)->v); struct Cyc_Absyn_PtrInfo
_temp1451; struct Cyc_Absyn_Conref* _temp1453; struct Cyc_Absyn_Tqual* _temp1455;
struct Cyc_Absyn_Conref* _temp1457; void* _temp1459; void* _temp1461; _LL1447:
if(( unsigned int) _temp1445 > 4u?(( struct _tunion_struct*) _temp1445)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1452: _temp1451=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1445)->f1; _LL1462: _temp1461=( void*)
_temp1451.elt_typ; goto _LL1460; _LL1460: _temp1459=( void*) _temp1451.rgn_typ;
goto _LL1458; _LL1458: _temp1457=( struct Cyc_Absyn_Conref*) _temp1451.nullable;
goto _LL1456; _LL1456: _temp1455=( struct Cyc_Absyn_Tqual*) _temp1451.tq; goto
_LL1454; _LL1454: _temp1453=( struct Cyc_Absyn_Conref*) _temp1451.bounds; goto
_LL1448;} else{ goto _LL1449;} _LL1449: goto _LL1450; _LL1448:{ void* _temp1463=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1453); struct
Cyc_Absyn_Exp* _temp1469; _LL1465: if(( int) _temp1463 == Cyc_Absyn_Unknown_b){
goto _LL1466;} else{ goto _LL1467;} _LL1467: if(( unsigned int) _temp1463 > 1u?((
struct _tunion_struct*) _temp1463)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1470:
_temp1469=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1463)->f1;
goto _LL1468;} else{ goto _LL1464;} _LL1466: { struct _tuple1* x= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array(
_temp1461); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1,
_temp1455); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1,
_temp1455); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1091, s1, 0), 0))->r));
goto _LL1464;} _LL1468: { int nullable= Cyc_Toc_is_nullable(( void*)( _temp1091->topt)->v);
if( nullable){ struct _tuple1* x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c((
void*)( _temp1091->topt)->v); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( _temp1469->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1091, s3, 0), 0))->r));} goto _LL1464;} _LL1464:;} goto _LL1446; _LL1450:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1471=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1472; _temp1472.curr= _temp1471; _temp1472.base= _temp1471; _temp1472.last_plus_one=
_temp1471 + 29; _temp1472;})); goto _LL1446; _LL1446:;} goto _LL950; _LL992: Cyc_Toc_exp_to_c(
nv, _temp1095); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v), Cyc_Absyn_new_exp((
void*) e->r, 0), 0))->r));} goto _LL950; _LL994: Cyc_Toc_exp_to_c( nv, _temp1099);
if( Cyc_Toc_is_nullable(( void*)( _temp1099->topt)->v)){ struct _tuple1* x= Cyc_Toc_temp_var();
void* tx= Cyc_Toc_typ_to_c(( void*)( _temp1099->topt)->v); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_signed_int_exp( 0, 0),
0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp(
0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt( e2, Cyc_Absyn_exp_stmt(
e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_exp_stmt(
Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0), _temp1097, 0), 0); struct
Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1099, s3, 0), 0))->r));} else{ if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)( e->topt)->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}} goto _LL950; _LL996: { void*
e1_typ= Cyc_Tcutil_compress(( void*)( _temp1103->topt)->v); Cyc_Toc_exp_to_c( nv,
_temp1103); Cyc_Toc_exp_to_c( nv, _temp1101);( void*)( _temp1103->r=( void*)((
void*)( Cyc_Toc_array_to_ptr( Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp((
void*) _temp1103->r, 0), 0))->r));{ void* _temp1473= e1_typ; struct Cyc_List_List*
_temp1483; struct Cyc_Absyn_Exp* _temp1485; struct Cyc_Absyn_Tqual* _temp1487;
void* _temp1489; struct Cyc_Absyn_PtrInfo _temp1491; struct Cyc_Absyn_Conref*
_temp1493; struct Cyc_Absyn_Tqual* _temp1495; struct Cyc_Absyn_Conref* _temp1497;
void* _temp1499; void* _temp1501; _LL1475: if(( unsigned int) _temp1473 > 4u?((
struct _tunion_struct*) _temp1473)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1484:
_temp1483=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1473)->f1; goto _LL1476;} else{ goto _LL1477;} _LL1477: if(( unsigned int)
_temp1473 > 4u?(( struct _tunion_struct*) _temp1473)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1490: _temp1489=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1473)->f1;
goto _LL1488; _LL1488: _temp1487=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1473)->f2; goto _LL1486; _LL1486: _temp1485=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1473)->f3; goto _LL1478;} else{ goto
_LL1479;} _LL1479: if(( unsigned int) _temp1473 > 4u?(( struct _tunion_struct*)
_temp1473)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1492: _temp1491=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1473)->f1;
_LL1502: _temp1501=( void*) _temp1491.elt_typ; goto _LL1500; _LL1500: _temp1499=(
void*) _temp1491.rgn_typ; goto _LL1498; _LL1498: _temp1497=( struct Cyc_Absyn_Conref*)
_temp1491.nullable; goto _LL1496; _LL1496: _temp1495=( struct Cyc_Absyn_Tqual*)
_temp1491.tq; goto _LL1494; _LL1494: _temp1493=( struct Cyc_Absyn_Conref*)
_temp1491.bounds; goto _LL1480;} else{ goto _LL1481;} _LL1481: goto _LL1482;
_LL1476: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1101) +(
unsigned int) 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1103,({ struct _tagged_string* _temp1503=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1503[ 0]= xprintf("f%d",( int)
i); _temp1503;}), 0))->r)); goto _LL1474;} _LL1478: if( _temp1485 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1504=( char*)"exp_to_c: Subscript_e array type bound null"; struct
_tagged_string _temp1505; _temp1505.curr= _temp1504; _temp1505.base= _temp1504;
_temp1505.last_plus_one= _temp1504 + 44; _temp1505;}));}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1103, _temp1101,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1506= _temp1485; if( _temp1506 == 0){ _throw(
Null_Exception);} _temp1506;}), _temp1489, _temp1487, 0))->r)); goto _LL1474;
_LL1480:{ void* _temp1507=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1493); struct Cyc_Absyn_Exp* _temp1513; _LL1509: if(( unsigned int)
_temp1507 > 1u?(( struct _tunion_struct*) _temp1507)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1514: _temp1513=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1507)->f1; goto _LL1510;} else{ goto _LL1511;} _LL1511: if(( int) _temp1507
== Cyc_Absyn_Unknown_b){ goto _LL1512;} else{ goto _LL1508;} _LL1510:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_known_size_subscript( _temp1103, _temp1101, _temp1513,
_temp1501, _temp1495, 0))->r)); goto _LL1508; _LL1512: { struct _tuple1* x= Cyc_Toc_temp_var();
struct _tuple1* i= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
void* ta1= Cyc_Toc_typ_to_c_array( _temp1501); struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
ta1, _temp1495); void* tx= Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1495); struct Cyc_Absyn_Exp* test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1101, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1103, s1, 0), 0))->r)); goto _LL1508;} _LL1508:;}
goto _LL1474; _LL1482:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1515=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1516; _temp1516.curr= _temp1515; _temp1516.base=
_temp1515; _temp1516.last_plus_one= _temp1515 + 49; _temp1516;})); goto _LL1474;
_LL1474:;} goto _LL950;} _LL998: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp1105))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1105); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1105 != 0; _temp1105= _temp1105->tl, i ++){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) _temp1105->hd);{ struct Cyc_List_List* des=({ struct
Cyc_List_List* _temp1517=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1517->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp1518=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp1518[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp1519; _temp1519.tag= Cyc_Absyn_FieldName_tag;
_temp1519.f1=({ struct _tagged_string* _temp1520=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1520[ 0]= xprintf("f%d", i);
_temp1520;}); _temp1519;}); _temp1518;})); _temp1517->tl= 0; _temp1517;}); dles=({
struct Cyc_List_List* _temp1521=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1521->hd=( void*)({ struct _tuple7* _temp1522=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1522->f1=( struct Cyc_List_List*)
des; _temp1522->f2=( struct Cyc_Absyn_Exp*) _temp1105->hd; _temp1522;});
_temp1521->tl= dles; _temp1521;});}}} dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0, dles, 0),
0))->r));} goto _LL950; _LL1000:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1523=( char*)"compoundlit"; struct
_tagged_string _temp1524; _temp1524.curr= _temp1523; _temp1524.base= _temp1523;
_temp1524.last_plus_one= _temp1523 + 12; _temp1524;})); goto _LL950; _LL1002:(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1111, 0))->r));{
struct Cyc_List_List* dles= _temp1111; for( 0; dles != 0; dles= dles->tl){
struct _tuple7 _temp1527; struct Cyc_Absyn_Exp* _temp1528; struct Cyc_List_List*
_temp1530; struct _tuple7* _temp1525=( struct _tuple7*) dles->hd; _temp1527=*
_temp1525; _LL1531: _temp1530= _temp1527.f1; goto _LL1529; _LL1529: _temp1528=
_temp1527.f2; goto _LL1526; _LL1526: Cyc_Toc_exp_to_c( nv, _temp1528);}} goto
_LL950; _LL1004: { unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1115);
Cyc_Toc_exp_to_c( nv, _temp1113);{ struct Cyc_List_List* es= 0;{ unsigned int i=
0; for( 0; i < sz; i ++){ es=({ struct Cyc_List_List* _temp1532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1532->hd=( void*)({ struct
_tuple7* _temp1533=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1533->f1= 0; _temp1533->f2= _temp1113; _temp1533;}); _temp1532->tl= es;
_temp1532;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
es, 0))->r)); goto _LL950;}} _LL1006: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv, old_typ, 0, _temp1121, _temp1125))->r));} else{
struct _tuple1* n= _temp1125; struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1535= _temp1119 == 0?(( struct Cyc_Absyn_Structdecl*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1534=
Cyc_Absynpp_qvar2string( _temp1125); xprintf("no structdecl %.*s", _temp1534.last_plus_one
- _temp1534.curr, _temp1534.curr);})): _temp1119; if( _temp1535 == 0){ _throw(
Null_Exception);} _temp1535;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1121,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields= fields->tl){ struct _tuple12 _temp1538;
struct Cyc_Absyn_Exp* _temp1539; struct Cyc_Absyn_Structfield* _temp1541; struct
_tuple12* _temp1536=( struct _tuple12*) fields->hd; _temp1538=* _temp1536;
_LL1542: _temp1541= _temp1538.f1; goto _LL1540; _LL1540: _temp1539= _temp1538.f2;
goto _LL1537; _LL1537: { void* fieldtyp=( void*) _temp1541->type; Cyc_Toc_exp_to_c(
nv, _temp1539); if( Cyc_Toc_is_void_star( fieldtyp)){( void*)( _temp1539->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1539->r, 0), 0))->r));}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0, _temp1121, 0), 0))->r));}
goto _LL950; _LL1008: { struct _tuple1* qv= _temp1127->name; if( ! nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));} else{ int
tag_count= 0; struct Cyc_List_List* fields= _temp1129->fields == 0? 0:( struct
Cyc_List_List*)( _temp1129->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,(( struct
Cyc_Absyn_Tunionfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)
fields->hd)->typs == 0){ tag_count ++;} fields= fields->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL950;} _LL1010: { struct _tuple1* qv= _temp1137->name; struct Cyc_List_List*
tqts= _temp1137->typs; struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1543=( char*)"_struct"; struct
_tagged_string _temp1544; _temp1544.curr= _temp1543; _temp1544.base= _temp1543;
_temp1544.last_plus_one= _temp1543 + 8; _temp1544;}))); if( nv->toplevel){ int
tag_count= 0; struct Cyc_List_List* fields= _temp1139->fields == 0? 0:( struct
Cyc_List_List*)( _temp1139->fields)->v; while( Cyc_Absyn_qvar_cmp( qv,(( struct
Cyc_Absyn_Tunionfield*) fields->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)
fields->hd)->typs != 0){ tag_count ++;} fields= fields->tl;}{ struct Cyc_List_List*
dles= 0; for( 0; _temp1141 != 0; _temp1141= _temp1141->tl, tqts= tqts->tl){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1141->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple5*) tqts->hd)).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);} dles=({ struct Cyc_List_List* _temp1545=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1545[ 0]=({ struct Cyc_List_List
_temp1546; _temp1546.hd=( void*)({ struct _tuple7* _temp1547=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7) * 1); _temp1547[ 0]=({ struct _tuple7
_temp1548; _temp1548.f1= 0; _temp1548.f2= cur_e; _temp1548;}); _temp1547;});
_temp1546.tl= dles; _temp1546;}); _temp1545;});}{ struct Cyc_Absyn_Exp* tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); dles=({ struct Cyc_List_List*
_temp1549=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1549[ 0]=({ struct Cyc_List_List _temp1550; _temp1550.hd=( void*)({ struct
_tuple7* _temp1551=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp1551[ 0]=({ struct _tuple7 _temp1552; _temp1552.f1= 0; _temp1552.f2=
tag_exp; _temp1552;}); _temp1551;}); _temp1550.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1550;}); _temp1549;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1553=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1553->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1554=( char*)"_struct"; struct _tagged_string _temp1555; _temp1555.curr=
_temp1554; _temp1555.base= _temp1554; _temp1555.last_plus_one= _temp1554 + 8;
_temp1555;})); _temp1553;}), dles, 0))->r));}}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1556=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1556->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1557=( char*)"_tag"; struct
_tagged_string _temp1558; _temp1558.curr= _temp1557; _temp1558.base= _temp1557;
_temp1558.last_plus_one= _temp1557 + 5; _temp1558;})), 0), 0); _temp1556->tl= 0;
_temp1556;});{ int i= 1; for( 0; _temp1141 != 0;( _temp1141= _temp1141->tl, i ++),
tqts= tqts->tl){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1141->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple5*) tqts->hd)).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1559=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1559[ 0]= xprintf("f%d", i); _temp1559;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1560=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1560->hd=( void*) a; _temp1560->tl=
rev_assign; _temp1560;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1561=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1561->hd=(
void*) final_stmt; _temp1561->tl= rev_assign; _temp1561;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL950;} _LL1012: { struct _tuple1* qv= _temp1147->name;
struct Cyc_List_List* tqts= _temp1147->typs; struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1562=( char*)"_tag";
struct _tagged_string _temp1563; _temp1563.curr= _temp1562; _temp1563.base=
_temp1562; _temp1563.last_plus_one= _temp1562 + 5; _temp1563;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1151); dles=({ struct Cyc_List_List*
_temp1564=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1564->hd=( void*)({ struct _tuple7* _temp1565=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1565->f1= 0; _temp1565->f2= tag_exp; _temp1565;});
_temp1564->tl= dles; _temp1564;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1566=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1566->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1567=( char*)"_struct"; struct _tagged_string
_temp1568; _temp1568.curr= _temp1567; _temp1568.base= _temp1567; _temp1568.last_plus_one=
_temp1567 + 8; _temp1568;})); _temp1566;}), dles, 0))->r));} else{ struct
_tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1569=( char*)"_struct"; struct _tagged_string
_temp1570; _temp1570.curr= _temp1569; _temp1570.base= _temp1569; _temp1570.last_plus_one=
_temp1569 + 8; _temp1570;}))); struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1571=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1571->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1572=( char*)"_tag"; struct _tagged_string
_temp1573; _temp1573.curr= _temp1572; _temp1573.base= _temp1572; _temp1573.last_plus_one=
_temp1572 + 5; _temp1573;})), 0), 0); _temp1571->tl= 0; _temp1571;});{ int i= 1;
for( 0; _temp1151 != 0;( _temp1151= _temp1151->tl, i ++), tqts= tqts->tl){
struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*) _temp1151->hd; void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple5*) tqts->hd)).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1574=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1574[ 0]= xprintf("f%d", i); _temp1574;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1575->hd=( void*) a; _temp1575->tl=
rev_assign; _temp1575;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1576=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1576->hd=(
void*) final_stmt; _temp1576->tl= rev_assign; _temp1576;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL950;} _LL1014: goto _LL950; _LL1016: { void* t_c= Cyc_Toc_typ_to_c(
_temp1163);( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1163, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r)); goto _LL950;} _LL1018:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1577=( char*)"unimplemented Malloc_e form";
struct _tagged_string _temp1578; _temp1578.curr= _temp1577; _temp1578.base=
_temp1577; _temp1578.last_plus_one= _temp1577 + 28; _temp1578;})); goto _LL950;
_LL1020: Cyc_Toc_stmt_to_c( nv, _temp1167); goto _LL950; _LL1022:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1579=( char*)"UnresolvedMem"; struct _tagged_string _temp1580; _temp1580.curr=
_temp1579; _temp1580.base= _temp1579; _temp1580.last_plus_one= _temp1579 + 14;
_temp1580;})); goto _LL950; _LL1024:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1581=( char*)"codegen"; struct
_tagged_string _temp1582; _temp1582.curr= _temp1581; _temp1582.base= _temp1581;
_temp1582.last_plus_one= _temp1581 + 8; _temp1582;})); goto _LL950; _LL1026:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1583=( char*)"fill"; struct _tagged_string _temp1584; _temp1584.curr=
_temp1583; _temp1584.base= _temp1583; _temp1584.last_plus_one= _temp1583 + 5;
_temp1584;})); goto _LL950; _LL950:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_string*
succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1585=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1625; struct Cyc_Absyn_Vardecl _temp1627; struct Cyc_List_List* _temp1628;
struct Cyc_Core_Opt* _temp1630; int _temp1632; struct Cyc_Absyn_Exp* _temp1634;
void* _temp1636; struct Cyc_Absyn_Tqual* _temp1638; struct _tuple1* _temp1640;
void* _temp1642; struct Cyc_Absyn_Vardecl* _temp1644; int _temp1646; void*
_temp1648; char _temp1650; struct _tagged_string _temp1652; struct Cyc_Absyn_Enumfield*
_temp1654; struct Cyc_Absyn_Enumdecl* _temp1656; struct _tuple1* _temp1658;
struct Cyc_Absyn_Tunionfield* _temp1660; struct Cyc_Absyn_Tuniondecl* _temp1662;
struct Cyc_List_List* _temp1664; struct Cyc_List_List* _temp1666; struct Cyc_Core_Opt*
_temp1668; struct _tuple1* _temp1670; struct Cyc_Absyn_Pat* _temp1672; struct
Cyc_Absyn_Pat _temp1674; struct Cyc_Position_Segment* _temp1675; struct Cyc_Core_Opt*
_temp1677; void* _temp1679; struct Cyc_Absyn_Tunionfield* _temp1681; struct Cyc_Absyn_Tuniondecl*
_temp1683; struct Cyc_List_List* _temp1685; struct Cyc_List_List* _temp1687;
struct Cyc_Core_Opt* _temp1689; struct _tuple1* _temp1691; struct Cyc_Absyn_Pat*
_temp1693; struct Cyc_Absyn_Pat _temp1695; struct Cyc_Position_Segment*
_temp1696; struct Cyc_Core_Opt* _temp1698; void* _temp1700; struct Cyc_Absyn_Tunionfield*
_temp1702; struct Cyc_Absyn_XTuniondecl* _temp1704; struct Cyc_List_List*
_temp1706; struct Cyc_List_List* _temp1708; struct _tuple1* _temp1710; struct
Cyc_List_List* _temp1712; struct Cyc_List_List* _temp1714; struct Cyc_List_List*
_temp1716; struct Cyc_Core_Opt* _temp1718; struct Cyc_Absyn_Structdecl*
_temp1720; struct Cyc_Absyn_Pat* _temp1722; struct _tuple1* _temp1724; struct
Cyc_List_List* _temp1726; struct Cyc_List_List* _temp1728; struct _tuple1*
_temp1730; struct Cyc_List_List* _temp1732; struct Cyc_List_List* _temp1734;
struct _tuple1* _temp1736; struct Cyc_Absyn_Tunionfield* _temp1738; struct Cyc_Absyn_Tuniondecl*
_temp1740; struct Cyc_List_List* _temp1742; struct Cyc_List_List* _temp1744;
struct Cyc_Core_Opt* _temp1746; struct _tuple1* _temp1748; struct Cyc_Absyn_Tunionfield*
_temp1750; struct Cyc_Absyn_XTuniondecl* _temp1752; struct Cyc_List_List*
_temp1754; struct Cyc_List_List* _temp1756; struct _tuple1* _temp1758; _LL1587:
if(( int) _temp1585 == Cyc_Absyn_Wild_p){ goto _LL1588;} else{ goto _LL1589;}
_LL1589: if(( unsigned int) _temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag
== Cyc_Absyn_Var_p_tag: 0){ _LL1626: _temp1625=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1585)->f1; _temp1627=* _temp1625; _LL1643:
_temp1642=( void*) _temp1627.sc; goto _LL1641; _LL1641: _temp1640=( struct
_tuple1*) _temp1627.name; goto _LL1639; _LL1639: _temp1638=( struct Cyc_Absyn_Tqual*)
_temp1627.tq; goto _LL1637; _LL1637: _temp1636=( void*) _temp1627.type; goto
_LL1635; _LL1635: _temp1634=( struct Cyc_Absyn_Exp*) _temp1627.initializer; goto
_LL1633; _LL1633: _temp1632=( int) _temp1627.shadow; goto _LL1631; _LL1631:
_temp1630=( struct Cyc_Core_Opt*) _temp1627.region; goto _LL1629; _LL1629:
_temp1628=( struct Cyc_List_List*) _temp1627.attributes; goto _LL1590;} else{
goto _LL1591;} _LL1591: if(( unsigned int) _temp1585 > 2u?(( struct
_tunion_struct*) _temp1585)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL1645:
_temp1644=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1585)->f1; goto _LL1592;} else{ goto _LL1593;} _LL1593: if(( int) _temp1585
== Cyc_Absyn_Null_p){ goto _LL1594;} else{ goto _LL1595;} _LL1595: if((
unsigned int) _temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1649: _temp1648=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1585)->f1;
goto _LL1647; _LL1647: _temp1646=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1585)->f2; goto _LL1596;} else{ goto _LL1597;} _LL1597: if(( unsigned int)
_temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1651: _temp1650=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1585)->f1;
goto _LL1598;} else{ goto _LL1599;} _LL1599: if(( unsigned int) _temp1585 > 2u?((
struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1653:
_temp1652=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1585)->f1;
goto _LL1600;} else{ goto _LL1601;} _LL1601: if(( unsigned int) _temp1585 > 2u?((
struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1659:
_temp1658=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1585)->f1;
goto _LL1657; _LL1657: _temp1656=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1585)->f2; goto _LL1655; _LL1655: _temp1654=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1585)->f3; goto _LL1602;} else{ goto
_LL1603;} _LL1603: if(( unsigned int) _temp1585 > 2u?(( struct _tunion_struct*)
_temp1585)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1671: _temp1670=( struct
_tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1585)->f1; goto _LL1669;
_LL1669: _temp1668=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1585)->f2; goto _LL1667; _LL1667: _temp1666=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1585)->f3; goto _LL1665; _LL1665:
_temp1664=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1585)->f4; if( _temp1664 == 0){ goto _LL1663;} else{ goto _LL1605;} _LL1663:
_temp1662=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1585)->f5; goto _LL1661; _LL1661: _temp1660=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1585)->f6; goto _LL1604;} else{ goto
_LL1605;} _LL1605: if(( unsigned int) _temp1585 > 2u?(( struct _tunion_struct*)
_temp1585)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1673: _temp1672=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1585)->f1; _temp1674=* _temp1672;
_LL1680: _temp1679=( void*) _temp1674.r; if(( unsigned int) _temp1679 > 2u?((
struct _tunion_struct*) _temp1679)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1692:
_temp1691=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1679)->f1;
goto _LL1690; _LL1690: _temp1689=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1679)->f2; goto _LL1688; _LL1688: _temp1687=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1679)->f3; goto _LL1686; _LL1686:
_temp1685=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1679)->f4; goto _LL1684; _LL1684: _temp1683=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1679)->f5; goto _LL1682; _LL1682:
_temp1681=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1679)->f6; goto _LL1678;} else{ goto _LL1607;} _LL1678: _temp1677=( struct
Cyc_Core_Opt*) _temp1674.topt; goto _LL1676; _LL1676: _temp1675=( struct Cyc_Position_Segment*)
_temp1674.loc; goto _LL1606;} else{ goto _LL1607;} _LL1607: if(( unsigned int)
_temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1694: _temp1693=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1585)->f1; _temp1695=* _temp1693; _LL1701: _temp1700=( void*) _temp1695.r;
if(( unsigned int) _temp1700 > 2u?(( struct _tunion_struct*) _temp1700)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL1711: _temp1710=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1700)->f1; goto _LL1709; _LL1709: _temp1708=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1700)->f2; goto _LL1707; _LL1707:
_temp1706=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1700)->f3; goto _LL1705; _LL1705: _temp1704=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1700)->f4; goto _LL1703; _LL1703:
_temp1702=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1700)->f5; goto _LL1699;} else{ goto _LL1609;} _LL1699: _temp1698=( struct
Cyc_Core_Opt*) _temp1695.topt; goto _LL1697; _LL1697: _temp1696=( struct Cyc_Position_Segment*)
_temp1695.loc; goto _LL1608;} else{ goto _LL1609;} _LL1609: if(( unsigned int)
_temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1713: _temp1712=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1585)->f1; goto _LL1610;} else{ goto _LL1611;} _LL1611: if(( unsigned int)
_temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1721: _temp1720=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1585)->f1; goto _LL1719; _LL1719: _temp1718=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1585)->f2; goto _LL1717; _LL1717:
_temp1716=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1585)->f3; goto _LL1715; _LL1715: _temp1714=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1585)->f4; goto _LL1612;} else{ goto
_LL1613;} _LL1613: if(( unsigned int) _temp1585 > 2u?(( struct _tunion_struct*)
_temp1585)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1723: _temp1722=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1585)->f1; goto _LL1614;} else{ goto
_LL1615;} _LL1615: if(( unsigned int) _temp1585 > 2u?(( struct _tunion_struct*)
_temp1585)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1725: _temp1724=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1585)->f1; goto _LL1616;}
else{ goto _LL1617;} _LL1617: if(( unsigned int) _temp1585 > 2u?(( struct
_tunion_struct*) _temp1585)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1731:
_temp1730=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1585)->f1;
goto _LL1729; _LL1729: _temp1728=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1585)->f2; goto _LL1727; _LL1727: _temp1726=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1585)->f3; goto _LL1618;} else{
goto _LL1619;} _LL1619: if(( unsigned int) _temp1585 > 2u?(( struct
_tunion_struct*) _temp1585)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1737:
_temp1736=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1585)->f1; goto _LL1735; _LL1735: _temp1734=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1585)->f2; goto _LL1733; _LL1733:
_temp1732=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1585)->f3; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int)
_temp1585 > 2u?(( struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL1749: _temp1748=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1585)->f1; goto _LL1747; _LL1747: _temp1746=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1585)->f2; goto _LL1745; _LL1745:
_temp1744=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1585)->f3; goto _LL1743; _LL1743: _temp1742=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1585)->f4; goto _LL1741; _LL1741:
_temp1740=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1585)->f5; goto _LL1739; _LL1739: _temp1738=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1585)->f6; goto _LL1622;} else{ goto
_LL1623;} _LL1623: if(( unsigned int) _temp1585 > 2u?(( struct _tunion_struct*)
_temp1585)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL1759: _temp1758=( struct
_tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1585)->f1; goto _LL1757;
_LL1757: _temp1756=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1585)->f2; goto _LL1755; _LL1755: _temp1754=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1585)->f3; goto _LL1753; _LL1753:
_temp1752=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1585)->f4; goto _LL1751; _LL1751: _temp1750=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1585)->f5; goto _LL1624;} else{ goto
_LL1586;} _LL1588: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1586; _LL1590:
nv= Cyc_Toc_add_varmap( nv, _temp1640, r); s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1586; _LL1592: { struct _tuple1* v= Cyc_Toc_temp_var(); decls=({ struct
Cyc_List_List* _temp1760=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1760->hd=( void*)({ struct _tuple14* _temp1761=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1761->f1= v; _temp1761->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp1761;}); _temp1760->tl= decls;
_temp1760;}); nv= Cyc_Toc_add_varmap( nv, _temp1644->name, Cyc_Absyn_var_exp( v,
0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1586;} _LL1594: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1586; _LL1596: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1648, _temp1646, 0), succ_lab, fail_lab); goto _LL1586; _LL1598: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1650, 0), succ_lab, fail_lab); goto _LL1586; _LL1600:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1652, 0), succ_lab,
fail_lab); goto _LL1586; _LL1602: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1762=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp1762[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp1763; _temp1763.tag= Cyc_Absyn_Enum_e_tag; _temp1763.f1= _temp1658;
_temp1763.f2=( struct Cyc_Absyn_Enumdecl*) _temp1656; _temp1763.f3=( struct Cyc_Absyn_Enumfield*)
_temp1654; _temp1763;}); _temp1762;}), 0), succ_lab, fail_lab); goto _LL1586;
_LL1604: s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_cast_exp( Cyc_Absyn_sint_t, r, 0),
Cyc_Absyn_var_exp( _temp1670, 0), succ_lab, fail_lab); goto _LL1586; _LL1606: {
struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1685); struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1681->name,(
struct _tagged_string)({ char* _temp1764=( char*)"_struct"; struct
_tagged_string _temp1765; _temp1765.curr= _temp1764; _temp1765.base= _temp1764;
_temp1765.last_plus_one= _temp1764 + 8; _temp1765;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1685); for( 0; rps != 0; rps= rps->tl, cnt --){ struct
Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1766=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1766->hd=( void*)({ struct _tuple14* _temp1767=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1767->f1= v; _temp1767->f2= t2c;
_temp1767;}); _temp1766->tl= decls; _temp1766;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp1768=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1768[ 0]= xprintf("f%d", cnt); _temp1768;}), 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp1769=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1769[ 0]= xprintf("f%d", cnt); _temp1769;}), 0), 0),
0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp1770=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1770->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1770->tl= ss;
_temp1770;}); succ_lab= lab;}}}}{ unsigned int max_tag= 0;{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)( _temp1683->fields)->v; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*) fs->hd; if( f->typs
== 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp(
max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct( Cyc_Toc__tunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structarrow_exp( e3, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5=
Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1691,(
struct _tagged_string)({ char* _temp1771=( char*)"_tag"; struct _tagged_string
_temp1772; _temp1772.curr= _temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one=
_temp1771 + 5; _temp1772;})), 0); struct Cyc_Absyn_Exp* test_exp= max_tag >(
unsigned int) 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp( e1, e, 0), 0): Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( test_exp, Cyc_Absyn_seq_stmts( ss, 0),
Cyc_Absyn_goto_stmt( fail_lab, 0), 0); goto _LL1586;}}} _LL1608: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1706);
struct _tuple1* xfstrct= Cyc_Toc_collapse_qvar_tag( _temp1702->name,( struct
_tagged_string)({ char* _temp1773=( char*)"_struct"; struct _tagged_string
_temp1774; _temp1774.curr= _temp1773; _temp1774.base= _temp1773; _temp1774.last_plus_one=
_temp1773 + 8; _temp1774;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq), r, 0);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1706); for( 0; rps != 0; rps= rps->tl, cnt --){ struct Cyc_Absyn_Pat* p2=(
struct Cyc_Absyn_Pat*) rps->hd; struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=(
void*)( p2->topt)->v; struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({
struct Cyc_List_List* _temp1775=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1775->hd=( void*)({ struct _tuple14* _temp1776=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1776->f1= v;
_temp1776->f2= Cyc_Toc_typ_to_c( t2); _temp1776;}); _temp1775->tl= decls;
_temp1775;});{ struct _tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp(
v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct _tagged_string* _temp1777=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp1777[ 0]= xprintf("f%d", cnt); _temp1777;}), 0), p2, succ_lab, fail_lab,
decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt* s3= result.f3;
struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1778=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1778[ 0]= xprintf("f%d", cnt);
_temp1778;}), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0);
ss=({ struct Cyc_List_List* _temp1779=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1779->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1779->tl= ss; _temp1779;}); succ_lab= lab;}}}} if( _temp1706 == 0){
s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0),
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1710,( struct _tagged_string)({
char* _temp1780=( char*)"_tag"; struct _tagged_string _temp1781; _temp1781.curr=
_temp1780; _temp1781.base= _temp1780; _temp1781.last_plus_one= _temp1780 + 5;
_temp1781;})), 0), succ_lab, fail_lab);} else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq),
r, 0); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_structmember_exp( e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e=
Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( _temp1710,( struct _tagged_string)({
char* _temp1782=( char*)"_tag"; struct _tagged_string _temp1783; _temp1783.curr=
_temp1782; _temp1783.base= _temp1782; _temp1783.last_plus_one= _temp1782 + 5;
_temp1783;})), 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0),
Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} goto
_LL1586;} _LL1610: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1712);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1712); for( 0; rps != 0; rps= rps->tl,
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) rps->hd; struct
_tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)( p2->topt)->v; struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1784->hd=( void*)({ struct _tuple14* _temp1785=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1785->f1= v; _temp1785->f2= Cyc_Toc_typ_to_c(
t2); _temp1785;}); _temp1784->tl= decls; _temp1784;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1786=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1786[ 0]= xprintf("f%d", cnt);
_temp1786;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1787=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1787[ 0]= xprintf("f%d", cnt); _temp1787;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1788=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1788->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1788->tl= ss; _temp1788;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1586;} _LL1612: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1714); for( 0; dlps != 0; dlps= dlps->tl){ struct _tuple15* tup=( struct
_tuple15*) dlps->hd; struct Cyc_Absyn_Pat* p2=(* tup).f2; struct _tagged_string*
f;{ void* _temp1789=( void*)((* tup).f1)->hd; struct _tagged_string* _temp1795;
_LL1791: if((( struct _tunion_struct*) _temp1789)->tag == Cyc_Absyn_FieldName_tag){
_LL1796: _temp1795=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1789)->f1; goto _LL1792;} else{ goto _LL1793;} _LL1793: goto _LL1794;
_LL1792: f= _temp1795; goto _LL1790; _LL1794:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Toc_Match_error_struct* _temp1797=( struct Cyc_Toc_Match_error_struct*)
GC_malloc( sizeof( struct Cyc_Toc_Match_error_struct)); _temp1797[ 0]=({ struct
Cyc_Toc_Match_error_struct _temp1798; _temp1798.tag= Cyc_Toc_Match_error_tag;
_temp1798;}); _temp1797;})); _LL1790:;}{ struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)( p2->topt)->v; void* t2c= Cyc_Toc_typ_to_c( t2); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1799=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1799->hd=( void*)({ struct _tuple14* _temp1800=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1800->f1= v; _temp1800->f2= t2c;
_temp1800;}); _temp1799->tl= decls; _temp1799;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r,
f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1801->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1801->tl= ss; _temp1801;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1586;} _LL1614: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)( _temp1722->topt)->v; decls=({ struct Cyc_List_List* _temp1802=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1802->hd=(
void*)({ struct _tuple14* _temp1803=( struct _tuple14*) GC_malloc( sizeof(
struct _tuple14)); _temp1803->f1= v; _temp1803->f2= Cyc_Toc_typ_to_c( t2);
_temp1803;}); _temp1802->tl= decls; _temp1802;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path, 0), _temp1722,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0), s2, 0); if( Cyc_Toc_really_is_nullable(
t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp(
0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);} else{ s= s3;} goto
_LL1586;}}} _LL1616: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1804=( char*)"unknownid";
struct _tagged_string _temp1805; _temp1805.curr= _temp1804; _temp1805.base=
_temp1804; _temp1805.last_plus_one= _temp1804 + 10; _temp1805;})); goto _LL1586;
_LL1618: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1806=( char*)"unknowncall"; struct
_tagged_string _temp1807; _temp1807.curr= _temp1806; _temp1807.base= _temp1806;
_temp1807.last_plus_one= _temp1806 + 12; _temp1807;})); goto _LL1586; _LL1620: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1808=( char*)"unknownfields"; struct _tagged_string
_temp1809; _temp1809.curr= _temp1808; _temp1809.base= _temp1808; _temp1809.last_plus_one=
_temp1808 + 14; _temp1809;})); goto _LL1586; _LL1622: goto _LL1624; _LL1624: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1810=( char*)"non-null [x]tunion pattern not under &";
struct _tagged_string _temp1811; _temp1811.curr= _temp1810; _temp1811.base=
_temp1810; _temp1811.last_plus_one= _temp1810 + 39; _temp1811;})); goto _LL1586;
_LL1586:;} return({ struct _tuple13 _temp1812; _temp1812.f1= nv; _temp1812.f2=
decls; _temp1812.f3= s; _temp1812;});} static struct _tuple16* Cyc_Toc_gen_label(
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp1813=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp1813->f1= Cyc_Toc_fresh_label();
_temp1813->f2= Cyc_Toc_fresh_label(); _temp1813->f3= sc; _temp1813;});} static
void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c(
nv, e);{ void* t=( void*)( e->topt)->v; int leave_as_switch;{ void* _temp1814=
Cyc_Tcutil_compress( t); void* _temp1820; void* _temp1822; _LL1816: if((
unsigned int) _temp1814 > 4u?(( struct _tunion_struct*) _temp1814)->tag == Cyc_Absyn_IntType_tag:
0){ _LL1823: _temp1822=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1814)->f1;
goto _LL1821; _LL1821: _temp1820=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1814)->f2; goto _LL1817;} else{ goto _LL1818;} _LL1818: goto _LL1819;
_LL1817: leave_as_switch= 1; goto _LL1815; _LL1819: leave_as_switch= 0; goto
_LL1815; _LL1815:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=
scs1->tl){ if(( struct Cyc_List_List*)((( struct Cyc_Absyn_Switch_clause*) scs1->hd)->pat_vars)->v
!= 0){ leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1= scs1->tl){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)
scs1->hd)->body;(( struct Cyc_Absyn_Switch_clause*) scs1->hd)->body= Cyc_Absyn_label_stmt(
next_l, s, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple1* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2= lscs2->tl){ struct
Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*) lscs2->hd)).f3; struct
_tagged_string* fail_lab= lscs2->tl == 0? end_l:(*(( struct _tuple16*)( lscs2->tl)->hd)).f1;
struct _tagged_string* succ_lab=(*(( struct _tuple16*) lscs2->hd)).f2; if( sc->where_clause
== 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1824=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1824->hd=( void*) res.f1;
_temp1824->tl= nvs; _temp1824;}); decls= res.f2; test_stmts=({ struct Cyc_List_List*
_temp1825=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1825->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*) lscs2->hd)).f1,
res.f3, 0); _temp1825->tl= test_stmts; _temp1825;});} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1826= sc->where_clause;
if( _temp1826 == 0){ _throw( Null_Exception);} _temp1826;}); struct
_tagged_string* intermed_lab= Cyc_Toc_fresh_label(); struct _tuple13 res= Cyc_Toc_xlate_pat(
nv, t, r, path, sc->pattern, intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c(
res.f1, e); nvs=({ struct Cyc_List_List* _temp1827=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1827->hd=( void*) res.f1;
_temp1827->tl= nvs; _temp1827;}); decls= res.f2;{ struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt(
e, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( intermed_lab, s2, 0);
test_stmts=({ struct Cyc_List_List* _temp1828=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1828->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*) lscs2->hd)).f1, Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0);
_temp1828->tl= test_stmts; _temp1828;});}}}}{ struct Cyc_Absyn_Stmt* test_stmt=
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
decls; decls != 0; decls= decls->tl){ struct _tuple14 _temp1831; void* _temp1832;
struct _tuple1* _temp1834; struct _tuple14* _temp1829=( struct _tuple14*) decls->hd;
_temp1831=* _temp1829; _LL1835: _temp1834= _temp1831.f1; goto _LL1833; _LL1833:
_temp1832= _temp1831.f2; goto _LL1830; _LL1830: res= Cyc_Absyn_declare_stmt(
_temp1834, _temp1832, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)( e->topt)->v),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp1836=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1836[ 0]=({ struct Cyc_List_List _temp1837; _temp1837.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp1837.tl= 0; _temp1837;}); _temp1836;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1838=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1876; struct Cyc_Absyn_Stmt* _temp1878; struct Cyc_Absyn_Stmt*
_temp1880; struct Cyc_Absyn_Exp* _temp1882; struct Cyc_Absyn_Stmt* _temp1884;
struct Cyc_Absyn_Stmt* _temp1886; struct Cyc_Absyn_Exp* _temp1888; struct Cyc_Absyn_Stmt*
_temp1890; struct _tuple3 _temp1892; struct Cyc_Absyn_Stmt* _temp1894; struct
Cyc_Absyn_Exp* _temp1896; struct Cyc_Absyn_Stmt* _temp1898; struct Cyc_Absyn_Stmt*
_temp1900; struct Cyc_Absyn_Stmt* _temp1902; struct _tagged_string* _temp1904;
struct Cyc_Absyn_Stmt* _temp1906; struct _tuple3 _temp1908; struct Cyc_Absyn_Stmt*
_temp1910; struct Cyc_Absyn_Exp* _temp1912; struct _tuple3 _temp1914; struct Cyc_Absyn_Stmt*
_temp1916; struct Cyc_Absyn_Exp* _temp1918; struct Cyc_Absyn_Exp* _temp1920;
struct Cyc_List_List* _temp1922; struct Cyc_Absyn_Exp* _temp1924; struct Cyc_Absyn_Switch_clause**
_temp1926; struct Cyc_List_List* _temp1928; struct Cyc_Absyn_Stmt* _temp1930;
struct Cyc_Absyn_Decl* _temp1932; struct Cyc_Absyn_Stmt* _temp1934; struct Cyc_Absyn_Stmt*
_temp1936; struct Cyc_Absyn_Stmt* _temp1938; struct _tagged_string* _temp1940;
struct _tuple3 _temp1942; struct Cyc_Absyn_Stmt* _temp1944; struct Cyc_Absyn_Exp*
_temp1946; struct Cyc_Absyn_Stmt* _temp1948; struct Cyc_List_List* _temp1950;
struct Cyc_Absyn_Stmt* _temp1952; _LL1840: if(( int) _temp1838 == Cyc_Absyn_Skip_s){
goto _LL1841;} else{ goto _LL1842;} _LL1842: if(( unsigned int) _temp1838 > 1u?((
struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1877:
_temp1876=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1838)->f1;
goto _LL1843;} else{ goto _LL1844;} _LL1844: if(( unsigned int) _temp1838 > 1u?((
struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1881:
_temp1880=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1838)->f1;
goto _LL1879; _LL1879: _temp1878=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1838)->f2; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(( unsigned int)
_temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1883: _temp1882=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1838)->f1; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(( unsigned int)
_temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1889: _temp1888=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1838)->f1; goto _LL1887; _LL1887: _temp1886=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1838)->f2; goto _LL1885; _LL1885:
_temp1884=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1838)->f3; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(( unsigned int)
_temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1893: _temp1892=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1838)->f1; _LL1897: _temp1896= _temp1892.f1; goto _LL1895; _LL1895:
_temp1894= _temp1892.f2; goto _LL1891; _LL1891: _temp1890=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1838)->f2; goto _LL1851;} else{ goto
_LL1852;} _LL1852: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1899: _temp1898=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1838)->f1; goto _LL1853;} else{ goto
_LL1854;} _LL1854: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1901: _temp1900=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1838)->f1; goto _LL1855;} else{ goto
_LL1856;} _LL1856: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1905: _temp1904=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1838)->f1; goto _LL1903;
_LL1903: _temp1902=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1838)->f2; goto _LL1857;} else{ goto _LL1858;} _LL1858: if(( unsigned int)
_temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1921: _temp1920=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1838)->f1; goto _LL1915; _LL1915: _temp1914=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1838)->f2; _LL1919: _temp1918= _temp1914.f1; goto _LL1917; _LL1917:
_temp1916= _temp1914.f2; goto _LL1909; _LL1909: _temp1908=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1838)->f3; _LL1913: _temp1912= _temp1908.f1;
goto _LL1911; _LL1911: _temp1910= _temp1908.f2; goto _LL1907; _LL1907: _temp1906=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1838)->f4; goto
_LL1859;} else{ goto _LL1860;} _LL1860: if(( unsigned int) _temp1838 > 1u?((
struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1925:
_temp1924=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1838)->f1; goto _LL1923; _LL1923: _temp1922=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1838)->f2; goto _LL1861;} else{ goto
_LL1862;} _LL1862: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1929: _temp1928=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1838)->f1; goto _LL1927; _LL1927:
_temp1926=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1838)->f2; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(( unsigned int)
_temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1933: _temp1932=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1838)->f1; goto _LL1931; _LL1931: _temp1930=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1838)->f2; goto _LL1865;} else{ goto
_LL1866;} _LL1866: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1935: _temp1934=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1838)->f1; goto _LL1867;} else{ goto
_LL1868;} _LL1868: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1937: _temp1936=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1838)->f1; goto _LL1869;} else{ goto
_LL1870;} _LL1870: if(( unsigned int) _temp1838 > 1u?(( struct _tunion_struct*)
_temp1838)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1941: _temp1940=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1838)->f1; goto
_LL1939; _LL1939: _temp1938=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1838)->f2; goto _LL1871;} else{ goto _LL1872;} _LL1872: if(( unsigned int)
_temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1949: _temp1948=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1838)->f1; goto _LL1943; _LL1943: _temp1942=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1838)->f2; _LL1947: _temp1946= _temp1942.f1; goto _LL1945; _LL1945:
_temp1944= _temp1942.f2; goto _LL1873;} else{ goto _LL1874;} _LL1874: if((
unsigned int) _temp1838 > 1u?(( struct _tunion_struct*) _temp1838)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1953: _temp1952=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1838)->f1; goto _LL1951; _LL1951: _temp1950=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1838)->f2; goto _LL1875;} else{ goto
_LL1839;} _LL1841: return; _LL1843: Cyc_Toc_exp_to_c( nv, _temp1876); return;
_LL1845: Cyc_Toc_stmt_to_c( nv, _temp1880); s= _temp1878; continue; _LL1847: {
struct Cyc_Core_Opt* topt= 0; if( _temp1882 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1954=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1954->v=( void*) Cyc_Toc_typ_to_c(( void*)( _temp1882->topt)->v); _temp1954;});
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1955=
_temp1882; if( _temp1955 == 0){ _throw( Null_Exception);} _temp1955;}));} if( s->try_depth
> 0){ if( topt != 0){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
retn_stmt= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x,
0), 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( x,( void*) topt->v,
_temp1882, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth),
retn_stmt, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth, s);}}
return;} _LL1849: Cyc_Toc_exp_to_c( nv, _temp1888); Cyc_Toc_stmt_to_c( nv,
_temp1886); s= _temp1884; continue; _LL1851: Cyc_Toc_exp_to_c( nv, _temp1896);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1890); return; _LL1853: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)( nv->break_lab)->v, 0))->r));}{ int dest_depth= _temp1898 == 0?
0: _temp1898->try_depth; Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s);
return;} _LL1855: if( nv->continue_lab != 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_string*)( nv->continue_lab)->v, 0))->r));}
Cyc_Toc_do_npop_before( s->try_depth - _temp1900->try_depth, s); return; _LL1857:
Cyc_Toc_do_npop_before( s->try_depth - _temp1902->try_depth, s); return; _LL1859:
Cyc_Toc_exp_to_c( nv, _temp1920); Cyc_Toc_exp_to_c( nv, _temp1918); Cyc_Toc_exp_to_c(
nv, _temp1912); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1906); return;
_LL1861: Cyc_Toc_xlate_switch( nv, s, _temp1924, _temp1922); return; _LL1863:
if( nv->fallthru_info == 0){( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1956=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp1957; _temp1957.curr=
_temp1956; _temp1957.base= _temp1956; _temp1957.last_plus_one= _temp1956 + 29;
_temp1957;})));}{ struct _tuple8 _temp1960; struct Cyc_Dict_Dict* _temp1961;
struct Cyc_List_List* _temp1963; struct _tagged_string* _temp1965; struct
_tuple8* _temp1958=( struct _tuple8*)( nv->fallthru_info)->v; _temp1960=*
_temp1958; _LL1966: _temp1965= _temp1960.f1; goto _LL1964; _LL1964: _temp1963=
_temp1960.f2; goto _LL1962; _LL1962: _temp1961= _temp1960.f3; goto _LL1959;
_LL1959: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1965, 0); Cyc_Toc_do_npop_before(
s->try_depth -((* _temp1926)->body)->try_depth, s2);{ struct Cyc_List_List* vs2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1963);
struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp1928); for( 0; vs2 != 0; vs2= vs2->tl, es2= es2->tl){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*) es2->hd); s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(((
struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
_temp1961,( struct _tuple1*) vs2->hd),( struct Cyc_Absyn_Exp*) es2->hd, 0), s2,
0);}( void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1865:{ void*
_temp1967=( void*) _temp1932->r; int _temp1973; struct Cyc_Absyn_Exp* _temp1975;
struct Cyc_Core_Opt* _temp1977; struct Cyc_Core_Opt* _temp1979; struct Cyc_Absyn_Pat*
_temp1981; _LL1969: if((( struct _tunion_struct*) _temp1967)->tag == Cyc_Absyn_Let_d_tag){
_LL1982: _temp1981=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1967)->f1; goto _LL1980; _LL1980: _temp1979=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1967)->f2; goto _LL1978; _LL1978: _temp1977=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1967)->f3; goto
_LL1976; _LL1976: _temp1975=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1967)->f4; goto _LL1974; _LL1974: _temp1973=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1967)->f5; goto _LL1970;} else{ goto _LL1971;} _LL1971: goto _LL1972;
_LL1970: { int var_case= 0;{ void* _temp1983=( void*) _temp1981->r; struct Cyc_Absyn_Vardecl*
_temp1989; _LL1985: if(( unsigned int) _temp1983 > 2u?(( struct _tunion_struct*)
_temp1983)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1990: _temp1989=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1983)->f1; goto _LL1986;} else{ goto
_LL1987;} _LL1987: goto _LL1988; _LL1986: var_case= 1; _temp1989->initializer=(
struct Cyc_Absyn_Exp*) _temp1975;( void*)( _temp1932->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1991=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1991[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1992; _temp1992.tag= Cyc_Absyn_Var_d_tag; _temp1992.f1= _temp1989;
_temp1992;}); _temp1991;}))); goto _LL1984; _LL1988: goto _LL1984; _LL1984:;}
if( var_case){ goto _LL1972;}( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c(
nv, _temp1981, _temp1977, _temp1975, _temp1973, _temp1930))->r)); goto _LL1968;}
_LL1972: { struct Cyc_List_List* _temp1996; struct Cyc_Toc_Env* _temp1998;
struct _tuple17 _temp1994= Cyc_Toc_decls_to_c( nv,({ struct Cyc_List_List*
_temp1993=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1993->hd=( void*) _temp1932; _temp1993->tl= 0; _temp1993;}), 0); _LL1999:
_temp1998= _temp1994.f1; goto _LL1997; _LL1997: _temp1996= _temp1994.f2; goto
_LL1995; _LL1995: Cyc_Toc_stmt_to_c( _temp1998, _temp1930); for( 0; _temp1996 !=
0; _temp1996= _temp1996->tl){ _temp1930= Cyc_Absyn_decl_stmt(( struct Cyc_Absyn_Decl*)
_temp1996->hd, _temp1930, 0);}( void*)( s->r=( void*)(( void*) _temp1930->r));
goto _LL1968;} _LL1968:;} return; _LL1867:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp2000=( char*)"cut"; struct
_tagged_string _temp2001; _temp2001.curr= _temp2000; _temp2001.base= _temp2000;
_temp2001.last_plus_one= _temp2000 + 4; _temp2001;})); return; _LL1869:(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp2002=( char*)"splice"; struct _tagged_string _temp2003; _temp2003.curr=
_temp2002; _temp2003.base= _temp2002; _temp2003.last_plus_one= _temp2002 + 7;
_temp2003;})); return; _LL1871: s= _temp1938; continue; _LL1873: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1948); Cyc_Toc_exp_to_c( nv, _temp1946); return;
_LL1875: { struct _tuple1* h_var= Cyc_Toc_temp_var(); struct _tuple1* e_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2004=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2004->f1= Cyc_Toc_abs_ns; _temp2004->f2=
Cyc_Toc__push_handler_sp; _temp2004;}), 0); struct Cyc_Absyn_Exp* setjmp_exp=
Cyc_Absyn_var_exp(({ struct _tuple1* _temp2005=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp2005->f1= Cyc_Toc_abs_ns; _temp2005->f2= Cyc_Toc_setjmp_sp;
_temp2005;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp2006=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2006->f1= Cyc_Toc_abs_ns; _temp2006->f2= Cyc_Toc__pop_handler_sp; _temp2006;}),
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2007=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2007->v=( void*) e_typ; _temp2007;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1952);{
struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp1952, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl*
x_vd= Cyc_Absyn_new_vardecl( x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt*
_temp2008=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2008->v=( void*) Cyc_Absyn_exn_typ; _temp2008;});{ struct Cyc_Absyn_Pat* p=({
struct Cyc_Absyn_Pat* _temp2009=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof(
struct Cyc_Absyn_Pat)); _temp2009->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2011=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2011[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2012; _temp2012.tag= Cyc_Absyn_Var_p_tag;
_temp2012.f1= x_vd; _temp2012;}); _temp2011;})); _temp2009->topt=({ struct Cyc_Core_Opt*
_temp2010=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2010->v=( void*) Cyc_Absyn_exn_typ; _temp2010;}); _temp2009->loc= 0;
_temp2009;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2013=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2013->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2013;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2014=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2014->pattern= p;
_temp2014->pat_vars=({ struct Cyc_Core_Opt* _temp2015=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2015->v=( void*)({ struct Cyc_List_List*
_temp2016=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2016->hd=( void*) x_vd; _temp2016->tl= 0; _temp2016;}); _temp2015;});
_temp2014->where_clause= 0; _temp2014->body= rs; _temp2014->loc= 0; _temp2014;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1950,({
struct Cyc_List_List* _temp2017=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2017->hd=( void*) default_case; _temp2017->tl= 0;
_temp2017;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2018=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2018->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2018->tl= 0; _temp2018;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2019=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2019->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2019->tl= 0;
_temp2019;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL1839:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args= args->tl){(*(( struct _tuple18*) args->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple18*) args->hd)).f3);{ struct _tuple1* x=({ struct _tuple1*
_temp2020=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2020->f1=(
void*) Cyc_Absyn_Loc_n; _temp2020->f2=(*(( struct _tuple18*) args->hd)).f1;
_temp2020;}); nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp2021= s; _LL2023: if(( int) _temp2021 == Cyc_Absyn_Abstract){ goto
_LL2024;} else{ goto _LL2025;} _LL2025: if(( int) _temp2021 == Cyc_Absyn_ExternC){
goto _LL2026;} else{ goto _LL2027;} _LL2027: goto _LL2028; _LL2024: return( void*)
Cyc_Absyn_Public; _LL2026: return( void*) Cyc_Absyn_Extern; _LL2028: return s;
_LL2022:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple1* n=( struct _tuple1*)( s->name)->v;
if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt*
_temp2029=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2029->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2029;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(* n).f2); if( dopt == 0){
seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_structs_so_far->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)
Cyc_Toc_structs_so_far->v,(* n).f2, s));} else{ if((( struct Cyc_Absyn_Structdecl*)
dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)( Cyc_Toc_structs_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_structs_so_far->v,(* n).f2, s)); seen_defn_before=
0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==(
void*) Cyc_Absyn_Extern? 1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*)
s->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) s->sc))); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2030=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2030->v=( void*) n;
_temp2030;}); if( s->fields != 0){ if( seen_defn_before){ s->fields= 0;} else{
struct Cyc_List_List* fields=( struct Cyc_List_List*)( s->fields)->v; for( 0;
fields != 0; fields= fields->tl){( void*)((( struct Cyc_Absyn_Structfield*)
fields->hd)->type=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0; static
void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct _tuple1* n=(
struct _tuple1*)( u->name)->v; if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({
struct Cyc_Core_Opt* _temp2031=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2031->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2031;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*) Cyc_Toc_unions_so_far->v,(* n).f2); if( dopt == 0){
seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_unions_so_far->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*) Cyc_Toc_unions_so_far->v,(*
n).f2, u));} else{ if((( struct Cyc_Absyn_Uniondecl*) dopt->v)->fields == 0){(
struct Cyc_Dict_Dict*)( Cyc_Toc_unions_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*) Cyc_Toc_unions_so_far->v,(* n).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((((
void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) u->sc ==( void*) Cyc_Absyn_Static)?
1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c((
void*) u->sc))); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2032=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2032->v=( void*) n;
_temp2032;}); if( u->fields != 0){ if( seen_defn_before){ u->fields= 0;} else{
struct Cyc_List_List* fields=( struct Cyc_List_List*)( u->fields)->v; for( 0;
fields != 0; fields= fields->tl){( void*)((( struct Cyc_Absyn_Structfield*)
fields->hd)->type=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)
fields->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far= 0;
static struct Cyc_List_List* Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)( ed->name)->v;
if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2033=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2033->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2033;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*) Cyc_Toc_tunions_so_far->v,(* n).f2);
if( dopt == 0){ seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)(
Cyc_Toc_tunions_so_far->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_tunions_so_far->v,(* n).f2, ed));} else{
seen_before= 1; if((( struct Cyc_Absyn_Tuniondecl*) dopt->v)->fields == 0){
seen_defn_before= 0;(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_tunions_so_far->v,(* n).f2, ed);} else{
seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_Absyn_Typedefdecl* tdef=({
struct Cyc_Absyn_Typedefdecl* _temp2034=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp2034->name= n; _temp2034->tvs=
0; _temp2034->defn=( void*) Cyc_Absyn_void_star_typ(); _temp2034;}); res=({
struct Cyc_List_List* _temp2035=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2035->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp2036=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2036[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2037; _temp2037.tag= Cyc_Absyn_Typedef_d_tag;
_temp2037.f1= tdef; _temp2037;}); _temp2036;}), 0); _temp2035->tl= res;
_temp2035;});}{ void* t=( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2038=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2038[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2039; _temp2039.tag=
Cyc_Absyn_TypedefType_tag; _temp2039.f1= n; _temp2039.f2= 0; _temp2039.f3=({
struct Cyc_Core_Opt* _temp2040=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2040->v=( void*) Cyc_Absyn_void_star_typ(); _temp2040;});
_temp2039;}); _temp2038;}); int tag_count= 0; int box_count= 0;{ struct Cyc_List_List*
fields= ed->fields == 0? 0:( struct Cyc_List_List*)( ed->fields)->v; for( 0;
fields != 0; fields= fields->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)
fields->hd; if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc ==(
void*) Cyc_Absyn_Extern? 0:( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( f->name,
Cyc_Absyn_uint_t, eopt);( void*)( vd->sc=( void*) Cyc_Toc_scope_to_c(( void*) ed->sc));
vd->tq=({ struct Cyc_Absyn_Tqual* _temp2041=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2041->q_const= 1;
_temp2041->q_volatile= 0; _temp2041->q_restrict= 0; _temp2041;}); res=({ struct
Cyc_List_List* _temp2042=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2042->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2043=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2043[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2044; _temp2044.tag= Cyc_Absyn_Var_d_tag;
_temp2044.f1= vd; _temp2044;}); _temp2043;}), 0); _temp2042->tl= res; _temp2042;});}
else{ struct Cyc_Absyn_Exp* eopt=( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 0:(
struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(( box_count ++), 0); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp2045=( char*)"_tag"; struct _tagged_string
_temp2046; _temp2046.curr= _temp2045; _temp2046.base= _temp2045; _temp2046.last_plus_one=
_temp2045 + 5; _temp2046;})), Cyc_Absyn_sint_t, eopt);( void*)( vd->sc=( void*)
Cyc_Toc_scope_to_c(( void*) ed->sc)); vd->tq=({ struct Cyc_Absyn_Tqual*
_temp2047=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2047->q_const= 1; _temp2047->q_volatile= 0; _temp2047->q_restrict= 0;
_temp2047;}); res=({ struct Cyc_List_List* _temp2048=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2048->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2049=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2049[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2050; _temp2050.tag= Cyc_Absyn_Var_d_tag; _temp2050.f1= vd; _temp2050;});
_temp2049;}), 0); _temp2048->tl= res; _temp2048;}); if( ! seen_defn_before){
struct Cyc_List_List* fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for(
0; ts != 0; ts= ts->tl, i ++){ struct _tagged_string* fname=( struct
_tagged_string*)({ struct _tagged_string* _temp2051=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp2051[ 0]= xprintf("f%d", i);
_temp2051;}); struct Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield*
_temp2052=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2052->name= fname; _temp2052->tq=(*(( struct _tuple5*) ts->hd)).f1;
_temp2052->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*) ts->hd)).f2);
_temp2052->width= 0; _temp2052->attributes= 0; _temp2052;}); fs=({ struct Cyc_List_List*
_temp2053=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2053->hd=( void*) f; _temp2053->tl= fs; _temp2053;});}} fs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs); fs=({ struct Cyc_List_List*
_temp2054=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2054->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp2055=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2055->name= Cyc_Toc_tag_sp;
_temp2055->tq= Cyc_Toc_mt_tq; _temp2055->type=( void*) Cyc_Absyn_sint_t;
_temp2055->width= 0; _temp2055->attributes= 0; _temp2055;}); _temp2054->tl= fs;
_temp2054;});{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp2056=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2056->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2056->name=({ struct Cyc_Core_Opt*
_temp2058=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2058->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2059=( char*)"_struct"; struct _tagged_string _temp2060; _temp2060.curr=
_temp2059; _temp2060.base= _temp2059; _temp2060.last_plus_one= _temp2059 + 8;
_temp2060;})); _temp2058;}); _temp2056->tvs= 0; _temp2056->fields=({ struct Cyc_Core_Opt*
_temp2057=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2057->v=( void*) fs; _temp2057;}); _temp2056->attributes= 0; _temp2056;});
res=({ struct Cyc_List_List* _temp2061=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2061->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp2062=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2062[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2063; _temp2063.tag= Cyc_Absyn_Struct_d_tag;
_temp2063.f1= sd; _temp2063;}); _temp2062;}), 0); _temp2061->tl= res; _temp2061;});}}}}}
return res;}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunions_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_XTuniondecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp2064=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2064->name= n; _temp2064->tvs= 0; _temp2064->defn=( void*) t; _temp2064;});
res=({ struct Cyc_List_List* _temp2065=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2065->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp2066=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2066[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2067; _temp2067.tag= Cyc_Absyn_Typedef_d_tag;
_temp2067.f1= td; _temp2067;}); _temp2066;}), 0); _temp2065->tl= res; _temp2065;});}
else{ if( Cyc_Toc_xtunions_so_far == 0){ Cyc_Toc_xtunions_so_far=({ struct Cyc_Core_Opt*
_temp2068=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2068->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2068;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs= fs->tl){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*) fs->hd; struct
_tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_uint_exp(({
struct _tagged_string _temp2069=* fn;( unsigned int)( _temp2069.last_plus_one -
_temp2069.curr);}), 0); struct Cyc_Absyn_ArrayType_struct* tag_typ=({ struct Cyc_Absyn_ArrayType_struct*
_temp2070=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2070[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2071; _temp2071.tag= Cyc_Absyn_ArrayType_tag;
_temp2071.f1=( void*) Cyc_Absyn_uchar_t; _temp2071.f2= Cyc_Toc_mt_tq; _temp2071.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2071;}); _temp2070;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp2072=( char*)"_tag"; struct _tagged_string
_temp2073; _temp2073.curr= _temp2072; _temp2073.base= _temp2072; _temp2073.last_plus_one=
_temp2072 + 5; _temp2073;})),( void*) tag_typ, initopt);( void*)( tag_decl->sc=(
void*)(( void*) xd->sc)); res=({ struct Cyc_List_List* _temp2074=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2074->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2075=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2075[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2076; _temp2076.tag= Cyc_Absyn_Var_d_tag; _temp2076.f1= tag_decl; _temp2076;});
_temp2075;}), 0); _temp2074->tl= res; _temp2074;});{ int seen_before; int
seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)
Cyc_Toc_xtunions_so_far->v, fn); if( dopt == 0){ seen_before= 0;
seen_defn_before= 0;( struct Cyc_Dict_Dict*)( Cyc_Toc_xtunions_so_far->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)
Cyc_Toc_xtunions_so_far->v, fn, xd));} else{ seen_before= 1; if((( struct Cyc_Absyn_XTuniondecl*)
dopt->v)->fields == 0){( struct Cyc_Dict_Dict*)( Cyc_Toc_xtunions_so_far->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*) Cyc_Toc_xtunions_so_far->v, fn, xd)); seen_defn_before= 0;}
else{ seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0;
int i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts= ts->tl, i ++){
struct _tagged_string* field_n=( struct _tagged_string*)({ struct _tagged_string*
_temp2077=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2077[ 0]= xprintf("f%d", i); _temp2077;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp2078=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2078->name= field_n;
_temp2078->tq=(*(( struct _tuple5*) ts->hd)).f1; _temp2078->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple5*) ts->hd)).f2); _temp2078->width= 0; _temp2078->attributes= 0;
_temp2078;}); fields=({ struct Cyc_List_List* _temp2079=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2079->hd=( void*) newf;
_temp2079->tl= fields; _temp2079;});}} fields=({ struct Cyc_List_List* _temp2080=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2080->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp2081=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2081->name= Cyc_Toc_tag_sp;
_temp2081->tq= Cyc_Toc_mt_tq; _temp2081->type=( void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t,
Cyc_Toc_mt_tq); _temp2081->width= 0; _temp2081->attributes= 0; _temp2081;});
_temp2080->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp2080;});{ struct Cyc_Absyn_Structdecl* strct_decl=({ struct Cyc_Absyn_Structdecl*
_temp2082=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp2082->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2082->name=({ struct Cyc_Core_Opt*
_temp2084=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2084->v=( void*) Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({
char* _temp2085=( char*)"_struct"; struct _tagged_string _temp2086; _temp2086.curr=
_temp2085; _temp2086.base= _temp2085; _temp2086.last_plus_one= _temp2085 + 8;
_temp2086;})); _temp2084;}); _temp2082->tvs= 0; _temp2082->fields=({ struct Cyc_Core_Opt*
_temp2083=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2083->v=( void*) fields; _temp2083;}); _temp2082->attributes= 0; _temp2082;});
res=({ struct Cyc_List_List* _temp2087=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2087->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp2088=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2088[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2089; _temp2089.tag= Cyc_Absyn_Struct_d_tag;
_temp2089.f1= strct_decl; _temp2089;}); _temp2088;}), 0); _temp2087->tl= res;
_temp2087;});}}}}}}} return res;} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple1* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs= fs->tl){ if((( struct Cyc_Absyn_Enumfield*) fs->hd)->tag !=
0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2090=(( struct Cyc_Absyn_Enumfield*) fs->hd)->tag;
if( _temp2090 == 0){ _throw( Null_Exception);} _temp2090;}));(( struct Cyc_Absyn_Enumfield*)
fs->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( i, 0);}}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s){ void* t=( void*) topt->v; Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x= Cyc_Toc_temp_var();
struct _tagged_string* succ_lab= Cyc_Toc_fresh_label(); struct _tagged_string*
fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label(); struct Cyc_Absyn_Stmt*
_temp2093; struct Cyc_List_List* _temp2095; struct Cyc_Toc_Env* _temp2097;
struct _tuple13 _temp2091= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp( x, 0),
Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2098: _temp2097=
_temp2091.f1; goto _LL2096; _LL2096: _temp2095= _temp2091.f2; goto _LL2094;
_LL2094: _temp2093= _temp2091.f3; goto _LL2092; _LL2092: Cyc_Toc_stmt_to_c(
_temp2097, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2093, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2099=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2099->f1= Cyc_Toc_abs_ns; _temp2099->f2= Cyc_Toc_Match_Exception_sp;
_temp2099;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2093, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2095 != 0; _temp2095= _temp2095->tl){
struct _tuple14 _temp2102; void* _temp2103; struct _tuple1* _temp2105; struct
_tuple14* _temp2100=( struct _tuple14*) _temp2095->hd; _temp2102=* _temp2100;
_LL2106: _temp2105= _temp2102.f1; goto _LL2104; _LL2104: _temp2103= _temp2102.f2;
goto _LL2101; _LL2101: s= Cyc_Absyn_declare_stmt( _temp2105, _temp2103, 0, s, 0);}
return s;}}} static struct _tuple17 Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_List_List* ds, int top){ struct Cyc_List_List* res= 0; for( 0; ds !=
0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) ds->hd; void*
_temp2107=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp2133; struct Cyc_Absyn_Fndecl*
_temp2135; int _temp2137; struct Cyc_Absyn_Exp* _temp2139; struct Cyc_Core_Opt*
_temp2141; struct Cyc_Core_Opt* _temp2143; struct Cyc_Absyn_Pat* _temp2145;
struct Cyc_Absyn_Structdecl* _temp2147; struct Cyc_Absyn_Uniondecl* _temp2149;
struct Cyc_Absyn_Tuniondecl* _temp2151; struct Cyc_Absyn_XTuniondecl* _temp2153;
struct Cyc_Absyn_Enumdecl* _temp2155; struct Cyc_Absyn_Typedefdecl* _temp2157;
struct Cyc_List_List* _temp2159; struct _tagged_string* _temp2161; struct Cyc_List_List*
_temp2163; struct _tuple1* _temp2165; struct Cyc_List_List* _temp2167; _LL2109:
if((( struct _tunion_struct*) _temp2107)->tag == Cyc_Absyn_Var_d_tag){ _LL2134:
_temp2133=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2107)->f1; goto _LL2110;} else{ goto _LL2111;} _LL2111: if((( struct
_tunion_struct*) _temp2107)->tag == Cyc_Absyn_Fn_d_tag){ _LL2136: _temp2135=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2107)->f1; goto
_LL2112;} else{ goto _LL2113;} _LL2113: if((( struct _tunion_struct*) _temp2107)->tag
== Cyc_Absyn_Let_d_tag){ _LL2146: _temp2145=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp2107)->f1; goto _LL2144; _LL2144: _temp2143=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2107)->f2; goto
_LL2142; _LL2142: _temp2141=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2107)->f3; goto _LL2140; _LL2140: _temp2139=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp2107)->f4; goto _LL2138; _LL2138: _temp2137=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp2107)->f5; goto _LL2114;} else{ goto
_LL2115;} _LL2115: if((( struct _tunion_struct*) _temp2107)->tag == Cyc_Absyn_Struct_d_tag){
_LL2148: _temp2147=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp2107)->f1; goto _LL2116;} else{ goto _LL2117;} _LL2117: if((( struct
_tunion_struct*) _temp2107)->tag == Cyc_Absyn_Union_d_tag){ _LL2150: _temp2149=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp2107)->f1;
goto _LL2118;} else{ goto _LL2119;} _LL2119: if((( struct _tunion_struct*)
_temp2107)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2152: _temp2151=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2107)->f1; goto _LL2120;} else{ goto
_LL2121;} _LL2121: if((( struct _tunion_struct*) _temp2107)->tag == Cyc_Absyn_XTunion_d_tag){
_LL2154: _temp2153=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp2107)->f1; goto _LL2122;} else{ goto _LL2123;} _LL2123: if((( struct
_tunion_struct*) _temp2107)->tag == Cyc_Absyn_Enum_d_tag){ _LL2156: _temp2155=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2107)->f1;
goto _LL2124;} else{ goto _LL2125;} _LL2125: if((( struct _tunion_struct*)
_temp2107)->tag == Cyc_Absyn_Typedef_d_tag){ _LL2158: _temp2157=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2107)->f1; goto _LL2126;} else{ goto
_LL2127;} _LL2127: if((( struct _tunion_struct*) _temp2107)->tag == Cyc_Absyn_Namespace_d_tag){
_LL2162: _temp2161=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2107)->f1; goto _LL2160; _LL2160: _temp2159=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2107)->f2; goto _LL2128;} else{ goto
_LL2129;} _LL2129: if((( struct _tunion_struct*) _temp2107)->tag == Cyc_Absyn_Using_d_tag){
_LL2166: _temp2165=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2107)->f1; goto _LL2164; _LL2164: _temp2163=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2107)->f2; goto _LL2130;} else{ goto
_LL2131;} _LL2131: if((( struct _tunion_struct*) _temp2107)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2168: _temp2167=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2107)->f1; goto _LL2132;} else{ goto _LL2108;} _LL2110: { struct _tuple1*
c_name= _temp2133->name; if(( void*) _temp2133->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple1* _temp2169=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp2169->f1= Cyc_Toc_abs_ns; _temp2169->f2=(* c_name).f2; _temp2169;});}
if( _temp2133->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2170= _temp2133->initializer; if( _temp2170 == 0){
_throw( Null_Exception);} _temp2170;}));} if( nv->toplevel){ res=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls,
res); Cyc_Toc_temp_topdecls= 0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp2133->name,
Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2171=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2171[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2172; _temp2172.tag= Cyc_Absyn_Global_b_tag;
_temp2172.f1= _temp2133; _temp2172;}); _temp2171;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp2133->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2173=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2173[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2174; _temp2174.tag= Cyc_Absyn_Local_b_tag;
_temp2174.f1= _temp2133; _temp2174;}); _temp2173;}), 0));} _temp2133->name=
c_name;( void*)( _temp2133->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2133->sc));(
void*)( _temp2133->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2133->type));
res=({ struct Cyc_List_List* _temp2175=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2175->hd=( void*) d; _temp2175->tl= res;
_temp2175;}); goto _LL2108;} _LL2112: { struct _tuple1* c_name= _temp2135->name;
if(( void*) _temp2135->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct
_tuple1* _temp2176=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2176->f1= Cyc_Toc_abs_ns; _temp2176->f2=(* c_name).f2; _temp2176;});} nv=
Cyc_Toc_add_varmap( nv, _temp2135->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2135->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2135); res=({ struct Cyc_List_List*
_temp2177=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2177->hd=( void*) d; _temp2177->tl= res; _temp2177;}); goto _LL2108;}
_LL2114:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2178=( char*)"letdecl"; struct _tagged_string
_temp2179; _temp2179.curr= _temp2178; _temp2179.base= _temp2178; _temp2179.last_plus_one=
_temp2178 + 8; _temp2179;})); goto _LL2108; _LL2116: Cyc_Toc_structdecl_to_c(
_temp2147); res=({ struct Cyc_List_List* _temp2180=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2180->hd=( void*) d; _temp2180->tl=
res; _temp2180;}); goto _LL2108; _LL2118: Cyc_Toc_uniondecl_to_c( _temp2149);
res=({ struct Cyc_List_List* _temp2181=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2181->hd=( void*) d; _temp2181->tl= res;
_temp2181;}); goto _LL2108; _LL2120: res= Cyc_Toc_tuniondecl_to_c( _temp2151,
res); goto _LL2108; _LL2122: res= Cyc_Toc_xtuniondecl_to_c( _temp2153, res);
goto _LL2108; _LL2124: Cyc_Toc_enumdecl_to_c( nv, _temp2155); res=({ struct Cyc_List_List*
_temp2182=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2182->hd=( void*) d; _temp2182->tl= res; _temp2182;}); goto _LL2108;
_LL2126: _temp2157->name= _temp2157->name; _temp2157->tvs= 0;( void*)( _temp2157->defn=(
void*) Cyc_Toc_typ_to_c_array(( void*) _temp2157->defn)); res=({ struct Cyc_List_List*
_temp2183=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2183->hd=( void*) d; _temp2183->tl= res; _temp2183;}); goto _LL2108;
_LL2128: _temp2163= _temp2159; goto _LL2130; _LL2130: _temp2167= _temp2163; goto
_LL2132; _LL2132: { struct _tuple17 p= Cyc_Toc_decls_to_c( nv, _temp2167, top);
nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( p.f2), res); goto _LL2108;} _LL2108:;} return({ struct
_tuple17 _temp2184; _temp2184.f1= nv; _temp2184.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp2184;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunions_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List*
_temp2187; struct Cyc_Toc_Env* _temp2189; struct _tuple17 _temp2185= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL2190: _temp2189= _temp2185.f1; goto _LL2188;
_LL2188: _temp2187= _temp2185.f2; goto _LL2186; _LL2186: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp2187);}}