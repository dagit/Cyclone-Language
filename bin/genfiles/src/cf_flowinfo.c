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
; typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
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
char* tag; } ; extern void* Cyc_Tcutil_compress( void* t); typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
const int Cyc_CfFlowInfo_VarRoot_tag= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_CfFlowInfo_MallocPt_tag=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_CfFlowInfo_StructF_tag= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; const int Cyc_CfFlowInfo_TupleF_tag= 1;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; const unsigned int Cyc_CfFlowInfo_Esc=
0; const unsigned int Cyc_CfFlowInfo_Unesc= 1; const unsigned int Cyc_CfFlowInfo_NoneIL=
0; const unsigned int Cyc_CfFlowInfo_ThisIL= 1; const unsigned int Cyc_CfFlowInfo_AllIL=
2; const int Cyc_CfFlowInfo_UnknownIS_tag= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; const int Cyc_CfFlowInfo_MustPointTo_tag= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; const int Cyc_CfFlowInfo_LeafPI_tag= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; const int Cyc_CfFlowInfo_TuplePI_tag= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; const int Cyc_CfFlowInfo_StructPI_tag= 2;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
const unsigned int Cyc_CfFlowInfo_BottomFL= 0; const int Cyc_CfFlowInfo_InitsFL_tag=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int
Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*);
extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_noneil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_thisil={
0u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_allil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_noneil={
0u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_thisil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_allil={
0u,( void*)(( void*) 0u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)}; void* Cyc_CfFlowInfo_leaf_unesc_none=(
void*)& Cyc_CfFlowInfo_leaf_unesc_all_v; void* Cyc_CfFlowInfo_leaf_unesc_this=(
void*)& Cyc_CfFlowInfo_leaf_unesc_this_v; void* Cyc_CfFlowInfo_leaf_unesc_all=(
void*)& Cyc_CfFlowInfo_leaf_unesc_all_v; void* Cyc_CfFlowInfo_leaf_esc_none=(
void*)& Cyc_CfFlowInfo_leaf_esc_none_v; void* Cyc_CfFlowInfo_leaf_esc_this=(
void*)& Cyc_CfFlowInfo_leaf_esc_this_v; void* Cyc_CfFlowInfo_leaf_esc_all=( void*)&
Cyc_CfFlowInfo_leaf_esc_all_v; void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void*
il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc; _temp12.f2=
il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void* _temp33; void*
_temp35; void* _temp37; void* _temp39; void* _temp41; void* _temp43; void*
_temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29= _temp13.f1; if((
int) _temp29 == Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{ goto _LL17;} _LL28:
_temp27= _temp13.f2; if(( int) _temp27 == Cyc_CfFlowInfo_NoneIL){ goto _LL16;}
else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if(( int) _temp33 == Cyc_CfFlowInfo_Unesc){
goto _LL32;} else{ goto _LL19;} _LL32: _temp31= _temp13.f2; if(( int) _temp31 ==
Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{ goto _LL19;} _LL19: _LL38: _temp37=
_temp13.f1; if(( int) _temp37 == Cyc_CfFlowInfo_Unesc){ goto _LL36;} else{ goto
_LL21;} _LL36: _temp35= _temp13.f2; if(( int) _temp35 == Cyc_CfFlowInfo_AllIL){
goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41= _temp13.f1; if(( int)
_temp41 == Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto _LL23;} _LL40: _temp39=
_temp13.f2; if(( int) _temp39 == Cyc_CfFlowInfo_NoneIL){ goto _LL22;} else{ goto
_LL23;} _LL23: _LL46: _temp45= _temp13.f1; if(( int) _temp45 == Cyc_CfFlowInfo_Esc){
goto _LL44;} else{ goto _LL25;} _LL44: _temp43= _temp13.f2; if(( int) _temp43 ==
Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{ goto _LL25;} _LL25: _LL50: _temp49=
_temp13.f1; if(( int) _temp49 == Cyc_CfFlowInfo_Esc){ goto _LL48;} else{ goto
_LL14;} _LL48: _temp47= _temp13.f2; if(( int) _temp47 == Cyc_CfFlowInfo_AllIL){
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
struct _tunion_struct*) _temp66)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL69:
_temp68=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(((
struct _tunion_struct*) _temp62)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL65:
_temp64=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if((( struct _tunion_struct*) _temp74)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL77: _temp76=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(((
struct _tunion_struct*) _temp70)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL73:
_temp72=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if((( struct _tunion_struct*) _temp82)->tag == Cyc_CfFlowInfo_VarRoot_tag){
_LL85: _temp84=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(((
struct _tunion_struct*) _temp78)->tag == Cyc_CfFlowInfo_VarRoot_tag){ _LL81:
_temp80=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if((( struct _tunion_struct*) _temp90)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL93: _temp92=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(((
struct _tunion_struct*) _temp86)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL89:
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
_temp109= _temp95.f1; if((( struct _tunion_struct*) _temp109)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL112: _temp111=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp109)->f1; goto _LL106;} else{ goto _LL99;} _LL106: _temp105= _temp95.f2;
if((( struct _tunion_struct*) _temp105)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL108: _temp107=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1;
goto _LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if(((
struct _tunion_struct*) _temp117)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL120:
_temp119=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp117)->f1; goto
_LL114;} else{ goto _LL101;} _LL114: _temp113= _temp95.f2; if((( struct
_tunion_struct*) _temp113)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL116: _temp115=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1;
goto _LL100;} else{ goto _LL101;} _LL101: _LL126: _temp125= _temp95.f1; if(((
struct _tunion_struct*) _temp125)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL128:
_temp127=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp125)->f1; goto _LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2;
if((( struct _tunion_struct*) _temp121)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL124: _temp123=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp121)->f1; goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133=
_temp95.f1; if((( struct _tunion_struct*) _temp133)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL136: _temp135=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1;
goto _LL130;} else{ goto _LL96;} _LL130: _temp129= _temp95.f2; if((( struct
_tunion_struct*) _temp129)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL132: _temp131=(
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
struct _tunion_struct*) _temp137)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL146:
_temp145=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp137)->f1;
goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 4u?((
struct _tunion_struct*) _temp137)->tag == Cyc_Absyn_StructType_tag: 0){ _LL167:
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
d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp171));}} return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp175=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp175[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp176; _temp176.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp176.f1= d; _temp176;}); _temp175;});} _LL142: { struct Cyc_Dict_Dict* d=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); for( 0; _temp156 !=
0; _temp156= _temp156->tl){ struct Cyc_Absyn_Structfield _temp179; struct Cyc_List_List*
_temp180; struct Cyc_Core_Opt* _temp182; void* _temp184; struct Cyc_Absyn_Tqual*
_temp186; struct _tagged_string* _temp188; struct Cyc_Absyn_Structfield*
_temp177=( struct Cyc_Absyn_Structfield*) _temp156->hd; _temp179=* _temp177;
_LL189: _temp188=( struct _tagged_string*) _temp179.name; goto _LL187; _LL187:
_temp186=( struct Cyc_Absyn_Tqual*) _temp179.tq; goto _LL185; _LL185: _temp184=(
void*) _temp179.type; goto _LL183; _LL183: _temp182=( struct Cyc_Core_Opt*)
_temp179.width; goto _LL181; _LL181: _temp180=( struct Cyc_List_List*) _temp179.attributes;
goto _LL178; _LL178: if( Cyc_String_strcmp(* _temp188,( struct _tagged_string)({
char* _temp190=( char*)""; struct _tagged_string _temp191; _temp191.curr=
_temp190; _temp191.base= _temp190; _temp191.last_plus_one= _temp190 + 1;
_temp191;})) != 0){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, void* data)) Cyc_Dict_insert)( d, _temp188, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp184));}} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp192=(
struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp192[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp193; _temp193.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp193.f1= d; _temp193;}); _temp192;});} _LL144:
return Cyc_CfFlowInfo_leaf_unesc_none; _LL138:;} void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place){ struct Cyc_CfFlowInfo_Place
_temp196; struct Cyc_List_List* _temp197; void* _temp199; struct Cyc_CfFlowInfo_Place*
_temp194= place; _temp196=* _temp194; _LL200: _temp199=( void*) _temp196.root;
goto _LL198; _LL198: _temp197=( struct Cyc_List_List*) _temp196.fields; goto
_LL195; _LL195: { void* ans=(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( d, _temp199); for( 0; _temp197 != 0; _temp197= _temp197->tl){
struct _tuple3 _temp202=({ struct _tuple3 _temp201; _temp201.f1= ans; _temp201.f2=(
void*) _temp197->hd; _temp201;}); void* _temp210; int _temp212; void* _temp214;
struct Cyc_Dict_Dict* _temp216; void* _temp218; struct _tagged_string* _temp220;
void* _temp222; struct Cyc_Dict_Dict* _temp224; _LL204: _LL215: _temp214=
_temp202.f1; if((( struct _tunion_struct*) _temp214)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL217: _temp216=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp214)->f1; goto _LL211;} else{ goto _LL206;} _LL211: _temp210= _temp202.f2;
if((( struct _tunion_struct*) _temp210)->tag == Cyc_CfFlowInfo_TupleF_tag){
_LL213: _temp212=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp210)->f1;
goto _LL205;} else{ goto _LL206;} _LL206: _LL223: _temp222= _temp202.f1; if(((
struct _tunion_struct*) _temp222)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL225:
_temp224=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp222)->f1; goto _LL219;} else{ goto _LL208;} _LL219: _temp218= _temp202.f2;
if((( struct _tunion_struct*) _temp218)->tag == Cyc_CfFlowInfo_StructF_tag){
_LL221: _temp220=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp218)->f1; goto _LL207;} else{ goto _LL208;} _LL208: goto _LL209; _LL205:
ans=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp216,
_temp212); goto _LL203; _LL207: ans=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp224, _temp220); goto _LL203; _LL209:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp226=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp226[ 0]=({ struct Cyc_Core_Impossible_struct _temp227; _temp227.tag= Cyc_Core_Impossible_tag;
_temp227.f1=( struct _tagged_string)({ char* _temp228=( char*)"bad lookup_place";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 17; _temp229;}); _temp227;}); _temp226;}));
_LL203:;} return ans;}} void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path,
struct Cyc_List_List* fields, void* pinfo){ if( fields == 0){ return pinfo;}{
struct _tuple5 _temp231=({ struct _tuple5 _temp230; _temp230.f1= insert_path;
_temp230.f2= fields; _temp230;}); struct Cyc_List_List* _temp239; struct Cyc_List_List
_temp241; struct Cyc_List_List* _temp242; void* _temp244; int _temp246; void*
_temp248; struct Cyc_Dict_Dict* _temp250; struct Cyc_List_List* _temp252; struct
Cyc_List_List _temp254; struct Cyc_List_List* _temp255; void* _temp257; struct
_tagged_string* _temp259; void* _temp261; struct Cyc_Dict_Dict* _temp263; _LL233:
_LL249: _temp248= _temp231.f1; if((( struct _tunion_struct*) _temp248)->tag ==
Cyc_CfFlowInfo_TuplePI_tag){ _LL251: _temp250=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp248)->f1; goto _LL240;} else{ goto _LL235;}
_LL240: _temp239= _temp231.f2; if( _temp239 == 0){ goto _LL235;} else{ _temp241=*
_temp239; _LL245: _temp244=( void*) _temp241.hd; if((( struct _tunion_struct*)
_temp244)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL247: _temp246=( int)(( struct
Cyc_CfFlowInfo_TupleF_struct*) _temp244)->f1; goto _LL243;} else{ goto _LL235;}
_LL243: _temp242=( struct Cyc_List_List*) _temp241.tl; goto _LL234;} _LL235:
_LL262: _temp261= _temp231.f1; if((( struct _tunion_struct*) _temp261)->tag ==
Cyc_CfFlowInfo_StructPI_tag){ _LL264: _temp263=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp261)->f1; goto _LL253;} else{ goto _LL237;}
_LL253: _temp252= _temp231.f2; if( _temp252 == 0){ goto _LL237;} else{ _temp254=*
_temp252; _LL258: _temp257=( void*) _temp254.hd; if((( struct _tunion_struct*)
_temp257)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL260: _temp259=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp257)->f1; goto
_LL256;} else{ goto _LL237;} _LL256: _temp255=( struct Cyc_List_List*) _temp254.tl;
goto _LL236;} _LL237: goto _LL238; _LL234: { void* next_path=(( void*(*)( struct
Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp250, _temp246); return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp265=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp265[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp266; _temp266.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp266.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp250, _temp246, Cyc_CfFlowInfo_insert_place_rec(
next_path, _temp242, pinfo)); _temp266;}); _temp265;});} _LL236: { void*
next_path=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp263, _temp259); return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp267=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp267[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp268; _temp268.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp268.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp263, _temp259,
Cyc_CfFlowInfo_insert_place_rec( next_path, _temp255, pinfo)); _temp268;});
_temp267;});} _LL238:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp269=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp269[ 0]=({ struct Cyc_Core_Impossible_struct _temp270; _temp270.tag= Cyc_Core_Impossible_tag;
_temp270.f1=( struct _tagged_string)({ char* _temp271=( char*)"bad insert_place";
struct _tagged_string _temp272; _temp272.curr= _temp271; _temp272.base= _temp271;
_temp272.last_plus_one= _temp271 + 17; _temp272;}); _temp270;}); _temp269;}));
_LL232:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){ struct Cyc_CfFlowInfo_Place
_temp275; struct Cyc_List_List* _temp276; void* _temp278; struct Cyc_CfFlowInfo_Place*
_temp273= place; _temp275=* _temp273; _LL279: _temp278=( void*) _temp275.root;
goto _LL277; _LL277: _temp276=( struct Cyc_List_List*) _temp275.fields; goto
_LL274; _LL274: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
key, void* data)) Cyc_Dict_insert)( d, _temp278, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp278),
_temp276, pinfo));} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env,
void* path); int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){
void* _temp280= state; void* _temp288; void* _temp290; void* _temp292; void*
_temp294; struct Cyc_CfFlowInfo_Place* _temp296; _LL282: if((( struct
_tunion_struct*) _temp280)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL291:
_temp290=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp280)->f1; goto
_LL289; _LL289: _temp288=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp280)->f2; if(( int) _temp288 == Cyc_CfFlowInfo_AllIL){ goto _LL283;} else{
goto _LL284;}} else{ goto _LL284;} _LL284: if((( struct _tunion_struct*)
_temp280)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL295: _temp294=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp280)->f1; goto _LL293; _LL293:
_temp292=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp280)->f2; goto
_LL285;} else{ goto _LL286;} _LL286: if((( struct _tunion_struct*) _temp280)->tag
== Cyc_CfFlowInfo_MustPointTo_tag){ _LL297: _temp296=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp280)->f1; goto _LL287;} else{
goto _LL281;} _LL283: return 1; _LL285: return 0; _LL287: { struct _tuple6
_temp300; struct Cyc_List_List* _temp301; struct Cyc_List_List** _temp303;
struct Cyc_Dict_Dict* _temp304; struct _tuple6* _temp298= env; _temp300=*
_temp298; _LL305: _temp304= _temp300.f1; goto _LL302; _LL302: _temp301= _temp300.f2;
_temp303=&(* _temp298).f2; goto _LL299; _LL299: if((( int(*)( int(* compare)(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*), struct Cyc_List_List*
l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp303, _temp296)){ return 1;}* _temp303=({ struct Cyc_List_List* _temp306=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp306[
0]=({ struct Cyc_List_List _temp307; _temp307.hd=( void*) _temp296; _temp307.tl=*
_temp303; _temp307;}); _temp306;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp304, _temp296));} _LL281:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp308= path; void* _temp316; struct Cyc_Dict_Dict* _temp318; struct Cyc_Dict_Dict*
_temp320; _LL310: if((( struct _tunion_struct*) _temp308)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL317: _temp316=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp308)->f1;
goto _LL311;} else{ goto _LL312;} _LL312: if((( struct _tunion_struct*) _temp308)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL319: _temp318=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp308)->f1; goto _LL313;} else{ goto
_LL314;} _LL314: if((( struct _tunion_struct*) _temp308)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL321: _temp320=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp308)->f1; goto _LL315;} else{ goto _LL309;} _LL311: return Cyc_CfFlowInfo_isAllInit_is(
env, _temp316); _LL313: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp318); _LL315: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_string*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp320); _LL309:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 isInitEnv=({ struct _tuple6 _temp322;
_temp322.f1= pinfo_dict; _temp322.f2= 0; _temp322;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, pinfo);} void* Cyc_CfFlowInfo_pinfo_getplaces_esc( struct _tuple7*
env, void* pinfo){ struct _tuple7 _temp325; struct Cyc_Set_Set** _temp326;
struct Cyc_Dict_Dict* _temp328; struct _tuple7* _temp323= env; _temp325=*
_temp323; _LL329: _temp328= _temp325.f1; goto _LL327; _LL327: _temp326= _temp325.f2;
goto _LL324; _LL324: { void* _temp330= pinfo; void* _temp340; struct Cyc_CfFlowInfo_Place*
_temp342; void* _temp344; void* _temp346; void* _temp348; struct Cyc_Dict_Dict*
_temp350; struct Cyc_Dict_Dict* _temp352; _LL332: if((( struct _tunion_struct*)
_temp330)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL341: _temp340=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp330)->f1; if((( struct _tunion_struct*)
_temp340)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL343: _temp342=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp340)->f1; goto _LL333;} else{
goto _LL334;}} else{ goto _LL334;} _LL334: if((( struct _tunion_struct*)
_temp330)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL345: _temp344=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp330)->f1; if((( struct _tunion_struct*)
_temp344)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL349: _temp348=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp344)->f1; goto _LL347; _LL347:
_temp346=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp344)->f2; goto
_LL335;} else{ goto _LL336;}} else{ goto _LL336;} _LL336: if((( struct
_tunion_struct*) _temp330)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL351: _temp350=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp330)->f1;
goto _LL337;} else{ goto _LL338;} _LL338: if((( struct _tunion_struct*) _temp330)->tag
== Cyc_CfFlowInfo_StructPI_tag){ _LL353: _temp352=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp330)->f1; goto _LL339;} else{ goto
_LL331;} _LL333:* _temp326=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* _temp326, _temp342); if(
Cyc_CfFlowInfo_isAllInit( _temp328, Cyc_CfFlowInfo_lookup_place( _temp328,
_temp342))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL335: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp346); _LL337: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp354=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp354[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp355; _temp355.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp355.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp350); _temp355;}); _temp354;}); _LL339: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp356=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp356[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp357; _temp357.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp357.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp352); _temp357;}); _temp356;});
_LL331:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set** places_ptr,
void* a, void* pinfo){ void* _temp358= pinfo; void* _temp368; void* _temp370;
void* _temp372; void* _temp374; struct Cyc_CfFlowInfo_Place* _temp376; struct
Cyc_Dict_Dict* _temp378; struct Cyc_Dict_Dict* _temp380; _LL360: if((( struct
_tunion_struct*) _temp358)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL369: _temp368=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp358)->f1; if((( struct
_tunion_struct*) _temp368)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL373:
_temp372=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp368)->f1; goto
_LL371; _LL371: _temp370=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp368)->f2; goto _LL361;} else{ goto _LL362;}} else{ goto _LL362;} _LL362:
if((( struct _tunion_struct*) _temp358)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL375: _temp374=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp358)->f1;
if((( struct _tunion_struct*) _temp374)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL377: _temp376=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp374)->f1; goto _LL363;} else{ goto _LL364;}} else{ goto _LL364;} _LL364:
if((( struct _tunion_struct*) _temp358)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL379: _temp378=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp358)->f1; goto _LL365;} else{ goto _LL366;} _LL366: if((( struct
_tunion_struct*) _temp358)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL381:
_temp380=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp358)->f1; goto _LL367;} else{ goto _LL359;} _LL361: return; _LL363:*
places_ptr=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(* places_ptr, _temp376); return; _LL365:(( void(*)( void(*
f)( struct Cyc_Set_Set**, int, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, int a, void*
pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr, _temp378); return; _LL367:((
void(*)( void(* f)( struct Cyc_Set_Set**, struct _tagged_string*, void*), struct
Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
Cyc_Set_Set** places_ptr, struct _tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp380); return; _LL359:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_Set_Set* places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct
Cyc_Set_Set* s)) Cyc_Set_is_empty)( places)) { struct Cyc_CfFlowInfo_Place*
place=(( struct Cyc_CfFlowInfo_Place*(*)( struct Cyc_Set_Set* s)) Cyc_Set_choose)(
places); places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_delete)( places, place);{ void* place_old_pinfo;{ struct
_handler_cons _temp382; _push_handler(& _temp382);{ struct _xtunion_struct*
_temp383=( struct _xtunion_struct*) setjmp( _temp382.handler); if( ! _temp383){
place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, place);; _pop_handler();} else{
struct _xtunion_struct* _temp385= _temp383; _LL387: if( _temp385->tag == Cyc_Dict_Absent_tag){
goto _LL388;} else{ goto _LL389;} _LL389: goto _LL390; _LL388: continue; _LL390:(
void) _throw( _temp385); _LL386:;}}}{ struct _tuple7 places_env=({ struct
_tuple7 _temp391; _temp391.f1= d; _temp391.f2=& places; _temp391;}); void*
place_new_pinfo= Cyc_CfFlowInfo_pinfo_getplaces_esc(& places_env,
place_old_pinfo); if( place_old_pinfo != place_new_pinfo){ d= Cyc_CfFlowInfo_insert_place(
d, place, place_new_pinfo);}}}} return d;} static struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr=
0; struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict*
d){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp392=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp392[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp392;});}{ struct Cyc_Set_Set* places=* Cyc_CfFlowInfo_empty_place_set_ptr;((
void(*)( struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
places, 0, pinfo); return Cyc_CfFlowInfo_escape_these( places, d);}} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp393= pinfo; void* _temp403; void* _temp405;
void* _temp407; void* _temp409; struct Cyc_Dict_Dict* _temp411; struct Cyc_Dict_Dict*
_temp413; _LL395: if((( struct _tunion_struct*) _temp393)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL404: _temp403=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp393)->f1;
if((( struct _tunion_struct*) _temp403)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL408: _temp407=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp403)->f1;
if(( int) _temp407 == Cyc_CfFlowInfo_Esc){ goto _LL406;} else{ goto _LL397;}
_LL406: _temp405=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp403)->f2;
goto _LL396;} else{ goto _LL397;}} else{ goto _LL397;} _LL397: if((( struct
_tunion_struct*) _temp393)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL410: _temp409=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp393)->f1; goto _LL398;}
else{ goto _LL399;} _LL399: if((( struct _tunion_struct*) _temp393)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL412: _temp411=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp393)->f1; goto _LL400;} else{ goto _LL401;} _LL401: if((( struct
_tunion_struct*) _temp393)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL414:
_temp413=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp393)->f1; goto _LL402;} else{ goto _LL394;} _LL396: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc, level); _LL398: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Unesc, level); _LL400: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp415=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp415[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp416; _temp416.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp416.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp411); _temp416;}); _temp415;}); _LL402: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp417=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp417[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp418; _temp418.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp418.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp413); _temp418;}); _temp417;}); _LL394:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_Set_Set** places;
} ; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t; void* Cyc_CfFlowInfo_join_escape(
void* esc1, void* esc2){ struct _tuple3 _temp420=({ struct _tuple3 _temp419;
_temp419.f1= esc1; _temp419.f2= esc2; _temp419;}); void* _temp428; void*
_temp430; void* _temp432; void* _temp434; _LL422: _LL431: _temp430= _temp420.f1;
if(( int) _temp430 == Cyc_CfFlowInfo_Esc){ goto _LL429;} else{ goto _LL424;}
_LL429: _temp428= _temp420.f2; goto _LL423; _LL424: _LL435: _temp434= _temp420.f1;
goto _LL433; _LL433: _temp432= _temp420.f2; if(( int) _temp432 == Cyc_CfFlowInfo_Esc){
goto _LL425;} else{ goto _LL426;} _LL426: goto _LL427; _LL423: goto _LL425;
_LL425: return( void*) Cyc_CfFlowInfo_Esc; _LL427: return( void*) Cyc_CfFlowInfo_Unesc;
_LL421:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp437=({ struct _tuple3 _temp436; _temp436.f1= il1; _temp436.f2= il2;
_temp436;}); void* _temp449; void* _temp451; void* _temp453; void* _temp455;
void* _temp457; void* _temp459; void* _temp461; void* _temp463; _LL439: _LL452:
_temp451= _temp437.f1; if(( int) _temp451 == Cyc_CfFlowInfo_NoneIL){ goto _LL450;}
else{ goto _LL441;} _LL450: _temp449= _temp437.f2; goto _LL440; _LL441: _LL456:
_temp455= _temp437.f1; goto _LL454; _LL454: _temp453= _temp437.f2; if(( int)
_temp453 == Cyc_CfFlowInfo_NoneIL){ goto _LL442;} else{ goto _LL443;} _LL443:
_LL460: _temp459= _temp437.f1; if(( int) _temp459 == Cyc_CfFlowInfo_ThisIL){
goto _LL458;} else{ goto _LL445;} _LL458: _temp457= _temp437.f2; goto _LL444;
_LL445: _LL464: _temp463= _temp437.f1; goto _LL462; _LL462: _temp461= _temp437.f2;
if(( int) _temp461 == Cyc_CfFlowInfo_ThisIL){ goto _LL446;} else{ goto _LL447;}
_LL447: goto _LL448; _LL440: goto _LL442; _LL442: return( void*) Cyc_CfFlowInfo_NoneIL;
_LL444: goto _LL446; _LL446: return( void*) Cyc_CfFlowInfo_ThisIL; _LL448:
return( void*) Cyc_CfFlowInfo_AllIL; _LL438:;} void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, void* state1, void* state2){ struct _tuple3
_temp466=({ struct _tuple3 _temp465; _temp465.f1= state1; _temp465.f2= state2;
_temp465;}); void* _temp476; void* _temp478; void* _temp480; void* _temp482;
void* _temp484; void* _temp486; void* _temp488; struct Cyc_CfFlowInfo_Place*
_temp490; void* _temp492; void* _temp494; void* _temp496; void* _temp498; void*
_temp500; void* _temp502; void* _temp504; struct Cyc_CfFlowInfo_Place* _temp506;
void* _temp508; struct Cyc_CfFlowInfo_Place* _temp510; void* _temp512; struct
Cyc_CfFlowInfo_Place* _temp514; _LL468: _LL483: _temp482= _temp466.f1; if(((
struct _tunion_struct*) _temp482)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL487:
_temp486=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp482)->f1; goto
_LL485; _LL485: _temp484=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp482)->f2; goto _LL477;} else{ goto _LL470;} _LL477: _temp476= _temp466.f2;
if((( struct _tunion_struct*) _temp476)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL481: _temp480=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp476)->f1;
goto _LL479; _LL479: _temp478=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp476)->f2; goto _LL469;} else{ goto _LL470;} _LL470: _LL493: _temp492=
_temp466.f1; if((( struct _tunion_struct*) _temp492)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL497: _temp496=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp492)->f1;
goto _LL495; _LL495: _temp494=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp492)->f2; goto _LL489;} else{ goto _LL472;} _LL489: _temp488= _temp466.f2;
if((( struct _tunion_struct*) _temp488)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL491: _temp490=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp488)->f1; goto _LL471;} else{ goto _LL472;} _LL472: _LL505: _temp504=
_temp466.f1; if((( struct _tunion_struct*) _temp504)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL507: _temp506=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp504)->f1; goto _LL499;} else{ goto _LL474;} _LL499: _temp498= _temp466.f2;
if((( struct _tunion_struct*) _temp498)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL503: _temp502=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp498)->f1;
goto _LL501; _LL501: _temp500=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp498)->f2; goto _LL473;} else{ goto _LL474;} _LL474: _LL513: _temp512=
_temp466.f1; if((( struct _tunion_struct*) _temp512)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL515: _temp514=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp512)->f1; goto _LL509;} else{ goto _LL467;} _LL509: _temp508= _temp466.f2;
if((( struct _tunion_struct*) _temp508)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL511: _temp510=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp508)->f1; goto _LL475;} else{ goto _LL467;} _LL469: return Cyc_CfFlowInfo_mkLeafPI(
Cyc_CfFlowInfo_join_escape( _temp486, _temp480), Cyc_CfFlowInfo_join_initlevel(
_temp484, _temp478)); _LL471: { struct _tuple6 isInitEnv=({ struct _tuple6
_temp516; _temp516.f1= env->d2; _temp516.f2= 0; _temp516;}); if( _temp494 ==(
void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(& isInitEnv, Cyc_CfFlowInfo_lookup_place(
env->d2, _temp490)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp490);
return Cyc_CfFlowInfo_mkLeafPI( _temp496, _temp494);} return Cyc_CfFlowInfo_mkLeafPI(
_temp496, Cyc_CfFlowInfo_join_initlevel( _temp494,( void*) Cyc_CfFlowInfo_ThisIL));}
_LL473: { struct _tuple6 isInitEnv=({ struct _tuple6 _temp517; _temp517.f1= env->d1;
_temp517.f2= 0; _temp517;}); if( _temp500 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d1, _temp506)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp506); return Cyc_CfFlowInfo_mkLeafPI(
_temp502, _temp500);} return Cyc_CfFlowInfo_mkLeafPI( _temp502, Cyc_CfFlowInfo_join_initlevel((
void*) Cyc_CfFlowInfo_ThisIL, _temp500));} _LL475: if( Cyc_CfFlowInfo_place_cmp(
_temp514, _temp510) == 0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp518=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp518[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp519; _temp519.tag= Cyc_CfFlowInfo_LeafPI_tag;
_temp519.f1=( void*) state1; _temp519;}); _temp518;});}{ struct _tuple6
isInitEnv1=({ struct _tuple6 _temp520; _temp520.f1= env->d1; _temp520.f2= 0;
_temp520;}); struct _tuple6 isInitEnv2=({ struct _tuple6 _temp521; _temp521.f1=
env->d2; _temp521.f2= 0; _temp521;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv1, Cyc_CfFlowInfo_lookup_place( env->d1, _temp514))? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv2, Cyc_CfFlowInfo_lookup_place( env->d2, _temp510)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp514);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp510); return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,(
void*) Cyc_CfFlowInfo_ThisIL);} _LL467:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp523=({ struct _tuple3 _temp522;
_temp522.f1= pinfo1; _temp522.f2= pinfo2; _temp522;}); void* _temp533; void*
_temp535; void* _temp537; void* _temp539; void* _temp541; struct Cyc_Dict_Dict*
_temp543; void* _temp545; struct Cyc_Dict_Dict* _temp547; void* _temp549; struct
Cyc_Dict_Dict* _temp551; void* _temp553; struct Cyc_Dict_Dict* _temp555; _LL525:
_LL538: _temp537= _temp523.f1; if((( struct _tunion_struct*) _temp537)->tag ==
Cyc_CfFlowInfo_LeafPI_tag){ _LL540: _temp539=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp537)->f1; goto _LL534;} else{ goto _LL527;} _LL534: _temp533= _temp523.f2;
if((( struct _tunion_struct*) _temp533)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL536: _temp535=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp533)->f1;
goto _LL526;} else{ goto _LL527;} _LL527: _LL546: _temp545= _temp523.f1; if(((
struct _tunion_struct*) _temp545)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL548:
_temp547=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp545)->f1; goto _LL542;} else{ goto _LL529;} _LL542: _temp541= _temp523.f2;
if((( struct _tunion_struct*) _temp541)->tag == Cyc_CfFlowInfo_TuplePI_tag){
_LL544: _temp543=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp541)->f1; goto _LL528;} else{ goto _LL529;} _LL529: _LL554: _temp553=
_temp523.f1; if((( struct _tunion_struct*) _temp553)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL556: _temp555=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp553)->f1; goto _LL550;} else{ goto _LL531;} _LL550: _temp549= _temp523.f2;
if((( struct _tunion_struct*) _temp549)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL552: _temp551=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp549)->f1; goto _LL530;} else{ goto _LL531;} _LL531: goto _LL532; _LL526:
if( _temp539 == _temp535){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp539, _temp535); _LL528: if( _temp547 == _temp543){ return pinfo1;}
return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp557=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp557[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp558; _temp558.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp558.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp547, _temp543); _temp558;}); _temp557;}); _LL530: if( _temp555 ==
_temp551){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp559=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp559[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp560; _temp560.tag=
Cyc_CfFlowInfo_StructPI_tag; _temp560.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp555, _temp551); _temp560;}); _temp559;});
_LL532:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp561=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp561[ 0]=({ struct Cyc_Core_Impossible_struct _temp562; _temp562.tag= Cyc_Core_Impossible_tag;
_temp562.f1=( struct _tagged_string)({ char* _temp563=( char*)"join_pathinfo";
struct _tagged_string _temp564; _temp564.curr= _temp563; _temp564.base= _temp563;
_temp564.last_plus_one= _temp563 + 14; _temp564;}); _temp562;}); _temp561;}));
_LL524:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){ if( Cyc_CfFlowInfo_empty_place_set_ptr
== 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({ struct Cyc_Set_Set** _temp565=(
struct Cyc_Set_Set**) GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp565[ 0]=((
struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp565;});} if( f1 == f2){ return
f1;}{ struct _tuple3 _temp567=({ struct _tuple3 _temp566; _temp566.f1= f1;
_temp566.f2= f2; _temp566;}); void* _temp575; void* _temp577; void* _temp579;
void* _temp581; void* _temp583; struct Cyc_Dict_Dict* _temp585; void* _temp587;
struct Cyc_Dict_Dict* _temp589; _LL569: _LL578: _temp577= _temp567.f1; if(( int)
_temp577 == Cyc_CfFlowInfo_BottomFL){ goto _LL576;} else{ goto _LL571;} _LL576:
_temp575= _temp567.f2; goto _LL570; _LL571: _LL582: _temp581= _temp567.f1; goto
_LL580; _LL580: _temp579= _temp567.f2; if(( int) _temp579 == Cyc_CfFlowInfo_BottomFL){
goto _LL572;} else{ goto _LL573;} _LL573: _LL588: _temp587= _temp567.f1; if((
unsigned int) _temp587 > 1u?(( struct _tunion_struct*) _temp587)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL590: _temp589=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp587)->f1; goto _LL584;} else{ goto _LL568;} _LL584: _temp583= _temp567.f2;
if(( unsigned int) _temp583 > 1u?(( struct _tunion_struct*) _temp583)->tag ==
Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL586: _temp585=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp583)->f1; goto _LL574;} else{ goto
_LL568;} _LL570: return f2; _LL572: return f1; _LL574: if( _temp589 == _temp585){
return f1;}{ struct Cyc_Set_Set* places=* Cyc_CfFlowInfo_empty_place_set_ptr;
struct Cyc_CfFlowInfo_JoinEnv joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp591;
_temp591.d1= _temp589; _temp591.d2= _temp585; _temp591.places=& places; _temp591;});
struct Cyc_Dict_Dict* outdict=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
joinenv, _temp589, _temp585); return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp592=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp592[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp593; _temp593.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp593.f1= Cyc_CfFlowInfo_escape_these( places, outdict); _temp593;});
_temp592;});} _LL568:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void*
state1, void* state2){ struct _tuple3 _temp595=({ struct _tuple3 _temp594;
_temp594.f1= state1; _temp594.f2= state2; _temp594;}); void* _temp603; void*
_temp605; void* _temp607; void* _temp609; void* _temp611; void* _temp613; void*
_temp615; struct Cyc_CfFlowInfo_Place* _temp617; void* _temp619; struct Cyc_CfFlowInfo_Place*
_temp621; _LL597: _LL610: _temp609= _temp595.f1; if((( struct _tunion_struct*)
_temp609)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL614: _temp613=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp609)->f1; goto _LL612; _LL612:
_temp611=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp609)->f2; goto
_LL604;} else{ goto _LL599;} _LL604: _temp603= _temp595.f2; if((( struct
_tunion_struct*) _temp603)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL608:
_temp607=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp603)->f1; goto
_LL606; _LL606: _temp605=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp603)->f2; goto _LL598;} else{ goto _LL599;} _LL599: _LL620: _temp619=
_temp595.f1; if((( struct _tunion_struct*) _temp619)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL622: _temp621=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp619)->f1; goto _LL616;} else{ goto _LL601;} _LL616: _temp615= _temp595.f2;
if((( struct _tunion_struct*) _temp615)->tag == Cyc_CfFlowInfo_MustPointTo_tag){
_LL618: _temp617=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp615)->f1; goto _LL600;} else{ goto _LL601;} _LL601: goto _LL602; _LL598:
return Cyc_CfFlowInfo_join_escape( _temp613, _temp607) == _temp607? Cyc_CfFlowInfo_join_initlevel(
_temp611, _temp605) == _temp605: 0; _LL600: return Cyc_CfFlowInfo_place_cmp(
_temp621, _temp617) == 0; _LL602: return 0; _LL596:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp624=({ struct _tuple3 _temp623; _temp623.f1= pinfo1;
_temp623.f2= pinfo2; _temp623;}); void* _temp634; void* _temp636; void* _temp638;
void* _temp640; void* _temp642; struct Cyc_Dict_Dict* _temp644; void* _temp646;
struct Cyc_Dict_Dict* _temp648; void* _temp650; struct Cyc_Dict_Dict* _temp652;
void* _temp654; struct Cyc_Dict_Dict* _temp656; _LL626: _LL639: _temp638=
_temp624.f1; if((( struct _tunion_struct*) _temp638)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL641: _temp640=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp638)->f1;
goto _LL635;} else{ goto _LL628;} _LL635: _temp634= _temp624.f2; if((( struct
_tunion_struct*) _temp634)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL637: _temp636=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp634)->f1; goto _LL627;}
else{ goto _LL628;} _LL628: _LL647: _temp646= _temp624.f1; if((( struct
_tunion_struct*) _temp646)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL649: _temp648=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp646)->f1;
goto _LL643;} else{ goto _LL630;} _LL643: _temp642= _temp624.f2; if((( struct
_tunion_struct*) _temp642)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL645: _temp644=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp642)->f1;
goto _LL629;} else{ goto _LL630;} _LL630: _LL655: _temp654= _temp624.f1; if(((
struct _tunion_struct*) _temp654)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL657:
_temp656=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp654)->f1; goto _LL651;} else{ goto _LL632;} _LL651: _temp650= _temp624.f2;
if((( struct _tunion_struct*) _temp650)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL653: _temp652=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp650)->f1; goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL627:
return _temp640 == _temp636? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp640, _temp636); _LL629: return _temp648 == _temp644? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp648, _temp644); _LL631: return _temp656 == _temp652? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp656, _temp652);
_LL633:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp658=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp658[ 0]=({ struct Cyc_Core_Impossible_struct _temp659; _temp659.tag= Cyc_Core_Impossible_tag;
_temp659.f1=( struct _tagged_string)({ char* _temp660=( char*)"pinfo_lessthan";
struct _tagged_string _temp661; _temp661.curr= _temp660; _temp661.base= _temp660;
_temp661.last_plus_one= _temp660 + 15; _temp661;}); _temp659;}); _temp658;}));
_LL625:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2){ if( f1
== f2){ return 1;}{ struct _tuple3 _temp663=({ struct _tuple3 _temp662; _temp662.f1=
f1; _temp662.f2= f2; _temp662;}); void* _temp671; void* _temp673; void* _temp675;
void* _temp677; void* _temp679; struct Cyc_Dict_Dict* _temp681; void* _temp683;
struct Cyc_Dict_Dict* _temp685; _LL665: _LL674: _temp673= _temp663.f1; if(( int)
_temp673 == Cyc_CfFlowInfo_BottomFL){ goto _LL672;} else{ goto _LL667;} _LL672:
_temp671= _temp663.f2; goto _LL666; _LL667: _LL678: _temp677= _temp663.f1; goto
_LL676; _LL676: _temp675= _temp663.f2; if(( int) _temp675 == Cyc_CfFlowInfo_BottomFL){
goto _LL668;} else{ goto _LL669;} _LL669: _LL684: _temp683= _temp663.f1; if((
unsigned int) _temp683 > 1u?(( struct _tunion_struct*) _temp683)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL686: _temp685=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp683)->f1; goto _LL680;} else{ goto _LL664;} _LL680: _temp679= _temp663.f2;
if(( unsigned int) _temp679 > 1u?(( struct _tunion_struct*) _temp679)->tag ==
Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL682: _temp681=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp679)->f1; goto _LL670;} else{ goto
_LL664;} _LL666: return 1; _LL668: return 0; _LL670: if( _temp685 == _temp681){
return 1;} return(( int(*)( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)(( int(*)( void* ignore,
void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp681,
_temp685); _LL664:;}}