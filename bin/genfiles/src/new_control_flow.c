 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
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
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern void* Cyc_Assert_AssertFail(
struct _tagged_string msg); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_string
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_string desc;
} ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple1{ void* f1;
struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
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
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const
int Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
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
unsigned char* _temp14=( unsigned char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14;
_temp15.last_plus_one= _temp14 + 37; _temp15;}); _temp13;}); _temp12;})); _LL4:;}
void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{
struct Cyc_Dict_Dict* all_roots; struct Cyc_List_List* comprehension_vars; } ;
struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_NewControlFlow_SharedAbsEnv* shared;
struct Cyc_List_List* lhsides; struct Cyc_List_List* access_path; } ; struct Cyc_NewControlFlow_AbsSynOne{
void* inner_exp; void* assigns; } ; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
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
unsigned char* _temp93=( unsigned char*)"new_control_flow.cyc:219 path->tl == null";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 42; _temp94;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL68: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL64; _LL70: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp85= _temp81; goto _LL72; _LL72: absop= Cyc_CfAbsexp_mkMemberOp(
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
unsigned char* _temp103=( unsigned char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
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
unsigned char* _temp151=( unsigned char*)"bad malloc access path"; struct
_tagged_string _temp152; _temp152.curr= _temp151; _temp152.base= _temp151;
_temp152.last_plus_one= _temp151 + 23; _temp152;}); _temp150;}); _temp149;}));
_LL133:;} return _temp130;}}}}} struct _tuple4{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp153=(
void*) e->r; struct Cyc_Absyn_Exp* _temp243; struct Cyc_List_List* _temp245;
struct Cyc_Absyn_Exp* _temp247; void* _temp249; int _temp251; void* _temp253;
void* _temp255; int _temp257; void* _temp259; struct Cyc_Absyn_Exp* _temp261;
void* _temp263; struct _tagged_string* _temp265; void* _temp267; void* _temp269;
struct Cyc_Absyn_Enumfield* _temp271; struct Cyc_Absyn_Enumdecl* _temp273;
struct _tuple1* _temp275; void* _temp277; struct Cyc_Absyn_Fndecl* _temp279;
struct _tuple1* _temp281; void* _temp283; struct Cyc_Absyn_Vardecl* _temp285;
struct _tuple1* _temp287; void* _temp289; struct Cyc_Absyn_Vardecl* _temp291;
struct _tuple1* _temp293; void* _temp295; struct Cyc_Absyn_Vardecl* _temp297;
struct _tuple1* _temp299; void* _temp301; struct Cyc_Absyn_Vardecl* _temp303;
struct _tuple1* _temp305; struct Cyc_Absyn_Exp* _temp307; struct Cyc_Core_Opt*
_temp309; struct Cyc_Absyn_Exp* _temp311; struct Cyc_Absyn_Exp* _temp313; struct
Cyc_Core_Opt* _temp315; struct Cyc_Absyn_Exp* _temp317; struct Cyc_List_List*
_temp319; void* _temp321; struct Cyc_Absyn_Exp* _temp323; void* _temp325; struct
Cyc_Absyn_VarargCallInfo* _temp327; struct Cyc_List_List* _temp329; struct Cyc_Absyn_Exp*
_temp331; struct Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp* _temp335; struct
Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Exp* _temp339; struct Cyc_Absyn_Exp*
_temp341; void* _temp343; struct Cyc_Absyn_Exp* _temp345; struct Cyc_Absyn_Exp*
_temp347; struct _tagged_string* _temp349; struct Cyc_Absyn_Exp* _temp351;
struct _tagged_string* _temp353; struct Cyc_Absyn_Exp* _temp355; struct Cyc_Absyn_Tunionfield*
_temp357; struct Cyc_Absyn_Tuniondecl* _temp359; struct Cyc_List_List* _temp361;
struct Cyc_Core_Opt* _temp363; struct Cyc_Core_Opt* _temp365; struct Cyc_List_List*
_temp367; struct Cyc_List_List* _temp369; void* _temp371; struct Cyc_Absyn_Structdecl*
_temp373; struct Cyc_List_List* _temp375; struct Cyc_Core_Opt* _temp377; struct
_tuple1* _temp379; struct Cyc_List_List* _temp381; void* _temp383; struct Cyc_Absyn_Exp*
_temp385; struct Cyc_Absyn_Exp* _temp387; struct Cyc_Absyn_Exp* _temp389; struct
Cyc_Absyn_Exp* _temp391; struct Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Stmt*
_temp395; struct Cyc_Absyn_Exp* _temp397; struct Cyc_Absyn_Exp* _temp399; struct
Cyc_Absyn_Exp* _temp401; struct Cyc_Absyn_Exp* _temp403; struct Cyc_Absyn_Vardecl*
_temp405; void* _temp407; struct _tuple1* _temp409; struct _tuple1* _temp411;
struct Cyc_List_List* _temp413; struct Cyc_Absyn_Exp* _temp415; struct Cyc_List_List*
_temp417; struct Cyc_Core_Opt* _temp419; struct Cyc_List_List* _temp421; struct
_tuple2* _temp423; struct Cyc_Absyn_Fndecl* _temp425; struct Cyc_Absyn_Exp*
_temp427; _LL155: if(*(( int*) _temp153) == Cyc_Absyn_NoInstantiate_e){ _LL244:
_temp243=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp153)->f1; goto _LL156;}
else{ goto _LL157;} _LL157: if(*(( int*) _temp153) == Cyc_Absyn_Instantiate_e){
_LL248: _temp247=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp153)->f1; goto
_LL246; _LL246: _temp245=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp153)->f2;
goto _LL158;} else{ goto _LL159;} _LL159: if(*(( int*) _temp153) == Cyc_Absyn_Const_e){
_LL250: _temp249=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp153)->f1; if((
unsigned int) _temp249 > 1u?*(( int*) _temp249) == Cyc_Absyn_Int_c: 0){ _LL254:
_temp253=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp249)->f1; if( _temp253
==( void*) Cyc_Absyn_Signed){ goto _LL252;} else{ goto _LL161;} _LL252: _temp251=((
struct Cyc_Absyn_Int_c_struct*) _temp249)->f2; if( _temp251 == 0){ goto _LL160;}
else{ goto _LL161;}} else{ goto _LL161;}} else{ goto _LL161;} _LL161: if(*(( int*)
_temp153) == Cyc_Absyn_Const_e){ _LL256: _temp255=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp153)->f1; if(( unsigned int) _temp255 > 1u?*(( int*) _temp255) == Cyc_Absyn_Int_c:
0){ _LL260: _temp259=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp255)->f1;
if( _temp259 ==( void*) Cyc_Absyn_Signed){ goto _LL258;} else{ goto _LL163;}
_LL258: _temp257=(( struct Cyc_Absyn_Int_c_struct*) _temp255)->f2; if( _temp257
== 1){ goto _LL162;} else{ goto _LL163;}} else{ goto _LL163;}} else{ goto _LL163;}
_LL163: if(*(( int*) _temp153) == Cyc_Absyn_Sizeofexp_e){ _LL262: _temp261=((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp153)->f1; goto _LL164;} else{ goto
_LL165;} _LL165: if(*(( int*) _temp153) == Cyc_Absyn_Sizeoftyp_e){ _LL264:
_temp263=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp153)->f1; goto
_LL166;} else{ goto _LL167;} _LL167: if(*(( int*) _temp153) == Cyc_Absyn_Offsetof_e){
_LL268: _temp267=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp153)->f1;
goto _LL266; _LL266: _temp265=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp153)->f2;
goto _LL168;} else{ goto _LL169;} _LL169: if(*(( int*) _temp153) == Cyc_Absyn_Const_e){
_LL270: _temp269=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp153)->f1;
goto _LL170;} else{ goto _LL171;} _LL171: if(*(( int*) _temp153) == Cyc_Absyn_Enum_e){
_LL276: _temp275=(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f1; goto _LL274;
_LL274: _temp273=(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f2; goto _LL272;
_LL272: _temp271=(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f3; goto _LL172;}
else{ goto _LL173;} _LL173: if(*(( int*) _temp153) == Cyc_Absyn_Var_e){ _LL282:
_temp281=(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f1; goto _LL278; _LL278:
_temp277=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f2; if((
unsigned int) _temp277 > 1u?*(( int*) _temp277) == Cyc_Absyn_Funname_b: 0){
_LL280: _temp279=(( struct Cyc_Absyn_Funname_b_struct*) _temp277)->f1; goto
_LL174;} else{ goto _LL175;}} else{ goto _LL175;} _LL175: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL288: _temp287=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL284; _LL284: _temp283=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp283 > 1u?*(( int*) _temp283) == Cyc_Absyn_Global_b:
0){ _LL286: _temp285=(( struct Cyc_Absyn_Global_b_struct*) _temp283)->f1; goto
_LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL294: _temp293=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL290; _LL290: _temp289=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp289 > 1u?*(( int*) _temp289) == Cyc_Absyn_Param_b:
0){ _LL292: _temp291=(( struct Cyc_Absyn_Param_b_struct*) _temp289)->f1; goto
_LL178;} else{ goto _LL179;}} else{ goto _LL179;} _LL179: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL300: _temp299=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL296; _LL296: _temp295=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp295 > 1u?*(( int*) _temp295) == Cyc_Absyn_Local_b:
0){ _LL298: _temp297=(( struct Cyc_Absyn_Local_b_struct*) _temp295)->f1; goto
_LL180;} else{ goto _LL181;}} else{ goto _LL181;} _LL181: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL306: _temp305=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL302; _LL302: _temp301=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp301 > 1u?*(( int*) _temp301) == Cyc_Absyn_Pat_b:
0){ _LL304: _temp303=(( struct Cyc_Absyn_Pat_b_struct*) _temp301)->f1; goto
_LL182;} else{ goto _LL183;}} else{ goto _LL183;} _LL183: if(*(( int*) _temp153)
== Cyc_Absyn_AssignOp_e){ _LL312: _temp311=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp153)->f1; goto _LL310; _LL310: _temp309=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp153)->f2; if( _temp309 == 0){ goto _LL308;} else{ goto _LL185;} _LL308:
_temp307=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f3; goto _LL184;}
else{ goto _LL185;} _LL185: if(*(( int*) _temp153) == Cyc_Absyn_AssignOp_e){
_LL318: _temp317=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f1; goto
_LL316; _LL316: _temp315=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f2;
goto _LL314; _LL314: _temp313=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f3;
goto _LL186;} else{ goto _LL187;} _LL187: if(*(( int*) _temp153) == Cyc_Absyn_Primop_e){
_LL322: _temp321=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp153)->f1;
goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_Primop_e_struct*) _temp153)->f2;
goto _LL188;} else{ goto _LL189;} _LL189: if(*(( int*) _temp153) == Cyc_Absyn_Cast_e){
_LL326: _temp325=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp153)->f1; goto
_LL324; _LL324: _temp323=(( struct Cyc_Absyn_Cast_e_struct*) _temp153)->f2; goto
_LL190;} else{ goto _LL191;} _LL191: if(*(( int*) _temp153) == Cyc_Absyn_FnCall_e){
_LL332: _temp331=(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f1; goto
_LL330; _LL330: _temp329=(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f2;
goto _LL328; _LL328: _temp327=(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f3;
goto _LL192;} else{ goto _LL193;} _LL193: if(*(( int*) _temp153) == Cyc_Absyn_Subscript_e){
_LL336: _temp335=(( struct Cyc_Absyn_Subscript_e_struct*) _temp153)->f1; goto
_LL334; _LL334: _temp333=(( struct Cyc_Absyn_Subscript_e_struct*) _temp153)->f2;
goto _LL194;} else{ goto _LL195;} _LL195: if(*(( int*) _temp153) == Cyc_Absyn_Address_e){
_LL338: _temp337=(( struct Cyc_Absyn_Address_e_struct*) _temp153)->f1; goto
_LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp153) == Cyc_Absyn_New_e){
_LL342: _temp341=(( struct Cyc_Absyn_New_e_struct*) _temp153)->f1; goto _LL340;
_LL340: _temp339=(( struct Cyc_Absyn_New_e_struct*) _temp153)->f2; goto _LL198;}
else{ goto _LL199;} _LL199: if(*(( int*) _temp153) == Cyc_Absyn_Malloc_e){
_LL346: _temp345=(( struct Cyc_Absyn_Malloc_e_struct*) _temp153)->f1; goto
_LL344; _LL344: _temp343=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp153)->f2;
goto _LL200;} else{ goto _LL201;} _LL201: if(*(( int*) _temp153) == Cyc_Absyn_Deref_e){
_LL348: _temp347=(( struct Cyc_Absyn_Deref_e_struct*) _temp153)->f1; goto _LL202;}
else{ goto _LL203;} _LL203: if(*(( int*) _temp153) == Cyc_Absyn_StructArrow_e){
_LL352: _temp351=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp153)->f1; goto
_LL350; _LL350: _temp349=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp153)->f2;
goto _LL204;} else{ goto _LL205;} _LL205: if(*(( int*) _temp153) == Cyc_Absyn_StructMember_e){
_LL356: _temp355=(( struct Cyc_Absyn_StructMember_e_struct*) _temp153)->f1; goto
_LL354; _LL354: _temp353=(( struct Cyc_Absyn_StructMember_e_struct*) _temp153)->f2;
goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp153) == Cyc_Absyn_Tunion_e){
_LL366: _temp365=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f1; goto
_LL364; _LL364: _temp363=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f2;
goto _LL362; _LL362: _temp361=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f3;
goto _LL360; _LL360: _temp359=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f4;
goto _LL358; _LL358: _temp357=(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f5;
goto _LL208;} else{ goto _LL209;} _LL209: if(*(( int*) _temp153) == Cyc_Absyn_Tuple_e){
_LL368: _temp367=(( struct Cyc_Absyn_Tuple_e_struct*) _temp153)->f1; goto _LL210;}
else{ goto _LL211;} _LL211: if(*(( int*) _temp153) == Cyc_Absyn_AnonStruct_e){
_LL372: _temp371=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp153)->f1;
goto _LL370; _LL370: _temp369=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp153)->f2;
goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp153) == Cyc_Absyn_Struct_e){
_LL380: _temp379=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f1; goto
_LL378; _LL378: _temp377=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f2;
goto _LL376; _LL376: _temp375=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f3;
goto _LL374; _LL374: _temp373=(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f4;
goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp153) == Cyc_Absyn_Array_e){
_LL382: _temp381=(( struct Cyc_Absyn_Array_e_struct*) _temp153)->f1; goto _LL216;}
else{ goto _LL217;} _LL217: if(*(( int*) _temp153) == Cyc_Absyn_Increment_e){
_LL386: _temp385=(( struct Cyc_Absyn_Increment_e_struct*) _temp153)->f1; goto
_LL384; _LL384: _temp383=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp153)->f2; goto _LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp153)
== Cyc_Absyn_Throw_e){ _LL388: _temp387=(( struct Cyc_Absyn_Throw_e_struct*)
_temp153)->f1; goto _LL220;} else{ goto _LL221;} _LL221: if(*(( int*) _temp153)
== Cyc_Absyn_Conditional_e){ _LL394: _temp393=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f1; goto _LL392; _LL392: _temp391=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f2; goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f3; goto _LL222;} else{ goto _LL223;} _LL223: if(*(( int*) _temp153)
== Cyc_Absyn_StmtExp_e){ _LL396: _temp395=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp153)->f1; goto _LL224;} else{ goto _LL225;} _LL225: if(*(( int*) _temp153)
== Cyc_Absyn_SeqExp_e){ _LL400: _temp399=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp153)->f1; goto _LL398; _LL398: _temp397=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp153)->f2; goto _LL226;} else{ goto _LL227;} _LL227: if(*(( int*) _temp153)
== Cyc_Absyn_Comprehension_e){ _LL406: _temp405=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f1; goto _LL404; _LL404: _temp403=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f2; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f3; goto _LL228;} else{ goto _LL229;} _LL229: if(*(( int*) _temp153)
== Cyc_Absyn_Var_e){ _LL410: _temp409=(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL408; _LL408: _temp407=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if( _temp407 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL230;}
else{ goto _LL231;}} else{ goto _LL231;} _LL231: if(*(( int*) _temp153) == Cyc_Absyn_UnknownId_e){
_LL412: _temp411=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp153)->f1; goto
_LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp153) == Cyc_Absyn_UnknownCall_e){
_LL416: _temp415=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp153)->f1; goto
_LL414; _LL414: _temp413=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp153)->f2;
goto _LL234;} else{ goto _LL235;} _LL235: if(*(( int*) _temp153) == Cyc_Absyn_UnresolvedMem_e){
_LL420: _temp419=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp153)->f1;
goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp153)->f2; goto _LL236;} else{ goto _LL237;} _LL237: if(*(( int*) _temp153)
== Cyc_Absyn_CompoundLit_e){ _LL424: _temp423=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp153)->f1; goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp153)->f2; goto _LL238;} else{ goto _LL239;} _LL239: if(*(( int*) _temp153)
== Cyc_Absyn_Codegen_e){ _LL426: _temp425=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp153)->f1; goto _LL240;} else{ goto _LL241;} _LL241: if(*(( int*) _temp153)
== Cyc_Absyn_Fill_e){ _LL428: _temp427=(( struct Cyc_Absyn_Fill_e_struct*)
_temp153)->f1; goto _LL242;} else{ goto _LL154;} _LL156: _temp247= _temp243;
goto _LL158; _LL158: return Cyc_NewControlFlow_abstract_exp( env, _temp247);
_LL160: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({
unsigned char* _temp429=( unsigned char*)"new_control_flow.cyc:284 env.access_path==null";
struct _tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 47; _temp430;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp431; _temp431.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp433;
_temp433.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp433.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp433;}); _temp431.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp432; _temp432.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp432.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp432;}); _temp431.lvalues= 0; _temp431;}); _LL162: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp434=( unsigned char*)"new_control_flow.cyc:289 env.access_path==null";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 47; _temp435;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp436; _temp436.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp438;
_temp438.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp438.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp438;});
_temp436.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp437; _temp437.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp437.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp437;}); _temp436.lvalues= 0; _temp436;}); _LL164: goto _LL166; _LL166: goto
_LL168; _LL168: goto _LL170; _LL170: goto _LL172; _LL172: goto _LL174; _LL174:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL176: return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL178: _temp297= _temp291; goto _LL180;
_LL180: _temp303= _temp297; goto _LL182; _LL182: { void* absop= Cyc_NewControlFlow_destruct_path(
env, Cyc_CfAbsexp_mkLocalOp( _temp303)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp439=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp439->hd=( void*) absop; _temp439->tl= 0; _temp439;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL184: { struct Cyc_NewControlFlow_AbsSyn
_temp440= Cyc_NewControlFlow_abstract_exp( env, _temp311); goto _LL441; _LL441: {
struct Cyc_List_List* _temp442= _temp440.lvalues == 0? Cyc_NewControlFlow_use_it_list():
_temp440.lvalues; goto _LL443; _LL443: { struct Cyc_NewControlFlow_AbsSyn
_temp444= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp442), _temp307); goto _LL445; _LL445: { void* _temp448; void* _temp450;
struct Cyc_NewControlFlow_AbsSynOne _temp446= Cyc_NewControlFlow_meet_absexp(
_temp440); _LL451: _temp450=( void*) _temp446.inner_exp; goto _LL449; _LL449:
_temp448=( void*) _temp446.assigns; goto _LL447; _LL447: { void* _temp454; void*
_temp456; struct Cyc_NewControlFlow_AbsSynOne _temp452= Cyc_NewControlFlow_meet_absexp(
_temp444); _LL457: _temp456=( void*) _temp452.inner_exp; goto _LL455; _LL455:
_temp454=( void*) _temp452.assigns; goto _LL453; _LL453: { void* _temp458= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp450, _temp456), _temp454); goto _LL459; _LL459: return Cyc_NewControlFlow_mkSyn_tf(
_temp458, 0, _temp448);}}}}}} _LL186: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp460=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp460->hd=( void*) _temp317; _temp460->tl=({
struct Cyc_List_List* _temp461=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp461->hd=( void*) _temp313; _temp461->tl= 0;
_temp461;}); _temp460;}), 0); _LL188: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp319, 0); _LL190: { int both_leaf= 1;{ void* _temp462=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp323->topt))->v; struct Cyc_List_List* _temp472;
struct Cyc_Absyn_Structdecl** _temp474; struct Cyc_List_List* _temp476; struct
_tuple1* _temp478; struct Cyc_List_List* _temp480; _LL464: if(( unsigned int)
_temp462 > 4u?*(( int*) _temp462) == Cyc_Absyn_AnonStructType: 0){ _LL473:
_temp472=(( struct Cyc_Absyn_AnonStructType_struct*) _temp462)->f1; goto _LL465;}
else{ goto _LL466;} _LL466: if(( unsigned int) _temp462 > 4u?*(( int*) _temp462)
== Cyc_Absyn_StructType: 0){ _LL479: _temp478=(( struct Cyc_Absyn_StructType_struct*)
_temp462)->f1; goto _LL477; _LL477: _temp476=(( struct Cyc_Absyn_StructType_struct*)
_temp462)->f2; goto _LL475; _LL475: _temp474=(( struct Cyc_Absyn_StructType_struct*)
_temp462)->f3; goto _LL467;} else{ goto _LL468;} _LL468: if(( unsigned int)
_temp462 > 4u?*(( int*) _temp462) == Cyc_Absyn_TupleType: 0){ _LL481: _temp480=((
struct Cyc_Absyn_TupleType_struct*) _temp462)->f1; goto _LL469;} else{ goto
_LL470;} _LL470: goto _LL471; _LL465: goto _LL467; _LL467: goto _LL469; _LL469:
both_leaf= 0; goto _LL463; _LL471: goto _LL463; _LL463:;}{ void* _temp482=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; struct Cyc_List_List* _temp492;
struct Cyc_Absyn_Structdecl** _temp494; struct Cyc_List_List* _temp496; struct
_tuple1* _temp498; struct Cyc_List_List* _temp500; _LL484: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_AnonStructType: 0){ _LL493:
_temp492=(( struct Cyc_Absyn_AnonStructType_struct*) _temp482)->f1; goto _LL485;}
else{ goto _LL486;} _LL486: if(( unsigned int) _temp482 > 4u?*(( int*) _temp482)
== Cyc_Absyn_StructType: 0){ _LL499: _temp498=(( struct Cyc_Absyn_StructType_struct*)
_temp482)->f1; goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_StructType_struct*)
_temp482)->f2; goto _LL495; _LL495: _temp494=(( struct Cyc_Absyn_StructType_struct*)
_temp482)->f3; goto _LL487;} else{ goto _LL488;} _LL488: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_TupleType: 0){ _LL501: _temp500=((
struct Cyc_Absyn_TupleType_struct*) _temp482)->f1; goto _LL489;} else{ goto
_LL490;} _LL490: goto _LL491; _LL485: goto _LL487; _LL487: goto _LL489; _LL489:
both_leaf= 0; goto _LL483; _LL491: goto _LL483; _LL483:;} if( both_leaf){ return
Cyc_NewControlFlow_abstract_exp( env, _temp323);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp502=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp502->hd=( void*) _temp323; _temp502->tl= 0;
_temp502;}), env.lhsides);} _LL192: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp503=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp503->hd=( void*) _temp331; _temp503->tl=
_temp329; _temp503;}), 0); _LL194: { void* _temp504= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp335->topt))->v); struct Cyc_List_List*
_temp510; _LL506: if(( unsigned int) _temp504 > 4u?*(( int*) _temp504) == Cyc_Absyn_TupleType:
0){ _LL511: _temp510=(( struct Cyc_Absyn_TupleType_struct*) _temp504)->f1; goto
_LL507;} else{ goto _LL508;} _LL508: goto _LL509; _LL507: return Cyc_NewControlFlow_descend_path(
env, e, _temp335); _LL509: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp512=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp512->hd=( void*) _temp335; _temp512->tl=({ struct
Cyc_List_List* _temp513=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp513->hd=( void*) _temp333; _temp513->tl= 0; _temp513;}); _temp512;}), Cyc_NewControlFlow_use_it_list());
_LL505:;} _LL196: { int is_malloc= 0;{ void* _temp514=( void*) _temp337->r;
struct Cyc_Absyn_Structdecl* _temp522; struct Cyc_List_List* _temp524; struct
Cyc_Core_Opt* _temp526; struct _tuple1* _temp528; struct Cyc_List_List* _temp530;
_LL516: if(*(( int*) _temp514) == Cyc_Absyn_Struct_e){ _LL529: _temp528=((
struct Cyc_Absyn_Struct_e_struct*) _temp514)->f1; goto _LL527; _LL527: _temp526=((
struct Cyc_Absyn_Struct_e_struct*) _temp514)->f2; goto _LL525; _LL525: _temp524=((
struct Cyc_Absyn_Struct_e_struct*) _temp514)->f3; goto _LL523; _LL523: _temp522=((
struct Cyc_Absyn_Struct_e_struct*) _temp514)->f4; goto _LL517;} else{ goto
_LL518;} _LL518: if(*(( int*) _temp514) == Cyc_Absyn_Tuple_e){ _LL531: _temp530=((
struct Cyc_Absyn_Tuple_e_struct*) _temp514)->f1; goto _LL519;} else{ goto _LL520;}
_LL520: goto _LL521; _LL517: goto _LL519; _LL519: is_malloc= 1; goto _LL515;
_LL521: goto _LL515; _LL515:;} if( is_malloc){ _temp341= 0; _temp339= _temp337;
goto _LL198;}{ struct Cyc_List_List* _temp532= env.access_path; struct Cyc_List_List
_temp544; struct Cyc_List_List* _temp545; struct Cyc_Absyn_Exp* _temp547; struct
Cyc_Absyn_Exp _temp549; struct Cyc_Position_Segment* _temp550; void* _temp552;
struct Cyc_Absyn_Exp* _temp554; struct Cyc_Absyn_Exp* _temp556; struct Cyc_Core_Opt*
_temp558; struct Cyc_List_List _temp560; struct Cyc_List_List* _temp561; struct
Cyc_Absyn_Exp* _temp563; struct Cyc_Absyn_Exp _temp565; struct Cyc_Position_Segment*
_temp566; void* _temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Core_Opt*
_temp572; struct Cyc_List_List _temp574; struct Cyc_List_List* _temp575; struct
Cyc_Absyn_Exp* _temp577; struct Cyc_Absyn_Exp _temp579; struct Cyc_Position_Segment*
_temp580; void* _temp582; struct _tagged_string* _temp584; struct Cyc_Absyn_Exp*
_temp586; struct Cyc_Core_Opt* _temp588; _LL534: if( _temp532 == 0){ goto _LL535;}
else{ goto _LL536;} _LL536: if( _temp532 == 0){ goto _LL538;} else{ _temp544=*
_temp532; _LL548: _temp547=( struct Cyc_Absyn_Exp*) _temp544.hd; _temp549=*
_temp547; _LL559: _temp558= _temp549.topt; goto _LL553; _LL553: _temp552=( void*)
_temp549.r; if(*(( int*) _temp552) == Cyc_Absyn_Subscript_e){ _LL557: _temp556=((
struct Cyc_Absyn_Subscript_e_struct*) _temp552)->f1; goto _LL555; _LL555:
_temp554=(( struct Cyc_Absyn_Subscript_e_struct*) _temp552)->f2; goto _LL551;}
else{ goto _LL538;} _LL551: _temp550= _temp549.loc; goto _LL546; _LL546:
_temp545= _temp544.tl; goto _LL537;} _LL538: if( _temp532 == 0){ goto _LL540;}
else{ _temp560=* _temp532; _LL564: _temp563=( struct Cyc_Absyn_Exp*) _temp560.hd;
_temp565=* _temp563; _LL573: _temp572= _temp565.topt; goto _LL569; _LL569:
_temp568=( void*) _temp565.r; if(*(( int*) _temp568) == Cyc_Absyn_Deref_e){
_LL571: _temp570=(( struct Cyc_Absyn_Deref_e_struct*) _temp568)->f1; goto _LL567;}
else{ goto _LL540;} _LL567: _temp566= _temp565.loc; goto _LL562; _LL562:
_temp561= _temp560.tl; goto _LL539;} _LL540: if( _temp532 == 0){ goto _LL542;}
else{ _temp574=* _temp532; _LL578: _temp577=( struct Cyc_Absyn_Exp*) _temp574.hd;
_temp579=* _temp577; _LL589: _temp588= _temp579.topt; goto _LL583; _LL583:
_temp582=( void*) _temp579.r; if(*(( int*) _temp582) == Cyc_Absyn_StructArrow_e){
_LL587: _temp586=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp582)->f1; goto
_LL585; _LL585: _temp584=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp582)->f2;
goto _LL581;} else{ goto _LL542;} _LL581: _temp580= _temp579.loc; goto _LL576;
_LL576: _temp575= _temp574.tl; goto _LL541;} _LL542: goto _LL543; _LL535: env.access_path=({
struct Cyc_List_List* _temp590=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp590->hd=( void*) e; _temp590->tl= env.access_path;
_temp590;}); goto _LL533; _LL537: Cyc_Evexp_eval_const_uint_exp( _temp554) == 0?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp591=( unsigned char*)"new_control_flow.cyc:413 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp592; _temp592.curr= _temp591; _temp592.base= _temp591;
_temp592.last_plus_one= _temp591 + 61; _temp592;})); _temp561= _temp545; goto
_LL539; _LL539: env.access_path= _temp561; goto _LL533; _LL541: env.access_path=({
struct Cyc_List_List* _temp593=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp593->hd=( void*) Cyc_Absyn_structmember_exp(
_temp586, _temp584, 0); _temp593->tl= _temp575; _temp593;}); goto _LL533; _LL543:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp594=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp594[ 0]=({ struct
Cyc_Core_Impossible_struct _temp595; _temp595.tag= Cyc_Core_Impossible; _temp595.f1=({
unsigned char* _temp596=( unsigned char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 36; _temp597;}); _temp595;}); _temp594;}));
_LL533:;} return Cyc_NewControlFlow_abstract_exp( env, _temp337);} _LL198: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp339->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, _temp341,( struct Cyc_Absyn_Exp*)
_temp339); _LL200: Cyc_NewControlFlow_add_malloc_root( env, e, _temp343); return
Cyc_NewControlFlow_abstract_malloc( env, e, _temp345, 0); _LL202: { struct Cyc_List_List*
_temp598= env.access_path; struct Cyc_List_List _temp604; struct Cyc_List_List*
_temp605; struct Cyc_Absyn_Exp* _temp607; struct Cyc_Absyn_Exp _temp609; struct
Cyc_Position_Segment* _temp610; void* _temp612; struct Cyc_Absyn_Exp* _temp614;
struct Cyc_Core_Opt* _temp616; _LL600: if( _temp598 == 0){ goto _LL602;} else{
_temp604=* _temp598; _LL608: _temp607=( struct Cyc_Absyn_Exp*) _temp604.hd;
_temp609=* _temp607; _LL617: _temp616= _temp609.topt; goto _LL613; _LL613:
_temp612=( void*) _temp609.r; if(*(( int*) _temp612) == Cyc_Absyn_Address_e){
_LL615: _temp614=(( struct Cyc_Absyn_Address_e_struct*) _temp612)->f1; goto
_LL611;} else{ goto _LL602;} _LL611: _temp610= _temp609.loc; goto _LL606; _LL606:
_temp605= _temp604.tl; goto _LL601;} _LL602: goto _LL603; _LL601: env.access_path=
_temp605;{ struct Cyc_NewControlFlow_AbsSyn _temp618= Cyc_NewControlFlow_abstract_exp(
env, _temp347); goto _LL619; _LL619: if( _temp618.lvalues == 0){ _temp618.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp618;} _LL603: return Cyc_NewControlFlow_descend_path(
env, e, _temp347); _LL599:;} _LL204: { void* _temp620= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp351->topt))->v); struct Cyc_Absyn_PtrInfo
_temp626; struct Cyc_Absyn_Conref* _temp628; struct Cyc_Absyn_Tqual _temp630;
struct Cyc_Absyn_Conref* _temp632; void* _temp634; void* _temp636; _LL622: if((
unsigned int) _temp620 > 4u?*(( int*) _temp620) == Cyc_Absyn_PointerType: 0){
_LL627: _temp626=(( struct Cyc_Absyn_PointerType_struct*) _temp620)->f1; _LL637:
_temp636=( void*) _temp626.elt_typ; goto _LL635; _LL635: _temp634=( void*)
_temp626.rgn_typ; goto _LL633; _LL633: _temp632= _temp626.nullable; goto _LL631;
_LL631: _temp630= _temp626.tq; goto _LL629; _LL629: _temp628= _temp626.bounds;
goto _LL623;} else{ goto _LL624;} _LL624: goto _LL625; _LL623:{ void* _temp638=
Cyc_Tcutil_compress( _temp636); struct Cyc_List_List* _temp650; struct Cyc_Absyn_Structdecl**
_temp652; struct Cyc_List_List* _temp654; struct _tuple1* _temp656; struct Cyc_List_List*
_temp658; struct Cyc_Absyn_Uniondecl** _temp660; struct Cyc_List_List* _temp662;
struct _tuple1* _temp664; _LL640: if(( unsigned int) _temp638 > 4u?*(( int*)
_temp638) == Cyc_Absyn_AnonStructType: 0){ _LL651: _temp650=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp638)->f1; goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int)
_temp638 > 4u?*(( int*) _temp638) == Cyc_Absyn_StructType: 0){ _LL657: _temp656=((
struct Cyc_Absyn_StructType_struct*) _temp638)->f1; goto _LL655; _LL655:
_temp654=(( struct Cyc_Absyn_StructType_struct*) _temp638)->f2; goto _LL653;
_LL653: _temp652=(( struct Cyc_Absyn_StructType_struct*) _temp638)->f3; goto
_LL643;} else{ goto _LL644;} _LL644: if(( unsigned int) _temp638 > 4u?*(( int*)
_temp638) == Cyc_Absyn_AnonUnionType: 0){ _LL659: _temp658=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp638)->f1; goto _LL645;} else{ goto _LL646;} _LL646: if(( unsigned int)
_temp638 > 4u?*(( int*) _temp638) == Cyc_Absyn_UnionType: 0){ _LL665: _temp664=((
struct Cyc_Absyn_UnionType_struct*) _temp638)->f1; goto _LL663; _LL663: _temp662=((
struct Cyc_Absyn_UnionType_struct*) _temp638)->f2; goto _LL661; _LL661: _temp660=((
struct Cyc_Absyn_UnionType_struct*) _temp638)->f3; goto _LL647;} else{ goto
_LL648;} _LL648: goto _LL649; _LL641: goto _LL643; _LL643: return Cyc_NewControlFlow_descend_path(
env, e, _temp351); _LL645: goto _LL647; _LL647: { struct Cyc_NewControlFlow_AbsSyn
_temp666= Cyc_NewControlFlow_abstract_exp( env, _temp351); goto _LL667; _LL667:
_temp666.lvalues= Cyc_NewControlFlow_use_it_list(); return _temp666;} _LL649:
goto _LL639; _LL639:;} goto _LL625; _LL625:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp668=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp668[ 0]=({ struct Cyc_Core_Impossible_struct _temp669; _temp669.tag= Cyc_Core_Impossible;
_temp669.f1=({ unsigned char* _temp670=( unsigned char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp671; _temp671.curr= _temp670; _temp671.base= _temp670;
_temp671.last_plus_one= _temp670 + 40; _temp671;}); _temp669;}); _temp668;}));
_LL621:;} _LL206: { void* _temp672= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp355->topt))->v); struct Cyc_List_List* _temp684; struct Cyc_Absyn_Structdecl**
_temp686; struct Cyc_List_List* _temp688; struct _tuple1* _temp690; struct Cyc_List_List*
_temp692; struct Cyc_Absyn_Uniondecl** _temp694; struct Cyc_List_List* _temp696;
struct _tuple1* _temp698; _LL674: if(( unsigned int) _temp672 > 4u?*(( int*)
_temp672) == Cyc_Absyn_AnonStructType: 0){ _LL685: _temp684=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp672)->f1; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp672 > 4u?*(( int*) _temp672) == Cyc_Absyn_StructType: 0){ _LL691: _temp690=((
struct Cyc_Absyn_StructType_struct*) _temp672)->f1; goto _LL689; _LL689:
_temp688=(( struct Cyc_Absyn_StructType_struct*) _temp672)->f2; goto _LL687;
_LL687: _temp686=(( struct Cyc_Absyn_StructType_struct*) _temp672)->f3; goto
_LL677;} else{ goto _LL678;} _LL678: if(( unsigned int) _temp672 > 4u?*(( int*)
_temp672) == Cyc_Absyn_AnonUnionType: 0){ _LL693: _temp692=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp672)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int)
_temp672 > 4u?*(( int*) _temp672) == Cyc_Absyn_UnionType: 0){ _LL699: _temp698=((
struct Cyc_Absyn_UnionType_struct*) _temp672)->f1; goto _LL697; _LL697: _temp696=((
struct Cyc_Absyn_UnionType_struct*) _temp672)->f2; goto _LL695; _LL695: _temp694=((
struct Cyc_Absyn_UnionType_struct*) _temp672)->f3; goto _LL681;} else{ goto
_LL682;} _LL682: goto _LL683; _LL675: goto _LL677; _LL677: return Cyc_NewControlFlow_descend_path(
env, e, _temp355); _LL679: goto _LL681; _LL681: { struct Cyc_NewControlFlow_AbsSyn
_temp700= Cyc_NewControlFlow_abstract_exp( env, _temp355); goto _LL701; _LL701:
_temp700.lvalues= Cyc_NewControlFlow_use_it_list(); return _temp700;} _LL683:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp702=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp702[ 0]=({ struct
Cyc_Core_Impossible_struct _temp703; _temp703.tag= Cyc_Core_Impossible; _temp703.f1=({
unsigned char* _temp704=( unsigned char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp705; _temp705.curr= _temp704; _temp705.base= _temp704;
_temp705.last_plus_one= _temp704 + 41; _temp705;}); _temp703;}); _temp702;}));
_LL673:;} _LL208: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(({ unsigned char* _temp706=( unsigned char*)"new_control_flow.cyc:483 env.access_path == null";
struct _tagged_string _temp707; _temp707.curr= _temp706; _temp707.base= _temp706;
_temp707.last_plus_one= _temp706 + 49; _temp707;})); if( _temp361 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));} _temp367=
_temp361; goto _LL210; _LL210: { struct Cyc_List_List* _temp708= env.access_path;
struct Cyc_List_List _temp716; struct Cyc_List_List* _temp717; struct Cyc_Absyn_Exp*
_temp719; struct Cyc_Absyn_Exp _temp721; struct Cyc_Position_Segment* _temp722;
void* _temp724; struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Exp* _temp728;
struct Cyc_Core_Opt* _temp730; _LL710: if( _temp708 == 0){ goto _LL711;} else{
goto _LL712;} _LL712: if( _temp708 == 0){ goto _LL714;} else{ _temp716=*
_temp708; _LL720: _temp719=( struct Cyc_Absyn_Exp*) _temp716.hd; _temp721=*
_temp719; _LL731: _temp730= _temp721.topt; goto _LL725; _LL725: _temp724=( void*)
_temp721.r; if(*(( int*) _temp724) == Cyc_Absyn_Subscript_e){ _LL729: _temp728=((
struct Cyc_Absyn_Subscript_e_struct*) _temp724)->f1; goto _LL727; _LL727:
_temp726=(( struct Cyc_Absyn_Subscript_e_struct*) _temp724)->f2; goto _LL723;}
else{ goto _LL714;} _LL723: _temp722= _temp721.loc; goto _LL718; _LL718:
_temp717= _temp716.tl; goto _LL713;} _LL714: goto _LL715; _LL711: { void*
_temp732= Cyc_CfAbsexp_mkSkipAE(); goto _LL733; _LL733: { void* _temp734= Cyc_CfAbsexp_mkSkipAE();
goto _LL735; _LL735:{ int j= 0; for( 0; _temp367 != 0;( _temp367=(( struct Cyc_List_List*)
_check_null( _temp367))->tl, ++ j)){ struct Cyc_List_List* _temp736= 0; goto
_LL737; _LL737:{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=((
struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp736=({ struct Cyc_List_List*
_temp738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp738->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp738->tl= _temp736;
_temp738;});} else{ _temp736=({ struct Cyc_List_List* _temp739=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp739->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp740=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp740[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp741; _temp741.tag= Cyc_CfFlowInfo_TupleF;
_temp741.f1= j; _temp741;}); _temp740;})); _temp739->tl= _temp736; _temp739;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp742= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp736),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp367))->hd); goto _LL743; _LL743: { void* _temp746; void* _temp748; struct
Cyc_NewControlFlow_AbsSynOne _temp744= Cyc_NewControlFlow_meet_absexp( _temp742);
_LL749: _temp748=( void*) _temp744.inner_exp; goto _LL747; _LL747: _temp746=(
void*) _temp744.assigns; goto _LL745; _LL745: _temp732= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp732, _temp748); _temp734= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp734, _temp746);}}}} return Cyc_NewControlFlow_mkSyn_tf(
_temp732, 0, _temp734);}} _LL713: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp726); struct Cyc_NewControlFlow_AbsEnv _temp750= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL751; _LL751: _temp750.access_path= 0; env.access_path= _temp717;{
void* _temp752= Cyc_CfAbsexp_mkSkipAE(); goto _LL753; _LL753: { void* _temp754=
Cyc_CfAbsexp_mkSkipAE(); goto _LL755; _LL755:{ int j= 0; for( 0; _temp367 != 0;(
_temp367=(( struct Cyc_List_List*) _check_null( _temp367))->tl, ++ j)){ if( i ==
j){ void* _temp758; void* _temp760; struct Cyc_NewControlFlow_AbsSynOne _temp756=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)); _LL761:
_temp760=( void*) _temp756.inner_exp; goto _LL759; _LL759: _temp758=( void*)
_temp756.assigns; goto _LL757; _LL757: _temp754= _temp758; _temp752= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp752, _temp760);} else{ void* _temp764; void*
_temp766; struct Cyc_NewControlFlow_AbsSynOne _temp762= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp750,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp367))->hd)); _LL767: _temp766=( void*) _temp762.inner_exp;
goto _LL765; _LL765: _temp764=( void*) _temp762.assigns; goto _LL763; _LL763:
_temp752= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp752,
_temp766);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp752,({ struct Cyc_List_List*
_temp768=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp768->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp768->tl= 0; _temp768;}),
_temp754);}}} _LL715:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp769=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp769[ 0]=({ struct Cyc_Core_Impossible_struct _temp770; _temp770.tag= Cyc_Core_Impossible;
_temp770.f1=({ unsigned char* _temp771=( unsigned char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp772; _temp772.curr= _temp771; _temp772.base= _temp771;
_temp772.last_plus_one= _temp771 + 46; _temp772;}); _temp770;}); _temp769;}));
_LL709:;} _LL212: _temp375= _temp369; goto _LL214; _LL214: { struct Cyc_List_List*
_temp773= env.access_path; struct Cyc_List_List _temp781; struct Cyc_List_List*
_temp782; struct Cyc_Absyn_Exp* _temp784; struct Cyc_Absyn_Exp _temp786; struct
Cyc_Position_Segment* _temp787; void* _temp789; struct _tagged_string* _temp791;
struct Cyc_Absyn_Exp* _temp793; struct Cyc_Core_Opt* _temp795; _LL775: if(
_temp773 == 0){ goto _LL776;} else{ goto _LL777;} _LL777: if( _temp773 == 0){
goto _LL779;} else{ _temp781=* _temp773; _LL785: _temp784=( struct Cyc_Absyn_Exp*)
_temp781.hd; _temp786=* _temp784; _LL796: _temp795= _temp786.topt; goto _LL790;
_LL790: _temp789=( void*) _temp786.r; if(*(( int*) _temp789) == Cyc_Absyn_StructMember_e){
_LL794: _temp793=(( struct Cyc_Absyn_StructMember_e_struct*) _temp789)->f1; goto
_LL792; _LL792: _temp791=(( struct Cyc_Absyn_StructMember_e_struct*) _temp789)->f2;
goto _LL788;} else{ goto _LL779;} _LL788: _temp787= _temp786.loc; goto _LL783;
_LL783: _temp782= _temp781.tl; goto _LL778;} _LL779: goto _LL780; _LL776: { void*
_temp797= Cyc_CfAbsexp_mkSkipAE(); goto _LL798; _LL798: { void* _temp799= Cyc_CfAbsexp_mkSkipAE();
goto _LL800; _LL800: for( 0; _temp375 != 0; _temp375=(( struct Cyc_List_List*)
_check_null( _temp375))->tl){ struct Cyc_List_List* _temp801= 0; goto _LL802;
_LL802:{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=(( struct
Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)((
struct Cyc_List_List*) _check_null( ls))->hd)){ _temp801=({ struct Cyc_List_List*
_temp803=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp803->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp803->tl= _temp801;
_temp803;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp375))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp804=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp810; struct _tagged_string*
_temp812; _LL806: if(*(( int*) _temp804) == Cyc_Absyn_ArrayElement){ _LL811:
_temp810=(( struct Cyc_Absyn_ArrayElement_struct*) _temp804)->f1; goto _LL807;}
else{ goto _LL808;} _LL808: if(*(( int*) _temp804) == Cyc_Absyn_FieldName){
_LL813: _temp812=(( struct Cyc_Absyn_FieldName_struct*) _temp804)->f1; goto
_LL809;} else{ goto _LL805;} _LL807:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp814=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp814[ 0]=({ struct Cyc_Core_Impossible_struct _temp815; _temp815.tag= Cyc_Core_Impossible;
_temp815.f1=({ unsigned char* _temp816=( unsigned char*)"bad struct designator";
struct _tagged_string _temp817; _temp817.curr= _temp816; _temp817.base= _temp816;
_temp817.last_plus_one= _temp816 + 22; _temp817;}); _temp815;}); _temp814;}));
_LL809: _temp801=({ struct Cyc_List_List* _temp818=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp818->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp819=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp819[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp820; _temp820.tag= Cyc_CfFlowInfo_StructF;
_temp820.f1= _temp812; _temp820;}); _temp819;})); _temp818->tl= _temp801;
_temp818;}); goto _LL805; _LL805:;}}}}{ void** _temp821=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp375))->hd)).f2)->topt))->v); goto _LL822; _LL822: { struct Cyc_NewControlFlow_AbsSyn
_temp823= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp801),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp375))->hd)).f2);
goto _LL824; _LL824: { void* _temp827; void* _temp829; struct Cyc_NewControlFlow_AbsSynOne
_temp825= Cyc_NewControlFlow_meet_absexp( _temp823); _LL830: _temp829=( void*)
_temp825.inner_exp; goto _LL828; _LL828: _temp827=( void*) _temp825.assigns;
goto _LL826; _LL826: _temp797= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp797, _temp829); _temp799= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp799, _temp827);}}}} return Cyc_NewControlFlow_mkSyn_tf( _temp797, 0,
_temp799);}} _LL778: { struct Cyc_NewControlFlow_AbsEnv _temp831= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL832; _LL832: _temp831.access_path= 0; env.access_path= _temp782;{
void* _temp833= Cyc_CfAbsexp_mkSkipAE(); goto _LL834; _LL834: { void* _temp835=
Cyc_CfAbsexp_mkSkipAE(); goto _LL836; _LL836: for( 0; _temp375 != 0; _temp375=((
struct Cyc_List_List*) _check_null( _temp375))->tl){ int used= 0;{ struct Cyc_List_List*
ds=(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp375))->hd)).f1;
for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void*
_temp837=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp843; struct _tagged_string* _temp845; _LL839: if(*(( int*) _temp837) == Cyc_Absyn_ArrayElement){
_LL844: _temp843=(( struct Cyc_Absyn_ArrayElement_struct*) _temp837)->f1; goto
_LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp837) == Cyc_Absyn_FieldName){
_LL846: _temp845=(( struct Cyc_Absyn_FieldName_struct*) _temp837)->f1; goto
_LL842;} else{ goto _LL838;} _LL840:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp847=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp847[ 0]=({ struct Cyc_Core_Impossible_struct _temp848; _temp848.tag= Cyc_Core_Impossible;
_temp848.f1=({ unsigned char* _temp849=( unsigned char*)"bad struct designator";
struct _tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 22; _temp850;}); _temp848;}); _temp847;}));
_LL842: if( Cyc_String_zstrptrcmp( _temp791, _temp845) == 0){ used= 1;} goto
_LL838; _LL838:;}} if( used){ void* _temp853; void* _temp855; struct Cyc_NewControlFlow_AbsSynOne
_temp851= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp375))->hd)).f2));
_LL856: _temp855=( void*) _temp851.inner_exp; goto _LL854; _LL854: _temp853=(
void*) _temp851.assigns; goto _LL852; _LL852: _temp835= _temp853; _temp833= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp833, _temp855);} else{ void* _temp859; void*
_temp861; struct Cyc_NewControlFlow_AbsSynOne _temp857= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp831,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp375))->hd)).f2)); _LL862: _temp861=( void*) _temp857.inner_exp;
goto _LL860; _LL860: _temp859=( void*) _temp857.assigns; goto _LL858; _LL858:
_temp833= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp833,
_temp861);}} return Cyc_NewControlFlow_mkSyn_tf( _temp833, 0, _temp835);}}}
_LL780:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp863=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp863[ 0]=({ struct Cyc_Core_Impossible_struct _temp864; _temp864.tag= Cyc_Core_Impossible;
_temp864.f1=({ unsigned char* _temp865=( unsigned char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp866; _temp866.curr= _temp865; _temp866.base= _temp865;
_temp866.last_plus_one= _temp865 + 47; _temp866;}); _temp864;}); _temp863;}));
_LL774:;} _LL216: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd, _temp381), 0); _LL218:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp867=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp867->hd=( void*) _temp385; _temp867->tl= 0; _temp867;}), 0); _LL220: {
struct Cyc_NewControlFlow_AbsSyn _temp868= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp387); goto _LL869; _LL869: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( _temp868)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL222: { struct Cyc_List_List*
_temp872; struct Cyc_NewControlFlow_AbsSynOne _temp874; struct Cyc_NewControlFlow_AbsSynOne
_temp876; struct Cyc_NewControlFlow_AbsSyn _temp870= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp393); _LL877: _temp876= _temp870.when_true;
goto _LL875; _LL875: _temp874= _temp870.when_false; goto _LL873; _LL873:
_temp872= _temp870.lvalues; goto _LL871; _LL871: { struct Cyc_List_List*
_temp880; struct Cyc_NewControlFlow_AbsSynOne _temp882; struct Cyc_NewControlFlow_AbsSynOne
_temp884; struct Cyc_NewControlFlow_AbsSyn _temp878= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp391); _LL885: _temp884= _temp878.when_true;
goto _LL883; _LL883: _temp882= _temp878.when_false; goto _LL881; _LL881:
_temp880= _temp878.lvalues; goto _LL879; _LL879: { struct Cyc_List_List*
_temp888; struct Cyc_NewControlFlow_AbsSynOne _temp890; struct Cyc_NewControlFlow_AbsSynOne
_temp892; struct Cyc_NewControlFlow_AbsSyn _temp886= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp389); _LL893: _temp892= _temp886.when_true;
goto _LL891; _LL891: _temp890= _temp886.when_false; goto _LL889; _LL889:
_temp888= _temp886.lvalues; goto _LL887; _LL887: { void* _temp894= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp876.inner_exp,(
void*) _temp876.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp884.inner_exp,( void*) _temp884.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp874.inner_exp,( void*) _temp874.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp892.inner_exp,( void*) _temp892.assigns)));
goto _LL895; _LL895: { void* _temp896= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp876.inner_exp,( void*) _temp876.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp882.inner_exp,(
void*) _temp882.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp874.inner_exp,(
void*) _temp874.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp890.inner_exp,( void*) _temp890.assigns))); goto _LL897; _LL897:
return({ struct Cyc_NewControlFlow_AbsSyn _temp898; _temp898.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp900; _temp900.inner_exp=( void*) _temp894;
_temp900.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp900;}); _temp898.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp899;
_temp899.inner_exp=( void*) _temp896; _temp899.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp899;}); _temp898.lvalues= 0; _temp898;});}}}}}
_LL224: { struct Cyc_NewControlFlow_AbsEnv _temp901= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL902; _LL902: _temp901.access_path= 0; Cyc_NewControlFlow_cf_prepass(
_temp901, _temp395);{ struct Cyc_Absyn_Stmt* _temp903= _temp395; goto _LL904;
_LL904: while( 1) { void* _temp905=( void*) _temp903->r; struct Cyc_Absyn_Stmt*
_temp915; struct Cyc_Absyn_Stmt* _temp917; struct Cyc_Absyn_Stmt* _temp919;
struct Cyc_Absyn_Decl* _temp921; struct Cyc_Absyn_Exp* _temp923; _LL907: if((
unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Seq_s: 0){ _LL918:
_temp917=(( struct Cyc_Absyn_Seq_s_struct*) _temp905)->f1; goto _LL916; _LL916:
_temp915=(( struct Cyc_Absyn_Seq_s_struct*) _temp905)->f2; goto _LL908;} else{
goto _LL909;} _LL909: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) ==
Cyc_Absyn_Decl_s: 0){ _LL922: _temp921=(( struct Cyc_Absyn_Decl_s_struct*)
_temp905)->f1; goto _LL920; _LL920: _temp919=(( struct Cyc_Absyn_Decl_s_struct*)
_temp905)->f2; goto _LL910;} else{ goto _LL911;} _LL911: if(( unsigned int)
_temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Exp_s: 0){ _LL924: _temp923=((
struct Cyc_Absyn_Exp_s_struct*) _temp905)->f1; goto _LL912;} else{ goto _LL913;}
_LL913: goto _LL914; _LL908: _temp903= _temp915; continue; _LL910: _temp903=
_temp919; continue; _LL912: { struct Cyc_NewControlFlow_AbsSyn _temp925= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp923); goto _LL926; _LL926: { void*
_temp929; void* _temp931; struct Cyc_NewControlFlow_AbsSynOne _temp927= Cyc_NewControlFlow_meet_absexp(
_temp925); _LL932: _temp931=( void*) _temp927.inner_exp; goto _LL930; _LL930:
_temp929=( void*) _temp927.assigns; goto _LL928; _LL928:( void*)((( struct Cyc_List_List*)
_check_null(( Cyc_NewControlFlow_get_stmt_annot( _temp903))->absexps))->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp931, _temp929));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp395), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}} _LL914:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp933=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp933[ 0]=({ struct Cyc_Core_Impossible_struct _temp934; _temp934.tag= Cyc_Core_Impossible;
_temp934.f1=({ unsigned char* _temp935=( unsigned char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 33; _temp936;}); _temp934;}); _temp933;}));
_LL906:;}}} _LL226: { struct Cyc_NewControlFlow_AbsEnv _temp937= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL938; _LL938: _temp937.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn
_temp939= Cyc_NewControlFlow_abstract_exp( _temp937, _temp399); goto _LL940;
_LL940: { struct Cyc_NewControlFlow_AbsSyn _temp941= Cyc_NewControlFlow_abstract_exp(
env, _temp397); goto _LL942; _LL942: { struct Cyc_NewControlFlow_AbsSynOne
_temp943= Cyc_NewControlFlow_meet_absexp( _temp939); goto _LL944; _LL944: return({
struct Cyc_NewControlFlow_AbsSyn _temp945; _temp945.when_true=({ struct Cyc_NewControlFlow_AbsSynOne
_temp947; _temp947.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp943.inner_exp,( void*)( _temp941.when_true).inner_exp); _temp947.assigns=(
void*)(( void*)( _temp941.when_true).assigns); _temp947;}); _temp945.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp946; _temp946.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp943.inner_exp,( void*)( _temp941.when_false).inner_exp);
_temp946.assigns=( void*)(( void*)( _temp941.when_false).assigns); _temp946;});
_temp945.lvalues= 0; _temp945;});}}}} _LL228: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char* _temp948=(
unsigned char*)"new_control_flow.cyc:703 env.access_path==null"; struct
_tagged_string _temp949; _temp949.curr= _temp948; _temp949.base= _temp948;
_temp949.last_plus_one= _temp948 + 47; _temp949;})); Cyc_NewControlFlow_add_var_root(
env, _temp405);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp950=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp950->hd=( void*) _temp405; _temp950->tl=( env.shared)->comprehension_vars;
_temp950;});{ struct Cyc_NewControlFlow_AbsEnv _temp951= Cyc_NewControlFlow_use_it_env(
env); goto _LL952; _LL952: { void* _temp955; void* _temp957; struct Cyc_NewControlFlow_AbsSynOne
_temp953= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp951, _temp403)); _LL958: _temp957=( void*) _temp953.inner_exp; goto _LL956;
_LL956: _temp955=( void*) _temp953.assigns; goto _LL954; _LL954: { void*
_temp961; void* _temp963; struct Cyc_NewControlFlow_AbsSynOne _temp959= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp951, _temp401)); _LL964: _temp963=( void*)
_temp959.inner_exp; goto _LL962; _LL962: _temp961=( void*) _temp959.assigns;
goto _LL960; _LL960: { void* _temp965= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp957, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp955, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp405), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp963, _temp961), Cyc_CfAbsexp_mkSkipAE()))));
goto _LL966; _LL966: return Cyc_NewControlFlow_mkSyn_tf( _temp965, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}}} _LL230: goto _LL232; _LL232: goto _LL234;
_LL234: goto _LL236; _LL236: goto _LL238; _LL238: goto _LL240; _LL240: goto
_LL242; _LL242:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp967=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp967[ 0]=({ struct Cyc_Core_Impossible_struct _temp968; _temp968.tag= Cyc_Core_Impossible;
_temp968.f1=({ unsigned char* _temp969=( unsigned char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp970; _temp970.curr= _temp969; _temp970.base= _temp969;
_temp970.last_plus_one= _temp969 + 34; _temp970;}); _temp968;}); _temp967;}));
_LL154:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp973; void* _temp975; struct Cyc_NewControlFlow_AbsSynOne
_temp971= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL976: _temp975=( void*) _temp971.inner_exp; goto _LL974; _LL974: _temp973=(
void*) _temp971.assigns; goto _LL972; _LL972: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp975, _temp973);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp977= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
e); goto _LL978; _LL978: return({ struct _tuple0 _temp979; _temp979.f1= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp977.when_true).inner_exp,( void*)(
_temp977.when_true).assigns); _temp979.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*)( _temp977.when_false).inner_exp,( void*)( _temp977.when_false).assigns);
_temp979;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp980= 0; goto _LL981; _LL981: for( 0; scs != 0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp984; struct Cyc_Position_Segment*
_temp985; struct Cyc_Absyn_Stmt* _temp987; struct Cyc_Absyn_Exp* _temp989;
struct Cyc_Core_Opt* _temp991; struct Cyc_Absyn_Pat* _temp993; struct Cyc_Absyn_Switch_clause*
_temp982=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp984=* _temp982; _LL994: _temp993= _temp984.pattern; goto _LL992;
_LL992: _temp991= _temp984.pat_vars; goto _LL990; _LL990: _temp989= _temp984.where_clause;
goto _LL988; _LL988: _temp987= _temp984.body; goto _LL986; _LL986: _temp985=
_temp984.loc; goto _LL983; _LL983: if( _temp991 == 0){( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp995=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp995[ 0]=({ struct
Cyc_Core_Impossible_struct _temp996; _temp996.tag= Cyc_Core_Impossible; _temp996.f1=({
unsigned char* _temp997=( unsigned char*)"switch clause vds not set"; struct
_tagged_string _temp998; _temp998.curr= _temp997; _temp998.base= _temp997;
_temp998.last_plus_one= _temp997 + 26; _temp998;}); _temp996;}); _temp995;}));}{
struct Cyc_List_List* vds=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp991))->v; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd);}} if( _temp989 != 0){ void*
_temp1001; void* _temp1003; struct _tuple0 _temp999= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp989)); _LL1004: _temp1003=
_temp999.f1; goto _LL1002; _LL1002: _temp1001= _temp999.f2; goto _LL1000;
_LL1000: _temp980=({ struct Cyc_List_List* _temp1005=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1005->hd=( void*) _temp1003;
_temp1005->tl=({ struct Cyc_List_List* _temp1006=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1006->hd=( void*) _temp1001;
_temp1006->tl= _temp980; _temp1006;}); _temp1005;});} Cyc_NewControlFlow_cf_prepass(
env, _temp987);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp980);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char* _temp1007=(
unsigned char*)"new_control_flow.cyc:755 env.lhsides == null"; struct
_tagged_string _temp1008; _temp1008.curr= _temp1007; _temp1008.base= _temp1007;
_temp1008.last_plus_one= _temp1007 + 45; _temp1008;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({ unsigned char*
_temp1009=( unsigned char*)"new_control_flow.cyc:756 env.access_path == null";
struct _tagged_string _temp1010; _temp1010.curr= _temp1009; _temp1010.base=
_temp1009; _temp1010.last_plus_one= _temp1009 + 49; _temp1010;}));{ void*
_temp1011=( void*) s->r; struct Cyc_Absyn_Exp* _temp1063; struct Cyc_Absyn_Stmt*
_temp1065; struct Cyc_Absyn_Stmt* _temp1067; struct Cyc_Absyn_Exp* _temp1069;
struct Cyc_Absyn_Exp* _temp1071; struct Cyc_Absyn_Stmt* _temp1073; struct Cyc_Absyn_Stmt*
_temp1075; struct Cyc_Absyn_Exp* _temp1077; struct _tuple3 _temp1079; struct Cyc_Absyn_Stmt*
_temp1081; struct Cyc_Absyn_Exp* _temp1083; struct Cyc_Absyn_Stmt* _temp1085;
struct Cyc_Absyn_Stmt* _temp1087; struct _tuple3 _temp1089; struct Cyc_Absyn_Stmt*
_temp1091; struct Cyc_Absyn_Exp* _temp1093; struct Cyc_Absyn_Stmt* _temp1095;
struct Cyc_Absyn_Stmt* _temp1097; struct Cyc_Absyn_Stmt* _temp1099; struct
_tagged_string* _temp1101; struct Cyc_Absyn_Stmt* _temp1103; struct
_tagged_string* _temp1105; struct Cyc_Absyn_Stmt* _temp1107; struct _tuple3
_temp1109; struct Cyc_Absyn_Stmt* _temp1111; struct Cyc_Absyn_Exp* _temp1113;
struct _tuple3 _temp1115; struct Cyc_Absyn_Stmt* _temp1117; struct Cyc_Absyn_Exp*
_temp1119; struct Cyc_Absyn_Exp* _temp1121; struct Cyc_List_List* _temp1123;
struct Cyc_Absyn_Exp* _temp1125; struct Cyc_Absyn_Switch_clause** _temp1127;
struct Cyc_List_List* _temp1129; struct Cyc_Absyn_Stmt* _temp1131; struct Cyc_Absyn_Decl*
_temp1133; struct Cyc_Absyn_Decl _temp1135; struct Cyc_Position_Segment*
_temp1136; void* _temp1138; struct Cyc_Absyn_Vardecl* _temp1140; struct Cyc_Absyn_Stmt*
_temp1142; struct Cyc_Absyn_Decl* _temp1144; struct Cyc_Absyn_Decl _temp1146;
struct Cyc_Position_Segment* _temp1147; void* _temp1149; int _temp1151; struct
Cyc_Absyn_Exp* _temp1153; struct Cyc_Core_Opt* _temp1155; struct Cyc_Core_Opt*
_temp1157; struct Cyc_Core_Opt _temp1159; struct Cyc_List_List* _temp1160;
struct Cyc_Absyn_Pat* _temp1162; struct Cyc_Absyn_Stmt* _temp1164; struct Cyc_Absyn_Decl*
_temp1166; struct Cyc_Absyn_Decl _temp1168; struct Cyc_Position_Segment*
_temp1169; void* _temp1171; struct Cyc_List_List* _temp1173; struct Cyc_Absyn_Stmt*
_temp1175; struct _tagged_string* _temp1177; struct Cyc_List_List* _temp1179;
struct Cyc_Absyn_Stmt* _temp1181; struct Cyc_Absyn_Stmt* _temp1183; struct Cyc_Absyn_Vardecl*
_temp1185; struct Cyc_Absyn_Tvar* _temp1187; struct Cyc_Absyn_Stmt* _temp1189;
struct Cyc_Absyn_Decl* _temp1191; struct Cyc_List_List* _temp1193; struct Cyc_Absyn_Exp*
_temp1195; struct Cyc_Absyn_Stmt* _temp1197; struct Cyc_Absyn_Stmt* _temp1199;
_LL1013: if( _temp1011 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1014;} else{ goto
_LL1015;} _LL1015: if(( unsigned int) _temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Exp_s:
0){ _LL1064: _temp1063=(( struct Cyc_Absyn_Exp_s_struct*) _temp1011)->f1; goto
_LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Seq_s: 0){ _LL1068: _temp1067=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1011)->f1; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1011)->f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Return_s: 0){ _LL1070:
_temp1069=(( struct Cyc_Absyn_Return_s_struct*) _temp1011)->f1; if( _temp1069 ==
0){ goto _LL1020;} else{ goto _LL1021;}} else{ goto _LL1021;} _LL1021: if((
unsigned int) _temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Return_s: 0){
_LL1072: _temp1071=(( struct Cyc_Absyn_Return_s_struct*) _temp1011)->f1; goto
_LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_IfThenElse_s: 0){ _LL1078: _temp1077=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1011)->f1; goto _LL1076; _LL1076: _temp1075=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1011)->f2; goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1011)->f3; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Do_s: 0){ _LL1086: _temp1085=((
struct Cyc_Absyn_Do_s_struct*) _temp1011)->f1; goto _LL1080; _LL1080: _temp1079=((
struct Cyc_Absyn_Do_s_struct*) _temp1011)->f2; _LL1084: _temp1083= _temp1079.f1;
goto _LL1082; _LL1082: _temp1081= _temp1079.f2; goto _LL1026;} else{ goto
_LL1027;} _LL1027: if(( unsigned int) _temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_While_s:
0){ _LL1090: _temp1089=(( struct Cyc_Absyn_While_s_struct*) _temp1011)->f1;
_LL1094: _temp1093= _temp1089.f1; goto _LL1092; _LL1092: _temp1091= _temp1089.f2;
goto _LL1088; _LL1088: _temp1087=(( struct Cyc_Absyn_While_s_struct*) _temp1011)->f2;
goto _LL1028;} else{ goto _LL1029;} _LL1029: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Break_s: 0){ _LL1096: _temp1095=(( struct Cyc_Absyn_Break_s_struct*)
_temp1011)->f1; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Continue_s: 0){ _LL1098:
_temp1097=(( struct Cyc_Absyn_Continue_s_struct*) _temp1011)->f1; goto _LL1032;}
else{ goto _LL1033;} _LL1033: if(( unsigned int) _temp1011 > 1u?*(( int*)
_temp1011) == Cyc_Absyn_Goto_s: 0){ _LL1102: _temp1101=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1011)->f1; goto _LL1100; _LL1100: _temp1099=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1011)->f2; if( _temp1099 == 0){ goto _LL1034;} else{ goto _LL1035;}} else{
goto _LL1035;} _LL1035: if(( unsigned int) _temp1011 > 1u?*(( int*) _temp1011)
== Cyc_Absyn_Goto_s: 0){ _LL1106: _temp1105=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1011)->f1; goto _LL1104; _LL1104: _temp1103=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1011)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_For_s: 0){ _LL1122: _temp1121=((
struct Cyc_Absyn_For_s_struct*) _temp1011)->f1; goto _LL1116; _LL1116: _temp1115=((
struct Cyc_Absyn_For_s_struct*) _temp1011)->f2; _LL1120: _temp1119= _temp1115.f1;
goto _LL1118; _LL1118: _temp1117= _temp1115.f2; goto _LL1110; _LL1110: _temp1109=((
struct Cyc_Absyn_For_s_struct*) _temp1011)->f3; _LL1114: _temp1113= _temp1109.f1;
goto _LL1112; _LL1112: _temp1111= _temp1109.f2; goto _LL1108; _LL1108: _temp1107=((
struct Cyc_Absyn_For_s_struct*) _temp1011)->f4; goto _LL1038;} else{ goto
_LL1039;} _LL1039: if(( unsigned int) _temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Switch_s:
0){ _LL1126: _temp1125=(( struct Cyc_Absyn_Switch_s_struct*) _temp1011)->f1;
goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_Switch_s_struct*) _temp1011)->f2;
goto _LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Fallthru_s: 0){ _LL1130: _temp1129=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1011)->f1; goto _LL1128; _LL1128: _temp1127=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1011)->f2; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Decl_s: 0){ _LL1134: _temp1133=((
struct Cyc_Absyn_Decl_s_struct*) _temp1011)->f1; _temp1135=* _temp1133; _LL1139:
_temp1138=( void*) _temp1135.r; if(*(( int*) _temp1138) == Cyc_Absyn_Var_d){
_LL1141: _temp1140=(( struct Cyc_Absyn_Var_d_struct*) _temp1138)->f1; goto
_LL1137;} else{ goto _LL1045;} _LL1137: _temp1136= _temp1135.loc; goto _LL1132;
_LL1132: _temp1131=(( struct Cyc_Absyn_Decl_s_struct*) _temp1011)->f2; goto
_LL1044;} else{ goto _LL1045;} _LL1045: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Decl_s: 0){ _LL1145: _temp1144=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f1; _temp1146=* _temp1144; _LL1150: _temp1149=( void*) _temp1146.r;
if(*(( int*) _temp1149) == Cyc_Absyn_Let_d){ _LL1163: _temp1162=(( struct Cyc_Absyn_Let_d_struct*)
_temp1149)->f1; goto _LL1158; _LL1158: _temp1157=(( struct Cyc_Absyn_Let_d_struct*)
_temp1149)->f2; if( _temp1157 == 0){ goto _LL1047;} else{ _temp1159=* _temp1157;
_LL1161: _temp1160=( struct Cyc_List_List*) _temp1159.v; goto _LL1156;} _LL1156:
_temp1155=(( struct Cyc_Absyn_Let_d_struct*) _temp1149)->f3; goto _LL1154;
_LL1154: _temp1153=(( struct Cyc_Absyn_Let_d_struct*) _temp1149)->f4; goto
_LL1152; _LL1152: _temp1151=(( struct Cyc_Absyn_Let_d_struct*) _temp1149)->f5;
goto _LL1148;} else{ goto _LL1047;} _LL1148: _temp1147= _temp1146.loc; goto
_LL1143; _LL1143: _temp1142=(( struct Cyc_Absyn_Decl_s_struct*) _temp1011)->f2;
goto _LL1046;} else{ goto _LL1047;} _LL1047: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Decl_s: 0){ _LL1167: _temp1166=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f1; _temp1168=* _temp1166; _LL1172: _temp1171=( void*) _temp1168.r;
if(*(( int*) _temp1171) == Cyc_Absyn_Letv_d){ _LL1174: _temp1173=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1171)->f1; goto _LL1170;} else{ goto _LL1049;} _LL1170: _temp1169=
_temp1168.loc; goto _LL1165; _LL1165: _temp1164=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f2; goto _LL1048;} else{ goto _LL1049;} _LL1049: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Label_s: 0){ _LL1178: _temp1177=((
struct Cyc_Absyn_Label_s_struct*) _temp1011)->f1; goto _LL1176; _LL1176:
_temp1175=(( struct Cyc_Absyn_Label_s_struct*) _temp1011)->f2; goto _LL1050;}
else{ goto _LL1051;} _LL1051: if(( unsigned int) _temp1011 > 1u?*(( int*)
_temp1011) == Cyc_Absyn_TryCatch_s: 0){ _LL1182: _temp1181=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1011)->f1; goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1011)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Region_s: 0){ _LL1188:
_temp1187=(( struct Cyc_Absyn_Region_s_struct*) _temp1011)->f1; goto _LL1186;
_LL1186: _temp1185=(( struct Cyc_Absyn_Region_s_struct*) _temp1011)->f2; goto
_LL1184; _LL1184: _temp1183=(( struct Cyc_Absyn_Region_s_struct*) _temp1011)->f3;
goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Decl_s: 0){ _LL1192: _temp1191=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f1; goto _LL1190; _LL1190: _temp1189=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1011)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_SwitchC_s: 0){ _LL1196:
_temp1195=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1011)->f1; goto _LL1194;
_LL1194: _temp1193=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1011)->f2; goto
_LL1058;} else{ goto _LL1059;} _LL1059: if(( unsigned int) _temp1011 > 1u?*((
int*) _temp1011) == Cyc_Absyn_Cut_s: 0){ _LL1198: _temp1197=(( struct Cyc_Absyn_Cut_s_struct*)
_temp1011)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(( unsigned int)
_temp1011 > 1u?*(( int*) _temp1011) == Cyc_Absyn_Splice_s: 0){ _LL1200:
_temp1199=(( struct Cyc_Absyn_Splice_s_struct*) _temp1011)->f1; goto _LL1062;}
else{ goto _LL1012;} _LL1014: return; _LL1016: { void* _temp1201= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1063); goto _LL1202; _LL1202: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1203=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1203->hd=( void*) _temp1201; _temp1203->tl= 0;
_temp1203;})); return;} _LL1018: Cyc_NewControlFlow_cf_prepass( env, _temp1067);
Cyc_NewControlFlow_cf_prepass( env, _temp1065); return; _LL1020: return; _LL1022: {
void* _temp1204= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env),( struct Cyc_Absyn_Exp*) _check_null( _temp1071)); goto _LL1205; _LL1205:
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1206=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1206->hd=( void*)
_temp1204; _temp1206->tl= 0; _temp1206;})); return;} _LL1024: { void* _temp1209;
void* _temp1211; struct _tuple0 _temp1207= Cyc_NewControlFlow_abstract_guard(
env, _temp1077); _LL1212: _temp1211= _temp1207.f1; goto _LL1210; _LL1210:
_temp1209= _temp1207.f2; goto _LL1208; _LL1208: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1213=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1213->hd=( void*) _temp1211; _temp1213->tl=({
struct Cyc_List_List* _temp1214=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1214->hd=( void*) _temp1209; _temp1214->tl= 0;
_temp1214;}); _temp1213;})); Cyc_NewControlFlow_cf_prepass( env, _temp1075); Cyc_NewControlFlow_cf_prepass(
env, _temp1073); return;} _LL1026: _temp1093= _temp1083; _temp1091= _temp1081;
_temp1087= _temp1085; goto _LL1028; _LL1028: { void* _temp1217; void* _temp1219;
struct _tuple0 _temp1215= Cyc_NewControlFlow_abstract_guard( env, _temp1093);
_LL1220: _temp1219= _temp1215.f1; goto _LL1218; _LL1218: _temp1217= _temp1215.f2;
goto _LL1216; _LL1216: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1221=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1221->hd=( void*) _temp1219; _temp1221->tl=({ struct Cyc_List_List*
_temp1222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1222->hd=( void*) _temp1217; _temp1222->tl= 0; _temp1222;}); _temp1221;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1087); return;} _LL1030: return;
_LL1032: return; _LL1034:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1223=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1223[ 0]=({ struct Cyc_Core_Impossible_struct _temp1224; _temp1224.tag= Cyc_Core_Impossible;
_temp1224.f1=({ unsigned char* _temp1225=( unsigned char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1226; _temp1226.curr= _temp1225; _temp1226.base=
_temp1225; _temp1226.last_plus_one= _temp1225 + 33; _temp1226;}); _temp1224;});
_temp1223;})); _LL1036: { struct Cyc_Absyn_Stmt* _temp1227=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; goto _LL1228; _LL1228: { struct Cyc_Absyn_Stmt* _temp1229=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1103)))->encloser; goto _LL1230;
_LL1230: while( _temp1229 != _temp1227) { struct Cyc_Absyn_Stmt* _temp1231=( Cyc_NewControlFlow_get_stmt_annot(
_temp1227))->encloser; goto _LL1232; _LL1232: if( _temp1231 == _temp1227){ Cyc_Tcutil_terr(
s->loc,({ unsigned char* _temp1233=( unsigned char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1234; _temp1234.curr= _temp1233; _temp1234.base=
_temp1233; _temp1234.last_plus_one= _temp1233 + 45; _temp1234;})); break;}
_temp1227= _temp1231;} return;}} _LL1038: { void* _temp1235= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1121); goto _LL1236; _LL1236: { void* _temp1239; void* _temp1241;
struct _tuple0 _temp1237= Cyc_NewControlFlow_abstract_guard( env, _temp1119);
_LL1242: _temp1241= _temp1237.f1; goto _LL1240; _LL1240: _temp1239= _temp1237.f2;
goto _LL1238; _LL1238: { void* _temp1243= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1113); goto _LL1244; _LL1244: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1245=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1245->hd=( void*) _temp1235; _temp1245->tl=({
struct Cyc_List_List* _temp1246=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1246->hd=( void*) _temp1241; _temp1246->tl=({
struct Cyc_List_List* _temp1247=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1247->hd=( void*) _temp1239; _temp1247->tl=({
struct Cyc_List_List* _temp1248=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1248->hd=( void*) _temp1243; _temp1248->tl= 0;
_temp1248;}); _temp1247;}); _temp1246;}); _temp1245;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1107); return;}}} _LL1040: { void* _temp1249= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1125); goto _LL1250; _LL1250: { struct
Cyc_List_List* _temp1251= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1123);
goto _LL1252; _LL1252: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1253=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1253->hd=( void*) _temp1249; _temp1253->tl= _temp1251; _temp1253;}));
return;}} _LL1042: { void* _temp1256; void* _temp1258; struct Cyc_NewControlFlow_AbsSynOne
_temp1254= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1129, 0)); _LL1259: _temp1258=( void*) _temp1254.inner_exp; goto
_LL1257; _LL1257: _temp1256=( void*) _temp1254.assigns; goto _LL1255; _LL1255: {
void* _temp1260= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1258, _temp1256); goto _LL1261; _LL1261: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1262=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1262->hd=( void*) _temp1260; _temp1262->tl= 0;
_temp1262;})); return;}} _LL1044: Cyc_NewControlFlow_add_var_root( env,
_temp1140); if( _temp1140->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp1264= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1263=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1263->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1140); _temp1263->tl= 0; _temp1263;})); goto
_LL1265; _LL1265: { void* _temp1266= Cyc_NewControlFlow_abstract_exp_top(
_temp1264,( struct Cyc_Absyn_Exp*) _check_null( _temp1140->initializer)); goto
_LL1267; _LL1267: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1268=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1268->hd=( void*) _temp1266; _temp1268->tl= 0; _temp1268;}));}} Cyc_NewControlFlow_cf_prepass(
env, _temp1131); return; _LL1046: for( 0; _temp1160 != 0; _temp1160=(( struct
Cyc_List_List*) _check_null( _temp1160))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( _temp1160))->hd);}{
void* _temp1269= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp1153); goto _LL1270; _LL1270: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1271=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1271->hd=( void*) _temp1269; _temp1271->tl= 0;
_temp1271;})); Cyc_NewControlFlow_cf_prepass( env, _temp1142); return;} _LL1048:
for( 0; _temp1173 != 0; _temp1173=(( struct Cyc_List_List*) _check_null(
_temp1173))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1173))->hd);} Cyc_NewControlFlow_cf_prepass(
env, _temp1164); return; _LL1050: Cyc_NewControlFlow_cf_prepass( env, _temp1175);
return; _LL1052: Cyc_NewControlFlow_cf_prepass( env, _temp1181);{ struct Cyc_List_List*
_temp1272= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1179); goto _LL1273;
_LL1273: Cyc_NewControlFlow_cf_set_absexps( s, _temp1272); return;} _LL1054: Cyc_NewControlFlow_add_var_root(
env, _temp1185); Cyc_NewControlFlow_cf_prepass( env, _temp1183); return; _LL1056:
goto _LL1058; _LL1058: goto _LL1060; _LL1060: goto _LL1062; _LL1062:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1274=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1274[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1275; _temp1275.tag= Cyc_Core_Impossible;
_temp1275.f1=({ unsigned char* _temp1276=( unsigned char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1277; _temp1277.curr= _temp1276; _temp1277.base=
_temp1276; _temp1277.last_plus_one= _temp1276 + 26; _temp1277;}); _temp1275;});
_temp1274;})); _LL1012:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp1278= Cyc_NewControlFlow_get_stmt_annot(
s); goto _LL1279; _LL1279:( void*)( _temp1278->flow=( void*) Cyc_CfFlowInfo_join_flow(
in_flow,( void*) _temp1278->flow)); ++ _temp1278->visited; _temp1278->visited ==
Cyc_NewControlFlow_iteration_num? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(({
unsigned char* _temp1280=( unsigned char*)"new_control_flow.cyc:894 annot->visited == iteration_num";
struct _tagged_string _temp1281; _temp1281.curr= _temp1280; _temp1281.base=
_temp1280; _temp1281.last_plus_one= _temp1280 + 57; _temp1281;})); return
_temp1278;} static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s,
void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot* _temp1282= Cyc_NewControlFlow_get_stmt_annot(
s); goto _LL1283; _LL1283: { void* _temp1284= Cyc_CfFlowInfo_join_flow( flow,(
void*) _temp1282->flow); goto _LL1285; _LL1285: if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp1284,( void*) _temp1282->flow)){( void*)( _temp1282->flow=( void*)
_temp1284); if( _temp1282->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1286= flow; struct Cyc_Dict_Dict*
_temp1292; _LL1288: if( _temp1286 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1289;} else{ goto _LL1290;} _LL1290: if(( unsigned int) _temp1286 > 1u?*((
int*) _temp1286) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1293: _temp1292=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1286)->f1; goto _LL1291;} else{ goto _LL1287;} _LL1289: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1291: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1296=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1294=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1294[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1295; _temp1295.tag=
Cyc_CfFlowInfo_VarRoot; _temp1295.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1295;}); _temp1294;}); goto _LL1297; _LL1297: {
void* _temp1298= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) _temp1296)); goto _LL1299; _LL1299: _temp1292=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1292,(
void*) _temp1296, _temp1298);}} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1300=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1300[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1301; _temp1301.tag=
Cyc_CfFlowInfo_InitsFL; _temp1301.f1= _temp1292; _temp1301;}); _temp1300;});
_LL1287:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1302= flow; struct Cyc_Dict_Dict*
_temp1308; _LL1304: if( _temp1302 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1305;} else{ goto _LL1306;} _LL1306: if(( unsigned int) _temp1302 > 1u?*((
int*) _temp1302) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1309: _temp1308=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1302)->f1; goto _LL1307;} else{ goto _LL1303;} _LL1305: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1307: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1312=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1310=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1310[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1311; _temp1311.tag=
Cyc_CfFlowInfo_VarRoot; _temp1311.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd; _temp1311;}); _temp1310;}); goto _LL1313; _LL1313:
_temp1308=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( _temp1308,( void*) _temp1312,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) _temp1312));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1314=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1314[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1315; _temp1315.tag= Cyc_CfFlowInfo_InitsFL;
_temp1315.f1= _temp1308; _temp1315;}); _temp1314;}); _LL1303:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)(( struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp1316= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); goto _LL1317; _LL1317: return({ struct _tuple0
_temp1318; _temp1318.f1= _temp1316; _temp1318.f2= _temp1316; _temp1318;});}
return({ struct _tuple0 _temp1319; _temp1319.f1= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null( aes))->hd, in_flow);
_temp1319.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd, in_flow);
_temp1319;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp1322; struct Cyc_Position_Segment*
_temp1323; struct Cyc_Absyn_Stmt* _temp1325; struct Cyc_Absyn_Exp* _temp1327;
struct Cyc_Core_Opt* _temp1329; struct Cyc_Absyn_Pat* _temp1331; struct Cyc_Absyn_Switch_clause*
_temp1320=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd; _temp1322=* _temp1320; _LL1332: _temp1331= _temp1322.pattern;
goto _LL1330; _LL1330: _temp1329= _temp1322.pat_vars; goto _LL1328; _LL1328:
_temp1327= _temp1322.where_clause; goto _LL1326; _LL1326: _temp1325= _temp1322.body;
goto _LL1324; _LL1324: _temp1323= _temp1322.loc; goto _LL1321; _LL1321: { void*
_temp1333= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1329))->v); goto _LL1334; _LL1334:
_temp1333= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1333,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1329))->v);{ void* body_outflow; if(
_temp1327 != 0){ void* _temp1337; void* _temp1339; struct _tuple0 _temp1335= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1327))->loc, aes, _temp1333);
_LL1340: _temp1339= _temp1335.f1; goto _LL1338; _LL1338: _temp1337= _temp1335.f2;
goto _LL1336; _LL1336: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp1337; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1325, _temp1339);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1325, _temp1333);}{ void* _temp1341= body_outflow; _LL1343: if(
_temp1341 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1344;} else{ goto _LL1345;}
_LL1345: goto _LL1346; _LL1344: goto _LL1342; _LL1346: Cyc_Tcutil_terr(
_temp1325->loc,({ unsigned char* _temp1347=( unsigned char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1348; _temp1348.curr= _temp1347; _temp1348.base=
_temp1347; _temp1348.last_plus_one= _temp1347 + 38; _temp1348;})); goto _LL1342;
_LL1342:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1349= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); goto _LL1350; _LL1350:
in_flow=( void*) _temp1349->flow;{ struct Cyc_List_List* _temp1351= _temp1349->absexps;
goto _LL1352; _LL1352: { void* _temp1353=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1399; struct Cyc_Absyn_Exp* _temp1401; struct Cyc_Absyn_Exp* _temp1403;
struct Cyc_Absyn_Stmt* _temp1405; struct Cyc_Absyn_Stmt* _temp1407; struct Cyc_Absyn_Stmt*
_temp1409; struct Cyc_Absyn_Stmt* _temp1411; struct Cyc_Absyn_Exp* _temp1413;
struct Cyc_Absyn_Stmt* _temp1415; struct _tuple3 _temp1417; struct Cyc_Absyn_Stmt*
_temp1419; struct Cyc_Absyn_Exp* _temp1421; struct _tuple3 _temp1423; struct Cyc_Absyn_Stmt*
_temp1425; struct Cyc_Absyn_Exp* _temp1427; struct Cyc_Absyn_Stmt* _temp1429;
struct Cyc_Absyn_Stmt* _temp1431; struct _tuple3 _temp1433; struct Cyc_Absyn_Stmt*
_temp1435; struct Cyc_Absyn_Exp* _temp1437; struct _tuple3 _temp1439; struct Cyc_Absyn_Stmt*
_temp1441; struct Cyc_Absyn_Exp* _temp1443; struct Cyc_Absyn_Exp* _temp1445;
struct Cyc_Absyn_Stmt* _temp1447; struct Cyc_Absyn_Switch_clause** _temp1449;
struct Cyc_Absyn_Switch_clause* _temp1451; struct Cyc_List_List* _temp1452;
struct Cyc_Absyn_Stmt* _temp1454; struct Cyc_Absyn_Stmt* _temp1456; struct Cyc_Absyn_Stmt*
_temp1458; struct _tagged_string* _temp1460; struct Cyc_List_List* _temp1462;
struct Cyc_Absyn_Exp* _temp1464; struct Cyc_List_List* _temp1466; struct Cyc_Absyn_Stmt*
_temp1468; struct Cyc_Absyn_Stmt* _temp1470; struct Cyc_Absyn_Decl* _temp1472;
struct Cyc_Absyn_Decl _temp1474; struct Cyc_Position_Segment* _temp1475; void*
_temp1477; struct Cyc_Absyn_Vardecl* _temp1479; struct Cyc_Absyn_Stmt* _temp1481;
struct Cyc_Absyn_Decl* _temp1483; struct Cyc_Absyn_Decl _temp1485; struct Cyc_Position_Segment*
_temp1486; void* _temp1488; int _temp1490; struct Cyc_Absyn_Exp* _temp1492;
struct Cyc_Core_Opt* _temp1494; struct Cyc_Core_Opt* _temp1496; struct Cyc_Core_Opt
_temp1498; struct Cyc_List_List* _temp1499; struct Cyc_Absyn_Pat* _temp1501;
struct Cyc_Absyn_Stmt* _temp1503; struct Cyc_Absyn_Decl* _temp1505; struct Cyc_Absyn_Decl
_temp1507; struct Cyc_Position_Segment* _temp1508; void* _temp1510; struct Cyc_List_List*
_temp1512; struct Cyc_Absyn_Stmt* _temp1514; struct _tagged_string* _temp1516;
struct Cyc_Absyn_Stmt* _temp1518; struct Cyc_Absyn_Vardecl* _temp1520; struct
Cyc_Absyn_Tvar* _temp1522; _LL1355: if( _temp1353 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1356;} else{ goto _LL1357;} _LL1357: if(( unsigned int) _temp1353 > 1u?*((
int*) _temp1353) == Cyc_Absyn_Return_s: 0){ _LL1400: _temp1399=(( struct Cyc_Absyn_Return_s_struct*)
_temp1353)->f1; if( _temp1399 == 0){ goto _LL1358;} else{ goto _LL1359;}} else{
goto _LL1359;} _LL1359: if(( unsigned int) _temp1353 > 1u?*(( int*) _temp1353)
== Cyc_Absyn_Return_s: 0){ _LL1402: _temp1401=(( struct Cyc_Absyn_Return_s_struct*)
_temp1353)->f1; goto _LL1360;} else{ goto _LL1361;} _LL1361: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Exp_s: 0){ _LL1404: _temp1403=((
struct Cyc_Absyn_Exp_s_struct*) _temp1353)->f1; goto _LL1362;} else{ goto
_LL1363;} _LL1363: if(( unsigned int) _temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Seq_s:
0){ _LL1408: _temp1407=(( struct Cyc_Absyn_Seq_s_struct*) _temp1353)->f1; goto
_LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_Seq_s_struct*) _temp1353)->f2;
goto _LL1364;} else{ goto _LL1365;} _LL1365: if(( unsigned int) _temp1353 > 1u?*((
int*) _temp1353) == Cyc_Absyn_IfThenElse_s: 0){ _LL1414: _temp1413=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1353)->f1; goto _LL1412; _LL1412: _temp1411=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1353)->f2; goto _LL1410; _LL1410: _temp1409=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1353)->f3; goto _LL1366;} else{ goto _LL1367;} _LL1367: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_While_s: 0){ _LL1418: _temp1417=((
struct Cyc_Absyn_While_s_struct*) _temp1353)->f1; _LL1422: _temp1421= _temp1417.f1;
goto _LL1420; _LL1420: _temp1419= _temp1417.f2; goto _LL1416; _LL1416: _temp1415=((
struct Cyc_Absyn_While_s_struct*) _temp1353)->f2; goto _LL1368;} else{ goto
_LL1369;} _LL1369: if(( unsigned int) _temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Do_s:
0){ _LL1430: _temp1429=(( struct Cyc_Absyn_Do_s_struct*) _temp1353)->f1; goto
_LL1424; _LL1424: _temp1423=(( struct Cyc_Absyn_Do_s_struct*) _temp1353)->f2;
_LL1428: _temp1427= _temp1423.f1; goto _LL1426; _LL1426: _temp1425= _temp1423.f2;
goto _LL1370;} else{ goto _LL1371;} _LL1371: if(( unsigned int) _temp1353 > 1u?*((
int*) _temp1353) == Cyc_Absyn_For_s: 0){ _LL1446: _temp1445=(( struct Cyc_Absyn_For_s_struct*)
_temp1353)->f1; goto _LL1440; _LL1440: _temp1439=(( struct Cyc_Absyn_For_s_struct*)
_temp1353)->f2; _LL1444: _temp1443= _temp1439.f1; goto _LL1442; _LL1442:
_temp1441= _temp1439.f2; goto _LL1434; _LL1434: _temp1433=(( struct Cyc_Absyn_For_s_struct*)
_temp1353)->f3; _LL1438: _temp1437= _temp1433.f1; goto _LL1436; _LL1436:
_temp1435= _temp1433.f2; goto _LL1432; _LL1432: _temp1431=(( struct Cyc_Absyn_For_s_struct*)
_temp1353)->f4; goto _LL1372;} else{ goto _LL1373;} _LL1373: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Break_s: 0){ _LL1448: _temp1447=((
struct Cyc_Absyn_Break_s_struct*) _temp1353)->f1; if( _temp1447 == 0){ goto
_LL1374;} else{ goto _LL1375;}} else{ goto _LL1375;} _LL1375: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Fallthru_s: 0){ _LL1453:
_temp1452=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1353)->f1; goto _LL1450;
_LL1450: _temp1449=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1353)->f2; if(
_temp1449 == 0){ goto _LL1377;} else{ _temp1451=* _temp1449; goto _LL1376;}}
else{ goto _LL1377;} _LL1377: if(( unsigned int) _temp1353 > 1u?*(( int*)
_temp1353) == Cyc_Absyn_Break_s: 0){ _LL1455: _temp1454=(( struct Cyc_Absyn_Break_s_struct*)
_temp1353)->f1; goto _LL1378;} else{ goto _LL1379;} _LL1379: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Continue_s: 0){ _LL1457:
_temp1456=(( struct Cyc_Absyn_Continue_s_struct*) _temp1353)->f1; goto _LL1380;}
else{ goto _LL1381;} _LL1381: if(( unsigned int) _temp1353 > 1u?*(( int*)
_temp1353) == Cyc_Absyn_Goto_s: 0){ _LL1461: _temp1460=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1353)->f1; goto _LL1459; _LL1459: _temp1458=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1353)->f2; goto _LL1382;} else{ goto _LL1383;} _LL1383: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Switch_s: 0){ _LL1465:
_temp1464=(( struct Cyc_Absyn_Switch_s_struct*) _temp1353)->f1; goto _LL1463;
_LL1463: _temp1462=(( struct Cyc_Absyn_Switch_s_struct*) _temp1353)->f2; goto
_LL1384;} else{ goto _LL1385;} _LL1385: if(( unsigned int) _temp1353 > 1u?*((
int*) _temp1353) == Cyc_Absyn_TryCatch_s: 0){ _LL1469: _temp1468=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1353)->f1; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1353)->f2; goto _LL1386;} else{ goto _LL1387;} _LL1387: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Decl_s: 0){ _LL1473: _temp1472=((
struct Cyc_Absyn_Decl_s_struct*) _temp1353)->f1; _temp1474=* _temp1472; _LL1478:
_temp1477=( void*) _temp1474.r; if(*(( int*) _temp1477) == Cyc_Absyn_Var_d){
_LL1480: _temp1479=(( struct Cyc_Absyn_Var_d_struct*) _temp1477)->f1; goto
_LL1476;} else{ goto _LL1389;} _LL1476: _temp1475= _temp1474.loc; goto _LL1471;
_LL1471: _temp1470=(( struct Cyc_Absyn_Decl_s_struct*) _temp1353)->f2; goto
_LL1388;} else{ goto _LL1389;} _LL1389: if(( unsigned int) _temp1353 > 1u?*((
int*) _temp1353) == Cyc_Absyn_Decl_s: 0){ _LL1484: _temp1483=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1353)->f1; _temp1485=* _temp1483; _LL1489: _temp1488=( void*) _temp1485.r;
if(*(( int*) _temp1488) == Cyc_Absyn_Let_d){ _LL1502: _temp1501=(( struct Cyc_Absyn_Let_d_struct*)
_temp1488)->f1; goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_Let_d_struct*)
_temp1488)->f2; if( _temp1496 == 0){ goto _LL1391;} else{ _temp1498=* _temp1496;
_LL1500: _temp1499=( struct Cyc_List_List*) _temp1498.v; goto _LL1495;} _LL1495:
_temp1494=(( struct Cyc_Absyn_Let_d_struct*) _temp1488)->f3; goto _LL1493;
_LL1493: _temp1492=(( struct Cyc_Absyn_Let_d_struct*) _temp1488)->f4; goto
_LL1491; _LL1491: _temp1490=(( struct Cyc_Absyn_Let_d_struct*) _temp1488)->f5;
goto _LL1487;} else{ goto _LL1391;} _LL1487: _temp1486= _temp1485.loc; goto
_LL1482; _LL1482: _temp1481=(( struct Cyc_Absyn_Decl_s_struct*) _temp1353)->f2;
goto _LL1390;} else{ goto _LL1391;} _LL1391: if(( unsigned int) _temp1353 > 1u?*((
int*) _temp1353) == Cyc_Absyn_Decl_s: 0){ _LL1506: _temp1505=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1353)->f1; _temp1507=* _temp1505; _LL1511: _temp1510=( void*) _temp1507.r;
if(*(( int*) _temp1510) == Cyc_Absyn_Letv_d){ _LL1513: _temp1512=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1510)->f1; goto _LL1509;} else{ goto _LL1393;} _LL1509: _temp1508=
_temp1507.loc; goto _LL1504; _LL1504: _temp1503=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1353)->f2; goto _LL1392;} else{ goto _LL1393;} _LL1393: if(( unsigned int)
_temp1353 > 1u?*(( int*) _temp1353) == Cyc_Absyn_Label_s: 0){ _LL1517: _temp1516=((
struct Cyc_Absyn_Label_s_struct*) _temp1353)->f1; goto _LL1515; _LL1515:
_temp1514=(( struct Cyc_Absyn_Label_s_struct*) _temp1353)->f2; goto _LL1394;}
else{ goto _LL1395;} _LL1395: if(( unsigned int) _temp1353 > 1u?*(( int*)
_temp1353) == Cyc_Absyn_Region_s: 0){ _LL1523: _temp1522=(( struct Cyc_Absyn_Region_s_struct*)
_temp1353)->f1; goto _LL1521; _LL1521: _temp1520=(( struct Cyc_Absyn_Region_s_struct*)
_temp1353)->f2; goto _LL1519; _LL1519: _temp1518=(( struct Cyc_Absyn_Region_s_struct*)
_temp1353)->f3; goto _LL1396;} else{ goto _LL1397;} _LL1397: goto _LL1398;
_LL1356: return in_flow; _LL1358: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1360: Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*) _check_null(
_temp1401))->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1351))->hd,
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1362: return Cyc_CfAbsexp_eval_absexp(
env, _temp1403->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1351))->hd,
in_flow); _LL1364: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1405,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1407, in_flow)); _LL1366: { void*
_temp1526; void* _temp1528; struct _tuple0 _temp1524= Cyc_NewControlFlow_cf_evalguard(
env, _temp1413->loc, _temp1351, in_flow); _LL1529: _temp1528= _temp1524.f1; goto
_LL1527; _LL1527: _temp1526= _temp1524.f2; goto _LL1525; _LL1525: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1411, _temp1528), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1409, _temp1526));} _LL1368: { void* _temp1530=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1419))->flow; goto _LL1531; _LL1531: { void* _temp1534; void*
_temp1536; struct _tuple0 _temp1532= Cyc_NewControlFlow_cf_evalguard( env,
_temp1421->loc, _temp1351, _temp1530); _LL1537: _temp1536= _temp1532.f1; goto
_LL1535; _LL1535: _temp1534= _temp1532.f2; goto _LL1533; _LL1533: { void*
_temp1538= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1415, _temp1536); goto
_LL1539; _LL1539: Cyc_NewControlFlow_update_flow( _temp1419, _temp1538); return
_temp1534;}}} _LL1370: { void* _temp1540= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1429, in_flow); goto _LL1541; _LL1541: { void* _temp1542=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1540, _temp1425))->flow; goto _LL1543; _LL1543: { void* _temp1546; void*
_temp1548; struct _tuple0 _temp1544= Cyc_NewControlFlow_cf_evalguard( env,
_temp1427->loc, _temp1351, _temp1542); _LL1549: _temp1548= _temp1544.f1; goto
_LL1547; _LL1547: _temp1546= _temp1544.f2; goto _LL1545; _LL1545: Cyc_NewControlFlow_update_flow(
_temp1429, _temp1548); return _temp1546;}}} _LL1372: { void* _temp1550= Cyc_CfAbsexp_eval_absexp(
env, _temp1445->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1351))->hd,
in_flow); goto _LL1551; _LL1551: { void* _temp1552=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1550, _temp1441))->flow; goto _LL1553; _LL1553: { void* _temp1556; void*
_temp1558; struct _tuple0 _temp1554= Cyc_NewControlFlow_cf_evalguard( env,
_temp1443->loc,(( struct Cyc_List_List*) _check_null( _temp1351))->tl, _temp1552);
_LL1559: _temp1558= _temp1554.f1; goto _LL1557; _LL1557: _temp1556= _temp1554.f2;
goto _LL1555; _LL1555: { void* _temp1560= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1431, _temp1558); goto _LL1561; _LL1561: { void* _temp1562=( void*)(
Cyc_NewControlFlow_pre_stmt_check( _temp1560, _temp1435))->flow; goto _LL1563;
_LL1563: { void* _temp1564= Cyc_CfAbsexp_eval_absexp( env, _temp1437->loc,( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1351))->tl))->tl))->tl))->hd, _temp1562); goto _LL1565; _LL1565: Cyc_NewControlFlow_update_flow(
_temp1441, _temp1564); return _temp1556;}}}}}} _LL1374: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1376: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp1351))->hd, in_flow);{ struct Cyc_List_List* _temp1566=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1451->pat_vars))->v;
goto _LL1567; _LL1567: in_flow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow, _temp1566); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env,
in_flow, _temp1566); _temp1454=( struct Cyc_Absyn_Stmt*) _temp1451->body; goto
_LL1378;} _LL1378: _temp1456= _temp1454; goto _LL1380; _LL1380: _temp1458=
_temp1456; goto _LL1382; _LL1382: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)
_check_null( _temp1458), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1384: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1464->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp1351))->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1462,(( struct Cyc_List_List*) _check_null( _temp1351))->tl, in_flow);
return( void*) Cyc_CfFlowInfo_BottomFL; _LL1386: { struct Cyc_NewControlFlow_AnalEnv
_temp1570; void* _temp1571; int _temp1573; struct Cyc_Dict_Dict* _temp1575;
struct Cyc_NewControlFlow_AnalEnv* _temp1568= env; _temp1570=* _temp1568;
_LL1576: _temp1575= _temp1570.roots; goto _LL1574; _LL1574: _temp1573= _temp1570.in_try;
goto _LL1572; _LL1572: _temp1571=( void*) _temp1570.tryflow; goto _LL1569;
_LL1569: env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void*
_temp1577= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1468, in_flow); goto
_LL1578; _LL1578: { void* _temp1579=( void*) env->tryflow; goto _LL1580; _LL1580:
env->in_try= _temp1573;( void*)( env->tryflow=( void*) _temp1571); Cyc_NewControlFlow_update_tryflow(
env, _temp1579); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1466,
_temp1351, _temp1579); return _temp1577;}}} _LL1388: { struct Cyc_List_List
_temp1582=({ struct Cyc_List_List _temp1581; _temp1581.hd=( void*) _temp1479;
_temp1581.tl= 0; _temp1581;}); goto _LL1583; _LL1583: { void* _temp1584= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)& _temp1582); goto _LL1585; _LL1585: {
struct Cyc_Absyn_Exp* _temp1586= _temp1479->initializer; goto _LL1587; _LL1587:
if( _temp1586 != 0){ _temp1584= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1586))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp1351))->hd, _temp1584);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1470, _temp1584);}}} _LL1390: { void* _temp1588= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1499); goto _LL1589; _LL1589: _temp1588= Cyc_CfAbsexp_eval_absexp(
env, _temp1492->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1351))->hd,
_temp1588); _temp1588= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1588,
_temp1499); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1481, _temp1588);}
_LL1392: { void* _temp1590= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1512); goto _LL1591; _LL1591: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1503, _temp1590);} _LL1394: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1514, in_flow); _LL1396: { struct Cyc_List_List* _temp1593=({ struct
Cyc_List_List* _temp1592=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1592->hd=( void*) _temp1520; _temp1592->tl= 0; _temp1592;}); goto _LL1594;
_LL1594: { void* _temp1595= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1593); goto _LL1596; _LL1596: _temp1595= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1595, _temp1593); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1518, _temp1595);}} _LL1398:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1597=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1597[ 0]=({ struct Cyc_Core_Impossible_struct _temp1598; _temp1598.tag= Cyc_Core_Impossible;
_temp1598.f1=({ unsigned char* _temp1599=( unsigned char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1600; _temp1600.curr= _temp1599; _temp1600.base=
_temp1599; _temp1600.last_plus_one= _temp1599 + 62; _temp1600;}); _temp1598;});
_temp1597;})); _LL1354:;}}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1601=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1625; struct Cyc_Absyn_Stmt* _temp1627;
struct Cyc_Absyn_Stmt* _temp1629; struct Cyc_Absyn_Stmt* _temp1631; struct Cyc_Absyn_Exp*
_temp1633; struct Cyc_Absyn_Stmt* _temp1635; struct _tuple3 _temp1637; struct
Cyc_Absyn_Stmt* _temp1639; struct Cyc_Absyn_Exp* _temp1641; struct _tuple3
_temp1643; struct Cyc_Absyn_Stmt* _temp1645; struct Cyc_Absyn_Exp* _temp1647;
struct Cyc_Absyn_Exp* _temp1649; struct Cyc_Absyn_Stmt* _temp1651; struct
_tuple3 _temp1653; struct Cyc_Absyn_Stmt* _temp1655; struct Cyc_Absyn_Exp*
_temp1657; struct _tuple3 _temp1659; struct Cyc_Absyn_Stmt* _temp1661; struct
Cyc_Absyn_Exp* _temp1663; struct Cyc_Absyn_Stmt* _temp1665; struct Cyc_Absyn_Stmt*
_temp1667; struct Cyc_Absyn_Vardecl* _temp1669; struct Cyc_Absyn_Tvar* _temp1671;
struct Cyc_Absyn_Stmt* _temp1673; struct _tagged_string* _temp1675; struct Cyc_Absyn_Stmt*
_temp1677; struct Cyc_Absyn_Decl* _temp1679; struct Cyc_List_List* _temp1681;
struct Cyc_Absyn_Stmt* _temp1683; struct Cyc_List_List* _temp1685; struct Cyc_Absyn_Exp*
_temp1687; _LL1603: if(( unsigned int) _temp1601 > 1u?*(( int*) _temp1601) ==
Cyc_Absyn_Seq_s: 0){ _LL1628: _temp1627=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1601)->f1; goto _LL1626; _LL1626: _temp1625=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1601)->f2; goto _LL1604;} else{ goto _LL1605;} _LL1605: if(( unsigned int)
_temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_IfThenElse_s: 0){ _LL1634:
_temp1633=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1601)->f1; goto _LL1632;
_LL1632: _temp1631=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1601)->f2;
goto _LL1630; _LL1630: _temp1629=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1601)->f3; goto _LL1606;} else{ goto _LL1607;} _LL1607: if(( unsigned int)
_temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_For_s: 0){ _LL1650: _temp1649=((
struct Cyc_Absyn_For_s_struct*) _temp1601)->f1; goto _LL1644; _LL1644: _temp1643=((
struct Cyc_Absyn_For_s_struct*) _temp1601)->f2; _LL1648: _temp1647= _temp1643.f1;
goto _LL1646; _LL1646: _temp1645= _temp1643.f2; goto _LL1638; _LL1638: _temp1637=((
struct Cyc_Absyn_For_s_struct*) _temp1601)->f3; _LL1642: _temp1641= _temp1637.f1;
goto _LL1640; _LL1640: _temp1639= _temp1637.f2; goto _LL1636; _LL1636: _temp1635=((
struct Cyc_Absyn_For_s_struct*) _temp1601)->f4; goto _LL1608;} else{ goto
_LL1609;} _LL1609: if(( unsigned int) _temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_While_s:
0){ _LL1654: _temp1653=(( struct Cyc_Absyn_While_s_struct*) _temp1601)->f1;
_LL1658: _temp1657= _temp1653.f1; goto _LL1656; _LL1656: _temp1655= _temp1653.f2;
goto _LL1652; _LL1652: _temp1651=(( struct Cyc_Absyn_While_s_struct*) _temp1601)->f2;
goto _LL1610;} else{ goto _LL1611;} _LL1611: if(( unsigned int) _temp1601 > 1u?*((
int*) _temp1601) == Cyc_Absyn_Do_s: 0){ _LL1666: _temp1665=(( struct Cyc_Absyn_Do_s_struct*)
_temp1601)->f1; goto _LL1660; _LL1660: _temp1659=(( struct Cyc_Absyn_Do_s_struct*)
_temp1601)->f2; _LL1664: _temp1663= _temp1659.f1; goto _LL1662; _LL1662:
_temp1661= _temp1659.f2; goto _LL1612;} else{ goto _LL1613;} _LL1613: if((
unsigned int) _temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_Region_s: 0){
_LL1672: _temp1671=(( struct Cyc_Absyn_Region_s_struct*) _temp1601)->f1; goto
_LL1670; _LL1670: _temp1669=(( struct Cyc_Absyn_Region_s_struct*) _temp1601)->f2;
goto _LL1668; _LL1668: _temp1667=(( struct Cyc_Absyn_Region_s_struct*) _temp1601)->f3;
goto _LL1614;} else{ goto _LL1615;} _LL1615: if(( unsigned int) _temp1601 > 1u?*((
int*) _temp1601) == Cyc_Absyn_Label_s: 0){ _LL1676: _temp1675=(( struct Cyc_Absyn_Label_s_struct*)
_temp1601)->f1; goto _LL1674; _LL1674: _temp1673=(( struct Cyc_Absyn_Label_s_struct*)
_temp1601)->f2; goto _LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int)
_temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_Decl_s: 0){ _LL1680: _temp1679=((
struct Cyc_Absyn_Decl_s_struct*) _temp1601)->f1; goto _LL1678; _LL1678:
_temp1677=(( struct Cyc_Absyn_Decl_s_struct*) _temp1601)->f2; goto _LL1618;}
else{ goto _LL1619;} _LL1619: if(( unsigned int) _temp1601 > 1u?*(( int*)
_temp1601) == Cyc_Absyn_TryCatch_s: 0){ _LL1684: _temp1683=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1601)->f1; goto _LL1682; _LL1682: _temp1681=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1601)->f2; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int)
_temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_Switch_s: 0){ _LL1688:
_temp1687=(( struct Cyc_Absyn_Switch_s_struct*) _temp1601)->f1; goto _LL1686;
_LL1686: _temp1685=(( struct Cyc_Absyn_Switch_s_struct*) _temp1601)->f2; goto
_LL1622;} else{ goto _LL1623;} _LL1623: goto _LL1624; _LL1604: _temp1631=
_temp1627; _temp1629= _temp1625; goto _LL1606; _LL1606: Cyc_NewControlFlow_cf_postpass(
_temp1631); Cyc_NewControlFlow_cf_postpass( _temp1629); return; _LL1608: Cyc_NewControlFlow_cf_postpass(
_temp1639); _temp1655= _temp1635; _temp1651= _temp1645; goto _LL1610; _LL1610:
_temp1665= _temp1651; _temp1661= _temp1655; goto _LL1612; _LL1612: Cyc_NewControlFlow_cf_postpass(
_temp1661); _temp1667= _temp1665; goto _LL1614; _LL1614: _temp1673= _temp1667;
goto _LL1616; _LL1616: _temp1677= _temp1673; goto _LL1618; _LL1618: Cyc_NewControlFlow_cf_postpass(
_temp1677); return; _LL1620: Cyc_NewControlFlow_cf_postpass( _temp1683);
_temp1685= _temp1681; goto _LL1622; _LL1622: for( 0; _temp1685 != 0; _temp1685=((
struct Cyc_List_List*) _check_null( _temp1685))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1685))->hd)->body);}
return; _LL1624: return; _LL1602:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1717; _temp1717.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1718=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1718->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1718->comprehension_vars= 0; _temp1718;}); _temp1717.lhsides= 0; _temp1717.access_path=
0; _temp1717;}); struct Cyc_List_List* _temp1689=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v; goto _LL1690;
_LL1690:{ struct Cyc_List_List* vds= _temp1689; for( 0; vds != 0; vds=(( struct
Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_List_List*
_temp1693; struct Cyc_List_List* _temp1695; struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1697; struct Cyc_NewControlFlow_SharedAbsEnv _temp1699; struct Cyc_List_List*
_temp1700; struct Cyc_Dict_Dict* _temp1702; struct Cyc_NewControlFlow_AbsEnv
_temp1691= absenv; _LL1698: _temp1697= _temp1691.shared; _temp1699=* _temp1697;
_LL1703: _temp1702= _temp1699.all_roots; goto _LL1701; _LL1701: _temp1700=
_temp1699.comprehension_vars; goto _LL1696; _LL1696: _temp1695= _temp1691.lhsides;
goto _LL1694; _LL1694: _temp1693= _temp1691.access_path; goto _LL1692; _LL1692: {
struct Cyc_NewControlFlow_AnalEnv* _temp1705=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1704=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1704->roots= _temp1702; _temp1704->in_try= 0; _temp1704->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1704;}); goto _LL1706; _LL1706: { void*
in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1715=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1715[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1716; _temp1716.tag= Cyc_CfFlowInfo_InitsFL;
_temp1716.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1716;}); _temp1715;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
_temp1705, in_flow, _temp1689); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
_temp1705, in_flow, _temp1700);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( _temp1705, fd->body, in_flow);}{
void* _temp1707= out_flow; _LL1709: if( _temp1707 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1710;} else{ goto _LL1711;} _LL1711: goto _LL1712; _LL1710: goto _LL1708;
_LL1712: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,({ unsigned char* _temp1713=( unsigned char*)"function may complete without returning a value";
struct _tagged_string _temp1714; _temp1714.curr= _temp1713; _temp1714.base=
_temp1713; _temp1714.last_plus_one= _temp1713 + 48; _temp1714;}));} goto _LL1708;
_LL1708:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp1719=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp1729; struct Cyc_List_List* _temp1731; struct _tuple1* _temp1733; struct
Cyc_List_List* _temp1735; struct _tagged_string* _temp1737; _LL1721: if(*(( int*)
_temp1719) == Cyc_Absyn_Fn_d){ _LL1730: _temp1729=(( struct Cyc_Absyn_Fn_d_struct*)
_temp1719)->f1; goto _LL1722;} else{ goto _LL1723;} _LL1723: if(*(( int*)
_temp1719) == Cyc_Absyn_Using_d){ _LL1734: _temp1733=(( struct Cyc_Absyn_Using_d_struct*)
_temp1719)->f1; goto _LL1732; _LL1732: _temp1731=(( struct Cyc_Absyn_Using_d_struct*)
_temp1719)->f2; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(*(( int*)
_temp1719) == Cyc_Absyn_Namespace_d){ _LL1738: _temp1737=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1719)->f1; goto _LL1736; _LL1736: _temp1735=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1719)->f2; goto _LL1726;} else{ goto _LL1727;} _LL1727: goto _LL1728;
_LL1722: Cyc_NewControlFlow_cf_check_fun( _temp1729); goto _LL1720; _LL1724:
_temp1735= _temp1731; goto _LL1726; _LL1726: Cyc_NewControlFlow_cf_check(
_temp1735); goto _LL1720; _LL1728: goto _LL1720; _LL1720:;}}