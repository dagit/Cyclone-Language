 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_string Cyc_Core_new_string( int); extern int
Cyc_Core_ptrcmp( void**, void**); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_string f1;
} ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern
unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_memq( struct Cyc_List_List*
l, void* x); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u];
extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void Cyc_Dict_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u];
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
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
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct
Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
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
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u];
static const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG=
1; static const int Cyc_CfAbsexp_OneofG= 2; extern void Cyc_CfAbsexp_check_absexp(
void*); extern struct _tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int
depth); extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]="\000\000\000\000BadAbsexp";
void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;} static
const int Cyc_CfAbsexp_BottomAE= 0; static const int Cyc_CfAbsexp_SkipAE= 1;
static const int Cyc_CfAbsexp_UseAE= 0; struct Cyc_CfAbsexp_UseAE_struct{ int
tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_CfAbsexp_UnknownOp=
0; static const int Cyc_CfAbsexp_AddressOp= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; struct Cyc_Absyn_Vardecl* _temp10; void* _temp12;
void* _temp14; int _temp16; struct Cyc_Absyn_Exp* _temp18; _LL2: if((
unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_LocalOp: 0){ _LL11:
_temp10=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp0)->f1; goto _LL3;} else{
goto _LL4;} _LL4: if(( unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_MemberOp:
0){ _LL15: _temp14=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp0)->f1;
goto _LL13; _LL13: _temp12=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp0)->f2; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?*((
int*) _temp0) == Cyc_CfAbsexp_MallocOp: 0){ _LL19: _temp18=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp0)->f1; goto _LL17; _LL17: _temp16=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp0)->f2; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3: return; _LL5:
return; _LL7: return; _LL9:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL1:;}
static void Cyc_CfAbsexp_ok_member_arg( void* ao){ void* _temp20= ao; struct Cyc_Absyn_Vardecl*
_temp32; void* _temp34; void* _temp36; int _temp38; struct Cyc_Absyn_Exp*
_temp40; void* _temp42; _LL22: if(( unsigned int) _temp20 > 1u?*(( int*) _temp20)
== Cyc_CfAbsexp_LocalOp: 0){ _LL33: _temp32=(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp20)->f1; goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp20
> 1u?*(( int*) _temp20) == Cyc_CfAbsexp_MemberOp: 0){ _LL37: _temp36=( void*)((
struct Cyc_CfAbsexp_MemberOp_struct*) _temp20)->f1; goto _LL35; _LL35: _temp34=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp20)->f2; goto _LL25;} else{
goto _LL26;} _LL26: if(( unsigned int) _temp20 > 1u?*(( int*) _temp20) == Cyc_CfAbsexp_MallocOp:
0){ _LL41: _temp40=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp20)->f1; goto
_LL39; _LL39: _temp38=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp20)->f2;
goto _LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp20 > 1u?*(( int*)
_temp20) == Cyc_CfAbsexp_DerefOp: 0){ _LL43: _temp42=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp20)->f1; goto _LL29;} else{ goto _LL30;} _LL30: goto _LL31; _LL23: return;
_LL25: return; _LL27: return; _LL29: return; _LL31:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL21:;} static void Cyc_CfAbsexp_ok_deref_arg( void* ao){ void* _temp44= ao;
struct Cyc_Absyn_Vardecl* _temp54; void* _temp56; void* _temp58; void* _temp60;
_LL46: if(( unsigned int) _temp44 > 1u?*(( int*) _temp44) == Cyc_CfAbsexp_LocalOp:
0){ _LL55: _temp54=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp44)->f1; goto
_LL47;} else{ goto _LL48;} _LL48: if(( unsigned int) _temp44 > 1u?*(( int*)
_temp44) == Cyc_CfAbsexp_MemberOp: 0){ _LL59: _temp58=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp44)->f1; goto _LL57; _LL57: _temp56=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp44)->f2; goto _LL49;} else{ goto _LL50;} _LL50: if(( unsigned int) _temp44
> 1u?*(( int*) _temp44) == Cyc_CfAbsexp_DerefOp: 0){ _LL61: _temp60=( void*)((
struct Cyc_CfAbsexp_DerefOp_struct*) _temp44)->f1; goto _LL51;} else{ goto _LL52;}
_LL52: goto _LL53; _LL47: return; _LL49: return; _LL51: return; _LL53:( void)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL45:;} static void Cyc_CfAbsexp_ok_lvalue(
void* ao){ void* _temp62= ao; void* _temp70; _LL64: if( _temp62 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL65;} else{ goto _LL66;} _LL66: if(( unsigned int) _temp62 > 1u?*(( int*)
_temp62) == Cyc_CfAbsexp_AddressOp: 0){ _LL71: _temp70=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp62)->f1; goto _LL67;} else{ goto _LL68;} _LL68: goto _LL69; _LL65: goto
_LL67; _LL67:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL69: return;
_LL63:;} static int Cyc_CfAbsexp_num_mallocpts= 0; static struct Cyc_Dict_Dict**
Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int( struct Cyc_Absyn_Exp*
e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*))) Cyc_Dict_empty)((
int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp); Cyc_CfAbsexp_mallocpt_dict=({
struct Cyc_Dict_Dict** _temp72=( struct Cyc_Dict_Dict**) GC_malloc( sizeof(
struct Cyc_Dict_Dict*) * 1); _temp72[ 0]= d; _temp72;});}{ int i= 0; if( !(( int(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key, int* ans_place)) Cyc_Dict_lookup_bool)(*((
struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)), e,& i)){*((
struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict))=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(*((
struct Cyc_Dict_Dict**) _check_null( Cyc_CfAbsexp_mallocpt_dict)), e,( i= ++ Cyc_CfAbsexp_num_mallocpts));}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp73=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp73[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp74; _temp74.tag= Cyc_CfAbsexp_AddressOp;
_temp74.f1=( void*) ao; _temp74;}); _temp73;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp75=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp75[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp76; _temp76.tag= Cyc_CfAbsexp_LocalOp;
_temp76.f1= vd; _temp76;}); _temp75;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp77=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp77[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp78; _temp78.tag= Cyc_CfAbsexp_MemberOp;
_temp78.f1=( void*) ao; _temp78.f2=( void*) f; _temp78;}); _temp77;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp79=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp79[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp80; _temp80.tag= Cyc_CfAbsexp_MallocOp;
_temp80.f1= e; _temp80.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp80;}); _temp79;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp81=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp81[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp82; _temp82.tag= Cyc_CfAbsexp_DerefOp; _temp82.f1=(
void*) ao; _temp82;}); _temp81;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp83=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp83[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp84; _temp84.tag= Cyc_CfAbsexp_MallocAE;
_temp84.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp85=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp85[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp86; _temp86.tag= Cyc_CfFlowInfo_MallocPt;
_temp86.f1= e; _temp86;}); _temp85;})); _temp84.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp84;}); _temp83;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp87=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp87[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp88; _temp88.tag= Cyc_CfAbsexp_StmtAE; _temp88.f1=
s; _temp88;}); _temp87;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp89=
ao; _LL91: if( _temp89 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL92;} else{
goto _LL93;} _LL93: goto _LL94; _LL92: return( void*) Cyc_CfAbsexp_SkipAE; _LL94:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp95=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp95[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp96; _temp96.tag= Cyc_CfAbsexp_UseAE; _temp96.f1=( void*) ao; _temp96;});
_temp95;}); _LL90:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp98=({ struct _tuple3 _temp97; _temp97.f1=
l; _temp97.f2= r; _temp97;}); void* _temp106; void* _temp108; void* _temp110;
void* _temp112; _LL100: _LL109: _temp108= _temp98.f1; if( _temp108 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL107;} else{ goto _LL102;} _LL107: _temp106=
_temp98.f2; if( _temp106 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL101;} else{
goto _LL102;} _LL102: _LL113: _temp112= _temp98.f1; if( _temp112 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL111;} else{ goto _LL104;} _LL111: _temp110= _temp98.f2; goto _LL103;
_LL104: goto _LL105; _LL101: return( void*) Cyc_CfAbsexp_SkipAE; _LL103: return
Cyc_CfAbsexp_mkUseAE( r); _LL105: Cyc_CfAbsexp_ok_lvalue( l); return( void*)({
struct Cyc_CfAbsexp_AssignAE_struct* _temp114=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp114[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp115; _temp115.tag= Cyc_CfAbsexp_AssignAE;
_temp115.f1=( void*) l; _temp115.f2=( void*) r; _temp115;}); _temp114;}); _LL99:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp117=({ struct _tuple4
_temp116; _temp116.f1= g; _temp116.f2= ae1; _temp116.f3= ae2; _temp116;}); void*
_temp140; void* _temp142; void* _temp144; void* _temp147; void* _temp149; void*
_temp151; void* _temp153; void* _temp155; void* _temp157; void* _temp159; void*
_temp161; void* _temp163; void* _temp165; void* _temp167; void* _temp169; void*
_temp172; struct Cyc_List_List* _temp174; void* _temp176; void* _temp178; struct
Cyc_List_List* _temp180; void* _temp182; void* _temp184; void* _temp187; void*
_temp189; struct Cyc_List_List* _temp191; void* _temp193; void* _temp195; void*
_temp197; void* _temp199; struct Cyc_List_List* _temp201; void* _temp203; void*
_temp205; void* _temp208; struct Cyc_List_List* _temp210; void* _temp212; void*
_temp214; void* _temp216; _LL119: _LL145: _temp144= _temp117.f1; goto _LL143;
_LL143: _temp142= _temp117.f2; goto _LL141; _LL141: _temp140= _temp117.f3; if(
_temp140 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL139;} else{ goto _LL121;}
_LL139: if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL120;} else{ goto _LL121;}
_LL121: _LL152: _temp151= _temp117.f1; goto _LL150; _LL150: _temp149= _temp117.f2;
if( _temp149 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL148;} else{ goto _LL123;}
_LL148: _temp147= _temp117.f3; goto _LL146; _LL146: if( g !=( void*) Cyc_CfAbsexp_OneofG){
goto _LL122;} else{ goto _LL123;} _LL123: _LL158: _temp157= _temp117.f1; if(
_temp157 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL156;} else{ goto _LL125;}
_LL156: _temp155= _temp117.f2; goto _LL154; _LL154: _temp153= _temp117.f3; if(
_temp153 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL124;} else{ goto _LL125;}
_LL125: _LL164: _temp163= _temp117.f1; if( _temp163 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL162;} else{ goto _LL127;} _LL162: _temp161= _temp117.f2; if( _temp161 ==(
void*) Cyc_CfAbsexp_BottomAE){ goto _LL160;} else{ goto _LL127;} _LL160:
_temp159= _temp117.f3; goto _LL126; _LL127: _LL170: _temp169= _temp117.f1; if(
_temp169 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL168;} else{ goto _LL129;}
_LL168: _temp167= _temp117.f2; if( _temp167 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL166;} else{ goto _LL129;} _LL166: _temp165= _temp117.f3; goto _LL128;
_LL129: _LL185: _temp184= _temp117.f1; goto _LL179; _LL179: _temp178= _temp117.f2;
if(( unsigned int) _temp178 > 2u?*(( int*) _temp178) == Cyc_CfAbsexp_GroupAE: 0){
_LL183: _temp182=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp178)->f1;
goto _LL181; _LL181: _temp180=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp178)->f2;
goto _LL173;} else{ goto _LL131;} _LL173: _temp172= _temp117.f3; if((
unsigned int) _temp172 > 2u?*(( int*) _temp172) == Cyc_CfAbsexp_GroupAE: 0){
_LL177: _temp176=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp172)->f1;
goto _LL175; _LL175: _temp174=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp172)->f2;
goto _LL171;} else{ goto _LL131;} _LL171: if( _temp184 == _temp182? _temp184 ==
_temp176: 0){ goto _LL130;} else{ goto _LL131;} _LL131: _LL196: _temp195=
_temp117.f1; goto _LL190; _LL190: _temp189= _temp117.f2; if(( unsigned int)
_temp189 > 2u?*(( int*) _temp189) == Cyc_CfAbsexp_GroupAE: 0){ _LL194: _temp193=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp189)->f1; goto _LL192; _LL192:
_temp191=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp189)->f2; goto _LL188;}
else{ goto _LL133;} _LL188: _temp187= _temp117.f3; goto _LL186; _LL186: if(
_temp195 == _temp193? _temp195 !=( void*) Cyc_CfAbsexp_OrderedG: 0){ goto _LL132;}
else{ goto _LL133;} _LL133: _LL206: _temp205= _temp117.f1; if( _temp205 ==( void*)
Cyc_CfAbsexp_OrderedG){ goto _LL200;} else{ goto _LL135;} _LL200: _temp199=
_temp117.f2; if(( unsigned int) _temp199 > 2u?*(( int*) _temp199) == Cyc_CfAbsexp_GroupAE:
0){ _LL204: _temp203=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp199)->f1;
if( _temp203 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL202;} else{ goto _LL135;}
_LL202: _temp201=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp199)->f2; goto
_LL198;} else{ goto _LL135;} _LL198: _temp197= _temp117.f3; goto _LL134; _LL135:
_LL217: _temp216= _temp117.f1; goto _LL215; _LL215: _temp214= _temp117.f2; goto
_LL209; _LL209: _temp208= _temp117.f3; if(( unsigned int) _temp208 > 2u?*(( int*)
_temp208) == Cyc_CfAbsexp_GroupAE: 0){ _LL213: _temp212=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp208)->f1; goto _LL211; _LL211: _temp210=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp208)->f2; goto _LL207;} else{ goto _LL137;} _LL207: if( _temp216 ==
_temp212){ goto _LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL120: return
ae1; _LL122: return ae2; _LL124: return ae1; _LL126: return ae2; _LL128: return(
void*) Cyc_CfAbsexp_BottomAE; _LL130: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp218=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp218[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp219; _temp219.tag= Cyc_CfAbsexp_GroupAE;
_temp219.f1=( void*) _temp184; _temp219.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp180, _temp174);
_temp219;}); _temp218;}); _LL132: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp220=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp220[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp221; _temp221.tag= Cyc_CfAbsexp_GroupAE;
_temp221.f1=( void*) _temp195; _temp221.f2=({ struct Cyc_List_List* _temp222=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp222[
0]=({ struct Cyc_List_List _temp223; _temp223.hd=( void*) ae2; _temp223.tl=
_temp191; _temp223;}); _temp222;}); _temp221;}); _temp220;}); _LL134: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp224=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp224[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp225; _temp225.tag= Cyc_CfAbsexp_GroupAE;
_temp225.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp225.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp201,({
struct Cyc_List_List* _temp226=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp226[ 0]=({ struct Cyc_List_List _temp227;
_temp227.hd=( void*) ae2; _temp227.tl= 0; _temp227;}); _temp226;})); _temp225;});
_temp224;}); _LL136: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp228=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp228[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp229; _temp229.tag= Cyc_CfAbsexp_GroupAE;
_temp229.f1=( void*) _temp216; _temp229.f2=({ struct Cyc_List_List* _temp230=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp230[
0]=({ struct Cyc_List_List _temp231; _temp231.hd=( void*) ae1; _temp231.tl=
_temp210; _temp231;}); _temp230;}); _temp229;}); _temp228;}); _LL138: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp232=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp232[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp233; _temp233.tag= Cyc_CfAbsexp_GroupAE;
_temp233.f1=( void*) g; _temp233.f2=({ struct Cyc_List_List* _temp234=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp234[ 0]=({
struct Cyc_List_List _temp235; _temp235.hd=( void*) ae1; _temp235.tl=({ struct
Cyc_List_List* _temp236=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp236[ 0]=({ struct Cyc_List_List _temp237; _temp237.hd=( void*) ae2;
_temp237.tl= 0; _temp237;}); _temp236;}); _temp235;}); _temp234;}); _temp233;});
_temp232;}); _LL118:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if((( struct Cyc_List_List*)
_check_null( ael))->tl == 0){ return( void*)(( struct Cyc_List_List*)
_check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp238=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp238[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp239; _temp239.tag= Cyc_CfAbsexp_GroupAE;
_temp239.f1=( void*) g; _temp239.f2= ael; _temp239;}); _temp238;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_string
Cyc_CfAbsexp_absop2string( void* ao){ void* _temp240= ao; void* _temp254; struct
Cyc_Absyn_Vardecl* _temp256; void* _temp258; void* _temp260; int _temp262;
struct Cyc_Absyn_Exp* _temp264; void* _temp266; _LL242: if( _temp240 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL243;} else{ goto _LL244;} _LL244: if((
unsigned int) _temp240 > 1u?*(( int*) _temp240) == Cyc_CfAbsexp_AddressOp: 0){
_LL255: _temp254=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp240)->f1;
goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int) _temp240 > 1u?*((
int*) _temp240) == Cyc_CfAbsexp_LocalOp: 0){ _LL257: _temp256=(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp240)->f1; goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int)
_temp240 > 1u?*(( int*) _temp240) == Cyc_CfAbsexp_MemberOp: 0){ _LL261: _temp260=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp240)->f1; goto _LL259;
_LL259: _temp258=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp240)->f2;
goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp240 > 1u?*((
int*) _temp240) == Cyc_CfAbsexp_MallocOp: 0){ _LL265: _temp264=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp240)->f1; goto _LL263; _LL263: _temp262=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp240)->f2; goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int)
_temp240 > 1u?*(( int*) _temp240) == Cyc_CfAbsexp_DerefOp: 0){ _LL267: _temp266=(
void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp240)->f1; goto _LL253;} else{
goto _LL241;} _LL243: return({ unsigned char* _temp268=( unsigned char*)"Unknown";
struct _tagged_string _temp269; _temp269.curr= _temp268; _temp269.base= _temp268;
_temp269.last_plus_one= _temp268 + 8; _temp269;}); _LL245: return( struct
_tagged_string)({ struct _tagged_string _temp270= Cyc_CfAbsexp_absop2string(
_temp254); xprintf("& %.*s", _temp270.last_plus_one - _temp270.curr, _temp270.curr);});
_LL247: return*(* _temp256->name).f2; _LL249: { struct _tagged_string fs;{ void*
_temp271= _temp258; struct _tagged_string* _temp277; int _temp279; _LL273: if(*((
int*) _temp271) == Cyc_CfFlowInfo_StructF){ _LL278: _temp277=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp271)->f1; goto _LL274;} else{ goto _LL275;} _LL275: if(*(( int*) _temp271)
== Cyc_CfFlowInfo_TupleF){ _LL280: _temp279=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp271)->f1; goto _LL276;} else{ goto _LL272;} _LL274: fs=* _temp277; goto
_LL272; _LL276: fs=( struct _tagged_string) xprintf("%d", _temp279); goto _LL272;
_LL272:;} return( struct _tagged_string)({ struct _tagged_string _temp281= Cyc_CfAbsexp_absop2string(
_temp260); struct _tagged_string _temp282= fs; xprintf("%.*s.%.*s", _temp281.last_plus_one
- _temp281.curr, _temp281.curr, _temp282.last_plus_one - _temp282.curr, _temp282.curr);});}
_LL251: return( struct _tagged_string) xprintf("<mpt%d>", _temp262); _LL253:
return( struct _tagged_string)({ struct _tagged_string _temp283= Cyc_CfAbsexp_absop2string(
_temp266); xprintf("(*%.*s)", _temp283.last_plus_one - _temp283.curr, _temp283.curr);});
_LL241:;} struct _tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth){
struct _tagged_string ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0; i <({
struct _tagged_string _temp284= ans;( unsigned int)( _temp284.last_plus_one -
_temp284.curr);}); ++ i){*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( ans, sizeof(
unsigned char), i)=' ';}}{ void* _temp285= ae; void* _temp303; int _temp305;
void* _temp307; struct Cyc_Absyn_Exp* _temp309; void* _temp311; void* _temp313;
struct Cyc_List_List* _temp315; void* _temp317; struct Cyc_Absyn_Stmt* _temp319;
_LL287: if( _temp285 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL288;} else{ goto
_LL289;} _LL289: if(( unsigned int) _temp285 > 2u?*(( int*) _temp285) == Cyc_CfAbsexp_UseAE:
0){ _LL304: _temp303=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp285)->f1;
goto _LL290;} else{ goto _LL291;} _LL291: if(( unsigned int) _temp285 > 2u?*((
int*) _temp285) == Cyc_CfAbsexp_MallocAE: 0){ _LL308: _temp307=( void*)(( struct
Cyc_CfAbsexp_MallocAE_struct*) _temp285)->f1; if(*(( int*) _temp307) == Cyc_CfFlowInfo_MallocPt){
_LL310: _temp309=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp307)->f1; goto
_LL306;} else{ goto _LL293;} _LL306: _temp305=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp285)->f2; goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int)
_temp285 > 2u?*(( int*) _temp285) == Cyc_CfAbsexp_AssignAE: 0){ _LL314: _temp313=(
void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp285)->f1; goto _LL312;
_LL312: _temp311=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp285)->f2;
goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int) _temp285 > 2u?*((
int*) _temp285) == Cyc_CfAbsexp_GroupAE: 0){ _LL318: _temp317=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp285)->f1; goto _LL316; _LL316: _temp315=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp285)->f2; goto _LL296;} else{ goto
_LL297;} _LL297: if(( unsigned int) _temp285 > 2u?*(( int*) _temp285) == Cyc_CfAbsexp_StmtAE:
0){ _LL320: _temp319=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp285)->f1; goto
_LL298;} else{ goto _LL299;} _LL299: if( _temp285 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL300;} else{ goto _LL301;} _LL301: goto _LL302; _LL288: ans=({ struct
_tagged_string _temp321=( struct _tagged_string) ans; xprintf("%.*sskip",
_temp321.last_plus_one - _temp321.curr, _temp321.curr);}); goto _LL286; _LL290:
ans=({ struct _tagged_string _temp322=( struct _tagged_string) ans; struct
_tagged_string _temp323= Cyc_CfAbsexp_absop2string( _temp303); xprintf("%.*suse %.*s",
_temp322.last_plus_one - _temp322.curr, _temp322.curr, _temp323.last_plus_one -
_temp323.curr, _temp323.curr);}); goto _LL286; _LL292: ans=({ struct
_tagged_string _temp324=( struct _tagged_string) ans; int _temp325= _temp305;
xprintf("%.*smalloc %i", _temp324.last_plus_one - _temp324.curr, _temp324.curr,
_temp325);}); goto _LL286; _LL294: ans=({ struct _tagged_string _temp326=(
struct _tagged_string) ans; struct _tagged_string _temp327= Cyc_CfAbsexp_absop2string(
_temp313); struct _tagged_string _temp328= Cyc_CfAbsexp_absop2string( _temp311);
xprintf("%.*sassign %.*s %.*s", _temp326.last_plus_one - _temp326.curr, _temp326.curr,
_temp327.last_plus_one - _temp327.curr, _temp327.curr, _temp328.last_plus_one -
_temp328.curr, _temp328.curr);}); goto _LL286; _LL296:{ void* _temp329= _temp317;
_LL331: if( _temp329 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL332;} else{ goto
_LL333;} _LL333: if( _temp329 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL334;}
else{ goto _LL335;} _LL335: if( _temp329 ==( void*) Cyc_CfAbsexp_OneofG){ goto
_LL336;} else{ goto _LL330;} _LL332: ans=({ struct _tagged_string _temp337=(
struct _tagged_string) ans; xprintf("%.*sordered", _temp337.last_plus_one -
_temp337.curr, _temp337.curr);}); goto _LL330; _LL334: ans=({ struct
_tagged_string _temp338=( struct _tagged_string) ans; xprintf("%.*sunordered",
_temp338.last_plus_one - _temp338.curr, _temp338.curr);}); goto _LL330; _LL336:
ans=({ struct _tagged_string _temp339=( struct _tagged_string) ans; xprintf("%.*soneof",
_temp339.last_plus_one - _temp339.curr, _temp339.curr);}); goto _LL330; _LL330:;}
for( 0; _temp315 != 0; _temp315=(( struct Cyc_List_List*) _check_null( _temp315))->tl){
ans=({ struct _tagged_string _temp340=( struct _tagged_string) ans; struct
_tagged_string _temp341= Cyc_CfAbsexp_absexp2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp315))->hd, depth + 1); xprintf("%.*s\n%.*s", _temp340.last_plus_one
- _temp340.curr, _temp340.curr, _temp341.last_plus_one - _temp341.curr, _temp341.curr);});}
goto _LL286; _LL298: ans=({ struct _tagged_string _temp342=( struct
_tagged_string) ans; xprintf("%.*s<<stmt>>", _temp342.last_plus_one - _temp342.curr,
_temp342.curr);}); goto _LL286; _LL300: ans=({ struct _tagged_string _temp343=(
struct _tagged_string) ans; xprintf("%.*sbottom", _temp343.last_plus_one -
_temp343.curr, _temp343.curr);}); goto _LL286; _LL302:( void) _throw(( void*)
Cyc_CfAbsexp_BadAbsexp); _LL286:;} return( struct _tagged_string) ans;} static
void Cyc_CfAbsexp_check_absop( void* ao){ void* _temp344= ao; void* _temp358;
void* _temp360; void* _temp362; void* _temp364; struct Cyc_Absyn_Vardecl*
_temp366; int _temp368; struct Cyc_Absyn_Exp* _temp370; _LL346: if( _temp344 ==(
void*) Cyc_CfAbsexp_UnknownOp){ goto _LL347;} else{ goto _LL348;} _LL348: if((
unsigned int) _temp344 > 1u?*(( int*) _temp344) == Cyc_CfAbsexp_AddressOp: 0){
_LL359: _temp358=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp344)->f1;
goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int) _temp344 > 1u?*((
int*) _temp344) == Cyc_CfAbsexp_DerefOp: 0){ _LL361: _temp360=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp344)->f1; goto _LL351;} else{ goto _LL352;}
_LL352: if(( unsigned int) _temp344 > 1u?*(( int*) _temp344) == Cyc_CfAbsexp_MemberOp:
0){ _LL365: _temp364=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp344)->f1;
goto _LL363; _LL363: _temp362=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp344)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp344 > 1u?*(( int*) _temp344) == Cyc_CfAbsexp_LocalOp: 0){ _LL367: _temp366=((
struct Cyc_CfAbsexp_LocalOp_struct*) _temp344)->f1; goto _LL355;} else{ goto
_LL356;} _LL356: if(( unsigned int) _temp344 > 1u?*(( int*) _temp344) == Cyc_CfAbsexp_MallocOp:
0){ _LL371: _temp370=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp344)->f1;
goto _LL369; _LL369: _temp368=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp344)->f2;
goto _LL357;} else{ goto _LL345;} _LL347: return; _LL349: Cyc_CfAbsexp_check_absop(
_temp358); Cyc_CfAbsexp_ok_address_arg( _temp358); return; _LL351: Cyc_CfAbsexp_check_absop(
_temp360); Cyc_CfAbsexp_ok_deref_arg( _temp360); return; _LL353: Cyc_CfAbsexp_check_absop(
_temp364); Cyc_CfAbsexp_ok_member_arg( _temp364); return; _LL355: return; _LL357:
return; _LL345:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp372= ae;
int _temp390; void* _temp392; struct Cyc_Absyn_Vardecl* _temp394; int _temp396;
void* _temp398; struct Cyc_Absyn_Stmt* _temp400; struct Cyc_List_List* _temp402;
void* _temp404; void* _temp406; void* _temp408; void* _temp410; _LL374: if(
_temp372 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL375;} else{ goto _LL376;}
_LL376: if( _temp372 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL377;} else{ goto
_LL378;} _LL378: if(( unsigned int) _temp372 > 2u?*(( int*) _temp372) == Cyc_CfAbsexp_MallocAE:
0){ _LL393: _temp392=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp372)->f1;
if(*(( int*) _temp392) == Cyc_CfFlowInfo_VarRoot){ _LL395: _temp394=(( struct
Cyc_CfFlowInfo_VarRoot_struct*) _temp392)->f1; goto _LL391;} else{ goto _LL380;}
_LL391: _temp390=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp372)->f2; goto
_LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp372 > 2u?*(( int*)
_temp372) == Cyc_CfAbsexp_MallocAE: 0){ _LL399: _temp398=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp372)->f1; goto _LL397; _LL397: _temp396=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp372)->f2; goto _LL381;} else{ goto _LL382;} _LL382: if(( unsigned int)
_temp372 > 2u?*(( int*) _temp372) == Cyc_CfAbsexp_StmtAE: 0){ _LL401: _temp400=((
struct Cyc_CfAbsexp_StmtAE_struct*) _temp372)->f1; goto _LL383;} else{ goto
_LL384;} _LL384: if(( unsigned int) _temp372 > 2u?*(( int*) _temp372) == Cyc_CfAbsexp_GroupAE:
0){ _LL405: _temp404=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp372)->f1;
goto _LL403; _LL403: _temp402=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp372)->f2;
goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp372 > 2u?*((
int*) _temp372) == Cyc_CfAbsexp_UseAE: 0){ _LL407: _temp406=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp372)->f1; goto _LL387;} else{ goto _LL388;}
_LL388: if(( unsigned int) _temp372 > 2u?*(( int*) _temp372) == Cyc_CfAbsexp_AssignAE:
0){ _LL411: _temp410=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp372)->f1;
goto _LL409; _LL409: _temp408=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp372)->f2; goto _LL389;} else{ goto _LL373;} _LL375: return; _LL377: return;
_LL379:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL381: return; _LL383:
return; _LL385: for( 0; _temp402 != 0; _temp402=(( struct Cyc_List_List*)
_check_null( _temp402))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp402))->hd);} return; _LL387: Cyc_CfAbsexp_check_absop(
_temp406); return; _LL389: Cyc_CfAbsexp_check_absop( _temp410); Cyc_CfAbsexp_check_absop(
_temp408); Cyc_CfAbsexp_ok_lvalue( _temp410); return; _LL373:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp412= ao; struct Cyc_Absyn_Vardecl*
_temp426; int _temp428; struct Cyc_Absyn_Exp* _temp430; void* _temp432; void*
_temp434; void* _temp436; void* _temp438; _LL414: if(( unsigned int) _temp412 >
1u?*(( int*) _temp412) == Cyc_CfAbsexp_LocalOp: 0){ _LL427: _temp426=(( struct
Cyc_CfAbsexp_LocalOp_struct*) _temp412)->f1; goto _LL415;} else{ goto _LL416;}
_LL416: if(( unsigned int) _temp412 > 1u?*(( int*) _temp412) == Cyc_CfAbsexp_MallocOp:
0){ _LL431: _temp430=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp412)->f1;
goto _LL429; _LL429: _temp428=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp412)->f2;
goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int) _temp412 > 1u?*((
int*) _temp412) == Cyc_CfAbsexp_MemberOp: 0){ _LL435: _temp434=( void*)(( struct
Cyc_CfAbsexp_MemberOp_struct*) _temp412)->f1; goto _LL433; _LL433: _temp432=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp412)->f2; goto _LL419;}
else{ goto _LL420;} _LL420: if(( unsigned int) _temp412 > 1u?*(( int*) _temp412)
== Cyc_CfAbsexp_DerefOp: 0){ _LL437: _temp436=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp412)->f1; goto _LL421;} else{ goto _LL422;} _LL422: if( _temp412 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL423;} else{ goto _LL424;} _LL424: if((
unsigned int) _temp412 > 1u?*(( int*) _temp412) == Cyc_CfAbsexp_AddressOp: 0){
_LL439: _temp438=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp412)->f1;
goto _LL425;} else{ goto _LL413;} _LL415: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp440=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp440[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp441; _temp441.tag= Cyc_CfFlowInfo_MustPointTo; _temp441.f1=({ struct Cyc_CfFlowInfo_Place*
_temp442=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp442[ 0]=({ struct Cyc_CfFlowInfo_Place _temp443; _temp443.root=( void*)((
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp444=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp444[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp445; _temp445.tag= Cyc_CfFlowInfo_VarRoot;
_temp445.f1= _temp426; _temp445;}); _temp444;})); _temp443.fields= 0; _temp443;});
_temp442;}); _temp441;}); _temp440;}); _LL417: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp446=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp446[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp447; _temp447.tag= Cyc_CfFlowInfo_MustPointTo; _temp447.f1=({ struct Cyc_CfFlowInfo_Place*
_temp448=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp448[ 0]=({ struct Cyc_CfFlowInfo_Place _temp449; _temp449.root=( void*)((
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp450=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp450[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp451; _temp451.tag= Cyc_CfFlowInfo_MallocPt;
_temp451.f1= _temp430; _temp451;}); _temp450;})); _temp449.fields= 0; _temp449;});
_temp448;}); _temp447;}); _temp446;}); _LL419: { void* _temp452= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp434); goto _LL453; _LL453: { void* _temp454= _temp452; void*
_temp460; void* _temp462; struct Cyc_CfFlowInfo_Place* _temp464; struct Cyc_CfFlowInfo_Place
_temp466; struct Cyc_List_List* _temp467; void* _temp469; _LL456: if(*(( int*)
_temp454) == Cyc_CfFlowInfo_UnknownIS){ _LL463: _temp462=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp454)->f1; goto _LL461; _LL461: _temp460=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp454)->f2; goto _LL457;} else{ goto _LL458;} _LL458: if(*(( int*) _temp454)
== Cyc_CfFlowInfo_MustPointTo){ _LL465: _temp464=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp454)->f1; _temp466=* _temp464; _LL470: _temp469=( void*) _temp466.root;
goto _LL468; _LL468: _temp467= _temp466.fields; goto _LL459;} else{ goto _LL455;}
_LL457: return _temp452; _LL459: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp471=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp471[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp472; _temp472.tag= Cyc_CfFlowInfo_MustPointTo; _temp472.f1=({ struct Cyc_CfFlowInfo_Place*
_temp473=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp473[ 0]=({ struct Cyc_CfFlowInfo_Place _temp474; _temp474.root=( void*)
_temp469; _temp474.fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp467,({ struct Cyc_List_List*
_temp475=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp475[ 0]=({ struct Cyc_List_List _temp476; _temp476.hd=( void*) _temp432;
_temp476.tl= 0; _temp476;}); _temp475;})); _temp474;}); _temp473;}); _temp472;});
_temp471;}); _LL455:;}} _LL421: { void* _temp477= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp436); goto _LL478; _LL478: { void* _temp479= _temp477; void*
_temp487; struct Cyc_Dict_Dict* _temp489; struct Cyc_Dict_Dict* _temp491; _LL481:
if(*(( int*) _temp479) == Cyc_CfFlowInfo_LeafPI){ _LL488: _temp487=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp479)->f1; goto _LL482;} else{ goto
_LL483;} _LL483: if(*(( int*) _temp479) == Cyc_CfFlowInfo_TuplePI){ _LL490:
_temp489=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp479)->f1; goto _LL484;}
else{ goto _LL485;} _LL485: if(*(( int*) _temp479) == Cyc_CfFlowInfo_StructPI){
_LL492: _temp491=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp479)->f1; goto
_LL486;} else{ goto _LL480;} _LL482: return _temp487; _LL484: goto _LL486;
_LL486:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL480:;}} _LL423: goto
_LL425; _LL425:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL413:;} void*
Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void*
_temp493= ao; struct Cyc_Absyn_Vardecl* _temp507; int _temp509; struct Cyc_Absyn_Exp*
_temp511; void* _temp513; void* _temp515; void* _temp517; void* _temp519; _LL495:
if(( unsigned int) _temp493 > 1u?*(( int*) _temp493) == Cyc_CfAbsexp_LocalOp: 0){
_LL508: _temp507=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp493)->f1; goto
_LL496;} else{ goto _LL497;} _LL497: if(( unsigned int) _temp493 > 1u?*(( int*)
_temp493) == Cyc_CfAbsexp_MallocOp: 0){ _LL512: _temp511=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp493)->f1; goto _LL510; _LL510: _temp509=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp493)->f2; goto _LL498;} else{ goto _LL499;} _LL499: if(( unsigned int)
_temp493 > 1u?*(( int*) _temp493) == Cyc_CfAbsexp_MemberOp: 0){ _LL516: _temp515=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp493)->f1; goto _LL514;
_LL514: _temp513=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp493)->f2;
goto _LL500;} else{ goto _LL501;} _LL501: if(( unsigned int) _temp493 > 1u?*((
int*) _temp493) == Cyc_CfAbsexp_DerefOp: 0){ _LL518: _temp517=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp493)->f1; goto _LL502;} else{ goto _LL503;}
_LL503: if( _temp493 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL504;} else{
goto _LL505;} _LL505: if(( unsigned int) _temp493 > 1u?*(( int*) _temp493) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL520: _temp519=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp493)->f1; goto _LL506;} else{ goto _LL494;} _LL496: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp521=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp521[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp522; _temp522.tag= Cyc_CfFlowInfo_VarRoot;
_temp522.f1= _temp507; _temp522;}); _temp521;})); _LL498: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp523=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp523[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp524; _temp524.tag= Cyc_CfFlowInfo_MallocPt;
_temp524.f1= _temp511; _temp524;}); _temp523;})); _LL500: { void* _temp525= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp515); goto _LL526; _LL526: { struct _tuple3 _temp528=({ struct
_tuple3 _temp527; _temp527.f1= _temp525; _temp527.f2= _temp513; _temp527;});
void* _temp538; void* _temp540; void* _temp542; void* _temp544; void* _temp546;
void* _temp548; int _temp550; void* _temp552; struct Cyc_Dict_Dict* _temp554;
void* _temp556; struct _tagged_string* _temp558; void* _temp560; struct Cyc_Dict_Dict*
_temp562; _LL530: _LL541: _temp540= _temp528.f1; if(*(( int*) _temp540) == Cyc_CfFlowInfo_LeafPI){
_LL543: _temp542=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp540)->f1;
if(*(( int*) _temp542) == Cyc_CfFlowInfo_UnknownIS){ _LL547: _temp546=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp542)->f1; goto _LL545; _LL545:
_temp544=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp542)->f2; goto
_LL539;} else{ goto _LL532;}} else{ goto _LL532;} _LL539: _temp538= _temp528.f2;
goto _LL531; _LL532: _LL553: _temp552= _temp528.f1; if(*(( int*) _temp552) ==
Cyc_CfFlowInfo_TuplePI){ _LL555: _temp554=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp552)->f1; goto _LL549;} else{ goto _LL534;} _LL549: _temp548= _temp528.f2;
if(*(( int*) _temp548) == Cyc_CfFlowInfo_TupleF){ _LL551: _temp550=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp548)->f1; goto _LL533;} else{ goto _LL534;} _LL534: _LL561: _temp560=
_temp528.f1; if(*(( int*) _temp560) == Cyc_CfFlowInfo_StructPI){ _LL563:
_temp562=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp560)->f1; goto _LL557;}
else{ goto _LL536;} _LL557: _temp556= _temp528.f2; if(*(( int*) _temp556) == Cyc_CfFlowInfo_StructF){
_LL559: _temp558=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp556)->f1; goto
_LL535;} else{ goto _LL536;} _LL536: goto _LL537; _LL531: return _temp525;
_LL533: return(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp554, _temp550); _LL535: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp562, _temp558); _LL537:( void)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL529:;}} _LL502: { void* _temp564=
Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp517); goto _LL565; _LL565: { void*
_temp566= _temp564; void* _temp576; void* _temp578; void* _temp580; void*
_temp582; void* _temp584; void* _temp586; void* _temp588; struct Cyc_CfFlowInfo_Place*
_temp590; _LL568: if(*(( int*) _temp566) == Cyc_CfFlowInfo_LeafPI){ _LL577:
_temp576=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp566)->f1; if(*((
int*) _temp576) == Cyc_CfFlowInfo_UnknownIS){ _LL581: _temp580=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp576)->f1; goto _LL579; _LL579: _temp578=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp576)->f2; if( _temp578 ==(
void*) Cyc_CfFlowInfo_ThisIL){ goto _LL569;} else{ goto _LL570;}} else{ goto
_LL570;}} else{ goto _LL570;} _LL570: if(*(( int*) _temp566) == Cyc_CfFlowInfo_LeafPI){
_LL583: _temp582=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp566)->f1;
if(*(( int*) _temp582) == Cyc_CfFlowInfo_UnknownIS){ _LL587: _temp586=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp582)->f1; goto _LL585; _LL585:
_temp584=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp582)->f2; goto
_LL571;} else{ goto _LL572;}} else{ goto _LL572;} _LL572: if(*(( int*) _temp566)
== Cyc_CfFlowInfo_LeafPI){ _LL589: _temp588=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp566)->f1; if(*(( int*) _temp588) == Cyc_CfFlowInfo_MustPointTo){ _LL591:
_temp590=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp588)->f1; goto
_LL573;} else{ goto _LL574;}} else{ goto _LL574;} _LL574: goto _LL575; _LL569:
return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_NoneIL);
_LL571: return _temp564; _LL573: return Cyc_CfFlowInfo_lookup_place( pinfo_dict,
_temp590); _LL575:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL567:;}}
_LL504: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL);
_LL506: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp592=( struct
Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp592[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp593; _temp593.tag= Cyc_CfFlowInfo_LeafPI;
_temp593.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp519); _temp593;});
_temp592;}); _LL494:;} void Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp595=({
struct _tuple3 _temp594; _temp594.f1= old_pinfo; _temp594.f2= new_pinfo;
_temp594;}); void* _temp607; void* _temp609; void* _temp611; void* _temp613;
void* _temp615; void* _temp617; void* _temp619; void* _temp621; void* _temp623;
void* _temp625; void* _temp627; struct Cyc_Dict_Dict* _temp629; void* _temp631;
struct Cyc_Dict_Dict* _temp633; void* _temp635; struct Cyc_Dict_Dict* _temp637;
void* _temp639; struct Cyc_Dict_Dict* _temp641; _LL597: _LL612: _temp611=
_temp595.f1; if(*(( int*) _temp611) == Cyc_CfFlowInfo_LeafPI){ _LL614: _temp613=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp611)->f1; if(*(( int*)
_temp613) == Cyc_CfFlowInfo_UnknownIS){ _LL618: _temp617=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp613)->f1; if( _temp617 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL616;} else{
goto _LL599;} _LL616: _temp615=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp613)->f2; goto _LL608;} else{ goto _LL599;}} else{ goto _LL599;} _LL608:
_temp607= _temp595.f2; if(*(( int*) _temp607) == Cyc_CfFlowInfo_LeafPI){ _LL610:
_temp609=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp607)->f1; goto
_LL598;} else{ goto _LL599;} _LL599: _LL624: _temp623= _temp595.f1; if(*(( int*)
_temp623) == Cyc_CfFlowInfo_LeafPI){ _LL626: _temp625=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp623)->f1; goto _LL620;} else{ goto _LL601;} _LL620: _temp619= _temp595.f2;
if(*(( int*) _temp619) == Cyc_CfFlowInfo_LeafPI){ _LL622: _temp621=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp619)->f1; goto _LL600;} else{ goto
_LL601;} _LL601: _LL632: _temp631= _temp595.f1; if(*(( int*) _temp631) == Cyc_CfFlowInfo_TuplePI){
_LL634: _temp633=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp631)->f1; goto
_LL628;} else{ goto _LL603;} _LL628: _temp627= _temp595.f2; if(*(( int*)
_temp627) == Cyc_CfFlowInfo_TuplePI){ _LL630: _temp629=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp627)->f1; goto _LL602;} else{ goto _LL603;} _LL603: _LL640: _temp639=
_temp595.f1; if(*(( int*) _temp639) == Cyc_CfFlowInfo_StructPI){ _LL642:
_temp641=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp639)->f1; goto _LL636;}
else{ goto _LL605;} _LL636: _temp635= _temp595.f2; if(*(( int*) _temp635) == Cyc_CfFlowInfo_StructPI){
_LL638: _temp637=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp635)->f1; goto
_LL604;} else{ goto _LL605;} _LL605: goto _LL606; _LL598:* escaping_states=({
struct Cyc_List_List* _temp643=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp643[ 0]=({ struct Cyc_List_List _temp644;
_temp644.hd=( void*) new_pinfo; _temp644.tl=* escaping_states; _temp644;});
_temp643;}); return; _LL600: return; _LL602:(( void(*)( void(* f)( struct Cyc_List_List**,
void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp633,
_temp629); return; _LL604:(( void(*)( void(* f)( struct Cyc_List_List**, void*,
void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp641,
_temp637); return; _LL606:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp645=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp645[ 0]=({ struct Cyc_Core_Impossible_struct _temp646; _temp646.tag= Cyc_Core_Impossible;
_temp646.f1=({ unsigned char* _temp647=( unsigned char*)"bad pinfos in assign_escape";
struct _tagged_string _temp648; _temp648.curr= _temp647; _temp648.base= _temp647;
_temp648.last_plus_one= _temp647 + 28; _temp648;}); _temp646;}); _temp645;}));
_LL596:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct Cyc_List_List*
ok_mallocs){ void* _temp649= lval; int _temp657; struct Cyc_Absyn_Exp* _temp659;
void* _temp661; void* _temp663; _LL651: if(( unsigned int) _temp649 > 1u?*(( int*)
_temp649) == Cyc_CfAbsexp_MallocOp: 0){ _LL660: _temp659=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp649)->f1; goto _LL658; _LL658: _temp657=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp649)->f2; goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int)
_temp649 > 1u?*(( int*) _temp649) == Cyc_CfAbsexp_MemberOp: 0){ _LL664: _temp663=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp649)->f1; goto _LL662;
_LL662: _temp661=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp649)->f2;
goto _LL654;} else{ goto _LL655;} _LL655: goto _LL656; _LL652: return(( int(*)(
struct Cyc_List_List* l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)( ok_mallocs,
_temp659); _LL654: return Cyc_CfAbsexp_is_ok_malloc_assign( _temp663, ok_mallocs);
_LL656: return 0; _LL650:;} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple5 Cyc_CfAbsexp_unordered_mallocs( struct
_RegionHandle* rgn, void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List*
_temp665= 0; goto _LL666; _LL666: { struct Cyc_List_List* _temp667= 0; goto
_LL668; _LL668:{ void* _temp669= ae; void* _temp691; void* _temp693; void*
_temp695; int _temp697; void* _temp699; struct Cyc_Absyn_Exp* _temp701; int
_temp703; void* _temp705; struct Cyc_Absyn_Stmt* _temp707; struct Cyc_List_List*
_temp709; void* _temp711; struct Cyc_List_List* _temp713; void* _temp715; struct
Cyc_List_List* _temp717; void* _temp719; _LL671: if( _temp669 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL672;} else{ goto _LL673;} _LL673: if( _temp669 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int) _temp669 > 2u?*((
int*) _temp669) == Cyc_CfAbsexp_UseAE: 0){ _LL692: _temp691=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp669)->f1; goto _LL676;} else{ goto _LL677;}
_LL677: if(( unsigned int) _temp669 > 2u?*(( int*) _temp669) == Cyc_CfAbsexp_AssignAE:
0){ _LL696: _temp695=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp669)->f1;
goto _LL694; _LL694: _temp693=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp669)->f2; goto _LL678;} else{ goto _LL679;} _LL679: if(( unsigned int)
_temp669 > 2u?*(( int*) _temp669) == Cyc_CfAbsexp_MallocAE: 0){ _LL700: _temp699=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp669)->f1; if(*(( int*)
_temp699) == Cyc_CfFlowInfo_MallocPt){ _LL702: _temp701=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp699)->f1; goto _LL698;} else{ goto _LL681;} _LL698: _temp697=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp669)->f2; goto _LL680;} else{ goto _LL681;} _LL681: if(( unsigned int)
_temp669 > 2u?*(( int*) _temp669) == Cyc_CfAbsexp_MallocAE: 0){ _LL706: _temp705=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp669)->f1; goto _LL704;
_LL704: _temp703=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp669)->f2; goto
_LL682;} else{ goto _LL683;} _LL683: if(( unsigned int) _temp669 > 2u?*(( int*)
_temp669) == Cyc_CfAbsexp_StmtAE: 0){ _LL708: _temp707=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp669)->f1; goto _LL684;} else{ goto _LL685;} _LL685: if(( unsigned int)
_temp669 > 2u?*(( int*) _temp669) == Cyc_CfAbsexp_GroupAE: 0){ _LL712: _temp711=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp669)->f1; if( _temp711 ==(
void*) Cyc_CfAbsexp_OrderedG){ goto _LL710;} else{ goto _LL687;} _LL710:
_temp709=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp669)->f2; goto _LL686;}
else{ goto _LL687;} _LL687: if(( unsigned int) _temp669 > 2u?*(( int*) _temp669)
== Cyc_CfAbsexp_GroupAE: 0){ _LL716: _temp715=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp669)->f1; if( _temp715 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL714;} else{
goto _LL689;} _LL714: _temp713=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp669)->f2;
goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int) _temp669 > 2u?*((
int*) _temp669) == Cyc_CfAbsexp_GroupAE: 0){ _LL720: _temp719=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp669)->f1; if( _temp719 ==( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL718;} else{ goto _LL670;} _LL718: _temp717=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp669)->f2; goto _LL690;} else{ goto _LL670;} _LL672: goto _LL670; _LL674:
goto _LL670; _LL676: goto _LL670; _LL678: if( Cyc_CfAbsexp_is_ok_malloc_assign(
_temp695, ok_mallocs)){ _temp665=({ struct Cyc_List_List* _temp721=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp721->hd=( void*) ae;
_temp721->tl= _temp665; _temp721;});} goto _LL670; _LL680: _temp667=({ struct
Cyc_List_List* _temp722=( struct Cyc_List_List*) _region_malloc( rgn, sizeof(
struct Cyc_List_List)); _temp722->hd=( void*) _temp701; _temp722->tl= _temp667;
_temp722;}); goto _LL670; _LL682:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp723=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp723[ 0]=({ struct Cyc_Core_Impossible_struct _temp724; _temp724.tag= Cyc_Core_Impossible;
_temp724.f1=({ unsigned char* _temp725=( unsigned char*)"local variable in MallocAE";
struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 27; _temp726;}); _temp724;}); _temp723;}));
_LL684: goto _LL670; _LL686: for( 0; _temp709 != 0; _temp709=(( struct Cyc_List_List*)
_check_null( _temp709))->tl){ struct Cyc_List_List* _temp729; struct Cyc_List_List*
_temp731; struct _tuple5 _temp727= Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)((
struct Cyc_List_List*) _check_null( _temp709))->hd,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp667, ok_mallocs)); _LL732: _temp731= _temp727.f1; goto _LL730; _LL730:
_temp729= _temp727.f2; goto _LL728; _LL728: _temp665=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp731, _temp665); _temp667=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp729, _temp667);} goto _LL670; _LL688: goto _LL670; _LL690: for( 0;
_temp717 != 0; _temp717=(( struct Cyc_List_List*) _check_null( _temp717))->tl){
struct Cyc_List_List* _temp735; struct Cyc_List_List* _temp737; struct _tuple5
_temp733= Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)(( struct Cyc_List_List*)
_check_null( _temp717))->hd, ok_mallocs); _LL738: _temp737= _temp733.f1; goto
_LL736; _LL736: _temp735= _temp733.f2; goto _LL734; _LL734: _temp665=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rappend)( rgn, _temp737, _temp665); _temp667=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp735, _temp667);} goto _LL670; _LL670:;} return({ struct _tuple5
_temp739; _temp739.f1= _temp665; _temp739.f2= _temp667; _temp739;});}} void* Cyc_CfAbsexp_eval_absexp(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
ae, void* in_flow){ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp740= in_flow;
struct Cyc_Dict_Dict* _temp746; _LL742: if( _temp740 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL743;} else{ goto _LL744;} _LL744: if(( unsigned int) _temp740 > 1u?*((
int*) _temp740) == Cyc_CfFlowInfo_InitsFL: 0){ _LL747: _temp746=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp740)->f1; goto _LL745;} else{ goto _LL741;} _LL743: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL745: pinfo_dict= _temp746; goto _LL741; _LL741:;}{ void* _temp748= ae; void*
_temp768; void* _temp770; void* _temp772; int _temp774; void* _temp776; struct
Cyc_Absyn_Stmt* _temp778; struct Cyc_List_List* _temp780; void* _temp782; struct
Cyc_List_List* _temp784; void* _temp786; struct Cyc_List_List* _temp788; void*
_temp790; _LL750: if(( unsigned int) _temp748 > 2u?*(( int*) _temp748) == Cyc_CfAbsexp_AssignAE:
0){ _LL771: _temp770=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp748)->f1;
goto _LL769; _LL769: _temp768=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp748)->f2; goto _LL751;} else{ goto _LL752;} _LL752: if(( unsigned int)
_temp748 > 2u?*(( int*) _temp748) == Cyc_CfAbsexp_UseAE: 0){ _LL773: _temp772=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp748)->f1; goto _LL753;} else{
goto _LL754;} _LL754: if(( unsigned int) _temp748 > 2u?*(( int*) _temp748) ==
Cyc_CfAbsexp_MallocAE: 0){ _LL777: _temp776=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp748)->f1; goto _LL775; _LL775: _temp774=(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp748)->f2; goto _LL755;} else{ goto _LL756;} _LL756: if( _temp748 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL757;} else{ goto _LL758;} _LL758: if( _temp748
==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL759;} else{ goto _LL760;} _LL760: if((
unsigned int) _temp748 > 2u?*(( int*) _temp748) == Cyc_CfAbsexp_StmtAE: 0){
_LL779: _temp778=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp748)->f1; goto
_LL761;} else{ goto _LL762;} _LL762: if(( unsigned int) _temp748 > 2u?*(( int*)
_temp748) == Cyc_CfAbsexp_GroupAE: 0){ _LL783: _temp782=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp748)->f1; if( _temp782 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL781;}
else{ goto _LL764;} _LL781: _temp780=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp748)->f2; goto _LL763;} else{ goto _LL764;} _LL764: if(( unsigned int)
_temp748 > 2u?*(( int*) _temp748) == Cyc_CfAbsexp_GroupAE: 0){ _LL787: _temp786=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp748)->f1; if( _temp786 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL785;} else{ goto _LL766;} _LL785: _temp784=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp748)->f2; goto _LL765;} else{ goto
_LL766;} _LL766: if(( unsigned int) _temp748 > 2u?*(( int*) _temp748) == Cyc_CfAbsexp_GroupAE:
0){ _LL791: _temp790=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp748)->f1;
if( _temp790 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL789;} else{ goto
_LL749;} _LL789: _temp788=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp748)->f2;
goto _LL767;} else{ goto _LL749;} _LL751:{ void* _temp792= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp770); void* _temp798; void* _temp800; struct Cyc_CfFlowInfo_Place*
_temp802; _LL794: if(*(( int*) _temp792) == Cyc_CfFlowInfo_UnknownIS){ _LL801:
_temp800=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp792)->f1; goto
_LL799; _LL799: _temp798=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp792)->f2; goto _LL795;} else{ goto _LL796;} _LL796: if(*(( int*) _temp792)
== Cyc_CfFlowInfo_MustPointTo){ _LL803: _temp802=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp792)->f1; goto _LL797;} else{ goto _LL793;} _LL795: goto _LL793; _LL797: {
void* _temp804= Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp802); goto _LL805;
_LL805: { void* _temp806= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp768); goto
_LL807; _LL807: { struct _tuple3 _temp809=({ struct _tuple3 _temp808; _temp808.f1=
_temp804; _temp808.f2= _temp806; _temp808;}); void* _temp817; void* _temp819;
void* _temp821; void* _temp823; void* _temp825; struct Cyc_Dict_Dict* _temp827;
void* _temp829; void* _temp831; void* _temp833; void* _temp835; void* _temp837;
struct Cyc_Dict_Dict* _temp839; _LL811: _LL826: _temp825= _temp809.f1; if(*((
int*) _temp825) == Cyc_CfFlowInfo_TuplePI){ _LL828: _temp827=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp825)->f1; goto _LL818;} else{ goto _LL813;} _LL818: _temp817= _temp809.f2;
if(*(( int*) _temp817) == Cyc_CfFlowInfo_LeafPI){ _LL820: _temp819=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp817)->f1; if(*(( int*) _temp819) ==
Cyc_CfFlowInfo_UnknownIS){ _LL824: _temp823=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp819)->f1; goto _LL822; _LL822: _temp821=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp819)->f2; goto _LL812;} else{ goto _LL813;}} else{ goto _LL813;} _LL813:
_LL838: _temp837= _temp809.f1; if(*(( int*) _temp837) == Cyc_CfFlowInfo_StructPI){
_LL840: _temp839=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp837)->f1; goto
_LL830;} else{ goto _LL815;} _LL830: _temp829= _temp809.f2; if(*(( int*)
_temp829) == Cyc_CfFlowInfo_LeafPI){ _LL832: _temp831=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp829)->f1; if(*(( int*) _temp831) == Cyc_CfFlowInfo_UnknownIS){ _LL836:
_temp835=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp831)->f1; goto
_LL834; _LL834: _temp833=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp831)->f2; goto _LL814;} else{ goto _LL815;}} else{ goto _LL815;} _LL815:
goto _LL816; _LL812: _temp833= _temp821; goto _LL814; _LL814: _temp806= Cyc_CfFlowInfo_assign_unknown_dict(
_temp833, _temp804); pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp802, _temp806);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp843=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp841=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp841[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp842; _temp842.tag= Cyc_CfFlowInfo_InitsFL;
_temp842.f1= pinfo_dict; _temp842;}); _temp841;}); goto _LL844; _LL844: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp843); return( void*) _temp843;} _LL816: { struct Cyc_List_List*
_temp845= 0; goto _LL846; _LL846: Cyc_CfAbsexp_assign_escape(& _temp845,
_temp804, _temp806); for( 0; _temp845 != 0; _temp845=(( struct Cyc_List_List*)
_check_null( _temp845))->tl){ void* _temp847=( void*)(( struct Cyc_List_List*)
_check_null( _temp845))->hd; goto _LL848; _LL848: if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, _temp847)){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp849=(
unsigned char*)"uninitialized value assigned to ambiguous place"; struct
_tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 48; _temp850;})); return( void*) Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( _temp847, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp802, _temp806);{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp853=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp851=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp851[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp852; _temp852.tag= Cyc_CfFlowInfo_InitsFL;
_temp852.f1= pinfo_dict; _temp852;}); _temp851;}); goto _LL854; _LL854: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp853); return( void*) _temp853;}} _LL810:;}}} _LL793:;}
_temp772= _temp768; goto _LL753; _LL753: { void* _temp855= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp772); goto _LL856; _LL856: if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, _temp855)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp857= Cyc_CfAbsexp_absop2string( _temp772); xprintf("%.*s must be fully initialized to be used here",
_temp857.last_plus_one - _temp857.curr, _temp857.curr);})); return( void*) Cyc_CfFlowInfo_BottomFL;}{
struct Cyc_Dict_Dict* _temp858= Cyc_CfFlowInfo_escape_pointsto( _temp855,
pinfo_dict); goto _LL859; _LL859: if( _temp858 == pinfo_dict){ return in_flow;}{
struct Cyc_CfFlowInfo_InitsFL_struct* _temp862=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp860=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp860[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp861; _temp861.tag= Cyc_CfFlowInfo_InitsFL;
_temp861.f1= pinfo_dict; _temp861;}); _temp860;}); goto _LL863; _LL863: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp862); return( void*) _temp862;}}} _LL755: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp864=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp864[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp865; _temp865.tag= Cyc_CfFlowInfo_InitsFL;
_temp865.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( pinfo_dict, _temp776,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots, _temp776)); _temp865;}); _temp864;});
_LL757: return( void*) Cyc_CfFlowInfo_BottomFL; _LL759: return in_flow; _LL761:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp778, in_flow); _LL763: for(
0; _temp780 != 0; _temp780=(( struct Cyc_List_List*) _check_null( _temp780))->tl){
in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp780))->hd, in_flow);} return in_flow; _LL765: { void* out_flow=(
void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp784 != 0; _temp784=(( struct Cyc_List_List*)
_check_null( _temp784))->tl){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp784))->hd, in_flow));}
return out_flow;} _LL767: { void* out_flow= in_flow; do { in_flow= out_flow;
out_flow= in_flow;{ struct Cyc_List_List* ael2= _temp788; for( 0; ael2 != 0;
ael2=(( struct Cyc_List_List*) _check_null( ael2))->tl){ out_flow= Cyc_CfFlowInfo_join_flow(
out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( ael2))->hd, out_flow));}}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx(
out_flow, in_flow));{ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp866= in_flow;
struct Cyc_Dict_Dict* _temp872; _LL868: if( _temp866 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL869;} else{ goto _LL870;} _LL870: if(( unsigned int) _temp866 > 1u?*((
int*) _temp866) == Cyc_CfFlowInfo_InitsFL: 0){ _LL873: _temp872=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp866)->f1; goto _LL871;} else{ goto _LL867;} _LL869: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL871: pinfo_dict= _temp872; goto _LL867; _LL867:;}{ struct _RegionHandle
_temp874= _new_region(); struct _RegionHandle* spine_rgn=& _temp874;
_push_region( spine_rgn);{ struct Cyc_List_List* _temp877; struct Cyc_List_List*
_temp879; struct _tuple5 _temp875= Cyc_CfAbsexp_unordered_mallocs( spine_rgn, ae,
0); _LL880: _temp879= _temp875.f1; goto _LL878; _LL878: _temp877= _temp875.f2;
goto _LL876; _LL876: for( 0; _temp877 != 0; _temp877=(( struct Cyc_List_List*)
_check_null( _temp877))->tl){ struct Cyc_CfFlowInfo_MallocPt_struct* _temp883=({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp881=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp881[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp882; _temp882.tag= Cyc_CfFlowInfo_MallocPt;
_temp882.f1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp877))->hd; _temp882;}); _temp881;}); goto _LL884; _LL884: pinfo_dict=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(
pinfo_dict,( void*) _temp883,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) _temp883));} out_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp885=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp885[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp886; _temp886.tag= Cyc_CfFlowInfo_InitsFL;
_temp886.f1= pinfo_dict; _temp886;}); _temp885;}); for( 0; _temp879 != 0;
_temp879=(( struct Cyc_List_List*) _check_null( _temp879))->tl){ out_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp879))->hd, out_flow);}};
_pop_region(& _temp874);} return out_flow;}} _LL749:;}}