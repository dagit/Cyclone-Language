 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; struct
_tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*);
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
src, Cyc_NewControlFlow_use_it_list()); goto _LL21; _LL21: _temp20.access_path=
0; return _temp20;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp24; struct Cyc_NewControlFlow_AbsSynOne
_temp26; void* _temp28; void* _temp30; struct Cyc_NewControlFlow_AbsSynOne
_temp32; void* _temp34; void* _temp36; struct Cyc_NewControlFlow_AbsSyn _temp22=
syn; _LL33: _temp32= _temp22.when_true; _LL37: _temp36=( void*) _temp32.inner_exp;
goto _LL35; _LL35: _temp34=( void*) _temp32.assigns; goto _LL27; _LL27: _temp26=
_temp22.when_false; _LL31: _temp30=( void*) _temp26.inner_exp; goto _LL29; _LL29:
_temp28=( void*) _temp26.assigns; goto _LL25; _LL25: _temp24= _temp22.lvalues;
goto _LL23; _LL23: { void* _temp38= _temp36 == _temp30? _temp36: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp36, _temp30); goto _LL39; _LL39: { void*
_temp40= _temp34 == _temp28? _temp34: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
_temp34, _temp28); goto _LL41; _LL41: return({ struct Cyc_NewControlFlow_AbsSynOne
_temp42; _temp42.inner_exp=( void*) _temp38; _temp42.assigns=( void*) _temp40;
_temp42;});}}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_mkSyn_tf(
void* ae, struct Cyc_List_List* lvals, void* assigns){ struct Cyc_NewControlFlow_AbsSynOne
_temp44=({ struct Cyc_NewControlFlow_AbsSynOne _temp43; _temp43.inner_exp=( void*)
ae; _temp43.assigns=( void*) assigns; _temp43;}); goto _LL45; _LL45: return({
struct Cyc_NewControlFlow_AbsSyn _temp46; _temp46.when_true= _temp44; _temp46.when_false=
_temp44; _temp46.lvalues= lvals; _temp46;});} void* Cyc_NewControlFlow_make_assigns(
struct Cyc_NewControlFlow_AbsEnv env, void* ae){ if( env.lhsides == 0){ return
Cyc_CfAbsexp_mkSkipAE();} if((( struct Cyc_List_List*) _check_null( env.lhsides))->tl
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( env.lhsides))->hd, ae);}{ struct Cyc_List_List* _temp47= 0; goto
_LL48; _LL48:{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0;
lhsides=(( struct Cyc_List_List*) _check_null( lhsides))->tl){ _temp47=({ struct
Cyc_List_List* _temp49=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp49->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( lhsides))->hd, ae); _temp49->tl= _temp47; _temp49;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, _temp47);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp50= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
goto _LL51; _LL51: { void* _temp54; void* _temp56; struct Cyc_NewControlFlow_AbsSynOne
_temp52= Cyc_NewControlFlow_meet_absexp( _temp50); _LL57: _temp56=( void*)
_temp52.inner_exp; goto _LL55; _LL55: _temp54=( void*) _temp52.assigns; goto
_LL53; _LL53: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp56, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp54, uses_exp);}} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp58=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp58->hd=( void*) path_e; _temp58->tl=
env.access_path; _temp58;});{ struct Cyc_NewControlFlow_AbsSyn _temp59= Cyc_NewControlFlow_abstract_exp(
env, inner_e); goto _LL60; _LL60: if( _temp59.lvalues == 0){ _temp59.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp59;}} void* Cyc_NewControlFlow_destruct_path(
struct Cyc_NewControlFlow_AbsEnv env, void* absop){{ struct Cyc_List_List* path=
env.access_path; for( 0; path != 0; path=(( struct Cyc_List_List*) _check_null(
path))->tl){ void* _temp61=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( path))->hd)->r; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp*
_temp77; struct _tagged_arr* _temp79; struct Cyc_Absyn_Exp* _temp81; struct
_tagged_arr* _temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp*
_temp87; struct Cyc_Absyn_Exp* _temp89; _LL63: if(*(( int*) _temp61) == Cyc_Absyn_Address_e){
_LL76: _temp75=(( struct Cyc_Absyn_Address_e_struct*) _temp61)->f1; goto _LL64;}
else{ goto _LL65;} _LL65: if(*(( int*) _temp61) == Cyc_Absyn_Deref_e){ _LL78:
_temp77=(( struct Cyc_Absyn_Deref_e_struct*) _temp61)->f1; goto _LL66;} else{
goto _LL67;} _LL67: if(*(( int*) _temp61) == Cyc_Absyn_StructArrow_e){ _LL82:
_temp81=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp61)->f1; goto _LL80;
_LL80: _temp79=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp61)->f2; goto
_LL68;} else{ goto _LL69;} _LL69: if(*(( int*) _temp61) == Cyc_Absyn_StructMember_e){
_LL86: _temp85=(( struct Cyc_Absyn_StructMember_e_struct*) _temp61)->f1; goto
_LL84; _LL84: _temp83=(( struct Cyc_Absyn_StructMember_e_struct*) _temp61)->f2;
goto _LL70;} else{ goto _LL71;} _LL71: if(*(( int*) _temp61) == Cyc_Absyn_Subscript_e){
_LL90: _temp89=(( struct Cyc_Absyn_Subscript_e_struct*) _temp61)->f1; goto _LL88;
_LL88: _temp87=(( struct Cyc_Absyn_Subscript_e_struct*) _temp61)->f2; goto _LL72;}
else{ goto _LL73;} _LL73: goto _LL74; _LL64:(( struct Cyc_List_List*)
_check_null( path))->tl == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:219 path->tl == null", sizeof( unsigned char), 42u));
return Cyc_CfAbsexp_mkAddressOp( absop); _LL66: absop= Cyc_CfAbsexp_mkDerefOp(
absop); goto _LL62; _LL68: absop= Cyc_CfAbsexp_mkDerefOp( absop); _temp83=
_temp79; goto _LL70; _LL70: absop= Cyc_CfAbsexp_mkMemberOp( absop,( void*)({
struct Cyc_CfFlowInfo_StructF_struct* _temp91=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp91[ 0]=({ struct
Cyc_CfFlowInfo_StructF_struct _temp92; _temp92.tag= Cyc_CfFlowInfo_StructF;
_temp92.f1= _temp83; _temp92;}); _temp91;})); goto _LL62; _LL72: { unsigned int
_temp93= Cyc_Evexp_eval_const_uint_exp( _temp87); goto _LL94; _LL94: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp95=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp95[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp96; _temp96.tag= Cyc_CfFlowInfo_TupleF;
_temp96.f1=( int) _temp93; _temp96;}); _temp95;})); goto _LL62;} _LL74:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp97=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp97[ 0]=({ struct
Cyc_Core_Impossible_struct _temp98; _temp98.tag= Cyc_Core_Impossible; _temp98.f1=
_tag_arr("abstract_exp: unexpected acces path element", sizeof( unsigned char),
44u); _temp98;}); _temp97;})); _LL62:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_malloc( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* rgn_exp, struct Cyc_Absyn_Exp*
init_exp){ void* _temp99= Cyc_CfAbsexp_mkMallocOp( outer_e); goto _LL100; _LL100: {
void* _temp101= Cyc_CfAbsexp_mkSkipAE(); goto _LL102; _LL102: { void* _temp103=
Cyc_CfAbsexp_mkSkipAE(); goto _LL104; _LL104: if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp106= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp105=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp105->hd=(
void*) _temp99; _temp105->tl= 0; _temp105;})); goto _LL107; _LL107: _temp106.access_path=
0;{ void* _temp110; void* _temp112; struct Cyc_NewControlFlow_AbsSynOne _temp108=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( _temp106,(
struct Cyc_Absyn_Exp*) _check_null( init_exp))); _LL113: _temp112=( void*)
_temp108.inner_exp; goto _LL111; _LL111: _temp110=( void*) _temp108.assigns;
goto _LL109; _LL109: _temp101= _temp112; _temp103= _temp110;}} if( rgn_exp != 0){
struct Cyc_NewControlFlow_AbsSyn _temp114= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env),( struct Cyc_Absyn_Exp*) _check_null( rgn_exp)); goto _LL115; _LL115: {
void* _temp118; void* _temp120; struct Cyc_NewControlFlow_AbsSynOne _temp116=
Cyc_NewControlFlow_meet_absexp( _temp114); _LL121: _temp120=( void*) _temp116.inner_exp;
goto _LL119; _LL119: _temp118=( void*) _temp116.assigns; goto _LL117; _LL117:
_temp101= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp101,
_temp120); _temp103= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp103, _temp118);}}{ void* _temp122= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp101, _temp103)); goto _LL123; _LL123: { struct Cyc_NewControlFlow_AbsSyn
_temp124= Cyc_NewControlFlow_mkSyn_tf( _temp122, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( _temp99))); goto _LL125; _LL125: if( env.access_path
!= 0){ void* _temp126=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( env.access_path))->hd)->r; struct Cyc_Absyn_Exp* _temp134; struct
_tagged_arr* _temp136; struct Cyc_Absyn_Exp* _temp138; _LL128: if(*(( int*)
_temp126) == Cyc_Absyn_Deref_e){ _LL135: _temp134=(( struct Cyc_Absyn_Deref_e_struct*)
_temp126)->f1; goto _LL129;} else{ goto _LL130;} _LL130: if(*(( int*) _temp126)
== Cyc_Absyn_StructArrow_e){ _LL139: _temp138=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp126)->f1; goto _LL137; _LL137: _temp136=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp126)->f2; goto _LL131;} else{ goto _LL132;} _LL132: goto _LL133; _LL129:
env.access_path=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp124.lvalues=({ struct Cyc_List_List* _temp140=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp140->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp99); _temp140->tl= 0; _temp140;}); goto _LL127; _LL131: env.access_path=({
struct Cyc_List_List* _temp141=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp141->hd=( void*) Cyc_Absyn_structmember_exp(
_temp138, _temp136, 0); _temp141->tl=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp141;}); _temp124.lvalues=({ struct Cyc_List_List* _temp142=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp142->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp99); _temp142->tl= 0; _temp142;}); goto _LL127; _LL133:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp143=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp143[ 0]=({ struct
Cyc_Core_Impossible_struct _temp144; _temp144.tag= Cyc_Core_Impossible; _temp144.f1=
_tag_arr("bad malloc access path", sizeof( unsigned char), 23u); _temp144;});
_temp143;})); _LL127:;} return _temp124;}}}}} struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp145=(
void*) e->r; struct Cyc_Absyn_Exp* _temp235; struct Cyc_List_List* _temp237;
struct Cyc_Absyn_Exp* _temp239; void* _temp241; int _temp243; void* _temp245;
void* _temp247; int _temp249; void* _temp251; struct Cyc_Absyn_Exp* _temp253;
void* _temp255; struct _tagged_arr* _temp257; void* _temp259; void* _temp261;
struct Cyc_Absyn_Enumfield* _temp263; struct Cyc_Absyn_Enumdecl* _temp265;
struct _tuple1* _temp267; void* _temp269; struct Cyc_Absyn_Fndecl* _temp271;
struct _tuple1* _temp273; void* _temp275; struct Cyc_Absyn_Vardecl* _temp277;
struct _tuple1* _temp279; void* _temp281; struct Cyc_Absyn_Vardecl* _temp283;
struct _tuple1* _temp285; void* _temp287; struct Cyc_Absyn_Vardecl* _temp289;
struct _tuple1* _temp291; void* _temp293; struct Cyc_Absyn_Vardecl* _temp295;
struct _tuple1* _temp297; struct Cyc_Absyn_Exp* _temp299; struct Cyc_Core_Opt*
_temp301; struct Cyc_Absyn_Exp* _temp303; struct Cyc_Absyn_Exp* _temp305; struct
Cyc_Core_Opt* _temp307; struct Cyc_Absyn_Exp* _temp309; struct Cyc_List_List*
_temp311; void* _temp313; struct Cyc_Absyn_Exp* _temp315; void* _temp317; struct
Cyc_Absyn_VarargCallInfo* _temp319; struct Cyc_List_List* _temp321; struct Cyc_Absyn_Exp*
_temp323; struct Cyc_Absyn_Exp* _temp325; struct Cyc_Absyn_Exp* _temp327; struct
Cyc_Absyn_Exp* _temp329; struct Cyc_Absyn_Exp* _temp331; struct Cyc_Absyn_Exp*
_temp333; void* _temp335; struct Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Exp*
_temp339; struct _tagged_arr* _temp341; struct Cyc_Absyn_Exp* _temp343; struct
_tagged_arr* _temp345; struct Cyc_Absyn_Exp* _temp347; struct Cyc_Absyn_Tunionfield*
_temp349; struct Cyc_Absyn_Tuniondecl* _temp351; struct Cyc_List_List* _temp353;
struct Cyc_Core_Opt* _temp355; struct Cyc_Core_Opt* _temp357; struct Cyc_List_List*
_temp359; struct Cyc_List_List* _temp361; void* _temp363; struct Cyc_Absyn_Structdecl*
_temp365; struct Cyc_List_List* _temp367; struct Cyc_Core_Opt* _temp369; struct
_tuple1* _temp371; struct Cyc_List_List* _temp373; void* _temp375; struct Cyc_Absyn_Exp*
_temp377; struct Cyc_Absyn_Exp* _temp379; struct Cyc_Absyn_Exp* _temp381; struct
Cyc_Absyn_Exp* _temp383; struct Cyc_Absyn_Exp* _temp385; struct Cyc_Absyn_Stmt*
_temp387; struct Cyc_Absyn_Exp* _temp389; struct Cyc_Absyn_Exp* _temp391; struct
Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Exp* _temp395; struct Cyc_Absyn_Vardecl*
_temp397; void* _temp399; struct _tuple1* _temp401; struct _tuple1* _temp403;
struct Cyc_List_List* _temp405; struct Cyc_Absyn_Exp* _temp407; struct Cyc_List_List*
_temp409; struct Cyc_Core_Opt* _temp411; struct Cyc_List_List* _temp413; struct
_tuple2* _temp415; struct Cyc_Absyn_Fndecl* _temp417; struct Cyc_Absyn_Exp*
_temp419; _LL147: if(*(( int*) _temp145) == Cyc_Absyn_NoInstantiate_e){ _LL236:
_temp235=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp145)->f1; goto _LL148;}
else{ goto _LL149;} _LL149: if(*(( int*) _temp145) == Cyc_Absyn_Instantiate_e){
_LL240: _temp239=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp145)->f1; goto
_LL238; _LL238: _temp237=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp145)->f2;
goto _LL150;} else{ goto _LL151;} _LL151: if(*(( int*) _temp145) == Cyc_Absyn_Const_e){
_LL242: _temp241=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp145)->f1; if((
unsigned int) _temp241 > 1u?*(( int*) _temp241) == Cyc_Absyn_Int_c: 0){ _LL246:
_temp245=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp241)->f1; if( _temp245
==( void*) Cyc_Absyn_Signed){ goto _LL244;} else{ goto _LL153;} _LL244: _temp243=((
struct Cyc_Absyn_Int_c_struct*) _temp241)->f2; if( _temp243 == 0){ goto _LL152;}
else{ goto _LL153;}} else{ goto _LL153;}} else{ goto _LL153;} _LL153: if(*(( int*)
_temp145) == Cyc_Absyn_Const_e){ _LL248: _temp247=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp145)->f1; if(( unsigned int) _temp247 > 1u?*(( int*) _temp247) == Cyc_Absyn_Int_c:
0){ _LL252: _temp251=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp247)->f1;
if( _temp251 ==( void*) Cyc_Absyn_Signed){ goto _LL250;} else{ goto _LL155;}
_LL250: _temp249=(( struct Cyc_Absyn_Int_c_struct*) _temp247)->f2; if( _temp249
== 1){ goto _LL154;} else{ goto _LL155;}} else{ goto _LL155;}} else{ goto _LL155;}
_LL155: if(*(( int*) _temp145) == Cyc_Absyn_Sizeofexp_e){ _LL254: _temp253=((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp145)->f1; goto _LL156;} else{ goto
_LL157;} _LL157: if(*(( int*) _temp145) == Cyc_Absyn_Sizeoftyp_e){ _LL256:
_temp255=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp145)->f1; goto
_LL158;} else{ goto _LL159;} _LL159: if(*(( int*) _temp145) == Cyc_Absyn_Offsetof_e){
_LL260: _temp259=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp145)->f1;
goto _LL258; _LL258: _temp257=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp145)->f2;
goto _LL160;} else{ goto _LL161;} _LL161: if(*(( int*) _temp145) == Cyc_Absyn_Const_e){
_LL262: _temp261=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp145)->f1;
goto _LL162;} else{ goto _LL163;} _LL163: if(*(( int*) _temp145) == Cyc_Absyn_Enum_e){
_LL268: _temp267=(( struct Cyc_Absyn_Enum_e_struct*) _temp145)->f1; goto _LL266;
_LL266: _temp265=(( struct Cyc_Absyn_Enum_e_struct*) _temp145)->f2; goto _LL264;
_LL264: _temp263=(( struct Cyc_Absyn_Enum_e_struct*) _temp145)->f3; goto _LL164;}
else{ goto _LL165;} _LL165: if(*(( int*) _temp145) == Cyc_Absyn_Var_e){ _LL274:
_temp273=(( struct Cyc_Absyn_Var_e_struct*) _temp145)->f1; goto _LL270; _LL270:
_temp269=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp145)->f2; if((
unsigned int) _temp269 > 1u?*(( int*) _temp269) == Cyc_Absyn_Funname_b: 0){
_LL272: _temp271=(( struct Cyc_Absyn_Funname_b_struct*) _temp269)->f1; goto
_LL166;} else{ goto _LL167;}} else{ goto _LL167;} _LL167: if(*(( int*) _temp145)
== Cyc_Absyn_Var_e){ _LL280: _temp279=(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f1; goto _LL276; _LL276: _temp275=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f2; if(( unsigned int) _temp275 > 1u?*(( int*) _temp275) == Cyc_Absyn_Global_b:
0){ _LL278: _temp277=(( struct Cyc_Absyn_Global_b_struct*) _temp275)->f1; goto
_LL168;} else{ goto _LL169;}} else{ goto _LL169;} _LL169: if(*(( int*) _temp145)
== Cyc_Absyn_Var_e){ _LL286: _temp285=(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f1; goto _LL282; _LL282: _temp281=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f2; if(( unsigned int) _temp281 > 1u?*(( int*) _temp281) == Cyc_Absyn_Param_b:
0){ _LL284: _temp283=(( struct Cyc_Absyn_Param_b_struct*) _temp281)->f1; goto
_LL170;} else{ goto _LL171;}} else{ goto _LL171;} _LL171: if(*(( int*) _temp145)
== Cyc_Absyn_Var_e){ _LL292: _temp291=(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f1; goto _LL288; _LL288: _temp287=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f2; if(( unsigned int) _temp287 > 1u?*(( int*) _temp287) == Cyc_Absyn_Local_b:
0){ _LL290: _temp289=(( struct Cyc_Absyn_Local_b_struct*) _temp287)->f1; goto
_LL172;} else{ goto _LL173;}} else{ goto _LL173;} _LL173: if(*(( int*) _temp145)
== Cyc_Absyn_Var_e){ _LL298: _temp297=(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f1; goto _LL294; _LL294: _temp293=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f2; if(( unsigned int) _temp293 > 1u?*(( int*) _temp293) == Cyc_Absyn_Pat_b:
0){ _LL296: _temp295=(( struct Cyc_Absyn_Pat_b_struct*) _temp293)->f1; goto
_LL174;} else{ goto _LL175;}} else{ goto _LL175;} _LL175: if(*(( int*) _temp145)
== Cyc_Absyn_AssignOp_e){ _LL304: _temp303=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp145)->f1; goto _LL302; _LL302: _temp301=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp145)->f2; if( _temp301 == 0){ goto _LL300;} else{ goto _LL177;} _LL300:
_temp299=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp145)->f3; goto _LL176;}
else{ goto _LL177;} _LL177: if(*(( int*) _temp145) == Cyc_Absyn_AssignOp_e){
_LL310: _temp309=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp145)->f1; goto
_LL308; _LL308: _temp307=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp145)->f2;
goto _LL306; _LL306: _temp305=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp145)->f3;
goto _LL178;} else{ goto _LL179;} _LL179: if(*(( int*) _temp145) == Cyc_Absyn_Primop_e){
_LL314: _temp313=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp145)->f1;
goto _LL312; _LL312: _temp311=(( struct Cyc_Absyn_Primop_e_struct*) _temp145)->f2;
goto _LL180;} else{ goto _LL181;} _LL181: if(*(( int*) _temp145) == Cyc_Absyn_Cast_e){
_LL318: _temp317=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp145)->f1; goto
_LL316; _LL316: _temp315=(( struct Cyc_Absyn_Cast_e_struct*) _temp145)->f2; goto
_LL182;} else{ goto _LL183;} _LL183: if(*(( int*) _temp145) == Cyc_Absyn_FnCall_e){
_LL324: _temp323=(( struct Cyc_Absyn_FnCall_e_struct*) _temp145)->f1; goto
_LL322; _LL322: _temp321=(( struct Cyc_Absyn_FnCall_e_struct*) _temp145)->f2;
goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_FnCall_e_struct*) _temp145)->f3;
goto _LL184;} else{ goto _LL185;} _LL185: if(*(( int*) _temp145) == Cyc_Absyn_Subscript_e){
_LL328: _temp327=(( struct Cyc_Absyn_Subscript_e_struct*) _temp145)->f1; goto
_LL326; _LL326: _temp325=(( struct Cyc_Absyn_Subscript_e_struct*) _temp145)->f2;
goto _LL186;} else{ goto _LL187;} _LL187: if(*(( int*) _temp145) == Cyc_Absyn_Address_e){
_LL330: _temp329=(( struct Cyc_Absyn_Address_e_struct*) _temp145)->f1; goto
_LL188;} else{ goto _LL189;} _LL189: if(*(( int*) _temp145) == Cyc_Absyn_New_e){
_LL334: _temp333=(( struct Cyc_Absyn_New_e_struct*) _temp145)->f1; goto _LL332;
_LL332: _temp331=(( struct Cyc_Absyn_New_e_struct*) _temp145)->f2; goto _LL190;}
else{ goto _LL191;} _LL191: if(*(( int*) _temp145) == Cyc_Absyn_Malloc_e){
_LL338: _temp337=(( struct Cyc_Absyn_Malloc_e_struct*) _temp145)->f1; goto
_LL336; _LL336: _temp335=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp145)->f2;
goto _LL192;} else{ goto _LL193;} _LL193: if(*(( int*) _temp145) == Cyc_Absyn_Deref_e){
_LL340: _temp339=(( struct Cyc_Absyn_Deref_e_struct*) _temp145)->f1; goto _LL194;}
else{ goto _LL195;} _LL195: if(*(( int*) _temp145) == Cyc_Absyn_StructArrow_e){
_LL344: _temp343=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp145)->f1; goto
_LL342; _LL342: _temp341=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp145)->f2;
goto _LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp145) == Cyc_Absyn_StructMember_e){
_LL348: _temp347=(( struct Cyc_Absyn_StructMember_e_struct*) _temp145)->f1; goto
_LL346; _LL346: _temp345=(( struct Cyc_Absyn_StructMember_e_struct*) _temp145)->f2;
goto _LL198;} else{ goto _LL199;} _LL199: if(*(( int*) _temp145) == Cyc_Absyn_Tunion_e){
_LL358: _temp357=(( struct Cyc_Absyn_Tunion_e_struct*) _temp145)->f1; goto
_LL356; _LL356: _temp355=(( struct Cyc_Absyn_Tunion_e_struct*) _temp145)->f2;
goto _LL354; _LL354: _temp353=(( struct Cyc_Absyn_Tunion_e_struct*) _temp145)->f3;
goto _LL352; _LL352: _temp351=(( struct Cyc_Absyn_Tunion_e_struct*) _temp145)->f4;
goto _LL350; _LL350: _temp349=(( struct Cyc_Absyn_Tunion_e_struct*) _temp145)->f5;
goto _LL200;} else{ goto _LL201;} _LL201: if(*(( int*) _temp145) == Cyc_Absyn_Tuple_e){
_LL360: _temp359=(( struct Cyc_Absyn_Tuple_e_struct*) _temp145)->f1; goto _LL202;}
else{ goto _LL203;} _LL203: if(*(( int*) _temp145) == Cyc_Absyn_AnonStruct_e){
_LL364: _temp363=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp145)->f1;
goto _LL362; _LL362: _temp361=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp145)->f2;
goto _LL204;} else{ goto _LL205;} _LL205: if(*(( int*) _temp145) == Cyc_Absyn_Struct_e){
_LL372: _temp371=(( struct Cyc_Absyn_Struct_e_struct*) _temp145)->f1; goto
_LL370; _LL370: _temp369=(( struct Cyc_Absyn_Struct_e_struct*) _temp145)->f2;
goto _LL368; _LL368: _temp367=(( struct Cyc_Absyn_Struct_e_struct*) _temp145)->f3;
goto _LL366; _LL366: _temp365=(( struct Cyc_Absyn_Struct_e_struct*) _temp145)->f4;
goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp145) == Cyc_Absyn_Array_e){
_LL374: _temp373=(( struct Cyc_Absyn_Array_e_struct*) _temp145)->f1; goto _LL208;}
else{ goto _LL209;} _LL209: if(*(( int*) _temp145) == Cyc_Absyn_Increment_e){
_LL378: _temp377=(( struct Cyc_Absyn_Increment_e_struct*) _temp145)->f1; goto
_LL376; _LL376: _temp375=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp145)->f2; goto _LL210;} else{ goto _LL211;} _LL211: if(*(( int*) _temp145)
== Cyc_Absyn_Throw_e){ _LL380: _temp379=(( struct Cyc_Absyn_Throw_e_struct*)
_temp145)->f1; goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp145)
== Cyc_Absyn_Conditional_e){ _LL386: _temp385=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp145)->f1; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp145)->f2; goto _LL382; _LL382: _temp381=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp145)->f3; goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp145)
== Cyc_Absyn_StmtExp_e){ _LL388: _temp387=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp145)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if(*(( int*) _temp145)
== Cyc_Absyn_SeqExp_e){ _LL392: _temp391=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp145)->f1; goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp145)->f2; goto _LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp145)
== Cyc_Absyn_Comprehension_e){ _LL398: _temp397=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp145)->f1; goto _LL396; _LL396: _temp395=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp145)->f2; goto _LL394; _LL394: _temp393=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp145)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(*(( int*) _temp145)
== Cyc_Absyn_Var_e){ _LL402: _temp401=(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f1; goto _LL400; _LL400: _temp399=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp145)->f2; if( _temp399 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL222;}
else{ goto _LL223;}} else{ goto _LL223;} _LL223: if(*(( int*) _temp145) == Cyc_Absyn_UnknownId_e){
_LL404: _temp403=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp145)->f1; goto
_LL224;} else{ goto _LL225;} _LL225: if(*(( int*) _temp145) == Cyc_Absyn_UnknownCall_e){
_LL408: _temp407=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp145)->f1; goto
_LL406; _LL406: _temp405=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp145)->f2;
goto _LL226;} else{ goto _LL227;} _LL227: if(*(( int*) _temp145) == Cyc_Absyn_UnresolvedMem_e){
_LL412: _temp411=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp145)->f1;
goto _LL410; _LL410: _temp409=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp145)->f2; goto _LL228;} else{ goto _LL229;} _LL229: if(*(( int*) _temp145)
== Cyc_Absyn_CompoundLit_e){ _LL416: _temp415=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp145)->f1; goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp145)->f2; goto _LL230;} else{ goto _LL231;} _LL231: if(*(( int*) _temp145)
== Cyc_Absyn_Codegen_e){ _LL418: _temp417=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp145)->f1; goto _LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp145)
== Cyc_Absyn_Fill_e){ _LL420: _temp419=(( struct Cyc_Absyn_Fill_e_struct*)
_temp145)->f1; goto _LL234;} else{ goto _LL146;} _LL148: _temp239= _temp235;
goto _LL150; _LL150: return Cyc_NewControlFlow_abstract_exp( env, _temp239);
_LL152: env.access_path == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:284 env.access_path==null", sizeof( unsigned char),
47u)); return({ struct Cyc_NewControlFlow_AbsSyn _temp421; _temp421.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp423; _temp423.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp423.assigns=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp423;}); _temp421.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp422; _temp422.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp422.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp422;}); _temp421.lvalues= 0; _temp421;}); _LL154: env.access_path == 0? 0:((
int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:289 env.access_path==null",
sizeof( unsigned char), 47u)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp424; _temp424.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp426;
_temp426.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp426.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp426;});
_temp424.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp425; _temp425.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp425.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp425;}); _temp424.lvalues= 0; _temp424;}); _LL156: goto _LL158; _LL158: goto
_LL160; _LL160: goto _LL162; _LL162: goto _LL164; _LL164: goto _LL166; _LL166:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL168: return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL170: _temp289= _temp283; goto _LL172;
_LL172: _temp295= _temp289; goto _LL174; _LL174: { void* absop= Cyc_NewControlFlow_destruct_path(
env, Cyc_CfAbsexp_mkLocalOp( _temp295)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp427=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp427->hd=( void*) absop; _temp427->tl= 0; _temp427;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL176: { struct Cyc_NewControlFlow_AbsSyn
_temp428= Cyc_NewControlFlow_abstract_exp( env, _temp303); goto _LL429; _LL429: {
struct Cyc_List_List* _temp430= _temp428.lvalues == 0? Cyc_NewControlFlow_use_it_list():
_temp428.lvalues; goto _LL431; _LL431: { struct Cyc_NewControlFlow_AbsSyn
_temp432= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp430), _temp299); goto _LL433; _LL433: { void* _temp436; void* _temp438;
struct Cyc_NewControlFlow_AbsSynOne _temp434= Cyc_NewControlFlow_meet_absexp(
_temp428); _LL439: _temp438=( void*) _temp434.inner_exp; goto _LL437; _LL437:
_temp436=( void*) _temp434.assigns; goto _LL435; _LL435: { void* _temp442; void*
_temp444; struct Cyc_NewControlFlow_AbsSynOne _temp440= Cyc_NewControlFlow_meet_absexp(
_temp432); _LL445: _temp444=( void*) _temp440.inner_exp; goto _LL443; _LL443:
_temp442=( void*) _temp440.assigns; goto _LL441; _LL441: { void* _temp446= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp438, _temp444), _temp442); goto _LL447; _LL447: return Cyc_NewControlFlow_mkSyn_tf(
_temp446, 0, _temp436);}}}}}} _LL178: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp448=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp448->hd=( void*) _temp309; _temp448->tl=({
struct Cyc_List_List* _temp449=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp449->hd=( void*) _temp305; _temp449->tl= 0;
_temp449;}); _temp448;}), 0); _LL180: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp311, 0); _LL182: { int both_leaf= 1;{ void* _temp450=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp315->topt))->v; struct Cyc_List_List* _temp460;
struct Cyc_Absyn_Structdecl** _temp462; struct Cyc_List_List* _temp464; struct
_tuple1* _temp466; struct Cyc_List_List* _temp468; _LL452: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_AnonStructType: 0){ _LL461:
_temp460=(( struct Cyc_Absyn_AnonStructType_struct*) _temp450)->f1; goto _LL453;}
else{ goto _LL454;} _LL454: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450)
== Cyc_Absyn_StructType: 0){ _LL467: _temp466=(( struct Cyc_Absyn_StructType_struct*)
_temp450)->f1; goto _LL465; _LL465: _temp464=(( struct Cyc_Absyn_StructType_struct*)
_temp450)->f2; goto _LL463; _LL463: _temp462=(( struct Cyc_Absyn_StructType_struct*)
_temp450)->f3; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_TupleType: 0){ _LL469: _temp468=((
struct Cyc_Absyn_TupleType_struct*) _temp450)->f1; goto _LL457;} else{ goto
_LL458;} _LL458: goto _LL459; _LL453: goto _LL455; _LL455: goto _LL457; _LL457:
both_leaf= 0; goto _LL451; _LL459: goto _LL451; _LL451:;}{ void* _temp470=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; struct Cyc_List_List* _temp480;
struct Cyc_Absyn_Structdecl** _temp482; struct Cyc_List_List* _temp484; struct
_tuple1* _temp486; struct Cyc_List_List* _temp488; _LL472: if(( unsigned int)
_temp470 > 4u?*(( int*) _temp470) == Cyc_Absyn_AnonStructType: 0){ _LL481:
_temp480=(( struct Cyc_Absyn_AnonStructType_struct*) _temp470)->f1; goto _LL473;}
else{ goto _LL474;} _LL474: if(( unsigned int) _temp470 > 4u?*(( int*) _temp470)
== Cyc_Absyn_StructType: 0){ _LL487: _temp486=(( struct Cyc_Absyn_StructType_struct*)
_temp470)->f1; goto _LL485; _LL485: _temp484=(( struct Cyc_Absyn_StructType_struct*)
_temp470)->f2; goto _LL483; _LL483: _temp482=(( struct Cyc_Absyn_StructType_struct*)
_temp470)->f3; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp470 > 4u?*(( int*) _temp470) == Cyc_Absyn_TupleType: 0){ _LL489: _temp488=((
struct Cyc_Absyn_TupleType_struct*) _temp470)->f1; goto _LL477;} else{ goto
_LL478;} _LL478: goto _LL479; _LL473: goto _LL475; _LL475: goto _LL477; _LL477:
both_leaf= 0; goto _LL471; _LL479: goto _LL471; _LL471:;} if( both_leaf){ return
Cyc_NewControlFlow_abstract_exp( env, _temp315);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp490=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp490->hd=( void*) _temp315; _temp490->tl= 0;
_temp490;}), env.lhsides);} _LL184: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp491=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp491->hd=( void*) _temp323; _temp491->tl=
_temp321; _temp491;}), 0); _LL186: { void* _temp492= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp327->topt))->v); struct Cyc_List_List*
_temp498; _LL494: if(( unsigned int) _temp492 > 4u?*(( int*) _temp492) == Cyc_Absyn_TupleType:
0){ _LL499: _temp498=(( struct Cyc_Absyn_TupleType_struct*) _temp492)->f1; goto
_LL495;} else{ goto _LL496;} _LL496: goto _LL497; _LL495: return Cyc_NewControlFlow_descend_path(
env, e, _temp327); _LL497: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp500=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp500->hd=( void*) _temp327; _temp500->tl=({ struct
Cyc_List_List* _temp501=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp501->hd=( void*) _temp325; _temp501->tl= 0; _temp501;}); _temp500;}), Cyc_NewControlFlow_use_it_list());
_LL493:;} _LL188: { int is_malloc= 0;{ void* _temp502=( void*) _temp329->r;
struct Cyc_Absyn_Structdecl* _temp510; struct Cyc_List_List* _temp512; struct
Cyc_Core_Opt* _temp514; struct _tuple1* _temp516; struct Cyc_List_List* _temp518;
_LL504: if(*(( int*) _temp502) == Cyc_Absyn_Struct_e){ _LL517: _temp516=((
struct Cyc_Absyn_Struct_e_struct*) _temp502)->f1; goto _LL515; _LL515: _temp514=((
struct Cyc_Absyn_Struct_e_struct*) _temp502)->f2; goto _LL513; _LL513: _temp512=((
struct Cyc_Absyn_Struct_e_struct*) _temp502)->f3; goto _LL511; _LL511: _temp510=((
struct Cyc_Absyn_Struct_e_struct*) _temp502)->f4; goto _LL505;} else{ goto
_LL506;} _LL506: if(*(( int*) _temp502) == Cyc_Absyn_Tuple_e){ _LL519: _temp518=((
struct Cyc_Absyn_Tuple_e_struct*) _temp502)->f1; goto _LL507;} else{ goto _LL508;}
_LL508: goto _LL509; _LL505: goto _LL507; _LL507: is_malloc= 1; goto _LL503;
_LL509: goto _LL503; _LL503:;} if( is_malloc){ _temp333= 0; _temp331= _temp329;
goto _LL190;}{ struct Cyc_List_List* _temp520= env.access_path; struct Cyc_List_List
_temp532; struct Cyc_List_List* _temp533; struct Cyc_Absyn_Exp* _temp535; struct
Cyc_Absyn_Exp _temp537; struct Cyc_Position_Segment* _temp538; void* _temp540;
struct Cyc_Absyn_Exp* _temp542; struct Cyc_Absyn_Exp* _temp544; struct Cyc_Core_Opt*
_temp546; struct Cyc_List_List _temp548; struct Cyc_List_List* _temp549; struct
Cyc_Absyn_Exp* _temp551; struct Cyc_Absyn_Exp _temp553; struct Cyc_Position_Segment*
_temp554; void* _temp556; struct Cyc_Absyn_Exp* _temp558; struct Cyc_Core_Opt*
_temp560; struct Cyc_List_List _temp562; struct Cyc_List_List* _temp563; struct
Cyc_Absyn_Exp* _temp565; struct Cyc_Absyn_Exp _temp567; struct Cyc_Position_Segment*
_temp568; void* _temp570; struct _tagged_arr* _temp572; struct Cyc_Absyn_Exp*
_temp574; struct Cyc_Core_Opt* _temp576; _LL522: if( _temp520 == 0){ goto _LL523;}
else{ goto _LL524;} _LL524: if( _temp520 == 0){ goto _LL526;} else{ _temp532=*
_temp520; _LL536: _temp535=( struct Cyc_Absyn_Exp*) _temp532.hd; _temp537=*
_temp535; _LL547: _temp546= _temp537.topt; goto _LL541; _LL541: _temp540=( void*)
_temp537.r; if(*(( int*) _temp540) == Cyc_Absyn_Subscript_e){ _LL545: _temp544=((
struct Cyc_Absyn_Subscript_e_struct*) _temp540)->f1; goto _LL543; _LL543:
_temp542=(( struct Cyc_Absyn_Subscript_e_struct*) _temp540)->f2; goto _LL539;}
else{ goto _LL526;} _LL539: _temp538= _temp537.loc; goto _LL534; _LL534:
_temp533= _temp532.tl; goto _LL525;} _LL526: if( _temp520 == 0){ goto _LL528;}
else{ _temp548=* _temp520; _LL552: _temp551=( struct Cyc_Absyn_Exp*) _temp548.hd;
_temp553=* _temp551; _LL561: _temp560= _temp553.topt; goto _LL557; _LL557:
_temp556=( void*) _temp553.r; if(*(( int*) _temp556) == Cyc_Absyn_Deref_e){
_LL559: _temp558=(( struct Cyc_Absyn_Deref_e_struct*) _temp556)->f1; goto _LL555;}
else{ goto _LL528;} _LL555: _temp554= _temp553.loc; goto _LL550; _LL550:
_temp549= _temp548.tl; goto _LL527;} _LL528: if( _temp520 == 0){ goto _LL530;}
else{ _temp562=* _temp520; _LL566: _temp565=( struct Cyc_Absyn_Exp*) _temp562.hd;
_temp567=* _temp565; _LL577: _temp576= _temp567.topt; goto _LL571; _LL571:
_temp570=( void*) _temp567.r; if(*(( int*) _temp570) == Cyc_Absyn_StructArrow_e){
_LL575: _temp574=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp570)->f1; goto
_LL573; _LL573: _temp572=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp570)->f2;
goto _LL569;} else{ goto _LL530;} _LL569: _temp568= _temp567.loc; goto _LL564;
_LL564: _temp563= _temp562.tl; goto _LL529;} _LL530: goto _LL531; _LL523: env.access_path=({
struct Cyc_List_List* _temp578=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp578->hd=( void*) e; _temp578->tl= env.access_path;
_temp578;}); goto _LL521; _LL525: Cyc_Evexp_eval_const_uint_exp( _temp542) == 0?
0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:413 Evexp::eval_const_uint_exp(e2) == 0",
sizeof( unsigned char), 61u)); _temp549= _temp533; goto _LL527; _LL527: env.access_path=
_temp549; goto _LL521; _LL529: env.access_path=({ struct Cyc_List_List* _temp579=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp579->hd=(
void*) Cyc_Absyn_structmember_exp( _temp574, _temp572, 0); _temp579->tl=
_temp563; _temp579;}); goto _LL521; _LL531:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp580=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp580[ 0]=({ struct Cyc_Core_Impossible_struct _temp581; _temp581.tag= Cyc_Core_Impossible;
_temp581.f1= _tag_arr("abstract_exp: bad access path for &", sizeof(
unsigned char), 36u); _temp581;}); _temp580;})); _LL521:;} return Cyc_NewControlFlow_abstract_exp(
env, _temp329);} _LL190: Cyc_NewControlFlow_add_malloc_root( env, e,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp331->topt))->v); return Cyc_NewControlFlow_abstract_malloc(
env, e, _temp333,( struct Cyc_Absyn_Exp*) _temp331); _LL192: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp335); return Cyc_NewControlFlow_abstract_malloc( env, e, _temp337,
0); _LL194: { struct Cyc_List_List* _temp582= env.access_path; struct Cyc_List_List
_temp588; struct Cyc_List_List* _temp589; struct Cyc_Absyn_Exp* _temp591; struct
Cyc_Absyn_Exp _temp593; struct Cyc_Position_Segment* _temp594; void* _temp596;
struct Cyc_Absyn_Exp* _temp598; struct Cyc_Core_Opt* _temp600; _LL584: if(
_temp582 == 0){ goto _LL586;} else{ _temp588=* _temp582; _LL592: _temp591=(
struct Cyc_Absyn_Exp*) _temp588.hd; _temp593=* _temp591; _LL601: _temp600=
_temp593.topt; goto _LL597; _LL597: _temp596=( void*) _temp593.r; if(*(( int*)
_temp596) == Cyc_Absyn_Address_e){ _LL599: _temp598=(( struct Cyc_Absyn_Address_e_struct*)
_temp596)->f1; goto _LL595;} else{ goto _LL586;} _LL595: _temp594= _temp593.loc;
goto _LL590; _LL590: _temp589= _temp588.tl; goto _LL585;} _LL586: goto _LL587;
_LL585: env.access_path= _temp589;{ struct Cyc_NewControlFlow_AbsSyn _temp602=
Cyc_NewControlFlow_abstract_exp( env, _temp339); goto _LL603; _LL603: if(
_temp602.lvalues == 0){ _temp602.lvalues= Cyc_NewControlFlow_use_it_list();}
return _temp602;} _LL587: return Cyc_NewControlFlow_descend_path( env, e,
_temp339); _LL583:;} _LL196: { void* _temp604= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp343->topt))->v); struct Cyc_Absyn_PtrInfo
_temp610; struct Cyc_Absyn_Conref* _temp612; struct Cyc_Absyn_Tqual _temp614;
struct Cyc_Absyn_Conref* _temp616; void* _temp618; void* _temp620; _LL606: if((
unsigned int) _temp604 > 4u?*(( int*) _temp604) == Cyc_Absyn_PointerType: 0){
_LL611: _temp610=(( struct Cyc_Absyn_PointerType_struct*) _temp604)->f1; _LL621:
_temp620=( void*) _temp610.elt_typ; goto _LL619; _LL619: _temp618=( void*)
_temp610.rgn_typ; goto _LL617; _LL617: _temp616= _temp610.nullable; goto _LL615;
_LL615: _temp614= _temp610.tq; goto _LL613; _LL613: _temp612= _temp610.bounds;
goto _LL607;} else{ goto _LL608;} _LL608: goto _LL609; _LL607:{ void* _temp622=
Cyc_Tcutil_compress( _temp620); struct Cyc_List_List* _temp634; struct Cyc_Absyn_Structdecl**
_temp636; struct Cyc_List_List* _temp638; struct _tuple1* _temp640; struct Cyc_List_List*
_temp642; struct Cyc_Absyn_Uniondecl** _temp644; struct Cyc_List_List* _temp646;
struct _tuple1* _temp648; _LL624: if(( unsigned int) _temp622 > 4u?*(( int*)
_temp622) == Cyc_Absyn_AnonStructType: 0){ _LL635: _temp634=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp622)->f1; goto _LL625;} else{ goto _LL626;} _LL626: if(( unsigned int)
_temp622 > 4u?*(( int*) _temp622) == Cyc_Absyn_StructType: 0){ _LL641: _temp640=((
struct Cyc_Absyn_StructType_struct*) _temp622)->f1; goto _LL639; _LL639:
_temp638=(( struct Cyc_Absyn_StructType_struct*) _temp622)->f2; goto _LL637;
_LL637: _temp636=(( struct Cyc_Absyn_StructType_struct*) _temp622)->f3; goto
_LL627;} else{ goto _LL628;} _LL628: if(( unsigned int) _temp622 > 4u?*(( int*)
_temp622) == Cyc_Absyn_AnonUnionType: 0){ _LL643: _temp642=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp622)->f1; goto _LL629;} else{ goto _LL630;} _LL630: if(( unsigned int)
_temp622 > 4u?*(( int*) _temp622) == Cyc_Absyn_UnionType: 0){ _LL649: _temp648=((
struct Cyc_Absyn_UnionType_struct*) _temp622)->f1; goto _LL647; _LL647: _temp646=((
struct Cyc_Absyn_UnionType_struct*) _temp622)->f2; goto _LL645; _LL645: _temp644=((
struct Cyc_Absyn_UnionType_struct*) _temp622)->f3; goto _LL631;} else{ goto
_LL632;} _LL632: goto _LL633; _LL625: goto _LL627; _LL627: return Cyc_NewControlFlow_descend_path(
env, e, _temp343); _LL629: goto _LL631; _LL631: { struct Cyc_NewControlFlow_AbsSyn
_temp650= Cyc_NewControlFlow_abstract_exp( env, _temp343); goto _LL651; _LL651:
_temp650.lvalues= Cyc_NewControlFlow_use_it_list(); return _temp650;} _LL633:
goto _LL623; _LL623:;} goto _LL609; _LL609:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp652=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp652[ 0]=({ struct Cyc_Core_Impossible_struct _temp653; _temp653.tag= Cyc_Core_Impossible;
_temp653.f1= _tag_arr("NewControlFlow: bad type in StructArrow", sizeof(
unsigned char), 40u); _temp653;}); _temp652;})); _LL605:;} _LL198: { void*
_temp654= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp347->topt))->v); struct Cyc_List_List* _temp666; struct Cyc_Absyn_Structdecl**
_temp668; struct Cyc_List_List* _temp670; struct _tuple1* _temp672; struct Cyc_List_List*
_temp674; struct Cyc_Absyn_Uniondecl** _temp676; struct Cyc_List_List* _temp678;
struct _tuple1* _temp680; _LL656: if(( unsigned int) _temp654 > 4u?*(( int*)
_temp654) == Cyc_Absyn_AnonStructType: 0){ _LL667: _temp666=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp654)->f1; goto _LL657;} else{ goto _LL658;} _LL658: if(( unsigned int)
_temp654 > 4u?*(( int*) _temp654) == Cyc_Absyn_StructType: 0){ _LL673: _temp672=((
struct Cyc_Absyn_StructType_struct*) _temp654)->f1; goto _LL671; _LL671:
_temp670=(( struct Cyc_Absyn_StructType_struct*) _temp654)->f2; goto _LL669;
_LL669: _temp668=(( struct Cyc_Absyn_StructType_struct*) _temp654)->f3; goto
_LL659;} else{ goto _LL660;} _LL660: if(( unsigned int) _temp654 > 4u?*(( int*)
_temp654) == Cyc_Absyn_AnonUnionType: 0){ _LL675: _temp674=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp654)->f1; goto _LL661;} else{ goto _LL662;} _LL662: if(( unsigned int)
_temp654 > 4u?*(( int*) _temp654) == Cyc_Absyn_UnionType: 0){ _LL681: _temp680=((
struct Cyc_Absyn_UnionType_struct*) _temp654)->f1; goto _LL679; _LL679: _temp678=((
struct Cyc_Absyn_UnionType_struct*) _temp654)->f2; goto _LL677; _LL677: _temp676=((
struct Cyc_Absyn_UnionType_struct*) _temp654)->f3; goto _LL663;} else{ goto
_LL664;} _LL664: goto _LL665; _LL657: goto _LL659; _LL659: return Cyc_NewControlFlow_descend_path(
env, e, _temp347); _LL661: goto _LL663; _LL663: { struct Cyc_NewControlFlow_AbsSyn
_temp682= Cyc_NewControlFlow_abstract_exp( env, _temp347); goto _LL683; _LL683:
_temp682.lvalues= Cyc_NewControlFlow_use_it_list(); return _temp682;} _LL665:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp684=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp684[ 0]=({ struct
Cyc_Core_Impossible_struct _temp685; _temp685.tag= Cyc_Core_Impossible; _temp685.f1=
_tag_arr("NewControlFlow: bad type in StructMember", sizeof( unsigned char), 41u);
_temp685;}); _temp684;})); _LL655:;} _LL200: env.access_path == 0? 0:(( int(*)(
struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:483 env.access_path == null",
sizeof( unsigned char), 49u)); if( _temp353 == 0){ return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _temp359= _temp353; goto _LL202; _LL202: {
struct Cyc_List_List* _temp686= env.access_path; struct Cyc_List_List _temp694;
struct Cyc_List_List* _temp695; struct Cyc_Absyn_Exp* _temp697; struct Cyc_Absyn_Exp
_temp699; struct Cyc_Position_Segment* _temp700; void* _temp702; struct Cyc_Absyn_Exp*
_temp704; struct Cyc_Absyn_Exp* _temp706; struct Cyc_Core_Opt* _temp708; _LL688:
if( _temp686 == 0){ goto _LL689;} else{ goto _LL690;} _LL690: if( _temp686 == 0){
goto _LL692;} else{ _temp694=* _temp686; _LL698: _temp697=( struct Cyc_Absyn_Exp*)
_temp694.hd; _temp699=* _temp697; _LL709: _temp708= _temp699.topt; goto _LL703;
_LL703: _temp702=( void*) _temp699.r; if(*(( int*) _temp702) == Cyc_Absyn_Subscript_e){
_LL707: _temp706=(( struct Cyc_Absyn_Subscript_e_struct*) _temp702)->f1; goto
_LL705; _LL705: _temp704=(( struct Cyc_Absyn_Subscript_e_struct*) _temp702)->f2;
goto _LL701;} else{ goto _LL692;} _LL701: _temp700= _temp699.loc; goto _LL696;
_LL696: _temp695= _temp694.tl; goto _LL691;} _LL692: goto _LL693; _LL689: { void*
_temp710= Cyc_CfAbsexp_mkSkipAE(); goto _LL711; _LL711: { void* _temp712= Cyc_CfAbsexp_mkSkipAE();
goto _LL713; _LL713:{ int j= 0; for( 0; _temp359 != 0;( _temp359=(( struct Cyc_List_List*)
_check_null( _temp359))->tl, ++ j)){ struct Cyc_List_List* _temp714= 0; goto
_LL715; _LL715:{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=((
struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp714=({ struct Cyc_List_List*
_temp716=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp716->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp716->tl= _temp714;
_temp716;});} else{ _temp714=({ struct Cyc_List_List* _temp717=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp717->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp718=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp718[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp719; _temp719.tag= Cyc_CfFlowInfo_TupleF;
_temp719.f1= j; _temp719;}); _temp718;})); _temp717->tl= _temp714; _temp717;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp720= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp714),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp359))->hd); goto _LL721; _LL721: { void* _temp724; void* _temp726; struct
Cyc_NewControlFlow_AbsSynOne _temp722= Cyc_NewControlFlow_meet_absexp( _temp720);
_LL727: _temp726=( void*) _temp722.inner_exp; goto _LL725; _LL725: _temp724=(
void*) _temp722.assigns; goto _LL723; _LL723: _temp710= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp710, _temp726); _temp712= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp712, _temp724);}}}} return Cyc_NewControlFlow_mkSyn_tf(
_temp710, 0, _temp712);}} _LL691: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp704); struct Cyc_NewControlFlow_AbsEnv _temp728= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL729; _LL729: _temp728.access_path= 0; env.access_path= _temp695;{
void* _temp730= Cyc_CfAbsexp_mkSkipAE(); goto _LL731; _LL731: { void* _temp732=
Cyc_CfAbsexp_mkSkipAE(); goto _LL733; _LL733:{ int j= 0; for( 0; _temp359 != 0;(
_temp359=(( struct Cyc_List_List*) _check_null( _temp359))->tl, ++ j)){ if( i ==
j){ void* _temp736; void* _temp738; struct Cyc_NewControlFlow_AbsSynOne _temp734=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp359))->hd)); _LL739:
_temp738=( void*) _temp734.inner_exp; goto _LL737; _LL737: _temp736=( void*)
_temp734.assigns; goto _LL735; _LL735: _temp732= _temp736; _temp730= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp730, _temp738);} else{ void* _temp742; void*
_temp744; struct Cyc_NewControlFlow_AbsSynOne _temp740= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp728,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp359))->hd)); _LL745: _temp744=( void*) _temp740.inner_exp;
goto _LL743; _LL743: _temp742=( void*) _temp740.assigns; goto _LL741; _LL741:
_temp730= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp730,
_temp744);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp730,({ struct Cyc_List_List*
_temp746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp746->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp746->tl= 0; _temp746;}),
_temp732);}}} _LL693:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp747=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp747[ 0]=({ struct Cyc_Core_Impossible_struct _temp748; _temp748.tag= Cyc_Core_Impossible;
_temp748.f1= _tag_arr("abstract_exp: unexpected access path to Tuple", sizeof(
unsigned char), 46u); _temp748;}); _temp747;})); _LL687:;} _LL204: _temp367=
_temp361; goto _LL206; _LL206: { struct Cyc_List_List* _temp749= env.access_path;
struct Cyc_List_List _temp757; struct Cyc_List_List* _temp758; struct Cyc_Absyn_Exp*
_temp760; struct Cyc_Absyn_Exp _temp762; struct Cyc_Position_Segment* _temp763;
void* _temp765; struct _tagged_arr* _temp767; struct Cyc_Absyn_Exp* _temp769;
struct Cyc_Core_Opt* _temp771; _LL751: if( _temp749 == 0){ goto _LL752;} else{
goto _LL753;} _LL753: if( _temp749 == 0){ goto _LL755;} else{ _temp757=*
_temp749; _LL761: _temp760=( struct Cyc_Absyn_Exp*) _temp757.hd; _temp762=*
_temp760; _LL772: _temp771= _temp762.topt; goto _LL766; _LL766: _temp765=( void*)
_temp762.r; if(*(( int*) _temp765) == Cyc_Absyn_StructMember_e){ _LL770:
_temp769=(( struct Cyc_Absyn_StructMember_e_struct*) _temp765)->f1; goto _LL768;
_LL768: _temp767=(( struct Cyc_Absyn_StructMember_e_struct*) _temp765)->f2; goto
_LL764;} else{ goto _LL755;} _LL764: _temp763= _temp762.loc; goto _LL759; _LL759:
_temp758= _temp757.tl; goto _LL754;} _LL755: goto _LL756; _LL752: { void*
_temp773= Cyc_CfAbsexp_mkSkipAE(); goto _LL774; _LL774: { void* _temp775= Cyc_CfAbsexp_mkSkipAE();
goto _LL776; _LL776: for( 0; _temp367 != 0; _temp367=(( struct Cyc_List_List*)
_check_null( _temp367))->tl){ struct Cyc_List_List* _temp777= 0; goto _LL778;
_LL778:{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=(( struct
Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)((
struct Cyc_List_List*) _check_null( ls))->hd)){ _temp777=({ struct Cyc_List_List*
_temp779=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp779->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp779->tl= _temp777;
_temp779;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp367))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp780=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp786; struct _tagged_arr*
_temp788; _LL782: if(*(( int*) _temp780) == Cyc_Absyn_ArrayElement){ _LL787:
_temp786=(( struct Cyc_Absyn_ArrayElement_struct*) _temp780)->f1; goto _LL783;}
else{ goto _LL784;} _LL784: if(*(( int*) _temp780) == Cyc_Absyn_FieldName){
_LL789: _temp788=(( struct Cyc_Absyn_FieldName_struct*) _temp780)->f1; goto
_LL785;} else{ goto _LL781;} _LL783:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp790=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp790[ 0]=({ struct Cyc_Core_Impossible_struct _temp791; _temp791.tag= Cyc_Core_Impossible;
_temp791.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp791;}); _temp790;})); _LL785: _temp777=({ struct Cyc_List_List* _temp792=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp792->hd=(
void*) Cyc_CfAbsexp_mkMemberOp(( void*)(( struct Cyc_List_List*) _check_null( ls))->hd,(
void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp793=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp793[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp794; _temp794.tag= Cyc_CfFlowInfo_StructF;
_temp794.f1= _temp788; _temp794;}); _temp793;})); _temp792->tl= _temp777;
_temp792;}); goto _LL781; _LL781:;}}}}{ void** _temp795=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp367))->hd)).f2)->topt))->v); goto _LL796; _LL796: { struct Cyc_NewControlFlow_AbsSyn
_temp797= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp777),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f2);
goto _LL798; _LL798: { void* _temp801; void* _temp803; struct Cyc_NewControlFlow_AbsSynOne
_temp799= Cyc_NewControlFlow_meet_absexp( _temp797); _LL804: _temp803=( void*)
_temp799.inner_exp; goto _LL802; _LL802: _temp801=( void*) _temp799.assigns;
goto _LL800; _LL800: _temp773= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp773, _temp803); _temp775= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp775, _temp801);}}}} return Cyc_NewControlFlow_mkSyn_tf( _temp773, 0,
_temp775);}} _LL754: { struct Cyc_NewControlFlow_AbsEnv _temp805= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL806; _LL806: _temp805.access_path= 0; env.access_path= _temp758;{
void* _temp807= Cyc_CfAbsexp_mkSkipAE(); goto _LL808; _LL808: { void* _temp809=
Cyc_CfAbsexp_mkSkipAE(); goto _LL810; _LL810: for( 0; _temp367 != 0; _temp367=((
struct Cyc_List_List*) _check_null( _temp367))->tl){ int used= 0;{ struct Cyc_List_List*
ds=(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f1;
for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void*
_temp811=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp817; struct _tagged_arr* _temp819; _LL813: if(*(( int*) _temp811) == Cyc_Absyn_ArrayElement){
_LL818: _temp817=(( struct Cyc_Absyn_ArrayElement_struct*) _temp811)->f1; goto
_LL814;} else{ goto _LL815;} _LL815: if(*(( int*) _temp811) == Cyc_Absyn_FieldName){
_LL820: _temp819=(( struct Cyc_Absyn_FieldName_struct*) _temp811)->f1; goto
_LL816;} else{ goto _LL812;} _LL814:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp821=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp821[ 0]=({ struct Cyc_Core_Impossible_struct _temp822; _temp822.tag= Cyc_Core_Impossible;
_temp822.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp822;}); _temp821;})); _LL816: if( Cyc_String_zstrptrcmp( _temp767, _temp819)
== 0){ used= 1;} goto _LL812; _LL812:;}} if( used){ void* _temp825; void*
_temp827; struct Cyc_NewControlFlow_AbsSynOne _temp823= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp367))->hd)).f2)); _LL828: _temp827=( void*) _temp823.inner_exp;
goto _LL826; _LL826: _temp825=( void*) _temp823.assigns; goto _LL824; _LL824:
_temp809= _temp825; _temp807= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp807, _temp827);} else{ void* _temp831; void* _temp833; struct Cyc_NewControlFlow_AbsSynOne
_temp829= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp805,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f2));
_LL834: _temp833=( void*) _temp829.inner_exp; goto _LL832; _LL832: _temp831=(
void*) _temp829.assigns; goto _LL830; _LL830: _temp807= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp807, _temp833);}} return Cyc_NewControlFlow_mkSyn_tf(
_temp807, 0, _temp809);}}} _LL756:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp835=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp835[ 0]=({ struct Cyc_Core_Impossible_struct _temp836; _temp836.tag= Cyc_Core_Impossible;
_temp836.f1= _tag_arr("abstract_exp: unexpected access path to Struct", sizeof(
unsigned char), 47u); _temp836;}); _temp835;})); _LL750:;} _LL208: return Cyc_NewControlFlow_use_exp_unordered(
env,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp373), 0); _LL210: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp837=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp837->hd=( void*) _temp377; _temp837->tl= 0;
_temp837;}), 0); _LL212: { struct Cyc_NewControlFlow_AbsSyn _temp838= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp379); goto _LL839; _LL839: return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
_temp838)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL214: { struct Cyc_List_List* _temp842; struct Cyc_NewControlFlow_AbsSynOne
_temp844; struct Cyc_NewControlFlow_AbsSynOne _temp846; struct Cyc_NewControlFlow_AbsSyn
_temp840= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp385); _LL847: _temp846= _temp840.when_true; goto _LL845; _LL845: _temp844=
_temp840.when_false; goto _LL843; _LL843: _temp842= _temp840.lvalues; goto
_LL841; _LL841: { struct Cyc_List_List* _temp850; struct Cyc_NewControlFlow_AbsSynOne
_temp852; struct Cyc_NewControlFlow_AbsSynOne _temp854; struct Cyc_NewControlFlow_AbsSyn
_temp848= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp383); _LL855: _temp854= _temp848.when_true; goto _LL853; _LL853: _temp852=
_temp848.when_false; goto _LL851; _LL851: _temp850= _temp848.lvalues; goto
_LL849; _LL849: { struct Cyc_List_List* _temp858; struct Cyc_NewControlFlow_AbsSynOne
_temp860; struct Cyc_NewControlFlow_AbsSynOne _temp862; struct Cyc_NewControlFlow_AbsSyn
_temp856= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp381); _LL863: _temp862= _temp856.when_true; goto _LL861; _LL861: _temp860=
_temp856.when_false; goto _LL859; _LL859: _temp858= _temp856.lvalues; goto
_LL857; _LL857: { void* _temp864= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp846.inner_exp,( void*) _temp846.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp854.inner_exp,(
void*) _temp854.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp844.inner_exp,(
void*) _temp844.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp862.inner_exp,( void*) _temp862.assigns))); goto _LL865; _LL865: {
void* _temp866= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp846.inner_exp,( void*) _temp846.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp852.inner_exp,( void*) _temp852.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp844.inner_exp,( void*) _temp844.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp860.inner_exp,(
void*) _temp860.assigns))); goto _LL867; _LL867: return({ struct Cyc_NewControlFlow_AbsSyn
_temp868; _temp868.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp870;
_temp870.inner_exp=( void*) _temp864; _temp870.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp870;}); _temp868.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp869; _temp869.inner_exp=( void*) _temp866; _temp869.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp869;}); _temp868.lvalues= 0; _temp868;});}}}}}
_LL216: { struct Cyc_NewControlFlow_AbsEnv _temp871= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL872; _LL872: _temp871.access_path= 0; Cyc_NewControlFlow_cf_prepass(
_temp871, _temp387);{ struct Cyc_Absyn_Stmt* _temp873= _temp387; goto _LL874;
_LL874: while( 1) { void* _temp875=( void*) _temp873->r; struct Cyc_Absyn_Stmt*
_temp885; struct Cyc_Absyn_Stmt* _temp887; struct Cyc_Absyn_Stmt* _temp889;
struct Cyc_Absyn_Decl* _temp891; struct Cyc_Absyn_Exp* _temp893; _LL877: if((
unsigned int) _temp875 > 1u?*(( int*) _temp875) == Cyc_Absyn_Seq_s: 0){ _LL888:
_temp887=(( struct Cyc_Absyn_Seq_s_struct*) _temp875)->f1; goto _LL886; _LL886:
_temp885=(( struct Cyc_Absyn_Seq_s_struct*) _temp875)->f2; goto _LL878;} else{
goto _LL879;} _LL879: if(( unsigned int) _temp875 > 1u?*(( int*) _temp875) ==
Cyc_Absyn_Decl_s: 0){ _LL892: _temp891=(( struct Cyc_Absyn_Decl_s_struct*)
_temp875)->f1; goto _LL890; _LL890: _temp889=(( struct Cyc_Absyn_Decl_s_struct*)
_temp875)->f2; goto _LL880;} else{ goto _LL881;} _LL881: if(( unsigned int)
_temp875 > 1u?*(( int*) _temp875) == Cyc_Absyn_Exp_s: 0){ _LL894: _temp893=((
struct Cyc_Absyn_Exp_s_struct*) _temp875)->f1; goto _LL882;} else{ goto _LL883;}
_LL883: goto _LL884; _LL878: _temp873= _temp885; continue; _LL880: _temp873=
_temp889; continue; _LL882: { struct Cyc_NewControlFlow_AbsSyn _temp895= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp893); goto _LL896; _LL896: { void*
_temp899; void* _temp901; struct Cyc_NewControlFlow_AbsSynOne _temp897= Cyc_NewControlFlow_meet_absexp(
_temp895); _LL902: _temp901=( void*) _temp897.inner_exp; goto _LL900; _LL900:
_temp899=( void*) _temp897.assigns; goto _LL898; _LL898:( void*)((( struct Cyc_List_List*)
_check_null(( Cyc_NewControlFlow_get_stmt_annot( _temp873))->absexps))->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp901, _temp899));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp387), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}} _LL884:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp903=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp903[ 0]=({ struct Cyc_Core_Impossible_struct _temp904; _temp904.tag= Cyc_Core_Impossible;
_temp904.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp904;}); _temp903;})); _LL876:;}}} _LL218: { struct Cyc_NewControlFlow_AbsEnv
_temp905= Cyc_NewControlFlow_change_lhs( env, 0); goto _LL906; _LL906: _temp905.access_path=
0;{ struct Cyc_NewControlFlow_AbsSyn _temp907= Cyc_NewControlFlow_abstract_exp(
_temp905, _temp391); goto _LL908; _LL908: { struct Cyc_NewControlFlow_AbsSyn
_temp909= Cyc_NewControlFlow_abstract_exp( env, _temp389); goto _LL910; _LL910: {
struct Cyc_NewControlFlow_AbsSynOne _temp911= Cyc_NewControlFlow_meet_absexp(
_temp907); goto _LL912; _LL912: return({ struct Cyc_NewControlFlow_AbsSyn
_temp913; _temp913.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp915;
_temp915.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp911.inner_exp,( void*)( _temp909.when_true).inner_exp); _temp915.assigns=(
void*)(( void*)( _temp909.when_true).assigns); _temp915;}); _temp913.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp914; _temp914.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp911.inner_exp,( void*)( _temp909.when_false).inner_exp);
_temp914.assigns=( void*)(( void*)( _temp909.when_false).assigns); _temp914;});
_temp913.lvalues= 0; _temp913;});}}}} _LL220: env.access_path == 0? 0:(( int(*)(
struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:703 env.access_path==null",
sizeof( unsigned char), 47u)); Cyc_NewControlFlow_add_var_root( env, _temp397);(
env.shared)->comprehension_vars=({ struct Cyc_List_List* _temp916=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp916->hd=( void*) _temp397;
_temp916->tl=( env.shared)->comprehension_vars; _temp916;});{ struct Cyc_NewControlFlow_AbsEnv
_temp917= Cyc_NewControlFlow_use_it_env( env); goto _LL918; _LL918: { void*
_temp921; void* _temp923; struct Cyc_NewControlFlow_AbsSynOne _temp919= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp917, _temp395)); _LL924: _temp923=( void*)
_temp919.inner_exp; goto _LL922; _LL922: _temp921=( void*) _temp919.assigns;
goto _LL920; _LL920: { void* _temp927; void* _temp929; struct Cyc_NewControlFlow_AbsSynOne
_temp925= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp917, _temp393)); _LL930: _temp929=( void*) _temp925.inner_exp; goto _LL928;
_LL928: _temp927=( void*) _temp925.assigns; goto _LL926; _LL926: { void*
_temp931= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp923, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp921, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp397), Cyc_CfAbsexp_mkUnknownOp()),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp929, _temp927), Cyc_CfAbsexp_mkSkipAE()))));
goto _LL932; _LL932: return Cyc_NewControlFlow_mkSyn_tf( _temp931, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}}} _LL222: goto _LL224; _LL224: goto _LL226;
_LL226: goto _LL228; _LL228: goto _LL230; _LL230: goto _LL232; _LL232: goto
_LL234; _LL234:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp933=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp933[ 0]=({ struct Cyc_Core_Impossible_struct _temp934; _temp934.tag= Cyc_Core_Impossible;
_temp934.f1= _tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char),
34u); _temp934;}); _temp933;})); _LL146:;} void* Cyc_NewControlFlow_abstract_exp_top(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp937;
void* _temp939; struct Cyc_NewControlFlow_AbsSynOne _temp935= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, e)); _LL940: _temp939=( void*) _temp935.inner_exp;
goto _LL938; _LL938: _temp937=( void*) _temp935.assigns; goto _LL936; _LL936:
return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp939, _temp937);}
struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn _temp941= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), e); goto _LL942; _LL942: return({ struct
_tuple0 _temp943; _temp943.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*)( _temp941.when_true).inner_exp,( void*)( _temp941.when_true).assigns);
_temp943.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)(
_temp941.when_false).inner_exp,( void*)( _temp941.when_false).assigns); _temp943;});}
struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_List_List* scs){ struct Cyc_List_List* _temp944= 0; goto _LL945;
_LL945: for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){
struct Cyc_Absyn_Switch_clause _temp948; struct Cyc_Position_Segment* _temp949;
struct Cyc_Absyn_Stmt* _temp951; struct Cyc_Absyn_Exp* _temp953; struct Cyc_Core_Opt*
_temp955; struct Cyc_Absyn_Pat* _temp957; struct Cyc_Absyn_Switch_clause*
_temp946=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp948=* _temp946; _LL958: _temp957= _temp948.pattern; goto _LL956;
_LL956: _temp955= _temp948.pat_vars; goto _LL954; _LL954: _temp953= _temp948.where_clause;
goto _LL952; _LL952: _temp951= _temp948.body; goto _LL950; _LL950: _temp949=
_temp948.loc; goto _LL947; _LL947: if( _temp955 == 0){( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp959=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp959[ 0]=({ struct
Cyc_Core_Impossible_struct _temp960; _temp960.tag= Cyc_Core_Impossible; _temp960.f1=
_tag_arr("switch clause vds not set", sizeof( unsigned char), 26u); _temp960;});
_temp959;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp955))->v; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd);}} if( _temp953 != 0){ void*
_temp963; void* _temp965; struct _tuple0 _temp961= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp953)); _LL966: _temp965= _temp961.f1;
goto _LL964; _LL964: _temp963= _temp961.f2; goto _LL962; _LL962: _temp944=({
struct Cyc_List_List* _temp967=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp967->hd=( void*) _temp965; _temp967->tl=({ struct
Cyc_List_List* _temp968=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp968->hd=( void*) _temp963; _temp968->tl= _temp944; _temp968;}); _temp967;});}
Cyc_NewControlFlow_cf_prepass( env, _temp951);} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp944);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s){ env.lhsides ==
0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:755 env.lhsides == null",
sizeof( unsigned char), 45u)); env.access_path == 0? 0:(( int(*)( struct
_tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:756 env.access_path == null",
sizeof( unsigned char), 49u));{ void* _temp969=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1021; struct Cyc_Absyn_Stmt* _temp1023; struct Cyc_Absyn_Stmt* _temp1025;
struct Cyc_Absyn_Exp* _temp1027; struct Cyc_Absyn_Exp* _temp1029; struct Cyc_Absyn_Stmt*
_temp1031; struct Cyc_Absyn_Stmt* _temp1033; struct Cyc_Absyn_Exp* _temp1035;
struct _tuple3 _temp1037; struct Cyc_Absyn_Stmt* _temp1039; struct Cyc_Absyn_Exp*
_temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct Cyc_Absyn_Stmt* _temp1045;
struct _tuple3 _temp1047; struct Cyc_Absyn_Stmt* _temp1049; struct Cyc_Absyn_Exp*
_temp1051; struct Cyc_Absyn_Stmt* _temp1053; struct Cyc_Absyn_Stmt* _temp1055;
struct Cyc_Absyn_Stmt* _temp1057; struct _tagged_arr* _temp1059; struct Cyc_Absyn_Stmt*
_temp1061; struct _tagged_arr* _temp1063; struct Cyc_Absyn_Stmt* _temp1065;
struct _tuple3 _temp1067; struct Cyc_Absyn_Stmt* _temp1069; struct Cyc_Absyn_Exp*
_temp1071; struct _tuple3 _temp1073; struct Cyc_Absyn_Stmt* _temp1075; struct
Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_Exp* _temp1079; struct Cyc_List_List*
_temp1081; struct Cyc_Absyn_Exp* _temp1083; struct Cyc_Absyn_Switch_clause**
_temp1085; struct Cyc_List_List* _temp1087; struct Cyc_Absyn_Stmt* _temp1089;
struct Cyc_Absyn_Decl* _temp1091; struct Cyc_Absyn_Decl _temp1093; struct Cyc_Position_Segment*
_temp1094; void* _temp1096; struct Cyc_Absyn_Vardecl* _temp1098; struct Cyc_Absyn_Stmt*
_temp1100; struct Cyc_Absyn_Decl* _temp1102; struct Cyc_Absyn_Decl _temp1104;
struct Cyc_Position_Segment* _temp1105; void* _temp1107; int _temp1109; struct
Cyc_Absyn_Exp* _temp1111; struct Cyc_Core_Opt* _temp1113; struct Cyc_Core_Opt*
_temp1115; struct Cyc_Core_Opt _temp1117; struct Cyc_List_List* _temp1118;
struct Cyc_Absyn_Pat* _temp1120; struct Cyc_Absyn_Stmt* _temp1122; struct Cyc_Absyn_Decl*
_temp1124; struct Cyc_Absyn_Decl _temp1126; struct Cyc_Position_Segment*
_temp1127; void* _temp1129; struct Cyc_List_List* _temp1131; struct Cyc_Absyn_Stmt*
_temp1133; struct _tagged_arr* _temp1135; struct Cyc_List_List* _temp1137;
struct Cyc_Absyn_Stmt* _temp1139; struct Cyc_Absyn_Stmt* _temp1141; struct Cyc_Absyn_Vardecl*
_temp1143; struct Cyc_Absyn_Tvar* _temp1145; struct Cyc_Absyn_Stmt* _temp1147;
struct Cyc_Absyn_Decl* _temp1149; struct Cyc_List_List* _temp1151; struct Cyc_Absyn_Exp*
_temp1153; struct Cyc_Absyn_Stmt* _temp1155; struct Cyc_Absyn_Stmt* _temp1157;
_LL971: if( _temp969 ==( void*) Cyc_Absyn_Skip_s){ goto _LL972;} else{ goto
_LL973;} _LL973: if(( unsigned int) _temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Exp_s:
0){ _LL1022: _temp1021=(( struct Cyc_Absyn_Exp_s_struct*) _temp969)->f1; goto
_LL974;} else{ goto _LL975;} _LL975: if(( unsigned int) _temp969 > 1u?*(( int*)
_temp969) == Cyc_Absyn_Seq_s: 0){ _LL1026: _temp1025=(( struct Cyc_Absyn_Seq_s_struct*)
_temp969)->f1; goto _LL1024; _LL1024: _temp1023=(( struct Cyc_Absyn_Seq_s_struct*)
_temp969)->f2; goto _LL976;} else{ goto _LL977;} _LL977: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Return_s: 0){ _LL1028: _temp1027=((
struct Cyc_Absyn_Return_s_struct*) _temp969)->f1; if( _temp1027 == 0){ goto
_LL978;} else{ goto _LL979;}} else{ goto _LL979;} _LL979: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Return_s: 0){ _LL1030: _temp1029=((
struct Cyc_Absyn_Return_s_struct*) _temp969)->f1; goto _LL980;} else{ goto
_LL981;} _LL981: if(( unsigned int) _temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_IfThenElse_s:
0){ _LL1036: _temp1035=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp969)->f1;
goto _LL1034; _LL1034: _temp1033=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp969)->f2; goto _LL1032; _LL1032: _temp1031=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp969)->f3; goto _LL982;} else{ goto _LL983;} _LL983: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Do_s: 0){ _LL1044: _temp1043=((
struct Cyc_Absyn_Do_s_struct*) _temp969)->f1; goto _LL1038; _LL1038: _temp1037=((
struct Cyc_Absyn_Do_s_struct*) _temp969)->f2; _LL1042: _temp1041= _temp1037.f1;
goto _LL1040; _LL1040: _temp1039= _temp1037.f2; goto _LL984;} else{ goto _LL985;}
_LL985: if(( unsigned int) _temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_While_s:
0){ _LL1048: _temp1047=(( struct Cyc_Absyn_While_s_struct*) _temp969)->f1;
_LL1052: _temp1051= _temp1047.f1; goto _LL1050; _LL1050: _temp1049= _temp1047.f2;
goto _LL1046; _LL1046: _temp1045=(( struct Cyc_Absyn_While_s_struct*) _temp969)->f2;
goto _LL986;} else{ goto _LL987;} _LL987: if(( unsigned int) _temp969 > 1u?*((
int*) _temp969) == Cyc_Absyn_Break_s: 0){ _LL1054: _temp1053=(( struct Cyc_Absyn_Break_s_struct*)
_temp969)->f1; goto _LL988;} else{ goto _LL989;} _LL989: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Continue_s: 0){ _LL1056:
_temp1055=(( struct Cyc_Absyn_Continue_s_struct*) _temp969)->f1; goto _LL990;}
else{ goto _LL991;} _LL991: if(( unsigned int) _temp969 > 1u?*(( int*) _temp969)
== Cyc_Absyn_Goto_s: 0){ _LL1060: _temp1059=(( struct Cyc_Absyn_Goto_s_struct*)
_temp969)->f1; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Goto_s_struct*)
_temp969)->f2; if( _temp1057 == 0){ goto _LL992;} else{ goto _LL993;}} else{
goto _LL993;} _LL993: if(( unsigned int) _temp969 > 1u?*(( int*) _temp969) ==
Cyc_Absyn_Goto_s: 0){ _LL1064: _temp1063=(( struct Cyc_Absyn_Goto_s_struct*)
_temp969)->f1; goto _LL1062; _LL1062: _temp1061=(( struct Cyc_Absyn_Goto_s_struct*)
_temp969)->f2; goto _LL994;} else{ goto _LL995;} _LL995: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_For_s: 0){ _LL1080: _temp1079=((
struct Cyc_Absyn_For_s_struct*) _temp969)->f1; goto _LL1074; _LL1074: _temp1073=((
struct Cyc_Absyn_For_s_struct*) _temp969)->f2; _LL1078: _temp1077= _temp1073.f1;
goto _LL1076; _LL1076: _temp1075= _temp1073.f2; goto _LL1068; _LL1068: _temp1067=((
struct Cyc_Absyn_For_s_struct*) _temp969)->f3; _LL1072: _temp1071= _temp1067.f1;
goto _LL1070; _LL1070: _temp1069= _temp1067.f2; goto _LL1066; _LL1066: _temp1065=((
struct Cyc_Absyn_For_s_struct*) _temp969)->f4; goto _LL996;} else{ goto _LL997;}
_LL997: if(( unsigned int) _temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Switch_s:
0){ _LL1084: _temp1083=(( struct Cyc_Absyn_Switch_s_struct*) _temp969)->f1; goto
_LL1082; _LL1082: _temp1081=(( struct Cyc_Absyn_Switch_s_struct*) _temp969)->f2;
goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int) _temp969 > 1u?*((
int*) _temp969) == Cyc_Absyn_Fallthru_s: 0){ _LL1088: _temp1087=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp969)->f1; goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp969)->f2; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Decl_s: 0){ _LL1092: _temp1091=((
struct Cyc_Absyn_Decl_s_struct*) _temp969)->f1; _temp1093=* _temp1091; _LL1097:
_temp1096=( void*) _temp1093.r; if(*(( int*) _temp1096) == Cyc_Absyn_Var_d){
_LL1099: _temp1098=(( struct Cyc_Absyn_Var_d_struct*) _temp1096)->f1; goto
_LL1095;} else{ goto _LL1003;} _LL1095: _temp1094= _temp1093.loc; goto _LL1090;
_LL1090: _temp1089=(( struct Cyc_Absyn_Decl_s_struct*) _temp969)->f2; goto
_LL1002;} else{ goto _LL1003;} _LL1003: if(( unsigned int) _temp969 > 1u?*(( int*)
_temp969) == Cyc_Absyn_Decl_s: 0){ _LL1103: _temp1102=(( struct Cyc_Absyn_Decl_s_struct*)
_temp969)->f1; _temp1104=* _temp1102; _LL1108: _temp1107=( void*) _temp1104.r;
if(*(( int*) _temp1107) == Cyc_Absyn_Let_d){ _LL1121: _temp1120=(( struct Cyc_Absyn_Let_d_struct*)
_temp1107)->f1; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_Let_d_struct*)
_temp1107)->f2; if( _temp1115 == 0){ goto _LL1005;} else{ _temp1117=* _temp1115;
_LL1119: _temp1118=( struct Cyc_List_List*) _temp1117.v; goto _LL1114;} _LL1114:
_temp1113=(( struct Cyc_Absyn_Let_d_struct*) _temp1107)->f3; goto _LL1112;
_LL1112: _temp1111=(( struct Cyc_Absyn_Let_d_struct*) _temp1107)->f4; goto
_LL1110; _LL1110: _temp1109=(( struct Cyc_Absyn_Let_d_struct*) _temp1107)->f5;
goto _LL1106;} else{ goto _LL1005;} _LL1106: _temp1105= _temp1104.loc; goto
_LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_Decl_s_struct*) _temp969)->f2;
goto _LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int) _temp969 > 1u?*((
int*) _temp969) == Cyc_Absyn_Decl_s: 0){ _LL1125: _temp1124=(( struct Cyc_Absyn_Decl_s_struct*)
_temp969)->f1; _temp1126=* _temp1124; _LL1130: _temp1129=( void*) _temp1126.r;
if(*(( int*) _temp1129) == Cyc_Absyn_Letv_d){ _LL1132: _temp1131=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1129)->f1; goto _LL1128;} else{ goto _LL1007;} _LL1128: _temp1127=
_temp1126.loc; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_Decl_s_struct*)
_temp969)->f2; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Label_s: 0){ _LL1136: _temp1135=((
struct Cyc_Absyn_Label_s_struct*) _temp969)->f1; goto _LL1134; _LL1134:
_temp1133=(( struct Cyc_Absyn_Label_s_struct*) _temp969)->f2; goto _LL1008;}
else{ goto _LL1009;} _LL1009: if(( unsigned int) _temp969 > 1u?*(( int*)
_temp969) == Cyc_Absyn_TryCatch_s: 0){ _LL1140: _temp1139=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp969)->f1; goto _LL1138; _LL1138: _temp1137=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp969)->f2; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Region_s: 0){ _LL1146: _temp1145=((
struct Cyc_Absyn_Region_s_struct*) _temp969)->f1; goto _LL1144; _LL1144:
_temp1143=(( struct Cyc_Absyn_Region_s_struct*) _temp969)->f2; goto _LL1142;
_LL1142: _temp1141=(( struct Cyc_Absyn_Region_s_struct*) _temp969)->f3; goto
_LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int) _temp969 > 1u?*(( int*)
_temp969) == Cyc_Absyn_Decl_s: 0){ _LL1150: _temp1149=(( struct Cyc_Absyn_Decl_s_struct*)
_temp969)->f1; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_Decl_s_struct*)
_temp969)->f2; goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_SwitchC_s: 0){ _LL1154: _temp1153=((
struct Cyc_Absyn_SwitchC_s_struct*) _temp969)->f1; goto _LL1152; _LL1152:
_temp1151=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp969)->f2; goto _LL1016;}
else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp969 > 1u?*(( int*)
_temp969) == Cyc_Absyn_Cut_s: 0){ _LL1156: _temp1155=(( struct Cyc_Absyn_Cut_s_struct*)
_temp969)->f1; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp969 > 1u?*(( int*) _temp969) == Cyc_Absyn_Splice_s: 0){ _LL1158: _temp1157=((
struct Cyc_Absyn_Splice_s_struct*) _temp969)->f1; goto _LL1020;} else{ goto
_LL970;} _LL972: return; _LL974: { void* _temp1159= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1021); goto _LL1160; _LL1160: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1161=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1161->hd=( void*) _temp1159; _temp1161->tl= 0;
_temp1161;})); return;} _LL976: Cyc_NewControlFlow_cf_prepass( env, _temp1025);
Cyc_NewControlFlow_cf_prepass( env, _temp1023); return; _LL978: return; _LL980: {
void* _temp1162= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env),( struct Cyc_Absyn_Exp*) _check_null( _temp1029)); goto _LL1163; _LL1163:
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1164=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1164->hd=( void*)
_temp1162; _temp1164->tl= 0; _temp1164;})); return;} _LL982: { void* _temp1167;
void* _temp1169; struct _tuple0 _temp1165= Cyc_NewControlFlow_abstract_guard(
env, _temp1035); _LL1170: _temp1169= _temp1165.f1; goto _LL1168; _LL1168:
_temp1167= _temp1165.f2; goto _LL1166; _LL1166: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1171=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1171->hd=( void*) _temp1169; _temp1171->tl=({
struct Cyc_List_List* _temp1172=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1172->hd=( void*) _temp1167; _temp1172->tl= 0;
_temp1172;}); _temp1171;})); Cyc_NewControlFlow_cf_prepass( env, _temp1033); Cyc_NewControlFlow_cf_prepass(
env, _temp1031); return;} _LL984: _temp1051= _temp1041; _temp1049= _temp1039;
_temp1045= _temp1043; goto _LL986; _LL986: { void* _temp1175; void* _temp1177;
struct _tuple0 _temp1173= Cyc_NewControlFlow_abstract_guard( env, _temp1051);
_LL1178: _temp1177= _temp1173.f1; goto _LL1176; _LL1176: _temp1175= _temp1173.f2;
goto _LL1174; _LL1174: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1179=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1179->hd=( void*) _temp1177; _temp1179->tl=({ struct Cyc_List_List*
_temp1180=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1180->hd=( void*) _temp1175; _temp1180->tl= 0; _temp1180;}); _temp1179;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1045); return;} _LL988: return; _LL990:
return; _LL992:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1181=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1181[ 0]=({ struct Cyc_Core_Impossible_struct _temp1182; _temp1182.tag= Cyc_Core_Impossible;
_temp1182.f1= _tag_arr("cf_prepass: goto w/o destination", sizeof( unsigned char),
33u); _temp1182;}); _temp1181;})); _LL994: { struct Cyc_Absyn_Stmt* _temp1183=(
Cyc_NewControlFlow_get_stmt_annot( s))->encloser; goto _LL1184; _LL1184: {
struct Cyc_Absyn_Stmt* _temp1185=( Cyc_NewControlFlow_get_stmt_annot(( struct
Cyc_Absyn_Stmt*) _check_null( _temp1061)))->encloser; goto _LL1186; _LL1186:
while( _temp1185 != _temp1183) { struct Cyc_Absyn_Stmt* _temp1187=( Cyc_NewControlFlow_get_stmt_annot(
_temp1183))->encloser; goto _LL1188; _LL1188: if( _temp1187 == _temp1183){ Cyc_Tcutil_terr(
s->loc, _tag_arr("goto enters local scope or exception handler", sizeof(
unsigned char), 45u)); break;} _temp1183= _temp1187;} return;}} _LL996: { void*
_temp1189= Cyc_NewControlFlow_abstract_exp_top( env, _temp1079); goto _LL1190;
_LL1190: { void* _temp1193; void* _temp1195; struct _tuple0 _temp1191= Cyc_NewControlFlow_abstract_guard(
env, _temp1077); _LL1196: _temp1195= _temp1191.f1; goto _LL1194; _LL1194:
_temp1193= _temp1191.f2; goto _LL1192; _LL1192: { void* _temp1197= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1071); goto _LL1198; _LL1198: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1199=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1199->hd=( void*) _temp1189; _temp1199->tl=({
struct Cyc_List_List* _temp1200=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1200->hd=( void*) _temp1195; _temp1200->tl=({
struct Cyc_List_List* _temp1201=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1201->hd=( void*) _temp1193; _temp1201->tl=({
struct Cyc_List_List* _temp1202=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1202->hd=( void*) _temp1197; _temp1202->tl= 0;
_temp1202;}); _temp1201;}); _temp1200;}); _temp1199;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1065); return;}}} _LL998: { void* _temp1203= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1083); goto _LL1204; _LL1204: { struct
Cyc_List_List* _temp1205= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1081);
goto _LL1206; _LL1206: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1207=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1207->hd=( void*) _temp1203; _temp1207->tl= _temp1205; _temp1207;}));
return;}} _LL1000: { void* _temp1210; void* _temp1212; struct Cyc_NewControlFlow_AbsSynOne
_temp1208= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1087, 0)); _LL1213: _temp1212=( void*) _temp1208.inner_exp; goto
_LL1211; _LL1211: _temp1210=( void*) _temp1208.assigns; goto _LL1209; _LL1209: {
void* _temp1214= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1212, _temp1210); goto _LL1215; _LL1215: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1216=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1216->hd=( void*) _temp1214; _temp1216->tl= 0;
_temp1216;})); return;}} _LL1002: Cyc_NewControlFlow_add_var_root( env,
_temp1098); if( _temp1098->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp1218= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1217=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1217->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1098); _temp1217->tl= 0; _temp1217;})); goto
_LL1219; _LL1219: { void* _temp1220= Cyc_NewControlFlow_abstract_exp_top(
_temp1218,( struct Cyc_Absyn_Exp*) _check_null( _temp1098->initializer)); goto
_LL1221; _LL1221: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1222->hd=( void*) _temp1220; _temp1222->tl= 0; _temp1222;}));}} Cyc_NewControlFlow_cf_prepass(
env, _temp1089); return; _LL1004: for( 0; _temp1118 != 0; _temp1118=(( struct
Cyc_List_List*) _check_null( _temp1118))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp1118))->hd);}{
void* _temp1223= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp1111); goto _LL1224; _LL1224: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1225=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1225->hd=( void*) _temp1223; _temp1225->tl= 0;
_temp1225;})); Cyc_NewControlFlow_cf_prepass( env, _temp1100); return;} _LL1006:
for( 0; _temp1131 != 0; _temp1131=(( struct Cyc_List_List*) _check_null(
_temp1131))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1131))->hd);} Cyc_NewControlFlow_cf_prepass(
env, _temp1122); return; _LL1008: Cyc_NewControlFlow_cf_prepass( env, _temp1133);
return; _LL1010: Cyc_NewControlFlow_cf_prepass( env, _temp1139);{ struct Cyc_List_List*
_temp1226= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1137); goto _LL1227;
_LL1227: Cyc_NewControlFlow_cf_set_absexps( s, _temp1226); return;} _LL1012: Cyc_NewControlFlow_add_var_root(
env, _temp1143); Cyc_NewControlFlow_cf_prepass( env, _temp1141); return; _LL1014:
goto _LL1016; _LL1016: goto _LL1018; _LL1018: goto _LL1020; _LL1020:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1228=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1228[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1229; _temp1229.tag= Cyc_Core_Impossible;
_temp1229.f1= _tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u);
_temp1229;}); _temp1228;})); _LL970:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1230= Cyc_NewControlFlow_get_stmt_annot(
s); goto _LL1231; _LL1231:( void*)( _temp1230->flow=( void*) Cyc_CfFlowInfo_join_flow(
in_flow,( void*) _temp1230->flow)); ++ _temp1230->visited; _temp1230->visited ==
Cyc_NewControlFlow_iteration_num? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:894 annot->visited == iteration_num", sizeof(
unsigned char), 57u)); return _temp1230;} static void Cyc_NewControlFlow_update_flow(
struct Cyc_Absyn_Stmt* s, void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1232= Cyc_NewControlFlow_get_stmt_annot( s); goto _LL1233; _LL1233: { void*
_temp1234= Cyc_CfFlowInfo_join_flow( flow,( void*) _temp1232->flow); goto
_LL1235; _LL1235: if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp1234,( void*)
_temp1232->flow)){( void*)( _temp1232->flow=( void*) _temp1234); if( _temp1232->visited
== Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check= 1;}}}}
static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1236= flow; struct Cyc_Dict_Dict*
_temp1242; _LL1238: if( _temp1236 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1239;} else{ goto _LL1240;} _LL1240: if(( unsigned int) _temp1236 > 1u?*((
int*) _temp1236) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1243: _temp1242=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1236)->f1; goto _LL1241;} else{ goto _LL1237;} _LL1239: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1241: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1246=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1244=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1244[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1245; _temp1245.tag=
Cyc_CfFlowInfo_VarRoot; _temp1245.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1245;}); _temp1244;}); goto _LL1247; _LL1247: {
void* _temp1248= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) _temp1246)); goto _LL1249; _LL1249: _temp1242=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1242,(
void*) _temp1246, _temp1248);}} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1250=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1250[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1251; _temp1251.tag=
Cyc_CfFlowInfo_InitsFL; _temp1251.f1= _temp1242; _temp1251;}); _temp1250;});
_LL1237:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1252= flow; struct Cyc_Dict_Dict*
_temp1258; _LL1254: if( _temp1252 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1255;} else{ goto _LL1256;} _LL1256: if(( unsigned int) _temp1252 > 1u?*((
int*) _temp1252) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1259: _temp1258=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1252)->f1; goto _LL1257;} else{ goto _LL1253;} _LL1255: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1257: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1262=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1260=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1260[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1261; _temp1261.tag=
Cyc_CfFlowInfo_VarRoot; _temp1261.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1261;}); _temp1260;}); goto _LL1263; _LL1263:
_temp1258=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( _temp1258,( void*) _temp1262,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) _temp1262));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1264=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1264[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1265; _temp1265.tag= Cyc_CfFlowInfo_InitsFL;
_temp1265.f1= _temp1258; _temp1265;}); _temp1264;}); _LL1253:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)(( struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp1266= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); goto _LL1267; _LL1267: return({ struct _tuple0
_temp1268; _temp1268.f1= _temp1266; _temp1268.f2= _temp1266; _temp1268;});}
return({ struct _tuple0 _temp1269; _temp1269.f1= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null( aes))->hd, in_flow);
_temp1269.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd, in_flow);
_temp1269;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp1272; struct Cyc_Position_Segment*
_temp1273; struct Cyc_Absyn_Stmt* _temp1275; struct Cyc_Absyn_Exp* _temp1277;
struct Cyc_Core_Opt* _temp1279; struct Cyc_Absyn_Pat* _temp1281; struct Cyc_Absyn_Switch_clause*
_temp1270=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd; _temp1272=* _temp1270; _LL1282: _temp1281= _temp1272.pattern;
goto _LL1280; _LL1280: _temp1279= _temp1272.pat_vars; goto _LL1278; _LL1278:
_temp1277= _temp1272.where_clause; goto _LL1276; _LL1276: _temp1275= _temp1272.body;
goto _LL1274; _LL1274: _temp1273= _temp1272.loc; goto _LL1271; _LL1271: { void*
_temp1283= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1279))->v); goto _LL1284; _LL1284:
_temp1283= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1283,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1279))->v);{ void* body_outflow; if(
_temp1277 != 0){ void* _temp1287; void* _temp1289; struct _tuple0 _temp1285= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1277))->loc, aes, _temp1283);
_LL1290: _temp1289= _temp1285.f1; goto _LL1288; _LL1288: _temp1287= _temp1285.f2;
goto _LL1286; _LL1286: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp1287; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1275, _temp1289);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1275, _temp1283);}{ void* _temp1291= body_outflow; _LL1293: if(
_temp1291 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1294;} else{ goto _LL1295;}
_LL1295: goto _LL1296; _LL1294: goto _LL1292; _LL1296: Cyc_Tcutil_terr(
_temp1275->loc, _tag_arr("switch clause may implicitly fallthru", sizeof(
unsigned char), 38u)); goto _LL1292; _LL1292:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1297= Cyc_NewControlFlow_pre_stmt_check(
in_flow, s); goto _LL1298; _LL1298: in_flow=( void*) _temp1297->flow;{ struct
Cyc_List_List* _temp1299= _temp1297->absexps; goto _LL1300; _LL1300: { void*
_temp1301=( void*) s->r; struct Cyc_Absyn_Exp* _temp1347; struct Cyc_Absyn_Exp*
_temp1349; struct Cyc_Absyn_Exp* _temp1351; struct Cyc_Absyn_Stmt* _temp1353;
struct Cyc_Absyn_Stmt* _temp1355; struct Cyc_Absyn_Stmt* _temp1357; struct Cyc_Absyn_Stmt*
_temp1359; struct Cyc_Absyn_Exp* _temp1361; struct Cyc_Absyn_Stmt* _temp1363;
struct _tuple3 _temp1365; struct Cyc_Absyn_Stmt* _temp1367; struct Cyc_Absyn_Exp*
_temp1369; struct _tuple3 _temp1371; struct Cyc_Absyn_Stmt* _temp1373; struct
Cyc_Absyn_Exp* _temp1375; struct Cyc_Absyn_Stmt* _temp1377; struct Cyc_Absyn_Stmt*
_temp1379; struct _tuple3 _temp1381; struct Cyc_Absyn_Stmt* _temp1383; struct
Cyc_Absyn_Exp* _temp1385; struct _tuple3 _temp1387; struct Cyc_Absyn_Stmt*
_temp1389; struct Cyc_Absyn_Exp* _temp1391; struct Cyc_Absyn_Exp* _temp1393;
struct Cyc_Absyn_Stmt* _temp1395; struct Cyc_Absyn_Switch_clause** _temp1397;
struct Cyc_Absyn_Switch_clause* _temp1399; struct Cyc_List_List* _temp1400;
struct Cyc_Absyn_Stmt* _temp1402; struct Cyc_Absyn_Stmt* _temp1404; struct Cyc_Absyn_Stmt*
_temp1406; struct _tagged_arr* _temp1408; struct Cyc_List_List* _temp1410;
struct Cyc_Absyn_Exp* _temp1412; struct Cyc_List_List* _temp1414; struct Cyc_Absyn_Stmt*
_temp1416; struct Cyc_Absyn_Stmt* _temp1418; struct Cyc_Absyn_Decl* _temp1420;
struct Cyc_Absyn_Decl _temp1422; struct Cyc_Position_Segment* _temp1423; void*
_temp1425; struct Cyc_Absyn_Vardecl* _temp1427; struct Cyc_Absyn_Stmt* _temp1429;
struct Cyc_Absyn_Decl* _temp1431; struct Cyc_Absyn_Decl _temp1433; struct Cyc_Position_Segment*
_temp1434; void* _temp1436; int _temp1438; struct Cyc_Absyn_Exp* _temp1440;
struct Cyc_Core_Opt* _temp1442; struct Cyc_Core_Opt* _temp1444; struct Cyc_Core_Opt
_temp1446; struct Cyc_List_List* _temp1447; struct Cyc_Absyn_Pat* _temp1449;
struct Cyc_Absyn_Stmt* _temp1451; struct Cyc_Absyn_Decl* _temp1453; struct Cyc_Absyn_Decl
_temp1455; struct Cyc_Position_Segment* _temp1456; void* _temp1458; struct Cyc_List_List*
_temp1460; struct Cyc_Absyn_Stmt* _temp1462; struct _tagged_arr* _temp1464;
struct Cyc_Absyn_Stmt* _temp1466; struct Cyc_Absyn_Vardecl* _temp1468; struct
Cyc_Absyn_Tvar* _temp1470; _LL1303: if( _temp1301 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1304;} else{ goto _LL1305;} _LL1305: if(( unsigned int) _temp1301 > 1u?*((
int*) _temp1301) == Cyc_Absyn_Return_s: 0){ _LL1348: _temp1347=(( struct Cyc_Absyn_Return_s_struct*)
_temp1301)->f1; if( _temp1347 == 0){ goto _LL1306;} else{ goto _LL1307;}} else{
goto _LL1307;} _LL1307: if(( unsigned int) _temp1301 > 1u?*(( int*) _temp1301)
== Cyc_Absyn_Return_s: 0){ _LL1350: _temp1349=(( struct Cyc_Absyn_Return_s_struct*)
_temp1301)->f1; goto _LL1308;} else{ goto _LL1309;} _LL1309: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Exp_s: 0){ _LL1352: _temp1351=((
struct Cyc_Absyn_Exp_s_struct*) _temp1301)->f1; goto _LL1310;} else{ goto
_LL1311;} _LL1311: if(( unsigned int) _temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Seq_s:
0){ _LL1356: _temp1355=(( struct Cyc_Absyn_Seq_s_struct*) _temp1301)->f1; goto
_LL1354; _LL1354: _temp1353=(( struct Cyc_Absyn_Seq_s_struct*) _temp1301)->f2;
goto _LL1312;} else{ goto _LL1313;} _LL1313: if(( unsigned int) _temp1301 > 1u?*((
int*) _temp1301) == Cyc_Absyn_IfThenElse_s: 0){ _LL1362: _temp1361=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1301)->f1; goto _LL1360; _LL1360: _temp1359=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1301)->f2; goto _LL1358; _LL1358: _temp1357=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1301)->f3; goto _LL1314;} else{ goto _LL1315;} _LL1315: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_While_s: 0){ _LL1366: _temp1365=((
struct Cyc_Absyn_While_s_struct*) _temp1301)->f1; _LL1370: _temp1369= _temp1365.f1;
goto _LL1368; _LL1368: _temp1367= _temp1365.f2; goto _LL1364; _LL1364: _temp1363=((
struct Cyc_Absyn_While_s_struct*) _temp1301)->f2; goto _LL1316;} else{ goto
_LL1317;} _LL1317: if(( unsigned int) _temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Do_s:
0){ _LL1378: _temp1377=(( struct Cyc_Absyn_Do_s_struct*) _temp1301)->f1; goto
_LL1372; _LL1372: _temp1371=(( struct Cyc_Absyn_Do_s_struct*) _temp1301)->f2;
_LL1376: _temp1375= _temp1371.f1; goto _LL1374; _LL1374: _temp1373= _temp1371.f2;
goto _LL1318;} else{ goto _LL1319;} _LL1319: if(( unsigned int) _temp1301 > 1u?*((
int*) _temp1301) == Cyc_Absyn_For_s: 0){ _LL1394: _temp1393=(( struct Cyc_Absyn_For_s_struct*)
_temp1301)->f1; goto _LL1388; _LL1388: _temp1387=(( struct Cyc_Absyn_For_s_struct*)
_temp1301)->f2; _LL1392: _temp1391= _temp1387.f1; goto _LL1390; _LL1390:
_temp1389= _temp1387.f2; goto _LL1382; _LL1382: _temp1381=(( struct Cyc_Absyn_For_s_struct*)
_temp1301)->f3; _LL1386: _temp1385= _temp1381.f1; goto _LL1384; _LL1384:
_temp1383= _temp1381.f2; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_For_s_struct*)
_temp1301)->f4; goto _LL1320;} else{ goto _LL1321;} _LL1321: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Break_s: 0){ _LL1396: _temp1395=((
struct Cyc_Absyn_Break_s_struct*) _temp1301)->f1; if( _temp1395 == 0){ goto
_LL1322;} else{ goto _LL1323;}} else{ goto _LL1323;} _LL1323: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Fallthru_s: 0){ _LL1401:
_temp1400=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1301)->f1; goto _LL1398;
_LL1398: _temp1397=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1301)->f2; if(
_temp1397 == 0){ goto _LL1325;} else{ _temp1399=* _temp1397; goto _LL1324;}}
else{ goto _LL1325;} _LL1325: if(( unsigned int) _temp1301 > 1u?*(( int*)
_temp1301) == Cyc_Absyn_Break_s: 0){ _LL1403: _temp1402=(( struct Cyc_Absyn_Break_s_struct*)
_temp1301)->f1; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Continue_s: 0){ _LL1405:
_temp1404=(( struct Cyc_Absyn_Continue_s_struct*) _temp1301)->f1; goto _LL1328;}
else{ goto _LL1329;} _LL1329: if(( unsigned int) _temp1301 > 1u?*(( int*)
_temp1301) == Cyc_Absyn_Goto_s: 0){ _LL1409: _temp1408=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1301)->f1; goto _LL1407; _LL1407: _temp1406=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1301)->f2; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Switch_s: 0){ _LL1413:
_temp1412=(( struct Cyc_Absyn_Switch_s_struct*) _temp1301)->f1; goto _LL1411;
_LL1411: _temp1410=(( struct Cyc_Absyn_Switch_s_struct*) _temp1301)->f2; goto
_LL1332;} else{ goto _LL1333;} _LL1333: if(( unsigned int) _temp1301 > 1u?*((
int*) _temp1301) == Cyc_Absyn_TryCatch_s: 0){ _LL1417: _temp1416=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1301)->f1; goto _LL1415; _LL1415: _temp1414=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1301)->f2; goto _LL1334;} else{ goto _LL1335;} _LL1335: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Decl_s: 0){ _LL1421: _temp1420=((
struct Cyc_Absyn_Decl_s_struct*) _temp1301)->f1; _temp1422=* _temp1420; _LL1426:
_temp1425=( void*) _temp1422.r; if(*(( int*) _temp1425) == Cyc_Absyn_Var_d){
_LL1428: _temp1427=(( struct Cyc_Absyn_Var_d_struct*) _temp1425)->f1; goto
_LL1424;} else{ goto _LL1337;} _LL1424: _temp1423= _temp1422.loc; goto _LL1419;
_LL1419: _temp1418=(( struct Cyc_Absyn_Decl_s_struct*) _temp1301)->f2; goto
_LL1336;} else{ goto _LL1337;} _LL1337: if(( unsigned int) _temp1301 > 1u?*((
int*) _temp1301) == Cyc_Absyn_Decl_s: 0){ _LL1432: _temp1431=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1301)->f1; _temp1433=* _temp1431; _LL1437: _temp1436=( void*) _temp1433.r;
if(*(( int*) _temp1436) == Cyc_Absyn_Let_d){ _LL1450: _temp1449=(( struct Cyc_Absyn_Let_d_struct*)
_temp1436)->f1; goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_Let_d_struct*)
_temp1436)->f2; if( _temp1444 == 0){ goto _LL1339;} else{ _temp1446=* _temp1444;
_LL1448: _temp1447=( struct Cyc_List_List*) _temp1446.v; goto _LL1443;} _LL1443:
_temp1442=(( struct Cyc_Absyn_Let_d_struct*) _temp1436)->f3; goto _LL1441;
_LL1441: _temp1440=(( struct Cyc_Absyn_Let_d_struct*) _temp1436)->f4; goto
_LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_Let_d_struct*) _temp1436)->f5;
goto _LL1435;} else{ goto _LL1339;} _LL1435: _temp1434= _temp1433.loc; goto
_LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_Decl_s_struct*) _temp1301)->f2;
goto _LL1338;} else{ goto _LL1339;} _LL1339: if(( unsigned int) _temp1301 > 1u?*((
int*) _temp1301) == Cyc_Absyn_Decl_s: 0){ _LL1454: _temp1453=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1301)->f1; _temp1455=* _temp1453; _LL1459: _temp1458=( void*) _temp1455.r;
if(*(( int*) _temp1458) == Cyc_Absyn_Letv_d){ _LL1461: _temp1460=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1458)->f1; goto _LL1457;} else{ goto _LL1341;} _LL1457: _temp1456=
_temp1455.loc; goto _LL1452; _LL1452: _temp1451=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1301)->f2; goto _LL1340;} else{ goto _LL1341;} _LL1341: if(( unsigned int)
_temp1301 > 1u?*(( int*) _temp1301) == Cyc_Absyn_Label_s: 0){ _LL1465: _temp1464=((
struct Cyc_Absyn_Label_s_struct*) _temp1301)->f1; goto _LL1463; _LL1463:
_temp1462=(( struct Cyc_Absyn_Label_s_struct*) _temp1301)->f2; goto _LL1342;}
else{ goto _LL1343;} _LL1343: if(( unsigned int) _temp1301 > 1u?*(( int*)
_temp1301) == Cyc_Absyn_Region_s: 0){ _LL1471: _temp1470=(( struct Cyc_Absyn_Region_s_struct*)
_temp1301)->f1; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_Region_s_struct*)
_temp1301)->f2; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_Region_s_struct*)
_temp1301)->f3; goto _LL1344;} else{ goto _LL1345;} _LL1345: goto _LL1346;
_LL1304: return in_flow; _LL1306: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1308: Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*) _check_null(
_temp1349))->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1299))->hd,
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1310: return Cyc_CfAbsexp_eval_absexp(
env, _temp1351->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1299))->hd,
in_flow); _LL1312: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1353,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1355, in_flow)); _LL1314: { void*
_temp1474; void* _temp1476; struct _tuple0 _temp1472= Cyc_NewControlFlow_cf_evalguard(
env, _temp1361->loc, _temp1299, in_flow); _LL1477: _temp1476= _temp1472.f1; goto
_LL1475; _LL1475: _temp1474= _temp1472.f2; goto _LL1473; _LL1473: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1359, _temp1476), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1357, _temp1474));} _LL1316: { void* _temp1478=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1367))->flow; goto _LL1479; _LL1479: { void* _temp1482; void*
_temp1484; struct _tuple0 _temp1480= Cyc_NewControlFlow_cf_evalguard( env,
_temp1369->loc, _temp1299, _temp1478); _LL1485: _temp1484= _temp1480.f1; goto
_LL1483; _LL1483: _temp1482= _temp1480.f2; goto _LL1481; _LL1481: { void*
_temp1486= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1363, _temp1484); goto
_LL1487; _LL1487: Cyc_NewControlFlow_update_flow( _temp1367, _temp1486); return
_temp1482;}}} _LL1318: { void* _temp1488= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1377, in_flow); goto _LL1489; _LL1489: { void* _temp1490=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1488, _temp1373))->flow; goto _LL1491; _LL1491: { void* _temp1494; void*
_temp1496; struct _tuple0 _temp1492= Cyc_NewControlFlow_cf_evalguard( env,
_temp1375->loc, _temp1299, _temp1490); _LL1497: _temp1496= _temp1492.f1; goto
_LL1495; _LL1495: _temp1494= _temp1492.f2; goto _LL1493; _LL1493: Cyc_NewControlFlow_update_flow(
_temp1377, _temp1496); return _temp1494;}}} _LL1320: { void* _temp1498= Cyc_CfAbsexp_eval_absexp(
env, _temp1393->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1299))->hd,
in_flow); goto _LL1499; _LL1499: { void* _temp1500=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1498, _temp1389))->flow; goto _LL1501; _LL1501: { void* _temp1504; void*
_temp1506; struct _tuple0 _temp1502= Cyc_NewControlFlow_cf_evalguard( env,
_temp1391->loc,(( struct Cyc_List_List*) _check_null( _temp1299))->tl, _temp1500);
_LL1507: _temp1506= _temp1502.f1; goto _LL1505; _LL1505: _temp1504= _temp1502.f2;
goto _LL1503; _LL1503: { void* _temp1508= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1379, _temp1506); goto _LL1509; _LL1509: { void* _temp1510=( void*)(
Cyc_NewControlFlow_pre_stmt_check( _temp1508, _temp1383))->flow; goto _LL1511;
_LL1511: { void* _temp1512= Cyc_CfAbsexp_eval_absexp( env, _temp1385->loc,( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1299))->tl))->tl))->tl))->hd, _temp1510); goto _LL1513; _LL1513: Cyc_NewControlFlow_update_flow(
_temp1389, _temp1512); return _temp1504;}}}}}} _LL1322: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1324: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp1299))->hd, in_flow);{ struct Cyc_List_List* _temp1514=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1399->pat_vars))->v;
goto _LL1515; _LL1515: in_flow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow, _temp1514); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env,
in_flow, _temp1514); _temp1402=( struct Cyc_Absyn_Stmt*) _temp1399->body; goto
_LL1326;} _LL1326: _temp1404= _temp1402; goto _LL1328; _LL1328: _temp1406=
_temp1404; goto _LL1330; _LL1330: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)
_check_null( _temp1406), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1332: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1412->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp1299))->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1410,(( struct Cyc_List_List*) _check_null( _temp1299))->tl, in_flow);
return( void*) Cyc_CfFlowInfo_BottomFL; _LL1334: { struct Cyc_NewControlFlow_AnalEnv
_temp1518; void* _temp1519; int _temp1521; struct Cyc_Dict_Dict* _temp1523;
struct Cyc_NewControlFlow_AnalEnv* _temp1516= env; _temp1518=* _temp1516;
_LL1524: _temp1523= _temp1518.roots; goto _LL1522; _LL1522: _temp1521= _temp1518.in_try;
goto _LL1520; _LL1520: _temp1519=( void*) _temp1518.tryflow; goto _LL1517;
_LL1517: env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void*
_temp1525= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1416, in_flow); goto
_LL1526; _LL1526: { void* _temp1527=( void*) env->tryflow; goto _LL1528; _LL1528:
env->in_try= _temp1521;( void*)( env->tryflow=( void*) _temp1519); Cyc_NewControlFlow_update_tryflow(
env, _temp1527); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1414,
_temp1299, _temp1527); return _temp1525;}}} _LL1336: { struct Cyc_List_List
_temp1530=({ struct Cyc_List_List _temp1529; _temp1529.hd=( void*) _temp1427;
_temp1529.tl= 0; _temp1529;}); goto _LL1531; _LL1531: { void* _temp1532= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)& _temp1530); goto _LL1533; _LL1533: {
struct Cyc_Absyn_Exp* _temp1534= _temp1427->initializer; goto _LL1535; _LL1535:
if( _temp1534 != 0){ _temp1532= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1534))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp1299))->hd, _temp1532);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1418, _temp1532);}}} _LL1338: { void* _temp1536= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1447); goto _LL1537; _LL1537: _temp1536= Cyc_CfAbsexp_eval_absexp(
env, _temp1440->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1299))->hd,
_temp1536); _temp1536= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1536,
_temp1447); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1429, _temp1536);}
_LL1340: { void* _temp1538= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1460); goto _LL1539; _LL1539: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1451, _temp1538);} _LL1342: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1462, in_flow); _LL1344: { struct Cyc_List_List* _temp1541=({ struct
Cyc_List_List* _temp1540=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1540->hd=( void*) _temp1468; _temp1540->tl= 0; _temp1540;}); goto _LL1542;
_LL1542: { void* _temp1543= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1541); goto _LL1544; _LL1544: _temp1543= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1543, _temp1541); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1466, _temp1543);}} _LL1346:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1545=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1545[ 0]=({ struct Cyc_Core_Impossible_struct _temp1546; _temp1546.tag= Cyc_Core_Impossible;
_temp1546.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp1546;}); _temp1545;})); _LL1302:;}}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1547=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1571; struct Cyc_Absyn_Stmt*
_temp1573; struct Cyc_Absyn_Stmt* _temp1575; struct Cyc_Absyn_Stmt* _temp1577;
struct Cyc_Absyn_Exp* _temp1579; struct Cyc_Absyn_Stmt* _temp1581; struct
_tuple3 _temp1583; struct Cyc_Absyn_Stmt* _temp1585; struct Cyc_Absyn_Exp*
_temp1587; struct _tuple3 _temp1589; struct Cyc_Absyn_Stmt* _temp1591; struct
Cyc_Absyn_Exp* _temp1593; struct Cyc_Absyn_Exp* _temp1595; struct Cyc_Absyn_Stmt*
_temp1597; struct _tuple3 _temp1599; struct Cyc_Absyn_Stmt* _temp1601; struct
Cyc_Absyn_Exp* _temp1603; struct _tuple3 _temp1605; struct Cyc_Absyn_Stmt*
_temp1607; struct Cyc_Absyn_Exp* _temp1609; struct Cyc_Absyn_Stmt* _temp1611;
struct Cyc_Absyn_Stmt* _temp1613; struct Cyc_Absyn_Vardecl* _temp1615; struct
Cyc_Absyn_Tvar* _temp1617; struct Cyc_Absyn_Stmt* _temp1619; struct _tagged_arr*
_temp1621; struct Cyc_Absyn_Stmt* _temp1623; struct Cyc_Absyn_Decl* _temp1625;
struct Cyc_List_List* _temp1627; struct Cyc_Absyn_Stmt* _temp1629; struct Cyc_List_List*
_temp1631; struct Cyc_Absyn_Exp* _temp1633; _LL1549: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Seq_s: 0){ _LL1574: _temp1573=((
struct Cyc_Absyn_Seq_s_struct*) _temp1547)->f1; goto _LL1572; _LL1572: _temp1571=((
struct Cyc_Absyn_Seq_s_struct*) _temp1547)->f2; goto _LL1550;} else{ goto
_LL1551;} _LL1551: if(( unsigned int) _temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_IfThenElse_s:
0){ _LL1580: _temp1579=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1547)->f1;
goto _LL1578; _LL1578: _temp1577=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1547)->f2; goto _LL1576; _LL1576: _temp1575=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1547)->f3; goto _LL1552;} else{ goto _LL1553;} _LL1553: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_For_s: 0){ _LL1596: _temp1595=((
struct Cyc_Absyn_For_s_struct*) _temp1547)->f1; goto _LL1590; _LL1590: _temp1589=((
struct Cyc_Absyn_For_s_struct*) _temp1547)->f2; _LL1594: _temp1593= _temp1589.f1;
goto _LL1592; _LL1592: _temp1591= _temp1589.f2; goto _LL1584; _LL1584: _temp1583=((
struct Cyc_Absyn_For_s_struct*) _temp1547)->f3; _LL1588: _temp1587= _temp1583.f1;
goto _LL1586; _LL1586: _temp1585= _temp1583.f2; goto _LL1582; _LL1582: _temp1581=((
struct Cyc_Absyn_For_s_struct*) _temp1547)->f4; goto _LL1554;} else{ goto
_LL1555;} _LL1555: if(( unsigned int) _temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_While_s:
0){ _LL1600: _temp1599=(( struct Cyc_Absyn_While_s_struct*) _temp1547)->f1;
_LL1604: _temp1603= _temp1599.f1; goto _LL1602; _LL1602: _temp1601= _temp1599.f2;
goto _LL1598; _LL1598: _temp1597=(( struct Cyc_Absyn_While_s_struct*) _temp1547)->f2;
goto _LL1556;} else{ goto _LL1557;} _LL1557: if(( unsigned int) _temp1547 > 1u?*((
int*) _temp1547) == Cyc_Absyn_Do_s: 0){ _LL1612: _temp1611=(( struct Cyc_Absyn_Do_s_struct*)
_temp1547)->f1; goto _LL1606; _LL1606: _temp1605=(( struct Cyc_Absyn_Do_s_struct*)
_temp1547)->f2; _LL1610: _temp1609= _temp1605.f1; goto _LL1608; _LL1608:
_temp1607= _temp1605.f2; goto _LL1558;} else{ goto _LL1559;} _LL1559: if((
unsigned int) _temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Region_s: 0){
_LL1618: _temp1617=(( struct Cyc_Absyn_Region_s_struct*) _temp1547)->f1; goto
_LL1616; _LL1616: _temp1615=(( struct Cyc_Absyn_Region_s_struct*) _temp1547)->f2;
goto _LL1614; _LL1614: _temp1613=(( struct Cyc_Absyn_Region_s_struct*) _temp1547)->f3;
goto _LL1560;} else{ goto _LL1561;} _LL1561: if(( unsigned int) _temp1547 > 1u?*((
int*) _temp1547) == Cyc_Absyn_Label_s: 0){ _LL1622: _temp1621=(( struct Cyc_Absyn_Label_s_struct*)
_temp1547)->f1; goto _LL1620; _LL1620: _temp1619=(( struct Cyc_Absyn_Label_s_struct*)
_temp1547)->f2; goto _LL1562;} else{ goto _LL1563;} _LL1563: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Decl_s: 0){ _LL1626: _temp1625=((
struct Cyc_Absyn_Decl_s_struct*) _temp1547)->f1; goto _LL1624; _LL1624:
_temp1623=(( struct Cyc_Absyn_Decl_s_struct*) _temp1547)->f2; goto _LL1564;}
else{ goto _LL1565;} _LL1565: if(( unsigned int) _temp1547 > 1u?*(( int*)
_temp1547) == Cyc_Absyn_TryCatch_s: 0){ _LL1630: _temp1629=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1547)->f1; goto _LL1628; _LL1628: _temp1627=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1547)->f2; goto _LL1566;} else{ goto _LL1567;} _LL1567: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Switch_s: 0){ _LL1634:
_temp1633=(( struct Cyc_Absyn_Switch_s_struct*) _temp1547)->f1; goto _LL1632;
_LL1632: _temp1631=(( struct Cyc_Absyn_Switch_s_struct*) _temp1547)->f2; goto
_LL1568;} else{ goto _LL1569;} _LL1569: goto _LL1570; _LL1550: _temp1577=
_temp1573; _temp1575= _temp1571; goto _LL1552; _LL1552: Cyc_NewControlFlow_cf_postpass(
_temp1577); Cyc_NewControlFlow_cf_postpass( _temp1575); return; _LL1554: Cyc_NewControlFlow_cf_postpass(
_temp1585); _temp1601= _temp1581; _temp1597= _temp1591; goto _LL1556; _LL1556:
_temp1611= _temp1597; _temp1607= _temp1601; goto _LL1558; _LL1558: Cyc_NewControlFlow_cf_postpass(
_temp1607); _temp1613= _temp1611; goto _LL1560; _LL1560: _temp1619= _temp1613;
goto _LL1562; _LL1562: _temp1623= _temp1619; goto _LL1564; _LL1564: Cyc_NewControlFlow_cf_postpass(
_temp1623); return; _LL1566: Cyc_NewControlFlow_cf_postpass( _temp1629);
_temp1631= _temp1627; goto _LL1568; _LL1568: for( 0; _temp1631 != 0; _temp1631=((
struct Cyc_List_List*) _check_null( _temp1631))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1631))->hd)->body);}
return; _LL1570: return; _LL1548:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1661; _temp1661.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1662=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1662->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1662->comprehension_vars= 0; _temp1662;}); _temp1661.lhsides= 0; _temp1661.access_path=
0; _temp1661;}); struct Cyc_List_List* _temp1635=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v; goto _LL1636;
_LL1636:{ struct Cyc_List_List* vds= _temp1635; for( 0; vds != 0; vds=(( struct
Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_List_List*
_temp1639; struct Cyc_List_List* _temp1641; struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1643; struct Cyc_NewControlFlow_SharedAbsEnv _temp1645; struct Cyc_List_List*
_temp1646; struct Cyc_Dict_Dict* _temp1648; struct Cyc_NewControlFlow_AbsEnv
_temp1637= absenv; _LL1644: _temp1643= _temp1637.shared; _temp1645=* _temp1643;
_LL1649: _temp1648= _temp1645.all_roots; goto _LL1647; _LL1647: _temp1646=
_temp1645.comprehension_vars; goto _LL1642; _LL1642: _temp1641= _temp1637.lhsides;
goto _LL1640; _LL1640: _temp1639= _temp1637.access_path; goto _LL1638; _LL1638: {
struct Cyc_NewControlFlow_AnalEnv* _temp1651=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1650=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1650->roots= _temp1648; _temp1650->in_try= 0; _temp1650->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1650;}); goto _LL1652; _LL1652: { void*
in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1659=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1659[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1660; _temp1660.tag= Cyc_CfFlowInfo_InitsFL;
_temp1660.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1660;}); _temp1659;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
_temp1651, in_flow, _temp1635); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
_temp1651, in_flow, _temp1646);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( _temp1651, fd->body, in_flow);}{
void* _temp1653= out_flow; _LL1655: if( _temp1653 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1656;} else{ goto _LL1657;} _LL1657: goto _LL1658; _LL1656: goto _LL1654;
_LL1658: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc, _tag_arr("function may complete without returning a value",
sizeof( unsigned char), 48u));} goto _LL1654; _LL1654:;} Cyc_NewControlFlow_cf_postpass(
fd->body);}}}}} void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds){
for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void*
_temp1663=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl* _temp1673; struct Cyc_List_List*
_temp1675; struct _tuple1* _temp1677; struct Cyc_List_List* _temp1679; struct
_tagged_arr* _temp1681; _LL1665: if(*(( int*) _temp1663) == Cyc_Absyn_Fn_d){
_LL1674: _temp1673=(( struct Cyc_Absyn_Fn_d_struct*) _temp1663)->f1; goto
_LL1666;} else{ goto _LL1667;} _LL1667: if(*(( int*) _temp1663) == Cyc_Absyn_Using_d){
_LL1678: _temp1677=(( struct Cyc_Absyn_Using_d_struct*) _temp1663)->f1; goto
_LL1676; _LL1676: _temp1675=(( struct Cyc_Absyn_Using_d_struct*) _temp1663)->f2;
goto _LL1668;} else{ goto _LL1669;} _LL1669: if(*(( int*) _temp1663) == Cyc_Absyn_Namespace_d){
_LL1682: _temp1681=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1663)->f1; goto
_LL1680; _LL1680: _temp1679=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1663)->f2;
goto _LL1670;} else{ goto _LL1671;} _LL1671: goto _LL1672; _LL1666: Cyc_NewControlFlow_cf_check_fun(
_temp1673); goto _LL1664; _LL1668: _temp1679= _temp1675; goto _LL1670; _LL1670:
Cyc_NewControlFlow_cf_check( _temp1679); goto _LL1664; _LL1672: goto _LL1664;
_LL1664:;}}