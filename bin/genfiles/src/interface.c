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
extern void Cyc_Interface_print( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
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
Cyc_Core_InvalidArg_struct _temp267; _temp267.tag= Cyc_Core_InvalidArg; _temp267.f1=({
unsigned char* _temp268=( unsigned char*)"check_public_type"; struct
_tagged_string _temp269; _temp269.curr= _temp268; _temp269.base= _temp268;
_temp269.last_plus_one= _temp268 + 18; _temp269;}); _temp267;}); _temp266;}));
_LL265:( void) _throw( _temp260); _LL261:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp207,({ unsigned char* _temp270=(
unsigned char*)"enum"; struct _tagged_string _temp271; _temp271.curr= _temp270;
_temp271.base= _temp270; _temp271.last_plus_one= _temp270 + 5; _temp271;}), ed->name);
return 0;} return 1;}}} _LL139: { struct _tuple0 _temp274; struct _tagged_string*
_temp275; void* _temp277; struct _tuple0* _temp272= _temp217->name; _temp274=*
_temp272; _LL278: _temp277= _temp274.f1; goto _LL276; _LL276: _temp275= _temp274.f2;
goto _LL273; _LL273: { struct Cyc_List_List* _temp279= Cyc_Interface_get_abs_ns(
_temp277); goto _LL280; _LL280: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp281; _push_handler(& _temp281);{ int _temp283= 0; if( setjmp(
_temp281.handler)){ _temp283= 1;} if( ! _temp283){{ struct Cyc_Tcenv_Genv*
_temp284=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp279); goto _LL285; _LL285: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp284->tuniondecls,
_temp275);}; _pop_handler();} else{ void* _temp282=( void*) _exn_thrown; void*
_temp287= _temp282; _LL289: if( _temp287 == Cyc_Dict_Absent){ goto _LL290;}
else{ goto _LL291;} _LL291: goto _LL292; _LL290:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp293=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp293[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp294; _temp294.tag= Cyc_Core_InvalidArg; _temp294.f1=({
unsigned char* _temp295=( unsigned char*)"check_public_type"; struct
_tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 18; _temp296;}); _temp294;}); _temp293;}));
_LL292:( void) _throw( _temp287); _LL288:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ unsigned char* _temp297=( unsigned char*)"[x]tunion";
struct _tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 10; _temp298;}), tud->name); return 0;}
return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,( void*(*)( void*))
Cyc_Core_identity, _temp213)? Cyc_Interface_check_public_tuniondecl( ae, seen,
tud): 0;}}} _LL141: { struct _tuple0 _temp301; struct _tagged_string* _temp302;
void* _temp304; struct _tuple0* _temp299= _temp227->name; _temp301=* _temp299;
_LL305: _temp304= _temp301.f1; goto _LL303; _LL303: _temp302= _temp301.f2; goto
_LL300; _LL300: { struct Cyc_List_List* _temp306= Cyc_Interface_get_abs_ns(
_temp304); goto _LL307; _LL307: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp308; _push_handler(& _temp308);{ int _temp310= 0; if( setjmp(
_temp308.handler)){ _temp310= 1;} if( ! _temp310){{ struct Cyc_Tcenv_Genv*
_temp311=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp306); goto _LL312; _LL312: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp311->tuniondecls,
_temp302);}; _pop_handler();} else{ void* _temp309=( void*) _exn_thrown; void*
_temp314= _temp309; _LL316: if( _temp314 == Cyc_Dict_Absent){ goto _LL317;}
else{ goto _LL318;} _LL318: goto _LL319; _LL317:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp320=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp320[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp321; _temp321.tag= Cyc_Core_InvalidArg; _temp321.f1=(
struct _tagged_string)({ struct _tagged_string _temp322= Cyc_Absynpp_qvar2string(
_temp227->name); xprintf("check_public_type (can't find tunion %.*s)", _temp322.last_plus_one
- _temp322.curr, _temp322.curr);}); _temp321;}); _temp320;})); _LL319:( void)
_throw( _temp314); _LL315:;}}} if( tud->fields == 0){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp323=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp323[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp324; _temp324.tag= Cyc_Core_InvalidArg; _temp324.f1=(
struct _tagged_string)({ struct _tagged_string _temp325= Cyc_Absynpp_qvar2string(
tud->name); xprintf("check_public_type (tunion %.*s has no fields)", _temp325.last_plus_one
- _temp325.curr, _temp325.curr);}); _temp324;}); _temp323;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp326=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; goto _LL327; _LL327: for( 0; _temp326 != 0;
_temp326=(( struct Cyc_List_List*) _check_null( _temp326))->tl){ if( Cyc_String_strptrcmp((*
_temp225->name).f2,(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp326))->hd)->name).f2) == 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp326))->hd);
break;}}} if( tuf1 == 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp328=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp328[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp329; _temp329.tag= Cyc_Core_InvalidArg;
_temp329.f1=( struct _tagged_string)({ struct _tagged_string _temp330= Cyc_Absynpp_qvar2string(
_temp225->name); xprintf("check_public_type (can't find tunionfield %.*s)",
_temp330.last_plus_one - _temp330.curr, _temp330.curr);}); _temp329;}); _temp328;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*) _check_null(
tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,({ unsigned char* _temp331=( unsigned char*)"[x]tunion"; struct
_tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 10; _temp332;}), tud->name); return 0;} if((
void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err( obj,
name,({ unsigned char* _temp333=( unsigned char*)"[x]tunion"; struct
_tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 10; _temp334;}), tud->name); return 0;} if((
void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,(
struct _tagged_string)({ struct _tagged_string _temp335= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _temp335.last_plus_one - _temp335.curr,
_temp335.curr);}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp221)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}}} _LL143: return 1; _LL121:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp338; struct Cyc_Interface_I* _temp339; struct Cyc_Interface_Seen*
_temp341; struct Cyc_Dict_Dict* _temp343; int _temp345; struct Cyc_Interface_Ienv*
_temp347; struct Cyc_Interface_Ienv* _temp349; struct _tuple11* _temp336= env;
_temp338=* _temp336; _LL350: _temp349= _temp338.f1; goto _LL348; _LL348:
_temp347= _temp338.f2; goto _LL346; _LL346: _temp345= _temp338.f3; goto _LL344;
_LL344: _temp343= _temp338.f4; goto _LL342; _LL342: _temp341= _temp338.f5; goto
_LL340; _LL340: _temp339= _temp338.f6; goto _LL337; _LL337: { struct Cyc_Absyn_Structdecl*
_temp351=* dp; goto _LL352; _LL352: { void* _temp353=( void*) _temp351->sc;
_LL355: if( _temp353 ==( void*) Cyc_Absyn_Static){ goto _LL356;} else{ goto
_LL357;} _LL357: if( _temp353 ==( void*) Cyc_Absyn_Abstract){ goto _LL358;}
else{ goto _LL359;} _LL359: if( _temp353 ==( void*) Cyc_Absyn_Public){ goto
_LL360;} else{ goto _LL361;} _LL361: if( _temp353 ==( void*) Cyc_Absyn_ExternC){
goto _LL362;} else{ goto _LL363;} _LL363: if( _temp353 ==( void*) Cyc_Absyn_Extern){
goto _LL364;} else{ goto _LL354;} _LL356: if( _temp345? _temp351->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp365=( unsigned char*)"static";
struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 7; _temp366;}),({ unsigned char* _temp367=(
unsigned char*)"struct"; struct _tagged_string _temp368; _temp368.curr= _temp367;
_temp368.base= _temp367; _temp368.last_plus_one= _temp367 + 7; _temp368;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp351->name))->v);}
goto _LL354; _LL358: if( _temp351->fields == 0){ if( _temp345){ Cyc_Interface_fields_err(({
unsigned char* _temp369=( unsigned char*)"abstract"; struct _tagged_string
_temp370; _temp370.curr= _temp369; _temp370.base= _temp369; _temp370.last_plus_one=
_temp369 + 9; _temp370;}),({ unsigned char* _temp371=( unsigned char*)"struct";
struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 7; _temp372;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp351->name))->v);}} else{ _temp351=({ struct Cyc_Absyn_Structdecl*
_temp373=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp373[ 0]=* _temp351; _temp373;}); _temp351->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp343, _temp341, _temp351)){ _temp347->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp347->structdecls, x, _temp351);} goto _LL354;
_LL360: if( _temp351->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp374=( unsigned char*)"public"; struct _tagged_string _temp375; _temp375.curr=
_temp374; _temp375.base= _temp374; _temp375.last_plus_one= _temp374 + 7;
_temp375;}),({ unsigned char* _temp376=( unsigned char*)"struct"; struct
_tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 7; _temp377;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp351->name))->v); _temp351=({ struct Cyc_Absyn_Structdecl*
_temp378=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp378[ 0]=* _temp351; _temp378;});( void*)( _temp351->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl( _temp343,
_temp341, _temp351)){ _temp347->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp347->structdecls, x, _temp351);} goto _LL354; _LL362:
goto _LL364; _LL364: if( Cyc_Interface_check_public_structdecl( _temp343,
_temp341, _temp351)){ _temp349->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp349->structdecls, x, _temp351);} goto _LL354; _LL354:;}}}
static void Cyc_Interface_extract_uniondecl( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11 _temp381;
struct Cyc_Interface_I* _temp382; struct Cyc_Interface_Seen* _temp384; struct
Cyc_Dict_Dict* _temp386; int _temp388; struct Cyc_Interface_Ienv* _temp390;
struct Cyc_Interface_Ienv* _temp392; struct _tuple11* _temp379= env; _temp381=*
_temp379; _LL393: _temp392= _temp381.f1; goto _LL391; _LL391: _temp390= _temp381.f2;
goto _LL389; _LL389: _temp388= _temp381.f3; goto _LL387; _LL387: _temp386=
_temp381.f4; goto _LL385; _LL385: _temp384= _temp381.f5; goto _LL383; _LL383:
_temp382= _temp381.f6; goto _LL380; _LL380: { struct Cyc_Absyn_Uniondecl*
_temp394=* dp; goto _LL395; _LL395: { void* _temp396=( void*) _temp394->sc;
_LL398: if( _temp396 ==( void*) Cyc_Absyn_Static){ goto _LL399;} else{ goto
_LL400;} _LL400: if( _temp396 ==( void*) Cyc_Absyn_Abstract){ goto _LL401;}
else{ goto _LL402;} _LL402: if( _temp396 ==( void*) Cyc_Absyn_Public){ goto
_LL403;} else{ goto _LL404;} _LL404: if( _temp396 ==( void*) Cyc_Absyn_ExternC){
goto _LL405;} else{ goto _LL406;} _LL406: if( _temp396 ==( void*) Cyc_Absyn_Extern){
goto _LL407;} else{ goto _LL397;} _LL399: if( _temp388? _temp394->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp408=( unsigned char*)"static";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 7; _temp409;}),({ unsigned char* _temp410=(
unsigned char*)"union"; struct _tagged_string _temp411; _temp411.curr= _temp410;
_temp411.base= _temp410; _temp411.last_plus_one= _temp410 + 6; _temp411;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp394->name))->v);}
goto _LL397; _LL401: if( _temp394->fields == 0){ if( _temp388){ Cyc_Interface_fields_err(({
unsigned char* _temp412=( unsigned char*)"abstract"; struct _tagged_string
_temp413; _temp413.curr= _temp412; _temp413.base= _temp412; _temp413.last_plus_one=
_temp412 + 9; _temp413;}),({ unsigned char* _temp414=( unsigned char*)"union";
struct _tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 6; _temp415;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp394->name))->v);}} else{ _temp394=({ struct Cyc_Absyn_Uniondecl*
_temp416=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp416[ 0]=* _temp394; _temp416;}); _temp394->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp386, _temp384, _temp394)){ _temp390->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp390->uniondecls, x, _temp394);} goto _LL397;
_LL403: if( _temp394->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp417=( unsigned char*)"public"; struct _tagged_string _temp418; _temp418.curr=
_temp417; _temp418.base= _temp417; _temp418.last_plus_one= _temp417 + 7;
_temp418;}),({ unsigned char* _temp419=( unsigned char*)"union"; struct
_tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 6; _temp420;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp394->name))->v); _temp394=({ struct Cyc_Absyn_Uniondecl*
_temp421=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp421[ 0]=* _temp394; _temp421;});( void*)( _temp394->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl( _temp386,
_temp384, _temp394)){ _temp390->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data))
Cyc_Dict_insert)( _temp390->uniondecls, x, _temp394);} goto _LL397; _LL405: goto
_LL407; _LL407: if( Cyc_Interface_check_public_uniondecl( _temp386, _temp384,
_temp394)){ _temp392->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp392->uniondecls, x, _temp394);} goto _LL397; _LL397:;}}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp424; struct Cyc_Interface_I* _temp425; struct Cyc_Interface_Seen*
_temp427; struct Cyc_Dict_Dict* _temp429; int _temp431; struct Cyc_Interface_Ienv*
_temp433; struct Cyc_Interface_Ienv* _temp435; struct _tuple11* _temp422= env;
_temp424=* _temp422; _LL436: _temp435= _temp424.f1; goto _LL434; _LL434:
_temp433= _temp424.f2; goto _LL432; _LL432: _temp431= _temp424.f3; goto _LL430;
_LL430: _temp429= _temp424.f4; goto _LL428; _LL428: _temp427= _temp424.f5; goto
_LL426; _LL426: _temp425= _temp424.f6; goto _LL423; _LL423: { struct Cyc_Absyn_Enumdecl*
_temp437=* dp; goto _LL438; _LL438: { void* _temp439=( void*) _temp437->sc;
_LL441: if( _temp439 ==( void*) Cyc_Absyn_Static){ goto _LL442;} else{ goto
_LL443;} _LL443: if( _temp439 ==( void*) Cyc_Absyn_Abstract){ goto _LL444;}
else{ goto _LL445;} _LL445: if( _temp439 ==( void*) Cyc_Absyn_Public){ goto
_LL446;} else{ goto _LL447;} _LL447: if( _temp439 ==( void*) Cyc_Absyn_ExternC){
goto _LL448;} else{ goto _LL449;} _LL449: if( _temp439 ==( void*) Cyc_Absyn_Extern){
goto _LL450;} else{ goto _LL440;} _LL442: if( _temp431? _temp437->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp451=( unsigned char*)"static";
struct _tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 7; _temp452;}),({ unsigned char* _temp453=(
unsigned char*)"enum"; struct _tagged_string _temp454; _temp454.curr= _temp453;
_temp454.base= _temp453; _temp454.last_plus_one= _temp453 + 5; _temp454;}),
_temp437->name);} goto _LL440; _LL444: if( _temp437->fields == 0){ if( _temp431){
Cyc_Interface_fields_err(({ unsigned char* _temp455=( unsigned char*)"abstract";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 9; _temp456;}),({ unsigned char* _temp457=(
unsigned char*)"enum"; struct _tagged_string _temp458; _temp458.curr= _temp457;
_temp458.base= _temp457; _temp458.last_plus_one= _temp457 + 5; _temp458;}),
_temp437->name);}} else{ _temp437=({ struct Cyc_Absyn_Enumdecl* _temp459=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp459[ 0]=* _temp437; _temp459;}); _temp437->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp429, _temp427, _temp437)){ _temp433->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp433->enumdecls, x, _temp437);} goto _LL440; _LL446:
if( _temp437->fields == 0){ Cyc_Interface_fields_err(({ unsigned char* _temp460=(
unsigned char*)"public"; struct _tagged_string _temp461; _temp461.curr= _temp460;
_temp461.base= _temp460; _temp461.last_plus_one= _temp460 + 7; _temp461;}),({
unsigned char* _temp462=( unsigned char*)"enum"; struct _tagged_string _temp463;
_temp463.curr= _temp462; _temp463.base= _temp462; _temp463.last_plus_one=
_temp462 + 5; _temp463;}), _temp437->name); _temp437=({ struct Cyc_Absyn_Enumdecl*
_temp464=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp464[ 0]=* _temp437; _temp464;});( void*)( _temp437->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl( _temp429,
_temp427, _temp437)){ _temp433->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp433->enumdecls, x, _temp437);} goto _LL440; _LL448: goto
_LL450; _LL450: if( Cyc_Interface_check_public_enumdecl( _temp429, _temp427,
_temp437)){ _temp435->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(
_temp435->enumdecls, x, _temp437);} goto _LL440; _LL440:;}}} static void Cyc_Interface_extract_xtunionfielddecl(
struct Cyc_Interface_I* i, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield*
f){ struct Cyc_List_List* _temp465= 0; goto _LL466; _LL466:{ void* _temp467=(* f->name).f1;
struct Cyc_List_List* _temp475; struct Cyc_List_List* _temp477; _LL469: if((
unsigned int) _temp467 > 1u?*(( int*) _temp467) == Cyc_Absyn_Rel_n: 0){ _LL476:
_temp475=(( struct Cyc_Absyn_Rel_n_struct*) _temp467)->f1; goto _LL470;} else{
goto _LL471;} _LL471: if(( unsigned int) _temp467 > 1u?*(( int*) _temp467) ==
Cyc_Absyn_Abs_n: 0){ _LL478: _temp477=(( struct Cyc_Absyn_Abs_n_struct*)
_temp467)->f1; goto _LL472;} else{ goto _LL473;} _LL473: goto _LL474; _LL470:
_temp477= _temp475; goto _LL472; _LL472: _temp465= _temp477; goto _LL468; _LL474:
goto _LL468; _LL468:;}{ struct Cyc_Dict_Dict** dict;{ void* _temp479=( void*) f->sc;
_LL481: if( _temp479 ==( void*) Cyc_Absyn_Static){ goto _LL482;} else{ goto
_LL483;} _LL483: if( _temp479 ==( void*) Cyc_Absyn_Extern){ goto _LL484;} else{
goto _LL485;} _LL485: if( _temp479 ==( void*) Cyc_Absyn_Public){ goto _LL486;}
else{ goto _LL487;} _LL487: goto _LL488; _LL482: return; _LL484: dict=& i->imports;
goto _LL480; _LL486: dict=& i->exports; goto _LL480; _LL488:( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp489=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp489[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp490; _temp490.tag= Cyc_Core_InvalidArg; _temp490.f1=({
unsigned char* _temp491=( unsigned char*)"add_xtunionfielddecl"; struct
_tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 21; _temp492;}); _temp490;}); _temp489;}));
_LL480:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons _temp493;
_push_handler(& _temp493);{ int _temp495= 0; if( setjmp( _temp493.handler)){
_temp495= 1;} if( ! _temp495){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp465);; _pop_handler();}
else{ void* _temp494=( void*) _exn_thrown; void* _temp497= _temp494; _LL499: if(
_temp497 == Cyc_Dict_Absent){ goto _LL500;} else{ goto _LL501;} _LL501: goto
_LL502; _LL500: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp465, env); goto _LL498; _LL502:( void)
_throw( _temp497); _LL498:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp503=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp503->base= d; _temp503->field= f; _temp503;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp506; struct Cyc_Interface_I* _temp507; struct Cyc_Interface_Seen*
_temp509; struct Cyc_Dict_Dict* _temp511; int _temp513; struct Cyc_Interface_Ienv*
_temp515; struct Cyc_Interface_Ienv* _temp517; struct _tuple11* _temp504= env;
_temp506=* _temp504; _LL518: _temp517= _temp506.f1; goto _LL516; _LL516:
_temp515= _temp506.f2; goto _LL514; _LL514: _temp513= _temp506.f3; goto _LL512;
_LL512: _temp511= _temp506.f4; goto _LL510; _LL510: _temp509= _temp506.f5; goto
_LL508; _LL508: _temp507= _temp506.f6; goto _LL505; _LL505: { struct Cyc_Absyn_Tuniondecl*
_temp519=* dp; goto _LL520; _LL520: { void* _temp521=( void*) _temp519->sc;
_LL523: if( _temp521 ==( void*) Cyc_Absyn_Static){ goto _LL524;} else{ goto
_LL525;} _LL525: if( _temp521 ==( void*) Cyc_Absyn_Abstract){ goto _LL526;}
else{ goto _LL527;} _LL527: if( _temp521 ==( void*) Cyc_Absyn_Public){ goto
_LL528;} else{ goto _LL529;} _LL529: if( _temp521 ==( void*) Cyc_Absyn_ExternC){
goto _LL530;} else{ goto _LL531;} _LL531: if( _temp521 ==( void*) Cyc_Absyn_Extern){
goto _LL532;} else{ goto _LL522;} _LL524: if(( ! _temp519->is_xtunion? _temp513:
0)? _temp519->fields == 0: 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp533=( unsigned char*)"static"; struct _tagged_string _temp534; _temp534.curr=
_temp533; _temp534.base= _temp533; _temp534.last_plus_one= _temp533 + 7;
_temp534;}),({ unsigned char* _temp535=( unsigned char*)"tunion"; struct
_tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 7; _temp536;}), _temp519->name);} goto _LL522;
_LL526: if( _temp519->fields == 0){ if( ! _temp519->is_xtunion? _temp513: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp537=( unsigned char*)"abstract";
struct _tagged_string _temp538; _temp538.curr= _temp537; _temp538.base= _temp537;
_temp538.last_plus_one= _temp537 + 9; _temp538;}),({ unsigned char* _temp539=(
unsigned char*)"tunion"; struct _tagged_string _temp540; _temp540.curr= _temp539;
_temp540.base= _temp539; _temp540.last_plus_one= _temp539 + 7; _temp540;}),
_temp519->name);}} else{ _temp519=({ struct Cyc_Absyn_Tuniondecl* _temp541=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp541[ 0]=* _temp519; _temp541;}); _temp519->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp511, _temp509, _temp519)){ _temp515->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp515->tuniondecls, x, _temp519);} goto _LL522;
_LL528: _temp519=({ struct Cyc_Absyn_Tuniondecl* _temp542=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp542[ 0]=* _temp519;
_temp542;}); if( ! _temp519->is_xtunion? _temp519->fields == 0: 0){ Cyc_Interface_fields_err(({
unsigned char* _temp543=( unsigned char*)"public"; struct _tagged_string
_temp544; _temp544.curr= _temp543; _temp544.base= _temp543; _temp544.last_plus_one=
_temp543 + 7; _temp544;}),({ unsigned char* _temp545=( unsigned char*)"tunion";
struct _tagged_string _temp546; _temp546.curr= _temp545; _temp546.base= _temp545;
_temp546.last_plus_one= _temp545 + 7; _temp546;}), _temp519->name);( void*)(
_temp519->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp511, _temp509, _temp519)){ if( _temp519->is_xtunion? _temp519->fields != 0:
0){ struct Cyc_List_List* _temp547=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp519->fields))->v; goto _LL548; _LL548: _temp519->fields= 0;{
struct Cyc_List_List* _temp549= _temp547; goto _LL550; _LL550: for( 0; _temp549
!= 0; _temp549=(( struct Cyc_List_List*) _check_null( _temp549))->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp507, _temp519,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp549))->hd);}}} _temp515->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp515->tuniondecls, x, _temp519);} goto _LL522;
_LL530:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp551=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp551[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp552; _temp552.tag= Cyc_Core_InvalidArg;
_temp552.f1=({ unsigned char* _temp553=( unsigned char*)"extract_tuniondecl";
struct _tagged_string _temp554; _temp554.curr= _temp553; _temp554.base= _temp553;
_temp554.last_plus_one= _temp553 + 19; _temp554;}); _temp552;}); _temp551;}));
_LL532: if( Cyc_Interface_check_public_tuniondecl( _temp511, _temp509, _temp519)){
if( _temp519->is_xtunion? _temp519->fields != 0: 0){ _temp519=({ struct Cyc_Absyn_Tuniondecl*
_temp555=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp555[ 0]=* _temp519; _temp555;});{ struct Cyc_List_List* _temp556=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp519->fields))->v; goto
_LL557; _LL557: _temp519->fields= 0;{ struct Cyc_List_List* _temp558= _temp556;
goto _LL559; _LL559: for( 0; _temp558 != 0; _temp558=(( struct Cyc_List_List*)
_check_null( _temp558))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp507,
_temp519,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp558))->hd);}}}} _temp517->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp517->tuniondecls, x, _temp519);} goto _LL522; _LL522:;}}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp562;
struct Cyc_Interface_I* _temp563; struct Cyc_Interface_Seen* _temp565; struct
Cyc_Dict_Dict* _temp567; int _temp569; struct Cyc_Interface_Ienv* _temp571;
struct Cyc_Interface_Ienv* _temp573; struct _tuple11* _temp560= env; _temp562=*
_temp560; _LL574: _temp573= _temp562.f1; goto _LL572; _LL572: _temp571= _temp562.f2;
goto _LL570; _LL570: _temp569= _temp562.f3; goto _LL568; _LL568: _temp567=
_temp562.f4; goto _LL566; _LL566: _temp565= _temp562.f5; goto _LL564; _LL564:
_temp563= _temp562.f6; goto _LL561; _LL561: if( Cyc_Interface_check_public_typedefdecl(
_temp567, _temp565, d)){ _temp571->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp571->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_string* x, struct _tuple7* v){ struct
_tuple11 _temp577; struct Cyc_Interface_I* _temp578; struct Cyc_Interface_Seen*
_temp580; struct Cyc_Dict_Dict* _temp582; int _temp584; struct Cyc_Interface_Ienv*
_temp586; struct Cyc_Interface_Ienv* _temp588; struct _tuple11* _temp575= env;
_temp577=* _temp575; _LL589: _temp588= _temp577.f1; goto _LL587; _LL587:
_temp586= _temp577.f2; goto _LL585; _LL585: _temp584= _temp577.f3; goto _LL583;
_LL583: _temp582= _temp577.f4; goto _LL581; _LL581: _temp580= _temp577.f5; goto
_LL579; _LL579: _temp578= _temp577.f6; goto _LL576; _LL576: { void* _temp590=(*
v).f1; goto _LL591; _LL591: { void* _temp592= _temp590; void* _temp598; _LL594:
if(*(( int*) _temp592) == Cyc_Tcenv_VarRes){ _LL599: _temp598=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp592)->f1; goto _LL595;} else{ goto _LL596;}
_LL596: goto _LL597; _LL595:{ void* _temp600= _temp598; struct Cyc_Absyn_Fndecl*
_temp608; struct Cyc_Absyn_Vardecl* _temp610; _LL602: if(( unsigned int)
_temp600 > 1u?*(( int*) _temp600) == Cyc_Absyn_Funname_b: 0){ _LL609: _temp608=((
struct Cyc_Absyn_Funname_b_struct*) _temp600)->f1; goto _LL603;} else{ goto
_LL604;} _LL604: if(( unsigned int) _temp600 > 1u?*(( int*) _temp600) == Cyc_Absyn_Global_b:
0){ _LL611: _temp610=(( struct Cyc_Absyn_Global_b_struct*) _temp600)->f1; goto
_LL605;} else{ goto _LL606;} _LL606: goto _LL607; _LL603: { struct Cyc_Absyn_Vardecl*
_temp614=({ struct Cyc_Absyn_Vardecl* _temp612=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp612->sc=( void*)(( void*)
_temp608->sc); _temp612->name= _temp608->name; _temp612->tq=({ struct Cyc_Absyn_Tqual
_temp613; _temp613.q_const= 0; _temp613.q_volatile= 0; _temp613.q_restrict= 0;
_temp613;}); _temp612->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp608->cached_typ))->v); _temp612->initializer= 0; _temp612->rgn=
0; _temp612->attributes= 0; _temp612;}); goto _LL615; _LL615: _temp584= 0;
_temp610= _temp614; goto _LL605;} _LL605: if( _temp610->initializer != 0){
_temp610=({ struct Cyc_Absyn_Vardecl* _temp616=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp616[ 0]=* _temp610; _temp616;});
_temp610->initializer= 0;}{ void* _temp617=( void*) _temp610->sc; _LL619: if(
_temp617 ==( void*) Cyc_Absyn_Static){ goto _LL620;} else{ goto _LL621;} _LL621:
if( _temp617 ==( void*) Cyc_Absyn_Abstract){ goto _LL622;} else{ goto _LL623;}
_LL623: if( _temp617 ==( void*) Cyc_Absyn_Public){ goto _LL624;} else{ goto
_LL625;} _LL625: if( _temp617 ==( void*) Cyc_Absyn_ExternC){ goto _LL626;} else{
goto _LL627;} _LL627: if( _temp617 ==( void*) Cyc_Absyn_Extern){ goto _LL628;}
else{ goto _LL618;} _LL620: if( _temp584? Cyc_Tcutil_is_function_type(( void*)
_temp610->type): 0){ Cyc_Interface_body_err(({ unsigned char* _temp629=(
unsigned char*)"static"; struct _tagged_string _temp630; _temp630.curr= _temp629;
_temp630.base= _temp629; _temp630.last_plus_one= _temp629 + 7; _temp630;}),
_temp610->name);} goto _LL618; _LL622:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp631=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp631[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp632; _temp632.tag= Cyc_Core_InvalidArg;
_temp632.f1=({ unsigned char* _temp633=( unsigned char*)"extract_ordinarie";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 18; _temp634;}); _temp632;}); _temp631;}));
_LL624: if( _temp584? Cyc_Tcutil_is_function_type(( void*) _temp610->type): 0){
Cyc_Interface_body_err(({ unsigned char* _temp635=( unsigned char*)"public";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 7; _temp636;}), _temp610->name);} if( Cyc_Interface_check_public_vardecl(
_temp582, _temp580, _temp610)){ _temp586->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( _temp586->vardecls, x, _temp610);} goto _LL618; _LL626:
goto _LL628; _LL628: if( Cyc_Interface_check_public_vardecl( _temp582, _temp580,
_temp610)){ _temp588->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp588->vardecls, x, _temp610);} goto _LL618; _LL618:;} goto _LL601; _LL607:
goto _LL601; _LL601:;} goto _LL593; _LL597: goto _LL593; _LL593:;}}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp639; struct Cyc_Interface_Seen* _temp640; struct Cyc_Dict_Dict* _temp642;
int _temp644; struct Cyc_Interface_I* _temp646; struct _tuple12* _temp637= env_f;
_temp639=* _temp637; _LL647: _temp646= _temp639.f1; goto _LL645; _LL645:
_temp644= _temp639.f2; goto _LL643; _LL643: _temp642= _temp639.f3; goto _LL641;
_LL641: _temp640= _temp639.f4; goto _LL638; _LL638: { struct _tuple11 _temp649=({
struct _tuple11 _temp648; _temp648.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp646->imports,
ns); _temp648.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp646->exports, ns); _temp648.f3=
_temp644; _temp648.f4= _temp642; _temp648.f5= _temp640; _temp648.f6= _temp646;
_temp648;}); goto _LL650; _LL650:(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& _temp649,
ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct _tagged_string*,
struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp649, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp649, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp649, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp649, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp649, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp652=({ struct _tuple12
_temp651; _temp651.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp651.f2= check_complete_defs; _temp651.f3= ae;
_temp651.f4= Cyc_Interface_new_seen(); _temp651;}); goto _LL653; _LL653:(( void(*)(
void(* f)( struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*),
struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp652, ae); return _temp652.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_string* msg1, struct
_tagged_string msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_string*);
struct _tagged_string f4; struct _tagged_string* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_string* x, void* y1){ struct _tuple13
_temp656; struct _tagged_string* _temp657; struct _tagged_string _temp659; int(*
_temp661)( void*, void*, struct _tagged_string*); struct Cyc_Dict_Dict* _temp663;
int _temp665; int* _temp667; struct _tuple13* _temp654= env; _temp656=* _temp654;
_LL666: _temp665= _temp656.f1; _temp667=&(* _temp654).f1; goto _LL664; _LL664:
_temp663= _temp656.f2; goto _LL662; _LL662: _temp661= _temp656.f3; goto _LL660;
_LL660: _temp659= _temp656.f4; goto _LL658; _LL658: _temp657= _temp656.f5; goto
_LL655; _LL655: { struct _handler_cons _temp668; _push_handler(& _temp668);{ int
_temp670= 0; if( setjmp( _temp668.handler)){ _temp670= 1;} if( ! _temp670){{
void* _temp671=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp663, x); goto _LL672; _LL672: if( ! _temp661( y1,
_temp671, _temp657)){* _temp667= 0;}}; _pop_handler();} else{ void* _temp669=(
void*) _exn_thrown; void* _temp674= _temp669; _LL676: if( _temp674 == Cyc_Dict_Absent){
goto _LL677;} else{ goto _LL678;} _LL678: goto _LL679; _LL677: Cyc_Interface_check_err(
_temp657,( struct _tagged_string)({ struct _tagged_string _temp680= _temp659;
struct _tagged_string _temp681=* x; xprintf("%.*s %.*s is missing", _temp680.last_plus_one
- _temp680.curr, _temp680.curr, _temp681.last_plus_one - _temp681.curr, _temp681.curr);}));*
_temp667= 0; goto _LL675; _LL679:( void) _throw( _temp674); _LL675:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple13 _temp683=({
struct _tuple13 _temp682; _temp682.f1= 1; _temp682.f2= dic2; _temp682.f3= incl_f;
_temp682.f4= t; _temp682.f5= msg; _temp682;}); goto _LL684; _LL684:(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_string*, void*), struct _tuple13*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& _temp683, dic1);
return _temp683.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Structdecl*
_temp685= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); goto _LL686; _LL686: if(
_temp685 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp685){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp687= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp687.last_plus_one - _temp687.curr, _temp687.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct
Cyc_Absyn_Uniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Uniondecl*
_temp688= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); goto _LL689; _LL689: if(
_temp688 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 != _temp688){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp690= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp690.last_plus_one - _temp690.curr, _temp690.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Tuniondecl*
_temp691= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); goto _LL692; _LL692: if(
_temp691 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 != _temp691){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp693= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp693.last_plus_one - _temp693.curr, _temp693.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Enumdecl*
_temp694= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); goto _LL695; _LL695: if(
_temp694 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp694){ Cyc_Interface_check_err(
msg,( struct _tagged_string)({ struct _tagged_string _temp696= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp696.last_plus_one - _temp696.curr, _temp696.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_string* msg){ struct Cyc_Absyn_Vardecl* _temp697= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL698; _LL698: if( _temp697 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp697){ Cyc_Interface_check_err( msg,( struct
_tagged_string)({ struct _tagged_string _temp699= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of variable %.*s discloses too much information", _temp699.last_plus_one
- _temp699.curr, _temp699.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct
_tagged_string* msg){ struct Cyc_Absyn_Typedefdecl* _temp700= Cyc_Tcdecl_merge_typedefdecl(
d0, d1, 0, msg); goto _LL701; _LL701: if( _temp700 == 0){ return 0;} if(( struct
Cyc_Absyn_Typedefdecl*) d0 != _temp700){ Cyc_Interface_check_err( msg,( struct
_tagged_string)({ struct _tagged_string _temp702= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of typedef %.*s discloses too much information", _temp702.last_plus_one
- _temp702.curr, _temp702.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct _tagged_string* msg){ struct Cyc_Tcdecl_Xtunionfielddecl* _temp703= Cyc_Tcdecl_merge_xtunionfielddecl(
d0, d1, 0, msg); goto _LL704; _LL704: if( _temp703 == 0){ return 0;} if(( struct
Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp703){ Cyc_Interface_check_err( msg,(
struct _tagged_string)({ struct _tagged_string _temp705= Cyc_Absynpp_qvar2string((
d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp705.last_plus_one - _temp705.curr, _temp705.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ int _temp708=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl,({
unsigned char* _temp706=( unsigned char*)"struct"; struct _tagged_string
_temp707; _temp707.curr= _temp706; _temp707.base= _temp706; _temp707.last_plus_one=
_temp706 + 7; _temp707;}), msg); goto _LL709; _LL709: { int _temp712=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->uniondecls, ie2->uniondecls, Cyc_Interface_incl_uniondecl,({ unsigned char*
_temp710=( unsigned char*)"union"; struct _tagged_string _temp711; _temp711.curr=
_temp710; _temp711.base= _temp710; _temp711.last_plus_one= _temp710 + 6;
_temp711;}), msg); goto _LL713; _LL713: { int _temp716=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->tuniondecls, ie2->tuniondecls,
Cyc_Interface_incl_tuniondecl,({ unsigned char* _temp714=( unsigned char*)"tunion";
struct _tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 7; _temp715;}), msg); goto _LL717; _LL717: {
int _temp720=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,({ unsigned char*
_temp718=( unsigned char*)"enum"; struct _tagged_string _temp719; _temp719.curr=
_temp718; _temp719.base= _temp718; _temp719.last_plus_one= _temp718 + 5;
_temp719;}), msg); goto _LL721; _LL721: { int _temp724=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp722=( unsigned char*)"typedef";
struct _tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 8; _temp723;}), msg) != 0; goto _LL725;
_LL725: { int _temp728=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl,({
unsigned char* _temp726=( unsigned char*)"variable"; struct _tagged_string
_temp727; _temp727.curr= _temp726; _temp727.base= _temp726; _temp727.last_plus_one=
_temp726 + 9; _temp727;}), msg); goto _LL729; _LL729: { int _temp732=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,({
unsigned char* _temp730=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp731; _temp731.curr= _temp730; _temp731.base= _temp730; _temp731.last_plus_one=
_temp730 + 13; _temp731;}), msg); goto _LL733; _LL733: return((((( _temp708?
_temp712: 0)? _temp716: 0)? _temp720: 0)? _temp724: 0)? _temp728: 0)? _temp732:
0;}}}}}}} struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct
_tagged_string* f3; } ; static void Cyc_Interface_incl_ns_f( struct _tuple14*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){ struct _tuple14
_temp736; struct _tagged_string* _temp737; struct Cyc_Dict_Dict* _temp739; int
_temp741; int* _temp743; struct _tuple14* _temp734= env; _temp736=* _temp734;
_LL742: _temp741= _temp736.f1; _temp743=&(* _temp734).f1; goto _LL740; _LL740:
_temp739= _temp736.f2; goto _LL738; _LL738: _temp737= _temp736.f3; goto _LL735;
_LL735: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp744;
_push_handler(& _temp744);{ int _temp746= 0; if( setjmp( _temp744.handler)){
_temp746= 1;} if( ! _temp746){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp739, ns);; _pop_handler();}
else{ void* _temp745=( void*) _exn_thrown; void* _temp748= _temp745; _LL750: if(
_temp748 == Cyc_Dict_Absent){ goto _LL751;} else{ goto _LL752;} _LL752: goto
_LL753; _LL751: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL749; _LL753:( void)
_throw( _temp748); _LL749:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp737)){*
_temp743= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple14
_temp755=({ struct _tuple14 _temp754; _temp754.f1= 1; _temp754.f2= dic2;
_temp754.f3= msg; _temp754;}); goto _LL756; _LL756:(( void(*)( void(* f)( struct
_tuple14*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple14*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_ns_f,&
_temp755, dic1); return _temp755.f1;} int Cyc_Interface_is_subinterface( struct
Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct
_handler_cons _temp757; _push_handler(& _temp757);{ int _temp759= 0; if( setjmp(
_temp757.handler)){ _temp759= 1;} if( ! _temp759){{ int _temp760= 1; goto _LL761;
_LL761: { struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct
_tagged_string* _temp762=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp762[ 0]=( struct _tagged_string)({ struct _tagged_string
_temp763=(*(( struct _tuple3*) _check_null( info))).f1; struct _tagged_string
_temp764=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp763.last_plus_one - _temp763.curr, _temp763.curr, _temp764.last_plus_one -
_temp764.curr, _temp764.curr);}); _temp762;});} if( ! Cyc_Interface_incl_ns( i1->exports,
i2->exports, msg)){ _temp760= 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp765=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp765[ 0]=( struct _tagged_string)({ struct _tagged_string _temp766=(*((
struct _tuple3*) _check_null( info))).f2; struct _tagged_string _temp767=(*((
struct _tuple3*) _check_null( info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp766.last_plus_one - _temp766.curr, _temp766.curr, _temp767.last_plus_one -
_temp767.curr, _temp767.curr);}); _temp765;});} if( ! Cyc_Interface_incl_ns( i2->imports,
i1->imports, msg)){ _temp760= 0;}{ int _temp768= _temp760; _npop_handler( 0u);
return _temp768;}}}; _pop_handler();} else{ void* _temp758=( void*) _exn_thrown;
void* _temp770= _temp758; _LL772: if( _temp770 == Cyc_Tcdecl_Incompatible){ goto
_LL773;} else{ goto _LL774;} _LL774: goto _LL775; _LL773: return 0; _LL775:(
void) _throw( _temp770); _LL771:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); struct
_tagged_string f6; struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f(
struct _tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15
_temp778; struct _tagged_string* _temp779; struct _tagged_string _temp781; void**(*
_temp783)( void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*);
struct Cyc_Dict_Dict* _temp785; struct Cyc_Dict_Dict* _temp787; struct Cyc_Dict_Dict*
_temp789; struct Cyc_Dict_Dict** _temp791; int _temp792; int* _temp794; struct
_tuple15* _temp776= env; _temp778=* _temp776; _LL793: _temp792= _temp778.f1;
_temp794=&(* _temp776).f1; goto _LL790; _LL790: _temp789= _temp778.f2; _temp791=&(*
_temp776).f2; goto _LL788; _LL788: _temp787= _temp778.f3; goto _LL786; _LL786:
_temp785= _temp778.f4; goto _LL784; _LL784: _temp783= _temp778.f5; goto _LL782;
_LL782: _temp781= _temp778.f6; goto _LL780; _LL780: _temp779= _temp778.f7; goto
_LL777; _LL777: { void** y;{ struct _handler_cons _temp795; _push_handler(&
_temp795);{ int _temp797= 0; if( setjmp( _temp795.handler)){ _temp797= 1;} if( !
_temp797){{ void** _temp798=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp787, x); goto _LL799; _LL799: {
void** _temp800= _temp783( _temp798, y2, 0, _temp779); goto _LL801; _LL801: if(
!(( unsigned int) _temp800)){* _temp794= 0; _npop_handler( 0u); return;} y=(
void**) _check_null( _temp800);}}; _pop_handler();} else{ void* _temp796=( void*)
_exn_thrown; void* _temp803= _temp796; _LL805: if( _temp803 == Cyc_Dict_Absent){
goto _LL806;} else{ goto _LL807;} _LL807: goto _LL808; _LL806: y= y2; goto
_LL804; _LL808:( void) _throw( _temp803); _LL804:;}}}{ struct _handler_cons
_temp809; _push_handler(& _temp809);{ int _temp811= 0; if( setjmp( _temp809.handler)){
_temp811= 1;} if( ! _temp811){{ void** _temp812=(( void**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp785, x); goto _LL813;
_LL813: { void** _temp814= _temp783( _temp812, y, 0, _temp779); goto _LL815;
_LL815: if( _temp814 !=( void**) _temp812){ if(( unsigned int) _temp814){ Cyc_Interface_check_err(
_temp779,( struct _tagged_string)({ struct _tagged_string _temp816= _temp781;
struct _tagged_string _temp817=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp816.last_plus_one - _temp816.curr, _temp816.curr, _temp817.last_plus_one -
_temp817.curr, _temp817.curr);}));}* _temp794= 0;}}}; _pop_handler();} else{
void* _temp810=( void*) _exn_thrown; void* _temp819= _temp810; _LL821: if(
_temp819 == Cyc_Dict_Absent){ goto _LL822;} else{ goto _LL823;} _LL823: goto
_LL824; _LL822: if(* _temp794){* _temp791=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(*
_temp791, x, y);} goto _LL820; _LL824:( void) _throw( _temp819); _LL820:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple15 _temp826=({
struct _tuple15 _temp825; _temp825.f1= 1; _temp825.f2= dic1; _temp825.f3= dic1;
_temp825.f4= excl; _temp825.f5= merge_f; _temp825.f6= t; _temp825.f7= msg;
_temp825;}); goto _LL827; _LL827:(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp826, dic2); if( _temp826.f1){ return({ struct Cyc_Core_Opt* _temp828=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp828->v=(
void*) _temp826.f2; _temp828;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp831=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->structdecls, ie2->structdecls, iexcl->structdecls,
Cyc_Tcdecl_merge_structdecl,({ unsigned char* _temp829=( unsigned char*)"struct";
struct _tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 7; _temp830;}), msg); goto _LL832; _LL832: {
struct Cyc_Core_Opt* _temp835=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(*
merge_f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls,
Cyc_Tcdecl_merge_uniondecl,({ unsigned char* _temp833=( unsigned char*)"union";
struct _tagged_string _temp834; _temp834.curr= _temp833; _temp834.base= _temp833;
_temp834.last_plus_one= _temp833 + 6; _temp834;}), msg); goto _LL836; _LL836: {
struct Cyc_Core_Opt* _temp839=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(*
merge_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls,
Cyc_Tcdecl_merge_tuniondecl,({ unsigned char* _temp837=( unsigned char*)"tunion";
struct _tagged_string _temp838; _temp838.curr= _temp837; _temp838.base= _temp837;
_temp838.last_plus_one= _temp837 + 7; _temp838;}), msg); goto _LL840; _LL840: {
struct Cyc_Core_Opt* _temp843=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(*
merge_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls,
Cyc_Tcdecl_merge_enumdecl,({ unsigned char* _temp841=( unsigned char*)"enum";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 5; _temp842;}), msg); goto _LL844; _LL844: {
struct Cyc_Core_Opt* _temp847=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,
iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp845=(
unsigned char*)"typedef"; struct _tagged_string _temp846; _temp846.curr=
_temp845; _temp846.base= _temp845; _temp846.last_plus_one= _temp845 + 8;
_temp846;}), msg); goto _LL848; _LL848: { struct Cyc_Core_Opt* _temp851=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,({
unsigned char* _temp849=( unsigned char*)"variable"; struct _tagged_string
_temp850; _temp850.curr= _temp849; _temp850.base= _temp849; _temp850.last_plus_one=
_temp849 + 9; _temp850;}), msg); goto _LL852; _LL852: { struct Cyc_Core_Opt*
_temp855=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl,({
unsigned char* _temp853=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp854; _temp854.curr= _temp853; _temp854.base= _temp853; _temp854.last_plus_one=
_temp853 + 13; _temp854;}), msg); goto _LL856; _LL856: if(((((( !(( unsigned int)
_temp831)? 1: !(( unsigned int) _temp835))? 1: !(( unsigned int) _temp839))? 1:
!(( unsigned int) _temp843))? 1: !(( unsigned int) _temp847))? 1: !((
unsigned int) _temp851))? 1: !(( unsigned int) _temp855)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp857=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp857->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp831))->v; _temp857->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp835))->v;
_temp857->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp839))->v; _temp857->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp843))->v; _temp857->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp847))->v;
_temp857->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp851))->v; _temp857->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp855))->v; _temp857;});}}}}}}}} struct _tuple16{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; struct
_tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct _tuple16*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple16
_temp860; struct _tagged_string* _temp861; struct Cyc_Dict_Dict* _temp863;
struct Cyc_Dict_Dict* _temp865; struct Cyc_Dict_Dict* _temp867; struct Cyc_Dict_Dict**
_temp869; int _temp870; int* _temp872; struct _tuple16* _temp858= env; _temp860=*
_temp858; _LL871: _temp870= _temp860.f1; _temp872=&(* _temp858).f1; goto _LL868;
_LL868: _temp867= _temp860.f2; _temp869=&(* _temp858).f2; goto _LL866; _LL866:
_temp865= _temp860.f3; goto _LL864; _LL864: _temp863= _temp860.f4; goto _LL862;
_LL862: _temp861= _temp860.f5; goto _LL859; _LL859: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp873;
_push_handler(& _temp873);{ int _temp875= 0; if( setjmp( _temp873.handler)){
_temp875= 1;} if( ! _temp875){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp863, ns);;
_pop_handler();} else{ void* _temp874=( void*) _exn_thrown; void* _temp877=
_temp874; _LL879: if( _temp877 == Cyc_Dict_Absent){ goto _LL880;} else{ goto
_LL881;} _LL881: goto _LL882; _LL880: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL878; _LL882:( void) _throw( _temp877); _LL878:;}}}{ struct _handler_cons
_temp883; _push_handler(& _temp883);{ int _temp885= 0; if( setjmp( _temp883.handler)){
_temp885= 1;} if( ! _temp885){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp865, ns);{ struct Cyc_Interface_Ienv*
_temp886= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp861); goto _LL887; _LL887: if( _temp886 == 0){* _temp872= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*) _check_null(
_temp886);}; _pop_handler();} else{ void* _temp884=( void*) _exn_thrown; void*
_temp889= _temp884; _LL891: if( _temp889 == Cyc_Dict_Absent){ goto _LL892;}
else{ goto _LL893;} _LL893: goto _LL894; _LL892: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL890; _LL894:( void) _throw( _temp889); _LL890:;}}}{ struct Cyc_Interface_Ienv*
_temp895= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp861); goto
_LL896; _LL896: if( _temp895 == 0){* _temp872= 0;} else{ if(* _temp872){*
_temp869=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp869, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp895));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 _temp898=({ struct _tuple16
_temp897; _temp897.f1= 1; _temp897.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp897.f3= dic1; _temp897.f4= excl; _temp897.f5= msg; _temp897;}); goto _LL899;
_LL899:(( void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct
Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_ns_f,& _temp898, dic2); if( _temp898.f1){ return({
struct Cyc_Core_Opt* _temp900=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp900->v=( void*) _temp898.f2; _temp900;});} else{ return 0;}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp903; struct _tagged_string* _temp904; struct _tagged_string _temp906;
struct Cyc_Dict_Dict* _temp908; struct Cyc_Dict_Dict* _temp910; struct Cyc_Dict_Dict**
_temp912; int _temp913; int* _temp915; struct _tuple17* _temp901= env; _temp903=*
_temp901; _LL914: _temp913= _temp903.f1; _temp915=&(* _temp901).f1; goto _LL911;
_LL911: _temp910= _temp903.f2; _temp912=&(* _temp901).f2; goto _LL909; _LL909:
_temp908= _temp903.f3; goto _LL907; _LL907: _temp906= _temp903.f4; goto _LL905;
_LL905: _temp904= _temp903.f5; goto _LL902; _LL902: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp908, x)){ Cyc_Interface_check_err(
_temp904,( struct _tagged_string)({ struct _tagged_string _temp916= _temp906;
struct _tagged_string _temp917=* x; xprintf("%.*s %.*s is exported more than once",
_temp916.last_plus_one - _temp916.curr, _temp916.curr, _temp917.last_plus_one -
_temp917.curr, _temp917.curr);}));* _temp915= 0;} else{ if(* _temp915){*
_temp912=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void* data)) Cyc_Dict_insert)(* _temp912, x, y);}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg){
struct _tuple17 _temp919=({ struct _tuple17 _temp918; _temp918.f1= 1; _temp918.f2=
dic1; _temp918.f3= dic1; _temp918.f4= t; _temp918.f5= msg; _temp918;}); goto
_LL920; _LL920:(( void(*)( void(* f)( struct _tuple17*, struct _tagged_string*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple17* env, struct _tagged_string* x, void* y)) Cyc_Interface_disj_merge_dict_f,&
_temp919, dic2); if( _temp919.f1){ return({ struct Cyc_Core_Opt* _temp921=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp921->v=(
void*) _temp919.f2; _temp921;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_string* msg){ struct Cyc_Core_Opt* _temp924=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls,
ie2->structdecls,({ unsigned char* _temp922=( unsigned char*)"struct"; struct
_tagged_string _temp923; _temp923.curr= _temp922; _temp923.base= _temp922;
_temp923.last_plus_one= _temp922 + 7; _temp923;}), msg); goto _LL925; _LL925: {
struct Cyc_Core_Opt* _temp928=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls,({
unsigned char* _temp926=( unsigned char*)"union"; struct _tagged_string _temp927;
_temp927.curr= _temp926; _temp927.base= _temp926; _temp927.last_plus_one=
_temp926 + 6; _temp927;}), msg); goto _LL929; _LL929: { struct Cyc_Core_Opt*
_temp932=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls,({ unsigned char* _temp930=( unsigned char*)"[x]tunion";
struct _tagged_string _temp931; _temp931.curr= _temp930; _temp931.base= _temp930;
_temp931.last_plus_one= _temp930 + 10; _temp931;}), msg); goto _LL933; _LL933: {
struct Cyc_Core_Opt* _temp936=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,({
unsigned char* _temp934=( unsigned char*)"enum"; struct _tagged_string _temp935;
_temp935.curr= _temp934; _temp935.base= _temp934; _temp935.last_plus_one=
_temp934 + 5; _temp935;}), msg); goto _LL937; _LL937: { struct Cyc_Core_Opt*
_temp940=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl,({
unsigned char* _temp938=( unsigned char*)"typedef"; struct _tagged_string
_temp939; _temp939.curr= _temp938; _temp939.base= _temp938; _temp939.last_plus_one=
_temp938 + 8; _temp939;}), msg); goto _LL941; _LL941: { struct Cyc_Core_Opt*
_temp944=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->vardecls, ie2->vardecls,({ unsigned char* _temp942=( unsigned char*)"variable";
struct _tagged_string _temp943; _temp943.curr= _temp942; _temp943.base= _temp942;
_temp943.last_plus_one= _temp942 + 9; _temp943;}), msg); goto _LL945; _LL945: {
struct Cyc_Core_Opt* _temp948=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,({
unsigned char* _temp946=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp947; _temp947.curr= _temp946; _temp947.base= _temp946; _temp947.last_plus_one=
_temp946 + 13; _temp947;}), msg); goto _LL949; _LL949: if(((((( !(( unsigned int)
_temp924)? 1: !(( unsigned int) _temp928))? 1: !(( unsigned int) _temp932))? 1:
!(( unsigned int) _temp936))? 1: !(( unsigned int) _temp940))? 1: !((
unsigned int) _temp944))? 1: !(( unsigned int) _temp948)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp950=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp950->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp924))->v; _temp950->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp928))->v;
_temp950->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp932))->v; _temp950->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp936))->v; _temp950->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp940))->v;
_temp950->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp944))->v; _temp950->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp948))->v; _temp950;});}}}}}}}} struct _tuple18{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; } ; void
Cyc_Interface_disj_merge_ns_f( struct _tuple18* env, struct Cyc_List_List* ns,
struct Cyc_Interface_Ienv* ie2){ struct _tuple18 _temp953; struct _tagged_string*
_temp954; struct Cyc_Dict_Dict* _temp956; struct Cyc_Dict_Dict* _temp958; struct
Cyc_Dict_Dict** _temp960; int _temp961; int* _temp963; struct _tuple18* _temp951=
env; _temp953=* _temp951; _LL962: _temp961= _temp953.f1; _temp963=&(* _temp951).f1;
goto _LL959; _LL959: _temp958= _temp953.f2; _temp960=&(* _temp951).f2; goto
_LL957; _LL957: _temp956= _temp953.f3; goto _LL955; _LL955: _temp954= _temp953.f4;
goto _LL952; _LL952: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons
_temp964; _push_handler(& _temp964);{ int _temp966= 0; if( setjmp( _temp964.handler)){
_temp966= 1;} if( ! _temp966){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp956, ns);; _pop_handler();}
else{ void* _temp965=( void*) _exn_thrown; void* _temp968= _temp965; _LL970: if(
_temp968 == Cyc_Dict_Absent){ goto _LL971;} else{ goto _LL972;} _LL972: goto
_LL973; _LL971: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL969; _LL973:( void)
_throw( _temp968); _LL969:;}}}{ struct Cyc_Interface_Ienv* _temp974= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp954); goto _LL975; _LL975: if( _temp974 == 0){* _temp963= 0;}
else{ if(* _temp963){* _temp960=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp960, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp974));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple18 _temp977=({
struct _tuple18 _temp976; _temp976.f1= 1; _temp976.f2= dic1; _temp976.f3= dic1;
_temp976.f4= msg; _temp976;}); goto _LL978; _LL978:(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp977, dic2); if( _temp977.f1){ return({ struct Cyc_Core_Opt* _temp979=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp979->v=(
void*) _temp977.f2; _temp979;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp980; _push_handler(& _temp980);{ int _temp982= 0; if(
setjmp( _temp980.handler)){ _temp982= 1;} if( ! _temp982){{ struct
_tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string* _temp983=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp983[ 0]=(
struct _tagged_string)({ struct _tagged_string _temp984=(*(( struct _tuple3*)
_check_null( info))).f1; struct _tagged_string _temp985=(*(( struct _tuple3*)
_check_null( info))).f2; xprintf("merging exports of %.*s and %.*s,", _temp984.last_plus_one
- _temp984.curr, _temp984.curr, _temp985.last_plus_one - _temp985.curr, _temp985.curr);});
_temp983;});}{ struct Cyc_Core_Opt* _temp986= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL987; _LL987: if( _temp986 == 0){ struct Cyc_Interface_I*
_temp988= 0; _npop_handler( 0u); return _temp988;} if( info != 0){ msg=({ struct
_tagged_string* _temp989=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp989[ 0]=( struct _tagged_string)({ struct _tagged_string
_temp990=(*(( struct _tuple3*) _check_null( info))).f1; struct _tagged_string
_temp991=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp990.last_plus_one - _temp990.curr, _temp990.curr, _temp991.last_plus_one -
_temp991.curr, _temp991.curr);}); _temp989;});}{ struct Cyc_Core_Opt* _temp992=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp986))->v, msg); goto _LL993; _LL993: if(
_temp992 == 0){ struct Cyc_Interface_I* _temp994= 0; _npop_handler( 0u); return
_temp994;}{ struct Cyc_Interface_I* _temp996=({ struct Cyc_Interface_I* _temp995=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp995->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp992))->v;
_temp995->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp986))->v; _temp995;}); _npop_handler( 0u); return _temp996;}}}};
_pop_handler();} else{ void* _temp981=( void*) _exn_thrown; void* _temp998=
_temp981; _LL1000: if( _temp998 == Cyc_Tcdecl_Incompatible){ goto _LL1001;}
else{ goto _LL1002;} _LL1002: goto _LL1003; _LL1001: return 0; _LL1003:( void)
_throw( _temp998); _LL999:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return(
struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_string* curr_info= linfo != 0?( struct _tagged_string*)(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=((
struct Cyc_List_List*) _check_null( li))->tl; if( linfo != 0){ linfo=(( struct
Cyc_List_List*) _check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp1005= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp1004=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp1004->f1=*(( struct _tagged_string*)
_check_null( curr_info)); _temp1004->f2=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp1004;}): 0); goto _LL1006; _LL1006: if(
_temp1005 == 0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null(
_temp1005); if( curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_string*
_temp1007=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp1007[ 0]=( struct _tagged_string)({ struct _tagged_string _temp1008=*((
struct _tagged_string*) _check_null( curr_info)); struct _tagged_string
_temp1009=*(( struct _tagged_string*)(( struct Cyc_List_List*) _check_null(
linfo))->hd); xprintf("%.*s+%.*s", _temp1008.last_plus_one - _temp1008.curr,
_temp1008.curr, _temp1009.last_plus_one - _temp1009.curr, _temp1009.curr);});
_temp1007;}): 0;} if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null(
linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List*
Cyc_Interface_add_namespace( struct Cyc_List_List* tds, struct Cyc_List_List* ns){
if( ns == 0){ return tds;} return({ struct Cyc_List_List* _temp1010=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1010->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1011=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp1011[ 0]=({
struct Cyc_Absyn_Namespace_d_struct _temp1012; _temp1012.tag= Cyc_Absyn_Namespace_d;
_temp1012.f1=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp1012.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp1012;}); _temp1011;}), 0); _temp1010->tl= 0;
_temp1010;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp1013=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1013->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp1014=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp1014[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp1015; _temp1015.tag= Cyc_Absyn_Struct_d;
_temp1015.f1= d; _temp1015;}); _temp1014;}), 0); _temp1013->tl= tds; _temp1013;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp1016=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp1016[ 0]=* d; _temp1016;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp1017=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1017->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1018=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1018[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1019; _temp1019.tag= Cyc_Absyn_Struct_d;
_temp1019.f1= d; _temp1019;}); _temp1018;}), 0); _temp1017->tl= tds; _temp1017;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp1020=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1020->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp1021=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp1021[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp1022; _temp1022.tag= Cyc_Absyn_Union_d;
_temp1022.f1= d; _temp1022;}); _temp1021;}), 0); _temp1020->tl= tds; _temp1020;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp1023=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp1023[ 0]=* d; _temp1023;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp1024=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1024->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp1025=(
struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp1025[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp1026; _temp1026.tag= Cyc_Absyn_Union_d;
_temp1026.f1= d; _temp1026;}); _temp1025;}), 0); _temp1024->tl= tds; _temp1024;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp1027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1027->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1028=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1028[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1029; _temp1029.tag= Cyc_Absyn_Tunion_d;
_temp1029.f1= d; _temp1029;}); _temp1028;}), 0); _temp1027->tl= tds; _temp1027;});}
static unsigned char _temp1032[ 2u]="_"; static struct _tagged_string Cyc_Interface_us={
_temp1032, _temp1032, _temp1032 + 2u}; static struct _tagged_string* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp1033=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1033->f1= Cyc_Absyn_empty_tqual();
_temp1033->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp1034=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1034[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1035; _temp1035.tag= Cyc_Absyn_VarType; _temp1035.f1=({
struct Cyc_Absyn_Tvar* _temp1036=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp1036->name= Cyc_Interface_us_p; _temp1036->identity=
0; _temp1036->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_BoxKind); _temp1036;}); _temp1035;}); _temp1034;}); _temp1033;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp1037=( struct
Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp1037[ 0]=* f; _temp1037;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple10*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List*
tds){ d=({ struct Cyc_Absyn_Tuniondecl* _temp1038=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1038[ 0]=* d; _temp1038;});
if( d->fields != 0){ d->fields=({ struct Cyc_Core_Opt* _temp1039=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1039->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp1039;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp1040=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1040->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp1041=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp1041[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp1042; _temp1042.tag= Cyc_Absyn_Tunion_d;
_temp1042.f1= d; _temp1042;}); _temp1041;}), 0); _temp1040->tl= tds; _temp1040;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1043=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1043->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp1044=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1044[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1045; _temp1045.tag= Cyc_Absyn_Enum_d;
_temp1045.f1= d; _temp1045;}); _temp1044;}), 0); _temp1043->tl= tds; _temp1043;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp1046=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp1046[ 0]=* d; _temp1046;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp1047=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1047->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp1048=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1048[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1049; _temp1049.tag= Cyc_Absyn_Enum_d;
_temp1049.f1= d; _temp1049;}); _temp1048;}), 0); _temp1047->tl= tds; _temp1047;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1050=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1050->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp1051=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp1051[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp1052; _temp1052.tag= Cyc_Absyn_Typedef_d;
_temp1052.f1= d; _temp1052;}); _temp1051;}), 0); _temp1050->tl= tds; _temp1050;});}
static struct Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1053=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1053->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1054=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1054[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1055; _temp1055.tag= Cyc_Absyn_Var_d;
_temp1055.f1= d; _temp1055;}); _temp1054;}), 0); _temp1053->tl= tds; _temp1053;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1058; struct Cyc_Absyn_Tunionfield*
_temp1059; struct Cyc_Absyn_Tuniondecl* _temp1061; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1056= d; _temp1058=* _temp1056; _LL1062: _temp1061= _temp1058.base; goto
_LL1060; _LL1060: _temp1059= _temp1058.field; goto _LL1057; _LL1057: _temp1061=({
struct Cyc_Absyn_Tuniondecl* _temp1063=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1063[ 0]=* _temp1061; _temp1063;});
_temp1061->fields=({ struct Cyc_Core_Opt* _temp1064=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1064->v=( void*)({ struct Cyc_List_List*
_temp1065=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1065->hd=( void*) _temp1059; _temp1065->tl= 0; _temp1065;}); _temp1064;});(
void*)( _temp1061->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1066=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1066->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1067=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1067[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1068; _temp1068.tag= Cyc_Absyn_Tunion_d;
_temp1068.f1= _temp1061; _temp1068;}); _temp1067;}), 0); _temp1066->tl= tds;
_temp1066;});} static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header(
struct _tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1071; struct Cyc_Absyn_Tunionfield*
_temp1072; struct Cyc_Absyn_Tuniondecl* _temp1074; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1069= d; _temp1071=* _temp1069; _LL1075: _temp1074= _temp1071.base; goto
_LL1073; _LL1073: _temp1072= _temp1071.field; goto _LL1070; _LL1070: _temp1074=({
struct Cyc_Absyn_Tuniondecl* _temp1076=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1076[ 0]=* _temp1074; _temp1076;});
_temp1072= Cyc_Interface_rewrite_tunionfield( _temp1072);( void*)( _temp1072->sc=(
void*)(( void*) Cyc_Absyn_Extern)); _temp1074->fields=({ struct Cyc_Core_Opt*
_temp1077=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1077->v=( void*)({ struct Cyc_List_List* _temp1078=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1078->hd=( void*) _temp1072;
_temp1078->tl= 0; _temp1078;}); _temp1077;});( void*)( _temp1074->sc=( void*)((
void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp1079=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1079->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp1080=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp1080[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp1081; _temp1081.tag= Cyc_Absyn_Tunion_d;
_temp1081.f1= _temp1074; _temp1081;}); _temp1080;}), 0); _temp1079->tl= tds;
_temp1079;});} static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp1082= 0; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp1082);
_temp1082=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp1082); _temp1082=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp1082); _temp1082=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp1082); if( _temp1082 != 0){ _temp1082=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1082); _temp1082= Cyc_Interface_add_namespace(
_temp1082, ns); Cyc_Absynpp_decllist2file( _temp1082, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1084= 0; goto _LL1085; _LL1085: _temp1084=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header,
ie->xtunionfielddecls, _temp1084); if( _temp1084 != 0){ _temp1084=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1084); _temp1084= Cyc_Interface_add_namespace(
_temp1084, ns); Cyc_Absynpp_decllist2file( _temp1084, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1086= 0; goto _LL1087; _LL1087: _temp1086=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp1086); if( _temp1086 != 0){ _temp1086=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1086); _temp1086= Cyc_Interface_add_namespace(
_temp1086, ns); Cyc_Absynpp_decllist2file( _temp1086, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1088= 0; goto _LL1089; _LL1089: _temp1088=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls,
_temp1088); _temp1088=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, _temp1088); _temp1088=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp1088); _temp1088=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp1088); _temp1088=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp1088); _temp1088=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp1088); if( _temp1088 != 0){ _temp1088=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1088); _temp1088= Cyc_Interface_add_namespace(
_temp1088, ns); Cyc_Absynpp_decllist2file( _temp1088, f);}} void Cyc_Interface_print(
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1090= Cyc_Parse_parse_file(
f); goto _LL1091; _LL1091: { struct Cyc_Tcenv_Tenv* _temp1092= Cyc_Tcenv_tc_init();
goto _LL1093; _LL1093: Cyc_Tc_tc( _temp1092, 0, _temp1090); return Cyc_Interface_gen_extract(
_temp1092->ae, 0);}}} void Cyc_Interface_save( struct Cyc_Interface_I* i, struct
Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i, f);} struct Cyc_Interface_I* Cyc_Interface_load(
struct Cyc_Stdio___sFILE* f){ return Cyc_Interface_parse( f);}