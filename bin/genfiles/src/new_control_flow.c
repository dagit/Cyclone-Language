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
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key, void* data); extern
void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern void* Cyc_Assert_AssertFail( struct
_tagged_string msg); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file;
struct _tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
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
int tag; struct Cyc_Dict_Dict* f1; } ; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u];
static const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG=
1; static const int Cyc_CfAbsexp_OneofG= 2; extern void* Cyc_CfAbsexp_mkAnyOrderG();
extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp(
void* ao); extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd);
extern void* Cyc_CfAbsexp_mkMemberOp( void* ao, void* f); extern void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern
void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE(); extern
void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt* encloser; int
visited; void* flow; struct Cyc_List_List* absexps; } ; unsigned char Cyc_NewControlFlow_CFAnnot[
12u]="\000\000\000\000CFAnnot"; struct Cyc_NewControlFlow_CFAnnot_struct{
unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot f1; } ; void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser){( void*)(
enclosee->annot=( void*)(( void*)({ struct Cyc_NewControlFlow_CFAnnot_struct*
_temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({ struct Cyc_NewControlFlow_CFAnnot_struct
_temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot; _temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot
_temp2; _temp2.encloser= encloser; _temp2.visited= 0; _temp2.flow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;})));}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt* s){ void* _temp3=( void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp9; struct Cyc_NewControlFlow_CFStmtAnnot* _temp11; _LL5: if(*(( void**)
_temp3) == Cyc_NewControlFlow_CFAnnot){ _LL10: _temp9=(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; _temp11=&(( struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1;
goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11; _LL8:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp12=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp12[ 0]=({ struct
Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible; _temp13.f1=({
unsigned char* _temp14="ControlFlow -- wrong stmt annotation"; struct
_tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14; _temp15.last_plus_one=
_temp14 + 37; _temp15;}); _temp13;}); _temp12;})); _LL4:;} void Cyc_NewControlFlow_cf_set_absexps(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot(
s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict*
all_roots; struct Cyc_List_List* comprehension_vars; } ; struct Cyc_NewControlFlow_AbsEnv{
struct Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides;
struct Cyc_List_List* access_path; } ; struct Cyc_NewControlFlow_AbsSynOne{ void*
inner_exp; void* assigns; } ; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp16=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp16[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_VarRoot;
_temp17.f1= vd; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp18=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp18[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp19; _temp19.tag= Cyc_CfFlowInfo_MallocPt;
_temp19.f1= e; _temp19;}); _temp18;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp20; _temp20.shared= src.shared; _temp20.lhsides= new_lhs; _temp20.access_path=
src.access_path; _temp20;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
== 0){ Cyc_NewControlFlow_unknown_list=({ struct Cyc_List_List* _temp21=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp21->hd=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp21->tl= 0; _temp21;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv _temp22= Cyc_NewControlFlow_change_lhs(
src, Cyc_NewControlFlow_use_it_list()); goto _LL23; _LL23: _temp22.access_path=
0; return _temp22;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp26; struct Cyc_NewControlFlow_AbsSynOne
_temp28; void* _temp30; void* _temp32; struct Cyc_NewControlFlow_AbsSynOne
_temp34; void* _temp36; void* _temp38; struct Cyc_NewControlFlow_AbsSyn _temp24=
syn; _LL35: _temp34= _temp24.when_true; _LL39: _temp38=( void*) _temp34.inner_exp;
goto _LL37; _LL37: _temp36=( void*) _temp34.assigns; goto _LL29; _LL29: _temp28=
_temp24.when_false; _LL33: _temp32=( void*) _temp28.inner_exp; goto _LL31; _LL31:
_temp30=( void*) _temp28.assigns; goto _LL27; _LL27: _temp26= _temp24.lvalues;
goto _LL25; _LL25: { void* _temp40= _temp38 == _temp32? _temp38: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp38, _temp32); goto _LL41; _LL41: { void*
_temp42= _temp36 == _temp30? _temp36: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
_temp36, _temp30); goto _LL43; _LL43: return({ struct Cyc_NewControlFlow_AbsSynOne
_temp44; _temp44.inner_exp=( void*) _temp40; _temp44.assigns=( void*) _temp42;
_temp44;});}}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_mkSyn_tf(
void* ae, struct Cyc_List_List* lvals, void* assigns){ struct Cyc_NewControlFlow_AbsSynOne
_temp46=({ struct Cyc_NewControlFlow_AbsSynOne _temp45; _temp45.inner_exp=( void*)
ae; _temp45.assigns=( void*) assigns; _temp45;}); goto _LL47; _LL47: return({
struct Cyc_NewControlFlow_AbsSyn _temp48; _temp48.when_true= _temp46; _temp48.when_false=
_temp46; _temp48.lvalues= lvals; _temp48;});} void* Cyc_NewControlFlow_make_assigns(
struct Cyc_NewControlFlow_AbsEnv env, void* ae){ if( env.lhsides == 0){ return
Cyc_CfAbsexp_mkSkipAE();} if((( struct Cyc_List_List*) _check_null( env.lhsides))->tl
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( env.lhsides))->hd, ae);}{ struct Cyc_List_List* _temp49= 0; goto
_LL50; _LL50:{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0;
lhsides=(( struct Cyc_List_List*) _check_null( lhsides))->tl){ _temp49=({ struct
Cyc_List_List* _temp51=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp51->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( lhsides))->hd, ae); _temp51->tl= _temp49; _temp51;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, _temp49);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp52= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
goto _LL53; _LL53: { void* _temp56; void* _temp58; struct Cyc_NewControlFlow_AbsSynOne
_temp54= Cyc_NewControlFlow_meet_absexp( _temp52); _LL59: _temp58=( void*)
_temp54.inner_exp; goto _LL57; _LL57: _temp56=( void*) _temp54.assigns; goto
_LL55; _LL55: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp58, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp56, uses_exp);}} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp60=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp60->hd=( void*) path_e; _temp60->tl=
env.access_path; _temp60;});{ struct Cyc_NewControlFlow_AbsSyn _temp61= Cyc_NewControlFlow_abstract_exp(
env, inner_e); goto _LL62; _LL62: if( _temp61.lvalues == 0){ _temp61.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp61;}} void* Cyc_NewControlFlow_destruct_path(
struct Cyc_NewControlFlow_AbsEnv env, void* absop){{ struct Cyc_List_List* path=
env.access_path; for( 0; path != 0; path=(( struct Cyc_List_List*) _check_null(
path))->tl){ void* _temp63=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( path))->hd)->r; struct Cyc_Absyn_Exp* _temp77; struct Cyc_Absyn_Exp*
_temp79; struct _tagged_string* _temp81; struct Cyc_Absyn_Exp* _temp83; struct
_tagged_string* _temp85; struct Cyc_Absyn_Exp* _temp87; struct Cyc_Absyn_Exp*
_temp89; struct Cyc_Absyn_Exp* _temp91; _LL65: if(*(( int*) _temp63) == Cyc_Absyn_Address_e){
_LL78: _temp77=(( struct Cyc_Absyn_Address_e_struct*) _temp63)->f1; goto _LL66;}
else{ goto _LL67;} _LL67: if(*(( int*) _temp63) == Cyc_Absyn_Deref_e){ _LL80:
_temp79=(( struct Cyc_Absyn_Deref_e_struct*) _temp63)->f1; goto _LL68;} else{
goto _LL69;} _LL69: if(*(( int*) _temp63) == Cyc_Absyn_StructArrow_e){ _LL84:
_temp83=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp63)->f1; goto _LL82;
_LL82: _temp81=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp63)->f2; goto
_LL70;} else{ goto _LL71;} _LL71: if(*(( int*) _temp63) == Cyc_Absyn_StructMember_e){
_LL88: _temp87=(( struct Cyc_Absyn_StructMember_e_struct*) _temp63)->f1; goto
_LL86; _LL86: _temp85=(( struct Cyc_Absyn_StructMember_e_struct*) _temp63)->f2;
goto _LL72;} else{ goto _LL73;} _LL73: if(*(( int*) _temp63) == Cyc_Absyn_Subscript_e){
_LL92: _temp91=(( struct Cyc_Absyn_Subscript_e_struct*) _temp63)->f1; goto _LL90;
_LL90: _temp89=(( struct Cyc_Absyn_Subscript_e_struct*) _temp63)->f2; goto _LL74;}
else{ goto _LL75;} _LL75: goto _LL76; _LL66:(( struct Cyc_List_List*)
_check_null( path))->tl == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({
unsigned char* _temp93="new_control_flow.cyc:219 path->tl == null"; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 42; _temp94;})); return Cyc_CfAbsexp_mkAddressOp( absop); _LL68: absop=
Cyc_CfAbsexp_mkDerefOp( absop); goto _LL64; _LL70: absop= Cyc_CfAbsexp_mkDerefOp(
absop); _temp85= _temp81; goto _LL72; _LL72: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp95=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp95[ 0]=({ struct
Cyc_CfFlowInfo_StructF_struct _temp96; _temp96.tag= Cyc_CfFlowInfo_StructF;
_temp96.f1= _temp85; _temp96;}); _temp95;})); goto _LL64; _LL74: { unsigned int
_temp97= Cyc_Evexp_eval_const_uint_exp( _temp89); goto _LL98; _LL98: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp99=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp99[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp100; _temp100.tag= Cyc_CfFlowInfo_TupleF;
_temp100.f1=( int) _temp97; _temp100;}); _temp99;})); goto _LL64;} _LL76:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp101=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp101[ 0]=({ struct
Cyc_Core_Impossible_struct _temp102; _temp102.tag= Cyc_Core_Impossible; _temp102.f1=({
unsigned char* _temp103="abstract_exp: unexpected acces path element"; struct
_tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 44; _temp104;}); _temp102;}); _temp101;}));
_LL64:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp*
rgn_exp, struct Cyc_Absyn_Exp* init_exp){ void* _temp105= Cyc_CfAbsexp_mkMallocOp(
outer_e); goto _LL106; _LL106: { void* _temp107= Cyc_CfAbsexp_mkSkipAE(); goto
_LL108; _LL108: { void* _temp109= Cyc_CfAbsexp_mkSkipAE(); goto _LL110; _LL110:
if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv _temp112= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp111->hd=( void*) _temp105; _temp111->tl= 0;
_temp111;})); goto _LL113; _LL113: _temp112.access_path= 0;{ void* _temp116;
void* _temp118; struct Cyc_NewControlFlow_AbsSynOne _temp114= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp112,( struct Cyc_Absyn_Exp*) _check_null(
init_exp))); _LL119: _temp118=( void*) _temp114.inner_exp; goto _LL117; _LL117:
_temp116=( void*) _temp114.assigns; goto _LL115; _LL115: _temp107= _temp118;
_temp109= _temp116;}} if( rgn_exp != 0){ struct Cyc_NewControlFlow_AbsSyn
_temp120= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),(
struct Cyc_Absyn_Exp*) _check_null( rgn_exp)); goto _LL121; _LL121: { void*
_temp124; void* _temp126; struct Cyc_NewControlFlow_AbsSynOne _temp122= Cyc_NewControlFlow_meet_absexp(
_temp120); _LL127: _temp126=( void*) _temp122.inner_exp; goto _LL125; _LL125:
_temp124=( void*) _temp122.assigns; goto _LL123; _LL123: _temp107= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp107, _temp126); _temp109= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp109, _temp124);}}{ void* _temp128= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp107, _temp109)); goto _LL129; _LL129: {
struct Cyc_NewControlFlow_AbsSyn _temp130= Cyc_NewControlFlow_mkSyn_tf( _temp128,
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( _temp105)));
goto _LL131; _LL131: if( env.access_path != 0){ void* _temp132=( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( env.access_path))->hd)->r;
struct Cyc_Absyn_Exp* _temp140; struct _tagged_string* _temp142; struct Cyc_Absyn_Exp*
_temp144; _LL134: if(*(( int*) _temp132) == Cyc_Absyn_Deref_e){ _LL141: _temp140=((
struct Cyc_Absyn_Deref_e_struct*) _temp132)->f1; goto _LL135;} else{ goto _LL136;}
_LL136: if(*(( int*) _temp132) == Cyc_Absyn_StructArrow_e){ _LL145: _temp144=((
struct Cyc_Absyn_StructArrow_e_struct*) _temp132)->f1; goto _LL143; _LL143:
_temp142=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp132)->f2; goto _LL137;}
else{ goto _LL138;} _LL138: goto _LL139; _LL135: env.access_path=(( struct Cyc_List_List*)
_check_null( env.access_path))->tl; _temp130.lvalues=({ struct Cyc_List_List*
_temp146=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp146->hd=( void*) Cyc_NewControlFlow_destruct_path( env, _temp105); _temp146->tl=
0; _temp146;}); goto _LL133; _LL137: env.access_path=({ struct Cyc_List_List*
_temp147=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp147->hd=( void*) Cyc_Absyn_structmember_exp( _temp144, _temp142, 0);
_temp147->tl=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp147;}); _temp130.lvalues=({ struct Cyc_List_List* _temp148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp148->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp105); _temp148->tl= 0; _temp148;}); goto _LL133; _LL139:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp149=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp149[ 0]=({ struct
Cyc_Core_Impossible_struct _temp150; _temp150.tag= Cyc_Core_Impossible; _temp150.f1=({
unsigned char* _temp151="bad malloc access path"; struct _tagged_string _temp152;
_temp152.curr= _temp151; _temp152.base= _temp151; _temp152.last_plus_one=
_temp151 + 23; _temp152;}); _temp150;}); _temp149;})); _LL133:;} return _temp130;}}}}}
struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct
Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp153=( void*) e->r; struct Cyc_Absyn_Exp*
_temp241; struct Cyc_Absyn_Exp* _temp243; struct Cyc_List_List* _temp245; struct
Cyc_Absyn_Exp* _temp247; void* _temp249; int _temp251; void* _temp253; void*
_temp255; int _temp257; void* _temp259; void* _temp261; void* _temp263; struct
Cyc_Absyn_Enumfield* _temp265; struct Cyc_Absyn_Enumdecl* _temp267; struct
_tuple1* _temp269; void* _temp271; struct Cyc_Absyn_Fndecl* _temp273; struct
_tuple1* _temp275; void* _temp277; struct Cyc_Absyn_Vardecl* _temp279; struct
_tuple1* _temp281; void* _temp283; struct Cyc_Absyn_Vardecl* _temp285; struct
_tuple1* _temp287; void* _temp289; struct Cyc_Absyn_Vardecl* _temp291; struct
_tuple1* _temp293; void* _temp295; struct Cyc_Absyn_Vardecl* _temp297; struct
_tuple1* _temp299; struct Cyc_Absyn_Exp* _temp301; struct Cyc_Core_Opt* _temp303;
struct Cyc_Absyn_Exp* _temp305; struct Cyc_Absyn_Exp* _temp307; struct Cyc_Core_Opt*
_temp309; struct Cyc_Absyn_Exp* _temp311; struct Cyc_List_List* _temp313; void*
_temp315; struct Cyc_Absyn_Exp* _temp317; void* _temp319; struct Cyc_Absyn_VarargCallInfo*
_temp321; struct Cyc_List_List* _temp323; struct Cyc_Absyn_Exp* _temp325; struct
Cyc_Absyn_Exp* _temp327; struct Cyc_Absyn_Exp* _temp329; struct Cyc_Absyn_Exp*
_temp331; struct Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp* _temp335; void*
_temp337; struct Cyc_Absyn_Exp* _temp339; struct Cyc_Absyn_Exp* _temp341; struct
_tagged_string* _temp343; struct Cyc_Absyn_Exp* _temp345; struct _tagged_string*
_temp347; struct Cyc_Absyn_Exp* _temp349; struct Cyc_Absyn_Tunionfield* _temp351;
struct Cyc_Absyn_Tuniondecl* _temp353; struct Cyc_List_List* _temp355; struct
Cyc_Core_Opt* _temp357; struct Cyc_Core_Opt* _temp359; struct Cyc_List_List*
_temp361; struct Cyc_List_List* _temp363; void* _temp365; struct Cyc_Absyn_Structdecl*
_temp367; struct Cyc_List_List* _temp369; struct Cyc_Core_Opt* _temp371; struct
_tuple1* _temp373; struct Cyc_List_List* _temp375; void* _temp377; struct Cyc_Absyn_Exp*
_temp379; struct Cyc_Absyn_Exp* _temp381; struct Cyc_Absyn_Exp* _temp383; struct
Cyc_Absyn_Exp* _temp385; struct Cyc_Absyn_Exp* _temp387; struct Cyc_Absyn_Stmt*
_temp389; struct Cyc_Absyn_Exp* _temp391; struct Cyc_Absyn_Exp* _temp393; struct
Cyc_Absyn_Exp* _temp395; struct Cyc_Absyn_Exp* _temp397; struct Cyc_Absyn_Vardecl*
_temp399; void* _temp401; struct _tuple1* _temp403; struct _tuple1* _temp405;
struct Cyc_List_List* _temp407; struct Cyc_Absyn_Exp* _temp409; struct Cyc_List_List*
_temp411; struct Cyc_Core_Opt* _temp413; struct Cyc_List_List* _temp415; struct
_tuple2* _temp417; struct Cyc_Absyn_Fndecl* _temp419; struct Cyc_Absyn_Exp*
_temp421; _LL155: if(*(( int*) _temp153) == Cyc_Absyn_NoInstantiate_e){ _LL242:
_temp241=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp153)->f1; goto _LL156;}
else{ goto _LL157;} _LL157: if(*(( int*) _temp153) == Cyc_Absyn_Sizeofexp_e){
_LL244: _temp243=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp153)->f1; goto
_LL158;} else{ goto _LL159;} _LL159: if(*(( int*) _temp153) == Cyc_Absyn_Instantiate_e){
_LL248: _temp247=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp153)->f1; goto
_LL246; _LL246: _temp245=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp153)->f2;
goto _LL160;} else{ goto _LL161;} _LL161: if(*(( int*) _temp153) == Cyc_Absyn_Const_e){
_LL250: _temp249=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp153)->f1; if((
unsigned int) _temp249 > 1u?*(( int*) _temp249) == Cyc_Absyn_Int_c: 0){ _LL254:
_temp253=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp249)->f1; if( _temp253
==( void*) Cyc_Absyn_Signed){ goto _LL252;} else{ goto _LL163;} _LL252: _temp251=((
struct Cyc_Absyn_Int_c_struct*) _temp249)->f2; if( _temp251 == 0){ goto _LL162;}
else{ goto _LL163;}} else{ goto _LL163;}} else{ goto _LL163;} _LL163: if(*(( int*)
_temp153) == Cyc_Absyn_Const_e){ _LL256: _temp255=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp153)->f1; if(( unsigned int) _temp255 > 1u?*(( int*) _temp255) == Cyc_Absyn_Int_c:
0){ _LL260: _temp259=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp255)->f1;
if( _temp259 ==( void*) Cyc_Absyn_Signed){ goto _LL258;} else{ goto _LL165;}
_LL258: _temp257=(( struct Cyc_Absyn_Int_c_struct*) _temp255)->f2; if( _temp257
== 1){ goto _LL164;} else{ goto _LL165;}} else{ goto _LL165;}} else{ goto _LL165;}
_LL165: if(*(( int*) _temp153) == Cyc_Absyn_Sizeoftyp_e){ _LL262: _temp261=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp153)->f1; goto _LL166;}
else{ goto _LL167;} _LL167: if(*(( int*) _temp153) == Cyc_Absyn_Const_e){ _LL264:
_temp263=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp153)->f1; goto _LL168;}
else{ goto _LL169;} _LL169: if(*(( int*) _temp153) == Cyc_Absyn_Enum_e){ _LL270:
_temp269=(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f1; goto _LL268; _LL268:
_temp267=(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f2; goto _LL266; _LL266:
_temp265=(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f3; goto _LL170;} else{
goto _LL171;} _LL171: if(*(( int*) _temp153) == Cyc_Absyn_Var_e){ _LL276:
_temp275=(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f1; goto _LL272; _LL272:
_temp271=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f2; if((
unsigned int) _temp271 > 1u?*(( int*) _temp271) == Cyc_Absyn_Funname_b: 0){
_LL274: _temp273=(( struct Cyc_Absyn_Funname_b_struct*) _temp271)->f1; goto
_LL172;} else{ goto _LL173;}} else{ goto _LL173;} _LL173: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL282: _temp281=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL278; _LL278: _temp277=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp277 > 1u?*(( int*) _temp277) == Cyc_Absyn_Global_b:
0){ _LL280: _temp279=(( struct Cyc_Absyn_Global_b_struct*) _temp277)->f1; goto
_LL174;} else{ goto _LL175;}} else{ goto _LL175;} _LL175: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL288: _temp287=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL284; _LL284: _temp283=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp283 > 1u?*(( int*) _temp283) == Cyc_Absyn_Param_b:
0){ _LL286: _temp285=(( struct Cyc_Absyn_Param_b_struct*) _temp283)->f1; goto
_LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL294: _temp293=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL290; _LL290: _temp289=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp289 > 1u?*(( int*) _temp289) == Cyc_Absyn_Local_b:
0){ _LL292: _temp291=(( struct Cyc_Absyn_Local_b_struct*) _temp289)->f1; goto
_LL178;} else{ goto _LL179;}} else{ goto _LL179;} _LL179: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL300: _temp299=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL296; _LL296: _temp295=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp295 > 1u?*(( int*) _temp295) == Cyc_Absyn_Pat_b:
0){ _LL298: _temp297=(( struct Cyc_Absyn_Pat_b_struct*) _temp295)->f1; goto
_LL180;} else{ goto _LL181;}} else{ goto _LL181;} _LL181: if(*(( int*) _temp153)
== Cyc_Absyn_AssignOp_e){ _LL306: _temp305=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp153)->f1; goto _LL304; _LL304: _temp303=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp153)->f2; if( _temp303 == 0){ goto _LL302;} else{ goto _LL183;} _LL302:
_temp301=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f3; goto _LL182;}
else{ goto _LL183;} _LL183: if(*(( int*) _temp153) == Cyc_Absyn_AssignOp_e){
_LL312: _temp311=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f1; goto
_LL310; _LL310: _temp309=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f2;
goto _LL308; _LL308: _temp307=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f3;
goto _LL184;} else{ goto _LL185;} _LL185: if(*(( int*) _temp153) == Cyc_Absyn_Primop_e){
_LL316: _temp315=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp153)->f1;
goto _LL314; _LL314: _temp313=(( struct Cyc_Absyn_Primop_e_struct*) _temp153)->f2;
goto _LL186;} else{ goto _LL187;} _LL187: if(*(( int*) _temp153) == Cyc_Absyn_Cast_e){
_LL320: _temp319=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp153)->f1; goto
_LL318; _LL318: _temp317=(( struct Cyc_Absyn_Cast_e_struct*) _temp153)->f2; goto
_LL188;} else{ goto _LL189;} _LL189: if(*(( int*) _temp153) == Cyc_Absyn_FnCall_e){
_LL326: _temp325=(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f1; goto
_LL324; _LL324: _temp323=(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f2;
goto _LL322; _LL322: _temp321=(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f3;
goto _LL190;} else{ goto _LL191;} _LL191: if(*(( int*) _temp153) == Cyc_Absyn_Subscript_e){
_LL330: _temp329=(( struct Cyc_Absyn_Subscript_e_struct*) _temp153)->f1; goto
_LL328; _LL328: _temp327=(( struct Cyc_Absyn_Subscript_e_struct*) _temp153)->f2;
goto _LL192;} else{ goto _LL193;} _LL193: if(*(( int*) _temp153) == Cyc_Absyn_Address_e){
_LL332: _temp331=(( struct Cyc_Absyn_Address_e_struct*) _temp153)->f1; goto
_LL194;} else{ goto _LL195;} _LL195: if(*(( int*) _temp153) == Cyc_Absyn_New_e){
_LL336: _temp335=(( struct Cyc_Absyn_New_e_struct*) _temp153)->f1; goto _LL334;
_LL334: _temp333=(( struct Cyc_Absyn_New_e_struct*) _temp153)->f2; goto _LL196;}
else{ goto _LL197;} _LL197: if(*(( int*) _temp153) == Cyc_Absyn_Malloc_e){
_LL340: _temp339=(( struct Cyc_Absyn_Malloc_e_struct*) _temp153)->f1; goto
_LL338; _LL338: _temp337=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp153)->f2;
goto _LL198;} else{ goto _LL199;} _LL199: if(*(( int*) _temp153) == Cyc_Absyn_Deref_e){
_LL342: _temp341=(( struct Cyc_Absyn_Deref_e_struct*) _temp153)->f1; goto _LL200;}
else{ goto _LL201;} _LL201: if(*(( int*) _temp153) == Cyc_Absyn_StructArrow_e){
_LL346: _temp345=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp153)->f1; goto
_LL344; _LL344: _temp343=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp153)->f2;
goto _LL202;} else{ goto _LL203;} _LL203: if(*(( int*) _temp153) == Cyc_Absyn_StructMember_e){
_LL350: _temp349=(( struct Cyc_Absyn_StructMember_e_struct*) _temp153)->f1; goto
_LL348; _LL348: _temp347=(( struct Cyc_Absyn_StructMember_e_struct*) _temp153)->f2;
goto _LL204;} else{ goto _LL205;} _LL205: if(*(( int*) _temp153) == Cyc_Absyn_Tunion_e){
_LL360: _temp359=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f1; goto
_LL358; _LL358: _temp357=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f2;
goto _LL356; _LL356: _temp355=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f3;
goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f4;
goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f5;
goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp153) == Cyc_Absyn_Tuple_e){
_LL362: _temp361=(( struct Cyc_Absyn_Tuple_e_struct*) _temp153)->f1; goto _LL208;}
else{ goto _LL209;} _LL209: if(*(( int*) _temp153) == Cyc_Absyn_AnonStruct_e){
_LL366: _temp365=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp153)->f1;
goto _LL364; _LL364: _temp363=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp153)->f2;
goto _LL210;} else{ goto _LL211;} _LL211: if(*(( int*) _temp153) == Cyc_Absyn_Struct_e){
_LL374: _temp373=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f1; goto
_LL372; _LL372: _temp371=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f2;
goto _LL370; _LL370: _temp369=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f3;
goto _LL368; _LL368: _temp367=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f4;
goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp153) == Cyc_Absyn_Array_e){
_LL376: _temp375=(( struct Cyc_Absyn_Array_e_struct*) _temp153)->f1; goto _LL214;}
else{ goto _LL215;} _LL215: if(*(( int*) _temp153) == Cyc_Absyn_Increment_e){
_LL380: _temp379=(( struct Cyc_Absyn_Increment_e_struct*) _temp153)->f1; goto
_LL378; _LL378: _temp377=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp153)->f2; goto _LL216;} else{ goto _LL217;} _LL217: if(*(( int*) _temp153)
== Cyc_Absyn_Throw_e){ _LL382: _temp381=(( struct Cyc_Absyn_Throw_e_struct*)
_temp153)->f1; goto _LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp153)
== Cyc_Absyn_Conditional_e){ _LL388: _temp387=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f1; goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f2; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(*(( int*) _temp153)
== Cyc_Absyn_StmtExp_e){ _LL390: _temp389=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp153)->f1; goto _LL222;} else{ goto _LL223;} _LL223: if(*(( int*) _temp153)
== Cyc_Absyn_SeqExp_e){ _LL394: _temp393=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp153)->f1; goto _LL392; _LL392: _temp391=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp153)->f2; goto _LL224;} else{ goto _LL225;} _LL225: if(*(( int*) _temp153)
== Cyc_Absyn_Comprehension_e){ _LL400: _temp399=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f1; goto _LL398; _LL398: _temp397=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f2; goto _LL396; _LL396: _temp395=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f3; goto _LL226;} else{ goto _LL227;} _LL227: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL404: _temp403=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL402; _LL402: _temp401=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if( _temp401 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL228;}
else{ goto _LL229;}} else{ goto _LL229;} _LL229: if(*(( int*) _temp153) == Cyc_Absyn_UnknownId_e){
_LL406: _temp405=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp153)->f1; goto
_LL230;} else{ goto _LL231;} _LL231: if(*(( int*) _temp153) == Cyc_Absyn_UnknownCall_e){
_LL410: _temp409=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp153)->f1; goto
_LL408; _LL408: _temp407=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp153)->f2;
goto _LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp153) == Cyc_Absyn_UnresolvedMem_e){
_LL414: _temp413=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp153)->f1;
goto _LL412; _LL412: _temp411=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp153)->f2; goto _LL234;} else{ goto _LL235;} _LL235: if(*(( int*) _temp153)
== Cyc_Absyn_CompoundLit_e){ _LL418: _temp417=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp153)->f1; goto _LL416; _LL416: _temp415=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp153)->f2; goto _LL236;} else{ goto _LL237;} _LL237: if(*(( int*) _temp153)
== Cyc_Absyn_Codegen_e){ _LL420: _temp419=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp153)->f1; goto _LL238;} else{ goto _LL239;} _LL239: if(*(( int*) _temp153)
== Cyc_Absyn_Fill_e){ _LL422: _temp421=(( struct Cyc_Absyn_Fill_e_struct*)
_temp153)->f1; goto _LL240;} else{ goto _LL154;} _LL156: _temp243= _temp241;
goto _LL158; _LL158: _temp247= _temp243; goto _LL160; _LL160: return Cyc_NewControlFlow_abstract_exp(
env, _temp247); _LL162: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(({ unsigned char* _temp423="new_control_flow.cyc:284 env.access_path==null";
struct _tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 47; _temp424;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp425; _temp425.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp427;
_temp427.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp427.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp427;}); _temp425.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp426; _temp426.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp426.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp426;}); _temp425.lvalues= 0; _temp425;}); _LL164: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp428="new_control_flow.cyc:289 env.access_path==null"; struct _tagged_string
_temp429; _temp429.curr= _temp428; _temp429.base= _temp428; _temp429.last_plus_one=
_temp428 + 47; _temp429;})); return({ struct Cyc_NewControlFlow_AbsSyn _temp430;
_temp430.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp432; _temp432.inner_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp432.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp432;}); _temp430.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp431; _temp431.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp431.assigns=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp431;}); _temp430.lvalues= 0; _temp430;});
_LL166: goto _LL168; _LL168: goto _LL170; _LL170: goto _LL172; _LL172: return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL174: return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL176: _temp291= _temp285; goto _LL178;
_LL178: _temp297= _temp291; goto _LL180; _LL180: { void* absop= Cyc_NewControlFlow_destruct_path(
env, Cyc_CfAbsexp_mkLocalOp( _temp297)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp433=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp433->hd=( void*) absop; _temp433->tl= 0; _temp433;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL182: { struct Cyc_NewControlFlow_AbsSyn
_temp434= Cyc_NewControlFlow_abstract_exp( env, _temp305); goto _LL435; _LL435: {
struct Cyc_List_List* _temp436= _temp434.lvalues == 0? Cyc_NewControlFlow_use_it_list():
_temp434.lvalues; goto _LL437; _LL437: { struct Cyc_NewControlFlow_AbsSyn
_temp438= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp436), _temp301); goto _LL439; _LL439: { void* _temp442; void* _temp444;
struct Cyc_NewControlFlow_AbsSynOne _temp440= Cyc_NewControlFlow_meet_absexp(
_temp434); _LL445: _temp444=( void*) _temp440.inner_exp; goto _LL443; _LL443:
_temp442=( void*) _temp440.assigns; goto _LL441; _LL441: { void* _temp448; void*
_temp450; struct Cyc_NewControlFlow_AbsSynOne _temp446= Cyc_NewControlFlow_meet_absexp(
_temp438); _LL451: _temp450=( void*) _temp446.inner_exp; goto _LL449; _LL449:
_temp448=( void*) _temp446.assigns; goto _LL447; _LL447: { void* _temp452= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp444, _temp450), _temp448); goto _LL453; _LL453: return Cyc_NewControlFlow_mkSyn_tf(
_temp452, 0, _temp442);}}}}}} _LL184: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp454=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp454->hd=( void*) _temp311; _temp454->tl=({
struct Cyc_List_List* _temp455=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp455->hd=( void*) _temp307; _temp455->tl= 0;
_temp455;}); _temp454;}), 0); _LL186: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp313, 0); _LL188: { int both_leaf= 1;{ void* _temp456=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp317->topt))->v; struct Cyc_List_List* _temp466;
struct Cyc_Absyn_Structdecl** _temp468; struct Cyc_List_List* _temp470; struct
_tuple1* _temp472; struct Cyc_List_List* _temp474; _LL458: if(( unsigned int)
_temp456 > 4u?*(( int*) _temp456) == Cyc_Absyn_AnonStructType: 0){ _LL467:
_temp466=(( struct Cyc_Absyn_AnonStructType_struct*) _temp456)->f1; goto _LL459;}
else{ goto _LL460;} _LL460: if(( unsigned int) _temp456 > 4u?*(( int*) _temp456)
== Cyc_Absyn_StructType: 0){ _LL473: _temp472=(( struct Cyc_Absyn_StructType_struct*)
_temp456)->f1; goto _LL471; _LL471: _temp470=(( struct Cyc_Absyn_StructType_struct*)
_temp456)->f2; goto _LL469; _LL469: _temp468=(( struct Cyc_Absyn_StructType_struct*)
_temp456)->f3; goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int)
_temp456 > 4u?*(( int*) _temp456) == Cyc_Absyn_TupleType: 0){ _LL475: _temp474=((
struct Cyc_Absyn_TupleType_struct*) _temp456)->f1; goto _LL463;} else{ goto
_LL464;} _LL464: goto _LL465; _LL459: goto _LL461; _LL461: goto _LL463; _LL463:
both_leaf= 0; goto _LL457; _LL465: goto _LL457; _LL457:;}{ void* _temp476=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; struct Cyc_List_List* _temp486;
struct Cyc_Absyn_Structdecl** _temp488; struct Cyc_List_List* _temp490; struct
_tuple1* _temp492; struct Cyc_List_List* _temp494; _LL478: if(( unsigned int)
_temp476 > 4u?*(( int*) _temp476) == Cyc_Absyn_AnonStructType: 0){ _LL487:
_temp486=(( struct Cyc_Absyn_AnonStructType_struct*) _temp476)->f1; goto _LL479;}
else{ goto _LL480;} _LL480: if(( unsigned int) _temp476 > 4u?*(( int*) _temp476)
== Cyc_Absyn_StructType: 0){ _LL493: _temp492=(( struct Cyc_Absyn_StructType_struct*)
_temp476)->f1; goto _LL491; _LL491: _temp490=(( struct Cyc_Absyn_StructType_struct*)
_temp476)->f2; goto _LL489; _LL489: _temp488=(( struct Cyc_Absyn_StructType_struct*)
_temp476)->f3; goto _LL481;} else{ goto _LL482;} _LL482: if(( unsigned int)
_temp476 > 4u?*(( int*) _temp476) == Cyc_Absyn_TupleType: 0){ _LL495: _temp494=((
struct Cyc_Absyn_TupleType_struct*) _temp476)->f1; goto _LL483;} else{ goto
_LL484;} _LL484: goto _LL485; _LL479: goto _LL481; _LL481: goto _LL483; _LL483:
both_leaf= 0; goto _LL477; _LL485: goto _LL477; _LL477:;} if( both_leaf){ return
Cyc_NewControlFlow_abstract_exp( env, _temp317);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp496=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp496->hd=( void*) _temp317; _temp496->tl= 0;
_temp496;}), env.lhsides);} _LL190: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp497=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp497->hd=( void*) _temp325; _temp497->tl=
_temp323; _temp497;}), 0); _LL192: { void* _temp498= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp329->topt))->v); struct Cyc_List_List*
_temp504; _LL500: if(( unsigned int) _temp498 > 4u?*(( int*) _temp498) == Cyc_Absyn_TupleType:
0){ _LL505: _temp504=(( struct Cyc_Absyn_TupleType_struct*) _temp498)->f1; goto
_LL501;} else{ goto _LL502;} _LL502: goto _LL503; _LL501: return Cyc_NewControlFlow_descend_path(
env, e, _temp329); _LL503: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp506=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp506->hd=( void*) _temp329; _temp506->tl=({ struct
Cyc_List_List* _temp507=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp507->hd=( void*) _temp327; _temp507->tl= 0; _temp507;}); _temp506;}), Cyc_NewControlFlow_use_it_list());
_LL499:;} _LL194: { int is_malloc= 0;{ void* _temp508=( void*) _temp331->r;
struct Cyc_Absyn_Structdecl* _temp516; struct Cyc_List_List* _temp518; struct
Cyc_Core_Opt* _temp520; struct _tuple1* _temp522; struct Cyc_List_List* _temp524;
_LL510: if(*(( int*) _temp508) == Cyc_Absyn_Struct_e){ _LL523: _temp522=((
struct Cyc_Absyn_Struct_e_struct*) _temp508)->f1; goto _LL521; _LL521: _temp520=((
struct Cyc_Absyn_Struct_e_struct*) _temp508)->f2; goto _LL519; _LL519: _temp518=((
struct Cyc_Absyn_Struct_e_struct*) _temp508)->f3; goto _LL517; _LL517: _temp516=((
struct Cyc_Absyn_Struct_e_struct*) _temp508)->f4; goto _LL511;} else{ goto
_LL512;} _LL512: if(*(( int*) _temp508) == Cyc_Absyn_Tuple_e){ _LL525: _temp524=((
struct Cyc_Absyn_Tuple_e_struct*) _temp508)->f1; goto _LL513;} else{ goto _LL514;}
_LL514: goto _LL515; _LL511: goto _LL513; _LL513: is_malloc= 1; goto _LL509;
_LL515: goto _LL509; _LL509:;} if( is_malloc){ _temp335= 0; _temp333= _temp331;
goto _LL196;}{ struct Cyc_List_List* _temp526= env.access_path; struct Cyc_List_List
_temp538; struct Cyc_List_List* _temp539; struct Cyc_Absyn_Exp* _temp541; struct
Cyc_Absyn_Exp _temp543; struct Cyc_Position_Segment* _temp544; void* _temp546;
struct Cyc_Absyn_Exp* _temp548; struct Cyc_Absyn_Exp* _temp550; struct Cyc_Core_Opt*
_temp552; struct Cyc_List_List _temp554; struct Cyc_List_List* _temp555; struct
Cyc_Absyn_Exp* _temp557; struct Cyc_Absyn_Exp _temp559; struct Cyc_Position_Segment*
_temp560; void* _temp562; struct Cyc_Absyn_Exp* _temp564; struct Cyc_Core_Opt*
_temp566; struct Cyc_List_List _temp568; struct Cyc_List_List* _temp569; struct
Cyc_Absyn_Exp* _temp571; struct Cyc_Absyn_Exp _temp573; struct Cyc_Position_Segment*
_temp574; void* _temp576; struct _tagged_string* _temp578; struct Cyc_Absyn_Exp*
_temp580; struct Cyc_Core_Opt* _temp582; _LL528: if( _temp526 == 0){ goto _LL529;}
else{ goto _LL530;} _LL530: if( _temp526 == 0){ goto _LL532;} else{ _temp538=*
_temp526; _LL542: _temp541=( struct Cyc_Absyn_Exp*) _temp538.hd; _temp543=*
_temp541; _LL553: _temp552= _temp543.topt; goto _LL547; _LL547: _temp546=( void*)
_temp543.r; if(*(( int*) _temp546) == Cyc_Absyn_Subscript_e){ _LL551: _temp550=((
struct Cyc_Absyn_Subscript_e_struct*) _temp546)->f1; goto _LL549; _LL549:
_temp548=(( struct Cyc_Absyn_Subscript_e_struct*) _temp546)->f2; goto _LL545;}
else{ goto _LL532;} _LL545: _temp544= _temp543.loc; goto _LL540; _LL540:
_temp539= _temp538.tl; goto _LL531;} _LL532: if( _temp526 == 0){ goto _LL534;}
else{ _temp554=* _temp526; _LL558: _temp557=( struct Cyc_Absyn_Exp*) _temp554.hd;
_temp559=* _temp557; _LL567: _temp566= _temp559.topt; goto _LL563; _LL563:
_temp562=( void*) _temp559.r; if(*(( int*) _temp562) == Cyc_Absyn_Deref_e){
_LL565: _temp564=(( struct Cyc_Absyn_Deref_e_struct*) _temp562)->f1; goto _LL561;}
else{ goto _LL534;} _LL561: _temp560= _temp559.loc; goto _LL556; _LL556:
_temp555= _temp554.tl; goto _LL533;} _LL534: if( _temp526 == 0){ goto _LL536;}
else{ _temp568=* _temp526; _LL572: _temp571=( struct Cyc_Absyn_Exp*) _temp568.hd;
_temp573=* _temp571; _LL583: _temp582= _temp573.topt; goto _LL577; _LL577:
_temp576=( void*) _temp573.r; if(*(( int*) _temp576) == Cyc_Absyn_StructArrow_e){
_LL581: _temp580=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp576)->f1; goto
_LL579; _LL579: _temp578=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp576)->f2;
goto _LL575;} else{ goto _LL536;} _LL575: _temp574= _temp573.loc; goto _LL570;
_LL570: _temp569= _temp568.tl; goto _LL535;} _LL536: goto _LL537; _LL529: env.access_path=({
struct Cyc_List_List* _temp584=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp584->hd=( void*) e; _temp584->tl= env.access_path;
_temp584;}); goto _LL527; _LL531: Cyc_Evexp_eval_const_uint_exp( _temp548) == 0?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp585="new_control_flow.cyc:411 Evexp::eval_const_uint_exp(e2) == 0"; struct
_tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 61; _temp586;})); _temp555= _temp539; goto
_LL533; _LL533: env.access_path= _temp555; goto _LL527; _LL535: env.access_path=({
struct Cyc_List_List* _temp587=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp587->hd=( void*) Cyc_Absyn_structmember_exp(
_temp580, _temp578, 0); _temp587->tl= _temp569; _temp587;}); goto _LL527; _LL537:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp588=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp588[ 0]=({ struct
Cyc_Core_Impossible_struct _temp589; _temp589.tag= Cyc_Core_Impossible; _temp589.f1=({
unsigned char* _temp590="abstract_exp: bad access path for &"; struct
_tagged_string _temp591; _temp591.curr= _temp590; _temp591.base= _temp590;
_temp591.last_plus_one= _temp590 + 36; _temp591;}); _temp589;}); _temp588;}));
_LL527:;} return Cyc_NewControlFlow_abstract_exp( env, _temp331);} _LL196: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp333->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, _temp335,( struct Cyc_Absyn_Exp*)
_temp333); _LL198: Cyc_NewControlFlow_add_malloc_root( env, e, _temp337); return
Cyc_NewControlFlow_abstract_malloc( env, e, _temp339, 0); _LL200: { struct Cyc_List_List*
_temp592= env.access_path; struct Cyc_List_List _temp598; struct Cyc_List_List*
_temp599; struct Cyc_Absyn_Exp* _temp601; struct Cyc_Absyn_Exp _temp603; struct
Cyc_Position_Segment* _temp604; void* _temp606; struct Cyc_Absyn_Exp* _temp608;
struct Cyc_Core_Opt* _temp610; _LL594: if( _temp592 == 0){ goto _LL596;} else{
_temp598=* _temp592; _LL602: _temp601=( struct Cyc_Absyn_Exp*) _temp598.hd;
_temp603=* _temp601; _LL611: _temp610= _temp603.topt; goto _LL607; _LL607:
_temp606=( void*) _temp603.r; if(*(( int*) _temp606) == Cyc_Absyn_Address_e){
_LL609: _temp608=(( struct Cyc_Absyn_Address_e_struct*) _temp606)->f1; goto
_LL605;} else{ goto _LL596;} _LL605: _temp604= _temp603.loc; goto _LL600; _LL600:
_temp599= _temp598.tl; goto _LL595;} _LL596: goto _LL597; _LL595: env.access_path=
_temp599;{ struct Cyc_NewControlFlow_AbsSyn _temp612= Cyc_NewControlFlow_abstract_exp(
env, _temp341); goto _LL613; _LL613: if( _temp612.lvalues == 0){ _temp612.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp612;} _LL597: return Cyc_NewControlFlow_descend_path(
env, e, _temp341); _LL593:;} _LL202: { void* _temp614= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp345->topt))->v); struct Cyc_Absyn_PtrInfo
_temp620; struct Cyc_Absyn_Conref* _temp622; struct Cyc_Absyn_Tqual _temp624;
struct Cyc_Absyn_Conref* _temp626; void* _temp628; void* _temp630; _LL616: if((
unsigned int) _temp614 > 4u?*(( int*) _temp614) == Cyc_Absyn_PointerType: 0){
_LL621: _temp620=(( struct Cyc_Absyn_PointerType_struct*) _temp614)->f1; _LL631:
_temp630=( void*) _temp620.elt_typ; goto _LL629; _LL629: _temp628=( void*)
_temp620.rgn_typ; goto _LL627; _LL627: _temp626= _temp620.nullable; goto _LL625;
_LL625: _temp624= _temp620.tq; goto _LL623; _LL623: _temp622= _temp620.bounds;
goto _LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL617:{ void* _temp632=
Cyc_Tcutil_compress( _temp630); struct Cyc_List_List* _temp644; struct Cyc_Absyn_Structdecl**
_temp646; struct Cyc_List_List* _temp648; struct _tuple1* _temp650; struct Cyc_List_List*
_temp652; struct Cyc_Absyn_Uniondecl** _temp654; struct Cyc_List_List* _temp656;
struct _tuple1* _temp658; _LL634: if(( unsigned int) _temp632 > 4u?*(( int*)
_temp632) == Cyc_Absyn_AnonStructType: 0){ _LL645: _temp644=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp632)->f1; goto _LL635;} else{ goto _LL636;} _LL636: if(( unsigned int)
_temp632 > 4u?*(( int*) _temp632) == Cyc_Absyn_StructType: 0){ _LL651: _temp650=((
struct Cyc_Absyn_StructType_struct*) _temp632)->f1; goto _LL649; _LL649:
_temp648=(( struct Cyc_Absyn_StructType_struct*) _temp632)->f2; goto _LL647;
_LL647: _temp646=(( struct Cyc_Absyn_StructType_struct*) _temp632)->f3; goto
_LL637;} else{ goto _LL638;} _LL638: if(( unsigned int) _temp632 > 4u?*(( int*)
_temp632) == Cyc_Absyn_AnonUnionType: 0){ _LL653: _temp652=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp632)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int)
_temp632 > 4u?*(( int*) _temp632) == Cyc_Absyn_UnionType: 0){ _LL659: _temp658=((
struct Cyc_Absyn_UnionType_struct*) _temp632)->f1; goto _LL657; _LL657: _temp656=((
struct Cyc_Absyn_UnionType_struct*) _temp632)->f2; goto _LL655; _LL655: _temp654=((
struct Cyc_Absyn_UnionType_struct*) _temp632)->f3; goto _LL641;} else{ goto
_LL642;} _LL642: goto _LL643; _LL635: goto _LL637; _LL637: return Cyc_NewControlFlow_descend_path(
env, e, _temp345); _LL639: goto _LL641; _LL641: { struct Cyc_NewControlFlow_AbsSyn
_temp660= Cyc_NewControlFlow_abstract_exp( env, _temp345); goto _LL661; _LL661:
_temp660.lvalues= Cyc_NewControlFlow_use_it_list(); return _temp660;} _LL643:
goto _LL633; _LL633:;} goto _LL619; _LL619:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp662=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp662[ 0]=({ struct Cyc_Core_Impossible_struct _temp663; _temp663.tag= Cyc_Core_Impossible;
_temp663.f1=({ unsigned char* _temp664="NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp665; _temp665.curr= _temp664; _temp665.base= _temp664;
_temp665.last_plus_one= _temp664 + 40; _temp665;}); _temp663;}); _temp662;}));
_LL615:;} _LL204: { void* _temp666= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp349->topt))->v); struct Cyc_List_List* _temp678; struct Cyc_Absyn_Structdecl**
_temp680; struct Cyc_List_List* _temp682; struct _tuple1* _temp684; struct Cyc_List_List*
_temp686; struct Cyc_Absyn_Uniondecl** _temp688; struct Cyc_List_List* _temp690;
struct _tuple1* _temp692; _LL668: if(( unsigned int) _temp666 > 4u?*(( int*)
_temp666) == Cyc_Absyn_AnonStructType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp666)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int)
_temp666 > 4u?*(( int*) _temp666) == Cyc_Absyn_StructType: 0){ _LL685: _temp684=((
struct Cyc_Absyn_StructType_struct*) _temp666)->f1; goto _LL683; _LL683:
_temp682=(( struct Cyc_Absyn_StructType_struct*) _temp666)->f2; goto _LL681;
_LL681: _temp680=(( struct Cyc_Absyn_StructType_struct*) _temp666)->f3; goto
_LL671;} else{ goto _LL672;} _LL672: if(( unsigned int) _temp666 > 4u?*(( int*)
_temp666) == Cyc_Absyn_AnonUnionType: 0){ _LL687: _temp686=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp666)->f1; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp666 > 4u?*(( int*) _temp666) == Cyc_Absyn_UnionType: 0){ _LL693: _temp692=((
struct Cyc_Absyn_UnionType_struct*) _temp666)->f1; goto _LL691; _LL691: _temp690=((
struct Cyc_Absyn_UnionType_struct*) _temp666)->f2; goto _LL689; _LL689: _temp688=((
struct Cyc_Absyn_UnionType_struct*) _temp666)->f3; goto _LL675;} else{ goto
_LL676;} _LL676: goto _LL677; _LL669: goto _LL671; _LL671: return Cyc_NewControlFlow_descend_path(
env, e, _temp349); _LL673: goto _LL675; _LL675: { struct Cyc_NewControlFlow_AbsSyn
_temp694= Cyc_NewControlFlow_abstract_exp( env, _temp349); goto _LL695; _LL695:
_temp694.lvalues= Cyc_NewControlFlow_use_it_list(); return _temp694;} _LL677:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp696=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp696[ 0]=({ struct
Cyc_Core_Impossible_struct _temp697; _temp697.tag= Cyc_Core_Impossible; _temp697.f1=({
unsigned char* _temp698="NewControlFlow: bad type in StructMember"; struct
_tagged_string _temp699; _temp699.curr= _temp698; _temp699.base= _temp698;
_temp699.last_plus_one= _temp698 + 41; _temp699;}); _temp697;}); _temp696;}));
_LL667:;} _LL206: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(({ unsigned char* _temp700="new_control_flow.cyc:481 env.access_path == null";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 49; _temp701;})); if( _temp355 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));} _temp361=
_temp355; goto _LL208; _LL208: { struct Cyc_List_List* _temp702= env.access_path;
struct Cyc_List_List _temp710; struct Cyc_List_List* _temp711; struct Cyc_Absyn_Exp*
_temp713; struct Cyc_Absyn_Exp _temp715; struct Cyc_Position_Segment* _temp716;
void* _temp718; struct Cyc_Absyn_Exp* _temp720; struct Cyc_Absyn_Exp* _temp722;
struct Cyc_Core_Opt* _temp724; _LL704: if( _temp702 == 0){ goto _LL705;} else{
goto _LL706;} _LL706: if( _temp702 == 0){ goto _LL708;} else{ _temp710=*
_temp702; _LL714: _temp713=( struct Cyc_Absyn_Exp*) _temp710.hd; _temp715=*
_temp713; _LL725: _temp724= _temp715.topt; goto _LL719; _LL719: _temp718=( void*)
_temp715.r; if(*(( int*) _temp718) == Cyc_Absyn_Subscript_e){ _LL723: _temp722=((
struct Cyc_Absyn_Subscript_e_struct*) _temp718)->f1; goto _LL721; _LL721:
_temp720=(( struct Cyc_Absyn_Subscript_e_struct*) _temp718)->f2; goto _LL717;}
else{ goto _LL708;} _LL717: _temp716= _temp715.loc; goto _LL712; _LL712:
_temp711= _temp710.tl; goto _LL707;} _LL708: goto _LL709; _LL705: { void*
_temp726= Cyc_CfAbsexp_mkSkipAE(); goto _LL727; _LL727: { void* _temp728= Cyc_CfAbsexp_mkSkipAE();
goto _LL729; _LL729:{ int j= 0; for( 0; _temp361 != 0;( _temp361=(( struct Cyc_List_List*)
_check_null( _temp361))->tl, ++ j)){ struct Cyc_List_List* _temp730= 0; goto
_LL731; _LL731:{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=((
struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp730=({ struct Cyc_List_List*
_temp732=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp732->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp732->tl= _temp730;
_temp732;});} else{ _temp730=({ struct Cyc_List_List* _temp733=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp733->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp734=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp734[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp735; _temp735.tag= Cyc_CfFlowInfo_TupleF;
_temp735.f1= j; _temp735;}); _temp734;})); _temp733->tl= _temp730; _temp733;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp736= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp730),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp361))->hd); goto _LL737; _LL737: { void* _temp740; void* _temp742; struct
Cyc_NewControlFlow_AbsSynOne _temp738= Cyc_NewControlFlow_meet_absexp( _temp736);
_LL743: _temp742=( void*) _temp738.inner_exp; goto _LL741; _LL741: _temp740=(
void*) _temp738.assigns; goto _LL739; _LL739: _temp726= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp726, _temp742); _temp728= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp728, _temp740);}}}} return Cyc_NewControlFlow_mkSyn_tf(
_temp726, 0, _temp728);}} _LL707: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp720); struct Cyc_NewControlFlow_AbsEnv _temp744= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL745; _LL745: _temp744.access_path= 0; env.access_path= _temp711;{
void* _temp746= Cyc_CfAbsexp_mkSkipAE(); goto _LL747; _LL747: { void* _temp748=
Cyc_CfAbsexp_mkSkipAE(); goto _LL749; _LL749:{ int j= 0; for( 0; _temp361 != 0;(
_temp361=(( struct Cyc_List_List*) _check_null( _temp361))->tl, ++ j)){ if( i ==
j){ void* _temp752; void* _temp754; struct Cyc_NewControlFlow_AbsSynOne _temp750=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp361))->hd)); _LL755:
_temp754=( void*) _temp750.inner_exp; goto _LL753; _LL753: _temp752=( void*)
_temp750.assigns; goto _LL751; _LL751: _temp748= _temp752; _temp746= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp746, _temp754);} else{ void* _temp758; void*
_temp760; struct Cyc_NewControlFlow_AbsSynOne _temp756= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp744,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp361))->hd)); _LL761: _temp760=( void*) _temp756.inner_exp;
goto _LL759; _LL759: _temp758=( void*) _temp756.assigns; goto _LL757; _LL757:
_temp746= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp746,
_temp760);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp746,({ struct Cyc_List_List*
_temp762=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp762->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp762->tl= 0; _temp762;}),
_temp748);}}} _LL709:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp763=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp763[ 0]=({ struct Cyc_Core_Impossible_struct _temp764; _temp764.tag= Cyc_Core_Impossible;
_temp764.f1=({ unsigned char* _temp765="abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp766; _temp766.curr= _temp765; _temp766.base= _temp765;
_temp766.last_plus_one= _temp765 + 46; _temp766;}); _temp764;}); _temp763;}));
_LL703:;} _LL210: _temp369= _temp363; goto _LL212; _LL212: { struct Cyc_List_List*
_temp767= env.access_path; struct Cyc_List_List _temp775; struct Cyc_List_List*
_temp776; struct Cyc_Absyn_Exp* _temp778; struct Cyc_Absyn_Exp _temp780; struct
Cyc_Position_Segment* _temp781; void* _temp783; struct _tagged_string* _temp785;
struct Cyc_Absyn_Exp* _temp787; struct Cyc_Core_Opt* _temp789; _LL769: if(
_temp767 == 0){ goto _LL770;} else{ goto _LL771;} _LL771: if( _temp767 == 0){
goto _LL773;} else{ _temp775=* _temp767; _LL779: _temp778=( struct Cyc_Absyn_Exp*)
_temp775.hd; _temp780=* _temp778; _LL790: _temp789= _temp780.topt; goto _LL784;
_LL784: _temp783=( void*) _temp780.r; if(*(( int*) _temp783) == Cyc_Absyn_StructMember_e){
_LL788: _temp787=(( struct Cyc_Absyn_StructMember_e_struct*) _temp783)->f1; goto
_LL786; _LL786: _temp785=(( struct Cyc_Absyn_StructMember_e_struct*) _temp783)->f2;
goto _LL782;} else{ goto _LL773;} _LL782: _temp781= _temp780.loc; goto _LL777;
_LL777: _temp776= _temp775.tl; goto _LL772;} _LL773: goto _LL774; _LL770: { void*
_temp791= Cyc_CfAbsexp_mkSkipAE(); goto _LL792; _LL792: { void* _temp793= Cyc_CfAbsexp_mkSkipAE();
goto _LL794; _LL794: for( 0; _temp369 != 0; _temp369=(( struct Cyc_List_List*)
_check_null( _temp369))->tl){ struct Cyc_List_List* _temp795= 0; goto _LL796;
_LL796:{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=(( struct
Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)((
struct Cyc_List_List*) _check_null( ls))->hd)){ _temp795=({ struct Cyc_List_List*
_temp797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp797->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp797->tl= _temp795;
_temp797;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp369))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp798=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp804; struct _tagged_string*
_temp806; _LL800: if(*(( int*) _temp798) == Cyc_Absyn_ArrayElement){ _LL805:
_temp804=(( struct Cyc_Absyn_ArrayElement_struct*) _temp798)->f1; goto _LL801;}
else{ goto _LL802;} _LL802: if(*(( int*) _temp798) == Cyc_Absyn_FieldName){
_LL807: _temp806=(( struct Cyc_Absyn_FieldName_struct*) _temp798)->f1; goto
_LL803;} else{ goto _LL799;} _LL801:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp808=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp808[ 0]=({ struct Cyc_Core_Impossible_struct _temp809; _temp809.tag= Cyc_Core_Impossible;
_temp809.f1=({ unsigned char* _temp810="bad struct designator"; struct
_tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 22; _temp811;}); _temp809;}); _temp808;}));
_LL803: _temp795=({ struct Cyc_List_List* _temp812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp812->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp813=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp813[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp814; _temp814.tag= Cyc_CfFlowInfo_StructF;
_temp814.f1= _temp806; _temp814;}); _temp813;})); _temp812->tl= _temp795;
_temp812;}); goto _LL799; _LL799:;}}}}{ void** _temp815=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp369))->hd)).f2)->topt))->v); goto _LL816; _LL816: { struct Cyc_NewControlFlow_AbsSyn
_temp817= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp795),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp369))->hd)).f2);
goto _LL818; _LL818: { void* _temp821; void* _temp823; struct Cyc_NewControlFlow_AbsSynOne
_temp819= Cyc_NewControlFlow_meet_absexp( _temp817); _LL824: _temp823=( void*)
_temp819.inner_exp; goto _LL822; _LL822: _temp821=( void*) _temp819.assigns;
goto _LL820; _LL820: _temp791= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp791, _temp823); _temp793= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp793, _temp821);}}}} return Cyc_NewControlFlow_mkSyn_tf( _temp791, 0,
_temp793);}} _LL772: { struct Cyc_NewControlFlow_AbsEnv _temp825= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL826; _LL826: _temp825.access_path= 0; env.access_path= _temp776;{
void* _temp827= Cyc_CfAbsexp_mkSkipAE(); goto _LL828; _LL828: { void* _temp829=
Cyc_CfAbsexp_mkSkipAE(); goto _LL830; _LL830: for( 0; _temp369 != 0; _temp369=((
struct Cyc_List_List*) _check_null( _temp369))->tl){ int used= 0;{ struct Cyc_List_List*
ds=(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp369))->hd)).f1;
for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void*
_temp831=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp837; struct _tagged_string* _temp839; _LL833: if(*(( int*) _temp831) == Cyc_Absyn_ArrayElement){
_LL838: _temp837=(( struct Cyc_Absyn_ArrayElement_struct*) _temp831)->f1; goto
_LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp831) == Cyc_Absyn_FieldName){
_LL840: _temp839=(( struct Cyc_Absyn_FieldName_struct*) _temp831)->f1; goto
_LL836;} else{ goto _LL832;} _LL834:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp841=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp841[ 0]=({ struct Cyc_Core_Impossible_struct _temp842; _temp842.tag= Cyc_Core_Impossible;
_temp842.f1=({ unsigned char* _temp843="bad struct designator"; struct
_tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 22; _temp844;}); _temp842;}); _temp841;}));
_LL836: if( Cyc_String_zstrptrcmp( _temp785, _temp839) == 0){ used= 1;} goto
_LL832; _LL832:;}} if( used){ void* _temp847; void* _temp849; struct Cyc_NewControlFlow_AbsSynOne
_temp845= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp369))->hd)).f2));
_LL850: _temp849=( void*) _temp845.inner_exp; goto _LL848; _LL848: _temp847=(
void*) _temp845.assigns; goto _LL846; _LL846: _temp829= _temp847; _temp827= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp827, _temp849);} else{ void* _temp853; void*
_temp855; struct Cyc_NewControlFlow_AbsSynOne _temp851= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp825,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp369))->hd)).f2)); _LL856: _temp855=( void*) _temp851.inner_exp;
goto _LL854; _LL854: _temp853=( void*) _temp851.assigns; goto _LL852; _LL852:
_temp827= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp827,
_temp855);}} return Cyc_NewControlFlow_mkSyn_tf( _temp827, 0, _temp829);}}}
_LL774:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp857=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp857[ 0]=({ struct Cyc_Core_Impossible_struct _temp858; _temp858.tag= Cyc_Core_Impossible;
_temp858.f1=({ unsigned char* _temp859="abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp860; _temp860.curr= _temp859; _temp860.base= _temp859;
_temp860.last_plus_one= _temp859 + 47; _temp860;}); _temp858;}); _temp857;}));
_LL768:;} _LL214: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd, _temp375), 0); _LL216:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp861=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp861->hd=( void*) _temp379; _temp861->tl= 0; _temp861;}), 0); _LL218: {
struct Cyc_NewControlFlow_AbsSyn _temp862= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp381); goto _LL863; _LL863: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( _temp862)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL220: { struct Cyc_List_List*
_temp866; struct Cyc_NewControlFlow_AbsSynOne _temp868; struct Cyc_NewControlFlow_AbsSynOne
_temp870; struct Cyc_NewControlFlow_AbsSyn _temp864= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp387); _LL871: _temp870= _temp864.when_true;
goto _LL869; _LL869: _temp868= _temp864.when_false; goto _LL867; _LL867:
_temp866= _temp864.lvalues; goto _LL865; _LL865: { struct Cyc_List_List*
_temp874; struct Cyc_NewControlFlow_AbsSynOne _temp876; struct Cyc_NewControlFlow_AbsSynOne
_temp878; struct Cyc_NewControlFlow_AbsSyn _temp872= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp385); _LL879: _temp878= _temp872.when_true;
goto _LL877; _LL877: _temp876= _temp872.when_false; goto _LL875; _LL875:
_temp874= _temp872.lvalues; goto _LL873; _LL873: { struct Cyc_List_List*
_temp882; struct Cyc_NewControlFlow_AbsSynOne _temp884; struct Cyc_NewControlFlow_AbsSynOne
_temp886; struct Cyc_NewControlFlow_AbsSyn _temp880= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp383); _LL887: _temp886= _temp880.when_true;
goto _LL885; _LL885: _temp884= _temp880.when_false; goto _LL883; _LL883:
_temp882= _temp880.lvalues; goto _LL881; _LL881: { void* _temp888= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp870.inner_exp,(
void*) _temp870.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp878.inner_exp,( void*) _temp878.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp868.inner_exp,( void*) _temp868.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp886.inner_exp,( void*) _temp886.assigns)));
goto _LL889; _LL889: { void* _temp890= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp870.inner_exp,( void*) _temp870.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp876.inner_exp,(
void*) _temp876.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp868.inner_exp,(
void*) _temp868.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp884.inner_exp,( void*) _temp884.assigns))); goto _LL891; _LL891:
return({ struct Cyc_NewControlFlow_AbsSyn _temp892; _temp892.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp894; _temp894.inner_exp=( void*) _temp888;
_temp894.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp894;}); _temp892.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp893;
_temp893.inner_exp=( void*) _temp890; _temp893.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp893;}); _temp892.lvalues= 0; _temp892;});}}}}}
_LL222: { struct Cyc_NewControlFlow_AbsEnv _temp895= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL896; _LL896: _temp895.access_path= 0; Cyc_NewControlFlow_cf_prepass(
_temp895, _temp389);{ struct Cyc_Absyn_Stmt* _temp897= _temp389; goto _LL898;
_LL898: while( 1) { void* _temp899=( void*) _temp897->r; struct Cyc_Absyn_Stmt*
_temp909; struct Cyc_Absyn_Stmt* _temp911; struct Cyc_Absyn_Stmt* _temp913;
struct Cyc_Absyn_Decl* _temp915; struct Cyc_Absyn_Exp* _temp917; _LL901: if((
unsigned int) _temp899 > 1u?*(( int*) _temp899) == Cyc_Absyn_Seq_s: 0){ _LL912:
_temp911=(( struct Cyc_Absyn_Seq_s_struct*) _temp899)->f1; goto _LL910; _LL910:
_temp909=(( struct Cyc_Absyn_Seq_s_struct*) _temp899)->f2; goto _LL902;} else{
goto _LL903;} _LL903: if(( unsigned int) _temp899 > 1u?*(( int*) _temp899) ==
Cyc_Absyn_Decl_s: 0){ _LL916: _temp915=(( struct Cyc_Absyn_Decl_s_struct*)
_temp899)->f1; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_Decl_s_struct*)
_temp899)->f2; goto _LL904;} else{ goto _LL905;} _LL905: if(( unsigned int)
_temp899 > 1u?*(( int*) _temp899) == Cyc_Absyn_Exp_s: 0){ _LL918: _temp917=((
struct Cyc_Absyn_Exp_s_struct*) _temp899)->f1; goto _LL906;} else{ goto _LL907;}
_LL907: goto _LL908; _LL902: _temp897= _temp909; continue; _LL904: _temp897=
_temp913; continue; _LL906: { struct Cyc_NewControlFlow_AbsSyn _temp919= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp917); goto _LL920; _LL920: { void*
_temp923; void* _temp925; struct Cyc_NewControlFlow_AbsSynOne _temp921= Cyc_NewControlFlow_meet_absexp(
_temp919); _LL926: _temp925=( void*) _temp921.inner_exp; goto _LL924; _LL924:
_temp923=( void*) _temp921.assigns; goto _LL922; _LL922:( void*)((( struct Cyc_List_List*)
_check_null(( Cyc_NewControlFlow_get_stmt_annot( _temp897))->absexps))->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp925, _temp923));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp389), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}} _LL908:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp927=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp927[ 0]=({ struct Cyc_Core_Impossible_struct _temp928; _temp928.tag= Cyc_Core_Impossible;
_temp928.f1=({ unsigned char* _temp929="abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp930; _temp930.curr= _temp929; _temp930.base= _temp929;
_temp930.last_plus_one= _temp929 + 33; _temp930;}); _temp928;}); _temp927;}));
_LL900:;}}} _LL224: { struct Cyc_NewControlFlow_AbsEnv _temp931= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL932; _LL932: _temp931.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn
_temp933= Cyc_NewControlFlow_abstract_exp( _temp931, _temp393); goto _LL934;
_LL934: { struct Cyc_NewControlFlow_AbsSyn _temp935= Cyc_NewControlFlow_abstract_exp(
env, _temp391); goto _LL936; _LL936: { struct Cyc_NewControlFlow_AbsSynOne
_temp937= Cyc_NewControlFlow_meet_absexp( _temp933); goto _LL938; _LL938: return({
struct Cyc_NewControlFlow_AbsSyn _temp939; _temp939.when_true=({ struct Cyc_NewControlFlow_AbsSynOne
_temp941; _temp941.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp937.inner_exp,( void*)( _temp935.when_true).inner_exp); _temp941.assigns=(
void*)(( void*)( _temp935.when_true).assigns); _temp941;}); _temp939.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp940; _temp940.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp937.inner_exp,( void*)( _temp935.when_false).inner_exp);
_temp940.assigns=( void*)(( void*)( _temp935.when_false).assigns); _temp940;});
_temp939.lvalues= 0; _temp939;});}}}} _LL226: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char* _temp942="new_control_flow.cyc:701 env.access_path==null";
struct _tagged_string _temp943; _temp943.curr= _temp942; _temp943.base= _temp942;
_temp943.last_plus_one= _temp942 + 47; _temp943;})); Cyc_NewControlFlow_add_var_root(
env, _temp399);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp944=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp944->hd=( void*) _temp399; _temp944->tl=( env.shared)->comprehension_vars;
_temp944;});{ struct Cyc_NewControlFlow_AbsEnv _temp945= Cyc_NewControlFlow_use_it_env(
env); goto _LL946; _LL946: { void* _temp949; void* _temp951; struct Cyc_NewControlFlow_AbsSynOne
_temp947= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp945, _temp397)); _LL952: _temp951=( void*) _temp947.inner_exp; goto _LL950;
_LL950: _temp949=( void*) _temp947.assigns; goto _LL948; _LL948: { void*
_temp955; void* _temp957; struct Cyc_NewControlFlow_AbsSynOne _temp953= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp945, _temp395)); _LL958: _temp957=( void*)
_temp953.inner_exp; goto _LL956; _LL956: _temp955=( void*) _temp953.assigns;
goto _LL954; _LL954: { void* _temp959= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp951, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp949, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp399), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp957, _temp955), Cyc_CfAbsexp_mkSkipAE()))));
goto _LL960; _LL960: return Cyc_NewControlFlow_mkSyn_tf( _temp959, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}}} _LL228: goto _LL230; _LL230: goto _LL232;
_LL232: goto _LL234; _LL234: goto _LL236; _LL236: goto _LL238; _LL238: goto
_LL240; _LL240:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp961=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp961[ 0]=({ struct Cyc_Core_Impossible_struct _temp962; _temp962.tag= Cyc_Core_Impossible;
_temp962.f1=({ unsigned char* _temp963="abstract_exp, unexpected exp form";
struct _tagged_string _temp964; _temp964.curr= _temp963; _temp964.base= _temp963;
_temp964.last_plus_one= _temp963 + 34; _temp964;}); _temp962;}); _temp961;}));
_LL154:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp967; void* _temp969; struct Cyc_NewControlFlow_AbsSynOne
_temp965= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL970: _temp969=( void*) _temp965.inner_exp; goto _LL968; _LL968: _temp967=(
void*) _temp965.assigns; goto _LL966; _LL966: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp969, _temp967);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp971= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
e); goto _LL972; _LL972: return({ struct _tuple0 _temp973; _temp973.f1= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp971.when_true).inner_exp,( void*)(
_temp971.when_true).assigns); _temp973.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*)( _temp971.when_false).inner_exp,( void*)( _temp971.when_false).assigns);
_temp973;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp974= 0; goto _LL975; _LL975: for( 0; scs != 0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp978; struct Cyc_Position_Segment*
_temp979; struct Cyc_Absyn_Stmt* _temp981; struct Cyc_Absyn_Exp* _temp983;
struct Cyc_Core_Opt* _temp985; struct Cyc_Absyn_Pat* _temp987; struct Cyc_Absyn_Switch_clause*
_temp976=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp978=* _temp976; _LL988: _temp987= _temp978.pattern; goto _LL986;
_LL986: _temp985= _temp978.pat_vars; goto _LL984; _LL984: _temp983= _temp978.where_clause;
goto _LL982; _LL982: _temp981= _temp978.body; goto _LL980; _LL980: _temp979=
_temp978.loc; goto _LL977; _LL977: if( _temp985 == 0){( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp989=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp989[ 0]=({ struct
Cyc_Core_Impossible_struct _temp990; _temp990.tag= Cyc_Core_Impossible; _temp990.f1=({
unsigned char* _temp991="switch clause vds not set"; struct _tagged_string
_temp992; _temp992.curr= _temp991; _temp992.base= _temp991; _temp992.last_plus_one=
_temp991 + 26; _temp992;}); _temp990;}); _temp989;}));}{ struct Cyc_List_List*
vds=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp985))->v;
for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
if( _temp983 != 0){ void* _temp995; void* _temp997; struct _tuple0 _temp993= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp983)); _LL998: _temp997= _temp993.f1;
goto _LL996; _LL996: _temp995= _temp993.f2; goto _LL994; _LL994: _temp974=({
struct Cyc_List_List* _temp999=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp999->hd=( void*) _temp997; _temp999->tl=({ struct
Cyc_List_List* _temp1000=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1000->hd=( void*) _temp995; _temp1000->tl= _temp974; _temp1000;}); _temp999;});}
Cyc_NewControlFlow_cf_prepass( env, _temp981);} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp974);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s){ env.lhsides ==
0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({
unsigned char* _temp1001="new_control_flow.cyc:753 env.lhsides == null"; struct
_tagged_string _temp1002; _temp1002.curr= _temp1001; _temp1002.base= _temp1001;
_temp1002.last_plus_one= _temp1001 + 45; _temp1002;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp1003="new_control_flow.cyc:754 env.access_path == null"; struct
_tagged_string _temp1004; _temp1004.curr= _temp1003; _temp1004.base= _temp1003;
_temp1004.last_plus_one= _temp1003 + 49; _temp1004;}));{ void* _temp1005=( void*)
s->r; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Stmt* _temp1059; struct
Cyc_Absyn_Stmt* _temp1061; struct Cyc_Absyn_Exp* _temp1063; struct Cyc_Absyn_Exp*
_temp1065; struct Cyc_Absyn_Stmt* _temp1067; struct Cyc_Absyn_Stmt* _temp1069;
struct Cyc_Absyn_Exp* _temp1071; struct _tuple3 _temp1073; struct Cyc_Absyn_Stmt*
_temp1075; struct Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_Stmt* _temp1079;
struct Cyc_Absyn_Stmt* _temp1081; struct _tuple3 _temp1083; struct Cyc_Absyn_Stmt*
_temp1085; struct Cyc_Absyn_Exp* _temp1087; struct Cyc_Absyn_Stmt* _temp1089;
struct Cyc_Absyn_Stmt* _temp1091; struct Cyc_Absyn_Stmt* _temp1093; struct
_tagged_string* _temp1095; struct Cyc_Absyn_Stmt* _temp1097; struct
_tagged_string* _temp1099; struct Cyc_Absyn_Stmt* _temp1101; struct _tuple3
_temp1103; struct Cyc_Absyn_Stmt* _temp1105; struct Cyc_Absyn_Exp* _temp1107;
struct _tuple3 _temp1109; struct Cyc_Absyn_Stmt* _temp1111; struct Cyc_Absyn_Exp*
_temp1113; struct Cyc_Absyn_Exp* _temp1115; struct Cyc_List_List* _temp1117;
struct Cyc_Absyn_Exp* _temp1119; struct Cyc_Absyn_Switch_clause** _temp1121;
struct Cyc_List_List* _temp1123; struct Cyc_Absyn_Stmt* _temp1125; struct Cyc_Absyn_Decl*
_temp1127; struct Cyc_Absyn_Decl _temp1129; struct Cyc_Position_Segment*
_temp1130; void* _temp1132; struct Cyc_Absyn_Vardecl* _temp1134; struct Cyc_Absyn_Stmt*
_temp1136; struct Cyc_Absyn_Decl* _temp1138; struct Cyc_Absyn_Decl _temp1140;
struct Cyc_Position_Segment* _temp1141; void* _temp1143; int _temp1145; struct
Cyc_Absyn_Exp* _temp1147; struct Cyc_Core_Opt* _temp1149; struct Cyc_Core_Opt*
_temp1151; struct Cyc_Core_Opt _temp1153; struct Cyc_List_List* _temp1154;
struct Cyc_Absyn_Pat* _temp1156; struct Cyc_Absyn_Stmt* _temp1158; struct Cyc_Absyn_Decl*
_temp1160; struct Cyc_Absyn_Decl _temp1162; struct Cyc_Position_Segment*
_temp1163; void* _temp1165; struct Cyc_List_List* _temp1167; struct Cyc_Absyn_Stmt*
_temp1169; struct _tagged_string* _temp1171; struct Cyc_List_List* _temp1173;
struct Cyc_Absyn_Stmt* _temp1175; struct Cyc_Absyn_Stmt* _temp1177; struct Cyc_Absyn_Vardecl*
_temp1179; struct Cyc_Absyn_Tvar* _temp1181; struct Cyc_Absyn_Stmt* _temp1183;
struct Cyc_Absyn_Decl* _temp1185; struct Cyc_List_List* _temp1187; struct Cyc_Absyn_Exp*
_temp1189; struct Cyc_Absyn_Stmt* _temp1191; struct Cyc_Absyn_Stmt* _temp1193;
_LL1007: if( _temp1005 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1008;} else{ goto
_LL1009;} _LL1009: if(( unsigned int) _temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Exp_s:
0){ _LL1058: _temp1057=(( struct Cyc_Absyn_Exp_s_struct*) _temp1005)->f1; goto
_LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Seq_s: 0){ _LL1062: _temp1061=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1005)->f1; goto _LL1060; _LL1060: _temp1059=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1005)->f2; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Return_s: 0){ _LL1064:
_temp1063=(( struct Cyc_Absyn_Return_s_struct*) _temp1005)->f1; if( _temp1063 ==
0){ goto _LL1014;} else{ goto _LL1015;}} else{ goto _LL1015;} _LL1015: if((
unsigned int) _temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Return_s: 0){
_LL1066: _temp1065=(( struct Cyc_Absyn_Return_s_struct*) _temp1005)->f1; goto
_LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_IfThenElse_s: 0){ _LL1072: _temp1071=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1005)->f1; goto _LL1070; _LL1070: _temp1069=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1005)->f2; goto _LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1005)->f3; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Do_s: 0){ _LL1080: _temp1079=((
struct Cyc_Absyn_Do_s_struct*) _temp1005)->f1; goto _LL1074; _LL1074: _temp1073=((
struct Cyc_Absyn_Do_s_struct*) _temp1005)->f2; _LL1078: _temp1077= _temp1073.f1;
goto _LL1076; _LL1076: _temp1075= _temp1073.f2; goto _LL1020;} else{ goto
_LL1021;} _LL1021: if(( unsigned int) _temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_While_s:
0){ _LL1084: _temp1083=(( struct Cyc_Absyn_While_s_struct*) _temp1005)->f1;
_LL1088: _temp1087= _temp1083.f1; goto _LL1086; _LL1086: _temp1085= _temp1083.f2;
goto _LL1082; _LL1082: _temp1081=(( struct Cyc_Absyn_While_s_struct*) _temp1005)->f2;
goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Break_s: 0){ _LL1090: _temp1089=(( struct Cyc_Absyn_Break_s_struct*)
_temp1005)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Continue_s: 0){ _LL1092:
_temp1091=(( struct Cyc_Absyn_Continue_s_struct*) _temp1005)->f1; goto _LL1026;}
else{ goto _LL1027;} _LL1027: if(( unsigned int) _temp1005 > 1u?*(( int*)
_temp1005) == Cyc_Absyn_Goto_s: 0){ _LL1096: _temp1095=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1005)->f1; goto _LL1094; _LL1094: _temp1093=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1005)->f2; if( _temp1093 == 0){ goto _LL1028;} else{ goto _LL1029;}} else{
goto _LL1029;} _LL1029: if(( unsigned int) _temp1005 > 1u?*(( int*) _temp1005)
== Cyc_Absyn_Goto_s: 0){ _LL1100: _temp1099=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1005)->f1; goto _LL1098; _LL1098: _temp1097=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1005)->f2; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_For_s: 0){ _LL1116: _temp1115=((
struct Cyc_Absyn_For_s_struct*) _temp1005)->f1; goto _LL1110; _LL1110: _temp1109=((
struct Cyc_Absyn_For_s_struct*) _temp1005)->f2; _LL1114: _temp1113= _temp1109.f1;
goto _LL1112; _LL1112: _temp1111= _temp1109.f2; goto _LL1104; _LL1104: _temp1103=((
struct Cyc_Absyn_For_s_struct*) _temp1005)->f3; _LL1108: _temp1107= _temp1103.f1;
goto _LL1106; _LL1106: _temp1105= _temp1103.f2; goto _LL1102; _LL1102: _temp1101=((
struct Cyc_Absyn_For_s_struct*) _temp1005)->f4; goto _LL1032;} else{ goto
_LL1033;} _LL1033: if(( unsigned int) _temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Switch_s:
0){ _LL1120: _temp1119=(( struct Cyc_Absyn_Switch_s_struct*) _temp1005)->f1;
goto _LL1118; _LL1118: _temp1117=(( struct Cyc_Absyn_Switch_s_struct*) _temp1005)->f2;
goto _LL1034;} else{ goto _LL1035;} _LL1035: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Fallthru_s: 0){ _LL1124: _temp1123=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1005)->f1; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1005)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Decl_s: 0){ _LL1128: _temp1127=((
struct Cyc_Absyn_Decl_s_struct*) _temp1005)->f1; _temp1129=* _temp1127; _LL1133:
_temp1132=( void*) _temp1129.r; if(*(( int*) _temp1132) == Cyc_Absyn_Var_d){
_LL1135: _temp1134=(( struct Cyc_Absyn_Var_d_struct*) _temp1132)->f1; goto
_LL1131;} else{ goto _LL1039;} _LL1131: _temp1130= _temp1129.loc; goto _LL1126;
_LL1126: _temp1125=(( struct Cyc_Absyn_Decl_s_struct*) _temp1005)->f2; goto
_LL1038;} else{ goto _LL1039;} _LL1039: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Decl_s: 0){ _LL1139: _temp1138=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1005)->f1; _temp1140=* _temp1138; _LL1144: _temp1143=( void*) _temp1140.r;
if(*(( int*) _temp1143) == Cyc_Absyn_Let_d){ _LL1157: _temp1156=(( struct Cyc_Absyn_Let_d_struct*)
_temp1143)->f1; goto _LL1152; _LL1152: _temp1151=(( struct Cyc_Absyn_Let_d_struct*)
_temp1143)->f2; if( _temp1151 == 0){ goto _LL1041;} else{ _temp1153=* _temp1151;
_LL1155: _temp1154=( struct Cyc_List_List*) _temp1153.v; goto _LL1150;} _LL1150:
_temp1149=(( struct Cyc_Absyn_Let_d_struct*) _temp1143)->f3; goto _LL1148;
_LL1148: _temp1147=(( struct Cyc_Absyn_Let_d_struct*) _temp1143)->f4; goto
_LL1146; _LL1146: _temp1145=(( struct Cyc_Absyn_Let_d_struct*) _temp1143)->f5;
goto _LL1142;} else{ goto _LL1041;} _LL1142: _temp1141= _temp1140.loc; goto
_LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_Decl_s_struct*) _temp1005)->f2;
goto _LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Decl_s: 0){ _LL1161: _temp1160=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1005)->f1; _temp1162=* _temp1160; _LL1166: _temp1165=( void*) _temp1162.r;
if(*(( int*) _temp1165) == Cyc_Absyn_Letv_d){ _LL1168: _temp1167=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1165)->f1; goto _LL1164;} else{ goto _LL1043;} _LL1164: _temp1163=
_temp1162.loc; goto _LL1159; _LL1159: _temp1158=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1005)->f2; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Label_s: 0){ _LL1172: _temp1171=((
struct Cyc_Absyn_Label_s_struct*) _temp1005)->f1; goto _LL1170; _LL1170:
_temp1169=(( struct Cyc_Absyn_Label_s_struct*) _temp1005)->f2; goto _LL1044;}
else{ goto _LL1045;} _LL1045: if(( unsigned int) _temp1005 > 1u?*(( int*)
_temp1005) == Cyc_Absyn_TryCatch_s: 0){ _LL1176: _temp1175=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1005)->f1; goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1005)->f2; goto _LL1046;} else{ goto _LL1047;} _LL1047: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Region_s: 0){ _LL1182:
_temp1181=(( struct Cyc_Absyn_Region_s_struct*) _temp1005)->f1; goto _LL1180;
_LL1180: _temp1179=(( struct Cyc_Absyn_Region_s_struct*) _temp1005)->f2; goto
_LL1178; _LL1178: _temp1177=(( struct Cyc_Absyn_Region_s_struct*) _temp1005)->f3;
goto _LL1048;} else{ goto _LL1049;} _LL1049: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Decl_s: 0){ _LL1186: _temp1185=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1005)->f1; goto _LL1184; _LL1184: _temp1183=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1005)->f2; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_SwitchC_s: 0){ _LL1190:
_temp1189=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1005)->f1; goto _LL1188;
_LL1188: _temp1187=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1005)->f2; goto
_LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1005 > 1u?*((
int*) _temp1005) == Cyc_Absyn_Cut_s: 0){ _LL1192: _temp1191=(( struct Cyc_Absyn_Cut_s_struct*)
_temp1005)->f1; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int)
_temp1005 > 1u?*(( int*) _temp1005) == Cyc_Absyn_Splice_s: 0){ _LL1194:
_temp1193=(( struct Cyc_Absyn_Splice_s_struct*) _temp1005)->f1; goto _LL1056;}
else{ goto _LL1006;} _LL1008: return; _LL1010: { void* _temp1195= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1057); goto _LL1196; _LL1196: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1197=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1197->hd=( void*) _temp1195; _temp1197->tl= 0;
_temp1197;})); return;} _LL1012: Cyc_NewControlFlow_cf_prepass( env, _temp1061);
Cyc_NewControlFlow_cf_prepass( env, _temp1059); return; _LL1014: return; _LL1016: {
void* _temp1198= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env),( struct Cyc_Absyn_Exp*) _check_null( _temp1065)); goto _LL1199; _LL1199:
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1200=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1200->hd=( void*)
_temp1198; _temp1200->tl= 0; _temp1200;})); return;} _LL1018: { void* _temp1203;
void* _temp1205; struct _tuple0 _temp1201= Cyc_NewControlFlow_abstract_guard(
env, _temp1071); _LL1206: _temp1205= _temp1201.f1; goto _LL1204; _LL1204:
_temp1203= _temp1201.f2; goto _LL1202; _LL1202: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1207=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1207->hd=( void*) _temp1205; _temp1207->tl=({
struct Cyc_List_List* _temp1208=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1208->hd=( void*) _temp1203; _temp1208->tl= 0;
_temp1208;}); _temp1207;})); Cyc_NewControlFlow_cf_prepass( env, _temp1069); Cyc_NewControlFlow_cf_prepass(
env, _temp1067); return;} _LL1020: _temp1087= _temp1077; _temp1085= _temp1075;
_temp1081= _temp1079; goto _LL1022; _LL1022: { void* _temp1211; void* _temp1213;
struct _tuple0 _temp1209= Cyc_NewControlFlow_abstract_guard( env, _temp1087);
_LL1214: _temp1213= _temp1209.f1; goto _LL1212; _LL1212: _temp1211= _temp1209.f2;
goto _LL1210; _LL1210: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1215=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1215->hd=( void*) _temp1213; _temp1215->tl=({ struct Cyc_List_List*
_temp1216=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1216->hd=( void*) _temp1211; _temp1216->tl= 0; _temp1216;}); _temp1215;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1081); return;} _LL1024: return;
_LL1026: return; _LL1028:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1217=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1217[ 0]=({ struct Cyc_Core_Impossible_struct _temp1218; _temp1218.tag= Cyc_Core_Impossible;
_temp1218.f1=({ unsigned char* _temp1219="cf_prepass: goto w/o destination";
struct _tagged_string _temp1220; _temp1220.curr= _temp1219; _temp1220.base=
_temp1219; _temp1220.last_plus_one= _temp1219 + 33; _temp1220;}); _temp1218;});
_temp1217;})); _LL1030: { struct Cyc_Absyn_Stmt* _temp1221=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; goto _LL1222; _LL1222: { struct Cyc_Absyn_Stmt* _temp1223=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1097)))->encloser; goto _LL1224;
_LL1224: while( _temp1223 != _temp1221) { struct Cyc_Absyn_Stmt* _temp1225=( Cyc_NewControlFlow_get_stmt_annot(
_temp1221))->encloser; goto _LL1226; _LL1226: if( _temp1225 == _temp1221){ Cyc_Tcutil_terr(
s->loc,({ unsigned char* _temp1227="goto enters local scope or exception handler";
struct _tagged_string _temp1228; _temp1228.curr= _temp1227; _temp1228.base=
_temp1227; _temp1228.last_plus_one= _temp1227 + 45; _temp1228;})); break;}
_temp1221= _temp1225;} return;}} _LL1032: { void* _temp1229= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1115); goto _LL1230; _LL1230: { void* _temp1233; void* _temp1235;
struct _tuple0 _temp1231= Cyc_NewControlFlow_abstract_guard( env, _temp1113);
_LL1236: _temp1235= _temp1231.f1; goto _LL1234; _LL1234: _temp1233= _temp1231.f2;
goto _LL1232; _LL1232: { void* _temp1237= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1107); goto _LL1238; _LL1238: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1239=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1239->hd=( void*) _temp1229; _temp1239->tl=({
struct Cyc_List_List* _temp1240=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1240->hd=( void*) _temp1235; _temp1240->tl=({
struct Cyc_List_List* _temp1241=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1241->hd=( void*) _temp1233; _temp1241->tl=({
struct Cyc_List_List* _temp1242=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1242->hd=( void*) _temp1237; _temp1242->tl= 0;
_temp1242;}); _temp1241;}); _temp1240;}); _temp1239;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1101); return;}}} _LL1034: { void* _temp1243= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1119); goto _LL1244; _LL1244: { struct
Cyc_List_List* _temp1245= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1117);
goto _LL1246; _LL1246: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1247->hd=( void*) _temp1243; _temp1247->tl= _temp1245; _temp1247;}));
return;}} _LL1036: { void* _temp1250; void* _temp1252; struct Cyc_NewControlFlow_AbsSynOne
_temp1248= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1123, 0)); _LL1253: _temp1252=( void*) _temp1248.inner_exp; goto
_LL1251; _LL1251: _temp1250=( void*) _temp1248.assigns; goto _LL1249; _LL1249: {
void* _temp1254= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1252, _temp1250); goto _LL1255; _LL1255: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1256=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1256->hd=( void*) _temp1254; _temp1256->tl= 0;
_temp1256;})); return;}} _LL1038: Cyc_NewControlFlow_add_var_root( env,
_temp1134); if( _temp1134->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp1258= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1257=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1257->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1134); _temp1257->tl= 0; _temp1257;})); goto
_LL1259; _LL1259: { void* _temp1260= Cyc_NewControlFlow_abstract_exp_top(
_temp1258,( struct Cyc_Absyn_Exp*) _check_null( _temp1134->initializer)); goto
_LL1261; _LL1261: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1262=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1262->hd=( void*) _temp1260; _temp1262->tl= 0; _temp1262;}));}} Cyc_NewControlFlow_cf_prepass(
env, _temp1125); return; _LL1040: for( 0; _temp1154 != 0; _temp1154=(( struct
Cyc_List_List*) _check_null( _temp1154))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp1154))->hd);}{
void* _temp1263= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp1147); goto _LL1264; _LL1264: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1265=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1265->hd=( void*) _temp1263; _temp1265->tl= 0;
_temp1265;})); Cyc_NewControlFlow_cf_prepass( env, _temp1136); return;} _LL1042:
for( 0; _temp1167 != 0; _temp1167=(( struct Cyc_List_List*) _check_null(
_temp1167))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1167))->hd);} Cyc_NewControlFlow_cf_prepass(
env, _temp1158); return; _LL1044: Cyc_NewControlFlow_cf_prepass( env, _temp1169);
return; _LL1046: Cyc_NewControlFlow_cf_prepass( env, _temp1175);{ struct Cyc_List_List*
_temp1266= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1173); goto _LL1267;
_LL1267: Cyc_NewControlFlow_cf_set_absexps( s, _temp1266); return;} _LL1048: Cyc_NewControlFlow_add_var_root(
env, _temp1179); Cyc_NewControlFlow_cf_prepass( env, _temp1177); return; _LL1050:
goto _LL1052; _LL1052: goto _LL1054; _LL1054: goto _LL1056; _LL1056:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1268=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1268[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1269; _temp1269.tag= Cyc_Core_Impossible;
_temp1269.f1=({ unsigned char* _temp1270="cf_prepass: bad stmt form"; struct
_tagged_string _temp1271; _temp1271.curr= _temp1270; _temp1271.base= _temp1270;
_temp1271.last_plus_one= _temp1270 + 26; _temp1271;}); _temp1269;}); _temp1268;}));
_LL1006:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0; static int Cyc_NewControlFlow_iteration_num=
0; struct Cyc_NewControlFlow_AnalEnv; void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow){ if( env->in_try){( void*)(
env->tryflow=( void*) Cyc_CfFlowInfo_join_flow( new_flow,( void*) env->tryflow));}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
void* in_flow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1272= Cyc_NewControlFlow_get_stmt_annot( s); goto _LL1273; _LL1273:( void*)(
_temp1272->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*) _temp1272->flow));
++ _temp1272->visited; _temp1272->visited == Cyc_NewControlFlow_iteration_num? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp1274="new_control_flow.cyc:892 annot->visited == iteration_num"; struct
_tagged_string _temp1275; _temp1275.curr= _temp1274; _temp1275.base= _temp1274;
_temp1275.last_plus_one= _temp1274 + 57; _temp1275;})); return _temp1272;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1276= Cyc_NewControlFlow_get_stmt_annot(
s); goto _LL1277; _LL1277: { void* _temp1278= Cyc_CfFlowInfo_join_flow( flow,(
void*) _temp1276->flow); goto _LL1279; _LL1279: if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp1278,( void*) _temp1276->flow)){( void*)( _temp1276->flow=( void*)
_temp1278); if( _temp1276->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1280= flow; struct Cyc_Dict_Dict*
_temp1286; _LL1282: if( _temp1280 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1283;} else{ goto _LL1284;} _LL1284: if(( unsigned int) _temp1280 > 1u?*((
int*) _temp1280) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1287: _temp1286=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1280)->f1; goto _LL1285;} else{ goto _LL1281;} _LL1283: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1285: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1290=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1288=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1288[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1289; _temp1289.tag=
Cyc_CfFlowInfo_VarRoot; _temp1289.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1289;}); _temp1288;}); goto _LL1291; _LL1291: {
void* _temp1292= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) _temp1290)); goto _LL1293; _LL1293: _temp1286=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1286,(
void*) _temp1290, _temp1292);}} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1294=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1294[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1295; _temp1295.tag=
Cyc_CfFlowInfo_InitsFL; _temp1295.f1= _temp1286; _temp1295;}); _temp1294;});
_LL1281:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1296= flow; struct Cyc_Dict_Dict*
_temp1302; _LL1298: if( _temp1296 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1299;} else{ goto _LL1300;} _LL1300: if(( unsigned int) _temp1296 > 1u?*((
int*) _temp1296) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1303: _temp1302=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1296)->f1; goto _LL1301;} else{ goto _LL1297;} _LL1299: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1301: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1306=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1304=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1304[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1305; _temp1305.tag=
Cyc_CfFlowInfo_VarRoot; _temp1305.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1305;}); _temp1304;}); goto _LL1307; _LL1307:
_temp1302=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( _temp1302,( void*) _temp1306,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) _temp1306));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1308=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1308[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1309; _temp1309.tag= Cyc_CfFlowInfo_InitsFL;
_temp1309.f1= _temp1302; _temp1309;}); _temp1308;}); _LL1297:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)(( struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp1310= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); goto _LL1311; _LL1311: return({ struct _tuple0
_temp1312; _temp1312.f1= _temp1310; _temp1312.f2= _temp1310; _temp1312;});}
return({ struct _tuple0 _temp1313; _temp1313.f1= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null( aes))->hd, in_flow);
_temp1313.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd, in_flow);
_temp1313;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp1316; struct Cyc_Position_Segment*
_temp1317; struct Cyc_Absyn_Stmt* _temp1319; struct Cyc_Absyn_Exp* _temp1321;
struct Cyc_Core_Opt* _temp1323; struct Cyc_Absyn_Pat* _temp1325; struct Cyc_Absyn_Switch_clause*
_temp1314=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd; _temp1316=* _temp1314; _LL1326: _temp1325= _temp1316.pattern;
goto _LL1324; _LL1324: _temp1323= _temp1316.pat_vars; goto _LL1322; _LL1322:
_temp1321= _temp1316.where_clause; goto _LL1320; _LL1320: _temp1319= _temp1316.body;
goto _LL1318; _LL1318: _temp1317= _temp1316.loc; goto _LL1315; _LL1315: { void*
_temp1327= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1323))->v); goto _LL1328; _LL1328:
_temp1327= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1327,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1323))->v);{ void* body_outflow; if(
_temp1321 != 0){ void* _temp1331; void* _temp1333; struct _tuple0 _temp1329= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1321))->loc, aes, _temp1327);
_LL1334: _temp1333= _temp1329.f1; goto _LL1332; _LL1332: _temp1331= _temp1329.f2;
goto _LL1330; _LL1330: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp1331; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1319, _temp1333);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1319, _temp1327);}{ void* _temp1335= body_outflow; _LL1337: if(
_temp1335 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1338;} else{ goto _LL1339;}
_LL1339: goto _LL1340; _LL1338: goto _LL1336; _LL1340: Cyc_Tcutil_terr(
_temp1319->loc,({ unsigned char* _temp1341="switch clause may implicitly fallthru";
struct _tagged_string _temp1342; _temp1342.curr= _temp1341; _temp1342.base=
_temp1341; _temp1342.last_plus_one= _temp1341 + 38; _temp1342;})); goto _LL1336;
_LL1336:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1343= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); goto _LL1344; _LL1344:
in_flow=( void*) _temp1343->flow;{ struct Cyc_List_List* _temp1345= _temp1343->absexps;
goto _LL1346; _LL1346: { void* _temp1347=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1393; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Exp* _temp1397;
struct Cyc_Absyn_Stmt* _temp1399; struct Cyc_Absyn_Stmt* _temp1401; struct Cyc_Absyn_Stmt*
_temp1403; struct Cyc_Absyn_Stmt* _temp1405; struct Cyc_Absyn_Exp* _temp1407;
struct Cyc_Absyn_Stmt* _temp1409; struct _tuple3 _temp1411; struct Cyc_Absyn_Stmt*
_temp1413; struct Cyc_Absyn_Exp* _temp1415; struct _tuple3 _temp1417; struct Cyc_Absyn_Stmt*
_temp1419; struct Cyc_Absyn_Exp* _temp1421; struct Cyc_Absyn_Stmt* _temp1423;
struct Cyc_Absyn_Stmt* _temp1425; struct _tuple3 _temp1427; struct Cyc_Absyn_Stmt*
_temp1429; struct Cyc_Absyn_Exp* _temp1431; struct _tuple3 _temp1433; struct Cyc_Absyn_Stmt*
_temp1435; struct Cyc_Absyn_Exp* _temp1437; struct Cyc_Absyn_Exp* _temp1439;
struct Cyc_Absyn_Stmt* _temp1441; struct Cyc_Absyn_Switch_clause** _temp1443;
struct Cyc_Absyn_Switch_clause* _temp1445; struct Cyc_List_List* _temp1446;
struct Cyc_Absyn_Stmt* _temp1448; struct Cyc_Absyn_Stmt* _temp1450; struct Cyc_Absyn_Stmt*
_temp1452; struct _tagged_string* _temp1454; struct Cyc_List_List* _temp1456;
struct Cyc_Absyn_Exp* _temp1458; struct Cyc_List_List* _temp1460; struct Cyc_Absyn_Stmt*
_temp1462; struct Cyc_Absyn_Stmt* _temp1464; struct Cyc_Absyn_Decl* _temp1466;
struct Cyc_Absyn_Decl _temp1468; struct Cyc_Position_Segment* _temp1469; void*
_temp1471; struct Cyc_Absyn_Vardecl* _temp1473; struct Cyc_Absyn_Stmt* _temp1475;
struct Cyc_Absyn_Decl* _temp1477; struct Cyc_Absyn_Decl _temp1479; struct Cyc_Position_Segment*
_temp1480; void* _temp1482; int _temp1484; struct Cyc_Absyn_Exp* _temp1486;
struct Cyc_Core_Opt* _temp1488; struct Cyc_Core_Opt* _temp1490; struct Cyc_Core_Opt
_temp1492; struct Cyc_List_List* _temp1493; struct Cyc_Absyn_Pat* _temp1495;
struct Cyc_Absyn_Stmt* _temp1497; struct Cyc_Absyn_Decl* _temp1499; struct Cyc_Absyn_Decl
_temp1501; struct Cyc_Position_Segment* _temp1502; void* _temp1504; struct Cyc_List_List*
_temp1506; struct Cyc_Absyn_Stmt* _temp1508; struct _tagged_string* _temp1510;
struct Cyc_Absyn_Stmt* _temp1512; struct Cyc_Absyn_Vardecl* _temp1514; struct
Cyc_Absyn_Tvar* _temp1516; _LL1349: if( _temp1347 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1350;} else{ goto _LL1351;} _LL1351: if(( unsigned int) _temp1347 > 1u?*((
int*) _temp1347) == Cyc_Absyn_Return_s: 0){ _LL1394: _temp1393=(( struct Cyc_Absyn_Return_s_struct*)
_temp1347)->f1; if( _temp1393 == 0){ goto _LL1352;} else{ goto _LL1353;}} else{
goto _LL1353;} _LL1353: if(( unsigned int) _temp1347 > 1u?*(( int*) _temp1347)
== Cyc_Absyn_Return_s: 0){ _LL1396: _temp1395=(( struct Cyc_Absyn_Return_s_struct*)
_temp1347)->f1; goto _LL1354;} else{ goto _LL1355;} _LL1355: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Exp_s: 0){ _LL1398: _temp1397=((
struct Cyc_Absyn_Exp_s_struct*) _temp1347)->f1; goto _LL1356;} else{ goto
_LL1357;} _LL1357: if(( unsigned int) _temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Seq_s:
0){ _LL1402: _temp1401=(( struct Cyc_Absyn_Seq_s_struct*) _temp1347)->f1; goto
_LL1400; _LL1400: _temp1399=(( struct Cyc_Absyn_Seq_s_struct*) _temp1347)->f2;
goto _LL1358;} else{ goto _LL1359;} _LL1359: if(( unsigned int) _temp1347 > 1u?*((
int*) _temp1347) == Cyc_Absyn_IfThenElse_s: 0){ _LL1408: _temp1407=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1347)->f1; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1347)->f2; goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1347)->f3; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_While_s: 0){ _LL1412: _temp1411=((
struct Cyc_Absyn_While_s_struct*) _temp1347)->f1; _LL1416: _temp1415= _temp1411.f1;
goto _LL1414; _LL1414: _temp1413= _temp1411.f2; goto _LL1410; _LL1410: _temp1409=((
struct Cyc_Absyn_While_s_struct*) _temp1347)->f2; goto _LL1362;} else{ goto
_LL1363;} _LL1363: if(( unsigned int) _temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Do_s:
0){ _LL1424: _temp1423=(( struct Cyc_Absyn_Do_s_struct*) _temp1347)->f1; goto
_LL1418; _LL1418: _temp1417=(( struct Cyc_Absyn_Do_s_struct*) _temp1347)->f2;
_LL1422: _temp1421= _temp1417.f1; goto _LL1420; _LL1420: _temp1419= _temp1417.f2;
goto _LL1364;} else{ goto _LL1365;} _LL1365: if(( unsigned int) _temp1347 > 1u?*((
int*) _temp1347) == Cyc_Absyn_For_s: 0){ _LL1440: _temp1439=(( struct Cyc_Absyn_For_s_struct*)
_temp1347)->f1; goto _LL1434; _LL1434: _temp1433=(( struct Cyc_Absyn_For_s_struct*)
_temp1347)->f2; _LL1438: _temp1437= _temp1433.f1; goto _LL1436; _LL1436:
_temp1435= _temp1433.f2; goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_For_s_struct*)
_temp1347)->f3; _LL1432: _temp1431= _temp1427.f1; goto _LL1430; _LL1430:
_temp1429= _temp1427.f2; goto _LL1426; _LL1426: _temp1425=(( struct Cyc_Absyn_For_s_struct*)
_temp1347)->f4; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Break_s: 0){ _LL1442: _temp1441=((
struct Cyc_Absyn_Break_s_struct*) _temp1347)->f1; if( _temp1441 == 0){ goto
_LL1368;} else{ goto _LL1369;}} else{ goto _LL1369;} _LL1369: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Fallthru_s: 0){ _LL1447:
_temp1446=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1347)->f1; goto _LL1444;
_LL1444: _temp1443=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1347)->f2; if(
_temp1443 == 0){ goto _LL1371;} else{ _temp1445=* _temp1443; goto _LL1370;}}
else{ goto _LL1371;} _LL1371: if(( unsigned int) _temp1347 > 1u?*(( int*)
_temp1347) == Cyc_Absyn_Break_s: 0){ _LL1449: _temp1448=(( struct Cyc_Absyn_Break_s_struct*)
_temp1347)->f1; goto _LL1372;} else{ goto _LL1373;} _LL1373: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Continue_s: 0){ _LL1451:
_temp1450=(( struct Cyc_Absyn_Continue_s_struct*) _temp1347)->f1; goto _LL1374;}
else{ goto _LL1375;} _LL1375: if(( unsigned int) _temp1347 > 1u?*(( int*)
_temp1347) == Cyc_Absyn_Goto_s: 0){ _LL1455: _temp1454=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1347)->f1; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1347)->f2; goto _LL1376;} else{ goto _LL1377;} _LL1377: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Switch_s: 0){ _LL1459:
_temp1458=(( struct Cyc_Absyn_Switch_s_struct*) _temp1347)->f1; goto _LL1457;
_LL1457: _temp1456=(( struct Cyc_Absyn_Switch_s_struct*) _temp1347)->f2; goto
_LL1378;} else{ goto _LL1379;} _LL1379: if(( unsigned int) _temp1347 > 1u?*((
int*) _temp1347) == Cyc_Absyn_TryCatch_s: 0){ _LL1463: _temp1462=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1347)->f1; goto _LL1461; _LL1461: _temp1460=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1347)->f2; goto _LL1380;} else{ goto _LL1381;} _LL1381: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Decl_s: 0){ _LL1467: _temp1466=((
struct Cyc_Absyn_Decl_s_struct*) _temp1347)->f1; _temp1468=* _temp1466; _LL1472:
_temp1471=( void*) _temp1468.r; if(*(( int*) _temp1471) == Cyc_Absyn_Var_d){
_LL1474: _temp1473=(( struct Cyc_Absyn_Var_d_struct*) _temp1471)->f1; goto
_LL1470;} else{ goto _LL1383;} _LL1470: _temp1469= _temp1468.loc; goto _LL1465;
_LL1465: _temp1464=(( struct Cyc_Absyn_Decl_s_struct*) _temp1347)->f2; goto
_LL1382;} else{ goto _LL1383;} _LL1383: if(( unsigned int) _temp1347 > 1u?*((
int*) _temp1347) == Cyc_Absyn_Decl_s: 0){ _LL1478: _temp1477=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1347)->f1; _temp1479=* _temp1477; _LL1483: _temp1482=( void*) _temp1479.r;
if(*(( int*) _temp1482) == Cyc_Absyn_Let_d){ _LL1496: _temp1495=(( struct Cyc_Absyn_Let_d_struct*)
_temp1482)->f1; goto _LL1491; _LL1491: _temp1490=(( struct Cyc_Absyn_Let_d_struct*)
_temp1482)->f2; if( _temp1490 == 0){ goto _LL1385;} else{ _temp1492=* _temp1490;
_LL1494: _temp1493=( struct Cyc_List_List*) _temp1492.v; goto _LL1489;} _LL1489:
_temp1488=(( struct Cyc_Absyn_Let_d_struct*) _temp1482)->f3; goto _LL1487;
_LL1487: _temp1486=(( struct Cyc_Absyn_Let_d_struct*) _temp1482)->f4; goto
_LL1485; _LL1485: _temp1484=(( struct Cyc_Absyn_Let_d_struct*) _temp1482)->f5;
goto _LL1481;} else{ goto _LL1385;} _LL1481: _temp1480= _temp1479.loc; goto
_LL1476; _LL1476: _temp1475=(( struct Cyc_Absyn_Decl_s_struct*) _temp1347)->f2;
goto _LL1384;} else{ goto _LL1385;} _LL1385: if(( unsigned int) _temp1347 > 1u?*((
int*) _temp1347) == Cyc_Absyn_Decl_s: 0){ _LL1500: _temp1499=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1347)->f1; _temp1501=* _temp1499; _LL1505: _temp1504=( void*) _temp1501.r;
if(*(( int*) _temp1504) == Cyc_Absyn_Letv_d){ _LL1507: _temp1506=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1504)->f1; goto _LL1503;} else{ goto _LL1387;} _LL1503: _temp1502=
_temp1501.loc; goto _LL1498; _LL1498: _temp1497=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1347)->f2; goto _LL1386;} else{ goto _LL1387;} _LL1387: if(( unsigned int)
_temp1347 > 1u?*(( int*) _temp1347) == Cyc_Absyn_Label_s: 0){ _LL1511: _temp1510=((
struct Cyc_Absyn_Label_s_struct*) _temp1347)->f1; goto _LL1509; _LL1509:
_temp1508=(( struct Cyc_Absyn_Label_s_struct*) _temp1347)->f2; goto _LL1388;}
else{ goto _LL1389;} _LL1389: if(( unsigned int) _temp1347 > 1u?*(( int*)
_temp1347) == Cyc_Absyn_Region_s: 0){ _LL1517: _temp1516=(( struct Cyc_Absyn_Region_s_struct*)
_temp1347)->f1; goto _LL1515; _LL1515: _temp1514=(( struct Cyc_Absyn_Region_s_struct*)
_temp1347)->f2; goto _LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_Region_s_struct*)
_temp1347)->f3; goto _LL1390;} else{ goto _LL1391;} _LL1391: goto _LL1392;
_LL1350: return in_flow; _LL1352: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1354: Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*) _check_null(
_temp1395))->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1345))->hd,
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1356: return Cyc_CfAbsexp_eval_absexp(
env, _temp1397->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1345))->hd,
in_flow); _LL1358: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1399,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1401, in_flow)); _LL1360: { void*
_temp1520; void* _temp1522; struct _tuple0 _temp1518= Cyc_NewControlFlow_cf_evalguard(
env, _temp1407->loc, _temp1345, in_flow); _LL1523: _temp1522= _temp1518.f1; goto
_LL1521; _LL1521: _temp1520= _temp1518.f2; goto _LL1519; _LL1519: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1405, _temp1522), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1403, _temp1520));} _LL1362: { void* _temp1524=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1413))->flow; goto _LL1525; _LL1525: { void* _temp1528; void*
_temp1530; struct _tuple0 _temp1526= Cyc_NewControlFlow_cf_evalguard( env,
_temp1415->loc, _temp1345, _temp1524); _LL1531: _temp1530= _temp1526.f1; goto
_LL1529; _LL1529: _temp1528= _temp1526.f2; goto _LL1527; _LL1527: { void*
_temp1532= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1409, _temp1530); goto
_LL1533; _LL1533: Cyc_NewControlFlow_update_flow( _temp1413, _temp1532); return
_temp1528;}}} _LL1364: { void* _temp1534= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1423, in_flow); goto _LL1535; _LL1535: { void* _temp1536=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1534, _temp1419))->flow; goto _LL1537; _LL1537: { void* _temp1540; void*
_temp1542; struct _tuple0 _temp1538= Cyc_NewControlFlow_cf_evalguard( env,
_temp1421->loc, _temp1345, _temp1536); _LL1543: _temp1542= _temp1538.f1; goto
_LL1541; _LL1541: _temp1540= _temp1538.f2; goto _LL1539; _LL1539: Cyc_NewControlFlow_update_flow(
_temp1423, _temp1542); return _temp1540;}}} _LL1366: { void* _temp1544= Cyc_CfAbsexp_eval_absexp(
env, _temp1439->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1345))->hd,
in_flow); goto _LL1545; _LL1545: { void* _temp1546=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1544, _temp1435))->flow; goto _LL1547; _LL1547: { void* _temp1550; void*
_temp1552; struct _tuple0 _temp1548= Cyc_NewControlFlow_cf_evalguard( env,
_temp1437->loc,(( struct Cyc_List_List*) _check_null( _temp1345))->tl, _temp1546);
_LL1553: _temp1552= _temp1548.f1; goto _LL1551; _LL1551: _temp1550= _temp1548.f2;
goto _LL1549; _LL1549: { void* _temp1554= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1425, _temp1552); goto _LL1555; _LL1555: { void* _temp1556=( void*)(
Cyc_NewControlFlow_pre_stmt_check( _temp1554, _temp1429))->flow; goto _LL1557;
_LL1557: { void* _temp1558= Cyc_CfAbsexp_eval_absexp( env, _temp1431->loc,( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1345))->tl))->tl))->tl))->hd, _temp1556); goto _LL1559; _LL1559: Cyc_NewControlFlow_update_flow(
_temp1435, _temp1558); return _temp1550;}}}}}} _LL1368: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1370: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp1345))->hd, in_flow);{ struct Cyc_List_List* _temp1560=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1445->pat_vars))->v;
goto _LL1561; _LL1561: in_flow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow, _temp1560); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env,
in_flow, _temp1560); _temp1448=( struct Cyc_Absyn_Stmt*) _temp1445->body; goto
_LL1372;} _LL1372: _temp1450= _temp1448; goto _LL1374; _LL1374: _temp1452=
_temp1450; goto _LL1376; _LL1376: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)
_check_null( _temp1452), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1378: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1458->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp1345))->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1456,(( struct Cyc_List_List*) _check_null( _temp1345))->tl, in_flow);
return( void*) Cyc_CfFlowInfo_BottomFL; _LL1380: { struct Cyc_NewControlFlow_AnalEnv
_temp1564; void* _temp1565; int _temp1567; struct Cyc_Dict_Dict* _temp1569;
struct Cyc_NewControlFlow_AnalEnv* _temp1562= env; _temp1564=* _temp1562;
_LL1570: _temp1569= _temp1564.roots; goto _LL1568; _LL1568: _temp1567= _temp1564.in_try;
goto _LL1566; _LL1566: _temp1565=( void*) _temp1564.tryflow; goto _LL1563;
_LL1563: env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void*
_temp1571= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1462, in_flow); goto
_LL1572; _LL1572: { void* _temp1573=( void*) env->tryflow; goto _LL1574; _LL1574:
env->in_try= _temp1567;( void*)( env->tryflow=( void*) _temp1565); Cyc_NewControlFlow_update_tryflow(
env, _temp1573); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1460,
_temp1345, _temp1573); return _temp1571;}}} _LL1382: { struct Cyc_List_List
_temp1576=({ struct Cyc_List_List _temp1575; _temp1575.hd=( void*) _temp1473;
_temp1575.tl= 0; _temp1575;}); goto _LL1577; _LL1577: { void* _temp1578= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)& _temp1576); goto _LL1579; _LL1579: {
struct Cyc_Absyn_Exp* _temp1580= _temp1473->initializer; goto _LL1581; _LL1581:
if( _temp1580 != 0){ _temp1578= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1580))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp1345))->hd, _temp1578);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1464, _temp1578);}}} _LL1384: { void* _temp1582= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1493); goto _LL1583; _LL1583: _temp1582= Cyc_CfAbsexp_eval_absexp(
env, _temp1486->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1345))->hd,
_temp1582); _temp1582= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1582,
_temp1493); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1475, _temp1582);}
_LL1386: { void* _temp1584= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1506); goto _LL1585; _LL1585: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1497, _temp1584);} _LL1388: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1508, in_flow); _LL1390: { struct Cyc_List_List* _temp1587=({ struct
Cyc_List_List* _temp1586=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1586->hd=( void*) _temp1514; _temp1586->tl= 0; _temp1586;}); goto _LL1588;
_LL1588: { void* _temp1589= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1587); goto _LL1590; _LL1590: _temp1589= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1589, _temp1587); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1512, _temp1589);}} _LL1392:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1591=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1591[ 0]=({ struct Cyc_Core_Impossible_struct _temp1592; _temp1592.tag= Cyc_Core_Impossible;
_temp1592.f1=({ unsigned char* _temp1593="cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1594; _temp1594.curr= _temp1593; _temp1594.base=
_temp1593; _temp1594.last_plus_one= _temp1593 + 62; _temp1594;}); _temp1592;});
_temp1591;})); _LL1348:;}}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1595=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1619; struct Cyc_Absyn_Stmt* _temp1621;
struct Cyc_Absyn_Stmt* _temp1623; struct Cyc_Absyn_Stmt* _temp1625; struct Cyc_Absyn_Exp*
_temp1627; struct Cyc_Absyn_Stmt* _temp1629; struct _tuple3 _temp1631; struct
Cyc_Absyn_Stmt* _temp1633; struct Cyc_Absyn_Exp* _temp1635; struct _tuple3
_temp1637; struct Cyc_Absyn_Stmt* _temp1639; struct Cyc_Absyn_Exp* _temp1641;
struct Cyc_Absyn_Exp* _temp1643; struct Cyc_Absyn_Stmt* _temp1645; struct
_tuple3 _temp1647; struct Cyc_Absyn_Stmt* _temp1649; struct Cyc_Absyn_Exp*
_temp1651; struct _tuple3 _temp1653; struct Cyc_Absyn_Stmt* _temp1655; struct
Cyc_Absyn_Exp* _temp1657; struct Cyc_Absyn_Stmt* _temp1659; struct Cyc_Absyn_Stmt*
_temp1661; struct Cyc_Absyn_Vardecl* _temp1663; struct Cyc_Absyn_Tvar* _temp1665;
struct Cyc_Absyn_Stmt* _temp1667; struct _tagged_string* _temp1669; struct Cyc_Absyn_Stmt*
_temp1671; struct Cyc_Absyn_Decl* _temp1673; struct Cyc_List_List* _temp1675;
struct Cyc_Absyn_Stmt* _temp1677; struct Cyc_List_List* _temp1679; struct Cyc_Absyn_Exp*
_temp1681; _LL1597: if(( unsigned int) _temp1595 > 1u?*(( int*) _temp1595) ==
Cyc_Absyn_Seq_s: 0){ _LL1622: _temp1621=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1595)->f1; goto _LL1620; _LL1620: _temp1619=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1595)->f2; goto _LL1598;} else{ goto _LL1599;} _LL1599: if(( unsigned int)
_temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_IfThenElse_s: 0){ _LL1628:
_temp1627=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1595)->f1; goto _LL1626;
_LL1626: _temp1625=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1595)->f2;
goto _LL1624; _LL1624: _temp1623=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1595)->f3; goto _LL1600;} else{ goto _LL1601;} _LL1601: if(( unsigned int)
_temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_For_s: 0){ _LL1644: _temp1643=((
struct Cyc_Absyn_For_s_struct*) _temp1595)->f1; goto _LL1638; _LL1638: _temp1637=((
struct Cyc_Absyn_For_s_struct*) _temp1595)->f2; _LL1642: _temp1641= _temp1637.f1;
goto _LL1640; _LL1640: _temp1639= _temp1637.f2; goto _LL1632; _LL1632: _temp1631=((
struct Cyc_Absyn_For_s_struct*) _temp1595)->f3; _LL1636: _temp1635= _temp1631.f1;
goto _LL1634; _LL1634: _temp1633= _temp1631.f2; goto _LL1630; _LL1630: _temp1629=((
struct Cyc_Absyn_For_s_struct*) _temp1595)->f4; goto _LL1602;} else{ goto
_LL1603;} _LL1603: if(( unsigned int) _temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_While_s:
0){ _LL1648: _temp1647=(( struct Cyc_Absyn_While_s_struct*) _temp1595)->f1;
_LL1652: _temp1651= _temp1647.f1; goto _LL1650; _LL1650: _temp1649= _temp1647.f2;
goto _LL1646; _LL1646: _temp1645=(( struct Cyc_Absyn_While_s_struct*) _temp1595)->f2;
goto _LL1604;} else{ goto _LL1605;} _LL1605: if(( unsigned int) _temp1595 > 1u?*((
int*) _temp1595) == Cyc_Absyn_Do_s: 0){ _LL1660: _temp1659=(( struct Cyc_Absyn_Do_s_struct*)
_temp1595)->f1; goto _LL1654; _LL1654: _temp1653=(( struct Cyc_Absyn_Do_s_struct*)
_temp1595)->f2; _LL1658: _temp1657= _temp1653.f1; goto _LL1656; _LL1656:
_temp1655= _temp1653.f2; goto _LL1606;} else{ goto _LL1607;} _LL1607: if((
unsigned int) _temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_Region_s: 0){
_LL1666: _temp1665=(( struct Cyc_Absyn_Region_s_struct*) _temp1595)->f1; goto
_LL1664; _LL1664: _temp1663=(( struct Cyc_Absyn_Region_s_struct*) _temp1595)->f2;
goto _LL1662; _LL1662: _temp1661=(( struct Cyc_Absyn_Region_s_struct*) _temp1595)->f3;
goto _LL1608;} else{ goto _LL1609;} _LL1609: if(( unsigned int) _temp1595 > 1u?*((
int*) _temp1595) == Cyc_Absyn_Label_s: 0){ _LL1670: _temp1669=(( struct Cyc_Absyn_Label_s_struct*)
_temp1595)->f1; goto _LL1668; _LL1668: _temp1667=(( struct Cyc_Absyn_Label_s_struct*)
_temp1595)->f2; goto _LL1610;} else{ goto _LL1611;} _LL1611: if(( unsigned int)
_temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_Decl_s: 0){ _LL1674: _temp1673=((
struct Cyc_Absyn_Decl_s_struct*) _temp1595)->f1; goto _LL1672; _LL1672:
_temp1671=(( struct Cyc_Absyn_Decl_s_struct*) _temp1595)->f2; goto _LL1612;}
else{ goto _LL1613;} _LL1613: if(( unsigned int) _temp1595 > 1u?*(( int*)
_temp1595) == Cyc_Absyn_TryCatch_s: 0){ _LL1678: _temp1677=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1595)->f1; goto _LL1676; _LL1676: _temp1675=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1595)->f2; goto _LL1614;} else{ goto _LL1615;} _LL1615: if(( unsigned int)
_temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_Switch_s: 0){ _LL1682:
_temp1681=(( struct Cyc_Absyn_Switch_s_struct*) _temp1595)->f1; goto _LL1680;
_LL1680: _temp1679=(( struct Cyc_Absyn_Switch_s_struct*) _temp1595)->f2; goto
_LL1616;} else{ goto _LL1617;} _LL1617: goto _LL1618; _LL1598: _temp1625=
_temp1621; _temp1623= _temp1619; goto _LL1600; _LL1600: Cyc_NewControlFlow_cf_postpass(
_temp1625); Cyc_NewControlFlow_cf_postpass( _temp1623); return; _LL1602: Cyc_NewControlFlow_cf_postpass(
_temp1633); _temp1649= _temp1629; _temp1645= _temp1639; goto _LL1604; _LL1604:
_temp1659= _temp1645; _temp1655= _temp1649; goto _LL1606; _LL1606: Cyc_NewControlFlow_cf_postpass(
_temp1655); _temp1661= _temp1659; goto _LL1608; _LL1608: _temp1667= _temp1661;
goto _LL1610; _LL1610: _temp1671= _temp1667; goto _LL1612; _LL1612: Cyc_NewControlFlow_cf_postpass(
_temp1671); return; _LL1614: Cyc_NewControlFlow_cf_postpass( _temp1677);
_temp1679= _temp1675; goto _LL1616; _LL1616: for( 0; _temp1679 != 0; _temp1679=((
struct Cyc_List_List*) _check_null( _temp1679))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1679))->hd)->body);}
return; _LL1618: return; _LL1596:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1711; _temp1711.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1712=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1712->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1712->comprehension_vars= 0; _temp1712;}); _temp1711.lhsides= 0; _temp1711.access_path=
0; _temp1711;}); struct Cyc_List_List* _temp1683=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v; goto _LL1684;
_LL1684:{ struct Cyc_List_List* vds= _temp1683; for( 0; vds != 0; vds=(( struct
Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_List_List*
_temp1687; struct Cyc_List_List* _temp1689; struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1691; struct Cyc_NewControlFlow_SharedAbsEnv _temp1693; struct Cyc_List_List*
_temp1694; struct Cyc_Dict_Dict* _temp1696; struct Cyc_NewControlFlow_AbsEnv
_temp1685= absenv; _LL1692: _temp1691= _temp1685.shared; _temp1693=* _temp1691;
_LL1697: _temp1696= _temp1693.all_roots; goto _LL1695; _LL1695: _temp1694=
_temp1693.comprehension_vars; goto _LL1690; _LL1690: _temp1689= _temp1685.lhsides;
goto _LL1688; _LL1688: _temp1687= _temp1685.access_path; goto _LL1686; _LL1686: {
struct Cyc_NewControlFlow_AnalEnv* _temp1699=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1698=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1698->roots= _temp1696; _temp1698->in_try= 0; _temp1698->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1698;}); goto _LL1700; _LL1700: { void*
in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1709=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1709[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1710; _temp1710.tag= Cyc_CfFlowInfo_InitsFL;
_temp1710.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1710;}); _temp1709;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
_temp1699, in_flow, _temp1683); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
_temp1699, in_flow, _temp1694);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( _temp1699, fd->body, in_flow);}{
void* _temp1701= out_flow; _LL1703: if( _temp1701 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1704;} else{ goto _LL1705;} _LL1705: goto _LL1706; _LL1704: goto _LL1702;
_LL1706: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,({ unsigned char* _temp1707="function may complete without returning a value";
struct _tagged_string _temp1708; _temp1708.curr= _temp1707; _temp1708.base=
_temp1707; _temp1708.last_plus_one= _temp1707 + 48; _temp1708;}));} goto _LL1702;
_LL1702:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp1713=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp1723; struct Cyc_List_List* _temp1725; struct _tuple1* _temp1727; struct
Cyc_List_List* _temp1729; struct _tagged_string* _temp1731; _LL1715: if(*(( int*)
_temp1713) == Cyc_Absyn_Fn_d){ _LL1724: _temp1723=(( struct Cyc_Absyn_Fn_d_struct*)
_temp1713)->f1; goto _LL1716;} else{ goto _LL1717;} _LL1717: if(*(( int*)
_temp1713) == Cyc_Absyn_Using_d){ _LL1728: _temp1727=(( struct Cyc_Absyn_Using_d_struct*)
_temp1713)->f1; goto _LL1726; _LL1726: _temp1725=(( struct Cyc_Absyn_Using_d_struct*)
_temp1713)->f2; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(*(( int*)
_temp1713) == Cyc_Absyn_Namespace_d){ _LL1732: _temp1731=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1713)->f1; goto _LL1730; _LL1730: _temp1729=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1713)->f2; goto _LL1720;} else{ goto _LL1721;} _LL1721: goto _LL1722;
_LL1716: Cyc_NewControlFlow_cf_check_fun( _temp1723); goto _LL1714; _LL1718:
_temp1729= _temp1725; goto _LL1720; _LL1720: Cyc_NewControlFlow_cf_check(
_temp1729); goto _LL1714; _LL1722: goto _LL1714; _LL1714:;}}