 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _tagged_arr Cyc_Core_string_of_int(
int); extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
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
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Nocommon_att= 6;
static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
12; static const int Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att=
14; static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
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
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned char Cyc_Tcdecl_Incompatible[
17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base;
struct Cyc_Absyn_Tunionfield* field; } ; extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg0, struct _tagged_arr msg1); struct _tuple3{ void*
f1; int f2; } ; extern struct _tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl( struct
Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct
Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f, int* res, struct
_tagged_arr* v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg);
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; extern unsigned char
Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2); extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); unsigned char Cyc_Tcdecl_Incompatible[
17u]="\000\000\000\000Incompatible"; void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg1, struct _tagged_arr msg2){ if( msg1 == 0){( int)
_throw(( void*) Cyc_Tcdecl_Incompatible);} else{ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp0=*(( struct _tagged_arr*) _check_null(
msg1)); struct _tagged_arr _temp1= msg2; xprintf("%.*s %.*s", _temp0.last_plus_one
- _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr, _temp1.curr);}));}}
static void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct _tagged_arr t,
struct _tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp2= t;
struct _tagged_arr _temp3= v; struct _tagged_arr _temp4= Cyc_Absynpp_scope2string(
s1); struct _tagged_arr _temp5= Cyc_Absynpp_scope2string( s0); xprintf("%.*s %.*s is %.*s whereas expected scope is %.*s",
_temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one - _temp3.curr,
_temp3.curr, _temp4.last_plus_one - _temp4.curr, _temp4.curr, _temp5.last_plus_one
- _temp5.curr, _temp5.curr);}));} struct _tuple4{ void* f1; void* f2; } ; struct
_tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1, struct _tagged_arr t, struct
_tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){{
struct _tuple4 _temp7=({ struct _tuple4 _temp6; _temp6.f1= s0; _temp6.f2= s1;
_temp6;}); void* _temp27; void* _temp29; void* _temp31; void* _temp33; void*
_temp35; void* _temp37; void* _temp39; void* _temp41; void* _temp43; void*
_temp45; void* _temp47; void* _temp49; void* _temp51; void* _temp53; void*
_temp55; void* _temp57; _LL9: _LL30: _temp29= _temp7.f1; if( _temp29 ==( void*)
Cyc_Absyn_ExternC){ goto _LL28;} else{ goto _LL11;} _LL28: _temp27= _temp7.f2;
if( _temp27 ==( void*) Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11:
_LL34: _temp33= _temp7.f1; if( _temp33 ==( void*) Cyc_Absyn_ExternC){ goto _LL32;}
else{ goto _LL13;} _LL32: _temp31= _temp7.f2; goto _LL12; _LL13: _LL38: _temp37=
_temp7.f1; goto _LL36; _LL36: _temp35= _temp7.f2; if( _temp35 ==( void*) Cyc_Absyn_ExternC){
goto _LL14;} else{ goto _LL15;} _LL15: _LL42: _temp41= _temp7.f1; goto _LL40;
_LL40: _temp39= _temp7.f2; if( _temp39 ==( void*) Cyc_Absyn_Extern){ goto _LL16;}
else{ goto _LL17;} _LL17: _LL46: _temp45= _temp7.f1; if( _temp45 ==( void*) Cyc_Absyn_Extern){
goto _LL44;} else{ goto _LL19;} _LL44: _temp43= _temp7.f2; goto _LL18; _LL19:
_LL50: _temp49= _temp7.f1; if( _temp49 ==( void*) Cyc_Absyn_Static){ goto _LL48;}
else{ goto _LL21;} _LL48: _temp47= _temp7.f2; if( _temp47 ==( void*) Cyc_Absyn_Static){
goto _LL20;} else{ goto _LL21;} _LL21: _LL54: _temp53= _temp7.f1; if( _temp53 ==(
void*) Cyc_Absyn_Public){ goto _LL52;} else{ goto _LL23;} _LL52: _temp51= _temp7.f2;
if( _temp51 ==( void*) Cyc_Absyn_Public){ goto _LL22;} else{ goto _LL23;} _LL23:
_LL58: _temp57= _temp7.f1; if( _temp57 ==( void*) Cyc_Absyn_Abstract){ goto
_LL56;} else{ goto _LL25;} _LL56: _temp55= _temp7.f2; if( _temp55 ==( void*) Cyc_Absyn_Abstract){
goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL10: goto _LL8; _LL12: goto
_LL14; _LL14: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp59; _temp59.f1= s1; _temp59.f2= 0; _temp59;}); _LL16: s1= s0;
goto _LL8; _LL18: goto _LL8; _LL20: goto _LL8; _LL22: goto _LL8; _LL24: goto
_LL8; _LL26: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp60; _temp60.f1= s1; _temp60.f2= 0; _temp60;}); _LL8:;}
return({ struct _tuple3 _temp61; _temp61.f1= s1; _temp61.f2= 1; _temp61;});}
static int Cyc_Tcdecl_check_type( void* t0, void* t1){ return Cyc_Tcutil_unify(
t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value( struct Cyc_Absyn_Exp*
e){ void* _temp62=( void*) e->r; void* _temp68; int _temp70; void* _temp72;
_LL64: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){ _LL69: _temp68=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if(( unsigned int) _temp68 > 1u?*((
int*) _temp68) == Cyc_Absyn_Int_c: 0){ _LL73: _temp72=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp68)->f1; goto _LL71; _LL71: _temp70=(( struct Cyc_Absyn_Int_c_struct*)
_temp68)->f2; goto _LL65;} else{ goto _LL66;}} else{ goto _LL66;} _LL66: goto
_LL67; _LL65: return( unsigned int) _temp70; _LL67:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp74=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp74[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp75; _temp75.tag= Cyc_Core_InvalidArg; _temp75.f1=
_tag_arr("Tcdecl::get_uint_const_value", sizeof( unsigned char), 29u); _temp75;});
_temp74;})); _LL63:;} inline static int Cyc_Tcdecl_check_tvs( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1, struct _tagged_arr t, struct _tagged_arr v,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs0) !=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp76= t; struct _tagged_arr _temp77= v; xprintf("%.*s %.*s has a different number of type parameters",
_temp76.last_plus_one - _temp76.curr, _temp76.curr, _temp77.last_plus_one -
_temp77.curr, _temp77.curr);})); return 0;}{ struct Cyc_List_List* _temp78= tvs0;
goto _LL79; _LL79: { struct Cyc_List_List* _temp80= tvs1; goto _LL81; _LL81:
for( 0; _temp78 != 0;( _temp78=(( struct Cyc_List_List*) _check_null( _temp78))->tl,
_temp80=(( struct Cyc_List_List*) _check_null( _temp80))->tl)){ struct Cyc_Absyn_Conref*
_temp82=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp78))->hd)->kind);
goto _LL83; _LL83: { struct Cyc_Absyn_Conref* _temp84=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp80))->hd)->kind); goto _LL85; _LL85:
if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp82) !=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp84)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp86= t; struct
_tagged_arr _temp87= v; struct _tagged_arr _temp88= Cyc_Absynpp_ckind2string(
_temp82); struct _tagged_arr _temp89=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp78))->hd)->name; struct _tagged_arr _temp90= Cyc_Absynpp_ckind2string(
_temp84); xprintf("%.*s %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp86.last_plus_one - _temp86.curr, _temp86.curr, _temp87.last_plus_one -
_temp87.curr, _temp87.curr, _temp88.last_plus_one - _temp88.curr, _temp88.curr,
_temp89.last_plus_one - _temp89.curr, _temp89.curr, _temp90.last_plus_one -
_temp90.curr, _temp90.curr);})); return 0;}}} return 1;}}} inline static int Cyc_Tcdecl_check_atts(
struct Cyc_List_List* atts0, struct Cyc_List_List* atts1, struct _tagged_arr t,
struct _tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp91= t; struct _tagged_arr _temp92= v;
xprintf("%.*s %.*s has different attributes", _temp91.last_plus_one - _temp91.curr,
_temp91.curr, _temp92.last_plus_one - _temp92.curr, _temp92.curr);})); return 0;}
else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* _temp93= 0; goto _LL94;
_LL94: for( 0; tvs0 != 0;( tvs0=(( struct Cyc_List_List*) _check_null( tvs0))->tl,
tvs1=(( struct Cyc_List_List*) _check_null( tvs1))->tl)){ _temp93=({ struct Cyc_List_List*
_temp95=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp95->hd=( void*)({ struct _tuple5* _temp96=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp96->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs1))->hd; _temp96->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp97=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp97[ 0]=({ struct Cyc_Absyn_VarType_struct _temp98; _temp98.tag= Cyc_Absyn_VarType;
_temp98.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs0))->hd;
_temp98;}); _temp97;}); _temp96;}); _temp95->tl= _temp93; _temp95;});} return
_temp93;} struct _tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_arr t, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tagged_arr _temp99= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( d0->name))->v); goto _LL100; _LL100: { int _temp101=
1; goto _LL102; _LL102: if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, t, _temp99,
loc, msg)){ return 0;}{ int _temp105; void* _temp107; struct _tuple3 _temp103=
Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc, t, _temp99, loc, msg);
_LL108: _temp107= _temp103.f1; goto _LL106; _LL106: _temp105= _temp103.f2; goto
_LL104; _LL104: if( ! _temp105){ _temp101= 0;} if( ! Cyc_Tcdecl_check_atts( d0->attributes,
d1->attributes, t, _temp99, loc, msg)){ _temp101= 0;}{ struct Cyc_Absyn_Structdecl*
d2;{ struct _tuple6 _temp110=({ struct _tuple6 _temp109; _temp109.f1= d0->fields;
_temp109.f2= d1->fields; _temp109;}); struct Cyc_Core_Opt* _temp118; struct Cyc_Core_Opt*
_temp120; struct Cyc_Core_Opt* _temp122; struct Cyc_Core_Opt* _temp124; struct
Cyc_Core_Opt* _temp126; struct Cyc_Core_Opt _temp128; struct Cyc_List_List*
_temp129; struct Cyc_Core_Opt* _temp131; struct Cyc_Core_Opt _temp133; struct
Cyc_List_List* _temp134; _LL112: _LL121: _temp120= _temp110.f1; goto _LL119;
_LL119: _temp118= _temp110.f2; if( _temp118 == 0){ goto _LL113;} else{ goto
_LL114;} _LL114: _LL125: _temp124= _temp110.f1; if( _temp124 == 0){ goto _LL123;}
else{ goto _LL116;} _LL123: _temp122= _temp110.f2; goto _LL115; _LL116: _LL132:
_temp131= _temp110.f1; if( _temp131 == 0){ goto _LL111;} else{ _temp133=*
_temp131; _LL135: _temp134=( struct Cyc_List_List*) _temp133.v; goto _LL127;}
_LL127: _temp126= _temp110.f2; if( _temp126 == 0){ goto _LL111;} else{ _temp128=*
_temp126; _LL130: _temp129=( struct Cyc_List_List*) _temp128.v; goto _LL117;}
_LL113: d2= d0; goto _LL111; _LL115: d2= d1; goto _LL111; _LL117: { struct Cyc_List_List*
_temp136= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); goto _LL137; _LL137: for(
0; _temp134 != 0? _temp129 != 0: 0;( _temp134=(( struct Cyc_List_List*)
_check_null( _temp134))->tl, _temp129=(( struct Cyc_List_List*) _check_null(
_temp129))->tl)){ struct Cyc_Absyn_Structfield _temp140; struct Cyc_List_List*
_temp141; struct Cyc_Core_Opt* _temp143; void* _temp145; struct Cyc_Absyn_Tqual
_temp147; struct _tagged_arr* _temp149; struct Cyc_Absyn_Structfield* _temp138=(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp134))->hd;
_temp140=* _temp138; _LL150: _temp149= _temp140.name; goto _LL148; _LL148:
_temp147= _temp140.tq; goto _LL146; _LL146: _temp145=( void*) _temp140.type;
goto _LL144; _LL144: _temp143= _temp140.width; goto _LL142; _LL142: _temp141=
_temp140.attributes; goto _LL139; _LL139: { struct Cyc_Absyn_Structfield
_temp153; struct Cyc_List_List* _temp154; struct Cyc_Core_Opt* _temp156; void*
_temp158; struct Cyc_Absyn_Tqual _temp160; struct _tagged_arr* _temp162; struct
Cyc_Absyn_Structfield* _temp151=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp129))->hd; _temp153=* _temp151; _LL163: _temp162= _temp153.name;
goto _LL161; _LL161: _temp160= _temp153.tq; goto _LL159; _LL159: _temp158=( void*)
_temp153.type; goto _LL157; _LL157: _temp156= _temp153.width; goto _LL155;
_LL155: _temp154= _temp153.attributes; goto _LL152; _LL152: if( Cyc_String_zstrptrcmp(
_temp149, _temp162) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp164= t; struct _tagged_arr _temp165= _temp99; struct
_tagged_arr _temp166=* _temp149; struct _tagged_arr _temp167=* _temp162; xprintf("%.*s %.*s : field name mismatch %.*s != %.*s",
_temp164.last_plus_one - _temp164.curr, _temp164.curr, _temp165.last_plus_one -
_temp165.curr, _temp165.curr, _temp166.last_plus_one - _temp166.curr, _temp166.curr,
_temp167.last_plus_one - _temp167.curr, _temp167.curr);})); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp141, _temp154)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp168= t; struct _tagged_arr _temp169= _temp99; struct
_tagged_arr _temp170=* _temp149; xprintf("%.*s %.*s : attribute mismatch on field %.*s",
_temp168.last_plus_one - _temp168.curr, _temp168.curr, _temp169.last_plus_one -
_temp169.curr, _temp169.curr, _temp170.last_plus_one - _temp170.curr, _temp170.curr);}));
_temp101= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp147, _temp160)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp171= t; struct
_tagged_arr _temp172= _temp99; struct _tagged_arr _temp173=* _temp149; xprintf("%.*s %.*s : qualifier mismatch on field %.*s",
_temp171.last_plus_one - _temp171.curr, _temp171.curr, _temp172.last_plus_one -
_temp172.curr, _temp172.curr, _temp173.last_plus_one - _temp173.curr, _temp173.curr);}));
_temp101= 0;} if(((( _temp143 != 0? _temp156 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp143))->v) !=
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp156))->v): 0)? 1:( _temp143 == 0? _temp156 != 0: 0))? 1:(
_temp143 != 0? _temp156 == 0: 0)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp174= t; struct _tagged_arr _temp175=
_temp99; struct _tagged_arr _temp176=* _temp149; xprintf("%.*s %.*s : bitfield mismatch on field %.*s",
_temp174.last_plus_one - _temp174.curr, _temp174.curr, _temp175.last_plus_one -
_temp175.curr, _temp175.curr, _temp176.last_plus_one - _temp176.curr, _temp176.curr);}));
_temp101= 0;}{ void* subst_t1= Cyc_Tcutil_substitute( _temp136, _temp158); if( !
Cyc_Tcdecl_check_type( _temp145, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp177= t; struct _tagged_arr _temp178=
_temp99; struct _tagged_arr _temp179=* _temp149; struct _tagged_arr _temp180=
Cyc_Absynpp_typ2string( _temp145); struct _tagged_arr _temp181= Cyc_Absynpp_typ2string(
subst_t1); xprintf("%.*s %.*s : type mismatch on field %.*s: %.*s != %.*s",
_temp177.last_plus_one - _temp177.curr, _temp177.curr, _temp178.last_plus_one -
_temp178.curr, _temp178.curr, _temp179.last_plus_one - _temp179.curr, _temp179.curr,
_temp180.last_plus_one - _temp180.curr, _temp180.curr, _temp181.last_plus_one -
_temp181.curr, _temp181.curr);})); _temp101= 0;}}}} if( _temp134 != 0){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp182= t; struct
_tagged_arr _temp183= _temp99; struct _tagged_arr _temp184=*(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp134))->hd)->name; xprintf("%.*s %.*s is missing field %.*s",
_temp182.last_plus_one - _temp182.curr, _temp182.curr, _temp183.last_plus_one -
_temp183.curr, _temp183.curr, _temp184.last_plus_one - _temp184.curr, _temp184.curr);}));
_temp101= 0;} if( _temp129 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp185= t; struct _tagged_arr _temp186= _temp99; struct
_tagged_arr _temp187=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp129))->hd)->name; xprintf("%.*s %.*s has extra field %.*s",
_temp185.last_plus_one - _temp185.curr, _temp185.curr, _temp186.last_plus_one -
_temp186.curr, _temp186.curr, _temp187.last_plus_one - _temp187.curr, _temp187.curr);}));
_temp101= 0;} d2= d0; goto _LL111;} _LL111:;} if( ! _temp101){ return 0;} if(
_temp107 ==( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*) d2;} else{ d2=({
struct Cyc_Absyn_Structdecl* _temp188=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp188[ 0]=* d2; _temp188;});( void*)(
d2->sc=( void*) _temp107); return( struct Cyc_Absyn_Structdecl*) d2;}}}}} struct
Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0, d1,
_tag_arr("struct", sizeof( unsigned char), 7u), loc, msg);} struct Cyc_Absyn_Uniondecl*
Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ return( struct
Cyc_Absyn_Uniondecl*) Cyc_Tcdecl_merge_struct_or_union_decl(( struct Cyc_Absyn_Structdecl*)
d0,( struct Cyc_Absyn_Structdecl*) d1, _tag_arr("union", sizeof( unsigned char),
6u), loc, msg);} inline static struct _tagged_arr Cyc_Tcdecl_is_x2string( int
is_x){ if( is_x){ return _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{
return _tag_arr("tunion", sizeof( unsigned char), 7u);}} struct _tuple7{ struct
Cyc_Absyn_Tqual f1; void* f2; } ; static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_merge_tunionfield(
struct Cyc_Absyn_Tunionfield* f0, struct Cyc_Absyn_Tunionfield* f1, struct Cyc_List_List*
inst, struct _tagged_arr t, struct _tagged_arr v, struct _tagged_arr* msg){
struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_String_zstrptrcmp((* f0->name).f2,(*
f1->name).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp189= t; struct _tagged_arr _temp190= v; struct _tagged_arr
_temp191=*(* f1->name).f2; struct _tagged_arr _temp192=*(* f0->name).f2; xprintf("%.*s %.*s: field name mismatch %.*s != %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr, _temp190.last_plus_one -
_temp190.curr, _temp190.curr, _temp191.last_plus_one - _temp191.curr, _temp191.curr,
_temp192.last_plus_one - _temp192.curr, _temp192.curr);})); return 0;}{ struct
_tagged_arr _temp193=*(* f0->name).f2; goto _LL194; _LL194: { int _temp199; void*
_temp201; struct _tuple3 _temp197= Cyc_Tcdecl_merge_scope(( void*) f0->sc,( void*)
f1->sc,( struct _tagged_arr)({ struct _tagged_arr _temp195= t; struct
_tagged_arr _temp196= v; xprintf("in %.*s %.*s, field", _temp195.last_plus_one -
_temp195.curr, _temp195.curr, _temp196.last_plus_one - _temp196.curr, _temp196.curr);}),
_temp193, loc, msg); _LL202: _temp201= _temp197.f1; goto _LL200; _LL200:
_temp199= _temp197.f2; goto _LL198; _LL198: { struct Cyc_List_List* _temp203= f0->tvs;
goto _LL204; _LL204: { struct Cyc_List_List* _temp205= f1->tvs; goto _LL206;
_LL206: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp203) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp205)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp207= t; struct
_tagged_arr _temp208= v; struct _tagged_arr _temp209= _temp193; xprintf("%.*s %.*s, field %.*s: type parameter number mismatch",
_temp207.last_plus_one - _temp207.curr, _temp207.curr, _temp208.last_plus_one -
_temp208.curr, _temp208.curr, _temp209.last_plus_one - _temp209.curr, _temp209.curr);}));
return 0;}{ struct Cyc_List_List* _temp210= Cyc_Tcdecl_build_tvs_map( _temp203,
_temp205); goto _LL211; _LL211: { struct Cyc_List_List* _temp212= f0->typs; goto
_LL213; _LL213: { struct Cyc_List_List* _temp214= f1->typs; goto _LL215; _LL215:
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp212) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp214)){ Cyc_Tcdecl_merr( loc,
msg,( struct _tagged_arr)({ struct _tagged_arr _temp216= t; struct _tagged_arr
_temp217= v; struct _tagged_arr _temp218= _temp193; xprintf("%.*s %.*s, field %.*s: parameter number mismatch",
_temp216.last_plus_one - _temp216.curr, _temp216.curr, _temp217.last_plus_one -
_temp217.curr, _temp217.curr, _temp218.last_plus_one - _temp218.curr, _temp218.curr);}));
_temp199= 0;} for( 0; _temp212 != 0;( _temp212=(( struct Cyc_List_List*)
_check_null( _temp212))->tl, _temp214=(( struct Cyc_List_List*) _check_null(
_temp214))->tl)){ if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple7*)(( struct
Cyc_List_List*) _check_null( _temp212))->hd)).f1,(*(( struct _tuple7*)(( struct
Cyc_List_List*) _check_null( _temp214))->hd)).f1)){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr _temp219= t; struct _tagged_arr
_temp220= v; struct _tagged_arr _temp221= _temp193; xprintf("%.*s %.*s, field %.*s: parameter qualifier",
_temp219.last_plus_one - _temp219.curr, _temp219.curr, _temp220.last_plus_one -
_temp220.curr, _temp220.curr, _temp221.last_plus_one - _temp221.curr, _temp221.curr);}));
_temp199= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp210,
inst),(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp214))->hd)).f2);
if( ! Cyc_Tcdecl_check_type((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp212))->hd)).f2, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp222= t; struct _tagged_arr _temp223= v;
struct _tagged_arr _temp224= _temp193; struct _tagged_arr _temp225= Cyc_Absynpp_typ2string((*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp212))->hd)).f2);
struct _tagged_arr _temp226= Cyc_Absynpp_typ2string( subst_t1); xprintf("%.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp222.last_plus_one - _temp222.curr, _temp222.curr, _temp223.last_plus_one -
_temp223.curr, _temp223.curr, _temp224.last_plus_one - _temp224.curr, _temp224.curr,
_temp225.last_plus_one - _temp225.curr, _temp225.curr, _temp226.last_plus_one -
_temp226.curr, _temp226.curr);})); _temp199= 0;}}} if( ! _temp199){ return 0;}
if(( void*) f0->sc != _temp201){ struct Cyc_Absyn_Tunionfield* _temp228=({
struct Cyc_Absyn_Tunionfield* _temp227=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp227[ 0]=* f0; _temp227;});
goto _LL229; _LL229:( void*)((( struct Cyc_Absyn_Tunionfield*) _check_null(
_temp228))->sc=( void*) _temp201); return _temp228;} else{ return( struct Cyc_Absyn_Tunionfield*)
f0;}}}}}}}}} static int Cyc_Tcdecl_substitute_tunionfield_f1( struct _tagged_arr*
name, struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp( x->name, name) ==
0;} static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2( struct Cyc_List_List*
inst, struct _tuple7* x){ struct _tuple7 _temp232; void* _temp233; struct Cyc_Absyn_Tqual
_temp235; struct _tuple7* _temp230= x; _temp232=* _temp230; _LL236: _temp235=
_temp232.f1; goto _LL234; _LL234: _temp233= _temp232.f2; goto _LL231; _LL231:
return({ struct _tuple7* _temp237=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp237->f1= _temp235; _temp237->f2= Cyc_Tcutil_substitute( inst,
_temp233); _temp237;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp239=({
struct Cyc_Absyn_Tunionfield* _temp238=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp238[ 0]=* f1; _temp238;});
goto _LL240; _LL240: { struct Cyc_List_List* _temp241= 0; goto _LL242; _LL242:{
struct Cyc_List_List* _temp243= f1->tvs; goto _LL244; _LL244: for( 0; _temp243
!= 0; _temp243=(( struct Cyc_List_List*) _check_null( _temp243))->tl){ struct
Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp243))->hd; if((( int(*)( int(* pred)( struct _tagged_arr*,
struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)? 1:(( int(*)( int(* pred)(
struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name,
d1tvs)){ int i= 0; struct _tagged_arr base=* tv->name; struct _tagged_arr*
new_name; do { new_name=({ struct _tagged_arr* _temp245=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp245[ 0]=( struct _tagged_arr) Cyc_String_strconcat(
base,( struct _tagged_arr) Cyc_Core_string_of_int( i)); _temp245;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct
_tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp246=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp246[ 0]=* tv; _temp246;}); tv->name=
new_name;} _temp241=({ struct Cyc_List_List* _temp247=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp247->hd=( void*) tv; _temp247->tl=
_temp241; _temp247;});}} _temp241=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp241); _temp239->tvs= _temp241;{ struct Cyc_List_List*
_temp248= Cyc_Tcdecl_build_tvs_map( _temp241, f1->tvs); goto _LL249; _LL249: {
struct Cyc_List_List* _temp250=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp248, inst1); goto _LL251;
_LL251: _temp239->typs=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct
Cyc_List_List*, struct _tuple7*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2, _temp250, f1->typs);
return _temp239;}}}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp265=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp265[ 0]= 0; _temp265;}); struct Cyc_List_List** _temp252= f2sp; goto _LL253;
_LL253: { int cmp= - 1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*)
_check_null( f1s))->tl){ while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { struct Cyc_List_List*
_temp255=({ struct Cyc_List_List* _temp254=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp254->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd); _temp254->tl= 0; _temp254;});
goto _LL256; _LL256:* _temp252= _temp255; _temp252=&(( struct Cyc_List_List*)
_check_null( _temp255))->tl; f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;}
if( f0s == 0? 1: cmp > 0){* incl= 0;{ struct Cyc_List_List* _temp258=({ struct
Cyc_List_List* _temp257=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp257->hd=( void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd);
_temp257->tl= 0; _temp257;}); goto _LL259; _LL259:* _temp252= _temp258; _temp252=&((
struct Cyc_List_List*) _check_null( _temp258))->tl;}} else{ struct Cyc_Absyn_Tunionfield*
_temp260= Cyc_Tcdecl_merge_tunionfield(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( f0s))->hd,( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( f1s))->hd, inst, t, v, msg); goto _LL261; _LL261:
if( _temp260 != 0){ if( _temp260 !=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)){* incl= 0;}{ struct Cyc_List_List*
_temp263=({ struct Cyc_List_List* _temp262=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp262->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp260)); _temp262->tl= 0; _temp262;}); goto _LL264; _LL264:*
_temp252= _temp263; _temp252=&(( struct Cyc_List_List*) _check_null( _temp263))->tl;}}
else{* res= 0;} f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;}} if( f1s
!= 0){* incl= 0;* _temp252= f1s;} else{* _temp252= f0s;} return* f2sp;}} struct
_tuple8{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple8 Cyc_Tcdecl_split( struct Cyc_List_List* f){ if( f == 0){ return({ struct
_tuple8 _temp266; _temp266.f1= 0; _temp266.f2= 0; _temp266;});} if((( struct Cyc_List_List*)
_check_null( f))->tl == 0){ return({ struct _tuple8 _temp267; _temp267.f1= f;
_temp267.f2= 0; _temp267;});}{ struct Cyc_List_List* _temp270; struct Cyc_List_List*
_temp272; struct _tuple8 _temp268=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( f))->tl))->tl); _LL273: _temp272= _temp268.f1; goto _LL271; _LL271:
_temp270= _temp268.f2; goto _LL269; _LL269: return({ struct _tuple8 _temp274;
_temp274.f1=({ struct Cyc_List_List* _temp276=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp276->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( f))->hd); _temp276->tl= _temp272; _temp276;}); _temp274.f2=({
struct Cyc_List_List* _temp275=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp275->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( f))->tl))->hd); _temp275->tl=
_temp270; _temp275;}); _temp274;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_arr* v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct Cyc_List_List* _temp279; struct Cyc_List_List*
_temp281; struct _tuple8 _temp277=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL282: _temp281= _temp277.f1; goto _LL280; _LL280:
_temp279= _temp277.f2; goto _LL278; _LL278: if( _temp281 != 0?(( struct Cyc_List_List*)
_check_null( _temp281))->tl != 0: 0){ _temp281= Cyc_Tcdecl_sort_xtunion_fields(
_temp281, res, v, loc, msg);} if( _temp279 != 0?(( struct Cyc_List_List*)
_check_null( _temp279))->tl != 0: 0){ _temp279= Cyc_Tcdecl_sort_xtunion_fields(
_temp279, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp281,
_temp279, 0, 0, 0, res,({ int* _temp283=( int*) GC_malloc_atomic( sizeof( int));
_temp283[ 0]= 1; _temp283;}), _tag_arr("xtunion", sizeof( unsigned char), 8u),*
v, loc, msg);} struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl( struct
Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp284= Cyc_Absynpp_qvar2string(
d0->name); goto _LL285; _LL285: { struct _tagged_arr t= _tag_arr("[x]tunion",
sizeof( unsigned char), 10u); int _temp286= 1; goto _LL287; _LL287: if( d0->is_xtunion
!= d1->is_xtunion){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp288= _temp284; struct _tagged_arr _temp289= Cyc_Tcdecl_is_x2string(
d0->is_xtunion); struct _tagged_arr _temp290= Cyc_Tcdecl_is_x2string( d1->is_xtunion);
xprintf("expected %.*s to be a %.*s instead of a %.*s", _temp288.last_plus_one -
_temp288.curr, _temp288.curr, _temp289.last_plus_one - _temp289.curr, _temp289.curr,
_temp290.last_plus_one - _temp290.curr, _temp290.curr);})); _temp286= 0;} else{
t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs( d0->tvs,
d1->tvs, t, _temp284, loc, msg)){ return 0;}{ int _temp293; void* _temp295;
struct _tuple3 _temp291= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, _temp284, loc, msg); _LL296: _temp295= _temp291.f1; goto _LL294; _LL294:
_temp293= _temp291.f2; goto _LL292; _LL292: if( ! _temp293){ _temp286= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp298=({ struct _tuple6
_temp297; _temp297.f1= d0->fields; _temp297.f2= d1->fields; _temp297;}); struct
Cyc_Core_Opt* _temp306; struct Cyc_Core_Opt* _temp308; struct Cyc_Core_Opt*
_temp310; struct Cyc_Core_Opt* _temp312; struct Cyc_Core_Opt* _temp314; struct
Cyc_Core_Opt _temp316; struct Cyc_List_List* _temp317; struct Cyc_Core_Opt*
_temp319; struct Cyc_Core_Opt _temp321; struct Cyc_List_List* _temp322; _LL300:
_LL309: _temp308= _temp298.f1; goto _LL307; _LL307: _temp306= _temp298.f2; if(
_temp306 == 0){ goto _LL301;} else{ goto _LL302;} _LL302: _LL313: _temp312=
_temp298.f1; if( _temp312 == 0){ goto _LL311;} else{ goto _LL304;} _LL311:
_temp310= _temp298.f2; goto _LL303; _LL304: _LL320: _temp319= _temp298.f1; if(
_temp319 == 0){ goto _LL299;} else{ _temp321=* _temp319; _LL323: _temp322=(
struct Cyc_List_List*) _temp321.v; goto _LL315;} _LL315: _temp314= _temp298.f2;
if( _temp314 == 0){ goto _LL299;} else{ _temp316=* _temp314; _LL318: _temp317=(
struct Cyc_List_List*) _temp316.v; goto _LL305;} _LL301: d2= d0; goto _LL299;
_LL303: d2= d1; goto _LL299; _LL305: { struct Cyc_List_List* _temp324= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); goto _LL325; _LL325: if( d0->is_xtunion){ int _temp326= 1;
goto _LL327; _LL327: { struct Cyc_List_List* _temp328= Cyc_Tcdecl_merge_xtunion_fields(
_temp322, _temp317, _temp324, d0->tvs, d1->tvs,& _temp286,& _temp326, t,
_temp284, loc, msg); goto _LL329; _LL329: if( _temp326){ d2= d0;} else{ d2=({
struct Cyc_Absyn_Tuniondecl* _temp330=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp330[ 0]=* d0; _temp330;});( void*)(
d2->sc=( void*) _temp295); d2->fields=({ struct Cyc_Core_Opt* _temp331=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp331->v=( void*)
_temp328; _temp331;});}}} else{ for( 0; _temp322 != 0? _temp317 != 0: 0;(
_temp322=(( struct Cyc_List_List*) _check_null( _temp322))->tl, _temp317=((
struct Cyc_List_List*) _check_null( _temp317))->tl)){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp322))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp317))->hd,
_temp324, t, _temp284, msg);} if( _temp322 != 0){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr _temp332= t; struct _tagged_arr
_temp333= _temp284; struct _tagged_arr _temp334=*(*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp317))->hd)->name).f2; xprintf("%.*s %.*s has extra field %.*s",
_temp332.last_plus_one - _temp332.curr, _temp332.curr, _temp333.last_plus_one -
_temp333.curr, _temp333.curr, _temp334.last_plus_one - _temp334.curr, _temp334.curr);}));
_temp286= 0;} if( _temp317 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp335= t; struct _tagged_arr _temp336= _temp284; struct
_tagged_arr _temp337=*(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp317))->hd)->name).f2; xprintf("%.*s %.*s is missing field %.*s",
_temp335.last_plus_one - _temp335.curr, _temp335.curr, _temp336.last_plus_one -
_temp336.curr, _temp336.curr, _temp337.last_plus_one - _temp337.curr, _temp337.curr);}));
_temp286= 0;} d2= d0;} goto _LL299;} _LL299:;} if( ! _temp286){ return 0;} if(
_temp295 ==( void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({
struct Cyc_Absyn_Tuniondecl* _temp338=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp338[ 0]=* d2; _temp338;});( void*)(
d2->sc=( void*) _temp295); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}}} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl* d0,
struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ struct _tagged_arr _temp339= Cyc_Absynpp_qvar2string( d0->name);
goto _LL340; _LL340: { int _temp341= 1; goto _LL342; _LL342: { int _temp345;
void* _temp347; struct _tuple3 _temp343= Cyc_Tcdecl_merge_scope(( void*) d0->sc,(
void*) d1->sc, _tag_arr("enum", sizeof( unsigned char), 5u), _temp339, loc, msg);
_LL348: _temp347= _temp343.f1; goto _LL346; _LL346: _temp345= _temp343.f2; goto
_LL344; _LL344: if( ! _temp345){ _temp341= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{
struct _tuple6 _temp350=({ struct _tuple6 _temp349; _temp349.f1= d0->fields;
_temp349.f2= d1->fields; _temp349;}); struct Cyc_Core_Opt* _temp358; struct Cyc_Core_Opt*
_temp360; struct Cyc_Core_Opt* _temp362; struct Cyc_Core_Opt* _temp364; struct
Cyc_Core_Opt* _temp366; struct Cyc_Core_Opt _temp368; struct Cyc_List_List*
_temp369; struct Cyc_Core_Opt* _temp371; struct Cyc_Core_Opt _temp373; struct
Cyc_List_List* _temp374; _LL352: _LL361: _temp360= _temp350.f1; goto _LL359;
_LL359: _temp358= _temp350.f2; if( _temp358 == 0){ goto _LL353;} else{ goto
_LL354;} _LL354: _LL365: _temp364= _temp350.f1; if( _temp364 == 0){ goto _LL363;}
else{ goto _LL356;} _LL363: _temp362= _temp350.f2; goto _LL355; _LL356: _LL372:
_temp371= _temp350.f1; if( _temp371 == 0){ goto _LL351;} else{ _temp373=*
_temp371; _LL375: _temp374=( struct Cyc_List_List*) _temp373.v; goto _LL367;}
_LL367: _temp366= _temp350.f2; if( _temp366 == 0){ goto _LL351;} else{ _temp368=*
_temp366; _LL370: _temp369=( struct Cyc_List_List*) _temp368.v; goto _LL357;}
_LL353: d2= d0; goto _LL351; _LL355: d2= d1; goto _LL351; _LL357: for( 0;
_temp374 != 0? _temp369 != 0: 0;( _temp374=(( struct Cyc_List_List*) _check_null(
_temp374))->tl, _temp369=(( struct Cyc_List_List*) _check_null( _temp369))->tl)){
struct Cyc_Absyn_Enumfield _temp378; struct Cyc_Position_Segment* _temp379;
struct Cyc_Absyn_Exp* _temp381; struct _tuple0* _temp383; struct Cyc_Absyn_Enumfield*
_temp376=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp374))->hd; _temp378=* _temp376; _LL384: _temp383= _temp378.name; goto
_LL382; _LL382: _temp381= _temp378.tag; goto _LL380; _LL380: _temp379= _temp378.loc;
goto _LL377; _LL377: { struct Cyc_Absyn_Enumfield _temp387; struct Cyc_Position_Segment*
_temp388; struct Cyc_Absyn_Exp* _temp390; struct _tuple0* _temp392; struct Cyc_Absyn_Enumfield*
_temp385=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp369))->hd; _temp387=* _temp385; _LL393: _temp392= _temp387.name; goto
_LL391; _LL391: _temp390= _temp387.tag; goto _LL389; _LL389: _temp388= _temp387.loc;
goto _LL386; _LL386: if( Cyc_String_zstrptrcmp((* _temp392).f2,(* _temp383).f2)
!= 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr
_temp394= _temp339; struct _tagged_arr _temp395=*(* _temp392).f2; struct
_tagged_arr _temp396=*(* _temp383).f2; xprintf("enum %.*s: field name mismatch %.*s != %.*s",
_temp394.last_plus_one - _temp394.curr, _temp394.curr, _temp395.last_plus_one -
_temp395.curr, _temp395.curr, _temp396.last_plus_one - _temp396.curr, _temp396.curr);}));
_temp341= 0;} if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp381)) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp390))){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp397= _temp339; struct _tagged_arr _temp398=*(* _temp392).f2;
xprintf("enum %.*s, field %.*s, value mismatch", _temp397.last_plus_one -
_temp397.curr, _temp397.curr, _temp398.last_plus_one - _temp398.curr, _temp398.curr);}));
_temp341= 0;}}} d2= d0; goto _LL351; _LL351:;} if( ! _temp341){ return 0;} if((
void*) d2->sc == _temp347){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({
struct Cyc_Absyn_Enumdecl* _temp399=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp399[ 0]=* d2; _temp399;});( void*)( d2->sc=(
void*) _temp347); return( struct Cyc_Absyn_Enumdecl*) d2;}}}}} inline static
struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl( void* sc0, void* t0, struct Cyc_Absyn_Tqual
tq0, struct Cyc_List_List* atts0, void* sc1, void* t1, struct Cyc_Absyn_Tqual
tq1, struct Cyc_List_List* atts1, struct _tagged_arr t, struct _tagged_arr v,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ int _temp400= 1;
goto _LL401; _LL401: { int _temp404; void* _temp406; struct _tuple3 _temp402=
Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL407: _temp406= _temp402.f1;
goto _LL405; _LL405: _temp404= _temp402.f2; goto _LL403; _LL403: if( ! _temp404){
_temp400= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr _temp408= t; struct _tagged_arr
_temp409= v; struct _tagged_arr _temp410= Cyc_Absynpp_typ2string( t1); struct
_tagged_arr _temp411= Cyc_Absynpp_typ2string( t0); xprintf("%.*s %.*s has type %.*s instead of %.*s",
_temp408.last_plus_one - _temp408.curr, _temp408.curr, _temp409.last_plus_one -
_temp409.curr, _temp409.curr, _temp410.last_plus_one - _temp410.curr, _temp410.curr,
_temp411.last_plus_one - _temp411.curr, _temp411.curr);})); _temp400= 0;} if( !
Cyc_Tcutil_equal_tqual( tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp412= t; struct _tagged_arr _temp413= v;
xprintf("%.*s %.*s has different type qualifiers", _temp412.last_plus_one -
_temp412.curr, _temp412.curr, _temp413.last_plus_one - _temp413.curr, _temp413.curr);}));
_temp400= 0;} if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr( loc,
msg,( struct _tagged_arr)({ struct _tagged_arr _temp414= t; struct _tagged_arr
_temp415= v; xprintf("%.*s %.*s has different attributes", _temp414.last_plus_one
- _temp414.curr, _temp414.curr, _temp415.last_plus_one - _temp415.curr, _temp415.curr);}));
_temp400= 0;} return({ struct _tuple3 _temp416; _temp416.f1= _temp406; _temp416.f2=
_temp400; _temp416;});}} struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp417= Cyc_Absynpp_qvar2string(
d0->name); goto _LL418; _LL418: { int _temp421; void* _temp423; struct _tuple3
_temp419= Cyc_Tcdecl_check_var_or_fn_decl(( void*) d0->sc,( void*) d0->type, d0->tq,
d0->attributes,( void*) d1->sc,( void*) d1->type, d1->tq, d1->attributes,
_tag_arr("variable", sizeof( unsigned char), 9u), _temp417, loc, msg); _LL424:
_temp423= _temp419.f1; goto _LL422; _LL422: _temp421= _temp419.f2; goto _LL420;
_LL420: if( ! _temp421){ return 0;} if(( void*) d0->sc == _temp423){ return(
struct Cyc_Absyn_Vardecl*) d0;} else{ struct Cyc_Absyn_Vardecl* _temp426=({
struct Cyc_Absyn_Vardecl* _temp425=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp425[ 0]=* d0; _temp425;}); goto _LL427;
_LL427:( void*)((( struct Cyc_Absyn_Vardecl*) _check_null( _temp426))->sc=( void*)
_temp423); return _temp426;}}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp428= Cyc_Absynpp_qvar2string(
d0->name); goto _LL429; _LL429: if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs,
_tag_arr("typedef", sizeof( unsigned char), 8u), _temp428, loc, msg)){ return 0;}{
struct Cyc_List_List* _temp430= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs);
goto _LL431; _LL431: { void* subst_defn1= Cyc_Tcutil_substitute( _temp430,( void*)
d1->defn); if( ! Cyc_Tcdecl_check_type(( void*) d0->defn, subst_defn1)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp432= _temp428; struct
_tagged_arr _temp433= Cyc_Absynpp_typ2string( subst_defn1); struct _tagged_arr
_temp434= Cyc_Absynpp_typ2string(( void*) d0->defn); xprintf("typedef %.*s does not refer to the same type: %.*s != %.*s",
_temp432.last_plus_one - _temp432.curr, _temp432.curr, _temp433.last_plus_one -
_temp433.curr, _temp433.curr, _temp434.last_plus_one - _temp434.curr, _temp434.curr);}));
return 0;} return( struct Cyc_Absyn_Typedefdecl*) d0;}}} void* Cyc_Tcdecl_merge_binding(
void* b0, void* b1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tuple4 _temp436=({ struct _tuple4 _temp435; _temp435.f1= b0; _temp435.f2=
b1; _temp435;}); void* _temp450; void* _temp452; void* _temp454; struct Cyc_Absyn_Vardecl*
_temp456; void* _temp458; struct Cyc_Absyn_Vardecl* _temp460; void* _temp462;
struct Cyc_Absyn_Fndecl* _temp464; void* _temp466; struct Cyc_Absyn_Vardecl*
_temp468; void* _temp470; struct Cyc_Absyn_Fndecl* _temp472; void* _temp474;
struct Cyc_Absyn_Fndecl* _temp476; void* _temp478; struct Cyc_Absyn_Vardecl*
_temp480; void* _temp482; struct Cyc_Absyn_Fndecl* _temp484; _LL438: _LL453:
_temp452= _temp436.f1; if( _temp452 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL451;} else{ goto _LL440;} _LL451: _temp450= _temp436.f2; if( _temp450 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL439;} else{ goto _LL440;} _LL440: _LL459:
_temp458= _temp436.f1; if(( unsigned int) _temp458 > 1u?*(( int*) _temp458) ==
Cyc_Absyn_Global_b: 0){ _LL461: _temp460=(( struct Cyc_Absyn_Global_b_struct*)
_temp458)->f1; goto _LL455;} else{ goto _LL442;} _LL455: _temp454= _temp436.f2;
if(( unsigned int) _temp454 > 1u?*(( int*) _temp454) == Cyc_Absyn_Global_b: 0){
_LL457: _temp456=(( struct Cyc_Absyn_Global_b_struct*) _temp454)->f1; goto
_LL441;} else{ goto _LL442;} _LL442: _LL467: _temp466= _temp436.f1; if((
unsigned int) _temp466 > 1u?*(( int*) _temp466) == Cyc_Absyn_Global_b: 0){
_LL469: _temp468=(( struct Cyc_Absyn_Global_b_struct*) _temp466)->f1; goto
_LL463;} else{ goto _LL444;} _LL463: _temp462= _temp436.f2; if(( unsigned int)
_temp462 > 1u?*(( int*) _temp462) == Cyc_Absyn_Funname_b: 0){ _LL465: _temp464=((
struct Cyc_Absyn_Funname_b_struct*) _temp462)->f1; goto _LL443;} else{ goto
_LL444;} _LL444: _LL475: _temp474= _temp436.f1; if(( unsigned int) _temp474 > 1u?*((
int*) _temp474) == Cyc_Absyn_Funname_b: 0){ _LL477: _temp476=(( struct Cyc_Absyn_Funname_b_struct*)
_temp474)->f1; goto _LL471;} else{ goto _LL446;} _LL471: _temp470= _temp436.f2;
if(( unsigned int) _temp470 > 1u?*(( int*) _temp470) == Cyc_Absyn_Funname_b: 0){
_LL473: _temp472=(( struct Cyc_Absyn_Funname_b_struct*) _temp470)->f1; goto
_LL445;} else{ goto _LL446;} _LL446: _LL483: _temp482= _temp436.f1; if((
unsigned int) _temp482 > 1u?*(( int*) _temp482) == Cyc_Absyn_Funname_b: 0){
_LL485: _temp484=(( struct Cyc_Absyn_Funname_b_struct*) _temp482)->f1; goto
_LL479;} else{ goto _LL448;} _LL479: _temp478= _temp436.f2; if(( unsigned int)
_temp478 > 1u?*(( int*) _temp478) == Cyc_Absyn_Global_b: 0){ _LL481: _temp480=((
struct Cyc_Absyn_Global_b_struct*) _temp478)->f1; goto _LL447;} else{ goto
_LL448;} _LL448: goto _LL449; _LL439: return( void*) Cyc_Absyn_Unresolved_b;
_LL441: { struct Cyc_Absyn_Vardecl* _temp486= Cyc_Tcdecl_merge_vardecl( _temp460,
_temp456, loc, msg); goto _LL487; _LL487: if( _temp486 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp460 == _temp486){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp456 == _temp486){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp488=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp488[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp489; _temp489.tag= Cyc_Absyn_Global_b; _temp489.f1=(
struct Cyc_Absyn_Vardecl*) _check_null( _temp486); _temp489;}); _temp488;});}}}
_LL443: { int _temp492; void* _temp494; struct _tuple3 _temp490= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp468->sc,( void*) _temp468->type, _temp468->tq, _temp468->attributes,(
void*) _temp464->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp464->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp464->attributes, _tag_arr("function", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp468->name), loc, msg); _LL495:
_temp494= _temp490.f1; goto _LL493; _LL493: _temp492= _temp490.f2; goto _LL491;
_LL491: if( ! _temp492){ return( void*) Cyc_Absyn_Unresolved_b;} return b1;}
_LL445: Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr
_temp496= Cyc_Absynpp_qvar2string( _temp472->name); xprintf("redefinition of function %.*s",
_temp496.last_plus_one - _temp496.curr, _temp496.curr);})); return( void*) Cyc_Absyn_Unresolved_b;
_LL447: { int _temp499; void* _temp501; struct _tuple3 _temp497= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp484->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp484->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp484->attributes,( void*) _temp480->sc,( void*)
_temp480->type, _temp480->tq, _temp480->attributes, _tag_arr("variable", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp484->name), loc, msg); _LL502:
_temp501= _temp497.f1; goto _LL500; _LL500: _temp499= _temp497.f2; goto _LL498;
_LL498: if( ! _temp499){ return( void*) Cyc_Absyn_Unresolved_b;} return b0;}
_LL449:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp503=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp503[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp504; _temp504.tag= Cyc_Core_InvalidArg;
_temp504.f1= _tag_arr("Tcdecl::merge_binding", sizeof( unsigned char), 22u);
_temp504;}); _temp503;})); _LL437:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp507; struct Cyc_Absyn_Tunionfield* _temp508; struct Cyc_Absyn_Tuniondecl*
_temp510; struct Cyc_Tcdecl_Xtunionfielddecl* _temp505= d0; _temp507=* _temp505;
_LL511: _temp510= _temp507.base; goto _LL509; _LL509: _temp508= _temp507.field;
goto _LL506; _LL506: { struct Cyc_Tcdecl_Xtunionfielddecl _temp514; struct Cyc_Absyn_Tunionfield*
_temp515; struct Cyc_Absyn_Tuniondecl* _temp517; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp512= d1; _temp514=* _temp512; _LL518: _temp517= _temp514.base; goto _LL516;
_LL516: _temp515= _temp514.field; goto _LL513; _LL513: { struct _tagged_arr
_temp519= Cyc_Absynpp_qvar2string( _temp508->name); goto _LL520; _LL520: if( Cyc_Absyn_qvar_cmp(
_temp510->name, _temp517->name) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp521= Cyc_Absynpp_qvar2string( _temp510->name);
struct _tagged_arr _temp522= Cyc_Absynpp_qvar2string( _temp517->name); struct
_tagged_arr _temp523= _temp519; xprintf("xtunions %.*s and %.*s have both a field named %.*s",
_temp521.last_plus_one - _temp521.curr, _temp521.curr, _temp522.last_plus_one -
_temp522.curr, _temp522.curr, _temp523.last_plus_one - _temp523.curr, _temp523.curr);}));
return 0;} if( ! Cyc_Tcdecl_check_tvs( _temp510->tvs, _temp517->tvs, _tag_arr("xtunion",
sizeof( unsigned char), 8u), Cyc_Absynpp_qvar2string( _temp510->name), loc, msg)){
return 0;}{ struct Cyc_List_List* _temp524= Cyc_Tcdecl_build_tvs_map( _temp510->tvs,
_temp517->tvs); goto _LL525; _LL525: { struct Cyc_Absyn_Tunionfield* _temp526=
Cyc_Tcdecl_merge_tunionfield( _temp508, _temp515, _temp524, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), _temp519, msg); goto _LL527; _LL527: if( _temp526
== 0){ return 0;} if( _temp526 ==( struct Cyc_Absyn_Tunionfield*) _temp508){
return( struct Cyc_Tcdecl_Xtunionfielddecl*) d0;} else{ return({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp528=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp528->base= _temp510; _temp528->field=( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp526); _temp528;});}}}}}}