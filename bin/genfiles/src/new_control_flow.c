 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd(
struct _tuple0*); extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern void* Cyc_Assert_AssertFail(
struct _tagged_arr msg); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
15u]; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr);
extern void* Cyc_Tcutil_compress( void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
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
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern void
Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); extern unsigned char Cyc_CfAbsexp_BadAbsexp[
14u]; static const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG=
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
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt* encloser; int visited; void* flow; struct Cyc_List_List*
absexps; } ; unsigned char Cyc_NewControlFlow_CFAnnot[ 12u]="\000\000\000\000CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{ unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1; } ; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){( void*)( enclosee->annot=( void*)(( void*)({
struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*)
GC_malloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({
struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;})));} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt* s){ void* _temp3=(
void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot _temp9; struct Cyc_NewControlFlow_CFStmtAnnot*
_temp11; _LL5: if(*(( void**) _temp3) == Cyc_NewControlFlow_CFAnnot){ _LL10:
_temp9=(( struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=&((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; goto _LL6;} else{ goto
_LL7;} _LL7: goto _LL8; _LL6: return _temp11; _LL8:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp12=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp12[ 0]=({ struct
Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible; _temp13.f1=
_tag_arr("ControlFlow -- wrong stmt annotation", sizeof( unsigned char), 37u);
_temp13;}); _temp12;})); _LL4:;} void Cyc_NewControlFlow_cf_set_absexps( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot(
s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict*
all_roots; struct Cyc_List_List* comprehension_vars; } ; struct Cyc_NewControlFlow_AbsEnv{
struct Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides;
struct Cyc_List_List* access_path; } ; struct Cyc_NewControlFlow_AbsSynOne{ void*
inner_exp; void* assigns; } ; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp14[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp15; _temp15.tag= Cyc_CfFlowInfo_VarRoot;
_temp15.f1= vd; _temp15;}); _temp14;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp16=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp16[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_MallocPt;
_temp17.f1= e; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp18; _temp18.shared= src.shared; _temp18.lhsides= new_lhs; _temp18.access_path=
src.access_path; _temp18;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
== 0){ Cyc_NewControlFlow_unknown_list=({ struct Cyc_List_List* _temp19=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp19->hd=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp19->tl= 0; _temp19;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv _temp20= Cyc_NewControlFlow_change_lhs(
src, Cyc_NewControlFlow_use_it_list()); _temp20.access_path= 0; return _temp20;}
struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn
syn){ struct Cyc_List_List* _temp23; struct Cyc_NewControlFlow_AbsSynOne _temp25;
void* _temp27; void* _temp29; struct Cyc_NewControlFlow_AbsSynOne _temp31; void*
_temp33; void* _temp35; struct Cyc_NewControlFlow_AbsSyn _temp21= syn; _LL32:
_temp31= _temp21.when_true; _LL36: _temp35=( void*) _temp31.inner_exp; goto
_LL34; _LL34: _temp33=( void*) _temp31.assigns; goto _LL26; _LL26: _temp25=
_temp21.when_false; _LL30: _temp29=( void*) _temp25.inner_exp; goto _LL28; _LL28:
_temp27=( void*) _temp25.assigns; goto _LL24; _LL24: _temp23= _temp21.lvalues;
goto _LL22; _LL22: { void* _temp37= _temp35 == _temp29? _temp35: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp35, _temp29); void* _temp38= _temp33 == _temp27?
_temp33: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp33, _temp27);
return({ struct Cyc_NewControlFlow_AbsSynOne _temp39; _temp39.inner_exp=( void*)
_temp37; _temp39.assigns=( void*) _temp38; _temp39;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne _temp40=({ struct Cyc_NewControlFlow_AbsSynOne
_temp42; _temp42.inner_exp=( void*) ae; _temp42.assigns=( void*) assigns;
_temp42;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp41; _temp41.when_true=
_temp40; _temp41.when_false= _temp40; _temp41.lvalues= lvals; _temp41;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if((( struct Cyc_List_List*)
_check_null( env.lhsides))->tl == 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)((
struct Cyc_List_List*) _check_null( env.lhsides))->hd, ae);}{ struct Cyc_List_List*
_temp43= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0;
lhsides=(( struct Cyc_List_List*) _check_null( lhsides))->tl){ _temp43=({ struct
Cyc_List_List* _temp44=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp44->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( lhsides))->hd, ae); _temp44->tl= _temp43; _temp44;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, _temp43);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp45= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
void* _temp48; void* _temp50; struct Cyc_NewControlFlow_AbsSynOne _temp46= Cyc_NewControlFlow_meet_absexp(
_temp45); _LL51: _temp50=( void*) _temp46.inner_exp; goto _LL49; _LL49: _temp48=(
void*) _temp46.assigns; goto _LL47; _LL47: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, _temp50, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp48, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp52=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp52->hd=( void*) path_e; _temp52->tl=
env.access_path; _temp52;});{ struct Cyc_NewControlFlow_AbsSyn _temp53= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( _temp53.lvalues == 0){ _temp53.lvalues= Cyc_NewControlFlow_use_it_list();}
return _temp53;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path=(( struct Cyc_List_List*) _check_null( path))->tl){ void* _temp54=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( path))->hd)->r;
struct Cyc_Absyn_Exp* _temp68; struct Cyc_Absyn_Exp* _temp70; struct _tagged_arr*
_temp72; struct Cyc_Absyn_Exp* _temp74; struct _tagged_arr* _temp76; struct Cyc_Absyn_Exp*
_temp78; struct Cyc_Absyn_Exp* _temp80; struct Cyc_Absyn_Exp* _temp82; _LL56:
if(*(( int*) _temp54) == Cyc_Absyn_Address_e){ _LL69: _temp68=(( struct Cyc_Absyn_Address_e_struct*)
_temp54)->f1; goto _LL57;} else{ goto _LL58;} _LL58: if(*(( int*) _temp54) ==
Cyc_Absyn_Deref_e){ _LL71: _temp70=(( struct Cyc_Absyn_Deref_e_struct*) _temp54)->f1;
goto _LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp54) == Cyc_Absyn_StructArrow_e){
_LL75: _temp74=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp54)->f1; goto
_LL73; _LL73: _temp72=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp54)->f2;
goto _LL61;} else{ goto _LL62;} _LL62: if(*(( int*) _temp54) == Cyc_Absyn_StructMember_e){
_LL79: _temp78=(( struct Cyc_Absyn_StructMember_e_struct*) _temp54)->f1; goto
_LL77; _LL77: _temp76=(( struct Cyc_Absyn_StructMember_e_struct*) _temp54)->f2;
goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp54) == Cyc_Absyn_Subscript_e){
_LL83: _temp82=(( struct Cyc_Absyn_Subscript_e_struct*) _temp54)->f1; goto _LL81;
_LL81: _temp80=(( struct Cyc_Absyn_Subscript_e_struct*) _temp54)->f2; goto _LL65;}
else{ goto _LL66;} _LL66: goto _LL67; _LL57:(( struct Cyc_List_List*)
_check_null( path))->tl == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:219 path->tl == null", sizeof( unsigned char), 42u));
return Cyc_CfAbsexp_mkAddressOp( absop); _LL59: absop= Cyc_CfAbsexp_mkDerefOp(
absop); goto _LL55; _LL61: absop= Cyc_CfAbsexp_mkDerefOp( absop); _temp76=
_temp72; goto _LL63; _LL63: absop= Cyc_CfAbsexp_mkMemberOp( absop,( void*)({
struct Cyc_CfFlowInfo_StructF_struct* _temp84=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp84[ 0]=({ struct
Cyc_CfFlowInfo_StructF_struct _temp85; _temp85.tag= Cyc_CfFlowInfo_StructF;
_temp85.f1= _temp76; _temp85;}); _temp84;})); goto _LL55; _LL65: { unsigned int
_temp86= Cyc_Evexp_eval_const_uint_exp( _temp80); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp87=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp87[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp88; _temp88.tag= Cyc_CfFlowInfo_TupleF;
_temp88.f1=( int) _temp86; _temp88;}); _temp87;})); goto _LL55;} _LL67:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp89=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp89[ 0]=({ struct
Cyc_Core_Impossible_struct _temp90; _temp90.tag= Cyc_Core_Impossible; _temp90.f1=
_tag_arr("abstract_exp: unexpected acces path element", sizeof( unsigned char),
44u); _temp90;}); _temp89;})); _LL55:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_malloc( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* rgn_exp, struct Cyc_Absyn_Exp*
init_exp){ void* _temp91= Cyc_CfAbsexp_mkMallocOp( outer_e); void* _temp92= Cyc_CfAbsexp_mkSkipAE();
void* _temp93= Cyc_CfAbsexp_mkSkipAE(); if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp94= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp101=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp101->hd=(
void*) _temp91; _temp101->tl= 0; _temp101;})); _temp94.access_path= 0;{ void*
_temp97; void* _temp99; struct Cyc_NewControlFlow_AbsSynOne _temp95= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp94,( struct Cyc_Absyn_Exp*) _check_null(
init_exp))); _LL100: _temp99=( void*) _temp95.inner_exp; goto _LL98; _LL98:
_temp97=( void*) _temp95.assigns; goto _LL96; _LL96: _temp92= _temp99; _temp93=
_temp97;}} if( rgn_exp != 0){ struct Cyc_NewControlFlow_AbsSyn _temp102= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
rgn_exp)); void* _temp105; void* _temp107; struct Cyc_NewControlFlow_AbsSynOne
_temp103= Cyc_NewControlFlow_meet_absexp( _temp102); _LL108: _temp107=( void*)
_temp103.inner_exp; goto _LL106; _LL106: _temp105=( void*) _temp103.assigns;
goto _LL104; _LL104: _temp92= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp92, _temp107); _temp93= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp93, _temp105);}{ void* _temp109= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp92, _temp93)); struct Cyc_NewControlFlow_AbsSyn _temp110= Cyc_NewControlFlow_mkSyn_tf(
_temp109, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp(
_temp91))); if( env.access_path != 0){ void* _temp111=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( env.access_path))->hd)->r; struct Cyc_Absyn_Exp*
_temp119; struct _tagged_arr* _temp121; struct Cyc_Absyn_Exp* _temp123; _LL113:
if(*(( int*) _temp111) == Cyc_Absyn_Deref_e){ _LL120: _temp119=(( struct Cyc_Absyn_Deref_e_struct*)
_temp111)->f1; goto _LL114;} else{ goto _LL115;} _LL115: if(*(( int*) _temp111)
== Cyc_Absyn_StructArrow_e){ _LL124: _temp123=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp111)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp111)->f2; goto _LL116;} else{ goto _LL117;} _LL117: goto _LL118; _LL114:
env.access_path=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp110.lvalues=({ struct Cyc_List_List* _temp125=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp125->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp91); _temp125->tl= 0; _temp125;}); goto _LL112; _LL116: env.access_path=({
struct Cyc_List_List* _temp126=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp126->hd=( void*) Cyc_Absyn_structmember_exp(
_temp123, _temp121, 0); _temp126->tl=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp126;}); _temp110.lvalues=({ struct Cyc_List_List* _temp127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp127->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp91); _temp127->tl= 0; _temp127;}); goto _LL112; _LL118:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp128=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp128[ 0]=({ struct
Cyc_Core_Impossible_struct _temp129; _temp129.tag= Cyc_Core_Impossible; _temp129.f1=
_tag_arr("bad malloc access path", sizeof( unsigned char), 23u); _temp129;});
_temp128;})); _LL112:;} return _temp110;}} struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp130=(
void*) e->r; struct Cyc_Absyn_Exp* _temp220; struct Cyc_List_List* _temp222;
struct Cyc_Absyn_Exp* _temp224; void* _temp226; int _temp228; void* _temp230;
void* _temp232; int _temp234; void* _temp236; struct Cyc_Absyn_Exp* _temp238;
void* _temp240; struct _tagged_arr* _temp242; void* _temp244; void* _temp246;
struct Cyc_Absyn_Enumfield* _temp248; struct Cyc_Absyn_Enumdecl* _temp250;
struct _tuple1* _temp252; void* _temp254; struct Cyc_Absyn_Fndecl* _temp256;
struct _tuple1* _temp258; void* _temp260; struct Cyc_Absyn_Vardecl* _temp262;
struct _tuple1* _temp264; void* _temp266; struct Cyc_Absyn_Vardecl* _temp268;
struct _tuple1* _temp270; void* _temp272; struct Cyc_Absyn_Vardecl* _temp274;
struct _tuple1* _temp276; void* _temp278; struct Cyc_Absyn_Vardecl* _temp280;
struct _tuple1* _temp282; struct Cyc_Absyn_Exp* _temp284; struct Cyc_Core_Opt*
_temp286; struct Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct
Cyc_Core_Opt* _temp292; struct Cyc_Absyn_Exp* _temp294; struct Cyc_List_List*
_temp296; void* _temp298; struct Cyc_Absyn_Exp* _temp300; void* _temp302; struct
Cyc_Absyn_VarargCallInfo* _temp304; struct Cyc_List_List* _temp306; struct Cyc_Absyn_Exp*
_temp308; struct Cyc_Absyn_Exp* _temp310; struct Cyc_Absyn_Exp* _temp312; struct
Cyc_Absyn_Exp* _temp314; struct Cyc_Absyn_Exp* _temp316; struct Cyc_Absyn_Exp*
_temp318; void* _temp320; struct Cyc_Absyn_Exp* _temp322; struct Cyc_Absyn_Exp*
_temp324; struct _tagged_arr* _temp326; struct Cyc_Absyn_Exp* _temp328; struct
_tagged_arr* _temp330; struct Cyc_Absyn_Exp* _temp332; struct Cyc_Absyn_Tunionfield*
_temp334; struct Cyc_Absyn_Tuniondecl* _temp336; struct Cyc_List_List* _temp338;
struct Cyc_Core_Opt* _temp340; struct Cyc_Core_Opt* _temp342; struct Cyc_List_List*
_temp344; struct Cyc_List_List* _temp346; void* _temp348; struct Cyc_Absyn_Structdecl*
_temp350; struct Cyc_List_List* _temp352; struct Cyc_Core_Opt* _temp354; struct
_tuple1* _temp356; struct Cyc_List_List* _temp358; void* _temp360; struct Cyc_Absyn_Exp*
_temp362; struct Cyc_Absyn_Exp* _temp364; struct Cyc_Absyn_Exp* _temp366; struct
Cyc_Absyn_Exp* _temp368; struct Cyc_Absyn_Exp* _temp370; struct Cyc_Absyn_Stmt*
_temp372; struct Cyc_Absyn_Exp* _temp374; struct Cyc_Absyn_Exp* _temp376; struct
Cyc_Absyn_Exp* _temp378; struct Cyc_Absyn_Exp* _temp380; struct Cyc_Absyn_Vardecl*
_temp382; void* _temp384; struct _tuple1* _temp386; struct _tuple1* _temp388;
struct Cyc_List_List* _temp390; struct Cyc_Absyn_Exp* _temp392; struct Cyc_List_List*
_temp394; struct Cyc_Core_Opt* _temp396; struct Cyc_List_List* _temp398; struct
_tuple2* _temp400; struct Cyc_Absyn_Fndecl* _temp402; struct Cyc_Absyn_Exp*
_temp404; _LL132: if(*(( int*) _temp130) == Cyc_Absyn_NoInstantiate_e){ _LL221:
_temp220=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp130)->f1; goto _LL133;}
else{ goto _LL134;} _LL134: if(*(( int*) _temp130) == Cyc_Absyn_Instantiate_e){
_LL225: _temp224=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp130)->f1; goto
_LL223; _LL223: _temp222=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp130)->f2;
goto _LL135;} else{ goto _LL136;} _LL136: if(*(( int*) _temp130) == Cyc_Absyn_Const_e){
_LL227: _temp226=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp130)->f1; if((
unsigned int) _temp226 > 1u?*(( int*) _temp226) == Cyc_Absyn_Int_c: 0){ _LL231:
_temp230=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp226)->f1; if( _temp230
==( void*) Cyc_Absyn_Signed){ goto _LL229;} else{ goto _LL138;} _LL229: _temp228=((
struct Cyc_Absyn_Int_c_struct*) _temp226)->f2; if( _temp228 == 0){ goto _LL137;}
else{ goto _LL138;}} else{ goto _LL138;}} else{ goto _LL138;} _LL138: if(*(( int*)
_temp130) == Cyc_Absyn_Const_e){ _LL233: _temp232=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp130)->f1; if(( unsigned int) _temp232 > 1u?*(( int*) _temp232) == Cyc_Absyn_Int_c:
0){ _LL237: _temp236=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp232)->f1;
if( _temp236 ==( void*) Cyc_Absyn_Signed){ goto _LL235;} else{ goto _LL140;}
_LL235: _temp234=(( struct Cyc_Absyn_Int_c_struct*) _temp232)->f2; if( _temp234
== 1){ goto _LL139;} else{ goto _LL140;}} else{ goto _LL140;}} else{ goto _LL140;}
_LL140: if(*(( int*) _temp130) == Cyc_Absyn_Sizeofexp_e){ _LL239: _temp238=((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp130)->f1; goto _LL141;} else{ goto
_LL142;} _LL142: if(*(( int*) _temp130) == Cyc_Absyn_Sizeoftyp_e){ _LL241:
_temp240=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp130)->f1; goto
_LL143;} else{ goto _LL144;} _LL144: if(*(( int*) _temp130) == Cyc_Absyn_Offsetof_e){
_LL245: _temp244=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp130)->f1;
goto _LL243; _LL243: _temp242=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp130)->f2;
goto _LL145;} else{ goto _LL146;} _LL146: if(*(( int*) _temp130) == Cyc_Absyn_Const_e){
_LL247: _temp246=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp130)->f1;
goto _LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp130) == Cyc_Absyn_Enum_e){
_LL253: _temp252=(( struct Cyc_Absyn_Enum_e_struct*) _temp130)->f1; goto _LL251;
_LL251: _temp250=(( struct Cyc_Absyn_Enum_e_struct*) _temp130)->f2; goto _LL249;
_LL249: _temp248=(( struct Cyc_Absyn_Enum_e_struct*) _temp130)->f3; goto _LL149;}
else{ goto _LL150;} _LL150: if(*(( int*) _temp130) == Cyc_Absyn_Var_e){ _LL259:
_temp258=(( struct Cyc_Absyn_Var_e_struct*) _temp130)->f1; goto _LL255; _LL255:
_temp254=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp130)->f2; if((
unsigned int) _temp254 > 1u?*(( int*) _temp254) == Cyc_Absyn_Funname_b: 0){
_LL257: _temp256=(( struct Cyc_Absyn_Funname_b_struct*) _temp254)->f1; goto
_LL151;} else{ goto _LL152;}} else{ goto _LL152;} _LL152: if(*(( int*) _temp130)
== Cyc_Absyn_Var_e){ _LL265: _temp264=(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f1; goto _LL261; _LL261: _temp260=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f2; if(( unsigned int) _temp260 > 1u?*(( int*) _temp260) == Cyc_Absyn_Global_b:
0){ _LL263: _temp262=(( struct Cyc_Absyn_Global_b_struct*) _temp260)->f1; goto
_LL153;} else{ goto _LL154;}} else{ goto _LL154;} _LL154: if(*(( int*) _temp130)
== Cyc_Absyn_Var_e){ _LL271: _temp270=(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f1; goto _LL267; _LL267: _temp266=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f2; if(( unsigned int) _temp266 > 1u?*(( int*) _temp266) == Cyc_Absyn_Param_b:
0){ _LL269: _temp268=(( struct Cyc_Absyn_Param_b_struct*) _temp266)->f1; goto
_LL155;} else{ goto _LL156;}} else{ goto _LL156;} _LL156: if(*(( int*) _temp130)
== Cyc_Absyn_Var_e){ _LL277: _temp276=(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f1; goto _LL273; _LL273: _temp272=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f2; if(( unsigned int) _temp272 > 1u?*(( int*) _temp272) == Cyc_Absyn_Local_b:
0){ _LL275: _temp274=(( struct Cyc_Absyn_Local_b_struct*) _temp272)->f1; goto
_LL157;} else{ goto _LL158;}} else{ goto _LL158;} _LL158: if(*(( int*) _temp130)
== Cyc_Absyn_Var_e){ _LL283: _temp282=(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f1; goto _LL279; _LL279: _temp278=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f2; if(( unsigned int) _temp278 > 1u?*(( int*) _temp278) == Cyc_Absyn_Pat_b:
0){ _LL281: _temp280=(( struct Cyc_Absyn_Pat_b_struct*) _temp278)->f1; goto
_LL159;} else{ goto _LL160;}} else{ goto _LL160;} _LL160: if(*(( int*) _temp130)
== Cyc_Absyn_AssignOp_e){ _LL289: _temp288=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp130)->f1; goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp130)->f2; if( _temp286 == 0){ goto _LL285;} else{ goto _LL162;} _LL285:
_temp284=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp130)->f3; goto _LL161;}
else{ goto _LL162;} _LL162: if(*(( int*) _temp130) == Cyc_Absyn_AssignOp_e){
_LL295: _temp294=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp130)->f1; goto
_LL293; _LL293: _temp292=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp130)->f2;
goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp130)->f3;
goto _LL163;} else{ goto _LL164;} _LL164: if(*(( int*) _temp130) == Cyc_Absyn_Primop_e){
_LL299: _temp298=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp130)->f1;
goto _LL297; _LL297: _temp296=(( struct Cyc_Absyn_Primop_e_struct*) _temp130)->f2;
goto _LL165;} else{ goto _LL166;} _LL166: if(*(( int*) _temp130) == Cyc_Absyn_Cast_e){
_LL303: _temp302=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp130)->f1; goto
_LL301; _LL301: _temp300=(( struct Cyc_Absyn_Cast_e_struct*) _temp130)->f2; goto
_LL167;} else{ goto _LL168;} _LL168: if(*(( int*) _temp130) == Cyc_Absyn_FnCall_e){
_LL309: _temp308=(( struct Cyc_Absyn_FnCall_e_struct*) _temp130)->f1; goto
_LL307; _LL307: _temp306=(( struct Cyc_Absyn_FnCall_e_struct*) _temp130)->f2;
goto _LL305; _LL305: _temp304=(( struct Cyc_Absyn_FnCall_e_struct*) _temp130)->f3;
goto _LL169;} else{ goto _LL170;} _LL170: if(*(( int*) _temp130) == Cyc_Absyn_Subscript_e){
_LL313: _temp312=(( struct Cyc_Absyn_Subscript_e_struct*) _temp130)->f1; goto
_LL311; _LL311: _temp310=(( struct Cyc_Absyn_Subscript_e_struct*) _temp130)->f2;
goto _LL171;} else{ goto _LL172;} _LL172: if(*(( int*) _temp130) == Cyc_Absyn_Address_e){
_LL315: _temp314=(( struct Cyc_Absyn_Address_e_struct*) _temp130)->f1; goto
_LL173;} else{ goto _LL174;} _LL174: if(*(( int*) _temp130) == Cyc_Absyn_New_e){
_LL319: _temp318=(( struct Cyc_Absyn_New_e_struct*) _temp130)->f1; goto _LL317;
_LL317: _temp316=(( struct Cyc_Absyn_New_e_struct*) _temp130)->f2; goto _LL175;}
else{ goto _LL176;} _LL176: if(*(( int*) _temp130) == Cyc_Absyn_Malloc_e){
_LL323: _temp322=(( struct Cyc_Absyn_Malloc_e_struct*) _temp130)->f1; goto
_LL321; _LL321: _temp320=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp130)->f2;
goto _LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp130) == Cyc_Absyn_Deref_e){
_LL325: _temp324=(( struct Cyc_Absyn_Deref_e_struct*) _temp130)->f1; goto _LL179;}
else{ goto _LL180;} _LL180: if(*(( int*) _temp130) == Cyc_Absyn_StructArrow_e){
_LL329: _temp328=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp130)->f1; goto
_LL327; _LL327: _temp326=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp130)->f2;
goto _LL181;} else{ goto _LL182;} _LL182: if(*(( int*) _temp130) == Cyc_Absyn_StructMember_e){
_LL333: _temp332=(( struct Cyc_Absyn_StructMember_e_struct*) _temp130)->f1; goto
_LL331; _LL331: _temp330=(( struct Cyc_Absyn_StructMember_e_struct*) _temp130)->f2;
goto _LL183;} else{ goto _LL184;} _LL184: if(*(( int*) _temp130) == Cyc_Absyn_Tunion_e){
_LL343: _temp342=(( struct Cyc_Absyn_Tunion_e_struct*) _temp130)->f1; goto
_LL341; _LL341: _temp340=(( struct Cyc_Absyn_Tunion_e_struct*) _temp130)->f2;
goto _LL339; _LL339: _temp338=(( struct Cyc_Absyn_Tunion_e_struct*) _temp130)->f3;
goto _LL337; _LL337: _temp336=(( struct Cyc_Absyn_Tunion_e_struct*) _temp130)->f4;
goto _LL335; _LL335: _temp334=(( struct Cyc_Absyn_Tunion_e_struct*) _temp130)->f5;
goto _LL185;} else{ goto _LL186;} _LL186: if(*(( int*) _temp130) == Cyc_Absyn_Tuple_e){
_LL345: _temp344=(( struct Cyc_Absyn_Tuple_e_struct*) _temp130)->f1; goto _LL187;}
else{ goto _LL188;} _LL188: if(*(( int*) _temp130) == Cyc_Absyn_AnonStruct_e){
_LL349: _temp348=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp130)->f1;
goto _LL347; _LL347: _temp346=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp130)->f2;
goto _LL189;} else{ goto _LL190;} _LL190: if(*(( int*) _temp130) == Cyc_Absyn_Struct_e){
_LL357: _temp356=(( struct Cyc_Absyn_Struct_e_struct*) _temp130)->f1; goto
_LL355; _LL355: _temp354=(( struct Cyc_Absyn_Struct_e_struct*) _temp130)->f2;
goto _LL353; _LL353: _temp352=(( struct Cyc_Absyn_Struct_e_struct*) _temp130)->f3;
goto _LL351; _LL351: _temp350=(( struct Cyc_Absyn_Struct_e_struct*) _temp130)->f4;
goto _LL191;} else{ goto _LL192;} _LL192: if(*(( int*) _temp130) == Cyc_Absyn_Array_e){
_LL359: _temp358=(( struct Cyc_Absyn_Array_e_struct*) _temp130)->f1; goto _LL193;}
else{ goto _LL194;} _LL194: if(*(( int*) _temp130) == Cyc_Absyn_Increment_e){
_LL363: _temp362=(( struct Cyc_Absyn_Increment_e_struct*) _temp130)->f1; goto
_LL361; _LL361: _temp360=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp130)->f2; goto _LL195;} else{ goto _LL196;} _LL196: if(*(( int*) _temp130)
== Cyc_Absyn_Throw_e){ _LL365: _temp364=(( struct Cyc_Absyn_Throw_e_struct*)
_temp130)->f1; goto _LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp130)
== Cyc_Absyn_Conditional_e){ _LL371: _temp370=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp130)->f1; goto _LL369; _LL369: _temp368=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp130)->f2; goto _LL367; _LL367: _temp366=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp130)->f3; goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*) _temp130)
== Cyc_Absyn_StmtExp_e){ _LL373: _temp372=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp130)->f1; goto _LL201;} else{ goto _LL202;} _LL202: if(*(( int*) _temp130)
== Cyc_Absyn_SeqExp_e){ _LL377: _temp376=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp130)->f1; goto _LL375; _LL375: _temp374=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp130)->f2; goto _LL203;} else{ goto _LL204;} _LL204: if(*(( int*) _temp130)
== Cyc_Absyn_Comprehension_e){ _LL383: _temp382=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp130)->f1; goto _LL381; _LL381: _temp380=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp130)->f2; goto _LL379; _LL379: _temp378=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp130)->f3; goto _LL205;} else{ goto _LL206;} _LL206: if(*(( int*) _temp130)
== Cyc_Absyn_Var_e){ _LL387: _temp386=(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f1; goto _LL385; _LL385: _temp384=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp130)->f2; if( _temp384 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL207;}
else{ goto _LL208;}} else{ goto _LL208;} _LL208: if(*(( int*) _temp130) == Cyc_Absyn_UnknownId_e){
_LL389: _temp388=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp130)->f1; goto
_LL209;} else{ goto _LL210;} _LL210: if(*(( int*) _temp130) == Cyc_Absyn_UnknownCall_e){
_LL393: _temp392=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp130)->f1; goto
_LL391; _LL391: _temp390=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp130)->f2;
goto _LL211;} else{ goto _LL212;} _LL212: if(*(( int*) _temp130) == Cyc_Absyn_UnresolvedMem_e){
_LL397: _temp396=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp130)->f1;
goto _LL395; _LL395: _temp394=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp130)->f2; goto _LL213;} else{ goto _LL214;} _LL214: if(*(( int*) _temp130)
== Cyc_Absyn_CompoundLit_e){ _LL401: _temp400=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp130)->f1; goto _LL399; _LL399: _temp398=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp130)->f2; goto _LL215;} else{ goto _LL216;} _LL216: if(*(( int*) _temp130)
== Cyc_Absyn_Codegen_e){ _LL403: _temp402=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp130)->f1; goto _LL217;} else{ goto _LL218;} _LL218: if(*(( int*) _temp130)
== Cyc_Absyn_Fill_e){ _LL405: _temp404=(( struct Cyc_Absyn_Fill_e_struct*)
_temp130)->f1; goto _LL219;} else{ goto _LL131;} _LL133: _temp224= _temp220;
goto _LL135; _LL135: return Cyc_NewControlFlow_abstract_exp( env, _temp224);
_LL137: env.access_path == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:284 env.access_path==null", sizeof( unsigned char),
47u)); return({ struct Cyc_NewControlFlow_AbsSyn _temp406; _temp406.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp408; _temp408.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp408.assigns=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp408;}); _temp406.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp407; _temp407.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp407.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp407;}); _temp406.lvalues= 0; _temp406;}); _LL139: env.access_path == 0? 0:((
int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:289 env.access_path==null",
sizeof( unsigned char), 47u)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp409; _temp409.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp411;
_temp411.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp411.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp411;});
_temp409.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp410; _temp410.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp410.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp410;}); _temp409.lvalues= 0; _temp409;}); _LL141: goto _LL143; _LL143: goto
_LL145; _LL145: goto _LL147; _LL147: goto _LL149; _LL149: goto _LL151; _LL151:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL153: return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL155: _temp274= _temp268; goto _LL157;
_LL157: _temp280= _temp274; goto _LL159; _LL159: { void* absop= Cyc_NewControlFlow_destruct_path(
env, Cyc_CfAbsexp_mkLocalOp( _temp280)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp412=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp412->hd=( void*) absop; _temp412->tl= 0; _temp412;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL161: { struct Cyc_NewControlFlow_AbsSyn
_temp413= Cyc_NewControlFlow_abstract_exp( env, _temp288); struct Cyc_List_List*
_temp414= _temp413.lvalues == 0? Cyc_NewControlFlow_use_it_list(): _temp413.lvalues;
struct Cyc_NewControlFlow_AbsSyn _temp415= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp414), _temp284); void* _temp418; void* _temp420; struct Cyc_NewControlFlow_AbsSynOne
_temp416= Cyc_NewControlFlow_meet_absexp( _temp413); _LL421: _temp420=( void*)
_temp416.inner_exp; goto _LL419; _LL419: _temp418=( void*) _temp416.assigns;
goto _LL417; _LL417: { void* _temp424; void* _temp426; struct Cyc_NewControlFlow_AbsSynOne
_temp422= Cyc_NewControlFlow_meet_absexp( _temp415); _LL427: _temp426=( void*)
_temp422.inner_exp; goto _LL425; _LL425: _temp424=( void*) _temp422.assigns;
goto _LL423; _LL423: { void* _temp428= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp420, _temp426),
_temp424); return Cyc_NewControlFlow_mkSyn_tf( _temp428, 0, _temp418);}}} _LL163:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp429=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp429->hd=( void*) _temp294; _temp429->tl=({ struct Cyc_List_List* _temp430=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp430->hd=(
void*) _temp290; _temp430->tl= 0; _temp430;}); _temp429;}), 0); _LL165: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp296, 0); _LL167: { int both_leaf=
1;{ void* _temp431=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp300->topt))->v;
struct Cyc_List_List* _temp441; struct Cyc_Absyn_Structdecl** _temp443; struct
Cyc_List_List* _temp445; struct _tuple1* _temp447; struct Cyc_List_List*
_temp449; _LL433: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_AnonStructType:
0){ _LL442: _temp441=(( struct Cyc_Absyn_AnonStructType_struct*) _temp431)->f1;
goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int) _temp431 > 4u?*((
int*) _temp431) == Cyc_Absyn_StructType: 0){ _LL448: _temp447=(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f1; goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f2; goto _LL444; _LL444: _temp443=(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f3; goto _LL436;} else{ goto _LL437;} _LL437: if(( unsigned int)
_temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_TupleType: 0){ _LL450: _temp449=((
struct Cyc_Absyn_TupleType_struct*) _temp431)->f1; goto _LL438;} else{ goto
_LL439;} _LL439: goto _LL440; _LL434: goto _LL436; _LL436: goto _LL438; _LL438:
both_leaf= 0; goto _LL432; _LL440: goto _LL432; _LL432:;}{ void* _temp451=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; struct Cyc_List_List* _temp461;
struct Cyc_Absyn_Structdecl** _temp463; struct Cyc_List_List* _temp465; struct
_tuple1* _temp467; struct Cyc_List_List* _temp469; _LL453: if(( unsigned int)
_temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_AnonStructType: 0){ _LL462:
_temp461=(( struct Cyc_Absyn_AnonStructType_struct*) _temp451)->f1; goto _LL454;}
else{ goto _LL455;} _LL455: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451)
== Cyc_Absyn_StructType: 0){ _LL468: _temp467=(( struct Cyc_Absyn_StructType_struct*)
_temp451)->f1; goto _LL466; _LL466: _temp465=(( struct Cyc_Absyn_StructType_struct*)
_temp451)->f2; goto _LL464; _LL464: _temp463=(( struct Cyc_Absyn_StructType_struct*)
_temp451)->f3; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_TupleType: 0){ _LL470: _temp469=((
struct Cyc_Absyn_TupleType_struct*) _temp451)->f1; goto _LL458;} else{ goto
_LL459;} _LL459: goto _LL460; _LL454: goto _LL456; _LL456: goto _LL458; _LL458:
both_leaf= 0; goto _LL452; _LL460: goto _LL452; _LL452:;} if( both_leaf){ return
Cyc_NewControlFlow_abstract_exp( env, _temp300);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp471=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp471->hd=( void*) _temp300; _temp471->tl= 0;
_temp471;}), env.lhsides);} _LL169: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp472=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp472->hd=( void*) _temp308; _temp472->tl=
_temp306; _temp472;}), 0); _LL171: { void* _temp473= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp312->topt))->v); struct Cyc_List_List*
_temp479; _LL475: if(( unsigned int) _temp473 > 4u?*(( int*) _temp473) == Cyc_Absyn_TupleType:
0){ _LL480: _temp479=(( struct Cyc_Absyn_TupleType_struct*) _temp473)->f1; goto
_LL476;} else{ goto _LL477;} _LL477: goto _LL478; _LL476: return Cyc_NewControlFlow_descend_path(
env, e, _temp312); _LL478: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp481=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp481->hd=( void*) _temp312; _temp481->tl=({ struct
Cyc_List_List* _temp482=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp482->hd=( void*) _temp310; _temp482->tl= 0; _temp482;}); _temp481;}), Cyc_NewControlFlow_use_it_list());
_LL474:;} _LL173: { int is_malloc= 0;{ void* _temp483=( void*) _temp314->r;
struct Cyc_Absyn_Structdecl* _temp491; struct Cyc_List_List* _temp493; struct
Cyc_Core_Opt* _temp495; struct _tuple1* _temp497; struct Cyc_List_List* _temp499;
_LL485: if(*(( int*) _temp483) == Cyc_Absyn_Struct_e){ _LL498: _temp497=((
struct Cyc_Absyn_Struct_e_struct*) _temp483)->f1; goto _LL496; _LL496: _temp495=((
struct Cyc_Absyn_Struct_e_struct*) _temp483)->f2; goto _LL494; _LL494: _temp493=((
struct Cyc_Absyn_Struct_e_struct*) _temp483)->f3; goto _LL492; _LL492: _temp491=((
struct Cyc_Absyn_Struct_e_struct*) _temp483)->f4; goto _LL486;} else{ goto
_LL487;} _LL487: if(*(( int*) _temp483) == Cyc_Absyn_Tuple_e){ _LL500: _temp499=((
struct Cyc_Absyn_Tuple_e_struct*) _temp483)->f1; goto _LL488;} else{ goto _LL489;}
_LL489: goto _LL490; _LL486: goto _LL488; _LL488: is_malloc= 1; goto _LL484;
_LL490: goto _LL484; _LL484:;} if( is_malloc){ _temp318= 0; _temp316= _temp314;
goto _LL175;}{ struct Cyc_List_List* _temp501= env.access_path; struct Cyc_List_List
_temp513; struct Cyc_List_List* _temp514; struct Cyc_Absyn_Exp* _temp516; struct
Cyc_Absyn_Exp _temp518; struct Cyc_Position_Segment* _temp519; void* _temp521;
struct Cyc_Absyn_Exp* _temp523; struct Cyc_Absyn_Exp* _temp525; struct Cyc_Core_Opt*
_temp527; struct Cyc_List_List _temp529; struct Cyc_List_List* _temp530; struct
Cyc_Absyn_Exp* _temp532; struct Cyc_Absyn_Exp _temp534; struct Cyc_Position_Segment*
_temp535; void* _temp537; struct Cyc_Absyn_Exp* _temp539; struct Cyc_Core_Opt*
_temp541; struct Cyc_List_List _temp543; struct Cyc_List_List* _temp544; struct
Cyc_Absyn_Exp* _temp546; struct Cyc_Absyn_Exp _temp548; struct Cyc_Position_Segment*
_temp549; void* _temp551; struct _tagged_arr* _temp553; struct Cyc_Absyn_Exp*
_temp555; struct Cyc_Core_Opt* _temp557; _LL503: if( _temp501 == 0){ goto _LL504;}
else{ goto _LL505;} _LL505: if( _temp501 == 0){ goto _LL507;} else{ _temp513=*
_temp501; _LL517: _temp516=( struct Cyc_Absyn_Exp*) _temp513.hd; _temp518=*
_temp516; _LL528: _temp527= _temp518.topt; goto _LL522; _LL522: _temp521=( void*)
_temp518.r; if(*(( int*) _temp521) == Cyc_Absyn_Subscript_e){ _LL526: _temp525=((
struct Cyc_Absyn_Subscript_e_struct*) _temp521)->f1; goto _LL524; _LL524:
_temp523=(( struct Cyc_Absyn_Subscript_e_struct*) _temp521)->f2; goto _LL520;}
else{ goto _LL507;} _LL520: _temp519= _temp518.loc; goto _LL515; _LL515:
_temp514= _temp513.tl; goto _LL506;} _LL507: if( _temp501 == 0){ goto _LL509;}
else{ _temp529=* _temp501; _LL533: _temp532=( struct Cyc_Absyn_Exp*) _temp529.hd;
_temp534=* _temp532; _LL542: _temp541= _temp534.topt; goto _LL538; _LL538:
_temp537=( void*) _temp534.r; if(*(( int*) _temp537) == Cyc_Absyn_Deref_e){
_LL540: _temp539=(( struct Cyc_Absyn_Deref_e_struct*) _temp537)->f1; goto _LL536;}
else{ goto _LL509;} _LL536: _temp535= _temp534.loc; goto _LL531; _LL531:
_temp530= _temp529.tl; goto _LL508;} _LL509: if( _temp501 == 0){ goto _LL511;}
else{ _temp543=* _temp501; _LL547: _temp546=( struct Cyc_Absyn_Exp*) _temp543.hd;
_temp548=* _temp546; _LL558: _temp557= _temp548.topt; goto _LL552; _LL552:
_temp551=( void*) _temp548.r; if(*(( int*) _temp551) == Cyc_Absyn_StructArrow_e){
_LL556: _temp555=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp551)->f1; goto
_LL554; _LL554: _temp553=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp551)->f2;
goto _LL550;} else{ goto _LL511;} _LL550: _temp549= _temp548.loc; goto _LL545;
_LL545: _temp544= _temp543.tl; goto _LL510;} _LL511: goto _LL512; _LL504: env.access_path=({
struct Cyc_List_List* _temp559=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp559->hd=( void*) e; _temp559->tl= env.access_path;
_temp559;}); goto _LL502; _LL506: Cyc_Evexp_eval_const_uint_exp( _temp523) == 0?
0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:413 Evexp::eval_const_uint_exp(e2) == 0",
sizeof( unsigned char), 61u)); _temp530= _temp514; goto _LL508; _LL508: env.access_path=
_temp530; goto _LL502; _LL510: env.access_path=({ struct Cyc_List_List* _temp560=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp560->hd=(
void*) Cyc_Absyn_structmember_exp( _temp555, _temp553, 0); _temp560->tl=
_temp544; _temp560;}); goto _LL502; _LL512:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp561=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp561[ 0]=({ struct Cyc_Core_Impossible_struct _temp562; _temp562.tag= Cyc_Core_Impossible;
_temp562.f1= _tag_arr("abstract_exp: bad access path for &", sizeof(
unsigned char), 36u); _temp562;}); _temp561;})); _LL502:;} return Cyc_NewControlFlow_abstract_exp(
env, _temp314);} _LL175: Cyc_NewControlFlow_add_malloc_root( env, e,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp316->topt))->v); return Cyc_NewControlFlow_abstract_malloc(
env, e, _temp318,( struct Cyc_Absyn_Exp*) _temp316); _LL177: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp320); return Cyc_NewControlFlow_abstract_malloc( env, e, _temp322,
0); _LL179: { struct Cyc_List_List* _temp563= env.access_path; struct Cyc_List_List
_temp569; struct Cyc_List_List* _temp570; struct Cyc_Absyn_Exp* _temp572; struct
Cyc_Absyn_Exp _temp574; struct Cyc_Position_Segment* _temp575; void* _temp577;
struct Cyc_Absyn_Exp* _temp579; struct Cyc_Core_Opt* _temp581; _LL565: if(
_temp563 == 0){ goto _LL567;} else{ _temp569=* _temp563; _LL573: _temp572=(
struct Cyc_Absyn_Exp*) _temp569.hd; _temp574=* _temp572; _LL582: _temp581=
_temp574.topt; goto _LL578; _LL578: _temp577=( void*) _temp574.r; if(*(( int*)
_temp577) == Cyc_Absyn_Address_e){ _LL580: _temp579=(( struct Cyc_Absyn_Address_e_struct*)
_temp577)->f1; goto _LL576;} else{ goto _LL567;} _LL576: _temp575= _temp574.loc;
goto _LL571; _LL571: _temp570= _temp569.tl; goto _LL566;} _LL567: goto _LL568;
_LL566: env.access_path= _temp570;{ struct Cyc_NewControlFlow_AbsSyn _temp583=
Cyc_NewControlFlow_abstract_exp( env, _temp324); if( _temp583.lvalues == 0){
_temp583.lvalues= Cyc_NewControlFlow_use_it_list();} return _temp583;} _LL568:
return Cyc_NewControlFlow_descend_path( env, e, _temp324); _LL564:;} _LL181: {
void* _temp584= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp328->topt))->v); struct Cyc_Absyn_PtrInfo _temp590; struct Cyc_Absyn_Conref*
_temp592; struct Cyc_Absyn_Tqual _temp594; struct Cyc_Absyn_Conref* _temp596;
void* _temp598; void* _temp600; _LL586: if(( unsigned int) _temp584 > 4u?*(( int*)
_temp584) == Cyc_Absyn_PointerType: 0){ _LL591: _temp590=(( struct Cyc_Absyn_PointerType_struct*)
_temp584)->f1; _LL601: _temp600=( void*) _temp590.elt_typ; goto _LL599; _LL599:
_temp598=( void*) _temp590.rgn_typ; goto _LL597; _LL597: _temp596= _temp590.nullable;
goto _LL595; _LL595: _temp594= _temp590.tq; goto _LL593; _LL593: _temp592=
_temp590.bounds; goto _LL587;} else{ goto _LL588;} _LL588: goto _LL589; _LL587:{
void* _temp602= Cyc_Tcutil_compress( _temp600); struct Cyc_List_List* _temp614;
struct Cyc_Absyn_Structdecl** _temp616; struct Cyc_List_List* _temp618; struct
_tuple1* _temp620; struct Cyc_List_List* _temp622; struct Cyc_Absyn_Uniondecl**
_temp624; struct Cyc_List_List* _temp626; struct _tuple1* _temp628; _LL604: if((
unsigned int) _temp602 > 4u?*(( int*) _temp602) == Cyc_Absyn_AnonStructType: 0){
_LL615: _temp614=(( struct Cyc_Absyn_AnonStructType_struct*) _temp602)->f1; goto
_LL605;} else{ goto _LL606;} _LL606: if(( unsigned int) _temp602 > 4u?*(( int*)
_temp602) == Cyc_Absyn_StructType: 0){ _LL621: _temp620=(( struct Cyc_Absyn_StructType_struct*)
_temp602)->f1; goto _LL619; _LL619: _temp618=(( struct Cyc_Absyn_StructType_struct*)
_temp602)->f2; goto _LL617; _LL617: _temp616=(( struct Cyc_Absyn_StructType_struct*)
_temp602)->f3; goto _LL607;} else{ goto _LL608;} _LL608: if(( unsigned int)
_temp602 > 4u?*(( int*) _temp602) == Cyc_Absyn_AnonUnionType: 0){ _LL623:
_temp622=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp602)->f1; goto _LL609;}
else{ goto _LL610;} _LL610: if(( unsigned int) _temp602 > 4u?*(( int*) _temp602)
== Cyc_Absyn_UnionType: 0){ _LL629: _temp628=(( struct Cyc_Absyn_UnionType_struct*)
_temp602)->f1; goto _LL627; _LL627: _temp626=(( struct Cyc_Absyn_UnionType_struct*)
_temp602)->f2; goto _LL625; _LL625: _temp624=(( struct Cyc_Absyn_UnionType_struct*)
_temp602)->f3; goto _LL611;} else{ goto _LL612;} _LL612: goto _LL613; _LL605:
goto _LL607; _LL607: return Cyc_NewControlFlow_descend_path( env, e, _temp328);
_LL609: goto _LL611; _LL611: { struct Cyc_NewControlFlow_AbsSyn _temp630= Cyc_NewControlFlow_abstract_exp(
env, _temp328); _temp630.lvalues= Cyc_NewControlFlow_use_it_list(); return
_temp630;} _LL613: goto _LL603; _LL603:;} goto _LL589; _LL589:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp631=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp631[ 0]=({ struct
Cyc_Core_Impossible_struct _temp632; _temp632.tag= Cyc_Core_Impossible; _temp632.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp632;}); _temp631;})); _LL585:;} _LL183: { void* _temp633= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp332->topt))->v); struct Cyc_List_List*
_temp645; struct Cyc_Absyn_Structdecl** _temp647; struct Cyc_List_List* _temp649;
struct _tuple1* _temp651; struct Cyc_List_List* _temp653; struct Cyc_Absyn_Uniondecl**
_temp655; struct Cyc_List_List* _temp657; struct _tuple1* _temp659; _LL635: if((
unsigned int) _temp633 > 4u?*(( int*) _temp633) == Cyc_Absyn_AnonStructType: 0){
_LL646: _temp645=(( struct Cyc_Absyn_AnonStructType_struct*) _temp633)->f1; goto
_LL636;} else{ goto _LL637;} _LL637: if(( unsigned int) _temp633 > 4u?*(( int*)
_temp633) == Cyc_Absyn_StructType: 0){ _LL652: _temp651=(( struct Cyc_Absyn_StructType_struct*)
_temp633)->f1; goto _LL650; _LL650: _temp649=(( struct Cyc_Absyn_StructType_struct*)
_temp633)->f2; goto _LL648; _LL648: _temp647=(( struct Cyc_Absyn_StructType_struct*)
_temp633)->f3; goto _LL638;} else{ goto _LL639;} _LL639: if(( unsigned int)
_temp633 > 4u?*(( int*) _temp633) == Cyc_Absyn_AnonUnionType: 0){ _LL654:
_temp653=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp633)->f1; goto _LL640;}
else{ goto _LL641;} _LL641: if(( unsigned int) _temp633 > 4u?*(( int*) _temp633)
== Cyc_Absyn_UnionType: 0){ _LL660: _temp659=(( struct Cyc_Absyn_UnionType_struct*)
_temp633)->f1; goto _LL658; _LL658: _temp657=(( struct Cyc_Absyn_UnionType_struct*)
_temp633)->f2; goto _LL656; _LL656: _temp655=(( struct Cyc_Absyn_UnionType_struct*)
_temp633)->f3; goto _LL642;} else{ goto _LL643;} _LL643: goto _LL644; _LL636:
goto _LL638; _LL638: return Cyc_NewControlFlow_descend_path( env, e, _temp332);
_LL640: goto _LL642; _LL642: { struct Cyc_NewControlFlow_AbsSyn _temp661= Cyc_NewControlFlow_abstract_exp(
env, _temp332); _temp661.lvalues= Cyc_NewControlFlow_use_it_list(); return
_temp661;} _LL644:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp662=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp662[ 0]=({ struct Cyc_Core_Impossible_struct _temp663; _temp663.tag= Cyc_Core_Impossible;
_temp663.f1= _tag_arr("NewControlFlow: bad type in StructMember", sizeof(
unsigned char), 41u); _temp663;}); _temp662;})); _LL634:;} _LL185: env.access_path
== 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:483 env.access_path == null",
sizeof( unsigned char), 49u)); if( _temp338 == 0){ return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _temp344= _temp338; goto _LL187; _LL187: {
struct Cyc_List_List* _temp664= env.access_path; struct Cyc_List_List _temp672;
struct Cyc_List_List* _temp673; struct Cyc_Absyn_Exp* _temp675; struct Cyc_Absyn_Exp
_temp677; struct Cyc_Position_Segment* _temp678; void* _temp680; struct Cyc_Absyn_Exp*
_temp682; struct Cyc_Absyn_Exp* _temp684; struct Cyc_Core_Opt* _temp686; _LL666:
if( _temp664 == 0){ goto _LL667;} else{ goto _LL668;} _LL668: if( _temp664 == 0){
goto _LL670;} else{ _temp672=* _temp664; _LL676: _temp675=( struct Cyc_Absyn_Exp*)
_temp672.hd; _temp677=* _temp675; _LL687: _temp686= _temp677.topt; goto _LL681;
_LL681: _temp680=( void*) _temp677.r; if(*(( int*) _temp680) == Cyc_Absyn_Subscript_e){
_LL685: _temp684=(( struct Cyc_Absyn_Subscript_e_struct*) _temp680)->f1; goto
_LL683; _LL683: _temp682=(( struct Cyc_Absyn_Subscript_e_struct*) _temp680)->f2;
goto _LL679;} else{ goto _LL670;} _LL679: _temp678= _temp677.loc; goto _LL674;
_LL674: _temp673= _temp672.tl; goto _LL669;} _LL670: goto _LL671; _LL667: { void*
_temp688= Cyc_CfAbsexp_mkSkipAE(); void* _temp689= Cyc_CfAbsexp_mkSkipAE();{ int
j= 0; for( 0; _temp344 != 0;( _temp344=(( struct Cyc_List_List*) _check_null(
_temp344))->tl, ++ j)){ struct Cyc_List_List* _temp690= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){
if( Cyc_CfAbsexp_isUnknownOp(( void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){
_temp690=({ struct Cyc_List_List* _temp691=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp691->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp691->tl= _temp690; _temp691;});} else{ _temp690=({ struct Cyc_List_List*
_temp692=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp692->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*)(( struct Cyc_List_List*)
_check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp693=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp693[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp694; _temp694.tag= Cyc_CfFlowInfo_TupleF;
_temp694.f1= j; _temp694;}); _temp693;})); _temp692->tl= _temp690; _temp692;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp695= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp690),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp344))->hd); void* _temp698; void* _temp700; struct Cyc_NewControlFlow_AbsSynOne
_temp696= Cyc_NewControlFlow_meet_absexp( _temp695); _LL701: _temp700=( void*)
_temp696.inner_exp; goto _LL699; _LL699: _temp698=( void*) _temp696.assigns;
goto _LL697; _LL697: _temp688= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp688, _temp700); _temp689= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp689, _temp698);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp688, 0,
_temp689);} _LL669: { int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp682);
struct Cyc_NewControlFlow_AbsEnv _temp702= Cyc_NewControlFlow_change_lhs( env, 0);
_temp702.access_path= 0; env.access_path= _temp673;{ void* _temp703= Cyc_CfAbsexp_mkSkipAE();
void* _temp704= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp344 != 0;(
_temp344=(( struct Cyc_List_List*) _check_null( _temp344))->tl, ++ j)){ if( i ==
j){ void* _temp707; void* _temp709; struct Cyc_NewControlFlow_AbsSynOne _temp705=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp344))->hd)); _LL710:
_temp709=( void*) _temp705.inner_exp; goto _LL708; _LL708: _temp707=( void*)
_temp705.assigns; goto _LL706; _LL706: _temp704= _temp707; _temp703= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp703, _temp709);} else{ void* _temp713; void*
_temp715; struct Cyc_NewControlFlow_AbsSynOne _temp711= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp702,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp344))->hd)); _LL716: _temp715=( void*) _temp711.inner_exp;
goto _LL714; _LL714: _temp713=( void*) _temp711.assigns; goto _LL712; _LL712:
_temp703= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp703,
_temp715);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp703,({ struct Cyc_List_List*
_temp717=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp717->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp717->tl= 0; _temp717;}),
_temp704);}} _LL671:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp718=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp718[ 0]=({ struct Cyc_Core_Impossible_struct _temp719; _temp719.tag= Cyc_Core_Impossible;
_temp719.f1= _tag_arr("abstract_exp: unexpected access path to Tuple", sizeof(
unsigned char), 46u); _temp719;}); _temp718;})); _LL665:;} _LL189: _temp352=
_temp346; goto _LL191; _LL191: { struct Cyc_List_List* _temp720= env.access_path;
struct Cyc_List_List _temp728; struct Cyc_List_List* _temp729; struct Cyc_Absyn_Exp*
_temp731; struct Cyc_Absyn_Exp _temp733; struct Cyc_Position_Segment* _temp734;
void* _temp736; struct _tagged_arr* _temp738; struct Cyc_Absyn_Exp* _temp740;
struct Cyc_Core_Opt* _temp742; _LL722: if( _temp720 == 0){ goto _LL723;} else{
goto _LL724;} _LL724: if( _temp720 == 0){ goto _LL726;} else{ _temp728=*
_temp720; _LL732: _temp731=( struct Cyc_Absyn_Exp*) _temp728.hd; _temp733=*
_temp731; _LL743: _temp742= _temp733.topt; goto _LL737; _LL737: _temp736=( void*)
_temp733.r; if(*(( int*) _temp736) == Cyc_Absyn_StructMember_e){ _LL741:
_temp740=(( struct Cyc_Absyn_StructMember_e_struct*) _temp736)->f1; goto _LL739;
_LL739: _temp738=(( struct Cyc_Absyn_StructMember_e_struct*) _temp736)->f2; goto
_LL735;} else{ goto _LL726;} _LL735: _temp734= _temp733.loc; goto _LL730; _LL730:
_temp729= _temp728.tl; goto _LL725;} _LL726: goto _LL727; _LL723: { void*
_temp744= Cyc_CfAbsexp_mkSkipAE(); void* _temp745= Cyc_CfAbsexp_mkSkipAE(); for(
0; _temp352 != 0; _temp352=(( struct Cyc_List_List*) _check_null( _temp352))->tl){
struct Cyc_List_List* _temp746= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls != 0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp746=({ struct Cyc_List_List*
_temp747=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp747->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp747->tl= _temp746;
_temp747;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp748=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp754; struct _tagged_arr*
_temp756; _LL750: if(*(( int*) _temp748) == Cyc_Absyn_ArrayElement){ _LL755:
_temp754=(( struct Cyc_Absyn_ArrayElement_struct*) _temp748)->f1; goto _LL751;}
else{ goto _LL752;} _LL752: if(*(( int*) _temp748) == Cyc_Absyn_FieldName){
_LL757: _temp756=(( struct Cyc_Absyn_FieldName_struct*) _temp748)->f1; goto
_LL753;} else{ goto _LL749;} _LL751:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp758=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp758[ 0]=({ struct Cyc_Core_Impossible_struct _temp759; _temp759.tag= Cyc_Core_Impossible;
_temp759.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp759;}); _temp758;})); _LL753: _temp746=({ struct Cyc_List_List* _temp760=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp760->hd=(
void*) Cyc_CfAbsexp_mkMemberOp(( void*)(( struct Cyc_List_List*) _check_null( ls))->hd,(
void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp761=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp761[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp762; _temp762.tag= Cyc_CfFlowInfo_StructF;
_temp762.f1= _temp756; _temp762;}); _temp761;})); _temp760->tl= _temp746;
_temp760;}); goto _LL749; _LL749:;}}}}{ void** _temp763=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f2)->topt))->v); struct Cyc_NewControlFlow_AbsSyn
_temp764= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp746),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp352))->hd)).f2);
void* _temp767; void* _temp769; struct Cyc_NewControlFlow_AbsSynOne _temp765=
Cyc_NewControlFlow_meet_absexp( _temp764); _LL770: _temp769=( void*) _temp765.inner_exp;
goto _LL768; _LL768: _temp767=( void*) _temp765.assigns; goto _LL766; _LL766:
_temp744= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp744,
_temp769); _temp745= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp745, _temp767);}} return Cyc_NewControlFlow_mkSyn_tf( _temp744, 0, _temp745);}
_LL725: { struct Cyc_NewControlFlow_AbsEnv _temp771= Cyc_NewControlFlow_change_lhs(
env, 0); _temp771.access_path= 0; env.access_path= _temp729;{ void* _temp772=
Cyc_CfAbsexp_mkSkipAE(); void* _temp773= Cyc_CfAbsexp_mkSkipAE(); for( 0;
_temp352 != 0; _temp352=(( struct Cyc_List_List*) _check_null( _temp352))->tl){
int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp774=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp780; struct _tagged_arr*
_temp782; _LL776: if(*(( int*) _temp774) == Cyc_Absyn_ArrayElement){ _LL781:
_temp780=(( struct Cyc_Absyn_ArrayElement_struct*) _temp774)->f1; goto _LL777;}
else{ goto _LL778;} _LL778: if(*(( int*) _temp774) == Cyc_Absyn_FieldName){
_LL783: _temp782=(( struct Cyc_Absyn_FieldName_struct*) _temp774)->f1; goto
_LL779;} else{ goto _LL775;} _LL777:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp784=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp784[ 0]=({ struct Cyc_Core_Impossible_struct _temp785; _temp785.tag= Cyc_Core_Impossible;
_temp785.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp785;}); _temp784;})); _LL779: if( Cyc_String_zstrptrcmp( _temp738, _temp782)
== 0){ used= 1;} goto _LL775; _LL775:;}} if( used){ void* _temp788; void*
_temp790; struct Cyc_NewControlFlow_AbsSynOne _temp786= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f2)); _LL791: _temp790=( void*) _temp786.inner_exp;
goto _LL789; _LL789: _temp788=( void*) _temp786.assigns; goto _LL787; _LL787:
_temp773= _temp788; _temp772= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp772, _temp790);} else{ void* _temp794; void* _temp796; struct Cyc_NewControlFlow_AbsSynOne
_temp792= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp771,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp352))->hd)).f2));
_LL797: _temp796=( void*) _temp792.inner_exp; goto _LL795; _LL795: _temp794=(
void*) _temp792.assigns; goto _LL793; _LL793: _temp772= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp772, _temp796);}} return Cyc_NewControlFlow_mkSyn_tf(
_temp772, 0, _temp773);}} _LL727:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp798=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp798[ 0]=({ struct Cyc_Core_Impossible_struct _temp799; _temp799.tag= Cyc_Core_Impossible;
_temp799.f1= _tag_arr("abstract_exp: unexpected access path to Struct", sizeof(
unsigned char), 47u); _temp799;}); _temp798;})); _LL721:;} _LL193: return Cyc_NewControlFlow_use_exp_unordered(
env,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp358), 0); _LL195: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp800=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp800->hd=( void*) _temp362; _temp800->tl= 0;
_temp800;}), 0); _LL197: { struct Cyc_NewControlFlow_AbsSyn _temp801= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp364); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
_temp801)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL199: { struct Cyc_List_List* _temp804; struct Cyc_NewControlFlow_AbsSynOne
_temp806; struct Cyc_NewControlFlow_AbsSynOne _temp808; struct Cyc_NewControlFlow_AbsSyn
_temp802= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp370); _LL809: _temp808= _temp802.when_true; goto _LL807; _LL807: _temp806=
_temp802.when_false; goto _LL805; _LL805: _temp804= _temp802.lvalues; goto
_LL803; _LL803: { struct Cyc_List_List* _temp812; struct Cyc_NewControlFlow_AbsSynOne
_temp814; struct Cyc_NewControlFlow_AbsSynOne _temp816; struct Cyc_NewControlFlow_AbsSyn
_temp810= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp368); _LL817: _temp816= _temp810.when_true; goto _LL815; _LL815: _temp814=
_temp810.when_false; goto _LL813; _LL813: _temp812= _temp810.lvalues; goto
_LL811; _LL811: { struct Cyc_List_List* _temp820; struct Cyc_NewControlFlow_AbsSynOne
_temp822; struct Cyc_NewControlFlow_AbsSynOne _temp824; struct Cyc_NewControlFlow_AbsSyn
_temp818= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp366); _LL825: _temp824= _temp818.when_true; goto _LL823; _LL823: _temp822=
_temp818.when_false; goto _LL821; _LL821: _temp820= _temp818.lvalues; goto
_LL819; _LL819: { void* _temp826= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp808.inner_exp,( void*) _temp808.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp816.inner_exp,(
void*) _temp816.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp806.inner_exp,(
void*) _temp806.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp824.inner_exp,( void*) _temp824.assigns))); void* _temp827= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp808.inner_exp,(
void*) _temp808.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp814.inner_exp,( void*) _temp814.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp806.inner_exp,( void*) _temp806.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp822.inner_exp,( void*) _temp822.assigns)));
return({ struct Cyc_NewControlFlow_AbsSyn _temp828; _temp828.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp830; _temp830.inner_exp=( void*) _temp826;
_temp830.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp830;}); _temp828.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp829;
_temp829.inner_exp=( void*) _temp827; _temp829.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp829;}); _temp828.lvalues= 0; _temp828;});}}}}
_LL201: { struct Cyc_NewControlFlow_AbsEnv _temp831= Cyc_NewControlFlow_change_lhs(
env, 0); _temp831.access_path= 0; Cyc_NewControlFlow_cf_prepass( _temp831,
_temp372);{ struct Cyc_Absyn_Stmt* _temp832= _temp372; while( 1) { void*
_temp833=( void*) _temp832->r; struct Cyc_Absyn_Stmt* _temp843; struct Cyc_Absyn_Stmt*
_temp845; struct Cyc_Absyn_Stmt* _temp847; struct Cyc_Absyn_Decl* _temp849;
struct Cyc_Absyn_Exp* _temp851; _LL835: if(( unsigned int) _temp833 > 1u?*(( int*)
_temp833) == Cyc_Absyn_Seq_s: 0){ _LL846: _temp845=(( struct Cyc_Absyn_Seq_s_struct*)
_temp833)->f1; goto _LL844; _LL844: _temp843=(( struct Cyc_Absyn_Seq_s_struct*)
_temp833)->f2; goto _LL836;} else{ goto _LL837;} _LL837: if(( unsigned int)
_temp833 > 1u?*(( int*) _temp833) == Cyc_Absyn_Decl_s: 0){ _LL850: _temp849=((
struct Cyc_Absyn_Decl_s_struct*) _temp833)->f1; goto _LL848; _LL848: _temp847=((
struct Cyc_Absyn_Decl_s_struct*) _temp833)->f2; goto _LL838;} else{ goto _LL839;}
_LL839: if(( unsigned int) _temp833 > 1u?*(( int*) _temp833) == Cyc_Absyn_Exp_s:
0){ _LL852: _temp851=(( struct Cyc_Absyn_Exp_s_struct*) _temp833)->f1; goto
_LL840;} else{ goto _LL841;} _LL841: goto _LL842; _LL836: _temp832= _temp843;
continue; _LL838: _temp832= _temp847; continue; _LL840: { struct Cyc_NewControlFlow_AbsSyn
_temp853= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp851); void* _temp856; void* _temp858; struct Cyc_NewControlFlow_AbsSynOne
_temp854= Cyc_NewControlFlow_meet_absexp( _temp853); _LL859: _temp858=( void*)
_temp854.inner_exp; goto _LL857; _LL857: _temp856=( void*) _temp854.assigns;
goto _LL855; _LL855:( void*)((( struct Cyc_List_List*) _check_null(( Cyc_NewControlFlow_get_stmt_annot(
_temp832))->absexps))->hd=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp858, _temp856)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE(
_temp372), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}
_LL842:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp860=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp860[ 0]=({ struct Cyc_Core_Impossible_struct _temp861; _temp861.tag= Cyc_Core_Impossible;
_temp861.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp861;}); _temp860;})); _LL834:;}}} _LL203: { struct Cyc_NewControlFlow_AbsEnv
_temp862= Cyc_NewControlFlow_change_lhs( env, 0); _temp862.access_path= 0;{
struct Cyc_NewControlFlow_AbsSyn _temp863= Cyc_NewControlFlow_abstract_exp(
_temp862, _temp376); struct Cyc_NewControlFlow_AbsSyn _temp864= Cyc_NewControlFlow_abstract_exp(
env, _temp374); struct Cyc_NewControlFlow_AbsSynOne _temp865= Cyc_NewControlFlow_meet_absexp(
_temp863); return({ struct Cyc_NewControlFlow_AbsSyn _temp866; _temp866.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp868; _temp868.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp865.inner_exp,( void*)( _temp864.when_true).inner_exp);
_temp868.assigns=( void*)(( void*)( _temp864.when_true).assigns); _temp868;});
_temp866.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp867; _temp867.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp865.inner_exp,(
void*)( _temp864.when_false).inner_exp); _temp867.assigns=( void*)(( void*)(
_temp864.when_false).assigns); _temp867;}); _temp866.lvalues= 0; _temp866;});}}
_LL205: env.access_path == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:703 env.access_path==null", sizeof( unsigned char),
47u)); Cyc_NewControlFlow_add_var_root( env, _temp382);( env.shared)->comprehension_vars=({
struct Cyc_List_List* _temp869=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp869->hd=( void*) _temp382; _temp869->tl=( env.shared)->comprehension_vars;
_temp869;});{ struct Cyc_NewControlFlow_AbsEnv _temp870= Cyc_NewControlFlow_use_it_env(
env); void* _temp873; void* _temp875; struct Cyc_NewControlFlow_AbsSynOne
_temp871= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp870, _temp380)); _LL876: _temp875=( void*) _temp871.inner_exp; goto _LL874;
_LL874: _temp873=( void*) _temp871.assigns; goto _LL872; _LL872: { void*
_temp879; void* _temp881; struct Cyc_NewControlFlow_AbsSynOne _temp877= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp870, _temp378)); _LL882: _temp881=( void*)
_temp877.inner_exp; goto _LL880; _LL880: _temp879=( void*) _temp877.assigns;
goto _LL878; _LL878: { void* _temp883= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp875, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp873, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp382), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp881, _temp879), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( _temp883, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL207: goto _LL209; _LL209: goto _LL211;
_LL211: goto _LL213; _LL213: goto _LL215; _LL215: goto _LL217; _LL217: goto
_LL219; _LL219:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp884=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp884[ 0]=({ struct Cyc_Core_Impossible_struct _temp885; _temp885.tag= Cyc_Core_Impossible;
_temp885.f1= _tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char),
34u); _temp885;}); _temp884;})); _LL131:;} void* Cyc_NewControlFlow_abstract_exp_top(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp888;
void* _temp890; struct Cyc_NewControlFlow_AbsSynOne _temp886= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, e)); _LL891: _temp890=( void*) _temp886.inner_exp;
goto _LL889; _LL889: _temp888=( void*) _temp886.assigns; goto _LL887; _LL887:
return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp890, _temp888);}
struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn _temp892= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), e); return({ struct _tuple0 _temp893;
_temp893.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)(
_temp892.when_true).inner_exp,( void*)( _temp892.when_true).assigns); _temp893.f2=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp892.when_false).inner_exp,(
void*)( _temp892.when_false).assigns); _temp893;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp894= 0; for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){
struct Cyc_Absyn_Switch_clause _temp897; struct Cyc_Position_Segment* _temp898;
struct Cyc_Absyn_Stmt* _temp900; struct Cyc_Absyn_Exp* _temp902; struct Cyc_Core_Opt*
_temp904; struct Cyc_Absyn_Pat* _temp906; struct Cyc_Absyn_Switch_clause*
_temp895=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp897=* _temp895; _LL907: _temp906= _temp897.pattern; goto _LL905;
_LL905: _temp904= _temp897.pat_vars; goto _LL903; _LL903: _temp902= _temp897.where_clause;
goto _LL901; _LL901: _temp900= _temp897.body; goto _LL899; _LL899: _temp898=
_temp897.loc; goto _LL896; _LL896: if( _temp904 == 0){( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp908=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp908[ 0]=({ struct
Cyc_Core_Impossible_struct _temp909; _temp909.tag= Cyc_Core_Impossible; _temp909.f1=
_tag_arr("switch clause vds not set", sizeof( unsigned char), 26u); _temp909;});
_temp908;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp904))->v; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd);}} if( _temp902 != 0){ void*
_temp912; void* _temp914; struct _tuple0 _temp910= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp902)); _LL915: _temp914= _temp910.f1;
goto _LL913; _LL913: _temp912= _temp910.f2; goto _LL911; _LL911: _temp894=({
struct Cyc_List_List* _temp916=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp916->hd=( void*) _temp914; _temp916->tl=({ struct
Cyc_List_List* _temp917=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp917->hd=( void*) _temp912; _temp917->tl= _temp894; _temp917;}); _temp916;});}
Cyc_NewControlFlow_cf_prepass( env, _temp900);} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp894);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s){ env.lhsides ==
0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:755 env.lhsides == null",
sizeof( unsigned char), 45u)); env.access_path == 0? 0:(( int(*)( struct
_tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:756 env.access_path == null",
sizeof( unsigned char), 49u));{ void* _temp918=( void*) s->r; struct Cyc_Absyn_Exp*
_temp970; struct Cyc_Absyn_Stmt* _temp972; struct Cyc_Absyn_Stmt* _temp974;
struct Cyc_Absyn_Exp* _temp976; struct Cyc_Absyn_Exp* _temp978; struct Cyc_Absyn_Stmt*
_temp980; struct Cyc_Absyn_Stmt* _temp982; struct Cyc_Absyn_Exp* _temp984;
struct _tuple3 _temp986; struct Cyc_Absyn_Stmt* _temp988; struct Cyc_Absyn_Exp*
_temp990; struct Cyc_Absyn_Stmt* _temp992; struct Cyc_Absyn_Stmt* _temp994;
struct _tuple3 _temp996; struct Cyc_Absyn_Stmt* _temp998; struct Cyc_Absyn_Exp*
_temp1000; struct Cyc_Absyn_Stmt* _temp1002; struct Cyc_Absyn_Stmt* _temp1004;
struct Cyc_Absyn_Stmt* _temp1006; struct _tagged_arr* _temp1008; struct Cyc_Absyn_Stmt*
_temp1010; struct _tagged_arr* _temp1012; struct Cyc_Absyn_Stmt* _temp1014;
struct _tuple3 _temp1016; struct Cyc_Absyn_Stmt* _temp1018; struct Cyc_Absyn_Exp*
_temp1020; struct _tuple3 _temp1022; struct Cyc_Absyn_Stmt* _temp1024; struct
Cyc_Absyn_Exp* _temp1026; struct Cyc_Absyn_Exp* _temp1028; struct Cyc_List_List*
_temp1030; struct Cyc_Absyn_Exp* _temp1032; struct Cyc_Absyn_Switch_clause**
_temp1034; struct Cyc_List_List* _temp1036; struct Cyc_Absyn_Stmt* _temp1038;
struct Cyc_Absyn_Decl* _temp1040; struct Cyc_Absyn_Decl _temp1042; struct Cyc_Position_Segment*
_temp1043; void* _temp1045; struct Cyc_Absyn_Vardecl* _temp1047; struct Cyc_Absyn_Stmt*
_temp1049; struct Cyc_Absyn_Decl* _temp1051; struct Cyc_Absyn_Decl _temp1053;
struct Cyc_Position_Segment* _temp1054; void* _temp1056; int _temp1058; struct
Cyc_Absyn_Exp* _temp1060; struct Cyc_Core_Opt* _temp1062; struct Cyc_Core_Opt*
_temp1064; struct Cyc_Core_Opt _temp1066; struct Cyc_List_List* _temp1067;
struct Cyc_Absyn_Pat* _temp1069; struct Cyc_Absyn_Stmt* _temp1071; struct Cyc_Absyn_Decl*
_temp1073; struct Cyc_Absyn_Decl _temp1075; struct Cyc_Position_Segment*
_temp1076; void* _temp1078; struct Cyc_List_List* _temp1080; struct Cyc_Absyn_Stmt*
_temp1082; struct _tagged_arr* _temp1084; struct Cyc_List_List* _temp1086;
struct Cyc_Absyn_Stmt* _temp1088; struct Cyc_Absyn_Stmt* _temp1090; struct Cyc_Absyn_Vardecl*
_temp1092; struct Cyc_Absyn_Tvar* _temp1094; struct Cyc_Absyn_Stmt* _temp1096;
struct Cyc_Absyn_Decl* _temp1098; struct Cyc_List_List* _temp1100; struct Cyc_Absyn_Exp*
_temp1102; struct Cyc_Absyn_Stmt* _temp1104; struct Cyc_Absyn_Stmt* _temp1106;
_LL920: if( _temp918 ==( void*) Cyc_Absyn_Skip_s){ goto _LL921;} else{ goto
_LL922;} _LL922: if(( unsigned int) _temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Exp_s:
0){ _LL971: _temp970=(( struct Cyc_Absyn_Exp_s_struct*) _temp918)->f1; goto
_LL923;} else{ goto _LL924;} _LL924: if(( unsigned int) _temp918 > 1u?*(( int*)
_temp918) == Cyc_Absyn_Seq_s: 0){ _LL975: _temp974=(( struct Cyc_Absyn_Seq_s_struct*)
_temp918)->f1; goto _LL973; _LL973: _temp972=(( struct Cyc_Absyn_Seq_s_struct*)
_temp918)->f2; goto _LL925;} else{ goto _LL926;} _LL926: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Return_s: 0){ _LL977: _temp976=((
struct Cyc_Absyn_Return_s_struct*) _temp918)->f1; if( _temp976 == 0){ goto
_LL927;} else{ goto _LL928;}} else{ goto _LL928;} _LL928: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Return_s: 0){ _LL979: _temp978=((
struct Cyc_Absyn_Return_s_struct*) _temp918)->f1; goto _LL929;} else{ goto
_LL930;} _LL930: if(( unsigned int) _temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_IfThenElse_s:
0){ _LL985: _temp984=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp918)->f1;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp918)->f2;
goto _LL981; _LL981: _temp980=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp918)->f3;
goto _LL931;} else{ goto _LL932;} _LL932: if(( unsigned int) _temp918 > 1u?*((
int*) _temp918) == Cyc_Absyn_Do_s: 0){ _LL993: _temp992=(( struct Cyc_Absyn_Do_s_struct*)
_temp918)->f1; goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_Do_s_struct*)
_temp918)->f2; _LL991: _temp990= _temp986.f1; goto _LL989; _LL989: _temp988=
_temp986.f2; goto _LL933;} else{ goto _LL934;} _LL934: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_While_s: 0){ _LL997: _temp996=((
struct Cyc_Absyn_While_s_struct*) _temp918)->f1; _LL1001: _temp1000= _temp996.f1;
goto _LL999; _LL999: _temp998= _temp996.f2; goto _LL995; _LL995: _temp994=((
struct Cyc_Absyn_While_s_struct*) _temp918)->f2; goto _LL935;} else{ goto _LL936;}
_LL936: if(( unsigned int) _temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Break_s:
0){ _LL1003: _temp1002=(( struct Cyc_Absyn_Break_s_struct*) _temp918)->f1; goto
_LL937;} else{ goto _LL938;} _LL938: if(( unsigned int) _temp918 > 1u?*(( int*)
_temp918) == Cyc_Absyn_Continue_s: 0){ _LL1005: _temp1004=(( struct Cyc_Absyn_Continue_s_struct*)
_temp918)->f1; goto _LL939;} else{ goto _LL940;} _LL940: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Goto_s: 0){ _LL1009: _temp1008=((
struct Cyc_Absyn_Goto_s_struct*) _temp918)->f1; goto _LL1007; _LL1007: _temp1006=((
struct Cyc_Absyn_Goto_s_struct*) _temp918)->f2; if( _temp1006 == 0){ goto _LL941;}
else{ goto _LL942;}} else{ goto _LL942;} _LL942: if(( unsigned int) _temp918 > 1u?*((
int*) _temp918) == Cyc_Absyn_Goto_s: 0){ _LL1013: _temp1012=(( struct Cyc_Absyn_Goto_s_struct*)
_temp918)->f1; goto _LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_Goto_s_struct*)
_temp918)->f2; goto _LL943;} else{ goto _LL944;} _LL944: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_For_s: 0){ _LL1029: _temp1028=((
struct Cyc_Absyn_For_s_struct*) _temp918)->f1; goto _LL1023; _LL1023: _temp1022=((
struct Cyc_Absyn_For_s_struct*) _temp918)->f2; _LL1027: _temp1026= _temp1022.f1;
goto _LL1025; _LL1025: _temp1024= _temp1022.f2; goto _LL1017; _LL1017: _temp1016=((
struct Cyc_Absyn_For_s_struct*) _temp918)->f3; _LL1021: _temp1020= _temp1016.f1;
goto _LL1019; _LL1019: _temp1018= _temp1016.f2; goto _LL1015; _LL1015: _temp1014=((
struct Cyc_Absyn_For_s_struct*) _temp918)->f4; goto _LL945;} else{ goto _LL946;}
_LL946: if(( unsigned int) _temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Switch_s:
0){ _LL1033: _temp1032=(( struct Cyc_Absyn_Switch_s_struct*) _temp918)->f1; goto
_LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_Switch_s_struct*) _temp918)->f2;
goto _LL947;} else{ goto _LL948;} _LL948: if(( unsigned int) _temp918 > 1u?*((
int*) _temp918) == Cyc_Absyn_Fallthru_s: 0){ _LL1037: _temp1036=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp918)->f1; goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp918)->f2; goto _LL949;} else{ goto _LL950;} _LL950: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Decl_s: 0){ _LL1041: _temp1040=((
struct Cyc_Absyn_Decl_s_struct*) _temp918)->f1; _temp1042=* _temp1040; _LL1046:
_temp1045=( void*) _temp1042.r; if(*(( int*) _temp1045) == Cyc_Absyn_Var_d){
_LL1048: _temp1047=(( struct Cyc_Absyn_Var_d_struct*) _temp1045)->f1; goto
_LL1044;} else{ goto _LL952;} _LL1044: _temp1043= _temp1042.loc; goto _LL1039;
_LL1039: _temp1038=(( struct Cyc_Absyn_Decl_s_struct*) _temp918)->f2; goto
_LL951;} else{ goto _LL952;} _LL952: if(( unsigned int) _temp918 > 1u?*(( int*)
_temp918) == Cyc_Absyn_Decl_s: 0){ _LL1052: _temp1051=(( struct Cyc_Absyn_Decl_s_struct*)
_temp918)->f1; _temp1053=* _temp1051; _LL1057: _temp1056=( void*) _temp1053.r;
if(*(( int*) _temp1056) == Cyc_Absyn_Let_d){ _LL1070: _temp1069=(( struct Cyc_Absyn_Let_d_struct*)
_temp1056)->f1; goto _LL1065; _LL1065: _temp1064=(( struct Cyc_Absyn_Let_d_struct*)
_temp1056)->f2; if( _temp1064 == 0){ goto _LL954;} else{ _temp1066=* _temp1064;
_LL1068: _temp1067=( struct Cyc_List_List*) _temp1066.v; goto _LL1063;} _LL1063:
_temp1062=(( struct Cyc_Absyn_Let_d_struct*) _temp1056)->f3; goto _LL1061;
_LL1061: _temp1060=(( struct Cyc_Absyn_Let_d_struct*) _temp1056)->f4; goto
_LL1059; _LL1059: _temp1058=(( struct Cyc_Absyn_Let_d_struct*) _temp1056)->f5;
goto _LL1055;} else{ goto _LL954;} _LL1055: _temp1054= _temp1053.loc; goto
_LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_Decl_s_struct*) _temp918)->f2;
goto _LL953;} else{ goto _LL954;} _LL954: if(( unsigned int) _temp918 > 1u?*((
int*) _temp918) == Cyc_Absyn_Decl_s: 0){ _LL1074: _temp1073=(( struct Cyc_Absyn_Decl_s_struct*)
_temp918)->f1; _temp1075=* _temp1073; _LL1079: _temp1078=( void*) _temp1075.r;
if(*(( int*) _temp1078) == Cyc_Absyn_Letv_d){ _LL1081: _temp1080=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1078)->f1; goto _LL1077;} else{ goto _LL956;} _LL1077: _temp1076= _temp1075.loc;
goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_Decl_s_struct*) _temp918)->f2;
goto _LL955;} else{ goto _LL956;} _LL956: if(( unsigned int) _temp918 > 1u?*((
int*) _temp918) == Cyc_Absyn_Label_s: 0){ _LL1085: _temp1084=(( struct Cyc_Absyn_Label_s_struct*)
_temp918)->f1; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_Label_s_struct*)
_temp918)->f2; goto _LL957;} else{ goto _LL958;} _LL958: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_TryCatch_s: 0){ _LL1089:
_temp1088=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp918)->f1; goto _LL1087;
_LL1087: _temp1086=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp918)->f2; goto
_LL959;} else{ goto _LL960;} _LL960: if(( unsigned int) _temp918 > 1u?*(( int*)
_temp918) == Cyc_Absyn_Region_s: 0){ _LL1095: _temp1094=(( struct Cyc_Absyn_Region_s_struct*)
_temp918)->f1; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_Region_s_struct*)
_temp918)->f2; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_Region_s_struct*)
_temp918)->f3; goto _LL961;} else{ goto _LL962;} _LL962: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Decl_s: 0){ _LL1099: _temp1098=((
struct Cyc_Absyn_Decl_s_struct*) _temp918)->f1; goto _LL1097; _LL1097: _temp1096=((
struct Cyc_Absyn_Decl_s_struct*) _temp918)->f2; goto _LL963;} else{ goto _LL964;}
_LL964: if(( unsigned int) _temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_SwitchC_s:
0){ _LL1103: _temp1102=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp918)->f1;
goto _LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp918)->f2;
goto _LL965;} else{ goto _LL966;} _LL966: if(( unsigned int) _temp918 > 1u?*((
int*) _temp918) == Cyc_Absyn_Cut_s: 0){ _LL1105: _temp1104=(( struct Cyc_Absyn_Cut_s_struct*)
_temp918)->f1; goto _LL967;} else{ goto _LL968;} _LL968: if(( unsigned int)
_temp918 > 1u?*(( int*) _temp918) == Cyc_Absyn_Splice_s: 0){ _LL1107: _temp1106=((
struct Cyc_Absyn_Splice_s_struct*) _temp918)->f1; goto _LL969;} else{ goto
_LL919;} _LL921: return; _LL923: { void* _temp1108= Cyc_NewControlFlow_abstract_exp_top(
env, _temp970); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1109=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1109->hd=( void*) _temp1108; _temp1109->tl= 0; _temp1109;})); return;}
_LL925: Cyc_NewControlFlow_cf_prepass( env, _temp974); Cyc_NewControlFlow_cf_prepass(
env, _temp972); return; _LL927: return; _LL929: { void* _temp1110= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
_temp978)); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1111=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1111->hd=( void*) _temp1110; _temp1111->tl= 0; _temp1111;})); return;}
_LL931: { void* _temp1114; void* _temp1116; struct _tuple0 _temp1112= Cyc_NewControlFlow_abstract_guard(
env, _temp984); _LL1117: _temp1116= _temp1112.f1; goto _LL1115; _LL1115:
_temp1114= _temp1112.f2; goto _LL1113; _LL1113: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1118=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1118->hd=( void*) _temp1116; _temp1118->tl=({
struct Cyc_List_List* _temp1119=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1119->hd=( void*) _temp1114; _temp1119->tl= 0;
_temp1119;}); _temp1118;})); Cyc_NewControlFlow_cf_prepass( env, _temp982); Cyc_NewControlFlow_cf_prepass(
env, _temp980); return;} _LL933: _temp1000= _temp990; _temp998= _temp988;
_temp994= _temp992; goto _LL935; _LL935: { void* _temp1122; void* _temp1124;
struct _tuple0 _temp1120= Cyc_NewControlFlow_abstract_guard( env, _temp1000);
_LL1125: _temp1124= _temp1120.f1; goto _LL1123; _LL1123: _temp1122= _temp1120.f2;
goto _LL1121; _LL1121: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1126=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1126->hd=( void*) _temp1124; _temp1126->tl=({ struct Cyc_List_List*
_temp1127=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1127->hd=( void*) _temp1122; _temp1127->tl= 0; _temp1127;}); _temp1126;}));
Cyc_NewControlFlow_cf_prepass( env, _temp994); return;} _LL937: return; _LL939:
return; _LL941:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1128=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1128[ 0]=({ struct Cyc_Core_Impossible_struct _temp1129; _temp1129.tag= Cyc_Core_Impossible;
_temp1129.f1= _tag_arr("cf_prepass: goto w/o destination", sizeof( unsigned char),
33u); _temp1129;}); _temp1128;})); _LL943: { struct Cyc_Absyn_Stmt* _temp1130=(
Cyc_NewControlFlow_get_stmt_annot( s))->encloser; struct Cyc_Absyn_Stmt*
_temp1131=( Cyc_NewControlFlow_get_stmt_annot(( struct Cyc_Absyn_Stmt*)
_check_null( _temp1010)))->encloser; while( _temp1131 != _temp1130) { struct Cyc_Absyn_Stmt*
_temp1132=( Cyc_NewControlFlow_get_stmt_annot( _temp1130))->encloser; if(
_temp1132 == _temp1130){ Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u)); break;} _temp1130= _temp1132;} return;} _LL945: {
void* _temp1133= Cyc_NewControlFlow_abstract_exp_top( env, _temp1028); void*
_temp1136; void* _temp1138; struct _tuple0 _temp1134= Cyc_NewControlFlow_abstract_guard(
env, _temp1026); _LL1139: _temp1138= _temp1134.f1; goto _LL1137; _LL1137:
_temp1136= _temp1134.f2; goto _LL1135; _LL1135: { void* _temp1140= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1020); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1141->hd=( void*) _temp1133; _temp1141->tl=({ struct Cyc_List_List*
_temp1142=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1142->hd=( void*) _temp1138; _temp1142->tl=({ struct Cyc_List_List*
_temp1143=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1143->hd=( void*) _temp1136; _temp1143->tl=({ struct Cyc_List_List*
_temp1144=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1144->hd=( void*) _temp1140; _temp1144->tl= 0; _temp1144;}); _temp1143;});
_temp1142;}); _temp1141;})); Cyc_NewControlFlow_cf_prepass( env, _temp1014);
return;}} _LL947: { void* _temp1145= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp1032); struct Cyc_List_List* _temp1146= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1030); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1147=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1147->hd=( void*) _temp1145; _temp1147->tl= _temp1146; _temp1147;}));
return;} _LL949: { void* _temp1150; void* _temp1152; struct Cyc_NewControlFlow_AbsSynOne
_temp1148= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1036, 0)); _LL1153: _temp1152=( void*) _temp1148.inner_exp; goto
_LL1151; _LL1151: _temp1150=( void*) _temp1148.assigns; goto _LL1149; _LL1149: {
void* _temp1154= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1152, _temp1150); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1155=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1155->hd=( void*) _temp1154; _temp1155->tl= 0; _temp1155;})); return;}}
_LL951: Cyc_NewControlFlow_add_var_root( env, _temp1047); if( _temp1047->initializer
!= 0){ struct Cyc_NewControlFlow_AbsEnv _temp1156= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp1159=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1159->hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp1047); _temp1159->tl= 0; _temp1159;})); void* _temp1157= Cyc_NewControlFlow_abstract_exp_top(
_temp1156,( struct Cyc_Absyn_Exp*) _check_null( _temp1047->initializer)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1158=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1158->hd=( void*) _temp1157; _temp1158->tl= 0;
_temp1158;}));} Cyc_NewControlFlow_cf_prepass( env, _temp1038); return; _LL953:
for( 0; _temp1067 != 0; _temp1067=(( struct Cyc_List_List*) _check_null(
_temp1067))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1067))->hd);}{ void* _temp1160= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1060); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1161=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1161->hd=( void*) _temp1160; _temp1161->tl= 0;
_temp1161;})); Cyc_NewControlFlow_cf_prepass( env, _temp1049); return;} _LL955:
for( 0; _temp1080 != 0; _temp1080=(( struct Cyc_List_List*) _check_null(
_temp1080))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1080))->hd);} Cyc_NewControlFlow_cf_prepass(
env, _temp1071); return; _LL957: Cyc_NewControlFlow_cf_prepass( env, _temp1082);
return; _LL959: Cyc_NewControlFlow_cf_prepass( env, _temp1088);{ struct Cyc_List_List*
_temp1162= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1086); Cyc_NewControlFlow_cf_set_absexps(
s, _temp1162); return;} _LL961: Cyc_NewControlFlow_add_var_root( env, _temp1092);
Cyc_NewControlFlow_cf_prepass( env, _temp1090); return; _LL963: goto _LL965;
_LL965: goto _LL967; _LL967: goto _LL969; _LL969:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp1163=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1163[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1164; _temp1164.tag= Cyc_Core_Impossible;
_temp1164.f1= _tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u);
_temp1164;}); _temp1163;})); _LL919:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1165= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp1165->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
_temp1165->flow)); ++ _temp1165->visited; _temp1165->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:894 annot->visited == iteration_num",
sizeof( unsigned char), 57u)); return _temp1165;} static void Cyc_NewControlFlow_update_flow(
struct Cyc_Absyn_Stmt* s, void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1166= Cyc_NewControlFlow_get_stmt_annot( s); void* _temp1167= Cyc_CfFlowInfo_join_flow(
flow,( void*) _temp1166->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp1167,( void*) _temp1166->flow)){( void*)( _temp1166->flow=( void*)
_temp1167); if( _temp1166->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1168= flow; struct Cyc_Dict_Dict*
_temp1174; _LL1170: if( _temp1168 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1171;} else{ goto _LL1172;} _LL1172: if(( unsigned int) _temp1168 > 1u?*((
int*) _temp1168) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1175: _temp1174=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1168)->f1; goto _LL1173;} else{ goto _LL1169;} _LL1171: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1173: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1176=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1178=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1178[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1179; _temp1179.tag=
Cyc_CfFlowInfo_VarRoot; _temp1179.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1179;}); _temp1178;}); void* _temp1177= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) _temp1176)); _temp1174=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1174,(
void*) _temp1176, _temp1177);} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1180=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1180[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1181; _temp1181.tag=
Cyc_CfFlowInfo_InitsFL; _temp1181.f1= _temp1174; _temp1181;}); _temp1180;});
_LL1169:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1182= flow; struct Cyc_Dict_Dict*
_temp1188; _LL1184: if( _temp1182 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1185;} else{ goto _LL1186;} _LL1186: if(( unsigned int) _temp1182 > 1u?*((
int*) _temp1182) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1189: _temp1188=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1182)->f1; goto _LL1187;} else{ goto _LL1183;} _LL1185: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1187: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1190=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1191=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1191[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1192; _temp1192.tag=
Cyc_CfFlowInfo_VarRoot; _temp1192.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1192;}); _temp1191;}); _temp1188=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1188,(
void*) _temp1190,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots,( void*) _temp1190));} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1193=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1193[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1194; _temp1194.tag=
Cyc_CfFlowInfo_InitsFL; _temp1194.f1= _temp1188; _temp1194;}); _temp1193;});
_LL1183:;} static struct _tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){
if(( void*)(( struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){
void* _temp1195= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp1196; _temp1196.f1=
_temp1195; _temp1196.f2= _temp1195; _temp1196;});} return({ struct _tuple0
_temp1197; _temp1197.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct
Cyc_List_List*) _check_null( aes))->hd, in_flow); _temp1197.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp1197;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp1200; struct Cyc_Position_Segment*
_temp1201; struct Cyc_Absyn_Stmt* _temp1203; struct Cyc_Absyn_Exp* _temp1205;
struct Cyc_Core_Opt* _temp1207; struct Cyc_Absyn_Pat* _temp1209; struct Cyc_Absyn_Switch_clause*
_temp1198=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd; _temp1200=* _temp1198; _LL1210: _temp1209= _temp1200.pattern;
goto _LL1208; _LL1208: _temp1207= _temp1200.pat_vars; goto _LL1206; _LL1206:
_temp1205= _temp1200.where_clause; goto _LL1204; _LL1204: _temp1203= _temp1200.body;
goto _LL1202; _LL1202: _temp1201= _temp1200.loc; goto _LL1199; _LL1199: { void*
_temp1211= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1207))->v); _temp1211= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1211,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1207))->v);{ void* body_outflow; if( _temp1205 != 0){ void* _temp1214; void*
_temp1216; struct _tuple0 _temp1212= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp1205))->loc, aes, _temp1211); _LL1217:
_temp1216= _temp1212.f1; goto _LL1215; _LL1215: _temp1214= _temp1212.f2; goto
_LL1213; _LL1213: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp1214; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1203, _temp1216);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1203, _temp1211);}{ void* _temp1218= body_outflow; _LL1220: if(
_temp1218 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1221;} else{ goto _LL1222;}
_LL1222: goto _LL1223; _LL1221: goto _LL1219; _LL1223: Cyc_Tcutil_terr(
_temp1203->loc, _tag_arr("switch clause may implicitly fallthru", sizeof(
unsigned char), 38u)); goto _LL1219; _LL1219:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1224= Cyc_NewControlFlow_pre_stmt_check(
in_flow, s); in_flow=( void*) _temp1224->flow;{ struct Cyc_List_List* _temp1225=
_temp1224->absexps; void* _temp1226=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1272; struct Cyc_Absyn_Exp* _temp1274; struct Cyc_Absyn_Exp* _temp1276;
struct Cyc_Absyn_Stmt* _temp1278; struct Cyc_Absyn_Stmt* _temp1280; struct Cyc_Absyn_Stmt*
_temp1282; struct Cyc_Absyn_Stmt* _temp1284; struct Cyc_Absyn_Exp* _temp1286;
struct Cyc_Absyn_Stmt* _temp1288; struct _tuple3 _temp1290; struct Cyc_Absyn_Stmt*
_temp1292; struct Cyc_Absyn_Exp* _temp1294; struct _tuple3 _temp1296; struct Cyc_Absyn_Stmt*
_temp1298; struct Cyc_Absyn_Exp* _temp1300; struct Cyc_Absyn_Stmt* _temp1302;
struct Cyc_Absyn_Stmt* _temp1304; struct _tuple3 _temp1306; struct Cyc_Absyn_Stmt*
_temp1308; struct Cyc_Absyn_Exp* _temp1310; struct _tuple3 _temp1312; struct Cyc_Absyn_Stmt*
_temp1314; struct Cyc_Absyn_Exp* _temp1316; struct Cyc_Absyn_Exp* _temp1318;
struct Cyc_Absyn_Stmt* _temp1320; struct Cyc_Absyn_Switch_clause** _temp1322;
struct Cyc_Absyn_Switch_clause* _temp1324; struct Cyc_List_List* _temp1325;
struct Cyc_Absyn_Stmt* _temp1327; struct Cyc_Absyn_Stmt* _temp1329; struct Cyc_Absyn_Stmt*
_temp1331; struct _tagged_arr* _temp1333; struct Cyc_List_List* _temp1335;
struct Cyc_Absyn_Exp* _temp1337; struct Cyc_List_List* _temp1339; struct Cyc_Absyn_Stmt*
_temp1341; struct Cyc_Absyn_Stmt* _temp1343; struct Cyc_Absyn_Decl* _temp1345;
struct Cyc_Absyn_Decl _temp1347; struct Cyc_Position_Segment* _temp1348; void*
_temp1350; struct Cyc_Absyn_Vardecl* _temp1352; struct Cyc_Absyn_Stmt* _temp1354;
struct Cyc_Absyn_Decl* _temp1356; struct Cyc_Absyn_Decl _temp1358; struct Cyc_Position_Segment*
_temp1359; void* _temp1361; int _temp1363; struct Cyc_Absyn_Exp* _temp1365;
struct Cyc_Core_Opt* _temp1367; struct Cyc_Core_Opt* _temp1369; struct Cyc_Core_Opt
_temp1371; struct Cyc_List_List* _temp1372; struct Cyc_Absyn_Pat* _temp1374;
struct Cyc_Absyn_Stmt* _temp1376; struct Cyc_Absyn_Decl* _temp1378; struct Cyc_Absyn_Decl
_temp1380; struct Cyc_Position_Segment* _temp1381; void* _temp1383; struct Cyc_List_List*
_temp1385; struct Cyc_Absyn_Stmt* _temp1387; struct _tagged_arr* _temp1389;
struct Cyc_Absyn_Stmt* _temp1391; struct Cyc_Absyn_Vardecl* _temp1393; struct
Cyc_Absyn_Tvar* _temp1395; _LL1228: if( _temp1226 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1229;} else{ goto _LL1230;} _LL1230: if(( unsigned int) _temp1226 > 1u?*((
int*) _temp1226) == Cyc_Absyn_Return_s: 0){ _LL1273: _temp1272=(( struct Cyc_Absyn_Return_s_struct*)
_temp1226)->f1; if( _temp1272 == 0){ goto _LL1231;} else{ goto _LL1232;}} else{
goto _LL1232;} _LL1232: if(( unsigned int) _temp1226 > 1u?*(( int*) _temp1226)
== Cyc_Absyn_Return_s: 0){ _LL1275: _temp1274=(( struct Cyc_Absyn_Return_s_struct*)
_temp1226)->f1; goto _LL1233;} else{ goto _LL1234;} _LL1234: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Exp_s: 0){ _LL1277: _temp1276=((
struct Cyc_Absyn_Exp_s_struct*) _temp1226)->f1; goto _LL1235;} else{ goto
_LL1236;} _LL1236: if(( unsigned int) _temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Seq_s:
0){ _LL1281: _temp1280=(( struct Cyc_Absyn_Seq_s_struct*) _temp1226)->f1; goto
_LL1279; _LL1279: _temp1278=(( struct Cyc_Absyn_Seq_s_struct*) _temp1226)->f2;
goto _LL1237;} else{ goto _LL1238;} _LL1238: if(( unsigned int) _temp1226 > 1u?*((
int*) _temp1226) == Cyc_Absyn_IfThenElse_s: 0){ _LL1287: _temp1286=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1226)->f1; goto _LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1226)->f2; goto _LL1283; _LL1283: _temp1282=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1226)->f3; goto _LL1239;} else{ goto _LL1240;} _LL1240: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_While_s: 0){ _LL1291: _temp1290=((
struct Cyc_Absyn_While_s_struct*) _temp1226)->f1; _LL1295: _temp1294= _temp1290.f1;
goto _LL1293; _LL1293: _temp1292= _temp1290.f2; goto _LL1289; _LL1289: _temp1288=((
struct Cyc_Absyn_While_s_struct*) _temp1226)->f2; goto _LL1241;} else{ goto
_LL1242;} _LL1242: if(( unsigned int) _temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Do_s:
0){ _LL1303: _temp1302=(( struct Cyc_Absyn_Do_s_struct*) _temp1226)->f1; goto
_LL1297; _LL1297: _temp1296=(( struct Cyc_Absyn_Do_s_struct*) _temp1226)->f2;
_LL1301: _temp1300= _temp1296.f1; goto _LL1299; _LL1299: _temp1298= _temp1296.f2;
goto _LL1243;} else{ goto _LL1244;} _LL1244: if(( unsigned int) _temp1226 > 1u?*((
int*) _temp1226) == Cyc_Absyn_For_s: 0){ _LL1319: _temp1318=(( struct Cyc_Absyn_For_s_struct*)
_temp1226)->f1; goto _LL1313; _LL1313: _temp1312=(( struct Cyc_Absyn_For_s_struct*)
_temp1226)->f2; _LL1317: _temp1316= _temp1312.f1; goto _LL1315; _LL1315:
_temp1314= _temp1312.f2; goto _LL1307; _LL1307: _temp1306=(( struct Cyc_Absyn_For_s_struct*)
_temp1226)->f3; _LL1311: _temp1310= _temp1306.f1; goto _LL1309; _LL1309:
_temp1308= _temp1306.f2; goto _LL1305; _LL1305: _temp1304=(( struct Cyc_Absyn_For_s_struct*)
_temp1226)->f4; goto _LL1245;} else{ goto _LL1246;} _LL1246: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Break_s: 0){ _LL1321: _temp1320=((
struct Cyc_Absyn_Break_s_struct*) _temp1226)->f1; if( _temp1320 == 0){ goto
_LL1247;} else{ goto _LL1248;}} else{ goto _LL1248;} _LL1248: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Fallthru_s: 0){ _LL1326:
_temp1325=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1226)->f1; goto _LL1323;
_LL1323: _temp1322=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1226)->f2; if(
_temp1322 == 0){ goto _LL1250;} else{ _temp1324=* _temp1322; goto _LL1249;}}
else{ goto _LL1250;} _LL1250: if(( unsigned int) _temp1226 > 1u?*(( int*)
_temp1226) == Cyc_Absyn_Break_s: 0){ _LL1328: _temp1327=(( struct Cyc_Absyn_Break_s_struct*)
_temp1226)->f1; goto _LL1251;} else{ goto _LL1252;} _LL1252: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Continue_s: 0){ _LL1330:
_temp1329=(( struct Cyc_Absyn_Continue_s_struct*) _temp1226)->f1; goto _LL1253;}
else{ goto _LL1254;} _LL1254: if(( unsigned int) _temp1226 > 1u?*(( int*)
_temp1226) == Cyc_Absyn_Goto_s: 0){ _LL1334: _temp1333=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1226)->f1; goto _LL1332; _LL1332: _temp1331=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1226)->f2; goto _LL1255;} else{ goto _LL1256;} _LL1256: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Switch_s: 0){ _LL1338:
_temp1337=(( struct Cyc_Absyn_Switch_s_struct*) _temp1226)->f1; goto _LL1336;
_LL1336: _temp1335=(( struct Cyc_Absyn_Switch_s_struct*) _temp1226)->f2; goto
_LL1257;} else{ goto _LL1258;} _LL1258: if(( unsigned int) _temp1226 > 1u?*((
int*) _temp1226) == Cyc_Absyn_TryCatch_s: 0){ _LL1342: _temp1341=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1226)->f1; goto _LL1340; _LL1340: _temp1339=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1226)->f2; goto _LL1259;} else{ goto _LL1260;} _LL1260: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Decl_s: 0){ _LL1346: _temp1345=((
struct Cyc_Absyn_Decl_s_struct*) _temp1226)->f1; _temp1347=* _temp1345; _LL1351:
_temp1350=( void*) _temp1347.r; if(*(( int*) _temp1350) == Cyc_Absyn_Var_d){
_LL1353: _temp1352=(( struct Cyc_Absyn_Var_d_struct*) _temp1350)->f1; goto
_LL1349;} else{ goto _LL1262;} _LL1349: _temp1348= _temp1347.loc; goto _LL1344;
_LL1344: _temp1343=(( struct Cyc_Absyn_Decl_s_struct*) _temp1226)->f2; goto
_LL1261;} else{ goto _LL1262;} _LL1262: if(( unsigned int) _temp1226 > 1u?*((
int*) _temp1226) == Cyc_Absyn_Decl_s: 0){ _LL1357: _temp1356=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1226)->f1; _temp1358=* _temp1356; _LL1362: _temp1361=( void*) _temp1358.r;
if(*(( int*) _temp1361) == Cyc_Absyn_Let_d){ _LL1375: _temp1374=(( struct Cyc_Absyn_Let_d_struct*)
_temp1361)->f1; goto _LL1370; _LL1370: _temp1369=(( struct Cyc_Absyn_Let_d_struct*)
_temp1361)->f2; if( _temp1369 == 0){ goto _LL1264;} else{ _temp1371=* _temp1369;
_LL1373: _temp1372=( struct Cyc_List_List*) _temp1371.v; goto _LL1368;} _LL1368:
_temp1367=(( struct Cyc_Absyn_Let_d_struct*) _temp1361)->f3; goto _LL1366;
_LL1366: _temp1365=(( struct Cyc_Absyn_Let_d_struct*) _temp1361)->f4; goto
_LL1364; _LL1364: _temp1363=(( struct Cyc_Absyn_Let_d_struct*) _temp1361)->f5;
goto _LL1360;} else{ goto _LL1264;} _LL1360: _temp1359= _temp1358.loc; goto
_LL1355; _LL1355: _temp1354=(( struct Cyc_Absyn_Decl_s_struct*) _temp1226)->f2;
goto _LL1263;} else{ goto _LL1264;} _LL1264: if(( unsigned int) _temp1226 > 1u?*((
int*) _temp1226) == Cyc_Absyn_Decl_s: 0){ _LL1379: _temp1378=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1226)->f1; _temp1380=* _temp1378; _LL1384: _temp1383=( void*) _temp1380.r;
if(*(( int*) _temp1383) == Cyc_Absyn_Letv_d){ _LL1386: _temp1385=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1383)->f1; goto _LL1382;} else{ goto _LL1266;} _LL1382: _temp1381=
_temp1380.loc; goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1226)->f2; goto _LL1265;} else{ goto _LL1266;} _LL1266: if(( unsigned int)
_temp1226 > 1u?*(( int*) _temp1226) == Cyc_Absyn_Label_s: 0){ _LL1390: _temp1389=((
struct Cyc_Absyn_Label_s_struct*) _temp1226)->f1; goto _LL1388; _LL1388:
_temp1387=(( struct Cyc_Absyn_Label_s_struct*) _temp1226)->f2; goto _LL1267;}
else{ goto _LL1268;} _LL1268: if(( unsigned int) _temp1226 > 1u?*(( int*)
_temp1226) == Cyc_Absyn_Region_s: 0){ _LL1396: _temp1395=(( struct Cyc_Absyn_Region_s_struct*)
_temp1226)->f1; goto _LL1394; _LL1394: _temp1393=(( struct Cyc_Absyn_Region_s_struct*)
_temp1226)->f2; goto _LL1392; _LL1392: _temp1391=(( struct Cyc_Absyn_Region_s_struct*)
_temp1226)->f3; goto _LL1269;} else{ goto _LL1270;} _LL1270: goto _LL1271;
_LL1229: return in_flow; _LL1231: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1233: Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*) _check_null(
_temp1274))->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1225))->hd,
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1235: return Cyc_CfAbsexp_eval_absexp(
env, _temp1276->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1225))->hd,
in_flow); _LL1237: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1278,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1280, in_flow)); _LL1239: { void*
_temp1399; void* _temp1401; struct _tuple0 _temp1397= Cyc_NewControlFlow_cf_evalguard(
env, _temp1286->loc, _temp1225, in_flow); _LL1402: _temp1401= _temp1397.f1; goto
_LL1400; _LL1400: _temp1399= _temp1397.f2; goto _LL1398; _LL1398: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1284, _temp1401), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1282, _temp1399));} _LL1241: { void* _temp1403=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1292))->flow; void* _temp1406; void* _temp1408; struct _tuple0
_temp1404= Cyc_NewControlFlow_cf_evalguard( env, _temp1294->loc, _temp1225,
_temp1403); _LL1409: _temp1408= _temp1404.f1; goto _LL1407; _LL1407: _temp1406=
_temp1404.f2; goto _LL1405; _LL1405: { void* _temp1410= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1288, _temp1408); Cyc_NewControlFlow_update_flow( _temp1292, _temp1410);
return _temp1406;}} _LL1243: { void* _temp1411= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1302, in_flow); void* _temp1412=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1411, _temp1298))->flow; void* _temp1415; void* _temp1417; struct _tuple0
_temp1413= Cyc_NewControlFlow_cf_evalguard( env, _temp1300->loc, _temp1225,
_temp1412); _LL1418: _temp1417= _temp1413.f1; goto _LL1416; _LL1416: _temp1415=
_temp1413.f2; goto _LL1414; _LL1414: Cyc_NewControlFlow_update_flow( _temp1302,
_temp1417); return _temp1415;} _LL1245: { void* _temp1419= Cyc_CfAbsexp_eval_absexp(
env, _temp1318->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1225))->hd,
in_flow); void* _temp1420=( void*)( Cyc_NewControlFlow_pre_stmt_check( _temp1419,
_temp1314))->flow; void* _temp1423; void* _temp1425; struct _tuple0 _temp1421=
Cyc_NewControlFlow_cf_evalguard( env, _temp1316->loc,(( struct Cyc_List_List*)
_check_null( _temp1225))->tl, _temp1420); _LL1426: _temp1425= _temp1421.f1; goto
_LL1424; _LL1424: _temp1423= _temp1421.f2; goto _LL1422; _LL1422: { void*
_temp1427= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1304, _temp1425); void*
_temp1428=( void*)( Cyc_NewControlFlow_pre_stmt_check( _temp1427, _temp1308))->flow;
void* _temp1429= Cyc_CfAbsexp_eval_absexp( env, _temp1310->loc,( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1225))->tl))->tl))->tl))->hd,
_temp1428); Cyc_NewControlFlow_update_flow( _temp1314, _temp1429); return
_temp1423;}} _LL1247: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1249: in_flow=
Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp1225))->hd, in_flow);{ struct Cyc_List_List* _temp1430=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1324->pat_vars))->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1430); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, _temp1430); _temp1327=(
struct Cyc_Absyn_Stmt*) _temp1324->body; goto _LL1251;} _LL1251: _temp1329=
_temp1327; goto _LL1253; _LL1253: _temp1331= _temp1329; goto _LL1255; _LL1255:
Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*) _check_null( _temp1331),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1257: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1337->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1225))->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1335,(( struct
Cyc_List_List*) _check_null( _temp1225))->tl, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1259: { struct Cyc_NewControlFlow_AnalEnv _temp1433; void* _temp1434; int
_temp1436; struct Cyc_Dict_Dict* _temp1438; struct Cyc_NewControlFlow_AnalEnv*
_temp1431= env; _temp1433=* _temp1431; _LL1439: _temp1438= _temp1433.roots; goto
_LL1437; _LL1437: _temp1436= _temp1433.in_try; goto _LL1435; _LL1435: _temp1434=(
void*) _temp1433.tryflow; goto _LL1432; _LL1432: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* _temp1440= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1341, in_flow); void* _temp1441=( void*) env->tryflow; env->in_try=
_temp1436;( void*)( env->tryflow=( void*) _temp1434); Cyc_NewControlFlow_update_tryflow(
env, _temp1441); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1339,
_temp1225, _temp1441); return _temp1440;}} _LL1261: { struct Cyc_List_List
_temp1442=({ struct Cyc_List_List _temp1445; _temp1445.hd=( void*) _temp1352;
_temp1445.tl= 0; _temp1445;}); void* _temp1443= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)& _temp1442); struct Cyc_Absyn_Exp*
_temp1444= _temp1352->initializer; if( _temp1444 != 0){ _temp1443= Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1444))->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp1225))->hd, _temp1443);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1343, _temp1443);} _LL1263: { void* _temp1446= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1372); _temp1446= Cyc_CfAbsexp_eval_absexp( env, _temp1365->loc,(
void*)(( struct Cyc_List_List*) _check_null( _temp1225))->hd, _temp1446);
_temp1446= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1446, _temp1372);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1354, _temp1446);} _LL1265: {
void* _temp1447= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1385);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1376, _temp1447);} _LL1267:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1387, in_flow); _LL1269: {
struct Cyc_List_List* _temp1448=({ struct Cyc_List_List* _temp1450=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1450->hd=( void*) _temp1393;
_temp1450->tl= 0; _temp1450;}); void* _temp1449= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1448); _temp1449= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp1449, _temp1448); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1391,
_temp1449);} _LL1271:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1451=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1451[ 0]=({ struct Cyc_Core_Impossible_struct _temp1452; _temp1452.tag= Cyc_Core_Impossible;
_temp1452.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp1452;}); _temp1451;})); _LL1227:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1453=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1477; struct Cyc_Absyn_Stmt*
_temp1479; struct Cyc_Absyn_Stmt* _temp1481; struct Cyc_Absyn_Stmt* _temp1483;
struct Cyc_Absyn_Exp* _temp1485; struct Cyc_Absyn_Stmt* _temp1487; struct
_tuple3 _temp1489; struct Cyc_Absyn_Stmt* _temp1491; struct Cyc_Absyn_Exp*
_temp1493; struct _tuple3 _temp1495; struct Cyc_Absyn_Stmt* _temp1497; struct
Cyc_Absyn_Exp* _temp1499; struct Cyc_Absyn_Exp* _temp1501; struct Cyc_Absyn_Stmt*
_temp1503; struct _tuple3 _temp1505; struct Cyc_Absyn_Stmt* _temp1507; struct
Cyc_Absyn_Exp* _temp1509; struct _tuple3 _temp1511; struct Cyc_Absyn_Stmt*
_temp1513; struct Cyc_Absyn_Exp* _temp1515; struct Cyc_Absyn_Stmt* _temp1517;
struct Cyc_Absyn_Stmt* _temp1519; struct Cyc_Absyn_Vardecl* _temp1521; struct
Cyc_Absyn_Tvar* _temp1523; struct Cyc_Absyn_Stmt* _temp1525; struct _tagged_arr*
_temp1527; struct Cyc_Absyn_Stmt* _temp1529; struct Cyc_Absyn_Decl* _temp1531;
struct Cyc_List_List* _temp1533; struct Cyc_Absyn_Stmt* _temp1535; struct Cyc_List_List*
_temp1537; struct Cyc_Absyn_Exp* _temp1539; _LL1455: if(( unsigned int)
_temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_Seq_s: 0){ _LL1480: _temp1479=((
struct Cyc_Absyn_Seq_s_struct*) _temp1453)->f1; goto _LL1478; _LL1478: _temp1477=((
struct Cyc_Absyn_Seq_s_struct*) _temp1453)->f2; goto _LL1456;} else{ goto
_LL1457;} _LL1457: if(( unsigned int) _temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_IfThenElse_s:
0){ _LL1486: _temp1485=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1453)->f1;
goto _LL1484; _LL1484: _temp1483=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1453)->f2; goto _LL1482; _LL1482: _temp1481=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1453)->f3; goto _LL1458;} else{ goto _LL1459;} _LL1459: if(( unsigned int)
_temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_For_s: 0){ _LL1502: _temp1501=((
struct Cyc_Absyn_For_s_struct*) _temp1453)->f1; goto _LL1496; _LL1496: _temp1495=((
struct Cyc_Absyn_For_s_struct*) _temp1453)->f2; _LL1500: _temp1499= _temp1495.f1;
goto _LL1498; _LL1498: _temp1497= _temp1495.f2; goto _LL1490; _LL1490: _temp1489=((
struct Cyc_Absyn_For_s_struct*) _temp1453)->f3; _LL1494: _temp1493= _temp1489.f1;
goto _LL1492; _LL1492: _temp1491= _temp1489.f2; goto _LL1488; _LL1488: _temp1487=((
struct Cyc_Absyn_For_s_struct*) _temp1453)->f4; goto _LL1460;} else{ goto
_LL1461;} _LL1461: if(( unsigned int) _temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_While_s:
0){ _LL1506: _temp1505=(( struct Cyc_Absyn_While_s_struct*) _temp1453)->f1;
_LL1510: _temp1509= _temp1505.f1; goto _LL1508; _LL1508: _temp1507= _temp1505.f2;
goto _LL1504; _LL1504: _temp1503=(( struct Cyc_Absyn_While_s_struct*) _temp1453)->f2;
goto _LL1462;} else{ goto _LL1463;} _LL1463: if(( unsigned int) _temp1453 > 1u?*((
int*) _temp1453) == Cyc_Absyn_Do_s: 0){ _LL1518: _temp1517=(( struct Cyc_Absyn_Do_s_struct*)
_temp1453)->f1; goto _LL1512; _LL1512: _temp1511=(( struct Cyc_Absyn_Do_s_struct*)
_temp1453)->f2; _LL1516: _temp1515= _temp1511.f1; goto _LL1514; _LL1514:
_temp1513= _temp1511.f2; goto _LL1464;} else{ goto _LL1465;} _LL1465: if((
unsigned int) _temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_Region_s: 0){
_LL1524: _temp1523=(( struct Cyc_Absyn_Region_s_struct*) _temp1453)->f1; goto
_LL1522; _LL1522: _temp1521=(( struct Cyc_Absyn_Region_s_struct*) _temp1453)->f2;
goto _LL1520; _LL1520: _temp1519=(( struct Cyc_Absyn_Region_s_struct*) _temp1453)->f3;
goto _LL1466;} else{ goto _LL1467;} _LL1467: if(( unsigned int) _temp1453 > 1u?*((
int*) _temp1453) == Cyc_Absyn_Label_s: 0){ _LL1528: _temp1527=(( struct Cyc_Absyn_Label_s_struct*)
_temp1453)->f1; goto _LL1526; _LL1526: _temp1525=(( struct Cyc_Absyn_Label_s_struct*)
_temp1453)->f2; goto _LL1468;} else{ goto _LL1469;} _LL1469: if(( unsigned int)
_temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_Decl_s: 0){ _LL1532: _temp1531=((
struct Cyc_Absyn_Decl_s_struct*) _temp1453)->f1; goto _LL1530; _LL1530:
_temp1529=(( struct Cyc_Absyn_Decl_s_struct*) _temp1453)->f2; goto _LL1470;}
else{ goto _LL1471;} _LL1471: if(( unsigned int) _temp1453 > 1u?*(( int*)
_temp1453) == Cyc_Absyn_TryCatch_s: 0){ _LL1536: _temp1535=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1453)->f1; goto _LL1534; _LL1534: _temp1533=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1453)->f2; goto _LL1472;} else{ goto _LL1473;} _LL1473: if(( unsigned int)
_temp1453 > 1u?*(( int*) _temp1453) == Cyc_Absyn_Switch_s: 0){ _LL1540:
_temp1539=(( struct Cyc_Absyn_Switch_s_struct*) _temp1453)->f1; goto _LL1538;
_LL1538: _temp1537=(( struct Cyc_Absyn_Switch_s_struct*) _temp1453)->f2; goto
_LL1474;} else{ goto _LL1475;} _LL1475: goto _LL1476; _LL1456: _temp1483=
_temp1479; _temp1481= _temp1477; goto _LL1458; _LL1458: Cyc_NewControlFlow_cf_postpass(
_temp1483); Cyc_NewControlFlow_cf_postpass( _temp1481); return; _LL1460: Cyc_NewControlFlow_cf_postpass(
_temp1491); _temp1507= _temp1487; _temp1503= _temp1497; goto _LL1462; _LL1462:
_temp1517= _temp1503; _temp1513= _temp1507; goto _LL1464; _LL1464: Cyc_NewControlFlow_cf_postpass(
_temp1513); _temp1519= _temp1517; goto _LL1466; _LL1466: _temp1525= _temp1519;
goto _LL1468; _LL1468: _temp1529= _temp1525; goto _LL1470; _LL1470: Cyc_NewControlFlow_cf_postpass(
_temp1529); return; _LL1472: Cyc_NewControlFlow_cf_postpass( _temp1535);
_temp1537= _temp1533; goto _LL1474; _LL1474: for( 0; _temp1537 != 0; _temp1537=((
struct Cyc_List_List*) _check_null( _temp1537))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1537))->hd)->body);}
return; _LL1476: return; _LL1454:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1565; _temp1565.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1566=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1566->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1566->comprehension_vars= 0; _temp1566;}); _temp1565.lhsides= 0; _temp1565.access_path=
0; _temp1565;}); struct Cyc_List_List* _temp1541=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v;{ struct Cyc_List_List*
vds= _temp1541; for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
Cyc_NewControlFlow_add_var_root( absenv,( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_List_List*) _check_null( vds))->hd);}} Cyc_NewControlFlow_cf_prepass( absenv,
fd->body);{ struct Cyc_List_List* _temp1544; struct Cyc_List_List* _temp1546;
struct Cyc_NewControlFlow_SharedAbsEnv* _temp1548; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1550; struct Cyc_List_List* _temp1551; struct Cyc_Dict_Dict* _temp1553;
struct Cyc_NewControlFlow_AbsEnv _temp1542= absenv; _LL1549: _temp1548=
_temp1542.shared; _temp1550=* _temp1548; _LL1554: _temp1553= _temp1550.all_roots;
goto _LL1552; _LL1552: _temp1551= _temp1550.comprehension_vars; goto _LL1547;
_LL1547: _temp1546= _temp1542.lhsides; goto _LL1545; _LL1545: _temp1544=
_temp1542.access_path; goto _LL1543; _LL1543: { struct Cyc_NewControlFlow_AnalEnv*
_temp1555=({ struct Cyc_NewControlFlow_AnalEnv* _temp1564=( struct Cyc_NewControlFlow_AnalEnv*)
GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp1564->roots=
_temp1553; _temp1564->in_try= 0; _temp1564->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp1564;}); void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1562=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1562[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1563; _temp1563.tag=
Cyc_CfFlowInfo_InitsFL; _temp1563.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1563;});
_temp1562;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp1555, in_flow,
_temp1541); in_flow= Cyc_NewControlFlow_add_vardecls_flow( _temp1555, in_flow,
_temp1551);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp1555, fd->body, in_flow);}{ void* _temp1556= out_flow; _LL1558: if(
_temp1556 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1559;} else{ goto _LL1560;}
_LL1560: goto _LL1561; _LL1559: goto _LL1557; _LL1561: if(( void*) fd->ret_type
!=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may complete without returning a value",
sizeof( unsigned char), 48u));} goto _LL1557; _LL1557:;} Cyc_NewControlFlow_cf_postpass(
fd->body);}}}} void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds){ for(
0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp1567=(
void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd)->r;
struct Cyc_Absyn_Fndecl* _temp1577; struct Cyc_List_List* _temp1579; struct
_tuple1* _temp1581; struct Cyc_List_List* _temp1583; struct _tagged_arr*
_temp1585; _LL1569: if(*(( int*) _temp1567) == Cyc_Absyn_Fn_d){ _LL1578:
_temp1577=(( struct Cyc_Absyn_Fn_d_struct*) _temp1567)->f1; goto _LL1570;} else{
goto _LL1571;} _LL1571: if(*(( int*) _temp1567) == Cyc_Absyn_Using_d){ _LL1582:
_temp1581=(( struct Cyc_Absyn_Using_d_struct*) _temp1567)->f1; goto _LL1580;
_LL1580: _temp1579=(( struct Cyc_Absyn_Using_d_struct*) _temp1567)->f2; goto
_LL1572;} else{ goto _LL1573;} _LL1573: if(*(( int*) _temp1567) == Cyc_Absyn_Namespace_d){
_LL1586: _temp1585=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1567)->f1; goto
_LL1584; _LL1584: _temp1583=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1567)->f2;
goto _LL1574;} else{ goto _LL1575;} _LL1575: goto _LL1576; _LL1570: Cyc_NewControlFlow_cf_check_fun(
_temp1577); goto _LL1568; _LL1572: _temp1583= _temp1579; goto _LL1574; _LL1574:
Cyc_NewControlFlow_cf_check( _temp1583); goto _LL1568; _LL1576: goto _LL1568;
_LL1568:;}}