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
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
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
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
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
struct Cyc_Absyn_Fndecl*); struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2;
} ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct
Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple5{ int f1; void* f2; } ; extern struct _tuple5 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); struct Cyc_CfFlowInfo_Place;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; static const int Cyc_CfFlowInfo_TupleF=
1; struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc=
0; static const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_UnknownIS= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfFlowInfo_MustPointTo= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI=
2; struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; void Cyc_NewControlFlow_cf_check( struct
Cyc_List_List* ds); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_string msg){ Cyc_Tcutil_terr(
loc, msg); return Cyc_Absyn_wildtyp();} static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct
_handler_cons _temp0; _push_handler(& _temp0);{ int _temp2= 0; if( setjmp(
_temp0.handler)){ _temp2= 1;} if( ! _temp2){{ void* _temp3= Cyc_Tcenv_lookup_ordinary(
te, e->loc, q); void* _temp13; struct Cyc_Absyn_Enumfield* _temp15; struct Cyc_Absyn_Enumdecl*
_temp17; struct Cyc_Absyn_Tunionfield* _temp19; struct Cyc_Absyn_Tuniondecl*
_temp21; struct Cyc_Absyn_Structdecl* _temp23; _LL5: if(*(( int*) _temp3) == Cyc_Tcenv_VarRes){
_LL14: _temp13=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3)->f1; goto
_LL6;} else{ goto _LL7;} _LL7: if(*(( int*) _temp3) == Cyc_Tcenv_EnumRes){ _LL18:
_temp17=(( struct Cyc_Tcenv_EnumRes_struct*) _temp3)->f1; goto _LL16; _LL16:
_temp15=(( struct Cyc_Tcenv_EnumRes_struct*) _temp3)->f2; goto _LL8;} else{ goto
_LL9;} _LL9: if(*(( int*) _temp3) == Cyc_Tcenv_TunionRes){ _LL22: _temp21=((
struct Cyc_Tcenv_TunionRes_struct*) _temp3)->f1; goto _LL20; _LL20: _temp19=((
struct Cyc_Tcenv_TunionRes_struct*) _temp3)->f2; goto _LL10;} else{ goto _LL11;}
_LL11: if(*(( int*) _temp3) == Cyc_Tcenv_StructRes){ _LL24: _temp23=(( struct
Cyc_Tcenv_StructRes_struct*) _temp3)->f1; goto _LL12;} else{ goto _LL4;} _LL6:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp25=( struct
Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp25[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp26; _temp26.tag= Cyc_Absyn_Var_e;
_temp26.f1= q; _temp26.f2=( void*) _temp13; _temp26;}); _temp25;}))); goto _LL4;
_LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp27=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp27[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp28; _temp28.tag= Cyc_Absyn_Enum_e;
_temp28.f1= q; _temp28.f2=( struct Cyc_Absyn_Enumdecl*) _temp17; _temp28.f3=(
struct Cyc_Absyn_Enumfield*) _temp15; _temp28;}); _temp27;}))); goto _LL4; _LL10:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp29=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp30; _temp30.tag= Cyc_Absyn_Tunion_e;
_temp30.f1= 0; _temp30.f2= 0; _temp30.f3= 0; _temp30.f4= _temp21; _temp30.f5=
_temp19; _temp30;}); _temp29;}))); goto _LL4; _LL12: Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp31= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp31.last_plus_one - _temp31.curr, _temp31.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp32=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp32[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp33; _temp33.tag= Cyc_Absyn_Var_e; _temp33.f1= q; _temp33.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp33;}); _temp32;}))); goto _LL4; _LL4:;};
_pop_handler();} else{ void* _temp1=( void*) _exn_thrown; void* _temp35= _temp1;
_LL37: if( _temp35 == Cyc_Dict_Absent){ goto _LL38;} else{ goto _LL39;} _LL39:
goto _LL40; _LL38:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp41=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp41[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp42; _temp42.tag= Cyc_Absyn_Var_e;
_temp42.f1= q; _temp42.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp42;});
_temp41;}))); goto _LL36; _LL40:( void) _throw( _temp35); _LL36:;}}} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple6* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple6* _temp43=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp43->f1=
0; _temp43->f2= e; _temp43;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp44=( void*) e1->r; struct _tuple1* _temp50; _LL46:
if(*(( int*) _temp44) == Cyc_Absyn_UnknownId_e){ _LL51: _temp50=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp44)->f1; goto _LL47;} else{ goto _LL48;} _LL48: goto _LL49; _LL47: { struct
_handler_cons _temp52; _push_handler(& _temp52);{ int _temp54= 0; if( setjmp(
_temp52.handler)){ _temp54= 1;} if( ! _temp54){{ void* _temp55= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp50); void* _temp65; struct Cyc_Absyn_Tunionfield* _temp67;
struct Cyc_Absyn_Tuniondecl* _temp69; struct Cyc_Absyn_Structdecl* _temp71;
struct Cyc_Absyn_Enumfield* _temp73; struct Cyc_Absyn_Enumdecl* _temp75; _LL57:
if(*(( int*) _temp55) == Cyc_Tcenv_VarRes){ _LL66: _temp65=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp55)->f1; goto _LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp55) ==
Cyc_Tcenv_TunionRes){ _LL70: _temp69=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp55)->f1; goto _LL68; _LL68: _temp67=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp55)->f2; goto _LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp55) ==
Cyc_Tcenv_StructRes){ _LL72: _temp71=(( struct Cyc_Tcenv_StructRes_struct*)
_temp55)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp55) ==
Cyc_Tcenv_EnumRes){ _LL76: _temp75=(( struct Cyc_Tcenv_EnumRes_struct*) _temp55)->f1;
goto _LL74; _LL74: _temp73=(( struct Cyc_Tcenv_EnumRes_struct*) _temp55)->f2;
goto _LL64;} else{ goto _LL56;} _LL58:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_FnCall_e_struct* _temp77=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp77[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp78; _temp78.tag= Cyc_Absyn_FnCall_e; _temp78.f1= e1; _temp78.f2= es;
_temp78.f3= 0; _temp78;}); _temp77;}))); _npop_handler( 0u); return; _LL60: if(
_temp67->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp79=
Cyc_Absynpp_qvar2string( _temp67->name); xprintf("%.*s is a constant, not a function",
_temp79.last_plus_one - _temp79.curr, _temp79.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp80=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp80[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp81; _temp81.tag= Cyc_Absyn_Tunion_e; _temp81.f1= 0; _temp81.f2= 0; _temp81.f3=
es; _temp81.f4= _temp69; _temp81.f5= _temp67; _temp81;}); _temp80;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL62: { struct Cyc_List_List*
_temp82=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); goto
_LL83; _LL83: if( _temp71->name != 0){( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_e_struct* _temp84=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp85; _temp85.tag= Cyc_Absyn_Struct_e; _temp85.f1=( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( _temp71->name))->v; _temp85.f2= 0; _temp85.f3=
_temp82; _temp85.f4=( struct Cyc_Absyn_Structdecl*) _temp71; _temp85;}); _temp84;})));}
else{ Cyc_Tcutil_terr( e->loc,({ unsigned char* _temp86="missing struct name";
struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86;
_temp87.last_plus_one= _temp86 + 20; _temp87;}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp88=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp88[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp89; _temp89.tag= Cyc_Absyn_Struct_e; _temp89.f1= _temp50; _temp89.f2= 0;
_temp89.f3= _temp82; _temp89.f4=( struct Cyc_Absyn_Structdecl*) _temp71; _temp89;});
_temp88;})));} _npop_handler( 0u); return;} _LL64: Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp90= Cyc_Absynpp_qvar2string( _temp50); xprintf("%.*s is an enum constructor, not a function",
_temp90.last_plus_one - _temp90.curr, _temp90.curr);})); _npop_handler( 0u);
return; _LL56:;}; _pop_handler();} else{ void* _temp53=( void*) _exn_thrown;
void* _temp92= _temp53; _LL94: if( _temp92 == Cyc_Dict_Absent){ goto _LL95;}
else{ goto _LL96;} _LL96: goto _LL97; _LL95: Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp98= Cyc_Absynpp_qvar2string( _temp50); xprintf("unbound identifier %.*s",
_temp98.last_plus_one - _temp98.curr, _temp98.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp99=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp99[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp100; _temp100.tag= Cyc_Absyn_FnCall_e; _temp100.f1= e1; _temp100.f2= es;
_temp100.f3= 0; _temp100;}); _temp99;}))); return; _LL97:( void) _throw( _temp92);
_LL93:;}}} _LL49:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp101=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp101[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp102; _temp102.tag= Cyc_Absyn_FnCall_e;
_temp102.f1= e1; _temp102.f2= es; _temp102.f3= 0; _temp102;}); _temp101;})));
return; _LL45:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp103=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp103[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp104; _temp104.tag= Cyc_Absyn_Array_e; _temp104.f1= des; _temp104;});
_temp103;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp105=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp115; struct Cyc_Absyn_Tqual
_temp117; void* _temp119; struct Cyc_Absyn_Structdecl** _temp121; struct Cyc_List_List*
_temp123; struct _tuple1* _temp125; struct Cyc_List_List* _temp127; _LL107: if((
unsigned int) _temp105 > 4u?*(( int*) _temp105) == Cyc_Absyn_ArrayType: 0){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f1;
goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f2;
goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f3;
goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int) _temp105 > 4u?*((
int*) _temp105) == Cyc_Absyn_StructType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_StructType_struct*)
_temp105)->f1; goto _LL124; _LL124: _temp123=(( struct Cyc_Absyn_StructType_struct*)
_temp105)->f2; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp105)->f3; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp105 > 4u?*(( int*) _temp105) == Cyc_Absyn_AnonStructType: 0){ _LL128:
_temp127=(( struct Cyc_Absyn_AnonStructType_struct*) _temp105)->f1; goto _LL112;}
else{ goto _LL113;} _LL113: goto _LL114; _LL108:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp129=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp129[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp130; _temp130.tag= Cyc_Absyn_Array_e; _temp130.f1= des; _temp130;});
_temp129;}))); goto _LL106; _LL110: if( _temp121 == 0? 1: _temp125 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp131="Compiler bug: struct type not properly set";
struct _tagged_string _temp132; _temp132.curr= _temp131; _temp132.base= _temp131;
_temp132.last_plus_one= _temp131 + 43; _temp132;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp133=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp133[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp134; _temp134.tag= Cyc_Absyn_Struct_e; _temp134.f1=(
struct _tuple1*) _check_null( _temp125); _temp134.f2= 0; _temp134.f3= des;
_temp134.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp121)); _temp134;}); _temp133;}))); goto _LL106; _LL112:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp135=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp135[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp136; _temp136.tag= Cyc_Absyn_AnonStruct_e;
_temp136.f1=( void*) t; _temp136.f2= des; _temp136;}); _temp135;}))); goto
_LL106; _LL114:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp137=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp137[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp138; _temp138.tag= Cyc_Absyn_Array_e;
_temp138.f1= des; _temp138;}); _temp137;}))); goto _LL106; _LL106:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp139= msg_part;
struct _tagged_string _temp140= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp139.last_plus_one - _temp139.curr, _temp139.curr, _temp140.last_plus_one -
_temp140.curr, _temp140.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp141= c; unsigned char _temp157; void* _temp159; short
_temp161; void* _temp163; long long _temp165; void* _temp167; struct
_tagged_string _temp169; int _temp171; void* _temp173; struct _tagged_string
_temp175; _LL143: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_Char_c:
0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp141)->f1;
goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Char_c_struct*) _temp141)->f2;
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?*((
int*) _temp141) == Cyc_Absyn_Short_c: 0){ _LL164: _temp163=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp141)->f1; goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_Short_c_struct*)
_temp141)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_LongLong_c: 0){ _LL168: _temp167=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp141)->f1; goto _LL166; _LL166:
_temp165=(( struct Cyc_Absyn_LongLong_c_struct*) _temp141)->f2; goto _LL148;}
else{ goto _LL149;} _LL149: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141)
== Cyc_Absyn_Float_c: 0){ _LL170: _temp169=(( struct Cyc_Absyn_Float_c_struct*)
_temp141)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_Int_c: 0){ _LL174: _temp173=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp141)->f1; goto _LL172; _LL172:
_temp171=(( struct Cyc_Absyn_Int_c_struct*) _temp141)->f2; goto _LL152;} else{
goto _LL153;} _LL153: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141) ==
Cyc_Absyn_String_c: 0){ _LL176: _temp175=(( struct Cyc_Absyn_String_c_struct*)
_temp141)->f1; goto _LL154;} else{ goto _LL155;} _LL155: if( _temp141 ==( void*)
Cyc_Absyn_Null_c){ goto _LL156;} else{ goto _LL142;} _LL144: t= _temp159 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL142;
_LL146: t= _temp163 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL142; _LL148: t= _temp167 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL142; _LL150: t= Cyc_Absyn_float_t; goto _LL142;
_LL152: if( topt == 0){ t= _temp173 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp177= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp187; void* _temp189; void* _temp191; void*
_temp193; void* _temp195; void* _temp197; _LL179: if(( unsigned int) _temp177 >
4u?*(( int*) _temp177) == Cyc_Absyn_IntType: 0){ _LL190: _temp189=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp177)->f1; goto _LL188; _LL188: _temp187=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp177)->f2; if( _temp187 ==( void*)
Cyc_Absyn_B1){ goto _LL180;} else{ goto _LL181;}} else{ goto _LL181;} _LL181:
if(( unsigned int) _temp177 > 4u?*(( int*) _temp177) == Cyc_Absyn_IntType: 0){
_LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp177)->f1;
goto _LL192; _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f2; if( _temp191 ==( void*) Cyc_Absyn_B2){ goto _LL182;} else{ goto
_LL183;}} else{ goto _LL183;} _LL183: if(( unsigned int) _temp177 > 4u?*(( int*)
_temp177) == Cyc_Absyn_IntType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f1; goto _LL196; _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f2; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL180: t=
_temp189 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp199=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp199[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp200; _temp200.tag= Cyc_Absyn_Const_e;
_temp200.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp201=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp201[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp202; _temp202.tag= Cyc_Absyn_Char_c;
_temp202.f1=( void*) _temp189; _temp202.f2=( unsigned char) _temp171; _temp202;});
_temp201;})); _temp200;}); _temp199;}))); goto _LL178; _LL182: t= _temp193 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp203=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp203[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp204; _temp204.tag= Cyc_Absyn_Const_e; _temp204.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp205=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp205[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp206; _temp206.tag= Cyc_Absyn_Short_c; _temp206.f1=( void*) _temp193;
_temp206.f2=( short) _temp171; _temp206;}); _temp205;})); _temp204;}); _temp203;})));
goto _LL178; _LL184: t= _temp197 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL178; _LL186: t= _temp173 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL178; _LL178:;} goto _LL142; _LL154: {
int len=( int)({ struct _tagged_string _temp248= _temp175;( unsigned int)(
_temp248.last_plus_one - _temp248.curr);}); struct Cyc_Absyn_Const_e_struct*
_temp211=({ struct Cyc_Absyn_Const_e_struct* _temp207=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp207[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp208; _temp208.tag= Cyc_Absyn_Const_e; _temp208.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp209=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp209[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp210; _temp210.tag= Cyc_Absyn_Int_c; _temp210.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp210.f2= len; _temp210;}); _temp209;})); _temp208;}); _temp207;}); goto
_LL212; _LL212: { struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*)
_temp211, loc); elen->topt=({ struct Cyc_Core_Opt* _temp213=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp213->v=( void*) Cyc_Absyn_uint_t;
_temp213;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp214=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp214[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp215; _temp215.tag= Cyc_Absyn_Upper_b;
_temp215.f1= elen; _temp215;}); _temp214;})); if( topt != 0){ void* _temp216=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_Exp*
_temp224; struct Cyc_Absyn_Tqual _temp226; void* _temp228; struct Cyc_Absyn_PtrInfo
_temp230; struct Cyc_Absyn_Conref* _temp232; struct Cyc_Absyn_Tqual _temp234;
struct Cyc_Absyn_Conref* _temp236; void* _temp238; void* _temp240; _LL218: if((
unsigned int) _temp216 > 4u?*(( int*) _temp216) == Cyc_Absyn_ArrayType: 0){
_LL229: _temp228=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp216)->f1;
goto _LL227; _LL227: _temp226=(( struct Cyc_Absyn_ArrayType_struct*) _temp216)->f2;
goto _LL225; _LL225: _temp224=(( struct Cyc_Absyn_ArrayType_struct*) _temp216)->f3;
goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp216 > 4u?*((
int*) _temp216) == Cyc_Absyn_PointerType: 0){ _LL231: _temp230=(( struct Cyc_Absyn_PointerType_struct*)
_temp216)->f1; _LL241: _temp240=( void*) _temp230.elt_typ; goto _LL239; _LL239:
_temp238=( void*) _temp230.rgn_typ; goto _LL237; _LL237: _temp236= _temp230.nullable;
goto _LL235; _LL235: _temp234= _temp230.tq; goto _LL233; _LL233: _temp232=
_temp230.bounds; goto _LL221;} else{ goto _LL222;} _LL222: goto _LL223; _LL219:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp242=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp242[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp243; _temp243.tag= Cyc_Absyn_ArrayType; _temp243.f1=(
void*) Cyc_Absyn_uchar_t; _temp243.f2= _temp226; _temp243.f3=( struct Cyc_Absyn_Exp*)
elen; _temp243;}); _temp242;}); _LL221: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp244=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp244->v=( void*) t; _temp244;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp245=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp245[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp246; _temp246.tag= Cyc_Absyn_Upper_b;
_temp246.f1= elen; _temp246;}); _temp245;})); if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp247=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp247->v=( void*) t; _temp247;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));}} goto _LL217; _LL223: goto _LL217; _LL217:;} return t;}}
_LL156: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp249=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp249[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp250; _temp250.tag= Cyc_Absyn_PointerType;
_temp250.f1=({ struct Cyc_Absyn_PtrInfo _temp251; _temp251.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp251.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp251.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp251.tq= Cyc_Absyn_empty_tqual(); _temp251.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp251;}); _temp250;});
_temp249;}); goto _LL142; _LL142:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp252= b; struct Cyc_Absyn_Vardecl* _temp266; struct Cyc_Absyn_Fndecl*
_temp268; struct Cyc_Absyn_Vardecl* _temp270; struct Cyc_Absyn_Vardecl* _temp272;
struct Cyc_Absyn_Vardecl* _temp274; _LL254: if( _temp252 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp252 > 1u?*((
int*) _temp252) == Cyc_Absyn_Global_b: 0){ _LL267: _temp266=(( struct Cyc_Absyn_Global_b_struct*)
_temp252)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int)
_temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Funname_b: 0){ _LL269: _temp268=((
struct Cyc_Absyn_Funname_b_struct*) _temp252)->f1; goto _LL259;} else{ goto
_LL260;} _LL260: if(( unsigned int) _temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Pat_b:
0){ _LL271: _temp270=(( struct Cyc_Absyn_Pat_b_struct*) _temp252)->f1; goto
_LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp252 > 1u?*(( int*)
_temp252) == Cyc_Absyn_Local_b: 0){ _LL273: _temp272=(( struct Cyc_Absyn_Local_b_struct*)
_temp252)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Param_b: 0){ _LL275: _temp274=((
struct Cyc_Absyn_Param_b_struct*) _temp252)->f1; goto _LL265;} else{ goto _LL253;}
_LL255: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp276=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s", _temp276.last_plus_one
- _temp276.curr, _temp276.curr);})); _LL257: q[ _check_known_subscript_notnull(
1u, 0)]=( _temp266->name)[ _check_known_subscript_notnull( 1u, 0)]; return( void*)
_temp266->type; _LL259: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp268->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp268);
_LL261: _temp272= _temp270; goto _LL263; _LL263: _temp274= _temp272; goto _LL265;
_LL265:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp274->type; _LL253:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp277=( void*) fmt->r; void* _temp283; struct _tagged_string _temp285;
_LL279: if(*(( int*) _temp277) == Cyc_Absyn_Const_e){ _LL284: _temp283=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp277)->f1; if(( unsigned int) _temp283 > 1u?*((
int*) _temp283) == Cyc_Absyn_String_c: 0){ _LL286: _temp285=(( struct Cyc_Absyn_String_c_struct*)
_temp283)->f1; goto _LL280;} else{ goto _LL281;}} else{ goto _LL281;} _LL281:
goto _LL282; _LL280: desc_types= type_getter( _temp285, fmt->loc); goto _LL278;
_LL282: Cyc_Tcutil_terr( fmt->loc,({ unsigned char* _temp287="expecting a literal format string";
struct _tagged_string _temp288; _temp288.curr= _temp287; _temp288.base= _temp287;
_temp288.last_plus_one= _temp287 + 34; _temp288;})); return; _LL278:;} for( 0;
desc_types != 0? args != 0: 0;( desc_types=(( struct Cyc_List_List*) _check_null(
desc_types))->tl, args=(( struct Cyc_List_List*) _check_null( args))->tl)){ void*
t=( void*)(( struct Cyc_List_List*) _check_null( desc_types))->hd; struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; if(
! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp289= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp290= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
xprintf("descriptor has type %.*s but argument has type %.*s", _temp289.last_plus_one
- _temp289.curr, _temp289.curr, _temp290.last_plus_one - _temp290.curr, _temp290.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,({ unsigned char* _temp291="too few arguments";
struct _tagged_string _temp292; _temp292.curr= _temp291; _temp292.base= _temp291;
_temp292.last_plus_one= _temp291 + 18; _temp292;}));} if( args != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->loc,({
unsigned char* _temp293="too many arguments"; struct _tagged_string _temp294;
_temp294.curr= _temp293; _temp294.base= _temp293; _temp294.last_plus_one=
_temp293 + 19; _temp294;}));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp295= p; _LL297: if( _temp295 ==( void*) Cyc_Absyn_Plus){ goto _LL298;}
else{ goto _LL299;} _LL299: if( _temp295 ==( void*) Cyc_Absyn_Minus){ goto
_LL300;} else{ goto _LL301;} _LL301: if( _temp295 ==( void*) Cyc_Absyn_Not){
goto _LL302;} else{ goto _LL303;} _LL303: if( _temp295 ==( void*) Cyc_Absyn_Bitnot){
goto _LL304;} else{ goto _LL305;} _LL305: if( _temp295 ==( void*) Cyc_Absyn_Size){
goto _LL306;} else{ goto _LL307;} _LL307: goto _LL308; _LL298: goto _LL300;
_LL300: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp309= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_temp309.last_plus_one - _temp309.curr, _temp309.curr);}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL302: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp310= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp310.last_plus_one - _temp310.curr, _temp310.curr);}));} return Cyc_Absyn_sint_t;
_LL304: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp311= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp311.last_plus_one - _temp311.curr, _temp311.curr);}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL306:{ void* _temp312= t;
struct Cyc_Absyn_Exp* _temp320; struct Cyc_Absyn_Tqual _temp322; void* _temp324;
struct Cyc_Absyn_PtrInfo _temp326; struct Cyc_Absyn_Conref* _temp328; struct Cyc_Absyn_Tqual
_temp330; struct Cyc_Absyn_Conref* _temp332; void* _temp334; void* _temp336;
_LL314: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_ArrayType:
0){ _LL325: _temp324=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f1;
goto _LL323; _LL323: _temp322=(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f2;
goto _LL321; _LL321: _temp320=(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f3;
goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int) _temp312 > 4u?*((
int*) _temp312) == Cyc_Absyn_PointerType: 0){ _LL327: _temp326=(( struct Cyc_Absyn_PointerType_struct*)
_temp312)->f1; _LL337: _temp336=( void*) _temp326.elt_typ; goto _LL335; _LL335:
_temp334=( void*) _temp326.rgn_typ; goto _LL333; _LL333: _temp332= _temp326.nullable;
goto _LL331; _LL331: _temp330= _temp326.tq; goto _LL329; _LL329: _temp328=
_temp326.bounds; goto _LL317;} else{ goto _LL318;} _LL318: goto _LL319; _LL315:
goto _LL313; _LL317: goto _LL313; _LL319: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp338= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_temp338.last_plus_one - _temp338.curr, _temp338.curr);})); goto _LL313; _LL313:;}
return Cyc_Absyn_uint_t; _LL308: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp339="Non-unary primop"; struct _tagged_string _temp340;
_temp340.curr= _temp339; _temp340.base= _temp339; _temp340.last_plus_one=
_temp339 + 17; _temp340;})); _LL296:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp341= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("type %.*s cannot be used here",
_temp341.last_plus_one - _temp341.curr, _temp341.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp342=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
xprintf("type %.*s cannot be used here", _temp342.last_plus_one - _temp342.curr,
_temp342.curr);})); return Cyc_Absyn_wildtyp();}{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type(
t1, t2);}} static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp343= t1; struct
Cyc_Absyn_PtrInfo _temp349; struct Cyc_Absyn_Conref* _temp351; struct Cyc_Absyn_Tqual
_temp353; struct Cyc_Absyn_Conref* _temp355; void* _temp357; void* _temp359;
_LL345: if(( unsigned int) _temp343 > 4u?*(( int*) _temp343) == Cyc_Absyn_PointerType:
0){ _LL350: _temp349=(( struct Cyc_Absyn_PointerType_struct*) _temp343)->f1;
_LL360: _temp359=( void*) _temp349.elt_typ; goto _LL358; _LL358: _temp357=( void*)
_temp349.rgn_typ; goto _LL356; _LL356: _temp355= _temp349.nullable; goto _LL354;
_LL354: _temp353= _temp349.tq; goto _LL352; _LL352: _temp351= _temp349.bounds;
goto _LL346;} else{ goto _LL347;} _LL347: goto _LL348; _LL346: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp361= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int but found %.*s", _temp361.last_plus_one - _temp361.curr,
_temp361.curr);}));} _temp351=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp351);{ void* _temp362=( void*) _temp351->v;
void* _temp370; void* _temp372; struct Cyc_Absyn_Exp* _temp374; _LL364: if((
unsigned int) _temp362 > 1u?*(( int*) _temp362) == Cyc_Absyn_Eq_constr: 0){
_LL371: _temp370=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp362)->f1;
if( _temp370 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL365;} else{ goto _LL366;}}
else{ goto _LL366;} _LL366: if(( unsigned int) _temp362 > 1u?*(( int*) _temp362)
== Cyc_Absyn_Eq_constr: 0){ _LL373: _temp372=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp362)->f1; if(( unsigned int) _temp372 > 1u?*(( int*) _temp372) == Cyc_Absyn_Upper_b:
0){ _LL375: _temp374=(( struct Cyc_Absyn_Upper_b_struct*) _temp372)->f1; goto
_LL367;} else{ goto _LL368;}} else{ goto _LL368;} _LL368: goto _LL369; _LL365:
return t1; _LL367: { unsigned int _temp376= Cyc_Evexp_eval_const_uint_exp(
_temp374); goto _LL377; _LL377: _temp355=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp355);{ int is_nullable;{
void* _temp378=( void*) _temp355->v; int _temp384; _LL380: if(( unsigned int)
_temp378 > 1u?*(( int*) _temp378) == Cyc_Absyn_Eq_constr: 0){ _LL385: _temp384=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp378)->f1; goto _LL381;} else{
goto _LL382;} _LL382: goto _LL383; _LL381: if( _temp384){ struct Cyc_Absyn_PointerType_struct*
_temp389=({ struct Cyc_Absyn_PointerType_struct* _temp386=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp386[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp387; _temp387.tag= Cyc_Absyn_PointerType;
_temp387.f1=({ struct Cyc_Absyn_PtrInfo _temp388; _temp388.elt_typ=( void*)
_temp359; _temp388.rgn_typ=( void*) _temp357; _temp388.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp388.tq= _temp353; _temp388.bounds=
_temp351; _temp388;}); _temp387;}); _temp386;}); goto _LL390; _LL390: Cyc_Tcutil_warn(
e1->loc,({ struct _tagged_string _temp391= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp392= Cyc_Absynpp_typ2string(( void*) _temp389); xprintf("coercing from %.*s to %.*s",
_temp391.last_plus_one - _temp391.curr, _temp391.curr, _temp392.last_plus_one -
_temp392.curr, _temp392.curr);})); Cyc_Tcutil_unchecked_cast( te, e1,( void*)
_temp389);} goto _LL379; _LL383:( void*)( _temp355->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp393=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp393[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp394; _temp394.tag= Cyc_Absyn_Eq_constr; _temp394.f1=(
void*) 0; _temp394;}); _temp393;}))); goto _LL379; _LL379:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int _temp395= Cyc_Evexp_eval_const_uint_exp( e2); goto _LL396;
_LL396: if( _temp376 < _temp395){ _temp395= _temp376; Cyc_Tcutil_warn( e1->loc,
xprintf("pointer arithmetic is out of bounds"));}{ struct Cyc_Absyn_Exp*
_temp397= Cyc_Absyn_uint_exp( _temp376 - _temp395, 0); goto _LL398; _LL398:
_temp397->topt=({ struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp399->v=( void*) Cyc_Absyn_uint_t;
_temp399;}); return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp400=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp400[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp401; _temp401.tag= Cyc_Absyn_PointerType;
_temp401.f1=({ struct Cyc_Absyn_PtrInfo _temp402; _temp402.elt_typ=( void*)
_temp359; _temp402.rgn_typ=( void*) _temp357; _temp402.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp402.tq= _temp353; _temp402.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp403=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp403[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp404; _temp404.tag= Cyc_Absyn_Upper_b; _temp404.f1= _temp397; _temp404;});
_temp403;})); _temp402;}); _temp401;}); _temp400;});}} else{ Cyc_Tcutil_terr( e1->loc,({
unsigned char* _temp405="cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 98; _temp406;})); return t1;}}} _LL369:( void*)(
_temp351->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp407=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp407[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp408; _temp408.tag= Cyc_Absyn_Eq_constr;
_temp408.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp408;}); _temp407;})));
return t1; _LL363:;} _LL348: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL344:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp409= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); struct _tagged_string _temp410= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp409.last_plus_one - _temp409.curr, _temp409.curr, _temp410.last_plus_one -
_temp410.curr, _temp410.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp411= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp412= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp411.last_plus_one - _temp411.curr, _temp411.curr, _temp412.last_plus_one -
_temp412.curr, _temp412.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_BoxKind)): 0){
return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2, t1)){
Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e1, t2); return Cyc_Absyn_sint_t;}
else{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ struct _tuple0
_temp414=({ struct _tuple0 _temp413; _temp413.f1= Cyc_Tcutil_compress( t1);
_temp413.f2= Cyc_Tcutil_compress( t2); _temp413;}); void* _temp420; struct Cyc_Absyn_PtrInfo
_temp422; struct Cyc_Absyn_Conref* _temp424; struct Cyc_Absyn_Tqual _temp426;
struct Cyc_Absyn_Conref* _temp428; void* _temp430; void* _temp432; void*
_temp434; struct Cyc_Absyn_PtrInfo _temp436; struct Cyc_Absyn_Conref* _temp438;
struct Cyc_Absyn_Tqual _temp440; struct Cyc_Absyn_Conref* _temp442; void*
_temp444; void* _temp446; _LL416: _LL435: _temp434= _temp414.f1; if((
unsigned int) _temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_PointerType: 0){
_LL437: _temp436=(( struct Cyc_Absyn_PointerType_struct*) _temp434)->f1; _LL447:
_temp446=( void*) _temp436.elt_typ; goto _LL445; _LL445: _temp444=( void*)
_temp436.rgn_typ; goto _LL443; _LL443: _temp442= _temp436.nullable; goto _LL441;
_LL441: _temp440= _temp436.tq; goto _LL439; _LL439: _temp438= _temp436.bounds;
goto _LL421;} else{ goto _LL418;} _LL421: _temp420= _temp414.f2; if((
unsigned int) _temp420 > 4u?*(( int*) _temp420) == Cyc_Absyn_PointerType: 0){
_LL423: _temp422=(( struct Cyc_Absyn_PointerType_struct*) _temp420)->f1; _LL433:
_temp432=( void*) _temp422.elt_typ; goto _LL431; _LL431: _temp430=( void*)
_temp422.rgn_typ; goto _LL429; _LL429: _temp428= _temp422.nullable; goto _LL427;
_LL427: _temp426= _temp422.tq; goto _LL425; _LL425: _temp424= _temp422.bounds;
goto _LL417;} else{ goto _LL418;} _LL418: goto _LL419; _LL417: if( Cyc_Tcutil_unify(
_temp446, _temp432)){ return Cyc_Absyn_sint_t;} goto _LL415; _LL419: goto _LL415;
_LL415:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp448= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp449= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp448.last_plus_one - _temp448.curr, _temp448.curr, _temp449.last_plus_one -
_temp449.curr, _temp449.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp450= p; _LL452: if( _temp450 ==( void*) Cyc_Absyn_Plus){ goto _LL453;}
else{ goto _LL454;} _LL454: if( _temp450 ==( void*) Cyc_Absyn_Minus){ goto
_LL455;} else{ goto _LL456;} _LL456: if( _temp450 ==( void*) Cyc_Absyn_Times){
goto _LL457;} else{ goto _LL458;} _LL458: if( _temp450 ==( void*) Cyc_Absyn_Div){
goto _LL459;} else{ goto _LL460;} _LL460: if( _temp450 ==( void*) Cyc_Absyn_Mod){
goto _LL461;} else{ goto _LL462;} _LL462: if( _temp450 ==( void*) Cyc_Absyn_Bitand){
goto _LL463;} else{ goto _LL464;} _LL464: if( _temp450 ==( void*) Cyc_Absyn_Bitor){
goto _LL465;} else{ goto _LL466;} _LL466: if( _temp450 ==( void*) Cyc_Absyn_Bitxor){
goto _LL467;} else{ goto _LL468;} _LL468: if( _temp450 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL469;} else{ goto _LL470;} _LL470: if( _temp450 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL471;} else{ goto _LL472;} _LL472: if( _temp450 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL473;} else{ goto _LL474;} _LL474: if( _temp450 ==( void*) Cyc_Absyn_Eq){
goto _LL475;} else{ goto _LL476;} _LL476: if( _temp450 ==( void*) Cyc_Absyn_Neq){
goto _LL477;} else{ goto _LL478;} _LL478: if( _temp450 ==( void*) Cyc_Absyn_Gt){
goto _LL479;} else{ goto _LL480;} _LL480: if( _temp450 ==( void*) Cyc_Absyn_Lt){
goto _LL481;} else{ goto _LL482;} _LL482: if( _temp450 ==( void*) Cyc_Absyn_Gte){
goto _LL483;} else{ goto _LL484;} _LL484: if( _temp450 ==( void*) Cyc_Absyn_Lte){
goto _LL485;} else{ goto _LL486;} _LL486: goto _LL487; _LL453: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL455: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL457: goto _LL459;
_LL459: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL461: goto _LL463; _LL463: goto _LL465; _LL465: goto _LL467; _LL467: goto
_LL469; _LL469: goto _LL471; _LL471: goto _LL473; _LL473: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL475: goto _LL477; _LL477: goto _LL479;
_LL479: goto _LL481; _LL481: goto _LL483; _LL483: goto _LL485; _LL485: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL487:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp488=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp488[ 0]=({ struct Cyc_Core_Failure_struct
_temp489; _temp489.tag= Cyc_Core_Failure; _temp489.f1=({ unsigned char* _temp490="bad binary primop";
struct _tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 18; _temp491;}); _temp489;}); _temp488;}));
_LL451:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp492= p; _LL494: if( _temp492
==( void*) Cyc_Absyn_Printf){ goto _LL495;} else{ goto _LL496;} _LL496: if(
_temp492 ==( void*) Cyc_Absyn_Fprintf){ goto _LL497;} else{ goto _LL498;} _LL498:
if( _temp492 ==( void*) Cyc_Absyn_Xprintf){ goto _LL499;} else{ goto _LL500;}
_LL500: if( _temp492 ==( void*) Cyc_Absyn_Scanf){ goto _LL501;} else{ goto
_LL502;} _LL502: if( _temp492 ==( void*) Cyc_Absyn_Fscanf){ goto _LL503;} else{
goto _LL504;} _LL504: if( _temp492 ==( void*) Cyc_Absyn_Sscanf){ goto _LL505;}
else{ goto _LL506;} _LL506: goto _LL507; _LL495: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ unsigned char* _temp508="missing format string in printf"; struct
_tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 32; _temp509;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL493; _LL497: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char*
_temp510="too few arguments to fprintf"; struct _tagged_string _temp511;
_temp511.curr= _temp510; _temp511.base= _temp510; _temp511.last_plus_one=
_temp510 + 29; _temp511;}));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,({ struct _tagged_string _temp512= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v); xprintf("expecting FILE @ but found %.*s",
_temp512.last_plus_one - _temp512.curr, _temp512.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd,(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->tl, Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL493; _LL499: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp513="missing format string in xprintf"; struct
_tagged_string _temp514; _temp514.curr= _temp513; _temp514.base= _temp513;
_temp514.last_plus_one= _temp513 + 33; _temp514;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL493; _LL501: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp515="missing format string in scanf"; struct _tagged_string
_temp516; _temp516.curr= _temp515; _temp516.base= _temp515; _temp516.last_plus_one=
_temp515 + 31; _temp516;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto
_LL493; _LL503: if( es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl
== 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp517="too few arguments to fscanf";
struct _tagged_string _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 28; _temp518;}));} if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp519= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _temp519.last_plus_one - _temp519.curr,
_temp519.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL493; _LL505: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp520="sscanf is not supported"; struct _tagged_string
_temp521; _temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 24; _temp521;})); t= Cyc_Absyn_sint_t; goto _LL493; _LL507: switch(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){ case 0: _LL522: return
Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp524="primitive operator has 0 arguments";
struct _tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 35; _temp525;})); case 1: _LL523: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL526: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL527: return Cyc_Tcexp_expr_err(
te, loc,({ unsigned char* _temp529="primitive operator has > 2 arguments";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 37; _temp530;}));} goto _LL493; _LL493:;}
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp531=( void*) e->r; void* _temp555; struct Cyc_Absyn_Vardecl* _temp557;
struct _tuple1* _temp559; void* _temp561; struct Cyc_Absyn_Vardecl* _temp563;
struct _tuple1* _temp565; void* _temp567; struct Cyc_Absyn_Vardecl* _temp569;
struct _tuple1* _temp571; void* _temp573; struct Cyc_Absyn_Vardecl* _temp575;
struct _tuple1* _temp577; struct Cyc_Absyn_Exp* _temp579; struct Cyc_Absyn_Exp*
_temp581; struct _tagged_string* _temp583; struct Cyc_Absyn_Exp* _temp585;
struct _tagged_string* _temp587; struct Cyc_Absyn_Exp* _temp589; struct Cyc_Absyn_Exp*
_temp591; struct Cyc_Absyn_Exp* _temp593; struct Cyc_List_List* _temp595; struct
Cyc_Absyn_Exp* _temp597; _LL533: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){
_LL560: _temp559=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL556;
_LL556: _temp555=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp555 > 1u?*(( int*) _temp555) == Cyc_Absyn_Param_b: 0){ _LL558:
_temp557=(( struct Cyc_Absyn_Param_b_struct*) _temp555)->f1; goto _LL534;} else{
goto _LL535;}} else{ goto _LL535;} _LL535: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){
_LL566: _temp565=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL562;
_LL562: _temp561=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp561 > 1u?*(( int*) _temp561) == Cyc_Absyn_Local_b: 0){ _LL564:
_temp563=(( struct Cyc_Absyn_Local_b_struct*) _temp561)->f1; goto _LL536;} else{
goto _LL537;}} else{ goto _LL537;} _LL537: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){
_LL572: _temp571=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL568;
_LL568: _temp567=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp567 > 1u?*(( int*) _temp567) == Cyc_Absyn_Pat_b: 0){ _LL570:
_temp569=(( struct Cyc_Absyn_Pat_b_struct*) _temp567)->f1; goto _LL538;} else{
goto _LL539;}} else{ goto _LL539;} _LL539: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){
_LL578: _temp577=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL574;
_LL574: _temp573=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp573 > 1u?*(( int*) _temp573) == Cyc_Absyn_Global_b: 0){
_LL576: _temp575=(( struct Cyc_Absyn_Global_b_struct*) _temp573)->f1; goto
_LL540;} else{ goto _LL541;}} else{ goto _LL541;} _LL541: if(*(( int*) _temp531)
== Cyc_Absyn_Subscript_e){ _LL582: _temp581=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp531)->f1; goto _LL580; _LL580: _temp579=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp531)->f2; goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*) _temp531)
== Cyc_Absyn_StructMember_e){ _LL586: _temp585=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp531)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp531)->f2; goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp531)
== Cyc_Absyn_StructArrow_e){ _LL590: _temp589=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp531)->f1; goto _LL588; _LL588: _temp587=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp531)->f2; goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp531)
== Cyc_Absyn_Deref_e){ _LL592: _temp591=(( struct Cyc_Absyn_Deref_e_struct*)
_temp531)->f1; goto _LL548;} else{ goto _LL549;} _LL549: if(*(( int*) _temp531)
== Cyc_Absyn_NoInstantiate_e){ _LL594: _temp593=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp531)->f1; goto _LL550;} else{ goto _LL551;} _LL551: if(*(( int*) _temp531)
== Cyc_Absyn_Instantiate_e){ _LL598: _temp597=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp531)->f1; goto _LL596; _LL596: _temp595=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp531)->f2; goto _LL552;} else{ goto _LL553;} _LL553: goto _LL554; _LL534:
_temp563= _temp557; goto _LL536; _LL536: _temp569= _temp563; goto _LL538; _LL538:
_temp575= _temp569; goto _LL540; _LL540: if( !( _temp575->tq).q_const){ return;}
goto _LL532; _LL542:{ void* _temp599= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp581->topt))->v); struct Cyc_Absyn_PtrInfo _temp609; struct Cyc_Absyn_Conref*
_temp611; struct Cyc_Absyn_Tqual _temp613; struct Cyc_Absyn_Conref* _temp615;
void* _temp617; void* _temp619; struct Cyc_Absyn_Exp* _temp621; struct Cyc_Absyn_Tqual
_temp623; void* _temp625; struct Cyc_List_List* _temp627; _LL601: if((
unsigned int) _temp599 > 4u?*(( int*) _temp599) == Cyc_Absyn_PointerType: 0){
_LL610: _temp609=(( struct Cyc_Absyn_PointerType_struct*) _temp599)->f1; _LL620:
_temp619=( void*) _temp609.elt_typ; goto _LL618; _LL618: _temp617=( void*)
_temp609.rgn_typ; goto _LL616; _LL616: _temp615= _temp609.nullable; goto _LL614;
_LL614: _temp613= _temp609.tq; goto _LL612; _LL612: _temp611= _temp609.bounds;
goto _LL602;} else{ goto _LL603;} _LL603: if(( unsigned int) _temp599 > 4u?*((
int*) _temp599) == Cyc_Absyn_ArrayType: 0){ _LL626: _temp625=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp599)->f1; goto _LL624; _LL624: _temp623=((
struct Cyc_Absyn_ArrayType_struct*) _temp599)->f2; goto _LL622; _LL622: _temp621=((
struct Cyc_Absyn_ArrayType_struct*) _temp599)->f3; goto _LL604;} else{ goto
_LL605;} _LL605: if(( unsigned int) _temp599 > 4u?*(( int*) _temp599) == Cyc_Absyn_TupleType:
0){ _LL628: _temp627=(( struct Cyc_Absyn_TupleType_struct*) _temp599)->f1; goto
_LL606;} else{ goto _LL607;} _LL607: goto _LL608; _LL602: _temp623= _temp613;
goto _LL604; _LL604: if( ! _temp623.q_const){ return;} goto _LL600; _LL606: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp579);{ struct _handler_cons
_temp629; _push_handler(& _temp629);{ int _temp631= 0; if( setjmp( _temp629.handler)){
_temp631= 1;} if( ! _temp631){{ struct _tuple7 _temp634; void* _temp635; struct
Cyc_Absyn_Tqual _temp637; struct _tuple7* _temp632=(( struct _tuple7*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp627,( int) i); _temp634=* _temp632;
_LL638: _temp637= _temp634.f1; goto _LL636; _LL636: _temp635= _temp634.f2; goto
_LL633; _LL633: if( ! _temp637.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp630=( void*) _exn_thrown; void* _temp640=
_temp630; _LL642: if( _temp640 == Cyc_List_Nth){ goto _LL643;} else{ goto _LL644;}
_LL644: goto _LL645; _LL643: return; _LL645:( void) _throw( _temp640); _LL641:;}}}
goto _LL600;} _LL608: goto _LL600; _LL600:;} goto _LL532; _LL544:{ void*
_temp646= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp585->topt))->v); struct Cyc_Absyn_Structdecl** _temp658; struct Cyc_List_List*
_temp660; struct _tuple1* _temp662; struct Cyc_List_List* _temp664; struct Cyc_List_List*
_temp666; struct Cyc_Absyn_Uniondecl** _temp668; struct Cyc_List_List* _temp670;
struct _tuple1* _temp672; _LL648: if(( unsigned int) _temp646 > 4u?*(( int*)
_temp646) == Cyc_Absyn_StructType: 0){ _LL663: _temp662=(( struct Cyc_Absyn_StructType_struct*)
_temp646)->f1; goto _LL661; _LL661: _temp660=(( struct Cyc_Absyn_StructType_struct*)
_temp646)->f2; goto _LL659; _LL659: _temp658=(( struct Cyc_Absyn_StructType_struct*)
_temp646)->f3; goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int)
_temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_AnonUnionType: 0){ _LL665:
_temp664=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp646)->f1; goto _LL651;}
else{ goto _LL652;} _LL652: if(( unsigned int) _temp646 > 4u?*(( int*) _temp646)
== Cyc_Absyn_AnonStructType: 0){ _LL667: _temp666=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp646)->f1; goto _LL653;} else{ goto _LL654;} _LL654: if(( unsigned int)
_temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_UnionType: 0){ _LL673: _temp672=((
struct Cyc_Absyn_UnionType_struct*) _temp646)->f1; goto _LL671; _LL671: _temp670=((
struct Cyc_Absyn_UnionType_struct*) _temp646)->f2; goto _LL669; _LL669: _temp668=((
struct Cyc_Absyn_UnionType_struct*) _temp646)->f3; goto _LL655;} else{ goto
_LL656;} _LL656: goto _LL657; _LL649: { struct Cyc_Absyn_Structfield* sf=
_temp658 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp658)), _temp583); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL647;} _LL651: _temp666= _temp664; goto _LL653; _LL653: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp666, _temp583); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL647;} _LL655: { struct Cyc_Absyn_Structfield*
sf= _temp668 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp668)), _temp583); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL647;} _LL657: goto _LL647; _LL647:;} goto _LL532; _LL546:{ void*
_temp674= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp589->topt))->v); struct Cyc_Absyn_PtrInfo _temp680; struct Cyc_Absyn_Conref*
_temp682; struct Cyc_Absyn_Tqual _temp684; struct Cyc_Absyn_Conref* _temp686;
void* _temp688; void* _temp690; _LL676: if(( unsigned int) _temp674 > 4u?*(( int*)
_temp674) == Cyc_Absyn_PointerType: 0){ _LL681: _temp680=(( struct Cyc_Absyn_PointerType_struct*)
_temp674)->f1; _LL691: _temp690=( void*) _temp680.elt_typ; goto _LL689; _LL689:
_temp688=( void*) _temp680.rgn_typ; goto _LL687; _LL687: _temp686= _temp680.nullable;
goto _LL685; _LL685: _temp684= _temp680.tq; goto _LL683; _LL683: _temp682=
_temp680.bounds; goto _LL677;} else{ goto _LL678;} _LL678: goto _LL679; _LL677:{
void* _temp692= Cyc_Tcutil_compress( _temp690); struct Cyc_Absyn_Structdecl**
_temp704; struct Cyc_List_List* _temp706; struct _tuple1* _temp708; struct Cyc_Absyn_Uniondecl**
_temp710; struct Cyc_List_List* _temp712; struct _tuple1* _temp714; struct Cyc_List_List*
_temp716; struct Cyc_List_List* _temp718; _LL694: if(( unsigned int) _temp692 >
4u?*(( int*) _temp692) == Cyc_Absyn_StructType: 0){ _LL709: _temp708=(( struct
Cyc_Absyn_StructType_struct*) _temp692)->f1; goto _LL707; _LL707: _temp706=((
struct Cyc_Absyn_StructType_struct*) _temp692)->f2; goto _LL705; _LL705:
_temp704=(( struct Cyc_Absyn_StructType_struct*) _temp692)->f3; goto _LL695;}
else{ goto _LL696;} _LL696: if(( unsigned int) _temp692 > 4u?*(( int*) _temp692)
== Cyc_Absyn_UnionType: 0){ _LL715: _temp714=(( struct Cyc_Absyn_UnionType_struct*)
_temp692)->f1; goto _LL713; _LL713: _temp712=(( struct Cyc_Absyn_UnionType_struct*)
_temp692)->f2; goto _LL711; _LL711: _temp710=(( struct Cyc_Absyn_UnionType_struct*)
_temp692)->f3; goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int)
_temp692 > 4u?*(( int*) _temp692) == Cyc_Absyn_AnonStructType: 0){ _LL717:
_temp716=(( struct Cyc_Absyn_AnonStructType_struct*) _temp692)->f1; goto _LL699;}
else{ goto _LL700;} _LL700: if(( unsigned int) _temp692 > 4u?*(( int*) _temp692)
== Cyc_Absyn_AnonUnionType: 0){ _LL719: _temp718=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp692)->f1; goto _LL701;} else{ goto _LL702;} _LL702: goto _LL703; _LL695: {
struct Cyc_Absyn_Structfield* sf= _temp704 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp704)), _temp587); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL693;} _LL697: { struct Cyc_Absyn_Structfield*
sf= _temp710 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp710)), _temp587); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL693;} _LL699: _temp718= _temp716; goto _LL701; _LL701: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp718, _temp587); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL693;} _LL703: goto _LL693;
_LL693:;} goto _LL675; _LL679: goto _LL675; _LL675:;} goto _LL532; _LL548:{ void*
_temp720= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp591->topt))->v); struct Cyc_Absyn_PtrInfo _temp728; struct Cyc_Absyn_Conref*
_temp730; struct Cyc_Absyn_Tqual _temp732; struct Cyc_Absyn_Conref* _temp734;
void* _temp736; void* _temp738; struct Cyc_Absyn_Exp* _temp740; struct Cyc_Absyn_Tqual
_temp742; void* _temp744; _LL722: if(( unsigned int) _temp720 > 4u?*(( int*)
_temp720) == Cyc_Absyn_PointerType: 0){ _LL729: _temp728=(( struct Cyc_Absyn_PointerType_struct*)
_temp720)->f1; _LL739: _temp738=( void*) _temp728.elt_typ; goto _LL737; _LL737:
_temp736=( void*) _temp728.rgn_typ; goto _LL735; _LL735: _temp734= _temp728.nullable;
goto _LL733; _LL733: _temp732= _temp728.tq; goto _LL731; _LL731: _temp730=
_temp728.bounds; goto _LL723;} else{ goto _LL724;} _LL724: if(( unsigned int)
_temp720 > 4u?*(( int*) _temp720) == Cyc_Absyn_ArrayType: 0){ _LL745: _temp744=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp720)->f1; goto _LL743; _LL743:
_temp742=(( struct Cyc_Absyn_ArrayType_struct*) _temp720)->f2; goto _LL741;
_LL741: _temp740=(( struct Cyc_Absyn_ArrayType_struct*) _temp720)->f3; goto
_LL725;} else{ goto _LL726;} _LL726: goto _LL727; _LL723: _temp742= _temp732;
goto _LL725; _LL725: if( ! _temp742.q_const){ return;} goto _LL721; _LL727: goto
_LL721; _LL721:;} goto _LL532; _LL550: _temp597= _temp593; goto _LL552; _LL552:
Cyc_Tcexp_check_writable( te, _temp597); return; _LL554: goto _LL532; _LL532:;}
Cyc_Tcutil_terr( e->loc,({ unsigned char* _temp746="attempt to write a read-only location";
struct _tagged_string _temp747; _temp747.curr= _temp746; _temp747.base= _temp746;
_temp747.last_plus_one= _temp746 + 38; _temp747;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp748="increment/decrement of non-lvalue";
struct _tagged_string _temp749; _temp749.curr= _temp748; _temp749.base= _temp748;
_temp749.last_plus_one= _temp748 + 34; _temp749;}));} Cyc_Tcexp_check_writable(
te, e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if(
! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp750= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic or ? type but found %.*s",
_temp750.last_plus_one - _temp750.curr, _temp750.curr);}));} return t;}} static
void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp*
e3){ Cyc_Tcexp_tcTest( te, e1,({ unsigned char* _temp751="conditional expression";
struct _tagged_string _temp752; _temp752.curr= _temp751; _temp752.base= _temp751;
_temp752.last_plus_one= _temp751 + 23; _temp752;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* _temp754=({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp753->hd=( void*) e3; _temp753->tl=
0; _temp753;}); goto _LL755; _LL755: { struct Cyc_List_List* _temp757=({ struct
Cyc_List_List* _temp756=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp756->hd=( void*) e2; _temp756->tl= _temp754; _temp756;}); goto _LL758;
_LL758: if( ! Cyc_Tcutil_coerce_list( te, t, _temp757)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp759= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); struct _tagged_string _temp760= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e3->topt))->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp759.last_plus_one - _temp759.curr, _temp759.curr, _temp760.last_plus_one -
_temp760.curr, _temp760.curr);}));} return t;}}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp761= Cyc_Tcutil_compress( t1); struct
Cyc_Absyn_Exp* _temp767; struct Cyc_Absyn_Tqual _temp769; void* _temp771; _LL763:
if(( unsigned int) _temp761 > 4u?*(( int*) _temp761) == Cyc_Absyn_ArrayType: 0){
_LL772: _temp771=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp761)->f1;
goto _LL770; _LL770: _temp769=(( struct Cyc_Absyn_ArrayType_struct*) _temp761)->f2;
goto _LL768; _LL768: _temp767=(( struct Cyc_Absyn_ArrayType_struct*) _temp761)->f3;
goto _LL764;} else{ goto _LL765;} _LL765: goto _LL766; _LL764: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp773="cannot assign to an array"; struct
_tagged_string _temp774; _temp774.curr= _temp773; _temp774.base= _temp773;
_temp774.last_plus_one= _temp773 + 26; _temp774;})); goto _LL762; _LL766: goto
_LL762; _LL762:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,({ unsigned char* _temp775="assignment to non-lvalue"; struct _tagged_string
_temp776; _temp776.curr= _temp775; _temp776.base= _temp775; _temp776.last_plus_one=
_temp775 + 25; _temp776;}));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){
if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp777= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp778= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp777.last_plus_one - _temp777.curr, _temp777.curr, _temp778.last_plus_one -
_temp778.curr, _temp778.curr);}));}} else{ void* _temp779=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; goto _LL780; _LL780: { void* _temp781= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp779, e1, e2); goto _LL782; _LL782: if( !( Cyc_Tcutil_unify(
_temp781, t1)? 1: Cyc_Tcutil_coerceable( _temp781))){ return Cyc_Tcexp_expr_err(
te, loc,({ unsigned char* _temp783="Cannot use this operator in an assignment";
struct _tagged_string _temp784; _temp784.curr= _temp783; _temp784.base= _temp783;
_temp784.last_plus_one= _temp783 + 42; _temp784;}));}}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* tu, struct
_RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List* fs){ struct
Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp787; struct Cyc_Position_Segment* _temp789; struct Cyc_List_List*
_temp791; struct Cyc_List_List* _temp793; struct _tuple1* _temp795; struct Cyc_Absyn_Tunionfield
_temp785=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL796: _temp795= _temp785.name; goto _LL794; _LL794: _temp793=
_temp785.tvs; goto _LL792; _LL792: _temp791= _temp785.typs; goto _LL790; _LL790:
_temp789= _temp785.loc; goto _LL788; _LL788: _temp787=( void*) _temp785.sc; goto
_LL786; _LL786: if(( _temp791 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp791))->tl != 0)? 1: _temp793 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp791))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp799; struct Cyc_Position_Segment* _temp801; struct Cyc_List_List*
_temp803; struct Cyc_List_List* _temp805; struct _tuple1* _temp807; struct Cyc_Absyn_Tunionfield
_temp797=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL808: _temp807= _temp797.name; goto _LL806; _LL806: _temp805=
_temp797.tvs; goto _LL804; _LL804: _temp803= _temp797.typs; goto _LL802; _LL802:
_temp801= _temp797.loc; goto _LL800; _LL800: _temp799=( void*) _temp797.sc; goto
_LL798; _LL798: if(( _temp803 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp803))->tl != 0)? 1: _temp805 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp803))->hd)).f2);
if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp809= Cyc_Absynpp_typ2string( tu); struct
_tagged_string _temp810= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_temp809.last_plus_one - _temp809.curr, _temp809.curr, _temp810.last_plus_one -
_temp810.curr, _temp810.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp811= t;
struct Cyc_Absyn_PtrInfo _temp817; struct Cyc_Absyn_Conref* _temp819; struct Cyc_Absyn_Tqual
_temp821; struct Cyc_Absyn_Conref* _temp823; void* _temp825; void* _temp827;
_LL813: if(( unsigned int) _temp811 > 4u?*(( int*) _temp811) == Cyc_Absyn_PointerType:
0){ _LL818: _temp817=(( struct Cyc_Absyn_PointerType_struct*) _temp811)->f1;
_LL828: _temp827=( void*) _temp817.elt_typ; goto _LL826; _LL826: _temp825=( void*)
_temp817.rgn_typ; goto _LL824; _LL824: _temp823= _temp817.nullable; goto _LL822;
_LL822: _temp821= _temp817.tq; goto _LL820; _LL820: _temp819= _temp817.bounds;
goto _LL814;} else{ goto _LL815;} _LL815: goto _LL816; _LL814: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp825); Cyc_Tcutil_check_nonzero_bound( loc, _temp819);{ void*
_temp829= Cyc_Tcutil_compress( _temp827); struct Cyc_Absyn_FnInfo _temp835;
struct Cyc_List_List* _temp837; struct Cyc_Absyn_VarargInfo* _temp839; int
_temp841; struct Cyc_List_List* _temp843; void* _temp845; struct Cyc_Core_Opt*
_temp847; struct Cyc_List_List* _temp849; _LL831: if(( unsigned int) _temp829 >
4u?*(( int*) _temp829) == Cyc_Absyn_FnType: 0){ _LL836: _temp835=(( struct Cyc_Absyn_FnType_struct*)
_temp829)->f1; _LL850: _temp849= _temp835.tvars; goto _LL848; _LL848: _temp847=
_temp835.effect; goto _LL846; _LL846: _temp845=( void*) _temp835.ret_typ; goto
_LL844; _LL844: _temp843= _temp835.args; goto _LL842; _LL842: _temp841= _temp835.c_varargs;
goto _LL840; _LL840: _temp839= _temp835.cyc_varargs; goto _LL838; _LL838:
_temp837= _temp835.attributes; goto _LL832;} else{ goto _LL833;} _LL833: goto
_LL834; _LL832: if( topt != 0){ Cyc_Tcutil_unify( _temp845,*(( void**)
_check_null( topt)));} while( es != 0? _temp843 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp843))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp851= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct
_tagged_string _temp852= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp851.last_plus_one - _temp851.curr, _temp851.curr, _temp852.last_plus_one -
_temp852.curr, _temp852.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp843=(( struct Cyc_List_List*) _check_null( _temp843))->tl;} if( _temp843 !=
0){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp853="too few arguments for function";
struct _tagged_string _temp854; _temp854.curr= _temp853; _temp854.base= _temp853;
_temp854.last_plus_one= _temp853 + 31; _temp854;}));} else{ if( es != 0){ if(
_temp841){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd);}} else{ if( _temp839 == 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp855="too many arguments for function"; struct _tagged_string
_temp856; _temp856.curr= _temp855; _temp856.base= _temp855; _temp856.last_plus_one=
_temp855 + 32; _temp856;}));} else{ int _temp859; void* _temp861; void* _temp863;
struct Cyc_Absyn_Tqual _temp865; struct Cyc_Core_Opt* _temp867; struct Cyc_Absyn_VarargInfo
_temp857=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp839)); _LL868:
_temp867= _temp857.name; goto _LL866; _LL866: _temp865= _temp857.tq; goto _LL864;
_LL864: _temp863=( void*) _temp857.type; goto _LL862; _LL862: _temp861=( void*)
_temp857.rgn; goto _LL860; _LL860: _temp859= _temp857.inject; goto _LL858;
_LL858: te= Cyc_Tcenv_new_block( loc, te);{ struct Cyc_Absyn_VarargCallInfo*
_temp870=({ struct Cyc_Absyn_VarargCallInfo* _temp869=( struct Cyc_Absyn_VarargCallInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp869->num_varargs= 0;
_temp869->injectors= 0; _temp869->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp839); _temp869;}); goto _LL871; _LL871:* vararg_call_info=
_temp870; if( ! Cyc_Tcutil_unify( Cyc_Tcenv_curr_rgn( te), _temp861)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp872= Cyc_Absynpp_typ2string( _temp861); struct
_tagged_string _temp873= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));
xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_temp872.last_plus_one - _temp872.curr, _temp872.curr, _temp873.last_plus_one -
_temp873.curr, _temp873.curr);}));} if( ! _temp859){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp870))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp863,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp863)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp874= Cyc_Absynpp_typ2string( _temp863); struct
_tagged_string _temp875= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_temp874.last_plus_one - _temp874.curr, _temp874.curr, _temp875.last_plus_one -
_temp875.curr, _temp875.curr);}));}}} else{ void* _temp876= Cyc_Tcutil_compress(
_temp863); struct Cyc_Absyn_TunionInfo _temp882; void* _temp884; struct Cyc_List_List*
_temp886; void* _temp888; struct Cyc_Absyn_Tuniondecl* _temp890; _LL878: if((
unsigned int) _temp876 > 4u?*(( int*) _temp876) == Cyc_Absyn_TunionType: 0){
_LL883: _temp882=(( struct Cyc_Absyn_TunionType_struct*) _temp876)->f1; _LL889:
_temp888=( void*) _temp882.tunion_info; if(*(( int*) _temp888) == Cyc_Absyn_KnownTunion){
_LL891: _temp890=(( struct Cyc_Absyn_KnownTunion_struct*) _temp888)->f1; goto
_LL887;} else{ goto _LL880;} _LL887: _temp886= _temp882.targs; goto _LL885;
_LL885: _temp884=( void*) _temp882.rgn; goto _LL879;} else{ goto _LL880;} _LL880:
goto _LL881; _LL879: { struct Cyc_Absyn_Tuniondecl* _temp892=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp890->name); goto _LL893; _LL893: if( ! Cyc_Tcutil_unify( _temp884,
_temp861)){ Cyc_Tcutil_terr( loc, xprintf("bad region for injected varargs"));}{
struct Cyc_List_List* fields= 0; if( _temp892->fields == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp894= Cyc_Absynpp_typ2string( _temp863);
xprintf("can't inject into %.*s", _temp894.last_plus_one - _temp894.curr,
_temp894.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp892->fields))->v;}{ struct _RegionHandle _temp895= _new_region();
struct _RegionHandle* rgn=& _temp895; _push_region( rgn);{ struct Cyc_List_List*
_temp896=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp892->tvs, _temp886); goto _LL897; _LL897: for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp870))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp898= Cyc_Tcexp_tcInjection( te, e1, _temp863,
rgn, _temp896, fields); goto _LL899; _LL899: if( _temp898 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp870))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp870))->injectors,({ struct Cyc_List_List* _temp900=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp900->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp898)); _temp900->tl= 0;
_temp900;}));}}}}}; _pop_region(& _temp895);} goto _LL877;}} _LL881: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp901="bad inject vararg type"; struct _tagged_string
_temp902; _temp902.curr= _temp901; _temp902.base= _temp901; _temp902.last_plus_one=
_temp901 + 23; _temp902;})); goto _LL877; _LL877:;}}}}}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp847))->v); return
_temp845; _LL834: return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp903="expected pointer to function";
struct _tagged_string _temp904; _temp904.curr= _temp903; _temp904.base= _temp903;
_temp904.last_plus_one= _temp903 + 29; _temp904;})); _LL830:;} _LL816: return
Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp905="expected pointer to function";
struct _tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 29; _temp906;})); _LL812:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp907= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_temp907.last_plus_one - _temp907.curr, _temp907.curr);}));} return Cyc_Absyn_wildtyp();}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp908= t1; struct Cyc_Absyn_PtrInfo
_temp914; struct Cyc_Absyn_Conref* _temp916; struct Cyc_Absyn_Tqual _temp918;
struct Cyc_Absyn_Conref* _temp920; void* _temp922; void* _temp924; _LL910: if((
unsigned int) _temp908 > 4u?*(( int*) _temp908) == Cyc_Absyn_PointerType: 0){
_LL915: _temp914=(( struct Cyc_Absyn_PointerType_struct*) _temp908)->f1; _LL925:
_temp924=( void*) _temp914.elt_typ; goto _LL923; _LL923: _temp922=( void*)
_temp914.rgn_typ; goto _LL921; _LL921: _temp920= _temp914.nullable; goto _LL919;
_LL919: _temp918= _temp914.tq; goto _LL917; _LL917: _temp916= _temp914.bounds;
goto _LL911;} else{ goto _LL912;} _LL912: goto _LL913; _LL911:{ void* _temp926=
Cyc_Tcutil_compress( _temp924); struct Cyc_Absyn_FnInfo _temp932; struct Cyc_List_List*
_temp934; struct Cyc_Absyn_VarargInfo* _temp936; int _temp938; struct Cyc_List_List*
_temp940; void* _temp942; struct Cyc_Core_Opt* _temp944; struct Cyc_List_List*
_temp946; _LL928: if(( unsigned int) _temp926 > 4u?*(( int*) _temp926) == Cyc_Absyn_FnType:
0){ _LL933: _temp932=(( struct Cyc_Absyn_FnType_struct*) _temp926)->f1; _LL947:
_temp946= _temp932.tvars; goto _LL945; _LL945: _temp944= _temp932.effect; goto
_LL943; _LL943: _temp942=( void*) _temp932.ret_typ; goto _LL941; _LL941:
_temp940= _temp932.args; goto _LL939; _LL939: _temp938= _temp932.c_varargs; goto
_LL937; _LL937: _temp936= _temp932.cyc_varargs; goto _LL935; _LL935: _temp934=
_temp932.attributes; goto _LL929;} else{ goto _LL930;} _LL930: goto _LL931;
_LL929: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp946)){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp948="type instantiation mismatch";
struct _tagged_string _temp949; _temp949.curr= _temp948; _temp949.base= _temp948;
_temp949.last_plus_one= _temp948 + 28; _temp949;}));} for( 0; ts != 0;( ts=((
struct Cyc_List_List*) _check_null( ts))->tl, _temp946=(( struct Cyc_List_List*)
_check_null( _temp946))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp946))->hd)->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({
struct Cyc_List_List* _temp950=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp950->hd=( void*)({ struct _tuple4* _temp951=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp951->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp946))->hd; _temp951->f2=( void*)((
struct Cyc_List_List*) _check_null( ts))->hd; _temp951;}); _temp950->tl=
instantiation; _temp950;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp955=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp955[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp956; _temp956.tag= Cyc_Absyn_FnType; _temp956.f1=({ struct Cyc_Absyn_FnInfo
_temp957; _temp957.tvars= 0; _temp957.effect= _temp944; _temp957.ret_typ=( void*)
_temp942; _temp957.args= _temp940; _temp957.c_varargs= _temp938; _temp957.cyc_varargs=
_temp936; _temp957.attributes= _temp934; _temp957;}); _temp956;}); _temp955;}));
void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp952=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp952[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp953; _temp953.tag= Cyc_Absyn_PointerType;
_temp953.f1=({ struct Cyc_Absyn_PtrInfo _temp954; _temp954.elt_typ=( void*)
new_fn_typ; _temp954.rgn_typ=( void*) _temp922; _temp954.nullable= _temp920;
_temp954.tq= _temp918; _temp954.bounds= _temp916; _temp954;}); _temp953;});
_temp952;}); return new_typ;}} _LL931: goto _LL927; _LL927:;} goto _LL909;
_LL913: goto _LL909; _LL909:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp958= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp958.last_plus_one - _temp958.curr, _temp958.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp959= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp960= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp959.last_plus_one - _temp959.curr, _temp959.curr, _temp960.last_plus_one -
_temp960.curr, _temp960.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** _temp961= 0; goto _LL962; _LL962: { struct Cyc_Absyn_Tqual
_temp963= Cyc_Absyn_empty_tqual(); goto _LL964; _LL964: if( topt != 0){ void*
_temp965= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp971; struct Cyc_Absyn_Conref* _temp973; struct Cyc_Absyn_Tqual _temp975;
struct Cyc_Absyn_Conref* _temp977; void* _temp979; void* _temp981; _LL967: if((
unsigned int) _temp965 > 4u?*(( int*) _temp965) == Cyc_Absyn_PointerType: 0){
_LL972: _temp971=(( struct Cyc_Absyn_PointerType_struct*) _temp965)->f1; _LL982:
_temp981=( void*) _temp971.elt_typ; goto _LL980; _LL980: _temp979=( void*)
_temp971.rgn_typ; goto _LL978; _LL978: _temp977= _temp971.nullable; goto _LL976;
_LL976: _temp975= _temp971.tq; goto _LL974; _LL974: _temp973= _temp971.bounds;
goto _LL968;} else{ goto _LL969;} _LL969: goto _LL970; _LL968: _temp961=({ void**
_temp983=( void**) GC_malloc( sizeof( void*)); _temp983[ 0]= _temp981; _temp983;});
_temp963= _temp975; goto _LL966; _LL970: goto _LL966; _LL966:;} Cyc_Tcexp_tcExpNoInst(
te, _temp961, e);{ void* _temp984=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp992; struct Cyc_List_List* _temp994; struct Cyc_Core_Opt* _temp996; struct
_tuple1* _temp998; struct Cyc_List_List* _temp1000; _LL986: if(*(( int*)
_temp984) == Cyc_Absyn_Struct_e){ _LL999: _temp998=(( struct Cyc_Absyn_Struct_e_struct*)
_temp984)->f1; goto _LL997; _LL997: _temp996=(( struct Cyc_Absyn_Struct_e_struct*)
_temp984)->f2; goto _LL995; _LL995: _temp994=(( struct Cyc_Absyn_Struct_e_struct*)
_temp984)->f3; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_Struct_e_struct*)
_temp984)->f4; goto _LL987;} else{ goto _LL988;} _LL988: if(*(( int*) _temp984)
== Cyc_Absyn_Tuple_e){ _LL1001: _temp1000=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp984)->f1; goto _LL989;} else{ goto _LL990;} _LL990: goto _LL991; _LL987:
goto _LL989; _LL989: Cyc_Tcutil_warn( loc,({ unsigned char* _temp1002="& used to allocate";
struct _tagged_string _temp1003; _temp1003.curr= _temp1002; _temp1003.base=
_temp1002; _temp1003.last_plus_one= _temp1002 + 19; _temp1003;}));{ void*(*
_temp1004)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
goto _LL1005; _LL1005: if( topt != 0){ void* _temp1006= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1012; struct Cyc_Absyn_Conref*
_temp1014; struct Cyc_Absyn_Tqual _temp1016; struct Cyc_Absyn_Conref* _temp1018;
void* _temp1020; void* _temp1022; _LL1008: if(( unsigned int) _temp1006 > 4u?*((
int*) _temp1006) == Cyc_Absyn_PointerType: 0){ _LL1013: _temp1012=(( struct Cyc_Absyn_PointerType_struct*)
_temp1006)->f1; _LL1023: _temp1022=( void*) _temp1012.elt_typ; goto _LL1021;
_LL1021: _temp1020=( void*) _temp1012.rgn_typ; goto _LL1019; _LL1019: _temp1018=
_temp1012.nullable; goto _LL1017; _LL1017: _temp1016= _temp1012.tq; goto _LL1015;
_LL1015: _temp1014= _temp1012.bounds; goto _LL1009;} else{ goto _LL1010;}
_LL1010: goto _LL1011; _LL1009: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1018)){ _temp1004= Cyc_Absyn_star_typ;} goto _LL1007; _LL1011: goto _LL1007;
_LL1007:;} return _temp1004(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp963);} _LL991: goto _LL985; _LL985:;}{ void*
_temp1026; int _temp1028; struct _tuple5 _temp1024= Cyc_Tcutil_addressof_props(
te, e); _LL1029: _temp1028= _temp1024.f1; goto _LL1027; _LL1027: _temp1026=
_temp1024.f2; goto _LL1025; _LL1025: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
if( _temp1028){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp1026, tq); return t;}}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1030= t;
struct Cyc_Absyn_PtrInfo _temp1036; struct Cyc_Absyn_Conref* _temp1038; struct
Cyc_Absyn_Tqual _temp1040; struct Cyc_Absyn_Conref* _temp1042; void* _temp1044;
void* _temp1046; _LL1032: if(( unsigned int) _temp1030 > 4u?*(( int*) _temp1030)
== Cyc_Absyn_PointerType: 0){ _LL1037: _temp1036=(( struct Cyc_Absyn_PointerType_struct*)
_temp1030)->f1; _LL1047: _temp1046=( void*) _temp1036.elt_typ; goto _LL1045;
_LL1045: _temp1044=( void*) _temp1036.rgn_typ; goto _LL1043; _LL1043: _temp1042=
_temp1036.nullable; goto _LL1041; _LL1041: _temp1040= _temp1036.tq; goto _LL1039;
_LL1039: _temp1038= _temp1036.bounds; goto _LL1033;} else{ goto _LL1034;}
_LL1034: goto _LL1035; _LL1033: Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1044); Cyc_Tcutil_check_nonzero_bound( loc, _temp1038); return _temp1046;
_LL1035: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1048=
Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp1048.last_plus_one - _temp1048.curr, _temp1048.curr);})); _LL1031:;}}
static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1049= t;
struct Cyc_Absyn_Structdecl** _temp1065; struct Cyc_Absyn_Structdecl* _temp1067;
struct Cyc_List_List* _temp1068; struct _tuple1* _temp1070; struct Cyc_List_List*
_temp1072; struct Cyc_List_List* _temp1074; struct Cyc_Absyn_Uniondecl**
_temp1076; struct Cyc_Absyn_Uniondecl* _temp1078; struct Cyc_List_List*
_temp1079; struct _tuple1* _temp1081; struct Cyc_Absyn_Exp* _temp1084; struct
Cyc_Absyn_Tqual _temp1086; void* _temp1088; struct Cyc_Absyn_PtrInfo _temp1093;
_LL1051: if(( unsigned int) _temp1049 > 4u?*(( int*) _temp1049) == Cyc_Absyn_StructType:
0){ _LL1071: _temp1070=(( struct Cyc_Absyn_StructType_struct*) _temp1049)->f1;
goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_StructType_struct*)
_temp1049)->f2; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_StructType_struct*)
_temp1049)->f3; if( _temp1065 == 0){ goto _LL1053;} else{ _temp1067=* _temp1065;
goto _LL1052;}} else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1049 > 4u?*((
int*) _temp1049) == Cyc_Absyn_AnonStructType: 0){ _LL1073: _temp1072=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1049)->f1; goto _LL1054;} else{ goto
_LL1055;} _LL1055: if(( unsigned int) _temp1049 > 4u?*(( int*) _temp1049) == Cyc_Absyn_AnonUnionType:
0){ _LL1075: _temp1074=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1049)->f1;
goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int) _temp1049 > 4u?*((
int*) _temp1049) == Cyc_Absyn_UnionType: 0){ _LL1082: _temp1081=(( struct Cyc_Absyn_UnionType_struct*)
_temp1049)->f1; goto _LL1080; _LL1080: _temp1079=(( struct Cyc_Absyn_UnionType_struct*)
_temp1049)->f2; goto _LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_UnionType_struct*)
_temp1049)->f3; if( _temp1076 == 0){ goto _LL1059;} else{ _temp1078=* _temp1076;
goto _LL1058;}} else{ goto _LL1059;} _LL1059: if(( unsigned int) _temp1049 > 4u?*((
int*) _temp1049) == Cyc_Absyn_ArrayType: 0){ _LL1089: _temp1088=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1049)->f1; goto _LL1087; _LL1087:
_temp1086=(( struct Cyc_Absyn_ArrayType_struct*) _temp1049)->f2; goto _LL1085;
_LL1085: _temp1084=(( struct Cyc_Absyn_ArrayType_struct*) _temp1049)->f3; goto
_LL1083;} else{ goto _LL1061;} _LL1083: if( Cyc_String_zstrcmp(* f,({
unsigned char* _temp1090="size"; struct _tagged_string _temp1091; _temp1091.curr=
_temp1090; _temp1091.base= _temp1090; _temp1091.last_plus_one= _temp1090 + 5;
_temp1091;})) == 0){ goto _LL1060;} else{ goto _LL1061;} _LL1061: if((
unsigned int) _temp1049 > 4u?*(( int*) _temp1049) == Cyc_Absyn_PointerType: 0){
_LL1094: _temp1093=(( struct Cyc_Absyn_PointerType_struct*) _temp1049)->f1; goto
_LL1092;} else{ goto _LL1063;} _LL1092: if( Cyc_String_zstrcmp(* f,({
unsigned char* _temp1095="size"; struct _tagged_string _temp1096; _temp1096.curr=
_temp1095; _temp1096.base= _temp1095; _temp1096.last_plus_one= _temp1095 + 5;
_temp1096;})) == 0){ goto _LL1062;} else{ goto _LL1063;} _LL1063: goto _LL1064;
_LL1052: if( _temp1070 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp1097="unresolved struct type (COMPILER ERROR)"; struct
_tagged_string _temp1098; _temp1098.curr= _temp1097; _temp1098.base= _temp1097;
_temp1098.last_plus_one= _temp1097 + 40; _temp1098;}));}{ struct Cyc_Absyn_Structfield*
_temp1099= Cyc_Absyn_lookup_struct_field( _temp1067, f); goto _LL1100; _LL1100:
if( _temp1099 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1101= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1070));
struct _tagged_string _temp1102=* f; xprintf("struct %.*s has no %.*s field",
_temp1101.last_plus_one - _temp1101.curr, _temp1101.curr, _temp1102.last_plus_one
- _temp1102.curr, _temp1102.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1103= _new_region(); struct _RegionHandle* rgn=& _temp1103; _push_region(
rgn);{ struct Cyc_List_List* _temp1104=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1067->tvs, _temp1068); goto _LL1105; _LL1105:
t2= Cyc_Tcutil_rsubstitute( rgn, _temp1104,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1099))->type);}; _pop_region(& _temp1103);} return t2;}}
_LL1054: { struct Cyc_Absyn_Structfield* _temp1106= Cyc_Absyn_lookup_field(
_temp1072, f); goto _LL1107; _LL1107: if( _temp1106 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1108=* f; xprintf("struct has no %.*s field",
_temp1108.last_plus_one - _temp1108.curr, _temp1108.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1106))->type;} _LL1056: {
struct Cyc_Absyn_Structfield* _temp1109= Cyc_Absyn_lookup_field( _temp1074, f);
goto _LL1110; _LL1110: if( _temp1109 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1111=* f; xprintf("union has no %.*s field",
_temp1111.last_plus_one - _temp1111.curr, _temp1111.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1109))->type;} _LL1058: if(
_temp1081 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp1112="unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp1113; _temp1113.curr= _temp1112; _temp1113.base=
_temp1112; _temp1113.last_plus_one= _temp1112 + 39; _temp1113;}));}{ struct Cyc_Absyn_Structfield*
_temp1114= Cyc_Absyn_lookup_union_field( _temp1078, f); goto _LL1115; _LL1115:
if( _temp1114 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1116= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1081));
struct _tagged_string _temp1117=* f; xprintf("union %.*s has no %.*s field",
_temp1116.last_plus_one - _temp1116.curr, _temp1116.curr, _temp1117.last_plus_one
- _temp1117.curr, _temp1117.curr);}));}{ struct Cyc_List_List* _temp1118=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1078->tvs, _temp1079); goto _LL1119; _LL1119: { void* _temp1120= Cyc_Tcutil_substitute(
_temp1118,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1114))->type);
goto _LL1121; _LL1121: return _temp1120;}}} _LL1060:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1122=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1122[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1123; _temp1123.tag= Cyc_Absyn_Primop_e;
_temp1123.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1123.f2=({ struct Cyc_List_List*
_temp1124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1124->hd=( void*) e; _temp1124->tl= 0; _temp1124;}); _temp1123;});
_temp1122;}))); return Cyc_Absyn_uint_t; _LL1062:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1125=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1125[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1126; _temp1126.tag= Cyc_Absyn_Primop_e;
_temp1126.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1126.f2=({ struct Cyc_List_List*
_temp1127=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1127->hd=( void*) e; _temp1127->tl= 0; _temp1127;}); _temp1126;});
_temp1125;}))); return Cyc_Absyn_uint_t; _LL1064: if( Cyc_String_zstrcmp(* f,({
unsigned char* _temp1128="size"; struct _tagged_string _temp1129; _temp1129.curr=
_temp1128; _temp1129.base= _temp1128; _temp1129.last_plus_one= _temp1128 + 5;
_temp1129;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1130= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1130.last_plus_one - _temp1130.curr, _temp1130.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1131= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1131.last_plus_one - _temp1131.curr, _temp1131.curr);}));} _LL1050:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1132= t; struct Cyc_Absyn_PtrInfo
_temp1138; struct Cyc_Absyn_Conref* _temp1140; struct Cyc_Absyn_Tqual _temp1142;
struct Cyc_Absyn_Conref* _temp1144; void* _temp1146; void* _temp1148; _LL1134:
if(( unsigned int) _temp1132 > 4u?*(( int*) _temp1132) == Cyc_Absyn_PointerType:
0){ _LL1139: _temp1138=(( struct Cyc_Absyn_PointerType_struct*) _temp1132)->f1;
_LL1149: _temp1148=( void*) _temp1138.elt_typ; goto _LL1147; _LL1147: _temp1146=(
void*) _temp1138.rgn_typ; goto _LL1145; _LL1145: _temp1144= _temp1138.nullable;
goto _LL1143; _LL1143: _temp1142= _temp1138.tq; goto _LL1141; _LL1141: _temp1140=
_temp1138.bounds; goto _LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137;
_LL1135: Cyc_Tcutil_check_nonzero_bound( loc, _temp1140);{ void* _temp1150= Cyc_Tcutil_compress(
_temp1148); struct Cyc_Absyn_Structdecl** _temp1162; struct Cyc_Absyn_Structdecl*
_temp1164; struct Cyc_List_List* _temp1165; struct _tuple1* _temp1167; struct
Cyc_Absyn_Uniondecl** _temp1169; struct Cyc_Absyn_Uniondecl* _temp1171; struct
Cyc_List_List* _temp1172; struct _tuple1* _temp1174; struct Cyc_List_List*
_temp1176; struct Cyc_List_List* _temp1178; _LL1152: if(( unsigned int)
_temp1150 > 4u?*(( int*) _temp1150) == Cyc_Absyn_StructType: 0){ _LL1168:
_temp1167=(( struct Cyc_Absyn_StructType_struct*) _temp1150)->f1; goto _LL1166;
_LL1166: _temp1165=(( struct Cyc_Absyn_StructType_struct*) _temp1150)->f2; goto
_LL1163; _LL1163: _temp1162=(( struct Cyc_Absyn_StructType_struct*) _temp1150)->f3;
if( _temp1162 == 0){ goto _LL1154;} else{ _temp1164=* _temp1162; goto _LL1153;}}
else{ goto _LL1154;} _LL1154: if(( unsigned int) _temp1150 > 4u?*(( int*)
_temp1150) == Cyc_Absyn_UnionType: 0){ _LL1175: _temp1174=(( struct Cyc_Absyn_UnionType_struct*)
_temp1150)->f1; goto _LL1173; _LL1173: _temp1172=(( struct Cyc_Absyn_UnionType_struct*)
_temp1150)->f2; goto _LL1170; _LL1170: _temp1169=(( struct Cyc_Absyn_UnionType_struct*)
_temp1150)->f3; if( _temp1169 == 0){ goto _LL1156;} else{ _temp1171=* _temp1169;
goto _LL1155;}} else{ goto _LL1156;} _LL1156: if(( unsigned int) _temp1150 > 4u?*((
int*) _temp1150) == Cyc_Absyn_AnonStructType: 0){ _LL1177: _temp1176=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1150)->f1; goto _LL1157;} else{ goto
_LL1158;} _LL1158: if(( unsigned int) _temp1150 > 4u?*(( int*) _temp1150) == Cyc_Absyn_AnonUnionType:
0){ _LL1179: _temp1178=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1150)->f1;
goto _LL1159;} else{ goto _LL1160;} _LL1160: goto _LL1161; _LL1153: { struct Cyc_Absyn_Structfield*
_temp1180= Cyc_Absyn_lookup_struct_field( _temp1164, f); goto _LL1181; _LL1181:
if( _temp1180 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1182= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1167));
struct _tagged_string _temp1183=* f; xprintf("struct %.*s has no %.*s field",
_temp1182.last_plus_one - _temp1182.curr, _temp1182.curr, _temp1183.last_plus_one
- _temp1183.curr, _temp1183.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1184= _new_region(); struct _RegionHandle* rgn=& _temp1184; _push_region(
rgn);{ struct Cyc_List_List* _temp1185=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1164->tvs, _temp1165); goto _LL1186; _LL1186:
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1185,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1180))->type);}; _pop_region(& _temp1184);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1146); return t3;}} _LL1155: { struct Cyc_Absyn_Structfield*
_temp1187= Cyc_Absyn_lookup_union_field( _temp1171, f); goto _LL1188; _LL1188:
if( _temp1187 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1189= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1174));
struct _tagged_string _temp1190=* f; xprintf("union %.*s has no %.*s field",
_temp1189.last_plus_one - _temp1189.curr, _temp1189.curr, _temp1190.last_plus_one
- _temp1190.curr, _temp1190.curr);}));}{ struct Cyc_List_List* _temp1191=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1171->tvs, _temp1172); goto _LL1192; _LL1192: { void* _temp1193= Cyc_Tcutil_substitute(
_temp1191,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1187))->type);
goto _LL1194; _LL1194: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1146);
return _temp1193;}}} _LL1157: { struct Cyc_Absyn_Structfield* _temp1195= Cyc_Absyn_lookup_field(
_temp1176, f); goto _LL1196; _LL1196: if( _temp1195 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1197=* f; xprintf("struct has no %.*s field",
_temp1197.last_plus_one - _temp1197.curr, _temp1197.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1195))->type;} _LL1159: {
struct Cyc_Absyn_Structfield* _temp1198= Cyc_Absyn_lookup_field( _temp1178, f);
goto _LL1199; _LL1199: if( _temp1198 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1200=* f; xprintf("union has no %.*s field",
_temp1200.last_plus_one - _temp1200.curr, _temp1200.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1198))->type;} _LL1161: return
Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1201= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp1201.last_plus_one -
_temp1201.curr, _temp1201.curr);})); _LL1151:;} _LL1137: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1202= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1202.last_plus_one - _temp1202.curr, _temp1202.curr);})); _LL1133:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1203; _push_handler(& _temp1203);{ int
_temp1205= 0; if( setjmp( _temp1203.handler)){ _temp1205= 1;} if( ! _temp1205){{
void* _temp1206=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1206;}; _pop_handler();} else{
void* _temp1204=( void*) _exn_thrown; void* _temp1208= _temp1204; _LL1210: if(
_temp1208 == Cyc_List_Nth){ goto _LL1211;} else{ goto _LL1212;} _LL1212: goto
_LL1213; _LL1211: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1213:(
void) _throw( _temp1208); _LL1209:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string _temp1214= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int subscript, found %.*s", _temp1214.last_plus_one -
_temp1214.curr, _temp1214.curr);}));}{ void* _temp1215= t1; struct Cyc_Absyn_PtrInfo
_temp1223; struct Cyc_Absyn_Conref* _temp1225; struct Cyc_Absyn_Tqual _temp1227;
struct Cyc_Absyn_Conref* _temp1229; void* _temp1231; void* _temp1233; struct Cyc_List_List*
_temp1235; _LL1217: if(( unsigned int) _temp1215 > 4u?*(( int*) _temp1215) ==
Cyc_Absyn_PointerType: 0){ _LL1224: _temp1223=(( struct Cyc_Absyn_PointerType_struct*)
_temp1215)->f1; _LL1234: _temp1233=( void*) _temp1223.elt_typ; goto _LL1232;
_LL1232: _temp1231=( void*) _temp1223.rgn_typ; goto _LL1230; _LL1230: _temp1229=
_temp1223.nullable; goto _LL1228; _LL1228: _temp1227= _temp1223.tq; goto _LL1226;
_LL1226: _temp1225= _temp1223.bounds; goto _LL1218;} else{ goto _LL1219;}
_LL1219: if(( unsigned int) _temp1215 > 4u?*(( int*) _temp1215) == Cyc_Absyn_TupleType:
0){ _LL1236: _temp1235=(( struct Cyc_Absyn_TupleType_struct*) _temp1215)->f1;
goto _LL1220;} else{ goto _LL1221;} _LL1221: goto _LL1222; _LL1218: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1225);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1225);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1231); return _temp1233; _LL1220: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1235, e2); _LL1222: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1237= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1237.last_plus_one - _temp1237.curr, _temp1237.curr);})); _LL1216:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1238= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1244; _LL1240: if((
unsigned int) _temp1238 > 4u?*(( int*) _temp1238) == Cyc_Absyn_TupleType: 0){
_LL1245: _temp1244=(( struct Cyc_Absyn_TupleType_struct*) _temp1238)->f1; goto
_LL1241;} else{ goto _LL1242;} _LL1242: goto _LL1243; _LL1241: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1244) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp1246="tuple expression has the wrong number of fields";
struct _tagged_string _temp1247; _temp1247.curr= _temp1246; _temp1247.base=
_temp1246; _temp1247.last_plus_one= _temp1246 + 48; _temp1247;})); goto _LL1239;}
for( 0; es != 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1244=((
struct Cyc_List_List*) _check_null( _temp1244))->tl)){ void* _temp1248=(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp1244))->hd)).f2;
goto _LL1249; _LL1249: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1248,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1250=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1250->hd=( void*)({ struct _tuple7* _temp1251=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1251->f1=(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( _temp1244))->hd)).f1; _temp1251->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1251;}); _temp1250->tl=
fields; _temp1250;});} done= 1; goto _LL1239; _LL1243: goto _LL1239; _LL1239:;}
if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1252=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1252->hd=( void*)({ struct
_tuple7* _temp1253=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1253->f1= Cyc_Absyn_empty_tqual(); _temp1253->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1253;}); _temp1252->tl= fields; _temp1252;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1254=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1254[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1255; _temp1255.tag= Cyc_Absyn_TupleType;
_temp1255.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1255;}); _temp1254;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,({
unsigned char* _temp1256="tcCompoundLit"; struct _tagged_string _temp1257;
_temp1257.curr= _temp1256; _temp1257.base= _temp1256; _temp1257.last_plus_one=
_temp1256 + 14; _temp1257;}));} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_Absyn_Const_e_struct* _temp1262=({ struct Cyc_Absyn_Const_e_struct*
_temp1258=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1258[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1259; _temp1259.tag= Cyc_Absyn_Const_e;
_temp1259.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1260=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1260[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1261; _temp1261.tag= Cyc_Absyn_Int_c;
_temp1261.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1261.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1261;}); _temp1260;}));
_temp1259;}); _temp1258;}); goto _LL1263; _LL1263: { struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1262, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1264=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1264->v=( void*) Cyc_Absyn_uint_t; _temp1264;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1278=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1278[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1279; _temp1279.tag= Cyc_Absyn_ArrayType;
_temp1279.f1=( void*) res; _temp1279.f2= Cyc_Absyn_empty_tqual(); _temp1279.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1279;}); _temp1278;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp1265= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1265.last_plus_one - _temp1265.curr, _temp1265.curr);}));}{ int offset= 0;
for( 0; des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1266=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_string* _temp1272; struct Cyc_Absyn_Exp*
_temp1274; _LL1268: if(*(( int*) _temp1266) == Cyc_Absyn_FieldName){ _LL1273:
_temp1272=(( struct Cyc_Absyn_FieldName_struct*) _temp1266)->f1; goto _LL1269;}
else{ goto _LL1270;} _LL1270: if(*(( int*) _temp1266) == Cyc_Absyn_ArrayElement){
_LL1275: _temp1274=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1266)->f1;
goto _LL1271;} else{ goto _LL1267;} _LL1269: Cyc_Tcutil_terr( loc,({
unsigned char* _temp1276="only array index designators are supported"; struct
_tagged_string _temp1277; _temp1277.curr= _temp1276; _temp1277.base= _temp1276;
_temp1277.last_plus_one= _temp1276 + 43; _temp1277;})); goto _LL1267; _LL1271:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1274);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1274); if( i != offset){ Cyc_Tcutil_terr( _temp1274->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1267;} _LL1267:;}}} return res_t2;}}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1280= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_temp1280.last_plus_one - _temp1280.curr, _temp1280.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp1281="comprehension index variable is not declared const!";
struct _tagged_string _temp1282; _temp1282.curr= _temp1281; _temp1282.base=
_temp1281; _temp1282.last_plus_one= _temp1281 + 52; _temp1282;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,({
unsigned char* _temp1283="bound is not constant"; struct _tagged_string
_temp1284; _temp1284.curr= _temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one=
_temp1283 + 22; _temp1284;}));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc,({ unsigned char* _temp1285="body is not constant"; struct
_tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base= _temp1285;
_temp1286.last_plus_one= _temp1285 + 21; _temp1286;}));}}{ struct Cyc_Absyn_PtrInfo
pinfo; void** _temp1287= 0; goto _LL1288; _LL1288: { struct Cyc_Absyn_Tqual*
_temp1289= 0; goto _LL1290; _LL1290: if( topt != 0){ void* _temp1291= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1299; struct Cyc_Absyn_Exp*
_temp1301; struct Cyc_Absyn_Tqual _temp1303; struct Cyc_Absyn_Tqual* _temp1305;
void* _temp1306; void** _temp1308; _LL1293: if(( unsigned int) _temp1291 > 4u?*((
int*) _temp1291) == Cyc_Absyn_PointerType: 0){ _LL1300: _temp1299=(( struct Cyc_Absyn_PointerType_struct*)
_temp1291)->f1; goto _LL1294;} else{ goto _LL1295;} _LL1295: if(( unsigned int)
_temp1291 > 4u?*(( int*) _temp1291) == Cyc_Absyn_ArrayType: 0){ _LL1307:
_temp1306=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1291)->f1;
_temp1308=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1291)->f1); goto
_LL1304; _LL1304: _temp1303=(( struct Cyc_Absyn_ArrayType_struct*) _temp1291)->f2;
_temp1305=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1291)->f2; goto _LL1302;
_LL1302: _temp1301=(( struct Cyc_Absyn_ArrayType_struct*) _temp1291)->f3; goto
_LL1296;} else{ goto _LL1297;} _LL1297: goto _LL1298; _LL1294: pinfo= _temp1299;
_temp1287=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1289=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1292; _LL1296: _temp1287=( void**) _temp1308; _temp1289=(
struct Cyc_Absyn_Tqual*) _temp1305; goto _LL1292; _LL1298: goto _LL1292; _LL1292:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1287, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1309=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1309[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1310; _temp1310.tag= Cyc_Absyn_ArrayType;
_temp1310.f1=( void*) t; _temp1310.f2= _temp1289 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1289)); _temp1310.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1310;}); _temp1309;});}}}} struct _tuple8{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1311; _push_handler(&
_temp1311);{ int _temp1313= 0; if( setjmp( _temp1311.handler)){ _temp1313= 1;}
if( ! _temp1313){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1312=( void*) _exn_thrown; void* _temp1315= _temp1312; _LL1317:
if( _temp1315 == Cyc_Dict_Absent){ goto _LL1318;} else{ goto _LL1319;} _LL1319:
goto _LL1320; _LL1318: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1321=
Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s", _temp1321.last_plus_one
- _temp1321.curr, _temp1321.curr);})); return topt != 0?*(( void**) _check_null(
topt)):( void*) Cyc_Absyn_VoidType; _LL1320:( void) _throw( _temp1315); _LL1316:;}}}*
sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=( struct
_tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{ struct
_RegionHandle _temp1322= _new_region(); struct _RegionHandle* rgn=& _temp1322;
_push_region( rgn);{ struct Cyc_List_List* _temp1323=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); goto _LL1324; _LL1324: { struct Cyc_List_List* _temp1325=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1323); goto
_LL1326; _LL1326: { struct Cyc_Absyn_StructType_struct* _temp1330=({ struct Cyc_Absyn_StructType_struct*
_temp1327=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1327[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1328; _temp1328.tag=
Cyc_Absyn_StructType; _temp1328.f1=( struct _tuple1*)* tn; _temp1328.f2=
_temp1325; _temp1328.f3=({ struct Cyc_Absyn_Structdecl** _temp1329=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1329[ 0]= sd; _temp1329;});
_temp1328;}); _temp1327;}); goto _LL1331; _LL1331: if( topt != 0){ Cyc_Tcutil_unify((
void*) _temp1330,*(( void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt*
_temp1332=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1332->v=( void*) _temp1325; _temp1332;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp1333="can't build abstract struct"; struct
_tagged_string _temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333;
_temp1334.last_plus_one= _temp1333 + 28; _temp1334;}));{ void* _temp1335=( void*)
_temp1330; _npop_handler( 0u); return _temp1335;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple8 _temp1338; struct Cyc_Absyn_Exp* _temp1339; struct Cyc_Absyn_Structfield*
_temp1341; struct _tuple8* _temp1336=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1338=* _temp1336; _LL1342: _temp1341= _temp1338.f1;
goto _LL1340; _LL1340: _temp1339= _temp1338.f2; goto _LL1337; _LL1337: { void*
_temp1343= Cyc_Tcutil_rsubstitute( rgn, _temp1323,( void*) _temp1341->type);
goto _LL1344; _LL1344: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1343,
_temp1339); if( ! Cyc_Tcutil_coerce_arg( te, _temp1339, _temp1343)){ Cyc_Tcutil_terr(
_temp1339->loc,({ struct _tagged_string _temp1345=* _temp1341->name; struct
_tagged_string _temp1346= Cyc_Absynpp_qvar2string(* tn); struct _tagged_string
_temp1347= Cyc_Absynpp_typ2string( _temp1343); struct _tagged_string _temp1348=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1339->topt))->v);
xprintf("field %.*s of struct %.*s expects type %.*s != %.*s", _temp1345.last_plus_one
- _temp1345.curr, _temp1345.curr, _temp1346.last_plus_one - _temp1346.curr,
_temp1346.curr, _temp1347.last_plus_one - _temp1347.curr, _temp1347.curr,
_temp1348.last_plus_one - _temp1348.curr, _temp1348.curr);}));}}}{ void*
_temp1349=( void*) _temp1330; _npop_handler( 0u); return _temp1349;}}}}};
_pop_region(& _temp1322);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1350= _new_region(); struct _RegionHandle* rgn=&
_temp1350; _push_region( rgn);{ void* _temp1351= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1357; _LL1353: if(( unsigned int) _temp1351 > 4u?*((
int*) _temp1351) == Cyc_Absyn_AnonStructType: 0){ _LL1358: _temp1357=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1351)->f1; goto _LL1354;} else{ goto
_LL1355;} _LL1355: goto _LL1356; _LL1354: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1357); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple8 _temp1361; struct Cyc_Absyn_Exp*
_temp1362; struct Cyc_Absyn_Structfield* _temp1364; struct _tuple8* _temp1359=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1361=*
_temp1359; _LL1365: _temp1364= _temp1361.f1; goto _LL1363; _LL1363: _temp1362=
_temp1361.f2; goto _LL1360; _LL1360: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1364->type)), _temp1362); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1362,( void*) _temp1364->type)){ Cyc_Tcutil_terr( _temp1362->loc,({ struct
_tagged_string _temp1366=* _temp1364->name; struct _tagged_string _temp1367= Cyc_Absynpp_typ2string((
void*) _temp1364->type); struct _tagged_string _temp1368= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1362->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1366.last_plus_one - _temp1366.curr, _temp1366.curr, _temp1367.last_plus_one
- _temp1367.curr, _temp1367.curr, _temp1368.last_plus_one - _temp1368.curr,
_temp1368.curr);}));}} goto _LL1352;} _LL1356:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp1369="tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1370; _temp1370.curr= _temp1369; _temp1370.base=
_temp1369; _temp1370.last_plus_one= _temp1369 + 37; _temp1370;})); goto _LL1352;
_LL1352:;}; _pop_region(& _temp1350);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1371= _new_region(); struct _RegionHandle* rgn=&
_temp1371; _push_region( rgn);{ struct Cyc_List_List* _temp1372=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tud->tvs); goto _LL1373; _LL1373: { struct Cyc_List_List* _temp1374=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn, tuf->tvs); goto
_LL1375; _LL1375: { struct Cyc_List_List* _temp1376=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, _temp1372); goto _LL1377; _LL1377: { struct Cyc_List_List*
_temp1378=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1374); goto
_LL1379; _LL1379: { struct Cyc_List_List* _temp1380=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp1372, _temp1374); goto _LL1381; _LL1381:* all_ref=({ struct Cyc_Core_Opt*
_temp1382=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1382->v=( void*) _temp1376; _temp1382;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1383=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1383->v=( void*) _temp1378; _temp1383;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1419=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1419[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1420; _temp1420.tag= Cyc_Absyn_TunionFieldType; _temp1420.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1421; _temp1421.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1422=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1422[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1423; _temp1423.tag= Cyc_Absyn_KnownTunionfield; _temp1423.f1= tud;
_temp1423.f2= tuf; _temp1423;}); _temp1422;})); _temp1421.targs= _temp1376;
_temp1421;}); _temp1420;}); _temp1419;}); if( topt != 0){ void* _temp1384= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo _temp1392; struct
Cyc_Absyn_TunionInfo _temp1394; void* _temp1396; struct Cyc_List_List* _temp1398;
void* _temp1400; _LL1386: if(( unsigned int) _temp1384 > 4u?*(( int*) _temp1384)
== Cyc_Absyn_TunionFieldType: 0){ _LL1393: _temp1392=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1384)->f1; goto _LL1387;} else{ goto _LL1388;} _LL1388: if(( unsigned int)
_temp1384 > 4u?*(( int*) _temp1384) == Cyc_Absyn_TunionType: 0){ _LL1395:
_temp1394=(( struct Cyc_Absyn_TunionType_struct*) _temp1384)->f1; _LL1401:
_temp1400=( void*) _temp1394.tunion_info; goto _LL1399; _LL1399: _temp1398=
_temp1394.targs; goto _LL1397; _LL1397: _temp1396=( void*) _temp1394.rgn; goto
_LL1389;} else{ goto _LL1390;} _LL1390: goto _LL1391; _LL1387: Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res); goto _LL1385; _LL1389:{ struct Cyc_List_List*
a= _temp1376; for( 0; a != 0? _temp1398 != 0: 0;( a=(( struct Cyc_List_List*)
_check_null( a))->tl, _temp1398=(( struct Cyc_List_List*) _check_null( _temp1398))->tl)){
Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,( void*)((
struct Cyc_List_List*) _check_null( _temp1398))->hd);}} if( tuf->typs == 0? es
== 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1402=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1402->v=( void*) res; _temp1402;});
res=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp1403=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1403[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1404; _temp1404.tag= Cyc_Absyn_TunionType;
_temp1404.f1=({ struct Cyc_Absyn_TunionInfo _temp1405; _temp1405.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1406=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1406[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1407; _temp1407.tag= Cyc_Absyn_KnownTunion;
_temp1407.f1= tud; _temp1407;}); _temp1406;})); _temp1405.targs= _temp1376;
_temp1405.rgn=( void*) _temp1396; _temp1405;}); _temp1404;}); _temp1403;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1408= res; _npop_handler( 0u); return _temp1408;}} goto
_LL1385; _LL1391: goto _LL1385; _LL1385:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es != 0? ts != 0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1380,(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1411= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1412= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1413= e->topt == 0?({ unsigned char* _temp1409="?"; struct _tagged_string
_temp1410; _temp1410.curr= _temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one=
_temp1409 + 2; _temp1410;}): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1411.last_plus_one - _temp1411.curr, _temp1411.curr, _temp1412.last_plus_one
- _temp1412.curr, _temp1412.curr, _temp1413.last_plus_one - _temp1413.curr,
_temp1413.curr);}));}} if( es != 0){ void* _temp1415= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp1414= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1414.last_plus_one - _temp1414.curr, _temp1414.curr);})); _npop_handler( 0u);
return _temp1415;} if( ts != 0){ void* _temp1417= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1416= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1416.last_plus_one - _temp1416.curr, _temp1416.curr);})); _npop_handler( 0u);
return _temp1417;}{ void* _temp1418= res; _npop_handler( 0u); return _temp1418;}}}}}}}};
_pop_region(& _temp1371);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1433=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1433[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1434; _temp1434.tag=
Cyc_Absyn_RgnHandleType; _temp1434.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1434;}); _temp1433;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void* _temp1424= Cyc_Tcutil_compress(
handle_type); void* _temp1430; _LL1426: if(( unsigned int) _temp1424 > 4u?*((
int*) _temp1424) == Cyc_Absyn_RgnHandleType: 0){ _LL1431: _temp1430=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1424)->f1; goto _LL1427;} else{
goto _LL1428;} _LL1428: goto _LL1429; _LL1427: rgn= _temp1430; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1425; _LL1429: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc,({ struct _tagged_string _temp1432= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1432.last_plus_one
- _temp1432.curr, _temp1432.curr);})); goto _LL1425; _LL1425:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1435= t; struct Cyc_Absyn_TunionFieldInfo _temp1441; struct Cyc_List_List*
_temp1443; void* _temp1445; struct Cyc_Absyn_Tunionfield* _temp1447; struct Cyc_Absyn_Tuniondecl*
_temp1449; _LL1437: if(( unsigned int) _temp1435 > 4u?*(( int*) _temp1435) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1442: _temp1441=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1435)->f1; _LL1446: _temp1445=( void*) _temp1441.field_info; if(*(( int*)
_temp1445) == Cyc_Absyn_KnownTunionfield){ _LL1450: _temp1449=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1445)->f1; goto _LL1448; _LL1448: _temp1447=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1445)->f2; goto _LL1444;} else{ goto _LL1439;} _LL1444: _temp1443=
_temp1441.targs; goto _LL1438;} else{ goto _LL1439;} _LL1439: goto _LL1440;
_LL1438: if( _temp1447->tvs != 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp1451="malloc with existential types not yet implemented"; struct
_tagged_string _temp1452; _temp1452.curr= _temp1451; _temp1452.base= _temp1451;
_temp1452.last_plus_one= _temp1451 + 50; _temp1452;}));} goto _LL1436; _LL1440:
goto _LL1436; _LL1436:;}{ void*(* _temp1453)( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq)= Cyc_Absyn_at_typ; goto _LL1454; _LL1454: if( topt != 0){ void* _temp1455=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1461; struct Cyc_Absyn_Conref* _temp1463; struct Cyc_Absyn_Tqual _temp1465;
struct Cyc_Absyn_Conref* _temp1467; void* _temp1469; void* _temp1471; _LL1457:
if(( unsigned int) _temp1455 > 4u?*(( int*) _temp1455) == Cyc_Absyn_PointerType:
0){ _LL1462: _temp1461=(( struct Cyc_Absyn_PointerType_struct*) _temp1455)->f1;
_LL1472: _temp1471=( void*) _temp1461.elt_typ; goto _LL1470; _LL1470: _temp1469=(
void*) _temp1461.rgn_typ; goto _LL1468; _LL1468: _temp1467= _temp1461.nullable;
goto _LL1466; _LL1466: _temp1465= _temp1461.tq; goto _LL1464; _LL1464: _temp1463=
_temp1461.bounds; goto _LL1458;} else{ goto _LL1459;} _LL1459: goto _LL1460;
_LL1458: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1467)){ _temp1453= Cyc_Absyn_star_typ;} goto _LL1456; _LL1460: goto _LL1456;
_LL1456:;} return _temp1453( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1473=( void*) s->r; struct Cyc_Absyn_Exp* _temp1483;
struct Cyc_Absyn_Stmt* _temp1485; struct Cyc_Absyn_Stmt* _temp1487; struct Cyc_Absyn_Stmt*
_temp1489; struct Cyc_Absyn_Decl* _temp1491; _LL1475: if(( unsigned int)
_temp1473 > 1u?*(( int*) _temp1473) == Cyc_Absyn_Exp_s: 0){ _LL1484: _temp1483=((
struct Cyc_Absyn_Exp_s_struct*) _temp1473)->f1; goto _LL1476;} else{ goto
_LL1477;} _LL1477: if(( unsigned int) _temp1473 > 1u?*(( int*) _temp1473) == Cyc_Absyn_Seq_s:
0){ _LL1488: _temp1487=(( struct Cyc_Absyn_Seq_s_struct*) _temp1473)->f1; goto
_LL1486; _LL1486: _temp1485=(( struct Cyc_Absyn_Seq_s_struct*) _temp1473)->f2;
goto _LL1478;} else{ goto _LL1479;} _LL1479: if(( unsigned int) _temp1473 > 1u?*((
int*) _temp1473) == Cyc_Absyn_Decl_s: 0){ _LL1492: _temp1491=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1473)->f1; goto _LL1490; _LL1490: _temp1489=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1473)->f2; goto _LL1480;} else{ goto _LL1481;} _LL1481: goto _LL1482;
_LL1476: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1483->topt))->v;
_LL1478: s= _temp1485; continue; _LL1480: s= _temp1489; continue; _LL1482:
return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp1493="statement expression must end with expression";
struct _tagged_string _temp1494; _temp1494.curr= _temp1493; _temp1494.base=
_temp1493; _temp1494.last_plus_one= _temp1493 + 46; _temp1494;})); _LL1474:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,({ unsigned char* _temp1495="tcCodegen"; struct _tagged_string _temp1496;
_temp1496.curr= _temp1495; _temp1496.base= _temp1495; _temp1496.last_plus_one=
_temp1495 + 10; _temp1496;}));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc,({ unsigned char* _temp1497="tcFill"; struct
_tagged_string _temp1498; _temp1498.curr= _temp1497; _temp1498.base= _temp1497;
_temp1498.last_plus_one= _temp1497 + 7; _temp1498;}));} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle != 0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1508=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1508[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1509; _temp1509.tag= Cyc_Absyn_RgnHandleType;
_temp1509.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp1509;});
_temp1508;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,(
struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void* _temp1499= Cyc_Tcutil_compress(
handle_type); void* _temp1505; _LL1501: if(( unsigned int) _temp1499 > 4u?*((
int*) _temp1499) == Cyc_Absyn_RgnHandleType: 0){ _LL1506: _temp1505=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1499)->f1; goto _LL1502;} else{
goto _LL1503;} _LL1503: goto _LL1504; _LL1502: rgn= _temp1505; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1500; _LL1504: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle))->loc,({ struct _tagged_string _temp1507= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1507.last_plus_one
- _temp1507.curr, _temp1507.curr);})); goto _LL1500; _LL1500:;}{ void* _temp1510=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1522; struct Cyc_Absyn_Exp* _temp1524;
struct Cyc_Absyn_Vardecl* _temp1526; struct Cyc_List_List* _temp1528; struct Cyc_Core_Opt*
_temp1530; struct Cyc_List_List* _temp1532; void* _temp1534; struct
_tagged_string _temp1536; _LL1512: if(*(( int*) _temp1510) == Cyc_Absyn_Comprehension_e){
_LL1527: _temp1526=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1510)->f1;
goto _LL1525; _LL1525: _temp1524=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1510)->f2; goto _LL1523; _LL1523: _temp1522=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1510)->f3; goto _LL1513;} else{ goto _LL1514;} _LL1514: if(*(( int*)
_temp1510) == Cyc_Absyn_UnresolvedMem_e){ _LL1531: _temp1530=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1510)->f1; goto _LL1529; _LL1529: _temp1528=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1510)->f2; goto _LL1515;} else{ goto _LL1516;} _LL1516: if(*(( int*)
_temp1510) == Cyc_Absyn_Array_e){ _LL1533: _temp1532=(( struct Cyc_Absyn_Array_e_struct*)
_temp1510)->f1; goto _LL1517;} else{ goto _LL1518;} _LL1518: if(*(( int*)
_temp1510) == Cyc_Absyn_Const_e){ _LL1535: _temp1534=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1510)->f1; if(( unsigned int) _temp1534 > 1u?*(( int*) _temp1534) == Cyc_Absyn_String_c:
0){ _LL1537: _temp1536=(( struct Cyc_Absyn_String_c_struct*) _temp1534)->f1;
goto _LL1519;} else{ goto _LL1520;}} else{ goto _LL1520;} _LL1520: goto _LL1521;
_LL1513: { void* _temp1538= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); goto
_LL1539; _LL1539: { void* _temp1540= Cyc_Tcutil_compress( _temp1538); struct Cyc_Absyn_Exp*
_temp1546; struct Cyc_Absyn_Tqual _temp1548; void* _temp1550; _LL1542: if((
unsigned int) _temp1540 > 4u?*(( int*) _temp1540) == Cyc_Absyn_ArrayType: 0){
_LL1551: _temp1550=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1540)->f1;
goto _LL1549; _LL1549: _temp1548=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1540)->f2; goto _LL1547; _LL1547: _temp1546=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1540)->f3; goto _LL1543;} else{ goto _LL1544;} _LL1544: goto _LL1545;
_LL1543: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1524)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1556=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1556[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1557; _temp1557.tag= Cyc_Absyn_Upper_b; _temp1557.f1=
_temp1524; _temp1557;}); _temp1556;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1553=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1553[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1554; _temp1554.tag= Cyc_Absyn_PointerType;
_temp1554.f1=({ struct Cyc_Absyn_PtrInfo _temp1555; _temp1555.elt_typ=( void*)
_temp1550; _temp1555.rgn_typ=( void*) rgn; _temp1555.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1555.tq= _temp1548; _temp1555.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1555;});
_temp1554;}); _temp1553;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1552=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1552->v=(
void*) res_typ; _temp1552;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1545: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1558="tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1559; _temp1559.curr= _temp1558; _temp1559.base= _temp1558;
_temp1559.last_plus_one= _temp1558 + 45; _temp1559;})); _LL1541:;}} _LL1515:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1560=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1560[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1561; _temp1561.tag= Cyc_Absyn_Array_e;
_temp1561.f1= _temp1528; _temp1561;}); _temp1560;}))); _temp1532= _temp1528;
goto _LL1517; _LL1517: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1562=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1568; struct Cyc_Absyn_Conref* _temp1570; struct Cyc_Absyn_Tqual _temp1572;
struct Cyc_Absyn_Conref* _temp1574; void* _temp1576; void* _temp1578; void**
_temp1580; _LL1564: if(( unsigned int) _temp1562 > 4u?*(( int*) _temp1562) ==
Cyc_Absyn_PointerType: 0){ _LL1569: _temp1568=(( struct Cyc_Absyn_PointerType_struct*)
_temp1562)->f1; _LL1579: _temp1578=( void*) _temp1568.elt_typ; _temp1580=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1562)->f1).elt_typ; goto _LL1577;
_LL1577: _temp1576=( void*) _temp1568.rgn_typ; goto _LL1575; _LL1575: _temp1574=
_temp1568.nullable; goto _LL1573; _LL1573: _temp1572= _temp1568.tq; goto _LL1571;
_LL1571: _temp1570= _temp1568.bounds; goto _LL1565;} else{ goto _LL1566;}
_LL1566: goto _LL1567; _LL1565: elt_typ_opt=( void**) _temp1580; goto _LL1563;
_LL1567: goto _LL1563; _LL1563:;}{ void* _temp1581= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); goto _LL1582; _LL1582: { void* res_typ;{ void* _temp1583= Cyc_Tcutil_compress(
_temp1581); struct Cyc_Absyn_Exp* _temp1589; struct Cyc_Absyn_Tqual _temp1591;
void* _temp1593; _LL1585: if(( unsigned int) _temp1583 > 4u?*(( int*) _temp1583)
== Cyc_Absyn_ArrayType: 0){ _LL1594: _temp1593=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1583)->f1; goto _LL1592; _LL1592: _temp1591=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1583)->f2; goto _LL1590; _LL1590: _temp1589=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1583)->f3; goto _LL1586;} else{ goto _LL1587;} _LL1587: goto _LL1588;
_LL1586: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1595=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1595[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1596; _temp1596.tag=
Cyc_Absyn_PointerType; _temp1596.f1=({ struct Cyc_Absyn_PtrInfo _temp1597;
_temp1597.elt_typ=( void*) _temp1593; _temp1597.rgn_typ=( void*) rgn; _temp1597.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1597.tq=
_temp1591; _temp1597.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1598=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1598[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1599; _temp1599.tag= Cyc_Absyn_Upper_b; _temp1599.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1589); _temp1599;}); _temp1598;}));
_temp1597;}); _temp1596;}); _temp1595;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1600=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1600->v=( void*) res_typ; _temp1600;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1584; _LL1588: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1601="tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1602; _temp1602.curr= _temp1601; _temp1602.base=
_temp1601; _temp1602.last_plus_one= _temp1601 + 50; _temp1602;})); _LL1584:;}
return res_typ;}}} _LL1519: { void* _temp1603= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); goto _LL1604;
_LL1604: { void* _temp1605= Cyc_Tcexp_tcExp( te,( void**)& _temp1603, e1); goto
_LL1606; _LL1606: return Cyc_Absyn_atb_typ( _temp1605, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1607=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1607[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1608; _temp1608.tag= Cyc_Absyn_Upper_b; _temp1608.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1608;}); _temp1607;}));}} _LL1521: { void**
topt2= 0; if( topt != 0){ void* _temp1609= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1617; struct Cyc_Absyn_Conref*
_temp1619; struct Cyc_Absyn_Tqual _temp1621; struct Cyc_Absyn_Conref* _temp1623;
void* _temp1625; void* _temp1627; void** _temp1629; struct Cyc_Absyn_TunionInfo
_temp1630; _LL1611: if(( unsigned int) _temp1609 > 4u?*(( int*) _temp1609) ==
Cyc_Absyn_PointerType: 0){ _LL1618: _temp1617=(( struct Cyc_Absyn_PointerType_struct*)
_temp1609)->f1; _LL1628: _temp1627=( void*) _temp1617.elt_typ; _temp1629=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1609)->f1).elt_typ; goto _LL1626;
_LL1626: _temp1625=( void*) _temp1617.rgn_typ; goto _LL1624; _LL1624: _temp1623=
_temp1617.nullable; goto _LL1622; _LL1622: _temp1621= _temp1617.tq; goto _LL1620;
_LL1620: _temp1619= _temp1617.bounds; goto _LL1612;} else{ goto _LL1613;}
_LL1613: if(( unsigned int) _temp1609 > 4u?*(( int*) _temp1609) == Cyc_Absyn_TunionType:
0){ _LL1631: _temp1630=(( struct Cyc_Absyn_TunionType_struct*) _temp1609)->f1;
goto _LL1614;} else{ goto _LL1615;} _LL1615: goto _LL1616; _LL1612: topt2=( void**)
_temp1629; goto _LL1610; _LL1614: topt2=({ void** _temp1632=( void**) GC_malloc(
sizeof( void*)); _temp1632[ 0]=*(( void**) _check_null( topt)); _temp1632;});
goto _LL1610; _LL1616: goto _LL1610; _LL1610:;}{ void* _temp1633= Cyc_Tcexp_tcExp(
te, topt2, e1); goto _LL1634; _LL1634: { void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1636=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1636[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1637; _temp1637.tag=
Cyc_Absyn_PointerType; _temp1637.f1=({ struct Cyc_Absyn_PtrInfo _temp1638;
_temp1638.elt_typ=( void*) _temp1633; _temp1638.rgn_typ=( void*) rgn; _temp1638.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1638.tq= Cyc_Absyn_empty_tqual();
_temp1638.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1639=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1639[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1640; _temp1640.tag= Cyc_Absyn_Upper_b; _temp1640.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1640;}); _temp1639;})); _temp1638;}); _temp1637;});
_temp1636;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1635=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1635->v=( void*) res_typ;
_temp1635;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}}} _LL1511:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1641= t; struct Cyc_Absyn_Exp* _temp1647; struct Cyc_Absyn_Tqual
_temp1649; void* _temp1651; _LL1643: if(( unsigned int) _temp1641 > 4u?*(( int*)
_temp1641) == Cyc_Absyn_ArrayType: 0){ _LL1652: _temp1651=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1641)->f1; goto _LL1650; _LL1650: _temp1649=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1641)->f2; goto _LL1648; _LL1648: _temp1647=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1641)->f3; goto _LL1644;} else{ goto _LL1645;} _LL1645: goto _LL1646;
_LL1644: { void* _temp1655; int _temp1657; struct _tuple5 _temp1653= Cyc_Tcutil_addressof_props(
te, e); _LL1658: _temp1657= _temp1653.f1; goto _LL1656; _LL1656: _temp1655=
_temp1653.f2; goto _LL1654; _LL1654: { void* _temp1661= _temp1647 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1659=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1659[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1660; _temp1660.tag= Cyc_Absyn_Upper_b;
_temp1660.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1647); _temp1660;});
_temp1659;}); goto _LL1662; _LL1662: t= Cyc_Absyn_atb_typ( _temp1651, _temp1655,
_temp1649, _temp1661);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=(
void*) t); return t;}} _LL1646: return t; _LL1642:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1663=( void*) e->r; struct Cyc_List_List* _temp1673;
struct Cyc_Absyn_Exp* _temp1675; struct Cyc_Absyn_Exp* _temp1677; struct Cyc_Absyn_Vardecl*
_temp1679; void* _temp1681; struct _tagged_string _temp1683; _LL1665: if(*(( int*)
_temp1663) == Cyc_Absyn_Array_e){ _LL1674: _temp1673=(( struct Cyc_Absyn_Array_e_struct*)
_temp1663)->f1; goto _LL1666;} else{ goto _LL1667;} _LL1667: if(*(( int*)
_temp1663) == Cyc_Absyn_Comprehension_e){ _LL1680: _temp1679=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1663)->f1; goto _LL1678; _LL1678: _temp1677=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1663)->f2; goto _LL1676; _LL1676: _temp1675=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1663)->f3; goto _LL1668;} else{ goto _LL1669;} _LL1669: if(*(( int*)
_temp1663) == Cyc_Absyn_Const_e){ _LL1682: _temp1681=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1663)->f1; if(( unsigned int) _temp1681 > 1u?*(( int*) _temp1681) == Cyc_Absyn_String_c:
0){ _LL1684: _temp1683=(( struct Cyc_Absyn_String_c_struct*) _temp1681)->f1;
goto _LL1670;} else{ goto _LL1671;}} else{ goto _LL1671;} _LL1671: goto _LL1672;
_LL1666: return t; _LL1668: return t; _LL1670: return t; _LL1672: t= Cyc_Tcutil_compress(
t);{ void* _temp1685= t; struct Cyc_Absyn_Exp* _temp1691; struct Cyc_Absyn_Tqual
_temp1693; void* _temp1695; _LL1687: if(( unsigned int) _temp1685 > 4u?*(( int*)
_temp1685) == Cyc_Absyn_ArrayType: 0){ _LL1696: _temp1695=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1685)->f1; goto _LL1694; _LL1694: _temp1693=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1685)->f2; goto _LL1692; _LL1692: _temp1691=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1685)->f3; goto _LL1688;} else{ goto _LL1689;} _LL1689: goto _LL1690;
_LL1688: { void* _temp1699; int _temp1701; struct _tuple5 _temp1697= Cyc_Tcutil_addressof_props(
te, e); _LL1702: _temp1701= _temp1697.f1; goto _LL1700; _LL1700: _temp1699=
_temp1697.f2; goto _LL1698; _LL1698: { void* b= _temp1691 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1703=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1703[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1704; _temp1704.tag= Cyc_Absyn_Upper_b; _temp1704.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1691); _temp1704;}); _temp1703;}); t=
Cyc_Absyn_atb_typ( _temp1695, _temp1699, _temp1693, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1690: return t; _LL1686:;} _LL1664:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1705=( void*) e->r; struct Cyc_Absyn_Exp* _temp1711; _LL1707: if(*(( int*)
_temp1705) == Cyc_Absyn_NoInstantiate_e){ _LL1712: _temp1711=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1705)->f1; goto _LL1708;} else{ goto _LL1709;} _LL1709: goto _LL1710;
_LL1708: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1711);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1711->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1711->topt))->v))); e->topt=
_temp1711->topt; goto _LL1706; _LL1710: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1713= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1719; struct Cyc_Absyn_Conref*
_temp1721; struct Cyc_Absyn_Tqual _temp1723; struct Cyc_Absyn_Conref* _temp1725;
void* _temp1727; void* _temp1729; _LL1715: if(( unsigned int) _temp1713 > 4u?*((
int*) _temp1713) == Cyc_Absyn_PointerType: 0){ _LL1720: _temp1719=(( struct Cyc_Absyn_PointerType_struct*)
_temp1713)->f1; _LL1730: _temp1729=( void*) _temp1719.elt_typ; goto _LL1728;
_LL1728: _temp1727=( void*) _temp1719.rgn_typ; goto _LL1726; _LL1726: _temp1725=
_temp1719.nullable; goto _LL1724; _LL1724: _temp1723= _temp1719.tq; goto _LL1722;
_LL1722: _temp1721= _temp1719.bounds; goto _LL1716;} else{ goto _LL1717;}
_LL1717: goto _LL1718; _LL1716:{ void* _temp1731= Cyc_Tcutil_compress( _temp1729);
struct Cyc_Absyn_FnInfo _temp1737; struct Cyc_List_List* _temp1739; struct Cyc_Absyn_VarargInfo*
_temp1741; int _temp1743; struct Cyc_List_List* _temp1745; void* _temp1747;
struct Cyc_Core_Opt* _temp1749; struct Cyc_List_List* _temp1751; _LL1733: if((
unsigned int) _temp1731 > 4u?*(( int*) _temp1731) == Cyc_Absyn_FnType: 0){
_LL1738: _temp1737=(( struct Cyc_Absyn_FnType_struct*) _temp1731)->f1; _LL1752:
_temp1751= _temp1737.tvars; goto _LL1750; _LL1750: _temp1749= _temp1737.effect;
goto _LL1748; _LL1748: _temp1747=( void*) _temp1737.ret_typ; goto _LL1746;
_LL1746: _temp1745= _temp1737.args; goto _LL1744; _LL1744: _temp1743= _temp1737.c_varargs;
goto _LL1742; _LL1742: _temp1741= _temp1737.cyc_varargs; goto _LL1740; _LL1740:
_temp1739= _temp1737.attributes; goto _LL1734;} else{ goto _LL1735;} _LL1735:
goto _LL1736; _LL1734: if( _temp1751 != 0){ struct _RegionHandle _temp1753=
_new_region(); struct _RegionHandle* rgn=& _temp1753; _push_region( rgn);{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1751); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1760=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1760[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1761; _temp1761.tag= Cyc_Absyn_FnType; _temp1761.f1=({ struct Cyc_Absyn_FnInfo
_temp1762; _temp1762.tvars= 0; _temp1762.effect= _temp1749; _temp1762.ret_typ=(
void*) _temp1747; _temp1762.args= _temp1745; _temp1762.c_varargs= _temp1743;
_temp1762.cyc_varargs= _temp1741; _temp1762.attributes= _temp1739; _temp1762;});
_temp1761;}); _temp1760;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1757=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1757[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1758; _temp1758.tag=
Cyc_Absyn_PointerType; _temp1758.f1=({ struct Cyc_Absyn_PtrInfo _temp1759;
_temp1759.elt_typ=( void*) ftyp; _temp1759.rgn_typ=( void*) _temp1727; _temp1759.nullable=
_temp1725; _temp1759.tq= _temp1723; _temp1759.bounds= _temp1721; _temp1759;});
_temp1758;}); _temp1757;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1754=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1754[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1755; _temp1755.tag= Cyc_Absyn_Instantiate_e; _temp1755.f1= inner;
_temp1755.f2= ts; _temp1755;}); _temp1754;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1756=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1756->v=( void*) new_typ; _temp1756;});}; _pop_region(& _temp1753);} goto
_LL1732; _LL1736: goto _LL1732; _LL1732:;} goto _LL1714; _LL1718: goto _LL1714;
_LL1714:;} goto _LL1706; _LL1706:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1763=( void*) e->r; struct Cyc_Absyn_Exp* _temp1835; struct
_tuple1* _temp1837; struct Cyc_List_List* _temp1839; struct Cyc_Absyn_Exp*
_temp1841; struct Cyc_List_List* _temp1843; struct Cyc_Core_Opt* _temp1845; void*
_temp1847; void* _temp1849; struct _tuple1* _temp1851; struct Cyc_List_List*
_temp1853; void* _temp1855; void* _temp1857; struct Cyc_Absyn_Exp* _temp1859;
struct Cyc_Absyn_Exp* _temp1861; struct Cyc_Core_Opt* _temp1863; struct Cyc_Absyn_Exp*
_temp1865; struct Cyc_Absyn_Exp* _temp1867; struct Cyc_Absyn_Exp* _temp1869;
struct Cyc_Absyn_Exp* _temp1871; struct Cyc_Absyn_Exp* _temp1873; struct Cyc_Absyn_Exp*
_temp1875; struct Cyc_Absyn_VarargCallInfo* _temp1877; struct Cyc_Absyn_VarargCallInfo**
_temp1879; struct Cyc_List_List* _temp1880; struct Cyc_Absyn_Exp* _temp1882;
struct Cyc_Absyn_Exp* _temp1884; struct Cyc_List_List* _temp1886; struct Cyc_Absyn_Exp*
_temp1888; struct Cyc_Absyn_Exp* _temp1890; void* _temp1892; struct Cyc_Absyn_Exp*
_temp1894; struct Cyc_Absyn_Exp* _temp1896; struct Cyc_Absyn_Exp* _temp1898;
struct Cyc_Absyn_Exp* _temp1900; void* _temp1902; struct Cyc_Absyn_Exp*
_temp1904; struct _tagged_string* _temp1906; struct Cyc_Absyn_Exp* _temp1908;
struct _tagged_string* _temp1910; struct Cyc_Absyn_Exp* _temp1912; struct Cyc_Absyn_Exp*
_temp1914; struct Cyc_Absyn_Exp* _temp1916; struct Cyc_List_List* _temp1918;
struct Cyc_List_List* _temp1920; struct _tuple2* _temp1922; struct Cyc_List_List*
_temp1924; struct Cyc_Absyn_Stmt* _temp1926; struct Cyc_Absyn_Fndecl* _temp1928;
struct Cyc_Absyn_Exp* _temp1930; struct Cyc_Absyn_Exp* _temp1932; struct Cyc_Absyn_Exp*
_temp1934; struct Cyc_Absyn_Vardecl* _temp1936; struct Cyc_Absyn_Structdecl*
_temp1938; struct Cyc_Absyn_Structdecl** _temp1940; struct Cyc_List_List*
_temp1941; struct Cyc_Core_Opt* _temp1943; struct Cyc_Core_Opt** _temp1945;
struct _tuple1* _temp1946; struct _tuple1** _temp1948; struct Cyc_List_List*
_temp1949; void* _temp1951; struct Cyc_Absyn_Tunionfield* _temp1953; struct Cyc_Absyn_Tuniondecl*
_temp1955; struct Cyc_List_List* _temp1957; struct Cyc_Core_Opt* _temp1959;
struct Cyc_Core_Opt** _temp1961; struct Cyc_Core_Opt* _temp1962; struct Cyc_Core_Opt**
_temp1964; struct Cyc_Absyn_Enumfield* _temp1965; struct Cyc_Absyn_Enumdecl*
_temp1967; struct _tuple1* _temp1969; struct _tuple1** _temp1971; void*
_temp1972; struct Cyc_Absyn_Exp* _temp1974; _LL1765: if(*(( int*) _temp1763) ==
Cyc_Absyn_NoInstantiate_e){ _LL1836: _temp1835=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1763)->f1; goto _LL1766;} else{ goto _LL1767;} _LL1767: if(*(( int*)
_temp1763) == Cyc_Absyn_UnknownId_e){ _LL1838: _temp1837=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1763)->f1; goto _LL1768;} else{ goto _LL1769;} _LL1769: if(*(( int*)
_temp1763) == Cyc_Absyn_UnknownCall_e){ _LL1842: _temp1841=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1763)->f1; goto _LL1840; _LL1840: _temp1839=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1763)->f2; goto _LL1770;} else{ goto _LL1771;} _LL1771: if(*(( int*)
_temp1763) == Cyc_Absyn_UnresolvedMem_e){ _LL1846: _temp1845=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1763)->f1; goto _LL1844; _LL1844: _temp1843=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1763)->f2; goto _LL1772;} else{ goto _LL1773;} _LL1773: if(*(( int*)
_temp1763) == Cyc_Absyn_Const_e){ _LL1848: _temp1847=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1763)->f1; goto _LL1774;} else{ goto _LL1775;} _LL1775: if(*(( int*)
_temp1763) == Cyc_Absyn_Var_e){ _LL1852: _temp1851=(( struct Cyc_Absyn_Var_e_struct*)
_temp1763)->f1; goto _LL1850; _LL1850: _temp1849=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1763)->f2; goto _LL1776;} else{ goto _LL1777;} _LL1777: if(*(( int*)
_temp1763) == Cyc_Absyn_Primop_e){ _LL1856: _temp1855=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1763)->f1; goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1763)->f2; goto _LL1778;} else{ goto _LL1779;} _LL1779: if(*(( int*)
_temp1763) == Cyc_Absyn_Increment_e){ _LL1860: _temp1859=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1763)->f1; goto _LL1858; _LL1858: _temp1857=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1763)->f2; goto _LL1780;} else{ goto _LL1781;} _LL1781: if(*(( int*)
_temp1763) == Cyc_Absyn_AssignOp_e){ _LL1866: _temp1865=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1763)->f1; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1763)->f2; goto _LL1862; _LL1862: _temp1861=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1763)->f3; goto _LL1782;} else{ goto _LL1783;} _LL1783: if(*(( int*)
_temp1763) == Cyc_Absyn_Conditional_e){ _LL1872: _temp1871=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1763)->f1; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1763)->f2; goto _LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1763)->f3; goto _LL1784;} else{ goto _LL1785;} _LL1785: if(*(( int*)
_temp1763) == Cyc_Absyn_SeqExp_e){ _LL1876: _temp1875=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1763)->f1; goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1763)->f2; goto _LL1786;} else{ goto _LL1787;} _LL1787: if(*(( int*)
_temp1763) == Cyc_Absyn_FnCall_e){ _LL1883: _temp1882=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1763)->f1; goto _LL1881; _LL1881: _temp1880=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1763)->f2; goto _LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1763)->f3; _temp1879=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1763)->f3;
goto _LL1788;} else{ goto _LL1789;} _LL1789: if(*(( int*) _temp1763) == Cyc_Absyn_Throw_e){
_LL1885: _temp1884=(( struct Cyc_Absyn_Throw_e_struct*) _temp1763)->f1; goto
_LL1790;} else{ goto _LL1791;} _LL1791: if(*(( int*) _temp1763) == Cyc_Absyn_Instantiate_e){
_LL1889: _temp1888=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1763)->f1;
goto _LL1887; _LL1887: _temp1886=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1763)->f2; goto _LL1792;} else{ goto _LL1793;} _LL1793: if(*(( int*)
_temp1763) == Cyc_Absyn_Cast_e){ _LL1893: _temp1892=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1763)->f1; goto _LL1891; _LL1891: _temp1890=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1763)->f2; goto _LL1794;} else{ goto _LL1795;} _LL1795: if(*(( int*)
_temp1763) == Cyc_Absyn_Address_e){ _LL1895: _temp1894=(( struct Cyc_Absyn_Address_e_struct*)
_temp1763)->f1; goto _LL1796;} else{ goto _LL1797;} _LL1797: if(*(( int*)
_temp1763) == Cyc_Absyn_New_e){ _LL1899: _temp1898=(( struct Cyc_Absyn_New_e_struct*)
_temp1763)->f1; goto _LL1897; _LL1897: _temp1896=(( struct Cyc_Absyn_New_e_struct*)
_temp1763)->f2; goto _LL1798;} else{ goto _LL1799;} _LL1799: if(*(( int*)
_temp1763) == Cyc_Absyn_Sizeofexp_e){ _LL1901: _temp1900=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1763)->f1; goto _LL1800;} else{ goto _LL1801;} _LL1801: if(*(( int*)
_temp1763) == Cyc_Absyn_Sizeoftyp_e){ _LL1903: _temp1902=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1763)->f1; goto _LL1802;} else{ goto _LL1803;} _LL1803: if(*(( int*)
_temp1763) == Cyc_Absyn_Deref_e){ _LL1905: _temp1904=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1763)->f1; goto _LL1804;} else{ goto _LL1805;} _LL1805: if(*(( int*)
_temp1763) == Cyc_Absyn_StructMember_e){ _LL1909: _temp1908=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1763)->f1; goto _LL1907; _LL1907: _temp1906=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1763)->f2; goto _LL1806;} else{ goto _LL1807;} _LL1807: if(*(( int*)
_temp1763) == Cyc_Absyn_StructArrow_e){ _LL1913: _temp1912=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1763)->f1; goto _LL1911; _LL1911: _temp1910=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1763)->f2; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(*(( int*)
_temp1763) == Cyc_Absyn_Subscript_e){ _LL1917: _temp1916=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1763)->f1; goto _LL1915; _LL1915: _temp1914=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1763)->f2; goto _LL1810;} else{ goto _LL1811;} _LL1811: if(*(( int*)
_temp1763) == Cyc_Absyn_Tuple_e){ _LL1919: _temp1918=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1763)->f1; goto _LL1812;} else{ goto _LL1813;} _LL1813: if(*(( int*)
_temp1763) == Cyc_Absyn_CompoundLit_e){ _LL1923: _temp1922=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1763)->f1; goto _LL1921; _LL1921: _temp1920=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1763)->f2; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(*(( int*)
_temp1763) == Cyc_Absyn_Array_e){ _LL1925: _temp1924=(( struct Cyc_Absyn_Array_e_struct*)
_temp1763)->f1; goto _LL1816;} else{ goto _LL1817;} _LL1817: if(*(( int*)
_temp1763) == Cyc_Absyn_StmtExp_e){ _LL1927: _temp1926=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1763)->f1; goto _LL1818;} else{ goto _LL1819;} _LL1819: if(*(( int*)
_temp1763) == Cyc_Absyn_Codegen_e){ _LL1929: _temp1928=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1763)->f1; goto _LL1820;} else{ goto _LL1821;} _LL1821: if(*(( int*)
_temp1763) == Cyc_Absyn_Fill_e){ _LL1931: _temp1930=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1763)->f1; goto _LL1822;} else{ goto _LL1823;} _LL1823: if(*(( int*)
_temp1763) == Cyc_Absyn_Comprehension_e){ _LL1937: _temp1936=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1763)->f1; goto _LL1935; _LL1935: _temp1934=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1763)->f2; goto _LL1933; _LL1933: _temp1932=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1763)->f3; goto _LL1824;} else{ goto _LL1825;} _LL1825: if(*(( int*)
_temp1763) == Cyc_Absyn_Struct_e){ _LL1947: _temp1946=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1763)->f1; _temp1948=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1763)->f1;
goto _LL1944; _LL1944: _temp1943=(( struct Cyc_Absyn_Struct_e_struct*) _temp1763)->f2;
_temp1945=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1763)->f2; goto _LL1942;
_LL1942: _temp1941=(( struct Cyc_Absyn_Struct_e_struct*) _temp1763)->f3; goto
_LL1939; _LL1939: _temp1938=(( struct Cyc_Absyn_Struct_e_struct*) _temp1763)->f4;
_temp1940=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1763)->f4; goto _LL1826;}
else{ goto _LL1827;} _LL1827: if(*(( int*) _temp1763) == Cyc_Absyn_AnonStruct_e){
_LL1952: _temp1951=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1763)->f1;
goto _LL1950; _LL1950: _temp1949=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1763)->f2; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(*(( int*)
_temp1763) == Cyc_Absyn_Tunion_e){ _LL1963: _temp1962=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1763)->f1; _temp1964=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1763)->f1;
goto _LL1960; _LL1960: _temp1959=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1763)->f2;
_temp1961=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1763)->f2; goto _LL1958;
_LL1958: _temp1957=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1763)->f3; goto
_LL1956; _LL1956: _temp1955=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1763)->f4;
goto _LL1954; _LL1954: _temp1953=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1763)->f5;
goto _LL1830;} else{ goto _LL1831;} _LL1831: if(*(( int*) _temp1763) == Cyc_Absyn_Enum_e){
_LL1970: _temp1969=(( struct Cyc_Absyn_Enum_e_struct*) _temp1763)->f1; _temp1971=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1763)->f1; goto _LL1968; _LL1968:
_temp1967=(( struct Cyc_Absyn_Enum_e_struct*) _temp1763)->f2; goto _LL1966;
_LL1966: _temp1965=(( struct Cyc_Absyn_Enum_e_struct*) _temp1763)->f3; goto
_LL1832;} else{ goto _LL1833;} _LL1833: if(*(( int*) _temp1763) == Cyc_Absyn_Malloc_e){
_LL1975: _temp1974=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1763)->f1; goto
_LL1973; _LL1973: _temp1972=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1763)->f2; goto _LL1834;} else{ goto _LL1764;} _LL1766: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1835); return; _LL1768: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1837); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1770: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1841, _temp1839); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1772: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1843); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1774: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1847, e);
goto _LL1764; _LL1776: t= Cyc_Tcexp_tcVar( te, loc, _temp1851, _temp1849); goto
_LL1764; _LL1778: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1855, _temp1853);
goto _LL1764; _LL1780: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1859,
_temp1857); goto _LL1764; _LL1782: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1865, _temp1863, _temp1861); goto _LL1764; _LL1784: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1871, _temp1869, _temp1867); goto _LL1764; _LL1786: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1875, _temp1873); goto _LL1764; _LL1788: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1882, _temp1880, _temp1879); goto _LL1764; _LL1790: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1884); goto _LL1764; _LL1792: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1888, _temp1886); goto _LL1764; _LL1794: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1892, _temp1890); goto _LL1764; _LL1796: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1894); goto _LL1764; _LL1798: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1898, e, _temp1896); goto _LL1764; _LL1800: { void* _temp1976= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1900); goto _LL1977; _LL1977: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1976); goto _LL1764;} _LL1802: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1902); goto _LL1764; _LL1804: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp1904); goto _LL1764; _LL1806: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp1908, _temp1906); goto _LL1764; _LL1808: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp1912, _temp1910); goto _LL1764; _LL1810: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1916, _temp1914); goto _LL1764; _LL1812: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1918); goto _LL1764; _LL1814: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1922, _temp1920); goto _LL1764; _LL1816: { void** elt_topt= 0;
if( topt != 0){ void* _temp1978= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp1984; struct Cyc_Absyn_Tqual _temp1986; void*
_temp1988; void** _temp1990; _LL1980: if(( unsigned int) _temp1978 > 4u?*(( int*)
_temp1978) == Cyc_Absyn_ArrayType: 0){ _LL1989: _temp1988=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1978)->f1; _temp1990=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1978)->f1); goto _LL1987; _LL1987: _temp1986=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1978)->f2; goto _LL1985; _LL1985: _temp1984=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1978)->f3; goto _LL1981;} else{ goto _LL1982;} _LL1982: goto _LL1983;
_LL1981: elt_topt=( void**) _temp1990; goto _LL1979; _LL1983: goto _LL1979;
_LL1979:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1924); goto _LL1764;}
_LL1818: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1926); goto _LL1764;
_LL1820: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1928); goto _LL1764;
_LL1822: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1930); goto _LL1764; _LL1824:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1936, _temp1934, _temp1932);
goto _LL1764; _LL1826: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1948,
_temp1945, _temp1941, _temp1940); goto _LL1764; _LL1828: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1951, _temp1949); goto _LL1764; _LL1830: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1964, _temp1961, _temp1957, _temp1955, _temp1953); goto
_LL1764; _LL1832:* _temp1971=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1965))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1991=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1991[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1992; _temp1992.tag= Cyc_Absyn_EnumType;
_temp1992.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1967))->name;
_temp1992.f2= _temp1967; _temp1992;}); _temp1991;}); goto _LL1764; _LL1834: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1974, _temp1972); goto _LL1764; _LL1764:;}
e->topt=({ struct Cyc_Core_Opt* _temp1993=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1993->v=( void*) t; _temp1993;});}