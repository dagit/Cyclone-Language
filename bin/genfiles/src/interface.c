 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern void*
Cyc_Core_identity( void*); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
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
_temp3= Cyc_Interface_empty(); goto _LL4; _LL4: { struct Cyc_Interface_Ienv*
_temp5= Cyc_Interface_new_ienv(); goto _LL6; _LL6: { struct Cyc_Absyn_Tuniondecl*
_temp8=({ struct Cyc_Absyn_Tuniondecl* _temp7=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp7[ 0]=* Cyc_Absyn_exn_tud;
_temp7;}); goto _LL9; _LL9:( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));
_temp5->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp5->tuniondecls,(*
Cyc_Absyn_exn_name).f2, _temp8);{ struct Cyc_Absyn_Tunionfield* _temp11=({
struct Cyc_Absyn_Tunionfield* _temp10=( struct Cyc_Absyn_Tunionfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Tunionfield)); _temp10[ 0]=* Cyc_Absyn_null_pointer_exn_tuf;
_temp10;}); goto _LL12; _LL12:( void*)( _temp11->sc=( void*)(( void*) Cyc_Absyn_Public));{
struct Cyc_Tcdecl_Xtunionfielddecl* _temp14=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp13=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp13->base= _temp8; _temp13->field= _temp11; _temp13;}); goto _LL15; _LL15:
_temp5->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key, struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)(
_temp5->xtunionfielddecls,(* Cyc_Absyn_null_pointer_exn_name).f2, _temp14);{
struct Cyc_Absyn_Tunionfield* _temp17=({ struct Cyc_Absyn_Tunionfield* _temp16=(
struct Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp16[ 0]=* Cyc_Absyn_match_exn_tuf; _temp16;}); goto _LL18; _LL18:( void*)(
_temp17->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp20=({ struct Cyc_Tcdecl_Xtunionfielddecl* _temp19=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp19->base= _temp8;
_temp19->field= _temp17; _temp19;}); goto _LL21; _LL21: _temp5->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp5->xtunionfielddecls,(*
Cyc_Absyn_match_exn_name).f2, _temp20); _temp3->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp5);{ struct Cyc_Interface_Ienv*
_temp22= Cyc_Interface_new_ienv(); goto _LL23; _LL23: { struct _tuple0* qmain=({
struct _tuple0* _temp30=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp30->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp31=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp31[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp32; _temp32.tag= Cyc_Absyn_Abs_n; _temp32.f1= 0; _temp32;}); _temp31;});
_temp30->f2= _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"main", sizeof( unsigned char), 5u); _temp30;}); struct Cyc_Absyn_Vardecl*
_temp28= Cyc_Absyn_new_vardecl( qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({
struct Cyc_List_List* _temp24=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp24->hd=( void*)({ struct _tuple1* _temp27=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp27->f1= 0; _temp27->f2= Cyc_Absyn_empty_tqual();
_temp27->f3= Cyc_Absyn_uint_t; _temp27;}); _temp24->tl=({ struct Cyc_List_List*
_temp25=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp25->hd=( void*)({ struct _tuple1* _temp26=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp26->f1= 0; _temp26->f2= Cyc_Absyn_empty_tqual();
_temp26->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp26;}); _temp25->tl= 0; _temp25;}); _temp24;}), 0,
0, 0, 0), 0); goto _LL29; _LL29:( void*)( _temp28->sc=( void*)(( void*) Cyc_Absyn_Extern));
_temp22->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp22->vardecls,(*
qmain).f2, _temp28); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp22); return _temp3;}}}}}}}}} static void Cyc_Interface_err(
struct _tagged_arr msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_arr sc, struct _tagged_arr t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_arr)({ struct _tagged_arr _temp33= sc; struct _tagged_arr _temp34=
t; struct _tagged_arr _temp35= Cyc_Absynpp_qvar2string( n); xprintf("fields of %.*s %.*s %.*s have never been defined",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr, _temp35.last_plus_one - _temp35.curr, _temp35.curr);}));}
static void Cyc_Interface_body_err( struct _tagged_arr sc, struct _tuple0* n){
Cyc_Interface_err(( struct _tagged_arr)({ struct _tagged_arr _temp36= sc; struct
_tagged_arr _temp37= Cyc_Absynpp_qvar2string( n); xprintf("the body of %.*s function %.*s has never been defined",
_temp36.last_plus_one - _temp36.curr, _temp36.curr, _temp37.last_plus_one -
_temp37.curr, _temp37.curr);}));} static void Cyc_Interface_static_err( struct
_tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct _tuple0*
name2){ if( obj1.curr !=(( struct _tagged_arr) _tag_arr( 0u, sizeof(
unsigned char), 0u)).curr){ Cyc_Interface_err(( struct _tagged_arr)({ struct
_tagged_arr _temp38= obj1; struct _tagged_arr _temp39= Cyc_Absynpp_qvar2string(
name1); struct _tagged_arr _temp40= obj2; struct _tagged_arr _temp41= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s", _temp38.last_plus_one
- _temp38.curr, _temp38.curr, _temp39.last_plus_one - _temp39.curr, _temp39.curr,
_temp40.last_plus_one - _temp40.curr, _temp40.curr, _temp41.last_plus_one -
_temp41.curr, _temp41.curr);}));}} static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct
_tuple0* name2){ if( obj1.curr !=(( struct _tagged_arr) _tag_arr( 0u, sizeof(
unsigned char), 0u)).curr){ Cyc_Interface_err(( struct _tagged_arr)({ struct
_tagged_arr _temp42= obj1; struct _tagged_arr _temp43= Cyc_Absynpp_qvar2string(
name1); struct _tagged_arr _temp44= obj2; struct _tagged_arr _temp45= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp42.last_plus_one - _temp42.curr, _temp42.curr, _temp43.last_plus_one -
_temp43.curr, _temp43.curr, _temp44.last_plus_one - _temp44.curr, _temp44.curr,
_temp45.last_plus_one - _temp45.curr, _temp45.curr);}));}} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; static struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({
struct Cyc_Interface_Seen* _temp46=( struct Cyc_Interface_Seen*) GC_malloc(
sizeof( struct Cyc_Interface_Seen)); _temp46->structs=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp46->unions=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp46->tunions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp46;});} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp47= 1; goto _LL48; _LL48: for( 0; l != 0; l=(( struct Cyc_List_List*)
_check_null( l))->tl){ if( ! Cyc_Interface_check_public_type( ae, seen, obj,
name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){ _temp47= 0;}}
return _temp47;} static int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp49; _push_handler(& _temp49);{ int _temp51= 0; if( setjmp(
_temp49.handler)){ _temp51= 1;} if( ! _temp51){{ int _temp52=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp52;}; _pop_handler();} else{ void* _temp50=( void*) _exn_thrown;
void* _temp54= _temp50; _LL56: if( _temp54 == Cyc_Dict_Absent){ goto _LL57;}
else{ goto _LL58;} _LL58: goto _LL59; _LL57: { int _temp60= 1; goto _LL61; _LL61:
seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp60); if( d->fields != 0){
struct Cyc_List_List* _temp62=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL63; _LL63: for( 0; _temp62 != 0; _temp62=((
struct Cyc_List_List*) _check_null( _temp62))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp62))->hd)->type)){ _temp60= 0;}}} seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp60); return _temp60;} _LL59:( void) _throw(
_temp54); _LL55:;}}} static int Cyc_Interface_check_public_uniondecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl* d){ struct
_handler_cons _temp64; _push_handler(& _temp64);{ int _temp66= 0; if( setjmp(
_temp64.handler)){ _temp66= 1;} if( ! _temp66){{ int _temp67=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp67;}; _pop_handler();} else{ void* _temp65=( void*) _exn_thrown;
void* _temp69= _temp65; _LL71: if( _temp69 == Cyc_Dict_Absent){ goto _LL72;}
else{ goto _LL73;} _LL73: goto _LL74; _LL72: { int _temp75= 1; goto _LL76; _LL76:
seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp75); if( d->fields != 0){
struct Cyc_List_List* _temp77=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL78; _LL78: for( 0; _temp77 != 0; _temp77=((
struct Cyc_List_List*) _check_null( _temp77))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp77))->hd)->type)){ _temp75= 0;}}} seen->unions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->unions,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp75); return _temp75;} _LL74:( void) _throw(
_temp69); _LL70:;}}} struct _tuple10{ struct Cyc_Absyn_Tqual f1; void* f2; } ;
static void* Cyc_Interface_get_type1( struct _tuple10* x){ return(* x).f2;}
static int Cyc_Interface_check_public_tuniondecl( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl* d){ struct
_handler_cons _temp79; _push_handler(& _temp79);{ int _temp81= 0; if( setjmp(
_temp79.handler)){ _temp81= 1;} if( ! _temp81){{ int _temp82=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions, d->name);
_npop_handler( 0u); return _temp82;}; _pop_handler();} else{ void* _temp80=(
void*) _exn_thrown; void* _temp84= _temp80; _LL86: if( _temp84 == Cyc_Dict_Absent){
goto _LL87;} else{ goto _LL88;} _LL88: goto _LL89; _LL87: { int _temp90= 1; goto
_LL91; _LL91: seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp90); if( d->fields != 0){ struct Cyc_List_List* _temp92=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; goto _LL93; _LL93: for( 0;
_temp92 != 0; _temp92=(( struct Cyc_List_List*) _check_null( _temp92))->tl){ if(
!(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct
_tagged_arr obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp92))->hd)->typs)){ _temp90= 0;}}} seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, _temp90); return _temp90;} _LL89:(
void) _throw( _temp84); _LL85:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl*
d){ return 1;} static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl* d){ return
Cyc_Interface_check_public_type( ae, seen,( struct _tagged_arr) _tag_arr( 0u,
sizeof( unsigned char), 0u), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen, _tag_arr("variable",
sizeof( unsigned char), 9u), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns(
void* ns){ void* _temp94= ns; struct Cyc_List_List* _temp100; _LL96: if((
unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Abs_n: 0){ _LL101:
_temp100=(( struct Cyc_Absyn_Abs_n_struct*) _temp94)->f1; goto _LL97;} else{
goto _LL98;} _LL98: goto _LL99; _LL97: return _temp100; _LL99:( int) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp102=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp102[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp103; _temp103.tag= Cyc_Core_InvalidArg; _temp103.f1=
_tag_arr("get_abs_ns", sizeof( unsigned char), 11u); _temp103;}); _temp102;}));
_LL95:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name,
void* t){ void* _temp104= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp128; struct Cyc_Absyn_Conref* _temp130; struct Cyc_Absyn_Tqual _temp132;
struct Cyc_Absyn_Conref* _temp134; void* _temp136; void* _temp138; struct Cyc_Absyn_Exp*
_temp140; struct Cyc_Absyn_Tqual _temp142; void* _temp144; struct Cyc_Core_Opt*
_temp146; struct Cyc_Core_Opt _temp148; void* _temp149; struct Cyc_List_List*
_temp151; struct _tuple0* _temp153; struct Cyc_Absyn_FnInfo _temp155; struct Cyc_List_List*
_temp157; struct Cyc_List_List* _temp159; struct Cyc_Absyn_VarargInfo* _temp161;
int _temp163; struct Cyc_List_List* _temp165; void* _temp167; struct Cyc_Core_Opt*
_temp169; struct Cyc_List_List* _temp171; struct Cyc_List_List* _temp173; struct
Cyc_Absyn_Structdecl** _temp175; struct Cyc_Absyn_Structdecl* _temp177; struct
Cyc_List_List* _temp178; struct _tuple0* _temp180; struct Cyc_Absyn_Uniondecl**
_temp182; struct Cyc_Absyn_Uniondecl* _temp184; struct Cyc_List_List* _temp185;
struct _tuple0* _temp187; struct Cyc_Absyn_Enumdecl* _temp189; struct _tuple0*
_temp191; struct Cyc_Absyn_TunionInfo _temp193; void* _temp195; struct Cyc_List_List*
_temp197; void* _temp199; struct Cyc_Absyn_Tuniondecl* _temp201; struct Cyc_Absyn_TunionFieldInfo
_temp203; struct Cyc_List_List* _temp205; void* _temp207; struct Cyc_Absyn_Tunionfield*
_temp209; struct Cyc_Absyn_Tuniondecl* _temp211; _LL106: if(( unsigned int)
_temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_PointerType: 0){ _LL129: _temp128=((
struct Cyc_Absyn_PointerType_struct*) _temp104)->f1; _LL139: _temp138=( void*)
_temp128.elt_typ; goto _LL137; _LL137: _temp136=( void*) _temp128.rgn_typ; goto
_LL135; _LL135: _temp134= _temp128.nullable; goto _LL133; _LL133: _temp132=
_temp128.tq; goto _LL131; _LL131: _temp130= _temp128.bounds; goto _LL107;} else{
goto _LL108;} _LL108: if(( unsigned int) _temp104 > 4u?*(( int*) _temp104) ==
Cyc_Absyn_ArrayType: 0){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp104)->f1; goto _LL143; _LL143: _temp142=(( struct Cyc_Absyn_ArrayType_struct*)
_temp104)->f2; goto _LL141; _LL141: _temp140=(( struct Cyc_Absyn_ArrayType_struct*)
_temp104)->f3; goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int)
_temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_TypedefType: 0){ _LL154: _temp153=((
struct Cyc_Absyn_TypedefType_struct*) _temp104)->f1; goto _LL152; _LL152:
_temp151=(( struct Cyc_Absyn_TypedefType_struct*) _temp104)->f2; goto _LL147;
_LL147: _temp146=(( struct Cyc_Absyn_TypedefType_struct*) _temp104)->f3; if(
_temp146 == 0){ goto _LL112;} else{ _temp148=* _temp146; _LL150: _temp149=( void*)
_temp148.v; goto _LL111;}} else{ goto _LL112;} _LL112: if(( unsigned int)
_temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_FnType: 0){ _LL156: _temp155=((
struct Cyc_Absyn_FnType_struct*) _temp104)->f1; _LL172: _temp171= _temp155.tvars;
goto _LL170; _LL170: _temp169= _temp155.effect; goto _LL168; _LL168: _temp167=(
void*) _temp155.ret_typ; goto _LL166; _LL166: _temp165= _temp155.args; goto
_LL164; _LL164: _temp163= _temp155.c_varargs; goto _LL162; _LL162: _temp161=
_temp155.cyc_varargs; goto _LL160; _LL160: _temp159= _temp155.rgn_po; goto
_LL158; _LL158: _temp157= _temp155.attributes; goto _LL113;} else{ goto _LL114;}
_LL114: if(( unsigned int) _temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_TupleType:
0){ _LL174: _temp173=(( struct Cyc_Absyn_TupleType_struct*) _temp104)->f1; goto
_LL115;} else{ goto _LL116;} _LL116: if(( unsigned int) _temp104 > 4u?*(( int*)
_temp104) == Cyc_Absyn_StructType: 0){ _LL181: _temp180=(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f1; goto _LL179; _LL179: _temp178=(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f2; goto _LL176; _LL176: _temp175=(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f3; if( _temp175 == 0){ goto _LL118;} else{ _temp177=* _temp175; goto
_LL117;}} else{ goto _LL118;} _LL118: if(( unsigned int) _temp104 > 4u?*(( int*)
_temp104) == Cyc_Absyn_UnionType: 0){ _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*)
_temp104)->f1; goto _LL186; _LL186: _temp185=(( struct Cyc_Absyn_UnionType_struct*)
_temp104)->f2; goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_UnionType_struct*)
_temp104)->f3; if( _temp182 == 0){ goto _LL120;} else{ _temp184=* _temp182; goto
_LL119;}} else{ goto _LL120;} _LL120: if(( unsigned int) _temp104 > 4u?*(( int*)
_temp104) == Cyc_Absyn_EnumType: 0){ _LL192: _temp191=(( struct Cyc_Absyn_EnumType_struct*)
_temp104)->f1; goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_EnumType_struct*)
_temp104)->f2; goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int)
_temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_TunionType: 0){ _LL194: _temp193=((
struct Cyc_Absyn_TunionType_struct*) _temp104)->f1; _LL200: _temp199=( void*)
_temp193.tunion_info; if(*(( int*) _temp199) == Cyc_Absyn_KnownTunion){ _LL202:
_temp201=(( struct Cyc_Absyn_KnownTunion_struct*) _temp199)->f1; goto _LL198;}
else{ goto _LL124;} _LL198: _temp197= _temp193.targs; goto _LL196; _LL196:
_temp195=( void*) _temp193.rgn; goto _LL123;} else{ goto _LL124;} _LL124: if((
unsigned int) _temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_TunionFieldType: 0){
_LL204: _temp203=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp104)->f1;
_LL208: _temp207=( void*) _temp203.field_info; if(*(( int*) _temp207) == Cyc_Absyn_KnownTunionfield){
_LL212: _temp211=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp207)->f1;
goto _LL210; _LL210: _temp209=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp207)->f2; goto _LL206;} else{ goto _LL126;} _LL206: _temp205= _temp203.targs;
goto _LL125;} else{ goto _LL126;} _LL126: goto _LL127; _LL107: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp138); _LL109: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp144); _LL111: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp149); _LL113: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp165)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp167): 0; if( _temp161 != 0){ int _temp215; void*
_temp217; void* _temp219; struct Cyc_Absyn_Tqual _temp221; struct Cyc_Core_Opt*
_temp223; struct Cyc_Absyn_VarargInfo _temp213=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp161)); _LL224: _temp223= _temp213.name; goto _LL222; _LL222:
_temp221= _temp213.tq; goto _LL220; _LL220: _temp219=( void*) _temp213.type;
goto _LL218; _LL218: _temp217=( void*) _temp213.rgn; goto _LL216; _LL216:
_temp215= _temp213.inject; goto _LL214; _LL214: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp219);} return b;} _LL115: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp173); _LL117: if(( void*)
_temp177->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp177->name))->v); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp178)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp177): 0; _LL119: if(( void*) _temp184->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp184->name))->v);
return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_arr obj, struct _tuple0* name, void*(* f)( void*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,(
void*(*)( void*)) Cyc_Core_identity, _temp185)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp184): 0; _LL121: { struct _tuple0 _temp227; struct _tagged_arr*
_temp228; void* _temp230; struct _tuple0* _temp225= _temp191; _temp227=*
_temp225; _LL231: _temp230= _temp227.f1; goto _LL229; _LL229: _temp228= _temp227.f2;
goto _LL226; _LL226: { struct Cyc_List_List* _temp232= Cyc_Interface_get_abs_ns(
_temp230); goto _LL233; _LL233: { struct Cyc_Absyn_Enumdecl* ed;{ struct
_handler_cons _temp234; _push_handler(& _temp234);{ int _temp236= 0; if( setjmp(
_temp234.handler)){ _temp236= 1;} if( ! _temp236){{ struct Cyc_Tcenv_Genv*
_temp237=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp232); goto _LL238; _LL238: ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp237->enumdecls,
_temp228);}; _pop_handler();} else{ void* _temp235=( void*) _exn_thrown; void*
_temp240= _temp235; _LL242: if( _temp240 == Cyc_Dict_Absent){ goto _LL243;}
else{ goto _LL244;} _LL244: goto _LL245; _LL243:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp246=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp246[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp247; _temp247.tag= Cyc_Core_InvalidArg; _temp247.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp248= Cyc_Absynpp_qvar2string(
_temp191); xprintf("check_public_type (can't find enum %.*s)", _temp248.last_plus_one
- _temp248.curr, _temp248.curr);}); _temp247;}); _temp246;})); _LL245:( void)
_throw( _temp240); _LL241:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp191, _tag_arr("enum", sizeof( unsigned char),
5u), ed->name); return 0;} return 1;}}} _LL123: { struct _tuple0 _temp251;
struct _tagged_arr* _temp252; void* _temp254; struct _tuple0* _temp249= _temp201->name;
_temp251=* _temp249; _LL255: _temp254= _temp251.f1; goto _LL253; _LL253:
_temp252= _temp251.f2; goto _LL250; _LL250: { struct Cyc_List_List* _temp256=
Cyc_Interface_get_abs_ns( _temp254); goto _LL257; _LL257: { struct Cyc_Absyn_Tuniondecl*
tud;{ struct _handler_cons _temp258; _push_handler(& _temp258);{ int _temp260= 0;
if( setjmp( _temp258.handler)){ _temp260= 1;} if( ! _temp260){{ struct Cyc_Tcenv_Genv*
_temp261=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp256); goto _LL262; _LL262: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp261->tuniondecls,
_temp252);}; _pop_handler();} else{ void* _temp259=( void*) _exn_thrown; void*
_temp264= _temp259; _LL266: if( _temp264 == Cyc_Dict_Absent){ goto _LL267;}
else{ goto _LL268;} _LL268: goto _LL269; _LL267:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp270=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp270[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp271; _temp271.tag= Cyc_Core_InvalidArg; _temp271.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp272= Cyc_Absynpp_qvar2string(
_temp201->name); xprintf("check_public_type (can't find [x]tunion %.*s)",
_temp272.last_plus_one - _temp272.curr, _temp272.curr);}); _temp271;}); _temp270;}));
_LL269:( void) _throw( _temp264); _LL265:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("[x]tunion", sizeof( unsigned char),
10u), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name, void*(*
f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae,
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp197)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}} _LL125: { struct _tuple0 _temp275; struct _tagged_arr*
_temp276; void* _temp278; struct _tuple0* _temp273= _temp211->name; _temp275=*
_temp273; _LL279: _temp278= _temp275.f1; goto _LL277; _LL277: _temp276= _temp275.f2;
goto _LL274; _LL274: { struct Cyc_List_List* _temp280= Cyc_Interface_get_abs_ns(
_temp278); goto _LL281; _LL281: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp282; _push_handler(& _temp282);{ int _temp284= 0; if( setjmp(
_temp282.handler)){ _temp284= 1;} if( ! _temp284){{ struct Cyc_Tcenv_Genv*
_temp285=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp280); goto _LL286; _LL286: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp285->tuniondecls,
_temp276);}; _pop_handler();} else{ void* _temp283=( void*) _exn_thrown; void*
_temp288= _temp283; _LL290: if( _temp288 == Cyc_Dict_Absent){ goto _LL291;}
else{ goto _LL292;} _LL292: goto _LL293; _LL291:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp294=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp294[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp295; _temp295.tag= Cyc_Core_InvalidArg; _temp295.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp296= Cyc_Absynpp_qvar2string(
_temp211->name); xprintf("check_public_type (can't find [x]tunion %.*s and search its fields)",
_temp296.last_plus_one - _temp296.curr, _temp296.curr);}); _temp295;}); _temp294;}));
_LL293:( void) _throw( _temp288); _LL289:;}}} if( tud->fields == 0){( int)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp297=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp297[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp298; _temp298.tag= Cyc_Core_InvalidArg; _temp298.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp299= Cyc_Absynpp_qvar2string( tud->name);
xprintf("check_public_type ([x]tunion %.*s has no fields)", _temp299.last_plus_one
- _temp299.curr, _temp299.curr);}); _temp298;}); _temp297;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp300=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; goto _LL301; _LL301: for( 0; _temp300 != 0;
_temp300=(( struct Cyc_List_List*) _check_null( _temp300))->tl){ if( Cyc_String_strptrcmp((*
_temp209->name).f2,(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp300))->hd)->name).f2) == 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp300))->hd);
break;}}} if( tuf1 == 0){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp302=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp302[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp303; _temp303.tag= Cyc_Core_InvalidArg;
_temp303.f1=( struct _tagged_arr)({ struct _tagged_arr _temp304= Cyc_Absynpp_qvar2string(
_temp209->name); xprintf("check_public_type (can't find [x]tunionfield %.*s)",
_temp304.last_plus_one - _temp304.curr, _temp304.curr);}); _temp303;}); _temp302;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*) _check_null(
tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct _tagged_arr _temp305= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _temp305.last_plus_one - _temp305.curr,
_temp305.curr);}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp205)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}}} _LL127: return 1; _LL105:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp308; struct Cyc_Interface_I* _temp309; struct Cyc_Interface_Seen*
_temp311; struct Cyc_Dict_Dict* _temp313; int _temp315; struct Cyc_Interface_Ienv*
_temp317; struct Cyc_Interface_Ienv* _temp319; struct _tuple11* _temp306= env;
_temp308=* _temp306; _LL320: _temp319= _temp308.f1; goto _LL318; _LL318:
_temp317= _temp308.f2; goto _LL316; _LL316: _temp315= _temp308.f3; goto _LL314;
_LL314: _temp313= _temp308.f4; goto _LL312; _LL312: _temp311= _temp308.f5; goto
_LL310; _LL310: _temp309= _temp308.f6; goto _LL307; _LL307: { struct Cyc_Absyn_Structdecl*
_temp321=* dp; goto _LL322; _LL322: { void* _temp323=( void*) _temp321->sc;
_LL325: if( _temp323 ==( void*) Cyc_Absyn_Static){ goto _LL326;} else{ goto
_LL327;} _LL327: if( _temp323 ==( void*) Cyc_Absyn_Abstract){ goto _LL328;}
else{ goto _LL329;} _LL329: if( _temp323 ==( void*) Cyc_Absyn_Public){ goto
_LL330;} else{ goto _LL331;} _LL331: if( _temp323 ==( void*) Cyc_Absyn_ExternC){
goto _LL332;} else{ goto _LL333;} _LL333: if( _temp323 ==( void*) Cyc_Absyn_Extern){
goto _LL334;} else{ goto _LL324;} _LL326: if( _temp315? _temp321->fields == 0: 0){
Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp321->name))->v);} goto _LL324; _LL328: if( _temp321->fields ==
0){ if( _temp315){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp321->name))->v);}} else{
_temp321=({ struct Cyc_Absyn_Structdecl* _temp335=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp335[ 0]=* _temp321;
_temp335;}); _temp321->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp313, _temp311, _temp321)){ _temp317->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp317->structdecls, x, _temp321);} goto _LL324;
_LL330: if( _temp321->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp321->name))->v);
_temp321=({ struct Cyc_Absyn_Structdecl* _temp336=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp336[ 0]=* _temp321;
_temp336;});( void*)( _temp321->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp313, _temp311, _temp321)){ _temp317->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp317->structdecls, x,
_temp321);} goto _LL324; _LL332: goto _LL334; _LL334: if( Cyc_Interface_check_public_structdecl(
_temp313, _temp311, _temp321)){ _temp319->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp319->structdecls, x, _temp321);} goto _LL324;
_LL324:;}}} static void Cyc_Interface_extract_uniondecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11
_temp339; struct Cyc_Interface_I* _temp340; struct Cyc_Interface_Seen* _temp342;
struct Cyc_Dict_Dict* _temp344; int _temp346; struct Cyc_Interface_Ienv*
_temp348; struct Cyc_Interface_Ienv* _temp350; struct _tuple11* _temp337= env;
_temp339=* _temp337; _LL351: _temp350= _temp339.f1; goto _LL349; _LL349:
_temp348= _temp339.f2; goto _LL347; _LL347: _temp346= _temp339.f3; goto _LL345;
_LL345: _temp344= _temp339.f4; goto _LL343; _LL343: _temp342= _temp339.f5; goto
_LL341; _LL341: _temp340= _temp339.f6; goto _LL338; _LL338: { struct Cyc_Absyn_Uniondecl*
_temp352=* dp; goto _LL353; _LL353: { void* _temp354=( void*) _temp352->sc;
_LL356: if( _temp354 ==( void*) Cyc_Absyn_Static){ goto _LL357;} else{ goto
_LL358;} _LL358: if( _temp354 ==( void*) Cyc_Absyn_Abstract){ goto _LL359;}
else{ goto _LL360;} _LL360: if( _temp354 ==( void*) Cyc_Absyn_Public){ goto
_LL361;} else{ goto _LL362;} _LL362: if( _temp354 ==( void*) Cyc_Absyn_ExternC){
goto _LL363;} else{ goto _LL364;} _LL364: if( _temp354 ==( void*) Cyc_Absyn_Extern){
goto _LL365;} else{ goto _LL355;} _LL357: if( _temp346? _temp352->fields == 0: 0){
Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp352->name))->v);} goto _LL355; _LL359: if( _temp352->fields ==
0){ if( _temp346){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp352->name))->v);}} else{
_temp352=({ struct Cyc_Absyn_Uniondecl* _temp366=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp366[ 0]=* _temp352;
_temp366;}); _temp352->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp344, _temp342, _temp352)){ _temp348->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp348->uniondecls, x, _temp352);} goto _LL355;
_LL361: if( _temp352->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp352->name))->v);
_temp352=({ struct Cyc_Absyn_Uniondecl* _temp367=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp367[ 0]=* _temp352;
_temp367;});( void*)( _temp352->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp344, _temp342, _temp352)){ _temp348->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp348->uniondecls, x,
_temp352);} goto _LL355; _LL363: goto _LL365; _LL365: if( Cyc_Interface_check_public_uniondecl(
_temp344, _temp342, _temp352)){ _temp350->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp350->uniondecls, x, _temp352);} goto _LL355;
_LL355:;}}} static void Cyc_Interface_extract_enumdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple11 _temp370;
struct Cyc_Interface_I* _temp371; struct Cyc_Interface_Seen* _temp373; struct
Cyc_Dict_Dict* _temp375; int _temp377; struct Cyc_Interface_Ienv* _temp379;
struct Cyc_Interface_Ienv* _temp381; struct _tuple11* _temp368= env; _temp370=*
_temp368; _LL382: _temp381= _temp370.f1; goto _LL380; _LL380: _temp379= _temp370.f2;
goto _LL378; _LL378: _temp377= _temp370.f3; goto _LL376; _LL376: _temp375=
_temp370.f4; goto _LL374; _LL374: _temp373= _temp370.f5; goto _LL372; _LL372:
_temp371= _temp370.f6; goto _LL369; _LL369: { struct Cyc_Absyn_Enumdecl*
_temp383=* dp; goto _LL384; _LL384: { void* _temp385=( void*) _temp383->sc;
_LL387: if( _temp385 ==( void*) Cyc_Absyn_Static){ goto _LL388;} else{ goto
_LL389;} _LL389: if( _temp385 ==( void*) Cyc_Absyn_Abstract){ goto _LL390;}
else{ goto _LL391;} _LL391: if( _temp385 ==( void*) Cyc_Absyn_Public){ goto
_LL392;} else{ goto _LL393;} _LL393: if( _temp385 ==( void*) Cyc_Absyn_ExternC){
goto _LL394;} else{ goto _LL395;} _LL395: if( _temp385 ==( void*) Cyc_Absyn_Extern){
goto _LL396;} else{ goto _LL386;} _LL388: if( _temp377? _temp383->fields == 0: 0){
Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("enum", sizeof( unsigned char), 5u), _temp383->name);} goto _LL386;
_LL390: if( _temp383->fields == 0){ if( _temp377){ Cyc_Interface_fields_err(
_tag_arr("abstract", sizeof( unsigned char), 9u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp383->name);}} else{ _temp383=({ struct Cyc_Absyn_Enumdecl*
_temp397=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp397[ 0]=* _temp383; _temp397;}); _temp383->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp375, _temp373, _temp383)){ _temp379->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp379->enumdecls, x, _temp383);} goto _LL386; _LL392:
if( _temp383->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp383->name);
_temp383=({ struct Cyc_Absyn_Enumdecl* _temp398=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp398[ 0]=* _temp383;
_temp398;});( void*)( _temp383->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp375, _temp373, _temp383)){ _temp379->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp379->enumdecls, x,
_temp383);} goto _LL386; _LL394: goto _LL396; _LL396: if( Cyc_Interface_check_public_enumdecl(
_temp375, _temp373, _temp383)){ _temp381->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp381->enumdecls, x, _temp383);} goto _LL386; _LL386:;}}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp399= 0; goto _LL400; _LL400:{ void* _temp401=(* f->name).f1; struct Cyc_List_List*
_temp409; struct Cyc_List_List* _temp411; _LL403: if(( unsigned int) _temp401 >
1u?*(( int*) _temp401) == Cyc_Absyn_Rel_n: 0){ _LL410: _temp409=(( struct Cyc_Absyn_Rel_n_struct*)
_temp401)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int)
_temp401 > 1u?*(( int*) _temp401) == Cyc_Absyn_Abs_n: 0){ _LL412: _temp411=((
struct Cyc_Absyn_Abs_n_struct*) _temp401)->f1; goto _LL406;} else{ goto _LL407;}
_LL407: goto _LL408; _LL404: _temp411= _temp409; goto _LL406; _LL406: _temp399=
_temp411; goto _LL402; _LL408: goto _LL402; _LL402:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp413=( void*) f->sc; _LL415: if( _temp413 ==( void*) Cyc_Absyn_Static){
goto _LL416;} else{ goto _LL417;} _LL417: if( _temp413 ==( void*) Cyc_Absyn_Extern){
goto _LL418;} else{ goto _LL419;} _LL419: if( _temp413 ==( void*) Cyc_Absyn_Public){
goto _LL420;} else{ goto _LL421;} _LL421: goto _LL422; _LL416: return; _LL418:
dict=& i->imports; goto _LL414; _LL420: dict=& i->exports; goto _LL414; _LL422:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp423=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp423[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp424; _temp424.tag= Cyc_Core_InvalidArg; _temp424.f1=
_tag_arr("add_xtunionfielddecl", sizeof( unsigned char), 21u); _temp424;});
_temp423;})); _LL414:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons
_temp425; _push_handler(& _temp425);{ int _temp427= 0; if( setjmp( _temp425.handler)){
_temp427= 1;} if( ! _temp427){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp399);; _pop_handler();}
else{ void* _temp426=( void*) _exn_thrown; void* _temp429= _temp426; _LL431: if(
_temp429 == Cyc_Dict_Absent){ goto _LL432;} else{ goto _LL433;} _LL433: goto
_LL434; _LL432: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp399, env); goto _LL430; _LL434:( void)
_throw( _temp429); _LL430:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp435=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp435->base= d; _temp435->field= f; _temp435;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp438; struct Cyc_Interface_I* _temp439; struct Cyc_Interface_Seen*
_temp441; struct Cyc_Dict_Dict* _temp443; int _temp445; struct Cyc_Interface_Ienv*
_temp447; struct Cyc_Interface_Ienv* _temp449; struct _tuple11* _temp436= env;
_temp438=* _temp436; _LL450: _temp449= _temp438.f1; goto _LL448; _LL448:
_temp447= _temp438.f2; goto _LL446; _LL446: _temp445= _temp438.f3; goto _LL444;
_LL444: _temp443= _temp438.f4; goto _LL442; _LL442: _temp441= _temp438.f5; goto
_LL440; _LL440: _temp439= _temp438.f6; goto _LL437; _LL437: { struct Cyc_Absyn_Tuniondecl*
_temp451=* dp; goto _LL452; _LL452: { void* _temp453=( void*) _temp451->sc;
_LL455: if( _temp453 ==( void*) Cyc_Absyn_Static){ goto _LL456;} else{ goto
_LL457;} _LL457: if( _temp453 ==( void*) Cyc_Absyn_Abstract){ goto _LL458;}
else{ goto _LL459;} _LL459: if( _temp453 ==( void*) Cyc_Absyn_Public){ goto
_LL460;} else{ goto _LL461;} _LL461: if( _temp453 ==( void*) Cyc_Absyn_ExternC){
goto _LL462;} else{ goto _LL463;} _LL463: if( _temp453 ==( void*) Cyc_Absyn_Extern){
goto _LL464;} else{ goto _LL454;} _LL456: if(( ! _temp451->is_xtunion? _temp445:
0)? _temp451->fields == 0: 0){ Cyc_Interface_fields_err( _tag_arr("static",
sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof( unsigned char), 7u),
_temp451->name);} goto _LL454; _LL458: if( _temp451->fields == 0){ if( !
_temp451->is_xtunion? _temp445: 0){ Cyc_Interface_fields_err( _tag_arr("abstract",
sizeof( unsigned char), 9u), _tag_arr("tunion", sizeof( unsigned char), 7u),
_temp451->name);}} else{ _temp451=({ struct Cyc_Absyn_Tuniondecl* _temp465=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp465[ 0]=* _temp451; _temp465;}); _temp451->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp443, _temp441, _temp451)){ _temp447->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp447->tuniondecls, x, _temp451);} goto _LL454;
_LL460: _temp451=({ struct Cyc_Absyn_Tuniondecl* _temp466=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp466[ 0]=* _temp451;
_temp466;}); if( ! _temp451->is_xtunion? _temp451->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp451->name);( void*)( _temp451->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp443,
_temp441, _temp451)){ if( _temp451->is_xtunion? _temp451->fields != 0: 0){
struct Cyc_List_List* _temp467=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp451->fields))->v; goto _LL468; _LL468: _temp451->fields= 0;{
struct Cyc_List_List* _temp469= _temp467; goto _LL470; _LL470: for( 0; _temp469
!= 0; _temp469=(( struct Cyc_List_List*) _check_null( _temp469))->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp439, _temp451,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp469))->hd);}}} _temp447->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp447->tuniondecls, x, _temp451);} goto _LL454;
_LL462:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp471=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp471[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp472; _temp472.tag= Cyc_Core_InvalidArg;
_temp472.f1= _tag_arr("extract_tuniondecl", sizeof( unsigned char), 19u);
_temp472;}); _temp471;})); _LL464: if( Cyc_Interface_check_public_tuniondecl(
_temp443, _temp441, _temp451)){ if( _temp451->is_xtunion? _temp451->fields != 0:
0){ _temp451=({ struct Cyc_Absyn_Tuniondecl* _temp473=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp473[ 0]=* _temp451;
_temp473;});{ struct Cyc_List_List* _temp474=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp451->fields))->v; goto _LL475; _LL475: _temp451->fields=
0;{ struct Cyc_List_List* _temp476= _temp474; goto _LL477; _LL477: for( 0;
_temp476 != 0; _temp476=(( struct Cyc_List_List*) _check_null( _temp476))->tl){
Cyc_Interface_extract_xtunionfielddecl( _temp439, _temp451,( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp476))->hd);}}}} _temp449->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp449->tuniondecls, x,
_temp451);} goto _LL454; _LL454:;}}} static void Cyc_Interface_extract_typedef(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple11 _temp480; struct Cyc_Interface_I* _temp481; struct Cyc_Interface_Seen*
_temp483; struct Cyc_Dict_Dict* _temp485; int _temp487; struct Cyc_Interface_Ienv*
_temp489; struct Cyc_Interface_Ienv* _temp491; struct _tuple11* _temp478= env;
_temp480=* _temp478; _LL492: _temp491= _temp480.f1; goto _LL490; _LL490:
_temp489= _temp480.f2; goto _LL488; _LL488: _temp487= _temp480.f3; goto _LL486;
_LL486: _temp485= _temp480.f4; goto _LL484; _LL484: _temp483= _temp480.f5; goto
_LL482; _LL482: _temp481= _temp480.f6; goto _LL479; _LL479: if( Cyc_Interface_check_public_typedefdecl(
_temp485, _temp483, d)){ _temp489->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp489->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_arr* x, struct _tuple7* v){ struct _tuple11
_temp495; struct Cyc_Interface_I* _temp496; struct Cyc_Interface_Seen* _temp498;
struct Cyc_Dict_Dict* _temp500; int _temp502; struct Cyc_Interface_Ienv*
_temp504; struct Cyc_Interface_Ienv* _temp506; struct _tuple11* _temp493= env;
_temp495=* _temp493; _LL507: _temp506= _temp495.f1; goto _LL505; _LL505:
_temp504= _temp495.f2; goto _LL503; _LL503: _temp502= _temp495.f3; goto _LL501;
_LL501: _temp500= _temp495.f4; goto _LL499; _LL499: _temp498= _temp495.f5; goto
_LL497; _LL497: _temp496= _temp495.f6; goto _LL494; _LL494: { void* _temp508=(*
v).f1; goto _LL509; _LL509: { void* _temp510= _temp508; void* _temp516; _LL512:
if(*(( int*) _temp510) == Cyc_Tcenv_VarRes){ _LL517: _temp516=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp510)->f1; goto _LL513;} else{ goto _LL514;}
_LL514: goto _LL515; _LL513:{ void* _temp518= _temp516; struct Cyc_Absyn_Fndecl*
_temp526; struct Cyc_Absyn_Vardecl* _temp528; _LL520: if(( unsigned int)
_temp518 > 1u?*(( int*) _temp518) == Cyc_Absyn_Funname_b: 0){ _LL527: _temp526=((
struct Cyc_Absyn_Funname_b_struct*) _temp518)->f1; goto _LL521;} else{ goto
_LL522;} _LL522: if(( unsigned int) _temp518 > 1u?*(( int*) _temp518) == Cyc_Absyn_Global_b:
0){ _LL529: _temp528=(( struct Cyc_Absyn_Global_b_struct*) _temp518)->f1; goto
_LL523;} else{ goto _LL524;} _LL524: goto _LL525; _LL521: { struct Cyc_Absyn_Vardecl*
_temp532=({ struct Cyc_Absyn_Vardecl* _temp530=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp530->sc=( void*)(( void*)
_temp526->sc); _temp530->name= _temp526->name; _temp530->tq=({ struct Cyc_Absyn_Tqual
_temp531; _temp531.q_const= 0; _temp531.q_volatile= 0; _temp531.q_restrict= 0;
_temp531;}); _temp530->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp526->cached_typ))->v); _temp530->initializer= 0; _temp530->rgn=
0; _temp530->attributes= 0; _temp530;}); goto _LL533; _LL533: _temp502= 0;
_temp528= _temp532; goto _LL523;} _LL523: if( _temp528->initializer != 0){
_temp528=({ struct Cyc_Absyn_Vardecl* _temp534=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp534[ 0]=* _temp528; _temp534;});
_temp528->initializer= 0;}{ void* _temp535=( void*) _temp528->sc; _LL537: if(
_temp535 ==( void*) Cyc_Absyn_Static){ goto _LL538;} else{ goto _LL539;} _LL539:
if( _temp535 ==( void*) Cyc_Absyn_Abstract){ goto _LL540;} else{ goto _LL541;}
_LL541: if( _temp535 ==( void*) Cyc_Absyn_Public){ goto _LL542;} else{ goto
_LL543;} _LL543: if( _temp535 ==( void*) Cyc_Absyn_ExternC){ goto _LL544;} else{
goto _LL545;} _LL545: if( _temp535 ==( void*) Cyc_Absyn_Extern){ goto _LL546;}
else{ goto _LL536;} _LL538: if( _temp502? Cyc_Tcutil_is_function_type(( void*)
_temp528->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp528->name);} goto _LL536; _LL540:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp547=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp547[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp548; _temp548.tag= Cyc_Core_InvalidArg; _temp548.f1=
_tag_arr("extract_ordinarie", sizeof( unsigned char), 18u); _temp548;});
_temp547;})); _LL542: if( _temp502? Cyc_Tcutil_is_function_type(( void*)
_temp528->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp528->name);} if( Cyc_Interface_check_public_vardecl(
_temp500, _temp498, _temp528)){ _temp504->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data))
Cyc_Dict_insert)( _temp504->vardecls, x, _temp528);} goto _LL536; _LL544: goto
_LL546; _LL546: if( Cyc_Interface_check_public_vardecl( _temp500, _temp498,
_temp528)){ _temp506->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp506->vardecls, x, _temp528);} goto _LL536; _LL536:;} goto _LL519; _LL525:
goto _LL519; _LL519:;} goto _LL511; _LL515: goto _LL511; _LL511:;}}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp551; struct Cyc_Interface_Seen* _temp552; struct Cyc_Dict_Dict* _temp554;
int _temp556; struct Cyc_Interface_I* _temp558; struct _tuple12* _temp549= env_f;
_temp551=* _temp549; _LL559: _temp558= _temp551.f1; goto _LL557; _LL557:
_temp556= _temp551.f2; goto _LL555; _LL555: _temp554= _temp551.f3; goto _LL553;
_LL553: _temp552= _temp551.f4; goto _LL550; _LL550: { struct _tuple11 _temp561=({
struct _tuple11 _temp560; _temp560.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp558->imports,
ns); _temp560.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp558->exports, ns); _temp560.f3=
_temp556; _temp560.f4= _temp554; _temp560.f5= _temp552; _temp560.f6= _temp558;
_temp560;}); goto _LL562; _LL562:(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& _temp561, ge->structdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp561, ge->uniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp561, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp561, ge->enumdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp561, ge->typedefs);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct _tuple7*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp561, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple12 _temp564=({
struct _tuple12 _temp563; _temp563.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp563.f2= check_complete_defs; _temp563.f3=
ae; _temp563.f4= Cyc_Interface_new_seen(); _temp563;}); goto _LL565; _LL565:((
void(*)( void(* f)( struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*),
struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp564, ae); return _temp564.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_arr*); struct
_tagged_arr f4; struct _tagged_arr* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_arr* x, void* y1){ struct _tuple13 _temp568;
struct _tagged_arr* _temp569; struct _tagged_arr _temp571; int(* _temp573)( void*,
void*, struct _tagged_arr*); struct Cyc_Dict_Dict* _temp575; int _temp577; int*
_temp579; struct _tuple13* _temp566= env; _temp568=* _temp566; _LL578: _temp577=
_temp568.f1; _temp579=&(* _temp566).f1; goto _LL576; _LL576: _temp575= _temp568.f2;
goto _LL574; _LL574: _temp573= _temp568.f3; goto _LL572; _LL572: _temp571=
_temp568.f4; goto _LL570; _LL570: _temp569= _temp568.f5; goto _LL567; _LL567: {
struct _handler_cons _temp580; _push_handler(& _temp580);{ int _temp582= 0; if(
setjmp( _temp580.handler)){ _temp582= 1;} if( ! _temp582){{ void* _temp583=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp575, x); goto _LL584; _LL584: if( ! _temp573( y1, _temp583, _temp569)){*
_temp579= 0;}}; _pop_handler();} else{ void* _temp581=( void*) _exn_thrown; void*
_temp586= _temp581; _LL588: if( _temp586 == Cyc_Dict_Absent){ goto _LL589;}
else{ goto _LL590;} _LL590: goto _LL591; _LL589: Cyc_Interface_check_err(
_temp569,( struct _tagged_arr)({ struct _tagged_arr _temp592= _temp571; struct
_tagged_arr _temp593=* x; xprintf("%.*s %.*s is missing", _temp592.last_plus_one
- _temp592.curr, _temp592.curr, _temp593.last_plus_one - _temp593.curr, _temp593.curr);}));*
_temp579= 0; goto _LL587; _LL591:( void) _throw( _temp586); _LL587:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg){ struct _tuple13 _temp595=({ struct _tuple13 _temp594;
_temp594.f1= 1; _temp594.f2= dic2; _temp594.f3= incl_f; _temp594.f4= t; _temp594.f5=
msg; _temp594;}); goto _LL596; _LL596:(( void(*)( void(* f)( struct _tuple13*,
struct _tagged_arr*, void*), struct _tuple13* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple13* env, struct _tagged_arr* x, void* y1)) Cyc_Interface_incl_dict_f,&
_temp595, dic1); return _temp595.f1;} static int Cyc_Interface_incl_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Structdecl* _temp597= Cyc_Tcdecl_merge_structdecl(
d0, d1, 0, msg); goto _LL598; _LL598: if( _temp597 == 0){ return 0;} if(( struct
Cyc_Absyn_Structdecl*) d0 != _temp597){ Cyc_Interface_check_err( msg,( struct
_tagged_arr)({ struct _tagged_arr _temp599= Cyc_Absynpp_qvar2string(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp599.last_plus_one - _temp599.curr, _temp599.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct
Cyc_Absyn_Uniondecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Uniondecl*
_temp600= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); goto _LL601; _LL601: if(
_temp600 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 != _temp600){
Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct _tagged_arr _temp602=
Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp602.last_plus_one - _temp602.curr, _temp602.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl*
_temp603= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); goto _LL604; _LL604: if(
_temp603 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 != _temp603){
Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct _tagged_arr _temp605=
Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp605.last_plus_one - _temp605.curr, _temp605.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl*
_temp606= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); goto _LL607; _LL607: if(
_temp606 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp606){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr _temp608= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp608.last_plus_one - _temp608.curr, _temp608.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl* _temp609= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL610; _LL610: if( _temp609 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp609){ Cyc_Interface_check_err( msg,( struct
_tagged_arr)({ struct _tagged_arr _temp611= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of variable %.*s discloses too much information", _temp611.last_plus_one
- _temp611.curr, _temp611.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl* _temp612= Cyc_Tcdecl_merge_typedefdecl(
d0, d1, 0, msg); goto _LL613; _LL613: if( _temp612 == 0){ return 0;} if(( struct
Cyc_Absyn_Typedefdecl*) d0 != _temp612){ Cyc_Interface_check_err( msg,( struct
_tagged_arr)({ struct _tagged_arr _temp614= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of typedef %.*s discloses too much information", _temp614.last_plus_one
- _temp614.curr, _temp614.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl* _temp615= Cyc_Tcdecl_merge_xtunionfielddecl(
d0, d1, 0, msg); goto _LL616; _LL616: if( _temp615 == 0){ return 0;} if(( struct
Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp615){ Cyc_Interface_check_err( msg,(
struct _tagged_arr)({ struct _tagged_arr _temp617= Cyc_Absynpp_qvar2string(( d1->field)->name);
xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp617.last_plus_one - _temp617.curr, _temp617.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_arr* msg){ int _temp618=((
int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->structdecls,
ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct", sizeof(
unsigned char), 7u), msg); goto _LL619; _LL619: { int _temp620=(( int(*)( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
goto _LL621; _LL621: { int _temp622=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct
Cyc_Absyn_Tuniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->tuniondecls, ie2->tuniondecls,
Cyc_Interface_incl_tuniondecl, _tag_arr("tunion", sizeof( unsigned char), 7u),
msg); goto _LL623; _LL623: { int _temp624=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*, struct
Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); goto _LL625; _LL625: { int
_temp626=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) != 0; goto _LL627; _LL627: { int _temp628=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); goto _LL629; _LL629: {
int _temp630=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,
_tag_arr("xtunionfield", sizeof( unsigned char), 13u), msg); goto _LL631; _LL631:
return((((( _temp618? _temp620: 0)? _temp622: 0)? _temp624: 0)? _temp626: 0)?
_temp628: 0)? _temp630: 0;}}}}}}} struct _tuple14{ int f1; struct Cyc_Dict_Dict*
f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f( struct
_tuple14* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){ struct
_tuple14 _temp634; struct _tagged_arr* _temp635; struct Cyc_Dict_Dict* _temp637;
int _temp639; int* _temp641; struct _tuple14* _temp632= env; _temp634=* _temp632;
_LL640: _temp639= _temp634.f1; _temp641=&(* _temp632).f1; goto _LL638; _LL638:
_temp637= _temp634.f2; goto _LL636; _LL636: _temp635= _temp634.f3; goto _LL633;
_LL633: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp642;
_push_handler(& _temp642);{ int _temp644= 0; if( setjmp( _temp642.handler)){
_temp644= 1;} if( ! _temp644){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp637, ns);; _pop_handler();}
else{ void* _temp643=( void*) _exn_thrown; void* _temp646= _temp643; _LL648: if(
_temp646 == Cyc_Dict_Absent){ goto _LL649;} else{ goto _LL650;} _LL650: goto
_LL651; _LL649: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL647; _LL651:( void)
_throw( _temp646); _LL647:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp635)){*
_temp641= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple14 _temp653=({
struct _tuple14 _temp652; _temp652.f1= 1; _temp652.f2= dic2; _temp652.f3= msg;
_temp652;}); goto _LL654; _LL654:(( void(*)( void(* f)( struct _tuple14*, struct
Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple14* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_ns_f,& _temp653, dic1); return _temp653.f1;}
int Cyc_Interface_is_subinterface( struct Cyc_Interface_I* i1, struct Cyc_Interface_I*
i2, struct _tuple3* info){ struct _handler_cons _temp655; _push_handler(&
_temp655);{ int _temp657= 0; if( setjmp( _temp655.handler)){ _temp657= 1;} if( !
_temp657){{ int _temp658= 1; goto _LL659; _LL659: { struct _tagged_arr* msg= 0;
if( info != 0){ msg=({ struct _tagged_arr* _temp660=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp660[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp661=(*(( struct _tuple3*) _check_null( info))).f1;
struct _tagged_arr _temp662=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("checking inclusion of %.*s exports into %.*s exports,", _temp661.last_plus_one
- _temp661.curr, _temp661.curr, _temp662.last_plus_one - _temp662.curr, _temp662.curr);});
_temp660;});} if( ! Cyc_Interface_incl_ns( i1->exports, i2->exports, msg)){
_temp658= 0;} if( info != 0){ msg=({ struct _tagged_arr* _temp663=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp663[ 0]=( struct
_tagged_arr)({ struct _tagged_arr _temp664=(*(( struct _tuple3*) _check_null(
info))).f2; struct _tagged_arr _temp665=(*(( struct _tuple3*) _check_null( info))).f1;
xprintf("checking inclusion of %.*s imports into %.*s imports,", _temp664.last_plus_one
- _temp664.curr, _temp664.curr, _temp665.last_plus_one - _temp665.curr, _temp665.curr);});
_temp663;});} if( ! Cyc_Interface_incl_ns( i2->imports, i1->imports, msg)){
_temp658= 0;}{ int _temp666= _temp658; _npop_handler( 0u); return _temp666;}}};
_pop_handler();} else{ void* _temp656=( void*) _exn_thrown; void* _temp668=
_temp656; _LL670: if( _temp668 == Cyc_Tcdecl_Incompatible){ goto _LL671;} else{
goto _LL672;} _LL672: goto _LL673; _LL671: return 0; _LL673:( void) _throw(
_temp668); _LL669:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict* f2; struct
Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**, void**,
struct Cyc_Position_Segment*, struct _tagged_arr*); struct _tagged_arr f6;
struct _tagged_arr* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_arr* x, void** y2){ struct _tuple15 _temp676;
struct _tagged_arr* _temp677; struct _tagged_arr _temp679; void**(* _temp681)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct Cyc_Dict_Dict*
_temp683; struct Cyc_Dict_Dict* _temp685; struct Cyc_Dict_Dict* _temp687; struct
Cyc_Dict_Dict** _temp689; int _temp690; int* _temp692; struct _tuple15* _temp674=
env; _temp676=* _temp674; _LL691: _temp690= _temp676.f1; _temp692=&(* _temp674).f1;
goto _LL688; _LL688: _temp687= _temp676.f2; _temp689=&(* _temp674).f2; goto
_LL686; _LL686: _temp685= _temp676.f3; goto _LL684; _LL684: _temp683= _temp676.f4;
goto _LL682; _LL682: _temp681= _temp676.f5; goto _LL680; _LL680: _temp679=
_temp676.f6; goto _LL678; _LL678: _temp677= _temp676.f7; goto _LL675; _LL675: {
void** y;{ struct _handler_cons _temp693; _push_handler(& _temp693);{ int
_temp695= 0; if( setjmp( _temp693.handler)){ _temp695= 1;} if( ! _temp695){{
void** _temp696=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp685, x); goto _LL697; _LL697: { void** _temp698= _temp681(
_temp696, y2, 0, _temp677); goto _LL699; _LL699: if( !(( unsigned int) _temp698)){*
_temp692= 0; _npop_handler( 0u); return;} y=( void**) _check_null( _temp698);}};
_pop_handler();} else{ void* _temp694=( void*) _exn_thrown; void* _temp701=
_temp694; _LL703: if( _temp701 == Cyc_Dict_Absent){ goto _LL704;} else{ goto
_LL705;} _LL705: goto _LL706; _LL704: y= y2; goto _LL702; _LL706:( void) _throw(
_temp701); _LL702:;}}}{ struct _handler_cons _temp707; _push_handler(& _temp707);{
int _temp709= 0; if( setjmp( _temp707.handler)){ _temp709= 1;} if( ! _temp709){{
void** _temp710=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp683, x); goto _LL711; _LL711: { void** _temp712= _temp681(
_temp710, y, 0, _temp677); goto _LL713; _LL713: if( _temp712 !=( void**)
_temp710){ if(( unsigned int) _temp712){ Cyc_Interface_check_err( _temp677,(
struct _tagged_arr)({ struct _tagged_arr _temp714= _temp679; struct _tagged_arr
_temp715=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp714.last_plus_one - _temp714.curr, _temp714.curr, _temp715.last_plus_one -
_temp715.curr, _temp715.curr);}));}* _temp692= 0;}}}; _pop_handler();} else{
void* _temp708=( void*) _exn_thrown; void* _temp717= _temp708; _LL719: if(
_temp717 == Cyc_Dict_Absent){ goto _LL720;} else{ goto _LL721;} _LL721: goto
_LL722; _LL720: if(* _temp692){* _temp689=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void** data)) Cyc_Dict_insert)(*
_temp689, x, y);} goto _LL718; _LL722:( void) _throw( _temp717); _LL718:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg){ struct _tuple15 _temp724=({ struct _tuple15 _temp723;
_temp723.f1= 1; _temp723.f2= dic1; _temp723.f3= dic1; _temp723.f4= excl;
_temp723.f5= merge_f; _temp723.f6= t; _temp723.f7= msg; _temp723;}); goto _LL725;
_LL725:(( void(*)( void(* f)( struct _tuple15*, struct _tagged_arr*, void**),
struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple15* env, struct _tagged_arr* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp724, dic2); if( _temp724.f1){ return({ struct Cyc_Core_Opt* _temp726=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp726->v=(
void*) _temp724.f2; _temp726;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_arr* msg){ struct Cyc_Core_Opt*
_temp727=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); goto _LL728; _LL728: {
struct Cyc_Core_Opt* _temp729=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(*
merge_f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); goto _LL730; _LL730: {
struct Cyc_Core_Opt* _temp731=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(*
merge_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); goto _LL732; _LL732: {
struct Cyc_Core_Opt* _temp733=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(*
merge_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); goto _LL734; _LL734: {
struct Cyc_Core_Opt* _temp735=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,
iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof(
unsigned char), 8u), msg); goto _LL736; _LL736: { struct Cyc_Core_Opt* _temp737=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); goto _LL738; _LL738: {
struct Cyc_Core_Opt* _temp739=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(*
merge_f)( struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); goto _LL740; _LL740: if(((((( !((
unsigned int) _temp727)? 1: !(( unsigned int) _temp729))? 1: !(( unsigned int)
_temp731))? 1: !(( unsigned int) _temp733))? 1: !(( unsigned int) _temp735))? 1:
!(( unsigned int) _temp737))? 1: !(( unsigned int) _temp739)){ return 0;} else{
return({ struct Cyc_Interface_Ienv* _temp741=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp741->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp727))->v; _temp741->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp729))->v;
_temp741->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp731))->v; _temp741->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp733))->v; _temp741->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp735))->v;
_temp741->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp737))->v; _temp741->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp739))->v; _temp741;});}}}}}}}} struct _tuple16{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; struct
_tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct _tuple16* env,
struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple16
_temp744; struct _tagged_arr* _temp745; struct Cyc_Dict_Dict* _temp747; struct
Cyc_Dict_Dict* _temp749; struct Cyc_Dict_Dict* _temp751; struct Cyc_Dict_Dict**
_temp753; int _temp754; int* _temp756; struct _tuple16* _temp742= env; _temp744=*
_temp742; _LL755: _temp754= _temp744.f1; _temp756=&(* _temp742).f1; goto _LL752;
_LL752: _temp751= _temp744.f2; _temp753=&(* _temp742).f2; goto _LL750; _LL750:
_temp749= _temp744.f3; goto _LL748; _LL748: _temp747= _temp744.f4; goto _LL746;
_LL746: _temp745= _temp744.f5; goto _LL743; _LL743: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp757;
_push_handler(& _temp757);{ int _temp759= 0; if( setjmp( _temp757.handler)){
_temp759= 1;} if( ! _temp759){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp747, ns);;
_pop_handler();} else{ void* _temp758=( void*) _exn_thrown; void* _temp761=
_temp758; _LL763: if( _temp761 == Cyc_Dict_Absent){ goto _LL764;} else{ goto
_LL765;} _LL765: goto _LL766; _LL764: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL762; _LL766:( void) _throw( _temp761); _LL762:;}}}{ struct _handler_cons
_temp767; _push_handler(& _temp767);{ int _temp769= 0; if( setjmp( _temp767.handler)){
_temp769= 1;} if( ! _temp769){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp749, ns);{ struct Cyc_Interface_Ienv*
_temp770= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp745); goto _LL771; _LL771: if( _temp770 == 0){* _temp756= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*) _check_null(
_temp770);}; _pop_handler();} else{ void* _temp768=( void*) _exn_thrown; void*
_temp773= _temp768; _LL775: if( _temp773 == Cyc_Dict_Absent){ goto _LL776;}
else{ goto _LL777;} _LL777: goto _LL778; _LL776: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL774; _LL778:( void) _throw( _temp773); _LL774:;}}}{ struct Cyc_Interface_Ienv*
_temp779= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp745); goto
_LL780; _LL780: if( _temp779 == 0){* _temp756= 0;} else{ if(* _temp756){*
_temp753=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp753, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp779));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_arr* msg){ struct _tuple16 _temp782=({ struct _tuple16
_temp781; _temp781.f1= 1; _temp781.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp781.f3= dic1; _temp781.f4= excl; _temp781.f5= msg; _temp781;}); goto _LL783;
_LL783:(( void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct
Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_ns_f,& _temp782, dic2); if( _temp782.f1){ return({
struct Cyc_Core_Opt* _temp784=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp784->v=( void*) _temp782.f2; _temp784;});} else{ return 0;}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr f4; struct _tagged_arr* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_arr* x, void* y){ struct _tuple17 _temp787;
struct _tagged_arr* _temp788; struct _tagged_arr _temp790; struct Cyc_Dict_Dict*
_temp792; struct Cyc_Dict_Dict* _temp794; struct Cyc_Dict_Dict** _temp796; int
_temp797; int* _temp799; struct _tuple17* _temp785= env; _temp787=* _temp785;
_LL798: _temp797= _temp787.f1; _temp799=&(* _temp785).f1; goto _LL795; _LL795:
_temp794= _temp787.f2; _temp796=&(* _temp785).f2; goto _LL793; _LL793: _temp792=
_temp787.f3; goto _LL791; _LL791: _temp790= _temp787.f4; goto _LL789; _LL789:
_temp788= _temp787.f5; goto _LL786; _LL786: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_member)( _temp792, x)){ Cyc_Interface_check_err(
_temp788,( struct _tagged_arr)({ struct _tagged_arr _temp800= _temp790; struct
_tagged_arr _temp801=* x; xprintf("%.*s %.*s is exported more than once",
_temp800.last_plus_one - _temp800.curr, _temp800.curr, _temp801.last_plus_one -
_temp801.curr, _temp801.curr);}));* _temp799= 0;} else{ if(* _temp799){*
_temp796=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)(* _temp796, x, y);}}} struct Cyc_Core_Opt*
Cyc_Interface_disj_merge_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple17 _temp803=({
struct _tuple17 _temp802; _temp802.f1= 1; _temp802.f2= dic1; _temp802.f3= dic1;
_temp802.f4= t; _temp802.f5= msg; _temp802;}); goto _LL804; _LL804:(( void(*)(
void(* f)( struct _tuple17*, struct _tagged_arr*, void*), struct _tuple17* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple17* env,
struct _tagged_arr* x, void* y)) Cyc_Interface_disj_merge_dict_f,& _temp803,
dic2); if( _temp803.f1){ return({ struct Cyc_Core_Opt* _temp805=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp805->v=( void*) _temp803.f2;
_temp805;});} else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_arr* msg){ struct Cyc_Core_Opt* _temp806=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); goto _LL807; _LL807: {
struct Cyc_Core_Opt* _temp808=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls, _tag_arr("union",
sizeof( unsigned char), 6u), msg); goto _LL809; _LL809: { struct Cyc_Core_Opt*
_temp810=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, _tag_arr("[x]tunion", sizeof( unsigned char),
10u), msg); goto _LL811; _LL811: { struct Cyc_Core_Opt* _temp812=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); goto _LL813; _LL813: {
struct Cyc_Core_Opt* _temp814=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef",
sizeof( unsigned char), 8u), msg); goto _LL815; _LL815: { struct Cyc_Core_Opt*
_temp816=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->vardecls, ie2->vardecls, _tag_arr("variable", sizeof( unsigned char), 9u),
msg); goto _LL817; _LL817: { struct Cyc_Core_Opt* _temp818=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof( unsigned char), 13u),
msg); goto _LL819; _LL819: if(((((( !(( unsigned int) _temp806)? 1: !((
unsigned int) _temp808))? 1: !(( unsigned int) _temp810))? 1: !(( unsigned int)
_temp812))? 1: !(( unsigned int) _temp814))? 1: !(( unsigned int) _temp816))? 1:
!(( unsigned int) _temp818)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp820=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp820->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp806))->v; _temp820->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp808))->v; _temp820->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp810))->v; _temp820->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp812))->v;
_temp820->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp814))->v; _temp820->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp816))->v; _temp820->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp818))->v; _temp820;});}}}}}}}}
struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct _tuple18*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple18
_temp823; struct _tagged_arr* _temp824; struct Cyc_Dict_Dict* _temp826; struct
Cyc_Dict_Dict* _temp828; struct Cyc_Dict_Dict** _temp830; int _temp831; int*
_temp833; struct _tuple18* _temp821= env; _temp823=* _temp821; _LL832: _temp831=
_temp823.f1; _temp833=&(* _temp821).f1; goto _LL829; _LL829: _temp828= _temp823.f2;
_temp830=&(* _temp821).f2; goto _LL827; _LL827: _temp826= _temp823.f3; goto
_LL825; _LL825: _temp824= _temp823.f4; goto _LL822; _LL822: { struct Cyc_Interface_Ienv*
ie1;{ struct _handler_cons _temp834; _push_handler(& _temp834);{ int _temp836= 0;
if( setjmp( _temp834.handler)){ _temp836= 1;} if( ! _temp836){ ie1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp826,
ns);; _pop_handler();} else{ void* _temp835=( void*) _exn_thrown; void* _temp838=
_temp835; _LL840: if( _temp838 == Cyc_Dict_Absent){ goto _LL841;} else{ goto
_LL842;} _LL842: goto _LL843; _LL841: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL839; _LL843:( void) _throw( _temp838); _LL839:;}}}{ struct Cyc_Interface_Ienv*
_temp844= Cyc_Interface_disj_merge_ienv( ie1, ie2, _temp824); goto _LL845;
_LL845: if( _temp844 == 0){* _temp833= 0;} else{ if(* _temp833){* _temp830=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp830, ns,( struct Cyc_Interface_Ienv*)
_check_null( _temp844));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){
struct _tuple18 _temp847=({ struct _tuple18 _temp846; _temp846.f1= 1; _temp846.f2=
dic1; _temp846.f3= dic1; _temp846.f4= msg; _temp846;}); goto _LL848; _LL848:((
void(*)( void(* f)( struct _tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct _tuple18* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp847, dic2); if( _temp847.f1){ return({ struct Cyc_Core_Opt* _temp849=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp849->v=(
void*) _temp847.f2; _temp849;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp850; _push_handler(& _temp850);{ int _temp852= 0; if(
setjmp( _temp850.handler)){ _temp852= 1;} if( ! _temp852){{ struct _tagged_arr*
msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp853=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp853[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp854=(*(( struct _tuple3*) _check_null( info))).f1;
struct _tagged_arr _temp855=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging exports of %.*s and %.*s,", _temp854.last_plus_one - _temp854.curr,
_temp854.curr, _temp855.last_plus_one - _temp855.curr, _temp855.curr);});
_temp853;});}{ struct Cyc_Core_Opt* _temp856= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL857; _LL857: if( _temp856 == 0){ struct Cyc_Interface_I*
_temp858= 0; _npop_handler( 0u); return _temp858;} if( info != 0){ msg=({ struct
_tagged_arr* _temp859=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp859[ 0]=( struct _tagged_arr)({ struct _tagged_arr _temp860=(*((
struct _tuple3*) _check_null( info))).f1; struct _tagged_arr _temp861=(*((
struct _tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp860.last_plus_one - _temp860.curr, _temp860.curr, _temp861.last_plus_one -
_temp861.curr, _temp861.curr);}); _temp859;});}{ struct Cyc_Core_Opt* _temp862=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp856))->v, msg); goto _LL863; _LL863: if(
_temp862 == 0){ struct Cyc_Interface_I* _temp864= 0; _npop_handler( 0u); return
_temp864;}{ struct Cyc_Interface_I* _temp866=({ struct Cyc_Interface_I* _temp865=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp865->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp862))->v;
_temp865->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp856))->v; _temp865;}); _npop_handler( 0u); return _temp866;}}}};
_pop_handler();} else{ void* _temp851=( void*) _exn_thrown; void* _temp868=
_temp851; _LL870: if( _temp868 == Cyc_Tcdecl_Incompatible){ goto _LL871;} else{
goto _LL872;} _LL872: goto _LL873; _LL871: return 0; _LL873:( void) _throw(
_temp868); _LL869:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp875= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp874=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp874->f1=*(( struct _tagged_arr*)
_check_null( curr_info)); _temp874->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp874;}): 0); goto _LL876; _LL876: if( _temp875 ==
0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null( _temp875); if(
curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_arr* _temp877=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp877[ 0]=( struct
_tagged_arr)({ struct _tagged_arr _temp878=*(( struct _tagged_arr*) _check_null(
curr_info)); struct _tagged_arr _temp879=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); xprintf("%.*s+%.*s", _temp878.last_plus_one -
_temp878.curr, _temp878.curr, _temp879.last_plus_one - _temp879.curr, _temp879.curr);});
_temp877;}): 0;} if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null(
linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo){ if( la == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i= get((
void*)(( struct Cyc_List_List*) _check_null( la))->hd); struct _tagged_arr*
curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd): 0; la=(( struct Cyc_List_List*) _check_null( la))->tl;
if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null( linfo))->tl;} for(
0; la != 0; la=(( struct Cyc_List_List*) _check_null( la))->tl){ struct Cyc_Interface_I*
_temp881= Cyc_Interface_merge( curr_i, get(( void*)(( struct Cyc_List_List*)
_check_null( la))->hd),( curr_info != 0? linfo != 0: 0)?({ struct _tuple3*
_temp880=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp880->f1=*((
struct _tagged_arr*) _check_null( curr_info)); _temp880->f2=*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd); _temp880;}): 0);
goto _LL882; _LL882: if( _temp881 == 0){ return 0;} curr_i=( struct Cyc_Interface_I*)
_check_null( _temp881); if( curr_info != 0){ curr_info= linfo != 0?({ struct
_tagged_arr* _temp883=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp883[ 0]=( struct _tagged_arr)({ struct _tagged_arr _temp884=*((
struct _tagged_arr*) _check_null( curr_info)); struct _tagged_arr _temp885=*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd); xprintf("%.*s+%.*s",
_temp884.last_plus_one - _temp884.curr, _temp884.curr, _temp885.last_plus_one -
_temp885.curr, _temp885.curr);}); _temp883;}): 0;} if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;}} return( struct Cyc_Interface_I*)
curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace( struct Cyc_List_List*
tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;} return({ struct Cyc_List_List*
_temp886=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp886->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp887=( struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp887[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp888; _temp888.tag= Cyc_Absyn_Namespace_d;
_temp888.f1=( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp888.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp888;}); _temp887;}), 0); _temp886->tl= 0; _temp886;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp889=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp889->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp890=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp890[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp891; _temp891.tag= Cyc_Absyn_Struct_d;
_temp891.f1= d; _temp891;}); _temp890;}), 0); _temp889->tl= tds; _temp889;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp892=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp892[ 0]=* d; _temp892;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp893->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp894=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp894[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp895; _temp895.tag= Cyc_Absyn_Struct_d; _temp895.f1=
d; _temp895;}); _temp894;}), 0); _temp893->tl= tds; _temp893;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp896=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp896->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp897=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp897[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp898; _temp898.tag= Cyc_Absyn_Union_d;
_temp898.f1= d; _temp898;}); _temp897;}), 0); _temp896->tl= tds; _temp896;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp899=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp899[ 0]=* d; _temp899;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp900->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp901=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp901[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp902; _temp902.tag= Cyc_Absyn_Union_d; _temp902.f1= d; _temp902;}); _temp901;}),
0); _temp900->tl= tds; _temp900;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp903=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp903->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp904=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp904[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp905; _temp905.tag= Cyc_Absyn_Tunion_d; _temp905.f1=
d; _temp905;}); _temp904;}), 0); _temp903->tl= tds; _temp903;});} static
unsigned char _temp906[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp906, _temp906, _temp906 + 2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp907=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp907->f1= Cyc_Absyn_empty_tqual();
_temp907->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp908=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp908[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp909; _temp909.tag= Cyc_Absyn_VarType; _temp909.f1=({ struct Cyc_Absyn_Tvar*
_temp910=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp910->name= Cyc_Interface_us_p; _temp910->identity= 0; _temp910->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_BoxKind);
_temp910;}); _temp909;}); _temp908;}); _temp907;});} static struct Cyc_Absyn_Tunionfield*
Cyc_Interface_rewrite_tunionfield( struct Cyc_Absyn_Tunionfield* f){ f=({ struct
Cyc_Absyn_Tunionfield* _temp911=( struct Cyc_Absyn_Tunionfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Tunionfield)); _temp911[ 0]=* f; _temp911;}); f->typs=((
struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct _tuple10*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type, f->typs); return f;}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp912=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp912[ 0]=* d; _temp912;}); if( d->fields
!= 0){ d->fields=({ struct Cyc_Core_Opt* _temp913=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp913->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp913;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp914=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp914->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp915=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp915[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp916; _temp916.tag= Cyc_Absyn_Tunion_d; _temp916.f1=
d; _temp916;}); _temp915;}), 0); _temp914->tl= tds; _temp914;});} static struct
Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp917=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp917->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp918=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp918[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp919; _temp919.tag= Cyc_Absyn_Enum_d;
_temp919.f1= d; _temp919;}); _temp918;}), 0); _temp917->tl= tds; _temp917;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp920=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp920[ 0]=* d; _temp920;}); d->fields= 0;
if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp921->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp922=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp922[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp923; _temp923.tag= Cyc_Absyn_Enum_d; _temp923.f1= d; _temp923;}); _temp922;}),
0); _temp921->tl= tds; _temp921;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp924=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp924->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp925=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp925[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp926; _temp926.tag= Cyc_Absyn_Typedef_d; _temp926.f1=
d; _temp926;}); _temp925;}), 0); _temp924->tl= tds; _temp924;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp927=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp927->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp928=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp928[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp929; _temp929.tag= Cyc_Absyn_Var_d;
_temp929.f1= d; _temp929;}); _temp928;}), 0); _temp927->tl= tds; _temp927;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp932; struct Cyc_Absyn_Tunionfield*
_temp933; struct Cyc_Absyn_Tuniondecl* _temp935; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp930= d; _temp932=* _temp930; _LL936: _temp935= _temp932.base; goto _LL934;
_LL934: _temp933= _temp932.field; goto _LL931; _LL931: _temp935=({ struct Cyc_Absyn_Tuniondecl*
_temp937=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp937[ 0]=* _temp935; _temp937;}); _temp935->fields=({ struct Cyc_Core_Opt*
_temp938=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp938->v=( void*)({ struct Cyc_List_List* _temp939=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp939->hd=( void*) _temp933;
_temp939->tl= 0; _temp939;}); _temp938;});( void*)( _temp935->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp940=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp940->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp941=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp941[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp942; _temp942.tag= Cyc_Absyn_Tunion_d; _temp942.f1=
_temp935; _temp942;}); _temp941;}), 0); _temp940->tl= tds; _temp940;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp945; struct Cyc_Absyn_Tunionfield*
_temp946; struct Cyc_Absyn_Tuniondecl* _temp948; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp943= d; _temp945=* _temp943; _LL949: _temp948= _temp945.base; goto _LL947;
_LL947: _temp946= _temp945.field; goto _LL944; _LL944: _temp948=({ struct Cyc_Absyn_Tuniondecl*
_temp950=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp950[ 0]=* _temp948; _temp950;}); _temp946= Cyc_Interface_rewrite_tunionfield(
_temp946);( void*)( _temp946->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp948->fields=({
struct Cyc_Core_Opt* _temp951=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp951->v=( void*)({ struct Cyc_List_List* _temp952=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp952->hd=( void*)
_temp946; _temp952->tl= 0; _temp952;}); _temp951;});( void*)( _temp948->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp953=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp953->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp954=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp954[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp955; _temp955.tag= Cyc_Absyn_Tunion_d;
_temp955.f1= _temp948; _temp955;}); _temp954;}), 0); _temp953->tl= tds; _temp953;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp956= 0; goto _LL957; _LL957: _temp956=(( struct Cyc_List_List*(*)( struct
Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct
Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)(
Cyc_Interface_add_structdecl_header, ie->structdecls, _temp956); _temp956=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_arr*,
struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp956); _temp956=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp956); _temp956=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp956); if( _temp956 != 0){ _temp956=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp956); _temp956= Cyc_Interface_add_namespace(
_temp956, ns); Cyc_Absynpp_decllist2file( _temp956, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp958= 0; goto _LL959; _LL959: _temp958=(( struct
Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp958); if( _temp958 != 0){ _temp958=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp958); _temp958= Cyc_Interface_add_namespace(
_temp958, ns); Cyc_Absynpp_decllist2file( _temp958, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp960= 0; goto _LL961; _LL961: _temp960=(( struct
Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp960); if(
_temp960 != 0){ _temp960=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp960); _temp960= Cyc_Interface_add_namespace( _temp960,
ns); Cyc_Absynpp_decllist2file( _temp960, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp962= 0; goto _LL963; _LL963: _temp962=(( struct
Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp962);
_temp962=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp962); _temp962=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp962); _temp962=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp962); _temp962=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp962); _temp962=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp962); if( _temp962 != 0){ _temp962=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp962); _temp962= Cyc_Interface_add_namespace(
_temp962, ns); Cyc_Absynpp_decllist2file( _temp962, f);}} void Cyc_Interface_print(
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp964= Cyc_Parse_parse_file( f);
goto _LL965; _LL965: { struct Cyc_Tcenv_Tenv* _temp966= Cyc_Tcenv_tc_init();
goto _LL967; _LL967: Cyc_Tc_tc( _temp966, 0, _temp964); return Cyc_Interface_gen_extract(
_temp966->ae, 0);}}} void Cyc_Interface_save( struct Cyc_Interface_I* i, struct
Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i, f);} struct Cyc_Interface_I* Cyc_Interface_load(
struct Cyc_Stdio___sFILE* f){ return Cyc_Interface_parse( f);}