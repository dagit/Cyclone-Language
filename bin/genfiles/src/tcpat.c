#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual*
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple9{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; }
; struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct
Cyc_List_List* f3; } ; struct _tuple11{ int f1; struct Cyc_Position_Segment* f2;
} ; struct _tuple12{ void* f1; struct _tuple11* f2; } ; struct _tuple13{ struct
Cyc_Position_Segment* f1; int f2; } ; struct _tuple14{ void* f1; int f2; } ;
typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
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
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env); struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef
unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
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
int tag; struct Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Tcenv_TunionRes_tag;
struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; extern const int Cyc_Tcenv_XTunionRes_tag;
struct Cyc_Tcenv_XTunionRes_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; extern const int Cyc_Tcenv_EnumRes_tag;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict*
xtuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs;
struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; } ; typedef
struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct
Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; typedef void* Cyc_Tcenv_Jumpee; extern const
unsigned int Cyc_Tcenv_NotLoop_j; extern const unsigned int Cyc_Tcenv_CaseEnd_j;
extern const unsigned int Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern char Cyc_Tcutil_TypeErr_tag[ 8u];
struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*,
void*); extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
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
int _temp35; void* _temp37; char _temp39; struct _tagged_string _temp41; struct
Cyc_Absyn_Vardecl* _temp43; struct Cyc_Absyn_Vardecl* _temp45; struct Cyc_Absyn_Pat*
_temp47; struct Cyc_List_List* _temp49; struct Cyc_List_List* _temp51; struct
Cyc_List_List* _temp53; struct Cyc_Core_Opt* _temp55; struct Cyc_Core_Opt**
_temp57; struct Cyc_Absyn_Structdecl* _temp58; struct Cyc_Absyn_Tunionfield*
_temp60; struct Cyc_Absyn_Tuniondecl* _temp62; struct Cyc_List_List* _temp64;
struct Cyc_List_List* _temp66; struct Cyc_Core_Opt* _temp68; struct _tuple1*
_temp70; struct Cyc_Absyn_Tunionfield* _temp72; struct Cyc_Absyn_XTuniondecl*
_temp74; struct Cyc_List_List* _temp76; struct Cyc_List_List* _temp78; struct
_tuple1* _temp80; struct Cyc_Absyn_Enumfield* _temp82; struct Cyc_Absyn_Enumdecl*
_temp84; struct _tuple1* _temp86; struct _tuple1* _temp88; struct Cyc_List_List*
_temp90; struct Cyc_List_List* _temp92; struct _tuple1* _temp94; struct Cyc_List_List*
_temp96; struct Cyc_List_List* _temp98; struct _tuple1* _temp100; _LL3: if(( int)
_temp1 == Cyc_Absyn_Wild_p){ goto _LL4;} else{ goto _LL5;} _LL5: if((
unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL38: _temp37=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1)->f1; goto
_LL36; _LL36: _temp35=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1)->f2; goto
_LL6;} else{ goto _LL7;} _LL7: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL40: _temp39=( char)((
struct Cyc_Absyn_Char_p_struct*) _temp1)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(( unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_Float_p_tag:
0){ _LL42: _temp41=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*)
_temp1)->f1; goto _LL10;} else{ goto _LL11;} _LL11: if(( int) _temp1 == Cyc_Absyn_Null_p){
goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL44: _temp43=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp1)->f1; goto
_LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL46: _temp45=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp1)->f1;
goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL48: _temp47=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp1)->f1; goto
_LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL50: _temp49=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1)->f1; goto
_LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL59: _temp58=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1)->f1;
goto _LL56; _LL56: _temp55=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f2; _temp57=&(( struct Cyc_Absyn_Struct_p_struct*) _temp1)->f2; goto
_LL54; _LL54: _temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f3; goto _LL52; _LL52: _temp51=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1)->f4; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp1 >
2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL71:
_temp70=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1)->f1;
goto _LL69; _LL69: _temp68=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1)->f2; goto _LL67; _LL67: _temp66=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1)->f3; goto _LL65; _LL65: _temp64=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1)->f4; goto _LL63; _LL63: _temp62=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp1)->f5; goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1)->f6; goto _LL24;} else{ goto _LL25;}
_LL25: if(( unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL81: _temp80=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1)->f1; goto _LL79; _LL79: _temp78=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1)->f2; goto _LL77; _LL77: _temp76=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1)->f3; goto _LL75; _LL75: _temp74=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1)->f4; goto _LL73; _LL73: _temp72=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1)->f5;
goto _LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp1 > 2u?(( struct
_tunion_struct*) _temp1)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL87: _temp86=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1)->f1; goto _LL85;
_LL85: _temp84=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1)->f2; goto _LL83; _LL83: _temp82=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Absyn_Enum_p_struct*) _temp1)->f3; goto _LL28;} else{ goto _LL29;} _LL29:
if(( unsigned int) _temp1 > 2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL89: _temp88=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1)->f1; goto _LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp1 >
2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL95: _temp94=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f1; goto _LL93; _LL93: _temp92=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f2; goto _LL91; _LL91: _temp90=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1)->f3; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp1 >
2u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL101: _temp100=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f1; goto _LL99; _LL99: _temp98=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f2; goto _LL97; _LL97: _temp96=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1)->f3; goto _LL34;} else{ goto _LL2;} _LL4: t= Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_MemKind); goto _LL2; _LL6: t= _temp37 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL2; _LL8: t= Cyc_Absyn_uchar_t; goto
_LL2; _LL10: t= Cyc_Absyn_float_t; goto _LL2; _LL12: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp102=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp102[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp103; _temp103.tag= Cyc_Absyn_PointerType_tag;
_temp103.f1=({ struct Cyc_Absyn_PtrInfo _temp104; _temp104.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp104.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp104.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp104.tq= Cyc_Absyn_empty_tqual(); _temp104.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp104;}); _temp103;});
_temp102;}); goto _LL2; _LL14: t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);(
void*)( _temp43->type=( void*) t); _temp43->tq= Cyc_Absyn_empty_tqual(); _temp43->shadow=
0; v_result=({ struct Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp105->hd=( void*) _temp43; _temp105->tl=
v_result; _temp105;}); goto _LL2; _LL16: t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
if( region_opt == 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp106=( char*)"* pattern would point into an unknown region"; struct
_tagged_string _temp107; _temp107.curr= _temp106; _temp107.base= _temp106;
_temp107.last_plus_one= _temp106 + 45; _temp107;})); goto _LL2;}( void*)(
_temp45->type=( void*)(( void*)({ struct Cyc_Absyn_PointerType_struct* _temp108=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp108[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp109; _temp109.tag= Cyc_Absyn_PointerType_tag;
_temp109.f1=({ struct Cyc_Absyn_PtrInfo _temp110; _temp110.elt_typ=( void*) t;
_temp110.rgn_typ=( void*)* region_opt; _temp110.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp110.tq= Cyc_Absyn_empty_tqual();
_temp110.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp110;}); _temp109;}); _temp108;}))); _temp45->tq= Cyc_Absyn_empty_tqual();
_temp45->shadow= 0; v_result=({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp111->hd=( void*) _temp45;
_temp111->tl= v_result; _temp111;}); goto _LL2; _LL18: { void* ptr_rgn= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); struct _tuple5 _temp114; struct Cyc_List_List*
_temp115; struct Cyc_List_List* _temp117; struct _tuple5* _temp112= Cyc_Tcpat_tcPat(
te, _temp47,( void**)& ptr_rgn); _temp114=* _temp112; _LL118: _temp117= _temp114.f1;
goto _LL116; _LL116: _temp115= _temp114.f2; goto _LL113; _LL113: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp117); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp115);{ void*
_temp119= Cyc_Tcutil_compress(( void*)( _temp47->topt)->v); struct Cyc_Absyn_TunionFieldInfo
_temp127; struct Cyc_Absyn_Tunionfield* _temp129; struct Cyc_Absyn_Tuniondecl*
_temp131; struct _tuple1* _temp133; struct Cyc_List_List* _temp135; struct
_tuple1* _temp137; struct Cyc_Absyn_XTunionFieldInfo _temp139; struct Cyc_Absyn_Tunionfield*
_temp141; struct Cyc_Absyn_XTuniondecl* _temp143; struct _tuple1* _temp145;
struct _tuple1* _temp147; _LL121: if(( unsigned int) _temp119 > 4u?(( struct
_tunion_struct*) _temp119)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL128:
_temp127=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp119)->f1; _LL138: _temp137=( struct _tuple1*) _temp127.name; goto _LL136;
_LL136: _temp135=( struct Cyc_List_List*) _temp127.targs; goto _LL134; _LL134:
_temp133=( struct _tuple1*) _temp127.fname; goto _LL132; _LL132: _temp131=(
struct Cyc_Absyn_Tuniondecl*) _temp127.tud; goto _LL130; _LL130: _temp129=(
struct Cyc_Absyn_Tunionfield*) _temp127.tufd; goto _LL122;} else{ goto _LL123;}
_LL123: if(( unsigned int) _temp119 > 4u?(( struct _tunion_struct*) _temp119)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL140: _temp139=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp119)->f1; _LL148: _temp147=(
struct _tuple1*) _temp139.name; goto _LL146; _LL146: _temp145=( struct _tuple1*)
_temp139.fname; goto _LL144; _LL144: _temp143=( struct Cyc_Absyn_XTuniondecl*)
_temp139.xtud; goto _LL142; _LL142: _temp141=( struct Cyc_Absyn_Tunionfield*)
_temp139.xtufd; goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL122: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp149=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp149[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp150; _temp150.tag= Cyc_Absyn_TunionType_tag;
_temp150.f1=({ struct Cyc_Absyn_TunionInfo _temp151; _temp151.name= _temp137;
_temp151.targs= _temp135; _temp151.rgn=( void*) ptr_rgn; _temp151.tud= _temp131;
_temp151;}); _temp150;}); _temp149;}); goto _LL120; _LL124: t=( void*)({ struct
Cyc_Absyn_XTunionType_struct* _temp152=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp152[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp153; _temp153.tag= Cyc_Absyn_XTunionType_tag;
_temp153.f1=({ struct Cyc_Absyn_XTunionInfo _temp154; _temp154.name= _temp147;
_temp154.rgn=( void*) ptr_rgn; _temp154.xtud= _temp143; _temp154;}); _temp153;});
_temp152;}); goto _LL120; _LL126: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp155=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp155[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp156; _temp156.tag= Cyc_Absyn_PointerType_tag;
_temp156.f1=({ struct Cyc_Absyn_PtrInfo _temp157; _temp157.elt_typ=( void*)((
void*)( _temp47->topt)->v); _temp157.rgn_typ=( void*) ptr_rgn; _temp157.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp157.tq= Cyc_Absyn_empty_tqual();
_temp157.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp157;}); _temp156;}); _temp155;}); goto _LL120; _LL120:;} goto _LL2;} _LL20: {
struct Cyc_List_List* ts= 0; for( 0; _temp49 != 0; _temp49= _temp49->tl){ struct
_tuple5 _temp160; struct Cyc_List_List* _temp161; struct Cyc_List_List* _temp163;
struct _tuple5* _temp158= Cyc_Tcpat_tcPat( te,( struct Cyc_Absyn_Pat*) _temp49->hd,
region_opt); _temp160=* _temp158; _LL164: _temp163= _temp160.f1; goto _LL162;
_LL162: _temp161= _temp160.f2; goto _LL159; _LL159: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp163); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp161); ts=({ struct Cyc_List_List*
_temp165=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp165->hd=( void*)({ struct _tuple7* _temp166=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp166->f1= Cyc_Absyn_empty_tqual(); _temp166->f2=(
void*)((( struct Cyc_Absyn_Pat*) _temp49->hd)->topt)->v; _temp166;}); _temp165->tl=
ts; _temp165;});} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp167=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp167[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp168; _temp168.tag= Cyc_Absyn_TupleType_tag;
_temp168.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ts); _temp168;}); _temp167;}); goto _LL2;} _LL22: if( _temp53 != 0){ return((
struct _tuple5*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp169=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp170; _temp170.curr= _temp169; _temp170.base= _temp169;
_temp170.last_plus_one= _temp169 + 35; _temp170;}));}{ struct Cyc_List_List*
dps0= _temp51; for( 0; dps0 != 0; dps0= dps0->tl){ struct _tuple5 _temp173;
struct Cyc_List_List* _temp174; struct Cyc_List_List* _temp176; struct _tuple5*
_temp171= Cyc_Tcpat_tcPat( te,(*(( struct _tuple6*) dps0->hd)).f2, region_opt);
_temp173=* _temp171; _LL177: _temp176= _temp173.f1; goto _LL175; _LL175:
_temp174= _temp173.f2; goto _LL172; _LL172: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp176); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp174);}}{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp58->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst);* _temp57=({ struct Cyc_Core_Opt* _temp178=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp178->v=( void*) all_typs;
_temp178;}); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp179=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp179[ 0]=({ struct Cyc_Absyn_StructType_struct _temp180; _temp180.tag= Cyc_Absyn_StructType_tag;
_temp180.f1=( struct _tuple1*)(( struct _tuple1*)( _temp58->name)->v); _temp180.f2=
all_typs; _temp180.f3=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp181=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp181[ 0]= _temp58;
_temp181;})); _temp180;}); _temp179;});{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)( p->loc,
_temp51, _temp58); for( 0; fields != 0; fields= fields->tl){ struct _tuple8
_temp184; struct Cyc_Absyn_Pat* _temp185; struct Cyc_Absyn_Structfield* _temp187;
struct _tuple8* _temp182=( struct _tuple8*) fields->hd; _temp184=* _temp182;
_LL188: _temp187= _temp184.f1; goto _LL186; _LL186: _temp185= _temp184.f2; goto
_LL183; _LL183: { void* inst_fieldtyp= Cyc_Tcutil_substitute( all_inst,( void*)
_temp187->type); if( ! Cyc_Tcutil_unify(( void*)( _temp185->topt)->v,
inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp189=*
_temp187->name; struct _tagged_string _temp190= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp191= Cyc_Absynpp_typ2string(( void*)(
_temp185->topt)->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr, _temp190.last_plus_one -
_temp190.curr, _temp190.curr, _temp191.last_plus_one - _temp191.curr, _temp191.curr);}));}}}
goto _LL2;}} _LL24: { struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc,
te, _temp66); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp66);{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp62->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* tqts= _temp60->typs; if( tqts ==
0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp192=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp192[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp193; _temp193.tag= Cyc_Absyn_TunionType_tag;
_temp193.f1=({ struct Cyc_Absyn_TunionInfo _temp194; _temp194.name=( struct
_tuple1*)( _temp62->name)->v; _temp194.targs= all_typs; _temp194.rgn=( void*)
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp194.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp62; _temp194;}); _temp193;}); _temp192;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp195=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp195[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp196; _temp196.tag= Cyc_Absyn_TunionFieldType_tag; _temp196.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp197; _temp197.name=( struct _tuple1*)( _temp62->name)->v; _temp197.targs=
all_typs; _temp197.fname= _temp60->name; _temp197.tud=( struct Cyc_Absyn_Tuniondecl*)
_temp62; _temp197.tufd=( struct Cyc_Absyn_Tunionfield*) _temp60; _temp197;});
_temp196;}); _temp195;});} for( 0; _temp64 != 0? tqts != 0: 0; _temp64= _temp64->tl,
tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) _temp64->hd;
struct _tuple5 _temp200; struct Cyc_List_List* _temp201; struct Cyc_List_List*
_temp203; struct _tuple5* _temp198= Cyc_Tcpat_tcPat( te2, p2, region_opt);
_temp200=* _temp198; _LL204: _temp203= _temp200.f1; goto _LL202; _LL202:
_temp201= _temp200.f2; goto _LL199; _LL199: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp203); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp201);{ void* field_typ= Cyc_Tcutil_substitute(
all_inst,(*(( struct _tuple7*) tqts->hd)).f2); if( ! Cyc_Tcutil_unify(( void*)(
p2->topt)->v, field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string
_temp205= Cyc_Absynpp_qvar2string( _temp70); struct _tagged_string _temp206= Cyc_Absynpp_typ2string(
field_typ); struct _tagged_string _temp207= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp205.last_plus_one -
_temp205.curr, _temp205.curr, _temp206.last_plus_one - _temp206.curr, _temp206.curr,
_temp207.last_plus_one - _temp207.curr, _temp207.curr);}));}}} if( _temp64 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp208= Cyc_Absynpp_qvar2string(
_temp70); xprintf("too many arguments for tunion constructor %.*s", _temp208.last_plus_one
- _temp208.curr, _temp208.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp209= Cyc_Absynpp_qvar2string( _temp70); xprintf("too few arguments for tunion constructor %.*s",
_temp209.last_plus_one - _temp209.curr, _temp209.curr);}));} goto _LL2;}} _LL26: {
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp78);
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp78);{ struct Cyc_List_List* tqts= _temp72->typs;
t=( void*)({ struct Cyc_Absyn_XTunionFieldType_struct* _temp210=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp210[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp211; _temp211.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp211.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp212; _temp212.name=
_temp74->name; _temp212.fname= _temp72->name; _temp212.xtud=( struct Cyc_Absyn_XTuniondecl*)
_temp74; _temp212.xtufd=( struct Cyc_Absyn_Tunionfield*) _temp72; _temp212;});
_temp211;}); _temp210;}); for( 0; _temp76 != 0? tqts != 0: 0; _temp76= _temp76->tl,
tqts= tqts->tl){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*) _temp76->hd;
struct _tuple5 _temp215; struct Cyc_List_List* _temp216; struct Cyc_List_List*
_temp218; struct _tuple5* _temp213= Cyc_Tcpat_tcPat( te2, p2, region_opt);
_temp215=* _temp213; _LL219: _temp218= _temp215.f1; goto _LL217; _LL217:
_temp216= _temp215.f2; goto _LL214; _LL214: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp218); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp216);{ void* field_typ=(*((
struct _tuple7*) tqts->hd)).f2; if( ! Cyc_Tcutil_unify(( void*)( p2->topt)->v,
field_typ)){ Cyc_Tcutil_terr( p2->loc,({ struct _tagged_string _temp220= Cyc_Absynpp_qvar2string(
_temp80); struct _tagged_string _temp221= Cyc_Absynpp_typ2string( field_typ);
struct _tagged_string _temp222= Cyc_Absynpp_typ2string(( void*)( p2->topt)->v);
xprintf("%.*s expects argument type %.*s, not %.*s", _temp220.last_plus_one -
_temp220.curr, _temp220.curr, _temp221.last_plus_one - _temp221.curr, _temp221.curr,
_temp222.last_plus_one - _temp222.curr, _temp222.curr);}));}}} if( _temp76 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp223= Cyc_Absynpp_qvar2string(
_temp80); xprintf("too many arguments for xtunion constructor %.*s", _temp223.last_plus_one
- _temp223.curr, _temp223.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp224= Cyc_Absynpp_qvar2string( _temp80); xprintf("too few arguments for xtunion constructor %.*s",
_temp224.last_plus_one - _temp224.curr, _temp224.curr);}));} goto _LL2;}} _LL28:
t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp225=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp225[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp226; _temp226.tag= Cyc_Absyn_EnumType_tag;
_temp226.f1= _temp84->name; _temp226.f2=( struct Cyc_Absyn_Enumdecl*) _temp84;
_temp226;}); _temp225;}); goto _LL2; _LL30:{ struct _handler_cons _temp227;
_push_handler(& _temp227);{ struct _xtunion_struct* _temp228=( struct
_xtunion_struct*) setjmp( _temp227.handler); if( ! _temp228){{ void* _temp229=
Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp88); struct Cyc_Absyn_Structdecl*
_temp241; struct Cyc_Absyn_Tunionfield* _temp243; struct Cyc_Absyn_Tuniondecl*
_temp245; struct Cyc_Absyn_Tunionfield* _temp247; struct Cyc_Absyn_XTuniondecl*
_temp249; struct Cyc_Absyn_Enumfield* _temp251; struct Cyc_Absyn_Enumdecl*
_temp253; void* _temp255; _LL231: if((( struct _tunion_struct*) _temp229)->tag
== Cyc_Tcenv_StructRes_tag){ _LL242: _temp241=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp229)->f1; goto _LL232;} else{ goto
_LL233;} _LL233: if((( struct _tunion_struct*) _temp229)->tag == Cyc_Tcenv_TunionRes_tag){
_LL246: _temp245=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp229)->f1; goto _LL244; _LL244: _temp243=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp229)->f2; goto _LL234;} else{ goto
_LL235;} _LL235: if((( struct _tunion_struct*) _temp229)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL250: _temp249=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp229)->f1; goto _LL248; _LL248: _temp247=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp229)->f2; goto _LL236;} else{ goto
_LL237;} _LL237: if((( struct _tunion_struct*) _temp229)->tag == Cyc_Tcenv_EnumRes_tag){
_LL254: _temp253=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp229)->f1; goto _LL252; _LL252: _temp251=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp229)->f2; goto _LL238;} else{ goto _LL239;}
_LL239: if((( struct _tunion_struct*) _temp229)->tag == Cyc_Tcenv_VarRes_tag){
_LL256: _temp255=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp229)->f1; goto
_LL240;} else{ goto _LL230;} _LL232: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp257=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 45; _temp258;})); t= Cyc_Absyn_wildtyp();
goto _LL230; _LL234:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp259=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp259[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp260; _temp260.tag= Cyc_Absyn_Tunion_p_tag;
_temp260.f1= _temp243->name; _temp260.f2= 0; _temp260.f3= 0; _temp260.f4= 0;
_temp260.f5= _temp245; _temp260.f6= _temp243; _temp260;}); _temp259;})));{
struct _tuple5* _temp261= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp261;} _LL236:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp262=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp262[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp263; _temp263.tag= Cyc_Absyn_XTunion_p_tag;
_temp263.f1= _temp247->name; _temp263.f2= 0; _temp263.f3= 0; _temp263.f4=
_temp249; _temp263.f5= _temp247; _temp263;}); _temp262;})));{ struct _tuple5*
_temp264= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u); return
_temp264;} _LL238:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp265=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp265[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp266; _temp266.tag= Cyc_Absyn_Enum_p_tag;
_temp266.f1= _temp251->name; _temp266.f2= _temp253; _temp266.f3= _temp251;
_temp266;}); _temp265;})));{ struct _tuple5* _temp267= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp267;} _LL240:{ void* _temp268=(*
_temp88).f1; struct Cyc_List_List* _temp276; _LL270: if(( int) _temp268 == Cyc_Absyn_Loc_n){
goto _LL271;} else{ goto _LL272;} _LL272: if(( unsigned int) _temp268 > 1u?((
struct _tunion_struct*) _temp268)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL277:
_temp276=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp268)->f1;
if( _temp276 == 0){ goto _LL273;} else{ goto _LL274;}} else{ goto _LL274;}
_LL274: goto _LL275; _LL271: goto _LL273; _LL273:(* _temp88).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp278=(
struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp278[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp279; _temp279.tag= Cyc_Absyn_Var_p_tag;
_temp279.f1= Cyc_Absyn_new_vardecl( _temp88,( void*) Cyc_Absyn_VoidType, 0);
_temp279;}); _temp278;})));{ struct _tuple5* _temp280= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp280;} _LL275: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp281=( char*)"qualified variable in pattern";
struct _tagged_string _temp282; _temp282.curr= _temp281; _temp282.base= _temp281;
_temp282.last_plus_one= _temp281 + 30; _temp282;})); goto _LL269; _LL269:;} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp283= Cyc_Absynpp_qvar2string( _temp88);
xprintf("unresolved identifier %.*s in pattern", _temp283.last_plus_one -
_temp283.curr, _temp283.curr);})); t= Cyc_Absyn_wildtyp(); goto _LL230; _LL230:;};
_pop_handler();} else{ struct _xtunion_struct* _temp285= _temp228; _LL287: if(
_temp285->tag == Cyc_Dict_Absent_tag){ goto _LL288;} else{ goto _LL289;} _LL289:
goto _LL290; _LL288:{ void* _temp291=(* _temp88).f1; struct Cyc_List_List*
_temp299; _LL293: if(( int) _temp291 == Cyc_Absyn_Loc_n){ goto _LL294;} else{
goto _LL295;} _LL295: if(( unsigned int) _temp291 > 1u?(( struct _tunion_struct*)
_temp291)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL300: _temp299=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp291)->f1; if( _temp299 == 0){ goto _LL296;}
else{ goto _LL297;}} else{ goto _LL297;} _LL297: goto _LL298; _LL294: goto
_LL296; _LL296:(* _temp88).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp301=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp301[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp302; _temp302.tag= Cyc_Absyn_Var_p_tag; _temp302.f1= Cyc_Absyn_new_vardecl(
_temp88,( void*) Cyc_Absyn_VoidType, 0); _temp302;}); _temp301;}))); return Cyc_Tcpat_tcPat(
te, p, region_opt); _LL298: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp303= Cyc_Absynpp_qvar2string( _temp88); xprintf("unbound identifier %.*s in pattern",
_temp303.last_plus_one - _temp303.curr, _temp303.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL292; _LL292:;} goto _LL286; _LL290:( void) _throw( _temp285); _LL286:;}}}
goto _LL2; _LL32:{ struct _handler_cons _temp304; _push_handler(& _temp304);{
struct _xtunion_struct* _temp305=( struct _xtunion_struct*) setjmp( _temp304.handler);
if( ! _temp305){{ void* _temp306= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp94);
struct Cyc_Absyn_Structdecl* _temp318; struct Cyc_Absyn_Tunionfield* _temp320;
struct Cyc_Absyn_Tuniondecl* _temp322; struct Cyc_Absyn_Tunionfield* _temp324;
struct Cyc_Absyn_XTuniondecl* _temp326; struct Cyc_Absyn_Enumfield* _temp328;
struct Cyc_Absyn_Enumdecl* _temp330; void* _temp332; _LL308: if((( struct
_tunion_struct*) _temp306)->tag == Cyc_Tcenv_StructRes_tag){ _LL319: _temp318=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp306)->f1;
goto _LL309;} else{ goto _LL310;} _LL310: if((( struct _tunion_struct*) _temp306)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL323: _temp322=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp306)->f1; goto _LL321; _LL321: _temp320=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp306)->f2;
goto _LL311;} else{ goto _LL312;} _LL312: if((( struct _tunion_struct*) _temp306)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL327: _temp326=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp306)->f1; goto _LL325; _LL325:
_temp324=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp306)->f2; goto _LL313;} else{ goto _LL314;} _LL314: if((( struct
_tunion_struct*) _temp306)->tag == Cyc_Tcenv_EnumRes_tag){ _LL331: _temp330=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp306)->f1;
goto _LL329; _LL329: _temp328=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp306)->f2; goto _LL315;} else{ goto _LL316;} _LL316: if((( struct
_tunion_struct*) _temp306)->tag == Cyc_Tcenv_VarRes_tag){ _LL333: _temp332=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp306)->f1; goto _LL317;} else{
goto _LL307;} _LL309: { struct Cyc_List_List* x=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcpat_null_designator, _temp90);( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp334=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp334[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp335; _temp335.tag= Cyc_Absyn_Struct_p_tag;
_temp335.f1= _temp318; _temp335.f2= 0; _temp335.f3= _temp92; _temp335.f4= x;
_temp335;}); _temp334;})));{ struct _tuple5* _temp336= Cyc_Tcpat_tcPat( te, p,
region_opt); _npop_handler( 0u); return _temp336;}} _LL311:( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp337=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp337[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp338; _temp338.tag= Cyc_Absyn_Tunion_p_tag;
_temp338.f1= _temp320->name; _temp338.f2= 0; _temp338.f3= _temp92; _temp338.f4=
_temp90; _temp338.f5= _temp322; _temp338.f6= _temp320; _temp338;}); _temp337;})));{
struct _tuple5* _temp339= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp339;} _LL313:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_XTunion_p_struct*
_temp340=( struct Cyc_Absyn_XTunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_p_struct));
_temp340[ 0]=({ struct Cyc_Absyn_XTunion_p_struct _temp341; _temp341.tag= Cyc_Absyn_XTunion_p_tag;
_temp341.f1= _temp324->name; _temp341.f2= _temp92; _temp341.f3= _temp90;
_temp341.f4= _temp326; _temp341.f5= _temp324; _temp341;}); _temp340;})));{
struct _tuple5* _temp342= Cyc_Tcpat_tcPat( te, p, region_opt); _npop_handler( 0u);
return _temp342;} _LL315: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp343= Cyc_Absynpp_qvar2string( _temp94); xprintf("bad enum constructor %.*s in pattern",
_temp343.last_plus_one - _temp343.curr, _temp343.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL307; _LL317: goto _LL307; _LL307:;}; _pop_handler();} else{ struct
_xtunion_struct* _temp345= _temp305; _LL347: if( _temp345->tag == Cyc_Dict_Absent_tag){
goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL348: goto _LL346;
_LL350:( void) _throw( _temp345); _LL346:;}}} Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp351= Cyc_Absynpp_qvar2string( _temp94); xprintf("%.*s is not a constructor in pattern",
_temp351.last_plus_one - _temp351.curr, _temp351.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL2; _LL34:{ struct _handler_cons _temp352; _push_handler(& _temp352);{
struct _xtunion_struct* _temp353=( struct _xtunion_struct*) setjmp( _temp352.handler);
if( ! _temp353){{ void* _temp354= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp100); struct Cyc_Absyn_Structdecl* _temp366; struct Cyc_Absyn_Tunionfield*
_temp368; struct Cyc_Absyn_Tuniondecl* _temp370; struct Cyc_Absyn_Tunionfield*
_temp372; struct Cyc_Absyn_XTuniondecl* _temp374; void* _temp376; struct Cyc_Absyn_Enumfield*
_temp378; struct Cyc_Absyn_Enumdecl* _temp380; _LL356: if((( struct
_tunion_struct*) _temp354)->tag == Cyc_Tcenv_StructRes_tag){ _LL367: _temp366=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp354)->f1;
goto _LL357;} else{ goto _LL358;} _LL358: if((( struct _tunion_struct*) _temp354)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL371: _temp370=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp354)->f1; goto _LL369; _LL369: _temp368=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp354)->f2;
goto _LL359;} else{ goto _LL360;} _LL360: if((( struct _tunion_struct*) _temp354)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL375: _temp374=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp354)->f1; goto _LL373; _LL373:
_temp372=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp354)->f2; goto _LL361;} else{ goto _LL362;} _LL362: if((( struct
_tunion_struct*) _temp354)->tag == Cyc_Tcenv_VarRes_tag){ _LL377: _temp376=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp354)->f1; goto _LL363;} else{
goto _LL364;} _LL364: if((( struct _tunion_struct*) _temp354)->tag == Cyc_Tcenv_EnumRes_tag){
_LL381: _temp380=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp354)->f1; goto _LL379; _LL379: _temp378=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp354)->f2; goto _LL365;} else{ goto _LL355;}
_LL357:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp382=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp382[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp383; _temp383.tag= Cyc_Absyn_Struct_p_tag;
_temp383.f1= _temp366; _temp383.f2= 0; _temp383.f3= _temp98; _temp383.f4=
_temp96; _temp383;}); _temp382;})));{ struct _tuple5* _temp384= Cyc_Tcpat_tcPat(
te, p, region_opt); _npop_handler( 0u); return _temp384;} _LL359: Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ char* _temp385=( char*)"tunion with designators not implemented";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 40; _temp386;})); t= Cyc_Absyn_wildtyp();
goto _LL355; _LL361: Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp387=( char*)"xtunion with designators not implemented"; struct
_tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 41; _temp388;})); t= Cyc_Absyn_wildtyp();
goto _LL355; _LL363: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp389=
Cyc_Absynpp_qvar2string( _temp100); xprintf("unbound struct %.*s in pattern",
_temp389.last_plus_one - _temp389.curr, _temp389.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL355; _LL365: Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp390=
Cyc_Absynpp_qvar2string( _temp100); xprintf("enum field %.*s used in struct pattern",
_temp390.last_plus_one - _temp390.curr, _temp390.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL355; _LL355:;}; _pop_handler();} else{ struct _xtunion_struct* _temp392=
_temp353; _LL394: if( _temp392->tag == Cyc_Dict_Absent_tag){ goto _LL395;} else{
goto _LL396;} _LL396: goto _LL397; _LL395: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp398= Cyc_Absynpp_qvar2string( _temp100); xprintf("unbound struct %.*s in pattern",
_temp398.last_plus_one - _temp398.curr, _temp398.curr);})); t= Cyc_Absyn_wildtyp();
goto _LL393; _LL397:( void) _throw( _temp392); _LL393:;}}} goto _LL2; _LL2:;} p->topt=({
struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp399->v=( void*) t; _temp399;}); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_name, v_result), p->loc,(
struct _tagged_string)({ char* _temp400=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 37; _temp401;})); return({ struct _tuple5*
_temp402=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp402->f1=
tv_result; _temp402->f2= v_result; _temp402;});} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp403=( void*) p->r;
struct Cyc_Absyn_Pat* _temp417; struct Cyc_Absyn_Tunionfield* _temp419; struct
Cyc_Absyn_Tuniondecl* _temp421; struct Cyc_List_List* _temp423; struct Cyc_List_List*
_temp425; struct Cyc_Core_Opt* _temp427; struct _tuple1* _temp429; struct Cyc_Absyn_Tunionfield*
_temp431; struct Cyc_Absyn_XTuniondecl* _temp433; struct Cyc_List_List* _temp435;
struct Cyc_List_List* _temp437; struct _tuple1* _temp439; struct Cyc_List_List*
_temp441; struct Cyc_List_List* _temp443; struct Cyc_Core_Opt* _temp445; struct
Cyc_Absyn_Structdecl* _temp447; struct Cyc_List_List* _temp449; _LL405: if((
unsigned int) _temp403 > 2u?(( struct _tunion_struct*) _temp403)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL418: _temp417=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp403)->f1; goto _LL406;} else{ goto _LL407;} _LL407: if(( unsigned int)
_temp403 > 2u?(( struct _tunion_struct*) _temp403)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL430: _temp429=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp403)->f1; goto _LL428; _LL428: _temp427=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp403)->f2; goto _LL426; _LL426: _temp425=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp403)->f3; goto
_LL424; _LL424: _temp423=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp403)->f4; goto _LL422; _LL422: _temp421=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp403)->f5; goto _LL420; _LL420: _temp419=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp403)->f6;
goto _LL408;} else{ goto _LL409;} _LL409: if(( unsigned int) _temp403 > 2u?((
struct _tunion_struct*) _temp403)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL440:
_temp439=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp403)->f1;
goto _LL438; _LL438: _temp437=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp403)->f2; goto _LL436; _LL436: _temp435=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp403)->f3; goto _LL434; _LL434: _temp433=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp403)->f4;
goto _LL432; _LL432: _temp431=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp403)->f5; goto _LL410;} else{ goto _LL411;} _LL411: if(( unsigned int)
_temp403 > 2u?(( struct _tunion_struct*) _temp403)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL448: _temp447=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp403)->f1; goto _LL446; _LL446: _temp445=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp403)->f2; goto _LL444; _LL444: _temp443=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp403)->f3; goto
_LL442; _LL442: _temp441=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp403)->f4; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp403 > 2u?(( struct _tunion_struct*) _temp403)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL450: _temp449=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp403)->f1; goto _LL414;} else{ goto _LL415;} _LL415: goto _LL416; _LL406:
Cyc_Tcpat_check_pat_regions( te, _temp417);{ void* _temp451=( void*)( p->topt)->v;
struct Cyc_Absyn_PtrInfo _temp461; struct Cyc_Absyn_Conref* _temp463; struct Cyc_Absyn_Tqual*
_temp465; struct Cyc_Absyn_Conref* _temp467; void* _temp469; void* _temp471;
struct Cyc_Absyn_TunionInfo _temp473; struct Cyc_Absyn_Tuniondecl* _temp475;
void* _temp477; struct Cyc_List_List* _temp479; struct _tuple1* _temp481; struct
Cyc_Absyn_XTunionInfo _temp483; struct Cyc_Absyn_XTuniondecl* _temp485; void*
_temp487; struct _tuple1* _temp489; _LL453: if(( unsigned int) _temp451 > 4u?((
struct _tunion_struct*) _temp451)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL462:
_temp461=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp451)->f1; _LL472: _temp471=( void*) _temp461.elt_typ; goto _LL470; _LL470:
_temp469=( void*) _temp461.rgn_typ; goto _LL468; _LL468: _temp467=( struct Cyc_Absyn_Conref*)
_temp461.nullable; goto _LL466; _LL466: _temp465=( struct Cyc_Absyn_Tqual*)
_temp461.tq; goto _LL464; _LL464: _temp463=( struct Cyc_Absyn_Conref*) _temp461.bounds;
goto _LL454;} else{ goto _LL455;} _LL455: if(( unsigned int) _temp451 > 4u?((
struct _tunion_struct*) _temp451)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL474:
_temp473=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp451)->f1; _LL482: _temp481=( struct _tuple1*) _temp473.name; goto _LL480;
_LL480: _temp479=( struct Cyc_List_List*) _temp473.targs; goto _LL478; _LL478:
_temp477=( void*) _temp473.rgn; goto _LL476; _LL476: _temp475=( struct Cyc_Absyn_Tuniondecl*)
_temp473.tud; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp451 > 4u?(( struct _tunion_struct*) _temp451)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL484: _temp483=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp451)->f1; _LL490: _temp489=( struct _tuple1*) _temp483.name; goto _LL488;
_LL488: _temp487=( void*) _temp483.rgn; goto _LL486; _LL486: _temp485=( struct
Cyc_Absyn_XTuniondecl*) _temp483.xtud; goto _LL458;} else{ goto _LL459;} _LL459:
goto _LL460; _LL454: _temp477= _temp469; goto _LL456; _LL456: _temp487= _temp477;
goto _LL458; _LL458: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp487);
return; _LL460:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp491=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp491[ 0]=({ struct Cyc_Core_Impossible_struct _temp492; _temp492.tag= Cyc_Core_Impossible_tag;
_temp492.f1=( struct _tagged_string)({ char* _temp493=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 36; _temp494;}); _temp492;}); _temp491;}));
_LL452:;} _LL408: for( 0; _temp423 != 0; _temp423= _temp423->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*) _temp423->hd);}{ void* _temp495=( void*)( p->topt)->v;
struct Cyc_Absyn_TunionInfo _temp503; struct Cyc_Absyn_Tuniondecl* _temp505;
void* _temp507; struct Cyc_List_List* _temp509; struct _tuple1* _temp511; struct
Cyc_Absyn_TunionFieldInfo _temp513; _LL497: if(( unsigned int) _temp495 > 4u?((
struct _tunion_struct*) _temp495)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL504:
_temp503=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp495)->f1; _LL512: _temp511=( struct _tuple1*) _temp503.name; goto _LL510;
_LL510: _temp509=( struct Cyc_List_List*) _temp503.targs; goto _LL508; _LL508:
_temp507=( void*) _temp503.rgn; goto _LL506; _LL506: _temp505=( struct Cyc_Absyn_Tuniondecl*)
_temp503.tud; goto _LL498;} else{ goto _LL499;} _LL499: if(( unsigned int)
_temp495 > 4u?(( struct _tunion_struct*) _temp495)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL514: _temp513=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp495)->f1; goto _LL500;} else{ goto _LL501;} _LL501: goto _LL502; _LL498:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp507); return; _LL500: return;
_LL502:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp515=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp515[ 0]=({ struct Cyc_Core_Impossible_struct _temp516; _temp516.tag= Cyc_Core_Impossible_tag;
_temp516.f1=( struct _tagged_string)({ char* _temp517=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 35; _temp518;}); _temp516;}); _temp515;}));
_LL496:;} _LL410: for( 0; _temp435 != 0; _temp435= _temp435->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*) _temp435->hd);}{ void* _temp519=( void*)( p->topt)->v;
struct Cyc_Absyn_XTunionInfo _temp527; struct Cyc_Absyn_XTuniondecl* _temp529;
void* _temp531; struct _tuple1* _temp533; struct Cyc_Absyn_XTunionFieldInfo
_temp535; _LL521: if(( unsigned int) _temp519 > 4u?(( struct _tunion_struct*)
_temp519)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL528: _temp527=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp519)->f1; _LL534: _temp533=( struct
_tuple1*) _temp527.name; goto _LL532; _LL532: _temp531=( void*) _temp527.rgn;
goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_XTuniondecl*) _temp527.xtud;
goto _LL522;} else{ goto _LL523;} _LL523: if(( unsigned int) _temp519 > 4u?((
struct _tunion_struct*) _temp519)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){
_LL536: _temp535=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp519)->f1; goto _LL524;} else{ goto _LL525;} _LL525: goto _LL526; _LL522:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp531); return; _LL524: return;
_LL526:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp537=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp537[ 0]=({ struct Cyc_Core_Impossible_struct _temp538; _temp538.tag= Cyc_Core_Impossible_tag;
_temp538.f1=( struct _tagged_string)({ char* _temp539=( char*)"check_pat_regions: bad xtunion type";
struct _tagged_string _temp540; _temp540.curr= _temp539; _temp540.base= _temp539;
_temp540.last_plus_one= _temp539 + 36; _temp540;}); _temp538;}); _temp537;}));
_LL520:;} _LL412: _temp449=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp441); goto _LL414; _LL414: for( 0; _temp449
!= 0; _temp449= _temp449->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)
_temp449->hd);} return; _LL416: return; _LL404:;} typedef void* Cyc_Tcpat_Name_value;
static const int Cyc_Tcpat_Name_v_tag= 0; struct Cyc_Tcpat_Name_v_struct{ int
tag; struct _tagged_string f1; } ; static const int Cyc_Tcpat_Int_v_tag= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ;
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; typedef void* Cyc_Tcpat_Simple_pat;
static const unsigned int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con_tag=
0; struct Cyc_Tcpat_Con_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; typedef void* Cyc_Tcpat_simple_pat_t; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp541=( void*)
c1->name; struct _tagged_string _temp547; int _temp549; _LL543: if((( struct
_tunion_struct*) _temp541)->tag == Cyc_Tcpat_Name_v_tag){ _LL548: _temp547=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp541)->f1; goto
_LL544;} else{ goto _LL545;} _LL545: if((( struct _tunion_struct*) _temp541)->tag
== Cyc_Tcpat_Int_v_tag){ _LL550: _temp549=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp541)->f1; goto _LL546;} else{ goto _LL542;} _LL544: { void* _temp551=( void*)
c2->name; struct _tagged_string _temp557; int _temp559; _LL553: if((( struct
_tunion_struct*) _temp551)->tag == Cyc_Tcpat_Name_v_tag){ _LL558: _temp557=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp551)->f1; goto
_LL554;} else{ goto _LL555;} _LL555: if((( struct _tunion_struct*) _temp551)->tag
== Cyc_Tcpat_Int_v_tag){ _LL560: _temp559=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp551)->f1; goto _LL556;} else{ goto _LL552;} _LL554: return Cyc_String_zstrcmp(
_temp547, _temp557); _LL556: return - 1; _LL552:;} _LL546: { void* _temp561=(
void*) c2->name; struct _tagged_string _temp567; int _temp569; _LL563: if(((
struct _tunion_struct*) _temp561)->tag == Cyc_Tcpat_Name_v_tag){ _LL568:
_temp567=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp561)->f1;
goto _LL564;} else{ goto _LL565;} _LL565: if((( struct _tunion_struct*) _temp561)->tag
== Cyc_Tcpat_Int_v_tag){ _LL570: _temp569=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp561)->f1; goto _LL566;} else{ goto _LL562;} _LL564: return 1; _LL566:
return _temp549 - _temp569; _LL562:;} _LL542:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){
return(( struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct
Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt
Cyc_Tcpat_one_opt=( struct Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt=( struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char
_temp574[ 5u]="null"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={
0u,( struct _tagged_string){ _temp574, _temp574, _temp574 + 5u}}; static char
_temp578[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,( struct _tagged_string){ _temp578, _temp578, _temp578 + 4u}}; static char
_temp582[ 6u]="tuple"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0u,( struct _tagged_string){ _temp582, _temp582, _temp582 + 6u}}; static struct
Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)((
void*)& Cyc_Tcpat_null_name_value),.arity= 0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=(
void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity= 1,.span=( struct Cyc_Core_Opt*)&
Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0=( struct
Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity= 1,.span=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=&
Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp583=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp583->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp584=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp584[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp585; _temp585.tag= Cyc_Tcpat_Int_v_tag;
_temp585.f1= i; _temp585;}); _temp584;})); _temp583->arity= 0; _temp583->span= 0;
_temp583;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_char_con( char c){ return({
struct Cyc_Tcpat_Con_s* _temp586=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp586->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct*
_temp588=( struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp588[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp589; _temp589.tag= Cyc_Tcpat_Int_v_tag;
_temp589.f1=( int) c; _temp589;}); _temp588;})); _temp586->arity= 0; _temp586->span=({
struct Cyc_Core_Opt* _temp587=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp587->v=( void*) 256; _temp587;}); _temp586;});}
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct _tagged_string f){
return({ struct Cyc_Tcpat_Con_s* _temp590=( struct Cyc_Tcpat_Con_s*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_s)); _temp590->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp591=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp591[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp592; _temp592.tag= Cyc_Tcpat_Name_v_tag;
_temp592.f1= f; _temp592;}); _temp591;})); _temp590->arity= 0; _temp590->span= 0;
_temp590;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_double_con( struct
_tagged_string d){ return({ struct Cyc_Tcpat_Con_s* _temp593=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp593->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp594=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp594[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp595; _temp595.tag= Cyc_Tcpat_Name_v_tag; _temp595.f1= d; _temp595;});
_temp594;})); _temp593->arity= 0; _temp593->span= 0; _temp593;});} static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp596=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp596->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp596->arity=
i; _temp596->span= Cyc_Tcpat_one_opt_ptr; _temp596;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp597=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp597[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp598; _temp598.tag= Cyc_Tcpat_Con_tag; _temp598.f1= Cyc_Tcpat_null_con;
_temp598.f2= 0; _temp598;}); _temp597;});} static void* Cyc_Tcpat_int_pat( int i){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp599=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp599[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp600; _temp600.tag= Cyc_Tcpat_Con_tag; _temp600.f1= Cyc_Tcpat_int_con( i);
_temp600.f2= 0; _temp600;}); _temp599;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp601=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp601[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp602; _temp602.tag= Cyc_Tcpat_Con_tag; _temp602.f1= Cyc_Tcpat_char_con( c);
_temp602.f2= 0; _temp602;}); _temp601;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp603=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp603[ 0]=({ struct Cyc_Tcpat_Con_struct _temp604; _temp604.tag= Cyc_Tcpat_Con_tag;
_temp604.f1= Cyc_Tcpat_float_con( f); _temp604.f2= 0; _temp604;}); _temp603;});}
static void* Cyc_Tcpat_double_pat( struct _tagged_string d){ return( void*)({
struct Cyc_Tcpat_Con_struct* _temp605=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp605[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp606; _temp606.tag= Cyc_Tcpat_Con_tag; _temp606.f1= Cyc_Tcpat_double_con( d);
_temp606.f2= 0; _temp606;}); _temp605;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp607=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp607[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp608; _temp608.tag= Cyc_Tcpat_Con_tag; _temp608.f1= Cyc_Tcpat_null_ptr_con;
_temp608.f2=({ struct Cyc_List_List* _temp609=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp609->hd=( void*) p; _temp609->tl= 0;
_temp609;}); _temp608;}); _temp607;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp610=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp610[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp611; _temp611.tag= Cyc_Tcpat_Con_tag; _temp611.f1= Cyc_Tcpat_ptr_con;
_temp611.f2=({ struct Cyc_List_List* _temp612=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp612->hd=( void*) p; _temp612->tl= 0;
_temp612;}); _temp611;}); _temp610;});} static void* Cyc_Tcpat_tuple_pat( struct
Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp613=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp613[ 0]=({ struct Cyc_Tcpat_Con_struct _temp614; _temp614.tag= Cyc_Tcpat_Con_tag;
_temp614.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp614.f2= ss; _temp614;}); _temp613;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp615=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp615->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp616=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp616[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp617; _temp617.tag= Cyc_Tcpat_Name_v_tag; _temp617.f1= con_name; _temp617;});
_temp616;})); _temp615->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp615->span= span; _temp615;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp618=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp618[ 0]=({ struct Cyc_Tcpat_Con_struct _temp619; _temp619.tag= Cyc_Tcpat_Con_tag;
_temp619.f1= c; _temp619.f2= ps; _temp619;}); _temp618;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp620=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp654; int _temp656; void* _temp658; char _temp660; struct _tagged_string
_temp662; struct Cyc_List_List* _temp664; struct Cyc_Absyn_Vardecl* _temp666;
struct Cyc_Absyn_Pat* _temp668; struct Cyc_List_List* _temp670; struct Cyc_List_List*
_temp672; struct Cyc_Core_Opt* _temp674; struct Cyc_Absyn_Structdecl* _temp676;
struct Cyc_Absyn_Tunionfield* _temp678; struct Cyc_Absyn_Tuniondecl* _temp680;
struct Cyc_List_List* _temp682; struct Cyc_List_List* _temp684; struct Cyc_Core_Opt*
_temp686; struct _tuple1* _temp688; struct Cyc_Absyn_Tunionfield* _temp690;
struct Cyc_Absyn_XTuniondecl* _temp692; struct Cyc_List_List* _temp694; struct
Cyc_List_List* _temp696; struct _tuple1* _temp698; struct Cyc_Absyn_Enumfield*
_temp700; struct Cyc_Absyn_Enumdecl* _temp702; struct _tuple1* _temp704; struct
_tuple1* _temp706; struct Cyc_List_List* _temp708; struct Cyc_List_List*
_temp710; struct _tuple1* _temp712; struct Cyc_List_List* _temp714; struct Cyc_List_List*
_temp716; struct _tuple1* _temp718; _LL622: if(( int) _temp620 == Cyc_Absyn_Wild_p){
goto _LL623;} else{ goto _LL624;} _LL624: if(( unsigned int) _temp620 > 2u?((
struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL655:
_temp654=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp620)->f1;
goto _LL625;} else{ goto _LL626;} _LL626: if(( int) _temp620 == Cyc_Absyn_Null_p){
goto _LL627;} else{ goto _LL628;} _LL628: if(( unsigned int) _temp620 > 2u?((
struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL659:
_temp658=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp620)->f1; goto _LL657;
_LL657: _temp656=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp620)->f2; goto
_LL629;} else{ goto _LL630;} _LL630: if(( unsigned int) _temp620 > 2u?(( struct
_tunion_struct*) _temp620)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL661: _temp660=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp620)->f1; goto _LL631;} else{ goto
_LL632;} _LL632: if(( unsigned int) _temp620 > 2u?(( struct _tunion_struct*)
_temp620)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL663: _temp662=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp620)->f1; goto _LL633;}
else{ goto _LL634;} _LL634: if(( unsigned int) _temp620 > 2u?(( struct
_tunion_struct*) _temp620)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL665: _temp664=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp620)->f1; goto
_LL635;} else{ goto _LL636;} _LL636: if(( unsigned int) _temp620 > 2u?(( struct
_tunion_struct*) _temp620)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL667:
_temp666=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp620)->f1; goto _LL637;} else{ goto _LL638;} _LL638: if(( unsigned int)
_temp620 > 2u?(( struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL669: _temp668=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp620)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int)
_temp620 > 2u?(( struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL677: _temp676=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp620)->f1; goto _LL675; _LL675: _temp674=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp620)->f2; goto _LL673; _LL673: _temp672=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp620)->f3; goto
_LL671; _LL671: _temp670=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp620)->f4; goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int)
_temp620 > 2u?(( struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL689: _temp688=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp620)->f1; goto _LL687; _LL687: _temp686=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp620)->f2; goto _LL685; _LL685: _temp684=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp620)->f3; goto
_LL683; _LL683: _temp682=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp620)->f4; goto _LL681; _LL681: _temp680=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp620)->f5; goto _LL679; _LL679: _temp678=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp620)->f6;
goto _LL643;} else{ goto _LL644;} _LL644: if(( unsigned int) _temp620 > 2u?((
struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL699:
_temp698=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp620)->f1;
goto _LL697; _LL697: _temp696=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp620)->f2; goto _LL695; _LL695: _temp694=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_p_struct*) _temp620)->f3; goto _LL693; _LL693: _temp692=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp620)->f4;
goto _LL691; _LL691: _temp690=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp620)->f5; goto _LL645;} else{ goto _LL646;} _LL646: if(( unsigned int)
_temp620 > 2u?(( struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL705: _temp704=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp620)->f1; goto _LL703; _LL703: _temp702=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp620)->f2; goto _LL701; _LL701: _temp700=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp620)->f3;
goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int) _temp620 > 2u?((
struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL707:
_temp706=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp620)->f1;
goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int) _temp620 > 2u?((
struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){
_LL713: _temp712=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp620)->f1; goto _LL711; _LL711: _temp710=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp620)->f2; goto _LL709; _LL709: _temp708=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp620)->f3;
goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int) _temp620 > 2u?((
struct _tunion_struct*) _temp620)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){
_LL719: _temp718=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp620)->f1; goto _LL717; _LL717: _temp716=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp620)->f2; goto _LL715; _LL715: _temp714=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp620)->f3;
goto _LL653;} else{ goto _LL621;} _LL623: s=( void*) Cyc_Tcpat_Any; goto _LL621;
_LL625: s=( void*) Cyc_Tcpat_Any; goto _LL621; _LL627: s= Cyc_Tcpat_null_pat();
goto _LL621; _LL629: s= Cyc_Tcpat_int_pat( _temp656); goto _LL621; _LL631: s=
Cyc_Tcpat_char_pat( _temp660); goto _LL621; _LL633: s= Cyc_Tcpat_float_pat(
_temp662); goto _LL621; _LL635: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp664)); goto _LL621; _LL637: s=( void*) Cyc_Tcpat_Any; goto _LL621; _LL639: {
void* ss= Cyc_Tcpat_compile_pat( _temp668);{ void* _temp720= Cyc_Tcutil_compress((
void*)( p->topt)->v); struct Cyc_Absyn_PtrInfo _temp730; struct Cyc_Absyn_Conref*
_temp732; struct Cyc_Absyn_Tqual* _temp734; struct Cyc_Absyn_Conref* _temp736;
void* _temp738; void* _temp740; struct Cyc_Absyn_TunionInfo _temp742; struct Cyc_Absyn_XTunionInfo
_temp744; _LL722: if(( unsigned int) _temp720 > 4u?(( struct _tunion_struct*)
_temp720)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL731: _temp730=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp720)->f1; _LL741: _temp740=( void*)
_temp730.elt_typ; goto _LL739; _LL739: _temp738=( void*) _temp730.rgn_typ; goto
_LL737; _LL737: _temp736=( struct Cyc_Absyn_Conref*) _temp730.nullable; goto
_LL735; _LL735: _temp734=( struct Cyc_Absyn_Tqual*) _temp730.tq; goto _LL733;
_LL733: _temp732=( struct Cyc_Absyn_Conref*) _temp730.bounds; goto _LL723;}
else{ goto _LL724;} _LL724: if(( unsigned int) _temp720 > 4u?(( struct
_tunion_struct*) _temp720)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL743:
_temp742=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp720)->f1; goto _LL725;} else{ goto _LL726;} _LL726: if(( unsigned int)
_temp720 > 4u?(( struct _tunion_struct*) _temp720)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL745: _temp744=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp720)->f1; goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729; _LL723: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp746=(
void*) _temp736->v; struct Cyc_Absyn_Conref* _temp754; int _temp756; _LL748: if((
unsigned int) _temp746 > 1u?(( struct _tunion_struct*) _temp746)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL755: _temp754=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp746)->f1; goto _LL749;} else{ goto _LL750;} _LL750: if(( int) _temp746 ==
Cyc_Absyn_No_constr){ goto _LL751;} else{ goto _LL752;} _LL752: if((
unsigned int) _temp746 > 1u?(( struct _tunion_struct*) _temp746)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL757: _temp756=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp746)->f1;
goto _LL753;} else{ goto _LL747;} _LL749:( void*)( _temp736->v=( void*)(( void*)
_temp754->v)); continue; _LL751:( void*)( _temp736->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp758=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp758[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp759; _temp759.tag= Cyc_Absyn_Eq_constr_tag;
_temp759.f1=( void*) 0; _temp759;}); _temp758;}))); is_nullable= 0;
still_working= 0; goto _LL747; _LL753: is_nullable=( int) _temp756;
still_working= 0; goto _LL747; _LL747:;} if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL721;} _LL725: s= ss; goto _LL721;
_LL727: s= ss; goto _LL721; _LL729: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp760=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp761; _temp761.curr= _temp760; _temp761.base= _temp760;
_temp761.last_plus_one= _temp760 + 44; _temp761;})); goto _LL721; _LL721:;} goto
_LL621;} _LL641: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=(
struct Cyc_List_List*)( _temp676->fields)->v; for( 0; fields != 0; fields=
fields->tl){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)
fields->hd)->name,( struct _tagged_string)({ char* _temp762=( char*)""; struct
_tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 1; _temp763;})) == 0;{ struct Cyc_List_List*
dlps0= _temp670; for( 0; ! found? dlps0 != 0: 0; dlps0= dlps0->tl){ struct
_tuple6 _temp766; struct Cyc_Absyn_Pat* _temp767; struct Cyc_List_List* _temp769;
struct _tuple6* _temp764=( struct _tuple6*) dlps0->hd; _temp766=* _temp764;
_LL770: _temp769= _temp766.f1; goto _LL768; _LL768: _temp767= _temp766.f2; goto
_LL765; _LL765: { struct Cyc_List_List* _temp771= _temp769; struct Cyc_List_List
_temp777; struct Cyc_List_List* _temp778; void* _temp780; struct _tagged_string*
_temp782; _LL773: if( _temp771 == 0){ goto _LL775;} else{ _temp777=* _temp771;
_LL781: _temp780=( void*) _temp777.hd; if((( struct _tunion_struct*) _temp780)->tag
== Cyc_Absyn_FieldName_tag){ _LL783: _temp782=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp780)->f1; goto _LL779;} else{ goto _LL775;}
_LL779: _temp778=( struct Cyc_List_List*) _temp777.tl; if( _temp778 == 0){ goto
_LL774;} else{ goto _LL775;}} _LL775: goto _LL776; _LL774: if( Cyc_String_zstrptrcmp(
_temp782,(( struct Cyc_Absyn_Structfield*) fields->hd)->name) == 0){ ps=({
struct Cyc_List_List* _temp784=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp784->hd=( void*) Cyc_Tcpat_compile_pat( _temp767);
_temp784->tl= ps; _temp784;}); found= 1;} goto _LL772; _LL776:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp785=(
char*)"bad designator(s)"; struct _tagged_string _temp786; _temp786.curr=
_temp785; _temp786.base= _temp785; _temp786.last_plus_one= _temp785 + 18;
_temp786;})); goto _LL772; _LL772:;}}} if( ! found){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp787=(
char*)"bad designator"; struct _tagged_string _temp788; _temp788.curr= _temp787;
_temp788.base= _temp787; _temp788.last_plus_one= _temp787 + 15; _temp788;}));}}}
s= Cyc_Tcpat_tuple_pat( ps); goto _LL621;} _LL643: { int span=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)( _temp680->fields)->v);
s= Cyc_Tcpat_con_pat(*(* _temp678->name).f2,({ struct Cyc_Core_Opt* _temp789=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp789->v=(
void*) span; _temp789;}),(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp682)); goto
_LL621;} _LL645: s= Cyc_Tcpat_con_pat(*(* _temp690->name).f2, 0,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp694)); goto _LL621; _LL647: { int span=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp702->fields); s= Cyc_Tcpat_con_pat(*(* _temp700->name).f2,({
struct Cyc_Core_Opt* _temp790=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp790->v=( void*) span; _temp790;}), 0); goto _LL621;}
_LL649: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp791=( char*)"compile_pat: unknownid"; struct
_tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 23; _temp792;})); goto _LL621; _LL651: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp793=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp794; _temp794.curr= _temp793; _temp794.base= _temp793; _temp794.last_plus_one=
_temp793 + 25; _temp794;})); goto _LL621; _LL653: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp795=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp796; _temp796.curr=
_temp795; _temp796.base= _temp795; _temp796.last_plus_one= _temp795 + 27;
_temp796;})); goto _LL621; _LL621:;} return s;} typedef void* Cyc_Tcpat_Term_desc;
const int Cyc_Tcpat_Pos_tag= 0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; const int Cyc_Tcpat_Neg_tag= 1; struct Cyc_Tcpat_Neg_struct{
int tag; struct Cyc_Set_Set* f1; } ; typedef void* Cyc_Tcpat_term_desc_t;
typedef void* Cyc_Tcpat_Access; static const unsigned int Cyc_Tcpat_Obj= 0;
static const int Cyc_Tcpat_Sel_tag= 0; struct Cyc_Tcpat_Sel_struct{ int tag; int
f1; void* f2; } ; typedef void* Cyc_Tcpat_access_t; typedef void* Cyc_Tcpat_Decision;
static const unsigned int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success_tag=
0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq_tag=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; void* f1; struct Cyc_Tcpat_Con_s* f2;
void* f3; void* f4; } ; typedef void* Cyc_Tcpat_decision_t; typedef void* Cyc_Tcpat_Answer;
static const unsigned int Cyc_Tcpat_Yes= 0; static const unsigned int Cyc_Tcpat_No=
1; static const unsigned int Cyc_Tcpat_Maybe= 2; typedef void* Cyc_Tcpat_answer_t;
static void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void*
_temp797= td; struct Cyc_Set_Set* _temp803; struct Cyc_List_List* _temp805;
struct Cyc_Tcpat_Con_s* _temp807; _LL799: if((( struct _tunion_struct*) _temp797)->tag
== Cyc_Tcpat_Neg_tag){ _LL804: _temp803=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp797)->f1; goto _LL800;} else{ goto _LL801;} _LL801: if((( struct
_tunion_struct*) _temp797)->tag == Cyc_Tcpat_Pos_tag){ _LL808: _temp807=( struct
Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp797)->f1; goto _LL806;
_LL806: _temp805=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp797)->f2; goto _LL802;} else{ goto _LL798;} _LL800: if((( int(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp803, c)){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp809=( char*)"add_neg called when constructor already in set"; struct
_tagged_string _temp810; _temp810.curr= _temp809; _temp810.base= _temp809;
_temp810.last_plus_one= _temp809 + 47; _temp810;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp803) + 1 >=( int)( c->span)->v:
0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp811=( char*)"add_neg called when |cs U {c}| >= span(c)"; struct
_tagged_string _temp812; _temp812.curr= _temp811; _temp812.base= _temp811;
_temp812.last_plus_one= _temp811 + 42; _temp812;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp813=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp813[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp814; _temp814.tag= Cyc_Tcpat_Neg_tag; _temp814.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp803,
c); _temp814;}); _temp813;}); _LL802: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp815=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp816; _temp816.curr= _temp815; _temp816.base= _temp815;
_temp816.last_plus_one= _temp815 + 35; _temp816;})); _LL798:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp817= td; struct Cyc_List_List*
_temp823; struct Cyc_Tcpat_Con_s* _temp825; struct Cyc_Set_Set* _temp827; _LL819:
if((( struct _tunion_struct*) _temp817)->tag == Cyc_Tcpat_Pos_tag){ _LL826:
_temp825=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp817)->f1;
goto _LL824; _LL824: _temp823=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp817)->f2; goto _LL820;} else{ goto _LL821;} _LL821: if((( struct
_tunion_struct*) _temp817)->tag == Cyc_Tcpat_Neg_tag){ _LL828: _temp827=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp817)->f1; goto _LL822;} else{
goto _LL818;} _LL820: if( Cyc_Tcpat_compare_con( c, _temp825) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL822: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp827,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)( c->span)->v ==((
int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp827) + 1: 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}} _LL818:;} static
struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt, void* dsc){
struct Cyc_List_List* _temp829= ctxt; struct Cyc_List_List _temp835; struct Cyc_List_List*
_temp836; struct _tuple9* _temp838; struct _tuple9 _temp840; struct Cyc_List_List*
_temp841; struct Cyc_Tcpat_Con_s* _temp843; _LL831: if( _temp829 == 0){ goto
_LL832;} else{ goto _LL833;} _LL833: if( _temp829 == 0){ goto _LL830;} else{
_temp835=* _temp829; _LL839: _temp838=( struct _tuple9*) _temp835.hd; _temp840=*
_temp838; _LL844: _temp843= _temp840.f1; goto _LL842; _LL842: _temp841= _temp840.f2;
goto _LL837; _LL837: _temp836=( struct Cyc_List_List*) _temp835.tl; goto _LL834;}
_LL832: return 0; _LL834: return({ struct Cyc_List_List* _temp845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp845->hd=( void*)({ struct
_tuple9* _temp846=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp846->f1= _temp843; _temp846->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp847=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp847->hd=( void*) dsc; _temp847->tl= _temp841; _temp847;}); _temp846;});
_temp845->tl= _temp836; _temp845;}); _LL830:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp848= ctxt; struct Cyc_List_List
_temp854; struct Cyc_List_List* _temp855; struct _tuple9* _temp857; struct
_tuple9 _temp859; struct Cyc_List_List* _temp860; struct Cyc_Tcpat_Con_s*
_temp862; _LL850: if( _temp848 == 0){ goto _LL851;} else{ goto _LL852;} _LL852:
if( _temp848 == 0){ goto _LL849;} else{ _temp854=* _temp848; _LL858: _temp857=(
struct _tuple9*) _temp854.hd; _temp859=* _temp857; _LL863: _temp862= _temp859.f1;
goto _LL861; _LL861: _temp860= _temp859.f2; goto _LL856; _LL856: _temp855=(
struct Cyc_List_List*) _temp854.tl; goto _LL853;} _LL851: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp864=( char*)"norm_context: empty context"; struct _tagged_string _temp865;
_temp865.curr= _temp864; _temp865.base= _temp864; _temp865.last_plus_one=
_temp864 + 28; _temp865;})); _LL853: return Cyc_Tcpat_augment( _temp855,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp866=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp866[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp867; _temp867.tag= Cyc_Tcpat_Pos_tag; _temp867.f1= _temp862; _temp867.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp860);
_temp867;}); _temp866;})); _LL849:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp869=({ struct _tuple5 _temp868; _temp868.f1= ctxt; _temp868.f2= work;
_temp868;}); struct Cyc_List_List* _temp879; struct Cyc_List_List* _temp881;
struct Cyc_List_List* _temp883; struct Cyc_List_List* _temp885; struct Cyc_List_List*
_temp887; struct Cyc_List_List* _temp889; struct Cyc_List_List* _temp891; struct
Cyc_List_List _temp893; struct Cyc_List_List* _temp894; struct _tuple10*
_temp896; struct _tuple10 _temp898; struct Cyc_List_List* _temp899; struct Cyc_List_List*
_temp901; struct Cyc_List_List* _temp903; struct Cyc_List_List* _temp905; struct
Cyc_List_List _temp907; struct Cyc_List_List* _temp908; struct _tuple9* _temp910;
struct _tuple9 _temp912; struct Cyc_List_List* _temp913; struct Cyc_Tcpat_Con_s*
_temp915; _LL871: _LL882: _temp881= _temp869.f1; if( _temp881 == 0){ goto _LL880;}
else{ goto _LL873;} _LL880: _temp879= _temp869.f2; if( _temp879 == 0){ goto
_LL872;} else{ goto _LL873;} _LL873: _LL886: _temp885= _temp869.f1; if( _temp885
== 0){ goto _LL884;} else{ goto _LL875;} _LL884: _temp883= _temp869.f2; goto
_LL874; _LL875: _LL890: _temp889= _temp869.f1; goto _LL888; _LL888: _temp887=
_temp869.f2; if( _temp887 == 0){ goto _LL876;} else{ goto _LL877;} _LL877:
_LL906: _temp905= _temp869.f1; if( _temp905 == 0){ goto _LL870;} else{ _temp907=*
_temp905; _LL911: _temp910=( struct _tuple9*) _temp907.hd; _temp912=* _temp910;
_LL916: _temp915= _temp912.f1; goto _LL914; _LL914: _temp913= _temp912.f2; goto
_LL909; _LL909: _temp908=( struct Cyc_List_List*) _temp907.tl; goto _LL892;}
_LL892: _temp891= _temp869.f2; if( _temp891 == 0){ goto _LL870;} else{ _temp893=*
_temp891; _LL897: _temp896=( struct _tuple10*) _temp893.hd; _temp898=* _temp896;
_LL904: _temp903= _temp898.f1; goto _LL902; _LL902: _temp901= _temp898.f2; goto
_LL900; _LL900: _temp899= _temp898.f3; goto _LL895; _LL895: _temp894=( struct
Cyc_List_List*) _temp893.tl; goto _LL878;} _LL872: return dsc; _LL874: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp917=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp918; _temp918.curr= _temp917; _temp918.base= _temp917;
_temp918.last_plus_one= _temp917 + 38; _temp918;})); _LL876: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp919=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp920; _temp920.curr= _temp919; _temp920.base= _temp919; _temp920.last_plus_one=
_temp919 + 38; _temp920;})); _LL878: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp921=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp921[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp922; _temp922.tag= Cyc_Tcpat_Pos_tag; _temp922.f1= _temp915; _temp922.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp913),({
struct Cyc_List_List* _temp923=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp923->hd=( void*) dsc; _temp923->tl= _temp899;
_temp923;})); _temp922;}); _temp921;}); return Cyc_Tcpat_build_desc( _temp908,(
void*) td, _temp894);} _LL870:;} static void* Cyc_Tcpat_match( void* p, void*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp924=
allmrules; struct Cyc_List_List _temp930; struct Cyc_List_List* _temp931; struct
_tuple0* _temp933; struct _tuple0 _temp935; void* _temp936; void* _temp938;
_LL926: if( _temp924 == 0){ goto _LL927;} else{ goto _LL928;} _LL928: if(
_temp924 == 0){ goto _LL925;} else{ _temp930=* _temp924; _LL934: _temp933=(
struct _tuple0*) _temp930.hd; _temp935=* _temp933; _LL939: _temp938= _temp935.f1;
goto _LL937; _LL937: _temp936= _temp935.f2; goto _LL932; _LL932: _temp931=(
struct Cyc_List_List*) _temp930.tl; goto _LL929;} _LL927: return( void*) Cyc_Tcpat_Failure;
_LL929: return(( void*(*)( void* p, void* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp938,( void*) Cyc_Tcpat_Obj, dsc, 0, 0, _temp936,
_temp931); _LL925:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp940=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp940[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp941; _temp941.tag= Cyc_Tcpat_Neg_tag; _temp941.f1= Cyc_Tcpat_empty_con_set();
_temp941;}); _temp940;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp942= work; struct Cyc_List_List
_temp950; struct Cyc_List_List* _temp951; struct _tuple10* _temp953; struct
_tuple10 _temp955; struct Cyc_List_List* _temp956; struct Cyc_List_List*
_temp958; struct Cyc_List_List* _temp960; struct Cyc_List_List _temp962; struct
Cyc_List_List* _temp963; struct _tuple10* _temp965; struct _tuple10 _temp967;
struct Cyc_List_List* _temp968; struct Cyc_List_List* _temp970; struct Cyc_List_List*
_temp972; _LL944: if( _temp942 == 0){ goto _LL945;} else{ goto _LL946;} _LL946:
if( _temp942 == 0){ goto _LL948;} else{ _temp950=* _temp942; _LL954: _temp953=(
struct _tuple10*) _temp950.hd; _temp955=* _temp953; _LL961: _temp960= _temp955.f1;
if( _temp960 == 0){ goto _LL959;} else{ goto _LL948;} _LL959: _temp958= _temp955.f2;
if( _temp958 == 0){ goto _LL957;} else{ goto _LL948;} _LL957: _temp956= _temp955.f3;
if( _temp956 == 0){ goto _LL952;} else{ goto _LL948;} _LL952: _temp951=( struct
Cyc_List_List*) _temp950.tl; goto _LL947;} _LL948: if( _temp942 == 0){ goto
_LL943;} else{ _temp962=* _temp942; _LL966: _temp965=( struct _tuple10*)
_temp962.hd; _temp967=* _temp965; _LL973: _temp972= _temp967.f1; goto _LL971;
_LL971: _temp970= _temp967.f2; goto _LL969; _LL969: _temp968= _temp967.f3; goto
_LL964; _LL964: _temp963=( struct Cyc_List_List*) _temp962.tl; goto _LL949;}
_LL945: return( void*)({ struct Cyc_Tcpat_Success_struct* _temp974=( struct Cyc_Tcpat_Success_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp974[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp975; _temp975.tag= Cyc_Tcpat_Success_tag; _temp975.f1=( void*)
right_hand_side; _temp975;}); _temp974;}); _LL947: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context( ctx), _temp951,
right_hand_side, rules); _LL949: if(( _temp972 == 0? 1: _temp970 == 0)? 1:
_temp968 == 0){ return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp976=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp977; _temp977.curr= _temp976; _temp977.base= _temp976;
_temp977.last_plus_one= _temp976 + 38; _temp977;}));}{ struct Cyc_List_List
_temp981; struct Cyc_List_List* _temp982; void* _temp984; struct Cyc_List_List*
_temp979=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp978= _temp972;
if( _temp978 == 0){ _throw( Null_Exception);} _temp978;}); _temp981=* _temp979;
_LL985: _temp984=( void*) _temp981.hd; goto _LL983; _LL983: _temp982=( struct
Cyc_List_List*) _temp981.tl; goto _LL980; _LL980: { struct Cyc_List_List
_temp989; struct Cyc_List_List* _temp990; void* _temp992; struct Cyc_List_List*
_temp987=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp986= _temp970;
if( _temp986 == 0){ _throw( Null_Exception);} _temp986;}); _temp989=* _temp987;
_LL993: _temp992=( void*) _temp989.hd; goto _LL991; _LL991: _temp990=( struct
Cyc_List_List*) _temp989.tl; goto _LL988; _LL988: { struct Cyc_List_List
_temp997; struct Cyc_List_List* _temp998; void* _temp1000; struct Cyc_List_List*
_temp995=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp994= _temp968;
if( _temp994 == 0){ _throw( Null_Exception);} _temp994;}); _temp997=* _temp995;
_LL1001: _temp1000=( void*) _temp997.hd; goto _LL999; _LL999: _temp998=( struct
Cyc_List_List*) _temp997.tl; goto _LL996; _LL996: { struct _tuple10* wf=({
struct _tuple10* _temp1002=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp1002->f1= _temp982; _temp1002->f2= _temp990; _temp1002->f3=
_temp998; _temp1002;}); return(( void*(*)( void* p, void* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp984, _temp992, _temp1000, ctx,({
struct Cyc_List_List* _temp1003=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1003->hd=( void*) wf; _temp1003->tl= _temp963;
_temp1003;}), right_hand_side, rules);}}}} _LL943:;} static struct Cyc_List_List*
Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp1004=
dsc; struct Cyc_Set_Set* _temp1010; struct Cyc_List_List* _temp1012; struct Cyc_Tcpat_Con_s*
_temp1014; _LL1006: if((( struct _tunion_struct*) _temp1004)->tag == Cyc_Tcpat_Neg_tag){
_LL1011: _temp1010=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp1004)->f1; goto _LL1007;} else{ goto _LL1008;} _LL1008: if((( struct
_tunion_struct*) _temp1004)->tag == Cyc_Tcpat_Pos_tag){ _LL1015: _temp1014=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp1004)->f1; goto
_LL1013; _LL1013: _temp1012=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp1004)->f2; goto _LL1009;} else{ goto _LL1005;} _LL1007: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1016=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1016[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1017; _temp1017.tag= Cyc_Tcpat_Neg_tag; _temp1017.f1= Cyc_Tcpat_empty_con_set();
_temp1017;}); _temp1016;}); struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i <
pcon->arity; ++ i){ res=({ struct Cyc_List_List* _temp1018=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1018->hd=( void*) any; _temp1018->tl=
res; _temp1018;});}} return res;} _LL1009: return _temp1012; _LL1005:;} static
void* Cyc_Tcpat_getoarg( void* obj, int i){ return( void*)({ struct Cyc_Tcpat_Sel_struct*
_temp1019=( struct Cyc_Tcpat_Sel_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Sel_struct));
_temp1019[ 0]=({ struct Cyc_Tcpat_Sel_struct _temp1020; _temp1020.tag= Cyc_Tcpat_Sel_tag;
_temp1020.f1= i + 1; _temp1020.f2=( void*) obj; _temp1020;}); _temp1019;});}
static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon,
void* obj){ return(( struct Cyc_List_List*(*)( int n, void*(* f)( void*, int),
void* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static
void* Cyc_Tcpat_match( void* p, void* obj, void* dsc, struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules){
void* _temp1021= p; struct Cyc_List_List* _temp1027; struct Cyc_Tcpat_Con_s*
_temp1029; _LL1023: if(( int) _temp1021 == Cyc_Tcpat_Any){ goto _LL1024;} else{
goto _LL1025;} _LL1025: if(( unsigned int) _temp1021 > 1u?(( struct
_tunion_struct*) _temp1021)->tag == Cyc_Tcpat_Con_tag: 0){ _LL1030: _temp1029=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Con_struct*) _temp1021)->f1; goto
_LL1028; _LL1028: _temp1027=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*)
_temp1021)->f2; goto _LL1026;} else{ goto _LL1022;} _LL1024: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1026: { void* _temp1031= Cyc_Tcpat_static_match(
_temp1029, dsc); _LL1033: if(( int) _temp1031 == Cyc_Tcpat_Yes){ goto _LL1034;}
else{ goto _LL1035;} _LL1035: if(( int) _temp1031 == Cyc_Tcpat_No){ goto _LL1036;}
else{ goto _LL1037;} _LL1037: if(( int) _temp1031 == Cyc_Tcpat_Maybe){ goto
_LL1038;} else{ goto _LL1032;} _LL1034: { struct Cyc_List_List* ctx2=({ struct
Cyc_List_List* _temp1039=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1039->hd=( void*)({ struct _tuple9* _temp1040=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1040->f1= _temp1029; _temp1040->f2= 0; _temp1040;});
_temp1039->tl= ctx; _temp1039;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1041=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1041->f1=
_temp1027; _temp1041->f2= Cyc_Tcpat_getoargs( _temp1029, obj); _temp1041->f3=
Cyc_Tcpat_getdargs( _temp1029, dsc); _temp1041;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1042=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1042->hd=( void*) work_frame; _temp1042->tl= work;
_temp1042;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules);} _LL1036: return(( void*(*)( void* dsc,
struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx,
dsc, work), rules); _LL1038: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1043=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1043->hd=( void*)({ struct _tuple9* _temp1044=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp1044->f1= _temp1029; _temp1044->f2= 0; _temp1044;});
_temp1043->tl= ctx; _temp1043;}); struct _tuple10* work_frame=({ struct _tuple10*
_temp1045=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1045->f1=
_temp1027; _temp1045->f2= Cyc_Tcpat_getoargs( _temp1029, obj); _temp1045->f3=
Cyc_Tcpat_getdargs( _temp1029, dsc); _temp1045;}); struct Cyc_List_List* work2=({
struct Cyc_List_List* _temp1046=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1046->hd=( void*) work_frame; _temp1046->tl= work;
_temp1046;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1029), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1047=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1047[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1048; _temp1048.tag= Cyc_Tcpat_IfEq_tag;
_temp1048.f1=( void*) obj; _temp1048.f2= _temp1029; _temp1048.f3=( void*) s;
_temp1048.f4=( void*) f; _temp1048;}); _temp1047;});} _LL1032:;} _LL1022:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1049= d;
void* _temp1057; void* _temp1059; void* _temp1061; struct Cyc_Tcpat_Con_s*
_temp1063; void* _temp1065; _LL1051: if(( int) _temp1049 == Cyc_Tcpat_Failure){
goto _LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1049 > 1u?((
struct _tunion_struct*) _temp1049)->tag == Cyc_Tcpat_Success_tag: 0){ _LL1058:
_temp1057=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1049)->f1; goto
_LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int) _temp1049 > 1u?((
struct _tunion_struct*) _temp1049)->tag == Cyc_Tcpat_IfEq_tag: 0){ _LL1066:
_temp1065=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1049)->f1; goto _LL1064;
_LL1064: _temp1063=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1049)->f2; goto _LL1062; _LL1062: _temp1061=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1049)->f3; goto _LL1060; _LL1060: _temp1059=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1049)->f4; goto _LL1056;} else{ goto _LL1050;} _LL1052: not_exhaust( env1);
goto _LL1050; _LL1054: rhs_appears( env2, _temp1057); goto _LL1050; _LL1056:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1061,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1059, not_exhaust, env1, rhs_appears, env2); goto _LL1050; _LL1050:;}
static struct _tuple12* Cyc_Tcpat_get_match( int* r, struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1067=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1067->f1= 0; _temp1067->f2=( swc->pattern)->loc; _temp1067;});
void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1068=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1068->hd=( void*) sp0; _temp1068->tl=({ struct Cyc_List_List* _temp1069=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1069->hd=(
void*) Cyc_Tcpat_int_pat(* r); _temp1069->tl= 0; _temp1069;}); _temp1068;}));* r=*
r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List* _temp1070=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1070->hd=(
void*) sp0; _temp1070->tl=({ struct Cyc_List_List* _temp1071=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1071->hd=( void*)(( void*) Cyc_Tcpat_Any);
_temp1071->tl= 0; _temp1071;}); _temp1070;}));} return({ struct _tuple12*
_temp1072=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1072->f1=
sp; _temp1072->f2= rhs; _temp1072;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp1073=( char*)"patterns may not be exhaustive"; struct _tagged_string
_temp1074; _temp1074.curr= _temp1073; _temp1074.base= _temp1073; _temp1074.last_plus_one=
_temp1073 + 31; _temp1074;}));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,({ int*
_temp1075=( int*) GC_malloc_atomic( sizeof( int)); _temp1075[ 0]= 0; _temp1075;}),
swcs); void* dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=
match_rules->tl){ struct _tuple11 _temp1078; struct Cyc_Position_Segment*
_temp1079; int _temp1081; struct _tuple11* _temp1076=(*(( struct _tuple12*)
match_rules->hd)).f2; _temp1078=* _temp1076; _LL1082: _temp1081= _temp1078.f1;
goto _LL1080; _LL1080: _temp1079= _temp1078.f2; goto _LL1077; _LL1077: if( !
_temp1081){ Cyc_Tcutil_terr( _temp1079,( struct _tagged_string)({ char*
_temp1083=( char*)"redundant pattern"; struct _tagged_string _temp1084;
_temp1084.curr= _temp1083; _temp1084.base= _temp1083; _temp1084.last_plus_one=
_temp1083 + 18; _temp1084;}));}}} static void Cyc_Tcpat_not_exhaust_warn( struct
_tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({
char* _temp1085=( char*)"pattern not exhaustive"; struct _tagged_string
_temp1086; _temp1086.curr= _temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one=
_temp1085 + 23; _temp1086;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){
return;} int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* match_rules=({ struct Cyc_List_List*
_temp1087=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1087->hd=( void*)({ struct _tuple14* _temp1088=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1088->f1= Cyc_Tcpat_compile_pat( p);
_temp1088->f2= 0; _temp1088;}); _temp1087->tl= 0; _temp1087;}); void* dec_tree=((
void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules); struct _tuple13* exhaust_env=({ struct _tuple13* _temp1089=(
struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1089->f1= loc;
_temp1089->f2= 1; _temp1089;});(( void(*)( void* d, void(* not_exhaust)( struct
_tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int env2))
Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_warn,
exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(* exhaust_env).f2;} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1090=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1090->f1=
0; _temp1090->f2=( swc->pattern)->loc; _temp1090;}); return({ struct _tuple12*
_temp1091=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1091->f1=
sp0; _temp1091->f2= rhs; _temp1091;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=
match_rules->tl){ struct _tuple11* pair=(*(( struct _tuple12*) match_rules->hd)).f2;
if( !(* pair).f1){ Cyc_Tcutil_terr((* pair).f2,( struct _tagged_string)({ char*
_temp1092=( char*)"redundant pattern"; struct _tagged_string _temp1093;
_temp1093.curr= _temp1092; _temp1093.base= _temp1092; _temp1093.last_plus_one=
_temp1092 + 18; _temp1093;}));}}}