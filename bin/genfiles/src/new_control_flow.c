#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd(
struct _tuple0*); extern int Cyc_Core_intcmp( int, int); extern char Cyc_Core_InvalidArg_tag[
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
Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern void* Cyc_Assert_AssertFail(
struct _tagged_string msg); struct Cyc_Lineno_Pos{ struct _tagged_string
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
; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
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
char* tag; } ; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); typedef void* Cyc_CfFlowInfo_LocalRoot;
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
void* root; struct Cyc_List_List* fields; } ; extern const unsigned int Cyc_CfFlowInfo_Esc;
extern const unsigned int Cyc_CfFlowInfo_Unesc; extern const unsigned int Cyc_CfFlowInfo_NoneIL;
extern const unsigned int Cyc_CfFlowInfo_ThisIL; extern const unsigned int Cyc_CfFlowInfo_AllIL;
extern const int Cyc_CfFlowInfo_UnknownIS_tag; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; extern const int Cyc_CfFlowInfo_MustPointTo_tag;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; extern const int Cyc_CfFlowInfo_LeafPI_tag; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; extern const int Cyc_CfFlowInfo_TuplePI_tag; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern const int Cyc_CfFlowInfo_StructPI_tag;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern const unsigned int Cyc_CfFlowInfo_BottomFL; extern const int Cyc_CfFlowInfo_InitsFL_tag;
struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern
void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct
Cyc_CfAbsexp_BadAbsexp_struct{ char* tag; } ; typedef void* Cyc_CfAbsexp_Absexp;
typedef void* Cyc_CfAbsexp_Absexpgroup; extern const unsigned int Cyc_CfAbsexp_OrderedG;
extern const unsigned int Cyc_CfAbsexp_UnorderedG; extern const unsigned int Cyc_CfAbsexp_OneofG;
typedef void* Cyc_CfAbsexp_Absop; typedef void* Cyc_CfAbsexp_absexp_t; typedef
void* Cyc_CfAbsexp_absexpgroup_t; typedef void* Cyc_CfAbsexp_absop_t; extern
void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkAssignAE( void*
l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern int Cyc_CfAbsexp_isUnknownOp( void*); extern
void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, void* ae, void* in_flow); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt*
encloser; int visited; void* flow; struct Cyc_List_List* absexps; } ; typedef
struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t; char
Cyc_NewControlFlow_CFAnnot_tag[ 8u]="CFAnnot"; struct Cyc_NewControlFlow_CFAnnot_struct{
char* tag; struct Cyc_NewControlFlow_CFStmtAnnot f1; } ; void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser){ enclosee->annot=(
struct _xtunion_struct*)({ struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=(
struct Cyc_NewControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct));
_temp0[ 0]=({ struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot_tag;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;});} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt* s){ struct
_xtunion_struct* _temp3= s->annot; struct Cyc_NewControlFlow_CFStmtAnnot _temp9;
struct Cyc_NewControlFlow_CFStmtAnnot* _temp11; _LL5: if((*(( struct
_xtunion_struct*) _temp3)).tag == Cyc_NewControlFlow_CFAnnot_tag){ _LL10: _temp9=((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11;
_LL8:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp12=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp12[ 0]=({ struct Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible_tag;
_temp13.f1=( struct _tagged_string)({ char* _temp14=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14;
_temp15.last_plus_one= _temp14 + 37; _temp15;}); _temp13;}); _temp12;})); _LL4:;}
void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{
struct Cyc_Dict_Dict* all_roots; struct Cyc_List_List* comprehension_vars; } ;
struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_NewControlFlow_SharedAbsEnv* shared;
struct Cyc_List_List* lhsides; struct Cyc_List_List* access_path; } ; typedef
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_absenv_t; struct Cyc_NewControlFlow_AbsSynOne{
void* inner_exp; void* assigns; } ; typedef struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_abssyn_one_t; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; typedef struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abssyn_t;
void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp16=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp16[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp17.f1= vd; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp18=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp18[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp19; _temp19.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp19.f1= e; _temp19;}); _temp18;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
void Cyc_NewControlFlow_add_tunion_root( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_Absyn_Tunionfield
_temp22; struct Cyc_Position_Segment* _temp23; struct Cyc_List_List* _temp25;
struct Cyc_List_List* _temp27; struct _tuple1* _temp29; struct Cyc_Absyn_Tunionfield*
_temp20= ef; _temp22=* _temp20; _LL30: _temp29=( struct _tuple1*) _temp22.name;
goto _LL28; _LL28: _temp27=( struct Cyc_List_List*) _temp22.tvs; goto _LL26;
_LL26: _temp25=( struct Cyc_List_List*) _temp22.typs; goto _LL24; _LL24: _temp23=(
struct Cyc_Position_Segment*) _temp22.loc; goto _LL21; _LL21: { struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{
int i= 0; for( 0; _temp25 != 0; _temp25= _temp25->tl, ++ i){ struct _tuple4
_temp33; void* _temp34; struct Cyc_Absyn_Tqual* _temp36; struct _tuple4* _temp31=(
struct _tuple4*) _temp25->hd; _temp33=* _temp31; _LL37: _temp36= _temp33.f1;
goto _LL35; _LL35: _temp34= _temp33.f2; goto _LL32; _LL32: d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp34));}}( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp38=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp38[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp39; _temp39.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp39.f1= e; _temp39;}); _temp38;}),( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp40=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp40[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp41; _temp41.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp41.f1= d; _temp41;}); _temp40;}));}} struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs(
struct Cyc_NewControlFlow_AbsEnv src, struct Cyc_List_List* new_lhs){ return({
struct Cyc_NewControlFlow_AbsEnv _temp42; _temp42.shared= src.shared; _temp42.lhsides=
new_lhs; _temp42.access_path= src.access_path; _temp42;});} static struct Cyc_List_List*
Cyc_NewControlFlow_unknown_list= 0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){
if( Cyc_NewControlFlow_unknown_list == 0){ Cyc_NewControlFlow_unknown_list=({
struct Cyc_List_List* _temp43=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp43->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp43->tl= 0;
_temp43;});} return Cyc_NewControlFlow_unknown_list;} struct Cyc_NewControlFlow_AbsEnv
Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv src){ struct Cyc_NewControlFlow_AbsEnv
ans= Cyc_NewControlFlow_change_lhs( src, Cyc_NewControlFlow_use_it_list()); ans.access_path=
0; return ans;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp46; struct Cyc_NewControlFlow_AbsSynOne
_temp48; void* _temp50; void* _temp52; struct Cyc_NewControlFlow_AbsSynOne
_temp54; void* _temp56; void* _temp58; struct Cyc_NewControlFlow_AbsSyn _temp44=
syn; _LL55: _temp54=( struct Cyc_NewControlFlow_AbsSynOne) _temp44.when_true;
_LL59: _temp58=( void*) _temp54.inner_exp; goto _LL57; _LL57: _temp56=( void*)
_temp54.assigns; goto _LL49; _LL49: _temp48=( struct Cyc_NewControlFlow_AbsSynOne)
_temp44.when_false; _LL53: _temp52=( void*) _temp48.inner_exp; goto _LL51; _LL51:
_temp50=( void*) _temp48.assigns; goto _LL47; _LL47: _temp46=( struct Cyc_List_List*)
_temp44.lvalues; goto _LL45; _LL45: { void* inn= _temp58 == _temp52? _temp58:
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp58, _temp52); void*
ass= _temp56 == _temp50? _temp56: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
_temp56, _temp50); return({ struct Cyc_NewControlFlow_AbsSynOne _temp60; _temp60.inner_exp=(
void*) inn; _temp60.assigns=( void*) ass; _temp60;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne one_way=({ struct Cyc_NewControlFlow_AbsSynOne
_temp61; _temp61.inner_exp=( void*) ae; _temp61.assigns=( void*) assigns;
_temp61;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp62; _temp62.when_true=
one_way; _temp62.when_false= one_way; _temp62.lvalues= lvals; _temp62;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(( env.lhsides)->tl ==
0){ return Cyc_CfAbsexp_mkAssignAE(( void*)( env.lhsides)->hd, ae);}{ struct Cyc_List_List*
l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0; lhsides=
lhsides->tl){ l=({ struct Cyc_List_List* _temp63=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp63->hd=( void*) Cyc_CfAbsexp_mkAssignAE((
void*) lhsides->hd, ae); _temp63->tl= l; _temp63;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es= es->tl){ struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*) es->hd); void* _temp66; void* _temp68; struct Cyc_NewControlFlow_AbsSynOne
_temp64= Cyc_NewControlFlow_meet_absexp( syn); _LL69: _temp68=( void*) _temp64.inner_exp;
goto _LL67; _LL67: _temp66=( void*) _temp64.assigns; goto _LL65; _LL65: es_exp=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp68, es_exp);
uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp66, uses_exp);}
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}
struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* es_exp, void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE( e), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e))));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp70=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp70->hd=( void*) path_e; _temp70->tl=
env.access_path; _temp70;});{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path= path->tl){ void* _temp71=( void*)(( struct Cyc_Absyn_Exp*) path->hd)->r;
struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp* _temp87; struct
_tagged_string* _temp89; struct Cyc_Absyn_Exp* _temp91; struct _tagged_string*
_temp93; struct Cyc_Absyn_Exp* _temp95; struct Cyc_Absyn_Exp* _temp97; struct
Cyc_Absyn_Exp* _temp99; _LL73: if((( struct _tunion_struct*) _temp71)->tag ==
Cyc_Absyn_Address_e_tag){ _LL86: _temp85=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp71)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if((( struct _tunion_struct*)
_temp71)->tag == Cyc_Absyn_Deref_e_tag){ _LL88: _temp87=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp71)->f1; goto _LL76;} else{ goto _LL77;}
_LL77: if((( struct _tunion_struct*) _temp71)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL92: _temp91=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp71)->f1; goto _LL90; _LL90: _temp89=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp71)->f2; goto _LL78;} else{ goto _LL79;} _LL79: if((( struct _tunion_struct*)
_temp71)->tag == Cyc_Absyn_StructMember_e_tag){ _LL96: _temp95=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp71)->f1; goto _LL94; _LL94:
_temp93=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp71)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _tunion_struct*)
_temp71)->tag == Cyc_Absyn_Subscript_e_tag){ _LL100: _temp99=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp71)->f1; goto _LL98; _LL98: _temp97=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp71)->f2;
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL74: path->tl == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp101=( char*)"new_control_flow.cyc:239 path->tl == null";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 42; _temp102;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL76: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL72; _LL78: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp93= _temp89; goto _LL80; _LL80: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp103=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp103[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp104; _temp104.tag= Cyc_CfFlowInfo_StructF_tag;
_temp104.f1= _temp93; _temp104;}); _temp103;})); goto _LL72; _LL82: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp97); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp105=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp105[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp106; _temp106.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp106.f1=( int) i; _temp106;}); _temp105;})); goto _LL72;} _LL84:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp107=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp107[ 0]=({ struct Cyc_Core_Impossible_struct _temp108; _temp108.tag= Cyc_Core_Impossible_tag;
_temp108.f1=( struct _tagged_string)({ char* _temp109=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 44; _temp110;}); _temp108;}); _temp107;}));
_LL72:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp111=(
void*) e->r; struct Cyc_Absyn_Exp* _temp201; struct Cyc_Absyn_Exp* _temp203;
struct Cyc_List_List* _temp205; struct Cyc_Absyn_Exp* _temp207; void* _temp209;
int _temp211; void* _temp213; void* _temp215; int _temp217; void* _temp219; void*
_temp221; void* _temp223; struct Cyc_Absyn_Enumfield* _temp225; struct Cyc_Absyn_Enumdecl*
_temp227; struct _tuple1* _temp229; void* _temp231; struct Cyc_Absyn_Fndecl*
_temp233; struct _tuple1* _temp235; void* _temp237; struct Cyc_Absyn_Vardecl*
_temp239; struct _tuple1* _temp241; void* _temp243; struct Cyc_Absyn_Vardecl*
_temp245; struct _tuple1* _temp247; void* _temp249; struct Cyc_Absyn_Vardecl*
_temp251; struct _tuple1* _temp253; void* _temp255; struct Cyc_Absyn_Vardecl*
_temp257; struct _tuple1* _temp259; struct Cyc_Absyn_Tunionfield* _temp261;
struct Cyc_Absyn_Tuniondecl* _temp263; struct Cyc_List_List* _temp265; struct
Cyc_Core_Opt* _temp267; struct Cyc_Core_Opt* _temp269; struct Cyc_Absyn_Tunionfield*
_temp271; struct Cyc_Absyn_XTuniondecl* _temp273; struct Cyc_List_List* _temp275;
struct Cyc_Core_Opt* _temp277; void* _temp279; void* _temp281; void* _temp283;
struct Cyc_Absyn_Exp* _temp285; struct Cyc_Core_Opt* _temp287; struct Cyc_Absyn_Exp*
_temp289; struct Cyc_Absyn_Exp* _temp291; struct Cyc_Core_Opt* _temp293; struct
Cyc_Absyn_Exp* _temp295; struct Cyc_List_List* _temp297; void* _temp299; struct
Cyc_Absyn_Exp* _temp301; void* _temp303; struct Cyc_List_List* _temp305; struct
Cyc_Absyn_Exp* _temp307; struct Cyc_Absyn_Exp* _temp309; struct Cyc_Absyn_Exp*
_temp311; struct Cyc_Absyn_Exp* _temp313; struct Cyc_Absyn_Exp* _temp315; struct
Cyc_Absyn_Exp* _temp317; struct _tagged_string* _temp319; struct Cyc_Absyn_Exp*
_temp321; struct _tagged_string* _temp323; struct Cyc_Absyn_Exp* _temp325;
struct Cyc_List_List* _temp327; struct Cyc_Absyn_Structdecl* _temp329; struct
Cyc_List_List* _temp331; struct Cyc_Core_Opt* _temp333; struct _tuple1* _temp335;
struct Cyc_List_List* _temp337; void* _temp339; struct Cyc_Absyn_Exp* _temp341;
struct Cyc_Absyn_Exp* _temp343; struct Cyc_Absyn_Exp* _temp345; struct Cyc_Absyn_Exp*
_temp347; struct Cyc_Absyn_Exp* _temp349; struct Cyc_Absyn_Stmt* _temp351;
struct Cyc_Absyn_Exp* _temp353; struct Cyc_Absyn_Exp* _temp355; struct Cyc_Absyn_Exp*
_temp357; struct Cyc_Absyn_Exp* _temp359; struct Cyc_Absyn_Vardecl* _temp361;
void* _temp363; struct _tuple1* _temp365; struct _tuple1* _temp367; struct Cyc_List_List*
_temp369; struct Cyc_Absyn_Exp* _temp371; struct Cyc_List_List* _temp373; struct
Cyc_Core_Opt* _temp375; struct Cyc_List_List* _temp377; struct _tuple2* _temp379;
struct Cyc_Absyn_Fndecl* _temp381; struct Cyc_Absyn_Exp* _temp383; _LL113: if(((
struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL202:
_temp201=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp111)->f1; goto _LL114;} else{ goto _LL115;} _LL115: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL204: _temp203=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp111)->f1;
goto _LL116;} else{ goto _LL117;} _LL117: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL208: _temp207=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp111)->f1; goto _LL206; _LL206:
_temp205=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp111)->f2; goto _LL118;} else{ goto _LL119;} _LL119: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Const_e_tag){ _LL210: _temp209=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp111)->f1; if(( unsigned int)
_temp209 > 1u?(( struct _tunion_struct*) _temp209)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL214: _temp213=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp209)->f1;
if(( int) _temp213 == Cyc_Absyn_Signed){ goto _LL212;} else{ goto _LL121;}
_LL212: _temp211=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp209)->f2; if(
_temp211 == 0){ goto _LL120;} else{ goto _LL121;}} else{ goto _LL121;}} else{
goto _LL121;} _LL121: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Const_e_tag){
_LL216: _temp215=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp111)->f1; if((
unsigned int) _temp215 > 1u?(( struct _tunion_struct*) _temp215)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL220: _temp219=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp215)->f1;
if(( int) _temp219 == Cyc_Absyn_Signed){ goto _LL218;} else{ goto _LL123;}
_LL218: _temp217=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp215)->f2; if(
_temp217 == 1){ goto _LL122;} else{ goto _LL123;}} else{ goto _LL123;}} else{
goto _LL123;} _LL123: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL222: _temp221=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp111)->f1;
goto _LL124;} else{ goto _LL125;} _LL125: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Const_e_tag){ _LL224: _temp223=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp111)->f1; goto _LL126;} else{ goto _LL127;} _LL127: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Enum_e_tag){ _LL230: _temp229=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp111)->f1; goto _LL228;
_LL228: _temp227=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp111)->f2; goto _LL226; _LL226: _temp225=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp111)->f3; goto _LL128;} else{ goto _LL129;}
_LL129: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Var_e_tag){
_LL236: _temp235=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp111)->f1;
goto _LL232; _LL232: _temp231=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp231 > 1u?(( struct _tunion_struct*)
_temp231)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL234: _temp233=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp231)->f1; goto _LL130;} else{ goto
_LL131;}} else{ goto _LL131;} _LL131: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Var_e_tag){ _LL242: _temp241=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f1; goto _LL238; _LL238: _temp237=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp237 > 1u?(( struct _tunion_struct*)
_temp237)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL240: _temp239=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp237)->f1; goto _LL132;} else{ goto
_LL133;}} else{ goto _LL133;} _LL133: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Var_e_tag){ _LL248: _temp247=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f1; goto _LL244; _LL244: _temp243=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp243 > 1u?(( struct _tunion_struct*)
_temp243)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL246: _temp245=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp243)->f1; goto _LL134;} else{ goto _LL135;}}
else{ goto _LL135;} _LL135: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Var_e_tag){
_LL254: _temp253=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp111)->f1;
goto _LL250; _LL250: _temp249=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp249 > 1u?(( struct _tunion_struct*)
_temp249)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL252: _temp251=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp249)->f1; goto _LL136;} else{ goto _LL137;}}
else{ goto _LL137;} _LL137: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Var_e_tag){
_LL260: _temp259=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp111)->f1;
goto _LL256; _LL256: _temp255=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp255 > 1u?(( struct _tunion_struct*)
_temp255)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL258: _temp257=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp255)->f1; goto _LL138;} else{ goto _LL139;}}
else{ goto _LL139;} _LL139: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Tunion_e_tag){
_LL270: _temp269=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp111)->f1; goto _LL268; _LL268: _temp267=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp111)->f2; goto _LL266; _LL266: _temp265=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp111)->f3; goto
_LL264; _LL264: _temp263=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp111)->f4; goto _LL262; _LL262: _temp261=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp111)->f5; goto _LL140;} else{ goto
_LL141;} _LL141: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_XTunion_e_tag){
_LL278: _temp277=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp111)->f1; goto _LL276; _LL276: _temp275=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_e_struct*) _temp111)->f2; goto _LL274; _LL274: _temp273=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp111)->f3;
goto _LL272; _LL272: _temp271=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp111)->f4; goto _LL142;} else{ goto _LL143;} _LL143: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Malloc_e_tag){ _LL280: _temp279=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp111)->f1; if((( struct
_tunion_struct*) _temp279)->tag == Cyc_Absyn_Typ_m_tag){ _LL282: _temp281=( void*)((
struct Cyc_Absyn_Typ_m_struct*) _temp279)->f1; goto _LL144;} else{ goto _LL145;}}
else{ goto _LL145;} _LL145: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Malloc_e_tag){
_LL284: _temp283=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp111)->f1;
goto _LL146;} else{ goto _LL147;} _LL147: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL290: _temp289=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp111)->f1; goto _LL288; _LL288: _temp287=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp111)->f2; if(
_temp287 == 0){ goto _LL286;} else{ goto _LL149;} _LL286: _temp285=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp111)->f3; goto _LL148;} else{ goto
_LL149;} _LL149: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL296: _temp295=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp111)->f1; goto _LL294; _LL294: _temp293=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp111)->f2; goto _LL292; _LL292: _temp291=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp111)->f3;
goto _LL150;} else{ goto _LL151;} _LL151: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Primop_e_tag){ _LL300: _temp299=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp111)->f1; goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp111)->f2; goto _LL152;} else{ goto _LL153;}
_LL153: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Cast_e_tag){
_LL304: _temp303=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp111)->f1; goto
_LL302; _LL302: _temp301=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp111)->f2; goto _LL154;} else{ goto _LL155;} _LL155: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_FnCall_e_tag){ _LL308: _temp307=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp111)->f1; goto
_LL306; _LL306: _temp305=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp111)->f2; goto _LL156;} else{ goto _LL157;} _LL157: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Subscript_e_tag){ _LL312: _temp311=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp111)->f1;
goto _LL310; _LL310: _temp309=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp111)->f2; goto _LL158;} else{ goto _LL159;} _LL159: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_New_e_tag){ _LL314: _temp313=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp111)->f1; goto
_LL160;} else{ goto _LL161;} _LL161: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Address_e_tag){ _LL316: _temp315=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp111)->f1; goto _LL162;} else{ goto _LL163;}
_LL163: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Deref_e_tag){
_LL318: _temp317=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp111)->f1; goto _LL164;} else{ goto _LL165;} _LL165: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL322:
_temp321=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp111)->f1; goto _LL320; _LL320: _temp319=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp111)->f2; goto _LL166;} else{ goto _LL167;}
_LL167: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_StructMember_e_tag){
_LL326: _temp325=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp111)->f1; goto _LL324; _LL324: _temp323=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp111)->f2; goto _LL168;} else{ goto _LL169;}
_LL169: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Tuple_e_tag){
_LL328: _temp327=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp111)->f1; goto _LL170;} else{ goto _LL171;} _LL171: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Struct_e_tag){ _LL336: _temp335=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp111)->f1; goto _LL334;
_LL334: _temp333=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp111)->f2; goto _LL332; _LL332: _temp331=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp111)->f3; goto _LL330; _LL330: _temp329=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp111)->f4;
goto _LL172;} else{ goto _LL173;} _LL173: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Array_e_tag){ _LL338: _temp337=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp111)->f1; goto _LL174;} else{ goto _LL175;}
_LL175: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Increment_e_tag){
_LL342: _temp341=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp111)->f1; goto _LL340; _LL340: _temp339=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp111)->f2; goto _LL176;} else{ goto _LL177;} _LL177: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Throw_e_tag){ _LL344: _temp343=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp111)->f1; goto
_LL178;} else{ goto _LL179;} _LL179: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL350: _temp349=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp111)->f1; goto _LL348; _LL348:
_temp347=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp111)->f2; goto _LL346; _LL346: _temp345=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp111)->f3; goto _LL180;} else{ goto _LL181;}
_LL181: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL352: _temp351=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp111)->f1; goto _LL182;} else{ goto _LL183;} _LL183: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL356: _temp355=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp111)->f1; goto
_LL354; _LL354: _temp353=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp111)->f2; goto _LL184;} else{ goto _LL185;} _LL185: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL362:
_temp361=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp111)->f1; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp111)->f2; goto _LL358; _LL358: _temp357=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp111)->f3;
goto _LL186;} else{ goto _LL187;} _LL187: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Var_e_tag){ _LL366: _temp365=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f1; goto _LL364; _LL364: _temp363=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( int) _temp363 == Cyc_Absyn_Unresolved_b){ goto _LL188;}
else{ goto _LL189;}} else{ goto _LL189;} _LL189: if((( struct _tunion_struct*)
_temp111)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL368: _temp367=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp111)->f1; goto _LL190;} else{ goto
_LL191;} _LL191: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL372: _temp371=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp111)->f1; goto _LL370; _LL370: _temp369=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp111)->f2; goto _LL192;} else{ goto _LL193;}
_LL193: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL376: _temp375=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp111)->f1; goto _LL374; _LL374: _temp373=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp111)->f2; goto _LL194;} else{ goto
_LL195;} _LL195: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL380: _temp379=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp111)->f1; goto _LL378; _LL378: _temp377=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp111)->f2; goto _LL196;} else{ goto _LL197;}
_LL197: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Codegen_e_tag){
_LL382: _temp381=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp111)->f1; goto _LL198;} else{ goto _LL199;} _LL199: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Fill_e_tag){ _LL384: _temp383=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp111)->f1; goto
_LL200;} else{ goto _LL112;} _LL114: _temp203= _temp201; goto _LL116; _LL116:
_temp207= _temp203; goto _LL118; _LL118: return Cyc_NewControlFlow_abstract_exp(
env, _temp207); _LL120: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp385=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 47; _temp386;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp387; _temp387.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp389;
_temp389.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp389.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp389;}); _temp387.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp388; _temp388.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp388.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp388;}); _temp387.lvalues= 0; _temp387;}); _LL122: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp390=( char*)"new_control_flow.cyc:268 env.access_path==null";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 47; _temp391;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp392; _temp392.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp394;
_temp394.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp394.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp394;});
_temp392.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp393; _temp393.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp393.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp393;}); _temp392.lvalues= 0; _temp392;}); _LL124: goto _LL126; _LL126: goto
_LL128; _LL128: goto _LL130; _LL130: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL132:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL134:
_temp251= _temp245; goto _LL136; _LL136: _temp257= _temp251; goto _LL138; _LL138: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp257)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp395=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp395->hd=( void*) absop; _temp395->tl= 0; _temp395;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL140: _temp275= _temp265;
_temp271= _temp261; goto _LL142; _LL142: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp396=( char*)"new_control_flow.cyc:299 env.access_path == null";
struct _tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 49; _temp397;})); if( _temp275 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_tunion_root( env, e,
_temp271);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp275 != 0; _temp275= _temp275->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp398=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp398->hd=(
void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp( e),( void*)({ struct
Cyc_CfFlowInfo_TupleF_struct* _temp399=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp399[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp400; _temp400.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp400.f1= i; _temp400;}); _temp399;})); _temp398->tl= 0; _temp398;})); struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*) _temp275->hd); void* _temp403; void* _temp405; struct Cyc_NewControlFlow_AbsSynOne
_temp401= Cyc_NewControlFlow_meet_absexp( syn); _LL406: _temp405=( void*)
_temp401.inner_exp; goto _LL404; _LL404: _temp403=( void*) _temp401.assigns;
goto _LL402; _LL402: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp405, es_exp); useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp403, useexp);}} return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp,
useexp);} _LL144: Cyc_NewControlFlow_add_malloc_root( env, e, _temp281); return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE( e), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp( e)))); _LL146:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp407=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp407[ 0]=({ struct Cyc_Core_Impossible_struct _temp408; _temp408.tag= Cyc_Core_Impossible_tag;
_temp408.f1=( struct _tagged_string)({ char* _temp409=( char*)"NewControlFlow: Malloc_e of unsupported variant";
struct _tagged_string _temp410; _temp410.curr= _temp409; _temp410.base= _temp409;
_temp410.last_plus_one= _temp409 + 48; _temp410;}); _temp408;}); _temp407;}));
_LL148: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp289); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp285); void* _temp413; void*
_temp415; struct Cyc_NewControlFlow_AbsSynOne _temp411= Cyc_NewControlFlow_meet_absexp(
syn1); _LL416: _temp415=( void*) _temp411.inner_exp; goto _LL414; _LL414:
_temp413=( void*) _temp411.assigns; goto _LL412; _LL412: { void* _temp419; void*
_temp421; struct Cyc_NewControlFlow_AbsSynOne _temp417= Cyc_NewControlFlow_meet_absexp(
syn2); _LL422: _temp421=( void*) _temp417.inner_exp; goto _LL420; _LL420:
_temp419=( void*) _temp417.assigns; goto _LL418; _LL418: { void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp415, _temp421), _temp419); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0,
_temp413);}}} _LL150: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp423=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp423->hd=( void*) _temp295; _temp423->tl=({ struct Cyc_List_List* _temp424=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp424->hd=(
void*) _temp291; _temp424->tl= 0; _temp424;}); _temp423;}), 0); _LL152: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp297, 0); _LL154: { int both_leaf=
1;{ void* _temp425=( void*)( _temp301->topt)->v; struct Cyc_Absyn_Structdecl**
_temp433; struct Cyc_List_List* _temp435; struct _tuple1* _temp437; struct Cyc_List_List*
_temp439; _LL427: if(( unsigned int) _temp425 > 4u?(( struct _tunion_struct*)
_temp425)->tag == Cyc_Absyn_StructType_tag: 0){ _LL438: _temp437=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp425)->f1; goto _LL436;
_LL436: _temp435=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp425)->f2; goto _LL434; _LL434: _temp433=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp425)->f3; goto _LL428;} else{ goto
_LL429;} _LL429: if(( unsigned int) _temp425 > 4u?(( struct _tunion_struct*)
_temp425)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL440: _temp439=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp425)->f1; goto _LL430;} else{ goto
_LL431;} _LL431: goto _LL432; _LL428: goto _LL430; _LL430: both_leaf= 0; goto
_LL426; _LL432: goto _LL426; _LL426:;}{ void* _temp441=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp449; struct Cyc_List_List* _temp451; struct
_tuple1* _temp453; struct Cyc_List_List* _temp455; _LL443: if(( unsigned int)
_temp441 > 4u?(( struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_StructType_tag:
0){ _LL454: _temp453=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp441)->f1; goto _LL452; _LL452: _temp451=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp441)->f2; goto _LL450; _LL450: _temp449=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp441)->f3;
goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int) _temp441 > 4u?((
struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL456:
_temp455=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp441)->f1;
goto _LL446;} else{ goto _LL447;} _LL447: goto _LL448; _LL444: goto _LL446;
_LL446: both_leaf= 0; goto _LL442; _LL448: goto _LL442; _LL442:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp301);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp457=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp457->hd=( void*) _temp301; _temp457->tl= 0;
_temp457;}), env.lhsides);} _LL156: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp458=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp458->hd=( void*) _temp307; _temp458->tl=
_temp305; _temp458;}), 0); _LL158: { void* _temp459= Cyc_Tcutil_compress(( void*)(
_temp311->topt)->v); struct Cyc_List_List* _temp465; _LL461: if(( unsigned int)
_temp459 > 4u?(( struct _tunion_struct*) _temp459)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL466: _temp465=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp459)->f1; goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462:
return Cyc_NewControlFlow_descend_path( env, e, _temp311); _LL464: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp467=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp467->hd=( void*) _temp311; _temp467->tl=({
struct Cyc_List_List* _temp468=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp468->hd=( void*) _temp309; _temp468->tl= 0;
_temp468;}); _temp467;}), Cyc_NewControlFlow_use_it_list()); _LL460:;} _LL160: {
void* _temp469=( void*) _temp313->r; struct Cyc_Absyn_Structdecl* _temp477;
struct Cyc_List_List* _temp479; struct Cyc_Core_Opt* _temp481; struct _tuple1*
_temp483; struct Cyc_List_List* _temp485; _LL471: if((( struct _tunion_struct*)
_temp469)->tag == Cyc_Absyn_Struct_e_tag){ _LL484: _temp483=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp469)->f1; goto _LL482; _LL482: _temp481=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp469)->f2; goto
_LL480; _LL480: _temp479=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp469)->f3; goto _LL478; _LL478: _temp477=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp469)->f4; goto _LL472;} else{ goto
_LL473;} _LL473: if((( struct _tunion_struct*) _temp469)->tag == Cyc_Absyn_Tuple_e_tag){
_LL486: _temp485=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp469)->f1; goto _LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL472:
goto _LL474; _LL474: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)( _temp313->topt)->v);{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp487=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp487->hd=(
void*) malloc_op; _temp487->tl= 0; _temp487;})); inner_env.access_path= 0;{ void*
_temp490; void* _temp492; struct Cyc_NewControlFlow_AbsSynOne _temp488= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp313)); _LL493: _temp492=( void*)
_temp488.inner_exp; goto _LL491; _LL491: _temp490=( void*) _temp488.assigns;
goto _LL489; _LL489: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp492, _temp490); if( env.access_path != 0){ void* _temp494=( void*)((
struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r; struct Cyc_Absyn_Exp* _temp504;
struct Cyc_Absyn_Exp* _temp506; struct Cyc_Absyn_Exp* _temp508; struct
_tagged_string* _temp510; struct Cyc_Absyn_Exp* _temp512; _LL496: if((( struct
_tunion_struct*) _temp494)->tag == Cyc_Absyn_Subscript_e_tag){ _LL507: _temp506=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp494)->f1;
goto _LL505; _LL505: _temp504=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp494)->f2; goto _LL497;} else{ goto _LL498;} _LL498: if((( struct
_tunion_struct*) _temp494)->tag == Cyc_Absyn_Deref_e_tag){ _LL509: _temp508=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp494)->f1; goto
_LL499;} else{ goto _LL500;} _LL500: if((( struct _tunion_struct*) _temp494)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL513: _temp512=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp494)->f1; goto _LL511; _LL511:
_temp510=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp494)->f2; goto _LL501;} else{ goto _LL502;} _LL502: goto _LL503; _LL497:
Cyc_Evexp_eval_const_uint_exp( _temp504) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp514=( char*)"new_control_flow.cyc:413 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 61; _temp515;})); goto _LL499; _LL499: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp516=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp516->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp516->tl= 0; _temp516;}); goto _LL495; _LL501: env.access_path=({
struct Cyc_List_List* _temp517=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp517->hd=( void*) Cyc_Absyn_structmember_exp(
_temp512, _temp510, 0); _temp517->tl=( env.access_path)->tl; _temp517;}); ans.lvalues=({
struct Cyc_List_List* _temp518=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp518->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp518->tl= 0; _temp518;}); goto _LL495; _LL503:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp519=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp519[ 0]=({ struct Cyc_Core_Impossible_struct _temp520; _temp520.tag= Cyc_Core_Impossible_tag;
_temp520.f1=( struct _tagged_string)({ char* _temp521=( char*)"bad malloc access path";
struct _tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 23; _temp522;}); _temp520;}); _temp519;}));
_LL495:;} return ans;}}}} _LL476: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp523=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp523->hd=( void*) _temp313; _temp523->tl= 0;
_temp523;}), 0); _LL470:;} _LL162:{ void* _temp524=( void*) _temp315->r; struct
Cyc_Absyn_Structdecl* _temp532; struct Cyc_List_List* _temp534; struct Cyc_Core_Opt*
_temp536; struct _tuple1* _temp538; struct Cyc_List_List* _temp540; _LL526: if(((
struct _tunion_struct*) _temp524)->tag == Cyc_Absyn_Struct_e_tag){ _LL539:
_temp538=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp524)->f1;
goto _LL537; _LL537: _temp536=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp524)->f2; goto _LL535; _LL535: _temp534=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp524)->f3; goto _LL533; _LL533: _temp532=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp524)->f4;
goto _LL527;} else{ goto _LL528;} _LL528: if((( struct _tunion_struct*) _temp524)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL541: _temp540=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp524)->f1; goto _LL529;} else{ goto _LL530;}
_LL530: goto _LL531; _LL527: goto _LL529; _LL529: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp315->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp542=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp542->hd=( void*) malloc_op; _temp542->tl= 0;
_temp542;})); inner_env.access_path= 0;{ void* _temp545; void* _temp547; struct
Cyc_NewControlFlow_AbsSynOne _temp543= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp315)); _LL548: _temp547=( void*) _temp543.inner_exp; goto _LL546;
_LL546: _temp545=( void*) _temp543.assigns; goto _LL544; _LL544: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp547, _temp545); if( env.access_path
!= 0){ void* _temp549=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp559; struct Cyc_Absyn_Exp* _temp561; struct Cyc_Absyn_Exp*
_temp563; struct _tagged_string* _temp565; struct Cyc_Absyn_Exp* _temp567;
_LL551: if((( struct _tunion_struct*) _temp549)->tag == Cyc_Absyn_Subscript_e_tag){
_LL562: _temp561=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp549)->f1; goto _LL560; _LL560: _temp559=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp549)->f2; goto _LL552;} else{ goto _LL553;}
_LL553: if((( struct _tunion_struct*) _temp549)->tag == Cyc_Absyn_Deref_e_tag){
_LL564: _temp563=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp549)->f1; goto _LL554;} else{ goto _LL555;} _LL555: if((( struct
_tunion_struct*) _temp549)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL568:
_temp567=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp549)->f1; goto _LL566; _LL566: _temp565=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp549)->f2; goto _LL556;} else{ goto _LL557;}
_LL557: goto _LL558; _LL552: Cyc_Evexp_eval_const_uint_exp( _temp559) ==(
unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp569=( char*)"new_control_flow.cyc:450 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 61; _temp570;})); goto _LL554; _LL554: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp571=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp571->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp571->tl= 0; _temp571;}); goto _LL550; _LL556: env.access_path=({
struct Cyc_List_List* _temp572=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp572->hd=( void*) Cyc_Absyn_structmember_exp(
_temp567, _temp565, 0); _temp572->tl=( env.access_path)->tl; _temp572;}); ans.lvalues=({
struct Cyc_List_List* _temp573=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp573->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp573->tl= 0; _temp573;}); goto _LL550; _LL558:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp574=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp574[ 0]=({ struct Cyc_Core_Impossible_struct _temp575; _temp575.tag= Cyc_Core_Impossible_tag;
_temp575.f1=( struct _tagged_string)({ char* _temp576=( char*)"bad malloc access path";
struct _tagged_string _temp577; _temp577.curr= _temp576; _temp577.base= _temp576;
_temp577.last_plus_one= _temp576 + 23; _temp577;}); _temp575;}); _temp574;}));
_LL550:;} return ans;}}}} _LL531: goto _LL525; _LL525:;}{ struct Cyc_List_List*
_temp578= env.access_path; struct Cyc_List_List _temp590; struct Cyc_List_List*
_temp591; struct Cyc_Absyn_Exp* _temp593; struct Cyc_Absyn_Exp _temp595; struct
Cyc_Position_Segment* _temp596; void* _temp598; struct Cyc_Absyn_Exp* _temp600;
struct Cyc_Absyn_Exp* _temp602; struct Cyc_Core_Opt* _temp604; struct Cyc_List_List
_temp606; struct Cyc_List_List* _temp607; struct Cyc_Absyn_Exp* _temp609; struct
Cyc_Absyn_Exp _temp611; struct Cyc_Position_Segment* _temp612; void* _temp614;
struct Cyc_Absyn_Exp* _temp616; struct Cyc_Core_Opt* _temp618; struct Cyc_List_List
_temp620; struct Cyc_List_List* _temp621; struct Cyc_Absyn_Exp* _temp623; struct
Cyc_Absyn_Exp _temp625; struct Cyc_Position_Segment* _temp626; void* _temp628;
struct _tagged_string* _temp630; struct Cyc_Absyn_Exp* _temp632; struct Cyc_Core_Opt*
_temp634; _LL580: if( _temp578 == 0){ goto _LL581;} else{ goto _LL582;} _LL582:
if( _temp578 == 0){ goto _LL584;} else{ _temp590=* _temp578; _LL594: _temp593=(
struct Cyc_Absyn_Exp*) _temp590.hd; _temp595=* _temp593; _LL605: _temp604=(
struct Cyc_Core_Opt*) _temp595.topt; goto _LL599; _LL599: _temp598=( void*)
_temp595.r; if((( struct _tunion_struct*) _temp598)->tag == Cyc_Absyn_Subscript_e_tag){
_LL603: _temp602=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp598)->f1; goto _LL601; _LL601: _temp600=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp598)->f2; goto _LL597;} else{ goto _LL584;}
_LL597: _temp596=( struct Cyc_Position_Segment*) _temp595.loc; goto _LL592;
_LL592: _temp591=( struct Cyc_List_List*) _temp590.tl; goto _LL583;} _LL584: if(
_temp578 == 0){ goto _LL586;} else{ _temp606=* _temp578; _LL610: _temp609=(
struct Cyc_Absyn_Exp*) _temp606.hd; _temp611=* _temp609; _LL619: _temp618=(
struct Cyc_Core_Opt*) _temp611.topt; goto _LL615; _LL615: _temp614=( void*)
_temp611.r; if((( struct _tunion_struct*) _temp614)->tag == Cyc_Absyn_Deref_e_tag){
_LL617: _temp616=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp614)->f1; goto _LL613;} else{ goto _LL586;} _LL613: _temp612=( struct Cyc_Position_Segment*)
_temp611.loc; goto _LL608; _LL608: _temp607=( struct Cyc_List_List*) _temp606.tl;
goto _LL585;} _LL586: if( _temp578 == 0){ goto _LL588;} else{ _temp620=*
_temp578; _LL624: _temp623=( struct Cyc_Absyn_Exp*) _temp620.hd; _temp625=*
_temp623; _LL635: _temp634=( struct Cyc_Core_Opt*) _temp625.topt; goto _LL629;
_LL629: _temp628=( void*) _temp625.r; if((( struct _tunion_struct*) _temp628)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL633: _temp632=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp628)->f1; goto _LL631; _LL631:
_temp630=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp628)->f2; goto _LL627;} else{ goto _LL588;} _LL627: _temp626=( struct Cyc_Position_Segment*)
_temp625.loc; goto _LL622; _LL622: _temp621=( struct Cyc_List_List*) _temp620.tl;
goto _LL587;} _LL588: goto _LL589; _LL581: env.access_path=({ struct Cyc_List_List*
_temp636=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp636->hd=( void*) e; _temp636->tl= env.access_path; _temp636;}); goto _LL579;
_LL583: Cyc_Evexp_eval_const_uint_exp( _temp600) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp637=( char*)"new_control_flow.cyc:478 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp638; _temp638.curr= _temp637; _temp638.base= _temp637;
_temp638.last_plus_one= _temp637 + 61; _temp638;})); _temp607= _temp591; goto
_LL585; _LL585: env.access_path= _temp607; goto _LL579; _LL587: env.access_path=({
struct Cyc_List_List* _temp639=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp639->hd=( void*) Cyc_Absyn_structmember_exp(
_temp632, _temp630, 0); _temp639->tl= _temp621; _temp639;}); goto _LL579; _LL589:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp640=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp640[ 0]=({ struct Cyc_Core_Impossible_struct _temp641; _temp641.tag= Cyc_Core_Impossible_tag;
_temp641.f1=( struct _tagged_string)({ char* _temp642=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp643; _temp643.curr= _temp642; _temp643.base= _temp642;
_temp643.last_plus_one= _temp642 + 36; _temp643;}); _temp641;}); _temp640;}));
_LL579:;} return Cyc_NewControlFlow_abstract_exp( env, _temp315); _LL164: {
struct Cyc_List_List* _temp644= env.access_path; struct Cyc_List_List _temp650;
struct Cyc_List_List* _temp651; struct Cyc_Absyn_Exp* _temp653; struct Cyc_Absyn_Exp
_temp655; struct Cyc_Position_Segment* _temp656; void* _temp658; struct Cyc_Absyn_Exp*
_temp660; struct Cyc_Core_Opt* _temp662; _LL646: if( _temp644 == 0){ goto _LL648;}
else{ _temp650=* _temp644; _LL654: _temp653=( struct Cyc_Absyn_Exp*) _temp650.hd;
_temp655=* _temp653; _LL663: _temp662=( struct Cyc_Core_Opt*) _temp655.topt;
goto _LL659; _LL659: _temp658=( void*) _temp655.r; if((( struct _tunion_struct*)
_temp658)->tag == Cyc_Absyn_Address_e_tag){ _LL661: _temp660=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp658)->f1; goto _LL657;} else{ goto
_LL648;} _LL657: _temp656=( struct Cyc_Position_Segment*) _temp655.loc; goto
_LL652; _LL652: _temp651=( struct Cyc_List_List*) _temp650.tl; goto _LL647;}
_LL648: goto _LL649; _LL647: env.access_path= _temp651;{ struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp317); if( syn1.lvalues == 0){
syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return syn1;} _LL649: return
Cyc_NewControlFlow_descend_path( env, e, _temp317); _LL645:;} _LL166: { void*
_temp664= Cyc_Tcutil_compress(( void*)( _temp321->topt)->v); struct Cyc_Absyn_PtrInfo
_temp670; struct Cyc_Absyn_Conref* _temp672; struct Cyc_Absyn_Tqual* _temp674;
struct Cyc_Absyn_Conref* _temp676; void* _temp678; void* _temp680; _LL666: if((
unsigned int) _temp664 > 4u?(( struct _tunion_struct*) _temp664)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL671: _temp670=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp664)->f1; _LL681: _temp680=( void*) _temp670.elt_typ; goto _LL679; _LL679:
_temp678=( void*) _temp670.rgn_typ; goto _LL677; _LL677: _temp676=( struct Cyc_Absyn_Conref*)
_temp670.nullable; goto _LL675; _LL675: _temp674=( struct Cyc_Absyn_Tqual*)
_temp670.tq; goto _LL673; _LL673: _temp672=( struct Cyc_Absyn_Conref*) _temp670.bounds;
goto _LL667;} else{ goto _LL668;} _LL668: goto _LL669; _LL667:{ void* _temp682=
Cyc_Tcutil_compress( _temp680); struct Cyc_Absyn_Structdecl** _temp690; struct
Cyc_List_List* _temp692; struct _tuple1* _temp694; struct Cyc_Absyn_Uniondecl**
_temp696; struct Cyc_List_List* _temp698; struct _tuple1* _temp700; _LL684: if((
unsigned int) _temp682 > 4u?(( struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_StructType_tag:
0){ _LL695: _temp694=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp682)->f1; goto _LL693; _LL693: _temp692=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp682)->f2; goto _LL691; _LL691: _temp690=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp682)->f3;
goto _LL685;} else{ goto _LL686;} _LL686: if(( unsigned int) _temp682 > 4u?((
struct _tunion_struct*) _temp682)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL701:
_temp700=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp682)->f1;
goto _LL699; _LL699: _temp698=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp682)->f2; goto _LL697; _LL697: _temp696=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp682)->f3; goto _LL687;} else{ goto
_LL688;} _LL688: goto _LL689; _LL685: return Cyc_NewControlFlow_descend_path(
env, e, _temp321); _LL687: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp321); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL689: goto _LL683; _LL683:;} goto _LL669; _LL669:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp702=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp702[ 0]=({ struct
Cyc_Core_Impossible_struct _temp703; _temp703.tag= Cyc_Core_Impossible_tag;
_temp703.f1=( struct _tagged_string)({ char* _temp704=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp705; _temp705.curr= _temp704; _temp705.base= _temp704;
_temp705.last_plus_one= _temp704 + 40; _temp705;}); _temp703;}); _temp702;}));
_LL665:;} _LL168: { void* _temp706= Cyc_Tcutil_compress(( void*)( _temp325->topt)->v);
struct Cyc_Absyn_Structdecl** _temp714; struct Cyc_List_List* _temp716; struct
_tuple1* _temp718; struct Cyc_Absyn_Uniondecl** _temp720; struct Cyc_List_List*
_temp722; struct _tuple1* _temp724; _LL708: if(( unsigned int) _temp706 > 4u?((
struct _tunion_struct*) _temp706)->tag == Cyc_Absyn_StructType_tag: 0){ _LL719:
_temp718=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp706)->f1;
goto _LL717; _LL717: _temp716=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp706)->f2; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp706)->f3; goto _LL709;} else{ goto
_LL710;} _LL710: if(( unsigned int) _temp706 > 4u?(( struct _tunion_struct*)
_temp706)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL725: _temp724=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp706)->f1; goto _LL723; _LL723: _temp722=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp706)->f2; goto
_LL721; _LL721: _temp720=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp706)->f3; goto _LL711;} else{ goto _LL712;} _LL712: goto _LL713; _LL709:
return Cyc_NewControlFlow_descend_path( env, e, _temp325); _LL711: { struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp325); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL713:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp726=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp726[ 0]=({ struct Cyc_Core_Impossible_struct _temp727; _temp727.tag= Cyc_Core_Impossible_tag;
_temp727.f1=( struct _tagged_string)({ char* _temp728=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 41; _temp729;}); _temp727;}); _temp726;}));
_LL707:;} _LL170: { struct Cyc_List_List* _temp730= env.access_path; struct Cyc_List_List
_temp738; struct Cyc_List_List* _temp739; struct Cyc_Absyn_Exp* _temp741; struct
Cyc_Absyn_Exp _temp743; struct Cyc_Position_Segment* _temp744; void* _temp746;
struct Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Exp* _temp750; struct Cyc_Core_Opt*
_temp752; _LL732: if( _temp730 == 0){ goto _LL733;} else{ goto _LL734;} _LL734:
if( _temp730 == 0){ goto _LL736;} else{ _temp738=* _temp730; _LL742: _temp741=(
struct Cyc_Absyn_Exp*) _temp738.hd; _temp743=* _temp741; _LL753: _temp752=(
struct Cyc_Core_Opt*) _temp743.topt; goto _LL747; _LL747: _temp746=( void*)
_temp743.r; if((( struct _tunion_struct*) _temp746)->tag == Cyc_Absyn_Subscript_e_tag){
_LL751: _temp750=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp746)->f1; goto _LL749; _LL749: _temp748=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp746)->f2; goto _LL745;} else{ goto _LL736;}
_LL745: _temp744=( struct Cyc_Position_Segment*) _temp743.loc; goto _LL740;
_LL740: _temp739=( struct Cyc_List_List*) _temp738.tl; goto _LL735;} _LL736:
goto _LL737; _LL733: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp327 != 0; _temp327= _temp327->tl,
++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides;
for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*) ls->hd)){
ith_lvals=({ struct Cyc_List_List* _temp754=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp754->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp754->tl= ith_lvals; _temp754;});} else{ ith_lvals=({ struct Cyc_List_List*
_temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp755->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,( void*)({ struct
Cyc_CfFlowInfo_TupleF_struct* _temp756=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp756[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp757; _temp757.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp757.f1= j; _temp757;}); _temp756;})); _temp755->tl= ith_lvals; _temp755;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp327->hd); void* _temp760; void*
_temp762; struct Cyc_NewControlFlow_AbsSynOne _temp758= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL763: _temp762=( void*) _temp758.inner_exp; goto _LL761; _LL761:
_temp760=( void*) _temp758.assigns; goto _LL759; _LL759: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp762); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp760);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL735: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp748); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp739;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp327 != 0;
_temp327= _temp327->tl, ++ j){ if( i == j){ void* _temp766; void* _temp768;
struct Cyc_NewControlFlow_AbsSynOne _temp764= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*) _temp327->hd));
_LL769: _temp768=( void*) _temp764.inner_exp; goto _LL767; _LL767: _temp766=(
void*) _temp764.assigns; goto _LL765; _LL765: assignexp= _temp766; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp768);} else{ void* _temp772; void*
_temp774; struct Cyc_NewControlFlow_AbsSynOne _temp770= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*) _temp327->hd));
_LL775: _temp774=( void*) _temp770.inner_exp; goto _LL773; _LL773: _temp772=(
void*) _temp770.assigns; goto _LL771; _LL771: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp774);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp776=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp776->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp776->tl= 0; _temp776;}), assignexp);}} _LL737:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp777=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp777[ 0]=({ struct
Cyc_Core_Impossible_struct _temp778; _temp778.tag= Cyc_Core_Impossible_tag;
_temp778.f1=( struct _tagged_string)({ char* _temp779=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp780; _temp780.curr= _temp779; _temp780.base= _temp779;
_temp780.last_plus_one= _temp779 + 46; _temp780;}); _temp778;}); _temp777;}));
_LL731:;} _LL172: { struct Cyc_List_List* _temp781= env.access_path; struct Cyc_List_List
_temp789; struct Cyc_List_List* _temp790; struct Cyc_Absyn_Exp* _temp792; struct
Cyc_Absyn_Exp _temp794; struct Cyc_Position_Segment* _temp795; void* _temp797;
struct _tagged_string* _temp799; struct Cyc_Absyn_Exp* _temp801; struct Cyc_Core_Opt*
_temp803; _LL783: if( _temp781 == 0){ goto _LL784;} else{ goto _LL785;} _LL785:
if( _temp781 == 0){ goto _LL787;} else{ _temp789=* _temp781; _LL793: _temp792=(
struct Cyc_Absyn_Exp*) _temp789.hd; _temp794=* _temp792; _LL804: _temp803=(
struct Cyc_Core_Opt*) _temp794.topt; goto _LL798; _LL798: _temp797=( void*)
_temp794.r; if((( struct _tunion_struct*) _temp797)->tag == Cyc_Absyn_StructMember_e_tag){
_LL802: _temp801=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp797)->f1; goto _LL800; _LL800: _temp799=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp797)->f2; goto _LL796;} else{ goto _LL787;}
_LL796: _temp795=( struct Cyc_Position_Segment*) _temp794.loc; goto _LL791;
_LL791: _temp790=( struct Cyc_List_List*) _temp789.tl; goto _LL786;} _LL787:
goto _LL788; _LL784: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp331 != 0; _temp331= _temp331->tl){ struct
Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls
!= 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*) ls->hd)){ ith_lvals=({
struct Cyc_List_List* _temp805=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp805->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp805->tl= ith_lvals; _temp805;});} else{ struct Cyc_List_List* ds=(*((
struct _tuple5*) _temp331->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp806=(
void*) ds->hd; struct Cyc_Absyn_Exp* _temp812; struct _tagged_string* _temp814;
_LL808: if((( struct _tunion_struct*) _temp806)->tag == Cyc_Absyn_ArrayElement_tag){
_LL813: _temp812=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp806)->f1; goto _LL809;} else{ goto _LL810;} _LL810: if((( struct
_tunion_struct*) _temp806)->tag == Cyc_Absyn_FieldName_tag){ _LL815: _temp814=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp806)->f1;
goto _LL811;} else{ goto _LL807;} _LL809:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp816=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp816[ 0]=({ struct
Cyc_Core_Impossible_struct _temp817; _temp817.tag= Cyc_Core_Impossible_tag;
_temp817.f1=( struct _tagged_string)({ char* _temp818=( char*)"bad struct designator";
struct _tagged_string _temp819; _temp819.curr= _temp818; _temp819.base= _temp818;
_temp819.last_plus_one= _temp818 + 22; _temp819;}); _temp817;}); _temp816;}));
_LL811: ith_lvals=({ struct Cyc_List_List* _temp820=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp820->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp821=( struct
Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp821[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp822; _temp822.tag= Cyc_CfFlowInfo_StructF_tag;
_temp822.f1= _temp814; _temp822;}); _temp821;})); _temp820->tl= ith_lvals;
_temp820;}); goto _LL807; _LL807:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp331->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp331->hd)).f2); void* _temp825; void* _temp827; struct Cyc_NewControlFlow_AbsSynOne
_temp823= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL828: _temp827=( void*)
_temp823.inner_exp; goto _LL826; _LL826: _temp825=( void*) _temp823.assigns;
goto _LL824; _LL824: ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
ansexp, _temp827); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp825);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL786: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp790;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp331 != 0; _temp331=
_temp331->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp331->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp829=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp835; struct _tagged_string* _temp837; _LL831: if(((
struct _tunion_struct*) _temp829)->tag == Cyc_Absyn_ArrayElement_tag){ _LL836:
_temp835=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp829)->f1; goto _LL832;} else{ goto _LL833;} _LL833: if((( struct
_tunion_struct*) _temp829)->tag == Cyc_Absyn_FieldName_tag){ _LL838: _temp837=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp829)->f1;
goto _LL834;} else{ goto _LL830;} _LL832:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp839=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp839[ 0]=({ struct
Cyc_Core_Impossible_struct _temp840; _temp840.tag= Cyc_Core_Impossible_tag;
_temp840.f1=( struct _tagged_string)({ char* _temp841=( char*)"bad struct designator";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 22; _temp842;}); _temp840;}); _temp839;}));
_LL834: if( Cyc_String_zstrptrcmp( _temp799, _temp837) == 0){ used= 1;} goto
_LL830; _LL830:;}} if( used){ void* _temp845; void* _temp847; struct Cyc_NewControlFlow_AbsSynOne
_temp843= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*) _temp331->hd)).f2)); _LL848: _temp847=( void*) _temp843.inner_exp;
goto _LL846; _LL846: _temp845=( void*) _temp843.assigns; goto _LL844; _LL844:
assignexp= _temp845; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
ansexp, _temp847);} else{ void* _temp851; void* _temp853; struct Cyc_NewControlFlow_AbsSynOne
_temp849= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp331->hd)).f2)); _LL854: _temp853=( void*)
_temp849.inner_exp; goto _LL852; _LL852: _temp851=( void*) _temp849.assigns;
goto _LL850; _LL850: ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
ansexp, _temp853);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL788:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp855=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp855[ 0]=({ struct Cyc_Core_Impossible_struct _temp856; _temp856.tag= Cyc_Core_Impossible_tag;
_temp856.f1=( struct _tagged_string)({ char* _temp857=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp858; _temp858.curr= _temp857; _temp858.base= _temp857;
_temp858.last_plus_one= _temp857 + 47; _temp858;}); _temp856;}); _temp855;}));
_LL782:;} _LL174: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp337), 0); _LL176:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp859=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp859->hd=( void*) _temp341; _temp859->tl= 0; _temp859;}), 0); _LL178: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp343); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL180: { struct Cyc_List_List*
_temp862; struct Cyc_NewControlFlow_AbsSynOne _temp864; struct Cyc_NewControlFlow_AbsSynOne
_temp866; struct Cyc_NewControlFlow_AbsSyn _temp860= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp349); _LL867: _temp866=( struct Cyc_NewControlFlow_AbsSynOne)
_temp860.when_true; goto _LL865; _LL865: _temp864=( struct Cyc_NewControlFlow_AbsSynOne)
_temp860.when_false; goto _LL863; _LL863: _temp862=( struct Cyc_List_List*)
_temp860.lvalues; goto _LL861; _LL861: { struct Cyc_List_List* _temp870; struct
Cyc_NewControlFlow_AbsSynOne _temp872; struct Cyc_NewControlFlow_AbsSynOne
_temp874; struct Cyc_NewControlFlow_AbsSyn _temp868= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp347); _LL875: _temp874=( struct Cyc_NewControlFlow_AbsSynOne)
_temp868.when_true; goto _LL873; _LL873: _temp872=( struct Cyc_NewControlFlow_AbsSynOne)
_temp868.when_false; goto _LL871; _LL871: _temp870=( struct Cyc_List_List*)
_temp868.lvalues; goto _LL869; _LL869: { struct Cyc_List_List* _temp878; struct
Cyc_NewControlFlow_AbsSynOne _temp880; struct Cyc_NewControlFlow_AbsSynOne
_temp882; struct Cyc_NewControlFlow_AbsSyn _temp876= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp345); _LL883: _temp882=( struct Cyc_NewControlFlow_AbsSynOne)
_temp876.when_true; goto _LL881; _LL881: _temp880=( struct Cyc_NewControlFlow_AbsSynOne)
_temp876.when_false; goto _LL879; _LL879: _temp878=( struct Cyc_List_List*)
_temp876.lvalues; goto _LL877; _LL877: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp866.inner_exp,(
void*) _temp866.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp874.inner_exp,( void*) _temp874.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp864.inner_exp,( void*) _temp864.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp882.inner_exp,( void*) _temp882.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp866.inner_exp,( void*) _temp866.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp872.inner_exp,( void*) _temp872.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp864.inner_exp,( void*) _temp864.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp880.inner_exp,(
void*) _temp880.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp884;
_temp884.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp886; _temp886.inner_exp=(
void*) true_exp; _temp886.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp886;}); _temp884.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp885; _temp885.inner_exp=( void*) false_exp; _temp885.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp885;}); _temp884.lvalues= 0; _temp884;});}}}}
_LL182: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp351);{
struct Cyc_Absyn_Stmt* last_s= _temp351; while( 1) { void* _temp887=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp897; struct Cyc_Absyn_Stmt* _temp899;
struct Cyc_Absyn_Stmt* _temp901; struct Cyc_Absyn_Decl* _temp903; struct Cyc_Absyn_Exp*
_temp905; _LL889: if(( unsigned int) _temp887 > 1u?(( struct _tunion_struct*)
_temp887)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL900: _temp899=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp887)->f1; goto _LL898; _LL898: _temp897=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp887)->f2; goto
_LL890;} else{ goto _LL891;} _LL891: if(( unsigned int) _temp887 > 1u?(( struct
_tunion_struct*) _temp887)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL904: _temp903=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp887)->f1; goto
_LL902; _LL902: _temp901=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp887)->f2; goto _LL892;} else{ goto _LL893;} _LL893: if(( unsigned int)
_temp887 > 1u?(( struct _tunion_struct*) _temp887)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL906: _temp905=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp887)->f1; goto _LL894;} else{ goto _LL895;} _LL895: goto _LL896; _LL890:
last_s= _temp897; continue; _LL892: last_s= _temp901; continue; _LL894: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp905); void* _temp909; void* _temp911; struct Cyc_NewControlFlow_AbsSynOne
_temp907= Cyc_NewControlFlow_meet_absexp( syn); _LL912: _temp911=( void*)
_temp907.inner_exp; goto _LL910; _LL910: _temp909=( void*) _temp907.assigns;
goto _LL908; _LL908:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp911, _temp909));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp351), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL896:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp913=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp913[ 0]=({ struct
Cyc_Core_Impossible_struct _temp914; _temp914.tag= Cyc_Core_Impossible_tag;
_temp914.f1=( struct _tagged_string)({ char* _temp915=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp916; _temp916.curr= _temp915; _temp916.base= _temp915;
_temp916.last_plus_one= _temp915 + 33; _temp916;}); _temp914;}); _temp913;}));
_LL888:;}}} _LL184: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp355); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp353); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp917; _temp917.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp919; _temp919.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp919.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp919;});
_temp917.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp918; _temp918.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp918.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp918;}); _temp917.lvalues= 0; _temp917;});}} _LL186: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp920=( char*)"new_control_flow.cyc:749 env.access_path==null";
struct _tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 47; _temp921;})); Cyc_NewControlFlow_add_var_root(
env, _temp361);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp922=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp922->hd=( void*) _temp361; _temp922->tl=( env.shared)->comprehension_vars;
_temp922;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp925; void* _temp927; struct Cyc_NewControlFlow_AbsSynOne
_temp923= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp359)); _LL928: _temp927=( void*) _temp923.inner_exp; goto _LL926;
_LL926: _temp925=( void*) _temp923.assigns; goto _LL924; _LL924: { void*
_temp931; void* _temp933; struct Cyc_NewControlFlow_AbsSynOne _temp929= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp357)); _LL934: _temp933=( void*)
_temp929.inner_exp; goto _LL932; _LL932: _temp931=( void*) _temp929.assigns;
goto _LL930; _LL930: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp927, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp925, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp361), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp933, _temp931), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL188: goto _LL190; _LL190: goto _LL192;
_LL192: goto _LL194; _LL194: goto _LL196; _LL196: goto _LL198; _LL198: goto
_LL200; _LL200:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp935=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp935[ 0]=({ struct Cyc_Core_Impossible_struct _temp936; _temp936.tag= Cyc_Core_Impossible_tag;
_temp936.f1=( struct _tagged_string)({ char* _temp937=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp938; _temp938.curr= _temp937; _temp938.base= _temp937;
_temp938.last_plus_one= _temp937 + 34; _temp938;}); _temp936;}); _temp935;}));
_LL112:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp941; void* _temp943; struct Cyc_NewControlFlow_AbsSynOne
_temp939= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL944: _temp943=( void*) _temp939.inner_exp; goto _LL942; _LL942: _temp941=(
void*) _temp939.assigns; goto _LL940; _LL940: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp943, _temp941);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp945; _temp945.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp945.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp945;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp948;
struct Cyc_Position_Segment* _temp949; struct Cyc_Absyn_Stmt* _temp951; struct
Cyc_Absyn_Exp* _temp953; struct Cyc_Core_Opt* _temp955; struct Cyc_Absyn_Pat*
_temp957; struct Cyc_Absyn_Switch_clause* _temp946=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp948=* _temp946; _LL958: _temp957=( struct Cyc_Absyn_Pat*) _temp948.pattern;
goto _LL956; _LL956: _temp955=( struct Cyc_Core_Opt*) _temp948.pat_vars; goto
_LL954; _LL954: _temp953=( struct Cyc_Absyn_Exp*) _temp948.where_clause; goto
_LL952; _LL952: _temp951=( struct Cyc_Absyn_Stmt*) _temp948.body; goto _LL950;
_LL950: _temp949=( struct Cyc_Position_Segment*) _temp948.loc; goto _LL947;
_LL947: if( _temp955 == 0){( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp959=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp959[ 0]=({ struct
Cyc_Core_Impossible_struct _temp960; _temp960.tag= Cyc_Core_Impossible_tag;
_temp960.f1=( struct _tagged_string)({ char* _temp961=( char*)"switch clause vds not set";
struct _tagged_string _temp962; _temp962.curr= _temp961; _temp962.base= _temp961;
_temp962.last_plus_one= _temp961 + 26; _temp962;}); _temp960;}); _temp959;}));}{
struct Cyc_List_List* vds=( struct Cyc_List_List*) _temp955->v; for( 0; vds != 0;
vds= vds->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)
vds->hd);}} if( _temp953 != 0){ void* _temp966; void* _temp968; struct _tuple0
_temp964= Cyc_NewControlFlow_abstract_guard( env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp963= _temp953; if( _temp963 == 0){ _throw(
Null_Exception);} _temp963;})); _LL969: _temp968= _temp964.f1; goto _LL967;
_LL967: _temp966= _temp964.f2; goto _LL965; _LL965: ans=({ struct Cyc_List_List*
_temp970=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp970->hd=( void*) _temp968; _temp970->tl=({ struct Cyc_List_List* _temp971=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp971->hd=(
void*) _temp966; _temp971->tl= ans; _temp971;}); _temp970;});} Cyc_NewControlFlow_cf_prepass(
env, _temp951);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp972=( char*)"new_control_flow.cyc:801 env.lhsides == null"; struct
_tagged_string _temp973; _temp973.curr= _temp972; _temp973.base= _temp972;
_temp973.last_plus_one= _temp972 + 45; _temp973;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp974=( char*)"new_control_flow.cyc:802 env.access_path == null";
struct _tagged_string _temp975; _temp975.curr= _temp974; _temp975.base= _temp974;
_temp975.last_plus_one= _temp974 + 49; _temp975;}));{ void* _temp976=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1022; struct Cyc_Absyn_Stmt* _temp1024; struct Cyc_Absyn_Stmt*
_temp1026; struct Cyc_Absyn_Exp* _temp1028; struct Cyc_Absyn_Exp* _temp1030;
struct Cyc_Absyn_Stmt* _temp1032; struct Cyc_Absyn_Stmt* _temp1034; struct Cyc_Absyn_Exp*
_temp1036; struct _tuple3 _temp1038; struct Cyc_Absyn_Stmt* _temp1040; struct
Cyc_Absyn_Exp* _temp1042; struct Cyc_Absyn_Stmt* _temp1044; struct Cyc_Absyn_Stmt*
_temp1046; struct _tuple3 _temp1048; struct Cyc_Absyn_Stmt* _temp1050; struct
Cyc_Absyn_Exp* _temp1052; struct Cyc_Absyn_Stmt* _temp1054; struct Cyc_Absyn_Stmt*
_temp1056; struct Cyc_Absyn_Stmt* _temp1058; struct _tagged_string* _temp1060;
struct Cyc_Absyn_Stmt* _temp1062; struct _tagged_string* _temp1064; struct Cyc_Absyn_Stmt*
_temp1066; struct _tuple3 _temp1068; struct Cyc_Absyn_Stmt* _temp1070; struct
Cyc_Absyn_Exp* _temp1072; struct _tuple3 _temp1074; struct Cyc_Absyn_Stmt*
_temp1076; struct Cyc_Absyn_Exp* _temp1078; struct Cyc_Absyn_Exp* _temp1080;
struct Cyc_List_List* _temp1082; struct Cyc_Absyn_Exp* _temp1084; struct Cyc_Absyn_Switch_clause**
_temp1086; struct Cyc_List_List* _temp1088; struct Cyc_Absyn_Stmt* _temp1090;
struct Cyc_Absyn_Decl* _temp1092; struct Cyc_Absyn_Decl _temp1094; struct Cyc_Position_Segment*
_temp1095; void* _temp1097; struct Cyc_Absyn_Vardecl* _temp1099; struct Cyc_Absyn_Stmt*
_temp1101; struct Cyc_Absyn_Decl* _temp1103; struct Cyc_Absyn_Decl _temp1105;
struct Cyc_Position_Segment* _temp1106; void* _temp1108; int _temp1110; struct
Cyc_Absyn_Exp* _temp1112; struct Cyc_Core_Opt* _temp1114; struct Cyc_Core_Opt*
_temp1116; struct Cyc_Core_Opt _temp1118; struct Cyc_List_List* _temp1119;
struct Cyc_Absyn_Pat* _temp1121; struct Cyc_Absyn_Stmt* _temp1123; struct Cyc_Absyn_Decl*
_temp1125; struct Cyc_Absyn_Stmt* _temp1127; struct Cyc_Absyn_Stmt* _temp1129;
struct Cyc_Absyn_Stmt* _temp1131; struct _tagged_string* _temp1133; struct Cyc_List_List*
_temp1135; struct Cyc_Absyn_Stmt* _temp1137; _LL978: if(( int) _temp976 == Cyc_Absyn_Skip_s){
goto _LL979;} else{ goto _LL980;} _LL980: if(( unsigned int) _temp976 > 1u?((
struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1023:
_temp1022=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp976)->f1;
goto _LL981;} else{ goto _LL982;} _LL982: if(( unsigned int) _temp976 > 1u?((
struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1027:
_temp1026=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp976)->f1;
goto _LL1025; _LL1025: _temp1024=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp976)->f2; goto _LL983;} else{ goto _LL984;} _LL984: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1029: _temp1028=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp976)->f1; if( _temp1028 == 0){ goto _LL985;} else{ goto _LL986;}} else{
goto _LL986;} _LL986: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*)
_temp976)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1031: _temp1030=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp976)->f1; goto _LL987;} else{ goto
_LL988;} _LL988: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*)
_temp976)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1037: _temp1036=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp976)->f1; goto
_LL1035; _LL1035: _temp1034=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp976)->f2; goto _LL1033; _LL1033: _temp1032=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp976)->f3; goto _LL989;} else{ goto
_LL990;} _LL990: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*)
_temp976)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1045: _temp1044=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp976)->f1; goto _LL1039; _LL1039: _temp1038=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp976)->f2; _LL1043:
_temp1042= _temp1038.f1; goto _LL1041; _LL1041: _temp1040= _temp1038.f2; goto
_LL991;} else{ goto _LL992;} _LL992: if(( unsigned int) _temp976 > 1u?(( struct
_tunion_struct*) _temp976)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1049: _temp1048=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp976)->f1; _LL1053:
_temp1052= _temp1048.f1; goto _LL1051; _LL1051: _temp1050= _temp1048.f2; goto
_LL1047; _LL1047: _temp1046=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp976)->f2; goto _LL993;} else{ goto _LL994;} _LL994: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL1055: _temp1054=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp976)->f1; goto _LL995;} else{ goto _LL996;} _LL996: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1057: _temp1056=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp976)->f1; goto _LL997;} else{ goto _LL998;} _LL998: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1061: _temp1060=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp976)->f1; goto _LL1059; _LL1059: _temp1058=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp976)->f2; if( _temp1058 == 0){ goto _LL999;}
else{ goto _LL1000;}} else{ goto _LL1000;} _LL1000: if(( unsigned int) _temp976
> 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Goto_s_tag: 0){
_LL1065: _temp1064=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp976)->f1; goto _LL1063; _LL1063: _temp1062=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp976)->f2; goto _LL1001;} else{ goto
_LL1002;} _LL1002: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*)
_temp976)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1081: _temp1080=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp976)->f1; goto _LL1075; _LL1075: _temp1074=(
struct _tuple3)(( struct Cyc_Absyn_For_s_struct*) _temp976)->f2; _LL1079:
_temp1078= _temp1074.f1; goto _LL1077; _LL1077: _temp1076= _temp1074.f2; goto
_LL1069; _LL1069: _temp1068=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp976)->f3; _LL1073: _temp1072= _temp1068.f1; goto _LL1071; _LL1071:
_temp1070= _temp1068.f2; goto _LL1067; _LL1067: _temp1066=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_For_s_struct*) _temp976)->f4; goto _LL1003;} else{ goto _LL1004;}
_LL1004: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag
== Cyc_Absyn_Switch_s_tag: 0){ _LL1085: _temp1084=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp976)->f1; goto _LL1083; _LL1083:
_temp1082=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp976)->f2;
goto _LL1005;} else{ goto _LL1006;} _LL1006: if(( unsigned int) _temp976 > 1u?((
struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1089:
_temp1088=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp976)->f1; goto _LL1087; _LL1087: _temp1086=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp976)->f2; goto _LL1007;} else{ goto
_LL1008;} _LL1008: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*)
_temp976)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1093: _temp1092=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp976)->f1; _temp1094=* _temp1092; _LL1098:
_temp1097=( void*) _temp1094.r; if((( struct _tunion_struct*) _temp1097)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1100: _temp1099=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1097)->f1; goto _LL1096;} else{ goto _LL1010;}
_LL1096: _temp1095=( struct Cyc_Position_Segment*) _temp1094.loc; goto _LL1091;
_LL1091: _temp1090=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp976)->f2; goto _LL1009;} else{ goto _LL1010;} _LL1010: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1104: _temp1103=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp976)->f1; _temp1105=* _temp1103; _LL1109: _temp1108=( void*) _temp1105.r;
if((( struct _tunion_struct*) _temp1108)->tag == Cyc_Absyn_Let_d_tag){ _LL1122:
_temp1121=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1108)->f1;
goto _LL1117; _LL1117: _temp1116=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1108)->f2; if( _temp1116 == 0){ goto _LL1012;} else{ _temp1118=* _temp1116;
_LL1120: _temp1119=( struct Cyc_List_List*) _temp1118.v; goto _LL1115;} _LL1115:
_temp1114=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1108)->f3;
goto _LL1113; _LL1113: _temp1112=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1108)->f4; goto _LL1111; _LL1111: _temp1110=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1108)->f5; goto _LL1107;} else{ goto _LL1012;} _LL1107: _temp1106=( struct
Cyc_Position_Segment*) _temp1105.loc; goto _LL1102; _LL1102: _temp1101=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp976)->f2; goto _LL1011;}
else{ goto _LL1012;} _LL1012: if(( unsigned int) _temp976 > 1u?(( struct
_tunion_struct*) _temp976)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1126: _temp1125=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp976)->f1; goto
_LL1124; _LL1124: _temp1123=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp976)->f2; goto _LL1013;} else{ goto _LL1014;} _LL1014: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL1128: _temp1127=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp976)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1130: _temp1129=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp976)->f1; goto _LL1017;} else{ goto _LL1018;} _LL1018: if(( unsigned int)
_temp976 > 1u?(( struct _tunion_struct*) _temp976)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1134: _temp1133=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp976)->f1; goto _LL1132; _LL1132: _temp1131=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp976)->f2; goto _LL1019;} else{ goto
_LL1020;} _LL1020: if(( unsigned int) _temp976 > 1u?(( struct _tunion_struct*)
_temp976)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1138: _temp1137=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp976)->f1; goto _LL1136; _LL1136:
_temp1135=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp976)->f2; goto _LL1021;} else{ goto _LL977;} _LL979: return; _LL981: { void*
ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1022); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1139=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1139->hd=( void*) ae; _temp1139->tl= 0; _temp1139;}));
return;} _LL983: Cyc_NewControlFlow_cf_prepass( env, _temp1026); Cyc_NewControlFlow_cf_prepass(
env, _temp1024); return; _LL985: return; _LL987: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1140= _temp1030; if( _temp1140 == 0){ _throw( Null_Exception);} _temp1140;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1141=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1141->hd=( void*)
ae; _temp1141->tl= 0; _temp1141;})); return;} _LL989: { void* _temp1144; void*
_temp1146; struct _tuple0 _temp1142= Cyc_NewControlFlow_abstract_guard( env,
_temp1036); _LL1147: _temp1146= _temp1142.f1; goto _LL1145; _LL1145: _temp1144=
_temp1142.f2; goto _LL1143; _LL1143: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1148=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1148->hd=( void*) _temp1146; _temp1148->tl=({
struct Cyc_List_List* _temp1149=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1149->hd=( void*) _temp1144; _temp1149->tl= 0;
_temp1149;}); _temp1148;})); Cyc_NewControlFlow_cf_prepass( env, _temp1034); Cyc_NewControlFlow_cf_prepass(
env, _temp1032); return;} _LL991: _temp1052= _temp1042; _temp1050= _temp1040;
_temp1046= _temp1044; goto _LL993; _LL993: { void* _temp1152; void* _temp1154;
struct _tuple0 _temp1150= Cyc_NewControlFlow_abstract_guard( env, _temp1052);
_LL1155: _temp1154= _temp1150.f1; goto _LL1153; _LL1153: _temp1152= _temp1150.f2;
goto _LL1151; _LL1151: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1156=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1156->hd=( void*) _temp1154; _temp1156->tl=({ struct Cyc_List_List*
_temp1157=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1157->hd=( void*) _temp1152; _temp1157->tl= 0; _temp1157;}); _temp1156;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1046); return;} _LL995: return; _LL997:
return; _LL999:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1158=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1158[ 0]=({ struct Cyc_Core_Impossible_struct _temp1159; _temp1159.tag= Cyc_Core_Impossible_tag;
_temp1159.f1=( struct _tagged_string)({ char* _temp1160=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1161; _temp1161.curr= _temp1160; _temp1161.base=
_temp1160; _temp1161.last_plus_one= _temp1160 + 33; _temp1161;}); _temp1159;});
_temp1158;})); _LL1001: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1162= _temp1062; if(
_temp1162 == 0){ _throw( Null_Exception);} _temp1162;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1163=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1164; _temp1164.curr= _temp1163; _temp1164.base=
_temp1163; _temp1164.last_plus_one= _temp1163 + 45; _temp1164;})); break;}
my_encloser= next_encloser;} return;} _LL1003: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1080); void* _temp1167; void* _temp1169; struct _tuple0 _temp1165= Cyc_NewControlFlow_abstract_guard(
env, _temp1078); _LL1170: _temp1169= _temp1165.f1; goto _LL1168; _LL1168:
_temp1167= _temp1165.f2; goto _LL1166; _LL1166: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1072); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1171=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1171->hd=( void*) ae1; _temp1171->tl=({ struct Cyc_List_List* _temp1172=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1172->hd=(
void*) _temp1169; _temp1172->tl=({ struct Cyc_List_List* _temp1173=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1173->hd=( void*) _temp1167;
_temp1173->tl=({ struct Cyc_List_List* _temp1174=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1174->hd=( void*) ae3; _temp1174->tl=
0; _temp1174;}); _temp1173;}); _temp1172;}); _temp1171;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1066); return;}} _LL1005: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1084); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1082); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1175=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1175->hd=( void*) ae; _temp1175->tl= ael; _temp1175;})); return;} _LL1007: {
void* _temp1178; void* _temp1180; struct Cyc_NewControlFlow_AbsSynOne _temp1176=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1088, 0)); _LL1181: _temp1180=( void*) _temp1176.inner_exp; goto _LL1179;
_LL1179: _temp1178=( void*) _temp1176.assigns; goto _LL1177; _LL1177: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1180, _temp1178);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1182=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1182->hd=( void*)
ae; _temp1182->tl= 0; _temp1182;})); return;}} _LL1009: Cyc_NewControlFlow_add_var_root(
env, _temp1099); if( _temp1099->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1183=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1183->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1099); _temp1183->tl= 0; _temp1183;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1184= _temp1099->initializer; if( _temp1184 == 0){
_throw( Null_Exception);} _temp1184;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1185=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1185->hd=( void*) ae; _temp1185->tl= 0; _temp1185;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1090); return; _LL1011: for( 0;
_temp1119 != 0; _temp1119= _temp1119->tl){ Cyc_NewControlFlow_add_var_root( env,(
struct Cyc_Absyn_Vardecl*) _temp1119->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1112); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1186=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1186->hd=( void*) ae; _temp1186->tl= 0; _temp1186;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1101); return;} _LL1013: goto _LL1015;
_LL1015: goto _LL1017; _LL1017:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1187=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1187[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1188; _temp1188.tag= Cyc_Core_Impossible_tag;
_temp1188.f1=( struct _tagged_string)({ char* _temp1189=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1190; _temp1190.curr= _temp1189; _temp1190.base=
_temp1189; _temp1190.last_plus_one= _temp1189 + 26; _temp1190;}); _temp1188;});
_temp1187;})); _LL1019: Cyc_NewControlFlow_cf_prepass( env, _temp1131); return;
_LL1021: Cyc_NewControlFlow_cf_prepass( env, _temp1137);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1135); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL977:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1191=( char*)"new_control_flow.cyc:930 annot->visited == iteration_num";
struct _tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base=
_temp1191; _temp1192.last_plus_one= _temp1191 + 57; _temp1192;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1193= flow; struct Cyc_Dict_Dict* _temp1199; _LL1195: if(( int)
_temp1193 == Cyc_CfFlowInfo_BottomFL){ goto _LL1196;} else{ goto _LL1197;}
_LL1197: if(( unsigned int) _temp1193 > 1u?(( struct _tunion_struct*) _temp1193)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL1200: _temp1199=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1193)->f1; goto _LL1198;} else{ goto
_LL1194;} _LL1196: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1198: for( 0; vds
!= 0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1201=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1201[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1202; _temp1202.tag=
Cyc_CfFlowInfo_VarRoot_tag; _temp1202.f1=( struct Cyc_Absyn_Vardecl*) vds->hd;
_temp1202;}); _temp1201;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) root)); _temp1199=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1199,(
void*) root, pinfo);} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1203=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1203[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1204; _temp1204.tag=
Cyc_CfFlowInfo_InitsFL_tag; _temp1204.f1= _temp1199; _temp1204;}); _temp1203;});
_LL1194:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1205= flow; struct Cyc_Dict_Dict*
_temp1211; _LL1207: if(( int) _temp1205 == Cyc_CfFlowInfo_BottomFL){ goto
_LL1208;} else{ goto _LL1209;} _LL1209: if(( unsigned int) _temp1205 > 1u?((
struct _tunion_struct*) _temp1205)->tag == Cyc_CfFlowInfo_InitsFL_tag: 0){
_LL1212: _temp1211=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1205)->f1; goto _LL1210;} else{ goto _LL1206;} _LL1208: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1210: for( 0; vds != 0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
root=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1213=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1213[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1214; _temp1214.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp1214.f1=( struct Cyc_Absyn_Vardecl*) vds->hd; _temp1214;}); _temp1213;});
_temp1211=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( _temp1211,( void*) root,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1215=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1215[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1216; _temp1216.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1216.f1= _temp1211; _temp1216;}); _temp1215;}); _LL1206:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*) aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1217;
_temp1217.f1= out_flow; _temp1217.f2= out_flow; _temp1217;});} return({ struct
_tuple0 _temp1218; _temp1218.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1218.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1218;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1221; struct Cyc_Position_Segment* _temp1222; struct Cyc_Absyn_Stmt*
_temp1224; struct Cyc_Absyn_Exp* _temp1226; struct Cyc_Core_Opt* _temp1228;
struct Cyc_Absyn_Pat* _temp1230; struct Cyc_Absyn_Switch_clause* _temp1219=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1221=* _temp1219; _LL1231:
_temp1230=( struct Cyc_Absyn_Pat*) _temp1221.pattern; goto _LL1229; _LL1229:
_temp1228=( struct Cyc_Core_Opt*) _temp1221.pat_vars; goto _LL1227; _LL1227:
_temp1226=( struct Cyc_Absyn_Exp*) _temp1221.where_clause; goto _LL1225; _LL1225:
_temp1224=( struct Cyc_Absyn_Stmt*) _temp1221.body; goto _LL1223; _LL1223:
_temp1222=( struct Cyc_Position_Segment*) _temp1221.loc; goto _LL1220; _LL1220: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1228->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1228->v);{ void* body_outflow;
if( _temp1226 != 0){ void* _temp1235; void* _temp1237; struct _tuple0 _temp1233=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1232= _temp1226; if( _temp1232 == 0){ _throw( Null_Exception);} _temp1232;}))->loc,
aes, body_inflow); _LL1238: _temp1237= _temp1233.f1; goto _LL1236; _LL1236:
_temp1235= _temp1233.f2; goto _LL1234; _LL1234: aes=( aes->tl)->tl; in_flow=
_temp1235; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1224,
_temp1237);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1224, body_inflow);}{ void* _temp1239= body_outflow; _LL1241: if(( int)
_temp1239 == Cyc_CfFlowInfo_BottomFL){ goto _LL1242;} else{ goto _LL1243;}
_LL1243: goto _LL1244; _LL1242: goto _LL1240; _LL1244: Cyc_Tcutil_terr(
_temp1224->loc,( struct _tagged_string)({ char* _temp1245=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base=
_temp1245; _temp1246.last_plus_one= _temp1245 + 38; _temp1246;})); goto _LL1240;
_LL1240:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1247=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1289; struct Cyc_Absyn_Exp* _temp1291; struct Cyc_Absyn_Exp*
_temp1293; struct Cyc_Absyn_Stmt* _temp1295; struct Cyc_Absyn_Stmt* _temp1297;
struct Cyc_Absyn_Stmt* _temp1299; struct Cyc_Absyn_Stmt* _temp1301; struct Cyc_Absyn_Exp*
_temp1303; struct Cyc_Absyn_Stmt* _temp1305; struct _tuple3 _temp1307; struct
Cyc_Absyn_Stmt* _temp1309; struct Cyc_Absyn_Exp* _temp1311; struct _tuple3
_temp1313; struct Cyc_Absyn_Stmt* _temp1315; struct Cyc_Absyn_Exp* _temp1317;
struct Cyc_Absyn_Stmt* _temp1319; struct Cyc_Absyn_Stmt* _temp1321; struct
_tuple3 _temp1323; struct Cyc_Absyn_Stmt* _temp1325; struct Cyc_Absyn_Exp*
_temp1327; struct _tuple3 _temp1329; struct Cyc_Absyn_Stmt* _temp1331; struct
Cyc_Absyn_Exp* _temp1333; struct Cyc_Absyn_Exp* _temp1335; struct Cyc_Absyn_Stmt*
_temp1337; struct Cyc_Absyn_Switch_clause** _temp1339; struct Cyc_Absyn_Switch_clause*
_temp1341; struct Cyc_List_List* _temp1342; struct Cyc_Absyn_Stmt* _temp1344;
struct Cyc_Absyn_Stmt* _temp1346; struct Cyc_Absyn_Stmt* _temp1348; struct
_tagged_string* _temp1350; struct Cyc_List_List* _temp1352; struct Cyc_Absyn_Exp*
_temp1354; struct Cyc_List_List* _temp1356; struct Cyc_Absyn_Stmt* _temp1358;
struct Cyc_Absyn_Stmt* _temp1360; struct Cyc_Absyn_Decl* _temp1362; struct Cyc_Absyn_Decl
_temp1364; struct Cyc_Position_Segment* _temp1365; void* _temp1367; struct Cyc_Absyn_Vardecl*
_temp1369; struct Cyc_Absyn_Stmt* _temp1371; struct Cyc_Absyn_Decl* _temp1373;
struct Cyc_Absyn_Decl _temp1375; struct Cyc_Position_Segment* _temp1376; void*
_temp1378; int _temp1380; struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Core_Opt*
_temp1384; struct Cyc_Core_Opt* _temp1386; struct Cyc_Core_Opt _temp1388; struct
Cyc_List_List* _temp1389; struct Cyc_Absyn_Pat* _temp1391; struct Cyc_Absyn_Stmt*
_temp1393; struct _tagged_string* _temp1395; _LL1249: if(( int) _temp1247 == Cyc_Absyn_Skip_s){
goto _LL1250;} else{ goto _LL1251;} _LL1251: if(( unsigned int) _temp1247 > 1u?((
struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1290:
_temp1289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1247)->f1; if( _temp1289 == 0){ goto _LL1252;} else{ goto _LL1253;}} else{
goto _LL1253;} _LL1253: if(( unsigned int) _temp1247 > 1u?(( struct
_tunion_struct*) _temp1247)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1292:
_temp1291=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1247)->f1; goto _LL1254;} else{ goto _LL1255;} _LL1255: if(( unsigned int)
_temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1294: _temp1293=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1247)->f1; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(( unsigned int)
_temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1298: _temp1297=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1247)->f1; goto _LL1296; _LL1296: _temp1295=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1247)->f2; goto _LL1258;} else{ goto
_LL1259;} _LL1259: if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*)
_temp1247)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1304: _temp1303=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1247)->f1; goto
_LL1302; _LL1302: _temp1301=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1247)->f2; goto _LL1300; _LL1300: _temp1299=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1247)->f3; goto _LL1260;} else{ goto
_LL1261;} _LL1261: if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*)
_temp1247)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1308: _temp1307=( struct
_tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1247)->f1; _LL1312: _temp1311=
_temp1307.f1; goto _LL1310; _LL1310: _temp1309= _temp1307.f2; goto _LL1306;
_LL1306: _temp1305=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1247)->f2; goto _LL1262;} else{ goto _LL1263;} _LL1263: if(( unsigned int)
_temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1320: _temp1319=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1247)->f1; goto _LL1314; _LL1314: _temp1313=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1247)->f2; _LL1318: _temp1317= _temp1313.f1; goto _LL1316; _LL1316:
_temp1315= _temp1313.f2; goto _LL1264;} else{ goto _LL1265;} _LL1265: if((
unsigned int) _temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1336: _temp1335=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1247)->f1; goto _LL1330; _LL1330: _temp1329=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1247)->f2; _LL1334: _temp1333= _temp1329.f1; goto _LL1332; _LL1332:
_temp1331= _temp1329.f2; goto _LL1324; _LL1324: _temp1323=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1247)->f3; _LL1328: _temp1327= _temp1323.f1;
goto _LL1326; _LL1326: _temp1325= _temp1323.f2; goto _LL1322; _LL1322: _temp1321=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1247)->f4; goto
_LL1266;} else{ goto _LL1267;} _LL1267: if(( unsigned int) _temp1247 > 1u?((
struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1338:
_temp1337=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1247)->f1; if( _temp1337 == 0){ goto _LL1268;} else{ goto _LL1269;}} else{
goto _LL1269;} _LL1269: if(( unsigned int) _temp1247 > 1u?(( struct
_tunion_struct*) _temp1247)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1343:
_temp1342=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1247)->f1; goto _LL1340; _LL1340: _temp1339=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1247)->f2; if( _temp1339 == 0){ goto
_LL1271;} else{ _temp1341=* _temp1339; goto _LL1270;}} else{ goto _LL1271;}
_LL1271: if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL1345: _temp1344=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1247)->f1; goto _LL1272;} else{ goto
_LL1273;} _LL1273: if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*)
_temp1247)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1347: _temp1346=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1247)->f1; goto _LL1274;} else{ goto
_LL1275;} _LL1275: if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*)
_temp1247)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1351: _temp1350=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1247)->f1; goto _LL1349;
_LL1349: _temp1348=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1247)->f2; goto _LL1276;} else{ goto _LL1277;} _LL1277: if(( unsigned int)
_temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL1355: _temp1354=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1247)->f1; goto _LL1353; _LL1353: _temp1352=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1247)->f2; goto _LL1278;} else{ goto
_LL1279;} _LL1279: if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*)
_temp1247)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1359: _temp1358=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1247)->f1; goto _LL1357; _LL1357:
_temp1356=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1247)->f2; goto _LL1280;} else{ goto _LL1281;} _LL1281: if(( unsigned int)
_temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1363: _temp1362=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1247)->f1; _temp1364=* _temp1362; _LL1368: _temp1367=( void*) _temp1364.r;
if((( struct _tunion_struct*) _temp1367)->tag == Cyc_Absyn_Var_d_tag){ _LL1370:
_temp1369=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1367)->f1; goto _LL1366;} else{ goto _LL1283;} _LL1366: _temp1365=( struct
Cyc_Position_Segment*) _temp1364.loc; goto _LL1361; _LL1361: _temp1360=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1247)->f2; goto _LL1282;}
else{ goto _LL1283;} _LL1283: if(( unsigned int) _temp1247 > 1u?(( struct
_tunion_struct*) _temp1247)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1374: _temp1373=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1247)->f1;
_temp1375=* _temp1373; _LL1379: _temp1378=( void*) _temp1375.r; if((( struct
_tunion_struct*) _temp1378)->tag == Cyc_Absyn_Let_d_tag){ _LL1392: _temp1391=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1378)->f1; goto
_LL1387; _LL1387: _temp1386=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f2; if( _temp1386 == 0){ goto _LL1285;} else{ _temp1388=* _temp1386;
_LL1390: _temp1389=( struct Cyc_List_List*) _temp1388.v; goto _LL1385;} _LL1385:
_temp1384=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1378)->f3;
goto _LL1383; _LL1383: _temp1382=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f4; goto _LL1381; _LL1381: _temp1380=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f5; goto _LL1377;} else{ goto _LL1285;} _LL1377: _temp1376=( struct
Cyc_Position_Segment*) _temp1375.loc; goto _LL1372; _LL1372: _temp1371=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1247)->f2; goto _LL1284;}
else{ goto _LL1285;} _LL1285: if(( unsigned int) _temp1247 > 1u?(( struct
_tunion_struct*) _temp1247)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1396:
_temp1395=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1247)->f1; goto _LL1394; _LL1394: _temp1393=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1247)->f2; goto _LL1286;} else{ goto
_LL1287;} _LL1287: goto _LL1288; _LL1250: return in_flow; _LL1252: return( void*)
Cyc_CfFlowInfo_BottomFL; _LL1254: Cyc_CfAbsexp_eval_absexp( env, _temp1291->loc,(
void*) aes->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1256:
return Cyc_CfAbsexp_eval_absexp( env, _temp1293->loc,( void*) aes->hd, in_flow);
_LL1258: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1295, Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1297, in_flow)); _LL1260: { void* _temp1399; void* _temp1401; struct
_tuple0 _temp1397= Cyc_NewControlFlow_cf_evalguard( env, _temp1303->loc, aes,
in_flow); _LL1402: _temp1401= _temp1397.f1; goto _LL1400; _LL1400: _temp1399=
_temp1397.f2; goto _LL1398; _LL1398: return Cyc_CfFlowInfo_join_flow( Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1301, _temp1401), Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1299,
_temp1399));} _LL1262: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1309))->flow; void* _temp1405; void* _temp1407; struct _tuple0
_temp1403= Cyc_NewControlFlow_cf_evalguard( env, _temp1311->loc, aes, e_inflow);
_LL1408: _temp1407= _temp1403.f1; goto _LL1406; _LL1406: _temp1405= _temp1403.f2;
goto _LL1404; _LL1404: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1305, _temp1407); Cyc_NewControlFlow_update_flow( _temp1309,
body_outflow); return _temp1405;}} _LL1264: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1319, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1315))->flow; void* _temp1411; void* _temp1413; struct
_tuple0 _temp1409= Cyc_NewControlFlow_cf_evalguard( env, _temp1317->loc, aes,
e_inflow); _LL1414: _temp1413= _temp1409.f1; goto _LL1412; _LL1412: _temp1411=
_temp1409.f2; goto _LL1410; _LL1410: Cyc_NewControlFlow_update_flow( _temp1319,
_temp1413); return _temp1411;} _LL1266: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1335->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1331))->flow; void* _temp1417; void* _temp1419; struct _tuple0
_temp1415= Cyc_NewControlFlow_cf_evalguard( env, _temp1333->loc, aes->tl,
e2_inflow); _LL1420: _temp1419= _temp1415.f1; goto _LL1418; _LL1418: _temp1417=
_temp1415.f2; goto _LL1416; _LL1416: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1321, _temp1419); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1325))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1327->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1331, e3_outflow); return _temp1417;}} _LL1268: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1270: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1341->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1344=( struct
Cyc_Absyn_Stmt*) _temp1341->body; goto _LL1272;} _LL1272: _temp1346= _temp1344;
goto _LL1274; _LL1274: _temp1348= _temp1346; goto _LL1276; _LL1276: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1421= _temp1348; if(
_temp1421 == 0){ _throw( Null_Exception);} _temp1421;}), in_flow); return( void*)
Cyc_CfFlowInfo_BottomFL; _LL1278: in_flow= Cyc_CfAbsexp_eval_absexp( env,
_temp1354->loc,( void*) aes->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1352, aes->tl, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1280: { struct Cyc_NewControlFlow_AnalEnv _temp1424; void* _temp1425; int
_temp1427; struct Cyc_Dict_Dict* _temp1429; struct Cyc_NewControlFlow_AnalEnv*
_temp1422= env; _temp1424=* _temp1422; _LL1430: _temp1429=( struct Cyc_Dict_Dict*)
_temp1424.roots; goto _LL1428; _LL1428: _temp1427=( int) _temp1424.in_try; goto
_LL1426; _LL1426: _temp1425=( void*) _temp1424.tryflow; goto _LL1423; _LL1423:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1358, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1427;( void*)( env->tryflow=( void*) _temp1425); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1356,
aes, scs_inflow); return s_outflow;}} _LL1282: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1431; _temp1431.hd=( void*) _temp1369; _temp1431.tl= 0;
_temp1431;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1369->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1432= e; if( _temp1432 == 0){ _throw( Null_Exception);}
_temp1432;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1360, body_inflow);} _LL1284: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1389); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1382->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1389); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1371, body_inflow);} _LL1286: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1393, in_flow); _LL1288:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1433=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1433[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1434; _temp1434.tag= Cyc_Core_Impossible_tag;
_temp1434.f1=( struct _tagged_string)({ char* _temp1435=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1436; _temp1436.curr= _temp1435; _temp1436.base=
_temp1435; _temp1436.last_plus_one= _temp1435 + 62; _temp1436;}); _temp1434;});
_temp1433;})); _LL1248:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){ s->annot=( struct _xtunion_struct*)({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1437=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EmptyAnnot_struct));
_temp1437[ 0]=({ struct Cyc_Absyn_EmptyAnnot_struct _temp1438; _temp1438.tag=
Cyc_Absyn_EmptyAnnot_tag; _temp1438;}); _temp1437;});{ void* _temp1439=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1461; struct Cyc_Absyn_Stmt* _temp1463; struct
Cyc_Absyn_Stmt* _temp1465; struct Cyc_Absyn_Stmt* _temp1467; struct Cyc_Absyn_Exp*
_temp1469; struct Cyc_Absyn_Stmt* _temp1471; struct _tuple3 _temp1473; struct
Cyc_Absyn_Stmt* _temp1475; struct Cyc_Absyn_Exp* _temp1477; struct _tuple3
_temp1479; struct Cyc_Absyn_Stmt* _temp1481; struct Cyc_Absyn_Exp* _temp1483;
struct Cyc_Absyn_Exp* _temp1485; struct Cyc_Absyn_Stmt* _temp1487; struct
_tuple3 _temp1489; struct Cyc_Absyn_Stmt* _temp1491; struct Cyc_Absyn_Exp*
_temp1493; struct _tuple3 _temp1495; struct Cyc_Absyn_Stmt* _temp1497; struct
Cyc_Absyn_Exp* _temp1499; struct Cyc_Absyn_Stmt* _temp1501; struct Cyc_Absyn_Stmt*
_temp1503; struct _tagged_string* _temp1505; struct Cyc_Absyn_Stmt* _temp1507;
struct Cyc_Absyn_Decl* _temp1509; struct Cyc_List_List* _temp1511; struct Cyc_Absyn_Stmt*
_temp1513; struct Cyc_List_List* _temp1515; struct Cyc_Absyn_Exp* _temp1517;
_LL1441: if(( unsigned int) _temp1439 > 1u?(( struct _tunion_struct*) _temp1439)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL1464: _temp1463=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1439)->f1; goto _LL1462; _LL1462: _temp1461=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1439)->f2; goto
_LL1442;} else{ goto _LL1443;} _LL1443: if(( unsigned int) _temp1439 > 1u?((
struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){
_LL1470: _temp1469=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1439)->f1; goto _LL1468; _LL1468: _temp1467=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1439)->f2; goto _LL1466; _LL1466:
_temp1465=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1439)->f3; goto _LL1444;} else{ goto _LL1445;} _LL1445: if(( unsigned int)
_temp1439 > 1u?(( struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1486: _temp1485=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1439)->f1; goto _LL1480; _LL1480: _temp1479=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1439)->f2; _LL1484: _temp1483= _temp1479.f1; goto _LL1482; _LL1482:
_temp1481= _temp1479.f2; goto _LL1474; _LL1474: _temp1473=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1439)->f3; _LL1478: _temp1477= _temp1473.f1;
goto _LL1476; _LL1476: _temp1475= _temp1473.f2; goto _LL1472; _LL1472: _temp1471=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1439)->f4; goto
_LL1446;} else{ goto _LL1447;} _LL1447: if(( unsigned int) _temp1439 > 1u?((
struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1490:
_temp1489=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1439)->f1;
_LL1494: _temp1493= _temp1489.f1; goto _LL1492; _LL1492: _temp1491= _temp1489.f2;
goto _LL1488; _LL1488: _temp1487=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1439)->f2; goto _LL1448;} else{ goto _LL1449;} _LL1449: if(( unsigned int)
_temp1439 > 1u?(( struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1502: _temp1501=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1439)->f1; goto _LL1496; _LL1496: _temp1495=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1439)->f2; _LL1500: _temp1499= _temp1495.f1; goto _LL1498; _LL1498:
_temp1497= _temp1495.f2; goto _LL1450;} else{ goto _LL1451;} _LL1451: if((
unsigned int) _temp1439 > 1u?(( struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1506: _temp1505=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1439)->f1; goto _LL1504; _LL1504: _temp1503=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1439)->f2; goto _LL1452;} else{ goto
_LL1453;} _LL1453: if(( unsigned int) _temp1439 > 1u?(( struct _tunion_struct*)
_temp1439)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1510: _temp1509=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1439)->f1; goto _LL1508; _LL1508:
_temp1507=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1439)->f2;
goto _LL1454;} else{ goto _LL1455;} _LL1455: if(( unsigned int) _temp1439 > 1u?((
struct _tunion_struct*) _temp1439)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1514:
_temp1513=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1439)->f1; goto _LL1512; _LL1512: _temp1511=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1439)->f2; goto _LL1456;} else{ goto
_LL1457;} _LL1457: if(( unsigned int) _temp1439 > 1u?(( struct _tunion_struct*)
_temp1439)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1518: _temp1517=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1439)->f1; goto _LL1516; _LL1516:
_temp1515=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1439)->f2; goto _LL1458;} else{ goto _LL1459;} _LL1459: goto _LL1460;
_LL1442: _temp1467= _temp1463; _temp1465= _temp1461; goto _LL1444; _LL1444: Cyc_NewControlFlow_cf_postpass(
_temp1467); Cyc_NewControlFlow_cf_postpass( _temp1465); return; _LL1446: Cyc_NewControlFlow_cf_postpass(
_temp1475); _temp1491= _temp1471; _temp1487= _temp1481; goto _LL1448; _LL1448:
_temp1501= _temp1487; _temp1497= _temp1491; goto _LL1450; _LL1450: Cyc_NewControlFlow_cf_postpass(
_temp1497); _temp1503= _temp1501; goto _LL1452; _LL1452: _temp1507= _temp1503;
goto _LL1454; _LL1454: Cyc_NewControlFlow_cf_postpass( _temp1507); return;
_LL1456: Cyc_NewControlFlow_cf_postpass( _temp1513); _temp1515= _temp1511; goto
_LL1458; _LL1458: for( 0; _temp1515 != 0; _temp1515= _temp1515->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1515->hd)->body);} return; _LL1460:
return; _LL1440:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1519; _temp1519.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1520=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1520->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1520->comprehension_vars=
0; _temp1520;}); _temp1519.lhsides= 0; _temp1519.access_path= 0; _temp1519;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1523; struct Cyc_List_List*
_temp1525; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1527; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1529; struct Cyc_List_List* _temp1530; struct Cyc_Dict_Dict* _temp1532;
struct Cyc_NewControlFlow_AbsEnv _temp1521= absenv; _LL1528: _temp1527=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1521.shared; _temp1529=* _temp1527;
_LL1533: _temp1532=( struct Cyc_Dict_Dict*) _temp1529.all_roots; goto _LL1531;
_LL1531: _temp1530=( struct Cyc_List_List*) _temp1529.comprehension_vars; goto
_LL1526; _LL1526: _temp1525=( struct Cyc_List_List*) _temp1521.lhsides; goto
_LL1524; _LL1524: _temp1523=( struct Cyc_List_List*) _temp1521.access_path; goto
_LL1522; _LL1522: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1534=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1534->roots= _temp1532; _temp1534->in_try= 0; _temp1534->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1534;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1535=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1535[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1536; _temp1536.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1536.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1536;}); _temp1535;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1530);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1537= out_flow; _LL1539: if(( int) _temp1537 == Cyc_CfFlowInfo_BottomFL){
goto _LL1540;} else{ goto _LL1541;} _LL1541: goto _LL1542; _LL1540: goto _LL1538;
_LL1542: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1543=( char*)"function may complete without returning a value";
struct _tagged_string _temp1544; _temp1544.curr= _temp1543; _temp1544.base=
_temp1543; _temp1544.last_plus_one= _temp1543 + 48; _temp1544;}));} goto _LL1538;
_LL1538:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1545=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1555; struct
Cyc_List_List* _temp1557; struct _tuple1* _temp1559; struct Cyc_List_List*
_temp1561; struct _tagged_string* _temp1563; _LL1547: if((( struct
_tunion_struct*) _temp1545)->tag == Cyc_Absyn_Fn_d_tag){ _LL1556: _temp1555=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1545)->f1; goto
_LL1548;} else{ goto _LL1549;} _LL1549: if((( struct _tunion_struct*) _temp1545)->tag
== Cyc_Absyn_Using_d_tag){ _LL1560: _temp1559=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1545)->f1; goto _LL1558; _LL1558: _temp1557=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1545)->f2; goto _LL1550;} else{ goto
_LL1551;} _LL1551: if((( struct _tunion_struct*) _temp1545)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1564: _temp1563=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1545)->f1; goto _LL1562; _LL1562: _temp1561=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1545)->f2; goto _LL1552;} else{ goto
_LL1553;} _LL1553: goto _LL1554; _LL1548: Cyc_NewControlFlow_cf_check_fun(
_temp1555); goto _LL1546; _LL1550: _temp1561= _temp1557; goto _LL1552; _LL1552:
Cyc_NewControlFlow_cf_check( _temp1561); goto _LL1546; _LL1554: goto _LL1546;
_LL1546:;}}