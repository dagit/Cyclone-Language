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
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
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
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl*
Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern int
Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum); extern
void Cyc_Dict_iter_c( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); struct Cyc_Interface_I;
extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern struct Cyc_Interface_I*
Cyc_Interface_final(); extern struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_string f1; struct
_tagged_string f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; extern int Cyc_Absynpp_expand_typedefs; extern
void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
var_default_init, struct Cyc_List_List* ds); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_string
f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_string, int, void*); void* read_fun_state; } ;
struct _tagged_ptr1{ int* curr; int* base; int* last_plus_one; } ; struct Cyc_Lexing_lex_tables{
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
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp7[ 0]=* Cyc_Absyn_exn_xed;
_temp7;}); goto _LL9; _LL9:( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));
_temp5->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp5->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp8); _temp3->exports=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp5);{
struct Cyc_Interface_Ienv* _temp10= Cyc_Interface_new_ienv(); goto _LL11; _LL11: {
struct _tuple0* qmain=({ struct _tuple0* _temp18=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp18->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp22=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp22[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp23; _temp23.tag= Cyc_Absyn_Abs_n;
_temp23.f1= 0; _temp23;}); _temp22;}); _temp18->f2=({ struct _tagged_string*
_temp19=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp19[ 0]=({ unsigned char* _temp20=( unsigned char*)"main"; struct
_tagged_string _temp21; _temp21.curr= _temp20; _temp21.base= _temp20; _temp21.last_plus_one=
_temp20 + 5; _temp21;}); _temp19;}); _temp18;}); struct Cyc_Absyn_Vardecl*
_temp16= Cyc_Absyn_new_vardecl( qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp12->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_uint_t; _temp15;}); _temp12->tl=({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp14=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp14->f1= 0; _temp14->f2= Cyc_Absyn_empty_tqual();
_temp14->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp14;}); _temp13->tl= 0; _temp13;}); _temp12;}), 0,
0, 0, 0), 0); goto _LL17; _LL17:( void*)( _temp16->sc=( void*)(( void*) Cyc_Absyn_Extern));
_temp10->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp10->vardecls,(*
qmain).f2, _temp16); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp10); return _temp3;}}}}} static void Cyc_Interface_err(
struct _tagged_string msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_string sc, struct _tagged_string t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_string)({ struct _tagged_string _temp24= sc; struct
_tagged_string _temp25= t; struct _tagged_string _temp26= Cyc_Absynpp_qvar2string(
n); xprintf("fields of %.*s %.*s %.*s have never been defined", _temp24.last_plus_one
- _temp24.curr, _temp24.curr, _temp25.last_plus_one - _temp25.curr, _temp25.curr,
_temp26.last_plus_one - _temp26.curr, _temp26.curr);}));} static void Cyc_Interface_body_err(
struct _tagged_string sc, struct _tuple0* n){ Cyc_Interface_err(( struct
_tagged_string)({ struct _tagged_string _temp27= sc; struct _tagged_string
_temp28= Cyc_Absynpp_qvar2string( n); xprintf("the body of %.*s function %.*s has never been defined",
_temp27.last_plus_one - _temp27.curr, _temp27.curr, _temp28.last_plus_one -
_temp28.curr, _temp28.curr);}));} static void Cyc_Interface_static_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(( struct _tagged_string)({ struct
_tagged_string _temp29= obj1; struct _tagged_string _temp30= Cyc_Absynpp_qvar2string(
name1); struct _tagged_string _temp31= obj2; struct _tagged_string _temp32= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s", _temp29.last_plus_one
- _temp29.curr, _temp29.curr, _temp30.last_plus_one - _temp30.curr, _temp30.curr,
_temp31.last_plus_one - _temp31.curr, _temp31.curr, _temp32.last_plus_one -
_temp32.curr, _temp32.curr);}));} static void Cyc_Interface_abstract_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(( struct _tagged_string)({ struct
_tagged_string _temp33= obj1; struct _tagged_string _temp34= Cyc_Absynpp_qvar2string(
name1); struct _tagged_string _temp35= obj2; struct _tagged_string _temp36= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr, _temp35.last_plus_one - _temp35.curr, _temp35.curr,
_temp36.last_plus_one - _temp36.curr, _temp36.curr);}));} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; static struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({
struct Cyc_Interface_Seen* _temp37=( struct Cyc_Interface_Seen*) GC_malloc(
sizeof( struct Cyc_Interface_Seen)); _temp37->structs=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp37->unions=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp37->tunions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp37;});} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp38= 1; goto _LL39; _LL39: for( 0; l != 0; l=(( struct Cyc_List_List*)
_check_null( l))->tl){ if( ! Cyc_Interface_check_public_type( ae, seen, obj,
name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){ _temp38= 0;}}
return _temp38;} static int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp40; _push_handler(& _temp40);{ int _temp42= 0; if( setjmp(
_temp40.handler)){ _temp42= 1;} if( ! _temp42){{ int _temp43=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp43;}; _pop_handler();} else{ void* _temp41=( void*) _exn_thrown;
void* _temp45= _temp41; _LL47: if( _temp45 == Cyc_Dict_Absent){ goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { int _temp51= 1; goto _LL52; _LL52:
seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp51); if( d->fields != 0){
struct Cyc_List_List* _temp53=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL54; _LL54: for( 0; _temp53 != 0; _temp53=((
struct Cyc_List_List*) _check_null( _temp53))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp55=( unsigned char*)"struct"; struct
_tagged_string _temp56; _temp56.curr= _temp55; _temp56.base= _temp55; _temp56.last_plus_one=
_temp55 + 7; _temp56;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp53))->hd)->type)){ _temp51= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp51);
return _temp51;} _LL50:( void) _throw( _temp45); _LL46:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp57; _push_handler(& _temp57);{ int _temp59= 0; if(
setjmp( _temp57.handler)){ _temp59= 1;} if( ! _temp59){{ int _temp60=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp60;}; _pop_handler();} else{ void* _temp58=(
void*) _exn_thrown; void* _temp62= _temp58; _LL64: if( _temp62 == Cyc_Dict_Absent){
goto _LL65;} else{ goto _LL66;} _LL66: goto _LL67; _LL65: { int _temp68= 1; goto
_LL69; _LL69: seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp68); if( d->fields != 0){
struct Cyc_List_List* _temp70=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL71; _LL71: for( 0; _temp70 != 0; _temp70=((
struct Cyc_List_List*) _check_null( _temp70))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp72=( unsigned char*)"union"; struct
_tagged_string _temp73; _temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one=
_temp72 + 6; _temp73;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp70))->hd)->type)){ _temp68= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp68);
return _temp68;} _LL67:( void) _throw( _temp62); _LL63:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp74; _push_handler(& _temp74);{ int _temp76= 0; if(
setjmp( _temp74.handler)){ _temp76= 1;} if( ! _temp76){{ int _temp77=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp77;}; _pop_handler();} else{ void*
_temp75=( void*) _exn_thrown; void* _temp79= _temp75; _LL81: if( _temp79 == Cyc_Dict_Absent){
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL82: { int _temp85= 1; goto
_LL86; _LL86: seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp85); if( d->fields != 0){ struct Cyc_List_List* _temp87=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; goto _LL88; _LL88: for( 0;
_temp87 != 0; _temp87=(( struct Cyc_List_List*) _check_null( _temp87))->tl){ if(
!(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct
_tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen,({
unsigned char* _temp89=( unsigned char*)"[x]tunion"; struct _tagged_string
_temp90; _temp90.curr= _temp89; _temp90.base= _temp89; _temp90.last_plus_one=
_temp89 + 10; _temp90;}), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp87))->hd)->typs)){ _temp85= 0;}}} seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, _temp85); return _temp85;} _LL84:(
void) _throw( _temp79); _LL80:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl*
d){ return 1;} static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl* d){ return
Cyc_Interface_check_public_type( ae, seen,({ unsigned char* _temp91=(
unsigned char*)"typedef"; struct _tagged_string _temp92; _temp92.curr= _temp91;
_temp92.base= _temp91; _temp92.last_plus_one= _temp91 + 8; _temp92;}), d->name,(
void*) d->defn);} static int Cyc_Interface_check_public_vardecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl* d){ return Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp93=( unsigned char*)"variable"; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 9; _temp94;}), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns(
void* ns){ void* _temp95= ns; struct Cyc_List_List* _temp101; _LL97: if((
unsigned int) _temp95 > 1u?*(( int*) _temp95) == Cyc_Absyn_Abs_n: 0){ _LL102:
_temp101=(( struct Cyc_Absyn_Abs_n_struct*) _temp95)->f1; goto _LL98;} else{
goto _LL99;} _LL99: goto _LL100; _LL98: return _temp101; _LL100:( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp103=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp103[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp104; _temp104.tag= Cyc_Core_InvalidArg; _temp104.f1=({
unsigned char* _temp105=( unsigned char*)"get_abs_ns"; struct _tagged_string
_temp106; _temp106.curr= _temp105; _temp106.base= _temp105; _temp106.last_plus_one=
_temp105 + 11; _temp106;}); _temp104;}); _temp103;})); _LL96:;} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t){ void* _temp107= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp129; struct Cyc_Absyn_Conref* _temp131; struct Cyc_Absyn_Tqual
_temp133; struct Cyc_Absyn_Conref* _temp135; void* _temp137; void* _temp139;
struct Cyc_Absyn_Exp* _temp141; struct Cyc_Absyn_Tqual _temp143; void* _temp145;
struct Cyc_Core_Opt* _temp147; struct Cyc_Core_Opt _temp149; void* _temp150;
struct Cyc_List_List* _temp152; struct _tuple0* _temp154; struct Cyc_Absyn_FnInfo
_temp156; struct Cyc_List_List* _temp158; struct Cyc_List_List* _temp160; struct
Cyc_Absyn_VarargInfo* _temp162; int _temp164; struct Cyc_List_List* _temp166;
void* _temp168; struct Cyc_Core_Opt* _temp170; struct Cyc_List_List* _temp172;
struct Cyc_List_List* _temp174; struct Cyc_Absyn_Structdecl** _temp176; struct
Cyc_Absyn_Structdecl* _temp178; struct Cyc_List_List* _temp179; struct _tuple0*
_temp181; struct Cyc_Absyn_Uniondecl** _temp183; struct Cyc_Absyn_Uniondecl*
_temp185; struct Cyc_List_List* _temp186; struct _tuple0* _temp188; struct Cyc_Absyn_TunionInfo
_temp190; void* _temp192; struct Cyc_List_List* _temp194; void* _temp196; struct
Cyc_Absyn_Tuniondecl* _temp198; struct Cyc_Absyn_TunionFieldInfo _temp200;
struct Cyc_List_List* _temp202; void* _temp204; struct Cyc_Absyn_Tunionfield*
_temp206; struct Cyc_Absyn_Tuniondecl* _temp208; _LL109: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_PointerType: 0){ _LL130: _temp129=((
struct Cyc_Absyn_PointerType_struct*) _temp107)->f1; _LL140: _temp139=( void*)
_temp129.elt_typ; goto _LL138; _LL138: _temp137=( void*) _temp129.rgn_typ; goto
_LL136; _LL136: _temp135= _temp129.nullable; goto _LL134; _LL134: _temp133=
_temp129.tq; goto _LL132; _LL132: _temp131= _temp129.bounds; goto _LL110;} else{
goto _LL111;} _LL111: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) ==
Cyc_Absyn_ArrayType: 0){ _LL146: _temp145=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp107)->f1; goto _LL144; _LL144: _temp143=(( struct Cyc_Absyn_ArrayType_struct*)
_temp107)->f2; goto _LL142; _LL142: _temp141=(( struct Cyc_Absyn_ArrayType_struct*)
_temp107)->f3; goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TypedefType: 0){ _LL155: _temp154=((
struct Cyc_Absyn_TypedefType_struct*) _temp107)->f1; goto _LL153; _LL153:
_temp152=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f2; goto _LL148;
_LL148: _temp147=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f3; if(
_temp147 == 0){ goto _LL115;} else{ _temp149=* _temp147; _LL151: _temp150=( void*)
_temp149.v; goto _LL114;}} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_FnType: 0){ _LL157: _temp156=((
struct Cyc_Absyn_FnType_struct*) _temp107)->f1; _LL173: _temp172= _temp156.tvars;
goto _LL171; _LL171: _temp170= _temp156.effect; goto _LL169; _LL169: _temp168=(
void*) _temp156.ret_typ; goto _LL167; _LL167: _temp166= _temp156.args; goto
_LL165; _LL165: _temp164= _temp156.c_varargs; goto _LL163; _LL163: _temp162=
_temp156.cyc_varargs; goto _LL161; _LL161: _temp160= _temp156.rgn_po; goto
_LL159; _LL159: _temp158= _temp156.attributes; goto _LL116;} else{ goto _LL117;}
_LL117: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TupleType:
0){ _LL175: _temp174=(( struct Cyc_Absyn_TupleType_struct*) _temp107)->f1; goto
_LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp107 > 4u?*(( int*)
_temp107) == Cyc_Absyn_StructType: 0){ _LL182: _temp181=(( struct Cyc_Absyn_StructType_struct*)
_temp107)->f1; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_StructType_struct*)
_temp107)->f2; goto _LL177; _LL177: _temp176=(( struct Cyc_Absyn_StructType_struct*)
_temp107)->f3; if( _temp176 == 0){ goto _LL121;} else{ _temp178=* _temp176; goto
_LL120;}} else{ goto _LL121;} _LL121: if(( unsigned int) _temp107 > 4u?*(( int*)
_temp107) == Cyc_Absyn_UnionType: 0){ _LL189: _temp188=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f1; goto _LL187; _LL187: _temp186=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f2; goto _LL184; _LL184: _temp183=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f3; if( _temp183 == 0){ goto _LL123;} else{ _temp185=* _temp183; goto
_LL122;}} else{ goto _LL123;} _LL123: if(( unsigned int) _temp107 > 4u?*(( int*)
_temp107) == Cyc_Absyn_TunionType: 0){ _LL191: _temp190=(( struct Cyc_Absyn_TunionType_struct*)
_temp107)->f1; _LL197: _temp196=( void*) _temp190.tunion_info; if(*(( int*)
_temp196) == Cyc_Absyn_KnownTunion){ _LL199: _temp198=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp196)->f1; goto _LL195;} else{ goto _LL125;} _LL195: _temp194= _temp190.targs;
goto _LL193; _LL193: _temp192=( void*) _temp190.rgn; goto _LL124;} else{ goto
_LL125;} _LL125: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TunionFieldType:
0){ _LL201: _temp200=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp107)->f1;
_LL205: _temp204=( void*) _temp200.field_info; if(*(( int*) _temp204) == Cyc_Absyn_KnownTunionfield){
_LL209: _temp208=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp204)->f1;
goto _LL207; _LL207: _temp206=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp204)->f2; goto _LL203;} else{ goto _LL127;} _LL203: _temp202= _temp200.targs;
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL110: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp139); _LL112: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp145); _LL114: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp150); _LL116: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp166)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp168): 0; if( _temp162 != 0){ int _temp212; void*
_temp214; void* _temp216; struct Cyc_Absyn_Tqual _temp218; struct Cyc_Core_Opt*
_temp220; struct Cyc_Absyn_VarargInfo _temp210=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp162)); _LL221: _temp220= _temp210.name; goto _LL219; _LL219:
_temp218= _temp210.tq; goto _LL217; _LL217: _temp216=( void*) _temp210.type;
goto _LL215; _LL215: _temp214=( void*) _temp210.rgn; goto _LL213; _LL213:
_temp212= _temp210.inject; goto _LL211; _LL211: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp216);} return b;} _LL118: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp174); _LL120: if(( void*)
_temp178->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({
unsigned char* _temp222=( unsigned char*)"struct"; struct _tagged_string
_temp223; _temp223.curr= _temp222; _temp223.base= _temp222; _temp223.last_plus_one=
_temp222 + 7; _temp223;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp178->name))->v); return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp179)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp178): 0; _LL122: if(( void*) _temp185->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ unsigned char* _temp224=( unsigned char*)"union";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 6; _temp225;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp185->name))->v); return 0;} return(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp186)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp185): 0; _LL124: { struct _tuple0 _temp228; struct _tagged_string*
_temp229; void* _temp231; struct _tuple0* _temp226= _temp198->name; _temp228=*
_temp226; _LL232: _temp231= _temp228.f1; goto _LL230; _LL230: _temp229= _temp228.f2;
goto _LL227; _LL227: { struct Cyc_List_List* _temp233= Cyc_Interface_get_abs_ns(
_temp231); goto _LL234; _LL234: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp235; _push_handler(& _temp235);{ int _temp237= 0; if( setjmp(
_temp235.handler)){ _temp237= 1;} if( ! _temp237){{ struct Cyc_Tcenv_Genv*
_temp238=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp233); goto _LL239; _LL239: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp238->tuniondecls,
_temp229);}; _pop_handler();} else{ void* _temp236=( void*) _exn_thrown; void*
_temp241= _temp236; _LL243: if( _temp241 == Cyc_Dict_Absent){ goto _LL244;}
else{ goto _LL245;} _LL245: goto _LL246; _LL244:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp247=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp247[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp248; _temp248.tag= Cyc_Core_InvalidArg; _temp248.f1=({
unsigned char* _temp249=( unsigned char*)"check_public_type"; struct
_tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 18; _temp250;}); _temp248;}); _temp247;}));
_LL246:( void) _throw( _temp241); _LL242:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ unsigned char* _temp251=( unsigned char*)"[x]tunion";
struct _tagged_string _temp252; _temp252.curr= _temp251; _temp252.base= _temp251;
_temp252.last_plus_one= _temp251 + 10; _temp252;}), tud->name); return 0;}
return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,( void*(*)( void*))
Cyc_Core_identity, _temp194)? Cyc_Interface_check_public_tuniondecl( ae, seen,
tud): 0;}}} _LL126: { struct _tuple0 _temp255; struct _tagged_string* _temp256;
void* _temp258; struct _tuple0* _temp253= _temp208->name; _temp255=* _temp253;
_LL259: _temp258= _temp255.f1; goto _LL257; _LL257: _temp256= _temp255.f2; goto
_LL254; _LL254: { struct Cyc_List_List* _temp260= Cyc_Interface_get_abs_ns(
_temp258); goto _LL261; _LL261: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp262; _push_handler(& _temp262);{ int _temp264= 0; if( setjmp(
_temp262.handler)){ _temp264= 1;} if( ! _temp264){{ struct Cyc_Tcenv_Genv*
_temp265=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp260); goto _LL266; _LL266: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp265->tuniondecls,
_temp256);}; _pop_handler();} else{ void* _temp263=( void*) _exn_thrown; void*
_temp268= _temp263; _LL270: if( _temp268 == Cyc_Dict_Absent){ goto _LL271;}
else{ goto _LL272;} _LL272: goto _LL273; _LL271:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp274=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp274[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp275; _temp275.tag= Cyc_Core_InvalidArg; _temp275.f1=({
unsigned char* _temp276=( unsigned char*)"check_public_type"; struct
_tagged_string _temp277; _temp277.curr= _temp276; _temp277.base= _temp276;
_temp277.last_plus_one= _temp276 + 18; _temp277;}); _temp275;}); _temp274;}));
_LL273:( void) _throw( _temp268); _LL269:;}}} if( tud->fields == 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp278=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp278[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp279; _temp279.tag= Cyc_Core_InvalidArg; _temp279.f1=({
unsigned char* _temp280=( unsigned char*)"check_public_type"; struct
_tagged_string _temp281; _temp281.curr= _temp280; _temp281.base= _temp280;
_temp281.last_plus_one= _temp280 + 18; _temp281;}); _temp279;}); _temp278;}));}{
struct Cyc_Absyn_Tunionfield* tuf1= 0;{ struct Cyc_List_List* _temp282=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v; goto
_LL283; _LL283: for( 0; _temp282 != 0; _temp282=(( struct Cyc_List_List*)
_check_null( _temp282))->tl){ int cmp; if(( cmp= Cyc_Absyn_qvar_cmp( _temp206->name,((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp282))->hd)->name))
>= 0){ if( cmp == 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp282))->hd);} break;}}} if( tuf1 == 0){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp284=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp284[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp285; _temp285.tag= Cyc_Core_InvalidArg; _temp285.f1=({
unsigned char* _temp286=( unsigned char*)"check_public_type"; struct
_tagged_string _temp287; _temp287.curr= _temp286; _temp287.base= _temp286;
_temp287.last_plus_one= _temp286 + 18; _temp287;}); _temp285;}); _temp284;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*) _check_null(
tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,({ unsigned char* _temp288=( unsigned char*)"[x]tunion"; struct
_tagged_string _temp289; _temp289.curr= _temp288; _temp289.base= _temp288;
_temp289.last_plus_one= _temp288 + 10; _temp289;}), tud->name); return 0;} if((
void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err( obj,
name,({ unsigned char* _temp290=( unsigned char*)"[x]tunion"; struct
_tagged_string _temp291; _temp291.curr= _temp290; _temp291.base= _temp290;
_temp291.last_plus_one= _temp290 + 10; _temp291;}), tud->name); return 0;} if((
void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,(
struct _tagged_string)({ struct _tagged_string _temp292= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _temp292.last_plus_one - _temp292.curr,
_temp292.curr);}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp202)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}}} _LL128: return 1; _LL108:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp295; struct Cyc_Interface_I* _temp296; struct Cyc_Interface_Seen*
_temp298; struct Cyc_Dict_Dict* _temp300; int _temp302; struct Cyc_Interface_Ienv*
_temp304; struct Cyc_Interface_Ienv* _temp306; struct _tuple11* _temp293= env;
_temp295=* _temp293; _LL307: _temp306= _temp295.f1; goto _LL305; _LL305:
_temp304= _temp295.f2; goto _LL303; _LL303: _temp302= _temp295.f3; goto _LL301;
_LL301: _temp300= _temp295.f4; goto _LL299; _LL299: _temp298= _temp295.f5; goto
_LL297; _LL297: _temp296= _temp295.f6; goto _LL294; _LL294: { struct Cyc_Absyn_Structdecl*
_temp308=* dp; goto _LL309; _LL309: { void* _temp310=( void*) _temp308->sc;
_LL312: if( _temp310 ==( void*) Cyc_Absyn_Static){ goto _LL313;} else{ goto
_LL314;} _LL314: if( _temp310 ==( void*) Cyc_Absyn_Abstract){ goto _LL315;}
else{ goto _LL316;} _LL316: if( _temp310 ==( void*) Cyc_Absyn_Public){ goto
_LL317;} else{ goto _LL318;} _LL318: if( _temp310 ==( void*) Cyc_Absyn_ExternC){
goto _LL319;} else{ goto _LL320;} _LL320: if( _temp310 ==( void*) Cyc_Absyn_Extern){
goto _LL321;} else{ goto _LL311;} _LL313: if( _temp302? _temp308->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp322=( unsigned char*)"static";
struct _tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 7; _temp323;}),({ unsigned char* _temp324=(
unsigned char*)"struct"; struct _tagged_string _temp325; _temp325.curr= _temp324;
_temp325.base= _temp324; _temp325.last_plus_one= _temp324 + 7; _temp325;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp308->name))->v);}
goto _LL311; _LL315: if( _temp308->fields == 0){ if( _temp302){ Cyc_Interface_fields_err(({
unsigned char* _temp326=( unsigned char*)"abstract"; struct _tagged_string
_temp327; _temp327.curr= _temp326; _temp327.base= _temp326; _temp327.last_plus_one=
_temp326 + 9; _temp327;}),({ unsigned char* _temp328=( unsigned char*)"struct";
struct _tagged_string _temp329; _temp329.curr= _temp328; _temp329.base= _temp328;
_temp329.last_plus_one= _temp328 + 7; _temp329;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp308->name))->v);}} else{ _temp308=({ struct Cyc_Absyn_Structdecl*
_temp330=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp330[ 0]=* _temp308; _temp330;}); _temp308->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp300, _temp298, _temp308)){ _temp304->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp304->structdecls, x, _temp308);} goto _LL311;
_LL317: if( _temp308->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp331=( unsigned char*)"public"; struct _tagged_string _temp332; _temp332.curr=
_temp331; _temp332.base= _temp331; _temp332.last_plus_one= _temp331 + 7;
_temp332;}),({ unsigned char* _temp333=( unsigned char*)"struct"; struct
_tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 7; _temp334;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp308->name))->v); _temp308=({ struct Cyc_Absyn_Structdecl*
_temp335=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp335[ 0]=* _temp308; _temp335;});( void*)( _temp308->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl( _temp300,
_temp298, _temp308)){ _temp304->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp304->structdecls, x, _temp308);} goto _LL311; _LL319:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp336=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp336[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp337; _temp337.tag= Cyc_Core_InvalidArg; _temp337.f1=({
unsigned char* _temp338=( unsigned char*)"extract_structdecl"; struct
_tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 19; _temp339;}); _temp337;}); _temp336;}));
_LL321: if( Cyc_Interface_check_public_structdecl( _temp300, _temp298, _temp308)){
_temp306->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp306->structdecls, x, _temp308);} goto _LL311; _LL311:;}}} static void Cyc_Interface_extract_uniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple11 _temp342; struct Cyc_Interface_I* _temp343; struct Cyc_Interface_Seen*
_temp345; struct Cyc_Dict_Dict* _temp347; int _temp349; struct Cyc_Interface_Ienv*
_temp351; struct Cyc_Interface_Ienv* _temp353; struct _tuple11* _temp340= env;
_temp342=* _temp340; _LL354: _temp353= _temp342.f1; goto _LL352; _LL352:
_temp351= _temp342.f2; goto _LL350; _LL350: _temp349= _temp342.f3; goto _LL348;
_LL348: _temp347= _temp342.f4; goto _LL346; _LL346: _temp345= _temp342.f5; goto
_LL344; _LL344: _temp343= _temp342.f6; goto _LL341; _LL341: { struct Cyc_Absyn_Uniondecl*
_temp355=* dp; goto _LL356; _LL356: { void* _temp357=( void*) _temp355->sc;
_LL359: if( _temp357 ==( void*) Cyc_Absyn_Static){ goto _LL360;} else{ goto
_LL361;} _LL361: if( _temp357 ==( void*) Cyc_Absyn_Abstract){ goto _LL362;}
else{ goto _LL363;} _LL363: if( _temp357 ==( void*) Cyc_Absyn_Public){ goto
_LL364;} else{ goto _LL365;} _LL365: if( _temp357 ==( void*) Cyc_Absyn_ExternC){
goto _LL366;} else{ goto _LL367;} _LL367: if( _temp357 ==( void*) Cyc_Absyn_Extern){
goto _LL368;} else{ goto _LL358;} _LL360: if( _temp349? _temp355->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp369=( unsigned char*)"static";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 7; _temp370;}),({ unsigned char* _temp371=(
unsigned char*)"union"; struct _tagged_string _temp372; _temp372.curr= _temp371;
_temp372.base= _temp371; _temp372.last_plus_one= _temp371 + 6; _temp372;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp355->name))->v);}
goto _LL358; _LL362: if( _temp355->fields == 0){ if( _temp349){ Cyc_Interface_fields_err(({
unsigned char* _temp373=( unsigned char*)"abstract"; struct _tagged_string
_temp374; _temp374.curr= _temp373; _temp374.base= _temp373; _temp374.last_plus_one=
_temp373 + 9; _temp374;}),({ unsigned char* _temp375=( unsigned char*)"union";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 6; _temp376;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp355->name))->v);}} else{ _temp355=({ struct Cyc_Absyn_Uniondecl*
_temp377=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp377[ 0]=* _temp355; _temp377;}); _temp355->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp347, _temp345, _temp355)){ _temp351->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp351->uniondecls, x, _temp355);} goto _LL358;
_LL364: if( _temp355->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp378=( unsigned char*)"public"; struct _tagged_string _temp379; _temp379.curr=
_temp378; _temp379.base= _temp378; _temp379.last_plus_one= _temp378 + 7;
_temp379;}),({ unsigned char* _temp380=( unsigned char*)"union"; struct
_tagged_string _temp381; _temp381.curr= _temp380; _temp381.base= _temp380;
_temp381.last_plus_one= _temp380 + 6; _temp381;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp355->name))->v); _temp355=({ struct Cyc_Absyn_Uniondecl*
_temp382=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp382[ 0]=* _temp355; _temp382;});( void*)( _temp355->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl( _temp347,
_temp345, _temp355)){ _temp351->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data))
Cyc_Dict_insert)( _temp351->uniondecls, x, _temp355);} goto _LL358; _LL366:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp383=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp383[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp384; _temp384.tag= Cyc_Core_InvalidArg; _temp384.f1=({
unsigned char* _temp385=( unsigned char*)"extract_uniondecl"; struct
_tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 18; _temp386;}); _temp384;}); _temp383;}));
_LL368: if( Cyc_Interface_check_public_uniondecl( _temp347, _temp345, _temp355)){
_temp353->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp353->uniondecls, x, _temp355);} goto _LL358; _LL358:;}}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp389; struct Cyc_Interface_I* _temp390; struct Cyc_Interface_Seen*
_temp392; struct Cyc_Dict_Dict* _temp394; int _temp396; struct Cyc_Interface_Ienv*
_temp398; struct Cyc_Interface_Ienv* _temp400; struct _tuple11* _temp387= env;
_temp389=* _temp387; _LL401: _temp400= _temp389.f1; goto _LL399; _LL399:
_temp398= _temp389.f2; goto _LL397; _LL397: _temp396= _temp389.f3; goto _LL395;
_LL395: _temp394= _temp389.f4; goto _LL393; _LL393: _temp392= _temp389.f5; goto
_LL391; _LL391: _temp390= _temp389.f6; goto _LL388; _LL388: { struct Cyc_Absyn_Enumdecl*
_temp402=* dp; goto _LL403; _LL403: { void* _temp404=( void*) _temp402->sc;
_LL406: if( _temp404 ==( void*) Cyc_Absyn_Static){ goto _LL407;} else{ goto
_LL408;} _LL408: if( _temp404 ==( void*) Cyc_Absyn_Abstract){ goto _LL409;}
else{ goto _LL410;} _LL410: if( _temp404 ==( void*) Cyc_Absyn_Public){ goto
_LL411;} else{ goto _LL412;} _LL412: if( _temp404 ==( void*) Cyc_Absyn_ExternC){
goto _LL413;} else{ goto _LL414;} _LL414: if( _temp404 ==( void*) Cyc_Absyn_Extern){
goto _LL415;} else{ goto _LL405;} _LL407: if( _temp396? _temp402->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp416=( unsigned char*)"static";
struct _tagged_string _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 7; _temp417;}),({ unsigned char* _temp418=(
unsigned char*)"enum"; struct _tagged_string _temp419; _temp419.curr= _temp418;
_temp419.base= _temp418; _temp419.last_plus_one= _temp418 + 5; _temp419;}),
_temp402->name);} goto _LL405; _LL409: if( _temp402->fields == 0){ if( _temp396){
Cyc_Interface_fields_err(({ unsigned char* _temp420=( unsigned char*)"abstract";
struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 9; _temp421;}),({ unsigned char* _temp422=(
unsigned char*)"enum"; struct _tagged_string _temp423; _temp423.curr= _temp422;
_temp423.base= _temp422; _temp423.last_plus_one= _temp422 + 5; _temp423;}),
_temp402->name);}} else{ _temp402=({ struct Cyc_Absyn_Enumdecl* _temp424=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp424[ 0]=* _temp402; _temp424;}); _temp402->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp394, _temp392, _temp402)){ _temp398->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp398->enumdecls, x, _temp402);} goto _LL405; _LL411:
if( _temp402->fields == 0){ Cyc_Interface_fields_err(({ unsigned char* _temp425=(
unsigned char*)"public"; struct _tagged_string _temp426; _temp426.curr= _temp425;
_temp426.base= _temp425; _temp426.last_plus_one= _temp425 + 7; _temp426;}),({
unsigned char* _temp427=( unsigned char*)"enum"; struct _tagged_string _temp428;
_temp428.curr= _temp427; _temp428.base= _temp427; _temp428.last_plus_one=
_temp427 + 5; _temp428;}), _temp402->name); _temp402=({ struct Cyc_Absyn_Enumdecl*
_temp429=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp429[ 0]=* _temp402; _temp429;});( void*)( _temp402->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl( _temp394,
_temp392, _temp402)){ _temp398->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp398->enumdecls, x, _temp402);} goto _LL405; _LL413:( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp430=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp430[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp431; _temp431.tag= Cyc_Core_InvalidArg; _temp431.f1=({
unsigned char* _temp432=( unsigned char*)"extract_enumdecl"; struct
_tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 17; _temp433;}); _temp431;}); _temp430;}));
_LL415: if( Cyc_Interface_check_public_enumdecl( _temp394, _temp392, _temp402)){
_temp400->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(
_temp400->enumdecls, x, _temp402);} goto _LL405; _LL405:;}}} static void Cyc_Interface_extract_xtunionfielddecl(
struct Cyc_Interface_I* i, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield*
f){ struct Cyc_List_List* _temp434= 0; goto _LL435; _LL435:{ void* _temp436=(* f->name).f1;
struct Cyc_List_List* _temp444; struct Cyc_List_List* _temp446; _LL438: if((
unsigned int) _temp436 > 1u?*(( int*) _temp436) == Cyc_Absyn_Rel_n: 0){ _LL445:
_temp444=(( struct Cyc_Absyn_Rel_n_struct*) _temp436)->f1; goto _LL439;} else{
goto _LL440;} _LL440: if(( unsigned int) _temp436 > 1u?*(( int*) _temp436) ==
Cyc_Absyn_Abs_n: 0){ _LL447: _temp446=(( struct Cyc_Absyn_Abs_n_struct*)
_temp436)->f1; goto _LL441;} else{ goto _LL442;} _LL442: goto _LL443; _LL439:
_temp446= _temp444; goto _LL441; _LL441: _temp434= _temp446; goto _LL437; _LL443:
goto _LL437; _LL437:;}{ struct Cyc_Dict_Dict** dict;{ void* _temp448=( void*) f->sc;
_LL450: if( _temp448 ==( void*) Cyc_Absyn_Static){ goto _LL451;} else{ goto
_LL452;} _LL452: if( _temp448 ==( void*) Cyc_Absyn_Extern){ goto _LL453;} else{
goto _LL454;} _LL454: if( _temp448 ==( void*) Cyc_Absyn_Public){ goto _LL455;}
else{ goto _LL456;} _LL456: goto _LL457; _LL451: return; _LL453: dict=& i->imports;
goto _LL449; _LL455: dict=& i->exports; goto _LL449; _LL457:( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp458=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp458[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp459; _temp459.tag= Cyc_Core_InvalidArg; _temp459.f1=({
unsigned char* _temp460=( unsigned char*)"add_xtunionfielddecl"; struct
_tagged_string _temp461; _temp461.curr= _temp460; _temp461.base= _temp460;
_temp461.last_plus_one= _temp460 + 21; _temp461;}); _temp459;}); _temp458;}));
_LL449:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons _temp462;
_push_handler(& _temp462);{ int _temp464= 0; if( setjmp( _temp462.handler)){
_temp464= 1;} if( ! _temp464){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp434);; _pop_handler();}
else{ void* _temp463=( void*) _exn_thrown; void* _temp466= _temp463; _LL468: if(
_temp466 == Cyc_Dict_Absent){ goto _LL469;} else{ goto _LL470;} _LL470: goto
_LL471; _LL469: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp434, env); goto _LL467; _LL471:( void)
_throw( _temp466); _LL467:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp472=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp472->base= d; _temp472->field= f; _temp472;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp475; struct Cyc_Interface_I* _temp476; struct Cyc_Interface_Seen*
_temp478; struct Cyc_Dict_Dict* _temp480; int _temp482; struct Cyc_Interface_Ienv*
_temp484; struct Cyc_Interface_Ienv* _temp486; struct _tuple11* _temp473= env;
_temp475=* _temp473; _LL487: _temp486= _temp475.f1; goto _LL485; _LL485:
_temp484= _temp475.f2; goto _LL483; _LL483: _temp482= _temp475.f3; goto _LL481;
_LL481: _temp480= _temp475.f4; goto _LL479; _LL479: _temp478= _temp475.f5; goto
_LL477; _LL477: _temp476= _temp475.f6; goto _LL474; _LL474: { struct Cyc_Absyn_Tuniondecl*
_temp488=* dp; goto _LL489; _LL489: { void* _temp490=( void*) _temp488->sc;
_LL492: if( _temp490 ==( void*) Cyc_Absyn_Static){ goto _LL493;} else{ goto
_LL494;} _LL494: if( _temp490 ==( void*) Cyc_Absyn_Abstract){ goto _LL495;}
else{ goto _LL496;} _LL496: if( _temp490 ==( void*) Cyc_Absyn_Public){ goto
_LL497;} else{ goto _LL498;} _LL498: if( _temp490 ==( void*) Cyc_Absyn_ExternC){
goto _LL499;} else{ goto _LL500;} _LL500: if( _temp490 ==( void*) Cyc_Absyn_Extern){
goto _LL501;} else{ goto _LL491;} _LL493: if(( ! _temp488->is_xtunion? _temp482:
0)? _temp488->fields == 0: 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp502=( unsigned char*)"static"; struct _tagged_string _temp503; _temp503.curr=
_temp502; _temp503.base= _temp502; _temp503.last_plus_one= _temp502 + 7;
_temp503;}),({ unsigned char* _temp504=( unsigned char*)"tunion"; struct
_tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 7; _temp505;}), _temp488->name);} goto _LL491;
_LL495: if( _temp488->fields == 0){ if( ! _temp488->is_xtunion? _temp482: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp506=( unsigned char*)"abstract";
struct _tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 9; _temp507;}),({ unsigned char* _temp508=(
unsigned char*)"tunion"; struct _tagged_string _temp509; _temp509.curr= _temp508;
_temp509.base= _temp508; _temp509.last_plus_one= _temp508 + 7; _temp509;}),
_temp488->name);}} else{ _temp488=({ struct Cyc_Absyn_Tuniondecl* _temp510=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp510[ 0]=* _temp488; _temp510;}); _temp488->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp480, _temp478, _temp488)){ _temp484->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp484->tuniondecls, x, _temp488);} goto _LL491;
_LL497: _temp488=({ struct Cyc_Absyn_Tuniondecl* _temp511=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp511[ 0]=* _temp488;
_temp511;}); if( ! _temp488->is_xtunion? _temp488->fields == 0: 0){ Cyc_Interface_fields_err(({
unsigned char* _temp512=( unsigned char*)"public"; struct _tagged_string
_temp513; _temp513.curr= _temp512; _temp513.base= _temp512; _temp513.last_plus_one=
_temp512 + 7; _temp513;}),({ unsigned char* _temp514=( unsigned char*)"tunion";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 7; _temp515;}), _temp488->name);( void*)(
_temp488->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp480, _temp478, _temp488)){ if( _temp488->is_xtunion? _temp488->fields != 0:
0){ struct Cyc_List_List* _temp516=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp488->fields))->v; goto _LL517; _LL517: _temp488->fields= 0;{
struct Cyc_List_List* _temp518= _temp516; goto _LL519; _LL519: for( 0; _temp518
!= 0; _temp518=(( struct Cyc_List_List*) _check_null( _temp518))->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp476, _temp488,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp518))->hd);}}} _temp484->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp484->tuniondecls, x, _temp488);} goto _LL491;
_LL499:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp520=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp520[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp521; _temp521.tag= Cyc_Core_InvalidArg;
_temp521.f1=({ unsigned char* _temp522=( unsigned char*)"extract_tuniondecl";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 19; _temp523;}); _temp521;}); _temp520;}));
_LL501: if( Cyc_Interface_check_public_tuniondecl( _temp480, _temp478, _temp488)){
if( _temp488->is_xtunion? _temp488->fields != 0: 0){ _temp488=({ struct Cyc_Absyn_Tuniondecl*
_temp524=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp524[ 0]=* _temp488; _temp524;});{ struct Cyc_List_List* _temp525=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp488->fields))->v; goto
_LL526; _LL526: _temp488->fields= 0;{ struct Cyc_List_List* _temp527= _temp525;
goto _LL528; _LL528: for( 0; _temp527 != 0; _temp527=(( struct Cyc_List_List*)
_check_null( _temp527))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp476,
_temp488,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp527))->hd);}}}} _temp486->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp486->tuniondecls, x, _temp488);} goto _LL491; _LL491:;}}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp531;
struct Cyc_Interface_I* _temp532; struct Cyc_Interface_Seen* _temp534; struct
Cyc_Dict_Dict* _temp536; int _temp538; struct Cyc_Interface_Ienv* _temp540;
struct Cyc_Interface_Ienv* _temp542; struct _tuple11* _temp529= env; _temp531=*
_temp529; _LL543: _temp542= _temp531.f1; goto _LL541; _LL541: _temp540= _temp531.f2;
goto _LL539; _LL539: _temp538= _temp531.f3; goto _LL537; _LL537: _temp536=
_temp531.f4; goto _LL535; _LL535: _temp534= _temp531.f5; goto _LL533; _LL533:
_temp532= _temp531.f6; goto _LL530; _LL530: if( Cyc_Interface_check_public_typedefdecl(
_temp536, _temp534, d)){ _temp540->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp540->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_string* x, struct _tuple7* v){ struct
_tuple11 _temp546; struct Cyc_Interface_I* _temp547; struct Cyc_Interface_Seen*
_temp549; struct Cyc_Dict_Dict* _temp551; int _temp553; struct Cyc_Interface_Ienv*
_temp555; struct Cyc_Interface_Ienv* _temp557; struct _tuple11* _temp544= env;
_temp546=* _temp544; _LL558: _temp557= _temp546.f1; goto _LL556; _LL556:
_temp555= _temp546.f2; goto _LL554; _LL554: _temp553= _temp546.f3; goto _LL552;
_LL552: _temp551= _temp546.f4; goto _LL550; _LL550: _temp549= _temp546.f5; goto
_LL548; _LL548: _temp547= _temp546.f6; goto _LL545; _LL545: { void* _temp559=(*
v).f1; goto _LL560; _LL560: { void* _temp561= _temp559; void* _temp567; _LL563:
if(*(( int*) _temp561) == Cyc_Tcenv_VarRes){ _LL568: _temp567=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp561)->f1; goto _LL564;} else{ goto _LL565;}
_LL565: goto _LL566; _LL564:{ void* _temp569= _temp567; struct Cyc_Absyn_Fndecl*
_temp577; struct Cyc_Absyn_Vardecl* _temp579; _LL571: if(( unsigned int)
_temp569 > 1u?*(( int*) _temp569) == Cyc_Absyn_Funname_b: 0){ _LL578: _temp577=((
struct Cyc_Absyn_Funname_b_struct*) _temp569)->f1; goto _LL572;} else{ goto
_LL573;} _LL573: if(( unsigned int) _temp569 > 1u?*(( int*) _temp569) == Cyc_Absyn_Global_b:
0){ _LL580: _temp579=(( struct Cyc_Absyn_Global_b_struct*) _temp569)->f1; goto
_LL574;} else{ goto _LL575;} _LL575: goto _LL576; _LL572: { struct Cyc_Absyn_Vardecl*
_temp583=({ struct Cyc_Absyn_Vardecl* _temp581=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp581->sc=( void*)(( void*)
_temp577->sc); _temp581->name= _temp577->name; _temp581->tq=({ struct Cyc_Absyn_Tqual
_temp582; _temp582.q_const= 0; _temp582.q_volatile= 0; _temp582.q_restrict= 0;
_temp582;}); _temp581->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp577->cached_typ))->v); _temp581->initializer= 0; _temp581->rgn=
0; _temp581->attributes= 0; _temp581;}); goto _LL584; _LL584: _temp553= 0;
_temp579= _temp583; goto _LL574;} _LL574: if( _temp579->initializer != 0){
_temp579=({ struct Cyc_Absyn_Vardecl* _temp585=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp585[ 0]=* _temp579; _temp585;});
_temp579->initializer= 0;}{ void* _temp586=( void*) _temp579->sc; _LL588: if(
_temp586 ==( void*) Cyc_Absyn_Static){ goto _LL589;} else{ goto _LL590;} _LL590:
if( _temp586 ==( void*) Cyc_Absyn_Abstract){ goto _LL591;} else{ goto _LL592;}
_LL592: if( _temp586 ==( void*) Cyc_Absyn_Public){ goto _LL593;} else{ goto
_LL594;} _LL594: if( _temp586 ==( void*) Cyc_Absyn_ExternC){ goto _LL595;} else{
goto _LL596;} _LL596: if( _temp586 ==( void*) Cyc_Absyn_Extern){ goto _LL597;}
else{ goto _LL587;} _LL589: if( _temp553? Cyc_Tcutil_is_function_type(( void*)
_temp579->type): 0){ Cyc_Interface_body_err(({ unsigned char* _temp598=(
unsigned char*)"static"; struct _tagged_string _temp599; _temp599.curr= _temp598;
_temp599.base= _temp598; _temp599.last_plus_one= _temp598 + 7; _temp599;}),
_temp579->name);} goto _LL587; _LL591:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp600=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp600[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp601; _temp601.tag= Cyc_Core_InvalidArg;
_temp601.f1=({ unsigned char* _temp602=( unsigned char*)"extract_ordinarie";
struct _tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 18; _temp603;}); _temp601;}); _temp600;}));
_LL593: if( _temp553? Cyc_Tcutil_is_function_type(( void*) _temp579->type): 0){
Cyc_Interface_body_err(({ unsigned char* _temp604=( unsigned char*)"public";
struct _tagged_string _temp605; _temp605.curr= _temp604; _temp605.base= _temp604;
_temp605.last_plus_one= _temp604 + 7; _temp605;}), _temp579->name);} if( Cyc_Interface_check_public_vardecl(
_temp551, _temp549, _temp579)){ _temp555->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( _temp555->vardecls, x, _temp579);} goto _LL587; _LL595:
goto _LL597; _LL597: if( Cyc_Interface_check_public_vardecl( _temp551, _temp549,
_temp579)){ _temp557->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp557->vardecls, x, _temp579);} goto _LL587; _LL587:;} goto _LL570; _LL576:
goto _LL570; _LL570:;} goto _LL562; _LL566: goto _LL562; _LL562:;}}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp608; struct Cyc_Interface_Seen* _temp609; struct Cyc_Dict_Dict* _temp611;
int _temp613; struct Cyc_Interface_I* _temp615; struct _tuple12* _temp606= env_f;
_temp608=* _temp606; _LL616: _temp615= _temp608.f1; goto _LL614; _LL614:
_temp613= _temp608.f2; goto _LL612; _LL612: _temp611= _temp608.f3; goto _LL610;
_LL610: _temp609= _temp608.f4; goto _LL607; _LL607: { struct _tuple11 _temp618=({
struct _tuple11 _temp617; _temp617.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp615->imports,
ns); _temp617.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp615->exports, ns); _temp617.f3=
_temp613; _temp617.f4= _temp611; _temp617.f5= _temp609; _temp617.f6= _temp615;
_temp617;}); goto _LL619; _LL619:(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& _temp618,
ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct _tagged_string*,
struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp618, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp618, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp618, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp618, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp618, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp621=({ struct _tuple12
_temp620; _temp620.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp620.f2= check_complete_defs; _temp620.f3= ae;
_temp620.f4= Cyc_Interface_new_seen(); _temp620;}); goto _LL622; _LL622:(( void(*)(
void(* f)( struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*),
struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp621, ae); return _temp621.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_string* msg1, struct
_tagged_string msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_string*);
struct _tagged_string f4; struct _tagged_string* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_string* x, void* y1){ struct _tuple13
_temp625; struct _tagged_string* _temp626; struct _tagged_string _temp628; int(*
_temp630)( void*, void*, struct _tagged_string*); struct Cyc_Dict_Dict* _temp632;
int _temp634; int* _temp636; struct _tuple13* _temp623= env; _temp625=* _temp623;
_LL635: _temp634= _temp625.f1; _temp636=&(* _temp623).f1; goto _LL633; _LL633:
_temp632= _temp625.f2; goto _LL631; _LL631: _temp630= _temp625.f3; goto _LL629;
_LL629: _temp628= _temp625.f4; goto _LL627; _LL627: _temp626= _temp625.f5; goto
_LL624; _LL624: { struct _handler_cons _temp637; _push_handler(& _temp637);{ int
_temp639= 0; if( setjmp( _temp637.handler)){ _temp639= 1;} if( ! _temp639){{
void* _temp640=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp632, x); goto _LL641; _LL641: if( ! _temp630( y1,
_temp640, _temp626)){* _temp636= 0;}}; _pop_handler();} else{ void* _temp638=(
void*) _exn_thrown; void* _temp643= _temp638; _LL645: if( _temp643 == Cyc_Dict_Absent){
goto _LL646;} else{ goto _LL647;} _LL647: goto _LL648; _LL646: Cyc_Interface_check_err(
_temp626,( struct _tagged_string)({ struct _tagged_string _temp649= _temp628;
struct _tagged_string _temp650=* x; xprintf("%.*s %.*s is missing", _temp649.last_plus_one
- _temp649.curr, _temp649.curr, _temp650.last_plus_one - _temp650.curr, _temp650.curr);}));*
_temp636= 0; goto _LL644; _LL648:( void) _throw( _temp643); _LL644:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple13 _temp652=({
struct _tuple13 _temp651; _temp651.f1= 1; _temp651.f2= dic2; _temp651.f3= incl_f;
_temp651.f4= t; _temp651.f5= msg; _temp651;}); goto _LL653; _LL653:(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_string*, void*), struct _tuple13*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& _temp652, dic1);
return _temp652.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Structdecl*
_temp654= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); goto _LL655; _LL655: if(
_temp654 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp654){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp656= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp656.last_plus_one - _temp656.curr, _temp656.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct
Cyc_Absyn_Uniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Uniondecl*
_temp657= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); goto _LL658; _LL658: if(
_temp657 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 != _temp657){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp659= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp659.last_plus_one - _temp659.curr, _temp659.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Tuniondecl*
_temp660= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); goto _LL661; _LL661: if(
_temp660 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 != _temp660){
Cyc_Interface_check_err( msg,( struct _tagged_string)({ struct _tagged_string
_temp662= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp662.last_plus_one - _temp662.curr, _temp662.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Enumdecl*
_temp663= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); goto _LL664; _LL664: if(
_temp663 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp663){ Cyc_Interface_check_err(
msg,( struct _tagged_string)({ struct _tagged_string _temp665= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp665.last_plus_one - _temp665.curr, _temp665.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_string* msg){ struct Cyc_Absyn_Vardecl* _temp666= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL667; _LL667: if( _temp666 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp666){ Cyc_Interface_check_err( msg,( struct
_tagged_string)({ struct _tagged_string _temp668= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of variable %.*s discloses too much information", _temp668.last_plus_one
- _temp668.curr, _temp668.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct
_tagged_string* msg){ struct Cyc_Absyn_Typedefdecl* _temp669= Cyc_Tcdecl_merge_typedefdecl(
d0, d1, 0, msg); goto _LL670; _LL670: if( _temp669 == 0){ return 0;} if(( struct
Cyc_Absyn_Typedefdecl*) d0 != _temp669){ Cyc_Interface_check_err( msg,( struct
_tagged_string)({ struct _tagged_string _temp671= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of typedef %.*s discloses too much information", _temp671.last_plus_one
- _temp671.curr, _temp671.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct _tagged_string* msg){ struct Cyc_Tcdecl_Xtunionfielddecl* _temp672= Cyc_Tcdecl_merge_xtunionfielddecl(
d0, d1, 0, msg); goto _LL673; _LL673: if( _temp672 == 0){ return 0;} if(( struct
Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp672){ Cyc_Interface_check_err( msg,(
struct _tagged_string)({ struct _tagged_string _temp674= Cyc_Absynpp_qvar2string((
d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp674.last_plus_one - _temp674.curr, _temp674.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ int _temp677=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl,({
unsigned char* _temp675=( unsigned char*)"struct"; struct _tagged_string
_temp676; _temp676.curr= _temp675; _temp676.base= _temp675; _temp676.last_plus_one=
_temp675 + 7; _temp676;}), msg); goto _LL678; _LL678: { int _temp681=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->uniondecls, ie2->uniondecls, Cyc_Interface_incl_uniondecl,({ unsigned char*
_temp679=( unsigned char*)"union"; struct _tagged_string _temp680; _temp680.curr=
_temp679; _temp680.base= _temp679; _temp680.last_plus_one= _temp679 + 6;
_temp680;}), msg); goto _LL682; _LL682: { int _temp685=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->tuniondecls, ie2->tuniondecls,
Cyc_Interface_incl_tuniondecl,({ unsigned char* _temp683=( unsigned char*)"tunion";
struct _tagged_string _temp684; _temp684.curr= _temp683; _temp684.base= _temp683;
_temp684.last_plus_one= _temp683 + 7; _temp684;}), msg); goto _LL686; _LL686: {
int _temp689=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,({ unsigned char*
_temp687=( unsigned char*)"enum"; struct _tagged_string _temp688; _temp688.curr=
_temp687; _temp688.base= _temp687; _temp688.last_plus_one= _temp687 + 5;
_temp688;}), msg); goto _LL690; _LL690: { int _temp693=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp691=( unsigned char*)"typedef";
struct _tagged_string _temp692; _temp692.curr= _temp691; _temp692.base= _temp691;
_temp692.last_plus_one= _temp691 + 8; _temp692;}), msg) != 0; goto _LL694;
_LL694: { int _temp697=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl,({
unsigned char* _temp695=( unsigned char*)"variable"; struct _tagged_string
_temp696; _temp696.curr= _temp695; _temp696.base= _temp695; _temp696.last_plus_one=
_temp695 + 9; _temp696;}), msg); goto _LL698; _LL698: { int _temp701=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,({
unsigned char* _temp699=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp700; _temp700.curr= _temp699; _temp700.base= _temp699; _temp700.last_plus_one=
_temp699 + 13; _temp700;}), msg); goto _LL702; _LL702: return((((( _temp677?
_temp681: 0)? _temp685: 0)? _temp689: 0)? _temp693: 0)? _temp697: 0)? _temp701:
0;}}}}}}} struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct
_tagged_string* f3; } ; static void Cyc_Interface_incl_ns_f( struct _tuple14*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){ struct _tuple14
_temp705; struct _tagged_string* _temp706; struct Cyc_Dict_Dict* _temp708; int
_temp710; int* _temp712; struct _tuple14* _temp703= env; _temp705=* _temp703;
_LL711: _temp710= _temp705.f1; _temp712=&(* _temp703).f1; goto _LL709; _LL709:
_temp708= _temp705.f2; goto _LL707; _LL707: _temp706= _temp705.f3; goto _LL704;
_LL704: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp713;
_push_handler(& _temp713);{ int _temp715= 0; if( setjmp( _temp713.handler)){
_temp715= 1;} if( ! _temp715){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp708, ns);; _pop_handler();}
else{ void* _temp714=( void*) _exn_thrown; void* _temp717= _temp714; _LL719: if(
_temp717 == Cyc_Dict_Absent){ goto _LL720;} else{ goto _LL721;} _LL721: goto
_LL722; _LL720: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL718; _LL722:( void)
_throw( _temp717); _LL718:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp706)){*
_temp712= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple14
_temp724=({ struct _tuple14 _temp723; _temp723.f1= 1; _temp723.f2= dic2;
_temp723.f3= msg; _temp723;}); goto _LL725; _LL725:(( void(*)( void(* f)( struct
_tuple14*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple14*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_ns_f,&
_temp724, dic1); return _temp724.f1;} int Cyc_Interface_is_subinterface( struct
Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct
_handler_cons _temp726; _push_handler(& _temp726);{ int _temp728= 0; if( setjmp(
_temp726.handler)){ _temp728= 1;} if( ! _temp728){{ int _temp729= 1; goto _LL730;
_LL730: { struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct
_tagged_string* _temp731=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp731[ 0]=( struct _tagged_string)({ struct _tagged_string
_temp732=(*(( struct _tuple3*) _check_null( info))).f1; struct _tagged_string
_temp733=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp732.last_plus_one - _temp732.curr, _temp732.curr, _temp733.last_plus_one -
_temp733.curr, _temp733.curr);}); _temp731;});} if( ! Cyc_Interface_incl_ns( i1->exports,
i2->exports, msg)){ _temp729= 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp734=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp734[ 0]=( struct _tagged_string)({ struct _tagged_string _temp735=(*((
struct _tuple3*) _check_null( info))).f2; struct _tagged_string _temp736=(*((
struct _tuple3*) _check_null( info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp735.last_plus_one - _temp735.curr, _temp735.curr, _temp736.last_plus_one -
_temp736.curr, _temp736.curr);}); _temp734;});} if( ! Cyc_Interface_incl_ns( i2->imports,
i1->imports, msg)){ _temp729= 0;}{ int _temp737= _temp729; _npop_handler( 0u);
return _temp737;}}}; _pop_handler();} else{ void* _temp727=( void*) _exn_thrown;
void* _temp739= _temp727; _LL741: if( _temp739 == Cyc_Tcdecl_Incompatible){ goto
_LL742;} else{ goto _LL743;} _LL743: goto _LL744; _LL742: return 0; _LL744:(
void) _throw( _temp739); _LL740:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); struct
_tagged_string f6; struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f(
struct _tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15
_temp747; struct _tagged_string* _temp748; struct _tagged_string _temp750; void**(*
_temp752)( void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*);
struct Cyc_Dict_Dict* _temp754; struct Cyc_Dict_Dict* _temp756; struct Cyc_Dict_Dict*
_temp758; struct Cyc_Dict_Dict** _temp760; int _temp761; int* _temp763; struct
_tuple15* _temp745= env; _temp747=* _temp745; _LL762: _temp761= _temp747.f1;
_temp763=&(* _temp745).f1; goto _LL759; _LL759: _temp758= _temp747.f2; _temp760=&(*
_temp745).f2; goto _LL757; _LL757: _temp756= _temp747.f3; goto _LL755; _LL755:
_temp754= _temp747.f4; goto _LL753; _LL753: _temp752= _temp747.f5; goto _LL751;
_LL751: _temp750= _temp747.f6; goto _LL749; _LL749: _temp748= _temp747.f7; goto
_LL746; _LL746: { void** y;{ struct _handler_cons _temp764; _push_handler(&
_temp764);{ int _temp766= 0; if( setjmp( _temp764.handler)){ _temp766= 1;} if( !
_temp766){{ void** _temp767=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp756, x); goto _LL768; _LL768: {
void** _temp769= _temp752( _temp767, y2, 0, _temp748); goto _LL770; _LL770: if(
!(( unsigned int) _temp769)){* _temp763= 0; _npop_handler( 0u); return;} y=(
void**) _check_null( _temp769);}}; _pop_handler();} else{ void* _temp765=( void*)
_exn_thrown; void* _temp772= _temp765; _LL774: if( _temp772 == Cyc_Dict_Absent){
goto _LL775;} else{ goto _LL776;} _LL776: goto _LL777; _LL775: y= y2; goto
_LL773; _LL777:( void) _throw( _temp772); _LL773:;}}}{ struct _handler_cons
_temp778; _push_handler(& _temp778);{ int _temp780= 0; if( setjmp( _temp778.handler)){
_temp780= 1;} if( ! _temp780){{ void** _temp781=(( void**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp754, x); goto _LL782;
_LL782: { void** _temp783= _temp752( _temp781, y, 0, _temp748); goto _LL784;
_LL784: if( _temp783 !=( void**) _temp781){ if(( unsigned int) _temp783){ Cyc_Interface_check_err(
_temp748,( struct _tagged_string)({ struct _tagged_string _temp785= _temp750;
struct _tagged_string _temp786=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp785.last_plus_one - _temp785.curr, _temp785.curr, _temp786.last_plus_one -
_temp786.curr, _temp786.curr);}));}* _temp763= 0;}}}; _pop_handler();} else{
void* _temp779=( void*) _exn_thrown; void* _temp788= _temp779; _LL790: if(
_temp788 == Cyc_Dict_Absent){ goto _LL791;} else{ goto _LL792;} _LL792: goto
_LL793; _LL791: if(* _temp763){* _temp760=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(*
_temp760, x, y);} goto _LL789; _LL793:( void) _throw( _temp788); _LL789:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple15 _temp795=({
struct _tuple15 _temp794; _temp794.f1= 1; _temp794.f2= dic1; _temp794.f3= dic1;
_temp794.f4= excl; _temp794.f5= merge_f; _temp794.f6= t; _temp794.f7= msg;
_temp794;}); goto _LL796; _LL796:(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp795, dic2); if( _temp795.f1){ return({ struct Cyc_Core_Opt* _temp797=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp797->v=(
void*) _temp795.f2; _temp797;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp800=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->structdecls, ie2->structdecls, iexcl->structdecls,
Cyc_Tcdecl_merge_structdecl,({ unsigned char* _temp798=( unsigned char*)"struct";
struct _tagged_string _temp799; _temp799.curr= _temp798; _temp799.base= _temp798;
_temp799.last_plus_one= _temp798 + 7; _temp799;}), msg); goto _LL801; _LL801: {
struct Cyc_Core_Opt* _temp804=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(*
merge_f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls,
Cyc_Tcdecl_merge_uniondecl,({ unsigned char* _temp802=( unsigned char*)"union";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 6; _temp803;}), msg); goto _LL805; _LL805: {
struct Cyc_Core_Opt* _temp808=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(*
merge_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls,
Cyc_Tcdecl_merge_tuniondecl,({ unsigned char* _temp806=( unsigned char*)"tunion";
struct _tagged_string _temp807; _temp807.curr= _temp806; _temp807.base= _temp806;
_temp807.last_plus_one= _temp806 + 7; _temp807;}), msg); goto _LL809; _LL809: {
struct Cyc_Core_Opt* _temp812=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(*
merge_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls,
Cyc_Tcdecl_merge_enumdecl,({ unsigned char* _temp810=( unsigned char*)"enum";
struct _tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 5; _temp811;}), msg); goto _LL813; _LL813: {
struct Cyc_Core_Opt* _temp816=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,
iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp814=(
unsigned char*)"typedef"; struct _tagged_string _temp815; _temp815.curr=
_temp814; _temp815.base= _temp814; _temp815.last_plus_one= _temp814 + 8;
_temp815;}), msg); goto _LL817; _LL817: { struct Cyc_Core_Opt* _temp820=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,({
unsigned char* _temp818=( unsigned char*)"variable"; struct _tagged_string
_temp819; _temp819.curr= _temp818; _temp819.base= _temp818; _temp819.last_plus_one=
_temp818 + 9; _temp819;}), msg); goto _LL821; _LL821: { struct Cyc_Core_Opt*
_temp824=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl,({
unsigned char* _temp822=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp823; _temp823.curr= _temp822; _temp823.base= _temp822; _temp823.last_plus_one=
_temp822 + 13; _temp823;}), msg); goto _LL825; _LL825: if(((((( !(( unsigned int)
_temp800)? 1: !(( unsigned int) _temp804))? 1: !(( unsigned int) _temp808))? 1:
!(( unsigned int) _temp812))? 1: !(( unsigned int) _temp816))? 1: !((
unsigned int) _temp820))? 1: !(( unsigned int) _temp824)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp826=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp826->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp800))->v; _temp826->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp804))->v;
_temp826->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp808))->v; _temp826->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp812))->v; _temp826->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp816))->v;
_temp826->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp820))->v; _temp826->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp824))->v; _temp826;});}}}}}}}} struct _tuple16{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; struct
_tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct _tuple16*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple16
_temp829; struct _tagged_string* _temp830; struct Cyc_Dict_Dict* _temp832;
struct Cyc_Dict_Dict* _temp834; struct Cyc_Dict_Dict* _temp836; struct Cyc_Dict_Dict**
_temp838; int _temp839; int* _temp841; struct _tuple16* _temp827= env; _temp829=*
_temp827; _LL840: _temp839= _temp829.f1; _temp841=&(* _temp827).f1; goto _LL837;
_LL837: _temp836= _temp829.f2; _temp838=&(* _temp827).f2; goto _LL835; _LL835:
_temp834= _temp829.f3; goto _LL833; _LL833: _temp832= _temp829.f4; goto _LL831;
_LL831: _temp830= _temp829.f5; goto _LL828; _LL828: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp842;
_push_handler(& _temp842);{ int _temp844= 0; if( setjmp( _temp842.handler)){
_temp844= 1;} if( ! _temp844){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp832, ns);;
_pop_handler();} else{ void* _temp843=( void*) _exn_thrown; void* _temp846=
_temp843; _LL848: if( _temp846 == Cyc_Dict_Absent){ goto _LL849;} else{ goto
_LL850;} _LL850: goto _LL851; _LL849: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL847; _LL851:( void) _throw( _temp846); _LL847:;}}}{ struct _handler_cons
_temp852; _push_handler(& _temp852);{ int _temp854= 0; if( setjmp( _temp852.handler)){
_temp854= 1;} if( ! _temp854){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp834, ns);{ struct Cyc_Interface_Ienv*
_temp855= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp830); goto _LL856; _LL856: if( _temp855 == 0){* _temp841= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*) _check_null(
_temp855);}; _pop_handler();} else{ void* _temp853=( void*) _exn_thrown; void*
_temp858= _temp853; _LL860: if( _temp858 == Cyc_Dict_Absent){ goto _LL861;}
else{ goto _LL862;} _LL862: goto _LL863; _LL861: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL859; _LL863:( void) _throw( _temp858); _LL859:;}}}{ struct Cyc_Interface_Ienv*
_temp864= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp830); goto
_LL865; _LL865: if( _temp864 == 0){* _temp841= 0;} else{ if(* _temp841){*
_temp838=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp838, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp864));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 _temp867=({ struct _tuple16
_temp866; _temp866.f1= 1; _temp866.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp866.f3= dic1; _temp866.f4= excl; _temp866.f5= msg; _temp866;}); goto _LL868;
_LL868:(( void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct
Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_ns_f,& _temp867, dic2); if( _temp867.f1){ return({
struct Cyc_Core_Opt* _temp869=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp869->v=( void*) _temp867.f2; _temp869;});} else{ return 0;}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp872; struct _tagged_string* _temp873; struct _tagged_string _temp875;
struct Cyc_Dict_Dict* _temp877; struct Cyc_Dict_Dict* _temp879; struct Cyc_Dict_Dict**
_temp881; int _temp882; int* _temp884; struct _tuple17* _temp870= env; _temp872=*
_temp870; _LL883: _temp882= _temp872.f1; _temp884=&(* _temp870).f1; goto _LL880;
_LL880: _temp879= _temp872.f2; _temp881=&(* _temp870).f2; goto _LL878; _LL878:
_temp877= _temp872.f3; goto _LL876; _LL876: _temp875= _temp872.f4; goto _LL874;
_LL874: _temp873= _temp872.f5; goto _LL871; _LL871: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp877, x)){ Cyc_Interface_check_err(
_temp873,( struct _tagged_string)({ struct _tagged_string _temp885= _temp875;
struct _tagged_string _temp886=* x; xprintf("%.*s %.*s is exported more than once",
_temp885.last_plus_one - _temp885.curr, _temp885.curr, _temp886.last_plus_one -
_temp886.curr, _temp886.curr);}));* _temp884= 0;} else{ if(* _temp884){*
_temp881=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void* data)) Cyc_Dict_insert)(* _temp881, x, y);}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg){
struct _tuple17 _temp888=({ struct _tuple17 _temp887; _temp887.f1= 1; _temp887.f2=
dic1; _temp887.f3= dic1; _temp887.f4= t; _temp887.f5= msg; _temp887;}); goto
_LL889; _LL889:(( void(*)( void(* f)( struct _tuple17*, struct _tagged_string*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple17* env, struct _tagged_string* x, void* y)) Cyc_Interface_disj_merge_dict_f,&
_temp888, dic2); if( _temp888.f1){ return({ struct Cyc_Core_Opt* _temp890=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp890->v=(
void*) _temp888.f2; _temp890;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_string* msg){ struct Cyc_Core_Opt* _temp893=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls,
ie2->structdecls,({ unsigned char* _temp891=( unsigned char*)"struct"; struct
_tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 7; _temp892;}), msg); goto _LL894; _LL894: {
struct Cyc_Core_Opt* _temp897=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls,({
unsigned char* _temp895=( unsigned char*)"union"; struct _tagged_string _temp896;
_temp896.curr= _temp895; _temp896.base= _temp895; _temp896.last_plus_one=
_temp895 + 6; _temp896;}), msg); goto _LL898; _LL898: { struct Cyc_Core_Opt*
_temp901=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls,({ unsigned char* _temp899=( unsigned char*)"[x]tunion";
struct _tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 10; _temp900;}), msg); goto _LL902; _LL902: {
struct Cyc_Core_Opt* _temp905=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,({
unsigned char* _temp903=( unsigned char*)"enum"; struct _tagged_string _temp904;
_temp904.curr= _temp903; _temp904.base= _temp903; _temp904.last_plus_one=
_temp903 + 5; _temp904;}), msg); goto _LL906; _LL906: { struct Cyc_Core_Opt*
_temp909=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl,({
unsigned char* _temp907=( unsigned char*)"typedef"; struct _tagged_string
_temp908; _temp908.curr= _temp907; _temp908.base= _temp907; _temp908.last_plus_one=
_temp907 + 8; _temp908;}), msg); goto _LL910; _LL910: { struct Cyc_Core_Opt*
_temp913=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->vardecls, ie2->vardecls,({ unsigned char* _temp911=( unsigned char*)"variable";
struct _tagged_string _temp912; _temp912.curr= _temp911; _temp912.base= _temp911;
_temp912.last_plus_one= _temp911 + 9; _temp912;}), msg); goto _LL914; _LL914: {
struct Cyc_Core_Opt* _temp917=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,({
unsigned char* _temp915=( unsigned char*)"xtunionfield"; struct _tagged_string
_temp916; _temp916.curr= _temp915; _temp916.base= _temp915; _temp916.last_plus_one=
_temp915 + 13; _temp916;}), msg); goto _LL918; _LL918: if(((((( !(( unsigned int)
_temp893)? 1: !(( unsigned int) _temp897))? 1: !(( unsigned int) _temp901))? 1:
!(( unsigned int) _temp905))? 1: !(( unsigned int) _temp909))? 1: !((
unsigned int) _temp913))? 1: !(( unsigned int) _temp917)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp919=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp919->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp893))->v; _temp919->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp897))->v;
_temp919->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp901))->v; _temp919->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp905))->v; _temp919->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp909))->v;
_temp919->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp913))->v; _temp919->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp917))->v; _temp919;});}}}}}}}} struct _tuple18{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; } ; void
Cyc_Interface_disj_merge_ns_f( struct _tuple18* env, struct Cyc_List_List* ns,
struct Cyc_Interface_Ienv* ie2){ struct _tuple18 _temp922; struct _tagged_string*
_temp923; struct Cyc_Dict_Dict* _temp925; struct Cyc_Dict_Dict* _temp927; struct
Cyc_Dict_Dict** _temp929; int _temp930; int* _temp932; struct _tuple18* _temp920=
env; _temp922=* _temp920; _LL931: _temp930= _temp922.f1; _temp932=&(* _temp920).f1;
goto _LL928; _LL928: _temp927= _temp922.f2; _temp929=&(* _temp920).f2; goto
_LL926; _LL926: _temp925= _temp922.f3; goto _LL924; _LL924: _temp923= _temp922.f4;
goto _LL921; _LL921: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons
_temp933; _push_handler(& _temp933);{ int _temp935= 0; if( setjmp( _temp933.handler)){
_temp935= 1;} if( ! _temp935){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp925, ns);; _pop_handler();}
else{ void* _temp934=( void*) _exn_thrown; void* _temp937= _temp934; _LL939: if(
_temp937 == Cyc_Dict_Absent){ goto _LL940;} else{ goto _LL941;} _LL941: goto
_LL942; _LL940: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL938; _LL942:( void)
_throw( _temp937); _LL938:;}}}{ struct Cyc_Interface_Ienv* _temp943= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp923); goto _LL944; _LL944: if( _temp943 == 0){* _temp932= 0;}
else{ if(* _temp932){* _temp929=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp929, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp943));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple18 _temp946=({
struct _tuple18 _temp945; _temp945.f1= 1; _temp945.f2= dic1; _temp945.f3= dic1;
_temp945.f4= msg; _temp945;}); goto _LL947; _LL947:(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp946, dic2); if( _temp946.f1){ return({ struct Cyc_Core_Opt* _temp948=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp948->v=(
void*) _temp946.f2; _temp948;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp949; _push_handler(& _temp949);{ int _temp951= 0; if(
setjmp( _temp949.handler)){ _temp951= 1;} if( ! _temp951){{ struct
_tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string* _temp952=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp952[ 0]=(
struct _tagged_string)({ struct _tagged_string _temp953=(*(( struct _tuple3*)
_check_null( info))).f1; struct _tagged_string _temp954=(*(( struct _tuple3*)
_check_null( info))).f2; xprintf("merging exports of %.*s and %.*s,", _temp953.last_plus_one
- _temp953.curr, _temp953.curr, _temp954.last_plus_one - _temp954.curr, _temp954.curr);});
_temp952;});}{ struct Cyc_Core_Opt* _temp955= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL956; _LL956: if( _temp955 == 0){ struct Cyc_Interface_I*
_temp957= 0; _npop_handler( 0u); return _temp957;} if( info != 0){ msg=({ struct
_tagged_string* _temp958=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp958[ 0]=( struct _tagged_string)({ struct _tagged_string
_temp959=(*(( struct _tuple3*) _check_null( info))).f1; struct _tagged_string
_temp960=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp959.last_plus_one - _temp959.curr, _temp959.curr, _temp960.last_plus_one -
_temp960.curr, _temp960.curr);}); _temp958;});}{ struct Cyc_Core_Opt* _temp961=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp955))->v, msg); goto _LL962; _LL962: if(
_temp961 == 0){ struct Cyc_Interface_I* _temp963= 0; _npop_handler( 0u); return
_temp963;}{ struct Cyc_Interface_I* _temp965=({ struct Cyc_Interface_I* _temp964=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp964->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp961))->v;
_temp964->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp955))->v; _temp964;}); _npop_handler( 0u); return _temp965;}}}};
_pop_handler();} else{ void* _temp950=( void*) _exn_thrown; void* _temp967=
_temp950; _LL969: if( _temp967 == Cyc_Tcdecl_Incompatible){ goto _LL970;} else{
goto _LL971;} _LL971: goto _LL972; _LL970: return 0; _LL972:( void) _throw(
_temp967); _LL968:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_string* curr_info= linfo != 0?( struct _tagged_string*)(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=((
struct Cyc_List_List*) _check_null( li))->tl; if( linfo != 0){ linfo=(( struct
Cyc_List_List*) _check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp974= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp973=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp973->f1=*(( struct _tagged_string*)
_check_null( curr_info)); _temp973->f2=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp973;}): 0); goto _LL975; _LL975: if( _temp974 ==
0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null( _temp974); if(
curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_string* _temp976=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp976[ 0]=(
struct _tagged_string)({ struct _tagged_string _temp977=*(( struct
_tagged_string*) _check_null( curr_info)); struct _tagged_string _temp978=*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( linfo))->hd);
xprintf("%.*s+%.*s", _temp977.last_plus_one - _temp977.curr, _temp977.curr,
_temp978.last_plus_one - _temp978.curr, _temp978.curr);}); _temp976;}): 0;} if(
linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null( linfo))->tl;}} return(
struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace(
struct Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;}
return({ struct Cyc_List_List* _temp979=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp979->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp980=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp980[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp981; _temp981.tag= Cyc_Absyn_Namespace_d;
_temp981.f1=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp981.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp981;}); _temp980;}), 0); _temp979->tl= 0; _temp979;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_string*
x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp982=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp982->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp983=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp983[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp984; _temp984.tag= Cyc_Absyn_Struct_d;
_temp984.f1= d; _temp984;}); _temp983;}), 0); _temp982->tl= tds; _temp982;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp985=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp985[ 0]=* d; _temp985;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp986=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp986->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp987=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp987[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp988; _temp988.tag= Cyc_Absyn_Struct_d;
_temp988.f1= d; _temp988;}); _temp987;}), 0); _temp986->tl= tds; _temp986;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp989=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp989->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp990=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp990[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp991; _temp991.tag= Cyc_Absyn_Union_d;
_temp991.f1= d; _temp991;}); _temp990;}), 0); _temp989->tl= tds; _temp989;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp992=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp992[ 0]=* d; _temp992;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp993=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp993->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp994=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp994[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp995; _temp995.tag= Cyc_Absyn_Union_d;
_temp995.f1= d; _temp995;}); _temp994;}), 0); _temp993->tl= tds; _temp993;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp996=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp996->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp997=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp997[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp998; _temp998.tag= Cyc_Absyn_Tunion_d;
_temp998.f1= d; _temp998;}); _temp997;}), 0); _temp996->tl= tds; _temp996;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp999=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp999[ 0]=* d; _temp999;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1000=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1000->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1001=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1001[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1002; _temp1002.tag= Cyc_Absyn_Tunion_d;
_temp1002.f1= d; _temp1002;}); _temp1001;}), 0); _temp1000->tl= tds; _temp1000;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1003=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1003->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp1004=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1004[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1005; _temp1005.tag= Cyc_Absyn_Enum_d;
_temp1005.f1= d; _temp1005;}); _temp1004;}), 0); _temp1003->tl= tds; _temp1003;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp1006=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp1006[ 0]=* d; _temp1006;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1007=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1007->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp1008=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1008[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1009; _temp1009.tag= Cyc_Absyn_Enum_d;
_temp1009.f1= d; _temp1009;}); _temp1008;}), 0); _temp1007->tl= tds; _temp1007;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1010=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1010->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp1011=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp1011[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp1012; _temp1012.tag= Cyc_Absyn_Typedef_d;
_temp1012.f1= d; _temp1012;}); _temp1011;}), 0); _temp1010->tl= tds; _temp1010;});}
static struct Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1013=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1013->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1014=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1014[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1015; _temp1015.tag= Cyc_Absyn_Var_d;
_temp1015.f1= d; _temp1015;}); _temp1014;}), 0); _temp1013->tl= tds; _temp1013;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1018; struct Cyc_Absyn_Tunionfield*
_temp1019; struct Cyc_Absyn_Tuniondecl* _temp1021; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1016= d; _temp1018=* _temp1016; _LL1022: _temp1021= _temp1018.base; goto
_LL1020; _LL1020: _temp1019= _temp1018.field; goto _LL1017; _LL1017: _temp1021=({
struct Cyc_Absyn_Tuniondecl* _temp1023=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1023[ 0]=* _temp1021; _temp1023;});
_temp1021->fields=({ struct Cyc_Core_Opt* _temp1024=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1024->v=( void*)({ struct Cyc_List_List*
_temp1025=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1025->hd=( void*) _temp1019; _temp1025->tl= 0; _temp1025;}); _temp1024;});(
void*)( _temp1021->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1026=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1026->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1027=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1027[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1028; _temp1028.tag= Cyc_Absyn_Tunion_d;
_temp1028.f1= _temp1021; _temp1028;}); _temp1027;}), 0); _temp1026->tl= tds;
_temp1026;});} static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp1029= 0; goto _LL1030; _LL1030: _temp1029=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp1029);
_temp1029=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp1029); _temp1029=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp1029); _temp1029=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp1029); if( _temp1029 != 0){ _temp1029=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1029); _temp1029= Cyc_Interface_add_namespace(
_temp1029, ns); Cyc_Absynpp_decllist2file( _temp1029, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1031= 0; goto _LL1032; _LL1032: _temp1031=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp1031); if( _temp1031 != 0){ _temp1031=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1031); _temp1031= Cyc_Interface_add_namespace(
_temp1031, ns); Cyc_Absynpp_decllist2file( _temp1031, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1033= 0; goto _LL1034; _LL1034: _temp1033=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls,
_temp1033); _temp1033=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, _temp1033); _temp1033=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp1033); _temp1033=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp1033); _temp1033=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp1033); _temp1033=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp1033); if( _temp1033 != 0){ _temp1033=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1033); _temp1033= Cyc_Interface_add_namespace(
_temp1033, ns); Cyc_Absynpp_decllist2file( _temp1033, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ fprintf( sfile_to_file(
f),"/****** needed (headers) ******/\n");(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports); fprintf( sfile_to_file( f),"\n/****** provided (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1035= Cyc_Parse_parse_file(
f); goto _LL1036; _LL1036: Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv*
_temp1037= Cyc_Tcenv_tc_init(); goto _LL1038; _LL1038: Cyc_Tc_tc( _temp1037, 0,
0, _temp1035); return Cyc_Interface_gen_extract( _temp1037->ae, 0);}}}