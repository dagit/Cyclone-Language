 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern void* Cyc_Core_identity( void*); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
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
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern struct
_tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud;
extern struct _tuple0* Cyc_Absyn_null_pointer_exn_name; extern struct _tuple0*
Cyc_Absyn_match_exn_name; extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_null_pointer_exn_tuf;
extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf; extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_List_List* rgn_po, struct
Cyc_List_List*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern void* Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes=
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
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init();
struct Cyc_Interface_I; extern struct Cyc_Interface_I* Cyc_Interface_empty();
extern struct Cyc_Interface_I* Cyc_Interface_final(); extern struct Cyc_Interface_I*
Cyc_Interface_extract( struct Cyc_Dict_Dict* ae); struct _tuple3{ struct
_tagged_string f1; struct _tagged_string f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info);
extern struct Cyc_Interface_I* Cyc_Interface_merge( struct Cyc_Interface_I* i1,
struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge_list( struct Cyc_List_List* li, struct Cyc_List_List* linfo);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
void Cyc_Interface_print( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*);
extern void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); extern void Cyc_Tc_tc(
struct Cyc_Tcenv_Tenv* te, int var_default_init, struct Cyc_List_List* ds);
extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_string, int, void*); void* read_fun_state; } ; struct
_tagged_ptr1{ int* curr; int* base; int* last_plus_one; } ; struct Cyc_Lexing_lex_tables{
struct _tagged_ptr1 lex_base; struct _tagged_ptr1 lex_backtrk; struct
_tagged_ptr1 lex_default; struct _tagged_ptr1 lex_trans; struct _tagged_ptr1
lex_check; } ; extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f); struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Bool_tok[ 13u];
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; extern unsigned char
Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclSpec_tok[
17u]; struct Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Declaration_spec*
f1; } ; extern unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple4* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple5{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple5* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple6{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple6* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple7{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple7* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple8{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple8* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple9{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple9* f1; } ; extern unsigned char Cyc_Rgn_tok[ 12u];
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Rgnorder_tok[ 17u]; struct Cyc_Rgnorder_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Scope_tok[
14u]; struct Cyc_Scope_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ unsigned char*
tag; short f1; } ; extern unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[
21u]; struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
extern unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Stringopt_tok[
18u]; struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt*
f1; } ; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; extern int Cyc_String_strptrcmp( struct
_tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern unsigned char Cyc_Tcutil_TypeErr[
12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_function_type(
void* t); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl* base; struct Cyc_Absyn_Tunionfield* field; } ;
extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment* loc, struct
_tagged_string* msg0, struct _tagged_string msg1); extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports;
struct Cyc_Dict_Dict* exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});} static
struct Cyc_Interface_Ienv* Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv*
ie= 0; if( ie == 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();}
return( struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)((
struct Cyc_Interface_Ienv*(*)( void* ignored)) Cyc_Interface_skel2i_f, skel);
_temp1->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(* f)(
void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct Cyc_Interface_Ienv*(*)(
void* ignored)) Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I*
Cyc_Interface_empty(){ return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
_temp3= Cyc_Interface_empty(); goto _LL4; _LL4: { struct Cyc_Interface_Ienv*
_temp5= Cyc_Interface_new_ienv(); goto _LL6; _LL6: { struct Cyc_Absyn_Tuniondecl*
_temp8=({ struct Cyc_Absyn_Tuniondecl* _temp7=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp7[ 0]=* Cyc_Absyn_exn_tud;
_temp7;}); goto _LL9; _LL9:( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));
_temp5->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp5->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp8);{ struct Cyc_Absyn_Tunionfield*
_temp11=({ struct Cyc_Absyn_Tunionfield* _temp10=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp10[ 0]=* Cyc_Absyn_null_pointer_exn_tuf;
_temp10;}); goto _LL12; _LL12:( void*)( _temp11->sc=( void*)(( void*) Cyc_Absyn_Public));{
struct Cyc_Tcdecl_Xtunionfielddecl* _temp14=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp13=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp13->base= _temp8; _temp13->field= _temp11; _temp13;}); goto _LL15; _LL15:
_temp5->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)(
_temp5->xtunionfielddecls,(* Cyc_Absyn_null_pointer_exn_name).f2, _temp14);{
struct Cyc_Absyn_Tunionfield* _temp17=({ struct Cyc_Absyn_Tunionfield* _temp16=(
struct Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp16[ 0]=* Cyc_Absyn_match_exn_tuf; _temp16;}); goto _LL18; _LL18:( void*)(
_temp17->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp20=({ struct Cyc_Tcdecl_Xtunionfielddecl* _temp19=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp19->base= _temp8;
_temp19->field= _temp17; _temp19;}); goto _LL21; _LL21: _temp5->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp5->xtunionfielddecls,(*
Cyc_Absyn_match_exn_name).f2, _temp20); _temp3->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp5);{ struct Cyc_Interface_Ienv*
_temp22= Cyc_Interface_new_ienv(); goto _LL23; _LL23: { struct _tuple0* qmain=({
struct _tuple0* _temp30=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp30->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp34=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp34[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp35; _temp35.tag= Cyc_Absyn_Abs_n; _temp35.f1= 0; _temp35;}); _temp34;});
_temp30->f2=({ struct _tagged_string* _temp31=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp31[ 0]=({ unsigned char*
_temp32=( unsigned char*)"main"; struct _tagged_string _temp33; _temp33.curr=
_temp32; _temp33.base= _temp32; _temp33.last_plus_one= _temp32 + 5; _temp33;});
_temp31;}); _temp30;}); struct Cyc_Absyn_Vardecl* _temp28= Cyc_Absyn_new_vardecl(
qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({ struct Cyc_List_List*
_temp24=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp24->hd=( void*)({ struct _tuple1* _temp27=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp27->f1= 0; _temp27->f2= Cyc_Absyn_empty_tqual();
_temp27->f3= Cyc_Absyn_uint_t; _temp27;}); _temp24->tl=({ struct Cyc_List_List*
_temp25=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp25->hd=( void*)({ struct _tuple1* _temp26=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp26->f1= 0; _temp26->f2= Cyc_Absyn_empty_tqual();
_temp26->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp26;}); _temp25->tl= 0; _temp25;}); _temp24;}), 0,
0, 0, 0), 0); goto _LL29; _LL29:( void*)( _temp28->sc=( void*)(( void*) Cyc_Absyn_Extern));
_temp22->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp22->vardecls,(*
qmain).f2, _temp28); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp22); return _temp3;}}}}}}}}} static void Cyc_Interface_err(
struct _tagged_string msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_string sc, struct _tagged_string t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_string)({ struct _tagged_string _temp36= sc; struct
_tagged_string _temp37= t; struct _tagged_string _temp38= Cyc_Absynpp_qvar2string(
n); xprintf("fields of %.*s %.*s %.*s have never been defined", _temp36.last_plus_one
- _temp36.curr, _temp36.curr, _temp37.last_plus_one - _temp37.curr, _temp37.curr,
_temp38.last_plus_one - _temp38.curr, _temp38.curr);}));} static void Cyc_Interface_body_err(
struct _tagged_string sc, struct _tuple0* n){ Cyc_Interface_err(( struct
_tagged_string)({ struct _tagged_string _temp39= sc; struct _tagged_string
_temp40= Cyc_Absynpp_qvar2string( n); xprintf("the body of %.*s function %.*s has never been defined",
_temp39.last_plus_one - _temp39.curr, _temp39.curr, _temp40.last_plus_one -
_temp40.curr, _temp40.curr);}));} static void Cyc_Interface_static_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ if( obj1.curr !=(( struct _tagged_string)({ struct
_tagged_string _temp41={ 0, 0, 0}; _temp41;})).curr){ Cyc_Interface_err(( struct
_tagged_string)({ struct _tagged_string _temp42= obj1; struct _tagged_string
_temp43= Cyc_Absynpp_qvar2string( name1); struct _tagged_string _temp44= obj2;
struct _tagged_string _temp45= Cyc_Absynpp_qvar2string( name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s",
_temp42.last_plus_one - _temp42.curr, _temp42.curr, _temp43.last_plus_one -
_temp43.curr, _temp43.curr, _temp44.last_plus_one - _temp44.curr, _temp44.curr,
_temp45.last_plus_one - _temp45.curr, _temp45.curr);}));}} static void Cyc_Interface_abstract_err(
struct _tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2,
struct _tuple0* name2){ if( obj1.curr !=(( struct _tagged_string)({ struct
_tagged_string _temp46={ 0, 0, 0}; _temp46;})).curr){ Cyc_Interface_err(( struct
_tagged_string)({ struct _tagged_string _temp47= obj1; struct _tagged_string
_temp48= Cyc_Absynpp_qvar2string( name1); struct _tagged_string _temp49= obj2;
struct _tagged_string _temp50= Cyc_Absynpp_qvar2string( name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp47.last_plus_one - _temp47.curr, _temp47.curr, _temp48.last_plus_one -
_temp48.curr, _temp48.curr, _temp49.last_plus_one - _temp49.curr, _temp49.curr,
_temp50.last_plus_one - _temp50.curr, _temp50.curr);}));}} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; static struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({
struct Cyc_Interface_Seen* _temp51=( struct Cyc_Interface_Seen*) GC_malloc(
sizeof( struct Cyc_Interface_Seen)); _temp51->structs=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp51->unions=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp51->tunions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp51;});} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp52= 1; goto _LL53; _LL53: for( 0; l != 0; l=(( struct Cyc_List_List*)
_check_null( l))->tl){ if( ! Cyc_Interface_check_public_type( ae, seen, obj,
name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){ _temp52= 0;}}
return _temp52;} static int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp54; _push_handler(& _temp54);{ int _temp56= 0; if( setjmp(
_temp54.handler)){ _temp56= 1;} if( ! _temp56){{ int _temp57=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp57;}; _pop_handler();} else{ void* _temp55=( void*) _exn_thrown;
void* _temp59= _temp55; _LL61: if( _temp59 == Cyc_Dict_Absent){ goto _LL62;}
else{ goto _LL63;} _LL63: goto _LL64; _LL62: { int _temp65= 1; goto _LL66; _LL66:
seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp65); if( d->fields != 0){
struct Cyc_List_List* _temp67=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL68; _LL68: for( 0; _temp67 != 0; _temp67=((
struct Cyc_List_List*) _check_null( _temp67))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp69=( unsigned char*)"struct"; struct
_tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69; _temp70.last_plus_one=
_temp69 + 7; _temp70;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp67))->hd)->type)){ _temp65= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp65);
return _temp65;} _LL64:( void) _throw( _temp59); _LL60:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp71; _push_handler(& _temp71);{ int _temp73= 0; if(
setjmp( _temp71.handler)){ _temp73= 1;} if( ! _temp73){{ int _temp74=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp74;}; _pop_handler();} else{ void* _temp72=(
void*) _exn_thrown; void* _temp76= _temp72; _LL78: if( _temp76 == Cyc_Dict_Absent){
goto _LL79;} else{ goto _LL80;} _LL80: goto _LL81; _LL79: { int _temp82= 1; goto
_LL83; _LL83: seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp82); if( d->fields != 0){
struct Cyc_List_List* _temp84=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL85; _LL85: for( 0; _temp84 != 0; _temp84=((
struct Cyc_List_List*) _check_null( _temp84))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp86=( unsigned char*)"union"; struct
_tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86; _temp87.last_plus_one=
_temp86 + 6; _temp87;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp84))->hd)->type)){ _temp82= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp82);
return _temp82;} _LL81:( void) _throw( _temp76); _LL77:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp88; _push_handler(& _temp88);{ int _temp90= 0; if(
setjmp( _temp88.handler)){ _temp90= 1;} if( ! _temp90){{ int _temp91=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp91;}; _pop_handler();} else{ void*
_temp89=( void*) _exn_thrown; void* _temp93= _temp89; _LL95: if( _temp93 == Cyc_Dict_Absent){
goto _LL96;} else{ goto _LL97;} _LL97: goto _LL98; _LL96: { int _temp99= 1; goto
_LL100; _LL100: seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp99); if( d->fields != 0){ struct Cyc_List_List* _temp101=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; goto _LL102; _LL102: for( 0;
_temp101 != 0; _temp101=(( struct Cyc_List_List*) _check_null( _temp101))->tl){
if( !(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen,({
unsigned char* _temp103=( unsigned char*)"[x]tunion"; struct _tagged_string
_temp104; _temp104.curr= _temp103; _temp104.base= _temp103; _temp104.last_plus_one=
_temp103 + 10; _temp104;}), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp101))->hd)->typs)){ _temp99= 0;}}} seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, _temp99); return _temp99;} _LL98:(
void) _throw( _temp93); _LL94:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl*
d){ return 1;} static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl* d){ return
Cyc_Interface_check_public_type( ae, seen,( struct _tagged_string)({ struct
_tagged_string _temp105={ 0, 0, 0}; _temp105;}), d->name,( void*) d->defn);}
static int Cyc_Interface_check_public_vardecl( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl* d){ return Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp106=( unsigned char*)"variable"; struct
_tagged_string _temp107; _temp107.curr= _temp106; _temp107.base= _temp106;
_temp107.last_plus_one= _temp106 + 9; _temp107;}), d->name,( void*) d->type);}
static void* Cyc_Interface_get_type2( struct _tuple1* x){ return(* x).f3;}
static struct Cyc_List_List* Cyc_Interface_get_abs_ns( void* ns){ void* _temp108=
ns; struct Cyc_List_List* _temp114; _LL110: if(( unsigned int) _temp108 > 1u?*((
int*) _temp108) == Cyc_Absyn_Abs_n: 0){ _LL115: _temp114=(( struct Cyc_Absyn_Abs_n_struct*)
_temp108)->f1; goto _LL111;} else{ goto _LL112;} _LL112: goto _LL113; _LL111:
return _temp114; _LL113:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp116=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp116[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp117; _temp117.tag= Cyc_Core_InvalidArg;
_temp117.f1=({ unsigned char* _temp118=( unsigned char*)"get_abs_ns"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 11; _temp119;}); _temp117;}); _temp116;}));
_LL109:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void* t){ void* _temp120= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp144; struct Cyc_Absyn_Conref* _temp146; struct Cyc_Absyn_Tqual _temp148;
struct Cyc_Absyn_Conref* _temp150; void* _temp152; void* _temp154; struct Cyc_Absyn_Exp*
_temp156; struct Cyc_Absyn_Tqual _temp158; void* _temp160; struct Cyc_Core_Opt*
_temp162; struct Cyc_Core_Opt _temp164; void* _temp165; struct Cyc_List_List*
_temp167; struct _tuple0* _temp169; struct Cyc_Absyn_FnInfo _temp171; struct Cyc_List_List*
_temp173; struct Cyc_List_List* _temp175; struct Cyc_Absyn_VarargInfo* _temp177;
int _temp179; struct Cyc_List_List* _temp181; void* _temp183; struct Cyc_Core_Opt*
_temp185; struct Cyc_List_List* _temp187; struct Cyc_List_List* _temp189; struct
Cyc_Absyn_Structdecl** _temp191; struct Cyc_Absyn_Structdecl* _temp193; struct
Cyc_List_List* _temp194; struct _tuple0* _temp196; struct Cyc_Absyn_Uniondecl**
_temp198; struct Cyc_Absyn_Uniondecl* _temp200; struct Cyc_List_List* _temp201;
struct _tuple0* _temp203; struct Cyc_Absyn_Enumdecl* _temp205; struct _tuple0*
_temp207; struct Cyc_Absyn_TunionInfo _temp209; void* _temp211; struct Cyc_List_List*
_temp213; void* _temp215; struct Cyc_Absyn_Tuniondecl* _temp217; struct Cyc_Absyn_TunionFieldInfo
_temp219; struct Cyc_List_List* _temp221; void* _temp223; struct Cyc_Absyn_Tunionfield*
_temp225; struct Cyc_Absyn_Tuniondecl* _temp227; _LL122: if(( unsigned int)
_temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_PointerType: 0){ _LL145: _temp144=((
struct Cyc_Absyn_PointerType_struct*) _temp120)->f1; _LL155: _temp154=( void*)
_temp144.elt_typ; goto _LL153; _LL153: _temp152=( void*) _temp144.rgn_typ; goto
_LL151; _LL151: _temp150= _temp144.nullable; goto _LL149; _LL149: _temp148=
_temp144.tq; goto _LL147; _LL147: _temp146= _temp144.bounds; goto _LL123;} else{
goto _LL124;} _LL124: if(( unsigned int) _temp120 > 4u?*(( int*) _temp120) ==
Cyc_Absyn_ArrayType: 0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp120)->f1; goto _LL159; _LL159: _temp158=(( struct Cyc_Absyn_ArrayType_struct*)
_temp120)->f2; goto _LL157; _LL157: _temp156=(( struct Cyc_Absyn_ArrayType_struct*)
_temp120)->f3; goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_TypedefType: 0){ _LL170: _temp169=((
struct Cyc_Absyn_TypedefType_struct*) _temp120)->f1; goto _LL168; _LL168:
_temp167=(( struct Cyc_Absyn_TypedefType_struct*) _temp120)->f2; goto _LL163;
_LL163: _temp162=(( struct Cyc_Absyn_TypedefType_struct*) _temp120)->f3; if(
_temp162 == 0){ goto _LL128;} else{ _temp164=* _temp162; _LL166: _temp165=( void*)
_temp164.v; goto _LL127;}} else{ goto _LL128;} _LL128: if(( unsigned int)
_temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_FnType: 0){ _LL172: _temp171=((
struct Cyc_Absyn_FnType_struct*) _temp120)->f1; _LL188: _temp187= _temp171.tvars;
goto _LL186; _LL186: _temp185= _temp171.effect; goto _LL184; _LL184: _temp183=(
void*) _temp171.ret_typ; goto _LL182; _LL182: _temp181= _temp171.args; goto
_LL180; _LL180: _temp179= _temp171.c_varargs; goto _LL178; _LL178: _temp177=
_temp171.cyc_varargs; goto _LL176; _LL176: _temp175= _temp171.rgn_po; goto
_LL174; _LL174: _temp173= _temp171.attributes; goto _LL129;} else{ goto _LL130;}
_LL130: if(( unsigned int) _temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_TupleType:
0){ _LL190: _temp189=(( struct Cyc_Absyn_TupleType_struct*) _temp120)->f1; goto
_LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp120 > 4u?*(( int*)
_temp120) == Cyc_Absyn_StructType: 0){ _LL197: _temp196=(( struct Cyc_Absyn_StructType_struct*)
_temp120)->f1; goto _LL195; _LL195: _temp194=(( struct Cyc_Absyn_StructType_struct*)
_temp120)->f2; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_StructType_struct*)
_temp120)->f3; if( _temp191 == 0){ goto _LL134;} else{ _temp193=* _temp191; goto
_LL133;}} else{ goto _LL134;} _LL134: if(( unsigned int) _temp120 > 4u?*(( int*)
_temp120) == Cyc_Absyn_UnionType: 0){ _LL204: _temp203=(( struct Cyc_Absyn_UnionType_struct*)
_temp120)->f1; goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_UnionType_struct*)
_temp120)->f2; goto _LL199; _LL199: _temp198=(( struct Cyc_Absyn_UnionType_struct*)
_temp120)->f3; if( _temp198 == 0){ goto _LL136;} else{ _temp200=* _temp198; goto
_LL135;}} else{ goto _LL136;} _LL136: if(( unsigned int) _temp120 > 4u?*(( int*)
_temp120) == Cyc_Absyn_EnumType: 0){ _LL208: _temp207=(( struct Cyc_Absyn_EnumType_struct*)
_temp120)->f1; goto _LL206; _LL206: _temp205=(( struct Cyc_Absyn_EnumType_struct*)
_temp120)->f2; goto _LL137;} else{ goto _LL138;} _LL138: if(( unsigned int)
_temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_TunionType: 0){ _LL210: _temp209=((
struct Cyc_Absyn_TunionType_struct*) _temp120)->f1; _LL216: _temp215=( void*)
_temp209.tunion_info; if(*(( int*) _temp215) == Cyc_Absyn_KnownTunion){ _LL218:
_temp217=(( struct Cyc_Absyn_KnownTunion_struct*) _temp215)->f1; goto _LL214;}
else{ goto _LL140;} _LL214: _temp213= _temp209.targs; goto _LL212; _LL212:
_temp211=( void*) _temp209.rgn; goto _LL139;} else{ goto _LL140;} _LL140: if((
unsigned int) _temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_TunionFieldType: 0){
_LL220: _temp219=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp120)->f1;
_LL224: _temp223=( void*) _temp219.field_info; if(*(( int*) _temp223) == Cyc_Absyn_KnownTunionfield){
_LL228: _temp227=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp223)->f1;
goto _LL226; _LL226: _temp225=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp223)->f2; goto _LL222;} else{ goto _LL142;} _LL222: _temp221= _temp219.targs;
goto _LL141;} else{ goto _LL142;} _LL142: goto _LL143; _LL123: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp154); _LL125: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp160); _LL127: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp165); _LL129: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp181)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp183): 0; if( _temp177 != 0){ int _temp231; void*
_temp233; void* _temp235; struct Cyc_Absyn_Tqual _temp237; struct Cyc_Core_Opt*
_temp239; struct Cyc_Absyn_VarargInfo _temp229=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp177)); _LL240: _temp239= _temp229.name; goto _LL238; _LL238:
_temp237= _temp229.tq; goto _LL236; _LL236: _temp235=( void*) _temp229.type;
goto _LL234; _LL234: _temp233=( void*) _temp229.rgn; goto _LL232; _LL232:
_temp231= _temp229.inject; goto _LL230; _LL230: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp235);} return b;} _LL131: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp189); _LL133: if(( void*)
_temp193->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({
unsigned char* _temp241=( unsigned char*)"struct"; struct _tagged_string
_temp242; _temp242.curr= _temp241; _temp242.base= _temp241; _temp242.last_plus_one=
_temp241 + 7; _temp242;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp193->name))->v); return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp194)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp193): 0; _LL135: if(( void*) _temp200->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ unsigned char* _temp243=( unsigned char*)"union";
struct _tagged_string _temp244; _temp244.curr= _temp243; _temp244.base= _temp243;
_temp244.last_plus_one= _temp243 + 6; _temp244;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp200->name))->v); return 0;} return(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp201)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp200): 0; _LL137: { struct _tuple0 _temp247; struct _tagged_string*
_temp248; void* _temp250; struct _tuple0* _temp245= _temp207; _temp247=*
_temp245; _LL251: _temp250= _temp247.f1; goto _LL249; _LL249: _temp248= _temp247.f2;
goto _LL246; _LL246: { struct Cyc_List_List* _temp252= Cyc_Interface_get_abs_ns(
_temp250); goto _LL253; _LL253: { struct Cyc_Absyn_Enumdecl* ed;{ struct
_handler_cons _temp254; _push_handler(& _temp254);{ int _temp256= 0; if( setjmp(
_temp254.handler)){ _temp256= 1;} if( ! _temp256){{ struct Cyc_Tcenv_Genv*
_temp257=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp252); goto _LL258; _LL258: ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp257->enumdecls,
_temp248);}; _pop_handler();} else{ void* _temp255=( void*) _exn_thrown; void*
_temp260= _temp255; _LL262: if( _temp260 == Cyc_Dict_Absent){ goto _LL263;}
else{ goto _LL264;} _LL264: goto _LL265; _LL263:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp266=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp266[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp267; _temp267.tag= Cyc_Core_InvalidArg; _temp267.f1=(
struct _tagged_string)({ struct _tagged_string _temp268= Cyc_Absynpp_qvar2string(
_temp207); xprintf("check_public_type (can't find enum %.*s)", _temp268.last_plus_one
- _temp268.curr, _temp268.curr);}); _temp267;}); _temp266;})); _LL265:( void)
_throw( _temp260); _LL261:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp207,({ unsigned char* _temp269=(
unsigned char*)"enum"; struct _tagged_string _temp270; _temp270.curr= _temp269;
_temp270.base= _temp269; _temp270.last_plus_one= _temp269 + 5; _temp270;}), ed->name);
return 0;} return 1;}}} _LL139: { struct _tuple0 _temp273; struct _tagged_string*
_temp274; void* _temp276; struct _tuple0* _temp271= _temp217->name; _temp273=*
_temp271; _LL277: _temp276= _temp273.f1; goto _LL275; _LL275: _temp274= _temp273.f2;
goto _LL272; _LL272: { struct Cyc_List_List* _temp278= Cyc_Interface_get_abs_ns(
_temp276); goto _LL279; _LL279: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp280; _push_handler(& _temp280);{ int _temp282= 0; if( setjmp(
_temp280.handler)){ _temp282= 1;} if( ! _temp282){{ struct Cyc_Tcenv_Genv*
_temp283=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp278); goto _LL284; _LL284: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp283->tuniondecls,
_temp274);}; _pop_handler();} else{ void* _temp281=( void*) _exn_thrown; void*
_temp286= _temp281; _LL288: if( _temp286 == Cyc_Dict_Absent){ goto _LL289;}
else{ goto _LL290;} _LL290: goto _LL291; _LL289:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp292=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp292[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp293; _temp293.tag= Cyc_Core_InvalidArg; _temp293.f1=(
struct _tagged_string)({ struct _tagged_string _temp294= Cyc_Absynpp_qvar2string(
_temp217->name); xprintf("check_public_type (can't find [x]tunion %.*s)",
_temp294.last_plus_one - _temp294.curr, _temp294.curr);}); _temp293;}); _temp292;}));
_LL291:( void) _throw( _temp286); _LL287:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ unsigned char* _temp295=( unsigned char*)"[x]tunion";
struct _tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 10; _temp296;}), tud->name); return 0;}
return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,( void*(*)( void*))
Cyc_Core_identity, _temp213)? Cyc_Interface_check_public_tuniondecl( ae, seen,
tud): 0;}}} _LL141: { struct _tuple0 _temp299; struct _tagged_string* _temp300;
void* _temp302; struct _tuple0* _temp297= _temp227->name; _temp299=* _temp297;
_LL303: _temp302= _temp299.f1; goto _LL301; _LL301: _temp300= _temp299.f2; goto
_LL298; _LL298: { struct Cyc_List_List* _temp304= Cyc_Interface_get_abs_ns(
_temp302); goto _LL305; _LL305: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp306; _push_handler(& _temp306);{ int _temp308= 0; if( setjmp(
_temp306.handler)){ _temp308= 1;} if( ! _temp308){{ struct Cyc_Tcenv_Genv*
_temp309=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp304); goto _LL310; _LL310: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp309->tuniondecls,
_temp300);}; _pop_handler();} else{ void* _temp307=( void*) _exn_thrown; void*
_temp312= _temp307; _LL314: if( _temp312 == Cyc_Dict_Absent){ goto _LL315;}
else{ goto _LL316;} _LL316: goto _LL317; _LL315:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp318=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp318[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp319; _temp319.tag= Cyc_Core_InvalidArg; _temp319.f1=(
struct _tagged_string)({ struct _tagged_string _temp320= Cyc_Absynpp_qvar2string(
_temp227->name); xprintf("check_public_type (can't find [x]tunion %.*s and search its fields)",
_temp320.last_plus_one - _temp320.curr, _temp320.curr);}); _temp319;}); _temp318;}));
_LL317:( void) _throw( _temp312); _LL313:;}}} if( tud->fields == 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp321=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp321[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp322; _temp322.tag= Cyc_Core_InvalidArg; _temp322.f1=(
struct _tagged_string)({ struct _tagged_string _temp323= Cyc_Absynpp_qvar2string(
tud->name); xprintf("check_public_type ([x]tunion %.*s has no fields)", _temp323.last_plus_one
- _temp323.curr, _temp323.curr);}); _temp322;}); _temp321;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp324=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; goto _LL325; _LL325: for( 0; _temp324 != 0;
_temp324=(( struct Cyc_List_List*) _check_null( _temp324))->tl){ if( Cyc_String_strptrcmp((*
_temp225->name).f2,(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp324))->hd)->name).f2) == 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp324))->hd);
break;}}} if( tuf1 == 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp326=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp326[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp327; _temp327.tag= Cyc_Core_InvalidArg;
_temp327.f1=( struct _tagged_string)({ struct _tagged_string _temp328= Cyc_Absynpp_qvar2string(
_temp225->name); xprintf("check_public_type (can't find [x]tunionfield %.*s)",
_temp328.last_plus_one - _temp328.curr, _temp328.curr);}); _temp327;}); _temp326;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*) _check_null(
tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,({ unsigned char* _temp329=( unsigned char*)"[x]tunion"; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 10; _temp330;}), tud->name); return 0;} if((
void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err( obj,
name,({ unsigned char* _temp331=( unsigned char*)"[x]tunion"; struct
_tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 10; _temp332;}), tud->name); return 0;} if((
void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,(
struct _tagged_string)({ struct _tagged_string _temp333= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _temp333.last_plus_one - _temp333.curr,
_temp333.curr);}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp221)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}}} _LL143: return 1; _LL121:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp336; struct Cyc_Interface_I* _temp337; struct Cyc_Interface_Seen*
_temp339; struct Cyc_Dict_Dict* _temp341; int _temp343; struct Cyc_Interface_Ienv*
_temp345; struct Cyc_Interface_Ienv* _temp347; struct _tuple11* _temp334= env;
_temp336=* _temp334; _LL348: _temp347= _temp336.f1; goto _LL346; _LL346:
_temp345= _temp336.f2; goto _LL344; _LL344: _temp343= _temp336.f3; goto _LL342;
_LL342: _temp341= _temp336.f4; goto _LL340; _LL340: _temp339= _temp336.f5; goto
_LL338; _LL338: _temp337= _temp336.f6; goto _LL335; _LL335: { struct Cyc_Absyn_Structdecl*
_temp349=* dp; goto _LL350; _LL350: { void* _temp351=( void*) _temp349->sc;
_LL353: if( _temp351 ==( void*) Cyc_Absyn_Static){ goto _LL354;} else{ goto
_LL355;} _LL355: if( _temp351 ==( void*) Cyc_Absyn_Abstract){ goto _LL356;}
else{ goto _LL357;} _LL357: if( _temp351 ==( void*) Cyc_Absyn_Public){ goto
_LL358;} else{ goto _LL359;} _LL359: if( _temp351 ==( void*) Cyc_Absyn_ExternC){
goto _LL360;} else{ goto _LL361;} _LL361: if( _temp351 ==( void*) Cyc_Absyn_Extern){
goto _LL362;} else{ goto _LL352;} _LL354: if( _temp343? _temp349->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp363=( unsigned char*)"static";
struct _tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 7; _temp364;}),({ unsigned char* _temp365=(
unsigned char*)"struct"; struct _tagged_string _temp366; _temp366.curr= _temp365;
_temp366.base= _temp365; _temp366.last_plus_one= _temp365 + 7; _temp366;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp349->name))->v);}
goto _LL352; _LL356: if( _temp349->fields == 0){ if( _temp343){ Cyc_Interface_fields_err(({
unsigned char* _temp367=( unsigned char*)"abstract"; struct _tagged_string
_temp368; _temp368.curr= _temp367; _temp368.base= _temp367; _temp368.last_plus_one=
_temp367 + 9; _temp368;}),({ unsigned char* _temp369=( unsigned char*)"struct";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 7; _temp370;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp349->name))->v);}} else{ _temp349=({ struct Cyc_Absyn_Structdecl*
_temp371=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp371[ 0]=* _temp349; _temp371;}); _temp349->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp341, _temp339, _temp349)){ _temp345->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp345->structdecls, x, _temp349);} goto _LL352;
_LL358: if( _temp349->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp372=( unsigned char*)"public"; struct _tagged_string _temp373; _temp373.curr=
_temp372; _temp373.base= _temp372; _temp373.last_plus_one= _temp372 + 7;
_temp373;}),({ unsigned char* _temp374=( unsigned char*)"struct"; struct
_tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 7; _temp375;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp349->name))->v); _temp349=({ struct Cyc_Absyn_Structdecl*
_temp376=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp376[ 0]=* _temp349; _temp376;});( void*)( _temp349->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl( _temp341,
_temp339, _temp349)){ _temp345->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp345->structdecls, x, _temp349);} goto _LL352; _LL360:
goto _LL362; _LL362: if( Cyc_Interface_check_public_structdecl( _temp341,
_temp339, _temp349)){ _temp347->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp347->structdecls, x, _temp349);} goto _LL352; _LL352:;}}}
static void Cyc_Interface_extract_uniondecl( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11 _temp379;
struct Cyc_Interface_I* _temp380; struct Cyc_Interface_Seen* _temp382; struct
Cyc_Dict_Dict* _temp384; int _temp386; struct Cyc_Interface_Ienv* _temp388;
struct Cyc_Interface_Ienv* _temp390; struct _tuple11* _temp377= env; _temp379=*
_temp377; _LL391: _temp390= _temp379.f1; goto _LL389; _LL389: _temp388= _temp379.f2;
goto _LL387; _LL387: _temp386= _temp379.f3; goto _LL385; _LL385: _temp384=
_temp379.f4; goto _LL383; _LL383: _temp382= _temp379.f5; goto _LL381; _LL381:
_temp380= _temp379.f6; goto _LL378; _LL378: { struct Cyc_Absyn_Uniondecl*
_temp392=* dp; goto _LL393; _LL393: { void* _temp394=( void*) _temp392->sc;
_LL396: if( _temp394 ==( void*) Cyc_Absyn_Static){ goto _LL397;} else{ goto
_LL398;} _LL398: if( _temp394 ==( void*) Cyc_Absyn_Abstract){ goto _LL399;}
else{ goto _LL400;} _LL400: if( _temp394 ==( void*) Cyc_Absyn_Public){ goto
_LL401;} else{ goto _LL402;} _LL402: if( _temp394 ==( void*) Cyc_Absyn_ExternC){
goto _LL403;} else{ goto _LL404;} _LL404: if( _temp394 ==( void*) Cyc_Absyn_Extern){
goto _LL405;} else{ goto _LL395;} _LL397: if( _temp386? _temp392->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp406=( unsigned char*)"static";
struct _tagged_string _temp407; _temp407.curr= _temp406; _temp407.base= _temp406;
_temp407.last_plus_one= _temp406 + 7; _temp407;}),({ unsigned char* _temp408=(
unsigned char*)"union"; struct _tagged_string _temp409; _temp409.curr= _temp408;
_temp409.base= _temp408; _temp409.last_plus_one= _temp408 + 6; _temp409;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp392->name))->v);}
goto _LL395; _LL399: if( _temp392->fields == 0){ if( _temp386){ Cyc_Interface_fields_err(({
unsigned char* _temp410=( unsigned char*)"abstract"; struct _tagged_string
_temp411; _temp411.curr= _temp410; _temp411.base= _temp410; _temp411.last_plus_one=
_temp410 + 9; _temp411;}),({ unsigned char* _temp412=( unsigned char*)"union";
struct _tagged_string _temp413; _temp413.curr= _temp412; _temp413.base= _temp412;
_temp413.last_plus_one= _temp412 + 6; _temp413;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp392->name))->v);}} else{ _temp392=({ struct Cyc_Absyn_Uniondecl*
_temp414=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp414[ 0]=* _temp392; _temp414;}); _temp392->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp384, _temp382, _temp392)){ _temp388->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp388->uniondecls, x, _temp392);} goto _LL395;
_LL401: if( _temp392->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp415=( unsigned char*)"public"; struct _tagged_string _temp416; _temp416.curr=
_temp415; _temp416.base= _temp415; _temp416.last_plus_one= _temp415 + 7;
_temp416;}),({ unsigned char* _temp417=( unsigned char*)"union"; struct
_tagged_string _temp418; _temp418.curr= _temp417; _temp418.base= _temp417;
_temp418.last_plus_one= _temp417 + 6; _temp418;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp392->name))->v); _temp392=({ struct Cyc_Absyn_Uniondecl*
_temp419=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp419[ 0]=* _temp392; _temp419;});( void*)( _temp392->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl( _temp384,
_temp382, _temp392)){ _temp388->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data))
Cyc_Dict_insert)( _temp388->uniondecls, x, _temp392);} goto _LL395; _LL403: goto
_LL405; _LL405: if( Cyc_Interface_check_public_uniondecl( _temp384, _temp382,
_temp392)){ _temp390->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp390->uniondecls, x, _temp392);} goto _LL395; _LL395:;}}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp422; struct Cyc_Interface_I* _temp423; struct Cyc_Interface_Seen*
_temp425; struct Cyc_Dict_Dict* _temp427; int _temp429; struct Cyc_Interface_Ienv*
_temp431; struct Cyc_Interface_Ienv* _temp433; struct _tuple11* _temp420= env;
_temp422=* _temp420; _LL434: _temp433= _temp422.f1; goto _LL432; _LL432:
_temp431= _temp422.f2; goto _LL430; _LL430: _temp429= _temp422.f3; goto _LL428;
_LL428: _temp427= _temp422.f4; goto _LL426; _LL426: _temp425= _temp422.f5; goto
_LL424; _LL424: _temp423= _temp422.f6; goto _LL421; _LL421: { struct Cyc_Absyn_Enumdecl*
_temp435=* dp; goto _LL436; _LL436: { void* _temp437=( void*) _temp435->sc;
_LL439: if( _temp437 ==( void*) Cyc_Absyn_Static){ goto _LL440;} else{ goto
_LL441;} _LL441: if( _temp437 ==( void*) Cyc_Absyn_Abstract){ goto _LL442;}
else{ goto _LL443;} _LL443: if( _temp437 ==( void*) Cyc_Absyn_Public){ goto
_LL444;} else{ goto _LL445;} _LL445: if( _temp437 ==( void*) Cyc_Absyn_ExternC){
goto _LL446;} else{ goto _LL447;} _LL447: if( _temp437 ==( void*) Cyc_Absyn_Extern){
goto _LL448;} else{ goto _LL438;} _LL440: if( _temp429? _temp435->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp449=( unsigned char*)"static";
struct _tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 7; _temp450;}),({ unsigned char* _temp451=(
unsigned char*)"enum"; struct _tagged_string _temp452; _temp452.curr= _temp451;
_temp452.base= _temp451; _temp452.last_plus_one= _temp451 + 5; _temp452;}),
_temp435->name);} goto _LL438; _LL442: if( _temp435->fields == 0){ if( _temp429){
Cyc_Interface_fields_err(({ unsigned char* _temp453=( unsigned char*)"abstract";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 9; _temp454;}),({ unsigned char* _temp455=(
unsigned char*)"enum"; struct _tagged_string _temp456; _temp456.curr= _temp455;
_temp456.base= _temp455; _temp456.last_plus_one= _temp455 + 5; _temp456;}),
_temp435->name);}} else{ _temp435=({ struct Cyc_Absyn_Enumdecl* _temp457=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp457[ 0]=* _temp435; _temp457;}); _temp435->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp427, _temp425, _temp435)){ _temp431->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp431->enumdecls, x, _temp435);} goto _LL438; _LL444:
if( _temp435->fields == 0){ Cyc_Interface_fields_err(({ unsigned char* _temp458=(
unsigned char*)"public"; struct _tagged_string _temp459; _temp459.curr= _temp458;
_temp459.base= _temp458; _temp459.last_plus_one= _temp458 + 7; _temp459;}),({
unsigned char* _temp460=( unsigned char*)"enum"; struct _tagged_string _temp461;
_temp461.curr= _temp460; _temp461.base= _temp460; _temp461.last_plus_one=
_temp460 + 5; _temp461;}), _temp435->name); _temp435=({ struct Cyc_Absyn_Enumdecl*
_temp462=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp462[ 0]=* _temp435; _temp462;});( void*)( _temp435->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl( _temp427,
_temp425, _temp435)){ _temp431->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp431->enumdecls, x, _temp435);} goto _LL438; _LL446: goto
_LL448; _LL448: if( Cyc_Interface_check_public_enumdecl( _temp427, _temp425,
_temp435)){ _temp433->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(
_temp433->enumdecls, x, _temp435);} goto _LL438; _LL438:;}}} static void Cyc_Interface_extract_xtunionfielddecl(
struct Cyc_Interface_I* i, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield*
f){ struct Cyc_List_List* _temp463= 0; goto _LL464; _LL464:{ void* _temp465=(* f->name).f1;
struct Cyc_List_List* _temp473; struct Cyc_List_List* _temp475; _LL467: if((
unsigned int) _temp465 > 1u?*(( int*) _temp465) == Cyc_Absyn_Rel_n: 0){ _LL474:
_temp473=(( struct Cyc_Absyn_Rel_n_struct*) _temp465)->f1; goto _LL468;} else{
goto _LL469;} _LL469: if(( unsigned int) _temp465 > 1u?*(( int*) _temp465) ==
Cyc_Absyn_Abs_n: 0){ _LL476: _temp475=(( struct Cyc_Absyn_Abs_n_struct*)
_temp465)->f1; goto _LL470;} else{ goto _LL471;} _LL471: goto _LL472; _LL468:
_temp475= _temp473; goto _LL470; _LL470: _temp463= _temp475; goto _LL466; _LL472:
goto _LL466; _LL466:;}{ struct Cyc_Dict_Dict** dict;{ void* _temp477=( void*) f->sc;
_LL479: if( _temp477 ==( void*) Cyc_Absyn_Static){ goto _LL480;} else{ goto
_LL481;} _LL481: if( _temp477 ==( void*) Cyc_Absyn_Extern){ goto _LL482;} else{
goto _LL483;} _LL483: if( _temp477 ==( void*) Cyc_Absyn_Public){ goto _LL484;}
else{ goto _LL485;} _LL485: goto _LL486; _LL480: return; _LL482: dict=& i->imports;
goto _LL478; _LL484: dict=& i->exports; goto _LL478; _LL486:( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp487=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp487[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp488; _temp488.tag= Cyc_Core_InvalidArg; _temp488.f1=({
unsigned char* _temp489=( unsigned char*)"add_xtunionfielddecl"; struct
_tagged_string _temp490; _temp490.curr= _temp489; _temp490.base= _temp489;
_temp490.last_plus_one= _temp489 + 21; _temp490;}); _temp488;}); _temp487;}));
_LL478:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons _temp491;
_push_handler(& _temp491);{ int _temp493= 0; if( setjmp( _temp491.handler)){
_temp493= 1;} if( ! _temp493){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp463);; _pop_handler();}
else{ void* _temp492=( void*) _exn_thrown; void* _temp495= _temp492; _LL497: if(
_temp495 == Cyc_Dict_Absent){ goto _LL498;} else{ goto _LL499;} _LL499: goto
_LL500; _LL498: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp463, env); goto _LL496; _LL500:( void)
_throw( _temp495); _LL496:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp501=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp501->base= d; _temp501->field= f; _temp501;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp504; struct Cyc_Interface_I* _temp505; struct Cyc_Interface_Seen*
_temp507; struct Cyc_Dict_Dict* _temp509; int _temp511; struct Cyc_Interface_Ienv*
_temp513; struct Cyc_Interface_Ienv* _temp515; struct _tuple11* _temp502= env;
_temp504=* _temp502; _LL516: _temp515= _temp504.f1; goto _LL514; _LL514:
_temp513= _temp504.f2; goto _LL512; _LL512: _temp511= _temp504.f3; goto _LL510;
_LL510: _temp509= _temp504.f4; goto _LL508; _LL508: _temp507= _temp504.f5; goto
_LL506; _LL506: _temp505= _temp504.f6; goto _LL503; _LL503: { struct Cyc_Absyn_Tuniondecl*
_temp517=* dp; goto _LL518; _LL518: { void* _temp519=( void*) _temp517->sc;
_LL521: if( _temp519 ==( void*) Cyc_Absyn_Static){ goto _LL522;} else{ goto
_LL523;} _LL523: if( _temp519 ==( void*) Cyc_Absyn_Abstract){ goto _LL524;}
else{ goto _LL525;} _LL525: if( _temp519 ==( void*) Cyc_Absyn_Public){ goto
_LL526;} else{ goto _LL527;} _LL527: if( _temp519 ==( void*) Cyc_Absyn_ExternC){
goto _LL528;} else{ goto _LL529;} _LL529: if( _temp519 ==( void*) Cyc_Absyn_Extern){
goto _LL530;} else{ goto _LL520;} _LL522: if(( ! _temp517->is_xtunion? _temp511:
0)? _temp517->fields == 0: 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp531=( unsigned char*)"static"; struct _tagged_string _temp532; _temp532.curr=
_temp531; _temp532.base= _temp531; _temp532.last_plus_one= _temp531 + 7;
_temp532;}),({ unsigned char* _temp533=( unsigned char*)"tunion"; struct
_tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 7; _temp534;}), _temp517->name);} goto _LL520;
_LL524: if( _temp517->fields == 0){ if( ! _temp517->is_xtunion? _temp511: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp535=( unsigned char*)"abstract";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 9; _temp536;}),({ unsigned char* _temp537=(
unsigned char*)"tunion"; struct _tagged_string _temp538; _temp538.curr= _temp537;
_temp538.base= _temp537; _temp538.last_plus_one= _temp537 + 7; _temp538;}),
_temp517->name);}} else{ _temp517=({ struct Cyc_Absyn_Tuniondecl* _temp539=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp539[ 0]=* _temp517; _temp539;}); _temp517->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp509, _temp507, _temp517)){ _temp513->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp513->tuniondecls, x, _temp517);} goto _LL520;
_LL526: _temp517=({ struct Cyc_Absyn_Tuniondecl* _temp540=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp540[ 0]=* _temp517;
_temp540;}); if( ! _temp517->is_xtunion? _temp517->fields == 0: 0){ Cyc_Interface_fields_err(({
unsigned char* _temp541=( unsigned char*)"public"; struct _tagged_string
_temp542; _temp542.curr= _temp541; _temp542.base= _temp541; _temp542.last_plus_one=
_temp541 + 7; _temp542;}),({ unsigned char* _temp543=( unsigned char*)"tunion";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 7; _temp544;}), _temp517->name);( void*)(
_temp517->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp509, _temp507, _temp517)){ if( _temp517->is_xtunion? _temp517->fields != 0:
0){ struct Cyc_List_List* _temp545=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp517->fields))->v; goto _LL546; _LL546: _temp517->fields= 0;{
struct Cyc_List_List* _temp547= _temp545; goto _LL548; _LL548: for( 0; _temp547
!= 0; _temp547=(( struct Cyc_List_List*) _check_null( _temp547))->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp505, _temp517,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp547))->hd);}}} _temp513->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp513->tuniondecls, x, _temp517);} goto _LL520;
_LL528:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp549=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp549[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp550; _temp550.tag= Cyc_Core_InvalidArg;
_temp550.f1=({ unsigned char* _temp551=( unsigned char*)"extract_tuniondecl";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 19; _temp552;}); _temp550;}); _temp549;}));
_LL530: if( Cyc_Interface_check_public_tuniondecl( _temp509, _temp507, _temp517)){
if( _temp517->is_xtunion? _temp517->fields != 0: 0){ _temp517=({ struct Cyc_Absyn_Tuniondecl*
_temp553=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp553[ 0]=* _temp517; _temp553;});{ struct Cyc_List_List* _temp554=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp517->fields))->v; goto
_LL555; _LL555: _temp517->fields= 0;{ struct Cyc_List_List* _temp556= _temp554;
goto _LL557; _LL557: for( 0; _temp556 != 0; _temp556=(( struct Cyc_List_List*)
_check_null( _temp556))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp505,
_temp517,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp556))->hd);}}}} _temp515->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp515->tuniondecls, x, _temp517);} goto _LL520; _LL520:;}}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp560;
struct Cyc_Interface_I* _temp561; struct Cyc_Interface_Seen* _temp563; struct
Cyc_Dict_Dict* _temp565; int _temp567; struct Cyc_Interface_Ienv* _temp569;
struct Cyc_Interface_Ienv* _temp571; struct _tuple11* _temp558= env; _temp560=*
_temp558; _LL572: _temp571= _temp560.f1; goto _LL570; _LL570: _temp569= _temp560.f2;
goto _LL568; _LL568: _temp567= _temp560.f3; goto _LL566; _LL566: _temp565=
_temp560.f4; goto _LL564; _LL564: _temp563= _temp560.f5; goto _LL562; _LL562:
_temp561= _temp560.f6; goto _LL559; _LL559: if( Cyc_Interface_check_public_typedefdecl(
_temp565, _temp563, d)){ _temp569->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp569->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_string* x, struct _tuple7* v){ struct
_tuple11 _temp575; struct Cyc_Interface_I* _temp576; struct Cyc_Interface_Seen*
_temp578; struct Cyc_Dict_Dict* _temp580; int _temp582; struct Cyc_Interface_Ienv*
_temp584; struct Cyc_Interface_Ienv* _temp586; struct _tuple11* _temp573= env;
_temp575=* _temp573; _LL587: _temp586= _temp575.f1; goto _LL585; _LL585:
_temp584= _temp575.f2; goto _LL583; _LL583: _temp582= _temp575.f3; goto _LL581;
_LL581: _temp580= _temp575.f4; goto _LL579; _LL579: _temp578= _temp575.f5; goto
_LL577; _LL577: _temp576= _temp575.f6; goto _LL574; _LL574: { void* _temp588=(*
v).f1; goto _LL589; _LL589: { void* _temp590= _temp588; void* _temp596; _LL592:
if(*(( int*) _temp590) == Cyc_Tcenv_VarRes){ _LL597: _temp596=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp590)->f1; goto _LL593;} else{ goto _LL594;}
_LL594: goto _LL595; _LL593:{ void* _temp598= _temp596; struct Cyc_Absyn_Fndecl*
_temp606; struct Cyc_Absyn_Vardecl* _temp608; _LL600: if(( unsigned int)
_temp598 > 1u?*(( int*) _temp598) == Cyc_Absyn_Funname_b: 0){ _LL607: _temp606=((
struct Cyc_Absyn_Funname_b_struct*) _temp598)->f1; goto _LL601;} else{ goto
_LL602;} _LL602: if(( unsigned int) _temp598 > 1u?*(( int*) _temp598) == Cyc_Absyn_Global_b:
0){ _LL609: _temp608=(( struct Cyc_Absyn_Global_b_struct*) _temp598)->f1; goto
_LL603;} else{ goto _LL604;} _LL604: goto _LL605; _LL601: { struct Cyc_Absyn_Vardecl*
_temp612=({ struct Cyc_Absyn_Vardecl* _temp610=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp610->sc=( void*)(( void*)
_temp606->sc); _temp610->name= _temp606->name; _temp610->tq=({ struct Cyc_Absyn_Tqual
_temp611; _temp611.q_const= 0; _temp611.q_volatile= 0; _temp611.q_restrict= 0;
_temp611;}); _temp610->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp606->cached_typ))->v); _temp610->initializer= 0; _temp610->rgn=
0; _temp610->attributes= 0; _temp610;}); goto _LL613; _LL613: _temp582= 0;
_temp608= _temp612; goto _LL603;} _LL603: if( _temp608->initializer != 0){
_temp608=({ struct Cyc_Absyn_Vardecl* _temp614=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp614[ 0]=* _temp608; _temp614;});
_temp608->initializer= 0;}{ void* _temp615=( void*) _temp608->sc; _LL617: if(
_temp615 ==( void*) Cyc_Absyn_Static){ goto _LL618;} else{ goto _LL619;} _LL619:
if( _temp615 ==( void*) Cyc_Absyn_Abstract){ goto _LL620;} else{ goto _LL621;}
_LL621: if( _temp615 ==( void*) Cyc_Absyn_Public){ goto _LL622;} else{ goto
_LL623;} _LL623: if( _temp615 ==( void*) Cyc_Absyn_ExternC){ goto _LL624;} else{
goto _LL625;} _LL625: if( _temp615 ==( void*) Cyc_Absyn_Extern){ goto _LL626;}
else{ goto _LL616;} _LL618: if( _temp582? Cyc_Tcutil_is_function_type(( void*)
_temp608->type): 0){ Cyc_Interface_body_err(({ unsigned char* _temp627=(
unsigned char*)"static"; struct _tagged_string _temp628; _temp628.curr= _temp627;
_temp628.base= _temp627; _temp628.last_plus_one= _temp627 + 7; _temp628;}),
_temp608->name);} goto _LL616; _LL620:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp629=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp629[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp630; _temp630.tag= Cyc_Core_InvalidArg;
_temp630.f1=({ unsigned char* _temp631=( unsigned char*)"extract_ordinarie";
struct _tagged_string _temp632; _temp632.curr= _temp631; _temp632.base= _temp631;
_temp632.last_plus_one= _temp631 + 18; _temp632;}); _temp630;}); _temp629;}));
_LL622: if( _temp582? Cyc_Tcutil_is_function_type(( void*) _temp608->type): 0){
Cyc_Interface_body_err(({ unsigned char* _temp633=( unsigned char*)"public";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 7; _temp634;}), _temp608->name);} if( Cyc_Interface_check_public_vardecl(
_temp580, _temp578, _temp608)){ _temp584->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( _temp584->vardecls, x, _temp608);} goto _LL616; _LL624:
goto _LL626; _LL626: if( Cyc_Interface_check_public_vardecl( _temp580, _temp578,
_temp608)){ _temp586->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp586->vardecls, x, _temp608);} goto _LL616; _LL616:;} goto _LL599; _LL605:
goto _LL599; _LL599:;} goto _LL591; _LL595: goto _LL591; _LL591:;}}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp637; struct Cyc_Interface_Seen* _temp638; struct Cyc_Dict_Dict* _temp640;
int _temp642; struct Cyc_Interface_I* _temp644; struct _tuple12* _temp635= env_f;
_temp637=* _temp635; _LL645: _temp644= _temp637.f1; goto _LL643; _LL643:
_temp642= _temp637.f2; goto _LL641; _LL641: _temp640= _temp637.f3; goto _LL639;
_LL639: _temp638= _temp637.f4; goto _LL636; _LL636: { struct _tuple11 _temp647=({
struct _tuple11 _temp646; _temp646.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp644->imports,
ns); _temp646.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp644->exports, ns); _temp646.f3=
_temp642; _temp646.f4= _temp640; _temp646.f5= _temp638; _temp646.f6= _temp644;
_temp646;}); goto _LL648; _LL648:(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& _temp647,
ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct _tagged_string*,
struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp647, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp647, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp647, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp647, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp647, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp650=({ struct _tuple12
_temp649; _temp649.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp649.f2= check_complete_defs; _temp649.f3= ae;
_temp649.f4= Cyc_Interface_new_seen(); _temp649;}); goto _LL651; _LL651:(( void(*)(
void(* f)( struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*),
struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp650, ae); return _temp650.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_string* msg1, struct
_tagged_string msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_string*);
struct _tagged_string f4; struct _tagged_string* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_string* x, void* y1){ struct _tuple13
_temp654; struct _tagged_string* _temp655; struct _tagged_string _temp657; int(*
_temp659)( void*, void*, struct _tagged_string*); struct Cyc_Dict_Dict* _temp661;
int _temp663; int* _temp665; struct _tuple13* _temp652= env; _temp654=* _temp652;
_LL664: _temp663= _temp654.f1; _temp665=&(* _temp652).f1; goto _LL662; _LL662:
_temp661= _temp654.f2; goto _LL660; _LL660: _temp659= _temp654.f3; goto _LL658;
_LL658: _temp657= _temp654.f4; goto _LL656; _LL656: _temp655= _temp654.f5; goto
_LL653; _LL653: { struct _handler_cons _temp666; _push_handler(& _temp666);{ int
_temp668= 0; if( setjmp( _temp666.handler)){ _temp668= 1;} if( ! _temp668){{
void* _temp669=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp661, x); goto _LL670; _LL670: if( ! _temp659( y1,
_temp669, _temp655)){* _temp665= 0;}}; _pop_handler();} else{ void* _temp667=(
void*) _exn_thrown; void* _temp672= _temp667; _LL674: if( _temp672 == Cyc_Dict_Absent){
goto _LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL675: Cyc_Interface_check_err(
_temp655,( struct _tagged_string)({ struct _tagged_string _temp678= _temp657;
struct _tagged_string _temp679=* x; xprintf("%.*s %.*s is missing", _temp678.last_plus_one
- _temp678.curr, _temp678.curr, _temp679.last_plus_one - _temp679.curr, _temp679.curr);}));*
_temp665= 0; goto _LL673; _LL677:( void) _throw( _temp672); _LL673:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple13 _temp681=({
struct _tuple13 _temp680; _temp680.f1= 1; _temp680.f2= dic2; _temp680.f3= incl_f;
_temp680.f4= t; _temp680.f5= msg; _temp680;}); goto _LL682; _LL682:(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_string*, void*), struct _tuple13*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& _temp681, dic1);
return _temp681.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Structdecl*
_temp683= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); goto _LL684; _LL684: if(
_temp683 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp683){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp685= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp685.last_plus_one - _temp685.curr, _temp685.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct
Cyc_Absyn_Uniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Uniondecl*
_temp686= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); goto _LL687; _LL687: if(
_temp686 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 != _temp686){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp688= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp688.last_plus_one - _temp688.curr, _temp688.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Tuniondecl*
_temp689= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); goto _LL690; _LL690: if(
_temp689 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 != _temp689){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp691= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp691.last_plus_one - _temp691.curr, _temp691.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Enumdecl*
_temp692= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); goto _LL693; _LL693: if(
_temp692 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp692){ Cyc_Interface_check_err(
msg,( struct _tagged_string)({ struct _tagged_string _temp694= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp694.last_plus_one - _temp694.curr, _temp694.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_string* msg){ struct Cyc_Absyn_Vardecl* _temp695= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL696; _LL696: if( _temp695 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp695){ Cyc_Interface_check_err( msg,( struct
_tagged_string)({ struct _tagged_string _temp697= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of variable %.*s discloses too much information", _temp697.last_plus_one
- _temp697.curr, _temp697.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct
_tagged_string* msg){ struct Cyc_Absyn_Typedefdecl* _temp698= Cyc_Tcdecl_merge_typedefdecl(
d0, d1, 0, msg); goto _LL699; _LL699: if( _temp698 == 0){ return 0;} if(( struct
Cyc_Absyn_Typedefdecl*) d0 != _temp698){ Cyc_Interface_check_err( msg,( struct
_tagged_string)({ struct _tagged_string _temp700= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of typedef %.*s discloses too much information", _temp700.last_plus_one
- _temp700.curr, _temp700.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct _tagged_string* msg){ struct Cyc_Tcdecl_Xtunionfielddecl* _temp701= Cyc_Tcdecl_merge_xtunionfielddecl(
d0, d1, 0, msg); goto _LL702; _LL702: if( _temp701 == 0){ return 0;} if(( struct
Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp701){ Cyc_Interface_check_err( msg,(
struct _tagged_string)({ struct _tagged_string _temp703= Cyc_Absynpp_qvar2string((
d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp703.last_plus_one - _temp703.curr, _temp703.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ int _temp706=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl,({
unsigned char* _temp704=( unsigned char*)"struct"; struct _tagged_string
_temp705; _temp705.curr= _temp704; _temp705.base= _temp704; _temp705.last_plus_one=
_temp704 + 7; _temp705;}), msg); goto _LL707; _LL707: { int _temp710=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->uniondecls, ie2->uniondecls, Cyc_Interface_incl_uniondecl,({ unsigned char*
_temp708=( unsigned char*)"union"; struct _tagged_string _temp709; _temp709.curr=
_temp708; _temp709.base= _temp708; _temp709.last_plus_one= _temp708 + 6;
_temp709;}), msg); goto _LL711; _LL711: { int _temp714=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->tuniondecls, ie2->tuniondecls,
Cyc_Interface_incl_tuniondecl,({ unsigned char* _temp712=( unsigned char*)"tunion";
struct _tagged_string _temp713; _temp713.curr= _temp712; _temp713.base= _temp712;
_temp713.last_plus_one= _temp712 + 7; _temp713;}), msg); goto _LL715; _LL715: {
int _temp718=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,({ unsigned char*
_temp716=( unsigned char*)"enum"; struct _tagged_string _temp717; _temp717.curr=
_temp716; _temp717.base= _temp716; _temp717.last_plus_one= _temp716 + 5;
_temp717;}), msg); goto _LL719; _LL719: { int _temp722=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp720=( unsigned char*)"typedef";
struct _tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 8; _temp721;}), msg) != 0; goto _LL723;
_LL723: { int _temp726=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl,({
unsigned char* _temp724=( unsigned char*)"variable"; struct _tagged_string
_temp725; _temp725.curr= _temp724; _temp725.base= _temp724; _temp725.last_plus_one=
_temp724 + 9; _temp725;}), msg); goto _LL727; _LL727: { int _temp730=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,({
unsigned char* _temp728=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp729; _temp729.curr= _temp728; _temp729.base= _temp728; _temp729.last_plus_one=
_temp728 + 13; _temp729;}), msg); goto _LL731; _LL731: return((((( _temp706?
_temp710: 0)? _temp714: 0)? _temp718: 0)? _temp722: 0)? _temp726: 0)? _temp730:
0;}}}}}}} struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct
_tagged_string* f3; } ; static void Cyc_Interface_incl_ns_f( struct _tuple14*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){ struct _tuple14
_temp734; struct _tagged_string* _temp735; struct Cyc_Dict_Dict* _temp737; int
_temp739; int* _temp741; struct _tuple14* _temp732= env; _temp734=* _temp732;
_LL740: _temp739= _temp734.f1; _temp741=&(* _temp732).f1; goto _LL738; _LL738:
_temp737= _temp734.f2; goto _LL736; _LL736: _temp735= _temp734.f3; goto _LL733;
_LL733: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp742;
_push_handler(& _temp742);{ int _temp744= 0; if( setjmp( _temp742.handler)){
_temp744= 1;} if( ! _temp744){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp737, ns);; _pop_handler();}
else{ void* _temp743=( void*) _exn_thrown; void* _temp746= _temp743; _LL748: if(
_temp746 == Cyc_Dict_Absent){ goto _LL749;} else{ goto _LL750;} _LL750: goto
_LL751; _LL749: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL747; _LL751:( void)
_throw( _temp746); _LL747:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp735)){*
_temp741= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple14
_temp753=({ struct _tuple14 _temp752; _temp752.f1= 1; _temp752.f2= dic2;
_temp752.f3= msg; _temp752;}); goto _LL754; _LL754:(( void(*)( void(* f)( struct
_tuple14*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple14*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_ns_f,&
_temp753, dic1); return _temp753.f1;} int Cyc_Interface_is_subinterface( struct
Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct
_handler_cons _temp755; _push_handler(& _temp755);{ int _temp757= 0; if( setjmp(
_temp755.handler)){ _temp757= 1;} if( ! _temp757){{ int _temp758= 1; goto _LL759;
_LL759: { struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct
_tagged_string* _temp760=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp760[ 0]=( struct _tagged_string)({ struct _tagged_string
_temp761=(*(( struct _tuple3*) _check_null( info))).f1; struct _tagged_string
_temp762=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp761.last_plus_one - _temp761.curr, _temp761.curr, _temp762.last_plus_one -
_temp762.curr, _temp762.curr);}); _temp760;});} if( ! Cyc_Interface_incl_ns( i1->exports,
i2->exports, msg)){ _temp758= 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp763=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp763[ 0]=( struct _tagged_string)({ struct _tagged_string _temp764=(*((
struct _tuple3*) _check_null( info))).f2; struct _tagged_string _temp765=(*((
struct _tuple3*) _check_null( info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp764.last_plus_one - _temp764.curr, _temp764.curr, _temp765.last_plus_one -
_temp765.curr, _temp765.curr);}); _temp763;});} if( ! Cyc_Interface_incl_ns( i2->imports,
i1->imports, msg)){ _temp758= 0;}{ int _temp766= _temp758; _npop_handler( 0u);
return _temp766;}}}; _pop_handler();} else{ void* _temp756=( void*) _exn_thrown;
void* _temp768= _temp756; _LL770: if( _temp768 == Cyc_Tcdecl_Incompatible){ goto
_LL771;} else{ goto _LL772;} _LL772: goto _LL773; _LL771: return 0; _LL773:(
void) _throw( _temp768); _LL769:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); struct
_tagged_string f6; struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f(
struct _tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15
_temp776; struct _tagged_string* _temp777; struct _tagged_string _temp779; void**(*
_temp781)( void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*);
struct Cyc_Dict_Dict* _temp783; struct Cyc_Dict_Dict* _temp785; struct Cyc_Dict_Dict*
_temp787; struct Cyc_Dict_Dict** _temp789; int _temp790; int* _temp792; struct
_tuple15* _temp774= env; _temp776=* _temp774; _LL791: _temp790= _temp776.f1;
_temp792=&(* _temp774).f1; goto _LL788; _LL788: _temp787= _temp776.f2; _temp789=&(*
_temp774).f2; goto _LL786; _LL786: _temp785= _temp776.f3; goto _LL784; _LL784:
_temp783= _temp776.f4; goto _LL782; _LL782: _temp781= _temp776.f5; goto _LL780;
_LL780: _temp779= _temp776.f6; goto _LL778; _LL778: _temp777= _temp776.f7; goto
_LL775; _LL775: { void** y;{ struct _handler_cons _temp793; _push_handler(&
_temp793);{ int _temp795= 0; if( setjmp( _temp793.handler)){ _temp795= 1;} if( !
_temp795){{ void** _temp796=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp785, x); goto _LL797; _LL797: {
void** _temp798= _temp781( _temp796, y2, 0, _temp777); goto _LL799; _LL799: if(
!(( unsigned int) _temp798)){* _temp792= 0; _npop_handler( 0u); return;} y=(
void**) _check_null( _temp798);}}; _pop_handler();} else{ void* _temp794=( void*)
_exn_thrown; void* _temp801= _temp794; _LL803: if( _temp801 == Cyc_Dict_Absent){
goto _LL804;} else{ goto _LL805;} _LL805: goto _LL806; _LL804: y= y2; goto
_LL802; _LL806:( void) _throw( _temp801); _LL802:;}}}{ struct _handler_cons
_temp807; _push_handler(& _temp807);{ int _temp809= 0; if( setjmp( _temp807.handler)){
_temp809= 1;} if( ! _temp809){{ void** _temp810=(( void**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp783, x); goto _LL811;
_LL811: { void** _temp812= _temp781( _temp810, y, 0, _temp777); goto _LL813;
_LL813: if( _temp812 !=( void**) _temp810){ if(( unsigned int) _temp812){ Cyc_Interface_check_err(
_temp777,( struct _tagged_string)({ struct _tagged_string _temp814= _temp779;
struct _tagged_string _temp815=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp814.last_plus_one - _temp814.curr, _temp814.curr, _temp815.last_plus_one -
_temp815.curr, _temp815.curr);}));}* _temp792= 0;}}}; _pop_handler();} else{
void* _temp808=( void*) _exn_thrown; void* _temp817= _temp808; _LL819: if(
_temp817 == Cyc_Dict_Absent){ goto _LL820;} else{ goto _LL821;} _LL821: goto
_LL822; _LL820: if(* _temp792){* _temp789=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(*
_temp789, x, y);} goto _LL818; _LL822:( void) _throw( _temp817); _LL818:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple15 _temp824=({
struct _tuple15 _temp823; _temp823.f1= 1; _temp823.f2= dic1; _temp823.f3= dic1;
_temp823.f4= excl; _temp823.f5= merge_f; _temp823.f6= t; _temp823.f7= msg;
_temp823;}); goto _LL825; _LL825:(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp824, dic2); if( _temp824.f1){ return({ struct Cyc_Core_Opt* _temp826=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp826->v=(
void*) _temp824.f2; _temp826;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp829=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->structdecls, ie2->structdecls, iexcl->structdecls,
Cyc_Tcdecl_merge_structdecl,({ unsigned char* _temp827=( unsigned char*)"struct";
struct _tagged_string _temp828; _temp828.curr= _temp827; _temp828.base= _temp827;
_temp828.last_plus_one= _temp827 + 7; _temp828;}), msg); goto _LL830; _LL830: {
struct Cyc_Core_Opt* _temp833=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(*
merge_f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls,
Cyc_Tcdecl_merge_uniondecl,({ unsigned char* _temp831=( unsigned char*)"union";
struct _tagged_string _temp832; _temp832.curr= _temp831; _temp832.base= _temp831;
_temp832.last_plus_one= _temp831 + 6; _temp832;}), msg); goto _LL834; _LL834: {
struct Cyc_Core_Opt* _temp837=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(*
merge_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls,
Cyc_Tcdecl_merge_tuniondecl,({ unsigned char* _temp835=( unsigned char*)"tunion";
struct _tagged_string _temp836; _temp836.curr= _temp835; _temp836.base= _temp835;
_temp836.last_plus_one= _temp835 + 7; _temp836;}), msg); goto _LL838; _LL838: {
struct Cyc_Core_Opt* _temp841=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(*
merge_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls,
Cyc_Tcdecl_merge_enumdecl,({ unsigned char* _temp839=( unsigned char*)"enum";
struct _tagged_string _temp840; _temp840.curr= _temp839; _temp840.base= _temp839;
_temp840.last_plus_one= _temp839 + 5; _temp840;}), msg); goto _LL842; _LL842: {
struct Cyc_Core_Opt* _temp845=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,
iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp843=(
unsigned char*)"typedef"; struct _tagged_string _temp844; _temp844.curr=
_temp843; _temp844.base= _temp843; _temp844.last_plus_one= _temp843 + 8;
_temp844;}), msg); goto _LL846; _LL846: { struct Cyc_Core_Opt* _temp849=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,({
unsigned char* _temp847=( unsigned char*)"variable"; struct _tagged_string
_temp848; _temp848.curr= _temp847; _temp848.base= _temp847; _temp848.last_plus_one=
_temp847 + 9; _temp848;}), msg); goto _LL850; _LL850: { struct Cyc_Core_Opt*
_temp853=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl,({
unsigned char* _temp851=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp852; _temp852.curr= _temp851; _temp852.base= _temp851; _temp852.last_plus_one=
_temp851 + 13; _temp852;}), msg); goto _LL854; _LL854: if(((((( !(( unsigned int)
_temp829)? 1: !(( unsigned int) _temp833))? 1: !(( unsigned int) _temp837))? 1:
!(( unsigned int) _temp841))? 1: !(( unsigned int) _temp845))? 1: !((
unsigned int) _temp849))? 1: !(( unsigned int) _temp853)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp855=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp855->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp829))->v; _temp855->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp833))->v;
_temp855->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp837))->v; _temp855->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp841))->v; _temp855->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp845))->v;
_temp855->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp849))->v; _temp855->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp853))->v; _temp855;});}}}}}}}} struct _tuple16{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; struct
_tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct _tuple16*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple16
_temp858; struct _tagged_string* _temp859; struct Cyc_Dict_Dict* _temp861;
struct Cyc_Dict_Dict* _temp863; struct Cyc_Dict_Dict* _temp865; struct Cyc_Dict_Dict**
_temp867; int _temp868; int* _temp870; struct _tuple16* _temp856= env; _temp858=*
_temp856; _LL869: _temp868= _temp858.f1; _temp870=&(* _temp856).f1; goto _LL866;
_LL866: _temp865= _temp858.f2; _temp867=&(* _temp856).f2; goto _LL864; _LL864:
_temp863= _temp858.f3; goto _LL862; _LL862: _temp861= _temp858.f4; goto _LL860;
_LL860: _temp859= _temp858.f5; goto _LL857; _LL857: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp871;
_push_handler(& _temp871);{ int _temp873= 0; if( setjmp( _temp871.handler)){
_temp873= 1;} if( ! _temp873){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp861, ns);;
_pop_handler();} else{ void* _temp872=( void*) _exn_thrown; void* _temp875=
_temp872; _LL877: if( _temp875 == Cyc_Dict_Absent){ goto _LL878;} else{ goto
_LL879;} _LL879: goto _LL880; _LL878: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL876; _LL880:( void) _throw( _temp875); _LL876:;}}}{ struct _handler_cons
_temp881; _push_handler(& _temp881);{ int _temp883= 0; if( setjmp( _temp881.handler)){
_temp883= 1;} if( ! _temp883){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp863, ns);{ struct Cyc_Interface_Ienv*
_temp884= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp859); goto _LL885; _LL885: if( _temp884 == 0){* _temp870= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*) _check_null(
_temp884);}; _pop_handler();} else{ void* _temp882=( void*) _exn_thrown; void*
_temp887= _temp882; _LL889: if( _temp887 == Cyc_Dict_Absent){ goto _LL890;}
else{ goto _LL891;} _LL891: goto _LL892; _LL890: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL888; _LL892:( void) _throw( _temp887); _LL888:;}}}{ struct Cyc_Interface_Ienv*
_temp893= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp859); goto
_LL894; _LL894: if( _temp893 == 0){* _temp870= 0;} else{ if(* _temp870){*
_temp867=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp867, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp893));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 _temp896=({ struct _tuple16
_temp895; _temp895.f1= 1; _temp895.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp895.f3= dic1; _temp895.f4= excl; _temp895.f5= msg; _temp895;}); goto _LL897;
_LL897:(( void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct
Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_ns_f,& _temp896, dic2); if( _temp896.f1){ return({
struct Cyc_Core_Opt* _temp898=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp898->v=( void*) _temp896.f2; _temp898;});} else{ return 0;}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp901; struct _tagged_string* _temp902; struct _tagged_string _temp904;
struct Cyc_Dict_Dict* _temp906; struct Cyc_Dict_Dict* _temp908; struct Cyc_Dict_Dict**
_temp910; int _temp911; int* _temp913; struct _tuple17* _temp899= env; _temp901=*
_temp899; _LL912: _temp911= _temp901.f1; _temp913=&(* _temp899).f1; goto _LL909;
_LL909: _temp908= _temp901.f2; _temp910=&(* _temp899).f2; goto _LL907; _LL907:
_temp906= _temp901.f3; goto _LL905; _LL905: _temp904= _temp901.f4; goto _LL903;
_LL903: _temp902= _temp901.f5; goto _LL900; _LL900: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp906, x)){ Cyc_Interface_check_err(
_temp902,( struct _tagged_string)({ struct _tagged_string _temp914= _temp904;
struct _tagged_string _temp915=* x; xprintf("%.*s %.*s is exported more than once",
_temp914.last_plus_one - _temp914.curr, _temp914.curr, _temp915.last_plus_one -
_temp915.curr, _temp915.curr);}));* _temp913= 0;} else{ if(* _temp913){*
_temp910=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void* data)) Cyc_Dict_insert)(* _temp910, x, y);}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg){
struct _tuple17 _temp917=({ struct _tuple17 _temp916; _temp916.f1= 1; _temp916.f2=
dic1; _temp916.f3= dic1; _temp916.f4= t; _temp916.f5= msg; _temp916;}); goto
_LL918; _LL918:(( void(*)( void(* f)( struct _tuple17*, struct _tagged_string*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple17* env, struct _tagged_string* x, void* y)) Cyc_Interface_disj_merge_dict_f,&
_temp917, dic2); if( _temp917.f1){ return({ struct Cyc_Core_Opt* _temp919=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp919->v=(
void*) _temp917.f2; _temp919;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_string* msg){ struct Cyc_Core_Opt* _temp922=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls,
ie2->structdecls,({ unsigned char* _temp920=( unsigned char*)"struct"; struct
_tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 7; _temp921;}), msg); goto _LL923; _LL923: {
struct Cyc_Core_Opt* _temp926=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls,({
unsigned char* _temp924=( unsigned char*)"union"; struct _tagged_string _temp925;
_temp925.curr= _temp924; _temp925.base= _temp924; _temp925.last_plus_one=
_temp924 + 6; _temp925;}), msg); goto _LL927; _LL927: { struct Cyc_Core_Opt*
_temp930=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls,({ unsigned char* _temp928=( unsigned char*)"[x]tunion";
struct _tagged_string _temp929; _temp929.curr= _temp928; _temp929.base= _temp928;
_temp929.last_plus_one= _temp928 + 10; _temp929;}), msg); goto _LL931; _LL931: {
struct Cyc_Core_Opt* _temp934=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,({
unsigned char* _temp932=( unsigned char*)"enum"; struct _tagged_string _temp933;
_temp933.curr= _temp932; _temp933.base= _temp932; _temp933.last_plus_one=
_temp932 + 5; _temp933;}), msg); goto _LL935; _LL935: { struct Cyc_Core_Opt*
_temp938=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl,({
unsigned char* _temp936=( unsigned char*)"typedef"; struct _tagged_string
_temp937; _temp937.curr= _temp936; _temp937.base= _temp936; _temp937.last_plus_one=
_temp936 + 8; _temp937;}), msg); goto _LL939; _LL939: { struct Cyc_Core_Opt*
_temp942=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->vardecls, ie2->vardecls,({ unsigned char* _temp940=( unsigned char*)"variable";
struct _tagged_string _temp941; _temp941.curr= _temp940; _temp941.base= _temp940;
_temp941.last_plus_one= _temp940 + 9; _temp941;}), msg); goto _LL943; _LL943: {
struct Cyc_Core_Opt* _temp946=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,({
unsigned char* _temp944=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp945; _temp945.curr= _temp944; _temp945.base= _temp944; _temp945.last_plus_one=
_temp944 + 13; _temp945;}), msg); goto _LL947; _LL947: if(((((( !(( unsigned int)
_temp922)? 1: !(( unsigned int) _temp926))? 1: !(( unsigned int) _temp930))? 1:
!(( unsigned int) _temp934))? 1: !(( unsigned int) _temp938))? 1: !((
unsigned int) _temp942))? 1: !(( unsigned int) _temp946)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp948=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp948->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp922))->v; _temp948->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp926))->v;
_temp948->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp930))->v; _temp948->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp934))->v; _temp948->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp938))->v;
_temp948->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp942))->v; _temp948->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp946))->v; _temp948;});}}}}}}}} struct _tuple18{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; } ; void
Cyc_Interface_disj_merge_ns_f( struct _tuple18* env, struct Cyc_List_List* ns,
struct Cyc_Interface_Ienv* ie2){ struct _tuple18 _temp951; struct _tagged_string*
_temp952; struct Cyc_Dict_Dict* _temp954; struct Cyc_Dict_Dict* _temp956; struct
Cyc_Dict_Dict** _temp958; int _temp959; int* _temp961; struct _tuple18* _temp949=
env; _temp951=* _temp949; _LL960: _temp959= _temp951.f1; _temp961=&(* _temp949).f1;
goto _LL957; _LL957: _temp956= _temp951.f2; _temp958=&(* _temp949).f2; goto
_LL955; _LL955: _temp954= _temp951.f3; goto _LL953; _LL953: _temp952= _temp951.f4;
goto _LL950; _LL950: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons
_temp962; _push_handler(& _temp962);{ int _temp964= 0; if( setjmp( _temp962.handler)){
_temp964= 1;} if( ! _temp964){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp954, ns);; _pop_handler();}
else{ void* _temp963=( void*) _exn_thrown; void* _temp966= _temp963; _LL968: if(
_temp966 == Cyc_Dict_Absent){ goto _LL969;} else{ goto _LL970;} _LL970: goto
_LL971; _LL969: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL967; _LL971:( void)
_throw( _temp966); _LL967:;}}}{ struct Cyc_Interface_Ienv* _temp972= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp952); goto _LL973; _LL973: if( _temp972 == 0){* _temp961= 0;}
else{ if(* _temp961){* _temp958=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp958, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp972));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple18 _temp975=({
struct _tuple18 _temp974; _temp974.f1= 1; _temp974.f2= dic1; _temp974.f3= dic1;
_temp974.f4= msg; _temp974;}); goto _LL976; _LL976:(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp975, dic2); if( _temp975.f1){ return({ struct Cyc_Core_Opt* _temp977=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp977->v=(
void*) _temp975.f2; _temp977;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp978; _push_handler(& _temp978);{ int _temp980= 0; if(
setjmp( _temp978.handler)){ _temp980= 1;} if( ! _temp980){{ struct
_tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string* _temp981=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp981[ 0]=(
struct _tagged_string)({ struct _tagged_string _temp982=(*(( struct _tuple3*)
_check_null( info))).f1; struct _tagged_string _temp983=(*(( struct _tuple3*)
_check_null( info))).f2; xprintf("merging exports of %.*s and %.*s,", _temp982.last_plus_one
- _temp982.curr, _temp982.curr, _temp983.last_plus_one - _temp983.curr, _temp983.curr);});
_temp981;});}{ struct Cyc_Core_Opt* _temp984= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL985; _LL985: if( _temp984 == 0){ struct Cyc_Interface_I*
_temp986= 0; _npop_handler( 0u); return _temp986;} if( info != 0){ msg=({ struct
_tagged_string* _temp987=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp987[ 0]=( struct _tagged_string)({ struct _tagged_string
_temp988=(*(( struct _tuple3*) _check_null( info))).f1; struct _tagged_string
_temp989=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp988.last_plus_one - _temp988.curr, _temp988.curr, _temp989.last_plus_one -
_temp989.curr, _temp989.curr);}); _temp987;});}{ struct Cyc_Core_Opt* _temp990=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp984))->v, msg); goto _LL991; _LL991: if(
_temp990 == 0){ struct Cyc_Interface_I* _temp992= 0; _npop_handler( 0u); return
_temp992;}{ struct Cyc_Interface_I* _temp994=({ struct Cyc_Interface_I* _temp993=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp993->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp990))->v;
_temp993->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp984))->v; _temp993;}); _npop_handler( 0u); return _temp994;}}}};
_pop_handler();} else{ void* _temp979=( void*) _exn_thrown; void* _temp996=
_temp979; _LL998: if( _temp996 == Cyc_Tcdecl_Incompatible){ goto _LL999;} else{
goto _LL1000;} _LL1000: goto _LL1001; _LL999: return 0; _LL1001:( void) _throw(
_temp996); _LL997:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_string* curr_info= linfo != 0?( struct _tagged_string*)(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=((
struct Cyc_List_List*) _check_null( li))->tl; if( linfo != 0){ linfo=(( struct
Cyc_List_List*) _check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp1003= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp1002=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1002->f1=*(( struct _tagged_string*)
_check_null( curr_info)); _temp1002->f2=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp1002;}): 0); goto _LL1004; _LL1004: if(
_temp1003 == 0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null(
_temp1003); if( curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_string*
_temp1005=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp1005[ 0]=( struct _tagged_string)({ struct _tagged_string _temp1006=*((
struct _tagged_string*) _check_null( curr_info)); struct _tagged_string
_temp1007=*(( struct _tagged_string*)(( struct Cyc_List_List*) _check_null(
linfo))->hd); xprintf("%.*s+%.*s", _temp1006.last_plus_one - _temp1006.curr,
_temp1006.curr, _temp1007.last_plus_one - _temp1007.curr, _temp1007.curr);});
_temp1005;}): 0;} if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null(
linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo){ if( la == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i= get((
void*)(( struct Cyc_List_List*) _check_null( la))->hd); struct _tagged_string*
curr_info= linfo != 0?( struct _tagged_string*)(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; la != 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl){ struct Cyc_Interface_I* _temp1009= Cyc_Interface_merge(
curr_i, get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd),( curr_info
!= 0? linfo != 0: 0)?({ struct _tuple3* _temp1008=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp1008->f1=*(( struct _tagged_string*) _check_null(
curr_info)); _temp1008->f2=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp1008;}): 0); goto _LL1010; _LL1010: if(
_temp1009 == 0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null(
_temp1009); if( curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_string*
_temp1011=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp1011[ 0]=( struct _tagged_string)({ struct _tagged_string _temp1012=*((
struct _tagged_string*) _check_null( curr_info)); struct _tagged_string
_temp1013=*(( struct _tagged_string*)(( struct Cyc_List_List*) _check_null(
linfo))->hd); xprintf("%.*s+%.*s", _temp1012.last_plus_one - _temp1012.curr,
_temp1012.curr, _temp1013.last_plus_one - _temp1013.curr, _temp1013.curr);});
_temp1011;}): 0;} if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null(
linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List*
Cyc_Interface_add_namespace( struct Cyc_List_List* tds, struct Cyc_List_List* ns){
if( ns == 0){ return tds;} return({ struct Cyc_List_List* _temp1014=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1014->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1015=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1015[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1016; _temp1016.tag= Cyc_Absyn_Namespace_d;
_temp1016.f1=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp1016.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp1016;}); _temp1015;}), 0); _temp1014->tl= 0;
_temp1014;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp1017=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1017->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp1018=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1018[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp1019; _temp1019.tag= Cyc_Absyn_Struct_d;
_temp1019.f1= d; _temp1019;}); _temp1018;}), 0); _temp1017->tl= tds; _temp1017;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp1020=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp1020[ 0]=* d; _temp1020;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp1021=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1021->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1022=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1022[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1023; _temp1023.tag= Cyc_Absyn_Struct_d;
_temp1023.f1= d; _temp1023;}); _temp1022;}), 0); _temp1021->tl= tds; _temp1021;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp1024=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1024->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp1025=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp1025[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp1026; _temp1026.tag= Cyc_Absyn_Union_d;
_temp1026.f1= d; _temp1026;}); _temp1025;}), 0); _temp1024->tl= tds; _temp1024;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp1027=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp1027[ 0]=* d; _temp1027;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp1028=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1028->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp1029=(
struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp1029[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp1030; _temp1030.tag= Cyc_Absyn_Union_d;
_temp1030.f1= d; _temp1030;}); _temp1029;}), 0); _temp1028->tl= tds; _temp1028;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp1031=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1031->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1032=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1032[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1033; _temp1033.tag= Cyc_Absyn_Tunion_d;
_temp1033.f1= d; _temp1033;}); _temp1032;}), 0); _temp1031->tl= tds; _temp1031;});}
static unsigned char _temp1036[ 2u]="_"; static struct _tagged_string Cyc_Interface_us={
_temp1036, _temp1036, _temp1036 + 2u}; static struct _tagged_string* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp1037=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1037->f1= Cyc_Absyn_empty_tqual();
_temp1037->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp1038=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1038[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1039; _temp1039.tag= Cyc_Absyn_VarType; _temp1039.f1=({
struct Cyc_Absyn_Tvar* _temp1040=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp1040->name= Cyc_Interface_us_p; _temp1040->identity=
0; _temp1040->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_BoxKind); _temp1040;}); _temp1039;}); _temp1038;}); _temp1037;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp1041=( struct
Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp1041[ 0]=* f; _temp1041;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple10*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List*
tds){ d=({ struct Cyc_Absyn_Tuniondecl* _temp1042=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1042[ 0]=* d; _temp1042;});
if( d->fields != 0){ d->fields=({ struct Cyc_Core_Opt* _temp1043=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1043->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp1043;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp1044=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1044->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp1045=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp1045[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp1046; _temp1046.tag= Cyc_Absyn_Tunion_d;
_temp1046.f1= d; _temp1046;}); _temp1045;}), 0); _temp1044->tl= tds; _temp1044;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1047=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1047->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp1048=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1048[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1049; _temp1049.tag= Cyc_Absyn_Enum_d;
_temp1049.f1= d; _temp1049;}); _temp1048;}), 0); _temp1047->tl= tds; _temp1047;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp1050=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp1050[ 0]=* d; _temp1050;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp1051=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1051->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp1052=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1052[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1053; _temp1053.tag= Cyc_Absyn_Enum_d;
_temp1053.f1= d; _temp1053;}); _temp1052;}), 0); _temp1051->tl= tds; _temp1051;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1054=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1054->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp1055=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp1055[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp1056; _temp1056.tag= Cyc_Absyn_Typedef_d;
_temp1056.f1= d; _temp1056;}); _temp1055;}), 0); _temp1054->tl= tds; _temp1054;});}
static struct Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1057=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1057->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1058=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1058[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1059; _temp1059.tag= Cyc_Absyn_Var_d;
_temp1059.f1= d; _temp1059;}); _temp1058;}), 0); _temp1057->tl= tds; _temp1057;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1062; struct Cyc_Absyn_Tunionfield*
_temp1063; struct Cyc_Absyn_Tuniondecl* _temp1065; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1060= d; _temp1062=* _temp1060; _LL1066: _temp1065= _temp1062.base; goto
_LL1064; _LL1064: _temp1063= _temp1062.field; goto _LL1061; _LL1061: _temp1065=({
struct Cyc_Absyn_Tuniondecl* _temp1067=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1067[ 0]=* _temp1065; _temp1067;});
_temp1065->fields=({ struct Cyc_Core_Opt* _temp1068=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1068->v=( void*)({ struct Cyc_List_List*
_temp1069=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1069->hd=( void*) _temp1063; _temp1069->tl= 0; _temp1069;}); _temp1068;});(
void*)( _temp1065->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1070=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1070->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1071=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1071[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1072; _temp1072.tag= Cyc_Absyn_Tunion_d;
_temp1072.f1= _temp1065; _temp1072;}); _temp1071;}), 0); _temp1070->tl= tds;
_temp1070;});} static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header(
struct _tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1075; struct Cyc_Absyn_Tunionfield*
_temp1076; struct Cyc_Absyn_Tuniondecl* _temp1078; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1073= d; _temp1075=* _temp1073; _LL1079: _temp1078= _temp1075.base; goto
_LL1077; _LL1077: _temp1076= _temp1075.field; goto _LL1074; _LL1074: _temp1078=({
struct Cyc_Absyn_Tuniondecl* _temp1080=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1080[ 0]=* _temp1078; _temp1080;});
_temp1076= Cyc_Interface_rewrite_tunionfield( _temp1076);( void*)( _temp1076->sc=(
void*)(( void*) Cyc_Absyn_Extern)); _temp1078->fields=({ struct Cyc_Core_Opt*
_temp1081=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1081->v=( void*)({ struct Cyc_List_List* _temp1082=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1082->hd=( void*) _temp1076;
_temp1082->tl= 0; _temp1082;}); _temp1081;});( void*)( _temp1078->sc=( void*)((
void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp1083=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1083->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp1084=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp1084[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp1085; _temp1085.tag= Cyc_Absyn_Tunion_d;
_temp1085.f1= _temp1078; _temp1085;}); _temp1084;}), 0); _temp1083->tl= tds;
_temp1083;});} static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp1086= 0; goto _LL1087; _LL1087: _temp1086=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp1086);
_temp1086=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp1086); _temp1086=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp1086); _temp1086=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp1086); if( _temp1086 != 0){ _temp1086=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1086); _temp1086= Cyc_Interface_add_namespace(
_temp1086, ns); Cyc_Absynpp_decllist2file( _temp1086, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1088= 0; goto _LL1089; _LL1089: _temp1088=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header,
ie->xtunionfielddecls, _temp1088); if( _temp1088 != 0){ _temp1088=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1088); _temp1088= Cyc_Interface_add_namespace(
_temp1088, ns); Cyc_Absynpp_decllist2file( _temp1088, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1090= 0; goto _LL1091; _LL1091: _temp1090=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp1090); if( _temp1090 != 0){ _temp1090=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1090); _temp1090= Cyc_Interface_add_namespace(
_temp1090, ns); Cyc_Absynpp_decllist2file( _temp1090, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1092= 0; goto _LL1093; _LL1093: _temp1092=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls,
_temp1092); _temp1092=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, _temp1092); _temp1092=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp1092); _temp1092=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp1092); _temp1092=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp1092); _temp1092=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp1092); if( _temp1092 != 0){ _temp1092=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1092); _temp1092= Cyc_Interface_add_namespace(
_temp1092, ns); Cyc_Absynpp_decllist2file( _temp1092, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Absynpp_set_params(&
Cyc_Absynpp_cyci_params_r); fprintf( sfile_to_file( f),"/****** needed (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports); fprintf( sfile_to_file( f),"\n/****** provided (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports); fprintf( sfile_to_file( f),"/****** needed (headers of xtunionfielddecls) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->imports); fprintf( sfile_to_file( f),"\n/****** provided (headers of xtunionfielddecls) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->exports); fprintf( sfile_to_file( f),"\n/****** provided (typedefs) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports); fprintf( sfile_to_file( f),"\n/****** needed (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->imports); fprintf( sfile_to_file( f),"\n/****** provided (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1094= Cyc_Parse_parse_file(
f); goto _LL1095; _LL1095: { struct Cyc_Tcenv_Tenv* _temp1096= Cyc_Tcenv_tc_init();
goto _LL1097; _LL1097: Cyc_Tc_tc( _temp1096, 0, _temp1094); return Cyc_Interface_gen_extract(
_temp1096->ae, 0);}}} void Cyc_Interface_save( struct Cyc_Interface_I* i, struct
Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i, f);} struct Cyc_Interface_I* Cyc_Interface_load(
struct Cyc_Stdio___sFILE* f){ return Cyc_Interface_parse( f);}