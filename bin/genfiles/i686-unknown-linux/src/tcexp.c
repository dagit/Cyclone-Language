 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct
_tuple0*); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct
Cyc_List_List* x, int i); extern struct Cyc_List_List* Cyc_List_nth_tail( struct
Cyc_List_List* x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void*
Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
extern int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern struct _tagged_arr Cyc_Std_vrprintf( struct _RegionHandle* r1, struct
_tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict*
typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; }
; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0; static const
int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2; static const
int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct
Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); struct
_tuple4{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; struct _tuple5{
struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple4*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, int
allow_evars, void*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); extern
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int
new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*); extern
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct
Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; static void* Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr
msg, struct _tagged_arr ap){({ void* _temp0[ 0u]={}; Cyc_Tcutil_terr( loc,(
struct _tagged_arr) Cyc_Std_vrprintf( Cyc_Core_heap_region, msg, ap), _tag_arr(
_temp0, sizeof( void*), 0u));}); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt*
_temp1=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp1;}));} static struct Cyc_Core_Opt
Cyc_Tcexp_rk={( void*)(( void*) 3u)}; static struct Cyc_Core_Opt Cyc_Tcexp_ak={(
void*)(( void*) 0u)}; static struct Cyc_Core_Opt Cyc_Tcexp_bk={( void*)(( void*)
2u)}; static struct Cyc_Core_Opt Cyc_Tcexp_mk={( void*)(( void*) 1u)}; static
void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp2; _push_handler(& _temp2);{
int _temp4= 0; if( setjmp( _temp2.handler)){ _temp4= 1;} if( ! _temp4){{ void*
_temp5= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void* _temp15; struct Cyc_Absyn_Enumfield*
_temp17; struct Cyc_Absyn_Enumdecl* _temp19; struct Cyc_Absyn_Tunionfield*
_temp21; struct Cyc_Absyn_Tuniondecl* _temp23; _LL7: if(*(( int*) _temp5) == 
Cyc_Tcenv_VarRes){ _LL16: _temp15=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9: if(*(( int*) _temp5) ==  Cyc_Tcenv_EnumRes){
_LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*) _temp5)->f1; goto _LL18;
_LL18: _temp17=(( struct Cyc_Tcenv_EnumRes_struct*) _temp5)->f2; goto _LL10;}
else{ goto _LL11;} _LL11: if(*(( int*) _temp5) ==  Cyc_Tcenv_TunionRes){ _LL24:
_temp23=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL22; _LL22:
_temp21=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL12;} else{
goto _LL13;} _LL13: if(*(( int*) _temp5) ==  Cyc_Tcenv_StructRes){ goto _LL14;}
else{ goto _LL6;} _LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp25=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp25[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp26; _temp26.tag= Cyc_Absyn_Var_e;
_temp26.f1= q; _temp26.f2=( void*) _temp15; _temp26;}); _temp25;}))); goto _LL6;
_LL10:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp27=(
struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp27[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp28; _temp28.tag= Cyc_Absyn_Enum_e;
_temp28.f1= q; _temp28.f2=( struct Cyc_Absyn_Enumdecl*) _temp19; _temp28.f3=(
struct Cyc_Absyn_Enumfield*) _temp17; _temp28;}); _temp27;}))); goto _LL6; _LL12:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp29=(
struct Cyc_Absyn_Tunion_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp30; _temp30.tag= Cyc_Absyn_Tunion_e;
_temp30.f1= 0; _temp30.f2= 0; _temp30.f3= 0; _temp30.f4= _temp23; _temp30.f5=
_temp21; _temp30;}); _temp29;}))); goto _LL6; _LL14:({ struct Cyc_Std_String_pa_struct
_temp32; _temp32.tag= Cyc_Std_String_pa; _temp32.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp31[ 1u]={& _temp32}; Cyc_Tcutil_terr( e->loc, _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u), _tag_arr( _temp31, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp33=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp34; _temp34.tag= Cyc_Absyn_Var_e; _temp34.f1= q; _temp34.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp34;}); _temp33;}))); goto _LL6; _LL6:;};
_pop_handler();} else{ void* _temp3=( void*) _exn_thrown; void* _temp36= _temp3;
_LL38: if( _temp36 ==  Cyc_Dict_Absent){ goto _LL39;} else{ goto _LL40;} _LL40:
goto _LL41; _LL39:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp42=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp43; _temp43.tag= Cyc_Absyn_Var_e;
_temp43.f1= q; _temp43.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp43;});
_temp42;}))); goto _LL37; _LL41:( void) _throw( _temp36); _LL37:;}}} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple7* _temp44=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp44->f1=
0; _temp44->f2= e; _temp44;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp45=( void*) e1->r; struct _tuple1* _temp51; _LL47:
if(*(( int*) _temp45) ==  Cyc_Absyn_UnknownId_e){ _LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct
_handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp(
_temp53.handler)){ _temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68;
struct Cyc_Absyn_Tuniondecl* _temp70; struct Cyc_Absyn_Structdecl* _temp72;
_LL58: if(*(( int*) _temp56) ==  Cyc_Tcenv_VarRes){ _LL67: _temp66=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto _LL59;} else{ goto _LL60;}
_LL60: if(*(( int*) _temp56) ==  Cyc_Tcenv_TunionRes){ _LL71: _temp70=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69; _LL69: _temp68=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;} else{ goto _LL62;} _LL62:
if(*(( int*) _temp56) ==  Cyc_Tcenv_StructRes){ _LL73: _temp72=(( struct Cyc_Tcenv_StructRes_struct*)
_temp56)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp56) == 
Cyc_Tcenv_EnumRes){ goto _LL65;} else{ goto _LL57;} _LL59:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp74=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp74[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp75; _temp75.tag= Cyc_Absyn_FnCall_e; _temp75.f1= e1; _temp75.f2= es;
_temp75.f3= 0; _temp75;}); _temp74;}))); _npop_handler( 0u); return; _LL61: if(
_temp68->typs ==  0){({ struct Cyc_Std_String_pa_struct _temp77; _temp77.tag=
Cyc_Std_String_pa; _temp77.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp68->name);{ void* _temp76[ 1u]={& _temp77}; Cyc_Tcutil_terr( e->loc,
_tag_arr("%s is a constant, not a function", sizeof( unsigned char), 33u),
_tag_arr( _temp76, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp78=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp78[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp79; _temp79.tag= Cyc_Absyn_Tunion_e; _temp79.f1= 0; _temp79.f2= 0; _temp79.f3=
es; _temp79.f4= _temp70; _temp79.f5= _temp68; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL63: { struct Cyc_List_List*
_temp80=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp72->name !=  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp81=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp82; _temp82.tag= Cyc_Absyn_Struct_e;
_temp82.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp72->name))->v;
_temp82.f2= 0; _temp82.f3= _temp80; _temp82.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp82;}); _temp81;})));} else{({ void* _temp83[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("missing struct name", sizeof( unsigned char), 20u), _tag_arr(
_temp83, sizeof( void*), 0u));});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp84=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp85; _temp85.tag= Cyc_Absyn_Struct_e;
_temp85.f1= _temp51; _temp85.f2= 0; _temp85.f3= _temp80; _temp85.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp85;}); _temp84;})));} _npop_handler( 0u); return;} _LL65:({ struct
Cyc_Std_String_pa_struct _temp87; _temp87.tag= Cyc_Std_String_pa; _temp87.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp51);{ void* _temp86[ 1u]={&
_temp87}; Cyc_Tcutil_terr( e->loc, _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u), _tag_arr( _temp86, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL57:;}; _pop_handler();} else{ void* _temp54=(
void*) _exn_thrown; void* _temp89= _temp54; _LL91: if( _temp89 ==  Cyc_Dict_Absent){
goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL92:({ struct Cyc_Std_String_pa_struct
_temp96; _temp96.tag= Cyc_Std_String_pa; _temp96.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp51);{ void* _temp95[ 1u]={& _temp96}; Cyc_Tcutil_terr( e1->loc, _tag_arr("unbound identifier %s",
sizeof( unsigned char), 22u), _tag_arr( _temp95, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp97=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp97[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp98; _temp98.tag= Cyc_Absyn_FnCall_e; _temp98.f1= e1; _temp98.f2= es;
_temp98.f3= 0; _temp98;}); _temp97;}))); return; _LL94:( void) _throw( _temp89);
_LL90:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp99=( struct Cyc_Absyn_FnCall_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp99[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp100; _temp100.tag= Cyc_Absyn_FnCall_e;
_temp100.f1= e1; _temp100.f2= es; _temp100.f3= 0; _temp100;}); _temp99;})));
return; _LL46:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt ==  0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp101=( struct Cyc_Absyn_Array_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp101[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp102; _temp102.tag= Cyc_Absyn_Array_e; _temp102.f1= des; _temp102;});
_temp101;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp103=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp113; void* _temp115; struct
Cyc_Absyn_Structdecl** _temp117; struct Cyc_List_List* _temp119; struct _tuple1*
_temp121; _LL105: if(( unsigned int) _temp103 >  4u?*(( int*) _temp103) ==  Cyc_Absyn_ArrayType:
0){ _LL116: _temp115=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f1;
goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 >  4u?*((
int*) _temp103) ==  Cyc_Absyn_StructType: 0){ _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f1; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 >  4u?*(( int*) _temp103) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL110;}
else{ goto _LL111;} _LL111: goto _LL112; _LL106:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp123=( struct Cyc_Absyn_Array_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp123[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp124; _temp124.tag= Cyc_Absyn_Array_e; _temp124.f1= des; _temp124;});
_temp123;}))); goto _LL104; _LL108: if( _temp117 ==  0? 1: _temp121 ==  0){({
void* _temp125[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u), _tag_arr( _temp125, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp126=(
struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp126[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp127; _temp127.tag= Cyc_Absyn_Struct_e;
_temp127.f1=( struct _tuple1*) _check_null( _temp121); _temp127.f2= 0; _temp127.f3=
des; _temp127.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp117)); _temp127;}); _temp126;}))); goto _LL104; _LL110:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp128=( struct
Cyc_Absyn_AnonStruct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp128[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp129; _temp129.tag= Cyc_Absyn_AnonStruct_e;
_temp129.f1=( void*) t; _temp129.f2= des; _temp129;}); _temp128;}))); goto
_LL104; _LL112:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp130=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp130[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp131; _temp131.tag= Cyc_Absyn_Array_e;
_temp131.f1= des; _temp131;}); _temp130;}))); goto _LL104; _LL104:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ struct Cyc_Std_String_pa_struct _temp134; _temp134.tag= Cyc_Std_String_pa;
_temp134.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp133; _temp133.tag=
Cyc_Std_String_pa; _temp133.f1=( struct _tagged_arr) msg_part;{ void* _temp132[
2u]={& _temp133,& _temp134}; Cyc_Tcutil_terr( e->loc, _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u), _tag_arr( _temp132, sizeof( void*), 2u));}}});}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp135= c;
void* _temp151; void* _temp153; void* _temp155; int _temp157; void* _temp159;
struct _tagged_arr _temp161; _LL137: if(( unsigned int) _temp135 >  1u?*(( int*)
_temp135) ==  Cyc_Absyn_Char_c: 0){ _LL152: _temp151=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp135)->f1; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_Short_c: 0){ _LL154: _temp153=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp135)->f1; goto _LL140;} else{
goto _LL141;} _LL141: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135) == 
Cyc_Absyn_LongLong_c: 0){ _LL156: _temp155=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp135)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_Float_c: 0){ goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135)
==  Cyc_Absyn_Int_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_String_c: 0){ _LL162: _temp161=((
struct Cyc_Absyn_String_c_struct*) _temp135)->f1; goto _LL148;} else{ goto
_LL149;} _LL149: if( _temp135 == ( void*) Cyc_Absyn_Null_c){ goto _LL150;} else{
goto _LL136;} _LL138: t= _temp151 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL136; _LL140: t= _temp153 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL136; _LL142: t= _temp155 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL136; _LL144: t= Cyc_Absyn_float_t; goto _LL136; _LL146: if( topt ==  0){ t=
_temp159 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp163= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); void*
_temp173; void* _temp175; void* _temp177; void* _temp179; void* _temp181; _LL165:
if(( unsigned int) _temp163 >  4u?*(( int*) _temp163) ==  Cyc_Absyn_IntType: 0){
_LL176: _temp175=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp163)->f1;
goto _LL174; _LL174: _temp173=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp173 == ( void*) Cyc_Absyn_B1){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int) _temp163 >  4u?*(( int*)
_temp163) ==  Cyc_Absyn_IntType: 0){ _LL180: _temp179=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL178; _LL178: _temp177=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp177 == ( void*) Cyc_Absyn_B2){ goto _LL168;} else{ goto
_LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int) _temp163 >  4u?*(( int*)
_temp163) ==  Cyc_Absyn_IntType: 0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL170;} else{ goto _LL171;} _LL171: goto _LL172; _LL166: t=
_temp175 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp183=(
struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp184; _temp184.tag= Cyc_Absyn_Const_e;
_temp184.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp185=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp185[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp186; _temp186.tag= Cyc_Absyn_Char_c;
_temp186.f1=( void*) _temp175; _temp186.f2=( unsigned char) _temp157; _temp186;});
_temp185;})); _temp184;}); _temp183;}))); goto _LL164; _LL168: t= _temp179 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp187=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp187[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp188; _temp188.tag= Cyc_Absyn_Const_e; _temp188.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp189=( struct Cyc_Absyn_Short_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp189[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp190; _temp190.tag= Cyc_Absyn_Short_c; _temp190.f1=( void*) _temp179;
_temp190.f2=( short) _temp157; _temp190;}); _temp189;})); _temp188;}); _temp187;})));
goto _LL164; _LL170: t= _temp181 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL164; _LL172: t= _temp159 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL164; _LL164:;} goto _LL136; _LL148: {
int len=( int) _get_arr_size( _temp161, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp191=({ struct Cyc_Absyn_Const_e_struct* _temp228=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp228[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp229; _temp229.tag= Cyc_Absyn_Const_e; _temp229.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp230=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp230[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp231; _temp231.tag= Cyc_Absyn_Int_c; _temp231.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp231.f2= len; _temp231;}); _temp230;})); _temp229;}); _temp228;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp191, loc); elen->topt=({
struct Cyc_Core_Opt* _temp192=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp192->v=( void*) Cyc_Absyn_uint_t; _temp192;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp193=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp193[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp194; _temp194.tag= Cyc_Absyn_Upper_b; _temp194.f1= elen; _temp194;});
_temp193;})); if( topt !=  0){ void* _temp195= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp203; struct Cyc_Absyn_Tqual
_temp205; void* _temp207; struct Cyc_Absyn_PtrInfo _temp209; struct Cyc_Absyn_Conref*
_temp211; struct Cyc_Absyn_Tqual _temp213; struct Cyc_Absyn_Conref* _temp215;
void* _temp217; void* _temp219; _LL197: if(( unsigned int) _temp195 >  4u?*((
int*) _temp195) ==  Cyc_Absyn_ArrayType: 0){ _LL208: _temp207=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp195)->f1; goto _LL206; _LL206: _temp205=((
struct Cyc_Absyn_ArrayType_struct*) _temp195)->f2; goto _LL204; _LL204: _temp203=((
struct Cyc_Absyn_ArrayType_struct*) _temp195)->f3; goto _LL198;} else{ goto
_LL199;} _LL199: if(( unsigned int) _temp195 >  4u?*(( int*) _temp195) ==  Cyc_Absyn_PointerType:
0){ _LL210: _temp209=(( struct Cyc_Absyn_PointerType_struct*) _temp195)->f1;
_LL220: _temp219=( void*) _temp209.elt_typ; goto _LL218; _LL218: _temp217=( void*)
_temp209.rgn_typ; goto _LL216; _LL216: _temp215= _temp209.nullable; goto _LL214;
_LL214: _temp213= _temp209.tq; goto _LL212; _LL212: _temp211= _temp209.bounds;
goto _LL200;} else{ goto _LL201;} _LL201: goto _LL202; _LL198: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp221=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp221[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp222; _temp222.tag= Cyc_Absyn_ArrayType; _temp222.f1=(
void*) Cyc_Absyn_uchar_t; _temp222.f2= _temp205; _temp222.f3=( struct Cyc_Absyn_Exp*)
elen; _temp222;}); _temp221;}); _LL200: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp223=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp223->v=( void*) t; _temp223;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp224=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp224->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp224;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp225=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp226; _temp226.tag= Cyc_Absyn_Upper_b; _temp226.f1= elen; _temp226;});
_temp225;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp227=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp227->v=( void*) t; _temp227;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL196; _LL202:
goto _LL196; _LL196:;} return t;} _LL150: { struct Cyc_List_List* _temp232= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp233=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp233[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp237=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=( void*)
_temp232; _temp237;})); _temp235.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*) _temp232;
_temp236;})); _temp235.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp235.tq= Cyc_Absyn_empty_tqual(); _temp235.bounds= Cyc_Absyn_empty_conref();
_temp235;}); _temp234;}); _temp233;}); goto _LL136;} _LL136:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp238= b; struct Cyc_Absyn_Vardecl*
_temp252; struct Cyc_Absyn_Fndecl* _temp254; struct Cyc_Absyn_Vardecl* _temp256;
struct Cyc_Absyn_Vardecl* _temp258; struct Cyc_Absyn_Vardecl* _temp260; _LL240:
if( _temp238 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL241;} else{ goto
_LL242;} _LL242: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Global_b:
0){ _LL253: _temp252=(( struct Cyc_Absyn_Global_b_struct*) _temp238)->f1; goto
_LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp238 >  1u?*(( int*)
_temp238) ==  Cyc_Absyn_Funname_b: 0){ _LL255: _temp254=(( struct Cyc_Absyn_Funname_b_struct*)
_temp238)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Pat_b: 0){ _LL257: _temp256=((
struct Cyc_Absyn_Pat_b_struct*) _temp238)->f1; goto _LL247;} else{ goto _LL248;}
_LL248: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Local_b:
0){ _LL259: _temp258=(( struct Cyc_Absyn_Local_b_struct*) _temp238)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp238 >  1u?*(( int*)
_temp238) ==  Cyc_Absyn_Param_b: 0){ _LL261: _temp260=(( struct Cyc_Absyn_Param_b_struct*)
_temp238)->f1; goto _LL251;} else{ goto _LL239;} _LL241: return({ struct Cyc_Std_String_pa_struct
_temp263; _temp263.tag= Cyc_Std_String_pa; _temp263.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp262[ 1u]={& _temp263}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("unbound identifier %s",
sizeof( unsigned char), 22u), _tag_arr( _temp262, sizeof( void*), 1u));}});
_LL243: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp252->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp252->type; _LL245:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp254->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp254);
_LL247: _temp258= _temp256; goto _LL249; _LL249: _temp260= _temp258; goto _LL251;
_LL251:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp260->type; _LL239:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp264=( void*) fmt->r; void* _temp272; struct _tagged_arr
_temp274; struct Cyc_Absyn_Exp* _temp276; struct Cyc_Absyn_Exp _temp278; void*
_temp279; void* _temp281; struct _tagged_arr _temp283; _LL266: if(*(( int*)
_temp264) ==  Cyc_Absyn_Const_e){ _LL273: _temp272=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp264)->f1; if(( unsigned int) _temp272 >  1u?*(( int*) _temp272) ==  Cyc_Absyn_String_c:
0){ _LL275: _temp274=(( struct Cyc_Absyn_String_c_struct*) _temp272)->f1; goto
_LL267;} else{ goto _LL268;}} else{ goto _LL268;} _LL268: if(*(( int*) _temp264)
==  Cyc_Absyn_Cast_e){ _LL277: _temp276=(( struct Cyc_Absyn_Cast_e_struct*)
_temp264)->f2; _temp278=* _temp276; _LL280: _temp279=( void*) _temp278.r; if(*((
int*) _temp279) ==  Cyc_Absyn_Const_e){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp279)->f1; if(( unsigned int) _temp281 >  1u?*(( int*) _temp281) ==  Cyc_Absyn_String_c:
0){ _LL284: _temp283=(( struct Cyc_Absyn_String_c_struct*) _temp281)->f1; goto
_LL269;} else{ goto _LL270;}} else{ goto _LL270;}} else{ goto _LL270;} _LL270:
goto _LL271; _LL267: _temp283= _temp274; goto _LL269; _LL269: desc_types=
type_getter( te,( struct _tagged_arr) _temp283, fmt->loc); goto _LL265; _LL271:
return; _LL265:;} if( opt_args !=  0){ struct Cyc_List_List* _temp285=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0;
desc_types !=  0? _temp285 !=  0: 0;( desc_types=(( struct Cyc_List_List*)
_check_null( desc_types))->tl, _temp285=(( struct Cyc_List_List*) _check_null(
_temp285))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp285))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct
Cyc_Std_String_pa_struct _temp288; _temp288.tag= Cyc_Std_String_pa; _temp288.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp287; _temp287.tag=
Cyc_Std_String_pa; _temp287.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp286[ 2u]={& _temp287,& _temp288}; Cyc_Tcutil_terr( e->loc, _tag_arr("descriptor has type %s but argument has type %s",
sizeof( unsigned char), 48u), _tag_arr( _temp286, sizeof( void*), 2u));}}});}}
if( desc_types !=  0){({ void* _temp289[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp289,
sizeof( void*), 0u));});} if( _temp285 !=  0){({ void* _temp290[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp285))->hd)->loc,
_tag_arr("too many arguments", sizeof( unsigned char), 19u), _tag_arr( _temp290,
sizeof( void*), 0u));});}}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp291= p; _LL293: if( _temp291 == ( void*) Cyc_Absyn_Plus){ goto _LL294;}
else{ goto _LL295;} _LL295: if( _temp291 == ( void*) Cyc_Absyn_Minus){ goto
_LL296;} else{ goto _LL297;} _LL297: if( _temp291 == ( void*) Cyc_Absyn_Not){
goto _LL298;} else{ goto _LL299;} _LL299: if( _temp291 == ( void*) Cyc_Absyn_Bitnot){
goto _LL300;} else{ goto _LL301;} _LL301: if( _temp291 == ( void*) Cyc_Absyn_Size){
goto _LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL294: goto _LL296;
_LL296: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Std_String_pa_struct
_temp306; _temp306.tag= Cyc_Std_String_pa; _temp306.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp305[ 1u]={& _temp306}; Cyc_Tcutil_terr( loc, _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u), _tag_arr( _temp305, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL298: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Std_String_pa_struct
_temp308; _temp308.tag= Cyc_Std_String_pa; _temp308.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp307[ 1u]={& _temp308}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp307, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL300: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Std_String_pa_struct
_temp310; _temp310.tag= Cyc_Std_String_pa; _temp310.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp309[ 1u]={& _temp310}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp309, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL302:{ void*
_temp311= t; struct Cyc_Absyn_PtrInfo _temp319; _LL313: if(( unsigned int)
_temp311 >  4u?*(( int*) _temp311) ==  Cyc_Absyn_ArrayType: 0){ goto _LL314;}
else{ goto _LL315;} _LL315: if(( unsigned int) _temp311 >  4u?*(( int*) _temp311)
==  Cyc_Absyn_PointerType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_PointerType_struct*)
_temp311)->f1; goto _LL316;} else{ goto _LL317;} _LL317: goto _LL318; _LL314:
goto _LL312; _LL316: goto _LL312; _LL318:({ struct Cyc_Std_String_pa_struct
_temp322; _temp322.tag= Cyc_Std_String_pa; _temp322.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp321[ 1u]={& _temp322}; Cyc_Tcutil_terr( loc, _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u), _tag_arr( _temp321, sizeof( void*), 1u));}}); goto
_LL312; _LL312:;} return Cyc_Absyn_uint_t; _LL304: return({ void* _temp323[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof( unsigned char), 17u),
_tag_arr( _temp323, sizeof( void*), 0u));}); _LL292:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Std_String_pa_struct
_temp325; _temp325.tag= Cyc_Std_String_pa; _temp325.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp324[ 1u]={&
_temp325}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp324, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp326=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp326->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp326;}));} if( ! checker( e2)){({ struct Cyc_Std_String_pa_struct
_temp328; _temp328.tag= Cyc_Std_String_pa; _temp328.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp327[ 1u]={&
_temp328}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp327, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp329=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp329->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp329;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp330= t1; struct
Cyc_Absyn_PtrInfo _temp336; struct Cyc_Absyn_Conref* _temp338; struct Cyc_Absyn_Tqual
_temp340; struct Cyc_Absyn_Conref* _temp342; void* _temp344; void* _temp346;
_LL332: if(( unsigned int) _temp330 >  4u?*(( int*) _temp330) ==  Cyc_Absyn_PointerType:
0){ _LL337: _temp336=(( struct Cyc_Absyn_PointerType_struct*) _temp330)->f1;
_LL347: _temp346=( void*) _temp336.elt_typ; goto _LL345; _LL345: _temp344=( void*)
_temp336.rgn_typ; goto _LL343; _LL343: _temp342= _temp336.nullable; goto _LL341;
_LL341: _temp340= _temp336.tq; goto _LL339; _LL339: _temp338= _temp336.bounds;
goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL333: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Std_String_pa_struct _temp349; _temp349.tag= Cyc_Std_String_pa;
_temp349.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp348[
1u]={& _temp349}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp348, sizeof( void*), 1u));}});}
_temp338= Cyc_Absyn_compress_conref( _temp338);{ void* _temp350=( void*)
_temp338->v; void* _temp358; void* _temp360; struct Cyc_Absyn_Exp* _temp362;
_LL352: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350) ==  Cyc_Absyn_Eq_constr:
0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp350)->f1;
if( _temp358 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL353;} else{ goto _LL354;}}
else{ goto _LL354;} _LL354: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350)
==  Cyc_Absyn_Eq_constr: 0){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp350)->f1; if(( unsigned int) _temp360 >  1u?*(( int*) _temp360) ==  Cyc_Absyn_Upper_b:
0){ _LL363: _temp362=(( struct Cyc_Absyn_Upper_b_struct*) _temp360)->f1; goto
_LL355;} else{ goto _LL356;}} else{ goto _LL356;} _LL356: goto _LL357; _LL353:
return t1; _LL355: { struct Cyc_Absyn_PointerType_struct* _temp364=({ struct Cyc_Absyn_PointerType_struct*
_temp365=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp365[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp366; _temp366.tag= Cyc_Absyn_PointerType;
_temp366.f1=({ struct Cyc_Absyn_PtrInfo _temp367; _temp367.elt_typ=( void*)
_temp346; _temp367.rgn_typ=( void*) _temp344; _temp367.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp367.tq= _temp340; _temp367.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp367;}); _temp366;}); _temp365;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp364); return( void*) _temp364;} _LL357:( void*)( _temp338->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp368=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp368[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp369; _temp369.tag= Cyc_Absyn_Eq_constr; _temp369.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp369;}); _temp368;}))); return t1;
_LL351:;} _LL335: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL331:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ struct Cyc_Std_String_pa_struct
_temp372; _temp372.tag= Cyc_Std_String_pa; _temp372.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp371; _temp371.tag= Cyc_Std_String_pa; _temp371.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp370[ 2u]={&
_temp371,& _temp372}; Cyc_Tcutil_terr( e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp370, sizeof( void*), 2u));}}});}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
struct Cyc_Std_String_pa_struct _temp375; _temp375.tag= Cyc_Std_String_pa;
_temp375.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp374; _temp374.tag= Cyc_Std_String_pa; _temp374.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp373[ 2u]={& _temp374,& _temp375}; Cyc_Tcutil_terr( e2->loc,
_tag_arr("expecting either %s or int but found %s", sizeof( unsigned char), 40u),
_tag_arr( _temp373, sizeof( void*), 2u));}}});} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp376=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp376->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp376;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp378=({ struct _tuple0 _temp377;
_temp377.f1= Cyc_Tcutil_compress( t1); _temp377.f2= Cyc_Tcutil_compress( t2);
_temp377;}); void* _temp384; struct Cyc_Absyn_PtrInfo _temp386; void* _temp388;
void* _temp390; struct Cyc_Absyn_PtrInfo _temp392; void* _temp394; _LL380:
_LL391: _temp390= _temp378.f1; if(( unsigned int) _temp390 >  4u?*(( int*)
_temp390) ==  Cyc_Absyn_PointerType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_PointerType_struct*)
_temp390)->f1; _LL395: _temp394=( void*) _temp392.elt_typ; goto _LL385;} else{
goto _LL382;} _LL385: _temp384= _temp378.f2; if(( unsigned int) _temp384 >  4u?*((
int*) _temp384) ==  Cyc_Absyn_PointerType: 0){ _LL387: _temp386=(( struct Cyc_Absyn_PointerType_struct*)
_temp384)->f1; _LL389: _temp388=( void*) _temp386.elt_typ; goto _LL381;} else{
goto _LL382;} _LL382: goto _LL383; _LL381: if( Cyc_Tcutil_unify( _temp394,
_temp388)){ return Cyc_Absyn_sint_t;} goto _LL379; _LL383: goto _LL379; _LL379:;}({
struct Cyc_Std_String_pa_struct _temp398; _temp398.tag= Cyc_Std_String_pa;
_temp398.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp397; _temp397.tag= Cyc_Std_String_pa; _temp397.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp396[ 2u]={& _temp397,& _temp398}; Cyc_Tcutil_terr( loc,
_tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char), 41u),
_tag_arr( _temp396, sizeof( void*), 2u));}}}); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp399->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp399;}));}}}}}
static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp400= p; _LL402: if( _temp400 == ( void*) Cyc_Absyn_Plus){ goto _LL403;}
else{ goto _LL404;} _LL404: if( _temp400 == ( void*) Cyc_Absyn_Minus){ goto
_LL405;} else{ goto _LL406;} _LL406: if( _temp400 == ( void*) Cyc_Absyn_Times){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp400 == ( void*) Cyc_Absyn_Div){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp400 == ( void*) Cyc_Absyn_Mod){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp400 == ( void*) Cyc_Absyn_Bitand){
goto _LL413;} else{ goto _LL414;} _LL414: if( _temp400 == ( void*) Cyc_Absyn_Bitor){
goto _LL415;} else{ goto _LL416;} _LL416: if( _temp400 == ( void*) Cyc_Absyn_Bitxor){
goto _LL417;} else{ goto _LL418;} _LL418: if( _temp400 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL419;} else{ goto _LL420;} _LL420: if( _temp400 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL421;} else{ goto _LL422;} _LL422: if( _temp400 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL423;} else{ goto _LL424;} _LL424: if( _temp400 == ( void*) Cyc_Absyn_Eq){
goto _LL425;} else{ goto _LL426;} _LL426: if( _temp400 == ( void*) Cyc_Absyn_Neq){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp400 == ( void*) Cyc_Absyn_Gt){
goto _LL429;} else{ goto _LL430;} _LL430: if( _temp400 == ( void*) Cyc_Absyn_Lt){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp400 == ( void*) Cyc_Absyn_Gte){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp400 == ( void*) Cyc_Absyn_Lte){
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL403: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL405: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL407: goto _LL409;
_LL409: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL411: goto _LL413; _LL413: goto _LL415; _LL415: goto _LL417; _LL417: goto
_LL419; _LL419: goto _LL421; _LL421: goto _LL423; _LL423: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL425: goto _LL427; _LL427: goto _LL429;
_LL429: goto _LL431; _LL431: goto _LL433; _LL433: goto _LL435; _LL435: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL437:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp438=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp438[ 0]=({ struct Cyc_Core_Failure_struct
_temp439; _temp439.tag= Cyc_Core_Failure; _temp439.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp439;}); _temp438;})); _LL401:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL440: return({ void* _temp442[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp442, sizeof( void*), 0u));}); case 1: _LL441: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL443: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL444: return({ void* _temp446[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp446, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp447=( void*) e->r; void* _temp471; struct Cyc_Absyn_Vardecl* _temp473;
void* _temp475; struct Cyc_Absyn_Vardecl* _temp477; void* _temp479; struct Cyc_Absyn_Vardecl*
_temp481; void* _temp483; struct Cyc_Absyn_Vardecl* _temp485; struct Cyc_Absyn_Exp*
_temp487; struct Cyc_Absyn_Exp* _temp489; struct _tagged_arr* _temp491; struct
Cyc_Absyn_Exp* _temp493; struct _tagged_arr* _temp495; struct Cyc_Absyn_Exp*
_temp497; struct Cyc_Absyn_Exp* _temp499; struct Cyc_Absyn_Exp* _temp501; struct
Cyc_Absyn_Exp* _temp503; _LL449: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL472: _temp471=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp471 >  1u?*(( int*) _temp471) ==  Cyc_Absyn_Param_b: 0){
_LL474: _temp473=(( struct Cyc_Absyn_Param_b_struct*) _temp471)->f1; goto _LL450;}
else{ goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL476: _temp475=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp475 >  1u?*(( int*) _temp475) ==  Cyc_Absyn_Local_b: 0){
_LL478: _temp477=(( struct Cyc_Absyn_Local_b_struct*) _temp475)->f1; goto _LL452;}
else{ goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL480: _temp479=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp479 >  1u?*(( int*) _temp479) ==  Cyc_Absyn_Pat_b: 0){ _LL482:
_temp481=(( struct Cyc_Absyn_Pat_b_struct*) _temp479)->f1; goto _LL454;} else{
goto _LL455;}} else{ goto _LL455;} _LL455: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL484: _temp483=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp483 >  1u?*(( int*) _temp483) ==  Cyc_Absyn_Global_b: 0){
_LL486: _temp485=(( struct Cyc_Absyn_Global_b_struct*) _temp483)->f1; goto
_LL456;} else{ goto _LL457;}} else{ goto _LL457;} _LL457: if(*(( int*) _temp447)
==  Cyc_Absyn_Subscript_e){ _LL490: _temp489=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f1; goto _LL488; _LL488: _temp487=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp447)
==  Cyc_Absyn_StructMember_e){ _LL494: _temp493=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f1; goto _LL492; _LL492: _temp491=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp447)
==  Cyc_Absyn_StructArrow_e){ _LL498: _temp497=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f1; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f2; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp447)
==  Cyc_Absyn_Deref_e){ _LL500: _temp499=(( struct Cyc_Absyn_Deref_e_struct*)
_temp447)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp447)
==  Cyc_Absyn_NoInstantiate_e){ _LL502: _temp501=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp447)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp447)
==  Cyc_Absyn_Instantiate_e){ _LL504: _temp503=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp447)->f1; goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL450:
_temp477= _temp473; goto _LL452; _LL452: _temp481= _temp477; goto _LL454; _LL454:
_temp485= _temp481; goto _LL456; _LL456: if( !( _temp485->tq).q_const){ return;}
goto _LL448; _LL458:{ void* _temp505= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp489->topt))->v); struct Cyc_Absyn_PtrInfo _temp515; struct Cyc_Absyn_Tqual
_temp517; struct Cyc_Absyn_Tqual _temp519; struct Cyc_List_List* _temp521;
_LL507: if(( unsigned int) _temp505 >  4u?*(( int*) _temp505) ==  Cyc_Absyn_PointerType:
0){ _LL516: _temp515=(( struct Cyc_Absyn_PointerType_struct*) _temp505)->f1;
_LL518: _temp517= _temp515.tq; goto _LL508;} else{ goto _LL509;} _LL509: if((
unsigned int) _temp505 >  4u?*(( int*) _temp505) ==  Cyc_Absyn_ArrayType: 0){
_LL520: _temp519=(( struct Cyc_Absyn_ArrayType_struct*) _temp505)->f2; goto
_LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp505 >  4u?*(( int*)
_temp505) ==  Cyc_Absyn_TupleType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_TupleType_struct*)
_temp505)->f1; goto _LL512;} else{ goto _LL513;} _LL513: goto _LL514; _LL508:
_temp519= _temp517; goto _LL510; _LL510: if( ! _temp519.q_const){ return;} goto
_LL506; _LL512: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp487);{
struct _handler_cons _temp523; _push_handler(& _temp523);{ int _temp525= 0; if(
setjmp( _temp523.handler)){ _temp525= 1;} if( ! _temp525){{ struct _tuple8
_temp528; struct Cyc_Absyn_Tqual _temp529; struct _tuple8* _temp526=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp521,( int) i);
_temp528=* _temp526; _LL530: _temp529= _temp528.f1; goto _LL527; _LL527: if( !
_temp529.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp524=( void*) _exn_thrown; void* _temp532= _temp524; _LL534: if( _temp532 == 
Cyc_List_Nth){ goto _LL535;} else{ goto _LL536;} _LL536: goto _LL537; _LL535:
return; _LL537:( void) _throw( _temp532); _LL533:;}}} goto _LL506;} _LL514: goto
_LL506; _LL506:;} goto _LL448; _LL460:{ void* _temp538= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp493->topt))->v); struct Cyc_Absyn_Structdecl**
_temp550; struct Cyc_List_List* _temp552; struct Cyc_List_List* _temp554; struct
Cyc_Absyn_Uniondecl** _temp556; _LL540: if(( unsigned int) _temp538 >  4u?*((
int*) _temp538) ==  Cyc_Absyn_StructType: 0){ _LL551: _temp550=(( struct Cyc_Absyn_StructType_struct*)
_temp538)->f3; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp538 >  4u?*(( int*) _temp538) ==  Cyc_Absyn_AnonUnionType: 0){ _LL553:
_temp552=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp538)->f1; goto _LL543;}
else{ goto _LL544;} _LL544: if(( unsigned int) _temp538 >  4u?*(( int*) _temp538)
==  Cyc_Absyn_AnonStructType: 0){ _LL555: _temp554=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp538)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp538 >  4u?*(( int*) _temp538) ==  Cyc_Absyn_UnionType: 0){ _LL557: _temp556=((
struct Cyc_Absyn_UnionType_struct*) _temp538)->f3; goto _LL547;} else{ goto
_LL548;} _LL548: goto _LL549; _LL541: { struct Cyc_Absyn_Structfield* sf=
_temp550 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp550)), _temp491); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL539;} _LL543: _temp554= _temp552; goto _LL545; _LL545: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp554, _temp491); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL539;}
_LL547: { struct Cyc_Absyn_Structfield* sf= _temp556 ==  0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp556)), _temp491); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL539;} _LL549: goto _LL539;
_LL539:;} goto _LL448; _LL462:{ void* _temp558= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp497->topt))->v); struct Cyc_Absyn_PtrInfo
_temp564; void* _temp566; _LL560: if(( unsigned int) _temp558 >  4u?*(( int*)
_temp558) ==  Cyc_Absyn_PointerType: 0){ _LL565: _temp564=(( struct Cyc_Absyn_PointerType_struct*)
_temp558)->f1; _LL567: _temp566=( void*) _temp564.elt_typ; goto _LL561;} else{
goto _LL562;} _LL562: goto _LL563; _LL561:{ void* _temp568= Cyc_Tcutil_compress(
_temp566); struct Cyc_Absyn_Structdecl** _temp580; struct Cyc_Absyn_Uniondecl**
_temp582; struct Cyc_List_List* _temp584; struct Cyc_List_List* _temp586; _LL570:
if(( unsigned int) _temp568 >  4u?*(( int*) _temp568) ==  Cyc_Absyn_StructType:
0){ _LL581: _temp580=(( struct Cyc_Absyn_StructType_struct*) _temp568)->f3; goto
_LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 >  4u?*(( int*)
_temp568) ==  Cyc_Absyn_UnionType: 0){ _LL583: _temp582=(( struct Cyc_Absyn_UnionType_struct*)
_temp568)->f3; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp568 >  4u?*(( int*) _temp568) ==  Cyc_Absyn_AnonStructType: 0){ _LL585:
_temp584=(( struct Cyc_Absyn_AnonStructType_struct*) _temp568)->f1; goto _LL575;}
else{ goto _LL576;} _LL576: if(( unsigned int) _temp568 >  4u?*(( int*) _temp568)
==  Cyc_Absyn_AnonUnionType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp568)->f1; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL571: {
struct Cyc_Absyn_Structfield* sf= _temp580 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp580)), _temp495); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL569;} _LL573: { struct Cyc_Absyn_Structfield*
sf= _temp582 ==  0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp582)), _temp495); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL569;} _LL575: _temp586= _temp584; goto _LL577; _LL577: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp586, _temp495); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL569;}
_LL579: goto _LL569; _LL569:;} goto _LL559; _LL563: goto _LL559; _LL559:;} goto
_LL448; _LL464:{ void* _temp588= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp499->topt))->v); struct Cyc_Absyn_PtrInfo _temp596; struct Cyc_Absyn_Tqual
_temp598; struct Cyc_Absyn_Tqual _temp600; _LL590: if(( unsigned int) _temp588 > 
4u?*(( int*) _temp588) ==  Cyc_Absyn_PointerType: 0){ _LL597: _temp596=(( struct
Cyc_Absyn_PointerType_struct*) _temp588)->f1; _LL599: _temp598= _temp596.tq;
goto _LL591;} else{ goto _LL592;} _LL592: if(( unsigned int) _temp588 >  4u?*((
int*) _temp588) ==  Cyc_Absyn_ArrayType: 0){ _LL601: _temp600=(( struct Cyc_Absyn_ArrayType_struct*)
_temp588)->f2; goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL591:
_temp600= _temp598; goto _LL593; _LL593: if( ! _temp600.q_const){ return;} goto
_LL589; _LL595: goto _LL589; _LL589:;} goto _LL448; _LL466: _temp503= _temp501;
goto _LL468; _LL468: Cyc_Tcexp_check_writable( te, _temp503); return; _LL470:
goto _LL448; _LL448:;}({ void* _temp602[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("attempt to write a read-only location", sizeof( unsigned char), 38u),
_tag_arr( _temp602, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return({ void* _temp603[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u), _tag_arr( _temp603, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if(
! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({
struct Cyc_Std_String_pa_struct _temp605; _temp605.tag= Cyc_Std_String_pa;
_temp605.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp604[ 1u]={&
_temp605}; Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp604, sizeof( void*), 1u));}});}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({
struct Cyc_Core_Opt* _temp613=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp613->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp613;}));
struct Cyc_List_List* _temp606=({ struct Cyc_List_List* _temp612=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp612->hd=( void*) e3; _temp612->tl=
0; _temp612;}); struct Cyc_List_List* _temp607=({ struct Cyc_List_List* _temp611=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp611->hd=(
void*) e2; _temp611->tl= _temp606; _temp611;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp607)){({ struct Cyc_Std_String_pa_struct _temp610; _temp610.tag= Cyc_Std_String_pa;
_temp610.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Std_String_pa_struct _temp609; _temp609.tag=
Cyc_Std_String_pa; _temp609.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp608[ 2u]={&
_temp609,& _temp610}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp608, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp614=
Cyc_Tcutil_compress( t1); _LL616: if(( unsigned int) _temp614 >  4u?*(( int*)
_temp614) ==  Cyc_Absyn_ArrayType: 0){ goto _LL617;} else{ goto _LL618;} _LL618:
goto _LL619; _LL617:({ void* _temp620[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp620, sizeof( void*), 0u));}); goto
_LL615; _LL619: goto _LL615; _LL615:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp621[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp621, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return({
struct Cyc_Std_String_pa_struct _temp624; _temp624.tag= Cyc_Std_String_pa;
_temp624.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp623; _temp623.tag= Cyc_Std_String_pa; _temp623.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp622[ 2u]={& _temp623,& _temp624}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u), _tag_arr(
_temp622, sizeof( void*), 2u));}}});}} else{ void* _temp625=( void*)(( struct
Cyc_Core_Opt*) _check_null( po))->v; void* _temp626= Cyc_Tcexp_tcBinPrimop( te,
loc, 0, _temp625, e1, e2); if( !( Cyc_Tcutil_unify( _temp626, t1)? 1:( Cyc_Tcutil_coerceable(
_temp626)? Cyc_Tcutil_coerceable( t1): 0))){ return({ struct Cyc_Std_String_pa_struct
_temp629; _temp629.tag= Cyc_Std_String_pa; _temp629.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp628; _temp628.tag= Cyc_Std_String_pa;
_temp628.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp627[
2u]={& _temp628,& _temp629}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp627, sizeof( void*), 2u));}}});}
return _temp626;} return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp630= Cyc_Tcutil_compress(
t1); void* _temp640; void* _temp642; _LL632: if( _temp630 == ( void*) Cyc_Absyn_FloatType){
goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int) _temp630 >  4u?*((
int*) _temp630) ==  Cyc_Absyn_IntType: 0){ _LL641: _temp640=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp630)->f2; if( _temp640 == ( void*) Cyc_Absyn_B1){
goto _LL635;} else{ goto _LL636;}} else{ goto _LL636;} _LL636: if(( unsigned int)
_temp630 >  4u?*(( int*) _temp630) ==  Cyc_Absyn_IntType: 0){ _LL643: _temp642=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp630)->f2; if( _temp642 == ( void*)
Cyc_Absyn_B2){ goto _LL637;} else{ goto _LL638;}} else{ goto _LL638;} _LL638:
goto _LL639; _LL633: Cyc_Tcutil_unchecked_cast( te, e,( void*) Cyc_Absyn_DoubleType);
t1=( void*) Cyc_Absyn_DoubleType; goto _LL631; _LL635: goto _LL637; _LL637: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); t1= Cyc_Absyn_sint_t; goto _LL631; _LL639: goto _LL631;
_LL631:;} for( fields= fs; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ void* _temp646; struct Cyc_Position_Segment*
_temp648; struct Cyc_List_List* _temp650; struct Cyc_List_List* _temp652; struct
_tuple1* _temp654; struct Cyc_Absyn_Tunionfield _temp644=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd); _LL655: _temp654= _temp644.name;
goto _LL653; _LL653: _temp652= _temp644.tvs; goto _LL651; _LL651: _temp650=
_temp644.typs; goto _LL649; _LL649: _temp648= _temp644.loc; goto _LL647; _LL647:
_temp646=( void*) _temp644.sc; goto _LL645; _LL645: if(( _temp650 ==  0? 1:((
struct Cyc_List_List*) _check_null( _temp650))->tl !=  0)? 1: _temp652 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( _temp650))->hd)).f2); if( Cyc_Tcutil_unify(
t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} for( fields= fs; fields != 
0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp658;
struct Cyc_Position_Segment* _temp660; struct Cyc_List_List* _temp662; struct
Cyc_List_List* _temp664; struct _tuple1* _temp666; struct Cyc_Absyn_Tunionfield
_temp656=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL667: _temp666= _temp656.name; goto _LL665; _LL665: _temp664=
_temp656.tvs; goto _LL663; _LL663: _temp662= _temp656.typs; goto _LL661; _LL661:
_temp660= _temp656.loc; goto _LL659; _LL659: _temp658=( void*) _temp656.sc; goto
_LL657; _LL657: if(( _temp662 ==  0? 1:(( struct Cyc_List_List*) _check_null(
_temp662))->tl !=  0)? 1: _temp664 !=  0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp662))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}({
struct Cyc_Std_String_pa_struct _temp670; _temp670.tag= Cyc_Std_String_pa;
_temp670.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ struct Cyc_Std_String_pa_struct
_temp669; _temp669.tag= Cyc_Std_String_pa; _temp669.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
tu);{ void* _temp668[ 2u]={& _temp669,& _temp670}; Cyc_Tcutil_terr( e->loc,
_tag_arr("can't find a field in %s to inject a value of type %s", sizeof(
unsigned char), 54u), _tag_arr( _temp668, sizeof( void*), 2u));}}}); return 0;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* args, struct
Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List* _temp671=
args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp672= t; struct Cyc_Absyn_PtrInfo _temp678; struct Cyc_Absyn_Conref*
_temp680; struct Cyc_Absyn_Tqual _temp682; struct Cyc_Absyn_Conref* _temp684;
void* _temp686; void* _temp688; _LL674: if(( unsigned int) _temp672 >  4u?*((
int*) _temp672) ==  Cyc_Absyn_PointerType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_PointerType_struct*)
_temp672)->f1; _LL689: _temp688=( void*) _temp678.elt_typ; goto _LL687; _LL687:
_temp686=( void*) _temp678.rgn_typ; goto _LL685; _LL685: _temp684= _temp678.nullable;
goto _LL683; _LL683: _temp682= _temp678.tq; goto _LL681; _LL681: _temp680=
_temp678.bounds; goto _LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL675:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp686); Cyc_Tcutil_check_nonzero_bound(
loc, _temp680);{ void* _temp690= Cyc_Tcutil_compress( _temp688); struct Cyc_Absyn_FnInfo
_temp696; struct Cyc_List_List* _temp698; struct Cyc_List_List* _temp700; struct
Cyc_Absyn_VarargInfo* _temp702; int _temp704; struct Cyc_List_List* _temp706;
void* _temp708; struct Cyc_Core_Opt* _temp710; struct Cyc_List_List* _temp712;
_LL692: if(( unsigned int) _temp690 >  4u?*(( int*) _temp690) ==  Cyc_Absyn_FnType:
0){ _LL697: _temp696=(( struct Cyc_Absyn_FnType_struct*) _temp690)->f1; _LL713:
_temp712= _temp696.tvars; goto _LL711; _LL711: _temp710= _temp696.effect; goto
_LL709; _LL709: _temp708=( void*) _temp696.ret_typ; goto _LL707; _LL707:
_temp706= _temp696.args; goto _LL705; _LL705: _temp704= _temp696.c_varargs; goto
_LL703; _LL703: _temp702= _temp696.cyc_varargs; goto _LL701; _LL701: _temp700=
_temp696.rgn_po; goto _LL699; _LL699: _temp698= _temp696.attributes; goto _LL693;}
else{ goto _LL694;} _LL694: goto _LL695; _LL693: if( topt !=  0){ Cyc_Tcutil_unify(
_temp708,*(( void**) _check_null( topt)));} while( _temp671 !=  0? _temp706 != 
0: 0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp671))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp706))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp716;
_temp716.tag= Cyc_Std_String_pa; _temp716.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp715; _temp715.tag= Cyc_Std_String_pa;
_temp715.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v);{ void* _temp714[ 2u]={& _temp715,& _temp716}; Cyc_Tcutil_terr(
loc, _tag_arr("actual argument has type %s but formal has type %s", sizeof(
unsigned char), 51u), _tag_arr( _temp714, sizeof( void*), 2u));}}});} _temp671=((
struct Cyc_List_List*) _check_null( _temp671))->tl; _temp706=(( struct Cyc_List_List*)
_check_null( _temp706))->tl;} if( _temp706 !=  0){({ void* _temp717[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments for function", sizeof( unsigned char), 31u),
_tag_arr( _temp717, sizeof( void*), 0u));});} else{ if(( _temp671 !=  0? 1:
_temp704)? 1: _temp702 !=  0){ if( _temp704){ for( 0; _temp671 !=  0; _temp671=((
struct Cyc_List_List*) _check_null( _temp671))->tl){ Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp671))->hd);}}
else{ if( _temp702 ==  0){({ void* _temp718[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too many arguments for function", sizeof( unsigned char), 32u),
_tag_arr( _temp718, sizeof( void*), 0u));});} else{ int _temp721; void* _temp723;
struct Cyc_Absyn_VarargInfo _temp719=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp702)); _LL724: _temp723=( void*) _temp719.type; goto _LL722;
_LL722: _temp721= _temp719.inject; goto _LL720; _LL720: { struct Cyc_Absyn_VarargCallInfo*
_temp725=({ struct Cyc_Absyn_VarargCallInfo* _temp754=( struct Cyc_Absyn_VarargCallInfo*)
_cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp754->num_varargs= 0;
_temp754->injectors= 0; _temp754->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp702); _temp754;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp725; if( ! _temp721){ for( 0; _temp671 !=  0; _temp671=(( struct Cyc_List_List*)
_check_null( _temp671))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp671))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp725))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp723,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp723)){({ struct Cyc_Std_String_pa_struct
_temp728; _temp728.tag= Cyc_Std_String_pa; _temp728.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp727; _temp727.tag= Cyc_Std_String_pa; _temp727.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp723);{ void* _temp726[ 2u]={& _temp727,& _temp728}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp726, sizeof( void*), 2u));}}});}}} else{
void* _temp729= Cyc_Tcutil_compress( _temp723); struct Cyc_Absyn_TunionInfo
_temp735; void* _temp737; struct Cyc_List_List* _temp739; void* _temp741; struct
Cyc_Absyn_Tuniondecl* _temp743; _LL731: if(( unsigned int) _temp729 >  4u?*((
int*) _temp729) ==  Cyc_Absyn_TunionType: 0){ _LL736: _temp735=(( struct Cyc_Absyn_TunionType_struct*)
_temp729)->f1; _LL742: _temp741=( void*) _temp735.tunion_info; if(*(( int*)
_temp741) ==  Cyc_Absyn_KnownTunion){ _LL744: _temp743=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp741)->f1; goto _LL740;} else{ goto _LL733;} _LL740: _temp739= _temp735.targs;
goto _LL738; _LL738: _temp737=( void*) _temp735.rgn; goto _LL732;} else{ goto
_LL733;} _LL733: goto _LL734; _LL732: { struct Cyc_Absyn_Tuniondecl* _temp745=*
Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp743->name); struct Cyc_List_List*
fields= 0; if( _temp745->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp747; _temp747.tag= Cyc_Std_String_pa; _temp747.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp723);{ void* _temp746[ 1u]={& _temp747}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp746, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp745->fields))->v;} if( ! Cyc_Tcutil_unify( _temp737, Cyc_Tcenv_curr_rgn( te))){({
void* _temp748[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp748, sizeof( void*), 0u));});}{
struct _RegionHandle _temp749= _new_region(); struct _RegionHandle* rgn=&
_temp749; _push_region( rgn);{ struct Cyc_List_List* _temp750=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp745->tvs, _temp739);
for( 0; _temp671 !=  0; _temp671=(( struct Cyc_List_List*) _check_null( _temp671))->tl){((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp725))->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp671))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{ struct Cyc_Absyn_Tunionfield*
_temp751= Cyc_Tcexp_tcInjection( te, e1, _temp723, rgn, _temp750, fields); if(
_temp751 !=  0){(( struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp725))->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp725))->injectors,({ struct
Cyc_List_List* _temp752=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp752->hd=( void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp751));
_temp752->tl= 0; _temp752;}));}}}}}; _pop_region( rgn);} goto _LL730;} _LL734:({
void* _temp753[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp753, sizeof( void*), 0u));}); goto
_LL730; _LL730:;}}}}}}{ struct Cyc_List_List* a= _temp698; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ void* _temp755=( void*)(( struct
Cyc_List_List*) _check_null( a))->hd; int _temp761; int _temp763; void* _temp765;
_LL757: if(( unsigned int) _temp755 >  16u?*(( int*) _temp755) ==  Cyc_Absyn_Format_att:
0){ _LL766: _temp765=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp755)->f1;
goto _LL764; _LL764: _temp763=(( struct Cyc_Absyn_Format_att_struct*) _temp755)->f2;
goto _LL762; _LL762: _temp761=(( struct Cyc_Absyn_Format_att_struct*) _temp755)->f3;
goto _LL758;} else{ goto _LL759;} _LL759: goto _LL760; _LL758:{ struct
_handler_cons _temp767; _push_handler(& _temp767);{ int _temp769= 0; if( setjmp(
_temp767.handler)){ _temp769= 1;} if( ! _temp769){{ struct Cyc_Absyn_Exp*
_temp770=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
args, _temp763 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp761 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp771=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp771->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp761 -  1);
_temp771;});}{ void* _temp772= _temp765; _LL774: if( _temp772 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL775;} else{ goto _LL776;} _LL776: if( _temp772 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL777;} else{ goto _LL773;} _LL775: Cyc_Tcexp_check_format_args( te,
_temp770, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL773; _LL777: Cyc_Tcexp_check_format_args(
te, _temp770, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL773; _LL773:;}};
_pop_handler();} else{ void* _temp768=( void*) _exn_thrown; void* _temp779=
_temp768; _LL781: if( _temp779 ==  Cyc_List_Nth){ goto _LL782;} else{ goto
_LL783;} _LL783: goto _LL784; _LL782:({ void* _temp785[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp785, sizeof( void*), 0u));}); goto _LL780; _LL784:( void) _throw( _temp779);
_LL780:;}}} goto _LL756; _LL760: goto _LL756; _LL756:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp710))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp700); return _temp708; _LL695: return({ void* _temp786[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp786, sizeof( void*), 0u));}); _LL691:;}
_LL677: return({ void* _temp787[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp787, sizeof( void*), 0u));});
_LL673:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp789; _temp789.tag= Cyc_Std_String_pa; _temp789.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp788[ 1u]={&
_temp789}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp788, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp790=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp790->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp790;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp791= t1;
struct Cyc_Absyn_PtrInfo _temp797; struct Cyc_Absyn_Conref* _temp799; struct Cyc_Absyn_Tqual
_temp801; struct Cyc_Absyn_Conref* _temp803; void* _temp805; void* _temp807;
_LL793: if(( unsigned int) _temp791 >  4u?*(( int*) _temp791) ==  Cyc_Absyn_PointerType:
0){ _LL798: _temp797=(( struct Cyc_Absyn_PointerType_struct*) _temp791)->f1;
_LL808: _temp807=( void*) _temp797.elt_typ; goto _LL806; _LL806: _temp805=( void*)
_temp797.rgn_typ; goto _LL804; _LL804: _temp803= _temp797.nullable; goto _LL802;
_LL802: _temp801= _temp797.tq; goto _LL800; _LL800: _temp799= _temp797.bounds;
goto _LL794;} else{ goto _LL795;} _LL795: goto _LL796; _LL794:{ void* _temp809=
Cyc_Tcutil_compress( _temp807); struct Cyc_Absyn_FnInfo _temp815; struct Cyc_List_List*
_temp817; struct Cyc_List_List* _temp819; struct Cyc_Absyn_VarargInfo* _temp821;
int _temp823; struct Cyc_List_List* _temp825; void* _temp827; struct Cyc_Core_Opt*
_temp829; struct Cyc_List_List* _temp831; _LL811: if(( unsigned int) _temp809 > 
4u?*(( int*) _temp809) ==  Cyc_Absyn_FnType: 0){ _LL816: _temp815=(( struct Cyc_Absyn_FnType_struct*)
_temp809)->f1; _LL832: _temp831= _temp815.tvars; goto _LL830; _LL830: _temp829=
_temp815.effect; goto _LL828; _LL828: _temp827=( void*) _temp815.ret_typ; goto
_LL826; _LL826: _temp825= _temp815.args; goto _LL824; _LL824: _temp823= _temp815.c_varargs;
goto _LL822; _LL822: _temp821= _temp815.cyc_varargs; goto _LL820; _LL820:
_temp819= _temp815.rgn_po; goto _LL818; _LL818: _temp817= _temp815.attributes;
goto _LL812;} else{ goto _LL813;} _LL813: goto _LL814; _LL812: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp831)){ return({ void* _temp833[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp833, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts=(( struct
Cyc_List_List*) _check_null( ts))->tl, _temp831=(( struct Cyc_List_List*)
_check_null( _temp831))->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp831))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp834=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp834->hd=( void*)({
struct _tuple5* _temp835=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp835->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp831))->hd; _temp835->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp835;}); _temp834->tl= instantiation; _temp834;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp839=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp839[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp840; _temp840.tag= Cyc_Absyn_FnType; _temp840.f1=({ struct Cyc_Absyn_FnInfo
_temp841; _temp841.tvars= 0; _temp841.effect= _temp829; _temp841.ret_typ=( void*)
_temp827; _temp841.args= _temp825; _temp841.c_varargs= _temp823; _temp841.cyc_varargs=
_temp821; _temp841.rgn_po= _temp819; _temp841.attributes= _temp817; _temp841;});
_temp840;}); _temp839;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp836=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp836[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp837; _temp837.tag= Cyc_Absyn_PointerType;
_temp837.f1=({ struct Cyc_Absyn_PtrInfo _temp838; _temp838.elt_typ=( void*)
new_fn_typ; _temp838.rgn_typ=( void*) _temp805; _temp838.nullable= _temp803;
_temp838.tq= _temp801; _temp838.bounds= _temp799; _temp838;}); _temp837;});
_temp836;}); return new_typ;}} _LL814: goto _LL810; _LL810:;} goto _LL792;
_LL796: goto _LL792; _LL792:;} return({ struct Cyc_Std_String_pa_struct _temp843;
_temp843.tag= Cyc_Std_String_pa; _temp843.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp842[ 1u]={& _temp843}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp842, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return({ struct Cyc_Std_String_pa_struct _temp846; _temp846.tag=
Cyc_Std_String_pa; _temp846.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp845; _temp845.tag= Cyc_Std_String_pa;
_temp845.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp844[
2u]={& _temp845,& _temp846}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp844, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void**
_temp847= 0; struct Cyc_Absyn_Tqual _temp848= Cyc_Absyn_empty_tqual(); if( topt
!=  0){ void* _temp849= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp855; struct Cyc_Absyn_Tqual _temp857; void*
_temp859; _LL851: if(( unsigned int) _temp849 >  4u?*(( int*) _temp849) ==  Cyc_Absyn_PointerType:
0){ _LL856: _temp855=(( struct Cyc_Absyn_PointerType_struct*) _temp849)->f1;
_LL860: _temp859=( void*) _temp855.elt_typ; goto _LL858; _LL858: _temp857=
_temp855.tq; goto _LL852;} else{ goto _LL853;} _LL853: goto _LL854; _LL852:
_temp847=({ void** _temp861=( void**) _cycalloc( sizeof( void*)); _temp861[ 0]=
_temp859; _temp861;}); _temp848= _temp857; goto _LL850; _LL854: goto _LL850;
_LL850:;} Cyc_Tcexp_tcExpNoInst( te, _temp847, e);{ void* _temp862=( void*) e->r;
_LL864: if(*(( int*) _temp862) ==  Cyc_Absyn_Struct_e){ goto _LL865;} else{ goto
_LL866;} _LL866: if(*(( int*) _temp862) ==  Cyc_Absyn_Tuple_e){ goto _LL867;}
else{ goto _LL868;} _LL868: goto _LL869; _LL865: goto _LL867; _LL867:({ void*
_temp870[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof(
unsigned char), 19u), _tag_arr( _temp870, sizeof( void*), 0u));});{ void*(*
_temp871)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp872= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp878; struct Cyc_Absyn_Conref* _temp880; _LL874:
if(( unsigned int) _temp872 >  4u?*(( int*) _temp872) ==  Cyc_Absyn_PointerType:
0){ _LL879: _temp878=(( struct Cyc_Absyn_PointerType_struct*) _temp872)->f1;
_LL881: _temp880= _temp878.nullable; goto _LL875;} else{ goto _LL876;} _LL876:
goto _LL877; _LL875: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp880)){ _temp871= Cyc_Absyn_star_typ;} goto _LL873; _LL877: goto _LL873;
_LL873:;} return _temp871(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp848);} _LL869: goto _LL863; _LL863:;}{ void*
_temp884; int _temp886; struct _tuple6 _temp882= Cyc_Tcutil_addressof_props( te,
e); _LL887: _temp886= _temp882.f1; goto _LL885; _LL885: _temp884= _temp882.f2;
goto _LL883; _LL883: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp886){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp884, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp889; _temp889.tag= Cyc_Std_String_pa; _temp889.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp888[ 1u]={& _temp889}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp888, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t, struct _tagged_arr*
n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t);{ int bad_type= 1;{ void* _temp890= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp898; _LL892: if(( unsigned int) _temp890 >  4u?*(( int*) _temp890) ==  Cyc_Absyn_StructType:
0){ _LL899: _temp898=(( struct Cyc_Absyn_StructType_struct*) _temp890)->f3; goto
_LL893;} else{ goto _LL894;} _LL894: if(( unsigned int) _temp890 >  4u?*(( int*)
_temp890) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL895;} else{ goto _LL896;}
_LL896: goto _LL897; _LL893: if( _temp898 ==  0){ return({ void* _temp900[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType", sizeof(
unsigned char), 33u), _tag_arr( _temp900, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp898)))->fields ==  0){ goto
_LL891;} goto _LL895; _LL895: bad_type= 0; goto _LL891; _LL897: goto _LL891;
_LL891:;} if( bad_type){({ struct Cyc_Std_String_pa_struct _temp902; _temp902.tag=
Cyc_Std_String_pa; _temp902.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp901[ 1u]={& _temp902}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct type",
sizeof( unsigned char), 30u), _tag_arr( _temp901, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;}} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp903= t; struct Cyc_Absyn_PtrInfo _temp909;
struct Cyc_Absyn_Conref* _temp911; void* _temp913; void* _temp915; _LL905: if((
unsigned int) _temp903 >  4u?*(( int*) _temp903) ==  Cyc_Absyn_PointerType: 0){
_LL910: _temp909=(( struct Cyc_Absyn_PointerType_struct*) _temp903)->f1; _LL916:
_temp915=( void*) _temp909.elt_typ; goto _LL914; _LL914: _temp913=( void*)
_temp909.rgn_typ; goto _LL912; _LL912: _temp911= _temp909.bounds; goto _LL906;}
else{ goto _LL907;} _LL907: goto _LL908; _LL906: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp913); Cyc_Tcutil_check_nonzero_bound( loc, _temp911); return
_temp915; _LL908: return({ struct Cyc_Std_String_pa_struct _temp918; _temp918.tag=
Cyc_Std_String_pa; _temp918.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp917[ 1u]={& _temp918}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp917, sizeof( void*), 1u));}});
_LL904:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp919= t; struct Cyc_Absyn_Structdecl** _temp935; struct
Cyc_Absyn_Structdecl* _temp937; struct Cyc_List_List* _temp938; struct _tuple1*
_temp940; struct Cyc_List_List* _temp942; struct Cyc_List_List* _temp944; struct
Cyc_Absyn_Uniondecl** _temp946; struct Cyc_Absyn_Uniondecl* _temp948; struct Cyc_List_List*
_temp949; struct _tuple1* _temp951; _LL921: if(( unsigned int) _temp919 >  4u?*((
int*) _temp919) ==  Cyc_Absyn_StructType: 0){ _LL941: _temp940=(( struct Cyc_Absyn_StructType_struct*)
_temp919)->f1; goto _LL939; _LL939: _temp938=(( struct Cyc_Absyn_StructType_struct*)
_temp919)->f2; goto _LL936; _LL936: _temp935=(( struct Cyc_Absyn_StructType_struct*)
_temp919)->f3; if( _temp935 ==  0){ goto _LL923;} else{ _temp937=* _temp935;
goto _LL922;}} else{ goto _LL923;} _LL923: if(( unsigned int) _temp919 >  4u?*((
int*) _temp919) ==  Cyc_Absyn_AnonStructType: 0){ _LL943: _temp942=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp919)->f1; goto _LL924;} else{ goto _LL925;} _LL925: if(( unsigned int)
_temp919 >  4u?*(( int*) _temp919) ==  Cyc_Absyn_AnonUnionType: 0){ _LL945:
_temp944=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp919)->f1; goto _LL926;}
else{ goto _LL927;} _LL927: if(( unsigned int) _temp919 >  4u?*(( int*) _temp919)
==  Cyc_Absyn_UnionType: 0){ _LL952: _temp951=(( struct Cyc_Absyn_UnionType_struct*)
_temp919)->f1; goto _LL950; _LL950: _temp949=(( struct Cyc_Absyn_UnionType_struct*)
_temp919)->f2; goto _LL947; _LL947: _temp946=(( struct Cyc_Absyn_UnionType_struct*)
_temp919)->f3; if( _temp946 ==  0){ goto _LL929;} else{ _temp948=* _temp946;
goto _LL928;}} else{ goto _LL929;} _LL929: if(( unsigned int) _temp919 >  4u?*((
int*) _temp919) ==  Cyc_Absyn_ArrayType: 0){ goto _LL953;} else{ goto _LL931;}
_LL953: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL930;} else{ goto _LL931;} _LL931: if(( unsigned int) _temp919 > 
4u?*(( int*) _temp919) ==  Cyc_Absyn_PointerType: 0){ goto _LL954;} else{ goto
_LL933;} _LL954: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char),
5u)) ==  0){ goto _LL932;} else{ goto _LL933;} _LL933: goto _LL934; _LL922: if(
_temp940 ==  0){ return({ void* _temp955[ 0u]={}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("unresolved struct type (COMPILER ERROR)", sizeof( unsigned char), 40u),
_tag_arr( _temp955, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp956= Cyc_Absyn_lookup_struct_field( _temp937, f); if( _temp956 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp959; _temp959.tag= Cyc_Std_String_pa;
_temp959.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct _temp958;
_temp958.tag= Cyc_Std_String_pa; _temp958.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp940));{ void* _temp957[ 2u]={& _temp958,&
_temp959}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp957, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp960= _new_region(); struct _RegionHandle*
rgn=& _temp960; _push_region( rgn);{ struct Cyc_List_List* _temp961=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp937->tvs,
_temp938); t2= Cyc_Tcutil_rsubstitute( rgn, _temp961,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp956))->type);}; _pop_region( rgn);} return t2;}} _LL924: {
struct Cyc_Absyn_Structfield* _temp962= Cyc_Absyn_lookup_field( _temp942, f);
if( _temp962 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp964; _temp964.tag=
Cyc_Std_String_pa; _temp964.f1=( struct _tagged_arr)* f;{ void* _temp963[ 1u]={&
_temp964}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u), _tag_arr( _temp963, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp962))->type;}
_LL926: { struct Cyc_Absyn_Structfield* _temp965= Cyc_Absyn_lookup_field(
_temp944, f); if( _temp965 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp967; _temp967.tag= Cyc_Std_String_pa; _temp967.f1=( struct _tagged_arr)* f;{
void* _temp966[ 1u]={& _temp967}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp966, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp965))->type;}
_LL928: if( _temp951 ==  0){ return({ void* _temp968[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u), _tag_arr( _temp968, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp969= Cyc_Absyn_lookup_union_field( _temp948, f); if( _temp969 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp972; _temp972.tag= Cyc_Std_String_pa;
_temp972.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct _temp971;
_temp971.tag= Cyc_Std_String_pa; _temp971.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp951));{ void* _temp970[ 2u]={& _temp971,&
_temp972}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp970, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp973=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp948->tvs, _temp949); void*
_temp974= Cyc_Tcutil_substitute( _temp973,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp969))->type); return _temp974;}} _LL930:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp975=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp975[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp976; _temp976.tag= Cyc_Absyn_Primop_e; _temp976.f1=(
void*)(( void*) Cyc_Absyn_Size); _temp976.f2=({ struct Cyc_List_List* _temp977=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp977->hd=(
void*) e; _temp977->tl= 0; _temp977;}); _temp976;}); _temp975;}))); return Cyc_Absyn_uint_t;
_LL932:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp978=( struct Cyc_Absyn_Primop_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp978[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp979; _temp979.tag= Cyc_Absyn_Primop_e;
_temp979.f1=( void*)(( void*) Cyc_Absyn_Size); _temp979.f2=({ struct Cyc_List_List*
_temp980=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp980->hd=( void*) e; _temp980->tl= 0; _temp980;}); _temp979;}); _temp978;})));
return Cyc_Absyn_uint_t; _LL934: if( Cyc_Std_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp982; _temp982.tag= Cyc_Std_String_pa; _temp982.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp981[ 1u]={& _temp982}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct, union or array, found %s",
sizeof( unsigned char), 43u), _tag_arr( _temp981, sizeof( void*), 1u));}});}
else{ return({ struct Cyc_Std_String_pa_struct _temp984; _temp984.tag= Cyc_Std_String_pa;
_temp984.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp983[ 1u]={&
_temp984}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp983, sizeof( void*), 1u));}});}
_LL920:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp985= t;
struct Cyc_Absyn_PtrInfo _temp991; struct Cyc_Absyn_Conref* _temp993; void*
_temp995; void* _temp997; _LL987: if(( unsigned int) _temp985 >  4u?*(( int*)
_temp985) ==  Cyc_Absyn_PointerType: 0){ _LL992: _temp991=(( struct Cyc_Absyn_PointerType_struct*)
_temp985)->f1; _LL998: _temp997=( void*) _temp991.elt_typ; goto _LL996; _LL996:
_temp995=( void*) _temp991.rgn_typ; goto _LL994; _LL994: _temp993= _temp991.bounds;
goto _LL988;} else{ goto _LL989;} _LL989: goto _LL990; _LL988: Cyc_Tcutil_check_nonzero_bound(
loc, _temp993);{ void* _temp999= Cyc_Tcutil_compress( _temp997); struct Cyc_Absyn_Structdecl**
_temp1011; struct Cyc_Absyn_Structdecl* _temp1013; struct Cyc_List_List*
_temp1014; struct _tuple1* _temp1016; struct Cyc_Absyn_Uniondecl** _temp1018;
struct Cyc_Absyn_Uniondecl* _temp1020; struct Cyc_List_List* _temp1021; struct
_tuple1* _temp1023; struct Cyc_List_List* _temp1025; struct Cyc_List_List*
_temp1027; _LL1001: if(( unsigned int) _temp999 >  4u?*(( int*) _temp999) == 
Cyc_Absyn_StructType: 0){ _LL1017: _temp1016=(( struct Cyc_Absyn_StructType_struct*)
_temp999)->f1; goto _LL1015; _LL1015: _temp1014=(( struct Cyc_Absyn_StructType_struct*)
_temp999)->f2; goto _LL1012; _LL1012: _temp1011=(( struct Cyc_Absyn_StructType_struct*)
_temp999)->f3; if( _temp1011 ==  0){ goto _LL1003;} else{ _temp1013=* _temp1011;
goto _LL1002;}} else{ goto _LL1003;} _LL1003: if(( unsigned int) _temp999 >  4u?*((
int*) _temp999) ==  Cyc_Absyn_UnionType: 0){ _LL1024: _temp1023=(( struct Cyc_Absyn_UnionType_struct*)
_temp999)->f1; goto _LL1022; _LL1022: _temp1021=(( struct Cyc_Absyn_UnionType_struct*)
_temp999)->f2; goto _LL1019; _LL1019: _temp1018=(( struct Cyc_Absyn_UnionType_struct*)
_temp999)->f3; if( _temp1018 ==  0){ goto _LL1005;} else{ _temp1020=* _temp1018;
goto _LL1004;}} else{ goto _LL1005;} _LL1005: if(( unsigned int) _temp999 >  4u?*((
int*) _temp999) ==  Cyc_Absyn_AnonStructType: 0){ _LL1026: _temp1025=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp999)->f1; goto _LL1006;} else{ goto
_LL1007;} _LL1007: if(( unsigned int) _temp999 >  4u?*(( int*) _temp999) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1028: _temp1027=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp999)->f1;
goto _LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010; _LL1002: { struct Cyc_Absyn_Structfield*
_temp1029= Cyc_Absyn_lookup_struct_field( _temp1013, f); if( _temp1029 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1032; _temp1032.tag= Cyc_Std_String_pa;
_temp1032.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1031; _temp1031.tag= Cyc_Std_String_pa; _temp1031.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1016));{ void*
_temp1030[ 2u]={& _temp1031,& _temp1032}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1030, sizeof( void*), 2u));}}});}{
void* t3;{ struct _RegionHandle _temp1033= _new_region(); struct _RegionHandle*
rgn=& _temp1033; _push_region( rgn);{ struct Cyc_List_List* _temp1034=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1013->tvs,
_temp1014); t3= Cyc_Tcutil_rsubstitute( rgn, _temp1034,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1029))->type);}; _pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp995); return t3;}} _LL1004: { struct Cyc_Absyn_Structfield*
_temp1035= Cyc_Absyn_lookup_union_field( _temp1020, f); if( _temp1035 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1038; _temp1038.tag= Cyc_Std_String_pa;
_temp1038.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1037; _temp1037.tag= Cyc_Std_String_pa; _temp1037.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1023));{ void*
_temp1036[ 2u]={& _temp1037,& _temp1038}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1036, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1039=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1020->tvs, _temp1021); void*
_temp1040= Cyc_Tcutil_substitute( _temp1039,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1035))->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp995); return _temp1040;}} _LL1006: { struct Cyc_Absyn_Structfield*
_temp1041= Cyc_Absyn_lookup_field( _temp1025, f); if( _temp1041 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1043; _temp1043.tag= Cyc_Std_String_pa;
_temp1043.f1=( struct _tagged_arr)* f;{ void* _temp1042[ 1u]={& _temp1043}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1042, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1041))->type;} _LL1008: { struct Cyc_Absyn_Structfield*
_temp1044= Cyc_Absyn_lookup_field( _temp1027, f); if( _temp1044 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1046; _temp1046.tag= Cyc_Std_String_pa;
_temp1046.f1=( struct _tagged_arr)* f;{ void* _temp1045[ 1u]={& _temp1046}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union has no %s field", sizeof( unsigned char), 22u),
_tag_arr( _temp1045, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1044))->type;} _LL1010: return({ struct Cyc_Std_String_pa_struct
_temp1048; _temp1048.tag= Cyc_Std_String_pa; _temp1048.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1047[ 1u]={& _temp1048}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1047, sizeof( void*), 1u));}}); _LL1000:;} _LL990: return({
struct Cyc_Std_String_pa_struct _temp1050; _temp1050.tag= Cyc_Std_String_pa;
_temp1050.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1049[
1u]={& _temp1050}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1049, sizeof( void*), 1u));}});
_LL986:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1051; _push_handler(& _temp1051);{ int _temp1053= 0; if(
setjmp( _temp1051.handler)){ _temp1053= 1;} if( ! _temp1053){{ void* _temp1054=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1054;}; _pop_handler();} else{ void* _temp1052=(
void*) _exn_thrown; void* _temp1056= _temp1052; _LL1058: if( _temp1056 ==  Cyc_List_Nth){
goto _LL1059;} else{ goto _LL1060;} _LL1060: goto _LL1061; _LL1059: return({
struct Cyc_Std_Int_pa_struct _temp1064; _temp1064.tag= Cyc_Std_Int_pa; _temp1064.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1063; _temp1063.tag= Cyc_Std_Int_pa; _temp1063.f1=(
int) i;{ void* _temp1062[ 2u]={& _temp1063,& _temp1064}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1062, sizeof( void*), 2u));}}}); _LL1061:( void) _throw(
_temp1056); _LL1057:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1066; _temp1066.tag= Cyc_Std_String_pa;
_temp1066.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1065[
1u]={& _temp1066}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1065, sizeof( void*), 1u));}});}{
void* _temp1067= t1; struct Cyc_Absyn_PtrInfo _temp1075; struct Cyc_Absyn_Conref*
_temp1077; struct Cyc_Absyn_Tqual _temp1079; void* _temp1081; void* _temp1083;
struct Cyc_List_List* _temp1085; _LL1069: if(( unsigned int) _temp1067 >  4u?*((
int*) _temp1067) ==  Cyc_Absyn_PointerType: 0){ _LL1076: _temp1075=(( struct Cyc_Absyn_PointerType_struct*)
_temp1067)->f1; _LL1084: _temp1083=( void*) _temp1075.elt_typ; goto _LL1082;
_LL1082: _temp1081=( void*) _temp1075.rgn_typ; goto _LL1080; _LL1080: _temp1079=
_temp1075.tq; goto _LL1078; _LL1078: _temp1077= _temp1075.bounds; goto _LL1070;}
else{ goto _LL1071;} _LL1071: if(( unsigned int) _temp1067 >  4u?*(( int*)
_temp1067) ==  Cyc_Absyn_TupleType: 0){ _LL1086: _temp1085=(( struct Cyc_Absyn_TupleType_struct*)
_temp1067)->f1; goto _LL1072;} else{ goto _LL1073;} _LL1073: goto _LL1074;
_LL1070: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1077);} else{{ void* _temp1087=( void*)( Cyc_Absyn_compress_conref(
_temp1077))->v; void* _temp1093; struct Cyc_Absyn_Exp* _temp1095; struct Cyc_Absyn_Exp
_temp1097; void* _temp1098; void* _temp1100; int _temp1102; void* _temp1104;
_LL1089: if(( unsigned int) _temp1087 >  1u?*(( int*) _temp1087) ==  Cyc_Absyn_Eq_constr:
0){ _LL1094: _temp1093=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1087)->f1;
if(( unsigned int) _temp1093 >  1u?*(( int*) _temp1093) ==  Cyc_Absyn_Upper_b: 0){
_LL1096: _temp1095=(( struct Cyc_Absyn_Upper_b_struct*) _temp1093)->f1;
_temp1097=* _temp1095; _LL1099: _temp1098=( void*) _temp1097.r; if(*(( int*)
_temp1098) ==  Cyc_Absyn_Const_e){ _LL1101: _temp1100=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1098)->f1; if(( unsigned int) _temp1100 >  1u?*(( int*) _temp1100) ==  Cyc_Absyn_Int_c:
0){ _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1100)->f1;
if( _temp1104 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1103;} else{ goto _LL1091;}
_LL1103: _temp1102=(( struct Cyc_Absyn_Int_c_struct*) _temp1100)->f2; if(
_temp1102 ==  1){ goto _LL1090;} else{ goto _LL1091;}} else{ goto _LL1091;}}
else{ goto _LL1091;}} else{ goto _LL1091;}} else{ goto _LL1091;} _LL1091: goto
_LL1092; _LL1090:({ void* _temp1106[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1106, sizeof( void*), 0u));}); goto
_LL1088; _LL1092: goto _LL1088; _LL1088:;} Cyc_Tcutil_check_nonzero_bound( loc,
_temp1077);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1081); return
_temp1083; _LL1072: return Cyc_Tcexp_ithTupleType( te, loc, _temp1085, e2);
_LL1074: return({ struct Cyc_Std_String_pa_struct _temp1108; _temp1108.tag= Cyc_Std_String_pa;
_temp1108.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1107[
1u]={& _temp1108}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1107, sizeof( void*), 1u));}});
_LL1068:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1109= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp1115; _LL1111: if((
unsigned int) _temp1109 >  4u?*(( int*) _temp1109) ==  Cyc_Absyn_TupleType: 0){
_LL1116: _temp1115=(( struct Cyc_Absyn_TupleType_struct*) _temp1109)->f1; goto
_LL1112;} else{ goto _LL1113;} _LL1113: goto _LL1114; _LL1112: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1115) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){({ void* _temp1117[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("tuple expression has the wrong number of fields", sizeof(
unsigned char), 48u), _tag_arr( _temp1117, sizeof( void*), 0u));}); goto _LL1110;}
for( 0; es !=  0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1115=((
struct Cyc_List_List*) _check_null( _temp1115))->tl)){ void* _temp1118=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1115))->hd)).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1118,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1119=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1119->hd=( void*)({ struct
_tuple8* _temp1120=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp1120->f1=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1115))->hd)).f1; _temp1120->f2=( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1120;}); _temp1119->tl= fields; _temp1119;});} done= 1; goto _LL1110;
_LL1114: goto _LL1110; _LL1110:;} if( ! done){ for( 0; es !=  0; es=(( struct
Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1121=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1121->hd=( void*)({ struct _tuple8* _temp1122=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1122->f1= Cyc_Absyn_empty_tqual();
_temp1122->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1122;});
_temp1121->tl= fields; _temp1121;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1123=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1123[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1124; _temp1124.tag= Cyc_Absyn_TupleType;
_temp1124.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1124;}); _temp1123;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1125[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1125, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1148=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1148->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1148;})); struct Cyc_Absyn_Const_e_struct* _temp1126=({ struct Cyc_Absyn_Const_e_struct*
_temp1144=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1144[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1145; _temp1145.tag= Cyc_Absyn_Const_e;
_temp1145.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1146=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1146[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1147; _temp1147.tag= Cyc_Absyn_Int_c;
_temp1147.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1147.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1147;}); _temp1146;}));
_temp1145;}); _temp1144;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1126, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1127=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1127->v=( void*)
Cyc_Absyn_uint_t; _temp1127;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1142=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1142[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1143; _temp1143.tag= Cyc_Absyn_ArrayType;
_temp1143.f1=( void*) res; _temp1143.f2= Cyc_Absyn_empty_tqual(); _temp1143.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1143;}); _temp1142;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){({ struct
Cyc_Std_String_pa_struct _temp1129; _temp1129.tag= Cyc_Std_String_pa; _temp1129.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void* _temp1128[ 1u]={&
_temp1129}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1128, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des=(( struct Cyc_List_List*)
_check_null( des))->tl)){ struct Cyc_List_List* ds=(*(( struct _tuple7*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1; if( ds !=  0){ void*
_temp1130=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp1136; _LL1132: if(*(( int*) _temp1130) ==  Cyc_Absyn_FieldName){ goto
_LL1133;} else{ goto _LL1134;} _LL1134: if(*(( int*) _temp1130) ==  Cyc_Absyn_ArrayElement){
_LL1137: _temp1136=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1130)->f1;
goto _LL1135;} else{ goto _LL1131;} _LL1133:({ void* _temp1138[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u), _tag_arr( _temp1138, sizeof( void*), 0u));}); goto _LL1131;
_LL1135: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1136);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1136); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct _temp1141;
_temp1141.tag= Cyc_Std_Int_pa; _temp1141.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1140; _temp1140.tag= Cyc_Std_Int_pa; _temp1140.f1=( int)(( unsigned int)
offset);{ void* _temp1139[ 2u]={& _temp1140,& _temp1141}; Cyc_Tcutil_terr(
_temp1136->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1139, sizeof( void*), 2u));}}});} goto
_LL1131;} _LL1131:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1150; _temp1150.tag= Cyc_Std_String_pa;
_temp1150.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1149[ 1u]={& _temp1150}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1149, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1151[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1151, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void*
_temp1152[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u), _tag_arr( _temp1152, sizeof( void*), 0u));});} if(
! Cyc_Tcutil_is_const_exp( te, body)){({ void* _temp1153[ 0u]={}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u),
_tag_arr( _temp1153, sizeof( void*), 0u));});}}{ struct Cyc_Absyn_PtrInfo pinfo;
void** _temp1154= 0; struct Cyc_Absyn_Tqual* _temp1155= 0; if( topt !=  0){ void*
_temp1156= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1164; struct Cyc_Absyn_Exp* _temp1166; struct Cyc_Absyn_Tqual _temp1168;
struct Cyc_Absyn_Tqual* _temp1170; void* _temp1171; void** _temp1173; _LL1158:
if(( unsigned int) _temp1156 >  4u?*(( int*) _temp1156) ==  Cyc_Absyn_PointerType:
0){ _LL1165: _temp1164=(( struct Cyc_Absyn_PointerType_struct*) _temp1156)->f1;
goto _LL1159;} else{ goto _LL1160;} _LL1160: if(( unsigned int) _temp1156 >  4u?*((
int*) _temp1156) ==  Cyc_Absyn_ArrayType: 0){ _LL1172: _temp1171=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1156)->f1; _temp1173=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1156)->f1); goto _LL1169; _LL1169:
_temp1168=(( struct Cyc_Absyn_ArrayType_struct*) _temp1156)->f2; _temp1170=(
struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*) _temp1156)->f2;
goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1156)->f3; goto _LL1161;} else{ goto _LL1162;} _LL1162: goto _LL1163;
_LL1159: pinfo= _temp1164; _temp1154=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1155=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1157; _LL1161: _temp1154=(
void**) _temp1173; _temp1155=( struct Cyc_Absyn_Tqual*) _temp1170; goto _LL1157;
_LL1163: goto _LL1157; _LL1157:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1154, body);
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1174=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1174[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1175; _temp1175.tag= Cyc_Absyn_ArrayType;
_temp1175.f1=( void*) t; _temp1175.f2= _temp1155 ==  0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1155)); _temp1175.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1175;}); _temp1174;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1176; _push_handler(&
_temp1176);{ int _temp1178= 0; if( setjmp( _temp1176.handler)){ _temp1178= 1;}
if( ! _temp1178){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1177=( void*) _exn_thrown; void* _temp1180= _temp1177; _LL1182:
if( _temp1180 ==  Cyc_Dict_Absent){ goto _LL1183;} else{ goto _LL1184;} _LL1184:
goto _LL1185; _LL1183:({ struct Cyc_Std_String_pa_struct _temp1187; _temp1187.tag=
Cyc_Std_String_pa; _temp1187.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1186[ 1u]={& _temp1187}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1186, sizeof( void*), 1u));}});
return topt !=  0?*(( void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType;
_LL1185:( void) _throw( _temp1180); _LL1181:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*)
sd; if( sd->name !=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v;}}{ struct _RegionHandle _temp1188= _new_region();
struct _RegionHandle* rgn=& _temp1188; _push_region( rgn);{ struct _tuple4
_temp1189=({ struct _tuple4 _temp1213; _temp1213.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1213.f2= rgn; _temp1213;}); struct Cyc_List_List* _temp1190=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1189, sd->tvs); struct Cyc_List_List*
_temp1191=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1190); struct
Cyc_Absyn_StructType_struct* _temp1192=({ struct Cyc_Absyn_StructType_struct*
_temp1210=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1210[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1211; _temp1211.tag=
Cyc_Absyn_StructType; _temp1211.f1=( struct _tuple1*)* tn; _temp1211.f2=
_temp1191; _temp1211.f3=({ struct Cyc_Absyn_Structdecl** _temp1212=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1212[ 0]= sd; _temp1212;});
_temp1211;}); _temp1210;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1192,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1193=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1193->v=( void*)
_temp1191; _temp1193;}); if( sd->fields ==  0){({ void* _temp1194[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1194, sizeof( void*), 0u));});{ void* _temp1195=( void*)
_temp1192; _npop_handler( 0u); return _temp1195;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1198; struct Cyc_Absyn_Exp* _temp1199; struct Cyc_Absyn_Structfield*
_temp1201; struct _tuple9* _temp1196=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1198=* _temp1196; _LL1202: _temp1201= _temp1198.f1;
goto _LL1200; _LL1200: _temp1199= _temp1198.f2; goto _LL1197; _LL1197: { void*
_temp1203= Cyc_Tcutil_rsubstitute( rgn, _temp1190,( void*) _temp1201->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1203, _temp1199); if( ! Cyc_Tcutil_coerce_arg( te, _temp1199,
_temp1203)){({ struct Cyc_Std_String_pa_struct _temp1208; _temp1208.tag= Cyc_Std_String_pa;
_temp1208.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1199->topt))->v);{ struct Cyc_Std_String_pa_struct _temp1207;
_temp1207.tag= Cyc_Std_String_pa; _temp1207.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1203);{ struct Cyc_Std_String_pa_struct _temp1206; _temp1206.tag= Cyc_Std_String_pa;
_temp1206.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1205; _temp1205.tag= Cyc_Std_String_pa; _temp1205.f1=( struct _tagged_arr)*
_temp1201->name;{ void* _temp1204[ 4u]={& _temp1205,& _temp1206,& _temp1207,&
_temp1208}; Cyc_Tcutil_terr( _temp1199->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1204, sizeof( void*), 4u));}}}}});}}}{
void* _temp1209=( void*) _temp1192; _npop_handler( 0u); return _temp1209;}}};
_pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1214= _new_region(); struct _RegionHandle* rgn=&
_temp1214; _push_region( rgn);{ void* _temp1215= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1221; _LL1217: if(( unsigned int) _temp1215 >  4u?*((
int*) _temp1215) ==  Cyc_Absyn_AnonStructType: 0){ _LL1222: _temp1221=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1215)->f1; goto _LL1218;} else{ goto
_LL1219;} _LL1219: goto _LL1220; _LL1218: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1221); for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1225; struct Cyc_Absyn_Exp*
_temp1226; struct Cyc_Absyn_Structfield* _temp1228; struct _tuple9* _temp1223=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1225=*
_temp1223; _LL1229: _temp1228= _temp1225.f1; goto _LL1227; _LL1227: _temp1226=
_temp1225.f2; goto _LL1224; _LL1224: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1228->type)), _temp1226); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1226,( void*) _temp1228->type)){({ struct Cyc_Std_String_pa_struct
_temp1233; _temp1233.tag= Cyc_Std_String_pa; _temp1233.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1226->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1232; _temp1232.tag= Cyc_Std_String_pa;
_temp1232.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1228->type);{
struct Cyc_Std_String_pa_struct _temp1231; _temp1231.tag= Cyc_Std_String_pa;
_temp1231.f1=( struct _tagged_arr)* _temp1228->name;{ void* _temp1230[ 3u]={&
_temp1231,& _temp1232,& _temp1233}; Cyc_Tcutil_terr( _temp1226->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1230, sizeof( void*), 3u));}}}});}}
goto _LL1216;} _LL1220:({ void* _temp1234[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u), _tag_arr( _temp1234, sizeof( void*), 0u));}); goto
_LL1216; _LL1216:;}; _pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1235= _new_region(); struct _RegionHandle* rgn=&
_temp1235; _push_region( rgn);{ struct _tuple4 _temp1236=({ struct _tuple4
_temp1281; _temp1281.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1281.f2= rgn;
_temp1281;}); struct Cyc_List_List* _temp1237=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1236, tud->tvs); struct Cyc_List_List* _temp1238=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1236, tuf->tvs); struct Cyc_List_List* _temp1239=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1237); struct Cyc_List_List* _temp1240=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1238); struct Cyc_List_List*
_temp1241=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1237, _temp1238);*
all_ref=({ struct Cyc_Core_Opt* _temp1242=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1242->v=( void*) _temp1239; _temp1242;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1243=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1243->v=( void*) _temp1240; _temp1243;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1276=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1276[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1277; _temp1277.tag=
Cyc_Absyn_TunionFieldType; _temp1277.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1278; _temp1278.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1279=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1279[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1280; _temp1280.tag= Cyc_Absyn_KnownTunionfield; _temp1280.f1= tud;
_temp1280.f2= tuf; _temp1280;}); _temp1279;})); _temp1278.targs= _temp1239;
_temp1278;}); _temp1277;}); _temp1276;}); if( topt !=  0){ void* _temp1244= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo _temp1252; void*
_temp1254; struct Cyc_List_List* _temp1256; _LL1246: if(( unsigned int)
_temp1244 >  4u?*(( int*) _temp1244) ==  Cyc_Absyn_TunionFieldType: 0){ goto
_LL1247;} else{ goto _LL1248;} _LL1248: if(( unsigned int) _temp1244 >  4u?*((
int*) _temp1244) ==  Cyc_Absyn_TunionType: 0){ _LL1253: _temp1252=(( struct Cyc_Absyn_TunionType_struct*)
_temp1244)->f1; _LL1257: _temp1256= _temp1252.targs; goto _LL1255; _LL1255:
_temp1254=( void*) _temp1252.rgn; goto _LL1249;} else{ goto _LL1250;} _LL1250:
goto _LL1251; _LL1247: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res);
goto _LL1245; _LL1249:{ struct Cyc_List_List* a= _temp1239; for( 0; a !=  0?
_temp1256 !=  0: 0;( a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1256=((
struct Cyc_List_List*) _check_null( _temp1256))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp1256))->hd);}} if( tuf->typs ==  0? es ==  0: 0){ e->topt=({
struct Cyc_Core_Opt* _temp1258=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1258->v=( void*) res; _temp1258;}); res=( void*)({ struct
Cyc_Absyn_TunionType_struct* _temp1259=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1259[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1260; _temp1260.tag= Cyc_Absyn_TunionType;
_temp1260.f1=({ struct Cyc_Absyn_TunionInfo _temp1261; _temp1261.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1262=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1262[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1263; _temp1263.tag= Cyc_Absyn_KnownTunion;
_temp1263.f1= tud; _temp1263;}); _temp1262;})); _temp1261.targs= _temp1239;
_temp1261.rgn=( void*) _temp1254; _temp1261;}); _temp1260;}); _temp1259;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1264= res; _npop_handler( 0u); return _temp1264;}} goto
_LL1245; _LL1251: goto _LL1245; _LL1245:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es !=  0? ts !=  0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1241,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Std_String_pa_struct
_temp1268; _temp1268.tag= Cyc_Std_String_pa; _temp1268.f1=( struct _tagged_arr)(
e->topt ==  0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct
_temp1267; _temp1267.tag= Cyc_Std_String_pa; _temp1267.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct _temp1266;
_temp1266.tag= Cyc_Std_String_pa; _temp1266.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1265[ 3u]={& _temp1266,& _temp1267,& _temp1268}; Cyc_Tcutil_terr(
e->loc, _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1265, sizeof( void*), 3u));}}}});}}
if( es !=  0){ void* _temp1271=({ struct Cyc_Std_String_pa_struct _temp1270;
_temp1270.tag= Cyc_Std_String_pa; _temp1270.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1269[ 1u]={& _temp1270}; Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,
_tag_arr("too many arguments for tunion constructor %s", sizeof( unsigned char),
45u), _tag_arr( _temp1269, sizeof( void*), 1u));}}); _npop_handler( 0u); return
_temp1271;} if( ts !=  0){ void* _temp1274=({ struct Cyc_Std_String_pa_struct
_temp1273; _temp1273.tag= Cyc_Std_String_pa; _temp1273.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1272[ 1u]={& _temp1273}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("too few arguments for tunion constructor %s", sizeof(
unsigned char), 44u), _tag_arr( _temp1272, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1274;}{ void* _temp1275= res; _npop_handler( 0u);
return _temp1275;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1292=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1292[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1293; _temp1293.tag= Cyc_Absyn_RgnHandleType; _temp1293.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1294=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1294->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1294;})); _temp1293;}); _temp1292;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1282= Cyc_Tcutil_compress( handle_type); void*
_temp1288; _LL1284: if(( unsigned int) _temp1282 >  4u?*(( int*) _temp1282) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1289: _temp1288=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1282)->f1; goto _LL1285;} else{ goto _LL1286;} _LL1286: goto _LL1287;
_LL1285: rgn= _temp1288; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1283; _LL1287:({ struct Cyc_Std_String_pa_struct _temp1291; _temp1291.tag=
Cyc_Std_String_pa; _temp1291.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1290[ 1u]={& _temp1291}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1290, sizeof( void*), 1u));}});
goto _LL1283; _LL1283:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp1295= t; struct Cyc_Absyn_TunionFieldInfo
_temp1301; void* _temp1303; struct Cyc_Absyn_Tunionfield* _temp1305; _LL1297:
if(( unsigned int) _temp1295 >  4u?*(( int*) _temp1295) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1302: _temp1301=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1295)->f1;
_LL1304: _temp1303=( void*) _temp1301.field_info; if(*(( int*) _temp1303) == 
Cyc_Absyn_KnownTunionfield){ _LL1306: _temp1305=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1303)->f2; goto _LL1298;} else{ goto _LL1299;}} else{ goto _LL1299;}
_LL1299: goto _LL1300; _LL1298: if( _temp1305->tvs !=  0){({ void* _temp1307[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u), _tag_arr( _temp1307, sizeof( void*), 0u));});}
goto _LL1296; _LL1300: goto _LL1296; _LL1296:;}{ void*(* _temp1308)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1309= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1315; struct Cyc_Absyn_Conref* _temp1317; _LL1311: if(( unsigned int)
_temp1309 >  4u?*(( int*) _temp1309) ==  Cyc_Absyn_PointerType: 0){ _LL1316:
_temp1315=(( struct Cyc_Absyn_PointerType_struct*) _temp1309)->f1; _LL1318:
_temp1317= _temp1315.nullable; goto _LL1312;} else{ goto _LL1313;} _LL1313: goto
_LL1314; _LL1312: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1317)){ _temp1308= Cyc_Absyn_star_typ;} goto _LL1310; _LL1314: goto _LL1310;
_LL1310:;} return _temp1308( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1319=( void*) s->r; struct Cyc_Absyn_Exp* _temp1329;
struct Cyc_Absyn_Stmt* _temp1331; struct Cyc_Absyn_Stmt* _temp1333; struct Cyc_Absyn_Stmt*
_temp1335; struct Cyc_Absyn_Decl* _temp1337; _LL1321: if(( unsigned int)
_temp1319 >  1u?*(( int*) _temp1319) ==  Cyc_Absyn_Exp_s: 0){ _LL1330: _temp1329=((
struct Cyc_Absyn_Exp_s_struct*) _temp1319)->f1; goto _LL1322;} else{ goto
_LL1323;} _LL1323: if(( unsigned int) _temp1319 >  1u?*(( int*) _temp1319) == 
Cyc_Absyn_Seq_s: 0){ _LL1334: _temp1333=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1319)->f1; goto _LL1332; _LL1332: _temp1331=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1319)->f2; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(( unsigned int)
_temp1319 >  1u?*(( int*) _temp1319) ==  Cyc_Absyn_Decl_s: 0){ _LL1338:
_temp1337=(( struct Cyc_Absyn_Decl_s_struct*) _temp1319)->f1; goto _LL1336;
_LL1336: _temp1335=(( struct Cyc_Absyn_Decl_s_struct*) _temp1319)->f2; goto
_LL1326;} else{ goto _LL1327;} _LL1327: goto _LL1328; _LL1322: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1329->topt))->v; _LL1324: s= _temp1331;
continue; _LL1326: s= _temp1335; continue; _LL1328: return({ void* _temp1339[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1339, sizeof( void*), 0u));});
_LL1320:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1340[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1340, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1341[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1341, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1352=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1352[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1353; _temp1353.tag= Cyc_Absyn_RgnHandleType;
_temp1353.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1354=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1354->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1354;}));
_temp1353;}); _temp1352;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1342= Cyc_Tcutil_compress( handle_type); void* _temp1348; _LL1344: if((
unsigned int) _temp1342 >  4u?*(( int*) _temp1342) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1349: _temp1348=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1342)->f1; goto _LL1345;} else{ goto _LL1346;} _LL1346: goto _LL1347;
_LL1345: rgn= _temp1348; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1343; _LL1347:({ struct Cyc_Std_String_pa_struct _temp1351; _temp1351.tag=
Cyc_Std_String_pa; _temp1351.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1350[ 1u]={& _temp1351}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1350, sizeof( void*), 1u));}});
goto _LL1343; _LL1343:;}{ void* _temp1355=( void*) e1->r; struct Cyc_Absyn_Exp*
_temp1367; struct Cyc_Absyn_Exp* _temp1369; struct Cyc_Absyn_Vardecl* _temp1371;
struct Cyc_List_List* _temp1373; struct Cyc_Core_Opt* _temp1375; struct Cyc_List_List*
_temp1377; void* _temp1379; struct _tagged_arr _temp1381; _LL1357: if(*(( int*)
_temp1355) ==  Cyc_Absyn_Comprehension_e){ _LL1372: _temp1371=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1355)->f1; goto _LL1370; _LL1370: _temp1369=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1355)->f2; goto _LL1368; _LL1368: _temp1367=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1355)->f3; goto _LL1358;} else{ goto _LL1359;} _LL1359: if(*(( int*)
_temp1355) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1376: _temp1375=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1355)->f1; goto _LL1374; _LL1374: _temp1373=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1355)->f2; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(*(( int*)
_temp1355) ==  Cyc_Absyn_Array_e){ _LL1378: _temp1377=(( struct Cyc_Absyn_Array_e_struct*)
_temp1355)->f1; goto _LL1362;} else{ goto _LL1363;} _LL1363: if(*(( int*)
_temp1355) ==  Cyc_Absyn_Const_e){ _LL1380: _temp1379=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1355)->f1; if(( unsigned int) _temp1379 >  1u?*(( int*) _temp1379) ==  Cyc_Absyn_String_c:
0){ _LL1382: _temp1381=(( struct Cyc_Absyn_String_c_struct*) _temp1379)->f1;
goto _LL1364;} else{ goto _LL1365;}} else{ goto _LL1365;} _LL1365: goto _LL1366;
_LL1358: { void* _temp1383= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1384= Cyc_Tcutil_compress( _temp1383); struct Cyc_Absyn_Tqual _temp1390;
void* _temp1392; _LL1386: if(( unsigned int) _temp1384 >  4u?*(( int*) _temp1384)
==  Cyc_Absyn_ArrayType: 0){ _LL1393: _temp1392=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1384)->f1; goto _LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1384)->f2; goto _LL1387;} else{ goto _LL1388;} _LL1388: goto _LL1389;
_LL1387: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1369)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1398=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1398[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1399; _temp1399.tag= Cyc_Absyn_Upper_b; _temp1399.f1=
_temp1369; _temp1399;}); _temp1398;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1395=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1395[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1396; _temp1396.tag= Cyc_Absyn_PointerType;
_temp1396.f1=({ struct Cyc_Absyn_PtrInfo _temp1397; _temp1397.elt_typ=( void*)
_temp1392; _temp1397.rgn_typ=( void*) rgn; _temp1397.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1397.tq= _temp1390; _temp1397.bounds= Cyc_Absyn_new_conref(
b); _temp1397;}); _temp1396;}); _temp1395;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1394=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1394->v=( void*) res_typ; _temp1394;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1389: return({ void* _temp1400[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1400, sizeof( void*), 0u));});
_LL1385:;} _LL1360:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1401=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1401[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1402; _temp1402.tag= Cyc_Absyn_Array_e;
_temp1402.f1= _temp1373; _temp1402;}); _temp1401;}))); _temp1377= _temp1373;
goto _LL1362; _LL1362: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1403=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1409; struct Cyc_Absyn_Tqual _temp1411; void* _temp1413; void** _temp1415;
_LL1405: if(( unsigned int) _temp1403 >  4u?*(( int*) _temp1403) ==  Cyc_Absyn_PointerType:
0){ _LL1410: _temp1409=(( struct Cyc_Absyn_PointerType_struct*) _temp1403)->f1;
_LL1414: _temp1413=( void*) _temp1409.elt_typ; _temp1415=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1403)->f1).elt_typ; goto _LL1412; _LL1412:
_temp1411= _temp1409.tq; goto _LL1406;} else{ goto _LL1407;} _LL1407: goto
_LL1408; _LL1406: elt_typ_opt=( void**) _temp1415; goto _LL1404; _LL1408: goto
_LL1404; _LL1404:;}{ void* _temp1416= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt,
e1); void* res_typ;{ void* _temp1417= Cyc_Tcutil_compress( _temp1416); struct
Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Tqual _temp1425; void* _temp1427;
_LL1419: if(( unsigned int) _temp1417 >  4u?*(( int*) _temp1417) ==  Cyc_Absyn_ArrayType:
0){ _LL1428: _temp1427=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1417)->f1;
goto _LL1426; _LL1426: _temp1425=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1417)->f2; goto _LL1424; _LL1424: _temp1423=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1417)->f3; goto _LL1420;} else{ goto _LL1421;} _LL1421: goto _LL1422;
_LL1420: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1429=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1429[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1430; _temp1430.tag=
Cyc_Absyn_PointerType; _temp1430.f1=({ struct Cyc_Absyn_PtrInfo _temp1431;
_temp1431.elt_typ=( void*) _temp1427; _temp1431.rgn_typ=( void*) rgn; _temp1431.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1431.tq=
_temp1425; _temp1431.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1432=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1432[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1433; _temp1433.tag= Cyc_Absyn_Upper_b;
_temp1433.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1423); _temp1433;});
_temp1432;})); _temp1431;}); _temp1430;}); _temp1429;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1434=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1434->v=( void*) res_typ; _temp1434;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1418; _LL1422: return({ void* _temp1435[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1435, sizeof( void*), 0u));});
_LL1418:;} return res_typ;}} _LL1364: { void* _temp1436= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1437= Cyc_Tcexp_tcExp(
te,( void**)& _temp1436, e1); return Cyc_Absyn_atb_typ( _temp1437, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1438=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1438[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1439; _temp1439.tag= Cyc_Absyn_Upper_b; _temp1439.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1439;}); _temp1438;}));} _LL1366: { void** topt2=
0; if( topt !=  0){ void* _temp1440= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1448; struct Cyc_Absyn_Conref* _temp1450;
void* _temp1452; void** _temp1454; _LL1442: if(( unsigned int) _temp1440 >  4u?*((
int*) _temp1440) ==  Cyc_Absyn_PointerType: 0){ _LL1449: _temp1448=(( struct Cyc_Absyn_PointerType_struct*)
_temp1440)->f1; _LL1453: _temp1452=( void*) _temp1448.elt_typ; _temp1454=( void**)&(((
struct Cyc_Absyn_PointerType_struct*) _temp1440)->f1).elt_typ; goto _LL1451;
_LL1451: _temp1450= _temp1448.nullable; goto _LL1443;} else{ goto _LL1444;}
_LL1444: if(( unsigned int) _temp1440 >  4u?*(( int*) _temp1440) ==  Cyc_Absyn_TunionType:
0){ goto _LL1445;} else{ goto _LL1446;} _LL1446: goto _LL1447; _LL1443: topt2=(
void**) _temp1454; goto _LL1441; _LL1445: topt2=({ void** _temp1455=( void**)
_cycalloc( sizeof( void*)); _temp1455[ 0]=*(( void**) _check_null( topt));
_temp1455;}); goto _LL1441; _LL1447: goto _LL1441; _LL1441:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1457=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1457[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1458; _temp1458.tag=
Cyc_Absyn_PointerType; _temp1458.f1=({ struct Cyc_Absyn_PtrInfo _temp1459;
_temp1459.elt_typ=( void*) telt; _temp1459.rgn_typ=( void*) rgn; _temp1459.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1459.tq= Cyc_Absyn_empty_tqual();
_temp1459.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1460=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1460[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1461; _temp1461.tag= Cyc_Absyn_Upper_b;
_temp1461.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1461;}); _temp1460;})); _temp1459;});
_temp1458;}); _temp1457;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1456=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1456->v=(
void*) res_typ; _temp1456;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1356:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress( Cyc_Tcexp_tcExpNoPromote(
te, topt, e)); void* _temp1462= t; struct Cyc_Absyn_Exp* _temp1468; struct Cyc_Absyn_Tqual
_temp1470; void* _temp1472; _LL1464: if(( unsigned int) _temp1462 >  4u?*(( int*)
_temp1462) ==  Cyc_Absyn_ArrayType: 0){ _LL1473: _temp1472=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1462)->f1; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1462)->f2; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1462)->f3; goto _LL1465;} else{ goto _LL1466;} _LL1466: goto _LL1467;
_LL1465: { void* _temp1476; struct _tuple6 _temp1474= Cyc_Tcutil_addressof_props(
te, e); _LL1477: _temp1476= _temp1474.f2; goto _LL1475; _LL1475: { void*
_temp1478= _temp1468 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1479=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1479[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1480; _temp1480.tag= Cyc_Absyn_Upper_b;
_temp1480.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1468); _temp1480;});
_temp1479;}); t= Cyc_Absyn_atb_typ( _temp1472, _temp1476, _temp1470, _temp1478);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1467: return t; _LL1463:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1481=( void*) e->r; void* _temp1491; _LL1483: if(*(( int*)
_temp1481) ==  Cyc_Absyn_Array_e){ goto _LL1484;} else{ goto _LL1485;} _LL1485:
if(*(( int*) _temp1481) ==  Cyc_Absyn_Comprehension_e){ goto _LL1486;} else{
goto _LL1487;} _LL1487: if(*(( int*) _temp1481) ==  Cyc_Absyn_Const_e){ _LL1492:
_temp1491=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1481)->f1; if((
unsigned int) _temp1491 >  1u?*(( int*) _temp1491) ==  Cyc_Absyn_String_c: 0){
goto _LL1488;} else{ goto _LL1489;}} else{ goto _LL1489;} _LL1489: goto _LL1490;
_LL1484: return t; _LL1486: return t; _LL1488: return t; _LL1490: t= Cyc_Tcutil_compress(
t);{ void* _temp1493= t; struct Cyc_Absyn_Exp* _temp1499; struct Cyc_Absyn_Tqual
_temp1501; void* _temp1503; _LL1495: if(( unsigned int) _temp1493 >  4u?*(( int*)
_temp1493) ==  Cyc_Absyn_ArrayType: 0){ _LL1504: _temp1503=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1493)->f1; goto _LL1502; _LL1502: _temp1501=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1493)->f2; goto _LL1500; _LL1500: _temp1499=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1493)->f3; goto _LL1496;} else{ goto _LL1497;} _LL1497: goto _LL1498;
_LL1496: { void* _temp1507; struct _tuple6 _temp1505= Cyc_Tcutil_addressof_props(
te, e); _LL1508: _temp1507= _temp1505.f2; goto _LL1506; _LL1506: { void* b=
_temp1499 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1509=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1509[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1510; _temp1510.tag= Cyc_Absyn_Upper_b;
_temp1510.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1499); _temp1510;});
_temp1509;}); t= Cyc_Absyn_atb_typ( _temp1503, _temp1507, _temp1501, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1498: return t; _LL1494:;} _LL1482:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1511=( void*) e->r; struct Cyc_Absyn_Exp* _temp1517; _LL1513: if(*(( int*)
_temp1511) ==  Cyc_Absyn_NoInstantiate_e){ _LL1518: _temp1517=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1511)->f1; goto _LL1514;} else{ goto _LL1515;} _LL1515: goto _LL1516;
_LL1514: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1517);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1517->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1517->topt))->v)); e->topt= _temp1517->topt;
goto _LL1512; _LL1516: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1519=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1525; struct Cyc_Absyn_Conref* _temp1527; struct
Cyc_Absyn_Tqual _temp1529; struct Cyc_Absyn_Conref* _temp1531; void* _temp1533;
void* _temp1535; _LL1521: if(( unsigned int) _temp1519 >  4u?*(( int*) _temp1519)
==  Cyc_Absyn_PointerType: 0){ _LL1526: _temp1525=(( struct Cyc_Absyn_PointerType_struct*)
_temp1519)->f1; _LL1536: _temp1535=( void*) _temp1525.elt_typ; goto _LL1534;
_LL1534: _temp1533=( void*) _temp1525.rgn_typ; goto _LL1532; _LL1532: _temp1531=
_temp1525.nullable; goto _LL1530; _LL1530: _temp1529= _temp1525.tq; goto _LL1528;
_LL1528: _temp1527= _temp1525.bounds; goto _LL1522;} else{ goto _LL1523;}
_LL1523: goto _LL1524; _LL1522:{ void* _temp1537= Cyc_Tcutil_compress( _temp1535);
struct Cyc_Absyn_FnInfo _temp1543; struct Cyc_List_List* _temp1545; struct Cyc_List_List*
_temp1547; struct Cyc_Absyn_VarargInfo* _temp1549; int _temp1551; struct Cyc_List_List*
_temp1553; void* _temp1555; struct Cyc_Core_Opt* _temp1557; struct Cyc_List_List*
_temp1559; _LL1539: if(( unsigned int) _temp1537 >  4u?*(( int*) _temp1537) == 
Cyc_Absyn_FnType: 0){ _LL1544: _temp1543=(( struct Cyc_Absyn_FnType_struct*)
_temp1537)->f1; _LL1560: _temp1559= _temp1543.tvars; goto _LL1558; _LL1558:
_temp1557= _temp1543.effect; goto _LL1556; _LL1556: _temp1555=( void*) _temp1543.ret_typ;
goto _LL1554; _LL1554: _temp1553= _temp1543.args; goto _LL1552; _LL1552:
_temp1551= _temp1543.c_varargs; goto _LL1550; _LL1550: _temp1549= _temp1543.cyc_varargs;
goto _LL1548; _LL1548: _temp1547= _temp1543.rgn_po; goto _LL1546; _LL1546:
_temp1545= _temp1543.attributes; goto _LL1540;} else{ goto _LL1541;} _LL1541:
goto _LL1542; _LL1540: if( _temp1559 !=  0){ struct _RegionHandle _temp1561=
_new_region(); struct _RegionHandle* rgn=& _temp1561; _push_region( rgn);{
struct _tuple4 _temp1562=({ struct _tuple4 _temp1572; _temp1572.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1572.f2= rgn; _temp1572;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1562, _temp1559); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1569=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1569[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1570; _temp1570.tag= Cyc_Absyn_FnType; _temp1570.f1=({ struct Cyc_Absyn_FnInfo
_temp1571; _temp1571.tvars= 0; _temp1571.effect= _temp1557; _temp1571.ret_typ=(
void*) _temp1555; _temp1571.args= _temp1553; _temp1571.c_varargs= _temp1551;
_temp1571.cyc_varargs= _temp1549; _temp1571.rgn_po= _temp1547; _temp1571.attributes=
_temp1545; _temp1571;}); _temp1570;}); _temp1569;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1566=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1566[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1567; _temp1567.tag= Cyc_Absyn_PointerType;
_temp1567.f1=({ struct Cyc_Absyn_PtrInfo _temp1568; _temp1568.elt_typ=( void*)
ftyp; _temp1568.rgn_typ=( void*) _temp1533; _temp1568.nullable= _temp1531;
_temp1568.tq= _temp1529; _temp1568.bounds= _temp1527; _temp1568;}); _temp1567;});
_temp1566;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1563=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1563[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1564; _temp1564.tag= Cyc_Absyn_Instantiate_e;
_temp1564.f1= inner; _temp1564.f2= ts; _temp1564;}); _temp1563;}))); e->topt=({
struct Cyc_Core_Opt* _temp1565=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1565->v=( void*) new_typ; _temp1565;});}; _pop_region( rgn);}
goto _LL1538; _LL1542: goto _LL1538; _LL1538:;} goto _LL1520; _LL1524: goto
_LL1520; _LL1520:;} goto _LL1512; _LL1512:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1573=( void*) e->r; struct Cyc_Absyn_Exp* _temp1647; struct
_tuple1* _temp1649; struct Cyc_List_List* _temp1651; struct Cyc_Absyn_Exp*
_temp1653; struct Cyc_List_List* _temp1655; struct Cyc_Core_Opt* _temp1657; void*
_temp1659; void* _temp1661; struct _tuple1* _temp1663; struct Cyc_List_List*
_temp1665; void* _temp1667; void* _temp1669; struct Cyc_Absyn_Exp* _temp1671;
struct Cyc_Absyn_Exp* _temp1673; struct Cyc_Core_Opt* _temp1675; struct Cyc_Absyn_Exp*
_temp1677; struct Cyc_Absyn_Exp* _temp1679; struct Cyc_Absyn_Exp* _temp1681;
struct Cyc_Absyn_Exp* _temp1683; struct Cyc_Absyn_Exp* _temp1685; struct Cyc_Absyn_Exp*
_temp1687; struct Cyc_Absyn_VarargCallInfo* _temp1689; struct Cyc_Absyn_VarargCallInfo**
_temp1691; struct Cyc_List_List* _temp1692; struct Cyc_Absyn_Exp* _temp1694;
struct Cyc_Absyn_Exp* _temp1696; struct Cyc_List_List* _temp1698; struct Cyc_Absyn_Exp*
_temp1700; struct Cyc_Absyn_Exp* _temp1702; void* _temp1704; struct Cyc_Absyn_Exp*
_temp1706; struct Cyc_Absyn_Exp* _temp1708; struct Cyc_Absyn_Exp* _temp1710;
struct Cyc_Absyn_Exp* _temp1712; void* _temp1714; struct _tagged_arr* _temp1716;
void* _temp1718; struct Cyc_Absyn_Exp* _temp1720; struct _tagged_arr* _temp1722;
struct Cyc_Absyn_Exp* _temp1724; struct _tagged_arr* _temp1726; struct Cyc_Absyn_Exp*
_temp1728; struct Cyc_Absyn_Exp* _temp1730; struct Cyc_Absyn_Exp* _temp1732;
struct Cyc_List_List* _temp1734; struct Cyc_List_List* _temp1736; struct _tuple2*
_temp1738; struct Cyc_List_List* _temp1740; struct Cyc_Absyn_Stmt* _temp1742;
struct Cyc_Absyn_Fndecl* _temp1744; struct Cyc_Absyn_Exp* _temp1746; struct Cyc_Absyn_Exp*
_temp1748; struct Cyc_Absyn_Exp* _temp1750; struct Cyc_Absyn_Vardecl* _temp1752;
struct Cyc_Absyn_Structdecl* _temp1754; struct Cyc_Absyn_Structdecl** _temp1756;
struct Cyc_List_List* _temp1757; struct Cyc_Core_Opt* _temp1759; struct Cyc_Core_Opt**
_temp1761; struct _tuple1* _temp1762; struct _tuple1** _temp1764; struct Cyc_List_List*
_temp1765; void* _temp1767; struct Cyc_Absyn_Tunionfield* _temp1769; struct Cyc_Absyn_Tuniondecl*
_temp1771; struct Cyc_List_List* _temp1773; struct Cyc_Core_Opt* _temp1775;
struct Cyc_Core_Opt** _temp1777; struct Cyc_Core_Opt* _temp1778; struct Cyc_Core_Opt**
_temp1780; struct Cyc_Absyn_Enumfield* _temp1781; struct Cyc_Absyn_Enumdecl*
_temp1783; struct _tuple1* _temp1785; struct _tuple1** _temp1787; void*
_temp1788; struct Cyc_Absyn_Exp* _temp1790; _LL1575: if(*(( int*) _temp1573) == 
Cyc_Absyn_NoInstantiate_e){ _LL1648: _temp1647=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1573)->f1; goto _LL1576;} else{ goto _LL1577;} _LL1577: if(*(( int*)
_temp1573) ==  Cyc_Absyn_UnknownId_e){ _LL1650: _temp1649=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1573)->f1; goto _LL1578;} else{ goto _LL1579;} _LL1579: if(*(( int*)
_temp1573) ==  Cyc_Absyn_UnknownCall_e){ _LL1654: _temp1653=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1573)->f1; goto _LL1652; _LL1652: _temp1651=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1573)->f2; goto _LL1580;} else{ goto _LL1581;} _LL1581: if(*(( int*)
_temp1573) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1658: _temp1657=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1573)->f1; goto _LL1656; _LL1656: _temp1655=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1573)->f2; goto _LL1582;} else{ goto _LL1583;} _LL1583: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Const_e){ _LL1660: _temp1659=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1573)->f1; goto _LL1584;} else{ goto _LL1585;} _LL1585: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Var_e){ _LL1664: _temp1663=(( struct Cyc_Absyn_Var_e_struct*)
_temp1573)->f1; goto _LL1662; _LL1662: _temp1661=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1573)->f2; goto _LL1586;} else{ goto _LL1587;} _LL1587: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Primop_e){ _LL1668: _temp1667=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1573)->f1; goto _LL1666; _LL1666: _temp1665=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1573)->f2; goto _LL1588;} else{ goto _LL1589;} _LL1589: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Increment_e){ _LL1672: _temp1671=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1573)->f1; goto _LL1670; _LL1670: _temp1669=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1573)->f2; goto _LL1590;} else{ goto _LL1591;} _LL1591: if(*(( int*)
_temp1573) ==  Cyc_Absyn_AssignOp_e){ _LL1678: _temp1677=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1573)->f1; goto _LL1676; _LL1676: _temp1675=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1573)->f2; goto _LL1674; _LL1674: _temp1673=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1573)->f3; goto _LL1592;} else{ goto _LL1593;} _LL1593: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Conditional_e){ _LL1684: _temp1683=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1573)->f1; goto _LL1682; _LL1682: _temp1681=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1573)->f2; goto _LL1680; _LL1680: _temp1679=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1573)->f3; goto _LL1594;} else{ goto _LL1595;} _LL1595: if(*(( int*)
_temp1573) ==  Cyc_Absyn_SeqExp_e){ _LL1688: _temp1687=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1573)->f1; goto _LL1686; _LL1686: _temp1685=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1573)->f2; goto _LL1596;} else{ goto _LL1597;} _LL1597: if(*(( int*)
_temp1573) ==  Cyc_Absyn_FnCall_e){ _LL1695: _temp1694=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1573)->f1; goto _LL1693; _LL1693: _temp1692=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1573)->f2; goto _LL1690; _LL1690: _temp1689=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1573)->f3; _temp1691=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1573)->f3; goto _LL1598;} else{ goto _LL1599;} _LL1599: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Throw_e){ _LL1697: _temp1696=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1573)->f1; goto _LL1600;} else{ goto _LL1601;} _LL1601: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Instantiate_e){ _LL1701: _temp1700=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1573)->f1; goto _LL1699; _LL1699: _temp1698=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1573)->f2; goto _LL1602;} else{ goto _LL1603;} _LL1603: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Cast_e){ _LL1705: _temp1704=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1573)->f1; goto _LL1703; _LL1703: _temp1702=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1573)->f2; goto _LL1604;} else{ goto _LL1605;} _LL1605: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Address_e){ _LL1707: _temp1706=(( struct Cyc_Absyn_Address_e_struct*)
_temp1573)->f1; goto _LL1606;} else{ goto _LL1607;} _LL1607: if(*(( int*)
_temp1573) ==  Cyc_Absyn_New_e){ _LL1711: _temp1710=(( struct Cyc_Absyn_New_e_struct*)
_temp1573)->f1; goto _LL1709; _LL1709: _temp1708=(( struct Cyc_Absyn_New_e_struct*)
_temp1573)->f2; goto _LL1608;} else{ goto _LL1609;} _LL1609: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Sizeofexp_e){ _LL1713: _temp1712=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1573)->f1; goto _LL1610;} else{ goto _LL1611;} _LL1611: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1715: _temp1714=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1573)->f1; goto _LL1612;} else{ goto _LL1613;} _LL1613: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Offsetof_e){ _LL1719: _temp1718=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1573)->f1; goto _LL1717; _LL1717: _temp1716=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1573)->f2; goto _LL1614;} else{ goto _LL1615;} _LL1615: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Deref_e){ _LL1721: _temp1720=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1573)->f1; goto _LL1616;} else{ goto _LL1617;} _LL1617: if(*(( int*)
_temp1573) ==  Cyc_Absyn_StructMember_e){ _LL1725: _temp1724=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1573)->f1; goto _LL1723; _LL1723: _temp1722=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1573)->f2; goto _LL1618;} else{ goto _LL1619;} _LL1619: if(*(( int*)
_temp1573) ==  Cyc_Absyn_StructArrow_e){ _LL1729: _temp1728=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1573)->f1; goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1573)->f2; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Subscript_e){ _LL1733: _temp1732=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1573)->f1; goto _LL1731; _LL1731: _temp1730=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1573)->f2; goto _LL1622;} else{ goto _LL1623;} _LL1623: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Tuple_e){ _LL1735: _temp1734=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1573)->f1; goto _LL1624;} else{ goto _LL1625;} _LL1625: if(*(( int*)
_temp1573) ==  Cyc_Absyn_CompoundLit_e){ _LL1739: _temp1738=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1573)->f1; goto _LL1737; _LL1737: _temp1736=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1573)->f2; goto _LL1626;} else{ goto _LL1627;} _LL1627: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Array_e){ _LL1741: _temp1740=(( struct Cyc_Absyn_Array_e_struct*)
_temp1573)->f1; goto _LL1628;} else{ goto _LL1629;} _LL1629: if(*(( int*)
_temp1573) ==  Cyc_Absyn_StmtExp_e){ _LL1743: _temp1742=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1573)->f1; goto _LL1630;} else{ goto _LL1631;} _LL1631: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Codegen_e){ _LL1745: _temp1744=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1573)->f1; goto _LL1632;} else{ goto _LL1633;} _LL1633: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Fill_e){ _LL1747: _temp1746=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1573)->f1; goto _LL1634;} else{ goto _LL1635;} _LL1635: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Comprehension_e){ _LL1753: _temp1752=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1573)->f1; goto _LL1751; _LL1751: _temp1750=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1573)->f2; goto _LL1749; _LL1749: _temp1748=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1573)->f3; goto _LL1636;} else{ goto _LL1637;} _LL1637: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Struct_e){ _LL1763: _temp1762=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f1; _temp1764=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f1; goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f2; _temp1761=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f2; goto _LL1758; _LL1758: _temp1757=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f3; goto _LL1755; _LL1755: _temp1754=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f4; _temp1756=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1573)->f4; goto _LL1638;} else{ goto _LL1639;} _LL1639: if(*(( int*)
_temp1573) ==  Cyc_Absyn_AnonStruct_e){ _LL1768: _temp1767=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1573)->f1; goto _LL1766; _LL1766: _temp1765=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1573)->f2; goto _LL1640;} else{ goto _LL1641;} _LL1641: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Tunion_e){ _LL1779: _temp1778=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f1; _temp1780=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f1; goto _LL1776; _LL1776: _temp1775=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f2; _temp1777=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f2; goto _LL1774; _LL1774: _temp1773=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f3; goto _LL1772; _LL1772: _temp1771=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f4; goto _LL1770; _LL1770: _temp1769=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1573)->f5; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Enum_e){ _LL1786: _temp1785=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1573)->f1; _temp1787=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1573)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1573)->f2; goto _LL1782; _LL1782: _temp1781=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1573)->f3; goto _LL1644;} else{ goto _LL1645;} _LL1645: if(*(( int*)
_temp1573) ==  Cyc_Absyn_Malloc_e){ _LL1791: _temp1790=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1573)->f1; goto _LL1789; _LL1789: _temp1788=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1573)->f2; goto _LL1646;} else{ goto _LL1574;} _LL1576: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1647); return; _LL1578: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1649); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1580: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1653, _temp1651); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1582: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1655); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1584: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1659, e);
goto _LL1574; _LL1586: t= Cyc_Tcexp_tcVar( te, loc, _temp1663, _temp1661); goto
_LL1574; _LL1588: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1667, _temp1665);
goto _LL1574; _LL1590: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1671,
_temp1669); goto _LL1574; _LL1592: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1677, _temp1675, _temp1673); goto _LL1574; _LL1594: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1683, _temp1681, _temp1679); goto _LL1574; _LL1596: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1687, _temp1685); goto _LL1574; _LL1598: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1694, _temp1692, _temp1691); goto _LL1574; _LL1600: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1696); goto _LL1574; _LL1602: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1700, _temp1698); goto _LL1574; _LL1604: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1704, _temp1702); goto _LL1574; _LL1606: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1706); goto _LL1574; _LL1608: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1710, e, _temp1708); goto _LL1574; _LL1610: { void* _temp1792= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1712); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1792); goto
_LL1574;} _LL1612: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1714); goto
_LL1574; _LL1614: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1718, _temp1716);
goto _LL1574; _LL1616: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1720); goto
_LL1574; _LL1618: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1724,
_temp1722); goto _LL1574; _LL1620: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1728, _temp1726); goto _LL1574; _LL1622: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1732, _temp1730); goto _LL1574; _LL1624: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1734); goto _LL1574; _LL1626: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1738, _temp1736); goto _LL1574; _LL1628: { void** elt_topt= 0;
if( topt !=  0){ void* _temp1793= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp1799; void** _temp1801; _LL1795: if(( unsigned int)
_temp1793 >  4u?*(( int*) _temp1793) ==  Cyc_Absyn_ArrayType: 0){ _LL1800:
_temp1799=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1793)->f1;
_temp1801=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1793)->f1);
goto _LL1796;} else{ goto _LL1797;} _LL1797: goto _LL1798; _LL1796: elt_topt=(
void**) _temp1801; goto _LL1794; _LL1798: goto _LL1794; _LL1794:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp1740); goto _LL1574;} _LL1630: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1742); goto _LL1574; _LL1632: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1744); goto _LL1574; _LL1634: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1746); goto _LL1574; _LL1636: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1752, _temp1750, _temp1748); goto _LL1574; _LL1638: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1764, _temp1761, _temp1757, _temp1756); goto _LL1574;
_LL1640: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp1767, _temp1765); goto _LL1574;
_LL1642: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp1780, _temp1777,
_temp1773, _temp1771, _temp1769); goto _LL1574; _LL1644:* _temp1787=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp1781))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1802=( struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1802[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1803; _temp1803.tag= Cyc_Absyn_EnumType;
_temp1803.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1783))->name;
_temp1803.f2= _temp1783; _temp1803;}); _temp1802;}); goto _LL1574; _LL1646: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1790, _temp1788); goto _LL1574; _LL1574:;}
e->topt=({ struct Cyc_Core_Opt* _temp1804=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1804->v=( void*) t; _temp1804;});}
