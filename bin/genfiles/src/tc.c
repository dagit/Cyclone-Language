 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple0{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
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
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tagged_arr*,
struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern unsigned char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_arr);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr);
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
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f,
int* res, struct _tagged_arr* v, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg); static unsigned char _temp0[ 1u]=""; static struct
_tagged_arr Cyc_Tc_tc_msg_c={ _temp0, _temp0, _temp0 + 1u}; static struct
_tagged_arr* Cyc_Tc_tc_msg=( struct _tagged_arr*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp1= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp7;
struct Cyc_List_List* _temp9; struct Cyc_List_List** _temp11; struct Cyc_List_List*
_temp12; struct Cyc_Absyn_VarargInfo* _temp14; int _temp16; struct Cyc_List_List*
_temp18; void* _temp20; struct Cyc_Core_Opt* _temp22; struct Cyc_List_List*
_temp24; _LL3: if(( unsigned int) _temp1 > 4u?*(( int*) _temp1) == Cyc_Absyn_FnType:
0){ _LL8: _temp7=(( struct Cyc_Absyn_FnType_struct*) _temp1)->f1; _LL25: _temp24=
_temp7.tvars; goto _LL23; _LL23: _temp22= _temp7.effect; goto _LL21; _LL21:
_temp20=( void*) _temp7.ret_typ; goto _LL19; _LL19: _temp18= _temp7.args; goto
_LL17; _LL17: _temp16= _temp7.c_varargs; goto _LL15; _LL15: _temp14= _temp7.cyc_varargs;
goto _LL13; _LL13: _temp12= _temp7.rgn_po; goto _LL10; _LL10: _temp9= _temp7.attributes;
_temp11=&((( struct Cyc_Absyn_FnType_struct*) _temp1)->f1).attributes; goto _LL4;}
else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_List_List* _temp26= 0;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp27=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int _temp43;
_LL29: if(( unsigned int) _temp27 > 16u?*(( int*) _temp27) == Cyc_Absyn_Regparm_att:
0){ _LL44: _temp43=(( struct Cyc_Absyn_Regparm_att_struct*) _temp27)->f1; goto
_LL30;} else{ goto _LL31;} _LL31: if( _temp27 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL32;} else{ goto _LL33;} _LL33: if( _temp27 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL34;} else{ goto _LL35;} _LL35: if( _temp27 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL36;} else{ goto _LL37;} _LL37: if( _temp27 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL38;} else{ goto _LL39;} _LL39: if( _temp27 ==( void*) Cyc_Absyn_Const_att){
goto _LL40;} else{ goto _LL41;} _LL41: goto _LL42; _LL30: goto _LL32; _LL32:
goto _LL34; _LL34: goto _LL36; _LL36: goto _LL38; _LL38: goto _LL40; _LL40:*
_temp11=({ struct Cyc_List_List* _temp45=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp45->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp45->tl=* _temp11; _temp45;}); continue; _LL42:
_temp26=({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp46->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp46->tl= _temp26; _temp46;}); continue; _LL28:;}
return _temp26;} _LL6: return(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("transfer_fn_type_atts", sizeof( unsigned char), 22u));
_LL2:;} struct _tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd, int check_var_init){ struct _tagged_arr* v=(*
vd->name).f2; void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List*
atts= vd->attributes;{ void* _temp47=(* vd->name).f1; struct Cyc_List_List*
_temp55; struct Cyc_List_List* _temp57; _LL49: if(( unsigned int) _temp47 > 1u?*((
int*) _temp47) == Cyc_Absyn_Rel_n: 0){ _LL56: _temp55=(( struct Cyc_Absyn_Rel_n_struct*)
_temp47)->f1; if( _temp55 == 0){ goto _LL50;} else{ goto _LL51;}} else{ goto
_LL51;} _LL51: if(( unsigned int) _temp47 > 1u?*(( int*) _temp47) == Cyc_Absyn_Abs_n:
0){ _LL58: _temp57=(( struct Cyc_Absyn_Abs_n_struct*) _temp47)->f1; if( _temp57
== 0){ goto _LL52;} else{ goto _LL53;}} else{ goto _LL53;} _LL53: goto _LL54;
_LL50: goto _LL48; _LL52: goto _LL48; _LL54: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL48:;}(* vd->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp59=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp59[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp60; _temp60.tag= Cyc_Absyn_Abs_n; _temp60.f1= te->ns; _temp60;}); _temp59;});{
void* _temp61= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp68; struct
Cyc_Absyn_Tqual _temp70; void* _temp72; _LL63: if(( unsigned int) _temp61 > 4u?*((
int*) _temp61) == Cyc_Absyn_ArrayType: 0){ _LL73: _temp72=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp61)->f1; goto _LL71; _LL71: _temp70=(( struct Cyc_Absyn_ArrayType_struct*)
_temp61)->f2; goto _LL69; _LL69: _temp68=(( struct Cyc_Absyn_ArrayType_struct*)
_temp61)->f3; if( _temp68 == 0){ goto _LL67;} else{ goto _LL65;}} else{ goto
_LL65;} _LL67: if( vd->initializer != 0){ goto _LL64;} else{ goto _LL65;} _LL65:
goto _LL66; _LL64:{ void* _temp74=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp84; struct _tagged_arr _temp86; struct Cyc_List_List*
_temp88; struct Cyc_Core_Opt* _temp90; struct Cyc_List_List* _temp92; _LL76: if(*((
int*) _temp74) == Cyc_Absyn_Const_e){ _LL85: _temp84=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp74)->f1; if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) == Cyc_Absyn_String_c:
0){ _LL87: _temp86=(( struct Cyc_Absyn_String_c_struct*) _temp84)->f1; goto
_LL77;} else{ goto _LL78;}} else{ goto _LL78;} _LL78: if(*(( int*) _temp74) ==
Cyc_Absyn_UnresolvedMem_e){ _LL91: _temp90=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp74)->f1; goto _LL89; _LL89: _temp88=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp74)->f2; goto _LL79;} else{ goto _LL80;} _LL80: if(*(( int*) _temp74) ==
Cyc_Absyn_Array_e){ _LL93: _temp92=(( struct Cyc_Absyn_Array_e_struct*) _temp74)->f1;
goto _LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL77: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp94=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp94[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp95; _temp95.tag= Cyc_Absyn_ArrayType; _temp95.f1=(
void*) _temp72; _temp95.f2= _temp70; _temp95.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp86, sizeof( unsigned char)), 0); _temp95;}); _temp94;})));
goto _LL75; _LL79: _temp92= _temp88; goto _LL81; _LL81: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp96=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp96[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp97; _temp97.tag= Cyc_Absyn_ArrayType; _temp97.f1=(
void*) _temp72; _temp97.f2= _temp70; _temp97.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp92), 0);
_temp97;}); _temp96;}))); goto _LL75; _LL83: goto _LL75; _LL75:;} goto _LL62;
_LL66: goto _LL62; _LL62:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc, _tag_arr("extern declaration should not have initializer",
sizeof( unsigned char), 47u));}} else{ if( ! Cyc_Tcutil_is_function_type( t)){
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp98=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int _temp118;
struct _tagged_arr _temp120; _LL100: if(( unsigned int) _temp98 > 16u?*(( int*)
_temp98) == Cyc_Absyn_Aligned_att: 0){ _LL119: _temp118=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp98)->f1; goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int)
_temp98 > 16u?*(( int*) _temp98) == Cyc_Absyn_Section_att: 0){ _LL121: _temp120=((
struct Cyc_Absyn_Section_att_struct*) _temp98)->f1; goto _LL103;} else{ goto
_LL104;} _LL104: if( _temp98 ==( void*) Cyc_Absyn_Nocommon_att){ goto _LL105;}
else{ goto _LL106;} _LL106: if( _temp98 ==( void*) Cyc_Absyn_Shared_att){ goto
_LL107;} else{ goto _LL108;} _LL108: if( _temp98 ==( void*) Cyc_Absyn_Unused_att){
goto _LL109;} else{ goto _LL110;} _LL110: if( _temp98 ==( void*) Cyc_Absyn_Weak_att){
goto _LL111;} else{ goto _LL112;} _LL112: if( _temp98 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp98 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL115;} else{ goto _LL116;} _LL116: goto _LL117; _LL101: continue; _LL103:
continue; _LL105: continue; _LL107: continue; _LL109: continue; _LL111:
continue; _LL113: continue; _LL115: continue; _LL117: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp122= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); struct _tagged_arr
_temp123= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_get_arr_size( _temp122, 1u), _temp122.curr, _get_arr_size( _temp123, 1u),
_temp123.curr);})); goto _LL99; _LL99:;} if( vd->initializer == 0){ if(
check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp124= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_arr _temp125= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_get_arr_size( _temp124, 1u), _temp124.curr, _get_arr_size( _temp125, 1u),
_temp125.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp126=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp127= Cyc_Tcexp_tcExpInitializer( te,(
void**)& t, _temp126); if( ! Cyc_Tcutil_coerce_assign( te, _temp126, t)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp128= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_arr _temp129= Cyc_Absynpp_typ2string( t); struct
_tagged_arr _temp130= Cyc_Absynpp_typ2string( _temp127); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_get_arr_size( _temp128, 1u), _temp128.curr, _get_arr_size( _temp129, 1u),
_temp129.curr, _get_arr_size( _temp130, 1u), _temp130.curr);}));} if( ! Cyc_Tcutil_is_const_exp(
te, _temp126)){ Cyc_Tcutil_terr( loc, _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u));}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp131=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp151; int _temp153; _LL133: if(( unsigned int)
_temp131 > 16u?*(( int*) _temp131) == Cyc_Absyn_Regparm_att: 0){ _LL152:
_temp151=(( struct Cyc_Absyn_Regparm_att_struct*) _temp131)->f1; goto _LL134;}
else{ goto _LL135;} _LL135: if( _temp131 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL136;} else{ goto _LL137;} _LL137: if( _temp131 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL138;} else{ goto _LL139;} _LL139: if( _temp131 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL140;} else{ goto _LL141;} _LL141: if( _temp131 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL142;} else{ goto _LL143;} _LL143: if( _temp131 ==( void*) Cyc_Absyn_Const_att){
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp131 > 16u?*((
int*) _temp131) == Cyc_Absyn_Aligned_att: 0){ _LL154: _temp153=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp131)->f1; goto _LL146;} else{ goto _LL147;} _LL147: if( _temp131 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL148;} else{ goto _LL149;} _LL149: goto _LL150;
_LL134: goto _LL136; _LL136: goto _LL138; _LL138: goto _LL140; _LL140: goto
_LL142; _LL142: goto _LL144; _LL144:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcVardecl: fn type attributes in function var decl", sizeof(
unsigned char), 51u)); goto _LL132; _LL146: goto _LL148; _LL148: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp155= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_get_arr_size( _temp155, 1u), _temp155.curr);})); goto _LL132; _LL150: continue;
_LL132:;}}}{ struct _handler_cons _temp156; _push_handler(& _temp156);{ int
_temp158= 0; if( setjmp( _temp156.handler)){ _temp158= 1;} if( ! _temp158){{
struct _tuple3* _temp159=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp160=(*
_temp159).f1; void* _temp170; struct Cyc_Absyn_Structdecl* _temp172; struct Cyc_Absyn_Tunionfield*
_temp174; struct Cyc_Absyn_Tuniondecl* _temp176; struct Cyc_Absyn_Enumfield*
_temp178; struct Cyc_Absyn_Enumdecl* _temp180; _LL162: if(*(( int*) _temp160) ==
Cyc_Tcenv_VarRes){ _LL171: _temp170=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp160)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(*(( int*) _temp160)
== Cyc_Tcenv_StructRes){ _LL173: _temp172=(( struct Cyc_Tcenv_StructRes_struct*)
_temp160)->f1; goto _LL165;} else{ goto _LL166;} _LL166: if(*(( int*) _temp160)
== Cyc_Tcenv_TunionRes){ _LL177: _temp176=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp160)->f1; goto _LL175; _LL175: _temp174=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp160)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if(*(( int*) _temp160)
== Cyc_Tcenv_EnumRes){ _LL181: _temp180=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp160)->f1; goto _LL179; _LL179: _temp178=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp160)->f2; goto _LL169;} else{ goto _LL161;} _LL163: { struct Cyc_Absyn_Global_b_struct*
_temp182=({ struct Cyc_Absyn_Global_b_struct* _temp187=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp187[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp188; _temp188.tag= Cyc_Absyn_Global_b; _temp188.f1=
vd; _temp188;}); _temp187;}); void* _temp183= Cyc_Tcdecl_merge_binding( _temp170,(
void*) _temp182, loc, Cyc_Tc_tc_msg); if( _temp183 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL161;} if( _temp183 == _temp170?(* _temp159).f2: 0){ goto _LL161;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp184=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp184->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp185=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp185[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp186; _temp186.tag= Cyc_Tcenv_VarRes; _temp186.f1=( void*) _temp183;
_temp186;}); _temp185;}); _temp184->f2= 1; _temp184;})); goto _LL161;} _LL165:
Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL161; _LL167: Cyc_Tcutil_warn( loc,
_tag_arr("variable declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL161; _LL169: Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL161; _LL161:;}; _pop_handler();} else{
void* _temp157=( void*) _exn_thrown; void* _temp190= _temp157; _LL192: if(
_temp190 == Cyc_Dict_Absent){ goto _LL193;} else{ goto _LL194;} _LL194: goto
_LL195; _LL193: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp196=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp196->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp197=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp197[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp198; _temp198.tag= Cyc_Tcenv_VarRes;
_temp198.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp199=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp199[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp200; _temp200.tag= Cyc_Absyn_Global_b;
_temp200.f1= vd; _temp200;}); _temp199;})); _temp198;}); _temp197;}); _temp196->f2=
0; _temp196;})); goto _LL191; _LL195:( void) _throw( _temp190); _LL191:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp201=(* fd->name).f1;
struct Cyc_List_List* _temp209; struct Cyc_List_List* _temp211; _LL203: if((
unsigned int) _temp201 > 1u?*(( int*) _temp201) == Cyc_Absyn_Rel_n: 0){ _LL210:
_temp209=(( struct Cyc_Absyn_Rel_n_struct*) _temp201)->f1; if( _temp209 == 0){
goto _LL204;} else{ goto _LL205;}} else{ goto _LL205;} _LL205: if(( unsigned int)
_temp201 > 1u?*(( int*) _temp201) == Cyc_Absyn_Abs_n: 0){ _LL212: _temp211=((
struct Cyc_Absyn_Abs_n_struct*) _temp201)->f1; goto _LL206;} else{ goto _LL207;}
_LL207: goto _LL208; _LL204: goto _LL202; _LL206:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp213=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp213[ 0]=({ struct
Cyc_Core_Impossible_struct _temp214; _temp214.tag= Cyc_Core_Impossible; _temp214.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp214;});
_temp213;})); _LL208: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL202:;}(* fd->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp215=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp215[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp216; _temp216.tag= Cyc_Absyn_Abs_n; _temp216.f1= te->ns; _temp216;});
_temp215;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp217=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp225; _LL219: if(
_temp217 ==( void*) Cyc_Absyn_Packed_att){ goto _LL220;} else{ goto _LL221;}
_LL221: if(( unsigned int) _temp217 > 16u?*(( int*) _temp217) == Cyc_Absyn_Aligned_att:
0){ _LL226: _temp225=(( struct Cyc_Absyn_Aligned_att_struct*) _temp217)->f1;
goto _LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL220: goto _LL222;
_LL222: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp227=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s for function", _get_arr_size( _temp227, 1u),
_temp227.curr);})); goto _LL218; _LL224: goto _LL218; _LL218:;}}{ struct
_handler_cons _temp228; _push_handler(& _temp228);{ int _temp230= 0; if( setjmp(
_temp228.handler)){ _temp230= 1;} if( ! _temp230){{ struct _tuple3* _temp231=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); void* _temp232=(* _temp231).f1; void* _temp242; struct Cyc_Absyn_Structdecl*
_temp244; struct Cyc_Absyn_Tunionfield* _temp246; struct Cyc_Absyn_Tuniondecl*
_temp248; struct Cyc_Absyn_Enumfield* _temp250; struct Cyc_Absyn_Enumdecl*
_temp252; _LL234: if(*(( int*) _temp232) == Cyc_Tcenv_VarRes){ _LL243: _temp242=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp232)->f1; goto _LL235;} else{
goto _LL236;} _LL236: if(*(( int*) _temp232) == Cyc_Tcenv_StructRes){ _LL245:
_temp244=(( struct Cyc_Tcenv_StructRes_struct*) _temp232)->f1; goto _LL237;}
else{ goto _LL238;} _LL238: if(*(( int*) _temp232) == Cyc_Tcenv_TunionRes){
_LL249: _temp248=(( struct Cyc_Tcenv_TunionRes_struct*) _temp232)->f1; goto
_LL247; _LL247: _temp246=(( struct Cyc_Tcenv_TunionRes_struct*) _temp232)->f2;
goto _LL239;} else{ goto _LL240;} _LL240: if(*(( int*) _temp232) == Cyc_Tcenv_EnumRes){
_LL253: _temp252=(( struct Cyc_Tcenv_EnumRes_struct*) _temp232)->f1; goto _LL251;
_LL251: _temp250=(( struct Cyc_Tcenv_EnumRes_struct*) _temp232)->f2; goto _LL241;}
else{ goto _LL233;} _LL235: { struct Cyc_Absyn_Funname_b_struct* _temp254=({
struct Cyc_Absyn_Funname_b_struct* _temp259=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp259[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp260; _temp260.tag= Cyc_Absyn_Funname_b; _temp260.f1=
fd; _temp260;}); _temp259;}); void* _temp255= Cyc_Tcdecl_merge_binding( _temp242,(
void*) _temp254, loc, Cyc_Tc_tc_msg); if( _temp255 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL233;} if( _temp255 == _temp242?(* _temp231).f2: 0){ goto _LL233;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp256=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp256->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp257=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp257[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp258; _temp258.tag= Cyc_Tcenv_VarRes; _temp258.f1=( void*) _temp255;
_temp258;}); _temp257;}); _temp256->f2= 1; _temp256;})); goto _LL233;} _LL237:
Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL233; _LL239: Cyc_Tcutil_warn( loc,
_tag_arr("function declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL233; _LL241: Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL233; _LL233:;}; _pop_handler();} else{
void* _temp229=( void*) _exn_thrown; void* _temp262= _temp229; _LL264: if(
_temp262 == Cyc_Dict_Absent){ goto _LL265;} else{ goto _LL266;} _LL266: goto
_LL267; _LL265: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp268=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp268->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp269=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp269[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp270; _temp270.tag= Cyc_Tcenv_VarRes;
_temp270.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp271=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp271[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp272; _temp272.tag= Cyc_Absyn_Funname_b;
_temp272.f1= fd; _temp272;}); _temp271;})); _temp270;}); _temp269;}); _temp268->f2=
0; _temp268;})); goto _LL263; _LL267:( void) _throw( _temp262); _LL263:;}}} te->le=({
struct Cyc_Core_Opt* _temp273=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp273->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp274=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp274[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp275; _temp275.tag= Cyc_Tcenv_Outermost;
_temp275.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp275;}); _temp274;}));
_temp273;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u));} te->le= 0;}} static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl* td){ struct _tagged_arr* v=(* td->name).f2;{
void* _temp276=(* td->name).f1; struct Cyc_List_List* _temp284; struct Cyc_List_List*
_temp286; _LL278: if(( unsigned int) _temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Rel_n:
0){ _LL285: _temp284=(( struct Cyc_Absyn_Rel_n_struct*) _temp276)->f1; if(
_temp284 == 0){ goto _LL279;} else{ goto _LL280;}} else{ goto _LL280;} _LL280:
if(( unsigned int) _temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Abs_n: 0){
_LL287: _temp286=(( struct Cyc_Absyn_Abs_n_struct*) _temp276)->f1; if( _temp286
== 0){ goto _LL281;} else{ goto _LL282;}} else{ goto _LL282;} _LL282: goto
_LL283; _LL279: goto _LL277; _LL281: goto _LL277; _LL283: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL277:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)( ge->typedefs, v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp288=* v; xprintf("redeclaration of typedef %.*s",
_get_arr_size( _temp288, 1u), _temp288.curr);})); return;}(* td->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp289=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp289[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp290; _temp290.tag= Cyc_Absyn_Abs_n; _temp290.f1= te->ns; _temp290;});
_temp289;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_add_tvar_identities(
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,(
void*) td->defn); ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
ge->typedefs, v, td);} static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, struct Cyc_List_List* fields, struct Cyc_List_List* tvs){
struct _RegionHandle _temp291= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp291; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* _temp292= fields; for( 0; _temp292 != 0; _temp292=((
struct Cyc_List_List*) _check_null( _temp292))->tl){ struct Cyc_Absyn_Structfield
_temp295; struct Cyc_List_List* _temp296; struct Cyc_Core_Opt* _temp298; void*
_temp300; struct Cyc_Absyn_Tqual _temp302; struct _tagged_arr* _temp304; struct
Cyc_Absyn_Structfield* _temp293=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp292))->hd; _temp295=* _temp293; _LL305: _temp304= _temp295.name;
goto _LL303; _LL303: _temp302= _temp295.tq; goto _LL301; _LL301: _temp300=( void*)
_temp295.type; goto _LL299; _LL299: _temp298= _temp295.width; goto _LL297;
_LL297: _temp296= _temp295.attributes; goto _LL294; _LL294: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp304)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp306=* _temp304; struct _tagged_arr _temp307= obj; xprintf("duplicate field %.*s in %.*s",
_get_arr_size( _temp306, 1u), _temp306.curr, _get_arr_size( _temp307, 1u),
_temp307.curr);}));} if( Cyc_String_strcmp(* _temp304, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List* _temp308=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp308->hd=( void*) _temp304; _temp308->tl= prev_fields; _temp308;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp300); if( _temp298 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp298))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp309=* _temp304; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp309, 1u), _temp309.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp298))->v);}{
void* _temp310= Cyc_Tcutil_compress( _temp300); void* _temp316; void* _temp318;
_LL312: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_IntType:
0){ _LL319: _temp318=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp310)->f1;
goto _LL317; _LL317: _temp316=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp310)->f2; goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL313:{
void* _temp320= _temp316; _LL322: if( _temp320 ==( void*) Cyc_Absyn_B1){ goto
_LL323;} else{ goto _LL324;} _LL324: if( _temp320 ==( void*) Cyc_Absyn_B2){ goto
_LL325;} else{ goto _LL326;} _LL326: if( _temp320 ==( void*) Cyc_Absyn_B4){ goto
_LL327;} else{ goto _LL328;} _LL328: if( _temp320 ==( void*) Cyc_Absyn_B8){ goto
_LL329;} else{ goto _LL321;} _LL323: if( w > 8){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL321; _LL325: if( w > 16){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL321; _LL327: if( w > 32){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL321; _LL329: if( w > 64){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL321; _LL321:;} goto _LL311; _LL315: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp330=* _temp304; struct _tagged_arr _temp331= Cyc_Absynpp_typ2string(
_temp300); xprintf("bitfield %.*s must have integral type but has type %.*s",
_get_arr_size( _temp330, 1u), _temp330.curr, _get_arr_size( _temp331, 1u),
_temp331.curr);})); goto _LL311; _LL311:;}}}}; _pop_region( uprev_rgn);} static
void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr obj, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl*
sd){ struct _tagged_arr* _temp332=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; if( sd->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp333= obj; xprintf("anonymous %.*ss are not allowed at top level",
_get_arr_size( _temp333, 1u), _temp333.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp334=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp342; _LL336: if( _temp334 ==( void*) Cyc_Absyn_Packed_att){
goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int) _temp334 > 16u?*((
int*) _temp334) == Cyc_Absyn_Aligned_att: 0){ _LL343: _temp342=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp334)->f1; goto _LL339;} else{ goto _LL340;} _LL340: goto _LL341; _LL337:
continue; _LL339: continue; _LL341: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp344= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_arr _temp345= obj; struct _tagged_arr
_temp346=* _temp332; xprintf("bad attribute %.*s in %.*s %.*s definition",
_get_arr_size( _temp344, 1u), _temp344.curr, _get_arr_size( _temp345, 1u),
_temp345.curr, _get_arr_size( _temp346, 1u), _temp346.curr);})); goto _LL335;
_LL335:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0; tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref* _temp347=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
void* _temp348=( void*) _temp347->v; void* _temp356; _LL350: if( _temp348 ==(
void*) Cyc_Absyn_No_constr){ goto _LL351;} else{ goto _LL352;} _LL352: if((
unsigned int) _temp348 > 1u?*(( int*) _temp348) == Cyc_Absyn_Eq_constr: 0){
_LL357: _temp356=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp348)->f1;
if( _temp356 ==( void*) Cyc_Absyn_MemKind){ goto _LL353;} else{ goto _LL354;}}
else{ goto _LL354;} _LL354: goto _LL355; _LL351:( void*)( _temp347->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp358=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp358[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp359; _temp359.tag= Cyc_Absyn_Eq_constr; _temp359.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp359;}); _temp358;}))); continue; _LL353:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp360= obj;
struct _tagged_arr _temp361=* _temp332; struct _tagged_arr _temp362=*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_get_arr_size( _temp360, 1u), _temp360.curr, _get_arr_size( _temp361, 1u),
_temp361.curr, _get_arr_size( _temp362, 1u), _temp362.curr);})); continue;
_LL355: continue; _LL349:;}}{ void* _temp363=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp371; struct Cyc_List_List*
_temp373; _LL365: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Rel_n:
0){ _LL372: _temp371=(( struct Cyc_Absyn_Rel_n_struct*) _temp363)->f1; if(
_temp371 == 0){ goto _LL366;} else{ goto _LL367;}} else{ goto _LL367;} _LL367:
if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Abs_n: 0){
_LL374: _temp373=(( struct Cyc_Absyn_Abs_n_struct*) _temp363)->f1; if( _temp373
== 0){ goto _LL368;} else{ goto _LL369;}} else{ goto _LL369;} _LL369: goto
_LL370; _LL366: goto _LL364; _LL368: goto _LL364; _LL370: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL364:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp375=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp375[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp376; _temp376.tag= Cyc_Absyn_Abs_n;
_temp376.f1= te->ns; _temp376;}); _temp375;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs); Cyc_Tcutil_add_tvar_identities( sd->tvs);} struct _tuple4{ struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_arr* _temp377=(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; const
unsigned char* _temp378="struct"; Cyc_Tc_tcStructOrUniondecl( te, ge, _tag_arr(
_temp378, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List* _temp379=
sd->tvs;{ struct _tuple4 _temp381=({ struct _tuple4 _temp380; _temp380.f1= sd->fields;
_temp380.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp377); _temp380;});
struct Cyc_Core_Opt* _temp391; struct Cyc_Core_Opt* _temp393; struct Cyc_Core_Opt*
_temp395; struct Cyc_Core_Opt* _temp397; struct Cyc_Core_Opt _temp399; struct
Cyc_List_List* _temp400; struct Cyc_Core_Opt* _temp402; struct Cyc_Core_Opt
_temp404; struct Cyc_Absyn_Structdecl** _temp405; struct Cyc_Core_Opt* _temp407;
struct Cyc_Core_Opt _temp409; struct Cyc_List_List* _temp410; struct Cyc_Core_Opt*
_temp412; struct Cyc_Core_Opt _temp414; struct Cyc_Absyn_Structdecl** _temp415;
struct Cyc_Core_Opt* _temp417; _LL383: _LL394: _temp393= _temp381.f1; if(
_temp393 == 0){ goto _LL392;} else{ goto _LL385;} _LL392: _temp391= _temp381.f2;
if( _temp391 == 0){ goto _LL384;} else{ goto _LL385;} _LL385: _LL398: _temp397=
_temp381.f1; if( _temp397 == 0){ goto _LL387;} else{ _temp399=* _temp397; _LL401:
_temp400=( struct Cyc_List_List*) _temp399.v; goto _LL396;} _LL396: _temp395=
_temp381.f2; if( _temp395 == 0){ goto _LL386;} else{ goto _LL387;} _LL387:
_LL408: _temp407= _temp381.f1; if( _temp407 == 0){ goto _LL389;} else{ _temp409=*
_temp407; _LL411: _temp410=( struct Cyc_List_List*) _temp409.v; goto _LL403;}
_LL403: _temp402= _temp381.f2; if( _temp402 == 0){ goto _LL389;} else{ _temp404=*
_temp402; _LL406: _temp405=( struct Cyc_Absyn_Structdecl**) _temp404.v; goto
_LL388;} _LL389: _LL418: _temp417= _temp381.f1; if( _temp417 == 0){ goto _LL413;}
else{ goto _LL382;} _LL413: _temp412= _temp381.f2; if( _temp412 == 0){ goto
_LL382;} else{ _temp414=* _temp412; _LL416: _temp415=( struct Cyc_Absyn_Structdecl**)
_temp414.v; goto _LL390;} _LL384: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp377,({ struct Cyc_Absyn_Structdecl**
_temp419=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp419[ 0]= sd; _temp419;})); goto _LL382; _LL386: { struct Cyc_Absyn_Structdecl**
_temp420=({ struct Cyc_Absyn_Structdecl** _temp421=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp421[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp422=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp422->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp422->name= sd->name;
_temp422->tvs= _temp379; _temp422->fields= 0; _temp422->attributes= 0; _temp422;});
_temp421;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp377, _temp420); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp378, sizeof( unsigned char), 7u), _temp400, _temp379);* _temp420=
sd; goto _LL382;} _LL388: { struct Cyc_Absyn_Structdecl* _temp423=* _temp405;*
_temp405=({ struct Cyc_Absyn_Structdecl* _temp424=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp424->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp424->name= sd->name; _temp424->tvs= _temp379; _temp424->fields=
0; _temp424->attributes= 0; _temp424;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp378, sizeof( unsigned char), 7u), _temp410, _temp379);* _temp405=
_temp423; _temp415= _temp405; goto _LL390;} _LL390: { struct Cyc_Absyn_Structdecl*
_temp425= Cyc_Tcdecl_merge_structdecl(* _temp415, sd, loc, Cyc_Tc_tc_msg); if(
_temp425 == 0){ return;} else{* _temp415=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp425); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp425);
goto _LL382;}} _LL382:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp377,({ struct _tuple3* _temp426=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp426->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp427=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp427[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp428; _temp428.tag= Cyc_Tcenv_StructRes;
_temp428.f1= sd; _temp428;}); _temp427;}); _temp426->f2= 1; _temp426;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp429=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp430="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp430, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp431= ud->tvs; struct _tuple4 _temp433=({ struct
_tuple4 _temp432; _temp432.f1= ud->fields; _temp432.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp429); _temp432;}); struct Cyc_Core_Opt* _temp443; struct Cyc_Core_Opt*
_temp445; struct Cyc_Core_Opt* _temp447; struct Cyc_Core_Opt* _temp449; struct
Cyc_Core_Opt _temp451; struct Cyc_List_List* _temp452; struct Cyc_Core_Opt*
_temp454; struct Cyc_Core_Opt _temp456; struct Cyc_Absyn_Uniondecl** _temp457;
struct Cyc_Core_Opt* _temp459; struct Cyc_Core_Opt _temp461; struct Cyc_List_List*
_temp462; struct Cyc_Core_Opt* _temp464; struct Cyc_Core_Opt _temp466; struct
Cyc_Absyn_Uniondecl** _temp467; struct Cyc_Core_Opt* _temp469; _LL435: _LL446:
_temp445= _temp433.f1; if( _temp445 == 0){ goto _LL444;} else{ goto _LL437;}
_LL444: _temp443= _temp433.f2; if( _temp443 == 0){ goto _LL436;} else{ goto
_LL437;} _LL437: _LL450: _temp449= _temp433.f1; if( _temp449 == 0){ goto _LL439;}
else{ _temp451=* _temp449; _LL453: _temp452=( struct Cyc_List_List*) _temp451.v;
goto _LL448;} _LL448: _temp447= _temp433.f2; if( _temp447 == 0){ goto _LL438;}
else{ goto _LL439;} _LL439: _LL460: _temp459= _temp433.f1; if( _temp459 == 0){
goto _LL441;} else{ _temp461=* _temp459; _LL463: _temp462=( struct Cyc_List_List*)
_temp461.v; goto _LL455;} _LL455: _temp454= _temp433.f2; if( _temp454 == 0){
goto _LL441;} else{ _temp456=* _temp454; _LL458: _temp457=( struct Cyc_Absyn_Uniondecl**)
_temp456.v; goto _LL440;} _LL441: _LL470: _temp469= _temp433.f1; if( _temp469 ==
0){ goto _LL465;} else{ goto _LL434;} _LL465: _temp464= _temp433.f2; if(
_temp464 == 0){ goto _LL434;} else{ _temp466=* _temp464; _LL468: _temp467=(
struct Cyc_Absyn_Uniondecl**) _temp466.v; goto _LL442;} _LL436: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp429,({
struct Cyc_Absyn_Uniondecl** _temp471=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp471[ 0]= ud; _temp471;})); goto
_LL434; _LL438: { struct Cyc_Absyn_Uniondecl** _temp472=({ struct Cyc_Absyn_Uniondecl**
_temp476=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp476[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp477=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp477->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp477->name= ud->name; _temp477->tvs= _temp431; _temp477->fields=
0; _temp477->attributes= ud->attributes; _temp477;}); _temp476;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp429,
_temp472); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp430, sizeof(
unsigned char), 6u), _temp452, _temp431);{ struct Cyc_List_List* f= _temp452;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp473=*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->name;
struct _tagged_arr _temp474=* _temp429; struct _tagged_arr _temp475= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type);
xprintf("field %.*s of union %.*s has type %.*s which is not `bits-only'",
_get_arr_size( _temp473, 1u), _temp473.curr, _get_arr_size( _temp474, 1u),
_temp474.curr, _get_arr_size( _temp475, 1u), _temp475.curr);}));}}}* _temp472=
ud; goto _LL434;} _LL440: { struct Cyc_Absyn_Uniondecl* _temp478=* _temp457;*
_temp457=({ struct Cyc_Absyn_Uniondecl* _temp479=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp479->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp479->name= ud->name; _temp479->tvs= _temp431; _temp479->fields=
0; _temp479->attributes= ud->attributes; _temp479;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp430, sizeof( unsigned char), 6u), _temp462, _temp431);*
_temp457= _temp478; _temp467= _temp457; goto _LL442;} _LL442: { struct Cyc_Absyn_Uniondecl*
_temp480= Cyc_Tcdecl_merge_uniondecl(* _temp467, ud, loc, Cyc_Tc_tc_msg); if(
_temp480 == 0){ return;} else{* _temp467=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp480); goto _LL434;}} _LL434:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp481= fields; for( 0; _temp481 != 0; _temp481=(( struct Cyc_List_List*)
_check_null( _temp481))->tl){ struct Cyc_Absyn_Tunionfield* _temp482=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp481))->hd;{
struct Cyc_List_List* tvs= _temp482->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp483=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp484=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp483->kind); void* _temp485=( void*) _temp484->v; void* _temp493; _LL487:
if( _temp485 ==( void*) Cyc_Absyn_No_constr){ goto _LL488;} else{ goto _LL489;}
_LL489: if(( unsigned int) _temp485 > 1u?*(( int*) _temp485) == Cyc_Absyn_Eq_constr:
0){ _LL494: _temp493=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp485)->f1;
if( _temp493 ==( void*) Cyc_Absyn_MemKind){ goto _LL490;} else{ goto _LL491;}}
else{ goto _LL491;} _LL491: goto _LL492; _LL488:( void*)( _temp484->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp495=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp495[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp496; _temp496.tag= Cyc_Absyn_Eq_constr; _temp496.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp496;}); _temp495;}))); goto _LL486;
_LL490: Cyc_Tcutil_terr( _temp482->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp497=*(* _temp482->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_get_arr_size( _temp497, 1u), _temp497.curr);})); goto _LL486; _LL492: goto
_LL486; _LL486:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp482->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp482->tvs);{ struct Cyc_List_List* typs= _temp482->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp482->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp498=(*
_temp482->name).f1; struct Cyc_List_List* _temp508; struct Cyc_List_List*
_temp510; struct Cyc_List_List* _temp512; _LL500: if(( unsigned int) _temp498 >
1u?*(( int*) _temp498) == Cyc_Absyn_Rel_n: 0){ _LL509: _temp508=(( struct Cyc_Absyn_Rel_n_struct*)
_temp498)->f1; if( _temp508 == 0){ goto _LL501;} else{ goto _LL502;}} else{ goto
_LL502;} _LL502: if(( unsigned int) _temp498 > 1u?*(( int*) _temp498) == Cyc_Absyn_Rel_n:
0){ _LL511: _temp510=(( struct Cyc_Absyn_Rel_n_struct*) _temp498)->f1; goto
_LL503;} else{ goto _LL504;} _LL504: if(( unsigned int) _temp498 > 1u?*(( int*)
_temp498) == Cyc_Absyn_Abs_n: 0){ _LL513: _temp512=(( struct Cyc_Absyn_Abs_n_struct*)
_temp498)->f1; goto _LL505;} else{ goto _LL506;} _LL506: if( _temp498 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL507;} else{ goto _LL499;} _LL501: if( is_xtunion){(*
_temp482->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp514=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp514[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp515; _temp515.tag= Cyc_Absyn_Abs_n;
_temp515.f1= te->ns; _temp515;}); _temp514;});} else{(* _temp482->name).f1=(*
name).f1;} goto _LL499; _LL503: Cyc_Tcutil_terr( _temp482->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u)); goto _LL499; _LL505: goto _LL499; _LL507:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp516=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp516[ 0]=({ struct
Cyc_Core_Impossible_struct _temp517; _temp517.tag= Cyc_Core_Impossible; _temp517.f1=
_tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u); _temp517;});
_temp516;})); goto _LL499; _LL499:;}}}}{ struct Cyc_List_List* fields2; if(
is_xtunion){ int _temp518= 1; struct Cyc_List_List* _temp519= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp518,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp518){ fields2=
_temp519;} else{ fields2= 0;}} else{ struct _RegionHandle _temp520= _new_region();
struct _RegionHandle* uprev_rgn=& _temp520; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp521=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp521->name).f2)){ Cyc_Tcutil_terr( _temp521->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp522=*(* _temp521->name).f2; struct
_tagged_arr _temp523= obj; xprintf("duplicate field name %.*s in %.*s",
_get_arr_size( _temp522, 1u), _temp522.curr, _get_arr_size( _temp523, 1u),
_temp523.curr);}));} else{ prev_fields=({ struct Cyc_List_List* _temp524=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp524->hd=( void*)(* _temp521->name).f2; _temp524->tl= prev_fields; _temp524;});}
if(( void*) _temp521->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp525=*(* _temp521->name).f2;
xprintf("ignoring scope of field %.*s", _get_arr_size( _temp525, 1u), _temp525.curr);}));(
void*)( _temp521->sc=( void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;};
_pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp526= fields; for( 0;
_temp526 != 0; _temp526=(( struct Cyc_List_List*) _check_null( _temp526))->tl){
struct Cyc_Absyn_Tunionfield* _temp527=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp526))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp527->name).f2,({ struct _tuple3* _temp528=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp528->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp529=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp529[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp530; _temp530.tag= Cyc_Tcenv_TunionRes; _temp530.f1=
tudres; _temp530.f2= _temp527; _temp530;}); _temp529;}); _temp528->f2= 1;
_temp528;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->kind); void* _temp531=( void*) c->v; void* _temp539;
_LL533: if( _temp531 ==( void*) Cyc_Absyn_No_constr){ goto _LL534;} else{ goto
_LL535;} _LL535: if(( unsigned int) _temp531 > 1u?*(( int*) _temp531) == Cyc_Absyn_Eq_constr:
0){ _LL540: _temp539=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp531)->f1;
if( _temp539 ==( void*) Cyc_Absyn_MemKind){ goto _LL536;} else{ goto _LL537;}}
else{ goto _LL537;} _LL537: goto _LL538; _LL534:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp541=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp541[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp542; _temp542.tag= Cyc_Absyn_Eq_constr; _temp542.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp542;}); _temp541;}))); goto _LL532;
_LL536: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp543=
obj; struct _tagged_arr _temp544=* v; struct _tagged_arr _temp545=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_get_arr_size( _temp543, 1u), _temp543.curr, _get_arr_size( _temp544, 1u),
_temp544.curr, _get_arr_size( _temp545, 1u), _temp545.curr);})); goto _LL532;
_LL538: goto _LL532; _LL532:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp546; _push_handler(& _temp546);{ int _temp548= 0; if( setjmp( _temp546.handler)){
_temp548= 1;} if( ! _temp548){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp547=( void*) _exn_thrown; void*
_temp550= _temp547; _LL552: if( _temp550 == Cyc_Dict_Absent){ goto _LL553;}
else{ goto _LL554;} _LL554: goto _LL555; _LL553: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp556= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_get_arr_size( _temp556, 1u), _temp556.curr);})); return; _LL555:( void) _throw(
_temp550); _LL551:;}}} if( tud_opt != 0){ tud->name=(*(( struct Cyc_Absyn_Tuniondecl**)((
struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;} else{(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp557=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp557[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp558; _temp558.tag= Cyc_Absyn_Abs_n; _temp558.f1= te->ns; _temp558;});
_temp557;});}} else{{ void* _temp559=(* tud->name).f1; struct Cyc_List_List*
_temp567; struct Cyc_List_List* _temp569; _LL561: if(( unsigned int) _temp559 >
1u?*(( int*) _temp559) == Cyc_Absyn_Rel_n: 0){ _LL568: _temp567=(( struct Cyc_Absyn_Rel_n_struct*)
_temp559)->f1; if( _temp567 == 0){ goto _LL562;} else{ goto _LL563;}} else{ goto
_LL563;} _LL563: if(( unsigned int) _temp559 > 1u?*(( int*) _temp559) == Cyc_Absyn_Abs_n:
0){ _LL570: _temp569=(( struct Cyc_Absyn_Abs_n_struct*) _temp559)->f1; goto
_LL564;} else{ goto _LL565;} _LL565: goto _LL566; _LL562:(* tud->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp571=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp571[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp572; _temp572.tag= Cyc_Absyn_Abs_n; _temp572.f1= te->ns; _temp572;});
_temp571;}); goto _LL560; _LL564: goto _LL566; _LL566: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL560:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp574=({ struct _tuple4 _temp573; _temp573.f1= tud->fields; _temp573.f2=
tud_opt; _temp573;}); struct Cyc_Core_Opt* _temp584; struct Cyc_Core_Opt*
_temp586; struct Cyc_Core_Opt* _temp588; struct Cyc_Core_Opt* _temp590; struct
Cyc_Core_Opt _temp592; struct Cyc_List_List* _temp593; struct Cyc_List_List**
_temp595; struct Cyc_Core_Opt* _temp596; struct Cyc_Core_Opt _temp598; struct
Cyc_Absyn_Tuniondecl** _temp599; struct Cyc_Core_Opt* _temp601; struct Cyc_Core_Opt
_temp603; struct Cyc_List_List* _temp604; struct Cyc_List_List** _temp606;
struct Cyc_Core_Opt* _temp607; struct Cyc_Core_Opt _temp609; struct Cyc_Absyn_Tuniondecl**
_temp610; struct Cyc_Core_Opt* _temp612; _LL576: _LL587: _temp586= _temp574.f1;
if( _temp586 == 0){ goto _LL585;} else{ goto _LL578;} _LL585: _temp584= _temp574.f2;
if( _temp584 == 0){ goto _LL577;} else{ goto _LL578;} _LL578: _LL591: _temp590=
_temp574.f1; if( _temp590 == 0){ goto _LL580;} else{ _temp592=* _temp590; _LL594:
_temp593=( struct Cyc_List_List*) _temp592.v; _temp595=&(* _temp574.f1).v; goto
_LL589;} _LL589: _temp588= _temp574.f2; if( _temp588 == 0){ goto _LL579;} else{
goto _LL580;} _LL580: _LL602: _temp601= _temp574.f1; if( _temp601 == 0){ goto
_LL582;} else{ _temp603=* _temp601; _LL605: _temp604=( struct Cyc_List_List*)
_temp603.v; _temp606=&(* _temp574.f1).v; goto _LL597;} _LL597: _temp596=
_temp574.f2; if( _temp596 == 0){ goto _LL582;} else{ _temp598=* _temp596; _LL600:
_temp599=( struct Cyc_Absyn_Tuniondecl**) _temp598.v; goto _LL581;} _LL582:
_LL613: _temp612= _temp574.f1; if( _temp612 == 0){ goto _LL608;} else{ goto
_LL575;} _LL608: _temp607= _temp574.f2; if( _temp607 == 0){ goto _LL575;} else{
_temp609=* _temp607; _LL611: _temp610=( struct Cyc_Absyn_Tuniondecl**) _temp609.v;
goto _LL583;} _LL577: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp614=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp614[ 0]= tud; _temp614;}));
goto _LL575; _LL579: { struct Cyc_Absyn_Tuniondecl** _temp615=({ struct Cyc_Absyn_Tuniondecl**
_temp616=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp616[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp617=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp617->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp617->name= tud->name; _temp617->tvs= tvs; _temp617->fields=
0; _temp617->is_xtunion= tud->is_xtunion; _temp617;}); _temp616;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp615);* _temp595= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp595, tvs, tud);* _temp615= tud; goto _LL575;} _LL581: { struct
Cyc_Absyn_Tuniondecl* _temp618=* _temp599;* _temp599=({ struct Cyc_Absyn_Tuniondecl*
_temp619=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp619->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp619->name= tud->name;
_temp619->tvs= tvs; _temp619->fields= 0; _temp619->is_xtunion= tud->is_xtunion;
_temp619;});* _temp606= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp606, tvs, tud);* _temp599= _temp618; _temp610= _temp599; goto
_LL583;} _LL583: { struct Cyc_Absyn_Tuniondecl* _temp620= Cyc_Tcdecl_merge_tuniondecl(*
_temp610, tud, loc, Cyc_Tc_tc_msg); if( _temp620 == 0){ return;} else{* _temp610=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp620); goto _LL575;}} _LL575:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp621=(* ed->name).f1; struct Cyc_List_List*
_temp629; struct Cyc_List_List* _temp631; _LL623: if(( unsigned int) _temp621 >
1u?*(( int*) _temp621) == Cyc_Absyn_Rel_n: 0){ _LL630: _temp629=(( struct Cyc_Absyn_Rel_n_struct*)
_temp621)->f1; if( _temp629 == 0){ goto _LL624;} else{ goto _LL625;}} else{ goto
_LL625;} _LL625: if(( unsigned int) _temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_Abs_n:
0){ _LL632: _temp631=(( struct Cyc_Absyn_Abs_n_struct*) _temp621)->f1; if(
_temp631 == 0){ goto _LL626;} else{ goto _LL627;}} else{ goto _LL627;} _LL627:
goto _LL628; _LL624: goto _LL622; _LL626: goto _LL622; _LL628: Cyc_Tcutil_terr(
loc, _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u)); return; _LL622:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp633=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp633[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp634; _temp634.tag= Cyc_Absyn_Abs_n;
_temp634.f1= te->ns; _temp634;}); _temp633;}); if( ed->fields != 0){ struct
_RegionHandle _temp635= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp635; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp636=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp636->name).f2)){ Cyc_Tcutil_terr( _temp636->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp637=*(* _temp636->name).f2; xprintf("duplicate field name %.*s",
_get_arr_size( _temp637, 1u), _temp637.curr);}));} else{ prev_fields=({ struct
Cyc_List_List* _temp638=( struct Cyc_List_List*) _region_malloc( uprev_rgn,
sizeof( struct Cyc_List_List)); _temp638->hd=( void*)(* _temp636->name).f2;
_temp638->tl= prev_fields; _temp638;});} if( _temp636->tag == 0){ _temp636->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp636->loc);} else{ if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp636->tag))){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp639=* v;
struct _tagged_arr _temp640=*(* _temp636->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_get_arr_size( _temp639, 1u), _temp639.curr, _get_arr_size( _temp640, 1u),
_temp640.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*) _check_null( _temp636->tag)); tag_count= t1 + 1;(* _temp636->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp641=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp641[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp642; _temp642.tag= Cyc_Absyn_Abs_n; _temp642.f1= te->ns; _temp642;});
_temp641;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp643;
_push_handler(& _temp643);{ int _temp645= 0; if( setjmp( _temp643.handler)){
_temp645= 1;} if( ! _temp645){{ struct Cyc_Absyn_Enumdecl** _temp646=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp647= Cyc_Tcdecl_merge_enumdecl(*
_temp646, ed, loc, Cyc_Tc_tc_msg); if( _temp647 == 0){ _npop_handler( 0u);
return;}* _temp646=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp647);};
_pop_handler();} else{ void* _temp644=( void*) _exn_thrown; void* _temp649=
_temp644; _LL651: if( _temp649 == Cyc_Dict_Absent){ goto _LL652;} else{ goto
_LL653;} _LL653: goto _LL654; _LL652: { struct Cyc_Absyn_Enumdecl** _temp655=({
struct Cyc_Absyn_Enumdecl** _temp656=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp656[ 0]= ed; _temp656;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp655);
goto _LL650;} _LL654:( void) _throw( _temp649); _LL650:;}}} if( ed->fields != 0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp657=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp657->name).f2,({ struct _tuple3* _temp658=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp658->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp659=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp659[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp660; _temp660.tag= Cyc_Tcenv_EnumRes; _temp660.f1= ed; _temp660.f2=
_temp657; _temp660;}); _temp659;}); _temp658->f2= 1; _temp658;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp661=
sc; _LL663: if( _temp661 ==( void*) Cyc_Absyn_Static){ goto _LL664;} else{ goto
_LL665;} _LL665: if( _temp661 ==( void*) Cyc_Absyn_Abstract){ goto _LL666;}
else{ goto _LL667;} _LL667: if( _temp661 ==( void*) Cyc_Absyn_Public){ goto
_LL668;} else{ goto _LL669;} _LL669: if( _temp661 ==( void*) Cyc_Absyn_Extern){
goto _LL670;} else{ goto _LL671;} _LL671: if( _temp661 ==( void*) Cyc_Absyn_ExternC){
goto _LL672;} else{ goto _LL662;} _LL664: Cyc_Tcutil_warn( loc, _tag_arr("static declaration nested within extern \"C\"",
sizeof( unsigned char), 44u)); return 0; _LL666: Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u)); return 0; _LL668: return 1; _LL670: return 1;
_LL672: Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration",
sizeof( unsigned char), 30u)); return 1; _LL662:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp673= ds0; for( 0; _temp673 != 0; _temp673=(( struct
Cyc_List_List*) _check_null( _temp673))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp673))->hd; struct
Cyc_Position_Segment* loc= d->loc; void* _temp674=( void*) d->r; int _temp700;
struct Cyc_Absyn_Exp* _temp702; struct Cyc_Core_Opt* _temp704; struct Cyc_Core_Opt*
_temp706; struct Cyc_Absyn_Pat* _temp708; struct Cyc_List_List* _temp710; struct
Cyc_Absyn_Vardecl* _temp712; struct Cyc_Absyn_Fndecl* _temp714; struct Cyc_Absyn_Typedefdecl*
_temp716; struct Cyc_Absyn_Structdecl* _temp718; struct Cyc_Absyn_Uniondecl*
_temp720; struct Cyc_Absyn_Tuniondecl* _temp722; struct Cyc_Absyn_Enumdecl*
_temp724; struct Cyc_List_List* _temp726; struct _tagged_arr* _temp728; struct
Cyc_List_List* _temp730; struct _tuple0* _temp732; struct _tuple0 _temp734;
struct _tagged_arr* _temp735; void* _temp737; struct Cyc_List_List* _temp739;
_LL676: if(*(( int*) _temp674) == Cyc_Absyn_Let_d){ _LL709: _temp708=(( struct
Cyc_Absyn_Let_d_struct*) _temp674)->f1; goto _LL707; _LL707: _temp706=(( struct
Cyc_Absyn_Let_d_struct*) _temp674)->f2; goto _LL705; _LL705: _temp704=(( struct
Cyc_Absyn_Let_d_struct*) _temp674)->f3; goto _LL703; _LL703: _temp702=(( struct
Cyc_Absyn_Let_d_struct*) _temp674)->f4; goto _LL701; _LL701: _temp700=(( struct
Cyc_Absyn_Let_d_struct*) _temp674)->f5; goto _LL677;} else{ goto _LL678;} _LL678:
if(*(( int*) _temp674) == Cyc_Absyn_Letv_d){ _LL711: _temp710=(( struct Cyc_Absyn_Letv_d_struct*)
_temp674)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp674)
== Cyc_Absyn_Var_d){ _LL713: _temp712=(( struct Cyc_Absyn_Var_d_struct*)
_temp674)->f1; goto _LL681;} else{ goto _LL682;} _LL682: if(*(( int*) _temp674)
== Cyc_Absyn_Fn_d){ _LL715: _temp714=(( struct Cyc_Absyn_Fn_d_struct*) _temp674)->f1;
goto _LL683;} else{ goto _LL684;} _LL684: if(*(( int*) _temp674) == Cyc_Absyn_Typedef_d){
_LL717: _temp716=(( struct Cyc_Absyn_Typedef_d_struct*) _temp674)->f1; goto
_LL685;} else{ goto _LL686;} _LL686: if(*(( int*) _temp674) == Cyc_Absyn_Struct_d){
_LL719: _temp718=(( struct Cyc_Absyn_Struct_d_struct*) _temp674)->f1; goto
_LL687;} else{ goto _LL688;} _LL688: if(*(( int*) _temp674) == Cyc_Absyn_Union_d){
_LL721: _temp720=(( struct Cyc_Absyn_Union_d_struct*) _temp674)->f1; goto _LL689;}
else{ goto _LL690;} _LL690: if(*(( int*) _temp674) == Cyc_Absyn_Tunion_d){
_LL723: _temp722=(( struct Cyc_Absyn_Tunion_d_struct*) _temp674)->f1; goto
_LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp674) == Cyc_Absyn_Enum_d){
_LL725: _temp724=(( struct Cyc_Absyn_Enum_d_struct*) _temp674)->f1; goto _LL693;}
else{ goto _LL694;} _LL694: if(*(( int*) _temp674) == Cyc_Absyn_Namespace_d){
_LL729: _temp728=(( struct Cyc_Absyn_Namespace_d_struct*) _temp674)->f1; goto
_LL727; _LL727: _temp726=(( struct Cyc_Absyn_Namespace_d_struct*) _temp674)->f2;
goto _LL695;} else{ goto _LL696;} _LL696: if(*(( int*) _temp674) == Cyc_Absyn_Using_d){
_LL733: _temp732=(( struct Cyc_Absyn_Using_d_struct*) _temp674)->f1; _temp734=*
_temp732; _LL738: _temp737= _temp734.f1; goto _LL736; _LL736: _temp735= _temp734.f2;
goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Using_d_struct*) _temp674)->f2;
goto _LL697;} else{ goto _LL698;} _LL698: if(*(( int*) _temp674) == Cyc_Absyn_ExternC_d){
_LL740: _temp739=(( struct Cyc_Absyn_ExternC_d_struct*) _temp674)->f1; goto
_LL699;} else{ goto _LL675;} _LL677: Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u)); goto _LL675; _LL679: Cyc_Tcutil_terr( loc,
_tag_arr("top level let-declarations are not implemented", sizeof( unsigned char),
47u)); goto _LL675; _LL681: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp712->sc): 0){( void*)( _temp712->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcVardecl( te, ge, loc, _temp712, check_var_init); goto _LL675; _LL683:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp714->sc): 0){( void*)(
_temp714->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcFndecl( te, ge,
loc, _temp714); goto _LL675; _LL685: Cyc_Tc_tcTypedefdecl( te, ge, loc, _temp716);
goto _LL675; _LL687: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp718->sc): 0){( void*)( _temp718->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp718); goto _LL675; _LL689: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp720->sc): 0){( void*)( _temp720->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp720);
goto _LL675; _LL691: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp722->sc): 0){( void*)( _temp722->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp722); goto _LL675; _LL693: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp724->sc): 0){( void*)( _temp724->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp724);
goto _LL675; _LL695: { struct Cyc_List_List* _temp741= te->ns; struct Cyc_List_List*
_temp742=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp741,({ struct Cyc_List_List* _temp743=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp743->hd=( void*) _temp728;
_temp743->tl= 0; _temp743;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp728)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp728); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp742, Cyc_Tcenv_empty_genv());} te->ns= _temp742; Cyc_Tc_tc_decls( te,
_temp726, in_externC, check_var_init); te->ns= _temp741; goto _LL675;} _LL697: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp744= _temp737;
struct Cyc_List_List* _temp756; struct Cyc_List_List* _temp758; struct Cyc_List_List*
_temp760; struct Cyc_List_List _temp762; struct Cyc_List_List* _temp763; struct
_tagged_arr* _temp765; struct Cyc_List_List* _temp767; struct Cyc_List_List
_temp769; struct Cyc_List_List* _temp770; struct _tagged_arr* _temp772; _LL746:
if( _temp744 ==( void*) Cyc_Absyn_Loc_n){ goto _LL747;} else{ goto _LL748;}
_LL748: if(( unsigned int) _temp744 > 1u?*(( int*) _temp744) == Cyc_Absyn_Rel_n:
0){ _LL757: _temp756=(( struct Cyc_Absyn_Rel_n_struct*) _temp744)->f1; if(
_temp756 == 0){ goto _LL749;} else{ goto _LL750;}} else{ goto _LL750;} _LL750:
if(( unsigned int) _temp744 > 1u?*(( int*) _temp744) == Cyc_Absyn_Abs_n: 0){
_LL759: _temp758=(( struct Cyc_Absyn_Abs_n_struct*) _temp744)->f1; if( _temp758
== 0){ goto _LL751;} else{ goto _LL752;}} else{ goto _LL752;} _LL752: if((
unsigned int) _temp744 > 1u?*(( int*) _temp744) == Cyc_Absyn_Rel_n: 0){ _LL761:
_temp760=(( struct Cyc_Absyn_Rel_n_struct*) _temp744)->f1; if( _temp760 == 0){
goto _LL754;} else{ _temp762=* _temp760; _LL766: _temp765=( struct _tagged_arr*)
_temp762.hd; goto _LL764; _LL764: _temp763= _temp762.tl; goto _LL753;}} else{
goto _LL754;} _LL754: if(( unsigned int) _temp744 > 1u?*(( int*) _temp744) ==
Cyc_Absyn_Abs_n: 0){ _LL768: _temp767=(( struct Cyc_Absyn_Abs_n_struct*)
_temp744)->f1; if( _temp767 == 0){ goto _LL745;} else{ _temp769=* _temp767;
_LL773: _temp772=( struct _tagged_arr*) _temp769.hd; goto _LL771; _LL771:
_temp770= _temp769.tl; goto _LL755;}} else{ goto _LL745;} _LL747: goto _LL749;
_LL749: goto _LL751; _LL751: first= _temp735; rest= 0; goto _LL745; _LL753:
_temp772= _temp765; _temp770= _temp763; goto _LL755; _LL755: first= _temp772;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp770,({ struct Cyc_List_List* _temp774=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp774->hd=( void*) _temp735;
_temp774->tl= 0; _temp774;})); goto _LL745; _LL745:;}{ struct Cyc_List_List*
_temp775= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp776=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp776->hd=( void*) _temp775; _temp776->tl= ge->availables;
_temp776;}); Cyc_Tc_tc_decls( te, _temp730, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL675;}} _LL699:
Cyc_Tc_tc_decls( te, _temp739, 1, check_var_init); goto _LL675; _LL675:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp777=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp787; struct Cyc_List_List*
_temp789; struct Cyc_List_List** _temp791; struct _tuple0* _temp792; struct Cyc_List_List*
_temp794; struct Cyc_List_List** _temp796; struct _tagged_arr* _temp797; _LL779:
if(*(( int*) _temp777) == Cyc_Absyn_Var_d){ _LL788: _temp787=(( struct Cyc_Absyn_Var_d_struct*)
_temp777)->f1; goto _LL780;} else{ goto _LL781;} _LL781: if(*(( int*) _temp777)
== Cyc_Absyn_Using_d){ _LL793: _temp792=(( struct Cyc_Absyn_Using_d_struct*)
_temp777)->f1; goto _LL790; _LL790: _temp789=(( struct Cyc_Absyn_Using_d_struct*)
_temp777)->f2; _temp791=&(( struct Cyc_Absyn_Using_d_struct*) _temp777)->f2;
goto _LL782;} else{ goto _LL783;} _LL783: if(*(( int*) _temp777) == Cyc_Absyn_Namespace_d){
_LL798: _temp797=(( struct Cyc_Absyn_Namespace_d_struct*) _temp777)->f1; goto
_LL795; _LL795: _temp794=(( struct Cyc_Absyn_Namespace_d_struct*) _temp777)->f2;
_temp796=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp777)->f2; goto _LL784;}
else{ goto _LL785;} _LL785: goto _LL786; _LL780: if(( void*) _temp787->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp801; struct
_tagged_arr* _temp802; void* _temp804; struct _tuple0* _temp799= _temp787->name;
_temp801=* _temp799; _LL805: _temp804= _temp801.f1; goto _LL803; _LL803:
_temp802= _temp801.f2; goto _LL800; _LL800: { struct Cyc_List_List* ns;{ void*
_temp806= _temp804; struct Cyc_List_List* _temp814; struct Cyc_List_List*
_temp816; _LL808: if( _temp806 ==( void*) Cyc_Absyn_Loc_n){ goto _LL809;} else{
goto _LL810;} _LL810: if(( unsigned int) _temp806 > 1u?*(( int*) _temp806) ==
Cyc_Absyn_Rel_n: 0){ _LL815: _temp814=(( struct Cyc_Absyn_Rel_n_struct*)
_temp806)->f1; goto _LL811;} else{ goto _LL812;} _LL812: if(( unsigned int)
_temp806 > 1u?*(( int*) _temp806) == Cyc_Absyn_Abs_n: 0){ _LL817: _temp816=((
struct Cyc_Absyn_Abs_n_struct*) _temp806)->f1; goto _LL813;} else{ goto _LL807;}
_LL809: ns= 0; goto _LL807; _LL811: ns= _temp814; goto _LL807; _LL813: ns=
_temp816; goto _LL807; _LL807:;}{ struct _tuple6* _temp818=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp819=(* _temp818).f1; int _temp820=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp819->ordinaries, _temp802)).f2; if( ! _temp820){(* _temp818).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp818).f2, _temp802);} return _temp820;}}} _LL782: _temp796= _temp791; goto
_LL784; _LL784:* _temp796= Cyc_Tc_treeshake_f( env,* _temp796); return 1; _LL786:
return 1; _LL778:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp821=(* nsenv).f1; struct Cyc_Set_Set* _temp822=(* nsenv).f2; _temp821->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(
Cyc_Tc_treeshake_remove_f, _temp822, _temp821->ordinaries);} static struct
_tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp823=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp823->f1= ge; _temp823->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp823;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp824=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* _temp825= Cyc_Tc_treeshake_f(
_temp824, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp824);
return _temp825;}