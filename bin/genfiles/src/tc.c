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
Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1;
int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1;
int rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict*
d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key); extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tagged_string*,
struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern unsigned char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only( void* t); extern int
Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e);
extern int Cyc_Tcutil_supports_default( void*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int new_block); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List*); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u];
struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base; struct
Cyc_Absyn_Tunionfield* field; } ; extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
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
void* _temp177=(* _temp175).f1; void* _temp187; struct Cyc_Absyn_Structdecl*
_temp189; struct Cyc_Absyn_Tunionfield* _temp191; struct Cyc_Absyn_Tuniondecl*
_temp193; struct Cyc_Absyn_Enumfield* _temp195; struct Cyc_Absyn_Enumdecl*
_temp197; _LL179: if(*(( int*) _temp177) == Cyc_Tcenv_VarRes){ _LL188: _temp187=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp177)->f1; goto _LL180;} else{
goto _LL181;} _LL181: if(*(( int*) _temp177) == Cyc_Tcenv_StructRes){ _LL190:
_temp189=(( struct Cyc_Tcenv_StructRes_struct*) _temp177)->f1; goto _LL182;}
else{ goto _LL183;} _LL183: if(*(( int*) _temp177) == Cyc_Tcenv_TunionRes){
_LL194: _temp193=(( struct Cyc_Tcenv_TunionRes_struct*) _temp177)->f1; goto
_LL192; _LL192: _temp191=(( struct Cyc_Tcenv_TunionRes_struct*) _temp177)->f2;
goto _LL184;} else{ goto _LL185;} _LL185: if(*(( int*) _temp177) == Cyc_Tcenv_EnumRes){
_LL198: _temp197=(( struct Cyc_Tcenv_EnumRes_struct*) _temp177)->f1; goto _LL196;
_LL196: _temp195=(( struct Cyc_Tcenv_EnumRes_struct*) _temp177)->f2; goto _LL186;}
else{ goto _LL178;} _LL180: { struct Cyc_Absyn_Global_b_struct* _temp201=({
struct Cyc_Absyn_Global_b_struct* _temp199=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp199[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp200; _temp200.tag= Cyc_Absyn_Global_b; _temp200.f1=
vd; _temp200;}); _temp199;}); goto _LL202; _LL202: { void* _temp203= Cyc_Tcdecl_merge_binding(
_temp187,( void*) _temp201, loc, Cyc_Tc_tc_msg); goto _LL204; _LL204: if(
_temp203 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL178;} if( _temp203 ==
_temp187?(* _temp175).f2: 0){ goto _LL178;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp205=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp205->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp206=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp206[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp207; _temp207.tag= Cyc_Tcenv_VarRes;
_temp207.f1=( void*) _temp203; _temp207;}); _temp206;}); _temp205->f2= 1;
_temp205;})); goto _LL178;}} _LL182: Cyc_Tcutil_warn( loc,({ unsigned char*
_temp208=( unsigned char*)"variable declaration shadows previous struct declaration";
struct _tagged_string _temp209; _temp209.curr= _temp208; _temp209.base= _temp208;
_temp209.last_plus_one= _temp208 + 57; _temp209;})); goto _LL178; _LL184: Cyc_Tcutil_warn(
loc,({ unsigned char* _temp210=( unsigned char*)"variable declaration shadows previous [x]tunion constructor";
struct _tagged_string _temp211; _temp211.curr= _temp210; _temp211.base= _temp210;
_temp211.last_plus_one= _temp210 + 60; _temp211;})); goto _LL178; _LL186: Cyc_Tcutil_warn(
loc,({ unsigned char* _temp212=( unsigned char*)"variable declaration shadows previous enum tag";
struct _tagged_string _temp213; _temp213.curr= _temp212; _temp213.base= _temp212;
_temp213.last_plus_one= _temp212 + 47; _temp213;})); goto _LL178; _LL178:;}};
_pop_handler();} else{ void* _temp173=( void*) _exn_thrown; void* _temp215=
_temp173; _LL217: if( _temp215 == Cyc_Dict_Absent){ goto _LL218;} else{ goto
_LL219;} _LL219: goto _LL220; _LL218: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp221=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp221->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp222=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp222[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp223; _temp223.tag= Cyc_Tcenv_VarRes;
_temp223.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp224=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp224[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp225; _temp225.tag= Cyc_Absyn_Global_b;
_temp225.f1= vd; _temp225;}); _temp224;})); _temp223;}); _temp222;}); _temp221->f2=
0; _temp221;})); goto _LL216; _LL220:( void) _throw( _temp215); _LL216:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp226=(*
fd->name).f1; struct Cyc_List_List* _temp234; struct Cyc_List_List* _temp236;
_LL228: if(( unsigned int) _temp226 > 1u?*(( int*) _temp226) == Cyc_Absyn_Rel_n:
0){ _LL235: _temp234=(( struct Cyc_Absyn_Rel_n_struct*) _temp226)->f1; if(
_temp234 == 0){ goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230:
if(( unsigned int) _temp226 > 1u?*(( int*) _temp226) == Cyc_Absyn_Abs_n: 0){
_LL237: _temp236=(( struct Cyc_Absyn_Abs_n_struct*) _temp226)->f1; goto _LL231;}
else{ goto _LL232;} _LL232: goto _LL233; _LL229: goto _LL227; _LL231:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp238=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp238[ 0]=({ struct
Cyc_Core_Impossible_struct _temp239; _temp239.tag= Cyc_Core_Impossible; _temp239.f1=({
unsigned char* _temp240=( unsigned char*)"tc: Abs_n in tcFndecl"; struct
_tagged_string _temp241; _temp241.curr= _temp240; _temp241.base= _temp240;
_temp241.last_plus_one= _temp240 + 22; _temp241;}); _temp239;}); _temp238;}));
_LL233: Cyc_Tcutil_terr( loc,({ unsigned char* _temp242=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp243; _temp243.curr= _temp242; _temp243.base= _temp242;
_temp243.last_plus_one= _temp242 + 43; _temp243;})); return; _LL227:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp244=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp244[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp245; _temp245.tag= Cyc_Absyn_Abs_n; _temp245.f1= te->ns; _temp245;});
_temp244;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp246=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp254; _LL248: if(
_temp246 ==( void*) Cyc_Absyn_Packed_att){ goto _LL249;} else{ goto _LL250;}
_LL250: if(( unsigned int) _temp246 > 16u?*(( int*) _temp246) == Cyc_Absyn_Aligned_att:
0){ _LL255: _temp254=(( struct Cyc_Absyn_Aligned_att_struct*) _temp246)->f1;
goto _LL251;} else{ goto _LL252;} _LL252: goto _LL253; _LL249: goto _LL251;
_LL251: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp256= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); xprintf("bad attribute %.*s for function", _temp256.last_plus_one
- _temp256.curr, _temp256.curr);})); goto _LL247; _LL253: goto _LL247; _LL247:;}}{
struct _handler_cons _temp257; _push_handler(& _temp257);{ int _temp259= 0; if(
setjmp( _temp257.handler)){ _temp259= 1;} if( ! _temp259){{ struct _tuple3*
_temp260=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL261; _LL261: { void*
_temp262=(* _temp260).f1; void* _temp272; struct Cyc_Absyn_Structdecl* _temp274;
struct Cyc_Absyn_Tunionfield* _temp276; struct Cyc_Absyn_Tuniondecl* _temp278;
struct Cyc_Absyn_Enumfield* _temp280; struct Cyc_Absyn_Enumdecl* _temp282;
_LL264: if(*(( int*) _temp262) == Cyc_Tcenv_VarRes){ _LL273: _temp272=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp262)->f1; goto _LL265;} else{ goto _LL266;}
_LL266: if(*(( int*) _temp262) == Cyc_Tcenv_StructRes){ _LL275: _temp274=((
struct Cyc_Tcenv_StructRes_struct*) _temp262)->f1; goto _LL267;} else{ goto
_LL268;} _LL268: if(*(( int*) _temp262) == Cyc_Tcenv_TunionRes){ _LL279:
_temp278=(( struct Cyc_Tcenv_TunionRes_struct*) _temp262)->f1; goto _LL277;
_LL277: _temp276=(( struct Cyc_Tcenv_TunionRes_struct*) _temp262)->f2; goto
_LL269;} else{ goto _LL270;} _LL270: if(*(( int*) _temp262) == Cyc_Tcenv_EnumRes){
_LL283: _temp282=(( struct Cyc_Tcenv_EnumRes_struct*) _temp262)->f1; goto _LL281;
_LL281: _temp280=(( struct Cyc_Tcenv_EnumRes_struct*) _temp262)->f2; goto _LL271;}
else{ goto _LL263;} _LL265: { struct Cyc_Absyn_Funname_b_struct* _temp286=({
struct Cyc_Absyn_Funname_b_struct* _temp284=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp284[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp285; _temp285.tag= Cyc_Absyn_Funname_b; _temp285.f1=
fd; _temp285;}); _temp284;}); goto _LL287; _LL287: { void* _temp288= Cyc_Tcdecl_merge_binding(
_temp272,( void*) _temp286, loc, Cyc_Tc_tc_msg); goto _LL289; _LL289: if(
_temp288 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL263;} if( _temp288 ==
_temp272?(* _temp260).f2: 0){ goto _LL263;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp290=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp290->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp291=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp291[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp292; _temp292.tag= Cyc_Tcenv_VarRes;
_temp292.f1=( void*) _temp288; _temp292;}); _temp291;}); _temp290->f2= 1;
_temp290;})); goto _LL263;}} _LL267: Cyc_Tcutil_warn( loc,({ unsigned char*
_temp293=( unsigned char*)"function declaration shadows previous struct declaration";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 57; _temp294;})); goto _LL263; _LL269: Cyc_Tcutil_warn(
loc,({ unsigned char* _temp295=( unsigned char*)"function declaration shadows previous [x]tunion constructor";
struct _tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 60; _temp296;})); goto _LL263; _LL271: Cyc_Tcutil_warn(
loc,({ unsigned char* _temp297=( unsigned char*)"function declaration shadows previous enum tag";
struct _tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 47; _temp298;})); goto _LL263; _LL263:;}};
_pop_handler();} else{ void* _temp258=( void*) _exn_thrown; void* _temp300=
_temp258; _LL302: if( _temp300 == Cyc_Dict_Absent){ goto _LL303;} else{ goto
_LL304;} _LL304: goto _LL305; _LL303: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp306=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp306->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp307=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp307[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp308; _temp308.tag= Cyc_Tcenv_VarRes;
_temp308.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp309=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp309[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp310; _temp310.tag= Cyc_Absyn_Funname_b;
_temp310.f1= fd; _temp310;}); _temp309;})); _temp308;}); _temp307;}); _temp306->f2=
0; _temp306;})); goto _LL301; _LL305:( void) _throw( _temp300); _LL301:;}}} te->le=({
struct Cyc_Core_Opt* _temp311=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp311->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp312=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp312[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp313; _temp313.tag= Cyc_Tcenv_Outermost;
_temp313.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp313;}); _temp312;}));
_temp311;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp314=( unsigned char*)"function has goto statements to undefined labels";
struct _tagged_string _temp315; _temp315.curr= _temp314; _temp315.base= _temp314;
_temp315.last_plus_one= _temp314 + 49; _temp315;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp316=(* td->name).f1; struct Cyc_List_List*
_temp324; struct Cyc_List_List* _temp326; _LL318: if(( unsigned int) _temp316 >
1u?*(( int*) _temp316) == Cyc_Absyn_Rel_n: 0){ _LL325: _temp324=(( struct Cyc_Absyn_Rel_n_struct*)
_temp316)->f1; if( _temp324 == 0){ goto _LL319;} else{ goto _LL320;}} else{ goto
_LL320;} _LL320: if(( unsigned int) _temp316 > 1u?*(( int*) _temp316) == Cyc_Absyn_Abs_n:
0){ _LL327: _temp326=(( struct Cyc_Absyn_Abs_n_struct*) _temp316)->f1; if(
_temp326 == 0){ goto _LL321;} else{ goto _LL322;}} else{ goto _LL322;} _LL322:
goto _LL323; _LL319: goto _LL317; _LL321: goto _LL317; _LL323: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp328=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp329; _temp329.curr= _temp328; _temp329.base= _temp328;
_temp329.last_plus_one= _temp328 + 43; _temp329;})); return; _LL317:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp330=* v; xprintf("redeclaration of typedef %.*s", _temp330.last_plus_one -
_temp330.curr, _temp330.curr);})); return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp331=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp331[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp332; _temp332.tag= Cyc_Absyn_Abs_n;
_temp332.f1= te->ns; _temp332;}); _temp331;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp333= _new_region();
struct _RegionHandle* uprev_rgn=& _temp333; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp334= fields; goto
_LL335; _LL335: for( 0; _temp334 != 0; _temp334=(( struct Cyc_List_List*)
_check_null( _temp334))->tl){ struct Cyc_Absyn_Structfield _temp338; struct Cyc_List_List*
_temp339; struct Cyc_Core_Opt* _temp341; void* _temp343; struct Cyc_Absyn_Tqual
_temp345; struct _tagged_string* _temp347; struct Cyc_Absyn_Structfield*
_temp336=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp334))->hd; _temp338=* _temp336; _LL348: _temp347= _temp338.name; goto
_LL346; _LL346: _temp345= _temp338.tq; goto _LL344; _LL344: _temp343=( void*)
_temp338.type; goto _LL342; _LL342: _temp341= _temp338.width; goto _LL340;
_LL340: _temp339= _temp338.attributes; goto _LL337; _LL337: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp347)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp349=* _temp347; struct _tagged_string _temp350= obj; xprintf("duplicate field %.*s in %.*s",
_temp349.last_plus_one - _temp349.curr, _temp349.curr, _temp350.last_plus_one -
_temp350.curr, _temp350.curr);}));} if( Cyc_String_strcmp(* _temp347,({
unsigned char* _temp351=( unsigned char*)""; struct _tagged_string _temp352;
_temp352.curr= _temp351; _temp352.base= _temp351; _temp352.last_plus_one=
_temp351 + 1; _temp352;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp353=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp353->hd=( void*) _temp347; _temp353->tl= prev_fields; _temp353;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp343); if( _temp341 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp341))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp354=* _temp347; xprintf("bitfield %.*s does not have constant width",
_temp354.last_plus_one - _temp354.curr, _temp354.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp341))->v);}{
void* _temp355= Cyc_Tcutil_compress( _temp343); void* _temp361; void* _temp363;
_LL357: if(( unsigned int) _temp355 > 4u?*(( int*) _temp355) == Cyc_Absyn_IntType:
0){ _LL364: _temp363=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp355)->f1;
goto _LL362; _LL362: _temp361=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp355)->f2; goto _LL358;} else{ goto _LL359;} _LL359: goto _LL360; _LL358:{
void* _temp365= _temp361; _LL367: if( _temp365 ==( void*) Cyc_Absyn_B1){ goto
_LL368;} else{ goto _LL369;} _LL369: if( _temp365 ==( void*) Cyc_Absyn_B2){ goto
_LL370;} else{ goto _LL371;} _LL371: if( _temp365 ==( void*) Cyc_Absyn_B4){ goto
_LL372;} else{ goto _LL373;} _LL373: if( _temp365 ==( void*) Cyc_Absyn_B8){ goto
_LL374;} else{ goto _LL366;} _LL368: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp375=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 26; _temp376;}));} goto _LL366; _LL370: if( w
> 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp377=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 26; _temp378;}));} goto _LL366; _LL372: if( w
> 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp379=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 26; _temp380;}));} goto _LL366; _LL374: if( w
> 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp381=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 26; _temp382;}));} goto _LL366; _LL366:;}
goto _LL356; _LL360: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp383=* _temp347; struct _tagged_string _temp384= Cyc_Absynpp_typ2string(
_temp343); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp383.last_plus_one - _temp383.curr, _temp383.curr, _temp384.last_plus_one -
_temp384.curr, _temp384.curr);})); goto _LL356; _LL356:;}}}}; _pop_region(&
_temp333);} static void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp385=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL386; _LL386: if( sd->name == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp387= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp387.last_plus_one - _temp387.curr, _temp387.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp388=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp396; _LL390: if( _temp388 ==( void*) Cyc_Absyn_Packed_att){
goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int) _temp388 > 16u?*((
int*) _temp388) == Cyc_Absyn_Aligned_att: 0){ _LL397: _temp396=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp388)->f1; goto _LL393;} else{ goto _LL394;} _LL394: goto _LL395; _LL391:
continue; _LL393: continue; _LL395: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp398= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp399= obj; struct
_tagged_string _temp400=* _temp385; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp398.last_plus_one - _temp398.curr, _temp398.curr, _temp399.last_plus_one -
_temp399.curr, _temp399.curr, _temp400.last_plus_one - _temp400.curr, _temp400.curr);}));
goto _LL389; _LL389:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp401=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
goto _LL402; _LL402: { void* _temp403=( void*) _temp401->v; void* _temp411;
_LL405: if( _temp403 ==( void*) Cyc_Absyn_No_constr){ goto _LL406;} else{ goto
_LL407;} _LL407: if(( unsigned int) _temp403 > 1u?*(( int*) _temp403) == Cyc_Absyn_Eq_constr:
0){ _LL412: _temp411=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp403)->f1;
if( _temp411 ==( void*) Cyc_Absyn_MemKind){ goto _LL408;} else{ goto _LL409;}}
else{ goto _LL409;} _LL409: goto _LL410; _LL406:( void*)( _temp401->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp413=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp413[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp414; _temp414.tag= Cyc_Absyn_Eq_constr; _temp414.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp414;}); _temp413;}))); continue; _LL408:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp415=
obj; struct _tagged_string _temp416=* _temp385; struct _tagged_string _temp417=*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name;
xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M", _temp415.last_plus_one
- _temp415.curr, _temp415.curr, _temp416.last_plus_one - _temp416.curr, _temp416.curr,
_temp417.last_plus_one - _temp417.curr, _temp417.curr);})); continue; _LL410:
continue; _LL404:;}}}{ void* _temp418=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp426; struct Cyc_List_List*
_temp428; _LL420: if(( unsigned int) _temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_Rel_n:
0){ _LL427: _temp426=(( struct Cyc_Absyn_Rel_n_struct*) _temp418)->f1; if(
_temp426 == 0){ goto _LL421;} else{ goto _LL422;}} else{ goto _LL422;} _LL422:
if(( unsigned int) _temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_Abs_n: 0){
_LL429: _temp428=(( struct Cyc_Absyn_Abs_n_struct*) _temp418)->f1; if( _temp428
== 0){ goto _LL423;} else{ goto _LL424;}} else{ goto _LL424;} _LL424: goto
_LL425; _LL421: goto _LL419; _LL423: goto _LL419; _LL425: Cyc_Tcutil_terr( loc,({
unsigned char* _temp430=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 43; _temp431;})); return; _LL419:;}(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp432=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp432[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp433; _temp433.tag= Cyc_Absyn_Abs_n; _temp433.f1= te->ns; _temp433;});
_temp432;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* _temp434=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; goto _LL435; _LL435: { const unsigned char*
_temp436="struct"; goto _LL437; _LL437: Cyc_Tc_tcStructOrUniondecl( te, ge,({
unsigned char* _temp438=( unsigned char*) _temp436; struct _tagged_string
_temp439; _temp439.curr= _temp438; _temp439.base= _temp438; _temp439.last_plus_one=
_temp438 + 7; _temp439;}), loc, sd);{ struct Cyc_List_List* _temp440= sd->tvs;
goto _LL441; _LL441:{ struct _tuple4 _temp443=({ struct _tuple4 _temp442;
_temp442.f1= sd->fields; _temp442.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp434);
_temp442;}); struct Cyc_Core_Opt* _temp453; struct Cyc_Core_Opt* _temp455;
struct Cyc_Core_Opt* _temp457; struct Cyc_Core_Opt* _temp459; struct Cyc_Core_Opt
_temp461; struct Cyc_List_List* _temp462; struct Cyc_Core_Opt* _temp464; struct
Cyc_Core_Opt _temp466; struct Cyc_Absyn_Structdecl** _temp467; struct Cyc_Core_Opt*
_temp469; struct Cyc_Core_Opt _temp471; struct Cyc_List_List* _temp472; struct
Cyc_Core_Opt* _temp474; struct Cyc_Core_Opt _temp476; struct Cyc_Absyn_Structdecl**
_temp477; struct Cyc_Core_Opt* _temp479; _LL445: _LL456: _temp455= _temp443.f1;
if( _temp455 == 0){ goto _LL454;} else{ goto _LL447;} _LL454: _temp453= _temp443.f2;
if( _temp453 == 0){ goto _LL446;} else{ goto _LL447;} _LL447: _LL460: _temp459=
_temp443.f1; if( _temp459 == 0){ goto _LL449;} else{ _temp461=* _temp459; _LL463:
_temp462=( struct Cyc_List_List*) _temp461.v; goto _LL458;} _LL458: _temp457=
_temp443.f2; if( _temp457 == 0){ goto _LL448;} else{ goto _LL449;} _LL449:
_LL470: _temp469= _temp443.f1; if( _temp469 == 0){ goto _LL451;} else{ _temp471=*
_temp469; _LL473: _temp472=( struct Cyc_List_List*) _temp471.v; goto _LL465;}
_LL465: _temp464= _temp443.f2; if( _temp464 == 0){ goto _LL451;} else{ _temp466=*
_temp464; _LL468: _temp467=( struct Cyc_Absyn_Structdecl**) _temp466.v; goto
_LL450;} _LL451: _LL480: _temp479= _temp443.f1; if( _temp479 == 0){ goto _LL475;}
else{ goto _LL444;} _LL475: _temp474= _temp443.f2; if( _temp474 == 0){ goto
_LL444;} else{ _temp476=* _temp474; _LL478: _temp477=( struct Cyc_Absyn_Structdecl**)
_temp476.v; goto _LL452;} _LL446: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp434,({ struct Cyc_Absyn_Structdecl**
_temp481=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp481[ 0]= sd; _temp481;})); goto _LL444; _LL448: { struct Cyc_Absyn_Structdecl**
_temp484=({ struct Cyc_Absyn_Structdecl** _temp482=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp482[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp483=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp483->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp483->name= sd->name;
_temp483->tvs= _temp440; _temp483->fields= 0; _temp483->attributes= 0; _temp483;});
_temp482;}); goto _LL485; _LL485: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp434, _temp484); Cyc_Tc_tcStructFields(
te, ge, loc,({ unsigned char* _temp486=( unsigned char*) _temp436; struct
_tagged_string _temp487; _temp487.curr= _temp486; _temp487.base= _temp486;
_temp487.last_plus_one= _temp486 + 7; _temp487;}), _temp462, _temp440);*
_temp484= sd; goto _LL444;} _LL450: { struct Cyc_Absyn_Structdecl* _temp488=*
_temp467; goto _LL489; _LL489:* _temp467=({ struct Cyc_Absyn_Structdecl*
_temp490=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp490->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp490->name= sd->name;
_temp490->tvs= _temp440; _temp490->fields= 0; _temp490->attributes= 0; _temp490;});
Cyc_Tc_tcStructFields( te, ge, loc,({ unsigned char* _temp491=( unsigned char*)
_temp436; struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base=
_temp491; _temp492.last_plus_one= _temp491 + 7; _temp492;}), _temp472, _temp440);*
_temp467= _temp488; _temp477= _temp467; goto _LL452;} _LL452: { struct Cyc_Absyn_Structdecl*
_temp493= Cyc_Tcdecl_merge_structdecl(* _temp477, sd, loc, Cyc_Tc_tc_msg); goto
_LL494; _LL494: if( _temp493 == 0){ return;} else{* _temp477=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp493); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp493);
goto _LL444;}} _LL444:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp434,({ struct _tuple3* _temp495=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp495->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp496=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp496[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp497; _temp497.tag= Cyc_Tcenv_StructRes;
_temp497.f1= sd; _temp497;}); _temp496;}); _temp495->f2= 1; _temp495;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* _temp498=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)).f2; goto _LL499; _LL499: { const unsigned char*
_temp500="union"; goto _LL501; _LL501: Cyc_Tc_tcStructOrUniondecl( te, ge,({
unsigned char* _temp502=( unsigned char*) _temp500; struct _tagged_string
_temp503; _temp503.curr= _temp502; _temp503.base= _temp502; _temp503.last_plus_one=
_temp502 + 6; _temp503;}), loc,( struct Cyc_Absyn_Structdecl*) ud);{ struct Cyc_List_List*
_temp504= ud->tvs; goto _LL505; _LL505: { struct _tuple4 _temp507=({ struct
_tuple4 _temp506; _temp506.f1= ud->fields; _temp506.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp498); _temp506;}); struct Cyc_Core_Opt* _temp517; struct Cyc_Core_Opt*
_temp519; struct Cyc_Core_Opt* _temp521; struct Cyc_Core_Opt* _temp523; struct
Cyc_Core_Opt _temp525; struct Cyc_List_List* _temp526; struct Cyc_Core_Opt*
_temp528; struct Cyc_Core_Opt _temp530; struct Cyc_Absyn_Uniondecl** _temp531;
struct Cyc_Core_Opt* _temp533; struct Cyc_Core_Opt _temp535; struct Cyc_List_List*
_temp536; struct Cyc_Core_Opt* _temp538; struct Cyc_Core_Opt _temp540; struct
Cyc_Absyn_Uniondecl** _temp541; struct Cyc_Core_Opt* _temp543; _LL509: _LL520:
_temp519= _temp507.f1; if( _temp519 == 0){ goto _LL518;} else{ goto _LL511;}
_LL518: _temp517= _temp507.f2; if( _temp517 == 0){ goto _LL510;} else{ goto
_LL511;} _LL511: _LL524: _temp523= _temp507.f1; if( _temp523 == 0){ goto _LL513;}
else{ _temp525=* _temp523; _LL527: _temp526=( struct Cyc_List_List*) _temp525.v;
goto _LL522;} _LL522: _temp521= _temp507.f2; if( _temp521 == 0){ goto _LL512;}
else{ goto _LL513;} _LL513: _LL534: _temp533= _temp507.f1; if( _temp533 == 0){
goto _LL515;} else{ _temp535=* _temp533; _LL537: _temp536=( struct Cyc_List_List*)
_temp535.v; goto _LL529;} _LL529: _temp528= _temp507.f2; if( _temp528 == 0){
goto _LL515;} else{ _temp530=* _temp528; _LL532: _temp531=( struct Cyc_Absyn_Uniondecl**)
_temp530.v; goto _LL514;} _LL515: _LL544: _temp543= _temp507.f1; if( _temp543 ==
0){ goto _LL539;} else{ goto _LL508;} _LL539: _temp538= _temp507.f2; if(
_temp538 == 0){ goto _LL508;} else{ _temp540=* _temp538; _LL542: _temp541=(
struct Cyc_Absyn_Uniondecl**) _temp540.v; goto _LL516;} _LL510: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp498,({
struct Cyc_Absyn_Uniondecl** _temp545=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp545[ 0]= ud; _temp545;})); goto
_LL508; _LL512: { struct Cyc_Absyn_Uniondecl** _temp548=({ struct Cyc_Absyn_Uniondecl**
_temp546=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp546[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp547=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp547->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp547->name= ud->name; _temp547->tvs= _temp504; _temp547->fields=
0; _temp547->attributes= ud->attributes; _temp547;}); _temp546;}); goto _LL549;
_LL549: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp498, _temp548); Cyc_Tc_tcStructFields( te, ge, loc,({
unsigned char* _temp550=( unsigned char*) _temp500; struct _tagged_string
_temp551; _temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 6; _temp551;}), _temp526, _temp504);{ struct Cyc_List_List* f=
_temp526; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(
! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->type)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp552=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->name; struct _tagged_string _temp553=* _temp498; struct
_tagged_string _temp554= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type); xprintf("field %.*s of union %.*s has type %.*s which is not `bits-only'",
_temp552.last_plus_one - _temp552.curr, _temp552.curr, _temp553.last_plus_one -
_temp553.curr, _temp553.curr, _temp554.last_plus_one - _temp554.curr, _temp554.curr);}));}}}*
_temp548= ud; goto _LL508;} _LL514: { struct Cyc_Absyn_Uniondecl* _temp555=*
_temp531; goto _LL556; _LL556:* _temp531=({ struct Cyc_Absyn_Uniondecl* _temp557=(
struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp557->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp557->name= ud->name;
_temp557->tvs= _temp504; _temp557->fields= 0; _temp557->attributes= ud->attributes;
_temp557;}); Cyc_Tc_tcStructFields( te, ge, loc,({ unsigned char* _temp558=(
unsigned char*) _temp500; struct _tagged_string _temp559; _temp559.curr=
_temp558; _temp559.base= _temp558; _temp559.last_plus_one= _temp558 + 6;
_temp559;}), _temp536, _temp504);* _temp531= _temp555; _temp541= _temp531; goto
_LL516;} _LL516: { struct Cyc_Absyn_Uniondecl* _temp560= Cyc_Tcdecl_merge_uniondecl(*
_temp541, ud, loc, Cyc_Tc_tc_msg); goto _LL561; _LL561: if( _temp560 == 0){
return;} else{* _temp541=( struct Cyc_Absyn_Uniondecl*) _check_null( _temp560);
goto _LL508;}} _LL508:;}}}} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp562= fields; goto _LL563; _LL563: for( 0; _temp562 != 0;
_temp562=(( struct Cyc_List_List*) _check_null( _temp562))->tl){ struct Cyc_Absyn_Tunionfield*
_temp564=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp562))->hd; goto _LL565; _LL565:{ struct Cyc_List_List* tvs= _temp564->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Tvar* _temp566=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd; goto _LL567; _LL567: { struct Cyc_Absyn_Conref* _temp568=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp566->kind); goto _LL569; _LL569: { void* _temp570=( void*) _temp568->v;
void* _temp578; _LL572: if( _temp570 ==( void*) Cyc_Absyn_No_constr){ goto
_LL573;} else{ goto _LL574;} _LL574: if(( unsigned int) _temp570 > 1u?*(( int*)
_temp570) == Cyc_Absyn_Eq_constr: 0){ _LL579: _temp578=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp570)->f1; if( _temp578 ==( void*) Cyc_Absyn_MemKind){ goto _LL575;} else{
goto _LL576;}} else{ goto _LL576;} _LL576: goto _LL577; _LL573:( void*)(
_temp568->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp580=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp580[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp581; _temp581.tag= Cyc_Absyn_Eq_constr;
_temp581.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp581;}); _temp580;})));
goto _LL571; _LL575: Cyc_Tcutil_terr( _temp564->loc,( struct _tagged_string)({
struct _tagged_string _temp582=*(* _temp564->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp582.last_plus_one - _temp582.curr, _temp582.curr);})); goto _LL571; _LL577:
goto _LL571; _LL571:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp564->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp564->tvs);{ struct Cyc_List_List* typs= _temp564->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp564->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp583=(*
_temp564->name).f1; struct Cyc_List_List* _temp593; struct Cyc_List_List*
_temp595; struct Cyc_List_List* _temp597; _LL585: if(( unsigned int) _temp583 >
1u?*(( int*) _temp583) == Cyc_Absyn_Rel_n: 0){ _LL594: _temp593=(( struct Cyc_Absyn_Rel_n_struct*)
_temp583)->f1; if( _temp593 == 0){ goto _LL586;} else{ goto _LL587;}} else{ goto
_LL587;} _LL587: if(( unsigned int) _temp583 > 1u?*(( int*) _temp583) == Cyc_Absyn_Rel_n:
0){ _LL596: _temp595=(( struct Cyc_Absyn_Rel_n_struct*) _temp583)->f1; goto
_LL588;} else{ goto _LL589;} _LL589: if(( unsigned int) _temp583 > 1u?*(( int*)
_temp583) == Cyc_Absyn_Abs_n: 0){ _LL598: _temp597=(( struct Cyc_Absyn_Abs_n_struct*)
_temp583)->f1; goto _LL590;} else{ goto _LL591;} _LL591: if( _temp583 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL592;} else{ goto _LL584;} _LL586: if( is_xtunion){(*
_temp564->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp599=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp599[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp600; _temp600.tag= Cyc_Absyn_Abs_n;
_temp600.f1= te->ns; _temp600;}); _temp599;});} else{(* _temp564->name).f1=(*
name).f1;} goto _LL584; _LL588: Cyc_Tcutil_terr( _temp564->loc,({ unsigned char*
_temp601=( unsigned char*)"qualified declarations are not allowed"; struct
_tagged_string _temp602; _temp602.curr= _temp601; _temp602.base= _temp601;
_temp602.last_plus_one= _temp601 + 39; _temp602;})); goto _LL584; _LL590: goto
_LL584; _LL592:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp603=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp603[ 0]=({ struct Cyc_Core_Impossible_struct _temp604; _temp604.tag= Cyc_Core_Impossible;
_temp604.f1=({ unsigned char* _temp605=( unsigned char*)"tcTunionFields: Loc_n";
struct _tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 22; _temp606;}); _temp604;}); _temp603;}));
goto _LL584; _LL584:;}}}}{ struct Cyc_List_List* fields2; if( is_xtunion){ int
_temp607= 1; goto _LL608; _LL608: { struct Cyc_List_List* _temp609= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp607,(* name).f2, loc, Cyc_Tc_tc_msg); goto _LL610; _LL610: if(
_temp607){ fields2= _temp609;} else{ fields2= 0;}}} else{ struct _RegionHandle
_temp611= _new_region(); struct _RegionHandle* uprev_rgn=& _temp611;
_push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List*
fs= fields; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct Cyc_Absyn_Tunionfield* _temp612=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( fs))->hd; goto _LL613; _LL613: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
_temp612->name).f2)){ Cyc_Tcutil_terr( _temp612->loc,( struct _tagged_string)({
struct _tagged_string _temp614=*(* _temp612->name).f2; struct _tagged_string
_temp615= obj; xprintf("duplicate field name %.*s in %.*s", _temp614.last_plus_one
- _temp614.curr, _temp614.curr, _temp615.last_plus_one - _temp615.curr, _temp615.curr);}));}
else{ prev_fields=({ struct Cyc_List_List* _temp616=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp616->hd=( void*)(*
_temp612->name).f2; _temp616->tl= prev_fields; _temp616;});} if(( void*)
_temp612->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ struct _tagged_string _temp617=*(* _temp612->name).f2; xprintf("ignoring scope of field %.*s",
_temp617.last_plus_one - _temp617.curr, _temp617.curr);}));( void*)( _temp612->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp611);}{
struct Cyc_List_List* _temp618= fields; goto _LL619; _LL619: for( 0; _temp618 !=
0; _temp618=(( struct Cyc_List_List*) _check_null( _temp618))->tl){ struct Cyc_Absyn_Tunionfield*
_temp620=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp618))->hd; goto _LL621; _LL621: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp620->name).f2,({ struct _tuple3* _temp622=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp622->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp623=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp623[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp624; _temp624.tag= Cyc_Tcenv_TunionRes; _temp624.f1=
tudres; _temp624.f2= _temp620; _temp624;}); _temp623;}); _temp622->f2= 1;
_temp622;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=({ unsigned char* _temp625=( unsigned char*)"xtunion";
struct _tagged_string _temp626; _temp626.curr= _temp625; _temp626.base= _temp625;
_temp626.last_plus_one= _temp625 + 8; _temp626;});} else{ obj=({ unsigned char*
_temp627=( unsigned char*)"tunion"; struct _tagged_string _temp628; _temp628.curr=
_temp627; _temp628.base= _temp627; _temp628.last_plus_one= _temp627 + 7;
_temp628;});}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List* tvs2=
tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->kind); void* _temp629=( void*) c->v; void* _temp637;
_LL631: if( _temp629 ==( void*) Cyc_Absyn_No_constr){ goto _LL632;} else{ goto
_LL633;} _LL633: if(( unsigned int) _temp629 > 1u?*(( int*) _temp629) == Cyc_Absyn_Eq_constr:
0){ _LL638: _temp637=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp629)->f1;
if( _temp637 ==( void*) Cyc_Absyn_MemKind){ goto _LL634;} else{ goto _LL635;}}
else{ goto _LL635;} _LL635: goto _LL636; _LL632:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp639=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp639[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp640; _temp640.tag= Cyc_Absyn_Eq_constr; _temp640.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp640;}); _temp639;}))); goto _LL630;
_LL634: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp641= obj; struct _tagged_string _temp642=* v; struct _tagged_string
_temp643=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name;
xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M", _temp641.last_plus_one
- _temp641.curr, _temp641.curr, _temp642.last_plus_one - _temp642.curr, _temp642.curr,
_temp643.last_plus_one - _temp643.curr, _temp643.curr);})); goto _LL630; _LL636:
goto _LL630; _LL630:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp644; _push_handler(& _temp644);{ int _temp646= 0; if( setjmp( _temp644.handler)){
_temp646= 1;} if( ! _temp646){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp645=( void*) _exn_thrown; void*
_temp648= _temp645; _LL650: if( _temp648 == Cyc_Dict_Absent){ goto _LL651;}
else{ goto _LL652;} _LL652: goto _LL653; _LL651: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp654= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp654.last_plus_one - _temp654.curr, _temp654.curr);})); return; _LL653:(
void) _throw( _temp648); _LL649:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp655=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp655[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp656; _temp656.tag= Cyc_Absyn_Abs_n;
_temp656.f1= te->ns; _temp656;}); _temp655;});}} else{{ void* _temp657=(* tud->name).f1;
struct Cyc_List_List* _temp665; struct Cyc_List_List* _temp667; _LL659: if((
unsigned int) _temp657 > 1u?*(( int*) _temp657) == Cyc_Absyn_Rel_n: 0){ _LL666:
_temp665=(( struct Cyc_Absyn_Rel_n_struct*) _temp657)->f1; if( _temp665 == 0){
goto _LL660;} else{ goto _LL661;}} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp657 > 1u?*(( int*) _temp657) == Cyc_Absyn_Abs_n: 0){ _LL668: _temp667=((
struct Cyc_Absyn_Abs_n_struct*) _temp657)->f1; goto _LL662;} else{ goto _LL663;}
_LL663: goto _LL664; _LL660:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp669=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp669[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp670; _temp670.tag= Cyc_Absyn_Abs_n;
_temp670.f1= te->ns; _temp670;}); _temp669;}); goto _LL658; _LL662: goto _LL664;
_LL664: Cyc_Tcutil_terr( loc,({ unsigned char* _temp671=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp672; _temp672.curr= _temp671; _temp672.base= _temp671;
_temp672.last_plus_one= _temp671 + 43; _temp672;})); return; _LL658:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp674=({ struct
_tuple4 _temp673; _temp673.f1= tud->fields; _temp673.f2= tud_opt; _temp673;});
struct Cyc_Core_Opt* _temp684; struct Cyc_Core_Opt* _temp686; struct Cyc_Core_Opt*
_temp688; struct Cyc_Core_Opt* _temp690; struct Cyc_Core_Opt _temp692; struct
Cyc_List_List* _temp693; struct Cyc_List_List** _temp695; struct Cyc_Core_Opt*
_temp696; struct Cyc_Core_Opt _temp698; struct Cyc_Absyn_Tuniondecl** _temp699;
struct Cyc_Core_Opt* _temp701; struct Cyc_Core_Opt _temp703; struct Cyc_List_List*
_temp704; struct Cyc_List_List** _temp706; struct Cyc_Core_Opt* _temp707; struct
Cyc_Core_Opt _temp709; struct Cyc_Absyn_Tuniondecl** _temp710; struct Cyc_Core_Opt*
_temp712; _LL676: _LL687: _temp686= _temp674.f1; if( _temp686 == 0){ goto _LL685;}
else{ goto _LL678;} _LL685: _temp684= _temp674.f2; if( _temp684 == 0){ goto
_LL677;} else{ goto _LL678;} _LL678: _LL691: _temp690= _temp674.f1; if( _temp690
== 0){ goto _LL680;} else{ _temp692=* _temp690; _LL694: _temp693=( struct Cyc_List_List*)
_temp692.v; _temp695=&(* _temp674.f1).v; goto _LL689;} _LL689: _temp688=
_temp674.f2; if( _temp688 == 0){ goto _LL679;} else{ goto _LL680;} _LL680:
_LL702: _temp701= _temp674.f1; if( _temp701 == 0){ goto _LL682;} else{ _temp703=*
_temp701; _LL705: _temp704=( struct Cyc_List_List*) _temp703.v; _temp706=&(*
_temp674.f1).v; goto _LL697;} _LL697: _temp696= _temp674.f2; if( _temp696 == 0){
goto _LL682;} else{ _temp698=* _temp696; _LL700: _temp699=( struct Cyc_Absyn_Tuniondecl**)
_temp698.v; goto _LL681;} _LL682: _LL713: _temp712= _temp674.f1; if( _temp712 ==
0){ goto _LL708;} else{ goto _LL675;} _LL708: _temp707= _temp674.f2; if(
_temp707 == 0){ goto _LL675;} else{ _temp709=* _temp707; _LL711: _temp710=(
struct Cyc_Absyn_Tuniondecl**) _temp709.v; goto _LL683;} _LL677: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp714=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp714[ 0]= tud; _temp714;}));
goto _LL675; _LL679: { struct Cyc_Absyn_Tuniondecl** _temp717=({ struct Cyc_Absyn_Tuniondecl**
_temp715=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp715[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp716=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp716->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp716->name= tud->name; _temp716->tvs= tvs; _temp716->fields=
0; _temp716->is_xtunion= tud->is_xtunion; _temp716;}); _temp715;}); goto _LL718;
_LL718: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp717);* _temp695= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp695, tvs, tud);* _temp717= tud; goto
_LL675;} _LL681: { struct Cyc_Absyn_Tuniondecl* _temp719=* _temp699; goto _LL720;
_LL720:* _temp699=({ struct Cyc_Absyn_Tuniondecl* _temp721=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp721->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp721->name= tud->name; _temp721->tvs= tvs; _temp721->fields=
0; _temp721->is_xtunion= tud->is_xtunion; _temp721;});* _temp706= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp706, tvs, tud);* _temp699=
_temp719; _temp710= _temp699; goto _LL683;} _LL683: { struct Cyc_Absyn_Tuniondecl*
_temp722= Cyc_Tcdecl_merge_tuniondecl(* _temp710, tud, loc, Cyc_Tc_tc_msg); goto
_LL723; _LL723: if( _temp722 == 0){ return;} else{* _temp710=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp722); goto _LL675;}} _LL675:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp724=(* ed->name).f1; struct Cyc_List_List* _temp732; struct Cyc_List_List*
_temp734; _LL726: if(( unsigned int) _temp724 > 1u?*(( int*) _temp724) == Cyc_Absyn_Rel_n:
0){ _LL733: _temp732=(( struct Cyc_Absyn_Rel_n_struct*) _temp724)->f1; if(
_temp732 == 0){ goto _LL727;} else{ goto _LL728;}} else{ goto _LL728;} _LL728:
if(( unsigned int) _temp724 > 1u?*(( int*) _temp724) == Cyc_Absyn_Abs_n: 0){
_LL735: _temp734=(( struct Cyc_Absyn_Abs_n_struct*) _temp724)->f1; if( _temp734
== 0){ goto _LL729;} else{ goto _LL730;}} else{ goto _LL730;} _LL730: goto
_LL731; _LL727: goto _LL725; _LL729: goto _LL725; _LL731: Cyc_Tcutil_terr( loc,({
unsigned char* _temp736=( unsigned char*)"qualified declarations are not implemented";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 43; _temp737;})); return; _LL725:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp738=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp738[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp739; _temp739.tag= Cyc_Absyn_Abs_n; _temp739.f1= te->ns; _temp739;});
_temp738;}); if( ed->fields != 0){ struct _RegionHandle _temp740= _new_region();
struct _RegionHandle* uprev_rgn=& _temp740; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp741=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL742; _LL742: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp741->name).f2)){ Cyc_Tcutil_terr(
_temp741->loc,( struct _tagged_string)({ struct _tagged_string _temp743=*(*
_temp741->name).f2; xprintf("duplicate field name %.*s", _temp743.last_plus_one
- _temp743.curr, _temp743.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp744=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp744->hd=( void*)(* _temp741->name).f2; _temp744->tl= prev_fields; _temp744;});}
if( _temp741->tag == 0){ _temp741->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp741->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp741->tag))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp745=* v; struct _tagged_string _temp746=*(* _temp741->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp745.last_plus_one
- _temp745.curr, _temp745.curr, _temp746.last_plus_one - _temp746.curr, _temp746.curr);}));}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp741->tag)); tag_count= t1 + 1;(* _temp741->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp747=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp747[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp748; _temp748.tag= Cyc_Absyn_Abs_n; _temp748.f1= te->ns; _temp748;});
_temp747;});}}}; _pop_region(& _temp740);}{ struct _handler_cons _temp749;
_push_handler(& _temp749);{ int _temp751= 0; if( setjmp( _temp749.handler)){
_temp751= 1;} if( ! _temp751){{ struct Cyc_Absyn_Enumdecl** _temp752=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->enumdecls, v); goto _LL753; _LL753: { struct Cyc_Absyn_Enumdecl*
_temp754= Cyc_Tcdecl_merge_enumdecl(* _temp752, ed, loc, Cyc_Tc_tc_msg); goto
_LL755; _LL755: if( _temp754 == 0){ _npop_handler( 0u); return;}* _temp752=(
struct Cyc_Absyn_Enumdecl*) _check_null( _temp754);}}; _pop_handler();} else{
void* _temp750=( void*) _exn_thrown; void* _temp757= _temp750; _LL759: if(
_temp757 == Cyc_Dict_Absent){ goto _LL760;} else{ goto _LL761;} _LL761: goto
_LL762; _LL760: { struct Cyc_Absyn_Enumdecl** _temp764=({ struct Cyc_Absyn_Enumdecl**
_temp763=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*));
_temp763[ 0]= ed; _temp763;}); goto _LL765; _LL765: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl**
data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp764); goto _LL758;} _LL762:(
void) _throw( _temp757); _LL758:;}}} if( ed->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp766=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL767; _LL767: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp766->name).f2,({ struct _tuple3* _temp768=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp768->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp769=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp769[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp770; _temp770.tag= Cyc_Tcenv_EnumRes;
_temp770.f1= ed; _temp770.f2= _temp766; _temp770;}); _temp769;}); _temp768->f2=
1; _temp768;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp771= sc; _LL773: if( _temp771 ==( void*) Cyc_Absyn_Static){
goto _LL774;} else{ goto _LL775;} _LL775: if( _temp771 ==( void*) Cyc_Absyn_Abstract){
goto _LL776;} else{ goto _LL777;} _LL777: if( _temp771 ==( void*) Cyc_Absyn_Public){
goto _LL778;} else{ goto _LL779;} _LL779: if( _temp771 ==( void*) Cyc_Absyn_Extern){
goto _LL780;} else{ goto _LL781;} _LL781: if( _temp771 ==( void*) Cyc_Absyn_ExternC){
goto _LL782;} else{ goto _LL772;} _LL774: Cyc_Tcutil_warn( loc,({ unsigned char*
_temp783=( unsigned char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp784; _temp784.curr= _temp783; _temp784.base= _temp783;
_temp784.last_plus_one= _temp783 + 44; _temp784;})); return 0; _LL776: Cyc_Tcutil_warn(
loc,({ unsigned char* _temp785=( unsigned char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 46; _temp786;})); return 0; _LL778: return 1;
_LL780: return 1; _LL782: Cyc_Tcutil_warn( loc,({ unsigned char* _temp787=(
unsigned char*)"nested extern \"C\" declaration"; struct _tagged_string _temp788;
_temp788.curr= _temp787; _temp788.base= _temp787; _temp788.last_plus_one=
_temp787 + 30; _temp788;})); return 1; _LL772:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp789= ds0; goto _LL790; _LL790: for( 0; _temp789 != 0;
_temp789=(( struct Cyc_List_List*) _check_null( _temp789))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp789))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp791=( void*) d->r; int
_temp817; struct Cyc_Absyn_Exp* _temp819; struct Cyc_Core_Opt* _temp821; struct
Cyc_Core_Opt* _temp823; struct Cyc_Absyn_Pat* _temp825; struct Cyc_List_List*
_temp827; struct Cyc_Absyn_Vardecl* _temp829; struct Cyc_Absyn_Fndecl* _temp831;
struct Cyc_Absyn_Typedefdecl* _temp833; struct Cyc_Absyn_Structdecl* _temp835;
struct Cyc_Absyn_Uniondecl* _temp837; struct Cyc_Absyn_Tuniondecl* _temp839;
struct Cyc_Absyn_Enumdecl* _temp841; struct Cyc_List_List* _temp843; struct
_tagged_string* _temp845; struct Cyc_List_List* _temp847; struct _tuple0*
_temp849; struct _tuple0 _temp851; struct _tagged_string* _temp852; void*
_temp854; struct Cyc_List_List* _temp856; _LL793: if(*(( int*) _temp791) == Cyc_Absyn_Let_d){
_LL826: _temp825=(( struct Cyc_Absyn_Let_d_struct*) _temp791)->f1; goto _LL824;
_LL824: _temp823=(( struct Cyc_Absyn_Let_d_struct*) _temp791)->f2; goto _LL822;
_LL822: _temp821=(( struct Cyc_Absyn_Let_d_struct*) _temp791)->f3; goto _LL820;
_LL820: _temp819=(( struct Cyc_Absyn_Let_d_struct*) _temp791)->f4; goto _LL818;
_LL818: _temp817=(( struct Cyc_Absyn_Let_d_struct*) _temp791)->f5; goto _LL794;}
else{ goto _LL795;} _LL795: if(*(( int*) _temp791) == Cyc_Absyn_Letv_d){ _LL828:
_temp827=(( struct Cyc_Absyn_Letv_d_struct*) _temp791)->f1; goto _LL796;} else{
goto _LL797;} _LL797: if(*(( int*) _temp791) == Cyc_Absyn_Var_d){ _LL830:
_temp829=(( struct Cyc_Absyn_Var_d_struct*) _temp791)->f1; goto _LL798;} else{
goto _LL799;} _LL799: if(*(( int*) _temp791) == Cyc_Absyn_Fn_d){ _LL832:
_temp831=(( struct Cyc_Absyn_Fn_d_struct*) _temp791)->f1; goto _LL800;} else{
goto _LL801;} _LL801: if(*(( int*) _temp791) == Cyc_Absyn_Typedef_d){ _LL834:
_temp833=(( struct Cyc_Absyn_Typedef_d_struct*) _temp791)->f1; goto _LL802;}
else{ goto _LL803;} _LL803: if(*(( int*) _temp791) == Cyc_Absyn_Struct_d){
_LL836: _temp835=(( struct Cyc_Absyn_Struct_d_struct*) _temp791)->f1; goto
_LL804;} else{ goto _LL805;} _LL805: if(*(( int*) _temp791) == Cyc_Absyn_Union_d){
_LL838: _temp837=(( struct Cyc_Absyn_Union_d_struct*) _temp791)->f1; goto _LL806;}
else{ goto _LL807;} _LL807: if(*(( int*) _temp791) == Cyc_Absyn_Tunion_d){
_LL840: _temp839=(( struct Cyc_Absyn_Tunion_d_struct*) _temp791)->f1; goto
_LL808;} else{ goto _LL809;} _LL809: if(*(( int*) _temp791) == Cyc_Absyn_Enum_d){
_LL842: _temp841=(( struct Cyc_Absyn_Enum_d_struct*) _temp791)->f1; goto _LL810;}
else{ goto _LL811;} _LL811: if(*(( int*) _temp791) == Cyc_Absyn_Namespace_d){
_LL846: _temp845=(( struct Cyc_Absyn_Namespace_d_struct*) _temp791)->f1; goto
_LL844; _LL844: _temp843=(( struct Cyc_Absyn_Namespace_d_struct*) _temp791)->f2;
goto _LL812;} else{ goto _LL813;} _LL813: if(*(( int*) _temp791) == Cyc_Absyn_Using_d){
_LL850: _temp849=(( struct Cyc_Absyn_Using_d_struct*) _temp791)->f1; _temp851=*
_temp849; _LL855: _temp854= _temp851.f1; goto _LL853; _LL853: _temp852= _temp851.f2;
goto _LL848; _LL848: _temp847=(( struct Cyc_Absyn_Using_d_struct*) _temp791)->f2;
goto _LL814;} else{ goto _LL815;} _LL815: if(*(( int*) _temp791) == Cyc_Absyn_ExternC_d){
_LL857: _temp856=(( struct Cyc_Absyn_ExternC_d_struct*) _temp791)->f1; goto
_LL816;} else{ goto _LL792;} _LL794: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp858=( unsigned char*)"top level let-declarations are not implemented";
struct _tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 47; _temp859;})); goto _LL792; _LL796: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp860=( unsigned char*)"top level let-declarations are not implemented";
struct _tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 47; _temp861;})); goto _LL792; _LL798: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp829->sc): 0){( void*)(
_temp829->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp829, check_var_init); goto _LL792; _LL800: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp831->sc): 0){( void*)( _temp831->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp831); goto _LL792; _LL802: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp833); goto _LL792; _LL804: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp835->sc): 0){( void*)( _temp835->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp835); goto _LL792; _LL806: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp837->sc): 0){( void*)( _temp837->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp837);
goto _LL792; _LL808: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp839->sc): 0){( void*)( _temp839->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp839); goto _LL792; _LL810: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp841->sc): 0){( void*)( _temp841->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp841);
goto _LL792; _LL812: { struct Cyc_List_List* _temp862= te->ns; goto _LL863;
_LL863: { struct Cyc_List_List* _temp865=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp862,({ struct Cyc_List_List*
_temp864=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp864->hd=( void*) _temp845; _temp864->tl= 0; _temp864;})); goto _LL866;
_LL866: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge->namespaces, _temp845)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp845); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp865, Cyc_Tcenv_empty_genv());}
te->ns= _temp865; Cyc_Tc_tc_decls( te, _temp843, in_externC, check_var_init); te->ns=
_temp862; goto _LL792;}} _LL814: { struct _tagged_string* first; struct Cyc_List_List*
rest;{ void* _temp867= _temp854; struct Cyc_List_List* _temp879; struct Cyc_List_List*
_temp881; struct Cyc_List_List* _temp883; struct Cyc_List_List _temp885; struct
Cyc_List_List* _temp886; struct _tagged_string* _temp888; struct Cyc_List_List*
_temp890; struct Cyc_List_List _temp892; struct Cyc_List_List* _temp893; struct
_tagged_string* _temp895; _LL869: if( _temp867 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL870;} else{ goto _LL871;} _LL871: if(( unsigned int) _temp867 > 1u?*(( int*)
_temp867) == Cyc_Absyn_Rel_n: 0){ _LL880: _temp879=(( struct Cyc_Absyn_Rel_n_struct*)
_temp867)->f1; if( _temp879 == 0){ goto _LL872;} else{ goto _LL873;}} else{ goto
_LL873;} _LL873: if(( unsigned int) _temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_Abs_n:
0){ _LL882: _temp881=(( struct Cyc_Absyn_Abs_n_struct*) _temp867)->f1; if(
_temp881 == 0){ goto _LL874;} else{ goto _LL875;}} else{ goto _LL875;} _LL875:
if(( unsigned int) _temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_Rel_n: 0){
_LL884: _temp883=(( struct Cyc_Absyn_Rel_n_struct*) _temp867)->f1; if( _temp883
== 0){ goto _LL877;} else{ _temp885=* _temp883; _LL889: _temp888=( struct
_tagged_string*) _temp885.hd; goto _LL887; _LL887: _temp886= _temp885.tl; goto
_LL876;}} else{ goto _LL877;} _LL877: if(( unsigned int) _temp867 > 1u?*(( int*)
_temp867) == Cyc_Absyn_Abs_n: 0){ _LL891: _temp890=(( struct Cyc_Absyn_Abs_n_struct*)
_temp867)->f1; if( _temp890 == 0){ goto _LL868;} else{ _temp892=* _temp890;
_LL896: _temp895=( struct _tagged_string*) _temp892.hd; goto _LL894; _LL894:
_temp893= _temp892.tl; goto _LL878;}} else{ goto _LL868;} _LL870: goto _LL872;
_LL872: goto _LL874; _LL874: first= _temp852; rest= 0; goto _LL868; _LL876:
_temp895= _temp888; _temp893= _temp886; goto _LL878; _LL878: first= _temp895;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp893,({ struct Cyc_List_List* _temp897=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp897->hd=( void*) _temp852;
_temp897->tl= 0; _temp897;})); goto _LL868; _LL868:;}{ struct Cyc_List_List*
_temp898= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); goto _LL899;
_LL899: ge->availables=({ struct Cyc_List_List* _temp900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp900->hd=( void*) _temp898;
_temp900->tl= ge->availables; _temp900;}); Cyc_Tc_tc_decls( te, _temp847,
in_externC, check_var_init); ge->availables=(( struct Cyc_List_List*)
_check_null( ge->availables))->tl; goto _LL792;}} _LL816: Cyc_Tc_tc_decls( te,
_temp856, 1, check_var_init); goto _LL792; _LL792:;}} void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int check_var_init, struct Cyc_List_List* ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls( te, ds, 0, check_var_init);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d); static struct Cyc_List_List*
Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return((
struct Cyc_List_List*(*)( int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*),
struct Cyc_Dict_Dict* env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed,
env, ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; }
; static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp901=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp911; struct Cyc_List_List*
_temp913; struct Cyc_List_List** _temp915; struct _tuple0* _temp916; struct Cyc_List_List*
_temp918; struct Cyc_List_List** _temp920; struct _tagged_string* _temp921;
_LL903: if(*(( int*) _temp901) == Cyc_Absyn_Var_d){ _LL912: _temp911=(( struct
Cyc_Absyn_Var_d_struct*) _temp901)->f1; goto _LL904;} else{ goto _LL905;} _LL905:
if(*(( int*) _temp901) == Cyc_Absyn_Using_d){ _LL917: _temp916=(( struct Cyc_Absyn_Using_d_struct*)
_temp901)->f1; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_Using_d_struct*)
_temp901)->f2; _temp915=&(( struct Cyc_Absyn_Using_d_struct*) _temp901)->f2;
goto _LL906;} else{ goto _LL907;} _LL907: if(*(( int*) _temp901) == Cyc_Absyn_Namespace_d){
_LL922: _temp921=(( struct Cyc_Absyn_Namespace_d_struct*) _temp901)->f1; goto
_LL919; _LL919: _temp918=(( struct Cyc_Absyn_Namespace_d_struct*) _temp901)->f2;
_temp920=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp901)->f2; goto _LL908;}
else{ goto _LL909;} _LL909: goto _LL910; _LL904: if(( void*) _temp911->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp925; struct
_tagged_string* _temp926; void* _temp928; struct _tuple0* _temp923= _temp911->name;
_temp925=* _temp923; _LL929: _temp928= _temp925.f1; goto _LL927; _LL927:
_temp926= _temp925.f2; goto _LL924; _LL924: { struct Cyc_List_List* ns;{ void*
_temp930= _temp928; struct Cyc_List_List* _temp938; struct Cyc_List_List*
_temp940; _LL932: if( _temp930 ==( void*) Cyc_Absyn_Loc_n){ goto _LL933;} else{
goto _LL934;} _LL934: if(( unsigned int) _temp930 > 1u?*(( int*) _temp930) ==
Cyc_Absyn_Rel_n: 0){ _LL939: _temp938=(( struct Cyc_Absyn_Rel_n_struct*)
_temp930)->f1; goto _LL935;} else{ goto _LL936;} _LL936: if(( unsigned int)
_temp930 > 1u?*(( int*) _temp930) == Cyc_Absyn_Abs_n: 0){ _LL941: _temp940=((
struct Cyc_Absyn_Abs_n_struct*) _temp930)->f1; goto _LL937;} else{ goto _LL931;}
_LL933: ns= 0; goto _LL931; _LL935: ns= _temp938; goto _LL931; _LL937: ns=
_temp940; goto _LL931; _LL931:;}{ struct _tuple6* _temp942=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
goto _LL943; _LL943: { struct Cyc_Tcenv_Genv* _temp944=(* _temp942).f1; goto
_LL945; _LL945: { int _temp946=(*(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp944->ordinaries, _temp926)).f2;
goto _LL947; _LL947: if( ! _temp946){(* _temp942).f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((* _temp942).f2,
_temp926);} return _temp946;}}}}} _LL906: _temp920= _temp915; goto _LL908;
_LL908:* _temp920= Cyc_Tc_treeshake_f( env,* _temp920); return 1; _LL910: return
1; _LL902:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set* set,
struct _tagged_string* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp948=(* nsenv).f1; goto _LL949; _LL949: { struct Cyc_Set_Set* _temp950=(*
nsenv).f2; goto _LL951; _LL951: _temp948->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple3*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
_temp950, _temp948->ordinaries);}} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp952=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp952->f1= ge; _temp952->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp952;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp953=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
goto _LL954; _LL954: { struct Cyc_List_List* _temp955= Cyc_Tc_treeshake_f(
_temp953, ds); goto _LL956; _LL956:(( void(*)( void(* f)( struct Cyc_List_List*,
struct _tuple6*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
_temp953); return _temp955;}}