 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple0{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; int f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_VarType= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType= 2;
struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1; } ;
static const int Cyc_Absyn_TunionFieldType= 3; struct Cyc_Absyn_TunionFieldType_struct{
int tag; struct Cyc_Absyn_TunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType=
4; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType= 5; struct Cyc_Absyn_IntType_struct{ int
tag; void* f1; void* f2; } ; static const int Cyc_Absyn_FloatType= 1; static
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Null_c= 0; static
const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static const
int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const int Cyc_Absyn_Mod=
4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq= 6; static
const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static const int
Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int Cyc_Absyn_Not=
11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand= 13;
static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 31; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
32; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 33; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 34; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 35; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_SwitchC_s= 10; struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Fallthru_s=
11; struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Absyn_Switch_clause** f2; } ; static const int Cyc_Absyn_Decl_s= 12;
struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Cut_s= 13; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Splice_s= 14;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Absyn_Label_s= 15; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
16; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_SwitchC_clause{
struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment*
loc; } ; static const int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b= 1; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b= 3; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b= 4; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
tvs; void* defn; } ; static const int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Fn_d= 1;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; static
const int Cyc_Absyn_Let_d= 2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat*
f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4;
int f5; } ; static const int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Struct_d= 4;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
static const int Cyc_Absyn_Union_d= 5; struct Cyc_Absyn_Union_d_struct{ int tag;
struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d= 6;
struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; } ;
static const int Cyc_Absyn_Enum_d= 7; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 8;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; unsigned char Cyc_Absyn_EmptyAnnot[ 15u]="\000\000\000\000EmptyAnnot";
extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Rel_n_struct
Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual x, struct
Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_schar_t; extern void*
Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_double_t; extern void* Cyc_Absyn_empty_effect; extern struct _tuple0*
Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud;
extern void* Cyc_Absyn_exn_typ; extern struct _tuple0* Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0* Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_const_string_typ( void* rgn); extern void*
Cyc_Absyn_file_typ(); extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern struct Cyc_Core_Opt* Cyc_Absyn_void_star_typ_opt(); extern void* Cyc_Absyn_strct(
struct _tagged_arr* name); extern void* Cyc_Absyn_strctq( struct _tuple0* name);
extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name); extern void* Cyc_Absyn_union_typ(
struct _tagged_arr* name); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp( void*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_array_exp( struct Cyc_List_List*, struct Cyc_Position_Segment*);
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
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct Cyc_List_List*
ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment* loc);
extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_List_List* rgn_po, struct Cyc_List_List*); extern void*
Cyc_Absyn_pointer_expand( void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple3*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern unsigned char Cyc_Tcutil_TypeErr[ 12u];
extern void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); static int Cyc_Absyn_zstrlist_cmp(
struct Cyc_List_List* ss1, struct Cyc_List_List* ss2){ return(( int(*)( int(*
cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_zstrptrcmp, ss1, ss2);}
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List* vs1, struct Cyc_List_List* vs2){
return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} struct _tuple4{ void* f1; void* f2; }
; int Cyc_Absyn_qvar_cmp( struct _tuple0* q1, struct _tuple0* q2){ void* _temp0=(*
q1).f1; void* _temp1=(* q2).f1;{ struct _tuple4 _temp3=({ struct _tuple4 _temp2;
_temp2.f1= _temp0; _temp2.f2= _temp1; _temp2;}); void* _temp19; void* _temp21;
void* _temp23; struct Cyc_List_List* _temp25; void* _temp27; struct Cyc_List_List*
_temp29; void* _temp31; struct Cyc_List_List* _temp33; void* _temp35; struct Cyc_List_List*
_temp37; void* _temp39; void* _temp41; void* _temp43; void* _temp45; _LL5: _LL22:
_temp21= _temp3.f1; if( _temp21 == ( void*) Cyc_Absyn_Loc_n){ goto _LL20;} else{
goto _LL7;} _LL20: _temp19= _temp3.f2; if( _temp19 == ( void*) Cyc_Absyn_Loc_n){
goto _LL6;} else{ goto _LL7;} _LL7: _LL28: _temp27= _temp3.f1; if(( unsigned int)
_temp27 >  1u?*(( int*) _temp27) ==  Cyc_Absyn_Rel_n: 0){ _LL30: _temp29=((
struct Cyc_Absyn_Rel_n_struct*) _temp27)->f1; goto _LL24;} else{ goto _LL9;}
_LL24: _temp23= _temp3.f2; if(( unsigned int) _temp23 >  1u?*(( int*) _temp23)
==  Cyc_Absyn_Rel_n: 0){ _LL26: _temp25=(( struct Cyc_Absyn_Rel_n_struct*)
_temp23)->f1; goto _LL8;} else{ goto _LL9;} _LL9: _LL36: _temp35= _temp3.f1; if((
unsigned int) _temp35 >  1u?*(( int*) _temp35) ==  Cyc_Absyn_Abs_n: 0){ _LL38:
_temp37=(( struct Cyc_Absyn_Abs_n_struct*) _temp35)->f1; goto _LL32;} else{ goto
_LL11;} _LL32: _temp31= _temp3.f2; if(( unsigned int) _temp31 >  1u?*(( int*)
_temp31) ==  Cyc_Absyn_Abs_n: 0){ _LL34: _temp33=(( struct Cyc_Absyn_Abs_n_struct*)
_temp31)->f1; goto _LL10;} else{ goto _LL11;} _LL11: _LL40: _temp39= _temp3.f1;
if( _temp39 == ( void*) Cyc_Absyn_Loc_n){ goto _LL12;} else{ goto _LL13;} _LL13:
_LL42: _temp41= _temp3.f2; if( _temp41 == ( void*) Cyc_Absyn_Loc_n){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL44: _temp43= _temp3.f1; if(( unsigned int) _temp43
>  1u?*(( int*) _temp43) ==  Cyc_Absyn_Rel_n: 0){ goto _LL16;} else{ goto _LL17;}
_LL17: _LL46: _temp45= _temp3.f2; if(( unsigned int) _temp45 >  1u?*(( int*)
_temp45) ==  Cyc_Absyn_Rel_n: 0){ goto _LL18;} else{ goto _LL4;} _LL6: goto _LL4;
_LL8: _temp37= _temp29; _temp33= _temp25; goto _LL10; _LL10: { int i= Cyc_Absyn_zstrlist_cmp(
_temp37, _temp33); if( i !=  0){ return i;} goto _LL4;} _LL12: return - 1; _LL14:
return 1; _LL16: return - 1; _LL18: return 1; _LL4:;} return Cyc_Std_zstrptrcmp((*
q1).f2,(* q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct
Cyc_Absyn_Tvar* tv2){ int i= Cyc_Std_zstrptrcmp( tv1->name, tv2->name); if( i != 
0){ return i;} if( tv1->identity ==  tv2->identity){ return 0;} if( tv1->identity
!=  0? tv2->identity !=  0: 0){ return*(( int*) _check_null( tv1->identity)) - *((
int*) _check_null( tv2->identity));} else{ if( tv1->identity ==  0){ return - 1;}
else{ return 1;}}} struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={ 0u,
0}; void* Cyc_Absyn_rel_ns_null=( void*)& Cyc_Absyn_rel_ns_null_value; int Cyc_Absyn_is_qvar_qualified(
struct _tuple0* qv){ void* _temp48=(* qv).f1; struct Cyc_List_List* _temp58;
struct Cyc_List_List* _temp60; _LL50: if(( unsigned int) _temp48 >  1u?*(( int*)
_temp48) ==  Cyc_Absyn_Rel_n: 0){ _LL59: _temp58=(( struct Cyc_Absyn_Rel_n_struct*)
_temp48)->f1; if( _temp58 ==  0){ goto _LL51;} else{ goto _LL52;}} else{ goto
_LL52;} _LL52: if(( unsigned int) _temp48 >  1u?*(( int*) _temp48) ==  Cyc_Absyn_Abs_n:
0){ _LL61: _temp60=(( struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; if( _temp60
==  0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if( _temp48
== ( void*) Cyc_Absyn_Loc_n){ goto _LL55;} else{ goto _LL56;} _LL56: goto _LL57;
_LL51: goto _LL53; _LL53: goto _LL55; _LL55: return 0; _LL57: return 1; _LL49:;}
static int Cyc_Absyn_new_type_counter= 0; void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* env){ return( void*)({ struct Cyc_Absyn_Evar_struct*
_temp62=( struct Cyc_Absyn_Evar_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Evar_struct));
_temp62[ 0]=({ struct Cyc_Absyn_Evar_struct _temp63; _temp63.tag= Cyc_Absyn_Evar;
_temp63.f1= k; _temp63.f2= 0; _temp63.f3= Cyc_Absyn_new_type_counter ++; _temp63.f4=
env; _temp63;}); _temp62;});} static struct Cyc_Core_Opt Cyc_Absyn_mk={( void*)((
void*) 1u)}; void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt* tenv){ return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk, tenv);} struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y){ return({ struct Cyc_Absyn_Tqual
_temp64; _temp64.q_const= x.q_const? 1: y.q_const; _temp64.q_volatile= x.q_volatile?
1: y.q_volatile; _temp64.q_restrict= x.q_restrict? 1: y.q_restrict; _temp64;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp65; _temp65.q_const= 0; _temp65.q_volatile= 0; _temp65.q_restrict= 0;
_temp65;});} struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp66; _temp66.q_const= 1; _temp66.q_volatile= 0; _temp66.q_restrict= 0;
_temp66;});} struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x){ return({
struct Cyc_Absyn_Conref* _temp67=( struct Cyc_Absyn_Conref*) _cycalloc( sizeof(
struct Cyc_Absyn_Conref)); _temp67->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp68=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp68[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp69; _temp69.tag= Cyc_Absyn_Eq_constr;
_temp69.f1=( void*) x; _temp69;}); _temp68;})); _temp67;});} struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(){ return({ struct Cyc_Absyn_Conref* _temp70=( struct Cyc_Absyn_Conref*)
_cycalloc( sizeof( struct Cyc_Absyn_Conref)); _temp70->v=( void*)(( void*) Cyc_Absyn_No_constr);
_temp70;});} static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0u,( void*) 1}; static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={
0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={( void*)(( void*)&
Cyc_Absyn_true_constraint)}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(
void*)(( void*)& Cyc_Absyn_false_constraint)}; struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v; struct Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){
void* _temp73=( void*) x->v; struct Cyc_Absyn_Conref* _temp81; struct Cyc_Absyn_Conref**
_temp83; _LL75: if( _temp73 == ( void*) Cyc_Absyn_No_constr){ goto _LL76;} else{
goto _LL77;} _LL77: if(( unsigned int) _temp73 >  1u?*(( int*) _temp73) ==  Cyc_Absyn_Eq_constr:
0){ goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp73 >  1u?*((
int*) _temp73) ==  Cyc_Absyn_Forward_constr: 0){ _LL82: _temp81=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp73)->f1; _temp83=( struct Cyc_Absyn_Conref**)&(( struct Cyc_Absyn_Forward_constr_struct*)
_temp73)->f1; goto _LL80;} else{ goto _LL74;} _LL76: return x; _LL78: return x;
_LL80: { struct Cyc_Absyn_Conref* _temp84= Cyc_Absyn_compress_conref(* _temp83);*
_temp83= _temp84; return _temp84;} _LL74:;} void* Cyc_Absyn_conref_val( struct
Cyc_Absyn_Conref* x){ void* _temp85=( void*)( Cyc_Absyn_compress_conref( x))->v;
void* _temp91; _LL87: if(( unsigned int) _temp85 >  1u?*(( int*) _temp85) == 
Cyc_Absyn_Eq_constr: 0){ _LL92: _temp91=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp85)->f1; goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL88: return
_temp91; _LL90: return({ void* _temp93[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("conref_val",
sizeof( unsigned char), 11u), _tag_arr( _temp93, sizeof( void*), 0u));}); _LL86:;}
void* Cyc_Absyn_conref_def( void* y, struct Cyc_Absyn_Conref* x){ void* _temp94=(
void*)( Cyc_Absyn_compress_conref( x))->v; void* _temp100; _LL96: if((
unsigned int) _temp94 >  1u?*(( int*) _temp94) ==  Cyc_Absyn_Eq_constr: 0){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp94)->f1;
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: return _temp100; _LL99:
return y; _LL95:;} static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={ 5u,(
void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={ 5u,( void*)(( void*) 1u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 3u)}; void*
Cyc_Absyn_uchar_t=( void*)& Cyc_Absyn_uchar_tt; void* Cyc_Absyn_ushort_t=( void*)&
Cyc_Absyn_ushort_tt; void* Cyc_Absyn_uint_t=( void*)& Cyc_Absyn_uint_tt; void*
Cyc_Absyn_ulong_t=( void*)& Cyc_Absyn_ulong_tt; void* Cyc_Absyn_ulonglong_t=(
void*)& Cyc_Absyn_ulonglong_tt; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={ 5u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 3u)}; void*
Cyc_Absyn_schar_t=( void*)& Cyc_Absyn_schar_tt; void* Cyc_Absyn_sshort_t=( void*)&
Cyc_Absyn_sshort_tt; void* Cyc_Absyn_sint_t=( void*)& Cyc_Absyn_sint_tt; void*
Cyc_Absyn_slong_t=( void*)& Cyc_Absyn_slong_tt; void* Cyc_Absyn_slonglong_t=(
void*)& Cyc_Absyn_slonglong_tt; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=(
void*) 2u; static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={ 1u, 0};
static unsigned char _temp113[ 4u]="exn"; static struct _tagged_arr Cyc_Absyn_exn_str={
_temp113, _temp113, _temp113 +  4u}; static struct _tuple0 Cyc_Absyn_exn_name_v={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_exn_str}; struct _tuple0* Cyc_Absyn_exn_name=&
Cyc_Absyn_exn_name_v; static unsigned char _temp114[ 15u]="Null_Exception";
static struct _tagged_arr Cyc_Absyn_Null_Exception_str={ _temp114, _temp114,
_temp114 +  15u}; static struct _tuple0 Cyc_Absyn_Null_Exception_pr={( void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str}; struct _tuple0* Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v; static
unsigned char _temp115[ 13u]="Array_bounds"; static struct _tagged_arr Cyc_Absyn_Array_bounds_str={
_temp115, _temp115, _temp115 +  13u}; static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str}; struct _tuple0* Cyc_Absyn_Array_bounds_name=&
Cyc_Absyn_Array_bounds_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={&
Cyc_Absyn_Array_bounds_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v; static unsigned char
_temp116[ 16u]="Match_Exception"; static struct _tagged_arr Cyc_Absyn_Match_Exception_str={
_temp116, _temp116, _temp116 +  16u}; static struct _tuple0 Cyc_Absyn_Match_Exception_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str}; struct _tuple0* Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v; static
unsigned char _temp117[ 10u]="Bad_alloc"; static struct _tagged_arr Cyc_Absyn_Bad_alloc_str={
_temp117, _temp117, _temp117 +  10u}; static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str}; struct _tuple0* Cyc_Absyn_Bad_alloc_name=&
Cyc_Absyn_Bad_alloc_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={&
Cyc_Absyn_Bad_alloc_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v; static struct Cyc_List_List
Cyc_Absyn_exn_l0={( void*)& Cyc_Absyn_Null_Exception_tuf_v, 0}; static struct
Cyc_List_List Cyc_Absyn_exn_l1={( void*)& Cyc_Absyn_Array_bounds_tuf_v,( struct
Cyc_List_List*)& Cyc_Absyn_exn_l0}; static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,( struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={( void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2}; static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)(( struct Cyc_List_List*)& Cyc_Absyn_exn_l3)}; static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={( void*)(( void*) 3u),& Cyc_Absyn_exn_name_v, 0,( struct
Cyc_Core_Opt*)& Cyc_Absyn_exn_ol, 1}; struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud=&
Cyc_Absyn_exn_tud_v; static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={
1u,& Cyc_Absyn_exn_tud_v}; static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={
2u,{( void*)(( void*)& Cyc_Absyn_exn_tinfou), 0,( void*)(( void*) 3u)}}; void*
Cyc_Absyn_exn_typ=( void*)& Cyc_Absyn_exn_typ_tt; static unsigned char _temp120[
9u]="PrintArg"; static struct _tagged_arr Cyc_Absyn_printarg_str={ _temp120,
_temp120, _temp120 +  9u}; static unsigned char _temp121[ 9u]="ScanfArg"; static
struct _tagged_arr Cyc_Absyn_scanfarg_str={ _temp121, _temp121, _temp121 +  9u};
static unsigned char _temp122[ 4u]="Cyc"; static struct _tagged_arr Cyc_Absyn_cyc_str={
_temp122, _temp122, _temp122 +  4u}; static unsigned char _temp123[ 4u]="Std";
static struct _tagged_arr Cyc_Absyn_std_str={ _temp123, _temp123, _temp123 +  4u};
static struct Cyc_List_List Cyc_Absyn_std_list={( void*)& Cyc_Absyn_std_str, 0};
static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_std_namespace={ 1u,( struct Cyc_List_List*)&
Cyc_Absyn_std_list}; static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str}; static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={( void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};
struct _tuple0* Cyc_Absyn_tunion_print_arg_qvar=& Cyc_Absyn_tunion_print_arg_qvar_p;
struct _tuple0* Cyc_Absyn_tunion_scanf_arg_qvar=& Cyc_Absyn_tunion_scanf_arg_qvar_p;
static struct Cyc_Core_Opt* Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ(
void* rgn){ if( rgn != ( void*) Cyc_Absyn_HeapRgn){ return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);}
else{ if( Cyc_Absyn_string_t_opt ==  0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);
Cyc_Absyn_string_t_opt=({ struct Cyc_Core_Opt* _temp125=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp125->v=( void*) t; _temp125;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_string_t_opt))->v;}}
static struct Cyc_Core_Opt* Cyc_Absyn_const_string_t_opt= 0; void* Cyc_Absyn_const_string_typ(
void* rgn){ if( rgn != ( void*) Cyc_Absyn_HeapRgn){ return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);}
else{ if( Cyc_Absyn_const_string_t_opt ==  0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);
Cyc_Absyn_const_string_t_opt=({ struct Cyc_Core_Opt* _temp126=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp126->v=( void*) t; _temp126;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_const_string_t_opt))->v;}}
static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={ 2u,( void*)(( void*) 1u),
1}; static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={ 0u,( void*)((
void*)& Cyc_Absyn_one_intc)}; struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={
0,( void*)(( void*)& Cyc_Absyn_one_b_raw), 0}; struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v; static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0u,& Cyc_Absyn_exp_unsigned_one_v}; void* Cyc_Absyn_bounds_one=( void*)& Cyc_Absyn_one_bt;
void* Cyc_Absyn_starb_typ( void* t, void* r, struct Cyc_Absyn_Tqual tq, void* b){
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp130=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp130[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp131; _temp131.tag= Cyc_Absyn_PointerType;
_temp131.f1=({ struct Cyc_Absyn_PtrInfo _temp132; _temp132.elt_typ=( void*) t;
_temp132.rgn_typ=( void*) r; _temp132.nullable= Cyc_Absyn_true_conref; _temp132.tq=
tq; _temp132.bounds= Cyc_Absyn_new_conref( b); _temp132;}); _temp131;});
_temp130;});} void* Cyc_Absyn_atb_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp133=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp133[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp134; _temp134.tag= Cyc_Absyn_PointerType;
_temp134.f1=({ struct Cyc_Absyn_PtrInfo _temp135; _temp135.elt_typ=( void*) t;
_temp135.rgn_typ=( void*) r; _temp135.nullable= Cyc_Absyn_false_conref; _temp135.tq=
tq; _temp135.bounds= Cyc_Absyn_new_conref( b); _temp135;}); _temp134;});
_temp133;});} void* Cyc_Absyn_star_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return Cyc_Absyn_starb_typ( t, r, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq){ return Cyc_Absyn_starb_typ( t,( void*) Cyc_Absyn_HeapRgn,
tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_at_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return Cyc_Absyn_atb_typ( t, r, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_tagged_typ(
void* t, void* r, struct Cyc_Absyn_Tqual tq){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp136=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp136[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp137; _temp137.tag= Cyc_Absyn_PointerType;
_temp137.f1=({ struct Cyc_Absyn_PtrInfo _temp138; _temp138.elt_typ=( void*) t;
_temp138.rgn_typ=( void*) r; _temp138.nullable= Cyc_Absyn_true_conref; _temp138.tq=
tq; _temp138.bounds= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_Unknown_b);
_temp138;}); _temp137;}); _temp136;});} static struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt=
0; static unsigned char _temp139[ 8u]="__sFILE"; static struct _tagged_arr Cyc_Absyn_sf_str={
_temp139, _temp139, _temp139 +  8u}; static struct _tagged_arr* Cyc_Absyn_sf=&
Cyc_Absyn_sf_str; static unsigned char _temp140[ 4u]="Std"; static struct
_tagged_arr Cyc_Absyn_st_str={ _temp140, _temp140, _temp140 +  4u}; static
struct _tagged_arr* Cyc_Absyn_st=& Cyc_Absyn_st_str; void* Cyc_Absyn_file_typ(){
if( Cyc_Absyn_file_t_opt ==  0){ struct _tuple0* file_t_name=({ struct _tuple0*
_temp147=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp147->f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp148=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp148[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp149; _temp149.tag= Cyc_Absyn_Abs_n; _temp149.f1=({ struct _tagged_arr*
_temp150[ 1u]; _temp150[ 0u]= Cyc_Absyn_st;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp150, sizeof( struct _tagged_arr*),
1u));}); _temp149;}); _temp148;}); _temp147->f2= Cyc_Absyn_sf; _temp147;});
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp145=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp145->sc=( void*)(( void*) Cyc_Absyn_Abstract); _temp145->name=({ struct Cyc_Core_Opt*
_temp146=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp146->v=( void*) file_t_name; _temp146;}); _temp145->tvs= 0; _temp145->fields=
0; _temp145->attributes= 0; _temp145;}); void* file_struct_typ=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp142=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp142[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp143; _temp143.tag= Cyc_Absyn_StructType;
_temp143.f1=( struct _tuple0*) file_t_name; _temp143.f2= 0; _temp143.f3=({
struct Cyc_Absyn_Structdecl** _temp144=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*) *  1); _temp144[ 0]= sd;
_temp144;}); _temp143;}); _temp142;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp141=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp141->v=( void*) Cyc_Absyn_at_typ( file_struct_typ,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp141;});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Absyn_file_t_opt))->v;} static struct Cyc_Core_Opt* Cyc_Absyn_void_star_t_opt=
0; void* Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt ==  0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp151=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp151->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp151;});} return( void*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_void_star_t_opt))->v;} struct Cyc_Core_Opt*
Cyc_Absyn_void_star_typ_opt(){ if( Cyc_Absyn_void_star_t_opt ==  0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp152=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp152->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp152;});} return Cyc_Absyn_void_star_t_opt;}
static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={ 17u, 0}; void* Cyc_Absyn_empty_effect=(
void*)& Cyc_Absyn_empty_eff; void* Cyc_Absyn_strct( struct _tagged_arr* name){
return( void*)({ struct Cyc_Absyn_StructType_struct* _temp154=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp154[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp155; _temp155.tag= Cyc_Absyn_StructType;
_temp155.f1=({ struct _tuple0* _temp156=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp156->f1= Cyc_Absyn_rel_ns_null; _temp156->f2= name;
_temp156;}); _temp155.f2= 0; _temp155.f3= 0; _temp155;}); _temp154;});} void*
Cyc_Absyn_union_typ( struct _tagged_arr* name){ return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp157=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp157[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp158; _temp158.tag= Cyc_Absyn_UnionType;
_temp158.f1=({ struct _tuple0* _temp159=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp159->f1= Cyc_Absyn_rel_ns_null; _temp159->f2= name;
_temp159;}); _temp158.f2= 0; _temp158.f3= 0; _temp158;}); _temp157;});} void*
Cyc_Absyn_strctq( struct _tuple0* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp160=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp160[ 0]=({ struct Cyc_Absyn_StructType_struct _temp161; _temp161.tag= Cyc_Absyn_StructType;
_temp161.f1=( struct _tuple0*) name; _temp161.f2= 0; _temp161.f3= 0; _temp161;});
_temp160;});} void* Cyc_Absyn_unionq_typ( struct _tuple0* name){ return( void*)({
struct Cyc_Absyn_UnionType_struct* _temp162=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp162[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp163; _temp163.tag= Cyc_Absyn_UnionType; _temp163.f1=(
struct _tuple0*) name; _temp163.f2= 0; _temp163.f3= 0; _temp163;}); _temp162;});}
struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp164=( struct Cyc_Absyn_Exp*) _cycalloc(
sizeof( struct Cyc_Absyn_Exp)); _temp164->topt= 0; _temp164->r=( void*) r;
_temp164->loc= loc; _temp164;});} struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_New_e_struct*
_temp165=( struct Cyc_Absyn_New_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_New_e_struct));
_temp165[ 0]=({ struct Cyc_Absyn_New_e_struct _temp166; _temp166.tag= Cyc_Absyn_New_e;
_temp166.f1= rgn_handle; _temp166.f2= e; _temp166;}); _temp165;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp* e){ return({ struct Cyc_Absyn_Exp*
_temp167=( struct Cyc_Absyn_Exp*) _cycalloc( sizeof( struct Cyc_Absyn_Exp));
_temp167->topt= e->topt; _temp167->r=( void*)(( void*) e->r); _temp167->loc= e->loc;
_temp167;});} struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void* c, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp168=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp168[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp169; _temp169.tag= Cyc_Absyn_Const_e;
_temp169.f1=( void*) c; _temp169;}); _temp168;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp170=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp170[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp171; _temp171.tag= Cyc_Absyn_Const_e; _temp171.f1=( void*)(( void*) Cyc_Absyn_Null_c);
_temp171;}); _temp170;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*
s, int i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp172=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp172[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp173; _temp173.tag= Cyc_Absyn_Int_c; _temp173.f1=( void*) s; _temp173.f2= i;
_temp173;}); _temp172;}), seg);} struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
i, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Unsigned,( int) i, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_signed_int_exp( b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_bool_exp( 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
unsigned char c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Char_c_struct* _temp174=( struct Cyc_Absyn_Char_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp174[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp175; _temp175.tag= Cyc_Absyn_Char_c; _temp175.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp175.f2= c; _temp175;}); _temp174;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_arr f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Float_c_struct* _temp176=( struct Cyc_Absyn_Float_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp176[ 0]=({ struct Cyc_Absyn_Float_c_struct
_temp177; _temp177.tag= Cyc_Absyn_Float_c; _temp177.f1= f; _temp177;}); _temp176;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct _tagged_arr s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_String_c_struct*
_temp178=( struct Cyc_Absyn_String_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp178[ 0]=({ struct Cyc_Absyn_String_c_struct _temp179; _temp179.tag= Cyc_Absyn_String_c;
_temp179.f1= s; _temp179;}); _temp178;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp180=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp180[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp181; _temp181.tag= Cyc_Absyn_Var_e; _temp181.f1= q; _temp181.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp181;}); _temp180;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp( struct _tuple0* q, void* b, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp182=(
struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp182[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp183; _temp183.tag= Cyc_Absyn_Var_e;
_temp183.f1= q; _temp183.f2=( void*) b; _temp183;}); _temp182;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0* q, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp184=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp184[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp185; _temp185.tag= Cyc_Absyn_UnknownId_e;
_temp185.f1= q; _temp185;}); _temp184;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp186=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp186[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp187; _temp187.tag= Cyc_Absyn_Primop_e; _temp187.f1=(
void*) p; _temp187.f2= es; _temp187;}); _temp186;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp188=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp188->hd=( void*)
e; _temp188->tl= 0; _temp188;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp189=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp189->hd=( void*)
e1; _temp189->tl=({ struct Cyc_List_List* _temp190=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp190->hd=( void*) e2; _temp190->tl=
0; _temp190;}); _temp189;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Eq, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Neq,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Gt, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lt, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Lte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* popt, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_AssignOp_e_struct* _temp191=( struct Cyc_Absyn_AssignOp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp191[ 0]=({ struct
Cyc_Absyn_AssignOp_e_struct _temp192; _temp192.tag= Cyc_Absyn_AssignOp_e;
_temp192.f1= e1; _temp192.f2= popt; _temp192.f3= e2; _temp192;}); _temp191;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp(
e1, 0, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp( struct Cyc_Absyn_Exp*
e, void* i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp193=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp193[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp194; _temp194.tag= Cyc_Absyn_Increment_e;
_temp194.f1= e; _temp194.f2=( void*) i; _temp194;}); _temp193;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostInc, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreInc, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreDec, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostDec, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Conditional_e_struct*
_temp195=( struct Cyc_Absyn_Conditional_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp195[ 0]=({ struct Cyc_Absyn_Conditional_e_struct _temp196; _temp196.tag=
Cyc_Absyn_Conditional_e; _temp196.f1= e1; _temp196.f2= e2; _temp196.f3= e3;
_temp196;}); _temp195;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_SeqExp_e_struct* _temp197=( struct Cyc_Absyn_SeqExp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp197[ 0]=({ struct
Cyc_Absyn_SeqExp_e_struct _temp198; _temp198.tag= Cyc_Absyn_SeqExp_e; _temp198.f1=
e1; _temp198.f2= e2; _temp198;}); _temp197;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownCall_e_struct*
_temp199=( struct Cyc_Absyn_UnknownCall_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct));
_temp199[ 0]=({ struct Cyc_Absyn_UnknownCall_e_struct _temp200; _temp200.tag=
Cyc_Absyn_UnknownCall_e; _temp200.f1= e; _temp200.f2= es; _temp200;}); _temp199;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp201=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp201[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp202; _temp202.tag= Cyc_Absyn_FnCall_e; _temp202.f1=
e; _temp202.f2= es; _temp202.f3= 0; _temp202;}); _temp201;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_NoInstantiate_e_struct*
_temp203=( struct Cyc_Absyn_NoInstantiate_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NoInstantiate_e_struct)); _temp203[ 0]=({ struct Cyc_Absyn_NoInstantiate_e_struct
_temp204; _temp204.tag= Cyc_Absyn_NoInstantiate_e; _temp204.f1= e; _temp204;});
_temp203;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp205=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp205[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp206; _temp206.tag= Cyc_Absyn_Instantiate_e;
_temp206.f1= e; _temp206.f2= ts; _temp206;}); _temp205;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp207=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp207[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp208; _temp208.tag= Cyc_Absyn_Cast_e;
_temp208.f1=( void*) t; _temp208.f2= e; _temp208;}); _temp207;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Throw_e_struct*
_temp209=( struct Cyc_Absyn_Throw_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp209[ 0]=({ struct Cyc_Absyn_Throw_e_struct _temp210; _temp210.tag= Cyc_Absyn_Throw_e;
_temp210.f1= e; _temp210;}); _temp209;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Address_e_struct* _temp211=( struct Cyc_Absyn_Address_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Address_e_struct)); _temp211[ 0]=({ struct
Cyc_Absyn_Address_e_struct _temp212; _temp212.tag= Cyc_Absyn_Address_e; _temp212.f1=
e; _temp212;}); _temp211;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct* _temp213=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp213[ 0]=({ struct
Cyc_Absyn_Sizeoftyp_e_struct _temp214; _temp214.tag= Cyc_Absyn_Sizeoftyp_e;
_temp214.f1=( void*) t; _temp214;}); _temp213;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Sizeofexp_e_struct*
_temp215=( struct Cyc_Absyn_Sizeofexp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeofexp_e_struct));
_temp215[ 0]=({ struct Cyc_Absyn_Sizeofexp_e_struct _temp216; _temp216.tag= Cyc_Absyn_Sizeofexp_e;
_temp216.f1= e; _temp216;}); _temp215;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp(
void* t, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp217=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp217[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp218; _temp218.tag= Cyc_Absyn_Offsetof_e;
_temp218.f1=( void*) t; _temp218.f2= n; _temp218;}); _temp217;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Deref_e_struct*
_temp219=( struct Cyc_Absyn_Deref_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Deref_e_struct));
_temp219[ 0]=({ struct Cyc_Absyn_Deref_e_struct _temp220; _temp220.tag= Cyc_Absyn_Deref_e;
_temp220.f1= e; _temp220;}); _temp219;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp* e, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_StructMember_e_struct*
_temp221=( struct Cyc_Absyn_StructMember_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_StructMember_e_struct)); _temp221[ 0]=({ struct Cyc_Absyn_StructMember_e_struct
_temp222; _temp222.tag= Cyc_Absyn_StructMember_e; _temp222.f1= e; _temp222.f2= n;
_temp222;}); _temp221;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp* e, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_StructArrow_e_struct*
_temp223=( struct Cyc_Absyn_StructArrow_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct));
_temp223[ 0]=({ struct Cyc_Absyn_StructArrow_e_struct _temp224; _temp224.tag=
Cyc_Absyn_StructArrow_e; _temp224.f1= e; _temp224.f2= n; _temp224;}); _temp223;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp* e,
struct _tagged_arr* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp(
Cyc_Absyn_structmember_exp( e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Subscript_e_struct*
_temp225=( struct Cyc_Absyn_Subscript_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Subscript_e_struct));
_temp225[ 0]=({ struct Cyc_Absyn_Subscript_e_struct _temp226; _temp226.tag= Cyc_Absyn_Subscript_e;
_temp226.f1= e1; _temp226.f2= e2; _temp226;}); _temp225;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Tuple_e_struct* _temp227=(
struct Cyc_Absyn_Tuple_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp227[ 0]=({ struct Cyc_Absyn_Tuple_e_struct _temp228; _temp228.tag= Cyc_Absyn_Tuple_e;
_temp228.f1= es; _temp228;}); _temp227;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StmtExp_e_struct* _temp229=( struct Cyc_Absyn_StmtExp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct)); _temp229[ 0]=({ struct
Cyc_Absyn_StmtExp_e_struct _temp230; _temp230.tag= Cyc_Absyn_StmtExp_e; _temp230.f1=
s; _temp230;}); _temp229;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_var_exp( Cyc_Absyn_Match_Exception_name,
loc);} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* dles= 0; for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ dles=({ struct Cyc_List_List* _temp231=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp231->hd=( void*)({ struct
_tuple5* _temp232=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp232->f1= 0; _temp232->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd; _temp232;}); _temp231->tl= dles; _temp231;});} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp233=(
struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp233[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp234; _temp234.tag= Cyc_Absyn_Array_e;
_temp234.f1= dles; _temp234;}); _temp233;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp235=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp235[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp236; _temp236.tag= Cyc_Absyn_UnresolvedMem_e; _temp236.f1= n; _temp236.f2=
dles; _temp236;}); _temp235;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Stmt*
_temp237=( struct Cyc_Absyn_Stmt*) _cycalloc( sizeof( struct Cyc_Absyn_Stmt));
_temp237->r=( void*) s; _temp237->loc= loc; _temp237->non_local_preds= 0;
_temp237->try_depth= 0; _temp237->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot);
_temp237;});} struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*) Cyc_Absyn_Skip_s, loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Exp_s_struct* _temp238=(
struct Cyc_Absyn_Exp_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Exp_s_struct));
_temp238[ 0]=({ struct Cyc_Absyn_Exp_s_struct _temp239; _temp239.tag= Cyc_Absyn_Exp_s;
_temp239.f1= e; _temp239;}); _temp238;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List* ss, struct Cyc_Position_Segment* loc){ if( ss ==  0){
return Cyc_Absyn_skip_stmt( loc);} else{ if((( struct Cyc_List_List*)
_check_null( ss))->tl ==  0){ return( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( ss))->hd;} else{ return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*) _check_null( ss))->hd, Cyc_Absyn_seq_stmts((( struct Cyc_List_List*)
_check_null( ss))->tl, loc), loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Return_s_struct* _temp240=( struct Cyc_Absyn_Return_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Return_s_struct)); _temp240[ 0]=({ struct
Cyc_Absyn_Return_s_struct _temp241; _temp241.tag= Cyc_Absyn_Return_s; _temp241.f1=
e; _temp241;}); _temp240;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_IfThenElse_s_struct* _temp242=( struct Cyc_Absyn_IfThenElse_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct)); _temp242[ 0]=({
struct Cyc_Absyn_IfThenElse_s_struct _temp243; _temp243.tag= Cyc_Absyn_IfThenElse_s;
_temp243.f1= e; _temp243.f2= s1; _temp243.f3= s2; _temp243;}); _temp242;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_While_s_struct* _temp244=( struct Cyc_Absyn_While_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_While_s_struct)); _temp244[ 0]=({ struct Cyc_Absyn_While_s_struct
_temp245; _temp245.tag= Cyc_Absyn_While_s; _temp245.f1=({ struct _tuple2
_temp246; _temp246.f1= e; _temp246.f2= Cyc_Absyn_skip_stmt( e->loc); _temp246;});
_temp245.f2= s; _temp245;}); _temp244;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Break_s_struct* _temp247=( struct Cyc_Absyn_Break_s_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Break_s_struct)); _temp247[ 0]=({ struct Cyc_Absyn_Break_s_struct
_temp248; _temp248.tag= Cyc_Absyn_Break_s; _temp248.f1= 0; _temp248;}); _temp247;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Continue_s_struct*
_temp249=( struct Cyc_Absyn_Continue_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp249[ 0]=({ struct Cyc_Absyn_Continue_s_struct _temp250; _temp250.tag= Cyc_Absyn_Continue_s;
_temp250.f1= 0; _temp250;}); _temp249;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_For_s_struct* _temp251=( struct Cyc_Absyn_For_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_For_s_struct)); _temp251[ 0]=({ struct Cyc_Absyn_For_s_struct
_temp252; _temp252.tag= Cyc_Absyn_For_s; _temp252.f1= e1; _temp252.f2=({ struct
_tuple2 _temp253; _temp253.f1= e2; _temp253.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp253;}); _temp252.f3=({ struct _tuple2 _temp254; _temp254.f1= e3; _temp254.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp254;}); _temp252.f4= s; _temp252;});
_temp251;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Switch_s_struct* _temp255=( struct Cyc_Absyn_Switch_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp255[ 0]=({ struct
Cyc_Absyn_Switch_s_struct _temp256; _temp256.tag= Cyc_Absyn_Switch_s; _temp256.f1=
e; _temp256.f2= scs; _temp256;}); _temp255;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc){ void* _temp257=( void*) s1->r; _LL259: if( _temp257 == ( void*) Cyc_Absyn_Skip_s){
goto _LL260;} else{ goto _LL261;} _LL261: goto _LL262; _LL260: return s2; _LL262:
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Seq_s_struct* _temp263=(
struct Cyc_Absyn_Seq_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Seq_s_struct));
_temp263[ 0]=({ struct Cyc_Absyn_Seq_s_struct _temp264; _temp264.tag= Cyc_Absyn_Seq_s;
_temp264.f1= s1; _temp264.f2= s2; _temp264;}); _temp263;}), loc); _LL258:;}
struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Fallthru_s_struct*
_temp265=( struct Cyc_Absyn_Fallthru_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct));
_temp265[ 0]=({ struct Cyc_Absyn_Fallthru_s_struct _temp266; _temp266.tag= Cyc_Absyn_Fallthru_s;
_temp266.f1= el; _temp266.f2= 0; _temp266;}); _temp265;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp267=( struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp267[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp268; _temp268.tag= Cyc_Absyn_Decl_s;
_temp268.f1= d; _temp268.f2= s; _temp268;}); _temp267;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl*
d= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp271=(
struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp271[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp272; _temp272.tag= Cyc_Absyn_Var_d;
_temp272.f1= Cyc_Absyn_new_vardecl( x, t, init); _temp272;}); _temp271;}), loc);
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp269=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp269[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp270; _temp270.tag= Cyc_Absyn_Decl_s;
_temp270.f1= d; _temp270.f2= s; _temp270;}); _temp269;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct* _temp273=(
struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp273[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp274; _temp274.tag= Cyc_Absyn_Cut_s;
_temp274.f1= s; _temp274;}); _temp273;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp275=( struct Cyc_Absyn_Splice_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp275[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp276; _temp276.tag= Cyc_Absyn_Splice_s; _temp276.f1=
s; _temp276;}); _temp275;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp277=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp277[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp278; _temp278.tag= Cyc_Absyn_Label_s;
_temp278.f1= v; _temp278.f2= s; _temp278;}); _temp277;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Do_s_struct*
_temp279=( struct Cyc_Absyn_Do_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp279[ 0]=({ struct Cyc_Absyn_Do_s_struct _temp280; _temp280.tag= Cyc_Absyn_Do_s;
_temp280.f1= s; _temp280.f2=({ struct _tuple2 _temp281; _temp281.f1= e; _temp281.f2=
Cyc_Absyn_skip_stmt( e->loc); _temp281;}); _temp280;}); _temp279;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_TryCatch_s_struct* _temp282=( struct Cyc_Absyn_TryCatch_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct)); _temp282[ 0]=({ struct
Cyc_Absyn_TryCatch_s_struct _temp283; _temp283.tag= Cyc_Absyn_TryCatch_s;
_temp283.f1= s; _temp283.f2= scs; _temp283;}); _temp282;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Goto_s_struct* _temp284=(
struct Cyc_Absyn_Goto_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Goto_s_struct));
_temp284[ 0]=({ struct Cyc_Absyn_Goto_s_struct _temp285; _temp285.tag= Cyc_Absyn_Goto_s;
_temp285.f1= lab; _temp285.f2= 0; _temp285;}); _temp284;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp286=( struct Cyc_Absyn_Pat*)
_cycalloc( sizeof( struct Cyc_Absyn_Pat)); _temp286->r=( void*) p; _temp286->topt=
0; _temp286->loc= s; _temp286;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp287=( struct Cyc_Absyn_Decl*) _cycalloc( sizeof( struct Cyc_Absyn_Decl));
_temp287->r=( void*) r; _temp287->loc= loc; _temp287;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Let_d_struct* _temp288=( struct Cyc_Absyn_Let_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp288[ 0]=({ struct Cyc_Absyn_Let_d_struct
_temp289; _temp289.tag= Cyc_Absyn_Let_d; _temp289.f1= p; _temp289.f2= 0;
_temp289.f3= t_opt; _temp289.f4= e; _temp289.f5= 0; _temp289;}); _temp288;}),
loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List* vds,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(( void*)({ struct
Cyc_Absyn_Letv_d_struct* _temp290=( struct Cyc_Absyn_Letv_d_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Letv_d_struct)); _temp290[ 0]=({ struct Cyc_Absyn_Letv_d_struct
_temp291; _temp291.tag= Cyc_Absyn_Letv_d; _temp291.f1= vds; _temp291;});
_temp290;}), loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct
_tuple0* x, void* t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl*
_temp292=( struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp292->sc=( void*)(( void*) Cyc_Absyn_Public); _temp292->name= x; _temp292->tq=
Cyc_Absyn_empty_tqual(); _temp292->type=( void*) t; _temp292->initializer= init;
_temp292->rgn= 0; _temp292->attributes= 0; _temp292;});} struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){
return({ struct Cyc_Absyn_Vardecl* _temp293=( struct Cyc_Absyn_Vardecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp293->sc=( void*)(( void*)
Cyc_Absyn_Static); _temp293->name= x; _temp293->tq= Cyc_Absyn_empty_tqual();
_temp293->type=( void*) t; _temp293->initializer= init; _temp293->rgn= 0;
_temp293->attributes= 0; _temp293;});} struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp294=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp294[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp295; _temp295.tag= Cyc_Absyn_Struct_d; _temp295.f1=({
struct Cyc_Absyn_Structdecl* _temp296=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp296->sc=( void*) s; _temp296->name=
n; _temp296->tvs= ts; _temp296->fields= fs; _temp296->attributes= atts; _temp296;});
_temp295;}); _temp294;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp297=( struct Cyc_Absyn_Union_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp297[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp298; _temp298.tag= Cyc_Absyn_Union_d; _temp298.f1=({ struct Cyc_Absyn_Uniondecl*
_temp299=( struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp299->sc=( void*) s; _temp299->name= n; _temp299->tvs= ts; _temp299->fields=
fs; _temp299->attributes= atts; _temp299;}); _temp298;}); _temp297;}), loc);}
struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct
Cyc_List_List* ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp300=( struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp300[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp301; _temp301.tag= Cyc_Absyn_Tunion_d;
_temp301.f1=({ struct Cyc_Absyn_Tuniondecl* _temp302=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp302->sc=( void*) s;
_temp302->name= n; _temp302->tvs= ts; _temp302->fields= fs; _temp302->is_xtunion=
is_xtunion; _temp302;}); _temp301;}); _temp300;}), loc);} static struct _tuple1*
Cyc_Absyn_expand_arg( struct _tuple1* a){ return({ struct _tuple1* _temp303=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp303->f1=(* a).f1;
_temp303->f2=(* a).f2; _temp303->f3= Cyc_Absyn_pointer_expand((* a).f3);
_temp303;});} void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_List_List* rgn_po, struct
Cyc_List_List* atts){ return( void*)({ struct Cyc_Absyn_FnType_struct* _temp304=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp304[ 0]=({ struct Cyc_Absyn_FnType_struct _temp305; _temp305.tag= Cyc_Absyn_FnType;
_temp305.f1=({ struct Cyc_Absyn_FnInfo _temp306; _temp306.tvars= tvs; _temp306.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp306.effect= eff_typ; _temp306.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp306.c_varargs= c_varargs;
_temp306.cyc_varargs= cyc_varargs; _temp306.rgn_po= rgn_po; _temp306.attributes=
atts; _temp306;}); _temp305;}); _temp304;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp307= Cyc_Tcutil_compress( t); _LL309: if(( unsigned int)
_temp307 >  4u?*(( int*) _temp307) ==  Cyc_Absyn_FnType: 0){ goto _LL310;} else{
goto _LL311;} _LL311: goto _LL312; _LL310: return Cyc_Absyn_at_typ( t,( void*)
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _LL312: return t; _LL308:;} int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp* e){ void* _temp313=( void*) e->r; void* _temp337; void*
_temp339; struct Cyc_Absyn_Vardecl* _temp341; void* _temp343; struct Cyc_Absyn_Vardecl*
_temp345; struct Cyc_Absyn_Exp* _temp347; struct Cyc_Absyn_Exp* _temp349; struct
_tagged_arr* _temp351; struct Cyc_Absyn_Exp* _temp353; struct _tagged_arr*
_temp355; struct Cyc_Absyn_Exp* _temp357; struct Cyc_Absyn_Exp* _temp359; struct
Cyc_Absyn_Exp* _temp361; struct Cyc_Absyn_Exp* _temp363; _LL315: if(*(( int*)
_temp313) ==  Cyc_Absyn_Var_e){ _LL338: _temp337=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp313)->f2; if(( unsigned int) _temp337 >  1u?*(( int*) _temp337) ==  Cyc_Absyn_Funname_b:
0){ goto _LL316;} else{ goto _LL317;}} else{ goto _LL317;} _LL317: if(*(( int*)
_temp313) ==  Cyc_Absyn_Var_e){ _LL340: _temp339=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp313)->f2; if(( unsigned int) _temp339 >  1u?*(( int*) _temp339) ==  Cyc_Absyn_Global_b:
0){ _LL342: _temp341=(( struct Cyc_Absyn_Global_b_struct*) _temp339)->f1; goto
_LL318;} else{ goto _LL319;}} else{ goto _LL319;} _LL319: if(*(( int*) _temp313)
==  Cyc_Absyn_Var_e){ _LL344: _temp343=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp313)->f2; if(( unsigned int) _temp343 >  1u?*(( int*) _temp343) ==  Cyc_Absyn_Local_b:
0){ _LL346: _temp345=(( struct Cyc_Absyn_Local_b_struct*) _temp343)->f1; goto
_LL320;} else{ goto _LL321;}} else{ goto _LL321;} _LL321: if(*(( int*) _temp313)
==  Cyc_Absyn_Var_e){ goto _LL322;} else{ goto _LL323;} _LL323: if(*(( int*)
_temp313) ==  Cyc_Absyn_Subscript_e){ _LL350: _temp349=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp313)->f1; goto _LL348; _LL348: _temp347=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp313)->f2; goto _LL324;} else{ goto _LL325;} _LL325: if(*(( int*) _temp313)
==  Cyc_Absyn_StructMember_e){ _LL354: _temp353=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp313)->f1; goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp313)->f2; goto _LL326;} else{ goto _LL327;} _LL327: if(*(( int*) _temp313)
==  Cyc_Absyn_StructArrow_e){ _LL358: _temp357=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp313)->f1; goto _LL356; _LL356: _temp355=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp313)->f2; goto _LL328;} else{ goto _LL329;} _LL329: if(*(( int*) _temp313)
==  Cyc_Absyn_Deref_e){ _LL360: _temp359=(( struct Cyc_Absyn_Deref_e_struct*)
_temp313)->f1; goto _LL330;} else{ goto _LL331;} _LL331: if(*(( int*) _temp313)
==  Cyc_Absyn_Instantiate_e){ _LL362: _temp361=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp313)->f1; goto _LL332;} else{ goto _LL333;} _LL333: if(*(( int*) _temp313)
==  Cyc_Absyn_NoInstantiate_e){ _LL364: _temp363=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp313)->f1; goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336; _LL316:
return 0; _LL318: _temp345= _temp341; goto _LL320; _LL320: { void* _temp365= Cyc_Tcutil_compress((
void*) _temp345->type); _LL367: if(( unsigned int) _temp365 >  4u?*(( int*)
_temp365) ==  Cyc_Absyn_ArrayType: 0){ goto _LL368;} else{ goto _LL369;} _LL369:
goto _LL370; _LL368: return 0; _LL370: return 1; _LL366:;} _LL322: return 1;
_LL324: return 1; _LL326: return Cyc_Absyn_is_lvalue( _temp353); _LL328: return
1; _LL330: return 1; _LL332: return Cyc_Absyn_is_lvalue( _temp361); _LL334:
return Cyc_Absyn_is_lvalue( _temp363); _LL336: return 0; _LL314:;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List* fields, struct _tagged_arr* v){{
struct Cyc_List_List* fs= fields; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Structfield* _temp371=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; if( Cyc_Std_zstrptrcmp( _temp371->name,
v) ==  0){ return( struct Cyc_Absyn_Structfield*) _temp371;}}} return 0;} struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*
sd, struct _tagged_arr* v){ return sd->fields ==  0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v, v);}
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*
ud, struct _tagged_arr* v){ return ud->fields ==  0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v, v);}
struct _tuple3* Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List* ts, int i){
for( 0; i !=  0; -- i){ if( ts ==  0){ return 0;} ts=(( struct Cyc_List_List*)
_check_null( ts))->tl;} if( ts ==  0){ return 0;} return( struct _tuple3*)((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( ts))->hd);} struct
_tagged_arr Cyc_Absyn_attribute2string( void* a){ void* _temp372= a; int
_temp416; int _temp418; struct _tagged_arr _temp420; int _temp422; int _temp424;
void* _temp426; int _temp428; int _temp430; void* _temp432; _LL374: if((
unsigned int) _temp372 >  16u?*(( int*) _temp372) ==  Cyc_Absyn_Regparm_att: 0){
_LL417: _temp416=(( struct Cyc_Absyn_Regparm_att_struct*) _temp372)->f1; goto
_LL375;} else{ goto _LL376;} _LL376: if( _temp372 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL377;} else{ goto _LL378;} _LL378: if( _temp372 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL379;} else{ goto _LL380;} _LL380: if( _temp372 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL381;} else{ goto _LL382;} _LL382: if( _temp372 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL383;} else{ goto _LL384;} _LL384: if( _temp372 == ( void*) Cyc_Absyn_Const_att){
goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp372 >  16u?*((
int*) _temp372) ==  Cyc_Absyn_Aligned_att: 0){ _LL419: _temp418=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp372)->f1; goto _LL387;} else{ goto _LL388;} _LL388: if( _temp372 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL389;} else{ goto _LL390;} _LL390: if((
unsigned int) _temp372 >  16u?*(( int*) _temp372) ==  Cyc_Absyn_Section_att: 0){
_LL421: _temp420=(( struct Cyc_Absyn_Section_att_struct*) _temp372)->f1; goto
_LL391;} else{ goto _LL392;} _LL392: if( _temp372 == ( void*) Cyc_Absyn_Nocommon_att){
goto _LL393;} else{ goto _LL394;} _LL394: if( _temp372 == ( void*) Cyc_Absyn_Shared_att){
goto _LL395;} else{ goto _LL396;} _LL396: if( _temp372 == ( void*) Cyc_Absyn_Unused_att){
goto _LL397;} else{ goto _LL398;} _LL398: if( _temp372 == ( void*) Cyc_Absyn_Weak_att){
goto _LL399;} else{ goto _LL400;} _LL400: if( _temp372 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL401;} else{ goto _LL402;} _LL402: if( _temp372 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL403;} else{ goto _LL404;} _LL404: if( _temp372 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL405;} else{ goto _LL406;} _LL406: if( _temp372 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp372 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp372 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL411;} else{ goto _LL412;} _LL412: if(( unsigned int) _temp372 >  16u?*((
int*) _temp372) ==  Cyc_Absyn_Format_att: 0){ _LL427: _temp426=( void*)(( struct
Cyc_Absyn_Format_att_struct*) _temp372)->f1; if( _temp426 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL425;} else{ goto _LL414;} _LL425: _temp424=(( struct Cyc_Absyn_Format_att_struct*)
_temp372)->f2; goto _LL423; _LL423: _temp422=(( struct Cyc_Absyn_Format_att_struct*)
_temp372)->f3; goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int)
_temp372 >  16u?*(( int*) _temp372) ==  Cyc_Absyn_Format_att: 0){ _LL433:
_temp432=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp372)->f1; if(
_temp432 == ( void*) Cyc_Absyn_Scanf_ft){ goto _LL431;} else{ goto _LL373;}
_LL431: _temp430=(( struct Cyc_Absyn_Format_att_struct*) _temp372)->f2; goto
_LL429; _LL429: _temp428=(( struct Cyc_Absyn_Format_att_struct*) _temp372)->f3;
goto _LL415;} else{ goto _LL373;} _LL375: return( struct _tagged_arr)({ struct
Cyc_Std_Int_pa_struct _temp435; _temp435.tag= Cyc_Std_Int_pa; _temp435.f1=( int)((
unsigned int) _temp416);{ void* _temp434[ 1u]={& _temp435}; Cyc_Std_aprintf(
_tag_arr("regparm(%d)", sizeof( unsigned char), 12u), _tag_arr( _temp434,
sizeof( void*), 1u));}}); _LL377: return _tag_arr("stdcall", sizeof(
unsigned char), 8u); _LL379: return _tag_arr("cdecl", sizeof( unsigned char), 6u);
_LL381: return _tag_arr("fastcall", sizeof( unsigned char), 9u); _LL383: return
_tag_arr("noreturn", sizeof( unsigned char), 9u); _LL385: return _tag_arr("const",
sizeof( unsigned char), 6u); _LL387: if( _temp418 ==  - 1){ return _tag_arr("aligned",
sizeof( unsigned char), 8u);} else{ return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp437; _temp437.tag= Cyc_Std_Int_pa; _temp437.f1=( int)(( unsigned int)
_temp418);{ void* _temp436[ 1u]={& _temp437}; Cyc_Std_aprintf( _tag_arr("aligned(%d)",
sizeof( unsigned char), 12u), _tag_arr( _temp436, sizeof( void*), 1u));}});}
_LL389: return _tag_arr("packed", sizeof( unsigned char), 7u); _LL391: return(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp439; _temp439.tag=
Cyc_Std_String_pa; _temp439.f1=( struct _tagged_arr) _temp420;{ void* _temp438[
1u]={& _temp439}; Cyc_Std_aprintf( _tag_arr("section(\"%s\")", sizeof(
unsigned char), 14u), _tag_arr( _temp438, sizeof( void*), 1u));}}); _LL393:
return _tag_arr("nocommon", sizeof( unsigned char), 9u); _LL395: return _tag_arr("shared",
sizeof( unsigned char), 7u); _LL397: return _tag_arr("unused", sizeof(
unsigned char), 7u); _LL399: return _tag_arr("weak", sizeof( unsigned char), 5u);
_LL401: return _tag_arr("dllimport", sizeof( unsigned char), 10u); _LL403:
return _tag_arr("dllexport", sizeof( unsigned char), 10u); _LL405: return
_tag_arr("no_instrument_function", sizeof( unsigned char), 23u); _LL407: return
_tag_arr("constructor", sizeof( unsigned char), 12u); _LL409: return _tag_arr("destructor",
sizeof( unsigned char), 11u); _LL411: return _tag_arr("no_check_memory_usage",
sizeof( unsigned char), 22u); _LL413: return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp442; _temp442.tag= Cyc_Std_Int_pa; _temp442.f1=( unsigned int) _temp422;{
struct Cyc_Std_Int_pa_struct _temp441; _temp441.tag= Cyc_Std_Int_pa; _temp441.f1=(
unsigned int) _temp424;{ void* _temp440[ 2u]={& _temp441,& _temp442}; Cyc_Std_aprintf(
_tag_arr("format(printf,%u,%u)", sizeof( unsigned char), 21u), _tag_arr(
_temp440, sizeof( void*), 2u));}}}); _LL415: return( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp445; _temp445.tag= Cyc_Std_Int_pa; _temp445.f1=(
unsigned int) _temp428;{ struct Cyc_Std_Int_pa_struct _temp444; _temp444.tag=
Cyc_Std_Int_pa; _temp444.f1=( unsigned int) _temp430;{ void* _temp443[ 2u]={&
_temp444,& _temp445}; Cyc_Std_aprintf( _tag_arr("format(scanf,%u,%u)", sizeof(
unsigned char), 20u), _tag_arr( _temp443, sizeof( void*), 2u));}}}); _LL373:;}
int Cyc_Absyn_fntype_att( void* a){ void* _temp446= a; _LL448: if(( unsigned int)
_temp446 >  16u?*(( int*) _temp446) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL449;}
else{ goto _LL450;} _LL450: if( _temp446 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL451;} else{ goto _LL452;} _LL452: if( _temp446 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp446 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL455;} else{ goto _LL456;} _LL456: if( _temp446 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int) _temp446 >  16u?*((
int*) _temp446) ==  Cyc_Absyn_Format_att: 0){ goto _LL459;} else{ goto _LL460;}
_LL460: if( _temp446 == ( void*) Cyc_Absyn_Const_att){ goto _LL461;} else{ goto
_LL462;} _LL462: goto _LL463; _LL449: goto _LL451; _LL451: goto _LL453; _LL453:
goto _LL455; _LL455: goto _LL457; _LL457: goto _LL459; _LL459: goto _LL461;
_LL461: return 1; _LL463: return 0; _LL447:;}
