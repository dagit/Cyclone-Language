 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern int
Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter(
void(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
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
extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type( void* t); extern
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only( void* t); extern int
Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e);
extern int Cyc_Tcutil_supports_default( void*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int new_block); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
var_default_init, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern unsigned char Cyc_Tcdecl_Incompatible[
17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base;
struct Cyc_Absyn_Tunionfield* field; } ; extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern void* Cyc_Tcdecl_merge_binding( void*
d0, void* d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg);
extern struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); static unsigned char _temp2[ 1u]=""; static struct
_tagged_string Cyc_Tc_tc_msg_c={ _temp2, _temp2, _temp2 + 1u}; static struct
_tagged_string* Cyc_Tc_tc_msg=( struct _tagged_string*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp3= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp9;
struct Cyc_List_List* _temp11; struct Cyc_List_List** _temp13; struct Cyc_List_List*
_temp14; struct Cyc_Absyn_VarargInfo* _temp16; int _temp18; struct Cyc_List_List*
_temp20; void* _temp22; struct Cyc_Core_Opt* _temp24; struct Cyc_List_List*
_temp26; _LL5: if(( unsigned int) _temp3 > 4u?*(( int*) _temp3) == Cyc_Absyn_FnType:
0){ _LL10: _temp9=(( struct Cyc_Absyn_FnType_struct*) _temp3)->f1; _LL27:
_temp26= _temp9.tvars; goto _LL25; _LL25: _temp24= _temp9.effect; goto _LL23;
_LL23: _temp22=( void*) _temp9.ret_typ; goto _LL21; _LL21: _temp20= _temp9.args;
goto _LL19; _LL19: _temp18= _temp9.c_varargs; goto _LL17; _LL17: _temp16= _temp9.cyc_varargs;
goto _LL15; _LL15: _temp14= _temp9.rgn_po; goto _LL12; _LL12: _temp11= _temp9.attributes;
_temp13=&((( struct Cyc_Absyn_FnType_struct*) _temp3)->f1).attributes; goto _LL6;}
else{ goto _LL7;} _LL7: goto _LL8; _LL6: { struct Cyc_List_List* _temp28= 0;
goto _LL29; _LL29: for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ void* _temp30=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd;
int _temp46; _LL32: if(( unsigned int) _temp30 > 16u?*(( int*) _temp30) == Cyc_Absyn_Regparm_att:
0){ _LL47: _temp46=(( struct Cyc_Absyn_Regparm_att_struct*) _temp30)->f1; goto
_LL33;} else{ goto _LL34;} _LL34: if( _temp30 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp30 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL37;} else{ goto _LL38;} _LL38: if( _temp30 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL39;} else{ goto _LL40;} _LL40: if( _temp30 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL41;} else{ goto _LL42;} _LL42: if( _temp30 ==( void*) Cyc_Absyn_Const_att){
goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL33: goto _LL35; _LL35:
goto _LL37; _LL37: goto _LL39; _LL39: goto _LL41; _LL41: goto _LL43; _LL43:*
_temp13=({ struct Cyc_List_List* _temp48=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp48->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp48->tl=* _temp13; _temp48;}); continue; _LL45:
_temp28=({ struct Cyc_List_List* _temp49=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp49->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp49->tl= _temp28; _temp49;}); continue; _LL31:;}
return _temp28;} _LL8: return(( struct Cyc_List_List*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp50=( unsigned char*)"transfer_fn_type_atts";
struct _tagged_string _temp51; _temp51.curr= _temp50; _temp51.base= _temp50;
_temp51.last_plus_one= _temp50 + 22; _temp51;})); _LL4:;} struct _tuple3{ void*
f1; int f2; } ; static void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl*
vd, int check_var_init){ struct _tagged_string* v=(* vd->name).f2; void* t=(
void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{
void* _temp52=(* vd->name).f1; struct Cyc_List_List* _temp60; struct Cyc_List_List*
_temp62; _LL54: if(( unsigned int) _temp52 > 1u?*(( int*) _temp52) == Cyc_Absyn_Rel_n:
0){ _LL61: _temp60=(( struct Cyc_Absyn_Rel_n_struct*) _temp52)->f1; if( _temp60
== 0){ goto _LL55;} else{ goto _LL56;}} else{ goto _LL56;} _LL56: if((
unsigned int) _temp52 > 1u?*(( int*) _temp52) == Cyc_Absyn_Abs_n: 0){ _LL63:
_temp62=(( struct Cyc_Absyn_Abs_n_struct*) _temp52)->f1; if( _temp62 == 0){ goto
_LL57;} else{ goto _LL58;}} else{ goto _LL58;} _LL58: goto _LL59; _LL55: goto
_LL53; _LL57: goto _LL53; _LL59: Cyc_Tcutil_terr( loc,({ unsigned char* _temp64=(
unsigned char*)"qualified declarations are not implemented"; struct
_tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64; _temp65.last_plus_one=
_temp64 + 43; _temp65;})); return; _LL53:;}(* vd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp66=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp66[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp67; _temp67.tag= Cyc_Absyn_Abs_n;
_temp67.f1= te->ns; _temp67;}); _temp66;});{ void* _temp68= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Tqual _temp77; void* _temp79;
_LL70: if(( unsigned int) _temp68 > 4u?*(( int*) _temp68) == Cyc_Absyn_ArrayType:
0){ _LL80: _temp79=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp68)->f1;
goto _LL78; _LL78: _temp77=(( struct Cyc_Absyn_ArrayType_struct*) _temp68)->f2;
goto _LL76; _LL76: _temp75=(( struct Cyc_Absyn_ArrayType_struct*) _temp68)->f3;
if( _temp75 == 0){ goto _LL74;} else{ goto _LL72;}} else{ goto _LL72;} _LL74:
if( vd->initializer != 0){ goto _LL71;} else{ goto _LL72;} _LL72: goto _LL73;
_LL71:{ void* _temp81=( void*)(( struct Cyc_Absyn_Exp*) _check_null( vd->initializer))->r;
void* _temp91; struct _tagged_string _temp93; struct Cyc_List_List* _temp95;
struct Cyc_Core_Opt* _temp97; struct Cyc_List_List* _temp99; _LL83: if(*(( int*)
_temp81) == Cyc_Absyn_Const_e){ _LL92: _temp91=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp81)->f1; if(( unsigned int) _temp91 > 1u?*(( int*) _temp91) == Cyc_Absyn_String_c:
0){ _LL94: _temp93=(( struct Cyc_Absyn_String_c_struct*) _temp91)->f1; goto
_LL84;} else{ goto _LL85;}} else{ goto _LL85;} _LL85: if(*(( int*) _temp81) ==
Cyc_Absyn_UnresolvedMem_e){ _LL98: _temp97=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp81)->f1; goto _LL96; _LL96: _temp95=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp81)->f2; goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp81) ==
Cyc_Absyn_Array_e){ _LL100: _temp99=(( struct Cyc_Absyn_Array_e_struct*) _temp81)->f1;
goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL84: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp101=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp101[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp102; _temp102.tag= Cyc_Absyn_ArrayType; _temp102.f1=(
void*) _temp79; _temp102.f2= _temp77; _temp102.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(({
struct _tagged_string _temp103= _temp93;( unsigned int)( _temp103.last_plus_one
- _temp103.curr);}), 0); _temp102;}); _temp101;}))); goto _LL82; _LL86: _temp99=
_temp95; goto _LL88; _LL88: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp104=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp104[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp105; _temp105.tag= Cyc_Absyn_ArrayType;
_temp105.f1=( void*) _temp79; _temp105.f2= _temp77; _temp105.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp99), 0); _temp105;}); _temp104;}))); goto _LL82; _LL90: goto _LL82; _LL82:;}
goto _LL69; _LL73: goto _LL69; _LL69:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp106=(
unsigned char*)"extern declaration should not have initializer"; struct
_tagged_string _temp107; _temp107.curr= _temp106; _temp107.base= _temp106;
_temp107.last_plus_one= _temp106 + 47; _temp107;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp108=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int
_temp128; struct _tagged_string _temp130; _LL110: if(( unsigned int) _temp108 >
16u?*(( int*) _temp108) == Cyc_Absyn_Aligned_att: 0){ _LL129: _temp128=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp108)->f1; goto _LL111;} else{ goto _LL112;}
_LL112: if(( unsigned int) _temp108 > 16u?*(( int*) _temp108) == Cyc_Absyn_Section_att:
0){ _LL131: _temp130=(( struct Cyc_Absyn_Section_att_struct*) _temp108)->f1;
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp108 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp108 ==( void*) Cyc_Absyn_Shared_att){
goto _LL117;} else{ goto _LL118;} _LL118: if( _temp108 ==( void*) Cyc_Absyn_Unused_att){
goto _LL119;} else{ goto _LL120;} _LL120: if( _temp108 ==( void*) Cyc_Absyn_Weak_att){
goto _LL121;} else{ goto _LL122;} _LL122: if( _temp108 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL123;} else{ goto _LL124;} _LL124: if( _temp108 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL125;} else{ goto _LL126;} _LL126: goto _LL127; _LL111: continue; _LL113:
continue; _LL115: continue; _LL117: continue; _LL119: continue; _LL121:
continue; _LL123: continue; _LL125: continue; _LL127: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp132= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); struct _tagged_string
_temp133= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp132.last_plus_one - _temp132.curr, _temp132.curr, _temp133.last_plus_one -
_temp133.curr, _temp133.curr);})); goto _LL109; _LL109:;} if( vd->initializer ==
0){ if( check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp134= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp135= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp134.last_plus_one - _temp134.curr, _temp134.curr, _temp135.last_plus_one -
_temp135.curr, _temp135.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp136=(
struct Cyc_Absyn_Exp*) _check_null( vd->initializer); goto _LL137; _LL137: {
void* _temp138= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp136); goto
_LL139; _LL139: if( ! Cyc_Tcutil_coerce_assign( te, _temp136, t)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp140= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp141= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp142= Cyc_Absynpp_typ2string( _temp138); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp140.last_plus_one - _temp140.curr, _temp140.curr, _temp141.last_plus_one -
_temp141.curr, _temp141.curr, _temp142.last_plus_one - _temp142.curr, _temp142.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, _temp136)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp143=( unsigned char*)"initializer is not a constant expression";
struct _tagged_string _temp144; _temp144.curr= _temp143; _temp144.base= _temp143;
_temp144.last_plus_one= _temp143 + 41; _temp144;}));}}}} else{ for( 0; atts != 0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp145=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp165; int _temp167;
_LL147: if(( unsigned int) _temp145 > 16u?*(( int*) _temp145) == Cyc_Absyn_Regparm_att:
0){ _LL166: _temp165=(( struct Cyc_Absyn_Regparm_att_struct*) _temp145)->f1;
goto _LL148;} else{ goto _LL149;} _LL149: if( _temp145 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL150;} else{ goto _LL151;} _LL151: if( _temp145 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL152;} else{ goto _LL153;} _LL153: if( _temp145 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL154;} else{ goto _LL155;} _LL155: if( _temp145 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL156;} else{ goto _LL157;} _LL157: if( _temp145 ==( void*) Cyc_Absyn_Const_att){
goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int) _temp145 > 16u?*((
int*) _temp145) == Cyc_Absyn_Aligned_att: 0){ _LL168: _temp167=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp145)->f1; goto _LL160;} else{ goto _LL161;} _LL161: if( _temp145 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL162;} else{ goto _LL163;} _LL163: goto _LL164;
_LL148: goto _LL150; _LL150: goto _LL152; _LL152: goto _LL154; _LL154: goto
_LL156; _LL156: goto _LL158; _LL158:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp169=( unsigned char*)"tcVardecl: fn type attributes in function var decl";
struct _tagged_string _temp170; _temp170.curr= _temp169; _temp170.base= _temp169;
_temp170.last_plus_one= _temp169 + 51; _temp170;})); goto _LL146; _LL160: goto
_LL162; _LL162: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp171= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_temp171.last_plus_one - _temp171.curr, _temp171.curr);})); goto _LL146; _LL164:
continue; _LL146:;}}}{ struct _handler_cons _temp172; _push_handler(& _temp172);{
int _temp174= 0; if( setjmp( _temp172.handler)){ _temp174= 1;} if( ! _temp174){{
struct _tuple3* _temp175=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL176; _LL176: {
void* _temp177=(* _temp175).f1; void* _temp183; _LL179: if(*(( int*) _temp177)
== Cyc_Tcenv_VarRes){ _LL184: _temp183=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp177)->f1; goto _LL180;} else{ goto _LL181;} _LL181: goto _LL182; _LL180: {
struct Cyc_Absyn_Global_b_struct* _temp187=({ struct Cyc_Absyn_Global_b_struct*
_temp185=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp185[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp186; _temp186.tag= Cyc_Absyn_Global_b;
_temp186.f1= vd; _temp186;}); _temp185;}); goto _LL188; _LL188: { void* _temp189=
Cyc_Tcdecl_merge_binding( _temp183,( void*) _temp187, loc, Cyc_Tc_tc_msg); goto
_LL190; _LL190: if( _temp189 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL178;}
if( _temp189 == _temp183?(* _temp175).f2: 0){ goto _LL178;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp191=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp191->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp192=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp192[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp193; _temp193.tag= Cyc_Tcenv_VarRes; _temp193.f1=( void*) _temp189;
_temp193;}); _temp192;}); _temp191->f2= 1; _temp191;})); goto _LL178;}} _LL182:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp194=(
unsigned char*)"tcVardecl: a variable is being declared with a wrong name";
struct _tagged_string _temp195; _temp195.curr= _temp194; _temp195.base= _temp194;
_temp195.last_plus_one= _temp194 + 58; _temp195;})); goto _LL178; _LL178:;}};
_pop_handler();} else{ void* _temp173=( void*) _exn_thrown; void* _temp197=
_temp173; _LL199: if( _temp197 == Cyc_Dict_Absent){ goto _LL200;} else{ goto
_LL201;} _LL201: goto _LL202; _LL200: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp203=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp203->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp204=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp204[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp205; _temp205.tag= Cyc_Tcenv_VarRes;
_temp205.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp206=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp206[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp207; _temp207.tag= Cyc_Absyn_Global_b;
_temp207.f1= vd; _temp207;}); _temp206;})); _temp205;}); _temp204;}); _temp203->f2=
0; _temp203;})); goto _LL198; _LL202:( void) _throw( _temp197); _LL198:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp208=(*
fd->name).f1; struct Cyc_List_List* _temp216; struct Cyc_List_List* _temp218;
_LL210: if(( unsigned int) _temp208 > 1u?*(( int*) _temp208) == Cyc_Absyn_Rel_n:
0){ _LL217: _temp216=(( struct Cyc_Absyn_Rel_n_struct*) _temp208)->f1; if(
_temp216 == 0){ goto _LL211;} else{ goto _LL212;}} else{ goto _LL212;} _LL212:
if(( unsigned int) _temp208 > 1u?*(( int*) _temp208) == Cyc_Absyn_Abs_n: 0){
_LL219: _temp218=(( struct Cyc_Absyn_Abs_n_struct*) _temp208)->f1; goto _LL213;}
else{ goto _LL214;} _LL214: goto _LL215; _LL211: goto _LL209; _LL213:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp220=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp220[ 0]=({ struct
Cyc_Core_Impossible_struct _temp221; _temp221.tag= Cyc_Core_Impossible; _temp221.f1=({
unsigned char* _temp222=( unsigned char*)"tc: Abs_n in tcFndecl"; struct
_tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 22; _temp223;}); _temp221;}); _temp220;}));
_LL215: Cyc_Tcutil_terr( loc,({ unsigned char* _temp224=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 43; _temp225;})); return; _LL209:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp226=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp226[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp227; _temp227.tag= Cyc_Absyn_Abs_n; _temp227.f1= te->ns; _temp227;});
_temp226;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp228=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp236; _LL230: if(
_temp228 ==( void*) Cyc_Absyn_Packed_att){ goto _LL231;} else{ goto _LL232;}
_LL232: if(( unsigned int) _temp228 > 16u?*(( int*) _temp228) == Cyc_Absyn_Aligned_att:
0){ _LL237: _temp236=(( struct Cyc_Absyn_Aligned_att_struct*) _temp228)->f1;
goto _LL233;} else{ goto _LL234;} _LL234: goto _LL235; _LL231: goto _LL233;
_LL233: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp238= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); xprintf("bad attribute %.*s for function", _temp238.last_plus_one
- _temp238.curr, _temp238.curr);})); goto _LL229; _LL235: goto _LL229; _LL229:;}}{
struct _handler_cons _temp239; _push_handler(& _temp239);{ int _temp241= 0; if(
setjmp( _temp239.handler)){ _temp241= 1;} if( ! _temp241){{ struct _tuple3*
_temp242=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL243; _LL243: { void*
_temp244=(* _temp242).f1; void* _temp250; _LL246: if(*(( int*) _temp244) == Cyc_Tcenv_VarRes){
_LL251: _temp250=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp244)->f1; goto
_LL247;} else{ goto _LL248;} _LL248: goto _LL249; _LL247: { struct Cyc_Absyn_Funname_b_struct*
_temp254=({ struct Cyc_Absyn_Funname_b_struct* _temp252=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp253; _temp253.tag= Cyc_Absyn_Funname_b; _temp253.f1=
fd; _temp253;}); _temp252;}); goto _LL255; _LL255: { void* _temp256= Cyc_Tcdecl_merge_binding(
_temp250,( void*) _temp254, loc, Cyc_Tc_tc_msg); goto _LL257; _LL257: if(
_temp256 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL245;} if( _temp256 ==
_temp250?(* _temp242).f2: 0){ goto _LL245;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp258=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp258->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp259=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp259[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp260; _temp260.tag= Cyc_Tcenv_VarRes;
_temp260.f1=( void*) _temp256; _temp260;}); _temp259;}); _temp258->f2= 1;
_temp258;})); goto _LL245;}} _LL249:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp261=( unsigned char*)"tcFndecl: a variable is being declared with a wrong name";
struct _tagged_string _temp262; _temp262.curr= _temp261; _temp262.base= _temp261;
_temp262.last_plus_one= _temp261 + 57; _temp262;})); goto _LL245; _LL245:;}};
_pop_handler();} else{ void* _temp240=( void*) _exn_thrown; void* _temp264=
_temp240; _LL266: if( _temp264 == Cyc_Dict_Absent){ goto _LL267;} else{ goto
_LL268;} _LL268: goto _LL269; _LL267: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp270=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp270->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp271=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp271[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp272; _temp272.tag= Cyc_Tcenv_VarRes;
_temp272.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp273=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp273[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp274; _temp274.tag= Cyc_Absyn_Funname_b;
_temp274.f1= fd; _temp274;}); _temp273;})); _temp272;}); _temp271;}); _temp270->f2=
0; _temp270;})); goto _LL265; _LL269:( void) _throw( _temp264); _LL265:;}}} te->le=({
struct Cyc_Core_Opt* _temp275=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp275->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp276=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp276[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp277; _temp277.tag= Cyc_Tcenv_Outermost;
_temp277.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp277;}); _temp276;}));
_temp275;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp278=( unsigned char*)"function has goto statements to undefined labels";
struct _tagged_string _temp279; _temp279.curr= _temp278; _temp279.base= _temp278;
_temp279.last_plus_one= _temp278 + 49; _temp279;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp280=(* td->name).f1; struct Cyc_List_List*
_temp288; struct Cyc_List_List* _temp290; _LL282: if(( unsigned int) _temp280 >
1u?*(( int*) _temp280) == Cyc_Absyn_Rel_n: 0){ _LL289: _temp288=(( struct Cyc_Absyn_Rel_n_struct*)
_temp280)->f1; if( _temp288 == 0){ goto _LL283;} else{ goto _LL284;}} else{ goto
_LL284;} _LL284: if(( unsigned int) _temp280 > 1u?*(( int*) _temp280) == Cyc_Absyn_Abs_n:
0){ _LL291: _temp290=(( struct Cyc_Absyn_Abs_n_struct*) _temp280)->f1; if(
_temp290 == 0){ goto _LL285;} else{ goto _LL286;}} else{ goto _LL286;} _LL286:
goto _LL287; _LL283: goto _LL281; _LL285: goto _LL281; _LL287: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp292=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp293; _temp293.curr= _temp292; _temp293.base= _temp292;
_temp293.last_plus_one= _temp292 + 43; _temp293;})); return; _LL281:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp294=* v; xprintf("redeclaration of typedef %.*s", _temp294.last_plus_one -
_temp294.curr, _temp294.curr);})); return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp295=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp295[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp296; _temp296.tag= Cyc_Absyn_Abs_n;
_temp296.f1= te->ns; _temp296;}); _temp295;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp297= _new_region();
struct _RegionHandle* uprev_rgn=& _temp297; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp298= fields; goto
_LL299; _LL299: for( 0; _temp298 != 0; _temp298=(( struct Cyc_List_List*)
_check_null( _temp298))->tl){ struct Cyc_Absyn_Structfield _temp302; struct Cyc_List_List*
_temp303; struct Cyc_Core_Opt* _temp305; void* _temp307; struct Cyc_Absyn_Tqual
_temp309; struct _tagged_string* _temp311; struct Cyc_Absyn_Structfield*
_temp300=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp298))->hd; _temp302=* _temp300; _LL312: _temp311= _temp302.name; goto
_LL310; _LL310: _temp309= _temp302.tq; goto _LL308; _LL308: _temp307=( void*)
_temp302.type; goto _LL306; _LL306: _temp305= _temp302.width; goto _LL304;
_LL304: _temp303= _temp302.attributes; goto _LL301; _LL301: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp311)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp313=* _temp311; struct _tagged_string _temp314= obj; xprintf("duplicate field %.*s in %.*s",
_temp313.last_plus_one - _temp313.curr, _temp313.curr, _temp314.last_plus_one -
_temp314.curr, _temp314.curr);}));} if( Cyc_String_strcmp(* _temp311,({
unsigned char* _temp315=( unsigned char*)""; struct _tagged_string _temp316;
_temp316.curr= _temp315; _temp316.base= _temp315; _temp316.last_plus_one=
_temp315 + 1; _temp316;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp317=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp317->hd=( void*) _temp311; _temp317->tl= prev_fields; _temp317;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp307); if( _temp305 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp305))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp318=* _temp311; xprintf("bitfield %.*s does not have constant width",
_temp318.last_plus_one - _temp318.curr, _temp318.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp305))->v);}{
void* _temp319= Cyc_Tcutil_compress( _temp307); void* _temp325; void* _temp327;
_LL321: if(( unsigned int) _temp319 > 4u?*(( int*) _temp319) == Cyc_Absyn_IntType:
0){ _LL328: _temp327=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp319)->f1;
goto _LL326; _LL326: _temp325=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp319)->f2; goto _LL322;} else{ goto _LL323;} _LL323: goto _LL324; _LL322:{
void* _temp329= _temp325; _LL331: if( _temp329 ==( void*) Cyc_Absyn_B1){ goto
_LL332;} else{ goto _LL333;} _LL333: if( _temp329 ==( void*) Cyc_Absyn_B2){ goto
_LL334;} else{ goto _LL335;} _LL335: if( _temp329 ==( void*) Cyc_Absyn_B4){ goto
_LL336;} else{ goto _LL337;} _LL337: if( _temp329 ==( void*) Cyc_Absyn_B8){ goto
_LL338;} else{ goto _LL330;} _LL332: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp339=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 26; _temp340;}));} goto _LL330; _LL334: if( w
> 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp341=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 26; _temp342;}));} goto _LL330; _LL336: if( w
> 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp343=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 26; _temp344;}));} goto _LL330; _LL338: if( w
> 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp345=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 26; _temp346;}));} goto _LL330; _LL330:;}
goto _LL320; _LL324: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp347=* _temp311; struct _tagged_string _temp348= Cyc_Absynpp_typ2string(
_temp307); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp347.last_plus_one - _temp347.curr, _temp347.curr, _temp348.last_plus_one -
_temp348.curr, _temp348.curr);})); goto _LL320; _LL320:;}}}}; _pop_region(&
_temp297);} static void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp349=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL350; _LL350: if( sd->name == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp351= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp351.last_plus_one - _temp351.curr, _temp351.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp352=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp360; _LL354: if( _temp352 ==( void*) Cyc_Absyn_Packed_att){
goto _LL355;} else{ goto _LL356;} _LL356: if(( unsigned int) _temp352 > 16u?*((
int*) _temp352) == Cyc_Absyn_Aligned_att: 0){ _LL361: _temp360=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp352)->f1; goto _LL357;} else{ goto _LL358;} _LL358: goto _LL359; _LL355:
continue; _LL357: continue; _LL359: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp362= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp363= obj; struct
_tagged_string _temp364=* _temp349; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp362.last_plus_one - _temp362.curr, _temp362.curr, _temp363.last_plus_one -
_temp363.curr, _temp363.curr, _temp364.last_plus_one - _temp364.curr, _temp364.curr);}));
goto _LL353; _LL353:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp365=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
goto _LL366; _LL366: { void* _temp367=( void*) _temp365->v; void* _temp375;
_LL369: if( _temp367 ==( void*) Cyc_Absyn_No_constr){ goto _LL370;} else{ goto
_LL371;} _LL371: if(( unsigned int) _temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Eq_constr:
0){ _LL376: _temp375=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp367)->f1;
if( _temp375 ==( void*) Cyc_Absyn_MemKind){ goto _LL372;} else{ goto _LL373;}}
else{ goto _LL373;} _LL373: goto _LL374; _LL370:( void*)( _temp365->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp377=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp377[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp378; _temp378.tag= Cyc_Absyn_Eq_constr; _temp378.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp378;}); _temp377;}))); continue; _LL372:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp379=
obj; struct _tagged_string _temp380=* _temp349; struct _tagged_string _temp381=*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name;
xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M", _temp379.last_plus_one
- _temp379.curr, _temp379.curr, _temp380.last_plus_one - _temp380.curr, _temp380.curr,
_temp381.last_plus_one - _temp381.curr, _temp381.curr);})); continue; _LL374:
continue; _LL368:;}}}{ void* _temp382=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp390; struct Cyc_List_List*
_temp392; _LL384: if(( unsigned int) _temp382 > 1u?*(( int*) _temp382) == Cyc_Absyn_Rel_n:
0){ _LL391: _temp390=(( struct Cyc_Absyn_Rel_n_struct*) _temp382)->f1; if(
_temp390 == 0){ goto _LL385;} else{ goto _LL386;}} else{ goto _LL386;} _LL386:
if(( unsigned int) _temp382 > 1u?*(( int*) _temp382) == Cyc_Absyn_Abs_n: 0){
_LL393: _temp392=(( struct Cyc_Absyn_Abs_n_struct*) _temp382)->f1; if( _temp392
== 0){ goto _LL387;} else{ goto _LL388;}} else{ goto _LL388;} _LL388: goto
_LL389; _LL385: goto _LL383; _LL387: goto _LL383; _LL389: Cyc_Tcutil_terr( loc,({
unsigned char* _temp394=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 43; _temp395;})); return; _LL383:;}(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp396=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp396[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp397; _temp397.tag= Cyc_Absyn_Abs_n; _temp397.f1= te->ns; _temp397;});
_temp396;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* _temp398=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; goto _LL399; _LL399: { const unsigned char*
_temp400="struct"; goto _LL401; _LL401: Cyc_Tc_tcStructOrUniondecl( te, ge,({
unsigned char* _temp402=( unsigned char*) _temp400; struct _tagged_string
_temp403; _temp403.curr= _temp402; _temp403.base= _temp402; _temp403.last_plus_one=
_temp402 + 7; _temp403;}), loc, sd);{ struct Cyc_List_List* _temp404= sd->tvs;
goto _LL405; _LL405:{ struct _tuple4 _temp407=({ struct _tuple4 _temp406;
_temp406.f1= sd->fields; _temp406.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp398);
_temp406;}); struct Cyc_Core_Opt* _temp417; struct Cyc_Core_Opt* _temp419;
struct Cyc_Core_Opt* _temp421; struct Cyc_Core_Opt* _temp423; struct Cyc_Core_Opt
_temp425; struct Cyc_List_List* _temp426; struct Cyc_Core_Opt* _temp428; struct
Cyc_Core_Opt _temp430; struct Cyc_Absyn_Structdecl** _temp431; struct Cyc_Core_Opt*
_temp433; struct Cyc_Core_Opt _temp435; struct Cyc_List_List* _temp436; struct
Cyc_Core_Opt* _temp438; struct Cyc_Core_Opt _temp440; struct Cyc_Absyn_Structdecl**
_temp441; struct Cyc_Core_Opt* _temp443; _LL409: _LL420: _temp419= _temp407.f1;
if( _temp419 == 0){ goto _LL418;} else{ goto _LL411;} _LL418: _temp417= _temp407.f2;
if( _temp417 == 0){ goto _LL410;} else{ goto _LL411;} _LL411: _LL424: _temp423=
_temp407.f1; if( _temp423 == 0){ goto _LL413;} else{ _temp425=* _temp423; _LL427:
_temp426=( struct Cyc_List_List*) _temp425.v; goto _LL422;} _LL422: _temp421=
_temp407.f2; if( _temp421 == 0){ goto _LL412;} else{ goto _LL413;} _LL413:
_LL434: _temp433= _temp407.f1; if( _temp433 == 0){ goto _LL415;} else{ _temp435=*
_temp433; _LL437: _temp436=( struct Cyc_List_List*) _temp435.v; goto _LL429;}
_LL429: _temp428= _temp407.f2; if( _temp428 == 0){ goto _LL415;} else{ _temp430=*
_temp428; _LL432: _temp431=( struct Cyc_Absyn_Structdecl**) _temp430.v; goto
_LL414;} _LL415: _LL444: _temp443= _temp407.f1; if( _temp443 == 0){ goto _LL439;}
else{ goto _LL408;} _LL439: _temp438= _temp407.f2; if( _temp438 == 0){ goto
_LL408;} else{ _temp440=* _temp438; _LL442: _temp441=( struct Cyc_Absyn_Structdecl**)
_temp440.v; goto _LL416;} _LL410: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp398,({ struct Cyc_Absyn_Structdecl**
_temp445=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp445[ 0]= sd; _temp445;})); goto _LL408; _LL412: { struct Cyc_Absyn_Structdecl**
_temp448=({ struct Cyc_Absyn_Structdecl** _temp446=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp446[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp447=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp447->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp447->name= sd->name;
_temp447->tvs= _temp404; _temp447->fields= 0; _temp447->attributes= 0; _temp447;});
_temp446;}); goto _LL449; _LL449: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp398, _temp448); Cyc_Tc_tcStructFields(
te, ge, loc,({ unsigned char* _temp450=( unsigned char*) _temp400; struct
_tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 7; _temp451;}), _temp426, _temp404);*
_temp448= sd; goto _LL408;} _LL414: { struct Cyc_Absyn_Structdecl* _temp452=*
_temp431; goto _LL453; _LL453:* _temp431=({ struct Cyc_Absyn_Structdecl*
_temp454=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp454->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp454->name= sd->name;
_temp454->tvs= _temp404; _temp454->fields= 0; _temp454->attributes= 0; _temp454;});
Cyc_Tc_tcStructFields( te, ge, loc,({ unsigned char* _temp455=( unsigned char*)
_temp400; struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base=
_temp455; _temp456.last_plus_one= _temp455 + 7; _temp456;}), _temp436, _temp404);*
_temp431= _temp452; _temp441= _temp431; goto _LL416;} _LL416: { struct Cyc_Absyn_Structdecl*
_temp457= Cyc_Tcdecl_merge_structdecl(* _temp441, sd, loc, Cyc_Tc_tc_msg); goto
_LL458; _LL458: if( _temp457 == 0){ return;} else{* _temp441=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp457); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp457);
goto _LL408;}} _LL408:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp398,({ struct _tuple3* _temp459=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp459->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp460=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp460[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp461; _temp461.tag= Cyc_Tcenv_StructRes;
_temp461.f1= sd; _temp461;}); _temp460;}); _temp459->f2= 1; _temp459;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* _temp462=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)).f2; goto _LL463; _LL463: { const unsigned char*
_temp464="union"; goto _LL465; _LL465: Cyc_Tc_tcStructOrUniondecl( te, ge,({
unsigned char* _temp466=( unsigned char*) _temp464; struct _tagged_string
_temp467; _temp467.curr= _temp466; _temp467.base= _temp466; _temp467.last_plus_one=
_temp466 + 6; _temp467;}), loc,( struct Cyc_Absyn_Structdecl*) ud);{ struct Cyc_List_List*
_temp468= ud->tvs; goto _LL469; _LL469: { struct _tuple4 _temp471=({ struct
_tuple4 _temp470; _temp470.f1= ud->fields; _temp470.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp462); _temp470;}); struct Cyc_Core_Opt* _temp481; struct Cyc_Core_Opt*
_temp483; struct Cyc_Core_Opt* _temp485; struct Cyc_Core_Opt* _temp487; struct
Cyc_Core_Opt _temp489; struct Cyc_List_List* _temp490; struct Cyc_Core_Opt*
_temp492; struct Cyc_Core_Opt _temp494; struct Cyc_Absyn_Uniondecl** _temp495;
struct Cyc_Core_Opt* _temp497; struct Cyc_Core_Opt _temp499; struct Cyc_List_List*
_temp500; struct Cyc_Core_Opt* _temp502; struct Cyc_Core_Opt _temp504; struct
Cyc_Absyn_Uniondecl** _temp505; struct Cyc_Core_Opt* _temp507; _LL473: _LL484:
_temp483= _temp471.f1; if( _temp483 == 0){ goto _LL482;} else{ goto _LL475;}
_LL482: _temp481= _temp471.f2; if( _temp481 == 0){ goto _LL474;} else{ goto
_LL475;} _LL475: _LL488: _temp487= _temp471.f1; if( _temp487 == 0){ goto _LL477;}
else{ _temp489=* _temp487; _LL491: _temp490=( struct Cyc_List_List*) _temp489.v;
goto _LL486;} _LL486: _temp485= _temp471.f2; if( _temp485 == 0){ goto _LL476;}
else{ goto _LL477;} _LL477: _LL498: _temp497= _temp471.f1; if( _temp497 == 0){
goto _LL479;} else{ _temp499=* _temp497; _LL501: _temp500=( struct Cyc_List_List*)
_temp499.v; goto _LL493;} _LL493: _temp492= _temp471.f2; if( _temp492 == 0){
goto _LL479;} else{ _temp494=* _temp492; _LL496: _temp495=( struct Cyc_Absyn_Uniondecl**)
_temp494.v; goto _LL478;} _LL479: _LL508: _temp507= _temp471.f1; if( _temp507 ==
0){ goto _LL503;} else{ goto _LL472;} _LL503: _temp502= _temp471.f2; if(
_temp502 == 0){ goto _LL472;} else{ _temp504=* _temp502; _LL506: _temp505=(
struct Cyc_Absyn_Uniondecl**) _temp504.v; goto _LL480;} _LL474: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp462,({
struct Cyc_Absyn_Uniondecl** _temp509=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp509[ 0]= ud; _temp509;})); goto
_LL472; _LL476: { struct Cyc_Absyn_Uniondecl** _temp512=({ struct Cyc_Absyn_Uniondecl**
_temp510=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp510[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp511=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp511->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp511->name= ud->name; _temp511->tvs= _temp468; _temp511->fields=
0; _temp511->attributes= ud->attributes; _temp511;}); _temp510;}); goto _LL513;
_LL513: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp462, _temp512); Cyc_Tc_tcStructFields( te, ge, loc,({
unsigned char* _temp514=( unsigned char*) _temp464; struct _tagged_string
_temp515; _temp515.curr= _temp514; _temp515.base= _temp514; _temp515.last_plus_one=
_temp514 + 6; _temp515;}), _temp490, _temp468);{ struct Cyc_List_List* f=
_temp490; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(
! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->type)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp516=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->name; struct _tagged_string _temp517=* _temp462; struct
_tagged_string _temp518= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type); xprintf("field %.*s of union %.*s has type %.*s which is not `bits-only'",
_temp516.last_plus_one - _temp516.curr, _temp516.curr, _temp517.last_plus_one -
_temp517.curr, _temp517.curr, _temp518.last_plus_one - _temp518.curr, _temp518.curr);}));}}}*
_temp512= ud; goto _LL472;} _LL478: { struct Cyc_Absyn_Uniondecl* _temp519=*
_temp495; goto _LL520; _LL520:* _temp495=({ struct Cyc_Absyn_Uniondecl* _temp521=(
struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp521->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp521->name= ud->name;
_temp521->tvs= _temp468; _temp521->fields= 0; _temp521->attributes= ud->attributes;
_temp521;}); Cyc_Tc_tcStructFields( te, ge, loc,({ unsigned char* _temp522=(
unsigned char*) _temp464; struct _tagged_string _temp523; _temp523.curr=
_temp522; _temp523.base= _temp522; _temp523.last_plus_one= _temp522 + 6;
_temp523;}), _temp500, _temp468);* _temp495= _temp519; _temp505= _temp495; goto
_LL480;} _LL480: { struct Cyc_Absyn_Uniondecl* _temp524= Cyc_Tcdecl_merge_uniondecl(*
_temp505, ud, loc, Cyc_Tc_tc_msg); goto _LL525; _LL525: if( _temp524 == 0){
return;} else{* _temp505=( struct Cyc_Absyn_Uniondecl*) _check_null( _temp524);
goto _LL472;}} _LL472:;}}}} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp526= fields; goto _LL527; _LL527: for( 0; _temp526 != 0;
_temp526=(( struct Cyc_List_List*) _check_null( _temp526))->tl){ struct Cyc_Absyn_Tunionfield*
_temp528=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp526))->hd; goto _LL529; _LL529:{ struct Cyc_List_List* tvs= _temp528->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Tvar* _temp530=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd; goto _LL531; _LL531: { struct Cyc_Absyn_Conref* _temp532=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp530->kind); goto _LL533; _LL533: { void* _temp534=( void*) _temp532->v;
void* _temp542; _LL536: if( _temp534 ==( void*) Cyc_Absyn_No_constr){ goto
_LL537;} else{ goto _LL538;} _LL538: if(( unsigned int) _temp534 > 1u?*(( int*)
_temp534) == Cyc_Absyn_Eq_constr: 0){ _LL543: _temp542=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp534)->f1; if( _temp542 ==( void*) Cyc_Absyn_MemKind){ goto _LL539;} else{
goto _LL540;}} else{ goto _LL540;} _LL540: goto _LL541; _LL537:( void*)(
_temp532->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp544=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp544[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp545; _temp545.tag= Cyc_Absyn_Eq_constr;
_temp545.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp545;}); _temp544;})));
goto _LL535; _LL539: Cyc_Tcutil_terr( _temp528->loc,( struct _tagged_string)({
struct _tagged_string _temp546=*(* _temp528->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp546.last_plus_one - _temp546.curr, _temp546.curr);})); goto _LL535; _LL541:
goto _LL535; _LL535:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp528->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp528->tvs);{ struct Cyc_List_List* typs= _temp528->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp528->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp547=(*
_temp528->name).f1; struct Cyc_List_List* _temp557; struct Cyc_List_List*
_temp559; struct Cyc_List_List* _temp561; _LL549: if(( unsigned int) _temp547 >
1u?*(( int*) _temp547) == Cyc_Absyn_Rel_n: 0){ _LL558: _temp557=(( struct Cyc_Absyn_Rel_n_struct*)
_temp547)->f1; if( _temp557 == 0){ goto _LL550;} else{ goto _LL551;}} else{ goto
_LL551;} _LL551: if(( unsigned int) _temp547 > 1u?*(( int*) _temp547) == Cyc_Absyn_Rel_n:
0){ _LL560: _temp559=(( struct Cyc_Absyn_Rel_n_struct*) _temp547)->f1; goto
_LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp547 > 1u?*(( int*)
_temp547) == Cyc_Absyn_Abs_n: 0){ _LL562: _temp561=(( struct Cyc_Absyn_Abs_n_struct*)
_temp547)->f1; goto _LL554;} else{ goto _LL555;} _LL555: if( _temp547 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL556;} else{ goto _LL548;} _LL550: if( is_xtunion){(*
_temp528->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp563=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp563[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp564; _temp564.tag= Cyc_Absyn_Abs_n;
_temp564.f1= te->ns; _temp564;}); _temp563;});} else{(* _temp528->name).f1=(*
name).f1;} goto _LL548; _LL552: Cyc_Tcutil_terr( _temp528->loc,({ unsigned char*
_temp565=( unsigned char*)"qualified declarations are not allowed"; struct
_tagged_string _temp566; _temp566.curr= _temp565; _temp566.base= _temp565;
_temp566.last_plus_one= _temp565 + 39; _temp566;})); goto _LL548; _LL554: goto
_LL548; _LL556:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp567=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp567[ 0]=({ struct Cyc_Core_Impossible_struct _temp568; _temp568.tag= Cyc_Core_Impossible;
_temp568.f1=({ unsigned char* _temp569=( unsigned char*)"tcTunionFields: Loc_n";
struct _tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 22; _temp570;}); _temp568;}); _temp567;}));
goto _LL548; _LL548:;}}}}{ struct Cyc_List_List* fields2; if( is_xtunion){ int
_temp571= 1; goto _LL572; _LL572: { struct Cyc_List_List* _temp573= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp571,(* name).f2, loc, Cyc_Tc_tc_msg); goto _LL574; _LL574: if(
_temp571){ fields2= _temp573;} else{ fields2= 0;}}} else{ struct _RegionHandle
_temp575= _new_region(); struct _RegionHandle* uprev_rgn=& _temp575;
_push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List*
fs= fields; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct Cyc_Absyn_Tunionfield* _temp576=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( fs))->hd; goto _LL577; _LL577: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
_temp576->name).f2)){ Cyc_Tcutil_terr( _temp576->loc,( struct _tagged_string)({
struct _tagged_string _temp578=*(* _temp576->name).f2; struct _tagged_string
_temp579= obj; xprintf("duplicate field name %.*s in %.*s", _temp578.last_plus_one
- _temp578.curr, _temp578.curr, _temp579.last_plus_one - _temp579.curr, _temp579.curr);}));}
else{ prev_fields=({ struct Cyc_List_List* _temp580=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp580->hd=( void*)(*
_temp576->name).f2; _temp580->tl= prev_fields; _temp580;});} if(( void*)
_temp576->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ struct _tagged_string _temp581=*(* _temp576->name).f2; xprintf("ignoring scope of field %.*s",
_temp581.last_plus_one - _temp581.curr, _temp581.curr);}));( void*)( _temp576->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp575);}{
struct Cyc_List_List* _temp582= fields; goto _LL583; _LL583: for( 0; _temp582 !=
0; _temp582=(( struct Cyc_List_List*) _check_null( _temp582))->tl){ struct Cyc_Absyn_Tunionfield*
_temp584=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp582))->hd; goto _LL585; _LL585: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp584->name).f2,({ struct _tuple3* _temp586=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp586->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp587=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp587[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp588; _temp588.tag= Cyc_Tcenv_TunionRes; _temp588.f1=
tudres; _temp588.f2= _temp584; _temp588;}); _temp587;}); _temp586->f2= 1;
_temp586;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=({ unsigned char* _temp589=( unsigned char*)"xtunion";
struct _tagged_string _temp590; _temp590.curr= _temp589; _temp590.base= _temp589;
_temp590.last_plus_one= _temp589 + 8; _temp590;});} else{ obj=({ unsigned char*
_temp591=( unsigned char*)"tunion"; struct _tagged_string _temp592; _temp592.curr=
_temp591; _temp592.base= _temp591; _temp592.last_plus_one= _temp591 + 7;
_temp592;});}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List* tvs2=
tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->kind); void* _temp593=( void*) c->v; void* _temp601;
_LL595: if( _temp593 ==( void*) Cyc_Absyn_No_constr){ goto _LL596;} else{ goto
_LL597;} _LL597: if(( unsigned int) _temp593 > 1u?*(( int*) _temp593) == Cyc_Absyn_Eq_constr:
0){ _LL602: _temp601=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp593)->f1;
if( _temp601 ==( void*) Cyc_Absyn_MemKind){ goto _LL598;} else{ goto _LL599;}}
else{ goto _LL599;} _LL599: goto _LL600; _LL596:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp603=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp603[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp604; _temp604.tag= Cyc_Absyn_Eq_constr; _temp604.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp604;}); _temp603;}))); goto _LL594;
_LL598: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp605= obj; struct _tagged_string _temp606=* v; struct _tagged_string
_temp607=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name;
xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M", _temp605.last_plus_one
- _temp605.curr, _temp605.curr, _temp606.last_plus_one - _temp606.curr, _temp606.curr,
_temp607.last_plus_one - _temp607.curr, _temp607.curr);})); goto _LL594; _LL600:
goto _LL594; _LL594:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp608; _push_handler(& _temp608);{ int _temp610= 0; if( setjmp( _temp608.handler)){
_temp610= 1;} if( ! _temp610){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp609=( void*) _exn_thrown; void*
_temp612= _temp609; _LL614: if( _temp612 == Cyc_Dict_Absent){ goto _LL615;}
else{ goto _LL616;} _LL616: goto _LL617; _LL615: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp618= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp618.last_plus_one - _temp618.curr, _temp618.curr);})); return; _LL617:(
void) _throw( _temp612); _LL613:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp619=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp619[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp620; _temp620.tag= Cyc_Absyn_Abs_n;
_temp620.f1= te->ns; _temp620;}); _temp619;});}} else{{ void* _temp621=(* tud->name).f1;
struct Cyc_List_List* _temp629; struct Cyc_List_List* _temp631; _LL623: if((
unsigned int) _temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_Rel_n: 0){ _LL630:
_temp629=(( struct Cyc_Absyn_Rel_n_struct*) _temp621)->f1; if( _temp629 == 0){
goto _LL624;} else{ goto _LL625;}} else{ goto _LL625;} _LL625: if(( unsigned int)
_temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_Abs_n: 0){ _LL632: _temp631=((
struct Cyc_Absyn_Abs_n_struct*) _temp621)->f1; goto _LL626;} else{ goto _LL627;}
_LL627: goto _LL628; _LL624:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp633=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp633[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp634; _temp634.tag= Cyc_Absyn_Abs_n;
_temp634.f1= te->ns; _temp634;}); _temp633;}); goto _LL622; _LL626: goto _LL628;
_LL628: Cyc_Tcutil_terr( loc,({ unsigned char* _temp635=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 43; _temp636;})); return; _LL622:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp638=({ struct
_tuple4 _temp637; _temp637.f1= tud->fields; _temp637.f2= tud_opt; _temp637;});
struct Cyc_Core_Opt* _temp648; struct Cyc_Core_Opt* _temp650; struct Cyc_Core_Opt*
_temp652; struct Cyc_Core_Opt* _temp654; struct Cyc_Core_Opt _temp656; struct
Cyc_List_List* _temp657; struct Cyc_List_List** _temp659; struct Cyc_Core_Opt*
_temp660; struct Cyc_Core_Opt _temp662; struct Cyc_Absyn_Tuniondecl** _temp663;
struct Cyc_Core_Opt* _temp665; struct Cyc_Core_Opt _temp667; struct Cyc_List_List*
_temp668; struct Cyc_List_List** _temp670; struct Cyc_Core_Opt* _temp671; struct
Cyc_Core_Opt _temp673; struct Cyc_Absyn_Tuniondecl** _temp674; struct Cyc_Core_Opt*
_temp676; _LL640: _LL651: _temp650= _temp638.f1; if( _temp650 == 0){ goto _LL649;}
else{ goto _LL642;} _LL649: _temp648= _temp638.f2; if( _temp648 == 0){ goto
_LL641;} else{ goto _LL642;} _LL642: _LL655: _temp654= _temp638.f1; if( _temp654
== 0){ goto _LL644;} else{ _temp656=* _temp654; _LL658: _temp657=( struct Cyc_List_List*)
_temp656.v; _temp659=&(* _temp638.f1).v; goto _LL653;} _LL653: _temp652=
_temp638.f2; if( _temp652 == 0){ goto _LL643;} else{ goto _LL644;} _LL644:
_LL666: _temp665= _temp638.f1; if( _temp665 == 0){ goto _LL646;} else{ _temp667=*
_temp665; _LL669: _temp668=( struct Cyc_List_List*) _temp667.v; _temp670=&(*
_temp638.f1).v; goto _LL661;} _LL661: _temp660= _temp638.f2; if( _temp660 == 0){
goto _LL646;} else{ _temp662=* _temp660; _LL664: _temp663=( struct Cyc_Absyn_Tuniondecl**)
_temp662.v; goto _LL645;} _LL646: _LL677: _temp676= _temp638.f1; if( _temp676 ==
0){ goto _LL672;} else{ goto _LL639;} _LL672: _temp671= _temp638.f2; if(
_temp671 == 0){ goto _LL639;} else{ _temp673=* _temp671; _LL675: _temp674=(
struct Cyc_Absyn_Tuniondecl**) _temp673.v; goto _LL647;} _LL641: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp678=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp678[ 0]= tud; _temp678;}));
goto _LL639; _LL643: { struct Cyc_Absyn_Tuniondecl** _temp681=({ struct Cyc_Absyn_Tuniondecl**
_temp679=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp679[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp680=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp680->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp680->name= tud->name; _temp680->tvs= tvs; _temp680->fields=
0; _temp680->is_xtunion= tud->is_xtunion; _temp680;}); _temp679;}); goto _LL682;
_LL682: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp681);* _temp659= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp659, tvs, tud);* _temp681= tud; goto
_LL639;} _LL645: { struct Cyc_Absyn_Tuniondecl* _temp683=* _temp663; goto _LL684;
_LL684:* _temp663=({ struct Cyc_Absyn_Tuniondecl* _temp685=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp685->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp685->name= tud->name; _temp685->tvs= tvs; _temp685->fields=
0; _temp685->is_xtunion= tud->is_xtunion; _temp685;});* _temp670= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp670, tvs, tud);* _temp663=
_temp683; _temp674= _temp663; goto _LL647;} _LL647: { struct Cyc_Absyn_Tuniondecl*
_temp686= Cyc_Tcdecl_merge_tuniondecl(* _temp674, tud, loc, Cyc_Tc_tc_msg); goto
_LL687; _LL687: if( _temp686 == 0){ return;} else{* _temp674=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp686); goto _LL639;}} _LL639:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp688=(* ed->name).f1; struct Cyc_List_List* _temp696; struct Cyc_List_List*
_temp698; _LL690: if(( unsigned int) _temp688 > 1u?*(( int*) _temp688) == Cyc_Absyn_Rel_n:
0){ _LL697: _temp696=(( struct Cyc_Absyn_Rel_n_struct*) _temp688)->f1; if(
_temp696 == 0){ goto _LL691;} else{ goto _LL692;}} else{ goto _LL692;} _LL692:
if(( unsigned int) _temp688 > 1u?*(( int*) _temp688) == Cyc_Absyn_Abs_n: 0){
_LL699: _temp698=(( struct Cyc_Absyn_Abs_n_struct*) _temp688)->f1; if( _temp698
== 0){ goto _LL693;} else{ goto _LL694;}} else{ goto _LL694;} _LL694: goto
_LL695; _LL691: goto _LL689; _LL693: goto _LL689; _LL695: Cyc_Tcutil_terr( loc,({
unsigned char* _temp700=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 43; _temp701;})); return; _LL689:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp702=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp702[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp703; _temp703.tag= Cyc_Absyn_Abs_n; _temp703.f1= te->ns; _temp703;});
_temp702;}); if( ed->fields != 0){ struct _RegionHandle _temp704= _new_region();
struct _RegionHandle* uprev_rgn=& _temp704; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp705=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL706; _LL706: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp705->name).f2)){ Cyc_Tcutil_terr(
_temp705->loc,( struct _tagged_string)({ struct _tagged_string _temp707=*(*
_temp705->name).f2; xprintf("duplicate field name %.*s", _temp707.last_plus_one
- _temp707.curr, _temp707.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp708=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp708->hd=( void*)(* _temp705->name).f2; _temp708->tl= prev_fields; _temp708;});}
if( _temp705->tag == 0){ _temp705->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp705->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp705->tag))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp709=* v; struct _tagged_string _temp710=*(* _temp705->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp709.last_plus_one
- _temp709.curr, _temp709.curr, _temp710.last_plus_one - _temp710.curr, _temp710.curr);}));}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp705->tag)); tag_count= t1 + 1;(* _temp705->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp711=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp711[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp712; _temp712.tag= Cyc_Absyn_Abs_n; _temp712.f1= te->ns; _temp712;});
_temp711;});}}}; _pop_region(& _temp704);}{ struct _handler_cons _temp713;
_push_handler(& _temp713);{ int _temp715= 0; if( setjmp( _temp713.handler)){
_temp715= 1;} if( ! _temp715){{ struct Cyc_Absyn_Enumdecl** _temp716=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->enumdecls, v); goto _LL717; _LL717: { struct Cyc_Absyn_Enumdecl*
_temp718= Cyc_Tcdecl_merge_enumdecl(* _temp716, ed, loc, Cyc_Tc_tc_msg); goto
_LL719; _LL719: if( _temp718 == 0){ _npop_handler( 0u); return;}* _temp716=(
struct Cyc_Absyn_Enumdecl*) _check_null( _temp718);}}; _pop_handler();} else{
void* _temp714=( void*) _exn_thrown; void* _temp721= _temp714; _LL723: if(
_temp721 == Cyc_Dict_Absent){ goto _LL724;} else{ goto _LL725;} _LL725: goto
_LL726; _LL724: { struct Cyc_Absyn_Enumdecl** _temp728=({ struct Cyc_Absyn_Enumdecl**
_temp727=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*));
_temp727[ 0]= ed; _temp727;}); goto _LL729; _LL729: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl**
data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp728); goto _LL722;} _LL726:(
void) _throw( _temp721); _LL722:;}}} if( ed->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp730=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL731; _LL731: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp730->name).f2,({ struct _tuple3* _temp732=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp732->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp733=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp733[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp734; _temp734.tag= Cyc_Tcenv_EnumRes;
_temp734.f1= ed; _temp734.f2= _temp730; _temp734;}); _temp733;}); _temp732->f2=
1; _temp732;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp735= sc; _LL737: if( _temp735 ==( void*) Cyc_Absyn_Static){
goto _LL738;} else{ goto _LL739;} _LL739: if( _temp735 ==( void*) Cyc_Absyn_Abstract){
goto _LL740;} else{ goto _LL741;} _LL741: if( _temp735 ==( void*) Cyc_Absyn_Public){
goto _LL742;} else{ goto _LL743;} _LL743: if( _temp735 ==( void*) Cyc_Absyn_Extern){
goto _LL744;} else{ goto _LL745;} _LL745: if( _temp735 ==( void*) Cyc_Absyn_ExternC){
goto _LL746;} else{ goto _LL736;} _LL738: Cyc_Tcutil_warn( loc,({ unsigned char*
_temp747=( unsigned char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp748; _temp748.curr= _temp747; _temp748.base= _temp747;
_temp748.last_plus_one= _temp747 + 44; _temp748;})); return 0; _LL740: Cyc_Tcutil_warn(
loc,({ unsigned char* _temp749=( unsigned char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp750; _temp750.curr= _temp749; _temp750.base= _temp749;
_temp750.last_plus_one= _temp749 + 46; _temp750;})); return 0; _LL742: return 1;
_LL744: return 1; _LL746: Cyc_Tcutil_warn( loc,({ unsigned char* _temp751=(
unsigned char*)"nested extern \"C\" declaration"; struct _tagged_string _temp752;
_temp752.curr= _temp751; _temp752.base= _temp751; _temp752.last_plus_one=
_temp751 + 30; _temp752;})); return 1; _LL736:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp753= ds0; goto _LL754; _LL754: for( 0; _temp753 != 0;
_temp753=(( struct Cyc_List_List*) _check_null( _temp753))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp753))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp755=( void*) d->r; int
_temp781; struct Cyc_Absyn_Exp* _temp783; struct Cyc_Core_Opt* _temp785; struct
Cyc_Core_Opt* _temp787; struct Cyc_Absyn_Pat* _temp789; struct Cyc_List_List*
_temp791; struct Cyc_Absyn_Vardecl* _temp793; struct Cyc_Absyn_Fndecl* _temp795;
struct Cyc_Absyn_Typedefdecl* _temp797; struct Cyc_Absyn_Structdecl* _temp799;
struct Cyc_Absyn_Uniondecl* _temp801; struct Cyc_Absyn_Tuniondecl* _temp803;
struct Cyc_Absyn_Enumdecl* _temp805; struct Cyc_List_List* _temp807; struct
_tagged_string* _temp809; struct Cyc_List_List* _temp811; struct _tuple0*
_temp813; struct _tuple0 _temp815; struct _tagged_string* _temp816; void*
_temp818; struct Cyc_List_List* _temp820; _LL757: if(*(( int*) _temp755) == Cyc_Absyn_Let_d){
_LL790: _temp789=(( struct Cyc_Absyn_Let_d_struct*) _temp755)->f1; goto _LL788;
_LL788: _temp787=(( struct Cyc_Absyn_Let_d_struct*) _temp755)->f2; goto _LL786;
_LL786: _temp785=(( struct Cyc_Absyn_Let_d_struct*) _temp755)->f3; goto _LL784;
_LL784: _temp783=(( struct Cyc_Absyn_Let_d_struct*) _temp755)->f4; goto _LL782;
_LL782: _temp781=(( struct Cyc_Absyn_Let_d_struct*) _temp755)->f5; goto _LL758;}
else{ goto _LL759;} _LL759: if(*(( int*) _temp755) == Cyc_Absyn_Letv_d){ _LL792:
_temp791=(( struct Cyc_Absyn_Letv_d_struct*) _temp755)->f1; goto _LL760;} else{
goto _LL761;} _LL761: if(*(( int*) _temp755) == Cyc_Absyn_Var_d){ _LL794:
_temp793=(( struct Cyc_Absyn_Var_d_struct*) _temp755)->f1; goto _LL762;} else{
goto _LL763;} _LL763: if(*(( int*) _temp755) == Cyc_Absyn_Fn_d){ _LL796:
_temp795=(( struct Cyc_Absyn_Fn_d_struct*) _temp755)->f1; goto _LL764;} else{
goto _LL765;} _LL765: if(*(( int*) _temp755) == Cyc_Absyn_Typedef_d){ _LL798:
_temp797=(( struct Cyc_Absyn_Typedef_d_struct*) _temp755)->f1; goto _LL766;}
else{ goto _LL767;} _LL767: if(*(( int*) _temp755) == Cyc_Absyn_Struct_d){
_LL800: _temp799=(( struct Cyc_Absyn_Struct_d_struct*) _temp755)->f1; goto
_LL768;} else{ goto _LL769;} _LL769: if(*(( int*) _temp755) == Cyc_Absyn_Union_d){
_LL802: _temp801=(( struct Cyc_Absyn_Union_d_struct*) _temp755)->f1; goto _LL770;}
else{ goto _LL771;} _LL771: if(*(( int*) _temp755) == Cyc_Absyn_Tunion_d){
_LL804: _temp803=(( struct Cyc_Absyn_Tunion_d_struct*) _temp755)->f1; goto
_LL772;} else{ goto _LL773;} _LL773: if(*(( int*) _temp755) == Cyc_Absyn_Enum_d){
_LL806: _temp805=(( struct Cyc_Absyn_Enum_d_struct*) _temp755)->f1; goto _LL774;}
else{ goto _LL775;} _LL775: if(*(( int*) _temp755) == Cyc_Absyn_Namespace_d){
_LL810: _temp809=(( struct Cyc_Absyn_Namespace_d_struct*) _temp755)->f1; goto
_LL808; _LL808: _temp807=(( struct Cyc_Absyn_Namespace_d_struct*) _temp755)->f2;
goto _LL776;} else{ goto _LL777;} _LL777: if(*(( int*) _temp755) == Cyc_Absyn_Using_d){
_LL814: _temp813=(( struct Cyc_Absyn_Using_d_struct*) _temp755)->f1; _temp815=*
_temp813; _LL819: _temp818= _temp815.f1; goto _LL817; _LL817: _temp816= _temp815.f2;
goto _LL812; _LL812: _temp811=(( struct Cyc_Absyn_Using_d_struct*) _temp755)->f2;
goto _LL778;} else{ goto _LL779;} _LL779: if(*(( int*) _temp755) == Cyc_Absyn_ExternC_d){
_LL821: _temp820=(( struct Cyc_Absyn_ExternC_d_struct*) _temp755)->f1; goto
_LL780;} else{ goto _LL756;} _LL758: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp822=( unsigned char*)"top level let-declarations are not implemented";
struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 47; _temp823;})); goto _LL756; _LL760: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp824=( unsigned char*)"top level let-declarations are not implemented";
struct _tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 47; _temp825;})); goto _LL756; _LL762: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp793->sc): 0){( void*)(
_temp793->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp793, check_var_init); goto _LL756; _LL764: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp795->sc): 0){( void*)( _temp795->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp795); goto _LL756; _LL766: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp797); goto _LL756; _LL768: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp799->sc): 0){( void*)( _temp799->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp799); goto _LL756; _LL770: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp801->sc): 0){( void*)( _temp801->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp801);
goto _LL756; _LL772: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp803->sc): 0){( void*)( _temp803->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp803); goto _LL756; _LL774: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp805->sc): 0){( void*)( _temp805->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp805);
goto _LL756; _LL776: { struct Cyc_List_List* _temp826= te->ns; goto _LL827;
_LL827: { struct Cyc_List_List* _temp829=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp826,({ struct Cyc_List_List*
_temp828=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp828->hd=( void*) _temp809; _temp828->tl= 0; _temp828;})); goto _LL830;
_LL830: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge->namespaces, _temp809)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp809); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp829, Cyc_Tcenv_empty_genv());}
te->ns= _temp829; Cyc_Tc_tc_decls( te, _temp807, in_externC, check_var_init); te->ns=
_temp826; goto _LL756;}} _LL778: { struct _tagged_string* first; struct Cyc_List_List*
rest;{ void* _temp831= _temp818; struct Cyc_List_List* _temp843; struct Cyc_List_List*
_temp845; struct Cyc_List_List* _temp847; struct Cyc_List_List _temp849; struct
Cyc_List_List* _temp850; struct _tagged_string* _temp852; struct Cyc_List_List*
_temp854; struct Cyc_List_List _temp856; struct Cyc_List_List* _temp857; struct
_tagged_string* _temp859; _LL833: if( _temp831 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL834;} else{ goto _LL835;} _LL835: if(( unsigned int) _temp831 > 1u?*(( int*)
_temp831) == Cyc_Absyn_Rel_n: 0){ _LL844: _temp843=(( struct Cyc_Absyn_Rel_n_struct*)
_temp831)->f1; if( _temp843 == 0){ goto _LL836;} else{ goto _LL837;}} else{ goto
_LL837;} _LL837: if(( unsigned int) _temp831 > 1u?*(( int*) _temp831) == Cyc_Absyn_Abs_n:
0){ _LL846: _temp845=(( struct Cyc_Absyn_Abs_n_struct*) _temp831)->f1; if(
_temp845 == 0){ goto _LL838;} else{ goto _LL839;}} else{ goto _LL839;} _LL839:
if(( unsigned int) _temp831 > 1u?*(( int*) _temp831) == Cyc_Absyn_Rel_n: 0){
_LL848: _temp847=(( struct Cyc_Absyn_Rel_n_struct*) _temp831)->f1; if( _temp847
== 0){ goto _LL841;} else{ _temp849=* _temp847; _LL853: _temp852=( struct
_tagged_string*) _temp849.hd; goto _LL851; _LL851: _temp850= _temp849.tl; goto
_LL840;}} else{ goto _LL841;} _LL841: if(( unsigned int) _temp831 > 1u?*(( int*)
_temp831) == Cyc_Absyn_Abs_n: 0){ _LL855: _temp854=(( struct Cyc_Absyn_Abs_n_struct*)
_temp831)->f1; if( _temp854 == 0){ goto _LL832;} else{ _temp856=* _temp854;
_LL860: _temp859=( struct _tagged_string*) _temp856.hd; goto _LL858; _LL858:
_temp857= _temp856.tl; goto _LL842;}} else{ goto _LL832;} _LL834: goto _LL836;
_LL836: goto _LL838; _LL838: first= _temp816; rest= 0; goto _LL832; _LL840:
_temp859= _temp852; _temp857= _temp850; goto _LL842; _LL842: first= _temp859;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp857,({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp861->hd=( void*) _temp816;
_temp861->tl= 0; _temp861;})); goto _LL832; _LL832:;}{ struct Cyc_List_List*
_temp862= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); goto _LL863;
_LL863: ge->availables=({ struct Cyc_List_List* _temp864=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp864->hd=( void*) _temp862;
_temp864->tl= ge->availables; _temp864;}); Cyc_Tc_tc_decls( te, _temp811,
in_externC, check_var_init); ge->availables=(( struct Cyc_List_List*)
_check_null( ge->availables))->tl; goto _LL756;}} _LL780: Cyc_Tc_tc_decls( te,
_temp820, 1, check_var_init); goto _LL756; _LL756:;}} static unsigned char
_temp867[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string={ _temp867,
_temp867, _temp867 + 4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string;
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
check_var_init, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({ struct
Cyc_List_List* _temp868=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp868->hd=( void*)({ struct Cyc_Absyn_Decl* _temp869=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp869->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp870=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp870[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp871; _temp871.tag= Cyc_Absyn_Namespace_d;
_temp871.f1= Cyc_Tc_cyc_ns; _temp871.f2= ds; _temp871;}); _temp870;})); _temp869->loc=
0; _temp869;}); _temp868->tl= 0; _temp868;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp872=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp882; struct Cyc_List_List*
_temp884; struct Cyc_List_List** _temp886; struct _tuple0* _temp887; struct Cyc_List_List*
_temp889; struct Cyc_List_List** _temp891; struct _tagged_string* _temp892;
_LL874: if(*(( int*) _temp872) == Cyc_Absyn_Var_d){ _LL883: _temp882=(( struct
Cyc_Absyn_Var_d_struct*) _temp872)->f1; goto _LL875;} else{ goto _LL876;} _LL876:
if(*(( int*) _temp872) == Cyc_Absyn_Using_d){ _LL888: _temp887=(( struct Cyc_Absyn_Using_d_struct*)
_temp872)->f1; goto _LL885; _LL885: _temp884=(( struct Cyc_Absyn_Using_d_struct*)
_temp872)->f2; _temp886=&(( struct Cyc_Absyn_Using_d_struct*) _temp872)->f2;
goto _LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp872) == Cyc_Absyn_Namespace_d){
_LL893: _temp892=(( struct Cyc_Absyn_Namespace_d_struct*) _temp872)->f1; goto
_LL890; _LL890: _temp889=(( struct Cyc_Absyn_Namespace_d_struct*) _temp872)->f2;
_temp891=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp872)->f2; goto _LL879;}
else{ goto _LL880;} _LL880: goto _LL881; _LL875: if(( void*) _temp882->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp896; struct
_tagged_string* _temp897; void* _temp899; struct _tuple0* _temp894= _temp882->name;
_temp896=* _temp894; _LL900: _temp899= _temp896.f1; goto _LL898; _LL898:
_temp897= _temp896.f2; goto _LL895; _LL895: { struct Cyc_List_List* ns;{ void*
_temp901= _temp899; struct Cyc_List_List* _temp909; struct Cyc_List_List*
_temp911; _LL903: if( _temp901 ==( void*) Cyc_Absyn_Loc_n){ goto _LL904;} else{
goto _LL905;} _LL905: if(( unsigned int) _temp901 > 1u?*(( int*) _temp901) ==
Cyc_Absyn_Rel_n: 0){ _LL910: _temp909=(( struct Cyc_Absyn_Rel_n_struct*)
_temp901)->f1; goto _LL906;} else{ goto _LL907;} _LL907: if(( unsigned int)
_temp901 > 1u?*(( int*) _temp901) == Cyc_Absyn_Abs_n: 0){ _LL912: _temp911=((
struct Cyc_Absyn_Abs_n_struct*) _temp901)->f1; goto _LL908;} else{ goto _LL902;}
_LL904: ns= 0; goto _LL902; _LL906: ns= _temp909; goto _LL902; _LL908: ns=
_temp911; goto _LL902; _LL902:;}{ struct _tuple6* _temp913=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
goto _LL914; _LL914: { struct Cyc_Tcenv_Genv* _temp915=(* _temp913).f1; goto
_LL916; _LL916: { int _temp917=(*(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp915->ordinaries, _temp897)).f2;
goto _LL918; _LL918: if( ! _temp917){(* _temp913).f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((* _temp913).f2,
_temp897);} return _temp917;}}}}} _LL877: _temp891= _temp886; goto _LL879;
_LL879:* _temp891= Cyc_Tc_treeshake_f( env,* _temp891); return 1; _LL881: return
1; _LL873:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set* set,
struct _tagged_string* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp919=(* nsenv).f1; goto _LL920; _LL920: { struct Cyc_Set_Set* _temp921=(*
nsenv).f2; goto _LL922; _LL922: _temp919->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple3*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
_temp921, _temp919->ordinaries);}} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp923=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp923->f1= ge; _temp923->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp923;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp924=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
goto _LL925; _LL925: { struct Cyc_List_List* _temp926= Cyc_Tc_treeshake_f(
_temp924, ds); goto _LL927; _LL927:(( void(*)( void(* f)( struct Cyc_List_List*,
struct _tuple6*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
_temp924); return _temp926;}}