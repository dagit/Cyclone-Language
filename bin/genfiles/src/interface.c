 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern void* Cyc_Core_identity( void*); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u];
extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern struct _tuple0* Cyc_Absyn_exn_name; extern
struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud; extern struct _tuple0* Cyc_Absyn_null_pointer_exn_name;
extern struct _tuple0* Cyc_Absyn_match_exn_name; extern struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_null_pointer_exn_tuf; extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf;
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
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
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_arr f1; struct
_tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); extern void Cyc_Interface_save(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_load( struct Cyc_Stdio___sFILE*); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1;
int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1;
int rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct Cyc_List_List*
ds); extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
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
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[
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
struct _tagged_arr text; } ; extern int Cyc_String_strptrcmp( struct _tagged_arr*
s1, struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); extern unsigned char Cyc_Tcutil_TypeErr[ 12u];
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_function_type(
void* t); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl* base; struct Cyc_Absyn_Tunionfield* field; } ;
extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg0, struct _tagged_arr msg1); extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports;
struct Cyc_Dict_Dict* exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});} static struct Cyc_Interface_Ienv*
Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv* ie= 0; if( ie
== 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();} return(
struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
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
_temp3= Cyc_Interface_empty(); struct Cyc_Interface_Ienv* _temp4= Cyc_Interface_new_ienv();
struct Cyc_Absyn_Tuniondecl* _temp5=({ struct Cyc_Absyn_Tuniondecl* _temp23=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp23[ 0]=* Cyc_Absyn_exn_tud; _temp23;});( void*)( _temp5->sc=( void*)(( void*)
Cyc_Absyn_Public)); _temp4->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp4->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp5);{ struct Cyc_Absyn_Tunionfield*
_temp6=({ struct Cyc_Absyn_Tunionfield* _temp22=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp22[ 0]=* Cyc_Absyn_null_pointer_exn_tuf;
_temp22;});( void*)( _temp6->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp7=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp21=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp21->base= _temp5; _temp21->field= _temp6; _temp21;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*
Cyc_Absyn_null_pointer_exn_name).f2, _temp7);{ struct Cyc_Absyn_Tunionfield*
_temp8=({ struct Cyc_Absyn_Tunionfield* _temp20=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp20[ 0]=* Cyc_Absyn_match_exn_tuf;
_temp20;});( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp9=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp19=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp19->base= _temp5; _temp19->field= _temp8; _temp19;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*
Cyc_Absyn_match_exn_name).f2, _temp9); _temp3->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp4);{ struct Cyc_Interface_Ienv*
_temp10= Cyc_Interface_new_ienv(); struct _tuple0* qmain=({ struct _tuple0*
_temp16=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp16->f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp17=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp17[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp18; _temp18.tag= Cyc_Absyn_Abs_n; _temp18.f1= 0; _temp18;}); _temp17;});
_temp16->f2= _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"main", sizeof( unsigned char), 5u); _temp16;}); struct Cyc_Absyn_Vardecl*
_temp11= Cyc_Absyn_new_vardecl( qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({
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
0, 0, 0), 0);( void*)( _temp11->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp10->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp10->vardecls,(* qmain).f2,
_temp11); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp10); return _temp3;}}}}}} static void Cyc_Interface_err(
struct _tagged_arr msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_arr sc, struct _tagged_arr t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_arr)({ struct _tagged_arr _temp24= sc; struct _tagged_arr _temp25=
t; struct _tagged_arr _temp26= Cyc_Absynpp_qvar2string( n); xprintf("fields of %.*s %.*s %.*s have never been defined",
_get_arr_size( _temp24, 1u), _temp24.curr, _get_arr_size( _temp25, 1u), _temp25.curr,
_get_arr_size( _temp26, 1u), _temp26.curr);}));} static void Cyc_Interface_body_err(
struct _tagged_arr sc, struct _tuple0* n){ Cyc_Interface_err(( struct
_tagged_arr)({ struct _tagged_arr _temp27= sc; struct _tagged_arr _temp28= Cyc_Absynpp_qvar2string(
n); xprintf("the body of %.*s function %.*s has never been defined",
_get_arr_size( _temp27, 1u), _temp27.curr, _get_arr_size( _temp28, 1u), _temp28.curr);}));}
static void Cyc_Interface_static_err( struct _tagged_arr obj1, struct _tuple0*
name1, struct _tagged_arr obj2, struct _tuple0* name2){ if( obj1.curr !=((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Interface_err(( struct
_tagged_arr)({ struct _tagged_arr _temp29= obj1; struct _tagged_arr _temp30= Cyc_Absynpp_qvar2string(
name1); struct _tagged_arr _temp31= obj2; struct _tagged_arr _temp32= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s",
_get_arr_size( _temp29, 1u), _temp29.curr, _get_arr_size( _temp30, 1u), _temp30.curr,
_get_arr_size( _temp31, 1u), _temp31.curr, _get_arr_size( _temp32, 1u), _temp32.curr);}));}}
static void Cyc_Interface_abstract_err( struct _tagged_arr obj1, struct _tuple0*
name1, struct _tagged_arr obj2, struct _tuple0* name2){ if( obj1.curr !=((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Interface_err(( struct
_tagged_arr)({ struct _tagged_arr _temp33= obj1; struct _tagged_arr _temp34= Cyc_Absynpp_qvar2string(
name1); struct _tagged_arr _temp35= obj2; struct _tagged_arr _temp36= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_get_arr_size( _temp33, 1u), _temp33.curr, _get_arr_size( _temp34, 1u), _temp34.curr,
_get_arr_size( _temp35, 1u), _temp35.curr, _get_arr_size( _temp36, 1u), _temp36.curr);}));}}
struct Cyc_Interface_Seen{ struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict*
unions; struct Cyc_Dict_Dict* tunions; } ; static struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){
return({ struct Cyc_Interface_Seen* _temp37=( struct Cyc_Interface_Seen*)
GC_malloc( sizeof( struct Cyc_Interface_Seen)); _temp37->structs=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp37->unions=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp37->tunions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp37;});} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp38= 1; for( 0; l != 0; l=(( struct Cyc_List_List*) _check_null( l))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, obj, name, f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd))){ _temp38= 0;}} return _temp38;} static
int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Structdecl* d){ struct _handler_cons _temp39;
_push_handler(& _temp39);{ int _temp41= 0; if( setjmp( _temp39.handler)){
_temp41= 1;} if( ! _temp41){{ int _temp42=(( int(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u); return
_temp42;}; _pop_handler();} else{ void* _temp40=( void*) _exn_thrown; void*
_temp44= _temp40; _LL46: if( _temp44 == Cyc_Dict_Absent){ goto _LL47;} else{
goto _LL48;} _LL48: goto _LL49; _LL47: { int _temp50= 1; seen->structs=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)(
seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,
_temp50); if( d->fields != 0){ struct Cyc_List_List* _temp51=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; for( 0; _temp51 != 0; _temp51=((
struct Cyc_List_List*) _check_null( _temp51))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp51))->hd)->type)){ _temp50= 0;}}} seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp50); return _temp50;} _LL49:( void) _throw(
_temp44); _LL45:;}}} static int Cyc_Interface_check_public_uniondecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl* d){ struct
_handler_cons _temp52; _push_handler(& _temp52);{ int _temp54= 0; if( setjmp(
_temp52.handler)){ _temp54= 1;} if( ! _temp54){{ int _temp55=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp55;}; _pop_handler();} else{ void* _temp53=( void*) _exn_thrown;
void* _temp57= _temp53; _LL59: if( _temp57 == Cyc_Dict_Absent){ goto _LL60;}
else{ goto _LL61;} _LL61: goto _LL62; _LL60: { int _temp63= 1; seen->unions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->unions,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp63); if( d->fields != 0){ struct Cyc_List_List*
_temp64=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp64 != 0; _temp64=(( struct Cyc_List_List*) _check_null( _temp64))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp64))->hd)->type)){ _temp63= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp63);
return _temp63;} _LL62:( void) _throw( _temp57); _LL58:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp65; _push_handler(& _temp65);{ int _temp67= 0; if(
setjmp( _temp65.handler)){ _temp67= 1;} if( ! _temp67){{ int _temp68=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp68;}; _pop_handler();} else{ void*
_temp66=( void*) _exn_thrown; void* _temp70= _temp66; _LL72: if( _temp70 == Cyc_Dict_Absent){
goto _LL73;} else{ goto _LL74;} _LL74: goto _LL75; _LL73: { int _temp76= 1; seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, _temp76); if( d->fields != 0){ struct
Cyc_List_List* _temp77=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; for( 0; _temp77 != 0; _temp77=(( struct Cyc_List_List*)
_check_null( _temp77))->tl){ if( !(( int(*)( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name, void*(*
f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), d->name, Cyc_Interface_get_type1,((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp77))->hd)->typs)){
_temp76= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp76); return _temp76;} _LL75:( void) _throw( _temp70); _LL71:;}}} static int
Cyc_Interface_check_public_enumdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Enumdecl* d){ return 1;} static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl*
d){ return Cyc_Interface_check_public_type( ae, seen,( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen, _tag_arr("variable",
sizeof( unsigned char), 9u), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns(
void* ns){ void* _temp78= ns; struct Cyc_List_List* _temp84; _LL80: if((
unsigned int) _temp78 > 1u?*(( int*) _temp78) == Cyc_Absyn_Abs_n: 0){ _LL85:
_temp84=(( struct Cyc_Absyn_Abs_n_struct*) _temp78)->f1; goto _LL81;} else{ goto
_LL82;} _LL82: goto _LL83; _LL81: return _temp84; _LL83:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp86=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp86[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp87; _temp87.tag= Cyc_Core_InvalidArg; _temp87.f1=
_tag_arr("get_abs_ns", sizeof( unsigned char), 11u); _temp87;}); _temp86;}));
_LL79:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name,
void* t){ void* _temp88= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp112; struct Cyc_Absyn_Conref* _temp114; struct Cyc_Absyn_Tqual _temp116;
struct Cyc_Absyn_Conref* _temp118; void* _temp120; void* _temp122; struct Cyc_Absyn_Exp*
_temp124; struct Cyc_Absyn_Tqual _temp126; void* _temp128; struct Cyc_Core_Opt*
_temp130; struct Cyc_Core_Opt _temp132; void* _temp133; struct Cyc_List_List*
_temp135; struct _tuple0* _temp137; struct Cyc_Absyn_FnInfo _temp139; struct Cyc_List_List*
_temp141; struct Cyc_List_List* _temp143; struct Cyc_Absyn_VarargInfo* _temp145;
int _temp147; struct Cyc_List_List* _temp149; void* _temp151; struct Cyc_Core_Opt*
_temp153; struct Cyc_List_List* _temp155; struct Cyc_List_List* _temp157; struct
Cyc_Absyn_Structdecl** _temp159; struct Cyc_Absyn_Structdecl* _temp161; struct
Cyc_List_List* _temp162; struct _tuple0* _temp164; struct Cyc_Absyn_Uniondecl**
_temp166; struct Cyc_Absyn_Uniondecl* _temp168; struct Cyc_List_List* _temp169;
struct _tuple0* _temp171; struct Cyc_Absyn_Enumdecl* _temp173; struct _tuple0*
_temp175; struct Cyc_Absyn_TunionInfo _temp177; void* _temp179; struct Cyc_List_List*
_temp181; void* _temp183; struct Cyc_Absyn_Tuniondecl* _temp185; struct Cyc_Absyn_TunionFieldInfo
_temp187; struct Cyc_List_List* _temp189; void* _temp191; struct Cyc_Absyn_Tunionfield*
_temp193; struct Cyc_Absyn_Tuniondecl* _temp195; _LL90: if(( unsigned int)
_temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_PointerType: 0){ _LL113: _temp112=((
struct Cyc_Absyn_PointerType_struct*) _temp88)->f1; _LL123: _temp122=( void*)
_temp112.elt_typ; goto _LL121; _LL121: _temp120=( void*) _temp112.rgn_typ; goto
_LL119; _LL119: _temp118= _temp112.nullable; goto _LL117; _LL117: _temp116=
_temp112.tq; goto _LL115; _LL115: _temp114= _temp112.bounds; goto _LL91;} else{
goto _LL92;} _LL92: if(( unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_ArrayType:
0){ _LL129: _temp128=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f1;
goto _LL127; _LL127: _temp126=(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f2;
goto _LL125; _LL125: _temp124=(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f3;
goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp88 > 4u?*(( int*)
_temp88) == Cyc_Absyn_TypedefType: 0){ _LL138: _temp137=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f1; goto _LL136; _LL136: _temp135=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f2; goto _LL131; _LL131: _temp130=(( struct Cyc_Absyn_TypedefType_struct*)
_temp88)->f3; if( _temp130 == 0){ goto _LL96;} else{ _temp132=* _temp130; _LL134:
_temp133=( void*) _temp132.v; goto _LL95;}} else{ goto _LL96;} _LL96: if((
unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_FnType: 0){ _LL140:
_temp139=(( struct Cyc_Absyn_FnType_struct*) _temp88)->f1; _LL156: _temp155=
_temp139.tvars; goto _LL154; _LL154: _temp153= _temp139.effect; goto _LL152;
_LL152: _temp151=( void*) _temp139.ret_typ; goto _LL150; _LL150: _temp149=
_temp139.args; goto _LL148; _LL148: _temp147= _temp139.c_varargs; goto _LL146;
_LL146: _temp145= _temp139.cyc_varargs; goto _LL144; _LL144: _temp143= _temp139.rgn_po;
goto _LL142; _LL142: _temp141= _temp139.attributes; goto _LL97;} else{ goto
_LL98;} _LL98: if(( unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_TupleType:
0){ _LL158: _temp157=(( struct Cyc_Absyn_TupleType_struct*) _temp88)->f1; goto
_LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp88 > 4u?*(( int*)
_temp88) == Cyc_Absyn_StructType: 0){ _LL165: _temp164=(( struct Cyc_Absyn_StructType_struct*)
_temp88)->f1; goto _LL163; _LL163: _temp162=(( struct Cyc_Absyn_StructType_struct*)
_temp88)->f2; goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_StructType_struct*)
_temp88)->f3; if( _temp159 == 0){ goto _LL102;} else{ _temp161=* _temp159; goto
_LL101;}} else{ goto _LL102;} _LL102: if(( unsigned int) _temp88 > 4u?*(( int*)
_temp88) == Cyc_Absyn_UnionType: 0){ _LL172: _temp171=(( struct Cyc_Absyn_UnionType_struct*)
_temp88)->f1; goto _LL170; _LL170: _temp169=(( struct Cyc_Absyn_UnionType_struct*)
_temp88)->f2; goto _LL167; _LL167: _temp166=(( struct Cyc_Absyn_UnionType_struct*)
_temp88)->f3; if( _temp166 == 0){ goto _LL104;} else{ _temp168=* _temp166; goto
_LL103;}} else{ goto _LL104;} _LL104: if(( unsigned int) _temp88 > 4u?*(( int*)
_temp88) == Cyc_Absyn_EnumType: 0){ _LL176: _temp175=(( struct Cyc_Absyn_EnumType_struct*)
_temp88)->f1; goto _LL174; _LL174: _temp173=(( struct Cyc_Absyn_EnumType_struct*)
_temp88)->f2; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_TunionType: 0){ _LL178: _temp177=((
struct Cyc_Absyn_TunionType_struct*) _temp88)->f1; _LL184: _temp183=( void*)
_temp177.tunion_info; if(*(( int*) _temp183) == Cyc_Absyn_KnownTunion){ _LL186:
_temp185=(( struct Cyc_Absyn_KnownTunion_struct*) _temp183)->f1; goto _LL182;}
else{ goto _LL108;} _LL182: _temp181= _temp177.targs; goto _LL180; _LL180:
_temp179=( void*) _temp177.rgn; goto _LL107;} else{ goto _LL108;} _LL108: if((
unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_TunionFieldType: 0){
_LL188: _temp187=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp88)->f1;
_LL192: _temp191=( void*) _temp187.field_info; if(*(( int*) _temp191) == Cyc_Absyn_KnownTunionfield){
_LL196: _temp195=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp191)->f1;
goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp191)->f2; goto _LL190;} else{ goto _LL110;} _LL190: _temp189= _temp187.targs;
goto _LL109;} else{ goto _LL110;} _LL110: goto _LL111; _LL91: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp122); _LL93: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp128); _LL95: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp133); _LL97: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp149)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp151): 0; if( _temp145 != 0){ int _temp199; void*
_temp201; void* _temp203; struct Cyc_Absyn_Tqual _temp205; struct Cyc_Core_Opt*
_temp207; struct Cyc_Absyn_VarargInfo _temp197=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp145)); _LL208: _temp207= _temp197.name; goto _LL206; _LL206:
_temp205= _temp197.tq; goto _LL204; _LL204: _temp203=( void*) _temp197.type;
goto _LL202; _LL202: _temp201=( void*) _temp197.rgn; goto _LL200; _LL200:
_temp199= _temp197.inject; goto _LL198; _LL198: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp203);} return b;} _LL99: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp157); _LL101: if(( void*)
_temp161->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp161->name))->v); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp162)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp161): 0; _LL103: if(( void*) _temp168->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp168->name))->v);
return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_arr obj, struct _tuple0* name, void*(* f)( void*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,(
void*(*)( void*)) Cyc_Core_identity, _temp169)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp168): 0; _LL105: { struct _tuple0 _temp211; struct _tagged_arr*
_temp212; void* _temp214; struct _tuple0* _temp209= _temp175; _temp211=*
_temp209; _LL215: _temp214= _temp211.f1; goto _LL213; _LL213: _temp212= _temp211.f2;
goto _LL210; _LL210: { struct Cyc_List_List* _temp216= Cyc_Interface_get_abs_ns(
_temp214); struct Cyc_Absyn_Enumdecl* ed;{ struct _handler_cons _temp217;
_push_handler(& _temp217);{ int _temp219= 0; if( setjmp( _temp217.handler)){
_temp219= 1;} if( ! _temp219){{ struct Cyc_Tcenv_Genv* _temp220=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae,
_temp216); ed=*(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp220->enumdecls, _temp212);};
_pop_handler();} else{ void* _temp218=( void*) _exn_thrown; void* _temp222=
_temp218; _LL224: if( _temp222 == Cyc_Dict_Absent){ goto _LL225;} else{ goto
_LL226;} _LL226: goto _LL227; _LL225:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp228=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp228[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp229; _temp229.tag= Cyc_Core_InvalidArg;
_temp229.f1=( struct _tagged_arr)({ struct _tagged_arr _temp230= Cyc_Absynpp_qvar2string(
_temp175); xprintf("check_public_type (can't find enum %.*s)", _get_arr_size(
_temp230, 1u), _temp230.curr);}); _temp229;}); _temp228;})); _LL227:( void)
_throw( _temp222); _LL223:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp175, _tag_arr("enum", sizeof( unsigned char),
5u), ed->name); return 0;} return 1;}} _LL107: { struct _tuple0 _temp233; struct
_tagged_arr* _temp234; void* _temp236; struct _tuple0* _temp231= _temp185->name;
_temp233=* _temp231; _LL237: _temp236= _temp233.f1; goto _LL235; _LL235:
_temp234= _temp233.f2; goto _LL232; _LL232: { struct Cyc_List_List* _temp238=
Cyc_Interface_get_abs_ns( _temp236); struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp239; _push_handler(& _temp239);{ int _temp241= 0; if( setjmp(
_temp239.handler)){ _temp241= 1;} if( ! _temp241){{ struct Cyc_Tcenv_Genv*
_temp242=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp238); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp242->tuniondecls,
_temp234);}; _pop_handler();} else{ void* _temp240=( void*) _exn_thrown; void*
_temp244= _temp240; _LL246: if( _temp244 == Cyc_Dict_Absent){ goto _LL247;}
else{ goto _LL248;} _LL248: goto _LL249; _LL247:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp250=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp250[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp251; _temp251.tag= Cyc_Core_InvalidArg; _temp251.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp252= Cyc_Absynpp_qvar2string(
_temp185->name); xprintf("check_public_type (can't find [x]tunion %.*s)",
_get_arr_size( _temp252, 1u), _temp252.curr);}); _temp251;}); _temp250;}));
_LL249:( void) _throw( _temp244); _LL245:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("[x]tunion", sizeof( unsigned char),
10u), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name, void*(*
f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae,
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp181)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}} _LL109: { struct _tuple0 _temp255; struct _tagged_arr*
_temp256; void* _temp258; struct _tuple0* _temp253= _temp195->name; _temp255=*
_temp253; _LL259: _temp258= _temp255.f1; goto _LL257; _LL257: _temp256= _temp255.f2;
goto _LL254; _LL254: { struct Cyc_List_List* _temp260= Cyc_Interface_get_abs_ns(
_temp258); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp261;
_push_handler(& _temp261);{ int _temp263= 0; if( setjmp( _temp261.handler)){
_temp263= 1;} if( ! _temp263){{ struct Cyc_Tcenv_Genv* _temp264=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae,
_temp260); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp264->tuniondecls, _temp256);};
_pop_handler();} else{ void* _temp262=( void*) _exn_thrown; void* _temp266=
_temp262; _LL268: if( _temp266 == Cyc_Dict_Absent){ goto _LL269;} else{ goto
_LL270;} _LL270: goto _LL271; _LL269:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp272=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp272[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp273; _temp273.tag= Cyc_Core_InvalidArg;
_temp273.f1=( struct _tagged_arr)({ struct _tagged_arr _temp274= Cyc_Absynpp_qvar2string(
_temp195->name); xprintf("check_public_type (can't find [x]tunion %.*s and search its fields)",
_get_arr_size( _temp274, 1u), _temp274.curr);}); _temp273;}); _temp272;}));
_LL271:( void) _throw( _temp266); _LL267:;}}} if( tud->fields == 0){( int)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp275=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp275[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp276; _temp276.tag= Cyc_Core_InvalidArg; _temp276.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp277= Cyc_Absynpp_qvar2string( tud->name);
xprintf("check_public_type ([x]tunion %.*s has no fields)", _get_arr_size(
_temp277, 1u), _temp277.curr);}); _temp276;}); _temp275;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp278=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; for( 0; _temp278 != 0; _temp278=(( struct Cyc_List_List*)
_check_null( _temp278))->tl){ if( Cyc_String_strptrcmp((* _temp193->name).f2,(*((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp278))->hd)->name).f2)
== 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp278))->hd); break;}}} if( tuf1 == 0){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp279=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp279[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp280; _temp280.tag= Cyc_Core_InvalidArg; _temp280.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp281= Cyc_Absynpp_qvar2string(
_temp193->name); xprintf("check_public_type (can't find [x]tunionfield %.*s)",
_get_arr_size( _temp281, 1u), _temp281.curr);}); _temp280;}); _temp279;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*) _check_null(
tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct _tagged_arr _temp282= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _get_arr_size( _temp282, 1u), _temp282.curr);}),
tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_arr obj, struct _tuple0* name, void*(* f)( void*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,(
void*(*)( void*)) Cyc_Core_identity, _temp189)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}} _LL111: return 1; _LL89:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp285; struct Cyc_Interface_I* _temp286; struct Cyc_Interface_Seen*
_temp288; struct Cyc_Dict_Dict* _temp290; int _temp292; struct Cyc_Interface_Ienv*
_temp294; struct Cyc_Interface_Ienv* _temp296; struct _tuple11* _temp283= env;
_temp285=* _temp283; _LL297: _temp296= _temp285.f1; goto _LL295; _LL295:
_temp294= _temp285.f2; goto _LL293; _LL293: _temp292= _temp285.f3; goto _LL291;
_LL291: _temp290= _temp285.f4; goto _LL289; _LL289: _temp288= _temp285.f5; goto
_LL287; _LL287: _temp286= _temp285.f6; goto _LL284; _LL284: { struct Cyc_Absyn_Structdecl*
_temp298=* dp; void* _temp299=( void*) _temp298->sc; _LL301: if( _temp299 ==(
void*) Cyc_Absyn_Static){ goto _LL302;} else{ goto _LL303;} _LL303: if( _temp299
==( void*) Cyc_Absyn_Abstract){ goto _LL304;} else{ goto _LL305;} _LL305: if(
_temp299 ==( void*) Cyc_Absyn_Public){ goto _LL306;} else{ goto _LL307;} _LL307:
if( _temp299 ==( void*) Cyc_Absyn_ExternC){ goto _LL308;} else{ goto _LL309;}
_LL309: if( _temp299 ==( void*) Cyc_Absyn_Extern){ goto _LL310;} else{ goto
_LL300;} _LL302: if( _temp292? _temp298->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp298->name))->v);} goto _LL300; _LL304: if( _temp298->fields == 0){ if(
_temp292){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp298->name))->v);}} else{ _temp298=({ struct Cyc_Absyn_Structdecl*
_temp311=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp311[ 0]=* _temp298; _temp311;}); _temp298->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp290, _temp288, _temp298)){ _temp294->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp294->structdecls, x, _temp298);} goto _LL300;
_LL306: if( _temp298->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp298->name))->v);
_temp298=({ struct Cyc_Absyn_Structdecl* _temp312=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp312[ 0]=* _temp298;
_temp312;});( void*)( _temp298->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp290, _temp288, _temp298)){ _temp294->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp294->structdecls, x,
_temp298);} goto _LL300; _LL308: goto _LL310; _LL310: if( Cyc_Interface_check_public_structdecl(
_temp290, _temp288, _temp298)){ _temp296->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp296->structdecls, x, _temp298);} goto _LL300;
_LL300:;}} static void Cyc_Interface_extract_uniondecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11
_temp315; struct Cyc_Interface_I* _temp316; struct Cyc_Interface_Seen* _temp318;
struct Cyc_Dict_Dict* _temp320; int _temp322; struct Cyc_Interface_Ienv*
_temp324; struct Cyc_Interface_Ienv* _temp326; struct _tuple11* _temp313= env;
_temp315=* _temp313; _LL327: _temp326= _temp315.f1; goto _LL325; _LL325:
_temp324= _temp315.f2; goto _LL323; _LL323: _temp322= _temp315.f3; goto _LL321;
_LL321: _temp320= _temp315.f4; goto _LL319; _LL319: _temp318= _temp315.f5; goto
_LL317; _LL317: _temp316= _temp315.f6; goto _LL314; _LL314: { struct Cyc_Absyn_Uniondecl*
_temp328=* dp; void* _temp329=( void*) _temp328->sc; _LL331: if( _temp329 ==(
void*) Cyc_Absyn_Static){ goto _LL332;} else{ goto _LL333;} _LL333: if( _temp329
==( void*) Cyc_Absyn_Abstract){ goto _LL334;} else{ goto _LL335;} _LL335: if(
_temp329 ==( void*) Cyc_Absyn_Public){ goto _LL336;} else{ goto _LL337;} _LL337:
if( _temp329 ==( void*) Cyc_Absyn_ExternC){ goto _LL338;} else{ goto _LL339;}
_LL339: if( _temp329 ==( void*) Cyc_Absyn_Extern){ goto _LL340;} else{ goto
_LL330;} _LL332: if( _temp322? _temp328->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp328->name))->v);} goto _LL330; _LL334: if( _temp328->fields == 0){ if(
_temp322){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp328->name))->v);}} else{ _temp328=({ struct Cyc_Absyn_Uniondecl*
_temp341=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp341[ 0]=* _temp328; _temp341;}); _temp328->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp320, _temp318, _temp328)){ _temp324->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp324->uniondecls, x, _temp328);} goto _LL330;
_LL336: if( _temp328->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp328->name))->v);
_temp328=({ struct Cyc_Absyn_Uniondecl* _temp342=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp342[ 0]=* _temp328;
_temp342;});( void*)( _temp328->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp320, _temp318, _temp328)){ _temp324->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp324->uniondecls, x,
_temp328);} goto _LL330; _LL338: goto _LL340; _LL340: if( Cyc_Interface_check_public_uniondecl(
_temp320, _temp318, _temp328)){ _temp326->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp326->uniondecls, x, _temp328);} goto _LL330;
_LL330:;}} static void Cyc_Interface_extract_enumdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple11 _temp345;
struct Cyc_Interface_I* _temp346; struct Cyc_Interface_Seen* _temp348; struct
Cyc_Dict_Dict* _temp350; int _temp352; struct Cyc_Interface_Ienv* _temp354;
struct Cyc_Interface_Ienv* _temp356; struct _tuple11* _temp343= env; _temp345=*
_temp343; _LL357: _temp356= _temp345.f1; goto _LL355; _LL355: _temp354= _temp345.f2;
goto _LL353; _LL353: _temp352= _temp345.f3; goto _LL351; _LL351: _temp350=
_temp345.f4; goto _LL349; _LL349: _temp348= _temp345.f5; goto _LL347; _LL347:
_temp346= _temp345.f6; goto _LL344; _LL344: { struct Cyc_Absyn_Enumdecl*
_temp358=* dp; void* _temp359=( void*) _temp358->sc; _LL361: if( _temp359 ==(
void*) Cyc_Absyn_Static){ goto _LL362;} else{ goto _LL363;} _LL363: if( _temp359
==( void*) Cyc_Absyn_Abstract){ goto _LL364;} else{ goto _LL365;} _LL365: if(
_temp359 ==( void*) Cyc_Absyn_Public){ goto _LL366;} else{ goto _LL367;} _LL367:
if( _temp359 ==( void*) Cyc_Absyn_ExternC){ goto _LL368;} else{ goto _LL369;}
_LL369: if( _temp359 ==( void*) Cyc_Absyn_Extern){ goto _LL370;} else{ goto
_LL360;} _LL362: if( _temp352? _temp358->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp358->name);} goto _LL360; _LL364: if( _temp358->fields
== 0){ if( _temp352){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp358->name);}}
else{ _temp358=({ struct Cyc_Absyn_Enumdecl* _temp371=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp371[ 0]=* _temp358;
_temp371;}); _temp358->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp350, _temp348, _temp358)){ _temp354->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp354->enumdecls, x, _temp358);} goto _LL360; _LL366:
if( _temp358->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp358->name);
_temp358=({ struct Cyc_Absyn_Enumdecl* _temp372=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp372[ 0]=* _temp358;
_temp372;});( void*)( _temp358->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp350, _temp348, _temp358)){ _temp354->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp354->enumdecls, x,
_temp358);} goto _LL360; _LL368: goto _LL370; _LL370: if( Cyc_Interface_check_public_enumdecl(
_temp350, _temp348, _temp358)){ _temp356->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp356->enumdecls, x, _temp358);} goto _LL360; _LL360:;}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp373= 0;{ void* _temp374=(* f->name).f1; struct Cyc_List_List* _temp382;
struct Cyc_List_List* _temp384; _LL376: if(( unsigned int) _temp374 > 1u?*(( int*)
_temp374) == Cyc_Absyn_Rel_n: 0){ _LL383: _temp382=(( struct Cyc_Absyn_Rel_n_struct*)
_temp374)->f1; goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int)
_temp374 > 1u?*(( int*) _temp374) == Cyc_Absyn_Abs_n: 0){ _LL385: _temp384=((
struct Cyc_Absyn_Abs_n_struct*) _temp374)->f1; goto _LL379;} else{ goto _LL380;}
_LL380: goto _LL381; _LL377: _temp384= _temp382; goto _LL379; _LL379: _temp373=
_temp384; goto _LL375; _LL381: goto _LL375; _LL375:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp386=( void*) f->sc; _LL388: if( _temp386 ==( void*) Cyc_Absyn_Static){
goto _LL389;} else{ goto _LL390;} _LL390: if( _temp386 ==( void*) Cyc_Absyn_Extern){
goto _LL391;} else{ goto _LL392;} _LL392: if( _temp386 ==( void*) Cyc_Absyn_Public){
goto _LL393;} else{ goto _LL394;} _LL394: goto _LL395; _LL389: return; _LL391:
dict=& i->imports; goto _LL387; _LL393: dict=& i->exports; goto _LL387; _LL395:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp396=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp396[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp397; _temp397.tag= Cyc_Core_InvalidArg; _temp397.f1=
_tag_arr("add_xtunionfielddecl", sizeof( unsigned char), 21u); _temp397;});
_temp396;})); _LL387:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons
_temp398; _push_handler(& _temp398);{ int _temp400= 0; if( setjmp( _temp398.handler)){
_temp400= 1;} if( ! _temp400){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp373);; _pop_handler();}
else{ void* _temp399=( void*) _exn_thrown; void* _temp402= _temp399; _LL404: if(
_temp402 == Cyc_Dict_Absent){ goto _LL405;} else{ goto _LL406;} _LL406: goto
_LL407; _LL405: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp373, env); goto _LL403; _LL407:( void)
_throw( _temp402); _LL403:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp408=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp408->base= d; _temp408->field= f; _temp408;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp411; struct Cyc_Interface_I* _temp412; struct Cyc_Interface_Seen*
_temp414; struct Cyc_Dict_Dict* _temp416; int _temp418; struct Cyc_Interface_Ienv*
_temp420; struct Cyc_Interface_Ienv* _temp422; struct _tuple11* _temp409= env;
_temp411=* _temp409; _LL423: _temp422= _temp411.f1; goto _LL421; _LL421:
_temp420= _temp411.f2; goto _LL419; _LL419: _temp418= _temp411.f3; goto _LL417;
_LL417: _temp416= _temp411.f4; goto _LL415; _LL415: _temp414= _temp411.f5; goto
_LL413; _LL413: _temp412= _temp411.f6; goto _LL410; _LL410: { struct Cyc_Absyn_Tuniondecl*
_temp424=* dp; void* _temp425=( void*) _temp424->sc; _LL427: if( _temp425 ==(
void*) Cyc_Absyn_Static){ goto _LL428;} else{ goto _LL429;} _LL429: if( _temp425
==( void*) Cyc_Absyn_Abstract){ goto _LL430;} else{ goto _LL431;} _LL431: if(
_temp425 ==( void*) Cyc_Absyn_Public){ goto _LL432;} else{ goto _LL433;} _LL433:
if( _temp425 ==( void*) Cyc_Absyn_ExternC){ goto _LL434;} else{ goto _LL435;}
_LL435: if( _temp425 ==( void*) Cyc_Absyn_Extern){ goto _LL436;} else{ goto
_LL426;} _LL428: if(( ! _temp424->is_xtunion? _temp418: 0)? _temp424->fields ==
0: 0){ Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp424->name);} goto _LL426;
_LL430: if( _temp424->fields == 0){ if( ! _temp424->is_xtunion? _temp418: 0){
Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char), 9u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp424->name);}} else{
_temp424=({ struct Cyc_Absyn_Tuniondecl* _temp437=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp437[ 0]=* _temp424;
_temp437;}); _temp424->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp416, _temp414, _temp424)){ _temp420->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp420->tuniondecls, x, _temp424);} goto _LL426;
_LL432: _temp424=({ struct Cyc_Absyn_Tuniondecl* _temp438=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp438[ 0]=* _temp424;
_temp438;}); if( ! _temp424->is_xtunion? _temp424->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp424->name);( void*)( _temp424->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp416,
_temp414, _temp424)){ if( _temp424->is_xtunion? _temp424->fields != 0: 0){
struct Cyc_List_List* _temp439=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp424->fields))->v; _temp424->fields= 0;{ struct Cyc_List_List*
_temp440= _temp439; for( 0; _temp440 != 0; _temp440=(( struct Cyc_List_List*)
_check_null( _temp440))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp412,
_temp424,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp440))->hd);}}} _temp420->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp420->tuniondecls, x, _temp424);} goto _LL426; _LL434:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp441=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp441[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp442; _temp442.tag= Cyc_Core_InvalidArg; _temp442.f1=
_tag_arr("extract_tuniondecl", sizeof( unsigned char), 19u); _temp442;});
_temp441;})); _LL436: if( Cyc_Interface_check_public_tuniondecl( _temp416,
_temp414, _temp424)){ if( _temp424->is_xtunion? _temp424->fields != 0: 0){
_temp424=({ struct Cyc_Absyn_Tuniondecl* _temp443=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp443[ 0]=* _temp424;
_temp443;});{ struct Cyc_List_List* _temp444=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp424->fields))->v; _temp424->fields= 0;{ struct
Cyc_List_List* _temp445= _temp444; for( 0; _temp445 != 0; _temp445=(( struct Cyc_List_List*)
_check_null( _temp445))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp412,
_temp424,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp445))->hd);}}}} _temp422->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp422->tuniondecls, x, _temp424);} goto _LL426; _LL426:;}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp448;
struct Cyc_Interface_I* _temp449; struct Cyc_Interface_Seen* _temp451; struct
Cyc_Dict_Dict* _temp453; int _temp455; struct Cyc_Interface_Ienv* _temp457;
struct Cyc_Interface_Ienv* _temp459; struct _tuple11* _temp446= env; _temp448=*
_temp446; _LL460: _temp459= _temp448.f1; goto _LL458; _LL458: _temp457= _temp448.f2;
goto _LL456; _LL456: _temp455= _temp448.f3; goto _LL454; _LL454: _temp453=
_temp448.f4; goto _LL452; _LL452: _temp451= _temp448.f5; goto _LL450; _LL450:
_temp449= _temp448.f6; goto _LL447; _LL447: if( Cyc_Interface_check_public_typedefdecl(
_temp453, _temp451, d)){ _temp457->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp457->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_arr* x, struct _tuple7* v){ struct _tuple11
_temp463; struct Cyc_Interface_I* _temp464; struct Cyc_Interface_Seen* _temp466;
struct Cyc_Dict_Dict* _temp468; int _temp470; struct Cyc_Interface_Ienv*
_temp472; struct Cyc_Interface_Ienv* _temp474; struct _tuple11* _temp461= env;
_temp463=* _temp461; _LL475: _temp474= _temp463.f1; goto _LL473; _LL473:
_temp472= _temp463.f2; goto _LL471; _LL471: _temp470= _temp463.f3; goto _LL469;
_LL469: _temp468= _temp463.f4; goto _LL467; _LL467: _temp466= _temp463.f5; goto
_LL465; _LL465: _temp464= _temp463.f6; goto _LL462; _LL462: { void* _temp476=(*
v).f1; void* _temp477= _temp476; void* _temp483; _LL479: if(*(( int*) _temp477)
== Cyc_Tcenv_VarRes){ _LL484: _temp483=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp477)->f1; goto _LL480;} else{ goto _LL481;} _LL481: goto _LL482; _LL480:{
void* _temp485= _temp483; struct Cyc_Absyn_Fndecl* _temp493; struct Cyc_Absyn_Vardecl*
_temp495; _LL487: if(( unsigned int) _temp485 > 1u?*(( int*) _temp485) == Cyc_Absyn_Funname_b:
0){ _LL494: _temp493=(( struct Cyc_Absyn_Funname_b_struct*) _temp485)->f1; goto
_LL488;} else{ goto _LL489;} _LL489: if(( unsigned int) _temp485 > 1u?*(( int*)
_temp485) == Cyc_Absyn_Global_b: 0){ _LL496: _temp495=(( struct Cyc_Absyn_Global_b_struct*)
_temp485)->f1; goto _LL490;} else{ goto _LL491;} _LL491: goto _LL492; _LL488: {
struct Cyc_Absyn_Vardecl* _temp497=({ struct Cyc_Absyn_Vardecl* _temp498=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp498->sc=( void*)(( void*) _temp493->sc); _temp498->name= _temp493->name;
_temp498->tq=({ struct Cyc_Absyn_Tqual _temp499; _temp499.q_const= 0; _temp499.q_volatile=
0; _temp499.q_restrict= 0; _temp499;}); _temp498->type=( void*)(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp493->cached_typ))->v); _temp498->initializer=
0; _temp498->rgn= 0; _temp498->attributes= 0; _temp498;}); _temp470= 0; _temp495=
_temp497; goto _LL490;} _LL490: if( _temp495->initializer != 0){ _temp495=({
struct Cyc_Absyn_Vardecl* _temp500=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp500[ 0]=* _temp495; _temp500;});
_temp495->initializer= 0;}{ void* _temp501=( void*) _temp495->sc; _LL503: if(
_temp501 ==( void*) Cyc_Absyn_Static){ goto _LL504;} else{ goto _LL505;} _LL505:
if( _temp501 ==( void*) Cyc_Absyn_Abstract){ goto _LL506;} else{ goto _LL507;}
_LL507: if( _temp501 ==( void*) Cyc_Absyn_Public){ goto _LL508;} else{ goto
_LL509;} _LL509: if( _temp501 ==( void*) Cyc_Absyn_ExternC){ goto _LL510;} else{
goto _LL511;} _LL511: if( _temp501 ==( void*) Cyc_Absyn_Extern){ goto _LL512;}
else{ goto _LL502;} _LL504: if( _temp470? Cyc_Tcutil_is_function_type(( void*)
_temp495->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp495->name);} goto _LL502; _LL506:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp513=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp513[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp514; _temp514.tag= Cyc_Core_InvalidArg; _temp514.f1=
_tag_arr("extract_ordinarie", sizeof( unsigned char), 18u); _temp514;});
_temp513;})); _LL508: if( _temp470? Cyc_Tcutil_is_function_type(( void*)
_temp495->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp495->name);} if( Cyc_Interface_check_public_vardecl(
_temp468, _temp466, _temp495)){ _temp472->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data))
Cyc_Dict_insert)( _temp472->vardecls, x, _temp495);} goto _LL502; _LL510: goto
_LL512; _LL512: if( Cyc_Interface_check_public_vardecl( _temp468, _temp466,
_temp495)){ _temp474->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp474->vardecls, x, _temp495);} goto _LL502; _LL502:;} goto _LL486; _LL492:
goto _LL486; _LL486:;} goto _LL478; _LL482: goto _LL478; _LL478:;}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp517; struct Cyc_Interface_Seen* _temp518; struct Cyc_Dict_Dict* _temp520;
int _temp522; struct Cyc_Interface_I* _temp524; struct _tuple12* _temp515= env_f;
_temp517=* _temp515; _LL525: _temp524= _temp517.f1; goto _LL523; _LL523:
_temp522= _temp517.f2; goto _LL521; _LL521: _temp520= _temp517.f3; goto _LL519;
_LL519: _temp518= _temp517.f4; goto _LL516; _LL516: { struct _tuple11 _temp526=({
struct _tuple11 _temp527; _temp527.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp524->imports,
ns); _temp527.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp524->exports, ns); _temp527.f3=
_temp522; _temp527.f4= _temp520; _temp527.f5= _temp518; _temp527.f6= _temp524;
_temp527;});(( void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct
Cyc_Absyn_Structdecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_structdecl,& _temp526, ge->structdecls);(( void(*)( void(*
f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp526, ge->uniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp526, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp526, ge->enumdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp526, ge->typedefs);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct _tuple7*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp526, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple12 _temp528=({
struct _tuple12 _temp529; _temp529.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp529.f2= check_complete_defs; _temp529.f3=
ae; _temp529.f4= Cyc_Interface_new_seen(); _temp529;});(( void(*)( void(* f)(
struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp528, ae); return _temp528.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_arr*); struct
_tagged_arr f4; struct _tagged_arr* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_arr* x, void* y1){ struct _tuple13 _temp532;
struct _tagged_arr* _temp533; struct _tagged_arr _temp535; int(* _temp537)( void*,
void*, struct _tagged_arr*); struct Cyc_Dict_Dict* _temp539; int _temp541; int*
_temp543; struct _tuple13* _temp530= env; _temp532=* _temp530; _LL542: _temp541=
_temp532.f1; _temp543=&(* _temp530).f1; goto _LL540; _LL540: _temp539= _temp532.f2;
goto _LL538; _LL538: _temp537= _temp532.f3; goto _LL536; _LL536: _temp535=
_temp532.f4; goto _LL534; _LL534: _temp533= _temp532.f5; goto _LL531; _LL531: {
struct _handler_cons _temp544; _push_handler(& _temp544);{ int _temp546= 0; if(
setjmp( _temp544.handler)){ _temp546= 1;} if( ! _temp546){{ void* _temp547=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp539, x); if( ! _temp537( y1, _temp547, _temp533)){* _temp543= 0;}};
_pop_handler();} else{ void* _temp545=( void*) _exn_thrown; void* _temp549=
_temp545; _LL551: if( _temp549 == Cyc_Dict_Absent){ goto _LL552;} else{ goto
_LL553;} _LL553: goto _LL554; _LL552: Cyc_Interface_check_err( _temp533,( struct
_tagged_arr)({ struct _tagged_arr _temp555= _temp535; struct _tagged_arr
_temp556=* x; xprintf("%.*s %.*s is missing", _get_arr_size( _temp555, 1u),
_temp555.curr, _get_arr_size( _temp556, 1u), _temp556.curr);}));* _temp543= 0;
goto _LL550; _LL554:( void) _throw( _temp549); _LL550:;}}}} static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( void*,
void*, struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){
struct _tuple13 _temp557=({ struct _tuple13 _temp558; _temp558.f1= 1; _temp558.f2=
dic2; _temp558.f3= incl_f; _temp558.f4= t; _temp558.f5= msg; _temp558;});(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_arr*, void*), struct _tuple13* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_arr* x, void* y1)) Cyc_Interface_incl_dict_f,& _temp557, dic1);
return _temp557.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Structdecl*
_temp559= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); if( _temp559 == 0){
return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp559){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr _temp560= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_get_arr_size( _temp560, 1u), _temp560.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Uniondecl* _temp561= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); if( _temp561 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*)
d0 != _temp561){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp562= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_get_arr_size( _temp562, 1u), _temp562.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl* _temp563= Cyc_Tcdecl_merge_tuniondecl(
d0, d1, 0, msg); if( _temp563 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*)
d0 != _temp563){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp564= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_get_arr_size( _temp564, 1u), _temp564.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl* _temp565= Cyc_Tcdecl_merge_enumdecl(
d0, d1, 0, msg); if( _temp565 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*)
d0 != _temp565){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp566= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of enum %.*s discloses too much information",
_get_arr_size( _temp566, 1u), _temp566.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl* _temp567= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); if( _temp567 == 0){ return 0;} if(( struct Cyc_Absyn_Vardecl*)
d0 != _temp567){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp568= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of variable %.*s discloses too much information",
_get_arr_size( _temp568, 1u), _temp568.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl* _temp569= Cyc_Tcdecl_merge_typedefdecl(
d0, d1, 0, msg); if( _temp569 == 0){ return 0;} if(( struct Cyc_Absyn_Typedefdecl*)
d0 != _temp569){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp570= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_get_arr_size( _temp570, 1u), _temp570.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl* d0,
struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp571= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0, msg); if( _temp571 == 0){
return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp571){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr _temp572= Cyc_Absynpp_qvar2string((
d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_get_arr_size( _temp572, 1u), _temp572.curr);})); return 0;} return 1;} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_arr* msg){ int _temp573=((
int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->structdecls,
ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct", sizeof(
unsigned char), 7u), msg); int _temp574=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*, struct
Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
int _temp575=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl, _tag_arr("tunion",
sizeof( unsigned char), 7u), msg); int _temp576=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); int _temp577=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) != 0; int _temp578=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); int _temp579=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); return((((( _temp573? _temp574: 0)? _temp575: 0)?
_temp576: 0)? _temp577: 0)? _temp578: 0)? _temp579: 0;} struct _tuple14{ int f1;
struct Cyc_Dict_Dict* f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f(
struct _tuple14* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){
struct _tuple14 _temp582; struct _tagged_arr* _temp583; struct Cyc_Dict_Dict*
_temp585; int _temp587; int* _temp589; struct _tuple14* _temp580= env; _temp582=*
_temp580; _LL588: _temp587= _temp582.f1; _temp589=&(* _temp580).f1; goto _LL586;
_LL586: _temp585= _temp582.f2; goto _LL584; _LL584: _temp583= _temp582.f3; goto
_LL581; _LL581: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp590;
_push_handler(& _temp590);{ int _temp592= 0; if( setjmp( _temp590.handler)){
_temp592= 1;} if( ! _temp592){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp585, ns);; _pop_handler();}
else{ void* _temp591=( void*) _exn_thrown; void* _temp594= _temp591; _LL596: if(
_temp594 == Cyc_Dict_Absent){ goto _LL597;} else{ goto _LL598;} _LL598: goto
_LL599; _LL597: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL595; _LL599:( void)
_throw( _temp594); _LL595:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp583)){*
_temp589= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple14 _temp600=({
struct _tuple14 _temp601; _temp601.f1= 1; _temp601.f2= dic2; _temp601.f3= msg;
_temp601;});(( void(*)( void(* f)( struct _tuple14*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _temp600, dic1); return _temp600.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp602; _push_handler(& _temp602);{ int _temp604= 0; if(
setjmp( _temp602.handler)){ _temp604= 1;} if( ! _temp604){{ int _temp605= 1;
struct _tagged_arr* msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp606=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp606[ 0]=(
struct _tagged_arr)({ struct _tagged_arr _temp607=(*(( struct _tuple3*)
_check_null( info))).f1; struct _tagged_arr _temp608=(*(( struct _tuple3*)
_check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_get_arr_size( _temp607, 1u), _temp607.curr, _get_arr_size( _temp608, 1u),
_temp608.curr);}); _temp606;});} if( ! Cyc_Interface_incl_ns( i1->exports, i2->exports,
msg)){ _temp605= 0;} if( info != 0){ msg=({ struct _tagged_arr* _temp609=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp609[ 0]=(
struct _tagged_arr)({ struct _tagged_arr _temp610=(*(( struct _tuple3*)
_check_null( info))).f2; struct _tagged_arr _temp611=(*(( struct _tuple3*)
_check_null( info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_get_arr_size( _temp610, 1u), _temp610.curr, _get_arr_size( _temp611, 1u),
_temp611.curr);}); _temp609;});} if( ! Cyc_Interface_incl_ns( i2->imports, i1->imports,
msg)){ _temp605= 0;}{ int _temp612= _temp605; _npop_handler( 0u); return
_temp612;}}; _pop_handler();} else{ void* _temp603=( void*) _exn_thrown; void*
_temp614= _temp603; _LL616: if( _temp614 == Cyc_Tcdecl_Incompatible){ goto
_LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL617: return 0; _LL619:(
void) _throw( _temp614); _LL615:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct _tagged_arr
f6; struct _tagged_arr* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_arr* x, void** y2){ struct _tuple15 _temp622;
struct _tagged_arr* _temp623; struct _tagged_arr _temp625; void**(* _temp627)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct Cyc_Dict_Dict*
_temp629; struct Cyc_Dict_Dict* _temp631; struct Cyc_Dict_Dict* _temp633; struct
Cyc_Dict_Dict** _temp635; int _temp636; int* _temp638; struct _tuple15* _temp620=
env; _temp622=* _temp620; _LL637: _temp636= _temp622.f1; _temp638=&(* _temp620).f1;
goto _LL634; _LL634: _temp633= _temp622.f2; _temp635=&(* _temp620).f2; goto
_LL632; _LL632: _temp631= _temp622.f3; goto _LL630; _LL630: _temp629= _temp622.f4;
goto _LL628; _LL628: _temp627= _temp622.f5; goto _LL626; _LL626: _temp625=
_temp622.f6; goto _LL624; _LL624: _temp623= _temp622.f7; goto _LL621; _LL621: {
void** y;{ struct _handler_cons _temp639; _push_handler(& _temp639);{ int
_temp641= 0; if( setjmp( _temp639.handler)){ _temp641= 1;} if( ! _temp641){{
void** _temp642=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp631, x); void** _temp643= _temp627( _temp642, y2, 0,
_temp623); if( !(( unsigned int) _temp643)){* _temp638= 0; _npop_handler( 0u);
return;} y=( void**) _check_null( _temp643);}; _pop_handler();} else{ void*
_temp640=( void*) _exn_thrown; void* _temp645= _temp640; _LL647: if( _temp645 ==
Cyc_Dict_Absent){ goto _LL648;} else{ goto _LL649;} _LL649: goto _LL650; _LL648:
y= y2; goto _LL646; _LL650:( void) _throw( _temp645); _LL646:;}}}{ struct
_handler_cons _temp651; _push_handler(& _temp651);{ int _temp653= 0; if( setjmp(
_temp651.handler)){ _temp653= 1;} if( ! _temp653){{ void** _temp654=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp629, x);
void** _temp655= _temp627( _temp654, y, 0, _temp623); if( _temp655 !=( void**)
_temp654){ if(( unsigned int) _temp655){ Cyc_Interface_check_err( _temp623,(
struct _tagged_arr)({ struct _tagged_arr _temp656= _temp625; struct _tagged_arr
_temp657=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_get_arr_size( _temp656, 1u), _temp656.curr, _get_arr_size( _temp657, 1u),
_temp657.curr);}));}* _temp638= 0;}}; _pop_handler();} else{ void* _temp652=(
void*) _exn_thrown; void* _temp659= _temp652; _LL661: if( _temp659 == Cyc_Dict_Absent){
goto _LL662;} else{ goto _LL663;} _LL663: goto _LL664; _LL662: if(* _temp638){*
_temp635=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void** data)) Cyc_Dict_insert)(* _temp635, x, y);} goto _LL660;
_LL664:( void) _throw( _temp659); _LL660:;}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple15
_temp665=({ struct _tuple15 _temp667; _temp667.f1= 1; _temp667.f2= dic1;
_temp667.f3= dic1; _temp667.f4= excl; _temp667.f5= merge_f; _temp667.f6= t;
_temp667.f7= msg; _temp667;});(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_arr*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_arr* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp665, dic2); if( _temp665.f1){ return({ struct Cyc_Core_Opt* _temp666=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp666->v=(
void*) _temp665.f2; _temp666;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_arr* msg){ struct Cyc_Core_Opt*
_temp668=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp669=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt*
_temp670=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp671=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp672=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef", sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt*
_temp673=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp674=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp668)? 1: !((
unsigned int) _temp669))? 1: !(( unsigned int) _temp670))? 1: !(( unsigned int)
_temp671))? 1: !(( unsigned int) _temp672))? 1: !(( unsigned int) _temp673))? 1:
!(( unsigned int) _temp674)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp675=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp675->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp668))->v; _temp675->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp669))->v; _temp675->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp670))->v; _temp675->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp671))->v;
_temp675->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp672))->v; _temp675->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp673))->v; _temp675->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp674))->v; _temp675;});}}
struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple16 _temp678; struct _tagged_arr* _temp679; struct Cyc_Dict_Dict*
_temp681; struct Cyc_Dict_Dict* _temp683; struct Cyc_Dict_Dict* _temp685; struct
Cyc_Dict_Dict** _temp687; int _temp688; int* _temp690; struct _tuple16* _temp676=
env; _temp678=* _temp676; _LL689: _temp688= _temp678.f1; _temp690=&(* _temp676).f1;
goto _LL686; _LL686: _temp685= _temp678.f2; _temp687=&(* _temp676).f2; goto
_LL684; _LL684: _temp683= _temp678.f3; goto _LL682; _LL682: _temp681= _temp678.f4;
goto _LL680; _LL680: _temp679= _temp678.f5; goto _LL677; _LL677: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp691;
_push_handler(& _temp691);{ int _temp693= 0; if( setjmp( _temp691.handler)){
_temp693= 1;} if( ! _temp693){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp681, ns);;
_pop_handler();} else{ void* _temp692=( void*) _exn_thrown; void* _temp695=
_temp692; _LL697: if( _temp695 == Cyc_Dict_Absent){ goto _LL698;} else{ goto
_LL699;} _LL699: goto _LL700; _LL698: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL696; _LL700:( void) _throw( _temp695); _LL696:;}}}{ struct _handler_cons
_temp701; _push_handler(& _temp701);{ int _temp703= 0; if( setjmp( _temp701.handler)){
_temp703= 1;} if( ! _temp703){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp683, ns);{ struct Cyc_Interface_Ienv*
_temp704= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp679); if( _temp704 == 0){* _temp690= 0; _npop_handler( 0u); return;}
ie1=( struct Cyc_Interface_Ienv*) _check_null( _temp704);}; _pop_handler();}
else{ void* _temp702=( void*) _exn_thrown; void* _temp706= _temp702; _LL708: if(
_temp706 == Cyc_Dict_Absent){ goto _LL709;} else{ goto _LL710;} _LL710: goto
_LL711; _LL709: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL707; _LL711:( void)
_throw( _temp706); _LL707:;}}}{ struct Cyc_Interface_Ienv* _temp712= Cyc_Interface_compat_merge_ienv(
ie1, ie2, iexcl, _temp679); if( _temp712 == 0){* _temp690= 0;} else{ if(*
_temp690){* _temp687=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp687, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp712));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_arr* msg){
struct _tuple16 _temp713=({ struct _tuple16 _temp715; _temp715.f1= 1; _temp715.f2=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp715.f3= dic1; _temp715.f4= excl;
_temp715.f5= msg; _temp715;});(( void(*)( void(* f)( struct _tuple16*, struct
Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,& _temp713, dic2); if(
_temp713.f1){ return({ struct Cyc_Core_Opt* _temp714=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp714->v=( void*) _temp713.f2;
_temp714;});} else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct _tagged_arr f4; struct _tagged_arr* f5; } ;
void Cyc_Interface_disj_merge_dict_f( struct _tuple17* env, struct _tagged_arr*
x, void* y){ struct _tuple17 _temp718; struct _tagged_arr* _temp719; struct
_tagged_arr _temp721; struct Cyc_Dict_Dict* _temp723; struct Cyc_Dict_Dict*
_temp725; struct Cyc_Dict_Dict** _temp727; int _temp728; int* _temp730; struct
_tuple17* _temp716= env; _temp718=* _temp716; _LL729: _temp728= _temp718.f1;
_temp730=&(* _temp716).f1; goto _LL726; _LL726: _temp725= _temp718.f2; _temp727=&(*
_temp716).f2; goto _LL724; _LL724: _temp723= _temp718.f3; goto _LL722; _LL722:
_temp721= _temp718.f4; goto _LL720; _LL720: _temp719= _temp718.f5; goto _LL717;
_LL717: if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
_temp723, x)){ Cyc_Interface_check_err( _temp719,( struct _tagged_arr)({ struct
_tagged_arr _temp731= _temp721; struct _tagged_arr _temp732=* x; xprintf("%.*s %.*s is exported more than once",
_get_arr_size( _temp731, 1u), _temp731.curr, _get_arr_size( _temp732, 1u),
_temp732.curr);}));* _temp730= 0;} else{ if(* _temp730){* _temp727=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(*
_temp727, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct
_tagged_arr* msg){ struct _tuple17 _temp733=({ struct _tuple17 _temp735;
_temp735.f1= 1; _temp735.f2= dic1; _temp735.f3= dic1; _temp735.f4= t; _temp735.f5=
msg; _temp735;});(( void(*)( void(* f)( struct _tuple17*, struct _tagged_arr*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple17* env, struct _tagged_arr* x, void* y)) Cyc_Interface_disj_merge_dict_f,&
_temp733, dic2); if( _temp733.f1){ return({ struct Cyc_Core_Opt* _temp734=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp734->v=(
void*) _temp733.f2; _temp734;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_arr* msg){ struct Cyc_Core_Opt* _temp736=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp737=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->uniondecls, ie2->uniondecls, _tag_arr("union", sizeof( unsigned char), 6u),
msg); struct Cyc_Core_Opt* _temp738=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), msg); struct Cyc_Core_Opt* _temp739=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp740=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg); struct Cyc_Core_Opt* _temp741=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt* _temp742=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof( unsigned char), 13u),
msg); if(((((( !(( unsigned int) _temp736)? 1: !(( unsigned int) _temp737))? 1:
!(( unsigned int) _temp738))? 1: !(( unsigned int) _temp739))? 1: !((
unsigned int) _temp740))? 1: !(( unsigned int) _temp741))? 1: !(( unsigned int)
_temp742)){ return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp743=(
struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp743->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp736))->v; _temp743->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp737))->v; _temp743->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp738))->v; _temp743->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp739))->v;
_temp743->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp740))->v; _temp743->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp741))->v; _temp743->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp742))->v; _temp743;});}}
struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct _tuple18*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple18
_temp746; struct _tagged_arr* _temp747; struct Cyc_Dict_Dict* _temp749; struct
Cyc_Dict_Dict* _temp751; struct Cyc_Dict_Dict** _temp753; int _temp754; int*
_temp756; struct _tuple18* _temp744= env; _temp746=* _temp744; _LL755: _temp754=
_temp746.f1; _temp756=&(* _temp744).f1; goto _LL752; _LL752: _temp751= _temp746.f2;
_temp753=&(* _temp744).f2; goto _LL750; _LL750: _temp749= _temp746.f3; goto
_LL748; _LL748: _temp747= _temp746.f4; goto _LL745; _LL745: { struct Cyc_Interface_Ienv*
ie1;{ struct _handler_cons _temp757; _push_handler(& _temp757);{ int _temp759= 0;
if( setjmp( _temp757.handler)){ _temp759= 1;} if( ! _temp759){ ie1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp749,
ns);; _pop_handler();} else{ void* _temp758=( void*) _exn_thrown; void* _temp761=
_temp758; _LL763: if( _temp761 == Cyc_Dict_Absent){ goto _LL764;} else{ goto
_LL765;} _LL765: goto _LL766; _LL764: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL762; _LL766:( void) _throw( _temp761); _LL762:;}}}{ struct Cyc_Interface_Ienv*
_temp767= Cyc_Interface_disj_merge_ienv( ie1, ie2, _temp747); if( _temp767 == 0){*
_temp756= 0;} else{ if(* _temp756){* _temp753=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* _temp753, ns,( struct Cyc_Interface_Ienv*) _check_null(
_temp767));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple18
_temp768=({ struct _tuple18 _temp770; _temp770.f1= 1; _temp770.f2= dic1;
_temp770.f3= dic1; _temp770.f4= msg; _temp770;});(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp768, dic2); if( _temp768.f1){ return({ struct Cyc_Core_Opt* _temp769=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp769->v=(
void*) _temp768.f2; _temp769;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp771; _push_handler(& _temp771);{ int _temp773= 0; if(
setjmp( _temp771.handler)){ _temp773= 1;} if( ! _temp773){{ struct _tagged_arr*
msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp774=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp774[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp775=(*(( struct _tuple3*) _check_null( info))).f1;
struct _tagged_arr _temp776=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging exports of %.*s and %.*s,", _get_arr_size( _temp775, 1u),
_temp775.curr, _get_arr_size( _temp776, 1u), _temp776.curr);}); _temp774;});}{
struct Cyc_Core_Opt* _temp777= Cyc_Interface_disj_merge_ns( i1->exports, i2->exports,
msg); if( _temp777 == 0){ struct Cyc_Interface_I* _temp778= 0; _npop_handler( 0u);
return _temp778;} if( info != 0){ msg=({ struct _tagged_arr* _temp779=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp779[ 0]=( struct
_tagged_arr)({ struct _tagged_arr _temp780=(*(( struct _tuple3*) _check_null(
info))).f1; struct _tagged_arr _temp781=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging imports of %.*s and %.*s,", _get_arr_size( _temp780, 1u),
_temp780.curr, _get_arr_size( _temp781, 1u), _temp781.curr);}); _temp779;});}{
struct Cyc_Core_Opt* _temp782= Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp777))->v, msg);
if( _temp782 == 0){ struct Cyc_Interface_I* _temp783= 0; _npop_handler( 0u);
return _temp783;}{ struct Cyc_Interface_I* _temp785=({ struct Cyc_Interface_I*
_temp784=( struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I));
_temp784->imports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp782))->v; _temp784->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp777))->v; _temp784;}); _npop_handler( 0u); return _temp785;}}}};
_pop_handler();} else{ void* _temp772=( void*) _exn_thrown; void* _temp787=
_temp772; _LL789: if( _temp787 == Cyc_Tcdecl_Incompatible){ goto _LL790;} else{
goto _LL791;} _LL791: goto _LL792; _LL790: return 0; _LL792:( void) _throw(
_temp787); _LL788:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp793= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp797=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp797->f1=*(( struct _tagged_arr*)
_check_null( curr_info)); _temp797->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp797;}): 0); if( _temp793 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp793); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp794=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp794[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp795=*(( struct _tagged_arr*) _check_null( curr_info));
struct _tagged_arr _temp796=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); xprintf("%.*s+%.*s", _get_arr_size( _temp795, 1u),
_temp795.curr, _get_arr_size( _temp796, 1u), _temp796.curr);}); _temp794;}): 0;}
if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null( linfo))->tl;}}
return( struct Cyc_Interface_I*) curr_i;}} struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(* get)( void*), struct Cyc_List_List* la, struct Cyc_List_List*
linfo){ if( la == 0){ return( struct Cyc_Interface_I*) Cyc_Interface_empty();}{
struct Cyc_Interface_I* curr_i= get(( void*)(( struct Cyc_List_List*)
_check_null( la))->hd); struct _tagged_arr* curr_info= linfo != 0?( struct
_tagged_arr*)(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd):
0; la=(( struct Cyc_List_List*) _check_null( la))->tl; if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;} for( 0; la != 0; la=(( struct
Cyc_List_List*) _check_null( la))->tl){ struct Cyc_Interface_I* _temp798= Cyc_Interface_merge(
curr_i, get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd),( curr_info
!= 0? linfo != 0: 0)?({ struct _tuple3* _temp802=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp802->f1=*(( struct _tagged_arr*) _check_null(
curr_info)); _temp802->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp802;}): 0); if( _temp798 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp798); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp799=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp799[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp800=*(( struct _tagged_arr*) _check_null( curr_info));
struct _tagged_arr _temp801=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); xprintf("%.*s+%.*s", _get_arr_size( _temp800, 1u),
_temp800.curr, _get_arr_size( _temp801, 1u), _temp801.curr);}); _temp799;}): 0;}
if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null( linfo))->tl;}}
return( struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace(
struct Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;}
return({ struct Cyc_List_List* _temp803=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp803->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp804=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp804[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp805; _temp805.tag= Cyc_Absyn_Namespace_d;
_temp805.f1=( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp805.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp805;}); _temp804;}), 0); _temp803->tl= 0; _temp803;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp806=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp806->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp807=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp807[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp808; _temp808.tag= Cyc_Absyn_Struct_d;
_temp808.f1= d; _temp808;}); _temp807;}), 0); _temp806->tl= tds; _temp806;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp809=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp809[ 0]=* d; _temp809;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp810->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp811=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp811[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp812; _temp812.tag= Cyc_Absyn_Struct_d; _temp812.f1=
d; _temp812;}); _temp811;}), 0); _temp810->tl= tds; _temp810;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp813=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp813->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp814=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp814[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp815; _temp815.tag= Cyc_Absyn_Union_d;
_temp815.f1= d; _temp815;}); _temp814;}), 0); _temp813->tl= tds; _temp813;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp816=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp816[ 0]=* d; _temp816;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp817=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp817->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp818=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp818[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp819; _temp819.tag= Cyc_Absyn_Union_d; _temp819.f1= d; _temp819;}); _temp818;}),
0); _temp817->tl= tds; _temp817;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp820=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp820->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp821=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp821[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp822; _temp822.tag= Cyc_Absyn_Tunion_d; _temp822.f1=
d; _temp822;}); _temp821;}), 0); _temp820->tl= tds; _temp820;});} static
unsigned char _temp823[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp823, _temp823, _temp823 + 2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp824=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp824->f1= Cyc_Absyn_empty_tqual();
_temp824->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp825=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp825[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp826; _temp826.tag= Cyc_Absyn_VarType; _temp826.f1=({ struct Cyc_Absyn_Tvar*
_temp827=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp827->name= Cyc_Interface_us_p; _temp827->identity= 0; _temp827->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_BoxKind);
_temp827;}); _temp826;}); _temp825;}); _temp824;});} static struct Cyc_Absyn_Tunionfield*
Cyc_Interface_rewrite_tunionfield( struct Cyc_Absyn_Tunionfield* f){ f=({ struct
Cyc_Absyn_Tunionfield* _temp828=( struct Cyc_Absyn_Tunionfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Tunionfield)); _temp828[ 0]=* f; _temp828;}); f->typs=((
struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct _tuple10*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type, f->typs); return f;}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp829=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp829[ 0]=* d; _temp829;}); if( d->fields
!= 0){ d->fields=({ struct Cyc_Core_Opt* _temp830=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp830->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp830;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp831=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp831->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp832=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp832[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp833; _temp833.tag= Cyc_Absyn_Tunion_d; _temp833.f1=
d; _temp833;}); _temp832;}), 0); _temp831->tl= tds; _temp831;});} static struct
Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp834=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp834->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp835=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp835[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp836; _temp836.tag= Cyc_Absyn_Enum_d;
_temp836.f1= d; _temp836;}); _temp835;}), 0); _temp834->tl= tds; _temp834;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp837=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp837[ 0]=* d; _temp837;}); d->fields= 0;
if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp838->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp839=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp839[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp840; _temp840.tag= Cyc_Absyn_Enum_d; _temp840.f1= d; _temp840;}); _temp839;}),
0); _temp838->tl= tds; _temp838;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp841=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp841->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp842=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp842[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp843; _temp843.tag= Cyc_Absyn_Typedef_d; _temp843.f1=
d; _temp843;}); _temp842;}), 0); _temp841->tl= tds; _temp841;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp844=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp844->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp845=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp845[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp846; _temp846.tag= Cyc_Absyn_Var_d;
_temp846.f1= d; _temp846;}); _temp845;}), 0); _temp844->tl= tds; _temp844;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp849; struct Cyc_Absyn_Tunionfield*
_temp850; struct Cyc_Absyn_Tuniondecl* _temp852; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp847= d; _temp849=* _temp847; _LL853: _temp852= _temp849.base; goto _LL851;
_LL851: _temp850= _temp849.field; goto _LL848; _LL848: _temp852=({ struct Cyc_Absyn_Tuniondecl*
_temp854=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp854[ 0]=* _temp852; _temp854;}); _temp852->fields=({ struct Cyc_Core_Opt*
_temp855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp855->v=( void*)({ struct Cyc_List_List* _temp856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp856->hd=( void*) _temp850;
_temp856->tl= 0; _temp856;}); _temp855;});( void*)( _temp852->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp857=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp857->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp858=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp858[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp859; _temp859.tag= Cyc_Absyn_Tunion_d; _temp859.f1=
_temp852; _temp859;}); _temp858;}), 0); _temp857->tl= tds; _temp857;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp862; struct Cyc_Absyn_Tunionfield*
_temp863; struct Cyc_Absyn_Tuniondecl* _temp865; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp860= d; _temp862=* _temp860; _LL866: _temp865= _temp862.base; goto _LL864;
_LL864: _temp863= _temp862.field; goto _LL861; _LL861: _temp865=({ struct Cyc_Absyn_Tuniondecl*
_temp867=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp867[ 0]=* _temp865; _temp867;}); _temp863= Cyc_Interface_rewrite_tunionfield(
_temp863);( void*)( _temp863->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp865->fields=({
struct Cyc_Core_Opt* _temp868=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp868->v=( void*)({ struct Cyc_List_List* _temp869=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp869->hd=( void*)
_temp863; _temp869->tl= 0; _temp869;}); _temp868;});( void*)( _temp865->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp870=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp870->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp871=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp871[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp872; _temp872.tag= Cyc_Absyn_Tunion_d;
_temp872.f1= _temp865; _temp872;}); _temp871;}), 0); _temp870->tl= tds; _temp870;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp873= 0; _temp873=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, _temp873); _temp873=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp873); _temp873=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp873); _temp873=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp873); if( _temp873 != 0){ _temp873=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp873); _temp873= Cyc_Interface_add_namespace(
_temp873, ns); Cyc_Absynpp_decllist2file( _temp873, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp874= 0; _temp874=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp874); if( _temp874 != 0){ _temp874=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp874); _temp874= Cyc_Interface_add_namespace(
_temp874, ns); Cyc_Absynpp_decllist2file( _temp874, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp875= 0; _temp875=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp875); if(
_temp875 != 0){ _temp875=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp875); _temp875= Cyc_Interface_add_namespace( _temp875,
ns); Cyc_Absynpp_decllist2file( _temp875, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp876= 0; _temp876=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp876);
_temp876=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp876); _temp876=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp876); _temp876=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp876); _temp876=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp876); _temp876=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp876); if( _temp876 != 0){ _temp876=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp876); _temp876= Cyc_Interface_add_namespace(
_temp876, ns); Cyc_Absynpp_decllist2file( _temp876, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Absynpp_set_params(&
Cyc_Absynpp_cyci_params_r); fprintf( _sfile_to_file( f),"/****** needed (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports); fprintf( _sfile_to_file( f),"\n/****** provided (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports); fprintf( _sfile_to_file( f),"/****** needed (headers of xtunionfielddecls) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->imports); fprintf( _sfile_to_file( f),"\n/****** provided (headers of xtunionfielddecls) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->exports); fprintf( _sfile_to_file( f),"\n/****** provided (typedefs) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports); fprintf( _sfile_to_file( f),"\n/****** needed (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->imports); fprintf( _sfile_to_file( f),"\n/****** provided (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp877= Cyc_Parse_parse_file( f);
struct Cyc_Tcenv_Tenv* _temp878= Cyc_Tcenv_tc_init(); Cyc_Tc_tc( _temp878, 0,
_temp877); return Cyc_Interface_gen_extract( _temp878->ae, 0);}} void Cyc_Interface_save(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i,
f);} struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE* f){
return Cyc_Interface_parse( f);}