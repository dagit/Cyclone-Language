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
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
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
_tag_arr("new_control_flow.cyc:238 path->tl == null", sizeof( unsigned char), 42u));
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
_tag_arr("new_control_flow.cyc:303 env.access_path==null", sizeof( unsigned char),
47u)); return({ struct Cyc_NewControlFlow_AbsSyn _temp406; _temp406.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp408; _temp408.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp408.assigns=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp408;}); _temp406.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp407; _temp407.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp407.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp407;}); _temp406.lvalues= 0; _temp406;}); _LL139: env.access_path == 0? 0:((
int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:308 env.access_path==null",
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
0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:432 Evexp::eval_const_uint_exp(e2) == 0",
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
0); _LL179: if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp324->topt))->v)){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp563=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp563->hd=( void*) _temp324; _temp563->tl= 0;
_temp563;}), Cyc_NewControlFlow_use_it_list());}{ struct Cyc_List_List* _temp564=
env.access_path; struct Cyc_List_List _temp570; struct Cyc_List_List* _temp571;
struct Cyc_Absyn_Exp* _temp573; struct Cyc_Absyn_Exp _temp575; struct Cyc_Position_Segment*
_temp576; void* _temp578; struct Cyc_Absyn_Exp* _temp580; struct Cyc_Core_Opt*
_temp582; _LL566: if( _temp564 == 0){ goto _LL568;} else{ _temp570=* _temp564;
_LL574: _temp573=( struct Cyc_Absyn_Exp*) _temp570.hd; _temp575=* _temp573;
_LL583: _temp582= _temp575.topt; goto _LL579; _LL579: _temp578=( void*) _temp575.r;
if(*(( int*) _temp578) == Cyc_Absyn_Address_e){ _LL581: _temp580=(( struct Cyc_Absyn_Address_e_struct*)
_temp578)->f1; goto _LL577;} else{ goto _LL568;} _LL577: _temp576= _temp575.loc;
goto _LL572; _LL572: _temp571= _temp570.tl; goto _LL567;} _LL568: goto _LL569;
_LL567: env.access_path= _temp571;{ struct Cyc_NewControlFlow_AbsSyn _temp584=
Cyc_NewControlFlow_abstract_exp( env, _temp324); if( _temp584.lvalues == 0){
_temp584.lvalues= Cyc_NewControlFlow_use_it_list();} return _temp584;} _LL569:
return Cyc_NewControlFlow_descend_path( env, e, _temp324); _LL565:;} _LL181: if(
Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp328->topt))->v)){ return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp585=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp585->hd=( void*) _temp328; _temp585->tl= 0;
_temp585;}), Cyc_NewControlFlow_use_it_list());}{ void* _temp586= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp328->topt))->v); struct Cyc_Absyn_PtrInfo
_temp592; struct Cyc_Absyn_Conref* _temp594; struct Cyc_Absyn_Tqual _temp596;
struct Cyc_Absyn_Conref* _temp598; void* _temp600; void* _temp602; _LL588: if((
unsigned int) _temp586 > 4u?*(( int*) _temp586) == Cyc_Absyn_PointerType: 0){
_LL593: _temp592=(( struct Cyc_Absyn_PointerType_struct*) _temp586)->f1; _LL603:
_temp602=( void*) _temp592.elt_typ; goto _LL601; _LL601: _temp600=( void*)
_temp592.rgn_typ; goto _LL599; _LL599: _temp598= _temp592.nullable; goto _LL597;
_LL597: _temp596= _temp592.tq; goto _LL595; _LL595: _temp594= _temp592.bounds;
goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL589:{ void* _temp604=
Cyc_Tcutil_compress( _temp602); struct Cyc_List_List* _temp616; struct Cyc_Absyn_Structdecl**
_temp618; struct Cyc_List_List* _temp620; struct _tuple1* _temp622; struct Cyc_List_List*
_temp624; struct Cyc_Absyn_Uniondecl** _temp626; struct Cyc_List_List* _temp628;
struct _tuple1* _temp630; _LL606: if(( unsigned int) _temp604 > 4u?*(( int*)
_temp604) == Cyc_Absyn_AnonStructType: 0){ _LL617: _temp616=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp604)->f1; goto _LL607;} else{ goto _LL608;} _LL608: if(( unsigned int)
_temp604 > 4u?*(( int*) _temp604) == Cyc_Absyn_StructType: 0){ _LL623: _temp622=((
struct Cyc_Absyn_StructType_struct*) _temp604)->f1; goto _LL621; _LL621:
_temp620=(( struct Cyc_Absyn_StructType_struct*) _temp604)->f2; goto _LL619;
_LL619: _temp618=(( struct Cyc_Absyn_StructType_struct*) _temp604)->f3; goto
_LL609;} else{ goto _LL610;} _LL610: if(( unsigned int) _temp604 > 4u?*(( int*)
_temp604) == Cyc_Absyn_AnonUnionType: 0){ _LL625: _temp624=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp604)->f1; goto _LL611;} else{ goto _LL612;} _LL612: if(( unsigned int)
_temp604 > 4u?*(( int*) _temp604) == Cyc_Absyn_UnionType: 0){ _LL631: _temp630=((
struct Cyc_Absyn_UnionType_struct*) _temp604)->f1; goto _LL629; _LL629: _temp628=((
struct Cyc_Absyn_UnionType_struct*) _temp604)->f2; goto _LL627; _LL627: _temp626=((
struct Cyc_Absyn_UnionType_struct*) _temp604)->f3; goto _LL613;} else{ goto
_LL614;} _LL614: goto _LL615; _LL607: goto _LL609; _LL609: return Cyc_NewControlFlow_descend_path(
env, e, _temp328); _LL611: goto _LL613; _LL613: { struct Cyc_NewControlFlow_AbsSyn
_temp632= Cyc_NewControlFlow_abstract_exp( env, _temp328); _temp632.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp632;} _LL615: goto _LL605; _LL605:;} goto _LL591; _LL591:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp633=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp633[ 0]=({ struct
Cyc_Core_Impossible_struct _temp634; _temp634.tag= Cyc_Core_Impossible; _temp634.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp634;}); _temp633;})); _LL587:;} _LL183: { void* _temp635= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp332->topt))->v); struct Cyc_List_List*
_temp647; struct Cyc_Absyn_Structdecl** _temp649; struct Cyc_List_List* _temp651;
struct _tuple1* _temp653; struct Cyc_List_List* _temp655; struct Cyc_Absyn_Uniondecl**
_temp657; struct Cyc_List_List* _temp659; struct _tuple1* _temp661; _LL637: if((
unsigned int) _temp635 > 4u?*(( int*) _temp635) == Cyc_Absyn_AnonStructType: 0){
_LL648: _temp647=(( struct Cyc_Absyn_AnonStructType_struct*) _temp635)->f1; goto
_LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp635 > 4u?*(( int*)
_temp635) == Cyc_Absyn_StructType: 0){ _LL654: _temp653=(( struct Cyc_Absyn_StructType_struct*)
_temp635)->f1; goto _LL652; _LL652: _temp651=(( struct Cyc_Absyn_StructType_struct*)
_temp635)->f2; goto _LL650; _LL650: _temp649=(( struct Cyc_Absyn_StructType_struct*)
_temp635)->f3; goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int)
_temp635 > 4u?*(( int*) _temp635) == Cyc_Absyn_AnonUnionType: 0){ _LL656:
_temp655=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp635)->f1; goto _LL642;}
else{ goto _LL643;} _LL643: if(( unsigned int) _temp635 > 4u?*(( int*) _temp635)
== Cyc_Absyn_UnionType: 0){ _LL662: _temp661=(( struct Cyc_Absyn_UnionType_struct*)
_temp635)->f1; goto _LL660; _LL660: _temp659=(( struct Cyc_Absyn_UnionType_struct*)
_temp635)->f2; goto _LL658; _LL658: _temp657=(( struct Cyc_Absyn_UnionType_struct*)
_temp635)->f3; goto _LL644;} else{ goto _LL645;} _LL645: goto _LL646; _LL638:
goto _LL640; _LL640: return Cyc_NewControlFlow_descend_path( env, e, _temp332);
_LL642: goto _LL644; _LL644: { struct Cyc_NewControlFlow_AbsSyn _temp663= Cyc_NewControlFlow_abstract_exp(
env, _temp332); _temp663.lvalues= Cyc_NewControlFlow_use_it_list(); return
_temp663;} _LL646:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp664=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp664[ 0]=({ struct Cyc_Core_Impossible_struct _temp665; _temp665.tag= Cyc_Core_Impossible;
_temp665.f1= _tag_arr("NewControlFlow: bad type in StructMember", sizeof(
unsigned char), 41u); _temp665;}); _temp664;})); _LL636:;} _LL185: env.access_path
== 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:506 env.access_path == null",
sizeof( unsigned char), 49u)); if( _temp338 == 0){ return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _temp344= _temp338; goto _LL187; _LL187: {
struct Cyc_List_List* _temp666= env.access_path; struct Cyc_List_List _temp674;
struct Cyc_List_List* _temp675; struct Cyc_Absyn_Exp* _temp677; struct Cyc_Absyn_Exp
_temp679; struct Cyc_Position_Segment* _temp680; void* _temp682; struct Cyc_Absyn_Exp*
_temp684; struct Cyc_Absyn_Exp* _temp686; struct Cyc_Core_Opt* _temp688; _LL668:
if( _temp666 == 0){ goto _LL669;} else{ goto _LL670;} _LL670: if( _temp666 == 0){
goto _LL672;} else{ _temp674=* _temp666; _LL678: _temp677=( struct Cyc_Absyn_Exp*)
_temp674.hd; _temp679=* _temp677; _LL689: _temp688= _temp679.topt; goto _LL683;
_LL683: _temp682=( void*) _temp679.r; if(*(( int*) _temp682) == Cyc_Absyn_Subscript_e){
_LL687: _temp686=(( struct Cyc_Absyn_Subscript_e_struct*) _temp682)->f1; goto
_LL685; _LL685: _temp684=(( struct Cyc_Absyn_Subscript_e_struct*) _temp682)->f2;
goto _LL681;} else{ goto _LL672;} _LL681: _temp680= _temp679.loc; goto _LL676;
_LL676: _temp675= _temp674.tl; goto _LL671;} _LL672: goto _LL673; _LL669: { void*
_temp690= Cyc_CfAbsexp_mkSkipAE(); void* _temp691= Cyc_CfAbsexp_mkSkipAE();{ int
j= 0; for( 0; _temp344 != 0;( _temp344=(( struct Cyc_List_List*) _check_null(
_temp344))->tl, ++ j)){ struct Cyc_List_List* _temp692= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){
if( Cyc_CfAbsexp_isUnknownOp(( void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){
_temp692=({ struct Cyc_List_List* _temp693=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp693->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp693->tl= _temp692; _temp693;});} else{ _temp692=({ struct Cyc_List_List*
_temp694=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp694->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*)(( struct Cyc_List_List*)
_check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp695=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp695[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp696; _temp696.tag= Cyc_CfFlowInfo_TupleF;
_temp696.f1= j; _temp696;}); _temp695;})); _temp694->tl= _temp692; _temp694;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp697= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp692),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp344))->hd); void* _temp700; void* _temp702; struct Cyc_NewControlFlow_AbsSynOne
_temp698= Cyc_NewControlFlow_meet_absexp( _temp697); _LL703: _temp702=( void*)
_temp698.inner_exp; goto _LL701; _LL701: _temp700=( void*) _temp698.assigns;
goto _LL699; _LL699: _temp690= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp690, _temp702); _temp691= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp691, _temp700);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp690, 0,
_temp691);} _LL671: { int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp684);
struct Cyc_NewControlFlow_AbsEnv _temp704= Cyc_NewControlFlow_change_lhs( env, 0);
_temp704.access_path= 0; env.access_path= _temp675;{ void* _temp705= Cyc_CfAbsexp_mkSkipAE();
void* _temp706= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp344 != 0;(
_temp344=(( struct Cyc_List_List*) _check_null( _temp344))->tl, ++ j)){ if( i ==
j){ void* _temp709; void* _temp711; struct Cyc_NewControlFlow_AbsSynOne _temp707=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp344))->hd)); _LL712:
_temp711=( void*) _temp707.inner_exp; goto _LL710; _LL710: _temp709=( void*)
_temp707.assigns; goto _LL708; _LL708: _temp706= _temp709; _temp705= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp705, _temp711);} else{ void* _temp715; void*
_temp717; struct Cyc_NewControlFlow_AbsSynOne _temp713= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp704,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp344))->hd)); _LL718: _temp717=( void*) _temp713.inner_exp;
goto _LL716; _LL716: _temp715=( void*) _temp713.assigns; goto _LL714; _LL714:
_temp705= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp705,
_temp717);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp705,({ struct Cyc_List_List*
_temp719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp719->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp719->tl= 0; _temp719;}),
_temp706);}} _LL673:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp720=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp720[ 0]=({ struct Cyc_Core_Impossible_struct _temp721; _temp721.tag= Cyc_Core_Impossible;
_temp721.f1= _tag_arr("abstract_exp: unexpected access path to Tuple", sizeof(
unsigned char), 46u); _temp721;}); _temp720;})); _LL667:;} _LL189: _temp352=
_temp346; goto _LL191; _LL191: { struct Cyc_List_List* _temp722= env.access_path;
struct Cyc_List_List _temp730; struct Cyc_List_List* _temp731; struct Cyc_Absyn_Exp*
_temp733; struct Cyc_Absyn_Exp _temp735; struct Cyc_Position_Segment* _temp736;
void* _temp738; struct _tagged_arr* _temp740; struct Cyc_Absyn_Exp* _temp742;
struct Cyc_Core_Opt* _temp744; _LL724: if( _temp722 == 0){ goto _LL725;} else{
goto _LL726;} _LL726: if( _temp722 == 0){ goto _LL728;} else{ _temp730=*
_temp722; _LL734: _temp733=( struct Cyc_Absyn_Exp*) _temp730.hd; _temp735=*
_temp733; _LL745: _temp744= _temp735.topt; goto _LL739; _LL739: _temp738=( void*)
_temp735.r; if(*(( int*) _temp738) == Cyc_Absyn_StructMember_e){ _LL743:
_temp742=(( struct Cyc_Absyn_StructMember_e_struct*) _temp738)->f1; goto _LL741;
_LL741: _temp740=(( struct Cyc_Absyn_StructMember_e_struct*) _temp738)->f2; goto
_LL737;} else{ goto _LL728;} _LL737: _temp736= _temp735.loc; goto _LL732; _LL732:
_temp731= _temp730.tl; goto _LL727;} _LL728: goto _LL729; _LL725: { void*
_temp746= Cyc_CfAbsexp_mkSkipAE(); void* _temp747= Cyc_CfAbsexp_mkSkipAE(); for(
0; _temp352 != 0; _temp352=(( struct Cyc_List_List*) _check_null( _temp352))->tl){
struct Cyc_List_List* _temp748= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls != 0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp748=({ struct Cyc_List_List*
_temp749=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp749->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp749->tl= _temp748;
_temp749;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp750=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp756; struct _tagged_arr*
_temp758; _LL752: if(*(( int*) _temp750) == Cyc_Absyn_ArrayElement){ _LL757:
_temp756=(( struct Cyc_Absyn_ArrayElement_struct*) _temp750)->f1; goto _LL753;}
else{ goto _LL754;} _LL754: if(*(( int*) _temp750) == Cyc_Absyn_FieldName){
_LL759: _temp758=(( struct Cyc_Absyn_FieldName_struct*) _temp750)->f1; goto
_LL755;} else{ goto _LL751;} _LL753:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp760=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp760[ 0]=({ struct Cyc_Core_Impossible_struct _temp761; _temp761.tag= Cyc_Core_Impossible;
_temp761.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp761;}); _temp760;})); _LL755: _temp748=({ struct Cyc_List_List* _temp762=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp762->hd=(
void*) Cyc_CfAbsexp_mkMemberOp(( void*)(( struct Cyc_List_List*) _check_null( ls))->hd,(
void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp763=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp763[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp764; _temp764.tag= Cyc_CfFlowInfo_StructF;
_temp764.f1= _temp758; _temp764;}); _temp763;})); _temp762->tl= _temp748;
_temp762;}); goto _LL751; _LL751:;}}}}{ void** _temp765=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f2)->topt))->v); struct Cyc_NewControlFlow_AbsSyn
_temp766= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp748),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp352))->hd)).f2);
void* _temp769; void* _temp771; struct Cyc_NewControlFlow_AbsSynOne _temp767=
Cyc_NewControlFlow_meet_absexp( _temp766); _LL772: _temp771=( void*) _temp767.inner_exp;
goto _LL770; _LL770: _temp769=( void*) _temp767.assigns; goto _LL768; _LL768:
_temp746= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp746,
_temp771); _temp747= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp747, _temp769);}} return Cyc_NewControlFlow_mkSyn_tf( _temp746, 0, _temp747);}
_LL727: { struct Cyc_NewControlFlow_AbsEnv _temp773= Cyc_NewControlFlow_change_lhs(
env, 0); _temp773.access_path= 0; env.access_path= _temp731;{ void* _temp774=
Cyc_CfAbsexp_mkSkipAE(); void* _temp775= Cyc_CfAbsexp_mkSkipAE(); for( 0;
_temp352 != 0; _temp352=(( struct Cyc_List_List*) _check_null( _temp352))->tl){
int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp776=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp782; struct _tagged_arr*
_temp784; _LL778: if(*(( int*) _temp776) == Cyc_Absyn_ArrayElement){ _LL783:
_temp782=(( struct Cyc_Absyn_ArrayElement_struct*) _temp776)->f1; goto _LL779;}
else{ goto _LL780;} _LL780: if(*(( int*) _temp776) == Cyc_Absyn_FieldName){
_LL785: _temp784=(( struct Cyc_Absyn_FieldName_struct*) _temp776)->f1; goto
_LL781;} else{ goto _LL777;} _LL779:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp786=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp786[ 0]=({ struct Cyc_Core_Impossible_struct _temp787; _temp787.tag= Cyc_Core_Impossible;
_temp787.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp787;}); _temp786;})); _LL781: if( Cyc_String_zstrptrcmp( _temp740, _temp784)
== 0){ used= 1;} goto _LL777; _LL777:;}} if( used){ void* _temp790; void*
_temp792; struct Cyc_NewControlFlow_AbsSynOne _temp788= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp352))->hd)).f2)); _LL793: _temp792=( void*) _temp788.inner_exp;
goto _LL791; _LL791: _temp790=( void*) _temp788.assigns; goto _LL789; _LL789:
_temp775= _temp790; _temp774= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp774, _temp792);} else{ void* _temp796; void* _temp798; struct Cyc_NewControlFlow_AbsSynOne
_temp794= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp773,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp352))->hd)).f2));
_LL799: _temp798=( void*) _temp794.inner_exp; goto _LL797; _LL797: _temp796=(
void*) _temp794.assigns; goto _LL795; _LL795: _temp774= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp774, _temp798);}} return Cyc_NewControlFlow_mkSyn_tf(
_temp774, 0, _temp775);}} _LL729:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp800=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp800[ 0]=({ struct Cyc_Core_Impossible_struct _temp801; _temp801.tag= Cyc_Core_Impossible;
_temp801.f1= _tag_arr("abstract_exp: unexpected access path to Struct", sizeof(
unsigned char), 47u); _temp801;}); _temp800;})); _LL723:;} _LL193: return Cyc_NewControlFlow_use_exp_unordered(
env,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp358), 0); _LL195: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp802=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp802->hd=( void*) _temp362; _temp802->tl= 0;
_temp802;}), 0); _LL197: { struct Cyc_NewControlFlow_AbsSyn _temp803= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp364); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
_temp803)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL199: { struct Cyc_List_List* _temp806; struct Cyc_NewControlFlow_AbsSynOne
_temp808; struct Cyc_NewControlFlow_AbsSynOne _temp810; struct Cyc_NewControlFlow_AbsSyn
_temp804= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp370); _LL811: _temp810= _temp804.when_true; goto _LL809; _LL809: _temp808=
_temp804.when_false; goto _LL807; _LL807: _temp806= _temp804.lvalues; goto
_LL805; _LL805: { struct Cyc_List_List* _temp814; struct Cyc_NewControlFlow_AbsSynOne
_temp816; struct Cyc_NewControlFlow_AbsSynOne _temp818; struct Cyc_NewControlFlow_AbsSyn
_temp812= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp368); _LL819: _temp818= _temp812.when_true; goto _LL817; _LL817: _temp816=
_temp812.when_false; goto _LL815; _LL815: _temp814= _temp812.lvalues; goto
_LL813; _LL813: { struct Cyc_List_List* _temp822; struct Cyc_NewControlFlow_AbsSynOne
_temp824; struct Cyc_NewControlFlow_AbsSynOne _temp826; struct Cyc_NewControlFlow_AbsSyn
_temp820= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp366); _LL827: _temp826= _temp820.when_true; goto _LL825; _LL825: _temp824=
_temp820.when_false; goto _LL823; _LL823: _temp822= _temp820.lvalues; goto
_LL821; _LL821: { void* _temp828= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp810.inner_exp,( void*) _temp810.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp818.inner_exp,(
void*) _temp818.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp808.inner_exp,(
void*) _temp808.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp826.inner_exp,( void*) _temp826.assigns))); void* _temp829= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp810.inner_exp,(
void*) _temp810.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp816.inner_exp,( void*) _temp816.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp808.inner_exp,( void*) _temp808.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp824.inner_exp,( void*) _temp824.assigns)));
return({ struct Cyc_NewControlFlow_AbsSyn _temp830; _temp830.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp832; _temp832.inner_exp=( void*) _temp828;
_temp832.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp832;}); _temp830.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp831;
_temp831.inner_exp=( void*) _temp829; _temp831.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp831;}); _temp830.lvalues= 0; _temp830;});}}}}
_LL201: { struct Cyc_NewControlFlow_AbsEnv _temp833= Cyc_NewControlFlow_change_lhs(
env, 0); _temp833.access_path= 0; Cyc_NewControlFlow_cf_prepass( _temp833,
_temp372);{ struct Cyc_Absyn_Stmt* _temp834= _temp372; while( 1) { void*
_temp835=( void*) _temp834->r; struct Cyc_Absyn_Stmt* _temp845; struct Cyc_Absyn_Stmt*
_temp847; struct Cyc_Absyn_Stmt* _temp849; struct Cyc_Absyn_Decl* _temp851;
struct Cyc_Absyn_Exp* _temp853; _LL837: if(( unsigned int) _temp835 > 1u?*(( int*)
_temp835) == Cyc_Absyn_Seq_s: 0){ _LL848: _temp847=(( struct Cyc_Absyn_Seq_s_struct*)
_temp835)->f1; goto _LL846; _LL846: _temp845=(( struct Cyc_Absyn_Seq_s_struct*)
_temp835)->f2; goto _LL838;} else{ goto _LL839;} _LL839: if(( unsigned int)
_temp835 > 1u?*(( int*) _temp835) == Cyc_Absyn_Decl_s: 0){ _LL852: _temp851=((
struct Cyc_Absyn_Decl_s_struct*) _temp835)->f1; goto _LL850; _LL850: _temp849=((
struct Cyc_Absyn_Decl_s_struct*) _temp835)->f2; goto _LL840;} else{ goto _LL841;}
_LL841: if(( unsigned int) _temp835 > 1u?*(( int*) _temp835) == Cyc_Absyn_Exp_s:
0){ _LL854: _temp853=(( struct Cyc_Absyn_Exp_s_struct*) _temp835)->f1; goto
_LL842;} else{ goto _LL843;} _LL843: goto _LL844; _LL838: _temp834= _temp845;
continue; _LL840: _temp834= _temp849; continue; _LL842: { struct Cyc_NewControlFlow_AbsSyn
_temp855= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp853); void* _temp858; void* _temp860; struct Cyc_NewControlFlow_AbsSynOne
_temp856= Cyc_NewControlFlow_meet_absexp( _temp855); _LL861: _temp860=( void*)
_temp856.inner_exp; goto _LL859; _LL859: _temp858=( void*) _temp856.assigns;
goto _LL857; _LL857:( void*)((( struct Cyc_List_List*) _check_null(( Cyc_NewControlFlow_get_stmt_annot(
_temp834))->absexps))->hd=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp860, _temp858)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE(
_temp372), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}
_LL844:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp862=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp862[ 0]=({ struct Cyc_Core_Impossible_struct _temp863; _temp863.tag= Cyc_Core_Impossible;
_temp863.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp863;}); _temp862;})); _LL836:;}}} _LL203: { struct Cyc_NewControlFlow_AbsEnv
_temp864= Cyc_NewControlFlow_change_lhs( env, 0); _temp864.access_path= 0;{
struct Cyc_NewControlFlow_AbsSyn _temp865= Cyc_NewControlFlow_abstract_exp(
_temp864, _temp376); struct Cyc_NewControlFlow_AbsSyn _temp866= Cyc_NewControlFlow_abstract_exp(
env, _temp374); struct Cyc_NewControlFlow_AbsSynOne _temp867= Cyc_NewControlFlow_meet_absexp(
_temp865); return({ struct Cyc_NewControlFlow_AbsSyn _temp868; _temp868.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp870; _temp870.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp867.inner_exp,( void*)( _temp866.when_true).inner_exp);
_temp870.assigns=( void*)(( void*)( _temp866.when_true).assigns); _temp870;});
_temp868.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp869; _temp869.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp867.inner_exp,(
void*)( _temp866.when_false).inner_exp); _temp869.assigns=( void*)(( void*)(
_temp866.when_false).assigns); _temp869;}); _temp868.lvalues= 0; _temp868;});}}
_LL205: env.access_path == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:726 env.access_path==null", sizeof( unsigned char),
47u)); Cyc_NewControlFlow_add_var_root( env, _temp382);( env.shared)->comprehension_vars=({
struct Cyc_List_List* _temp871=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp871->hd=( void*) _temp382; _temp871->tl=( env.shared)->comprehension_vars;
_temp871;});{ struct Cyc_NewControlFlow_AbsEnv _temp872= Cyc_NewControlFlow_use_it_env(
env); void* _temp875; void* _temp877; struct Cyc_NewControlFlow_AbsSynOne
_temp873= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp872, _temp380)); _LL878: _temp877=( void*) _temp873.inner_exp; goto _LL876;
_LL876: _temp875=( void*) _temp873.assigns; goto _LL874; _LL874: { void*
_temp881; void* _temp883; struct Cyc_NewControlFlow_AbsSynOne _temp879= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp872, _temp378)); _LL884: _temp883=( void*)
_temp879.inner_exp; goto _LL882; _LL882: _temp881=( void*) _temp879.assigns;
goto _LL880; _LL880: { void* _temp885= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp877, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp875, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp382), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp883, _temp881), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( _temp885, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL207: goto _LL209; _LL209: goto _LL211;
_LL211: goto _LL213; _LL213: goto _LL215; _LL215: goto _LL217; _LL217: goto
_LL219; _LL219:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp886=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp886[ 0]=({ struct Cyc_Core_Impossible_struct _temp887; _temp887.tag= Cyc_Core_Impossible;
_temp887.f1= _tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char),
34u); _temp887;}); _temp886;})); _LL131:;} void* Cyc_NewControlFlow_abstract_exp_top(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp890;
void* _temp892; struct Cyc_NewControlFlow_AbsSynOne _temp888= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, e)); _LL893: _temp892=( void*) _temp888.inner_exp;
goto _LL891; _LL891: _temp890=( void*) _temp888.assigns; goto _LL889; _LL889:
return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp892, _temp890);}
struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn _temp894= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), e); return({ struct _tuple0 _temp895;
_temp895.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)(
_temp894.when_true).inner_exp,( void*)( _temp894.when_true).assigns); _temp895.f2=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp894.when_false).inner_exp,(
void*)( _temp894.when_false).assigns); _temp895;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp896= 0; for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){
struct Cyc_Absyn_Switch_clause _temp899; struct Cyc_Position_Segment* _temp900;
struct Cyc_Absyn_Stmt* _temp902; struct Cyc_Absyn_Exp* _temp904; struct Cyc_Core_Opt*
_temp906; struct Cyc_Absyn_Pat* _temp908; struct Cyc_Absyn_Switch_clause*
_temp897=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp899=* _temp897; _LL909: _temp908= _temp899.pattern; goto _LL907;
_LL907: _temp906= _temp899.pat_vars; goto _LL905; _LL905: _temp904= _temp899.where_clause;
goto _LL903; _LL903: _temp902= _temp899.body; goto _LL901; _LL901: _temp900=
_temp899.loc; goto _LL898; _LL898: if( _temp906 == 0){( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp910=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp910[ 0]=({ struct
Cyc_Core_Impossible_struct _temp911; _temp911.tag= Cyc_Core_Impossible; _temp911.f1=
_tag_arr("switch clause vds not set", sizeof( unsigned char), 26u); _temp911;});
_temp910;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp906))->v; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd);}} if( _temp904 != 0){ void*
_temp914; void* _temp916; struct _tuple0 _temp912= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp904)); _LL917: _temp916= _temp912.f1;
goto _LL915; _LL915: _temp914= _temp912.f2; goto _LL913; _LL913: _temp896=({
struct Cyc_List_List* _temp918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp918->hd=( void*) _temp916; _temp918->tl=({ struct
Cyc_List_List* _temp919=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp919->hd=( void*) _temp914; _temp919->tl= _temp896; _temp919;}); _temp918;});}
Cyc_NewControlFlow_cf_prepass( env, _temp902);} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp896);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s){ env.lhsides ==
0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:778 env.lhsides == null",
sizeof( unsigned char), 45u)); env.access_path == 0? 0:(( int(*)( struct
_tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:779 env.access_path == null",
sizeof( unsigned char), 49u));{ void* _temp920=( void*) s->r; struct Cyc_Absyn_Exp*
_temp972; struct Cyc_Absyn_Stmt* _temp974; struct Cyc_Absyn_Stmt* _temp976;
struct Cyc_Absyn_Exp* _temp978; struct Cyc_Absyn_Exp* _temp980; struct Cyc_Absyn_Stmt*
_temp982; struct Cyc_Absyn_Stmt* _temp984; struct Cyc_Absyn_Exp* _temp986;
struct _tuple3 _temp988; struct Cyc_Absyn_Stmt* _temp990; struct Cyc_Absyn_Exp*
_temp992; struct Cyc_Absyn_Stmt* _temp994; struct Cyc_Absyn_Stmt* _temp996;
struct _tuple3 _temp998; struct Cyc_Absyn_Stmt* _temp1000; struct Cyc_Absyn_Exp*
_temp1002; struct Cyc_Absyn_Stmt* _temp1004; struct Cyc_Absyn_Stmt* _temp1006;
struct Cyc_Absyn_Stmt* _temp1008; struct _tagged_arr* _temp1010; struct Cyc_Absyn_Stmt*
_temp1012; struct _tagged_arr* _temp1014; struct Cyc_Absyn_Stmt* _temp1016;
struct _tuple3 _temp1018; struct Cyc_Absyn_Stmt* _temp1020; struct Cyc_Absyn_Exp*
_temp1022; struct _tuple3 _temp1024; struct Cyc_Absyn_Stmt* _temp1026; struct
Cyc_Absyn_Exp* _temp1028; struct Cyc_Absyn_Exp* _temp1030; struct Cyc_List_List*
_temp1032; struct Cyc_Absyn_Exp* _temp1034; struct Cyc_Absyn_Switch_clause**
_temp1036; struct Cyc_List_List* _temp1038; struct Cyc_Absyn_Stmt* _temp1040;
struct Cyc_Absyn_Decl* _temp1042; struct Cyc_Absyn_Decl _temp1044; struct Cyc_Position_Segment*
_temp1045; void* _temp1047; struct Cyc_Absyn_Vardecl* _temp1049; struct Cyc_Absyn_Stmt*
_temp1051; struct Cyc_Absyn_Decl* _temp1053; struct Cyc_Absyn_Decl _temp1055;
struct Cyc_Position_Segment* _temp1056; void* _temp1058; int _temp1060; struct
Cyc_Absyn_Exp* _temp1062; struct Cyc_Core_Opt* _temp1064; struct Cyc_Core_Opt*
_temp1066; struct Cyc_Core_Opt _temp1068; struct Cyc_List_List* _temp1069;
struct Cyc_Absyn_Pat* _temp1071; struct Cyc_Absyn_Stmt* _temp1073; struct Cyc_Absyn_Decl*
_temp1075; struct Cyc_Absyn_Decl _temp1077; struct Cyc_Position_Segment*
_temp1078; void* _temp1080; struct Cyc_List_List* _temp1082; struct Cyc_Absyn_Stmt*
_temp1084; struct _tagged_arr* _temp1086; struct Cyc_List_List* _temp1088;
struct Cyc_Absyn_Stmt* _temp1090; struct Cyc_Absyn_Stmt* _temp1092; struct Cyc_Absyn_Vardecl*
_temp1094; struct Cyc_Absyn_Tvar* _temp1096; struct Cyc_Absyn_Stmt* _temp1098;
struct Cyc_Absyn_Decl* _temp1100; struct Cyc_List_List* _temp1102; struct Cyc_Absyn_Exp*
_temp1104; struct Cyc_Absyn_Stmt* _temp1106; struct Cyc_Absyn_Stmt* _temp1108;
_LL922: if( _temp920 ==( void*) Cyc_Absyn_Skip_s){ goto _LL923;} else{ goto
_LL924;} _LL924: if(( unsigned int) _temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Exp_s:
0){ _LL973: _temp972=(( struct Cyc_Absyn_Exp_s_struct*) _temp920)->f1; goto
_LL925;} else{ goto _LL926;} _LL926: if(( unsigned int) _temp920 > 1u?*(( int*)
_temp920) == Cyc_Absyn_Seq_s: 0){ _LL977: _temp976=(( struct Cyc_Absyn_Seq_s_struct*)
_temp920)->f1; goto _LL975; _LL975: _temp974=(( struct Cyc_Absyn_Seq_s_struct*)
_temp920)->f2; goto _LL927;} else{ goto _LL928;} _LL928: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Return_s: 0){ _LL979: _temp978=((
struct Cyc_Absyn_Return_s_struct*) _temp920)->f1; if( _temp978 == 0){ goto
_LL929;} else{ goto _LL930;}} else{ goto _LL930;} _LL930: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Return_s: 0){ _LL981: _temp980=((
struct Cyc_Absyn_Return_s_struct*) _temp920)->f1; goto _LL931;} else{ goto
_LL932;} _LL932: if(( unsigned int) _temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_IfThenElse_s:
0){ _LL987: _temp986=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp920)->f1;
goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp920)->f2;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp920)->f3;
goto _LL933;} else{ goto _LL934;} _LL934: if(( unsigned int) _temp920 > 1u?*((
int*) _temp920) == Cyc_Absyn_Do_s: 0){ _LL995: _temp994=(( struct Cyc_Absyn_Do_s_struct*)
_temp920)->f1; goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_Do_s_struct*)
_temp920)->f2; _LL993: _temp992= _temp988.f1; goto _LL991; _LL991: _temp990=
_temp988.f2; goto _LL935;} else{ goto _LL936;} _LL936: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_While_s: 0){ _LL999: _temp998=((
struct Cyc_Absyn_While_s_struct*) _temp920)->f1; _LL1003: _temp1002= _temp998.f1;
goto _LL1001; _LL1001: _temp1000= _temp998.f2; goto _LL997; _LL997: _temp996=((
struct Cyc_Absyn_While_s_struct*) _temp920)->f2; goto _LL937;} else{ goto _LL938;}
_LL938: if(( unsigned int) _temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Break_s:
0){ _LL1005: _temp1004=(( struct Cyc_Absyn_Break_s_struct*) _temp920)->f1; goto
_LL939;} else{ goto _LL940;} _LL940: if(( unsigned int) _temp920 > 1u?*(( int*)
_temp920) == Cyc_Absyn_Continue_s: 0){ _LL1007: _temp1006=(( struct Cyc_Absyn_Continue_s_struct*)
_temp920)->f1; goto _LL941;} else{ goto _LL942;} _LL942: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Goto_s: 0){ _LL1011: _temp1010=((
struct Cyc_Absyn_Goto_s_struct*) _temp920)->f1; goto _LL1009; _LL1009: _temp1008=((
struct Cyc_Absyn_Goto_s_struct*) _temp920)->f2; if( _temp1008 == 0){ goto _LL943;}
else{ goto _LL944;}} else{ goto _LL944;} _LL944: if(( unsigned int) _temp920 > 1u?*((
int*) _temp920) == Cyc_Absyn_Goto_s: 0){ _LL1015: _temp1014=(( struct Cyc_Absyn_Goto_s_struct*)
_temp920)->f1; goto _LL1013; _LL1013: _temp1012=(( struct Cyc_Absyn_Goto_s_struct*)
_temp920)->f2; goto _LL945;} else{ goto _LL946;} _LL946: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_For_s: 0){ _LL1031: _temp1030=((
struct Cyc_Absyn_For_s_struct*) _temp920)->f1; goto _LL1025; _LL1025: _temp1024=((
struct Cyc_Absyn_For_s_struct*) _temp920)->f2; _LL1029: _temp1028= _temp1024.f1;
goto _LL1027; _LL1027: _temp1026= _temp1024.f2; goto _LL1019; _LL1019: _temp1018=((
struct Cyc_Absyn_For_s_struct*) _temp920)->f3; _LL1023: _temp1022= _temp1018.f1;
goto _LL1021; _LL1021: _temp1020= _temp1018.f2; goto _LL1017; _LL1017: _temp1016=((
struct Cyc_Absyn_For_s_struct*) _temp920)->f4; goto _LL947;} else{ goto _LL948;}
_LL948: if(( unsigned int) _temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Switch_s:
0){ _LL1035: _temp1034=(( struct Cyc_Absyn_Switch_s_struct*) _temp920)->f1; goto
_LL1033; _LL1033: _temp1032=(( struct Cyc_Absyn_Switch_s_struct*) _temp920)->f2;
goto _LL949;} else{ goto _LL950;} _LL950: if(( unsigned int) _temp920 > 1u?*((
int*) _temp920) == Cyc_Absyn_Fallthru_s: 0){ _LL1039: _temp1038=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp920)->f1; goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp920)->f2; goto _LL951;} else{ goto _LL952;} _LL952: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Decl_s: 0){ _LL1043: _temp1042=((
struct Cyc_Absyn_Decl_s_struct*) _temp920)->f1; _temp1044=* _temp1042; _LL1048:
_temp1047=( void*) _temp1044.r; if(*(( int*) _temp1047) == Cyc_Absyn_Var_d){
_LL1050: _temp1049=(( struct Cyc_Absyn_Var_d_struct*) _temp1047)->f1; goto
_LL1046;} else{ goto _LL954;} _LL1046: _temp1045= _temp1044.loc; goto _LL1041;
_LL1041: _temp1040=(( struct Cyc_Absyn_Decl_s_struct*) _temp920)->f2; goto
_LL953;} else{ goto _LL954;} _LL954: if(( unsigned int) _temp920 > 1u?*(( int*)
_temp920) == Cyc_Absyn_Decl_s: 0){ _LL1054: _temp1053=(( struct Cyc_Absyn_Decl_s_struct*)
_temp920)->f1; _temp1055=* _temp1053; _LL1059: _temp1058=( void*) _temp1055.r;
if(*(( int*) _temp1058) == Cyc_Absyn_Let_d){ _LL1072: _temp1071=(( struct Cyc_Absyn_Let_d_struct*)
_temp1058)->f1; goto _LL1067; _LL1067: _temp1066=(( struct Cyc_Absyn_Let_d_struct*)
_temp1058)->f2; if( _temp1066 == 0){ goto _LL956;} else{ _temp1068=* _temp1066;
_LL1070: _temp1069=( struct Cyc_List_List*) _temp1068.v; goto _LL1065;} _LL1065:
_temp1064=(( struct Cyc_Absyn_Let_d_struct*) _temp1058)->f3; goto _LL1063;
_LL1063: _temp1062=(( struct Cyc_Absyn_Let_d_struct*) _temp1058)->f4; goto
_LL1061; _LL1061: _temp1060=(( struct Cyc_Absyn_Let_d_struct*) _temp1058)->f5;
goto _LL1057;} else{ goto _LL956;} _LL1057: _temp1056= _temp1055.loc; goto
_LL1052; _LL1052: _temp1051=(( struct Cyc_Absyn_Decl_s_struct*) _temp920)->f2;
goto _LL955;} else{ goto _LL956;} _LL956: if(( unsigned int) _temp920 > 1u?*((
int*) _temp920) == Cyc_Absyn_Decl_s: 0){ _LL1076: _temp1075=(( struct Cyc_Absyn_Decl_s_struct*)
_temp920)->f1; _temp1077=* _temp1075; _LL1081: _temp1080=( void*) _temp1077.r;
if(*(( int*) _temp1080) == Cyc_Absyn_Letv_d){ _LL1083: _temp1082=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1080)->f1; goto _LL1079;} else{ goto _LL958;} _LL1079: _temp1078= _temp1077.loc;
goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_Decl_s_struct*) _temp920)->f2;
goto _LL957;} else{ goto _LL958;} _LL958: if(( unsigned int) _temp920 > 1u?*((
int*) _temp920) == Cyc_Absyn_Label_s: 0){ _LL1087: _temp1086=(( struct Cyc_Absyn_Label_s_struct*)
_temp920)->f1; goto _LL1085; _LL1085: _temp1084=(( struct Cyc_Absyn_Label_s_struct*)
_temp920)->f2; goto _LL959;} else{ goto _LL960;} _LL960: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_TryCatch_s: 0){ _LL1091:
_temp1090=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp920)->f1; goto _LL1089;
_LL1089: _temp1088=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp920)->f2; goto
_LL961;} else{ goto _LL962;} _LL962: if(( unsigned int) _temp920 > 1u?*(( int*)
_temp920) == Cyc_Absyn_Region_s: 0){ _LL1097: _temp1096=(( struct Cyc_Absyn_Region_s_struct*)
_temp920)->f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_Region_s_struct*)
_temp920)->f2; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_Region_s_struct*)
_temp920)->f3; goto _LL963;} else{ goto _LL964;} _LL964: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Decl_s: 0){ _LL1101: _temp1100=((
struct Cyc_Absyn_Decl_s_struct*) _temp920)->f1; goto _LL1099; _LL1099: _temp1098=((
struct Cyc_Absyn_Decl_s_struct*) _temp920)->f2; goto _LL965;} else{ goto _LL966;}
_LL966: if(( unsigned int) _temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_SwitchC_s:
0){ _LL1105: _temp1104=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp920)->f1;
goto _LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp920)->f2;
goto _LL967;} else{ goto _LL968;} _LL968: if(( unsigned int) _temp920 > 1u?*((
int*) _temp920) == Cyc_Absyn_Cut_s: 0){ _LL1107: _temp1106=(( struct Cyc_Absyn_Cut_s_struct*)
_temp920)->f1; goto _LL969;} else{ goto _LL970;} _LL970: if(( unsigned int)
_temp920 > 1u?*(( int*) _temp920) == Cyc_Absyn_Splice_s: 0){ _LL1109: _temp1108=((
struct Cyc_Absyn_Splice_s_struct*) _temp920)->f1; goto _LL971;} else{ goto
_LL921;} _LL923: return; _LL925: { void* _temp1110= Cyc_NewControlFlow_abstract_exp_top(
env, _temp972); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1111=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1111->hd=( void*) _temp1110; _temp1111->tl= 0; _temp1111;})); return;}
_LL927: Cyc_NewControlFlow_cf_prepass( env, _temp976); Cyc_NewControlFlow_cf_prepass(
env, _temp974); return; _LL929: return; _LL931: { void* _temp1112= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
_temp980)); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1113=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1113->hd=( void*) _temp1112; _temp1113->tl= 0; _temp1113;})); return;}
_LL933: { void* _temp1116; void* _temp1118; struct _tuple0 _temp1114= Cyc_NewControlFlow_abstract_guard(
env, _temp986); _LL1119: _temp1118= _temp1114.f1; goto _LL1117; _LL1117:
_temp1116= _temp1114.f2; goto _LL1115; _LL1115: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1120=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1120->hd=( void*) _temp1118; _temp1120->tl=({
struct Cyc_List_List* _temp1121=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1121->hd=( void*) _temp1116; _temp1121->tl= 0;
_temp1121;}); _temp1120;})); Cyc_NewControlFlow_cf_prepass( env, _temp984); Cyc_NewControlFlow_cf_prepass(
env, _temp982); return;} _LL935: _temp1002= _temp992; _temp1000= _temp990;
_temp996= _temp994; goto _LL937; _LL937: { void* _temp1124; void* _temp1126;
struct _tuple0 _temp1122= Cyc_NewControlFlow_abstract_guard( env, _temp1002);
_LL1127: _temp1126= _temp1122.f1; goto _LL1125; _LL1125: _temp1124= _temp1122.f2;
goto _LL1123; _LL1123: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1128=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1128->hd=( void*) _temp1126; _temp1128->tl=({ struct Cyc_List_List*
_temp1129=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1129->hd=( void*) _temp1124; _temp1129->tl= 0; _temp1129;}); _temp1128;}));
Cyc_NewControlFlow_cf_prepass( env, _temp996); return;} _LL939: return; _LL941:
return; _LL943:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1130=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1130[ 0]=({ struct Cyc_Core_Impossible_struct _temp1131; _temp1131.tag= Cyc_Core_Impossible;
_temp1131.f1= _tag_arr("cf_prepass: goto w/o destination", sizeof( unsigned char),
33u); _temp1131;}); _temp1130;})); _LL945: { struct Cyc_Absyn_Stmt* _temp1132=(
Cyc_NewControlFlow_get_stmt_annot( s))->encloser; struct Cyc_Absyn_Stmt*
_temp1133=( Cyc_NewControlFlow_get_stmt_annot(( struct Cyc_Absyn_Stmt*)
_check_null( _temp1012)))->encloser; while( _temp1133 != _temp1132) { struct Cyc_Absyn_Stmt*
_temp1134=( Cyc_NewControlFlow_get_stmt_annot( _temp1132))->encloser; if(
_temp1134 == _temp1132){ Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u)); break;} _temp1132= _temp1134;} return;} _LL947: {
void* _temp1135= Cyc_NewControlFlow_abstract_exp_top( env, _temp1030); void*
_temp1138; void* _temp1140; struct _tuple0 _temp1136= Cyc_NewControlFlow_abstract_guard(
env, _temp1028); _LL1141: _temp1140= _temp1136.f1; goto _LL1139; _LL1139:
_temp1138= _temp1136.f2; goto _LL1137; _LL1137: { void* _temp1142= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1022); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1143=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1143->hd=( void*) _temp1135; _temp1143->tl=({ struct Cyc_List_List*
_temp1144=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1144->hd=( void*) _temp1140; _temp1144->tl=({ struct Cyc_List_List*
_temp1145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1145->hd=( void*) _temp1138; _temp1145->tl=({ struct Cyc_List_List*
_temp1146=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1146->hd=( void*) _temp1142; _temp1146->tl= 0; _temp1146;}); _temp1145;});
_temp1144;}); _temp1143;})); Cyc_NewControlFlow_cf_prepass( env, _temp1016);
return;}} _LL949: { void* _temp1147= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp1034); struct Cyc_List_List* _temp1148= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1032); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1149=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1149->hd=( void*) _temp1147; _temp1149->tl= _temp1148; _temp1149;}));
return;} _LL951: { void* _temp1152; void* _temp1154; struct Cyc_NewControlFlow_AbsSynOne
_temp1150= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1038, 0)); _LL1155: _temp1154=( void*) _temp1150.inner_exp; goto
_LL1153; _LL1153: _temp1152=( void*) _temp1150.assigns; goto _LL1151; _LL1151: {
void* _temp1156= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1154, _temp1152); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1157=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1157->hd=( void*) _temp1156; _temp1157->tl= 0; _temp1157;})); return;}}
_LL953: Cyc_NewControlFlow_add_var_root( env, _temp1049); if( _temp1049->initializer
!= 0){ struct Cyc_NewControlFlow_AbsEnv _temp1158= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp1161=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1161->hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp1049); _temp1161->tl= 0; _temp1161;})); void* _temp1159= Cyc_NewControlFlow_abstract_exp_top(
_temp1158,( struct Cyc_Absyn_Exp*) _check_null( _temp1049->initializer)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1160=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1160->hd=( void*) _temp1159; _temp1160->tl= 0;
_temp1160;}));} Cyc_NewControlFlow_cf_prepass( env, _temp1040); return; _LL955:
for( 0; _temp1069 != 0; _temp1069=(( struct Cyc_List_List*) _check_null(
_temp1069))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1069))->hd);}{ void* _temp1162= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1062); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1163=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1163->hd=( void*) _temp1162; _temp1163->tl= 0;
_temp1163;})); Cyc_NewControlFlow_cf_prepass( env, _temp1051); return;} _LL957:
for( 0; _temp1082 != 0; _temp1082=(( struct Cyc_List_List*) _check_null(
_temp1082))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1082))->hd);} Cyc_NewControlFlow_cf_prepass(
env, _temp1073); return; _LL959: Cyc_NewControlFlow_cf_prepass( env, _temp1084);
return; _LL961: Cyc_NewControlFlow_cf_prepass( env, _temp1090);{ struct Cyc_List_List*
_temp1164= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1088); Cyc_NewControlFlow_cf_set_absexps(
s, _temp1164); return;} _LL963: Cyc_NewControlFlow_add_var_root( env, _temp1094);
Cyc_NewControlFlow_cf_prepass( env, _temp1092); return; _LL965: goto _LL967;
_LL967: goto _LL969; _LL969: goto _LL971; _LL971:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp1165=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1165[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1166; _temp1166.tag= Cyc_Core_Impossible;
_temp1166.f1= _tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u);
_temp1166;}); _temp1165;})); _LL921:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1167= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp1167->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
_temp1167->flow)); ++ _temp1167->visited; _temp1167->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:917 annot->visited == iteration_num",
sizeof( unsigned char), 57u)); return _temp1167;} static void Cyc_NewControlFlow_update_flow(
struct Cyc_Absyn_Stmt* s, void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1168= Cyc_NewControlFlow_get_stmt_annot( s); void* _temp1169= Cyc_CfFlowInfo_join_flow(
flow,( void*) _temp1168->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp1169,( void*) _temp1168->flow)){( void*)( _temp1168->flow=( void*)
_temp1169); if( _temp1168->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1170= flow; struct Cyc_Dict_Dict*
_temp1176; _LL1172: if( _temp1170 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1173;} else{ goto _LL1174;} _LL1174: if(( unsigned int) _temp1170 > 1u?*((
int*) _temp1170) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1177: _temp1176=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1170)->f1; goto _LL1175;} else{ goto _LL1171;} _LL1173: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1175: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1178=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1180=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1180[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1181; _temp1181.tag=
Cyc_CfFlowInfo_VarRoot; _temp1181.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1181;}); _temp1180;}); void* _temp1179= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) _temp1178)); _temp1176=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1176,(
void*) _temp1178, _temp1179);} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1182=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1182[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1183; _temp1183.tag=
Cyc_CfFlowInfo_InitsFL; _temp1183.f1= _temp1176; _temp1183;}); _temp1182;});
_LL1171:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1184= flow; struct Cyc_Dict_Dict*
_temp1190; _LL1186: if( _temp1184 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1187;} else{ goto _LL1188;} _LL1188: if(( unsigned int) _temp1184 > 1u?*((
int*) _temp1184) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1191: _temp1190=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1184)->f1; goto _LL1189;} else{ goto _LL1185;} _LL1187: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1189: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1192=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1193=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1193[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1194; _temp1194.tag=
Cyc_CfFlowInfo_VarRoot; _temp1194.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1194;}); _temp1193;}); _temp1190=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1190,(
void*) _temp1192,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots,( void*) _temp1192));} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1195=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1195[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1196; _temp1196.tag=
Cyc_CfFlowInfo_InitsFL; _temp1196.f1= _temp1190; _temp1196;}); _temp1195;});
_LL1185:;} static struct _tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){
if(( void*)(( struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){
void* _temp1197= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp1198; _temp1198.f1=
_temp1197; _temp1198.f2= _temp1197; _temp1198;});} return({ struct _tuple0
_temp1199; _temp1199.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct
Cyc_List_List*) _check_null( aes))->hd, in_flow); _temp1199.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp1199;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp1202; struct Cyc_Position_Segment*
_temp1203; struct Cyc_Absyn_Stmt* _temp1205; struct Cyc_Absyn_Exp* _temp1207;
struct Cyc_Core_Opt* _temp1209; struct Cyc_Absyn_Pat* _temp1211; struct Cyc_Absyn_Switch_clause*
_temp1200=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd; _temp1202=* _temp1200; _LL1212: _temp1211= _temp1202.pattern;
goto _LL1210; _LL1210: _temp1209= _temp1202.pat_vars; goto _LL1208; _LL1208:
_temp1207= _temp1202.where_clause; goto _LL1206; _LL1206: _temp1205= _temp1202.body;
goto _LL1204; _LL1204: _temp1203= _temp1202.loc; goto _LL1201; _LL1201: { void*
_temp1213= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1209))->v); _temp1213= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1213,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1209))->v);{ void* body_outflow; if( _temp1207 != 0){ void* _temp1216; void*
_temp1218; struct _tuple0 _temp1214= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp1207))->loc, aes, _temp1213); _LL1219:
_temp1218= _temp1214.f1; goto _LL1217; _LL1217: _temp1216= _temp1214.f2; goto
_LL1215; _LL1215: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp1216; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1205, _temp1218);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1205, _temp1213);}{ void* _temp1220= body_outflow; _LL1222: if(
_temp1220 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1223;} else{ goto _LL1224;}
_LL1224: goto _LL1225; _LL1223: goto _LL1221; _LL1225: Cyc_Tcutil_terr(
_temp1205->loc, _tag_arr("switch clause may implicitly fallthru", sizeof(
unsigned char), 38u)); goto _LL1221; _LL1221:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1226= Cyc_NewControlFlow_pre_stmt_check(
in_flow, s); in_flow=( void*) _temp1226->flow;{ struct Cyc_List_List* _temp1227=
_temp1226->absexps; void* _temp1228=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1274; struct Cyc_Absyn_Exp* _temp1276; struct Cyc_Absyn_Exp* _temp1278;
struct Cyc_Absyn_Stmt* _temp1280; struct Cyc_Absyn_Stmt* _temp1282; struct Cyc_Absyn_Stmt*
_temp1284; struct Cyc_Absyn_Stmt* _temp1286; struct Cyc_Absyn_Exp* _temp1288;
struct Cyc_Absyn_Stmt* _temp1290; struct _tuple3 _temp1292; struct Cyc_Absyn_Stmt*
_temp1294; struct Cyc_Absyn_Exp* _temp1296; struct _tuple3 _temp1298; struct Cyc_Absyn_Stmt*
_temp1300; struct Cyc_Absyn_Exp* _temp1302; struct Cyc_Absyn_Stmt* _temp1304;
struct Cyc_Absyn_Stmt* _temp1306; struct _tuple3 _temp1308; struct Cyc_Absyn_Stmt*
_temp1310; struct Cyc_Absyn_Exp* _temp1312; struct _tuple3 _temp1314; struct Cyc_Absyn_Stmt*
_temp1316; struct Cyc_Absyn_Exp* _temp1318; struct Cyc_Absyn_Exp* _temp1320;
struct Cyc_Absyn_Stmt* _temp1322; struct Cyc_Absyn_Switch_clause** _temp1324;
struct Cyc_Absyn_Switch_clause* _temp1326; struct Cyc_List_List* _temp1327;
struct Cyc_Absyn_Stmt* _temp1329; struct Cyc_Absyn_Stmt* _temp1331; struct Cyc_Absyn_Stmt*
_temp1333; struct _tagged_arr* _temp1335; struct Cyc_List_List* _temp1337;
struct Cyc_Absyn_Exp* _temp1339; struct Cyc_List_List* _temp1341; struct Cyc_Absyn_Stmt*
_temp1343; struct Cyc_Absyn_Stmt* _temp1345; struct Cyc_Absyn_Decl* _temp1347;
struct Cyc_Absyn_Decl _temp1349; struct Cyc_Position_Segment* _temp1350; void*
_temp1352; struct Cyc_Absyn_Vardecl* _temp1354; struct Cyc_Absyn_Stmt* _temp1356;
struct Cyc_Absyn_Decl* _temp1358; struct Cyc_Absyn_Decl _temp1360; struct Cyc_Position_Segment*
_temp1361; void* _temp1363; int _temp1365; struct Cyc_Absyn_Exp* _temp1367;
struct Cyc_Core_Opt* _temp1369; struct Cyc_Core_Opt* _temp1371; struct Cyc_Core_Opt
_temp1373; struct Cyc_List_List* _temp1374; struct Cyc_Absyn_Pat* _temp1376;
struct Cyc_Absyn_Stmt* _temp1378; struct Cyc_Absyn_Decl* _temp1380; struct Cyc_Absyn_Decl
_temp1382; struct Cyc_Position_Segment* _temp1383; void* _temp1385; struct Cyc_List_List*
_temp1387; struct Cyc_Absyn_Stmt* _temp1389; struct _tagged_arr* _temp1391;
struct Cyc_Absyn_Stmt* _temp1393; struct Cyc_Absyn_Vardecl* _temp1395; struct
Cyc_Absyn_Tvar* _temp1397; _LL1230: if( _temp1228 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1231;} else{ goto _LL1232;} _LL1232: if(( unsigned int) _temp1228 > 1u?*((
int*) _temp1228) == Cyc_Absyn_Return_s: 0){ _LL1275: _temp1274=(( struct Cyc_Absyn_Return_s_struct*)
_temp1228)->f1; if( _temp1274 == 0){ goto _LL1233;} else{ goto _LL1234;}} else{
goto _LL1234;} _LL1234: if(( unsigned int) _temp1228 > 1u?*(( int*) _temp1228)
== Cyc_Absyn_Return_s: 0){ _LL1277: _temp1276=(( struct Cyc_Absyn_Return_s_struct*)
_temp1228)->f1; goto _LL1235;} else{ goto _LL1236;} _LL1236: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Exp_s: 0){ _LL1279: _temp1278=((
struct Cyc_Absyn_Exp_s_struct*) _temp1228)->f1; goto _LL1237;} else{ goto
_LL1238;} _LL1238: if(( unsigned int) _temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Seq_s:
0){ _LL1283: _temp1282=(( struct Cyc_Absyn_Seq_s_struct*) _temp1228)->f1; goto
_LL1281; _LL1281: _temp1280=(( struct Cyc_Absyn_Seq_s_struct*) _temp1228)->f2;
goto _LL1239;} else{ goto _LL1240;} _LL1240: if(( unsigned int) _temp1228 > 1u?*((
int*) _temp1228) == Cyc_Absyn_IfThenElse_s: 0){ _LL1289: _temp1288=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1228)->f1; goto _LL1287; _LL1287: _temp1286=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1228)->f2; goto _LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1228)->f3; goto _LL1241;} else{ goto _LL1242;} _LL1242: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_While_s: 0){ _LL1293: _temp1292=((
struct Cyc_Absyn_While_s_struct*) _temp1228)->f1; _LL1297: _temp1296= _temp1292.f1;
goto _LL1295; _LL1295: _temp1294= _temp1292.f2; goto _LL1291; _LL1291: _temp1290=((
struct Cyc_Absyn_While_s_struct*) _temp1228)->f2; goto _LL1243;} else{ goto
_LL1244;} _LL1244: if(( unsigned int) _temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Do_s:
0){ _LL1305: _temp1304=(( struct Cyc_Absyn_Do_s_struct*) _temp1228)->f1; goto
_LL1299; _LL1299: _temp1298=(( struct Cyc_Absyn_Do_s_struct*) _temp1228)->f2;
_LL1303: _temp1302= _temp1298.f1; goto _LL1301; _LL1301: _temp1300= _temp1298.f2;
goto _LL1245;} else{ goto _LL1246;} _LL1246: if(( unsigned int) _temp1228 > 1u?*((
int*) _temp1228) == Cyc_Absyn_For_s: 0){ _LL1321: _temp1320=(( struct Cyc_Absyn_For_s_struct*)
_temp1228)->f1; goto _LL1315; _LL1315: _temp1314=(( struct Cyc_Absyn_For_s_struct*)
_temp1228)->f2; _LL1319: _temp1318= _temp1314.f1; goto _LL1317; _LL1317:
_temp1316= _temp1314.f2; goto _LL1309; _LL1309: _temp1308=(( struct Cyc_Absyn_For_s_struct*)
_temp1228)->f3; _LL1313: _temp1312= _temp1308.f1; goto _LL1311; _LL1311:
_temp1310= _temp1308.f2; goto _LL1307; _LL1307: _temp1306=(( struct Cyc_Absyn_For_s_struct*)
_temp1228)->f4; goto _LL1247;} else{ goto _LL1248;} _LL1248: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Break_s: 0){ _LL1323: _temp1322=((
struct Cyc_Absyn_Break_s_struct*) _temp1228)->f1; if( _temp1322 == 0){ goto
_LL1249;} else{ goto _LL1250;}} else{ goto _LL1250;} _LL1250: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Fallthru_s: 0){ _LL1328:
_temp1327=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1228)->f1; goto _LL1325;
_LL1325: _temp1324=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1228)->f2; if(
_temp1324 == 0){ goto _LL1252;} else{ _temp1326=* _temp1324; goto _LL1251;}}
else{ goto _LL1252;} _LL1252: if(( unsigned int) _temp1228 > 1u?*(( int*)
_temp1228) == Cyc_Absyn_Break_s: 0){ _LL1330: _temp1329=(( struct Cyc_Absyn_Break_s_struct*)
_temp1228)->f1; goto _LL1253;} else{ goto _LL1254;} _LL1254: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Continue_s: 0){ _LL1332:
_temp1331=(( struct Cyc_Absyn_Continue_s_struct*) _temp1228)->f1; goto _LL1255;}
else{ goto _LL1256;} _LL1256: if(( unsigned int) _temp1228 > 1u?*(( int*)
_temp1228) == Cyc_Absyn_Goto_s: 0){ _LL1336: _temp1335=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1228)->f1; goto _LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1228)->f2; goto _LL1257;} else{ goto _LL1258;} _LL1258: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Switch_s: 0){ _LL1340:
_temp1339=(( struct Cyc_Absyn_Switch_s_struct*) _temp1228)->f1; goto _LL1338;
_LL1338: _temp1337=(( struct Cyc_Absyn_Switch_s_struct*) _temp1228)->f2; goto
_LL1259;} else{ goto _LL1260;} _LL1260: if(( unsigned int) _temp1228 > 1u?*((
int*) _temp1228) == Cyc_Absyn_TryCatch_s: 0){ _LL1344: _temp1343=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1228)->f1; goto _LL1342; _LL1342: _temp1341=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1228)->f2; goto _LL1261;} else{ goto _LL1262;} _LL1262: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Decl_s: 0){ _LL1348: _temp1347=((
struct Cyc_Absyn_Decl_s_struct*) _temp1228)->f1; _temp1349=* _temp1347; _LL1353:
_temp1352=( void*) _temp1349.r; if(*(( int*) _temp1352) == Cyc_Absyn_Var_d){
_LL1355: _temp1354=(( struct Cyc_Absyn_Var_d_struct*) _temp1352)->f1; goto
_LL1351;} else{ goto _LL1264;} _LL1351: _temp1350= _temp1349.loc; goto _LL1346;
_LL1346: _temp1345=(( struct Cyc_Absyn_Decl_s_struct*) _temp1228)->f2; goto
_LL1263;} else{ goto _LL1264;} _LL1264: if(( unsigned int) _temp1228 > 1u?*((
int*) _temp1228) == Cyc_Absyn_Decl_s: 0){ _LL1359: _temp1358=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1228)->f1; _temp1360=* _temp1358; _LL1364: _temp1363=( void*) _temp1360.r;
if(*(( int*) _temp1363) == Cyc_Absyn_Let_d){ _LL1377: _temp1376=(( struct Cyc_Absyn_Let_d_struct*)
_temp1363)->f1; goto _LL1372; _LL1372: _temp1371=(( struct Cyc_Absyn_Let_d_struct*)
_temp1363)->f2; if( _temp1371 == 0){ goto _LL1266;} else{ _temp1373=* _temp1371;
_LL1375: _temp1374=( struct Cyc_List_List*) _temp1373.v; goto _LL1370;} _LL1370:
_temp1369=(( struct Cyc_Absyn_Let_d_struct*) _temp1363)->f3; goto _LL1368;
_LL1368: _temp1367=(( struct Cyc_Absyn_Let_d_struct*) _temp1363)->f4; goto
_LL1366; _LL1366: _temp1365=(( struct Cyc_Absyn_Let_d_struct*) _temp1363)->f5;
goto _LL1362;} else{ goto _LL1266;} _LL1362: _temp1361= _temp1360.loc; goto
_LL1357; _LL1357: _temp1356=(( struct Cyc_Absyn_Decl_s_struct*) _temp1228)->f2;
goto _LL1265;} else{ goto _LL1266;} _LL1266: if(( unsigned int) _temp1228 > 1u?*((
int*) _temp1228) == Cyc_Absyn_Decl_s: 0){ _LL1381: _temp1380=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1228)->f1; _temp1382=* _temp1380; _LL1386: _temp1385=( void*) _temp1382.r;
if(*(( int*) _temp1385) == Cyc_Absyn_Letv_d){ _LL1388: _temp1387=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1385)->f1; goto _LL1384;} else{ goto _LL1268;} _LL1384: _temp1383=
_temp1382.loc; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1228)->f2; goto _LL1267;} else{ goto _LL1268;} _LL1268: if(( unsigned int)
_temp1228 > 1u?*(( int*) _temp1228) == Cyc_Absyn_Label_s: 0){ _LL1392: _temp1391=((
struct Cyc_Absyn_Label_s_struct*) _temp1228)->f1; goto _LL1390; _LL1390:
_temp1389=(( struct Cyc_Absyn_Label_s_struct*) _temp1228)->f2; goto _LL1269;}
else{ goto _LL1270;} _LL1270: if(( unsigned int) _temp1228 > 1u?*(( int*)
_temp1228) == Cyc_Absyn_Region_s: 0){ _LL1398: _temp1397=(( struct Cyc_Absyn_Region_s_struct*)
_temp1228)->f1; goto _LL1396; _LL1396: _temp1395=(( struct Cyc_Absyn_Region_s_struct*)
_temp1228)->f2; goto _LL1394; _LL1394: _temp1393=(( struct Cyc_Absyn_Region_s_struct*)
_temp1228)->f3; goto _LL1271;} else{ goto _LL1272;} _LL1272: goto _LL1273;
_LL1231: return in_flow; _LL1233: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1235: Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*) _check_null(
_temp1276))->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1227))->hd,
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1237: return Cyc_CfAbsexp_eval_absexp(
env, _temp1278->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1227))->hd,
in_flow); _LL1239: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1280,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1282, in_flow)); _LL1241: { void*
_temp1401; void* _temp1403; struct _tuple0 _temp1399= Cyc_NewControlFlow_cf_evalguard(
env, _temp1288->loc, _temp1227, in_flow); _LL1404: _temp1403= _temp1399.f1; goto
_LL1402; _LL1402: _temp1401= _temp1399.f2; goto _LL1400; _LL1400: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1286, _temp1403), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1284, _temp1401));} _LL1243: { void* _temp1405=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1294))->flow; void* _temp1408; void* _temp1410; struct _tuple0
_temp1406= Cyc_NewControlFlow_cf_evalguard( env, _temp1296->loc, _temp1227,
_temp1405); _LL1411: _temp1410= _temp1406.f1; goto _LL1409; _LL1409: _temp1408=
_temp1406.f2; goto _LL1407; _LL1407: { void* _temp1412= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1290, _temp1410); Cyc_NewControlFlow_update_flow( _temp1294, _temp1412);
return _temp1408;}} _LL1245: { void* _temp1413= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1304, in_flow); void* _temp1414=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1413, _temp1300))->flow; void* _temp1417; void* _temp1419; struct _tuple0
_temp1415= Cyc_NewControlFlow_cf_evalguard( env, _temp1302->loc, _temp1227,
_temp1414); _LL1420: _temp1419= _temp1415.f1; goto _LL1418; _LL1418: _temp1417=
_temp1415.f2; goto _LL1416; _LL1416: Cyc_NewControlFlow_update_flow( _temp1304,
_temp1419); return _temp1417;} _LL1247: { void* _temp1421= Cyc_CfAbsexp_eval_absexp(
env, _temp1320->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1227))->hd,
in_flow); void* _temp1422=( void*)( Cyc_NewControlFlow_pre_stmt_check( _temp1421,
_temp1316))->flow; void* _temp1425; void* _temp1427; struct _tuple0 _temp1423=
Cyc_NewControlFlow_cf_evalguard( env, _temp1318->loc,(( struct Cyc_List_List*)
_check_null( _temp1227))->tl, _temp1422); _LL1428: _temp1427= _temp1423.f1; goto
_LL1426; _LL1426: _temp1425= _temp1423.f2; goto _LL1424; _LL1424: { void*
_temp1429= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1306, _temp1427); void*
_temp1430=( void*)( Cyc_NewControlFlow_pre_stmt_check( _temp1429, _temp1310))->flow;
void* _temp1431= Cyc_CfAbsexp_eval_absexp( env, _temp1312->loc,( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1227))->tl))->tl))->tl))->hd,
_temp1430); Cyc_NewControlFlow_update_flow( _temp1316, _temp1431); return
_temp1425;}} _LL1249: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1251: in_flow=
Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp1227))->hd, in_flow);{ struct Cyc_List_List* _temp1432=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1326->pat_vars))->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1432); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, _temp1432); _temp1329=(
struct Cyc_Absyn_Stmt*) _temp1326->body; goto _LL1253;} _LL1253: _temp1331=
_temp1329; goto _LL1255; _LL1255: _temp1333= _temp1331; goto _LL1257; _LL1257:
Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*) _check_null( _temp1333),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1259: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1339->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1227))->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1337,(( struct
Cyc_List_List*) _check_null( _temp1227))->tl, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1261: { struct Cyc_NewControlFlow_AnalEnv _temp1435; void* _temp1436; int
_temp1438; struct Cyc_Dict_Dict* _temp1440; struct Cyc_NewControlFlow_AnalEnv*
_temp1433= env; _temp1435=* _temp1433; _LL1441: _temp1440= _temp1435.roots; goto
_LL1439; _LL1439: _temp1438= _temp1435.in_try; goto _LL1437; _LL1437: _temp1436=(
void*) _temp1435.tryflow; goto _LL1434; _LL1434: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* _temp1442= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1343, in_flow); void* _temp1443=( void*) env->tryflow; env->in_try=
_temp1438;( void*)( env->tryflow=( void*) _temp1436); Cyc_NewControlFlow_update_tryflow(
env, _temp1443); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1341,
_temp1227, _temp1443); return _temp1442;}} _LL1263: { struct Cyc_List_List
_temp1444=({ struct Cyc_List_List _temp1447; _temp1447.hd=( void*) _temp1354;
_temp1447.tl= 0; _temp1447;}); void* _temp1445= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)& _temp1444); struct Cyc_Absyn_Exp*
_temp1446= _temp1354->initializer; if( _temp1446 != 0){ _temp1445= Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1446))->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp1227))->hd, _temp1445);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1345, _temp1445);} _LL1265: { void* _temp1448= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1374); _temp1448= Cyc_CfAbsexp_eval_absexp( env, _temp1367->loc,(
void*)(( struct Cyc_List_List*) _check_null( _temp1227))->hd, _temp1448);
_temp1448= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1448, _temp1374);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1356, _temp1448);} _LL1267: {
void* _temp1449= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1387);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1378, _temp1449);} _LL1269:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1389, in_flow); _LL1271: {
struct Cyc_List_List* _temp1450=({ struct Cyc_List_List* _temp1452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1452->hd=( void*) _temp1395;
_temp1452->tl= 0; _temp1452;}); void* _temp1451= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1450); _temp1451= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp1451, _temp1450); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1393,
_temp1451);} _LL1273:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1453=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1453[ 0]=({ struct Cyc_Core_Impossible_struct _temp1454; _temp1454.tag= Cyc_Core_Impossible;
_temp1454.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp1454;}); _temp1453;})); _LL1229:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1455=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1479; struct Cyc_Absyn_Stmt*
_temp1481; struct Cyc_Absyn_Stmt* _temp1483; struct Cyc_Absyn_Stmt* _temp1485;
struct Cyc_Absyn_Exp* _temp1487; struct Cyc_Absyn_Stmt* _temp1489; struct
_tuple3 _temp1491; struct Cyc_Absyn_Stmt* _temp1493; struct Cyc_Absyn_Exp*
_temp1495; struct _tuple3 _temp1497; struct Cyc_Absyn_Stmt* _temp1499; struct
Cyc_Absyn_Exp* _temp1501; struct Cyc_Absyn_Exp* _temp1503; struct Cyc_Absyn_Stmt*
_temp1505; struct _tuple3 _temp1507; struct Cyc_Absyn_Stmt* _temp1509; struct
Cyc_Absyn_Exp* _temp1511; struct _tuple3 _temp1513; struct Cyc_Absyn_Stmt*
_temp1515; struct Cyc_Absyn_Exp* _temp1517; struct Cyc_Absyn_Stmt* _temp1519;
struct Cyc_Absyn_Stmt* _temp1521; struct Cyc_Absyn_Vardecl* _temp1523; struct
Cyc_Absyn_Tvar* _temp1525; struct Cyc_Absyn_Stmt* _temp1527; struct _tagged_arr*
_temp1529; struct Cyc_Absyn_Stmt* _temp1531; struct Cyc_Absyn_Decl* _temp1533;
struct Cyc_List_List* _temp1535; struct Cyc_Absyn_Stmt* _temp1537; struct Cyc_List_List*
_temp1539; struct Cyc_Absyn_Exp* _temp1541; _LL1457: if(( unsigned int)
_temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Seq_s: 0){ _LL1482: _temp1481=((
struct Cyc_Absyn_Seq_s_struct*) _temp1455)->f1; goto _LL1480; _LL1480: _temp1479=((
struct Cyc_Absyn_Seq_s_struct*) _temp1455)->f2; goto _LL1458;} else{ goto
_LL1459;} _LL1459: if(( unsigned int) _temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_IfThenElse_s:
0){ _LL1488: _temp1487=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1455)->f1;
goto _LL1486; _LL1486: _temp1485=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1455)->f2; goto _LL1484; _LL1484: _temp1483=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1455)->f3; goto _LL1460;} else{ goto _LL1461;} _LL1461: if(( unsigned int)
_temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_For_s: 0){ _LL1504: _temp1503=((
struct Cyc_Absyn_For_s_struct*) _temp1455)->f1; goto _LL1498; _LL1498: _temp1497=((
struct Cyc_Absyn_For_s_struct*) _temp1455)->f2; _LL1502: _temp1501= _temp1497.f1;
goto _LL1500; _LL1500: _temp1499= _temp1497.f2; goto _LL1492; _LL1492: _temp1491=((
struct Cyc_Absyn_For_s_struct*) _temp1455)->f3; _LL1496: _temp1495= _temp1491.f1;
goto _LL1494; _LL1494: _temp1493= _temp1491.f2; goto _LL1490; _LL1490: _temp1489=((
struct Cyc_Absyn_For_s_struct*) _temp1455)->f4; goto _LL1462;} else{ goto
_LL1463;} _LL1463: if(( unsigned int) _temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_While_s:
0){ _LL1508: _temp1507=(( struct Cyc_Absyn_While_s_struct*) _temp1455)->f1;
_LL1512: _temp1511= _temp1507.f1; goto _LL1510; _LL1510: _temp1509= _temp1507.f2;
goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_While_s_struct*) _temp1455)->f2;
goto _LL1464;} else{ goto _LL1465;} _LL1465: if(( unsigned int) _temp1455 > 1u?*((
int*) _temp1455) == Cyc_Absyn_Do_s: 0){ _LL1520: _temp1519=(( struct Cyc_Absyn_Do_s_struct*)
_temp1455)->f1; goto _LL1514; _LL1514: _temp1513=(( struct Cyc_Absyn_Do_s_struct*)
_temp1455)->f2; _LL1518: _temp1517= _temp1513.f1; goto _LL1516; _LL1516:
_temp1515= _temp1513.f2; goto _LL1466;} else{ goto _LL1467;} _LL1467: if((
unsigned int) _temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Region_s: 0){
_LL1526: _temp1525=(( struct Cyc_Absyn_Region_s_struct*) _temp1455)->f1; goto
_LL1524; _LL1524: _temp1523=(( struct Cyc_Absyn_Region_s_struct*) _temp1455)->f2;
goto _LL1522; _LL1522: _temp1521=(( struct Cyc_Absyn_Region_s_struct*) _temp1455)->f3;
goto _LL1468;} else{ goto _LL1469;} _LL1469: if(( unsigned int) _temp1455 > 1u?*((
int*) _temp1455) == Cyc_Absyn_Label_s: 0){ _LL1530: _temp1529=(( struct Cyc_Absyn_Label_s_struct*)
_temp1455)->f1; goto _LL1528; _LL1528: _temp1527=(( struct Cyc_Absyn_Label_s_struct*)
_temp1455)->f2; goto _LL1470;} else{ goto _LL1471;} _LL1471: if(( unsigned int)
_temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Decl_s: 0){ _LL1534: _temp1533=((
struct Cyc_Absyn_Decl_s_struct*) _temp1455)->f1; goto _LL1532; _LL1532:
_temp1531=(( struct Cyc_Absyn_Decl_s_struct*) _temp1455)->f2; goto _LL1472;}
else{ goto _LL1473;} _LL1473: if(( unsigned int) _temp1455 > 1u?*(( int*)
_temp1455) == Cyc_Absyn_TryCatch_s: 0){ _LL1538: _temp1537=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1455)->f1; goto _LL1536; _LL1536: _temp1535=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1455)->f2; goto _LL1474;} else{ goto _LL1475;} _LL1475: if(( unsigned int)
_temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Switch_s: 0){ _LL1542:
_temp1541=(( struct Cyc_Absyn_Switch_s_struct*) _temp1455)->f1; goto _LL1540;
_LL1540: _temp1539=(( struct Cyc_Absyn_Switch_s_struct*) _temp1455)->f2; goto
_LL1476;} else{ goto _LL1477;} _LL1477: goto _LL1478; _LL1458: _temp1485=
_temp1481; _temp1483= _temp1479; goto _LL1460; _LL1460: Cyc_NewControlFlow_cf_postpass(
_temp1485); Cyc_NewControlFlow_cf_postpass( _temp1483); return; _LL1462: Cyc_NewControlFlow_cf_postpass(
_temp1493); _temp1509= _temp1489; _temp1505= _temp1499; goto _LL1464; _LL1464:
_temp1519= _temp1505; _temp1515= _temp1509; goto _LL1466; _LL1466: Cyc_NewControlFlow_cf_postpass(
_temp1515); _temp1521= _temp1519; goto _LL1468; _LL1468: _temp1527= _temp1521;
goto _LL1470; _LL1470: _temp1531= _temp1527; goto _LL1472; _LL1472: Cyc_NewControlFlow_cf_postpass(
_temp1531); return; _LL1474: Cyc_NewControlFlow_cf_postpass( _temp1537);
_temp1539= _temp1535; goto _LL1476; _LL1476: for( 0; _temp1539 != 0; _temp1539=((
struct Cyc_List_List*) _check_null( _temp1539))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1539))->hd)->body);}
return; _LL1478: return; _LL1456:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1567; _temp1567.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1568=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1568->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1568->comprehension_vars= 0; _temp1568;}); _temp1567.lhsides= 0; _temp1567.access_path=
0; _temp1567;}); struct Cyc_List_List* _temp1543=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v;{ struct Cyc_List_List*
vds= _temp1543; for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
Cyc_NewControlFlow_add_var_root( absenv,( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_List_List*) _check_null( vds))->hd);}} Cyc_NewControlFlow_cf_prepass( absenv,
fd->body);{ struct Cyc_List_List* _temp1546; struct Cyc_List_List* _temp1548;
struct Cyc_NewControlFlow_SharedAbsEnv* _temp1550; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1552; struct Cyc_List_List* _temp1553; struct Cyc_Dict_Dict* _temp1555;
struct Cyc_NewControlFlow_AbsEnv _temp1544= absenv; _LL1551: _temp1550=
_temp1544.shared; _temp1552=* _temp1550; _LL1556: _temp1555= _temp1552.all_roots;
goto _LL1554; _LL1554: _temp1553= _temp1552.comprehension_vars; goto _LL1549;
_LL1549: _temp1548= _temp1544.lhsides; goto _LL1547; _LL1547: _temp1546=
_temp1544.access_path; goto _LL1545; _LL1545: { struct Cyc_NewControlFlow_AnalEnv*
_temp1557=({ struct Cyc_NewControlFlow_AnalEnv* _temp1566=( struct Cyc_NewControlFlow_AnalEnv*)
GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp1566->roots=
_temp1555; _temp1566->in_try= 0; _temp1566->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp1566;}); void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1564=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1564[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1565; _temp1565.tag=
Cyc_CfFlowInfo_InitsFL; _temp1565.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1565;});
_temp1564;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp1557, in_flow,
_temp1543); in_flow= Cyc_NewControlFlow_add_vardecls_flow( _temp1557, in_flow,
_temp1553);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp1557, fd->body, in_flow);}{ void* _temp1558= out_flow; _LL1560: if(
_temp1558 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1561;} else{ goto _LL1562;}
_LL1562: goto _LL1563; _LL1561: goto _LL1559; _LL1563: if(( void*) fd->ret_type
!=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may complete without returning a value",
sizeof( unsigned char), 48u));} goto _LL1559; _LL1559:;} Cyc_NewControlFlow_cf_postpass(
fd->body);}}}} void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds){ for(
0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp1569=(
void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( ds))->hd)->r;
struct Cyc_Absyn_Fndecl* _temp1579; struct Cyc_List_List* _temp1581; struct
_tuple1* _temp1583; struct Cyc_List_List* _temp1585; struct _tagged_arr*
_temp1587; _LL1571: if(*(( int*) _temp1569) == Cyc_Absyn_Fn_d){ _LL1580:
_temp1579=(( struct Cyc_Absyn_Fn_d_struct*) _temp1569)->f1; goto _LL1572;} else{
goto _LL1573;} _LL1573: if(*(( int*) _temp1569) == Cyc_Absyn_Using_d){ _LL1584:
_temp1583=(( struct Cyc_Absyn_Using_d_struct*) _temp1569)->f1; goto _LL1582;
_LL1582: _temp1581=(( struct Cyc_Absyn_Using_d_struct*) _temp1569)->f2; goto
_LL1574;} else{ goto _LL1575;} _LL1575: if(*(( int*) _temp1569) == Cyc_Absyn_Namespace_d){
_LL1588: _temp1587=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1569)->f1; goto
_LL1586; _LL1586: _temp1585=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1569)->f2;
goto _LL1576;} else{ goto _LL1577;} _LL1577: goto _LL1578; _LL1572: Cyc_NewControlFlow_cf_check_fun(
_temp1579); goto _LL1570; _LL1574: _temp1585= _temp1581; goto _LL1576; _LL1576:
Cyc_NewControlFlow_cf_check( _temp1585); goto _LL1570; _LL1578: goto _LL1570;
_LL1570:;}}