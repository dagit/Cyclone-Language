 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
} ; extern int Cyc_String_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr*
s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_function_type( void* t); extern unsigned char
Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl*
base; struct Cyc_Absyn_Tunionfield* field; } ; extern void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg0, struct _tagged_arr
msg1); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct
Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
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
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I* Cyc_Interface_empty(){
return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
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
_temp112; void* _temp114; void* _temp116; struct Cyc_Core_Opt* _temp118; struct
Cyc_Core_Opt _temp120; void* _temp121; struct Cyc_Absyn_FnInfo _temp123; struct
Cyc_Absyn_VarargInfo* _temp125; struct Cyc_List_List* _temp127; void* _temp129;
struct Cyc_List_List* _temp131; struct Cyc_Absyn_Structdecl** _temp133; struct
Cyc_Absyn_Structdecl* _temp135; struct Cyc_List_List* _temp136; struct Cyc_Absyn_Uniondecl**
_temp138; struct Cyc_Absyn_Uniondecl* _temp140; struct Cyc_List_List* _temp141;
struct _tuple0* _temp143; struct Cyc_Absyn_TunionInfo _temp145; struct Cyc_List_List*
_temp147; void* _temp149; struct Cyc_Absyn_Tuniondecl* _temp151; struct Cyc_Absyn_TunionFieldInfo
_temp153; struct Cyc_List_List* _temp155; void* _temp157; struct Cyc_Absyn_Tunionfield*
_temp159; struct Cyc_Absyn_Tuniondecl* _temp161; _LL90: if(( unsigned int)
_temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_PointerType: 0){ _LL113: _temp112=((
struct Cyc_Absyn_PointerType_struct*) _temp88)->f1; _LL115: _temp114=( void*)
_temp112.elt_typ; goto _LL91;} else{ goto _LL92;} _LL92: if(( unsigned int)
_temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_ArrayType: 0){ _LL117: _temp116=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp88)->f1; goto _LL93;} else{
goto _LL94;} _LL94: if(( unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_TypedefType:
0){ _LL119: _temp118=(( struct Cyc_Absyn_TypedefType_struct*) _temp88)->f3; if(
_temp118 == 0){ goto _LL96;} else{ _temp120=* _temp118; _LL122: _temp121=( void*)
_temp120.v; goto _LL95;}} else{ goto _LL96;} _LL96: if(( unsigned int) _temp88 >
4u?*(( int*) _temp88) == Cyc_Absyn_FnType: 0){ _LL124: _temp123=(( struct Cyc_Absyn_FnType_struct*)
_temp88)->f1; _LL130: _temp129=( void*) _temp123.ret_typ; goto _LL128; _LL128:
_temp127= _temp123.args; goto _LL126; _LL126: _temp125= _temp123.cyc_varargs;
goto _LL97;} else{ goto _LL98;} _LL98: if(( unsigned int) _temp88 > 4u?*(( int*)
_temp88) == Cyc_Absyn_TupleType: 0){ _LL132: _temp131=(( struct Cyc_Absyn_TupleType_struct*)
_temp88)->f1; goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int)
_temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_StructType: 0){ _LL137: _temp136=((
struct Cyc_Absyn_StructType_struct*) _temp88)->f2; goto _LL134; _LL134: _temp133=((
struct Cyc_Absyn_StructType_struct*) _temp88)->f3; if( _temp133 == 0){ goto
_LL102;} else{ _temp135=* _temp133; goto _LL101;}} else{ goto _LL102;} _LL102:
if(( unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_UnionType: 0){
_LL142: _temp141=(( struct Cyc_Absyn_UnionType_struct*) _temp88)->f2; goto
_LL139; _LL139: _temp138=(( struct Cyc_Absyn_UnionType_struct*) _temp88)->f3;
if( _temp138 == 0){ goto _LL104;} else{ _temp140=* _temp138; goto _LL103;}}
else{ goto _LL104;} _LL104: if(( unsigned int) _temp88 > 4u?*(( int*) _temp88)
== Cyc_Absyn_EnumType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_EnumType_struct*)
_temp88)->f1; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_TunionType: 0){ _LL146: _temp145=((
struct Cyc_Absyn_TunionType_struct*) _temp88)->f1; _LL150: _temp149=( void*)
_temp145.tunion_info; if(*(( int*) _temp149) == Cyc_Absyn_KnownTunion){ _LL152:
_temp151=(( struct Cyc_Absyn_KnownTunion_struct*) _temp149)->f1; goto _LL148;}
else{ goto _LL108;} _LL148: _temp147= _temp145.targs; goto _LL107;} else{ goto
_LL108;} _LL108: if(( unsigned int) _temp88 > 4u?*(( int*) _temp88) == Cyc_Absyn_TunionFieldType:
0){ _LL154: _temp153=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp88)->f1;
_LL158: _temp157=( void*) _temp153.field_info; if(*(( int*) _temp157) == Cyc_Absyn_KnownTunionfield){
_LL162: _temp161=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp157)->f1;
goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp157)->f2; goto _LL156;} else{ goto _LL110;} _LL156: _temp155= _temp153.targs;
goto _LL109;} else{ goto _LL110;} _LL110: goto _LL111; _LL91: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp114); _LL93: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp116); _LL95: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp121); _LL97: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp127)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp129): 0; if( _temp125 != 0){ void* _temp165; struct
Cyc_Absyn_VarargInfo _temp163=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp125)); _LL166: _temp165=( void*) _temp163.type; goto _LL164; _LL164: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp165);} return b;} _LL99: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp131); _LL101: if(( void*)
_temp135->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp135->name))->v); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp136)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp135): 0; _LL103: if(( void*) _temp140->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp140->name))->v);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp141)? Cyc_Interface_check_public_uniondecl( ae, seen, _temp140): 0; _LL105: {
struct _tuple0 _temp169; struct _tagged_arr* _temp170; void* _temp172; struct
_tuple0* _temp167= _temp143; _temp169=* _temp167; _LL173: _temp172= _temp169.f1;
goto _LL171; _LL171: _temp170= _temp169.f2; goto _LL168; _LL168: { struct Cyc_List_List*
_temp174= Cyc_Interface_get_abs_ns( _temp172); struct Cyc_Absyn_Enumdecl* ed;{
struct _handler_cons _temp175; _push_handler(& _temp175);{ int _temp177= 0; if(
setjmp( _temp175.handler)){ _temp177= 1;} if( ! _temp177){{ struct Cyc_Tcenv_Genv*
_temp178=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp174); ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp178->enumdecls,
_temp170);}; _pop_handler();} else{ void* _temp176=( void*) _exn_thrown; void*
_temp180= _temp176; _LL182: if( _temp180 == Cyc_Dict_Absent){ goto _LL183;}
else{ goto _LL184;} _LL184: goto _LL185; _LL183:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp186=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp186[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp187; _temp187.tag= Cyc_Core_InvalidArg; _temp187.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp188= Cyc_Absynpp_qvar2string(
_temp143); xprintf("check_public_type (can't find enum %.*s)", _get_arr_size(
_temp188, 1u), _temp188.curr);}); _temp187;}); _temp186;})); _LL185:( void)
_throw( _temp180); _LL181:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp143, _tag_arr("enum", sizeof( unsigned char),
5u), ed->name); return 0;} return 1;}} _LL107: { struct _tuple0 _temp191; struct
_tagged_arr* _temp192; void* _temp194; struct _tuple0* _temp189= _temp151->name;
_temp191=* _temp189; _LL195: _temp194= _temp191.f1; goto _LL193; _LL193:
_temp192= _temp191.f2; goto _LL190; _LL190: { struct Cyc_List_List* _temp196=
Cyc_Interface_get_abs_ns( _temp194); struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp197; _push_handler(& _temp197);{ int _temp199= 0; if( setjmp(
_temp197.handler)){ _temp199= 1;} if( ! _temp199){{ struct Cyc_Tcenv_Genv*
_temp200=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp196); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp200->tuniondecls,
_temp192);}; _pop_handler();} else{ void* _temp198=( void*) _exn_thrown; void*
_temp202= _temp198; _LL204: if( _temp202 == Cyc_Dict_Absent){ goto _LL205;}
else{ goto _LL206;} _LL206: goto _LL207; _LL205:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp208=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp208[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp209; _temp209.tag= Cyc_Core_InvalidArg; _temp209.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp210= Cyc_Absynpp_qvar2string(
_temp151->name); xprintf("check_public_type (can't find [x]tunion %.*s)",
_get_arr_size( _temp210, 1u), _temp210.curr);}); _temp209;}); _temp208;}));
_LL207:( void) _throw( _temp202); _LL203:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("[x]tunion", sizeof( unsigned char),
10u), tud->name); return 0;} return Cyc_Interface_check_public_type_list( ae,
seen, obj, name, Cyc_Core_identity, _temp147)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}} _LL109: { struct _tuple0 _temp213; struct _tagged_arr*
_temp214; void* _temp216; struct _tuple0* _temp211= _temp161->name; _temp213=*
_temp211; _LL217: _temp216= _temp213.f1; goto _LL215; _LL215: _temp214= _temp213.f2;
goto _LL212; _LL212: { struct Cyc_List_List* _temp218= Cyc_Interface_get_abs_ns(
_temp216); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp219;
_push_handler(& _temp219);{ int _temp221= 0; if( setjmp( _temp219.handler)){
_temp221= 1;} if( ! _temp221){{ struct Cyc_Tcenv_Genv* _temp222=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae,
_temp218); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp222->tuniondecls, _temp214);};
_pop_handler();} else{ void* _temp220=( void*) _exn_thrown; void* _temp224=
_temp220; _LL226: if( _temp224 == Cyc_Dict_Absent){ goto _LL227;} else{ goto
_LL228;} _LL228: goto _LL229; _LL227:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp230=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp230[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp231; _temp231.tag= Cyc_Core_InvalidArg;
_temp231.f1=( struct _tagged_arr)({ struct _tagged_arr _temp232= Cyc_Absynpp_qvar2string(
_temp161->name); xprintf("check_public_type (can't find [x]tunion %.*s and search its fields)",
_get_arr_size( _temp232, 1u), _temp232.curr);}); _temp231;}); _temp230;}));
_LL229:( void) _throw( _temp224); _LL225:;}}} if( tud->fields == 0){( int)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp233=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp233[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp234; _temp234.tag= Cyc_Core_InvalidArg; _temp234.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp235= Cyc_Absynpp_qvar2string( tud->name);
xprintf("check_public_type ([x]tunion %.*s has no fields)", _get_arr_size(
_temp235, 1u), _temp235.curr);}); _temp234;}); _temp233;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp236=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; for( 0; _temp236 != 0; _temp236=(( struct Cyc_List_List*)
_check_null( _temp236))->tl){ if( Cyc_String_strptrcmp((* _temp159->name).f2,(*((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp236))->hd)->name).f2)
== 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp236))->hd); break;}}} if( tuf1 == 0){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp237=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp237[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp238; _temp238.tag= Cyc_Core_InvalidArg; _temp238.f1=(
struct _tagged_arr)({ struct _tagged_arr _temp239= Cyc_Absynpp_qvar2string(
_temp159->name); xprintf("check_public_type (can't find [x]tunionfield %.*s)",
_get_arr_size( _temp239, 1u), _temp239.curr);}); _temp238;}); _temp237;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*) _check_null(
tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct _tagged_arr _temp240= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _get_arr_size( _temp240, 1u), _temp240.curr);}),
tud->name); return 0;} return Cyc_Interface_check_public_type_list( ae, seen,
obj, name, Cyc_Core_identity, _temp155)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}} _LL111: return 1; _LL89:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp243; struct Cyc_Interface_Seen* _temp244; struct Cyc_Dict_Dict*
_temp246; int _temp248; struct Cyc_Interface_Ienv* _temp250; struct Cyc_Interface_Ienv*
_temp252; struct _tuple11* _temp241= env; _temp243=* _temp241; _LL253: _temp252=
_temp243.f1; goto _LL251; _LL251: _temp250= _temp243.f2; goto _LL249; _LL249:
_temp248= _temp243.f3; goto _LL247; _LL247: _temp246= _temp243.f4; goto _LL245;
_LL245: _temp244= _temp243.f5; goto _LL242; _LL242: { struct Cyc_Absyn_Structdecl*
_temp254=* dp; void* _temp255=( void*) _temp254->sc; _LL257: if( _temp255 ==(
void*) Cyc_Absyn_Static){ goto _LL258;} else{ goto _LL259;} _LL259: if( _temp255
==( void*) Cyc_Absyn_Abstract){ goto _LL260;} else{ goto _LL261;} _LL261: if(
_temp255 ==( void*) Cyc_Absyn_Public){ goto _LL262;} else{ goto _LL263;} _LL263:
if( _temp255 ==( void*) Cyc_Absyn_ExternC){ goto _LL264;} else{ goto _LL265;}
_LL265: if( _temp255 ==( void*) Cyc_Absyn_Extern){ goto _LL266;} else{ goto
_LL256;} _LL258: if( _temp248? _temp254->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp254->name))->v);} goto _LL256; _LL260: if( _temp254->fields == 0){ if(
_temp248){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp254->name))->v);}} else{ _temp254=({ struct Cyc_Absyn_Structdecl*
_temp267=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp267[ 0]=* _temp254; _temp267;}); _temp254->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp246, _temp244, _temp254)){ _temp250->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp250->structdecls, x, _temp254);} goto _LL256;
_LL262: if( _temp254->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp254->name))->v);
_temp254=({ struct Cyc_Absyn_Structdecl* _temp268=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp268[ 0]=* _temp254;
_temp268;});( void*)( _temp254->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp246, _temp244, _temp254)){ _temp250->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp250->structdecls, x,
_temp254);} goto _LL256; _LL264: goto _LL266; _LL266: if( Cyc_Interface_check_public_structdecl(
_temp246, _temp244, _temp254)){ _temp252->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp252->structdecls, x, _temp254);} goto _LL256;
_LL256:;}} static void Cyc_Interface_extract_uniondecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11
_temp271; struct Cyc_Interface_Seen* _temp272; struct Cyc_Dict_Dict* _temp274;
int _temp276; struct Cyc_Interface_Ienv* _temp278; struct Cyc_Interface_Ienv*
_temp280; struct _tuple11* _temp269= env; _temp271=* _temp269; _LL281: _temp280=
_temp271.f1; goto _LL279; _LL279: _temp278= _temp271.f2; goto _LL277; _LL277:
_temp276= _temp271.f3; goto _LL275; _LL275: _temp274= _temp271.f4; goto _LL273;
_LL273: _temp272= _temp271.f5; goto _LL270; _LL270: { struct Cyc_Absyn_Uniondecl*
_temp282=* dp; void* _temp283=( void*) _temp282->sc; _LL285: if( _temp283 ==(
void*) Cyc_Absyn_Static){ goto _LL286;} else{ goto _LL287;} _LL287: if( _temp283
==( void*) Cyc_Absyn_Abstract){ goto _LL288;} else{ goto _LL289;} _LL289: if(
_temp283 ==( void*) Cyc_Absyn_Public){ goto _LL290;} else{ goto _LL291;} _LL291:
if( _temp283 ==( void*) Cyc_Absyn_ExternC){ goto _LL292;} else{ goto _LL293;}
_LL293: if( _temp283 ==( void*) Cyc_Absyn_Extern){ goto _LL294;} else{ goto
_LL284;} _LL286: if( _temp276? _temp282->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp282->name))->v);} goto _LL284; _LL288: if( _temp282->fields == 0){ if(
_temp276){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp282->name))->v);}} else{ _temp282=({ struct Cyc_Absyn_Uniondecl*
_temp295=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp295[ 0]=* _temp282; _temp295;}); _temp282->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp274, _temp272, _temp282)){ _temp278->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp278->uniondecls, x, _temp282);} goto _LL284;
_LL290: if( _temp282->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp282->name))->v);
_temp282=({ struct Cyc_Absyn_Uniondecl* _temp296=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp296[ 0]=* _temp282;
_temp296;});( void*)( _temp282->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp274, _temp272, _temp282)){ _temp278->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp278->uniondecls, x,
_temp282);} goto _LL284; _LL292: goto _LL294; _LL294: if( Cyc_Interface_check_public_uniondecl(
_temp274, _temp272, _temp282)){ _temp280->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp280->uniondecls, x, _temp282);} goto _LL284;
_LL284:;}} static void Cyc_Interface_extract_enumdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple11 _temp299;
struct Cyc_Interface_Seen* _temp300; struct Cyc_Dict_Dict* _temp302; int
_temp304; struct Cyc_Interface_Ienv* _temp306; struct Cyc_Interface_Ienv*
_temp308; struct _tuple11* _temp297= env; _temp299=* _temp297; _LL309: _temp308=
_temp299.f1; goto _LL307; _LL307: _temp306= _temp299.f2; goto _LL305; _LL305:
_temp304= _temp299.f3; goto _LL303; _LL303: _temp302= _temp299.f4; goto _LL301;
_LL301: _temp300= _temp299.f5; goto _LL298; _LL298: { struct Cyc_Absyn_Enumdecl*
_temp310=* dp; void* _temp311=( void*) _temp310->sc; _LL313: if( _temp311 ==(
void*) Cyc_Absyn_Static){ goto _LL314;} else{ goto _LL315;} _LL315: if( _temp311
==( void*) Cyc_Absyn_Abstract){ goto _LL316;} else{ goto _LL317;} _LL317: if(
_temp311 ==( void*) Cyc_Absyn_Public){ goto _LL318;} else{ goto _LL319;} _LL319:
if( _temp311 ==( void*) Cyc_Absyn_ExternC){ goto _LL320;} else{ goto _LL321;}
_LL321: if( _temp311 ==( void*) Cyc_Absyn_Extern){ goto _LL322;} else{ goto
_LL312;} _LL314: if( _temp304? _temp310->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp310->name);} goto _LL312; _LL316: if( _temp310->fields
== 0){ if( _temp304){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp310->name);}}
else{ _temp310=({ struct Cyc_Absyn_Enumdecl* _temp323=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp323[ 0]=* _temp310;
_temp323;}); _temp310->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp302, _temp300, _temp310)){ _temp306->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp306->enumdecls, x, _temp310);} goto _LL312; _LL318:
if( _temp310->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp310->name);
_temp310=({ struct Cyc_Absyn_Enumdecl* _temp324=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp324[ 0]=* _temp310;
_temp324;});( void*)( _temp310->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp302, _temp300, _temp310)){ _temp306->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp306->enumdecls, x,
_temp310);} goto _LL312; _LL320: goto _LL322; _LL322: if( Cyc_Interface_check_public_enumdecl(
_temp302, _temp300, _temp310)){ _temp308->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp308->enumdecls, x, _temp310);} goto _LL312; _LL312:;}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp325= 0;{ void* _temp326=(* f->name).f1; struct Cyc_List_List* _temp334;
struct Cyc_List_List* _temp336; _LL328: if(( unsigned int) _temp326 > 1u?*(( int*)
_temp326) == Cyc_Absyn_Rel_n: 0){ _LL335: _temp334=(( struct Cyc_Absyn_Rel_n_struct*)
_temp326)->f1; goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int)
_temp326 > 1u?*(( int*) _temp326) == Cyc_Absyn_Abs_n: 0){ _LL337: _temp336=((
struct Cyc_Absyn_Abs_n_struct*) _temp326)->f1; goto _LL331;} else{ goto _LL332;}
_LL332: goto _LL333; _LL329: _temp336= _temp334; goto _LL331; _LL331: _temp325=
_temp336; goto _LL327; _LL333: goto _LL327; _LL327:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp338=( void*) f->sc; _LL340: if( _temp338 ==( void*) Cyc_Absyn_Static){
goto _LL341;} else{ goto _LL342;} _LL342: if( _temp338 ==( void*) Cyc_Absyn_Extern){
goto _LL343;} else{ goto _LL344;} _LL344: if( _temp338 ==( void*) Cyc_Absyn_Public){
goto _LL345;} else{ goto _LL346;} _LL346: goto _LL347; _LL341: return; _LL343:
dict=& i->imports; goto _LL339; _LL345: dict=& i->exports; goto _LL339; _LL347:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp348=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp348[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp349; _temp349.tag= Cyc_Core_InvalidArg; _temp349.f1=
_tag_arr("add_xtunionfielddecl", sizeof( unsigned char), 21u); _temp349;});
_temp348;})); _LL339:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons
_temp350; _push_handler(& _temp350);{ int _temp352= 0; if( setjmp( _temp350.handler)){
_temp352= 1;} if( ! _temp352){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp325);; _pop_handler();}
else{ void* _temp351=( void*) _exn_thrown; void* _temp354= _temp351; _LL356: if(
_temp354 == Cyc_Dict_Absent){ goto _LL357;} else{ goto _LL358;} _LL358: goto
_LL359; _LL357: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp325, env); goto _LL355; _LL359:( void)
_throw( _temp354); _LL355:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp360=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp360->base= d; _temp360->field= f; _temp360;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp363; struct Cyc_Interface_I* _temp364; struct Cyc_Interface_Seen*
_temp366; struct Cyc_Dict_Dict* _temp368; int _temp370; struct Cyc_Interface_Ienv*
_temp372; struct Cyc_Interface_Ienv* _temp374; struct _tuple11* _temp361= env;
_temp363=* _temp361; _LL375: _temp374= _temp363.f1; goto _LL373; _LL373:
_temp372= _temp363.f2; goto _LL371; _LL371: _temp370= _temp363.f3; goto _LL369;
_LL369: _temp368= _temp363.f4; goto _LL367; _LL367: _temp366= _temp363.f5; goto
_LL365; _LL365: _temp364= _temp363.f6; goto _LL362; _LL362: { struct Cyc_Absyn_Tuniondecl*
_temp376=* dp; void* _temp377=( void*) _temp376->sc; _LL379: if( _temp377 ==(
void*) Cyc_Absyn_Static){ goto _LL380;} else{ goto _LL381;} _LL381: if( _temp377
==( void*) Cyc_Absyn_Abstract){ goto _LL382;} else{ goto _LL383;} _LL383: if(
_temp377 ==( void*) Cyc_Absyn_Public){ goto _LL384;} else{ goto _LL385;} _LL385:
if( _temp377 ==( void*) Cyc_Absyn_ExternC){ goto _LL386;} else{ goto _LL387;}
_LL387: if( _temp377 ==( void*) Cyc_Absyn_Extern){ goto _LL388;} else{ goto
_LL378;} _LL380: if(( ! _temp376->is_xtunion? _temp370: 0)? _temp376->fields ==
0: 0){ Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp376->name);} goto _LL378;
_LL382: if( _temp376->fields == 0){ if( ! _temp376->is_xtunion? _temp370: 0){
Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char), 9u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp376->name);}} else{
_temp376=({ struct Cyc_Absyn_Tuniondecl* _temp389=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp389[ 0]=* _temp376;
_temp389;}); _temp376->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp368, _temp366, _temp376)){ _temp372->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp372->tuniondecls, x, _temp376);} goto _LL378;
_LL384: _temp376=({ struct Cyc_Absyn_Tuniondecl* _temp390=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp390[ 0]=* _temp376;
_temp390;}); if( ! _temp376->is_xtunion? _temp376->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp376->name);( void*)( _temp376->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp368,
_temp366, _temp376)){ if( _temp376->is_xtunion? _temp376->fields != 0: 0){
struct Cyc_List_List* _temp391=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp376->fields))->v; _temp376->fields= 0;{ struct Cyc_List_List*
_temp392= _temp391; for( 0; _temp392 != 0; _temp392=(( struct Cyc_List_List*)
_check_null( _temp392))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp364,
_temp376,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp392))->hd);}}} _temp372->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp372->tuniondecls, x, _temp376);} goto _LL378; _LL386:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp393=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp393[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp394; _temp394.tag= Cyc_Core_InvalidArg; _temp394.f1=
_tag_arr("extract_tuniondecl", sizeof( unsigned char), 19u); _temp394;});
_temp393;})); _LL388: if( Cyc_Interface_check_public_tuniondecl( _temp368,
_temp366, _temp376)){ if( _temp376->is_xtunion? _temp376->fields != 0: 0){
_temp376=({ struct Cyc_Absyn_Tuniondecl* _temp395=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp395[ 0]=* _temp376;
_temp395;});{ struct Cyc_List_List* _temp396=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp376->fields))->v; _temp376->fields= 0;{ struct
Cyc_List_List* _temp397= _temp396; for( 0; _temp397 != 0; _temp397=(( struct Cyc_List_List*)
_check_null( _temp397))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp364,
_temp376,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp397))->hd);}}}} _temp374->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp374->tuniondecls, x, _temp376);} goto _LL378; _LL378:;}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp400;
struct Cyc_Interface_Seen* _temp401; struct Cyc_Dict_Dict* _temp403; struct Cyc_Interface_Ienv*
_temp405; struct _tuple11* _temp398= env; _temp400=* _temp398; _LL406: _temp405=
_temp400.f2; goto _LL404; _LL404: _temp403= _temp400.f4; goto _LL402; _LL402:
_temp401= _temp400.f5; goto _LL399; _LL399: if( Cyc_Interface_check_public_typedefdecl(
_temp403, _temp401, d)){ _temp405->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp405->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_arr* x, struct _tuple7* v){ struct _tuple11
_temp409; struct Cyc_Interface_Seen* _temp410; struct Cyc_Dict_Dict* _temp412;
int _temp414; struct Cyc_Interface_Ienv* _temp416; struct Cyc_Interface_Ienv*
_temp418; struct _tuple11* _temp407= env; _temp409=* _temp407; _LL419: _temp418=
_temp409.f1; goto _LL417; _LL417: _temp416= _temp409.f2; goto _LL415; _LL415:
_temp414= _temp409.f3; goto _LL413; _LL413: _temp412= _temp409.f4; goto _LL411;
_LL411: _temp410= _temp409.f5; goto _LL408; _LL408: { void* _temp420=(* v).f1;
void* _temp421= _temp420; void* _temp427; _LL423: if(*(( int*) _temp421) == Cyc_Tcenv_VarRes){
_LL428: _temp427=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp421)->f1; goto
_LL424;} else{ goto _LL425;} _LL425: goto _LL426; _LL424:{ void* _temp429=
_temp427; struct Cyc_Absyn_Fndecl* _temp437; struct Cyc_Absyn_Vardecl* _temp439;
_LL431: if(( unsigned int) _temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Funname_b:
0){ _LL438: _temp437=(( struct Cyc_Absyn_Funname_b_struct*) _temp429)->f1; goto
_LL432;} else{ goto _LL433;} _LL433: if(( unsigned int) _temp429 > 1u?*(( int*)
_temp429) == Cyc_Absyn_Global_b: 0){ _LL440: _temp439=(( struct Cyc_Absyn_Global_b_struct*)
_temp429)->f1; goto _LL434;} else{ goto _LL435;} _LL435: goto _LL436; _LL432: {
struct Cyc_Absyn_Vardecl* _temp441=({ struct Cyc_Absyn_Vardecl* _temp442=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp442->sc=( void*)(( void*) _temp437->sc); _temp442->name= _temp437->name;
_temp442->tq=({ struct Cyc_Absyn_Tqual _temp443; _temp443.q_const= 0; _temp443.q_volatile=
0; _temp443.q_restrict= 0; _temp443;}); _temp442->type=( void*)(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp437->cached_typ))->v); _temp442->initializer=
0; _temp442->rgn= 0; _temp442->attributes= 0; _temp442;}); _temp414= 0; _temp439=
_temp441; goto _LL434;} _LL434: if( _temp439->initializer != 0){ _temp439=({
struct Cyc_Absyn_Vardecl* _temp444=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp444[ 0]=* _temp439; _temp444;});
_temp439->initializer= 0;}{ void* _temp445=( void*) _temp439->sc; _LL447: if(
_temp445 ==( void*) Cyc_Absyn_Static){ goto _LL448;} else{ goto _LL449;} _LL449:
if( _temp445 ==( void*) Cyc_Absyn_Abstract){ goto _LL450;} else{ goto _LL451;}
_LL451: if( _temp445 ==( void*) Cyc_Absyn_Public){ goto _LL452;} else{ goto
_LL453;} _LL453: if( _temp445 ==( void*) Cyc_Absyn_ExternC){ goto _LL454;} else{
goto _LL455;} _LL455: if( _temp445 ==( void*) Cyc_Absyn_Extern){ goto _LL456;}
else{ goto _LL446;} _LL448: if( _temp414? Cyc_Tcutil_is_function_type(( void*)
_temp439->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp439->name);} goto _LL446; _LL450:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp457=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp457[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp458; _temp458.tag= Cyc_Core_InvalidArg; _temp458.f1=
_tag_arr("extract_ordinarie", sizeof( unsigned char), 18u); _temp458;});
_temp457;})); _LL452: if( _temp414? Cyc_Tcutil_is_function_type(( void*)
_temp439->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp439->name);} if( Cyc_Interface_check_public_vardecl(
_temp412, _temp410, _temp439)){ _temp416->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data))
Cyc_Dict_insert)( _temp416->vardecls, x, _temp439);} goto _LL446; _LL454: goto
_LL456; _LL456: if( Cyc_Interface_check_public_vardecl( _temp412, _temp410,
_temp439)){ _temp418->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp418->vardecls, x, _temp439);} goto _LL446; _LL446:;} goto _LL430; _LL436:
goto _LL430; _LL430:;} goto _LL422; _LL426: goto _LL422; _LL422:;}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp461; struct Cyc_Interface_Seen* _temp462; struct Cyc_Dict_Dict* _temp464;
int _temp466; struct Cyc_Interface_I* _temp468; struct _tuple12* _temp459= env_f;
_temp461=* _temp459; _LL469: _temp468= _temp461.f1; goto _LL467; _LL467:
_temp466= _temp461.f2; goto _LL465; _LL465: _temp464= _temp461.f3; goto _LL463;
_LL463: _temp462= _temp461.f4; goto _LL460; _LL460: { struct _tuple11 _temp470=({
struct _tuple11 _temp471; _temp471.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp468->imports,
ns); _temp471.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp468->exports, ns); _temp471.f3=
_temp466; _temp471.f4= _temp464; _temp471.f5= _temp462; _temp471.f6= _temp468;
_temp471;});(( void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct
Cyc_Absyn_Structdecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_structdecl,& _temp470, ge->structdecls);(( void(*)( void(*
f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp470, ge->uniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp470, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp470, ge->enumdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp470, ge->typedefs);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct _tuple7*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp470, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple12 _temp472=({
struct _tuple12 _temp473; _temp473.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp473.f2= check_complete_defs; _temp473.f3=
ae; _temp473.f4= Cyc_Interface_new_seen(); _temp473;});(( void(*)( void(* f)(
struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp472, ae); return _temp472.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_arr*); struct
_tagged_arr f4; struct _tagged_arr* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_arr* x, void* y1){ struct _tuple13 _temp476;
struct _tagged_arr* _temp477; struct _tagged_arr _temp479; int(* _temp481)( void*,
void*, struct _tagged_arr*); struct Cyc_Dict_Dict* _temp483; int _temp485; int*
_temp487; struct _tuple13* _temp474= env; _temp476=* _temp474; _LL486: _temp485=
_temp476.f1; _temp487=&(* _temp474).f1; goto _LL484; _LL484: _temp483= _temp476.f2;
goto _LL482; _LL482: _temp481= _temp476.f3; goto _LL480; _LL480: _temp479=
_temp476.f4; goto _LL478; _LL478: _temp477= _temp476.f5; goto _LL475; _LL475: {
struct _handler_cons _temp488; _push_handler(& _temp488);{ int _temp490= 0; if(
setjmp( _temp488.handler)){ _temp490= 1;} if( ! _temp490){{ void* _temp491=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp483, x); if( ! _temp481( y1, _temp491, _temp477)){* _temp487= 0;}};
_pop_handler();} else{ void* _temp489=( void*) _exn_thrown; void* _temp493=
_temp489; _LL495: if( _temp493 == Cyc_Dict_Absent){ goto _LL496;} else{ goto
_LL497;} _LL497: goto _LL498; _LL496: Cyc_Interface_check_err( _temp477,( struct
_tagged_arr)({ struct _tagged_arr _temp499= _temp479; struct _tagged_arr
_temp500=* x; xprintf("%.*s %.*s is missing", _get_arr_size( _temp499, 1u),
_temp499.curr, _get_arr_size( _temp500, 1u), _temp500.curr);}));* _temp487= 0;
goto _LL494; _LL498:( void) _throw( _temp493); _LL494:;}}}} static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( void*,
void*, struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){
struct _tuple13 _temp501=({ struct _tuple13 _temp502; _temp502.f1= 1; _temp502.f2=
dic2; _temp502.f3= incl_f; _temp502.f4= t; _temp502.f5= msg; _temp502;});(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_arr*, void*), struct _tuple13* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_dict_f,& _temp501,
dic1); return _temp501.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Structdecl*
_temp503= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); if( _temp503 == 0){
return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp503){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr _temp504= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_get_arr_size( _temp504, 1u), _temp504.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Uniondecl* _temp505= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); if( _temp505 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*)
d0 != _temp505){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp506= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_get_arr_size( _temp506, 1u), _temp506.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl* _temp507= Cyc_Tcdecl_merge_tuniondecl(
d0, d1, 0, msg); if( _temp507 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*)
d0 != _temp507){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp508= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_get_arr_size( _temp508, 1u), _temp508.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl* _temp509= Cyc_Tcdecl_merge_enumdecl(
d0, d1, 0, msg); if( _temp509 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*)
d0 != _temp509){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp510= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of enum %.*s discloses too much information",
_get_arr_size( _temp510, 1u), _temp510.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl* _temp511= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); if( _temp511 == 0){ return 0;} if(( struct Cyc_Absyn_Vardecl*)
d0 != _temp511){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp512= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of variable %.*s discloses too much information",
_get_arr_size( _temp512, 1u), _temp512.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl* _temp513= Cyc_Tcdecl_merge_typedefdecl(
d0, d1, 0, msg); if( _temp513 == 0){ return 0;} if(( struct Cyc_Absyn_Typedefdecl*)
d0 != _temp513){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr _temp514= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_get_arr_size( _temp514, 1u), _temp514.curr);})); return 0;} return 1;} static
int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl* d0,
struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp515= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0, msg); if( _temp515 == 0){
return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp515){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr _temp516= Cyc_Absynpp_qvar2string((
d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_get_arr_size( _temp516, 1u), _temp516.curr);})); return 0;} return 1;} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_arr* msg){ int _temp517=((
int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->structdecls,
ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct", sizeof(
unsigned char), 7u), msg); int _temp518=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*, struct
Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
int _temp519=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl, _tag_arr("tunion",
sizeof( unsigned char), 7u), msg); int _temp520=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); int _temp521=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) != 0; int _temp522=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); int _temp523=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); return((((( _temp517? _temp518: 0)? _temp519: 0)?
_temp520: 0)? _temp521: 0)? _temp522: 0)? _temp523: 0;} struct _tuple14{ int f1;
struct Cyc_Dict_Dict* f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f(
struct _tuple14* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){
struct _tuple14 _temp526; struct _tagged_arr* _temp527; struct Cyc_Dict_Dict*
_temp529; int _temp531; int* _temp533; struct _tuple14* _temp524= env; _temp526=*
_temp524; _LL532: _temp531= _temp526.f1; _temp533=&(* _temp524).f1; goto _LL530;
_LL530: _temp529= _temp526.f2; goto _LL528; _LL528: _temp527= _temp526.f3; goto
_LL525; _LL525: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp534;
_push_handler(& _temp534);{ int _temp536= 0; if( setjmp( _temp534.handler)){
_temp536= 1;} if( ! _temp536){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp529, ns);; _pop_handler();}
else{ void* _temp535=( void*) _exn_thrown; void* _temp538= _temp535; _LL540: if(
_temp538 == Cyc_Dict_Absent){ goto _LL541;} else{ goto _LL542;} _LL542: goto
_LL543; _LL541: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL539; _LL543:( void)
_throw( _temp538); _LL539:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp527)){*
_temp533= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple14 _temp544=({
struct _tuple14 _temp545; _temp545.f1= 1; _temp545.f2= dic2; _temp545.f3= msg;
_temp545;});(( void(*)( void(* f)( struct _tuple14*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _temp544, dic1); return _temp544.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp546; _push_handler(& _temp546);{ int _temp548= 0; if(
setjmp( _temp546.handler)){ _temp548= 1;} if( ! _temp548){{ int _temp549= 1;
struct _tagged_arr* msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp550=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp550[ 0]=(
struct _tagged_arr)({ struct _tagged_arr _temp551=(*(( struct _tuple3*)
_check_null( info))).f1; struct _tagged_arr _temp552=(*(( struct _tuple3*)
_check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_get_arr_size( _temp551, 1u), _temp551.curr, _get_arr_size( _temp552, 1u),
_temp552.curr);}); _temp550;});} if( ! Cyc_Interface_incl_ns( i1->exports, i2->exports,
msg)){ _temp549= 0;} if( info != 0){ msg=({ struct _tagged_arr* _temp553=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp553[ 0]=(
struct _tagged_arr)({ struct _tagged_arr _temp554=(*(( struct _tuple3*)
_check_null( info))).f2; struct _tagged_arr _temp555=(*(( struct _tuple3*)
_check_null( info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_get_arr_size( _temp554, 1u), _temp554.curr, _get_arr_size( _temp555, 1u),
_temp555.curr);}); _temp553;});} if( ! Cyc_Interface_incl_ns( i2->imports, i1->imports,
msg)){ _temp549= 0;}{ int _temp556= _temp549; _npop_handler( 0u); return
_temp556;}}; _pop_handler();} else{ void* _temp547=( void*) _exn_thrown; void*
_temp558= _temp547; _LL560: if( _temp558 == Cyc_Tcdecl_Incompatible){ goto
_LL561;} else{ goto _LL562;} _LL562: goto _LL563; _LL561: return 0; _LL563:(
void) _throw( _temp558); _LL559:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct _tagged_arr
f6; struct _tagged_arr* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_arr* x, void** y2){ struct _tuple15 _temp566;
struct _tagged_arr* _temp567; struct _tagged_arr _temp569; void**(* _temp571)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct Cyc_Dict_Dict*
_temp573; struct Cyc_Dict_Dict* _temp575; struct Cyc_Dict_Dict* _temp577; struct
Cyc_Dict_Dict** _temp579; int _temp580; int* _temp582; struct _tuple15* _temp564=
env; _temp566=* _temp564; _LL581: _temp580= _temp566.f1; _temp582=&(* _temp564).f1;
goto _LL578; _LL578: _temp577= _temp566.f2; _temp579=&(* _temp564).f2; goto
_LL576; _LL576: _temp575= _temp566.f3; goto _LL574; _LL574: _temp573= _temp566.f4;
goto _LL572; _LL572: _temp571= _temp566.f5; goto _LL570; _LL570: _temp569=
_temp566.f6; goto _LL568; _LL568: _temp567= _temp566.f7; goto _LL565; _LL565: {
void** y;{ struct _handler_cons _temp583; _push_handler(& _temp583);{ int
_temp585= 0; if( setjmp( _temp583.handler)){ _temp585= 1;} if( ! _temp585){{
void** _temp586=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp575, x); void** _temp587= _temp571( _temp586, y2, 0,
_temp567); if( !(( unsigned int) _temp587)){* _temp582= 0; _npop_handler( 0u);
return;} y=( void**) _check_null( _temp587);}; _pop_handler();} else{ void*
_temp584=( void*) _exn_thrown; void* _temp589= _temp584; _LL591: if( _temp589 ==
Cyc_Dict_Absent){ goto _LL592;} else{ goto _LL593;} _LL593: goto _LL594; _LL592:
y= y2; goto _LL590; _LL594:( void) _throw( _temp589); _LL590:;}}}{ struct
_handler_cons _temp595; _push_handler(& _temp595);{ int _temp597= 0; if( setjmp(
_temp595.handler)){ _temp597= 1;} if( ! _temp597){{ void** _temp598=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp573, x);
void** _temp599= _temp571( _temp598, y, 0, _temp567); if( _temp599 !=( void**)
_temp598){ if(( unsigned int) _temp599){ Cyc_Interface_check_err( _temp567,(
struct _tagged_arr)({ struct _tagged_arr _temp600= _temp569; struct _tagged_arr
_temp601=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_get_arr_size( _temp600, 1u), _temp600.curr, _get_arr_size( _temp601, 1u),
_temp601.curr);}));}* _temp582= 0;}}; _pop_handler();} else{ void* _temp596=(
void*) _exn_thrown; void* _temp603= _temp596; _LL605: if( _temp603 == Cyc_Dict_Absent){
goto _LL606;} else{ goto _LL607;} _LL607: goto _LL608; _LL606: if(* _temp582){*
_temp579=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void** data)) Cyc_Dict_insert)(* _temp579, x, y);} goto _LL604;
_LL608:( void) _throw( _temp603); _LL604:;}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple15
_temp609=({ struct _tuple15 _temp611; _temp611.f1= 1; _temp611.f2= dic1;
_temp611.f3= dic1; _temp611.f4= excl; _temp611.f5= merge_f; _temp611.f6= t;
_temp611.f7= msg; _temp611;});(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_arr*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _temp609, dic2); if( _temp609.f1){ return({
struct Cyc_Core_Opt* _temp610=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp610->v=( void*) _temp609.f2; _temp610;});} else{ return 0;}}
struct Cyc_Interface_Ienv* Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct Cyc_Interface_Ienv* iexcl, struct
_tagged_arr* msg){ struct Cyc_Core_Opt* _temp612=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp613=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt*
_temp614=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp615=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp616=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef", sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt*
_temp617=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp618=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp612)? 1: !((
unsigned int) _temp613))? 1: !(( unsigned int) _temp614))? 1: !(( unsigned int)
_temp615))? 1: !(( unsigned int) _temp616))? 1: !(( unsigned int) _temp617))? 1:
!(( unsigned int) _temp618)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp619=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp619->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp612))->v; _temp619->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp613))->v; _temp619->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp614))->v; _temp619->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp615))->v;
_temp619->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp616))->v; _temp619->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp617))->v; _temp619->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp618))->v; _temp619;});}}
struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple16 _temp622; struct _tagged_arr* _temp623; struct Cyc_Dict_Dict*
_temp625; struct Cyc_Dict_Dict* _temp627; struct Cyc_Dict_Dict* _temp629; struct
Cyc_Dict_Dict** _temp631; int _temp632; int* _temp634; struct _tuple16* _temp620=
env; _temp622=* _temp620; _LL633: _temp632= _temp622.f1; _temp634=&(* _temp620).f1;
goto _LL630; _LL630: _temp629= _temp622.f2; _temp631=&(* _temp620).f2; goto
_LL628; _LL628: _temp627= _temp622.f3; goto _LL626; _LL626: _temp625= _temp622.f4;
goto _LL624; _LL624: _temp623= _temp622.f5; goto _LL621; _LL621: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp635;
_push_handler(& _temp635);{ int _temp637= 0; if( setjmp( _temp635.handler)){
_temp637= 1;} if( ! _temp637){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp625, ns);;
_pop_handler();} else{ void* _temp636=( void*) _exn_thrown; void* _temp639=
_temp636; _LL641: if( _temp639 == Cyc_Dict_Absent){ goto _LL642;} else{ goto
_LL643;} _LL643: goto _LL644; _LL642: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL640; _LL644:( void) _throw( _temp639); _LL640:;}}}{ struct _handler_cons
_temp645; _push_handler(& _temp645);{ int _temp647= 0; if( setjmp( _temp645.handler)){
_temp647= 1;} if( ! _temp647){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp627, ns);{ struct Cyc_Interface_Ienv*
_temp648= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp623); if( _temp648 == 0){* _temp634= 0; _npop_handler( 0u); return;}
ie1=( struct Cyc_Interface_Ienv*) _check_null( _temp648);}; _pop_handler();}
else{ void* _temp646=( void*) _exn_thrown; void* _temp650= _temp646; _LL652: if(
_temp650 == Cyc_Dict_Absent){ goto _LL653;} else{ goto _LL654;} _LL654: goto
_LL655; _LL653: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL651; _LL655:( void)
_throw( _temp650); _LL651:;}}}{ struct Cyc_Interface_Ienv* _temp656= Cyc_Interface_compat_merge_ienv(
ie1, ie2, iexcl, _temp623); if( _temp656 == 0){* _temp634= 0;} else{ if(*
_temp634){* _temp631=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp631, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp656));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_arr* msg){
struct _tuple16 _temp657=({ struct _tuple16 _temp659; _temp659.f1= 1; _temp659.f2=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp659.f3= dic1; _temp659.f4= excl;
_temp659.f5= msg; _temp659;});(( void(*)( void(* f)( struct _tuple16*, struct
Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,& _temp657, dic2); if(
_temp657.f1){ return({ struct Cyc_Core_Opt* _temp658=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp658->v=( void*) _temp657.f2;
_temp658;});} else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct _tagged_arr f4; struct _tagged_arr* f5; } ;
void Cyc_Interface_disj_merge_dict_f( struct _tuple17* env, struct _tagged_arr*
x, void* y){ struct _tuple17 _temp662; struct _tagged_arr* _temp663; struct
_tagged_arr _temp665; struct Cyc_Dict_Dict* _temp667; struct Cyc_Dict_Dict*
_temp669; struct Cyc_Dict_Dict** _temp671; int _temp672; int* _temp674; struct
_tuple17* _temp660= env; _temp662=* _temp660; _LL673: _temp672= _temp662.f1;
_temp674=&(* _temp660).f1; goto _LL670; _LL670: _temp669= _temp662.f2; _temp671=&(*
_temp660).f2; goto _LL668; _LL668: _temp667= _temp662.f3; goto _LL666; _LL666:
_temp665= _temp662.f4; goto _LL664; _LL664: _temp663= _temp662.f5; goto _LL661;
_LL661: if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
_temp667, x)){ Cyc_Interface_check_err( _temp663,( struct _tagged_arr)({ struct
_tagged_arr _temp675= _temp665; struct _tagged_arr _temp676=* x; xprintf("%.*s %.*s is exported more than once",
_get_arr_size( _temp675, 1u), _temp675.curr, _get_arr_size( _temp676, 1u),
_temp676.curr);}));* _temp674= 0;} else{ if(* _temp674){* _temp671=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(*
_temp671, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct
_tagged_arr* msg){ struct _tuple17 _temp677=({ struct _tuple17 _temp679;
_temp679.f1= 1; _temp679.f2= dic1; _temp679.f3= dic1; _temp679.f4= t; _temp679.f5=
msg; _temp679;});(( void(*)( void(* f)( struct _tuple17*, struct _tagged_arr*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_dict_f,&
_temp677, dic2); if( _temp677.f1){ return({ struct Cyc_Core_Opt* _temp678=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp678->v=(
void*) _temp677.f2; _temp678;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_arr* msg){ struct Cyc_Core_Opt* _temp680=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp681=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->uniondecls, ie2->uniondecls, _tag_arr("union", sizeof( unsigned char), 6u),
msg); struct Cyc_Core_Opt* _temp682=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), msg); struct Cyc_Core_Opt* _temp683=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp684=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg); struct Cyc_Core_Opt* _temp685=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt* _temp686=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof( unsigned char), 13u),
msg); if(((((( !(( unsigned int) _temp680)? 1: !(( unsigned int) _temp681))? 1:
!(( unsigned int) _temp682))? 1: !(( unsigned int) _temp683))? 1: !((
unsigned int) _temp684))? 1: !(( unsigned int) _temp685))? 1: !(( unsigned int)
_temp686)){ return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp687=(
struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp687->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp680))->v; _temp687->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp681))->v; _temp687->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp682))->v; _temp687->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp683))->v;
_temp687->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp684))->v; _temp687->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp685))->v; _temp687->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp686))->v; _temp687;});}}
struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct _tuple18*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple18
_temp690; struct _tagged_arr* _temp691; struct Cyc_Dict_Dict* _temp693; struct
Cyc_Dict_Dict* _temp695; struct Cyc_Dict_Dict** _temp697; int _temp698; int*
_temp700; struct _tuple18* _temp688= env; _temp690=* _temp688; _LL699: _temp698=
_temp690.f1; _temp700=&(* _temp688).f1; goto _LL696; _LL696: _temp695= _temp690.f2;
_temp697=&(* _temp688).f2; goto _LL694; _LL694: _temp693= _temp690.f3; goto
_LL692; _LL692: _temp691= _temp690.f4; goto _LL689; _LL689: { struct Cyc_Interface_Ienv*
ie1;{ struct _handler_cons _temp701; _push_handler(& _temp701);{ int _temp703= 0;
if( setjmp( _temp701.handler)){ _temp703= 1;} if( ! _temp703){ ie1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp693,
ns);; _pop_handler();} else{ void* _temp702=( void*) _exn_thrown; void* _temp705=
_temp702; _LL707: if( _temp705 == Cyc_Dict_Absent){ goto _LL708;} else{ goto
_LL709;} _LL709: goto _LL710; _LL708: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL706; _LL710:( void) _throw( _temp705); _LL706:;}}}{ struct Cyc_Interface_Ienv*
_temp711= Cyc_Interface_disj_merge_ienv( ie1, ie2, _temp691); if( _temp711 == 0){*
_temp700= 0;} else{ if(* _temp700){* _temp697=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* _temp697, ns,( struct Cyc_Interface_Ienv*) _check_null(
_temp711));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple18
_temp712=({ struct _tuple18 _temp714; _temp714.f1= 1; _temp714.f2= dic1;
_temp714.f3= dic1; _temp714.f4= msg; _temp714;});(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp712, dic2); if( _temp712.f1){ return({ struct Cyc_Core_Opt* _temp713=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp713->v=(
void*) _temp712.f2; _temp713;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp715; _push_handler(& _temp715);{ int _temp717= 0; if(
setjmp( _temp715.handler)){ _temp717= 1;} if( ! _temp717){{ struct _tagged_arr*
msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp718=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp718[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp719=(*(( struct _tuple3*) _check_null( info))).f1;
struct _tagged_arr _temp720=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging exports of %.*s and %.*s,", _get_arr_size( _temp719, 1u),
_temp719.curr, _get_arr_size( _temp720, 1u), _temp720.curr);}); _temp718;});}{
struct Cyc_Core_Opt* _temp721= Cyc_Interface_disj_merge_ns( i1->exports, i2->exports,
msg); if( _temp721 == 0){ struct Cyc_Interface_I* _temp722= 0; _npop_handler( 0u);
return _temp722;} if( info != 0){ msg=({ struct _tagged_arr* _temp723=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp723[ 0]=( struct
_tagged_arr)({ struct _tagged_arr _temp724=(*(( struct _tuple3*) _check_null(
info))).f1; struct _tagged_arr _temp725=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging imports of %.*s and %.*s,", _get_arr_size( _temp724, 1u),
_temp724.curr, _get_arr_size( _temp725, 1u), _temp725.curr);}); _temp723;});}{
struct Cyc_Core_Opt* _temp726= Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp721))->v, msg);
if( _temp726 == 0){ struct Cyc_Interface_I* _temp727= 0; _npop_handler( 0u);
return _temp727;}{ struct Cyc_Interface_I* _temp729=({ struct Cyc_Interface_I*
_temp728=( struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I));
_temp728->imports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp726))->v; _temp728->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp721))->v; _temp728;}); _npop_handler( 0u); return _temp729;}}}};
_pop_handler();} else{ void* _temp716=( void*) _exn_thrown; void* _temp731=
_temp716; _LL733: if( _temp731 == Cyc_Tcdecl_Incompatible){ goto _LL734;} else{
goto _LL735;} _LL735: goto _LL736; _LL734: return 0; _LL736:( void) _throw(
_temp731); _LL732:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp737= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp741=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp741->f1=*(( struct _tagged_arr*)
_check_null( curr_info)); _temp741->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp741;}): 0); if( _temp737 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp737); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp738=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp738[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp739=*(( struct _tagged_arr*) _check_null( curr_info));
struct _tagged_arr _temp740=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); xprintf("%.*s+%.*s", _get_arr_size( _temp739, 1u),
_temp739.curr, _get_arr_size( _temp740, 1u), _temp740.curr);}); _temp738;}): 0;}
if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null( linfo))->tl;}}
return( struct Cyc_Interface_I*) curr_i;}} struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(* get)( void*), struct Cyc_List_List* la, struct Cyc_List_List*
linfo){ if( la == 0){ return( struct Cyc_Interface_I*) Cyc_Interface_empty();}{
struct Cyc_Interface_I* curr_i= get(( void*)(( struct Cyc_List_List*)
_check_null( la))->hd); struct _tagged_arr* curr_info= linfo != 0?( struct
_tagged_arr*)(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd):
0; la=(( struct Cyc_List_List*) _check_null( la))->tl; if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;} for( 0; la != 0; la=(( struct
Cyc_List_List*) _check_null( la))->tl){ struct Cyc_Interface_I* _temp742= Cyc_Interface_merge(
curr_i, get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd),( curr_info
!= 0? linfo != 0: 0)?({ struct _tuple3* _temp746=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp746->f1=*(( struct _tagged_arr*) _check_null(
curr_info)); _temp746->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp746;}): 0); if( _temp742 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp742); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp743=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp743[ 0]=( struct _tagged_arr)({
struct _tagged_arr _temp744=*(( struct _tagged_arr*) _check_null( curr_info));
struct _tagged_arr _temp745=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); xprintf("%.*s+%.*s", _get_arr_size( _temp744, 1u),
_temp744.curr, _get_arr_size( _temp745, 1u), _temp745.curr);}); _temp743;}): 0;}
if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null( linfo))->tl;}}
return( struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace(
struct Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;}
return({ struct Cyc_List_List* _temp747=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp747->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp748=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp748[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp749; _temp749.tag= Cyc_Absyn_Namespace_d;
_temp749.f1=( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp749.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp749;}); _temp748;}), 0); _temp747->tl= 0; _temp747;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp750->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp751=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp751[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp752; _temp752.tag= Cyc_Absyn_Struct_d;
_temp752.f1= d; _temp752;}); _temp751;}), 0); _temp750->tl= tds; _temp750;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp753=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp753[ 0]=* d; _temp753;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp754=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp754->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp755=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp755[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp756; _temp756.tag= Cyc_Absyn_Struct_d; _temp756.f1=
d; _temp756;}); _temp755;}), 0); _temp754->tl= tds; _temp754;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp757=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp757->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp758=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp758[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp759; _temp759.tag= Cyc_Absyn_Union_d;
_temp759.f1= d; _temp759;}); _temp758;}), 0); _temp757->tl= tds; _temp757;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp760=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp760[ 0]=* d; _temp760;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp761=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp761->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp762=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp762[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp763; _temp763.tag= Cyc_Absyn_Union_d; _temp763.f1= d; _temp763;}); _temp762;}),
0); _temp761->tl= tds; _temp761;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp764=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp764->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp765=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp765[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp766; _temp766.tag= Cyc_Absyn_Tunion_d; _temp766.f1=
d; _temp766;}); _temp765;}), 0); _temp764->tl= tds; _temp764;});} static
unsigned char _temp767[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp767, _temp767, _temp767 + 2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp768=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp768->f1= Cyc_Absyn_empty_tqual();
_temp768->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp769=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp769[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp770; _temp770.tag= Cyc_Absyn_VarType; _temp770.f1=({ struct Cyc_Absyn_Tvar*
_temp771=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp771->name= Cyc_Interface_us_p; _temp771->identity= 0; _temp771->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_BoxKind); _temp771;}); _temp770;}); _temp769;}); _temp768;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp772=( struct
Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp772[ 0]=* f; _temp772;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple10*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
d=({ struct Cyc_Absyn_Tuniondecl* _temp773=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp773[ 0]=* d; _temp773;});
if( d->fields != 0){ d->fields=({ struct Cyc_Core_Opt* _temp774=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp774->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp774;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp775=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp775->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp776=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp776[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp777; _temp777.tag= Cyc_Absyn_Tunion_d; _temp777.f1=
d; _temp777;}); _temp776;}), 0); _temp775->tl= tds; _temp775;});} static struct
Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp778=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp778->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp779=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp779[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp780; _temp780.tag= Cyc_Absyn_Enum_d;
_temp780.f1= d; _temp780;}); _temp779;}), 0); _temp778->tl= tds; _temp778;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp781=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp781[ 0]=* d; _temp781;}); d->fields= 0;
if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp782=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp782->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp783=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp783[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp784; _temp784.tag= Cyc_Absyn_Enum_d; _temp784.f1= d; _temp784;}); _temp783;}),
0); _temp782->tl= tds; _temp782;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp785=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp785->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp786=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp786[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp787; _temp787.tag= Cyc_Absyn_Typedef_d; _temp787.f1=
d; _temp787;}); _temp786;}), 0); _temp785->tl= tds; _temp785;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp788=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp788->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp789=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp789[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp790; _temp790.tag= Cyc_Absyn_Var_d;
_temp790.f1= d; _temp790;}); _temp789;}), 0); _temp788->tl= tds; _temp788;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp793; struct Cyc_Absyn_Tunionfield*
_temp794; struct Cyc_Absyn_Tuniondecl* _temp796; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp791= d; _temp793=* _temp791; _LL797: _temp796= _temp793.base; goto _LL795;
_LL795: _temp794= _temp793.field; goto _LL792; _LL792: _temp796=({ struct Cyc_Absyn_Tuniondecl*
_temp798=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp798[ 0]=* _temp796; _temp798;}); _temp796->fields=({ struct Cyc_Core_Opt*
_temp799=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp799->v=( void*)({ struct Cyc_List_List* _temp800=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp800->hd=( void*) _temp794;
_temp800->tl= 0; _temp800;}); _temp799;});( void*)( _temp796->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp801->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp802=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp802[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp803; _temp803.tag= Cyc_Absyn_Tunion_d; _temp803.f1=
_temp796; _temp803;}); _temp802;}), 0); _temp801->tl= tds; _temp801;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp806; struct Cyc_Absyn_Tunionfield*
_temp807; struct Cyc_Absyn_Tuniondecl* _temp809; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp804= d; _temp806=* _temp804; _LL810: _temp809= _temp806.base; goto _LL808;
_LL808: _temp807= _temp806.field; goto _LL805; _LL805: _temp809=({ struct Cyc_Absyn_Tuniondecl*
_temp811=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp811[ 0]=* _temp809; _temp811;}); _temp807= Cyc_Interface_rewrite_tunionfield(
_temp807);( void*)( _temp807->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp809->fields=({
struct Cyc_Core_Opt* _temp812=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp812->v=( void*)({ struct Cyc_List_List* _temp813=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp813->hd=( void*)
_temp807; _temp813->tl= 0; _temp813;}); _temp812;});( void*)( _temp809->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp814=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp814->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp815=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp815[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp816; _temp816.tag= Cyc_Absyn_Tunion_d;
_temp816.f1= _temp809; _temp816;}); _temp815;}), 0); _temp814->tl= tds; _temp814;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp817= 0; _temp817=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, _temp817); _temp817=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp817); _temp817=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp817); _temp817=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp817); if( _temp817 != 0){ _temp817=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp817); _temp817= Cyc_Interface_add_namespace(
_temp817, ns); Cyc_Absynpp_decllist2file( _temp817, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp818= 0; _temp818=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp818); if( _temp818 != 0){ _temp818=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp818); _temp818= Cyc_Interface_add_namespace(
_temp818, ns); Cyc_Absynpp_decllist2file( _temp818, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp819= 0; _temp819=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp819); if(
_temp819 != 0){ _temp819=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp819); _temp819= Cyc_Interface_add_namespace( _temp819,
ns); Cyc_Absynpp_decllist2file( _temp819, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp820= 0; _temp820=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp820);
_temp820=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp820); _temp820=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp820); _temp820=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp820); _temp820=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp820); _temp820=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp820); if( _temp820 != 0){ _temp820=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp820); _temp820= Cyc_Interface_add_namespace(
_temp820, ns); Cyc_Absynpp_decllist2file( _temp820, f);}} void Cyc_Interface_print(
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp821= Cyc_Parse_parse_file( f);
struct Cyc_Tcenv_Tenv* _temp822= Cyc_Tcenv_tc_init(); Cyc_Tc_tc( _temp822, 0,
_temp821); return Cyc_Interface_gen_extract( _temp822->ae, 0);}} void Cyc_Interface_save(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i,
f);} struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE* f){
return Cyc_Interface_parse( f);}