 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd(
struct _tuple0*); extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple1{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
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
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ( void*
t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct
Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
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
loc, void* eff); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern void
Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*); struct _tuple4{ struct Cyc_List_List* f1; struct
_RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*, struct _tagged_string, struct Cyc_Position_Segment*);
extern struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_string, struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
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
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_string msg){ Cyc_Tcutil_terr(
loc, msg); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp0=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp0->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp0;}));} static struct Cyc_Core_Opt Cyc_Tcexp_rk={( void*)(( void*) 3u)};
static struct Cyc_Core_Opt Cyc_Tcexp_ak={( void*)(( void*) 0u)}; static struct
Cyc_Core_Opt Cyc_Tcexp_bk={( void*)(( void*) 2u)}; static struct Cyc_Core_Opt
Cyc_Tcexp_mk={( void*)(( void*) 1u)}; static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct
_handler_cons _temp1; _push_handler(& _temp1);{ int _temp3= 0; if( setjmp(
_temp1.handler)){ _temp3= 1;} if( ! _temp3){{ void* _temp4= Cyc_Tcenv_lookup_ordinary(
te, e->loc, q); void* _temp14; struct Cyc_Absyn_Enumfield* _temp16; struct Cyc_Absyn_Enumdecl*
_temp18; struct Cyc_Absyn_Tunionfield* _temp20; struct Cyc_Absyn_Tuniondecl*
_temp22; struct Cyc_Absyn_Structdecl* _temp24; _LL6: if(*(( int*) _temp4) == Cyc_Tcenv_VarRes){
_LL15: _temp14=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp4)->f1; goto
_LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp4) == Cyc_Tcenv_EnumRes){ _LL19:
_temp18=(( struct Cyc_Tcenv_EnumRes_struct*) _temp4)->f1; goto _LL17; _LL17:
_temp16=(( struct Cyc_Tcenv_EnumRes_struct*) _temp4)->f2; goto _LL9;} else{ goto
_LL10;} _LL10: if(*(( int*) _temp4) == Cyc_Tcenv_TunionRes){ _LL23: _temp22=((
struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f1; goto _LL21; _LL21: _temp20=((
struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f2; goto _LL11;} else{ goto _LL12;}
_LL12: if(*(( int*) _temp4) == Cyc_Tcenv_StructRes){ _LL25: _temp24=(( struct
Cyc_Tcenv_StructRes_struct*) _temp4)->f1; goto _LL13;} else{ goto _LL5;} _LL7:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp26=( struct
Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp27; _temp27.tag= Cyc_Absyn_Var_e;
_temp27.f1= q; _temp27.f2=( void*) _temp14; _temp27;}); _temp26;}))); goto _LL5;
_LL9:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp28=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp28[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp29; _temp29.tag= Cyc_Absyn_Enum_e;
_temp29.f1= q; _temp29.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp29.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp29;}); _temp28;}))); goto _LL5; _LL11:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp30=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp31; _temp31.tag= Cyc_Absyn_Tunion_e;
_temp31.f1= 0; _temp31.f2= 0; _temp31.f3= 0; _temp31.f4= _temp22; _temp31.f5=
_temp20; _temp31;}); _temp30;}))); goto _LL5; _LL13: Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ struct _tagged_string _temp32= Cyc_Absynpp_qvar2string(
q); xprintf("bad occurrence of struct name %.*s", _temp32.last_plus_one -
_temp32.curr, _temp32.curr);}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp33=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp34; _temp34.tag= Cyc_Absyn_Var_e;
_temp34.f1= q; _temp34.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp34;});
_temp33;}))); goto _LL5; _LL5:;}; _pop_handler();} else{ void* _temp2=( void*)
_exn_thrown; void* _temp36= _temp2; _LL38: if( _temp36 == Cyc_Dict_Absent){ goto
_LL39;} else{ goto _LL40;} _LL40: goto _LL41; _LL39:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp42=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp43; _temp43.tag= Cyc_Absyn_Var_e; _temp43.f1= q; _temp43.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp43;}); _temp42;}))); goto _LL37; _LL41:( void)
_throw( _temp36); _LL37:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp44=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp44->f1= 0; _temp44->f2= e; _temp44;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp45=( void*)
e1->r; struct _tuple1* _temp51; _LL47: if(*(( int*) _temp45) == Cyc_Absyn_UnknownId_e){
_LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp45)->f1; goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct _handler_cons _temp53;
_push_handler(& _temp53);{ int _temp55= 0; if( setjmp( _temp53.handler)){
_temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68; struct Cyc_Absyn_Tuniondecl*
_temp70; struct Cyc_Absyn_Structdecl* _temp72; struct Cyc_Absyn_Enumfield*
_temp74; struct Cyc_Absyn_Enumdecl* _temp76; _LL58: if(*(( int*) _temp56) == Cyc_Tcenv_VarRes){
_LL67: _temp66=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto
_LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp56) == Cyc_Tcenv_TunionRes){
_LL71: _temp70=(( struct Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69;
_LL69: _temp68=(( struct Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;}
else{ goto _LL62;} _LL62: if(*(( int*) _temp56) == Cyc_Tcenv_StructRes){ _LL73:
_temp72=(( struct Cyc_Tcenv_StructRes_struct*) _temp56)->f1; goto _LL63;} else{
goto _LL64;} _LL64: if(*(( int*) _temp56) == Cyc_Tcenv_EnumRes){ _LL77: _temp76=((
struct Cyc_Tcenv_EnumRes_struct*) _temp56)->f1; goto _LL75; _LL75: _temp74=((
struct Cyc_Tcenv_EnumRes_struct*) _temp56)->f2; goto _LL65;} else{ goto _LL57;}
_LL59:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp78=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp78[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp79; _temp79.tag= Cyc_Absyn_FnCall_e;
_temp79.f1= e1; _temp79.f2= es; _temp79.f3= 0; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _LL61: if( _temp68->typs == 0){ Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ struct _tagged_string _temp80= Cyc_Absynpp_qvar2string(
_temp68->name); xprintf("%.*s is a constant, not a function", _temp80.last_plus_one
- _temp80.curr, _temp80.curr);}));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct*
_temp81=( struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp82; _temp82.tag= Cyc_Absyn_Tunion_e;
_temp82.f1= 0; _temp82.f2= 0; _temp82.f3= es; _temp82.f4= _temp70; _temp82.f5=
_temp68; _temp82;}); _temp81;}))); _npop_handler( 0u); return; _npop_handler( 0u);
return; _LL63: { struct Cyc_List_List* _temp83=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcexp_make_struct_arg, es); goto _LL84; _LL84: if( _temp72->name != 0){(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp85=(
struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp85[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp86; _temp86.tag= Cyc_Absyn_Struct_e;
_temp86.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp72->name))->v;
_temp86.f2= 0; _temp86.f3= _temp83; _temp86.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp86;}); _temp85;})));} else{ Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp87=( unsigned char*)"missing struct name"; struct
_tagged_string _temp88; _temp88.curr= _temp87; _temp88.base= _temp87; _temp88.last_plus_one=
_temp87 + 20; _temp88;}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp89=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp89[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp90; _temp90.tag= Cyc_Absyn_Struct_e;
_temp90.f1= _temp51; _temp90.f2= 0; _temp90.f3= _temp83; _temp90.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp90;}); _temp89;})));} _npop_handler( 0u); return;} _LL65: Cyc_Tcutil_terr(
e->loc,( struct _tagged_string)({ struct _tagged_string _temp91= Cyc_Absynpp_qvar2string(
_temp51); xprintf("%.*s is an enum constructor, not a function", _temp91.last_plus_one
- _temp91.curr, _temp91.curr);})); _npop_handler( 0u); return; _LL57:;};
_pop_handler();} else{ void* _temp54=( void*) _exn_thrown; void* _temp93=
_temp54; _LL95: if( _temp93 == Cyc_Dict_Absent){ goto _LL96;} else{ goto _LL97;}
_LL97: goto _LL98; _LL96: Cyc_Tcutil_terr( e1->loc,( struct _tagged_string)({
struct _tagged_string _temp99= Cyc_Absynpp_qvar2string( _temp51); xprintf("unbound identifier %.*s",
_temp99.last_plus_one - _temp99.curr, _temp99.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp100=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp100[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp101; _temp101.tag= Cyc_Absyn_FnCall_e; _temp101.f1=
e1; _temp101.f2= es; _temp101.f3= 0; _temp101;}); _temp100;}))); return; _LL98:(
void) _throw( _temp93); _LL94:;}}} _LL50:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp102=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp102[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp103; _temp103.tag= Cyc_Absyn_FnCall_e; _temp103.f1=
e1; _temp103.f2= es; _temp103.f3= 0; _temp103;}); _temp102;}))); return; _LL46:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp104=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp104[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp105; _temp105.tag= Cyc_Absyn_Array_e;
_temp105.f1= des; _temp105;}); _temp104;}))); return;}{ void* t=*(( void**)
_check_null( topt)); void* _temp106= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp116; struct Cyc_Absyn_Tqual _temp118; void* _temp120; struct Cyc_Absyn_Structdecl**
_temp122; struct Cyc_List_List* _temp124; struct _tuple1* _temp126; struct Cyc_List_List*
_temp128; _LL108: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_ArrayType:
0){ _LL121: _temp120=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp106)->f1;
goto _LL119; _LL119: _temp118=(( struct Cyc_Absyn_ArrayType_struct*) _temp106)->f2;
goto _LL117; _LL117: _temp116=(( struct Cyc_Absyn_ArrayType_struct*) _temp106)->f3;
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp106 > 4u?*((
int*) _temp106) == Cyc_Absyn_StructType: 0){ _LL127: _temp126=(( struct Cyc_Absyn_StructType_struct*)
_temp106)->f1; goto _LL125; _LL125: _temp124=(( struct Cyc_Absyn_StructType_struct*)
_temp106)->f2; goto _LL123; _LL123: _temp122=(( struct Cyc_Absyn_StructType_struct*)
_temp106)->f3; goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_AnonStructType: 0){ _LL129:
_temp128=(( struct Cyc_Absyn_AnonStructType_struct*) _temp106)->f1; goto _LL113;}
else{ goto _LL114;} _LL114: goto _LL115; _LL109:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp130=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp130[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp131; _temp131.tag= Cyc_Absyn_Array_e; _temp131.f1= des; _temp131;});
_temp130;}))); goto _LL107; _LL111: if( _temp122 == 0? 1: _temp126 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp132=( unsigned char*)"Compiler bug: struct type not properly set";
struct _tagged_string _temp133; _temp133.curr= _temp132; _temp133.base= _temp132;
_temp133.last_plus_one= _temp132 + 43; _temp133;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp134=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp134[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp135; _temp135.tag= Cyc_Absyn_Struct_e; _temp135.f1=(
struct _tuple1*) _check_null( _temp126); _temp135.f2= 0; _temp135.f3= des;
_temp135.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp122)); _temp135;}); _temp134;}))); goto _LL107; _LL113:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp136=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp136[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp137; _temp137.tag= Cyc_Absyn_AnonStruct_e;
_temp137.f1=( void*) t; _temp137.f2= des; _temp137;}); _temp136;}))); goto
_LL107; _LL115:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp138=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp138[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp139; _temp139.tag= Cyc_Absyn_Array_e;
_temp139.f1= des; _temp139;}); _temp138;}))); goto _LL107; _LL107:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ struct
_tagged_string _temp140= msg_part; struct _tagged_string _temp141= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp140.last_plus_one - _temp140.curr, _temp140.curr, _temp141.last_plus_one -
_temp141.curr, _temp141.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp142= c; unsigned char _temp158; void* _temp160; short
_temp162; void* _temp164; long long _temp166; void* _temp168; struct
_tagged_string _temp170; int _temp172; void* _temp174; struct _tagged_string
_temp176; _LL144: if(( unsigned int) _temp142 > 1u?*(( int*) _temp142) == Cyc_Absyn_Char_c:
0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp142)->f1;
goto _LL159; _LL159: _temp158=(( struct Cyc_Absyn_Char_c_struct*) _temp142)->f2;
goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int) _temp142 > 1u?*((
int*) _temp142) == Cyc_Absyn_Short_c: 0){ _LL165: _temp164=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp142)->f1; goto _LL163; _LL163: _temp162=(( struct Cyc_Absyn_Short_c_struct*)
_temp142)->f2; goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int)
_temp142 > 1u?*(( int*) _temp142) == Cyc_Absyn_LongLong_c: 0){ _LL169: _temp168=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp142)->f1; goto _LL167; _LL167:
_temp166=(( struct Cyc_Absyn_LongLong_c_struct*) _temp142)->f2; goto _LL149;}
else{ goto _LL150;} _LL150: if(( unsigned int) _temp142 > 1u?*(( int*) _temp142)
== Cyc_Absyn_Float_c: 0){ _LL171: _temp170=(( struct Cyc_Absyn_Float_c_struct*)
_temp142)->f1; goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp142 > 1u?*(( int*) _temp142) == Cyc_Absyn_Int_c: 0){ _LL175: _temp174=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp142)->f1; goto _LL173; _LL173:
_temp172=(( struct Cyc_Absyn_Int_c_struct*) _temp142)->f2; goto _LL153;} else{
goto _LL154;} _LL154: if(( unsigned int) _temp142 > 1u?*(( int*) _temp142) ==
Cyc_Absyn_String_c: 0){ _LL177: _temp176=(( struct Cyc_Absyn_String_c_struct*)
_temp142)->f1; goto _LL155;} else{ goto _LL156;} _LL156: if( _temp142 ==( void*)
Cyc_Absyn_Null_c){ goto _LL157;} else{ goto _LL143;} _LL145: t= _temp160 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL143;
_LL147: t= _temp164 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL143; _LL149: t= _temp168 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL143; _LL151: t= Cyc_Absyn_float_t; goto _LL143;
_LL153: if( topt == 0){ t= _temp174 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp178= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp188; void* _temp190; void* _temp192; void*
_temp194; void* _temp196; void* _temp198; _LL180: if(( unsigned int) _temp178 >
4u?*(( int*) _temp178) == Cyc_Absyn_IntType: 0){ _LL191: _temp190=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp178)->f1; goto _LL189; _LL189: _temp188=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp178)->f2; if( _temp188 ==( void*)
Cyc_Absyn_B1){ goto _LL181;} else{ goto _LL182;}} else{ goto _LL182;} _LL182:
if(( unsigned int) _temp178 > 4u?*(( int*) _temp178) == Cyc_Absyn_IntType: 0){
_LL195: _temp194=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp178)->f1;
goto _LL193; _LL193: _temp192=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp178)->f2; if( _temp192 ==( void*) Cyc_Absyn_B2){ goto _LL183;} else{ goto
_LL184;}} else{ goto _LL184;} _LL184: if(( unsigned int) _temp178 > 4u?*(( int*)
_temp178) == Cyc_Absyn_IntType: 0){ _LL199: _temp198=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp178)->f1; goto _LL197; _LL197: _temp196=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp178)->f2; goto _LL185;} else{ goto _LL186;} _LL186: goto _LL187; _LL181: t=
_temp190 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp200=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp200[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp201; _temp201.tag= Cyc_Absyn_Const_e;
_temp201.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp202=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp202[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp203; _temp203.tag= Cyc_Absyn_Char_c;
_temp203.f1=( void*) _temp190; _temp203.f2=( unsigned char) _temp172; _temp203;});
_temp202;})); _temp201;}); _temp200;}))); goto _LL179; _LL183: t= _temp194 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp204=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp204[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp205; _temp205.tag= Cyc_Absyn_Const_e; _temp205.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp206=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp206[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp207; _temp207.tag= Cyc_Absyn_Short_c; _temp207.f1=( void*) _temp194;
_temp207.f2=( short) _temp172; _temp207;}); _temp206;})); _temp205;}); _temp204;})));
goto _LL179; _LL185: t= _temp198 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL179; _LL187: t= _temp174 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL179; _LL179:;} goto _LL143; _LL155: {
int len=( int)({ struct _tagged_string _temp250= _temp176;( unsigned int)(
_temp250.last_plus_one - _temp250.curr);}); struct Cyc_Absyn_Const_e_struct*
_temp212=({ struct Cyc_Absyn_Const_e_struct* _temp208=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp208[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp209; _temp209.tag= Cyc_Absyn_Const_e; _temp209.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp210=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp210[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp211; _temp211.tag= Cyc_Absyn_Int_c; _temp211.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp211.f2= len; _temp211;}); _temp210;})); _temp209;}); _temp208;}); goto
_LL213; _LL213: { struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*)
_temp212, loc); elen->topt=({ struct Cyc_Core_Opt* _temp214=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp214->v=( void*) Cyc_Absyn_uint_t;
_temp214;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp215=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp215[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp216; _temp216.tag= Cyc_Absyn_Upper_b;
_temp216.f1= elen; _temp216;}); _temp215;})); if( topt != 0){ void* _temp217=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_Exp*
_temp225; struct Cyc_Absyn_Tqual _temp227; void* _temp229; struct Cyc_Absyn_PtrInfo
_temp231; struct Cyc_Absyn_Conref* _temp233; struct Cyc_Absyn_Tqual _temp235;
struct Cyc_Absyn_Conref* _temp237; void* _temp239; void* _temp241; _LL219: if((
unsigned int) _temp217 > 4u?*(( int*) _temp217) == Cyc_Absyn_ArrayType: 0){
_LL230: _temp229=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp217)->f1;
goto _LL228; _LL228: _temp227=(( struct Cyc_Absyn_ArrayType_struct*) _temp217)->f2;
goto _LL226; _LL226: _temp225=(( struct Cyc_Absyn_ArrayType_struct*) _temp217)->f3;
goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp217 > 4u?*((
int*) _temp217) == Cyc_Absyn_PointerType: 0){ _LL232: _temp231=(( struct Cyc_Absyn_PointerType_struct*)
_temp217)->f1; _LL242: _temp241=( void*) _temp231.elt_typ; goto _LL240; _LL240:
_temp239=( void*) _temp231.rgn_typ; goto _LL238; _LL238: _temp237= _temp231.nullable;
goto _LL236; _LL236: _temp235= _temp231.tq; goto _LL234; _LL234: _temp233=
_temp231.bounds; goto _LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL220:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp243=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp243[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp244; _temp244.tag= Cyc_Absyn_ArrayType; _temp244.f1=(
void*) Cyc_Absyn_uchar_t; _temp244.f2= Cyc_Absyn_const_tqual(); _temp244.f3=(
struct Cyc_Absyn_Exp*) elen; _temp244;}); _temp243;}); _LL222: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp245=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp245->v=( void*) t; _temp245;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp246=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp246->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp246;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp247=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp247[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp248; _temp248.tag= Cyc_Absyn_Upper_b; _temp248.f1= elen; _temp248;});
_temp247;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp249=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp249->v=( void*) t; _temp249;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL218; _LL224:
goto _LL218; _LL218:;} return t;}} _LL157: { struct Cyc_List_List* _temp251= Cyc_Tcenv_lookup_type_vars(
te); goto _LL252; _LL252: t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp253=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp253[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp254; _temp254.tag= Cyc_Absyn_PointerType;
_temp254.f1=({ struct Cyc_Absyn_PtrInfo _temp255; _temp255.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp257=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp257->v=( void*)
_temp251; _temp257;})); _temp255.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp256=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp256->v=( void*) _temp251;
_temp256;})); _temp255.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp255.tq= Cyc_Absyn_empty_tqual(); _temp255.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp255;}); _temp254;}); _temp253;}); goto _LL143;}
_LL143:;} return t;} static void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple1* q, void* b){ void* _temp258= b;
struct Cyc_Absyn_Vardecl* _temp272; struct Cyc_Absyn_Fndecl* _temp274; struct
Cyc_Absyn_Vardecl* _temp276; struct Cyc_Absyn_Vardecl* _temp278; struct Cyc_Absyn_Vardecl*
_temp280; _LL260: if( _temp258 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL261;}
else{ goto _LL262;} _LL262: if(( unsigned int) _temp258 > 1u?*(( int*) _temp258)
== Cyc_Absyn_Global_b: 0){ _LL273: _temp272=(( struct Cyc_Absyn_Global_b_struct*)
_temp258)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp258 > 1u?*(( int*) _temp258) == Cyc_Absyn_Funname_b: 0){ _LL275: _temp274=((
struct Cyc_Absyn_Funname_b_struct*) _temp258)->f1; goto _LL265;} else{ goto
_LL266;} _LL266: if(( unsigned int) _temp258 > 1u?*(( int*) _temp258) == Cyc_Absyn_Pat_b:
0){ _LL277: _temp276=(( struct Cyc_Absyn_Pat_b_struct*) _temp258)->f1; goto
_LL267;} else{ goto _LL268;} _LL268: if(( unsigned int) _temp258 > 1u?*(( int*)
_temp258) == Cyc_Absyn_Local_b: 0){ _LL279: _temp278=(( struct Cyc_Absyn_Local_b_struct*)
_temp258)->f1; goto _LL269;} else{ goto _LL270;} _LL270: if(( unsigned int)
_temp258 > 1u?*(( int*) _temp258) == Cyc_Absyn_Param_b: 0){ _LL281: _temp280=((
struct Cyc_Absyn_Param_b_struct*) _temp258)->f1; goto _LL271;} else{ goto _LL259;}
_LL261: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ struct
_tagged_string _temp282= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp282.last_plus_one - _temp282.curr, _temp282.curr);})); _LL263: q[
_check_known_subscript_notnull( 1u, 0)]=( _temp272->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp272->type; _LL265:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp274->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp274);
_LL267: _temp278= _temp276; goto _LL269; _LL269: _temp280= _temp278; goto _LL271;
_LL271:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp280->type; _LL259:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_string, struct Cyc_Position_Segment*)){ struct
Cyc_List_List* desc_types;{ void* _temp283=( void*) fmt->r; void* _temp289;
struct _tagged_string _temp291; _LL285: if(*(( int*) _temp283) == Cyc_Absyn_Const_e){
_LL290: _temp289=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp283)->f1; if((
unsigned int) _temp289 > 1u?*(( int*) _temp289) == Cyc_Absyn_String_c: 0){
_LL292: _temp291=(( struct Cyc_Absyn_String_c_struct*) _temp289)->f1; goto
_LL286;} else{ goto _LL287;}} else{ goto _LL287;} _LL287: goto _LL288; _LL286:
desc_types= type_getter( te, _temp291, fmt->loc); goto _LL284; _LL288: Cyc_Tcutil_terr(
fmt->loc,({ unsigned char* _temp293=( unsigned char*)"expecting a literal format string";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 34; _temp294;})); return; _LL284:;} for( 0;
desc_types != 0? args != 0: 0;( desc_types=(( struct Cyc_List_List*) _check_null(
desc_types))->tl, args=(( struct Cyc_List_List*) _check_null( args))->tl)){ void*
t=( void*)(( struct Cyc_List_List*) _check_null( desc_types))->hd; struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; if(
! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ struct _tagged_string _temp295= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp296= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_temp295.last_plus_one - _temp295.curr, _temp295.curr, _temp296.last_plus_one -
_temp296.curr, _temp296.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,({
unsigned char* _temp297=( unsigned char*)"too few arguments"; struct
_tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 18; _temp298;}));} if( args != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->loc,({
unsigned char* _temp299=( unsigned char*)"too many arguments"; struct
_tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 19; _temp300;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp301= p; _LL303: if( _temp301 ==( void*)
Cyc_Absyn_Plus){ goto _LL304;} else{ goto _LL305;} _LL305: if( _temp301 ==( void*)
Cyc_Absyn_Minus){ goto _LL306;} else{ goto _LL307;} _LL307: if( _temp301 ==(
void*) Cyc_Absyn_Not){ goto _LL308;} else{ goto _LL309;} _LL309: if( _temp301 ==(
void*) Cyc_Absyn_Bitnot){ goto _LL310;} else{ goto _LL311;} _LL311: if( _temp301
==( void*) Cyc_Absyn_Size){ goto _LL312;} else{ goto _LL313;} _LL313: goto
_LL314; _LL304: goto _LL306; _LL306: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp315= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp315.last_plus_one -
_temp315.curr, _temp315.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL308: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp316= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp316.last_plus_one - _temp316.curr, _temp316.curr);}));} return Cyc_Absyn_sint_t;
_LL310: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp317= Cyc_Absynpp_typ2string( t);
xprintf("expecting integral type but found %.*s", _temp317.last_plus_one -
_temp317.curr, _temp317.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL312:{ void* _temp318= t; struct Cyc_Absyn_Exp*
_temp326; struct Cyc_Absyn_Tqual _temp328; void* _temp330; struct Cyc_Absyn_PtrInfo
_temp332; struct Cyc_Absyn_Conref* _temp334; struct Cyc_Absyn_Tqual _temp336;
struct Cyc_Absyn_Conref* _temp338; void* _temp340; void* _temp342; _LL320: if((
unsigned int) _temp318 > 4u?*(( int*) _temp318) == Cyc_Absyn_ArrayType: 0){
_LL331: _temp330=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp318)->f1;
goto _LL329; _LL329: _temp328=(( struct Cyc_Absyn_ArrayType_struct*) _temp318)->f2;
goto _LL327; _LL327: _temp326=(( struct Cyc_Absyn_ArrayType_struct*) _temp318)->f3;
goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp318 > 4u?*((
int*) _temp318) == Cyc_Absyn_PointerType: 0){ _LL333: _temp332=(( struct Cyc_Absyn_PointerType_struct*)
_temp318)->f1; _LL343: _temp342=( void*) _temp332.elt_typ; goto _LL341; _LL341:
_temp340=( void*) _temp332.rgn_typ; goto _LL339; _LL339: _temp338= _temp332.nullable;
goto _LL337; _LL337: _temp336= _temp332.tq; goto _LL335; _LL335: _temp334=
_temp332.bounds; goto _LL323;} else{ goto _LL324;} _LL324: goto _LL325; _LL321:
goto _LL319; _LL323: goto _LL319; _LL325: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp344= Cyc_Absynpp_typ2string( t);
xprintf("size requires pointer or array type, not %.*s", _temp344.last_plus_one
- _temp344.curr, _temp344.curr);})); goto _LL319; _LL319:;} return Cyc_Absyn_uint_t;
_LL314: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp345=( unsigned char*)"Non-unary primop"; struct
_tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 17; _temp346;})); _LL302:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,( struct _tagged_string)({ struct _tagged_string _temp347= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("type %.*s cannot be used here",
_temp347.last_plus_one - _temp347.curr, _temp347.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp348=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp348->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp348;}));}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_string)({ struct
_tagged_string _temp349= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); xprintf("type %.*s cannot be used here", _temp349.last_plus_one
- _temp349.curr, _temp349.curr);})); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt*
_temp350=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp350->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp350;}));}{ void* t1=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp351= t1; struct Cyc_Absyn_PtrInfo _temp357; struct Cyc_Absyn_Conref*
_temp359; struct Cyc_Absyn_Tqual _temp361; struct Cyc_Absyn_Conref* _temp363;
void* _temp365; void* _temp367; _LL353: if(( unsigned int) _temp351 > 4u?*(( int*)
_temp351) == Cyc_Absyn_PointerType: 0){ _LL358: _temp357=(( struct Cyc_Absyn_PointerType_struct*)
_temp351)->f1; _LL368: _temp367=( void*) _temp357.elt_typ; goto _LL366; _LL366:
_temp365=( void*) _temp357.rgn_typ; goto _LL364; _LL364: _temp363= _temp357.nullable;
goto _LL362; _LL362: _temp361= _temp357.tq; goto _LL360; _LL360: _temp359=
_temp357.bounds; goto _LL354;} else{ goto _LL355;} _LL355: goto _LL356; _LL354:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct
_tagged_string)({ struct _tagged_string _temp369= Cyc_Absynpp_typ2string( t2);
xprintf("expecting int but found %.*s", _temp369.last_plus_one - _temp369.curr,
_temp369.curr);}));} _temp359=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp359);{ void* _temp370=( void*) _temp359->v;
void* _temp378; void* _temp380; struct Cyc_Absyn_Exp* _temp382; _LL372: if((
unsigned int) _temp370 > 1u?*(( int*) _temp370) == Cyc_Absyn_Eq_constr: 0){
_LL379: _temp378=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp370)->f1;
if( _temp378 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL373;} else{ goto _LL374;}}
else{ goto _LL374;} _LL374: if(( unsigned int) _temp370 > 1u?*(( int*) _temp370)
== Cyc_Absyn_Eq_constr: 0){ _LL381: _temp380=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp370)->f1; if(( unsigned int) _temp380 > 1u?*(( int*) _temp380) == Cyc_Absyn_Upper_b:
0){ _LL383: _temp382=(( struct Cyc_Absyn_Upper_b_struct*) _temp380)->f1; goto
_LL375;} else{ goto _LL376;}} else{ goto _LL376;} _LL376: goto _LL377; _LL373:
return t1; _LL375: { unsigned int _temp384= Cyc_Evexp_eval_const_uint_exp(
_temp382); goto _LL385; _LL385: _temp363=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp363);{ int is_nullable;{
void* _temp386=( void*) _temp363->v; int _temp392; _LL388: if(( unsigned int)
_temp386 > 1u?*(( int*) _temp386) == Cyc_Absyn_Eq_constr: 0){ _LL393: _temp392=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp386)->f1; goto _LL389;} else{
goto _LL390;} _LL390: goto _LL391; _LL389: if( _temp392){ struct Cyc_Absyn_PointerType_struct*
_temp397=({ struct Cyc_Absyn_PointerType_struct* _temp394=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp394[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp395; _temp395.tag= Cyc_Absyn_PointerType;
_temp395.f1=({ struct Cyc_Absyn_PtrInfo _temp396; _temp396.elt_typ=( void*)
_temp367; _temp396.rgn_typ=( void*) _temp365; _temp396.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp396.tq= _temp361; _temp396.bounds=
_temp359; _temp396;}); _temp395;}); _temp394;}); goto _LL398; _LL398: Cyc_Tcutil_warn(
e1->loc,( struct _tagged_string)({ struct _tagged_string _temp399= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp400= Cyc_Absynpp_typ2string(( void*) _temp397);
xprintf("coercing from %.*s to %.*s", _temp399.last_plus_one - _temp399.curr,
_temp399.curr, _temp400.last_plus_one - _temp400.curr, _temp400.curr);})); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp397);} goto _LL387; _LL391:( void*)( _temp363->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp401=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp401[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp402; _temp402.tag= Cyc_Absyn_Eq_constr; _temp402.f1=(
void*) 0; _temp402;}); _temp401;}))); goto _LL387; _LL387:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int _temp403= Cyc_Evexp_eval_const_uint_exp( e2); goto _LL404;
_LL404: if( _temp384 < _temp403){ _temp403= _temp384; Cyc_Tcutil_warn( e1->loc,(
struct _tagged_string) xprintf("pointer arithmetic is out of bounds"));}{ struct
Cyc_Absyn_Exp* _temp405= Cyc_Absyn_uint_exp( _temp384 - _temp403, 0); goto
_LL406; _LL406: _temp405->topt=({ struct Cyc_Core_Opt* _temp407=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp407->v=( void*) Cyc_Absyn_uint_t;
_temp407;}); return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp408=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp408[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp409; _temp409.tag= Cyc_Absyn_PointerType;
_temp409.f1=({ struct Cyc_Absyn_PtrInfo _temp410; _temp410.elt_typ=( void*)
_temp367; _temp410.rgn_typ=( void*) _temp365; _temp410.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp410.tq= _temp361; _temp410.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp411=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp411[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp412; _temp412.tag= Cyc_Absyn_Upper_b; _temp412.f1= _temp405; _temp412;});
_temp411;})); _temp410;}); _temp409;}); _temp408;});}} else{ Cyc_Tcutil_terr( e1->loc,({
unsigned char* _temp413=( unsigned char*)"cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 98; _temp414;})); return t1;}}} _LL377:( void*)(
_temp359->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp415=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp415[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp416; _temp416.tag= Cyc_Absyn_Eq_constr;
_temp416.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp416;}); _temp415;})));
return t1; _LL371:;} _LL356: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL352:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_string)({ struct _tagged_string _temp417= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_string
_temp418= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
e2->topt))->v); xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp417.last_plus_one - _temp417.curr, _temp417.curr, _temp418.last_plus_one -
_temp418.curr, _temp418.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_string)({ struct
_tagged_string _temp419= Cyc_Absynpp_typ2string( t1); struct _tagged_string
_temp420= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp419.last_plus_one - _temp419.curr, _temp419.curr, _temp420.last_plus_one -
_temp420.curr, _temp420.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp421=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp421->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp421;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp423=({ struct _tuple0 _temp422;
_temp422.f1= Cyc_Tcutil_compress( t1); _temp422.f2= Cyc_Tcutil_compress( t2);
_temp422;}); void* _temp429; struct Cyc_Absyn_PtrInfo _temp431; struct Cyc_Absyn_Conref*
_temp433; struct Cyc_Absyn_Tqual _temp435; struct Cyc_Absyn_Conref* _temp437;
void* _temp439; void* _temp441; void* _temp443; struct Cyc_Absyn_PtrInfo
_temp445; struct Cyc_Absyn_Conref* _temp447; struct Cyc_Absyn_Tqual _temp449;
struct Cyc_Absyn_Conref* _temp451; void* _temp453; void* _temp455; _LL425:
_LL444: _temp443= _temp423.f1; if(( unsigned int) _temp443 > 4u?*(( int*)
_temp443) == Cyc_Absyn_PointerType: 0){ _LL446: _temp445=(( struct Cyc_Absyn_PointerType_struct*)
_temp443)->f1; _LL456: _temp455=( void*) _temp445.elt_typ; goto _LL454; _LL454:
_temp453=( void*) _temp445.rgn_typ; goto _LL452; _LL452: _temp451= _temp445.nullable;
goto _LL450; _LL450: _temp449= _temp445.tq; goto _LL448; _LL448: _temp447=
_temp445.bounds; goto _LL430;} else{ goto _LL427;} _LL430: _temp429= _temp423.f2;
if(( unsigned int) _temp429 > 4u?*(( int*) _temp429) == Cyc_Absyn_PointerType: 0){
_LL432: _temp431=(( struct Cyc_Absyn_PointerType_struct*) _temp429)->f1; _LL442:
_temp441=( void*) _temp431.elt_typ; goto _LL440; _LL440: _temp439=( void*)
_temp431.rgn_typ; goto _LL438; _LL438: _temp437= _temp431.nullable; goto _LL436;
_LL436: _temp435= _temp431.tq; goto _LL434; _LL434: _temp433= _temp431.bounds;
goto _LL426;} else{ goto _LL427;} _LL427: goto _LL428; _LL426: if( Cyc_Tcutil_unify(
_temp455, _temp441)){ return Cyc_Absyn_sint_t;} goto _LL424; _LL428: goto _LL424;
_LL424:;} Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp457= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp458= Cyc_Absynpp_typ2string(
t2); xprintf("comparison not allowed between %.*s and %.*s", _temp457.last_plus_one
- _temp457.curr, _temp457.curr, _temp458.last_plus_one - _temp458.curr, _temp458.curr);}));
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp459=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp459->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp459;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp460= p; _LL462: if( _temp460 ==( void*)
Cyc_Absyn_Plus){ goto _LL463;} else{ goto _LL464;} _LL464: if( _temp460 ==( void*)
Cyc_Absyn_Minus){ goto _LL465;} else{ goto _LL466;} _LL466: if( _temp460 ==(
void*) Cyc_Absyn_Times){ goto _LL467;} else{ goto _LL468;} _LL468: if( _temp460
==( void*) Cyc_Absyn_Div){ goto _LL469;} else{ goto _LL470;} _LL470: if(
_temp460 ==( void*) Cyc_Absyn_Mod){ goto _LL471;} else{ goto _LL472;} _LL472:
if( _temp460 ==( void*) Cyc_Absyn_Bitand){ goto _LL473;} else{ goto _LL474;}
_LL474: if( _temp460 ==( void*) Cyc_Absyn_Bitor){ goto _LL475;} else{ goto
_LL476;} _LL476: if( _temp460 ==( void*) Cyc_Absyn_Bitxor){ goto _LL477;} else{
goto _LL478;} _LL478: if( _temp460 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL479;}
else{ goto _LL480;} _LL480: if( _temp460 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL481;} else{ goto _LL482;} _LL482: if( _temp460 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL483;} else{ goto _LL484;} _LL484: if( _temp460 ==( void*) Cyc_Absyn_Eq){
goto _LL485;} else{ goto _LL486;} _LL486: if( _temp460 ==( void*) Cyc_Absyn_Neq){
goto _LL487;} else{ goto _LL488;} _LL488: if( _temp460 ==( void*) Cyc_Absyn_Gt){
goto _LL489;} else{ goto _LL490;} _LL490: if( _temp460 ==( void*) Cyc_Absyn_Lt){
goto _LL491;} else{ goto _LL492;} _LL492: if( _temp460 ==( void*) Cyc_Absyn_Gte){
goto _LL493;} else{ goto _LL494;} _LL494: if( _temp460 ==( void*) Cyc_Absyn_Lte){
goto _LL495;} else{ goto _LL496;} _LL496: goto _LL497; _LL463: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL465: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL467: goto _LL469;
_LL469: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL471: goto _LL473; _LL473: goto _LL475; _LL475: goto _LL477; _LL477: goto
_LL479; _LL479: goto _LL481; _LL481: goto _LL483; _LL483: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL485: goto _LL487; _LL487: goto _LL489;
_LL489: goto _LL491; _LL491: goto _LL493; _LL493: goto _LL495; _LL495: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL497:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp498=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp498[ 0]=({ struct Cyc_Core_Failure_struct
_temp499; _temp499.tag= Cyc_Core_Failure; _temp499.f1=({ unsigned char* _temp500=(
unsigned char*)"bad binary primop"; struct _tagged_string _temp501; _temp501.curr=
_temp500; _temp501.base= _temp500; _temp501.last_plus_one= _temp500 + 18;
_temp501;}); _temp499;}); _temp498;})); _LL461:;} static void* Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp( te,
topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp502= p; _LL504: if( _temp502
==( void*) Cyc_Absyn_Printf){ goto _LL505;} else{ goto _LL506;} _LL506: if(
_temp502 ==( void*) Cyc_Absyn_Fprintf){ goto _LL507;} else{ goto _LL508;} _LL508:
if( _temp502 ==( void*) Cyc_Absyn_Xprintf){ goto _LL509;} else{ goto _LL510;}
_LL510: if( _temp502 ==( void*) Cyc_Absyn_Scanf){ goto _LL511;} else{ goto
_LL512;} _LL512: if( _temp502 ==( void*) Cyc_Absyn_Fscanf){ goto _LL513;} else{
goto _LL514;} _LL514: if( _temp502 ==( void*) Cyc_Absyn_Sscanf){ goto _LL515;}
else{ goto _LL516;} _LL516: goto _LL517; _LL505: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ unsigned char* _temp518=( unsigned char*)"missing format string in printf";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 32; _temp519;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL503; _LL507: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char*
_temp520=( unsigned char*)"too few arguments to fprintf"; struct _tagged_string
_temp521; _temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 29; _temp521;}));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_string)({ struct _tagged_string
_temp522= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _temp522.last_plus_one - _temp522.curr,
_temp522.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType; goto _LL503;
_LL509: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char*
_temp523=( unsigned char*)"missing format string in xprintf"; struct
_tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 33; _temp524;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp525=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp525->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp525;})));
goto _LL503; _LL511: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp526=( unsigned char*)"missing format string in scanf";
struct _tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 31; _temp527;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t=
Cyc_Absyn_sint_t; goto _LL503; _LL513: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char*
_temp528=( unsigned char*)"too few arguments to fscanf"; struct _tagged_string
_temp529; _temp529.curr= _temp528; _temp529.base= _temp528; _temp529.last_plus_one=
_temp528 + 28; _temp529;}));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_string)({ struct _tagged_string
_temp530= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _temp530.last_plus_one - _temp530.curr,
_temp530.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL503; _LL515: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp531=( unsigned char*)"sscanf is not supported";
struct _tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 24; _temp532;})); t= Cyc_Absyn_sint_t; goto
_LL503; _LL517: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL533: return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp535=(
unsigned char*)"primitive operator has 0 arguments"; struct _tagged_string
_temp536; _temp536.curr= _temp535; _temp536.base= _temp535; _temp536.last_plus_one=
_temp535 + 35; _temp536;})); case 1: _LL534: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL537: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL538: return Cyc_Tcexp_expr_err( te, loc,({ unsigned char*
_temp540=( unsigned char*)"primitive operator has > 2 arguments"; struct
_tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 37; _temp541;}));} goto _LL503; _LL503:;}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp542=( void*) e->r; void* _temp566; struct Cyc_Absyn_Vardecl* _temp568;
struct _tuple1* _temp570; void* _temp572; struct Cyc_Absyn_Vardecl* _temp574;
struct _tuple1* _temp576; void* _temp578; struct Cyc_Absyn_Vardecl* _temp580;
struct _tuple1* _temp582; void* _temp584; struct Cyc_Absyn_Vardecl* _temp586;
struct _tuple1* _temp588; struct Cyc_Absyn_Exp* _temp590; struct Cyc_Absyn_Exp*
_temp592; struct _tagged_string* _temp594; struct Cyc_Absyn_Exp* _temp596;
struct _tagged_string* _temp598; struct Cyc_Absyn_Exp* _temp600; struct Cyc_Absyn_Exp*
_temp602; struct Cyc_Absyn_Exp* _temp604; struct Cyc_List_List* _temp606; struct
Cyc_Absyn_Exp* _temp608; _LL544: if(*(( int*) _temp542) == Cyc_Absyn_Var_e){
_LL571: _temp570=(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f1; goto _LL567;
_LL567: _temp566=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f2; if((
unsigned int) _temp566 > 1u?*(( int*) _temp566) == Cyc_Absyn_Param_b: 0){ _LL569:
_temp568=(( struct Cyc_Absyn_Param_b_struct*) _temp566)->f1; goto _LL545;} else{
goto _LL546;}} else{ goto _LL546;} _LL546: if(*(( int*) _temp542) == Cyc_Absyn_Var_e){
_LL577: _temp576=(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f1; goto _LL573;
_LL573: _temp572=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f2; if((
unsigned int) _temp572 > 1u?*(( int*) _temp572) == Cyc_Absyn_Local_b: 0){ _LL575:
_temp574=(( struct Cyc_Absyn_Local_b_struct*) _temp572)->f1; goto _LL547;} else{
goto _LL548;}} else{ goto _LL548;} _LL548: if(*(( int*) _temp542) == Cyc_Absyn_Var_e){
_LL583: _temp582=(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f1; goto _LL579;
_LL579: _temp578=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f2; if((
unsigned int) _temp578 > 1u?*(( int*) _temp578) == Cyc_Absyn_Pat_b: 0){ _LL581:
_temp580=(( struct Cyc_Absyn_Pat_b_struct*) _temp578)->f1; goto _LL549;} else{
goto _LL550;}} else{ goto _LL550;} _LL550: if(*(( int*) _temp542) == Cyc_Absyn_Var_e){
_LL589: _temp588=(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f1; goto _LL585;
_LL585: _temp584=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp542)->f2; if((
unsigned int) _temp584 > 1u?*(( int*) _temp584) == Cyc_Absyn_Global_b: 0){
_LL587: _temp586=(( struct Cyc_Absyn_Global_b_struct*) _temp584)->f1; goto
_LL551;} else{ goto _LL552;}} else{ goto _LL552;} _LL552: if(*(( int*) _temp542)
== Cyc_Absyn_Subscript_e){ _LL593: _temp592=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp542)->f1; goto _LL591; _LL591: _temp590=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp542)->f2; goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp542)
== Cyc_Absyn_StructMember_e){ _LL597: _temp596=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp542)->f1; goto _LL595; _LL595: _temp594=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp542)->f2; goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp542)
== Cyc_Absyn_StructArrow_e){ _LL601: _temp600=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp542)->f1; goto _LL599; _LL599: _temp598=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp542)->f2; goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp542)
== Cyc_Absyn_Deref_e){ _LL603: _temp602=(( struct Cyc_Absyn_Deref_e_struct*)
_temp542)->f1; goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp542)
== Cyc_Absyn_NoInstantiate_e){ _LL605: _temp604=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp542)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp542)
== Cyc_Absyn_Instantiate_e){ _LL609: _temp608=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp542)->f1; goto _LL607; _LL607: _temp606=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp542)->f2; goto _LL563;} else{ goto _LL564;} _LL564: goto _LL565; _LL545:
_temp574= _temp568; goto _LL547; _LL547: _temp580= _temp574; goto _LL549; _LL549:
_temp586= _temp580; goto _LL551; _LL551: if( !( _temp586->tq).q_const){ return;}
goto _LL543; _LL553:{ void* _temp610= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp592->topt))->v); struct Cyc_Absyn_PtrInfo _temp620; struct Cyc_Absyn_Conref*
_temp622; struct Cyc_Absyn_Tqual _temp624; struct Cyc_Absyn_Conref* _temp626;
void* _temp628; void* _temp630; struct Cyc_Absyn_Exp* _temp632; struct Cyc_Absyn_Tqual
_temp634; void* _temp636; struct Cyc_List_List* _temp638; _LL612: if((
unsigned int) _temp610 > 4u?*(( int*) _temp610) == Cyc_Absyn_PointerType: 0){
_LL621: _temp620=(( struct Cyc_Absyn_PointerType_struct*) _temp610)->f1; _LL631:
_temp630=( void*) _temp620.elt_typ; goto _LL629; _LL629: _temp628=( void*)
_temp620.rgn_typ; goto _LL627; _LL627: _temp626= _temp620.nullable; goto _LL625;
_LL625: _temp624= _temp620.tq; goto _LL623; _LL623: _temp622= _temp620.bounds;
goto _LL613;} else{ goto _LL614;} _LL614: if(( unsigned int) _temp610 > 4u?*((
int*) _temp610) == Cyc_Absyn_ArrayType: 0){ _LL637: _temp636=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp610)->f1; goto _LL635; _LL635: _temp634=((
struct Cyc_Absyn_ArrayType_struct*) _temp610)->f2; goto _LL633; _LL633: _temp632=((
struct Cyc_Absyn_ArrayType_struct*) _temp610)->f3; goto _LL615;} else{ goto
_LL616;} _LL616: if(( unsigned int) _temp610 > 4u?*(( int*) _temp610) == Cyc_Absyn_TupleType:
0){ _LL639: _temp638=(( struct Cyc_Absyn_TupleType_struct*) _temp610)->f1; goto
_LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL613: _temp634= _temp624;
goto _LL615; _LL615: if( ! _temp634.q_const){ return;} goto _LL611; _LL617: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp590);{ struct _handler_cons
_temp640; _push_handler(& _temp640);{ int _temp642= 0; if( setjmp( _temp640.handler)){
_temp642= 1;} if( ! _temp642){{ struct _tuple8 _temp645; void* _temp646; struct
Cyc_Absyn_Tqual _temp648; struct _tuple8* _temp643=(( struct _tuple8*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp638,( int) i); _temp645=* _temp643;
_LL649: _temp648= _temp645.f1; goto _LL647; _LL647: _temp646= _temp645.f2; goto
_LL644; _LL644: if( ! _temp648.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp641=( void*) _exn_thrown; void* _temp651=
_temp641; _LL653: if( _temp651 == Cyc_List_Nth){ goto _LL654;} else{ goto _LL655;}
_LL655: goto _LL656; _LL654: return; _LL656:( void) _throw( _temp651); _LL652:;}}}
goto _LL611;} _LL619: goto _LL611; _LL611:;} goto _LL543; _LL555:{ void*
_temp657= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp596->topt))->v); struct Cyc_Absyn_Structdecl** _temp669; struct Cyc_List_List*
_temp671; struct _tuple1* _temp673; struct Cyc_List_List* _temp675; struct Cyc_List_List*
_temp677; struct Cyc_Absyn_Uniondecl** _temp679; struct Cyc_List_List* _temp681;
struct _tuple1* _temp683; _LL659: if(( unsigned int) _temp657 > 4u?*(( int*)
_temp657) == Cyc_Absyn_StructType: 0){ _LL674: _temp673=(( struct Cyc_Absyn_StructType_struct*)
_temp657)->f1; goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_StructType_struct*)
_temp657)->f2; goto _LL670; _LL670: _temp669=(( struct Cyc_Absyn_StructType_struct*)
_temp657)->f3; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp657 > 4u?*(( int*) _temp657) == Cyc_Absyn_AnonUnionType: 0){ _LL676:
_temp675=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp657)->f1; goto _LL662;}
else{ goto _LL663;} _LL663: if(( unsigned int) _temp657 > 4u?*(( int*) _temp657)
== Cyc_Absyn_AnonStructType: 0){ _LL678: _temp677=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp657)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp657 > 4u?*(( int*) _temp657) == Cyc_Absyn_UnionType: 0){ _LL684: _temp683=((
struct Cyc_Absyn_UnionType_struct*) _temp657)->f1; goto _LL682; _LL682: _temp681=((
struct Cyc_Absyn_UnionType_struct*) _temp657)->f2; goto _LL680; _LL680: _temp679=((
struct Cyc_Absyn_UnionType_struct*) _temp657)->f3; goto _LL666;} else{ goto
_LL667;} _LL667: goto _LL668; _LL660: { struct Cyc_Absyn_Structfield* sf=
_temp669 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp669)), _temp594); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL658;} _LL662: _temp677= _temp675; goto _LL664; _LL664: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp677, _temp594); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL658;} _LL666: { struct Cyc_Absyn_Structfield*
sf= _temp679 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp679)), _temp594); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL658;} _LL668: goto _LL658; _LL658:;} goto _LL543; _LL557:{ void*
_temp685= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp600->topt))->v); struct Cyc_Absyn_PtrInfo _temp691; struct Cyc_Absyn_Conref*
_temp693; struct Cyc_Absyn_Tqual _temp695; struct Cyc_Absyn_Conref* _temp697;
void* _temp699; void* _temp701; _LL687: if(( unsigned int) _temp685 > 4u?*(( int*)
_temp685) == Cyc_Absyn_PointerType: 0){ _LL692: _temp691=(( struct Cyc_Absyn_PointerType_struct*)
_temp685)->f1; _LL702: _temp701=( void*) _temp691.elt_typ; goto _LL700; _LL700:
_temp699=( void*) _temp691.rgn_typ; goto _LL698; _LL698: _temp697= _temp691.nullable;
goto _LL696; _LL696: _temp695= _temp691.tq; goto _LL694; _LL694: _temp693=
_temp691.bounds; goto _LL688;} else{ goto _LL689;} _LL689: goto _LL690; _LL688:{
void* _temp703= Cyc_Tcutil_compress( _temp701); struct Cyc_Absyn_Structdecl**
_temp715; struct Cyc_List_List* _temp717; struct _tuple1* _temp719; struct Cyc_Absyn_Uniondecl**
_temp721; struct Cyc_List_List* _temp723; struct _tuple1* _temp725; struct Cyc_List_List*
_temp727; struct Cyc_List_List* _temp729; _LL705: if(( unsigned int) _temp703 >
4u?*(( int*) _temp703) == Cyc_Absyn_StructType: 0){ _LL720: _temp719=(( struct
Cyc_Absyn_StructType_struct*) _temp703)->f1; goto _LL718; _LL718: _temp717=((
struct Cyc_Absyn_StructType_struct*) _temp703)->f2; goto _LL716; _LL716:
_temp715=(( struct Cyc_Absyn_StructType_struct*) _temp703)->f3; goto _LL706;}
else{ goto _LL707;} _LL707: if(( unsigned int) _temp703 > 4u?*(( int*) _temp703)
== Cyc_Absyn_UnionType: 0){ _LL726: _temp725=(( struct Cyc_Absyn_UnionType_struct*)
_temp703)->f1; goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_UnionType_struct*)
_temp703)->f2; goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_UnionType_struct*)
_temp703)->f3; goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int)
_temp703 > 4u?*(( int*) _temp703) == Cyc_Absyn_AnonStructType: 0){ _LL728:
_temp727=(( struct Cyc_Absyn_AnonStructType_struct*) _temp703)->f1; goto _LL710;}
else{ goto _LL711;} _LL711: if(( unsigned int) _temp703 > 4u?*(( int*) _temp703)
== Cyc_Absyn_AnonUnionType: 0){ _LL730: _temp729=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp703)->f1; goto _LL712;} else{ goto _LL713;} _LL713: goto _LL714; _LL706: {
struct Cyc_Absyn_Structfield* sf= _temp715 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp715)), _temp598); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL704;} _LL708: { struct Cyc_Absyn_Structfield*
sf= _temp721 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp721)), _temp598); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL704;} _LL710: _temp729= _temp727; goto _LL712; _LL712: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp729, _temp598); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL704;} _LL714: goto _LL704;
_LL704:;} goto _LL686; _LL690: goto _LL686; _LL686:;} goto _LL543; _LL559:{ void*
_temp731= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp602->topt))->v); struct Cyc_Absyn_PtrInfo _temp739; struct Cyc_Absyn_Conref*
_temp741; struct Cyc_Absyn_Tqual _temp743; struct Cyc_Absyn_Conref* _temp745;
void* _temp747; void* _temp749; struct Cyc_Absyn_Exp* _temp751; struct Cyc_Absyn_Tqual
_temp753; void* _temp755; _LL733: if(( unsigned int) _temp731 > 4u?*(( int*)
_temp731) == Cyc_Absyn_PointerType: 0){ _LL740: _temp739=(( struct Cyc_Absyn_PointerType_struct*)
_temp731)->f1; _LL750: _temp749=( void*) _temp739.elt_typ; goto _LL748; _LL748:
_temp747=( void*) _temp739.rgn_typ; goto _LL746; _LL746: _temp745= _temp739.nullable;
goto _LL744; _LL744: _temp743= _temp739.tq; goto _LL742; _LL742: _temp741=
_temp739.bounds; goto _LL734;} else{ goto _LL735;} _LL735: if(( unsigned int)
_temp731 > 4u?*(( int*) _temp731) == Cyc_Absyn_ArrayType: 0){ _LL756: _temp755=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp731)->f1; goto _LL754; _LL754:
_temp753=(( struct Cyc_Absyn_ArrayType_struct*) _temp731)->f2; goto _LL752;
_LL752: _temp751=(( struct Cyc_Absyn_ArrayType_struct*) _temp731)->f3; goto
_LL736;} else{ goto _LL737;} _LL737: goto _LL738; _LL734: _temp753= _temp743;
goto _LL736; _LL736: if( ! _temp753.q_const){ return;} goto _LL732; _LL738: goto
_LL732; _LL732:;} goto _LL543; _LL561: _temp608= _temp604; goto _LL563; _LL563:
Cyc_Tcexp_check_writable( te, _temp608); return; _LL565: goto _LL543; _LL543:;}
Cyc_Tcutil_terr( e->loc,({ unsigned char* _temp757=( unsigned char*)"attempt to write a read-only location";
struct _tagged_string _temp758; _temp758.curr= _temp757; _temp758.base= _temp757;
_temp758.last_plus_one= _temp757 + 38; _temp758;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp759=(
unsigned char*)"increment/decrement of non-lvalue"; struct _tagged_string
_temp760; _temp760.curr= _temp759; _temp760.base= _temp759; _temp760.last_plus_one=
_temp759 + 34; _temp760;}));} Cyc_Tcexp_check_writable( te, e);{ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric( e)?
! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ struct _tagged_string _temp761= Cyc_Absynpp_typ2string( t);
xprintf("expecting arithmetic or ? type but found %.*s", _temp761.last_plus_one
- _temp761.curr, _temp761.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,({ unsigned char* _temp762=( unsigned char*)"conditional expression";
struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 23; _temp763;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp772=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp772->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp772;})); struct Cyc_List_List* _temp765=({ struct Cyc_List_List*
_temp764=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp764->hd=( void*) e3; _temp764->tl= 0; _temp764;}); goto _LL766; _LL766: {
struct Cyc_List_List* _temp768=({ struct Cyc_List_List* _temp767=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp767->hd=( void*) e2; _temp767->tl=
_temp765; _temp767;}); goto _LL769; _LL769: if( ! Cyc_Tcutil_coerce_list( te, t,
_temp768)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp770= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); struct _tagged_string _temp771= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e3->topt))->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp770.last_plus_one - _temp770.curr, _temp770.curr, _temp771.last_plus_one -
_temp771.curr, _temp771.curr);}));} return t;}}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp773= Cyc_Tcutil_compress( t1); struct
Cyc_Absyn_Exp* _temp779; struct Cyc_Absyn_Tqual _temp781; void* _temp783; _LL775:
if(( unsigned int) _temp773 > 4u?*(( int*) _temp773) == Cyc_Absyn_ArrayType: 0){
_LL784: _temp783=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp773)->f1;
goto _LL782; _LL782: _temp781=(( struct Cyc_Absyn_ArrayType_struct*) _temp773)->f2;
goto _LL780; _LL780: _temp779=(( struct Cyc_Absyn_ArrayType_struct*) _temp773)->f3;
goto _LL776;} else{ goto _LL777;} _LL777: goto _LL778; _LL776: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp785=( unsigned char*)"cannot assign to an array";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 26; _temp786;})); goto _LL774; _LL778: goto
_LL774; _LL774:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,({ unsigned char* _temp787=( unsigned char*)"assignment to non-lvalue";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 25; _temp788;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ struct _tagged_string _temp789= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp790= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp789.last_plus_one - _temp789.curr, _temp789.curr, _temp790.last_plus_one -
_temp790.curr, _temp790.curr);}));}} else{ void* _temp791=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; goto _LL792; _LL792: { void* _temp793= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp791, e1, e2); goto _LL794; _LL794: if( !( Cyc_Tcutil_unify(
_temp793, t1)? 1: Cyc_Tcutil_coerceable( _temp793))){ return Cyc_Tcexp_expr_err(
te, loc,({ unsigned char* _temp795=( unsigned char*)"Cannot use this operator in an assignment";
struct _tagged_string _temp796; _temp796.curr= _temp795; _temp796.base= _temp795;
_temp796.last_plus_one= _temp795 + 42; _temp796;}));}}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* tu, struct
_RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List* fs){ struct
Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp799; struct Cyc_Position_Segment* _temp801; struct Cyc_List_List*
_temp803; struct Cyc_List_List* _temp805; struct _tuple1* _temp807; struct Cyc_Absyn_Tunionfield
_temp797=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL808: _temp807= _temp797.name; goto _LL806; _LL806: _temp805=
_temp797.tvs; goto _LL804; _LL804: _temp803= _temp797.typs; goto _LL802; _LL802:
_temp801= _temp797.loc; goto _LL800; _LL800: _temp799=( void*) _temp797.sc; goto
_LL798; _LL798: if(( _temp803 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp803))->tl != 0)? 1: _temp805 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp803))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp811; struct Cyc_Position_Segment* _temp813; struct Cyc_List_List*
_temp815; struct Cyc_List_List* _temp817; struct _tuple1* _temp819; struct Cyc_Absyn_Tunionfield
_temp809=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL820: _temp819= _temp809.name; goto _LL818; _LL818: _temp817=
_temp809.tvs; goto _LL816; _LL816: _temp815= _temp809.typs; goto _LL814; _LL814:
_temp813= _temp809.loc; goto _LL812; _LL812: _temp811=( void*) _temp809.sc; goto
_LL810; _LL810: if(( _temp815 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp815))->tl != 0)? 1: _temp817 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp815))->hd)).f2);
if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ struct _tagged_string _temp821= Cyc_Absynpp_typ2string(
tu); struct _tagged_string _temp822= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_temp821.last_plus_one - _temp821.curr, _temp821.curr, _temp822.last_plus_one -
_temp822.curr, _temp822.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){ te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp823= t; struct Cyc_Absyn_PtrInfo _temp829; struct Cyc_Absyn_Conref*
_temp831; struct Cyc_Absyn_Tqual _temp833; struct Cyc_Absyn_Conref* _temp835;
void* _temp837; void* _temp839; _LL825: if(( unsigned int) _temp823 > 4u?*(( int*)
_temp823) == Cyc_Absyn_PointerType: 0){ _LL830: _temp829=(( struct Cyc_Absyn_PointerType_struct*)
_temp823)->f1; _LL840: _temp839=( void*) _temp829.elt_typ; goto _LL838; _LL838:
_temp837=( void*) _temp829.rgn_typ; goto _LL836; _LL836: _temp835= _temp829.nullable;
goto _LL834; _LL834: _temp833= _temp829.tq; goto _LL832; _LL832: _temp831=
_temp829.bounds; goto _LL826;} else{ goto _LL827;} _LL827: goto _LL828; _LL826:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp837); Cyc_Tcutil_check_nonzero_bound(
loc, _temp831);{ void* _temp841= Cyc_Tcutil_compress( _temp839); struct Cyc_Absyn_FnInfo
_temp847; struct Cyc_List_List* _temp849; struct Cyc_Absyn_VarargInfo* _temp851;
int _temp853; struct Cyc_List_List* _temp855; void* _temp857; struct Cyc_Core_Opt*
_temp859; struct Cyc_List_List* _temp861; _LL843: if(( unsigned int) _temp841 >
4u?*(( int*) _temp841) == Cyc_Absyn_FnType: 0){ _LL848: _temp847=(( struct Cyc_Absyn_FnType_struct*)
_temp841)->f1; _LL862: _temp861= _temp847.tvars; goto _LL860; _LL860: _temp859=
_temp847.effect; goto _LL858; _LL858: _temp857=( void*) _temp847.ret_typ; goto
_LL856; _LL856: _temp855= _temp847.args; goto _LL854; _LL854: _temp853= _temp847.c_varargs;
goto _LL852; _LL852: _temp851= _temp847.cyc_varargs; goto _LL850; _LL850:
_temp849= _temp847.attributes; goto _LL844;} else{ goto _LL845;} _LL845: goto
_LL846; _LL844: if( topt != 0){ Cyc_Tcutil_unify( _temp857,*(( void**)
_check_null( topt)));} while( es != 0? _temp855 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp855))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp863=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
struct _tagged_string _temp864= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp863.last_plus_one - _temp863.curr, _temp863.curr, _temp864.last_plus_one -
_temp864.curr, _temp864.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp855=(( struct Cyc_List_List*) _check_null( _temp855))->tl;} if( _temp855 !=
0){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp865=( unsigned char*)"too few arguments for function";
struct _tagged_string _temp866; _temp866.curr= _temp865; _temp866.base= _temp865;
_temp866.last_plus_one= _temp865 + 31; _temp866;}));} else{ if( es != 0){ if(
_temp853){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd);}} else{ if( _temp851 == 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp867=( unsigned char*)"too many arguments for function";
struct _tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 32; _temp868;}));} else{ int _temp871; void*
_temp873; void* _temp875; struct Cyc_Absyn_Tqual _temp877; struct Cyc_Core_Opt*
_temp879; struct Cyc_Absyn_VarargInfo _temp869=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp851)); _LL880: _temp879= _temp869.name; goto _LL878; _LL878:
_temp877= _temp869.tq; goto _LL876; _LL876: _temp875=( void*) _temp869.type;
goto _LL874; _LL874: _temp873=( void*) _temp869.rgn; goto _LL872; _LL872:
_temp871= _temp869.inject; goto _LL870; _LL870: { struct Cyc_Absyn_VarargCallInfo*
_temp882=({ struct Cyc_Absyn_VarargCallInfo* _temp881=( struct Cyc_Absyn_VarargCallInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp881->num_varargs= 0;
_temp881->injectors= 0; _temp881->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp851); _temp881;}); goto _LL883; _LL883:* vararg_call_info=
_temp882; if( ! Cyc_Tcutil_unify( Cyc_Tcenv_curr_rgn( te), _temp873)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp884= Cyc_Absynpp_typ2string(
_temp873); struct _tagged_string _temp885= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn(
te)); xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_temp884.last_plus_one - _temp884.curr, _temp884.curr, _temp885.last_plus_one -
_temp885.curr, _temp885.curr);}));} if( ! _temp871){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp882))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp875,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp875)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp886= Cyc_Absynpp_typ2string(
_temp875); struct _tagged_string _temp887= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_temp886.last_plus_one - _temp886.curr, _temp886.curr, _temp887.last_plus_one -
_temp887.curr, _temp887.curr);}));}}} else{ void* _temp888= Cyc_Tcutil_compress(
_temp875); struct Cyc_Absyn_TunionInfo _temp894; void* _temp896; struct Cyc_List_List*
_temp898; void* _temp900; struct Cyc_Absyn_Tuniondecl* _temp902; _LL890: if((
unsigned int) _temp888 > 4u?*(( int*) _temp888) == Cyc_Absyn_TunionType: 0){
_LL895: _temp894=(( struct Cyc_Absyn_TunionType_struct*) _temp888)->f1; _LL901:
_temp900=( void*) _temp894.tunion_info; if(*(( int*) _temp900) == Cyc_Absyn_KnownTunion){
_LL903: _temp902=(( struct Cyc_Absyn_KnownTunion_struct*) _temp900)->f1; goto
_LL899;} else{ goto _LL892;} _LL899: _temp898= _temp894.targs; goto _LL897;
_LL897: _temp896=( void*) _temp894.rgn; goto _LL891;} else{ goto _LL892;} _LL892:
goto _LL893; _LL891: { struct Cyc_Absyn_Tuniondecl* _temp904=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp902->name); goto _LL905; _LL905: if( ! Cyc_Tcutil_unify( _temp896,
_temp873)){ Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("bad region for injected varargs"));}{
struct Cyc_List_List* fields= 0; if( _temp904->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp906= Cyc_Absynpp_typ2string(
_temp875); xprintf("can't inject into %.*s", _temp906.last_plus_one - _temp906.curr,
_temp906.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp904->fields))->v;}{ struct _RegionHandle _temp907= _new_region();
struct _RegionHandle* rgn=& _temp907; _push_region( rgn);{ struct Cyc_List_List*
_temp908=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp904->tvs, _temp898); goto _LL909; _LL909: for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp882))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp910= Cyc_Tcexp_tcInjection( te, e1, _temp875,
rgn, _temp908, fields); goto _LL911; _LL911: if( _temp910 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp882))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp882))->injectors,({ struct Cyc_List_List* _temp912=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp912->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp910)); _temp912->tl= 0;
_temp912;}));}}}}}; _pop_region(& _temp907);} goto _LL889;}} _LL893: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp913=( unsigned char*)"bad inject vararg type"; struct
_tagged_string _temp914; _temp914.curr= _temp913; _temp914.base= _temp913;
_temp914.last_plus_one= _temp913 + 23; _temp914;})); goto _LL889; _LL889:;}}}}}}
Cyc_Tcenv_check_effect_accessible( te, loc,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp859))->v); return _temp857; _LL846: return Cyc_Tcexp_expr_err(
te, loc,({ unsigned char* _temp915=( unsigned char*)"expected pointer to function";
struct _tagged_string _temp916; _temp916.curr= _temp915; _temp916.base= _temp915;
_temp916.last_plus_one= _temp915 + 29; _temp916;})); _LL842:;} _LL828: return
Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp917=( unsigned char*)"expected pointer to function";
struct _tagged_string _temp918; _temp918.curr= _temp917; _temp918.base= _temp917;
_temp918.last_plus_one= _temp917 + 29; _temp918;})); _LL824:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp919= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_temp919.last_plus_one - _temp919.curr, _temp919.curr);}));} return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp920=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp920->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp920;}));}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp921= t1; struct Cyc_Absyn_PtrInfo
_temp927; struct Cyc_Absyn_Conref* _temp929; struct Cyc_Absyn_Tqual _temp931;
struct Cyc_Absyn_Conref* _temp933; void* _temp935; void* _temp937; _LL923: if((
unsigned int) _temp921 > 4u?*(( int*) _temp921) == Cyc_Absyn_PointerType: 0){
_LL928: _temp927=(( struct Cyc_Absyn_PointerType_struct*) _temp921)->f1; _LL938:
_temp937=( void*) _temp927.elt_typ; goto _LL936; _LL936: _temp935=( void*)
_temp927.rgn_typ; goto _LL934; _LL934: _temp933= _temp927.nullable; goto _LL932;
_LL932: _temp931= _temp927.tq; goto _LL930; _LL930: _temp929= _temp927.bounds;
goto _LL924;} else{ goto _LL925;} _LL925: goto _LL926; _LL924:{ void* _temp939=
Cyc_Tcutil_compress( _temp937); struct Cyc_Absyn_FnInfo _temp945; struct Cyc_List_List*
_temp947; struct Cyc_Absyn_VarargInfo* _temp949; int _temp951; struct Cyc_List_List*
_temp953; void* _temp955; struct Cyc_Core_Opt* _temp957; struct Cyc_List_List*
_temp959; _LL941: if(( unsigned int) _temp939 > 4u?*(( int*) _temp939) == Cyc_Absyn_FnType:
0){ _LL946: _temp945=(( struct Cyc_Absyn_FnType_struct*) _temp939)->f1; _LL960:
_temp959= _temp945.tvars; goto _LL958; _LL958: _temp957= _temp945.effect; goto
_LL956; _LL956: _temp955=( void*) _temp945.ret_typ; goto _LL954; _LL954:
_temp953= _temp945.args; goto _LL952; _LL952: _temp951= _temp945.c_varargs; goto
_LL950; _LL950: _temp949= _temp945.cyc_varargs; goto _LL948; _LL948: _temp947=
_temp945.attributes; goto _LL942;} else{ goto _LL943;} _LL943: goto _LL944;
_LL942: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp959)){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp961=(
unsigned char*)"type instantiation mismatch"; struct _tagged_string _temp962;
_temp962.curr= _temp961; _temp962.base= _temp961; _temp962.last_plus_one=
_temp961 + 28; _temp962;}));} for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, _temp959=(( struct Cyc_List_List*) _check_null( _temp959))->tl)){
void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp959))->hd)->kind);
Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({ struct Cyc_List_List*
_temp963=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp963->hd=( void*)({ struct _tuple5* _temp964=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp964->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp959))->hd; _temp964->f2=( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd; _temp964;}); _temp963->tl= instantiation; _temp963;});}{
void* new_fn_typ= Cyc_Tcutil_substitute( instantiation,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp968=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp968[ 0]=({ struct Cyc_Absyn_FnType_struct _temp969; _temp969.tag= Cyc_Absyn_FnType;
_temp969.f1=({ struct Cyc_Absyn_FnInfo _temp970; _temp970.tvars= 0; _temp970.effect=
_temp957; _temp970.ret_typ=( void*) _temp955; _temp970.args= _temp953; _temp970.c_varargs=
_temp951; _temp970.cyc_varargs= _temp949; _temp970.attributes= _temp947;
_temp970;}); _temp969;}); _temp968;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp965=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp965[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp966; _temp966.tag= Cyc_Absyn_PointerType;
_temp966.f1=({ struct Cyc_Absyn_PtrInfo _temp967; _temp967.elt_typ=( void*)
new_fn_typ; _temp967.rgn_typ=( void*) _temp935; _temp967.nullable= _temp933;
_temp967.tq= _temp931; _temp967.bounds= _temp929; _temp967;}); _temp966;});
_temp965;}); return new_typ;}} _LL944: goto _LL940; _LL940:;} goto _LL922;
_LL926: goto _LL922; _LL922:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ struct _tagged_string _temp971= Cyc_Absynpp_typ2string( t1);
xprintf("expecting polymorphic type but found %.*s", _temp971.last_plus_one -
_temp971.curr, _temp971.curr);}));}} static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t, struct Cyc_Absyn_Exp*
e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? !
Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ struct _tagged_string _temp972= Cyc_Absynpp_typ2string(
t2); struct _tagged_string _temp973= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp972.last_plus_one - _temp972.curr, _temp972.curr, _temp973.last_plus_one -
_temp973.curr, _temp973.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** _temp974= 0; goto _LL975; _LL975: { struct Cyc_Absyn_Tqual
_temp976= Cyc_Absyn_empty_tqual(); goto _LL977; _LL977: if( topt != 0){ void*
_temp978= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp984; struct Cyc_Absyn_Conref* _temp986; struct Cyc_Absyn_Tqual _temp988;
struct Cyc_Absyn_Conref* _temp990; void* _temp992; void* _temp994; _LL980: if((
unsigned int) _temp978 > 4u?*(( int*) _temp978) == Cyc_Absyn_PointerType: 0){
_LL985: _temp984=(( struct Cyc_Absyn_PointerType_struct*) _temp978)->f1; _LL995:
_temp994=( void*) _temp984.elt_typ; goto _LL993; _LL993: _temp992=( void*)
_temp984.rgn_typ; goto _LL991; _LL991: _temp990= _temp984.nullable; goto _LL989;
_LL989: _temp988= _temp984.tq; goto _LL987; _LL987: _temp986= _temp984.bounds;
goto _LL981;} else{ goto _LL982;} _LL982: goto _LL983; _LL981: _temp974=({ void**
_temp996=( void**) GC_malloc( sizeof( void*)); _temp996[ 0]= _temp994; _temp996;});
_temp976= _temp988; goto _LL979; _LL983: goto _LL979; _LL979:;} Cyc_Tcexp_tcExpNoInst(
te, _temp974, e);{ void* _temp997=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp1005; struct Cyc_List_List* _temp1007; struct Cyc_Core_Opt* _temp1009;
struct _tuple1* _temp1011; struct Cyc_List_List* _temp1013; _LL999: if(*(( int*)
_temp997) == Cyc_Absyn_Struct_e){ _LL1012: _temp1011=(( struct Cyc_Absyn_Struct_e_struct*)
_temp997)->f1; goto _LL1010; _LL1010: _temp1009=(( struct Cyc_Absyn_Struct_e_struct*)
_temp997)->f2; goto _LL1008; _LL1008: _temp1007=(( struct Cyc_Absyn_Struct_e_struct*)
_temp997)->f3; goto _LL1006; _LL1006: _temp1005=(( struct Cyc_Absyn_Struct_e_struct*)
_temp997)->f4; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(*(( int*)
_temp997) == Cyc_Absyn_Tuple_e){ _LL1014: _temp1013=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp997)->f1; goto _LL1002;} else{ goto _LL1003;} _LL1003: goto _LL1004;
_LL1000: goto _LL1002; _LL1002: Cyc_Tcutil_warn( loc,({ unsigned char* _temp1015=(
unsigned char*)"& used to allocate"; struct _tagged_string _temp1016; _temp1016.curr=
_temp1015; _temp1016.base= _temp1015; _temp1016.last_plus_one= _temp1015 + 19;
_temp1016;}));{ void*(* _temp1017)( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq)= Cyc_Absyn_at_typ; goto _LL1018; _LL1018: if( topt != 0){ void* _temp1019=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1025; struct Cyc_Absyn_Conref* _temp1027; struct Cyc_Absyn_Tqual _temp1029;
struct Cyc_Absyn_Conref* _temp1031; void* _temp1033; void* _temp1035; _LL1021:
if(( unsigned int) _temp1019 > 4u?*(( int*) _temp1019) == Cyc_Absyn_PointerType:
0){ _LL1026: _temp1025=(( struct Cyc_Absyn_PointerType_struct*) _temp1019)->f1;
_LL1036: _temp1035=( void*) _temp1025.elt_typ; goto _LL1034; _LL1034: _temp1033=(
void*) _temp1025.rgn_typ; goto _LL1032; _LL1032: _temp1031= _temp1025.nullable;
goto _LL1030; _LL1030: _temp1029= _temp1025.tq; goto _LL1028; _LL1028: _temp1027=
_temp1025.bounds; goto _LL1022;} else{ goto _LL1023;} _LL1023: goto _LL1024;
_LL1022: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1031)){ _temp1017= Cyc_Absyn_star_typ;} goto _LL1020; _LL1024: goto _LL1020;
_LL1020:;} return _temp1017(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp976);} _LL1004: goto _LL998; _LL998:;}{ void*
_temp1039; int _temp1041; struct _tuple6 _temp1037= Cyc_Tcutil_addressof_props(
te, e); _LL1042: _temp1041= _temp1037.f1; goto _LL1040; _LL1040: _temp1039=
_temp1037.f2; goto _LL1038; _LL1038: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
if( _temp1041){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp1039, tq); return t;}}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1043= t;
struct Cyc_Absyn_PtrInfo _temp1049; struct Cyc_Absyn_Conref* _temp1051; struct
Cyc_Absyn_Tqual _temp1053; struct Cyc_Absyn_Conref* _temp1055; void* _temp1057;
void* _temp1059; _LL1045: if(( unsigned int) _temp1043 > 4u?*(( int*) _temp1043)
== Cyc_Absyn_PointerType: 0){ _LL1050: _temp1049=(( struct Cyc_Absyn_PointerType_struct*)
_temp1043)->f1; _LL1060: _temp1059=( void*) _temp1049.elt_typ; goto _LL1058;
_LL1058: _temp1057=( void*) _temp1049.rgn_typ; goto _LL1056; _LL1056: _temp1055=
_temp1049.nullable; goto _LL1054; _LL1054: _temp1053= _temp1049.tq; goto _LL1052;
_LL1052: _temp1051= _temp1049.bounds; goto _LL1046;} else{ goto _LL1047;}
_LL1047: goto _LL1048; _LL1046: Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1057); Cyc_Tcutil_check_nonzero_bound( loc, _temp1051); return _temp1059;
_LL1048: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ struct
_tagged_string _temp1061= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp1061.last_plus_one - _temp1061.curr, _temp1061.curr);})); _LL1044:;}}
static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1062= t;
struct Cyc_Absyn_Structdecl** _temp1078; struct Cyc_Absyn_Structdecl* _temp1080;
struct Cyc_List_List* _temp1081; struct _tuple1* _temp1083; struct Cyc_List_List*
_temp1085; struct Cyc_List_List* _temp1087; struct Cyc_Absyn_Uniondecl**
_temp1089; struct Cyc_Absyn_Uniondecl* _temp1091; struct Cyc_List_List*
_temp1092; struct _tuple1* _temp1094; struct Cyc_Absyn_Exp* _temp1097; struct
Cyc_Absyn_Tqual _temp1099; void* _temp1101; struct Cyc_Absyn_PtrInfo _temp1106;
_LL1064: if(( unsigned int) _temp1062 > 4u?*(( int*) _temp1062) == Cyc_Absyn_StructType:
0){ _LL1084: _temp1083=(( struct Cyc_Absyn_StructType_struct*) _temp1062)->f1;
goto _LL1082; _LL1082: _temp1081=(( struct Cyc_Absyn_StructType_struct*)
_temp1062)->f2; goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_StructType_struct*)
_temp1062)->f3; if( _temp1078 == 0){ goto _LL1066;} else{ _temp1080=* _temp1078;
goto _LL1065;}} else{ goto _LL1066;} _LL1066: if(( unsigned int) _temp1062 > 4u?*((
int*) _temp1062) == Cyc_Absyn_AnonStructType: 0){ _LL1086: _temp1085=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1062)->f1; goto _LL1067;} else{ goto
_LL1068;} _LL1068: if(( unsigned int) _temp1062 > 4u?*(( int*) _temp1062) == Cyc_Absyn_AnonUnionType:
0){ _LL1088: _temp1087=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1062)->f1;
goto _LL1069;} else{ goto _LL1070;} _LL1070: if(( unsigned int) _temp1062 > 4u?*((
int*) _temp1062) == Cyc_Absyn_UnionType: 0){ _LL1095: _temp1094=(( struct Cyc_Absyn_UnionType_struct*)
_temp1062)->f1; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_UnionType_struct*)
_temp1062)->f2; goto _LL1090; _LL1090: _temp1089=(( struct Cyc_Absyn_UnionType_struct*)
_temp1062)->f3; if( _temp1089 == 0){ goto _LL1072;} else{ _temp1091=* _temp1089;
goto _LL1071;}} else{ goto _LL1072;} _LL1072: if(( unsigned int) _temp1062 > 4u?*((
int*) _temp1062) == Cyc_Absyn_ArrayType: 0){ _LL1102: _temp1101=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1062)->f1; goto _LL1100; _LL1100:
_temp1099=(( struct Cyc_Absyn_ArrayType_struct*) _temp1062)->f2; goto _LL1098;
_LL1098: _temp1097=(( struct Cyc_Absyn_ArrayType_struct*) _temp1062)->f3; goto
_LL1096;} else{ goto _LL1074;} _LL1096: if( Cyc_String_zstrcmp(* f,({
unsigned char* _temp1103=( unsigned char*)"size"; struct _tagged_string
_temp1104; _temp1104.curr= _temp1103; _temp1104.base= _temp1103; _temp1104.last_plus_one=
_temp1103 + 5; _temp1104;})) == 0){ goto _LL1073;} else{ goto _LL1074;} _LL1074:
if(( unsigned int) _temp1062 > 4u?*(( int*) _temp1062) == Cyc_Absyn_PointerType:
0){ _LL1107: _temp1106=(( struct Cyc_Absyn_PointerType_struct*) _temp1062)->f1;
goto _LL1105;} else{ goto _LL1076;} _LL1105: if( Cyc_String_zstrcmp(* f,({
unsigned char* _temp1108=( unsigned char*)"size"; struct _tagged_string
_temp1109; _temp1109.curr= _temp1108; _temp1109.base= _temp1108; _temp1109.last_plus_one=
_temp1108 + 5; _temp1109;})) == 0){ goto _LL1075;} else{ goto _LL1076;} _LL1076:
goto _LL1077; _LL1065: if( _temp1083 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp1110=( unsigned char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1111; _temp1111.curr= _temp1110; _temp1111.base=
_temp1110; _temp1111.last_plus_one= _temp1110 + 40; _temp1111;}));}{ struct Cyc_Absyn_Structfield*
_temp1112= Cyc_Absyn_lookup_struct_field( _temp1080, f); goto _LL1113; _LL1113:
if( _temp1112 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
struct _tagged_string _temp1114= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1083)); struct _tagged_string _temp1115=* f; xprintf("struct %.*s has no %.*s field",
_temp1114.last_plus_one - _temp1114.curr, _temp1114.curr, _temp1115.last_plus_one
- _temp1115.curr, _temp1115.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1116= _new_region(); struct _RegionHandle* rgn=& _temp1116; _push_region(
rgn);{ struct Cyc_List_List* _temp1117=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1080->tvs, _temp1081); goto _LL1118; _LL1118:
t2= Cyc_Tcutil_rsubstitute( rgn, _temp1117,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1112))->type);}; _pop_region(& _temp1116);} return t2;}}
_LL1067: { struct Cyc_Absyn_Structfield* _temp1119= Cyc_Absyn_lookup_field(
_temp1085, f); goto _LL1120; _LL1120: if( _temp1119 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ struct _tagged_string _temp1121=* f; xprintf("struct has no %.*s field",
_temp1121.last_plus_one - _temp1121.curr, _temp1121.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1119))->type;} _LL1069: {
struct Cyc_Absyn_Structfield* _temp1122= Cyc_Absyn_lookup_field( _temp1087, f);
goto _LL1123; _LL1123: if( _temp1122 == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ struct _tagged_string _temp1124=* f; xprintf("union has no %.*s field",
_temp1124.last_plus_one - _temp1124.curr, _temp1124.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1122))->type;} _LL1071: if(
_temp1094 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp1125=(
unsigned char*)"unresolved union type (COMPILER ERROR)"; struct _tagged_string
_temp1126; _temp1126.curr= _temp1125; _temp1126.base= _temp1125; _temp1126.last_plus_one=
_temp1125 + 39; _temp1126;}));}{ struct Cyc_Absyn_Structfield* _temp1127= Cyc_Absyn_lookup_union_field(
_temp1091, f); goto _LL1128; _LL1128: if( _temp1127 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ struct _tagged_string _temp1129= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1094)); struct _tagged_string _temp1130=* f;
xprintf("union %.*s has no %.*s field", _temp1129.last_plus_one - _temp1129.curr,
_temp1129.curr, _temp1130.last_plus_one - _temp1130.curr, _temp1130.curr);}));}{
struct Cyc_List_List* _temp1131=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1091->tvs, _temp1092); goto
_LL1132; _LL1132: { void* _temp1133= Cyc_Tcutil_substitute( _temp1131,( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1127))->type); goto _LL1134;
_LL1134: return _temp1133;}}} _LL1073:( void*)( outer_e->r=( void*)(( void*)({
struct Cyc_Absyn_Primop_e_struct* _temp1135=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1135[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1136; _temp1136.tag= Cyc_Absyn_Primop_e;
_temp1136.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1136.f2=({ struct Cyc_List_List*
_temp1137=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1137->hd=( void*) e; _temp1137->tl= 0; _temp1137;}); _temp1136;});
_temp1135;}))); return Cyc_Absyn_uint_t; _LL1075:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1138=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1138[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1139; _temp1139.tag= Cyc_Absyn_Primop_e;
_temp1139.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1139.f2=({ struct Cyc_List_List*
_temp1140=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1140->hd=( void*) e; _temp1140->tl= 0; _temp1140;}); _temp1139;});
_temp1138;}))); return Cyc_Absyn_uint_t; _LL1077: if( Cyc_String_zstrcmp(* f,({
unsigned char* _temp1141=( unsigned char*)"size"; struct _tagged_string
_temp1142; _temp1142.curr= _temp1141; _temp1142.base= _temp1141; _temp1142.last_plus_one=
_temp1141 + 5; _temp1142;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ struct _tagged_string _temp1143= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct, union or array, found %.*s", _temp1143.last_plus_one
- _temp1143.curr, _temp1143.curr);}));} else{ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ struct _tagged_string _temp1144= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct or union, found %.*s", _temp1144.last_plus_one -
_temp1144.curr, _temp1144.curr);}));} _LL1063:;}} static void* Cyc_Tcexp_tcStructArrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void*
t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp1145= t; struct Cyc_Absyn_PtrInfo _temp1151; struct Cyc_Absyn_Conref*
_temp1153; struct Cyc_Absyn_Tqual _temp1155; struct Cyc_Absyn_Conref* _temp1157;
void* _temp1159; void* _temp1161; _LL1147: if(( unsigned int) _temp1145 > 4u?*((
int*) _temp1145) == Cyc_Absyn_PointerType: 0){ _LL1152: _temp1151=(( struct Cyc_Absyn_PointerType_struct*)
_temp1145)->f1; _LL1162: _temp1161=( void*) _temp1151.elt_typ; goto _LL1160;
_LL1160: _temp1159=( void*) _temp1151.rgn_typ; goto _LL1158; _LL1158: _temp1157=
_temp1151.nullable; goto _LL1156; _LL1156: _temp1155= _temp1151.tq; goto _LL1154;
_LL1154: _temp1153= _temp1151.bounds; goto _LL1148;} else{ goto _LL1149;}
_LL1149: goto _LL1150; _LL1148: Cyc_Tcutil_check_nonzero_bound( loc, _temp1153);{
void* _temp1163= Cyc_Tcutil_compress( _temp1161); struct Cyc_Absyn_Structdecl**
_temp1175; struct Cyc_Absyn_Structdecl* _temp1177; struct Cyc_List_List*
_temp1178; struct _tuple1* _temp1180; struct Cyc_Absyn_Uniondecl** _temp1182;
struct Cyc_Absyn_Uniondecl* _temp1184; struct Cyc_List_List* _temp1185; struct
_tuple1* _temp1187; struct Cyc_List_List* _temp1189; struct Cyc_List_List*
_temp1191; _LL1165: if(( unsigned int) _temp1163 > 4u?*(( int*) _temp1163) ==
Cyc_Absyn_StructType: 0){ _LL1181: _temp1180=(( struct Cyc_Absyn_StructType_struct*)
_temp1163)->f1; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_StructType_struct*)
_temp1163)->f2; goto _LL1176; _LL1176: _temp1175=(( struct Cyc_Absyn_StructType_struct*)
_temp1163)->f3; if( _temp1175 == 0){ goto _LL1167;} else{ _temp1177=* _temp1175;
goto _LL1166;}} else{ goto _LL1167;} _LL1167: if(( unsigned int) _temp1163 > 4u?*((
int*) _temp1163) == Cyc_Absyn_UnionType: 0){ _LL1188: _temp1187=(( struct Cyc_Absyn_UnionType_struct*)
_temp1163)->f1; goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_UnionType_struct*)
_temp1163)->f2; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_UnionType_struct*)
_temp1163)->f3; if( _temp1182 == 0){ goto _LL1169;} else{ _temp1184=* _temp1182;
goto _LL1168;}} else{ goto _LL1169;} _LL1169: if(( unsigned int) _temp1163 > 4u?*((
int*) _temp1163) == Cyc_Absyn_AnonStructType: 0){ _LL1190: _temp1189=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1163)->f1; goto _LL1170;} else{ goto
_LL1171;} _LL1171: if(( unsigned int) _temp1163 > 4u?*(( int*) _temp1163) == Cyc_Absyn_AnonUnionType:
0){ _LL1192: _temp1191=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1163)->f1;
goto _LL1172;} else{ goto _LL1173;} _LL1173: goto _LL1174; _LL1166: { struct Cyc_Absyn_Structfield*
_temp1193= Cyc_Absyn_lookup_struct_field( _temp1177, f); goto _LL1194; _LL1194:
if( _temp1193 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
struct _tagged_string _temp1195= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1180)); struct _tagged_string _temp1196=* f; xprintf("struct %.*s has no %.*s field",
_temp1195.last_plus_one - _temp1195.curr, _temp1195.curr, _temp1196.last_plus_one
- _temp1196.curr, _temp1196.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1197= _new_region(); struct _RegionHandle* rgn=& _temp1197; _push_region(
rgn);{ struct Cyc_List_List* _temp1198=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1177->tvs, _temp1178); goto _LL1199; _LL1199:
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1198,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1193))->type);}; _pop_region(& _temp1197);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1159); return t3;}} _LL1168: { struct Cyc_Absyn_Structfield*
_temp1200= Cyc_Absyn_lookup_union_field( _temp1184, f); goto _LL1201; _LL1201:
if( _temp1200 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
struct _tagged_string _temp1202= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1187)); struct _tagged_string _temp1203=* f; xprintf("union %.*s has no %.*s field",
_temp1202.last_plus_one - _temp1202.curr, _temp1202.curr, _temp1203.last_plus_one
- _temp1203.curr, _temp1203.curr);}));}{ struct Cyc_List_List* _temp1204=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1184->tvs, _temp1185); goto _LL1205; _LL1205: { void* _temp1206= Cyc_Tcutil_substitute(
_temp1204,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1200))->type);
goto _LL1207; _LL1207: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1159);
return _temp1206;}}} _LL1170: { struct Cyc_Absyn_Structfield* _temp1208= Cyc_Absyn_lookup_field(
_temp1189, f); goto _LL1209; _LL1209: if( _temp1208 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ struct _tagged_string _temp1210=* f; xprintf("struct has no %.*s field",
_temp1210.last_plus_one - _temp1210.curr, _temp1210.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1208))->type;} _LL1172: {
struct Cyc_Absyn_Structfield* _temp1211= Cyc_Absyn_lookup_field( _temp1191, f);
goto _LL1212; _LL1212: if( _temp1211 == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ struct _tagged_string _temp1213=* f; xprintf("union has no %.*s field",
_temp1213.last_plus_one - _temp1213.curr, _temp1213.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1211))->type;} _LL1174: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ struct _tagged_string
_temp1214= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1214.last_plus_one - _temp1214.curr, _temp1214.curr);})); _LL1164:;}
_LL1150: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ struct
_tagged_string _temp1215= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1215.last_plus_one - _temp1215.curr, _temp1215.curr);})); _LL1146:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1216; _push_handler(& _temp1216);{ int
_temp1218= 0; if( setjmp( _temp1216.handler)){ _temp1218= 1;} if( ! _temp1218){{
void* _temp1219=(*(( struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1219;}; _pop_handler();} else{
void* _temp1217=( void*) _exn_thrown; void* _temp1221= _temp1217; _LL1223: if(
_temp1221 == Cyc_List_Nth){ goto _LL1224;} else{ goto _LL1225;} _LL1225: goto
_LL1226; _LL1224: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)
xprintf("index is %d but tuple has only %d fields",( int) i,(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1226:( void) _throw( _temp1221);
_LL1222:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0, e2);{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,(
struct _tagged_string)({ struct _tagged_string _temp1227= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int subscript, found %.*s", _temp1227.last_plus_one -
_temp1227.curr, _temp1227.curr);}));}{ void* _temp1228= t1; struct Cyc_Absyn_PtrInfo
_temp1236; struct Cyc_Absyn_Conref* _temp1238; struct Cyc_Absyn_Tqual _temp1240;
struct Cyc_Absyn_Conref* _temp1242; void* _temp1244; void* _temp1246; struct Cyc_List_List*
_temp1248; _LL1230: if(( unsigned int) _temp1228 > 4u?*(( int*) _temp1228) ==
Cyc_Absyn_PointerType: 0){ _LL1237: _temp1236=(( struct Cyc_Absyn_PointerType_struct*)
_temp1228)->f1; _LL1247: _temp1246=( void*) _temp1236.elt_typ; goto _LL1245;
_LL1245: _temp1244=( void*) _temp1236.rgn_typ; goto _LL1243; _LL1243: _temp1242=
_temp1236.nullable; goto _LL1241; _LL1241: _temp1240= _temp1236.tq; goto _LL1239;
_LL1239: _temp1238= _temp1236.bounds; goto _LL1231;} else{ goto _LL1232;}
_LL1232: if(( unsigned int) _temp1228 > 4u?*(( int*) _temp1228) == Cyc_Absyn_TupleType:
0){ _LL1249: _temp1248=(( struct Cyc_Absyn_TupleType_struct*) _temp1228)->f1;
goto _LL1233;} else{ goto _LL1234;} _LL1234: goto _LL1235; _LL1231: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1238);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1238);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1244); return _temp1246; _LL1233: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1248, e2); _LL1235: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ struct _tagged_string _temp1250= Cyc_Absynpp_typ2string( t1);
xprintf("subscript applied to %.*s", _temp1250.last_plus_one - _temp1250.curr,
_temp1250.curr);})); _LL1229:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){
int done= 0; struct Cyc_List_List* fields= 0; if( topt != 0){ void* _temp1251=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_List_List*
_temp1257; _LL1253: if(( unsigned int) _temp1251 > 4u?*(( int*) _temp1251) ==
Cyc_Absyn_TupleType: 0){ _LL1258: _temp1257=(( struct Cyc_Absyn_TupleType_struct*)
_temp1251)->f1; goto _LL1254;} else{ goto _LL1255;} _LL1255: goto _LL1256;
_LL1254: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1257) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp1259=( unsigned char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1260; _temp1260.curr= _temp1259; _temp1260.base=
_temp1259; _temp1260.last_plus_one= _temp1259 + 48; _temp1260;})); goto _LL1252;}
for( 0; es != 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1257=((
struct Cyc_List_List*) _check_null( _temp1257))->tl)){ void* _temp1261=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1257))->hd)).f2;
goto _LL1262; _LL1262: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1261,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1263=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1263->hd=( void*)({ struct _tuple8* _temp1264=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1264->f1=(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( _temp1257))->hd)).f1; _temp1264->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1264;}); _temp1263->tl=
fields; _temp1263;});} done= 1; goto _LL1252; _LL1256: goto _LL1252; _LL1252:;}
if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1265=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1265->hd=( void*)({ struct
_tuple8* _temp1266=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1266->f1= Cyc_Absyn_empty_tqual(); _temp1266->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1266;}); _temp1265->tl= fields; _temp1265;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1267=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1267[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1268; _temp1268.tag= Cyc_Absyn_TupleType;
_temp1268.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1268;}); _temp1267;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp1269=( unsigned char*)"tcCompoundLit"; struct _tagged_string
_temp1270; _temp1270.curr= _temp1269; _temp1270.base= _temp1269; _temp1270.last_plus_one=
_temp1269 + 14; _temp1270;}));} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1293=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1293->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1293;})); struct Cyc_Absyn_Const_e_struct* _temp1275=({ struct Cyc_Absyn_Const_e_struct*
_temp1271=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1271[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1272; _temp1272.tag= Cyc_Absyn_Const_e;
_temp1272.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1273=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1273[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1274; _temp1274.tag= Cyc_Absyn_Int_c;
_temp1274.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1274.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1274;}); _temp1273;}));
_temp1272;}); _temp1271;}); goto _LL1276; _LL1276: { struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1275, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1277=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1277->v=( void*) Cyc_Absyn_uint_t; _temp1277;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1291=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1291[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1292; _temp1292.tag= Cyc_Absyn_ArrayType;
_temp1292.f1=( void*) res; _temp1292.f2= Cyc_Absyn_empty_tqual(); _temp1292.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1292;}); _temp1291;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_string)({ struct _tagged_string _temp1278= Cyc_Absynpp_typ2string(
res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1278.last_plus_one - _temp1278.curr, _temp1278.curr);}));}{ int offset= 0;
for( 0; des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1279=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_string* _temp1285; struct Cyc_Absyn_Exp*
_temp1287; _LL1281: if(*(( int*) _temp1279) == Cyc_Absyn_FieldName){ _LL1286:
_temp1285=(( struct Cyc_Absyn_FieldName_struct*) _temp1279)->f1; goto _LL1282;}
else{ goto _LL1283;} _LL1283: if(*(( int*) _temp1279) == Cyc_Absyn_ArrayElement){
_LL1288: _temp1287=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1279)->f1;
goto _LL1284;} else{ goto _LL1280;} _LL1282: Cyc_Tcutil_terr( loc,({
unsigned char* _temp1289=( unsigned char*)"only array index designators are supported";
struct _tagged_string _temp1290; _temp1290.curr= _temp1289; _temp1290.base=
_temp1289; _temp1290.last_plus_one= _temp1289 + 43; _temp1290;})); goto _LL1280;
_LL1284: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1287);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1287); if( i != offset){ Cyc_Tcutil_terr( _temp1287->loc,( struct
_tagged_string) xprintf("expecting index designator %d but found %d", offset,(
int) i));} goto _LL1280;} _LL1280:;}}} return res_t2;}}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ struct _tagged_string
_temp1294= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
bound->topt))->v); xprintf("expecting unsigned int, found %.*s", _temp1294.last_plus_one
- _temp1294.curr, _temp1294.curr);}));} if( !( vd->tq).q_const){(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp1295=(
unsigned char*)"comprehension index variable is not declared const!"; struct
_tagged_string _temp1296; _temp1296.curr= _temp1295; _temp1296.base= _temp1295;
_temp1296.last_plus_one= _temp1295 + 52; _temp1296;}));} if( te->le != 0){ te=
Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{
if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,({
unsigned char* _temp1297=( unsigned char*)"bound is not constant"; struct
_tagged_string _temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297;
_temp1298.last_plus_one= _temp1297 + 22; _temp1298;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,({ unsigned char* _temp1299=(
unsigned char*)"body is not constant"; struct _tagged_string _temp1300;
_temp1300.curr= _temp1299; _temp1300.base= _temp1299; _temp1300.last_plus_one=
_temp1299 + 21; _temp1300;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void**
_temp1301= 0; goto _LL1302; _LL1302: { struct Cyc_Absyn_Tqual* _temp1303= 0;
goto _LL1304; _LL1304: if( topt != 0){ void* _temp1305= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1313; struct Cyc_Absyn_Exp*
_temp1315; struct Cyc_Absyn_Tqual _temp1317; struct Cyc_Absyn_Tqual* _temp1319;
void* _temp1320; void** _temp1322; _LL1307: if(( unsigned int) _temp1305 > 4u?*((
int*) _temp1305) == Cyc_Absyn_PointerType: 0){ _LL1314: _temp1313=(( struct Cyc_Absyn_PointerType_struct*)
_temp1305)->f1; goto _LL1308;} else{ goto _LL1309;} _LL1309: if(( unsigned int)
_temp1305 > 4u?*(( int*) _temp1305) == Cyc_Absyn_ArrayType: 0){ _LL1321:
_temp1320=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1305)->f1;
_temp1322=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1305)->f1); goto
_LL1318; _LL1318: _temp1317=(( struct Cyc_Absyn_ArrayType_struct*) _temp1305)->f2;
_temp1319=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1305)->f2; goto _LL1316;
_LL1316: _temp1315=(( struct Cyc_Absyn_ArrayType_struct*) _temp1305)->f3; goto
_LL1310;} else{ goto _LL1311;} _LL1311: goto _LL1312; _LL1308: pinfo= _temp1313;
_temp1301=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1303=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1306; _LL1310: _temp1301=( void**) _temp1322; _temp1303=(
struct Cyc_Absyn_Tqual*) _temp1319; goto _LL1306; _LL1312: goto _LL1306; _LL1306:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1301, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1323=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1323[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1324; _temp1324.tag= Cyc_Absyn_ArrayType;
_temp1324.f1=( void*) t; _temp1324.f2= _temp1303 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1303)); _temp1324.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1324;}); _temp1323;});}}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1325; _push_handler(&
_temp1325);{ int _temp1327= 0; if( setjmp( _temp1325.handler)){ _temp1327= 1;}
if( ! _temp1327){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1326=( void*) _exn_thrown; void* _temp1329= _temp1326; _LL1331:
if( _temp1329 == Cyc_Dict_Absent){ goto _LL1332;} else{ goto _LL1333;} _LL1333:
goto _LL1334; _LL1332: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp1335= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1335.last_plus_one - _temp1335.curr, _temp1335.curr);})); return topt != 0?*((
void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1334:( void) _throw(
_temp1329); _LL1330:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1336= _new_region(); struct _RegionHandle* rgn=&
_temp1336; _push_region( rgn);{ struct _tuple4 _temp1338=({ struct _tuple4
_temp1337; _temp1337.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1337.f2= rgn;
_temp1337;}); goto _LL1339; _LL1339: { struct Cyc_List_List* _temp1340=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1338, sd->tvs); goto _LL1341; _LL1341: {
struct Cyc_List_List* _temp1342=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1340); goto _LL1343; _LL1343: { struct Cyc_Absyn_StructType_struct*
_temp1347=({ struct Cyc_Absyn_StructType_struct* _temp1344=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1344[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1345; _temp1345.tag= Cyc_Absyn_StructType;
_temp1345.f1=( struct _tuple1*)* tn; _temp1345.f2= _temp1342; _temp1345.f3=({
struct Cyc_Absyn_Structdecl** _temp1346=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1346[ 0]= sd; _temp1346;});
_temp1345;}); _temp1344;}); goto _LL1348; _LL1348: if( topt != 0){ Cyc_Tcutil_unify((
void*) _temp1347,*(( void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt*
_temp1349=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1349->v=( void*) _temp1342; _temp1349;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp1350=( unsigned char*)"can't build abstract struct";
struct _tagged_string _temp1351; _temp1351.curr= _temp1350; _temp1351.base=
_temp1350; _temp1351.last_plus_one= _temp1350 + 28; _temp1351;}));{ void*
_temp1352=( void*) _temp1347; _npop_handler( 0u); return _temp1352;}}{ struct
Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, loc, args,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( sd->fields))->v); for( 0; fields != 0; fields=((
struct Cyc_List_List*) _check_null( fields))->tl){ struct _tuple9 _temp1355;
struct Cyc_Absyn_Exp* _temp1356; struct Cyc_Absyn_Structfield* _temp1358; struct
_tuple9* _temp1353=( struct _tuple9*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1355=* _temp1353; _LL1359: _temp1358= _temp1355.f1; goto
_LL1357; _LL1357: _temp1356= _temp1355.f2; goto _LL1354; _LL1354: { void*
_temp1360= Cyc_Tcutil_rsubstitute( rgn, _temp1340,( void*) _temp1358->type);
goto _LL1361; _LL1361: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1360,
_temp1356); if( ! Cyc_Tcutil_coerce_arg( te, _temp1356, _temp1360)){ Cyc_Tcutil_terr(
_temp1356->loc,( struct _tagged_string)({ struct _tagged_string _temp1362=*
_temp1358->name; struct _tagged_string _temp1363= Cyc_Absynpp_qvar2string(* tn);
struct _tagged_string _temp1364= Cyc_Absynpp_typ2string( _temp1360); struct
_tagged_string _temp1365= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1356->topt))->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1362.last_plus_one - _temp1362.curr, _temp1362.curr, _temp1363.last_plus_one
- _temp1363.curr, _temp1363.curr, _temp1364.last_plus_one - _temp1364.curr,
_temp1364.curr, _temp1365.last_plus_one - _temp1365.curr, _temp1365.curr);}));}}}{
void* _temp1366=( void*) _temp1347; _npop_handler( 0u); return _temp1366;}}}}}};
_pop_region(& _temp1336);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1367= _new_region(); struct _RegionHandle* rgn=&
_temp1367; _push_region( rgn);{ void* _temp1368= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1374; _LL1370: if(( unsigned int) _temp1368 > 4u?*((
int*) _temp1368) == Cyc_Absyn_AnonStructType: 0){ _LL1375: _temp1374=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1368)->f1; goto _LL1371;} else{ goto
_LL1372;} _LL1372: goto _LL1373; _LL1371: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1374); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1378; struct Cyc_Absyn_Exp*
_temp1379; struct Cyc_Absyn_Structfield* _temp1381; struct _tuple9* _temp1376=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1378=*
_temp1376; _LL1382: _temp1381= _temp1378.f1; goto _LL1380; _LL1380: _temp1379=
_temp1378.f2; goto _LL1377; _LL1377: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1381->type)), _temp1379); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1379,( void*) _temp1381->type)){ Cyc_Tcutil_terr( _temp1379->loc,( struct
_tagged_string)({ struct _tagged_string _temp1383=* _temp1381->name; struct
_tagged_string _temp1384= Cyc_Absynpp_typ2string(( void*) _temp1381->type);
struct _tagged_string _temp1385= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1379->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1383.last_plus_one - _temp1383.curr, _temp1383.curr, _temp1384.last_plus_one
- _temp1384.curr, _temp1384.curr, _temp1385.last_plus_one - _temp1385.curr,
_temp1385.curr);}));}} goto _LL1369;} _LL1373:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp1386=( unsigned char*)"tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1387; _temp1387.curr= _temp1386; _temp1387.base=
_temp1386; _temp1387.last_plus_one= _temp1386 + 37; _temp1387;})); goto _LL1369;
_LL1369:;}; _pop_region(& _temp1367);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1388= _new_region(); struct _RegionHandle* rgn=&
_temp1388; _push_region( rgn);{ struct _tuple4 _temp1390=({ struct _tuple4
_temp1389; _temp1389.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1389.f2= rgn;
_temp1389;}); goto _LL1391; _LL1391: { struct Cyc_List_List* _temp1392=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1390, tud->tvs); goto _LL1393; _LL1393: {
struct Cyc_List_List* _temp1394=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1390, tuf->tvs); goto _LL1395; _LL1395: { struct Cyc_List_List* _temp1396=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1392); goto
_LL1397; _LL1397: { struct Cyc_List_List* _temp1398=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1394); goto _LL1399; _LL1399: { struct Cyc_List_List*
_temp1400=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1392, _temp1394); goto
_LL1401; _LL1401:* all_ref=({ struct Cyc_Core_Opt* _temp1402=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1402->v=( void*) _temp1396;
_temp1402;});* exist_ref=({ struct Cyc_Core_Opt* _temp1403=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1403->v=( void*) _temp1398;
_temp1403;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1439=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1439[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1440; _temp1440.tag= Cyc_Absyn_TunionFieldType; _temp1440.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1441; _temp1441.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1442=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1442[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1443; _temp1443.tag= Cyc_Absyn_KnownTunionfield; _temp1443.f1= tud;
_temp1443.f2= tuf; _temp1443;}); _temp1442;})); _temp1441.targs= _temp1396;
_temp1441;}); _temp1440;}); _temp1439;}); if( topt != 0){ void* _temp1404= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo _temp1412; struct
Cyc_Absyn_TunionInfo _temp1414; void* _temp1416; struct Cyc_List_List* _temp1418;
void* _temp1420; _LL1406: if(( unsigned int) _temp1404 > 4u?*(( int*) _temp1404)
== Cyc_Absyn_TunionFieldType: 0){ _LL1413: _temp1412=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1404)->f1; goto _LL1407;} else{ goto _LL1408;} _LL1408: if(( unsigned int)
_temp1404 > 4u?*(( int*) _temp1404) == Cyc_Absyn_TunionType: 0){ _LL1415:
_temp1414=(( struct Cyc_Absyn_TunionType_struct*) _temp1404)->f1; _LL1421:
_temp1420=( void*) _temp1414.tunion_info; goto _LL1419; _LL1419: _temp1418=
_temp1414.targs; goto _LL1417; _LL1417: _temp1416=( void*) _temp1414.rgn; goto
_LL1409;} else{ goto _LL1410;} _LL1410: goto _LL1411; _LL1407: Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res); goto _LL1405; _LL1409:{ struct Cyc_List_List*
a= _temp1396; for( 0; a != 0? _temp1418 != 0: 0;( a=(( struct Cyc_List_List*)
_check_null( a))->tl, _temp1418=(( struct Cyc_List_List*) _check_null( _temp1418))->tl)){
Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,( void*)((
struct Cyc_List_List*) _check_null( _temp1418))->hd);}} if( tuf->typs == 0? es
== 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1422=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1422->v=( void*) res; _temp1422;});
res=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp1423=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1423[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1424; _temp1424.tag= Cyc_Absyn_TunionType;
_temp1424.f1=({ struct Cyc_Absyn_TunionInfo _temp1425; _temp1425.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1426=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1426[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1427; _temp1427.tag= Cyc_Absyn_KnownTunion;
_temp1427.f1= tud; _temp1427;}); _temp1426;})); _temp1425.targs= _temp1396;
_temp1425.rgn=( void*) _temp1416; _temp1425;}); _temp1424;}); _temp1423;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1428= res; _npop_handler( 0u); return _temp1428;}} goto
_LL1405; _LL1411: goto _LL1405; _LL1405:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es != 0? ts != 0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1400,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ struct _tagged_string _temp1431= Cyc_Absynpp_qvar2string( tuf->name);
struct _tagged_string _temp1432= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp1433= e->topt == 0?({ unsigned char* _temp1429=(
unsigned char*)"?"; struct _tagged_string _temp1430; _temp1430.curr= _temp1429;
_temp1430.base= _temp1429; _temp1430.last_plus_one= _temp1429 + 2; _temp1430;}):
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1431.last_plus_one - _temp1431.curr, _temp1431.curr, _temp1432.last_plus_one
- _temp1432.curr, _temp1432.curr, _temp1433.last_plus_one - _temp1433.curr,
_temp1433.curr);}));}} if( es != 0){ void* _temp1435= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_string)({ struct _tagged_string _temp1434= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("too many arguments for tunion constructor %.*s", _temp1434.last_plus_one
- _temp1434.curr, _temp1434.curr);})); _npop_handler( 0u); return _temp1435;}
if( ts != 0){ void* _temp1437= Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ struct _tagged_string _temp1436= Cyc_Absynpp_qvar2string( tuf->name);
xprintf("too few arguments for tunion constructor %.*s", _temp1436.last_plus_one
- _temp1436.curr, _temp1436.curr);})); _npop_handler( 0u); return _temp1437;}{
void* _temp1438= res; _npop_handler( 0u); return _temp1438;}}}}}}}}};
_pop_region(& _temp1388);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1453=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1453[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1454; _temp1454.tag=
Cyc_Absyn_RgnHandleType; _temp1454.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1455=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1455->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1455;})); _temp1454;}); _temp1453;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void*
_temp1444= Cyc_Tcutil_compress( handle_type); void* _temp1450; _LL1446: if((
unsigned int) _temp1444 > 4u?*(( int*) _temp1444) == Cyc_Absyn_RgnHandleType: 0){
_LL1451: _temp1450=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1444)->f1;
goto _LL1447;} else{ goto _LL1448;} _LL1448: goto _LL1449; _LL1447: rgn=
_temp1450; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto _LL1445; _LL1449:
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null( ropt))->loc,( struct
_tagged_string)({ struct _tagged_string _temp1452= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1452.last_plus_one
- _temp1452.curr, _temp1452.curr);})); goto _LL1445; _LL1445:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1456= t; struct Cyc_Absyn_TunionFieldInfo _temp1462; struct Cyc_List_List*
_temp1464; void* _temp1466; struct Cyc_Absyn_Tunionfield* _temp1468; struct Cyc_Absyn_Tuniondecl*
_temp1470; _LL1458: if(( unsigned int) _temp1456 > 4u?*(( int*) _temp1456) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1463: _temp1462=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1456)->f1; _LL1467: _temp1466=( void*) _temp1462.field_info; if(*(( int*)
_temp1466) == Cyc_Absyn_KnownTunionfield){ _LL1471: _temp1470=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1466)->f1; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1466)->f2; goto _LL1465;} else{ goto _LL1460;} _LL1465: _temp1464=
_temp1462.targs; goto _LL1459;} else{ goto _LL1460;} _LL1460: goto _LL1461;
_LL1459: if( _temp1468->tvs != 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp1472=( unsigned char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1473; _temp1473.curr= _temp1472; _temp1473.base=
_temp1472; _temp1473.last_plus_one= _temp1472 + 50; _temp1473;}));} goto _LL1457;
_LL1461: goto _LL1457; _LL1457:;}{ void*(* _temp1474)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL1475; _LL1475: if( topt !=
0){ void* _temp1476= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp1482; struct Cyc_Absyn_Conref* _temp1484; struct
Cyc_Absyn_Tqual _temp1486; struct Cyc_Absyn_Conref* _temp1488; void* _temp1490;
void* _temp1492; _LL1478: if(( unsigned int) _temp1476 > 4u?*(( int*) _temp1476)
== Cyc_Absyn_PointerType: 0){ _LL1483: _temp1482=(( struct Cyc_Absyn_PointerType_struct*)
_temp1476)->f1; _LL1493: _temp1492=( void*) _temp1482.elt_typ; goto _LL1491;
_LL1491: _temp1490=( void*) _temp1482.rgn_typ; goto _LL1489; _LL1489: _temp1488=
_temp1482.nullable; goto _LL1487; _LL1487: _temp1486= _temp1482.tq; goto _LL1485;
_LL1485: _temp1484= _temp1482.bounds; goto _LL1479;} else{ goto _LL1480;}
_LL1480: goto _LL1481; _LL1479: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1488)){ _temp1474= Cyc_Absyn_star_typ;} goto _LL1477; _LL1481: goto _LL1477;
_LL1477:;} return _temp1474( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1494=( void*) s->r; struct Cyc_Absyn_Exp* _temp1504;
struct Cyc_Absyn_Stmt* _temp1506; struct Cyc_Absyn_Stmt* _temp1508; struct Cyc_Absyn_Stmt*
_temp1510; struct Cyc_Absyn_Decl* _temp1512; _LL1496: if(( unsigned int)
_temp1494 > 1u?*(( int*) _temp1494) == Cyc_Absyn_Exp_s: 0){ _LL1505: _temp1504=((
struct Cyc_Absyn_Exp_s_struct*) _temp1494)->f1; goto _LL1497;} else{ goto
_LL1498;} _LL1498: if(( unsigned int) _temp1494 > 1u?*(( int*) _temp1494) == Cyc_Absyn_Seq_s:
0){ _LL1509: _temp1508=(( struct Cyc_Absyn_Seq_s_struct*) _temp1494)->f1; goto
_LL1507; _LL1507: _temp1506=(( struct Cyc_Absyn_Seq_s_struct*) _temp1494)->f2;
goto _LL1499;} else{ goto _LL1500;} _LL1500: if(( unsigned int) _temp1494 > 1u?*((
int*) _temp1494) == Cyc_Absyn_Decl_s: 0){ _LL1513: _temp1512=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1494)->f1; goto _LL1511; _LL1511: _temp1510=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1494)->f2; goto _LL1501;} else{ goto _LL1502;} _LL1502: goto _LL1503;
_LL1497: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1504->topt))->v;
_LL1499: s= _temp1506; continue; _LL1501: s= _temp1510; continue; _LL1503:
return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp1514=( unsigned char*)"statement expression must end with expression";
struct _tagged_string _temp1515; _temp1515.curr= _temp1514; _temp1515.base=
_temp1514; _temp1515.last_plus_one= _temp1514 + 46; _temp1515;})); _LL1495:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,({ unsigned char* _temp1516=( unsigned char*)"tcCodegen"; struct
_tagged_string _temp1517; _temp1517.curr= _temp1516; _temp1517.base= _temp1516;
_temp1517.last_plus_one= _temp1516 + 10; _temp1517;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char*
_temp1518=( unsigned char*)"tcFill"; struct _tagged_string _temp1519; _temp1519.curr=
_temp1518; _temp1519.base= _temp1518; _temp1519.last_plus_one= _temp1518 + 7;
_temp1519;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1529=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1529[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1530; _temp1530.tag= Cyc_Absyn_RgnHandleType; _temp1530.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1531=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1531->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1531;})); _temp1530;}); _temp1529;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1520= Cyc_Tcutil_compress( handle_type);
void* _temp1526; _LL1522: if(( unsigned int) _temp1520 > 4u?*(( int*) _temp1520)
== Cyc_Absyn_RgnHandleType: 0){ _LL1527: _temp1526=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1520)->f1; goto _LL1523;} else{ goto _LL1524;} _LL1524: goto _LL1525;
_LL1523: rgn= _temp1526; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1521; _LL1525: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_string)({ struct _tagged_string _temp1528=
Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1528.last_plus_one - _temp1528.curr, _temp1528.curr);})); goto _LL1521;
_LL1521:;}{ void* _temp1532=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1544;
struct Cyc_Absyn_Exp* _temp1546; struct Cyc_Absyn_Vardecl* _temp1548; struct Cyc_List_List*
_temp1550; struct Cyc_Core_Opt* _temp1552; struct Cyc_List_List* _temp1554; void*
_temp1556; struct _tagged_string _temp1558; _LL1534: if(*(( int*) _temp1532) ==
Cyc_Absyn_Comprehension_e){ _LL1549: _temp1548=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1532)->f1; goto _LL1547; _LL1547: _temp1546=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1532)->f2; goto _LL1545; _LL1545: _temp1544=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1532)->f3; goto _LL1535;} else{ goto _LL1536;} _LL1536: if(*(( int*)
_temp1532) == Cyc_Absyn_UnresolvedMem_e){ _LL1553: _temp1552=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1532)->f1; goto _LL1551; _LL1551: _temp1550=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1532)->f2; goto _LL1537;} else{ goto _LL1538;} _LL1538: if(*(( int*)
_temp1532) == Cyc_Absyn_Array_e){ _LL1555: _temp1554=(( struct Cyc_Absyn_Array_e_struct*)
_temp1532)->f1; goto _LL1539;} else{ goto _LL1540;} _LL1540: if(*(( int*)
_temp1532) == Cyc_Absyn_Const_e){ _LL1557: _temp1556=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1532)->f1; if(( unsigned int) _temp1556 > 1u?*(( int*) _temp1556) == Cyc_Absyn_String_c:
0){ _LL1559: _temp1558=(( struct Cyc_Absyn_String_c_struct*) _temp1556)->f1;
goto _LL1541;} else{ goto _LL1542;}} else{ goto _LL1542;} _LL1542: goto _LL1543;
_LL1535: { void* _temp1560= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); goto
_LL1561; _LL1561: { void* _temp1562= Cyc_Tcutil_compress( _temp1560); struct Cyc_Absyn_Exp*
_temp1568; struct Cyc_Absyn_Tqual _temp1570; void* _temp1572; _LL1564: if((
unsigned int) _temp1562 > 4u?*(( int*) _temp1562) == Cyc_Absyn_ArrayType: 0){
_LL1573: _temp1572=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1562)->f1;
goto _LL1571; _LL1571: _temp1570=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1562)->f2; goto _LL1569; _LL1569: _temp1568=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1562)->f3; goto _LL1565;} else{ goto _LL1566;} _LL1566: goto _LL1567;
_LL1565: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1546)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1578=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1578[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1579; _temp1579.tag= Cyc_Absyn_Upper_b; _temp1579.f1=
_temp1546; _temp1579;}); _temp1578;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1575=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1575[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1576; _temp1576.tag= Cyc_Absyn_PointerType;
_temp1576.f1=({ struct Cyc_Absyn_PtrInfo _temp1577; _temp1577.elt_typ=( void*)
_temp1572; _temp1577.rgn_typ=( void*) rgn; _temp1577.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1577.tq= _temp1570; _temp1577.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1577;});
_temp1576;}); _temp1575;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1574=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1574->v=(
void*) res_typ; _temp1574;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1567: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1580=( unsigned char*)"tcNew: comprehension returned non-array type";
struct _tagged_string _temp1581; _temp1581.curr= _temp1580; _temp1581.base=
_temp1580; _temp1581.last_plus_one= _temp1580 + 45; _temp1581;})); _LL1563:;}}
_LL1537:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1582=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1582[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1583; _temp1583.tag= Cyc_Absyn_Array_e;
_temp1583.f1= _temp1550; _temp1583;}); _temp1582;}))); _temp1554= _temp1550;
goto _LL1539; _LL1539: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1584=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1590; struct Cyc_Absyn_Conref* _temp1592; struct Cyc_Absyn_Tqual _temp1594;
struct Cyc_Absyn_Conref* _temp1596; void* _temp1598; void* _temp1600; void**
_temp1602; _LL1586: if(( unsigned int) _temp1584 > 4u?*(( int*) _temp1584) ==
Cyc_Absyn_PointerType: 0){ _LL1591: _temp1590=(( struct Cyc_Absyn_PointerType_struct*)
_temp1584)->f1; _LL1601: _temp1600=( void*) _temp1590.elt_typ; _temp1602=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1584)->f1).elt_typ; goto _LL1599;
_LL1599: _temp1598=( void*) _temp1590.rgn_typ; goto _LL1597; _LL1597: _temp1596=
_temp1590.nullable; goto _LL1595; _LL1595: _temp1594= _temp1590.tq; goto _LL1593;
_LL1593: _temp1592= _temp1590.bounds; goto _LL1587;} else{ goto _LL1588;}
_LL1588: goto _LL1589; _LL1587: elt_typ_opt=( void**) _temp1602; goto _LL1585;
_LL1589: goto _LL1585; _LL1585:;}{ void* _temp1603= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); goto _LL1604; _LL1604: { void* res_typ;{ void* _temp1605= Cyc_Tcutil_compress(
_temp1603); struct Cyc_Absyn_Exp* _temp1611; struct Cyc_Absyn_Tqual _temp1613;
void* _temp1615; _LL1607: if(( unsigned int) _temp1605 > 4u?*(( int*) _temp1605)
== Cyc_Absyn_ArrayType: 0){ _LL1616: _temp1615=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1605)->f1; goto _LL1614; _LL1614: _temp1613=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1605)->f2; goto _LL1612; _LL1612: _temp1611=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1605)->f3; goto _LL1608;} else{ goto _LL1609;} _LL1609: goto _LL1610;
_LL1608: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1617=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1617[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1618; _temp1618.tag=
Cyc_Absyn_PointerType; _temp1618.f1=({ struct Cyc_Absyn_PtrInfo _temp1619;
_temp1619.elt_typ=( void*) _temp1615; _temp1619.rgn_typ=( void*) rgn; _temp1619.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1619.tq=
_temp1613; _temp1619.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1620=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1620[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1621; _temp1621.tag= Cyc_Absyn_Upper_b; _temp1621.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1611); _temp1621;}); _temp1620;}));
_temp1619;}); _temp1618;}); _temp1617;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1622=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1622->v=( void*) res_typ; _temp1622;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1606; _LL1610: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1623=( unsigned char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1624; _temp1624.curr= _temp1623; _temp1624.base=
_temp1623; _temp1624.last_plus_one= _temp1623 + 50; _temp1624;})); _LL1606:;}
return res_typ;}}} _LL1541: { void* _temp1625= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); goto _LL1626;
_LL1626: { void* _temp1627= Cyc_Tcexp_tcExp( te,( void**)& _temp1625, e1); goto
_LL1628; _LL1628: return Cyc_Absyn_atb_typ( _temp1627, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1629=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1629[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1630; _temp1630.tag= Cyc_Absyn_Upper_b; _temp1630.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1630;}); _temp1629;}));}} _LL1543: { void**
topt2= 0; if( topt != 0){ void* _temp1631= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1639; struct Cyc_Absyn_Conref*
_temp1641; struct Cyc_Absyn_Tqual _temp1643; struct Cyc_Absyn_Conref* _temp1645;
void* _temp1647; void* _temp1649; void** _temp1651; struct Cyc_Absyn_TunionInfo
_temp1652; _LL1633: if(( unsigned int) _temp1631 > 4u?*(( int*) _temp1631) ==
Cyc_Absyn_PointerType: 0){ _LL1640: _temp1639=(( struct Cyc_Absyn_PointerType_struct*)
_temp1631)->f1; _LL1650: _temp1649=( void*) _temp1639.elt_typ; _temp1651=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1631)->f1).elt_typ; goto _LL1648;
_LL1648: _temp1647=( void*) _temp1639.rgn_typ; goto _LL1646; _LL1646: _temp1645=
_temp1639.nullable; goto _LL1644; _LL1644: _temp1643= _temp1639.tq; goto _LL1642;
_LL1642: _temp1641= _temp1639.bounds; goto _LL1634;} else{ goto _LL1635;}
_LL1635: if(( unsigned int) _temp1631 > 4u?*(( int*) _temp1631) == Cyc_Absyn_TunionType:
0){ _LL1653: _temp1652=(( struct Cyc_Absyn_TunionType_struct*) _temp1631)->f1;
goto _LL1636;} else{ goto _LL1637;} _LL1637: goto _LL1638; _LL1634: topt2=( void**)
_temp1651; goto _LL1632; _LL1636: topt2=({ void** _temp1654=( void**) GC_malloc(
sizeof( void*)); _temp1654[ 0]=*(( void**) _check_null( topt)); _temp1654;});
goto _LL1632; _LL1638: goto _LL1632; _LL1632:;}{ void* _temp1655= Cyc_Tcexp_tcExp(
te, topt2, e1); goto _LL1656; _LL1656: { void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1658=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1658[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1659; _temp1659.tag=
Cyc_Absyn_PointerType; _temp1659.f1=({ struct Cyc_Absyn_PtrInfo _temp1660;
_temp1660.elt_typ=( void*) _temp1655; _temp1660.rgn_typ=( void*) rgn; _temp1660.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1660.tq= Cyc_Absyn_empty_tqual();
_temp1660.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1661=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1661[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1662; _temp1662.tag= Cyc_Absyn_Upper_b; _temp1662.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1662;}); _temp1661;})); _temp1660;}); _temp1659;});
_temp1658;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1657=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1657->v=( void*) res_typ;
_temp1657;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}}} _LL1533:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1663= t; struct Cyc_Absyn_Exp* _temp1669; struct Cyc_Absyn_Tqual
_temp1671; void* _temp1673; _LL1665: if(( unsigned int) _temp1663 > 4u?*(( int*)
_temp1663) == Cyc_Absyn_ArrayType: 0){ _LL1674: _temp1673=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1663)->f1; goto _LL1672; _LL1672: _temp1671=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1663)->f2; goto _LL1670; _LL1670: _temp1669=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1663)->f3; goto _LL1666;} else{ goto _LL1667;} _LL1667: goto _LL1668;
_LL1666: { void* _temp1677; int _temp1679; struct _tuple6 _temp1675= Cyc_Tcutil_addressof_props(
te, e); _LL1680: _temp1679= _temp1675.f1; goto _LL1678; _LL1678: _temp1677=
_temp1675.f2; goto _LL1676; _LL1676: { void* _temp1683= _temp1669 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1681=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1681[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1682; _temp1682.tag= Cyc_Absyn_Upper_b;
_temp1682.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1669); _temp1682;});
_temp1681;}); goto _LL1684; _LL1684: t= Cyc_Absyn_atb_typ( _temp1673, _temp1677,
_temp1671, _temp1683);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=(
void*) t); return t;}} _LL1668: return t; _LL1664:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1685=( void*) e->r; struct Cyc_List_List* _temp1695;
struct Cyc_Absyn_Exp* _temp1697; struct Cyc_Absyn_Exp* _temp1699; struct Cyc_Absyn_Vardecl*
_temp1701; void* _temp1703; struct _tagged_string _temp1705; _LL1687: if(*(( int*)
_temp1685) == Cyc_Absyn_Array_e){ _LL1696: _temp1695=(( struct Cyc_Absyn_Array_e_struct*)
_temp1685)->f1; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(*(( int*)
_temp1685) == Cyc_Absyn_Comprehension_e){ _LL1702: _temp1701=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1685)->f1; goto _LL1700; _LL1700: _temp1699=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1685)->f2; goto _LL1698; _LL1698: _temp1697=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1685)->f3; goto _LL1690;} else{ goto _LL1691;} _LL1691: if(*(( int*)
_temp1685) == Cyc_Absyn_Const_e){ _LL1704: _temp1703=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1685)->f1; if(( unsigned int) _temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_String_c:
0){ _LL1706: _temp1705=(( struct Cyc_Absyn_String_c_struct*) _temp1703)->f1;
goto _LL1692;} else{ goto _LL1693;}} else{ goto _LL1693;} _LL1693: goto _LL1694;
_LL1688: return t; _LL1690: return t; _LL1692: return t; _LL1694: t= Cyc_Tcutil_compress(
t);{ void* _temp1707= t; struct Cyc_Absyn_Exp* _temp1713; struct Cyc_Absyn_Tqual
_temp1715; void* _temp1717; _LL1709: if(( unsigned int) _temp1707 > 4u?*(( int*)
_temp1707) == Cyc_Absyn_ArrayType: 0){ _LL1718: _temp1717=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1707)->f1; goto _LL1716; _LL1716: _temp1715=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1707)->f2; goto _LL1714; _LL1714: _temp1713=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1707)->f3; goto _LL1710;} else{ goto _LL1711;} _LL1711: goto _LL1712;
_LL1710: { void* _temp1721; int _temp1723; struct _tuple6 _temp1719= Cyc_Tcutil_addressof_props(
te, e); _LL1724: _temp1723= _temp1719.f1; goto _LL1722; _LL1722: _temp1721=
_temp1719.f2; goto _LL1720; _LL1720: { void* b= _temp1713 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1725=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1725[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1726; _temp1726.tag= Cyc_Absyn_Upper_b; _temp1726.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1713); _temp1726;}); _temp1725;}); t=
Cyc_Absyn_atb_typ( _temp1717, _temp1721, _temp1715, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1712: return t; _LL1708:;} _LL1686:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1727=( void*) e->r; struct Cyc_Absyn_Exp* _temp1733; _LL1729: if(*(( int*)
_temp1727) == Cyc_Absyn_NoInstantiate_e){ _LL1734: _temp1733=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1727)->f1; goto _LL1730;} else{ goto _LL1731;} _LL1731: goto _LL1732;
_LL1730: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1733);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1733->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1733->topt))->v))); e->topt=
_temp1733->topt; goto _LL1728; _LL1732: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1735= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1741; struct Cyc_Absyn_Conref*
_temp1743; struct Cyc_Absyn_Tqual _temp1745; struct Cyc_Absyn_Conref* _temp1747;
void* _temp1749; void* _temp1751; _LL1737: if(( unsigned int) _temp1735 > 4u?*((
int*) _temp1735) == Cyc_Absyn_PointerType: 0){ _LL1742: _temp1741=(( struct Cyc_Absyn_PointerType_struct*)
_temp1735)->f1; _LL1752: _temp1751=( void*) _temp1741.elt_typ; goto _LL1750;
_LL1750: _temp1749=( void*) _temp1741.rgn_typ; goto _LL1748; _LL1748: _temp1747=
_temp1741.nullable; goto _LL1746; _LL1746: _temp1745= _temp1741.tq; goto _LL1744;
_LL1744: _temp1743= _temp1741.bounds; goto _LL1738;} else{ goto _LL1739;}
_LL1739: goto _LL1740; _LL1738:{ void* _temp1753= Cyc_Tcutil_compress( _temp1751);
struct Cyc_Absyn_FnInfo _temp1759; struct Cyc_List_List* _temp1761; struct Cyc_Absyn_VarargInfo*
_temp1763; int _temp1765; struct Cyc_List_List* _temp1767; void* _temp1769;
struct Cyc_Core_Opt* _temp1771; struct Cyc_List_List* _temp1773; _LL1755: if((
unsigned int) _temp1753 > 4u?*(( int*) _temp1753) == Cyc_Absyn_FnType: 0){
_LL1760: _temp1759=(( struct Cyc_Absyn_FnType_struct*) _temp1753)->f1; _LL1774:
_temp1773= _temp1759.tvars; goto _LL1772; _LL1772: _temp1771= _temp1759.effect;
goto _LL1770; _LL1770: _temp1769=( void*) _temp1759.ret_typ; goto _LL1768;
_LL1768: _temp1767= _temp1759.args; goto _LL1766; _LL1766: _temp1765= _temp1759.c_varargs;
goto _LL1764; _LL1764: _temp1763= _temp1759.cyc_varargs; goto _LL1762; _LL1762:
_temp1761= _temp1759.attributes; goto _LL1756;} else{ goto _LL1757;} _LL1757:
goto _LL1758; _LL1756: if( _temp1773 != 0){ struct _RegionHandle _temp1775=
_new_region(); struct _RegionHandle* rgn=& _temp1775; _push_region( rgn);{
struct _tuple4 _temp1777=({ struct _tuple4 _temp1776; _temp1776.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1776.f2= rgn; _temp1776;}); goto _LL1778; _LL1778: { struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)(
struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1777, _temp1773);
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1785=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1785[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1786; _temp1786.tag= Cyc_Absyn_FnType;
_temp1786.f1=({ struct Cyc_Absyn_FnInfo _temp1787; _temp1787.tvars= 0; _temp1787.effect=
_temp1771; _temp1787.ret_typ=( void*) _temp1769; _temp1787.args= _temp1767;
_temp1787.c_varargs= _temp1765; _temp1787.cyc_varargs= _temp1763; _temp1787.attributes=
_temp1761; _temp1787;}); _temp1786;}); _temp1785;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1782=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1782[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1783; _temp1783.tag= Cyc_Absyn_PointerType;
_temp1783.f1=({ struct Cyc_Absyn_PtrInfo _temp1784; _temp1784.elt_typ=( void*)
ftyp; _temp1784.rgn_typ=( void*) _temp1749; _temp1784.nullable= _temp1747;
_temp1784.tq= _temp1745; _temp1784.bounds= _temp1743; _temp1784;}); _temp1783;});
_temp1782;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1779=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1779[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1780; _temp1780.tag= Cyc_Absyn_Instantiate_e;
_temp1780.f1= inner; _temp1780.f2= ts; _temp1780;}); _temp1779;}))); e->topt=({
struct Cyc_Core_Opt* _temp1781=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1781->v=( void*) new_typ; _temp1781;});}}; _pop_region(&
_temp1775);} goto _LL1754; _LL1758: goto _LL1754; _LL1754:;} goto _LL1736;
_LL1740: goto _LL1736; _LL1736:;} goto _LL1728; _LL1728:;} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1788=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1860; struct _tuple1* _temp1862; struct Cyc_List_List* _temp1864; struct
Cyc_Absyn_Exp* _temp1866; struct Cyc_List_List* _temp1868; struct Cyc_Core_Opt*
_temp1870; void* _temp1872; void* _temp1874; struct _tuple1* _temp1876; struct
Cyc_List_List* _temp1878; void* _temp1880; void* _temp1882; struct Cyc_Absyn_Exp*
_temp1884; struct Cyc_Absyn_Exp* _temp1886; struct Cyc_Core_Opt* _temp1888;
struct Cyc_Absyn_Exp* _temp1890; struct Cyc_Absyn_Exp* _temp1892; struct Cyc_Absyn_Exp*
_temp1894; struct Cyc_Absyn_Exp* _temp1896; struct Cyc_Absyn_Exp* _temp1898;
struct Cyc_Absyn_Exp* _temp1900; struct Cyc_Absyn_VarargCallInfo* _temp1902;
struct Cyc_Absyn_VarargCallInfo** _temp1904; struct Cyc_List_List* _temp1905;
struct Cyc_Absyn_Exp* _temp1907; struct Cyc_Absyn_Exp* _temp1909; struct Cyc_List_List*
_temp1911; struct Cyc_Absyn_Exp* _temp1913; struct Cyc_Absyn_Exp* _temp1915;
void* _temp1917; struct Cyc_Absyn_Exp* _temp1919; struct Cyc_Absyn_Exp*
_temp1921; struct Cyc_Absyn_Exp* _temp1923; struct Cyc_Absyn_Exp* _temp1925;
void* _temp1927; struct Cyc_Absyn_Exp* _temp1929; struct _tagged_string*
_temp1931; struct Cyc_Absyn_Exp* _temp1933; struct _tagged_string* _temp1935;
struct Cyc_Absyn_Exp* _temp1937; struct Cyc_Absyn_Exp* _temp1939; struct Cyc_Absyn_Exp*
_temp1941; struct Cyc_List_List* _temp1943; struct Cyc_List_List* _temp1945;
struct _tuple2* _temp1947; struct Cyc_List_List* _temp1949; struct Cyc_Absyn_Stmt*
_temp1951; struct Cyc_Absyn_Fndecl* _temp1953; struct Cyc_Absyn_Exp* _temp1955;
struct Cyc_Absyn_Exp* _temp1957; struct Cyc_Absyn_Exp* _temp1959; struct Cyc_Absyn_Vardecl*
_temp1961; struct Cyc_Absyn_Structdecl* _temp1963; struct Cyc_Absyn_Structdecl**
_temp1965; struct Cyc_List_List* _temp1966; struct Cyc_Core_Opt* _temp1968;
struct Cyc_Core_Opt** _temp1970; struct _tuple1* _temp1971; struct _tuple1**
_temp1973; struct Cyc_List_List* _temp1974; void* _temp1976; struct Cyc_Absyn_Tunionfield*
_temp1978; struct Cyc_Absyn_Tuniondecl* _temp1980; struct Cyc_List_List*
_temp1982; struct Cyc_Core_Opt* _temp1984; struct Cyc_Core_Opt** _temp1986;
struct Cyc_Core_Opt* _temp1987; struct Cyc_Core_Opt** _temp1989; struct Cyc_Absyn_Enumfield*
_temp1990; struct Cyc_Absyn_Enumdecl* _temp1992; struct _tuple1* _temp1994;
struct _tuple1** _temp1996; void* _temp1997; struct Cyc_Absyn_Exp* _temp1999;
_LL1790: if(*(( int*) _temp1788) == Cyc_Absyn_NoInstantiate_e){ _LL1861:
_temp1860=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1788)->f1; goto
_LL1791;} else{ goto _LL1792;} _LL1792: if(*(( int*) _temp1788) == Cyc_Absyn_UnknownId_e){
_LL1863: _temp1862=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1788)->f1; goto
_LL1793;} else{ goto _LL1794;} _LL1794: if(*(( int*) _temp1788) == Cyc_Absyn_UnknownCall_e){
_LL1867: _temp1866=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp1788)->f1;
goto _LL1865; _LL1865: _temp1864=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1788)->f2; goto _LL1795;} else{ goto _LL1796;} _LL1796: if(*(( int*)
_temp1788) == Cyc_Absyn_UnresolvedMem_e){ _LL1871: _temp1870=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1788)->f1; goto _LL1869; _LL1869: _temp1868=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1788)->f2; goto _LL1797;} else{ goto _LL1798;} _LL1798: if(*(( int*)
_temp1788) == Cyc_Absyn_Const_e){ _LL1873: _temp1872=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1788)->f1; goto _LL1799;} else{ goto _LL1800;} _LL1800: if(*(( int*)
_temp1788) == Cyc_Absyn_Var_e){ _LL1877: _temp1876=(( struct Cyc_Absyn_Var_e_struct*)
_temp1788)->f1; goto _LL1875; _LL1875: _temp1874=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1788)->f2; goto _LL1801;} else{ goto _LL1802;} _LL1802: if(*(( int*)
_temp1788) == Cyc_Absyn_Primop_e){ _LL1881: _temp1880=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1788)->f1; goto _LL1879; _LL1879: _temp1878=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1788)->f2; goto _LL1803;} else{ goto _LL1804;} _LL1804: if(*(( int*)
_temp1788) == Cyc_Absyn_Increment_e){ _LL1885: _temp1884=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1788)->f1; goto _LL1883; _LL1883: _temp1882=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1788)->f2; goto _LL1805;} else{ goto _LL1806;} _LL1806: if(*(( int*)
_temp1788) == Cyc_Absyn_AssignOp_e){ _LL1891: _temp1890=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1788)->f1; goto _LL1889; _LL1889: _temp1888=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1788)->f2; goto _LL1887; _LL1887: _temp1886=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1788)->f3; goto _LL1807;} else{ goto _LL1808;} _LL1808: if(*(( int*)
_temp1788) == Cyc_Absyn_Conditional_e){ _LL1897: _temp1896=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1788)->f1; goto _LL1895; _LL1895: _temp1894=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1788)->f2; goto _LL1893; _LL1893: _temp1892=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1788)->f3; goto _LL1809;} else{ goto _LL1810;} _LL1810: if(*(( int*)
_temp1788) == Cyc_Absyn_SeqExp_e){ _LL1901: _temp1900=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1788)->f1; goto _LL1899; _LL1899: _temp1898=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1788)->f2; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(*(( int*)
_temp1788) == Cyc_Absyn_FnCall_e){ _LL1908: _temp1907=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1788)->f1; goto _LL1906; _LL1906: _temp1905=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1788)->f2; goto _LL1903; _LL1903: _temp1902=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1788)->f3; _temp1904=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1788)->f3;
goto _LL1813;} else{ goto _LL1814;} _LL1814: if(*(( int*) _temp1788) == Cyc_Absyn_Throw_e){
_LL1910: _temp1909=(( struct Cyc_Absyn_Throw_e_struct*) _temp1788)->f1; goto
_LL1815;} else{ goto _LL1816;} _LL1816: if(*(( int*) _temp1788) == Cyc_Absyn_Instantiate_e){
_LL1914: _temp1913=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1788)->f1;
goto _LL1912; _LL1912: _temp1911=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1788)->f2; goto _LL1817;} else{ goto _LL1818;} _LL1818: if(*(( int*)
_temp1788) == Cyc_Absyn_Cast_e){ _LL1918: _temp1917=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1788)->f1; goto _LL1916; _LL1916: _temp1915=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1788)->f2; goto _LL1819;} else{ goto _LL1820;} _LL1820: if(*(( int*)
_temp1788) == Cyc_Absyn_Address_e){ _LL1920: _temp1919=(( struct Cyc_Absyn_Address_e_struct*)
_temp1788)->f1; goto _LL1821;} else{ goto _LL1822;} _LL1822: if(*(( int*)
_temp1788) == Cyc_Absyn_New_e){ _LL1924: _temp1923=(( struct Cyc_Absyn_New_e_struct*)
_temp1788)->f1; goto _LL1922; _LL1922: _temp1921=(( struct Cyc_Absyn_New_e_struct*)
_temp1788)->f2; goto _LL1823;} else{ goto _LL1824;} _LL1824: if(*(( int*)
_temp1788) == Cyc_Absyn_Sizeofexp_e){ _LL1926: _temp1925=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1788)->f1; goto _LL1825;} else{ goto _LL1826;} _LL1826: if(*(( int*)
_temp1788) == Cyc_Absyn_Sizeoftyp_e){ _LL1928: _temp1927=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1788)->f1; goto _LL1827;} else{ goto _LL1828;} _LL1828: if(*(( int*)
_temp1788) == Cyc_Absyn_Deref_e){ _LL1930: _temp1929=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1788)->f1; goto _LL1829;} else{ goto _LL1830;} _LL1830: if(*(( int*)
_temp1788) == Cyc_Absyn_StructMember_e){ _LL1934: _temp1933=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1788)->f1; goto _LL1932; _LL1932: _temp1931=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1788)->f2; goto _LL1831;} else{ goto _LL1832;} _LL1832: if(*(( int*)
_temp1788) == Cyc_Absyn_StructArrow_e){ _LL1938: _temp1937=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1788)->f1; goto _LL1936; _LL1936: _temp1935=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1788)->f2; goto _LL1833;} else{ goto _LL1834;} _LL1834: if(*(( int*)
_temp1788) == Cyc_Absyn_Subscript_e){ _LL1942: _temp1941=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1788)->f1; goto _LL1940; _LL1940: _temp1939=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1788)->f2; goto _LL1835;} else{ goto _LL1836;} _LL1836: if(*(( int*)
_temp1788) == Cyc_Absyn_Tuple_e){ _LL1944: _temp1943=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1788)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: if(*(( int*)
_temp1788) == Cyc_Absyn_CompoundLit_e){ _LL1948: _temp1947=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1788)->f1; goto _LL1946; _LL1946: _temp1945=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1788)->f2; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1788) == Cyc_Absyn_Array_e){ _LL1950: _temp1949=(( struct Cyc_Absyn_Array_e_struct*)
_temp1788)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: if(*(( int*)
_temp1788) == Cyc_Absyn_StmtExp_e){ _LL1952: _temp1951=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1788)->f1; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(*(( int*)
_temp1788) == Cyc_Absyn_Codegen_e){ _LL1954: _temp1953=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1788)->f1; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(*(( int*)
_temp1788) == Cyc_Absyn_Fill_e){ _LL1956: _temp1955=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1788)->f1; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(*(( int*)
_temp1788) == Cyc_Absyn_Comprehension_e){ _LL1962: _temp1961=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1788)->f1; goto _LL1960; _LL1960: _temp1959=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1788)->f2; goto _LL1958; _LL1958: _temp1957=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1788)->f3; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(*(( int*)
_temp1788) == Cyc_Absyn_Struct_e){ _LL1972: _temp1971=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1788)->f1; _temp1973=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1788)->f1;
goto _LL1969; _LL1969: _temp1968=(( struct Cyc_Absyn_Struct_e_struct*) _temp1788)->f2;
_temp1970=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1788)->f2; goto _LL1967;
_LL1967: _temp1966=(( struct Cyc_Absyn_Struct_e_struct*) _temp1788)->f3; goto
_LL1964; _LL1964: _temp1963=(( struct Cyc_Absyn_Struct_e_struct*) _temp1788)->f4;
_temp1965=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1788)->f4; goto _LL1851;}
else{ goto _LL1852;} _LL1852: if(*(( int*) _temp1788) == Cyc_Absyn_AnonStruct_e){
_LL1977: _temp1976=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1788)->f1;
goto _LL1975; _LL1975: _temp1974=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1788)->f2; goto _LL1853;} else{ goto _LL1854;} _LL1854: if(*(( int*)
_temp1788) == Cyc_Absyn_Tunion_e){ _LL1988: _temp1987=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1788)->f1; _temp1989=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1788)->f1;
goto _LL1985; _LL1985: _temp1984=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1788)->f2;
_temp1986=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1788)->f2; goto _LL1983;
_LL1983: _temp1982=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1788)->f3; goto
_LL1981; _LL1981: _temp1980=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1788)->f4;
goto _LL1979; _LL1979: _temp1978=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1788)->f5;
goto _LL1855;} else{ goto _LL1856;} _LL1856: if(*(( int*) _temp1788) == Cyc_Absyn_Enum_e){
_LL1995: _temp1994=(( struct Cyc_Absyn_Enum_e_struct*) _temp1788)->f1; _temp1996=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1788)->f1; goto _LL1993; _LL1993:
_temp1992=(( struct Cyc_Absyn_Enum_e_struct*) _temp1788)->f2; goto _LL1991;
_LL1991: _temp1990=(( struct Cyc_Absyn_Enum_e_struct*) _temp1788)->f3; goto
_LL1857;} else{ goto _LL1858;} _LL1858: if(*(( int*) _temp1788) == Cyc_Absyn_Malloc_e){
_LL2000: _temp1999=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1788)->f1; goto
_LL1998; _LL1998: _temp1997=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1788)->f2; goto _LL1859;} else{ goto _LL1789;} _LL1791: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1860); return; _LL1793: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1862); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1795: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1866, _temp1864); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1797: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1868); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1799: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1872, e);
goto _LL1789; _LL1801: t= Cyc_Tcexp_tcVar( te, loc, _temp1876, _temp1874); goto
_LL1789; _LL1803: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1880, _temp1878);
goto _LL1789; _LL1805: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1884,
_temp1882); goto _LL1789; _LL1807: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1890, _temp1888, _temp1886); goto _LL1789; _LL1809: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1896, _temp1894, _temp1892); goto _LL1789; _LL1811: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1900, _temp1898); goto _LL1789; _LL1813: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1907, _temp1905, _temp1904); goto _LL1789; _LL1815: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1909); goto _LL1789; _LL1817: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1913, _temp1911); goto _LL1789; _LL1819: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1917, _temp1915); goto _LL1789; _LL1821: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1919); goto _LL1789; _LL1823: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1923, e, _temp1921); goto _LL1789; _LL1825: { void* _temp2001= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1925); goto _LL2002; _LL2002: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp2001); goto _LL1789;} _LL1827: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1927); goto _LL1789; _LL1829: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp1929); goto _LL1789; _LL1831: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp1933, _temp1931); goto _LL1789; _LL1833: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp1937, _temp1935); goto _LL1789; _LL1835: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1941, _temp1939); goto _LL1789; _LL1837: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1943); goto _LL1789; _LL1839: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1947, _temp1945); goto _LL1789; _LL1841: { void** elt_topt= 0;
if( topt != 0){ void* _temp2003= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp2009; struct Cyc_Absyn_Tqual _temp2011; void*
_temp2013; void** _temp2015; _LL2005: if(( unsigned int) _temp2003 > 4u?*(( int*)
_temp2003) == Cyc_Absyn_ArrayType: 0){ _LL2014: _temp2013=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2003)->f1; _temp2015=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2003)->f1); goto _LL2012; _LL2012: _temp2011=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2003)->f2; goto _LL2010; _LL2010: _temp2009=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2003)->f3; goto _LL2006;} else{ goto _LL2007;} _LL2007: goto _LL2008;
_LL2006: elt_topt=( void**) _temp2015; goto _LL2004; _LL2008: goto _LL2004;
_LL2004:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1949); goto _LL1789;}
_LL1843: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1951); goto _LL1789;
_LL1845: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1953); goto _LL1789;
_LL1847: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1955); goto _LL1789; _LL1849:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1961, _temp1959, _temp1957);
goto _LL1789; _LL1851: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1973,
_temp1970, _temp1966, _temp1965); goto _LL1789; _LL1853: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1976, _temp1974); goto _LL1789; _LL1855: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1989, _temp1986, _temp1982, _temp1980, _temp1978); goto
_LL1789; _LL1857:* _temp1996=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1990))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp2016=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2016[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2017; _temp2017.tag= Cyc_Absyn_EnumType;
_temp2017.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1992))->name;
_temp2017.f2= _temp1992; _temp2017;}); _temp2016;}); goto _LL1789; _LL1859: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1999, _temp1997); goto _LL1789; _LL1789:;}
e->topt=({ struct Cyc_Core_Opt* _temp2018=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2018->v=( void*) t; _temp2018;});}